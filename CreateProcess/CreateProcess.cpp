#include "CreateProcess.h"

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	_tprintf_s(_T("ProcessIdentity:%d\r\n"), GetCurrentProcessId());

	// CallCreateProcess();
	// CallNtCreateProcessEx();
	// CallCreateProcessAsUser();   // NtCreateProcess     Ndll.dll
	// CallCreateProcessWithLogon();
	// CallNtCreateUserProcess();	// NtCreateUserProcess Ntdll.dll
	// CallShellExecute();   
	// CallWinExec();
	// CallShellExecuteEx();
	// CallCreateProcessInternal();

	_tprintf_s(_T("Input AnyKey To Exit\r\n"));
	_gettchar();
	return 0;
}


BOOL NtStatusToBool(NTSTATUS* Status)
{
	if ((*Status >= 0x0 && *Status <= 0x3FFFFFFF) || (*Status >= 0x40000000 && *Status <= 0x7FFFFFFF))
	{
		return TRUE;
	}
	return FALSE;
}

void CallCreateProcess()
{
	//子进程的名称
	LPCTSTR ApplicationName = _T("Test.exe");
	STARTUPINFO StartupInfo;
	PROCESS_INFORMATION ProcessInfo;
	BOOL IsOk = FALSE;
	//清空结构体的内存
	ZeroMemory(&StartupInfo, sizeof(StartupInfo));
	StartupInfo.cb = sizeof(StartupInfo);
	ZeroMemory(&ProcessInfo, sizeof(ProcessInfo));
	//创建子进程
	IsOk = CreateProcess(ApplicationName, NULL, NULL, NULL, FALSE,
		CREATE_NEW_CONSOLE, NULL, NULL, &StartupInfo, &ProcessInfo);
	DWORD LastError = GetLastError();
	if (!IsOk)
	{
		_tprintf_s(_T("CreateProcess() Error\n"));
		return;
	}
	//等待直到子进程退出
	WaitForSingleObject(ProcessInfo.hProcess, INFINITE);

	// 关闭进程和线程句柄
	CloseHandle(ProcessInfo.hProcess);
	CloseHandle(ProcessInfo.hThread);
}
//WIN32API函数CreateProcess用来创建一个新的进程和它的主线程，这个新进程运行指定的可执行文件
/*参数：1指向一个NULL结尾的、用来指定可执行模块的字符串
2指向一个NULL结尾的、用来指定要运行的命令行。
3指向一个SECURITY_ATTRIBUTES结构体，这个结构体决定是否返回的句柄可以被子进程继承。如果lpProcessAttributes参数为空（NULL），那么句柄不能被继承。
4指向一个SECURITY_ATTRIBUTES结构体，这个结构体决定是否返回的句柄可以被子进程继承。如果lpThreadAttributes参数为空（NULL），那么句柄不能被继承。
5指示新进程是否从调用进程处继承了句柄。如果参数的值为真，调用进程中的每一个可继承的打开句柄都将被子进程继承。被继承的句柄与原进程拥有完全相同的值和访问权限。
6指定附加的、用来控制优先类和进程的创建的标志。以下的创建标志可以以除下面列出的方式外的任何方式组合后指定。*/
void CallNtCreateProcessEx()
{
	HMODULE  NtModuleHandle = NULL;
	NTSTATUS Status = CMC_STATUS_SUCCESS; // STATUS_SUCCESS (Zero)
	HANDLE  TransactionHandle    = NULL;
	HANDLE  FileTransactedHandle = NULL;
	HANDLE  SectionHandle = NULL;
	HANDLE  ProcessHandle = NULL;
	LPCTSTR FileName = _T("Test.exe");
	LPFN_NTCREATEPROCESSEX  NtCreateProcessEx = NULL;
	LPFN_NTCREATESECTION    NtCreateSection   = NULL;
	
	BOOL rVal = FALSE;

	NtModuleHandle = GetModuleHandle(_T("Ntdll.dll"));

	if (NtModuleHandle != NULL)
	{
		NtCreateProcessEx =
			(LPFN_NTCREATEPROCESSEX)GetProcAddress(NtModuleHandle, "NtCreateProcessEx");

		//NtCreateProcessEx	ntdll.dll!0x76e156e8 (加载符号以获取其他信息)	
		//long(__stdcall*)
		//(void * *, unsigned long, _OBJECT_ATTRIBUTES *, void *, unsigned char, void *, void *, void *, unsigned char)

		
		//内存对象
		NtCreateSection = 
			(LPFN_NTCREATESECTION)GetProcAddress(NtModuleHandle, "NtCreateSection");
		// NtCreateSection	ntdll.dll!0x76e15728 (加载符号以获取其他信息)	long(__stdcall*)
		// (void * *, unsigned long, _OBJECT_ATTRIBUTES *, _LARGE_INTEGER *, unsigned long, unsigned long, void *)
		// 创建一个对象
		TransactionHandle = CreateTransaction(NULL, NULL, NULL, NULL, NULL, NULL, NULL);
		//TransactionHandle	0x00000020	void *
		if (TransactionHandle == INVALID_HANDLE_VALUE)
		{
			return;
		}
		//对象与文件关联
		FileTransactedHandle = CreateFileTransacted(
									FileName,
									(GENERIC_READ | GENERIC_WRITE), 
									0, 
									NULL, 
									OPEN_ALWAYS, 
									FILE_ATTRIBUTE_SYSTEM, 
									NULL, 
									TransactionHandle, 
									NULL, 
									NULL
								);
		//DestFileHandle	0x00000024	void *
		//创建一个Section对象
		Status = NtCreateSection(
					&SectionHandle, 
					SECTION_ALL_ACCESS, 
					NULL, 
					0, 
					PAGE_READONLY, 
					SEC_IMAGE, 
					FileTransactedHandle
				);
		//		SectionHandle	0x00000028	void *
		if (NtStatusToBool(&Status) != TRUE)
		{
			return;
		}
		if (NtCreateProcessEx != NULL)
		{
			Status = NtCreateProcessEx(
						&ProcessHandle,
						PROCESS_ALL_ACCESS,
						NULL,
						GetCurrentProcess(),
						TRUE,
						SectionHandle,
						NULL,
						NULL,
						FALSE
					);
		}
		else
		{
			_tprintf_s(_T("NtCreateProcessEx = NULL\r\n"));
		}
	}

	_tprintf_s(_T("Input any key To Exit\n"));
	_gettchar();
	return;
}

