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
$outfile_name = $values['filename'] . '.def';

$first = TRUE;
$IGNORE_NAMES = array('rhessys_version', 'filename', 'username');
foreach ($names as $name) {
	if (!in_array($name, $IGNORE_NAMES)) {
		// To avoid printing out a newline at end of file
		if (!$first) {
			echo "\n";
			$first = FALSE;
		}
		if ($values[$name] != '') {
			$line = $values[$name] . " " . $name;
		}
	}
}

header("Content-disposition: attachment; filename=$outfile_name");
header("Content-type: text/unknown");
?>
