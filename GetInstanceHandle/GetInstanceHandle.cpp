#include <cstdio>
#include <cstdlib>
#include <tchar.h>
#include <windows.h>
using namespace std;



void GMH_NULL();


extern "C" PVOID  __ImageBase;


//int _tmain(int _argc, TCHAR* _argv[], TCHAR* _envp[])
//{
//
//	TCHAR BufferData[0x1000] = { 0 };
//
//	HMODULE  hModuleHandle = (HMODULE)&__ImageBase;
//
//	_stprintf_s(BufferData, _T("0x%p"), hModuleHandle);
//	OutputDebugString(BufferData);
//
//	GMH_NULL();
//	_tprintf(_T("Input AnyKey To Exit\r\n"));
//	_gettchar();
//
//	return 0;
//}

int WINAPI _tWinMain(
	HINSTANCE  _hInstanceExe,
	HINSTANCE  _hPrevInstanceExe,
	PTSTR      _pszCmdLine,
	int        _nCmdShow
)
{

	TCHAR BufferData[0x1000];
	ZeroMemory(BufferData, sizeof(BufferData));

	_stprintf_s(BufferData, _T("0x%p"), _hInstanceExe);

	OutputDebugString(BufferData);

	GMH_NULL();

	return 0;
}


void GMH_NULL()
{
	HMODULE hModuleBase = GetModuleHandle(NULL);

	TCHAR BufferData[0x1000];
	ZeroMemory(BufferData, sizeof(BufferData));
	_stprintf_s(BufferData, _T("0x%p"), hModuleBase);
	OutputDebugString(BufferData);
	return;
}
