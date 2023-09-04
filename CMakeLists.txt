#
# URL:      https://github.com/panchaBhuta/converter
#
# Copyright (c) 2023-2023 Gautam Dhar
# All rights reserved.
#
# converter is distributed under the BSD 3-Clause license, see LICENSE for details.
#

# This code is losely based on 'project(UpstreamLib)' as in url below.
# https://cmake.org/cmake/help/v3.27/manual/cmake-packages.7.html#creating-packages

# Project
cmake_minimum_required(VERSION 3.23)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")
include(converter)
set(PROJECT_DESCRIPTION "A header-only lightweight C++ string2Type and Type2string converter")
set(PROJECT_HOMEPAGE_URL "https://github.com/panchaBhuta/converter")

# Get the version of the library
converter_getversion(VERSION)

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
set(clang_cxx "$<COMPILE_LANG_AND_ID:CXX,Clang>")
set(clang_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang>")
set(gcc_cxx "$<COMPILE_LANG_AND_ID:CXX,GNU>")
set(gcc_like_cxx "$<OR:$<COMPILE_LANG_AND_ID:CXX,GNU,LCC>,${clang_like_cxx}>")
set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")

set(_e_DISABLED_FEATURE_      -1)
set(_e_USE_DEFAULT_FEATURE_    0)
set(_e_USE_WORKAROUND_1_       1)
set(_e_USE_WORKAROUND_2_       2)



project(converter
        VERSION "${VERSION}"
        DESCRIPTION "${PROJECT_DESCRIPTION}"
        HOMEPAGE_URL "${PROJECT_HOMEPAGE_URL}"
        LANGUAGES CXX)

# Must include after the project call due to GNUInstallDirs requiring a language be enabled (IE. CXX)
include(GNUInstallDirs)

# Determine whether this is a standalone project or included by other projects
set(CONVERTER_STANDALONE_PROJECT OFF)
if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(CONVERTER_STANDALONE_PROJECT ON)
endif()

# Establish the project options
option(CONVERTER_BUILD_EXAMPLES "Set to ON to build examples"        ${CONVERTER_STANDALONE_PROJECT})
option(CONVERTER_BUILD_TESTS "Set to ON to build tests"              ${CONVERTER_STANDALONE_PROJECT})
option(CONVERTER_ENABLE_INSTALL "Generate the install target"        ${CONVERTER_STANDALONE_PROJECT})
option(CONVERTER_ENABLE_WARNINGS "Add warnings to CMAKE_CXX_FLAGS"   ${CONVERTER_STANDALONE_PROJECT})
#option(CONVERTER_USE_UNICODE_HELP "Use ICU Unicode library"         OFF)

if (CONVERTER_STANDALONE_PROJECT)
    converter_set_cxx_standard()
endif()

file(TO_NATIVE_PATH "/" _path_separator)
message("The systems's path separator is '${_path_separator}'")

# Library
add_library(converter INTERFACE)
add_library(converter::converter ALIAS converter)

converter_check_chrono_stream_functionality()
converter_check_clang_string_workaround()

check_cxx_compiler_flag_macro_prefix_map()

#if (CONVERTER_ENABLE_WARNINGS)
    converter_enable_warnings()
#endif()

converter_build()


# Link against the ICU library when requested
#if(CONVERTER_USE_UNICODE_HELP)
#    converter_use_unicode()
#endif()

# Install converter when requested by the user
if (CONVERTER_ENABLE_INSTALL)
    converter_install_logic()
endif()




# Enable testing when requested by the user
message(STATUS "Build tests: ${CONVERTER_BUILD_TESTS}")
if(CONVERTER_BUILD_TESTS)
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



#[======================[
# Build examples when requested by the user
if (CONVERTER_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
#]======================]
