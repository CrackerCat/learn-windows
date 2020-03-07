#pragma once
#include <windows.h>
#include <iostream>
#include <tchar.h>

using namespace std;

#ifndef NT_SUCCESS
#define NT_SUCCESS(StatCode)  ((NTSTATUS)(StatCode) >= 0)
#endif

#define STATUS_SUCCESS 0

typedef enum _PROCESSINFOCLASS
{
	ProcessBasicInformation,
	ProcessQuotaLimits,
	ProcessIoCounters,
	ProcessVmCounters,
	ProcessTimes,
	ProcessBasePriority,
	ProcessRaisePriority,
	ProcessDebugPort,
	ProcessExceptionPort,
	ProcessAccessToken,
	ProcessLdtInformation,
	ProcessLdtSize,
	ProcessDefaultHardErrorMode,
	ProcessIoPortHandlers,
	ProcessPooledUsageAndLimits,
	ProcessWorkingSetWatch,
	ProcessUserModeIOPL,
	ProcessEnableAlignmentFaultFixup,
	ProcessPriorityClass,
	ProcessWx86Information,
	ProcessHandleCount,
	ProcessAffinityMask,
	ProcessPriorityBoost,
	ProcessDeviceMap,
	ProcessSessionInformation,
	ProcessForegroundInformation,
	ProcessWow64Information,
	ProcessImageFileName,
	ProcessLUIDDeviceMapsEnabled,
	ProcessBreakOnTermination,
	ProcessDebugObjectHandle,
	ProcessDebugFlags,
	ProcessHandleTracing,
	ProcessIoPriority,
	ProcessExecuteFlags,
	ProcessTlsInformation,
	ProcessCookie,
	ProcessImageInformation,
	ProcessCycleTime,
	ProcessPagePriority,
	ProcessInstrumentationCallback,
	ProcessThreadStackAllocation,
	ProcessWorkingSetWatchEx,
	ProcessImageFileNameWin32,
	ProcessImageFileMapping,
	ProcessAffinityUpdateMode,
	ProcessMemoryAllocationMode,
	MaxProcessInfoClass
} PROCESSINFOCLASS;

template <class T>
struct _LIST_ENTRY_T
{
	T Flink;
	T Blink;
};

template <class T>
struct _UNICODE_STRING_T
{
	union
	{
		struct
		{
			WORD Length;
			WORD MaximumLength;
		};
		T dummy;
	};
	T Buffer;
};

template<class T>
struct  _STRING_T
{
	union
	{
		struct
		{
			WORD Length;
			WORD MaximumLength;
		};
		T dummy;
	};
	T Buffer;
};

template<class T>
struct _CURDIR_T
{
	_UNICODE_STRING_T<T> DosPath;
	T Handle;
};

template<class T>
struct _RTL_DRIVE_LETTER_CURDIR_T
{

	WORD Flags;
	WORD Length;
	ULONG TimeStamp;
	_UNICODE_STRING_T<T> DosPath;

};

template <class T, class NGF, int A>
struct _PEB_T
{
	union
	{
		struct
		{
			BYTE InheritedAddressSpace;
			BYTE ReadImageFileExecOptions;
			BYTE BeingDebugged;
			BYTE BitField;
		};
		T dummy01;
	};
	T Mutant;
	T ImageBaseAddress;
	T Ldr;
	T ProcessParameters;
	T SubSystemData;
	T ProcessHeap;
	T FastPebLock;
	T AtlThunkSListPtr;
	T IFEOKey;
	T CrossProcessFlags;
	T UserSharedInfoPtr;
	DWORD SystemReserved;
	DWORD AtlThunkSListPtr32;
	T ApiSetMap;
	T TlsExpansionCounter;
	T TlsBitmap;
	DWORD TlsBitmapBits[2];
	T ReadOnlySharedMemoryBase;
	T HotpatchInformation;
	T ReadOnlyStaticServerData;
	T AnsiCodePageData;
	T OemCodePageData;
	T UnicodeCaseTableData;
	DWORD NumberOfProcessors;
	union
	{
		DWORD NtGlobalFlag;
		NGF dummy02;
	};
	LARGE_INTEGER CriticalSectionTimeout;
	T HeapSegmentReserve;
	T HeapSegmentCommit;
	T HeapDeCommitTotalFreeThreshold;
	T HeapDeCommitFreeBlockThreshold;
	DWORD NumberOfHeaps;
	DWORD MaximumNumberOfHeaps;
	T ProcessHeaps;
	T GdiSharedHandleTable;
	T ProcessStarterHelper;
	T GdiDCAttributeList;
	T LoaderLock;
	DWORD OSMajorVersion;
	DWORD OSMinorVersion;
	WORD OSBuildNumber;
	WORD OSCSDVersion;
	DWORD OSPlatformId;
	DWORD ImageSubsystem;
	DWORD ImageSubsystemMajorVersion;
	T ImageSubsystemMinorVersion;
	T ActiveProcessAffinityMask;
	T GdiHandleBuffer[A];
	T PostProcessInitRoutine;
	T TlsExpansionBitmap;
	DWORD TlsExpansionBitmapBits[32];
	T SessionId;
	ULARGE_INTEGER AppCompatFlags;
	ULARGE_INTEGER AppCompatFlagsUser;
	T pShimData;
	T AppCompatInfo;
	_UNICODE_STRING_T<T> CSDVersion;
	T ActivationContextData;
	T ProcessAssemblyStorageMap;
	T SystemDefaultActivationContextData;
	T SystemAssemblyStorageMap;
	T MinimumStackCommit;
	T FlsCallback;
	_LIST_ENTRY_T<T> FlsListHead;
	T FlsBitmap;
	DWORD FlsBitmapBits[4];
	T FlsHighIndex;
	T WerRegistrationData;
	T WerShipAssertPtr;
	T pContextData;
	T pImageHeaderHash;
	T TracingFlags;
};

