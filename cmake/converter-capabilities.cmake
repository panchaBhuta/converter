#
# URL:      https://github.com/panchaBhuta/converter
#
# Copyright (c) 2023-2026 Gautam Dhar
# All rights reserved.
#
# converter is distributed under the BSD 3-Clause license, see LICENSE for details.
#










function(fetch_DATELIB)
    include( FetchContent )
    FetchContent_Declare( ${DATELIB}
                          GIT_REPOSITORY https://github.com/HowardHinnant/date.git
                          GIT_TAG        v3.0.4) # adjust tag/branch/commit as needed
    FetchContent_MakeAvailable(${DATELIB})

    #[==================[
    FetchContent_GetProperties( date ) #${DATELIB})
    if(NOT ${DATELIB}_POPULATED)
      FetchContent_Populate(${DATELIB})
    #  add_subdirectory(${${DATELIB}_SOURCE_DIR} ${${DATELIB}_BINARY_DIR} EXCLUDE_FROM_ALL)
    endif()
    #]==================]
endfunction()





#    Check if chrono-lib stream conversion is supported
function(check_chrono_stream_functionality)
    get_macro_value( ${CMAKE_CURRENT_BINARY_DIR}/include/converter/_workaroundConfig.h
                    "SUPPORTED_DATE_LIB_FOR_FROMSTREAM"
                     SUPPORTED_DATE_LIB_FOR_FROMSTREAM)
    message(STATUS "previous-build check for SUPPORTED_DATE_LIB_FOR_FROMSTREAM : ${SUPPORTED_DATE_LIB_FOR_FROMSTREAM}")

    get_macro_value( ${CMAKE_CURRENT_BINARY_DIR}/include/converter/_workaroundConfig.h
                    "SUPPORTED_DATE_LIB_FOR_TOSTREAM"
                     SUPPORTED_DATE_LIB_FOR_TOSTREAM)
    message(STATUS "previous-build check for SUPPORTED_DATE_LIB_FOR_TOSTREAM : ${SUPPORTED_DATE_LIB_FOR_TOSTREAM}")

    if(   (NOT SUPPORTED_DATE_LIB_FOR_FROMSTREAM   STREQUAL   "NOTFOUND") AND
          (NOT SUPPORTED_DATE_LIB_FOR_TOSTREAM     STREQUAL   "NOTFOUND")  )
        set(DATE_LIBRARY_DEPENDENCY_S2D    ${SUPPORTED_DATE_LIB_FOR_FROMSTREAM} PARENT_SCOPE)
        set(DATE_LIBRARY_DEPENDENCY_D2S    ${SUPPORTED_DATE_LIB_FOR_TOSTREAM}   PARENT_SCOPE)

        if( ( SUPPORTED_DATE_LIB_FOR_FROMSTREAM   STREQUAL "e_DATE_HHDATE" ) OR
            ( SUPPORTED_DATE_LIB_FOR_TOSTREAM     STREQUAL "e_DATE_HHDATE" )  )
            set(DATELIB "date")  # local-variable
            set(DATELIB ${DATELIB} PARENT_SCOPE)  # propagate to parent scope
            message(STATUS "Using DATE_TIME-lib : <date/date.h> as per previous build checks SUPPORTED_DATE_LIB_FOR_FROMSTREAM=${SUPPORTED_DATE_LIB_FOR_FROMSTREAM} ; SUPPORTED_DATE_LIB_FOR_TOSTREAM=${SUPPORTED_DATE_LIB_FOR_TOSTREAM}")

            fetch_DATELIB()
        endif()
        return()
    endif()

    set(DATE_LIBRARY_DEPENDENCY_S2D   "e_DATE_NO_LIB" PARENT_SCOPE)
    set(DATE_LIBRARY_DEPENDENCY_D2S   "e_DATE_NO_LIB" PARENT_SCOPE)


    # Platform check variables
    # https://cmake.org/cmake/help/latest/variable/CMAKE_HOST_SYSTEM_NAME.html#variable:CMAKE_HOST_SYSTEM_NAME
    # https://gitlab.kitware.com/cmake/community/-/wikis/doc/tutorials/How-To-Write-Platform-Checks#platform-variables
    # https://gitlab.kitware.com/cmake/community/-/wikis/doc/tutorials/How-To-Write-Platform-Checks#cmake-system
    set(linux_host "$<CMAKE_HOST_SYSTEM_NAME:Linux>")
    set(linux_host_with_gcc_like_cxx "$<AND:${linux_host},${gcc_like_cxx}>")
    set(linux_host_with_gcc_cxx "$<AND:${linux_host},${gcc_cxx}>")

    try_compile(COMPILE_RESULT_CHRONO_FROMSTREAM
                SOURCE_FROM_FILE    checkChrono_fromStream.cpp
                                    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/checkChrono_fromStream.cpp"
                COMPILE_DEFINITIONS "-DSUPPORTED_DATE_LIB_FOR_FROMSTREAM=e_DATE_CHRONO"
                CXX_STANDARD "${CMAKE_CXX_STANDARD}"
                CXX_STANDARD_REQUIRED True
                LOG_DESCRIPTION "compiler-check: checkChrono_fromStream.cpp : -DSUPPORTED_DATE_LIB_FOR_FROMSTREAM=e_DATE_CHRONO"
                OUTPUT_VARIABLE TRY_COMPILE_OUTPUT)
    message(STATUS ${TRY_COMPILE_OUTPUT})

    if(COMPILE_RESULT_CHRONO_FROMSTREAM)
        # for future reference, when <chrono> supports full functionality
        message(STATUS "checkChrono_fromStream[iss >> std::chrono::parse(fmt, ymd)] ++SUCCESS++")
        message(STATUS "checkChrono_fromStream[date::from_stream()] __SKIPPED__")
        set(DATE_LIBRARY_DEPENDENCY_S2D   "e_DATE_CHRONO" PARENT_SCOPE)
    else()
        message(STATUS "checkChrono_fromStream[iss >> std::chrono::from_stream()] --FAILED--")
    endif()

    try_compile(COMPILE_RESULT_CHRONO_TOSTREAM
                SOURCE_FROM_FILE    checkChrono_toStream.cpp
                                    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/checkChrono_toStream.cpp"
                COMPILE_DEFINITIONS "-DSUPPORTED_DATE_LIB_FOR_TOSTREAM=e_DATE_CHRONO"
                CXX_STANDARD "${CMAKE_CXX_STANDARD}"
                CXX_STANDARD_REQUIRED True
                LOG_DESCRIPTION "compiler-check: checkChrono_toStream.cpp : -DSUPPORTED_DATE_LIB_FOR_TOSTREAM=e_DATE_CHRONO"
                OUTPUT_VARIABLE TRY_COMPILE_OUTPUT)
    message(STATUS ${TRY_COMPILE_OUTPUT})

    if(COMPILE_RESULT_CHRONO_TOSTREAM)
        # for future reference, when <chrono> supports full functionality
        message(STATUS "checkChrono_toStream[oss << std::vformat()] ++SUCCESS++")
        message(STATUS "checkChrono_toStream[date::to_stream()] __SKIPPED__")
        set(DATE_LIBRARY_DEPENDENCY_D2S   "e_DATE_CHRONO" PARENT_SCOPE)
    else()
        message(STATUS "checkChrono_toStream[oss << std::vformat()] --FAILED--")
    endif()

    if(COMPILE_RESULT_CHRONO_FROMSTREAM   OR   COMPILE_RESULT_CHRONO_TOSTREAM)
        message(STATUS "Using DATE_TIME-lib : <chrono>")
    endif()

    if((NOT COMPILE_RESULT_CHRONO_FROMSTREAM)   OR   (NOT COMPILE_RESULT_CHRONO_TOSTREAM))
        ########## datelib start  ###############
        set(DATELIB "date")  # local-variable
        message(STATUS "checking with DATE_TIME-lib : <${DATELIB}>")

        fetch_DATELIB()
        ########## datelib end  ###############

        if(NOT COMPILE_RESULT_CHRONO_FROMSTREAM)
            try_compile(COMPILE_RESULT_DATE_FROMSTREAM
                        SOURCE_FROM_FILE    checkChrono_fromStream.cpp
                                            "${CMAKE_CURRENT_SOURCE_DIR}/cmake/checkChrono_fromStream.cpp"
                        #CMAKE_FLAGS  "-DINCLUDE_DIRECTORIES=${${DATELIB}_SOURCE_DIR}"
                        CMAKE_FLAGS  "-DINCLUDE_DIRECTORIES=${CMAKE_BINARY_DIR}/_deps/date-src/include/"
                                    #"-DLINK_DIRECTORIES=${${DATELIB}_BINARY_DIR}"
                        COMPILE_DEFINITIONS "-DSUPPORTED_DATE_LIB_FOR_FROMSTREAM=e_DATE_HHDATE"
                        CXX_STANDARD "${CMAKE_CXX_STANDARD}"
                        CXX_STANDARD_REQUIRED True
                        LOG_DESCRIPTION "compiler-check: checkChrono_fromStream.cpp : -DSUPPORTED_DATE_LIB_FOR_FROMSTREAM=e_DATE_HHDATE"
                        OUTPUT_VARIABLE TRY_COMPILE_OUTPUT)
            message(STATUS ${TRY_COMPILE_OUTPUT})

            if(COMPILE_RESULT_DATE_FROMSTREAM)
                # ubuntu, mac - clang
                message(STATUS "checkChrono_fromStream[date::from_stream()] ++SUCCESS++")
                set(DATE_LIBRARY_DEPENDENCY_S2D "e_DATE_HHDATE" PARENT_SCOPE)
            else()
                message(STATUS "checkChrono_fromStream[date::from_stream()] --FAILED--")
                set(DATE_LIBRARY_DEPENDENCY_S2D "e_DATE_NO_LIB" PARENT_SCOPE)
            endif()
        endif()

        if(NOT COMPILE_RESULT_CHRONO_TOSTREAM)
            try_compile(COMPILE_RESULT_DATE_TOSTREAM
                        SOURCE_FROM_FILE    checkChrono_toStream.cpp
                                            "${CMAKE_CURRENT_SOURCE_DIR}/cmake/checkChrono_toStream.cpp"
                        CMAKE_FLAGS  "-DINCLUDE_DIRECTORIES=${CMAKE_BINARY_DIR}/_deps/date-src/include/"
                        COMPILE_DEFINITIONS "-DSUPPORTED_DATE_LIB_FOR_TOSTREAM=e_DATE_HHDATE"
                        CXX_STANDARD "${CMAKE_CXX_STANDARD}"
                        CXX_STANDARD_REQUIRED True
                        LOG_DESCRIPTION "compiler-check: checkChrono_toStream.cpp : -DSUPPORTED_DATE_LIB_FOR_TOSTREAM=e_DATE_HHDATE"
                        OUTPUT_VARIABLE TRY_COMPILE_OUTPUT)
            message(STATUS ${TRY_COMPILE_OUTPUT})

            if(COMPILE_RESULT_DATE_TOSTREAM)
                # ubuntu, mac - clang
                message(STATUS "checkChrono_toStream[date::to_stream()] ++SUCCESS++")
                set(DATE_LIBRARY_DEPENDENCY_D2S "e_DATE_HHDATE" PARENT_SCOPE)
            else()
                # msvc
                message(STATUS "checkChrono_toStream[date::to_stream()] --FAILED--")
                set(DATE_LIBRARY_DEPENDENCY_D2S "e_DATE_NO_LIB" PARENT_SCOPE)
            endif()
        endif()

        if(COMPILE_RESULT_DATE_FROMSTREAM   OR   COMPILE_RESULT_DATE_TOSTREAM)
            message(STATUS "Using DATE_TIME-lib : <date/date.h>")
            set(DATELIB ${DATELIB} PARENT_SCOPE)  # propagate to parent scope
        else()
            unset(DATELIB PARENT_SCOPE)  # propagate to parent scope
        endif()

    endif()

