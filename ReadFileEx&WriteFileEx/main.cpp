#include "packet.h"

HANDLE g_hEvent = INVALID_HANDLE_VALUE;


int _tmain(int argc, TCHAR *argv[], TCHAR *envp[])
{

	// sub_1();   // WriteFile(Overlapped)   GetOverlappedResult
	sub_2();   // WriteFileEx(CompletionRoutine)

	_tprintf_s(_T("Input any key to exit\r\n"));
	_gettchar();
	return 0;
}

void sub_1()
{
	if (SetConsoleCtrlHandler(
		(PHANDLER_ROUTINE)ConsoleHandlerRoutine, TRUE) == FALSE)
	{
		return;
	}

	HANDLE hFile = INVALID_HANDLE_VALUE;
	HANDLE hThread = NULL;
	TCHAR  szFilePath[MAX_PATH] = { 0 };

	// get current directory
	GetCurrentDirectory(MAX_PATH, szFilePath);
	_tcsrchr(szFilePath, _T('\\'))[1] = _T('\0');
	// append file name to dirctory to get file full path

#ifdef _WIN64
	_tcscat_s(szFilePath, _T("x64\\WriteFile.txt"));
#else
	_tcscat_s(szFilePath, _T("x86\\WriteFile.txt"));
#endif

	// create target file
	hFile = CreateFile(
			  szFilePath, 
			  GENERIC_WRITE, 
			  NULL, NULL,
			  CREATE_ALWAYS, 
			  FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
			  NULL
		  );

	_PACKET_ Object;
	Object.m_hFile = hFile;
	g_hEvent = Object.m_hEvents[0];
	// call GetOverlappedResult function when writting task is completed.
	hThread = CreateThread(
		NULL, 0, 
		(LPTHREAD_START_ROUTINE)ThreadProc, 
		(LPVOID)&Object, 0, NULL);

	for (int i = 0; i < MAX; i++)
	{
		TCHAR buf[MAX_PATH] = { 0 };


		// set the writting buffer
		_stprintf_s(buf, _T("CurrentEventHandle: [%d]\r\n"), Object.m_UserData[i].ol.hEvent);
		_tcscat_s(buf, _T("HelloWorld"));
		_tcscat_s(buf, _T("\r\n"));

		Object.m_UserData[i].dwTimeStamp = GetTickCount();

		DWORD dwWrittenBytes = 0;
		BOOL fOk = WriteFile(hFile, buf, _tcslen(buf) * sizeof(TCHAR), &dwWrittenBytes,
			(LPOVERLAPPED)&Object.m_UserData[i].ol);

		if (fOk == FALSE)
		{
			if (GetLastError() != ERROR_IO_PENDING)
			{
				return;
			}

		}

	}
	WaitForSingleObject(hThread, INFINITE);

}

DWORD WINAPI ThreadProc(LPVOID lpvParam)
{
	OVERLAPPED ol = { 0 };
	_PACKET_* Object = (_PACKET_*)lpvParam;
	HANDLE hEvent = NULL;
	DWORD fOk = FALSE;

	while (TRUE)
	{
		DWORD dwWrittenBytes = 0;
		fOk = WaitForMultipleObjects(MAX + 1, Object->m_hEvents, FALSE, INFINITE);
		if (fOk == WAIT_FAILED)
		{
			break;
		}

		fOk = fOk - WAIT_OBJECT_0;

		switch (fOk)
		{
		case 1:
		{
			hEvent = Object->m_hEvents[0];
			break;
		}
		case 2:
		{
			hEvent = Object->m_hEvents[1];
			break;
		}
		case 3:
		{
			hEvent = Object->m_hEvents[2];
			break;
		}
		case 0:
		{
			// exit
			_tprintf_s(_T("work thread exit\r\n"));
			return TRUE;
		}
		default:
		{

			return FALSE;
		}

		}
		/*BOOL WINAPI GetOverlappedResult(
		_In_  HANDLE       hFile,
		_In_  LPOVERLAPPED lpOverlapped,
		_Out_ LPDWORD      lpNumberOfBytesTransferred,
		_In_  BOOL         bWait
		);*/
		fOk = GetOverlappedResult(
				Object->m_hFile, 
				&Object->m_UserData[fOk].ol, 
				&dwWrittenBytes, 
				FALSE
			);

		if (fOk > 0 && hEvent != INVALID_HANDLE_VALUE)
		{
			_tprintf_s(_T("Event Handle [%d]    WriteFile() Success!   TimeCount[%d]\r\n"), 
				hEvent, GetTickCount() - Object->m_UserData[fOk].dwTimeStamp);

		}
	}
	return FALSE;

}


