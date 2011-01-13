include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB crochet_srcs "*.cpp")

set(crochet_qobject_headers
    settings.h
    mainwindow.h
    cell.h
    charttab.h
    crochetcell.h
    crochetdatamodel.h
    mainwindow.h
    stitchset.h
    crochetscene.h
    licensewizard.h
    licensevalidator.h
    undocommands.h
    settingsui.h
    splashscreen.h
    licensehttp.h
    exportui.h
    stitchcollection.h
    stitchlibraryui.h
    stitchlibrarydelegate.h
    stitchpalettedelegate.h
    )

set(crochet_headers
    version.h
    appinfo.h
    license.h
    stitch.h
    )

set(crochet_uis
    export.ui
    mainwindow.ui
    stitcheditor.ui
    stitchlibrary.ui
    newdocument.ui
    settings.ui
    )

set(crochet_resources
    ../crochet.qrc
    )

qt4_add_resources(crochet_rcc_srcs ${crochet_resources})
qt4_wrap_ui(crochet_ui_h ${crochet_uis})
qt4_wrap_cpp(crochet_moc_srcs ${crochet_qobject_headers} ${testmodel_headers})

add_executable(crochet WIN32 MACOSX_BUNDLE ${crochet_srcs} ${crochet_ui_h} ${crochet_moc_srcs} ${crochet_rcc_srcs}
                ${crochet_version} ${crochet_win} ${crochet_mac} ${crochet_nix} ${testmodel_srcs}
                )
target_link_libraries(crochet ${QT_LIBRARIES})
