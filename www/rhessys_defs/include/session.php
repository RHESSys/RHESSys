<?php
if (isset($_SESSION['username'])) {
	$username = $_SESSION['username'];
	$loggedin = TRUE;
} else {
	$loggedin = FALSE;
}
?>
