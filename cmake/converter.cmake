#
# URL:      https://github.com/panchaBhuta/converter
#
# Copyright (c) 2023-2023 Gautam Dhar
# All rights reserved.
#
# converter is distributed under the BSD 3-Clause license, see LICENSE for details.
#

if (CMAKE_VERSION VERSION_GREATER 3.10 OR CMAKE_VERSION VERSION_EQUAL 3.10)
    # Use include_guard() added in cmake 3.10
    include_guard()
endif()

include(CMakePackageConfigHelpers)

function(converter_getversion version_arg)
    # Parse the current version from the converter header
    file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/include/converter/converter.h" converter_version_defines
        REGEX "#define CONVERTER__VERSION_(MAJOR|MINOR|PATCH)")
    foreach(ver ${converter_version_defines})
        if(ver MATCHES "#define CONVERTER__VERSION_(MAJOR|MINOR|PATCH) +([^ ]+)$")
            set(CONVERTER__VERSION_${CMAKE_MATCH_1} "${CMAKE_MATCH_2}" CACHE INTERNAL "")
        endif()
    endforeach()
    set(VERSION ${CONVERTER__VERSION_MAJOR}.${CONVERTER__VERSION_MINOR}.${CONVERTER__VERSION_PATCH})

    # Give feedback to the user. Prefer DEBUG when available since large projects tend to have a lot
    # going on already
    if (CMAKE_VERSION VERSION_GREATER 3.15 OR CMAKE_VERSION VERSION_EQUAL 3.15)
        message(DEBUG "converter version ${VERSION}")
    else()
        message(STATUS "converter version ${VERSION}")
    endif()

    # Return the information to the caller
    set(${version_arg} ${VERSION} PARENT_SCOPE)
endfunction()

#[==================================================================================[
# Optionally, enable unicode support using the ICU library
function(converter_use_unicode)
    find_package(PkgConfig)
    pkg_check_modules(ICU REQUIRED icu-uc)

    target_link_libraries(converter INTERFACE ${ICU_LDFLAGS})
    target_compile_options(converter INTERFACE ${ICU_CFLAGS})
    target_compile_definitions(converter INTERFACE CONVERTER_USE_UNICODE)
endfunction()
#]==================================================================================]

# Request C++20 without gnu extension for the whole project and enable more warnings
macro(converter_set_cxx_standard)
    # DOUBT : Q  : where does CONVERTER_CXX_STANDARD get set?
    #         Ans: as this macro gets called only in standalone-mode,
    #              CONVERTER_CXX_STANDARD might be set on comman line
    #              while invoking cmake !
    if (CONVERTER_CXX_STANDARD)
        set(CMAKE_CXX_STANDARD ${CONVERTER_CXX_STANDARD})
    else()
        set(CMAKE_CXX_STANDARD 20)
    endif()

    #set(CMAKE_CXX_EXTENSIONS OFF)
endmacro()


