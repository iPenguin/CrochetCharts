#
# This file contains functions to generate documentaion from docbook files.
#
# by: Brian C. Milco <bcmilco@gmail.com>
#
#

function(_DOCBOOK_HTML input)
#xsltproc -o index.html /usr/share/xml/docbook/stylesheet/nwalsh/html/docbook.xsl ../../docs/crochet.docbook
    set(xslFile "/usr/share/xml/docbook/stylesheet/nwalsh/html/docbook.xsl")

    execute_process(
        COMMAND "/usr/bin/xsltproc" -o ${CMAKE_BINARY_DIR}/docs/html/index.html "${xslFile}" "${CMAKE_CURRENT_SOURCE_DIR}/${input}"
        OUTPUT_FILE ${CMAKE_BINARY_DIR}/docs/html/index.html
    )

endfunction()


function(_DOCBOOK_PDF input)

    set(outputBaseName "${CMAKE_BINARY_DIR}/docs/pdf/userGuide")
    set(xslFile "/usr/share/xml/docbook/stylesheet/nwalsh/fo/docbook.xsl")

    execute_process(
        COMMAND "/usr/bin/xsltproc" -o ${outputBaseName}.fo --stringparam use.extensions 0 --stringparam fop1.extensions 1 ${xslFile} "${CMAKE_CURRENT_SOURCE_DIR}/${input}"
        OUTPUT_FILE ${outputBaseName}.fo
    )

    execute_process(
        COMMAND "/usr/bin/fop" -fo "${outputBaseName}.fo" -pdf "${outputBaseName}.pdf"
        OUTPUT_FILE ${outputBaseName}.pdf
    )

endfunction()


function(_DOCBOOK_PAGES input)
#xsltproc -o index.xml --stringparam pages.template template-pages.xml dbk2pages.xsl my-docbook.xml
    set(xslFile "/usr/share/xml/docbook/stylesheet/docbook-xsl/roundtrip/dbk2pages.xsl")
    execute_process(
        COMMAND "/usr/bin/xsltproc" -o ${CMAKE_BINARY_DIR}/docs/pages/index.xml --stringparam pages.template template-pages.xml ${xslFile} "${CMAKE_CURRENT_SOURCE_DIR}/${input}"
        OUTPUT_FILE ${CMAKE_BINARY_DIR}/docs/pages/index.xml
    )
endfunction()


function(_DOCBOOK_HTMLHELP input)
#xsltproc /home/brian/Downloads/docbook-xsl-1.76.1/htmlhelp/htmlhelp.xsl ../../docs/crochet.docbook
    set(xslFile "/usr/share/xml/docbook/stylesheet/docbook-xsl/htmlhelp/htmlhelp.xsl")
    execute_process(
        COMMAND "/usr/bin/xsltproc" ${xslFile} "${CMAKE_CURRENT_SOURCE_DIR}/${input}"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/htmlhelp/
    )
endfunction()


function(DOCBOOK_GENERATE format inFile)
    if(format STREQUAL "html")
        _docbook_html("${inFile}")
    elseif(format STREQUAL "pdf")
        _docbook_pdf("${inFile}")
    elseif(format STREQUAL "pages")
        _docbook_pages("${inFile}")
    elseif(format STREQUAL "htmlhelp")
        _docbook_htmlhelp("${inFile}")
    else()
        message ( FATAL_ERROR "Unsupported docbook output format." )

    endif()

    set(${outList} ${${outList}} PARENT_SCOPE)

endfunction()
