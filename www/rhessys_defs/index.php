<?php
$path = $_SERVER['DOCUMENT_ROOT'];
require "$path/Smarty/Smarty.class.php";

$smarty = new Smarty();
$smarty->template_dir = "$path/rhessys_defs/smarty/templates";
$smarty->compile_dir = "$path/rhessys_defs/smarty/templates_c";
$smarty->cache_dir = "$path/rhessys_defs/smarty/cache";
$smarty->config_dir = "$path/rhessys_defs/smarty/configs";

$smarty->display("$path/rhessys_defs/smarty/templates/index.tpl");
?>
