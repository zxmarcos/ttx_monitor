// Wrapper for Direct3D
//
// Most difficult part of project.
// by Romhack


#include <windows.h>
#include "stdafx.h"
#define COM_NO_WINDOWS_H
#include <objbase.h>
#include <d3d9.h>
#include <DXErr.h>
#include <d3dx9effect.h>
#include "D3DWrapper.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dxerr.lib")

// Gera muito, mais muito log '-'
#if 1
#define LOG_API()	logmsg("%s\n", __FUNCTION__)
#else
#define LOG_API()
#endif



LPDirect3DCreate9 __Direct3DCreate9 = NULL;

static LPDIRECT3D9 pD3D;
static LPDIRECT3DDEVICE9 pD3Dev;

struct FAR HookIDirect3DDevice9;
static HookIDirect3DDevice9 D3DevWrapper, *pD3DevWrapper = &D3DevWrapper;



HRESULT HookIDirect3D9::QueryInterface(LPVOID _this, REFIID riid,LPVOID *ppvObj)
{
	LOG_API();
	return pD3D->QueryInterface(riid, ppvObj);
}

ULONG HookIDirect3D9::AddRef(LPVOID _this)
{
	LOG_API();
	return pD3D->AddRef();
}

ULONG HookIDirect3D9::Release(LPVOID _this)
{
	LOG_API();
	return pD3D->Release();
}

HRESULT HookIDirect3D9::RegisterSoftwareDevice(LPVOID _this, void* pInitializeFunction)
{
	LOG_API();
	return pD3D->RegisterSoftwareDevice(pInitializeFunction);
}

UINT HookIDirect3D9:: GetAdapterCount(LPVOID _this)
{
	LOG_API();
	return pD3D->GetAdapterCount();
}

HRESULT HookIDirect3D9::GetAdapterIdentifier(LPVOID _this, UINT Adapter,DWORD Flags,D3DADAPTER_IDENTIFIER9* pIdentifier)
{
	LOG_API();
	return pD3D->GetAdapterIdentifier(Adapter, Flags, pIdentifier);
}

UINT HookIDirect3D9:: GetAdapterModeCount(LPVOID _this, UINT Adapter,D3DFORMAT Format)
{
	LOG_API();
	return pD3D->GetAdapterModeCount(Adapter, Format);
}

HRESULT HookIDirect3D9::EnumAdapterModes(LPVOID _this, UINT Adapter,D3DFORMAT Format,UINT Mode,D3DDISPLAYMODE* pMode)
{
	LOG_API();
	return pD3D->EnumAdapterModes(Adapter, Format, Mode, pMode);
}

HRESULT HookIDirect3D9::GetAdapterDisplayMode(LPVOID _this, UINT Adapter,D3DDISPLAYMODE* pMode)
{
	LOG_API();
	return pD3D->GetAdapterDisplayMode(Adapter, pMode);
}

HRESULT HookIDirect3D9::CheckDeviceType(LPVOID _this, UINT Adapter,D3DDEVTYPE DevType,D3DFORMAT AdapterFormat,D3DFORMAT BackBufferFormat,BOOL bWindowed)
{
	LOG_API();
	return pD3D->CheckDeviceType(Adapter, DevType, AdapterFormat, BackBufferFormat, bWindowed);
}

HRESULT HookIDirect3D9::CheckDeviceFormat(LPVOID _this, UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,DWORD Usage,D3DRESOURCETYPE RType,D3DFORMAT CheckFormat)
{
	LOG_API();
	return pD3D->CheckDeviceFormat(Adapter, DeviceType, AdapterFormat, Usage, RType, CheckFormat);
}

HRESULT HookIDirect3D9::CheckDeviceMultiSampleType(LPVOID _this, UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SurfaceFormat,BOOL Windowed,D3DMULTISAMPLE_TYPE MultiSampleType,DWORD* pQualityLevels)
{
	LOG_API();
	return pD3D->CheckDeviceMultiSampleType(Adapter, DeviceType, SurfaceFormat, Windowed, MultiSampleType, pQualityLevels);
}

HRESULT HookIDirect3D9::CheckDepthStencilMatch(LPVOID _this, UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,D3DFORMAT RenderTargetFormat,D3DFORMAT DepthStencilFormat)
{
	LOG_API();
	return pD3D->CheckDepthStencilMatch(Adapter, DeviceType, AdapterFormat, RenderTargetFormat, DepthStencilFormat);
}

HRESULT HookIDirect3D9::CheckDeviceFormatConversion(LPVOID _this, UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SourceFormat,D3DFORMAT TargetFormat)
{
	LOG_API();
	return pD3D->CheckDeviceFormatConversion(Adapter, DeviceType, SourceFormat, TargetFormat);
}

HRESULT HookIDirect3D9::GetDeviceCaps(LPVOID _this, UINT Adapter,D3DDEVTYPE DeviceType,D3DCAPS9* pCaps)
{
	LOG_API();
	return pD3D->GetDeviceCaps(Adapter, DeviceType, pCaps);
}

HMONITOR HookIDirect3D9::GetAdapterMonitor(LPVOID _this, UINT Adapter)
{
	LOG_API();
	return pD3D->GetAdapterMonitor(Adapter);
}


