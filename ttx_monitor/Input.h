#ifndef __TTX_INPUT_H
#define __TTX_INPUT_H

#include <dinput.h>
#include <iostream>
#include <string>
#include <list>

using namespace std;

// ============================================================================
//
// ============================================================================
#define IS_JOY_OBJECT(n)	(n & 0x80000000)
#define IS_JOY_AXIS(n)		(n & 0x00030000)
#define GET_JOY_NUM(n)		((n >> 24) & 0x0F)
#define GET_JOY_BUT(n)		(n & 0xFF)
#define GET_JOY_AXIS(n)		((n >> 16) & 0xF)
#define IS_NEGATIVE_AXIS(n)	((n >> 20) & 1)
#define JOYV(joy, v)		(0x80000000 | ((joy & 0x0F) << 24) | (v & 0xFFFFFF))
#define KEYV(vk)			(vk)
#define JOYB(n)				(n & 0xFF)
#define JOYAXIS(a, n, v)	(((n & 1) << 20) | ((a & 0xF) << 16) | (v & 0xFFFF))
#define AXIS_X				1
#define AXIS_Y				2
#define AXIS_Z				3
#define MAX_AXIS_VAL		1000
#define DEADZONE_DIV		100
#define DEADZONE			(MAX_AXIS_VAL / DEADZONE_DIV)
#define POLL_INTERVAL		0
// ============================================================================
//
// ============================================================================
typedef enum
{
	P1_START = 0,
	P1_COIN,
	P1_SERVICE,
	P1_UP,
	P1_DOWN,
	P1_LEFT,
	P1_RIGHT,
	P1_BUTTON_1,
	P1_BUTTON_2,
	P1_BUTTON_3,
	P1_BUTTON_4,
	P1_BUTTON_5,
	P1_BUTTON_6,
	P2_START,
	P2_COIN,
	P2_SERVICE,
	P2_UP,
	P2_DOWN,
	P2_LEFT,
	P2_RIGHT,
	P2_BUTTON_1,
	P2_BUTTON_2,
	P2_BUTTON_3,
	P2_BUTTON_4,
	P2_BUTTON_5,
	P2_BUTTON_6,
	__INPUT_MAX__,
} TTX_InputsDef;


// ============================================================================
//
// ============================================================================
class TTX_InputManager
{
	
	HWND hSelf;
	WNDCLASS wndcls;
	LPDIRECTINPUT8 di;
	static BOOL EnumDevicesCallback(LPCDIDEVICEINSTANCE lpddi,
		LPVOID pvRef);
	static BOOL EnumDeviceObjectsCallback(
         LPCDIDEVICEOBJECTINSTANCE lpddoi,
         LPVOID pvRef);

	char keyBuffer[256];

	DWORD bindTable[__INPUT_MAX__];
	bool stateTable[__INPUT_MAX__];
	DWORD lastPoll;

	list<LPCDIDEVICEINSTANCE> devices;
	list<LPDIRECTINPUTDEVICE8> lpJoysticks;
	LPDIRECTINPUTDEVICE8 lpKeyboard;

	LPDIRECTINPUTDEVICE8 GetJoyDevice(int n);
	bool isInitialized;
	int Find();
	static LRESULT CALLBACK WindowProc(HWND hwnd,
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam);

	static void PollThread(void *arg) {
		TTX_InputManager *p = (TTX_InputManager *) arg;

		for (;;) {
			p->Poll2();
			Sleep(10);
		}
	}
public:
	bool IsInit() { return isInitialized; }
	TTX_InputManager();
	~TTX_InputManager();

	void SetHSELF(HWND _hSelf) {
		hSelf=_hSelf;
	}
	int Poll();
	int Poll2();
	bool GetState(TTX_InputsDef inp);
	int Save();
	int Load();
	int BindInput(TTX_InputsDef inp, int code);
	int GetInput();
	BOOL Init();


};

#endif