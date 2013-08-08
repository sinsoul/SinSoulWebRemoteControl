//Author:SinSoul
//E-mail:nh6080@gmail.com
//Function:用于HTTP通信,来自我自己的一个远控项目，项目体验地址http://nh6080.sinaapp.com
#include "MyStdAfx.h"

bool HttpMessenger::SocketLoaded=false;

HttpMessenger::HttpMessenger(char *szHostName,int nPort)
{
	m_bConnected=false;
	m_bResponsed=false;
	Chunked=false;
	m_ContentLength=0;
	m_port=nPort;
	server_addr+=szHostName;
	if (!CreateSocket())
	{
		return;
	}
}
string HttpMessenger::GetErrorLog()
{
	return m_Error;
}

bool HttpMessenger::CreateSocket()
{
	if(m_bConnected)
	{
		return false;
	}
	struct protoent *pv; 
	pv=getprotobyname("tcp"); 
	m_socket=socket(PF_INET,SOCK_STREAM,pv->p_proto);
	if(m_socket==INVALID_SOCKET)
	{
		m_Error=m_Error+"创建套接字失败!\r\n";
		return false;
	}
	return true;
}

bool HttpMessenger::CreateConnection()
{
	if(m_bConnected)
	{
		m_Error=m_Error+"连接已经建立了!\r\n";
		return false;
	}
	m_phostip=gethostbyname(server_addr.c_str());
	if(m_phostip==NULL)
	{
		m_Error=m_Error+"域名解析失败!\r\n";
		return false;
	}
	struct in_addr ip_addr;
	memcpy(&ip_addr,m_phostip->h_addr_list[0],4);
	struct sockaddr_in destaddr;
	memset((void *)&destaddr,0,sizeof(destaddr)); 
	destaddr.sin_family=AF_INET;
	destaddr.sin_port=htons(m_port);
	destaddr.sin_addr=ip_addr;
	if(connect(m_socket,(struct sockaddr*)&destaddr,sizeof(destaddr))!=0)
	{
		m_Error=m_Error+"连接到服务器失败!\n";
		return false;
	}
	m_bConnected=true;
	return true;
}

bool HttpMessenger::CreateAndSendRequest(char *RequestType,char *ResourcePath,char *Host,char *PostData,bool OnlyGetLength,char *Save2File)
{
	long recvlength=0;
	m_RequestHeader.erase();
	m_RequestHeader=m_RequestHeader+RequestType+" "+ResourcePath+" HTTP/1.1\r\n";
	if (Host==NULL)
	{
		m_RequestHeader=m_RequestHeader+"Host: "+server_addr+"\r\n";
	}
	else
	{
		m_RequestHeader=m_RequestHeader+"Host: "+Host+"\r\n";
	}
	
	m_RequestHeader=m_RequestHeader+"Connection: keep-alive\r\n";
	m_RequestHeader=m_RequestHeader+"User-Agent: WRC_Client\r\n";
//	m_RequestHeader=m_RequestHeader+"Cache-Control: max-age=0\r\n";
	m_RequestHeader=m_RequestHeader+"Accept: */*\r\n";
//	m_RequestHeader=m_RequestHeader+"Origin:  http://222.73.105.196\r\n";
	if (set_cookie)
	{
		m_RequestHeader=m_RequestHeader+"Cookie: "+cookie_data+"\r\n";
	}
	if (PostData!=NULL)
	{
		char ctemp[10];
		int ContentLength=strlen(PostData);
		_ltoa_s(ContentLength,ctemp,10);
		m_RequestHeader=m_RequestHeader+"Content-Type: application/x-www-form-urlencoded\r\n";
		m_RequestHeader=m_RequestHeader+"Content-Length: "+ctemp+"\r\n";
		m_RequestHeader=m_RequestHeader+"\r\n"+PostData+"\r\n\r\n";
	}
	else
	{
		m_RequestHeader+="\r\n";
	}

	if(!m_bConnected)
	{
		m_Error=m_Error+"尚未建立与服务器的连接!\r\n";
		return false;
	}
	
	if(send(m_socket,m_RequestHeader.c_str(),m_RequestHeader.length(),0)==SOCKET_ERROR)
	{
		m_Error=m_Error+"向服务器发送请求失败!\r\n";
		return false;
	}
	

	recvlength=Receive(OnlyGetLength,Save2File);
	if (recvlength<=0)
	{
		return false;
	}
//	Msg("接收到的数据长度:%ld\r\n",recvlength);
	return true;
}

