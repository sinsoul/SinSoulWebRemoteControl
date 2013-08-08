<?php
	session_start();
	session_write_close();
	//if (isset($_COOKIE["user_name"])||$_COOKIE["user_name"]!=""
	//	||isset($_COOKIE["rc_host_id"])||$_COOKIE["rc_host_id"]!="")
	if(isset($_SESSION["user_name"])&&
		isset($_SESSION["user_id"])&&
		isset($_SESSION["rc_host_id"]))
	{
		//echo  "";
	}
	else
	{
		echo "<script language='javascript'>";
		echo "location='../index.php';";
		echo "</script>";	
	}
	require("ConnectDB.php");
	require("ConnectMC.php");
  // CREATE TABLE `rc_cmd` (
  // `cmd_id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  // `cmd_type` int(11) NOT NULL,
  // `cmd_time` datetime NOT NULL,
  // `cmd_content` text NOT NULL,
  // `cmd_status` tinyint(4) NOT NULL,
  // `rc_host_id` bigint(20) unsigned NOT NULL,
  // `user_name` varchar(20) NOT NULL,
	
	
	$cmd_type = addslashes($_POST['cmd_type']);
	$cmd_content = addslashes($_POST['cmd_content']);
	
//	$rc_host_id = addslashes($_POST['rc_host_id']);
	$rc_host_id = addslashes($_SESSION["rc_host_id"]);
//	$user_name = addslashes($_POST['user_name']);
	$user_name = addslashes($_SESSION["user_name"]);
	
	date_default_timezone_set('PRC');
	$now = getdate();
	$cmd_time=$now[year]."-".$now[mon]."-".$now[mday]." ".$now[hours].":".$now[minutes].":".$now[seconds];
	
	$add_command="insert into `rc_cmd` (`cmd_type`,`cmd_time`,`cmd_content`,`cmd_status`,`cmd_read`,`rc_host_id`,`user_name`) values ('".$cmd_type."','".$cmd_time."','".$cmd_content."','0','0','".$rc_host_id."','".$user_name."');";
//	echo $add_command."</br>";
	$get_cmd_id="SELECT LAST_INSERT_ID()";//"select `cmd_id` from `rc_cmd` where (`cmd_type`='".$cmd_type."' and `cmd_time`='".$cmd_time."' and `cmd_content`='".$cmd_content."' and `rc_host_id`='".$rc_host_id."' and `user_name`='".$user_name."');";
	//and `cmd_status`='0'
//	echo $get_cmd_id."</br>";
	
	$mckey=$user_name."-".$rc_host_id;
	//die($mckey);
	$queryresult = mysql_query($add_command,$database);
	if($queryresult!=false)
	{
		$cmd_id=mysql_insert_id(); 
		// $new_cmd_id = mysql_query($get_cmd_id,$database);
		// if($new_cmd_id)
		// {
			// if(mysql_num_rows($new_cmd_id)!=0)
			// {
				// $cmd_id=mysql_fetch_assoc($new_cmd_id);
				// echo $cmd_id[cmd_id];
				echo $cmd_id;
				incrMC($memcache,$mckey);			//通知有新命令被加入
				$memcache->set($cmd_id,0);	//用于等待命令结果
				$memcache->close();
			// }
			// else
			// {
				// echo "false";
			// }
		//}
		// else
		// {
			// die('Query Error A1:'.mysql_error());
		// }
	}
	else
	{
		die('Query Error A2:'.mysql_error());
	}
	
	
//	$database=mysql_connect("localhost","root","initiald");  
//	if (!$database)
//	{
//		die('Could not connect: '.mysql_error());
//	}
//	mysql_select_db("remotecontrol",$database);
	
//	$database=mysql_connect(SAE_MYSQL_HOST_M.':'.SAE_MYSQL_PORT,SAE_MYSQL_USER,SAE_MYSQL_PASS);
	//$database=mysql_connect("localhost","root","initiald");
//	if (!$database)
//	{
//		die('Could not connect: '.mysql_error());
//	}
//	mysql_select_db(SAE_MYSQL_DB,$link);
	//mysql_select_db("remotecontrol",$database);
	
//	$database=new mysqli("localhost","root","initiald","remotecontrol");
	
	/*
	$existuser="select * from `Command` where (`cmd_user`='".$cmduser."');";
	$exist=mysql_query($existuser,$database);
	if($exist!=false)
	{
		if(mysql_num_rows($exist)==0)
		{
			$sql_statement="INSERT INTO `Command` (`cmd_user`,`cmd_type`,`cmd_time`,`cmd_content`) VALUES ('".$cmduser."','".$cmdtype."','".$cmd_time."','".$command."');";
		//	echo $sql_statement;
			$queryresult = mysql_query($sql_statement,$database);
			if($queryresult!=false)
			{
				echo "Add Success!\r\n";
			}
			else
			{
				die('Query Error:'.mysql_error());
			}
		//	echo $queryresult;
		}
		else
		{
			$updaterecord="UPDATE `Command` SET `cmd_time`='".$cmd_time."',`cmd_type`='".$cmdtype."',`cmd_content`='".$command."' WHERE (`cmd_user`='".$cmduser."');";
			$queryresult = mysql_query($updaterecord,$database);
			if($queryresult!=false)
			{
				echo "Update Success!\r\n";
			}
			else
			{
				die('Query Error:'.mysql_error());
			}
		}
	}
	else
	{
		die('Query Error:'.mysql_error());
	}
	*/
	mysql_close($database);
?>