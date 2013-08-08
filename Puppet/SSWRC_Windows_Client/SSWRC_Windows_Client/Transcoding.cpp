#include "MyStdAfx.h"

#define BUFF_SIZE 1024

void ANSIToUTF8(const char* ansi_str,string *utf8_result)
{
	long unicode_len ;
	wchar_t * unicode_buf;

	unicode_len = MultiByteToWideChar( CP_ACP, 0, ansi_str,-1, NULL,0 );
	unicode_buf = (wchar_t *)malloc((unicode_len+1)*sizeof(wchar_t));
	memset(unicode_buf,0,(unicode_len+1)*sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0,ansi_str,-1,(LPWSTR)unicode_buf,unicode_len);

	long utf8_len = WideCharToMultiByte( CP_UTF8, 0,unicode_buf, -1, NULL, 0, NULL, NULL );
	char *utf8_buf =(char *)malloc((utf8_len+1)*sizeof(char));
	memset(utf8_buf, 0, sizeof(char) * ( utf8_len + 1 ) );
	WideCharToMultiByte( CP_UTF8, 0, unicode_buf, -1, utf8_buf, utf8_len, NULL, NULL );

	*utf8_result+=utf8_buf;
	free(unicode_buf);
	free(utf8_buf);
}


void UTF8ToANSI(const char* utf8_str,string *ansi_result)
{
	int unicode_len ;
	wchar_t * unicode_buf;
	unicode_len = MultiByteToWideChar( CP_UTF8, 0, utf8_str,-1, NULL,0 );
	unicode_buf = (wchar_t *)malloc((unicode_len+1)*sizeof(wchar_t));
	memset(unicode_buf,0,(unicode_len+1)*sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8, 0,utf8_str,-1,(LPWSTR)unicode_buf,unicode_len);

	char* ansi_buf;
	int ansi_len;
	ansi_len = WideCharToMultiByte( CP_ACP, 0, unicode_buf, -1, NULL, 0, NULL, NULL );
	ansi_buf =(char *)malloc((ansi_len+1)*sizeof(char));
	memset( ansi_buf, 0, sizeof(char) * ( ansi_len + 1 ) );
	WideCharToMultiByte( CP_ACP, 0, unicode_buf, -1, ansi_buf, ansi_len, NULL, NULL );

	*ansi_result+=ansi_buf;
	free(unicode_buf);
	free(ansi_buf);
}