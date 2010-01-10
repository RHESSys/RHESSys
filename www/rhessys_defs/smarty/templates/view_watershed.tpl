{include file='header.tpl'}

<table>
{section name=row loop=$zones}
<tr>
<td>{$zones[row].</td>
</tr>
{/section}
</table>

<table>
{section name=row loop=$strata}
<tr>

</tr>
{/section}
</table>

<table>
{section name=row loop=$soils}
<tr>

</tr>
{/section}
</table>

<table>
{section name=row loops=$land_uses}
<tr>

</tr>
{/section}
</table>

{include file='footer.tpl'}
