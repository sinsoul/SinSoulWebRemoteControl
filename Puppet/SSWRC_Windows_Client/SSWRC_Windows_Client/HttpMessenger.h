#include "windows.h"
#include <iostream>
#include <string>
//#include "winsock2.h"

#pragma once
#pragma comment(lib,"Ws2_32.lib")
using namespace std;
class HttpMessenger
{
public:
	HttpMessenger(char *szHostName="127.0.0.1",int nPort=80);
	virtual ~HttpMessenger()
	{
		MyCloseSocket();
	}
	string m_RequestHeader;
	string m_ResponseHeader;
	string m_ResponseText;
	unsigned int m_ContentLength;
	bool m_bResponsed;
	bool m_bConnected;
	bool Chunked;
	string HttpMessenger::GetErrorLog();
	bool CreateAndSendRequest(char *RequestType,char *ResourcePath,char *Host=NULL,char *PostData=NULL,bool OnlyGetLength=false,char *Save2File=NULL);
	bool SetTimeout(int nTime, int nType);
	int GetResponseState();
	long HextoInt(string str,int length);
	bool GetConnectionState();
	bool CreateConnection();
	void UTF8ResponsetoANSI();
	int GetCookie(char *cookie,int buf_len);
	static bool InitialSocket();
	static bool SocketLoaded;
protected:
	int ReceiveChunked();
	bool CreateSocket();
	long GetContentLength();
	unsigned long Receive(bool OnlyGetLength=false,char *Save2File=NULL);
	bool MyCloseSocket();
	int m_port;
	SOCKET m_socket;
	hostent *m_phostip;
	string m_Error;
	string server_addr;
};