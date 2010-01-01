<?php
$dbhost = 'localhost';
$dbname = 'rhessys_defs';
$dbuser = 'root';
$dbpass = '';
$appname = 'RHESSys Defaults Database';

mysql_connect($dbhost, $dbuser, $dbpass) or die(mysql_error());
mysql_select_db($dbname) or die(mysql_error());
?>