// WriteFileEx (CompletionRoutine)
void sub_2()
{
	/*
	BOOL WINAPI WriteFileEx(
	_In_     HANDLE                          hFile,
	_In_opt_ LPCVOID                         lpBuffer,
	_In_     DWORD                           nNumberOfBytesToWrite,
	_Inout_  LPOVERLAPPED                    lpOverlapped,
	_In_     LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);*/

	TCHAR  szFilePath[MAX_PATH] = { 0 };
	HANDLE hFile = INVALID_HANDLE_VALUE;

	// get current directory
	GetCurrentDirectory(MAX_PATH, szFilePath);

	_tcsrchr(szFilePath, _T('\\'))[1] = _T('\0');

	// append file name
#ifdef _WIN64
	_tcscat_s(szFilePath, _T("x64\\WriteFileEx.txt"));
#else
	_tcscat_s(szFilePath, _T("x86\\WriteFileEx.txt"));
#endif

	hFile = CreateFile(
			szFilePath, 
			GENERIC_WRITE, NULL, 
			NULL, CREATE_ALWAYS, 
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, 
			NULL
		);

	USER_DATA UserData[MAX];

	for (int i = 0; i < MAX; i++)  //100  10    $g  10  40960  
	{
		
		UserData[i].ol.Offset = 0xFFFFFFFF;
		UserData[i].ol.OffsetHigh = 0xFFFFFFFF;
		

		TCHAR buf[MAX_PATH] = { 0 };


		_stprintf_s(buf, _T("%s\r\n"), _T("HelloWorld"));

		DWORD dwSize = _tcslen(buf) * sizeof(TCHAR);

		UserData[i].dwTimeStamp = GetTickCount();
		
		// WriteFileEx() ignores the ol structure, 
		// it notify the user by completion rountion.
		WriteFileEx(hFile, buf, dwSize, &UserData[i].ol, CompletionRoutine);

	}
	SleepEx(INFINITE, TRUE);
	/*
		DWORD SleepEx(
			DWORD dwMilliseconds,
			BOOL  bAlertable);

		DWORD WaitForSingleObjectEx(
			HANDLE  hObject,
			DWORD   dwMilliseconds,
			BOOL    bAlertable);

		DWORD WaitForMultipleObjectsEx(
			DWORD   cObjects,
			CONST HANDLE* phObjects,
			BOOL    bWaitAll,
			DWORD   dwMilliseconds,
			BOOL    bAlertable);

		BOOL SignalObjectAndWait(
			HANDLE  hObjectToSignal,
			HANDLE  hObjectToWaitOn,
			DWORD   dwMilliseconds,
			BOOL    bAlertable);

		BOOL GetQueuedCompletionStatusEx(
			HANDLE  hCompPort,
			LPOVERLAPPED_ENTRY pCompPortEntries,
			ULONG   ulCount,
			PULONG  pulNumEntriesRemoved,
			DWORD   dwMilliseconds,
			BOOL    bAlertable);

		DWORD MsgWaitForMultipleObjectsEx(
			DWORD   nCount,
			CONST HANDLE* pHandles,
			DWORD   dwMilliseconds,
			DWORD   dwWakeMask,
			DWORD   dwFlags);
		*/

	
	if (hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;
	}
}

VOID WINAPI CompletionRoutine( 
	DWORD       dwErrorCode,
	DWORD       dwNumberOfBytesTransfered,
	OVERLAPPED* lpOverlapped)
{

	if (dwNumberOfBytesTransfered > 0)
	{
		DWORD TickCount = GetTickCount();
		USER_DATA* UserData = CONTAINING_RECORD(lpOverlapped, USER_DATA, ol);

		_tprintf_s(_T("CurrentThreadID[%d] WriteFileEx() Success! TimeCount[%d]\r\n"), 
			GetCurrentThreadId(), TickCount - UserData->dwTimeStamp);
	}
}


BOOL WINAPI ConsoleHandlerRoutine(DWORD dwEventID)
{
	switch (dwEventID)
	{
	case CTRL_C_EVENT:
		break;
	case CTRL_BREAK_EVENT:
		break;
	case CTRL_CLOSE_EVENT:
		if (INVALID_HANDLE_VALUE != g_hEvent)
		{
			SetEvent(g_hEvent);
			Sleep(0);
		}
		break;
	case CTRL_LOGOFF_EVENT:
		break;
	case CTRL_SHUTDOWN_EVENT:
		break;
	}
	return TRUE;
}
