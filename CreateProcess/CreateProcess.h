#pragma once
#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <Userenv.h>
#include <WtsApi32.h>
#include <Ktmw32.h>

using namespace std;
#pragma comment(lib, "Wtsapi32.lib")
#pragma comment(lib, "Userenv.lib" )
#pragma comment(lib, "KtmW32.lib"  ) 

#define PS_ATTRIBUTE_NUMBER_MASK 0x0000FFFF
#define PS_ATTRIBUTE_THREAD      0x00010000 
#define PS_ATTRIBUTE_INPUT       0x00020000
#define PS_ATTRIBUTE_ADDITIVE    0x00040000 

#define PsAttributeValue(Number, Thread, Input, Additive) \
    (((Number) & PS_ATTRIBUTE_NUMBER_MASK) |              \
    ((Thread) ? PS_ATTRIBUTE_THREAD : 0)   |              \
    ((Input)  ? PS_ATTRIBUTE_INPUT  : 0)   |              \
    ((Additive) ? PS_ATTRIBUTE_ADDITIVE : 0))

typedef enum _PS_ATTRIBUTE_NUM {
	PsAttributeParentProcess,
	PsAttributeDebugPort,
	PsAttributeToken,
	PsAttributeClientId,
	PsAttributeTebAddress,
	PsAttributeImageName,
	PsAttributeImageInfo,
	PsAttributeMemoryReserve,
	PsAttributePriorityClass,
	PsAttributeErrorMode,
	PsAttributeStdHandleInfo,
	PsAttributeHandleList,
	PsAttributeGroupAffinity,
	PsAttributePreferredNode,
	PsAttributeIdealProcessor,
	PsAttributeUmsThread,
	PsAttributeMitigationOptions,
	PsAttributeProtectionLevel,
	PsAttributeSecureProcess,
	PsAttributeJobList,
	PsAttributeMax
} PS_ATTRIBUTE_NUM;

#define RTL_USER_PROCESS_PARAMETERS_NORMALIZED              0x01
#define RTL_USER_PROCESS_PARAMETERS_PROFILE_USER            0x02
#define RTL_USER_PROCESS_PARAMETERS_PROFILE_KERNEL          0x04
#define RTL_USER_PROCESS_PARAMETERS_PROFILE_SERVER          0x08
#define RTL_USER_PROCESS_PARAMETERS_UNKNOWN                 0x10
#define RTL_USER_PROCESS_PARAMETERS_RESERVE_1MB             0x20
#define RTL_USER_PROCESS_PARAMETERS_RESERVE_16MB            0x40
#define RTL_USER_PROCESS_PARAMETERS_CASE_SENSITIVE          0x80
#define RTL_USER_PROCESS_PARAMETERS_DISABLE_HEAP_CHECKS     0x100
#define RTL_USER_PROCESS_PARAMETERS_PROCESS_OR_1            0x200
#define RTL_USER_PROCESS_PARAMETERS_PROCESS_OR_2            0x400
#define RTL_USER_PROCESS_PARAMETERS_PRIVATE_DLL_PATH        0x1000
#define RTL_USER_PROCESS_PARAMETERS_LOCAL_DLL_PATH          0x2000
#define RTL_USER_PROCESS_PARAMETERS_IMAGE_KEY_MISSING       0x4000
#define RTL_USER_PROCESS_PARAMETERS_NX                      0x20000

