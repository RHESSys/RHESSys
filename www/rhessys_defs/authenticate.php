<?php
$path = $_SERVER['DOCUMENT_ROOT'];
$include_path = $path . "/rhessys_defs/include";
require "$include_path/smarty.php";
require_once "$include_path/login.php";
require_once "$include_path/util.php";

if (isset($_POST['username']) && isset($_POST['password'])) {
//	$query = "SELECT password FROM users WHERE username=$_POST['username']";
//	$result = mysql_query($query);
//	if ($result == $_POST['password']) {
//		session_start();
//		$_SESSION['username'] = $username;
//	} else {
//		$smarty->display("$path/rhessys_defs/smarty/templates/authenticate.tpl");
//	}
	echo "username and password found";
} else {
	$smarty->display("$path/rhessys_defs/smarty/templates/authenticate.tpl");
}
?>
