#include "Process64.h"

extern LPFN_NTQUERYINFORMATIONPROCESS        __NtQueryInformationProcess;

BOOL 
GetProcessEnvironmentVariableByPeb64(
	HANDLE hProcess
)
{
	BOOL   IsOk = TRUE;
	TCHAR* EnvpStrings = NULL;
	WCHAR* wstr = NULL;   
	PEB64  Peb;
	RTL_USER_PROCESS_PARAMETERS64 RtlUserProcessParameters;


	ZeroMemory(&Peb, sizeof(Peb));
	ZeroMemory(&RtlUserProcessParameters, sizeof(RtlUserProcessParameters));

	if (0 != GetPeb64(hProcess, &Peb)
		&& TRUE == ReadProcessMemoryEx64(
							hProcess,
							Peb.ProcessParameters, 
							&RtlUserProcessParameters, 
							sizeof(RtlUserProcessParameters), 
							0)
		){
		EnvpStrings = (TCHAR*)malloc(sizeof(TCHAR) * RtlUserProcessParameters.EnvironmentSize);
		if (EnvpStrings == NULL) {
			return FALSE;
		}

		if (!ReadProcessMemoryEx64(
						hProcess,
						RtlUserProcessParameters.Environment, 
						EnvpStrings,
						RtlUserProcessParameters.EnvironmentSize, 
						0)
			){
			goto Exit;
		}

		wstr = (WCHAR*)EnvpStrings;
		
		while (*wstr) {
			wprintf(L"%s\r\n", wstr);
			wstr += wcslen(wstr) + 1;
		}
	Exit:
		if (EnvpStrings != NULL) {
			free(EnvpStrings);
			EnvpStrings = NULL;
		}
	}
	return IsOk;
}

ULONG64 
GetPeb64(
	HANDLE hProcess, 
	PEB64* Peb
)
{
	ULONG ReturnLength = 0;
	PROCESS_BASIC_INFORMATION ProcessBasicInfo;
	
	ZeroMemory(&ProcessBasicInfo, sizeof(ProcessBasicInfo));
Loop:
	__try
	{
		if (__NtQueryInformationProcess) {
			if (NT_SUCCESS(__NtQueryInformationProcess(
				                        hProcess,
										ProcessBasicInformation, 
										&ProcessBasicInfo,
										(ULONG)sizeof(ProcessBasicInfo), 
										&ReturnLength)) 
				&& Peb) {
				if (ReadProcessMemory(
							hProcess,
							ProcessBasicInfo.PebBaseAddress, 
							Peb, 
							sizeof(PEB64), 
							NULL)
					) {
					return (reinterpret_cast<ULONG64>(ProcessBasicInfo.PebBaseAddress));
				}
			}
		}
		else {
			HMODULE hNtdllModuleBase = NULL;
			hNtdllModuleBase = GetModuleHandle(_T("Ntdll.dll"));
			if (hNtdllModuleBase == NULL) {
				return 0;
			}
			__NtQueryInformationProcess = 
				(LPFN_NTQUERYINFORMATIONPROCESS)GetProcAddress(
														hNtdllModuleBase,
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

BOOL 
ReadProcessMemoryEx64(
	HANDLE    hProcess, 
	DWORD64   BaseAddress, 
	LPVOID    BufferData, 
	SIZE_T    BufferSize, 
	DWORD64*  NumberOfBytesRead
)
{
	return ReadProcessMemory(
				hProcess, 
				reinterpret_cast<LPVOID>(BaseAddress), 
				BufferData,
				BufferSize,
				reinterpret_cast<SIZE_T*>(NumberOfBytesRead)
			);
}
