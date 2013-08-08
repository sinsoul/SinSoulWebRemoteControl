#pragma once

#define WM_CMDRCVD_MESSAGE (WM_USER+416)
#define WM_CMDPCSD_MESSAGE (WM_USER+417)
#define WM_ONLINE_MESSAGE (WM_USER+418)
//#define WM_CMDRCVD_MESSAGE (WM_USER+418)

LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK EditProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK SetServerAddr(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
void PrintText(char * szBuff);
char *U2A(LPCWSTR lpcwszStr);
wchar_t * A2U(LPCSTR lpcszStr);
bool GetThisUserName();
void Online(void *);
void Logout();

extern char szClassName[11 ];
extern char *wndName;
extern HWND hwndButton;
extern HWND hwndEdit;
extern HWND hwndLable_0;
extern HWND hwnd;
extern HWND hwndCheck;
extern bool conn_status;
extern CSystemTray TrayIcon;
extern HINSTANCE hinst;