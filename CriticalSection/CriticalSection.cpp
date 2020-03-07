#include "CriticalSection.h"

DWORD gRecursionCount; 
ULONG gWaitCount;
CRITICAL_SECTION gCS;

int _tmain(int argv, TCHAR* argc[], TCHAR* envp[])
{
	gWaitCount = 0;
	gRecursionCount = 0;
	Sub_1();
	gRecursionCount = 0;
	Sub_2();
	gRecursionCount = 0;
	Sub_3();
	gRecursionCount = 0;
	Sub_4();
	gRecursionCount = 0;
	Sub_5();
	_tprintf_s(_T("Input any key to exit...\r\n"));
	_gettchar();

	return 0;

}
// 1. The Critical Section can be created and deleted in different threads
// Thread1 - Create || Thread2 - delete
VOID Sub_1()
{
	_tprintf_s(_T("**************************************************************************\r\n"));
	_tprintf_s(_T("The Critical Section can be created and deleted in different threads\r\n"));
	int i = 0;
	HANDLE hThread[2];
	ZeroMemory(hThread, sizeof(hThread));
	hThread[0] = CreateThread(
					NULL, 0, 
					(LPTHREAD_START_ROUTINE)ThreadProc1_1,
					NULL, 0, NULL);
	WaitForSingleObject(hThread[0], INFINITE);

	hThread[1] = CreateThread(
					NULL, 0, 
					(LPTHREAD_START_ROUTINE)ThreadProc1_2, 
					NULL, 0, NULL);
	WaitForSingleObject(hThread[1], INFINITE);

	for (i = 0; i < 2; i++)
	{
		CloseHandle(hThread[i]);
		hThread[i] = NULL;
	}
	_tprintf_s(_T("**************************************************************************\r\n\r\n"));
	/*
	Debug Information
	The Critical Section can be created and deleted in different threads
	Current Thread Id: 18228  Owning Thread Id: 18228		RecursionCount:1    initCS
	Current Thread Id: 11216  Owning Thread Id: 11216		RecursionCount:1    deleCS
	*/
	return;
}
DWORD WINAPI ThreadProc1_1(LPVOID lpParam)
{
	InitializeCriticalSection(&gCS);
	EnterCriticalSection(&gCS);

	_tprintf_s(_T("Current Thread Id: %d\tOwning Thread Id: %d\tRecursionCount:%d\tInitCS\r\n"),
		GetCurrentThreadId(), (DWORD)gCS.OwningThread, gCS.RecursionCount);

	LeaveCriticalSection(&gCS);
	return 0;
}
DWORD WINAPI ThreadProc1_2(LPVOID lpParam)
{
	EnterCriticalSection(&gCS);

	_tprintf_s(_T("Current Thread Id: %d\tOwning Thread Id: %d\tRecursionCount:%d\tdeleCS\r\n"),
		GetCurrentThreadId(), (DWORD)gCS.OwningThread, gCS.RecursionCount);

	LeaveCriticalSection(&gCS);
	DeleteCriticalSection(&gCS);
	return 0;
}

// 2. RecursionCount
VOID Sub_2()
{
	_tprintf_s(_T("**************************************************************************\r\n"));
	_tprintf_s(_T("RecursionCount(itself++)\r\n"));
	int i = 0;
	HANDLE hThread = NULL;
	InitializeCriticalSection(&gCS);

	hThread = CreateThread(
		NULL, 0,
		(LPTHREAD_START_ROUTINE)ThreadProc2_1,
		NULL, 0, NULL);

	WaitForSingleObject(hThread, INFINITE);

	DeleteCriticalSection(&gCS);

	CloseHandle(hThread);
	hThread = NULL;

	_tprintf_s(_T("**************************************************************************\r\n\r\n"));
	
	/*
	Debug Information:
	RecursionCount(itself++)
	Current Thread Id: 4060 Owning Thread Id: 4060  LockCount: -2   RecursionCount:1
	Current Thread Id: 4060 Owning Thread Id: 4060  LockCount: -2   RecursionCount:2
	Current Thread Id: 4060 Owning Thread Id: 4060  LockCount: -2   RecursionCount:3
	Current Thread Id: 4060 Owning Thread Id: 4060  LockCount: -2   RecursionCount:4
	Current Thread Id: 4060 Owning Thread Id: 4060  LockCount: -2   RecursionCount:3
	Current Thread Id: 4060 Owning Thread Id: 4060  LockCount: -2   RecursionCount:2
	Current Thread Id: 4060 Owning Thread Id: 4060  LockCount: -2   RecursionCount:1
	Current Thread Id: 4060 Owning Thread Id: 0     LockCount: -1   RecursionCount:0
	*/
	return;
}
DWORD WINAPI ThreadProc2_1(LPVOID lpParam)
{
	EnterCriticalSection(&gCS);

	_tprintf_s(_T("Current Thread Id: %d\tOwning Thread Id: %d\tLockCount: %d\tRecursionCount:%d\r\n"),
		GetCurrentThreadId(), (DWORD)gCS.OwningThread, gCS.LockCount, gCS.RecursionCount);

	if (gRecursionCount == 3)
	{
		LeaveCriticalSection(&gCS);
		_tprintf_s(_T("Current Thread Id: %d\tOwning Thread Id: %d\tLockCount: %d\tRecursionCount:%d\r\n"),
			GetCurrentThreadId(), (DWORD)gCS.OwningThread, gCS.LockCount, gCS.RecursionCount);
		return 0;
	}
	gRecursionCount++;
	ThreadProc2_1(lpParam);

	LeaveCriticalSection(&gCS);
	_tprintf_s(_T("Current Thread Id: %d\tOwning Thread Id: %d\tLockCount: %d\tRecursionCount:%d\r\n"),
		GetCurrentThreadId(), (DWORD)gCS.OwningThread, gCS.LockCount, gCS.RecursionCount);

	return 0;
}


