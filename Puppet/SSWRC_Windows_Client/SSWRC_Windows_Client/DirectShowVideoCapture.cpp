#include "MyStdAfx.h"


#define REGISTER_FILTERGRAPH

DWORD g_dwGraphRegister=0;
BOOL CameraStatus=FALSE;
IVideoWindow  * g_pVW = NULL;
IMediaControl * g_pMC = NULL;
IMediaEventEx * g_pME = NULL;
IGraphBuilder * g_pGraph = NULL;
ICaptureGraphBuilder2 * g_pCapture = NULL;
PLAYSTATE g_psCurrent = Stopped;

IBaseFilter *pGrabberFilter = NULL;
ISampleGrabber *pGrabber = NULL;
HWND ghApp=0;

HRESULT CaptureVideo()
{
	HRESULT hr;
	IBaseFilter *pSrcFilter=NULL;
	hr = GetInterfaces();
	if (FAILED(hr))
	{
		Msg(TEXT("Failed to get video interfaces!  hr=0x%x"), hr);
		return hr;
	}

	hr = g_pCapture->SetFiltergraph(g_pGraph);
	if (FAILED(hr))
	{
		Msg(TEXT("Failed to set capture filter graph!  hr=0x%x"), hr);
		return hr;
	}
	hr = FindCaptureDevice(&pSrcFilter);
	if (FAILED(hr))
	{
		return hr;
	}
	hr = g_pGraph->AddFilter(pSrcFilter, L"Video Capture");
	if (FAILED(hr))
	{
		Msg(TEXT("Couldn't add the capture filter to the graph!  hr=0x%x\r\n\r\n") 
			TEXT("If you have a working video capture device, please make sure\r\n")
			TEXT("that it is connected and is not being used by another application.\r\n\r\n")
			TEXT("The sample will now close."), hr);
		pSrcFilter->Release();
		return hr;
	}

	hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER,IID_IBaseFilter, (void**) & pGrabberFilter);
	hr = g_pGraph->AddFilter(pGrabberFilter, L"Sample Grabber");
	pGrabberFilter->QueryInterface(IID_ISampleGrabber, (void**)&pGrabber);
	AM_MEDIA_TYPE mt;
	ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
	mt.majortype = MEDIATYPE_Video;
	mt.subtype = MEDIASUBTYPE_RGB24;
	hr = pGrabber->SetMediaType(&mt);
	hr = pGrabber->SetOneShot(FALSE);
	hr = pGrabber->SetBufferSamples(TRUE);
	hr = g_pCapture->RenderStream (&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video,pSrcFilter, pGrabberFilter, NULL);
	if (FAILED(hr))
	{
		Msg(TEXT("Couldn't render the video capture stream.  hr=0x%x\r\n")
			TEXT("The capture device may already be in use by another application.\r\n\r\n")
			TEXT("The sample will now close."), hr);
		pSrcFilter->Release();
		return hr;
	}

	pSrcFilter->Release();

	hr = SetupVideoWindow();
	if (FAILED(hr))
	{
		Msg(TEXT("Couldn't initialize video window!  hr=0x%x"), hr);
		return hr;
	}

#ifdef REGISTER_FILTERGRAPH
	hr = AddGraphToRot(g_pGraph, &g_dwGraphRegister);
	if (FAILED(hr))
	{
		Msg(TEXT("Failed to register filter graph with ROT!  hr=0x%x"), hr);
		g_dwGraphRegister = 0;
	}
#endif
	hr = g_pMC->Run();
	if (FAILED(hr))
	{
		Msg(TEXT("Couldn't run the graph!  hr=0x%x"), hr);
		return hr;
	}
	g_psCurrent = Running;

	return S_OK;
}


