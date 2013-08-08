#include "MyStdAfx.h"
#include "Commctrl.h"
//#pragma comment(linker,"/SUBSYSTEM:Windows")

//#pragma comment(lib,"Comctl32.lib")
char szClassName[11 ] = "WindowsApp";
char *wndName="WRC_数据库系统课程设计客户端_GUI";
HWND hwndButton = 0;
HWND hwndEdit = 0; 
HWND hwndLable_0 = 0;
HWND hwnd=0; 
HWND hwndCheck=0;
bool conn_status=false;
WNDPROC OldEditProc;
HINSTANCE hinst;
CSystemTray TrayIcon;
#define	WM_ICON_NOTIFY WM_APP+10
int WINAPI WinMain (HINSTANCE hThisInstance,HINSTANCE hPrevInstance,LPSTR lpszArgument,int nFunsterStil)
{ 
	MSG messages;
	WNDCLASSEX wincl;

	wincl.hInstance = hThisInstance;
	wincl.lpszClassName = szClassName;
	wincl.lpfnWndProc = WindowProcedure;
	wincl.style = CS_DBLCLKS;
	wincl.cbSize = sizeof (WNDCLASSEX);
	wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
	wincl.hIconSm = (HICON)LoadImage(hThisInstance,MAKEINTRESOURCE(IDI_ICON1),IMAGE_ICON,
		GetSystemMetrics(SM_CXSMICON),GetSystemMetrics(SM_CYSMICON),LR_DEFAULTCOLOR);
	wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
	wincl.lpszMenuName = NULL;
	wincl.cbClsExtra = 0;
	wincl.cbWndExtra = 0;
	wincl.hbrBackground = (HBRUSH) COLOR_MENU;

	//InitCommonControls();

	if (!RegisterClassEx (&wincl))
	{
		return 0;
	}

	hwnd = CreateWindowEx (NULL,szClassName,wndName, 
		WS_OVERLAPPED|WS_SYSMENU|WS_MINIMIZEBOX,CW_USEDEFAULT,CW_USEDEFAULT,
		550,350,HWND_DESKTOP,NULL,hThisInstance,NULL);

	hwndButton=CreateWindowEx(NULL,TEXT("BUTTON"),TEXT("登录"),
		WS_CHILD|WS_VISIBLE
		,360,20,50,20,hwnd,(struct HMENU__ *)ID_BTN_LOGIN,hThisInstance,NULL);

	hwndEdit=CreateWindowEx(NULL,TEXT("EDIT"),TEXT(""),
		WS_CHILD|WS_VISIBLE|WS_BORDER|BS_TEXT,
		140,20,200,20,hwnd,(struct HMENU__ *)0,hThisInstance,NULL);

	hwndLable_0=CreateWindowEx(NULL,TEXT("EDIT"),TEXT(""),
		WS_CHILD|WS_VISIBLE|WS_BORDER|WS_VSCROLL|ES_MULTILINE|ES_READONLY,
		20,70,495,230,hwnd,(struct HMENU__ *)0,hThisInstance,NULL);
	Edit_LimitText(hwndLable_0,0);

	hwndCheck = CreateWindow("BUTTON","记住用户名并自动登录",
		WS_VISIBLE | WS_CHILD |BS_CHECKBOX ,
		155,45,170,20,hwnd,(struct HMENU__ *)8899,hThisInstance,NULL);

	TrayIcon.Create(hThisInstance,hwnd,WM_ICON_NOTIFY,_T("WRC数据库系统课程设计客户端"),
		(HICON)LoadImage(hThisInstance,MAKEINTRESOURCE(IDI_ICON1),IMAGE_ICON,
		GetSystemMetrics(SM_CXSMICON),GetSystemMetrics(SM_CYSMICON),LR_DEFAULTCOLOR),
		ID_POPUP_MENU);

	if (InitialStruct())
	{
		Msg("初始化完成,请输入用户名进行登录!\r\n");
	}
	else
	{
		Msg("初始化失败，检查网络及防火墙设置!\r\n");
	}
	OldEditProc=(WNDPROC)SetWindowLong (hwndEdit, GWL_WNDPROC, (LONG)EditProcedure);
	ShowWindow (hwnd, nFunsterStil);
	hinst=hThisInstance;
	while (GetMessage (&messages, NULL, 0, 0))
	{
		TranslateMessage(&messages);
		DispatchMessage(&messages);
	}
	return messages.wParam;
}


LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_ICON_NOTIFY:
		return TrayIcon.OnTrayNotification(wParam, lParam);
		break;
	case WM_DESTROY:
		{
			if (conn_status)
			{
				Logout();
			}
			PostQuitMessage (0);
		}
		break;
	case WM_CLOSE:
		{
			if (conn_status)
			{
				Logout();
			}
			PostQuitMessage (0);
		}break;
	case WM_SYSCOMMAND:
		{
			if (LOWORD(wParam)==SC_MINIMIZE)
			{
				TrayIcon.MinimiseToTray(hwnd);
			}
			if (LOWORD(wParam)==SC_CLOSE)
			{
				DestroyWindow(hwnd);
			}
			return DefWindowProc (hwnd, message, wParam, lParam);
		}break;
	case WM_COMMAND:
		{ 
			if (LOWORD(wParam)==8899)
			{
				bool flag =SendMessage(hwndCheck,BM_GETCHECK,0,0) == BST_CHECKED ? false:true;
				SendMessage(hwndCheck,BM_SETCHECK,flag,0);
			}
			if (LOWORD(wParam)==IDM_EXIT)
			{
				DestroyWindow(hwnd);
			}
			if (LOWORD(wParam)==IDM_HIDE)
			{
				TrayIcon.MinimiseToTray(hwnd);
			}
			if (LOWORD(wParam)==IDM_SHOW)
			{
				TrayIcon.MaximiseFromTray(hwnd);
			}
			if (LOWORD(wParam)==ID_POPUP_SETTING)
			{
				TrayIcon.MaximiseFromTray(hwnd);
				DialogBox(hinst, (LPCTSTR)IDD_SETTING, hwnd, (DLGPROC)SetServerAddr);
			}
			
			if(LOWORD(wParam)==ID_BTN_LOGIN) 
			{
				//InitialDX();
				//DXScreenShot();
				//UnloadDx();
				
				if (!conn_status)
				{
					GetHostInfo(pHI);			
					if (GetThisUserName())
					{
						EnableWindow(hwndButton,false);
						_beginthread(Online,0,NULL);
					}
					else
					{
						return -1;
					}
				}
				else              //记住这里还要写主机下线的代码！！！
				{
					Logout();
					conn_status=false;
					if (CameraStatus)
					{
						CameraStatus=FALSE;
						StopCaptureDevice();
					}
					SetWindowText(hwndButton,TEXT("登录"));
				}			
			}
		}
		break;
	case WM_ONLINE_MESSAGE:
		{
			int args=0;
			_beginthread(ReceiveCommand,0,(void*)args);
		}
		break;
	case WM_CMDRCVD_MESSAGE:
		{
			//			CMD_ID=88 : 下线
			//			CMD_ID=0 : 命令行
			//			CMD_ID=1 : 屏幕截图
			//			CMD_ID=2 : 摄像头截图
			//			CMD_ID=3 : 开启摄像头
			//			CMD_ID=5 : 关闭摄像头
			//			CMD_ID=4 : 鼠标事件
			//			CMD_ID=6 : 键盘按下
			//			CMD_ID=7 : 键盘弹起
			//			CMD_ID=8 : 枚举文件夹（创建文件夹树）
			//			CMD_ID=9 : 枚举文件夹内文件（列表展示）
			//			CMD_ID=10 : 枚举进程
			//			CMD_ID=11 : 基本文件操作（复制，粘贴，删除，重命名）
			//			CMD_ID=12 : 基本进程操作（结束，列模块，列线程）
			cmdinfo *cmd_rcvd=new cmdinfo;
			memcpy(cmd_rcvd,(cmdinfo*)wParam,sizeof(cmdinfo));
			switch(atoi(cmd_rcvd->cmd_type))
			{
			case 88:
				{
					Msg("下线通知\r\n");
				}break;
			case 0:
				{
					_beginthread(RunCmd,0,(void*)cmd_rcvd);
				}break;
			case 1:
				{
					_beginthread(ScreenCapture,0,(void*)cmd_rcvd);
				}
				break;
			case 2:
				{
					_beginthread(CaptureCurrentFrame,0,(void*)cmd_rcvd);
				}
				break;
			case 3:
				{
					if (CameraStatus)
					{
						SubmitResult(cmd_rcvd,"1"); 
					}
					else if (StartCaptureDevice(hwnd))
					{
						//	Sleep(1500);//等待摄像头真正启动成功
						SubmitResult(cmd_rcvd,"1"); 
						CameraStatus=TRUE;
					}
					else
					{
						SubmitResult(cmd_rcvd,"0");
					}

				}
				break;
			case 5:
				{
					CameraStatus=FALSE;
					StopCaptureDevice();
					SubmitResult(cmd_rcvd,"1"); 
				}
				break;
			case 4:	//鼠标命令格式：<act>4</act><x>124</x><y>3534</y>
				{	//action=1:单击
					//action=2:双击
					//action=3:右击
					mousecommand mscmd;
					if (AnalyzeMouseCommand(cmd_rcvd->cmd_content,mscmd))
					{
						if (mscmd.action==1)
						{
							LeftClick(mscmd.x_pos,mscmd.y_pos);
						}else if (mscmd.action==2)
						{
							LeftDoubleClick(mscmd.x_pos,mscmd.y_pos);
						}else if (mscmd.action==3)
						{
							RightClick(mscmd.x_pos,mscmd.y_pos);
						}
					//	SubmitResult(cmd_rcvd->cmd_id,"0");
					}
					else
					{
						SubmitResult(cmd_rcvd,"鼠标命令出错");
					}
				}
				break;
			case 6:
				{
					KeyDown(atoi(cmd_rcvd->cmd_content.c_str()));
				}
				break;
			case 7:
				{
					KeyUp(atoi(cmd_rcvd->cmd_content.c_str()));
				}
				break;
			case 8:
				{
					_beginthread(Enum_Drectory,0,(void*)cmd_rcvd);
				}
				break;
			case 9:
				{
					_beginthread(Enum_All_File_in_Drectory,0,(void*)cmd_rcvd);
				}
				break;	
			case 10:
				{
					//EnumProcess();
					_beginthread(TaskList,0,(void*)cmd_rcvd);
				}
				break;	
			case 11:
				{		
					_beginthread(FileOperate,0,(void*)cmd_rcvd);
				}
				break;	
			case 12:
				{		
					_beginthread(TaskOperate,0,(void*)cmd_rcvd);
				}
				break;	
			}
			//		delete[] cmd_rcvd;
		}
		break;
	case WM_CMDPCSD_MESSAGE:
		{
			MessageBox(hwnd,TEXT("命令处理完成"),0,0);
		}
		break;
	default:                    
		return DefWindowProc (hwnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK EditProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CHAR:
		{
			if (wParam==VK_RETURN)
			{
				if (!conn_status)
				{
					GetHostInfo(pHI);			
					if (GetThisUserName())
					{
						EnableWindow(hwndButton,false);
						_beginthread(Online,0,NULL);
					}
					else
					{
						return -1;
					}
				}
				else
				{
					Logout();
					conn_status=false;
					if (CameraStatus)
					{
						CameraStatus=FALSE;
						StopCaptureDevice();
					}
					SetWindowText(hwndButton,TEXT("登录"));
				}	
			}
			CallWindowProc(OldEditProc,hwnd,message,wParam,lParam);
		}break;
	default:                    
		return CallWindowProc(OldEditProc,hwnd,message,wParam,lParam);
		break;
	}
	return 0;
}

