#include "MyStdAfx.h"
#pragma warning(disable:4995)
#pragma warning(disable:4244)

char userName[20]="0";
char hostid[20]="0";
hostinfo pHI={"0","0"};
char online_post_data[1024]="\0";
//char getcmd_post_data[1024]="\0";
char cookie_data[1024]="\0";
bool set_cookie=false;
//char server_addr[50]="127.0.0.1";
//char server_addr[255]="192.168.0.1";
char server_addr[255]="sswrc.sinaapp.com";

bool AnalyzeCommand(string cmd,cmdinfo *ci)
{
	//<cmd_id>1</cmd_id>
	//<cmd_type>0</cmd_type>
	//<cmd_time>2012-03-21 14:42:35</cmd_time>
	//<cmd_content>ipconfig /all</cmd_content>

	Msg((char *)cmd.c_str());
	Msg("\r\n");
	ci->cmd_content.erase();
	int startPos=-1;
	int endPos=-1;
	startPos=cmd.find("<cmd_id>",0);
	if (startPos==string::npos)
	{
		Msg("命令格式错误，未发现命令ID，收到的内容:%s\r\n",ci->cmd_content.c_str());
		return false;
	}
	startPos+=8;//strlen("<cmd_id>");
	endPos=cmd.find("</cmd_id>",startPos);
	char *cmdid=new char[endPos-startPos+2];
	ZeroMemory(cmdid,endPos-startPos+2);
	memcpy(cmdid,&cmd[startPos],endPos-startPos);
	if (strlen(cmdid)>22)
	{
		Msg("错误的命令ID\r\n");
		return false;
	}
	ZeroMemory(ci->cmd_id,20);
	memcpy(ci->cmd_id,cmdid,strlen(cmdid));

	startPos=cmd.find("<cmd_type>",endPos);
	if (startPos==string::npos)
	{
		Msg("命令类型格式错误\r\n");
		return false;
	}
	startPos+=10;
	endPos=cmd.find("</cmd_type>",startPos);
	char *cmdtype=new char[endPos-startPos+2];
	ZeroMemory(cmdtype,endPos-startPos+2);
	memcpy(cmdtype,&cmd[startPos],endPos-startPos);
	if (strlen(cmdtype)>4)
	{
		Msg("接收到错误的命令类型\r\n");
		return false;
	}
	ZeroMemory(ci->cmd_type,4);
	memcpy(ci->cmd_type,cmdtype,strlen(cmdtype));

	startPos=cmd.find("<cmd_content>",endPos);
	if (startPos==string::npos)
	{
		Msg("命令内容格式错误\r\n");
		return false;
	}
	startPos+=13;//strlen("<cmd_content>");
	endPos=cmd.find("</cmd_content>",startPos);
	char *cmdcontent=new char[endPos-startPos+2];
	ZeroMemory(cmdcontent,endPos-startPos+2);
	memcpy(cmdcontent,&cmd[startPos],endPos-startPos);

	ci->cmd_content.erase();
	ci->cmd_content+=cmdcontent;

	delete []cmdid;
	delete []cmdtype;
	delete []cmdcontent;
	return true;
}

