<html>
<head>
<title>
RHESSys Definition File Database
</title>
<link rel="stylesheet" type="text/css" href="styles/global.css" />
</head>

<body>
<div id="header">
<div id="left_header">
<a href="index.php">Main</a>
</div>

<div id="right_header">
{if $active_watershed}
{$active_watershed} |
{/if}
{if $loggedin}
{$username} | <a href="logout.php">Logout</a>
{else}
<a href="authenticate.php">Login</a>
{/if}
</div>
</div> <!-- End of id="header"-->
