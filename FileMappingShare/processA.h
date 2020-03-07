#pragma once
#include <windows.h>
#include <tchar.h>
#include <iostream>


using namespace std;


#define ACCESS_READ  0
#define ACCESS_WRITE 1
#define ACCESS_ALL   2

BOOL FileMappingShare();

BOOL CreateMemroyMappingEx(
	_In_  DWORD      ReadOrWrite, 
	_In_  DWORD      MaximumSizeHigh,
	_In_  DWORD      MaximumSizeLow, 
	_In_  LPCTSTR    ObjectName,
	_Out_ LPHANDLE   MappingHandle, 
	_Out_ LPVOID*    BaseAddress
);

BOOL UnmapMemoryEx(
	_In_ HANDLE  hMappingHandle, 
	_In_ LPCVOID BaseAddress
);

BOOL CloseHandleEx(
	_In_ HANDLE hObject
);
