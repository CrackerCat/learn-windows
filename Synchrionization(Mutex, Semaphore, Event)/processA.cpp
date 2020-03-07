#include "ProcessA.h"

int _tmain(int _argc, _TCHAR* _argv[], _TCHAR* _envp[])
{
	setlocale(LC_ALL, "Chinese-simplified");
	// SynNamedEvent();
	// SynNamedMutex();
	// SynNamedSemaphore();
	SynDupSemaphore();
	_tprintf_s(_T("Input AnyKey To Exit\r\n"));
	_gettchar();
	_gettchar();
	
	return 0;
}

BOOL SynNamedEvent()
{
	HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, _T("Iris"));

	if (hEvent == NULL)
		return FALSE;

	WaitForSingleObject(hEvent, INFINITE);

	_tprintf_s(_T("Iris bloom\r\n"));


	CloseHandleEx(hEvent);
	hEvent = NULL;

	return TRUE;
}

BOOL SynNamedMutex()
{
	HANDLE hMutex = CreateMutex(NULL, TRUE, _T("Iris"));     
	if (hMutex == NULL)
		return FALSE;

	WaitForSingleObject(hMutex, INFINITE);
	TCHAR oath[0x1000];
	ZeroMemory(&oath, sizeof(oath));

	_tprintf(_T("Where is your glory, knight ?\r\n"));
	_tscanf(_T("%s"), oath);

	if (_tcscmp(oath,_T("Avalon")) == 0)
	{
		ReleaseMutex(hMutex);
		ReleaseMutex(hMutex);
	}

	CloseHandleEx(hMutex);
	hMutex = NULL;

	return TRUE;

}

BOOL SynNamedSemaphore()
{
	HANDLE hSemaphore = CreateSemaphore(NULL, 0, 2, _T("Iris"));
	if (hSemaphore == NULL)
		return FALSE;


	TCHAR oath[0x1000];
	ZeroMemory(&oath, sizeof(oath));

	_tprintf_s(_T("Where is your glory, knight ?\r\n"));
	_tscanf(_T("%s"), oath);

	if (_tcscmp(oath, _T("Avalon")) == 0)
	{
		ReleaseSemaphore(hSemaphore, 2, NULL);
		Sleep(10);
	}
	CloseHandle(hSemaphore);
	hSemaphore = NULL;

	return TRUE;
}

BOOL SynDupSemaphore()
{

	HANDLE hMutex = CreateMutex(NULL, TRUE, NULL);
	if (hMutex == NULL)
		return FALSE;

	_tprintf_s(_T("Process ID  : %d\r\n"), GetCurrentProcessId());
	_tprintf_s(_T("Mutex Handle: %d\r\n"), hMutex);



	_tprintf_s(_T("Running Process B\r\n"));

	TCHAR oath[0x1000] { 0 };
	_tprintf_s(_T("oath:"));
	_tscanf(_T("%s"), oath);

	if (_tcscmp(oath, _T("Avalon")) == 0)
		ReleaseMutex(hMutex);
	
	Sleep(1000);	

	CloseHandleEx(hMutex);
	hMutex = NULL;

	return TRUE;
}


BOOL CloseHandleEx(HANDLE _hObject)
{
	DWORD HandleFlags;
	if (GetHandleInformation(_hObject, &HandleFlags)
		&& (HandleFlags & HANDLE_FLAG_PROTECT_FROM_CLOSE) != HANDLE_FLAG_PROTECT_FROM_CLOSE)
		return !!CloseHandle(_hObject);
	return FALSE;
}
