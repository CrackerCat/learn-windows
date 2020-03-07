#include "ProcessA.h"


int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	setlocale(LC_ALL, "Chiness");

	FileMappingShare();

	_tprintf_s(_T("Input AnyKey To Exit\r\n"));
	_gettchar();
	_gettchar();
	return 0;
}

BOOL FileMappingShare()
{

	HANDLE		hMappingHandle = NULL;
	LPVOID   	BaseAddress    = NULL;
	DWORD       LastError = 0;

	if (!CreateMemroyMappingEx(ACCESS_WRITE, 0, 0x1000, _T("Iris"), 
            &hMappingHandle, &BaseAddress)) {
		LastError = GetLastError();
	}
	else {
		__try
		{
			memcpy(
				BaseAddress, 
				_T("Blood for blood"),
				sizeof(TCHAR)*(_tcslen(_T("Blood for blood")) + 1)
			);
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			LastError = GetExceptionCode();
		}
	}

	_tprintf(_T("Input AnyKey To Continue\r\n"));
	_gettchar();

	if(BaseAddress != NULL) {
        
		_tprintf_s(_T("%s\r\n"), (TCHAR*)BaseAddress);
	}
	UnmapMemoryEx(hMappingHandle, BaseAddress);
	return 0;
}

BOOL 
CreateMemroyMappingEx(
	_In_  DWORD ReadOrWrite,
	_In_  DWORD MaximumSizeHigh,
	_In_  DWORD MaximumSizeLow,
	_In_  LPCTSTR   ObjectName,
	_Out_ LPHANDLE  hMappingHandle,
	_Out_ LPVOID*   BaseAddress)
{
	DWORD  DesiredAccess = 0;
	DWORD  Protect = 0;
	HANDLE hFileMapping = NULL;
	LPVOID lpBaseAddress  = NULL;
	int    LastError = 0;

	if (hMappingHandle == NULL || BaseAddress == NULL) {
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	if (ReadOrWrite == ACCESS_READ) {
		Protect = PAGE_READONLY;
		DesiredAccess = SECTION_MAP_READ;

	}

	else if (ReadOrWrite == ACCESS_WRITE) {

		Protect = PAGE_READWRITE;
		DesiredAccess = SECTION_MAP_READ | SECTION_MAP_WRITE;
	}

	else {
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	__try
	{
		hFileMapping = CreateFileMapping(
					INVALID_HANDLE_VALUE, 
					NULL, 
					Protect, 
					MaximumSizeHigh, 
					MaximumSizeLow, 
					ObjectName
				);

		if (hFileMapping != NULL) {

			*hMappingHandle = hFileMapping;

			lpBaseAddress = MapViewOfFile(hFileMapping, DesiredAccess, 0, 0, 0);

			if (BaseAddress != NULL) {
                
				(*BaseAddress) = lpBaseAddress;
				return TRUE;
			}
		}

	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		SetLastError(GetExceptionCode());
	}

	return FALSE;
}

BOOL UnmapMemoryEx(_In_ HANDLE hMappingHandle, _In_ LPCVOID BaseAddress)
{
	__try
	{
		if (UnmapViewOfFile((LPCVOID)BaseAddress)) {
            
			CloseHandleEx(hMappingHandle);
			return TRUE;
		}

	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		SetLastError(GetExceptionCode());
	}
	return FALSE;
}

BOOL CloseHandleEx(HANDLE _hObject)
{
	DWORD HandleFlags;
	if (GetHandleInformation(_hObject, &HandleFlags)
		&& (HandleFlags & HANDLE_FLAG_PROTECT_FROM_CLOSE) != HANDLE_FLAG_PROTECT_FROM_CLOSE)
		return !!CloseHandle(_hObject);
	return FALSE;
}