void CallCreateProcessAsUser()
{
	HANDLE TokenHandle = NULL;
	HANDLE DuplicateTokenHandle = NULL;
	
	LPVOID EnvironmentBlock = NULL;
	DWORD  SessionId  = NULL;
	HANDLE hProcess   = NULL;
	
	STARTUPINFO         StartupInfo;
	PROCESS_INFORMATION ProcessInfo;
	DWORD CreationFlags = NULL;
	TCHAR Desktop[]     = _T("WinSta0\\Default");
	TCHAR CommandLine[] = _T("Notepad.exe");
	BOOL  rVal = FALSE;

	SessionId = WTSGetActiveConsoleSessionId();
	hProcess  = GetCurrentProcess();
	ZeroMemory(&StartupInfo, sizeof(STARTUPINFO));
	ZeroMemory(&ProcessInfo, sizeof(PROCESS_INFORMATION));

	//获取当前进程的Token
	OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &TokenHandle);
	rVal = DuplicateTokenEx(
				TokenHandle, 
				MAXIMUM_ALLOWED, 
				NULL, 
				SecurityIdentification, 
				TokenPrimary, 
				&DuplicateTokenHandle
			);
	if (!rVal)
	{
		_tprintf_s(_T("%d"), GetLastError());
		CloseHandle(TokenHandle);
		return;
	}

	SetTokenInformation(DuplicateTokenHandle, TokenSessionId, &SessionId, sizeof(DWORD));
	//创建环境信息
	rVal = CreateEnvironmentBlock(&EnvironmentBlock, DuplicateTokenHandle, FALSE);
	if (!rVal)
	{
		_tprintf_s(_T("%d"), GetLastError());
		CloseHandle(DuplicateTokenHandle);
		CloseHandle(hProcess);
		return;
	}
	
	StartupInfo.cb = sizeof(StartupInfo);
	CreationFlags  = NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE;
	StartupInfo.lpDesktop = Desktop;
	
