//修改自LomoX的colin3dmax大侠的Js调试器，非常感谢
var prompt;
var version;
var editor;
var author;
var title;
var quit;
var cursor;
var echo;		//是否显示提示符
var historyCmd;	//存放历史记录
var sr_controller=new Array();
var first_cmd_id;
var first_response=0;
function changeCaret(){
	if($("#caret").hasClass("light")){
		$("#caret").removeClass("light");
		$("#caret").addClass("unlight");
	}else{
		$("#caret").removeClass("unlight");
		$("#caret").addClass("light");
	}
	//$("#inputBuf").focus();	//这里导致在其它框架中输入时焦点被抢占
}
function lightCaret(){
	setInterval("changeCaret()",500);
}

function createANewLine(){
	$("#caret").remove();
	$(".editBefore:first").removeClass("editBefore");
	$(".editAfter:first").removeClass("editAfter");

	if(typeof(prompt)==="function"){
		$(".console").append("<br/>"+(echo?prompt():"")+cursor);
	}else{
		$(".console").append("<br/>"+(echo?"<span class='prompt'>"+prompt+"</span>":"")+cursor);
	}
}
function printErr(info){
	print(info,"errorMessage");
}
function printMsg(info,cmd_result){
	print(info,"message",cmd_result);
}
function print(info,className,cmd_result)
{
	$(".console").append("<br/><span class=\""+className+"\" id=\""+cmd_result+"\" >"+info+"</span>");
}
function clear(){
	first_response=0;
	for (x in sr_controller)
	{
		clearInterval(sr_controller[x]);
		sr_controller[x]=null;
	}
	$(".console").empty();
	$("#inputBuf").focus();
}


function exit(){
	window.close();
}

function replace_code(textvalue)
{
	var s = "";  
	if (textvalue.length == 0) return "";
	s = textvalue.replace(/&/g, "&gt;");  
	s = s.replace(/</g, "&lt;");  
	s = s.replace(/>/g, "&gt;");  
	s = s.replace(/ /g, "&nbsp;");  
//	s = s.replace(/\'/g, "&#;");   
	s = s.replace(/\"/g, "&quot;");  
//  s = s.replace(/\n/g, "<br>");  //替换后在Chrome下不好看,不替换IE下无换行
	return s;
}

function addcommand(cmd_content)
{
	//$cmd_type = addslashes($_POST['cmd_type']);
	//$cmd_content = addslashes($_POST['cmd_content']);
	var cmd_post_data="cmd_type=0&cmd_content="+cmd_content;
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
			if(first_response==0)
			{
				first_cmd_id=current_cmdid;
				first_response=1;
			}
			//showresult(current_cmdid);
			getresult(current_cmdid)
			printMsg("正在获取结果...",("cmd_result_lable_"+(current_cmdid-first_cmd_id)));
			createANewLine();
			ac_xmlhttp=null;
		}
	}
	ac_xmlhttp.open("POST","../function/AddCommand.php",true);
	ac_xmlhttp.setRequestHeader("Content-type","application/x-www-form-urlencoded");
	ac_xmlhttp.send(cmd_post_data);
}
// function showresult(current_cmdid)
// {
	// sr_controller[current_cmdid-first_cmd_id]=setInterval(_getresult(current_cmdid),200);
// }
// function _getresult(current_cmdid)
// {
	// return function(){getresult(current_cmdid);}
// }//短轮询时留下的
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
			//document.getElementById("AddResult").innerHTML=gr_xmlhttp.responseText;
			//alert(gr_xmlhttp.responseText);
			var base64test = /[^A-Za-z0-9/+///=]/g;
			if (base64test.exec(gr_xmlhttp.responseText)||gr_xmlhttp.responseText=="TIMEOUT")
			{
				getresult(current_cmdid);
			}
			else
			{
				//alert(strAnsi2Unicode(Base64Decode(gr_xmlhttp.responseText)));
				//clearInterval(sr_controller[current_cmdid-first_cmd_id]);
				//printMsg(replace_code(strAnsi2Unicode(Base64Decode(gr_xmlhttp.responseText))));
				cmd_selector="#"+("cmd_result_lable_"+(current_cmdid-first_cmd_id));
				$(cmd_selector).html(replace_code(strAnsi2Unicode(Base64Decode(gr_xmlhttp.responseText))));
				//alert($("#consoleClient").scrollTop());
				$("#consoleClient").animate({scrollTop:$("#consoleClient").height()*1000},1000);
				gr_xmlhttp=null;
				//createANewLine();
				$("#inputBuf").focus();
				$(".editBefore:first").text($(".editBefore:first").text().substring(0,$(".editBefore:first").text().length-1));
				
			}
		}
	}
	gr_xmlhttp.open("POST","../function/GetResult.php",true);
	gr_xmlhttp.setRequestHeader("Content-type","application/x-www-form-urlencoded");
	gr_xmlhttp.send("cmd_id="+current_cmdid);
}

function doCmd(){
	var cmd=$(".editBefore:first").text()+$("#caret").text()+$(".editAfter:first").text();
//	cmd=cmd.trim();
	try{
		if($.trim(cmd)==="cls" || $.trim(cmd)==="clear")
		{
			clear();
		}
		else
		{
			if($.trim(cmd)==="")
			{
				return;
			}
			addcommand($.trim(cmd));
			$("#consoleClient").animate({scrollTop:$("#consoleClient").height()*1000},1000);
		}
	}catch(e){
		printErr(e.name+":"+e.message);
	}finally{
		historyCmd.put(cmd);
	}
}

