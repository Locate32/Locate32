/* Locate32 - Copyright (c) 1997-2010 Janne Huttunen */

#include <HFCLib.h>
#include "Locate32.h"



/////////////////////////////////////////////
// Definitions

// For CTrayIconWnd
#define MAXTIPTEXTLENGTH		128
#define MAXINFOTEXTLENGTH		256
#define MAXINFOTITLELENGTH		64

// For CTrayIconWnd::CUpdateStatusWnd
#define EXTRA_MARGINSX 3
#define EXTRA_MARGINSY 3
#define EXTRA_LINES 2
#define UPDATETIPBUTTON_CX 9
#define UPDATETIPBUTTON_CY 9
#define UPDATETIPBUTTON_SPACEX 2



/////////////////////////////////////////////
// CTrayIconWnd 

CTrayIconWnd::CTrayIconWnd()
:	m_pAbout(NULL),m_pSettings(NULL),m_hAppIcon(NULL),
	m_pLocateDlgThread(NULL),m_pCpuUsage(NULL),
	m_pUpdateAnimIcons(NULL),m_hHook(NULL)
{
	DebugMessage("CTrayIconWnd::CTrayIconWnd()");



	InitializeCriticalSection(&m_csAnimBitmaps);
	InitializeCriticalSection(&m_csLocateThread);
}

CTrayIconWnd::~CTrayIconWnd()
{
	DebugMessage("CTrayIconWnd::~CTrayIconWnd()");
	//m_Schedules.RemoveAll();

	DeleteCriticalSection(&m_csAnimBitmaps);
	DeleteCriticalSection(&m_csLocateThread);
	
	if (m_pCpuUsage!=NULL)
		delete m_pCpuUsage;
}


int CTrayIconWnd::OnCreate(LPCREATESTRUCT lpcs)
{
	// Loading menu
	m_Menu.LoadMenu(IDR_SYSTEMTRAYMENU);


	// Set schedules
	SetSchedules(NULL,TRUE);
	
	SetMenuDefaultItem(m_Menu.GetSubMenu(0),IDM_OPENLOCATE,FALSE);
	
	// Setting icons
	HICON hIcon=(HICON)LoadImage(IDI_APPLICATIONICON,IMAGE_ICON,32,32,LR_SHARED);
	SetIcon(hIcon,TRUE);
	SetClassLong(gclHIcon,(LONG)hIcon);

	LoadAppIcon();

	return CFrameWnd::OnCreate(lpcs);
}

BOOL CTrayIconWnd::OnCreateClient(LPCREATESTRUCT lpcs)
{
	AddTaskbarIcon();

	BOOL bDoOpen=FALSE;

	// Opening dialog if STARTUP_DONOTOPENDIALOG is not set 
	if (GetLocateApp()->GetStartupFlags()&CLocateApp::CStartData::startupDoNotOpenDialog)
		GetLocateApp()->ClearStartData();
	else
		bDoOpen=TRUE;
	
	// Updating database if necessary
	if (GetLocateApp()->IsStartupFlagSet(CLocateApp::CStartData::startupUpdate))
		PostMessage(WM_COMMAND,MAKEWPARAM(IDM_GLOBALUPDATEDB,0),NULL);
	
	TurnOnShortcuts();

	BOOL bRet=CFrameWnd::OnCreateClient(lpcs);
	if (bDoOpen)
		OnLocate();
	return bRet;
}

/////////////////////////////////////////////
// CTrayIconWnd - other members


void CTrayIconWnd::LoadAppIcon()
{
	if (m_hAppIcon!=NULL)
	{
		DebugCloseGdiObject(m_hAppIcon);
		DeleteObject(m_hAppIcon);
	}

	
	CRegKey2 RegKey;
	if (RegKey.OpenKey(HKCU,"\\General",CRegKey::defRead)==ERROR_SUCCESS)
	{
		CStringW CustomTrayIcon;
		if (RegKey.QueryValue(L"CustomTrayIcon",CustomTrayIcon))
		{
			m_hAppIcon=(HICON)LoadImage(CustomTrayIcon,IMAGE_ICON,GetSystemMetrics(SM_CXSMICON),GetSystemMetrics(SM_CYSMICON),LR_LOADFROMFILE);
			DebugOpenGdiObject(m_hAppIcon);
			if (m_hAppIcon!=NULL)
				return;
		}
	}

	m_hAppIcon=(HICON)LoadImage(IDI_APPLICATIONICON,IMAGE_ICON,GetSystemMetrics(SM_CXSMICON),GetSystemMetrics(SM_CYSMICON),0);
	DebugOpenGdiObject(m_hAppIcon);
}



BOOL CTrayIconWnd::TurnOnShortcuts()
{
	TurnOffShortcuts();

	OSVERSIONINFOEX oi;
	oi.dwOSVersionInfoSize=sizeof(OSVERSIONINFOEX);
	BOOL bRet=GetVersionEx((LPOSVERSIONINFO) &oi);
	BOOL bCanHook=!(bRet && oi.dwPlatformId!=VER_PLATFORM_WIN32_NT ||
		!(oi.dwMajorVersion>=5 || (oi.dwMajorVersion==4 && oi.wServicePackMajor>=3) ));

	// Loading new shortcuts
	if (!CShortcut::LoadShortcuts(m_aShortcuts,(
		bCanHook?CShortcut::loadGlobalHook:0)|CShortcut::loadGlobalHotkey))
	{
        if (!CShortcut::GetDefaultShortcuts(m_aShortcuts,
			CShortcut::loadGlobalHotkey|(bCanHook?CShortcut::loadGlobalHook:0)))
			ShowErrorMessage(IDS_ERRORCANNOTLOADDEFAULTSHORTUCS,IDS_ERROR);
	}

	// Resolving mnemonics
	CLocateDlg* pLocateDlg=GetLocateDlg();
	if (pLocateDlg!=NULL)
	{
		HWND hDialogs[]={*pLocateDlg,pLocateDlg->m_NameDlg,pLocateDlg->m_SizeDateDlg,
			pLocateDlg->m_AdvancedDlg,NULL};
		CShortcut::ResolveMnemonics(m_aShortcuts,NULL);
	}
	else
	{
		HWND hDialogs[5];
		hDialogs[0]=CreateDialog(GetLanguageSpecificResourceHandle(),
			MAKEINTRESOURCE(IDD_MAIN),NULL,(DLGPROC)CLocateApp::DummyDialogProc);
		hDialogs[1]=CreateDialog(GetLanguageSpecificResourceHandle(),
			MAKEINTRESOURCE(IDD_NAME),hDialogs[0],(DLGPROC)CLocateApp::DummyDialogProc);
		hDialogs[2]=CreateDialog(GetLanguageSpecificResourceHandle(),
			MAKEINTRESOURCE(IDD_SIZEDATE),hDialogs[0],(DLGPROC)CLocateApp::DummyDialogProc);
		hDialogs[3]=CreateDialog(GetLanguageSpecificResourceHandle(),
			MAKEINTRESOURCE(IDD_ADVANCED),hDialogs[0],(DLGPROC)CLocateApp::DummyDialogProc);
		hDialogs[4]=NULL;
		CShortcut::ResolveMnemonics(m_aShortcuts,NULL);
		for (int i=0;hDialogs[i]!=NULL;i++)
            ::DestroyWindow(hDialogs[i]);
	}

	// Count hooks and register hotkeys
	UINT nHooks=0;
	for (int i=0;i<m_aShortcuts.GetSize();i++)
	{
		switch(m_aShortcuts[i]->m_dwFlags&CShortcut::sfKeyTypeMask)
		{
		case CShortcut::sfGlobalHook:
			nHooks++;
			break;
		case CShortcut::sfGlobalHotkey:
			if (!RegisterHotKey(*this,i,m_aShortcuts[i]->m_bModifiers,m_aShortcuts[i]->m_bVirtualKey))
			{
				// TODO: Error message if settings dialog is already open
			}
			break;
		}
	}

	if (nHooks>0)
	{
		ASSERT(bCanHook);
		m_hHook=SetHook(*this,m_aShortcuts.GetData(),m_aShortcuts.GetSize());
	}
		
	
	return TRUE;
}

BOOL CTrayIconWnd::TurnOffShortcuts()
{
	// Unregister hotkeys
	for (int i=0;i<m_aShortcuts.GetSize();i++)
	{
		if ((m_aShortcuts[i]->m_dwFlags&CShortcut::sfKeyTypeMask)==CShortcut::sfGlobalHotkey)
			UnregisterHotKey(*this,i);
	}

	// Stop hooking
	if (m_hHook!=NULL)
	{
		UnsetHook(m_hHook);
		m_hHook;
	}

	m_aShortcuts.RemoveAll();

	return NULL;
}


BOOL CTrayIconWnd::GetRootInfos(WORD& wThreads,WORD& wRunning,RootInfo*& pRootInfos)
{
	// Initializing
	wThreads=0;
	pRootInfos=NULL;
	wRunning=0;
	
	// Counting threads		
	EnterCriticalSection(&GetLocateApp()->m_cUpdatersPointersInUse);
	if (GetLocateApp()->m_ppUpdaters==NULL)
	{
		// Not updating
		LeaveCriticalSection(&GetLocateApp()->m_cUpdatersPointersInUse);
		return FALSE;
	}
	
	for (;GetLocateApp()->m_ppUpdaters[wThreads]!=NULL;wThreads++);

	pRootInfos=new RootInfo[max(wThreads,2)];
	
	
	for (WORD i=0;i<wThreads;i++)
	{
		if (IS_UPDATER_EXITED(GetLocateApp()->m_ppUpdaters[i]))
		{
			pRootInfos[i].pName=NULL;
			pRootInfos[i].pRoot=NULL;
			pRootInfos[i].ueError=GET_UPDATER_CODE(GetLocateApp()->m_ppUpdaters[i]);
			pRootInfos[i].usStatus=statusFinished;
		}
		else 
		{
			wRunning++;

			pRootInfos[i].usStatus=GetLocateApp()->m_ppUpdaters[i]->GetStatus();
			pRootInfos[i].ueError=ueStillWorking;

			if (GetLocateApp()->m_ppUpdaters[i]->GetCurrentDatabaseName()==NULL)
			{
				// Not started yet
				pRootInfos[i].pName=NULL;
				pRootInfos[i].pRoot=NULL;

				if (pRootInfos[i].usStatus==statusFinishing)
					pRootInfos[i].ueError=ueSuccess;

				pRootInfos[i].dwNumberOfDatabases=GetLocateApp()->m_ppUpdaters[i]->GetNumberOfDatabases();
				pRootInfos[i].dwCurrentDatabase=0;
				pRootInfos[i].wProgressState=0;
				
			}
			else
			{
				pRootInfos[i].pName=GetLocateApp()->m_ppUpdaters[i]->GetCurrentDatabaseNameStr();
				if (GetLocateApp()->m_ppUpdaters[i]->GetCurrentRoot()==NULL)
					pRootInfos[i].pRoot=NULL; // Is writing database
				else
					pRootInfos[i].pRoot=GetLocateApp()->m_ppUpdaters[i]->GetCurrentRootPathInDatabaseStr();
				
				pRootInfos[i].dwNumberOfDatabases=GetLocateApp()->m_ppUpdaters[i]->GetNumberOfDatabases();
				pRootInfos[i].dwCurrentDatabase=GetLocateApp()->m_ppUpdaters[i]->GetCurrentDatabase();
				pRootInfos[i].wProgressState=GetLocateApp()->m_ppUpdaters[i]->GetProgressStatus();
	
			}
			
		}
	}
	LeaveCriticalSection(&GetLocateApp()->m_cUpdatersPointersInUse);
	return TRUE;
}

void CTrayIconWnd::FreeRootInfos(WORD wThreads,RootInfo* pRootInfos)
{
	// Freeing memory
	if (pRootInfos==NULL)
		return;
	for (int i=0;i<wThreads;i++)
	{
		if (pRootInfos[i].pName!=NULL)
			delete[] pRootInfos[i].pName;
		if (pRootInfos[i].pRoot!=NULL)
			delete[] pRootInfos[i].pRoot;
	}
	delete[] pRootInfos;
}



