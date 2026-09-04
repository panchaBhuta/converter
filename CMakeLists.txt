#
# URL:      https://github.com/panchaBhuta/converter
#
# Copyright (c) 2023-2025 Gautam Dhar
# All rights reserved.
#
# converter is distributed under the BSD 3-Clause license, see LICENSE for details.
#

# This code is losely based on 'project(UpstreamLib)' as in url below.
# https://cmake.org/cmake/help/v3.27/manual/cmake-packages.7.html#creating-packages

# Project
cmake_minimum_required(VERSION 3.23)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")
include(package-build-infrstructure)  # cmake/package-build-infrstructure.cmake
include(project-Instrumentation)      # cmake/project-Instrumentation.cmake
include(converter-capabilities)       # cmake/converter-capabilities.cmake
include(converter)                    # cmake/converter.cmake
include(converter-install)            # cmake/converter-install.cmake




# Get the version of the library
project_getversion(converter
                   "${CMAKE_CURRENT_SOURCE_DIR}/include/converter/converter.h"
                   "CONVERTER_VERSION_"
                   PROJECT_VERSION)

#[==================================================================================[
# https://cmake.org/cmake/help/v3.27/guide/tutorial/Adding%20Generator%20Expressions.html#cmakelists-txt-target-compile-options-genex
#
# refer https://cmake.org/cmake/help/v3.27/manual/cmake-generator-expressions.7.html#genex:COMPILE_LANG_AND_ID
# when the language used for compilation unit matches language and CMake's compiler id of the language compiler
# matches any one of the comma-separated entries in compiler_ids, otherwise 0.
#
# Compiler identification string : Possible values
# https://cmake.org/cmake/help/latest/variable/CMAKE_LANG_COMPILER_ID.html
#
#]==================================================================================]

set(PROJECT_DESCRIPTION "A header-only lightweight C++ string2Type and Type2string converter")
set(PROJECT_HOMEPAGE_URL "https://github.com/panchaBhuta/converter")

project(converter
        VERSION "${PROJECT_VERSION}"
        DESCRIPTION "${PROJECT_DESCRIPTION}"
        HOMEPAGE_URL "${PROJECT_HOMEPAGE_URL}"
        LANGUAGES CXX)



# Must include after the project call due to GNUInstallDirs requiring a language be enabled (IE. CXX)
include(GNUInstallDirs)

