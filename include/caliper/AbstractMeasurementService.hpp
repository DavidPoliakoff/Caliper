// Copyright (c) 2015, Lawrence Livermore National Security, LLC.  
// Produced at the Lawrence Livermore National Laboratory.
//
// This file is part of Caliper.
// Written by David Boehme, boehme3@llnl.gov.
// LLNL-CODE-678900
// All rights reserved.
//
// For details, see https://github.com/scalability-llnl/Caliper.
// Please also see the LICENSE file for our additional BSD notice.
//
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//
//  * Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the disclaimer below.
//  * Redistributions in binary form must reproduce the above copyright notice, this list of
//    conditions and the disclaimer (as noted below) in the documentation and/or other materials
//    provided with the distribution.
//  * Neither the name of the LLNS/LLNL nor the names of its contributors may be used to endorse
//    or promote products derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
// OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// LAWRENCE LIVERMORE NATIONAL SECURITY, LLC, THE U.S. DEPARTMENT OF ENERGY OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
// ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Timestamp.cpp
// Timestamp provider for caliper records
#pragma once
#include "caliper/CaliperService.h"

#include "caliper/Caliper.h"
#include "caliper/SnapshotRecord.h"

#include "caliper/common/RuntimeConfig.h"
#include "caliper/common/ContextRecord.h"
#include "caliper/common/Log.h"

#include <cassert>
#include <chrono>
#include <map>
#include <mutex>
#include <string>
#include <type_traits>
#include <vector>

using namespace cali;
using namespace std;

namespace 
{

// TODO: ensure the number of variants returned by different measurement functions matches number of attributes included
template<typename MeasurementImplementation>
struct MeasurementPattern {
    
