<?php
$path = $_SERVER['DOCUMENT_ROOT']; 
require "$path/rhessy_defs/include/smarty.php"; 
require_once "$path/rhessys_defs/include/login.php";
require_once "$path/rhessys_defs/include/util.php";

mysql_close($db_server);

$smarty->display("$path/rhessys_defs/smarty/templates/search.tpl");
?>
