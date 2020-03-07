#pragma once

#include <tchar.h>
#include <Windows.h>



#include <vector>



#define PAGE_READ_FLAGS  \
    (PAGE_READONLY | PAGE_READWRITE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE)
#define PAGE_WRITE_FLAGS \
    (PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)

template<typename T>
/* user's thread pool work item context */
struct UTP_WORK_CONTEXT {

    HANDLE hProcess;
    T  key;
    T* addrLow;
    T* addrHigh;
    std::vector<T*>& posVector;

    UTP_WORK_CONTEXT(std::vector<T*>& vec)
        : posVector(vec) {
    }
};

BOOL 
CtsIsValidReadPoint(
    LPCVOID lpAddress
);

BOOL 
CtsIsValidWritePoint(
    LPCVOID lpAddress
);

BOOL 
CtsCloseHandle(
    HANDLE hObject
);

VOID 
CtsGetSystemInfo(
    LPVOID* lpMaximumApplicationAddress, 
    DWORD*  dwPageSize
);

BOOL 
CtsEnableSeDebugPrivilege(
    HANDLE hProcess, 
    BOOL bEnable
);

BOOL 
CtsGetProcessId(
    LPCTSTR szImageName, 
    LPDWORD lpProcessId
);

HANDLE 
CtsOpenProcess(
    DWORD dwDesiredAccess, 
    BOOL  bInheritHandle, 
    DWORD dwProcessId
);

HANDLE 
CtsOpenThread(
    DWORD dwDesiredAccess, 
    BOOL  bInheritHandle, 
    DWORD dwThreadId
);

VOID 
CtsSuspendProcess(
    DWORD dwProcessId
);

VOID 
CtsResumeProcess(
    DWORD dwProcessId
);

template<typename T>
BOOL CtsSearchMemoryFirst(
    HANDLE hProcess, 
    T key, 
    std::vector<T*>& posVector
);

template <typename T>
BOOL 
CtsSearchMemoryNext(
    HANDLE hProcess, 
    T key, 
    std::vector<T*>& __Array
);

template<typename T>
BOOL 
CtsRemoteMemoryFix(
    HANDLE hProcess, 
    T newValue, 
    std::vector<T*>& __Array
);

template<typename T>
VOID NTAPI
WorkProc(
    PTP_CALLBACK_INSTANCE Instance, 
    PVOID Context, 
    PTP_WORK Work
);