HRESULT HookIDirect3D9::CreateDevice(LPVOID _this, UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DDevice9** ppReturnedDeviceInterface)
{

#if 1
	LOG_API();
	HRESULT res = 0;

	if (configMgr.GetConfig(TTX_CONFIG_WINDOWED))
	{

		D3DPRESENT_PARAMETERS d3dpp;
		RECT rect;

		ShowWindow(hFocusWindow, SW_HIDE);
		GetClientRect(hFocusWindow, &rect);
		AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
		SetWindowLong(hFocusWindow, GWL_STYLE, WS_OVERLAPPEDWINDOW);
		SetWindowPos(hFocusWindow, HWND_TOPMOST, 100, 100, rect.right, rect.bottom, SWP_FRAMECHANGED | SWP_NOCOPYBITS);
		ShowWindow(hFocusWindow, SW_SHOWNORMAL);
		ShowCursor(TRUE);
		InvalidateRect(hFocusWindow, NULL, TRUE);

		logmsg("--%d,%d,%d,%d\n", rect.left,rect.top,rect.right,rect.bottom);

		ZeroMemory(&d3dpp, sizeof(d3dpp));


		GetClientRect(pPresentationParameters->hDeviceWindow, &rect);

		logmsg("Area = %d,%d\n", pPresentationParameters->BackBufferWidth, pPresentationParameters->BackBufferHeight);
		d3dpp.BackBufferHeight = pPresentationParameters->BackBufferHeight;
		d3dpp.BackBufferWidth = pPresentationParameters->BackBufferWidth;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.BackBufferFormat = pPresentationParameters->BackBufferFormat;
		d3dpp.BackBufferCount = pPresentationParameters->BackBufferCount;
		d3dpp.Windowed = TRUE;
		d3dpp.hDeviceWindow = NULL;//pPresentationParameters->hDeviceWindow;
		d3dpp.FullScreen_RefreshRateInHz = 0;
		d3dpp.PresentationInterval = pPresentationParameters->PresentationInterval;
		d3dpp.AutoDepthStencilFormat = pPresentationParameters->AutoDepthStencilFormat;
		d3dpp.Flags = pPresentationParameters->Flags;
		d3dpp.EnableAutoDepthStencil = TRUE;


		res = pD3D->CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags, &d3dpp, &pD3Dev);
	} else 
		res = pD3D->CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, &pD3Dev);
	if (FAILED(res)) {
		logmsg("Err = %X\n", res);
		MessageBox(hFocusWindow, DXGetErrorDescriptionA(res), "Erro Direct3D", MB_OK);
		*ppReturnedDeviceInterface = NULL;
	} else {
		*ppReturnedDeviceInterface = (IDirect3DDevice9*) pD3DevWrapper;
	}

	return res;
#else
	return pD3D->CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);
#endif
}

static HookIDirect3D9 D3DWrapper, *pD3DWrapper = &D3DWrapper;

IDirect3D9* __stdcall Hook_Direct3DCreate9(UINT SDKVersion)
{
	LOG_API();
	pD3D = __Direct3DCreate9(SDKVersion);
	return (LPDIRECT3D9) pD3DWrapper;
}






HRESULT HookIDirect3DDevice9::QueryInterface(LPVOID _this, REFIID riid,LPVOID *ppvObj)
{
	LOG_API();
	return pD3Dev->QueryInterface(riid, ppvObj);
}

ULONG HookIDirect3DDevice9::AddRef(LPVOID _this)
{
	LOG_API();
	return pD3Dev->AddRef();
}

ULONG HookIDirect3DDevice9::Release(LPVOID _this)
{
	LOG_API();
	return pD3Dev->Release();
}



HRESULT HookIDirect3DDevice9::TestCooperativeLevel(LPVOID _this)
{
	LOG_API();
	return pD3Dev->TestCooperativeLevel();
}


UINT HookIDirect3DDevice9::GetAvailableTextureMem(LPVOID _this)
{
	LOG_API();
	return pD3Dev->GetAvailableTextureMem();
}

HRESULT HookIDirect3DDevice9::EvictManagedResources(LPVOID _this)
{
	LOG_API();
	return pD3Dev->EvictManagedResources();
}

HRESULT HookIDirect3DDevice9::GetDirect3D(LPVOID _this, IDirect3D9** ppD3D9)
{
	LOG_API();
	return pD3Dev->GetDirect3D(ppD3D9);
}

HRESULT HookIDirect3DDevice9::GetDeviceCaps(LPVOID _this, D3DCAPS9* pCaps)
{
	LOG_API();
	return pD3Dev->GetDeviceCaps(pCaps);
}

HRESULT HookIDirect3DDevice9::GetDisplayMode(LPVOID _this, UINT iSwapChain,D3DDISPLAYMODE* pMode)
{
	LOG_API();
	return pD3Dev->GetDisplayMode(iSwapChain, pMode);
}

HRESULT HookIDirect3DDevice9::GetCreationParameters(LPVOID _this, D3DDEVICE_CREATION_PARAMETERS *pParameters)
{
	LOG_API();
	return pD3Dev->GetCreationParameters(pParameters);
}

