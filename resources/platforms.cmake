if(${SWS_PLATFORM} STREQUAL "WIN32")

    #If we're on Windows
    if(WIN32)
        set(WIN32_BASE "C:/Qt/2010.05/qt")
        set(SWS_TARGET_WIN "crochet")
    #Cross Compile Settings
    else()
        set(CMAKE_SYSTEM_NAME "Windows")
        set(CMAKE_C_COMPILER "/usr/bin/i586-mingw32msvc-gcc")
        set(CMAKE_CXX_COMPILER "/usr/bin/i586-mingw32msvc-g++")
        set(CMAKE_FIND_ROOT_PATH "/usr/i586-mingw32msvc")
        set(CMAKE_RC_COMPILER "/usr/bin/i586-mingw32msvc-windres")

        set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
        set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
        set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

        set(SWS_TARGET_WIN "crochet.exe")
        set(WIN32_BASE "/home/crosscompile/win32")
    endif()

    #generate the embeded icon for the exe file.
    set(CMAKE_RC_COMPILER_INIT windres)
    enable_language(RC)
    set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")
    set(crochet_win ${CMAKE_CURRENT_SOURCE_DIR}/resources/win/crochet.rc)


    set(WIN32_LIBS "${WIN32_BASE}/bin")
    set(WIN32_PLUGINS "${WIN32_BASE}/plugins")

    set(QT_LIBS_WINDOWS "${WIN32_LIBS}/QtSvg4.dll" "${WIN32_LIBS}/QtCore4.dll" "${WIN32_LIBS}/QtGui4.dll"
                        "${WIN32_LIBS}/QtNetwork4.dll" "${WIN32_LIBS}/QtXml4.dll")
    set(QT_DEPS_WINDOWS "${WIN32_LIBS}/libgcc_s_dw2-1.dll" "${WIN32_LIBS}/mingwm10.dll"
                        "${CMAKE_CURRENT_SOURCE_DIR}/resources/qt.conf")
    set(QT_PLUGINS_WINDOWS "${WIN32_PLUGINS}/imageformats" "${WIN32_PLUGINS}/accessible" "${WIN32_PLUGINS}/iconengines")

elseif (${SWS_PLATFORM} STREQUAL "DARWIN")

    #if we're on a Mac
    if(APPLE)
        set(CMAKE_OSX_ARCHITECTURES "i386;x86_64") #";x86_64;ppc64;")

        set(DARWIN_LIBS "/Developer/SDKs/MacOSX10.6.sdk/Library/Frameworks/")
        set(DARWIN_PLUGINS "/Developer/Applications/Qt/plugins")
    #Cross Compile Settings
    else()
        set(CMAKE_SYSTEM_NAME "Darwin")

        if(${SWS_DARWIN_ARCH} STREQUAL "i386")
            set(CMAKE_C_COMPILER "/home/crosscompile/darwin/bin/i686-apple-darwin8-gcc")
            set(CMAKE_CXX_COMPILER "/home/crosscompile/darwin/bin/i686-apple-darwin8-g++")
            set(CMAKE_FIND_ROOT_PATH "/home/crosscompile/darwin/SDKs/MacOSX10.4u.sdk/usr/")
        else()
            set(CMAKE_C_COMPILER "/home/crosscompile/darwin/bin/powerpc-apple-darwin8-gcc")
            set(CMAKE_CXX_COMPILER "/home/crosscompile/darwin/bin/powerpc-apple-darwin8-g++")
            set(CMAKE_FIND_ROOT_PATH "/home/crosscompile/darwin/SDKs/MacOSX10.4u.sdk/usr/")
            include_directories("/home/crosscompile/darwin/SDKs/MacOSX10.4u.sdk/usr/lib/gcc/powerpc-apple-darwin8/4.0.1/include")
        endif()

        set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
        set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
        set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

        set(DARWIN_LIBS "/home/crosscompile/darwin/Library/Frameworks")
        set(DARWIN_PLUGINS "/home/crosscompile/darwin/Developer/Applications/Qt/plugins")

        set(CMAKE_OSX_ARCHITECTURES "ppc;i386") #";x86_64;ppc64;")

    endif()

    set(crochet_mac)
	
    # Need to copy the icon file
    #exec_program("mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/src/Crochet.app/Contents/Resources")
    #exec_program("cp ${CMAKE_CURRENT_SOURCE_DIR}/resources/mac/crochet.icns ${CMAKE_CURRENT_BINARY_DIR}/src/Crochet.app/Contents/Resources")

    set(QT_LIBS_DARWIN "${DARWIN_LIBS}/QtSvg.framework/Versions/4/QtSvg" "${DARWIN_LIBS}/QtCore.framework/Versions/4/QtCore"
           "${DARWIN_LIBS}/QtGui.framework/Versions/4/QtGui" "${DARWIN_LIBS}/QtNetwork.framework/Versions/4/QtNetwork"
           "${DARWIN_LIBS}/QtXml.framework/Versions/4/QtXml")

    set(QT_PLUGINS_DARWIN "${DARWIN_PLUGINS}/imageformats" "${DARWIN_PLUGINS}/accessible" "${DARWIN_PLUGINS}/iconengines")

    # Overload the module path so my Info.plist is the first one found
    set(CMAKE_MODULE_PATH ${crochet_SOURCE_DIR}/resources/mac ${CMAKE_MODULE_PATH})

    set(QT_DEPS_DARWIN "${CMAKE_CURRENT_SOURCE_DIR}/resources/qt.conf")

else() #LINUX
    set(crochet_nix)

endif()
