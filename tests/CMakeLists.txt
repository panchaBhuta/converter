#
# URL:      https://github.com/panchaBhuta/converter
#
# Copyright (c) 2023-2023 Gautam Dhar
# All rights reserved.
#
# converter is distributed under the BSD 3-Clause license, see LICENSE for details.
#


# https://stackoverflow.com/questions/14446495/cmake-project-structure-with-unit-tests


# Test macro add_unit_test
macro(add_unit_test testname)
  if(CMAKE_BUILD_TYPE MATCHES "Debug")
  #if(BUILD_TESTING)
    FILE(GLOB sources ${testname}*.cpp)
    add_executable(${testname} ${sources})
    add_test(NAME ${testname} COMMAND "${PROJECT_BINARY_DIR}/tests/${testname}")
    target_compile_definitions(${testname} PUBLIC
	         CONVERTER_TEST_MACOS_13=$<STREQUAL:"${CONVERTER_TEST_OS_VERSION}","macos-13">)
    target_link_libraries(${testname} converter) # donot specify PUBLIC here,
    # executable ${testname} has to have transitive association to all its dependencies(aka library chain)
    # refer   https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html#target-usage-requirements
    # refer   https://cmake.org/cmake/help/latest/command/target_link_libraries.html#libraries-for-a-target-and-or-its-dependents
    # refer   https://cmake.org/cmake/help/latest/command/target_link_libraries.html#libraries-for-a-target-and-or-its-dependents
    set_target_properties(${testname} PROPERTIES  LINKER_LANGUAGE CXX)
  endif()
endmacro(add_unit_test)

# Test macro add_perf_test
macro(add_perf_test testname)
  if(CMAKE_BUILD_TYPE MATCHES "Release")
    FILE(GLOB sources ${testname}*.cpp)
    add_executable(${testname} ${sources})
    add_test(NAME ${testname} COMMAND "${PROJECT_BINARY_DIR}/tests/${testname}")
    target_link_libraries(${testname} converter) # donot specify PUBLIC here,
    # executable ${testname} has to have transitive association to all its dependencies(aka library chain)
    # refer   https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html#target-usage-requirements
    # refer   https://cmake.org/cmake/help/latest/command/target_link_libraries.html#libraries-for-a-target-and-or-its-dependents
    # refer   https://cmake.org/cmake/help/latest/command/target_link_libraries.html#libraries-for-a-target-and-or-its-dependents
  endif()
endmacro(add_perf_test)




# Unit tests
add_unit_test(testDefaultTemplateInstantiation)
add_unit_test(testAllTemplateInstantiation)
add_unit_test(testBasicConversions)
add_unit_test(testFloatingPointNaN)
add_unit_test(testIntegerNaN)

add_unit_test(testFloatingPointPrecision)
add_unit_test(testUserDefinedConverter_lowerPrecision)
add_unit_test(testUserDefinedConverter_locale)
add_unit_test(testFloatingPointLocale)

add_unit_test(testDateConversions)
add_unit_test(testDateConversionFormat)
add_unit_test(testDateConversionWindowsHeader)
add_unit_test(testDateYMD_format_dbY)
add_unit_test(testDateYMD_format_YMD)

add_unit_test(testTupleConversions)
##add_unit_test(testInvalidComposition)

add_unit_test(testCIstring)

add_unit_test(testPlatform)

#[======================[
# perf tests
add_perf_test(ptest001)
add_perf_test(ptest002)
#]======================]