HRESULT HookIDirect3DDevice9::SetCursorProperties(LPVOID _this, UINT XHotSpot,UINT YHotSpot,IDirect3DSurface9* pCursorBitmap)
{
	LOG_API();
	return pD3Dev->SetCursorProperties(XHotSpot, YHotSpot, pCursorBitmap);
}

VOID HookIDirect3DDevice9::SetCursorPosition(LPVOID _this, int X,int Y,DWORD Flags)
{
	LOG_API();
	return pD3Dev->SetCursorPosition(X, Y, Flags);
}

BOOL HookIDirect3DDevice9::ShowCursor(LPVOID _this, BOOL bShow)
{
	LOG_API();
	return pD3Dev->ShowCursor(bShow);
}

HRESULT HookIDirect3DDevice9::CreateAdditionalSwapChain(LPVOID _this,
														D3DPRESENT_PARAMETERS* pPresentationParameters,
														IDirect3DSwapChain9** pSwapChain)
{
	LOG_API();
	return pD3Dev->CreateAdditionalSwapChain(pPresentationParameters, pSwapChain);
}

HRESULT HookIDirect3DDevice9::GetSwapChain(LPVOID _this, UINT iSwapChain,IDirect3DSwapChain9** pSwapChain)
{
	LOG_API();
	return pD3Dev->GetSwapChain(iSwapChain, pSwapChain);
}

UINT HookIDirect3DDevice9::GetNumberOfSwapChains(LPVOID _this)
{
	LOG_API();
	return pD3Dev->GetNumberOfSwapChains();
}

HRESULT HookIDirect3DDevice9::Reset(LPVOID _this, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	LOG_API();
	return pD3Dev->Reset(pPresentationParameters);
}

HRESULT HookIDirect3DDevice9::Present(LPVOID _this,
									  CONST RECT* pSourceRect,
									  CONST RECT* pDestRect,
									  HWND hDestWindowOverride,
									  CONST RGNDATA* pDirtyRegion)
{
	LOG_API();
	return pD3Dev->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}


HRESULT HookIDirect3DDevice9::GetBackBuffer(LPVOID _this,
											UINT iSwapChain,
											UINT iBackBuffer,
											D3DBACKBUFFER_TYPE Type,
											IDirect3DSurface9** ppBackBuffer)
{
	LOG_API();
	return pD3Dev->GetBackBuffer(iSwapChain, iBackBuffer, Type, ppBackBuffer);
}

HRESULT HookIDirect3DDevice9::GetRasterStatus(LPVOID _this, UINT iSwapChain,D3DRASTER_STATUS* pRasterStatus)
{
	LOG_API();
	return pD3Dev->GetRasterStatus(iSwapChain, pRasterStatus);
}

HRESULT HookIDirect3DDevice9::SetDialogBoxMode(LPVOID _this, BOOL bEnableDialogs)
{
	LOG_API();
	return pD3Dev->SetDialogBoxMode(bEnableDialogs);
}

VOID HookIDirect3DDevice9::SetGammaRamp(LPVOID _this, UINT iSwapChain,DWORD Flags,CONST D3DGAMMARAMP* pRamp)
{
	LOG_API();
	return pD3Dev->SetGammaRamp(iSwapChain, Flags, pRamp);
}


VOID HookIDirect3DDevice9::GetGammaRamp(LPVOID _this, UINT iSwapChain,D3DGAMMARAMP* pRamp)
{
	LOG_API();
	return pD3Dev->GetGammaRamp(iSwapChain, pRamp);
}

HRESULT HookIDirect3DDevice9::CreateTexture(LPVOID _this, UINT Width,UINT Height,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DTexture9** ppTexture,HANDLE* pSharedHandle)
{
	LOG_API();
	return pD3Dev->CreateTexture(Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle);
}

HRESULT HookIDirect3DDevice9::CreateVolumeTexture(LPVOID _this,
												  UINT Width,
												  UINT Height,
												  UINT Depth,
												  UINT Levels,
												  DWORD Usage,
												  D3DFORMAT Format,
												  D3DPOOL Pool,
												  IDirect3DVolumeTexture9** ppVolumeTexture,
												  HANDLE* pSharedHandle)
{
	LOG_API();
	return pD3Dev->CreateVolumeTexture(Width, Height, Depth, Levels, Usage, Format, Pool, ppVolumeTexture, pSharedHandle);
}


HRESULT HookIDirect3DDevice9::CreateCubeTexture(LPVOID _this,
												UINT EdgeLength,
												UINT Levels,
												DWORD Usage,
												D3DFORMAT Format,
												D3DPOOL Pool,
												IDirect3DCubeTexture9** ppCubeTexture,
												HANDLE* pSharedHandle)
{
	LOG_API();
	return pD3Dev->CreateCubeTexture(EdgeLength, Levels, Usage, Format, Pool, ppCubeTexture, pSharedHandle);
}