void ReceiveCommand(void *temp)
{ 
	//	WPARAM wp=(WPARAM)szBuff;
	//	LPARAM lp=(LPARAM)TEXT("type c:\\real.pac");
	//	SendMessage(hwnd,WM_CMDRCVD_MESSAGE,wp,lp);
	//	MessageBox(hwnd,szBuff,0,0);
	//	RunCmd(U2A(szBuff));
	//	char* tmpCmd=new char[length];
	//	wcstombs_s(tmpCmd,szBuff,length);
	int nCL=0;
	string command;
	string cmd_false ("False\r\n\r\n");
	string cmd_SAE_false ("False\r\n\r\n\r\n0\r\n\r\n");

//	sprintf_s(getcmd_post_data,"user_name=%s&rc_host_id=%s",userName,hostid);
	char tips[30]="0";
	int tip=1;
	while(conn_status)
	{
		cmdinfo *s_cmdinfo=new cmdinfo;
		HttpMessenger *hm_cmd_receiver=new HttpMessenger(server_addr);
		if (!hm_cmd_receiver->CreateConnection())
		{
			TrayIcon.ShowBalloon("未能成功连接到命令服务器，30秒后重试。","网络错误",0,5);
			Msg("接收命令时出错:%s，10秒后重试\r\n",hm_cmd_receiver->GetErrorLog().c_str());
			Sleep(30000);
			delete[] hm_cmd_receiver;
			continue;
		}
		else
		{
//			Msg("连接命令服务器成功\r\n");
		}
	//	hm_cmd_receiver->CreateAndSendRequest("POST","/function/GetCommand.php",server_addr,getcmd_post_data,false,NULL);
		hm_cmd_receiver->CreateAndSendRequest("GET","/function/GetCommand.php",server_addr);
		//当请求次数过多时SAE会返回一个跳转页面，以验证访问者是否是浏览器，这里用于模仿浏览器进行跳转。
		AnalyzeJumpPage(hm_cmd_receiver);
//		Msg("所请求的内容:%s\r\n",hm_cmd_receiver->m_ResponseText.c_str());
		if ((hm_cmd_receiver->m_ResponseText.find("TIMEOUT",0)!=string::npos)
				||(hm_cmd_receiver->m_ResponseText.compare(cmd_false)==0)
				||(hm_cmd_receiver->m_ResponseText.compare(cmd_SAE_false)==0))
		{
//			Msg("\r\n获取命令出错或超时返回");
		}
		else
		{
			if (AnalyzeCommand(hm_cmd_receiver->m_ResponseText,s_cmdinfo))
			{
				SendMessage(hwnd,WM_CMDRCVD_MESSAGE,(WPARAM)s_cmdinfo,(LPARAM)0);
			}
			else
			{
				Msg("分析命令出错\r\n");
			}	
		}
		delete[] hm_cmd_receiver;
	}
}
 
void AnalyzeJumpPage(HttpMessenger *hm_cmd_receiver)
{
	if (hm_cmd_receiver->m_ResponseText.find("<html>")==0)
	{
		Msg("可能收到跳转页面.开始解析.\r\n");
		char jmp_host[255];
		char jmp_port[10];
		char jmp_resource[1024];
		int start_pos=-1,end_pos=-1;
		string ResponseText=hm_cmd_receiver->m_ResponseText;
		ZeroMemory(jmp_resource,1024);
		ZeroMemory(jmp_port,10);
		ZeroMemory(jmp_host,255);
		start_pos=ResponseText.find("url=http://");
		if (start_pos!=string::npos)
		{
			start_pos+=11;
			end_pos=ResponseText.find("\"",start_pos);
			ResponseText.erase(end_pos,ResponseText.length());
			end_pos=ResponseText.find(":",start_pos);
			if (end_pos!=string::npos)
			{
				//如果找到冒号表示包含端口号
				memcpy(jmp_host,ResponseText.c_str()+start_pos,end_pos-start_pos);
				start_pos=end_pos+1;
				end_pos=ResponseText.find("/",start_pos);
				if (end_pos==string::npos)
				{
					end_pos=ResponseText.length();
					memcpy(jmp_port,ResponseText.c_str()+start_pos,end_pos-start_pos);
					memcpy(jmp_resource,"/",1);
				}
				else
				{
					memcpy(jmp_port,ResponseText.c_str()+start_pos,end_pos-start_pos);
					start_pos=end_pos;
					end_pos=ResponseText.length();
					memcpy(jmp_resource,ResponseText.c_str()+start_pos,end_pos-start_pos);
				}
			}
			else
			{
				memcpy(jmp_port,"80",2);
				//查找主机地址尾部
				end_pos=ResponseText.find("/",start_pos);
				if (end_pos!=string::npos)
				{
					if (end_pos-start_pos<255)
					{
						memcpy(jmp_host,ResponseText.c_str()+start_pos,end_pos-start_pos);
						start_pos=end_pos;
						end_pos=ResponseText.length();
						memcpy(jmp_resource,ResponseText.c_str()+start_pos,end_pos-start_pos);
					}
				}
				else
				{
					//没有找到资源
					end_pos=ResponseText.length();
					memcpy(jmp_host,ResponseText.c_str()+start_pos,end_pos-start_pos);
					memcpy(jmp_resource,"/",1);
				}
			}
			Msg("跳转主机:%s\r\n跳转端口:%s\r\n跳转资源:%s\r\n",jmp_host,jmp_port,jmp_resource);
			HttpMessenger hm_jmp(jmp_host,atoi(jmp_port));
			hm_jmp.CreateConnection();
			hm_jmp.CreateAndSendRequest("GET",jmp_resource,jmp_host,0,false,0);
			AnalyzeJumpPage(&hm_jmp);
			hm_cmd_receiver->m_ResponseText.erase();
			hm_cmd_receiver->m_ResponseText=hm_jmp.m_ResponseText;
		}
		else
		{
			Msg("分析跳转页面出错\r\n");
		}
	}
}