  MeasurementPattern(){
  }
    bool is_this_our_stack_attribute(cali_id_t id){
      /** TODO: implement */
      return true;
    }
    Attribute begin_evt_attr { Attribute::invalid };
    Attribute set_evt_attr   { Attribute::invalid };
    Attribute end_evt_attr   { Attribute::invalid };
    Attribute lvl_attr       { Attribute::invalid };
    template<typename ItemType>
    using ArrayType = std::vector<ItemType>;
    using AttributeId = cali_id_t;
    template<typename ItemType>
    using TbdReturnType = ArrayType<ItemType>; //&&?
    using VariantArray = ArrayType<Variant>;
    ArrayType<Attribute> m_measurement_attrs;
    ArrayType<Attribute> m_offset_attrs;
    ArrayType<Attribute> m_exclusive_attrs;
    ArrayType<Attribute> m_inclusive_attrs;
    template<typename Foo = MeasurementImplementation>
    typename std::enable_if<!std::is_same<typename std::result_of<decltype(&Foo::createPreamble)(Foo)>::type,void>::value,void>::type snapshot_cb(
        Caliper* c, Experiment* exp, int scopes, const SnapshotRecord* info,  SnapshotRecord* sbuf
        ){
        std::cout << "DOG1\n";
        auto subthis = static_cast<MeasurementImplementation*>(this);
        auto preamble = subthis->createPreamble();
        std::cout << "POSTPREAMBLE"<<std::endl; 
        auto measurements = subthis->recordMeasurements(preamble);
        //sbuf->append(m_measurement_attrs.size(), m_measurement_attrs.data(), measurements.data());
        c->make_entrylist(m_measurement_attrs.size(), m_measurement_attrs.data(), measurements.data(), *sbuf);
        ArrayType<Variant> last(m_offset_attrs.size());
        auto offs = subthis->recordBlackboardOffsets(preamble);
        
        for (size_t i = 0; i < m_offset_attrs.size(); ++i)    
            last[i] = c->exchange(exp, m_offset_attrs[i], offs[i]);

        auto excls = subthis->recordExclusiveValues(preamble, last);

        //sbuf->append(m_exclusive_attrs.size(), m_exclusive_attrs.data(), excls.data());
        c->make_entrylist(m_exclusive_attrs.size(), m_exclusive_attrs.data(), excls.data(), *sbuf);
        if (m_inclusive_attrs.size() && info) {
            //using StackThing = typeRouter<true>::type<ArrayType<decltype(preamble),Attribute>>;

            using StackThing = std::vector<decltype(subthis->handlePhaseBegin(std::declval<cali_id_t>(),preamble))>;
            static thread_local std::vector< StackThing* > exp_stacks;

            size_t expI = exp->id();

            StackThing* stack = nullptr;
            if (exp_stacks.size() > expI)
                stack = exp_stacks[expI];

            if (!stack) {
                if (exp_stacks.size() <= expI){
                    exp_stacks.resize(expI + 1);
                    stack = new StackThing;
                    exp_stacks[expI] = stack;
                }
                else{
                  stack = exp_stacks[expI]; // TODO: what if we hit the stacks in non (1,2,3,4,5) order?
                }
            }

            Entry event = info->get(begin_evt_attr);

            if (event.is_empty())
                event = info->get(end_evt_attr);
            if (event.is_empty())
                return;

            cali_id_t spawning_attr_id = event.value().to_id();

            if (!is_this_our_stack_attribute(spawning_attr_id))
                return;

            if (event.attribute() == begin_evt_attr.id()) {
                stack->push_back( subthis->handlePhaseBegin(spawning_attr_id, preamble) );
            } else if (event.attribute() == end_evt_attr.id()) {
                
                auto stack_top = stack->back();
                auto ret = subthis->handlePhaseEnd(spawning_attr_id, stack_top);
                //sbuf->append(m_inclusive_attrs.size(), m_inclusive_attrs.data(), ret.data());
                c->make_entrylist(m_inclusive_attrs.size(), m_inclusive_attrs.data(), ret.data(),*sbuf); 
                stack->pop_back();
            }
        }
    }
    
    
    template<typename Foo = MeasurementImplementation>
    typename std::enable_if<std::is_same<
                              typename std::result_of<decltype(&Foo::createPreamble)(Foo)>::type,
                              void
                           >::value,void>::type snapshot_cb(
        Caliper* c, Experiment* exp, int scopes, const SnapshotRecord* info,  SnapshotRecord* sbuf
        ){
        auto subthis = static_cast<MeasurementImplementation*>(this);
      

        auto measurements = subthis->recordMeasurements();
        c->make_entrylist(m_measurement_attrs.size(), m_measurement_attrs.data(), measurements.data(),*sbuf);

        ArrayType<Variant> last(m_offset_attrs.size());
        auto offs = subthis->recordBlackboardOffsets();
        
        for (size_t i = 0; i < m_offset_attrs.size(); ++i)    
            last[i] = c->exchange(exp, m_offset_attrs[i], offs[i]);

        auto excls = subthis->recordExclusiveValues(last);

        c->make_entrylist(m_exclusive_attrs.size(), m_exclusive_attrs.data(), excls.data(),*sbuf);

        if (m_inclusive_attrs.size() && info) {
            
            using StackThing = std::vector<decltype(subthis->handlePhaseBegin(std::declval<cali_id_t>()))>;
            static thread_local std::vector< StackThing* > exp_stacks;

            size_t expI = exp->id();

            StackThing* stack = nullptr;

            if (exp_stacks.size() > expI)
                stack = exp_stacks[expI];

            if (!stack) {
                if (exp_stacks.size() <= expI)
                    exp_stacks.resize(expI + 1);
                
                stack = new StackThing;

                exp_stacks[expI] = stack;
            }

            Entry event = info->get(begin_evt_attr);

            if (event.is_empty())
                event = info->get(end_evt_attr);
            if (event.is_empty())
                return;

            cali_id_t spawning_attr_id = event.value().to_id();

            if (!is_this_our_stack_attribute(spawning_attr_id))
                return;

            if (event.attribute() == begin_evt_attr.id()) {
                auto ret = subthis->handlePhaseBegin(spawning_attr_id);
                stack->push_back(ret);
            } else if (event.attribute() == end_evt_attr.id()) {
                auto last = stack->back();
                auto ret = subthis->handlePhaseEnd(spawning_attr_id, last);
                c->make_entrylist(m_inclusive_attrs.size(), m_inclusive_attrs.data(), ret.data(),*sbuf);
                stack->pop_back();
            }
        }
    }
    template<typename Foo=MeasurementImplementation> 
    using preambleCallType = typename std::result_of<decltype(&Foo::createPreamble)(Foo)>::type;
    void createMeasurementAttributes()
        /** TODO: Implement */{
        
    }
    void setMeasurementAttributes(ArrayType<Attribute> measurement_attributes){
      m_measurement_attrs = measurement_attributes;
    }
    void setExclusiveAttributes(ArrayType<Attribute> measurement_attributes){
      m_exclusive_attrs = measurement_attributes;
    }
    void setInclusiveAttributes(ArrayType<Attribute> measurement_attributes){
      m_inclusive_attrs = measurement_attributes;
    }
    template<typename Foo = MeasurementImplementation>
    typename std::enable_if<!std::is_same<preambleCallType<Foo>,void>::value,TbdReturnType<Variant>>::type    
    //typename std::enable_if<!std::is_same<typename std::result_of<decltype(&Foo::createPreamble)(Foo)>::type,void>::value,TbdReturnType<Variant>>::type    
    handlePhaseBegin(AttributeId tbd_spawning_event_id, typename std::result_of<decltype(&Foo::createPreamble)(Foo)>::type){
    }
    template<typename Foo = MeasurementImplementation>
    typename std::enable_if<!std::is_same<preambleCallType<Foo>,void>::value,TbdReturnType<Variant>>::type    
    handlePhaseEnd(AttributeId tbd_spawning_event_id,typename std::result_of<decltype(&Foo::template handlePhaseBegin<Foo>)(Foo,AttributeId,preambleCallType<Foo>)>::type){
    }
    template<typename Foo = MeasurementImplementation>
    typename std::enable_if<!std::is_same<preambleCallType<Foo>,void>::value,TbdReturnType<Variant>>::type    
    handlePhaseEnd(AttributeId tbd_spawning_event_id,typename std::result_of<decltype(&Foo::handlePhaseBegin)(Foo,AttributeId,preambleCallType<Foo>)>::type){
    }
    //template<typename Foo = MeasurementImplementation>
    //typename std::enable_if<!std::is_same<typename std::result_of<decltype(&Foo::createPreamble)(Foo)>::type,void>::value,TbdReturnType<Variant>>::type    
    //handlePhaseEnd(AttributeId tbd_spawning_event_id,typename std::result_of<decltype(&Foo::createPreamble)(Foo)>::type, ArrayType<Variant>& begin_vals){
    //}
    template<typename Foo = MeasurementImplementation>
    typename std::enable_if<!std::is_same<preambleCallType<Foo>,void>::value,TbdReturnType<Variant>>::type    
    recordMeasurements(typename std::result_of<decltype(&Foo::createPreamble)(Foo)>::type){
      return TbdReturnType<Variant>();
    }
    template<typename Foo = MeasurementImplementation>
    typename std::enable_if<!std::is_same<preambleCallType<Foo>,void>::value,TbdReturnType<Variant>>::type    
    recordBlackboardOffsets(typename std::result_of<decltype(&Foo::createPreamble)(Foo)>::type){
      return TbdReturnType<Variant>();
    }
    template<typename Foo = MeasurementImplementation>
    typename std::enable_if<!std::is_same<preambleCallType<Foo>,void>::value,TbdReturnType<Variant>>::type    
    recordExclusiveValues(typename std::result_of<decltype(&Foo::createPreamble)(Foo)>::type preamble, ArrayType<Variant> last){
      return TbdReturnType<Variant>();
    }

