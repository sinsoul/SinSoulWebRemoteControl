<?php
if($_GET["action"]=="logout")
{
	//setcookie("user_name","",time()-3600);
	//setcookie("user_id", "",time()-3600);
	session_start();
	session_destroy();
	die("OK");
}
?>
<!DOCTYPE html>
<html>
    <head>
        <meta charset="utf-8" />
        <title>SinSoul Web远程控制系统</title>    
        <!-- Our CSS stylesheet file -->
        <link rel="stylesheet" href="assets/css/login.css" />
        
        <!--[if lt IE 9]>
          <script src="http://html5shiv.googlecode.com/svn/trunk/html5.js"></script>
        <![endif]-->
		
    </head>    
    <body>
		<script type="text/javascript" src="assets/js/jquery-1.6.4.js"></script>
		<script type="text/javascript" src="assets/js/login.js"></script>
		<div id="formContainer">
			<form id="login" method="post" action="./"  onsubmit="login();">
				<a href="#" id="flipToRecover" class="flipLink">Register</a>
				<input type="text" name="UserName" id="UserName" value="UserName"  onfocus="CleanText(this.id);" onblur="RecoveryTips(this.id);"/>
				<input type="password" name="loginPass" id="loginPass" value="pass" onfocus="CleanText(this.id);" onblur="RecoveryTips(this.id);"/>
				<input type="submit" name="submit" value="Login" />

				<div id="logintips" name="logintips" class="tips"></div>
			</form>
			<form id="Register" method="post" action="./" onsubmit="Register();">
				<a href="#" id="flipToLogin" class="flipLink">Login</a>
				<input type="text" name="RegisterUser" id="RegisterUser" value="UserName"  onfocus="CleanText(this.id);" onblur="RecoveryTips(this.id);"/>
				<input type="password" name="RegisterPass" id="RegisterPass" value="pass"  onfocus="CleanText(this.id);" onblur="RecoveryTips(this.id);"/>
				<input type="password" name="RegisterRePass" id="RegisterRePass" value="pass"  onfocus="CleanText(this.id);" onblur="RecoveryTips(this.id);"/>
				<input type="text" name="RegisterEmail" id="RegisterEmail" value="Email"  onfocus="CleanText(this.id);" onblur="RecoveryTips(this.id);"/>
				<input type="submit" name="submit" value="Register" />
			</form>
		</div>
        <footer>
	        <h2><i>Curriculum Design:</i><a href="http://sinsoul.com/?page_id=33" target="_blank">SinSoul Web Remote Control</a></h2>
            <a class="tzine" target="_blank" href="http://sinsoul.com/?page_id=18">@author:SinSoul</a>
        </footer>
    </body>
</html>