#    Check if chrono-lib stream conversion is supported
macro(converter_check_chrono_stream_functionality)
    try_compile(USE_CHRONO_FROMSTREAM
                SOURCE_FROM_FILE   checkChrono_fromStream.cpp
                                   "${CMAKE_CURRENT_SOURCE_DIR}/cmake/checkChrono_fromStream.cpp"
                #CMAKE_FLAGS  "--std=gnu++2a -fconcepts"
                COMPILE_DEFINITIONS "-DUSE_CHRONO_FROMSTREAM=1"
                CXX_STANDARD "${CMAKE_CXX_STANDARD}"
                CXX_STANDARD_REQUIRED True)

    if(USE_CHRONO_FROMSTREAM)
        # msvc
        message(STATUS "checkChrono_fromStream[std::chrono::from_stream] ++SUCCESS++")
        message(STATUS "checkChrono_fromStream[date::from_stream] __SKIPPED__")
        set(USE_CHRONO_FROMSTREAM 1)
        set(USE_DATE_FROMSTREAM   0)
    else()
        message(STATUS "checkChrono_fromStream[std::chrono::from_stream] --FAILED--")
        set(USE_CHRONO_FROMSTREAM 0)
        set(USE_DATE_FROMSTREAM   1)
    endif()

    try_compile(USE_CHRONO_TOSTREAM
                SOURCE_FROM_FILE  checkChrono_toStream.cpp
                                  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/checkChrono_toStream.cpp"
                #CMAKE_FLAGS  "--std=gnu++2a -fconcepts"
                COMPILE_DEFINITIONS "-DUSE_CHRONO_TOSTREAM=1"
                CXX_STANDARD "${CMAKE_CXX_STANDARD}"
                CXX_STANDARD_REQUIRED True)

    if(USE_CHRONO_TOSTREAM)
        # for future reference, when <chrono> supports full functionality
        message(STATUS "checkChrono_toStream[std::chrono::to_stream] ++SUCCESS++")
        message(STATUS "checkChrono_toStream[date::to_stream] __SKIPPED__")
        set(USE_CHRONO_TOSTREAM 1)
        set(USE_DATE_TOSTREAM   0)
    else()
        message(STATUS "checkChrono_toStream[std::chrono::to_stream] --FAILED--")
        set(USE_CHRONO_TOSTREAM 0)
        set(USE_DATE_TOSTREAM   1)
    endif()

    if(USE_CHRONO_FROMSTREAM   AND   USE_CHRONO_TOSTREAM)
        message(STATUS "Using DATE_TIME-lib : <chrono>")
    else()
        set(DATELIB "date")  # local-variable
        # https://stackoverflow.com/questions/29892929/variables-set-with-parent-scope-are-empty-in-the-corresponding-child-scope-why
        #set(DATELIB ${DATELIB} PARENT_SCOPE)  # global-variable
        set_target_properties(converter PROPERTIES DATELIB ${DATELIB})  # global-variable
        message(STATUS "checking with DATE_TIME-lib : <${DATELIB}>")

        include( FetchContent )
        FetchContent_Declare( ${DATELIB}
                              GIT_REPOSITORY https://github.com/HowardHinnant/date.git
                              GIT_TAG        v3.0.1) # adjust tag/branch/commit as needed
        FetchContent_MakeAvailable(${DATELIB})

        #[==================[
        FetchContent_GetProperties( date ) #${DATELIB})
        if(NOT ${DATELIB}_POPULATED)
          FetchContent_Populate(${DATELIB})
        #  add_subdirectory(${${DATELIB}_SOURCE_DIR} ${${DATELIB}_BINARY_DIR} EXCLUDE_FROM_ALL)
        endif()
        #]==================]

        if(NOT USE_CHRONO_FROMSTREAM)
            try_compile(USE_DATE_FROMSTREAM
                        SOURCE_FROM_FILE   checkChrono_fromStream.cpp
                                           "${CMAKE_CURRENT_SOURCE_DIR}/cmake/checkChrono_fromStream.cpp"
                        #CMAKE_FLAGS  "--std=gnu++2a -fconcepts"
                        #CMAKE_FLAGS  "-DINCLUDE_DIRECTORIES=${${DATELIB}_SOURCE_DIR}"
                        CMAKE_FLAGS  "-DINCLUDE_DIRECTORIES=${CMAKE_BINARY_DIR}/_deps/date-src/include/"
                                    #"-DLINK_DIRECTORIES=${${DATELIB}_BINARY_DIR}"
                        COMPILE_DEFINITIONS "-DUSE_CHRONO_FROMSTREAM=0"
                        CXX_STANDARD "${CMAKE_CXX_STANDARD}"
                        CXX_STANDARD_REQUIRED True)

            if(USE_DATE_FROMSTREAM)
                # ubuntu, mac - clang
                message(STATUS "checkChrono_fromStream[date::from_stream] ++SUCCESS++")
                set(USE_DATE_FROMSTREAM   1)
            else()
                message(STATUS "checkChrono_fromStream[date::from_stream] --FAILED--")
                set(USE_DATE_FROMSTREAM   0)
            endif()
        endif()

        if(NOT USE_CHRONO_TOSTREAM)
            try_compile(USE_DATE_TOSTREAM
                        SOURCE_FROM_FILE  checkChrono_toStream.cpp
                                          "${CMAKE_CURRENT_SOURCE_DIR}/cmake/checkChrono_toStream.cpp"
                        #CMAKE_FLAGS  "--std=gnu++2a -fconcepts"
                        CMAKE_FLAGS  "-DINCLUDE_DIRECTORIES=${CMAKE_BINARY_DIR}/_deps/date-src/include/"
                        COMPILE_DEFINITIONS "-DUSE_CHRONO_TOSTREAM=0"
                        CXX_STANDARD "${CMAKE_CXX_STANDARD}"
                        CXX_STANDARD_REQUIRED True)

            if(USE_DATE_TOSTREAM)
                # ubuntu, mac - clang
                message(STATUS "checkChrono_toStream[date::to_stream] ++SUCCESS++")
                set(USE_DATE_TOSTREAM   1)
            else()
                # msvc
                message(STATUS "checkChrono_toStream[date::to_stream] --FAILED--")
                set(USE_DATE_TOSTREAM   0)
            endif()
        endif()

        if(USE_DATE_FROMSTREAM   OR   USE_DATE_TOSTREAM)
            message(STATUS "Using DATE_TIME-lib : <date/date.h>")
        else()
            unset(DATELIB PARENT_SCOPE)
        endif()
    endif()

    add_compile_definitions(USE_CHRONO_FROMSTREAM=${USE_CHRONO_FROMSTREAM})
    add_compile_definitions(USE_CHRONO_TOSTREAM=${USE_CHRONO_TOSTREAM})
    add_compile_definitions(USE_DATE_FROMSTREAM=${USE_DATE_FROMSTREAM})
    add_compile_definitions(USE_DATE_TOSTREAM=${USE_DATE_TOSTREAM})

    if((NOT USE_CHRONO_FROMSTREAM)   AND   (NOT USE_DATE_FROMSTREAM))
        message(STATUS "WARNING: Due to limitations of underlying libs, no definition is provided here. If needed, user might declare their own specific implementation of this method in their code base.")
    endif()

    if((NOT USE_CHRONO_TOSTREAM)   AND   (NOT USE_DATE_TOSTREAM))
        message(STATUS "WARNING: Due to limitations of underlying libs. The conversion here handles a limited sub-set of format specifiers.")
    endif()
