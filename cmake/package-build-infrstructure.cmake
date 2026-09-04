#
# URL:      https://github.com/panchaBhuta/converter
#
# Copyright (c) 2026-2026 Gautam Dhar
# All rights reserved.
#
# capability-probe-framework is distributed under the BSD 3-Clause license, see LICENSE for details.
#


set(e_DISABLE_FEATURE    0)
set(e_ENABLE_FEATURE     1)


if (CMAKE_VERSION VERSION_GREATER_EQUAL 3.10)
    # Use include_guard() added in cmake 3.10
    include_guard()
endif()





#[==============================================================================================[
  extract the version number as declared in a header file.
  For e.g. in include/converter/_workaroundConfig.h there can be following lines as below ...

      #define  SUPPORTED_DATE_LIB_FOR_FROMSTREAM   e_DATE_CHRONO


  fetch the macro-value from the header-file, by making a call from cmake file like so ...
      get_macro_value( ${CMAKE_CURRENT_BINARY_DIR}/include/converter/_workaroundConfig.h
                      "SUPPORTED_DATE_LIB_FOR_FROMSTREAM"
                       SUPPORTED_DATE_LIB_FOR_FROMSTREAM)

#]==============================================================================================]
function(get_macro_value 
             HEADER_PATH
             HEADER_MACRO_NAME
             RESULT_VARIABLE)
    # 1. Check if file exists first to avoid errors
    if(NOT EXISTS "${HEADER_PATH}")
        message(WARNING "Header file not found: ${HEADER_PATH}")
        set(${RESULT_VARIABLE} "NOTFOUND" PARENT_SCOPE)
        return()
    endif()

    # 2. Search for the line containing the #define
    # This regex looks for: #define <HEADER_MACRO_NAME> <VALUE>
    file(STRINGS "${HEADER_PATH}" HEADER_MACRO_LINE 
         REGEX "^#define[ \t]+${HEADER_MACRO_NAME}[ \t]+")

    if(HEADER_MACRO_LINE)
        # 3. Extract the value part specifically
        # We replace the entire line with just the captured group ([^ \t\r\n]+)
        string(REGEX REPLACE "^#define[ \t]+${HEADER_MACRO_NAME}[ \t]+([^ \t\r\n]+).*" "\\1" 
               VALUE "${HEADER_MACRO_LINE}")
        
        set(${RESULT_VARIABLE} "${VALUE}" PARENT_SCOPE)
    else()
        set(${RESULT_VARIABLE} "NOTFOUND" PARENT_SCOPE)
    endif()
endfunction()




#[==============================================================================================[
  extract the version number as declared in a header file.
  For e.g. in include/converter.h there can be following lines as below ...

      #define CONVERTER_VERSION_MAJOR 2
      #define CONVERTER_VERSION_MINOR 4
      #define CONVERTER_VERSION_PATCH 33

  fetch the version number from the header-file, by making a call from cmake file like so ...
      project_getversion("converter"
                         "${CMAKE_CURRENT_SOURCE_DIR}/include/converter/converter.h"
                         "CONVERTER_VERSION_"
                         PROJECT_VERSION) 

#]==============================================================================================]
function(project_getversion
             PROJECT__NAME
             HEADER_PATH
             HEADER_MACRO_NAME_PREFIX
             version_arg)
    # 1. Check if file exists first to avoid errors
    if(NOT EXISTS "${HEADER_PATH}")
        message(WARNING "Header file not found: ${HEADER_PATH}")
        set(${RESULT_VARIABLE} "NOTFOUND" PARENT_SCOPE)
        return()
    endif()

    # Parse the current version from the project header
    file(STRINGS "${HEADER_PATH}" project_version_defines
        REGEX "#define ${HEADER_MACRO_NAME_PREFIX}(MAJOR|MINOR|PATCH)")   # -> "#define CONVERTER_VERSION_(MAJOR|MINOR|PATCH)"
    foreach(ver ${project_version_defines})
        if(ver MATCHES "#define ${HEADER_MACRO_NAME_PREFIX}(MAJOR|MINOR|PATCH) +([^ ]+)$")  # -> "#define CONVERTER_VERSION_(MAJOR|MINOR|PATCH) +([^ ]+)$"
            set(PROJECT_VERSION_${CMAKE_MATCH_1} "${CMAKE_MATCH_2}" CACHE INTERNAL "")
        endif()
    endforeach()
    set(VERSION_NUMBER ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})

    # Give feedback to the user. Prefer DEBUG when available since large projects tend to have a lot
    # going on already
    if (CMAKE_VERSION VERSION_GREATER_EQUAL 3.15)
        message(DEBUG "${PROJECT__NAME} version ${VERSION_NUMBER}")
    else()
        message(STATUS "${PROJECT__NAME} version ${VERSION_NUMBER}")
    endif()

    # Return the information to the caller
    set(${version_arg} ${VERSION_NUMBER} PARENT_SCOPE)