#ifdef  _UNICODE 
	CreationFlags = CreationFlags | CREATE_UNICODE_ENVIRONMENT;
#endif 
	rVal = CreateProcessAsUser(
					DuplicateTokenHandle,
					NULL, 
					CommandLine, 
					NULL, 
					NULL, 
					FALSE,
					CreationFlags, 
					EnvironmentBlock, 
					NULL, 
					&StartupInfo, 
					&ProcessInfo
				);
	if(!rVal)
	{
		_tprintf_s(_T("%d"), GetLastError());

		DestroyEnvironmentBlock(EnvironmentBlock);
		CloseHandle(DuplicateTokenHandle);
		CloseHandle(TokenHandle);
		return;
	}
	//等待直到子进程退出
	WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
	DestroyEnvironmentBlock(EnvironmentBlock);
	CloseHandle(DuplicateTokenHandle);
	CloseHandle(TokenHandle);

	return;
	/*CreateProcessAsUser函数创造一个新的进程和它的主线程。然后，新流程运行指定的可执行文件。
	CreateProcessAsUser函数类似于CreateProcess函数，只不过新进程在由hToken参数表示的用户的安全上下文中运行。
	默认情况下，新的进程是非交互式的，也就是说，它在一个不可见的桌面上运行，并且不能接收用户输入。
	另外，在默认情况下，新流程继承了调用流程的环境，而不是与指定用户相关联的环境。
	CreateProcessWithLogonW函数类似于CreateProcessAsUser，只不过调用者不需要调用LogonUser函数来认证用户并获得一个令牌。
	这个函数也类似于SHCreateProcessAsUserW函数。*/
}

void CallCreateProcessWithLogon()
{
	WCHAR Buffer[MAX_PATH];
	STARTUPINFOW        StartupInfo;
	PROCESS_INFORMATION ProcessInfo;
	WCHAR CommandLine[] = L"Notepad";
	BOOL  rVal    = FALSE;
	DWORD ErrCode = ERROR_SUCCESS;

	ZeroMemory(Buffer, sizeof(Buffer));
	ZeroMemory(&StartupInfo, sizeof(StartupInfo));
	ZeroMemory(&ProcessInfo, sizeof(ProcessInfo));

	GetCurrentDirectoryW(MAX_PATH, Buffer);
	

	//第一参数  用户名
	//第二参数  帐户数据库包含lpUsername帐户的域或服务器的名称，可以传NULL
	//第三参数  lpUsername帐户的明文密码
	//第四参数	登录选项
	//          #define LOGON_WITH_PROFILE 0x00000001
	//			#define LOGON_NETCREDENTIALS_ONLY 0x00000002
	//第五参数	要执行的模块名称，可以传NULL
	//第六参数	子进程的名称，注意此参数不能是指向只读存储器的指针，例如const变量或常量字符串
	//第七参数  控制进程创建的标志
	//			#define	CREATE_DEFAULT_ERROR_MODE    0x04000000
	//			#define CREATE_NEW_CONSOLE           0x00000010
	//			#define	CREATE_NEW_PROCESS_GROUP     0x00000200
	//			#define CREATE_SEPARATE_WOW_VDM      0x00000800
	//			#define	CREATE_SUSPENDED             0x00000004
	//			#define CREATE_UNICODE_ENVIRONMENT   0x00000400
	//			#define EXTENDED_STARTUPINFO_PRESENT 0x00080000
	//第八参数  一个指向新进程的环境块的指针。如果该参数为NULL，
	//			那么新的进程将使用由lpUsername指定的用户配置文件创建的环境
	//第九参数	进程的当前目录的完整路径 传NULL则为C:\WINDOWS\system32目录
	//第十参数	 一个指向STARTUPINFO 或者STARTUPINFOEX 的指针
	//第十一参数 一个指向PROCESS_INFORMATION 的指针

	StartupInfo.cb = sizeof(STARTUPINFO);
	rVal = CreateProcessWithLogonW(
					L"Minghao", 
					NULL, 
					L"987412365",
					LOGON_WITH_PROFILE, 
					NULL,
					CommandLine,
					CREATE_UNICODE_ENVIRONMENT, 
					NULL, 
					Buffer,
					&StartupInfo, 
					&ProcessInfo
				);
	
	if (rVal < 32)//检测是否成功
	{
		ErrCode = GetLastError();
	}
	return;
}