typedef _PEB_T<DWORD, DWORD64, 34> PEB32;
typedef _PEB_T<DWORD64, DWORD, 30> PEB64;

template <class T>
struct _RTL_USER_PROCESS_PARAMETERS_T
{
	ULONG MaximumLength;
	ULONG Length;
	ULONG Flags;
	ULONG DebugFlags;
	T     ConsoleHandle;
	ULONG ConsoleFlags;
	T     StandardInput;
	T     StandardOutput;
	T     StandardError;
	_CURDIR_T<T> CurrentDirectory;
	_UNICODE_STRING_T<T> DllPath;
	_UNICODE_STRING_T<T> ImagePathName;
	_UNICODE_STRING_T<T> CommandLine;
	T     Environment;
	ULONG StartingX;
	ULONG StartingY;
	ULONG CountX;
	ULONG CountY;
	ULONG CountCharsX;
	ULONG CountCharsY;
	ULONG FillAttribute;
	ULONG WindowFlags;
	ULONG ShowWindowFlags;
	_UNICODE_STRING_T<T> WindowTitle;
	_UNICODE_STRING_T<T> DesktopInfo;
	_UNICODE_STRING_T<T> ShellInfo;
	_UNICODE_STRING_T<T> RuntimeData;
	_RTL_DRIVE_LETTER_CURDIR_T<T> CurrentDirectores[32];
	T     EnvironmentSize;
	T     EnvironmentVersion;
};
typedef _RTL_USER_PROCESS_PARAMETERS_T<DWORD>   RTL_USER_PROCESS_PARAMETERS32;
typedef _RTL_USER_PROCESS_PARAMETERS_T<DWORD64> RTL_USER_PROCESS_PARAMETERS64;

template<typename T>
struct _PEB_T2
{
	typedef typename std::conditional<std::is_same<T, DWORD>::value, PEB32, PEB64>::type type;
};

template<typename T>
struct _PROCESS_BASIC_INFORMATION_T
{
	NTSTATUS ExitStatus;
	ULONG    Reserved0;
	T	     PebBaseAddress;
	T	     AffinityMask;
	LONG	 BasePriority;
	ULONG	 Reserved1;
	T	     UniqueProcessId;
	T	     InheritedFromUniqueProcessId;
};

typedef struct _PROCESS_BASIC_INFORMATION
{
	NTSTATUS  ExitStatus;
	PVOID     PebBaseAddress;
	ULONG_PTR AffinityMask;
	ULONG     BasePriority;
	HANDLE    UniqueProcessId;
	HANDLE    InheritedFromUniqueProcessId;
} PROCESS_BASIC_INFORMATION, *PPROCESS_BASIC_INFORMATION;

// IsWowProcess
typedef
BOOL(
WINAPI 
*LPFN_ISWOW64PROCESS)(
	_In_  HANDLE hProcess, 
	_Out_ PBOOL  Wow64Process
);

// NtQueryInformationProcess
typedef
NTSTATUS(
NTAPI
*LPFN_NTQUERYINFORMATIONPROCESS)(
	_In_  HANDLE hProcess,
	_In_  ULONG  ProcessInformationClass,
	_Out_ PVOID  ProcessInformation,
	_In_  ULONG  ProcessInformationLength,
	_Out_opt_ PULONG ReturnLength
);

