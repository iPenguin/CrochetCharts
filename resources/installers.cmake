
set(PROJECT_DESCRIPTION  "Chart desing software for crochet")
set(PROJECT_COPYRIGHT    "Copyright (c) 2010-2011 Stitch Works Software")
set(PROJECT_CONTACT      "support@stitchworkssoftware.com")
set(PROJECT_VENDOR       "Stitch Works Software")
set(ORG_WEBSITE          "www.stitchworkssoftware.com")


set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/resources/installer-license.txt")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/resources/installer-readme.txt")
set(CPACK_RESOURCE_FILE_WELCOME "${CMAKE_CURRENT_SOURCE_DIR}/resources/installer-welcome.txt")
set(CPACK_PACKAGE_DECRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/resources/installer-description.txt")

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
set(CPACK_PACKAGE_VENDOR ${PROJECT_VENDOR})
set(CPACK_PACKAGE_VERSION ${SWS_VERSION})
set(CPACK_PACKAGE_VERSION_MAJOR ${SWS_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${SWS_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${SWS_VERSION_PATCH})
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${PROJECT_NAME}")
set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/images/installer.bmp")
set(CPACK_PACKAGE_CONTACT ${PROJECT_CONTACT})

if(${SWS_PLATFORM} STREQUAL "WIN32")
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_NSIS_PACKAGE_NAME "${PROJECT_NAME}")
    set(CPACK_NSIS_DISPLAY_NAME "${PROJECT_NAME}")
    set(CPACK_NSIS_CONTACT ${CPACK_PACKAGE_CONTACT})
    set(CPACK_PACKAGE_EXECUTABLES "crochet" "${PROJECT_NAME}")
    set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${CMAKE_PROJECT_NAME}-${SWS_VERSION_MAJOR}.${SWS_VERSION_MINOR}.${SWS_VERSION_PATCH}")

    set(CPACK_NSIS_MENU_LINKS "${ORG_WEBSITE}" "Homepage for ${PROJECT_VENDOR}")

    # this doesn't work for the NSIS installer
    set(CPACK_CREATE_DESKTOP_LINKS "crochet")

    #CPACK_NSIS_CREATE_ICONS_EXTRA -- set(CPACK_NSIS_CREATE_ICONS "CreateShortCut '\$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\${PROJECT_NAME}.lnk' '\$INSTDIR\\\\${PROJECT_NAME}.exe'")
    #CPACK_NSIS_DELETE_ICONS_EXTRA
    # Icon in the add/remove control panel. Must be an .exe file
    set(CPACK_NSIS_INSTALLED_ICON_NAME "crochet.exe")

    set(CPACK_NSIS_URL_INFO_ABOUT "${ORG_WEBSITE}")
    set(CPACK_NSIS_HELP_LINK "${ORG_WEBSITE}")

    set(CPACK_NSIS_MUI_FINISHPAGE_RUN "crochet.exe")

elseif(${SWS_PLATFORM} STREQUAL "DARWIN")
    set(CPACK_SYSTEM_NAME ${CMAKE_SYSTEM_NAME})
    set(CPACK_GENERATOR "DragNDrop")

    set(CPACK_OSX_PACKAGE_VERSION "10.4") #min package version

    set(MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_NAME} version ${SWS_VERSION}")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${SWS_VERSION_MAJOR}.${SWS_VERSION_MINOR}.${SWS_VERSION_PATCH}")
    set(MACOSX_BUNDLE_COPYRIGHT "${PROJECT_COPYRIGHT}. All rights reserved.")
    #add qt.conf file to bundle as qt needs it to locate the plugins: Contents/Resources/

#for more see: http://www.mail-archive.com/cmake@cmake.org/msg05498.html
#and see: http://www.cmake.org/Wiki/CMake:Bundles_And_Frameworks
#plutil command line utility to edit plist files.
#http://rixstep.com/2/20060901,00.shtml
    set(MACOSX_BUNDLE_INFO_STRING "${PROJECT_NAME} - version ${SWS_VERSION_MAJOR}.${SWS_VERSION_MINOR}.${SWS_VERSION_PATCH}")
    set(MACOSX_BUNDLE_BUNDLE_VERSION "${SWS_VERSION_MAJOR}.${SWS_VERSION_MINOR}.${SWS_VERSION_PATCH}")
    set(MACOSX_BUNDLE_ICON_FILE "resources/mac/crochet.icns")
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.stitchworkssoftware")
    set(MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}")

else()
    set(CPACK_GENERATOR "DEB;RPM;STGZ;TBZ2")

    #TODO: finish adding the deb and rpm stuff here.
endif()

include(CPack)