endfunction()


# Failure to compile std::u16string from libstdc++ 12.1 in c++20 mode #55560 
# https://github.com/llvm/llvm-project/issues/55560
function(check_clang_string_workaround)
    get_macro_value( ${CMAKE_CURRENT_BINARY_DIR}/include/converter/_workaroundConfig.h
                    "TEST_HAS_CLANG_STRING_1"
                     TEST_HAS_CLANG_STRING_1)
    message(STATUS "previous-build check for TEST_HAS_CLANG_STRING_1 : ${TEST_HAS_CLANG_STRING_1}")

    get_macro_value( ${CMAKE_CURRENT_BINARY_DIR}/include/converter/_workaroundConfig.h
                    "TEST_WORKAROUND_CLANG_STRING_2"
                     TEST_WORKAROUND_CLANG_STRING_2)
    message(STATUS "previous-build check for TEST_WORKAROUND_CLANG_STRING_2 : ${TEST_WORKAROUND_CLANG_STRING_2}")

    if(   (NOT TEST_HAS_CLANG_STRING_1       STREQUAL "NOTFOUND") AND
          (NOT TEST_WORKAROUND_CLANG_STRING_2  STREQUAL "NOTFOUND")  )
        set(TEST_HAS_CLANG_STRING_1      ${TEST_HAS_CLANG_STRING_1}      PARENT_SCOPE)
        set(TEST_WORKAROUND_CLANG_STRING_2 ${TEST_WORKAROUND_CLANG_STRING_2} PARENT_SCOPE)
        return()
    endif()

    set(TEST_HAS_CLANG_STRING_1       ${e_ENABLE_FEATURE} PARENT_SCOPE)
    set(TEST_WORKAROUND_CLANG_STRING_2  ${e_DISABLE_FEATURE} PARENT_SCOPE)

    if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
        try_compile(COMPILE_RESULT_CLANG_STRING_DEFAULT
                    SOURCE_FROM_FILE    check_clang_string.cpp
                                        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/check_clang_string.cpp"
                    COMPILE_DEFINITIONS "-DTEST_WORKAROUND_CLANG_STRING_2=${e_DISABLE_FEATURE}"
                    CXX_STANDARD "${CMAKE_CXX_STANDARD}"
                    CXX_STANDARD_REQUIRED True
                    LOG_DESCRIPTION "compiler-check: check_clang_string.cpp : -DTEST_WORKAROUND_CLANG_STRING_2=${e_DISABLE_FEATURE}"
                    OUTPUT_VARIABLE TRY_COMPILE_OUTPUT)
        message(STATUS ${TRY_COMPILE_OUTPUT})

        if(COMPILE_RESULT_CLANG_STRING_DEFAULT)
            message(STATUS "check_clang_string :: default mode ++SUCCESS++")
            message(STATUS "check_clang_string :: workaround   __SKIPPED__")
            set(TEST_HAS_CLANG_STRING_1       ${e_ENABLE_FEATURE} PARENT_SCOPE)
            set(TEST_WORKAROUND_CLANG_STRING_2  ${e_DISABLE_FEATURE} PARENT_SCOPE)
        else()
            message(STATUS "check_clang_string :: default mode --FAILED--")

            try_compile(COMPILE_RESULT_CLANG_STRING_WORKAROUND
                        SOURCE_FROM_FILE    check_clang_string.cpp
                                            "${CMAKE_CURRENT_SOURCE_DIR}/cmake/check_clang_string.cpp"
                        COMPILE_DEFINITIONS "-DTEST_WORKAROUND_CLANG_STRING_2=${e_ENABLE_FEATURE}"
                        CXX_STANDARD "${CMAKE_CXX_STANDARD}"
                        CXX_STANDARD_REQUIRED True
                        LOG_DESCRIPTION "compiler-check: check_clang_string.cpp : -DTEST_WORKAROUND_CLANG_STRING_2=${e_ENABLE_FEATURE}"
                        OUTPUT_VARIABLE TRY_COMPILE_OUTPUT)
            message(STATUS ${TRY_COMPILE_OUTPUT})

            if(COMPILE_RESULT_CLANG_STRING_WORKAROUND)
                message(STATUS "check_clang_string :: workaround ++SUCCESS++")
                set(TEST_HAS_CLANG_STRING_1       ${e_DISABLE_FEATURE} PARENT_SCOPE)
                set(TEST_WORKAROUND_CLANG_STRING_2  ${e_ENABLE_FEATURE} PARENT_SCOPE)
            else()
                message(STATUS "check_clang_string :: workaround --FAILED--")
                set(TEST_HAS_CLANG_STRING_1       ${e_DISABLE_FEATURE} PARENT_SCOPE)
                set(TEST_WORKAROUND_CLANG_STRING_2  ${e_DISABLE_FEATURE} PARENT_SCOPE)
            endif()
        endif()

    else()
        message(STATUS "NON Clang compiler, default settings for TEST_HAS_CLANG_STRING_1")
    endif()