HRESULT FindCaptureDevice(IBaseFilter ** ppSrcFilter)
{
	HRESULT hr;
	IBaseFilter * pSrc = NULL;
	CComPtr <IMoniker> pMoniker =NULL;
	ULONG cFetched;

	if (!ppSrcFilter)
	{
		return E_POINTER;
	}

	CComPtr <ICreateDevEnum> pDevEnum =NULL;
	hr = CoCreateInstance (CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC,IID_ICreateDevEnum, (void **) &pDevEnum);
	if (FAILED(hr))
	{
		Msg(TEXT("Couldn't create system enumerator!  hr=0x%x"), hr);
		return hr;
	}

	CComPtr <IEnumMoniker> pClassEnum = NULL;
	hr = pDevEnum->CreateClassEnumerator (CLSID_VideoInputDeviceCategory, &pClassEnum, 0);
	if (FAILED(hr))
	{
		Msg(TEXT("Couldn't create class enumerator!  hr=0x%x"), hr);
		return hr;
	}

	if (pClassEnum == NULL)
	{
		PrintText("未找到摄像设备\r\n");
		return E_FAIL;
	}

	if (S_OK == (pClassEnum->Next (1, &pMoniker, &cFetched)))
	{
		hr = pMoniker->BindToObject(0,0,IID_IBaseFilter, (void**)&pSrc);
		if (FAILED(hr))
		{
			Msg(TEXT("Couldn't bind moniker to filter object!  hr=0x%x"), hr);
			return hr;
		}
	}
	else
	{
		Msg(TEXT("Unable to access video capture device!"));   
		return E_FAIL;
	}

	*ppSrcFilter = pSrc;
	return hr;
}


HRESULT GetInterfaces(void)
{
	HRESULT hr;

	hr = CoCreateInstance (CLSID_FilterGraph, NULL, CLSCTX_INPROC,IID_IGraphBuilder, (void **) &g_pGraph);
	if (FAILED(hr))
	{
		return hr;
	}

	hr = CoCreateInstance (CLSID_CaptureGraphBuilder2 , NULL, CLSCTX_INPROC,IID_ICaptureGraphBuilder2, (void **) &g_pCapture);
	if (FAILED(hr))
	{
		return hr;
	}

	hr = g_pGraph->QueryInterface(IID_IMediaControl,(LPVOID *) &g_pMC);
	if (FAILED(hr))
	{
		return hr;
	}

	hr = g_pGraph->QueryInterface(IID_IVideoWindow, (LPVOID *) &g_pVW);
	if (FAILED(hr))
	{
		return hr;
	}

	hr = g_pGraph->QueryInterface(IID_IMediaEvent, (LPVOID *) &g_pME);
	if (FAILED(hr))
	{
		return hr;
	}

	hr = g_pME->SetNotifyWindow((OAHWND)ghApp, WM_GRAPHNOTIFY, 0);
	return hr;
}


void CloseInterfaces(void)
{

	if (g_pMC)
	{
		g_pMC->StopWhenReady();
	}
	g_psCurrent = Stopped;

	if (g_pME)
	{
		g_pME->SetNotifyWindow(NULL, WM_GRAPHNOTIFY, 0);
	}

	if(g_pVW)
	{
		g_pVW->put_Visible(OAFALSE);
		g_pVW->put_Owner(NULL);
	}

#ifdef REGISTER_FILTERGRAPH
	if (g_dwGraphRegister)
		RemoveGraphFromRot(g_dwGraphRegister);
#endif
	SAFE_RELEASE(g_pMC);
	SAFE_RELEASE(g_pME);
	SAFE_RELEASE(g_pVW);
	SAFE_RELEASE(g_pGraph);
	SAFE_RELEASE(g_pCapture);
}


HRESULT SetupVideoWindow(void)
{
	HRESULT hr;
	hr = g_pVW->put_Owner((OAHWND)ghApp);
	if (FAILED(hr))
	{
		return hr;
	}
	hr = g_pVW->put_WindowStyle(WS_CHILD | WS_CLIPCHILDREN);
	if (FAILED(hr))
	{
		return hr;
	}

	ResizeVideoWindow();

	hr = g_pVW->put_Visible(OATRUE);
	if (FAILED(hr))
	{
		return hr;
	}
	return hr;
}


void ResizeVideoWindow(void)
{
	if (g_pVW)
	{
		//RECT rc;
		//GetClientRect(ghApp, &rc);
		g_pVW->SetWindowPosition(0, 0, 50,40);
	}
}


HRESULT ChangePreviewState(int nShow)
{
	HRESULT hr=S_OK;

	if (!g_pMC)
	{
		return S_OK;
	}
	if (nShow)
	{
		if (g_psCurrent != Running)
		{
			hr = g_pMC->Run();
			g_psCurrent = Running;
		}
	}
	else
	{
		hr = g_pMC->StopWhenReady();
		g_psCurrent = Stopped;
	}
	return hr;
}


