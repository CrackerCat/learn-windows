#include <cstdlib>
#include <iostream>
#include <tchar.h>
#include <windows.h>

using namespace std;

void Sub_1();
void Sub_2();
void Sub_3();

int _tmain(int argc, TCHAR* argv[], TCHAR *envp[])
{

	setlocale(LC_ALL, "Chinese-simplified");

	_tprintf_s(_T("%s\n"), argv[0]);
	_tprintf_s(_T("CommandLine Address：%p\n"), argv);

	SIZE_T Size = sizeof(MEMORY_BASIC_INFORMATION);
	MEMORY_BASIC_INFORMATION MemoryBasicInfo;
	ZeroMemory(&MemoryBasicInfo, sizeof(MemoryBasicInfo));
	VirtualQuery(argv[0], &MemoryBasicInfo, Size);

	_tprintf_s(_T("Protect Attribute：%d\n"), MemoryBasicInfo.AllocationProtect);  // PAGE_READWRITE
	
	Sub_1();
	Sub_2();
	Sub_3();

	_tprintf_s(_T("Input AnyKey To Exit\r\n"));
	_gettchar();

	return 0;
}


void Sub_1()
{
#ifdef _UNICODE
	wprintf_s(L"%s\n", __wargv[0]);
	wprintf_s(L"CommandLine Address：%p \r\n", __wargv);
#else
	printf_s("%s\n", __argv[0]);
	printf_s("CommandLine Address：%p \r\n", __argv);
#endif 
}

void Sub_2()
{
	TCHAR*  CommandLine = GetCommandLine();
	_tprintf_s(_T("%s\r\n"), CommandLine);

	//Query CommandLine Memory protect attribute
	SIZE_T Size = sizeof(MEMORY_BASIC_INFORMATION);
	MEMORY_BASIC_INFORMATION MemoryBasicInfo;
	ZeroMemory(&MemoryBasicInfo, sizeof(MemoryBasicInfo));
	VirtualQuery(CommandLine, &MemoryBasicInfo, Size);

	_tprintf_s(_T("CommandLine Address：%p\r\n"), CommandLine);
	_tprintf_s(_T("Protect Attribute  ：%d\r\n"), MemoryBasicInfo.AllocationProtect);
}

void Sub_3()
{
	LPWSTR*   argv_U = NULL;
	INT		NumArgs = 0;
	INT		i = 0;

	argv_U = CommandLineToArgvW(GetCommandLineW(), &NumArgs);

	if (NULL == argv_U) {
		return;
	}
	else {
		for (i = 0; i < NumArgs; i++) {
			// printf("%d: %ws\n", i, argv_U[i]);
			wprintf(L"%d: %s\r\n", i, argv_U[i]);
		}
		LocalFree(argv_U);
	}
	return;

	/*
	LPWSTR * CommandLineToArgvW(LPCWSTR _lpCmdLine, int* _pNumArgs)
	{
		LPWSTR* argv_U;
		LPWSTR  cmdstart;                 // start of command line to parse
		INT     numbytes;
		WCHAR   pgmname[MAX_PATH];

		if (_pNumArgs == NULL) {
			SetLastError(ERROR_INVALID_PARAMETER);
			return NULL;
		}

		// Get the program name pointer from Win32 Base

		GetModuleFileName(NULL, pgmname, sizeof(pgmname) / sizeof(WCHAR));

		// if there's no command line at all (won't happen from cmd.exe, but
		// possibly another program), then we use pgmname as the command line
		// to parse, so that argv[0] is initialized to the program name
		cmdstart = (*_lpCmdLine == TEXT('\0')) ? pgmname : (LPWSTR)_lpCmdLine;

		//first find out how much space is needed to store args
		Parse_Cmdline(cmdstart, NULL, NULL, _pNumArgs, &numbytes);

		//allocate space for argv[] vector and strings
		argv_U = (LPWSTR*)LocalAlloc(                         //LocalAlloc !!!!
								LMEM_ZEROINIT,				
								(*_pNumArgs + 1) * sizeof(LPWSTR) + numbytes
							);
		if (!argv_U) {
			SetLastError(ERROR_NOT_ENOUGH_MEMORY);
			return (NULL);
		}

		//store args and argv ptrs in just allocated block
		Parse_Cmdline(
				cmdstart, 
				argv_U,
				(LPWSTR)(((LPBYTE)argv_U) + *_pNumArgs * sizeof(LPWSTR)),
				_pNumArgs, 
				&numbytes
			);

		return (argv_U);
	}
	*/
}
