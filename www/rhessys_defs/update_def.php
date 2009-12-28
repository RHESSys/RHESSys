<?php
$path = $_SERVER['DOCUMENT_ROOT']; 
require "$path/Smarty/Smarty.class.php"; 

$smarty = new Smarty();
$smarty->template_dir = "$path/rhessys_defs/smarty/templates";
$smarty->compile_dir = "$path/rhessys_defs/smarty/templates_c";
$smarty->cache_dir = "$path/rhessys_defs/smarty/cache";
$smarty->config_dir = "$path/rhessys_defs/smarty/configs";
echo "Up high!";

mysql_connect('localhost', 'root', '') or die(mysql_error());
$db_server = mysql_select_db('rhessys_defs') or die(mysql_error());

$names_query = "SELECT column_name FROM information_schema.columns WHERE table_name=\"$table_name\"";
$names_result = mysql_query($names_query);
$rows = mysql_num_rows($names_result);
for ($j=0; $j < $rows; ++$j) {
	$names[] = mysql_fetch_array($names_result);
}
echo "HERE!";
mysql_close($db_server);

$smarty->("names", $names);
$smarty->display("$path/rhessys_defs/smarty/templates/update_def.tpl");
?>
