#include "deepDir.h"


int _tmain(int argc, TCHAR *argv[], TCHAR *envp[])
{
	HANDLE hFile = 
		CreateFileInDirectory(
			_T("D:\\1\\2\\3\\4\\5\\6\\7\\8\\Test.txt"),
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ,
			NULL,
			CREATE_NEW,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		);

	_tsystem(_T("PAUSE"));

	DeleteDirectory(_T("D:\\1"), _tcslen(_T("D:\\1")));

	if (hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
		hFile = NULL;
	}
	
	return 0;
}
