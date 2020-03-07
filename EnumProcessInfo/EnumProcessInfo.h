#pragma once
#include <windows.h>
#include <tchar.h>
#include <iostream>
#include <vector>
#include <TlHelp32.h>
#include <Winternl.h>
#include <ntstatus.h>
using namespace std;

typedef struct _HANDLE_INFORMATION_ {
	USHORT HandleValue;
	PVOID  Object;
	UCHAR  ObjectTypeIndex;
}HANDLE_INFORMATION, *PHANDLE_INFORMATION;


namespace _OBJECT_HELPER_ {

typedef enum _SYSTEM_INFORMATION_CLASS {
	SystemBasicInformation,
	SystemProcessorInformation,
	SystemPerformanceInformation,
	SystemTimeOfDayInformation,
	SystemPathInformation,
	SystemProcessInformation,
	SystemCallCountInformation,
	SystemDeviceInformation,
	SystemProcessorPerformanceInformation,
	SystemFlagsInformation,
	SystemCallTimeInformation,
	SystemModuleInformation,
	SystemLocksInformation,
	SystemStackTraceInformation,
	SystemPagedPoolInformation,
	SystemNonPagedPoolInformation,
	SystemHandleInformation,
	SystemObjectInformation,
	SystemPageFileInformation,
	SystemVdmInstemulInformation,
	SystemVdmBopInformation,
	SystemFileCacheInformation,
	SystemPoolTagInformation,
	SystemInterruptInformation,
	SystemDpcBehaviorInformation,
	SystemFullMemoryInformation,
	SystemLoadGdiDriverInformation,
	SystemUnloadGdiDriverInformation,
	SystemTimeAdjustmentInformation,
	SystemSummaryMemoryInformation,
	SystemNextEventIdInformation,
	SystemEventIdsInformation,
	SystemCrashDumpInformation,
	SystemExceptionInformation,
	SystemCrashDumpStateInformation,
	SystemKernelDebuggerInformation,
	SystemContextSwitchInformation,
	SystemRegistryQuotaInformation,
	SystemExtendServiceTableInformation,
	SystemPrioritySeperation,
	SystemPlugPlayBusInformation,
	SystemDockInformation,
#if !defined PO_CB_SYSTEM_POWER_POLICY
	SystemPowerInformation,
#else
	_SystemPowerInformation,
#endif
	SystemProcessorSpeedInformation,
	SystemCurrentTimeZoneInformation,
	SystemLookasideInformation
} SYSTEM_INFORMATION_CLASS;

typedef struct _SYSTEM_HANDLE_TABLE_ENTRY_INFO {
	USHORT  UniqueProcessId;
	USHORT  CreatorBackTraceIndex;
	UCHAR   ObjectTypeIndex;
	UCHAR   HandleAttributes;
	USHORT  HandleValue;
	PVOID   Object;
	ULONG   GrantedAccess;
} SYSTEM_HANDLE_TABLE_ENTRY_INFO, *PSYSTEM_HANDLE_TABLE_ENTRY_INFO;

typedef struct _SYSTEM_HANDLE_INFORMATION {
	ULONG NumberOfHandles;
	SYSTEM_HANDLE_TABLE_ENTRY_INFO Handles[1];
} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;


};
 

typedef decltype(&NtQuerySystemInformation) lpfn_NtQuerySystemInformation;

void Sub_1();
