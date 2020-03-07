// Parent Process

#include <windows.h>
#include <tchar.h>
#include <cstdio>


int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	HandleInheritance();
	return 0;
}

void HandleInheritance()
{

	// Create an Event Object (bInheritHandle = TRUE)
	SECURITY_ATTRIBUTES SecurityAttributes;
	
	SecurityAttributes.nLength 
		= sizeof(SECURITY_ATTRIBUTES);
	SecurityAttributes.lpSecurityDescriptor 
		= NULL;
	SecurityAttributes.bInheritHandle 
		= TRUE;
        
	HANDLE EventHandle = CreateEvent(&SecurityAttributes, TRUE, FALSE, NULL);
	if (EventHandle == NULL) {
		OutputDebugString(_T("Creating Event Object Fails\r\n"));
	}
	else {
		_tprintf_s(_T("EventHandle:%d\r\n"), EventHandle);
		TCHAR BufferData[20];
		ZeroMemory(BufferData,sizeof(BufferData));
		_stprintf_s(BufferData, _T("%d"), EventHandle);

		STARTUPINFO StartupInfo;
		ZeroMemory(&StartupInfo, sizeof(STARTUPINFO));
		StartupInfo.cb = sizeof(STARTUPINFO);

		PROCESS_INFORMATION ProcessInfo;
		ZeroMemory(&ProcessInfo, sizeof(PROCESS_INFORMATION));

		// CommandLine Pass Handle Value;
		BOOL IsOk = CreateProcess(
						_T("Child_Process.exe"),
						(LPTSTR)BufferData, 
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
			OutputDebugString(_T("Create Child Process Fails\r\n"));
		}

		
		if (ProcessInfo.hProcess != NULL || ProcessInfo.hThread != NULL) {
			CloseHandle(ProcessInfo.hProcess);
			CloseHandle(ProcessInfo.hThread);
			ProcessInfo.hProcess = NULL;
			ProcessInfo.hThread  = NULL;
		}
		_tprintf_s(_T("BufferData:%s\r\n"), BufferData);
		// Waits until Event Signals
		WaitForSingleObject(EventHandle, INFINITE);

		// When child process sets Event,pop-up a MessageBox
		MessageBox(NULL, _T("Child Process Sets Event, Handle Inheritance Succeeds"), _T("Success"), MB_OK);
		
		if (EventHandle != NULL) {
			CloseHandle(EventHandle);
			EventHandle = NULL;
		}

		printf("Input any key to exit\r\n");
		getchar();
	}
}
