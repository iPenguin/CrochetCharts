include(DocbookDocGeneration)

configure_file(${CMAKE_SOURCE_DIR}/cmake/modules/DocbookEntities.ent.in
                ${CMAKE_CURRENT_BINARY_DIR}/application.ent)

message(STATUS "Createing documentation: html")
docbook_generate(html "${CMAKE_CURRENT_SOURCE_DIR}/index.docbook" "${CMAKE_CURRENT_BINARY_DIR}/docs")
message(STATUS "Createing documentation: pdf")
docbook_generate(pdf "${CMAKE_CURRENT_SOURCE_DIR}/index.docbook" "${CMAKE_CURRENT_BINARY_DIR}/docs")
message(STATUS "Createing documentation: pages")
docbook_generate(pages "${CMAKE_CURRENT_SOURCE_DIR}/index.docbook" "${CMAKE_CURRENT_BINARY_DIR}/docs")
message(STATUS "Createing documentation: htmlhelp")
docbook_generate(htmlhelp "${CMAKE_CURRENT_SOURCE_DIR}/index.docbook" "${CMAKE_CURRENT_BINARY_DIR}/docs")

message("-------------------------------------------------------")

#if(APPLE)
#    install("${CMAKE_BINARY_DIR}/docs/apple")
#elseif(WIN32)
#    install("${CMAKE_BINARY_DIR}/docs/win32")
#elseif(UNIX)
#    install("${CMAKE_BINARY_DIR}/docs/linux")
#endif()
