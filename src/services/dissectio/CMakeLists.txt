if(GOTCHA_INCLUDE_DIR)
  include_directories(${GOTCHA_INCLUDE_DIR})
else()
  include_directories(${CMAKE_CURRENT_BINARY_DIR}/ext/gotcha/gotcha-download/gotcha-src/include)
  set(${CMAKE_CURRENT_BINARY_DIR}/ext/gotcha/gotcha-download/gotcha-src/include)
endif()

set(CALIPER_DISSECTIO_SOURCES
  Dissectio.cpp)


if(CALIPER_DISSECTIO_ALL_SILO)
  list(APPEND CALIPER_DISSECTIO_SOURCES "gotcha_all_silo.c")
endif()
if(CALIPER_DISSECTIO_HDF5)
  list(APPEND CALIPER_DISSECTIO_SOURCES "gotcha_hdf5.c")
endif()
if(CALIPER_DISSECTIO_MPI_IO)
  list(APPEND CALIPER_DISSECTIO_SOURCES "gotcha_mpi_io.c")
endif()
if(CALIPER_DISSECTIO_SILO)
  list(APPEND CALIPER_DISSECTIO_SOURCES "gotcha_silo.c")
endif()
list(APPEND CALIPER_DISSECTIO_SOURCES "gotcha_posix.c")

add_service_sources(${CALIPER_DISSECTIO_SOURCES})
add_caliper_service("dissectio CALIPER_HAVE_DISSECTIO")