void CallNtCreateUserProcess()
{
	//创建出64位的进程EProcess 
	//
	UNICODE_STRING   ProcessImageName;
	PS_CREATE_INFO   PsCreateInfo;
	RTL_USER_PROCESS_PARAMETERS RtlUserProcessParameters;
	PS_ATTRIBUTE_LIST PsAttributeList;
	PS_PROTECTION     PsProtection;

	NTSTATUS Status = STATUS_PENDING;
	HANDLE   ProcessHandle = NULL;
	HANDLE   ThreadHandle  = NULL;
	WCHAR    NameBuffer[] = L"\\??\\C:\\Windows\\System32\\notepad.exe";
	HMODULE  ModuleBase = NULL;
	ULONG	 PreviousSuspendCount = 0;

	///We should supply a minimal environment (environment variables). Following one is simple yet fits our needs. 
	char Environment[2 * sizeof(ULONGLONG)] = { 'Y', 0x00, 0x3D, 0x00, 'Q', 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };


	LPFN_NTCREATEUSERPROCESS NtCreateUserProcess = NULL;
	LPFN_NTRESUMETHREAD      NtResumeThread = NULL;

	ZeroMemory(&ProcessImageName, sizeof(ProcessImageName));
	
	ProcessImageName.Buffer = NameBuffer;
	ProcessImageName.Length = sizeof(NameBuffer) - sizeof(UNICODE_NULL);
	ProcessImageName.MaximumLength = sizeof(NameBuffer);
	
	

	PsProtection.Signer = (UCHAR)PsProtectedSignerNone;
	PsProtection.Type   = (UCHAR)PsProtectedTypeNone;
	PsProtection.Audit  = 0;

	
	RtlSecureZeroMemory(&RtlUserProcessParameters, sizeof(RTL_USER_PROCESS_PARAMETERS));
	RtlSecureZeroMemory(&PsAttributeList, sizeof(PS_ATTRIBUTE_LIST) - sizeof(PS_ATTRIBUTE));
	RtlSecureZeroMemory(&PsCreateInfo,    sizeof(PS_CREATE_INFO));

	RtlUserProcessParameters.Length = sizeof(RTL_USER_PROCESS_PARAMETERS);
	RtlUserProcessParameters.MaximumLength = sizeof(RTL_USER_PROCESS_PARAMETERS);

	PsAttributeList.TotalLength = sizeof(PS_ATTRIBUTE_LIST) - sizeof(PS_ATTRIBUTE);
	PsCreateInfo.Size = sizeof(PS_CREATE_INFO);

	RtlUserProcessParameters.Environment = (WCHAR*)Environment;
	RtlUserProcessParameters.EnvironmentSize = sizeof(Environment);
	RtlUserProcessParameters.EnvironmentVersion = 0;
	RtlUserProcessParameters.Flags = RTL_USER_PROCESS_PARAMETERS_NORMALIZED;

	PsAttributeList.Attributes[0].Attribute = PsAttributeValue(PsAttributeImageName, FALSE, TRUE, FALSE);
	PsAttributeList.Attributes[0].Size = ProcessImageName.Length;
	PsAttributeList.Attributes[0].Value = (ULONG_PTR)ProcessImageName.Buffer;



	ModuleBase = GetModuleHandle(_T("ntdll.dll"));

	NtCreateUserProcess =
		(LPFN_NTCREATEUSERPROCESS)GetProcAddress(ModuleBase, "NtCreateUserProcess");

	NtResumeThread = 
		(LPFN_NTRESUMETHREAD)GetProcAddress(ModuleBase, "NtResumeThread");

	//Ndlld.dll
	Status = NtCreateUserProcess(
					&ProcessHandle,
					&ThreadHandle,
					MAXIMUM_ALLOWED, 
					MAXIMUM_ALLOWED, 
					NULL, 
					NULL,
					NULL, 
					THREAD_CREATE_FLAGS_CREATE_SUSPENDED,
					&RtlUserProcessParameters,
					&PsCreateInfo,
					&PsAttributeList
				);

	Status = NtResumeThread(ThreadHandle, &PreviousSuspendCount);

	return;
}

