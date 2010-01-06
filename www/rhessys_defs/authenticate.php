<?php
$path = $_SERVER['DOCUMENT_ROOT'];
$include_path = $path . "/rhessys_defs/include";
require "$include_path/smarty.php";
require_once "$include_path/login.php";
require_once "$include_path/util.php";
require "$include_path/session.php";

if (isset($_POST['username']) && isset($_POST['password'])) {
	$temp_username = $_POST['username'];
	$query = "SELECT password FROM users WHERE username=\"$temp_username\"";
	$result = mysql_query($query);
	$row = mysql_fetch_row($result);	
	if ($row[0] == $_POST['password']) {
		$_SESSION['username'] = $_POST['username'];
		header("Location: index.php");
		exit;	
	} else {
		echo "Incorrect Password";
		$smarty->display("$path/rhessys_defs/smarty/templates/authenticate.tpl");
	}
} else {
	$smarty->display("$path/rhessys_defs/smarty/templates/authenticate.tpl");
}
?>
