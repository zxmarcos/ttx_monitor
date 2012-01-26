// ttx_config2.0.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <windowsx.h>
#include "resource.h"
#include <CommCtrl.h>
#include "../Shared/Shared.h"
#include "../Shared/DInput.h"

#pragma comment(lib, "ComCtl32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


TTX_InputManager inputMgr;
TTX_ConfigManager configMgr;



struct InputBind {
	char *name;
	TTX_InputsDef index;
};

#define INPDLG_FOUND		WM_USER+1
#define INPDLG_NOTFOUND		WM_USER+2
#define INPDLG_COUNTDOWN	WM_USER+3

InputBind InputTable[__INPUT_MAX__] = {
	{ "P1 Start",	P1_START },
	{ "P1 Coin",	P1_COIN },
	{ "P1 Service",	P1_SERVICE },
	{ "P1 Up",		P1_UP },
	{ "P1 Down",	P1_DOWN },
	{ "P1 Left",	P1_LEFT },
	{ "P1 Right",	P1_RIGHT },
	{ "P1 But 1",	P1_BUTTON_1 },
	{ "P1 But 2",	P1_BUTTON_2 },
	{ "P1 But 3",	P1_BUTTON_3 },
	{ "P1 But 4",	P1_BUTTON_4 },
	{ "P1 But 5",	P1_BUTTON_5 },
	{ "P1 But 6",	P1_BUTTON_6 },
	{ "P2 Start",	P2_START },
	{ "P2 Coin",	P2_COIN },
	{ "P2 Service",	P2_SERVICE },
	{ "P2 Up",		P2_UP },
	{ "P2 Down",	P2_DOWN },
	{ "P2 Left",	P2_LEFT },
	{ "P2 Right",	P2_RIGHT },
	{ "P2 But 1",	P2_BUTTON_1 },
	{ "P2 But 2",	P2_BUTTON_2 },
	{ "P2 But 3",	P2_BUTTON_3 },
	{ "P2 But 4",	P2_BUTTON_4 },
	{ "P2 But 5",	P2_BUTTON_5 },
	{ "P2 But 6",	P2_BUTTON_6 },
	{ "Test Mode",  TEST_MODE },
	{ "Exit",		TTX_EXIT_CODE },
};

HINSTANCE hInstance = 0;
HWND hInputList = 0;
HWND hWaitForInput = 0;
HWND hMainDlg = 0;

// Fiquei pensando muito tempo como fazer uma janelinha que ficasse esperando
// por um input, depois de reescrever várias vezes, o método de utilizar
// uma thread pra fazer o polling e enviar uma mensagem WM_USER pra janela,
// é simplesmente perfeito, não só isso, podemos também fazer o timeout facilmente.
// Uma vez que a função do Dialogo mesmo é só retornar o valor do input, nossa thread
// fica encarregada de fazer o resto :-)
static void WaitForInput(void *arg)
{
	HWND hParent = (HWND) arg;
	int result = -1;
	DWORD initTime = GetTickCount();
	DWORD currTime = initTime;

	while (currTime < (initTime + 5000))
	{
		result = inputMgr.Find();
		if (result != -1)
			break;
		Sleep(30);
		currTime = GetTickCount();
	}
	if (result != -1)
		SendMessage(hParent, INPDLG_FOUND, 0, result);
	else
		SendMessage(hParent, INPDLG_NOTFOUND, 0, result);
}

BOOL CALLBACK  WaitForInputDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HANDLE hWaitThread;
	static DWORD threadId;

	switch (uMsg)
	{
	case WM_CLOSE:

		EndDialog(hwnd, -1);
		break;

	case WM_CHAR:
		return TRUE;

	case WM_INITDIALOG:
		{			
			hWaitThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) WaitForInput,
				(LPVOID) hwnd, 0, &threadId);
			break;
		}
	case INPDLG_NOTFOUND:
	case INPDLG_FOUND:
		EndDialog(hwnd, lParam);
		break;
	}
	return FALSE;
}

