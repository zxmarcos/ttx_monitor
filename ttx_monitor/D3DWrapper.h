
#pragma once

#include <windows.h>
#include "stdafx.h"
#define COM_NO_WINDOWS_H
#include <objbase.h>
#include <d3d9.h>

struct FAR HookIDirect3D9
{
	/*** IUnknown methods ***/
	virtual HRESULT QueryInterface(LPVOID _this, REFIID riid,LPVOID *ppvObj);
	virtual ULONG AddRef(LPVOID _this);
	virtual ULONG Release(LPVOID _this);

	virtual HRESULT RegisterSoftwareDevice(LPVOID _this, void* pInitializeFunction) ;
	virtual UINT GetAdapterCount(LPVOID _this) ;
	virtual HRESULT GetAdapterIdentifier(LPVOID _this, UINT Adapter,DWORD Flags,D3DADAPTER_IDENTIFIER9* pIdentifier) ;
	virtual UINT GetAdapterModeCount(LPVOID _this, UINT Adapter,D3DFORMAT Format) ;
	virtual HRESULT EnumAdapterModes(LPVOID _this, UINT Adapter,D3DFORMAT Format,UINT Mode,D3DDISPLAYMODE* pMode) ;
	virtual HRESULT GetAdapterDisplayMode(LPVOID _this, UINT Adapter,D3DDISPLAYMODE* pMode) ;
	virtual HRESULT CheckDeviceType(LPVOID _this, UINT Adapter,D3DDEVTYPE DevType,D3DFORMAT AdapterFormat,D3DFORMAT BackBufferFormat,BOOL bWindowed) ;
	virtual HRESULT CheckDeviceFormat(LPVOID _this, UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,DWORD Usage,D3DRESOURCETYPE RType,D3DFORMAT CheckFormat) ;
	virtual HRESULT CheckDeviceMultiSampleType(LPVOID _this, UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SurfaceFormat,BOOL Windowed,D3DMULTISAMPLE_TYPE MultiSampleType,DWORD* pQualityLevels) ;
	virtual HRESULT CheckDepthStencilMatch(LPVOID _this, UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,D3DFORMAT RenderTargetFormat,D3DFORMAT DepthStencilFormat) ;
	virtual HRESULT CheckDeviceFormatConversion(LPVOID _this, UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SourceFormat,D3DFORMAT TargetFormat) ;
	virtual HRESULT GetDeviceCaps(LPVOID _this, UINT Adapter,D3DDEVTYPE DeviceType,D3DCAPS9* pCaps) ;
	virtual HMONITOR GetAdapterMonitor(LPVOID _this, UINT Adapter) ;
	virtual HRESULT CreateDevice(LPVOID _this, UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DDevice9** ppReturnedDeviceInterface) ;
};

struct FAR HookIDirect3DDevice9
{
	/*** IUnknown methods ***/
	virtual HRESULT QueryInterface(LPVOID _this, REFIID riid,LPVOID *ppvObj);
	virtual ULONG AddRef(LPVOID _this);
	virtual ULONG Release(LPVOID _this);

