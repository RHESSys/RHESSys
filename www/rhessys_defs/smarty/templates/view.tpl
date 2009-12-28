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
<td>{$values[$names[row][0]]}</td>
<td>{$refs[$names[row][0]]}</td>
</tr>
{/section}
</table>
<form method="post" action="save_def.php">
<input type="hidden" name="type" value="{$type}">
<input type="hidden" name="id" value="{$id}">
<input type="submit" value="Download this definition">
</form>
{include file='footer.tpl'}
