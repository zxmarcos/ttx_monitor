// ============================================================================
// ttx_monitor: Opensource TaitoTypeX loader
// by Romhack
// ============================================================================

#include <windows.h>
#include <string.h>
#include "stdafx.h"
#include <list>

#define DEBUG_API	0
#define LOG_IO_STREAM	0

static const HANDLE hConnection = (HANDLE) 0xCAFEBEEF;
using namespace std;


std::queue<BYTE> replyBuffer;

static int isInitialized = 0;



// ============================================================================
static LPGetCommModemStatus __GetCommModemStatus = NULL;
static LPEscapeCommFunction __EscapeCommFunction = NULL;
static LPClearCommError __ClearCommError = NULL;
static LPWriteFile __WriteFile = NULL;
static LPReadFile __ReadFile = NULL;
static LPCloseHandle __CloseHandle = NULL;
static LPCreateFile  __CreateFile = NULL;
static LPSetupComm  __SetupComm = NULL;
static LPGetCommState  __GetCommState = NULL;
static LPSetCommState  __SetCommState = NULL;
static LPSetCommMask  __SetCommMask = NULL;
static LPSetCommTimeouts  __SetCommTimeouts = NULL;
static LPGetCommTimeouts  __GetCommTimeouts = NULL;
static LPGetWindowTextA __GetWindowTextA = NULL;
static LPGetWindowTextW __GetWindowTextW = NULL;
static LPGetAsyncKeyState __GetAsyncKeyState = NULL;
static LPCreateWindowExA __CreateWindowExA = NULL;
static LPCreateWindowExW __CreateWindowExW = NULL;
static LPCreateFileA  __CreateFileA = NULL;
static LPCreateFileW  __CreateFileW = NULL;


//////////////////////////////////////////////////////////////////////////////////////
// Unicode/ANSI functions (W/A)
//////////////////////////////////////////////////////////////////////////////////////



HWND __stdcall Hook_CreateWindowExA(DWORD dwExStyle,
									LPCSTR lpClassName,
									LPCSTR lpWindowName,
									DWORD dwStyle,
									int x,
									int y,
									int nWidth,
									int nHeight,
									HWND hWndParent,
									HMENU hMenu,
									HINSTANCE hInstance,
									LPVOID lpParam)
{

	//if (!(dwStyle & WS_CHILD))
	//{
	//	dwExStyle =0;
	//	dwStyle = WS_OVERLAPPEDWINDOW;
	//}
	logmsg("%s(%x,%s,%s,%x,%d,%d,%d,%d,%x,%x,%x,%p)\n", __FUNCTION__,
		dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent,
		hMenu, hInstance, lpParam);

	return __CreateWindowExA(dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent,
		hMenu, hInstance, lpParam);
}


HWND __stdcall Hook_CreateWindowExW(DWORD dwExStyle,
									LPCWSTR lpClassName,
									LPCWSTR lpWindowName,
									DWORD dwStyle,
									int x,
									int y,
									int nWidth,
									int nHeight,
									HWND hWndParent,
									HMENU hMenu,
									HINSTANCE hInstance,
									LPVOID lpParam)
{
	//if (!(dwStyle & WS_CHILD))
	//{
	//	dwExStyle =0;
	//	dwStyle = WS_OVERLAPPEDWINDOW;
	//}

	logmsgw(L"%s(%x,%s,%s,%x,%d,%d,%d,%d,%x,%x,%x,%p)\n", __FUNCTION__,
		dwExStyle, NULL, NULL, dwStyle, x, y, nWidth, nHeight, hWndParent,
		hMenu, hInstance, lpParam);

	return __CreateWindowExW(dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent,
		hMenu, hInstance, lpParam);
}


int __stdcall Hook_GetWindowTextA(HWND hWnd, LPSTR lpString, int nMaxCount)
{
	LPSTR text = "ttx_monitor_by_romhack";
	LPSTR s = text, p = lpString;

	for (int i=0; i<nMaxCount; i++) {
		if (*s)
			*p++ = *s++;
	}
	return TRUE;
}

