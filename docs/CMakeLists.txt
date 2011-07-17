include(DocbookGen)

configure_file(${CMAKE_SOURCE_DIR}/docs/index.docbook.in
            ${CMAKE_CURRENT_BINARY_DIR}/index.docbook)

set(docbookFile "${CMAKE_CURRENT_BINARY_DIR}/index.docbook")

message(STATUS "Createing documentation: html")
docbook_generate(html "${docbookFile}" "${SWS_VERSION_SHORT}")
message(STATUS "Createing documentation: pdf")
docbook_generate(pdf "${docbookFile}" "${SWS_VERSION_SHORT}")

if(APPLE)
    #message(STATUS "Createing documentation: pages")
    #docbook_generate(pages "${CMAKE_CURRENT_SOURCE_DIR}/index.docbook" "${SWS_VERSION_SHORT}")
elseif(WIN32)
    #message(STATUS "Createing documentation: htmlhelp")
    #docbook_generate(htmlhelp "${CMAKE_CURRENT_SOURCE_DIR}/index.docbook" "${SWS_VERSION_SHORT}")
endif()

message("-------------------------------------------------------")

if(WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/docs/pdf/${PROJECT_NAME}_User_Guide_${SWS_VERSION_SHORT}.pdf" DESTINATION .)
elseif(APPLE)
    install(FILES "${CMAKE_BINARY_DIR}/docs/pdf/${PROJECT_NAME}_User_Guide_${SWS_VERSION_SHORT}.pdf" DESTINATION ../MacOS)
else()
    install(FILES "${CMAKE_BINARY_DIR}/docs/pdf/${PROJECT_NAME}_User_Guide_${SWS_VERSION_SHORT}.pdf" DESTINATION bin)
endif()
