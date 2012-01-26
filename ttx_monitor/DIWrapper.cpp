// ============================================================================
// ttx_monitor: Opensource TaitoTypeX loader
// by Romhack
// ============================================================================

// Fake API do DirectInput, assim impedimos que os jogos tenham acesso aos controles.
#include <windows.h>
#include <string.h>

#include "stdafx.h"

#define COM_NO_WINDOWS_H
#include <objbase.h>

LPDirectInput8Create __DirectInput8Create;

#if 0
#define LOG_API()	logmsg("%s\n", __FUNCTION__)
#else
#define LOG_API()
#endif
#pragma pack(4)
struct FAR HookIUnknown
{
	/*** IUnknown methods ***/
	virtual HRESULT QueryInterface(LPVOID _this, REFIID riid,LPVOID *ppvObj);
	virtual ULONG AddRef(LPVOID _this);
	virtual ULONG Release(LPVOID _this);
};

HRESULT HookIUnknown::QueryInterface(LPVOID _this, REFIID riid,
									 LPVOID *ppvObj)
{
	LOG_API();
	*ppvObj = NULL;
	return DIERR_UNSUPPORTED;
}

ULONG HookIUnknown::AddRef(LPVOID _this)
{
	LOG_API();
	return 0;
}

ULONG HookIUnknown::Release(LPVOID _this)
{
	LOG_API();
	return 0;
}

static HookIUnknown UnknownInfo, *pUnknownInfo = &UnknownInfo;
#pragma pack(4)
struct FAR HookIDirectInputDevice
{
	/*** IUnknown methods ***/
	virtual HRESULT QueryInterface(LPVOID _this, REFIID riid,LPVOID *ppvObj);
	virtual ULONG AddRef(LPVOID _this);
	virtual ULONG Release(LPVOID _this);
	/*** IDirectInputDeviceA methods ***/
	virtual HRESULT GetCapabilities(LPVOID _this, LPDIDEVCAPS lpDIDevCaps);
	virtual HRESULT EnumObjects(LPVOID _this, LPDIENUMDEVICEOBJECTSCALLBACK lpCallback,LPVOID pvRef,DWORD dwFlags);
	virtual HRESULT GetProperty(LPVOID _this, REFGUID rguidProp,LPDIPROPHEADER pdiph);
	virtual HRESULT SetProperty(LPVOID _this, REFGUID rguidProp,LPCDIPROPHEADER pdiph);
	virtual HRESULT Acquire(LPVOID _this);
	virtual HRESULT Unacquire(LPVOID _this);
	virtual HRESULT GetDeviceState(LPVOID _this, DWORD cbData,LPVOID lpvData);
	virtual HRESULT GetDeviceData(LPVOID _this, DWORD cbObjectData,LPDIDEVICEOBJECTDATA rgdod,LPDWORD pdwInOut,DWORD dwFlags);
	virtual HRESULT SetDataFormat(LPVOID _this, LPCDIDATAFORMAT lpdf);
	virtual HRESULT SetEventNotification(LPVOID _this, HANDLE hEvent);
	virtual HRESULT SetCooperativeLevel(LPVOID _this, HWND hwnd,DWORD dwFlags);
	virtual HRESULT GetObjectInfo(LPVOID _this, LPDIDEVICEOBJECTINSTANCE pdidoi,DWORD dwObj,DWORD dwHow);
	virtual HRESULT GetDeviceInfo(LPVOID _this, LPDIDEVICEINSTANCE pdidi);
	virtual HRESULT RunControlPanel(LPVOID _this, HWND hwndOwner,DWORD dwFlags);
	virtual HRESULT Initialize(LPVOID _this, HINSTANCE hinst,DWORD dwVersion,REFGUID rguid);
	virtual HRESULT CreateEffect(LPVOID _this, REFGUID rguid,LPCDIEFFECT lpeff,LPDIRECTINPUTEFFECT * ppdeff,LPUNKNOWN punkOuter);
	virtual HRESULT EnumEffects(LPVOID _this, LPDIENUMEFFECTSCALLBACK lpCallback,LPVOID pvRef,DWORD dwEffType);
	virtual HRESULT GetEffectInfo(LPVOID _this, LPDIEFFECTINFOA pdei,REFGUID rguid);
	virtual HRESULT GetForceFeedbackState(LPVOID _this, LPDWORD pdwOut);
	virtual HRESULT SendForceFeedbackCommand(LPVOID _this, DWORD dwFlags);
	virtual HRESULT EnumCreatedEffectObjects(LPVOID _this, LPDIENUMCREATEDEFFECTOBJECTSCALLBACK lpCallback,LPVOID pvRef,DWORD fl);
	virtual HRESULT Escape(LPVOID _this, LPDIEFFESCAPE pesc);
	virtual HRESULT Poll(LPVOID _this);
	virtual HRESULT SendDeviceData(LPVOID _this, DWORD cbObjectData,LPCDIDEVICEOBJECTDATA rgdod,LPDWORD pdwInOut,DWORD fl);
	virtual HRESULT EnumEffectsInFile(LPVOID _this, LPCSTR lpszFileName,LPDIENUMEFFECTSINFILECALLBACK pec,LPVOID pvRef,DWORD dwFlags);
	virtual HRESULT WriteEffectToFile(LPVOID _this, LPCSTR lpszFileName,DWORD dwEntries,LPCDIFILEEFFECT rgDiFileEft,DWORD dwFlags);
	virtual HRESULT BuildActionMap(LPVOID _this, LPDIACTIONFORMAT lpdiaf,LPCTSTR lpszUserName,DWORD dwFlags);
	virtual HRESULT SetActionMap(LPVOID _this, LPCDIACTIONFORMAT lpdiActionFormat,LPCTSTR lptszUserName,DWORD dwFlags);
	virtual HRESULT GetImageInfo(LPVOID _this, LPDIDEVICEIMAGEINFOHEADER lpdiDevImageInfoHeader);
};


