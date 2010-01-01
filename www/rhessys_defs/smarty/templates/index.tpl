{include file='header.tpl'}
<div style="background-color:lightgrey">
Land Use:
<form action="view.php" method="post">
<input type="hidden" name="type" value="Land_Use" />
<select name="list" multiple="multiple">
{section name=row loop=$land_uses}
<option value="{$land_uses[row].landuse_default_ID}">{$land_uses[row].name}</option>
{/section}
</select>
<input type="submit" value="View">
</form>

<form action="update_def.php" method="post">
<input type="hidden" name="type" value="Land_Use">
<input type="submit" value="Create New Land Use" />
</form>

<form action="upload_def.php" method="post" enctype="multipart/form-data">
<input type="hidden" name="type" value="Land_Use" />
<input name="filename" type="file" /><br />
<input type="submit" value="Upload File" />
</form>
</div>

<div style="background-color:lightgrey">
Soil:
<form action="view.php" method="post">
<input type="hidden" name="type" value="Soil" />
<select name="list" multiple="multiple">
{section name=row loop=$soils}
<option value="{$soils[row].patch_default_ID}">{$soils[row].name}</option>
{/section}
</select>
<input type="submit" value="View">
</form>

<form action="update_def.php" method="post">
<input type="hidden" name="type" value="Soil">
<input type="submit" value="Create New Soil" />
</form>


<form action="upload_def.php" method="post" enctype="multipart/form-data">
<input type="hidden" name="type" value="Soil" />
<input name="filename" type="file" /><br />
<input type="submit" value="Upload File" />
</form>
</div>

<div style="background-color:lightgrey">
Strata:
<form action="view.php" method="post">
<input type="hidden" name="type" value="Stratum">
<select name="list" multiple="multiple">
{section name=row loop=$strata}
<option value="{$strata[row].stratum_default_ID}">{$strata[row].name}</option>
{/section}
</select>
<input type="submit" value="View">
</form>

<form action="update_def.php" method="post">
<input type="hidden" name="type" value="Stratum">
<input type="submit" value="Create New Stratum" />
</form>

<form action="upload_def.php" method="post" enctype="multipart/form-data">
<input type="hidden" name="type" value="Stratum" />
<input name="filename" type="file" /><br />
<input type="submit" value="Upload File" />
</form>
</div>

<div style="background-color:lightgrey">
Zones:
<form action="view.php" method="post">
<input type="hidden" name="type" value="Zone">
<select name="list" multiple="multiple">
{section name=row loop=$zones}
<option value="{$zones[row].zone_default_ID}">{$zones[row].name}</option>
{/section}
</select>
<input type="submit" value="View">
</form>

<form action="update_def.php" method="post">
<input type="hidden" name="type" value="Zone">
<input type="submit" value="Create New Zone" />
</form>

<form action="upload_def.php" method="post" enctype="multipart/form-data">
<input type="hidden" name="type" value="Zone" />
<input name="filename" type="file" /><br />
<input type="submit" value="Upload File" />
</form>
</div>

{include file='footer.tpl'}
