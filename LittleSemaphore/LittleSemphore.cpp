#include "LittleSemaphore.h"


HANDLE  g_hSemaphore;

int _tmain(int argc, TCHAR const *argv[], TCHAR const *envp[])
{
	g_hSemaphore = NULL;
	// sub_1();
	// sub_2();
	sub_4();

	return 0;
}

void sub_1()
{
	HANDLE  hSemaphore = NULL;
	HANDLE  hThread = NULL;
	hSemaphore = CreateSemaphore(NULL, 2, 4, NULL);
	// KeInitializeSemaphore
	WaitForSingleObject(hSemaphore, INFINITE);

	ReleaseSemaphore(hSemaphore, 1, NULL);
	WaitForSingleObject(hSemaphore, INFINITE);
	WaitForSingleObject(hSemaphore, INFINITE);
	CloseHandle(hSemaphore);
}



void sub_2()
{
	g_hSemaphore = CreateSemaphore(NULL, 2, 2, NULL);
	int recrCnt = 0;
	sub_3(recrCnt);
	_tprintf_s(_T("Input any key to exit\r\n"));
	_gettchar();
	if (g_hSemaphore != NULL) {
		CloseHandle(g_hSemaphore);
		g_hSemaphore = NULL;
	}
}


void sub_3(int recrCnt)
{
	if (recrCnt == 2) {
		ReleaseSemaphore(g_hSemaphore, 2, NULL);
		return;
	}
	else {
		WaitForSingleObject(g_hSemaphore, INFINITE);
		_tprintf_s(_T("sub_3() Recursion Count: %d\r\n"), ++recrCnt);
		sub_3(recrCnt);
	}
}


void sub_4()
{
	HANDLE hThreads[2] { 0 };
	

	HANDLE hSemaphore = CreateSemaphore(NULL, 3, 3, NULL);

	try {
		if (hSemaphore == NULL) {
			_tprintf_s(_T("Err: CreateSemaphore() ( %d )\r\n"), GetLastError());
			throw;
		}

		for (int i = 0; i < _countof(hThreads); i++) {
			hThreads[i] = CreateThread(
				NULL, 0,
				(LPTHREAD_START_ROUTINE)ThreadProc,
				(PVOID)&hSemaphore,
				0, NULL);

			if (hThreads[i] == NULL) {
				_tprintf_s(_T("Err: CreateThread() ( %d )\r\n"), GetLastError());
				throw;
			}
		}

		WaitForMultipleObjects(_countof(hThreads), hThreads, TRUE, INFINITE);
	} catch (...) { }

	for (int i = 0; i < _countof(hThreads); i++) {
		if (hThreads[i] != NULL) {
			CloseHandle(hThreads[i]);
			hThreads[i] = NULL;
		}
	}
	if (hSemaphore != NULL) {
		CloseHandle(hSemaphore);
		hSemaphore = NULL;
	}

	return;
}




DWORD WINAPI ThreadProc(LPVOID lpvParam)
{
	HANDLE hSemaphore = *((HANDLE*)lpvParam);
	BOOL fOk = 0;
	while (TRUE) {
		fOk = WaitForSingleObject(hSemaphore, INFINITE);
		switch (fOk)
		{
		case WAIT_OBJECT_0:
			_tprintf_s(_T("current ThreadId: %d\r\n"), 
				GetCurrentThreadId());
			break;
		case WAIT_ABANDONED:
			return -1;
		}
	}
	return 0;
}
