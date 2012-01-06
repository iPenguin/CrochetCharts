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
    ../src/center.h
    ../src/stitch.h
    ../src/stitchset.h
    ../src/settings.h
    ../src/stitchlibrary.h
    ../src/appinfo.h
    ../src/debug.h
    )

set(crochet_app_cpp
    ../src/license.cpp
    ../src/cell.cpp
    ../src/center.cpp
    ../src/stitch.cpp
    ../src/stitchset.cpp
    ../src/settings.cpp
    ../src/stitchlibrary.cpp
    ../src/version.h
    ../src/appinfo.cpp
    ../src/debug.cpp
    ${CMAKE_BINARY_DIR}/version.cpp
    )

set(crochet_app_resources
    ../crochet.qrc
    ../stitches/stitches.qrc
    )

qt4_add_resources(crochet_app_rcc_srcs ${crochet_app_resources})

add_executable(tests main.cpp ${crochet_test_srcs} ${crochet_app_cpp} ${crochet_app_rcc_srcs})
target_link_libraries(tests ${QT_LIBRARIES})