LRESULT CALLBACK SetServerAddr(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
		if (LOWORD(wParam) == IDC_BTN_SAVE) 
		{
			int length;
			length=GetWindowTextLength(GetDlgItem(hDlg,IDC_SERVER_ADDR));
			if (length<2)
			{
				MessageBox(hDlg,"输入的地址不正确","错误",0);
				return false;
			}
			length+=3;
			char *szBuff=new char[length];
			ZeroMemory(szBuff,length);
			GetWindowText(GetDlgItem(hDlg,IDC_SERVER_ADDR),szBuff,length);

			hostent *m_phostip;
			m_phostip=gethostbyname(szBuff);
			if(m_phostip==NULL)
			{
				MessageBox(hDlg,"无法解析你所输入的服务器地址，请检查后再试","域名解析失败",0);
				return false;
			}
			SaveCustomServerIP(szBuff);
			GetCustomServerIP();
			if (conn_status)
			{
				Logout();
				conn_status=false;
				if (CameraStatus)
				{
					CameraStatus=FALSE;
					StopCaptureDevice();
				}
				SetWindowText(hwndButton,TEXT("登录"));
				GetHostInfo(pHI);			
				if (GetThisUserName())
				{
					EnableWindow(hwndButton,false);
					_beginthread(Online,0,NULL);
				}
				else
				{
					return -1;
				}
			}
			EndDialog(hDlg, LOWORD(wParam));
			delete [] szBuff;
			return true;
		}
		if(LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
		}
		break;
	}
	return false;
}

void PrintText(TCHAR * szBuff)
{
	int nTextLength=GetWindowTextLength(hwndLable_0);
	SendMessage (hwndLable_0, EM_SETSEL, (WPARAM)nTextLength, (LPARAM)nTextLength);
	SendMessage (hwndLable_0, EM_REPLACESEL, 0, (LPARAM) ((LPSTR) szBuff));
}

