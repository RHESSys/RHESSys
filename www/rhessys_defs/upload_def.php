<?php
//require_once 'include/header.php';
$path = $_SERVER['DOCUMENT_ROOT'];
require "$path/Smarty/Smarty.class.php";

$smarty = new Smarty();
$smarty->template_dir = "$path/rhessys_defs/smarty/templates";
$smarty->compile_dir = "$path/rhessys_defs/smarty/templates_c";
$smarty->cache_dir = "$path/rhessys_defs/smarty/cache";
$smarty->config_dir = "$path/rhessys_defs/smarty/configs";

mysql_connect('localhost', 'root', '') or die(mysql_error());
$db_server = mysql_select_db('rhessys_defs') or die(mysql_error());

$cols = 'name';
$vals = '"' . $_FILES['filename']['name'] . '"';
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

	// For field names with a '.' in them, SQL requires the name
	// be in backticks
	if (strstr($items[1], '.')) {
		$cols = $cols . ', `' . $items[1] . '`';
	} else {
		$cols = $cols . ', ' . $items[1];
	}
	
	if (is_numeric($items[0])) {
		$vals = $vals . ", " . $items[0];
	} else {
		$vals = $vals . ", '" . $items[0] . "'";
	}
	
	$line = fgets($fh);
} 

fclose($fh);

$query = "INSERT INTO $table_name (" . $cols . ") VALUES(" . $vals . ");";
echo $query;
$result = mysql_query($query);
if (!$result) die("Database access failed: " . mysql_error());

mysql_close($db_server);

$smarty->display("$path/rhessys_defs/smarty/templates/upload_def.tpl");
?>
