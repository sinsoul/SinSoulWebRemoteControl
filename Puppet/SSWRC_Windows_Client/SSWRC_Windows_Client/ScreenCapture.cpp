#include "MyStdAfx.h"

WCHAR * BStr2WStr(const char *pSource, int &iDstLen)
{
	iDstLen = MultiByteToWideChar(0, 0, pSource, (int)strlen(pSource), NULL, 0);
	WCHAR *pWDst = new WCHAR[iDstLen + 1];
	memset(pWDst, 0, sizeof(WCHAR) * (iDstLen + 1));
	MultiByteToWideChar(0, 0, pSource, (int)strlen(pSource), pWDst, iDstLen);
	return pWDst;
}

int GetCodecClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT num = 0;
	UINT size = 0;
	Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

	Gdiplus::GetImageEncodersSize(&num, &size);
	if(size == 0)
	{
		return -1;
	}
	pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL)
	{
		return -1;
	}

	GetImageEncoders(num, size, pImageCodecInfo);

	for(UINT j = 0; j < num; ++j)
	{
		if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;
		}
	}
	free(pImageCodecInfo);
	return -1;
}
static int frame_count=0;
void ScreenCapture(void *cmd)
{
	Msg("开始截图...");
	frame_count++;
	cmdinfo *pci=(cmdinfo*)cmd;
	
	DWORD file_lenth=0;
	HGLOBAL lpAddress=NULL;
	BYTE* lpData=NULL;
	DWORD dwLen=0;
	int bgotbits=0;
	HDC hdc,CompatibleHDC;
	HBITMAP BmpScreen;
	BITMAP bm;
	HPALETTE hPal;

	hdc = CreateDC("DISPLAY", NULL, NULL, NULL); 

	CompatibleHDC = CreateCompatibleDC(hdc); 
	BmpScreen = CreateCompatibleBitmap(hdc,GetDeviceCaps(hdc, HORZRES),GetDeviceCaps(hdc, VERTRES)); 
	SelectObject(CompatibleHDC, BmpScreen);
	BitBlt(CompatibleHDC,0,0,GetDeviceCaps(hdc, HORZRES), GetDeviceCaps(hdc, VERTRES),hdc,0,0,SRCCOPY);

	//POINT point;
	//GetCursorPos(&point);
	//HICON hicon = (HICON)GetCursor();
	//DrawIcon(CompatibleHDC,(int)point.x,(int)point.y,hicon);

	hPal=(HPALETTE) GetStockObject(DEFAULT_PALETTE);
	GetObject(BmpScreen,sizeof(bm),(LPSTR)&bm);

	DWORD size=bm.bmWidthBytes*bm.bmHeight;

	BITMAPINFO *bInfo=new BITMAPINFO;
	bInfo->bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	bInfo->bmiHeader.biWidth=bm.bmWidth;
	bInfo->bmiHeader.biHeight=bm.bmHeight;
	bInfo->bmiHeader.biPlanes=1;
	bInfo->bmiHeader.biBitCount=bm.bmPlanes * bm.bmBitsPixel;
	bInfo->bmiHeader.biCompression=BI_RGB;
	bInfo->bmiHeader.biSizeImage=bInfo->bmiHeader.biSizeImage = ((((bInfo->bmiHeader.biWidth * bInfo->bmiHeader.biBitCount) + 31) & ~31) / 8)* bInfo->bmiHeader.biHeight;
	bInfo->bmiHeader.biXPelsPerMeter=0;
	bInfo->bmiHeader.biYPelsPerMeter=0;
	bInfo->bmiHeader.biClrUsed=0;
	bInfo->bmiHeader.biClrImportant=0;

	//BITMAPINFOHEADER bi;
	//bi.biSize=sizeof(BITMAPINFOHEADER);
	//bi.biWidth=bm.bmWidth;
	//bi.biHeight=bm.bmHeight;
	//bi.biPlanes=1;
	//bi.biBitCount=bm.bmPlanes * bm.bmBitsPixel;
	//bi.biCompression=BI_RGB;
	//bi.biSizeImage=bi.biSizeImage = ((((bi.biWidth * bi.biBitCount) + 31) & ~31) / 8)* bi.biHeight;
	//bi.biXPelsPerMeter=0;
	//bi.biYPelsPerMeter=0;
	//bi.biClrUsed=0;
	//bi.biClrImportant=0;

	//bInfo.bmiHeader = bi;

	int ncolors=(1<< bInfo->bmiHeader.biBitCount);
	if( ncolors>256)
	{
		ncolors = 0;
	}

	dwLen = bInfo->bmiHeader.biSize + ncolors * sizeof(RGBQUAD)+bInfo->bmiHeader.biSizeImage;
	lpData = new BYTE[dwLen];
	Msg("第%d帧...",frame_count);
	bgotbits=GetDIBits(CompatibleHDC,BmpScreen,0,bInfo->bmiHeader.biHeight,lpData,bInfo,DIB_RGB_COLORS);
	if( !bgotbits )
	{
		Msg("截图失败:%d\n",GetLastError());
		delete [] lpData;
		SelectPalette(hdc,hPal,FALSE);
		ReleaseDC(NULL,hdc);
		return;
	}
	DeleteObject(BmpScreen);
	BITMAPFILEHEADER bfh;
	bfh.bfType = ((WORD)('M'<< 8)|'B');
	bfh.bfReserved1 = 0;
	bfh.bfReserved2 = 0;
	bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bfh.bfSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+dwLen;

	file_lenth=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+dwLen;
	lpAddress=GlobalAlloc(GMEM_MOVEABLE,file_lenth);
	if(lpAddress == NULL)
	{
		Msg("GlobalAlloc error: %d\r\n");
		return;
	}
	byte *  pbLocked = (BYTE*) GlobalLock(lpAddress); 
	if (pbLocked==NULL)
	{
		Msg("GlobalLock error: %d\r\n");
		return;
	}
	memcpy(pbLocked,&bfh,sizeof(BITMAPFILEHEADER));
	memcpy(pbLocked+(sizeof(BITMAPFILEHEADER)),&bInfo->bmiHeader,sizeof(BITMAPINFOHEADER));
	memcpy(pbLocked+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER),lpData,dwLen);
	GlobalUnlock(lpAddress);
	delete bInfo;
	SelectPalette(hdc,hPal,FALSE);
	ReleaseDC(NULL,hdc);
	ReleaseDC(NULL,CompatibleHDC);
	delete [] lpData;
	DeleteDC(hdc);
	DeleteDC(CompatibleHDC);
	Msg("截图完成...");
