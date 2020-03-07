#include "MemoryCheck.h"


#include <TlHelp32.h>

#include <cstdio>


DWORD __EnableDebugPrivilege = TRUE;
DWORD PAGE_SIZE = 0;
SRWLOCK __SRWLock;
SIZE_T  __ArrayCount = 0;
std::vector<PTP_WORK> __WorkVector;
LPVOID __MaximumApplicationAddress = NULL;

int _tmain(int argc, TCHAR* argv[]) {

    CtsGetSystemInfo(&__MaximumApplicationAddress, &PAGE_SIZE);

    TCHAR szImageName[] = _T("Test.exe");
    DWORD dwProcessId = 0;

    if (CtsGetProcessId(szImageName, &dwProcessId) == FALSE) {
        return 0;
    }
    _tprintf(_T("Test.exe - ID: %d\r\n"), dwProcessId);

    HANDLE hProcess = NULL;
    hProcess = CtsOpenProcess(
        PROCESS_VM_READ | PROCESS_QUERY_INFORMATION | PROCESS_VM_WRITE,
        FALSE, dwProcessId);

    if (hProcess == NULL) {
        return 0;
    }

    /***********************/
    /* search memory space */

    char cValue;
    _tprintf_s(_T("Input DataValue (char) = "));
    _tscanf_s(_T("%c"), &cValue, 1);

    /* suspend target process */
    CtsSuspendProcess(dwProcessId);

    std::vector<char*> __Array;
    __Array.clear();


    if (CtsSearchMemoryFirst(hProcess, cValue, __Array) == FALSE) {

        goto Exit;
    }

    _tprintf(_T("find disputable targets\r\n"));
    _tprintf(_T("Count: %d\r\n"), __ArrayCount);
    _tprintf(_T("enter ENTER, continue\r\n"));
    _gettchar();
    _gettchar();

    CtsResumeProcess(dwProcessId);
    do
    {
        _tprintf_s(_T("input target value again = "));
        _tscanf_s(_T(" %c"), &cValue, 1);
        if (CtsSearchMemoryNext(hProcess, cValue, __Array) == FALSE) {
            goto Exit;
        }
        for (SIZE_T i = 0; i < __ArrayCount; i++) {
            _tprintf(_T("Address: 0x%p\r\n"), __Array[i]);
        }
    } while (__ArrayCount > 1);

    /* suspend process again for modify data */
    CtsSuspendProcess(dwProcessId);

    _tprintf_s(_T("please input new value:\r\n"));
    _gettchar();
    _tscanf_s(_T("%c"), &cValue, 1);

    /* modify the memory */
    CtsRemoteMemoryFix(hProcess, cValue, __Array);

    /* resume the process */
    CtsResumeProcess(dwProcessId);

Exit:
    if (hProcess != NULL) {

        CtsCloseHandle(hProcess);
        hProcess = NULL;
    }

    _tprintf_s(_T("input any key to exit...\r\n"));
    _gettchar();
    _gettchar();

    return 0;
}



BOOL CtsIsValidReadPoint(LPCVOID lpAddress)
{
    BOOL bOk = FALSE;
    MEMORY_BASIC_INFORMATION MemoryBasicInfo { 0 };
    VirtualQuery(lpAddress, 
        &MemoryBasicInfo, sizeof(MEMORY_BASIC_INFORMATION));

    if ((MemoryBasicInfo.State == MEM_COMMIT && 
        (MemoryBasicInfo.Protect & PAGE_READ_FLAGS))) {

        bOk = TRUE;
    }
    return bOk;
}

BOOL CtsIsValidWritePoint(LPCVOID lpAddress)
{
    BOOL bOk = FALSE;
    MEMORY_BASIC_INFORMATION MemoryBasicInfo { 0 };
    VirtualQuery(lpAddress, 
        &MemoryBasicInfo, sizeof(MEMORY_BASIC_INFORMATION));

    if ((MemoryBasicInfo.State == MEM_COMMIT && 
        (MemoryBasicInfo.Protect & PAGE_WRITE_FLAGS))) {

        bOk = TRUE;
    }
    return bOk;
}