bool GetThisUserName()
{
	int length;
	length=GetWindowTextLength(hwndEdit);
	if (length<4)
	{
		MessageBox(hwnd,"用户名不合法,请检查输入是否正确",0,0);
		return false;
	}
	length+=3;
	TCHAR *szBuff=new TCHAR[length];
	ZeroMemory(szBuff,length);
	GetWindowText(hwndEdit,szBuff,length);
	if (strlen(szBuff)>20)
	{
		MessageBox(hwnd,TEXT("用户名过长,请检查输入是否正确"),0,0);
		return false;
	}
	string userTemp;
	ANSIToUTF8(szBuff,&userTemp);
	sprintf_s(userName,userTemp.c_str());
	return true;
}

void Logout()
{
	if (conn_status&&set_cookie)
	{
		conn_status=false;
		HttpMessenger *hm_logout=new HttpMessenger(server_addr);
		if (!hm_logout->CreateConnection())
		{
			Msg("下线时服务器出错，错误日志:%s\r\n",hm_logout->GetErrorLog().c_str());
			EnableWindow(hwndButton,true);
			return;
		}
		
		hm_logout->CreateAndSendRequest("GET","/function/ClientLogout.php");
			//hm_logout->CreateAndSendRequest("POST","/index.php?action=logout");
		Msg("登出状态:%s\r\n",hm_logout->m_ResponseText.c_str());
		set_cookie=false;
		ZeroMemory(cookie_data,1024);
		delete hm_logout;
	}
}
void Online(void *)
{
	HttpMessenger *hm_online=new HttpMessenger(server_addr); 
	set_cookie=false;
	ZeroMemory(cookie_data,1024);
	if (!hm_online->CreateConnection())
	{
		Msg("连接服务器出错，错误日志:%s\r\n",hm_online->GetErrorLog().c_str());
		EnableWindow(hwndButton,true);
		return;
	}
	PrintText("成功连接到服务器\r\n");
	sprintf_s(online_post_data,"user_name=%s&rc_host_name=%s&rc_host_os=%s",userName,pHI.rc_host_name,pHI.rc_host_os);
	if (!hm_online->CreateAndSendRequest("POST","/function/AddUserHost.php",server_addr,online_post_data))
	{
		Msg("与服务器通信出错，错误日志:%s\r\n",hm_online->GetErrorLog().c_str());
		EnableWindow(hwndButton,true);
		return;
	}
	AnalyzeJumpPage(hm_online);
	if (hm_online->GetCookie(cookie_data,1024))
	{
		set_cookie=true;
	}
	else
	{
		MessageBox(hwnd,"未能成功在服务器记录本机，请联系服务器管理员","登录错误",0);
	}
	hm_online->UTF8ResponsetoANSI();
	Msg("服务器响应:%s\r\n",hm_online->m_ResponseText.c_str());
	if (hm_online->m_ResponseText.find("rror",0)==string::npos&&hm_online->m_ContentLength<20)
	{
		string session_id=cookie_data;
		session_id.erase(0,10);
		Msg("登录成功.此次会话标识:%s\r\n",session_id.c_str());

		if (SendMessage(hwndCheck,BM_GETCHECK,0,0) == BST_CHECKED)
		{
			SaveCustomUserName(userName);
		}

	//以下注释部分在服务器不支持session时可使用
	/*	sprintf_s(hostid,hm_online->m_ResponseText.c_str());
		for (int i=0;i<20;i++)
		{
			if (hostid[i]=='\r'&&hostid[i+1]=='\n')
			{
				for (;i<20;i++)
				{
					hostid[i]=0x0;
				}
			}
		}
		if (strlen(hostid)==0)
		{
			PrintText("未从服务器响应中获取到主机ID\r\n");
			EnableWindow(hwndButton,true);
			return;
		}

		Msg("登录成功,本主机ID:%s\r\n",hostid);*/

		conn_status=true;
		SetWindowText(hwndButton,TEXT("断开"));
		SendMessage(hwnd,WM_ONLINE_MESSAGE,0,0);
		EnableWindow(hwndButton,true);
		
//		ShowWindow(hwnd,SW_HIDE);
	}
	else
	{
		MessageBox(hwnd,hm_online->m_ResponseText.c_str(),"发生错误",0);
		PrintText("登录过程中发生错误，终止登录。\r\n");
		EnableWindow(hwndButton,true);
	}
	delete hm_online;
	return;
}