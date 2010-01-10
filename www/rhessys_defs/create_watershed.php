<?php
$path = $_SERVER['DOCUMENT_ROOT'];
$include_path = $path . "/rhessys_defs/include";
require "$include_path/smarty.php";
require_once "$include_path/login.php";
require_once "$include_path/util.php";
require "$include_path/session.php";

$smarty->display("$path/rhessys_defs/smarty/templates/create_watershed.tpl");
?>