void RunCmd(void *cmd)
{
	cmdinfo *pci=(cmdinfo*)cmd;

	string Base64Result;
	string Command="c:\\windows\\system32\\cmd.exe /c ";
	Command+=pci->cmd_content;
	string Result;
	int args[3] = {0}; 
	args[0]=(int)&Command;   
	args[1]=(int)&Result;

	if (pci->cmd_content.find("cd ")==0)
	{
		char szCurrentDir[1024];
		ZeroMemory(szCurrentDir,1024);
		pci->cmd_content.erase(0,3);
		Msg("切换目录:%s",pci->cmd_content.c_str());
		if (!SetCurrentDirectory(pci->cmd_content.c_str()))
		{
			LPSTR lpBuffer=NULL;    
			FormatMessage ( FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_IGNORE_INSERTS|FORMAT_MESSAGE_FROM_SYSTEM,
				NULL,GetLastError(),LANG_NEUTRAL,(LPTSTR) & lpBuffer,0 ,NULL);
			if(lpBuffer==NULL)
			{
				Result=Result+"命令执行出错,原因:未知.\n";
			}
			Result=Result+"命令执行出错,原因:"+lpBuffer;
			LocalFree (lpBuffer);
		}
		else
		{
			GetCurrentDirectory(1024,szCurrentDir);
			Result=Result+"当前目录:"+szCurrentDir;
		}
	}
	else
	{
		CMDExecute((void *)args);
	}
	
	if (Result.length()==0)
	{
		Result+="命令执行完成";
	}

	Base64Encode(reinterpret_cast<const unsigned char*>(Result.c_str()),Result.length(),Base64Result);
	Replace_plus(Base64Result);
	SubmitResult(pci,Base64Result.c_str());
	Result.erase();
	Base64Result.erase();
}

void SubmitResult(cmdinfo *pci,const char *result)
{
	//$cmd_id=addslashes($_POST['cmd_id']);
	//	$submit_time=addslashes($_POST['submit_time']);
	//$result_content=addslashes($_POST['result_content']);

	HttpMessenger *hm_SubmitResult=new HttpMessenger(server_addr);
	string sr_post_data;
	DWORD nCL=0;
	sr_post_data+="cmd_id=";
	sr_post_data+=pci->cmd_id;
	sr_post_data+="&result_content=";
	sr_post_data+=result;

	Msg("开始提交%s的执行结果\r\n",pci->cmd_id);
	if (!hm_SubmitResult->CreateConnection())
	{
		Msg("连接结果提交服务器失败，错误日志:\r\n");
		Msg((char *)hm_SubmitResult->GetErrorLog().c_str());
		delete[] hm_SubmitResult;
		return;
	}
	else
	{			
	//		Msg("\r\n连接结果提交服务器成功\r\n");
	}
	
	if (!hm_SubmitResult->CreateAndSendRequest("POST","/function/SubmitResult.php",server_addr,(char *)sr_post_data.c_str()))
	{
		Msg("提交命令结果失败，错误日志:\r\n%s",hm_SubmitResult->GetErrorLog().c_str());
		delete[] hm_SubmitResult;
		return;
	}
	
	if (hm_SubmitResult->m_ResponseText.find("Success")==string::npos)
	{
		Msg("\r\n");
		Msg((char *)hm_SubmitResult->m_ResponseText.c_str());
	}
	else
	{
		Msg("提交%s的结果完成\r\n",pci->cmd_id);
	}
	delete []hm_SubmitResult;
	sr_post_data.erase();
	delete pci;
}

