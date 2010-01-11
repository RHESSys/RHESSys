{include file='header.tpl'}
<p>
</table>
<h2>{$selected_watershed} Watershed</h2>
</p>

<div id="left_side">
<h3>Land Uses</h3>
<table>
<tr>
<th>ID:</th>
<th>Filename:</th>
<th>Owner:</th>
</tr>
{section name=row loop=$land_uses}
<tr>
<td><a href="view.php?id={$land_uses[row].landuse_default_ID}&type=Land_Use">
{$land_uses[row].landuse_default_ID}</a></td>
<td>{$land_uses[row].filename}</td>
<td>owner</td>
</tr>
{/section}
</table>

<h3>Zones</h3>
<table>
<tr>
<th>ID:</th>
<th>Filename:</th>
<th>Owner:</th>
</tr>
{section name=row loop=$zones}
<tr>
<td><a href="view.php?id={$zones[row].zone_default_ID}&type=Zone">
{$zones[row].zone_default_ID}</a></td>
<td>{$zones[row].filename}</td>
<td>owner</td>
</tr>
</tr>
{/section}
</table>

<p>
<form action="" method="POST">
<input type="submit" value="Download Watershed Defs" />
</form>
</p>

</div> <!--end of left side-->

<div id="right_side">
<h3>Strata</h3>
<table>
<tr>
<th>ID:</th>
<th>Filename:</th>
<th>Owner:</th>
</tr>
{section name=row loop=$strata}
<tr>
<td><a href="view.php?id={$strata[row].stratum_default_IDi}&type=Stratum">
{$strata[row].stratum_default_ID}</a></td>
<td>{$strata[row].filename}</td>
<td>owner</td>
</tr>
{/section}
</table>

<h3>Soils</h3>
<table>
<tr>
<th>ID:</th>
<th>Filename:</th>
<th>Owner:</th>
</tr>
{section name=row loop=$soils}
<tr>
<td><a href="view.php?id={$soils[row].patch_default_IDi}&type=Soil">
{$soils[row].patch_default_ID}</a></td>
<td>{$soils[row].filename}</td>
<td>Owner</td>
</tr>
</tr>
{/section}
</table>

{include file='footer.tpl'}
