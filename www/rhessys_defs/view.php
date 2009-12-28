<?php
$path = $_SERVER['DOCUMENT_ROOT'];
require "$path/Smarty/Smarty.class.php";

$smarty = new Smarty();
$smarty->template_dir = "$path/rhessys_defs/smarty/templates";
$smarty->compile_dir = "$path/rhessys_defs/smarty/templates_c";
$smarty->cache_dir = "$path/rhessys_defs/smarty/cache";
$smarty->config_dir = "$path/rhessys_defs/smarty/configs";

mysql_connect('localhost', 'root', '') or die(mysql_error());
$db_server = mysql_select_db('rhessys_defs') or die(mysql_error());

$table_name = $_POST['type'];
$id = $_POST['list'];
$reference_name = $table_name . "_Reference";
switch ($table_name) {
	case "Land_Use":
		$id_field = "landuse_default_ID";
		break;
	case "Stratum":
		$id_field = "stratum_default_ID";
		break;
	case "Soil":
		$id_field = "patch_default_ID";
		break;
	case "Zone":
		$id_field = "zone_default_ID";
		break;
	default:
		die("Invalid database table");
}

$names_query = "SELECT column_name FROM information_schema.columns WHERE table_name=\"$table_name\"";
$names_result = mysql_query($names_query);
$rows = mysql_num_rows($names_result);
for ($j=0; $j < $rows; ++$j) {
	$names[] = mysql_fetch_array($names_result);
}

$values_query = "SELECT * FROM $table_name WHERE $id_field=$id";
$ref_query = "SELECT * FROM $reference_name WHERE $id_field=$id";

$values_result = mysql_query($values_query);
$values = mysql_fetch_array($values_result);

$refs_result = mysql_query($ref_query);
$refs = mysql_fetch_array($refs_result);

mysql_close($db_server);

$smarty->assign("type", $table_name);
$smarty->assign("names", $names);
$smarty->assign("values", $values);
$smarty->assign("refs", $refs);
$smarty->assign("id", $id);
$smarty->display("$path/rhessys_defs/smarty/templates/view.tpl");
?>
