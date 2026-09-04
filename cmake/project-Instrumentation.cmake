#
# URL:      https://github.com/panchaBhuta/converter
#
# Copyright (c) 2026-2026 Gautam Dhar
# All rights reserved.
#
# project-Instrumentation is distributed under the BSD 3-Clause license, see LICENSE for details.
#



macro(configure_test_Instrumentation_Environment  PROJECT_CXX_STANDARD)
    if(CMAKE_BUILD_TYPE MATCHES "MemCheck")
        find_program(MEMORYCHECK_COMMAND  NAMES valgrind REQUIRED)

        # Set Valgrind options for memcheck
        #set(CTEST_MEMORYCHECK_TYPE "Valgrind")    CTEST_* is meant to be for variables defined in 'memchec.cmake' and called so -> "ctest -S memcheck.cmake ...."
        #set(CTEST_MEMORYCHECK_COMMAND "valgrind --log-file=MemoryChecker.%p.log")
        #set(CTEST_MEMORYCHECK_COMMAND_OPTIONS ...) doesn't work
        #set(VALGRIND_COMMAND "valgrind --log-file=memCheck.%p.log")  # doesnot work
        #set(VALGRIND_COMMAND_OPTIONS "--leak-check=full --show-leak-kinds=all --track-origins=yes --verbose")     # call before `include(CTest)

        set(MEMORYCHECK_TYPE Valgrind)
        #set(MEMORYCHECK_COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/tests/valgrind_wrapper_memcheck.sh")
        set(MEMORYCHECK_COMMAND /usr/bin/valgrind)
        set(MEMORYCHECK_COMMAND_OPTIONS "--tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose")     # call before `include(CTest)
        #set(MEMORYCHECK_SUPPRESSIONS_FILE "${PROJECT_SOURCE_DIR}/.valgrind-suppressions")  # must go before `include(CTest)`
    endif()

    if(CMAKE_BUILD_TYPE MATCHES "Coverage")
        find_program(COVERAGE_COMMAND  NAMES gcov REQUIRED)
        find_program(COVER_COMMAND  NAMES gcovr REQUIRED)
        find_program(LCOV_COMMAND  NAMES lcov REQUIRED)
        find_program(GENHTML_COMMAND  NAMES genhtml REQUIRED) # genhtml gets installed along with lcov, no need to install seperately
    endif()

    if(CMAKE_BUILD_TYPE MATCHES "CpuProf")
        find_program(CPUPROFILING_COMMAND  NAMES valgrind REQUIRED)

        set(MEMORYCHECK_TYPE Valgrind)
        set(MEMORYCHECK_COMMAND /usr/bin/valgrind)
        set(MEMORYCHECK_COMMAND_OPTIONS "--tool=callgrind --dump-instr=yes --collect-jumps=yes")     # call before `include(CTest)
    endif()

    if(CMAKE_BUILD_TYPE MATCHES "MemProf")
        find_program(MEMPROFILING_COMMAND  NAMES valgrind REQUIRED)

        set(MEMORYCHECK_TYPE Valgrind)
        set(MEMORYCHECK_COMMAND /usr/bin/valgrind)
        set(MEMORYCHECK_COMMAND_OPTIONS "--tool=massif")     # call before `include(CTest)
    endif()

    if(CMAKE_BUILD_TYPE MATCHES "CppCheck")
        find_program(CPPCHECK_COMMAND NAMES cppcheck REQUIRED)

        set(CPPCHECK_COMMAND_OPTIONS --enable=all --inline-suppr --quiet --std=c++${PROJECT_CXX_STANDARD} # -i/home/vishnu/repositories/converter/build-cppcheck-gpp/_deps/*
                                     --output-file=${PROJECT_BINARY_DIR}/cppcheck_staticAnalysis/cppcheck_output.txt
                                     --suppressions-list=${PROJECT_BINARY_DIR}/cppcheck_staticAnalysis/CppCheckSuppressions.txt)
        #set(CMAKE_C_CPPCHECK ${CPPCHECK_COMMAND} --std=c11 ${CPPCHECK_COMMAND_OPTIONS})
        set(CMAKE_CXX_CPPCHECK ${CPPCHECK_COMMAND} --std=c++${PROJECT_CXX_STANDARD} ${CPPCHECK_COMMAND_OPTIONS})
    endif()
endmacro()

