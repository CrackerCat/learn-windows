#include "LittleMutex.h"



INT _tmain(INT argc, TCHAR CONST *argv[], TCHAR CONST *envp[])
{
    sub_1();
    sub_2();

    _tprintf_s(_T("Input any key to exit\r\n"));
    _gettchar();

    return 0;
}

void sub_1()
{
    HANDLE hThreads[2] { 0 };
    HANDLE hMutex = CreateMutex(NULL, FALSE, NULL);

    try {
        if (hMutex == NULL) {

            _tprintf_s(_T("Err: CreateMutex() ( %d )\r\n"), GetLastError());
            throw;
        }
        for (int i = 0; i < _countof(hThreads); i++) {
            hThreads[i] = CreateThread(
                NULL, 0,
                (LPTHREAD_START_ROUTINE)ThreadProc_1,
                (PVOID)&hMutex,
                0, NULL);

            if (hThreads[i] == NULL) {
                _tprintf_s(_T("Err: CreateThread() ( %d )\r\n"), GetLastError());
                throw;
            }
        }
        WaitForMultipleObjects(_countof(hThreads), hThreads, TRUE, INFINITE);
    }
    catch (...) {}

    for (int i = 0; i < _countof(hThreads); i++) {

        if (hThreads[i] != NULL) {

            CloseHandle(hThreads[i]);
            hThreads[i] = NULL;
        }
    }

    if (hMutex != NULL) {

        CloseHandle(hMutex);
        hMutex = NULL;
    }

    return;
}

DWORD WINAPI ThreadProc_1(LPVOID lpvParam)
{
    HANDLE hMutex = *((HANDLE*)lpvParam);
    DWORD LoopCount = 0;
    BOOL  fOk = 0;

    while (LoopCount < 5)
    {
        fOk = WaitForSingleObject(hMutex, INFINITE);

        switch (fOk)
        {
        case WAIT_OBJECT_0:
            _tprintf_s(_T("%d: current ThreadId: %d\r\n"),
                ++LoopCount, GetCurrentThreadId());
            ReleaseMutex(hMutex);
            break;
        case WAIT_ABANDONED:
            return -1;
        }
    }
    return 0;
}


void sub_2()
{
    HANDLE hThreads[2] = { 0 };
    HANDLE hMutex = CreateMutex(NULL, FALSE, NULL);
    try {
        if (hMutex == NULL) {
            _tprintf_s(_T("Err: CreateMutex() ( %d )\r\n"), GetLastError());
            throw;
        }
        for (int i = 0; i < _countof(hThreads); i++) {
            hThreads[i] = CreateThread(
                NULL, 0,
                (LPTHREAD_START_ROUTINE)ThreadProc_2,
                (PVOID)&hMutex, 0, NULL);
            
            if (hThreads[i] == NULL) {
                _tprintf_s(_T("Err: CreateThread() ( %d )\r\n"), GetLastError());
                throw;
            }
        }
        WaitForMultipleObjects(_countof(hThreads), hThreads, TRUE, INFINITE);
    }
    catch (...) {}

    for (int i = 0; i < _countof(hThreads); i++) {
        if (hThreads[i] != NULL) {
            CloseHandle(hThreads[i]);
            hThreads[i] = NULL;
        }
    }

    if (hMutex != NULL) {
        CloseHandle(hMutex);
        hMutex = NULL;
    }

    return;
}

DWORD WINAPI ThreadProc_2(LPVOID lpvParam)
{
    HANDLE hMutex = *((HANDLE*)lpvParam);
    DWORD LoopCount = 0;
    BOOL  fOk = 0;

    while (LoopCount < 5) {
        fOk = WaitForSingleObject(hMutex, INFINITE);
        switch (fOk)
        {
        case WAIT_OBJECT_0:
            _tprintf_s(_T("%d: currnentThreadID: %d\r\n"),
                ++LoopCount, GetCurrentThreadId());
            break;
        case WAIT_ABANDONED:
            return -1;
        }
    }
    LoopCount = 0;
    while (LoopCount < 5) {
        ReleaseMutex(hMutex);
        LoopCount++;
    }

    return 0;
}
