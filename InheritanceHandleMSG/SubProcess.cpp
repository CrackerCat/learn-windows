#include <windows.h>
#include <tchar.h>
#include <iostream>
#include "resource.h"

using namespace std;


LRESULT CALLBACK CallBackRoutine(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
#define  UM_SEND  (WM_USER + 0x0010)
HANDLE __EventHandle = INVALID_HANDLE_VALUE;


int WINAPI _tWinMain(
	HINSTANCE _Instance,
	HINSTANCE _PrevInstance,
	PTSTR     _CmdLine,
	INT       _CmdShow
)
{
	DialogBoxParam(
			_Instance, 
			MAKEINTRESOURCE(IDD_DIALOG), 
			NULL, 
			(DLGPROC)CallBackRoutine, 
			NULL
		);

	return 0;
}

LRESULT CALLBACK CallBackRoutine(HWND _hWnd, UINT _Message, WPARAM _wParam, LPARAM _lParam)
{
	switch (_Message)
	{
	case WM_COMMAND:   
		DWORD msg = LOWORD(_wParam);     //low word  high wrod
		switch (msg)
		{
		case IDOK:
			if (__EventHandle != INVALID_HANDLE_VALUE)
				SetEvent(__EventHandle);
			break;
		case IDCANCEL:
			EndDialog(_hWnd, 0);
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(_hWnd, 0);
		break;
	case UM_SEND:   
		__EventHandle = (HANDLE)_lParam;
		TCHAR BufferData[20] { 0 };
		_stprintf_s(BufferData, _T("0x%p"), _lParam);
		SetDlgItemText(_hWnd, IDC_STATIC_SHOW, BufferData);
	}
	return 0;
}