bool GetHostInfo(hostinfo &pHI)
{
		DWORD len=500;
		OSVERSIONINFO osvi;

		if (!GetComputerName(pHI.rc_host_name,&len))
		{
			return false;
		}

		memset(&osvi,0,sizeof(osvi));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if(!GetVersionEx(&osvi))
		{
			return false;
		}
		switch (osvi.dwPlatformId)
		{
		case VER_PLATFORM_WIN32_NT:
			if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion ==2 )		
				wsprintf(pHI.rc_host_os,TEXT("Windows 8"));
			if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion ==1 )		
				wsprintf(pHI.rc_host_os,TEXT("Windows 7"));
			if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion ==0 )		
				wsprintf(pHI.rc_host_os,TEXT("Windows Vista"));
			if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion ==2 )			
				wsprintf(pHI.rc_host_os,TEXT("Windows 2003"));
			if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion ==1 )
				wsprintf(pHI.rc_host_os,TEXT("Windows XP"));
			if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion ==0 )
				wsprintf(pHI.rc_host_os,TEXT("Windows 2000"));
			if ( osvi.dwMajorVersion <= 4 )
				wsprintf(pHI.rc_host_os,TEXT("Windows NT"));
			break;
		case VER_PLATFORM_WIN32_WINDOWS:
			if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion ==0)
				wsprintf(pHI.rc_host_os,TEXT("Windows 95"));
			if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
				wsprintf(pHI.rc_host_os,TEXT("Windows 98"));
			break;
		}
		//MessageBox(hwnd,pHI.rc_host_os,"操作系统版本",0);
		return true;
}

void CMDExecute(void *temp)
{
	DWORD bytesRead;
	char buffer[4097]={0};
	int *args=(int *)temp;	
	string *hs_Command=(string *)args[0];
	string *hs_Result=(string*)args[1];

	STARTUPINFO hs_StartInfo;
	PROCESS_INFORMATION hs_ProcessInformation;
	SECURITY_ATTRIBUTES hs_SecurityAttributes;
	HANDLE hs_PipeRead,hs_PipeWrite;

	memset(&hs_StartInfo,0,sizeof(STARTUPINFO));
	memset(&hs_ProcessInformation,0,sizeof(PROCESS_INFORMATION));
	memset(&hs_SecurityAttributes,0,sizeof(SECURITY_ATTRIBUTES));

	hs_SecurityAttributes.nLength=sizeof(hs_SecurityAttributes);
	hs_SecurityAttributes.lpSecurityDescriptor=NULL;
	hs_SecurityAttributes.bInheritHandle=TRUE;

	if (!CreatePipe(&hs_PipeRead,&hs_PipeWrite,&hs_SecurityAttributes,0))
	{
		return;
	}
	GetStartupInfo(&hs_StartInfo);
	hs_StartInfo.cb=sizeof(STARTUPINFO);
	hs_StartInfo.dwFlags=STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	hs_StartInfo.wShowWindow=SW_HIDE;
	hs_StartInfo.hStdError=hs_StartInfo.hStdOutput=hs_PipeWrite;
	hs_StartInfo.hStdInput=hs_PipeWrite;

#ifdef UNICODE
	int wclen= MultiByteToWideChar(CP_ACP,0,hs_Command->c_str(),strlen(hs_Command->c_str())+1,NULL,0);
	LPTSTR lpcmd=new WCHAR[wclen];
	MultiByteToWideChar(CP_ACP,0,hs_Command->c_str(),strlen(hs_Command->c_str())+1,lpcmd,wclen);
#else
	string lpcmd;

	UTF8ToANSI((char *)hs_Command->c_str(),&lpcmd);
#endif

	if (CreateProcess(NULL,(char *)lpcmd.c_str(),NULL,NULL,1,0,NULL,NULL,&hs_StartInfo,&hs_ProcessInformation))
	{
		//	WaitForSingleObject(hs_ProcessInformation.hProcess,INFINITE);
		CloseHandle(hs_ProcessInformation.hProcess);
		CloseHandle(hs_ProcessInformation.hThread);
		CloseHandle(hs_PipeWrite);
#ifdef UNICODE  
		delete [] lpcmd; 
#endif
	}
	else
	{
		CloseHandle(hs_PipeRead);
		CloseHandle(hs_PipeWrite);
#ifdef UNICODE 
		delete [] lpcmd;
#endif
		return;
	} 

	while (true)  
	{
		if (!ReadFile(hs_PipeRead,buffer,4096,&bytesRead,NULL))
		{
			break;
		}
		*hs_Result+=buffer;
		memset(buffer,0,4097);
	}
	CloseHandle(hs_PipeRead);
}

