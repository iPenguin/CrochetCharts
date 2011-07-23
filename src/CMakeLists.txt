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
    #indicator.h
    )

set(crochet_resources
    ../crochet.qrc
    )

qt4_add_resources(crochet_rcc_srcs ${crochet_resources})
qt4_wrap_ui(crochet_ui_h ${crochet_uis})
qt4_wrap_cpp(crochet_moc_srcs ${crochet_qobject_headers} ${testmodel_headers})

    add_executable(${PROJECT_NAME} WIN32 ${crochet_srcs} ${crochet_ui_h} ${crochet_moc_srcs} ${crochet_rcc_srcs}
                ${crochet_version} ${crochet_win} ${crochet_mac} ${crochet_nix})
    target_link_libraries(${PROJECT_NAME} ${QT_LIBRARIES})

if(WIN32)

    install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.exe ${QT_LIBS_WINDOWS} ${QT_DEPS_WINDOWS} DESTINATION .)
    install(DIRECTORY ${QT_PLUGINS_WINDOWS} DESTINATION plugins)
    
elseif(APPLE)
    install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME} DESTINATION ../MacOS)

    set(LIBS QtCore QtGui QtNetwork QtSvg QtXml)
    foreach(support_lib ${LIBS})
        set(support_path ${support_lib}.framework/Versions/4)
        install(FILES "${DARWIN_LIBS}/${support_path}/${support_lib}" DESTINATION ../Frameworks/${support_path} COMPONENT Runtime)
    endforeach()

    install(DIRECTORY "${DARWIN_LIBS}/QtGui.framework/Resources/qt_menu.nib" DESTINATION ../Frameworks/QtGui.framework/Resources COMPONENT Runtime)

    install(DIRECTORY "${QT_PLUGINS_DIR}/imageformats" DESTINATION ../PlugIns COMPONENT Runtime)
    install(DIRECTORY "${QT_PLUGINS_DIR}/accessible" DESTINATION ../PlugIns COMPONENT Runtime)
    install(DIRECTORY "${QT_PLUGINS_DIR}/iconengines" DESTINATION ../PlugIns COMPONENT Runtime)

    find_program(APPLE_RESOURCE Rez /Developer/Tools)
    if(APPLE_RESOURCE)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${APPLE_RESOURCE}
            ARGS Carbon.r -o ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME} COMMENT "Resource Fork")
    endif()

    set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/${MACOSX_BUNDLE_ICON_FILE}"
                PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

else()

    install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME} DESTINATION bin)

endif()
