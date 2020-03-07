#include "GetTargetCommandLine.h"

BOOL __EnableDebugPrivilege = TRUE;
void InjectDll();

int _tmain(int _argc, TCHAR* _argv[], TCHAR* _envp[])
{

	_tprintf(_T("Input Process ImageName\r\n"));

	if (__EnableDebugPrivilege)
	{
		EnableSeDebugPrivilege(GetCurrentProcess(), TRUE);
	}
	Wow64EnableWow64FsRedirection(FALSE);


	InjectDll();

	if (OpenClipboard(GetDesktopWindow()))
	{
		HANDLE ClipboardHandle = INVALID_HANDLE_VALUE;
		TCHAR* CommandLine = NULL;
		//获得剪贴板里的信息
		ClipboardHandle = GetClipboardData(CF_TEXT);

		TCHAR* BufferData = (TCHAR*)GlobalLock(ClipboardHandle);

		SIZE_T CommandLength = GlobalSize(ClipboardHandle);

		CommandLine = new TCHAR[CommandLength + 1];

		//将剪贴板里面的数据拷贝出来
		memcpy(CommandLine, BufferData, CommandLength);


		GlobalUnlock(ClipboardHandle);

		_tprintf_s(_T("%s\r\n"), CommandLine);

		if (CommandLine != NULL)
		{
			delete CommandLine;
			CommandLine = NULL;
		}
		EmptyClipboard();
		//最后关闭剪贴板
		CloseClipboard();
	}
	if (__EnableDebugPrivilege)
	{
		EnableSeDebugPrivilege(GetCurrentProcess(), FALSE);
	}
	_tprintf_s(_T("Input AnyKey To Exit\r\n"));
	_gettchar();

	return 0;
}


void InjectDll()
{
	TCHAR   ProcessImageName[MAX_PATH];
	HANDLE  hProcess = NULL;
	DWORD   LastError = ERROR_SUCCESS;
	BOOL    IsWow64_1 = FALSE;
	BOOL    IsWow64_2 = FALSE;
	BOOL    IsOk = TRUE;
	LPTSTR  ProcessFullPath = NULL;
	SIZE_T  DllFullPathLength = 0;
	LPVOID  VirtualAddress = NULL;
	SIZE_T  WrittenLength = 0;
	SIZE_T  ProcessFullPathLength = 0;
    TCHAR   DllFullPath[MAX_PATH];
	

	int     i = 0;
	TCHAR  ch = _gettchar();
	while (ch != _T('\n')) {
		ProcessImageName[i++] = ch;
		ch = _gettchar();
	}
	ZeroMemory(DllFullPath, sizeof(DllFullPath));
	ZeroMemory(ProcessImageName, sizeof(ProcessImageName));
	//通过进程ProcessImageName获得进程ID
	DWORD  ProcessId = NULL;
	if (!GetProcessId(&ProcessId, ProcessImageName)) {
		LastError = GetLastError();
		goto Exit;
	}

	IsWow64Process(GetCurrentProcess(), &IsWow64_1);


	//通过进程ID获得进程完整路径
	if (GetProcessFullPath(&ProcessFullPath, ProcessId)) {
		LPTSTR v1 = ProcessFullPath;
		ProcessFullPath = DosPathToNtPath(ProcessFullPath);
		free(v1);

		if (ProcessFullPath != NULL) {
			IsWow64ProcessEx(ProcessFullPath, _tcslen(ProcessFullPath), &IsWow64_2);

			if (IsWow64_1 == IsWow64_2)
			{
				//获取DLL完整路径
				GetCurrentDirectory(MAX_PATH, DllFullPath);

				_tcscat_s(DllFullPath, _T("\\Dll.dll"));

				//获取目标进程的句柄
				hProcess = OpenProcessEx(PROCESS_ALL_ACCESS, FALSE, ProcessId);

				//根据在目标进程句柄在目标进程空间中申请内存
				DllFullPathLength = (_tcslen(DllFullPath) + 1) * sizeof(TCHAR);

				VirtualAddress = VirtualAllocEx(
										hProcess, 
										NULL, 
										DllFullPathLength, 
										MEM_COMMIT, 
										PAGE_READWRITE
									);

				if (VirtualAddress == NULL) {
					LastError = GetLastError();
					goto Exit;
				}
				//在目标进程空间申请的内存中写入动态库的完整路径
				if (!ProcessMemoryWriteSafe(
									hProcess, 
									VirtualAddress, 
									DllFullPath,
									DllFullPathLength, 
									&WrittenLength)
								) {
					LastError = GetLastError();
					goto Exit;
				}
				//让目标进程执行一个线程去执行LoadLibrary(DllFullPath)
				HANDLE hThread = CreateRemoteThread(
											hProcess,
											NULL, 
											0, 
											(LPTHREAD_START_ROUTINE)LoadLibrary,
											VirtualAddress, 
											0, 
											NULL
										);
				if (hThread == NULL) {

					//释放掉曾经申请的DllFullPath所占有的内存
					VirtualFreeEx(hProcess, VirtualAddress, DllFullPathLength, MEM_RELEASE);

					goto Exit;
				}
				WaitForSingleObject(hThread, INFINITE);
				if (VirtualAddress != NULL) {
					VirtualFreeEx(hProcess, VirtualAddress, DllFullPathLength, MEM_RELEASE);
				}
			}
		}
	}
	
Exit:
	if (hProcess != NULL)
	{
		CloseHandleEx(hProcess);
		hProcess = NULL;
	}

	if (ProcessFullPath != NULL) {
		VirtualFree(ProcessFullPath, 0, MEM_RELEASE);
	}
	
}






