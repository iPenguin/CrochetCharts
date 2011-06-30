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

if(${SWS_PLATFORM} STREQUAL "WIN32")
    add_executable(${SWS_TARGET_WIN} WIN32 ${crochet_srcs} ${crochet_ui_h} ${crochet_moc_srcs} ${crochet_rcc_srcs}
                ${crochet_version} ${crochet_win})
    target_link_libraries(${SWS_TARGET_WIN} ${QT_LIBS_WINDOWS})
    install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/crochet.exe ${QT_LIBS_WINDOWS} ${QT_DEPS_WINDOWS} DESTINATION .)
    install(DIRECTORY ${QT_PLUGINS_WINDOWS} DESTINATION plugins)
elseif(${SWS_PLATFORM} STREQUAL "DARWIN")
    add_executable(Crochet ${crochet_srcs} ${crochet_ui_h} ${crochet_moc_srcs} ${crochet_rcc_srcs}
            ${crochet_version} ${crochet_mac})
    target_link_libraries(Crochet ${QT_LIBRARIES})
    install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/Crochet DESTINATION ../MacOS)
    
    install(CODE "include(BundleUtilities)
    fixup_bundle(\"${CMAKE_CURRENT_BINARY_DIR}/src/Crochet.app\" \"\" \"${QT_LIBRARIES}\")" COMPONENT Runtime)

    find_program(APPLE_RESOURCE Rez /Developer/Tools)
    if(APPLE_RESOURCE)
        add_custom_command(TARGET Crochet POST_BUILD COMMAND ${APPLE_RESOURCE}
            ARGS Carbon.r -o ${CMAKE_CURRENT_BINARY_DIR}/Crochet COMMENT "Resource Fork")
    endif()

    set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/${MACOSX_BUNDLE_ICON_FILE}"
                PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

    #TODO: use /home/crosscompile/darwin/bin/i686-apple-darwin8-lipo to join the different archs for the mac into 1 file.
    #command: i686-apple-darwin8-lipo -create a.out.i386 a.out.x86_64 a.out.ppc a.out.ppc64 -output a.out
    #Q: would you need to do the same thing for the libraries?
    # execute_process(. COMMAND i686-apple-darwin8-lipo -create a.out.i386 a.out.ppc -output a.out) and add_to_distclean(a.out)

    #install(FILES ${QT_LIBS_DARWIN} DESTINATION Frameworks)
    #install(FILES ${QT_DEPS_DARWIN} DESTINATION Resources)
    #install(DIRECTORY ${QT_PLUGINS_DARWIN} DESTINATION plugins)
else()
    add_executable(crochet ${crochet_srcs} ${crochet_ui_h} ${crochet_moc_srcs} ${crochet_rcc_srcs}
                ${crochet_version} ${crochet_nix})
    target_link_libraries(crochet ${QT_LIBRARIES})
    install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/crochet DESTINATION bin)

endif()
