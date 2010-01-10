<?php
$path = $_SERVER['DOCUMENT_ROOT']; 
$include_path = $path . "/rhessys_defs/include";
require "$include_path/smarty.php"; 
require_once "$include_path/login.php";
require_once "$include_path/util.php";
require "$include_path/session.php";

$table_name = $_POST['type'];
$id_field = getIDField($table_name);
$names = getNames($table_name);

if (isset($_POST['searching'])) {
	// Assemble the query
	foreach ($names as $name) {
		if (in_array($name, $_POST['filter'])) {
			$test_name = $name . "_test";
			$test_value = $name . "_value";
			$value = $_POST[$test_value];
	
			if(!is_numeric($value)) {
				$value = "\"" . $value . "\"";
			}

			// Add "AND" to all but the first condition
			if($condition != '') {
				$condition .= " AND ";
			}

			switch($_POST[$test_name]) {
				case "eq":
					$condition .= "$name=$value";
					break;
				case "gt":
					$condition .= "$name>$value";
					break;
				case "lt":
					$condition .= "$name<$value";
					break;
				default:
					break;
			}
		}		
	}

	$query = "SELECT $id_field, filename, username FROM $table_name WHERE $condition";
	$result = mysql_query($query);
	$rows = mysql_num_rows($result);	
	for ($j=0; $j < $rows; ++$j) {
		$results[] = mysql_fetch_array($result);
	}

	$smarty->assign("type", $table_name);	
	$smarty->assign("results", $results);
	$smarty->assign("id_field", $id_field);
	$smarty->display("$path/rhessys_defs/smarty/templates/results.tpl");	

} else {
	$names = getNames($table_name);

	$smarty->assign("type", $table_name);
	$smarty->assign("names", $names);
	$smarty->display("$path/rhessys_defs/smarty/templates/search.tpl");
}
?>
