#include "asychronous.h"



// file pointer
LARGE_INTEGER  __FilePointer { 0 };

// thread pool io completion port
PTP_IO __ThreadPoolIo = NULL;

// thread count
const SIZE_T MAX_THREAD = 2;

int _tmain(int argc, LPCTSTR argv[], LPCTSTR envp[])
{

	_tprintf(_T("main thread ID：0x%x\n"), GetCurrentThreadId());


	HANDLE hThreads[MAX_THREAD]{ 0 };
	TCHAR  szFileName[MAX_PATH] = _T("HelloWorld.txt");


	// create file object
	HANDLE hFile = CreateFile(
			szFileName,
			GENERIC_WRITE, 
			FILE_SHARE_READ, NULL, 
			CREATE_ALWAYS, 
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, 
			NULL);


	if (hFile == INVALID_HANDLE_VALUE) {

		_tprintf(_T("Err: CreateFile(%s) ( %d )\r\n"), szFileName, GetLastError());
		_tsystem(_T("PAUSE"));
		return 0;
	}

	TP_CALLBACK_ENVIRON PoolEnvironment { 0 };
	InitializeThreadpoolEnvironment(&PoolEnvironment);

	// create thread pool I/O object

		// create a thread pool I/O object by calling CreateThreadpoolIo, 
		// passing into it the handle of the file/device which associated 
		// with the thread pool's internal I/O completion port.
	__ThreadPoolIo = CreateThreadpoolIo(
			hFile, 
			OverlappedCompetionRoutine,
			hFile, 
			&PoolEnvironment);
	/*
		PTP_IO WINAPI CreateThreadpoolIo(
			_In_        HANDLE                fl,
			_In_        PTP_WIN32_IO_CALLBACK pfnio,
			_Inout_opt_ PVOID                 pv,
			_In_opt_    PTP_CALLBACK_ENVIRON  pcbe
		);

		Parameter:
			fl		The file handle to bind to this I/O completion object.
			pfnio		The callback function to be called each time an overlapped 
						I/O operation completes on the file. 
			pv		Optional application-defined data to pass to the callback function.
			pcbe		defines the environment in which to execute the callback
	*/

#ifdef _UNICODE
	StartThreadpoolIo(__ThreadPoolIo);

	// Unicode Prefix : 0xff 0xfe
	PUSER_DATA  UserData = new USER_DATA;


	if (UserData == NULL) {	
		return 0;
	}

	UserData->dwOption = OPTION_WRITE;
	UserData->hFile = hFile;
	UserData->lpBuff = new WORD;


	if (UserData->lpBuff == NULL) {

		delete UserData;
		return 0;
	}

	// unicode prefix
	*((WORD*)UserData->lpBuff) = MAKEWORD(0xff, 0xfe);
	UserData->cbSize = sizeof(WORD);

	// set new file pointer
	UserData->ol.Offset = __FilePointer.LowPart;
	UserData->ol.OffsetHigh = __FilePointer.HighPart;
	__FilePointer.QuadPart += UserData->cbSize;

	UserData->dwTimestamp = GetTickCount();


	WriteFile(
		hFile, 
		UserData->lpBuff, 
		UserData->cbSize, 
		&UserData->dwBytesToWritten,
		&UserData->ol);

	//FlushFileBuffers(FileHandle);
	//Sleep(1);
#endif


	// start write threads
	for (int i = 0; i < MAX_THREAD; i++) {

		hThreads[i] = CreateThread(
			NULL, 0, 
			(LPTHREAD_START_ROUTINE)ThreadProc, 
			hFile, 
			0, NULL);
	}

	// wait until every subthreads exit
	WaitForMultipleObjects(MAX_THREAD, hThreads, TRUE, INFINITE);

	for (int i = 0; i < MAX_THREAD; i++) {

		CtsCloseHandle(hThreads[i]);
	}

	// wait for an outstanding I/O request to complete
	WaitForThreadpoolIoCallbacks(__ThreadPoolIo, FALSE);

	// close it and disassociate it from the thread pool
	CloseThreadpoolIo(__ThreadPoolIo);

	// clean envp
	DestroyThreadpoolEnvironment(&PoolEnvironment);

	// close file object
	if (hFile != INVALID_HANDLE_VALUE) {

		CtsCloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;
	}


	_tprintf(_T("input any key to exit\r\n"));
	_gettchar();
	return 0;
}

