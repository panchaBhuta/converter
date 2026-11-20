
set(DIAGNOSTIC_LDB_DENORM_MIN_FILE          "${DIAGNOSTIC_RESULT_FOLDER}/check_ldb_denorm_min.txt")


function(check_ldb_denorm_min FUNCTION_RETURN)
    get_os_description(OS_DESCRIPTION)
    get_compiler_description(COMPILER_DESCRIPTION)

    try_run    (RUN_RESULT_LDB_DENORM_MIN
                COMPILE_RESULT_LDB_DENORM_MIN
                SOURCE_FROM_FILE    check_ldb_denorm_min.cpp
                                    "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/check_ldb_denorm_min.cpp"
                #COMPILE_DEFINITIONS "-DTEMPLATE_CONSTRAINT=${LDB_DENORM_MIN}"
                ARGS "'${OS_DESCRIPTION}'" "${COMPILER_DESCRIPTION}"    # OS COMPILER argument to compiled binary
                CXX_STANDARD "${CMAKE_CXX_STANDARD}"
                CXX_STANDARD_REQUIRED True
                LOG_DESCRIPTION "run-check: check_ldb_denorm_min.cpp"
                COMPILE_OUTPUT_VARIABLE  TRY_COMPILE_OUTPUT_TTP
                #RUN_OUTPUT_VARIABLE      TRY_RUN_OUTPUT_TTP)     # return 0;
                RUN_OUTPUT_STDOUT_VARIABLE LOG_RUN_STDOUT_TTP
                RUN_OUTPUT_STDERR_VARIABLE LOG_RUN_STDERR_TTP)
    message(STATUS "check_ldb_denorm_min compile output : ${COMPILE_RESULT_LDB_DENORM_MIN}")
    message(STATUS "check_ldb_denorm_min run     output : ${RUN_RESULT_LDB_DENORM_MIN}")


    if( COMPILE_RESULT_LDB_DENORM_MIN  AND
        ( ( NOT RUN_RESULT_LDB_DENORM_MIN   STREQUAL  "FAILED_TO_RUN" ) AND
          (     RUN_RESULT_LDB_DENORM_MIN   EQUAL 0    OR
	        RUN_RESULT_LDB_DENORM_MIN   EQUAL 2 )
        )
      )
        message(STATUS "check_ldb_denorm_min ::  ++SUCCESS++\n${LOG_RUN_STDOUT_TTP}\n${LOG_RUN_STDERR_TTP}")
        file(WRITE "${DIAGNOSTIC_LDB_DENORM_MIN_FILE}" "${LOG_RUN_STDOUT_TTP}")
        set(${FUNCTION_RETURN}    ${RUN_RESULT_LDB_DENORM_MIN} PARENT_SCOPE)
    else()
        message(STATUS "check_ldb_denorm_min ::  --FAILED--")
        message(STATUS "check_ldb_denorm_min ::  ---------std::cout--------")
        message(STATUS "check_ldb_denorm_min ::  ${LOG_RUN_STDOUT_TTP}\n${LOG_RUN_STDERR_TTP}")
        message(STATUS "check_ldb_denorm_min ::  ---------std::cerr--------")
        message(STATUS "check_ldb_denorm_min ::  ${LOG_RUN_STDERR_TTP}")
        #set(${FUNCTION_RETURN}    ${e_DISABLE_FEATURE} PARENT_SCOPE)
    endif()
endfunction()
