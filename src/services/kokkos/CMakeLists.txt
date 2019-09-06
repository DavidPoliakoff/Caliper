set(CALIPER_KOKKOS_SOURCES
  KokkosProfilingSymbols.cpp
  KokkosLookup.cpp
  KokkosTime.cpp
  proto/mpeye.cpp
)
set(KOKKOS_ENABLE_CUDA ON)
add_subdirectory(embedded_kokkos)
include_directories(${gotcha_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR}/embedded_kokkos)
include_directories(embedded_kokkos/core/src)
include_directories(/usr/tce/packages/cuda/cuda-9.2.148/include)
link_directories(/usr/tce/packages/cuda/cuda-9.2.148/lib)
include_directories(/usr/tce/packages/spectrum-mpi/ibm/spectrum-mpi-2019.01.30/include)
add_library(caliper-kokkos OBJECT ${CALIPER_KOKKOS_SOURCES})

add_service_objlib("caliper-kokkos")

target_link_libraries(caliper-kokkos cudart)
add_caliper_service("kokkostime")
add_caliper_service("kokkoslookup")
add_caliper_service("mpeye")
