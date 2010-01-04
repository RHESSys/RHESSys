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
?>
