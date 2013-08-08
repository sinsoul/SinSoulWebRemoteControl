<?php
session_start();
if(isset($_SESSION["user_name"])&&
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
?>

<!DOCTYPE HTML>
<html>
<head>
<meta charset="utf-8"/>
	<title>CameraViewer</title>
	<script type="text/javascript" src="../assets/js/jquery-1.6.4.js"></script>
</head>
<body>

<div id="doc" name="doc">
    <button type="button" name="monitor_sw" id="monitor_sw" onclick="Monitor_Switch()" >开始查看</button>
	<div id="DeviceStatus"></div>
	<div id="bd">
	<img src="" id="ScreenOutput">
		<script type="text/javascript">
		var start_switch=0;
		
		function Monitor_Switch()
		{
			if(start_switch==0)
			{
				
				$("#monitor_sw").attr("disabled",true);	
				startobj=$.ajax({
					url: "../function/AddCommand.php",
					type: 'POST',
					data: {cmd_type:"3",cmd_content:"StartCamera"},
					success: function(cmdid){
											//alert(text)
											statusobj=$.ajax({
												url: "../function/GetResult.php",
												type: "POST",
												data: {cmd_id:cmdid},
												success: function(status){
												if(status=="1")
												{
													sc_id=Math.floor(Math.random()*823);
													addcommand(sc_id);
													start_switch=1
													document.getElementById("monitor_sw").innerHTML="停止查看";
													$("#monitor_sw").removeAttr("disabled");
												}
												else
												{
													alert("摄像头启动失败");
													$("#monitor_sw").removeAttr("disabled");
												}
												}
											})
									
											}
				});
			}
			else
			{
				$("#monitor_sw").attr("disabled",true);
				stopobj=$.ajax({
				url: "../function/AddCommand.php",
				type: 'POST',
				data: {cmd_type:"5",cmd_content:"StopCamera"},
				success: function(cmdid){
										statusobj=$.ajax({
											url: "../function/GetResult.php",
											type: "POST",
											data: {cmd_id:cmdid},
											success: function(status){
											if(status=="1")
											{
												document.getElementById("monitor_sw").innerHTML="开始查看";
												start_switch=0;
												$("#monitor_sw").removeAttr("disabled");
											}
											else
											{
												$("#monitor_sw").removeAttr("disabled");
												alert("摄像头停止失败");
											}
											}
										})
								
										}
			});
			}
		}
		function addcommand(sc_id)
		{
			var cmd_post_data="cmd_type=2&cmd_content=CameraCapture"+sc_id;
			var ac_xmlhttp;
			if (window.XMLHttpRequest)
			{
				ac_xmlhttp=new XMLHttpRequest();
			}
			else
			{
				ac_xmlhttp=new ActiveXObject("Microsoft.XMLHTTP");
			}
			ac_xmlhttp.onreadystatechange=function()
			{
				if (ac_xmlhttp.readyState==4 && ac_xmlhttp.status==200)
				{
					current_cmdid=ac_xmlhttp.responseText;
					getresult(current_cmdid);
					ac_xmlhttp=null;
				}
			}
			ac_xmlhttp.open("POST","../function/AddCommand.php",true);
			ac_xmlhttp.setRequestHeader("Content-type","application/x-www-form-urlencoded");
			ac_xmlhttp.send(cmd_post_data);
		}

		function getresult(current_cmdid)
		{
			var gr_xmlhttp;
			if (window.XMLHttpRequest)
			{
				gr_xmlhttp=new XMLHttpRequest();
			}
			else
			{
				gr_xmlhttp=new ActiveXObject("Microsoft.XMLHTTP");
			}
			gr_xmlhttp.onreadystatechange=function()
			{
				if (gr_xmlhttp.readyState==4 && gr_xmlhttp.status==200)
				{
				//alert(gr_xmlhttp.responseText.length);
					if (gr_xmlhttp.responseText.length<256&&gr_xmlhttp.responseText.length>2)
					{
						if(gr_xmlhttp.responseText=="Stoped")
						{
							return;
						}
						gr_xmlhttp=null;
						getresult(current_cmdid);
					}
					else
					{
						document.getElementById('ScreenOutput').src='data:image/jpeg;base64,' + gr_xmlhttp.responseText;
						gr_xmlhttp=null;
						
						if(start_switch==1)
						{
							sc_id=current_cmdid;
							current_cmdid=0;
							addcommand(sc_id);
						}
					}
				}
			}
			gr_xmlhttp.open("POST","../function/GetResult.php",true);
			gr_xmlhttp.setRequestHeader("Content-type","application/x-www-form-urlencoded");
			gr_xmlhttp.send("cmd_id="+current_cmdid);
		}
		</script>
	</div> 
	<div id="ft"></div>
</div>
</body>
</html>