endfunction()



#[==================================================================================[
# Optionally, enable unicode support using the ICU library
macro(project_use_unicode
                PROJECT__NAME
                SCOPE)                  #     {INTERFACE|PUBLIC|PRIVATE}
    find_package(PkgConfig)
    pkg_check_modules(ICU REQUIRED icu-uc)

    target_link_libraries(${PROJECT__NAME} ${SCOPE} ${ICU_LDFLAGS})
    target_compile_options(${PROJECT__NAME} ${SCOPE} ${ICU_CFLAGS})
    target_compile_definitions(${PROJECT__NAME} ${SCOPE} ${PROJECT__NAME}_USE_UNICODE)
endmacro()
#]==================================================================================]



# Request C++20 without gnu extension for the whole project
macro(project_set_cxx_standard
             PROJECT__NAME
             SCOPE                   #     {INTERFACE|PUBLIC|PRIVATE}
             PROJECT_CXX_STANDARD)    
    # 1. Set the global default for internal targets, but make it strict
    set(CMAKE_CXX_STANDARD ${PROJECT_CXX_STANDARD})
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF) # Disables compiler-specific extensions (e.g., -std=gnu++20)

    # 2. Explicitly tag your targets (especially libraries) to communicate requirements
    target_compile_features(${PROJECT__NAME} ${SCOPE} cxx_std_${PROJECT_CXX_STANDARD})

    #[========================================================[
        C++ Meta-Features (cxx_std_<version>)
          *  cxx_std_98: Enforces at least C++98
          *  cxx_std_11: Enforces at least C++11
          *  cxx_std_14: Enforces at least C++14
          *  cxx_std_17: Enforces at least C++17
          *  cxx_std_20: Enforces at least C++20
          *  cxx_std_23: Enforces at least C++23
          *  cxx_std_26: Enforces at least C++26
    #]========================================================]
endmacro()



