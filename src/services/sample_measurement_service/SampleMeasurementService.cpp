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

#include "caliper/CaliperService.h"

#include "caliper/Caliper.h"
#include "caliper/SnapshotRecord.h"

#include "caliper/common/RuntimeConfig.h"
#include "caliper/common/ContextRecord.h"
#include "caliper/common/Log.h"
#include "caliper/AbstractMeasurementService.hpp"
#include <cassert>
#include <chrono>
#include <map>
#include <mutex>
#include <string>
#include <type_traits>
#include <vector>

using namespace cali;
using namespace std;


/** For debug purposes */
struct DoesImmediateTest : public MeasurementPattern<DoesImmediateTest>{
  const char* service_tag(){
    return "sample_phase_service";
  }
  DoesImmediateTest() : MeasurementPattern<DoesImmediateTest>(){}
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
  static void timestamp_register(Caliper* c, Experiment* exp) {
      DoesImmediate* instance = new DoesImmediate();
        Attribute doggo_attr =
            c->create_attribute("DOGGOOOOO", CALI_TYPE_UINT,
                                CALI_ATTR_ASVALUE     |
                                CALI_ATTR_SKIP_EVENTS |
                                CALI_ATTR_SCOPE_THREAD); // FIXME: needs to be original attribute's scope
      instance->setInclusiveAttributes({doggo_attr});
    }

};

void instantiator(){
  DoesImmediate doz;
  doz.snapshot_cb(NULL,NULL,0,NULL,NULL);
}

namespace cali
{

CaliperService timestamp_service = { "samplemeasurement", ::DoesImmediate::make_service };

} // namespace cali