HRESULT HookIDirect3DDevice9::CreateVertexBuffer(LPVOID _this,
												 UINT Length,
												 DWORD Usage,
												 DWORD FVF,
												 D3DPOOL Pool,
												 IDirect3DVertexBuffer9** ppVertexBuffer,
												 HANDLE* pSharedHandle)
{
	LOG_API();
	return pD3Dev->CreateVertexBuffer(Length, Usage, FVF, Pool, ppVertexBuffer, pSharedHandle);
}

HRESULT HookIDirect3DDevice9::CreateIndexBuffer(LPVOID _this,
												UINT Length,
												DWORD Usage,
												D3DFORMAT Format,
												D3DPOOL Pool,
												IDirect3DIndexBuffer9** ppIndexBuffer,
												HANDLE* pSharedHandle)
{
	LOG_API();
	return pD3Dev->CreateIndexBuffer(Length, Usage, Format, Pool, ppIndexBuffer, pSharedHandle);
}

HRESULT HookIDirect3DDevice9::CreateRenderTarget(LPVOID _this,
												 UINT Width,
												 UINT Height,
												 D3DFORMAT Format,
												 D3DMULTISAMPLE_TYPE MultiSample,
												 DWORD MultisampleQuality,
												 BOOL Lockable,
												 IDirect3DSurface9** ppSurface,
												 HANDLE* pSharedHandle)
{
	LOG_API();
	return pD3Dev->CreateRenderTarget(Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle);
}

HRESULT HookIDirect3DDevice9::CreateDepthStencilSurface(LPVOID _this,
														UINT Width,
														UINT Height,
														D3DFORMAT Format,
														D3DMULTISAMPLE_TYPE MultiSample,
														DWORD MultisampleQuality,
														BOOL Discard,
														IDirect3DSurface9** ppSurface,
														HANDLE* pSharedHandle)
{
	LOG_API();
	return pD3Dev->CreateDepthStencilSurface(Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle);
}

HRESULT HookIDirect3DDevice9::UpdateSurface(LPVOID _this,
											IDirect3DSurface9* pSourceSurface,
											CONST RECT* pSourceRect,
											IDirect3DSurface9* pDestinationSurface,
											CONST POINT* pDestPoint)
{
	LOG_API();
	return pD3Dev->UpdateSurface(pSourceSurface, pSourceRect, pDestinationSurface, pDestPoint);
}


HRESULT HookIDirect3DDevice9::UpdateTexture(LPVOID _this,
											IDirect3DBaseTexture9* pSourceTexture,
											IDirect3DBaseTexture9* pDestinationTexture)
{
	LOG_API();
	return pD3Dev->UpdateTexture(pSourceTexture, pDestinationTexture);
}

HRESULT HookIDirect3DDevice9::GetRenderTargetData(LPVOID _this,
												  IDirect3DSurface9* pRenderTarget,
												  IDirect3DSurface9* pDestSurface)
{
	LOG_API();
	return pD3Dev->GetRenderTargetData(pRenderTarget, pDestSurface);
}

HRESULT HookIDirect3DDevice9::GetFrontBufferData(LPVOID _this,
												 UINT iSwapChain,
												 IDirect3DSurface9* pDestSurface)
{
	LOG_API();
	return pD3Dev->GetFrontBufferData(iSwapChain, pDestSurface);
}

HRESULT HookIDirect3DDevice9::StretchRect(LPVOID _this,
										  IDirect3DSurface9* pSourceSurface,
										  CONST RECT* pSourceRect,
										  IDirect3DSurface9* pDestSurface,
										  CONST RECT* pDestRect,
										  D3DTEXTUREFILTERTYPE Filter)
{
	LOG_API();
	return pD3Dev->StretchRect(pSourceSurface, pSourceRect, pDestSurface, pDestRect, Filter);
}

HRESULT HookIDirect3DDevice9::ColorFill(LPVOID _this,
										IDirect3DSurface9* pSurface,
										CONST RECT* pRect,
										D3DCOLOR color)
{
	LOG_API();
	return pD3Dev->ColorFill(pSurface, pRect, color);
}


HRESULT HookIDirect3DDevice9::CreateOffscreenPlainSurface(LPVOID _this,
														  UINT Width,
														  UINT Height,
														  D3DFORMAT Format,
														  D3DPOOL Pool,
														  IDirect3DSurface9** ppSurface,
														  HANDLE* pSharedHandle)
{
	LOG_API();
	return pD3Dev->CreateOffscreenPlainSurface(Width, Height, Format, Pool, ppSurface, pSharedHandle);
}

HRESULT HookIDirect3DDevice9::SetRenderTarget(LPVOID _this,
											  DWORD RenderTargetIndex,
											  IDirect3DSurface9* pRenderTarget)
{
	LOG_API();
	return pD3Dev->SetRenderTarget(RenderTargetIndex, pRenderTarget);
}


HRESULT HookIDirect3DDevice9::GetRenderTarget(LPVOID _this,
											  DWORD RenderTargetIndex,
											  IDirect3DSurface9** ppRenderTarget)
{
	LOG_API();
	return pD3Dev->GetRenderTarget(RenderTargetIndex, ppRenderTarget);
}


HRESULT HookIDirect3DDevice9::SetDepthStencilSurface(LPVOID _this,
													 IDirect3DSurface9* pNewZStencil)
{
	LOG_API();
	return pD3Dev->SetDepthStencilSurface(pNewZStencil);
}

