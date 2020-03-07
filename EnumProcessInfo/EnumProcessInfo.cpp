#include "EnumHandleInfo.h"

lpfn_NtQuerySystemInformation  __NtQuerySystemInformation = NULL;

vector<HANDLE_INFORMATION> __HandleInfoVector;

void _tmain(int argc, TCHAR *argv[], TCHAR *envp[])
{
	setlocale(LC_ALL, "chs");

	Sub_1();

	_tprintf(_T("Input AnyKey to Exit\r\n"));
	_gettchar();
	 return;
}

void Sub_1()
{	
	vector<HANDLE_INFORMATION>::iterator iter;
	TCHAR  ProcessImageName[MAX_PATH] = { 0 };
	DWORD  LastError = 0;
	vector<HANDLE> ThreadIdentify{};
	int i = 0;

	TCHAR ch = _gettchar();
	while (ch != '\n') {
		ProcessImageName[i++] = ch;
		ch = _gettchar();
	}

	DWORD  ProcessId = NULL;
	if (!GetProcessId(ProcessImageName, &ProcessId)) {
		LastError = GetLastError();
		goto Exit;

	}
	if (!EnumHandleInfo(__HandleInfoVector,ProcessId)) {
		goto Exit;
	}

	for (iter = __HandleInfoVector.begin(); iter != __HandleInfoVector.end(); iter++) {
		_tprintf_s(_T("Handle Value：%p\r\n"  ), iter->HandleValue);
		_tprintf_s(_T("Object      ：0x%p\r\n"), iter->Object);
		_tprintf_s(_T("Type Index  ：%u\r\n"  ), iter->ObjectTypeIndex);


	}
Exit:
	vector<HANDLE_INFORMATION>().swap(__HandleInfoVector);

	return;
}


BOOL GetProcessId(
	_In_  LPCTSTR  _ProcessImageName,
	_Out_ DWORD*   _ProcessId
)
{
	BOOL   IsOk      = FALSE;
	DWORD  LastError = ERROR_SUCCESS;
	HANDLE hSnapshot = INVALID_HANDLE_VALUE;

	PROCESSENTRY32 pe32;
	ZeroMemory(&pe32, sizeof(pe32));
	pe32.dwSize = sizeof(PROCESSENTRY32);
	
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	
	if (hSnapshot == INVALID_HANDLE_VALUE) {
		LastError = GetLastError();
		goto Exit;
	}
	if (!Process32First(hSnapshot, &pe32)) {
		LastError = GetLastError();
		goto Exit;
	}

	do {
		if (_tcsicmp(pe32.szExeFile, _ProcessImageName) == 0) {
			*_ProcessId = pe32.th32ProcessID;
			IsOk = TRUE;
			goto Exit;
		}

	} while (Process32Next(hSnapshot, &pe32));

	LastError = ERROR_MOD_NOT_FOUND;
Exit:
	if (hSnapshot != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hSnapshot);
	}

	hSnapshot = NULL;
	SetLastError(LastError);

	return IsOk;
}

BOOL EnumHandleInfo(
	vector<HANDLE_INFORMATION>& _HandleInfoVector,
	DWORD _ProcessId)
{
	HANDLE_INFORMATION HandleInfo;
	ZeroMemory(&HandleInfo, sizeof(HandleInfo));

	SIZE_T BufferLength = 0x4000;
	SIZE_T rLen = 0;
	HMODULE hModuleHandle  = NULL;
	DWORD   LastError = 0;

	HANDLE   v2 = 0;
	NTSTATUS Status;

	_OBJECT_HELPER_::PSYSTEM_HANDLE_INFORMATION SystemHandleInfo = NULL;
	_OBJECT_HELPER_::PSYSTEM_HANDLE_INFORMATION v1 = NULL;
	

	hModuleHandle = GetModuleHandle(_T("Ntdll.dll"));
	if (hModuleHandle == NULL) {
		goto Exit;
	}

	if (__NtQuerySystemInformation == NULL) {

		__NtQuerySystemInformation = 
			(lpfn_NtQuerySystemInformation)GetProcAddress(hModuleHandle, "NtQuerySystemInformation");

	    if (__NtQuerySystemInformation==NULL)
	    {
			LastError = GetLastError();
			goto Exit;
			
	    }
	}
	SystemHandleInfo = (_OBJECT_HELPER_::PSYSTEM_HANDLE_INFORMATION)malloc(BufferLength);

	while (TRUE) {
		Status = __NtQuerySystemInformation(
			(SYSTEM_INFORMATION_CLASS)(_OBJECT_HELPER_::SystemHandleInformation), 
			SystemHandleInfo,
			BufferLength, 
			&rLen
		);
		if (Status != STATUS_SUCCESS) {
			if (Status == STATUS_INFO_LENGTH_MISMATCH) {
				free(SystemHandleInfo);
				BufferLength = BufferLength * 2;

				SystemHandleInfo = (_OBJECT_HELPER_::PSYSTEM_HANDLE_INFORMATION)malloc(BufferLength);
				if (!SystemHandleInfo) {
					goto Exit;
				}
			}
			else {
				goto Exit;
			}
		}
		else {
			break;
		}
	}

	for (int i = 0; i < SystemHandleInfo->NumberOfHandles; i++) {

		if ( SystemHandleInfo->Handles[i].UniqueProcessId == (USHORT)_ProcessId) {
			HandleInfo.Object = SystemHandleInfo->Handles[i].Object; 
			HandleInfo.HandleValue     = SystemHandleInfo->Handles[i].HandleValue; 
			HandleInfo.ObjectTypeIndex = SystemHandleInfo->Handles[i].ObjectTypeIndex;

			_HandleInfoVector.push_back(HandleInfo);
		}
	}
Exit:
	if (SystemHandleInfo != NULL) {
		free(SystemHandleInfo);
		SystemHandleInfo = NULL;
	}
	return (_HandleInfoVector.size() > 0);
}
