#
# URL:      https://github.com/panchaBhuta/converter
#
# Copyright (c) 2026-2026 Gautam Dhar
# All rights reserved.
#
# converter is distributed under the BSD 3-Clause license, see LICENSE for details.
#

	#[===================================================================[
	   installation
	#]===================================================================]
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
    set(targets_export_name converterTargets)    # targets_export_name used in packaging/converter-config.cmake.in
    set(PackagingTemplatesDir "${PROJECT_SOURCE_DIR}/packaging")


    if(${CMAKE_VERSION} VERSION_GREATER "3.14")
        set(OPTIONAL_ARCH_INDEPENDENT "ARCH_INDEPENDENT")
    endif()





    include( CMakePackageConfigHelpers )
    # Generate the version, config and target files into the build directory.
    write_basic_package_version_file(
        ${version_config}
        VERSION       ${VERSION}
        #COMPATIBILITY AnyNewerVersion
        COMPATIBILITY SameMajorVersion
        ${OPTIONAL_ARCH_INDEPENDENT})
    #[==================================================================================[
    # configure_package_config_file() should be used instead of the plain configure_file()
    # command when creating the <PackageName>Config.cmake or <PackageName>-config.cmake file
    # for installing a project or library. It helps making the resulting package relocatable
    # by avoiding hardcoded paths in the installed Config.cmake file.
    # https://cmake.org/cmake/help/v3.27/module/CMakePackageConfigHelpers.html
    #]==================================================================================]
    configure_package_config_file(
        ${PackagingTemplatesDir}/converter-config.cmake.in      # donot change "converter-config.cmake" to ${project_config}
        ${project_config}
        INSTALL_DESTINATION ${CONVERTER_CMAKE_DIR})

    # Install version, config and target files.
    install(
        FILES       ${project_config} ${version_config}
        DESTINATION ${CONVERTER_CMAKE_DIR})








	if( CONVERTER_ENABLE_INSTALL )
        #[==================================================================================[
        # https://cmake.org/cmake/help/v3.27/command/install.html#targets
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
        install(TARGETS         converter
                                #${DATELIB}      donot add :  causes error when sourcing from local directory, difficult to debug
            EXPORT              ${targets_export_name}
                FILE_SET        converter_headers
                DESTINATION     ${CMAKE_INSTALL_INCLUDEDIR}
            INCLUDES
                # This directory will be used as include directory.
                DESTINATION     ${CMAKE_INSTALL_INCLUDEDIR}
        )
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
        install(EXPORT      ${targets_export_name}
	        FILE            ${targets_export_name}.cmake
            DESTINATION     ${CONVERTER_CMAKE_DIR}
            NAMESPACE       converter::
        )


        # https://cmake.org/cmake/help/v3.27/command/export.html#command:export
        # Export targets or packages for outside projects to use them
        # directly from the current project's build tree, without installation.
        export(TARGETS converter
                       #${DATELIB}      donot add : causes error when sourcing from github, in consumer projects(not in this project)
            NAMESPACE  converter::
            FILE       ${targets_export_name}.cmake)         # as per date/CMakeLists.txt


        ##########       PACKAGING
        set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
        set(CPACK_PACKAGE_VENDOR "converter developers")
        set(CPACK_PACKAGE_DESCRIPTION "${PROJECT_DESCRIPTION}")
        set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
        set(CPACK_PACKAGE_VERSION_MAJOR "${CONVERTER_VERSION_MAJOR}")
        set(CPACK_PACKAGE_VERSION_MINOR "${CONVERTER_VERSION_MINOR}")
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

        #include(CPack)

        # https://cmake.org/cmake/help/v3.27/command/export.html#exporting-packages
        #export(PACKAGE <PackageName>) ????? not needed for now as this deals with package-registry


        # Uninstall
        add_custom_target(uninstall_converter COMMAND "${CMAKE_COMMAND}" -E remove "${CMAKE_INSTALL_PREFIX}/include/converter")
	endif()

endfunction(converter_install_logic)
