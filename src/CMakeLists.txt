include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB crochet_srcs "*.cpp")
file(GLOB crochet_uis "*.ui")

set(crochet_qobject_headers
    settings.h
    mainwindow.h
    cell.h
    crochettab.h
    mainwindow.h
    stitchset.h
    licensewizard.h
    licensevalidator.h
    settingsui.h
    splashscreen.h
    licensehttp.h
    exportui.h
    stitchlibrary.h
    stitchlibraryui.h
    stitchlibrarydelegate.h
    stitchpalettedelegate.h
    chartview.h
    textview.h
    updater.h
    stitchiconui.h
    undogroup.h
    tabinterface.h
    legends.h
    scene.h
    roweditdialog.h
    rowsdock.h
    aligndock.h
    mirrordock.h
    indicator.h
    )

set(crochet_resources
    ../crochet.qrc
    ../stitches/stitches.qrc
    )

qt4_add_resources(crochet_rcc_srcs ${crochet_resources})
qt4_wrap_ui(crochet_ui_h ${crochet_uis})
qt4_wrap_cpp(crochet_moc_srcs ${crochet_qobject_headers})

if(WIN32)
    #generate the embeded icon for the exe file.
    set(CMAKE_RC_COMPILER_INIT windres)
    enable_language(RC)
    set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")
    set(crochet_win "${CMAKE_SOURCE_DIR}/crochet.rc")

    set(QT_WIN_LIBS "C://Qt//4.7.3//bin")
    
    set(QT_LIBS_WINDOWS "${QT_WIN_LIBS}//QtSvg4.dll" "${QT_WIN_LIBS}//QtCore4.dll" "${QT_WIN_LIBS}//QtGui4.dll"
                        "${QT_WIN_LIBS}//QtNetwork4.dll" "${QT_WIN_LIBS}//QtXml4.dll")
    set(QT_DEPS_WINDOWS "${QT_WIN_LIBS}//libgcc_s_dw2-1.dll" "${QT_WIN_LIBS}//mingwm10.dll"
                        "${CMAKE_SOURCE_DIR}/resources/qt.conf")

    set(QT_PLUGINS_WINDOWS "${QT_PLUGINS_DIR}/imageformats" "${QT_PLUGINS_DIR}/accessible" "${QT_PLUGINS_DIR}/iconengines")

else()

    set(crochet_nix)

endif()

add_executable(${PROJECT_NAME} WIN32 ${crochet_srcs} ${crochet_ui_h} ${crochet_moc_srcs} ${crochet_rcc_srcs}
            ${crochet_version} ${crochet_win} ${crochet_mac} ${crochet_nix})
target_link_libraries(${PROJECT_NAME} ${QT_LIBRARIES})

if(APPLE)
    install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME} DESTINATION ../MacOS)

    install(CODE "
        execute_process(
            COMMAND \"/usr/bin/macdeployqt\"
            \"@CMAKE_BINARY_DIR@/_CPack_Packages/Darwin/Bundle/@PROJECT_NAME@-@SWS_VERSION_SHORT@/@PRJ_NAME@.app/\"
            WORKING_DIRECTORY \"@CMAKE_BINARY_DIR@\"
            OUTPUT_VARIABLE _output)
    " COMPONENT Runtime)

else()
    install(TARGETS ${PROJECT_NAME} BUNDLE DESTINATION . COMPONENT Runtime RUNTIME DESTINATION bin COMPONENT Runtime)
endif()

if(WIN32)
    install(PROGRAMS ${QT_LIBS_WINDOWS} ${QT_DEPS_WINDOWS} DESTINATION bin)
    install(DIRECTORY ${QT_PLUGINS_WINDOWS} DESTINATION plugins)
endif()