endfunction()



# ------------------------------------------------------------------------------
# check_compile_file()
#
# Compiles a C++ source file using try_compile() to determine whether a
# particular piece of C++ functionality is supported by the compiler.
#
# The source file is expected to be located in:
#
#     ${CMAKE_CURRENT_SOURCE_DIR}/cmake/
#
# FLOAT_TYPE is passed to the source file as the preprocessor definition:
#
#     FLOAT_TYPE=<FLOAT_TYPE>
#
# This allows the same compiler-check source file to be tested with different
# floating-point types, for example:
#
#     float
#     double
#     long double
#
# Parameters:
#
#   FILE_NAME
#       Name of the C++ source file containing the compiler check.
#
#   FLOAT_TYPE
#       Floating-point type to be passed to the source file through the
#       FLOAT_TYPE preprocessor definition.
#
#   COMPILE_RESULT
#       Name of the variable in the caller's scope that will receive the
#       result of try_compile().
#
#       TRUE  - compilation succeeded
#       FALSE - compilation failed
#
#   OUTPUT_VARIABLE
#       Name of the variable in the caller's scope that will receive the
#       diagnostic/output generated by try_compile().
#
# Example:
#
#   check_compile_file(
#       check_floatingPoint_fromChars.cpp
#       float
#       COMPILE_FLOATINGPOINT_FROM_CHARS
#       TRY_COMPILE_OUTPUT
#   )
#
#   if(COMPILE_FLOATINGPOINT_FROM_CHARS)
#       message(STATUS "Floating-point from_chars: supported")
#   else()
#       message(STATUS "Floating-point from_chars: NOT supported")
#       message(STATUS "${TRY_COMPILE_OUTPUT}")
#   endif()
#
# ------------------------------------------------------------------------------