void Base64Encode(unsigned char const* bytes_to_encode, unsigned int in_len,string &ret)
{
	const string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	int i = 0;
	int j = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];

	while (in_len--)
	{
		char_array_3[i++] = *(bytes_to_encode++);
		if (i == 3) 
		{
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for(i = 0; (i <4) ; i++)
			{
				ret += base64_chars[char_array_4[i]];
			}
			i = 0;
		}
	}

	if (i)
	{
		for(j = i; j < 3; j++)
		{
			char_array_3[j] = '\0';
		}

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;

		for (j = 0; (j < i + 1); j++)
		{
			ret += base64_chars[char_array_4[j]];
		}

		while((i++ < 3))
		{
			ret += '=';
		}
	}
}

static inline bool is_base64(unsigned char c) 
{
	return (isalnum(c) || (c == '+') || (c == '/'));
}

void Base64Decode(std::string const& encoded_string,string &ret)
{
	const string base64_chars ="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	int in_len = encoded_string.size();
	int i = 0;
	int j = 0;
	int in_ = 0;
	unsigned char char_array_4[4], char_array_3[3];

	while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) 
	{
		char_array_4[i++] = encoded_string[in_]; in_++;
		if (i ==4) 
		{
			for (i = 0; i <4; i++)
			{
				char_array_4[i] = base64_chars.find(char_array_4[i]);
			}
			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (i = 0; (i < 3); i++)
			{
				ret += char_array_3[i];
			}
			i = 0;
		}
	}
	if (i) 
	{
		for (j = i; j <4; j++)
		{
			char_array_4[j] = 0;
		}

		for (j = 0; j <4; j++)
		{
			char_array_4[j] = base64_chars.find(char_array_4[j]);
		}
		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

		for (j = 0; (j < i - 1); j++) 
		{
			ret += char_array_3[j];
		}
	}
}

void PathDecode(string base64_json,string &path_out)
{
	string utf8_path;
//	base64_json.erase(0,1);
//	base64_json.erase(base64_json.length()-2,1);
	Base64Decode(base64_json,utf8_path);
	UTF8ToANSI(utf8_path.c_str(),&path_out);
}

void Replace_plus(string &str)
{
	while(true)
	{
		string::size_type pos_plus(0);
		if((pos_plus=str.find("+"))!=string::npos)
		{
			str.replace(pos_plus,1,"%2B");
		}
		else
		{
			break;
		}
	}
}

