{include file='header.tpl'}
<h2>{$type}</h2>
<table border="2">
<tr>
<td>Name:</td>
<td>Value:</td>
<td>Reference:</td>
</tr>
{section name=row loop=$names}
<tr>
<td>{$names[row][0]}</td>
<td><input type="text" name="{$names[row][0]}_value" /></td>
<td><input type="text" name="{$names[row][0]}_ref" /></td>
</tr>
{/section}
</table>
{include file='footer.tpl'}
