#include "SlimRWLock.h"

/*
SRWLock(Slim Read-Write Lock)
用来同步多线程对共享资源的访问。
不能递归调用自己！因为当查询到共享资源被锁定时，再次调用AcquireSRWLock系列函数会导致堵塞。
与临界区不同的是，SRWLock能够区分读取共享资源的线程（读者->AcquireSRWLockShared）和
修改共享资源的线程（修改者->AcquireSRWLockExclusive）。SRWLock允许多个读者并发的读取被保护的资源，
因为读取操作并不会引发数据破坏，但在修改者修改资源时，SRWLock不允许其它读者线程和修改者线程访问该资源。
*/
SRWLOCK      gSRWLock;                 // Global slim rw lock 
const int    READER_NUMBER = 5;        // the count of reader threads
int _tmain(int argc, TCHAR *argv[], TCHAR *envp[])
{


	_tprintf_s(_T("*****************************************\r\n"));

	// Initialize Slim Read Write Lock
	InitializeSRWLock(&gSRWLock);

	HANDLE hThread[READER_NUMBER + 1];
	ZeroMemory(hThread, sizeof(hThread));
	
	int i;
	// Start two reader threads
	for (i = 1; i <= 2; i++)
	{
		hThread[i] = CreateThread(
						NULL, 0, 
						(LPTHREAD_START_ROUTINE)ReaderThreadProc, 
						NULL, 0, NULL);
	}

	// Start a writer thread
	hThread[0] = CreateThread(
					NULL, 0, 
					(LPTHREAD_START_ROUTINE)WriterThreadProc, 
					NULL, 0, NULL);
	Sleep(50);
	// Start other (two) reader threads
	for (i; i <= READER_NUMBER; i++)
	{
		hThread[i] = CreateThread(
						NULL, 0, 
						(LPTHREAD_START_ROUTINE)ReaderThreadProc, 
						NULL, 0, NULL);
	}
	WaitForMultipleObjects(READER_NUMBER + 1, hThread, TRUE, INFINITE);
	for (i = 0; i < READER_NUMBER + 1; i++)
	{
		if (hThread[i] != NULL)
		{
			CtsCloseHandle(hThread[i]);
			hThread[i] = NULL;
		}

	}

	_tprintf_s(_T("Input AnyKey To Exit\r\n"));
	_gettchar();
	return 0;
}


VOID ReaderPrintf(LPCTSTR Format, ...)   //参数列表     
{
	va_list  arglist;
	va_start(arglist, Format);
	
#if UNICODE
	char mbstr[MAX_PATH];
	memset(mbstr, 0, sizeof(mbstr));
	size_t rval = 0;
	wcstombs_s(&rval, mbstr, Format, MAX_PATH);
	vfprintf(stdout, mbstr, arglist);
#else
	vfprintf(stdout, Format, arglist);
#endif
	va_end(arglist);
}
DWORD ReaderThreadProc(PVOID ParameterData)
{
	ReaderPrintf(_T("%d Reader Thread is waiting...\r\n"), GetCurrentThreadId());
	/*
	可使用TryAcquireSRWLockShared代替
	TryAcquireSRWLockShared与AcquireSRWLockShared区别：
	TryAcquireSRWLockShared    ->     尝试获得对被保护资源的共享访问权。如果不能获取，则返回零值；如果能获取，则返回非零值。不会堵塞。（BOOLEAN）
	AcquireSRWLockShared       ->     尝试获得对被保护资源的共享访问权。如果不能获取，则堵塞。（VOID）
	*/
	//尝试获得对被保护资源的共享访问，申请读取共享资源
	if (TryAcquireSRWLockShared(&gSRWLock))
	{
		// read shared resource
		ReaderPrintf(_T("%d Reader Thread is reading file...\r\n"), GetCurrentThreadId());
		Sleep(rand() % 100);
		ReaderPrintf(_T("%d Reader Thread finishs reading...\r\n"), GetCurrentThreadId());
		//finish reading
		ReleaseSRWLockShared(&gSRWLock);
	}
	else
	{
		AcquireSRWLockShared(&gSRWLock);
		// read shared resource
		ReaderPrintf(_T("%d Reader Thread is reading file...\r\n"), GetCurrentThreadId());
		Sleep(rand() % 100);
		ReaderPrintf(_T("%d Reader Thread finishs reading...\r\n"), GetCurrentThreadId());
		//finish reading
		ReleaseSRWLockShared(&gSRWLock);
	}

	// OS will automatically release SRWLock
	// OS never provide release API
	return 0;
}
VOID WriterPrintf(LPCTSTR String)
{
	_tprintf_s(_T("%s"), String);
}

DWORD WriterThreadProc(PVOID lpParam)
{
	WriterPrintf(_T("Writer Thread is waiting...\r\n"));

	/*
	可使用TryAcquireSRWLockExclusive代替
	TryAcquireSRWLockExclusived与AcquireSRWLockExclusive区别：
	TryAcquireSRWLockExclusive    ->    尝试获得对被保护资源的独占访问权。如果不能获取，则返回零值；如果能获取，则返回非零值。不会堵塞。（BOOLEAN）
	AcquireSRWLockExclusive       ->    尝试获得对被保护资源的独占访问权。如果不能获取，则堵塞。（VOID）
	*/
	//尝试获得对被保护资源的独占访问，申请修改共享资源尝试获得对被保护资源的独占访问
	if (TryAcquireSRWLockExclusive(&gSRWLock))
	{
		// Update shared resource
		WriterPrintf(_T("Reader Thread is updating file.....\r\n"));
		Sleep(rand() % 100);
		WriterPrintf(_T("Reader Thread finish updating\r\n"));
		// Release X Lock
		ReleaseSRWLockExclusive(&gSRWLock);
	}
	else
	{
		AcquireSRWLockExclusive(&gSRWLock);
		// Update shared resource
		WriterPrintf(_T("Reader Thread is updating file.....\r\n"));
		Sleep(rand() % 100);
		WriterPrintf(_T("Reader Thread finish updating\r\n"));
		// Release X Lock
		ReleaseSRWLockExclusive(&gSRWLock);
	}
	return 0;
}

BOOL CtsCloseHandle(HANDLE hObject)
{
	DWORD HandleFlags;
	if (GetHandleInformation(hObject, &HandleFlags) &&
		(HandleFlags & HANDLE_FLAG_PROTECT_FROM_CLOSE) != HANDLE_FLAG_PROTECT_FROM_CLOSE)
	{
		return CloseHandle(hObject);
	}

	return FALSE;
}