BOOL CtsCloseHandle(HANDLE hObject)
{
    DWORD dwFlags;
    if (GetHandleInformation(hObject, &dwFlags)
        && (dwFlags & HANDLE_FLAG_PROTECT_FROM_CLOSE)
            != HANDLE_FLAG_PROTECT_FROM_CLOSE)
        return !!CloseHandle(hObject);
    return FALSE;
}

VOID CtsGetSystemInfo(LPVOID* lpMaximumApplicationAddress, DWORD* dwPageSize)
{
    SYSTEM_INFO SystemInfo;
    GetSystemInfo(&SystemInfo);
    *lpMaximumApplicationAddress = 
        SystemInfo.lpMaximumApplicationAddress;
    *dwPageSize = SystemInfo.dwPageSize;

    _tprintf_s(_T("MaximumApplicationAddress = %p\r\n"), 
        lpMaximumApplicationAddress);
    _tprintf_s(_T("PageSize = %d\r\n"), *dwPageSize);
}

BOOL CtsEnableSeDebugPrivilege(HANDLE hProcess, BOOL bEnable)
{
    DWORD  dwErrCode = ERROR_SUCCESS;
    HANDLE hToken = NULL;

    if (!OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {

        dwErrCode = GetLastError();
        if (hToken) {
            CloseHandle(hToken);
        }
        return dwErrCode;
    }

    TOKEN_PRIVILEGES TokenPrivileges { 0 };
    LUID luid;

    if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid)) {

        dwErrCode = GetLastError();
        CloseHandle(hToken);
        return dwErrCode;
    }
    TokenPrivileges.PrivilegeCount = 1;
    TokenPrivileges.Privileges[0].Luid = luid;
    if (bEnable) {
        TokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    }
    else {
        TokenPrivileges.Privileges[0].Attributes = 0;
    }
    AdjustTokenPrivileges(hToken, FALSE, 
        &TokenPrivileges, sizeof(TOKEN_PRIVILEGES), NULL, NULL);

    dwErrCode = GetLastError();
    CloseHandle(hToken);

    return dwErrCode;
}


BOOL CtsGetProcessId(LPCTSTR szImageName, LPDWORD lpProcessId)
{

    BOOL bOk = FALSE;
    HANDLE hSnapshot = INVALID_HANDLE_VALUE;
    PROCESSENTRY32 ProcessEntry32 { 0 };
    ProcessEntry32.dwSize = sizeof(PROCESSENTRY32);

    if (CtsIsValidReadPoint(szImageName) == FALSE || 
        CtsIsValidWritePoint(lpProcessId) == FALSE) {
        /* if the parameter is invalid, return */
        return FALSE;
    }

    /* create the snap shot */
    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {

        _tprintf_s(_T("Err: CreateToolhelp32Snapshot() ( %d )\r\n"), GetLastError());
        return FALSE;
    }
    if (Process32First(hSnapshot, &ProcessEntry32) == FALSE) {

        _tprintf_s(_T("Err: Process32First() ( %d )\r\n"), GetLastError());
        CloseHandle(hSnapshot);
        return FALSE;
    }

    do {
        /* loop, until find the target process or get nothing */
        if (0 == _memicmp(ProcessEntry32.szExeFile, 
                szImageName, _tcslen(szImageName) * sizeof(TCHAR))) {
            /* if match, break and return the target ID */
            *lpProcessId = ProcessEntry32.th32ProcessID;
            bOk = TRUE;
            break;
        }

    } while (Process32Next(hSnapshot, &ProcessEntry32));

    if (hSnapshot != INVALID_HANDLE_VALUE) {
        CloseHandle(hSnapshot);
    }

    return bOk;
}

HANDLE CtsOpenProcess(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId)
{

    HANDLE hProcess = NULL;
    DWORD dwErrCode = ERROR_SUCCESS;

    // privilege level up
    if (__EnableDebugPrivilege) {
        
        CtsEnableSeDebugPrivilege(GetCurrentProcess(), TRUE);
    }

    hProcess = OpenProcess(dwDesiredAccess, bInheritHandle, dwProcessId);
    dwErrCode = GetLastError();

    // privilege level down
    if (__EnableDebugPrivilege) {
        
        CtsEnableSeDebugPrivilege(GetCurrentProcess(), FALSE);
    }
    SetLastError(dwErrCode);
    return hProcess;
}