BOOL CTrayIconWnd::SetTrayIcon(HICON hIcon,UINT uTip,LPCWSTR szText)
{
	if (CLocateApp::GetProgramFlags()&CLocateApp::pfDontShowSystemTrayIcon)
		return TRUE;

	NOTIFYICONDATAW nid;
	ZeroMemory(&nid,sizeof(NOTIFYICONDATAW));
	
	if (GetLocateApp()->m_wShellDllVersion>=0x0500)
		nid.cbSize=sizeof(NOTIFYICONDATAW);
	else
		nid.cbSize=NOTIFYICONDATAW_V1_SIZE;

	nid.hWnd=*this;
	nid.uID=1000;
	nid.uFlags=0;
		
	if (hIcon!=NULL)
	{
		// Updating icon
		if (hIcon==INVALID_HANDLE_VALUE)
			nid.hIcon=m_hAppIcon;
		else
			nid.hIcon=hIcon;
		nid.uFlags|=NIF_ICON;
	}
	
	if (uTip==IDS_NOTIFYUPDATING)
	{
		nid.uFlags|=NIF_TIP;
		
		WORD wThreads,wRunning;
		RootInfo* pRootInfos;
		
		if (!GetRootInfos(wThreads,wRunning,pRootInfos))
		{
			// TODO
			m_pUpdateStatusWnd->Update(0,0,NULL);
			NotifyFinishingUpdating();			
			return FALSE;
		}

		
			
		if (m_pUpdateStatusWnd!=NULL)
			m_pUpdateStatusWnd->Update(wThreads,wRunning,pRootInfos);
			
		if (wThreads>1)
		{
			if (((CLocateApp*)GetApp())->m_wShellDllVersion>=0x0500 &&	wThreads<10)
			{
				// Loading string
				WCHAR szThread[20];
				int iThreadLen=LoadString(IDS_NOTIFYTHREAD,szThread,20);
				WCHAR szCaption[30];
				int iCaptionLen=LoadString(IDS_NOTIFYUPDATINGDBS2,szCaption,30);
                WCHAR szDone[20];
				int iDoneLen=LoadString(IDS_NOTIFYDONE,szDone,20);
				WCHAR szWriting[25];
				int iWritingLen=LoadString(IDS_NOTIFYWRITINGDATABASE,szWriting,25);
				WCHAR szInitializing[25];
				int iInitializingLen=LoadString(IDS_NOTIFYINITIALIZING,szInitializing,25);

				// Computing required length for string
				int iRequired=iCaptionLen;
				int iRequiredForRoots=0;

				// Format: Thread N: name, root
				for (int i=0;i<wThreads;i++)
				{
					iRequired+=iThreadLen+1+4;
					if (pRootInfos[i].pName==NULL)
						iRequired+=iDoneLen;
					else if (pRootInfos[i].pName[0]=='\0')
						iRequired+=iInitializingLen;
					else
					{
						iRequired+=(int)istrlenw(pRootInfos[i].pName);
						if (pRootInfos[i].pRoot==NULL)
							iRequiredForRoots+=iWritingLen+2;
						else
                        	iRequiredForRoots+=(int)istrlenw(pRootInfos[i].pRoot)+2;
					}
				}
				
				if (iRequired>=MAXTIPTEXTLENGTH)
				{
					WCHAR szTemp[54];
					LoadString(IDS_NOTIFYUPDATINGDBS,szTemp,54);
					StringCbPrintfW(nid.szTip,64*sizeof(WCHAR),szTemp,(int)wRunning,(int)wThreads);
				}
				else
				{
					LPWSTR pPtr=nid.szTip;
					StringCbCopyNW(pPtr,128*sizeof(WCHAR),szCaption,iCaptionLen);
					pPtr+=iCaptionLen;

					for (int i=0;i<wThreads;i++)
					{
						*(pPtr++)='\n';

						CopyMemory(pPtr,szThread,iThreadLen);
						pPtr+=iThreadLen;
						*(pPtr++)=' ';
						*(pPtr++)='1'+char(i);
						*(pPtr++)=':';
						*(pPtr++)=' ';
                        
						if (pRootInfos[i].pName==NULL)
						{
							MemCopyW(pPtr,szDone,iDoneLen);
							pPtr+=iDoneLen;
						}
						else if (pRootInfos[i].pName[0]=='\0')
						{
							MemCopyW(pPtr,szInitializing,iInitializingLen);
							pPtr+=iInitializingLen;
						}
						else
						{
							int iLen=(int)istrlenw(pRootInfos[i].pName);
							MemCopyW(pPtr,pRootInfos[i].pName,iLen);
							pPtr+=iLen;
							if (iRequired+iRequiredForRoots<MAXTIPTEXTLENGTH)
							{
								*(pPtr++)=',';
								*(pPtr++)=' ';
								if (pRootInfos[i].pRoot==NULL)
								{
									MemCopyW(pPtr,szWriting,iWritingLen);
									pPtr+=iWritingLen;
								}
								else
								{
									int iLen=(int)istrlenw(pRootInfos[i].pRoot);
									MemCopyW(pPtr,pRootInfos[i].pRoot,iLen);
									pPtr+=iLen;
								}
							}
						}

					}
					
					// Checking that space is correctly calculated
					ASSERT(iRequired+iRequiredForRoots<MAXTIPTEXTLENGTH?
						DWORD(pPtr-nid.szTip)==iRequired+iRequiredForRoots:
						DWORD(pPtr-nid.szTip)==iRequired);

					*pPtr='\0';
				}
			}
			else
			{
				WCHAR szTemp[54];
                LoadString(IDS_NOTIFYUPDATINGDBS,szTemp,54);
				StringCbPrintfW(nid.szTip,64*sizeof(WCHAR),szTemp,(int)wRunning,(int)wThreads);
			}
		}
		else
		{
			// Only one thread
			if (pRootInfos[0].pRoot==NULL) // Is writing database
				StringCbPrintfW(nid.szTip,64*sizeof(WCHAR),(LPCWSTR)ID2W(IDS_UPDATINGWRITINGDATABASE));
			else
			{
				WCHAR szBuf[50];
				LoadString(IDS_NOTIFYUPDATING,szBuf,50);
			
				LPWSTR pRoot=pRootInfos[0].pRoot;

				// Cutting to 35 characters
				int i;
				for (i=0;i<35 && pRoot[i]!='\0';i++); 
				if (i==35)
				{
					pRoot[32]='.';
					pRoot[33]='.';
					pRoot[34]='.';
					pRoot[35]='\0';
				}
				StringCbPrintfW(nid.szTip,64*sizeof(WCHAR),szBuf,pRoot);
			}
		}


		FreeRootInfos(wThreads,pRootInfos);
			
	}
	else if (uTip!=0)
	{
		if (szText!=NULL)
		{
			WCHAR szString[64];
			LoadString(uTip,szString,63);
			StringCbPrintfW(nid.szTip,63*sizeof(WCHAR),szString,szText);
		}
		else
			LoadString(uTip,nid.szTip,63);
		nid.uFlags|=NIF_TIP;
	}
	
	//DebugMessage("CTrayIconWnd::SetUpdateStatusInformation: END");
	
	return Shell_NotifyIconW(NIM_MODIFY,&nid);
}


