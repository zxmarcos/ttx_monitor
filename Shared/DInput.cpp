// ============================================================================
// ttx_monitor: Opensource TaitoTypeX loader
// by Romhack
// ============================================================================
#include <dinput.h>
#include <iostream>
#include <string>
#include <list>

#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")

#include "Shared.h"
#include "DInput.h"

using namespace std;



// ============================================================================
//
// ============================================================================
TTX_InputManager::TTX_InputManager()
{
	lastPoll = 0;
	for (int i = P1_START; i < __INPUT_MAX__; i++)
		stateTable[i] = 0;
	di = NULL;
}

// ============================================================================
//
// ============================================================================]
TTX_InputManager::~TTX_InputManager()
{
#ifndef __TTX_CONFIG_BUILD__
	for (list<LPDIRECTINPUTDEVICE8>::iterator pdev = lpJoysticks.begin(); pdev != lpJoysticks.end(); pdev++)
		(*pdev)->Release();
	lpKeyboard->Release();
#endif
}


// ============================================================================
//
// ============================================================================
int TTX_InputManager::Poll()
{
#ifndef __TTX_CONFIG_BUILD__
	WaitForSingleObject(hPollMutex, INFINITE);
	DWORD now = GetTickCount();
	//if (now < (lastPoll + POLL_INTERVAL))
	//	goto _Exit;

	static DIJOYSTATE2 js[16];

	for (list<LPDIRECTINPUTDEVICE8>::iterator pdev = lpJoysticks.begin(); pdev != lpJoysticks.end(); pdev++) {
		HRESULT res = (*pdev)->Poll();
		switch (res) {
			case DIERR_NOTACQUIRED:
			case DIERR_INPUTLOST:
				logmsg("Houve algum erro :/\n");
				res = (*pdev)->Acquire();
				if (res != DI_OK) 
					logmsg("Perdemos o controle :/\n");
				break;
		}
	}


	lpKeyboard->GetDeviceState(sizeof(keyBuffer), &keyBuffer);

	// POG para fazer o polling apenas uma vez em cada joystick
	bool isPolled[16];
	for (int i = 0; i < 16; i++)
		isPolled[i] = false;

	for (int i = P1_START; i < __INPUT_MAX__; i++) {
		DWORD val = bindTable[i];
		// verifica se é um comando de origem do joystick
		if (IS_JOY_OBJECT(val)) {
			DWORD joyNum = GET_JOY_NUM(val);
			// pega o joystick de origem
			LPDIRECTINPUTDEVICE8 lpDev = GetJoyDevice(joyNum);
			if (!lpDev) {
				stateTable[i] = 0;
				continue;
			}

			// continuando com a POG kkkkkkkk
			if (!isPolled[joyNum]) {
				HRESULT res = lpDev->GetDeviceState(sizeof(DIJOYSTATE2), &js[joyNum]);
				switch (res)
				{
				case DIERR_NOTACQUIRED:
				case DIERR_INPUTLOST:
					logmsg("Houve algum erro :/\n");
					res = lpDev->Acquire();
					if (res != DI_OK) 
						logmsg("Perdemos o controle :/\n");
					break;
				}
				isPolled[joyNum] = true;
			}

			if (IS_JOY_AXIS(val)) {
				stateTable[i] = 0;
				switch (GET_JOY_AXIS(val))
				{
				case AXIS_X:
					if (IS_NEGATIVE_AXIS(val)) {
						if (js[joyNum].lX < -DEADZONE)
							stateTable[i] = 1;
					} else {
						if (js[joyNum].lX > DEADZONE)
							stateTable[i] = 1;
					}
					break;

				case AXIS_Y:
					if (IS_NEGATIVE_AXIS(val)) {
						if (js[joyNum].lY < -DEADZONE)
							stateTable[i] = 1;
					} else {
						if (js[joyNum].lY > DEADZONE)
							stateTable[i] = 1;
					}
					break;
				case POVN(0):
				case POVN(1):
				case POVN(2):
				case POVN(3):
					{
					WORD dir = js[joyNum].rgdwPOV[POVN(0)-GET_JOY_AXIS(val)];
				//	if (js[joyNum].rgdwPOV[POVN(0)-GET_JOY_AXIS(val)] == GET_JOY_RANGE(val))
				//		stateTable[i] = 1;
					stateTable[i] = 0;
					switch (GET_JOY_RANGE(val))
					{
					case POV_UP:
						if ((dir == POV_UP) || (dir == POV_UP_RIGHT) || (dir == POV_LEFT_UP))
							stateTable[i] = 1;
						break;

					case POV_RIGHT:
						if ((dir == POV_RIGHT) || (dir == POV_UP_RIGHT) || (dir == POV_RIGHT_DOWN))
							stateTable[i] = 1;
						break;

					case POV_DOWN:
						if ((dir == POV_DOWN) || (dir == POV_RIGHT_DOWN) || (dir == POV_DOWN_LEFT))
							stateTable[i] = 1;
						break;

					case POV_LEFT:
						if ((dir == POV_LEFT) || (dir == POV_DOWN_LEFT) || (dir == POV_LEFT_UP))
							stateTable[i] = 1;
						break;

					case POV_UP_RIGHT:
						if (dir == POV_UP_RIGHT)
							stateTable[i] = 1;
						break;

					case POV_RIGHT_DOWN:
						if (dir == POV_RIGHT_DOWN)
							stateTable[i] = 1;
						break;

					case POV_DOWN_LEFT:
						if (dir == POV_DOWN_LEFT)
							stateTable[i] = 1;
						break;

					case POV_LEFT_UP:
						if (dir == POV_LEFT_UP)
							stateTable[i] = 1;
						break;
					
					}

					break;
					}
				}

			} else {
				int but = GET_JOY_BUT(val);
				if (js[joyNum].rgbButtons[but] & 0x80)
					stateTable[i] = 1;
				else
					stateTable[i] = 0;
			}
		} else {
			if (keyBuffer[val & 0xFF] & 0x80)
				stateTable[i] = 1;
			else
				stateTable[i] = 0;
		}
	}
	lastPoll = now;
//_Exit:
	ReleaseMutex(hPollMutex);
#endif
	return 1;
}
// ============================================================================
//
//
// ============================================================================
int TTX_InputManager::Find()
{
	DIJOYSTATE2 js;

	for (list<LPDIRECTINPUTDEVICE8>::iterator pdev = lpJoysticks.begin(); pdev != lpJoysticks.end(); pdev++) {
		(*pdev)->Poll();
		if ((*pdev)->GetDeviceState(sizeof( DIJOYSTATE2 ), &js) == DI_OK) {

			if (js.lX > DEADZONE)
				return JOYV(distance(lpJoysticks.begin(), pdev), JOYAXIS(AXIS_X, 0, js.lX)); /* js.lX)); */

			if (js.lX < -DEADZONE)
				return JOYV(distance(lpJoysticks.begin(), pdev), JOYAXIS(AXIS_X, 1, js.lX));

			if (js.lY > DEADZONE)
				return JOYV(distance(lpJoysticks.begin(), pdev), JOYAXIS(AXIS_Y, 0, js.lY));/* js.lY)) */
			if (js.lY < -DEADZONE)
				return JOYV(distance(lpJoysticks.begin(), pdev), JOYAXIS(AXIS_Y, 1, js.lY));


			for (int i = 0; i < 128; i++) {
				if (js.rgbButtons[i] & 0x80) {
					return JOYV(distance(lpJoysticks.begin(), pdev), JOYB(i));
				}
			}

			for (int i = 0; i < 4; i++) {
				if (js.rgdwPOV[i] != -1) {
					WORD povDir = 0;
					switch (js.rgdwPOV[i])
					{
					case POV_UP: povDir = POV_UP; break;
					case POV_RIGHT: povDir = POV_RIGHT; break;
					case POV_DOWN: povDir = POV_DOWN; break;
					case POV_LEFT: povDir = POV_LEFT; break;

					case POV_UP_RIGHT: povDir = POV_UP_RIGHT; break;
					case POV_RIGHT_DOWN: povDir = POV_RIGHT_DOWN; break;
					case POV_DOWN_LEFT: povDir = POV_DOWN_LEFT; break;
					case POV_LEFT_UP: povDir = POV_LEFT_UP; break;
					}
					//return JOYV(distance(lpJoysticks.begin(), pdev), JOYAXIS(POVN(i), 0, js.rgdwPOV[i]));
					return JOYV(distance(lpJoysticks.begin(), pdev), JOYAXIS(POVN(i), 0, povDir));
				}
			}
		}
	}
	if (lpKeyboard->GetDeviceState(sizeof(keyBuffer), (LPVOID) keyBuffer) == DI_OK) {
		for (int i = 0; i < 256; i++) {
			if (keyBuffer[i] & 0x80) {
				return KEYV(i);
			}
		}
	}
	return -1;
}