BOOL 
EnableSeDebugPrivilege(
	HANDLE _hProcess, 
	BOOL   _IsEnable
) 
{
	DWORD  LastError;
	HANDLE hToken;
	LUID   Luid;
	TOKEN_PRIVILEGES _TokenPrivileges;
	ZeroMemory(&_TokenPrivileges, sizeof(_TokenPrivileges));
	if (!OpenProcessToken(_hProcess, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
		LastError = GetLastError();
		if (hToken)
			CloseHandleEx(hToken);
		SetLastError(LastError);
		return FALSE;
	}

	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &Luid)) {
		LastError = GetLastError();
		CloseHandleEx(hToken);
		SetLastError(LastError);
		return FALSE;
	}

	_TokenPrivileges.PrivilegeCount = 1;
	_TokenPrivileges.Privileges[0].Luid = Luid;
	if (_IsEnable)
		_TokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		_TokenPrivileges.Privileges[0].Attributes = 0;
	if (!AdjustTokenPrivileges(hToken, FALSE, &_TokenPrivileges, sizeof(TOKEN_PRIVILEGES), NULL, NULL)) {
		LastError = GetLastError();
		CloseHandleEx(hToken);
        
		SetLastError(LastError);
		return FALSE;
	}

	CloseHandleEx(hToken);
	return TRUE;
}

HANDLE 
OpenProcessEx(
	DWORD _DesiredAccess, 
	BOOL  _IsInheritHandle, 
	DWORD _ProcessId
)
{
	HANDLE hProcess;
	DWORD  LastError;
	if (__EnableDebugPrivilege) {
		EnableSeDebugPrivilege(GetCurrentProcess(), TRUE);
	}
	hProcess = OpenProcess(_DesiredAccess, _IsInheritHandle, _ProcessId);
	LastError = GetLastError();

	if (__EnableDebugPrivilege) {
		EnableSeDebugPrivilege(GetCurrentProcess(), FALSE);
	}

	SetLastError(LastError);
	return hProcess;
}

BOOL 
CloseHandleEx(
	HANDLE _hObject
)
{
	DWORD HandleFlags;
	if (GetHandleInformation(_hObject, &HandleFlags)
		&& (HandleFlags & HANDLE_FLAG_PROTECT_FROM_CLOSE) != HANDLE_FLAG_PROTECT_FROM_CLOSE)
		return (!!CloseHandle(_hObject));

	return FALSE;
}