int __stdcall Hook_GetWindowTextW(HWND hWnd, LPWSTR lpString, int nMaxCount)
{
	LPWSTR text = L"ttx_monitor_by_romhack";
	LPWSTR s = text, p = lpString;

	for (int i=0; i<nMaxCount; i++) {
		if (*s)
			*p++ = *s++;
	}
	return TRUE;
}




// ============================================================================


LPVOID __stdcall SetHookFunction(LPVOID dst, LPVOID lpHookFunction, LPCTSTR name)
{

	DWORD oldProt = 0, newProt = PAGE_EXECUTE_READWRITE;
	VirtualProtect(dst, 4, newProt, &oldProt);

#if 1
	LPVOID ret = (LPVOID) *((DWORD*) dst); 
#else
	LPVOID ret = (LPVOID) GetProcAddress(NULL, name);
#endif
	logmsg("Hooking %s em %08p (%08X) para %08p\n", name, dst, *((DWORD*)dst), lpHookFunction);

	*((DWORD *) dst) = (DWORD) lpHookFunction; 
	VirtualProtect(dst, 4, oldProt, &newProt);

	return ret;
}

#define __HOOK(addr, name)	\
	__ ##name = (LP ##name) SetHookFunction((LPVOID *) addr, &Hook_ ##name, #name)

#define __XHOOKA(mod, name)	\
	__ ##name = (LP ##name) HookIt(mod, #name "A", (LPVOID) &Hook_ ##name)

#define __XHOOKW(mod, name)	\
	__ ##name = (LP ##name) HookIt(mod, #name "W", (LPVOID) &Hook_ ##name)
#define __XHOOKn(mod, name)	\
	__ ##name = (LP ##name) HookIt(mod, #name, (LPVOID) &Hook_ ##name)

#define __XHOOKX(mod, ori, name)	\
	__ ##name = (LP ##name) HookIt(mod, #ori, (LPVOID) &Hook_ ##name)

BOOL __stdcall TTX_HookFunctions()
{
	logmsg("Fazendo o hooking das funções...\n");
	//__XHOOKA("kernel32.dll", CreateFile);

	__XHOOKn("kernel32.dll", CreateFileA);
	__XHOOKn("kernel32.dll", CreateFileW);

	// O samurai trava nessa função, logo...
	__XHOOKn("user32.dll", GetWindowTextA);
	__XHOOKn("user32.dll", GetWindowTextW);

	//__XHOOKn("user32.dll", CreateWindowExA);
	//__XHOOKn("user32.dll", CreateWindowExW);

	__XHOOKn("kernel32.dll", WriteFile);
	__XHOOKn("kernel32.dll", ReadFile);
	__XHOOKn("kernel32.dll", CloseHandle);
	__XHOOKn("kernel32.dll", GetCommModemStatus);
	__XHOOKn("kernel32.dll", EscapeCommFunction);
	__XHOOKn("kernel32.dll", ClearCommError);
	__XHOOKn("kernel32.dll", SetCommMask);
	__XHOOKn("kernel32.dll", SetupComm);
	__XHOOKn("kernel32.dll", GetCommState);
	__XHOOKn("kernel32.dll", SetCommState);
	__XHOOKn("kernel32.dll", SetCommTimeouts);
	__XHOOKn("kernel32.dll", GetCommTimeouts);
	__XHOOKn("DINPUT8.dll", DirectInput8Create);

	// As duas versões são retrocompatíveis.
	//HookIt("D3D8.dll", "Direct3DCreate8", (LPVOID) &Direct3DCreate9);
	//__XHOOKn("D3D9.dll", Direct3DCreate9);

	__XHOOKn("user32.dll", GetAsyncKeyState);

	return TRUE;
}
#undef __HOOK
#undef __XHOOKA
#undef __XHOOKn



SHORT __stdcall Hook_GetAsyncKeyState(int vKey)
{
	return 0;
}

