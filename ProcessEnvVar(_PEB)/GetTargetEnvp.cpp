
#include "ProcessHelper.h"
#include "Process.h"
#include "Process64.h"


VOID GetEvInTarget(DWORD ProcessID);

int _tmain(int _argc, TCHAR* _argv[], TCHAR* _envp[])
{
	setlocale(LC_ALL, "Chinese-simplified");

	DWORD ProcessId = NULL;

	_tprintf_s(_T("Enter Process ID:"));
	_tscanf_s(_T("%d"), &ProcessId);

	GetEvInTarget(ProcessId);

	_tprintf_s(_T("Input AnyKey To Exit\r\n"));
	_gettchar();

	return 0;
}

VOID GetEvInTarget(DWORD ProcessID)
{
	HANDLE hProcess  = NULL;
	BOOL   IsWow64_1 = FALSE;
	BOOL   IsWow64_2 = FALSE;
	// #define NtCurrentProcess()((HANDLE) -1) 
	IsWow64Process(GetCurrentProcess(), &IsWow64_1);

	if (ProcessID == GetCurrentProcessId()) {
		hProcess = GetCurrentProcess();
	}
	else {
		hProcess = OpenProcessEx(
							PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, 
							FALSE, 
							ProcessID
						);
		if (hProcess == NULL) {
			return;
		}
	}

	//!peb
	//dt _Peb 
	//dt _RTL_USER_PROCESS_PARAMETERS  
	//判断目标进程位数

	if (IsWow64ProcessEx(hProcess, &IsWow64_2) == FALSE){
		return;
	}
	if (IsWow64_1 == TRUE && IsWow64_2 == TRUE) {	
		// x86  x86
		GetProcessEnvironmentVariableByPeb<DWORD>(hProcess);
	}
	else if (IsWow64_1 == TRUE && IsWow64_2 == FALSE) {
		// x86  x64
		GetProcessEnvironmentVariableByPeb<DWORD64>(hProcess);
	}
	else if (IsWow64_1 == FALSE && IsWow64_2 == TRUE) {
		// x64  x86
		GetProcessEnvironmentVariableByPeb64(hProcess);
	}
	else {
		// x64  x64
		GetProcessEnvironmentVariableByPeb64(hProcess);
	}
	if (hProcess != NULL){
		CloseHandleEx(hProcess);
		hProcess = NULL;
	}

	return;
}
