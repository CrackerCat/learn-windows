#include <windows.h>
#include <iostream>
#include <tchar.h>

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR *envp[])
{
	setlocale(LC_ALL, "Chinese");

	ULONG_PTR  v1 = 0;
	
	_stscanf_s(argv[0], _T("%d"), &v1);
	HANDLE hPseudoProcess = (HANDLE)v1;
	
	_stscanf_s(argv[1], _T("%d"), &v1);
	HANDLE hRealProcess = (HANDLE)v1;


	_tprintf(_T("Inherit Handle: %d %d\r\n"), (int)hPseudoProcess, (int)hRealProcess);
	

	_tprintf(_T("Try to Terminate Parent Process!\r\n"));
	Sleep(5000);

	if (!TerminateProcess(hRealProcess, 0)) {
		_tprintf(_T("Fail to Terminate Parent Process!\r\n"));
		
		Sleep(5000);
	}

	_tprintf(_T("Input AnyKey To Exit\r\n"));
	_gettchar();

	ExitProcess(0);

	return;
}