function(check_compile_file FILE_NAME FLOAT_TYPE COMPILE_RESULT OUTPUT_VARIABLE)

    if(FLOAT_TYPE STREQUAL "long_double")
        set(FLOAT_TYPE_DEFINE "long double")
    else()
        set(FLOAT_TYPE_DEFINE "${FLOAT_TYPE}")
    endif()

    try_compile(RESULT
                SOURCE_FROM_FILE "${FILE_NAME}"
                                 "${CMAKE_CURRENT_SOURCE_DIR}/cmake/${FILE_NAME}"
                COMPILE_DEFINITIONS "-DFLOAT_TYPE=${FLOAT_TYPE_DEFINE}"
                CXX_STANDARD "${CMAKE_CXX_STANDARD}"
                CXX_STANDARD_REQUIRED True
                LOG_DESCRIPTION "compiler-check: ${FILE_NAME} [${FLOAT_TYPE}]"
                OUTPUT_VARIABLE TRY_COMPILE_OUTPUT)

    if(RESULT)
        message(STATUS "${FILE_NAME} [${FLOAT_TYPE}] :: ++SUCCESS++")
    else()
        message(STATUS "${FILE_NAME} [${FLOAT_TYPE}] :: --FAILED--")
    endif()

    set(${COMPILE_RESULT}
        "${RESULT}"
        PARENT_SCOPE)

    set(${OUTPUT_VARIABLE}
        "${TRY_COMPILE_OUTPUT}"
        PARENT_SCOPE)

