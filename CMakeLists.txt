cmake_minimum_required(VERSION 2.8)
project(Crochet)

# Appends the cmake/modules path to MAKE_MODULE_PATH variable...
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})

#Forces out of source builds just in case.
include(MacroOutOfSourceBuild)
macro_ensure_out_of_source_build("${PROJECT_NAME} requires an out of source build.")

set(CMAKE_CONFIGURATION_TYPES Debug Release Profiling CACHE TYPE INTERNAL FORCE)
if(${CMAKE_BUILD_TYPE} STREQUAL "")
    set(CMAKE_BUILD_TYPE "Debug")
endif()

set (HUNSPELL_FIND_QUIETLY TRUE)
include(FindHunSpell)

include(GetGitRevisionDescription)
git_describe(SWS_VERSION --tags --dirty=-working)

string(REGEX REPLACE "^([0-9]+)\\..*" "\\1" SWS_VERSION_MAJOR "${SWS_VERSION}")
string(REGEX REPLACE "^[0-9]+\\.([0-9]+).*" "\\1" SWS_VERSION_MINOR "${SWS_VERSION}")
string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" SWS_VERSION_PATCH "${SWS_VERSION}")
string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.[0-9]+(.*)" "\\1" SWS_VERSION_SHA1 "${SWS_VERSION}")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/version.cpp.in
                ${CMAKE_CURRENT_BINARY_DIR}/version.cpp)

find_package(Qt4 REQUIRED)

option(UNIT_TESTING  "Build the test libraries for this software" OFF)
option(FORCE_32BIT "Force 32-bit builds when cross compiling with distcc (needed for 64-bit compile nodes)" OFF)
option(SHADOW_FLAG "Use -Wshadow when compiling" OFF)
set(SWS_RELEASE_PLATFORM "LINUX" CACHE STRING "Compile the software for the given platform (LINUX, DARWIN, WIN32)")

#if we're on the other platforms automatically set the release platform.
if(APPLE)
    set(SWS_RELEASE_PLATFORM "DARWIN")
elseif(WIN32)
    set(SWS_RELEASE_PLATFORM "WIN32")
endif()

message("-------------------------------------------------------")
message(STATUS ${PROJECT_NAME} " Version: " ${SWS_VERSION})
message(STATUS "SWS Release Platform: " ${SWS_RELEASE_PLATFORM})
message(STATUS "CMake Build Type: " ${CMAKE_BUILD_TYPE})
message(STATUS "Unit Testing: " ${UNIT_TESTING})
message(STATUS "Force 32bit: " ${FORCE_32BIT})
message(STATUS "Use -Wshadow: " ${SHADOW_FLAG})
message(STATUS "Found Hunspell: " ${HUNSPELL_FOUND})
message("-------------------------------------------------------")

###########################################################################
# Cross Compile Variables
###########################################################################
if(${SWS_RELEASE_PLATFORM} STREQUAL "WIN32")
    if(WIN32)
        set(WIN32_BASE="C:/Qt/2010.05/qt")
    else()
        set(CMAKE_SYSTEM_NAME "Windows")
        set(CMAKE_C_COMPILER "/usr/bin/i586-mingw32msvc-gcc")
        set(CMAKE_CXX_COMPILER "/usr/bin/i586-mingw32msvc-g++")
        set(CMAKE_FIND_ROOT_PATH "/usr/i586-mingw32msvc")

        set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
        set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
        set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

        set(WIN32_BASE "/home/crosscompile/win32")
    endif()

    set(WIN32_LIBS "${WIN32_BASE}/bin")
    set(WIN32_PLUGINS "${WIN32_BASE}/plugins")
    
    set(QT_LIBS_WINDOWS "${WIN32_LIBS}/QtSvg4.dll" "${WIN32_LIBS}/QtCore4.dll" "${WIN32_LIBS}/QtGui4.dll"
                        "${WIN32_LIBS}/QtNetwork4.dll" "${WIN32_LIBS}/QtXml4.dll")
    set(QT_DEPS_WINDOWS "${WIN32_LIBS}/libgcc_s_dw2-1.dll" "${WIN32_LIBS}/mingwm10.dll")
    set(QT_PLUGINS_WINDOWS "${WIN32_BASE}/imageformats" "${WIN32_BASE}/accessible" "${WIN32_BASE}/iconengines")

