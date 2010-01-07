<?php
$path = $_SERVER['DOCUMENT_ROOT']; 
$include_path = $path . "/rhessys_defs/include";
require "$include_path/smarty.php"; 
require_once "$include_path/login.php";
require_once "$include_path/util.php";
require "$include_path/session.php";

$table_name = $_POST['type'];
$id = $_POST['id'];
$reference_name = $table_name . "_Reference";
$id_field = getIDField($table_name);
$names = getNames($table_name);

// If id is set, then this is updating an existing one.  If id is
// not set, then this page was reached by selecting new def from
// the main page.
if (isset($id)) {
	// Populate default values 
	$rows = count($names);
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

$smarty->assign("names", $names);
$smarty->assign("values", $values);
$smarty->assign("refs", $refs);
$smarty->assign("id", $id);
$smarty->assign("type", $table_name); 
$smarty->display("$path/rhessys_defs/smarty/templates/update_def.tpl");
?>
