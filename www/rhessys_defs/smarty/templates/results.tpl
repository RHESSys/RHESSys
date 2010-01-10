{include file='header.tpl'}

<h2>Search Results</h2>
<table border="1">
<tr>
<td>ID</td>
<td>Filename</td>
<td>Owner</td>
</tr>

{section name=row loop=$results}
<tr>
<td><a href="view.php?id={$results[row].$id_field}&type={$type}">{$results[row].$id_field}</a></td>
<td>{$results[row].filename}</td>
<td>{$results[row].username}</td>
</tr>
{/section}
<table>

<form action="search.php" method="post">
<input type="hidden" name="type" value="{$type}" />
<input type="submit" value="Search Again" />
</form>
{include file='footer.tpl'}