BOOL 
GetProcessId(
	DWORD*  _ProcessId, 
	LPCTSTR _ProcessImageName
)
{
	BOOL   IsOk      =  FALSE;
	HANDLE hSnapshot =  NULL;
	PROCESSENTRY32 pe32;

	pe32.dwSize = sizeof(pe32);
	DWORD LastError = 0;
	if (!_ProcessId || !_ProcessImageName) {
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}
	//快照
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE) {
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	if (!Process32First(hSnapshot, &pe32)) {
		LastError = GetLastError();
		CloseHandleEx(hSnapshot);
		SetLastError(LastError);
		return FALSE;
	}

	do {
		if (_tcsicmp(pe32.szExeFile, _ProcessImageName) == 0) {
			*_ProcessId = pe32.th32ProcessID;
			IsOk = TRUE;
			LastError = ERROR_SUCCESS;
			break;
		}
		LastError = ERROR_MOD_NOT_FOUND;
	} while (Process32Next(hSnapshot, &pe32));
	
	if (hSnapshot != INVALID_HANDLE_VALUE) {
		CloseHandleEx(hSnapshot);
	}

	hSnapshot = NULL;
	SetLastError(LastError);
	return IsOk;
}

BOOL
GetProcessFullPath(
	LPTSTR*  _ProcessFullPath,
	DWORD    _ProcessId
)
{
	HANDLE  hProcess   = NULL;
	TCHAR*  BufferData = NULL;
	SIZE_T  BufferLength = PAGE_SIZE;
	DWORD   LastError    = ERROR_SUCCESS;
	if (!_ProcessFullPath||!_ProcessId) {
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	hProcess = OpenProcessEx(PROCESS_QUERY_INFORMATION, FALSE, _ProcessId);
	if (hProcess == NULL) {
		hProcess = OpenProcessEx(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, _ProcessId);
		if (hProcess == NULL) {
			return FALSE;
		}
	}
	
	__try 
	{
		do {
			BufferData = (TCHAR*)malloc(BufferLength * sizeof(TCHAR));
			if (BufferData == NULL) {
				LastError = GetLastError();
				goto Exit;
			}
			if (!GetProcessImageFileName(hProcess, BufferData, BufferLength))
			{
				LastError = GetLastError();
				free(BufferData);
				BufferData = NULL;
				if (LastError != ERROR_INSUFFICIENT_BUFFER) {
					LastError = GetLastError();
					goto Exit;
				}
				BufferLength += PAGE_SIZE;
			}
			else {
				*_ProcessFullPath = BufferData;
				break;
			}
		} while (TRUE);
	}

	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		LastError = GetLastError();
	}



Exit:
	if (hProcess != INVALID_HANDLE_VALUE) {
		CloseHandleEx(hProcess);
		hProcess = NULL;
	}
	SetLastError(LastError);

	return TRUE;
}

LPTSTR
DosPathToNtPath(
	LPCTSTR _DosPath
)
{
	LPTSTR NtPath = (LPTSTR)VirtualAlloc(NULL, PAGE_SIZE, MEM_COMMIT, PAGE_READWRITE);
	if (NtPath == NULL) {
		return NULL;
	}
	TCHAR VolumeDeviceName[3] = _T("A:");
	TCHAR COMDeviceName[5] = _T("COM0");
	DWORD   offset = 0;
	while (VolumeDeviceName[0] <= 0x5A)
	{
		ZeroMemory(NtPath, PAGE_SIZE);

		if (QueryDosDevice(VolumeDeviceName, NtPath, MAX_PATH * 2) > NULL)
		{
			offset = _tcslen(NtPath);
			_tcscat(NtPath, (_DosPath + offset));
			if (_tcsicmp(NtPath, _DosPath) == NULL)
			{
				ZeroMemory(NtPath, PAGE_SIZE);
				_tcscat(NtPath, VolumeDeviceName);
				_tcscat(NtPath, (_DosPath + offset));
				return NtPath;
			}
		}
		VolumeDeviceName[0]++;
	}
	while (COMDeviceName[3] <= 0x39) {
		ZeroMemory(NtPath, PAGE_SIZE);
		if (QueryDosDevice(COMDeviceName, NtPath, MAX_PATH * 2) > NULL) {
			offset = _tcslen(NtPath);
			_tcscat(NtPath, (_DosPath + offset));
			if (_tcsicmp(NtPath, _DosPath) == 0) {
				ZeroMemory(NtPath, PAGE_SIZE);
				_tcscat(NtPath, COMDeviceName);
				_tcscat(NtPath, (_DosPath + offset));
				return(NtPath);
			}
		}
		COMDeviceName[3]++;
	}
	VirtualFree(NtPath, NULL, MEM_RELEASE);

	return NULL;

}

