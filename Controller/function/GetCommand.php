<?php
	session_start();
	session_write_close();
	if(isset($_SESSION["user_name"])&&isset($_SESSION["rc_host_id"]))
	{
		
	}
	else
	{
		die("ERROR");
	}
	require("ConnectMC.php");
	
	// CREATE TABLE `rc_cmd` (
	// `cmd_id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
	// `cmd_type` int(11) NOT NULL,
	// `cmd_time` datetime NOT NULL,
	// `cmd_content` text NOT NULL,
	// `cmd_status` tinyint(4) NOT NULL,
	// `rc_host_id` bigint(20) unsigned NOT NULL,
	// `user_name` varchar(20) NOT NULL,

	// $user_name = addslashes($_POST['user_name']);
	// $rc_host_id = addslashes($_POST['rc_host_id']);
	 
	 $user_name = $_SESSION["user_name"];
	 $rc_host_id = $_SESSION["rc_host_id"];
	 
	 // $user_name = addslashes($_GET['user_name']);
	 // $rc_host_id = addslashes($_GET['rc_host_id']);
	
	$mckey=$user_name."-".$rc_host_id;
	//die($mckey);
	for($i=1;$i<290;$i++)
	{
		if($memcache->get($mckey)!="0")
		//if(memcache_get($memcache,$mckey)!="0")
		{
			require("ConnectDB.php");
			$sql_get_cmd="select `cmd_id`,`cmd_type`,`cmd_time`,`cmd_content` from `rc_cmd` where (`user_name`='".$user_name."' and `rc_host_id`='".$rc_host_id."' and `cmd_status`='0' and `cmd_read`='0');";
			$queryresult = mysql_query($sql_get_cmd,$database);
			if($queryresult!=false)
			{
				if(mysql_num_rows($queryresult)!=0)
				{
					$cmdarray=mysql_fetch_assoc($queryresult);
					$mark_as_read="update `rc_cmd` set `cmd_read`='1' where (`cmd_id`='".$cmdarray['cmd_id']."');";
					mysql_query($mark_as_read,$database);
					decrMC($memcache,$mckey);
					mysql_close($database);
				//////////////////以下部分用于LAMP/WAMP环境//////////////////
				
					// echo "<cmd_id>".$cmdarray['cmd_id'].
						// "</cmd_id><cmd_type>".$cmdarray['cmd_type'].
						// "</cmd_type><cmd_time>".$cmdarray['cmd_time'].
						// "</cmd_time><cmd_content>".stripcslashes($cmdarray['cmd_content'])."</cmd_content>";
				
				//////////////////以下部分用于SAE环境//////////////////
					echo "<cmd_id>".$cmdarray['cmd_id'].
						"</cmd_id><cmd_type>".$cmdarray['cmd_type'].
						"</cmd_type><cmd_time>".$cmdarray['cmd_time'].
						"</cmd_time><cmd_content>".$cmdarray['cmd_content']."</cmd_content>";
				}
				else
				{
					echo "False\r\n\r\n";
				}
			}
			else
			{
				echo "Query False G1:".mysql_error();
			}
			$memcache->close();
			exit(0);
		}
		usleep(200000);
	}
	$memcache->close();
	date_default_timezone_set('PRC');
	$now = getdate();
	$update_time=$now[year]."-".$now[mon]."-".$now[mday]." ".$now[hours].":".$now[minutes].":".$now[seconds];
	$update_exist_host="UPDATE `rc_host` SET `update_time`='".$update_time."' WHERE (`user_name`='".$user_name."' and `rc_host_id`='".$rc_host_id."');";
	require("ConnectDB.php");
	mysql_query($update_exist_host,$database);
	mysql_close($database);
	echo "TIMEOUT";
?>