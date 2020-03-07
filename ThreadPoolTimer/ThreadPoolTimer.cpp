#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>

#define ID_MSGBOX_STATIC_TEXT 0x0000ffff

static TCHAR szWindowName[100] { _T("Timed Message Box") };

int g_elapse = 0;

VOID WINAPI TimeoutCallback(
	_Inout_     PTP_CALLBACK_INSTANCE Instance,
	_Inout_opt_ PVOID                 Context,
	_Inout_     PTP_TIMER             Timer);

int _tmain(int argc, LPCTSTR argv[], LPCTSTR envp[])
{

	g_elapse = 10;

	//inform the thread pool when to invoke your function 
	PTP_TIMER ptpTimer = 
		CreateThreadpoolTimer(
			reinterpret_cast<PTP_TIMER_CALLBACK>(TimeoutCallback), 
			NULL, NULL);


	if (ptpTimer == NULL) {
		TCHAR szMsg[MAX_PATH];
		StringCchPrintf(
			szMsg, _countof(szMsg), 
			_T("Err: CreateThreadpoolTimer  ( %d )"), GetLastError());

		MessageBox(NULL, szMsg, _T("Error"), MB_OK | MB_ICONERROR);

		return 0;
	}


	LARGE_INTEGER liTime;

	liTime.QuadPart = 10000000;

	FILETIME fileTime;
	fileTime.dwHighDateTime = liTime.HighPart;
	fileTime.dwLowDateTime  = liTime.LowPart;

	// register the timer with the thread pool
	SetThreadpoolTimer(ptpTimer, &fileTime, 1000, 0);

	MessageBox(
		NULL, _T("you have 10 Seconds to respond"),
		szWindowName, MB_OK);

	CloseThreadpoolTimer(ptpTimer);

	MessageBox(
		NULL, 
		(g_elapse == 1) ? _T("Timeout") : _T("User responded"), 
		_T("Result"), MB_OK);

	return 0;
}


VOID WINAPI TimeoutCallback(
	_Inout_     PTP_CALLBACK_INSTANCE Instance,
	_Inout_opt_ PVOID                 Context,
	_Inout_     PTP_TIMER             Timer)
{
	HWND hWnd = FindWindow(NULL, szWindowName);

	if (hWnd != NULL) {
	
		if (g_elapse == 1) {
			
			EndDialog(hWnd, IDOK);
			return;
		}
		
		TCHAR szMsg[100] { 0 };
		StringCchPrintf(
			szMsg, _countof(szMsg), 
			_T("You have %d seconds to respond"), 
			--g_elapse);
		SetDlgItemText(hWnd, ID_MSGBOX_STATIC_TEXT, szMsg);
	}
	else {
		
	}
}
