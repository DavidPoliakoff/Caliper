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

///\file  PthreadService.cpp
///\brief Service for pthreads-based threading runtimes

#include "../CaliperService.h"

#include "caliper/Caliper.h"

#include "caliper/common/Log.h"

#include <cstdlib>
#include <pthread.h>

#include <gotcha/gotcha.h>

using namespace cali;

namespace
{

int (*orig_pthread_create)(pthread_t*, const pthread_attr_t*, void*(*)(void*), void*) = NULL;

Attribute id_attr = Attribute::invalid;

struct wrapper_args {
    void* (*fn)(void*);
    void* arg;
};

// Wrapper for the user-provided thread start function.
// We wrap the original thread start function to create Caliper thread scope
// on the new child thread.
void*
thread_wrapper(void *arg)
{
    uint64_t id = static_cast<uint64_t>(pthread_self());

    Caliper::instance().set(id_attr, Variant(CALI_TYPE_UINT, &id, sizeof(id)));

    wrapper_args* wrap = static_cast<wrapper_args*>(arg);
    void* ret = (*(wrap->fn))(wrap->arg);

    delete wrap;
    return ret;
}

// Wrapper for pthread_create()
int
cali_pthread_create_wrapper(pthread_t *thread, const pthread_attr_t *attr,
                            void *(*fn)(void*), void* arg)
{
    return (*orig_pthread_create)(thread, attr, thread_wrapper, new wrapper_args({ fn, arg }));
}

// Initialization routine.
void 
pthreadservice_initialize(Caliper* c)
{
    id_attr = c->create_attribute("pthread.id", CALI_TYPE_UINT, CALI_ATTR_DEFAULT);

    struct gotcha_binding_t pthread_binding[] = { 
        { "pthread_create", (void*) cali_pthread_create_wrapper, &orig_pthread_create }
    };

    gotcha_wrap(pthread_binding, sizeof(pthread_binding)/sizeof(struct gotcha_binding_t), "Caliper");

    Log(1).stream() << "Registered pthread service" << std::endl;
}

} // namespace [anonymous]

namespace cali
{
    CaliperService pthread_service { "pthread", ::pthreadservice_initialize };
}
