include_directories(${QT_INCLUDES} ${CMAKE_BINARY_DIR}/src ${CMAKE_SOURCE_DIR}/src
                    ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

set(QT_USE_QTMAIN true)
set(QT_USE_QTTEST true)

include(${QT_USE_FILE})

file(GLOB crochet_test_srcs "test*.cpp")
file(GLOB crochet_test_qobject_headers "test*.h")

set(crochet_app_h
    ../src/cell.h
    ../src/stitch.h
    ../src/stitchset.h
    ../src/settings.h
    ../src/stitchlibrary.h
    ../src/textview.h
    ../src/appinfo.h
    ../src/scene.h
    ../src/crochetchartcommands.h
    ../src/indicatorundo.h
    ../src/indicator.h
    ../src/chartview.h
    )

set(crochet_app_cpp
    ../src/cell.cpp
    ../src/stitch.cpp
    ../src/stitchset.cpp
    ../src/settings.cpp
    ../src/stitchlibrary.cpp
    ../src/textview.cpp
    ../src/appinfo.cpp
    ../src/scene.cpp
    ../src/crochetchartcommands.cpp
    ../src/indicatorundo.cpp
    ../src/indicator.cpp
    ../src/chartview.cpp
    ${CMAKE_BINARY_DIR}/version.cpp
    )

set(crochet_app_resources
    ../crochet.qrc
    ../stitches/stitches.qrc
    )

qt4_add_resources(crochet_app_rcc_srcs ${crochet_app_resources})
qt4_wrap_cpp(crochet_test_moc_srcs ${crochet_test_qobject_headers} ${crochet_app_h})

add_executable(tests main.cpp ${crochet_test_srcs} ${crochet_test_moc_srcs} ${crochet_app_rcc_srcs})
target_link_libraries(tests ${QT_LIBRARIES})