HANDLE CtsOpenThread(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwThreadId)
{
    HANDLE  hThread = NULL;
    DWORD dwErrCode = ERROR_SUCCESS;

    // privilege level up
    if (__EnableDebugPrivilege) {
    
        CtsEnableSeDebugPrivilege(GetCurrentProcess(), TRUE);
    }
    
    hThread = OpenThread(dwDesiredAccess, bInheritHandle, dwThreadId);
    dwErrCode = GetLastError();
    
    // privilege level down
    if (__EnableDebugPrivilege) {

        CtsEnableSeDebugPrivilege(GetCurrentProcess(), FALSE);
    }
    SetLastError(dwErrCode);
    return hThread;
}

VOID CtsSuspendProcess(DWORD dwProcessId)
{
    /* create a snap shot in target process for enumerate thread */
    HANDLE hSnapshot = 
        CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, dwProcessId);

    if (hSnapshot != INVALID_HANDLE_VALUE) {

        THREADENTRY32 te32 = { sizeof(THREADENTRY32) };
        BOOL bOk = Thread32First(hSnapshot, &te32);

        /* suspend all threads in the process */
        for (; bOk; bOk = Thread32Next(hSnapshot, &te32)) {

            if (te32.th32OwnerProcessID == dwProcessId) {
                HANDLE hThread = 
                    CtsOpenThread(THREAD_SUSPEND_RESUME, 
                        FALSE, te32.th32ThreadID);
                if (hThread != NULL) {
                    SuspendThread(hThread);
                }
                CloseHandle(hThread);
            }
        }
        CloseHandle(hSnapshot);
    }
}

VOID CtsResumeProcess(DWORD dwProcessId)
{
    /* create a snap shot in target process for enumerate thread */
    HANDLE ToolHelp32Handle = 
        CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, dwProcessId);

    if (ToolHelp32Handle != INVALID_HANDLE_VALUE) {
        
        THREADENTRY32 te32 = { sizeof(THREADENTRY32) };
        BOOL bOk = Thread32First(ToolHelp32Handle, &te32);

        /* resume all threads in the process */
        for (; bOk; bOk = Thread32Next(ToolHelp32Handle, &te32)) {
            
            if (te32.th32OwnerProcessID == dwProcessId) {
                
                HANDLE hThread = 
                    CtsOpenThread(THREAD_SUSPEND_RESUME, 
                        FALSE, te32.th32ThreadID);
                if (hThread != NULL){
                    ResumeThread(hThread);
                }
                CloseHandle(hThread);
            }
        }
        CloseHandle(ToolHelp32Handle);
    }
}

