include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})


file(GLOB crochet_srcs "*.cpp")
file(GLOB crochet_uis "*.ui")

set(crochet_qobject_headers
    settings.h
    mainwindow.h
    cell.h
    crochettab.h
    crochetcell.h
    mainwindow.h
    stitchset.h
    crochetscene.h
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
    crochettextview.h
    crochethighlighter.h
    updater.h
    stitchiconui.h
    undogroup.h
    tabinterface.h
    legends.h
    )

set(crochet_resources
    ../crochet.qrc
    )

qt4_add_resources(crochet_rcc_srcs ${crochet_resources})
qt4_wrap_ui(crochet_ui_h ${crochet_uis})
qt4_wrap_cpp(crochet_moc_srcs ${crochet_qobject_headers} ${testmodel_headers})

if(WINDOWS)
    add_executable(crochet.exe WIN32 ${crochet_srcs} ${crochet_ui_h} ${crochet_moc_srcs} ${crochet_rcc_srcs}
                ${crochet_version} ${crochet_win})
    target_link_libraries(crochet.exe "/home/brian/qt-win32/bin/QtSvg4.dll" "/home/brian/qt-win32/bin/QtCore4.dll"
        "/home/brian/qt-win32/bin/QtGui4.dll" "/home/brian/qt-win32/bin/QtNetwork4.dll" "/home/brian/qt-win32/bin/QtXml4.dll")
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/crochet.exe "/home/brian/qt-win32/bin/QtSvg4.dll" "/home/brian/qt-win32/bin/QtCore4.dll"
        "/home/brian/qt-win32/bin/QtGui4.dll" "/home/brian/qt-win32/bin/QtNetwork4.dll" "/home/brian/qt-win32/bin/QtXml4.dll"
        "/home/brian/qt-win32/bin/libgcc_s_dw2-1.dll" "/home/brian/qt-win32/bin/mingwm10.dll" DESTINATION bin)

else()
    add_executable(crochet MACOSX_BUNDLE ${crochet_srcs} ${crochet_ui_h} ${crochet_moc_srcs} ${crochet_rcc_srcs}
                ${crochet_version} ${crochet_mac} ${crochet_nix})
    target_link_libraries(crochet ${QT_LIBRARIES})
endif()