elseif (${SWS_RELEASE_PLATFORM} STREQUAL "DARWIN")

    if(APPLE)
        set(DARWIN_LIBS "")
        set(DARWIN_PLUGINS "")
    else()
        set(CMAKE_SYSTEM_NAME "Darwin")
        set(CMAKE_C_COMPILER "/home/crosscompile/darwin/bin/i686-apple-darwin8-gcc")
        set(CMAKE_CXX_COMPILER "/home/crosscompile/darwin/bin/i686-apple-darwin8-g++")
        set(CMAKE_FIND_ROOT_PATH "/home/crosscompile/darwin/SDKs/MacOSX10.4u.sdk/usr/")

        set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
        set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
        set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

        set(DARWIN_LIBS "/home/crosscompile/darwin/Library/Frameworks")
        set(DARWIN_PLUGINS "/home/crosscompile/darwin/Developer/Applications/Qt/plugins")
    endif()

    set(QT_LIBS_DARWIN "${DARWIN_LIBS}/QtSvg.framework/Versions/4/QtSvg" "${DARWIN_LIBS}/QtCore.framework/Versions/4/QtCore"
                       "${DARWIN_LIBS}/QtGui.framework/Versions/4/QtGui" "${DARWIN_LIBS}/QtNetwork.framework/Versions/4/QtNetwork"
                       "${DARWIN_LIBS}/QtXml.framework/Versions/4/QtXml")
    set(QT_DEPS_DARWIN "")
    
    set(QT_PLUGINS_DARWIN "${DARWIN_PLUGINS}/imageformats" "${DARWIN_PLUGINS}/accessible" "${DARWIN_PLUGINS}/iconengines")
endif()

if(FORCE_32BIT)
    set(extra_build_flags "-m32")
    set(extra_link_flags "-m elf_i386")
endif()

if(SHADOW_FLAG)
    list(APPEND extra_build_flags " -Wshadow")
endif()

set(CMAKE_C_FLAGS "${extra_build_flags} -g -ggdb -W -Wall") #-ansi -pedantic
set(CMAKE_CXX_FLAGS ${CMAKE_C_FLAGS})
set(CMAKE_EXE_LINKER_FLAGS " ${CMAKE_EXE_LINKER_FLAGS}")