function(project_cmake_variables_config
                    PROJECT__NAME)
    # parameters which are NON Generator-Expressions if set outside of function,
    # not all variables CMAKE_* e.g(CMAKE_SYSTEM_NAME) are NOT set by CMAKE.
    # Seems those variables get set after call of 'include(GNUInstallDirs)' in CMakeLists.txt
    # this parameters are not available outside of this function
    set(_CNV_OS_FLAGS_ "OS-flags: UNIX=${UNIX} , APPLE=${APPLE} , WIN32=${WIN32}")
    set(_CNV_OS_NAME_  "OS-name: CMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME} , CMAKE_HOST_SYSTEM_NAME=${CMAKE_HOST_SYSTEM_NAME}")
    set(_CNV_SYSTEM_PROCESSOR_ "system-processor: CMAKE_SYSTEM_VERSION=${CMAKE_SYSTEM_VERSION} , CMAKE_SYSTEM_PROCESSOR=${CMAKE_SYSTEM_PROCESSOR}")
    set(_CNV_CXX_COMPILER_ "cxx-compiler: CMAKE_CXX_COMPILER_ID=${CMAKE_CXX_COMPILER_ID} , CMAKE_CXX_COMPILER_VERSION=${CMAKE_CXX_COMPILER_VERSION}")
    set(_CNV_COMPILE_LANGUAGE_ "compile-language: COMPILE_LANG_AND_ID=${COMPILE_LANG_AND_ID} , COMPILE_LANGUAGE=${COMPILE_LANGUAGE} , CXX_COMPILER_ID=${CXX_COMPILER_ID}")
    set(_CNV_LINK_LANGUAGE_ "link-language: LINK_LANG_AND_ID=${LINK_LANG_AND_ID}")
    set(_CNV_BUILD_ENV_ "ENV: MINGW=${MINGW} , MSYS=${MSYS} , CYGWIN=${CYGWIN}")
    set(_CNV_ENV_MSYSTEM_ "ENV: MSYSTEM=$ENV{MSYSTEM}")

    # Configure-time values
    message(STATUS "++++ ${_CNV_OS_FLAGS_}")
    message(STATUS "++++ ${_CNV_OS_NAME_}")
    message(STATUS "++++ ${_CNV_SYSTEM_PROCESSOR_}")
    message(STATUS "++++ ${_CNV_CXX_COMPILER_}")
    message(STATUS "++++ ${_CNV_COMPILE_LANGUAGE_}")
    message(STATUS "++++ ${_CNV_LINK_LANGUAGE_}")
    message(STATUS "++++ ${_CNV_BUILD_ENV_}")
    message(STATUS "++++ ${_CNV_ENV_MSYSTEM_}")

    # https://gitlab.kitware.com/cmake/community/-/wikis/doc/tutorials/How-To-Write-Platform-Checks
    # Build-time values
    add_custom_target(genexdebug_${PROJECT__NAME}_1 ALL COMMAND ${CMAKE_COMMAND} -E echo "**** ${_CNV_OS_FLAGS_}")
    add_custom_target(genexdebug_${PROJECT__NAME}_2 ALL COMMAND ${CMAKE_COMMAND} -E echo "**** ${_CNV_OS_NAME_}")
    add_custom_target(genexdebug_${PROJECT__NAME}_3 ALL COMMAND ${CMAKE_COMMAND} -E echo "**** ${_CNV_SYSTEM_PROCESSOR_}")
    add_custom_target(genexdebug_${PROJECT__NAME}_4 ALL COMMAND ${CMAKE_COMMAND} -E echo "**** ${_CNV_CXX_COMPILER_}")
    add_custom_target(genexdebug_${PROJECT__NAME}_5 ALL COMMAND ${CMAKE_COMMAND} -E echo "**** ${_CNV_COMPILE_LANGUAGE_}")
    add_custom_target(genexdebug_${PROJECT__NAME}_6 ALL COMMAND ${CMAKE_COMMAND} -E echo "**** ${_CNV_LINK_LANGUAGE_}")
    add_custom_target(genexdebug_${PROJECT__NAME}_7 ALL COMMAND ${CMAKE_COMMAND} -E echo "**** ${_CNV_BUILD_ENV_}")
    add_custom_target(genexdebug_${PROJECT__NAME}_8 ALL COMMAND ${CMAKE_COMMAND} -E echo "**** ${_CNV_ENV_MSYSTEM_}")
endfunction()

