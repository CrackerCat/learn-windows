#pragma once

#include <cstdio>
#include <tchar.h>
#include <windows.h>

#define MAX 3
void sub_1();
void sub_2();
DWORD WINAPI ThreadProc(LPVOID lpvParam);
BOOL WINAPI ConsoleHandlerRoutine(DWORD dwEventID);


VOID WINAPI CompletionRoutine(
	DWORD dwErrCode,
	DWORD dwWrittenBytes,
	LPOVERLAPPED lpOL);

typedef struct _USER_DATA_
{
	OVERLAPPED ol;
	DWORD dwTimeStamp;
}USER_DATA, *PUSER_DATA;

struct _PACKET_
{
	_PACKET_() {
		m_cbSize = MAX;
		m_hEvents[0] = CreateEvent(NULL, FALSE, FALSE, NULL);
		for (size_t i = 1; i < m_cbSize + 1; i++)  // 1 2 3
		{
			m_UserData[i - 1].ol.hEvent = 
				m_hEvents[i] = 
					CreateEvent(NULL, FALSE, FALSE, NULL);
			m_UserData[i - 1].ol.Offset = -1;
			m_UserData[i - 1].ol.OffsetHigh = -1;
		}
	}
	
	~_PACKET_()
	{
		CloseHandle(m_hFile);
		m_hFile = NULL;
		for (size_t i = 0; i < m_cbSize + 1; i++)
		{
			CloseHandle(m_hEvents[i]);
			m_hEvents[i] = NULL;
		}
		memset(m_UserData, sizeof(m_UserData), 0);
		
	}
	
	USER_DATA m_UserData[MAX];
	SIZE_T    m_cbSize;
	HANDLE    m_hEvents[MAX + 1];
	HANDLE    m_hFile;
};
