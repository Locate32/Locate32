////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////

#include "HFCLib.h"





#if defined(DEF_WCHAR) && !defined(_WIN64) && !defined(MICROSOFT_LAYER_FOR_UNICODE)
BOOL bIsFullUnicodeSupport=(GetVersion()&0x80000000)?FALSE:TRUE;
#endif

void CAppData::stdfunc()
{
#if defined(DEF_WCHAR) && !defined(_WIN64) && !defined(MICROSOFT_LAYER_FOR_UNICODE)
	bIsFullUnicodeSupport=(GetVersion()&0x80000000)?FALSE:TRUE;
#endif
}



////////////////////////////
// WinMain
////////////////////////////

#ifdef DEF_APP

int PASCAL WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,
	 LPTSTR lpszCmdLine,int nCmdShow)
{

	int nResult;
	if (GetAppData()->pAppClass!=NULL)
	{
		if (GetAppData()->m_pThreads!=NULL)
		{
			DebugMessage("WinMain(): m_pThreads already allocated?");
		}
		GetAppData()->m_pThreads=new CList<CWinThread*>;
		if (GetAppData()->m_pThreads==NULL)
		{
			MessageBox(NULL,"Cannot Allocate Memory",szError,MB_ICONSTOP|MB_OK);
			return FALSE;
		}
		GetAppData()->m_pThreads->AddTail(GetAppData()->pAppClass);
		GetAppData()->hAppInstance=hInstance;
		SetResourceHandle(hInstance,SetBoth);
		GetAppData()->pAppClass->m_szCmdLine=lpszCmdLine;
		GetAppData()->pAppClass->m_nCmdShow=nCmdShow;
		
		StartDebugLogging();
		GetAppData()->pAppClass->InitApplication();
		if (GetAppData()->pAppClass->InitInstance())
			GetAppData()->pAppClass->ModalLoop();	
		nResult=GetAppData()->pAppClass->ExitInstance();

		GetAppData()->m_pThreads->RemoveAll();
		delete GetAppData()->m_pThreads;
		GetAppData()->m_pThreads=NULL;
		return nResult;
	}
	else
	{
		MessageBox(NULL,"You have to create CWinApp based class.","Program Error",MB_ICONSTOP|MB_OK);
		DebugMessage("No any CWinApp class");
		return 1;
	}
}

#endif

#ifdef DLL
int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	return 0;
}
#endif
