#include "Process(B).h"

BOOL __EnableDebugPrivilege = TRUE;
void _tmain(int argc, TCHAR* argv[], TCHAR *envp[])
{
	setlocale(LC_ALL,"Chinese-simplified");
	// SynNamedEvent();
	// SynNamedMutex();
	// SynNamedSemaphore();
	SynDupSemaphore();
	_tprintf_s(_T("Input AnyKey To Exit\r\n"));
	_gettchar();
	_gettchar();
}


BOOL SynNamedEvent()
{
	HANDLE hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE,_T("Iris"));

	if (hEvent == NULL)
		return FALSE;


	for (int i = 0; i < 3; i++) 
		_tprintf_s(_T("Blood for blood\r\n"));
	
	SetEvent(hEvent);

	if (hEvent != NULL)
	{
		CloseHandleEx(hEvent);
	}

	return TRUE;
}

BOOL SynNamedMutex()
{
	HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, _T("Iris"));

	DWORD IsOk = WaitForSingleObject(hMutex,  INFINITE);

	switch (IsOk)
	{
	case WAIT_FAILED:
		return FALSE;
		break;
	case WAIT_OBJECT_0:
		__try 
		{
			_tprintf_s(_T("Blood for blood\r\n"));
		}
		__finally 
		{
			// Release ownership of the mutex object.
			if (!ReleaseMutex(hMutex))
				return FALSE;
			break;
		}
		// Cannot get mutex ownership due to time-out.
	case WAIT_TIMEOUT:
		return FALSE;
		break;
		// Got ownership of the abandoned mutex object.
	case WAIT_ABANDONED:
		return FALSE;
		break;
	default:
		return FALSE;
		break;
	}

	if (hMutex != NULL)
		CloseHandleEx(hMutex);
	
	return TRUE;
}

BOOL SynNamedSemaphore()
{
	HANDLE hSemaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, _T("Iris"));
	if (hSemaphore == NULL)
		return FALSE;

	//等待一个授信的信号灯中的信号
	WaitForSingleObject(hSemaphore,INFINITE);
	WaitForSingleObject(hSemaphore,INFINITE);

	_tprintf_s(_T("Blood for blood\r\n"));

	if (hSemaphore != NULL)
		CloseHandleEx(hSemaphore);
	
	return TRUE;
}


BOOL SynDupSemaphore()
{
	DWORD ProcessId = 0;
	HANDLE hSrcMutex = 0;
	HANDLE hMutex = NULL;
	_tprintf(_T("Input Process Identity:\r\n"));
	_tscanf(_T("%d"),&ProcessId);


	HANDLE hProcess =  OpenProcessEx(PROCESS_DUP_HANDLE, FALSE, ProcessId);

	if (hProcess == NULL)
		return FALSE;

	_tprintf(_T("Input Handle Value:\r\n"));
	_tscanf(_T("%d"), &hSrcMutex);
	BOOL IsOk = DuplicateHandle(
						hProcess, 
						hSrcMutex, 
						GetCurrentProcess(), 
						&hMutex,
						0,
						FALSE, 
						DUPLICATE_SAME_ACCESS
					);

	if (hMutex == NULL)
	{
		CloseHandleEx(hProcess);
		hProcess = NULL;
		return FALSE;
	}

	IsOk = WaitForSingleObject(hMutex, INFINITE);
	switch (IsOk)
	{
	case WAIT_OBJECT_0:
	{
		_tprintf_s(_T("Blood for blood\r\n"));
		break;
	}
	case WAIT_ABANDONED:
	{
		_tprintf_s(_T("Iris withered\r\n"));
		break;
	}
	}

	if (hProcess != NULL)
	{
		CloseHandleEx(hProcess);
		hProcess = NULL;
	}
	if (hMutex != NULL)
	{
		CloseHandleEx(hMutex);
		hMutex = NULL;
	}

	return TRUE;
}

BOOL EnableSeDebugPrivilege(HANDLE _hProcess, BOOL _IsEnable) {
	DWORD  LastError;
	HANDLE hToken;
	LUID   Luid;
	TOKEN_PRIVILEGES _TokenPrivileges;
	ZeroMemory(&_TokenPrivileges, sizeof(_TokenPrivileges));
	if (!OpenProcessToken(_hProcess, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
		LastError = GetLastError();
		if (hToken)
			CloseHandleEx(hToken);
		SetLastError(LastError);
		return FALSE;
	}

	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &Luid)) {
		LastError = GetLastError();
		CloseHandleEx(hToken);
		SetLastError(LastError);
		return FALSE;
	}

	_TokenPrivileges.PrivilegeCount = 1;
	_TokenPrivileges.Privileges[0].Luid = Luid;
	if (_IsEnable)
		_TokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		_TokenPrivileges.Privileges[0].Attributes = 0;
	if (!AdjustTokenPrivileges(hToken, FALSE, &_TokenPrivileges, sizeof(TOKEN_PRIVILEGES), NULL, NULL)) {
		LastError = GetLastError();
		CloseHandleEx(hToken);
		SetLastError(LastError);
		return FALSE;
	}

	CloseHandleEx(hToken);
	return TRUE;
}

HANDLE OpenProcessEx(DWORD _DesiredAccess, BOOL _IsInheritHandle, DWORD _ProcessId)
{
	HANDLE hProcess;
	DWORD  LastError;
	if (__EnableDebugPrivilege) {
		EnableSeDebugPrivilege(GetCurrentProcess(), TRUE);
	}
	hProcess = OpenProcess(_DesiredAccess, _IsInheritHandle, _ProcessId);
	LastError = GetLastError();

	if (__EnableDebugPrivilege) {
		EnableSeDebugPrivilege(GetCurrentProcess(), FALSE);
	}

	SetLastError(LastError);
	return hProcess;
}

BOOL CloseHandleEx(HANDLE _hObject)
{
	DWORD HandleFlags;
	if (GetHandleInformation(_hObject, &HandleFlags)
		&& (HandleFlags & HANDLE_FLAG_PROTECT_FROM_CLOSE) != HANDLE_FLAG_PROTECT_FROM_CLOSE)
		return !!CloseHandle(_hObject);
	return FALSE;
}