HRESULT HookIDirectInputDevice::QueryInterface(LPVOID _this, REFIID riid,
											   LPVOID *ppvObj)
{
	LOG_API();
	*ppvObj = NULL;
	return 0;
}

ULONG HookIDirectInputDevice::AddRef(LPVOID _this)
{
	LOG_API();
	return 2;
}

ULONG HookIDirectInputDevice::Release(LPVOID _this)
{
	LOG_API();
	return 0;
}


HRESULT HookIDirectInputDevice::GetCapabilities(LPVOID _this, LPDIDEVCAPS lpDIDevCaps)
{
	LOG_API();
	return DI_OK;
}

HRESULT HookIDirectInputDevice::EnumObjects(LPVOID _this, LPDIENUMDEVICEOBJECTSCALLBACK lpCallback,
											LPVOID pvRef,
											DWORD dwFlags)
{
	LOG_API();
	return DI_OK;
}

HRESULT HookIDirectInputDevice::GetProperty(LPVOID _this, REFGUID rguidProp,
											LPDIPROPHEADER pdiph)
{
	LOG_API();
	return DI_OK;
}

HRESULT HookIDirectInputDevice::SetProperty(LPVOID _this, REFGUID rguidProp,
											LPCDIPROPHEADER pdiph)
{
	LOG_API();
	return DI_OK;
}

HRESULT HookIDirectInputDevice::Acquire(LPVOID _this)
{
	LOG_API();
	return DI_OK;
}

HRESULT HookIDirectInputDevice::Unacquire(LPVOID _this)
{
	LOG_API();
	return DI_OK;
}

HRESULT HookIDirectInputDevice::GetDeviceState(LPVOID _this, DWORD cbData,
											   LPVOID lpvData)
{
	if (lpvData)
		ZeroMemory(lpvData, cbData);

	LOG_API();
	return DI_OK;
}

