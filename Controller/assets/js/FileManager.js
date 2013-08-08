$(document).ready(function(){
Array.prototype.RemoveByValue=function(value)
{
	for(var index=0;index<this.length;index++)
	{
		if(this[index].name==value.name&&
			this[index].path==value.path&&
			this[index].parentPath==value.parentPath&&
			this[index].type==value.type)
		{
			this.splice(index,1);
			index--;
		}
	}
}

$(function () {
    $("#FileTree").jstree({
        "json_data" : {
            "data" : [
                {
                    "data" : {"title" : "计算机"},
					"attr" : { "id" : "root","path":"/" },
                    //"metadata" : { "path" : "c:\\test","info":"hello"},
					"state" : "closed"
					//"id" : "root"
                    //,"children" : [ "Child 1", "A Child 2" ]
                }
            ],	
			"ajax" : {
				"type": "POST",
				"url" : "../function/GetResult.php",
				"data": function (n) {
							ajaxobj=$.ajax({
							async : false,
							"type": "POST",
							"url" : "../function/AddCommand.php",
							"data": {"cmd_type" : "8","cmd_content" : n.attr("path")}
							});
							return "cmd_id="+ajaxobj.responseText+"&decode=base64";
						}
				}		

        },
        "plugins" : [ "themes", "json_data", "ui",]
    }).bind("select_node.jstree", function (e, data) { enmu_all_file(data.rslt.obj.attr("path"),true);});

		// INSTANCES
	// 1) you can call most functions just by selecting the container and calling `.jstree("func",`
	//setTimeout(function () { $("#FileTree").jstree("set_focus"); }, 500);
	// with the methods below you can call even private functions (prefixed with `_`)
	// 2) you can get the focused instance using `$.jstree._focused()`. 
	//setTimeout(function () { $.jstree._focused().select_node("#root"); }, 1000);
	// 3) you can use $.jstree._reference - just pass the container, a node inside it, or a selector
	//setTimeout(function () { $.jstree._reference("#root").close_node("#root"); }, 1500);
	// 4) when you are working with an event you can use a shortcut
	//$("#FileTree").bind("open_node.jstree", function (e, data) {
		// data.inst is the instance which triggered this event
		//data.inst.select_node("#phtml_2", true);
	//});
	setTimeout(function () { $.jstree._reference("#root").open_node("#root"); }, 200);
});


//$("#file_list").append("<div class=\"file-grid filelist-item ui-draggable ui-droppable\" index=\"2\" title=\"Name:baidu 03/21/2013 22:08\" file_name=\"baidu\" style=\"\"><div class=\"img-wrap\"><img src=\"../assets/img/ui/grid_dir.png\"></div><div class=\"file-name over-ellipsis\">baidu</div></div>");
function ShowDlg(dlgName,dlgLable,idYesBtn,promptNoBtn,oldVlue){
	var inputBox;
	if(oldVlue)
	{
		inputBox='<input type="text" id="prompInput" maxlenght="50" value="'+(oldVlue)+'">';
	}
	else
	{
		inputBox="</br>";
	}
	$("[class='file-body']").append(
		'<div id="confirm-overlay" class="simplemodal-overlay" style="opacity: 0.5; height: 100%; width: 100%; position: fixed; left: 0px; top: 0px; z-index: 1001;"></div><div id="confirm-container" class="simplemodal-container" style="position: fixed; z-index: 1002; height: 190px; width: 402px; left: 252px; top: 30%;"><a class="modalCloseImg simplemodal-close" title="Close"></a><div tabindex="-1" class="simplemodal-wrap" style="height: 100%; outline: 0px; width: 100%; overflow: visible;"><div id="confirmMsg" style="position: relative; left: 24px; top: -2px;" class="ui-draggable simplemodal-data"><h4 id="confirm_title">'
		+dlgName+
		'</h4><p id="msgZone" style="padding: 30px;"><span class="mr16">'
		+dlgLable+
		'</span>'+inputBox+'<span class="info"></span></p><div class="ui-actions is-border-boxing"><button type="button" class="btn btn-primary" id="'
		+idYesBtn+
		'">OK</button><button type="button" class="btn ml16" id="'
		+promptNoBtn+
		'">Cancel</button></div></div></div></div>');
}
$("[class='file-grid filelist-item ui-draggable ui-droppable']").live("click",function(){
	enmu_all_file($(this).attr("path"),true);
	});
	
$("[class='file-grid filelist-item ui-draggable']").live("click",function(){
	alert("要操作的文件:"+$(this).attr("path"));
	
	});

var SelectedFile=new Array();

$("[class='file-grid filelist-item ui-draggable ui-droppable']").live("contextmenu",function(){
		//文件夹的右键选定操作
		SelectedFile.push({"name":$(this).attr("title"),"path":$(this).attr("path"),"parentPath":$(this).attr("parent_dir"),"type":$(this).attr("type")});
		$("#renameBtn").removeClass("disabled");
		$("#cutBtn").removeClass("disabled");
		$("#copyBtn").removeClass("disabled");	
		$(this).addClass("ui-selected");
		return false;
	});

$("[class='file-grid filelist-item ui-draggable']").live("contextmenu",function(){
		//文件的右键选定操作
		
		SelectedFile.push({"name":$(this).attr("title"),"path":$(this).attr("path"),"parentPath":$(this).attr("parent_dir"),"type":$(this).attr("type")});
		$("#renameBtn").removeClass("disabled");
		$("#cutBtn").removeClass("disabled");
		$("#copyBtn").removeClass("disabled");
		$(this).addClass("ui-selected");
		return false;
	});

$("[class='file-grid filelist-item ui-draggable ui-droppable ui-selected']").live("contextmenu",function(){
		//文件夹取消选定
		SelectedFile.RemoveByValue({"name":$(this).attr("title"),"path":$(this).attr("path"),"parentPath":$(this).attr("parent_dir"),"type":$(this).attr("type")});
		if(SelectedFile.length==0)
		{
			$("#renameBtn").addClass("disabled");
			$("#cutBtn").addClass("disabled");
			$("#copyBtn").addClass("disabled");
		}
		$(this).removeClass("ui-selected");
		return false;
	});
	
$("[class='file-grid filelist-item ui-draggable ui-selected']").live("contextmenu",function(){
		//文件取消选定
		SelectedFile.RemoveByValue({"name":$(this).attr("title"),"path":$(this).attr("path"),"parentPath":$(this).attr("parent_dir"),"type":$(this).attr("type")});
		if(SelectedFile.length==0)
		{
			$("#renameBtn").addClass("disabled");
			$("#cutBtn").addClass("disabled");
			$("#copyBtn").addClass("disabled");
		}
		$(this).removeClass("ui-selected");
		return false;
	});

function FileOperator(cmd_content)
{
	$.ajax({
	"type": "POST",
	"url" : "../function/AddCommand.php",
	"data": {"cmd_type" : "11","cmd_content" : cmd_content},
	"success" : function(cmdid){
		$.ajax({
				//async : false,
				"type": "POST",
				"url" : "../function/GetResult.php",
				"data": {"cmd_id" : cmdid},
				"success": function(result)
				{
					if(Base64Decode(result)=="Complete")
					{
						if($("#address_input").attr("value")!="")
						{
							enmu_all_file($("#address_input").attr("value"),true);
						}
					}
					else
					{
						alert(strAnsi2Unicode(Base64Decode(result)));
						enmu_all_file($("#address_input").attr("value"),true);
					}
				}
			});
		}
	});
	$("#confirm-overlay").detach();
	$("#confirm-container").detach();
}

$("#newFolderBtn").click(function(){
	if($("#address_input").attr("value")=="")
	{
		alert("当前路径无法创建文件夹");
		return;
	}
	var current_dir=$("#address_input").attr("value");
	
	if(current_dir[current_dir.length-1]!="\\")
	{
		current_dir=$("#address_input").attr("value")+"\\";
	}

	ShowDlg("新建文件夹","文件夹名称：","nf_dlg_yes","nf_dlg_no","新建文件夹");
	
	$("#nf_dlg_yes").bind("click",function(){
		var cmd_content='{"operate":"newdir","newDirPath":"'
						+Base64Encode(current_dir+$("#prompInput").attr("value"))+
						'"}';	
		FileOperator(cmd_content);
	});
	
	$("#nf_dlg_no").bind("click",function(){
		$("#confirm-overlay").detach();
		$("#confirm-container").detach();
			
	});
});

$("#deleteBtn").click(function(){
	if(SelectedFile.length==0)
	{
		alert("未选中任何文件");
		return;
	}
	ShowDlg("确定删除？","删除后文件无法恢复，确定删除？","del_dlg_yes","del_dlg_no",null);
	
	$("#del_dlg_yes").bind("click",function(){
		var files=new Array();
		var dirs=new Array();
		for(var i=0;i<SelectedFile.length;i++)
		{
			if((SelectedFile[i].type)=="file")
			{
				files.push('{"file":"'+Base64Encode((SelectedFile[i].path))+'"}');
			}
			else
			{
				dirs.push('{"dir":"'+Base64Encode((SelectedFile[i].path))+'"}')
			}
		}
	
		var cmd_content='{"operate":"delete","filelist":['+files.toString()+'],"dirlist":['+dirs.toString()+']}';
	//	document.write(cmd_content);								
		FileOperator(cmd_content);
	});
	
	$("#del_dlg_no").bind("click",function(){
		$("#confirm-overlay").detach();
		$("#confirm-container").detach();
		
	});
	
	
});
$("#renameBtn").click(function(){
	if($(this).hasClass("disabled"))
	{
		return;
	}
	if(SelectedFile.length>1)
	{
		alert("重命名操作只能选定一个文件夹");
		return;
	}

	ShowDlg("重命名","新名称","rnm_dlg_yes","rnm_dlg_no",SelectedFile[0].name);
	
	$("#rnm_dlg_yes").bind("click",function(){
		var cmd_content='{"operate":"rename","oldName":"'
						+Base64Encode((SelectedFile[0].path))+
						'","newName":"'
						+Base64Encode((SelectedFile[0].parentPath)+$("#prompInput").attr("value"))+
						'"}';				
		FileOperator(cmd_content);
	});
	
	$("#rnm_dlg_no").bind("click",function(){
		$("#confirm-overlay").detach();
		$("#confirm-container").detach();
		
	});
});

var CopyorCut;
var PasteData=new Array;
$("#copyBtn").click(function(){
	if($(this).hasClass("disabled"))
	{
		return;
	}
	if(SelectedFile.length==0)
	{
		alert("未选中任何文件");
		return;
	}
	CopyorCut="Copy";
	$("#pasteBtn").removeClass("disabled");
	PasteData.splice(0,PasteData.length);
	PasteData=SelectedFile.concat();
});

$("#cutBtn").click(function(){
	if($(this).hasClass("disabled"))
	{
		return;
	}
	if(SelectedFile.length==0)
	{
		alert("未选中任何文件");
		return;
	}
	CopyorCut="Cut";
	$("#pasteBtn").removeClass("disabled");
	PasteData.splice(0,PasteData.length);
	PasteData=SelectedFile.concat();
});

$("#pasteBtn").click(function(){
	if($(this).hasClass("disabled"))
	{
		return;
	}
	if($("#address_input").attr("value")=="")
	{
		alert("不能粘贴到此位置");
	}
	var current_dir=$("#address_input").attr("value");
	
	if(current_dir[current_dir.length-1]!="\\")
	{
		current_dir=$("#address_input").attr("value")+"\\";
	}
	var src=new Array();
	var dest=new Array();
	for(var i=0;i<PasteData.length;i++)
	{
		src.push('{"src":"'+Base64Encode((PasteData[i].path))+'"}');
		dest.push('{"dest":"'+Base64Encode(current_dir+(PasteData[i].name))+'"}');
	}
	var cmd_content='{"operate":"'+CopyorCut+
					'","Source":['+src.toString()+
					'],"Destination":['+dest.toString()+']}';
	if(CopyorCut=="Cut")
	{
		$("#pasteBtn").addClass("disabled");
	}
	//document.write(cmd_content);								
	FileOperator(cmd_content);
	
});


$("#file_list").empty();
var back_stack=new Array();
var forward_stack=new Array();
var first_back=false;

$("#toolbar_back").bind("click",function(){
	if($("#toolbar_back").hasClass("btn_back_disable"))
	{
		return;
	}
	var back_dir=back_stack.pop();
	if($("#address_input").attr("value")==back_dir&&first_back)
	{
		forward_stack.push(back_dir);
		first_back=false;
		back_dir=back_stack.pop();
	}
	forward_stack.push(back_dir);
	if(forward_stack.length>0)
	{
		$("#toolbar_forward").removeClass("btn_forward_disable");
	}
	if(back_stack.length==0)
	{
		$("#toolbar_back").addClass("btn_back_disable");	
		enmu_all_file(back_dir,true);
		return;
	}
	enmu_all_file(back_dir,true);
	
});
$("#toolbar_forward").click(function(){
	if($("#toolbar_forward").hasClass("btn_forward_disable"))
	{
		return;
	}
	var forward_dir=forward_stack.pop();
	if($("#address_input").attr("value")==forward_dir)
	{
		back_stack.push(forward_dir);
		forward_dir=forward_stack.pop();
	}
	if(forward_stack.length<1)
	{
		$("#toolbar_forward").addClass("btn_forward_disable");
	}
	enmu_all_file(forward_dir,true);
});
$("#Enter").click(function(){
	if($("#address_input").attr("value")!="")
	{
		enmu_all_file($("#address_input").attr("value"),true);
	}
})
$("#refreshFileBtn").click(function(){
	if($("#address_input").attr("value")!="")
	{
		enmu_all_file($("#address_input").attr("value"),true);
	}
});
function show_loading()
{
//	$("#file_list").append('<div style="overflow:hiden; height: 128px;width:128px;position:absolute;left:45%;top:45%"><img src="../assets/img/ui/loading.gif"></div>');
//	$("#file_list").append('<div id="facebookG"><div id="blockG_1" class="facebook_blockG"></div><div id="blockG_2" class="facebook_blockG"></div><div id="blockG_3" class="facebook_blockG"></div><div id="blockG_3" class="facebook_blockG"></div><div id="blockG_3" class="facebook_blockG"></div></div>');
	$("#file_list").append('<div style="overflow:hiden; height: 100px;width:300px;position:absolute;left:40%;top:40%"><div id="facebookG"><div id="blockG_1" class="facebook_blockG"></div><div id="blockG_2" class="facebook_blockG"></div><div id="blockG_3" class="facebook_blockG"></div></div></div>');
}
function enmu_all_file(dir,record)
{
	if(dir=="/")
	{
		return;
	}
	SelectedFile.splice(0,SelectedFile.length);
	$("#renameBtn").addClass("disabled");
	$("#cutBtn").addClass("disabled");
	$("#copyBtn").addClass("disabled");
	$("#file_list").empty();
	show_loading();
	$.ajax({
			"type": "POST",
			"url" : "../function/AddCommand.php",
			"data": {"cmd_type" : "9","cmd_content" : dir},
			"success" : function(cmdid){
				$.ajax({
						//async : false,
						"type": "POST",
						"url" : "../function/GetResult.php",
						"data": {"cmd_id" : cmdid, "decode" : "base64"},
						"success": function(result)
						{
							document.getElementById("address_input").value=dir;
						//	if($("#address_input").attr("value")!=back_stack[back_stack.length-1])
						//	{
						//		back_stack.push($("#address_input").attr("value"));
						//	}
							if(record==true)
							{
								back_stack.push(dir);
								first_back=true;
								if(back_stack.length>1&&$("#toolbar_back").hasClass("btn_back_disable"))
								{
										$("#toolbar_back").removeClass("btn_back_disable");
								}
							}
							
							$("#file_list").empty();
							if(result==null)
							{
								$("#file_list").append('<div style="overflow:hiden; height: 128px;width:128px;position:absolute;left:45%;top:45%">空目录</div>');
							}
							var json = eval(result.dir);
							
							for(var i=0; i<json.length; i++)
							{
								if(json[i].icon=="grid_dir.png")
								{
									if(dir[dir.length-1]=="\\")
									{
										$("#file_list").append('<div class="file-grid filelist-item ui-draggable ui-droppable" title="'
											+json[i].title+'"path="'+dir+json[i].name+
											'" parent_dir="'+dir+
											'" type="dir"><div class="img-wrap"><img src="../assets/img/ui/'
											+json[i].icon+'"></div><div class="file-name over-ellipsis">'
											+json[i].name+'</div></div>');
									}
									else
									{
										$("#file_list").append('<div class="file-grid filelist-item ui-draggable ui-droppable" title="'
											+json[i].title+'"path="'+dir+'\\'+json[i].name+
											'" parent_dir="'+dir+
											'\\" type="dir"><div class="img-wrap"><img src="../assets/img/ui/'
											+json[i].icon+'"></div><div class="file-name over-ellipsis">'
											+json[i].name+'</div></div>');
									}
								}
								else
								{
									if(dir[dir.length-1]=="\\")
									{
										$("#file_list").append('<div class="file-grid filelist-item ui-draggable" title="'
											+json[i].title+'"path="'+dir+json[i].name+
											'" parent_dir="'+dir+
											'" type="file"><div class="img-wrap"><img src="../assets/img/ui/'
											+json[i].icon+'"></div><div class="file-name over-ellipsis">'
											+json[i].name+'</div></div>');
									}
									else
									{
										$("#file_list").append('<div class="file-grid filelist-item ui-draggable" title="'
											+json[i].title+'"path="'+dir+'\\'+json[i].name+
											'" parent_dir="'+dir+
											'\\" type="file"><div class="img-wrap"><img src="../assets/img/ui/'
											+json[i].icon+'"></div><div class="file-name over-ellipsis">'
											+json[i].name+'</div></div>');
									}
								}
							}
						}
						});
			}
			});
}});