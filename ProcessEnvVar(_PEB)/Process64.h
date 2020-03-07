#include "ProcessHelper.h"

BOOL 
GetProcessEnvironmentVariableByPeb64(
	_In_ HANDLE hProcess
);

ULONG64 GetPeb64(
	_In_  HANDLE hProcess, 
	_Out_ PEB64* Peb
);

BOOL 
ReadProcessMemoryEx64(
	_In_  HANDLE   hProcess, 
	_In_  DWORD64  BaseAddress, 
	_Out_ LPVOID   BufferData, 
	_In_  SIZE_T   BufferSize, 
	_Out_ DWORD64* NumberOfBytesRead
);
