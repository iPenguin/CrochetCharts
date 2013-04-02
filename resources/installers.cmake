include(InstallRequiredSystemLibraries)


set(PROJECT_DESCRIPTION  "Symbol crochet design software")
set(PROJECT_VENDOR       "Stitch Works Software")
set(ORG_BASE_URL         "StitchWorksSoftware.com")
set(PROJECT_CONTACT      "support@${ORG_BASE_URL}")
set(ORG_CONTACT          "http://${ORG_BASE_URL}/contact")
set(ORG_DOWNLOAD         "http://${ORG_BASE_URL}/downloads")
set(ORG_WEBSITE          "www.${ORG_BASE_URL}")
set(PROJECT_VERSION      "${VERSION_SHORT}")
set(PROJECT_COPYRIGHT    "Copyright (c) ${PROJECT_LIFE} ${PROJECT_VENDOR}")
set(PROJECT_MACOSX_ICON  "${PROJECT_NAME}.icns")


set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
set(CPACK_PACKAGE_VENDOR ${PROJECT_VENDOR})
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})
set(CPACK_PACKAGE_CONTACT ${PROJECT_CONTACT})
set(CPACK_PACKAGE_EXECUTABLES "${PROJECT_NAME};${PROJECT_NAME}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${PROJECT_NAME}")

#FIXME: use the FindDoxygen.cmake module.
if(DOXYGEN)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/Doxyfile.in
                ${CMAKE_BINARY_DIR}/Doxyfile)
    execute_process(
                COMMAND "/usr/bin/doxygen" "${CMAKE_BINARY_DIR}/Doxyfile"
                WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
                OUTPUT_VARIABLE _output)
endif()


SET(plugin_dest_dir bin)
SET(qtconf_dest_dir bin)
SET(APPS "@CMAKE_INSTALL_PREFIX@/bin/${PROJECT_NAME}")
IF(APPLE)
  SET(plugin_dest_dir "${PROJECT_NAME}.app/Contents/PlugIns")
  SET(qtconf_dest_dir "${PROJECT_NAME}.app/Contents/Resources")
  SET(APPS "@CMAKE_CURRENT_BINARY_DIR@/${PROJECT_NAME}")
ENDIF(APPLE)
IF(WIN32)
  SET(APPS "@CMAKE_INSTALL_PREFIX@/bin/${PROJECT_NAME}.exe")
ENDIF(WIN32)


if(WIN32)

    set(CPACK_PACKAGE_ICON "C:\\\\Documents and Settings\\\\Brian Milco\\\\My Documents\\\\crochet.git\\\\images\\\\installer.bmp")
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_NSIS_PACKAGE_NAME "${PROJECT_NAME}")
    set(CPACK_NSIS_DISPLAY_NAME "${PROJECT_NAME}")
    set(CPACK_NSIS_CONTACT "${CPACK_PACKAGE_CONTACT}")
    set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${PROJECT_NAME}-${VERSION_MAJOR}.${VERSION_MINOR}")

    set(CPACK_CMAKE_MODULES_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/")

    set(CPACK_NSIS_MENU_LINKS "docs/homepage.html" "Homepage for ${PROJECT_VENDOR}"
                              "bin/${PROJECT_NAME}_User_Guide_${VERSION_SHORT}.pdf" "${PROJECT_NAME} Help")
    # this doesn't work for the NSIS installer
    set(CPACK_CREATE_DESKTOP_LINKS "${PROJECT_NAME}.exe")

    #set(CPACK_NSIS_CREATE_ICONS_EXTRA "CreateShortCut '\$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\${PROJECT_NAME}.lnk' '\$INSTDIR\\\\${PROJECT_NAME}.exe'"
    #                                  "CreateShortCut '\$DESKTOP\\\\${PROJECT_NAME}.lnk' '\$INSTDIR\\\\${PROJECT_NAME}.exe'")
    # Icon in the add/remove control panel. Must be an .exe file
    set(CPACK_NSIS_INSTALLED_ICON_NAME "${PROJECT_NAME}.exe")

    set(CPACK_NSIS_URL_INFO_ABOUT "${ORG_WEBSITE}")
    set(CPACK_NSIS_HELP_LINK "${ORG_WEBSITE}")

    set(CPACK_NSIS_MUI_FINISHPAGE_RUN "${PROJECT_NAME}.exe")

elseif(APPLE)
    set(CPACK_SYSTEM_NAME ${CMAKE_SYSTEM_NAME})
    set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}")
    set(CPACK_GENERATOR "Bundle")
    set(CPACK_BUNDLE_NAME "${PROJECT_NAME}")
    set(CPACK_BUNDLE_PLIST "${CMAKE_BINARY_DIR}/Info.plist")
    set(CPACK_BUNDLE_ICON "${CMAKE_SOURCE_DIR}/images/${PROJECT_MACOSX_ICON}")
    
    set(CPACK_DMG_VOLUME_NAME "${PROJECT_NAME}")
    set(CPACK_DMG_DS_STORE "${CMAKE_SOURCE_DIR}/resources/mac/MacDmgDsStore")
    set(CPACK_DMG_BACKGROUND_IMAGE "${CMAKE_SOURCE_DIR}/images/dmg_background.pdf")

    set(CPACK_OSX_PACKAGE_VERSION "10.5") #min package version
    
    set(MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_NAME} version ${VERSION}")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}")
    set(MACOSX_BUNDLE_COPYRIGHT "${PROJECT_COPYRIGHT}. All rights reserved.")

