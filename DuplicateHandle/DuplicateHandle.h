#pragma once
#ifndef _DUPLICATE_HANDLE_H
#define _DUPLICATE_HANDLE_H


#include <cstdio>
#include <tchar.h>
#include <windows.h>


VOID   DupFileFromDelete();
BOOL   CloseHandleEx(HANDLE hObject);
BOOL   EnableSeDebugPrivilege(HANDLE hProcess, BOOL IsEnable);
HANDLE OpenProcessEx(DWORD DesiredAccess, BOOL IsInheritHandle, DWORD ProcessId);





#endif
