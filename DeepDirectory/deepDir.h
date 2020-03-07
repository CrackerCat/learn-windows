#pragma once

#include <cstdio>
#include <tchar.h>
#include <Windows.h>

HANDLE
CreateFileInDirectory(
	_In_ LPCTSTR lpFullPath,
	_In_ DWORD dwDesiredAccess,
	_In_ DWORD dwShareMode,
	_In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	_In_ DWORD dwCreationDisposition,
	_In_ DWORD dwFlagsAndAttributes,
	_In_opt_ HANDLE hTemplateFile
);

BOOL
DeleteDirectory(
	_In_ LPCTSTR lpFullPath,
	_In_ SIZE_T nPathLen
);
