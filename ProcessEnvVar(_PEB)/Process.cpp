#include "Process.h"
#include "ProcessHelper.h"

extern LPFN_NTQUERYINFORMATIONPROCESS __NtQueryInformationProcess;


LPFN_NTWOW64READVIRTUALMEMORY64       __NtWow64ReadVirtualMemory64       = NULL;
LPFN_NTWOW64QUERYINFORMATIONPROCESS64 __NtWow64QueryInformationProcess64 = NULL;


ULONG32 GetPeb(
	HANDLE hProcess, 
	PEB32* Peb
)
{
	PROCESS_BASIC_INFORMATION ProcessBasicInfo;
	ZeroMemory(&ProcessBasicInfo, sizeof(ProcessBasicInfo));

	ULONG ReturnLength = 0;
Loop:
	__try
	{
		if (__NtQueryInformationProcess)
		{
			if (NT_SUCCESS(__NtQueryInformationProcess(
									hProcess, 
									ProcessBasicInformation,
									&ProcessBasicInfo,
									(ULONG)sizeof(ProcessBasicInfo), 
									&ReturnLength)) 
				&& Peb) {
				if (TRUE == ReadProcessMemory(
									hProcess, 
									ProcessBasicInfo.PebBaseAddress,
									Peb, 
									sizeof(PEB32), 
									NULL) 
					) {
					return reinterpret_cast<ULONG32>(ProcessBasicInfo.PebBaseAddress);
				}
			}
		}
		else
		{
			HMODULE hModuleBase = NULL;
			hModuleBase = GetModuleHandle(_T("Ntdll.dll"));
			if (hModuleBase == NULL)
			{
				return 0;
			}
			__NtQueryInformationProcess = 
				(LPFN_NTQUERYINFORMATIONPROCESS)GetProcAddress(
								hModuleBase,
								"NtQueryInformationProcess"
							);
			if (__NtQueryInformationProcess == NULL) {
				return 0;
			}
			goto Loop;
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return 0;
	}

	return 0;
}

ULONG64 GetPeb(
	HANDLE hProcess, 
	PEB64* Peb
)
{
	_PROCESS_BASIC_INFORMATION_T<DWORD64> ProcessBasicInfo = { 0 };
	ULONG ReturnLength = 0;

Loop:
	__try
	{
		if (__NtWow64QueryInformationProcess64&&__NtWow64ReadVirtualMemory64) {
			if (NT_SUCCESS(__NtWow64QueryInformationProcess64(
									hProcess, 
									ProcessBasicInformation, 
									&ProcessBasicInfo,
									(ULONG)sizeof(ProcessBasicInfo), 
									&ReturnLength)) 
				&& Peb) {
				if (NT_SUCCESS(__NtWow64ReadVirtualMemory64(
									hProcess, 
									ProcessBasicInfo.PebBaseAddress,
									Peb, 
									sizeof(PEB64), 
									NULL))
					) {
					return ProcessBasicInfo.PebBaseAddress;
				}
				else {
					return NULL;
				}
			}
		}
		else {
			HMODULE hNtdllModuleBase = NULL;
			hNtdllModuleBase = GetModuleHandle(_T("Ntdll.dll"));
			if (hNtdllModuleBase == NULL)
			{
				return 0;
			}
			__NtWow64QueryInformationProcess64 = 
				(LPFN_NTWOW64QUERYINFORMATIONPROCESS64)GetProcAddress(
									hNtdllModuleBase,
									"NtWow64QueryInformationProcess64"
								);
			__NtWow64ReadVirtualMemory64 = 
				(LPFN_NTWOW64READVIRTUALMEMORY64)GetProcAddress(
									hNtdllModuleBase,
									"NtWow64ReadVirtualMemory64"
								);

			if (__NtWow64QueryInformationProcess64 == NULL 
				 ||   __NtWow64ReadVirtualMemory64 == NULL) {
				return 0;
			}
			goto Loop;
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return 0;
	}
	return 0;
}

BOOL 
ReadProcessMemoryEx(
	HANDLE   hProcess, 
	DWORD    BaseAddress, 
	LPVOID   BufferData, 
	SIZE_T   BufferSize, 
	DWORD64* ReturnLength
)
{
	return ReadProcessMemory(
				hProcess, 
				reinterpret_cast<LPVOID>(BaseAddress), 
				BufferData,
				BufferSize, 
				reinterpret_cast<SIZE_T*>(ReturnLength)
			);
}

BOOL 
ReadProcessMemoryEx(
	HANDLE   hProcess, 
	DWORD64  BaseAddress,
	LPVOID   BufferData, 
	SIZE_T   BufferSize, 
	DWORD64* ReturnLength
)
{
Loop:
	__try
	{
		if (__NtWow64ReadVirtualMemory64)
		{
			if ( STATUS_SUCCESS != __NtWow64ReadVirtualMemory64(
							hProcess,
							BaseAddress, 
							BufferData, 
							BufferSize, 
							ReturnLength)
				) {
				return FALSE;
			}
		}
		else {
			HMODULE hNtdllModuleBase = NULL;
			hNtdllModuleBase = GetModuleHandle(_T("Ntdll.dll"));
			if (hNtdllModuleBase == NULL)
			{
				return FALSE;
			}
			__NtWow64ReadVirtualMemory64 = 
				(LPFN_NTWOW64READVIRTUALMEMORY64)GetProcAddress(
										hNtdllModuleBase,
										"NtWow64ReadVirtualMemory64"
									);
			if (__NtWow64ReadVirtualMemory64 == NULL) {
				return FALSE;
			}
			goto Loop;
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return FALSE;
	}

	return TRUE;
}