bool AnalyzeMouseCommand(string cmd,mousecommand &mscmd)
{
	int startPos=-1;
	int endPos=-1;
	startPos=cmd.find("<act>",0);
	if (startPos==string::npos)
	{
		Msg("鼠标控制命令格式错误\r\n");
		return false;
	}
	startPos+=5;	//strlen("<act>")
	endPos=cmd.find("</act>",startPos);
	char *cmdaction=new char[endPos-startPos+2];
	ZeroMemory(cmdaction,endPos-startPos+2);
	memcpy(cmdaction,&cmd[startPos],endPos-startPos);
	if (strlen(cmdaction)>2)
	{
		Msg("错误的鼠标控制动作\r\n");
		return false;
	}
	//鼠标动作类型
	mscmd.action=atoi(cmdaction);

	startPos=cmd.find("<x>",endPos);
	if (startPos==string::npos)
	{
		Msg("错误的鼠标控制动作—X\r\n");
		return false;
	}
	startPos+=3;//strlen("<x>");
	endPos=cmd.find("</x>",startPos);
	char *point_x=new char[endPos-startPos+2];
	ZeroMemory(point_x,endPos-startPos+2);
	memcpy(point_x,&cmd[startPos],endPos-startPos);
	if (strlen(point_x)>10)
	{
		Msg("错误的X坐标\r\n");
		return false;
	}
	//鼠标X坐标
	mscmd.x_pos=atoi(point_x)*1.5;

	startPos=cmd.find("<y>",endPos);
	if (startPos==string::npos)
	{
		Msg("错误的鼠标控制动作—Y\r\n");
		return false;
	}
	startPos+=3;	//strlen("<y>");
	endPos=cmd.find("</y>",startPos);
	char *point_y=new char[endPos-startPos+2];
	ZeroMemory(point_y,endPos-startPos+2);
	memcpy(point_y,&cmd[startPos],endPos-startPos);
	if (strlen(point_y)>10)
	{
		Msg("错误的Y坐标\r\n");
		return false;
	}
	//鼠标Y坐标
	mscmd.y_pos=atoi(point_y)*1.5;

	delete cmdaction;
	delete point_x;
	delete point_y;
	return true;
}

clock_t start_time=0;
clock_t finish_time=0;
double totaltime=0;
void startclock()
{
	start_time=clock();	
}

void stopclock(char *tips)
{
	finish_time=clock();
	totaltime=(double)(finish_time-start_time);
	char outstr[200];
	sprintf_s(outstr,"%s用时:%lf毫秒",tips,totaltime);
	MessageBox(NULL,outstr,"提示",0);
}

bool InitialStruct()
{
	//初始化套接字
	HttpMessenger::InitialSocket();

	//初始化GDI+
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	GetCodecClsid(L"image/jpeg", &codecClsid);
	GetCustomServerIP();
	GetCustomUserName();
	//初始化DX用于截屏
//	InitialDX();

	return true;
}

void CleanStruct()
{
	WSACleanup();
	Gdiplus::GdiplusShutdown(gdiplusToken);
}

void SaveCustomServerIP(char *pCustomIP)
{
	HKEY hSubKey;
	HKEY hKey;

	if( ERROR_SUCCESS!=RegOpenKeyEx( HKEY_CURRENT_USER,"Software\\SinSoul\\DatabaseSystemDevelopment_GUI\\",0,KEY_READ | KEY_WRITE,&hKey))
	{
		RegOpenKeyEx( HKEY_CURRENT_USER,"Software",0,KEY_READ | KEY_WRITE,&hKey);
		RegCreateKey(hKey,"SinSoul\\DatabaseSystemDevelopment_GUI\\",&hSubKey);
		if( ERROR_SUCCESS != RegSetValueEx(hSubKey,"ServerIP",0,REG_SZ,(LPBYTE)pCustomIP,strlen(pCustomIP)))
		{
			Msg("保存自定义服务器出错\r\n");
			return;
		}
		RegCloseKey(hKey);
		RegCloseKey(hSubKey);
		return;
	}

	if( ERROR_SUCCESS != RegSetValueEx(hKey,"ServerIP",0,REG_SZ,(LPBYTE)pCustomIP,strlen(pCustomIP)))
	{
		Msg("保存自定义服务器出错\r\n");
		return;
	}
	RegCloseKey(hKey);
}

