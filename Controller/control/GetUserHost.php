<?php
session_start();
//if (isset($_COOKIE["user_name"])&&$_COOKIE["user_name"]!="")
if(isset($_SESSION["user_name"])&&isset($_SESSION["user_id"]))
{
	require("../function/ConnectDB.php");
	//$user_name = addslashes($_COOKIE["user_name"]);
	$user_name = addslashes($_SESSION["user_name"]);
	$sqlcmd="select * from `rc_host` where (`user_name`='".$user_name."');";
	date_default_timezone_set('PRC');
	$now = getdate();
	$current_time=$now[year]."-".$now[mon]."-".$now[mday]." ".$now[hours].":".$now[minutes].":".$now[seconds];
	$query_user_host = mysql_query($sqlcmd,$database);
	if($query_user_host)
	{
		if(mysql_num_rows($query_user_host)!=0)
		{
			while($host_array=mysql_fetch_assoc($query_user_host))
			{
			  // `rc_host_id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
			  // `user_name` varchar(20) NOT NULL,
			  // `update_time` datetime NOT NULL,
			  // `rc_host_name` varchar(50) DEFAULT NULL,
			  // `rc_host_os` varchar(30) DEFAULT NULL,
			  $host_status;
			  $online="select `cmd_time` from `rc_cmd` where(`cmd_read`=1 and `user_name`='".$user_name."' and `rc_host_id`=".$host_array['rc_host_id'].")order by `cmd_id` desc limit 1;";
			  
			  $query_latest_cmd_time = mysql_query($online,$database);
			  if($query_latest_cmd_time)
			  {
				$latest_cmd_time=mysql_fetch_assoc($query_latest_cmd_time);
				if((strtotime($current_time)-strtotime($latest_cmd_time['cmd_time']))<70||
				(strtotime($current_time)-(strtotime($host_array['update_time'])))<70)
				{
					$host_status="在线";
				}
				else
				{
					$host_status="离线";
				}
				
			  }
			  else
			  {
				$host_status="未知";
			  }
			  $hostlist=$hostlist."<tr style=\"cursor:pointer\"><td>".$host_array['rc_host_id']."</td>
										<td>".$host_array['user_name']."</td>
										<td>".$host_array['update_time']."</td>
										<td>".$host_array['rc_host_name']."</td>
										<td>".$host_array['rc_host_os']."</td>
										<td>".$host_status."</td></tr>";	  
			}
		}
		else
		{
			$hostlist="<h2>你目前没有可供控制的主机，请下载客户端在被控主机上运行</h2>";
		}
	}
	else
	{
		echo "Error:".mysql_error();
	}
}
else
{
	header("location:../index.php");
}
?>

<!DOCTYPE html>
<html>
<head>
	<title>Select Host</title>
	<link rel="stylesheet" href="../assets/css/selecthost.css"/>
	<meta charset="utf-8">
	<script type="text/javascript" src="../assets/js/jquery-1.6.4.js"></script>
	<script type="text/javascript">
		$(function() 
		{
		  $("#SelectHost tr").click(function()
		  {
			hostid=$(this).closest("tr").find("td").eq(0).text();
			selectHostByID(hostid)
		  });  
		});

		function selectHostByID(hostid)
		{
			if(hostid=='#'||hostid=="")
			{
				return;
			}
			else
			{
				//alert(hostid);
				//$("#SelectHost").append("<tr><td>20</td><td>sinsoul</td><td>2012-05-06 12:11:29</td><td>SINSOUL-US</td><td>Windows 7</td><tr>");
				var xmlhttp;
				if (window.XMLHttpRequest)
				{
					xmlhttp=new XMLHttpRequest();
				}
				else
				{
					xmlhttp=new ActiveXObject("Microsoft.XMLHTTP");
				}
				xmlhttp.onreadystatechange=function()
				{
					if (xmlhttp.readyState==4 && xmlhttp.status==200)
					{
						
						if(xmlhttp.responseText==hostid)
						{
							location='VirtualDesktop.php';
						}
						else
						{
							alert("选择主机出错");
						}
					}
				}
				xmlhttp.open("POST","../function/SelectHost.php",true);
				xmlhttp.setRequestHeader("Content-type","application/x-www-form-urlencoded");
				xmlhttp.send("hostid="+hostid);
			
			}
		}
	</script>
</head>

<body>
<h2>选择要控制的主机</h2>
<table class="bordered" id="SelectHost">
    <thead>
    <tr>
        <th>#</th>        
        <th>用户名</th>
        <th>在线时间</th>
		<th>主机名</th>
		<th>操作系统</th>
		<th>状态</th>
    </tr>
    </thead>
	<?php echo $hostlist;?>
</table>
<br><br>
<div class="bordered" align="center" ><a href="http://nh6080.sinaapp.com/SSWRCDownload.php">下载客户端</a></div>
</body>
</html>