include_directories(${CUPTI_INCLUDE_DIR})
include_directories(${CUDA_INCLUDE_DIRS})
include_directories(${gotcha_INCLUDE_DIRS})

set(CALIPER_CUPTI_SOURCES
  CuptiEventSampling.cpp
  Cupti.cpp)

set(CUDA_WRAP_SRC
  CudaWrap.cpp
)

add_library(caliper-cupti OBJECT ${CALIPER_CUPTI_SOURCES})
add_library(caliper-cudawrap OBJECT ${CUDA_WRAP_SRC})


add_service_objlib("caliper-cupti")
add_service_objlib("caliper-cudawrap")
add_caliper_service("cudawrap CALIPER_HAVE_GOTCHA")