// 3. RecursionCount can be modify by any threads
// Thread1 Enter ++
// Thread2 Leave --
VOID Sub_3()
{
	_tprintf_s(_T("**************************************************************************\r\n"));
	_tprintf_s(_T("Recursion can be modify by any threads\r\n"));
	int i = 0;
	HANDLE hThread[2];
	ZeroMemory(hThread, sizeof(hThread));
	InitializeCriticalSection(&gCS);

	hThread[0] = CreateThread(
					NULL, 0, 
					(LPTHREAD_START_ROUTINE)ThreadProc3_1, 
					NULL, 0, NULL);
	Sleep(1000);
	hThread[1] = CreateThread(
					NULL, 0, 
					(LPTHREAD_START_ROUTINE)ThreadProc3_2, 
					NULL, 0, NULL);

	WaitForMultipleObjects(2, hThread, TRUE, INFINITE);

	DeleteCriticalSection(&gCS);

	for (i = 0; i < 2; i++)
	{
		CloseHandle(hThread[i]);
		hThread[i] = NULL;
	}
	_tprintf_s(_T("**************************************************************************\r\n\r\n"));
	/*
	!! Pay attion!! Thread Id would not be change by LeaveCriticalSection
	Debug Information:
	Recursion can be modify by any threads
	Current Thread Id: 5512 Owning Thread Id: 5512  LockCount: -2   RecursionCount:1
	Current Thread Id: 5512 Owning Thread Id: 5512  LockCount: -2   RecursionCount:2
	Current Thread Id: 5512 Owning Thread Id: 5512  LockCount: -2   RecursionCount:3
	Current Thread Id: 5512 Owning Thread Id: 5512  LockCount: -2   RecursionCount:4
	Current Thread Id: 2936 Owning Thread Id: 5512  LockCount: -2   RecursionCount:3
	Current Thread Id: 2936 Owning Thread Id: 5512  LockCount: -2   RecursionCount:2
	Current Thread Id: 2936 Owning Thread Id: 5512  LockCount: -2   RecursionCount:1
	Current Thread Id: 2936 Owning Thread Id: 0     LockCount: -1   RecursionCount:0
	*/
}
DWORD WINAPI ThreadProc3_1(LPVOID lpParam)
{
	EnterCriticalSection(&gCS);
	_tprintf_s(_T("Current Thread Id: %d\tOwning Thread Id: %d\tLockCount: %d\tRecursionCount:%d\r\n"),
		GetCurrentThreadId(), (DWORD)gCS.OwningThread, gCS.LockCount, gCS.RecursionCount);
	if (gRecursionCount == 3)
	{
		return 0;
	}
	else
	{
		gRecursionCount++;
		ThreadProc3_1(lpParam);
	}

	return 0;
}
DWORD WINAPI ThreadProc3_2(LPVOID lpParam)
{
	if (gRecursionCount == 0)
	{
		LeaveCriticalSection(&gCS);
		_tprintf_s(_T("Current Thread Id: %d\tOwning Thread Id: %d\tLockCount: %d\tRecursionCount:%d\r\n"),
			GetCurrentThreadId(), (DWORD)gCS.OwningThread, gCS.LockCount, gCS.RecursionCount);
		return 0;
	}
	else
	{
		gRecursionCount--;
		ThreadProc3_2(lpParam);
	}

	LeaveCriticalSection(&gCS);
	_tprintf_s(_T("Current Thread Id: %d\tOwning Thread Id: %d\tLockCount: %d\tRecursionCount:%d\r\n"),
		GetCurrentThreadId(), (DWORD)gCS.OwningThread, gCS.LockCount, gCS.RecursionCount); 
	return 0;
}

