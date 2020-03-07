#include "server.h"
#include <cstdio>
#include <tchar.h>
#include <vector>


SOCKET *g_sListen = NULL;

int _tmain(int argc, LPCTSTR argv[], LPCTSTR envp[])
{
	WSADATA         wsaData;			// version of winsock
	SOCKET          sListen;			// listen socket
	SOCKET          sClient;			// client socket
	SOCKADDR_IN     addrServer;			// server address
	SOCKADDR_IN		addrClient;
	int             rVal = 0;

	if (FALSE == SetConsoleCtrlHandler(ConsoleHandlerRoutine, TRUE)) {

		return -1;
	}

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {

		_tprintf(_T("Err: WSAStartup() ( %d )\r\n"), GetLastError());
		return -1;
	}
	// create listen socket
	sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	g_sListen = &sListen;

	if (INVALID_SOCKET == sListen) {

		_tprintf(_T("Err: socket() ( %d )\r\n"), GetLastError());
		WSACleanup();
		return -1;
	}

	// initialize server address
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(2356);
	addrServer.sin_addr.s_addr = INADDR_ANY;

	// bind with socket
	rVal = bind(sListen, (LPSOCKADDR)&addrServer, sizeof(SOCKADDR_IN));

	if (SOCKET_ERROR == rVal) {

		_tprintf(_T("Err: bind() ( %d )\r\n"), GetLastError());
		closesocket(sListen);
		WSACleanup();
		return -1;
	}
	// start to listen port
	rVal = listen(sListen, 10);
	if (SOCKET_ERROR == rVal) {

		_tprintf(_T("Err: listen() ( %d )\r\n"), GetLastError());
		closesocket(sListen);
		WSACleanup();
		return -1;
	}

	
	int addrSize = sizeof(SOCKADDR_IN);

	std::vector<PEX_FD_SET> pfdSetVector;
	std::vector<PTP_WORK>   ptpWorkVector;

	PEX_FD_SET listenSet =  new EX_FD_SET;

	if (listenSet == NULL) {

		closesocket(sListen);
		WSACleanup();
		return -1;
	}

	FD_ZERO(&listenSet->fdset);

	listenSet->flag = FALSE;
	listenSet->fExit = FALSE;
	pfdSetVector.push_back(listenSet);

	PEX_FD_SET clientSet = NULL;
	while (TRUE) {
		sClient = accept(
			sListen, 
			reinterpret_cast<SOCKADDR*> (&addrClient), 
			&addrSize);

		if (sListen == INVALID_SOCKET){

			for (size_t i = 0; i < pfdSetVector.size(); ++i) {

				pfdSetVector[i]->fExit = TRUE;
				if (pfdSetVector[i]->fdset.fd_count != 0) {

					for (size_t j = 0; j < pfdSetVector[i]->fdset.fd_count + 1; ++j) {
						
						closesocket(pfdSetVector[i]->fdset.fd_array[j]);
						pfdSetVector[i]->fdset.fd_array[j] = NULL;
					}
					pfdSetVector[i]->fdset.fd_count = 0;
					
					WaitForThreadpoolWorkCallbacks(ptpWorkVector[i], TRUE);
				}
			}
			break;
		}

		 
		for (auto i : pfdSetVector) {

			if (i->fdset.fd_count < FD_SETSIZE) {

				if (i->fdset.fd_count == 0) {

					i->flag = FALSE;
				}
				clientSet = i;
				break;
			}
			else {
				
				clientSet = new EX_FD_SET;
				FD_ZERO(&clientSet->fdset);
				clientSet->flag = FALSE;
				clientSet->fExit = FALSE;
				pfdSetVector.push_back(clientSet);
				break;
			}
		}

		FD_SET(sClient, &clientSet->fdset);
		if (clientSet->flag == FALSE) {

			clientSet->flag = TRUE;


			// create work item
			PTP_WORK PtpWork = CreateThreadpoolWork(ThreadProc_2, (PVOID)clientSet, NULL);
			// submit the item
			SubmitThreadpoolWork(PtpWork);
			ptpWorkVector.push_back(PtpWork);


			/*
				PTP_POOL ThreadPool;
				TP_CALLBACK_ENVIRON ThreadCalllBackEnvironment;
				// create thread pool
				ThreadPool = CreateThreadpool(NULL);
				// set maxium & minium
				SetThreadpoolThreadMinimum(ThreadPool, 4);
				SetThreadpoolThreadMaximum(ThreadPool, 500);
				// init envp
				InitializeThreadpoolEnvironment(&ThreadCalllBackEnvironment);
				// set envp
				SetThreadpoolCallbackPool(&ThreadCalllBackEnvironment, ThreadPool);
				// use private thread pool
				TrySubmitThreadpoolCallback(ThreadCallBack, (LPVOID)&(Temp->SocketArray), &ThreadCalllBackEnvironment);
				// clean envp
				DestroyThreadpoolEnvironment(&ThreadCalllBackEnvironment);
				// close thread pool
				CloseThreadpool(ThreadPool);
			*/

		}
	}

	for (auto i : pfdSetVector) {
		
		delete i;
	}

	for (auto i : ptpWorkVector) {
		
		CloseThreadpoolWork(i);
	}


	std::vector<PEX_FD_SET>().swap(pfdSetVector);
	std::vector<PTP_WORK>().swap(ptpWorkVector);
	_tprintf(_T("Server shut down...\r\n"));

	Sleep(3000);
	WSACleanup();
	return 0;
}