typedef struct _UNICODE_STRING
{
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef enum _PS_CREATE_STATE {
	PsCreateInitialState,
	PsCreateFailOnFileOpen,
	PsCreateFailOnSectionCreate,
	PsCreateFailExeFormat,
	PsCreateFailMachineMismatch,
	PsCreateFailExeName, // Debugger specified
	PsCreateSuccess,
	PsCreateMaximumStates
} PS_CREATE_STATE;

typedef struct _PS_CREATE_INFO {
	SIZE_T Size;
	PS_CREATE_STATE State;
	union {
		// PsCreateInitialState
		struct {
			union {
				ULONG InitFlags;
				struct {
					UCHAR WriteOutputOnExit : 1;
					UCHAR DetectManifest    : 1;
					UCHAR IFEOSkipDebugger  : 1;
					UCHAR IFEODoNotPropagateKeyState : 1;
					UCHAR SpareBits1 : 4;
					UCHAR SpareBits2 : 8;
					USHORT ProhibitedImageCharacteristics : 16;
				};
			};
			ACCESS_MASK AdditionalFileAccess;
		} InitState;

		// PsCreateFailOnSectionCreate
		struct {
			HANDLE FileHandle;
		} FailSection;

		// PsCreateFailExeFormat
		struct {
			USHORT DllCharacteristics;
		} ExeFormat;

		// PsCreateFailExeName
		struct {
			HANDLE IFEOKey;
		} ExeName;

		// PsCreateSuccess
		struct {
			union {
				ULONG OutputFlags;
				struct {
					UCHAR ProtectedProcess : 1;
					UCHAR AddressSpaceOverride  : 1;
					UCHAR DevOverrideEnabled    : 1; // from Image File Execution Options
					UCHAR ManifestDetected      : 1;
					UCHAR ProtectedProcessLight : 1;
					UCHAR SpareBits1  : 3;
					UCHAR SpareBits2  : 8;
					USHORT SpareBits3 : 16;
				};
			};
			HANDLE FileHandle;
			HANDLE SectionHandle;
			ULONGLONG UserProcessParametersNative;
			ULONG UserProcessParametersWow64;
			ULONG CurrentParameterFlags;
			ULONGLONG PebAddressNative;
			ULONG PebAddressWow64;
			ULONGLONG ManifestAddress;
			ULONG ManifestSize;
		} SuccessState;
	};
} PS_CREATE_INFO, *PPS_CREATE_INFO;

typedef struct _CURDIR
{
	UNICODE_STRING DosPath;
	HANDLE Handle;
} CURDIR, *PCURDIR;

typedef struct _RTL_DRIVE_LETTER_CURDIR
{
	USHORT Flags;
	USHORT Length;
	ULONG  TimeStamp;
	UNICODE_STRING DosPath;
} RTL_DRIVE_LETTER_CURDIR, *PRTL_DRIVE_LETTER_CURDIR;

#define RTL_MAX_DRIVE_LETTERS 32

typedef struct _RTL_USER_PROCESS_PARAMETERS
{
	ULONG MaximumLength;	// 6c0
	ULONG Length;// 6c0
	ULONG Flags; // 0
	ULONG DebugFlags;// 0
	HANDLE ConsoleHandle; // NULL
	ULONG  ConsoleFlags;  // 0
	HANDLE StandardInput; // NULL
	HANDLE StandardOutput;// NULL
	HANDLE StandardError; // NULL
	CURDIR CurrentDirectory;
	UNICODE_STRING DllPath;
	UNICODE_STRING ImagePathName;
	UNICODE_STRING CommandLine;
	PWSTR Environment;
	ULONG StartingX;
	ULONG StartingY;
	ULONG CountX;
	ULONG CountY;
	ULONG CountCharsX;
	ULONG CountCharsY;
	ULONG FillAttribute;
	ULONG WindowFlags;
	ULONG ShowWindowFlags;
	UNICODE_STRING WindowTitle;
	UNICODE_STRING DesktopInfo;
	UNICODE_STRING ShellInfo;
	UNICODE_STRING RuntimeData;
	RTL_DRIVE_LETTER_CURDIR CurrentDirectories[RTL_MAX_DRIVE_LETTERS];
#if (NTDDI_VERSION >= NTDDI_LONGHORN)
	SIZE_T EnvironmentSize;
#endif
#if (NTDDI_VERSION >= NTDDI_WIN7)
	SIZE_T EnvironmentVersion;
#endif
} RTL_USER_PROCESS_PARAMETERS, *PRTL_USER_PROCESS_PARAMETERS;

typedef struct _PS_ATTRIBUTE {
	ULONGLONG Attribute;				/// PROC_THREAD_ATTRIBUTE_XXX | PROC_THREAD_ATTRIBUTE_XXX modifiers, 
										 /// see ProcThreadAttributeValue macro and Windows Internals 6 (372)
	SIZE_T Size;					    /// Size of Value or *ValuePtr
	union {
		ULONG_PTR Value;				/// Reserve 8 bytes for data (such as a Handle or a data pointer)
		PVOID ValuePtr;					/// data pointer
	};
	PSIZE_T ReturnLength;			    /// Either 0 or specifies size of data returned to caller via "ValuePtr"
} PS_ATTRIBUTE, *PPS_ATTRIBUTE;

typedef struct _PS_ATTRIBUTE_LIST {
	SIZE_T TotalLength;					/// sizeof(PS_ATTRIBUTE_LIST)
	PS_ATTRIBUTE Attributes[2];			/// Depends on how many attribute entries should be supplied to NtCreateUserProcess
} PS_ATTRIBUTE_LIST, *PPS_ATTRIBUTE_LIST;

typedef struct _PS_PROTECTION {
	union {
		UCHAR Level;
		struct {
			UCHAR Type   : 3;
			UCHAR Audit  : 1;
			UCHAR Signer : 4;
		};
	};
} PS_PROTECTION, *PPS_PROTECTION;

typedef enum _PS_PROTECTED_TYPE {
	PsProtectedTypeNone,
	PsProtectedTypeProtectedLight,
	PsProtectedTypeProtected,
	PsProtectedTypeMax
} PS_PROTECTED_TYPE;

typedef enum _PS_PROTECTED_SIGNER {
	PsProtectedSignerNone,
	PsProtectedSignerAuthenticode,
	PsProtectedSignerCodeGen,
	PsProtectedSignerAntimalware,
	PsProtectedSignerLsa,
	PsProtectedSignerWindows,
	PsProtectedSignerWinTcb,
	PsProtectedSignerMax
} PS_PROTECTED_SIGNER;

#define PROCESS_CREATE_FLAGS_LARGE_PAGE_SYSTEM_DLL  0x00000020
#define PROCESS_CREATE_FLAGS_PROTECTED_PROCESS      0x00000040
#define PROCESS_CREATE_FLAGS_CREATE_SESSION         0x00000080 
#define PROCESS_CREATE_FLAGS_INHERIT_FROM_PARENT    0x00000100
#define THREAD_CREATE_FLAGS_CREATE_SUSPENDED        0x00000001
#define THREAD_CREATE_FLAGS_SKIP_THREAD_ATTACH      0x00000002 
#define THREAD_CREATE_FLAGS_HIDE_FROM_DEBUGGER      0x00000004
#define THREAD_CREATE_FLAGS_HAS_SECURITY_DESCRIPTOR 0x00000010 
#define THREAD_CREATE_FLAGS_ACCESS_CHECK_IN_TARGET  0x00000020
#define THREAD_CREATE_FLAGS_INITIAL_THREAD          0x00000080

typedef 
NTSTATUS(NTAPI 
	*LPFN_NTCREATEUSERPROCESS)(
		_Out_ PHANDLE ProcessHandle,
		_Out_ PHANDLE ThreadHandle,
		_In_  ACCESS_MASK ProcessDesiredAccess,
		_In_  ACCESS_MASK ThreadDesiredAccess,
		_In_opt_  PVOID ProcessObjectAttributes,
		_In_opt_  PVOID ThreadObjectAttributes,
		_In_  ULONG CreateProcessFlags,
		_In_  ULONG CreateThreadFlags,
		_In_  PRTL_USER_PROCESS_PARAMETERS ProcessParameters,
		_In_  PVOID CreateInfo,
		_In_  PVOID AttributeList
	);

typedef
NTSTATUS(NTAPI
	*LPFN_NTRESUMETHREAD)(
		_In_  HANDLE ThreadHandle,
		_Out_opt_ PULONG PreviousSuspendCount
	);

typedef struct _OBJECT_ATTRIBUTES
{
	ULONG  Length;
	HANDLE RootDirectory;
	PUNICODE_STRING ObjectName;
	ULONG Attributes;
	PVOID SecurityDescriptor;
	PVOID SecurityQualityOfService;
} OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;

typedef 
NTSTATUS(__stdcall 
	*LPFN_NTCREATEPROCESSEX)(
		_Out_    PHANDLE ProcessHandle,
		_In_     ACCESS_MASK        DesiredAccess,
		_In_opt_ POBJECT_ATTRIBUTES ObjectAttributes,
		_In_     HANDLE  ParentProcess,
		_In_     BOOLEAN InheritObjectTable,
		_In_opt_ HANDLE  SectionHandle,
		_In_opt_ HANDLE  DebugPort,
		_In_opt_ HANDLE  ExceptionPort,
		_In_     BOOLEAN InJob
	);

typedef 
NTSTATUS(NTAPI 
	*LPFN_NTCREATESECTION)(
		_Out_    PHANDLE SectionHandle,
		_In_     ACCESS_MASK        DesiredAccess,
		_In_opt_ POBJECT_ATTRIBUTES ObjectAttributes,
		_In_opt_ PLARGE_INTEGER     MaximumSize,
		_In_     ULONG  SectionPageProtection,
		_In_     ULONG  AllocationAttributes,
		_In_opt_ HANDLE FileHandle
	);

BOOL 
NtStatusToBool(
	NTSTATUS* Status
);



typedef 
BOOL(WINAPI 
	*LPFN_CREATEPROCESSINTERNALW)(
		HANDLE  hToken,
		LPCWSTR lpApplicationName, // 执行程序文件名 
		LPCWSTR lpCommandLine,     // 参数行 
		LPSECURITY_ATTRIBUTES lpProcessAttributes, // 进程安全参数 
		LPSECURITY_ATTRIBUTES lpThreadAttributes,  // 线程安全参数 
		BOOL    bInheritHandles, // 继承标记 
		DWORD   dwCreationFlags, // 创建标记 
		LPVOID  lpEnvironment,   // 环境变量 
		LPCWSTR lpCurrentDirectory,
		LPSTARTUPINFOW lpStartupInfo, // 创建该子进程的相关参数 
		LPPROCESS_INFORMATION lpProcessInformation, // 创建后用于被创建子进程的信息
		PHANDLE hNewToken
	);

typedef
BOOL(WINAPI
	*LPFN_CREATEPROCESSINTERNALA)(
		HANDLE hToken,
		LPCSTR lpApplicationName, // 执行程序文件名 
		LPSTR  lpCommandLine,     // 参数行 
		LPSECURITY_ATTRIBUTES lpProcessAttributes, // 进程安全参数 
		LPSECURITY_ATTRIBUTES lpThreadAttributes,  // 线程安全参数 
		BOOL   bInheritHandles, // 继承标记 
		DWORD  dwCreationFlags, // 创建标记 
		LPVOID lpEnvironment,   // 环境变量 
		LPCSTR lpCurrentDirectory,
		LPSTARTUPINFOA lpStartupInfo, // 创建该子进程的相关参数 
		LPPROCESS_INFORMATION lpProcessInformation, // 创建后用于被创建子进程的信息
		PHANDLE hNewToken
		);

#ifdef _UNICODE
#define LPFN_CREATEPROCESSINTERNAL LPFN_CREATEPROCESSINTERNALW
#else
#define LPFN_CREATEPROCESSINTERNAL LPFN_CREATEPROCESSINTERNALA
#endif


void CallNtCreateUserProcess();	// NtCreateUserProcess
void CallCreateProcess();           // CreateProcess
void CallNtCreateProcessEx();       // NtCreateProcessEx
void CallCreateProcessAsUser();     // CreateProcessAsUser
void CallCreateProcessWithLogon();  // CreateProcessWithLogon
void CallShellExecute();			// ShellExecute
void CallWinExec();				// WinExec
void CallShellExecuteEx();          // ShellExecuteEx
void CallCreateProcessInternal();	// CreateProcessInternal
