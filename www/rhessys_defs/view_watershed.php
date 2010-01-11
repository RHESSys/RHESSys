<?php
$path = $_SERVER['DOCUMENT_ROOT'];
$include_path = $path . "/rhessys_defs/include";
require "$include_path/smarty.php";
require_once "$include_path/login.php";
require_once "$include_path/util.php";
require "$include_path/session.php";

if ($_POST['watershed_submit'] == "Activate Watershed") {
	$_SESSION['watershed'] = $_POST['list'];
	header("Location: index.php");
} else if ($_POST['watershed_submit'] = "View Watershed") {

	$selected_watershed = $_POST['list'];
	$table_namess = array("Land_Use", "Soil", "Stratum", "Zone");

	foreach ($table_namess as $table_name) {
		$id_field = getIDField($table_name);
		$query = "SELECT $id_field,filename FROM $table_name NATURAL JOIN Watershed_$table_name WHERE watershed_name=\"$selected_watershed\"";
		$result = mysql_query($query);

		$rows = mysql_num_rows($result);
		for ($j=0; $j < $rows; ++$j) {
			$things[] = mysql_fetch_array($result);	
		}

		switch ($table_name) {
			case "Zone":
				$smarty->assign("zones", $things);
				break;
			case "Soil":
				$smarty->assign("soils", $things);
				break;
			case "Land_Use":
				$smarty->assign("land_uses", $things);
				break;
			case "Stratum":
				$smarty->assign("strata", $things);
				break;
		}
		unset($things);
	}

	$smarty->assign("selected_watershed", $selected_watershed);
	$smarty->display("$path/rhessys_defs/smarty/templates/view_watershed.tpl");
}
?>
