<?php
$path = $_SERVER['DOCUMENT_ROOT'];
$include_path = $path . "/rhessys_defs/include";
require "$include_path/smarty.php";
require_once "$include_path/login.php";
require_once "$include_path/util.php";

if (isset($_POST['username']) && isset($_POST['password'])) {
	$temp_username = $_POST['username'];
	$temp_password = $_POST['password'];
	$query = "INSERT INTO Users VALUES(\"$temp_username\", \"$temp_password\")";
	mysql_query($query);
	$smarty->display("$path/rhessys_defs/smarty/templates/authenticate.tpl");
} else {
	$smarty->display("$path/rhessys_defs/smarty/templates/create_account.tpl");
}
?>