// NtWow64QueryInformationProcess64
typedef
NTSTATUS(
NTAPI
*LPFN_NTWOW64QUERYINFORMATIONPROCESS64)(
	_In_  HANDLE  hProcess,
	_In_  ULONG   ProcessInformationClass,
	_Out_ PVOID   ProcessInformation,
	_In_  ULONG   ProcessInformationLength,
	_Out_opt_ PULONG ReturnLength
);

// NtWow64ReadVirtualMemory64()
typedef 
NTSTATUS(
NTAPI 
*LPFN_NTWOW64READVIRTUALMEMORY64)(
	_In_  HANDLE   hProcess,
	_In_  ULONG64  BaseAddress,
	_Out_ PVOID    BufferData,
	_In_  ULONG64  BufferSize,
	_Out_opt_ PULONG64 ReturnLength
);


BOOL
EnableSeDebugPrivilege(
	_In_ HANDLE _hProcess,
	_In_ BOOL   _IsEnable
);

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

BOOL IsWow64ProcessEx(
	_In_  HANDLE _hProcess,
	_Out_ BOOL*  _IsWow64Process
);

#include "ProcessHelper.h"

ULONG64 
GetPeb(
	_In_  HANDLE ProcessHandle, 
	_Out_ PEB64* Peb
);

ULONG32 GetPeb(
	_In_  HANDLE ProcessHandle,
	_Out_ PEB32* Peb
);

BOOL
ReadProcessMemoryEx(
	_In_  HANDLE   hProcess, 
	_In_  DWORD    BaseAddress,
	_Out_ LPVOID   BufferData, 
	_In_  SIZE_T   BufferSize,
	_Out_opt_ DWORD64* ReturnLength
);

BOOL
ReadProcessMemoryEx(
	_In_  HANDLE   hProcess,
	_In_  DWORD64  BaseAddress,
	_Out_ LPVOID   BufferData,
	_In_  SIZE_T   BufferSize,
	_Out_opt_ DWORD64* ReturnLength
);

template<typename T>
BOOL GetProcessEnvironmentVariableByPeb(
	_In_ HANDLE hProcess
) //32       32  64
{
	BOOL   IsOk = TRUE;
	TCHAR* EnvpStrings = NULL;
	WCHAR* wstr = NULL;      //游走指针  环境变量在内存中存储是以UNICODE字符集方式存储

	typename _PEB_T2<T>::type Peb = { { { 0 } } };
	_RTL_USER_PROCESS_PARAMETERS_T<T> RtlUserProcessParameters = { 0 };

	if (GetPeb(hProcess, &Peb) != 0 
		&& TRUE == ReadProcessMemoryEx(
						hProcess,
						Peb.ProcessParameters, 
						&RtlUserProcessParameters, 
						sizeof(RtlUserProcessParameters), 
						0)
		) {

		EnvpStrings = (TCHAR*)malloc(sizeof(TCHAR)*RtlUserProcessParameters.EnvironmentSize);
		if (EnvpStrings == NULL)
		{
			return FALSE;
		}
		//读环境变量
		if (FALSE == ReadProcessMemoryEx(
					hProcess,
					RtlUserProcessParameters.
					Environment, 
					EnvpStrings,
					RtlUserProcessParameters.EnvironmentSize, 
					0)
			) {
			goto Exit;
		}
		wstr = (WCHAR*)EnvpStrings;   // 赋值给游走指针,为了最后释放内存
									    // 输出打印
		while (*wstr)
		{

			wprintf(L"%s\r\n", wstr); // 按照双字打印，
			wstr += wcslen(wstr) + 1;  //   因为环境变量在内存中存储是以 
		}                                                               //   UNICODE字符集方式存储
	Exit:
		if (EnvpStrings != NULL)
		{
			free(EnvpStrings);
			EnvpStrings = NULL;
		}
	}

	return IsOk;
}

Process.cpp

#include "Process.h"
#include "ProcessHelper.h"

extern LPFN_NTQUERYINFORMATIONPROCESS __NtQueryInformationProcess;


LPFN_NTWOW64READVIRTUALMEMORY64       __NtWow64ReadVirtualMemory64       = NULL;
LPFN_NTWOW64QUERYINFORMATIONPROCESS64 __NtWow64QueryInformationProcess64 = NULL;


