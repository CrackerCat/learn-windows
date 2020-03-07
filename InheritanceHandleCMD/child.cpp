#include <windows.h>
#include <tchar.h>
#include <cstdio>

int _tmain(int argc, TCHAR* argv[], TCHAR *envp[])
{

	ULONG_PTR  v1 = 0;
	// the handle value is at 0 index in CommandLine with single word. 
	_stscanf_s((TCHAR*)argv[0], _T("%d"), &v1);
	HANDLE EventHandle = (HANDLE)v1;

	_tprintf_s(_T("EventHandle:%d\r\n"), EventHandle);

	__try
	{
		// Set the Event
		SetEvent(EventHandle);
		Sleep(5000);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		ExitProcess(0);
	}
	return;
}