unsigned long HttpMessenger::Receive(bool OnlyGetLength,char *Save2File)
{
	char temp[1025]={'\0'};
	char c[3]={'\0'};
	long rcvdsize=0,nLength=0;
	unsigned long recvstatus=0;
	ZeroMemory(temp,1025);
	//if (Save2File==NULL)
	//{
	//	SetTimeout(5000,0);//设置接收超时，尝试解决10%卡死的BUG
	//}
	
	for (nLength=0;nLength<=1024;nLength++)
	{
		if (recv(m_socket,c,1,0)==0)
		{
				m_Error=m_Error+"接收响应头时意外终止!\r\n";
				return -1;
		}
		temp[nLength]=*c;
		if (temp[nLength]=='\n'&&
			temp[nLength-1]=='\r'&&
			temp[nLength-2]=='\n'&&
			temp[nLength-3]=='\r')
		{
			break;
		}
	}

	m_ResponseHeader+=temp;

//	Msg("接收到的响应头:%s",temp);

	m_bResponsed=true;
	if (GetResponseState()!=200)
	{
		m_Error+="服务器返回状态异常";
		return -1;
	}
	m_ContentLength=GetContentLength();
	if (m_ContentLength==-1)
	{
		return -1;
	}
	if (OnlyGetLength)
	{
		return 1;
	}

	ULONG n_recvd=0;
	double n_recvperc=0,total_Length=m_ContentLength,last_recvperc=0;

	//HANDLE hOut;
	//hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	//CONSOLE_SCREEN_BUFFER_INFO info={0};  
	//GetConsoleScreenBufferInfo( hOut , &info );  
	//	SetConsoleTextAttribute(hOut,0x0A);

	//	Msg("总长度:%d\r\n",m_ContentLength);
	HANDLE hFileDownload=INVALID_HANDLE_VALUE;
	if (Save2File!=NULL)
	{
		hFileDownload=CreateFile(Save2File,GENERIC_WRITE|GENERIC_READ,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
		if (hFileDownload==INVALID_HANDLE_VALUE)
		{
			Msg("创建本地文件失败.\r\n");
		}
	}
	

	while(m_ContentLength>n_recvd)
	{
		unsigned long Lastlength=m_ContentLength-n_recvd;
		char * LastResponse= new char[Lastlength+1];
		if (LastResponse==NULL)
		{
			Msg("内存分配出错\r\n");
		}
		ZeroMemory(LastResponse,Lastlength+1);
		recvstatus=recv(m_socket,LastResponse,Lastlength,0);

		n_recvd+=recvstatus;
		n_recvperc=(n_recvd/total_Length);
		//SetConsoleCursorPosition(hOut, info.dwCursorPosition);
		if (last_recvperc!=0&&n_recvperc*100<last_recvperc)
		{
			Msg("接收过程出错，强制结束本次请求并重试.....\r\n");
			return 0;
		}
		last_recvperc=n_recvperc*100;
		if (hFileDownload!=INVALID_HANDLE_VALUE)
		{
			DWORD dwNumberOfBytesWritten = 0;
			if(!WriteFile(hFileDownload,LastResponse, recvstatus,&dwNumberOfBytesWritten, NULL))
			{
				Msg("写入文件时发生错误，错误代码:%ld\r\n",GetLastError());
			}
//			Msg("写入:%ld，接收:%ld\r\n",dwNumberOfBytesWritten,recvstatus);
			delete[] LastResponse;
		}
		else
		{
			m_ResponseText+=LastResponse;
			delete[] LastResponse;
		}
		
		if (recvstatus==0)
		{
			return 1;
		}
		
	/*	ProgressMsg("D:%.2lf%%,当前:%d",n_recvperc*100,m_ResponseText.length());
		Msg("recvstatus:%ld\tn_recvd:%ld\tResponseLen:%ld\r\n",recvstatus,n_recvd,m_ResponseText.length());*/
		
	}
	if (Chunked)
	{
		//对于响应头中没有Content-Length而是Transfer-Encoding: chunked的服务器进行处理
		//这个标志在GetContentLength函数中会设置	
		return ReceiveChunked();
	}
	CloseHandle(hFileDownload);
	return  n_recvd;
}

int HttpMessenger::ReceiveChunked()
{
	char hexlength[12]={'\0'};
	int nIndex=0;
	unsigned int trunklength=0;
	string trunktext;
	char c;
	do
	{
		nIndex=0;
		recv(m_socket,hexlength,2,0);
		if (hexlength[0]!='\r'||hexlength[1]!='\n')
		{
			m_Error+="获取下一个chunk长度时出错\r\n";
			return -1;
		}
		ZeroMemory(hexlength,12);
		while(nIndex<10)
		{
			recv(m_socket,&c,1,0);
			hexlength[nIndex]=c;
			if (hexlength[nIndex]=='\n'&&hexlength[nIndex-1]=='\r')
			{
				hexlength[nIndex]='\0';
				hexlength[nIndex-1]='\0';
				trunklength=HextoInt(hexlength,strlen(hexlength));
				m_ContentLength+=trunklength;
				break;
			}
			nIndex++;
		}

		if (nIndex>9)
		{
			m_Error=m_Error+"分块长度分析错误!\r\n";
			return -1;
		}
		trunktext.erase();
		while(trunklength>trunktext.length())
		{
			int Lastlength=trunklength-trunktext.length();
			char * LastResponse= new char[Lastlength+1];
			ZeroMemory(LastResponse,Lastlength+1);
			recv(m_socket,LastResponse,Lastlength,0);
			trunktext+=LastResponse;
			delete[] LastResponse;
		}
		m_ResponseText+=trunktext;
	}while(trunklength!=0);
	return m_ContentLength;
}


int HttpMessenger::GetResponseState()
{
	if(!m_bResponsed)
	{
		return -1;
	}
	if (m_ResponseHeader.empty())
	{
		return -1;
	}
	char szState[3];
	szState[0]=m_ResponseHeader[9];
	szState[1]=m_ResponseHeader[10];
	szState[2]=m_ResponseHeader[11];
	return atoi(szState);
}

long HttpMessenger::GetContentLength()
{
	if(!m_bResponsed)
	{
		return -1;
	}
	long nFileSize=0;
	char szValue[10];
	int nPos=-1;
	nPos=m_ResponseHeader.find("Content-Length",0);
	if(nPos != -1)
	{
		nPos += 16;
		int nCr=m_ResponseHeader.find("\r\n",nPos);
		memcpy(szValue,&m_ResponseHeader[nPos],nCr-nPos);
		nFileSize=atoi(szValue);
		return nFileSize;
	}
	else if (m_ResponseHeader.find("ding: chun",0)!=string::npos)
	{
		char hexlength[14]={'\0'};
		char c;
		int n_hex=0;
		for (n_hex=0;n_hex<=12;n_hex++)
		{
			recv(m_socket,&c,1,0);
			hexlength[n_hex]=c;
			if (hexlength[n_hex]=='\n'&&hexlength[n_hex-1]=='\r')
			{
				break;
			}
		}
		hexlength[n_hex]='\0';
		hexlength[n_hex-1]='\0';
//		Msg("获取到分块长度Hex:%s,位数为:%d\r\n",hexlength,n_hex+1);
		if (strlen(hexlength)>10)
		{
				m_Error=m_Error+"获取分块长度出错\r\n";
				return -1;
		}
		nFileSize=HextoInt(hexlength,strlen(hexlength));
		Chunked=true;
		return nFileSize;
	}
	else
	{
		m_Error+="未能识别返回长度表示方式\r\n";
		return -1;
	}
}

bool HttpMessenger::MyCloseSocket()
{
//	SocketLoaded=false;
//	
//	char c;
//	while(recv(m_socket,&c,1,0)){}
	closesocket(m_socket);
	return true;
}

bool HttpMessenger::SetTimeout(int nTime, int nType)
{
	if(nType==0)
	{
		nType=SO_RCVTIMEO;
	}
	else
	{
		nType=SO_SNDTIMEO;
	}

	DWORD dwErr;

	dwErr=setsockopt(m_socket,SOL_SOCKET,nType,(char*)&nTime,sizeof(nTime)); 
	if(dwErr)
	{
		m_Error=m_Error+"设置Socket选项失败!\n";
		return false;
	}
	return true;
}

long HttpMessenger::HextoInt(string str,int length)
{
	char *revstr=new char[length+1];
	int *num=new int[length];
	int count=1;
	long result=0;
	strncpy_s(revstr,length+1,str.c_str(),length);
	for(int i=length-1;i>=0;i--)
	{
		if ((revstr[i]>='0') && (revstr[i]<='9'))
		{
			num[i]=revstr[i]-48;
		}
		else if ((revstr[i]>='a') && (revstr[i]<='f'))
		{
			num[i]=revstr[i]-'a'+10;
		}
		else if ((revstr[i]>='A') && (revstr[i]<='F'))
		{
			num[i]=revstr[i]-'A'+10;
		}
		else
		{
			num[i]=0;
		}
		result=result+num[i]*count;
		count=count*16;
	}
	return result;
}

bool HttpMessenger::InitialSocket()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested=MAKEWORD( 2, 2 );
	err=WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) 
	{
//		PrintText("加载套接字失败!\r\n");
		return false;
	}

	if ( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 2 ) 
	{
//		PrintText("未找到指定版本的套接字!\r\n");
		WSACleanup( );
		return false; 
	}
	HttpMessenger::SocketLoaded=true;
	return true;
}

