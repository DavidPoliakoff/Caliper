// Copyright (c) 2019, Lawrence Livermore National Security, LLC.
// See top-level LICENSE file for details.

// Cupti.cpp
// Implementation of Cupti service

#include "CuptiEventSampling.h"

#include "caliper/CaliperService.h"

#include "caliper/Caliper.h"
#include "caliper/SnapshotRecord.h"

#include "caliper/common/Log.h"
#include "caliper/common/RuntimeConfig.h"

#include <cupti.h>

#include <cuda_runtime_api.h>

#include <nvToolsExt.h>
#if CUDART_VERSION >= 9000
#include <nvToolsExtSync.h>
#endif
#include <generated_nvtx_meta.h>
#include <gotcha/gotcha.h>

#include <vector>
#include "caliper/Annotation.h"
using namespace cali;

static gotcha_wrappee_handle_t sync_h;
static gotcha_wrappee_handle_t copy_h;
static gotcha_wrappee_handle_t async_copy_h;

size_t sync_wrapper(){
  cali::Annotation("cuda.sync").begin();
  cali::Annotation("cuda.sync").end();
  auto wrapped = reinterpret_cast<decltype(&sync_wrapper)> (gotcha_get_wrappee(sync_h)); 
  return wrapped();
}

size_t copy_wrapper( void* dst, const void* src, size_t count, size_t kind ){
  cali::Annotation("cuda.memcpy").begin();
  cali::Annotation("cuda.memcpy").end();
  auto wrapped = reinterpret_cast<decltype(&copy_wrapper)>(gotcha_get_wrappee(copy_h)); 
  return wrapped(dst,src,count,kind);
}

size_t copy_async_wrapper( void* dst, const void* src, size_t count, size_t kind, size_t stream ){
  cali::Annotation("cuda.memcpyasync").begin();
  cali::Annotation("cuda.memcpyasync").end();
  auto wrapped = reinterpret_cast<decltype(&copy_async_wrapper)>( gotcha_get_wrappee(async_copy_h)); 
  return wrapped(dst,src,count,kind,stream);
}

struct gotcha_binding_t wraps[] = {
  { "cudaDeviceSynchronize", (void*)sync_wrapper, &sync_h },
  { "cudaMemcpy", (void*)copy_wrapper, &copy_h },
  { "cudaMemcpyAsync", (void*)copy_wrapper, &async_copy_h }
};

namespace
{

void initialize_wrappers(Caliper* c, Channel* chn){
    gotcha_wrap(wraps, sizeof(wraps)/sizeof(struct gotcha_binding_t), "caliper.cuda.wrappers");
}

} // namespace

namespace cali
{

CaliperService cudawrap_service = { "cudawrap", ::initialize_wrappers };

}