template<typename T>
BOOL CtsSearchMemoryFirst(HANDLE hProcess, T key, std::vector<T*>& posVector)
{   
    PTP_POOL  ThreadPool = NULL;
    TP_CALLBACK_ENVIRON CallBackEnviron;

    PTP_CLEANUP_GROUP CleanUpGroup = NULL;
    BOOL bOk = FALSE;


    UTP_WORK_CONTEXT<T>* utpWorkContext = NULL;
    SIZE_T addrBase = NULL;

    /* get system version */
    OSVERSIONINFO OSVersionInfo { 0 };
#pragma warning(disable : 4996)
    GetVersionEx(&OSVersionInfo);
    if (OSVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) {
        /* Windows 98 4MB - 2G */
        addrBase = 4 * 1024 * 1024;
    }
    else {
        /* Windows NT 64KB - 2G  NULL */
        addrBase = 64 * 1024;
    }

    SIZE_T currRegion = addrBase;
    SIZE_T nextRegion = NULL;

    /* create custom thread pool */
    ThreadPool = CreateThreadpool(NULL);
    /* Initialize a callback environment */
    InitializeThreadpoolEnvironment(&CallBackEnviron);

    

    if (NULL == ThreadPool) {
        
        _tprintf_s(_T("Err: CreateThreadpool() ( %d )\r\n"), GetLastError());
        goto Exit;
    }

    /* Set the minimum and maximum number of threads in the thread pool */
    SetThreadpoolThreadMaximum(ThreadPool, 10);
    bOk = SetThreadpoolThreadMinimum(ThreadPool, 8);

    if (!bOk) {
        _tprintf_s(_T("Err: SetThreadpoolThreadMinimum() ( %d )\r\n"), GetLastError());
        goto Exit;
    }

    /* create clean tp group */
    CleanUpGroup = CreateThreadpoolCleanupGroup();
    if (NULL == CleanUpGroup) {
        _tprintf(_T("Err: CreateThreadpoolCleanupGroup() ( %d )\r\n"), GetLastError());
    }

    /* associate thread pool with callback environment */
    SetThreadpoolCallbackPool(&CallBackEnviron, ThreadPool);

    /* associate callback environment with cleanup group */
    SetThreadpoolCallbackCleanupGroup(&CallBackEnviron, CleanUpGroup, NULL);

    InitializeSRWLock(&__SRWLock);

    currRegion = addrBase;

    while (currRegion < (SIZE_T)__MaximumApplicationAddress) {
        
        MEMORY_BASIC_INFORMATION MemoryBasicInfo { 0 };
        SIZE_T BlockSize = 0;

        T* addrLow  = NULL;
        T* addrHigh = NULL;
        

        /* 
         * VirtualQueryEx:
         * Retrieves information about a range of pages within 
         * the virtual address space of a specified process.
         */
        BlockSize = VirtualQueryEx(hProcess, 
            (LPCVOID)currRegion, &MemoryBasicInfo, sizeof(MemoryBasicInfo));
        
        if (BlockSize == sizeof(MemoryBasicInfo)) {

            nextRegion = (SIZE_T)MemoryBasicInfo.BaseAddress
                + (SIZE_T)MemoryBasicInfo.RegionSize + 1;
            
            if (!(MemoryBasicInfo.Protect & (PAGE_NOACCESS | PAGE_GUARD)))
            {
                addrLow  = (T*)MemoryBasicInfo.BaseAddress;
                addrHigh = (T*)((SIZE_T)MemoryBasicInfo.BaseAddress
                                    + (SIZE_T)MemoryBasicInfo.RegionSize);
            }
        }

        utpWorkContext = new UTP_WORK_CONTEXT<T>(posVector);

        utpWorkContext->hProcess = hProcess;
        utpWorkContext->key = key;
        utpWorkContext->addrLow  = addrLow;
        utpWorkContext->addrHigh = addrHigh;
        __WorkVector.push_back(
            CreateThreadpoolWork(
                (PTP_WORK_CALLBACK)WorkProc<T> ,
                (PVOID)utpWorkContext,
                &CallBackEnviron)
        );

        if (nextRegion <= currRegion) {
            break;
        }
        currRegion = nextRegion;
    }
    
    for (PTP_WORK item : __WorkVector) {

        SubmitThreadpoolWork(item);
    }

Exit:
    /* waits until all the items remaining in the thread pool's work group */
    CloseThreadpoolCleanupGroupMembers(CleanUpGroup, FALSE, NULL);

    /* close the thread pool cleanup group */
    CloseThreadpoolCleanupGroup(CleanUpGroup);
    CleanUpGroup = NULL;

    /* Clean the callback environment */
    DestroyThreadpoolEnvironment(&CallBackEnviron);

    /* close thread pool */
    CloseThreadpool(ThreadPool);
    ThreadPool = NULL;

    return TRUE;
}

template <typename T>
BOOL CtsSearchMemoryNext(HANDLE hProcess, T key, std::vector<T*>& __Array)
{
    // 保存 m_arList 数组中有效地址的个数，初始化新的 m_nListCnt 值
    SIZE_T cnt = __ArrayCount;   //[][]
    __ArrayCount = 0;
    // 在m_arList数组记录的地址处查找
    BOOL bOk = FALSE;
    T value = 0;
    for (SIZE_T i = 0; i < cnt; i++) {
        
        if (ReadProcessMemory(hProcess, (LPVOID)__Array[i], 
                &value, sizeof(T), NULL)) {

            if (value == key) { 
                __Array[__ArrayCount++] = __Array[i];
                bOk = TRUE;
            }
        }
    }
    return bOk;
}

