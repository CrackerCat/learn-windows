#include "FileSuicide.h"
#include <Shlobj.h>
#pragma comment(lib,"Shell32.lib")

int _tmain(int argc, TCHAR argv[], TCHAR envp[])
{
	SuicideByBat();
	return 0;
}


BOOL SuicideByBat()
{
	CHAR  szFileDir[MAX_PATH] = { 0 };
	CHAR  szFilePath[MAX_PATH] = { 0 };
	
	CHAR BatFileFullPath[MAX_PATH] = { 0 };
	GetModuleFileNameA(0, szFilePath, sizeof(szFilePath));
	GetCurrentDirectoryA(MAX_PATH, szFileDir);
	
	
	strcpy_s(BatFileFullPath, szFileDir);
	strcat_s(BatFileFullPath, "\\SuicideByBatFile.bat");
	
	HANDLE hFile = CreateFileA(
			BatFileFullPath,
			GENERIC_READ | GENERIC_WRITE, 
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, 
			CREATE_ALWAYS, 
			FILE_ATTRIBUTE_NORMAL, NULL
		);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	CHAR  szBat_1[MAX_PATH] { 0 };
	CHAR  szBat_2[MAX_PATH] { 0 };


	sprintf_s(szBat_1, "del %s\n", szFilePath);
	sprintf_s(szBat_2, "del %%0\n");
	
	
	
	
	DWORD dwWrittenBytes = 0;
	WriteFile(hFile, szBat_1, strlen(szBat_1), &dwWrittenBytes, NULL);
	WriteFile(hFile, szBat_2, strlen(szBat_2), &dwWrittenBytes, NULL);
	CloseHandle(hFile);
	
	
	WinExec(BatFileFullPath, SW_HIDE);
	
	return TRUE;
}


BOOL SuicideByCmd()
{
	SHELLEXECUTEINFO ShellExecuteInfo = { 0 };
	TCHAR szFilePath[MAX_PATH] = { 0 };
	TCHAR szCmdPath[MAX_PATH] = { 0 };
	TCHAR szParam[MAX_PATH] = { 0 };

	// get file full path and cmd full path
	if ((GetModuleFileName(0, szFilePath, MAX_PATH) != 0) &&
		(GetEnvironmentVariable(_T("COMSPEC"), szCmdPath, MAX_PATH) != 0))
	{
		// set cmd parameter 
		_tcscpy_s(szParam, _T("/c del "));
		_tcscat_s(szParam, szFilePath);
		_tcscat_s(szParam, _T(" > nul"));

		// set SHELLEXECUTEINFO struct
		ShellExecuteInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		ShellExecuteInfo.lpVerb = _T("Open");
		ShellExecuteInfo.lpFile = szCmdPath;
		ShellExecuteInfo.lpParameters = szParam;
		ShellExecuteInfo.nShow = SW_HIDE;
		ShellExecuteInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		// execute Shell command
		if (ShellExecuteEx(&ShellExecuteInfo))
		{
			// set cmd 's previlege IDLE
			SetPriorityClass(ShellExecuteInfo.hProcess, IDLE_PRIORITY_CLASS);
			SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
			SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

			// notice Windows source browser - the file was deleted
			SHChangeNotify(SHCNE_DELETE, SHCNF_PATH, szFilePath, 0);
			return TRUE;
		}
	}
	return FALSE;
}

