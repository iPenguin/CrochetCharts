    
    set(APPS "${CMAKE_INSTALL_PREFIX}/bin/Crochet")
   
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/src/Crochet.app/Contents/Resources/qt.conf" "[Paths]\nPlugins = plugins\n")
    install(CODE "
    include(BundleUtilities)
    fixup_bundle(\"${CMAKE_CURRENT_BINARY_DIR}/src/Crochet.app\" \"\${QT_PLUGINS_DARWIN}\" \"QtCore;QtGui;QtNetworking;QtXml;QtSvg\")"
    COMPONENT Runtime)
   
    set(CPACK_SYSTEM_NAME ${CMAKE_SYSTEM_NAME})
    set(CPACK_GENERATOR "DragNDrop")

    set(CPACK_OSX_PACKAGE_VERSION "10.4") #min package version

    set(MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_NAME} version ${SWS_VERSION}")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${CPACK_PACKAGE_VERSION}")
    set(MACOSX_BUNDLE_COPYRIGHT "${PROJECT_COPYRIGHT}. All rights reserved.")
    #add qt.conf file to bundle as qt needs it to locate the plugins: Contents/Resources/

#for more see: http://www.mail-archive.com/cmake@cmake.org/msg05498.html
#and see: http://www.cmake.org/Wiki/CMake:Bundles_And_Frameworks
#plutil command line utility to edit plist files.
#http://rixstep.com/2/20060901,00.shtml
    set(MACOSX_BUNDLE_INFO_STRING "${PROJECT_NAME} - version ${CPACK_PACKAGE_VERSION}")
    set(MACOSX_BUNDLE_BUNDLE_VERSION "${CPACK_PACKAGE_VERSION}")
    set(MACOSX_BUNDLE_ICON_FILE "resources/mac/crochet.icns")
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.stitchworkssoftware.crochet")
    set(MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}")

    set(CMAKE_OSX_ARCHITECTURES "i386;x86_64") #";x86_64;ppc64;")

    set(DARWIN_LIBS "/Developer/SDKs/MacOSX10.6.sdk/Library/Frameworks/")
    set(DARWIN_PLUGINS "/Developer/Applications/Qt/plugins")

    set(crochet_mac)

    set(QT_PLUGINS_DARWIN "${DARWIN_PLUGINS}/imageformats" "${DARWIN_PLUGINS}/accessible" "${DARWIN_PLUGINS}/iconengines")

    # Overload the module path so my Info.plist is the first one found
    set(CMAKE_MODULE_PATH ${crochet_SOURCE_DIR}/resources/mac ${CMAKE_MODULE_PATH})

    set(QT_DEPS_DARWIN "${CMAKE_CURRENT_SOURCE_DIR}/resources/qt.conf")
