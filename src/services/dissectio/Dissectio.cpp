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

///@file  EnvironmentInfo.cpp
///@brief A Caliper service that collects various environment information

#include "../CaliperService.h"

#include "caliper/Caliper.h"

#include "caliper/common/Log.h"
#include "caliper/common/RuntimeConfig.h"
#include "caliper/common/Variant.h"

#include "caliper/common/util/split.hpp"

#include <sys/utsname.h>
#include <time.h>
#include <unistd.h>

#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#ifdef CALIPER_HAVE_SILO
#include "gotcha_all_silo.h"
#include "gotcha_silo.h"
#endif

#ifdef CALIPER_HAVE_HDF5
#include "gotcha_hdf5.h"
#endif

#ifdef GOTCHA_HAVE_MPI_IO
#include "gotcha_mpi_io.h"
#endif

#include "gotcha_posix.h"

using namespace cali;
using namespace std;

namespace
{

static const ConfigSet::Entry s_configdata[] = {
    { "extra", CALI_TYPE_STRING, "",
      "List of environment variables to add to the Caliper blackboard",
      "List of environment variables to add to the Caliper blackboard"
    },
    ConfigSet::Terminator
};

ConfigSet config;

void dissectio_service_register(Caliper* c)
{
    Log(1).stream() << "Registered dissectio service" << endl;
    Log(1).stream() << "Starting trace of I/O functions" << endl;

    config = RuntimeConfig::init("dissectio", s_configdata);
#ifdef CALIPER_HAVE_SILO
    init_gotcha_all_silo();
    init_gotcha_silo();
#endif
    
#ifdef CALIPER_HAVE_HDF5
    init_gotcha_hdf5();
#endif

#ifdef CALIPER_HAVE_MPI_IO
    init_gotcha_mpi_io();
#endif

    init_gotcha_posix();
}
  
}

namespace cali
{
    CaliperService dissectio_service = { "dissectio", ::dissectio_service_register };
} // namespace cali
 
