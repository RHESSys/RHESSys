{include file='header.tpl'}

<form method="POST" action="create_account.php">
<table>
<tr>
<td colspan='2'>
CREATE NEW ACCOUNT
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
<input type="submit" value="Create Account" />
</form>

{include file='footer.tpl'}
