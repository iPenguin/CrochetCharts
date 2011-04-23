include(DocbookDocGeneration)


message(STATUS "Createing documentation: html")
docbook_generate(html crochet.docbook "${CMAKE_CURRENT_BINARY_DIR}/docs")
message(STATUS "Createing documentation: pdf")
docbook_generate(pdf crochet.docbook "${CMAKE_CURRENT_BINARY_DIR}/docs")
message(STATUS "Createing documentation: pages")
docbook_generate(pages crochet.docbook "${CMAKE_CURRENT_BINARY_DIR}/docs")
message(STATUS "Createing documentation: htmlhelp")
docbook_generate(htmlhelp crochet.docbook "${CMAKE_CURRENT_BINARY_DIR}/docs")

message("-------------------------------------------------------")

#if(APPLE)
#    install("${CMAKE_BINARY_DIR}/docs/apple")
#elseif(WIN32)
#    install("${CMAKE_BINARY_DIR}/docs/win32")
#elseif(UNIX)
#    install("${CMAKE_BINARY_DIR}/docs/linux")
#endif()