#ifdef REGISTER_FILTERGRAPH

HRESULT AddGraphToRot(IUnknown *pUnkGraph, DWORD *pdwRegister) 
{
	IMoniker * pMoniker;
	IRunningObjectTable *pROT;
	WCHAR wsz[128];
	HRESULT hr;

	if (!pUnkGraph || !pdwRegister)
	{
		return E_POINTER;
	}
	if (FAILED(GetRunningObjectTable(0, &pROT)))
	{
		return E_FAIL;
	}
	hr = StringCchPrintfW(wsz, NUMELMS(wsz), L"FilterGraph %08x pid %08x\0", (DWORD_PTR)pUnkGraph, GetCurrentProcessId());

	hr = CreateItemMoniker(L"!", wsz, &pMoniker);
	if (SUCCEEDED(hr)) 
	{

		hr = pROT->Register(ROTFLAGS_REGISTRATIONKEEPSALIVE, pUnkGraph, pMoniker, pdwRegister);
		pMoniker->Release();
	}
	pROT->Release();
	return hr;
}

void RemoveGraphFromRot(DWORD pdwRegister)
{
	IRunningObjectTable *pROT;

	if (SUCCEEDED(GetRunningObjectTable(0, &pROT))) 
	{
		pROT->Revoke(pdwRegister);
		pROT->Release();
	}
}
#endif

void Msg(TCHAR *szFormat, ...)
{
	TCHAR szBuffer[1024];
	const size_t NUMCHARS = sizeof(szBuffer) / sizeof(szBuffer[0]);
	const int LASTCHAR = NUMCHARS - 1;
	va_list pArgs;
	va_start(pArgs, szFormat);
	(void)StringCchVPrintf(szBuffer, NUMCHARS - 1, szFormat, pArgs);
	va_end(pArgs);
	szBuffer[LASTCHAR] = TEXT('\0');

	PrintText((char *)szBuffer);
	//MessageBox(NULL, szBuffer, TEXT("PlayCap Message"), MB_OK | MB_ICONERROR);
}


HRESULT HandleGraphEvent(void)
{
	LONG evCode;
	LONG_PTR evParam1, evParam2;
	HRESULT hr=S_OK;
	if (!g_pME)
	{
		return E_POINTER;
	}
	while(SUCCEEDED(g_pME->GetEvent(&evCode, &evParam1, &evParam2, 0)))
	{
		hr = g_pME->FreeEventParams(evCode, evParam1, evParam2);
	}

	return hr;
}