HRESULT HookIDirect3DDevice9::GetDepthStencilSurface(LPVOID _this,
													 IDirect3DSurface9** ppZStencilSurface)
{
	LOG_API();
	return pD3Dev->GetDepthStencilSurface(ppZStencilSurface);
}

HRESULT HookIDirect3DDevice9::BeginScene(LPVOID _this)
{
	LOG_API();
	HRESULT res = 0;
	res = pD3Dev->BeginScene();

	return res;
}

HRESULT HookIDirect3DDevice9::EndScene(LPVOID _this)
{
	LOG_API();

	return pD3Dev->EndScene();
}

HRESULT HookIDirect3DDevice9::Clear(LPVOID _this,
									DWORD Count,
									CONST D3DRECT* pRects,
									DWORD Flags,
									D3DCOLOR Color,
									float Z,
									DWORD Stencil)
{
	LOG_API();
	return pD3Dev->Clear(Count, pRects, Flags, Color, Z, Stencil);
}

HRESULT HookIDirect3DDevice9::SetTransform(LPVOID _this,
										   D3DTRANSFORMSTATETYPE State,
										   CONST D3DMATRIX* pMatrix)
{
	LOG_API();
	return pD3Dev->SetTransform(State, pMatrix);
}

HRESULT HookIDirect3DDevice9::GetTransform(LPVOID _this,
										   D3DTRANSFORMSTATETYPE State,
										   D3DMATRIX* pMatrix)
{
	LOG_API();
	return pD3Dev->GetTransform(State, pMatrix);
}

HRESULT HookIDirect3DDevice9::MultiplyTransform(LPVOID _this,
												D3DTRANSFORMSTATETYPE Type,
												CONST D3DMATRIX* pMatrix)
{
	LOG_API();
	return pD3Dev->MultiplyTransform(Type, pMatrix);
}

HRESULT HookIDirect3DDevice9::SetViewport(LPVOID _this,
										  CONST D3DVIEWPORT9* pViewport)
{
	LOG_API();
	return pD3Dev->SetViewport(pViewport);
}

HRESULT HookIDirect3DDevice9::GetViewport(LPVOID _this,
										  D3DVIEWPORT9* pViewport)
{
	LOG_API();
	return pD3Dev->GetViewport(pViewport);
}

HRESULT HookIDirect3DDevice9::SetMaterial(LPVOID _this,
										  CONST D3DMATERIAL9* pMaterial)
{
	LOG_API();
	return pD3Dev->SetMaterial(pMaterial);
}

HRESULT HookIDirect3DDevice9::GetMaterial(LPVOID _this,
										  D3DMATERIAL9* pMaterial)
{
	LOG_API();
	return pD3Dev->GetMaterial(pMaterial);
}

HRESULT HookIDirect3DDevice9::SetLight(LPVOID _this,
									   DWORD Index,
									   CONST D3DLIGHT9* pLight)
{
	LOG_API();
	return pD3Dev->SetLight(Index, pLight);
}

HRESULT HookIDirect3DDevice9::GetLight(LPVOID _this,
									   DWORD Index,
									   D3DLIGHT9* pLight)
{
	LOG_API();
	return pD3Dev->GetLight(Index, pLight);
}

HRESULT HookIDirect3DDevice9::LightEnable(LPVOID _this,
										  DWORD Index,
										  BOOL Enable)
{
	LOG_API();
	return pD3Dev->LightEnable(Index, Enable);
}

HRESULT HookIDirect3DDevice9::GetLightEnable(LPVOID _this, DWORD Index,BOOL* pEnable)
{
	LOG_API();
	return pD3Dev->GetLightEnable(Index, pEnable);
}


HRESULT HookIDirect3DDevice9::SetClipPlane(LPVOID _this, DWORD Index,CONST float* pPlane)
{
	LOG_API();
	return pD3Dev->SetClipPlane(Index, pPlane);
}

HRESULT HookIDirect3DDevice9::GetClipPlane(LPVOID _this, DWORD Index,float* pPlane)
{
	LOG_API();
	return pD3Dev->GetClipPlane(Index, pPlane);
}

HRESULT HookIDirect3DDevice9::SetRenderState(LPVOID _this, D3DRENDERSTATETYPE State,DWORD Value)
{
	LOG_API();
	return pD3Dev->SetRenderState(State, Value);
}

HRESULT HookIDirect3DDevice9::GetRenderState(LPVOID _this, D3DRENDERSTATETYPE State,DWORD* pValue)
{
	LOG_API();
	return pD3Dev->GetRenderState(State, pValue);
}

HRESULT HookIDirect3DDevice9::CreateStateBlock(LPVOID _this,
											   D3DSTATEBLOCKTYPE Type,
											   IDirect3DStateBlock9** ppSB)
{
	LOG_API();
	return pD3Dev->CreateStateBlock(Type, ppSB);
}

HRESULT HookIDirect3DDevice9::BeginStateBlock(LPVOID _this)
{
	LOG_API();
	return pD3Dev->BeginStateBlock();
}