	/*** IDirect3DDevice9 methods ***/
    virtual HRESULT TestCooperativeLevel(LPVOID _this);
    virtual UINT GetAvailableTextureMem(LPVOID _this);
    virtual HRESULT EvictManagedResources(LPVOID _this);
    virtual HRESULT GetDirect3D(LPVOID _this, IDirect3D9** ppD3D9);
    virtual HRESULT GetDeviceCaps(LPVOID _this, D3DCAPS9* pCaps);
    virtual HRESULT GetDisplayMode(LPVOID _this, UINT iSwapChain,D3DDISPLAYMODE* pMode);
    virtual HRESULT GetCreationParameters(LPVOID _this, D3DDEVICE_CREATION_PARAMETERS *pParameters);
    virtual HRESULT SetCursorProperties(LPVOID _this, UINT XHotSpot,UINT YHotSpot,IDirect3DSurface9* pCursorBitmap);
	virtual VOID SetCursorPosition(LPVOID _this, int X,int Y,DWORD Flags);
    virtual BOOL ShowCursor(LPVOID _this, BOOL bShow);
    virtual HRESULT CreateAdditionalSwapChain(LPVOID _this, D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DSwapChain9** pSwapChain);
    virtual HRESULT GetSwapChain(LPVOID _this, UINT iSwapChain,IDirect3DSwapChain9** pSwapChain);
    virtual UINT GetNumberOfSwapChains(LPVOID _this);
    virtual HRESULT Reset(LPVOID _this, D3DPRESENT_PARAMETERS* pPresentationParameters);
    virtual HRESULT Present(LPVOID _this, CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion);
    virtual HRESULT GetBackBuffer(LPVOID _this, UINT iSwapChain,UINT iBackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface9** ppBackBuffer);
    virtual HRESULT GetRasterStatus(LPVOID _this, UINT iSwapChain,D3DRASTER_STATUS* pRasterStatus);
    virtual HRESULT SetDialogBoxMode(LPVOID _this, BOOL bEnableDialogs);
    virtual VOID SetGammaRamp(LPVOID _this, UINT iSwapChain,DWORD Flags,CONST D3DGAMMARAMP* pRamp);
    virtual VOID GetGammaRamp(LPVOID _this, UINT iSwapChain,D3DGAMMARAMP* pRamp);
    virtual HRESULT CreateTexture(LPVOID _this, UINT Width,UINT Height,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DTexture9** ppTexture,HANDLE* pSharedHandle);
    virtual HRESULT CreateVolumeTexture(LPVOID _this, UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DVolumeTexture9** ppVolumeTexture,HANDLE* pSharedHandle);
    virtual HRESULT CreateCubeTexture(LPVOID _this, UINT EdgeLength,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DCubeTexture9** ppCubeTexture,HANDLE* pSharedHandle);
    virtual HRESULT CreateVertexBuffer(LPVOID _this, UINT Length,DWORD Usage,DWORD FVF,D3DPOOL Pool,IDirect3DVertexBuffer9** ppVertexBuffer,HANDLE* pSharedHandle);
    virtual HRESULT CreateIndexBuffer(LPVOID _this, UINT Length,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DIndexBuffer9** ppIndexBuffer,HANDLE* pSharedHandle);
    virtual HRESULT CreateRenderTarget(LPVOID _this, UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Lockable,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle);
    virtual HRESULT CreateDepthStencilSurface(LPVOID _this, UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Discard,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle);
	virtual HRESULT UpdateSurface(LPVOID _this, IDirect3DSurface9* pSourceSurface,CONST RECT* pSourceRect,IDirect3DSurface9* pDestinationSurface,CONST POINT* pDestPoint);
    virtual HRESULT UpdateTexture(LPVOID _this, IDirect3DBaseTexture9* pSourceTexture,IDirect3DBaseTexture9* pDestinationTexture);
    virtual HRESULT GetRenderTargetData(LPVOID _this, IDirect3DSurface9* pRenderTarget,IDirect3DSurface9* pDestSurface);
    virtual HRESULT GetFrontBufferData(LPVOID _this, UINT iSwapChain,IDirect3DSurface9* pDestSurface);
    virtual HRESULT StretchRect(LPVOID _this, IDirect3DSurface9* pSourceSurface,CONST RECT* pSourceRect,IDirect3DSurface9* pDestSurface,CONST RECT* pDestRect,D3DTEXTUREFILTERTYPE Filter);
    virtual HRESULT ColorFill(LPVOID _this, IDirect3DSurface9* pSurface,CONST RECT* pRect,D3DCOLOR color);
    virtual HRESULT CreateOffscreenPlainSurface(LPVOID _this, UINT Width,UINT Height,D3DFORMAT Format,D3DPOOL Pool,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle);
    virtual HRESULT SetRenderTarget(LPVOID _this, DWORD RenderTargetIndex,IDirect3DSurface9* pRenderTarget);
    virtual HRESULT GetRenderTarget(LPVOID _this, DWORD RenderTargetIndex,IDirect3DSurface9** ppRenderTarget);
    virtual HRESULT SetDepthStencilSurface(LPVOID _this, IDirect3DSurface9* pNewZStencil);
    virtual HRESULT GetDepthStencilSurface(LPVOID _this, IDirect3DSurface9** ppZStencilSurface);
    virtual HRESULT BeginScene(LPVOID _this);
    virtual HRESULT EndScene(LPVOID _this);
    virtual HRESULT Clear(LPVOID _this, DWORD Count,CONST D3DRECT* pRects,DWORD Flags,D3DCOLOR Color,float Z,DWORD Stencil);
    virtual HRESULT SetTransform(LPVOID _this, D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX* pMatrix);
    virtual HRESULT GetTransform(LPVOID _this, D3DTRANSFORMSTATETYPE State,D3DMATRIX* pMatrix);
    virtual HRESULT MultiplyTransform(LPVOID _this, D3DTRANSFORMSTATETYPE,CONST D3DMATRIX*);
    virtual HRESULT SetViewport(LPVOID _this, CONST D3DVIEWPORT9* pViewport);
    virtual HRESULT GetViewport(LPVOID _this, D3DVIEWPORT9* pViewport);
    virtual HRESULT SetMaterial(LPVOID _this, CONST D3DMATERIAL9* pMaterial);
    virtual HRESULT GetMaterial(LPVOID _this, D3DMATERIAL9* pMaterial);
    virtual HRESULT SetLight(LPVOID _this, DWORD Index,CONST D3DLIGHT9*);
    virtual HRESULT GetLight(LPVOID _this, DWORD Index,D3DLIGHT9*);
    virtual HRESULT LightEnable(LPVOID _this, DWORD Index,BOOL Enable);
    virtual HRESULT GetLightEnable(LPVOID _this, DWORD Index,BOOL* pEnable);
    virtual HRESULT SetClipPlane(LPVOID _this, DWORD Index,CONST float* pPlane);
    virtual HRESULT GetClipPlane(LPVOID _this, DWORD Index,float* pPlane);
    virtual HRESULT SetRenderState(LPVOID _this, D3DRENDERSTATETYPE State,DWORD Value);
    virtual HRESULT GetRenderState(LPVOID _this, D3DRENDERSTATETYPE State,DWORD* pValue);
    virtual HRESULT CreateStateBlock(LPVOID _this, D3DSTATEBLOCKTYPE Type,IDirect3DStateBlock9** ppSB);
    virtual HRESULT BeginStateBlock(LPVOID _this);
    virtual HRESULT EndStateBlock(LPVOID _this, IDirect3DStateBlock9** ppSB);
    virtual HRESULT SetClipStatus(LPVOID _this, CONST D3DCLIPSTATUS9* pClipStatus);
    virtual HRESULT GetClipStatus(LPVOID _this, D3DCLIPSTATUS9* pClipStatus);
    virtual HRESULT GetTexture(LPVOID _this, DWORD Stage,IDirect3DBaseTexture9** ppTexture);
    virtual HRESULT SetTexture(LPVOID _this, DWORD Stage,IDirect3DBaseTexture9* pTexture);
    virtual HRESULT GetTextureStageState(LPVOID _this, DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD* pValue);
    virtual HRESULT SetTextureStageState(LPVOID _this, DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value);
    virtual HRESULT GetSamplerState(LPVOID _this, DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD* pValue);
    virtual HRESULT SetSamplerState(LPVOID _this, DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD Value);
    virtual HRESULT ValidateDevice(LPVOID _this, DWORD* pNumPasses);
    virtual HRESULT SetPaletteEntries(LPVOID _this, UINT PaletteNumber,CONST PALETTEENTRY* pEntries);
    virtual HRESULT GetPaletteEntries(LPVOID _this, UINT PaletteNumber,PALETTEENTRY* pEntries);
    virtual HRESULT SetCurrentTexturePalette(LPVOID _this, UINT PaletteNumber);
    virtual HRESULT GetCurrentTexturePalette(LPVOID _this, UINT *PaletteNumber);
    virtual HRESULT SetScissorRect(LPVOID _this, CONST RECT* pRect);
    virtual HRESULT GetScissorRect(LPVOID _this, RECT* pRect);
    virtual HRESULT SetSoftwareVertexProcessing(LPVOID _this, BOOL bSoftware);