void CallShellExecute()
{
	HINSTANCE rVal = NULL;

	rVal = ShellExecute(
				NULL, 
				_T("open"), 
				_T("Test.exe"), 
				NULL, 
				NULL, 
				SW_NORMAL
			);//打开exe
	if (rVal < (HINSTANCE)32)//检测是否指定成功
		MessageBox(NULL, _T("ERROR"), NULL, MB_OK);

	rVal = ShellExecute(
				NULL, 
				_T("open"), 
				_T("Windows核心编程.pdf"), 
				NULL, 
				NULL, 
				SW_NORMAL
			);//打开指定文件，将调用默认处理的程序打开
	if (rVal < (HINSTANCE)32)
	{
		MessageBox(NULL, _T("ERROR"), NULL, MB_OK);
	}
	rVal = ShellExecute(
				NULL, 
				_T("open"), 
				_T("https://www.baidu.com"), 
				NULL, 
				NULL, 
				SW_NORMAL
			);//打开网址
	if (rVal < (HINSTANCE)32)
	{
		MessageBox(NULL, _T("ERROR"), NULL, MB_OK);
	}
	rVal = ShellExecute(
				NULL, 
				_T("open"), 
				_T("c:\\windows"), 
				NULL, 
				NULL, 
				SW_NORMAL
			);//打开文件夹
	if (rVal < (HINSTANCE)32)
	{
		MessageBox(NULL, _T("ERROR"), NULL, MB_OK);
	}

	rVal = ShellExecute(
				NULL, 
				_T("runas"), 
				_T("cmd.exe"), 
				NULL, 
				NULL, 
				SW_NORMAL
			);//请求管理员权限打开cmd
	if (rVal < (HINSTANCE)32)
	{
		MessageBox(NULL, _T("ERROR"), NULL, MB_OK);
	}
	//ShellExecute不仅可以运行EXE文件，也可以运行已经关联的文件。比如网页  文件夹 pdf文档等等
	return;
}

void CallWinExec()
{
	UINT rVal = 0;
	rVal = WinExec("notepad HelloWorld.txt", SW_SHOWNORMAL);
	if (rVal < 32)//检测是否成功
		MessageBox(NULL, _T("ERROR"), NULL, MB_OK);
	/*1)WinExec() 函数原型：
		UINT WinExec(LPCSTR lpCmdLine, UINT uCmdShow);
	参数说明：lpCmdLine：以0结尾的字符串，命令行参数。
		uCmdShow : 新的应用程序的运行方式。其取值如下：
		SW_HIDE 隐藏
		SW_MAXIMIZE 最大化
		SW_MINIMIZE 最小化，并把Z order顺序在此窗口之后（即窗口下一层）的窗口激活
		SW_RESTORE 激活窗口并还原为初始化大小 SW_SHOW 以当前大小和状态激活窗口
		SW_SHOWDEFAULT 以默认方式运行
		SW_SHOWMAXIMIZED 激活窗口并最大化
		SW_SHOWMINIMIZED 激活窗口并最小化
		SW_SHOWMINNOACTIVE 最小化但不改变当前激活的窗口
		SW_SHOWNA 以当前状态显示窗口但不改变当前激活的窗口
		SW_SHOWNOACTIVATE 以初始化大小显示窗口但不改变当前激活的窗口
		SW_SHOWNORMAL 激活并显示窗口，如果是最大(小)化，窗口将会还原。第一次运行程序 时应该使用这个值

		如果调用成功，这个函数会返回一个不小于31的值，否则调用失败，其返回值的意义如下：
		0 系统内存或资源不足
		ERROR_BAD_FORMAT .EXE文件格式无效（比如不是32位应用程序）
		ERROR_FILE_NOT_FOUND 指定的文件设有找到
		ERROR_PATH_NOT_FOUND 指定的路径没有找到

		限制是 WinExec主要运行EXE文件。*/

}

