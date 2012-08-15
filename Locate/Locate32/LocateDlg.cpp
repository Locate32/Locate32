/* Locate32 - Copyright (c) 1997-2010 Janne Huttunen */

#include <HFCLib.h>
#include "Locate32.h"
#include <uxtheme.h>
#include <vssym32.h>







////////////////////////////////////////////////////////////
// CLocateDlgThread
////////////////////////////////////////////////////////////


BOOL CLocateDlgThread::InitInstance()
{
	DebugFormatMessage("CLocateDlgThread::InitInstance(), thread is 0x%X",GetCurrentThreadId());

	CWinThread::InitInstance();
	
	// Initialize OLE/COM
	CoInitializeEx(NULL,COINIT_APARTMENTTHREADED);
	
	// Size & Date tab uses "in house" date time picker
	RegisterDataTimeExCltr();

	// Creating main dialog
	m_pMainWnd=m_pLocate=new CLocateDlg;
	m_pLocate->Create(NULL);
	
	// Settings transparency
	//m_pLocate->SetDialogTransparency();
	

	// Register dialog for this thread
	RegisterDialog(*m_pLocate);

	return TRUE;
}

int CLocateDlgThread::ExitInstance()
{
	// Free locate dialog class
	delete m_pLocate;
	m_pLocate=NULL;
	
	// Set global locate dialog handle to NULL
	GetTrayIconWnd()->SetLocateDlgThreadToNull();
	
	
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
	
	return CWinThread::ExitInstance();
}

BOOL CLocateDlgThread::OnThreadMessage(MSG* pMsg)
{
	if (m_pLocate->m_pListTooltips==NULL)
		return FALSE;

	if (pMsg->hwnd==*m_pLocate->m_pListTooltips)
	{
		switch (pMsg->message)
		{
		case WM_MOUSEMOVE:
			// If this is given to work, tooltip will be shown and 
			// hidden sequentally when mouse is over tooltip
			if (m_pLocate->m_bTooltipActive)
				return TRUE;
            return FALSE;
		case WM_TIMER:
			// If 3rd timer is given to work, tooltip will be shown and 
			// hidden sequentally when mouse is over tooltip
			if (m_pLocate->m_bTooltipActive && pMsg->wParam==3)
				return TRUE;
			break;
		}
		return FALSE;
	}
	if (pMsg->hwnd==*m_pLocate->m_pListCtrl)
	{
		if (pMsg->message!=WM_MOUSEMOVE || 
			pMsg->wParam&(MK_LBUTTON|MK_MBUTTON|MK_RBUTTON|MK_XBUTTON1|MK_XBUTTON2))
			return FALSE;

			
		// Resolving item below cursor, if any
		LVHITTESTINFO ht;
		ht.pt.x=LOWORD(pMsg->lParam);
		ht.pt.y=HIWORD(pMsg->lParam);
		int iRet=m_pLocate->m_pListCtrl->SubItemHitTest(&ht);

		if (iRet!=-1)
		{
			if (m_pLocate->m_iTooltipItem!=ht.iItem || 
				m_pLocate->m_iTooltipSubItem!=ht.iSubItem)
			{
				// First, let control handle messages
				m_pLocate->m_pListTooltips->RelayEvent(pMsg);

				// Deleting previous tools
				m_pLocate->DeleteTooltipTools();

				if (DetailType(m_pLocate->m_pListCtrl->GetColumnIDFromSubItem(ht.iSubItem))==Name)
				{
					TOOLINFOW tii;
					tii.cbSize = TTTOOLINFOW_V2_SIZE;
					
					tii.uFlags = TTF_IDISHWND;
					tii.hwnd   = *m_pLocate;
					tii.uId    = (UINT)pMsg->hwnd;
					tii.hinst  = NULL;
					tii.lpszText  = LPSTR_TEXTCALLBACKW;
					tii.lParam = 0;
					
					m_pLocate->m_pListTooltips->AddTool(&tii);
					m_pLocate->m_pListTooltips->SetMaxTipWidth(400);

					m_pLocate->m_iTooltipItem=ht.iItem;
					m_pLocate->m_iTooltipSubItem=ht.iSubItem;
				}
				else
				{
					CRect rc,rc2;
					m_pLocate->m_pListCtrl->GetSubItemRect(ht.iItem,ht.iSubItem,LVIR_LABEL,&rc);
					m_pLocate->m_pListCtrl->GetSubItemRect(ht.iItem,ht.iSubItem,LVIR_ICON,&rc2);


					
					CStringW sText;
					m_pLocate->m_pListCtrl->GetItemText(sText,ht.iItem,ht.iSubItem);
					int nWidth=m_pLocate->m_pListCtrl->GetStringWidth(sText)+12;
					
					// InFolder need also space for icon
					if (DetailType(m_pLocate->m_pListCtrl->GetColumnIDFromSubItem(ht.iSubItem))==InFolder)
						nWidth+=rc2.Width()+5;

					if (nWidth>rc.Width())
					{
						TOOLINFOW tii;
						tii.cbSize = TTTOOLINFOW_V2_SIZE;
		
						tii.uFlags = TTF_IDISHWND;
						tii.hwnd   = *m_pLocate;
						tii.uId    = (UINT)pMsg->hwnd;
						tii.hinst  = NULL;
						tii.lpszText  = LPSTR_TEXTCALLBACKW;
						tii.lParam = 0;

						m_pLocate->GetClientRect(&tii.rect);
						m_pLocate->m_pListTooltips->AddTool(&tii);
				
                        m_pLocate->m_iTooltipItem=ht.iItem;
						m_pLocate->m_iTooltipSubItem=ht.iSubItem;

						
					}
					else
					{
						m_pLocate->m_iTooltipItem=-1;
						m_pLocate->m_iTooltipSubItem=-1;
					}


				}
			}
			else
			{
				if (!m_pLocate->m_bTooltipActive) 
					m_pLocate->m_pListTooltips->RelayEvent(pMsg);
				return FALSE;
			}
		}
		else
		{
			if (m_pLocate->m_iTooltipItem!=-1)
				m_pLocate->DeleteTooltipTools();

			m_pLocate->m_pListTooltips->RelayEvent(pMsg);			
		}
	}
	return FALSE;
}


////////////////////////////////////////////////////////////
// CLocateDlg - Initialization / deinitialization
////////////////////////////////////////////////////////////

