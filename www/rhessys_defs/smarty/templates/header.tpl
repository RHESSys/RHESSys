<html>
<head>
<title>
RHESSys Definition File Database
</title>
</head>

<body>
<div>
<a href="index.php">Main</a>
</div>

<div>
{if $loggedin}
{$username} | <a href="logout.php">Logout</a>
{else}
<a href="authenticate.php">Login</a>
{/if}
</div>
