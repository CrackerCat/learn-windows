#pragma once

#include <cstdio>
#include <tchar.h>
#include <Windows.h>

VOID  ReaderPrintf(LPCTSTR Format, ...);
VOID  WriterPrintf(LPCTSTR String);

DWORD ReaderThreadProc(PVOID lpParam);
DWORD WriterThreadProc(PVOID lpParam);

BOOL  CtsCloseHandle(HANDLE hObject);
