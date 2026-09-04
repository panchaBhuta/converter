#
# URL:      https://github.com/panchaBhuta/converter
#
# Copyright (c) 2023-2026 Gautam Dhar
# All rights reserved.
#
# converter is distributed under the BSD 3-Clause license, see LICENSE for details.
#




# Helper function to configure, include, compile, link, build
macro(converter_build)
    if(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/include/converter/_workaroundConfig.h)
        message(STATUS "Config file exists: ${CMAKE_CURRENT_BINARY_DIR}/include/converter/_workaroundConfig.h")
    else()
        message(STATUS "creating Config file: ${CMAKE_CURRENT_BINARY_DIR}/include/converter/_workaroundConfig.h")
        configure_file(
            ${CMAKE_CURRENT_SOURCE_DIR}/cmake/_workaroundConfig.h.in
            ${CMAKE_CURRENT_BINARY_DIR}/include/converter/_workaroundConfig.h)
    endif()

    #[==================================================================================[
    # refer https://cmake.org/cmake/help/v3.27/manual/cmake-buildsystem.7.html
    # The BUILD_INTERFACE expression wraps requirements which are only used when consumed
    # from a target in the same buildsystem, or when consumed from a target exported to
    # the build directory using the export() command. The INSTALL_INTERFACE expression
    # wraps requirements which are only used when consumed from a target which has been
    # installed and exported with the install(EXPORT) command
    #]==================================================================================]
    target_include_directories( converter INTERFACE
        # BUILD_INTERFACE: Content of ... when the property is exported using export(), or when the
        # target is used by another target in the same buildsystem. Expands to the empty string otherwise.
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>  # for  /converter/_workaroundConfig.h
        # https://cmake.org/cmake/help/v3.27/manual/cmake-packages.7.html#creating-relocatable-packages
        # INSTALL_INTERFACE: Content of ... when the property is exported using install(EXPORT), and empty otherwise.
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}> )

    set(ENV_MSYSTEM "$ENV{MSYSTEM}")
    add_custom_target(genexdebug_converter_12 ALL COMMAND ${CMAKE_COMMAND} -E echo "**** ENV_MSYSTEM=${ENV_MSYSTEM}  , windows_os=${windows_os}")
    # IMPORTANT : below code doesn't work as expected as "if($<BOOL:${windows_os})" is evaluated
    #             during Configure time, during which windows_os and WIN32 among other variables
    #             would evaluate to BOOL=false. Thus, MSYSTEM_VALUE="MSYSTEM_NOTAPPLICABLE_NonWindowsOS"
    #             gets set incorrectly in configure time itself(instead of compile time).
    #             These variables(WIN32 etc) are set later during Compile time, but then this if
    #             condition doen't get evaluted.
    #             For MSYSTEM_VALUE to be correctly evaluted we need to set it thru generator-expressions.
    #if($<BOOL:${windows_os})
    #    message(STATUS "windows_os=true")
    #    if($<BOOL:${ENV_MSYSTEM}>)
    #        set(MSYSTEM_VALUE "MSYSTEM_${ENV_MSYSTEM}")
    #    else()
    #        set(MSYSTEM_VALUE "MSYSTEM_NOTSET")
    #    endif()
    #else()
    #    message(STATUS "windows_os=false")
    #    set(MSYSTEM_VALUE "MSYSTEM_NOTAPPLICABLE_NonWindowsOS")
    #endif()
    set(MSYSTEM_VALUE_WIN32 "$<IF:$<BOOL:${ENV_MSYSTEM}>,MSYSTEM_${ENV_MSYSTEM},MSYSTEM_NOTSET>")
    set(MSYSTEM_VALUE "$<IF:$<BOOL:${windows_os}>,${MSYSTEM_VALUE_WIN32},MSYSTEM_NOTAPPLICABLE_NonWindowsOS>")
    add_custom_target(genexdebug_converter_13 ALL COMMAND ${CMAKE_COMMAND} -E echo "**** MSYSTEM_VALUE=${MSYSTEM_VALUE}")
    target_compile_definitions(converter INTERFACE
        $<$<CONFIG:Debug>:DEBUG_BUILD>
        $<$<CONFIG:Release>:RELEASE_BUILD>
        FLAG_CONVERTER_debug_log=$<BOOL:${OPTION_CONVERTER_debug_log}>
        # below variables are useful for Windows GNU build environment
        "${MSYSTEM_VALUE}"  "COMPILER_${CMAKE_CXX_COMPILER_ID}"
        $<$<BOOL:mingw_build_env>:BUILD_ENV_MINGW>
        $<$<BOOL:msys_build_env>:BUILD_ENV_MSYS>
        $<$<BOOL:cygwin_build_env>:BUILD_ENV_CYGWIN>)

    target_compile_features(converter INTERFACE
        cxx_constexpr
        cxx_lambdas
        cxx_auto_type
        cxx_strong_enums
        cxx_variadic_templates
        cxx_long_long_type)

    if(DATELIB)
        message(STATUS "'converter' linking to '${DATELIB}'")
        #[======================[
        # https://cmake.org/cmake/help/latest/command/target_link_libraries.html#libraries-for-both-a-target-and-its-dependents
        # Library dependencies are transitive by default with this signature. When this target is linked into another target
        # then the libraries linked to this target will appear on the link line for the other target too.
        #
        # https://cmake.org/cmake/help/latest/command/target_link_libraries.html#libraries-for-a-target-and-or-its-dependents
        # The PUBLIC, PRIVATE and INTERFACE scope keywords can be used to specify both the
        # link dependencies and the link interface in one command.
        #
        # when <target> is a library (i.e NOT an executable), then specify the scope.
            target_link_libraries(<target>
                        <PRIVATE|PUBLIC|INTERFACE> <item>...
                        [<PRIVATE|PUBLIC|INTERFACE> <item>...]...)
        #]======================]
        target_link_libraries(converter INTERFACE ${DATELIB})
    endif()

    # adding header sources just helps IDEs
    #[==================================================================================[
    # Add source to a target ( target_source -> cmake v3.23 )
    # File set(s) are defined here.
    # https://cmake.org/cmake/help/v3.27/command/target_sources.html#file-sets
    #]==================================================================================]
    target_sources(
        converter INTERFACE
        FILE_SET   converter_headers
        TYPE       HEADERS
#        BASE_DIRS  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include> $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>  #include
        FILES
            $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>/specializedTypes/CompTimeStr.h
            $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>/specializedTypes/date.h
            $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>/specializedTypes/RegexString.hpp
            $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>/specializedTypes/RegexString.h
            $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>/specializedTypes/case_insensitive_string.h
            $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>/converter/converter.h
            $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>/converter/_common.h
            $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>/converter/_convertStr2T.h
            $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>/converter/_convertT2Str.h
            $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>/converter/_convertDate.h
            $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>/converter/_convertDateWorkaround.h
            $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>/converter/_convertTuple.h
            $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>/converter/specializedHelper/_case_insensitive_string.h
            $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>/converter/specializedHelper/_dateFormat.h
            $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>/commonUtils/platform.h
    )
endmacro(converter_build)

