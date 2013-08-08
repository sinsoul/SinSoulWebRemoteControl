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

<!DOCTYPE html>
<html>
<head>
	<meta charset="utf-8"/>
	<link href="icon/favicon.ico" rel="shortcut icon">
	<link type="text/css" rel="stylesheet" media="all" href="../assets/css/console.css"></link>
	<script type="text/javascript" src="../assets/js/jquery-1.6.4.js"></script>
	<script type="text/javascript" src="../assets/js/console.js"></script>
	<script type="text/javascript" src="../assets/js/Decode.js"></script>
	<title>Command Shell-<?php echo $_SESSION["user_name"];?></title>
</head>
<body>
	<div class="consoleUI">
		<div id="consoleTitle"><img class="consoleIcon" src="../assets/img/icons/js.png">Command Shell-<?php echo $_SESSION["user_name"];?></div>
		<div class="console" id="consoleClient"></div>
		<div class="keyInput" > 
			<textarea id="inputBuf"></textarea>
			<button id="ClearBtn" >清屏</button> 
			<button id="CloseBtn">关闭</button>
		</div>
	</div>
</body>
</html>