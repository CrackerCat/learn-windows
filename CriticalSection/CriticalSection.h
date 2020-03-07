#pragma once

#include <cstdio>
#include <Windows.h>
#include <tchar.h>


// The Critical Section can be created and deleted in different threads
VOID Sub_1();
DWORD WINAPI ThreadProc1_1(LPVOID lpParam);	// InitializeCriticalSection
DWORD WINAPI ThreadProc1_2(LPVOID lpParam);	// DeleteCriticalSection

// RecursionCount(itself++)
VOID Sub_2();
DWORD WINAPI ThreadProc2_1(LPVOID lpParam);

// 3. Recursion can be modify by any threads
VOID Sub_3();
DWORD WINAPI ThreadProc3_1(LPVOID lpParam);	// Increment recursively
DWORD WINAPI ThreadProc3_2(LPVOID lpParam);	// Decrement recursively

// The Thread will be at in wait state, and wake up at a proper time
VOID Sub_4();
DWORD WINAPI ThreadProc4_1(LPVOID lpParam);

// CriticalSetion and SpinLock
VOID Sub_5();
DWORD WINAPI ThreadProc5_1(LPVOID lpParam);