HRESULT HookIDirectInputDevice::GetDeviceData(LPVOID _this, DWORD cbObjectData,
											  LPDIDEVICEOBJECTDATA rgdod,
											  LPDWORD pdwInOut,
											  DWORD dwFlags)
{
	LOG_API();
	// Função necessária, Chaos Breaker não rodará sem isso :)
	if (rgdod) {
		ZeroMemory(rgdod, cbObjectData * (*pdwInOut));
	}
	return DI_OK;
}

HRESULT HookIDirectInputDevice::SetDataFormat(LPVOID _this, LPCDIDATAFORMAT lpdf)
{
	LOG_API();
	return DI_OK;
}

HRESULT HookIDirectInputDevice::SetEventNotification(LPVOID _this, HANDLE hEvent)
{
	LOG_API();
	return DI_OK;
}

HRESULT HookIDirectInputDevice::SetCooperativeLevel(LPVOID _this, HWND hwnd,
													DWORD dwFlags)
{
	LOG_API();
	return DI_OK;
}

HRESULT HookIDirectInputDevice::GetObjectInfo(LPVOID _this, LPDIDEVICEOBJECTINSTANCE pdidoi,
											  DWORD dwObj,
											  DWORD dwHow)
{
	LOG_API();
	return DI_OK;
}
HRESULT HookIDirectInputDevice::GetDeviceInfo(LPVOID _this, LPDIDEVICEINSTANCE pdidi)
{
	LOG_API();
	return DI_OK;
}

HRESULT HookIDirectInputDevice::RunControlPanel(LPVOID _this, HWND hwndOwner,
												DWORD dwFlags)
{
	LOG_API();
	return DI_OK;
}
HRESULT HookIDirectInputDevice::Initialize(LPVOID _this, HINSTANCE hinst,
										   DWORD dwVersion,
										   REFGUID rguid)
{
	LOG_API();
	return DI_OK;
}

HRESULT HookIDirectInputDevice::CreateEffect(
	LPVOID _this, 
	REFGUID rguid,
	LPCDIEFFECT lpeff,
	LPDIRECTINPUTEFFECT * ppdeff,
	LPUNKNOWN punkOuter)
{
	LOG_API();
	return DIERR_UNSUPPORTED;
}

HRESULT HookIDirectInputDevice::EnumEffects(
	LPVOID _this, 
	LPDIENUMEFFECTSCALLBACK lpCallback,
	LPVOID pvRef,
	DWORD dwEffType
	) { LOG_API();return DIERR_UNSUPPORTED;}

HRESULT HookIDirectInputDevice::GetEffectInfo(
	LPVOID _this, 
	LPDIEFFECTINFOA pdei,
	REFGUID rguid
	) { LOG_API();return DIERR_UNSUPPORTED;}


HRESULT HookIDirectInputDevice::GetForceFeedbackState(
	LPVOID _this, 
	LPDWORD pdwOut
	) { LOG_API();return DIERR_UNSUPPORTED;}

HRESULT HookIDirectInputDevice::SendForceFeedbackCommand(
	LPVOID _this, 
	DWORD dwFlags
	) { LOG_API();return DIERR_UNSUPPORTED;}

HRESULT HookIDirectInputDevice::EnumCreatedEffectObjects(
	LPVOID _this, 
	LPDIENUMCREATEDEFFECTOBJECTSCALLBACK lpCallback,
	LPVOID pvRef,
	DWORD fl
	) { LOG_API();return DIERR_UNSUPPORTED;}

HRESULT HookIDirectInputDevice::Escape(
									   LPVOID _this, 
									   LPDIEFFESCAPE pesc
									   ) { LOG_API();return DIERR_UNSUPPORTED;}

HRESULT HookIDirectInputDevice::Poll(LPVOID _this) { LOG_API();return DI_NOEFFECT ;}

