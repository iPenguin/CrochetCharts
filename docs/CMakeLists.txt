include(DocbookGen)

if(DOCS)
    configure_file(${CMAKE_SOURCE_DIR}/docs/index.docbook.in
                ${CMAKE_CURRENT_BINARY_DIR}/index.docbook)
    
    set(docbookFile "${CMAKE_CURRENT_BINARY_DIR}/index.docbook")
    
    message(STATUS "Createing documentation: html")
    docbook_generate(html "${docbookFile}" "${VERSION_SHORT}")
    message(STATUS "Createing documentation: pdf")
    docbook_generate(pdf "${docbookFile}" "${VERSION_SHORT}")
    
    if(APPLE)
        message(STATUS "Createing documentation: pages")
        docbook_generate(pages "${docbookFile}" "${VERSION_SHORT}")
    elseif(WIN32)
        message(STATUS "Createing documentation: htmlhelp")
        #docbook_generate(htmlhelp "${docbookFile}" "${VERSION_SHORT}")
    endif()
    
    message("-------------------------------------------------------")
endif()

if(WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/docs/pdf/${EXE_NAME}_User_Guide_${VERSION_SHORT}.pdf" DESTINATION bin)
    #install(FILES "${CMAKE_BINARY_DIR}/docs/htmlhelp/${EXE_NAME}.chm" DESTINATION docs)
    install(FILES "${CMAKE_SOURCE_DIR}/docs/homepage.html" DESTINATION docs)

elseif(APPLE)
    install(FILES "${CMAKE_BINARY_DIR}/docs/pdf/${EXE_NAME}_User_Guide_${VERSION_SHORT}.pdf" DESTINATION ../MacOS)
else()
    install(FILES "${CMAKE_BINARY_DIR}/docs/pdf/${PROJECT_NAME}_User_Guide_${VERSION_SHORT}.pdf" DESTINATION share/${PROJECT_NAME})
endif()
