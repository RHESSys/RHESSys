<?php
$path = $_SERVER['DOCUMENT_ROOT']; 
$include_path = $path . "/rhessys_defs/include";
require "$include_path/smarty.php"; 
require_once "$include_path/login.php";
require_once "$include_path/util.php";
require "$include_path/session.php";

$table_name = $_POST['type'];
$id_field = getIDField($table_name);
$names = getNames($table_name);

if (isset($_POST['searching'])) {
	// Assemble the query
	foreach ($names as $name) {
			
	}
	$query = "SELECT $id_field,filename FROM $table_name WHERE conditions";
	$result = mysql_query($query);

	$smarty->assign("type", $table_name);	
	$smarty->display("$path/rhessys_defs/smarty/templates/results.tpl");	
} else {
	$names = getNames($table_name);

	$smarty->assign("type", $table_name);
	$smarty->assign("names", $names);
	$smarty->display("$path/rhessys_defs/smarty/templates/search.tpl");
}
?>
