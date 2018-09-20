set(CALIPER_SAMPLE_MEASUREMENT_SOURCES
  SampleMeasurementService.cpp)

add_service_sources(${CALIPER_SAMPLE_MEASUREMENT_SOURCES})
add_caliper_service("samplemeasurement")
