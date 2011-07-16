<?xml version="1.0"?>
<!DOCTYPE book [
<!ENTITY % allent SYSTEM "../build/docs/application.ent">
%allent;
]>

<xsl:stylesheet version='1.0' xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                              xmlns:fo="http://www.w3.org/1999/XSL/Format"
                              xmlns:d="http://docbook.org/ns/docbook">

    <xsl:import href="/usr/share/xml/docbook/stylesheet/docbook-xsl-ns/fo/docbook.xsl" />

    <xsl:template name="book.titlepage.recto">

        <xsl:apply-templates mode="book.titlepage.recto.auto.mode" select="d:bookinfo/d:title"/>

        <fo:block text-align="center">
            <fo:external-graphic src="url(images/cover.png)"/>
        </fo:block>

        <xsl:apply-templates mode="book.titlepage.recto.auto.mode" select="d:bookinfo/d:subtitle"/>
        <xsl:apply-templates mode="book.titlepage.recto.auto.mode" select="d:bookinfo/d:author/d:affiliation/d:orgname"/>
    
    </xsl:template>

    <xsl:template match="d:orgname" mode="book.titlepage.recto.auto.mode">
        <fo:block xsl:use-attribute-sets="book.titlepage.recto.style">
            <xsl:text>by </xsl:text>
            <xsl:apply-templates mode="titlepage.mode"/>
        </fo:block>
    </xsl:template>


    <xsl:template match="d:author" mode="titlepage.mode">
        <fo:block linefeed-treatment="preserve">
            <xsl:call-template name="anchor"/>
            <xsl:call-template name="person.name"/>
            <xsl:if test="d:affiliation/d:jobtitle">
                <xsl:text>, </xsl:text>
                <xsl:apply-templates select="(d:affiliation/d:jobtitle)[1]"/>
            </xsl:if>
            <xsl:if test="d:affiliation/d:orgname">
                <xsl:text>&#xA;</xsl:text>
                <xsl:apply-templates select="d:affiliation/d:orgname" mode="titlepage.mode"/>
            </xsl:if>
            <xsl:if test="d:email|d:affiliation/d:address/d:email">
                <xsl:text> </xsl:text>
                <xsl:apply-templates select="(d:email|d:affiliation/d:address/d:email)[1]"/>
            </xsl:if>
        </fo:block>
    </xsl:template>


<!-- Tell ulink to use the href.properties properties for styling the link -->
    <xsl:template match="d:ulink" name="ulink">
        <xsl:param name="url" select="@url"/>

        <xsl:variable name ="ulink.url">
            <xsl:call-template name="fo-external-image">
            <xsl:with-param name="filename" select="$url"/>
            </xsl:call-template>
        </xsl:variable>

        <fo:basic-link xsl:use-attribute-sets="href.properties"
                        external-destination="{$ulink.url}">
            <xsl:choose>
            <xsl:when test="count(child::node())=0">
                <xsl:call-template name="hyphenate-url">
                <xsl:with-param name="url" select="$url"/>
                </xsl:call-template>
            </xsl:when>
            <xsl:otherwise>
                <xsl:apply-templates/>
            </xsl:otherwise>
            </xsl:choose>
        </fo:basic-link>
        <!-- * Call the template for determining whether the URL for this -->
        <!-- * hyperlink is displayed, and how to display it (either inline or -->
        <!-- * as a numbered footnote). -->
        <xsl:call-template name="hyperlink.url.display">
            <xsl:with-param name="url" select="$url"/>
            <xsl:with-param name="ulink.url" select="$ulink.url"/>
        </xsl:call-template>
    </xsl:template>

    <xsl:template match="d:guibutton">
        <xsl:call-template name="inline.boldseq"/>
    </xsl:template>

    <xsl:template match="d:guiicon">
        <xsl:call-template name="inline.boldseq"/>
    </xsl:template>

    <xsl:template match="d:guilabel">
        <xsl:call-template name="inline.boldseq"/>
    </xsl:template>

    <xsl:template match="d:guimenu">
        <xsl:call-template name="inline.boldseq"/>
    </xsl:template>

    <xsl:template match="d:guimenuitem">
        <xsl:call-template name="inline.boldseq"/>
    </xsl:template>

    <xsl:template match="d:guisubmenu">
        <xsl:call-template name="inline.boldseq"/>
    </xsl:template>


<!-- style <link> -->
    <xsl:attribute-set name="xref.properties">
        <xsl:attribute name="color">blue</xsl:attribute>
    </xsl:attribute-set>

<!-- style <ulink> -->
    <xsl:param name="ulink.show" select="0" />
    <xsl:attribute-set name="href.properties">
        <xsl:attribute name="color">blue</xsl:attribute>
        <xsl:attribute name="text-decoration">underline</xsl:attribute>
    </xsl:attribute-set>

<!-- Style <term> -->
    <xsl:attribute-set name="variablelist.term.properties">
        <xsl:attribute name="font-weight">bold</xsl:attribute>
    </xsl:attribute-set>
    
</xsl:stylesheet>