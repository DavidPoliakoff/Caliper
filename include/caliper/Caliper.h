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

/// \file Caliper.h 
/// Initialization function and global data declaration

#pragma once

#include "cali_definitions.h"

#include "common/Attribute.h"
#include "common/CaliperMetadataAccessInterface.h"
#include "common/Entry.h"
#include "common/Record.h"
#include "common/Variant.h"
#include "common/util/callback.hpp"

#include <utility>

namespace cali
{

// Forward declarations

class CaliperService;
class Node;    
class SnapshotRecord;
    
/// \class Caliper
/// \brief The main interface for the caliper runtime system

class Caliper : public CaliperMetadataAccessInterface
{
public:

    struct Scope;

    typedef Scope* (*ScopeCallbackFn)(Caliper*, bool can_create);

    
//TODO DO-NOT-MERGE REENABLE DATA PROTECTIONprivate:
    
    struct GlobalData;
    
    GlobalData* mG;
    static GlobalData* createGlobalData();
    template<class... Args>
    static GlobalData* globalDataForTag(){
      static GlobalData* data;
      if(!data){
        data = createGlobalData();
      }
      return data;
    }
    static Caliper create_caliper_instance(GlobalData* sG);
    static Caliper create_caliper_sigsafe_instance(GlobalData* sG);
    template<class... Args>
    static Caliper CaliperForTags(){
      GlobalData* dataForTags = globalDataForTag<Args...>();
      return create_caliper_instance(dataForTags);
    }
private: //TODO DO-NOT-MERGE DELET THIS, USE OTHER PRIVATE TAG ABOVE
    Scope* m_thread_scope;
    Scope* m_task_scope;

    bool   m_is_signal; // are we in a signal handler?

    
    Caliper(GlobalData* g, Scope* thread = 0, Scope* task = 0, bool sig = false)
        : mG(g), m_thread_scope(thread), m_task_scope(task), m_is_signal(sig)
        { }

    Scope* scope(cali_context_scope_t scope);
    

public:
    
    Caliper();
    
    ~Caliper()
        { }

    Caliper(const Caliper&) = default;

    Caliper& operator = (const Caliper&) = default;

    bool is_signal() const { return m_is_signal; };

    // --- Typedefs

    typedef std::function<bool(const SnapshotRecord*)> SnapshotFlushFn;
    
    // --- Events

    struct Events {
        typedef util::callback<void(Caliper*,const Attribute&)>
            create_attr_cbvec;
        typedef util::callback<void(Caliper*,const std::string&,cali_attr_type,int*,Node**)>
            pre_create_attr_cbvec;                        
        typedef util::callback<void(Caliper*,const Attribute&,const Variant&)>
            update_cbvec;
        typedef util::callback<void(Caliper*)>
            caliper_cbvec;
        typedef util::callback<void(Caliper*,cali_context_scope_t)>
            scope_cbvec;

        typedef util::callback<void(Caliper*,int,const SnapshotRecord*,SnapshotRecord*)>
            snapshot_cbvec;
        typedef util::callback<void(Caliper*,const SnapshotRecord*,const SnapshotRecord*)>
            process_snapshot_cbvec;
        typedef util::callback<void(Caliper*,SnapshotRecord*)>
            edit_snapshot_cbvec;

        typedef util::callback<void(Caliper*,const SnapshotRecord*,SnapshotFlushFn)>
            flush_cbvec;
        typedef util::callback<void(Caliper*,const SnapshotRecord*)>
            write_cbvec;
                                    
        pre_create_attr_cbvec  pre_create_attr_evt;
        create_attr_cbvec      create_attr_evt;

        update_cbvec           pre_begin_evt;
        update_cbvec           post_begin_evt;
        update_cbvec           pre_set_evt;
        update_cbvec           post_set_evt;
        update_cbvec           pre_end_evt;
        update_cbvec           post_end_evt;

        scope_cbvec            create_scope_evt;
        scope_cbvec            release_scope_evt;

        caliper_cbvec          post_init_evt;
        caliper_cbvec          finish_evt;

        snapshot_cbvec         snapshot;
        process_snapshot_cbvec process_snapshot;

        write_cbvec            pre_flush_evt;
        flush_cbvec            flush_evt;

        edit_snapshot_cbvec    postprocess_snapshot;

        write_cbvec            pre_write_evt;
        process_snapshot_cbvec write_snapshot;
        write_cbvec            post_write_evt;
    };

    Events&   events();

    // --- Context environment API

    Scope*    create_scope(cali_context_scope_t context);
    Scope*    default_scope(cali_context_scope_t context);

    void      release_scope(Scope*);

    void      set_scope_callback(cali_context_scope_t context, ScopeCallbackFn cb);

    // --- Snapshot API

    /// \name Snapshot API
    /// \{

    void      push_snapshot(int scopes, const SnapshotRecord* trigger_info);
    void      pull_snapshot(int scopes, const SnapshotRecord* trigger_info, SnapshotRecord* snapshot);

    // --- Flush and I/O API

    /// \}
    /// \name Flush and I/O
    /// \{

    void      flush(const SnapshotRecord* flush_info, SnapshotFlushFn proc_fn);
    void      flush_and_write(const SnapshotRecord* flush_info);

    // --- Annotation API

    /// \}
    /// \name Annotation API
    /// \{

    cali_err  begin(const Attribute& attr, const Variant& data);
    cali_err  end(const Attribute& attr);
    cali_err  set(const Attribute& attr, const Variant& data);
    cali_err  set_path(const Attribute& attr, size_t n, const Variant data[]);

    /// \}
    /// \name Blackboard access
    /// \{