    virtual BOOL GetSoftwareVertexProcessing(LPVOID _this);
    virtual HRESULT SetNPatchMode(LPVOID _this, float nSegments);
    virtual float GetNPatchMode(LPVOID _this);
    virtual HRESULT DrawPrimitive(LPVOID _this, D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount);
    virtual HRESULT DrawIndexedPrimitive(LPVOID _this, D3DPRIMITIVETYPE,INT BaseVertexIndex,UINT MinVertexIndex,UINT NumVertices,UINT startIndex,UINT primCount);
    virtual HRESULT DrawPrimitiveUP(LPVOID _this, D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride);
    virtual HRESULT DrawIndexedPrimitiveUP(LPVOID _this, D3DPRIMITIVETYPE PrimitiveType,UINT MinVertexIndex,UINT NumVertices,UINT PrimitiveCount,CONST void* pIndexData,D3DFORMAT IndexDataFormat,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride);
    virtual HRESULT ProcessVertices(LPVOID _this, UINT SrcStartIndex,UINT DestIndex,UINT VertexCount,IDirect3DVertexBuffer9* pDestBuffer,IDirect3DVertexDeclaration9* pVertexDecl,DWORD Flags);
    virtual HRESULT CreateVertexDeclaration(LPVOID _this, CONST D3DVERTEXELEMENT9* pVertexElements,IDirect3DVertexDeclaration9** ppDecl);
    virtual HRESULT SetVertexDeclaration(LPVOID _this, IDirect3DVertexDeclaration9* pDecl);
    virtual HRESULT GetVertexDeclaration(LPVOID _this, IDirect3DVertexDeclaration9** ppDecl);
    virtual HRESULT SetFVF(LPVOID _this, DWORD FVF);
    virtual HRESULT GetFVF(LPVOID _this, DWORD* pFVF);
    virtual HRESULT CreateVertexShader(LPVOID _this, CONST DWORD* pFunction,IDirect3DVertexShader9** ppShader);
    virtual HRESULT SetVertexShader(LPVOID _this, IDirect3DVertexShader9* pShader);
    virtual HRESULT GetVertexShader(LPVOID _this, IDirect3DVertexShader9** ppShader);
    virtual HRESULT SetVertexShaderConstantF(LPVOID _this, UINT StartRegister,CONST float* pConstantData,UINT Vector4fCount);
    virtual HRESULT GetVertexShaderConstantF(LPVOID _this, UINT StartRegister,float* pConstantData,UINT Vector4fCount);
    virtual HRESULT SetVertexShaderConstantI(LPVOID _this, UINT StartRegister,CONST int* pConstantData,UINT Vector4iCount);
    virtual HRESULT GetVertexShaderConstantI(LPVOID _this, UINT StartRegister,int* pConstantData,UINT Vector4iCount);
    virtual HRESULT SetVertexShaderConstantB(LPVOID _this, UINT StartRegister,CONST BOOL* pConstantData,UINT  BoolCount);
    virtual HRESULT GetVertexShaderConstantB(LPVOID _this, UINT StartRegister,BOOL* pConstantData,UINT BoolCount);
    virtual HRESULT SetStreamSource(LPVOID _this, UINT StreamNumber,IDirect3DVertexBuffer9* pStreamData,UINT OffsetInBytes,UINT Stride);
    virtual HRESULT GetStreamSource(LPVOID _this, UINT StreamNumber,IDirect3DVertexBuffer9** ppStreamData,UINT* pOffsetInBytes,UINT* pStride);
    virtual HRESULT SetStreamSourceFreq(LPVOID _this, UINT StreamNumber,UINT Setting);
    virtual HRESULT GetStreamSourceFreq(LPVOID _this, UINT StreamNumber,UINT* pSetting);
    virtual HRESULT SetIndices(LPVOID _this, IDirect3DIndexBuffer9* pIndexData);
    virtual HRESULT GetIndices(LPVOID _this, IDirect3DIndexBuffer9** ppIndexData);
    virtual HRESULT CreatePixelShader(LPVOID _this, CONST DWORD* pFunction,IDirect3DPixelShader9** ppShader);
    virtual HRESULT SetPixelShader(LPVOID _this, IDirect3DPixelShader9* pShader);
    virtual HRESULT GetPixelShader(LPVOID _this, IDirect3DPixelShader9** ppShader);
    virtual HRESULT SetPixelShaderConstantF(LPVOID _this, UINT StartRegister,CONST float* pConstantData,UINT Vector4fCount);
    virtual HRESULT GetPixelShaderConstantF(LPVOID _this, UINT StartRegister,float* pConstantData,UINT Vector4fCount);
    virtual HRESULT SetPixelShaderConstantI(LPVOID _this, UINT StartRegister,CONST int* pConstantData,UINT Vector4iCount);
    virtual HRESULT GetPixelShaderConstantI(LPVOID _this, UINT StartRegister,int* pConstantData,UINT Vector4iCount);
    virtual HRESULT SetPixelShaderConstantB(LPVOID _this, UINT StartRegister,CONST BOOL* pConstantData,UINT  BoolCount);
    virtual HRESULT GetPixelShaderConstantB(LPVOID _this, UINT StartRegister,BOOL* pConstantData,UINT BoolCount);
    virtual HRESULT DrawRectPatch(LPVOID _this, UINT Handle,CONST float* pNumSegs,CONST D3DRECTPATCH_INFO* pRectPatchInfo);
    virtual HRESULT DrawTriPatch(LPVOID _this, UINT Handle,CONST float* pNumSegs,CONST D3DTRIPATCH_INFO* pTriPatchInfo);
    virtual HRESULT DeletePatch(LPVOID _this, UINT Handle);
    virtual HRESULT CreateQuery(LPVOID _this, D3DQUERYTYPE Type,IDirect3DQuery9** ppQuery);
};
