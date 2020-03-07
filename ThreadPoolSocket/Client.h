#pragma once

#include "winsock2.h"  
#pragma comment(lib, "ws2_32.lib")


typedef struct _CLIENT_DATA
{
	DWORD id;
	char buf[MAX_PATH];
}CLIENT_DATA, *PCLIENT_DATA;

DWORD WINAPI ThreadProc(LPVOID lpParam);

BOOL  WINAPI ConsoleHandlerRoutine(DWORD CtrlType);
