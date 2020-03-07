#include "ProcessHelper.h"


BOOL __EnableDebugPrivilege = TRUE;
LPFN_ISWOW64PROCESS				__IsWow64Process            = NULL;
LPFN_NTQUERYINFORMATIONPROCESS		__NtQueryInformationProcess = NULL;

BOOL
EnableSeDebugPrivilege(
	HANDLE _hProcess,
	BOOL   _IsEnable
)
{
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

HANDLE
OpenProcessEx(
	DWORD _DesiredAccess,
	BOOL  _IsInheritHandle,
	DWORD _ProcessId
)
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

BOOL
CloseHandleEx(
	HANDLE _hObject
)
{
	DWORD HandleFlags;
	if (GetHandleInformation(_hObject, &HandleFlags)
		&& (HandleFlags & HANDLE_FLAG_PROTECT_FROM_CLOSE) != HANDLE_FLAG_PROTECT_FROM_CLOSE)
		return (!!CloseHandle(_hObject));

	return FALSE;
}

BOOL IsWow64ProcessEx(
	HANDLE _hProcess, 
	BOOL*  _IsWow64Process
)
{
	if(__IsWow64Process == NULL) {
		HMODULE	hModuleBase = NULL;  
		__try
		{
			hModuleBase = GetModuleHandle(_T("kernel32.dll"));  
			if (hModuleBase == NULL) {
				_tprintf_s(_T("GetModuleHandle(kernel32.dll) Error (%d)"), GetLastError());
				return FALSE;
			}
			__IsWow64Process = 
				(LPFN_ISWOW64PROCESS)GetProcAddress(
											hModuleBase,
											"IsWow64Process"
										);  
			 
			if (__IsWow64Process == NULL) {
				_tprintf_s(_T("GetProcAddress() Error (%d)"), GetLastError());
				return FALSE;
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return FALSE;
		}
	}
	else {
		return __IsWow64Process(_hProcess, _IsWow64Process);
	}
	return TRUE;
}