function init(){

	$("#ClearBtn").click(function(event){
		clear();
		createANewLine();
	});
	$("#CloseBtn").click(function(event){
		exit();
	});
	
	quit=false;
	echo=true;
	prompt=">";
	version="0.0.1";
	editor="Windows Command Emulator";
	author="SinSoul";
	title=editor+" [版本 "+version+" ]<br/>版权所有(C) 2012 "+author+"。保留所有权利。"
	cursor="<span class=\"beforeCaret editBefore\"></span><span id=\"caret\">&nbsp;</span><span class=\"afterCaret editAfter\"></span>";
	historyCmd={
		id:0,
		content:new Array(),
		put:function(str){
			this.content.push(str);
			this.id=this.content.length;
		},
		forward:function(){
			if(this.id<this.content.length-1){
				this.id++;
			}
			var cmd = this.content[this.id];
			return cmd;
		},
		back:function(){
			if(this.id>0)this.id--;
			var cmd = this.content[this.id];
			return cmd;
		}
	};

	
	$(".console").append(title);
	createANewLine();
	lightCaret();
	$("#inputBuf").bind("keyup",function(event){
		if(event.keyCode===13){
			$("#inputBuf").val("");
			return;
		}
		var value = $(".editBefore:first").text()+$("#inputBuf").val();
		$(".editBefore:first").text(value);
		$("#inputBuf").val("");
		
	});
	$("#inputBuf").bind("keypress",function(event){
		event = (event)?event:((window.event)?window.event:" ")
		key = event.keyCode?event.keyCode:(event.which?ev.which:event.charCode);
		if(key===13){
			doCmd();
			var world=$("#caret").text();
			if(world!=" "){
				$(".editAfter:first").text(world+$(".editAfter:first").text());
			}
			$("#inputBuf").val("");
			return;
		}
		var value = $(".editBefore:first").text()+$("#inputBuf").val();
		$(".editBefore:first").text(value);
		$("#inputBuf").val("");
		switch(event.keyCode){
			case 8:
				$(".editBefore:first").text($(".editBefore:first").text().substring(0,$(".editBefore:first").text().length-1));
				break;
			case 37:
				if($(".editBefore:first").text().length > 0){
					$(".editAfter:first").text($("#caret").text()+$(".editAfter:first").text());	
					var world=$(".editBefore:first").text().substring($(".editBefore:first").text().length-1);
					$("#caret").text(world);
					$(".editBefore:first").text($(".editBefore:first").text().substring(0,$(".editBefore:first").text().length-1));
				}
				break;
			case 38:
				$(".editBefore:first").text(historyCmd.back());
				$(".editAfter:first").text("");
				$("#caret").text(" ");
				break;
			case 39:
				if($(".editAfter:first").text().length > 0){
					$(".editBefore:first").text($(".editBefore:first").text()+$("#caret").text());
					var world=$(".editAfter:first").text().substring(0,1);
					$("#caret").text(world);
					$(".editAfter:first").text($(".editAfter:first").text().substring(1,$(".editAfter:first").text().length));
				}
				break;
			case 40:
				if(historyCmd.id<historyCmd.content.length-1){
					$(".editBefore:first").text(historyCmd.forward());
					$(".editAfter:first").text("");
					$("#caret").text(" ");
				}
				break;
			case 46:
				if($(".editAfter:first").text().length > 0){
					var world=$(".editAfter:first").text().substring(0,1);
					$("#caret").text(world);
					$(".editAfter:first").text($(".editAfter:first").text().substring(1,$(".editAfter:first").text().length));
				}
				break;
		}
		
		
	});

	if(!$.browser.mozilla)
	$("#inputBuf").bind("keydown",function(event){
		event = (event)?event:((window.event)?window.event:" ")
		key = event.keyCode?event.keyCode:(event.which?ev.which:event.charCode);
		switch(event.keyCode){
			case 8:
				$(".editBefore:first").text($(".editBefore:first").text().substring(0,$(".editBefore:first").text().length-1));
				break;
			case 37:
				if($(".editBefore:first").text().length > 0){
					$(".editAfter:first").text($("#caret").text()+$(".editAfter:first").text());	
					var world=$(".editBefore:first").text().substring($(".editBefore:first").text().length-1);
					$("#caret").text(world);
					$(".editBefore:first").text($(".editBefore:first").text().substring(0,$(".editBefore:first").text().length-1));
				}
				break;
			case 38:
				$(".editBefore:first").text(historyCmd.back());
				$(".editAfter:first").text("");
				$("#caret").text(" ");
				break;
			case 39:
				if($(".editAfter:first").text().length > 0){
					$(".editBefore:first").text($(".editBefore:first").text()+$("#caret").text());
					var world=$(".editAfter:first").text().substring(0,1);
					$("#caret").text(world);
					$(".editAfter:first").text($(".editAfter:first").text().substring(1,$(".editAfter:first").text().length));
				}
				break;
			case 40:
				if(historyCmd.id<historyCmd.content.length-1){
					$(".editBefore:first").text(historyCmd.forward());
					$(".editAfter:first").text("");
					$("#caret").text(" ");
				}
				break;
			case 46:
				if($(".editAfter:first").text().length > 0){
					var world=$(".editAfter:first").text().substring(0,1);
					$("#caret").text(world);
					$(".editAfter:first").text($(".editAfter:first").text().substring(1,$(".editAfter:first").text().length));
				}
				break;
		}
				
	});
	$("#inputBuf").focus();
	$(".console").click(function(){
		$("#inputBuf").focus();
	});
}
$(function(){
	init();
});