ULONG32 GetPeb(
	HANDLE hProcess, 
	PEB32* Peb
)
{
	PROCESS_BASIC_INFORMATION ProcessBasicInfo;
	ZeroMemory(&ProcessBasicInfo, sizeof(ProcessBasicInfo));

	ULONG ReturnLength = 0;
Loop:
	__try
	{
		if (__NtQueryInformationProcess)
		{
			if (NT_SUCCESS(__NtQueryInformationProcess(
									hProcess, 
									ProcessBasicInformation,
									&ProcessBasicInfo,
									(ULONG)sizeof(ProcessBasicInfo), 
									&ReturnLength)) 
				&& Peb) {
				if (TRUE == ReadProcessMemory(
									hProcess, 
									ProcessBasicInfo.PebBaseAddress,
									Peb, 
									sizeof(PEB32), 
									NULL) 
					) {
					return reinterpret_cast<ULONG32>(ProcessBasicInfo.PebBaseAddress);
				}
			}
		}
		else
		{
			HMODULE hModuleBase = NULL;
			hModuleBase = GetModuleHandle(_T("Ntdll.dll"));
			if (hModuleBase == NULL)
			{
				return 0;
			}
			__NtQueryInformationProcess = 
				(LPFN_NTQUERYINFORMATIONPROCESS)GetProcAddress(
								hModuleBase,
								"NtQueryInformationProcess"
							);
			if (__NtQueryInformationProcess == NULL) {
				return 0;
			}
			goto Loop;
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return 0;
	}

	return 0;
}

ULONG64 GetPeb(
	HANDLE hProcess, 
	PEB64* Peb
)
{
	_PROCESS_BASIC_INFORMATION_T<DWORD64> ProcessBasicInfo = { 0 };
	ULONG ReturnLength = 0;

Loop:
	__try
	{
		if (__NtWow64QueryInformationProcess64&&__NtWow64ReadVirtualMemory64) {
			if (NT_SUCCESS(__NtWow64QueryInformationProcess64(
									hProcess, 
									ProcessBasicInformation, 
									&ProcessBasicInfo,
									(ULONG)sizeof(ProcessBasicInfo), 
									&ReturnLength)) 
				&& Peb) {
				if (NT_SUCCESS(__NtWow64ReadVirtualMemory64(
									hProcess, 
									ProcessBasicInfo.PebBaseAddress,
									Peb, 
									sizeof(PEB64), 
									NULL))
					) {
					return ProcessBasicInfo.PebBaseAddress;
				}
				else {
					return NULL;
				}
			}
		}
		else {
			HMODULE hNtdllModuleBase = NULL;
			hNtdllModuleBase = GetModuleHandle(_T("Ntdll.dll"));
			if (hNtdllModuleBase == NULL)
			{
				return 0;
			}
			__NtWow64QueryInformationProcess64 = 
				(LPFN_NTWOW64QUERYINFORMATIONPROCESS64)GetProcAddress(
									hNtdllModuleBase,
									"NtWow64QueryInformationProcess64"
								);
			__NtWow64ReadVirtualMemory64 = 
				(LPFN_NTWOW64READVIRTUALMEMORY64)GetProcAddress(
									hNtdllModuleBase,
									"NtWow64ReadVirtualMemory64"
								);

			if (__NtWow64QueryInformationProcess64 == NULL 
				 ||   __NtWow64ReadVirtualMemory64 == NULL) {
				return 0;
			}
			goto Loop;
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return 0;
	}
	return 0;
}

BOOL 
ReadProcessMemoryEx(
	HANDLE   hProcess, 
	DWORD    BaseAddress, 
	LPVOID   BufferData, 
	SIZE_T   BufferSize, 
	DWORD64* ReturnLength
)
{
	return ReadProcessMemory(
				hProcess, 
				reinterpret_cast<LPVOID>(BaseAddress), 
				BufferData,
				BufferSize, 
				reinterpret_cast<SIZE_T*>(ReturnLength)
			);
}

BOOL 
ReadProcessMemoryEx(
	HANDLE   hProcess, 
	DWORD64  BaseAddress,
	LPVOID   BufferData, 
	SIZE_T   BufferSize, 
	DWORD64* ReturnLength
)
{
Loop:
	__try
	{
		if (__NtWow64ReadVirtualMemory64)
		{
			if ( STATUS_SUCCESS != __NtWow64ReadVirtualMemory64(
							hProcess,
							BaseAddress, 
							BufferData, 
							BufferSize, 
							ReturnLength)
				) {
				return FALSE;
			}
		}
		else {
			HMODULE hNtdllModuleBase = NULL;
			hNtdllModuleBase = GetModuleHandle(_T("Ntdll.dll"));
			if (hNtdllModuleBase == NULL)
			{
				return FALSE;
			}
			__NtWow64ReadVirtualMemory64 = 
				(LPFN_NTWOW64READVIRTUALMEMORY64)GetProcAddress(
										hNtdllModuleBase,
										"NtWow64ReadVirtualMemory64"
