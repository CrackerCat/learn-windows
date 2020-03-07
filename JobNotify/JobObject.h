#pragma once
#include <Windows.h>
#include <cstdio>
#include <tchar.h>

#define JOB_NOTIFY 0x6666

void  RestrictProcess();
DWORD WINAPI ThreadProc(LPVOID lpParam);