void GetCustomServerIP()
{
	//server_addr
	char CustomServer[255];
	HINSTANCE old=hinst;
	HKEY  hKEY;
	LPCTSTR RegPath = "Software\\SinSoul\\DatabaseSystemDevelopment_GUI\\";
	if(ERROR_SUCCESS!=RegOpenKeyEx(HKEY_CURRENT_USER, RegPath, 0, KEY_READ, &hKEY))
	{
		Msg("使用默认服务器：%s \r\n",server_addr); 
		return;
	} 
	ZeroMemory(CustomServer,255);
	DWORD cbData = 255*2;
	DWORD type = REG_SZ;
	if(ERROR_SUCCESS!=RegQueryValueEx(hKEY, "ServerIP", NULL, &type,(LPBYTE)CustomServer, &cbData)) 
	{
		Msg("使用默认服务器.\r\n"); 
		return; 
	}
	ZeroMemory(server_addr,255);
	memcpy(server_addr,CustomServer,strlen(CustomServer));
	Msg("使用自定义服务器:%s\r\n",server_addr);
	RegCloseKey(hKEY);
	hinst=old;
}

void SaveCustomUserName(char *pUserName)
{
	HKEY hSubKey;
	HKEY hKey;

	if( ERROR_SUCCESS!=RegOpenKeyEx( HKEY_CURRENT_USER,"Software\\SinSoul\\DatabaseSystemDevelopment_GUI\\",0,KEY_READ | KEY_WRITE,&hKey))
	{
		RegOpenKeyEx( HKEY_CURRENT_USER,"Software",0,KEY_READ | KEY_WRITE,&hKey);
		RegCreateKey(hKey,"SinSoul\\DatabaseSystemDevelopment_GUI\\",&hSubKey);
		if( ERROR_SUCCESS != RegSetValueEx(hSubKey,"UserName",0,REG_SZ,(LPBYTE)pUserName,strlen(pUserName)))
		{
			Msg("保存用户名出错\r\n");
			return;
		}
		RegCloseKey(hKey);
		RegCloseKey(hSubKey);
		return;
	}

	if( ERROR_SUCCESS != RegSetValueEx(hKey,"UserName",0,REG_SZ,(LPBYTE)pUserName,strlen(pUserName)))
	{
		Msg("保存用户名出错\r\n");
		return;
	}
	RegCloseKey(hKey);
}

void GetCustomUserName()
{
	char UserName[25];
	HINSTANCE old=hinst;
	HKEY  hKEY;
	LPCTSTR RegPath = "Software\\SinSoul\\DatabaseSystemDevelopment_GUI\\";
	if(ERROR_SUCCESS!=RegOpenKeyEx(HKEY_CURRENT_USER, RegPath, 0, KEY_READ, &hKEY))
	{
//		Msg("未保存用户名..\r\n"); 
		return;
	} 
	ZeroMemory(UserName,25);
	DWORD cbData = 25*2;
	DWORD type = REG_SZ;
	if(ERROR_SUCCESS!=RegQueryValueEx(hKEY, "UserName", NULL, &type,(LPBYTE)UserName, &cbData)) 
	{
//		Msg("未保存用户名..\r\n"); 
		return; 
	}
	SetWindowText(hwndEdit,UserName);
	SendMessage(hwndCheck,BM_SETCHECK,true,0);
	SendMessage(hwnd,WM_COMMAND,ID_BTN_LOGIN,(LPARAM)hwndButton);
	TrayIcon.MinimiseToTray(hwnd);
	TrayIcon.ShowBalloon("WRC客户端已经启动...","自动登录",0,5);
	RegCloseKey(hKEY);
	hinst=old;
}