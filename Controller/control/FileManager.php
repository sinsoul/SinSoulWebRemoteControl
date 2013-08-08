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
	<title>FileManager</title>
	<script type="text/javascript" src="../assets/js/jquery-1.6.4.js"></script>
	<script type="text/javascript" src="../assets/js/jquery.jstree.js"></script>
	<script type="text/javascript" src="../assets/js/Decode.js"></script>
	<script type="text/javascript" src="../assets/js/FileManager.js"></script>
	<link type="text/css" href="../assets/css/filemanager.css" rel="stylesheet">
</head>
<body>
<div class="file-body" style="height: 550px; width:950px;overflow: hiden;">
	<div class="ai-toolbar" style="padding:5px 10px;margin:0 0 18px;background-color:#f5f5f5;background-repeat:repeat-x;background-image:-khtml-gradient(linear,left top,left bottom,from(#fff),to(#f5f5f5));background-image:-moz-linear-gradient(top,#fff,#f5f5f5);background-image:-ms-linear-gradient(top,#fff,#f5f5f5);background-image:-webkit-gradient(linear,left top,left bottom,color-stop(0%,#fff),color-stop(100%,#f5f5f5));background-image:-webkit-linear-gradient(top,#fff,#f5f5f5);background-image:-o-linear-gradient(top,#fff,#f5f5f5);background-image:linear-gradient(top,#fff,#f5f5f5);filter:progid:DXImageTransform.Microsoft.gradient(startColorstr = '#ffffff',endColorstr = '#f5f5f5',GradientType = 0);border:1px solid #ddd;-webkit-border-radius:3px;-moz-border-radius:3px;border-radius:3px;-webkit-box-shadow:inset 0 1px 0 #fff;-moz-box-shadow:inset 0 1px 0 #fff;box-shadow:inset 0 1px 0 #fff;position:relative;top:0px;left:0px">
		<div class="search" style="position:relative;float:left;top:0px;width:515px">
				<input id="address_input" style="position:relative;float:left;top:0px;width:385px" type="text">
				<button type="button" class="btn" id="Enter" style="position:relative;float:left;left:0">转到</button>
			 	<div class="icon-nav nav_btn btn_back btn_back_disable" id="toolbar_back"></div>
				<div class="icon-nav nav_btn btn_forward btn_forward_disable" id="toolbar_forward"></div>
		</div>

		<div class="actions">
		
			<button type="button" id="refreshFileBtn" title="Refresh"><span class="icon-btn btn-refresh"></span> </button>
			<button class="partition"></button>
			<button type="button" id="newFolderBtn" title="New folder"><span class="icon-btn btn-newfolder"></span> </button>
			<button type="button" class="disabled" id="renameBtn" menu="Rename" title="Rename"><span class="icon-btn btn-rename"></span></button>
			<button type="button" class="disabled" id="cutBtn" title="Cut"><span class="icon-btn btn-cut"></span></button>
			<button type="button" class="disabled" id="copyBtn" title="Copy"><span class="icon-btn btn-copy"></span></button>
			<button type="button" class="disabled" id="pasteBtn" title="Paste"><span class="icon-btn btn-paste"></span></button>
			<button class="partition"></button>
			<button type="button" class="" id="deleteBtn" title="Delete"><span class="icon-btn btn-delete"></span></button>
		</div>
		<div class="btns">
			<button type="button" class="btn" id="downloadFileBtn">下载</button>
			<button type="button" class="btn btn-primary" id="importFileBtn">上传</button>
		</div>
	</div>

	<div id="file_tree" class="file-tree jspScrollable" style="overflow-x: scroll;overflow-y: scroll; padding: 0px; width: 260px; height: 550px;position:absolute;float:left;top:47px;" tabindex="0">
		<div id="FileTree" style="position:absolute;height: 550px;overflow:hiden;"></div>
	</div>
	<div id="filelist" style="height: 550px; width: 700px;overflow: auto;position:absolute;float:left;top:47px;">
		<div class="file-list" id="file_list">
		</div>
	</div>
</div>

</body>
</html>