endmacro()

# Check if compiler supports '-fmacro-prefix-map=old=new'  option
# refer ::: https://fossies.org/linux/bareos/core/CMakeLists.txt
macro(check_cxx_compiler_flag_macro_prefix_map)
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag(-fmacro-prefix-map=${CMAKE_CURRENT_SOURCE_DIR}=.
                            cxx_compiler_macro_prefix_map
                           )
    if(cxx_compiler_macro_prefix_map)
        message(STATUS "compiler option '-fmacro-prefix-map=old=new' SUPPORTED")
        add_compile_definitions(USE_MACROPREFIXMAP=1)

        set(CMAKE_CXX_FLAGS
            "${CMAKE_CXX_FLAGS} -fmacro-prefix-map=${CMAKE_CURRENT_SOURCE_DIR}${_path_separator}="
           )
    else()
        # as of writing this code, clang does not support option '-fmacro-prefix-map=...'
        message(STATUS "compiler option '-fmacro-prefix-map=old=new' NOT SUPPORTED")
        add_compile_definitions(USE_MACROPREFIXMAP=0)

        # https://stackoverflow.com/questions/8487986/file-macro-shows-full-path/40947954#40947954
        string(LENGTH "${CMAKE_CURRENT_SOURCE_DIR}/" CONVERTER_SOURCE_PATH_SIZE)
        add_compile_definitions(CONVERTER_SOURCE_PATH_SIZE=${CONVERTER_SOURCE_PATH_SIZE})
    endif()
endmacro()