BOOL CALLBACK  MainDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{ 
	switch (uMsg)
	{
	case WM_CLOSE:
		ExitProcess(-1);
		break;
	case  WM_INITDIALOG:
		{
			hMainDlg = hwnd;
			configMgr.Load();
			inputMgr.Init();
			inputMgr.Load();


			LVCOLUMN col;

			HWND hHiresCheck = GetDlgItem(hwnd, IDC_CHECK_HIRES);
			if (configMgr.GetConfig(TTX_CONFIG_HIRES))
				Button_SetCheck(hHiresCheck, BST_CHECKED);
			else
				Button_SetCheck(hHiresCheck, BST_UNCHECKED);

			HWND hWindowedCheck = GetDlgItem(hwnd, IDC_CHECK_WINDOWED);
			if (configMgr.GetConfig(TTX_CONFIG_WINDOWED))
				Button_SetCheck(hWindowedCheck, BST_CHECKED);
			else
				Button_SetCheck(hWindowedCheck, BST_UNCHECKED);


			hInputList = GetDlgItem(hwnd, IDC_INPUTLIST);
			SendMessage(hInputList, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);

			ZeroMemory(&col, sizeof(col));

			col.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
			col.cx = 80;
			col.pszText = "Input";
			col.iSubItem = 1;
			ListView_InsertColumn(hInputList, 0, &col);
			col.cx = 120;
			col.iSubItem = 2;
			col.pszText = "Valor";
			ListView_InsertColumn(hInputList, 1, &col);

			LVITEM item;
			ZeroMemory(&item, sizeof(item));

			item.mask = LVIF_TEXT;// | LVIF_PARAM;
			item.iSubItem = 0;
			for (int i=0; i < __INPUT_MAX__; i++) {
				item.pszText = InputTable[i].name;
				item.iItem = i;
				item.lParam = InputTable[i].index;
				ListView_InsertItem(hInputList, &item);
			}
			static char nameBuf[128];

			item.iSubItem = 1;
			for (int i=0; i < __INPUT_MAX__; i++) {
				item.pszText = (LPSTR) inputMgr.GetCodeName(InputTable[i].index, nameBuf);
				item.iItem = i;
				item.lParam = InputTable[i].index;
				ListView_SetItem(hInputList, &item);
			}


			return TRUE;
		}

	case WM_NOTIFY:
		{
			switch (((LPNMHDR)lParam)->code)
			{
			case NM_DBLCLK:
				{
					static char nameBuf[128];
					LPNMITEMACTIVATE lpItem = (LPNMITEMACTIVATE) lParam;

					if (lpItem) {
						int value = DialogBox(hInstance, MAKEINTRESOURCE(IDD_WAITFORINPUT), hMainDlg, WaitForInputDlgProc);

						if (value != -1) 
						{
							TTX_InputsDef idx = (TTX_InputsDef) lpItem->iItem;
							inputMgr.BindInput(idx, value);

							LVITEM item;
							item.mask = LVIF_TEXT;
							item.pszText = (LPSTR) inputMgr.GetCodeName(idx, nameBuf);
							item.iItem = lpItem->iItem;
							item.iSubItem = 1;
							ListView_SetItem(hInputList, &item);
						}

					}

					return TRUE;
					break;
				}
			}
			break;
		}
	case WM_COMMAND:

		switch (LOWORD(wParam))
		{
		case IDOK:
			inputMgr.Save();
			configMgr.Save();
			DestroyWindow(hwnd);
			break;
		case IDCANCEL:
			DestroyWindow(hwnd);
			break;

		case IDC_BUTTON_PLAY:
			{
				inputMgr.Save();
				configMgr.Save();
				system("ttx_loader");
				return TRUE;	
			}


		case IDC_CHECK_HIRES:
			{
				switch (HIWORD(wParam))
				{
				case BN_CLICKED:
					if (Button_GetCheck(GetDlgItem(hwnd, IDC_CHECK_HIRES)) == BST_CHECKED) {
						configMgr.SetConfig(TTX_CONFIG_HIRES, 1);
					} else {
						configMgr.SetConfig(TTX_CONFIG_HIRES, 0);
					}
					break;

				}
				return TRUE;
			}
		case IDC_CHECK_WINDOWED:
			{
				switch (HIWORD(wParam))
				{
				case BN_CLICKED:
					if (Button_GetCheck(GetDlgItem(hwnd, IDC_CHECK_WINDOWED)) == BST_CHECKED) {
						configMgr.SetConfig(TTX_CONFIG_WINDOWED, 1);
					} else {
						configMgr.SetConfig(TTX_CONFIG_WINDOWED, 0);
					}
					break;

				}
				return TRUE;
			}
		}


		return TRUE;
		break;


	}
	return FALSE;
}


int _tmain(int argc, _TCHAR* argv[])
{
	hInstance = GetModuleHandle(0);
	InitCommonControls();
	DialogBox(hInstance,
		MAKEINTRESOURCE(IDD_MAIN_DLG),
		GetConsoleWindow(),
		MainDlgProc);
	return 0;
}