message(STATUS "############################################### converter :: CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
message(STATUS "#### OS-id=${CMAKE_SYSTEM_NAME}   OS-id-version=${CMAKE_SYSTEM}")
message(STATUS "#### CMAKE_CXX_COMPILER_ID=${CMAKE_CXX_COMPILER_ID}    version=${CMAKE_CXX_COMPILER_VERSION}")

project_cmake_variables_config(converter)
project_os_compiler_probe(converter)

# Determine whether this is a standalone project or included by other projects
set(CONVERTER_STANDALONE_PROJECT OFF)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(CONVERTER_STANDALONE_PROJECT ON)
endif()
#[===========[  donot use generator-expressions in set() functions
set(CONVERTER_STANDALONE_PROJECT   $<STREQUAL:"${CMAKE_SOURCE_DIR}","${PROJECT_SOURCE_DIR}">)
# refer  https://stackoverflow.com/questions/35695152/cmake-generator-expression-is-not-being-evaluated
# While generator expression is stored at configuration stage
# (when corresponded CMake command is executed), evaluation of
# generator expressions is performed at build stage.

# This is why message() command prints generator expression in
# non-dereferenced form: value denoted by the generator
# expression is not known at this stage.
#]===========]
message(STATUS "CONVERTER_STANDALONE_PROJECT=${CONVERTER_STANDALONE_PROJECT}")

# Establish the project options
option(CONVERTER_BUILD_EXAMPLES   "Set to ON to build examples"        ${CONVERTER_STANDALONE_PROJECT})
option(CONVERTER_BUILD_TESTS      "Set to ON to build tests"           ${CONVERTER_STANDALONE_PROJECT})
option(CONVERTER_ENABLE_INSTALL   "Generate the install target"        ON)
option(CONVERTER_ENABLE_WARNINGS  "Add warnings to CMAKE_CXX_FLAGS"    ${CONVERTER_STANDALONE_PROJECT})
#option(CONVERTER_USE_UNICODE_HELP "Use ICU Unicode library"            OFF)

set(_DEBUG_LOG OFF)
if (CONVERTER_STANDALONE_PROJECT AND
    CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(_DEBUG_LOG ON)
endif()
#message(STATUS "_DEBUG_LOG=${_DEBUG_LOG}")
# for _DEBUG_LOG can't use generator-expression as its computed during build-stage, but we need it during config-stage
option(OPTION_CONVERTER_debug_log  "Set to ON for debugging logs"  ${_DEBUG_LOG})
message(STATUS "OPTION_CONVERTER_debug_log=${OPTION_CONVERTER_debug_log}")
#[===========[  donot use generator-expressions in option() functions
# option(OPTION_CONVERTER_debug_log  "Set to ON for debugging logs"   "$<AND:$<CONFIG:Debug>,$<CONVERTER_STANDALONE_PROJECT>>")
#]===========]




file(TO_NATIVE_PATH "/" _path_separator)
message(STATUS "The systems's path separator is '${_path_separator}'")


#[===================================================================[
   converter (header only) library
#]===================================================================]
add_library( converter INTERFACE )
add_library( converter::converter ALIAS converter )

message(STATUS "CONVERTER_STANDALONE_PROJECT=${CONVERTER_STANDALONE_PROJECT}")
set(CONVERTER_CXX_STANDARD 20)
project_set_cxx_standard(converter INTERFACE ${CONVERTER_CXX_STANDARD})
#[======================================================================================[
Q :: what if down stream project has ...
         set(CMAKE_CXX_STANDARD 23)   and
         target_compile_features(my_target PUBLIC cxx_std_23)
     ... will there be any conflict in the build  ?

A :: No, there will be no build conflict. CMake handles this situation completely automatically and gracefully by design.
     When a downstream project requires C++23, CMake resolves the differences using a strict "highest standard wins" rule.

     How CMake Resolves It
     ---------------------
     When the downstream project links to your library (my_target), CMake looks at both requirements:
        1. Your library requires: At least C++20 (cxx_std_20)
        2. The downstream target requires: At least C++23 (cxx_std_23)
     Since C++23 is backward-compatible with C++20, CMake satisfies both requirements
     simultaneously by compiling the downstream target with -std=c++23.

     The One Catch: ABI Compatibility
     --------------------------------
     While CMake will compile the files without any configuration errors,
     you must ensure your library code is source-compatible with C++23.

     If your library uses a feature that was deprecated or removed in C++23
     (for example, std::allocator<void> or certain elements of <codecvt>),
     the downstream project will encounter compilation errors when including your headers,
     because its compiler is evaluating your header files through the lens of C++23 rules.

     Summary of Behaviors
     --------------------
     Downstream is C++23, You are C++20:
                Downstream compiles as C++23. (No conflict).
     Downstream is C++17, You are C++20:
                CMake will force the downstream target up to C++20 to satisfy your library's requirement.
#]======================================================================================]


check_chrono_stream_functionality()  # fetches date-lib if needed
check_clang_string_workaround()
check_floatingPoint_elementaryStringConversions()



project_check_cxx_compiler_flag_file_prefix_map(converter INTERFACE CONVERTER)


if (CONVERTER_ENABLE_WARNINGS)
    project_enable_warnings(converter INTERFACE)
endif()

converter_build()


# Link against the ICU library when requested
#if(CONVERTER_USE_UNICODE_HELP)
#    project_use_unicode(converter INTERFACE)
#endif()

# Install converter when requested by the user
if (CONVERTER_ENABLE_INSTALL)
    converter_install_logic()
endif()




message(STATUS "Build tests: ${CONVERTER_BUILD_TESTS}")
if(CONVERTER_BUILD_TESTS)

    # Enable instrument-tool(s) when requested by the user
    configure_test_Instrumentation_Environment(${CONVERTER_CXX_STANDARD})




    enable_testing()
    include(CTest)
    # Ccache
    find_program(CCACHE_PROGRAM ccache)
    if(CCACHE_PROGRAM)
        message(STATUS "Found ccache")
        set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
    endif()

    add_subdirectory(tests)
endif()




# Build examples when requested by the user
message(STATUS "Build examples: ${CONVERTER_BUILD_EXAMPLES}")
if (CONVERTER_BUILD_EXAMPLES)
    #add_subdirectory(examples)
endif()

