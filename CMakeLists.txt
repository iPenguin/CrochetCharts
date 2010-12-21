cmake_minimum_required(VERSION 2.8)
project(Crochet)

find_package(Qt4 REQUIRED)

set(CMAKE_CXX_FLAGS "-g -Wall")
set(CMAKE_C_FLAGS   "-g -Wall")

option(CODE_PROFILING "Profile the code using gprof" false)
option(UNIT_TESTING  "Build QTestLib Unit Tests" false)

message("-- Code Profiling - " ${CODE_PROFILING})
message("-- Unit Testing   - " ${UNIT_TESTING})

if(CODE_PROFILING)
    set(CMAKE_CXX_FLAGS "-g -pg -Wall")
    set(CMAKE_C_FLAGS "-g -pg -Wall")
    set(CMAKE_EXE_LINKER_FLAGS "-pg " ${CMAKE_EXE_LINKER_FLAGS})
endif()

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

# for more information on the following lines see: http://cmake.org/cmake/help/cmake2.6docs.html#module:FindQt4
set(QT_USE_QTSVG 1)
include(${QT_USE_FILE})

add_definitions(${QT_DEFINITIONS})

set(crochet_srcs
    src/cell.cpp
    src/charttab.cpp
    src/crochetcell.cpp
    src/crochetdatamodel.cpp
    src/main.cpp
    src/mainwindow.cpp
    src/tabinterface.cpp
    src/stitch.cpp
    src/stitchset.cpp
    src/crochetscene.cpp
    src/licensewizard.cpp
    )

set(crochet_headers
    src/appinfo.h
    src/mainwindow.h
    src/cell.h
    src/charttab.h
    src/crochetcell.h
    src/crochetdatamodel.h
    src/mainwindow.h
    src/tabinterface.h
    src/stitch.h
    src/stitchset.h
    src/crochetscene.h
    src/licensewizard.h
    src/license.h
    )
 
set(crochet_uis
    src/export.ui
    src/mainwindow.ui
    src/stitcheditor.ui
    src/stitchlibrary.ui
    src/newdocument.ui
    )

set(crochet_resources
    crochet.qrc
    )

if(WIN32)
#http://public.kitware.com/Bug/view.php?id=4068
#if(MINGW)
#  set(CMAKE_RC_COMPILER_INIT windres)
#  ENABLE_LANGUAGE(RC)
#  SET(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <FLAGS> <DEFINES> -o <OBJECT> <SOURCE>")
# ALT: set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")
#endif(MINGW)

    set(crochet_win crochet.rc
        )
elseif(APPLE)
    set(crochet_mac
        )
else(UNIX)
    set(crochet_nix
        )
endif()

qt4_wrap_ui(crochet_ui_h ${crochet_uis})
qt4_wrap_cpp(crochet_moc_srcs ${crochet_headers})
qt4_add_resources(crochet_rcc_srcs ${crochet_resources})

add_executable(crochet ${crochet_srcs} ${crochet_ui_h} ${crochet_moc_srcs} ${crochet_rcc_srcs}
                ${crochet_win} ${crochet_mac} ${crochet_nix}
                )
target_link_libraries(crochet ${QT_LIBRARIES})

if(UNIT_TESTING)
    add_subdirectory(tests)
endif()
