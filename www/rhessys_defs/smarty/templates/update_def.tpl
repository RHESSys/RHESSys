{include file='header.tpl'}
<h2>{$type}</h2>

<form method="post" action="view.php">
<input type="hidden" name="type" value="{$type}" />
<input type="hidden" name="id" value="{$id}" />
<table border="2">
<tr>
<th>Name:</th>
<th>Value:</th>
<th>Reference:</th>
</tr>
{section name=row loop=$names}
<tr>
<th>{$names[row]}</td>
<td><input type="text" name="{$names[row]}_value" value="{$values[row]}" /></td>
<td><input type="text" name="{$names[row]}_ref" value="{$refs[row]}" /></td>
</tr>
{/section}
</table>
<input type="submit" name="save" value="Save Changes" />
<input type="submit" name="cancel" value="Cancel" />
</form>
{include file='footer.tpl'}
