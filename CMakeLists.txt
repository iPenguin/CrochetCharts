cmake_minimum_required(VERSION 2.8.6)
if(APPLE OR WIN32)
    project("Crochet Charts")
else()
    project("CrochetCharts")
endif()

if(WIN32)
    set(EXE_NAME "Crochet_Charts")
else()
    set(EXE_NAME ${PROJECT_NAME})
endif()

# Appends the cmake/modules path to MAKE_MODULE_PATH variable...
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})

#Forces out of source builds just in case.
include(MacroOutOfSourceBuild)
macro_ensure_out_of_source_build("${PROJECT_NAME} requires an out of source build.")

set(CMAKE_CONFIGURATION_TYPES Debug Release Profile CACHE TYPE INTERNAL FORCE)
if(CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Debug")
endif()

set (HUNSPELL_FIND_QUIETLY TRUE)
include(FindHunSpell)

include(GetGitRevisionDescription)
git_describe(VERSION --tags --dirty=-w)

string(REGEX REPLACE "^([0-9]+)\\..*" "\\1" VERSION_MAJOR "${VERSION}")
string(REGEX REPLACE "^[0-9]+\\.([0-9]+).*" "\\1" VERSION_MINOR "${VERSION}")
string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" VERSION_PATCH "${VERSION}")
string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.[0-9]+(.*)" "\\1" VERSION_SHA1 "${VERSION}")
set(VERSION_SHORT "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

set(PROJECT_LIFE         "2010-2013")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/version.cpp.in
                ${CMAKE_CURRENT_BINARY_DIR}/version.cpp)
set(crochet_version "${CMAKE_CURRENT_BINARY_DIR}/version.cpp")

find_package(Qt4 REQUIRED)

option(UNIT_TESTING  "Build the test libraries for this software" OFF)
option(FORCE_32BIT "Force 32-bit builds when cross compiling with distcc (needed for 64-bit compile nodes)" OFF)
option(DOCS "Create docbook documentation" OFF)
option(DOXYGEN "Create Doxygen documentation" OFF)
option(APP_STORE "Create an apple app store version" OFF)
set(CMAKE_OSX_ARCHITECTURES "i386;x86_64")

if(FORCE_32BIT)
    set(extra_build_flags "-m32")
    set(extra_link_flags "-m elf_i386")
    set(extra_build_flags "${extra_build_flags} -g -gdwarf-2")
endif()

if((APPLE OR UNIX) AND NOT FORCE_32BIT)
    set(extra_build_flags "${extra_build_flags} -g -gdwarf-2 -Qunused-arguments -fcolor-diagnostics")
else()
    set(extra_build_flags "${extra_build_flags} -g -ggdb")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(extra_build_flags "${extra_build_flags} -O2")
else()
    set(extra_build_flags "${extra_build_flags} -fsanitize=address -O1 -fno-omit-frame-pointer -fno-optimize-sibling-calls")
    set(extra_link_flags "${extra_link_flags} -g -fsanitize=address")
endif()

set(CMAKE_C_FLAGS "${extra_build_flags} -W -Wall")  #") #-ansi -pedantic
set(CMAKE_CXX_FLAGS ${CMAKE_C_FLAGS})
set(CMAKE_EXE_LINKER_FLAGS "${extra_link_flags}")

set(CMAKE_C_FLAGS_DEBUG "${extra_build_flags} -W -Wall")
set(CMAKE_CXX_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${extra_link_flags} ${CMAKE_EXE_LINKER_FLAGS}")

set(CMAKE_C_FLAGS_RELEASE "${extra_build_flags} -W -Wall")
set(CMAKE_CXX_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${extra_link_flags} ${CMAKE_EXE_LINKER_FLAGS}")

set(CMAKE_C_FLAGS_PROFILE "${extra_build_flags} -pg -W -Wall")
set(CMAKE_CXX_FLAGS_PROFILE ${CMAKE_C_FLAGS_PROFILE})
set(CMAKE_EXE_LINKER_FLAGS_PROFILE "-pg ${extra_link_flags} ${CMAKE_EXE_LINKER_FLAGS}")

message("-------------------------------------------------------")
message(STATUS ${PROJECT_NAME} " Version: " ${VERSION})
message(STATUS "CMake Build Type: " ${CMAKE_BUILD_TYPE})
message(STATUS "Create Documentation: " ${DOCS})
message(STATUS "Force 32bit: " ${FORCE_32BIT})
message(STATUS "App Store version: " ${APP_STORE})
message(STATUS "Unit Testing: " ${UNIT_TESTING})
message(STATUS "Found Hunspell: " ${HUNSPELL_FOUND})
message(STATUS "Doxygen Docs: " ${DOXYGEN})
message(STATUS "Build flags: ${CMAKE_CXX_FLAGS}")
message(STATUS "Linker flags: ${CMAKE_EXE_LINKER_FLAGS}")
message("-------------------------------------------------------")

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

#More info see: http://cmake.org/cmake/help/cmake2.6docs.html#module:FindQt4
if(WIN32)
    set(QT_USE_QTMAIN true)
endif()

set(QT_USE_QTSVG true)
set(QT_USE_QTNETWORK true)
include(${QT_USE_FILE})

#QT4_CREATE_TRANSLATION( qm_files directories ... sources ... ts_files ... OPTIONS ...)
#QT4_ADD_TRANSLATION( qm_files ts_files ... )

add_definitions(${QT_DEFINITIONS})

set(CMAKE_AUTOMOC TRUE)

###################################################################################
#CPack includes to create installers
###################################################################################
include("${CMAKE_CURRENT_SOURCE_DIR}/resources/installers.cmake")

if(DOCS)
    add_subdirectory(docs)
endif()

if(UNIT_TESTING)
    add_subdirectory(tests)
endif()

add_subdirectory(src)

set(CPACK_BINARY_DRAGNDROP ON)
include(CPack)
