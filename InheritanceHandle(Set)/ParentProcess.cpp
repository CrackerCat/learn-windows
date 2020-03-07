#include"Process.h"


int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	setlocale(LC_ALL, "Chinese");

	InheritOneBetweenBoth();

	_tprintf_s(_T("input any key to exit\r\n"));
	_gettchar();

	return 0;
}


void InheritOneBetweenBoth()
{
	HANDLE hRealProcess   = NULL;
	HANDLE hPseudoProcess = GetCurrentProcess();
	TCHAR  BufferData[20] { 0 };
	ZeroMemory(BufferData, sizeof(BufferData));

	DuplicateHandle(
			GetCurrentProcess(), 
			hPseudoProcess, 
			GetCurrentProcess(), 
			&hRealProcess,
			0, 
			FALSE, 
			DUPLICATE_SAME_ACCESS
		);

	_tprintf_s(_T("Parent Psedo Handle value : %d\r\n"), hPseudoProcess);
	_tprintf_s(_T("Parent Real  Handle value : %d\r\n"), hRealProcess);

	_stprintf_s(BufferData, _T("%d %d"), hPseudoProcess, hRealProcess);

	SetHandleInformation(hRealProcess, HANDLE_FLAG_INHERIT, NULL);
	
	STARTUPINFO StartupInfo;
	ZeroMemory(&StartupInfo, sizeof(StartupInfo));
	StartupInfo.cb = sizeof(StartupInfo);

	PROCESS_INFORMATION ProcessInfo;
	ZeroMemory(&ProcessInfo, sizeof(ProcessInfo));

	BOOL IsOk = CreateProcess(
					_T("SubProcess(A).exe"),
					BufferData,
					NULL,
					NULL,
					TRUE,
					CREATE_NEW_CONSOLE,
					NULL,
					NULL,
					&StartupInfo, 
					&ProcessInfo
				);
	
	if (IsOk == FALSE) {
		if (hRealProcess == NULL)
			CloseHandle(hRealProcess);
		return;
	}

	if (ProcessInfo.hProcess != NULL) {
		CloseHandle(ProcessInfo.hProcess);
		ProcessInfo.hProcess = NULL;
	}

	if (ProcessInfo.hThread != NULL) {
		CloseHandle(ProcessInfo.hThread);
		ProcessInfo.hThread = NULL;
	}

	Sleep(2000);

	SetHandleInformation(hRealProcess, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);
	
	ZeroMemory(&StartupInfo, sizeof(StartupInfo));
	StartupInfo.cb = sizeof(STARTUPINFO);

	ZeroMemory(&ProcessInfo, sizeof(ProcessInfo));

	IsOk = FALSE;
	IsOk = CreateProcess(
					_T("SubProcess(B).exe"),
					BufferData,
					NULL,
					NULL,
					TRUE,
					CREATE_NEW_CONSOLE,
					NULL,
					NULL,
					&StartupInfo,
					&ProcessInfo
				);
	if (IsOk == FALSE) {
		if (hRealProcess == NULL)
			CloseHandle(hRealProcess);
		return;
	}

	if (ProcessInfo.hProcess != NULL) {
		CloseHandle(ProcessInfo.hProcess);
		ProcessInfo.hProcess = NULL;
	}
	if (ProcessInfo.hThread != NULL) {
		CloseHandle(ProcessInfo.hThread);
		ProcessInfo.hThread = NULL;
	}

	if (hRealProcess != NULL) {
		CloseHandle(hRealProcess);
		hRealProcess = NULL;
	}

	return;
}
