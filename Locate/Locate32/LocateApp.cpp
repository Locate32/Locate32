/* Locate32 - Copyright (c) 1997-2010 Janne Huttunen */

#include <HFCLib.h>
#include "Locate32.h"
#include "../common/win95crtfixes.h"

//#include "wfext.h"

//#include <pdh.h>
//#pragma comment(lib, "pdh.lib")

//#include "3rdparty/cpuusage.h"



////////////////////////////////////////////////////////////
// CLocateApp - static variables

UINT CLocateApp::m_nHFCInstallationMessage=0;
UINT CLocateApp::m_nTaskbarCreated=0;
UINT CLocateApp::m_nLocateAppMessage=0;



////////////////////////////////////////////////////////////
// CLocateApp - member functions

CLocateApp::CLocateApp()
:	CWinApp("LOCATE32"),m_nDelImage(0),m_nStartup(0),
	m_ppUpdaters(NULL),m_pLastDatabase(NULL),m_nFileSizeFormat(fsfOverKBasKB),
	m_dwProgramFlags(pfDefault),m_nInstance(0),m_szCommonRegFile(NULL),
	m_szCommonRegKey(NULL),m_pLocaleNumberFormat(NULL)
{
	DebugMessage("CLocateApp::CLocateApp()");
	m_pStartData=new CStartData;

	InitializeCriticalSection(&m_cUpdatersPointersInUse);
}

CLocateApp::~CLocateApp()
{
	DebugMessage("CLocateApp::~CLocateApp()");
	if (m_pStartData!=NULL)
	{
		delete m_pStartData;
		m_pStartData=NULL;
	}

	if (m_pLocaleNumberFormat!=NULL)
	{
		delete m_pLocaleNumberFormat;
		m_pLocaleNumberFormat=NULL;
	}
	
	EnterCriticalSection(&m_cUpdatersPointersInUse);
	ASSERT(m_ppUpdaters==NULL);
	LeaveCriticalSection(&m_cUpdatersPointersInUse);

	DeleteCriticalSection(&m_cUpdatersPointersInUse);
}

BOOL CALLBACK CLocateApp::EnumLocateSTWindows(HWND hwnd,LPARAM lParam)
{
	char szClass[101];
	GetClassName(hwnd,szClass,100);

	if (strcmp(szClass,"LOCATEAPPST")==0)
		++*((DWORD*)lParam);
	return TRUE;	
}

BOOL CLocateApp::InitInstance()
{
	CWinApp::InitInstance();
	LPCWSTR pCommandLine;
	
	//DebugSetFlags(DebugLogHandleOperations);

	//extern BOOL bIsFullUnicodeSupport;
	//bIsFullUnicodeSupport=FALSE;

	DebugFormatMessage("CLocateApp::InitInstance(), thread is 0x%X",GetCurrentThreadId());

	// Initializing HFC library
	if (!InitializeHFC())
	{
		MessageBox(NULL,"InitializeHFC() returned FALSE","ERROR",MB_ICONSTOP);
		m_nStartup|=CStartData::startupExitedBeforeInitialization;
		return FALSE;
	}
	
	// Initializing locater library
	InitLocaterLibrary();
	
	// Initialize buffers
	InitializeBuffers();


	if (!IsUnicodeSystem())
		m_pGetLongPathName=CLocateApp::GetLongPathNameNoUni;
	else 
	{
		m_pGetLongPathName=(DWORD(WINAPI *)(LPCWSTR,LPWSTR,DWORD))GetProcAddress(GetModuleHandle("kernel32.dll"),"GetLongPathNameW");
		if (m_pGetLongPathName==NULL)
			m_pGetLongPathName=CLocateApp::GetLongPathName;
	}
	
	// Set messages
	m_nHFCInstallationMessage=RegisterWindowMessage("HFCINSTALLMESSAGE");
	m_nTaskbarCreated=RegisterWindowMessage("TaskbarCreated");
	m_nLocateAppMessage=RegisterWindowMessage("Locate32Communication");

	// Enumerate instances
	EnumWindows(EnumLocateSTWindows,(LPARAM)&m_nInstance);

	// Handling command line arguments
	pCommandLine=GetCommandLineW();
	while (*pCommandLine==L' ') pCommandLine++;
	if (*pCommandLine==L'\"')
	{
		pCommandLine++;
		while (*pCommandLine!=L'\"' && *pCommandLine!=L'\0') 
			pCommandLine++;
		if (*pCommandLine==L'\"')
			pCommandLine++;
	}
	else
	{
		while (*pCommandLine!=L' ' && *pCommandLine!=L'\0') 
			pCommandLine++;
	}			
	while (*pCommandLine==L' ') pCommandLine++;
	
	DebugFormatMessage("CommandLine: %S",pCommandLine);
	InitCommonRegKey(pCommandLine);
	ParseParameters(pCommandLine,m_pStartData);



	m_nStartup=m_pStartData->m_nStartup;
	
	// If databases are specified by command line, use it
	if (m_pStartData->m_nStartup&CStartData::startupDatabasesOverridden)
	{
		m_aDatabases.Swap(m_pStartData->m_aDatabases);
		if (m_aDatabases.GetSize()>0)
			m_pLastDatabase=m_aDatabases[0];
	}
	CheckDatabases();

	// Setting prioriy if needed
	if (m_pStartData->m_nPriority!=CStartData::priorityDontChange)
		SetPriorityClass(GetCurrentProcess(),m_pStartData->m_nPriority);


	// Setting up language files
	if (!SetLanguageSpecifigHandles())
	{
		m_nStartup|=CStartData::startupExitedBeforeInitialization;
		return FALSE;
	}


	// Load date and time format strings from registry	
	LoadRegistry();

	
	// Startup updategin if '-U' argument is given
	if (m_pStartData->m_nStartup&CStartData::startupUpdate && 
		m_pStartData->m_nStartup&CStartData::startupDoNotOpenDialog &&
		(m_pStartData->m_nStartup&CStartData::startupLeaveBackground)==0)
	{
		// Starting updating and exiting
		m_nStartup|=CStartData::startupExitedBeforeInitialization;

		if (GlobalUpdate())
		{
			while (IsUpdating())
				Sleep(100);
		}
		return FALSE;
	}
	
	// Chechkin whether locate32 is already running
	if ((m_pStartData->m_nStartup&CStartData::startupNewInstance)==0)
	{
		if (ActivateOtherInstances(pCommandLine))
		{
			m_nStartup|=CStartData::startupExitedBeforeInitialization;
			return FALSE;
		}
	}


	
	// Ppreventing error messages when e.g. CDROM is not available
	SetErrorMode(SEM_NOOPENFILEERRORBOX|SEM_FAILCRITICALERRORS); 
    
	// Initialize common controls
	INITCOMMONCONTROLSEX icex;
	icex.dwSize=sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC=ICC_DATE_CLASSES|ICC_USEREX_CLASSES|ICC_WIN95_CLASSES;
	InitCommonControlsEx(&icex);
	
	// Get versions of comctr32.dll and shell32.dll
	DWORD dwTemp=GetFileVersion("comctl32.dll");
	m_wComCtrlVersion=WORD(dwTemp>>8|LOBYTE(dwTemp));
	dwTemp=GetFileVersion("shell32.dll");
	m_wShellDllVersion=WORD(dwTemp>>8|LOBYTE(dwTemp));
	
	// Initializing COM 
	CoInitializeEx(NULL,COINIT_APARTMENTTHREADED);
	
	
	
	
	// Retrieving default icons
	SetDeleteAndDefaultImage();

	// Registering window class for notify icon handler window
	RegisterWndClass("LOCATEAPPST",0,NULL,NULL,NULL,NULL);

	m_pMainWnd=&m_AppWnd;
	RECT rc={-10,-10,-9,-9};
	m_AppWnd.Create("LOCATEAPPST","Locate ST",WS_OVERLAPPED,&rc,0,(UINT)0,0);
	
	return TRUE;
}

