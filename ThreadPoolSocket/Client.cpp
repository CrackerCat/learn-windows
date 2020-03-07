#include "Client.h"
#include <cstdio>
#include <tchar.h>

SOCKET* g_sClient = NULL;

int _tmain(int argc, LPCTSTR argv[], LPCTSTR envp[])
{
	WSADATA wsaData;
	SOCKET  sClient;
	SOCKADDR_IN addrServer;
	char buff[MAX_PATH];
	int  rVal = 0;
	if (SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandlerRoutine, TRUE) == FALSE) {

		return -1;
	}

	// start up socket library
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {

		_tprintf(_T("Err: WSAStartup() ( %d )\r\n"), GetLastError());
		return -1;
	}
	
	// create client socket
	sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == sClient) {

		_tprintf(_T("Err: socket() ( %d )\r\n"), GetLastError());
		WSACleanup();
		return  -1;
	}
	g_sClient = &sClient;

	// set server address
	addrServer.sin_family = AF_INET;
#pragma warning(disable : 4996)
	addrServer.sin_addr.s_addr = inet_addr("127.0.0.1");
	addrServer.sin_port = htons(static_cast<u_short>(2356));

	// connect to server
	rVal = connect(sClient, (LPSOCKADDR)&addrServer, sizeof(addrServer));

	if (SOCKET_ERROR == rVal) {

		_tprintf(_T("Err: connect() ( %d )\r\n"), GetLastError());
		closesocket(sClient);
		WSACleanup();
		return -1;
	}
	

	DWORD clientId = GetCurrentProcessId();
	_tprintf(_T("ClientID: %d\r\n"), clientId);

	HANDLE hThread = CreateThread(
		0, 0, 
		ThreadProc, 
		reinterpret_cast<LPVOID*>(sClient), 
		0, 0);

	// client main thread stop here
	rVal = recv(sClient, buff, sizeof(buff), 0);

	if (rVal == -1) {

		TerminateThread(hThread, 0);
		if (hThread != NULL) {
			
			CloseHandle(hThread);
			hThread = NULL;
		}
		WSACleanup();
		
		_tprintf(_T("Client shut down...\r\n"));
		Sleep(3000);
		return 0;
	}
	else {
		TerminateThread(hThread, 0);
		if (hThread != NULL) {
			CloseHandle(hThread);
			hThread = NULL;
		}
		closesocket(sClient);
		WSACleanup();
		
		_tprintf(_T("Server offline, client shuts in 3 seconds"));
		Sleep(3000);
		return 0;
	}

	WSACleanup();
	return 0;
}

DWORD WINAPI ThreadProc(LPVOID lpParam)
{
	char   buff[MAX_PATH];
	DWORD  ClientID = GetCurrentProcessId();
	SOCKET sClient = reinterpret_cast<SOCKET>(lpParam);


	while (TRUE) {

		CLIENT_DATA clientData { 0 };

		memset(buff, 0, sizeof(buff));
		_tprintf(_T("Send Message: "));
#pragma warning(disable : 4996)
		_tscanf(_T("%s"), &buff);
		clientData.id = ClientID;
		memcpy(clientData.buf, buff, sizeof(char)*MAX_PATH);
		int rVal 
			= send( sClient, 
					reinterpret_cast<char*>(&clientData), 
					sizeof(CLIENT_DATA), 0);

		if (SOCKET_ERROR == rVal) {
			closesocket(sClient);
			WSACleanup();
			return -1;
		}
	}
	return 0;
}

BOOL  WINAPI ConsoleHandlerRoutine(DWORD CtrlType)
{
	switch (CtrlType)
	{
	case CTRL_C_EVENT:
		break;
	case CTRL_BREAK_EVENT:
		break;
	case CTRL_CLOSE_EVENT:
		if (INVALID_SOCKET != *g_sClient) {

			closesocket(*g_sClient);
			*g_sClient = INVALID_SOCKET;
			Sleep(5000);
		}
		break;
	case CTRL_LOGOFF_EVENT:
		break;
	case CTRL_SHUTDOWN_EVENT:
		break;
	}
	return TRUE;
}
