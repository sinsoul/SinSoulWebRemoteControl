<?php
	
//	$sql_statement="select `cmd_type`,`cmd_time`,`cmd_content` from `command` where (cmd_user='".$cmduser."' and cmd_id='".$cmdid."');";
//	$queryresult = mysql_query($sql_statement,$database);
if($_GET[action] == install)
{
	require("function/ConnectDB.php");

	$ar[0]="SET FOREIGN_KEY_CHECKS=0;";
	$ar[1]="DROP TABLE IF EXISTS `rc_cmd`;";
	$ar[2]="CREATE TABLE `rc_cmd` (  `cmd_id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,  `cmd_type` int(11) NOT NULL,  `cmd_time` datetime NOT NULL,  `cmd_content` text NOT NULL,  `cmd_status` tinyint(4) NOT NULL, `cmd_read` tinyint(4) NOT NULL, `rc_host_id` bigint(20) unsigned NOT NULL,  `user_name` varchar(20) NOT NULL,  PRIMARY KEY (`cmd_id`),  KEY `fk_host_id_c` (`rc_host_id`),  KEY `fk_user_name_c` (`user_name`),  CONSTRAINT `fk_host_id_c` FOREIGN KEY (`rc_host_id`) REFERENCES `rc_host` (`rc_host_id`) ON DELETE NO ACTION ON UPDATE NO ACTION,  CONSTRAINT `fk_user_name_c` FOREIGN KEY (`user_name`) REFERENCES `rc_user` (`user_name`) ON DELETE NO ACTION ON UPDATE NO ACTION) ENGINE=InnoDB DEFAULT CHARSET=utf8;";
	$ar[3]="DROP TABLE IF EXISTS `rc_host`;";
	$ar[4]="CREATE TABLE `rc_host` (  `rc_host_id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,  `user_name` varchar(20) NOT NULL,  `update_time` datetime NOT NULL,  `rc_host_name` varchar(50) DEFAULT NULL,  `rc_host_os` varchar(30) DEFAULT NULL,  PRIMARY KEY (`rc_host_id`),  KEY `fk_user_name_h` (`user_name`),  CONSTRAINT `fk_user_name_h` FOREIGN KEY (`user_name`) REFERENCES `rc_user` (`user_name`) ON DELETE NO ACTION ON UPDATE NO ACTION) ENGINE=InnoDB DEFAULT CHARSET=utf8;";
	$ar[5]="DROP TABLE IF EXISTS `rc_result`;";
	$ar[6]="CREATE TABLE `rc_result` (  `cmd_id` bigint(20) unsigned NOT NULL,  `submit_time` datetime NOT NULL,  `result_content` longtext NOT NULL,  PRIMARY KEY (`cmd_id`),  CONSTRAINT `fk_cmd_id_r` FOREIGN KEY (`cmd_id`) REFERENCES `rc_cmd` (`cmd_id`) ON DELETE NO ACTION ON UPDATE NO ACTION) ENGINE=InnoDB DEFAULT CHARSET=utf8;";
	$ar[7]="DROP TABLE IF EXISTS `rc_user`;";
	$ar[8]="CREATE TABLE `rc_user` (  `user_name` varchar(30) NOT NULL,  `password` varchar(50) NOT NULL,  `e-mail` varchar(100) NOT NULL,  PRIMARY KEY (`user_name`)) ENGINE=InnoDB DEFAULT CHARSET=utf8;";
//	$ar[9]="DROP TRIGGER IF EXISTS `t_after_submit_result`;";
//	$ar[10]="DELIMITER ;;";
//	$ar[11]="CREATE TRIGGER `t_after_submit_result` AFTER INSERT ON `rc_result` FOR EACH ROW beginUPDATE `rc_cmd` SET `cmd_status`='1' WHERE (`cmd_id`=new.cmd_id);end;;";
//	$ar[12]="DELIMITER ;";
//	$ar[13]="INSERT INTO `rc_user` VALUES ('sinsoul', 'BH*2hG(IOhw0', 'root@sinsoul.com');";
//	$ar[14]="INSERT INTO `rc_host` VALUES ('1', 'sinsoul', '2012-03-21 14:42:35', 'SinSoul', 'Windows 7');";
//	$ar[15]="INSERT INTO `rc_cmd` VALUES ('1', '0', '2012-03-21 14:42:35', 'ipconfig /all', '0', '0', '1', 'sinsoul');";
//	$ar[16]="INSERT INTO `rc_result` VALUES ('1', '2012-03-21 14:42:35', '123');";
	for($i=0;$i<9;$i++)
	{
		echo "Query:".$ar[$i]."</br>";
		$queryresult=mysql_query($ar[$i],$database);
		if($queryresult==false)
		{
			echo '<h5>Query Error:'.mysql_error()."</h5>";
		}
		else
		{
			echo "Success!!</br></br>";
		}
	}
}
/*
if($_GET[action] == install)
{

	require("ConnectSAEDB.php");
	$sql_file = file_get_contents("./Create.sql");
	echo $sql_file;
	if(mysqli_multi_query($sql_file,$database))
	{
		header("location:index.php");
		exit;
	}
	else
	{
		die('Query Error:'.mysql_error());
	}
	
}
*/
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>安装程序</title>
</head>

<body>
<form name=form method=post action=Install.php?action=install>
<table>
	<tr><td colspan=2><input type=submit value=安装></td></tr>
</table>
</form>
</body>
</html>