    /// void typed

    template<typename Foo = MeasurementImplementation>
    typename std::enable_if<std::is_same<preambleCallType<Foo>,void>::value,TbdReturnType<Variant>>::type    
    handlePhaseBegin(AttributeId tbd_spawning_event_id){
    }
    template<typename Foo = MeasurementImplementation>
    typename std::enable_if<std::is_same<preambleCallType<Foo>,void>::value,TbdReturnType<Variant>>::type    
    handlePhaseEnd(AttributeId tbd_spawning_event_id, typename std::result_of<decltype(&Foo::template handlePhaseBegin<Foo>)(Foo,AttributeId)>::type
){
    }
    template<typename Foo = MeasurementImplementation>
    typename std::enable_if<std::is_same<preambleCallType<Foo>,void>::value,TbdReturnType<Variant>>::type    
    handlePhaseEnd(AttributeId tbd_spawning_event_id, typename std::result_of<decltype(&Foo::handlePhaseBegin)(Foo,AttributeId)>::type
){
    }
    // DBG
    
    //
    template<typename Foo = MeasurementImplementation>
    typename std::enable_if<std::is_same<preambleCallType<Foo>,void>::value,TbdReturnType<Variant>>::type    
    recordMeasurements(){
    }
    template<typename Foo = MeasurementImplementation>
    typename std::enable_if<std::is_same<preambleCallType<Foo>,void>::value,TbdReturnType<Variant>>::type    
    recordBlackboardOffsets(){
    }

