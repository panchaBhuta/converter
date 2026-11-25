
set(DIAGNOSTIC_FLOATTYPE_BOUNDARY_FILE          "${DIAGNOSTIC_RESULT_FOLDER}/check_floatType_boundary.txt")



function(diagnostic_floatType_boundary FUNCTION_RETURN)
    get_os_description(OS_DESCRIPTION)
    get_compiler_description(COMPILER_DESCRIPTION)

    try_run    (RUN_RESULT_FLOATTYPE_BOUNDARY
                COMPILE_RESULT_FLOATTYPE_BOUNDARY
                SOURCE_FROM_FILE    diagnostic_floatType_boundary.cpp
                                    "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/diagnostic_floatType_boundary.cpp"
                CMAKE_FLAGS
                    "-DINCLUDE_DIRECTORIES:STRING=${PROJECT_BINARY_DIR}/include"
                    #"-DINCLUDE_DIRECTORIES:STRING=${PROJECT_SOURCE_DIR}/include;${PROJECT_BINARY_DIR}/include"
                #INCLUDE_DIRECTORIES    not supported by this version of cmake, using CMAKE_FLAGS instead
                    ##"${PROJECT_SOURCE_DIR}/include"    converter/_compilerWorkAround.h
                    #"${PROJECT_BINARY_DIR}/include"     #converter/_workaroundConfig.h
                #COMPILE_DEFINITIONS "-DTEMPLATE_CONSTRAINT=${FLOATTYPE_BOUNDARY}"
                ARGS "'${OS_DESCRIPTION}'" "${COMPILER_DESCRIPTION}"   # OS COMPILER argument to compiled binary
                CXX_STANDARD "${CMAKE_CXX_STANDARD}"
                CXX_STANDARD_REQUIRED True
                LOG_DESCRIPTION "run-check: diagnostic_floatType_boundary.cpp"
                COMPILE_OUTPUT_VARIABLE  TRY_COMPILE_OUTPUT_BDRY
                #RUN_OUTPUT_VARIABLE      TRY_RUN_OUTPUT_BDRY)     # return 0;
                RUN_OUTPUT_STDOUT_VARIABLE LOG_RUN_STDOUT_BDRY
                RUN_OUTPUT_STDERR_VARIABLE LOG_RUN_STDERR_BDRY)
    message(STATUS "diagnostic_floatType_boundary compile output : ${COMPILE_RESULT_FLOATTYPE_BOUNDARY}")
    message(STATUS "diagnostic_floatType_boundary run     output : ${RUN_RESULT_FLOATTYPE_BOUNDARY}")


    if( COMPILE_RESULT_FLOATTYPE_BOUNDARY  AND
        ( ( NOT RUN_RESULT_FLOATTYPE_BOUNDARY   STREQUAL  "FAILED_TO_RUN" ) OR
          (     RUN_RESULT_FLOATTYPE_BOUNDARY   EQUAL 0 )
        )
      )
        message(STATUS "diagnostic_floatType_boundary ::  ++SUCCESS++\n")
        file(APPEND "${DIAGNOSTIC_FLOATTYPE_BOUNDARY_FILE}" "${LOG_RUN_STDOUT_BDRY}")

        string(REGEX MATCHALL
               "CONSOLIDATED_RESULT_([a-z_0-9]+)=([0-9]+)"
               _match
               "${LOG_RUN_STDERR_BDRY}"
        )

        if(_match)
            set(${FUNCTION_RETURN} "${_match}" PARENT_SCOPE)
        else()
            message(WARNING "diagnostic_floatType_boundary ::  --FAILED-- :: Could not retrieve consolidated diagnostic result")
        endif()


    else()
        message(WARNING "diagnostic_floatType_boundary ::  --FAILED-- :: failure run diagnostic_floatType_boundary.cpp")
    endif()

    message(STATUS "diagnostic_floatType_boundary ::  ---------std::cout--------\n${LOG_RUN_STDOUT_BDRY}\n")
    message(STATUS "diagnostic_floatType_boundary ::  ---------std::cerr--------\n${LOG_RUN_STDERR_BDRY}")
    message(STATUS "diagnostic_floatType_boundary ::  ---------end--------\n")

