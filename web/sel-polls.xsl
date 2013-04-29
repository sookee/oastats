<?xml version="1.0" encoding="ISO-8859-1"?>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	
<xsl:template match="/">
	<xsl:for-each select='polls/poll'>
		<xsl:sort select='.' order='ascending'/>
		<xsl:choose>
			<xsl:when test="@sel = '1'">
				<option selected='1'><xsl:value-of select='.'/></option>
			</xsl:when> 
			<xsl:otherwise>
				<option><xsl:value-of select='.'/></option>
			</xsl:otherwise>		
		</xsl:choose>
	</xsl:for-each>
</xsl:template>

</xsl:stylesheet> 