#include "CtsGetCurrentDirectory.h"

#ifdef  _UNICODE
#define CtsGetCurrentDirectory CtsGetCurrentDirectoryW
#else
#define CtsGetCurrentDirectory CtsGetCurrentDirectoryA
#endif 

//char short 
//Tchar 



int _tmain(int argc, TCHAR* argv[],TCHAR *envp[])
{
     setlocale(LC_ALL,"Chinese-simplified");

	 TCHAR  BufferData[MAX_PATH];
	 DWORD  LastError = ERROR_SUCCESS;

	 ZeroMemory(BufferData, sizeof(BufferData));

	 if (CtsGetCurrentDirectory(BufferData, MAX_PATH) == 0) {
		 LastError = GetLastError();
	 }
	
    _tprintf_s(_T("Input AnyKey To Exit\r\n"));
    _gettchar();
    
    return 0;
}

DWORD
WINAPI
CtsGetCurrentDirectoryA(LPSTR BufferData,DWORD BufferLength)
{
	ANSI_STRING AnsiString ;
	NTSTATUS    Status;
	DWORD LastError = ERROR_SUCCESS;
	int   IsOk = 0;

	WCHAR wstr[MAX_PATH];
	UNICODE_STRING UnicodeString;     //Peb short Sting
	
	ZeroMemory(wstr, sizeof(wstr));
	ZeroMemory(&UnicodeString, sizeof(UnicodeString));

	UnicodeString.Buffer = wstr;
	UnicodeString.MaximumLength = sizeof(wstr);  


	ULONG MaxLength;
	MaxLength = BufferLength;   //0x1000
	if (BufferLength >= UNICODE_STRING_MAX_BYTES)
	{
		MaxLength = UNICODE_STRING_MAX_BYTES - 1;
	}

	UnicodeString.Length = 
		(USHORT)RtlGetCurrentDirectory_U(
						UnicodeString.MaximumLength, 
						UnicodeString.Buffer
					);

	IsOk = WideCharToMultiByte(
						CP_ACP,	
						NULL, 
						UnicodeString.Buffer, 
						UnicodeString.Length, 
						NULL, 
						0, 
						NULL, 
						NULL
					);

	if (!IsOk) {
		return IsOk;
	}

	if (MaxLength <= IsOk) {
		SetLastError(ERROR_INSUFFICIENT_BUFFER);
		IsOk = 0;
		return IsOk;
	}

	IsOk = WideCharToMultiByte(
					CP_ACP, 
					NULL, 
					UnicodeString.Buffer,
					UnicodeString.Length, 
					BufferData, 
					MaxLength, 
					NULL, 
					NULL
				);
	
	if (!IsOk) {
		return 0;
	}

	return IsOk;
}




DWORD
WINAPI
CtsGetCurrentDirectoryW(LPWSTR BufferData,DWORD BufferLength)
{
	return RtlGetCurrentDirectory_U(BufferLength * sizeof(WCHAR), BufferData) / sizeof(TCHAR);
}

ULONG
NTAPI
RtlGetCurrentDirectory_U(
	ULONG MaximumLength,
	_Out_bytecap_(MaximumLength) WCHAR* BufferData)
{
	ULONG Length, Bytes;
	PCURDIR CurrentDirectory;
	PWSTR CurDirName;
	PPEB  Peb;

	Peb = (PPEB)NtCurrentPeb();

	EnterCriticalSection(Peb->FastPebLock);
	CurrentDirectory = &(Peb->ProcessParameters->CurrentDirectory);
	/* Get the buffer and character length */
	CurDirName = CurrentDirectory->DosPath.Buffer;
	Length = CurrentDirectory->DosPath.Length / sizeof(WCHAR);  
	Bytes  = Length * sizeof(WCHAR);
	if ((Length <= 1) || (CurDirName[Length - 2] == L':'))
	{
		
		if (MaximumLength <= Bytes)
		{

			LeaveCriticalSection(Peb->FastPebLock);
#define OBJ_NAME_PATH_SEPARATOR ((WCHAR)L'\\')
			return Bytes + sizeof(OBJ_NAME_PATH_SEPARATOR);
		}
	}
	else
	{
		if (MaximumLength < Bytes)
		{
			LeaveCriticalSection(Peb->FastPebLock);
			return Bytes;
		}
	}

	RtlCopyMemory(BufferData, CurDirName, Bytes);


	if ((Length <= 1) || (BufferData[Length - 2] != L':'))
	{
	
		BufferData[Length - 1] = UNICODE_NULL;
		--Length;
	}
	else
	{

		BufferData[Length] = UNICODE_NULL;
	}
	LeaveCriticalSection(Peb->FastPebLock);
	return Length * sizeof(WCHAR);

}
