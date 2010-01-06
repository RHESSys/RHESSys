<?php
// Gets the list of names from the table called $var
function getNames($var) {
	$query = "SELECT column_name FROM information_schema.columns WHERE table_name=\"$var\""; 	
	$result = mysql_query($query);
	$rows = mysql_num_rows($result);
	for ($j=0; $j < $rows; ++$j) {
		$record = mysql_fetch_row($result);
		$names[] = $record[0];
	}

	return $names;
}

function getIDField($table_name) {
	switch ($table_name) {
		case "Land_Use":
			$id_field = "landuse_default_ID";
			break;
		case "Stratum":
			$id_field = "stratum_default_ID";
			break;
		case "Soil":
			$id_field = "patch_default_ID";
			break;
		case "Zone":
			$id_field = "zone_default_ID";
			break;
		default:
			return FALSE;	
	}

	return $id_field;
}
?>