void CallShellExecuteEx()
{
	/*
	要使用ShellExecute或ShellExecuteEx，您的应用程序必须指定要执行的文件或文件夹对象，以及指定操作的动词。
	对于ShellExecute，将这些值分配给适当的参数。对于ShellExecuteEx，填写一个SHELLEXECUTEINFO结构的适当成员。
	如果该函数成功，它将SHELLEXECUTEINFO结构的hInstApp成员设置为该函数启动的应用程序的实例句柄。
   */
	SHELLEXECUTEINFO ShellExecuteInfo;
	ZeroMemory(&ShellExecuteInfo, sizeof(ShellExecuteInfo));
	ShellExecuteInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShellExecuteInfo.fMask  = SEE_MASK_NOCLOSEPROCESS;
	ShellExecuteInfo.hwnd   = NULL;
	ShellExecuteInfo.lpVerb = _T("open");
	ShellExecuteInfo.lpFile = _T("Test.exe");
	ShellExecuteInfo.lpParameters = NULL;
	ShellExecuteInfo.lpDirectory  = NULL;
	ShellExecuteInfo.nShow    = SW_NORMAL;
	ShellExecuteInfo.hInstApp = NULL;
	//如果该函数失败，hInstApp是一个表示失败原因的seerr错误值之一。
	//（实例句柄总是大于32，错误值小于32）
	ShellExecuteEx(&ShellExecuteInfo);
	WaitForSingleObject(ShellExecuteInfo.hProcess, INFINITE);
	//注意，seerr错误值是为了与ShellExecute函数兼容;使用GetLastError函数来检索错误信息。GetLastError返回的错误值对应于seerr值，并且是下表中描述的值之一。
	return;
}

void CallCreateProcessInternal()
{
	HANDLE TokenHandle = NULL;
	HANDLE DuplicateTokenHandle = NULL;
	HANDLE hProcess  = NULL;//得到当前进程句柄
	DWORD  SessionId = NULL;
	HMODULE     Kernel32Module = NULL; 
	STARTUPINFO         StartupInfo;
	PROCESS_INFORMATION ProcessInfo;
	TCHAR ApplicationName[] = _T("CFFExplorer.exe");
	LPFN_CREATEPROCESSINTERNAL CreateProcessInternal = NULL;
	BOOL  rVal = FALSE;

	ZeroMemory(&StartupInfo, sizeof(StartupInfo));
	ZeroMemory(&ProcessInfo, sizeof(ProcessInfo));
	hProcess  = GetCurrentProcess();//得到当前进程句柄
	SessionId = WTSGetActiveConsoleSessionId();

	OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &TokenHandle);//得到进程的令牌句柄
	rVal = DuplicateTokenEx(
				TokenHandle, 
				MAXIMUM_ALLOWED, 
				NULL, 
				SecurityIdentification, 
				TokenPrimary, 
				&DuplicateTokenHandle
			);
	if (!rVal)//复制令牌
	{
		_tprintf_s(_T("%d"), GetLastError());
		CloseHandle(TokenHandle);
		return;
	}
	//设置令牌属性
	SetTokenInformation(DuplicateTokenHandle, TokenSessionId, &SessionId, sizeof(DWORD));

	Kernel32Module = LoadLibrary(_T("kernel32.dll"));

	GetStartupInfo(&StartupInfo);

	if (Kernel32Module)
	{
#ifdef _UNICODE
		CreateProcessInternal =
			(LPFN_CREATEPROCESSINTERNAL)GetProcAddress(Kernel32Module, "CreateProcessInternalW");
#else
		CreateProcessInternal =
			(LPFN_CREATEPROCESSINTERNAL)GetProcAddress(Kernel32Module, "CreateProcessInternalA");
#endif
		if (CreateProcessInternal)
		{
			CreateProcessInternal(
					DuplicateTokenHandle,
					ApplicationName,
					0,
					0,
					0,
					1,
					0,
					0,
					0,
					&StartupInfo,
					&ProcessInfo,
					0
				);
		}
		FreeLibrary(Kernel32Module);
		Kernel32Module = NULL;
	}
	return;
}