# Helper function to enable warnings
macro(converter_enable_warnings)
    # https://cmake.org/cmake/help/v3.27/guide/tutorial/Adding%20Generator%20Expressions.html#cmakelists-txt-target-compile-options-genex
    set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
    set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")

    set(gcc_warnings "-g;-Wall;-Wextra;-Wpedantic;-Wshadow;-Wpointer-arith")
    set(gcc_warnings "${gcc_warnings};-Wcast-qual;-Wno-missing-braces;-Wswitch-default;-Wcast-align;-Winit-self")
    set(gcc_warnings "${gcc_warnings};-Wunreachable-code;-Wundef;-Wuninitialized;-Wold-style-cast;-Wwrite-strings")
    set(gcc_warnings "${gcc_warnings};-Wsign-conversion;-Weffc++")

    # https://cmake.org/cmake/help/v3.27/manual/cmake-generator-expressions.7.html
    set(is_gnu "$<CXX_COMPILER_ID:GNU>")
    set(v5_or_later "$<VERSION_GREATER_EQUAL:$<CXX_COMPILER_VERSION>,5>")
    set(is_gnu_v5_or_later "$<AND:${is_gnu},${v5_or_later}>")
    #[==================================================================================[
    # we only want these warning flags to be used during builds.
    # Consumers of our installed project should not inherit our warning flags.
    # To specify this, we wrap our flags in a generator expression using the BUILD_INTERFACE condition.
    #]==================================================================================]
    target_compile_options(converter INTERFACE
        "$<${gcc_like_cxx}:$<BUILD_INTERFACE:${gcc_warnings}>>"
        "$<${is_gnu_v5_or_later}:$<BUILD_INTERFACE:-Wsuggest-override>>"
        "$<${msvc_cxx}:$<BUILD_INTERFACE:-W4>>")
        #add_compile_options("/utf-8")  for msvc  -> check in cxxopts.cmake
endmacro()

# Helper function to configure, include, compile, link, build
macro(converter_build)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/_dateConfig.h.in
        ${CMAKE_CURRENT_BINARY_DIR}/include/converter/_dateConfig.h)

    # Build type
    set(DEFAULT_BUILD_TYPE "Release")
    if (CONVERTER_STANDALONE_PROJECT)
        set(DEFAULT_BUILD_TYPE "Debug")
    endif()
    if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
        message(STATUS "Using build type '${DEFAULT_BUILD_TYPE}' (default).")
        set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}")
    else()
        message(STATUS "Using build type '${CMAKE_BUILD_TYPE}'.")
    endif()

    #[==================================================================================[
    add_subdirectory(include)  ??? is it needed ; if so then with include/coverter
    # NOT needed, as target_include_directories() can be called here, instead of
    # being called from include/CMakeLists.txt (refer cxxopts which which does this)
    #]==================================================================================]

    #[==================================================================================[
    # refer https://cmake.org/cmake/help/v3.27/manual/cmake-buildsystem.7.html
    # The BUILD_INTERFACE expression wraps requirements which are only used when consumed
    # from a target in the same buildsystem, or when consumed from a target exported to
    # the build directory using the export() command. The INSTALL_INTERFACE expression
    # wraps requirements which are only used when consumed from a target which has been
    # installed and exported with the install(EXPORT) command
    #]==================================================================================]
    target_include_directories(converter INTERFACE
        # BUILD_INTERFACE: Content of ... when the property is exported using export(), or when the
        # target is used by another target in the same buildsystem. Expands to the empty string otherwise.
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        # https://cmake.org/cmake/help/v3.27/manual/cmake-packages.7.html#creating-relocatable-packages
        # INSTALL_INTERFACE: Content of ... when the property is exported using install(EXPORT), and empty otherwise.
        $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>)
    target_compile_definitions(converter INTERFACE
        $<$<CONFIG:Debug>:DEBUG_BUILD>
        $<$<CONFIG:Release>:RELEASE_BUILD>)
    target_compile_features(converter INTERFACE
        cxx_constexpr
        cxx_variadic_templates
        cxx_long_long_type)

    if(DATELIB)
        message(STATUS "'converter' linking to '${DATELIB}'")
        target_link_libraries(converter INTERFACE ${DATELIB})
    endif()
endmacro()

