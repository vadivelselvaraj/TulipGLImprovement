<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version='1.0'>
<xsl:output method="text"/>
<xsl:param name="path"/>
<xsl:template match="book">
    &lt;dcf ref="<xsl:value-of select="concat($path,'index.html')"/>" title="<xsl:value-of select="title"/>">
    	<xsl:apply-templates select="chapter">
            <xsl:with-param name="path"><xsl:value-of select="$path"/></xsl:with-param> 
        </xsl:apply-templates>
    &lt;/dcf>
</xsl:template>

<xsl:template match="chapter">
    <xsl:param name="path"/>
    <xsl:variable name="ch-href">
        <xsl:choose>
            <xsl:when test="10 > position()">
                <xsl:value-of select="concat('ch0',position())"/>
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="concat('ch',position())"/>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:variable>
    &lt;section ref="<xsl:value-of select="concat($path,$ch-href)"/>.html" title="<xsl:value-of select="title"/>">
        &lt;keyword ref="<xsl:value-of select="concat($path,$ch-href)"/>.html"><xsl:value-of select="title"/>&lt;/keyword>
        <xsl:apply-templates select="sect1[1]">
            <!--  le début du nom de fichier est passé en paramètres -->
            <xsl:with-param name="ch-href"><xsl:value-of select="$ch-href"/></xsl:with-param> 
            <xsl:with-param name="path"><xsl:value-of select="$path"/></xsl:with-param> 
        </xsl:apply-templates>
        <xsl:apply-templates select="sect1[position()>1]">
            <xsl:with-param name="ch-href"><xsl:value-of select="$ch-href"/></xsl:with-param> 
            <xsl:with-param name="path"><xsl:value-of select="$path"/></xsl:with-param> 
        </xsl:apply-templates>
    &lt;/section>
</xsl:template>

<!--  SECTION   -->
<xsl:template match="sect1[1]">
    <xsl:param name="path"/>
    <xsl:param name="ch-href"/><!-- on va directement à la section -->
    &lt;section ref="<xsl:value-of select="concat($path,$ch-href)"/>.html#<xsl:value-of select="@id"/>" title="<xsl:value-of select="title"/>">
        &lt;keyword ref="<xsl:value-of select="concat($path,$ch-href)"/>.html#<xsl:value-of select="@id"/>"><xsl:value-of select="title"/>&lt;/keyword>
    &lt;/section>
    <xsl:apply-templates select="sect2|sect3|sect4|sect5">
        <xsl:with-param name="sec-href"><xsl:value-of select="$ch-href"/></xsl:with-param> 
        <xsl:with-param name="path"><xsl:value-of select="$path"/></xsl:with-param>
    </xsl:apply-templates>
</xsl:template>

<!--  SECTION (BIS) -->
<xsl:template match="sect1[position()>1]">
    <xsl:param name="path"/>
    <xsl:param name="ch-href"/>
    <xsl:variable name="href">
        <xsl:choose>
            <xsl:when test="10 > position()">
                <xsl:value-of select="concat($ch-href,'s0',position()+1)"/>
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="concat($ch-href,'s',position()+1)"/>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:variable>
    &lt;section ref="<xsl:value-of select="concat($path,$href)"/>.html" title="<xsl:value-of select="title"/>">
        &lt;keyword ref="<xsl:value-of select="concat($path,$href)"/>.html"><xsl:value-of select="title"/>&lt;/keyword>
    &lt;/section>
    <xsl:apply-templates select="sect2|sect3|sect4|sect5">
        <xsl:with-param name="sec-href">
            <xsl:value-of select="$href"/>
        </xsl:with-param> 
        <xsl:with-param name="path"><xsl:value-of select="$path"/></xsl:with-param>
    </xsl:apply-templates>
</xsl:template>

<xsl:template match="sect2|sect3|sect4|sect5">
    <xsl:param name="sec-href"/>
    <xsl:param name="path"/>
    &lt;keyword ref="<xsl:value-of select="concat($path,$sec-href)"/>.html#<xsl:value-of select="@id"/>"><xsl:value-of select="title"/>&lt;/keyword>
</xsl:template>

<!--<xsl:template match="application">
    <keyword ref="...html#..."><xsl:value-of select="./"/></keyword>
</xsl:template>

    idem pour 
-->

</xsl:stylesheet>