BOOL CTrayIconWnd::StartUpdateStatusNotification()
{
	// Closing existing update status window if necessary
	if (m_pUpdateStatusWnd!=NULL)
	{
		m_pUpdateStatusWnd->DestroyWindow();
		delete m_pUpdateStatusWnd;
		m_pUpdateStatusWnd=NULL;
	}
	
	EnterCriticalSection(&m_csAnimBitmaps);
	if (m_pUpdateAnimIcons==NULL)
	{
		m_pUpdateAnimIcons=new HICON[COUNT_UPDATEANIMATIONS];

		for (int i=0;i<COUNT_UPDATEANIMATIONS;i++)
		{
			m_pUpdateAnimIcons[i]=(HICON)LoadImage(IDI_UANIM1+i,IMAGE_ICON,16,16,0);
			DebugOpenGdiObject(m_pUpdateAnimIcons[i]);
		}
		
		m_nCurUpdateAnimBitmap=0;
		SetTimer(ID_UPDATEANIM,100);
	}
	LeaveCriticalSection(&m_csAnimBitmaps);
	
	if (CLocateApp::GetProgramFlags()&CLocateApp::pfEnableUpdateTooltip && 
		m_pUpdateStatusWnd==NULL)
	{
		m_pUpdateStatusWnd=new CUpdateStatusWnd;
		
		// Registering window class for notify icon handler window
		BOOL(WINAPI * pSetLayeredWindowAttributes)(HWND,COLORREF,BYTE,DWORD)=(BOOL(WINAPI *)(HWND,COLORREF,BYTE,DWORD))GetProcAddress(GetModuleHandle("user32.dll"),"SetLayeredWindowAttributes");
	
		BYTE nTransparency=0;
		DWORD dwExtra=WS_EX_TOOLWINDOW/*|WS_EX_NOACTIVATE*/;
		
		if ((CLocateApp::GetProgramFlags()&CLocateApp::pfUpdateTooltipTopmostMask)==
			CLocateApp::pfUpdateTooltipAlwaysTopmost)
			dwExtra|=WS_EX_TOPMOST;
		else if((CLocateApp::GetProgramFlags()&CLocateApp::pfUpdateTooltipTopmostMask)!=
			CLocateApp::pfUpdateTooltipNeverTopmost)
		{
			// Depends on foreground window
			HWND hForegroundWindow=GetForegroundWindow();
			if (hForegroundWindow==NULL)
				dwExtra|=WS_EX_TOPMOST;
			else
			{
				LONG dwStyle=::GetWindowLong(hForegroundWindow,GWL_STYLE);
				if (dwStyle&WS_MAXIMIZE)
				{
					if ((CLocateApp::GetProgramFlags()&CLocateApp::pfUpdateTooltipTopmostMask)==
						CLocateApp::pfUpdateTooltipNoTopmostWhdnForegroundWndInFullScreen &&
						dwStyle&WS_CAPTION)
						dwExtra|=WS_EX_TOPMOST;
				}
				else
					dwExtra|=WS_EX_TOPMOST;
      		}
		}

		if (pSetLayeredWindowAttributes!=NULL)
		{
			CRegKey2 RegKey;
			if (RegKey.OpenKey(HKCU,"Dialogs\\Updatestatus",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
			{
				DWORD dwTemp;
				if (RegKey.QueryValue("Transparency100",dwTemp))
					nTransparency=(BYTE)((min(dwTemp,100)*255)/100);
				else if (RegKey.QueryValue("Transparency",dwTemp))
					nTransparency=(BYTE)min(dwTemp,255);

			}

			if (nTransparency>0)
				dwExtra|=WS_EX_LAYERED;
		}
		
		m_pUpdateStatusWnd->Create("LOCATEAPPUPDATESTATUS","US",WS_POPUPWINDOW/*|WS_VISIBLE*/,
			NULL,NULL,LPCSTR(0),dwExtra);

		if (nTransparency>0)
			pSetLayeredWindowAttributes(*m_pUpdateStatusWnd,0,255-nTransparency,LWA_ALPHA);

		m_pUpdateStatusWnd->SetPosition();
		m_pUpdateStatusWnd->ShowWindow(CWnd::swShowNA);
		
		
	}

	return TRUE;
}

BOOL CTrayIconWnd::StopUpdateStatusNotification()
{
	if (m_pUpdateStatusWnd!=NULL)
		m_pUpdateStatusWnd->IdleClose();
	
	EnterCriticalSection(&m_csAnimBitmaps);
	if (m_pUpdateAnimIcons!=NULL)
	{
		KillTimer(ID_UPDATEANIM);
		for (int i=0;i<COUNT_UPDATEANIMATIONS;i++)
		{
			DebugCloseGdiObject(m_pUpdateAnimIcons[i]);
			DestroyIcon(m_pUpdateAnimIcons[i]);
		}
		delete[] m_pUpdateAnimIcons;
		m_pUpdateAnimIcons=NULL;
		GetTrayIconWnd()->SetTrayIcon(DEFAPPICON,IDS_NOTIFYLOCATE);
	}
	LeaveCriticalSection(&m_csAnimBitmaps);
	return TRUE;
}
	
void CTrayIconWnd::OnInitMenuPopup(HMENU hPopupMenu,UINT nIndex,BOOL bSysMenu)
{
	CFrameWnd::OnInitMenuPopup(hPopupMenu,nIndex,bSysMenu);

	if (bSysMenu)
		return;

	if (CLocateApp::IsDatabaseMenu(hPopupMenu))
		GetLocateApp()->OnInitDatabaseMenu(CMenu(hPopupMenu));
	else if (hPopupMenu==m_Menu.GetSubMenu(0))
	{
		int iDatabaseMenu=CLocateApp::GetDatabaseMenuIndex(hPopupMenu);
		if (iDatabaseMenu!=-1)
			EnableMenuItem(hPopupMenu,iDatabaseMenu,!GetLocateApp()->IsUpdating()?MF_BYPOSITION|MF_ENABLED:MF_BYPOSITION|MF_GRAYED);

		EnableMenuItem(hPopupMenu,IDM_GLOBALUPDATEDB,!GetLocateApp()->IsUpdating()?MF_BYCOMMAND|MF_ENABLED:MF_BYCOMMAND|MF_GRAYED);
		EnableMenuItem(hPopupMenu,IDM_UPDATEDATABASES,!GetLocateApp()->IsUpdating()?MF_BYCOMMAND|MF_ENABLED:MF_BYCOMMAND|MF_GRAYED);
		EnableMenuItem(hPopupMenu,IDM_STOPUPDATING,GetLocateApp()->IsUpdating()?MF_BYCOMMAND|MF_ENABLED:MF_BYCOMMAND|MF_GRAYED);
	}
}

BOOL CTrayIconWnd::OnCommand(WORD wID,WORD wNotifyCode,HWND hControl)
{
	CFrameWnd::OnCommand(wID,wNotifyCode,hControl);
	switch (wID)
	{
	case IDM_OPENLOCATE:
		OnLocate();
		break;
	case IDM_GLOBALUPDATEDB:
		if (GetLocateApp()->GetProgramFlags()&CLocateApp::pfAskConfirmationForUpdateDatabases)
		{
			CLocateDlg* pLocateDlg=GetLocateDlg();
			if ((pLocateDlg!=NULL?(CWnd*)pLocateDlg:this)->ShowErrorMessage(IDS_UPDATINGCONFIRMATION,IDS_UPDATINGDATABASE,MB_ICONQUESTION|MB_YESNO)==IDNO)
				break;
		}
		OnUpdate(FALSE);
		break;
	case IDM_UPDATEDATABASES:
		OnUpdate(FALSE,LPWSTR(-1));
		break;
	case IDM_STOPUPDATING:
		// Stopping updating quite nicely
		if (GetLocateApp()->IsUpdating())
			OnUpdate(TRUE);
		break;
	case IDM_SETTINGS:
		if (GetLocateDlg()!=NULL)
			GetLocateDlg()->SendMessage(WM_COMMAND,MAKEWPARAM(wID,wNotifyCode),(LPARAM)hControl);
		else
			OnSettings();
		break;
	case IDM_ABOUT:
		OnAbout();
		break;
	case IDC_COMEWITHME:
		// Locate dialog is closed
		if (GetLocateApp()->IsStartupFlagSet(CLocateApp::CStartData::startupLeaveBackground))
		{
			// Showing LocateST
			break;
		}
		if (GetLocateApp()->IsUpdating())
		{
			GetLocateApp()->SetStartupFlag(CLocateApp::CStartData::startupExitAfterUpdating);
			break;
		}
		if (m_pSettings!=NULL)
			break;
	case IDM_EXIT:
		if (GetLocateApp()->IsUpdating())
		{
			if (m_pLocateDlgThread!=NULL)
			{
				if (m_pLocateDlgThread->m_pLocate->ShowErrorMessage(IDS_QUITNOW,IDS_UPDATINGDATABASE,MB_YESNO|MB_ICONQUESTION)==IDYES)
					DestroyWindow();
			}
			else
			{
				switch (ShowErrorMessage(IDS_QUITNOW2,IDS_UPDATINGDATABASE,MB_YESNOCANCEL|MB_ICONQUESTION))
				{
				case IDCANCEL:
					GetLocateApp()->ClearStartupFlag(CLocateApp::CStartData::startupExitAfterUpdating);
					break;
				case IDNO:
					GetLocateApp()->SetStartupFlag(CLocateApp::CStartData::startupExitAfterUpdating);
					break;
				case IDYES:
					DestroyWindow();
					break;
				}
			}
		}
		else
			DestroyWindow();
		break;
	default:
		if (wID>=IDM_DEFUPDATEDBITEM && wID<IDM_DEFUPDATEDBITEM+1000)
			GetLocateApp()->OnDatabaseMenuItem(wID);
		break;
	}
	return FALSE;
}

BYTE CTrayIconWnd::OnAbout()
{
	if (m_pAbout==NULL)
	{
		m_pAbout=new CAboutDlg;

		if (GetLocateDlg()!=NULL)
			m_pAbout->DoModal(*GetLocateDlg());
		else
            m_pAbout->DoModal();
		delete m_pAbout;
		m_pAbout=NULL;
	}
	else
		m_pAbout->SetForegroundWindow();
	return TRUE;
}

BYTE CTrayIconWnd::OnSettings(int nPage)
{
	GetLocateApp()->ClearStartupFlag(CLocateApp::CStartData::startupExitAfterUpdating);
	
	if (m_pSettings==NULL)
	{
		// Creating new settings dialog
		if (GetLocateDlg()==NULL)
			m_pSettings=new CSettingsProperties(NULL);
		else
			m_pSettings=new CSettingsProperties(*GetLocateDlg());

		// Loading settings
		m_pSettings->LoadSettings();

		// Set active page
		if (nPage!=-1)
			m_pSettings->SetActivePage(nPage);

		// Opening dialog
		m_pSettings->DoModal();
		
		if (!m_pSettings->IsSettingsFlagSet(CSettingsProperties::settingsCancelled))
		{
			// Saving settings to registry
			m_pSettings->SaveSettings();
			
			// Set CTrayIconWnd to use new settings
			GetLocateApp()->UpdateSettings();
			if (GetLocateApp()->m_nInstance==0)
			{
				SetSchedules(m_pSettings->GetSchedules());
				SaveSchedules();
			}

			// Set icon
			LoadAppIcon();
			SetTrayIcon(DEFAPPICON,0);
			
			// Set LocateDlg to use new seetings
			CLocateDlg* pLocateDlg=GetLocateDlg();
			if (pLocateDlg!=NULL)
				pLocateDlg->UpdateSettings();
			
		}
		
		// Freeing memory
		delete m_pSettings;
		m_pSettings=NULL;

		
	}
	else
	{
		// Settings dialog is already opened, just activating it

		// Set active page
		if (nPage!=-1)
			m_pSettings->SetActivePage(nPage);

		m_pSettings->SetWindowPos(HWND_TOP,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
		m_pSettings->SetForegroundWindow();
	}
	return TRUE;
}

BYTE CTrayIconWnd::OnLocate()
{
	//DebugMessage("CTrayIconWnd::OnLocate() BEGIN");
	GetLocateApp()->ClearStartupFlag(CLocateApp::CStartData::startupExitAfterUpdating);
	
	// Refreshing icon
	EnterCriticalSection(&m_csAnimBitmaps);
	SetTrayIcon(m_pUpdateAnimIcons!=NULL?m_pUpdateAnimIcons[m_nCurUpdateAnimBitmap]:NULL);
	LeaveCriticalSection(&m_csAnimBitmaps);
	
	EnterCriticalSection(&m_csLocateThread);
	if (m_pLocateDlgThread==NULL)
	{
		// Hiding LocateST
		if (GetFocus()==NULL)
			ForceForegroundAndFocus();
		
		m_pLocateDlgThread=new CLocateDlgThread;
		m_pLocateDlgThread->CreateThread();



		while (m_pLocateDlgThread->m_pLocate==NULL)
			Sleep(10);
		while (m_pLocateDlgThread->m_pLocate->GetHandle()==NULL)
			Sleep(10);

		ShowWindow(swHide);

		
		m_pLocateDlgThread->m_pLocate->ForceForegroundAndFocus();
		

		
	}
	else
	{

		ForceForegroundAndFocus();
		
		CLocateDlg* pLocateDlg=GetLocateDlg();
		
		// pLocateDlg should not be NULL, but just for sure
		if (pLocateDlg!=NULL)
		{
			
			// Restore dialog if needed
			WINDOWPLACEMENT wp;
			wp.length=sizeof(WINDOWPLACEMENT);
			pLocateDlg->GetWindowPlacement(&wp);
			if (wp.showCmd!=SW_MAXIMIZE)
				pLocateDlg->ShowWindow(swRestore);



			pLocateDlg->BringWindowToTop();


			pLocateDlg->ForceForegroundAndFocus();
		}
	}
	LeaveCriticalSection(&m_csLocateThread);
	return TRUE;
}


void CTrayIconWnd::SetLocateDlgThreadToNull()
{
	EnterCriticalSection(&m_csLocateThread);
	InterlockedExchangePointer((PVOID*)&m_pLocateDlgThread,NULL);
	LeaveCriticalSection(&m_csLocateThread);
}

DWORD WINAPI CTrayIconWnd::KillUpdaterProc(LPVOID lpParameter)
{
	return ((CLocateApp*)GetApp())->StopUpdating(TRUE);
}



BYTE CTrayIconWnd::OnUpdate(BOOL bStopIfProcessing,LPWSTR pDatabases,int nThreadPriority)
{
	if (!GetLocateApp()->IsUpdating())
	{
		if (pDatabases==LPWSTR(-1))
		{
			CArrayFP<PDATABASE> aDatabases;
			CSelectDatabasesDlg dbd(GetLocateApp()->GetDatabases(),aDatabases,
				(GetLocateApp()->GetStartupFlags()&CLocateApp::CStartData::startupDatabasesOverridden?CSelectDatabasesDlg::flagDisablePresets:0)|
				CSelectDatabasesDlg::flagShowThreads|CSelectDatabasesDlg::flagSetUpdateState|CSelectDatabasesDlg::flagEnablePriority,
				CRegKey2::GetCommonKey()+"\\Dialogs\\SelectDatabases/Update");
			dbd.SetThreadPriority(nThreadPriority);
			
			CLocateDlg* pLocateDlg=GetLocateDlg();
			if (!dbd.DoModal(pLocateDlg!=NULL?HWND(*pLocateDlg):HWND(*this)))
                return FALSE;
			

			if (GetLocateApp()->IsUpdating())
				return FALSE;
			if (!GetLocateApp()->GlobalUpdate(&aDatabases,dbd.GetThreadPriority()))
				return FALSE;
		}
		else if (pDatabases==NULL)
		{
			if (!GetLocateApp()->GlobalUpdate(NULL,nThreadPriority))
				return FALSE;
		}
		else
		{
			const CArray<PDATABASE>& aGlobalDatabases=GetLocateApp()->GetDatabases();
			CArrayFP<PDATABASE> aDatabases;
			
			
			
			LPWSTR pPtr=pDatabases;
			while (*pPtr!=NULL)
			{
				int iStrLen=istrlenw(pPtr);
				int iNameLen=FirstCharIndex(pPtr,L'\\');
				
				if (iNameLen==-1)
					iNameLen=iStrLen;

				int iDB;
				for (iDB=0;iDB<aGlobalDatabases.GetSize();iDB++)
				{
					if (wcsncmp(pPtr,aGlobalDatabases[iDB]->GetName(),iNameLen)==0)
					{
						if (pPtr[iNameLen]==L'\0' || pPtr[iNameLen]==L'\\')
						{
							CDatabase* pDatabase=new CDatabase(*aGlobalDatabases[iDB]);
							pDatabase->UpdateGlobally(TRUE);
							if (iNameLen!=iStrLen)
								pDatabase->SetThreadId(_wtoi(pPtr+iNameLen+1));
							else
								pDatabase->SetThreadId(0);
							aDatabases.Add(pDatabase);
							break;
						}
					}
				}
				
				pPtr+=iStrLen+1;
			}

			
			if (aDatabases.GetSize()==0)
				return FALSE;
			if (!GetLocateApp()->GlobalUpdate(&aDatabases,nThreadPriority))
				return FALSE;
		}

		
	}
	else if (bStopIfProcessing)
	{
		// Starting thread which stops updating		
		DWORD dwThreadID;
		HANDLE hThread=CreateThread(NULL,0,KillUpdaterProc,(void*)this,0,&dwThreadID);
		DebugFormatMessage("KILLUPDATER: thread started ID=%X",dwThreadID);

		DebugOpenThread(hThread);
		if (hThread!=NULL)
		{
			CloseHandle(hThread);
			DebugCloseThread(hThread);
		}
	}
	return TRUE;
}

void CTrayIconWnd::OnDestroy()
{
	DebugMessage("void CTrayIconWnd::OnDestroy() START");
	
	TurnOffShortcuts();
	DeleteTaskbarIcon();

	PostQuitMessage(0);
	m_Menu.DestroyMenu();

	KillTimer(ID_CHECKSCHEDULES);
	
	// Ensure that update animation and status window are stopped
	StopUpdateStatusNotification();
	if (m_pUpdateStatusWnd!=NULL)
		m_pUpdateStatusWnd->DestroyWindow();
	
	
	



	if (m_pAbout!=NULL)
	{
		m_pAbout->DestroyWindow();
		delete m_pAbout;
		m_pAbout=NULL;
	}
	


	//EnterCriticalSection(m_csLocateThread);
	EnterCriticalSection(&m_csLocateThread);
	if (m_pLocateDlgThread!=NULL)
	{
		HANDLE hLocateThread;
		DuplicateHandle(GetCurrentProcess(),m_pLocateDlgThread->m_hThread,GetCurrentProcess(),
                    &hLocateThread,0,FALSE,DUPLICATE_SAME_ACCESS);
		DebugOpenThread(hLocateThread);

		if (m_pLocateDlgThread->IsRunning())
		{
			LeaveCriticalSection(&m_csLocateThread);
			
			
			::GetLocateDlg()->PostMessage(WM_CLOSEDIALOG);
			
#ifdef _DEBUG_LOGGING
			WaitForSingleObject(hLocateThread,1000);
#else
			WaitForSingleObject(hLocateThread,500);
#endif

		
			EnterCriticalSection(&m_csLocateThread);
	
			if (m_pLocateDlgThread!=NULL)
			{
				DebugFormatMessage("Terminating locate dialog thread %X",(DWORD)m_pLocateDlgThread);
				TerminateThread(hLocateThread,1,TRUE);
				
				if (m_pLocateDlgThread!=NULL)
					delete m_pLocateDlgThread;
				m_pLocateDlgThread=NULL;
			}
		}
		else
		{
			delete m_pLocateDlgThread;
			m_pLocateDlgThread=NULL;
		}

		CloseHandle(hLocateThread);
		DebugCloseThread(hLocateThread);
	}
	LeaveCriticalSection(&m_csLocateThread);
			

	((CLocateApp*)GetApp())->StopUpdating();
	
	
	SaveSchedules();

	if (m_hAppIcon!=NULL)
	{
		DebugCloseGdiObject(m_hAppIcon);
		DeleteObject(m_hAppIcon);
		m_hAppIcon=NULL;
	}

	::SendNotifyMessage(HWND_BROADCAST,CLocateApp::m_nLocateAppMessage,
		LOCATEMSG_INSTANCEEXITED,GetLocateApp()->m_nInstance);
	
	
	CFrameWnd::OnDestroy();
	
	DebugMessage("void CTrayIconWnd::OnDestroy() END");

}

LRESULT CTrayIconWnd::WindowProc(UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch (msg)
	{
	case WM_SYSTEMTRAY:
		return OnSystemTrayMessage((UINT)wParam,(UINT)lParam);
	case WM_GETICON:
	case WM_SETICON:
		DefWindowProc(*this,msg,wParam,lParam);
		break;
	case WM_QUERYENDSESSION:
		DebugFormatMessage("WM_QUERYENDSESSION, wParam:%X lParam:%X",wParam,lParam);
		return TRUE;	
	case WM_ENDSESSION:
		DebugFormatMessage("WM_ENDSESSION, wParam:%X lParam:%X",wParam,lParam);
		OnDestroy();
		return 0;
	case WM_GETLOCATEDLG:
		if (wParam==2)
			return (BOOL)this;
        if (m_pLocateDlgThread==NULL)
			return NULL;
		if (wParam==1)
			return (BOOL)m_pLocateDlgThread->m_pLocate;
		if (m_pLocateDlgThread->m_pLocate==NULL)
			return NULL;
		return (BOOL)(HWND)*m_pLocateDlgThread->m_pLocate;
	case WM_FREEUPDATESTATUSPOINTERS:
		if (m_pUpdateStatusWnd!=NULL)
		{
			delete m_pUpdateStatusWnd;
			m_pUpdateStatusWnd=NULL;
		}
		break;
	case WM_HOTKEY:
		if (int(wParam)>=0 && int(wParam)<m_aShortcuts.GetSize())
		{
			if (!m_aShortcuts[int(wParam)]->IsWhenAndWhereSatisfied(*this))
				break;

			if (m_aShortcuts[int(wParam)]->m_nDelay>0 && 
				m_aShortcuts[int(wParam)]->m_nDelay!=-1)
				SetTimer(ID_SHORTCUTACTIONTIMER+int(wParam),m_aShortcuts[int(wParam)]->m_nDelay);
			else if (m_aShortcuts[int(wParam)]->m_nDelay==-1 && lParam!=LPARAM(-1))
				PostMessage(WM_EXECUTESHORTCUT,wParam,LPARAM(-1));
			else           			
				m_aShortcuts[int(wParam)]->ExecuteAction();
        }
		break;
	case WM_EXECUTESHORTCUT:
		if (int(wParam)>=0 && int(wParam)<m_aShortcuts.GetSize())
		{
			if (m_aShortcuts[int(wParam)]->m_nDelay>0 && 
				m_aShortcuts[int(wParam)]->m_nDelay!=-1)
				SetTimer(ID_SHORTCUTACTIONTIMER+int(wParam),m_aShortcuts[int(wParam)]->m_nDelay);
			else if (m_aShortcuts[int(wParam)]->m_nDelay==-1 && lParam!=LPARAM(-1))
				PostMessage(WM_EXECUTESHORTCUT,wParam,LPARAM(-1));
			else           			
				m_aShortcuts[int(wParam)]->ExecuteAction();
		}
		break;
	case WM_OPENDIALOG:
		OnLocate();
		break;
	case WM_RESETSHORTCUTS:
		TurnOnShortcuts();
		break;
	default:
		if (msg==CLocateApp::m_nHFCInstallationMessage)
		{
			if (lParam!=NULL)
			{	
				char szAppLine[257];
				GlobalGetAtomName((ATOM)lParam,szAppLine,256);
				if (strcasecmp(GetApp()->GetAppName(),szAppLine)==0)
				{
					if (wParam==1 || wParam==2) // Installing (1) or UnInstalling (2)...
						DestroyWindow();
				}
			}
			return (BOOL)(HWND)*this;
		}
		else if (msg==CLocateApp::m_nTaskbarCreated)
			AddTaskbarIcon();
		else if (msg==CLocateApp::m_nLocateAppMessage)
		{
			switch (LOWORD(wParam))
			{
			case LOCATEMSG_ACTIVATEINSTANCE:
				if (GetLocateApp()->m_nInstance==SHORT(HIWORD(wParam)))
					OnActivateAnotherInstance((ATOM)lParam);
				break;
			case LOCATEMSG_INSTANCEEXITED:
				if (GetLocateApp()->m_nInstance>DWORD(lParam))
				{
					GetLocateApp()->m_nInstance--;

					if (GetLocateApp()->m_nInstance==0)
						SetSchedules();
				}
				break;
			case LOCATEMSG_EXITPROCESSS:
				DestroyWindow();
				break;
			}
		}
		break;
	}
	return CFrameWnd::WindowProc(msg,wParam,lParam);
}

void CTrayIconWnd::AddTaskbarIcon(BOOL bForce)
{
	if (!bForce && CLocateApp::GetProgramFlags()&CLocateApp::pfDontShowSystemTrayIcon)
		return;

	// Creating taskbar icon
	NOTIFYICONDATA nid;
	nid.cbSize=NOTIFYICONDATA_V1_SIZE;
	nid.hWnd=*this;
	nid.uID=1000;
	nid.uFlags=NIF_ICON|NIF_MESSAGE|NIF_TIP;
	nid.uCallbackMessage=WM_SYSTEMTRAY;
	nid.hIcon=m_hAppIcon;
	LoadString(IDS_NOTIFYLOCATE,nid.szTip,63);

	Shell_NotifyIcon(NIM_ADD,&nid);

	if (((CLocateApp*)GetApp())->m_wShellDllVersion>=0x0500)
	{
		nid.cbSize=sizeof(NOTIFYICONDATA);
		nid.uVersion=NOTIFYICON_VERSION;
		Shell_NotifyIcon(NIM_SETVERSION,&nid);
	}
}

void CTrayIconWnd::DeleteTaskbarIcon(BOOL bForce)
{
	if (!bForce && CLocateApp::GetProgramFlags()&CLocateApp::pfDontShowSystemTrayIcon)
		return;

	NOTIFYICONDATA nid;
	nid.cbSize=sizeof(NOTIFYICONDATA);
	nid.hWnd=*this;
	nid.uID=1000;
	nid.uFlags=0;
	Shell_NotifyIcon(NIM_DELETE,&nid);
}


DWORD CTrayIconWnd::OnSystemTrayMessage(UINT uID,UINT msg)
{
	if (GetLocateApp()->m_wShellDllVersion>=0x0500)
	{
		switch (msg)
		{
		case WM_CONTEXTMENU:
			{
				/*POINT pt;
				GetCursorPos(&pt);
				SetForegroundWindow();
				TrackPopupMenu(m_Menu.GetSubMenu(0),
					TPM_RIGHTALIGN|TPM_BOTTOMALIGN|TPM_RIGHTBUTTON,pt.x,pt.y,0,
					*this,NULL);*/
				break;
			}
		case NIN_KEYSELECT:
			// Keyboard space
			OnLocate();
			break;
		case NIN_SELECT:
			// One click
			break;
		case WM_LBUTTONDBLCLK:
			// Doubleclick
			OnLocate();
			break;
		case WM_LBUTTONDOWN:
			if (GetLocateApp()->GetProgramFlags()&CLocateApp::pfTrayIconClickActivate)
				OnLocate();
			break;
		case WM_RBUTTONUP:
			{
				//DebugMessage("OpenPopupMenu");
				POINT pt;
				GetCursorPos(&pt);
				SetForegroundWindow();
				TrackPopupMenu(m_Menu.GetSubMenu(0),
					TPM_RIGHTALIGN|TPM_BOTTOMALIGN|TPM_RIGHTBUTTON,pt.x,pt.y,0,
					*this,NULL);
				//DebugMessage("OpenPopupMenu END");
				break;
			}
			break;
		case NIN_BALLOONSHOW:
			break;
		case NIN_BALLOONHIDE:
			break;
		case NIN_BALLOONTIMEOUT:
			break;
		case NIN_BALLOONUSERCLICK:
			break;
		default:
			break;
		}
	}
	else
	{
		switch (msg)
		{
		case WM_LBUTTONDOWN:
			if (GetLocateApp()->GetProgramFlags()&CLocateApp::pfTrayIconClickActivate)
				OnLocate();
			break;
		case WM_LBUTTONDBLCLK:
			OnLocate();
			break;
		case WM_RBUTTONUP:
			{
				POINT pt;
				GetCursorPos(&pt);
				SetForegroundWindow();
				TrackPopupMenu(m_Menu.GetSubMenu(0),
					TPM_RIGHTALIGN|TPM_BOTTOMALIGN|TPM_RIGHTBUTTON,pt.x,pt.y,0,
					*this,NULL);
				break;
			}
		}
	}
	return TRUE;
}

void CTrayIconWnd::OnTimer(DWORD wTimerID)
{
	//CFrameWnd::OnTimer(wTimerID);
	switch (wTimerID)
	{
	case ID_UPDATEANIM:
		// Checking this abnormal state
		if (GetLocateApp()->m_ppUpdaters==NULL)
		{
			// Stop update animation
			NotifyFinishingUpdating();
			break;
		}
		

		EnterCriticalSection(&m_csAnimBitmaps);
		if (m_pUpdateAnimIcons!=NULL)
		{
			m_nCurUpdateAnimBitmap++;
			if (m_nCurUpdateAnimBitmap>=COUNT_UPDATEANIMATIONS)
				m_nCurUpdateAnimBitmap=0;
			SetTrayIcon(m_pUpdateAnimIcons[m_nCurUpdateAnimBitmap]);
		}
		LeaveCriticalSection(&m_csAnimBitmaps);
		break;
	case ID_SYNCSCHEDULES:
		KillTimer(ID_SYNCSCHEDULES);
		SetTimer(ID_CHECKSCHEDULES,1000,NULL);
	case ID_CHECKSCHEDULES:
		CheckSchedules();
		break;
	case ID_RUNSTARTUPSCHEDULES:
		KillTimer(ID_RUNSTARTUPSCHEDULES);
		if (RunStartupSchedules())
			SetTimer(ID_RUNSTARTUPSCHEDULES,1000);
		break;
	default:
		if (int(wTimerID)>=ID_SHORTCUTACTIONTIMER)
		{
			KillTimer(wTimerID);

			if (int(wTimerID)-ID_SHORTCUTACTIONTIMER<m_aShortcuts.GetSize())
				m_aShortcuts[int(wTimerID)-ID_SHORTCUTACTIONTIMER]->ExecuteAction();

		}
		break;		
	}
}

DWORD CTrayIconWnd::OnActivateAnotherInstance(ATOM aCommandLine)
{
	if (aCommandLine==NULL)
		OnLocate();
	else
	{
		WCHAR szCmdLine[2000];
		if (IsUnicodeSystem())
			GlobalGetAtomNameW(aCommandLine,szCmdLine,2000);
		else
		{
			char szCmdLineA[1000];
			GlobalGetAtomName(aCommandLine,szCmdLineA,2000);
			MultiByteToWideChar(CP_ACP,0,szCmdLineA,-1,szCmdLine,2000);
		}			

		CLocateApp::CStartData* pStartData=new CLocateApp::CStartData;
		CLocateApp::ParseParameters(szCmdLine,pStartData);
		if (pStartData->m_nStartup&CLocateApp::CStartData::startupDoNotOpenDialog &&
			pStartData->m_nStartup&CLocateApp::CStartData::startupUpdate)
		{
			OnUpdate(FALSE);
			delete pStartData;
		}
		else
		{
			BOOL bUpdate=pStartData->m_nStartup&CLocateApp::CStartData::startupUpdate;
			CLocateDlg* pLocateDlg=GetLocateDlg();
			if (pLocateDlg!=NULL)
			{
				ForceForegroundAndFocus();
		
				
				
				// Restore if minimized
				WINDOWPLACEMENT wp;
				wp.length=sizeof(WINDOWPLACEMENT);
				pLocateDlg->GetWindowPlacement(&wp);
				if (wp.showCmd==SW_SHOWMINIMIZED)
					pLocateDlg->ShowWindow(swRestore);
		
				pLocateDlg->BringWindowToTop();
				pLocateDlg->ForceForegroundAndFocus();m_pLocateDlgThread->m_pLocate->ForceForegroundAndFocus();

				pLocateDlg->SendMessage(WM_SETSTARTDATA,0,(LPARAM)pStartData);
				delete pStartData;
			}
			else
			{
				GetLocateApp()->SetStartData(pStartData);
				OnLocate();
			}
			if (bUpdate)
				OnUpdate(FALSE);
		}
	}
	return 0;
}
		
DWORD CTrayIconWnd::SetSchedules(CList<CSchedule*>* pSchedules,BOOL bRunStartupSchedules)
{
	if (GetLocateApp()->m_nInstance!=0)
		return 0;

	
	// Clear existing schedules
	m_Schedules.RemoveAll();

	BOOL bNeedCpuUsage=FALSE;
	DWORD m_dwDelay=0;
	
	//DebugFormatMessage("CTrayIconWnd::SetSchedules(0x%X) START",(DWORD)pSchedules);
	if (pSchedules==NULL)
	{
		CRegKey2 RegKey;
		if (RegKey.OpenKey(HKCU,"\\General",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
		{
			DWORD nKeyLen=RegKey.QueryValueLength("Schedules");
			BYTE* pSchedules=new BYTE[nKeyLen];
			RegKey.QueryValue("Schedules Delay",m_dwDelay);
			RegKey.QueryValue("Schedules",(LPSTR)pSchedules,nKeyLen);
#ifdef _DEBUG
			char* pTmpData=new char[nKeyLen*2+2];
			for (DWORD i=0;i<nKeyLen;i++)
				StringCbPrintf(pTmpData+i*2,3,"%02X",pSchedules[i]);
			//DebugFormatMessage("SCHEDULES(length=%d): %s",nKeyLen,pTmpData);
			delete[] pTmpData;
#endif
			if (pSchedules[1]==1)
			{
				if (nKeyLen>=6 && pSchedules[0]==SCHEDULE_V1_LEN && 
					nKeyLen==6+SCHEDULE_V1_LEN*(*(DWORD*)(pSchedules+2)))
				{
					BYTE* pPtr=pSchedules+6;
					for (DWORD n=0;n<*(DWORD*)(pSchedules+2);n++)
					{
						//DebugFormatMessage("SCHEDULEV1: type=%d",((CSchedule*)pPtr)->m_nType);
						m_Schedules.AddTail(new CSchedule(pPtr,1));
					}
				}
			}	
			else if (pSchedules[1]==2)
			{
				if (nKeyLen>=6 && pSchedules[0]==SCHEDULE_V2_LEN)
				{
					BYTE* pPtr=pSchedules+6;
					for (DWORD n=0;n<*(DWORD*)(pSchedules+2);n++)
					{
						if (pPtr+SCHEDULE_V2_LEN>=pSchedules+nKeyLen)
							break;

						//DebugFormatMessage("SCHEDULEV2: type=%d",((CSchedule*)pPtr)->m_nType);
						m_Schedules.AddTail(new CSchedule(pPtr,2));
					}
				}
			}
			else if (pSchedules[1]==3 || pSchedules[1]==4)
			{
				if (nKeyLen>=6 && pSchedules[0]==SCHEDULE_V34_LEN)
				{
					BYTE* pPtr=pSchedules+6;
					for (DWORD n=0;n<*(DWORD*)(pSchedules+2);n++)
					{
						if (pPtr+SCHEDULE_V34_LEN>=pSchedules+nKeyLen)
							break;

						//DebugFormatMessage("SCHEDULEV3: type=%d",((CSchedule*)pPtr)->m_nType);
						CSchedule* pSchedule=new CSchedule(pPtr,pSchedules[1]);
						m_Schedules.AddTail(pSchedule);

						if (pSchedule->m_bFlags&CSchedule::flagEnabled &&
							pSchedule->m_wCpuUsageTheshold!=WORD(-1))
							bNeedCpuUsage=TRUE;
					}
				}
			}
			delete[] pSchedules;
		}	
	}
	else
	{
		
		m_Schedules.Swap(*pSchedules);
		POSITION pPos=m_Schedules.GetHeadPosition();
		while (pPos!=NULL)
		{
			CSchedule* pSchedule=m_Schedules.GetAt(pPos);
			//DebugFormatMessage("SCHEDULE: type=%d",pSchedule->m_nType);
			if (pSchedule->m_bFlags&CSchedule::flagEnabled &&
				pSchedule->m_wCpuUsageTheshold!=WORD(-1))
				bNeedCpuUsage=TRUE;
			pPos=m_Schedules.GetNextPosition(pPos);
		}
	}

	if (bNeedCpuUsage)
	{
		if (m_pCpuUsage==NULL)
		{
			m_pCpuUsage=new CCpuUsage;
			m_pCpuUsage->GetCpuUsage();
		}
		else
			bNeedCpuUsage=FALSE;
	}
	else if (m_pCpuUsage!=NULL)
	{
		delete m_pCpuUsage;
		m_pCpuUsage=NULL;
	}

	SYSTEMTIME st;
	GetLocalTime(&st);


	SetTimer(ID_SYNCSCHEDULES,m_dwDelay*1000+(bNeedCpuUsage?2000:1000)-st.wMilliseconds,NULL);
	//DebugMessage("CTrayIconWnd::SetSchedules END");

	if (bRunStartupSchedules)
		SetTimer(ID_RUNSTARTUPSCHEDULES,m_dwDelay*1000+500,NULL);

	return m_Schedules.GetCount();
}

BOOL CTrayIconWnd::SaveSchedules()
{
	if (GetLocateApp()->m_nInstance!=0)
		return 0;

	//DebugMessage("CTrayIconWnd::SaveSchedules() START");
	
	CRegKey2 RegKey;
	if (RegKey.OpenKey(HKCU,"\\General",CRegKey::createNew|CRegKey::samAll)==ERROR_SUCCESS)
	{
		DWORD dwDataLen=6;
		POSITION pPos=m_Schedules.GetHeadPosition();
		while (pPos!=NULL)
		{
			dwDataLen+=m_Schedules.GetAt(pPos)->GetDataLen();
			pPos=m_Schedules.GetNextPosition(pPos);
		}		
		
		//DebugFormatMessage("dwDataLen=%d",dwDataLen);

		BYTE* pSchedules=new BYTE[dwDataLen];
		if (pSchedules==NULL)
		{
			SetHFCError(HFC_CANNOTALLOC);
			//DebugMessage("LocateAppWnd::OnDestroy(): Cannot allocate memory.");
		}
		pSchedules[0]=SCHEDULE_V34_LEN;
		pSchedules[1]=4; //version
		*(DWORD*)(pSchedules+2)=m_Schedules.GetCount();
		
		
		BYTE* pPtr=pSchedules+6;
		pPos=m_Schedules.GetHeadPosition();
		while (pPos!=NULL)
		{
			//DebugFormatMessage("SCHEDULE: type %d",m_Schedules.GetAt(pPos)->m_nType);
			pPtr+=m_Schedules.GetAt(pPos)->GetData(pPtr);
			pPos=m_Schedules.GetNextPosition(pPos);
		}
		//DebugMessage("Saveing schedules into registry");
		RegKey.SetValue("Schedules",(LPCSTR)pSchedules,dwDataLen,REG_BINARY);
		
#ifdef _DEBUG
		DWORD dwTmpDataLen=sizeof(CSchedule)*m_Schedules.GetCount()+6;
		char* pTmpData=new char[dwTmpDataLen*2+2];
		for (DWORD i=0;i<dwTmpDataLen;i++)
			StringCbPrintf(pTmpData+i*2,3,"%02X",pSchedules[i]);
		//DebugFormatMessage("SCHEDULES(length=%d): %s",dwTmpDataLen,pTmpData);
		delete[] pTmpData;
#endif
		delete[] pSchedules;
	}
	
	//DebugMessage("CTrayIconWnd::SaveSchedules() END");
	return TRUE;
}
	
void CTrayIconWnd::CheckSchedules()
{
	if (GetLocateApp()->IsUpdating())
		return;
	
	CSchedule::STIME tCurTime;
	CSchedule::SDATE tCurDate;
	
	DWORD nNext=(DWORD)-1;
	UINT nDayOfWeek;
	CSchedule::GetCurrentDateAndTime(&tCurDate,&tCurTime,&nDayOfWeek);
	BOOL bSchedulesChanged=FALSE;	
	
	POSITION pPos=m_Schedules.GetHeadPosition();
	while (pPos!=NULL)
	{
		CSchedule* pSchedule=m_Schedules.GetAt(pPos);
		if (pSchedule!=NULL)
		{
			DWORD nTemp=pSchedule->WhenShouldRun(tCurTime,tCurDate,nDayOfWeek);
			if (nTemp<500)
			{
				BOOL bRun=TRUE;
				if (pSchedule->m_wCpuUsageTheshold!=WORD(-1))
				{
					ASSERT(m_pCpuUsage!=NULL);

					if (m_pCpuUsage->GetCpuUsage()>pSchedule->m_wCpuUsageTheshold)
						bRun=FALSE;
				}
				if (GetLocateApp()->IsUpdating())
					bRun=FALSE;

				if (bRun)
				{
					bSchedulesChanged=TRUE;
				
					sMemCopy(&pSchedule->m_tLastStartDate,&tCurDate,sizeof(CSchedule::SDATE));
					sMemCopy(&pSchedule->m_tLastStartTime,&tCurTime,sizeof(CSchedule::STIME));
					
					pSchedule->m_bFlags|=CSchedule::flagRunned;
									
					OnUpdate(FALSE,pSchedule->m_pDatabases,pSchedule->m_nThreadPriority);
					
					if (pSchedule->m_bFlags&CSchedule::flagDeleteAfterRun)
					{
						POSITION pTmp=m_Schedules.GetNextPosition(pPos);
						delete pSchedule;
						m_Schedules.RemoveAt(pPos);
						pPos=pTmp;
						continue;
					}
				}
			}
		}
		pPos=m_Schedules.GetNextPosition(pPos);
	}
	if (bSchedulesChanged)
		SaveSchedules();
}

BOOL CTrayIconWnd::RunStartupSchedules()
{
	CSchedule::STIME tCurTime;
	CSchedule::SDATE tCurDate;
	
	DWORD nNext=(DWORD)-1;
	UINT nDayOfWeek;
	CSchedule::GetCurrentDateAndTime(&tCurDate,&tCurTime,&nDayOfWeek);
	BOOL bSchedulesChanged=FALSE;	
	BOOL bShouldBeCalledAgain=FALSE;

	POSITION pPos=m_Schedules.GetHeadPosition();
	while (pPos!=NULL)
	{
		CSchedule* pSchedule=m_Schedules.GetAt(pPos);
		if (pSchedule!=NULL)
		{
			if (pSchedule->m_nType==CSchedule::typeAtStartup && 
				pSchedule->m_bFlags&CSchedule::flagEnabled)
			{
				BOOL bRun=TRUE;
				if (pSchedule->m_wCpuUsageTheshold!=WORD(-1))
				{
					ASSERT(m_pCpuUsage!=NULL);

					if (m_pCpuUsage->GetCpuUsage()>pSchedule->m_wCpuUsageTheshold)
						bRun=FALSE;
				}
				if (GetLocateApp()->IsUpdating())
					bRun=FALSE;

				if (bRun)
				{

					bSchedulesChanged=TRUE;
					
					sMemCopy(&pSchedule->m_tLastStartDate,&tCurDate,sizeof(CSchedule::SDATE));
					sMemCopy(&pSchedule->m_tLastStartTime,&tCurTime,sizeof(CSchedule::STIME));
					
					pSchedule->m_bFlags|=CSchedule::flagRunned;
									
					OnUpdate(FALSE,pSchedule->m_pDatabases,pSchedule->m_nThreadPriority);
					
					if (pSchedule->m_bFlags&CSchedule::flagDeleteAfterRun)
					{
						POSITION pTmp=m_Schedules.GetNextPosition(pPos);
						delete pSchedule;
						m_Schedules.RemoveAt(pPos);
						pPos=pTmp;
						continue;
					}
				}
				else
					bShouldBeCalledAgain=TRUE;
			}
		}
		pPos=m_Schedules.GetNextPosition(pPos);
	}

	if (bSchedulesChanged)
		SaveSchedules();

	return bShouldBeCalledAgain;
}

void CTrayIconWnd::OnHelp(LPHELPINFO lphi)
{
	if (GetLocateDlg()!=NULL)
	{
		GetLocateDlg()->OnHelp(lphi);
		return;
	}

	CTargetWnd::OnHelp(lphi);

	if (lphi->iContextType==HELPINFO_MENUITEM)
	{
		// Menu item
		if (HtmlHelp(HH_HELP_CONTEXT,lphi->iCtrlId)==NULL)
		{
			// No topic found, show topics window
			HtmlHelp(HH_DISPLAY_TOPIC,0);
		}
	}
}

void CTrayIconWnd::NotifyFinishingUpdating()
{
	// No updaters running anymore...
				
	// ...so stopping animations
	if (this!=NULL)
	{
		if (m_pUpdateStatusWnd!=NULL)
			m_pUpdateStatusWnd->Update();
		StopUpdateStatusNotification();
	}

	CLocateDlg* pLocateDlg=::GetLocateDlg();
	if (pLocateDlg!=NULL)
	{
		pLocateDlg->StopUpdateAnimation();
		
		
		// ... and constucting notification message:
		// checking wheter all are stopped, or cancelled 
		EnterCriticalSection(&GetLocateApp()->m_cUpdatersPointersInUse);
		
		if (GetLocateApp()->m_ppUpdaters!=NULL)
		{
			int iThreads;
			
			// Count threads and check which are interrupted
			BOOL bAllStopped=TRUE;
			for (iThreads=0;GetLocateApp()->m_ppUpdaters[iThreads]!=NULL;iThreads++)
			{
				if (GET_UPDATER_CODE(GetLocateApp()->m_ppUpdaters[iThreads])!=ueStopped)
					bAllStopped=FALSE;
			}

			if (bAllStopped)
			{
				// All updaters are interrupted by user
				pLocateDlg->SendMessage(WM_SETOPERATIONSTATUSBAR,(WPARAM)IDI_EXCLAMATION,
					(LPARAM)(LPCWSTR)ID2W(IDS_UPDATINGCANCELLED));
			}
			else
			{
				// All succeeded or some updaters failed/interrupted
				CStringW str2;
				int added=0;
					
				for (int i=0;i<iThreads;i++)
				{
					switch (GET_UPDATER_CODE(GetLocateApp()->m_ppUpdaters[i]))
					{
					case ueSuccess:
						break;
					case ueStopped:
						if (added>0)
							str2 << L", ";
						if (iThreads>1)
						{
							str2 << ID2W(IDS_UPDATINGTHREAD);
							str2 << ' ' << (int)(i+1) << L": ";
						}

						str2 << ID2W(IDS_UPDATINGCANCELLED2);
						added++;
						break;
					case ueFolderUnavailable:
						if (added>0)
							str2 << L", ";
						if (iThreads>1)
						{
							str2 << ID2W(IDS_UPDATINGTHREAD);
							str2 << ' ' << (int)(i+1) << L": ";
						}
						str2 << ID2W(IDS_UPDATINGUNAVAILABLEROOT);
						added++;
						break;
					default:
						if (added>0)
							str2 << L", ";
						if (iThreads>1)
						{
							str2 << ID2W(IDS_UPDATINGTHREAD);
							str2 << ' ' << (int)(i+1) << L": ";
						}
						str2 << ID2W(IDS_UPDATINGFAILED);
						added++;
						break;
					}
				}

				if (str2.IsEmpty())
				{
					pLocateDlg->SendMessage(WM_SETOPERATIONSTATUSBAR,0,
						(LPARAM)(LPCWSTR)ID2W(IDS_UPDATINGSUCCESS));				
				}
				else
				{
					CStringW str(IDS_UPDATINGENDED);
					str << L' ' << str2;
					pLocateDlg->SendMessage(WM_SETOPERATIONSTATUSBAR,
						(WPARAM)IDI_EXCLAMATION,(LPARAM)(LPCWSTR)str);				
				}
			}
		}
		else
		{
			pLocateDlg->SendMessage(WM_SETOPERATIONSTATUSBAR,0,
				(LPARAM)(LPCWSTR)ID2W(IDS_UPDATINGENDED));				
		}

		LeaveCriticalSection(&GetLocateApp()->m_cUpdatersPointersInUse);
	}

	
}






/////////////////////////////////////////////
// CTrayIconWnd::CUpdateStatusWnd


CTrayIconWnd::CUpdateStatusWnd::CUpdateStatusWnd()
:	CFrameWnd(),m_WindowSize(0,0),m_pMouseMove(NULL)
{
	
	RegisterWndClass("LOCATEAPPUPDATESTATUS",CS_HREDRAW|CS_VREDRAW,LoadCursor(NULL,IDC_ARROW),
		(HBRUSH)(COLOR_INFOBK+1),NULL);

	m_cTextColor=GetSysColor(COLOR_INFOTEXT);
	m_cTitleColor=GetSysColor(COLOR_INFOTEXT);
	m_cErrorColor=GetSysColor(COLOR_INFOTEXT);
	m_cBackColor=GetSysColor(COLOR_INFOBK);

	// Update status tooltip
	CRegKey2 RegKey;
	if (RegKey.OpenKey(HKCU,"\\Dialogs\\Updatestatus",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
	{
		DWORD dwTemp;
		if (RegKey.QueryValue("TextColor",dwTemp))
			m_cTextColor=dwTemp;
		if (RegKey.QueryValue("TitleColor",dwTemp))
			m_cTitleColor=dwTemp;
		if (RegKey.QueryValue("ErrorColor",dwTemp))
			m_cErrorColor=dwTemp;
		if (RegKey.QueryValue("BackColor",dwTemp))
			m_cBackColor=dwTemp;
	}

	InitializeCriticalSection(&m_cUpdate);
}

CTrayIconWnd::CUpdateStatusWnd::~CUpdateStatusWnd()
{
	EnterCriticalSection(&m_cUpdate);
	LeaveCriticalSection(&m_cUpdate);

	DeleteCriticalSection(&m_cUpdate);

	
	m_aErrors.RemoveAll();

	m_Font.DeleteObject();
	m_TitleFont.DeleteObject();

	
}
	
int CTrayIconWnd::CUpdateStatusWnd::OnCreate(LPCREATESTRUCT lpcs)
{
	SetTimer(ID_UPDATESTATUS,500,NULL);
	SetTimer(ID_CHECKFOREGROUNDWND,100,NULL);
	return CFrameWnd::OnCreate(lpcs);
}



void CTrayIconWnd::CUpdateStatusWnd::OnDestroy()
{
	CTargetWnd::OnDestroy();
	if (m_pMouseMove!=NULL)
	{
		delete m_pMouseMove;
		m_pMouseMove=NULL;
		ReleaseCapture();			
	}

	SavePosition(HKCU,CRegKey2::GetCommonKey()+"\\Dialogs\\Updatestatus","WindowPos");

}



void CTrayIconWnd::CUpdateStatusWnd::OnNcDestroy()
{
	CTargetWnd::OnNcDestroy();

	// If this is NULL, class is deleted anyway
	if (GetTrayIconWnd()->m_pUpdateStatusWnd!=NULL)
		GetTrayIconWnd()->PostMessage(WM_FREEUPDATESTATUSPOINTERS);
	else
		delete this;
}


void CTrayIconWnd::CUpdateStatusWnd::OnTimer(DWORD wTimerID)
{
	switch (wTimerID)
	{
	case ID_UPDATESTATUS:
		Update();
		break;
	case ID_IDLEEXIT:
		KillTimer(ID_IDLEEXIT);
		CWnd::DestroyWindow();
		break;
	case ID_CHECKFOREGROUNDWND:
		CheckForegroundWindow();
		break;
	}
}

void CTrayIconWnd::CUpdateStatusWnd::OnPaint()
{
	CPaintDC dc(this);

	dc.SetMapMode(MM_TEXT);
	dc.SetBkMode(TRANSPARENT);
	dc.SelectObject(m_TitleFont);

	EnterCriticalSection(&m_cUpdate);
	
	RECT rcClient,rc2;
	GetClientRect(&rcClient);
	rcClient.left+=EXTRA_MARGINSX;
	//rcClient.top+=EXTRA_MARGINSY;

	
	// Draw stop button

	rc2.left=rcClient.right-2*UPDATETIPBUTTON_CX-UPDATETIPBUTTON_SPACEX-1;
	rc2.right=rcClient.right-UPDATETIPBUTTON_CX-UPDATETIPBUTTON_SPACEX-1;
	rc2.top=rcClient.top+1;
	rc2.bottom=rcClient.top+UPDATETIPBUTTON_CY+1;
	dc.DrawFrameControl(&rc2,DFC_CAPTION,DFCS_CAPTIONMIN|DFCS_TRANSPARENT|DFCS_FLAT);
	dc.SelectStockObject(GRAY_BRUSH);
	dc.SelectStockObject(NULL_PEN);
	rc2.left+=2;	rc2.top+=2;	rc2.right-=1;	rc2.bottom-=1;
	dc.Rectangle(&rc2);

	
	// Draw close button
	rc2.left=rcClient.right-UPDATETIPBUTTON_CX-1;
	rc2.right=rcClient.right-1;
	rc2.top=rcClient.top+1;
	rc2.bottom=rcClient.top+UPDATETIPBUTTON_CY+1;
	dc.DrawFrameControl(&rc2,DFC_CAPTION,DFCS_CAPTIONCLOSE/*|DFCS_TRANSPARENT*/|DFCS_FLAT);
	

    // Drawing title
	LPCWSTR pPtr=m_sStatusText;
	int nLength=(int)FirstCharIndex(pPtr,L'\n');
	rc2=rcClient;
	dc.SetTextColor(m_cTitleColor);
	dc.DrawText(pPtr,nLength,&rc2,DT_SINGLELINE|DT_CALCRECT);
	dc.DrawText(pPtr,nLength,&rc2,DT_SINGLELINE);
	rcClient.top+=rc2.bottom-rc2.top+EXTRA_LINES;

	dc.SelectObject(m_Font);
	
	// Drawing texts
	dc.SetTextColor(m_cTextColor);
	while (nLength!=-1)
	{
		pPtr+=nLength+1;
		nLength=(int)FirstCharIndex(pPtr,L'\n');

		rc2=rcClient;
		dc.DrawText(pPtr,nLength,&rc2,DT_SINGLELINE|DT_CALCRECT);
		dc.DrawText(pPtr,nLength,&rc2,DT_SINGLELINE);
	
		rcClient.top+=rc2.bottom-rc2.top+EXTRA_LINES;
	}

	// Drawing errors
	dc.SetTextColor(m_cErrorColor);
	for (int i=0;i<m_aErrors.GetSize();i++)
	{
		rc2=rcClient;	
		dc.DrawText(m_aErrors[i],-1,&rc2,DT_SINGLELINE|DT_CALCRECT);
		dc.DrawText(m_aErrors[i],-1,&rc2,DT_SINGLELINE);
	
		rcClient.top+=rc2.bottom-rc2.top+EXTRA_LINES;
	}

	LeaveCriticalSection(&m_cUpdate);
}


void CTrayIconWnd::CUpdateStatusWnd::FormatErrorForStatusTooltip(UpdateError ueError,CDatabaseUpdater* pUpdater)
{
	if (ueError==ueStopped)
		return;

	EnterCriticalSection(&m_cUpdate);

	// Now, change pointer to null, if someone is accesing pointer, it may have enough time to read
	WCHAR error[300];
	int nLabelLength=LoadString(IDS_LASTERROR,error,200);

	LPCWSTR szExtra=NULL;

	switch(ueError)
	{
	case ueUnknown:
		LoadString(IDS_LASTERRORUNKNOWN,error+nLabelLength,300-nLabelLength);
		szExtra=pUpdater->GetCurrentDatabaseName();
		break;
	case ueCreate:
	case ueOpen:
		LoadString(IDS_LASTERRORCANNOTOPEN,error+nLabelLength,300-nLabelLength);
		szExtra=pUpdater->GetCurrentDatabaseFile();
		break;
	case ueRead:
		LoadString(IDS_LASTERRORCANNOTREAD,error+nLabelLength,300-nLabelLength);
		szExtra=pUpdater->GetCurrentDatabaseFile();
		break;
	case ueWrite:
		LoadString(IDS_LASTERRORCANNOTWRITE,error+nLabelLength,300-nLabelLength);
		szExtra=pUpdater->GetCurrentDatabaseFile();
		break;
	case ueAlloc:
		LoadString(IDS_LASTERRORCANNOTALLOCATE,error+nLabelLength,300-nLabelLength);
		break;
	case ueInvalidDatabase:
		LoadString(IDS_LASTERRORINVALIDDB,error+nLabelLength,300-nLabelLength);
		szExtra=pUpdater->GetCurrentDatabaseFile();
		break;
	case ueFolderUnavailable:
		LoadString(IDS_LASTERRORROOTUNAVAILABLE,error+nLabelLength,300-nLabelLength);
		szExtra=pUpdater->GetCurrentRootPath();
		break;
	case ueCannotIncrement:
		LoadString(IDS_LASTERRORCANNOTINCREMENTDB,error+nLabelLength,300-nLabelLength);
		szExtra=pUpdater->GetCurrentDatabaseName();
		break;
	default:
		StringCbPrintfW(error+nLabelLength,(300-nLabelLength)*sizeof(WCHAR),L"%d",(int)ueError);
		break;
	}

	LPWSTR szNewPtr;
	int nLength;
	if (szExtra!=NULL)
	{
		int iLen=(int)istrlenw(error)+(int)istrlenw(szExtra)+1;
		szNewPtr=new WCHAR[iLen];
		StringCbPrintfW(szNewPtr,iLen*sizeof(WCHAR),error,szExtra);
		nLength=istrlenw(szNewPtr);
	}
	else
	    szNewPtr=alloccopy(error,nLength=istrlenw(error));

	m_aErrors.Add(szNewPtr);

	// Make status window bigger
	{
		CDC dc(this);
		dc.SetMapMode(MM_TEXT);
		dc.SelectObject(m_Font);
		
		CRect rc(0,0,0,0);
		dc.DrawText(szNewPtr,-1,&rc,DT_SINGLELINE|DT_CALCRECT);
		if (m_WindowSize.cx<rc.Width())
			m_WindowSize.cx=rc.Width();
		m_WindowSize.cy+=rc.Height()+EXTRA_MARGINSY;

		//CSize sz=dc.GetTextExtent(szNewPtr,(int)nLength);
		//if (m_WindowSize.cx<sz.cx)
		//	m_WindowSize.cx=sz.cx;
		//m_WindowSize.cy+=sz.cy+EXTRA_MARGINSY;
	}
	
	LeaveCriticalSection(&m_cUpdate);

	SetPosition();
}


void CTrayIconWnd::CUpdateStatusWnd::FormatStatusTextLine(CStringW& str,const CTrayIconWnd::RootInfo& pRootInfo,int nThreadID,int nThreads)
{
	// #X  thread number
	if (nThreadID!=-1)
		str << L'#' << (int)nThreadID;
 
	switch (pRootInfo.usStatus)
	{
	case statusInitializing:
		// Initializing
		if (nThreadID!=-1)
			str << L": ";
		str.AddString(IDS_NOTIFYINITIALIZING);
		break;
	case statusFinishing:
		// Finishing
		if (nThreadID!=-1)
			str << L": ";
		str.AddString(IDS_NOTIFYFINISHING);
		break;
	case statusFinished:
		// Finished
		if (nThreadID!=-1)
			str << L": ";

		switch (pRootInfo.ueError)
		{
		case ueStopped:
			str.AddString(IDS_UPDATINGCANCELLED);
			break;
		case ueFolderUnavailable:
		case ueSuccess:
			str.AddString(IDS_NOTIFYDONE);
			break;
		default:
			str.AddString(IDS_UPDATINGFAILED2);
			break;
		}
		break;
	default:
		{
			CStringW tmp;

			if (pRootInfo.dwNumberOfDatabases>1)
			{
				if (nThreadID!=-1)
					str << L' ';
				str << (int)(pRootInfo.dwCurrentDatabase+1) << L'/' << (int)pRootInfo.dwNumberOfDatabases << L": ";
			}
			else if (nThreadID!=-1)
				str << L": ";
			
			tmp.Format(IDS_UPDATINGUPDATING,pRootInfo.pName);
			str << tmp;
			
			switch (pRootInfo.usStatus)
			{
			case statusInitializeWriting:
			case statusWritingDB:
				str << L": ";
				str.AddString(IDS_NOTIFYWRITINGDATABASE);
				break;
			case statusCustom1: // delaying
				str << L": ";
				str.AddString(IDS_UPDATINGDELAYWRITING);
				break;
			case statusScanning:
				if (pRootInfo.wProgressState!=WORD(-1))
				{
					str << L' ' << (int)((int)(pRootInfo.wProgressState)/10) << L'%';
				}

				str << L": ";
				tmp.Format(IDS_UPDATINGSCANNINGROOT,pRootInfo.pRoot);
				str << tmp;
				break;
			}
			break;
		}
	}
}

void CTrayIconWnd::CUpdateStatusWnd::FillFontStructs(LOGFONT* pTextFont,LOGFONT* pTitleFont)
{
	int nDeviceCaps;
	{
		// Getting device caps
		HDC hScreenDC=::GetDC(NULL);
		nDeviceCaps=::GetDeviceCaps(hScreenDC,LOGPIXELSY);
		::ReleaseDC(NULL,hScreenDC);
	}

	if (pTextFont!=NULL)
	{
		ZeroMemory(pTextFont,sizeof(LOGFONT));
		pTextFont->lfHeight=-MulDiv(8, nDeviceCaps, 72);
		pTextFont->lfWeight=FW_NORMAL;
		//pTextFont->lfCharSet=ANSI_CHARSET;
		//pTextFont->lfOutPrecision=OUT_DEFAULT_PRECIS;
		//pTextFont->lfClipPrecision=CLIP_DEFAULT_PRECIS;
		//pTextFont->lfQuality=DEFAULT_QUALITY;
		//pTextFont->lfPitchAndFamily=DEFAULT_PITCH|FF_DONTCARE;
		StringCbCopy(pTextFont->lfFaceName,LF_FACESIZE,"Tahoma");
	}

	if (pTitleFont!=NULL)
	{
		ZeroMemory(pTitleFont,sizeof(LOGFONT));
		pTitleFont->lfHeight=-MulDiv(10, nDeviceCaps, 72);
		pTitleFont->lfWeight=FW_BOLD;
		//pTitleFont->lfCharSet=ANSI_CHARSET;
		//pTitleFont->lfOutPrecision=OUT_DEFAULT_PRECIS;
		//pTitleFont->lfClipPrecision=CLIP_DEFAULT_PRECIS;
		//pTitleFont->lfQuality=DEFAULT_QUALITY;
		//pTitleFont->lfPitchAndFamily=DEFAULT_PITCH|FF_DONTCARE;
		StringCbCopy(pTitleFont->lfFaceName,LF_FACESIZE,"Tahoma");
	}
}


void CTrayIconWnd::CUpdateStatusWnd::SetFonts()
{
	if (m_TitleFont.m_hObject!=NULL && m_Font.m_hObject!=NULL)
		return;

	// Update status tooltip
	CRegKey2 RegKey;
	LOGFONT lTitleFont,lTextFont;
	if (RegKey.OpenKey(HKCU,"\\Dialogs\\Updatestatus",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
	{
		if (RegKey.QueryValue("TextFont",(LPSTR)&lTextFont,sizeof(LOGFONT))<sizeof(LOGFONT))
			CTrayIconWnd::CUpdateStatusWnd::FillFontStructs(&lTextFont,NULL);
	
		if (RegKey.QueryValue("TitleFont",(LPSTR)&lTitleFont,sizeof(LOGFONT))<sizeof(LOGFONT))
			CTrayIconWnd::CUpdateStatusWnd::FillFontStructs(NULL,&lTitleFont);
	}
	else
		CTrayIconWnd::CUpdateStatusWnd::FillFontStructs(&lTextFont,&lTitleFont);

	
	if (m_TitleFont.m_hObject==NULL)
	{
		if (!m_TitleFont.CreateFontIndirect(&lTitleFont))
		{
			lTitleFont.lfFaceName[0]='\0';
			if (!m_TitleFont.CreateFontIndirect(&lTitleFont))
				m_TitleFont.CreateStockObject(DEFAULT_GUI_FONT);
		}
		
	}
	if (m_Font.m_hObject==NULL)
	{
		if (!m_Font.CreateFontIndirect(&lTextFont))
		{
			lTextFont.lfFaceName[0]='\0';
			if (!m_Font.CreateFontIndirect(&lTextFont))
				m_Font.CreateStockObject(DEFAULT_GUI_FONT);
		}
		
	}
}

LRESULT CTrayIconWnd::CUpdateStatusWnd::WindowProc(UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch (msg)
	{
	case WM_ERASEBKGND:
		{
			HBRUSH hBrush=CreateSolidBrush(m_cBackColor);
			DebugOpenGdiObject(hBrush);
			if (hBrush!=NULL)
			{
				RECT rc;
				GetClientRect(&rc);
                FillRect((HDC)wParam,&rc,hBrush);
				DebugCloseGdiObject(hBrush);
				DeleteObject(hBrush);
				return TRUE;
			}
			break;
		}
	case WM_LBUTTONDOWN:
		if (m_pMouseMove==NULL)
		{
			SHORT x=SHORT(LOWORD(lParam)),y=SHORT(HIWORD(lParam));
			
			// First check if "close button" area clicked
			RECT rcClient;
			GetClientRect(&rcClient);
			if (x>=rcClient.right-2*UPDATETIPBUTTON_CX-UPDATETIPBUTTON_SPACEX-1 && 
				x<=rcClient.right-UPDATETIPBUTTON_CX-UPDATETIPBUTTON_SPACEX-1 && y<=UPDATETIPBUTTON_CY+1)
			{
				GetTrayIconWnd()->PostMessage(WM_COMMAND,MAKEWPARAM(IDM_STOPUPDATING,0),NULL);
				return FALSE;
			}
			else if (x>=rcClient.right-UPDATETIPBUTTON_CX-1 && y<=UPDATETIPBUTTON_CY+1)
			{
				DestroyWindow();
				return FALSE;
			}

			m_pMouseMove=new MouseMove;
			m_pMouseMove->nStartPointX=x;
			m_pMouseMove->nStartPointY=y;

			ASSERT(m_pMouseMove->nStartPointX>=0 && m_pMouseMove->nStartPointX<1600);
			ASSERT(m_pMouseMove->nStartPointY>=0 && m_pMouseMove->nStartPointY<1600);


			SetCapture();
		}
		break;
	case WM_LBUTTONUP:
		if (m_pMouseMove!=NULL)
		{
			delete m_pMouseMove;
			m_pMouseMove=NULL;
			ReleaseCapture();			
		}
		break;
	case WM_RBUTTONDOWN:
		{
			POINT pt;
			GetCursorPos(&pt);
			SetForegroundWindow();
			TrackPopupMenu(GetTrayIconWnd()->m_Menu.GetSubMenu(0),
				TPM_RIGHTALIGN|TPM_BOTTOMALIGN|TPM_RIGHTBUTTON,pt.x,pt.y,0,
				*GetTrayIconWnd(),NULL);
			//DebugMessage("OpenPopupMenu END");
		}
	}
	return CTargetWnd::WindowProc(msg,wParam,lParam);
}

void CTrayIconWnd::CUpdateStatusWnd::OnMouseMove(UINT fwKeys,WORD xPos,WORD yPos)
{
	if (m_pMouseMove!=NULL)
	{
		RECT rcWindowRect;
		GetWindowRect(&rcWindowRect);

		LONG nNewCoordX=rcWindowRect.left+(SHORT(xPos)-m_pMouseMove->nStartPointX);
		LONG nNewCoordY=rcWindowRect.top+(SHORT(yPos)-m_pMouseMove->nStartPointY);
		

		SetWindowPos(NULL,nNewCoordX,nNewCoordY,0,0,SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);
	}
}
	
void CTrayIconWnd::CUpdateStatusWnd::SetPosition()
{	
	CRect rcDesktopRect,rcTrayRect;
	CPoint ptUpperLeft;
	CSize szSize;
	BOOL bAdjustDesktopRect=FALSE; // If TRUE, remove tray are from rcDesktopRect

	HWND hShellTrayWnd=FindWindow("Shell_TrayWnd",NULL); // Whole tray window
	HWND hDesktop=FindWindow("Progman","Program Manager");
	
	if (hDesktop==NULL)
		hDesktop=GetDesktopWindow();
	
	
	// Bound monitor arrea
	HMONITOR (WINAPI *pMonitorFromWindow)(HWND,DWORD);
	pMonitorFromWindow=(HMONITOR (WINAPI *)(HWND,DWORD))GetProcAddress(GetModuleHandle("user32.dll"),"MonitorFromWindow");
	if (pMonitorFromWindow!=NULL)
	{
		HMONITOR hCurrentMonitor=MonitorFromWindow(hShellTrayWnd!=NULL?hShellTrayWnd:*this,MONITOR_DEFAULTTOPRIMARY);
		MONITORINFO mi;
		mi.cbSize = sizeof(mi);
		GetMonitorInfo(hCurrentMonitor, &mi);
		
		rcDesktopRect=mi.rcWork;
	}
	else
	{
		// Get screen size
		::GetWindowRect(hDesktop,&rcDesktopRect);
		bAdjustDesktopRect=TRUE;
	}

	// Compute center point
	POINT ptDesktopCenter={(rcDesktopRect.left+rcDesktopRect.right)/2,(rcDesktopRect.top+rcDesktopRect.bottom)/2};
	

	// Computing width and height
	if (m_WindowSize.cx==0 && m_WindowSize.cy==0)
	{
		SetFonts();
	
		CDC dc(this);
		dc.SetMapMode(MM_TEXT);

		// Checking how much space title will take
		CStringW str(IDS_UPDATINGTOOLTIPTITLE);
		dc.SelectObject(m_TitleFont);
		szSize=dc.GetTextExtent(str);	
		
		str.LoadString(IDS_UPDATINGENDED);
		EnlargeSizeForText(dc,str,szSize);
		
        // Changing font
		dc.SelectObject(m_Font);
		
		EnterCriticalSection(&GetLocateApp()->m_cUpdatersPointersInUse);
		if (GetLocateApp()->m_ppUpdaters!=NULL)
		{
			WORD wThreads;
			for (wThreads=0;GetLocateApp()->m_ppUpdaters[wThreads]!=NULL;wThreads++);

            CTrayIconWnd::RootInfo ri;
			for (int i=0;GetLocateApp()->m_ppUpdaters[i]!=NULL;i++)
			{
				szSize.cy+=EXTRA_LINES;

				ri.pName=NULL;
				ri.pRoot=NULL;
				ri.ueError=ueSuccess;
				ri.wProgressState=1000;
				
				// Cheking how much space "initializing" will take
				str.Empty();
				ri.usStatus=statusInitializing;
				FormatStatusTextLine(str,ri,wThreads>1?i+1:-1,wThreads);
				CSize szThisLine=dc.GetTextExtent(str);
				
				
				// Cheking how much space "finishing" will take
				str.Empty();
				ri.usStatus=statusFinishing;
				FormatStatusTextLine(str,ri,wThreads>1?i+1:-1,wThreads);
				EnlargeSizeForText(dc,str,szThisLine);
				
				// Cheking how much space "finished" will take
				str.Empty();
				ri.usStatus=statusFinished;
				FormatStatusTextLine(str,ri,wThreads>1?i+1:-1,wThreads);
				EnlargeSizeForText(dc,str,szThisLine);
				
				
					
				
				LPWSTR szFile=NULL;
				CDatabase::ArchiveType nArchiveType;
				CDatabaseUpdater::CRootDirectory* pRoot;
				ri.dwNumberOfDatabases=GetLocateApp()->m_ppUpdaters[i]->GetNumberOfDatabases();
				for (ri.dwCurrentDatabase=0;GetLocateApp()->m_ppUpdaters[i]->EnumDatabases(ri.dwCurrentDatabase,ri.pName,szFile,nArchiveType,pRoot);ri.dwCurrentDatabase++)
				{
					ri.pRoot=NULL;	
					
					// Checking how much space "delay writing" will take
					str.Empty();
					ri.usStatus=statusCustom1;
					FormatStatusTextLine(str,ri,wThreads>1?i+1:-1,wThreads);
					EnlargeSizeForText(dc,str,szThisLine);
				
					// Checking how much space "writing database" will take
					str.Empty();
					ri.usStatus=statusWritingDB;
					FormatStatusTextLine(str,ri,wThreads>1?i+1:-1,wThreads);
					EnlargeSizeForText(dc,str,szThisLine);
				

					ri.usStatus=statusScanning;
					
					while (pRoot!=NULL)
					{
						// Checking how much space "scanning root" will take
						ri.pRoot=pRoot->m_PathInDatabase.GetBuffer();
						str.Empty();
						FormatStatusTextLine(str,ri,wThreads>1?i+1:-1,wThreads);
						EnlargeSizeForText(dc,str,szThisLine);
						pRoot=pRoot->m_pNext;
					}
					
				}

				szSize.cy+=szThisLine.cy;
				if (szSize.cx<szThisLine.cx)
					szSize.cx=szThisLine.cx;

			}
		}    
		LeaveCriticalSection(&GetLocateApp()->m_cUpdatersPointersInUse);
		
		szSize.cx+=2*EXTRA_MARGINSX;
		szSize.cy+=2*EXTRA_MARGINSY;

	}
	else
		szSize=m_WindowSize;

	// This is postion to where tooltip is placed
	DWORD nPosition=CLocateApp::pfUpdateTooltipPositionDownRight;
	
	// Checking where tray window is
	if (hShellTrayWnd!=NULL)
	{
		::GetWindowRect(hShellTrayWnd,&rcTrayRect);
	
		// Resolvinf position near clock and removing tray are from rcDekstopRect
		if (rcTrayRect.top>ptDesktopCenter.y)
		{
			// Tray is on bottom
			nPosition=CLocateApp::pfUpdateTooltipPositionDownRight;
			
			if (bAdjustDesktopRect)
				rcDesktopRect.bottom=rcTrayRect.top;
		}
		else if (rcTrayRect.bottom<ptDesktopCenter.y)
		{
			// Tray is on top
			nPosition=CLocateApp::pfUpdateTooltipPositionUpRight;
			if (bAdjustDesktopRect)
				rcDesktopRect.top=rcTrayRect.bottom;
		}
		else if (rcTrayRect.right<ptDesktopCenter.x)
		{
			// Tray is on left
			nPosition=CLocateApp::pfUpdateTooltipPositionDownLeft;
			if (bAdjustDesktopRect)
				rcDesktopRect.left=rcTrayRect.right;
		}
		else
		{
			// Tray is on right
			nPosition=CLocateApp::pfUpdateTooltipPositionDownRight;
			if (bAdjustDesktopRect)
				rcDesktopRect.right=rcTrayRect.left;
		}
	}

	switch (GetLocateApp()->GetProgramFlags()&CLocateApp::pfUpdateTooltipPositionMask)
	{
	case CLocateApp::pfUpdateTooltipPositionDefault:
		break;
	case CLocateApp::pfUpdateTooltipPositionLastPosition:
		if (LoadPosition(HKCU,CRegKey2::GetCommonKey()+"\\Dialogs\\Updatestatus","WindowPos",fgOnlyNormalPosition))
			return;
		break;
	default:
		nPosition=GetLocateApp()->GetProgramFlags()&CLocateApp::pfUpdateTooltipPositionMask;
		break;
	}

	if ((nPosition&CLocateApp::pfUpdateTooltipPositionDown)==CLocateApp::pfUpdateTooltipPositionDown)
		ptUpperLeft.y=rcDesktopRect.bottom-szSize.cy-2;
	else
		ptUpperLeft.y=rcDesktopRect.top+2;
	
	if ((nPosition&CLocateApp::pfUpdateTooltipPositionRight)==CLocateApp::pfUpdateTooltipPositionRight)
		ptUpperLeft.x=rcDesktopRect.right-szSize.cx-2;
	else
		ptUpperLeft.x=rcDesktopRect.left+2;

		
	CLocateDlg* pLocateDlg=::GetLocateDlg();
	
	if (pLocateDlg!=NULL)
	{
		SetWindowPos(HWND_TOP,ptUpperLeft.x,ptUpperLeft.y,szSize.cx,szSize.cy,
			GetForegroundWindow()==*pLocateDlg?SWP_NOACTIVATE:(SWP_NOACTIVATE|SWP_NOZORDER));
	}
	else 
		SetWindowPos(NULL,ptUpperLeft.x,ptUpperLeft.y,szSize.cx,szSize.cy,SWP_NOZORDER|SWP_NOACTIVATE);


	m_WindowSize=szSize;
}

void CTrayIconWnd::CUpdateStatusWnd::Update() 
{
	WORD wThreads,wRunning;
	RootInfo* pRootInfos;
	if (GetTrayIconWnd()->GetRootInfos(wThreads,wRunning,pRootInfos))
	{
		Update(wThreads,wRunning,pRootInfos);
		CTrayIconWnd::FreeRootInfos(wThreads,pRootInfos);
	}
	else
	{
		Update(0,0,NULL);
		IdleClose();
	}
}

void CTrayIconWnd::CUpdateStatusWnd::CheckForegroundWindow()
{		
	// Check window status
    if ((CLocateApp::GetProgramFlags()&CLocateApp::pfUpdateTooltipTopmostMask)==
		CLocateApp::pfUpdateTooltipNoTopmostWhenForegroundWndMaximized ||
		(CLocateApp::GetProgramFlags()&CLocateApp::pfUpdateTooltipTopmostMask)==
		CLocateApp::pfUpdateTooltipNoTopmostWhdnForegroundWndInFullScreen)
	{
		BOOL bTopMost=FALSE;
		HWND hForegroundWindow=GetForegroundWindow();

		if (hForegroundWindow==NULL)
			bTopMost=TRUE;
		else
		{
			LONG dwStyle=::GetWindowLong(hForegroundWindow,GWL_STYLE);
			if (dwStyle&WS_MAXIMIZE)
			{
				if ((CLocateApp::GetProgramFlags()&CLocateApp::pfUpdateTooltipTopmostMask)==
					CLocateApp::pfUpdateTooltipNoTopmostWhdnForegroundWndInFullScreen &&
					dwStyle&WS_CAPTION)
					bTopMost=TRUE;
			}
			else
				bTopMost=TRUE;
      	}

		if (GetExStyle()&WS_EX_TOPMOST)
		{
			if (!bTopMost)
			{
                SetWindowPos(HWND_NOTOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
				::SetWindowPos(hForegroundWindow,*this,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
			}
		}
		else if (bTopMost)
			SetWindowPos(HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
		
	}
}

void CTrayIconWnd::CUpdateStatusWnd::Update(WORD wThreads,WORD wRunning,RootInfo* pRootInfos)
{
	EnterCriticalSection(&m_cUpdate);
	
	
	m_sStatusText.Empty();

	// Forming title
	if (wRunning>0)
		m_sStatusText.LoadString(IDS_UPDATINGTOOLTIPTITLE);
	else
		m_sStatusText.LoadString(IDS_UPDATINGENDED);

	if (pRootInfos!=NULL)
	{
		for (WORD i=0;i<wThreads;i++)
		{
			m_sStatusText << L'\n';
			FormatStatusTextLine(m_sStatusText,pRootInfos[i],wThreads>1?i+1:-1,wThreads);
		}
	}
	else
	{
		m_sStatusText << L'\n';
		LPWSTR pError=CLocateApp::FormatLastOsError();
		if (pError!=NULL)
		{
			CStringW str;
			str.Format(IDS_ERRORUNKNOWNOS,pError);
			while (str.LastChar()=='\n' || str.LastChar()=='\r')
			str.DelLastChar();
			m_sStatusText << str;
		}
		else
			m_sStatusText.AddString(IDS_ERRORUNKNOWN);
	}
		
		

	LeaveCriticalSection(&m_cUpdate);

	InvalidateRect(NULL,TRUE);
}


void CTrayIconWnd::CUpdateStatusWnd::IdleClose()
{
	KillTimer(ID_UPDATESTATUS);
	SetTimer(ID_IDLEEXIT,4000,NULL);
}

BOOL CTrayIconWnd::CUpdateStatusWnd::DestroyWindow()
{
	KillTimer(ID_IDLEEXIT);
	KillTimer(ID_CHECKFOREGROUNDWND);
	GetTrayIconWnd()->m_pUpdateStatusWnd=NULL;
	CWnd::DestroyWindow();
	return TRUE;
}