BOOL
MappingFileEx(
	PCTSTR    _FileFullPath,
	DWORD     _DesiredAccess,
	LPHANDLE  _hFile,
	SIZE_T*   _FileSize,
	LPHANDLE  _hMappingHandle,
	LPVOID    _MappedFileVA,
	DWORD     _FileOffset
)
{
	DWORD FileAccess  = NULL;
	DWORD FileMapType = NULL;
	DWORD FileMapViewType = NULL;
	DWORD LastError = ERROR_SUCCESS;


	if (PathFileExists(_FileFullPath)) {
		if (_DesiredAccess == ACCESS_READ) {
			FileAccess  = GENERIC_READ;
			FileMapType = PAGE_READONLY;
			FileMapViewType = FILE_MAP_READ;
		}
		else if (_DesiredAccess == ACCESS_WRITE) {
			FileAccess  = GENERIC_WRITE;
			FileMapType = PAGE_READWRITE;
			FileMapViewType = FILE_MAP_WRITE;
		}
		else if (_DesiredAccess == ACCESS_ALL) {
			FileAccess  = GENERIC_READ | GENERIC_WRITE | GENERIC_EXECUTE;
			FileMapType = PAGE_EXECUTE_READWRITE;
			FileMapViewType = FILE_MAP_WRITE;
		}
		else {
			FileAccess  = GENERIC_READ | GENERIC_WRITE | GENERIC_EXECUTE;
			FileMapType = PAGE_EXECUTE_READWRITE;
			FileMapViewType = FILE_MAP_ALL_ACCESS;
		}

		//通过文件绝对路径获得文件对象
		HANDLE hFileHandle = CreateFile(
						_FileFullPath, 
						FileAccess,
						FILE_SHARE_READ, 
						NULL, 
						OPEN_EXISTING, 
						FILE_ATTRIBUTE_NORMAL, 
						NULL
					);

		if (hFileHandle != INVALID_HANDLE_VALUE) {
			*_hFile = hFileHandle;

			//通过文件句柄获得文件大小 
			DWORD FileSize = GetFileSize(hFileHandle, NULL);
			FileSize = FileSize + _FileOffset;

			*_FileSize = FileSize;

			//根据文件句柄和文件大小创建映射对象
			HANDLE hFileMapping = CreateFileMapping(
										hFileHandle, 
										NULL,
										FileMapType, 
										NULL, 
										FileSize, 
										NULL
									);

			if (hFileMapping != NULL) {
				*_hMappingHandle = hFileMapping;

				//通过映射对象获取映射的虚拟内存
				LPVOID VirtualAddress = MapViewOfFile(
											hFileMapping,
											FileMapViewType,
											NULL, 
											NULL,
											NULL
										);

				if (VirtualAddress) {
					MoveMemory(
							_MappedFileVA,
							&VirtualAddress,
							sizeof(LPVOID)
						);
					return TRUE;
				}
				else {
					LastError = GetLastError();
				}
			}
			else {
				LastError = GetLastError();
			}

			ZeroMemory(_MappedFileVA, sizeof(ULONG_PTR));
			*_hFile    = NULL;
			*_FileSize = NULL;
			CloseHandleEx(_hFile);
		}
		else {
			LastError = GetLastError();
			ZeroMemory(_MappedFileVA, sizeof(ULONG_PTR));
		}
	}

	SetLastError(LastError);

	return FALSE;
}

