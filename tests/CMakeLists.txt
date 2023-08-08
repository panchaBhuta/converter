#
# URL:      https://github.com/panchaBhuta/converter
#
# Copyright (c) 2023-2023 Gautam Dhar
# All rights reserved.
#
# converter is distributed under the BSD 3-Clause license, see LICENSE for details.
#


# Ccache
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  message(STATUS "Found ccache")
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

# https://stackoverflow.com/questions/14446495/cmake-project-structure-with-unit-tests

# Test macro add_perf_test
macro(add_perf_test testname)
  if(CMAKE_BUILD_TYPE MATCHES "Release")
    FILE(GLOB sources ${testname}*.cpp)
    add_executable(${testname} ${sources})
    add_test(NAME ${testname} COMMAND "${PROJECT_BINARY_DIR}/tests/${testname}")
    target_link_libraries(${testname} PUBLIC converter)
  endif()
endmacro(add_perf_test)

# Test macro add_unit_test
macro(add_unit_test testname)
  if(CMAKE_BUILD_TYPE MATCHES "Debug")
  #if(BUILD_TESTING)
    FILE(GLOB sources ${testname}*.cpp)
    add_executable(${testname} ${sources})
    add_test(NAME ${testname} COMMAND "${PROJECT_BINARY_DIR}/tests/${testname}")
    target_link_libraries(${testname} PUBLIC converter)
  endif()
endmacro(add_unit_test)

# Unit tests
add_unit_test(testTemplateInstantiation)
add_unit_test(test001)
#[=========[

add_unit_test(testViewYmd001)
add_unit_test(testViewYmd003)
add_unit_test(testViewYmd005)

# perf tests
add_perf_test(ptest001)
add_perf_test(ptest002)
#]=========]