HRESULT HookIDirect3DDevice9::EndStateBlock(LPVOID _this, IDirect3DStateBlock9** ppSB)
{
	LOG_API();
	return pD3Dev->EndStateBlock(ppSB);
}

HRESULT HookIDirect3DDevice9::SetClipStatus(LPVOID _this, CONST D3DCLIPSTATUS9* pClipStatus)
{
	LOG_API();
	return pD3Dev->SetClipStatus(pClipStatus);
}

HRESULT HookIDirect3DDevice9::GetClipStatus(LPVOID _this, D3DCLIPSTATUS9* pClipStatus)
{
	LOG_API();
	return pD3Dev->GetClipStatus(pClipStatus);
}

HRESULT HookIDirect3DDevice9::GetTexture(LPVOID _this, DWORD Stage,IDirect3DBaseTexture9** ppTexture)
{
	LOG_API();
	return pD3Dev->GetTexture(Stage, ppTexture);
}

HRESULT HookIDirect3DDevice9::SetTexture(LPVOID _this, DWORD Stage,IDirect3DBaseTexture9* pTexture)
{
	LOG_API();
	return pD3Dev->SetTexture(Stage, pTexture);
}

HRESULT HookIDirect3DDevice9::GetTextureStageState(LPVOID _this,
												   DWORD Stage,
												   D3DTEXTURESTAGESTATETYPE Type,
												   DWORD* pValue)
{
	LOG_API();
	return pD3Dev->GetTextureStageState(Stage, Type, pValue);
}

HRESULT HookIDirect3DDevice9::SetTextureStageState(LPVOID _this, DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value)
{
	LOG_API();
	return pD3Dev->SetTextureStageState(Stage, Type, Value);
}

HRESULT HookIDirect3DDevice9::GetSamplerState(LPVOID _this, DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD* pValue)
{
	LOG_API();
	return pD3Dev->GetSamplerState(Sampler, Type, pValue);
}

HRESULT HookIDirect3DDevice9::SetSamplerState(LPVOID _this, DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD Value)
{
	LOG_API();
	return pD3Dev->SetSamplerState(Sampler, Type, Value);
}

HRESULT HookIDirect3DDevice9::ValidateDevice(LPVOID _this, DWORD* pNumPasses)
{
	LOG_API();
	return pD3Dev->ValidateDevice(pNumPasses);
}

HRESULT HookIDirect3DDevice9::SetPaletteEntries(LPVOID _this, UINT PaletteNumber,CONST PALETTEENTRY* pEntries)
{
	LOG_API();
	return pD3Dev->SetPaletteEntries(PaletteNumber, pEntries);
}

HRESULT HookIDirect3DDevice9::GetPaletteEntries(LPVOID _this, UINT PaletteNumber,PALETTEENTRY* pEntries)
{
	LOG_API();
	return pD3Dev->GetPaletteEntries(PaletteNumber, pEntries);
}

HRESULT HookIDirect3DDevice9::SetCurrentTexturePalette(LPVOID _this, UINT PaletteNumber)
{
	LOG_API();
	return pD3Dev->SetCurrentTexturePalette(PaletteNumber);
}

HRESULT HookIDirect3DDevice9::GetCurrentTexturePalette(LPVOID _this, UINT *PaletteNumber)
{
	LOG_API();
	return pD3Dev->GetCurrentTexturePalette(PaletteNumber);
}

HRESULT HookIDirect3DDevice9::SetScissorRect(LPVOID _this, CONST RECT* pRect)
{
	LOG_API();
	return pD3Dev->SetScissorRect(pRect);
}

HRESULT HookIDirect3DDevice9::GetScissorRect(LPVOID _this, RECT* pRect)
{
	LOG_API();
	return pD3Dev->GetScissorRect(pRect);
}

HRESULT HookIDirect3DDevice9::SetSoftwareVertexProcessing(LPVOID _this, BOOL bSoftware)
{
	LOG_API();
	return pD3Dev->SetSoftwareVertexProcessing(bSoftware);
}


BOOL HookIDirect3DDevice9::GetSoftwareVertexProcessing(LPVOID _this)
{
	LOG_API();
	return pD3Dev->GetSoftwareVertexProcessing();
}

HRESULT HookIDirect3DDevice9::SetNPatchMode(LPVOID _this, float nSegments)
{
	LOG_API();
	return pD3Dev->SetNPatchMode(nSegments);
}

float HookIDirect3DDevice9::GetNPatchMode(LPVOID _this)
{
	LOG_API();
	return pD3Dev->GetNPatchMode();
}

HRESULT HookIDirect3DDevice9::DrawPrimitive(LPVOID _this,
											D3DPRIMITIVETYPE PrimitiveType,
											UINT StartVertex,
											UINT PrimitiveCount)
{
	LOG_API();
	return pD3Dev->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount);
}

