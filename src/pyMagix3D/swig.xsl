<!-- xsltproc swig.xsl index.xml > swig_doc.i
index dans !EB/Tools/Magix3D/gen/produits/share/docs/pyMagix3D/pyMagix3D/xml
retappé par EB le 10/7/2014, trouvé sur le net...

modifs: 
 ISO-8859-1 ajouté
 suppression du "see also"
 gestion des paramètres des fonctions en une ligne

Il faut activer la sortie xml dans doxygen
Copier ce fichier dans le répertoire "xml" et faire  
xsltproc swig.xsl index.xml > swig_doc.i

A faire manuellement ensuite:
 regrouper les fonction avec la même signature (dnas un même manager):
   importMDL, newSphere, newVertex, addSmoothing, ::split*, setMeshingProperty

 -->

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
<xsl:output method="text" encoding="UTF-8"/>
<xsl:strip-space elements="*" />
<xsl:template match="/">
<!-- process each compound one by one -->
<xsl:for-each select="doxygenindex/compound">
	<xsl:apply-templates select="document( concat( @refid, '.xml' ) )/*" />
</xsl:for-each>
</xsl:template>

<xsl:template match="doxygen">
<!-- for each class, output brief and detailed description -->
<xsl:for-each
select="compounddef[@kind='class']|compounddef[@kind='namespace']">
<xsl:text>%feature("docstring") </xsl:text>
<xsl:value-of select="compoundname" />
<xsl:text> " &#10;</xsl:text>
<xsl:apply-templates select="briefdescription" />
<xsl:text>
</xsl:text>
<xsl:apply-templates select="detaileddescription" />
<xsl:text>";&#10;&#10;</xsl:text>

<!-- output for each function individually -->
<xsl:for-each
select="*/memberdef[@kind='function' and not(starts-with(name,'operator'))]">
<xsl:text>%feature("docstring") </xsl:text>
<xsl:value-of select="../../compoundname" />::<xsl:value-of select="name" />
<xsl:text> "&#10;</xsl:text>
			<xsl:value-of select="definition" />
			<xsl:apply-templates select="argsstring" />
			<xsl:text>

</xsl:text>
			<xsl:apply-templates select="briefdescription" />
			<xsl:text>
</xsl:text>
			<xsl:apply-templates select="detaileddescription" />
			<xsl:text>
";&#10;</xsl:text>
		</xsl:for-each>
	</xsl:for-each>
</xsl:template>

<xsl:template match="detaileddescription">
	<xsl:for-each select="para">
		<xsl:choose>
			<xsl:when test="parameterlist">
				<xsl:apply-templates select="parameterlist" />
			</xsl:when>
			<xsl:otherwise>
				<xsl:call-template name="CleanQuotes">
					<xsl:with-param name="body" select="."></xsl:with-param>
				</xsl:call-template>
				<xsl:text>
</xsl:text>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:for-each>
</xsl:template>


<xsl:template match="parameterlist">
	<xsl:text>
</xsl:text>
	<xsl:for-each select="parameteritem">
		<xsl:value-of select="parameternamelist/parametername" />
		<xsl:text> : </xsl:text>
		<xsl:apply-templates select="parameterdescription/para" />
		<xsl:text>
</xsl:text>
	</xsl:for-each>
</xsl:template>

<!-- Call the recursive text cleaner with the entire content of the descriptive elements -->
<xsl:template match="briefdescription|para|argsstring">
	<xsl:call-template name="CleanQuotes">
		<xsl:with-param name="body" select="."></xsl:with-param>
	</xsl:call-template>
</xsl:template>

<!-- recursive cleaner, matche string to first quote then recurses to process the
rest. Note how it uses a When to test if the termination condition of no quotes has been reached -->
<xsl:template name="CleanQuotes"><xsl:param name="body" />
<xsl:choose>
<xsl:when test="contains($body, '&quot;')">
<xsl:value-of select="substring-before($body, '&quot;')" />\&quot;<xsl:call-template name="CleanQuotes">
<xsl:with-param name="body" select="substring-after($body,'&quot;')"></xsl:with-param></xsl:call-template>
</xsl:when>
<xsl:otherwise>
<xsl:value-of select="$body" />
<!-- finished recursing -->
</xsl:otherwise>
</xsl:choose>
</xsl:template>

</xsl:stylesheet>
