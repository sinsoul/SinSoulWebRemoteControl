<?php
	session_start();
	if(isset($_SESSION["user_name"])&&
		isset($_SESSION["rc_host_id"]))
	{
		//echo  "";
	}
	else
	{
		die("ERROR");
	}
	require("ConnectMC.php");
	require("ConnectDB.php");
	$user_name=$_SESSION["user_name"];
	$rc_host_id=$_SESSION["rc_host_id"];
	$mckey=$user_name."-".$rc_host_id;
	
	date_default_timezone_set('PRC');
	$now = getdate();
	$cmd_time=$now[year]."-".$now[mon]."-".$now[mday]." ".$now[hours].":".($now[minutes]-2).":".($now[seconds]-1);
	$logout_time=$now[year]."-".$now[mon]."-".$now[mday]." ".$now[hours].":".($now[minutes]-2).":".($now[seconds]-1);
	$add_command="insert into `rc_cmd` (`cmd_type`,`cmd_time`,`cmd_content`,`cmd_status`,`cmd_read`,`rc_host_id`,`user_name`) values ('88','".$cmd_time."','logout','0','0','".$rc_host_id."','".$user_name."');";
	if(!mysql_query($add_command,$database))
	{
		die('Query Error CL1:'.mysql_error());
	}
	incrMC($memcache,$mckey);
	
	$update_exist_host="UPDATE `rc_host` SET `update_time`='".$logout_time."' WHERE (`user_name`='".$user_name."' and `rc_host_id`='".$rc_host_id."');";
	if(!mysql_query($update_exist_host,$database))
	{
		die('Query Error CL2:'.mysql_error());
	}
	mysql_close($database);
	$memcache->close();
	@session_destroy();
	die("OK");
?>