// ============================================================================
//
// ============================================================================
int TTX_InputManager::BindInput(TTX_InputsDef inp, int code)
{
	bindTable[inp] = code;
	return 0;
}


// ============================================================================
//
// ============================================================================
int TTX_InputManager::GetInput()
{
	int result = -1;
	DWORD initTime = GetTickCount();
	DWORD currTime = initTime;

	while (currTime < (initTime + 5000))
	{
		result = Find();
		if (result != -1)
			break;
		Sleep(30);
		currTime = GetTickCount();
	}
	if (result != -1) {
		logmsg(" %X\n", result);
	} else
		logmsg("Não encontrado!\n");
	return result;
}

// ============================================================================
// Callback para realizar a enumeração
// ============================================================================
BOOL TTX_InputManager::EnumDevicesCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
	TTX_InputManager *im = (TTX_InputManager *) pvRef;
	im->devices.push_back(lpddi);

	LPDIRECTINPUTDEVICE8 lpdev;
	if (im->di->CreateDevice(lpddi->guidInstance, &lpdev, NULL) == DI_OK)
	{
		logmsg("Device: %08X %08X %08X  %s \n", 
			lpddi->dwDevType, lpddi->wUsage, lpddi->wUsagePage, lpddi->tszInstanceName );


		lpdev->SetDataFormat(&c_dfDIJoystick2);
		if (lpdev->SetCooperativeLevel(im->hSelfWin,
			DISCL_EXCLUSIVE | DISCL_BACKGROUND ) != DI_OK)
			logmsg("Falha ao especificar nivel de cooperatividade!\n");

		im->lpJoysticks.push_back(lpdev);
		lpdev->EnumObjects((LPDIENUMDEVICEOBJECTSCALLBACK) &TTX_InputManager::EnumDeviceObjectsCallback,
			lpdev,
			DIDFT_ALL);

		return DIENUM_CONTINUE;
	}
	return DIENUM_STOP;
}

