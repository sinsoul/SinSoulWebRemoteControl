<?php
	header("Content-Type:text/html;charset=utf-8");
	require("ConnectDB.php");
	$reguser = strtolower(addslashes($_POST['reguser']));
	$regpass = addslashes($_POST['regpass']);
	$regmail = addslashes($_POST['regmail']);
	
	if(strlen($reguser)<5)
	{
		die("用户名不能少于5个字符");
	}
	if(strlen($regpass)<6)
	{
		die("密码不能少于6个字符");
	}
	//str_replace(" ", "",$reguser);
	//$pwd=md5($reguser.$regpass."FuckGFW");
	
	//echo "用户名:".$reguser."密码:".$regpass."邮箱:".$regmail;
	$existuser="select * from `rc_user` where (`user_name`='".$reguser."');";
	$exist=mysql_query($existuser,$database);
	if($exist!=false)
	{
		if(mysql_num_rows($exist)==0)
		{
			$existemail="select * from `rc_user` where (`e-mail`='".$regmail."');";
			//echo $existemail;
			$exist=mysql_query($existemail,$database);
			if(mysql_num_rows($exist)==0)
			{
				$registerstr="INSERT INTO `rc_user` VALUES ('".$reguser."', '".$regpass."', '".$regmail."');";
				$regresult=mysql_query($registerstr,$database);
				if($regresult!=false)
				{
					echo "注册成功";
				}
				else
				{
					mysql_close($database);
					die('RegError:'.mysql_error());
				}				
			}
			else
			{
				mysql_close($database);
				die("邮箱已存在，请换一个");
			}
		}
		else
		{
			mysql_close($database);
			die("用户名已经存在，请换一个");
		}
	}
	else
	{
		die('Query Error:'.mysql_error());
	}
	mysql_close($database);
?>