endfunction()


function(boundry_result_extractor
        CONSOLIDATED_RESULT_FUN
	LIST_CONSOLIDATED_RESULT_FUN
        SUP_FUN_FLT
        SUP_FUN_DBL
        SUP_FUN_LDBL)

    set(SUPPORTED_FUN_ALL    0)

    string(REGEX MATCH
           "${CONSOLIDATED_RESULT_FUN}=([0-9]+)"
           _match
           "${LIST_CONSOLIDATED_RESULT_FUN}"
    )

    if(_match)
        set(FLAG_FUN_ALL "${CMAKE_MATCH_1}")
    else()
        message(WARNING "boundry_result_extractor :: --FAILED-- Could not retrieve consolidated diagnostic result")
    endif()

    message(STATUS "FLAG_FUN_ALL=${FLAG_FUN_ALL}")

    math(EXPR FLAG_FUN_FLT  "${FLAG_FUN_ALL} & 7")

    math(EXPR _FLAG_FUN_DBL "${FLAG_FUN_ALL} >> 3")
    math(EXPR FLAG_FUN_DBL  "${_FLAG_FUN_DBL} & 7")

    math(EXPR _FLAG_FUN_LDBL "${FLAG_FUN_ALL} >> 6")
    math(EXPR FLAG_FUN_LDBL "${_FLAG_FUN_LDBL} & 7")

    set(${SUP_FUN_FLT}  "${FLAG_FUN_FLT}"  PARENT_SCOPE)
    set(${SUP_FUN_DBL}  "${FLAG_FUN_DBL}"  PARENT_SCOPE)
    set(${SUP_FUN_LDBL} "${FLAG_FUN_LDBL}" PARENT_SCOPE)

    message(STATUS
        "${CONSOLIDATED_RESULT_FUN}: packed=${FLAG_FUN_ALL}; "
        "${SUP_FUN_FLT}=${FLAG_FUN_FLT}; "
        "${SUP_FUN_DBL}=${FLAG_FUN_DBL}; "
        "${SUP_FUN_LDBL}=${FLAG_FUN_LDBL}")

endfunction()


function(diagnostic_floatType_boundary_multi)
    get_os_description(OS_DESCRIPTION)
    get_compiler_description(COMPILER_DESCRIPTION)
    #message(STATUS "OS_DESCRIPTION=${OS_DESCRIPTION}")
    #message(STATUS "COMPILER_DESCRIPTION=${COMPILER_DESCRIPTION}")

    file(WRITE "${DIAGNOSTIC_FLOATTYPE_BOUNDARY_FILE}"
               "#OS,compiler,float-type,functionName,inputOriginal,inputCurated,flag_fromChars,flag_stoFP,flag_stream\n")

    diagnostic_floatType_boundary(LIST_CONSOLIDATED_RESULT_FUN)

    boundry_result_extractor(
        "CONSOLIDATED_RESULT_min"
        "${LIST_CONSOLIDATED_RESULT_FUN}"
        SUPPORTED_MIN_FLT
        SUPPORTED_MIN_DBL
        SUPPORTED_MIN_LDBL)

    boundry_result_extractor(
        "CONSOLIDATED_RESULT_max"
        "${LIST_CONSOLIDATED_RESULT_FUN}"
        SUPPORTED_MAX_FLT
        SUPPORTED_MAX_DBL
        SUPPORTED_MAX_LDBL)

    boundry_result_extractor(
        "CONSOLIDATED_RESULT_denorm_min"
        "${LIST_CONSOLIDATED_RESULT_FUN}"
        SUPPORTED_DENORM_MIN_FLT
        SUPPORTED_DENORM_MIN_DBL
        SUPPORTED_DENORM_MIN_LDBL)

    boundry_result_extractor(
        "CONSOLIDATED_RESULT_epsilon"
        "${LIST_CONSOLIDATED_RESULT_FUN}"
        SUPPORTED_EPS_FLT
        SUPPORTED_EPS_DBL
        SUPPORTED_EPS_LDBL)

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/_testSupportedConfig.h.in
        ${CMAKE_CURRENT_BINARY_DIR}/include/tests/_testSupportedConfig.h)

endfunction()