void CaptureCurrentFrame(void * cmd)
{
	cmdinfo *pci=(cmdinfo*)cmd;
	if (!CameraStatus)
	{
		SubmitResult(pci,"Stoped"); 
		return ;
	}
	HRESULT hr;
	AM_MEDIA_TYPE mt;
	HDC hDC=NULL;
	DWORD dwDIBSize=0, dwWritten=0;
	BITMAPFILEHEADER bmfHdr;
	LPBITMAPINFOHEADER lpbi;
	HANDLE  hDib, hPal,hOldPal=NULL;
	HBITMAP hBitmap;
	void   *buffer   =   NULL;   
	long size = 0; 
	BITMAPINFO BitmapInfo;
	VIDEOINFOHEADER *pVih=NULL;
	hr = pGrabber->GetConnectedMediaType(&mt);
	if (FAILED (hr))
	{
		PrintText(TEXT("获取类型错误"));
	}
	pVih = (VIDEOINFOHEADER *)mt.pbFormat;
	ZeroMemory(&BitmapInfo, sizeof(BitmapInfo));  
	CopyMemory(&BitmapInfo.bmiHeader, &(pVih->bmiHeader),sizeof(BITMAPINFOHEADER));  

	hBitmap = CreateDIBSection(0, &BitmapInfo, DIB_RGB_COLORS, &buffer, NULL, 0);

	do 
	{
		hr = pGrabber->GetCurrentBuffer(&size,NULL);  
		hr = pGrabber->GetCurrentBuffer(&size,(long *)buffer);
		if (size==0)
		{
			Sleep(200);	//等待摄像头真正启动
		}
		else{break;}
	} while (true);

	hDib=GlobalAlloc(GMEM_MOVEABLE,size+sizeof(BITMAPINFOHEADER));
	if (hDib == NULL)
	{
		return;
	}
	lpbi=(LPBITMAPINFOHEADER)GlobalLock(hDib);
	if (lpbi==NULL)
	{
		return;
	}
	*lpbi=BitmapInfo.bmiHeader;

	hPal=GetStockObject(DEFAULT_PALETTE);
	if (hPal)
	{
		hDC=GetDC(NULL);
		hOldPal=SelectPalette(hDC,(HPALETTE)hPal, FALSE);
		RealizePalette(hDC);
	}
	GetDIBits(hDC,hBitmap, 0,BitmapInfo.bmiHeader.biHeight,(byte *)lpbi+ sizeof(BITMAPINFOHEADER),(BITMAPINFO *)lpbi, DIB_RGB_COLORS);

	if (hOldPal)
	{
		SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
		RealizePalette(hDC);
		ReleaseDC(NULL, hDC);
	}
	bmfHdr.bfType=0x4D42;
	dwDIBSize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+size;
	bmfHdr.bfSize=dwDIBSize;
	bmfHdr.bfReserved1=0;
	bmfHdr.bfReserved2=0; 
	bmfHdr.bfOffBits=(DWORD)sizeof(BITMAPFILEHEADER)+(DWORD)sizeof(BITMAPINFOHEADER);

	//////////////////////////////////////////////格式转换/////////////////////////////////////////////////////
	HGLOBAL lpAddress=GlobalAlloc(GMEM_MOVEABLE,dwDIBSize);
	if(lpAddress == NULL)
	{
		PrintText("GlobalAlloc error\r\n");
		return;
	}

	byte *  pbLocked = (BYTE*) GlobalLock(lpAddress); 
	memcpy(pbLocked,&bmfHdr,sizeof(BITMAPFILEHEADER));
	memcpy(pbLocked+(sizeof(BITMAPFILEHEADER)),lpbi,dwDIBSize);
	GlobalUnlock(lpAddress);

	//DWORD dwWrite;
	//HANDLE  hFile=CreateFile("MEM_JPG.bmp",GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
	//WriteFile(hFile,jpgfile,stg.cbSize.LowPart,&dwWrite,NULL);
	//CloseHandle(hFile);
	
	//CLSID codecClsid;
	//Gdiplus::EncoderParameters encoderParameters;
	//Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	//ULONG gdiplusToken;
	//DWORD iQuality=75;
	//IStream *bmp_stream = NULL;
	//IStream *jpg_stream = NULL;
	//LARGE_INTEGER isBegin = {0};
	//STATSTG stg={0};
	//ULONG u_read=0;

	//Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	//GetCodecClsid(L"image/jpeg", &codecClsid);

	//encoderParameters.Count = 1;
	//encoderParameters.Parameter[0].Guid = Gdiplus::EncoderQuality;
	//encoderParameters.Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
	//encoderParameters.Parameter[0].NumberOfValues = 1;
	//encoderParameters.Parameter[0].Value = &iQuality;

	//HGLOBAL lpjpgAddr=GlobalAlloc(GMEM_MOVEABLE,0);
	//if(lpjpgAddr == NULL)
	//{
	//	PrintText("GlobalAlloc error: %d\r\n");
	//	return;
	//}

	//if( CreateStreamOnHGlobal( lpAddress, TRUE, &bmp_stream ) != S_OK )
	//{
	//	PrintText("创建BMP流错误\r\n");
	//	return;
	//}


	//if( CreateStreamOnHGlobal( lpjpgAddr, TRUE, &jpg_stream ) != S_OK )
	//{
	//	PrintText("创建JPG流错误\r\n");
	//	return;
	//}
	//Gdiplus::Image *pImage = new Gdiplus::Image(bmp_stream);
	//Gdiplus::Status bStatus = pImage->Save(jpg_stream, &codecClsid, &encoderParameters);
	//jpg_stream->Stat(&stg,STATFLAG_NONAME);
	//char *jpgfile=new char[stg.cbSize.LowPart];
	//jpg_stream->Seek(isBegin,STREAM_SEEK_SET,NULL);
	//jpg_stream->Read(jpgfile,stg.cbSize.LowPart,&u_read);

	//DWORD dwWrite;
	//HANDLE  hFile=CreateFile("c:\\MEM_JPG.jpg",GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
	//WriteFile(hFile,jpgfile,stg.cbSize.LowPart,&dwWrite,NULL);
	//CloseHandle(hFile);

	//bmp_stream->Release();
	//GlobalFree(lpAddress);
	//jpg_stream->Release();
	//GlobalFree(lpjpgAddr);
	//delete[] pImage;
	//delete[] jpgfile;
	//Gdiplus::GdiplusShutdown(gdiplusToken);

	ConvertAndSubmitBMP(&lpAddress,pci);
	GlobalFree(lpAddress);

	if (mt.cbFormat != 0)
	{
		CoTaskMemFree((PVOID)mt.pbFormat);
		mt.cbFormat = 0;
		mt.pbFormat = NULL;
	}
	if (mt.pUnk != NULL)
	{
		mt.pUnk->Release();
		mt.pUnk = NULL;
	}
	GlobalUnlock(hDib);
	GlobalFree(hDib);	
	DeleteObject(hBitmap);
}

