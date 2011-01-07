include_directories(${QT_INCLUDES} ${CMAKE_BINARY_DIR}/src ${CMAKE_SOURCE_DIR}/src
                    ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

set(QT_USE_QTMAIN true)
set(QT_USE_QTTEST true)

include(${QT_USE_FILE})

file(GLOB crochet_test_srcs "test*.cpp")
file(GLOB crochet_test_qobject_headers "test*.h")

set(crochet_app_h
    ../src/license.h
    ../src/cell.h
    ../src/stitch.h
    ../src/crochetdatamodel.h
    ../src/stitchset.h
    )

set(crochet_app_cpp
    ../src/license.cpp
    ../src/cell.cpp
    ../src/stitch.cpp
    ../src/crochetdatamodel.cpp
    ../src/stitchset.cpp
    )

set(crochet_app_resources
    ../crochet.qrc
    )

qt4_add_resources(crochet_app_rcc_srcs ${crochet_app_resources})
qt4_wrap_cpp(crochet_test_moc_srcs ${crochet_test_qobject_headers} ${crochet_app_h})

add_executable(crochet_tests main.cpp ${crochet_test_srcs} ${crochet_test_moc_srcs} ${crochet_app_cpp} ${crochet_app_rcc_srcs})
target_link_libraries(crochet_tests ${QT_LIBRARIES})
