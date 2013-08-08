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
	<meta charset="utf-8">
	<title>TaskManager</title>
	<script type="text/javascript" src="../assets/js/jquery-1.6.4.js"></script>
	<script type="text/javascript" src="../assets/js/Decode.js"></script>
	<script type="text/javascript" src="../assets/js/TaskManager.js"></script>
	<link type="text/css" href="../assets/css/filemanager.css" rel="stylesheet">
	<link type="text/css" href="../assets/css/taskmanager.css" rel="stylesheet">
</head>
<body>
<div class="task-manager" style="height: 560px; width:800px; overflow: hiden;">
	<div id="toolbar" style="padding:5px 10px;margin:0 0 5px;background-color:#f5f5f5;background-repeat:repeat-x;background-image:-khtml-gradient(linear,left top,left bottom,from(#fff),to(#f5f5f5));background-image:-moz-linear-gradient(top,#fff,#f5f5f5);background-image:-ms-linear-gradient(top,#fff,#f5f5f5);background-image:-webkit-gradient(linear,left top,left bottom,color-stop(0%,#fff),color-stop(100%,#f5f5f5));background-image:-webkit-linear-gradient(top,#fff,#f5f5f5);background-image:-o-linear-gradient(top,#fff,#f5f5f5);background-image:linear-gradient(top,#fff,#f5f5f5);filter:progid:DXImageTransform.Microsoft.gradient(startColorstr = '#ffffff',endColorstr = '#f5f5f5',GradientType = 0);border:1px solid #ddd;-webkit-border-radius:3px;-moz-border-radius:3px;border-radius:3px;-webkit-box-shadow:inset 0 1px 0 #fff;-moz-box-shadow:inset 0 1px 0 #fff;box-shadow:inset 0 1px 0 #fff;position:relative;top:0px;left:0px height:20px">
		<button type="button" class="btn" id="new">新建运行</button>
		<button type="button" class="btn" id="terminal">结束进程</button>
		<button type="button" class="btn" id="enum_module">列举模块</button>
		<button type="button" class="btn" id="enum_thread">列举线程</button>
		<button type="button" class="btn" id="refresh">刷新列表</button>
	</div>
	<div id="task_grid" style="height: 525px;width:800px;">
		<div class="head-row" style="height: 25px;width:800px;">
			<table class="ai-list">
				<thead>
					<tr>
						<td class="select">*</td>
						<td class="name clickable"><div class="column-title">映像名称</div></td>
						<td class="pid clickable"><div class="column-title">PID</div></td>
						<td class="user clickable"><div class="column-title">用户名</div></td>
						<td class="mem clickable"><div class="column-title">内存(KB)</div></td>
						<td class="path clickable"><div class="column-title">路径</div></td>
						
					</tr>
				</thead>
			</table>
		</div>
		<div id="process-list" style="height: 500px; overflow-y:scroll; overflow-x:hidden; padding: 0px; width: 800px;">		
			<table class="ai-list">
				<tbody id="task-list">
					
				</tbody>
			</table>
		</div>
	</div>
</div>
</body>
</html>