<?php
	session_start();
// CREATE TABLE `rc_host` (
  // `rc_host_id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  // `user_name` varchar(20) NOT NULL,
  // `update_time` datetime NOT NULL,
  // `rc_host_name` varchar(50) DEFAULT NULL,
  // `rc_host_os` varchar(30) DEFAULT NULL,
  // PRIMARY KEY (`rc_host_id`),
  // KEY `fk_user_name_h` (`user_name`),2012-03-21 14:42:35
  // CONSTRAINT `fk_user_name_h` FOREIGN KEY (`user_name`) REFERENCES `rc_user` (`user_name`) ON DELETE NO ACTION ON UPDATE NO ACTION
// ) ENGINE=InnoDB DEFAULT CHARSET=gbk;


	require("ConnectDB.php");
	require("ConnectMC.php");
//	$rc_host_id = addslashes($_POST['rc_host_id']);
	$user_name = strtolower(addslashes($_POST['user_name']));
//	$user_name = iconv('ANSI','GBK',$_POST['user_name']);
//	$update_time = addslashes($_POST['update_time']);
	$rc_host_name= addslashes($_POST['rc_host_name']);
	$rc_host_os = addslashes($_POST['rc_host_os']);
	
	date_default_timezone_set('PRC');
	$now = getdate();
	$update_time=$now[year]."-".$now[mon]."-".$now[mday]." ".$now[hours].":".$now[minutes].":".$now[seconds];

	$add_host="INSERT INTO `rc_host` (`user_name`,`update_time`,`rc_host_name`,`rc_host_os`) VALUES ('".$user_name."', '".$update_time."', '".$rc_host_name."', '".$rc_host_os."');";
	
	$get_host_id="select `rc_host_id` from `rc_host` where (`user_name`='".$user_name."' and `update_time`='".$update_time."' and `rc_host_name`='".$rc_host_name."' and `rc_host_os`='".$rc_host_os."');";

	$exist_host="select `rc_host_id` from `rc_host` where (`user_name`='".$user_name."' and `rc_host_name`='".$rc_host_name."' and `rc_host_os`='".$rc_host_os."');";

	$update_exist_host="UPDATE `rc_host` SET `update_time`='".$update_time."' WHERE (`user_name`='".$user_name."' and `rc_host_name`='".$rc_host_name."' and `rc_host_os`='".$rc_host_os."');";
	
	$check_user_name="select * from `rc_user` where(`user_name`='".$user_name."');";
	
	$exist_user=mysql_query($check_user_name,$database);
	if($exist_user)
	{
		if(mysql_num_rows($exist_user)==0)
		{
			echo $user_name."并不是已注册用户.";
			session_destroy();
			exit;
		}
	}
	else
	{
		die('Query Error:'.mysql_error());
	}
	// $exist=mysql_query($existuser,$database);
	// if($exist!=false)
	// {
		// if(mysql_num_rows($exist)==0)
		// {
			// $sql_statement="INSERT INTO `Command` (`cmd_user`,`cmd_type`,`cmd_time`,`cmd_content`) VALUES ('".$cmduser."','".$cmdtype."','".$cmd_time."','".$command."');";
		//	echo $sql_statement;
			// $queryresult = mysql_query($sql_statement,$database);
			// if($queryresult!=false)
			// {
				// echo "Add Success!\r\n";
			// }
			// else
			// {
				// die('Query Error:'.mysql_error());
			// }
		//	echo $queryresult;
		// }
		// else
		// {
			// $updaterecord="UPDATE `Command` SET `cmd_time`='".$cmd_time."',`cmd_type`='".$cmdtype."',`cmd_content`='".$command."' WHERE (`cmd_user`='".$cmduser."');";
			// $queryresult = mysql_query($updaterecord,$database);
			// if($queryresult!=false)
			// {
				// echo "Update Success!\r\n";
			// }
			// else
			// {
				// die('Query Error:'.mysql_error());
			// }
		// }
	// }	
	
	
//	echo $add_host;
//	echo $get_host_id;
	 

		$exist_host_id = mysql_query($exist_host,$database);
		if($exist_host_id)
		{
			if(mysql_num_rows($exist_host_id)!=0)
			{
				$host_id=mysql_fetch_assoc($exist_host_id);
				
				if(mysql_query($update_exist_host,$database))
				{
					//echo "Update Success!</br> Host ID: ".$host_id[rc_host_id];
					//echo $host_id[rc_host_id];
					$_SESSION["rc_host_id"]=$host_id[rc_host_id];
					$_SESSION["user_name"]=$user_name;
					echo "OK";
				}
				else
				{
					echo "Update Error or Get Host ID Error!";
				}
	
			}	
			else
			{
				$addresult = mysql_query($add_host,$database);
				if($addresult)
				{
					$new_host_id = mysql_query($get_host_id,$database);
					if($new_host_id)
					{
						if(mysql_num_rows($new_host_id)!=0)
						{
							$host_id=mysql_fetch_assoc($new_host_id);
							//echo $host_id[rc_host_id];
							$_SESSION["rc_host_id"]=$host_id[rc_host_id];
							$_SESSION["user_name"]=$user_name;
							echo "OK";
						}
						else
						{
							echo "Add Error or Get Host ID Error!";
						}
					}
					else
					{
						die('Query Error:'.mysql_error());
					}
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
	//以“用户名-主机ID”为key，创建Memcache以进行命令通知,可加入salt防止仿冒MCkey
	$mckey=$user_name."-".$host_id[rc_host_id];
	$memcache->set($mckey,0);
	$memcache->close();
	$mark_as_read="update `rc_cmd` set `cmd_read`='1' where (`rc_host_id`='".$host_id[rc_host_id]."' and `user_name`='".$user_name."' and `cmd_read`='0');";
	mysql_query($mark_as_read,$database);
	mysql_close($database);
	//echo "error:".$mark_as_read;
?>