// ============================================================================
//
// ============================================================================
BOOL TTX_InputManager::EnumDeviceObjectsCallback(
	LPCDIDEVICEOBJECTINSTANCE lpddoi,
	LPVOID pvRef)
{
	
	LPDIRECTINPUTDEVICE8 dev = (LPDIRECTINPUTDEVICE8) pvRef;

	if (lpddoi->dwType & DIDFT_AXIS) {
		DIPROPRANGE diprg;
		diprg.diph.dwSize = sizeof( DIPROPRANGE );
		diprg.diph.dwHeaderSize = sizeof( DIPROPHEADER );
		diprg.diph.dwHow = DIPH_BYID;
		diprg.diph.dwObj = lpddoi->dwType; // Specify the enumerated axis
		diprg.lMin = -MAX_AXIS_VAL;
		diprg.lMax = +MAX_AXIS_VAL;
		dev->SetProperty(DIPROP_RANGE, &diprg.diph);

		DIPROPDWORD dipdw;
		memset(&dipdw, 0, sizeof( DIPROPDWORD ));
		dipdw.diph.dwSize = sizeof( DIPROPDWORD );
		dipdw.diph.dwHeaderSize =  sizeof(DIPROPHEADER);
		dipdw.diph.dwHow = DIPH_BYID;
		dipdw.diph.dwObj = lpddoi->dwType;
		dipdw.dwData = DEADZONE;

		dev->SetProperty(DIPROP_DEADZONE, &dipdw.diph);
	}


	logmsg("\t%08X %08X %08X  ", lpddoi->dwOfs, lpddoi->dwType, lpddoi->dwFlags);
	logmsg("%s\n", lpddoi->tszName);
	return DIENUM_CONTINUE;
}