BOOL __stdcall Hook_GetCommModemStatus(HANDLE hFile, LPDWORD lpModemStat)
{
	if (hFile != hConnection) {
		return __GetCommModemStatus(hFile, lpModemStat);
	}
#if DEBUG_API
	logmsg("GetCommModemStatus(%x, %p)\n",
		hFile, lpModemStat);
#endif

	// Só devemos retornar esse valor quando a JVS receber o comando de Address
	if (is_addressed())
		*lpModemStat = 0x10;
	else
		*lpModemStat = 0;

	return TRUE;
}

BOOL __stdcall Hook_EscapeCommFunction(HANDLE hFile, DWORD dwFunc)
{

	if (hFile != hConnection) {
		return __EscapeCommFunction(hFile, dwFunc);
	}
#if DEBUG_API
	logmsg("EscapeCommFunction(%x, %x)\n",
		hFile, dwFunc);
#endif
	return TRUE;
}

BOOL __stdcall Hook_ClearCommError(HANDLE hFile, LPDWORD lpErrors, LPCOMSTAT lpStat)
{
	if (hFile != hConnection) {
		return __ClearCommError(hFile, lpErrors, lpStat);
	}
#if DEBUG_API
	logmsg("ClearCommError(%x, %p, %p)\n",
		hFile, lpErrors, lpStat);
#endif

	if (lpStat) {
		if (!replyBuffer.empty())
			lpStat->cbInQue = replyBuffer.size();
		else
			lpStat->cbInQue = 0;
	}

	return TRUE;
}

BOOL __stdcall Hook_SetupComm(HANDLE hFile, DWORD dwInQueue, DWORD dwOutQueue)
{

	if (hFile != hConnection) {
		return __SetupComm(hFile, dwInQueue, dwOutQueue);
	}
#if DEBUG_API
	logmsg("SetupComm(%x, %d, %d)\n",
		hFile, dwInQueue, dwOutQueue);
#endif
	return TRUE;
}

BOOL __stdcall Hook_GetCommState(HANDLE hFile, LPDCB lpDCB)
{
	if (hFile != hConnection) {
		return __GetCommState(hFile, lpDCB);
	}
#if DEBUG_API
	logmsg("GetCommState(%x, %p)\n",
		hFile, lpDCB);
#endif
	return TRUE;
}

BOOL __stdcall Hook_SetCommState(HANDLE hFile, LPDCB lpDCB)
{
	if (hFile != hConnection) {
		return __SetCommState(hFile, lpDCB);
	}
#if DEBUG_API
	logmsg("SetCommState(%x, %p)\n",
		hFile, lpDCB);
#endif
	return TRUE;
}

BOOL __stdcall Hook_SetCommMask(HANDLE hFile, DWORD dwEvtMask)
{

	if (hFile != hConnection) {
		return __SetCommMask(hFile, dwEvtMask);
	}
#if DEBUG_API
	logmsg("SetCommMask(%x, %x)\n",
		hFile, dwEvtMask);
#endif
	return TRUE;
}

BOOL __stdcall Hook_GetCommTimeouts(HANDLE hFile, LPCOMMTIMEOUTS lpCommTimeouts)
{

	if (hFile != hConnection) {
		return __GetCommTimeouts(hFile, lpCommTimeouts);
	}
#if DEBUG_API
	logmsg("GetCommTimeouts(%x, %p)\n",
		hFile, lpCommTimeouts);
#endif
	return TRUE;
}

BOOL __stdcall Hook_SetCommTimeouts(HANDLE hFile, LPCOMMTIMEOUTS lpCommTimeouts)
{

	if (hFile != hConnection) {
		return __SetCommTimeouts(hFile, lpCommTimeouts);
	}
#if DEBUG_API
	logmsg("SetCommTimeouts(%x, %p)\n",
		hFile, lpCommTimeouts);
#endif
	return TRUE;
}


