<?php
	require("ConnectDB.php");
	require("ConnectMC.php");
	$cmd_id=addslashes($_POST['cmd_id']);
//	$submit_time=addslashes($_POST['submit_time']);
	$result_content=addslashes($_POST['result_content']);

	date_default_timezone_set('UTC');
	$now = getdate();
	$submit_time=$now[year]."-".$now[mon]."-".$now[mday]." ".$now[hours].":".$now[minutes].":".$now[seconds];
	
//	echo $submit_time;
	$submit_result="insert into `rc_result` (`cmd_id`,`submit_time`,`result_content`) values ('".$cmd_id."','".$submit_time."','".$result_content."');";
//	echo $submit_result;
	$modify_cmd_status="UPDATE `rc_cmd` SET `cmd_status`='1' WHERE (`cmd_id`='".$cmd_id."');";
//	echo $modify_cmd_status;
	
	//使用数据库存储执行结果
	$queryresult = mysql_query($submit_result,$database);
	
	incrMC($memcache,$cmd_id);
	$memcache->close();
	
	if($queryresult)
	{
		echo "Add Success!";
		$queryresult = mysql_query($modify_cmd_status,$database);
		if($queryresult)
		{
			mysql_close($database);
			die("Modify Success!");	
		}
		else
		{
			die('Query Error S1:'.mysql_error());
		}
	}
	else
	{
		die('Query Error S2:'.mysql_error());
	}
	
	
	//使用SAE平台的KVDB存储执行结果
	// $sae_kvdb->set($cmd_id,$result_content);
	// incrMC($memcache,$cmd_id);
	// $memcache->close();
	// echo "Add Success!";
	// $queryresult = mysql_query($modify_cmd_status,$database);
	// if($queryresult)
	// {
		// mysql_close($database);
		// die("Modify Success!");	
	// }
	// else
	// {
		// die('Query Error S1:'.mysql_error());
	// }
	

?>