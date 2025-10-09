#
# URL:      https://github.com/panchaBhuta/converter
#
# Copyright (c) 2023-2025 Gautam Dhar
# All rights reserved.
#
# converter is distributed under the BSD 3-Clause license, see LICENSE for details.
#


# https://stackoverflow.com/questions/14446495/cmake-project-structure-with-unit-tests


# Test macro add_unit_test
macro(add_unit_test testname)
  if(CMAKE_BUILD_TYPE MATCHES "Debug"     OR
     CMAKE_BUILD_TYPE MATCHES "MemCheck"  OR
     CMAKE_BUILD_TYPE MATCHES "Coverage"  OR
     CMAKE_BUILD_TYPE MATCHES "CpuProf"   OR
     CMAKE_BUILD_TYPE MATCHES "MemProf"   OR
     CMAKE_BUILD_TYPE MATCHES "CppCheck")
  #if(BUILD_TESTING)

    FILE(GLOB sourcesExe  ${testname}.cpp ${ARGN})
    # ARGN here contains list of additional cpp files(includes the extension) if any
    add_executable(${testname} ${sourcesExe})
    add_test(NAME ${testname} COMMAND "${PROJECT_BINARY_DIR}/tests/${testname}")

    if(CMAKE_BUILD_TYPE MATCHES "MemCheck"  OR
       CMAKE_BUILD_TYPE MATCHES "CpuProf"   OR
       CMAKE_BUILD_TYPE MATCHES "MemProf")
      # https://stackoverflow.com/questions/44102050/valgrind-leading-to-numeric-issues-with-long-doubles
      #
      target_compile_definitions(${testname} PRIVATE USE_VALGRIND)
    endif()
    #target_compile_definitions(${testname}
    #            gives error as COMPILE_LANG_AND_ID is empty
    #            $<$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang>:COMPILING_CXX_WITH_CLANG>
    #            $<$<COMPILE_LANG_AND_ID:CXX,Intel>:COMPILING_CXX_WITH_INTEL>)

    target_link_libraries(${testname} converter) # donot specify PUBLIC here,
    # executable ${testname} has to have transitive association to all its dependencies(aka library chain)
    # refer   https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html#target-usage-requirements
    # refer   https://cmake.org/cmake/help/latest/command/target_link_libraries.html#libraries-for-a-target-and-or-its-dependents

    if(CMAKE_BUILD_TYPE MATCHES "Coverage")
      #refer   https://github.com/bilke/cmake-modules/blob/master/CodeCoverage.cmake
      # compile with coverage options
      target_compile_options(${testname} PRIVATE --coverage -O0)
      target_link_options(${testname} PRIVATE --coverage)

      # How to use gcovr for code coverage?
      # If you have multiple compilers installed or if you are using Clang,
      # you will likely need to tell gcovr which gcov executable to use.
      # By default, gcovr just uses the program named gcov .
      # This is fine for the default GCC compiler, e.g. gcc or g++ .
      # Otherwise, you must use the --gcov-executable to tell gcovr what to use.

      #  doesn't  work as expected, instead directly calling gcovr in make.sh
      # add coverage target
#      add_custom_target(coverage_${testname}
        # gather data
#        COMMAND ${LCOV_COMMAND} --directory . --capture --output-file coverage.info
        # generate report
#        COMMAND ${GENHTML_COMMAND} --demangle-cpp -o coverage coverage.info
#        WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

    endif()

    set_target_properties(${testname} PROPERTIES  LINKER_LANGUAGE CXX)
  endif()
endmacro(add_unit_test)

# Test macro add_perf_test
macro(add_perf_test testname)
  if(CMAKE_BUILD_TYPE MATCHES "Release")
    FILE(GLOB sourcesExe  ${testname}.cpp ${ARGN})
    # ARGN here contains list of additional cpp files(includes the extension) if any
    add_executable(${testname} ${sourcesExe})
    add_test(NAME ${testname} COMMAND "${PROJECT_BINARY_DIR}/tests/${testname}")

    #target_compile_definitions(${testname}
    #            $<$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang>:COMPILING_CXX_WITH_CLANG>
    #            $<$<COMPILE_LANG_AND_ID:CXX,Intel>:COMPILING_CXX_WITH_INTEL>)

    target_link_libraries(${testname} converter)   # donot specify PUBLIC here,
    # executable ${testname} has to have transitive association to all its dependencies(aka library chain)
    # refer   https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html#target-usage-requirements
    # refer   https://cmake.org/cmake/help/latest/command/target_link_libraries.html#libraries-for-a-target-and-or-its-dependents
  endif()
endmacro(add_perf_test)




# Unit tests
add_unit_test(testPlatform)

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
add_unit_test(testRegexString)


#[======================[
# perf tests
add_perf_test(ptest001)
add_perf_test(ptest002)
#]======================]


