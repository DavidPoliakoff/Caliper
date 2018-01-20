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

/// @file Services.cpp
/// Services class implementation

#include "caliper/caliper-config.h"

#include "Services.h"

#include "caliper/CaliperService.h"

#include "caliper/common/Log.h"
#include "caliper/common/RuntimeConfig.h"

#include <algorithm>
#include <string>
#include <sstream>
#include <vector>

using namespace cali;
using namespace std;

// List of services, defined in services.inc.cpp
#include "services.inc.cpp"

namespace
{

struct ServicesList {
    const CaliperService* services;
    ServicesList* next;
};


}


namespace cali
{
const ConfigSet::Entry             configdata[] = {
    // key, type, value, short description, long description
    { "enable", CALI_TYPE_STRING, "",
      "List of service modules to enable",
      "A list of comma-separated names of the service modules to enable"      
    },
    ConfigSet::Terminator
};

struct Services::ServicesImpl
{
    // --- data

    //std::unique_ptr<ServicesImpl> s_instance;

    const ConfigSet::Entry*        s_configdata;
    ServicesList* s_services_list;

    ConfigSet m_config;


    // --- interface

    void register_services(Caliper* c) {
        // list services

        if (Log::verbosity() >= 2) {
            ostringstream sstr;

            for (const ServicesList* lp = s_services_list; lp; lp = lp->next)
                for (const CaliperService* s = lp->services; s->name && s->register_fn; ++s)
                    sstr << ' ' << s->name;

            Log(2).stream() << "Available services:" << sstr.str() << endl;
        }

        vector<string> services = m_config.get("enable").to_stringlist(",:");

        // register caliper services

        for (const ServicesList* lp = s_services_list; lp; lp = lp->next)
            for (const CaliperService* s = lp->services; s->name && s->register_fn; ++s) {
                auto it = find(services.begin(), services.end(), string(s->name));

                if (it != services.end()) {
                    (*s->register_fn)(c);
                    services.erase(it);
                }
            }

        for ( const string& s : services )
            Log(0).stream() << "Warning: service \"" << s << "\" not found" << endl;
    }

    ServicesImpl()
        : s_configdata{configdata},
          m_config { RuntimeConfig::init("services", s_configdata) },
          //s_services_list {new ServicesList({nullptr})}
          s_services_list {nullptr}
        { }

};

//unique_ptr<Services::ServicesImpl> Services::ServicesImpl::s_instance      { nullptr };


} // namespace cali


//
// --- Services public interface
//

void Services::add_services(const CaliperService* services)
{
    ServicesList* elem = new ServicesList { services, mP->s_services_list };
    mP->s_services_list = elem;
}

void Services::add_default_services()
{
    add_services(caliper_services);
}

void Services::register_services(Caliper* c)
{
    return mP->register_services(c);
}

Services::Services(){
  mP = new ServicesImpl();
}