/////////////////////////截图完成,格式转换//////////////////////////////////////////////////
	ConvertAndSubmitBMP(&lpAddress,pci);
	GlobalFree(lpAddress);


	//从截屏线程启动到完成需要300毫秒
////////////////////////转换完成///////////////////////////////////////////////////////////
}

//ConvertAndSubmitBMP(lpAddress,pci->cmd_id);

CLSID codecClsid;
Gdiplus::GdiplusStartupInput gdiplusStartupInput;
ULONG gdiplusToken;	//这些在初始化函数中进行赋值

void ConvertAndSubmitBMP(HGLOBAL *lpAddress,cmdinfo *pci)
{
	string Base64Result;
	Gdiplus::EncoderParameters encoderParameters;

	DWORD iQuality=50;
	IStream *bmp_stream = NULL;
	IStream *jpg_stream = NULL;
	LARGE_INTEGER isBegin = {0};
	STATSTG stg={0};
	ULONG u_read=0;

	encoderParameters.Count = 1;
	encoderParameters.Parameter[0].Guid = Gdiplus::EncoderQuality;
	encoderParameters.Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
	encoderParameters.Parameter[0].NumberOfValues = 1;
	encoderParameters.Parameter[0].Value = &iQuality;
	Msg("开始转换...");
	if( CreateStreamOnHGlobal( *lpAddress, TRUE, &bmp_stream ) != S_OK )
	{
		PrintText("创建BMP流错误\r\n");
		return;
	}

	HGLOBAL lpjpgAddr=GlobalAlloc(GMEM_MOVEABLE,0);
	if(lpjpgAddr == NULL)
	{
		PrintText("GlobalAlloc error: %d\r\n");
		return;
	}

	if( CreateStreamOnHGlobal( lpjpgAddr, TRUE, &jpg_stream ) != S_OK )
	{
		PrintText("创建JPG流错误\r\n");
		return;
	}
	Gdiplus::Image *pImage = new Gdiplus::Image(bmp_stream);

	Gdiplus::Image *p_small_Image=pImage->GetThumbnailImage(pImage->GetWidth()/1.5,pImage->GetHeight()/1.5);

	//	Gdiplus::Status bStatus = pImage->Save(jpg_stream, &codecClsid, &encoderParameters);

	Gdiplus::Status bStatus = p_small_Image->Save(jpg_stream, &codecClsid, &encoderParameters);

//	Gdiplus::Status bStatus = pImage->Save(jpg_stream, &codecClsid, &encoderParameters);
	jpg_stream->Stat(&stg,STATFLAG_NONAME);
	char *jpgfile=new char[stg.cbSize.LowPart];
	jpg_stream->Seek(isBegin,STREAM_SEEK_SET,NULL);
	jpg_stream->Read(jpgfile,stg.cbSize.LowPart,&u_read);

	//DWORD dwWrite;
	//HANDLE  hFile=CreateFile("c:\\MEM_JPG.jpg",GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
	//WriteFile(hFile,jpgfile,stg.cbSize.LowPart,&dwWrite,NULL);
	//CloseHandle(hFile);

	Base64Encode(reinterpret_cast<const unsigned char*>(jpgfile),stg.cbSize.LowPart,Base64Result);
	Replace_plus(Base64Result);
	Msg("转换完成...\r\n");
	SubmitResult(pci,Base64Result.c_str());
	Base64Result.erase();

	bmp_stream->Release();
	//GlobalFree(lpAddress);
	jpg_stream->Release();
	GlobalFree(lpjpgAddr);
	delete pImage;
	delete[] p_small_Image;
	delete[] jpgfile;
}