{include file='header.tpl'}

<table>

<tr>
<td>
Land Use:
<form action="view.php" method="post">
<input type="hidden" name="type" value="Land_Use" />
<p><select name="list" multiple="multiple">
{section name=row loop=$land_uses}
<option value="{$land_uses[row].landuse_default_ID}">{$land_uses[row].filename}</option>
{/section}
</select></p>
<p><input type="submit" value="View"></p>
</form>

<form action="search.php" method="post">
<input type="hidden" name="type" value="Land_Use">
<p><input type="submit" value="Search" /></p>
</form>

{if $loggedin}
<form action="update_def.php" method="post">
<input type="hidden" name="type" value="Land_Use">
<p><input type="submit" value="Create New Land Use" /></p>
</form>

<form action="upload_def.php" method="post" enctype="multipart/form-data">
<input type="hidden" name="type" value="Land_Use" />
<p><input name="filename" type="file" /></p>
<p><input type="submit" value="Upload File" /></p>
</form>
{/if}
</td>
<td>
Soil:
<form action="view.php" method="post">
<input type="hidden" name="type" value="Soil" />
<p><select name="list" multiple="multiple">
{section name=row loop=$soils}
<option value="{$soils[row].patch_default_ID}">{$soils[row].filename}</option>
{/section}
</select></p>
<p><input type="submit" value="View"></p>
</form>

<form action="search.php" method="post">
<input type="hidden" name="type" value="Soil">
<p><input type="submit" value="Search" /></p>
</form>

{if $loggedin}
<form action="update_def.php" method="post">
<input type="hidden" name="type" value="Soil">
<p><input type="submit" value="Create New Soil" /></p>
</form>

<form action="upload_def.php" method="post" enctype="multipart/form-data">
<input type="hidden" name="type" value="Soil" />
<p><input name="filename" type="file" /></p>
<P><input type="submit" value="Upload File" /></p>
</form>
{/if}

</tr>
<tr>
<td>
Strata:
<form action="view.php" method="post">
<p><input type="hidden" name="type" value="Stratum">
<select name="list" multiple="multiple">
{section name=row loop=$strata}
<option value="{$strata[row].stratum_default_ID}">{$strata[row].filename}</option>
{/section}
</select></p>
<input type="submit" value="View">
</form>

<form action="search.php" method="post">
<input type="hidden" name="type" value="Stratum">
<p><input type="submit" value="Search" /></p>
</form>

{if $loggedin}
<form action="update_def.php" method="post">
<input type="hidden" name="type" value="Stratum">
<p><input type="submit" value="Create New Stratum" /></p>
</form>

<form action="upload_def.php" method="post" enctype="multipart/form-data">
<input type="hidden" name="type" value="Stratum" />
<p><input name="filename" type="file" /></p>
<p><input type="submit" value="Upload File" /></p>
</form>
{/if}
</td>
<td>
Zones:
<form action="view.php" method="post">
<input type="hidden" name="type" value="Zone">
<p><select name="list" multiple="multiple">
{section name=row loop=$zones}
<option value="{$zones[row].zone_default_ID}">{$zones[row].filename}</option>
{/section}
</select></p>
<p><input type="submit" value="View"></p>
</form>

<form action="search.php" method="post">
<input type="hidden" name="type" value="Zone">
<p><input type="submit" value="Search" /></p>
</form>

{if $loggedin}
<form action="update_def.php" method="post">
<input type="hidden" name="type" value="Zone">
<p><input type="submit" value="Create New Zone" /></p>
</form>

<form action="upload_def.php" method="post" enctype="multipart/form-data">
<input type="hidden" name="type" value="Zone" />
<p><input name="filename" type="file" /></p>
<p><input type="submit" value="Upload File" /></p>
</form>
{/if}
</td>
</tr>
<tr>
<td colspan='2'>
Watersheds:
<form action="view_watershed.php" method="post">
<p>
<select name="list" multiple="multiple">
{section name=row loop=$watersheds}
<option value="{$watersheds[row].watershed_name}">{$watersheds[row].watershed_name}</option>
{/section}
</select>
</p>
<p><input type="submit" name="watershed_submit" value="View Watershed" /></p>
<p><input type="submit" name="watershed_submit" value="Activate Watershed" /></p>
</form>

{if $loggedin}
<form action="create_watershed.php" method="post">
<input type="submit" value="Create New Watershed" />
</form>
{/if}
</td>
</tr>
</table>
{include file='footer.tpl'}
