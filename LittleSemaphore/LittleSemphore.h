#pragma once
#ifndef _LITTLE_SEMPHORE
#define _LITTLE_SEMPHORE
#include <cstdio>
#include <tchar.h>
#include <windows.h>

void sub_1();
void sub_2();
void sub_3(int recrCnt);
void sub_4();

DWORD WINAPI ThreadProc(LPVOID lpvParam);

#endif
