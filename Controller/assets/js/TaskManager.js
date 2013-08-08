$(document).ready(function(){
Array.prototype.RemoveByValue=function(value)
{
	for(var index=0;index<this.length;index++)
	{
		if(this[index].name==value.name&&
			this[index].path==value.path&&
			this[index].pid==value.pid)
		{
			this.splice(index,1);
			index--;
		}
	}
}

function show_loading()
{
	$("#process-list").append('<div id="loading" style="overflow:hiden; height: 100px;width:300px;position:absolute;left:40%;top:40%"><div id="facebookG"><div id="blockG_1" class="facebook_blockG"></div><div id="blockG_2" class="facebook_blockG"></div><div id="blockG_3" class="facebook_blockG"></div></div></div>');
}

function ShowDlg(dlgName,dlgLable,idYesBtn,promptNoBtn,oldVlue){
	var inputBox;
	if(oldVlue)
	{
		inputBox='<input type="text" id="prompInput" maxlenght="512" value="'+(oldVlue)+'">';
	}
	else
	{
		inputBox="</br>";
	}
	$("#task_grid").append(
		'<div id="confirm-overlay" class="simplemodal-overlay" style="opacity: 0.5; height: 100%; width: 100%; position: fixed; left: 0px; top: 0px; z-index: 1001;"></div><div id="confirm-container" class="simplemodal-container" style="position: fixed; z-index: 1002; height: 190px; width: 402px; left: 180px; top: 30%;"><a class="modalCloseImg simplemodal-close" title="Close"></a><div tabindex="-1" class="simplemodal-wrap" style="height: 100%; outline: 0px; width: 100%; overflow: visible;"><div id="confirmMsg" style="position: relative; left: 24px; top: -2px;" class="ui-draggable simplemodal-data"><h4 id="confirm_title">'
		+dlgName+
		'</h4><p id="msgZone" style="padding: 30px;"><span class="mr16">'
		+dlgLable+
		'</span>'+inputBox+'<span class="info"></span></p><div class="ui-actions is-border-boxing"><button type="button" class="btn btn-primary" id="'
		+idYesBtn+
		'">OK</button><button type="button" class="btn ml16" id="'
		+promptNoBtn+
		'">Cancel</button></div></div></div></div>');
}

var SelectedTask=new Array();

function TaskOperator(cmd_content)
{
	$.ajax({
	"type": "POST",
	"url" : "../function/AddCommand.php",
	"data": {"cmd_type" : "12","cmd_content" : cmd_content},
	"success" : function(cmdid){
		$.ajax({
				//async : false,
				"type": "POST",
				"url" : "../function/GetResult.php",
				"data": {"cmd_id" : cmdid},
				"success": function(result)
				{
					if(Base64Decode(result)=="Success")
					{
						RefreshTaskList();
					}
					else
					{
						alert(strAnsi2Unicode(Base64Decode(result)));
						RefreshTaskList();
					}
					
				}
			});
		}
	});
	$("#confirm-overlay").detach();
	$("#confirm-container").detach();
}

$("#new").click(function(){
	ShowDlg("运行","打开：","run_dlg_yes","run_dlg_no","程序路径");
	$("#run_dlg_yes").bind("click",function(){
		if($("#prompInput").attr("value")=="程序路径"||$("#prompInput").attr("value")=="")
		{
			$('[class="info"]').html("请输入程序名称或路径！");
			return;
		}
		var cmd_content='{"operate":"run","path":"'+Base64Encode($("#prompInput").attr("value"))+'"}';
		TaskOperator(cmd_content);
		
	});
	
	$("#prompInput").bind("click",function(){
		if($("#prompInput").attr("value")=="程序路径")
		{
			$("#prompInput").attr("value","");
		}
		
	});
	$("#run_dlg_no").bind("click",function(){
		$("#confirm-overlay").detach();
		$("#confirm-container").detach();
			
	});
});

$("#msg_dlg_yes").live("click",function(){
		$("#confirm-overlay").detach();
		$("#confirm-container").detach();
			
});
$("#msg_dlg_no").live("click",function(){
		$("#confirm-overlay").detach();
		$("#confirm-container").detach();
			
});

$("#terminal").click(function(){
	if(SelectedTask.length==0)
	{
		ShowDlg("未选中任何进程","请先选择你想要结束的进程，再执行此操作","msg_dlg_yes","msg_dlg_no","");
		return;
	}
	var killlist=new Array();
	for(var i=0;i<SelectedTask.length;i++)
	{
		killlist.push('{"pid":'+SelectedTask[i].pid+',"name":"'+SelectedTask[i].name+'"}');
	}
	var cmd_content='{"operate":"terminal","killlist":['+killlist.toString()+']}';
	TaskOperator(cmd_content);
	//document.write(cmd_content);
});

$("#refresh").click(function(){
	RefreshTaskList();
});

function RefreshTaskList(){
	$("#task-list").empty();
	SelectedTask.splice(0,SelectedTask.length);
	$.ajax({
			"type": "POST",
			"url" : "../function/AddCommand.php",
			"data": {"cmd_type" : "10","cmd_content" : "tasklist"+Math.floor(Math.random()*823)},
			"success" : function(cmdid){
			
				show_loading();	
				
				$.ajax({
						//async : false,
						"type": "POST",
						"url" : "../function/GetResult.php",
						"data": {"cmd_id" : cmdid, "decode" : "base64"},
						"success": function(result)
						{
							//$("#task-list").empty();
							$("#loading").detach();
							var json = eval(result.task);
							for(var i=0; i<json.length; i++)
							{
								$("#task-list").append('<tr class="process-item" pid="'+json[i].pid+'" name="'+json[i].name+'" path="'
										+json[i].path+'"><td class="select"><div class="checkbox" check="0"></div></td><td class="name clickable"><div class="text" style="width:180px">'
										+json[i].name+'</div></td><td class="pid clickable"><div class="text" style="width:70px">'
										+json[i].pid+'</div></td><td class="user clickable"><div class="text">'
										+json[i].user+'</div></td><td class="mem clickable"><div class="text">'
										+json[i].mem+'</div></td><td class="path clickable"><div class="text" title="'
										+json[i].path+'">'+json[i].path+'</div></td></tr>');
							}
							
							
						}
					});
			}
			});
}

RefreshTaskList();
$("[class='process-item']").live("click",function(){
	//alert($(this).attr("pid"));
	});
	
$("[class='checkbox']").live("click",function(){
	$(this).attr("check","1");
	$(this).addClass("checked");
	SelectedTask.push({"pid":$(this).parent().parent().attr("pid"),
						"name":$(this).parent().parent().attr("name"),
						"path":$(this).parent().parent().attr("path")});
});	

$("[class='checkbox checked']").live("click",function(){
	$(this).attr("check","0");
	$(this).removeClass("checked");
	SelectedTask.RemoveByValue({"pid":$(this).parent().parent().attr("pid"),
								"name":$(this).parent().parent().attr("name"),
								"path":$(this).parent().parent().attr("path")});
});
});