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
qt4_wrap_cpp(crochet_moc_srcs ${crochet_qobject_headers})

if(WIN32)
    set(WIN32_BASE "C:/Qt/2010.05/qt")

    #generate the embeded icon for the exe file.
    set(CMAKE_RC_COMPILER_INIT windres)
    enable_language(RC)
    set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")
    set(crochet_win "${CMAKE_SOURCE_DIR}/crochet.rc")
    
    set(WIN32_LIBS "${WIN32_BASE}/bin")
    set(WIN32_PLUGINS "${WIN32_BASE}/plugins")

    set(QT_LIBS_WINDOWS "${WIN32_LIBS}/QtSvg4.dll" "${WIN32_LIBS}/QtCore4.dll" "${WIN32_LIBS}/QtGui4.dll"
                        "${WIN32_LIBS}/QtNetwork4.dll" "${WIN32_LIBS}/QtXml4.dll")
    set(QT_DEPS_WINDOWS "${WIN32_LIBS}/libgcc_s_dw2-1.dll" "${WIN32_LIBS}/mingwm10.dll"
                        "${CMAKE_SOURCE_DIR}/resources/qt.conf")
                    #"C:/OpenSSL-Win32/libeay32.dll" "C:/OpenSSL-Win32/ssleay32.dll")

    set(QT_PLUGINS_WINDOWS "${WIN32_PLUGINS}/imageformats" "${WIN32_PLUGINS}/accessible" "${WIN32_PLUGINS}/iconengines")

elseif(APPLE)

    file(WRITE "${CMAKE_BINARY_DIR}/qt.conf" "[Paths]\nPlugins = plugins\n")
    install(FILES "${CMAKE_BINARY_DIR}/qt.conf" DESTINATION .)

    set(CMAKE_OSX_ARCHITECTURES "i386;x86_64") #";x86_64;ppc64;")

    set(DARWIN_LIBS "/Developer/SDKs/MacOSX10.4u.sdk/Library/Frameworks/")

    set(crochet_mac)

else()

    set(crochet_nix)

endif()

add_executable(${PROJECT_NAME} WIN32 ${crochet_srcs} ${crochet_ui_h} ${crochet_moc_srcs} ${crochet_rcc_srcs}
            ${crochet_version} ${crochet_win} ${crochet_mac} ${crochet_nix})
target_link_libraries(${PROJECT_NAME} ${QT_LIBRARIES})

if(WIN32)

    install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.exe ${QT_LIBS_WINDOWS} ${QT_DEPS_WINDOWS} DESTINATION bin)
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
