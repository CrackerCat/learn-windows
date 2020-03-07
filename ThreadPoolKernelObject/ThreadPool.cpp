#include <Windows.h>
#include <iostream>
#include <tchar.h>

VOID CALLBACK WaitCallBack(
		PTP_CALLBACK_INSTANCE pInstance, 
		PVOID			Context, 
		PTP_WAIT		Wait,
		TP_WAIT_RESULT	WaitResult);

int _tmain(int argc, LPTSTR argv[], LPTSTR envp[])
{
	// create a thread pool wait object by calling CreateThreadpoolWait
	// register a work item to be executed when a kernel object is signaled
	PTP_WAIT ptpWait;
	ptpWait = CreateThreadpoolWait((PTP_WAIT_CALLBACK)WaitCallBack, NULL, NULL);
	
	HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	
	// bind a kernel object to this thread pool wait pool wait object
	SetThreadpoolWait(ptpWait, hEvent, NULL);
	SetEvent(hEvent); // call callback function
	
	Sleep(1000);
	SetThreadpoolWait(ptpWait, hEvent, NULL); // register again

	SetEvent(hEvent);
	WaitForThreadpoolWaitCallbacks(ptpWait, FALSE);

//	Sleep(1000);
	// remove kernel object
	SetThreadpoolWait(ptpWait, NULL, NULL);

	return 0;
}

VOID CALLBACK WaitCallBack(
		PTP_CALLBACK_INSTANCE pInstance, 
		PVOID Context, 
		PTP_WAIT Wait,
		TP_WAIT_RESULT WaitResult){

	_tprintf(_T("WaitCallBack()\r\n"));
	Sleep(5000);
}
