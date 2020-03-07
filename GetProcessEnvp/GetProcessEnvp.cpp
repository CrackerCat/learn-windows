#include <windows.h>
#include <iostream>
#include <tchar.h>

using namespace std;

void GetEVinEnvp(TCHAR** envp);
void GetEVinGES();

int _tmain(int _argc, TCHAR* _argv[],TCHAR* _envp[])
{
	 GetEVinEnvp(_envp);
	 GetEVinGES(); 

	 _tprintf_s(_T("Input AnyKey To Exit\r\n"));
    _gettchar();
    
    return 0;
}


void GetEVinEnvp(TCHAR** envp)
{
	LPTSTR str = NULL;
	while (envp != NULL) {
		str = (LPWSTR)(*envp);
		if (str == NULL) {
			break;
		}
		else {
			_tprintf_s(_T("%s\r\n"), str);
			envp++;
		}
	}
	_tprintf_s(_T("\n\n"));

	return;
}

void GetEVinGES()
{
	LPTCH  ret  = NULL;
	LPTSTR envp = NULL;
	ret = GetEnvironmentStrings();

	if (ret == NULL) {
		_tprintf_s(_T("GetEnvironmentStrings() Error\r\n"));
	}
	else {
		envp = (LPTSTR)ret;
		while (*envp) {
			_tprintf_s(_T("%s\r\n"), envp);
			envp += lstrlen(envp) + sizeof(TCHAR);
		}
		FreeEnvironmentStrings(ret);
	}
	_tprintf_s(_T("\n\n"));

	return;
}
