<?php
$path = $_SERVER['DOCUMENT_ROOT'];
$include_path = $path . "/rhessys_defs/include";
require "$include_path/smarty.php";
require_once "$include_path/login.php";
require_once "$include_path/util.php";
require "$include_path/session.php";

if ($_POST['name']) {
	$temp_username = $_SESSION['username'];
	$temp_name = $_POST['name'];
	$temp_notes = $_POST['notes'];
	$query = "INSERT INTO Watershed VALUES(\"$temp_name\", \"$temp_username\", \"$temp_notes\")";
	mysql_query($query);
	header("Location: index.php");
} else {
	$smarty->display("$path/rhessys_defs/smarty/templates/create_watershed.tpl");
}
?>