int TTX_InputManager::Load()
{
#if 1
	FILE *fp = fopen(MONITOR_INPUT_FILENAME, "r");
	if (!fp) {
		logmsg("Não foi possível ler o arquivo: %s!\n", MONITOR_INPUT_FILENAME);
		return 1;
	}

	fread(bindTable, __INPUT_MAX__ * sizeof(DWORD), 1, fp);
	fclose(fp);
#else
	DWORD dwDisp = 0;
	HKEY hKey = 0;
	DWORD dwType = 0;
	DWORD dwSize = 4 * __INPUT_MAX__;
	RegCreateKeyEx(HKEY_LOCAL_MACHINE,
		"Software\\ttx_monitor",
		0,
		NULL,
		0,
		KEY_ALL_ACCESS,
		NULL,
		&hKey,
		&dwDisp);
	if (!dwDisp) {
           MessageBox(0,"kkk","hhhh",MB_OK); 
		   return 1;
	}
	RegQueryValueEx(hKey, MONITOR_INPUT_FILENAME, NULL, &dwType, (BYTE *) bindTable, &dwSize);
	RegCloseKey(hKey);
#endif

	for (int i = P1_START; i < __INPUT_MAX__; i++) {
		register DWORD val = bindTable[i];
		logmsg("%08X  ", val);
		if (IS_JOY_OBJECT(val)) {
			logmsg("Joy%d ", GET_JOY_NUM(val));
			if (IS_JOY_AXIS(val)) {

				if (GET_JOY_AXIS(val) < POVN(0))
					logmsg("Axis ");
				else
					logmsg("POV", GET_JOY_AXIS(val));
				switch (GET_JOY_AXIS(val)) {
					case AXIS_X: logmsg("X"); break;
					case AXIS_Y: logmsg("Y"); break;
					case POVN(0): logmsg("0"); break;
					case POVN(1): logmsg("1"); break;
					case POVN(2): logmsg("2"); break;
					case POVN(3): logmsg("3"); break;
				}
				if (IS_NEGATIVE_AXIS(val))
					logmsg("_Negative");
				logmsg(" %d", (signed short)(val & 0xFFFF));
			} else
				logmsg("Botão %d", val & 0xFF);
		} else {
			logmsg("KEY %d", val & 0xFF);
		}
		logmsg("\n");
	}

	
	return 0;
}
int TTX_InputManager::Save()
{
#if 1
	FILE *fp = fopen(MONITOR_INPUT_FILENAME, "w");
	if (!fp) {
		logmsg("Não foi possível criar o arquivo: %s\n", MONITOR_INPUT_FILENAME);
		return 1;
	}

	fwrite(bindTable, __INPUT_MAX__ * sizeof(DWORD), 1, fp);
	fclose(fp);
	return 0;
#else
	DWORD dwDisp = 0;
	HKEY hKey = 0;

	RegCreateKeyEx(HKEY_LOCAL_MACHINE,
		"Software\\ttx_monitor",
		0,
		NULL,
		0,
		KEY_ALL_ACCESS,
		NULL,
		&hKey,
		&dwDisp);
	if (!dwDisp) {
           MessageBox(0,"kkk","hhhh",MB_OK); 
		   return 1;
	}
	RegSetValueEx(hKey, MONITOR_INPUT_FILENAME, 0, REG_BINARY, (BYTE*) bindTable, 4*__INPUT_MAX__);
	RegCloseKey(hKey);
	return 0;
#endif
}


LPDIRECTINPUTDEVICE8 TTX_InputManager::GetJoyDevice(int n) {

	int i = 0;
	if (lpJoysticks.empty())
		return NULL;

	for (list<LPDIRECTINPUTDEVICE8>::iterator pdev = lpJoysticks.begin();
		pdev != lpJoysticks.end();
		pdev++) {
			if (i == n)
				return *pdev;
			else
				i++;
	}
	return NULL;
}