HRESULT HookIDirectInputDevice::SendDeviceData(
	LPVOID _this, 
	DWORD cbObjectData,
	LPCDIDEVICEOBJECTDATA rgdod,
	LPDWORD pdwInOut,
	DWORD fl
	) { LOG_API();return DIERR_UNSUPPORTED;}

HRESULT HookIDirectInputDevice::EnumEffectsInFile(
	LPVOID _this, 
	LPCSTR lpszFileName,
	LPDIENUMEFFECTSINFILECALLBACK pec,
	LPVOID pvRef,
	DWORD dwFlags
	) { LOG_API();return DIERR_UNSUPPORTED;}

HRESULT HookIDirectInputDevice::WriteEffectToFile(
	LPVOID _this, 
	LPCSTR lpszFileName,
	DWORD dwEntries,
	LPCDIFILEEFFECT rgDiFileEft,
	DWORD dwFlags
	) { LOG_API();return DIERR_UNSUPPORTED;}

HRESULT HookIDirectInputDevice::BuildActionMap(
	LPVOID _this, 
	LPDIACTIONFORMAT lpdiaf,
	LPCTSTR lpszUserName,
	DWORD dwFlags
	) { LOG_API();return DIERR_UNSUPPORTED;}

HRESULT HookIDirectInputDevice::SetActionMap(
	LPVOID _this, 
	LPCDIACTIONFORMAT lpdiActionFormat,
	LPCTSTR lptszUserName,
	DWORD dwFlags
	) { LOG_API();return DIERR_UNSUPPORTED;}

HRESULT HookIDirectInputDevice::GetImageInfo(
	LPVOID _this, 
	LPDIDEVICEIMAGEINFOHEADER lpdiDevImageInfoHeader
	) { LOG_API();return DIERR_UNSUPPORTED;}



static HookIDirectInputDevice FakeDevice, *pFakeDevice = &FakeDevice;



#pragma pack(4)
struct FAR HookIDirectInput8
{

	/*** IUnknown methods ***/
	virtual HRESULT QueryInterface(LPVOID _this, REFIID riid,LPVOID *ppvObj);
	virtual ULONG AddRef(LPVOID _this);
	virtual ULONG Release(LPVOID _this);

	/*** IDirectInput8 methods ***/
	virtual HRESULT CreateDevice(LPVOID _this, REFGUID rguid,
		LPDIRECTINPUTDEVICE8A * lplpDirectInputDevice,
		LPUNKNOWN pUnkOuter);

	virtual HRESULT EnumDevices(LPVOID _this, DWORD dwDevType,
		LPDIENUMDEVICESCALLBACK lpCallback,
		LPVOID pvRef,
		DWORD dwFlags);

	virtual HRESULT GetDeviceStatus(LPVOID _this, REFGUID rguidInstance);

	virtual HRESULT RunControlPanel(LPVOID _this, HWND hwndOwner,
		DWORD dwFlags);

	virtual HRESULT Initialize(LPVOID _this,HINSTANCE hinst,
		DWORD dwVersion);

	virtual HRESULT FindDevice(LPVOID _this,REFGUID rguidClass,
		LPCTSTR ptszName,
		LPGUID pguidInstance);

	virtual HRESULT EnumDevicesBySemantics(LPVOID _this,LPCTSTR ptszUserName,
		LPDIACTIONFORMAT lpdiActionFormat,
		LPDIENUMDEVICESBYSEMANTICSCB lpCallback,
		LPVOID pvRef,
		DWORD dwFlags);

	virtual HRESULT ConfigureDevices(LPVOID _this,LPDICONFIGUREDEVICESCALLBACK lpdiCallback,
		LPDICONFIGUREDEVICESPARAMS lpdiCDParams,
		DWORD dwFlags,
		LPVOID pvRefData);
};


