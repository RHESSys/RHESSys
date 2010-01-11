<?php
if (isset($_SESSION['username'])) {
	$username = $_SESSION['username'];
	$loggedin = TRUE;
	$smarty->assign("username", $username);
	$smarty->assign("loggedin", $loggedin);
} else {
	$loggedin = FALSE;
	$smarty->assign("loggedin", $loggedin);
}

if (isset($_SESSION['watershed'])) {
	$active_watershed = $_SESSION['watershed'];
	$smarty->assign("active_watershed", $active_watershed);
}
?>
