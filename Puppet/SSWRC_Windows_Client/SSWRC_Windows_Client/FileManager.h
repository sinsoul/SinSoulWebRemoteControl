bool GetDirverInfo(Json::Value *root);
DWORD Enum_Current_Drectory(LPSTR szPath,Json::Value *root);
void Enum_Drectory(void *cmd);
void Enum_All_File_in_Drectory(void *cmd);
void FileOperate(void * cmd);
bool Delete_Drectory(const char *dirName);
void DelFileHiddenAndReadonly(const char *szFileName);