    Variant   exchange(const Attribute& attr, const Variant& data);

    Entry     get(const Attribute& attr);

    // --- Direct metadata / data access API

    /// \}
    /// \name Explicit snapshot record manipulation
    /// \{

    void      make_entrylist(size_t n, 
                             const Attribute  attr[], 
                             const Variant    data[], 
                             SnapshotRecord&  list);
    void      make_entrylist(const Attribute& attr, 
                             size_t           n,
                             const Variant    data[], 
                             SnapshotRecord&  list);

    // --- Metadata Access Interface

    /// \}
    /// \name Context tree manipulation and metadata access
    /// \{

    size_t    num_attributes() const;

    Attribute get_attribute(cali_id_t id) const;
    Attribute get_attribute(const std::string& name) const;

    std::vector<Attribute> get_attributes() const;

    Attribute create_attribute(const std::string& name,
                               cali_attr_type     type,
                               int                prop = CALI_ATTR_DEFAULT,
                               int                meta = 0,
                               const Attribute*   meta_attr = nullptr,
                               const Variant*     meta_data = nullptr);
    
    /// \brief Return node by id
    Node*     node(cali_id_t id) const;

    /// \brief Get or create tree path with data from given nodes in given order 
    Node*     make_tree_entry(size_t n, const Node* nodelist[], Node* parent = nullptr);

    /// \brief Get or create tree entry with given attribute/value pair
    Node*     make_tree_entry(const Attribute& attr, const Variant& value, Node* parent = nullptr);

    /// \}

    // --- Caliper API access

    operator bool () const {
        return mG != 0;
    }

    static Caliper instance();
    static void    release();
    
    static Caliper sigsafe_instance();

    static bool    is_initialized();

    /// \brief Add a list of available caliper services.
    static void    add_services(const CaliperService*);

    /// \brief Add a function that is called during %Caliper initialization.
    static void    add_init_hook(void(*fn)());

    friend struct GlobalData;
    //template<class... Args>
    //static Caliper instance();
    //template<class... Args>
    //static void    release();
    //
    //template<class... Args>
    //static Caliper sigsafe_instance();

    //template<class... Args>
    //static bool    is_initialized();

    ///// \brief Add a list of available caliper services.
    //template<class... Args>
    //static void    add_services(const CaliperService*);

    ///// \brief Add a function that is called during %Caliper initialization.
    //template<class... Args>
    //static void    add_init_hook(void(*fn)());
///****************/
//template<class... Args>
//Caliper
//instance()
//{
//    if (GlobalData::s_init_lock != 0) {
//        if (GlobalData::s_init_lock == 2)
//            // Caliper had been initialized previously; we're past the static destructor
//            return Caliper(0);
//
//        lock_guard<mutex> lock(GlobalData::s_init_mutex);
//
//        if (!GlobalData::sG) {
//            if (atexit(::exit_handler) != 0)
//                Log(0).stream() << "Unable to register exit handler";
//
//            GlobalData::sG = new Caliper::GlobalData;
//            GlobalData::s_init_lock = 0;
//        }
//    }
//
//    return Caliper(GlobalData::sG, GlobalData::sG->acquire_thread_scope());
//}
//
///// \brief Construct a signal-safe Caliper instance object.
/////
///// A signal-safe Caliper instance object will have a flag set to instruct
///// the API and services that only signal-safe operations can be used.
/////
///// \see instance()
/////
///// \return Caliper instance object
//
//template<class... Args>
//Caliper
//sigsafe_instance()
//{
//    if (GlobalData::s_init_lock != 0)
//        return Caliper(0);
//
//    Scope* task_scope   = 0; // FIXME: figure out task scope
//    Scope* thread_scope = GlobalData::sG->acquire_thread_scope(false);
//
//    if (!thread_scope || thread_scope->lock.is_locked())
//        return Caliper(0);
//
//    return Caliper(GlobalData::sG, thread_scope, task_scope, true /* is signal */);
//}
//
//template<class... Args>
//void
//release()
//{
//    delete GlobalData::sG;
//    GlobalData::sG = 0;
//}
//
///// \brief Test if Caliper has been initialized yet.
//
//template<class... Args>
//bool
//is_initialized()
//{
//    return GlobalData::sG != nullptr;
//}
//
///// \brief Add a list of available caliper services.
///// Adds services that will be made available by %Caliper. This does *not*
///// activate the services automatically, they must still be listed in the
///// CALI_SERVICES_ENABLE configuration variable at runtime.
///// Services must be provided in a list of CaliperService entries,
///// terminated by a `{ nullptr, nullptr}` entry. Example:
/////
///// \code
/////   extern void myservice_register(Caliper* c);
/////
/////   CaliperService my_services[] = {
/////     { "myservice", myservice_register },
/////     { nullptr, nullptr }
/////   };
/////
/////   Caliper::add_services(my_services);
///// \endcode
/////
///// This is only effective _before_ %Caliper is initialized, and should be
///// called e.g. during static initialization or from a library constructor.
//
//template<class... Args>
//void
//add_services(const CaliperService* s)
//{
//    if (is_initialized())
//        Log(0).stream() << "add_services(): Caliper is already initialized - cannot add new services" << std::endl;
//    else    
//        Services::add_services(s);
//}
//
//template<class... Args>
//void
//add_init_hook(void (*hook)())
//{
//    if (is_initialized())
//        Log(0).stream() << "add_init_hook(): Caliper is already initialized - cannot add init hook" << std::endl;
//    else
//        GlobalData::add_init_hook(hook);
//}    //friend struct GlobalData;
};

} // namespace cali
