#include <Psapi.h>
#include <Tlhelp32.h>
#include <Wtsapi32.h>
#pragma comment (lib, "psapi.lib")
#pragma comment (lib, "Wtsapi32.lib")

void TaskList(void *cmd);
void TaskOperate(void *cmd);
int EnableDebugPrivilege();
void EnumProcess(Json::Value *tasks);
char* GetUserNameFromUserSid(PSID pUserSid);