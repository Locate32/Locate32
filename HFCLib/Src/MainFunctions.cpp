////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////

#include "HFCLib.h"



// Library variables
#define  HFCVERSIONMS		MAKEVERSION(6,50)
#define  HFCVERSIONLS		MAKEVERSION(10,8220)

#ifdef _DEBUG
#define  HFCVERSIONSTR		"HFC Library (DEBUG) v6.50.10.8220";
#else
#define  HFCVERSIONSTR		"HFC Library v6.50.10.8220";
#endif

LPCSTR szEmpty="";
LPCSTR szError="Error";
#ifdef DEF_WCHAR
LPCWSTR szwEmpty=L"";
LPCWSTR	szwError=L"Error";
#endif

DWORD GetHFCLibVerMS(void)
{
	return HFCVERSIONMS;
}

DWORD GetHFCLibVerLS(void)
{
	return HFCVERSIONLS;
}

ULONGLONG GetHFCLibVer(void)
{
	return ((((ULONGLONG)HFCVERSIONMS)<<32)|HFCVERSIONLS);
}

LPCSTR GetHFCLibStr(void)
{
	return HFCVERSIONSTR;
}

DWORD GetHFCLibFlags(void)
{
	return 0
#ifdef WIN32
	|HFCFLAG_WIN32
#endif
#ifdef DLL
	|HFCFLAG_DLL
#endif
#ifdef _DEBUG
	|HFCFLAG_DEBUG
#endif
#ifdef CONSOLE
	|HFCFLAG_CONSOLE
#endif
#ifdef DEF_WCHAR
	|HFCFLAG_WCHAR
#endif
#ifdef DEF_RESOURCES
	|HFCFLAG_RESOURCES
#endif
#ifdef DEF_WINDOWS
	|HFCFLAG_WINDOWS
#endif
#ifdef DEF_COM
	|HFCFLAG_COM
#endif
	;
}


#ifdef _DEBUG
void MsgToText(DWORD msg,LPSTR text,DWORD maxlen)
{
#ifdef WIN32
	switch (msg)
	{
	case WM_ACTIVATE:
		StringCbCopy(text,maxlen,"WM_ACTIVATE");
		break;
	case WM_ACTIVATEAPP:
		StringCbCopy(text,maxlen,"WM_ACTIVATEAPP");
		break;
	case WM_CANCELMODE:
		StringCbCopy(text,maxlen,"WM_CANCELMODE");
		break;
	case WM_CAPTURECHANGED:
		StringCbCopy(text,maxlen,"WM_CAPTURECHANGED");
		break;
	case WM_CHAR:
		StringCbCopy(text,maxlen,"WM_CHAR");
		break;
	case WM_CLOSE:
		StringCbCopy(text,maxlen,"WM_CLOSE");
		break;
	case WM_CREATE:
		StringCbCopy(text,maxlen,"WM_CREATE");
		break;
	case WM_COMMAND:
		StringCbCopy(text,maxlen,"WM_COMMAND");
		break;
	case WM_DESTROY:
		StringCbCopy(text,maxlen,"WM_DESTROY");
		break;
	case WM_DRAWITEM:
		StringCbCopy(text,maxlen,"WM_DRAWITEM");
		break;
	case WM_DROPFILES:
		StringCbCopy(text,maxlen,"WM_DROPFILES");
		break;
	case WM_GETICON:
		StringCbCopy(text,maxlen,"WM_GETICON");
		break;
	case WM_HSCROLL:
		StringCbCopy(text,maxlen,"WM_HSCROLL");
		break;
	case WM_KEYDOWN:
		StringCbCopy(text,maxlen,"WM_KEYDOWN");
		break;
	case WM_KEYUP:
		StringCbCopy(text,maxlen,"WM_KEYUP");
		break;
	case WM_KILLFOCUS:
		StringCbCopy(text,maxlen,"WM_KILLFOCUS");
		break;
	case WM_MENUSELECT:
		StringCbCopy(text,maxlen,"WM_MENUSELECT");
		break;
	case WM_MOUSEMOVE:
		StringCbCopy(text,maxlen,"WM_MOUSEMOVE");
		break;
	case WM_MOVE:
		StringCbCopy(text,maxlen,"WM_MOVE");
		break;
	case WM_NOTIFY:
		StringCbCopy(text,maxlen,"WM_NOTIFY");
		break;
	case WM_PAINT:
		StringCbCopy(text,maxlen,"WM_PAINT");
		break;
	case WM_SETICON:
		StringCbCopy(text,maxlen,"WM_SETICON");
		break;
	case WM_SIZE:
		StringCbCopy(text,maxlen,"WM_SIZE");
		break;
	case WM_SIZING:
		StringCbCopy(text,maxlen,"WM_SIZING");
		break;
	case WM_TIMER:
		StringCbCopy(text,maxlen,"WM_TIMER");
		break;
	case WM_VSCROLL:
		StringCbCopy(text,maxlen,"WM_VSCROLL");
		break;
	default:
		StringCbPrintf(text,maxlen,"%lX",msg);
		break;
	}
#else
	sprintf(text,"%lX",msg);
#endif
}

#endif

#ifdef WIN32

DWORD GetFileVersion(LPCSTR szFile,DWORD* dwFileVersionLo)
{
	DWORD dwHandle;
	DWORD nLen=GetFileVersionInfoSize((char*)szFile,&dwHandle);
	if (!nLen)
		return 0;

	BYTE* pVersion=new BYTE[nLen+2];
	if (GetFileVersionInfo((char*)szFile,0,nLen,pVersion))
	{
		VS_FIXEDFILEINFO* pffi;
		UINT nflen;
		VerQueryValue(pVersion,"\\",(void**)&pffi,&nflen);
		if (dwFileVersionLo!=NULL)
			*dwFileVersionLo=pffi->dwFileVersionLS;	
		delete[] pVersion;
		return pffi->dwFileVersionMS;
	}
	delete[] pVersion;
	return 0;
}
	
#endif