endfunction()


# ------------------------------------------------------------------------------
# check_floatingPoint_types()
#
# Checks whether the specified source file successfully compiles for each of
# the supported floating-point types:
#
#     float
#     double
#     long double
#
# The source file is compiled once for each floating-point type using the
# check_compile_file() function.
#
# A bitmask is used to aggregate the results:
#
#     1 -> float       compilation succeeded
#     2 -> double      compilation succeeded
#     4 -> long double compilation succeeded
#
# Therefore:
#
#     0 -> none of the floating-point types are supported
#     1 -> float only
#     2 -> double only
#     3 -> float + double
#     4 -> long double only
#     5 -> float + long double
#     6 -> double + long double
#     7 -> all three types are supported
#
# Parameters:
#
#     FILE_NAME
#         Name of the C++ source file used for the compiler capability check.
#
#     AGGREGATE_RESULT
#         Name of the variable in the caller's scope that will receive the
#         resulting bitmask.
#
# Example:
#
#     check_floatingPoint_types(
#         check_floatingPoint_fromChars.cpp
#         AGGREGATE_RESULT
#     )
#
#     message(STATUS "Aggregate_result = ${AGGREGATE_RESULT}")
#
# ------------------------------------------------------------------------------

function(check_floatingPoint_types FILE_NAME AGGREGATE_RESULT)

    # Start with no floating-point type reported as supported.
    set(Aggregate_result 0)

    foreach(FLOAT_TYPE float double long_double)

        # Assign one bit to each floating-point type so that the individual
        # results can be combined into a single aggregate bitmask.
        if(FLOAT_TYPE STREQUAL "float")
            set(FLOAT_TYPE_BIT 1)

        elseif(FLOAT_TYPE STREQUAL "double")
            set(FLOAT_TYPE_BIT 2)

        elseif(FLOAT_TYPE STREQUAL "long_double")
            set(FLOAT_TYPE_BIT 4)

        endif()

        # Compile the test source using the current floating-point type.
        check_compile_file(
            ${FILE_NAME}
            ${FLOAT_TYPE}
            COMPILE_RESULT
            TRY_COMPILE_OUTPUT
        )

        # If compilation succeeded, set the corresponding bit in the
        # aggregate result.
        if(COMPILE_RESULT)
            math(EXPR Aggregate_result
                 "${Aggregate_result} + ${FLOAT_TYPE_BIT}")
        endif()

    endforeach()

    # Return the aggregate bitmask to the caller's scope.
    set(${AGGREGATE_RESULT}
        "${Aggregate_result}"
        PARENT_SCOPE)

    message(STATUS "${FILE_NAME} Aggregate_result = ${AGGREGATE_RESULT}")

