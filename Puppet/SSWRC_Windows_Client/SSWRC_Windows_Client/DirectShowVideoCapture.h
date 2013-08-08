#include <atlbase.h>
#include <dshow.h>
#include <stdio.h>
#include <d3d9.h>
#include <d3dx9tex.h>

#pragma comment(lib,"dxguid.lib")  
#pragma comment(lib,"d3d9.lib")  
#pragma comment(lib,"d3dx9.lib")

#pragma include_alias( "dxtrans.h", "qedit.h" )
#define __IDxtCompositor_INTERFACE_DEFINED__
#define __IDxtAlphaSetter_INTERFACE_DEFINED__
#define __IDxtJpeg_INTERFACE_DEFINED__
#define __IDxtKey_INTERFACE_DEFINED__
#include "qedit.h"
#pragma comment(lib,"Strmiids.lib")

#include <Gdiplus.h>
#include <GdiPlusEnums.h>
#pragma comment(lib,"gdiplus")

HRESULT GetInterfaces(void);
HRESULT CaptureVideo();
HRESULT FindCaptureDevice(IBaseFilter ** ppSrcFilter);
HRESULT SetupVideoWindow(void);
HRESULT ChangePreviewState(int nShow);
HRESULT HandleGraphEvent(void);
HRESULT AddGraphToRot(IUnknown *pUnkGraph, DWORD *pdwRegister);
void RemoveGraphFromRot(DWORD pdwRegister);
void Msg(TCHAR *szFormat, ...);
void CloseInterfaces(void);
void ResizeVideoWindow(void);

//LPD3DXBUFFER lpdb;
extern LPDIRECT3D9 g_pD3D;
//extern D3DDISPLAYMODE ddm;
//extern D3DPRESENT_PARAMETERS d3dpp;
extern IDirect3DDevice9 *g_pd3dDevice;
extern IDirect3DSurface9 *pSurface;

bool InitialDX();
int DXScreenShot();
void UnloadDx();

bool StartCaptureDevice(HWND parent);
void CaptureCurrentFrame(void * cmd);
bool StopCaptureDevice();


enum PLAYSTATE {Stopped, Paused, Running, Init};


#define SAFE_RELEASE(x) { if (x) x->Release(); x = NULL; }

#define JIF(x) if (FAILED(hr=(x))) \
{Msg(TEXT("FAILED(hr=0x%x) in ") TEXT(#x) TEXT("\n\0"), hr); return hr;}


#define DEFAULT_VIDEO_WIDTH     640
#define DEFAULT_VIDEO_HEIGHT    480

#define APPLICATIONNAME TEXT("Video Capture\0")
#define CLASSNAME       TEXT("SinSoulVideoCapture\0")

#define WM_GRAPHNOTIFY  WM_APP+1

#define IDI_VIDPREVIEW          100
extern BOOL CameraStatus;
extern DWORD g_dwGraphRegister;
extern IVideoWindow  * g_pVW;
extern IMediaControl * g_pMC;
extern IMediaEventEx * g_pME;
extern IGraphBuilder * g_pGraph;
extern ICaptureGraphBuilder2 * g_pCapture;
extern PLAYSTATE g_psCurrent;
extern IBaseFilter *pGrabberFilter;
extern ISampleGrabber *pGrabber;
extern HWND ghApp;