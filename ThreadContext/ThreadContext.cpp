#include <Windows.h>
#include <tchar.h>
#include <iostream>

// unsigned long __stdcall ThreadProc(void* lpParam);
DWORD WINAPI  ThreadProc(LPVOID lpParam);
VOID CALLBACK Change();

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	setlocale(LC_ALL, "Chinese-simplified");

	HANDLE hThread = CreateThread(
				NULL,
				0,
				(LPTHREAD_START_ROUTINE)ThreadProc,
				NULL,
				0,
				NULL);

	Sleep(1000);
	SuspendThread(hThread);

	CONTEXT Context;
	ZeroMemory(&Context, sizeof(Context));

	Context.ContextFlags = CONTEXT_CONTROL | CONTEXT_FULL;

	GetThreadContext(hThread, &Context);
#ifdef _WIN64
	Context.Rip = (unsigned __int64)Change;
#else
	Context.Eip = (unsigned __int32)Change;
#endif

	SetThreadContext(hThread, &Context);
	ResumeThread(hThread);
	WaitForSingleObject(hThread, INFINITE);

	if (hThread != NULL)
		CloseHandle(hThread);
	
	_tprintf_s(_T("Input any key to exit\r\n"));
	_gettchar();

	return 0;
}

VOID CALLBACK Change()
{
	int i;
	for (i = 0; i < 1000; i++)
		_tprintf_s(_T("Change()\r\n"));
	
	return;
}

DWORD WINAPI ThreadProc(LPVOID lpParam)
{
	int i;
	for (i = 0; i < 1000; i++)
		_tprintf_s(_T("ThreadProc()\r\n"));

	return 0;
}
