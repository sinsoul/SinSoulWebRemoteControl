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
	$cmd_id=addslashes($_POST['cmd_id']);
	$decode=addslashes($_POST['decode']);
	$user_name=$_SESSION["user_name"];
	$rc_host_id=$_SESSION["rc_host_id"];
	
//	$submit_time=addslashes($_POST['submit_time']);
//	$result_content=addslashes($_POST['result_content']);
//	$Verify="select `user_name`,`rc_host_id` from `rc_cmd` where (`cmd_id`='".$cmd_id."');";
	
	for($i=0;$i<300;$i++)
	{
		//从数据库中获取执行结果
		if($memcache->get($cmd_id)!="0")//if(memcache_get($memcache,$cmd_id)!="0")
		{
			$get_result="select `result_content` from `rc_result` where (`cmd_id`='".$cmd_id."');";
			$del_result="delete from `rc_result` where (`cmd_id`='".$cmd_id."');";
			require("ConnectDB.php");
			$queryresult = mysql_query($get_result,$database);
			if($queryresult)
			{
				if(mysql_num_rows($queryresult)!=0)
				{
					$cmdarray=mysql_fetch_assoc($queryresult);
				//	mysql_query($del_result,$database); //取出执行结果后将结果从数据库删除
					$memcache->delete($cmd_id,0); //删除以命令ID为key的MC，这货是一次性的。
					mysql_close($database);
					if($decode=="base64")
					{
						header("Content-Type: text/json; charset=utf-8");
						echo base64_decode($cmdarray['result_content']);
					}
					else
					{
						echo $cmdarray['result_content'];//."\r\ncmd_time:".$cmdarray['cmd_time']."\r\ncmd_content:".$cmdarray['cmd_content'];
					}
				}
				else
				{
					mysql_close($database);
					echo "Error:Requested user id not found in database";
				}
			}
			else
			{
				echo "Error:".mysql_error();
			}
			$memcache->close();
			exit(0);
		}
		
		//从SAE平台的KVDB获取执行结果
		// if($memcache->get($cmd_id)!="0")
		// {
			// $result_content = $sae_kvdb->get($cmd_id);
			// $sae_kvdb->delete($cmd_id);	
			// $memcache->delete($cmd_id,0);

			// if($decode=="base64")
			// {
				// header("Content-Type: text/json; charset=utf-8");
				// echo base64_decode($result_content);
			// }
			// else
			// {
				// echo $result_content;
			// }
			// $memcache->close();
			// exit(0);
		// }
		usleep(100000);
	}
	$memcache->close();
	echo "TIMEOUT";
?>