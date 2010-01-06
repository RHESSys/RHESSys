<?php
$path = $_SERVER['DOCUMENT_ROOT'];
$include_path = $path . "/rhessys_defs/include";
require "$include_path/smarty.php";
require_once "$include_path/login.php";
require_once "$include_path/util.php";
require "$include_path/session.php";

$table_name = $_POST['type'];
$ref_table_name = $table_name . "_Reference";
$id_field = getIDField($table_name);
$names = getNames($table_name);

// Check if this view was called from update.php, if it was,
// create a SQL query to update the database before viewing
if (isset($_POST['save'])) {
	$id = $_POST['id'];

	foreach ($names as $name) {
		$var_value = $name . "_value";
		$var_ref = $name . "_ref";

		$query = "UPDATE $table_name SET $name=\"$_POST[$var_value]\" WHERE $id_field=$id";
		mysql_query($query);
		
		// Update the reference next
		$ref_table_name = $table_name . "_Reference";
		$query = "UPDATE $ref_table_name SET $name=\"$_POST[$var_ref]\" WHERE $id_field=$id";
		mysql_query($query);

		// If ID was updated, then we need to reassign ID for use in future
		// update statements
		if (strstr($name, "_default_ID")) {
			$id = $_POST[$var_value];
		}
	}	
} else if (isset($_POST['cancel'])) {
	// Called from update.php, but do not update the db. Get
	// the current id from the $id var
	echo "Cancelled Update<br />\n";
	$id = $_POST['id'];	
} else {
	$id = $_POST['list'];
}


$values_query = "SELECT * FROM $table_name WHERE $id_field=$id";
$ref_query = "SELECT * FROM $ref_table_name WHERE $id_field=$id";

$values_result = mysql_query($values_query);
$values = mysql_fetch_array($values_result);

$refs_result = mysql_query($ref_query);
$refs = mysql_fetch_array($refs_result);

mysql_close($db_server);

// Compare the owner of this record to the logged in user to
// see if we should enable the update button
if ($username == $values['username']) {
	$mutable = TRUE;
} else {
	$mutable = FALSE;
}

$smarty->assign("type", $table_name);
$smarty->assign("names", $names);
$smarty->assign("values", $values);
$smarty->assign("refs", $refs);
$smarty->assign("id", $id);
$smarty->assign("mutable", $mutable);
$smarty->display("$path/rhessys_defs/smarty/templates/view.tpl");
?>
