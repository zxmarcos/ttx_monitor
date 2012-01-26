// ============================================================================
// ttx_monitor: Opensource TaitoTypeX loader
// by Romhack
// ============================================================================
#ifndef __TTX_INPUT_H
#define __TTX_INPUT_H
#define DIRECTINPUT_VERSION 0x800
#include <dinput.h>
#include <iostream>
#include <string>
#include <list>
#include "Shared.h"

using namespace std;
#ifndef __TTX_CONFIG_BUILD__
extern DWORD hPollThreadId;
extern int DImagicCall;
#endif


// ============================================================================
//
// ============================================================================
class TTX_InputManager
{
	LPDIRECTINPUT8 di;

	static BOOL EnumDevicesCallback(LPCDIDEVICEINSTANCE lpddi,
		LPVOID pvRef);
	static BOOL EnumDeviceObjectsCallback(
         LPCDIDEVICEOBJECTINSTANCE lpddoi,
         LPVOID pvRef);

	char keyBuffer[256];

	DWORD bindTable[__INPUT_MAX__];
	int stateTable[__INPUT_MAX__];
	DWORD lastPoll;

	list<LPCDIDEVICEINSTANCE> devices;
	list<LPDIRECTINPUTDEVICE8> lpJoysticks;
	LPDIRECTINPUTDEVICE8 lpKeyboard;


	LPDIRECTINPUTDEVICE8 GetJoyDevice(int n);

	
	void Setup();
	HWND hSelfWin;

#ifndef __TTX_CONFIG_BUILD__
	WNDCLASS wndcls;
	HANDLE hPollMutex;
	ATOM clsId;
	static int PollThread(TTX_InputManager *pThis);
	DWORD threadId;
#endif
	
public:
	char strBuf[128];


	TTX_InputManager();
	~TTX_InputManager();

	int Find();
	void Close();
	int Poll();
	int GetState(TTX_InputsDef inp);
	int Save();
	int Load();
	int BindInput(TTX_InputsDef inp, int code);
	int GetInput();
	BOOL Init();


	const char *GetCodeName(TTX_InputsDef inpCode, char *buf) {
		register DWORD val = bindTable[inpCode];

		if (IS_JOY_OBJECT(val)) {
			if (IS_JOY_AXIS(val)) {
				bool isneg = false;

				if (IS_NEGATIVE_AXIS(val))
					isneg = true;

				switch (GET_JOY_AXIS(val)) {
					case AXIS_X:
						sprintf(buf, "Joy%d Axis %cX %i", GET_JOY_NUM(val),
							((isneg) ? '-' : '+'), (signed short) (val & 0xFFFF));
						break;
					case AXIS_Y:
						sprintf(buf, "Joy%d Axis %cY %i", GET_JOY_NUM(val),
							((isneg) ? '-' : '+'), (signed short) (val & 0xFFFF));
						break;

					case POVN(0):
					case POVN(1):
					case POVN(2):
					case POVN(3):
						sprintf(buf, "Joy%d POV%d %i", GET_JOY_NUM(val),
							GET_JOY_AXIS(val)-POVN(0), (signed short) (val & 0xFFFF));
						break;

				}
				
			} else {
				sprintf(buf, "Joy%d Botão %d", GET_JOY_NUM(val), val & 0xFF);
				
			}
		} else {
			char zbuf[128];
			int kCode = val & 0xFF;
			GetKeyNameText(kCode<<16, zbuf, 128);

			sprintf(buf, "Key %s (%d)", zbuf, val & 0xFF);
		}
		return buf;
	}

};

#endif