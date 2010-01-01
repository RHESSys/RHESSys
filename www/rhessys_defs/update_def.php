<?php
$path = $_SERVER['DOCUMENT_ROOT']; 
require "$path/rhessys_defs/include/smarty.php"; 
require_once "$path/rhessys_defs/include/login.php";
require_once "$path/rhessys_defs/include/util.php";

$table_name = $_POST['type'];
$id = $_POST['id'];
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

$names = getNames($table_name);

// If id is set, then this is updating an existing one.  If id is
// not set, then this page was reached by selecting new def from
// the main page.
if (isset($id)) {
	// Populate default values 
	for ($j=0; $j < $rows; ++$j) {
		$query = "SELECT $names[$j] FROM $table_name WHERE $id_field=$id";
		$result = mysql_query($query);
		$record = mysql_fetch_row($result);
		$values[] = $record[0];

		$query = "SELECT $names[$j] FROM $reference_name WHERE $id_field=$id";
		$result = mysql_query($query);
		$record = mysql_fetch_row($result);
		$refs[] = $record[0];
	}
}

print_r($values);
print_r($refs);

mysql_close($db_server);

$smarty->assign("names", $names);
$smarty->assign("values", $values);
$smarty->assign("refs", $refs);
$smarty->assign("id", $id);
$smarty->assign("type", $table_name); 
$smarty->display("$path/rhessys_defs/smarty/templates/update_def.tpl");
?>
