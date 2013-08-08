#include "MyStdAfx.h"

DWORD dwTotalFileNum = 0;
#define BUFSIZE 1024

DWORD Enum_Current_Drectory(LPSTR sz_Utf8_Path,Json::Value *root)
{
	Json::Value dir;
	Json::Value data;
	Json::Value attr;

	CHAR szFilePath[MAX_PATH];
	CHAR szFullPath[MAX_PATH];
	WIN32_FIND_DATA FindFileData;
	HANDLE hListFile;
	
	string szPath;
	string utf8_FullPath,utf8_FileName;

	UTF8ToANSI(sz_Utf8_Path,&szPath);

	if (szPath.c_str()[szPath.length()-1]!='\\')
	{
		sprintf_s(szFilePath,"%s\\*",szPath.c_str());
	}
	else
	{
		sprintf_s(szFilePath,"%s*",szPath.c_str());
	}

	hListFile = FindFirstFile(szFilePath,&FindFileData);
	if(hListFile==INVALID_HANDLE_VALUE)
	{
		Msg("开始遍历文件时出错：%d\r\n",GetLastError());
		return 1;
	}
	else
	{
		do
		{
			if(lstrcmp(FindFileData.cFileName,TEXT("."))==0||
				lstrcmp(FindFileData.cFileName,TEXT(".."))==0)
			{
				continue;
			}

			if (szPath.c_str()[szPath.length()-1]!='\\')
			{
				sprintf_s(szFullPath,"%s\\%s",szPath.c_str(),FindFileData.cFileName);
			}
			else
			{
				sprintf_s(szFullPath,"%s%s",szPath.c_str(),FindFileData.cFileName);
			}
			
			dwTotalFileNum++;

			utf8_FileName.erase();
			utf8_FullPath.erase();

			ANSIToUTF8(szFullPath,&utf8_FullPath);
			attr["path"]=Json::Value(utf8_FullPath.c_str());
			
			ANSIToUTF8(FindFileData.cFileName,&utf8_FileName);
			data["title"]=Json::Value(utf8_FileName.c_str());
			attr["title"]=Json::Value(utf8_FileName.c_str());
			dir["data"]=data;
			dir["attr"]=attr;
			if(FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			{
				dir["state"]=Json::Value("closed");
			}
			else
			{
				continue;
			}
			root->append(dir);
		}
		while(FindNextFile(hListFile, &FindFileData));
		FindClose(hListFile);
		//谨记MSDN的教导：
		//When the search handle is no longer needed, close it by using the FindClose function, not CloseHandle.
	}

	szPath.erase();
	utf8_FullPath.erase();
	utf8_FileName.erase();
	return 0;
}

bool GetDirverInfo(Json::Value *root)
{

	DWORD lInfoSize = GetLogicalDriveStrings(0, NULL);
	CHAR *szLogicalDriveStrings=new char[(lInfoSize+1)];
	PCHAR szDrive=NULL;

	ZeroMemory(szLogicalDriveStrings,lInfoSize+1);
	GetLogicalDriveStrings(lInfoSize,szLogicalDriveStrings);	
	szDrive = (PCHAR)szLogicalDriveStrings;
	do
	{
		Json::Value dir;
		Json::Value data;
		Json::Value attr;

		UINT uDriveType;
		DWORD dwVolumeSerialNumber;
		DWORD dwMaximumComponentLength;
		DWORD dwFileSystemFlags;
		CHAR szFileSystemNameBuffer[BUFSIZE];
		CHAR szDirveName[MAX_PATH];
		ULARGE_INTEGER nTotalBytes, nTotalFreeBytes, nTotalAvailable;  

		attr["path"]=Json::Value(szDrive);
		char vol_type[256];
		ZeroMemory(vol_type,256);
		uDriveType = GetDriveType(szDrive);
		switch(uDriveType)
		{
		case DRIVE_UNKNOWN:
			sprintf_s(vol_type,256,"磁盘类型：外星设备\r\n");
			break;
		case DRIVE_NO_ROOT_DIR:
			sprintf_s(vol_type,256,"磁盘类型：非根目录\r\n");
			break;
		case DRIVE_REMOVABLE:
			sprintf_s(vol_type,256,"磁盘类型：可移动磁盘\r\n");
			break;
		case DRIVE_FIXED:
			sprintf_s(vol_type,256,"磁盘类型：硬盘\r\n");
			break;
		case DRIVE_REMOTE:
			sprintf_s(vol_type,256,"磁盘类型：远程驱动器\r\n");
			break;
		case DRIVE_CDROM:
			sprintf_s(vol_type,256,"磁盘类型：光盘\r\n");
			break;
		case DRIVE_RAMDISK:
			sprintf_s(vol_type,256,"磁盘类型：内存盘\r\n");
			break;
		default: 
			break;
		}
		if(uDriveType == DRIVE_FIXED)  
		{ 
			char attr_title[1024];
			char total_size[512];
			char fee_size[256];
			if(GetDiskFreeSpaceEx(szDrive, &nTotalAvailable, &nTotalBytes, &nTotalFreeBytes))
			{
				ZeroMemory(total_size,512);
				ZeroMemory(fee_size,256);
				ZeroMemory(attr_title,1024);
				sprintf_s(total_size,512,"总大小：%ld MB\r\n",nTotalBytes.QuadPart/1024/1024);
				sprintf_s(fee_size,256,"可用空间：%ld MB\r\n",nTotalFreeBytes.QuadPart/1024/1024);
				sprintf_s(attr_title,1024,"%s%s%s",total_size,fee_size,vol_type);
				attr["title"]=Json::Value(attr_title);
			}  
		} 
		if (!GetVolumeInformation(szDrive,szDirveName,MAX_PATH,&dwVolumeSerialNumber,&dwMaximumComponentLength,
			&dwFileSystemFlags,szFileSystemNameBuffer,BUFSIZE))
		{
			data["title"]=Json::Value(szDrive);
			attr["title"]=Json::Value("设备未就绪");
			dir["data"]=data;
			dir["attr"]=attr;
			dir["state"]=Json::Value("");
			root->append(dir);
			szDrive += (lstrlen(szDrive)+1);
			continue;
		}
		if(0!=lstrlen(szDirveName))
		{
			data["title"]=Json::Value(szDirveName);
		}
		else
			{
				data["title"]=Json::Value(szDrive);
			}
		szDrive += (lstrlen(szDrive)+1);
		dir["data"]=data;
		dir["attr"]=attr;
		dir["state"]=Json::Value("closed");
		root->append(dir);
	}
	while(*szDrive!='\x00');
	delete []szLogicalDriveStrings;

	return true;
}
void FileOperate(void * cmd)
{
	cmdinfo *pci=(cmdinfo*)cmd;
	Json::Value json_cmd_content;
	Json::Reader reader;
	Json::FastWriter fast_writer;
	string Result,Base64Result;

	if (reader.parse(pci->cmd_content.c_str(), json_cmd_content))
	{
		string operate=fast_writer.write(json_cmd_content["operate"]);
		if (operate.find("rename")!=string::npos)
		{
			string oldName,newName;
			PathDecode(json_cmd_content["oldName"].asString(),oldName);
			PathDecode(json_cmd_content["newName"].asString(),newName);
			//Msg("操作:重命名\r\n");
			//Msg("原名称:%s\r\n",oldName.c_str());
			//Msg("新名称:%s\r\n",newName.c_str());

			if(!MoveFile(oldName.c_str(),newName.c_str()))
			{
				LPSTR lpBuffer=NULL;    
				FormatMessage ( FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_IGNORE_INSERTS|FORMAT_MESSAGE_FROM_SYSTEM,
												NULL,GetLastError(),LANG_NEUTRAL,(LPTSTR) & lpBuffer,0 ,NULL);
				if(lpBuffer==NULL)
				{
					ANSIToUTF8("Rename File Error.",&Result);
				}
				Result+=lpBuffer;
				LocalFree (lpBuffer);
			}
			else
			{
				ANSIToUTF8("Complete",&Result);
			}

		}
		if (operate.find("newdir")!=string::npos)
		{
			string newDirPath;
			PathDecode(json_cmd_content["newDirPath"].asString(),newDirPath);
//			Msg("操作:新建文件夹:%s\r\n",newDirPath.c_str());
			if (!CreateDirectory(newDirPath.c_str(), NULL))
			{
				LPSTR lpBuffer=NULL;    
				FormatMessage ( FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_IGNORE_INSERTS|FORMAT_MESSAGE_FROM_SYSTEM,
					NULL,GetLastError(),LANG_NEUTRAL,(LPTSTR) & lpBuffer,0 ,NULL);
				if(lpBuffer==NULL)
				{
					ANSIToUTF8("Rename File Error.",&Result);
				}
				Result+=lpBuffer;
				LocalFree (lpBuffer);
			}
			else
			{
				ANSIToUTF8("Complete",&Result);
			}
		}
		if (operate.find("delete")!=string::npos)
		{
			Json::Value filelist,dirlist;
			filelist=json_cmd_content["filelist"];
			dirlist=json_cmd_content["dirlist"];

			int iSize = filelist.size();
			string temp;
			for (int nIndex=0;nIndex<iSize;++nIndex )
			{
				temp.erase();
				PathDecode(filelist[nIndex]["file"].asString(),temp);
//				Msg("文件:%s\r\n",temp.c_str());
				DelFileHiddenAndReadonly(temp.c_str());
				if (!DeleteFile(temp.c_str()))
				{
					LPSTR lpBuffer=NULL;    
					FormatMessage ( FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_IGNORE_INSERTS|FORMAT_MESSAGE_FROM_SYSTEM,
						NULL,GetLastError(),LANG_NEUTRAL,(LPTSTR) & lpBuffer,0 ,NULL);
					if(lpBuffer==NULL)
					{
						Result=Result+"删除文件\""+temp.c_str()+"\"时出错,原因:未知.\n";
					}
					Result=Result+"删除文件\""+temp.c_str()+"\"时出错,原因:"+lpBuffer;
					LocalFree (lpBuffer);
				}
				else
				{
					Result=Result+"删除文件\""+temp.c_str()+"\"完成\n";
				}
			}
			iSize=dirlist.size();
			for (int nIndex=0;nIndex<iSize;++nIndex )
			{
				temp.erase();
				PathDecode(dirlist[nIndex]["dir"].asString(),temp);
	//			Msg("目录:%s\r\n",temp.c_str());
				if (Delete_Drectory(temp.c_str()))
				{
					Result=Result+"删除目录\""+temp.c_str()+"\"完成\n";
				}
				else
				{
					Result=Result+"删除目录\""+temp.c_str()+"\"失败\n";
				}
			}
		}
		if (operate.find("Copy")!=string::npos)
		{
			string temp_src,temp_dest;
			Json::Value src,dest;
			src=json_cmd_content["Source"];
			dest=json_cmd_content["Destination"];
			int iSize = src.size();
			if (iSize!=dest.size())
			{
				Result+="Json Error";
			}
			for (int nIndex=0;nIndex<iSize;++nIndex )
			{
				temp_src.erase();
				temp_dest.erase();
				PathDecode(src[nIndex]["src"].asString(),temp_src);
				PathDecode(dest[nIndex]["dest"].asString(),temp_dest);
	//			Msg("从\"%s\"到\"%s\"\r\n",temp_src.c_str(),temp_dest.c_str());
				DelFileHiddenAndReadonly(temp_src.c_str());
				if(!CopyFile(temp_src.c_str(),temp_dest.c_str(),FALSE))
				{
					LPSTR lpBuffer=NULL;    
					FormatMessage ( FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_IGNORE_INSERTS|FORMAT_MESSAGE_FROM_SYSTEM,
						NULL,GetLastError(),LANG_NEUTRAL,(LPTSTR) & lpBuffer,0 ,NULL);
					if(lpBuffer==NULL)
					{
						Result=Result+"复制文件\""+temp_src.c_str()+"\"时出错,原因:未知.\n";
					}
					Result=Result+"复制文件\""+temp_src.c_str()+"\"时出错,原因:"+lpBuffer;
					LocalFree (lpBuffer);
				}
				else
				{
					Result=Result+"复制文件\""+temp_src.c_str()+"\"完成\n";
				}

			}
		}
		if (operate.find("Cut")!=string::npos)
		{
			string temp_src,temp_dest;
			Json::Value src,dest;
			src=json_cmd_content["Source"];
			dest=json_cmd_content["Destination"];
			int iSize = src.size();
			if (iSize!=dest.size())
			{
				Result+="Json Error";
			}
			for (int nIndex=0;nIndex<iSize;++nIndex )
			{
				temp_src.erase();
				temp_dest.erase();
				PathDecode(src[nIndex]["src"].asString(),temp_src);
				PathDecode(dest[nIndex]["dest"].asString(),temp_dest);
	//			Msg("从\"%s\"到\"%s\"\r\n",temp_src.c_str(),temp_dest.c_str());
				DelFileHiddenAndReadonly(temp_src.c_str());
				if(!MoveFile(temp_src.c_str(),temp_dest.c_str()))
				{
					LPSTR lpBuffer=NULL;    
					FormatMessage ( FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_IGNORE_INSERTS|FORMAT_MESSAGE_FROM_SYSTEM,
						NULL,GetLastError(),LANG_NEUTRAL,(LPTSTR) & lpBuffer,0 ,NULL);
					if(lpBuffer==NULL)
					{
						Result=Result+"移动文件\""+temp_src.c_str()+"\"时出错,原因:未知.\n";
					}
					Result=Result+"移动文件\""+temp_src.c_str()+"\"时出错,原因:"+lpBuffer;
					LocalFree (lpBuffer);
				}
				else
				{
					Result=Result+"移动文件\""+temp_src.c_str()+"\"完成\n";
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
void Enum_Drectory(void *cmd)
{
	Json::FastWriter fast_writer;
	Json::Value root;
	string Base64Result;
	cmdinfo *pci=(cmdinfo*)cmd;
	if (pci->cmd_content.compare("/")==0)
	{
		string utf8_drive;
		//Msg("获取驱动器\r\n");
		GetDirverInfo(&root);

		ANSIToUTF8(fast_writer.write(root).c_str(),&utf8_drive);
		Base64Encode(reinterpret_cast<const unsigned char*>(utf8_drive.c_str()),utf8_drive.length(),Base64Result);
		Replace_plus(Base64Result);
		SubmitResult(pci,Base64Result.c_str());
		Base64Result.erase();
		utf8_drive.erase();
		root.clear();
	}
	else
	{
		//Msg("获取文件夹列表\r\n");
		Enum_Current_Drectory((char *)pci->cmd_content.c_str(),&root);
		Base64Encode(reinterpret_cast<const unsigned char*>(fast_writer.write(root).c_str()),fast_writer.write(root).length(),Base64Result);
		Replace_plus(Base64Result);
		SubmitResult(pci,Base64Result.c_str());
		Base64Result.erase();
		root.clear();
	}
}

void Enum_All_File_in_Drectory(void *cmd)
{
	Json::FastWriter fast_writer;
	Json::Value dir;
	Json::Value file;

	string Base64Result;
	cmdinfo *pci=(cmdinfo*)cmd;

	CHAR szFilePath[MAX_PATH];
	CHAR szFullPath[MAX_PATH];
	WIN32_FIND_DATA FindFileData;
	HANDLE hListFile;

	string szPath;
	string utf8_FullPath,utf8_FileName;

	//file["title"]=Json::Value("Dir1");
	//file["path"]=Json::Value("C:\\Dir1");
	//file["icon"]=Json::Value("dir.png");
	//file["name"]=Json::Value("Dir1");

	UTF8ToANSI((char *)pci->cmd_content.c_str(),&szPath);
	if (szPath.c_str()[szPath.length()-1]!='\\')
	{
		sprintf_s(szFilePath,"%s\\*",szPath.c_str());
	}
	else
	{
		sprintf_s(szFilePath,"%s*",szPath.c_str());
	}
	hListFile = FindFirstFile(szFilePath,&FindFileData);
	if(hListFile==INVALID_HANDLE_VALUE)
	{
		Msg("开始遍历文件时出错：%d\r\n",GetLastError());
	}
	else
	{
		do
		{
			if(lstrcmp(FindFileData.cFileName,TEXT("."))==0||
				lstrcmp(FindFileData.cFileName,TEXT(".."))==0)
			{
				continue;
			}
			if (szPath.c_str()[szPath.length()-1]!='\\')
			{
				sprintf_s(szFullPath,"%s\\%s",szPath.c_str(),FindFileData.cFileName);
			}
			else
			{
				sprintf_s(szFullPath,"%s%s",szPath.c_str(),FindFileData.cFileName);
			}
			dwTotalFileNum++;

			utf8_FileName.erase();

			ANSIToUTF8(FindFileData.cFileName,&utf8_FileName);

			file["title"]=Json::Value(utf8_FileName.c_str());
			file["name"]=Json::Value(utf8_FileName.c_str());


			if(FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			{
				//			Msg("<DIR>");
				file["icon"]=Json::Value("grid_dir.png");
			}
			else
			{
				file["icon"]=Json::Value("file.png");
			}
			dir["dir"].append(file);
		}
		while(FindNextFile(hListFile, &FindFileData));
		FindClose(hListFile);
	}
	szPath.erase();
	utf8_FileName.erase();
	Base64Encode(reinterpret_cast<const unsigned char*>((char *)fast_writer.write(dir).c_str()),fast_writer.write(dir).length(),Base64Result);
	Replace_plus(Base64Result);
	SubmitResult(pci,Base64Result.c_str());
	Base64Result.erase();
	dir.clear();
	
}

bool Delete_Drectory(const char *dirName)
{
	CHAR szFilePath[MAX_PATH];
	CHAR szFullPath[MAX_PATH];
	WIN32_FIND_DATA FindFileData;
	HANDLE hListFile;
	sprintf_s(szFilePath,"%s\\*",dirName);
	hListFile = FindFirstFile(szFilePath,&FindFileData);
	if(hListFile==INVALID_HANDLE_VALUE)
	{
		Msg("开始删除文件时出错：%d\r\n",GetLastError());
	}
	else
	{
		do
		{
			if(lstrcmp(FindFileData.cFileName,TEXT("."))==0||
				lstrcmp(FindFileData.cFileName,TEXT(".."))==0)
			{
				continue;
			}
			sprintf_s(szFullPath,"%s\\%s",dirName,FindFileData.cFileName);
			if(FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			{
				Delete_Drectory(szFullPath);
			}
			else
			{
	//			Msg("删除:%s\r\n",szFullPath);
				DelFileHiddenAndReadonly(szFullPath);
				DeleteFile(szFullPath);
			}
		}
		while(FindNextFile(hListFile, &FindFileData));
		FindClose(hListFile);
		if(!RemoveDirectory(dirName))
		{
			return false;
		}
		return true;
	}
	return false;
}

void DelFileHiddenAndReadonly(const char *szFileName)
{
	DWORD dwFileAttributes = GetFileAttributes(szFileName);
	dwFileAttributes &= FILE_ATTRIBUTE_READONLY;
	dwFileAttributes &= FILE_ATTRIBUTE_HIDDEN;
	dwFileAttributes &= FILE_ATTRIBUTE_SYSTEM;
	SetFileAttributes(szFileName, dwFileAttributes);
}