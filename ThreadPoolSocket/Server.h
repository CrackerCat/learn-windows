#pragma once

#include <winsock2.h>  
#pragma comment(lib, "ws2_32.lib")

typedef struct _EX_FD_SET
{
	fd_set fdset; // has an array of SOCKETs
	BOOL   flag;
	BOOL   fExit;
}EX_FD_SET, *PEX_FD_SET;

typedef struct _CLIENT_DATA
{
	DWORD id;
	char buf[MAX_PATH];
}CLIENT_DATA, *PCLIENT_DATA;

DWORD WINAPI ThreadProc_1(LPVOID lpParam);
VOID  WINAPI ThreadProc_2(PTP_CALLBACK_INSTANCE CallBackInstance, PVOID lpParam, PTP_WORK Work);

BOOL  WINAPI ConsoleHandlerRoutine(DWORD CtrlType);