void
UnmappingFileEx(
	HANDLE  _hFile,
	SIZE_T  _FileSize,
	HANDLE  _hMappingHandle,
	LPVOID  _MappedFileVA
)
{
	if (UnmapViewOfFile(_MappedFileVA)) {
		CloseHandle(_hMappingHandle);
		SetFilePointer(_hFile, _FileSize, NULL, FILE_BEGIN);
		SetEndOfFile(_hFile);
		CloseHandleEx(_hFile);
	}
}

BOOL IsWow64ProcessEx(
	PCTSTR _ProcessFullPath,
	PBOOL  _IsWow64Process
)
{
	PIMAGE_DOS_HEADER ImageDosHeader = NULL;
	PIMAGE_NT_HEADERS ImageNtHeaders = NULL;
	
	HANDLE  hFile = NULL;
	SIZE_T  FileSize = 0;

	HANDLE  hMappingHandle = NULL;
	LPVOID  MappedFileVA = 0;
	
	BOOL    IsOk = TRUE;
	DWORD   LastError = ERROR_SUCCESS;

	//内存映射
	if (!MappingFileEx(
				_ProcessFullPath, 
				ACCESS_READ,
				&hFile, 
				&FileSize, 
				&hMappingHandle, 
				&MappedFileVA, 
				0)
		) {
		LastError = GetLastError();
		IsOk = FALSE;
	}
	else {

		//PE文件操作
		ImageDosHeader = (PIMAGE_DOS_HEADER)MappedFileVA;
		ImageNtHeaders = (PIMAGE_NT_HEADERS)((UINT8*)MappedFileVA + ImageDosHeader->e_lfanew);
		switch (ImageNtHeaders->FileHeader.Machine)
		{
		case IMAGE_FILE_MACHINE_I386:
		{
			*_IsWow64Process = TRUE;
			break;
		}
		case IMAGE_FILE_MACHINE_IA64:
		case IMAGE_FILE_MACHINE_AMD64:
		{
			*_IsWow64Process = FALSE;
			break;
		}
		default:
		{
			LastError = ERROR_INVALID_EA_HANDLE;
			IsOk = FALSE;
			break;
		}
		}
		UnmappingFileEx(hFile, FileSize, hMappingHandle, MappedFileVA);
	}
	SetLastError(LastError);
		return IsOk;
}

BOOL ProcessMemoryWriteSafe(
	HANDLE   _hProcess,
	LPVOID   _BaseAddress,
	LPCVOID  _BufferData,
	SIZE_T   _BufferLength,
	SIZE_T*  _WritenLength
)
{
	SIZE_T  NumOfByteWritten   = 0;
	SIZE_T* lpNumOfByteWritten = 0;
	DWORD    LastError  = 0;
	DWORD  OldProtect = 0;
	BOOL   IsOk = FALSE;
	if (!_hProcess || !_BaseAddress || !_BufferData || !_BufferLength ){

		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}
	if (!_WritenLength) {
		lpNumOfByteWritten = &NumOfByteWritten;
	}
	else {
		lpNumOfByteWritten = _WritenLength;
	}

	if (!WriteProcessMemory(_hProcess, _BaseAddress, _BufferData, _BufferLength, lpNumOfByteWritten)) {
		if (VirtualProtectEx(_hProcess, _BaseAddress, _BufferLength, PAGE_EXECUTE_READWRITE, &OldProtect)) {
			if (WriteProcessMemory(_hProcess, _BaseAddress, _BufferData, _BufferLength, lpNumOfByteWritten)) {
				IsOk = TRUE;
			}
			else {
				LastError = GetLastError();
			}
			VirtualProtectEx(_hProcess, _BaseAddress, _BufferLength, OldProtect, &OldProtect);
		}
		else {
			LastError = GetLastError();
		}
	}
	else {
		IsOk = TRUE;
	}

	SetLastError(LastError);
	return IsOk;
}
