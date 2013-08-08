var formswitch=0;
var ieformswitch=0;

$(function(){
	
	// Checking for CSS 3D transformation support
	$.support.css3d = supportsCSS3D();
	
	var formContainer = $('#formContainer');
	
	// Listening for clicks on the ribbon links
	$('.flipLink').click(function(e)
	{
	//alert(navigator.userAgent);
		if($.support.css3d)
		{
			formContainer.toggleClass('flipped');
			if(formswitch==0)
			{
				$('#login').fadeOut('fast');
				formswitch=1;
			}
			else
			{
				$('#login').fadeIn('500');
				formswitch=0;
			}
		}
		else
		{
			formContainer.toggleClass('flipped');
			if(ieformswitch==0)
			{
				$('#login').fadeOut('fast');
			//	$('#login').hide('fast');
			//	$('#Register').show('fast');
				ieformswitch=1;
			}
			else
			{
				$('#login').fadeIn('500');
			//	$('#Register').hide('fast');
			//	$('#login').show('fast');
				ieformswitch=0;
			}
		//	$('#login').toggle(1000);
		//	$('#Register').toggle(1000);
		}
		e.preventDefault();
	});
	
	formContainer.find('form').submit(function(e){
		// Preventing form submissions. If you implement
		// a backend, you might want to remove this code
		e.preventDefault();
	});
	
	
	// A helper function that checks for the 
	// support of the 3D CSS3 transformations.
	function supportsCSS3D() {
		var props = [
			'perspectiveProperty', 'WebkitPerspective', 'MozPerspective'
		], testDom = document.createElement('a');
		  
		for(var i=0; i<props.length; i++){
			if(props[i] in testDom.style){
				return true;
			}
		}
		
		return false;
	}
});
var jmptime=2;
var mytime;
function jmpvd()
{
	document.getElementById("logintips").innerHTML="登录成功,"+jmptime+"秒后跳转";
	if(--jmptime==0)
	{
		window.clearInterval(mytime);
		window.location.href="control/GetUserHost.php";
	}
}
function login()
{
	var loginuser=document.getElementById("UserName").value;
	var loginpass=document.getElementById("loginPass").value;
	if(loginuser=="UserName"||loginuser=="")
	{
		alert("请输入用户名");
		return;
	}
	if(loginpass=="pass"||loginpass=="")
	{
		alert("请输入密码");
		return;
	}
	var logxmlhttp=false;
	logxmlhttp=false;
	if (window.XMLHttpRequest)
	{
		logxmlhttp=new XMLHttpRequest();
	}
	else
	{
		logxmlhttp=new ActiveXObject("Microsoft.XMLHTTP");
	}
	logxmlhttp.onreadystatechange=function()
	{
		if (logxmlhttp.readyState==4 && logxmlhttp.status==200)
		{
			if(logxmlhttp.responseText=="true")
			{
				document.getElementById("logintips").innerHTML="登录成功,3秒后跳转";
				mytime=window.setInterval("jmpvd()",1000);
			}
			else
			{
				document.getElementById("logintips").innerHTML="用户名或密码错误";
			}						
		}
	}
	logxmlhttp.open("POST","function/VerifyAccount.php",true);
	logxmlhttp.setRequestHeader("Content-type","application/x-www-form-urlencoded");
	logxmlhttp.send("loginuser="+loginuser+"&loginpass="+loginpass);	
}

function Register()
{
	var reguser=document.getElementById("RegisterUser").value;
	var regpass=document.getElementById("RegisterPass").value;
	var regrepass=document.getElementById("RegisterRePass").value;
	var regmail=document.getElementById("RegisterEmail").value;
	if(reguser=="UserName"||reguser=="")
	{
		alert("请输入用户名");
		return;
	}
	if(regpass!=regrepass||regpass=="pass"||regpass=="")
	{
		alert("输入密码不配或未输入");
		return;
	}
	if(regmail=="Email"||regmail=="")
	{
		alert("请输入你的常用邮箱");
		return; 
	}
	else if(regmail.search(/^([a-zA-Z0-9]+[_|\_|\.]?)*[a-zA-Z0-9]+@([a-zA-Z0-9]+[_|\_|\.]?)*[a-zA-Z0-9]+\.[a-zA-Z]{2,3}$/)== -1)
	{
		alert("邮箱不合法");
		document.getElementById("RegisterEmail").focus();
		return;
	}
	var regxmlhttp;
	if (window.XMLHttpRequest)
	{
		regxmlhttp=new XMLHttpRequest();
	}
	else
	{
		regxmlhttp=new ActiveXObject("Microsoft.XMLHTTP");
	}
	regxmlhttp.onreadystatechange=function()
	{
		if (regxmlhttp.readyState==4 && regxmlhttp.status==200)
		{
			alert(regxmlhttp.responseText);
		}
	}
	regxmlhttp.open("POST","function/Register.php",true);
	regxmlhttp.setRequestHeader("Content-type","application/x-www-form-urlencoded");
	regxmlhttp.send("reguser="+reguser+"&regpass="+regpass+"&regmail="+regmail);
}			
function CleanText(id)
{
	var idvalue=document.getElementById(id).value;
	if(idvalue=="UserName"||idvalue=="pass"||idvalue=="Email")
	{
		document.getElementById(id).value="";
	}
}
function RecoveryTips(id)
{
	var idvalue=document.getElementById(id).value;
	var idname=document.getElementById(id).name;
	if(idvalue=="")
	{
		switch(idname)
		{
			case "UserName":
			case "RegisterUser":document.getElementById(id).value="UserName";break;
			case "RegisterPass":
			case "RegisterRePass":
			case "loginPass":document.getElementById(id).value="pass";break;
			case "RegisterEmail":document.getElementById(id).value="Email";break;
		}
	}
}