template<typename T>
BOOL CtsRemoteMemoryFix(HANDLE hProcess, T newValue, std::vector<T*>& __Array)
{
    if (WriteProcessMemory(hProcess,
        (LPVOID)__Array[0], &newValue, sizeof(T), NULL) == TRUE) {

        return TRUE;
    }
    return FALSE;
}

template<typename T>
VOID NTAPI
WorkProc(PTP_CALLBACK_INSTANCE Instance, PVOID Context, PTP_WORK Work)
{
    DWORD dwOldProtect = 0;
    PBYTE lpBuffer = NULL;
    HANDLE hFileMapping = NULL;

    /* if 'new' fails, we create a file mapping */
    lpBuffer = new BYTE[PAGE_SIZE];

    if (lpBuffer == NULL) {

        hFileMapping = CreateFileMapping(
            INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE,
            0, PAGE_SIZE, NULL);

        lpBuffer = (LPBYTE)MapViewOfFile(
            hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);

        if (lpBuffer == NULL) {

           
        }

    }

    UTP_WORK_CONTEXT<T>* utpWorkContext = (UTP_WORK_CONTEXT<T>*)Context;
    std::vector<T*>& posVector = utpWorkContext->posVector;

    T* addrLow  = utpWorkContext->addrLow;
    T* addrHigh = utpWorkContext->addrHigh;

    if (addrLow > 0 && addrHigh > 0) {

        /* round up -- align: PAGE_SIZE */
        addrLow = (T*)((SIZE_T)addrLow - ((SIZE_T)addrLow % PAGE_SIZE));

        while (addrLow < addrHigh) {

            if (!ReadProcessMemory(
                    utpWorkContext->hProcess, (LPCVOID)addrLow,
                    lpBuffer, PAGE_SIZE, NULL)) {

                /* modify the memory attribute */
                VirtualProtect((LPVOID)addrLow, PAGE_SIZE, PAGE_READWRITE, &dwOldProtect);

                if (!ReadProcessMemory(
                        utpWorkContext->hProcess, (LPCVOID)addrLow,
                        lpBuffer, PAGE_SIZE, NULL)) {

                    /* if still can't read the data, return */
                    addrLow += PAGE_SIZE;
                    continue;
                }

                T* lpKey;
                for (SIZE_T i = 0; i < (SIZE_T)PAGE_SIZE - sizeof(T) + 1; i++) {

                    lpKey = (T*)&lpBuffer[i];
                    if (lpKey[0] == utpWorkContext->key) {
                        /* find, insert to posVector */
                        AcquireSRWLockExclusive(&__SRWLock);
                        __ArrayCount++;
                        posVector.push_back((T*)((SIZE_T)addrLow + i));
                        ReleaseSRWLockExclusive(&__SRWLock);
                    }
                }
                /* recover the memory attribute */
                VirtualProtect((LPVOID)addrLow, PAGE_SIZE, dwOldProtect, NULL);
            }
            else {

                T* lpKey;
                for (SIZE_T i = 0; i < (SIZE_T)PAGE_SIZE - sizeof(T) + 1; i++) {

                    lpKey = (T*)&lpBuffer[i];
                    if (lpKey[0] == utpWorkContext->key) {
                        /* find, insert to posVector */
                        AcquireSRWLockExclusive(&__SRWLock);
                        __ArrayCount++;
                        posVector.push_back((T*)((SIZE_T)addrLow + i));
                        ReleaseSRWLockExclusive(&__SRWLock);
                    }
                }
            }
            addrLow += PAGE_SIZE;
        }
    }
    delete Context;

    if (hFileMapping != NULL) {
        UnmapViewOfFile(lpBuffer);
        lpBuffer = NULL;
        CtsCloseHandle(hFileMapping);
    }

    if (lpBuffer != NULL) {

        delete lpBuffer;
        lpBuffer = NULL;
    }
    return;
}
