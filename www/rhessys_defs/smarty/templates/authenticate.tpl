{include file='header.tpl'}

<form method="POST" action="authenticate.php">
<table>
<tr>
<td colspan='2'>
LOGIN
</td>
</tr>

<tr>
<td>Username:</td>
<td><input type="text" name="username" /></td>
</tr>
<tr>
<td>Password:</td>
<td><input type="password" name="password" /></td>
</tr>
</table>
<input type="submit" value="Login">
</form>
<a href="create_account.php">Create New Account</a>

{include file='footer.tpl'}