set(CMAKE_C_FLAGS_DEBUG "${extra_build_flags} -g -ggdb -W -Wall") #-march=athlon64 -march=atom, core2, i686, (-mtune=generic, native)
set(CMAKE_CXX_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${extra_link_flags} ${CMAKE_EXE_LINKER_FLAGS}")

set(CMAKE_C_FLAGS_RELEASE "${extra_build_flags} -W -Wall -O2")
set(CMAKE_CXX_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${extra_link_flags} ${CMAKE_EXE_LINKER_FLAGS}")

set(CMAKE_C_FLAGS_PROFILING "${extra_build_flags} -g -ggdb -pg -W -Wall")
set(CMAKE_CXX_FLAGS_PROFILING ${CMAKE_C_FLAGS_PROFILING})
set(CMAKE_EXE_LINKER_FLAGS_PROFILING "-pg " ${CMAKE_EXE_LINKER_FLAGS})

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

#More info see: http://cmake.org/cmake/help/cmake2.6docs.html#module:FindQt4
set(QT_USE_QTMAIN true)
set(QT_USE_QTSVG true)
set(QT_USE_QTNETWORK true)
include(${QT_USE_FILE})

#QT4_CREATE_TRANSLATION( qm_files directories ... sources ... ts_files ... OPTIONS ...)
#QT4_ADD_TRANSLATION( qm_files ts_files ... )

add_definitions(${QT_DEFINITIONS})

set(crochet_version "${CMAKE_CURRENT_BINARY_DIR}/version.cpp")

if(${SWS_RELEASE_PLATFORM} STREQUAL "WIN32")
    set(crochet_win ${CMAKE_CURRENT_SOURCE_DIR}/platforms/win/crochet.rc)

elseif(${SWS_RELEASE_PLATFORM} STREQUAL "DARWIN")

    set(crochet_mac)
    set(CMAKE_OSX_ARCHITECTURES "ppc;i386") #;ppc64;x86_64

#for more see: http://www.mail-archive.com/cmake@cmake.org/msg05498.html
#and see: http://www.cmake.org/Wiki/CMake:Bundles_And_Frameworks
#plutil command line utility to edit plist files.
#http://rixstep.com/2/20060901,00.shtml
    set(MACOSX_BUNDLE_INFO_STRING "Crochet - version ${SWS_VERSION_MAJOR}.${SWS_VERSION_MINOR}.${SWS_VERSION_PATCH}")
    set(MACOSX_BUNDLE_BUNDLE_VERSION "${SWS_VERSION_MAJOR}.${SWS_VERSION_MINOR}.${SWS_VERSION_PATCH}")
    set(MACOSX_BUNDLE_ICON_FILE "platforms/mac/crochet.icns")
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.stitchworkssoftware")
    set(MACOSX_BUNDLE_BUNDLE_NAME "Crochet")

    # Need to copy the icon file
    exec_program("mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/src/Crochet.app/Contents/Resources")
    exec_program("cp ${CMAKE_CURRENT_SOURCE_DIR}/platforms/mac/crochet.icns ${CMAKE_CURRENT_BINARY_DIR}/src/Crochet.app/Contents/Resources")

    # Overload the module path so my Info.plist is the first one found
    set(CMAKE_MODULE_PATH ${crochet_SOURCE_DIR}/platforms/mac ${CMAKE_MODULE_PATH})

else()
    set(crochet_nix)
endif()

add_subdirectory(src)
add_subdirectory(docs)

if(UNIT_TESTING)
    add_subdirectory(tests)
endif()

###################################################################################
#Package management
###################################################################################

include(InstallRequiredSystemLibraries)

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/license.txt")
#set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/license.txt")
#set(CPACK_RESOURCE_FILE_WELCOME "${CMAKE_CURRENT_SOURCE_DIR}/license.txt")

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Chart design software for crochet")
set(CPACK_PACKAGE_VENDOR "Stitch Works Software")
set(CPACK_PACKAGE_VERSION ${SWS_VERSION})
set(CPACK_PACKAGE_VERSION_MAJOR ${SWS_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${SWS_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${SWS_VERSION_PATCH})
set(CPACK_PACKAGE_INSTALL_DIRECTORY "crochet-${SWS_VERSION_MAJOR}.${SWS_VERSION_MINOR}")

if(${SWS_RELEASE_PLATFORM} STREQUAL "WIN32")
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/images/crochet_icon.png")
    set(CPACK_NSIS_PACKAGE_NAME "Crochet")
    set(CPACK_NSIS_DISPLAY_NAME "Crochet")
    set(CPACK_NSIS_CONTACT "support@stitchworkssoftware.com")
    set(CPACK_PACKAGE_EXECUTABLES "crochet.exe" "Crochet")
    set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${CMAKE_PROJECT_NAME}-${SWS_VERSION_MAJOR}.${SWS_VERSION_MINOR}.${SWS_VERSION_PATCH}")

    set(CPACK_NSIS_MENU_LINKS "http://www.stitchworkssoftware.com" "Homepage for Stitch Works Software")

    # this doesn't work for the NSIS installer
    set(CPACK_CREATE_DESKTOP_LINKS "crochet.exe")
    set(CPACK_NSIS_MODIFY_PATH "ON")

    # Icon in the add/remove control panel. Must be an .exe file
    set(CPACK_NSIS_INSTALLED_ICON_NAME bin\\\\crochet.exe)

    set(CPACK_NSIS_URL_INFO_ABOUT "http://www.stitchworkssoftware.com")
    set(CPACK_NSIS_HELP_LINK "http://www.stitchworkssoftware.com")
elseif(${SWS_RELEASE_PLATFORM} STREQUAL "DARWIN")

else()
    set(CPACK_GENERATOR "DEB;RPM")

    #TODO: finish adding the deb and rpm stuff here.
endif()

include(CPack)

