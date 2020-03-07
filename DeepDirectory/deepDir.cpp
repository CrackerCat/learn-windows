#include "deepDir.h"


HANDLE 
CreateFileInDirectory(
	_In_ LPCTSTR lpFullPath, 
	_In_ DWORD dwDesiredAccess,
	_In_ DWORD dwShareMode, 
	_In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	_In_ DWORD dwCreationDisposition, 
	_In_ DWORD dwFlagsAndAttributes,
	_In_opt_ HANDLE hTemplateFile)
{
	HANDLE hFile = NULL;
	LPTSTR lpDirPath = NULL;
	SIZE_T nPathLen = 0;

	TCHAR buf[MAX_PATH] = { 0 };
	_tcscpy_s(buf, lpFullPath);

	TCHAR *ch = buf;
	while (*ch != _T('\\'))
		ch++;
	ch++;

	while (*ch != _T('\0'))
	{
		if (*ch == _T('\\')) {

			nPathLen = ch - buf;
			lpDirPath = new TCHAR[nPathLen + 1];
			memset(lpDirPath, 0, (nPathLen + 1) * sizeof(TCHAR));
			memcpy(lpDirPath, buf, nPathLen * sizeof(TCHAR));

			if (CreateDirectory(lpDirPath, NULL) == 0) {

				if (GetLastError() == ERROR_ALREADY_EXISTS) {

					ch++;
					continue;
				} else {
					return FALSE;
				}
			} else ch++;
		} else ch++;
	}

	hFile = CreateFile(
			lpFullPath, dwDesiredAccess, 
			dwShareMode, lpSecurityAttributes,
			dwCreationDisposition, dwFlagsAndAttributes, 
			hTemplateFile);

	return hFile;
}

BOOL
DeleteDirectory(
	_In_ LPCTSTR lpFullPath,
	_In_ SIZE_T nPathLen
)
{
	BOOL rVal = TRUE;
	WIN32_FIND_DATA	Win32FindData = { 0 };
	TCHAR szBuf[MAX_PATH] = { 0 };

	/* test a parameter if valid */
	if (IsBadReadPtr(lpFullPath, nPathLen)) {

		SetLastError(ERROR_INVALID_PARAMETER);
		rVal = FALSE;
		return rVal;
	}

	_stprintf_s(szBuf, _T("%s\\*.*"), lpFullPath);

	/* searches a directory for a file or subdirectory */
	HANDLE hFindFile = FindFirstFile(szBuf, &Win32FindData);


	if (hFindFile == INVALID_HANDLE_VALUE) {
		rVal = FALSE;
		return rVal;
	}

	do {
		/* ignore the dot dir and double dots dir */
		if (Win32FindData.cFileName[0] == _T('.')&& _tcslen(Win32FindData.cFileName) <= 2)
			continue;

		else
		{
			/* if is subdirectory */
			if (Win32FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				TCHAR szDirPath[MAX_PATH] = { 0 };
				_stprintf_s(szDirPath, _T("%s\\%s"), lpFullPath, Win32FindData.cFileName);

				/* delete the directory recursively */
				DeleteDirectory(szDirPath, _tcslen(szDirPath));
			}
			else /* if is file */
			{
				TCHAR szFilePath[MAX_PATH] = { 0 };
				_stprintf_s(szFilePath, _T("%s\\%s"), lpFullPath, Win32FindData.cFileName);

				/* delete the file dirctly */
				DeleteFile(szFilePath);
			}
		}
		/* delete the next item */
	} while (FindNextFile(hFindFile, &Win32FindData));

	/* close the search handle */
	FindClose(hFindFile);

	/* delete the empty dirctory */
	if (!RemoveDirectory(lpFullPath))
		rVal = FALSE;

	return rVal;
}