BOOL __stdcall Hook_WriteFile(HANDLE hFile,
							  LPVOID lpBuffer,
							  DWORD nNumberOfBytesToWrite,
							  LPDWORD lpNumberOfBytesWritten,
							  LPOVERLAPPED lpOverlapped)
{

	if (hFile != hConnection) {
		return __WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
	}
#if DEBUG_API
	logmsg("WriteFile(%x, %p, %d, %p, %p)\n",
		hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
#endif

	BYTE *ptr = (BYTE *) lpBuffer;

	//logmsg("Recebendo %d bytes...\n", nNumberOfBytesToWrite);
#if LOG_IO_STREAM
	logmsg("RD:  ");
	for (DWORD i = 0; i < nNumberOfBytesToWrite; i++) {
		logmsg("%02X ", (DWORD) *ptr);
		++ptr;
	}
	logmsg("\n");
#endif

	*lpNumberOfBytesWritten = nNumberOfBytesToWrite;
	static BYTE rbuffer[1024];
	DWORD sz = process_stream((LPBYTE)lpBuffer, nNumberOfBytesToWrite, rbuffer, 1024);
	if (sz != 1) {
		for (DWORD i=0; i < sz; i++)
			replyBuffer.push(rbuffer[i]);
	}

	return TRUE;
}



BOOL __stdcall Hook_ReadFile(HANDLE hFile,
							 LPVOID lpBuffer,
							 DWORD nNumberOfBytesToRead,
							 LPDWORD lpNumberOfBytesRead,
							 LPOVERLAPPED lpOverlapped)
{


	if (hFile != hConnection) {
		return __ReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
	}
#if DEBUG_API
	logmsg("ReadFile(%x, %p, %d, %p, %p)\n",
		hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
#endif

	if (replyBuffer.size())
	{
		if (nNumberOfBytesToRead >= replyBuffer.size())
			nNumberOfBytesToRead = replyBuffer.size();
		*lpNumberOfBytesRead = nNumberOfBytesToRead;
		BYTE *ptr = (BYTE*) lpBuffer;
		for (DWORD i=0; i < nNumberOfBytesToRead; i++) {
			if (!replyBuffer.empty()) {
				*ptr++ = replyBuffer.front();
				replyBuffer.pop();
			} else
				*ptr++ = 0;
		}
#if LOG_IO_STREAM
		//logmsg("Lidos %d\n", nNumberOfBytesToRead);
		ptr = (BYTE*) lpBuffer;
		logmsg("SD:  ");
		for (DWORD i=0; i < nNumberOfBytesToRead; i++) {
			logmsg("%02X ", (DWORD) *ptr++);
		}
		logmsg("\n");
#endif
	} else {
		*lpNumberOfBytesRead = 0;
		return TRUE;
	}


	return TRUE;
}


BOOL __stdcall Hook_CloseHandle(HANDLE hObject)
{

	if (hObject != hConnection) {
		return __CloseHandle(hObject);
	} else
		reset_addressed();
#if DEBUG_API
	logmsg("CloseHandle(%x)\n", hObject);
#endif
	return TRUE;
}


static int __createfile_nested = 0;
//
//
//
HANDLE __stdcall Hook_CreateFile(LPCTSTR lpFileName,
								 DWORD dwDesiredAccess,
								 DWORD dwShareMode,
								 LPSECURITY_ATTRIBUTES lpSecurityAttributes,
								 DWORD dwCreationDisposition,
								 DWORD dwFlagsAndAttributes,
								 HANDLE hTemplateFile)
{
#if 0//DEBUG_API
	if (!__createfile_nested) {
		__createfile_nested = 1;
		logmsg("CreateFile(%s, %x, %x, %p, %x, %x, %x)\n",
			lpFileName,
			dwDesiredAccess,
			dwShareMode,
			lpSecurityAttributes,
			dwCreationDisposition,
			dwFlagsAndAttributes,
			hTemplateFile);
		__createfile_nested = 0;
	}
#endif
	if (!strcmp(lpFileName, "COM2")) {
		if (!isInitialized) {
			inputMgr.Init();
			isInitialized = 1;

		}
		return hConnection;

	} else {
		return __CreateFile(lpFileName,
			dwDesiredAccess,
			dwShareMode,
			lpSecurityAttributes,
			dwCreationDisposition,
			dwFlagsAndAttributes,
			hTemplateFile);
	}
}




int _mbsnbcmp(const char *a, const char *b, int l)
{
	int ret = 0;
	for (int i=0;i<l;i++) {
		ret += ((int)*a - (int)*b);
		++a;
		++b;
	}
	return ret;
}

HANDLE __stdcall Hook_CreateFileA(LPCSTR lpFileName,
								  DWORD dwDesiredAccess,
								  DWORD dwShareMode,
								  LPSECURITY_ATTRIBUTES lpSecurityAttributes,
								  DWORD dwCreationDisposition,
								  DWORD dwFlagsAndAttributes,
								  HANDLE hTemplateFile)
{
#if 1//DEBUG_API
	if (!__createfile_nested) {
		__createfile_nested = 1;
		logmsg("CreateFile(%s, %x, %x, %p, %x, %x, %x)\n",
			lpFileName,
			dwDesiredAccess,
			dwShareMode,
			lpSecurityAttributes,
			dwCreationDisposition,
			dwFlagsAndAttributes,
			hTemplateFile);
		__createfile_nested = 0;
	}
#endif
	if (!strcmp(lpFileName, "COM2")) {
		if (!isInitialized) {
			inputMgr.Init();
			isInitialized = 1;

		}
		return hConnection;

	}
#if 1
	else
	{

		if (!(_mbsnbcmp(lpFileName, "D:\\", 3)))
		{
			HANDLE hTemp = __CreateFileA(lpFileName, GENERIC_READ,
				FILE_SHARE_READ | FILE_SHARE_WRITE,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL,
				NULL);
			if (hTemp == INVALID_HANDLE_VALUE)
			{

				if ((_mbsnbcmp(lpFileName, "D:\\WINDOWS", 10)))
				{
					static char pathbuf[MAX_PATH];
					char *pstr, *str = strdup(lpFileName);
					pstr = str;
					while (*pstr != '\\')
						pstr++;
					*pstr = '/';

					sprintf(pathbuf, "%s%s", "save", pstr);
					logmsg("PATH: %s\n", pathbuf);

					char *_path = strdup(pathbuf);
					pstr = _path;
					char *lastSlash = NULL;
					while (*pstr) {
						if (*pstr == '\\')
							*pstr = '/';
						if (*pstr == '/')
							lastSlash = pstr;
						++pstr;
					}
					if (lastSlash) {
						*lastSlash = '\0';
						logmsg("Criando diretório %s\n", _path);
						CreateDirectory(_path, NULL);
						free(_path);
					}

					HANDLE ret = __CreateFileA(pathbuf,
						dwDesiredAccess,
						dwShareMode,
						lpSecurityAttributes,
						dwCreationDisposition,
						dwFlagsAndAttributes,
						hTemplateFile);

					free(str);

					return ret;
				}
			}
			else
				CloseHandle(hTemp);
		}
	}
#endif
	return __CreateFileA(lpFileName,
		dwDesiredAccess,
		dwShareMode,
		lpSecurityAttributes,
		dwCreationDisposition,
		dwFlagsAndAttributes,
		hTemplateFile);
}

HANDLE __stdcall Hook_CreateFileW(LPCWSTR lpFileName,
								  DWORD dwDesiredAccess,
								  DWORD dwShareMode,
								  LPSECURITY_ATTRIBUTES lpSecurityAttributes,
								  DWORD dwCreationDisposition,
								  DWORD dwFlagsAndAttributes,
								  HANDLE hTemplateFile)
{
#if 0//DEBUG_API
	if (!__createfile_nested) {
		__createfile_nested = 1;
		logmsg("CreateFile(%s, %x, %x, %p, %x, %x, %x)\n",
			lpFileName,
			dwDesiredAccess,
			dwShareMode,
			lpSecurityAttributes,
			dwCreationDisposition,
			dwFlagsAndAttributes,
			hTemplateFile);
		__createfile_nested = 0;
	}
#endif
	if (!wcscmp(lpFileName, L"COM2")) {
		if (!isInitialized) {
			inputMgr.Init();
			isInitialized = 1;

		}
		return hConnection;

	} else {
		return __CreateFileW(lpFileName,
			dwDesiredAccess,
			dwShareMode,
			lpSecurityAttributes,
			dwCreationDisposition,
			dwFlagsAndAttributes,
			hTemplateFile);
	}
}