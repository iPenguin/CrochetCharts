include(DocbookGen)

configure_file(${CMAKE_SOURCE_DIR}/cmake/modules/DocbookEntities.ent.in
			${CMAKE_CURRENT_BINARY_DIR}/application.ent)

message(STATUS "Createing documentation: html")
docbook_generate(html "${CMAKE_CURRENT_SOURCE_DIR}/index.docbook" "${SWS_VERSION}")
message(STATUS "Createing documentation: pdf")
docbook_generate(pdf "${CMAKE_CURRENT_SOURCE_DIR}/index.docbook" "${SWS_VERSION_MAJOR}.${SWS_VERSION_MINOR}.${SWS_VERSION_PATCH}")

if(APPLE)
    #message(STATUS "Createing documentation: pages")
    #docbook_generate(pages "${CMAKE_CURRENT_SOURCE_DIR}/index.docbook" "${SWS_VERSION_MAJOR}.${SWS_VERSION_MINOR}.${SWS_VERSION_PATCH}")
elseif(WIN32)
    #message(STATUS "Createing documentation: htmlhelp")
    #docbook_generate(htmlhelp "${CMAKE_CURRENT_SOURCE_DIR}/index.docbook" "${SWS_VERSION_MAJOR}.${SWS_VERSION_MINOR}.${SWS_VERSION_PATCH}")
endif()

message("-------------------------------------------------------")

if(WIN32)
    install(FILES "${CMAKE_BINARY_DIR}/docs/pdf/${PROJECT_NAME}_User_Guide_${SWS_VERSION_MAJOR}.${SWS_VERSION_MINOR}.${SWS_VERSION_PATCH}.pdf" DESTINATION .)
elseif(APPLE)
    install(FILES "${CMAKE_BINARY_DIR}/docs/pdf/${PROJECT_NAME}_User_Guide_${SWS_VERSION_MAJOR}.${SWS_VERSION_MINOR}.${SWS_VERSION_PATCH}.pdf" DESTINATION ../MacOS)
else()
    install(FILES "${CMAKE_BINARY_DIR}/docs/pdf/${PROJECT_NAME}_User_Guide_${SWS_VERSION_MAJOR}.${SWS_VERSION_MINOR}.${SWS_VERSION_PATCH}.pdf" DESTINATION bin)
endif()
