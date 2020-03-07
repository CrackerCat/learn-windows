#include <windows.h>
#include <tchar.h>
#include <iostream>
using namespace std;

void Sub_1();
#define  UM_SEND  (WM_USER + 0x0010)

int _tmain(int _argc, TCHAR* _argv[], TCHAR* _envp[])
{
	setlocale(LC_ALL,"Chinese-simplified");
	Sub_1();
	_tprintf_s(_T("input any key to exit\r\n"));
	_gettchar();

	return;
}

void Sub_1()
{
	SECURITY_ATTRIBUTES sa;
	ZeroMemory(&sa, sizeof(sa));

	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle       = TRUE;
	sa.lpSecurityDescriptor = NULL;

	HANDLE hEvent = CreateEvent(&sa, TRUE, FALSE, NULL);
	if (hEvent == NULL) {
		_tprintf_s(_T("CreateEvent() Error"));
		return;
	}
	

	STARTUPINFO StartupInfo;
	PROCESS_INFORMATION  ProcessInfo;
	ZeroMemory(&StartupInfo, sizeof(StartupInfo));
	StartupInfo.cb = sizeof(STARTUPINFO);
	ZeroMemory(&ProcessInfo, sizeof(PROCESS_INFORMATION));


	int IsOk = TRUE;

	IsOk = CreateProcess(
					_T("SubProcess.exe"),
					NULL, 
					NULL, 
					NULL, 
					TRUE, 
					CREATE_NEW_CONSOLE, 
					NULL, 
					NULL, 
					&StartupInfo, 
					&ProcessInfo
				);

	if (IsOk != 0)
	{
		while (TRUE)
		{
			IsOk = WaitForInputIdle(ProcessInfo.hProcess, 1000);
			if (IsOk == WAIT_FAILED) {
				break;
			}
			else if (IsOk == WAIT_TIMEOUT) {
				continue;
			}
			else {
				HWND hWnd = FindWindow(NULL, _T("Dialog"));
				SendMessage(hWnd, UM_SEND, NULL, (LPARAM)hEvent);
				WaitForSingleObject(hEvent, INFINITE);
				_tprintf(_T("Hello World\r\n"));
				break;
			}
		}
	}

	if (ProcessInfo.hThread) {
		CloseHandle(ProcessInfo.hThread);
	}
	
	if (ProcessInfo.hProcess) {
		CloseHandle(ProcessInfo.hProcess);
	}
	if (hEvent) {
		CloseHandle(hEvent);
	}
}
