#pragma once
#include <windows.h>
#include <iostream>
#include <tchar.h>
using namespace std;


BOOL SynNamedEvent();
BOOL SynNamedMutex();
BOOL SynNamedSemaphore();
BOOL SynDupSemaphore();

BOOL   CloseHandleEx(HANDLE _hObject);
BOOL   EnableSeDebugPrivilege(HANDLE _hProcess, BOOL _IsEnable);
HANDLE OpenProcessEx(DWORD _DesiredAccess, BOOL _IsInheritHandle, DWORD _ProcessId);