    template<typename Foo = MeasurementImplementation>
    typename std::enable_if<std::is_same<preambleCallType<Foo>,void>::value,TbdReturnType<Variant>>::type    
    recordExclusiveValues(ArrayType<Variant> last){
    }
    void createPreamble(){
      std::cout << "In base createPreamble\n";
    }
    void base_pre_initialize(Caliper* c, Experiment* exp){
      begin_evt_attr = c->get_attribute("cali.event.begin");
      set_evt_attr   = c->get_attribute("cali.event.set");
      end_evt_attr   = c->get_attribute("cali.event.end");
      lvl_attr       = c->get_attribute("cali.event.attr.level");
    }
    void base_post_initialize(Caliper* c, Experiment* exp){
    }
    static void make_service(Caliper* c, Experiment* exp) {
        MeasurementImplementation* binding = new MeasurementImplementation();
        binding->base_pre_initialize(c, exp);
        binding->initialize(c, exp);
        binding->base_post_initialize(c, exp);

        exp->events().snapshot.connect(
          [binding](Caliper* c, Experiment* exp, int scopes, const SnapshotRecord* info, SnapshotRecord* snapshot){
              binding->snapshot_cb(c, exp, scopes, info, snapshot);
          }
        );

        Log(1).stream() << "Registered " << binding->service_tag()
                        << " binding for experiment " << exp->name() << std::endl;
    }
     
};
/** For debug purposes */
struct DoesImmediate : public MeasurementPattern<DoesImmediate>{
  const char* service_tag(){
    return "sample_phase_service";
  }
  DoesImmediate() : MeasurementPattern<DoesImmediate>(){}
  int createPreamble(){
    return 4;
  }
  int handlePhaseBegin(AttributeId id, int preamble){
    std::cout << "Preamble provided "<<preamble<<"\n";
    return preamble + 1;
  }
  TbdReturnType<Variant> handlePhaseEnd(AttributeId id, int fromBegin){
    std::cout << "Begin provided "<<fromBegin<<"\n";
    return TbdReturnType<Variant>({Variant(fromBegin + 1)});
  }
  void initialize(Caliper* c, Experiment* exp) {
      Attribute doggo_attr = 
            c->create_attribute("DOGGOOOOO", CALI_TYPE_UINT,
                                CALI_ATTR_ASVALUE     |
                                CALI_ATTR_SKIP_EVENTS |
                                CALI_ATTR_SCOPE_THREAD); // FIXME: needs to be original attribute's scope
      setInclusiveAttributes({doggo_attr});
    }

};

void debug_instantiator(){
  DoesImmediate doz;
  doz.snapshot_cb(NULL,NULL,0,NULL,NULL);
}

}

