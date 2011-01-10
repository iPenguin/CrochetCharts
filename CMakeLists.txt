cmake_minimum_required(VERSION 2.8)
project(Crochet)

# Appends the cmake/modules path to MAKE_MODULE_PATH variable...
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})

#Forces out of source builds just in case.
include(MacroOutOfSourceBuild)
macro_ensure_out_of_source_build("${PROJECT_NAME} requires an out of source build.")

set(CMAKE_CONFIGURATION_TYPES Debug Release DebugProfiling CACHE TYPE INTERNAL FORCE)
set(CMAKE_BUILD_TYPE "Debug")

include(GetGitRevisionDescription)
git_describe(VERSION --tags --dirty=-working)

string(REGEX REPLACE "^([0-9]+)\\..*" "\\1" VERSION_MAJOR "${VERSION}")
string(REGEX REPLACE "^[0-9]+\\.([0-9]+).*" "\\1" VERSION_MINOR "${VERSION}")
string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" VERSION_PATCH "${VERSION}")
string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.[0-9]+(.*)" "\\1" VERSION_SHA1 "${VERSION}")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/version.cpp.in
                ${CMAKE_CURRENT_BINARY_DIR}/version.cpp)

find_package(Qt4 REQUIRED)

option(UNIT_TESTING  "Build QTestLib Unit Tests" OFF)
option(ARCH_32BIT "Force 32-bit Build" OFF)
option(SHADOW_FLAG "Use -Wshadow when compiling" OFF)

message(STATUS "Building " ${PROJECT_NAME} " version - " ${VERSION_MAJOR} "." ${VERSION_MINOR} "." ${VERSION_PATCH} ${VERSION_SHA1})
message(STATUS "Build Profile - " ${CMAKE_BUILD_TYPE})
message(STATUS "Unit Testing - " ${UNIT_TESTING})
message(STATUS "Force 32-bit Builds - " ${ARCH_32BIT})
message(STATUS "Use -Wshadow when compiling - " ${SHADOW_FLAG})

if(ARCH_32BIT)
    set(extra_build_flags "-m32")
endif()

if(SHADOW_FLAG)
    list(APPEND extra_build_flags " -Wshadow")
endif()

set(CMAKE_C_FLAGS "${extra_build_flags} -g -ggdb -W -Wall")
set(CMAKE_CXX_FLAGS ${CMAKE_C_FLAGS})

set(CMAKE_C_FLAGS_DEBUG "${extra_build_flags} -g -ggdb -W -Wall") #-march=athlon64 -march=atom, core2, i686, (-mtune=generic, native)
set(CMAKE_CXX_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})

set(CMAKE_C_FLAGS_RELEASE "${extra_build_flags} -W -Wall -O2")
set(CMAKE_CXX_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})

set(CMAKE_C_FLAGS_DEBUGPROFILING "${extra_build_flags} -g -ggdb -pg -W -Wall")
set(CMAKE_CXX_FLAGS_DEBUGPROFILING ${CMAKE_C_FLAGS_DEBUGPROFILING})
set(CMAKE_EXE_LINKER_FLAGS_DEBUGPROFILING "-pg " ${CMAKE_EXE_LINKER_FLAGS})

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

#More info see: http://cmake.org/cmake/help/cmake2.6docs.html#module:FindQt4
set(QT_USE_QTMAIN true)
set(QT_USE_QTSVG true)
set(QT_USE_QTNETWORK true)
include(${QT_USE_FILE})

#QT4_CREATE_TRANSLATION( qm_files directories ... sources ... ts_files ... OPTIONS ...)
#QT4_ADD_TRANSLATION( qm_files ts_files ... )

add_definitions(${QT_DEFINITIONS})

set(crochet_version
    "${CMAKE_CURRENT_BINARY_DIR}/version.cpp"
    )

if(WIN32)
#http://public.kitware.com/Bug/view.php?id=4068
#if(MINGW)
#  set(CMAKE_RC_COMPILER_INIT windres)
#  ENABLE_LANGUAGE(RC)
#  SET(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <FLAGS> <DEFINES> -o <OBJECT> <SOURCE>")
# ALT: set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")
#endif(MINGW)

    set(crochet_win platforms/win/crochet.rc
        )
elseif(APPLE)

    set(crochet_mac
        )
    set(CMAKE_OSX_ARCHITECTURES "ppc;i386;ppc64;x86_64")
#TODO: make the version come from the VERSION file or from git.
#for more see: http://www.mail-archive.com/cmake@cmake.org/msg05498.html
#and see: http://www.cmake.org/Wiki/CMake:Bundles_And_Frameworks
#plutil command line utility to edit plist files.
#http://rixstep.com/2/20060901,00.shtml
    set(MACOSX_BUNDLE_INFO_STRING "Crochet - version " ${VERSION_MAJOR} "." ${VERSION_MINOR} "." ${VERSION_PATCH})
    set(MACOSX_BUNDLE_BUNDLE_VERSION ${VERSION_MAJOR} "." ${VERSION_MINOR} "." ${VERSION_PATCH})
    set(MACOSX_BUNDLE_ICON_FILE "platforms/mac/crochet.icns")
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.stitchworkssoftware")
    set(MACOSX_BUNDLE_BUNDLE_NAME "Crochet")

    # Need to copy the icon file
    exec_program("mkdir -p ${crochet_BINARY_DIR}/src/ Crochet.app/Contents/Resources")
    exec_program("cp ${crochet_SOURCE_DIR}/platforms/mac/crochet.icns $ {crochet_BINARY_DIR}/src/Crochet.app/Contents/Resources")

    # Overload the Info.plist default
    set(CMAKE_MODULE_PATH ${crochet_SOURCE_DIR}/platforms/mac ${CMAKE_MODULE_PATH})

else(UNIX)
    set(crochet_nix
        )
endif()

add_subdirectory(src)

if(UNIT_TESTING)
    add_subdirectory(tests)
endif()

###################################################################################
#Package management
###################################################################################

include(InstallRequiredSystemLibraries)

set(CPACK_RESOURCE_FILE_LICENSE  "${CMAKE_CURRENT_SOURCE_DIR}/license.txt")
#TODO: get the version information from git.
set(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})

include(CPack)
