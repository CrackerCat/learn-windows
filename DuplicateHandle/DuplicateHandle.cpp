#include "DuplicateHandle.h"

BOOL __EnableDebugPrivilege = TRUE;

int _tmain(int argc, _TCHAR* argv[], _TCHAR *envp[])
{
	DupFileFromDelete();

	return 0;
}

void DupFileFromDelete()
{

	HANDLE  hProcess;
	HANDLE  hTarget;
	HANDLE  hFile;
	BOOL    IsOk;
	hProcess = OpenProcessEx(PROCESS_DUP_HANDLE, FALSE, 10724);
	if (hProcess == NULL){
		return;
	}
	
	hFile = CreateFile(
					_T("ReadMe.txt"), 
					GENERIC_READ, 
					0, 
					NULL, 
					CREATE_ALWAYS, 
					FILE_ATTRIBUTE_NORMAL, 
					NULL
				);

	if (hFile == INVALID_HANDLE_VALUE) {
		CloseHandleEx(hProcess);
		hProcess = NULL;
		return;
	}

	IsOk = DuplicateHandle(
						GetCurrentProcess(), 
						hFile, 
						hProcess, 
						&hTarget, 
						0,
						FALSE, 
						DUPLICATE_SAME_ACCESS
					);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandleEx(_hFile);
		hFile = INVALID_HANDLE_VALUE;
	}
	if (hProcess != NULL)
	{
		CloseHandleEx(hProcess);
		hProcess = NULL;
	}

	return;
}

BOOL EnableSeDebugPrivilege(HANDLE hProcess, BOOL IsEnable)
{
	DWORD  LastError;
	HANDLE hToken;
	LUID   Luid;
	TOKEN_PRIVILEGES TokenPrivileges;
	ZeroMemory(&TokenPrivileges, sizeof(TokenPrivileges));
	if (!OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
		LastError = GetLastError();
		if (hToken)
			CloseHandleEx(hToken);
		SetLastError(LastError);
		return FALSE;
	}

	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &Luid))
	{
		LastError = GetLastError();
		CloseHandleEx(hToken);
		SetLastError(LastError);
		return FALSE;
	}
	TokenPrivileges.PrivilegeCount = 1;
	TokenPrivileges.Privileges[0].Luid = Luid;
	if (IsEnable)
		TokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		TokenPrivileges.Privileges[0].Attributes = 0;
	if (!AdjustTokenPrivileges(hToken, FALSE, &TokenPrivileges, sizeof(TOKEN_PRIVILEGES), NULL, NULL))
	{
		LastError = GetLastError();
		CloseHandleEx(hToken);
		SetLastError(LastError);
		return FALSE;
	}

	CloseHandleEx(hToken);
	return TRUE;
}



HANDLE OpenProcessEx(DWORD DesiredAccess, BOOL IsInheritHandle, DWORD ProcessId)
{
	HANDLE hProcess;
	DWORD  LastError;
	if (__EnableDebugPrivilege) {
		EnableSeDebugPrivilege(GetCurrentProcess(), TRUE);
	}
	hProcess = OpenProcess(DesiredAccess, IsInheritHandle, ProcessId);
	LastError = GetLastError();

	if (__EnableDebugPrivilege) {
		EnableSeDebugPrivilege(GetCurrentProcess(), FALSE);
	}

	SetLastError(LastError);
	return hProcess;
}

BOOL CloseHandleEx(HANDLE hObject)
{
	DWORD Flags;
	if (GetHandleInformation(hObject, &Flags)
		&& (Flags & HANDLE_FLAG_PROTECT_FROM_CLOSE) != HANDLE_FLAG_PROTECT_FROM_CLOSE)
		return (!!CloseHandle(hObject));

	return FALSE;
}