DWORD WINAPI ThreadProc_1(LPVOID lpParam)
{
	CLIENT_DATA      ClientData { 0 };
	int              rVal = 0;
	timeval timeout { 1, 0 };
	PEX_FD_SET clientSet = (PEX_FD_SET)lpParam;

	while (TRUE) {
		// FdSet->IsExit = TRUE | Server shut down, thread exit
		if (clientSet->fdset.fd_count == 0 || clientSet->fExit == TRUE) {

			break;
		}

		fd_set newSet = clientSet->fdset;

		rVal = select(0, &newSet, NULL, NULL, &timeout);

		if (rVal == 0) {
			// nothing happened
			continue;
		}
		if (rVal > 0) {

			// socket set changes

			// traverse the old socket set 
			for (u_int i = 0; i < clientSet->fdset.fd_count; i++) {

				// singled, if in new socket set after selecting.
				if (FD_ISSET(clientSet->fdset.fd_array[i], &newSet)) {

					rVal = recv(
						clientSet->fdset.fd_array[i], 
						reinterpret_cast<char*>(&ClientData), 
						sizeof(CLIENT_DATA), 0);

					if (rVal > 0) {
						// print message
						_tprintf(_T("client: %d | data: %s\r\n"), ClientData.id, ClientData.buf);
						break;
					}
					else {

						// return value < 0, client closes socket.
						closesocket(clientSet->fdset.fd_array[i]);
						clientSet->fdset.fd_array[i] = NULL;
						// remove from the socket set
						FD_CLR(clientSet->fdset.fd_array[i], &(clientSet->fdset));

						_tprintf(_T("one client shut down...\r\n"));
						break;
					}
				}
			}
		}
		else {

			_tprintf(_T("Err: select() ( %d )\r\n"), GetLastError());
			break;
		}
	}
	return 0;

}

void WINAPI ThreadProc_2(PTP_CALLBACK_INSTANCE CallBackInstance, PVOID lpParam, PTP_WORK Work)
{
	CLIENT_DATA      ClientData { 0 };
	int              rVal = 0;
	timeval TimeWait { 1, 0 };
	PEX_FD_SET oldSet = (PEX_FD_SET)lpParam;
	while (TRUE)
	{
		// fdset->fExit = TRUE, Server exit..
		if (oldSet->fdset.fd_count == 0 || oldSet->fExit == TRUE) {

			break;
		}

		fd_set newSet = oldSet->fdset;

		rVal = select(0, &newSet, NULL, NULL, &TimeWait);

		if (rVal == 0) {
			// nothing happened
			continue;
		}
		if (rVal > 0) {

			// traverse the old socket set 
			for (int i = 0; i < (int)oldSet->fdset.fd_count; i++)
			{
				// singled, if in new socket set after selecting.
				if (FD_ISSET(oldSet->fdset.fd_array[i], &newSet))
				{

					rVal = recv((SOCKET)oldSet->fdset.fd_array[i], (char*)&ClientData, sizeof(CLIENT_DATA), 0);
					if (rVal > 0) {

						_tprintf(_T("client: %d | data: %s\r\n"), ClientData.id, ClientData.buf);
						break;
					}
					else {
						// server closes sListen, and all thread exits
						if (oldSet->fExit == TRUE) {
							break;
						}
						// return value < 0, client closes socket.
						closesocket(oldSet->fdset.fd_array[i]);
						oldSet->fdset.fd_array[i] = NULL;
						// remove from socket set
						FD_CLR(oldSet->fdset.fd_array[i], &(oldSet->fdset));

						_tprintf(_T("client shut down\r\n"));
						break;
					}
				}
			}
		}
		else {

			_tprintf(_T("Failed Select\r\n"));
			break;
		}
	}

}

BOOL WINAPI ConsoleHandlerRoutine(DWORD CtrlType)
{
	switch (CtrlType)
	{
	case CTRL_C_EVENT:
		break;
	case CTRL_BREAK_EVENT:
		break;
	case CTRL_CLOSE_EVENT:
		if (INVALID_SOCKET != *g_sListen) {
			
			closesocket(*g_sListen);
			*g_sListen = INVALID_SOCKET;
			Sleep(3000);
		}
		break;
	case CTRL_LOGOFF_EVENT:
		break;
	case CTRL_SHUTDOWN_EVENT:
		break;
	}
	return TRUE;
}
