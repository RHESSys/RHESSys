<?php
$path = $_SERVER['DOCUMENT_ROOT'];
require "$path/rhessys_defs/include/smarty.php";
require_once "$path/rhessys_defs/include/login.php";
require_once "$path/rhessys_defs/include/util.php";


$table_name = $_POST['type'];

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

// Check if this view was called from update.php, if it was,
// create a SQL query to update the database before viewing
if (isset($_POST['save'])) {
	$id = $_POST['id'];

	echo "Updating...<br />\n";	

	foreach ($names as $name) {
		$var_value = $name . "_value";
		$var_ref = $name . "_ref";

		$query = "UPDATE $table_name SET $name=$_POST[$var_value] WHERE $id_field=$id";
		mysql_query($query);
		
		// Update the reference next
		$ref_table_name = $table_name . "_Reference";
		$query = "UPDATE $ref_table_name SET $name=\"$_POST[$var_ref]\" WHERE $id_field=$id";
		echo $query . "<br />\n";
		mysql_query($query);

		// If ID was updated, then we need to reassign ID for use in future
		// update statements
		if (strstr($name, "_default_ID")) {
			$id = $_POST[$var_value];
		}
	}	
	
	$id = $_POST['id'];	
} else if (isset($_POST['cancel'])) {
	// Called from update.php, but do not update the db. Get
	// the current id from the $id var
	$id = $_POST['id'];	
} else {
	$id = $_POST['list'];
}

$reference_name = $table_name . "_Reference";

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
