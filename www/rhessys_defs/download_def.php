<?php
$path = $_SERVER['DOCUMENT_ROOT'];
require_once "$path/rhessys_defs/include/login.php";
require_once "$path/rhessys_defs/include/util.php";

$table_name = $_POST['type'];
$id = $_POST['id'];
$id_field = getIDField($table_name);
$names = getNames($table_name);

$values_query = "SELECT * FROM $table_name WHERE $id_field=$id";
$values_result = mysql_query($values_query);
$values = mysql_fetch_array($values_result);

mysql_close($db_server);
$outfile_name = $values['name'] . '.def';

$IGNORE_NAMES = array('rhessys_version', 'name', 'username');
foreach ($names as $name) {
	if (!in_array($name, $IGNORE_NAMES)) {
		$line = $values[$name] . " " . $name;
		echo $line . "\n";
	}
}

header("Content-disposition: attachment; filename=$outfile_name");
header("Content-type: text/unknown");
?>