bool HttpMessenger::GetConnectionState()
{
	if(!m_bResponsed)
	{
		return false;
	}
	int nPos=-1,nCr1=-1,nCr2=-1;
	nPos=m_ResponseHeader.find("Connection:");
	if (nPos!=-1)
	{
		nCr1=m_ResponseHeader.find("keep-alive",nPos);
		nCr2=m_ResponseHeader.find("Keep-Alive",nPos);
		if ((nCr1!=-1)||(nCr2!=-1))
		{
			return true;
		}
	}
	return false;
}

int HttpMessenger::GetCookie(char *cookie,int buf_len)
{
	if(!m_bResponsed)
	{
		return 0;
	}
	ZeroMemory(cookie,buf_len);

	int nStart=-1,nEnd=-1;

	nStart=m_ResponseHeader.find("PHPSESSID=");
	if (nStart!=string::npos)
	{
		nEnd=m_ResponseHeader.find(";",nStart);
		if (nEnd!=string::npos&&buf_len>=nEnd-nStart)
		{
			memcpy(cookie,m_ResponseHeader.c_str()+nStart,nEnd-nStart);
			return nEnd-nStart;
		}
		else
		{
			return 0;
		}
	}
	return 0;
}
void HttpMessenger::UTF8ResponsetoANSI()
{
	string temp;
	UTF8ToANSI(m_ResponseText.c_str(),&temp);
	m_ResponseText.erase();
	m_ResponseText=temp;
	temp.erase();
}

