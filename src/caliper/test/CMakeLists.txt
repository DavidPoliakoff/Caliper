set(CALIPER_TEST_SOURCES
  test_c_snapshot.cpp)

add_executable(test_caliper ${CALIPER_TEST_SOURCES})
target_link_libraries(test_caliper caliper gtest_main)

add_test(NAME test-caliper COMMAND test_caliper)
