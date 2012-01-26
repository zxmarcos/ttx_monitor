// ============================================================================
// ttx_monitor: Opensource TaitoTypeX loader
// by Romhack
// ============================================================================
// 
// loader.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <iostream>
#include <string>

using namespace std;

// ============================================================================
//
// ============================================================================
class LProcess {
	PROCESS_INFORMATION pi;
	BOOL is_loaded;
	STARTUPINFO si;
	void Error(TCHAR *msg);
	HANDLE dllHandle;
public:
	DWORD Resume();
	LProcess(const TCHAR* name);
	~LProcess();
	DWORD Write(void *buf, void *dst, int len);
	DWORD Read(void *buf, void *src, int len);
	void Patch(DWORD addr, BYTE data)
	{
		if (is_loaded)
			Write(&data, (void *) (addr + 0x400000), 1);
	}
	BOOL InjectDLL(const TCHAR *name);
};

// ============================================================================
//
// ============================================================================
void LProcess::Error(TCHAR *msg)
{
	MessageBox(0, msg, _TEXT("Error"), MB_OK);
	exit(1);
}

// ============================================================================
//
// ============================================================================
LProcess::LProcess(const TCHAR *name)
: is_loaded(FALSE)
{

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);

	_tprintf(_T("Carregando o processo %s...\n"), name);

	is_loaded = CreateProcess(name,
		NULL,
		NULL,
		NULL,
		FALSE,
		CREATE_SUSPENDED,
		NULL,
		NULL,
		&si,
		&pi);
	if (!is_loaded)
		Error(_TEXT("Processo não carregado!"));

	is_loaded = TRUE;
}

// ============================================================================
//
// ============================================================================
DWORD LProcess::Write(void *buf, void *dst, int len)
{
	DWORD ret = 0;
	DWORD oldProt, newProt;
//	VirtualProtectEx(pi.hProcess, dst, len, PAGE_EXECUTE_READWRITE, &oldProt);
	WriteProcessMemory(pi.hProcess, dst, buf, len, &ret);
//	VirtualProtectEx(pi.hProcess, dst, len, oldProt, &newProt);
	return ret;
}

// ============================================================================
//
// ============================================================================
DWORD LProcess::Read(void *buf, void *src, int len)
{
	DWORD ret = 0;
	DWORD oldProt, newProt;
//	VirtualProtectEx(pi.hProcess, src, len, PAGE_EXECUTE_READWRITE, &oldProt);
	ReadProcessMemory(pi.hProcess, src, buf, len, &ret);
//	VirtualProtectEx(pi.hProcess, src, len, oldProt, &newProt);
	return ret;
}

// ============================================================================
//
// ============================================================================
DWORD LProcess::Resume()
{
	return ResumeThread(pi.hThread);
}

// ============================================================================
//
//    Depois de muito estudar como fazer isso, encontrando altas gambiarras
// complicadas na internet, acabei por "pegar" o conceito, que na verdade é
// muito simples. O que precisamos fazer é carregar o processo em modo suspenso,
// então, usamos VirtualAllocEx pra alocar memória virtual para um string, que 
// nesse caso é o nome da DLL a ser carregada, lembrando que é preciso utilizar
// a flag PAGE_EXECUTE_READWRITE. Depois de alocarmos a memória, escrevemos a
// string utilizando WriteProcessMemory.
//    Agora criaremos uma thread remota que executará a função LoadLibrary no
// endereçamento do processo com o parametro apontando para o endereço da string
// que escrevemos e assim teremos carregado a DLL.
//
// ============================================================================
BOOL LProcess::InjectDLL(const TCHAR *name)
{
	LPVOID _lproc;
	LPVOID _pbase;
	DWORD threadid;
	TCHAR errbuf[256];



	

		_lproc = (LPVOID) GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "LoadLibraryA");
	if (!_lproc) {
		_stprintf(errbuf, _TEXT("Error %X\n"), GetLastError());
		Error(errbuf);
	}


	_pbase = VirtualAllocEx(pi.hProcess,
							NULL,
							_tcslen(name),
							MEM_RESERVE | MEM_COMMIT,
							PAGE_EXECUTE_READWRITE);
	if (!_pbase) {
		
		_stprintf(errbuf, _TEXT("Error %X\n"), GetLastError());
		Error(errbuf);
	}

	_tprintf(_T("VmAddress = 0x%08X\n"), _pbase);
	
	//ZeroMemory(_pbase, _tcslen(name));
	Write((void *)name, _pbase, _tcslen(name));

	dllHandle = CreateRemoteThread(pi.hProcess,
								   NULL,
								   NULL,
								   (LPTHREAD_START_ROUTINE) _lproc,
								   _pbase,
								   0,
								   &threadid);
	if (!dllHandle) {
		_stprintf(errbuf, _TEXT("Error %X\n"), GetLastError());
		Error(errbuf);
	}
	WaitForSingleObject(dllHandle, INFINITE);
	_tprintf(_T("DLL %s attached\n"), name);
	
	return TRUE;
}


int _tmain(int argc, _TCHAR* argv[])
{
	if (argc < 2) {
		LProcess *proc = new LProcess(_TEXT("game.exe"));
		proc->InjectDLL(_T("ttx_monitor.dll"));
		proc->Resume();
	} else {
		LProcess *proc = new LProcess(argv[1]);
		proc->InjectDLL(_T("ttx_monitor.dll"));
		proc->Resume();
	}
	return 0;
}

