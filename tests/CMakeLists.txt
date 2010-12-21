cmake_minimum_required(VERSION 2.8)
project(CrochetTests)

find_package(Qt4 REQUIRED)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

# for more information on the following lines see: http://cmake.org/cmake/help/cmake2.6docs.html#module:FindQt4
set(QT_USE_QTMAIN true)
set(QT_USE_QTTEST true)

include(${QT_USE_FILE})

add_definitions(${QT_DEFINITIONS})

set(crochet_test_srcs
    ../src/stitch.cpp
    testlicense.cpp
    teststitch.cpp
    )

set(crochet_test_headers
    ../src/license.h
    ../src/stitch.h
    testlicense.h
    teststitch.h
    )

qt4_automoc(${crochet_test_srcs})
qt4_wrap_cpp(crochet_test_moc_srcs ${crochet_test_headers})

add_executable(crochet_tests main.cpp ${crochet_test_srcs} ${crochet_test_moc_srcs})
target_link_libraries(crochet_tests ${QT_LIBRARIES})