# Helper function to ecapsulate install logic
function(converter_install_logic)
    if(CMAKE_LIBRARY_ARCHITECTURE)
        string(REPLACE "/${CMAKE_LIBRARY_ARCHITECTURE}" "" CMAKE_INSTALL_LIBDIR_ARCHIND "${CMAKE_INSTALL_LIBDIR}")
    else()
        # On some systems (e.g. NixOS), `CMAKE_LIBRARY_ARCHITECTURE` can be empty
        set(CMAKE_INSTALL_LIBDIR_ARCHIND "${CMAKE_INSTALL_LIBDIR}")
    endif()
    set(CONVERTER_CMAKE_DIR "${CMAKE_INSTALL_LIBDIR_ARCHIND}/cmake/converter"
        CACHE STRING "Installation directory for cmake files, relative to ${CMAKE_INSTALL_PREFIX}.")
    # PROJECT_BINARY_DIR : the binary directory of the most recent project() command.
    set(version_config "${PROJECT_BINARY_DIR}/converter-config-version.cmake")
    set(project_config "${PROJECT_BINARY_DIR}/converter-config.cmake")
    #[==================================================================================[
    # Note :The installed <export-name>.cmake file may come with additional
    # per-configuration <export-name>-*.cmake files to be loaded by globbing.
    # Do not use an export name that is the same as the package name in
    # combination with installing a <package-name>-config.cmake file or the
    # latter may be incorrectly matched by the glob and loaded.
    # https://cmake.org/cmake/help/v3.27/command/install.html#export
    #]==================================================================================]
    set(targets_export_name converter-export)
    set(PackagingTemplatesDir "${PROJECT_SOURCE_DIR}/packaging")


    if(${CMAKE_VERSION} VERSION_GREATER "3.14")
        set(OPTIONAL_ARCH_INDEPENDENT "ARCH_INDEPENDENT")
    endif()

    # Generate the version, config and target files into the build directory.
    write_basic_package_version_file(
        ${version_config}
        VERSION       ${VERSION}
        COMPATIBILITY AnyNewerVersion
        ${OPTIONAL_ARCH_INDEPENDENT})
    #[==================================================================================[
    # configure_package_config_file() should be used instead of the plain configure_file()
    # command when creating the <PackageName>Config.cmake or <PackageName>-config.cmake file
    # for installing a project or library. It helps making the resulting package relocatable
    # by avoiding hardcoded paths in the installed Config.cmake file.
    # https://cmake.org/cmake/help/v3.27/module/CMakePackageConfigHelpers.html
    #]==================================================================================]
    configure_package_config_file(
        ${PackagingTemplatesDir}/converter-config.cmake.in
        ${project_config}
        INSTALL_DESTINATION ${CONVERTER_CMAKE_DIR})
    # https://cmake.org/cmake/help/v3.27/command/export.html#command:export
    # Export targets or packages for outside projects to use them
    # directly from the current project's build tree, without installation.
    export(TARGETS converter
        NAMESPACE  upSt_converter::
        FILE       ${PROJECT_BINARY_DIR}/${targets_export_name}.cmake)

    # Install version, config and target files.
    install(
        FILES       ${project_config} ${version_config}
        DESTINATION ${CONVERTER_CMAKE_DIR})

    # Add source to a target ( target_source -> cmake v3.23 )
    # File set(s) are defined here.
    target_sources(converter
        INTERFACE
        FILE_SET   converter_headers
        TYPE       HEADERS
        BASE_DIRS  include
        PRIVATE
        FILE_SET   converter_autoheader
        TYPE       HEADERS
        FILES      ${CMAKE_CURRENT_BINARY_DIR}/include/converter/_dateConfig.h)
    #[==================================================================================[
    https://cmake.org/cmake/help/v3.27/command/install.html#targets
    # Install the header files and export the target
    # EXPORT
    # This option associates(or defines) the installed target files with an export called <export-name>.
    # It must appear before any target options. To actually install the export file itself,
    # call install(EXPORT), documented below.
    # Here  ${targets_export_name} == <export-name>

    # If EXPORT is used and the targets include PUBLIC or INTERFACE file sets, all of them
    # must be specified with FILE_SET arguments. All PUBLIC or INTERFACE file sets associated
    # with a target are included in the export.
    #]==================================================================================]
    install(TARGETS      converter
        EXPORT           ${targets_export_name}
        FILE_SET         converter_headers
        DESTINATION      ${CMAKE_INSTALL_INCLUDEDIR}
        FILE_SET         converter_autoheader
        DESTINATION      ${CMAKE_INSTALL_INCLUDEDIR}/converter)
    #[==================================================================================[
    # https://cmake.org/cmake/help/v3.27/command/install.html#export
    # The EXPORT form generates and installs a CMake file containing code
    # to import targets from the installation tree into another project.
    # By default the generated file will be called <export-name>.cmake
    # but the FILE option may be used to specify a different name.

    # https://cmake.org/cmake/help/v3.27/manual/cmake-packages.7.html#creating-packages
    # This command generates the ClimbingStatsTargets.cmake file to contain IMPORTED
    # targets, suitable for use by downstreams and arranges to install it to
    # lib/cmake/ClimbingStats.
    # The generated ClimbingStatsConfigVersion.cmake and a cmake/ClimbingStatsConfig.cmake
    # are installed to the same location, completing the package.

    # A NAMESPACE with double-colons is specified when exporting the targets for installation.
    # This convention of double-colons gives CMake a hint that the name is an IMPORTED target
    # when it is used by downstreams with the target_link_libraries() command.
    # This way, CMake can issue a diagnostic if the package providing it has not yet been found.
    #]==================================================================================]
    install(EXPORT    ${targets_export_name}
        DESTINATION   ${CONVERTER_CMAKE_DIR}
        NAMESPACE     upStr_converter::)
    #[==================================================================================[
    export(EXPORT ${targets_export_name} ....)  not needed to be called, reason as per below.
    # https://cmake.org/cmake/help/v3.27/command/export.html#exporting-targets-matching-install-export
    # Creates a file <filename> that may be included by outside projects to import targets
    # from the current project's build tree. This is the same as the export(TARGETS) signature,
    # except that the targets are not explicitly listed. Instead, it exports the targets
    # associated with the installation export <export-name>.
    #]==================================================================================]

    # https://cmake.org/cmake/help/v3.27/command/install.html
    install(TARGETS         converter
        CONFIGURATIONS      Debug
        RUNTIME DESTINATION Debug/bin)
    install(TARGETS         converter
        CONFIGURATIONS      Release
        RUNTIME DESTINATION Release/bin)

    set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
    set(CPACK_PACKAGE_VENDOR "converter developers")
    set(CPACK_PACKAGE_DESCRIPTION "${PROJECT_DESCRIPTION}")
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
    set(CPACK_PACKAGE_VERSION_MAJOR "${converter_VERSION_MAJOR}")
    set(CPACK_PACKAGE_VERSION_MINOR "${converter_VERSION_MINOR}")
    set(CPACK_SOURCE_GENERATOR "TGZ")

    set(CPACK_DEBIAN_PACKAGE_NAME "${CPACK_PACKAGE_NAME}")
    set(CPACK_RPM_PACKAGE_NAME "${CPACK_PACKAGE_NAME}")
    set(CPACK_PACKAGE_HOMEPAGE_URL "${PROJECT_HOMEPAGE_URL}")
    set(CPACK_PACKAGE_MAINTAINER "${CPACK_PACKAGE_VENDOR}")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${CPACK_PACKAGE_MAINTAINER}")
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
    set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

    set(CPACK_DEBIAN_PACKAGE_NAME "lib${PROJECT_NAME}-dev")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6-dev")
    set(CPACK_DEBIAN_PACKAGE_SUGGESTS "cmake, pkg-config, pkg-conf")

    set(CPACK_RPM_PACKAGE_NAME "lib${PROJECT_NAME}-devel")
    set(CPACK_RPM_PACKAGE_SUGGESTS "${CPACK_DEBIAN_PACKAGE_SUGGESTS}")

    set(CPACK_DEB_COMPONENT_INSTALL ON)
    set(CPACK_RPM_COMPONENT_INSTALL ON)
    set(CPACK_NSIS_COMPONENT_INSTALL ON)
    set(CPACK_DEBIAN_COMPRESSION_TYPE "xz")

    set(PKG_CONFIG_FILE_NAME "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc")
    configure_file("${PackagingTemplatesDir}/pkgconfig.pc.in" "${PKG_CONFIG_FILE_NAME}" @ONLY)
    install(FILES "${PKG_CONFIG_FILE_NAME}"
            DESTINATION "${CMAKE_INSTALL_LIBDIR_ARCHIND}/pkgconfig")

    include(CPack)

    # https://cmake.org/cmake/help/v3.27/command/export.html#exporting-packages
    #export(PACKAGE <PackageName>) ????? not needed for now as this deals with package-registry
endfunction()
