// Copyright (c) 2017, Lawrence Livermore National Security, LLC.
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

// KokkosLookup.cpp
// Caliper kokkos variable lookup service

#include "caliper/CaliperService.h"

#include "caliper/Caliper.h"
#include "../../../caliper/MemoryPool.h"

#include "caliper/common/Log.h"
#include "caliper/common/RuntimeConfig.h"
#include "caliper/Annotation.h"

#include <iterator>
#include <cstdint>

#include <gotcha/gotcha.h>
#include <cuda_runtime_api.h>
#include <Kokkos_Core.hpp>

#include <mpi.h>
using namespace cali;
namespace {
    std::map<void*, size_t> allocation_size_tracker;
    std::map<MPI_Request*, void*> request_tracker;
    gotcha_wrappee_handle_t mpi_send_handle = nullptr;
    gotcha_wrappee_handle_t mpi_isend_handle = nullptr;
    gotcha_wrappee_handle_t mpi_reduce_handle = nullptr;
    gotcha_wrappee_handle_t mpi_allreduce_handle = nullptr;
    gotcha_wrappee_handle_t mpi_wait_handle = nullptr;
    gotcha_wrappee_handle_t mpi_waitall_handle = nullptr;
    gotcha_wrappee_handle_t cuda_malloc_handle = nullptr;
    gotcha_wrappee_handle_t cuda_malloc_managed_handle = nullptr;
    gotcha_wrappee_handle_t cuda_free_handle = nullptr;
    cudaError_t cudaFreeWrapper(void* buf){
        decltype(&cudaFreeWrapper) wrapped_call = reinterpret_cast<decltype(&cudaFreeWrapper)>(gotcha_get_wrappee(cuda_free_handle));
        allocation_size_tracker.erase(buf);
        cudaError_t error = wrapped_call(buf);
        return error;
      
    }
    cudaError_t cudaMallocWrapper(void** buf, size_t size){
        decltype(&cudaMallocWrapper) wrapped_call = reinterpret_cast<decltype(&cudaMallocWrapper)>(gotcha_get_wrappee(cuda_malloc_handle));
        cudaError_t error = wrapped_call(buf,size);
        allocation_size_tracker[*buf] = size;
        return error;
      
    }
    cudaError_t cudaMallocManagedWrapper(void** buf, size_t size, int flags){
        decltype(&cudaMallocManagedWrapper) wrapped_call = reinterpret_cast<decltype(&cudaMallocManagedWrapper)>(gotcha_get_wrappee(cuda_malloc_managed_handle));
        cudaError_t error = wrapped_call(buf,size,flags);
        allocation_size_tracker[*buf] = size;
        return error;
    }
    template<typename transform>
    auto with_host_buffer(const void* buf, transform trans) -> decltype(trans(buf)){
        cudaPointerAttributes attrs;
        cudaPointerGetAttributes(&attrs, buf);
        Kokkos::View<char*,Kokkos::CudaSpace,Kokkos::MemoryTraits<Kokkos::Unmanaged>> uvm_view((char*)buf,allocation_size_tracker[
           const_cast<void*>(buf)
        ]);
        if(attrs.isManaged) {
            auto host_view = Kokkos::create_mirror_view(uvm_view);
            Kokkos::deep_copy(host_view, uvm_view);
            const void* final_buf = host_view.data();
            return trans(final_buf);
        }
        else{
            return trans(buf);
        }
        
    }
    int MPEyeSend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm) {
        decltype(&MPEyeSend) wrapped_call = reinterpret_cast<decltype(&MPEyeSend)>(gotcha_get_wrappee(mpi_send_handle));
        return with_host_buffer(buf, [&](const void* host_buf){
           return wrapped_call(host_buf, count, datatype, dest, tag, comm);
        });
        
    }
    int MPEyeISend(const void* buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm ,MPI_Request* request){
        decltype(&MPEyeISend) wrapped_call = reinterpret_cast<decltype(&MPEyeISend)>(gotcha_get_wrappee(mpi_isend_handle));
        return with_host_buffer(buf, [&](const  void* host_buf) {
           int return_code = wrapped_call(host_buf, count, datatype, dest, tag, comm, request);
           request_tracker[request] = const_cast<void*>(host_buf); 
           return return_code;
        });
    }
    int MPEyeReduce(const void* sendbuf, const void* recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm){
        decltype(&MPEyeReduce) wrapped_call = reinterpret_cast<decltype(&MPEyeReduce)>(gotcha_get_wrappee(mpi_reduce_handle));
        return with_host_buffer(sendbuf, [&](const void* hostsendbuf) {
          return with_host_buffer(recvbuf, [&](const void* hostrecvbuf) {
            return wrapped_call(hostsendbuf, hostrecvbuf, count, datatype, op, root, comm); 
          }); 
        }); 
    }
    int MPEyeAllReduce(const void* sendbuf, const void* recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm){
        decltype(&MPEyeAllReduce) wrapped_call = reinterpret_cast<decltype(&MPEyeAllReduce)>(gotcha_get_wrappee(mpi_allreduce_handle));
        return with_host_buffer(sendbuf, [&](const void* hostsendbuf) {
          return with_host_buffer(recvbuf, [&](const void* hostrecvbuf) {
            return wrapped_call(hostsendbuf, hostrecvbuf, count, datatype, op, comm); 
          }); 
        }); 
    }
    int MPEyeWait(MPI_Request* req, MPI_Status* stat){
        decltype(&MPEyeWait) wrapped_call = reinterpret_cast<decltype(&MPEyeWait)>(gotcha_get_wrappee(mpi_wait_handle));
        int status =  wrapped_call(req,stat);
         
        return status;
    }
    int MPEyeWaitall(int count, MPI_Request array_of_requests[], MPI_Status array_of_statuses[]){
        decltype(&MPEyeWaitall) wrapped_call = reinterpret_cast<decltype(&MPEyeWaitall)>(gotcha_get_wrappee(mpi_waitall_handle));
        return wrapped_call(count, array_of_requests, array_of_statuses);
    }
    class MPEye {
        static bool is_wrapped;
        static const ConfigSet::Entry s_configdata[];
        cali::Annotation kernel_name_annot;
        cali::Annotation kernel_type_annot;

        MPEye(Caliper *c, Channel *chn) : kernel_name_annot("function"),
                                               kernel_type_annot("kernel_type", CALI_ATTR_SKIP_EVENTS) {
        }

    public:

        void pushRegion(const char *name, const char *type) {
            kernel_type_annot.begin(type);
            kernel_name_annot.begin(name);
        }

        void popRegion() {
            kernel_name_annot.end();
            kernel_type_annot.end();
        }

        static void MPEyeRegister(Caliper *c, Channel *chn) {
            if(!is_wrapped){
            struct gotcha_binding_t mpeye_bindings[] = {
               {"MPI_Send", (void*) MPEyeSend, &mpi_send_handle},
               {"MPI_Isend", (void*) MPEyeISend, &mpi_isend_handle},
               {"MPI_Reduce", (void*) MPEyeReduce, &mpi_reduce_handle},
               {"MPI_Allreduce", (void*) MPEyeAllReduce, &mpi_allreduce_handle},
               {"MPI_Wait", (void*) MPEyeWait, &mpi_wait_handle},
               {"MPI_Waitall", (void*) MPEyeWaitall, &mpi_waitall_handle},
               {"cudaMalloc", (void*) cudaMallocWrapper, &cuda_malloc_handle},
               {"cudaMallocManaged", (void*) cudaMallocManagedWrapper, &cuda_malloc_managed_handle},
               {"cudaFree", (void*) cudaFreeWrapper, &cuda_free_handle}
            };
            gotcha_wrap(mpeye_bindings, sizeof(mpeye_bindings)/sizeof(struct gotcha_binding_t),"caliper/mpeye");
            }
            auto *instance = new MPEye(c, chn);
            chn->events().post_init_evt.connect(
                    [instance](Caliper *c, Channel *chn) {
                    });
            Log(1).stream() << chn->name() << ": Registered kokkostime service" << std::endl;
        }
    };

    const ConfigSet::Entry MPEye::s_configdata[] = {
            ConfigSet::Terminator
    };
    bool MPEye::is_wrapped = false;

} // namespace [anonymous]


namespace cali {

    CaliperService mpeye_service{"mpeye", ::MPEye::MPEyeRegister};

}
