cmake_minimum_required(VERSION 2.8)
project(Crochet)

find_package(Qt4 REQUIRED)

# The version number.
set (Crochet_VERSION_MAJOR 0)
set (Crochet_VERSION_MINOR 1)
set (Crochet_VERSION_REVISION 0)

set(CMAKE_CXX_FLAGS "-g -Wall")
set(CMAKE_C_FLAGS   "-g -Wall")

#add_subdirectory(src)

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

#set(crochet_test_src
#    src/tests/testcrochetdatamodel.cpp
#    src/tests/teststitch.cpp
#    )

#set(crochet_test_h
#    src/tests/testcrochetdatamodel.h
#    src/tests/teststitch.h
#    )

qt4_wrap_ui(crochet_ui_h ${crochet_uis})
qt4_wrap_cpp(crochet_moc_srcs ${crochet_headers})
qt4_add_resources(crochet_rcc_srcs ${crochet_resources})

add_executable(crochet ${crochet_srcs} ${crochet_ui_h} ${crochet_moc_srcs} ${crochet_rcc_srcs})
target_link_libraries(crochet ${QT_LIBRARIES})


#add_subdirectory(tests)
