#define  WIN32_LEAN_AND_MEAN  
#include <windows.h>
#include <tchar.h>
#include <TlHelp32.h>
#include <cstdio>
#include <psapi.h>
#include <Shlwapi.h>
#pragma comment(lib,"Psapi.lib")
#pragma comment(lib,"Shlwapi.lib")
#pragma warning(disable: 4996)

#define PAGE_SIZE 0x2000


//打开目标进程
HANDLE
OpenProcessEx(
	_In_ DWORD _DesiredAccess,
	_In_ BOOL  _IsInheritHandle,
	_In_ DWORD _ProcessId
);

BOOL
CloseHandleEx(
	_In_ HANDLE _hObject
);

BOOL
EnableSeDebugPrivilege(
	_In_ HANDLE _hProcess,
	_In_ BOOL   _IsEnable
);


// 通过ImageName获得进程ID
BOOL
GetProcessId(
	_Out_ DWORD*  _ProcessId,
	_In_  LPCTSTR _ProcessImageName);

//通过进程ID获得完整路径
BOOL
GetProcessFullPath(
	_Out_ LPTSTR*  _ProcessFullPath,
	_In_  DWORD    _ProcessId
);

//将完整路径进程转换
LPTSTR
DosPathToNtPath(
	_In_ LPCTSTR _DosPath
);

//通过目标进程完整路径获得目标与当前进程的位数

#define ACCESS_READ  0
#define ACCESS_WRITE 1
#define ACCESS_ALL   2

BOOL
MappingFileEx(
	_In_  PCTSTR    _FileFullPath,
	_In_  DWORD     _DesiredAccess,
	_Out_ LPHANDLE  _hFile,
	_Out_ SIZE_T*   _FileSize,
	_Out_ LPHANDLE  _hMappingHandle,
	_Out_ LPVOID    _MappedFileVA,
	_In_opt_ DWORD  _FileOffset
);

void
UnmappingFileEx(
	_In_ HANDLE  _hFile,
	_In_ SIZE_T  _FileSize,
	_In_ HANDLE  _hMappingHandle,
	_In_ LPVOID  _MappedFileVA
);

BOOL 
IsWow64ProcessEx(
	_In_  PCTSTR _ProcessFullPath,
	_Out_ PBOOL  _IsWow64Process
);


//在目标进程空间申请内存并将动态库的路径写入目标
BOOL 
ProcessMemoryWriteSafe(
	_In_  HANDLE   _hProcess,
	_In_  LPVOID   _BaseAddress,
	_In_  LPCVOID  _BufferData,
	_In_  SIZE_T   _BufferLength,
	_Out_ SIZE_T*  _WritenLength
);
