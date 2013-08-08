#pragma once
#include <Gdiplus.h>
#include <GdiPlusEnums.h>
#pragma comment(lib,"gdiplus") 
#pragma warning(disable:4244)

extern CLSID codecClsid;
extern Gdiplus::GdiplusStartupInput gdiplusStartupInput;
extern ULONG gdiplusToken;
void ScreenCapture(void *cmd);
int GetCodecClsid(const WCHAR* format, CLSID* pClsid);
void ConvertAndSubmitBMP(HGLOBAL *lpAddress,cmdinfo *pci);