// 4. The Thread will be at in wait state, and wake up at a proper time
VOID Sub_4()
{
	_tprintf_s(_T("**************************************************************************\r\n"));
	_tprintf_s(_T("The Thread will be at in wait state, and wake up at a proper time\r\n"));
	int i = 0;
	HANDLE hThread[3];
	ZeroMemory(hThread, sizeof(hThread));
	InitializeCriticalSection(&gCS);

	for (i = 0; i < 3; i++)
	{
		hThread[i] = CreateThread(
						NULL, 0, 
						(LPTHREAD_START_ROUTINE)ThreadProc4_1, 
						NULL, 0, NULL);
	}
	WaitForMultipleObjects(3, hThread, TRUE, INFINITE);

	for (i = 0; i < 3; i++)
	{
		CloseHandle(hThread[i]);
		hThread[i] = NULL;
	}

	DeleteCriticalSection(&gCS);
	_tprintf_s(_T("**************************************************************************\r\n\r\n"));
	/*
	Debug Information:
	Current Thread Id: 19736    Owning Thread Id: 19736 LockCount: -2   RecursionCount:1    WaitCount: 10000
	Current Thread Id: 10020    Owning Thread Id: 10020 LockCount: -2   RecursionCount:1    WaitCount: 20000
	Current Thread Id: 19440    Owning Thread Id: 19440 LockCount: -2   RecursionCount:1    WaitCount: 30000
	*/
}
DWORD WINAPI ThreadProc4_1(LPVOID lpParam)
{
	int i = 0;
	EnterCriticalSection(&gCS);

	for (i = 0; i < 10000; i++)
	{
		gWaitCount++;
	}
	_tprintf_s(_T("Current Thread Id: %d\tOwning Thread Id: %d\tLockCount: %d\tRecursionCount:%d\tWaitCount: %d\r\n"),
		GetCurrentThreadId(), (DWORD)gCS.OwningThread, gCS.LockCount, gCS.RecursionCount, gWaitCount);
	LeaveCriticalSection(&gCS);
	return 0;
}

// 5. CriticalSetion and SpinLock
VOID Sub_5()
{
	_tprintf_s(_T("**************************************************************************\r\n"));
	_tprintf_s(_T("CriticalSetion and SpinLock\r\n"));
	int i = 0;
	HANDLE hThread[3];
	ZeroMemory(hThread, sizeof(hThread));
	if (!InitializeCriticalSectionAndSpinCount(&gCS, 0x4000))    //0x4000 Parameter Scale [0，0x00FFFFFF]
	{
		return;
	}

	for (i = 0; i < 3; i++)
	{
		hThread[i] = CreateThread(
							NULL, 0, 
							(LPTHREAD_START_ROUTINE)ThreadProc5_1, 
							NULL, 0, NULL);
	}

	WaitForMultipleObjects(3, hThread, TRUE, INFINITE);

	DeleteCriticalSection(&gCS);

	for (i = 0; i < 3; i++)
	{
		CloseHandle(hThread[i]);
		hThread[i] = NULL;
	}
	_tprintf_s(_T("**************************************************************************\r\n\r\n"));
	/*
	Pay attion!! SpinLock can't enter resursivily
	Debug Information:
	Current Thread Id: 10760    Owning Thread Id: 10760		LockCount: -2   RecursionCount:1
	Current Thread Id: 10760    Owning Thread Id: 10760		LockCount: -10  RecursionCount:1
	Current Thread Id: 15088    Owning Thread Id: 15088		LockCount: -6   RecursionCount:1
	Current Thread Id: 15088    Owning Thread Id: 15088		LockCount: -6   RecursionCount:1
	Current Thread Id: 10044    Owning Thread Id: 10044		LockCount: -2   RecursionCount:1
	Current Thread Id: 10044    Owning Thread Id: 10044		LockCount: -2   RecursionCount:1
	*/

}
DWORD WINAPI ThreadProc5_1(LPVOID lpParam)
{
	int i = 0;
	EnterCriticalSection(&gCS);
	for (i = 0; i < 10000; i++)
	{
		if (i % 5000 == 0)
		{
			_tprintf_s(_T("Current Thread Id: %d\tOwning Thread Id: %d\tLockCount: %d\tRecursionCount:%d\r\n"),
				GetCurrentThreadId(), (DWORD)gCS.OwningThread, gCS.LockCount, gCS.RecursionCount);
		}
	}
	LeaveCriticalSection(&gCS);

	return 0;
}

