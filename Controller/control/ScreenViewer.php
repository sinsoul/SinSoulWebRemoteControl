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
	<title>ScreenViewer</title>
	<script type="text/javascript" src="../assets/js/jquery-1.6.4.js"></script>
</head>
<body  oncontextmenu="return false">

<div id="doc" name="doc">
    <button type="button" name="monitor_sw" id="monitor_sw" onclick="Monitor_Switch()">开始查看</button>
	<div id="textoutput"></div>
	<div id="bd">
		<div id="ScreenArea">
		<img src="" id="ScreenOutput">
		</div>
		<!--<script src="/assets/js/mxhr.js"></script>-->
		<script type="text/javascript"> 
var timer;
var click_count=0;

// $("#ScreenArea").mousemove(function(e)
// {
	// position=getClickPos(e);
	// $("#textoutput").html("坐标:X:"+position[0]+"Y:"+position[1]);
// });
//<act>4</act><x>124</x><y>3534</y>

$(document).keyup(function(e){
        var key =  e.which;
//		alert("弹起:"+key);

		addkeyboardcommand(7,key);
		if(key==8){
			return false;
		}
        }
    );
$(document).keydown(function(e){
        var key =  e.which;
//		alert("按下:"+key);
		addkeyboardcommand(6,key);
		if(key==8){
			return false;
		}
        }
    );

function addmousecommand(act,x_pos,y_pos)
{
	startobj=$.ajax({
			url: "../function/AddCommand.php",
			type: 'POST',
			data: {cmd_type:"4",cmd_content:"<act>"+act+"</act><x>"+x_pos+"</x><y>"+y_pos+"</y>"},
			success: function(cmdid){

									}
		});
}
function addkeyboardcommand(direction,key_code)
{
	startobj=$.ajax({
			url: "../function/AddCommand.php",
			type: 'POST',
			data: {cmd_type:direction,cmd_content:key_code},
			success: function(cmdid){

									}
		});
}
$("#ScreenArea").mousedown(function(e)
{
	if(3 == e.which)
	{
		position=getClickPos(e);
		//$("#textoutput").html("右键单击,坐标:X:"+position[0]+"Y:"+position[1]);
		addmousecommand(3,position[0],position[1]);
	}
	else if(1 == e.which)
	{ 
		if(click_count==1)
		{
			clearTimeout(timer);
			click_count=0;
			position=getClickPos(e);
			//$("#textoutput").html("左键双击,坐标:X:"+position[0]+"Y:"+position[1]);
			addmousecommand(2,position[0],position[1]);
			return;
		}
		click_count=1;
		timer=setTimeout(function(){
			position=getClickPos(e);
			//$("#textoutput").html("左键单击,坐标:X:"+position[0]+"Y:"+position[1]);
			addmousecommand(2,position[0],position[1]);
			click_count=0;
			},350);
	}
});
		

function getClickPos(e){
    var xPage = (navigator.appName == 'Netscape')? e.pageX : event.x+document.body.scrollLeft;
    var yPage = (navigator.appName == 'Netscape')? e.pageY : event.y+document.body.scrollTop;
    identifyImage = document.getElementById("ScreenOutput");
    img_x = locationLeft(identifyImage);
    img_y = locationTop(identifyImage);
    var xPos = xPage-img_x;
    var yPos = yPage-img_y;
	return [xPos,yPos]
}
function locationLeft(element){
    offsetTotal = element.offsetLeft;
    scrollTotal = 0;
    if (element.tagName != "BODY"){
       if (element.offsetParent != null)
          return offsetTotal+scrollTotal+locationLeft(element.offsetParent);
    }
    return offsetTotal+scrollTotal;
}
function locationTop(element){
    offsetTotal = element.offsetTop;
    scrollTotal = 0; 
    if (element.tagName != "BODY"){
       if (element.offsetParent != null)
          return offsetTotal+scrollTotal+locationTop(element.offsetParent);
    }
    return offsetTotal+scrollTotal;
}
// var sr_controller=new Array();
// var first_cmd_id;
// var first_response=0;
//var sc_id=0;
var start_switch=0;
function Monitor_Switch()
{

	if(start_switch==0)
	{
		document.getElementById("monitor_sw").innerHTML="停止查看";
		sc_id=Math.floor(Math.random()*823);
		addcommand(sc_id);
		start_switch=1
	}
	else
	{
		// for (x in sr_controller)
		// {
			// clearInterval(sr_controller[x]);
			// sr_controller[x]=null;
		// }
		document.getElementById("monitor_sw").innerHTML="开始查看";
		start_switch=0;
//			first_response=0;
	}
}
function addcommand(sc_id)
{
	var cmd_post_data="cmd_type=1&cmd_content=ScreenCapture"+sc_id;
	//alert(cmd_post_data);
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
			
			// if(first_response==0)
			// {
				// first_cmd_id=current_cmdid;
			//	 alert("请求ID"+sc_id);
				// first_response=1;
			// }
			//showresult(current_cmdid);
			getresult(current_cmdid);
			ac_xmlhttp=null;
		}
	}
	ac_xmlhttp.open("POST","../function/AddCommand.php",true);
	ac_xmlhttp.setRequestHeader("Content-type","application/x-www-form-urlencoded");
	ac_xmlhttp.send(cmd_post_data);
}
// function showresult(current_cmdid)
// {
	// sr_controller[current_cmdid-first_cmd_id]=setInterval(_getresult(current_cmdid),400);
// }
// function _getresult(current_cmdid)
// {
	// return function(){getresult(current_cmdid);}
// }
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
			if (gr_xmlhttp.responseText.length<256)
			{
			//	alert(gr_xmlhttp.responseText); 
				gr_xmlhttp=null;
				getresult(current_cmdid);
			}
			else
			{
			//	clearInterval(sr_controller[current_cmdid-first_cmd_id]);
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
// 		function getScreenshot(current_cmdid)
//		{	
//			F.mxhr.listen('image/jpeg', function(payload, payloadId) {
//				var img = document.createElement('img');
//				img.src = 'data:image/jpeg;base64,' + payload;
//				alert(img.src);
//				img.width = 1000;
//				img.height = 560;
//				document.getElementById('mxhr-output').appendChild(img);
//			});
//			F.mxhr.load('ScreenCapture.php?send_stream='+current_cmdid);
//			
//			F.mxhr=null;
//		} 
		</script>
	</div> 
	<div id="ft"></div>
</div>

</body>
</html>