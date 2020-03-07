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

			wprintf(L"%s\r\n", wstr);  // 按照双字打印，
			wstr += wcslen(wstr) + 1;  //   因为环境变量在内存中存储是以 
		}                              //   UNICODE字符集方式存储
	Exit:
		if (EnvpStrings != NULL)
		{
			free(EnvpStrings);
			EnvpStrings = NULL;
		}
	}

	return IsOk;
}
