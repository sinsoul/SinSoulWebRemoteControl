#pragma once
#include "resource.h"
#include "HttpMessenger.h"
#include "Transcoding.h"
#include "SystemTraySDK.h"
#include "CommandHandler.h"
#include "DatabaseSystemDevelopment_GUI.h"
#include ".\JsonCppLib\json.h"
#include "FileManager.h"
#include "TaskManager.h"
#include "ScreenCapture.h"
#include "ScreenControl.h"
#include "DirectShowVideoCapture.h"
#include <Process.h>
#include <windows.h>
#include<time.h>




#if defined(_DEBUG) 
#pragma comment(lib,"libcmtd.lib")
#pragma comment(linker,"/NODEFAULTLIB:LIBCMT")
#endif

using namespace std;

