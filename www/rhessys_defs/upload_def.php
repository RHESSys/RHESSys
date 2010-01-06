<?php
$path = $_SERVER['DOCUMENT_ROOT'];
require "$path/rhessys_defs/include/smarty.php";
require_once "$path/rhessys_defs/include/login.php";
require_once "$path/rhessys_defs/include/util.php";


$cols = 'name';
$filename = explode(".", $_FILES['filename']['name']);
$vals = '"' . $filename[0] . '"';
$table_name = $_POST['type'];

if ($fh = fopen($_FILES['filename']['tmp_name'], 'r')) 

$line = fgets($fh);
// Check that this def file is of the proper type by checking
// the name of the ID field
//$line = trim($line);
//$items = preg_split("/[\s,]+/", $line);
//switch (items[1]) {
//	case "landuse_default_ID":
//		if ($table_name != "Land_Use")
//			die("Attempting to load a Land Use");
//		break;
//	case "patch_default_ID":
//		if ($table_name != "Soil")
//			die("Attempting to load a Soil");
//		break;
//	case "stratum_default_ID":
//		if ($table_name != "Stratum")
//			die("Attempting to load Stratum");
//		break;
//	case "zone_default_ID":
//		if ($table_name != "Zone") 
//			die("Attempting to load Zone");
//		break;
//	default:
//		die("Unrecognized default file type");
//		break;
//}

while (!feof($fh)) {
	$line = trim($line);
	$items = preg_split("/[\s,]+/", $line);
	echo "1: " . $items[1] . "<br />";
	echo "0: " . $items[0] . "<br />";
	
	// Capture the ID entry for creating the reference row
	if (strstr($items[1], "_default_ID")) {
		$ref_id_name = $items[1];
		$ref_id_value = $items[0];
	}

	// For field names with a '.' in them, SQL requires the name
	// be in backticks
	if (strstr($items[1], '.')) {
		$cols = $cols . ', `' . $items[1] . '`';
	} else {
		$cols = $cols . ', ' . $items[1];
	}

	// Quote string values	
	if (is_numeric($items[0])) {
		$vals = $vals . ", " . $items[0];
	} else {
		$vals = $vals . ", '" . $items[0] . "'";
	}
	
	$line = fgets($fh);
} 

fclose($fh);

// Create the data entry
$query = "INSERT INTO $table_name (" . $cols . ") VALUES(" . $vals . ");";
echo $query;
$result = mysql_query($query);
if (!$result) die("Database access failed: " . mysql_error());

// Create the reference entry
$ref_table_name = $table_name . "_Reference";
$query = "INSERT INTO $ref_table_name ($ref_id_name) VALUES($ref_id_value)";
echo $query;
mysql_query($query);

mysql_close($db_server);

$smarty->display("$path/rhessys_defs/smarty/templates/upload_def.tpl");
?>