HRESULT HookIDirect3DDevice9::DrawIndexedPrimitive(LPVOID _this,
												   D3DPRIMITIVETYPE PrimitiveType,
												   INT BaseVertexIndex,
												   UINT MinVertexIndex,
												   UINT NumVertices,
												   UINT startIndex,
												   UINT primCount)
{
	LOG_API();
	return pD3Dev->DrawIndexedPrimitive(PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
}

HRESULT HookIDirect3DDevice9::DrawPrimitiveUP(LPVOID _this,
											  D3DPRIMITIVETYPE PrimitiveType,
											  UINT PrimitiveCount,
											  CONST void* pVertexStreamZeroData,
											  UINT VertexStreamZeroStride)
{
	LOG_API();
	return pD3Dev->DrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);
}

HRESULT HookIDirect3DDevice9::DrawIndexedPrimitiveUP(LPVOID _this,
													 D3DPRIMITIVETYPE PrimitiveType,
													 UINT MinVertexIndex,
													 UINT NumVertices,
													 UINT PrimitiveCount,
													 CONST void* pIndexData,
													 D3DFORMAT IndexDataFormat,
													 CONST void* pVertexStreamZeroData,
													 UINT VertexStreamZeroStride)
{
	LOG_API();
	return pD3Dev->DrawIndexedPrimitiveUP(PrimitiveType, MinVertexIndex, NumVertices, PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride);
}

HRESULT HookIDirect3DDevice9::ProcessVertices(LPVOID _this,
											  UINT SrcStartIndex,
											  UINT DestIndex,
											  UINT VertexCount,
											  IDirect3DVertexBuffer9* pDestBuffer,
											  IDirect3DVertexDeclaration9* pVertexDecl,
											  DWORD Flags)
{
	LOG_API();
	return pD3Dev->ProcessVertices(SrcStartIndex, DestIndex, VertexCount, pDestBuffer, pVertexDecl, Flags);
}

HRESULT HookIDirect3DDevice9::CreateVertexDeclaration(LPVOID _this,
													  CONST D3DVERTEXELEMENT9* pVertexElements,
													  IDirect3DVertexDeclaration9** ppDecl)
{
	LOG_API();
	return pD3Dev->CreateVertexDeclaration(pVertexElements, ppDecl);
}

HRESULT HookIDirect3DDevice9::SetVertexDeclaration(LPVOID _this, IDirect3DVertexDeclaration9* pDecl)
{
	LOG_API();
	return pD3Dev->SetVertexDeclaration(pDecl);
}

HRESULT HookIDirect3DDevice9::GetVertexDeclaration(LPVOID _this, IDirect3DVertexDeclaration9** ppDecl)
{
	LOG_API();
	return pD3Dev->GetVertexDeclaration(ppDecl);
}

HRESULT HookIDirect3DDevice9::SetFVF(LPVOID _this, DWORD FVF)
{
	LOG_API();
	return pD3Dev->SetFVF(FVF);
}

HRESULT HookIDirect3DDevice9::GetFVF(LPVOID _this, DWORD* pFVF)
{
	LOG_API();
	return pD3Dev->GetFVF(pFVF);
}

HRESULT HookIDirect3DDevice9::CreateVertexShader(LPVOID _this,
												 CONST DWORD* pFunction,
												 IDirect3DVertexShader9** ppShader)
{
	LOG_API();
	return pD3Dev->CreateVertexShader(pFunction, ppShader);
}

HRESULT HookIDirect3DDevice9::SetVertexShader(LPVOID _this, IDirect3DVertexShader9* pShader)
{
	LOG_API();
	return pD3Dev->SetVertexShader(pShader);
}

HRESULT HookIDirect3DDevice9::GetVertexShader(LPVOID _this, IDirect3DVertexShader9** ppShader)
{
	LOG_API();
	return pD3Dev->GetVertexShader(ppShader);
}

