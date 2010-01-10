{include file='header.tpl'}
<h2>{$type}</h2>
<table>
<tr>
<th>Name:</th>
<th>Value:</th>
<th>Reference:</th>
</tr>
{section name=row loop=$names}
<tr>
<th>{$names[row]}</th>
<td>{$values[$names[row]]}</td>
<td>{$refs[$names[row]]}</td>
</tr>
{/section}
</table>

{if $mutable}
<form method="post" action="update_def.php">
<input type="hidden" name="type" value="{$type}" />
<input type="hidden" name="id" value="{$id}" />
<input type="submit" value="Update" />
</form>
{/if}

<form method="post" action="download_def.php">
<input type="hidden" name="type" value="{$type}" />
<input type="hidden" name="id" value="{$id}" />
<input type="submit" value="Download this definition" />
</form>

{include file='footer.tpl'}
