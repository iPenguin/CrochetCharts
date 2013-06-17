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
set(CPACK_PACKAGE_EXECUTABLES "${PROJECT_NAME}" "${PROJECT_NAME}")
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

set(DESKTOP_DIR     "/usr/share/applications/")
set(PIXMAPS_DIR     "/usr/share/icons/")

# try to set up the menu system
find_program(XDG-MIME_EXECUTABLE xdg-mime)
find_program(XDG-DESKTOP-MENU_EXECUTABLE xdg-desktop-menu)

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
    set(CPACK_MONOLITHIC_INSTALL 1)
    set(CPACK_PACKAGE_ICON "C:\\\\Documents and Settings\\\\Brian Milco\\\\My Documents\\\\crochet.git\\\\images\\\\installer.bmp")
    set(CPACK_GENERATOR "NSIS")
    #
    #set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL "ON")
    #
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

    set(CPACK_BUNDLE_NAME "${PROJECT_NAME}")
    set(CPACK_BUNDLE_PLIST "${CMAKE_BINARY_DIR}/Info.plist")
    set(CPACK_BUNDLE_ICON "${CMAKE_SOURCE_DIR}/images/${PROJECT_MACOSX_ICON}")

    set(CPACK_DMG_VOLUME_NAME "${PROJECT_NAME}")
    set(CPACK_DMG_DS_STORE "${CMAKE_SOURCE_DIR}/resources/MacDmgDsStore")
    set(CPACK_DMG_BACKGROUND_IMAGE "${CMAKE_SOURCE_DIR}/images/dmg_background.pdf")
    
    set(MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_NAME} version ${VERSION}")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}")
    set(MACOSX_BUNDLE_COPYRIGHT "${PROJECT_COPYRIGHT}. All rights reserved.")

    set(CPACK_APPLE_CODESIGN_FILES "/Contents/Frameworks/QtCore.framework/Versions/4/QtCore" 
                                   "/Contents/Frameworks/QtGui.framework/Versions/4/QtGui"
                                   "/Contents/Frameworks/QtNetwork.framework/Versions/4/QtNetwork"
                                   "/Contents/Frameworks/QtXml.framework/Versions/4/QtXml"
                                   "/Contents/Frameworks/QtSvg.framework/Versions/4/QtSvg"
                                   "/Contents/PlugIns/accessible/libqtaccessiblewidgets.dylib"
                                   "/Contents/PlugIns/bearer/libqgenericbearer.dylib"
                                   "/Contents/PlugIns/codecs/libqcncodecs.dylib"
                                   "/Contents/PlugIns/codecs/libqjpcodecs.dylib"
                                   "/Contents/PlugIns/codecs/libqkrcodecs.dylib"
                                   "/Contents/PlugIns/codecs/libqtwcodecs.dylib"
                                   "/Contents/PlugIns/iconengines/libqsvgicon.dylib"
                                   "/Contents/PlugIns/imageformats/libqgif.dylib"
                                   "/Contents/PlugIns/imageformats/libqico.dylib"
                                   "/Contents/PlugIns/imageformats/libqjpeg.dylib"
                                   "/Contents/PlugIns/imageformats/libqmng.dylib"
                                   "/Contents/PlugIns/imageformats/libqsvg.dylib"
                                   "/Contents/PlugIns/imageformats/libqtga.dylib"
                                   "/Contents/PlugIns/imageformats/libqtiff.dylib"
    )

    set(CPACK_APPLE_ENTITLEMENTS "${CMAKE_SOURCE_DIR}/resources/Entitlements.plist")

    if(${APP_STORE})
        set(CPACK_GENERATOR "MacAppStore")
        set(CPACK_APPLE_CERT_APP "3rd Party Mac Developer Application: Brian Milco")
        set(CPACK_APPLE_CERT_INSTALLER "3rd Party Mac Developer Installer: Brian Milco")
    else()
        set(CPACK_GENERATOR "Bundle")
        set(CPACK_APPLE_CERT_APP "Developer ID Application: Brian Milco")
        set(CPACK_APPLE_CERT_INSTALLER "Developer ID Installer: Brian Milco")
    endif()

