#include "MyStdAfx.h"

void LeftClick(int x,int y)
{
	//×ó¼üµ¥»÷
	SetCursorPos(x,y);
	mouse_event(MOUSEEVENTF_LEFTDOWN,x,y,0,0);
	mouse_event(MOUSEEVENTF_LEFTUP,x,y,0,0);
}
void LeftDoubleClick(int x,int y)
{	
	//×ó¼üË«»÷
	SetCursorPos(x,y);
	mouse_event(MOUSEEVENTF_LEFTDOWN,x,y,0,0);
	mouse_event(MOUSEEVENTF_LEFTUP,x,y,0,0);
	mouse_event(MOUSEEVENTF_LEFTDOWN,x,y,0,0);
	mouse_event(MOUSEEVENTF_LEFTUP,x,y,0,0);
}
void RightClick(int x,int y)
{
	//ÓÒ¼üµ¥»÷
	SetCursorPos(x,y);
	mouse_event(MOUSEEVENTF_RIGHTDOWN,x,y,0,0);
	mouse_event(MOUSEEVENTF_RIGHTUP,x,y,0,0);
}

void KeyDown(BYTE bVK)
{
	if(bVK>=97&&bVK<=122)
		bVK=bVK-32;
	keybd_event(bVK,0,0,0);
}

void KeyUp(BYTE bVK)
{
	if(bVK>=97&&bVK<=122)
		bVK=bVK-32;
	keybd_event(bVK,0,KEYEVENTF_KEYUP,0);
}