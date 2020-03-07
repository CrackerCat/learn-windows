#pragma once
#include<iostream>
#include<windows.h>
#include<tchar.h>
#include<winnt.h>
#include<strsafe.h>


enum {
	OPTION_READ	= 0x1,	// [op] read
	OPTION_WRITE		// [op] write
};



/*
#define CONTAINING_RECORD(address, type, field) ((type FAR *)( \
										  (PCHAR)(address) - \
										  (PCHAR)(&((type *)0)->field)))*/


typedef struct _USER_DATA_
{
	OVERLAPPED ol;
	HANDLE     hFile;
	DWORD	   dwOption;
	LPVOID	   lpBuff;
	SIZE_T	   cbSize;
	DWORD      dwBytesToWritten;
	DWORD	   dwTimestamp;

	_USER_DATA_() {

		memset(this, 0, sizeof(*this));
	}

}USER_DATA, *PUSER_DATA;



VOID 
CALLBACK 
OverlappedCompetionRoutine(
	PTP_CALLBACK_INSTANCE Instance, 
	PVOID pvContext, 
	PVOID pOverlapped, 		
	ULONG IoResult, 
	ULONG_PTR NumberOfBytesTransferred, 
	PTP_IO pIo
);
DWORD WINAPI ThreadProc(LPVOID lpParam);

BOOL CtsCloseHandle(HANDLE hObject);
