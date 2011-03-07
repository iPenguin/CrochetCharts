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

if(${SWS_RELEASE_PLATFORM} STREQUAL "WIN32")
    add_executable(crochet.exe WIN32 ${crochet_srcs} ${crochet_ui_h} ${crochet_moc_srcs} ${crochet_rcc_srcs}
                ${crochet_version} ${crochet_win})
    target_link_libraries(crochet.exe ${QT_LIBS_WINDOWS})
    install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/crochet.exe ${QT_LIBS_WINDOWS} ${QT_DEPS_WINDOWS} DESTINATION bin)
    install(DIRECTORY ${QT_PLUGINS_WINDOWS} DESTINATION plugins)
elseif(${SWS_RELEASE_PLATFORM} STREQUAL "DARWIN")
    add_executable(crochet_mac MACOSX_BUNDLE ${crochet_srcs} ${crochet_ui_h} ${crochet_moc_srcs} ${crochet_rcc_srcs}
                ${crochet_version} ${crochet_mac})
    target_link_libraries(crochet_mac ${QT_LIBS_DARWIN})

    set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/${MACOSX_BUNDLE_ICON_FILE}"
                PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

    install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/crochet_mac RENAME crochet DESTINATION bin)
    install(FILES ${QT_LIBS_DARWIN} ${QT_DEPS_DARWIN} DESTINATION libs)
    install(DIRECTORY ${QT_PLUGINS_DARWIN} DESTINATION plugins)
else()
    add_executable(crochet ${crochet_srcs} ${crochet_ui_h} ${crochet_moc_srcs} ${crochet_rcc_srcs}
                ${crochet_version} ${crochet_nix})
    target_link_libraries(crochet ${QT_LIBRARIES})
    install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/crochet DESTINATION bin)
    #depends(qt4 >= 4.7)
endif()