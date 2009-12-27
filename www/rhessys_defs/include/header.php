<?php
$dbhost = 'localhost';
$dbname = 'rhessys_defs';
$dbuser = 'root';
$dbpass = '';
$appname = 'RHESSys Defaults Database';

mysql_connect($dbhost, $dbuser, $dbpass) or die(mysql_error());
mysql_select_db($dbname) or die(mysql_error());

function sanitizeString($var) {
	$var = strip_tags($var);
	$var = htmlentities($var);
	$var = stripslashes($var);
	return mysql_real_escape_string($var);
}

function columnNames($table) {
	$query = "SELECT column_name FROM information_schema.columns WHERE table_name=$table";
	$result = mysql_query($query);
	return $result;	
}
?>
