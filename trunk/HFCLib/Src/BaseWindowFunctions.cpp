////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////

#include "HFCLib.h"


#if defined(WIN32)

COLORREF GetSystemColor(LPCSTR szKey)
{
	CRegKey RegKey;
	if (RegKey.OpenKey(HKCU,"Control Panel\\Colors",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
	{
		RGBTRIPLE rgb;
		char szBuffer[15];
		ULONG_PTR i;
		RegKey.QueryValue(szKey,szBuffer,15);
		for (i=0;szBuffer[i]!='\0';i--)
		{
			if (szBuffer[i]==' ')
				szBuffer[i]=0;
		}
		rgb.rgbtRed=atoi(szBuffer);
		i=istrlen(szBuffer)+1;
		rgb.rgbtGreen=atoi(szBuffer+i);
		i+=istrlen(szBuffer+i)+1;
		rgb.rgbtBlue=atoi(szBuffer+i);
		return RGB(rgb.rgbtRed,rgb.rgbtGreen,rgb.rgbtBlue);	
	}
	return (COLORREF)-1;
}

#endif


////////////////////////////
// Class CWinApp
////////////////////////////

#ifdef DEF_APP

CWinApp::CWinApp(LPCTSTR lpszAppName)
:	m_szAppName(lpszAppName),m_szCmdLine(NULL),m_szHelpFile(NULL)
{
#ifdef _DEBUG_LOGGING
	extern CRITICAL_SECTION cHandleCriticalSection;
	InitializeCriticalSection(&cHandleCriticalSection);
#endif

	GetAppData()->pAppClass=this;
	GetAppData()->hCommonResourceHandle=NULL;
	GetAppData()->hLanguageSpecificResourceHandle=NULL;
	GetAppData()->m_pCommonDialog=NULL;
	m_hThread=GetCurrentThread();
	m_nThreadID=GetCurrentThreadId();
}

CString CWinApp::GetExeName() const
{
	DWORD len;
	CHAR szExeName[MAX_PATH];
	len=GetModuleFileName(GetInstanceHandle(),szExeName,MAX_PATH);
	return CString(szExeName,len);
}

#ifdef DEF_WCHAR
CStringW CWinApp::GetExeNameW() const
{
	if (IsUnicodeSystem())
	{
		DWORD len;
		WCHAR szExeName[MAX_PATH];
		len=GetModuleFileNameW(GetInstanceHandle(),szExeName,MAX_PATH);
		return CStringW(szExeName,len);
	}

	return CStringW(GetExeName());
}

void CWinApp::SetHelpFile(LPCSTR szHelpFile)
{
	if (m_szHelpFile!=NULL)
		delete[] m_szHelpFile;
	if (szHelpFile!=NULL)
		m_szHelpFile=alloccopyAtoW(szHelpFile);
	else
		m_szHelpFile=NULL;
}

void CWinApp::SetHelpFile(LPCWSTR szHelpFile)
{
	if (m_szHelpFile!=NULL)
		delete[] m_szHelpFile;
	if (szHelpFile!=NULL)
		m_szHelpFile=alloccopy(szHelpFile);
	else
		m_szHelpFile=NULL;
}
#endif


BOOL CWinApp::InitApplication()
{
	return FALSE;
}

CWinApp::~CWinApp()
{
	m_hThread=NULL;
	m_nThreadID=0;

	
	if (m_szHelpFile!=NULL)
		delete[] m_szHelpFile;

	
#ifdef _DEBUG_LOGGING
	extern CRITICAL_SECTION cHandleCriticalSection;
	void DebugClearOpenHandlesList();
	DebugLogOpenHandles();
	DebugClearOpenHandlesList();	
	DeleteCriticalSection(&cHandleCriticalSection);
#endif


	StopDebugLogging();
}


#endif
#ifdef DEF_RESOURCES

HINSTANCE SetResourceHandle(HINSTANCE hHandle,TypeOfResourceHandle bType)
{
	HINSTANCE hOldHandle=GetAppData()->hCommonResourceHandle;
	if (bType==SetBoth || bType==CommonResource)
		GetAppData()->hCommonResourceHandle=hHandle;
	if (bType==SetBoth || bType==LanguageSpecificResource)
		GetAppData()->hLanguageSpecificResourceHandle=hHandle;
	return hOldHandle;
}

#endif


