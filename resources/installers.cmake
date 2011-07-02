include(InstallRequiredSystemLibraries)


set(PROJECT_DESCRIPTION  "Chart desing software for crochet")
set(PROJECT_VENDOR       "Stitch Works Software")
set(PROJECT_LIFE         "2010-2011")
set(PROJECT_CONTACT      "support@StitchWorksSoftware.com")
set(ORG_WEBSITE          "www.StitchWorksSoftware.com")
set(PROJECT_VERSION      "${SWS_VERSION_MAJOR}.${SWS_VERSION_MINOR}.${SWS_VERSION_PATCH}")
set(PROJECT_COPYRIGHT    "Copyright (c) ${PROJECT_LIFE} ${PROJECT_VENDOR}")
set(PROJECT_MACOSX_ICON  "Crochet.icns")

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/resources/installer-license.txt")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/resources/installer-readme.txt")
set(CPACK_RESOURCE_FILE_WELCOME "${CMAKE_CURRENT_SOURCE_DIR}/resources/installer-welcome.txt")
set(CPACK_PACKAGE_DECRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/resources/installer-description.txt")

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
set(CPACK_PACKAGE_VENDOR ${PROJECT_VENDOR})
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_VERSION_MAJOR ${SWS_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${SWS_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${SWS_VERSION_PATCH})
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${PROJECT_NAME}")
#set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/images/installer.bmp")
set(CPACK_PACKAGE_CONTACT ${PROJECT_CONTACT})
set(CPACK_PACKAGE_EXECUTABLES "crochet;Crochet")

set(plugin_dest_dir bin)
set(qtconf_dest_dir bin)
set(APPS "\${CMAKE_INSTALL_PREFIX}/Crochet/bin")

if(WIN32)
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_NSIS_PACKAGE_NAME "${PROJECT_NAME}")
    set(CPACK_NSIS_DISPLAY_NAME "${PROJECT_NAME}")
    set(CPACK_NSIS_CONTACT ${CPACK_PACKAGE_CONTACT})
    set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${CMAKE_PROJECT_NAME}-${SWS_VERSION_MAJOR}.${SWS_VERSION_MINOR}")

	install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/docs/homepage.html" DESTINATION docs)
    set(CPACK_NSIS_MENU_LINKS "docs/homepage.html" "Homepage for ${PROJECT_VENDOR}")

    # this doesn't work for the NSIS installer
    #set(CPACK_CREATE_DESKTOP_LINKS "crochet")

    set(CPACK_NSIS_CREATE_ICONS_EXTRA "CreateShortCut '\$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\${PROJECT_NAME}.lnk' '\$INSTDIR\\\\${PROJECT_NAME}.exe'")
    #CPACK_NSIS_DELETE_ICONS_EXTRA
    # Icon in the add/remove control panel. Must be an .exe file
    set(CPACK_NSIS_INSTALLED_ICON_NAME "crochet.exe")

    set(CPACK_NSIS_URL_INFO_ABOUT "${ORG_WEBSITE}")
    set(CPACK_NSIS_HELP_LINK "${ORG_WEBSITE}")

    set(CPACK_NSIS_MUI_FINISHPAGE_RUN "crochet.exe")

elseif(APPLE)
    set(CPACK_SYSTEM_NAME ${CMAKE_SYSTEM_NAME})
    set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}")
    set(CPACK_GENERATOR "Bundle")
    set(CPACK_BUNDLE_NAME "${PROJECT_NAME}")
    set(CPACK_BUNDLE_PLIST "${CMAKE_CURRENT_BINARY_DIR}/Info.plist")
    set(CPACK_BUNDLE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/images/crochet.icns")
    
    set(CPACK_DMG_VOLUME_NAME "${PROJECT_NAME}")
    set(CPACK_DMG_DS_STORE "${CMAKE_CURRENT_SOURCE_DIR}/resources/mac/MacDmgDsStore")
    set(CPACK_DMG_BACKGROUND_IMAGE "${CMAKE_CURRENT_SOURCE_DIR}/images/dmg_background.png")

    set(CPACK_BINARY_DRAGNDROP ON)

    set(CPACK_OSX_PACKAGE_VERSION "10.4") #min package version
    
    set(MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_NAME} version ${SWS_VERSION}")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}")
    set(MACOSX_BUNDLE_COPYRIGHT "${PROJECT_COPYRIGHT}. All rights reserved.")

#for more see: http://www.mail-archive.com/cmake@cmake.org/msg05498.html
#and see: http://www.cmake.org/Wiki/CMake:Bundles_And_Frameworks
#plutil command line utility to edit plist files.
#http://rixstep.com/2/20060901,00.shtml
    set(MACOSX_BUNDLE_INFO_STRING "${PROJECT_NAME} - version ${PROJECT_VERSION}")
    set(MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}")
    set(MACOSX_BUNDLE_ICON_FILE "${PROJECT_MACOSX_ICON}")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/images/crochet.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.stitchworkssoftware.crochet")
    set(MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}")

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/MacOSXBundleInfo.plist.in
                ${CMAKE_CURRENT_BINARY_DIR}/Info.plist)

else()
    set(CPACK_GENERATOR "DEB;RPM;STGZ;TBZ2")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Brian Milco")
    set(CPACK_RPM_PACKAGE_LICENSE "Commercial")
    set(CPACK_RPM_PACKAGE_GROUP "Amusements/Graphics")

    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqtgui4 (>= 4:4.7.0), libqtcore4 (>= 4:4.7.0)")

    #depends(qt4 >= 4.7)
    #TODO: finish adding the deb and rpm stuff here.
endif()

include(CPack)