macro(project_os_compiler_probe
                    PROJECT__NAME)
    # parameters with Generator-Expressions can reside here at this level (i.e. Non-function)
    set(unix_like_os "$<BOOL:${UNIX}>")     # is TRUE on all UNIX-like OS's, including Apple OS X and CygWin
    set(apple_os     "$<BOOL:${APPLE}>")    # is TRUE on Apple systems. Note this does not imply the
                                            # system is Mac OS X, only that APPLE is #defined in C/C++ header files.
    set(windows_os   "$<BOOL:${WIN32}>")    # is TRUE on Windows. Prior to 2.8.4 this included CygWin
    set(unix_os      "$<AND:$<BOOL:${UNIX}>,$<NOT:$<OR:${apple_os},${windows_os}>>>")
                                            # is TRUE on all UNIX-like OS's, excluding Apple OS X and CygWin (on Windows)
    add_custom_target(genexdebug_${PROJECT__NAME}_9 ALL COMMAND ${CMAKE_COMMAND} -E echo "**** OS: unix_like_os=${unix_like_os} , unix_os=${unix_os} , apple_os=${apple_os} , windows_os=${windows_os}")

    set(mingw_build_env    "$<BOOL:${MINGW}>")    # is TRUE when using the MinGW compiler in Windows
    set(msys_build_env     "$<BOOL:${MSYS}>")     # is TRUE when using the MSYS developer environment in Windows
    set(cygwin_build_env   "$<BOOL:${CYGWIN}>")   # is TRUE on Windows when using the CygWin version of cmake
    add_custom_target(genexdebug_${PROJECT__NAME}_10 ALL COMMAND ${CMAKE_COMMAND} -E echo "**** ENV-flag: mingw_build_env=${mingw_build_env} , msys_build_env=${msys_build_env} , cygwin_build_env=${cygwin_build_env}")

    set(clang_cxx "$<COMPILE_LANG_AND_ID:CXX,Clang>")
    set(clang_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang>")
    set(gcc_cxx "$<COMPILE_LANG_AND_ID:CXX,GNU>")
    set(gcc_like_cxx "$<OR:$<COMPILE_LANG_AND_ID:CXX,GNU,LCC>,${clang_like_cxx}>")
    set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")
    message(STATUS "++++ COMPILE_LANG_AND_ID=${COMPILE_LANG_AND_ID}")
    # COMPILE_LANG_AND_ID can't be called in add_custom_target()
    #add_custom_target(genexdebug_${PROJECT__NAME}_11 ALL COMMAND ${CMAKE_COMMAND} -E echo "**** ENV: clang_cxx=${clang_cxx} , clang_like_cxx=${clang_like_cxx} , gcc_cxx=${gcc_cxx} , gcc_like_cxx=${gcc_like_cxx} , msvc_cxx=${msvc_cxx}")
endmacro()




#[===========[
  Check if compiler supports '-fmacro-prefix-map=old=new'  option
  refer ::: https://fossies.org/linux/bareos/core/CMakeLists.txt
  also refer the links  below ...
  https://reproducible-builds.org/docs/build-path/
  https://blog.conan.io/2019/09/02/Deterministic-builds-with-C-C++.html
  https://gcc.gnu.org/onlinedocs/gcc/Debugging-Options.html#index-fdebug-prefix-map
  https://gcc.gnu.org/onlinedocs/gcc/Preprocessor-Options.html#index-fmacro-prefix-map
  https://gcc.gnu.org/onlinedocs/gcc/Overall-Options.html#index-ffile-prefix-map
#]===========]
macro(project_check_cxx_compiler_flag_file_prefix_map
                    PROJECT__NAME               #  = converter
                    SCOPE                       #     {INTERFACE|PUBLIC|PRIVATE}
                    PROJECT_PREFIX)             #  = CONVERTER
    set(cxx_compiler_file_prefix_map OFF)
    if(CMAKE_BUILD_TYPE MATCHES "Coverage")
        message(STATUS "${PROJECT__NAME} : compiler option '-ffile-prefix-map=old=new' skipped in 'Coverage' build, as it messes the gcovr file paths during header file search")
    else()
        include(CheckCXXCompilerFlag)
        check_cxx_compiler_flag(-ffile-prefix-map=${CMAKE_CURRENT_SOURCE_DIR}=.
                                cxx_compiler_file_prefix_map
                               )
    endif()

    if(cxx_compiler_file_prefix_map)
        message(STATUS "${PROJECT__NAME} : compiler option '-ffile-prefix-map=old=new' SUPPORTED")
        target_compile_definitions( ${PROJECT__NAME} ${SCOPE}
                                    ${PROJECT_PREFIX}_USE_FILEPREFIXMAP=1)   # -> CONVERTER_USE_FILEPREFIXMAP=1

        ####  target_compile_options  causes coverage failure in consumer-project(rapidcsv_FilterSort)
        target_compile_options(${PROJECT__NAME} ${SCOPE}
            "-ffile-prefix-map=${CMAKE_CURRENT_SOURCE_DIR}${_path_separator}=")
    else()
        if(NOT CMAKE_BUILD_TYPE MATCHES "Coverage")
            # as of writing this code, clang does not support option '-ffile-prefix-map=...'
            message(STATUS "WARNING :: ${PROJECT__NAME} : compiler option '-ffile-prefix-map=old=new' NOT SUPPORTED")
        endif()
        string(LENGTH "${CMAKE_CURRENT_SOURCE_DIR}/" PROJECT_SOURCE_PATH_SIZE)
        target_compile_definitions( ${PROJECT__NAME} ${SCOPE}
                                    ${PROJECT_PREFIX}_USE_FILEPREFIXMAP=0     #  ->   CONVERTER_USE_FILEPREFIXMAP=0
        # https://stackoverflow.com/questions/8487986/file-macro-shows-full-path/40947954#40947954
                                    ${PROJECT_PREFIX}_SOURCE_PATH_SIZE=${PROJECT_SOURCE_PATH_SIZE})  # -> CONVERTER_SOURCE_PATH_SIZE=${PROJECT_SOURCE_PATH_SIZE}
    endif()
endmacro()



# Helper function to enable warnings
macro(project_enable_warnings
                    PROJECT__NAME
                    SCOPE)                      #     {INTERFACE|PUBLIC|PRIVATE}
    set(gcc_warnings "-Wextra;-Wpedantic;-Wshadow;-Wpointer-arith")
    set(gcc_warnings "${gcc_warnings};-Wcast-qual;-Wno-missing-braces;-Wswitch-default;-Wcast-align;-Winit-self")
    set(gcc_warnings "${gcc_warnings};-Wunreachable-code;-Wundef;-Wuninitialized;-Wold-style-cast;-Wwrite-strings")
    set(gcc_warnings "${gcc_warnings};-Wsign-conversion;-Weffc++")
    if(CMAKE_SYSTEM_NAME  EQUAL LINUX   AND   CMAKE_CXX_COMPILER_ID EQUAL Clang )
        if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 13.0 OR CMAKE_CXX_COMPILER_VERSION VERSION_LESS 16.0)
            set(gcc_warnings "${gcc_warnings};-Wno-defaulted-function-deleted")
        endif()
    endif()

    # https://cmake.org/cmake/help/v3.27/manual/cmake-generator-expressions.7.html
    #set(is_gnu "$<CXX_COMPILER_ID:GNU>")
    set(v5_or_later "$<VERSION_GREATER_EQUAL:$<CXX_COMPILER_VERSION>,5>")
    set(gcc_cxx_v5_or_later "$<AND:${gcc_cxx},${v5_or_later}>")
    set(windows_os_clang_cxx "$<AND:${windows_os},${clang_cxx}>")  # used when Windows-ClangCl toolchain

    #[==================================================================================[
    # we only want these warning flags to be used during builds.
    # Consumers of our installed project should not inherit our warning flags.
    # To specify this, we wrap our flags in a generator expression using the BUILD_INTERFACE condition.
    #]==================================================================================]
    target_compile_options(${PROJECT__NAME} ${SCOPE}
        "$<$<NOT:${msvc_cxx}>:$<BUILD_INTERFACE:${gcc_warnings}>>"
        "$<$<AND:${gcc_like_cxx},$<NOT:${windows_os_clang_cxx}>>:$<BUILD_INTERFACE:-Wall>>" # -Wall for 'windows_os_clang_cxx' gives lot of warnings
        "$<${gcc_cxx_v5_or_later}:$<BUILD_INTERFACE:-Wsuggest-override>>"
        "$<$<NOT:${windows_os}>:$<BUILD_INTERFACE:-g>>"  # for linux and macOS
        "$<$<AND:${windows_os},${gcc_cxx}>:$<BUILD_INTERFACE:-g>>"  # for g++ on windows
        "$<$<AND:${windows_os},$<NOT:${gcc_cxx}>>:$<BUILD_INTERFACE:-Z7>>"  # -Z7 is equivalent for -g
        #"$<${windows_os_clang_cxx}:$<BUILD_INTERFACE:-Wno-c++98-compat;-Wno-c++98-compat-pedantic>>"
        #"$<${windows_os_clang_cxx}:$<BUILD_INTERFACE:-Wno-global-constructors;-Wno-exit-time-destructors>>"
        #"$<${windows_os_clang_cxx}:$<BUILD_INTERFACE:-Wno-extra-semi-stmt;-Wno-string-plus-int>>"
        "$<${msvc_cxx}:$<BUILD_INTERFACE:-W4>>")
    #add_compile_options("/utf-8")  for msvc  -> check in cxxopts.cmake
endmacro()


