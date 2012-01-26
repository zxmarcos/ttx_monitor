// ============================================================================
// ttx_monitor: Opensource TaitoTypeX loader
// by Romhack
// ============================================================================
#include <iostream>
#include <string>
#include <list>
#include <cstdio>
#include <cctype>
#include "Shared.h"

using namespace std;

BOOL bUniversalConfig = FALSE;

TTX_ConfigManager::TTX_ConfigManager()
{
	for (int i=0; i < __CONFIG_MAX__; i++)
		optTable[i] = 0;
}

TTX_ConfigManager::~TTX_ConfigManager()
{
}

DWORD TTX_ConfigManager::GetConfig(TTX_ConfigDef index)
{
	return optTable[index];
}

void TTX_ConfigManager::Config()
{
	string s;
	printf("Usar video em alta resolução (y/n): ");
	getline(cin, s);
	if (tolower(s[0]) == 'y')
		optTable[TTX_CONFIG_HIRES] = 1;
	else
		optTable[TTX_CONFIG_HIRES] = 0;

}

void TTX_ConfigManager::SetConfig(TTX_ConfigDef index, DWORD val)
{
	optTable[index] = val;
}

void TTX_ConfigManager::Load()
{

	FILE *fp = fopen(MONITOR_MISC_FILENAME, "r");
	if (fp) {
		fread(optTable, 4, __CONFIG_MAX__, fp);
		fclose(fp);
	}

#if 0
	DWORD dwDisp = 0;
	HKEY hKey = 0;
	DWORD dwType = 0;
	DWORD dwSize = 4 * __CONFIG_MAX__;
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
		return;
	}
	RegQueryValueEx(hKey, MONITOR_MISC_FILENAME, NULL, &dwType, (BYTE *) optTable, &dwSize);
	RegCloseKey(hKey);
}
#endif

}
void TTX_ConfigManager::Save()
{
	FILE *fp = fopen(MONITOR_MISC_FILENAME, "w");
	if (fp) {
		fwrite(optTable, 4, __CONFIG_MAX__, fp);
		fclose(fp);
	}
#if 0

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

		return;
	}
	RegSetValueEx(hKey, MONITOR_MISC_FILENAME, 0, REG_BINARY, (BYTE*) optTable, 4*__CONFIG_MAX__);
	RegCloseKey(hKey);
}
#endif
}