#for more see: http://www.mail-archive.com/cmake@cmake.org/msg05498.html
#and see: http://www.cmake.org/Wiki/CMake:Bundles_And_Frameworks
#plutil command line utility to edit plist files.
#http://rixstep.com/2/20060901,00.shtml
    set(MACOSX_BUNDLE_INFO_STRING "${PROJECT_NAME} - version ${PROJECT_VERSION}")
    set(MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}")
    set(MACOSX_BUNDLE_ICON_FILE "${CMAKE_CURRENT_SOURCE_DIR}/images/${PROJECT_MACOSX_ICON}")
    set_source_files_properties("${MACOSX_BUNDLE_ICON_FILE}" PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

    set(MACOSX_BUNDLE_GUI_IDENTIFIER "${BUNDLE_ID}")
    set(CPACK_APPLE_BUNDLE_ID "${MACOSX_BUNDLE_GUI_IDENTIFIER}")
    set(MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}")

    configure_file(${CMAKE_SOURCE_DIR}/cmake/modules/MacOSXBundleInfo.plist.in
                ${CMAKE_BINARY_DIR}/Info.plist)

    set(DIRS ${QT_LIBRARY_DIRS})

    set(crochet_mac "${CPACK_BUNDLE_ICON}")

    install(FILES images/PatternDocument.icns DESTINATION ../Resources/)
    install(CODE "
        file(COPY \"@CMAKE_BINARY_DIR@/docs/pdf/@PROJECT_NAME@_User_Guide_@VERSION_SHORT@.pdf\" 
             DESTINATION \"@CMAKE_BINARY_DIR@/_CPack_Packages/Darwin/@CPACK_GENERATOR@/@PROJECT_NAME@-@VERSION_SHORT@\")
        file(RENAME \"@CMAKE_BINARY_DIR@/_CPack_Packages/Darwin/@CPACK_GENERATOR@/@PROJECT_NAME@-@VERSION_SHORT@/@PROJECT_NAME@_User_Guide_@VERSION_SHORT@.pdf\" 
        \"@CMAKE_BINARY_DIR@/_CPack_Packages/Darwin/@CPACK_GENERATOR@/@PROJECT_NAME@-@VERSION_SHORT@/User Guide.pdf\")
        " COMPONENT Runtime)

else()
    set(CPACK_GENERATOR "DEB;RPM") #;RPM;STGZ;TBZ2")

    configure_file(${CMAKE_SOURCE_DIR}/resources/CrochetCharts.desktop.in
                "${CMAKE_BINARY_DIR}/Crochet Charts.desktop")

    install (FILES "@CMAKE_BINARY_DIR@/Crochet Charts.desktop" DESTINATION ${DESKTOP_DIR})
    install (FILES "resources/vnd.stitchworks.pattern.xml" DESTINATION share/${PROJECT_NAME})

    install(FILES "@CMAKE_SOURCE_DIR@/images/CrochetCharts.svg" DESTINATION share/${PROJECT_NAME}/images)
    install(FILES "@CMAKE_SOURCE_DIR@/images/stitchworks-pattern.svg" DESTINATION share/${PROJECT_NAME}/images)
    install(DIRECTORY "@CMAKE_BINARY_DIR@/icons" DESTINATION share/${PROJECT_NAME})

    set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA
        "${CMAKE_SOURCE_DIR}/resources/deb/postinst;${CMAKE_SOURCE_DIR}/resources/deb/prerm")

    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${CPACK_PACKAGE_VENDOR} <${PROJECT_CONTACT}>")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqtgui4 (>= 4.7.0), libqtcore4 (>= 4.7.0), libqt4-svg (>= 4.7.0), libqt4-xml (>= 4.7.0), libqt4-network (>= 4.7.0)")
    set(CPACK_DEBIAN_PACKAGE_SECTION "Graphics")
    set(CPACK_DEBIAN_PACKAGE_VERSION ${CPACK_PACKAGE_VERSION})
    set(CPACK_DEBIAN_PACKAGE_NAME ${CPACK_PACKAGE_NAME})
    set(CPACK_DEBIAN_PACKAGE_DESCRIPTION ${CPACK_PACKAGE_DESCRIPTION_SUMMARY})


    set(CPACK_RPM_PACKAGE_LICENSE "Commercial")
    set(CPACK_RPM_PACKAGE_GROUP "Amusements/Graphics")
    set(CPACK_RPM_PACKAGE_VENDOR ${CPACK_PACKAGE_VENDOR})
    set(CPACK_RPM_PACKAGE_REQUIRES "libqt4 >= 4.7, libqt4-x11 >= 4.7")
    set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")

    if(AMD64)
        set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE amd64)
        set(CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")
    else()
        set(CPACK_RPM_PACKAGE_ARCHITECTURE "i386")
        set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE i386)
    endif()
    
endif()