bool StartCaptureDevice(HWND parent)
{
	HRESULT hr;
	ghApp=parent;
	if(FAILED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED)))
	{
		Msg(TEXT("CoInitialize Failed!\r\n"));   
		return false;
	} 
	hr = CaptureVideo();
	if (FAILED (hr))
	{
		CloseInterfaces();
		CoUninitialize();
		return false;
	}
	return true;
}

bool StopCaptureDevice()
{
	ghApp=NULL;


	CloseInterfaces();
	CoUninitialize();
	return true;
}



//LPD3DXBUFFER lpdb;

IDirect3DSurface9 *pSurface;
IDirect3DDevice9 *g_pd3dDevice;
LPDIRECT3D9 g_pD3D = NULL;

bool InitialDX()
{
	
	D3DDISPLAYMODE ddm;
	D3DPRESENT_PARAMETERS d3dpp;
	
	ZeroMemory(&d3dpp,sizeof(D3DPRESENT_PARAMETERS));
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	if((g_pD3D=Direct3DCreate9(D3D_SDK_VERSION))==NULL)
	{
		return false;
	}
	if(FAILED(g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&ddm)))
	{
		return false;
	}

	d3dpp.Windowed=TRUE;
	d3dpp.Flags=D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	d3dpp.BackBufferFormat=ddm.Format;
	d3dpp.BackBufferHeight=ddm.Height;
	d3dpp.BackBufferWidth=ddm.Width;
	d3dpp.MultiSampleType=D3DMULTISAMPLE_NONE;
	d3dpp.SwapEffect=D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow=GetDesktopWindow();
	d3dpp.PresentationInterval=D3DPRESENT_INTERVAL_DEFAULT;
	d3dpp.FullScreen_RefreshRateInHz=D3DPRESENT_RATE_DEFAULT;

	if(FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,GetDesktopWindow(),D3DCREATE_SOFTWARE_VERTEXPROCESSING ,&d3dpp,&g_pd3dDevice)))
	{
		return false;
	}

	if(FAILED(g_pd3dDevice->CreateOffscreenPlainSurface(ddm.Width, ddm.Height, D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &pSurface, NULL)))
	{
		return false;
	}
	return true;
}


int DXScreenShot()
{
startclock();
//	g_pd3dDevice->CreateOffscreenPlainSurface(ddm.Width, ddm.Height,D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &pSurface, NULL);
	g_pd3dDevice->GetFrontBufferData(0, pSurface);

	
	D3DXSaveSurfaceToFile("C:\\test.bmp",D3DXIFF_BMP,pSurface,NULL,NULL);//保存为 jpg格式
	
//	D3DXSaveSurfaceToFileInMemory(&lpdb,D3DXIFF_BMP,pSurface,NULL,NULL);
	
//	ConvertAndSubmitBMP((HGLOBAL *)lpdb->GetBufferPointer(),"32");
	stopclock("截图");

	//pSurface->Release();
	//g_pd3dDevice->Release();
	//g_pD3D->Release ();

	return 0;
}
void UnloadDx()
{
	pSurface->Release();
	g_pd3dDevice->Release();
	g_pD3D->Release ();
}

