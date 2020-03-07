#include "JobObject.h"

int _tmain(int argc, TCHAR *argv[], TCHAR *envp[])
{
	// setlocale(LC_ALL, "chs");
	RestrictProcess();

	_gettchar();
	return 0;
}

void RestrictProcess()
{
	HANDLE JobHandle = NULL;
	DWORD  ErrCode   = ERROR_SUCCESS;
	BOOL   RetVal    = FALSE;
	STARTUPINFO StartupInfo;
	PROCESS_INFORMATION ProcessInfo;
	DWORD  CreateFlags = NULL;
	
	HANDLE hIOCP = NULL;
	JOBOBJECT_ASSOCIATE_COMPLETION_PORT JobObjectAssociateCompletionPort;

	HANDLE hThread = NULL;

	TCHAR CommandLine[] = _T("SubProcess.exe");

	ZeroMemory(&StartupInfo, sizeof(StartupInfo));
	ZeroMemory(&ProcessInfo, sizeof(ProcessInfo));
	ZeroMemory(&JobObjectAssociateCompletionPort,
		sizeof(JobObjectAssociateCompletionPort));

	// Create a job object
	JobHandle =  CreateJobObject(NULL, NULL);							

	if (JobHandle == NULL)
	{
		goto Clean;
	}

	StartupInfo.cb = sizeof(STARTUPINFO);
#ifdef UNICODE
	CreateFlags = CREATE_UNICODE_ENVIRONMENT;
#endif
	
	// Create son process
	RetVal = CreateProcess(
		NULL,
		CommandLine,
		NULL,
		NULL,
		FALSE,
		CreateFlags | CREATE_NEW_CONSOLE,
		NULL,
		NULL,
		&StartupInfo,
		&ProcessInfo);
	if (RetVal == FALSE)
	{
		ErrCode = GetLastError();
		goto Clean;
	}

	// Accociate job object to process
	RetVal = AssignProcessToJobObject(JobHandle, ProcessInfo.hProcess);
	if (RetVal == FALSE)
	{
		ErrCode = GetLastError();
		goto Clean;
	}
	
	// Job Notify
	// Create IO completion port 
	hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	if (hIOCP == NULL)
	{
		ErrCode = GetLastError();
		goto Clean;
	}
	
	// associate Completion Port to job object
	JobObjectAssociateCompletionPort.CompletionKey = (PVOID)JOB_NOTIFY;	// Set Completion Key
	JobObjectAssociateCompletionPort.CompletionPort = hIOCP;
	RetVal = 
		SetInformationJobObject(
			JobHandle,
			JobObjectAssociateCompletionPortInformation,
			&JobObjectAssociateCompletionPort,
			sizeof(JobObjectAssociateCompletionPort));
	if (RetVal == FALSE)
	{
		ErrCode = GetLastError();
		goto Clean;
	}

	// Create a thread to handle IO Completion Port Message 
	hThread = CreateThread(
		NULL, 
		0, 
		(LPTHREAD_START_ROUTINE)ThreadProc,
		hIOCP, 
		0, 
		NULL);
	if (hThread == NULL)
	{
		ErrCode = GetLastError();
		goto Clean;
	}
	WaitForSingleObject(hThread, INFINITE);

Clean:
	if (JobHandle != NULL)
	{
		CloseHandle(JobHandle);
		JobHandle = NULL;
	}
	if (ProcessInfo.hProcess != NULL)
	{
		CloseHandle(ProcessInfo.hProcess);
		ProcessInfo.hProcess = NULL;
	}
	if (ProcessInfo.hThread != NULL)
	{
		CloseHandle(ProcessInfo.hThread);
		ProcessInfo.hThread = NULL;
	}
	if (hIOCP == NULL)
	{
		CloseHandle(hIOCP);
		hIOCP = NULL;
	}
	if (hThread == NULL)
	{
		CloseHandle(hThread);
		hThread = NULL;
	}
}

DWORD WINAPI ThreadProc(LPVOID lpParam)
{
	DWORD  NumberOfBytesTransferred = 0;
	ULONG_PTR    CompletionKey      = NULL;
	LPOVERLAPPED Overlapped         = NULL;
	BOOL RetVal = FALSE;
	BOOL IsWork = TRUE;

	while (IsWork)
	{

		RetVal =
			GetQueuedCompletionStatus(
				lpParam,
				&NumberOfBytesTransferred, 
				&CompletionKey,         
				&Overlapped,
				INFINITE);
		if (RetVal == TRUE)
		{
			if (CompletionKey == JOB_NOTIFY)
			{
				switch (NumberOfBytesTransferred)
				{
					/*
					事件类型：
					JOB_OBJECT_MSG_ABNORMAL_EXIT_PROCESS       进程异常退出
					JOB_OBJECT_MSG_ACTIVE_PROCESS_LIMIT        同时活动的进程数达到设置的上限
					JOB_OBJECT_MSG_ACTIVE_PROCESS_ZERO         作业对象中没有活动的进程了
					JOB_OBJECT_MSG_END_OF_JOB_TIME             作业对象的CPU周期耗尽
					JOB_OBJECT_MSG_END_OF_PROCESS_TIME         进程的CPU周期耗尽
					JOB_OBJECT_MSG_EXIT_PROCESS                进程正常退出
					JOB_OBJECT_MSG_JOB_MEMORY_LIMIT            作业对象消耗内存达到上限
					JOB_OBJECT_MSG_NEW_PROCESS                 有新进程加入到作业对象中
					JOB_OBJECT_MSG_PROCESS_MEMORY_LIMIT        进程消耗内存数达到上限
					*/
				case JOB_OBJECT_MSG_EXIT_PROCESS:
					_tprintf_s(_T("SubProcessExit\r\n"));
					IsWork = FALSE;
					break;
				default:
					break;
				}
			}
			else
			{
				_tprintf_s(_T("Associate wrong Object to CompletionPort \r\n"));
				break;
			}
		}
	}
	_tprintf_s(_T("ThreadProc() Exit\r\n"));
	return TRUE;
}