inline CLocateDlg::CLocateDlg()
:	CDialog(IDD_MAIN),m_dwFlags(fgDefault),m_dwExtraFlags(efDefault),
	m_nSorting(BYTE(-1)),m_dwInstantFlags(isDefault),
	m_dwInstantLimit(DEFAULT_INSTANTSEARCHLIMIT),
	m_dwInstantDelay(DEFAULT_INSTANTSEARCHDELAY),
	m_dwInstantChars(DEFAULT_INSTANTSEARCHCHARS),
	m_nMaxYMinimized(0),m_nMaxYMaximized(0),m_nLargeY(354),
	m_ClickWait(FALSE),m_hSendToListFont(NULL),
	m_pListCtrl(NULL),m_pTabCtrl(NULL),m_pStatusCtrl(NULL),m_pListTooltips(NULL),
	m_pLocater(NULL),m_pBackgroundUpdater(NULL),m_pActiveContextMenu(NULL),
	m_pLocateAnimBitmaps(NULL),m_pUpdateAnimBitmaps(NULL),
	m_pFileNotificationsThread(NULL),m_dwMaxFoundFiles(0),m_pDropTarget(NULL),
	m_pImageHandler(NULL),m_iTooltipItem(-1),m_iTooltipSubItem(-1),m_bTooltipActive(FALSE),
	m_hLastFocus(NULL),m_WaitEvery30(15),m_WaitEvery60(30),m_hDialogFont(NULL),
	m_hLargeDialogIcon(NULL),m_hSmallDialogIcon(NULL),
	m_nCurrentListType(ltDetails),m_dwThumbnailFlags(tfDefault),m_pSystemImageList(NULL),
	m_pStrCmp(NULL)
{
	ZeroMemory(m_aResultListActions,TypeCount*ListActionCount*sizeof(void*));

	if (GetSystemFeaturesFlag()&efWinVista)
		m_dwThumbnailFlags|=tfVistaFeaturesAvailable;
	
	if (FAILED(SHGetDesktopFolder(&m_pDesktopFolder)))
		m_pDesktopFolder=NULL;

	InitializeCriticalSection(&m_csUpdateAnimBitmaps);
	InitializeCriticalSection(&m_csLocateAnimBitmaps);


	// If "Show results list always shown", enable large mode  now
	CRegKey2 RegKey;
	if (RegKey.OpenKey(HKCU,"\\General",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
	{
		DWORD temp=0;
		RegKey.QueryValue("Program Status",temp);
		if (temp&fgDialogLargeModeOnly)
			m_dwFlags|=fgLargeMode;
	}
}


CLocateDlg::~CLocateDlg()
{
	DebugFormatMessage("CLocateDlg::~CLocateDlg() this is %X",DWORD(this));

	ASSERT(m_pActiveContextMenu==NULL);

	if (m_pDesktopFolder!=NULL)
		m_pDesktopFolder->Release();

	DeleteCriticalSection(&m_csUpdateAnimBitmaps);
	DeleteCriticalSection(&m_csLocateAnimBitmaps);
}


void CLocateDlg::SetStartData(const CLocateApp::CStartData* pStartData)
{	
	ISDLGTHREADOK

	DWORD dwChanged=0;

	// Loading preset first
	if (pStartData->m_pLoadPreset!=NULL)
		LoadPreset(pStartData->m_pLoadPreset);

    // Set fields	
	m_NameDlg.SetStartData(pStartData,dwChanged);
	m_SizeDateDlg.SetStartData(pStartData,dwChanged);
	m_AdvancedDlg.SetStartData(pStartData,dwChanged);

	// Set view mode
	if (pStartData->m_nListMode!=BYTE(-1))
		SetListType((CLocateDlg::ListType)pStartData->m_nListMode);

	// Set sorting
	if (pStartData->m_nSorting!=BYTE(-1))
	{
		if (m_dwFlags&fgLargeMode && 
			(pStartData->m_nStatus&CLocateApp::CStartData::statusRunAtStartUp)==0)
		{
			if (m_nSorting!=pStartData->m_nSorting)
			{
				SortItems(DetailType(pStartData->m_nSorting&127),pStartData->m_nSorting&128);
				m_nSorting=pStartData->m_nSorting;
			}
		}
		else
			SetSorting(pStartData->m_nSorting);
	}

	if (pStartData->m_dwMaxFoundFiles!=DWORD(-1))
		m_dwMaxFoundFiles=pStartData->m_dwMaxFoundFiles;
	
	
	// Set focus
	switch (pStartData->m_nActivateControl)
	{
	case CLocateApp::CStartData::Named:
		SetTimer((UINT)m_NameDlg.GetDlgItem(IDC_NAME),100);
		break;
	case CLocateApp::CStartData::Type:
		SetTimer((UINT)m_NameDlg.GetDlgItem(IDC_TYPE),100);
		break;
	case CLocateApp::CStartData::LookIn:
		SetTimer((UINT)m_NameDlg.GetDlgItem(IDC_LOOKIN),100);
		break;
	case CLocateApp::CStartData::Results:
		SetTimer((UINT)(HWND)*m_pListCtrl,100);
		break;
	}

	m_NameDlg.HilightTab(m_NameDlg.IsChanged());
	m_SizeDateDlg.HilightTab(m_SizeDateDlg.IsChanged());
	m_AdvancedDlg.HilightTab(m_AdvancedDlg.IsChanged());

	if (pStartData->m_nStatus&CLocateApp::CStartData::statusRunAtStartUp)
		PostMessage(WM_COMMAND,IDC_OK);
	else if (dwChanged)
		OnFieldChange(dwChanged|isByCommandLine);

}

void CLocateDlg::SaveRegistry()
{
	CRegKey2 RegKey;
	
	if(RegKey.OpenKey(HKCU,"\\General",CRegKey::defWrite)==ERROR_SUCCESS)
	{
		CRect rect;
		//CMenu menu(GetSubMenu(GetMenu(),2));
		CMenu menu(GetMenu());
		RegKey.SetValue("Program Status",m_dwFlags&fgSave);
		RegKey.SetValue("Program StatusExtra",m_dwExtraFlags&efSave);
		RegKey.SetValue("Instant Searching",m_dwInstantFlags&isSave);
		RegKey.SetValue("Thumbnail Flags",m_dwThumbnailFlags&tfSave);
		
		SavePosition(RegKey,NULL,"Position");
		
		//GetWindowRect(&rect);
		WINDOWPLACEMENT wp;
		wp.length=sizeof(WINDOWPLACEMENT);
		GetWindowPlacement(&wp);
		
		if (m_dwFlags&fgLargeMode)
			m_nLargeY=wp.rcNormalPosition.bottom-wp.rcNormalPosition.top;;

		RegKey.SetValue("LargeCY",(LPCTSTR)&m_nLargeY,4,REG_DWORD);
		
		RegKey.SetValue("ListType",(DWORD)m_nCurrentListType);
		RegKey.SetValue("AutoArrange",(m_pListCtrl->GetStyle()&LVS_AUTOARRANGE)?1L:0L);
		RegKey.SetValue("AlignToGrid",(m_pListCtrl->GetExtendedListViewStyle()&LVS_EX_SNAPTOGRID)?1L:0L);
	
	}

}

void CLocateDlg::LoadRegistry()
{
	CMenu menu(GetMenu());
	CRegKey2 RegKey;
	DWORD temp;
	DWORD x=100,y=100,cx=438;

	if (RegKey.OpenKey(HKCU,"\\Locate",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
	{
		RegKey.QueryValue("MaximumFoundFiles",(LPTSTR)&m_dwMaxFoundFiles,sizeof(DWORD));
		RegKey.QueryValue("Instant Search Limit",(LPTSTR)&m_dwInstantLimit,sizeof(DWORD));
		RegKey.QueryValue("Instant Search Delay",(LPTSTR)&m_dwInstantDelay,sizeof(DWORD));
		RegKey.QueryValue("Instant Search Chars",(LPTSTR)&m_dwInstantChars,sizeof(DWORD));
		RegKey.CloseKey();
	}

	if (RegKey.OpenKey(HKCU,"\\General",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
	{
		// Loading dwFlags
		temp=m_dwFlags;
		RegKey.QueryValue("Program Status",temp);
		m_dwFlags&=~fgSave;
		m_dwFlags|=temp&fgSave;
		
		temp=m_dwExtraFlags;
		RegKey.QueryValue("Program StatusExtra",temp);
		m_dwExtraFlags&=~efSave;
		m_dwExtraFlags|=temp&efSave;
		if ((m_dwExtraFlags&efItemUpdatingMask)==efDisableItemUpdating)
			m_dwExtraFlags|=efLVNoUpdateWhileSorting;

		temp=m_dwInstantFlags;
		RegKey.QueryValue("Instant Searching",temp);
		m_dwInstantFlags&=~isSave;
		m_dwInstantFlags|=temp&isSave;
		
		temp=m_dwThumbnailFlags;
		RegKey.QueryValue("Thumbnail Flags",temp);
		m_dwThumbnailFlags&=~tfSave;
		m_dwThumbnailFlags|=temp&tfSave;	

		DWORD dwOldFlags=m_dwFlags;
		m_dwFlags|=fgLargeMode;
		LoadPosition(RegKey,NULL,"Position",fgAllowMaximized|fgOnlyNormalPosition);
		m_dwFlags=dwOldFlags;
		
		if (!LoadPosition(RegKey,NULL,"Position",fgAllowMaximized|fgOnlySpecifiedPosition))
		{
			// Could not load previous position, we will cause OnSize to be runned
			WINDOWPLACEMENT wp;
			wp.length=sizeof(wp);
			GetWindowPlacement(&wp);
			wp.rcNormalPosition.right+=10;
			wp.showCmd=SW_SHOW;
			SetWindowPlacement(&wp);
		} 
		
		RegKey.QueryValue("LargeCY",m_nLargeY);
		
		
		temp=0;
		RegKey.QueryValue("AutoArrange",temp);
		if (temp)
			m_pListCtrl->SetStyle(m_pListCtrl->GetStyle()|LVS_AUTOARRANGE);
		temp=0;
		RegKey.QueryValue("AlignToGrid",temp);
		if (temp)
			m_pListCtrl->SetExtendedListViewStyle(LVS_EX_SNAPTOGRID,LVS_EX_SNAPTOGRID);
		
		temp=ltDetails;
		RegKey.QueryValue("ListType",temp);
		m_nCurrentListType=(ListType)temp;
	}
	else
	{	
		// Could not load previous position, we will cause OnSize to be runned
		WINDOWPLACEMENT wp;
		wp.length=sizeof(wp);
		GetWindowPlacement(&wp);
		wp.rcNormalPosition.right+=10;
		wp.rcNormalPosition.bottom=wp.rcNormalPosition.top+m_nMaxYMinimized;
		wp.showCmd=SW_SHOW;
		SetWindowPlacement(&wp);
	}

	
	if (RegKey.OpenKey(HKCU,"\\Locate",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
	{
		DWORD dwTemp;
		if (RegKey.QueryValue("WaitEvery30",dwTemp))
			m_WaitEvery30=WORD(dwTemp);
		if (RegKey.QueryValue("WaitEvery60",dwTemp))
			m_WaitEvery60=WORD(dwTemp);
	}

}

BOOL CLocateDlg::UpdateSettings()
{
	CRegKey2 RegKey;
	DWORD temp=3;
	
	if (RegKey.OpenKey(HKCU,"\\General",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
	{
		//Dialog flags
		temp=m_dwFlags;
		RegKey.QueryValue("Program Status",temp);
        m_dwFlags&=~fgSave;
		m_dwFlags|=temp&fgSave;

		//Extra flags
		temp=m_dwExtraFlags;
		RegKey.QueryValue("Program StatusExtra",temp);
        m_dwExtraFlags&=~efSave;
		m_dwExtraFlags|=temp&efSave;

		if ((m_dwExtraFlags&efItemUpdatingMask)==efDisableItemUpdating)
			m_dwExtraFlags|=efLVNoUpdateWhileSorting;


		//Instant searching
		temp=m_dwInstantFlags;
		RegKey.QueryValue("Instant Searching",temp);
		m_dwInstantFlags&=~isSave;
		m_dwInstantFlags|=temp&isSave;


		
		// Thumbnails
		temp=m_dwThumbnailFlags;
		RegKey.QueryValue("Thumbnail Flags",temp);

		BOOL bSetListTypeAgain=FALSE;

		switch (m_nCurrentListType)
		{
		case ltMediumIcons:
			if ((m_dwThumbnailFlags&tfSave) != (temp&tfSave))
				bSetListTypeAgain=TRUE;
			break;
		case ltLargeIcons:
			{
				CRegKey2 RegKey;
				DWORD dwIconSize;
				if (RegKey.OpenKey(HKCU,"\\General",CRegKey::defRead)==ERROR_SUCCESS)
				{				
					if (RegKey.QueryValue("Thumbnail Large Icon Size",dwIconSize))
					{
						if (dwIconSize!=m_sCurrentIconSize.cx)
							bSetListTypeAgain=TRUE;
					}
				}
				break;
			}
		case ltExtraLargeIcons:
			{
				CRegKey2 RegKey;
				DWORD dwIconSize;
				if (RegKey.OpenKey(HKCU,"\\General",CRegKey::defRead)==ERROR_SUCCESS)
				{				
					if (RegKey.QueryValue("Thumbnail Extra Large Icon Size",dwIconSize))
					{
						if (dwIconSize!=m_sCurrentIconSize.cx)
							bSetListTypeAgain=TRUE;
					}
				}
				break;
			}
		}

		m_dwThumbnailFlags&=~tfSave;
		m_dwThumbnailFlags|=temp&tfSave;

		if (bSetListTypeAgain)
			SetListType(m_nCurrentListType,TRUE);

	}


	m_pListCtrl->RedrawItems(0,m_pListCtrl->GetItemCount());
	SetListSelStyle();


	// Update tooltip setting	
	InitTooltips();


	// Set result list font
	SetResultListFont();

	// Set Transparency
	SetDialogTransparency();

	// Set icon
	LoadDialogIcon();

	// Set compare function
	SetListCompareFunction();

	// Do not use selected databases anymore
	RemoveExtraFlags(efUseLastSelectedDatabases);



	if (m_NameDlg.EnableMultiDirectorySupport(GetFlags()&fgNameMultibleDirectories?TRUE:FALSE))
	{
		// This trick causes OnSize call
		WINDOWPLACEMENT wp;
		wp.length=sizeof(wp);
		m_NameDlg.GetWindowPlacement(&wp);
		wp.showCmd=m_pTabCtrl->GetCurSel()==0?SW_SHOWNORMAL:SW_HIDE;
		wp.rcNormalPosition.bottom++;
		m_NameDlg.SetWindowPlacement(&wp);
		wp.rcNormalPosition.bottom--;
		m_NameDlg.SetWindowPlacement(&wp);
	}




	// Insert '*' for Advanced tab if defaults are changed 
	m_AdvancedDlg.HilightTab(m_AdvancedDlg.IsChanged());


	// Init "Look In" combobox
	m_NameDlg.InitDriveBox();


	// If large dialog wanted, change mode
	SetDialogMode(GetFlags()&fgLargeMode);


	// Allow/disallow in place editing in the result list
	if (GetFlags()&fgLVAllowInPlaceRenaming)
		m_pListCtrl->ModifyStyle(0,LVS_EDITLABELS);
	else
		m_pListCtrl->ModifyStyle(LVS_EDITLABELS,0);


	

	// Setting topmost status
	if (GetFlags()&fgDialogTopMost)
	{
		if (!(GetExStyle()&WS_EX_TOPMOST))
			SetWindowPos(HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
	}
	else if (GetExStyle()&WS_EX_TOPMOST)
		SetWindowPos(HWND_NOTOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);




	// Enable/disable file system tracking
	if (GetExtraFlags()&efEnableFSTracking)
	{
		if (m_pFileNotificationsThread==NULL && m_DatabasesUsedInSearch.GetSize()>0)
			StartBackgroundOperations();
	}
	else if (m_pFileNotificationsThread!=NULL)
		m_pFileNotificationsThread->Stop();

	return TRUE;
}


////////////////////////////////////////////////////////////
// CLocateDlg - Dialog, tabs and items
////////////////////////////////////////////////////////////

void CLocateDlg::SetDialogMode(BOOL bLarge,BOOL bForceRepositionIfMaximized)
{
	if (bLarge || GetFlags()&fgDialogLargeModeOnly)
	{
		if (!(m_dwFlags&fgLargeMode) || bForceRepositionIfMaximized)
		{
			m_dwFlags|=fgLargeMode;
			m_pListCtrl->SetStyle(m_pListCtrl->GetStyle()|WS_TABSTOP);
				
			WINDOWPLACEMENT wp;
			wp.length=sizeof(WINDOWPLACEMENT);
			CRect rect;
			GetWindowRect(&rect);
			GetWindowPlacement(&wp);
			
			if (wp.showCmd==SW_MAXIMIZE)
			{
				wp.rcNormalPosition.bottom=wp.rcNormalPosition.top+m_nLargeY;
				SetWindowPlacement(&wp);
				if (m_nMaxYMaximized>0)
					SetWindowPos(HWND_BOTTOM,0,0,rect.Width(),m_nMaxYMaximized,SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
			}
			else
				SetWindowPos(HWND_BOTTOM,0,0,rect.Width(),m_nLargeY,SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
		
			ShowDlgItem(IDC_FILELIST,swShow);
			ShowDlgItem(IDC_STATUS,swShow);
		}
	}
	else
	{
		if (m_dwFlags&fgLargeMode)
		{
			CRect rect;
			m_dwFlags&=~fgLargeMode;
			m_pListCtrl->SetStyle(m_pListCtrl->GetStyle()&~WS_TABSTOP);
			GetWindowRect(&rect);
			m_nLargeY=rect.Height();
			SetWindowPos(HWND_BOTTOM,0,0,rect.Width(),m_nMaxYMinimized,
				SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);

			// Now, terminate available background updater
			RemoveResultsFromList();
			
			FreeBuffers();
			m_pListCtrl->ShowWindow(swHide);
		}
	}
}

void CLocateDlg::EnableItems(BOOL bEnable)
{
	CButton OK(GetDlgItem(IDC_OK));
	CButton Stop(GetDlgItem(IDC_STOP));
	OK.EnableWindow(bEnable);
	OK.SetButtonStyle(bEnable?BS_DEFPUSHBUTTON:BS_PUSHBUTTON);
	Stop.SetButtonStyle(bEnable?BS_PUSHBUTTON:BS_DEFPUSHBUTTON);
	Stop.EnableWindow(!bEnable);
	EnableDlgItem(IDC_PRESETS,bEnable);
	m_NameDlg.EnableItems(bEnable);
	m_SizeDateDlg.EnableItems(bEnable);
	m_AdvancedDlg.EnableItems(bEnable);

	if (bEnable)
	{
		HWND hFocus=GetFocus();
			
		if ((GetFlags()&fgLVActivateFirstResult) && m_pListCtrl->GetItemCount()>0)
		{
			if (hFocus==NULL || hFocus==*m_pListCtrl)
			{
				if (hFocus==NULL)
					PostMessage(WM_SETITEMFOCUS,(WPARAM)(HWND)*m_pListCtrl);
			}
		}
		else
		{
			if (hFocus==NULL)
			{
				// Give focus to the selected tab
				switch (m_pTabCtrl->GetCurSel())
				{
				case 0:
					m_NameDlg.SetFocus();
					break;
				case 1:
					m_SizeDateDlg.SetFocus();
					break;
				case 2:
					m_AdvancedDlg.SetFocus();
					break;
				}
				
				
				if (m_hLastFocus!=NULL)
				{
					PostMessage(WM_SETITEMFOCUS,(WPARAM)::GetParent(m_hLastFocus));
					PostMessage(WM_SETITEMFOCUS,(WPARAM)m_hLastFocus);
					m_hLastFocus=NULL;
				}
			}
		}
	}
}

void CLocateDlg::LoadDialogIcon()
{
	if (m_hLargeDialogIcon!=NULL)
	{
		DebugCloseGdiObject(m_hLargeDialogIcon);
		DeleteObject(m_hLargeDialogIcon);
		m_hLargeDialogIcon=NULL;
	}
	if (m_hSmallDialogIcon!=NULL)
	{
		DebugCloseGdiObject(m_hSmallDialogIcon);
		DeleteObject(m_hSmallDialogIcon);
		m_hSmallDialogIcon=NULL;
	}

	
	CRegKey2 RegKey;
	if (RegKey.OpenKey(HKCU,"\\General",CRegKey::defRead)==ERROR_SUCCESS)
	{
		CStringW CustomDialogIcon;
		if (RegKey.QueryValue(L"CustomDialogIcon",CustomDialogIcon))
		{
			m_hLargeDialogIcon=(HICON)LoadImage(CustomDialogIcon,
				IMAGE_ICON,GetSystemMetrics(SM_CXICON),GetSystemMetrics(SM_CYICON),LR_LOADFROMFILE|LR_CREATEDIBSECTION);
			DebugOpenGdiObject(m_hLargeDialogIcon);
			m_hSmallDialogIcon=(HICON)LoadImage(CustomDialogIcon,
				IMAGE_ICON,GetSystemMetrics(SM_CXSMICON),GetSystemMetrics(SM_CYSMICON),LR_LOADFROMFILE|LR_CREATEDIBSECTION);
			DebugOpenGdiObject(m_hSmallDialogIcon);
		}
	}

	if (m_hLargeDialogIcon==NULL)
	{
		m_hLargeDialogIcon=(HICON)LoadImage(IDI_APPLICATIONICON,IMAGE_ICON,
			GetSystemMetrics(SM_CXICON),GetSystemMetrics(SM_CYICON),LR_DEFAULTCOLOR|LR_CREATEDIBSECTION);
		DebugOpenGdiObject(m_hLargeDialogIcon);	
	}

	if (m_hSmallDialogIcon==NULL)
	{
		m_hSmallDialogIcon=(HICON)LoadImage(IDI_APPLICATIONICON,IMAGE_ICON,
			GetSystemMetrics(SM_CXSMICON),GetSystemMetrics(SM_CYSMICON),LR_DEFAULTCOLOR|LR_CREATEDIBSECTION);
		DebugOpenGdiObject(m_hSmallDialogIcon);	
	}

	SetIcon(m_hLargeDialogIcon,TRUE);
	SetClassLong(gclHIcon,(LONG_PTR)m_hLargeDialogIcon);
	SetIcon(m_hSmallDialogIcon,FALSE);
	SetClassLong(gclHIconSm,(LONG_PTR)m_hSmallDialogIcon);

}

void CLocateDlg::SetControlPositions(UINT nType,int cx, int cy)
{
	CRect rc;
	// Frame line
	::SetWindowPos(GetDlgItem(IDC_FRAME),HWND_BOTTOM,0,0,cx,2,SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
	
	// Buttons
	::SetWindowPos(GetDlgItem(IDC_OK),HWND_BOTTOM,cx-m_nButtonWidth-7,28,0,0,SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER);
	::SetWindowPos(GetDlgItem(IDC_STOP),HWND_BOTTOM,cx-m_nButtonWidth-7,28+m_nButtonSpacing,0,0,SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER);
	::SetWindowPos(GetDlgItem(IDC_NEWSEARCH),HWND_BOTTOM,cx-m_nButtonWidth-7,28+m_nButtonSpacing*2,0,0,SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER);
	::SetWindowPos(GetDlgItem(IDC_PRESETS),HWND_BOTTOM,cx+m_nPresetsButtonOffset-m_nPresetsButtonWidth-8,35+m_nTabbedDialogHeight-int(m_nPresetsButtonHeight),0,0,SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER);
	
	
	
	CRect rect(0,0,10,m_nTabbedDialogHeight);
	m_pTabCtrl->AdjustRect(TRUE,&rect);
	m_pTabCtrl->SetWindowPos(HWND_BOTTOM,0,0,cx-m_nButtonWidth-30,m_nTabbedDialogHeight+m_nTabHeaderHeight,SWP_NOACTIVATE|SWP_NOMOVE);
	
	GetWindowRect(&rect);
	m_pTabCtrl->GetWindowRect(&rc);
	


	m_NameDlg.SetWindowPos(HWND_TOP,0,0,cx-m_nButtonWidth-37,m_nTabbedDialogHeight,SWP_NOACTIVATE|SWP_NOMOVE);
	m_SizeDateDlg.SetWindowPos(HWND_TOP,0,0,cx-m_nButtonWidth-37,m_nTabbedDialogHeight,SWP_NOACTIVATE|SWP_NOMOVE);
	m_AdvancedDlg.SetWindowPos(HWND_TOP,0,0,cx-m_nButtonWidth-37,m_nTabbedDialogHeight,SWP_NOACTIVATE|SWP_NOMOVE);
	
	
	if (m_dwFlags&fgLargeMode)
	{
		int parts[4];
		if (nType&SIZE_MAXIMIZED)
		{
			parts[0]=(cx-44)/2;
			parts[1]=cx-44;
			parts[2]=cx-22;
			parts[3]=cx; // Update icon
		}
		else
		{
			if (((CLocateApp*)GetApp())->m_wComCtrlVersion<0x0600)
			{
				parts[0]=(cx-62)/2;
				parts[1]=cx-62;
				parts[2]=cx-40;
				parts[3]=cx-18;
			}
			else // Using theme
			{
				parts[0]=(cx-72)/2;
				parts[1]=cx-72;
				parts[2]=cx-47;
				parts[3]=cx-25;
			}
		}
		m_pStatusCtrl->SetWindowPos(HWND_BOTTOM,0,cy-20,cx,20,SWP_NOACTIVATE|SWP_NOZORDER);
		RECT rect,rect2;
		m_pStatusCtrl->GetWindowRect(&rect);
		m_pListCtrl->GetWindowRect(&rect2);
		m_pListCtrl->SetWindowPos(HWND_BOTTOM,0,0,cx-5,rect.top-rect2.top,SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
		m_pListCtrl->UpdateWindow();
		m_pStatusCtrl->SetParts(5,parts);
	}
}

void CLocateDlg::SetVisibleWindowInTab()
{
	switch (m_pTabCtrl->GetCurSel())
	{
	case 0:
		if (!m_NameDlg.IsWindowVisible())
		{
			m_SizeDateDlg.ShowWindow(swHide);
			m_AdvancedDlg.ShowWindow(swHide);				
			m_NameDlg.ShowWindow(swShow);
			
			
			m_NameDlg.SetFocus();
		}
		break;
	case 1:
		if (!m_SizeDateDlg.IsWindowVisible())
		{
			m_NameDlg.ShowWindow(swHide);
			m_AdvancedDlg.ShowWindow(swHide);				
			m_SizeDateDlg.ShowWindow(swShow);
			
			m_SizeDateDlg.SetFocus();
		}
		break;
	case 2:
		if (!m_AdvancedDlg.IsWindowVisible())
		{
			m_NameDlg.ShowWindow(swHide);
			m_SizeDateDlg.ShowWindow(swHide);
			m_AdvancedDlg.ShowWindow(swShow);				

			m_AdvancedDlg.SetFocus();
		}
		break;
	}
}
	
void CLocateDlg::HilightTab(int nTab,int nID,BOOL bHilight)
{
	TC_ITEMW ti;
	WCHAR szText[80];
	ti.mask=TCIF_TEXT;
	ti.cchTextMax=80;
	ti.pszText=szText;
		
	if (!m_pTabCtrl->GetItem(nTab,&ti))
		return;
	
	int nLen=istrlenw(szText);
	if (nLen==0)
		nLen=LoadString(nID,szText,80);

	if (bHilight && szText[nLen-1]!=L'*')
	{
		szText[nLen]=L'*';
		szText[++nLen]=L'\0';
		m_pTabCtrl->SetItem(nTab,&ti);
	}
	else if (!bHilight && szText[nLen-1]==L'*')
	{
		szText[nLen-1]=L'\0';
		m_pTabCtrl->SetItem(nTab,&ti);
	}
}

void CLocateDlg::SetDialogTransparency()
{
	BOOL(WINAPI * pSetLayeredWindowAttributes)(HWND,COLORREF,BYTE,DWORD)=(BOOL(WINAPI *)(HWND,COLORREF,BYTE,DWORD))GetProcAddress(GetModuleHandle("user32.dll"),"SetLayeredWindowAttributes");
	if (pSetLayeredWindowAttributes!=NULL)
	{
		CRegKey2 RegKey;
		if (RegKey.OpenKey(HKCU,"\\General",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
		{
			DWORD dwTransparency;
			if (RegKey.QueryValue("Transparency100",dwTransparency))
			{	
				dwTransparency*=255;
				dwTransparency/=100;
			}
			else if (!RegKey.QueryValue("Transparency",dwTransparency))
				dwTransparency=0;
			

			if (dwTransparency>255)
				dwTransparency=255;

			if (dwTransparency==0)
				ModifyStyleEx(WS_EX_LAYERED,0);				
			else
			{
				ModifyStyleEx(0,WS_EX_LAYERED);				
				pSetLayeredWindowAttributes(*this,0,BYTE(255-dwTransparency),LWA_ALPHA);
			}
		}
	}
}


////////////////////////////////////////////////////////////
// CLocateDlg - Tooltip support
////////////////////////////////////////////////////////////

void CLocateDlg::InitTooltips()
{
    if (m_dwFlags&fgLVDontShowTooltips)
	{
		if (m_pListTooltips!=NULL)
		{
			//m_pListCtrl->SetToolTips(NULL);
			m_pListTooltips->DestroyToolTipCtrl();
			delete m_pListTooltips;
			m_pListTooltips=NULL;
		}
	}
	else 
	{
		if (m_pListTooltips==NULL)
		{
			m_pListTooltips=new CToolTipCtrl;
			m_pListTooltips->Create(*this);
			m_pListTooltips->SetFont(m_pListCtrl->GetFont());
			if (IsUnicodeSystem())
				m_pListCtrl->SetUnicodeFormat(TRUE);
			
			m_iTooltipItem=-1;
			m_iTooltipSubItem=-1;
			m_bTooltipActive=FALSE;
		}

		// Set delay times
		CRegKey2 RegKey;
		if (RegKey.OpenKey(HKCU,"\\General",CRegKey::defRead)==ERROR_SUCCESS)
		{
			DWORD dwDelay;
			if (RegKey.QueryValue("TooltipDelayAutopop",dwDelay))
				m_pListTooltips->SetDelayTime(dwDelay,CToolTipCtrl::autoPop);
			else
				m_pListTooltips->SetDelayTime(UINT(-1),CToolTipCtrl::autoPop);

			if (RegKey.QueryValue("TooltipDelayInitial",dwDelay))
			{
				m_pListTooltips->SetDelayTime(dwDelay,CToolTipCtrl::initial);
				m_pListTooltips->SetDelayTime(dwDelay/5,CToolTipCtrl::reshow);
			}
			else
			{
				m_pListTooltips->SetDelayTime(UINT(-1),CToolTipCtrl::initial);
				m_pListTooltips->SetDelayTime(UINT(-1),CToolTipCtrl::reshow);
			}

		}
	}
}

void CLocateDlg::DeleteTooltipTools()
{
	ASSERT (m_pListTooltips!=NULL);

	m_pListTooltips->Pop();

	m_iTooltipItem=-1;
	m_iTooltipSubItem=-1;
	
	TOOLINFO ti;
	ti.cbSize=TTTOOLINFOA_V2_SIZE;
	for (int i=m_pListTooltips->GetToolCount()-1;m_pListTooltips->EnumTools(i,&ti) && i>=0;i--)
		m_pListTooltips->DelTool(&ti);

}


////////////////////////////////////////////////////////////
// CLocateDlg - Presets
////////////////////////////////////////////////////////////

BOOL CLocateDlg::OnPresetsSave()
{
	CSavePresetDlg PresetDialog;
	if (!PresetDialog.DoModal(*this))
		return FALSE; // Cancel pressed



	DWORD dwPresets;
	DWORD dwID=CheckExistenceOfPreset(PresetDialog.m_sReturnedPreset,&dwPresets);

	if (dwID==DWORD(-1))
		dwID=dwPresets;

	CRegKey2 MainKey;
	CRegKey PresetKey;

	if (MainKey.OpenKey(HKCU,"\\Dialogs\\SearchPresets",
		CRegKey::createNew|CRegKey::samAll)!=ERROR_SUCCESS)
		return FALSE;

	char szKeyName[30];
	StringCbPrintf(szKeyName,30,"Preset %03d",dwID);

	// Deleting key if it exists
	MainKey.DeleteKey(szKeyName);
	
	if (PresetKey.OpenKey(MainKey,szKeyName,CRegKey::createNew|CRegKey::samAll)!=ERROR_SUCCESS)
		return FALSE;
	
	PresetKey.SetValue(L"",PresetDialog.m_sReturnedPreset);
	m_NameDlg.SaveControlStates(PresetKey);
	m_SizeDateDlg.SaveControlStates(PresetKey);
	m_AdvancedDlg.SaveControlStates(PresetKey);

	return TRUE;
}

BOOL CLocateDlg::OnPresetsSelection(int nPreset)
{
	CRegKey2 RegKey;
	CRegKey PresetKey;
	
	if (RegKey.OpenKey(HKCU,"\\Dialogs\\SearchPresets",CRegKey::openExist|CRegKey::samRead)!=ERROR_SUCCESS)
		return FALSE;

	char szBuffer[30];
	StringCbPrintf(szBuffer,30,"Preset %03d",nPreset);

	if (PresetKey.OpenKey(RegKey,szBuffer,CRegKey::openExist|CRegKey::samRead)!=ERROR_SUCCESS)
		return FALSE;

	m_NameDlg.LoadControlStates(PresetKey);
	m_NameDlg.EnableItems(TRUE);

	m_SizeDateDlg.LoadControlStates(PresetKey,TRUE);
	m_SizeDateDlg.EnableItems(TRUE);

	m_AdvancedDlg.LoadControlStates(PresetKey);
	m_AdvancedDlg.EnableItems(TRUE);

	return TRUE;
}

BOOL CLocateDlg::LoadPreset(LPCWSTR szPreset)
{
	CRegKey2 RegKey;
	CRegKey PresetKey;


	if (RegKey.OpenKey(HKCU,"\\Dialogs\\SearchPresets",CRegKey::openExist|CRegKey::samRead)!=ERROR_SUCCESS)
		return FALSE;
	
	// Finding registry entry for the preset
	for (int nPreset=0;;nPreset++) 
	{
		CHAR szBuffer[30];
		StringCbPrintf(szBuffer,30,"Preset %03d",nPreset);

		if (PresetKey.OpenKey(RegKey,szBuffer,CRegKey::openExist|CRegKey::samRead)!=ERROR_SUCCESS)
			return FALSE;

		CStringW Name;
		if (PresetKey.QueryValue(L"",Name))
		{
			if (Name.Compare(szPreset)==0)
				break;
		}
	} 

	m_NameDlg.LoadControlStates(PresetKey);
	m_NameDlg.EnableItems(TRUE);

	m_SizeDateDlg.LoadControlStates(PresetKey,TRUE);
	m_SizeDateDlg.EnableItems(TRUE);

	m_AdvancedDlg.LoadControlStates(PresetKey);
	m_AdvancedDlg.EnableItems(TRUE);

	// Give focus for "Named:" if the button is used
	if (GetFocus()==GetDlgItem(IDC_PRESETS))
		SetFocus(IDC_NAME);

	return TRUE;
}

DWORD CLocateDlg::CheckExistenceOfPreset(LPCWSTR szName,DWORD* pdwPresets) // Returns index to preset or FFFFFFFF
{
	// First, find free indentifier
	CRegKey2 RegKey;
	
	if (RegKey.OpenKey(HKCU,"\\Dialogs\\SearchPresets",CRegKey::openExist|CRegKey::samRead)!=ERROR_SUCCESS)
	{
		if (pdwPresets!=NULL)
			*pdwPresets=0;
		return DWORD(-1);
	}
	DWORD nID=DWORD(-1);

	int nPreset;
	CRegKey RegKey2;
	char szBuffer[30];

	for (nPreset=0;nPreset<1000;nPreset++)
	{
		StringCbPrintf(szBuffer,30,"Preset %03d",nPreset);

		if (RegKey2.OpenKey(RegKey,szBuffer,CRegKey::openExist|CRegKey::samRead)!=ERROR_SUCCESS)
			break;

		CStringW sCurrentName;
		if (RegKey2.QueryValue(L"",sCurrentName)>0)
		{
			if (sCurrentName.CompareNoCase(szName)==0)
			{
				if (pdwPresets==NULL)
					return DWORD(nPreset);
				nID=nPreset;
			}
		}
		RegKey2.CloseKey();
	}		

	if (pdwPresets!=NULL)
		*pdwPresets=nPreset;

	return nID;
}

void CLocateDlg::LoadDialogTexts()
{
	CRegKey2 RegKey;
	if (RegKey.OpenKey(HKCU,"\\Dialogs",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
	{
		m_NameDlg.LoadControlStates(RegKey);
		m_NameDlg.EnableItems(TRUE);

		m_SizeDateDlg.LoadControlStates(RegKey,FALSE);
		m_SizeDateDlg.EnableItems(TRUE);

		m_AdvancedDlg.LoadControlStates(RegKey);
		m_AdvancedDlg.EnableItems(TRUE);

		OnFieldChange(isAllChanged);
	}
	
}

void CLocateDlg::SaveDialogTexts()
{
	CRegKey2 RegKey;

	if(RegKey.OpenKey(HKCU,"\\Dialogs",CRegKey::defWrite)==ERROR_SUCCESS)
	{
		m_NameDlg.SaveControlStates(RegKey);
		m_SizeDateDlg.SaveControlStates(RegKey);
		m_AdvancedDlg.SaveControlStates(RegKey);
		
	}
}



////////////////////////////////////////////////////////////
// CLocateDlg - Keyboard shortcuts
////////////////////////////////////////////////////////////

void CLocateDlg::SetShortcuts()
{
	CLocateDlgThread* pLocateDlgThread=GetTrayIconWnd()->GetLocateDlgThread();
	// This function should not be called from other thread than dialog's thread
	// (to be sure that m_Accels is not accessed during this call
	ASSERT(GetCurrentThread()!=*pLocateDlgThread);
	

	// Clear accel tables
	ClearShortcuts();

	// Loading shortcuts
	if (!CShortcut::LoadShortcuts(m_aShortcuts,CShortcut::loadLocal))
	{
        if (!CShortcut::GetDefaultShortcuts(m_aShortcuts,CShortcut::loadLocal))
		{
			SetMenus();
			return;
		}
	}

	HWND hDialogs[]={*this,m_NameDlg,m_SizeDateDlg,m_AdvancedDlg,NULL};
	CShortcut::ResolveMnemonics(m_aShortcuts,hDialogs);

	// Cointing items
	UINT nInResultListAccels=0,nNotInResultListAccels=0;
	for (int i=0;i<m_aShortcuts.GetSize();i++)
	{
		ASSERT((m_aShortcuts[i]->m_dwFlags&CShortcut::sfKeyTypeMask)==CShortcut::sfLocal);

		if (m_aShortcuts[i]->m_wWhenPressed&CShortcut::wpFocusInResultList)
			nInResultListAccels++;
		if (m_aShortcuts[i]->m_wWhenPressed&CShortcut::wpFocusNotInResultList)
			nNotInResultListAccels++;
	}

	// Creating result list accels
	if (nInResultListAccels>0)
	{
		ASSERT(m_pListCtrl!=NULL);

		ACCEL* pAccels=new ACCEL[nInResultListAccels];
		

		int nAccels=0;
		for (int i=0;i<m_aShortcuts.GetSize();i++)
		{
			if (!(m_aShortcuts[i]->m_wWhenPressed&CShortcut::wpFocusInResultList))
				continue;
			
			pAccels[nAccels].fVirt=FNOINVERT|FVIRTKEY;
			if (m_aShortcuts[i]->m_bModifiers&CShortcut::ModifierAlt)
				pAccels[nAccels].fVirt|=FALT;
			if (m_aShortcuts[i]->m_bModifiers&CShortcut::ModifierControl)
				pAccels[nAccels].fVirt|=FCONTROL;
			if (m_aShortcuts[i]->m_bModifiers&CShortcut::ModifierShift)
				pAccels[nAccels].fVirt|=FSHIFT;

			// Check whether accel already listed
			for (int k=0;k<nAccels;k++)
			{
				if (pAccels[k].key==m_aShortcuts[i]->m_bVirtualKey &&
					pAccels[k].fVirt==pAccels[nAccels].fVirt)
				{
					UINT nAlreadyInList;
					for (nAlreadyInList=0;m_aActiveShortcuts[k][nAlreadyInList]!=NULL;nAlreadyInList++);

                    CShortcut** pList=new CShortcut*[nAlreadyInList+2];
					CopyMemory(pList,m_aActiveShortcuts[k],sizeof(CShortcut*)*nAlreadyInList);
					pList[nAlreadyInList++]=m_aShortcuts[i];
					pList[nAlreadyInList]=NULL;

					delete[] m_aActiveShortcuts[k];
					m_aActiveShortcuts[k]=pList;

					continue;
				}
			}


			// Registering new active shortcut
			CShortcut** pList=new CShortcut*[2];
			pList[0]=m_aShortcuts[i];
			pList[1]=NULL;

			pAccels[nAccels].key=m_aShortcuts[i]->m_bVirtualKey;
			pAccels[nAccels].cmd=IDM_DEFSHORTCUTITEM+m_aActiveShortcuts.GetSize();
			m_aActiveShortcuts.Add(pList);
			

			nAccels++;
		}
		HACCEL hAccel=CreateAcceleratorTable(pAccels,nInResultListAccels);
		pLocateDlgThread->SetAccelTableForWindow(*m_pListCtrl,hAccel,FALSE,*this);
		delete[] pAccels;
	}

	// Creating accels for other controls
	if (nNotInResultListAccels>0)
	{
		ASSERT((HWND)m_NameDlg!=NULL);

		ACCEL* pAccels=new ACCEL[nNotInResultListAccels];
		int nFirstActive=m_aActiveShortcuts.GetSize();

		int nAccels=0;
		for (int i=0;i<m_aShortcuts.GetSize();i++)
		{
			if (!(m_aShortcuts[i]->m_wWhenPressed&CShortcut::wpFocusNotInResultList))
				continue;

			
			pAccels[nAccels].fVirt=FNOINVERT|FVIRTKEY;
			if (m_aShortcuts[i]->m_bModifiers&CShortcut::ModifierAlt)
				pAccels[nAccels].fVirt|=FALT;
			if (m_aShortcuts[i]->m_bModifiers&CShortcut::ModifierControl)
				pAccels[nAccels].fVirt|=FCONTROL;
			if (m_aShortcuts[i]->m_bModifiers&CShortcut::ModifierShift)
				pAccels[nAccels].fVirt|=FSHIFT;
			pAccels[nAccels].cmd=IDM_DEFSHORTCUTITEM+i;
			
			// Check whether accel already listed
			for (int k=0;k<nAccels;k++)
			{
				if (pAccels[k].key==m_aShortcuts[i]->m_bVirtualKey &&
					pAccels[k].fVirt==pAccels[nAccels].fVirt)
				{
					UINT nAlreadyInList;
					for (nAlreadyInList=0;m_aActiveShortcuts[nFirstActive+k][nAlreadyInList]!=NULL;nAlreadyInList++);

                    CShortcut** pList=new CShortcut*[nAlreadyInList+2];
					CopyMemory(pList,m_aActiveShortcuts[nFirstActive+k],sizeof(CShortcut*)*nAlreadyInList);
					pList[nAlreadyInList++]=m_aShortcuts[i];
					pList[nAlreadyInList]=NULL;

					delete[] m_aActiveShortcuts[nFirstActive+k];
					m_aActiveShortcuts[nFirstActive+k]=pList;

					continue;
				}
			}


			// Registering new active shortcut
			CShortcut** pList=new CShortcut*[2];
			pList[0]=m_aShortcuts[i];
			pList[1]=NULL;

			pAccels[nAccels].key=m_aShortcuts[i]->m_bVirtualKey;
			pAccels[nAccels].cmd=IDM_DEFSHORTCUTITEM+m_aActiveShortcuts.GetSize();
			m_aActiveShortcuts.Add(pList);
			

			nAccels++;
		}
		HACCEL hAccel=CreateAcceleratorTable(pAccels,nNotInResultListAccels);
		
		pLocateDlgThread->SetAccelTableForWindow(*this,hAccel,FALSE,*this);
		pLocateDlgThread->SetAccelTableForChilds(*this,hAccel,TRUE,*this);
		pLocateDlgThread->SetAccelTableForWindow(m_NameDlg,hAccel,FALSE,*this);
		pLocateDlgThread->SetAccelTableForChilds(m_NameDlg,hAccel,FALSE,*this);
		pLocateDlgThread->SetAccelTableForWindow(m_SizeDateDlg,hAccel,FALSE,*this);
		pLocateDlgThread->SetAccelTableForChilds(m_SizeDateDlg,hAccel,FALSE,*this);
		pLocateDlgThread->SetAccelTableForWindow(m_AdvancedDlg,hAccel,FALSE,*this);
		pLocateDlgThread->SetAccelTableForChilds(m_AdvancedDlg,hAccel,FALSE,*this);
		
		delete[] pAccels;
	}


	SetMenus();
}

void CLocateDlg::ClearShortcuts()
{
	CLocateDlgThread* pLocateDlgThread=GetTrayIconWnd()->GetLocateDlgThread();
	// This function should not be called from other thread than dialog's thread
	// (to be sure that m_Accels is not accessed during this call
	ASSERT(GetCurrentThread()!=*pLocateDlgThread);
	
	// Name dlg accel
	HACCEL hAccel=pLocateDlgThread->GetAccelTableForWindow(m_NameDlg);
	if (hAccel!=NULL)
		DestroyAcceleratorTable(hAccel);
	
	// Size and date dlg accel
	hAccel=pLocateDlgThread->GetAccelTableForWindow(m_SizeDateDlg);
	if (hAccel!=NULL)
		DestroyAcceleratorTable(hAccel);
	
	// Advanced dialog accel
	hAccel=pLocateDlgThread->GetAccelTableForWindow(m_AdvancedDlg);
	if (hAccel!=NULL)
		DestroyAcceleratorTable(hAccel);
	
	// Result list accel
	if (m_pListCtrl!=NULL)
	{
		hAccel=pLocateDlgThread->GetAccelTableForWindow(*m_pListCtrl);
		if (hAccel!=NULL)
			DestroyAcceleratorTable(hAccel);
	}

	pLocateDlgThread->ClearAccelTables();
	m_aShortcuts.RemoveAll();
	m_aActiveShortcuts.RemoveAll();

}

////////////////////////////////////////////////////////////
// CLocateDlg - DB Updater / Locaters related stuff
////////////////////////////////////////////////////////////

CLocater* CLocateDlg::ResolveParametersAndInitializeLocater(CArrayFAP<LPWSTR>& aExtensions,CArrayFAP<LPWSTR>& aDirectories,
															CArrayFAP<LPWSTR>& aNames,BOOL bForInstantSearch,BOOL bShowDatabasesDialog)
{
	// Resolving Name and Type, CNameDlg::OnOk can
	// stop execution of this function if Look In directory
	// is not ok
	CStringW Name;
	if (!m_NameDlg.GetNameExtensionsAndDirectories(Name,aExtensions,aDirectories,bForInstantSearch))
	{
		return NULL;
	}

	if (bForInstantSearch)
	{
		// Check if there is something which would restrict the number of results
		if (Name.GetLength()<(int)max(m_dwInstantChars,1) && aExtensions.GetSize()==0 && 
			aDirectories.GetSize()==0 && !m_SizeDateDlg.IsChanged())
			return NULL;
	}



	// Loading databases
	CArray<PDATABASE>* pDatabases;
	BOOL bFreeDatabases=FALSE;
	if (bShowDatabasesDialog)
	{
		// Use Select databases dialog
		pDatabases=new CArray<PDATABASE>;
		bFreeDatabases=TRUE;

		CSelectDatabasesDlg dbd(GetLocateApp()->GetDatabases(),*pDatabases,
			CSelectDatabasesDlg::flagEnableUseDatabases|
			(GetLocateApp()->GetStartupFlags()&CLocateApp::CStartData::startupDatabasesOverridden?CSelectDatabasesDlg::flagDisablePresets:0),
			CRegKey2::GetCommonKey()+"\\Dialogs\\SelectDatabases/Locate");
		if (!dbd.DoModal(*this))
		{
			for (int i=0;i<pDatabases->GetSize();i++)
				delete pDatabases->GetAt(i);
			delete pDatabases;
			return NULL;
		}

		if (dbd.m_bUseTemporally)
			AddExtraFlags(efUseLastSelectedDatabases);
		else
			RemoveExtraFlags(efUseLastSelectedDatabases);

	}
	else if (IsExtraFlagSet(efUseLastSelectedDatabases))
	{
		pDatabases=new CArray<PDATABASE>;
		bFreeDatabases=TRUE;

		if (!CSelectDatabasesDlg::GetLastSelectedDatabases(CRegKey2::GetCommonKey()+"\\Dialogs\\SelectDatabases/Locate",
			GetLocateApp()->GetDatabases(),*pDatabases))
		{
			for (int i=0;i<pDatabases->GetSize();i++)
				delete pDatabases->GetAt(i);
			delete pDatabases;
			return NULL;
		}

	}
	else
	{
		// Use enabled databases
		pDatabases=GetLocateApp()->GetDatabasesPtr();
		if (pDatabases->GetSize()==0)
		{
			if (!bForInstantSearch)
				ShowErrorMessage(IDS_ERRORCNODATABASESSELECTED,IDS_ERROR);
			return NULL;
		}
	}


	m_DatabasesUsedInSearch.RemoveAll();
	for (int i=0;i<pDatabases->GetSize();i++)
	{
		if (pDatabases->GetAt(i)->IsEnabled())
			m_DatabasesUsedInSearch.Add(pDatabases->GetAt(i)->GetID());
	}


	// Create locater object
	CLocater* pLocater=new CLocater(*pDatabases);

	// Databases pointer are not needed anymore
	if (bFreeDatabases)
	{
		for (int i=0;i<pDatabases->GetSize();i++)
			delete pDatabases->GetAt(i);
		delete pDatabases; 
	}


	// Calling routines for subdialogs
	m_SizeDateDlg.SetSizesAndDaterForLocater(pLocater);
	DWORD dwAdvancedFlags=m_AdvancedDlg.SetAdvancedFlagsForLocater(pLocater,bForInstantSearch);

	if (dwAdvancedFlags&CAdvancedDlg::flagMatchCase)
		pLocater->AddAdvancedFlags(LOCATE_NAMEMATCHCASE);
	if (IsExtraFlagSet(efMatchWhileNameIfAsterisks) && Name.Find(L'*')!=-1)
		dwAdvancedFlags|=CAdvancedDlg::flagMatchWholeNameOnly;
	

	//////////////////////////////////////////////////////7
	// Check name 

	BOOL bPlusOrMinusFound=FALSE; // String contains logical operations '+' and '-':

	if (!Name.IsEmpty()) 
	{
		if (Name[0]==':') 
		{
			// Name is regular expression, PCRE will be used
			Name.DelChar(0);
				
			if (Name.IsEmpty() || (Name.GetLength()==1 && Name[0]==L':'))
				Name.Empty(); // ":" or "::" only, tread as all files
			else
			{
				pLocater->AddAdvancedFlags(LOCATE_NAMEREGULAREXPRESSION);
				if (Name[0]==':')
				{
					Name.DelChar(0);
					pLocater->AddAdvancedFlags(LOCATE_CHECKWHOLEPATH);
				}
				else if (Name[0]==' ')
					Name.DelChar(0);

				aNames.Add(Name.GiveBuffer());
			}
		}
		else
		{
			// If replace spaces with asterisks is chosen, 
			// replace ' ' -> '*'
			if (dwAdvancedFlags&CAdvancedDlg::flagReplaceSpaces)
			{
				m_AdvancedDlg.ReplaceCharsWithAsterisks(Name);

				// Remove multiple (and therefore
				// meaningless) asterisks
				for (int i=0;i<Name.GetLength();i++)
				{
					if (Name[i]==L'*')
					{
						while (Name[i+1]==L'*')
							Name.DelChar(i+1);
					}
				}

			}

			
			// First replace '/' with '\\'
			Name.ReplaceChars('/','\\');

			// Parse string, split into separate string etc...
			LPCWSTR pStr=Name;
			UINT nIndex;
			
			for(;;)
			{
				// First, if logical operations are enabled, 
				// check whether + or - is present
				enum {
					NotSpecified,
					MustExist, // + found
					MustNotExist // - found
				} nType=NotSpecified;
				
				if (IsExtraFlagSet(efEnableLogicalOperations))
				{
					if (pStr[0]==L'+')
					{
						// '+' present, 
						nType=MustExist;
						bPlusOrMinusFound=TRUE;
						pStr++;
					}
					else if (pStr[0]==L'-')
					{
						// '-' present, 
						nType=MustNotExist;
						bPlusOrMinusFound=TRUE;
						pStr++;
					}
				}


				// Check whether apostrophes are used
				BOOL bApostrophes=FALSE;
				// Apostrophes may be found after spaces
				for (nIndex=0;pStr[nIndex]==' ';nIndex++); 
				
				if (pStr[nIndex]==L'\"')
				{
					// Parenthes on use
					bApostrophes=TRUE;
					pStr+=nIndex+1;

					// Calculate length
					for (nIndex=0;pStr[nIndex]!=L'\"' && pStr[nIndex]!=L'\0';nIndex++);
				}
				else if (IsExtraFlagSet(efAllowSpacesAsSeparators))
				{
					// Calculate length, separators are ' ', ',' and ';'
					for (nIndex=0;pStr[nIndex]!=L' ' && pStr[nIndex]!=L',' && pStr[nIndex]!=L';' && pStr[nIndex]!=L'|' && pStr[nIndex]!=L'\0';nIndex++);
				}
				else
				{
					// Calculate length, separators are ',' and ';'
					for (nIndex=0;pStr[nIndex]!=L',' && pStr[nIndex]!=L';' && pStr[nIndex]!=L'|' && pStr[nIndex]!=L'\0';nIndex++);
				}

				// If length > 0, insert to aNames list 
				if (nIndex>0)
				{
					// Add possible asterisks
					if (dwAdvancedFlags&CAdvancedDlg::flagMatchWholeNameOnly)
					{
						// Match case search, do not add asterisks
						if (nType==NotSpecified)
						{
							// No logical operatoins given
							aNames.Add(alloccopy(pStr,nIndex));
						}
						else
						{
							// Logical operations given, insert '+' or '-'
							WCHAR* pNewString=new WCHAR[nIndex+2];
							pNewString[0]=nType==MustExist?L'+':L'-';
							MemCopyW(pNewString+1,pStr,nIndex);
							pNewString[nIndex+1]='\0';
							aNames.Add(pNewString);
						}
					}
					else
					{
						// No match case, inserting '*' to the begin and the end 
						// (no end if extension is given)
						
						
						// 4 bytes extra in alloctaion is enough for '*' at the begin 
						// and end of string, for '\0' and for '+' or '-'
						WCHAR* pNewString=new WCHAR[nIndex+4]; 
						WCHAR* pNewStringPtr=pNewString;

						if (nType!=NotSpecified)
						{
							*pNewStringPtr=nType==MustExist?L'+':L'-';
							
							// Move pointer forward
							pNewStringPtr++;
						}


						if (pStr[0]!=L'*')
						{
							// No asterisk at the beginning of string, 
							// Insert asterisk
							*pNewStringPtr=L'*';
							pNewStringPtr++;
						}

						// Copy string
						MemCopyW(pNewStringPtr,pStr,nIndex);
						pNewStringPtr+=nIndex;
							
						
						
						if (pStr[nIndex-1]!=L'*')
						{
							// Insert asterisk to the end of string if necessary (if there is 
							// not extension specified or * is wanted anyway)
							if (GetExtraFlags()&efAsteriskAtEndEvenIfExtensionExists ||
								!(pLocater->GetAdvancedFlags()&LOCATE_FILENAMES) ||
								FirstCharIndex(pStr,L'.')==-1)
								*(pNewStringPtr++)=L'*';
						}
						

						pNewStringPtr[0]=L'\0';
						

						aNames.Add(pNewString);

					}
	
			
				}				

				if (bApostrophes)
				{
					// Apostrophe was used at the beginning of string,
					// terminate processing if no apostrophe at the end
					if (pStr[nIndex]!=L'\"')
						break;

					// Move pointer
					pStr+=nIndex+1;
					
					// Ignore spaces after apostrophe
					while (*pStr==L' ')
						pStr++;
				}
				else
				{
					// Move pointer
					pStr+=nIndex;
				}

				if (IsExtraFlagSet(efAllowSpacesAsSeparators))
				{
					// Ignore all separators
					while (*pStr==L' ' || *pStr==',' || *pStr==';' || *pStr==L'|')
						pStr++;
					if (*pStr=='\0')
						break;
				}
				else
				{
					// Separator must found
					if (*pStr!=L',' && *pStr!=L';' && *pStr!=L'|')
						break;
					pStr++;
				}
				
			}
		}
	}

	// Enable Logical Operations
	if (!(pLocater->GetAdvancedFlags()&LOCATE_NAMEREGULAREXPRESSION) &&
		(bPlusOrMinusFound || (IsExtraFlagSet(efAndModeAlways) && Name.Find(L'|')==-1)))
	{
		// '+' or '-' found
		pLocater->AddAdvancedFlags(LOCATE_LOGICALOPERATIONS);

		// Insert '+' for string which does not have '+' or '-'
		for (int i=0;i<aNames.GetSize();i++)
		{
			if (aNames[i][0]!=L'+' && aNames[i][0]!=L'-')
			{
				int nLen=istrlen(aNames[i])+1;
				LPWSTR pNew=new WCHAR[nLen+1];
				pNew[0]=L'+';
				MemCopyW(pNew+1,aNames[i],nLen);
				delete[] aNames[i];
				aNames[i]=pNew;
			}
		}
	}
				

	// Check if extension contains logicical operations
	if (IsExtraFlagSet(efEnableLogicalOperations))
	{
		for (int i=0;i<aExtensions.GetSize();i++)
		{
			if (aExtensions[i][0]==L'-')
			{
				pLocater->AddAdvancedFlags(LOCATE_LOGICALOPERATIONSINEXT);
				break;
			}
		}
	}
	
	

	// Extensions related checks
	if (aExtensions.GetSize()==0 && 
		(pLocater->GetAdvancedFlags()&(LOCATE_FILENAMES|LOCATE_FOLDERNAMES))!=LOCATE_FOLDERNAMES)
	{
		// No extensions given, checking if name contains extension
		pLocater->AddAdvancedFlags(LOCATE_EXTENSIONWITHNAME);
	}



	// No bubdirectories checkbox set?
	if (m_NameDlg.IsDlgButtonChecked(IDC_NOSUBDIRECTORIES))
		pLocater->AddAdvancedFlags(LOCATE_NOSUBDIRECTORIES);



	if (!bForInstantSearch || IsInstantSearchingFlagSet(isUpdateTitle))
	{
		CStringW Title;
		// Making title
		Title.LoadString(IDS_TITLE);
		if (pLocater->GetAdvancedFlags()&LOCATE_NAMEREGULAREXPRESSION)
		{
			if (pLocater->GetAdvancedFlags()&LOCATE_CHECKWHOLEPATH)
				Title.AddString(IDS_REGULAREXPRESSIONFULLPATH);
			else
				Title.AddString(IDS_REGULAREXPRESSION);
			Title << aNames[0];
		}
		else if (aNames.GetSize()==0)
		{
			if (aExtensions.GetSize()==0)
				Title.AddString(IDS_ALLFILES);
			else
			{
				CStringW Extensions,Format;
				for (int i=0;i<aExtensions.GetSize();i++)
					Extensions << aExtensions[i] << L' ';
				Format.Format(IDS_TITLEEXTENSIONS,(LPCWSTR)Extensions);
				Title << Format;
			}
		}
		else
		{
			CStringW Format;
			
			if (aNames.GetSize()==1)
			{
				if (aExtensions.GetSize()==1)
				{
					CStringW Files(aNames[0]);
					Files << L'.' << aExtensions[0];
					Format.Format(IDS_TITLESEARCHRESULTS,LPCWSTR(Files));
				}
				else
					Format.Format(IDS_TITLESEARCHRESULTS,LPCWSTR(aNames[0]));
			}
			else
				Format.Format(IDS_TITLESEARCHRESULTS,LPCWSTR(Name));

			Title << Format;
		}

		if (GetLocateApp()->m_nInstance>0)
			Title << L" (" << DWORD(GetLocateApp()->m_nInstance+1) << L')';
		SetText(Title);

	}

	return pLocater;
	
}

BOOL CLocateDlg::LocateProc(DWORD_PTR dwParam,CallingReason crReason,UpdateError ueCode,DWORD_PTR dwInfo,const CLocater* pLocater)
{
	DbcDebugFormatMessage2("CLocateDlg::LocateProc BEGIN, reason=%d, code=%d",crReason,ueCode);
	
	switch (crReason)
	{
	case StartedDatabase:
		while (GetLocateApp()->IsWritingDatabases())
		{
			((CLocateDlg*)dwParam)->m_pStatusCtrl->SetText(ID2W(IDS_LOCATINGWAITDBWRITING),STATUSBAR_OPERATIONSTATUS,0);
			Sleep(200);
		}
		break;
	case Initializing:
	{
		if (ueCode!=ueStillWorking && ueCode!=ueSuccess) // Initializing failed
		{
			((CLocateDlg*)dwParam)->m_pStatusCtrl->SetText(ID2W(IDS_LOCATINGFAILED),STATUSBAR_OPERATIONSTATUS,0);
			return FALSE;
		}

		// Disabling items and give focus to to whole dialog
		((CLocateDlg*)dwParam)->SendMessage(WM_ENABLEITEMS,FALSE);

		
		((CLocateDlg*)dwParam)->m_pStatusCtrl->SetText(szEmpty,STATUSBAR_MISC,0);
		((CLocateDlg*)dwParam)->m_pStatusCtrl->SetText(ID2W(IDS_LOCATING),STATUSBAR_OPERATIONSTATUS,0);
		((CLocateDlg*)dwParam)->StartLocateAnimation();

		
		// Selecting path column
		int nColumn=-1;
		if (((CLocateDlg*)dwParam)->m_nSorting==BYTE(-1))
			nColumn=((CLocateDlg*)dwParam)->m_pListCtrl->GetVisibleColumn(((CLocateDlg*)dwParam)->m_pListCtrl->GetColumnFromID(InFolder));
		else if ((((CLocateDlg*)dwParam)->m_nSorting&127)!=Extension)
			nColumn=((CLocateDlg*)dwParam)->m_pListCtrl->GetVisibleColumn(((CLocateDlg*)dwParam)->m_pListCtrl->GetColumnFromID(((CLocateDlg*)dwParam)->m_nSorting&127));
		
		if (nColumn!=-1)
			((CLocateDlg*)dwParam)->m_pListCtrl->SendMessage(LVM_FIRST+140/* LVM_SETSELECTEDCOLUMN */,nColumn,0);


		// Clearing volume information
		((CLocateDlg*)dwParam)->m_aVolumeInformation.RemoveAll();
		return TRUE;
	}
	case FinishedLocating:
	{
		// Stop animations
		((CLocateDlg*)dwParam)->StopLocateAnimation();
		
		// Selecting and focusing the first item in the list if not any selected
		if (((CLocateDlg*)dwParam)->m_pListCtrl->GetItemCount()>0 && 
			((CLocateDlg*)dwParam)->m_pListCtrl->GetSelectedCount()==0)
		{
			int nItem=((CLocateDlg*)dwParam)->m_pListCtrl->GetNextItem(-1,LVNI_ALL);
			if (nItem!=-1)
			{
				if (((CLocateDlg*)dwParam)->IsFlagSet(fgLVActivateFirstResult))
					((CLocateDlg*)dwParam)->m_pListCtrl->SetItemState(nItem,LVIS_FOCUSED|LVIS_SELECTED,LVIS_FOCUSED|LVIS_SELECTED);
				else
					((CLocateDlg*)dwParam)->m_pListCtrl->SetItemState(nItem,LVIS_FOCUSED,LVIS_FOCUSED);
			}
		}

		// Enable/disable dialog items		
		((CLocateDlg*)dwParam)->SendMessage(WM_ENABLEITEMS,TRUE);


		CStringW NumberOfFiles;
		if (pLocater->GetNumberOfFoundFiles()>0)
		{
			if (pLocater->GetNumberOfFoundDirectories()>0)
				NumberOfFiles.FormatEx(IDS_ITEMSFOUND,pLocater->GetNumberOfFoundFiles(),pLocater->GetNumberOfFoundDirectories());
			else
				NumberOfFiles.Format(IDS_FILESFOUND,pLocater->GetNumberOfFoundFiles());
		}
		else if (pLocater->GetNumberOfFoundDirectories()>0)
			NumberOfFiles.FormatEx(IDS_DIRECTORIESFOUND,pLocater->GetNumberOfFoundDirectories());
		else
			NumberOfFiles.LoadString(IDS_NORESULTS);


		CStringW str;
		if (ueCode==ueStopped)
		{
			str.LoadString(IDS_LOCATINGCANCELLED);
			((CLocateDlg*)dwParam)->m_pStatusCtrl->SetText(LPCSTR(::LoadIcon(NULL,IDI_WARNING)),STATUSBAR_LOCATEICON,SBT_OWNERDRAW);
		}
		else if (ueCode==ueLimitReached)
		{
			str.LoadString(IDS_LOCATINGLIMITREACHED);
			((CLocateDlg*)dwParam)->m_pStatusCtrl->SetText(LPCSTR(::LoadIcon(NULL,IDI_INFORMATION)),STATUSBAR_LOCATEICON,SBT_OWNERDRAW);
		}
		else if (ueCode!=ueStillWorking && ueCode!=ueSuccess) // Locating failed
		{
			CStringW error;
			str.LoadString(IDS_LOCATINGFAILED);
			((CLocateDlg*)dwParam)->m_pStatusCtrl->SetText(LPCSTR(::LoadIcon(NULL,IDI_ERROR)),STATUSBAR_LOCATEICON,SBT_OWNERDRAW);
				
			switch (ueCode)
			{
			case ueUnknown:
				error.LoadString(IDS_LASTERRORUNKNOWN);
				break;
			case ueOpen:
				error.LoadString(IDS_LASTERRORCANNOTOPEN);
				break;
			case ueRead:
				error.LoadString(IDS_LASTERRORCANNOTREAD);
				break;
			case ueAlloc:
				error.LoadString(IDS_LASTERRORCANNOTALLOCATE);
				break;
			case ueInvalidDatabase:
				error.LoadString(IDS_LASTERRORINVALIDDB);
				break;
			}
			if (!error.IsEmpty())
			{
				CStringW str2;
				int nIndex=error.Find(L'%');
				if (nIndex!=-1)
				{
					error.DelChar(nIndex);
					error.DelChar(nIndex);
					if (nIndex>0 && error[nIndex-1]==' ')
						error.DelChar(nIndex-1);
				}
				
				LPCWSTR szDBName=pLocater->GetLastErrorDatabaseName();
				if (szDBName!=NULL)
					str2.FormatEx(IDS_ERROROCCUREDWITHDB,szDBName,LPCWSTR(error));
				else
					str2.FormatEx(IDS_ERROROCCURED,LPCWSTR(error));
				str << L" " << str2;

				((CLocateDlg*)dwParam)->m_pStatusCtrl->SetText(LPCSTR(::LoadIcon(NULL,IDI_ERROR)),STATUSBAR_LOCATEICON,SBT_OWNERDRAW);
			}
		}
		else
			str.LoadString(IDS_LOCATINGSUCCESS);

		str << L", " << NumberOfFiles;			
		((CLocateDlg*)dwParam)->m_pStatusCtrl->SetText(str,STATUSBAR_OPERATIONSTATUS,0);		
		((CLocateDlg*)dwParam)->m_pStatusCtrl->InvalidateRect(NULL,TRUE);
		((CLocateDlg*)dwParam)->CheckClipboard();
		return TRUE;
	}
	case SearchingStarted:
		if (pLocater->IsCurrentDatabaseUnicode())
		{
			CStringW text;
			text.Format(IDS_SEARCHINGFROMFILE,pLocater->GetFileNameW());
			((CLocateDlg*)dwParam)->m_pStatusCtrl->SetText(text,STATUSBAR_SEARCHFROMFILE,0);
		}
		else
		{
			CString text;
			text.Format(IDS_SEARCHINGFROMFILE,pLocater->GetFileName());
			((CLocateDlg*)dwParam)->m_pStatusCtrl->SetText(text,STATUSBAR_SEARCHFROMFILE,0);
		}
		break;
	case SearchingEnded:
		((CLocateDlg*)dwParam)->m_pStatusCtrl->SetText(STRNULL,STATUSBAR_SEARCHFROMFILE,0);
		break;
	case ClassShouldDelete:
		InterlockedExchangePointer((PVOID*)&((CLocateDlg*)dwParam)->m_pLocater,NULL);
		delete pLocater;
		
		// To update items, looks like this is only way
		((CLocateDlg*)dwParam)->SetTimer(ID_REDRAWITEMS,50);
		return TRUE;
	case ErrorOccured:
		switch (ueCode)
		{
		case ueUnknown:
			{
				WCHAR* pError=CLocateApp::FormatLastOsError();
				if (pError!=NULL)
				{
					CStringW str;
					str.Format(IDS_ERRORUNKNOWNOS,pError);
					while (str.LastChar()=='\n' || str.LastChar()=='\r')
					str.DelLastChar();
						
					if (CLocateApp::GetProgramFlags()&CLocateApp::pfShowCriticalErrors)
						((CLocateDlg*)dwParam)->MessageBox(str,ID2W(IDS_ERROR),MB_OK|MB_ICONERROR);
					((CLocateDlg*)dwParam)->m_pStatusCtrl->SetText(str,STATUSBAR_LOCATEERRORS,0);
					LocalFree(pError);
				}
				else
				{
					ID2W str(IDS_ERRORUNKNOWN);
					((CLocateDlg*)dwParam)->m_pStatusCtrl->SetText(str,STATUSBAR_LOCATEERRORS,0);
					if (CLocateApp::GetProgramFlags()&CLocateApp::pfShowCriticalErrors)
						((CLocateDlg*)dwParam)->MessageBox(str,ID2W(IDS_ERROR),MB_OK|MB_ICONERROR);
				}
			}
			return FALSE;
		case ueOpen:
		case ueCreate:
			{
				CStringW str;
				str.Format(IDS_ERRORCANNOTOPENDB,pLocater->GetCurrentDatabaseFile());
				((CLocateDlg*)dwParam)->m_pStatusCtrl->SetText(str,STATUSBAR_LOCATEERRORS,0);
				if (CLocateApp::GetProgramFlags()&CLocateApp::pfShowCriticalErrors)
				{
					WCHAR* pError=CLocateApp::FormatLastOsError();
					if (pError!=NULL)
						str.FormatEx(IDS_ERRORCANNOTOPENDBFORREAD,pLocater->GetCurrentDatabaseFile(),pError);
					((CLocateDlg*)dwParam)->MessageBox(str,ID2W(IDS_ERROR),MB_ICONERROR|MB_OK);
				}
			}
			return FALSE;
		case ueRead:
			{
				CStringW str;
				str.Format(IDS_ERRORCANNOTREADDB,pLocater->GetCurrentDatabaseFile());
				((CLocateDlg*)dwParam)->m_pStatusCtrl->SetText(str,STATUSBAR_LOCATEERRORS,0);
				if (CLocateApp::GetProgramFlags()&CLocateApp::pfShowCriticalErrors)
				{
					WCHAR* pError=CLocateApp::FormatLastOsError();
					if (pError!=NULL)
						str.FormatEx(IDS_ERRORCANNOTREADDBWITHOSERROR,pLocater->GetCurrentDatabaseFile(),pError);
					((CLocateDlg*)dwParam)->MessageBox(str,ID2W(IDS_ERROR),MB_ICONERROR|MB_OK);
				}				
				return FALSE;
			}
		case ueAlloc:
			((CLocateDlg*)dwParam)->m_pStatusCtrl->SetText(ID2W(IDS_ERRORCANNOTALLOCATE),STATUSBAR_LOCATEERRORS,0);
			if (CLocateApp::GetProgramFlags()&CLocateApp::pfShowCriticalErrors)
				((CLocateDlg*)dwParam)->ShowErrorMessage(IDS_ERRORCANNOTALLOCATE,IDS_ERROR);
			return FALSE;
		case ueInvalidDatabase:
			{
				CStringW str;
				str.Format(IDS_ERRORINVALIDDB,pLocater->GetCurrentDatabaseFile());
				((CLocateDlg*)dwParam)->m_pStatusCtrl->SetText(str,STATUSBAR_LOCATEERRORS,0);
						
				if (CLocateApp::GetProgramFlags()&CLocateApp::pfShowCriticalErrors)
					((CLocateDlg*)dwParam)->MessageBox(str,ID2W(IDS_ERROR),MB_ICONERROR|MB_OK);
				
				return FALSE;
			}
		}
		break;
	case RootInformationAvail:
		((CLocateDlg*)dwParam)->m_aVolumeInformation.Add(new VolumeInformation(
			pLocater->GetCurrentDatabaseID(),pLocater->GetCurrentDatabaseRootID(),
			pLocater->GetCurrentDatabaseRootType(),pLocater->GetCurrentDatabaseVolumeSerial(),
			pLocater->GetCurrentDatabaseVolumeLabel(),pLocater->GetCurrentDatabaseFileSystem()));
		break;
	}
	
	DbcDebugMessage("CLocateDlg::LocateProc END");
	return TRUE;
}

BOOL CLocateDlg::LocateInstantProc(DWORD_PTR dwParam,CallingReason crReason,UpdateError ueCode,DWORD_PTR dwInfo,const CLocater* pLocater)
{
	switch (crReason)
	{
	case StartedDatabase:
		while (GetLocateApp()->IsWritingDatabases())
		{
			((CLocateDlg*)dwParam)->m_pStatusCtrl->SetText(ID2W(IDS_LOCATINGWAITDBWRITING),STATUSBAR_OPERATIONSTATUS,0);
			Sleep(100);
		}
		break;
	case Initializing:
	{
		if (ueCode!=ueStillWorking && ueCode!=ueSuccess) // Initializing failed
			return FALSE;

		// Enable stop
		((CLocateDlg*)dwParam)->EnableDlgItem(IDC_STOP,TRUE);
		


		// Selecting path column
		int nColumn=-1;
		if (((CLocateDlg*)dwParam)->m_nSorting==BYTE(-1))
			nColumn=((CLocateDlg*)dwParam)->m_pListCtrl->GetVisibleColumn(((CLocateDlg*)dwParam)->m_pListCtrl->GetColumnFromID(InFolder));
		else if ((((CLocateDlg*)dwParam)->m_nSorting&127)!=Extension)
			nColumn=((CLocateDlg*)dwParam)->m_pListCtrl->GetVisibleColumn(((CLocateDlg*)dwParam)->m_pListCtrl->GetColumnFromID(((CLocateDlg*)dwParam)->m_nSorting&127));
		
		if (nColumn!=-1)
			((CLocateDlg*)dwParam)->m_pListCtrl->SendMessage(LVM_FIRST+140/* LVM_SETSELECTEDCOLUMN */,nColumn,0);


		// Clearing volume information
		((CLocateDlg*)dwParam)->m_aVolumeInformation.RemoveAll();
		return TRUE;
	}
	case FinishedLocating:
	{
		// Selecting and focusing the first item in the list if not any selected
		if (((CLocateDlg*)dwParam)->m_pListCtrl->GetItemCount()>0 && 
			((CLocateDlg*)dwParam)->m_pListCtrl->GetSelectedCount()==0)
		{
			int nItem=((CLocateDlg*)dwParam)->m_pListCtrl->GetNextItem(-1,LVNI_ALL);
			if (nItem!=-1)
			{
				if (((CLocateDlg*)dwParam)->IsFlagSet(fgLVActivateFirstResult))
					((CLocateDlg*)dwParam)->m_pListCtrl->SetItemState(nItem,LVIS_FOCUSED|LVIS_SELECTED,LVIS_FOCUSED|LVIS_SELECTED);
				else
					((CLocateDlg*)dwParam)->m_pListCtrl->SetItemState(nItem,LVIS_FOCUSED,LVIS_FOCUSED);
			}
				
		}

		// Disable stop and return default button state
		CButton FindNow(((CLocateDlg*)dwParam)->GetDlgItem(IDC_OK));
		CButton Stop(((CLocateDlg*)dwParam)->GetDlgItem(IDC_STOP));
		Stop.EnableWindow(FALSE);
		Stop.SetButtonStyle(BS_PUSHBUTTON);
		FindNow.SetButtonStyle(BS_DEFPUSHBUTTON);
	


		CStringW NumberOfFiles;
		if (pLocater->GetNumberOfFoundFiles()>0)
		{
			if (pLocater->GetNumberOfFoundDirectories()>0)
				NumberOfFiles.FormatEx(IDS_ITEMSFOUND,pLocater->GetNumberOfFoundFiles(),pLocater->GetNumberOfFoundDirectories());
			else
				NumberOfFiles.Format(IDS_FILESFOUND,pLocater->GetNumberOfFoundFiles());
		}
		else if (pLocater->GetNumberOfFoundDirectories()>0)
			NumberOfFiles.FormatEx(IDS_DIRECTORIESFOUND,pLocater->GetNumberOfFoundDirectories());
		else
			NumberOfFiles.LoadString(IDS_NORESULTS);

		// Minimal messages
		CStringW str;
		if (ueCode==ueLimitReached)
		{
			str.LoadString(IDS_LOCATINGLIMITREACHED);
			str << L", " << NumberOfFiles;
			((CLocateDlg*)dwParam)->m_pStatusCtrl->SetText(LPCSTR(::LoadIcon(NULL,IDI_INFORMATION)),STATUSBAR_LOCATEICON,SBT_OWNERDRAW);
		}
		else
			str=NumberOfFiles;

		if (ueCode!=ueStillWorking && ueCode!=ueSuccess &&
			ueCode!=ueLimitReached && ueCode!=ueStopped) // Locating failed, format error message
		{
			((CLocateDlg*)dwParam)->m_pStatusCtrl->SetText(LPCSTR(::LoadIcon(NULL,IDI_ERROR)),STATUSBAR_LOCATEICON,SBT_OWNERDRAW);
			
			CStringW error;
			switch (ueCode)
			{
			case ueUnknown:
				error.LoadString(IDS_LASTERRORUNKNOWN);
				break;
			case ueOpen:
				error.LoadString(IDS_LASTERRORCANNOTOPEN);
				break;
			case ueRead:
				error.LoadString(IDS_LASTERRORCANNOTREAD);
				break;
			case ueAlloc:
				error.LoadString(IDS_LASTERRORCANNOTALLOCATE);
				break;
			case ueInvalidDatabase:
				error.LoadString(IDS_LASTERRORINVALIDDB);
				break;
			}
			if (!error.IsEmpty())
			{
				CStringW str2;
				int nIndex=error.Find(L'%');
				if (nIndex!=-1)
				{
					error.DelChar(nIndex);
					error.DelChar(nIndex);
					if (nIndex>0 && error[nIndex-1]==' ')
						error.DelChar(nIndex-1);
				}

				LPCWSTR szDBName=pLocater->GetLastErrorDatabaseName();
				if (szDBName!=NULL)
					str2.FormatEx(IDS_ERROROCCUREDWITHDB,szDBName,LPCWSTR(error));
				else
					str2.FormatEx(IDS_ERROROCCURED,LPCWSTR(error));
				str << L" " << str2;
			}
		}

		((CLocateDlg*)dwParam)->m_pStatusCtrl->SetText(str,STATUSBAR_OPERATIONSTATUS,0);		
		((CLocateDlg*)dwParam)->m_pStatusCtrl->InvalidateRect(NULL,TRUE);

		return TRUE;
	}
	case SearchingStarted:
		if (pLocater->IsCurrentDatabaseUnicode())
		{
			CStringW text;
			text.Format(IDS_SEARCHINGFROMFILE,pLocater->GetFileNameW());
			((CLocateDlg*)dwParam)->m_pStatusCtrl->SetText(text,STATUSBAR_SEARCHFROMFILE,0);
		}
		else
		{
			CString text;
			text.Format(IDS_SEARCHINGFROMFILE,pLocater->GetFileName());
			((CLocateDlg*)dwParam)->m_pStatusCtrl->SetText(text,STATUSBAR_SEARCHFROMFILE,0);
		}
		break;
	case SearchingEnded:
		((CLocateDlg*)dwParam)->m_pStatusCtrl->SetText(STRNULL,STATUSBAR_SEARCHFROMFILE,0);
		break;
	case ClassShouldDelete:
		InterlockedExchangePointer((PVOID*)&((CLocateDlg*)dwParam)->m_pLocater,NULL);
		delete pLocater;
		
		((CLocateDlg*)dwParam)->RemoveInstantSearchingFlags(isSearching);

		// To update items, looks like this is only way
		((CLocateDlg*)dwParam)->SetTimer(ID_REDRAWITEMS,50);
		return TRUE;
	case ErrorOccured:
		switch (ueCode)
		{
		case ueUnknown:
			{
				WCHAR* pError=CLocateApp::FormatLastOsError();
				if (pError!=NULL)
				{
					CStringW str;
					str.Format(IDS_ERRORUNKNOWNOS,pError);
					while (str.LastChar()=='\n' || str.LastChar()=='\r')
					str.DelLastChar();
						
					((CLocateDlg*)dwParam)->m_pStatusCtrl->SetText(str,STATUSBAR_LOCATEERRORS,0);
					LocalFree(pError);
				}
				else
					((CLocateDlg*)dwParam)->m_pStatusCtrl->SetText(ID2W(IDS_ERRORUNKNOWN),STATUSBAR_LOCATEERRORS,0);
			}
			return FALSE;
		case ueOpen:
		case ueCreate:
			{
				CStringW str;
				str.Format(IDS_ERRORCANNOTOPENDB,pLocater->GetCurrentDatabaseFile());
				((CLocateDlg*)dwParam)->m_pStatusCtrl->SetText(str,STATUSBAR_LOCATEERRORS,0);
			}
			return FALSE;
		case ueRead:
			{
				CStringW str;
				str.Format(IDS_ERRORCANNOTREADDB,pLocater->GetCurrentDatabaseFile());
				((CLocateDlg*)dwParam)->m_pStatusCtrl->SetText(str,STATUSBAR_LOCATEERRORS,0);
				return FALSE;
			}
		case ueAlloc:
			((CLocateDlg*)dwParam)->m_pStatusCtrl->SetText(ID2W(IDS_ERRORCANNOTALLOCATE),STATUSBAR_LOCATEERRORS,0);
			return FALSE;
		case ueInvalidDatabase:
			{
				CStringW str;
				str.Format(IDS_ERRORINVALIDDB,pLocater->GetCurrentDatabaseFile());
				((CLocateDlg*)dwParam)->m_pStatusCtrl->SetText(str,STATUSBAR_LOCATEERRORS,0);
				return FALSE;
			}
		}
		break;
	case RootInformationAvail:
		((CLocateDlg*)dwParam)->m_aVolumeInformation.Add(new VolumeInformation(
			pLocater->GetCurrentDatabaseID(),pLocater->GetCurrentDatabaseRootID(),
			pLocater->GetCurrentDatabaseRootType(),pLocater->GetCurrentDatabaseVolumeSerial(),
			pLocater->GetCurrentDatabaseVolumeLabel(),pLocater->GetCurrentDatabaseFileSystem()));
		break;
	}
	return TRUE;
}


BOOL CALLBACK CLocateDlg::LocateFoundProc(DWORD_PTR dwParam,BOOL bFolder,const CLocater* pLocater)
{
	DbcDebugMessage("CLocateDlg::LocateFoundProc BEGIN");
	
	// Hide system and hidden files if it's wanted
	if (((CLocateDlg*)dwParam)->GetFlags()&fgLVDontShowHiddenFiles)
	{
		if (bFolder)
		{
			if (pLocater->GetFolderAttributes()&(LITEMATTRIB_HIDDEN))
			{
				pLocater->IgnoreThisResult(TRUE);
				return TRUE;
			}
		}
		else if (pLocater->GetFileAttributes()&(LITEMATTRIB_HIDDEN))
		{
			pLocater->IgnoreThisResult(FALSE);
			return TRUE;
		}
	}

	

	// Check wheter item is already added
	if (((CLocateDlg*)dwParam)->GetFlags()&fgLVNoDoubleItems)
	{
		// Setting path, name and extension
		char szPath[MAX_PATH+2],szPath2[MAX_PATH+2];

		if (bFolder)
		{
			DWORD nPathLen=pLocater->GetCurrentPathLen();

			ASSERT(pLocater->GetCurrentPathLen()+pLocater->GetFolderNameLen()<MAX_PATH-1);

			sMemCopy(szPath,pLocater->GetCurrentPath(),nPathLen);
			szPath[nPathLen++]='\\';
			sMemCopy(szPath+nPathLen,pLocater->GetFolderName(),pLocater->GetFolderNameLen()+1);
		}
		else
		{
			DWORD nPathLen=pLocater->GetCurrentPathLen();

			ASSERT(pLocater->GetCurrentPathLen()+pLocater->GetFileNameLen()<MAX_PATH-1);

			sMemCopy(szPath,pLocater->GetCurrentPath(),nPathLen);
			szPath[nPathLen++]='\\';
			sMemCopy(szPath+nPathLen,pLocater->GetFileName(),pLocater->GetFileNameLen()+1);
		}
		MakeLower(szPath);

		
		if (((CLocateDlg*)dwParam)->m_pListCtrl->GetItemCount()>0)
		{
			int nItem=((CLocateDlg*)dwParam)->m_pListCtrl->GetNextItem(-1,LVNI_ALL);
			while (nItem!=-1)
			{
				CLocatedItem* pItem=(CLocatedItem*)((CLocateDlg*)dwParam)->m_pListCtrl->GetItemData(nItem);
				if (pItem!=NULL)
				{
					ASSERT(pItem->GetPathLen()<MAX_PATH);

					MemCopyWtoA(szPath2,MAX_PATH+2,pItem->GetPath(),pItem->GetPathLen()+1);
					MakeLower(szPath2);
				
					
					if (strcmp(szPath,szPath2)==0)
					{
						pLocater->IgnoreThisResult(bFolder);
						return TRUE; // Alreafy found
					}
					
				}
				nItem=((CLocateDlg*)dwParam)->m_pListCtrl->GetNextItem(nItem,LVNI_ALL);
			}
		}		

	}

	LV_ITEM li;
	li.mask=LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM;
	li.iSubItem=0;
	li.iImage=I_IMAGECALLBACK;
	li.lParam=(LPARAM)new CLocatedItem;
	if (li.lParam==NULL)
		return FALSE;
	if (bFolder)
		((CLocatedItem*)li.lParam)->SetFolder(pLocater);
	else
		((CLocatedItem*)li.lParam)->SetFile(pLocater);

	li.pszText=LPSTR_TEXTCALLBACK;

	// Check wheter file/folder is deleted
	if (((CLocateDlg*)dwParam)->GetExtraFlags()&efLVDontShowDeletedFiles)
	{
		if (bFolder)
		{
			if (!FileSystem::IsDirectory(((CLocatedItem*)li.lParam)->GetPath()))
			{
				delete (CLocatedItem*)li.lParam;
				pLocater->IgnoreThisResult(TRUE);
				return TRUE;
			}
		}
		else
		{
			if (!FileSystem::IsFile(((CLocatedItem*)li.lParam)->GetPath()))
			{
				delete (CLocatedItem*)li.lParam;
				pLocater->IgnoreThisResult(FALSE);
				return TRUE;
			}
		}
	}


	// To prevent drawing error
	DWORD dwResults=pLocater->GetNumberOfResults();
	
	if (dwResults%60==59)
		Sleep(((CLocateDlg*)dwParam)->m_WaitEvery60);
	else if (dwResults%30==29)
		Sleep(((CLocateDlg*)dwParam)->m_WaitEvery30);

	
	if (((CLocateDlg*)dwParam)->m_nSorting==BYTE(-1))
	{
		//li.iItem=((CLocateDlg*)dwParam)->m_pListCtrl->GetItemCount();
		if (((CLocateDlg*)dwParam)->GetFlags()&fgLVFoldersFirst && bFolder)
			li.iItem=pLocater->GetNumberOfFoundDirectories();
		else
			li.iItem=dwResults;
	}
	else
	{
		li.iItem=SortNewItem(((CLocateDlg*)dwParam)->m_pListCtrl,
			(CLocatedItem*)li.lParam,((CLocateDlg*)dwParam)->m_nSorting);
	}

	
	((CLocateDlg*)dwParam)->m_pListCtrl->InsertItem(&li);

	
	DbcDebugMessage("CLocateDlg::LocateFoundProc END");
	return TRUE;
}

BOOL CALLBACK CLocateDlg::LocateFoundProcW(DWORD_PTR dwParam,BOOL bFolder,const CLocater* pLocater)
{
	DbcDebugMessage("CLocateDlg::LocateFoundProcW BEGIN");
	
	// Hide system and hidden files if it's wanted
	if (((CLocateDlg*)dwParam)->GetFlags()&fgLVDontShowHiddenFiles)
	{
		if (bFolder)
		{
			if (pLocater->GetFolderAttributes()&(LITEMATTRIB_HIDDEN))
			{
				pLocater->IgnoreThisResult(TRUE);
				return TRUE;
			}
		}
		else if (pLocater->GetFileAttributes()&(LITEMATTRIB_HIDDEN))
		{
			pLocater->IgnoreThisResult(FALSE);
			return TRUE;
		}
	}

	// Check wheter item is already added
	if (((CLocateDlg*)dwParam)->GetFlags()&fgLVNoDoubleItems)
	{
		// Setting path, name and extension
		WCHAR szPath[MAX_PATH+2],szPath2[MAX_PATH+2];

		if (bFolder)
		{
			DWORD nPathLen=pLocater->GetCurrentPathLen();

			ASSERT(pLocater->GetCurrentPathLen()+pLocater->GetFolderNameLen()<MAX_PATH-1);

			MemCopyW(szPath,pLocater->GetCurrentPathW(),nPathLen);
			szPath[nPathLen++]=L'\\';
			MemCopyW(szPath+nPathLen,pLocater->GetFolderNameW(),pLocater->GetFolderNameLen()+1);
		}
		else
		{
			DWORD nPathLen=pLocater->GetCurrentPathLen();

			ASSERT(pLocater->GetCurrentPathLen()+pLocater->GetFileNameLen()<MAX_PATH-1);

			MemCopyW(szPath,pLocater->GetCurrentPathW(),nPathLen);
			szPath[nPathLen++]='\\';
			MemCopyW(szPath+nPathLen,pLocater->GetFileNameW(),pLocater->GetFileNameLen()+1);
		}
		MakeLower(szPath);
		
		if (((CLocateDlg*)dwParam)->m_pListCtrl->GetItemCount())
		{
			int nItem=((CLocateDlg*)dwParam)->m_pListCtrl->GetNextItem(-1,LVNI_ALL);
			while (nItem!=-1)
			{
				CLocatedItem* pItem=(CLocatedItem*)((CLocateDlg*)dwParam)->m_pListCtrl->GetItemData(nItem);
				if (pItem!=NULL)
				{
					ASSERT(pItem->GetPathLen()<MAX_PATH);

					MemCopyW(szPath2,pItem->GetPath(),pItem->GetPathLen()+1);
					MakeLower(szPath2);
				
					
					if (wcscmp(szPath,szPath2)==0)
					{
						pLocater->IgnoreThisResult(bFolder);
						return TRUE; // Alreafy found
					}
					
				}
				nItem=((CLocateDlg*)dwParam)->m_pListCtrl->GetNextItem(nItem,LVNI_ALL);
			}
		}		

	}

	LV_ITEMW li;
	li.mask=LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM;
	li.iSubItem=0;
	li.iImage=I_IMAGECALLBACK;
	li.lParam=(LPARAM)new CLocatedItem;
	if (li.lParam==NULL)
		return FALSE;
	if (bFolder)
		((CLocatedItem*)li.lParam)->SetFolderW(pLocater);
	else
		((CLocatedItem*)li.lParam)->SetFileW(pLocater);

	li.pszText=LPSTR_TEXTCALLBACKW;

		// Check wheter file/folder is deleted
	if (((CLocateDlg*)dwParam)->GetExtraFlags()&efLVDontShowDeletedFiles)
	{
		if (bFolder)
		{
			if (!FileSystem::IsDirectory(((CLocatedItem*)li.lParam)->GetPath()))
			{
				delete (CLocatedItem*)li.lParam;
				pLocater->IgnoreThisResult(TRUE);
				return TRUE;
			}
		}
		else
		{
			if (!FileSystem::IsFile(((CLocatedItem*)li.lParam)->GetPath()))
			{
				delete (CLocatedItem*)li.lParam;
				pLocater->IgnoreThisResult(FALSE);
				return TRUE;
			}
		}
	}

	// To prevent drawing error
	DWORD dwResults=pLocater->GetNumberOfResults();
	
	if (dwResults%60==59)
		Sleep(((CLocateDlg*)dwParam)->m_WaitEvery60);
	else if (dwResults%30==29)
		Sleep(((CLocateDlg*)dwParam)->m_WaitEvery30);


	if (((CLocateDlg*)dwParam)->m_nSorting==BYTE(-1))
	{
		//li.iItem=((CLocateDlg*)dwParam)->m_pListCtrl->GetItemCount();
		if (((CLocateDlg*)dwParam)->GetFlags()&fgLVFoldersFirst && bFolder)
			li.iItem=pLocater->GetNumberOfFoundDirectories();
		else
			li.iItem=dwResults;
	}
	else
	{
		li.iItem=SortNewItem(((CLocateDlg*)dwParam)->m_pListCtrl,
			(CLocatedItem*)li.lParam,((CLocateDlg*)dwParam)->m_nSorting);
	}

	
	((CLocateDlg*)dwParam)->m_pListCtrl->InsertItem(&li);

	DbcDebugMessage("CLocateDlg::LocateFoundProc END");
	return TRUE;
}

BOOL CLocateDlg::StartLocateAnimation()
{
	EnterCriticalSection(&m_csLocateAnimBitmaps);
	if (m_pLocateAnimBitmaps!=NULL)
	{
		LeaveCriticalSection(&m_csLocateAnimBitmaps);
		return TRUE;
	}
	
		
	m_pLocateAnimBitmaps=new HICON[COUNT_LOCATEANIMATIONS];
	for (int i=0;i<COUNT_LOCATEANIMATIONS;i++)
	{
		m_pLocateAnimBitmaps[i]=(HICON)LoadImage(IDI_LANIM1+i,IMAGE_ICON,16,16,LR_DEFAULTCOLOR);
		DebugOpenGdiObject(m_pLocateAnimBitmaps[i]);
	}
	
	SetTimer(ID_LOCATEANIM,200);
	m_nCurLocateAnimBitmap=0;
	m_pStatusCtrl->SetText((LPCSTR)m_pLocateAnimBitmaps[0],STATUSBAR_LOCATEICON,SBT_OWNERDRAW);
	LeaveCriticalSection(&m_csLocateAnimBitmaps);
	return TRUE;
}

BOOL CLocateDlg::StopLocateAnimation()
{
	EnterCriticalSection(&m_csLocateAnimBitmaps);
	if (m_pLocateAnimBitmaps!=NULL)
	{
		KillTimer(ID_LOCATEANIM);
		for (int i=0;i<COUNT_LOCATEANIMATIONS;i++)
		{
			DebugCloseGdiObject(m_pLocateAnimBitmaps[i]);
			DestroyIcon(m_pLocateAnimBitmaps[i]);
		}
		delete[] m_pLocateAnimBitmaps;
		m_pLocateAnimBitmaps=NULL;
		LeaveCriticalSection(&m_csLocateAnimBitmaps);
		
		if (m_pStatusCtrl!=NULL)
			m_pStatusCtrl->SetText(STRNULL,STATUSBAR_LOCATEICON,SBT_OWNERDRAW);
		return TRUE;
	}
	LeaveCriticalSection(&m_csLocateAnimBitmaps);
	return TRUE;
}
	
BOOL CLocateDlg::StartUpdateAnimation()
{
	if (m_pStatusCtrl==NULL)
		return FALSE;

	EnterCriticalSection(&m_csUpdateAnimBitmaps);
	if (m_pUpdateAnimBitmaps!=NULL)
	{
		LeaveCriticalSection(&m_csUpdateAnimBitmaps);
		return TRUE;
	}
	
	m_pUpdateAnimBitmaps=new HICON[COUNT_UPDATEANIMATIONS];
	for (int i=0;i<COUNT_UPDATEANIMATIONS;i++)
	{
		m_pUpdateAnimBitmaps[i]=(HICON)LoadImage(IDI_UANIM1+i,IMAGE_ICON,16,16,LR_DEFAULTCOLOR);
		DebugOpenGdiObject(m_pUpdateAnimBitmaps[i]);
	}

	SetTimer(ID_UPDATEANIM,100);
	m_nCurUpdateAnimBitmap=0;
	m_pStatusCtrl->SetText((LPCSTR)m_pUpdateAnimBitmaps[0],STATUSBAR_UPDATEICON,SBT_OWNERDRAW);
	
	LeaveCriticalSection(&m_csUpdateAnimBitmaps);
	return TRUE;
}

BOOL CLocateDlg::StopUpdateAnimation()
{
	EnterCriticalSection(&m_csUpdateAnimBitmaps);
	if (m_pUpdateAnimBitmaps!=NULL)
	{
		KillTimer(ID_UPDATEANIM);
		for (int i=0;i<COUNT_LOCATEANIMATIONS;i++)
		{
			DebugCloseGdiObject(m_pUpdateAnimBitmaps[i]);
			DestroyIcon(m_pUpdateAnimBitmaps[i]);
		}
		delete[] m_pUpdateAnimBitmaps;
		m_pUpdateAnimBitmaps=NULL;
		LeaveCriticalSection(&m_csUpdateAnimBitmaps);
		if (m_pStatusCtrl!=NULL)
			m_pStatusCtrl->SetText(STRNULL,STATUSBAR_UPDATEICON,SBT_OWNERDRAW);
	
		return TRUE;
	}
	LeaveCriticalSection(&m_csUpdateAnimBitmaps);
	return TRUE;
}

BOOL CLocateDlg::IsDatabaseUsedInSearch(WORD wID) const
{
	for (int i=0;i<m_DatabasesUsedInSearch.GetSize();i++)
	{
		if (m_DatabasesUsedInSearch[i]==wID)
			return TRUE;
	}
	return FALSE;
}

////////////////////////////////////////////////////////////
// CLocateDlg - Background operations
////////////////////////////////////////////////////////////

void CLocateDlg::StartBackgroundOperations()
{
	DlgDebugMessage("StartBackgroundOperations BEGIN");
	if (m_pBackgroundUpdater==NULL)
	{
		m_pBackgroundUpdater=new CBackgroundUpdater(m_pListCtrl);
		m_pBackgroundUpdater->Start();
	}
	else
		m_pBackgroundUpdater->StopIgnoringItems();



	if (GetLocateDlg()->GetExtraFlags()&CLocateDlg::efEnableItemUpdating)
	{
		if (GetExtraFlags()&efEnableFSTracking)
		{
			if (m_pFileNotificationsThread==NULL)
				InterlockedExchangePointer((PVOID*)&m_pFileNotificationsThread,new CCheckFileNotificationsThread);
			
			m_pFileNotificationsThread->Start();
		}
		else if (m_pFileNotificationsThread!=NULL)
			m_pFileNotificationsThread->CouldStop();
	}

	DlgDebugMessage("StartBackgroundOperations END");
}

void CLocateDlg::ChangeBackgroundOperationsPriority(BOOL bLower)
{
	DebugFormatMessage("CLocateDlg::ChangeBackgroundOperationsPriority(bLower=%d) B=%X F=%X:  BEGIN",bLower,m_pBackgroundUpdater,m_pFileNotificationsThread);
	
	int nPriority=bLower?THREAD_PRIORITY_LOWEST:THREAD_PRIORITY_BELOW_NORMAL;

	if (m_pBackgroundUpdater!=NULL)
		SetThreadPriority(m_pBackgroundUpdater->m_hThread,nPriority);
	
	if (m_pFileNotificationsThread!=NULL)
		SetThreadPriority(m_pFileNotificationsThread->m_hThread,nPriority);


	DebugMessage("CLocateDlg::ChangeBackgroundOperationsPriority():  END");
}

BOOL CLocateDlg::CheckClipboard()
{
	//DebugMessage("CLocateDlg::CheckClipboard() BEGIN");
	
	if (m_pListCtrl==NULL)
		return TRUE;
	if (m_pListCtrl->GetItemCount()==0)
		return TRUE; // No need to do anything
	
	
	CArrayFAP<LPWSTR> aFiles;
	BYTE bIsCuttedFiles=FALSE;
	UINT nPreferredDropEffectFormat=RegisterClipboardFormat(CFSTR_PREFERREDDROPEFFECT);
	
	OpenClipboard();
	
	while (IsClipboardFormatAvailable(nPreferredDropEffectFormat))
	{
		HANDLE hData=GetClipboardData(nPreferredDropEffectFormat);
		if (hData==NULL)
			break;

		BYTE* pData=(BYTE*)GlobalLock(hData);
		if (pData==NULL)
			break;
        
		BYTE bDropEffect=*pData;
        GlobalUnlock(hData);        		

		if (!(bDropEffect&DROPEFFECT_MOVE))
			break;

		if (IsClipboardFormatAvailable(CF_HDROP))
		{
			HANDLE hData=GetClipboardData(CF_HDROP);
			if (hData!=NULL)
			{
				if (IsUnicodeSystem())
				{
					bIsCuttedFiles=TRUE;
					int last=DragQueryFileW((HDROP)hData,0xFFFFFFFF,NULL,0);
					for (int i=0;i<last;i++)
					{
						UINT nLen=DragQueryFileW((HDROP)hData,i,NULL,0);
						if (nLen>0)
						{
							LPWSTR szPath=new WCHAR[nLen+2];
							if (DragQueryFileW((HDROP)hData,i,szPath,nLen+2)>0)
								aFiles.Add(szPath);
							else
								delete[] szPath;
						}
					}
				}
				else
				{
					bIsCuttedFiles=TRUE;
					int last=DragQueryFile((HDROP)hData,0xFFFFFFFF,NULL,0);
					for (int i=0;i<last;i++)
					{
						char szPath[MAX_PATH];
						if (DragQueryFile((HDROP)hData,i,szPath,MAX_PATH)>0)
							aFiles.Add(alloccopyAtoW(szPath));
					}
				}
			}
		}
		break;
	}
	CloseClipboard();

	int nReDraws=0;
	
	if (bIsCuttedFiles)
	{
		int nItem=m_pListCtrl->GetNextItem(-1,LVNI_ALL);
		while (nItem!=-1)
		{
			CLocatedItem* pItem=(CLocatedItem*)m_pListCtrl->GetItemData(nItem);
			int i;
			for (i=0;i<aFiles.GetSize();i++)
			{
				if (_wcsicmp(aFiles[i],pItem->GetPath())==0)
				{
					pItem->AddAttribute(LITEMATTRIB_CUTTED);
					m_pListCtrl->RedrawItems(nItem,nItem);
					nReDraws++;
					break;
				}
			}
			if (i==aFiles.GetSize() && pItem->IsCutted())
			{
				pItem->RemoveAttribute(LITEMATTRIB_CUTTED);
				m_pListCtrl->RedrawItems(nItem,nItem);
				nReDraws++;
			}					
			nItem=m_pListCtrl->GetNextItem(nItem,LVNI_ALL);
		}
	}
	else
	{
		int nItem=m_pListCtrl->GetNextItem(-1,LVNI_ALL);
		while (nItem!=-1)
		{
			CLocatedItem* pItem=(CLocatedItem*)m_pListCtrl->GetItemData(nItem);
			if (pItem->IsCutted())
			{
				pItem->RemoveAttribute(LITEMATTRIB_CUTTED);
				m_pListCtrl->RedrawItems(nItem,nItem);
				nReDraws++;
			}
			nItem=m_pListCtrl->GetNextItem(nItem,LVNI_ALL);
		}
	}

	if (nReDraws)
		m_pListCtrl->UpdateWindow();

	aFiles.RemoveAll();
	return TRUE;
}


////////////////////////////////////////////////////////////
// CLocateDlg - Instant search related stuff
////////////////////////////////////////////////////////////

void CLocateDlg::OnFieldChange(DWORD dwWhatChanged)
{
	
	// Stop if IS disabled
	if (!IsInstantSearchingFlagSet(isEnable))
		return;
	
	// Stop if message should be ignored (e.g., during initialization)
	if (IsInstantSearchingFlagSet(isIgnoreChangeMessages))
		return;

	// If search while typing and under delay, cancel timer
	KillTimer(ID_INSTANTSEARCHTIMER);

	
	// Stop if going to searching string inside files and that is not desired
	if (IsInstantSearchingFlagSet(isDisableIfDataSearch) && m_AdvancedDlg.IsDlgButtonChecked(IDC_CONTAINDATACHECK))
		return;

	// Check if changes are given by command line
	if (dwWhatChanged&isByCommandLine && !(m_dwInstantFlags&isByCommandLine))
		return;

	// Stop if not the right place
	if (!(m_dwInstantFlags&(dwWhatChanged&isAllChanged)))
		return;

	
	AddInstantSearchingFlags(isRunning);

	if (m_dwInstantDelay>0)
		SetTimer(ID_INSTANTSEARCHTIMER,m_dwInstantDelay);
	else
		InstantSearch();
}

void CLocateDlg::InstantSearch()
{
	/////////////////////////////////////////////////////////////////////////////
	// STEP 1 INITIALIZATIONS: Stopping background operatins, clearing list, etc
	/////////////////////////////////////////////////////////////////////////////
	
	
	// Stop locating process if still active
	if (IsLocating())
	{
		m_pLocater->StopLocating();
		RemoveInstantSearchingFlags(isSearching);
	}

	// Disable FS tracking
	if (m_pFileNotificationsThread!=NULL)
		m_pFileNotificationsThread->Stop();
	
	// Tell backgroundupdater to clear the list and go waiting
	if (m_pBackgroundUpdater!=NULL)
	{
		m_pBackgroundUpdater->IgnoreItemsAndGoToSleep();

				
		DlgDebugMessage("IS: ignoring items");

		while (!m_pBackgroundUpdater->IsWaiting())
			Sleep(25);
	}


	
	// Clear tooltips
	if (m_pListTooltips!=NULL)
		DeleteTooltipTools();

	// Deleting previous items
	RemoveResultsFromList();
	DlgDebugMessage("IS: items removed");


	

	/////////////////////////////////////////////////////////////////////////////
	// STEP 2 RESOLVE PARAMETERS: Stopping background operatins, clearing list, etc
	/////////////////////////////////////////////////////////////////////////////
	
	CArrayFAP<LPWSTR> aExtensions,aDirectories,aNames;
	m_pLocater=ResolveParametersAndInitializeLocater(aExtensions,aDirectories,
		aNames,TRUE,FALSE);

	if (m_pLocater==NULL)
		return;
	
	// Set funtion pointers
	m_pLocater->SetFunctions(LocateInstantProc,LocateFoundProc,LocateFoundProcW,DWORD(this));

	
	/////////////////////////////////////////////////////////////////////////////
	// STEP 3 SET DIALOG, START BACKGROUND OPERATIONS AND START LOCATING
	/////////////////////////////////////////////////////////////////////////////
	
	
	// If dialog is not large mode, change it
	SetDialogMode(TRUE);

	// LocateFoundProc uses UpdateList
	StartBackgroundOperations();

	AddInstantSearchingFlags(isSearching);

	// Starting location
	m_pLocater->LocateFiles(TRUE,(LPCWSTR*)aNames.GetData(),aNames.GetSize(),
		(LPCWSTR*)aExtensions.GetData(),aExtensions.GetSize(),
		(LPCWSTR*)aDirectories.GetData(),aDirectories.GetSize());

	DlgDebugMessage("IS END");
}

void CLocateDlg::CancelInstantSearch()
{
	// If search while typing and under delay, cancel timer
	KillTimer(ID_INSTANTSEARCHTIMER);

	RemoveInstantSearchingFlags(isRunning);
}

////////////////////////////////////////////////////////////
// CLocateDlg - Misc helpers
////////////////////////////////////////////////////////////

BOOL CLocateDlg::ExecuteCommand(LPCWSTR szCommand,int nItem)
{
	if (szCommand==NULL)
		return TRUE;
	
	int nIndexToPercent=nIndexToPercent=(int)FirstCharIndex(szCommand,L'%');
	if (nIndexToPercent==-1)
	{
		// Just execute command
		PROCESS_INFORMATION pi;
		STARTUPINFO si; // Ansi and Unicode versions are same
		si.cb=sizeof(STARTUPINFO);
		si.lpReserved=NULL;
		si.cbReserved2=0;
		si.lpReserved2=NULL;
		si.lpDesktop=NULL;
		si.lpTitle=NULL;
		si.dwFlags=STARTF_USESHOWWINDOW;
		si.wShowWindow=SW_SHOWDEFAULT;
		
		if (IsUnicodeSystem())
		{
			if (!CreateProcessW(NULL,LPWSTR(szCommand),NULL,
				NULL,FALSE,CREATE_DEFAULT_ERROR_MODE|NORMAL_PRIORITY_CLASS,
				NULL,NULL,(STARTUPINFOW*)&si,&pi))
				return FALSE;
		}
		else
		{
			if (!CreateProcess(NULL,(LPSTR)(LPCSTR)W2A(szCommand),NULL,
				NULL,FALSE,CREATE_DEFAULT_ERROR_MODE|NORMAL_PRIORITY_CLASS,
				NULL,NULL,&si,&pi))
				return FALSE;
		}

		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		return TRUE;		
	}

	CLocateDlg* pLocateDlg=GetLocateDlg();
	if (pLocateDlg==NULL)
		return FALSE;

	int nItems;
	CAutoPtrA<CLocatedItem*> pItems=pLocateDlg->GetSelectedItems(nItems,nItem);
	
	for (int i=0;i<nItems;i++)
	{
		if (pItems[i]!=NULL)
		{
			int nIndex=nIndexToPercent;
			LPWSTR pCommand=(LPWSTR)szCommand;
		
			// Replace %d with item path
			do
			{
				int nLen;
				LPCWSTR pPath;
                switch (pCommand[nIndex+1])
				{
				case L'd':
				case L'D':
					nLen=(int)pItems[i]->GetPathLen();
					pPath=pItems[i]->GetPath();
					break;
				case L'p':
				case L'P':
					pPath=pItems[i]->GetParent();
					nLen=(int)istrlenw(pPath);					
					break;
				case L'f':
				case L'F':
					pPath=pItems[i]->GetName();
					nLen=(int)pItems[i]->GetNameLen();
					break;
				case L'n':
				case L'N':
					pPath=pItems[i]->GetName();
					nLen=pItems[i]->GetExtensionLength()>0?(int)pItems[i]->GetExtensionPos()-1:pItems[i]->GetNameLen();
					break;
				case L'e':
				case L'E':
					pPath=pItems[i]->GetExtension();
					nLen=(int)pItems[i]->GetExtensionLength();
					break;
				default:
					pPath=pCommand+nIndex+1;
					nLen=1;
					break;
				}



				UINT nCommandLen=(UINT)istrlenw(pCommand);
				LPWSTR pNewCommand=new WCHAR[nCommandLen-2+nLen+1];
				MemCopyW(pNewCommand,pCommand,nIndex);
				MemCopyW(pNewCommand+nIndex,pPath,nLen);
				MemCopyW(pNewCommand+nIndex+nLen,pCommand+nIndex+2,nCommandLen-nIndex-2+1);


				if (pCommand!=szCommand)
					delete[] pCommand;

				pCommand=pNewCommand;

				nIndex=NextCharIndex(pCommand,L'%',nIndex+nLen);
			}
			while (nIndex!=-1);
	
			// Execute command
			PROCESS_INFORMATION pi;
			STARTUPINFO si;// Ansi and Unicode versions are same
			si.cb=sizeof(STARTUPINFO);
			si.lpReserved=NULL;
			si.cbReserved2=0;
			si.lpReserved2=NULL;
			si.lpDesktop=NULL;
			si.lpTitle=NULL;
			si.dwFlags=STARTF_USESHOWWINDOW;
			si.wShowWindow=SW_SHOWDEFAULT;
			
			BOOL bRet=FALSE;
			if (IsUnicodeSystem())
			{
				bRet=CreateProcessW(NULL,pCommand,NULL,
					NULL,FALSE,CREATE_DEFAULT_ERROR_MODE|NORMAL_PRIORITY_CLASS,
					NULL,NULL,(STARTUPINFOW*)&si,&pi);
			}
			else 
			{
				bRet=CreateProcess(NULL,(LPSTR)(LPCSTR)W2A(pCommand),NULL,
					NULL,FALSE,CREATE_DEFAULT_ERROR_MODE|NORMAL_PRIORITY_CLASS,
					NULL,NULL,&si,&pi);
			}
			
			if (pCommand!=szCommand)
				delete[] pCommand;

			if (bRet)
			{
				CloseHandle(pi.hThread);
				CloseHandle(pi.hProcess);
			} else
				return FALSE;
		}
	}

	return TRUE;

}

IDropTarget* CLocateDlg::GetDropTarget(LPITEMIDLIST pidl) const
{
	LPITEMIDLIST pidlLast, pidlNext;
    USHORT cb;
    IShellFolder* psf;
    IDropTarget* pdt;
		
	
    // Split pidl into parent part and the last part
	pidlLast = pidl;
    while(pidlNext = (LPITEMIDLIST)((BYTE*)pidlLast + pidlLast->mkid.cb),pidlNext->mkid.cb)
		pidlLast = pidlNext;

    //  Temporarily split the pidl at the point we found.
    cb = pidlLast->mkid.cb;
    pidlLast->mkid.cb = 0;

    //  Bind to the folder part.  
    if (pidl == pidlLast) 
	{
        psf = m_pDesktopFolder;
        m_pDesktopFolder->AddRef();
    }
	else
	{
		if (FAILED(m_pDesktopFolder->BindToObject(pidl,NULL,IID_IShellFolder,(LPVOID *)&psf)))
		{
			// Restore the pidl before exiting
			pidlLast->mkid.cb = cb;
			return NULL;
		}
	}
    
    // Restore the pidl we temporarily edited.
    pidlLast->mkid.cb = cb;
    HRESULT hRes = psf->GetUIObjectOf(*this,1,(LPCITEMIDLIST*)&pidlLast,IID_IDropTarget,NULL,(LPVOID*)&pdt);
    psf->Release();
	
	if (FAILED(hRes))
		return NULL;
	return pdt;
}

UINT CLocateDlg::GetSimpleIDLsandParentForFiles(LPCWSTR* ppFiles,UINT nFiles,LPITEMIDLIST& rpParentIDL,LPITEMIDLIST*& rpSimpleIDLs)
{
	if (m_pDesktopFolder==NULL)
		return 0;

	LPITEMIDLIST* pFullIDLs=new LPITEMIDLIST[nFiles];
	
	UINT nItems=0;

	for (UINT i=0;i<nFiles;i++)
	{	
		if (SUCCEEDED(m_pDesktopFolder->ParseDisplayName(*this,NULL,(LPWSTR)ppFiles[i],NULL,&pFullIDLs[nItems],NULL)))
			nItems++;
	}

	ASSERT(nItems==nFiles);

	BOOL bRet=GetSimpleIDLsandParentfromIDLs(nItems,pFullIDLs,&rpParentIDL,rpSimpleIDLs);

	for (UINT  i=0;i<nItems;i++)
		CoTaskMemFree(pFullIDLs[i]);
	delete[] pFullIDLs;

	if (!bRet)
	{
		delete[] rpSimpleIDLs;	
		return 0;
	}
	
	return nItems;
}

BOOL CLocateDlg::GetSimpleIDLsandParentfromIDLs(int nItems,LPITEMIDLIST* pFullIDLs,LPITEMIDLIST* rpParentIDL,LPITEMIDLIST* rpSimpleIDLs,int* pParentIDLLevel)
{
	if (m_pDesktopFolder==NULL)
		return FALSE;


	int nEqualLevels=0;
	
	if (nItems==1)
	{
		// Handle 1 file separately

		LPITEMIDLIST pidlLast, pidlNext;
		
		// Split pidl into parent part and the last part
		for (pidlLast = pFullIDLs[0]; 
			pidlNext = (LPITEMIDLIST)((BYTE*)pidlLast + pidlLast->mkid.cb),pidlNext->mkid.cb;
			pidlLast = pidlNext) 
			nEqualLevels++;

		if (pFullIDLs[0]==pidlLast)
			return FALSE;

		// Copy parent IDL
		DWORD nLength=DWORD(LPBYTE(pidlLast)-LPBYTE(pFullIDLs[0]));
		*rpParentIDL=(LPITEMIDLIST)CoTaskMemAlloc(nLength+2);
		CopyMemory(*rpParentIDL,pFullIDLs[0],nLength);
		*((WORD*)(LPBYTE(*rpParentIDL)+nLength))=0;

	

		// Copy simple IDL
		rpSimpleIDLs[0]=(LPITEMIDLIST)CoTaskMemAlloc(pidlLast->mkid.cb+2);
		CopyMemory(rpSimpleIDLs[0],pidlLast,pidlLast->mkid.cb+2);

		if (pParentIDLLevel!=NULL)
			*pParentIDLLevel=nEqualLevels;
		return TRUE;
	}

	if (pFullIDLs[0]->mkid.cb==0)
	{
		// Empty IDL
		return FALSE;
	}


	
	// Pointers
	LPITEMIDLIST* pPtr=new LPITEMIDLIST[nItems];
	for (int i=0;i<nItems;i++)
		pPtr[i]=pFullIDLs[i];
	

	// Check highest parent level
	BOOL bAllEqual=TRUE;
	DWORD nParentLength=0;
	
	while (((LPITEMIDLIST)((BYTE*)pPtr[0] + pPtr[0]->mkid.cb))->mkid.cb) // Net the last
	{
		// Side of first ID of first item lsit
		int i;
		WORD cb0,cb,cbpre=pPtr[0]->mkid.cb;

		pPtr[0]=(LPITEMIDLIST)((BYTE*)pPtr[0] + pPtr[0]->mkid.cb);
		cb0=pPtr[0]->mkid.cb;
		pPtr[0]->mkid.cb=0;

		for (i=1;i<nItems;i++)
		{
			pPtr[i]=(LPITEMIDLIST)((BYTE*)pPtr[i] + pPtr[i]->mkid.cb);
			cb=pPtr[i]->mkid.cb;
			pPtr[i]->mkid.cb=0;

			
			HRESULT hRes=m_pDesktopFolder->CompareIDs(0,pFullIDLs[0],pFullIDLs[i]);
			pPtr[i]->mkid.cb=cb;

			if (FAILED(hRes) || HRESULT_CODE(hRes)!=0)
			{
				bAllEqual=FALSE;
				break;
			}

		}

		pPtr[0]->mkid.cb=cb0;

		if (!bAllEqual)
			break;

		nEqualLevels++;

		nParentLength+=cbpre;
		
	}

	delete[] pPtr;

	if (nEqualLevels==0)
		return FALSE;
	
	if (pParentIDLLevel!=NULL)
		*pParentIDLLevel=nEqualLevels;
	
	
	// Copy parent IDL
	*rpParentIDL=(LPITEMIDLIST)CoTaskMemAlloc(nParentLength+2);
	CopyMemory(*rpParentIDL,pFullIDLs[0],nParentLength);
	*((WORD*)(LPBYTE(*rpParentIDL)+nParentLength))=0;

	// Copy simple IDLs
	for (int i=0;i<nItems;i++)
	{
		DWORD nLength=ShellFunctions::GetIDListSize((LPITEMIDLIST)(LPBYTE(pFullIDLs[i])+nParentLength));

		rpSimpleIDLs[i]=(LPITEMIDLIST)CoTaskMemAlloc(nLength);
		CopyMemory(rpSimpleIDLs[i],LPBYTE(pFullIDLs[i])+nParentLength,nLength);
	}
	return TRUE;
}

IExtractImage* CLocateDlg::GetExtractImageInterface(LPCWSTR szFile) const
{
	if (m_pDesktopFolder==NULL)
		return NULL;


	// Initializing pDesktopFolder
	IShellFolder* pParentFolder;
	IExtractImage* pExtractImage=NULL;
			
	LPITEMIDLIST pParentIDList,pItemIDList;
				
	int nParentLen=LastCharIndex(szFile,L'\\');
	CStringW sParent(szFile,++nParentLen);

	// Getting ID list of parent
	
	if (!SUCCEEDED(m_pDesktopFolder->ParseDisplayName(*this,NULL,(LPOLESTR)(LPCWSTR)sParent,NULL,&pParentIDList,NULL)))
		return NULL;

	// Querying IShellFolder interface for parent
	HRESULT hRes=m_pDesktopFolder->BindToObject(pParentIDList,NULL,IID_IShellFolder,(void**)&pParentFolder);
	CoTaskMemFree(pParentIDList);

	if (SUCCEEDED(hRes))
	{
		hRes=pParentFolder->ParseDisplayName(*this,NULL,(LPOLESTR)(LPCWSTR)szFile+nParentLen,NULL,&pItemIDList,NULL);
		if (SUCCEEDED(hRes))
		{
			hRes=pParentFolder->GetUIObjectOf(*this,1,(LPCITEMIDLIST*)&pItemIDList,IID_IExtractImage,NULL,(void**)&pExtractImage);
			CoTaskMemFree(pItemIDList);

			if (!SUCCEEDED(hRes))
				pExtractImage=NULL;
		}
		
		pParentFolder->Release();
	}

	return pExtractImage;
}

HBITMAP CLocateDlg::CreateThumbnail(LPCWSTR szFile,SIZE* pDesiredSize,SIZE* pActualSize) const
{
	if (m_dwThumbnailFlags&CLocateDlg::tfVistaFeaturesAvailable)
	{
		// Using IIthumbnailProvider to extract bitmap
		CComPtr<IThumbnailProvider> pThumbnailProv=GetThumbnailProvider(szFile);
		if (pThumbnailProv!=NULL)
		{
			HBITMAP hBitmap;
			WTS_ALPHATYPE at;
			HRESULT hRes=pThumbnailProv->GetThumbnail(pDesiredSize->cx,&hBitmap,&at);
			if (SUCCEEDED(hRes))
			{

				// Load size
				BITMAP bi;
				GetObject(hBitmap,sizeof(BITMAP),&bi);
				
				if (bi.bmWidth>pDesiredSize->cx || bi.bmHeight>pDesiredSize->cy)
				{
					// Image extractor does not handle size correctly
					HBITMAP hScaledBitmap=ScaleImage(hBitmap,pDesiredSize->cx,pDesiredSize->cy);
					if (hScaledBitmap!=NULL)
					{
						// Read dimensions again
						GetObject(hBitmap,sizeof(BITMAP),&bi);
						DeleteObject(hBitmap);
						hBitmap=hScaledBitmap;
					}
				}

				if (pActualSize!=NULL)
				{
					pActualSize->cx=bi.bmWidth;
					pActualSize->cy=bi.bmHeight;
				}

				return hBitmap;
			}
		}
	}

	// Extracting thumbnail icon
	CComPtr<IExtractImage> pExtractImage=GetExtractImageInterface(szFile);

	if (pExtractImage!=NULL)
	{
		WCHAR szPath[MAX_PATH];
		DWORD dwPriority,dwFlags=0;

		HRESULT hRes=pExtractImage->GetLocation(szPath,MAX_PATH,&dwPriority,pDesiredSize,32,&dwFlags);

		if (SUCCEEDED(hRes))
		{
			HBITMAP hBitmap;
			hRes=pExtractImage->Extract((HBITMAP*)&hBitmap);
			if (SUCCEEDED(hRes))
			{
				// Load size
				BITMAP bi;
				GetObject(hBitmap,sizeof(BITMAP),&bi);
				
				if (bi.bmWidth>pDesiredSize->cx || bi.bmHeight>pDesiredSize->cy)
				{
					// Image extractor does not handle size correctly
					HBITMAP hScaledBitmap=ScaleImage(hBitmap,pDesiredSize->cx,pDesiredSize->cy);
					if (hScaledBitmap!=NULL)
					{
						// Read dimensions again
						GetObject(hBitmap,sizeof(BITMAP),&bi);
						DeleteObject(hBitmap);
						hBitmap=hScaledBitmap;
					}
				}
				
				if (pActualSize!=NULL)
				{
					pActualSize->cx=bi.bmWidth;
					pActualSize->cy=bi.bmHeight;
				}
	
				return hBitmap;
			}
		}
	}

	return NULL;	
}


#undef DEFINE_GUID
#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    EXTERN_C const GUID DECLSPEC_SELECTANY name \
            = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }

DEFINE_GUID(BHID_ThumbnailHandler, 0x7b2e650a, 0x8e20, 0x4f4a, 0xb0, 0x9e, 0x65, 0x97, 0xaf, 0xc7, 0x2f, 0xb0);
DEFINE_GUID(IID_IThumbnailProvider,0xe357fccd, 0xa995, 0x4576, 0xb0, 0x1f, 0x23, 0x46, 0x30, 0x15, 0x4e, 0x96);

IThumbnailProvider* CLocateDlg::GetThumbnailProvider(LPCWSTR szFile) const
{
	IShellItem* pFileItem;

	HRESULT(STDAPICALLTYPE* pSHCreateItemFromParsingName)(PCWSTR,IBindCtx*,REFIID,void **)=
		(HRESULT(STDAPICALLTYPE*)(PCWSTR,IBindCtx*,REFIID,void **))GetProcAddress(GetModuleHandle("shell32.dll"),"SHCreateItemFromParsingName");

	if (pSHCreateItemFromParsingName==NULL)
		return NULL;
	
	HRESULT hRes=pSHCreateItemFromParsingName(szFile,NULL,IID_IShellItem,(void**)&pFileItem);
	if (SUCCEEDED(hRes))
	{
		IThumbnailProvider* pThumbProv;
		hRes=pFileItem->BindToHandler(NULL,BHID_ThumbnailHandler,IID_IThumbnailProvider,(void**)&pThumbProv);
		pFileItem->Release();


		if (SUCCEEDED(hRes))
			return pThumbProv;
	}	

	return NULL;
}

////////////////////////////////////////////////////////////
// CLocateDlg - Debug related stuff
////////////////////////////////////////////////////////////

// Window showing background statistics
#ifdef _DEBUG
LRESULT CALLBACK CLocateDlg::DebugWindowProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		::CreateWindow("EDIT","",WS_VISIBLE|WS_CHILD|ES_AUTOHSCROLL|ES_AUTOVSCROLL|ES_MULTILINE,0,0,100,100,
			hwnd,(HMENU)100,GetInstanceHandle(),NULL);
		::SetTimer(hwnd,1,500,NULL);
		return 0;
	case WM_SIZE:
		if (wParam==SIZE_RESTORED || wParam==0)
		{
			HWND hEdit=::GetDlgItem(hwnd,100);
			::SetWindowPos(hEdit,NULL,0,0,LOWORD(lParam),HIWORD(lParam),SWP_NOZORDER);

		}
		break;
	case WM_CLOSE:
		::KillTimer(hwnd,1);
		::DestroyWindow(hwnd);
		break;
	case WM_TIMER:
	{
		//int nCPU=int(100*GetCpuTime());
		CString str;
		CLocateDlg* pDlg=GetLocateDlg();
		if (pDlg->m_pBackgroundUpdater!=NULL)
		{
			str << "Background updated running, isWaiting=" << (int) pDlg->m_pBackgroundUpdater->IsWaiting();
			str << " items=" << (DWORD) pDlg->m_pBackgroundUpdater->GetUpdateListSize();
			str.SetBase(16);
			str << " hThread=0x" << (DWORD) pDlg->m_pBackgroundUpdater->m_hThread;
		}
		else
			str << "Background updater is not running";
		
		if (pDlg->m_pFileNotificationsThread!=NULL)
		{
			str << "\r\nFileNotfications is running";
			if (pDlg->m_pFileNotificationsThread->m_pReadDirectoryChangesW==NULL)
				str << " using old method";
			str << " hThread=0x" << (DWORD) pDlg->m_pFileNotificationsThread->m_hThread;
		}
		else
			str << "\r\nFileNotifications is not running";

		if (GetTrayIconWnd()->m_pCpuUsage!=NULL)
		{
			str.SetBase(10);
			str << "\r\nCpu time: " << GetTrayIconWnd()->m_pCpuUsage->GetCpuUsage();
		}

		::SetDlgItemText(hwnd,100,LPCSTR(str));
		break;
	}
	};
	return DefWindowProc(hwnd,uMsg,wParam,lParam);
}


#endif




////////////////////////////////////////////////////////////
// CLocateDlg::CSavePresetDlg
////////////////////////////////////////////////////////////


void CLocateDlg::CSavePresetDlg::OnOK()
{
	GetDlgItemText(IDC_EDIT,m_sReturnedPreset);

	if (m_sReturnedPreset.IsEmpty())
	{
		ShowErrorMessage(IDS_PRESETNAMENOTVALID,IDS_PRESETSAVETITLE,MB_OK|MB_ICONEXCLAMATION);
		SetFocus(IDC_EDIT);
		return;
	}

	DWORD dwID=CLocateDlg::CheckExistenceOfPreset(m_sReturnedPreset,NULL);
	if (dwID!=DWORD(-1) && SendDlgItemMessage(IDC_EDIT,CB_GETCURSEL)==CB_ERR)
	{
		CStringW msg;
		msg.Format(IDS_OVERWRITEPRESET,LPCWSTR(m_sReturnedPreset));

		if (MessageBox(msg,ID2W(IDS_PRESETSAVETITLE),MB_YESNO|MB_ICONQUESTION)==IDNO)
		{
			SetFocus(IDC_EDIT);
			return;
		}
	}
		
	EndDialog(1);
}

BOOL CLocateDlg::CSavePresetDlg::OnInitDialog(HWND hwndFocus)
{
	::CSavePresetDlg::OnInitDialog(hwndFocus);
	
	CString Path;
	CRegKey2 RegKey;
	CRegKey PresetKey;
	CComboBox Combo(GetDlgItem(IDC_EDIT));
	char szBuffer[30];

	if (RegKey.OpenKey(HKCU,"\\Dialogs\\SearchPresets",CRegKey::openExist|CRegKey::samRead)!=ERROR_SUCCESS)
		return FALSE;
	
	
	
	for (int nPreset=0;nPreset<1000;nPreset++)
	{
		StringCbPrintf(szBuffer,30,"Preset %03d",nPreset);

		if (PresetKey.OpenKey(RegKey,szBuffer,CRegKey::openExist|CRegKey::samRead)!=ERROR_SUCCESS)
			break;

		CStringW sCurrentName;
		PresetKey.QueryValue(L"",sCurrentName);
		Combo.AddString(sCurrentName);

		PresetKey.CloseKey();
	}	
	return FALSE;
}