endfunction()


# check if compiler supports "elementary string conversions" for floating-point types
# https://en.cppreference.com/w/cpp/compiler_support/17#C.2B.2B17_library_features
function(check_floatingPoint_elementaryStringConversions)
    get_macro_value( ${CMAKE_CURRENT_BINARY_DIR}/include/converter/_workaroundConfig.h
                    "HAS_FLOATINGPOINT_FROM_CHARS"
                     HAS_FLOATINGPOINT_FROM_CHARS)
    message(STATUS "previous-build check for HAS_FLOATINGPOINT_FROM_CHARS : ${HAS_FLOATINGPOINT_FROM_CHARS}")

    get_macro_value( ${CMAKE_CURRENT_BINARY_DIR}/include/converter/_workaroundConfig.h
                    "HAS_FLOATINGPOINT_TO_CHARS"
                     HAS_FLOATINGPOINT_TO_CHARS)
    message(STATUS "previous-build check for HAS_FLOATINGPOINT_TO_CHARS : ${HAS_FLOATINGPOINT_TO_CHARS}")

    get_macro_value( ${CMAKE_CURRENT_BINARY_DIR}/include/converter/_workaroundConfig.h
                    "ENABLE_FLOATINGPOINT_TO_STRING_1"
                     ENABLE_FLOATINGPOINT_TO_STRING_1)
    message(STATUS "previous-build check for ENABLE_FLOATINGPOINT_TO_STRING_1 : ${ENABLE_FLOATINGPOINT_TO_STRING_1}")

    if(   (NOT HAS_FLOATINGPOINT_FROM_CHARS  STREQUAL "NOTFOUND") AND
          (NOT HAS_FLOATINGPOINT_TO_CHARS    STREQUAL "NOTFOUND")  )
        set(HAS_FLOATINGPOINT_FROM_CHARS  ${HAS_FLOATINGPOINT_FROM_CHARS}  PARENT_SCOPE)
        set(HAS_FLOATINGPOINT_TO_CHARS    ${HAS_FLOATINGPOINT_TO_CHARS}    PARENT_SCOPE)
        return()
    endif()

    set(HAS_FLOATINGPOINT_FROM_CHARS  ${e_ENABLE_FEATURE} PARENT_SCOPE)
    set(HAS_FLOATINGPOINT_TO_CHARS    ${e_ENABLE_FEATURE} PARENT_SCOPE)

    check_floatingPoint_types( check_floatingPoint_fromChars.cpp AGGREGATE_RESULT_FLOATINGPOINT_FROM_CHARS )

    check_floatingPoint_types( check_floatingPoint_toChars.cpp   AGGREGATE_RESULT_FLOATINGPOINT_TO_CHARS )


    if(  AGGREGATE_RESULT_FLOATINGPOINT_FROM_CHARS  )
        message(STATUS "floatingPoint_fromChars algo ::  ENABLED")
        set(HAS_FLOATINGPOINT_FROM_CHARS   ${AGGREGATE_RESULT_FLOATINGPOINT_FROM_CHARS} PARENT_SCOPE)
    else()
        message(STATUS "WARNING :: floatingPoint_fromChars algo ::  DISABLED")
        set(HAS_FLOATINGPOINT_FROM_CHARS   ${e_DISABLE_FEATURE} PARENT_SCOPE)
    endif()

    if(  AGGREGATE_RESULT_FLOATINGPOINT_TO_CHARS  )
        message(STATUS "floatingPoint_toChars algo ::  ENABLED")
        set(HAS_FLOATINGPOINT_TO_CHARS     ${AGGREGATE_RESULT_FLOATINGPOINT_TO_CHARS} PARENT_SCOPE)
    else()
        message(STATUS "WARNING :: floatingPoint_toChars algo ::  DISABLED")
        set(HAS_FLOATINGPOINT_TO_CHARS     ${e_DISABLE_FEATURE} PARENT_SCOPE)
    endif()


    #  std::string strVal = std::to_string(value); // std::to_string(11.0f) -> "11.0000000" is not what is expected "11"
    try_run    (RUN_FLOATINGPOINT_TO_STRING  COMPILE_FLOATINGPOINT_TO_STRING
                SOURCE_FROM_FILE    check_floatingPoint_toString.cpp
                                    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/check_floatingPoint_toString.cpp"
                #COMPILE_DEFINITIONS "-DHAS_FLOATINGPOINT_FROM_CHARS=${e_ENABLE_FEATURE}"  not needed
                CXX_STANDARD "${CMAKE_CXX_STANDARD}"
                CXX_STANDARD_REQUIRED True
                LOG_DESCRIPTION "run-check: check_floatingPoint_toString.cpp"
                COMPILE_OUTPUT_VARIABLE  TRY_COMPILE_OUTPUT
                RUN_OUTPUT_VARIABLE      TRY_RUN_OUTPUT)     # return 0;
                #RUN_OUTPUT_STDOUT_VARIABLE LOG_RUN_STDOUT
                #RUN_OUTPUT_STDERR_VARIABLE LOG_RUN_STDERR)
    message(STATUS "check_floatingPoint_toString compile output : ${COMPILE_FLOATINGPOINT_TO_STRING}")
    message(STATUS "check_floatingPoint_toString run compile : ${RUN_FLOATINGPOINT_TO_STRING}")


    if( COMPILE_FLOATINGPOINT_TO_STRING  AND
        ( ( NOT RUN_FLOATINGPOINT_TO_STRING   STREQUAL  "FAILED_TO_RUN" ) AND
          (     RUN_FLOATINGPOINT_TO_STRING   EQUAL 0 )
        )
      )
        message(STATUS "check_floatingPoint_toString ::  ++SUCCESS++")
        message(STATUS "floatingPoint_toString algo ::  ENABLED")
        set(ENABLE_FLOATINGPOINT_TO_STRING_1    ${e_ENABLE_FEATURE} PARENT_SCOPE)
    else()
        message(STATUS "check_floatingPoint_toString ::  --FAILED--")
        message(STATUS "WARNING :: floatingPoint_toString algo ::  DISABLED")
        set(ENABLE_FLOATINGPOINT_TO_STRING_1    ${e_DISABLE_FEATURE} PARENT_SCOPE)
    endif()

    #[===[  for testing purpose
        set(HAS_FLOATINGPOINT_FROM_CHARS   ${e_DISABLE_FEATURE} PARENT_SCOPE)
        set(HAS_FLOATINGPOINT_TO_CHARS     ${e_DISABLE_FEATURE} PARENT_SCOPE)
        set(ENABLE_FLOATINGPOINT_TO_STRING_1    ${e_DISABLE_FEATURE} PARENT_SCOPE)
    #]===]
endfunction()

