// ============================================================================
// ttx_monitor: Opensource TaitoTypeX loader
// by Romhack
// ============================================================================
#include "stdafx.h"
#include <windows.h>


static HANDLE hLogMutex = 0;

FILE *__fp = NULL;

void __stdcall loginit()
{
	FILE *fp;
	fp = fopen("monitor_log.txt", "w");
	if (!fp)
		return;
	hLogMutex = CreateMutex(0, 0, NULL);
	fprintf(fp, "===============================================\n");
	fprintf(fp, "              Taito TypeX Loader\n");
	fprintf(fp, "                      by Romhack\n");
	fprintf(fp, "===============================================\n");
	fflush(fp);
	__fp = fp;
	//fclose(fp);
}


void __stdcall logmsg(const char *fmt, ...)
{
	if (__fp) {
#if 1
#if 1
		WaitForSingleObject(hLogMutex, INFINITE);
#ifdef ___TTX_CONFIG
		FILE *fp = stdout;
#else
		FILE *fp = __fp;
#endif
#endif

		va_list va;
		va_start(va, fmt);
		vfprintf(fp, fmt, va);
		fflush(fp);
		va_end(va);
		
		ReleaseMutex(hLogMutex);
#endif
	}
}

void __stdcall logmsgw(const WCHAR *fmt, ...)
{
	if (__fp) {
#if 1
#if 1
		WaitForSingleObject(hLogMutex, INFINITE);
#ifdef ___TTX_CONFIG
		FILE *fp = stdout;
#else
		FILE *fp = __fp;
#endif
#endif

		va_list va;
		va_start(va, fmt);
		vfwprintf(fp, fmt, va);
		fflush(fp);
		va_end(va);
		
		ReleaseMutex(hLogMutex);
#endif
	}
}
