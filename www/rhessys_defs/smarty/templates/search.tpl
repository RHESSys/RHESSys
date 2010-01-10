{include file='header.tpl'}

<h2>Searching {$type}</h2>

<form action="search.php" method="post">
<input type="hidden" name="searching" value="1" />
<input type="hidden" name="type" value="{$type}" />
{section name=row loop=$names}
<div>
<input type="checkbox" name="filter[]" value="{$names[row]}" />
{$names[row]}
<select name="{$names[row]}_test">
<option value="eq">is equal to</option>
<option value="gt">is greater than</option>
<option value="lt">is less than</option>
</select>
<input type="text" name="{$names[row]}_value" />
</div>
{/section}
<input type="submit" value="Search" />
</form>

{include file='footer.tpl'}