HRESULT HookIDirectInput8::QueryInterface(LPVOID _this, REFIID riid,
										  LPVOID *ppvObj)
{
	LOG_API();
	*ppvObj = NULL;
	return DIERR_UNSUPPORTED;
}

ULONG HookIDirectInput8::AddRef(LPVOID _this)
{
	LOG_API();
	return 0;
}

ULONG HookIDirectInput8::Release(LPVOID _this)
{
	LOG_API();
	return 0;
}


HRESULT HookIDirectInput8::CreateDevice(LPVOID _this, REFGUID rguid,
										LPDIRECTINPUTDEVICE8A * lplpDirectInputDevice,
										LPUNKNOWN pUnkOuter)
{
	LOG_API();

	if (rguid == GUID_SysKeyboard)
	{
		logmsg("Keyboard device!\n");
	}
	else
		if (rguid == GUID_SysMouse)
		{
			logmsg("Mouse device!\n");
		}
		else
			if (rguid == GUID_Joystick)
			{
				logmsg("Joy device!\n");
			}

			*lplpDirectInputDevice = (LPDIRECTINPUTDEVICE8A) pFakeDevice;
			if (pUnkOuter) {
				pUnkOuter = (LPUNKNOWN) NULL;
			}
			return DI_OK;

}



static DIDEVICEINSTANCEA devi;
HRESULT HookIDirectInput8::EnumDevices(LPVOID _this,
									   DWORD dwDevType,
									   LPDIENUMDEVICESCALLBACK lpCallback,
									   LPVOID pvRef,
									   DWORD dwFlags)
{
	/* Não enumeramos nada :), pois só damos "acesso" a um keyboard fake ou mouse */
	LOG_API();
	return DI_OK;
}


HRESULT HookIDirectInput8::GetDeviceStatus(LPVOID _this,REFGUID rguidInstance)
{
	LOG_API();
	return DI_OK;
}


HRESULT HookIDirectInput8::RunControlPanel(LPVOID _this,HWND hwndOwner,
										   DWORD dwFlags)
{
	LOG_API();
	return DIERR_UNSUPPORTED;
}

HRESULT HookIDirectInput8::Initialize(LPVOID _this,HINSTANCE hinst,
									  DWORD dwVersion)
{
	LOG_API();
	return DI_OK;
}

HRESULT HookIDirectInput8::FindDevice(LPVOID _this,REFGUID rguidClass,
									  LPCTSTR ptszName,
									  LPGUID pguidInstance)
{
	LOG_API();
	return DI_OK;
}



HRESULT HookIDirectInput8::EnumDevicesBySemantics(LPVOID _this,LPCTSTR ptszUserName,
												  LPDIACTIONFORMAT lpdiActionFormat,
												  LPDIENUMDEVICESBYSEMANTICSCB lpCallback,
												  LPVOID pvRef,
												  DWORD dwFlags)
{
	LOG_API();
	return DI_OK;
}

HRESULT HookIDirectInput8::ConfigureDevices(LPVOID _this,LPDICONFIGUREDEVICESCALLBACK lpdiCallback,
											LPDICONFIGUREDEVICESPARAMS lpdiCDParams,
											DWORD dwFlags,
											LPVOID pvRefData)
{
	LOG_API();
	return DI_OK;
}

static HookIDirectInput8 FakeInterface, *pFakeInterface = &FakeInterface;


HRESULT __stdcall Hook_DirectInput8Create(
	HINSTANCE hinst,
	DWORD dwVersion,
	REFIID riidltf,
	LPVOID * ppvOut,
	LPUNKNOWN punkOuter
	)
{
	LOG_API();
	if (DImagicCall) {
		logmsg("*  ttx_monitor\n");
		return __DirectInput8Create(hinst, dwVersion,
			riidltf,
			ppvOut,
			punkOuter);
	}
	else {
		*ppvOut = (LPVOID) pFakeInterface;
		punkOuter = NULL;
		return DI_OK;
	}
}