int TTX_InputManager::GetState(TTX_InputsDef inp) {
	return stateTable[inp];
}

#ifndef __TTX_CONFIG_BUILD__
int DImagicCall = 0;
DWORD hPollThreadId = 0;
int TTX_InputManager::PollThread(TTX_InputManager *pThis)
{
	HANDLE currThread = GetCurrentThread();
	hPollThreadId = GetCurrentThreadId();
	SetThreadPriority(currThread, 1);

	pThis->Setup();
	pThis->Load();
	for (;;) {
		pThis->Poll();

		if (pThis->GetState(TTX_EXIT_CODE)) {
			logmsg("Opa, é hora de dar tchau :)\n");
			ExitProcess(-1);
		}
#if 0
		for(int i=P1_START;i<__INPUT_MAX__;i++)
			logmsg("%d", pThis->stateTable[i]);
		logmsg("\n");
#endif
		Sleep(20);
	}
	return 0;
}
#endif



void TTX_InputManager::Setup()
{
#ifndef __TTX_CONFIG_BUILD__
	logmsg("Configurando a bagaça...\n");
	hPollMutex = CreateMutex(0, 0, "poll_mutex");
	ZeroMemory(&wndcls, sizeof(WNDCLASS));
	wndcls.lpfnWndProc = DefWindowProcA;
	wndcls.hInstance = GetModuleHandle(0);
	wndcls.hbrBackground = (HBRUSH) 2;
	wndcls.lpszClassName = "dinput_fdp";
	clsId = RegisterClass(&wndcls);
	if (!clsId) {
		DWORD err = GetLastError();
		if (err != ERROR_CLASS_ALREADY_EXISTS) {
			logmsg("Não foi possível registrar a classe! %d\n", err);
			ExitProcess(-1);
		}
	}
	hSelfWin = CreateWindowEx(0,
		"dinput_fdp",
		"dinput_muito_fdp",
		WS_POPUP,
		0,
		0,
		0,
		0,
		NULL,
		NULL,
		GetModuleHandle(0),
		NULL);
	if (!hSelfWin) {
		logmsg("Não foi possível criar a janela!\n");
		ExitProcess(-2);
	}
	DImagicCall = 1;
	if (DirectInput8Create(GetModuleHandle(0), 
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(LPVOID *) &di,
		NULL) != DI_OK) {
			logmsg("Erro ao iniciar DirectInput8\n");
			ExitProcess(-3);
	}
	DImagicCall = 0;

#else
	hSelfWin = GetConsoleWindow();
	if (DirectInput8Create(GetModuleHandle(0), 
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(LPVOID *) &di,
		NULL) != DI_OK) {
			logmsg("Erro ao iniciar DirectInput8\n");
			ExitProcess(-3);
	}
#endif

	di->EnumDevices(DI8DEVCLASS_GAMECTRL,
		(LPDIENUMDEVICESCALLBACKA) &TTX_InputManager::EnumDevicesCallback,
		(LPVOID) this,
		0);


	for (list<LPDIRECTINPUTDEVICE8>::iterator i = lpJoysticks.begin(); i != lpJoysticks.end(); i++)
		(*i)->Acquire();

	di->CreateDevice(GUID_SysKeyboard, &lpKeyboard, NULL);
	lpKeyboard->SetDataFormat(&c_dfDIKeyboard);

	lpKeyboard->SetCooperativeLevel(hSelfWin,
		DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);

	lpKeyboard->Acquire();

}

// ============================================================================
//
// ============================================================================
BOOL TTX_InputManager::Init()
{
#ifndef __TTX_CONFIG_BUILD__
	DWORD id = 0;
	CreateThread(0,
		0,
		(LPTHREAD_START_ROUTINE) &TTX_InputManager::PollThread, (LPVOID) this, 0, &id);
	//CreateThread(0, 0, (LPTHREAD_START_ROUTINE) &TTX_InputManager::PollThread, (LPVOID) this, 0, &id);
#else
	Setup();
#endif
	return TRUE;
}

void TTX_InputManager::Close()
{
#ifndef __TTX_CONFIG_BUILD__
#endif
}




