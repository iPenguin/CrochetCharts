<?xml version="1.0"?>
<!DOCTYPE book [
<!ENTITY % allent SYSTEM "../build/docs/application.ent">
%allent;
]>

<xsl:stylesheet version='1.0' xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                              xmlns:fo="http://www.w3.org/1999/XSL/Format">

    <xsl:import href="/usr/share/xml/docbook/stylesheet/docbook-xsl-ns/fo/docbook.xsl" />

    
    <xsl:template name="book.titlepage.recto">
        <fo:block xmlns:fo="http://www.w3.org/1999/XSL/Format"
                xsl:use-attribute-sets="book.titlepage.recto.style"
                text-align="center"
                font-size="25pt"
                space-before="18.6624pt"
                font-weight="bold"
                font-family="{$title.font.family}">
            The Official &appName; User Guide
        </fo:block>
        <fo:block xmlns:fo="http://www.w3.org/1999/XSL/Format"
                xsl:use-attribute-sets="book.titlepage.recto.style"
                text-align="center"
                font-size="20pt"
                space-before="18.6624pt"
                font-weight="bold"
                font-family="{$title.font.family}">
            Version &appVersion;
        </fo:block>
        <fo:block text-align="center">
            <fo:external-graphic src="url(images/cover.png)"/>
        </fo:block>
        <fo:block xmlns:fo="http://www.w3.org/1999/XSL/Format"
                xsl:use-attribute-sets="book.titlepage.recto.style"
                text-align="center"
                font-size="12pt"
                space-before="18.6624pt"
                font-weight="bold"
                font-family="{$title.font.family}">
            Published by &sws;
        </fo:block>
        <fo:block xmlns:fo="http://www.w3.org/1999/XSL/Format"
                xsl:use-attribute-sets="book.titlepage.recto.style"
                text-align="center"
                font-size="12pt"
                space-before="18.6624pt"
                font-weight="bold"
                font-family="{$title.font.family}">
            &copyright;
        </fo:block>
    </xsl:template>

</xsl:stylesheet>