VOID 
CALLBACK 
OverlappedCompetionRoutine(
	PTP_CALLBACK_INSTANCE Instance, 
	PVOID pvContext, 
	PVOID pOverlapped,
	ULONG IoResult,
	ULONG_PTR NumberOfBytesTransferred, 
	PTP_IO pIo)
{
	if (IoResult != NO_ERROR) {

		_tprintf(_T("Err: I/O ( %d )\r\n"), IoResult);
		return;
	}

	PUSER_DATA UserData = CONTAINING_RECORD(
		reinterpret_cast<LPOVERLAPPED>(pOverlapped), USER_DATA, ol);


	if (reinterpret_cast<HANDLE>(pvContext) == UserData->hFile) {
		switch (UserData->dwOption)
		{
		case OPTION_WRITE:
			// complete
			_tprintf(
				_T("thread[0x%x] complete IO request\r\n"),
				GetCurrentThreadId());

			if (UserData->lpBuff != NULL) {

				delete UserData->lpBuff;
				UserData->lpBuff = NULL;
			}

			if (UserData != NULL) {
				delete UserData;
				UserData = NULL;
			}
			break;
		case OPTION_READ:
			break;
		default:
			break;

		}
	}
}

DWORD WINAPI ThreadProc(LPVOID lpParam)
{
	
	PUSER_DATA  UserData = NULL;
	SIZE_T cbSize = 0;
	LPTSTR lpBuff = NULL;
	HANDLE hFile = reinterpret_cast<HANDLE>(lpParam);
	TCHAR szMsg[MAX_PATH] { 0 };

	StringCchPrintf(szMsg, MAX_PATH, _T("Thread[0x%x] writes\r\n"), GetCurrentThreadId());
	cbSize = (_tcslen(szMsg)) * sizeof(TCHAR);

	for (int i = 0; i < MAX_THREAD; i++) {

		lpBuff = (LPTSTR)new TCHAR[cbSize + sizeof(TCHAR)];
		if (lpBuff == NULL) {

			return 0;
		}

		memset(lpBuff, 0, cbSize + sizeof(TCHAR));
		memcpy(lpBuff, szMsg, cbSize);


		UserData = new USER_DATA;
		if (UserData == NULL) {

			return 0;
		}

		UserData->dwOption = OPTION_WRITE;
		UserData->lpBuff = lpBuff;
		UserData->cbSize = cbSize;

		UserData->hFile = hFile;

		// set file pointer, and write at the end of the file
		*((LONGLONG*)&UserData->ol.Pointer) = InterlockedCompareExchange64(
			&__FilePointer.QuadPart,
			__FilePointer.QuadPart + UserData->cbSize, 
			__FilePointer.QuadPart);

		UserData->dwTimestamp = GetTickCount(); // get time stamp  

		StartThreadpoolIo(__ThreadPoolIo);

		// write
		WriteFile(
			hFile, UserData->lpBuff,
			UserData->cbSize, &UserData->dwBytesToWritten,
			&UserData->ol);
	}

	return 0;
}
BOOL CtsCloseHandle(HANDLE hObject)
{
	DWORD HandleFlags;
	if (GetHandleInformation(hObject, &HandleFlags)
		&& (HandleFlags & HANDLE_FLAG_PROTECT_FROM_CLOSE) != HANDLE_FLAG_PROTECT_FROM_CLOSE)
		return !!CloseHandle(hObject);
	return FALSE;
}
