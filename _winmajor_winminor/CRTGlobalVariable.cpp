#include <cstdio>
#include <cstdlib>
#include <tchar.h>
#include <Windows.h>

extern unsigned int _winmajor;
extern unsigned int _winminor;
extern unsigned int _osver;
extern unsigned int _winver;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	PCTSTR OSName = NULL;
	PCTSTR BuildNumber = NULL;
	TCHAR  BufferData[0x1000];

	ZeroMemory(BufferData, sizeof(BufferData));

	switch (_winmajor)
	{
	case 1:
	case 2:
	case 3:
	case 4:	OSName = _T("WindowsNT"); break;
	case 5:
	{
		switch (_winminor)
		{
		case 0:	OSName = _T("Windows2000");  break;
		case 1:	OSName = _T("WindowsXP");    break;
		case 2:	OSName = _T("Windows2003");  break;
		default:
			OSName = _T("Unknown");
			break;
		}
		break;
	}
	case 6:
	{
		switch (_winminor)
		{
		case 0:	OSName = _T("WindowsVista");  break;
		case 1:	OSName = _T("Windows7");      break;
		case 2:	OSName = _T("Windows10");     break;
		default:
			OSName = _T("Unknown");
			break;
		}
		break;
	}
	default:
		OSName = _T("Unknown");
		break;
	}



	_stprintf_s(BufferData, _T("Os:%s,Version:%d\r\n"), OSName, _osver);

	OutputDebugString(BufferData);

	switch (_winver)
	{
	case 256:
	case 512:
	case 768:
	case 1024:  OSName = _T("WindowsNT"   );  break;
	case 1280:  OSName = _T("Windows2000" );  break;
	case 1281:  OSName = _T("WindowsXP"   );  break;
	case 1282:  OSName = _T("Windows2003" );  break;
	case 1536:  OSName = _T("WindowsVista");  break;
	case 1537:  OSName = _T("Windows7"    );  break;
	case 1538:  OSName = _T("Windows10"   );  break;
	default:    OSName = _T("Unknown"     );  break;
	}

	switch (_osver)
	{
	case 528:   BuildNumber = _T("3.1" );	break;
	case 807:   BuildNumber = _T("3.5" );	break;
	case 1057:  BuildNumber = _T("3.51");	break;
	case 1381:  BuildNumber = _T("4.0" );	break;
	case 2195:  BuildNumber = _T("NULL");	break;
	case 2600:  BuildNumber = _T("NULL");	break;
	case 3790:  BuildNumber = _T("NULL");	break;
	case 6000:  BuildNumber = _T("NULL");	break;
	case 6001:  BuildNumber = _T("SP1" );	break;
	case 6002:  BuildNumber = _T("SP2" );	break;
	case 7600:  BuildNumber = _T("NULL");	break;
	case 7601:  BuildNumber = _T("SP1" );	break;
	case 9200:  BuildNumber = _T("NULL");	break;

	}

	_tprintf_s(_T("Os:%s Version:%d\n"), OSName, _osver);
	_tprintf_s(_T("Build Version:%s\n"), BuildNumber);

	_tprintf_s(_T("Input AnyKey To Exit\r\n"));
	_gettchar();

	return 0;

}