int CLocateApp::ExitInstance()
{
	DebugMessage("CLocateApp::ExitInstance()");

	CWinApp::ExitInstance();

	if (!(m_nStartup&CStartData::startupExitedBeforeInitialization))
	{
		// Savind date and time format strings
		SaveRegistry();

		
		// Unitializing COM
		// Use expection handling, because some extensions may cause Locate32
		// to crash (maybe this does not help at all, but let's try anyway)
		try {
			CoUninitialize();

		}
		catch(...)
		{
			DWORD dwIgnoreMessage=FALSE;

			CRegKey2 RegKey;
			if (RegKey.OpenKey(HKCU,"\\General",
				CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
			{
				RegKey.QueryValue("Ignore CoUnitialize error",dwIgnoreMessage);
				RegKey.CloseKey();
			}
		
			if (!dwIgnoreMessage)
			{
				ForceForegroundAndFocus(NULL);
				if (::MessageBoxW(NULL,ID2W(IDS_ERRORCOUNINITIALIZE),ID2W(IDS_ERROR),MB_YESNO|MB_ICONERROR|MB_SETFOREGROUND)==IDNO)
				{
					if (RegKey.OpenKey(HKCU,"\\General",
					CRegKey::openExist|CRegKey::samWrite)==ERROR_SUCCESS) 
					{
						RegKey.SetValue("Ignore CoUnitialize error",DWORD(TRUE));
						RegKey.CloseKey();
					}
				}
			}
		}
		
		
	}


	if (GetLanguageSpecificResourceHandle()!=GetInstanceHandle())
	{
		FreeLibrary(GetLanguageSpecificResourceHandle());
		SetResourceHandle(GetInstanceHandle(),SetBoth);
	}

	FinalizeCommonRegKey();

	// DeInitialize buffers
	DeinitializeBuffers();

	return 0;
}

INT_PTR CALLBACK CLocateApp::DummyDialogProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	return FALSE;
}

DWORD CLocateApp::GetLongPathName(LPCWSTR lpszShortPath,LPWSTR lpszLongPath,DWORD cchBuffer)
{
	LPWSTR pTemp;
	return GetFullPathNameW(lpszShortPath,cchBuffer,lpszLongPath,&pTemp);
}

DWORD CLocateApp::GetLongPathNameNoUni(LPCWSTR lpszShortPath,LPWSTR lpszLongPath,DWORD cchBuffer)
{
	LPSTR pTmp;
	CHAR sPathA[MAX_PATH+10];
	DWORD dwRet=::GetFullPathNameA(W2A(lpszShortPath),MAX_PATH+10,sPathA,&pTmp);
	
	if (dwRet==0)
		return 0;
	MultiByteToWideChar(CP_ACP,0,sPathA,dwRet+1,lpszLongPath,cchBuffer);
	return dwRet;
}	

BOOL CLocateApp::ParseParameters(LPCWSTR lpCmdLine,CStartData* pStartData)
{
	int idx=0,temp;
	while (lpCmdLine[idx]==L' ') idx++;
	
	if ((lpCmdLine[idx]==L'/' || lpCmdLine[idx]==L'-'))
	{
		switch(lpCmdLine[++idx])
		{
		case L'X': // settings branch, ignore
			idx++;
			if (lpCmdLine[idx]==L':')
				idx++;
			while(lpCmdLine[idx]==L' ') idx++;
			while(lpCmdLine[idx]!=L' ' && lpCmdLine[idx]!='\0') idx++;

			if (lpCmdLine[idx]=='\0')
				return TRUE;
			break;		
		case L'-':
			if (lpCmdLine[++idx]!=L'\0')
			{
				if (lpCmdLine[idx]==L' ')
					idx++;
				ChangeAndAlloc(pStartData->m_pStartString,lpCmdLine+idx);
			}
			return TRUE;
		case L'P': // put 'path' to 'Look in' field
			idx++;
			if (lpCmdLine[idx]==L':')
				idx++;
			while(lpCmdLine[idx]==L' ') idx++;
			if (lpCmdLine[idx]!=L'\"')
			{
				temp=(int)FirstCharIndex(lpCmdLine+idx,L' ');
				ChangeAndAlloc(pStartData->m_pStartPath,lpCmdLine+idx,temp);
				if (temp<0)
					return TRUE;
				idx+=temp;
			}
			else
			{
				idx++;
				temp=(int)FirstCharIndex(lpCmdLine+idx,L'\"');
				ChangeAndAlloc(pStartData->m_pStartPath,lpCmdLine+idx,temp);
				if (temp<0)
					return TRUE;
				idx+=temp+1;
			}
			break;				
		case L'p': // Check also if path is correct
			idx++;
			if (lpCmdLine[idx]==L':')
				idx++;
			while(lpCmdLine[idx]==L' ') idx++;
			if (lpCmdLine[idx]!=L'\"')
			{
				temp=(int)FirstCharIndex(lpCmdLine+idx,L' ');
				if (temp!=-1)
					*(char*)(lpCmdLine+idx+temp)=L'\0'; // Setting line end for 
				int nLength=0;

				WCHAR szPath[MAX_PATH+10];
				WCHAR* tmp;
				nLength=FileSystem::GetFullPathName(lpCmdLine+idx,MAX_PATH,szPath,&tmp);
				ChangeAndAlloc(pStartData->m_pStartPath,szPath,nLength);
								
				if (temp<0)
					return TRUE;
				*(char*)(lpCmdLine+idx+temp)=' '; // Setting line end for 
				idx+=temp;
			}
			else
			{
				idx++;
				temp=(int)FirstCharIndex(lpCmdLine+idx,L'\"');
				if (temp!=-1)
					*(char*)(lpCmdLine+idx+temp)=L'\0'; // Setting line end for 
				int nLength;

				WCHAR szPath[MAX_PATH+10];
				nLength=GetLocateApp()->m_pGetLongPathName(lpCmdLine+idx,szPath,200);
				ChangeAndAlloc(pStartData->m_pStartPath,szPath,nLength);
				if (temp<0)
					return TRUE;
				*(char*)(lpCmdLine+idx+temp)=L'\"'; // Setting line end for 
				idx+=temp+1;
			}
			break;				
		case L'c':
		case L'C':
			{
				OpenClipboard(NULL);
				
				
				HANDLE hData=GetClipboardData(CF_UNICODETEXT);
				if (hData!=NULL)
				{
					LPCWSTR szLine=(LPCWSTR)GlobalLock(hData);
					if (szLine!=NULL)
						ChangeAndAlloc(pStartData->m_pStartString,szLine);
					GlobalUnlock(hData);
				}
				else
				{
					hData=GetClipboardData(CF_TEXT);
					if (hData!=NULL)
					{
						LPCSTR szLine=(LPCSTR)GlobalLock(hData);
						if (szLine!=NULL)
							ChangeAndAlloc(pStartData->m_pStartString,A2W(szLine));
						GlobalUnlock(hData);
					}
				}
				CloseClipboard();
				idx++;
				break;
			}
		case L'T': // put 'type' to 'Extensions' field
		case L't':
			idx++;
			if (lpCmdLine[idx]==L':')
				idx++;
			while(lpCmdLine[idx]==L' ') idx++;
			if (lpCmdLine[idx]!=L'\"')
			{
				temp=(int)FirstCharIndex(lpCmdLine+idx,L' ');
				ChangeAndAlloc(pStartData->m_pTypeString,lpCmdLine+idx,temp);
				if (temp<0)
					return TRUE;
				idx+=temp;
			}
			else
			{
				idx++;
				temp=(int)FirstCharIndex(lpCmdLine+idx,L'\"');
				ChangeAndAlloc(pStartData->m_pTypeString,lpCmdLine+idx,temp);
				if (temp<0)
					return TRUE;
				idx+=temp+1;
			}
			break;				
		case L'D': // activates database named 'name'
			idx++;
			if (lpCmdLine[idx]==L':')
					idx++;
			while(lpCmdLine[idx]==L' ') idx++;
			if (lpCmdLine[idx]!=L'\"')
			{
				temp=(int)FirstCharIndex(lpCmdLine+idx,L' ');

				if (CDatabase::FindByName(pStartData->m_aDatabases,lpCmdLine+idx,temp)==NULL)
				{
					CDatabase* pDatabase=CDatabase::FromName(HKCU,
						GetRegKey("Databases"),lpCmdLine+idx,temp);
					if (pDatabase!=NULL)
					{
						pDatabase->SetThreadId(0);
						pStartData->m_aDatabases.Add(pDatabase);
                        pStartData->m_nStartup|=CStartData::startupDatabasesOverridden;
					}
					if (temp<0)
						return TRUE;
				}
				idx+=temp;
			}
			else
			{
				idx++;
				temp=(int)FirstCharIndex(lpCmdLine+idx,L'\"');
				
				if (CDatabase::FindByName(pStartData->m_aDatabases,lpCmdLine+idx,temp)==NULL)
				{
					CDatabase* pDatabase=CDatabase::FromName(HKCU,
						GetRegKey("Databases"),lpCmdLine+idx,temp);
					if (pDatabase!=NULL)
					{
						pDatabase->SetThreadId(0);
						pStartData->m_aDatabases.Add(pDatabase);
						pStartData->m_nStartup|=CStartData::startupDatabasesOverridden;
					}
				}
				if (temp<0)
					return TRUE;
				idx+=temp+1;
			}
			break;				
		case L'd': // activates database file 'dbfile'
			idx++;
			if (lpCmdLine[idx]==L':')
				idx++;
			while(lpCmdLine[idx]==L' ') idx++;
			if (lpCmdLine[idx]!=L'\"')
			{
				temp=(int)FirstCharIndex(lpCmdLine+idx,L' ');
				if (pStartData->m_aDatabases.GetSize()==1 && wcscmp(pStartData->m_aDatabases[0]->GetName(),L"DEFAULTX")==0)
				{
					pStartData->m_aDatabases[0]->SetNamePtr(alloccopy(L"PARAMX"));
					if (temp!=-1)
						pStartData->m_aDatabases[0]->SetArchiveNamePtr(alloccopy(lpCmdLine+idx,temp));
					else
						pStartData->m_aDatabases[0]->SetArchiveNamePtr(alloccopy(lpCmdLine+idx));
				}
				else if (CDatabase::FindByFile(pStartData->m_aDatabases,lpCmdLine+idx,temp)==NULL)
				{
					CDatabase* pDatabase=CDatabase::FromFile(lpCmdLine+idx,temp);
					if (pDatabase!=NULL)
					{
						pStartData->m_aDatabases.Add(pDatabase);
						pStartData->m_nStartup|=CStartData::startupDatabasesOverridden;
					}
				}
				if (temp<0)
					return TRUE;
				idx+=temp;
			}
			else
			{
				idx++;
				temp=(int)FirstCharIndex(lpCmdLine+idx,L'\"');
				if (pStartData->m_aDatabases.GetSize()==1 && wcscmp(pStartData->m_aDatabases[0]->GetName(),L"DEFAULTX")==0)
				{
					pStartData->m_aDatabases[0]->SetNamePtr(alloccopy(L"PARAMX"));
					if (temp!=-1)
						pStartData->m_aDatabases[0]->SetArchiveNamePtr(alloccopy(lpCmdLine+idx,temp));
					else
						pStartData->m_aDatabases[0]->SetArchiveNamePtr(alloccopy(lpCmdLine+idx));
				}
				else if (CDatabase::FindByFile(pStartData->m_aDatabases,lpCmdLine+idx,temp)==NULL)
				{
					CDatabase* pDatabase=CDatabase::FromFile(lpCmdLine+idx,temp);
					if (pDatabase!=NULL)
					{
						pStartData->m_aDatabases.Add(pDatabase);
						pStartData->m_nStartup|=CStartData::startupDatabasesOverridden;
					}
				}
				if (temp<0)
					return TRUE;
				idx+=temp+1;
			}
			break;				
		case L'r': // start locating when Locate32 is opened
			idx++;
			pStartData->m_nStatus|=CStartData::statusRunAtStartUp;
			break;
		case L'i':
		case L'I':
			idx++;
			pStartData->m_nStartup|=CStartData::startupNewInstance;
			break;
		case L'n': // 'no'
			idx++;
			switch(lpCmdLine[idx])
			{
			case L't':
				pStartData->m_nStatus|=CStartData::statusNoExtension;
				break;	
			case L'f':
				ChangeAndAlloc(pStartData->m_pStartString,L"");
				break;	
			}
			idx++;
			break;
		case L'A': // Activate control
			idx++;
			if (lpCmdLine[idx]==L':')
				idx++;
			switch(lpCmdLine[idx])
			{
			case L'n':
			case L'N':
				pStartData->m_nActivateControl=CStartData::Named;
				break;	
			case L't':
			case L'T':
				pStartData->m_nActivateControl=CStartData::Type;
				break;	
			case L'l':
			case L'L':
				pStartData->m_nActivateControl=CStartData::LookIn;
				break;	
			case L'r':
			case L'R':
				pStartData->m_nActivateControl=CStartData::Results;
				break;	
			}
			idx++;
			break;
		case L'V': // Set view mode
			idx++;
			if (lpCmdLine[idx]==L':')
				idx++;
			switch(lpCmdLine[idx])
			{
			case L's':
			case L'S':
				pStartData->m_nListMode=CLocateDlg::ltSmallIcons;
				break;	
			case L'm':
			case L'M':
				pStartData->m_nListMode=CLocateDlg::ltMediumIcons;
				break;
			case L'l':
			case L'L':
				pStartData->m_nListMode=CLocateDlg::ltLargeIcons;
				break;	
			case L'x':
			case L'X':
				pStartData->m_nListMode=CLocateDlg::ltExtraLargeIcons;
				break;	
			case L'i':
			case L'I':
				pStartData->m_nListMode=CLocateDlg::ltList;
				break;	
			case L'd':
			case L'D':
				pStartData->m_nListMode=CLocateDlg::ltDetails;
				break;	
			}
			idx++;
			break;
		case L'a': // Activate instance
			idx++;
			if (lpCmdLine[idx]==L':')
				idx++;
			if (lpCmdLine[idx]==L' ')
				pStartData->m_nActivateInstance=-1;
			else if (lpCmdLine[idx]==L'\0')
			{
				pStartData->m_nActivateInstance=-1;
				return TRUE;
			}
			else
			{
				pStartData->m_nActivateInstance=_wtoi(lpCmdLine+idx);
				while(lpCmdLine[idx]!=' ' && lpCmdLine[idx]!='\0') idx++;
				if (lpCmdLine[idx]==L'\0')
					return TRUE;
			}
			break;		
		case L'S': // start locate32 to background, (adds icon to tastbar)
			pStartData->m_nStartup|=CStartData::startupLeaveBackground|CStartData::startupDoNotOpenDialog;
			idx++;
			break;
		case L's': // leave locate32 background when dialog is closed
			pStartData->m_nStartup|=CStartData::startupLeaveBackground;
			idx++;
			break;
		case L'u': // start update process at start
			idx++;
			pStartData->m_nStartup|=CStartData::startupUpdate;
			break;
		case L'U': // start update process and exit
			idx++;
			pStartData->m_nStartup|=CStartData::startupUpdate|CStartData::startupDoNotOpenDialog;
			break;
		case L'R':
			switch(lpCmdLine[++idx])
			{
			case L'h':
			case L'H':
				pStartData->m_nPriority=CStartData::priorityHigh;
				break;	
			case L'a':
			case L'A':
			case L'+':
				pStartData->m_nPriority=CStartData::priorityAbove;
				break;
			case L'n':
			case L'N':
			case L'0':
				pStartData->m_nPriority=CStartData::priorityNormal;
				break;
			case L'b':
			case L'B':
			case L'-':
				pStartData->m_nPriority=CStartData::priorityBelow;
				break;
			case L'i':
			case L'I':
				pStartData->m_nPriority=CStartData::priorityIdle;
				break;
			case L'r':
			case L'R':
				pStartData->m_nPriority=CStartData::priorityRealTime;
				break;
			}
			idx++;
			break;
		case L'L':
			idx++;
			if (lpCmdLine[idx]==L'1')
			{
				if (pStartData->m_aDatabases.GetSize()==0)
				{
					pStartData->m_aDatabases.Add(CDatabase::FromDefaults(TRUE));
					pStartData->m_aDatabases[0]->SetNamePtr(alloccopy(L"DEFAULTX"));
					pStartData->m_aDatabases[0]->SetThreadId(0);
					pStartData->m_nStartup|=CStartData::startupDatabasesOverridden;
					pStartData->m_aDatabases.GetLast()->AddLocalRoots();
				}
				else if (wcsncmp(pStartData->m_aDatabases.GetLast()->GetName(),L"PARAMX",6)==0 || 
					wcsncmp(pStartData->m_aDatabases.GetLast()->GetName(),L"DEFAULTX",8)==0)
					pStartData->m_aDatabases.GetLast()->AddLocalRoots();
				
				while (lpCmdLine[idx]!=L' ') idx++;
			}
			else 
			{
				while (lpCmdLine[idx]==L' ') idx++;
				
				CStringW Directory;
				if (lpCmdLine[idx]!=L'\"')
				{
					Directory.Copy(lpCmdLine+idx,FirstCharIndex(lpCmdLine+idx,L' '));
					idx+=(int)Directory.GetLength();
				}
				else
				{
					idx++;
					int nIndex=(int)FirstCharIndex(lpCmdLine+idx,L'\"');
					if (nIndex==-1)
						return TRUE;
					Directory.Copy(lpCmdLine+idx,nIndex);
					idx+=nIndex+1;
				}
				while (Directory.LastChar()==L'\\')
					Directory.DelLastChar();
					
				if (Directory.GetLength()>1)
				{
					LPCWSTR pDir=NULL;
					if (Directory[1]==L':' && Directory.GetLength()==2)
						pDir=alloccopy(Directory);
					else if (FileSystem::IsDirectory(Directory))
						pDir=alloccopy(Directory);

					if (pDir!=NULL)
					{
						if (pStartData->m_aDatabases.GetSize()==0)
						{
							pStartData->m_aDatabases.Add(CDatabase::FromDefaults(TRUE));
							pStartData->m_aDatabases[0]->SetNamePtr(alloccopy(L"DEFAULTX"));
							pStartData->m_aDatabases[0]->SetThreadId(0);
							pStartData->m_nStartup|=CStartData::startupDatabasesOverridden;
							pStartData->m_aDatabases.GetLast()->AddRoot(pDir);
						}
						else if (wcsncmp(pStartData->m_aDatabases.GetLast()->GetName(),L"PARAMX",6)==0 || 
							wcsncmp(pStartData->m_aDatabases.GetLast()->GetName(),L"DEFAULTX",8)==0)
							pStartData->m_aDatabases.GetLast()->AddRoot(pDir);
						else
							delete[] pDir;
					}
				}
			}
			break;
		case L'l':
			switch(lpCmdLine[++idx])
			{
			case L'P':
				idx++;
				if (lpCmdLine[idx]==L':')
					idx++;
				
				while (lpCmdLine[idx]==L' ') idx++;
				if (lpCmdLine[idx]!=L'\"')
				{
					temp=(int)FirstCharIndex(lpCmdLine+idx,L' ');
					ChangeAndAlloc(pStartData->m_pLoadPreset,lpCmdLine+idx,temp);
					if (temp<0)
						return TRUE;
					idx+=temp;
				}
				else
				{
					idx++;
					temp=(int)FirstCharIndex(lpCmdLine+idx,L'\"');
					ChangeAndAlloc(pStartData->m_pLoadPreset,lpCmdLine+idx,temp);
					if (temp<0)
						return TRUE;
					idx+=temp+1;
				}
				break;				
			case L'n': // set number of maximum found files
				{
					idx++;
					if (lpCmdLine[idx]==L':')
						idx++;
					while (lpCmdLine[idx]==L' ') idx++;
					temp=(int)FirstCharIndex(lpCmdLine+idx,L' ');
					CStringW str(lpCmdLine+idx,temp);
					
					int val=_wtoi(str);
					if (val!=0)
						pStartData->m_dwMaxFoundFiles=val;
					else if (str.Compare(L"0")==0)
						pStartData->m_dwMaxFoundFiles=0;

					if (temp<0)
						return TRUE;
					idx+=temp+1;
						break;
				}
			case L'f': // Set check field to 'File Names Only'
				idx++;
				pStartData->m_nStatus|=CStartData::statusFindFileNames;
				if (lpCmdLine[idx]==L'd')
				{
					pStartData->m_nStatus|=CStartData::statusFindFolderNames;
					idx++;
				}
				break;
			case L'd': // Set check field to 'Folder Names Only'
				idx++;
				pStartData->m_nStatus|=CStartData::statusFindFolderNames;
				if (lpCmdLine[idx]==L'f')
				{
					pStartData->m_nStatus|=CStartData::statusFindFileNames;
					idx++;
				}
				break;
			case L'c': // put 'text' to 'file containing text' field
				idx++;
				if (lpCmdLine[idx]==L'n' && lpCmdLine[idx+1]==L'm')
				{
					idx+=2;
					pStartData->m_nStatus|=CStartData::statusFindIsNotMatchCase;
					break;
				}
				
				if (lpCmdLine[idx]==L':')
					idx++;
				
				while (lpCmdLine[idx]==L' ') idx++;
				if (lpCmdLine[idx]!=L'\"')
				{
					temp=(int)FirstCharIndex(lpCmdLine+idx,L' ');
					ChangeAndAlloc(pStartData->m_pFindText,lpCmdLine+idx,temp);
					if (temp<0)
						return TRUE;
					idx+=temp;
				}
				else
				{
					idx++;
					temp=(int)FirstCharIndex(lpCmdLine+idx,L'\"');
					ChangeAndAlloc(pStartData->m_pFindText,lpCmdLine+idx,temp);
					if (temp<0)
						return TRUE;
					idx+=temp+1;
				}
				break;
			case L'w': // check 'Match whole name only' field
				idx++;
				if (lpCmdLine[idx]==L'n')
				{
					idx++;
					pStartData->m_nStatus|=CStartData::statusNoMatchWholeName;
				}
				else
					pStartData->m_nStatus|=CStartData::statusMatchWholeName;
				break;
			case L'r': // check 'Replace asterisks' field
				idx++;
				if (lpCmdLine[idx]==L'n')
				{
					idx++;
					pStartData->m_nStatus|=CStartData::statusNoReplaceSpacesWithAsterisks;
				}
				else
					pStartData->m_nStatus|=CStartData::statusReplaceSpacesWithAsterisks;
				break;
			case L'W': // check 'Use whole path' field
				idx++;
				if (lpCmdLine[idx]==L'n')
				{
					idx++;
					pStartData->m_nStatus|=CStartData::statusNoUseWholePath;
				}
				else
					pStartData->m_nStatus|=CStartData::statusUseWholePath;
				break;
			case L'm': // set minumum file size
				{
					idx++;
					if (lpCmdLine[idx]==':')
						idx++;
					while (lpCmdLine[idx]==' ') idx++;
					temp=(int)FirstCharIndex(lpCmdLine+idx,L' ');
					CStringW str(lpCmdLine+idx,temp);
					while ((str.LastChar()<L'0' || str.LastChar()>L'9') && !str.IsEmpty())
					{
						pStartData->m_cMinSizeType=W2Ac(str.LastChar());
						str.DelLastChar();
					}
					
					int val=_wtoi(str);
					if (val!=0)
						pStartData->m_dwMinFileSize=val;
					else if (str.Compare(L"0")==0)
						pStartData->m_dwMinFileSize=0;


					if (temp<0)
						return TRUE;
					idx+=temp+1;
					break;
				}
			case L'M': // set maximum file size
				{
					idx++;
					if (lpCmdLine[idx]==L':')
						idx++;
					while (lpCmdLine[idx]==L' ') idx++;
					temp=(int)FirstCharIndex(lpCmdLine+idx,L' ');
					CStringW str(lpCmdLine+idx,temp);
					while ((str.LastChar()<L'0' || str.LastChar()>L'9') && !str.IsEmpty())
					{
						pStartData->m_cMaxSizeType=W2Ac(str.LastChar());
						str.DelLastChar();
					}
					
					int val=_wtoi(str);
					if (val!=0)
						pStartData->m_dwMaxFileSize=val;
					else if (str.Compare(L"0")==0)
						pStartData->m_dwMaxFileSize=0;

					if (temp<0)
						return TRUE;
					idx+=temp+1;
					break;
				}
			case L'D': // dates
				{
					idx++;
					while (lpCmdLine[idx]==L' ')
						idx++;
                    int nLength=(int)FirstCharIndex(lpCmdLine+idx,L' ');
					if (nLength<0)
					{
						nLength=(int)wcslen(lpCmdLine+idx);
						if (nLength<7)
                            return TRUE;
					}

					if (nLength<2)
					{
						idx+=nLength;
						break;
					}

					BOOL bMaxDate=IsCharUpper(lpCmdLine[idx]);
					if (bMaxDate) // max date
						pStartData->m_cMaxDateType=W2Ac(lpCmdLine[idx++]);
					else
						pStartData->m_cMinDateType=W2Ac(lpCmdLine[idx++]);
					nLength--;


					DWORD dwDate;
					if (nLength<6)
					{
						dwDate=_wtoi(lpCmdLine+idx);
						dwDate|=1<<31;
					}
					else
					{
						WCHAR szBuf[]=L"XX";
						szBuf[0]=lpCmdLine[idx];
						szBuf[1]=lpCmdLine[idx+1];
						WORD bYear=_wtoi(szBuf);
						if (bYear<60)
							bYear+=2000;
						else
							bYear+=1900;
						szBuf[0]=lpCmdLine[idx+2];
						szBuf[1]=lpCmdLine[idx+3];
						BYTE bMonth=_wtoi(szBuf);
						if (bMonth<1 || bMonth>12)
							bMonth=1;
						szBuf[0]=lpCmdLine[idx+4];
						szBuf[1]=lpCmdLine[idx+5];
						BYTE bDay=_wtoi(szBuf);
						if (bDay<1 || bDay>CTime::GetDaysInMonth(bMonth,bYear))
							bDay=1;			

						dwDate=(bYear<<16)|(bMonth<<8)|(bDay);
					}
					idx+=nLength;
					
					if (bMaxDate)
						pStartData->m_dwMaxDate=dwDate;
					else
						pStartData->m_dwMinDate=dwDate;

					break;
				}
			case L's':
			case L'S': // Sort items
				idx++;
				switch (lpCmdLine[idx])
				{
				case L'n':
				case L'N':
					pStartData->m_nSorting=Name;
					break;
				case L'f':
				case L'F':
					pStartData->m_nSorting=InFolder;
					break;
				case L's':
				case L'S':
					pStartData->m_nSorting=FileSize;
					break;
				case L't':
				case L'T':
					pStartData->m_nSorting=FileType;
					break;
				case L'd':
				case L'D':
					pStartData->m_nSorting=DateModified;
					break;
				}
				idx++;
				if (lpCmdLine[idx]==L'd' || lpCmdLine[idx]==L'D')
					pStartData->m_nSorting|=128;
				idx++;
				break;
			}
			break;
		case 'W':
			{
				BOOL bAlsoRun=FALSE;

				// Search protocol query
				idx++;
				if (lpCmdLine[idx]=='R')
				{
					pStartData->m_nStatus|=CStartData::statusRunAtStartUp;
					idx++;
				}
				else
				{
					bAlsoRun=TRUE,
					idx++;
				}
				if (lpCmdLine[idx]==':') idx++;
				
				// Check "search:" part
				if (_wcsnicmp(lpCmdLine+idx,L"search:",7)!=0)
					break;
				idx+=7;

				// 
				for (;;)
				{
					if (_wcsnicmp(lpCmdLine+idx,L"query=",6)==0)
					{
						idx+=6;
						idx+=ParseSearchProtocolString(&pStartData->m_pStartString,lpCmdLine+idx);
						if (bAlsoRun && pStartData->m_pStartString!=NULL)
							pStartData->m_nStatus|=CStartData::statusRunAtStartUp;
			
					}
					else if (_wcsnicmp(lpCmdLine+idx,L"crumb=",6)==0)
					{
						idx+=6;
						if (_wcsnicmp(lpCmdLine+idx,L"location:",9)==0)
						{
							idx+=9;
							idx+=ParseSearchProtocolString(&pStartData->m_pStartPath,lpCmdLine+idx);
						}
						else
						{
							// Ignore this parameter
							idx+=ParseSearchProtocolString(NULL,lpCmdLine+idx);
						}
					}
					else
					{
						// Ignore this parameter
						idx+=ParseSearchProtocolString(NULL,lpCmdLine+idx);
					}


					if (lpCmdLine[idx]!='&')
						break;

					idx++;
				}
				break;
			}
		default:
			idx++;
			break;
		}
		return ParseParameters(lpCmdLine+idx,pStartData);
	
	}
	if (lpCmdLine[idx]!=L'\0')
		ChangeAndAlloc(pStartData->m_pStartString,lpCmdLine+idx);
	return TRUE;
}

void CLocateApp::ChangeAndAlloc(LPWSTR& pVar,LPCWSTR szText)
{
	DWORD dwLength=istrlenw(szText);
	if (pVar!=NULL)
		delete[] pVar;
	pVar=new WCHAR [dwLength+1];
	MemCopyW(pVar,szText,dwLength+1);
}

void CLocateApp::ChangeAndAlloc(LPWSTR& pVar,LPCWSTR szText,DWORD dwLength)
{
	if (dwLength==DWORD(-1))
		dwLength=(DWORD)istrlenw(szText);
	
	if (pVar!=NULL)
		delete[] pVar;
	pVar=new WCHAR [dwLength+1];
	MemCopyW(pVar,szText,dwLength);
	pVar[dwLength]='\0';
}

int CLocateApp::ParseSearchProtocolString(LPWSTR* ppVar,LPCWSTR szText)
{
	// Parse text
	CStringW pNewText;
	LPCWSTR pPtr=szText;
	for (;*pPtr!=L' ' && *pPtr!=L'&' && *pPtr!=L'\0';)
	{
		if (*pPtr==L'%')
		{
			pPtr++;

			WCHAR szTemp[3],*temp;
			*((DWORD*)szTemp)=*((DWORD*)pPtr);
			szTemp[2]='\0';

			pNewText << (char)wcstoul(szTemp,&temp,16);
			pPtr+=int(temp-szTemp);
		}
		else
			pNewText << *(pPtr++);
	}
	
	if (ppVar!=NULL)
	{
		if (*ppVar!=NULL)
			delete[] *ppVar;

		if (!pNewText.IsEmpty())
		{
			pNewText.FreeExtra();
			*ppVar=pNewText.GiveBuffer();
		}
		else
			*ppVar=NULL;
	}

	return int(pPtr-szText);
}


BOOL CLocateApp::ParseSettingsBranch(LPCWSTR lpCmdLine,LPWSTR& rpSettingBranch)
{
	for (int idx=0;lpCmdLine[idx]!='\0';idx++)
	{
		if ((lpCmdLine[idx]==L'/' || lpCmdLine[idx]==L'-') &&
			lpCmdLine[idx+1]==L'X')
		{
			idx+=2;
			if (lpCmdLine[idx]==L':')
				idx++;
			while(lpCmdLine[idx]==L' ') idx++;
			
			int temp=(int)FirstCharIndex(lpCmdLine+idx,L' ');
			ChangeAndAlloc(rpSettingBranch,lpCmdLine+idx,temp);

			if (temp<0)
				return TRUE;
			idx+=temp;
		}
	}
	
	return TRUE;
}

BYTE CLocateApp::CheckDatabases()
{
	// First, check that there is database 
	if (m_aDatabases.GetSize()==0)
		CDatabase::LoadFromRegistry(HKCU,CLocateApp::GetRegKey("Databases"),m_aDatabases);

	// If there is still no any available database, try to load old style db
	if (m_aDatabases.GetSize()==0)
	{
		CDatabase* pDatabase=CDatabase::FromOldStyleDatabase(HKCU,"Software\\Update\\Database");
		if (pDatabase==NULL)
		{
			pDatabase=CDatabase::FromDefaults(TRUE); // Nothing else can be done?
		}
		else
		{
			if (CDatabase::SaveToRegistry(HKCU,CLocateApp::GetRegKey("Databases"),&pDatabase,1))
				CRegKey::DeleteKey(HKCU,"Software\\Update\\Database");
		}
		m_aDatabases.Add(pDatabase);
	}

	m_pLastDatabase=m_aDatabases[0];

	CDatabase::CheckValidNames(m_aDatabases);
	CDatabase::CheckDoubleNames(m_aDatabases);
	CDatabase::CheckIDs(m_aDatabases);
	return TRUE;
}

BYTE CLocateApp::SetDeleteAndDefaultImage()
{
	WCHAR szDirectory[MAX_PATH];
	WCHAR szTempFileName[MAX_PATH];
	SHFILEINFOW fi;
			
	
	if (!FileSystem::GetTempPath(MAX_PATH,szDirectory))
		return FALSE;


	if (FileSystem::GetTempFileName(szDirectory,L"locfil",0,szTempFileName))
	{
		FileSystem::Remove(szTempFileName);

		wcscat_s(szTempFileName,MAX_PATH,L".ico");

		CFile File(TRUE);
		HGLOBAL hGlobal=NULL;
		BYTE* pData=NULL;

		try 
		{
			File.OpenWrite(szTempFileName);
			HRSRC hRsrc=FindResource(GetCommonResourceHandle(),MAKEINTRESOURCE(IDI_FILE),"DATA");

			if (hRsrc==NULL)
				throw CException(CException::cannotReadResource);

			DWORD nSize=SizeofResource(GetCommonResourceHandle(),hRsrc);
			hGlobal=LoadResource(GetCommonResourceHandle(),hRsrc);
			if (hGlobal==NULL)
				throw CException(CException::cannotReadResource);

			pData=(BYTE*)LockResource(hGlobal);

			File.Write(pData,nSize);

			File.Close();


			if (ShellFunctions::GetFileInfo(szTempFileName,0,&fi,SHGFI_SYSICONINDEX))
				m_nDefImage=fi.iIcon;
		}
		catch (...)
		{
		}

		if (hGlobal!=NULL)
		{
			if (pData!=NULL)
				UnlockResource(hGlobal);
			FreeResource(hGlobal);
		}

		FileSystem::Remove(szTempFileName);
	}

	if (FileSystem::GetTempFileName(szDirectory,L"locdel",0,szTempFileName))
	{
		FileSystem::Remove(szTempFileName);

		wcscat_s(szTempFileName,MAX_PATH,L".ico");

		CFile File(TRUE);
		HGLOBAL hGlobal=NULL;
		BYTE* pData=NULL;

		try 
		{
			File.OpenWrite(szTempFileName);
			HRSRC hRsrc=FindResource(GetCommonResourceHandle(),MAKEINTRESOURCE(IDI_DELETEDFILE),"DATA");

			if (hRsrc==NULL)
				throw CException(CException::cannotReadResource);

			DWORD nSize=SizeofResource(GetCommonResourceHandle(),hRsrc);
			hGlobal=LoadResource(GetCommonResourceHandle(),hRsrc);
			if (hGlobal==NULL)
				throw CException(CException::cannotReadResource);

			pData=(BYTE*)LockResource(hGlobal);

			File.Write(pData,nSize);

			File.Close();


			if (ShellFunctions::GetFileInfo(szTempFileName,0,&fi,SHGFI_SYSICONINDEX))
				m_nDelImage=fi.iIcon;
		}
		catch (...)
		{
		}

		if (hGlobal!=NULL)
		{
			if (pData!=NULL)
				UnlockResource(hGlobal);
			FreeResource(hGlobal);
		}

		FileSystem::Remove(szTempFileName);
	}

	
	if (FileSystem::GetSystemDirectory(szDirectory,_MAX_PATH)>0)
	{
		fi.iIcon=1;
		ShellFunctions::GetFileInfo(szDirectory,0,&fi,/*SHGFI_ICON|SHGFI_SMALLICON|*/SHGFI_SYSICONINDEX);
		m_nDirImage=fi.iIcon;

		WCHAR szDrives[100];
		if (FileSystem::GetLogicalDriveStrings(100,szDrives)>0)
		{
			LPCWSTR pPtr=szDrives;
			while (*pPtr!='\0')
			{
				if (FileSystem::GetDriveType(pPtr)==DRIVE_FIXED &&
					(*pPtr!=szDirectory[0] || szDirectory[1]!=':'))
					break;

				pPtr+=istrlenw(pPtr)+1;
			}

			ShellFunctions::GetFileInfo(*pPtr!='\0'?pPtr:szDrives,0,&fi,SHGFI_SYSICONINDEX);
			m_nDriveImage=fi.iIcon;
		}
		
	}
	
	
	//DebugMessage("SetDeleteAndDefaultImage END");
	return TRUE;
}


BOOL CLocateApp::ActivateOtherInstances(LPCWSTR pCmdLine)
{
	//HWND hWnd=FindWindow("LOCATEAPPST","Locate ST");
	//if (hWnd!=NULL)

	if (m_nInstance!=0 || m_pStartData->m_nActivateInstance!=0)
	{
		ATOM aCommandLine;
		if (IsUnicodeSystem())
			aCommandLine=GlobalAddAtomW(pCmdLine);
		else
			aCommandLine=GlobalAddAtom(W2A(pCmdLine));

		if (m_pStartData->m_nActivateInstance>0)
		{
			::SendNotifyMessage(HWND_BROADCAST,m_nLocateAppMessage,
				MAKEWPARAM(LOCATEMSG_ACTIVATEINSTANCE,m_pStartData->m_nActivateInstance-1),
				(LPARAM)aCommandLine);
		}
		else
			::SendNotifyMessage(HWND_BROADCAST,m_nLocateAppMessage,LOCATEMSG_ACTIVATEINSTANCE,(LPARAM)aCommandLine);

		if (aCommandLine!=NULL)
			DeleteAtom(aCommandLine);
		return TRUE;
	}	
	return FALSE;
}


	
LPWSTR CLocateApp::FormatDateAndTimeString(WORD wDate,WORD wTime)
{
	WCHAR szDate[201],szTime[201];
	DWORD dwDateLen=0,dwTimeLen=0;

	// First, format date to szDate
	
	// If wDate is -2, file date older than 1.1.1980
	if (wDate==WORD(-2))
		return allocstringW(IDS_OLDFILE);

	// If wDate is 0 or -1, skip date
	if (wDate!=WORD(-1) && wDate!=0)
	{
		// No formatting string, using system default (GetDateFormat)
			
		SYSTEMTIME st;
		st.wDay=DOSDATETODAY(wDate);
		st.wMonth=DOSDATETOMONTH(wDate);
		st.wYear=DOSDATETOYEAR(wDate);
		st.wHour=1;
		st.wMinute=1;
		st.wSecond=1;

		if (IsUnicodeSystem())
		{
			dwDateLen=GetDateFormatW(LOCALE_USER_DEFAULT,m_strDateFormat.IsEmpty()?DATE_SHORTDATE:0,&st,
				m_strDateFormat.IsEmpty()?NULL:LPCWSTR(m_strDateFormat),szDate,200);
			if (dwDateLen>0)
				dwDateLen--; 

		}
		else
		{
			char szDateA[201];
			dwDateLen=GetDateFormatA(LOCALE_USER_DEFAULT,m_strDateFormat.IsEmpty()?DATE_SHORTDATE:0,&st,
				m_strDateFormat.IsEmpty()?NULL:(LPCSTR)W2A(m_strDateFormat),szDateA,200);
			if (dwDateLen>0) 
			{
				MemCopyAtoW(szDate,201,szDateA,dwDateLen);
				dwDateLen--;
			}
		}

		// If GetDateFormat fails and m_strDateFormat is not empty,
		// parse file date by own

		if (dwDateLen==0 && !m_strDateFormat.IsEmpty())
		{
			for (int i=0;i<m_strDateFormat.GetLength();i++)
			{
				if (dwDateLen>196)
					break;

				switch (m_strDateFormat[i])
				{
				case L'd':
					if (m_strDateFormat[i+1]=='d') // "dd" , "ddd" and "dddd" will not be handled
					{
						szDate[dwDateLen++]=DOSDATETODAY(wDate)/10+L'0';
						szDate[dwDateLen++]=DOSDATETODAY(wDate)%10+L'0';
						i++;
					}
					else // "d"
					{
						if (DOSDATETODAY(wDate)>9)
						{
							szDate[dwDateLen++]=DOSDATETODAY(wDate)/10+L'0';
							szDate[dwDateLen++]=DOSDATETODAY(wDate)%10+L'0';
						}
						else
							szDate[dwDateLen++]=DOSDATETODAY(wDate)+L'0';
					}
					break;
				case L'M':
					if (m_strDateFormat[i+1]==L'M')  // 'MMM' and 'MMMM' are not handled
					{
						szDate[dwDateLen++]=DOSDATETOMONTH(wDate)/10+L'0';
						szDate[dwDateLen++]=DOSDATETOMONTH(wDate)%10+L'0';
						i++;
					}
					else // "M"
					{
						if (DOSDATETOMONTH(wDate)>9)
						{
							szDate[dwDateLen++]=DOSDATETOMONTH(wDate)/10+L'0';
							szDate[dwDateLen++]=DOSDATETOMONTH(wDate)%10+L'0';
						}
						else
							szDate[dwDateLen++]=DOSDATETOMONTH(wDate)+L'0';
						
					}
					break;
				case L'y':
					if (m_strDateFormat[i+1]==L'y')
					{
						if (m_strDateFormat[i+2]==L'y') // "yyy" & "yyyy"
						{
							szDate[dwDateLen++]=DOSDATETOYEAR(wDate)/1000+L'0';
							szDate[dwDateLen++]=(DOSDATETOYEAR(wDate)/100)%10+L'0';
							szDate[dwDateLen++]=(DOSDATETOYEAR(wDate)/10)%10+L'0';
							szDate[dwDateLen++]=DOSDATETOYEAR(wDate)%10+L'0';
							if (m_strDateFormat[i+3]==L'y')
								i+=3;
							else
								i+=2;
						}
						else // "yy"
						{
							szDate[dwDateLen++]=(DOSDATETOYEAR(wDate)/10)%10+L'0';
							szDate[dwDateLen++]=DOSDATETOYEAR(wDate)%10+L'0';
							i++;
						}			
					}
					else // "y"
					{
						if (DOSDATETOYEAR(wDate)/1000>9)
						{
							szDate[dwDateLen++]=(DOSDATETOYEAR(wDate)/10)%10+L'0';
							szDate[dwDateLen++]=DOSDATETOYEAR(wDate)%10+L'0';
						}
						else
							szDate[dwDateLen++]=DOSDATETOYEAR(wDate)%10+L'0';
					}
					break;
				case L'\'':
					continue;
				default:
					szDate[dwDateLen++]=m_strDateFormat[i];
					break;
				}
			}
		}
	}
	
	// Format time to szTime
	// If wTime is -1 or both wTime and wDate are 0, skip time
	if (wTime!=WORD(-1) && !(wTime==0 && wDate==0))
	{
		// Using GetTimeFormat as default
		SYSTEMTIME st;
		st.wHour=DOSTIMETO24HOUR(wTime);
		st.wMinute=DOSTIMETOMINUTE(wTime);
		st.wSecond=DOSTIMETOSECOND(wTime);
		st.wMilliseconds=0;
		st.wDay=1;
		st.wMonth=1;
		st.wYear=2000;
		
		if (IsUnicodeSystem())
		{
			dwTimeLen=GetTimeFormatW(LOCALE_USER_DEFAULT,TIME_NOSECONDS,&st,
				m_strTimeFormat.IsEmpty()?NULL:LPCWSTR(m_strTimeFormat),szTime,200);
			if (dwTimeLen>0)
				dwTimeLen--;
		}
		else
		{
			char szTimeA[201];
			dwTimeLen=GetTimeFormatA(LOCALE_USER_DEFAULT,TIME_NOSECONDS,&st,
				m_strTimeFormat.IsEmpty()?NULL:(LPCSTR)W2A(m_strTimeFormat),szTimeA,200);
			if (dwTimeLen>0) 
			{
				MemCopyAtoW(szTime,201,szTimeA,dwTimeLen);
				dwTimeLen--;
			}
		}



		// If GetTimeFormat fails and m_strTimeFormat is not empty,
		// parse file date by own

		if (dwTimeLen==0 && !m_strTimeFormat.IsEmpty())
		{
			for (int i=0;i<m_strTimeFormat.GetLength();i++)
			{
				if (dwTimeLen>196)
					break;

				switch (m_strTimeFormat[i])
				{
				case L'h':
					if (m_strTimeFormat[i+1]==L'h')
					{
						szTime[dwTimeLen++]=DOSTIMETO12HOUR(wTime)/10+L'0';
						szTime[dwTimeLen++]=DOSTIMETO12HOUR(wTime)%10+L'0';
						i++;
					}
					else
					{
						if (DOSTIMETO12HOUR(wTime)>9)
						{
							szTime[dwTimeLen++]=DOSTIMETO12HOUR(wTime)/10+L'0';
							szTime[dwTimeLen++]=DOSTIMETO12HOUR(wTime)%10+L'0';
						}
						else
							szTime[dwTimeLen++]=DOSTIMETO12HOUR(wTime)%10+L'0';
					}
					break;
				case L'H':
					if (m_strTimeFormat[i+1]==L'H')
					{
						szTime[dwTimeLen++]=DOSTIMETO24HOUR(wTime)/10+L'0';
						szTime[dwTimeLen++]=DOSTIMETO24HOUR(wTime)%10+L'0';
						i++;
					}
					else
					{
						if (DOSTIMETO24HOUR(wTime)>9)
						{
							szTime[dwTimeLen++]=DOSTIMETO24HOUR(wTime)/10+L'0';
							szTime[dwTimeLen++]=DOSTIMETO24HOUR(wTime)%10+L'0';
						}
						else
							szTime[dwTimeLen++]=DOSTIMETO24HOUR(wTime)%10+L'0';
					}
					break;
				case L'm':
					if (m_strTimeFormat[i+1]==L'm')
					{
						szTime[dwTimeLen++]=DOSTIMETOMINUTE(wTime)/10+L'0';
						szTime[dwTimeLen++]=DOSTIMETOMINUTE(wTime)%10+L'0';
						i++;
					}
					else
					{
						if (DOSTIMETOMINUTE(wTime)>9)
						{
							szTime[dwTimeLen++]=DOSTIMETOMINUTE(wTime)/10+L'0';
							szTime[dwTimeLen++]=DOSTIMETOMINUTE(wTime)%10+L'0';
						}
						else
							szTime[dwTimeLen++]=DOSTIMETOMINUTE(wTime)%10+L'0';
					}
					break;
				case L's':
					if (m_strTimeFormat[i+1]==L's')
					{
						szTime[dwTimeLen++]=DOSTIMETOSECOND(wTime)/10+L'0';
						szTime[dwTimeLen++]=DOSTIMETOSECOND(wTime)%10+L'0';
						i++;
					}
					else
					{
						if (DOSTIMETOSECOND(wTime)>9)
						{
							szTime[dwTimeLen++]=DOSTIMETOSECOND(wTime)/10+L'0';
							szTime[dwTimeLen++]=DOSTIMETOSECOND(wTime)%10+L'0';
						}
						else
							szTime[dwTimeLen++]=DOSTIMETOSECOND(wTime)%10+L'0';
					}
					break;
				case 't':
					{
						WCHAR szAMPM[10];
						LoadString(DOSTIMETO24HOUR(wTime)>11?IDS_PM:IDS_AM,szAMPM,10);
						
						if (m_strTimeFormat[i+1]==L't')
						{
							for (WCHAR* ptr=szAMPM;*ptr!=L'\0';ptr++)
								szTime[dwTimeLen++]=*ptr;
							i++;
						}
						else
							szTime[dwTimeLen++]=szAMPM[0];
					}
					break;
				case L'\'':
					continue;
				default:
					szTime[dwTimeLen++]=m_strTimeFormat[i];
					break;
				}
			}
		}
		
	}

	if (dwTimeLen==0 && dwDateLen>0) 
		return alloccopy(szDate,dwDateLen);

	if (dwDateLen==0 && dwTimeLen>0)
		return alloccopy(szTime,dwTimeLen);
	
	// Combine date and time
	WCHAR* pRet=new WCHAR[dwDateLen+dwTimeLen+2];
	MemCopyW(pRet,szDate,dwDateLen);
	pRet[dwDateLen]=L' ';
	MemCopyW(pRet+dwDateLen+1,szTime,dwTimeLen);
	pRet[dwDateLen+dwTimeLen+1]='\0';
	return pRet;
}

LPWSTR CLocateApp::FormatFileSizeString(DWORD dwFileSizeLo,DWORD bFileSizeHi) const
{
	WCHAR* szRet=new WCHAR[40];
	WCHAR szUnit[10];
	BOOL bDigits=0;
		

	switch (m_nFileSizeFormat)
	{
	case fsfOverKBasKB:
		if (bFileSizeHi>0)
		{
			LoadString(IDS_KB,szUnit,10);
			
			_int64 uiFileSize=((_int64)bFileSizeHi)<<32|(_int64)dwFileSizeLo;
			_ui64tow_s(uiFileSize/1024,szRet,40,10);
		}
		else if (dwFileSizeLo<1024)
		{
			LoadString(IDS_BYTES,szUnit,10);
			
			_ultow_s(dwFileSizeLo,szRet,40,10);
		}
		else
		{
			LoadString(IDS_KB,szUnit,10);
			
			_ultow_s((dwFileSizeLo/1024)+(dwFileSizeLo%1024==0?0:1),szRet,40,10);
		}
		break;
	case fsfOverMBasMB:
		if (bFileSizeHi>0)
		{
			LoadString(IDS_MB,szUnit,10);
			
			_int64 uiFileSize=((_int64)bFileSizeHi)<<32|(_int64)dwFileSizeLo;
			_ui64tow_s(uiFileSize/(1024*1024),szRet,40,10);
		}
		else if (dwFileSizeLo<1024)
		{
			LoadString(IDS_BYTES,szUnit,10);
			
			_ultow_s(dwFileSizeLo,szRet,40,10);
		}
		else if (dwFileSizeLo<1024*1024)
		{
			LoadString(IDS_KB,szUnit,10);
			
			_ultow_s(dwFileSizeLo/1024,szRet,40,10);
		}
		else
		{
			LoadString(IDS_MB,szUnit,10);
			
			_ultow_s((dwFileSizeLo/(1024*1024))+(dwFileSizeLo%(1024*1024)==0?0:1),szRet,40,10);
		}
		break;
	case fsfBestUnit:
		if (bFileSizeHi>0)
		{
			DWORD num=DWORD(((((_int64)bFileSizeHi)<<32|(_int64)dwFileSizeLo))/(1024*1024));

			if (num>=10*1024)
				_ultow_s(num/1024,szRet,40,10);
			else
			{
				bDigits=1;
				StringCbPrintfW(szRet,40*sizeof(WCHAR),L"%1.1f",double(num)/1024);
			}

			LoadString(IDS_GB,szUnit,10);
		}
		else if (dwFileSizeLo>1024*1024*1024)
		{
			DWORD num=dwFileSizeLo/(1024*1024);

			if (num>=10*1024)
				_ultow_s(num/1024,szRet,40,10);
			else
			{
				bDigits=1;
				StringCbPrintfW(szRet,40*sizeof(WCHAR),L"%1.1f",double(num)/1024);
			}

			LoadString(IDS_GB,szUnit,10);
		}
		else if (dwFileSizeLo>1048576) // As MB
		{
			DWORD num=dwFileSizeLo/1024;
			
			if (num>=10*1024)
				_ultow_s(num/1024,szRet,40,10);
			else
			{
				bDigits=1;
				StringCbPrintfW(szRet,40*sizeof(WCHAR),L"%1.1f",double(num)/1024);
			}

			LoadString(IDS_MB,szUnit,10);
		}
		else if (dwFileSizeLo>1024) // As KB
		{
			if (dwFileSizeLo>=10*1024)
				_ultow_s(dwFileSizeLo/1024,szRet,40,10);
			else
			{
				bDigits=1;
				StringCbPrintfW(szRet,40*sizeof(WCHAR),L"%1.1f",double(dwFileSizeLo)/1024);
			}
			
			LoadString(IDS_KB,szUnit,10);
		}
		else // As B
		{
			_ultow_s(dwFileSizeLo,szRet,40,10);
		
			LoadString(IDS_BYTES,szUnit,10);
		}		
		break;
	case fsfBytes:
		LoadString(IDS_BYTES,szUnit,10);
	case fsfBytesNoUnit:
		if (bFileSizeHi>0)
		{
			_int64 uiFileSize=((_int64)bFileSizeHi)<<32|(_int64)dwFileSizeLo;
			_ui64tow_s(uiFileSize,szRet,40,10);
		}
		else
			_ultow_s(dwFileSizeLo,szRet,40,10);
		
		break;
	case fsfKiloBytes:
		LoadString(IDS_KB,szUnit,10);
	case fsfKiloBytesNoUnit:
		if (bFileSizeHi>0)
		{
			_int64 uiFileSize=((_int64)bFileSizeHi)<<32|(_int64)dwFileSizeLo;
			_ui64tow_s(uiFileSize/1024,szRet,40,10);
		}
		else if (dwFileSizeLo>10*1024)
			_ultow_s(dwFileSizeLo/1024,szRet,40,10);
		else if (dwFileSizeLo<1024)
		{
			bDigits=3;
			StringCbPrintfW(szRet,40*sizeof(WCHAR),L"%1.3f",((double)dwFileSizeLo)/1024);
		}
		else
		{
			bDigits=1;
			StringCbPrintfW(szRet,40*sizeof(WCHAR),L"%1.1f",((double)dwFileSizeLo)/1024);
		}
		break;
	case fsfMegaBytesMegaBytes:
		LoadString(IDS_MB,szUnit,10);
	case fsfMegaBytesMegaBytesNoUnit:
		if (bFileSizeHi>0)
		{
			_int64 uiFileSize=((_int64)bFileSizeHi)<<32|(_int64)dwFileSizeLo;
			_ui64tow_s(uiFileSize/1048576,szRet,40,10);
		}
		else if (dwFileSizeLo>10*1048576)
			_ultow_s(dwFileSizeLo/1048576,szRet,40,10);
		else if (dwFileSizeLo<1048576)
		{
			bDigits=3;
			StringCbPrintfW(szRet,40*sizeof(WCHAR),L"%1.3f",((double)dwFileSizeLo)/1048576);
		}
		else
		{
			bDigits=1;
			StringCbPrintfW(szRet,40*sizeof(WCHAR),L"%1.1f",((double)dwFileSizeLo)/1048576);
		}		
		break;
	}

	if (m_dwProgramFlags&pfFormatUseLocaleFormat)
	{
		if (GetLocateApp()->m_pLocaleNumberFormat==NULL)
			GetLocateApp()->m_pLocaleNumberFormat=new LocaleNumberFormat;

		union {
			NUMBERFMTW fmtw;
			NUMBERFMT fmt;
		};

		fmt.NumDigits=bDigits; 
		fmt.LeadingZero=GetLocateApp()->m_pLocaleNumberFormat->uLeadingZero;
		fmt.Grouping=GetLocateApp()->m_pLocaleNumberFormat->uGrouping; 
		fmt.NegativeOrder=1; 
			
		if (IsUnicodeSystem())
		{
			WCHAR* szFormattedStr=new WCHAR[60];
			
			fmtw.lpDecimalSep=GetLocateApp()->m_pLocaleNumberFormat->pDecimal; 
			fmtw.lpThousandSep=GetLocateApp()->m_pLocaleNumberFormat->pThousand; 
			
			if (GetNumberFormatW(LOCALE_USER_DEFAULT,0,szRet,&fmtw,szFormattedStr,60)>0)
			{
				delete[] szRet;
				szRet=szFormattedStr;
			}
			else
				delete[] szFormattedStr;
		}
		else
		{
			char szFormattedStr[50];
					
			fmt.lpDecimalSep=alloccopyWtoA(GetLocateApp()->m_pLocaleNumberFormat->pDecimal); 
			fmt.lpThousandSep=alloccopyWtoA(GetLocateApp()->m_pLocaleNumberFormat->pThousand); 
			
			if (GetNumberFormat(LOCALE_USER_DEFAULT,0,W2A(szRet),&fmt,szFormattedStr,50)>0)
				MultiByteToWideChar(CP_ACP,0,szFormattedStr,-1,szRet,40);

			delete[] fmt.lpDecimalSep;
			delete[] fmt.lpThousandSep;
		}
	}

	if (!(m_nFileSizeFormat==fsfBytesNoUnit || 
		m_nFileSizeFormat==fsfKiloBytesNoUnit|| 
		m_nFileSizeFormat==fsfMegaBytesMegaBytesNoUnit))
		StringCbCatW(szRet,40*sizeof(WCHAR),szUnit);

	return szRet;
}


BOOL CLocateApp::StopUpdating(BOOL bForce)
{
    if (!IsUpdating())
		return TRUE; // Already stopped

	// Return falue
	BOOL bRet=TRUE;

	// Stop every updater thread
	EnterCriticalSection(&m_cUpdatersPointersInUse);
	for (int i=0;m_ppUpdaters!=NULL && m_ppUpdaters[i]!=NULL;i++)
	{
		if (!IS_UPDATER_EXITED(m_ppUpdaters[i]))
		{
			// StopUpdating() may want to use m_pUpdaters points 
			// trough UpdateProc. UpdateProc dows not change order
			// or the size of m_ppUpdaters but may free it and set to NULL
			LeaveCriticalSection(&m_cUpdatersPointersInUse);

			if (!m_ppUpdaters[i]->StopUpdating(bForce))
				bRet=FALSE;
			
			EnterCriticalSection(&m_cUpdatersPointersInUse);
		}
	}
	LeaveCriticalSection(&m_cUpdatersPointersInUse);
	
	// Closes update status window and stops animation
	GetTrayIconWnd()->StopUpdateStatusNotification();


	/// Notify the dialog so that it can reset lookin combo etc
	CLocateDlg* pLocateDlg=GetLocateDlg();
	if (pLocateDlg!=NULL)
		pLocateDlg->SendMessage(WM_UPDATINGSTOPPED);
		

	// Set tray icon
	GetTrayIconWnd()->SetTrayIcon(DEFAPPICON,IDS_NOTIFYLOCATE);
	
	// I think pointers are removed elsewhere
	ASSERT(m_ppUpdaters==NULL);



	return bRet;
}

BOOL CLocateApp::IsWritingDatabases()
{
	if (!IsUpdating())
		return FALSE; // Not updating in progress

	// Checking whether some updater is writing a database
	BOOL bWriting=FALSE;
	EnterCriticalSection(&m_cUpdatersPointersInUse);
	for (int i=0;m_ppUpdaters!=NULL && m_ppUpdaters[i]!=NULL;i++)
	{
		if (!IS_UPDATER_EXITED(m_ppUpdaters[i]))
		{
			if (m_ppUpdaters[i]->GetStatus()==statusWritingDB)
			{
				LeaveCriticalSection(&m_cUpdatersPointersInUse);
				return TRUE;
			}
		}
	}
	LeaveCriticalSection(&m_cUpdatersPointersInUse);

	return FALSE;
}

void CLocateApp::SetDatabases(const CArray<CDatabase*>& rDatabases)
{
	m_aDatabases.RemoveAll();

	int i;
	for (i=0;i<rDatabases.GetSize();i++)
		m_aDatabases.Add(new CDatabase(*rDatabases[i]));

	if (i>0)
		m_pLastDatabase=m_aDatabases[0];
}

WORD CLocateApp::GetDatabasesNumberOfThreads() const
{
	WORD wHighestThread=0;
	for (int i=0;i<m_aDatabases.GetSize();i++)
	{
		if (wHighestThread<m_aDatabases[i]->GetThreadId())
			wHighestThread=m_aDatabases[i]->GetThreadId();
	}
	return wHighestThread+1;
}

void CLocateApp::ClearStartData()
{
	if (m_pStartData!=NULL)
	{
		delete ((CLocateApp*)GetApp())->m_pStartData;
		((CLocateApp*)GetApp())->m_pStartData=NULL;
	}
}



int CLocateApp::ErrorBox(int nError,UINT uType)
{
	ID2W Text(nError);
	ID2W Title(IDS_ERROR);

	CLocateDlg* pLocateDlg=GetLocateDlg();
	if (pLocateDlg!=NULL)
		return pLocateDlg->MessageBox(Text,Title,uType);
	
	extern CLocateApp theApp;;
	if (theApp.m_pMainWnd!=NULL)
	{
		theApp.m_pMainWnd->ForceForegroundAndFocus();
		theApp.m_pMainWnd->SetForegroundWindow();
	}
	
	return CWnd::MessageBox(NULL,Text,Title,uType|MB_TOPMOST);
}


int CLocateApp::ErrorBox(LPCWSTR szError,UINT uType)
{
	ID2W Title(IDS_ERROR);

	CLocateDlg* pLocateDlg=GetLocateDlg();
	if (pLocateDlg!=NULL)
		return pLocateDlg->MessageBox(szError,Title,uType);
	
	extern CLocateApp theApp;;
	if (theApp.m_pMainWnd!=NULL)
	{
		theApp.m_pMainWnd->ForceForegroundAndFocus();
		theApp.m_pMainWnd->SetForegroundWindow();	
	}
	
	return CWnd::MessageBox(NULL,szError,Title,uType|MB_TOPMOST);
}
	
BOOL CALLBACK CLocateApp::UpdateProc(DWORD_PTR dwParam,CallingReason crReason,UpdateError ueCode,CDatabaseUpdater* pUpdater)
{
	DbcDebugFormatMessage2("CLocateApp::UpdateProc BEGIN, reason=%d, code=%d",crReason,ueCode);
	
	switch (crReason)
	{
	case Initializing:
		{
			// Start animations
			if (GetTrayIconWnd()->GetHandle()!=NULL)
			{
				GetTrayIconWnd()->StartUpdateStatusNotification();
			
				CLocateDlg* pLocateDlg=GetLocateDlg();
				if (pLocateDlg!=NULL)
				{
					pLocateDlg->StartUpdateAnimation();
				
					//pLocateDlg->m_pStatusCtrl->SetText(szEmpty,0,0);
					pLocateDlg->SendMessage(WM_SETOPERATIONSTATUSBAR,0,(LPARAM)(LPCWSTR)ID2W(IDS_UPDATINGDATABASE));							
				}
			}

			return TRUE;
		}
	case RootChanged:
		{
			CLocateDlg* pLocateDlg=GetLocateDlg();
			if (pLocateDlg!=NULL)
			{
				CStringW str;

				str.FormatEx(IDS_UPDATINGDATABASE2,
					pUpdater->GetCurrentDatabaseName(),
					pUpdater->GetCurrentRoot()!=NULL?
					(LPCWSTR)pUpdater->GetCurrentRoot()->m_Path:
					(LPCWSTR)ID2W(IDS_UPDATINGWRITINGDATABASE));

				pLocateDlg->SendMessage(WM_SETOPERATIONSTATUSBAR,0,(LPARAM)(LPCWSTR)str);
			}
		
			
			if (dwParam!=NULL)
				((CTrayIconWnd*)dwParam)->SetTrayIcon(NULL,IDS_NOTIFYUPDATING);
			return TRUE;
		}
	case WritingDatabase:
		{
			CLocateDlg* pLocateDlg=GetLocateDlg();
			if (pLocateDlg!=NULL)
			{
				CStringW str;
				str.FormatEx(IDS_UPDATINGDATABASE2,
					pUpdater->GetCurrentDatabaseName(),
					(LPCWSTR)ID2W(IDS_UPDATINGWRITINGDATABASE));
				
				pLocateDlg->SendMessage(WM_SETOPERATIONSTATUSBAR,0,(LPARAM)(LPCWSTR)str);
			}
		
			
			if (dwParam!=NULL)
				((CTrayIconWnd*)dwParam)->SetTrayIcon(NULL,IDS_NOTIFYUPDATING);
			return TRUE;
		}
	case InitializeWriting:
		{
			CLocateDlg* pLocateDlg=GetLocateDlg();
			if (pLocateDlg!=NULL)
			{
				// Locating in progress, waiting
				
				// statusCustom1 is for delay writing
				pUpdater->SetStatus(statusCustom1);

				HANDLE hThread=pLocateDlg->GetLocaterThread(TRUE);
				if (hThread!=NULL)
				{
					CStringW str;
					str.FormatEx(IDS_UPDATINGDATABASE2,
						pUpdater->GetCurrentDatabaseName(),
						(LPCWSTR)ID2W(IDS_UPDATINGDELAYWRITING));

					pLocateDlg->SendMessage(WM_SETOPERATIONSTATUSBAR,0,(LPARAM)(LPCWSTR)str);

					WaitForSingleObject(hThread,INFINITE);

					CloseHandle(hThread);
				}
			}
			return TRUE;
		}
	case FinishedDatabase:
		{
			CLocateDlg* pLocateDlg=GetLocateDlg();
			if (pLocateDlg!=NULL)
			{
				if (ueCode==ueStopped)
				{
					CStringW str;
					str.FormatEx(IDS_UPDATINGDATABASE2,
						pUpdater->GetCurrentDatabaseName(),
						(LPCWSTR)ID2W(IDS_UPDATINGCANCELLED2));

					pLocateDlg->SendMessage(WM_SETOPERATIONSTATUSBAR,0,(LPARAM)(LPCWSTR)str);
					return FALSE;
				}
				else if (ueCode!=ueSuccess)
				{
					CStringW str;
					str.FormatEx(IDS_UPDATINGDATABASE2,
						pUpdater->GetCurrentDatabaseName(),
						(LPCWSTR)ID2W(IDS_UPDATINGFAILED));

					pLocateDlg->SendMessage(WM_SETOPERATIONSTATUSBAR,0,(LPARAM)(LPCWSTR)str);
					return FALSE;
				}
				CStringW str;
				str.FormatEx(IDS_UPDATINGDATABASE2,
					pUpdater->GetCurrentDatabaseName(),
					(LPCWSTR)ID2W(IDS_UPDATINGDONE));

				pLocateDlg->SendMessage(WM_SETOPERATIONSTATUSBAR,0,(LPARAM)(LPCWSTR)str);
			}

			return TRUE;
		}
	case ClassShouldDelete:
		{
			if (!GetLocateApp()->IsUpdating())
				return TRUE; // One thread mode

						
			DWORD dwRunning=0;
			
			EnterCriticalSection(&GetLocateApp()->m_cUpdatersPointersInUse);
						
			if (GetLocateApp()->m_ppUpdaters==NULL)
			{
				LeaveCriticalSection(&GetLocateApp()->m_cUpdatersPointersInUse);
				return FALSE;
			}

			
			for (int i=0;GetLocateApp()->m_ppUpdaters[i]!=NULL;i++)
			{
				if (GetLocateApp()->m_ppUpdaters[i]==pUpdater)
					GetLocateApp()->m_ppUpdaters[i]=UPDATER_EXITED(ueCode);
				else if (!IS_UPDATER_EXITED(GetLocateApp()->m_ppUpdaters[i]))
					dwRunning++;
			}
			delete pUpdater;
			
			if (dwRunning==0)
			{
				((CTrayIconWnd*)dwParam)->NotifyFinishingUpdating();
				
				// Freeing memory
				delete[] GetLocateApp()->m_ppUpdaters;
				GetLocateApp()->m_ppUpdaters=NULL;

				LeaveCriticalSection(&GetLocateApp()->m_cUpdatersPointersInUse);
	
				/*
				// Stop update animation if already not stopped
				if (ueCode!=ueStopped) 
				{
					CLocateDlg* pLocateDlg=GetLocateDlg();
					if (pLocateDlg!=NULL)
						pLocateDlg->PostMessage(WM_UPDATINGSTOPPED);
				}*/

				if (dwParam!=NULL && GetLocateApp()->m_nStartup&CStartData::startupExitAfterUpdating)
					((CTrayIconWnd*)dwParam)->PostMessage(WM_COMMAND,IDM_EXIT,NULL);
			}
			else 
			{
				// Updaters still running, updating shell notify icons
				LeaveCriticalSection(&GetLocateApp()->m_cUpdatersPointersInUse);
				
				if (dwParam!=NULL)
				{
					// dwParam can be NULL, if updating process is started via 
					// command line parameters such that dialog is not opened
				
					((CTrayIconWnd*)dwParam)->SetTrayIcon(NULL,IDS_NOTIFYUPDATING);
				}
			}
			return TRUE;
		}
	case ErrorOccured:
		if (dwParam!=NULL)
		{
			if (((CTrayIconWnd*)dwParam)->m_pUpdateStatusWnd!=NULL)
				((CTrayIconWnd*)dwParam)->m_pUpdateStatusWnd->FormatErrorForStatusTooltip(ueCode,pUpdater);
		}

		switch(ueCode)
		{
		case ueUnknown:
			if (CLocateApp::GetProgramFlags()&CLocateApp::pfShowCriticalErrors)
			{
				WCHAR* pError=FormatLastOsError();

				if (pError!=NULL)
				{
					CStringW str,state;
					if (pUpdater->GetCurrentRoot()==NULL)
						state.Format(IDS_ERRORUNKNOWNWRITEDB,pUpdater->GetCurrentDatabaseFile());
					else
						state.Format(IDS_ERRORUNKNOWNSCANROOT,pUpdater->GetCurrentRootPath());
					
					
					str.FormatEx(IDS_ERRORUNKNOWNOS,pError);
					while (str.LastChar()=='\n' || str.LastChar()=='\r')
						str.DelLastChar();
					str << state;
					
					ErrorBox(str);
					LocalFree(pError);

					
				}
				else
					ErrorBox(IDS_ERRORUNKNOWN);
				
			}
			return FALSE;
		case ueCreate:
		case ueOpen:
			if (CLocateApp::GetProgramFlags()&CLocateApp::pfShowCriticalErrors)
			{
				CStringW str;
				WCHAR* pError=CLocateApp::FormatLastOsError();
				if (pError!=NULL)
					str.FormatEx(IDS_ERRORCANNOTOPENDBFORWRITE,pUpdater->GetCurrentDatabaseFile(),pError);
				else
					str.Format(IDS_ERRORCANNOTOPENDB,pUpdater->GetCurrentDatabaseFile());
				ErrorBox(str);
			}
			return FALSE;
		case ueRead:
			if (CLocateApp::GetProgramFlags()&CLocateApp::pfShowCriticalErrors)
			{
				CStringW str;
				WCHAR* pError=CLocateApp::FormatLastOsError();
				if (pError!=NULL)
					str.FormatEx(IDS_ERRORCANNOTREADDBWITHOSERROR,pUpdater->GetCurrentDatabaseFile(),pError);
				else
					str.Format(IDS_ERRORCANNOTREADDB,pUpdater->GetCurrentDatabaseFile());
				ErrorBox(str);
			}
			return FALSE;
		case ueWrite:
			if (CLocateApp::GetProgramFlags()&CLocateApp::pfShowCriticalErrors)
			{
				CStringW str;
				WCHAR* pError=CLocateApp::FormatLastOsError();
				if (pError!=NULL)
					str.FormatEx(IDS_ERRORCANNOTWRITEDBWITHOSERROR,pUpdater->GetCurrentDatabaseFile(),pError);
				else
					str.Format(IDS_ERRORCANNOTWRITEDB,pUpdater->GetCurrentDatabaseFile());
				ErrorBox(str);
			}
			return FALSE;
		case ueAlloc:
			if (CLocateApp::GetProgramFlags()&CLocateApp::pfShowCriticalErrors)
				ErrorBox(IDS_ERRORCANNOTALLOCATE);
			return FALSE;
		case ueInvalidDatabase:
			if (CLocateApp::GetProgramFlags()&CLocateApp::pfShowCriticalErrors)
			{
				CStringW str;
				str.Format(IDS_ERRORINVALIDDB,W2A(pUpdater->GetCurrentDatabaseName()));
				ErrorBox(str);
				return FALSE;
			}
			return FALSE;
		case ueFolderUnavailable:
			if (CLocateApp::GetProgramFlags()&CLocateApp::pfShowNonCriticalErrors)
			{
				CStringW str;
				
				// Check if drive could be removed from database settings and, if possibl, ask that
				WORD wID=pUpdater->GetCurrentDatabaseID();
				if (wID!=0 && pUpdater->GetCurrentRootPath()!=NULL && 
					!(GetLocateApp()->GetStartupFlags()&CStartData::startupDatabasesOverridden))
				{
					CDatabase* pDatabase=GetLocateApp()->GetDatabaseNonConst(wID);
					if (pDatabase!=NULL)
					{
						CArray<LPWSTR> aRoots;
						pDatabase->GetRoots(aRoots);
						
						if (aRoots.GetSize()>1)
						{
							int nRoot;
							for (nRoot=0;nRoot<aRoots.GetSize();nRoot++)
							{
								if (_wcsicmp(aRoots[nRoot],pUpdater->GetCurrentRootPath())==0)
									break;
							}

							if (nRoot<aRoots.GetSize())
							{
								str.FormatEx(IDS_ERRORROOTNOTAVAILABLEASKREMOVE,
									pUpdater->GetCurrentRootPath()!=NULL?pUpdater->GetCurrentRootPath():ID2W(IDS_UNKNOWN),
									pUpdater->GetCurrentDatabaseName()!=NULL?pUpdater->GetCurrentDatabaseName():ID2W(IDS_UNKNOWN));
								switch(ErrorBox(str,MB_ICONERROR|MB_YESNOCANCEL|(pUpdater->IsStopIfUnuavailableSet()?MB_DEFBUTTON3:MB_DEFBUTTON2)))
								{
								case IDYES:
									aRoots.RemoveAt(nRoot);
									pDatabase->SetRoots(aRoots);

									// Save databases to registry
									CDatabase::SaveToRegistry(HKCU,CLocateApp::GetRegKey("Databases"),GetLocateApp()->GetDatabases());
									return 1;
								case IDNO:
									return 1;
								default:
									return -1;
								}
							}
						}
					}
				}

				str.FormatEx(IDS_ERRORROOTNOTAVAILABLE,
						pUpdater->GetCurrentRootPath()!=NULL?pUpdater->GetCurrentRootPath():L"",
						pUpdater->GetCurrentDatabaseName()!=NULL?pUpdater->GetCurrentDatabaseName():L"");
				return ErrorBox(str,MB_ICONERROR|MB_YESNO|(pUpdater->IsStopIfUnuavailableSet()?MB_DEFBUTTON2:MB_DEFBUTTON1))==IDYES?1:-1;
			}
			return FALSE;
		case ueCannotIncrement:
			if (CLocateApp::GetProgramFlags()&CLocateApp::pfShowNonCriticalErrors)
			{
				CStringW str;
				str.Format(IDS_ERRORCANNOTWRITEINCREMENTALLY,W2A(pUpdater->GetCurrentDatabaseName()));
				return ErrorBox(str,MB_ICONERROR|MB_YESNO)==IDYES;
			}
			return TRUE;
		case ueWrongCharset:
			if (CLocateApp::GetProgramFlags()&CLocateApp::pfShowNonCriticalErrors)
			{
				CStringW str;
				str.Format(IDS_ERRORDIFFERENTCHARSETINDB,W2A(pUpdater->GetCurrentDatabaseName()));
				return ErrorBox(str,MB_ICONERROR|MB_YESNO)==IDYES;
			}
			return TRUE;
		case ueCannotCreateThread:
			if (CLocateApp::GetProgramFlags()&CLocateApp::pfShowCriticalErrors)
				ErrorBox(IDS_ERRORCANNOTCREATETHREAD);
			return FALSE;
		}
		break;
	}
	
	DbcDebugMessage("CLocateApp::UpdateProc END");
	return TRUE;
}

BOOL CLocateApp::SetLanguageSpecifigHandles()
{
	CRegKey2 RegKey;
	CStringW LangFile;
	if (RegKey.OpenKey(HKCU,"",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
	{
		RegKey.QueryValue(L"Language",LangFile);
		RegKey.CloseKey();
	}
	if (LangFile.IsEmpty())
		LangFile=L"lan_en.dll";

	CStringW sExeName(GetApp()->GetExeNameW());
	CStringW Path(sExeName,sExeName.FindLast('\\')+1);
	
	
/*
#ifdef _WIN64
	BOOL bDataFileOk=TRUE;
#else
	OSVERSIONINFOEX ver;
	ver.dwOSVersionInfoSize=sizeof(OSVERSIONINFOEX);
	BOOL bDataFileOk=GetVersionEx((OSVERSIONINFO*)&ver);
	bDataFileOk=bDataFileOk && ver.dwPlatformId==VER_PLATFORM_WIN32_NT && 
		(ver.dwMajorVersion>5 || (ver.dwMajorVersion==5 && ver.dwMinorVersion>=2) ||
		(ver.dwMajorVersion==5 && ver.dwMinorVersion==1 && ver.wServicePackMajor>=2));
#endif
*/


	// We have tried LOAD_LIBRARY_AS_DATAFILE as a parameter for
	// LoadLibrary but some translation won't then work. 

	HINSTANCE hLib=FileSystem::LoadLibrary(Path+LangFile,NULL);
	
	if (hLib==NULL)
	{
		
		hLib=FileSystem::LoadLibrary(Path+L"lan_en.dll",NULL); 
		

		if (hLib==NULL)
		{
			MessageBox(NULL,ID2A(IDS_ERRORCANNOTLOADLANGUAGEFILE,CommonResource),
				ID2A(IDS_ERROR),MB_ICONERROR|MB_OK);
			return FALSE;
		}

		MessageBox(NULL,ID2A(IDS_ERRORCANNOTLOADLANGUAGEFILE2,CommonResource),
			ID2A(IDS_ERROR),MB_ICONERROR|MB_OK);
	}

	SetResourceHandle(hLib,LanguageSpecificResource);

	// Set help file
	LangFile.LoadString(IDS_HELPFILE);
	if (FileSystem::IsFile(Path+LangFile))
		GetApp()->SetHelpFile(LangFile);

	return TRUE;
}


BOOL CLocateApp::GlobalUpdate(CArray<PDATABASE>* paDatabasesArg,int nThreadPriority)
{
	if (IsUpdating())
		return FALSE;

	CArray<PDATABASE>* paDatabases;
	if (paDatabasesArg==NULL)
	{
		paDatabases=new CArray<PDATABASE>;
		for (int i=0;i<m_aDatabases.GetSize();i++)
		{
			if (m_aDatabases[i]->IsGloballyUpdated())
				paDatabases->Add(new CDatabase(*m_aDatabases[i]));
		}
	}
	else
		paDatabases=paDatabasesArg;

	WORD wThreads=CDatabase::CheckIDs(*paDatabases);
	if (wThreads==0)
		return FALSE;

	EnterCriticalSection(&m_cUpdatersPointersInUse);	
	m_ppUpdaters=new CDatabaseUpdater*[wThreads+1];
	if (m_ppUpdaters==NULL)
		return FALSE;

	WORD wThread;
	for (wThread=0;wThread<wThreads;wThread++)
	{
		m_ppUpdaters[wThread]=new CDatabaseUpdater(*paDatabases,paDatabases->GetSize(),
			UpdateProc,wThread,(DWORD_PTR)m_pMainWnd);
	}
	m_ppUpdaters[wThreads]=NULL;
    
	// Starting
	DWORD dwRunning=0;
	for (wThread=0;wThread<wThreads;wThread++)
	{
		UpdateError ueCode=m_ppUpdaters[wThread]->Update(TRUE,nThreadPriority);
		if (ueCode==ueSuccess)
			dwRunning++;
		else
		{
			delete m_ppUpdaters[wThread];
			m_ppUpdaters[wThread]=UPDATER_EXITED(ueCode);
		}
	}
			
	if (dwRunning==0)
	{
		((CTrayIconWnd*)m_pMainWnd)->NotifyFinishingUpdating();
				
		// Freeing memory
		delete[] GetLocateApp()->m_ppUpdaters;
		GetLocateApp()->m_ppUpdaters=NULL;

		

		if (GetLocateApp()->m_nStartup&CStartData::startupExitAfterUpdating)
			m_pMainWnd->PostMessage(WM_COMMAND,IDM_EXIT,NULL);
		m_ppUpdaters=NULL;
	}
	
	LeaveCriticalSection(&m_cUpdatersPointersInUse);
	
	if (paDatabasesArg==NULL)
	{
		for (int i=0;i<paDatabases->GetSize();i++)
			delete paDatabases->GetAt(i);
		delete paDatabases;
	}
	return dwRunning>0;
}

void CLocateApp::OnInitDatabaseMenu(CMenu& PopupMenu)
{
	// Removing existing items
	for(int i=PopupMenu.GetMenuItemCount()-1;i>=0;i--)
		PopupMenu.DeleteMenu(i,MF_BYPOSITION);

	CStringW title;
	MENUITEMINFOW mi;
	mi.cbSize=sizeof(MENUITEMINFOW);
	mi.fMask=MIIM_DATA|MIIM_ID|MIIM_STATE|MIIM_TYPE;
	mi.wID=IDM_DEFUPDATEDBITEM;
	mi.fType=MFT_STRING;
	mi.fState=MFS_ENABLED;
	
	if (m_aDatabases.GetSize()==0)
	{
		// Inserting default items
		title.LoadString(IDS_EMPTY);
		mi.dwTypeData=(LPWSTR)(LPCWSTR)title;
		mi.dwItemData=0;
		mi.fState=MFS_GRAYED;
		PopupMenu.InsertMenu(mi.wID,FALSE,&mi);
		return;
	}

	// Starting to insert database items 
	for (int i=0;i<m_aDatabases.GetSize();i++)
	{
		title.FormatEx(L"&%d: %s",i+1,m_aDatabases[i]->GetName());
		mi.dwTypeData=(LPWSTR)(LPCWSTR)title;
		mi.dwItemData=m_aDatabases[i]->GetID();
		mi.wID=IDM_DEFUPDATEDBITEM+i;
		PopupMenu.InsertMenu(mi.wID,FALSE,&mi);
	}	
}

void CLocateApp::OnDatabaseMenuItem(WORD wID)
{
	int iDB=wID-IDM_DEFUPDATEDBITEM;

	ASSERT(iDB>=0 && iDB<m_aDatabases.GetSize());

	DWORD dwLength=(DWORD)istrlenw(m_aDatabases[iDB]->GetName());
	LPWSTR pDatabaseName=new WCHAR[dwLength+2];
	MemCopyW(pDatabaseName,m_aDatabases[iDB]->GetName(),dwLength);
	pDatabaseName[dwLength]='\0';
	pDatabaseName[dwLength+1]='\0';

	GetTrayIconWnd()->OnUpdate(FALSE,pDatabaseName);

	delete[] pDatabaseName;
}

int CLocateApp::GetDatabaseMenuIndex(HMENU hPopupMenu)
{
	MENUITEMINFO mii;
	mii.cbSize=sizeof(MENUITEMINFO);
	mii.fMask=MIIM_SUBMENU;
	
	for(int i=GetMenuItemCount(hPopupMenu)-1;i>=0;i--)
	{
		if (!GetMenuItemInfo(hPopupMenu,i,TRUE,&mii))
			continue;

		if (mii.hSubMenu!=NULL)
		{
			if (IsDatabaseMenu(GetSubMenu(hPopupMenu,i)))
				return i;
		}
	}
	return -1;
}


void CLocateApp::SaveRegistry() const
{
	CRegKey2 RegKey;
	if(RegKey.OpenKey(HKCU,"\\General",
		CRegKey::createNew|CRegKey::samAll)==ERROR_SUCCESS)
	{
		RegKey.SetValue("General Flags",m_dwProgramFlags&pfSave);

		RegKey.SetValue(L"DateFormat",m_strDateFormat);
		RegKey.SetValue(L"TimeFormat",m_strTimeFormat);
		RegKey.SetValue("SizeFormat",(DWORD)m_nFileSizeFormat);
	}


}

void CLocateApp::LoadRegistry()
{
	// When modifications are done, check whether 
	// function is applicable for UpdateSettings

	CRegKey2 RegKey;
	m_strDateFormat.Empty();
	m_strTimeFormat.Empty();

	if (RegKey.OpenKey(HKCU,"\\General",
		CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
	{
		// Loading dwFlags
		DWORD temp=m_dwProgramFlags;
		RegKey.QueryValue("General Flags",temp);
		m_dwProgramFlags&=~pfSave;
		m_dwProgramFlags|=temp&pfSave;



		RegKey.QueryValue(L"DateFormat",m_strDateFormat);
		RegKey.QueryValue(L"TimeFormat",m_strTimeFormat);
		RegKey.QueryValue("SizeFormat",*((DWORD*)&m_nFileSizeFormat));

	}
}

BOOL CLocateApp::UpdateSettings()
{
	CRegKey2 RegKey;
	if (RegKey.OpenKey(HKCU,"\\General",
		CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
	{
		// Loading dwFlags
		DWORD dwNewFlags=m_dwProgramFlags;
		RegKey.QueryValue("General Flags",dwNewFlags);
		
		if (dwNewFlags&pfDontShowSystemTrayIcon &&
			!(m_dwProgramFlags&pfDontShowSystemTrayIcon))
			m_AppWnd.DeleteTaskbarIcon(TRUE);
		else if (!(dwNewFlags&pfDontShowSystemTrayIcon) &&
			m_dwProgramFlags&pfDontShowSystemTrayIcon)
		{
			m_AppWnd.AddTaskbarIcon(TRUE);
			m_AppWnd.SetTrayIcon(NULL,IDS_NOTIFYLOCATE);
		}
				

		m_dwProgramFlags&=~pfSave;
		m_dwProgramFlags|=dwNewFlags&pfSave;
	}

	return TRUE;
}


BOOL CLocateApp::InitCommonRegKey(LPCWSTR lpCmdLine)
{
	LPWSTR pSettingBranch=NULL;

	// Parse parameters
	ParseSettingsBranch(lpCmdLine,pSettingBranch);

	
	m_szCommonRegKey=ReadIniFile(&m_szCommonRegFile,
		pSettingBranch!=NULL?W2A(pSettingBranch):NULL,m_bFileIsReg);
	
	if (pSettingBranch!=NULL)
		delete[] pSettingBranch;

	if (m_szCommonRegKey!=NULL)
	{
		if (m_szCommonRegFile!=NULL)
			LoadSettingsFromFile(m_szCommonRegKey,m_szCommonRegFile,m_bFileIsReg);
	}
	else
	{
		// Use default
		m_szCommonRegKey=alloccopy("Software\\Update");
	}

	return TRUE;
}

void CLocateApp::FinalizeCommonRegKey()
{
	if (m_szCommonRegKey==NULL)
		return;
	
	if (m_szCommonRegFile!=NULL)
	{
		SaveSettingsToFile(m_szCommonRegKey,m_szCommonRegFile,m_bFileIsReg);

		delete[] m_szCommonRegFile;
	}

	delete[] m_szCommonRegKey;
}

CAutoPtrA<CHAR> CLocateApp::GetRegKey(LPCSTR szSubKey)
{
	extern CLocateApp theApp;

	int nCommonKeyLen=istrlen(theApp.m_szCommonRegKey);
	int nSubKeyLen=istrlen(szSubKey)+1;

	char* pKey=new char[nCommonKeyLen+nSubKeyLen+(szSubKey[0]!='\\')];

	MemCopy(pKey,theApp.m_szCommonRegKey,nCommonKeyLen);
	
	if (szSubKey[0]!='\\')
		pKey[nCommonKeyLen++]='\\';

	MemCopy(pKey+nCommonKeyLen,szSubKey,nSubKeyLen);

	return pKey;
}

LPWSTR CLocateApp::FormatLastOsError()
{
	if (IsUnicodeSystem())
	{
		LPWSTR pError;
		if (FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,NULL,
			GetLastError(),LANG_USER_DEFAULT,(LPWSTR)&pError,0,NULL))
			return pError;
		return NULL;
	}
	else
	{
		LPSTR pError;
		DWORD dwLen=FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,NULL,
			GetLastError(),LANG_USER_DEFAULT,(LPSTR)&pError,0,NULL);
		if (dwLen>0)
		{
			LPWSTR pErrorW=(LPWSTR)LocalAlloc(LMEM_FIXED,dwLen+2);
			MultiByteToWideChar(CP_ACP,0,pError,dwLen+1,pErrorW,dwLen+2);
			LocalFree(pError);
			return pErrorW;
		}
		return NULL;
	}
}

BOOL CLocateApp::OpenHelp(HWND hWnd,LPCSTR szHelpPage,HELPINFO* lphi,HelpID* pHelpID,int nIDs)
{
	LPCWSTR szwHelpFile=GetApp()->m_szHelpFile;
	if (szwHelpFile==NULL)
		return FALSE;

	// Form path to help file
	CStringW sHelpFile=GetApp()->GetExeNameW();
	sHelpFile.FreeExtra(sHelpFile.FindLast(L'\\')+1);
	sHelpFile << szwHelpFile << "::/";
	
	if (szHelpPage!=NULL)
		sHelpFile << szHelpPage;

	if (lphi!=NULL)
	{
		if (lphi->iContextType!=HELPINFO_WINDOW || pHelpID==NULL)
			return FALSE;


		for (int i=0;i<nIDs;i++)
		{
			if (pHelpID[i].nID==lphi->iCtrlId)
			{
				if (szHelpPage!=NULL)
					sHelpFile << '#';
				sHelpFile << pHelpID[i].lpName;
				break;
			}
		}
	}


	if (IsUnicodeSystem())
	{
		if (HtmlHelpW(hWnd,sHelpFile,HH_DISPLAY_TOPIC,NULL)!=NULL)
			return TRUE;
	}
	else
	{
		if (HtmlHelpA(hWnd,W2A(sHelpFile),HH_DISPLAY_TOPIC,NULL)!=NULL)
			return TRUE;
	}
	return FALSE;
}


/////////////////////////////////////////////
// CLocateApp::LocaleNumberFormat

CLocateApp::LocaleNumberFormat::LocaleNumberFormat()
:	uLeadingZero(1),uGrouping(3)
{
	
	CRegKey RegKey;

	BOOL bRet=RegKey.OpenKey(HKCU,"Control Panel\\International",CRegKey::defRead)==ERROR_SUCCESS;

	if (!bRet)
		bRet=RegKey.OpenKey(HKEY_USERS,".DEFAULT\\Control Panel\\International",CRegKey::defRead)==ERROR_SUCCESS;

	if (!bRet)
	{
		pDecimal=new WCHAR[2];
		pDecimal[0]=L'.';
		pDecimal[1]=L'\0';
		pThousand=new WCHAR[2];
		pThousand[0]=L' ';
		pThousand[1]=L'\0';
		return;
	}



	WCHAR szTemp[10];
	int nLen;

	if (RegKey.QueryValue(L"iLZero",szTemp,10)>1)
		uLeadingZero=_wtoi(szTemp);
	if (RegKey.QueryValue(L"sGrouping",szTemp,10)>1)
		uGrouping=_wtoi(szTemp);

	nLen=RegKey.QueryValueLength(L"sDecimal");
	if (nLen>1)
	{
		pDecimal=new WCHAR[nLen+1];
		RegKey.QueryValue(L"sDecimal",pDecimal,nLen);
	}
	else
	{
		pDecimal=new WCHAR[2];
		pDecimal[0]=L'.';
		pDecimal[1]=L'\0';
	}

	nLen=RegKey.QueryValueLength(L"sThousand");
	if (nLen>1)
	{
		pThousand=new WCHAR[nLen+1];
		RegKey.QueryValue(L"sThousand",pThousand,nLen);
	}
	else
	{
		pThousand=new WCHAR[2];
		pThousand[0]=L' ';
		pThousand[1]=L'\0';
	}

}


/////////////////////////////////////////////
// Global variables

CLocateApp theApp;

