#include <windows.h>
#include <iostream>
#include <tchar.h>

using namespace std;

int _tmain(int _argc, TCHAR* _argv[], TCHAR *_envp[])
{
	setlocale(LC_ALL, "Chinese");

	ULONG_PTR  v1 = 0;
	
	_stscanf_s(_argv[0], _T("%d"), &v1);
	HANDLE hPseudoProcess = (HANDLE)v1;
	_stscanf_s(_argv[1], _T("%d"), &v1);
	HANDLE hRealProcess = (HANDLE)v1;

	_tprintf_s(_T("Inherit Handle: %d %d\r\n"), hPseudoProcess, hRealProcess);

	if (!TerminateProcess(hRealProcess, 0)) {
		_tprintf(_T("Fail to Terminate Parent Process!\r\n"));
	}

	_tprintf_s(_T("Terminate myself\r\n"));
	Sleep(5000);

	if (! TerminateProcess(GetCurrentProcess(), 0)) {
		_tprintf(_T("Fail to Terminate myself!\r\n"));
	}

	_tprintf_s(_T("Input AnyKey To Exit\r\n"));
	_gettchar();
	ExitProcess(0);

	return 0;
}