HRESULT HookIDirect3DDevice9::SetVertexShaderConstantF(LPVOID _this,
													   UINT StartRegister,
													   CONST float* pConstantData,
													   UINT Vector4fCount)
{
	LOG_API();
	return pD3Dev->SetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT HookIDirect3DDevice9::GetVertexShaderConstantF(LPVOID _this,
													   UINT StartRegister,
													   float* pConstantData,
													   UINT Vector4fCount)
{
	LOG_API();
	return pD3Dev->GetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT HookIDirect3DDevice9::SetVertexShaderConstantI(LPVOID _this, UINT StartRegister,CONST int* pConstantData,UINT Vector4iCount)
{
	LOG_API();
	return pD3Dev->SetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

HRESULT HookIDirect3DDevice9::GetVertexShaderConstantI(LPVOID _this, UINT StartRegister,int* pConstantData,UINT Vector4iCount)
{
	LOG_API();
	return pD3Dev->GetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

HRESULT HookIDirect3DDevice9::SetVertexShaderConstantB(LPVOID _this, UINT StartRegister,CONST BOOL* pConstantData,UINT  BoolCount)
{
	LOG_API();
	return pD3Dev->SetVertexShaderConstantB(StartRegister, pConstantData, BoolCount);
}

HRESULT HookIDirect3DDevice9::GetVertexShaderConstantB(LPVOID _this, UINT StartRegister,BOOL* pConstantData,UINT BoolCount)
{
	LOG_API();
	return pD3Dev->GetVertexShaderConstantB(StartRegister, pConstantData, BoolCount);
}


HRESULT HookIDirect3DDevice9::SetStreamSource(LPVOID _this, UINT StreamNumber,IDirect3DVertexBuffer9* pStreamData,UINT OffsetInBytes,UINT Stride)
{
	LOG_API();
	return pD3Dev->SetStreamSource(StreamNumber, pStreamData, OffsetInBytes, Stride);
}


HRESULT HookIDirect3DDevice9::GetStreamSource(LPVOID _this,
											  UINT StreamNumber,
											  IDirect3DVertexBuffer9** ppStreamData,
											  UINT* pOffsetInBytes,UINT* pStride)
{
	LOG_API();
	return pD3Dev->GetStreamSource(StreamNumber, ppStreamData, pOffsetInBytes, pStride);
}

HRESULT HookIDirect3DDevice9::SetStreamSourceFreq(LPVOID _this, UINT StreamNumber,UINT Setting)
{
	LOG_API();
	return pD3Dev->SetStreamSourceFreq(StreamNumber, Setting);
}

HRESULT HookIDirect3DDevice9::GetStreamSourceFreq(LPVOID _this, UINT StreamNumber,UINT* pSetting)
{
	LOG_API();
	return pD3Dev->GetStreamSourceFreq(StreamNumber, pSetting);
}

HRESULT HookIDirect3DDevice9::SetIndices(LPVOID _this, IDirect3DIndexBuffer9* pIndexData)
{
	LOG_API();
	return pD3Dev->SetIndices(pIndexData);
}

HRESULT HookIDirect3DDevice9::GetIndices(LPVOID _this, IDirect3DIndexBuffer9** ppIndexData)
{
	LOG_API();
	return pD3Dev->GetIndices(ppIndexData);
}

HRESULT HookIDirect3DDevice9::CreatePixelShader(LPVOID _this,
												CONST DWORD* pFunction,
												IDirect3DPixelShader9** ppShader)
{
	LOG_API();
	return pD3Dev->CreatePixelShader(pFunction, ppShader);
}

HRESULT HookIDirect3DDevice9::SetPixelShader(LPVOID _this, IDirect3DPixelShader9* pShader)
{
	LOG_API();
	return pD3Dev->SetPixelShader(pShader);
}

HRESULT HookIDirect3DDevice9::GetPixelShader(LPVOID _this, IDirect3DPixelShader9** ppShader)
{
	LOG_API();
	return pD3Dev->GetPixelShader(ppShader);
}

HRESULT HookIDirect3DDevice9::SetPixelShaderConstantF(LPVOID _this,
													  UINT StartRegister,
													  CONST float* pConstantData,
													  UINT Vector4fCount)
{
	LOG_API();
	return pD3Dev->SetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT HookIDirect3DDevice9::GetPixelShaderConstantF(LPVOID _this,
													  UINT StartRegister,
													  float* pConstantData,
													  UINT Vector4fCount)
{
	LOG_API();
	return pD3Dev->GetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT HookIDirect3DDevice9::SetPixelShaderConstantI(LPVOID _this,
													  UINT StartRegister,
													  CONST int* pConstantData,
													  UINT Vector4iCount)
{
	LOG_API();
	return pD3Dev->SetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}


HRESULT HookIDirect3DDevice9::GetPixelShaderConstantI(LPVOID _this,
													  UINT StartRegister,
													  int* pConstantData,
													  UINT Vector4iCount)
{
	LOG_API();
	return pD3Dev->GetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

HRESULT HookIDirect3DDevice9::SetPixelShaderConstantB(LPVOID _this, UINT StartRegister,CONST BOOL* pConstantData,UINT  BoolCount)
{
	LOG_API();
	return pD3Dev->SetPixelShaderConstantB(StartRegister, pConstantData, BoolCount);
}

HRESULT HookIDirect3DDevice9::GetPixelShaderConstantB(LPVOID _this,
													  UINT StartRegister,
													  BOOL* pConstantData,
													  UINT BoolCount)
{
	LOG_API();
	return pD3Dev->GetPixelShaderConstantB(StartRegister, pConstantData, BoolCount);
}

HRESULT HookIDirect3DDevice9::DrawRectPatch(LPVOID _this,
											UINT Handle,
											CONST float* pNumSegs,
											CONST D3DRECTPATCH_INFO* pRectPatchInfo)
{
	LOG_API();
	return pD3Dev->DrawRectPatch(Handle, pNumSegs, pRectPatchInfo);
}

HRESULT HookIDirect3DDevice9::DrawTriPatch(LPVOID _this,
										   UINT Handle,
										   CONST float* pNumSegs,
										   CONST D3DTRIPATCH_INFO* pTriPatchInfo)
{
	LOG_API();
	return pD3Dev->DrawTriPatch(Handle, pNumSegs, pTriPatchInfo);
}

HRESULT HookIDirect3DDevice9::DeletePatch(LPVOID _this, UINT Handle)
{
	LOG_API();
	return pD3Dev->DeletePatch(Handle);
}

HRESULT HookIDirect3DDevice9::CreateQuery(LPVOID _this, D3DQUERYTYPE Type,IDirect3DQuery9** ppQuery)
{
	LOG_API();
	return pD3Dev->CreateQuery(Type, ppQuery);
}
