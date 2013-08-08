<?php
	header("Content-Type:text/html;charset=utf-8");
	session_start();
	require("ConnectDB.php");
	$loginuser = addslashes(strtolower($_POST['loginuser']));
	$loginpass = addslashes($_POST['loginpass']);
	//str_replace(" ", "",$reguser);
	//$pwd=md5($reguser.$regpass."FuckGFW");

	$existuser="select * from `rc_user` where (`user_name`='".$loginuser."'and `password`='".$loginpass."');";

	$exist=mysql_query($existuser,$database);
	if($exist!=false)
	{
		if(mysql_num_rows($exist)==0)
		{
			echo "false";
		}
		else
		{
			$_SESSION["user_name"]=strtolower($loginuser);
			$_SESSION["user_id"]=md5($reguser.$regpass."FuckGFW");
			//setcookie("user_name",strtolower($loginuser),time()+360000,'/');
			//setcookie("user_id", md5($reguser.$regpass."FuckGFW"),time()+360000,'/');
			//header("location:VirtualDesktop.php");
			echo "true";
		}
	}
	else
	{
		die('Query Error:'.mysql_error());
	}
	mysql_close($database);
?>