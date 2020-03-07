#pragma once

#ifndef _LITTLE_MUTEX_H
#define _LITTLE_MUTEX_H


#include <cstdio>
#include <tchar.h>
#include <windows.h>


void sub_1();
DWORD WINAPI ThreadProc_1(LPVOID lpvParam);

void sub_2();
DWORD WINAPI ThreadProc_2(LPVOID lpvParam);

#endif