#for more see: http://www.mail-archive.com/cmake@cmake.org/msg05498.html
#and see: http://www.cmake.org/Wiki/CMake:Bundles_And_Frameworks
#plutil command line utility to edit plist files.
#http://rixstep.com/2/20060901,00.shtml
    set(MACOSX_BUNDLE_INFO_STRING "${PROJECT_NAME} - version ${PROJECT_VERSION}")
    set(MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}")
    set(MACOSX_BUNDLE_ICON_FILE "${CMAKE_CURRENT_SOURCE_DIR}/images/${PROJECT_MACOSX_ICON}")
    set_source_files_properties("${MACOSX_BUNDLE_ICON_FILE}" PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

    #FIXME: what are the implications of changing this to the correct com.stitchworkssoftware.crochetcharts?
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.stitchworkssoftware.Crochet Charts")
    set(MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}")

    configure_file(${CMAKE_SOURCE_DIR}/cmake/modules/MacOSXBundleInfo.plist.in
                ${CMAKE_BINARY_DIR}/Info.plist)

    install(CODE "
        file(COPY \"@CMAKE_BINARY_DIR@/docs/pdf/@PROJECT_NAME@_User_Guide_@VERSION_SHORT@.pdf\" 
             DESTINATION \"@CMAKE_BINARY_DIR@/_CPack_Packages/Darwin/Bundle/@PROJECT_NAME@-@VERSION_SHORT@\")
        file(RENAME \"@CMAKE_BINARY_DIR@/_CPack_Packages/Darwin/Bundle/@PROJECT_NAME@-@VERSION_SHORT@/@PROJECT_NAME@_User_Guide_@VERSION_SHORT@.pdf\" 
        \"@CMAKE_BINARY_DIR@/_CPack_Packages/Darwin/Bundle/@PROJECT_NAME@-@VERSION_SHORT@/User Guide.pdf\")
        " COMPONENT Runtime)

    set(DIRS ${QT_LIBRARY_DIRS})

    set(crochet_mac "${CPACK_BUNDLE_ICON}")

else()

    set(CPACK_GENERATOR "DEB") #;RPM;STGZ;TBZ2")

    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Brian Milco <${PROJECT_CONTACT}>")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6, libqtgui4 (>= 4.7.0), libqtcore4 (>= 4.7.0), libqt4-svg (>= 4.7.0), libqt4-xml (>= 4.7.0), libqt4-network (>= 4.7.0)")
    set(CPACK_DEBIAN_PACKAGE_SECTION "Graphics")
    set(CPACK_DEBIAN_PACKAGE_VERSION ${CPACK_PACKAGE_VERSION})


    set(CPACK_RPM_PACKAGE_LICENSE "Commercial")
    set(CPACK_RPM_PACKAGE_GROUP "Applications/Productivity")
    set(CPACK_RPM_PACKAGE_VENDOR ${CPACK_PACKAGE_VENDOR})
    set(CPACK_RPM_PACKAGE_REQUIRES "requires: libc6, qt >= 4.7, qt-x11 >= 4.7")

    if(FORCE_32BIT)
        set(CPACK_RPM_PACKAGE_ARCHITECTURE "i386")
        set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE i386)
    else()
        set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE amd64)
        set(CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")
    endif()
    
endif()

set(CPACK_BINARY_DRAGNDROP ON)
include(CPack)
