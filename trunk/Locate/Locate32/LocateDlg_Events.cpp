/* Locate32 - Copyright (c) 1997-2010 Janne Huttunen */

#include <HFCLib.h>
#include "Locate32.h"

#include <commoncontrols.h>

////////////////////////////////////////////////////////////
// CLocateDlg - Virtual message handlers
////////////////////////////////////////////////////////////


void CLocateDlg::OnActivate(WORD fActive,BOOL fMinimized,HWND hwnd)
{
	if (fActive!=WA_CLICKACTIVE || fMinimized || m_pListCtrl==NULL)
		return;

	if (GetFlags()&fgLVAllowInPlaceRenaming)
	{
		// When Locate32 dialog is activate by a mouseclieck, we check if 
		// an item in the result is clicked. If so, we clear its "Selected"
		// state to prevent the item be edited
		POINT ptCursor;
		GetCursorPos(&ptCursor);
		m_pListCtrl->ScreenToClient(&ptCursor);
		UINT uiFlags;
		int nItem=m_pListCtrl->HitTest(ptCursor,&uiFlags);
		if (nItem!=-1 && m_pListCtrl->GetItemState(nItem,LVIS_SELECTED))
			m_pListCtrl->SetItemState(nItem,0,LVIS_SELECTED);
	}	
}
	
void CLocateDlg::OnActivateApp(BOOL fActive,DWORD dwThreadID)
{
	if (!fActive)
	{
		// If list control is still in label edit mode,
		// the result list should got focus next time app is activated
		if (GetExtraFlags()&efLVRenamingActivated)
		{
			AddExtraFlags(efFocusToResultListWhenAppActivated);
			RemoveExtraFlags(efLVRenamingActivated);
		}

		// Hide tooltip if shown
		if (m_pListTooltips!=NULL)
			m_pListTooltips->Pop();

	}
	else
	{
		if (GetExtraFlags()&efFocusToResultListWhenAppActivated)
		{
			RemoveExtraFlags(efFocusToResultListWhenAppActivated);

			// Give focus for the result list
			if (m_pListCtrl!=NULL && GetFocus()!=*m_pListCtrl)
				m_pListCtrl->SetFocus();

			return;
		}

	}

	CDialog::OnActivateApp(fActive,dwThreadID);
}


void CLocateDlg::OnChangeCbChain(HWND hWndRemove,HWND hWndAfter)
{
	CDialog::OnChangeCbChain(hWndRemove,hWndAfter);
	if (m_hNextClipboardViewer==hWndRemove)
		m_hNextClipboardViewer=hWndAfter;
	if (m_hNextClipboardViewer!=NULL)
		::SendMessage(m_hNextClipboardViewer,WM_CHANGECBCHAIN,(WPARAM)hWndRemove,(LPARAM)hWndAfter);
}
	

BOOL CLocateDlg::OnClose()
{
	if (GetFlags()&fgDialogCloseMinimizesDialog)
	{
		ShowWindow(swMinimize);
		return 1;
	}

	CDialog::OnClose();
	DestroyWindow();
	return 1;
}


BOOL CLocateDlg::OnCommand(WORD wID,WORD wNotifyCode,HWND hControl)
{
	DebugFormatMessage("CDialog::OnCommand wID=%d",wID);

	if (HandleContextMenuCommand(wID))
		return 0;

	switch(wID)
	{
	case IDC_FILELIST:
		if (hControl==NULL && wNotifyCode==1 && GetFlags()&fgLargeMode) // Accelerator
			SetFocus(IDC_FILELIST);
		break;
	case IDC_OK:
		if (::IsWindowEnabled(GetDlgItem(IDC_OK)))
			OnOk(hControl==NULL && wNotifyCode==1,FALSE);
		break;
	case IDM_FINDUSINGDBS:
		if (::IsWindowEnabled(GetDlgItem(IDC_OK)))
			OnOk(hControl==NULL && wNotifyCode==1,TRUE);
		break;		
	case IDC_STOP:
		if (::IsWindowEnabled(GetDlgItem(IDC_STOP)))
			OnStop();
		break;
	case IDC_NEWSEARCH:
		if (::IsWindowEnabled(GetDlgItem(IDC_NEWSEARCH)))
			OnNewSearch();
		break;
	case IDM_ABOUT:
		{
			CAboutDlg About;
			About.DoModal(*this);
		}
		break;
	case IDM_HELPTOPICS:
		HtmlHelp(HH_DISPLAY_TOPIC,0);
		break;
	case IDM_EXTRALARGEICONS:
		SetListType(ltExtraLargeIcons);
		break;
	case IDM_LARGEICONS:
		SetListType(ltLargeIcons);
		break;
	case IDM_MEDIUMICONS:
		SetListType(ltMediumIcons);
		break;
	case IDM_SMALLICONS:
		SetListType(ltSmallIcons);
		break;
	case IDM_LIST:
		SetListType(ltList);
		break;
	case IDM_DETAILS:
		SetListType(ltDetails);
		break;
	case IDM_ARRANGENAME:
		SortItems(Name);
		break;
	case IDM_ARRANGEFOLDER:
		SortItems(InFolder);
		break;
	case IDM_ARRANGETYPE:
		SortItems(FileType);
		break;
	case IDM_ARRANGESIZE:
		SortItems(FileSize);
		break;
	case IDM_ARRANGEDATE:
		SortItems(DateModified);
		break;
	case IDM_ARRANGEEXTENSION:
		SortItems(Extension);
		break;
	case IDM_AUTOARRANGE:
		OnAutoArrange();
		break;
	case IDM_ALIGNTOGRID:
		OnAlignToGrid();
		break;
	case IDM_REFRESH:
		OnRefresh();
		break;
	case IDM_SETTINGS:
		OnSettings();
		break;
	case IDM_SETTOOL:
		OnSettingsTool();
		break;
	case IDM_DELETEPRIVATEDATA:
		OnDeletePrivateData();
		break;
	case IDM_SELECTDETAILS:
		OnSelectDetails();
		break;
	case IDM_GLOBALUPDATEDB:
	case IDM_UPDATEDATABASES:
	case IDM_STOPUPDATING:
		//CTrayIconWnd handles these
		GetTrayIconWnd()->PostMessage(WM_COMMAND,MAKEWPARAM(wID,wNotifyCode),LPARAM(hControl));
		break;
	case IDC_PRESETS:
		return !OnPresets();
	case IDM_DATABASEINFO:
		{
			CDatabaseInfos info(*this);
			info.DoModal();
			break;
		}
	case IDM_CLOSE:
		DestroyWindow();
		break;
	case IDM_EXIT:
		GetTrayIconWnd()->PostMessage(WM_COMMAND,IDM_EXIT,NULL);
		break;
	case IDC_SELECTALLKEY:
		if (GetFocus()!=GetDlgItem(IDC_FILELIST))
			break;
	case IDM_EDIT_SELECTALL:
		OnSelectAll();
		break;
	case IDM_EDIT_INVERTSELECTION:
		OnInvertSelection();
		break;
	case IDM_LINEUPICONS:
		m_pListCtrl->Arrange(LVA_SNAPTOGRID);
		break;
	case IDC_NAME:
	case IDC_TYPE:
	case IDC_LOOKIN:
	case IDC_BROWSE:
	case IDC_NOSUBDIRECTORIES:
		// This is to ensure that these conrols get focus e.g. when alt+n is pressed
		return (BOOL)m_NameDlg.SendMessage(WM_COMMAND,MAKEWPARAM(wID,wNotifyCode),(LPARAM)hControl);
	case IDC_CHECKMINIMUMSIZE:
	case IDC_MINIMUMSIZE:
	case IDC_MINSIZETYPE:
	case IDC_CHECKMAXIMUMSIZE:
	case IDC_MAXIMUMSIZE:
	case IDC_MAXSIZETYPE:
	case IDC_CHECKMINDATE:
	case IDC_MINDATE:
	case IDC_MINDATEMODE:
	case IDC_MINTYPE:
	case IDC_CHECKMAXDATE:
	case IDC_MAXDATE:
	case IDC_MAXDATEMODE:
	case IDC_MAXTYPE:
		// This is to ensure that these conrols get focus e.g. when alt+n is pressed
		return (BOOL)m_SizeDateDlg.SendMessage(WM_COMMAND,MAKEWPARAM(wID,wNotifyCode),(LPARAM)hControl);
	case IDC_CHECK:
	case IDC_MATCHWHOLENAME:
	case IDC_MATCHCASE:
	case IDC_FILETYPE:
	case IDC_CONTAINDATACHECK:
	case IDC_CONTAINDATA:
	case IDC_DATAMATCHCASE:
	case IDC_REPLACESPACES:
	case IDC_USEWHOLEPATH:
		// This is to ensure that these conrols get focus e.g. when alt+n is pressed
		return (BOOL)m_AdvancedDlg.SendMessage(WM_COMMAND,MAKEWPARAM(wID,wNotifyCode),(LPARAM)hControl);
	case IDC_NEXTCONTROL:
		DefDlgProc(*this,WM_NEXTDLGCTL,FALSE,0);
		//::SetFocus(GetNextDlgTabItem(GetFocus(),FALSE));
		break;
	case IDC_PREVCONTROL:
		DefDlgProc(*this,WM_NEXTDLGCTL,TRUE,0);
		//::SetFocus(GetNextDlgTabItem(GetFocus(),TRUE));
		break;
	case IDM_SAVERESULT:
	case IDM_COPYDATATOCB:
		OnSaveResults(wID==IDM_COPYDATATOCB);
		break;
	case IDM_PROPERTIES:
		OnProperties();
		break;
	case IDM_DEFOPEN:
		OnExecuteFile(NULL);
		break;
	case IDM_LOOKINNEWSELECTION:
	case IDM_LOOKINREMOVESELECTION:
	case IDM_LOOKINNEXTSELECTION:
	case IDM_LOOKINPREVSELECTION:
		return m_NameDlg.OnCommand(wID,wNotifyCode,hControl);
	default:
		// IDM_ should be in descending order
		if (wID>=IDM_DEFSHORTCUTITEM && wID<IDM_DEFSHORTCUTITEM+m_aActiveShortcuts.GetSize())
		{
			DebugFormatMessage("CDialog::OnCommand going to check keyboard shortcuts, wID=%d",wID);

			CShortcut** pShortcutList=m_aActiveShortcuts[wID-IDM_DEFSHORTCUTITEM];
			BOOL bSendBackToControl=TRUE;

			while (*pShortcutList!=NULL)
			{
				// Checking Win key state
				if ((GetKeyState(VK_LWIN)|GetKeyState(VK_RWIN)) & 0x8000)
				{
					if ((*pShortcutList)->m_bModifiers&CShortcut::ModifierWin)
					{
						if ((*pShortcutList)->IsWhenAndWhereSatisfied(*this))
						{
							(*pShortcutList)->ExecuteAction();
							bSendBackToControl=FALSE;
						}
					}
				}
				else if (!((*pShortcutList)->m_bModifiers&CShortcut::ModifierWin))
				{
					if ((*pShortcutList)->IsWhenAndWhereSatisfied(*this))
					{
						(*pShortcutList)->ExecuteAction();
						bSendBackToControl=FALSE;
					}
				}

				pShortcutList++;
			}


			if (bSendBackToControl && *m_aActiveShortcuts[wID-IDM_DEFSHORTCUTITEM]!=NULL)				
			{
				// Shortcut was not executed, send message to control
				(*m_aActiveShortcuts[wID-IDM_DEFSHORTCUTITEM])->SendEventBackToControl();
			}
			return 0;
		}
		else if (wID>=IDM_DEFUPDATEDBITEM && wID<IDM_DEFUPDATEDBITEM+1000)
			return (BOOL)GetTrayIconWnd()->SendMessage(WM_COMMAND,MAKEWPARAM(wID,wNotifyCode),LPARAM(hControl));
		else 
			return CDialog::OnCommand(wID,wNotifyCode,hControl);
	}
	return 0;
}

BOOL CLocateDlg::HandleContextMenuCommand(WORD wID)
{
	DebugFormatMessage("CLocateDlg::HandleContextMenuCommand(%d)",wID);

	// Check if wID corresponds to Send to menu item
	if (HandleSendToCommand(wID))
		return TRUE;
	
	// Check if wID corresponds to shell context menu command
	if (HandleShellCommands(wID))
		return TRUE;

	
	switch(wID)
	{
	default:
		return FALSE;
	case IDM_CUT:
	case IDM_COPY:
		OnCopy(wID==IDM_CUT);
		break;
	case IDM_PASTE:
		{
			HWND hFocus=GetFocus();
			char szClass[100];
			::GetClassName(hFocus,szClass,100);
			if (_stricmp(szClass,"EDIT")==0)
				::SendMessage(hFocus,WM_PASTE,0,0);
			break;
		}
	case IDM_OPENCONTAININGFOLDER:
		OpenSelectedFolder(TRUE,-1,m_pActiveContextMenu!=NULL?m_pActiveContextMenu->bForParents:FALSE);
		break;
	case IDM_CREATESHORTCUT:
		OnCreateShortcut();
		break;
	case IDM_DELETE:
		OnDelete();
		break;
	case IDM_RENAME:
		OnRenameFile();
		break;
	case IDM_REMOVEFROMTHISLIST:
		OnRemoveFromThisList();
		break;
	case IDM_COPYPATHTOCB:
		OnCopyPathToClipboard(FALSE);
		break;
	case IDM_COPYSHORTPATHTOCB:
		OnCopyPathToClipboard(TRUE);
		break;
	case IDM_COPYDATATOCB:
		OnSaveResults(TRUE);
		break;
	case IDM_CHANGECASE:
		OnChangeFileNameCase();
		break;
	case IDM_FORCEUPDATE:
		OnUpdateLocatedItem();
		break;
	case IDM_COMPUTEMD5SUM:
		OnComputeMD5Sums(FALSE);
		break;
	case IDM_MD5SUMSFORSAMESIZEFILES:
		OnComputeMD5Sums(TRUE);
		break;
	case IDM_COPYMD5SUMTOCLIPBOARD:
		OnCopyMD5SumsToClipboard();
		break;
	case IDM_SHOWFILEINFORMATION:
		OnShowFileInformation();
		break;
	case IDM_REMOVEDELETEDFILES:
		OnRemoveDeletedFiles();
		break;
	case IDM_CHANGEFILENAME:
		OnChangeFileName();
		break;
	case IDM_UPDATEDATABASESOFSELECTEDFILES:
		OnUpdateDatabasesOfSelectedFiles();
		break;
	case IDM_PROPERTIES:
		OnProperties();
		break;
	case IDM_DEFOPEN:
		if (m_pActiveContextMenu!=NULL && m_pActiveContextMenu->bForParents)
			OpenSelectedFolder(TRUE,-1);
		else 
			OnExecuteFile(NULL);
		break;
	}
	
	return TRUE;
}


void CLocateDlg::OnContextMenu(HWND hWnd,CPoint& pos)
{
	CRect rect;
		
	if (DWORD(pos.x)==0xffff && DWORD(pos.y)==0xffff)
	{
		// Key
		GetCursorPos(&pos);

		int nSelectedItems;
		CLocatedItem** pSelectedItems=GetSelectedItems(nSelectedItems);
		if (nSelectedItems>0)
		{
			ClearMenuVariables();

			if (CreateFileContextMenu(NULL,pSelectedItems,nSelectedItems))
			{
				TrackPopupMenu(*m_pActiveContextMenu,TPM_LEFTALIGN|TPM_RIGHTBUTTON,
					pos.x,pos.y,0,*this,NULL);	
			}
		}
		else
		{	
			TrackPopupMenu(m_Menu.GetSubMenu(SUBMENU_CONTEXTMENUNOITEMS),TPM_LEFTALIGN|TPM_RIGHTBUTTON,
				pos.x,pos.y,0,*this,NULL);	
		}

		delete pSelectedItems;

		return;			
	}


	m_pListCtrl->GetWindowRect(&rect);
	//Checking whether mouse is in the list control's area
	if (!rect.IsPtInRect(pos))
	{
		CDialog::OnContextMenu(hWnd,pos);
		return;
	}

	

	HWND hHeader=m_pListCtrl->GetHeader();
	::GetWindowRect(hHeader,&rect);
	if (rect.IsPtInRect(pos))
	{
		// Show context menu for header
		
		CMenu ColMenu(m_pListCtrl->CreateColumnSelectionMenu(1));
		CStringW text(IDS_SELECTDETAILS);
		
		MENUITEMINFOW mii;
		mii.cbSize=sizeof(MENUITEMINFOW);
		// Inserting separator
		mii.fMask=MIIM_TYPE;
		mii.fType=MFT_SEPARATOR;
		ColMenu.InsertMenu(WORD(-1),FALSE,&mii);
		mii.fMask=MIIM_ID|MIIM_TYPE;
		mii.fType=MFT_STRING;
		mii.dwTypeData=text.GetBuffer();
		mii.wID=IDM_SELECTDETAILS;
		ColMenu.InsertMenu(WORD(-1),FALSE,&mii);
		
		int iID=TrackPopupMenu(ColMenu,TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD,pos.x,pos.y,0,*this,NULL);	
		if (iID==IDM_SELECTDETAILS)
			OnSelectDetails();
		else if (iID!=0)
			m_pListCtrl->ColumnSelectionMenuProc(iID,1);
		
		ColMenu.DestroyMenu();
	}
	else
	{
		LVHITTESTINFO ht;
		ht.pt=pos;
		m_pListCtrl->ScreenToClient(&ht.pt);
		
		if (m_pListCtrl->SubItemHitTest(&ht)==-1)
		{
			// Not any file item
			TrackPopupMenu(m_Menu.GetSubMenu(SUBMENU_CONTEXTMENUNOITEMS),TPM_LEFTALIGN|TPM_RIGHTBUTTON,pos.x,pos.y,0,*this,NULL);	
		}
		CDialog::OnContextMenu(hWnd,pos);
	}
}	


void CLocateDlg::OnDestroy()
{
	DebugMessage("CLocateDlg::OnDestroy() BEGIN");
	
	CDialog::OnDestroy();
	
	// Ensure that locating process is terminated
	if (IsLocating())
	{
		m_pLocater->StopLocating();
		DebugFormatMessage("CLocateDlg::OnDestroy(): m_pLocater=%X",(DWORD)m_pLocater);
	}

	// Ensuring that update animations are not on
	StopUpdateAnimation();
	
	// Stop background operations
	if (m_pFileNotificationsThread!=NULL)
		m_pFileNotificationsThread->Stop();
	if (m_pBackgroundUpdater!=NULL)
		m_pBackgroundUpdater->Stop();

	ASSERT(m_pFileNotificationsThread==NULL);
	ASSERT(m_pBackgroundUpdater==NULL);

	ChangeClipboardChain(*this,m_hNextClipboardViewer);
	

	// Clearing accelerators
	ClearShortcuts();
	SaveResultlistActions();
	ClearResultlistActions();


	SaveDialogTexts();
	m_NameDlg.DestroyWindow();
	m_SizeDateDlg.DestroyWindow();
	m_AdvancedDlg.DestroyWindow();
	
	
	SaveRegistry();
	ISDLGTHREADOK

	// Freeing target paths in dwItemData
	ClearMenuVariables();
	HMENU hOldMenu=GetMenu();
	::DestroyMenu(hOldMenu);
	m_Menu.DestroyMenu(); // Destroy submenu

	// Releasing drop target
	RevokeDragDrop(*m_pListCtrl);
	m_pDropTarget->Release(); //This deletes class
	OleUninitialize();


	// Delete window classes
	// Tab control and statusbar
	if (m_pTabCtrl!=NULL)
	{
		delete m_pTabCtrl;
		m_pTabCtrl=NULL;
	}
	if (m_pStatusCtrl!=NULL)
	{
		delete m_pStatusCtrl;
		m_pStatusCtrl=NULL;
	}

	// List control and related stuff
	if (m_pListCtrl!=NULL)
	{
		if ((m_pListCtrl->GetStyle() & LVS_TYPEMASK)==LVS_REPORT || 
			(m_dwExtraFlags&efLVHeaderInAllViews && GetSystemFeaturesFlag()&efWinVista))
			m_pListCtrl->SaveColumnsState(HKCU,CRegKey2::GetCommonKey()+"\\General","ListWidths");
		
		// Free list font
		HFONT hFont=m_pListCtrl->GetFont();
		if (hFont!=m_hDialogFont)
		{
			DebugCloseGdiObject(hFont);
			DeleteObject(hFont);
		}
		
		delete m_pListCtrl;
		m_pListCtrl=NULL;
	}
	if (m_pListTooltips!=NULL)
	{
		m_pListTooltips->DestroyToolTipCtrl();
		delete m_pListTooltips;
		m_pListTooltips=NULL;
	}
	if (m_pImageHandler!=NULL)
	{
		delete m_pImageHandler;
		m_pImageHandler=NULL;
	}
	
	if (m_pSystemImageList!=NULL && m_dwThumbnailFlags&tfSystemImageListIsInterface)
	{
		m_pSystemImageList->Release();
		m_pSystemImageList=NULL;
	}



	// Buffers
	FreeBuffers();

	// SendTo list related stuff
	if (m_hSendToListFont!=NULL)
	{
		DebugCloseGdiObject(m_hSendToListFont);
		DeleteObject(m_hSendToListFont);
		m_hSendToListFont=NULL;
	}

	// Free icons
	if (m_hLargeDialogIcon!=NULL)
	{
		DestroyIcon(m_hLargeDialogIcon);
		DebugCloseGdiObject(m_hLargeDialogIcon);
		m_hLargeDialogIcon=NULL;
	}
	if (m_hSmallDialogIcon!=NULL)
	{
		DestroyIcon(m_hSmallDialogIcon);
		DebugCloseGdiObject(m_hSmallDialogIcon);
		m_hSmallDialogIcon=NULL;
	}


	if (!(m_dwFlags&fgDialogLeaveLocateBackground))
		GetTrayIconWnd()->PostMessage(WM_COMMAND,MAKEWPARAM(IDC_COMEWITHME,0));
	PostQuitMessage(0);

	DebugMessage("CLocateDlg::OnDestroy() END");
}


void CLocateDlg::OnDrawClipboard()
{
	::SendMessage(m_hNextClipboardViewer,WM_DRAWCLIPBOARD,0,0);
	CheckClipboard();
	CDialog::OnDrawClipboard();
}


void CLocateDlg::OnDrawItem(UINT idCtl,LPDRAWITEMSTRUCT lpdis)
{
	CDialog::OnDrawItem(idCtl,lpdis);
	
	switch (idCtl)
	{
	case 0:
		{
			// This is IContextMenu item
			if (m_pActiveContextMenu!=NULL && lpdis->itemID<IDM_DEFSENDTOITEM)
				return;

			// SendTo menu item
			CDC dc(lpdis->hDC);
			COLORREF backcolor,forecolor;
			
			if (lpdis->itemState&ODS_SELECTED)
			{
				forecolor=GetSysColor(COLOR_HIGHLIGHTTEXT);
				backcolor=GetSysColor(COLOR_HIGHLIGHT);
			}
			else
			{
				forecolor=GetSysColor(COLOR_MENUTEXT);
				backcolor=GetSysColor(COLOR_MENU);
			}

			if (lpdis->itemAction&ODA_SELECT || lpdis->itemAction&ODA_DRAWENTIRE)
			{
				HPEN oldpen;
				HBRUSH oldbrush;
				CBrush brush;
				CPen pen;
				brush.CreateSolidBrush(backcolor);
				pen.CreatePen(PS_SOLID,1,backcolor);
				oldbrush=(HBRUSH)dc.SelectObject(brush);
				oldpen=(HPEN)dc.SelectObject(pen);
				dc.Rectangle(lpdis->rcItem.left,lpdis->rcItem.top+1,lpdis->rcItem.right,lpdis->rcItem.bottom-1);
				dc.SelectObject(oldbrush);
				dc.SelectObject(oldpen);
			}

			
			SHFILEINFOW fi;
			fi.hIcon=NULL;
			HGDIOBJ hOld=dc.SelectObject(m_hSendToListFont);
			ShellFunctions::GetFileInfo((LPITEMIDLIST)(lpdis->itemData),0,&fi,SHGFI_DISPLAYNAME|SHGFI_ICON|SHGFI_SMALLICON);
			dc.SetTextColor(forecolor);
			dc.SetBkColor(backcolor);
			if (fi.hIcon!=NULL)
			{
				dc.DrawState(CPoint(lpdis->rcItem.left+5,lpdis->rcItem.top+(lpdis->rcItem.bottom-lpdis->rcItem.top)/2-8),CSize(16,16),(DRAWSTATEPROC)NULL,(LPARAM)fi.hIcon,DST_ICON);
				DestroyIcon(fi.hIcon);
			}
			CRect rect(lpdis->rcItem.left+27,lpdis->rcItem.top,lpdis->rcItem.right,lpdis->rcItem.bottom);
			dc.DrawText(fi.szDisplayName,-1,&rect,DT_SINGLELINE|DT_VCENTER|DT_LEFT);
			dc.SelectObject(hOld);

			
		}
		break;
	case IDC_STATUS:
		{
			switch(lpdis->itemID)
			{
			case 2:
			case 3:
				if (lpdis->itemData!=NULL)
				{
					DrawIconEx(lpdis->hDC,lpdis->rcItem.left+1,lpdis->rcItem.top,
						(HICON)lpdis->itemData,16,16,NULL,NULL,DI_NORMAL);
				}
				break;
			}		
			break;
		}
	/*case IDC_TAB:
		if (lpdis->itemAction==ODA_DRAWENTIRE)
		{
	
			HMODULE hUxTheme=GetModuleHandle("uxtheme.dll");
			if (hUxTheme!=NULL)
			{
				HRESULT(STDAPICALLTYPE *pDrawThemeParentBackground)(HWND hwnd,HDC hdc, RECT *prc);
				HRESULT(STDAPICALLTYPE *pDrawThemeEdge)(HANDLE,HDC,int,int,const RECT*,UINT,UINT, RECT *);
				HRESULT(STDAPICALLTYPE *pDrawThemeBackground)(HANDLE,HDC,int,int,const RECT*,const RECT*);
				HANDLE(STDAPICALLTYPE * pOpenThemeData)(HWND,LPCWSTR);
				HRESULT(STDAPICALLTYPE * pCloseThemeData)(HANDLE);

				pDrawThemeParentBackground=(HRESULT(STDAPICALLTYPE *)(HWND hwnd,HDC hdc, RECT *prc))GetProcAddress(hUxTheme,"DrawThemeParentBackground");
				pDrawThemeEdge=(HRESULT(STDAPICALLTYPE *)(HANDLE,HDC,int,int,const RECT*,UINT,UINT, RECT *))GetProcAddress(hUxTheme,"DrawThemeEdge");
				pDrawThemeBackground=(HRESULT(STDAPICALLTYPE *)(HANDLE,HDC,int,int,const RECT*,const RECT*))GetProcAddress(hUxTheme,"DrawThemeBackground");
				pOpenThemeData=(HANDLE(STDAPICALLTYPE*)(HWND,LPCWSTR))GetProcAddress(hUxTheme,"OpenThemeData");
				pCloseThemeData=(HRESULT(STDAPICALLTYPE *)(HANDLE))GetProcAddress(GetModuleHandle("uxtheme.dll"),"CloseThemeData");
		
			
				if (pDrawThemeParentBackground!=NULL)
					pDrawThemeParentBackground(lpdis->hwndItem,lpdis->hDC,&lpdis->rcItem);

				if (pOpenThemeData!=NULL)
				{
					HANDLE hTheme=pOpenThemeData(lpdis->hwndItem,L"tab");
					if (hTheme!=NULL)
					{
						if (pDrawThemeBackground!=NULL)
							pDrawThemeBackground(hTheme,lpdis->hDC,TABP_TOPTABITEMBOTHEDGE,(lpdis->itemState)&ODS_HOTLIGHT?TTIBES_HOT:TIBES_NORMAL,&lpdis->rcItem,NULL);
						
						//if (pDrawThemeEdge!=NULL)
						//	pDrawThemeEdge(hTheme,lpdis->hDC,TABP_TABITEMBOTHEDGE,TIBES_NORMAL,&lpdis->rcItem,EDGE_SUNKEN,BF_RECT,NULL);

						if (pCloseThemeData!=NULL)
							pCloseThemeData(hTheme);
					}
				}

			}
			
		}*/
	}
}

void CLocateDlg::OnHelp(LPHELPINFO lphi)
{
	CLocateApp::HelpID id[]= {
		{IDC_OK, "dialog_locate.htm#ld_findnow" },
		{IDC_STOP, "dialog_locate.htm#ld_stop" },
		{IDC_NEWSEARCH, "dialog_locate.htm#ld_newbutton" },
		{IDC_PRESETS,"dialog_locate.htm#ld_presets" },
		{IDC_NAME,"tab_name.htm#tn_name"},
		{IDC_TYPE,"tab_name.htm#tn_type"},
		{IDC_LOOKIN, "tab_name.htm#tn_lookin"},
		{IDC_MOREDIRECTORIES,"tab_name.htm#tn_lookin"},
		{IDC_NOSUBDIRECTORIES,"tab_name.htm#tn_nosubdirs"},
		{IDC_BROWSE,"tab_name.htm#tn_browse"},
		{IDC_CHECKMINIMUMSIZE,"tab_sizedate.htm#ts_minfilesize"},
		{IDC_MINIMUMSIZE,"tab_sizedate.htm#ts_minfilesize"},
		{IDC_MINSIZETYPE,"tab_sizedate.htm#ts_minfilesize"},
		{IDC_MINIMUMSIZESPIN,"tab_sizedate.htm#ts_minfilesize"},
		{IDC_CHECKMAXIMUMSIZE,"tab_sizedate.htm#ts_maxfilesize"},
		{IDC_MAXIMUMSIZE,"tab_sizedate.htm#ts_maxfilesize"},
		{IDC_MAXSIZETYPE,"tab_sizedate.htm#ts_maxfilesize"},
		{IDC_MAXIMUMSIZESPIN,"tab_sizedate.htm#ts_maxfilesize"},
		{IDC_CHECKMINDATE,"tab_sizedate.htm#ts_filesnewerthan"},
		{IDC_MINDATE,"tab_sizedate.htm#ts_filesnewerthan"},
		{IDC_MINTYPE,"tab_sizedate.htm#ts_filesnewerthan"},
		{IDC_CHECKMAXDATE,"tab_sizedate.htm#ts_filesolderthan"},
		{IDC_MAXDATE,"tab_sizedate.htm#ts_filesolderthan"},
		{IDC_MAXTYPE,"tab_sizedate.htm#ts_filesolderthan"},
		{IDC_CHECK,"tab_advanced.htm#ta_check"},
		{IDC_MATCHWHOLENAME,"tab_advanced.htm#ta_matchwholename"},
		{IDC_MATCHCASE,"tab_advanced.htm#ta_matchcase"},
		{IDC_REPLACESPACES,"tab_advanced.htm#ta_replacespaces"},
		{IDC_USEWHOLEPATH,"tab_advanced.htm#ta_usewholepath"},
		{IDC_FILETYPE,"tab_advanced.htm#ta_typeoffile"},
		{IDC_CONTAINDATACHECK,"tab_advanced.htm#ta_fileconttext"},
		{IDC_CONTAINDATA,"tab_advanced.htm#ta_fileconttext"},
		{IDC_DATAMATCHCASE,"tab_advanced.htm#ta_datamatchcase"},
		{IDC_HELPTOOLBAR,"tab_advanced.htm#ta_fileconttext"}
	};


	if (CLocateApp::OpenHelp(*this,NULL,lphi,id,sizeof(id)/sizeof(CLocateApp::HelpID)))
		return;


	// Menu item
	if (HtmlHelp(HH_HELP_CONTEXT,lphi->iCtrlId)==NULL)
	{
		// No topic found, show topics window
		HtmlHelp(HH_DISPLAY_TOPIC,0);
	}
	
}
	

BOOL CLocateDlg::OnInitDialog(HWND hwndFocus)
{
	//DebugMessage("CLocateDlg::OnInitDialog BEGIN");

	ShowDlgItem(IDC_FILELIST,swHide);
	ShowDlgItem(IDC_STATUS,swHide);
	
	// Loading "imghnd.dll"
	m_pImageHandler=new ImageHandlerDll;
	if (!m_pImageHandler->IsLoaded())
	{
		delete m_pImageHandler;
		m_pImageHandler=NULL;
	}

	CDialog::OnInitDialog(hwndFocus);

	// Load icons
	LoadDialogIcon();

	// Set dialog transparency
	SetDialogTransparency();
	
	// Initialize clipboard handler
	m_hNextClipboardViewer=SetClipboardViewer(*this);



	m_CircleBitmap.LoadBitmap(IDB_CIRCLE);
	
	m_pTabCtrl=new CTabCtrl(GetDlgItem(IDC_TAB));
	m_pListCtrl=new CListCtrlEx(GetDlgItem(IDC_FILELIST));
	m_pStatusCtrl=new CStatusBarCtrl(GetDlgItem(IDC_STATUS));

	if (IsUnicodeSystem())
	{
		m_pTabCtrl->SetUnicodeFormat(TRUE);
		m_pListCtrl->SetUnicodeFormat(TRUE);
		m_pStatusCtrl->SetUnicodeFormat(TRUE);
	}
	

	
	// Setting tab control labels	
	WCHAR Buffer[80];
	TC_ITEMW ti;
	
	LoadString(IDS_NAME,Buffer,80);
	ti.pszText=Buffer;
	ti.mask=TCIF_TEXT;
	m_pTabCtrl->InsertItem(0,&ti);
	LoadString(IDS_SIZEDATE,Buffer,80);
	ti.pszText=Buffer;
	ti.mask=TCIF_TEXT;
	m_pTabCtrl->InsertItem(1,&ti);
	LoadString(IDS_ADVANCED,Buffer,80);
	ti.pszText=Buffer;
	ti.mask=TCIF_TEXT;
	m_pTabCtrl->InsertItem(2,&ti);

	ViewDetails* pDetails=GetDefaultDetails();
	for (int i=0;i<TypeCount;i++)
	{
		m_pListCtrl->InsertColumn(DetailType(i),pDetails[i].nString,
			pDetails[i].bShow,pDetails[i].nAlign,pDetails[i].nWidth,LanguageSpecificResource);
	}
	delete[] pDetails;


	// Enable d'n'd support for the header
	m_pListCtrl->SetExtendedListViewStyle(LVS_EX_HEADERDRAGDROP,LVS_EX_HEADERDRAGDROP);
	
	// Enable double buffer in Vista
	m_pListCtrl->SetExtendedListViewStyle(LVS_EX_DOUBLEBUFFER,LVS_EX_DOUBLEBUFFER);
	
	
	// Loading column widths
	m_pListCtrl->LoadColumnsState(HKCU,CRegKey2::GetCommonKey()+"\\General","ListWidths");




	
	// Initializing drop target
	OleInitialize(NULL);
	m_pDropTarget=new CFileTarget;
	m_pDropTarget->AutoDelete();
	m_pDropTarget->AddRef();
	RegisterDragDrop(*m_pListCtrl,m_pDropTarget);



	// Creating windows
	m_NameDlg.Create(*this);
	m_NameDlg.ShowWindow(swShow);
	m_SizeDateDlg.Create(*this);
	DWORD dwError=GetLastError();
	m_SizeDateDlg.ShowWindow(swHide);

	m_AdvancedDlg.Create(*this);
	m_AdvancedDlg.ShowWindow(swHide);
	
	
	// Retrieving the maximum height
	CRect rcTemp,rcTemp2;
	m_NameDlg.GetClientRect(&rcTemp);
	m_nTabbedDialogHeight=rcTemp.Height();
    m_SizeDateDlg.GetClientRect(&rcTemp);
	if (m_nTabbedDialogHeight<rcTemp.Height())
		m_nTabbedDialogHeight=rcTemp.Height();
	m_AdvancedDlg.GetClientRect(&rcTemp);
	if (m_nTabbedDialogHeight<rcTemp.Height())
		m_nTabbedDialogHeight=rcTemp.Height();
	if (m_nTabbedDialogHeight<30)
		m_nTabbedDialogHeight=143;
	
	// Computing the height of dialog when dialog is minimized
	::GetWindowRect(GetDlgItem(IDC_TAB),&rcTemp);
	m_nTabHeaderHeight=rcTemp.Height();
	m_nMaxYMinimized=m_nTabbedDialogHeight+m_nTabHeaderHeight+GetSystemMetrics(SM_CYCAPTION)
		+GetSystemMetrics(SM_CYMENU)+2*GetSystemMetrics(SM_CYDLGFRAME)+30;	
	
	// Get width and spacing of buttons
	::GetWindowRect(GetDlgItem(IDC_OK),&rcTemp);
	m_nButtonWidth=rcTemp.Width();
	::GetWindowRect(GetDlgItem(IDC_STOP),&rcTemp2);
	m_nButtonSpacing=BYTE(rcTemp2.top-rcTemp.top);
	
	::GetWindowRect(GetDlgItem(IDC_PRESETS),&rcTemp);
	m_nPresetsButtonWidth=rcTemp.Width();
	m_nPresetsButtonHeight=rcTemp.Height();
	m_nPresetsButtonOffset=char(rcTemp.left-rcTemp2.left);
    	
	

	// Loading registry
	LoadRegistry();

	// Refreshing dialog box
	m_NameDlg.InitDriveBox(TRUE);

	


	// Check default shortcut integrity
#ifdef _DEBUG
	CArrayFP<CShortcut*> aDefaultShortcuts;
	ASSERT(CShortcut::GetDefaultShortcuts(aDefaultShortcuts,CShortcut::loadAll));
#endif

	// Set dialog mode
	SetDialogMode(FALSE,TRUE);

	// Set result list font
	SetResultListFont();
	
	// Set correct function for string comparisons
	SetListCompareFunction();

	// Setting topmost mode if needed
	if (GetFlags()&fgDialogTopMost)
		SetWindowPos(HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
	
	// Setting list control imagelists, type and selection style
	SetSystemImageLists(m_pListCtrl,&m_AdvancedDlg.m_hDefaultTypeIcon);
	SetListType(m_nCurrentListType,TRUE);
	SetListSelStyle();
	if (GetFlags()&fgLVAllowInPlaceRenaming)
		m_pListCtrl->ModifyStyle(0,LVS_EDITLABELS);


	
	

    
	// Setting tooltips
	InitTooltips();
	
	// Enabling multidirectory support if needed
	m_NameDlg.EnableMultiDirectorySupport(GetFlags()&fgNameMultibleDirectories?TRUE:FALSE);
	
	// Loading texts which are used at last time
	if (GetFlags()&fgDialogRememberFields)
		LoadDialogTexts();

	// Sorting
	SetSorting();
	
	// Load shortcuts and actions
	SetShortcuts();	
	LoadResultlistActions();


	// Taking command line parameters to use
	if (GetLocateApp()->GetStartData()!=NULL)
	{
		SetStartData(GetLocateApp()->GetStartData());
		GetLocateApp()->ClearStartData();
	}

	



	// If updating is started via command line arguments or otherwise, start animation
	if (GetTrayIconWnd()->IsUpdateAnimationRunning())
		StartUpdateAnimation();

#ifdef _DEBUG
	CMenu smenu(GetSystemMenu());
	MENUITEMINFO mii;
	mii.cbSize=sizeof(MENUITEMINFO);
	mii.fMask=MIIM_ID|MIIM_TYPE;
	mii.wID=0x76;
	mii.dwTypeData="Show shedule from data";
	mii.dwItemData=0;
	mii.fType=MFT_STRING;
	smenu.InsertMenu(0x76,FALSE,&mii);
	mii.wID=0x77;
	mii.dwTypeData="Show debug related information";
	mii.dwItemData=0;
	mii.fType=MFT_STRING;
	smenu.InsertMenu(0x77,FALSE,&mii);
	
#endif

	// Set focus
	m_NameDlg.SetFocus();
	m_NameDlg.m_Name.SetFocus();
	
	// Make title
	CStringW title;
	title.LoadString(IDS_TITLE);
	title.AddString(IDS_ALLFILES);
	
	if (GetLocateApp()->m_nInstance>0)
		title << L" (" << DWORD(GetLocateApp()->m_nInstance+1) << L')';
	SetText(title);
	
	return FALSE;
}

void CLocateDlg::OnInitMenuPopup(HMENU hPopupMenu,UINT nIndex,BOOL bSysMenu)
{
	if (bSysMenu)
		return;

	HMENU hMainMenu=GetMenu();
	HMENU hFileMenu=GetSubMenu(hMainMenu,0);
	// m_hActivePopupMenu points to last activate menu 
	
	if (nIndex>0 && GetSubMenu(GetMenu(),nIndex)==hPopupMenu)
	{
		// Initializing other main menu's submenus than File menu
		OnInitMainMenu(hPopupMenu,nIndex);
		return;
	}

	CDialog::OnInitMenuPopup(hPopupMenu,nIndex,bSysMenu);
	
	if (hPopupMenu==hFileMenu)
	{
		// File menu in main menu bar
		DWORD dwSimpleMenu=FALSE;

		CRegKey2 RegKey;
		if (RegKey.OpenKey(HKCU,"\\General",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
			RegKey.QueryValue(L"SimpleFileMenu",dwSimpleMenu);


		// Inserting default menu items
		int nSelectedItems=m_pListCtrl->GetSelectedCount();
		if (nSelectedItems>0)
		{
			CLocatedItem** pItems=new CLocatedItem* [nSelectedItems];
			int iItem=m_pListCtrl->GetNextItem(-1,LVNI_SELECTED);
			for (int i=0;i<nSelectedItems;i++)
			{
				pItems[i]=(CLocatedItem*)m_pListCtrl->GetItemData(iItem);
				iItem=m_pListCtrl->GetNextItem(iItem,LVNI_SELECTED);
			}
			
			CreateFileContextMenu(hPopupMenu,pItems,nSelectedItems,dwSimpleMenu,FALSE);
			delete[] pItems;
		}
		else
			CreateFileContextMenu(hPopupMenu,NULL,0,dwSimpleMenu,FALSE);
			

		// Enable items
		OnInitFileMenu(hPopupMenu);
	}
	else if (m_pActiveContextMenu!=NULL && hPopupMenu==m_pActiveContextMenu->hPopupMenu)
	{
		// Context menu for file item(s)		
		// Enable items
		OnInitFileMenu(hPopupMenu);
	}
	else if (IsSendToMenu(hPopupMenu))
	{
		// SentTo menu, deleting previous menu items and inserting 
		// new ones corresponding selected items
		OnInitSendToMenu(hPopupMenu);
	}
	else if (CLocateApp::IsDatabaseMenu(hPopupMenu))
	{
		// Database menu, deleting previous menu items and inserting 
		// new ones database items
		GetLocateApp()->OnInitDatabaseMenu(CMenu(hPopupMenu));
	}
}
	
void CLocateDlg::OnMeasureItem(int nIDCtl,LPMEASUREITEMSTRUCT lpmis)
{
	CDialog::OnMeasureItem(nIDCtl,lpmis);
	
	if (nIDCtl==0)
	{
		// This is IContextMenu item
		if (m_pActiveContextMenu!=NULL && lpmis->itemID<IDM_DEFSENDTOITEM)
			return;

		// SendTo menu item
		
		CDC dc(this);
		HGDIOBJ hOld=dc.SelectObject(m_hSendToListFont);
		
		SHFILEINFOW fi;
		fi.hIcon=NULL;
		ShellFunctions::GetFileInfo((LPITEMIDLIST)lpmis->itemData,0,&fi,SHGFI_DISPLAYNAME|SHGFI_ICON|SHGFI_SMALLICON);
			
		CSize sz=dc.GetTextExtent(fi.szDisplayName,(int)wcslen(fi.szDisplayName));
		lpmis->itemWidth=40+sz.cx;
		if (sz.cy>16)
			lpmis->itemHeight=sz.cy+4;
		else
			lpmis->itemHeight=20;
		dc.SelectObject(hOld);
	}
}
	
BOOL CLocateDlg::OnNotify(int idCtrl,LPNMHDR pnmh)
{
	//DebugFormatMessage("%X->CLocateDlg::OnNotify(%d,%X)",DWORD(this),idCtrl,DWORD(pnmh));

	switch (idCtrl)
	{
	case IDC_FILELIST:
		return ListNotifyHandler((NMLISTVIEW*)pnmh);
	case IDC_TAB:
		if (pnmh->code==TCN_SELCHANGE)
			SetVisibleWindowInTab();
		break;
	default:
		if (m_pListTooltips!=NULL)
		{
			if (pnmh->hwndFrom!=*m_pListTooltips)
				break;
			if (m_iTooltipItem==-1 || m_iTooltipSubItem==-1)
				break;

			switch (pnmh->code)
			{
			case TTN_GETDISPINFOA:
				//DebugNumMessage("CLocateDlg::OnNotify; TTN_GETDISPINFO, ((NMTTDISPINFO*)pnmh)->lParam=%X",((NMTTDISPINFO*)pnmh)->lParam);
				
				if (m_iTooltipItem==-1)
					break;

				((NMTTDISPINFO*)pnmh)->hinst=NULL;
				if (DetailType(m_pListCtrl->GetColumnIDFromSubItem(m_iTooltipSubItem))==Name)
				{
						
					CLocatedItem* pItem=(CLocatedItem*)m_pListCtrl->GetItemData(m_iTooltipItem);
					if (pItem!=NULL)
					{
						LPSTR szBuffer=alloccopyWtoA((LPCWSTR)pItem->GetToolTipText());
						AssignBuffer(szBuffer);
						((NMTTDISPINFO*)pnmh)->lpszText=szBuffer;
					}
					else
						((NMTTDISPINFO*)pnmh)->lpszText=(LPSTR)szEmpty;
				}
				else
				{
					CLocatedItem* pItem=(CLocatedItem*)m_pListCtrl->GetItemData(m_iTooltipItem);
					if (pItem!=NULL)
					{
						LPSTR szBuffer=alloccopyWtoA((LPCWSTR)pItem->GetDetailText(
							DetailType(m_pListCtrl->GetColumnIDFromSubItem(m_iTooltipSubItem))));
						AssignBuffer(szBuffer);
						((NMTTDISPINFO*)pnmh)->lpszText=szBuffer;
					}
					else
						((NMTTDISPINFO*)pnmh)->lpszText=(LPSTR)szEmpty;

				}
				break;
			case TTN_GETDISPINFOW:
				//DebugNumMessage("CLocateDlg::OnNotify; TTN_GETDISPINFO, ((NMTTDISPINFO*)pnmh)->lParam=%X",((NMTTDISPINFO*)pnmh)->lParam);
				
				if (m_iTooltipItem==-1)
					break;

				((NMTTDISPINFOW*)pnmh)->hinst=NULL;
				if (DetailType(m_pListCtrl->GetColumnIDFromSubItem(m_iTooltipSubItem))==Name)
				{
						
					CLocatedItem* pItem=(CLocatedItem*)m_pListCtrl->GetItemData(m_iTooltipItem);
					
					if (pItem!=NULL)
						((NMTTDISPINFOW*)pnmh)->lpszText=pItem->GetToolTipText();
					else
						((NMTTDISPINFOW*)pnmh)->lpszText=(LPWSTR)szEmpty;
				}
				else
				{
					CLocatedItem* pItem=(CLocatedItem*)m_pListCtrl->GetItemData(m_iTooltipItem);
					if (pItem!=NULL)
					{
						((NMTTDISPINFOW*)pnmh)->lpszText=pItem->GetDetailText(
							DetailType(m_pListCtrl->GetColumnIDFromSubItem(m_iTooltipSubItem)));
					}
					else
						((NMTTDISPINFOW*)pnmh)->lpszText=(LPWSTR)szwEmpty;

				}
				break;
			case TTN_SHOW:
				DebugMessage("CLocateDlg::OnNotify; TTN_SHOW");

				m_bTooltipActive=TRUE;

				if (DetailType(m_pListCtrl->GetColumnIDFromSubItem(m_iTooltipSubItem))!=Name)
				{
					CRect rc;
					m_pListCtrl->GetSubItemRect(m_iTooltipItem,m_iTooltipSubItem,LVIR_LABEL,&rc);
					m_pListCtrl->ClientToScreen(&rc);
                                     
					if (DetailType(m_pListCtrl->GetColumnIDFromSubItem(m_iTooltipSubItem))==InFolder)
					{
						CRect rc2;
						m_pListCtrl->GetSubItemRect(m_iTooltipItem,m_iTooltipSubItem,LVIR_ICON,&rc2);
						rc.left+=rc2.Width()+2;
					}

					rc.left+=3;

					if (!IsExtraFlagSet(efLVDontMoveTooltips))
						m_pListTooltips->SetWindowPos(HWND_TOPMOST,rc.left,rc.top,0,0,
							SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER|SWP_ASYNCWINDOWPOS|SWP_NOOWNERZORDER|SWP_NOSENDCHANGING);
					SetWindowLong(dwlMsgResult,1);

					return 1;
				}
				return 0;				
			case TTN_POP:
				DebugMessage("CLocateDlg::OnNotify; TTN_POP");
				m_bTooltipActive=FALSE;
				break;
			case NM_CUSTOMDRAW:
				return CDRF_DODEFAULT;
			}
		}
		break;
	}

	//DebugMessage("CLocateDlg::OnNotify exit");
	return CDialog::OnNotify(idCtrl,pnmh);
}


void CLocateDlg::OnSize(UINT nType, int cx, int cy)
{
	//DebugFormatMessage("CLocateDlg::OnSize(%d,%d,%d) BEGIN",nType,cx,cy);
	
	CDialog::OnSize(nType,cx,cy);

	switch (nType)
	{
	case SIZE_MINIMIZED:
		// Freeing not used memory
		ISDLGTHREADOK
		//StopBackgroundOperations();
		ChangeBackgroundOperationsPriority(TRUE);

		FreeBuffers();

		if (m_hSendToListFont!=NULL)
		{
			DebugCloseGdiObject(m_hSendToListFont);
			DeleteObject(m_hSendToListFont);
			m_hSendToListFont=NULL;
		}
		ClearMenuVariables();

		// Minimizing to system tray
		if (m_dwFlags&fgDialogMinimizeToST)
			ShowWindow(swHide);

		{
			CStringW Named;
			m_NameDlg.GetDlgItemText(IDC_NAME,Named);
			if (Named.GetLength()>0)
				GetTrayIconWnd()->SetTrayIcon(NULL,IDS_NOTIFYLOCATEWITHNAMED,Named);
			else
				GetTrayIconWnd()->SetTrayIcon(NULL,IDS_NOTIFYLOCATE,NULL);

		}
		break;
	case SIZE_RESTORED:
		DebugMessage("CLocateDlg::OnSize: SIZE_RESTORED");
		
		GetTrayIconWnd()->SetTrayIcon(NULL,IDS_NOTIFYLOCATE,NULL);
		if (m_dwFlags&fgLargeMode)
		{
			RECT rect;
			GetWindowRect(&rect);
			m_nLargeY=rect.bottom-rect.top;

			
			if (m_pListCtrl->GetItemCount()>0)
			{
				ChangeBackgroundOperationsPriority(FALSE);
				//StartBackgroundOperations();
				if (m_pBackgroundUpdater!=NULL)
					m_pBackgroundUpdater->StopWaiting();
			}

		}
		

		SetControlPositions(nType,cx,cy);
		break;
	case SIZE_MAXIMIZED:
		if (m_dwFlags&fgLargeMode)
		{
			if (m_pListCtrl->GetItemCount()>0)
			{
				ChangeBackgroundOperationsPriority(FALSE);
				//StartBackgroundOperations();
				if (m_pBackgroundUpdater!=NULL)
					m_pBackgroundUpdater->StopWaiting();
			}
		}
		else 
		{
			// If maximized, be sure that the window is not over the tray window
			HWND hShellTrayWnd=FindWindow("Shell_TrayWnd",NULL); // Whole tray window
			if (hShellTrayWnd!=NULL)
			{
				CRect rcWindowRect,rcTrayRect;
				GetWindowRect(&rcWindowRect);
				::GetWindowRect(hShellTrayWnd,&rcTrayRect);
				int nFullScreenCX=GetSystemMetrics(SM_CXFULLSCREEN);
					
				if (rcTrayRect.top<4 && rcTrayRect.left<4)
				{
					if (rcTrayRect.right>=nFullScreenCX) // Tray top
					{
						if (rcWindowRect.top<=0)
							SetWindowPos(NULL,rcWindowRect.left,rcWindowRect.top+rcTrayRect.Height(),0,0,SWP_NOSIZE|SWP_NOZORDER);

					}
					else // Tray on left
					{
						if (cx>nFullScreenCX)
						{
							rcWindowRect.left+=cx-nFullScreenCX;
							SetWindowPos(NULL,rcWindowRect.left,rcWindowRect.top,rcWindowRect.Width(),rcWindowRect.Height(),
								SWP_NOZORDER);
							cx=nFullScreenCX;
						}
					}
				} 
				else if (rcTrayRect.left>=nFullScreenCX) // Tray on right
				{
					if (cx>nFullScreenCX)
					{
						rcWindowRect.right-=cx-nFullScreenCX;
						SetWindowPos(NULL,rcWindowRect.left,rcWindowRect.top,rcWindowRect.Width(),rcWindowRect.Height(),
							SWP_NOZORDER);
						cx=nFullScreenCX;
					}
				}
			}
		}
		
		SetControlPositions(nType,cx,cy);
		break;
	default:
		// Cannot happen?
		ASSERT(0);
		break;
	}

	//DebugMessage("CLocateDlg::OnSize END");
}


void CLocateDlg::OnTimer(DWORD dwTimerID)
{
	// Somehow this hangs program (?)
	//CDialog::OnTimer(wTimerID);
	
	switch (LOWORD(dwTimerID))
	{
	case ID_REDRAWITEMS:
		KillTimer(ID_REDRAWITEMS);
		m_pListCtrl->InvalidateRect(NULL,FALSE);
		m_pListCtrl->UpdateWindow();
		break;
	case ID_UPDATESELECTED:
		{
			KillTimer(ID_UPDATESELECTED);
				
			int nSelecetedCount=m_pListCtrl->GetSelectedCount();
			int nFolders=0;
			
			if (nSelecetedCount==0)
				m_pStatusCtrl->SetText("",STATUSBAR_MISC,0);
			else if (nSelecetedCount==1 && m_nCurrentListType!=ltDetails)
			{
				int nItem=m_pListCtrl->GetNextItem(-1,LVNI_SELECTED);
				ASSERT(nItem!=-1);
				CLocatedItem* pItem=(CLocatedItem*)m_pListCtrl->GetItemData(nItem);
				if (pItem!=NULL)
				{
					// TODO: locatation could be shorten with ... if it's long
						
					CStringW str;
					if (pItem->IsFolder() || pItem->IsDeleted())
						str.Format(IDS_SELECTEDONEFOLDER,pItem->GetParent());
					else
					{
						ULONGLONG nSize=0;
						if (pItem->GetFileSizeLo()!=DWORD(-1))
							nSize+=pItem->GetFileSize();

						LPWSTR pFileSize=GetLocateApp()->FormatFileSizeString((DWORD)nSize,(DWORD)(nSize>>32));
					
						str.FormatEx(IDS_SELECTEDONEFILE,pFileSize,pItem->GetParent());
						delete[] pFileSize;
					}

					m_pStatusCtrl->SetText(str,STATUSBAR_MISC,0);
				}
				else
					m_pStatusCtrl->SetText("",STATUSBAR_MISC,0);
				
			}
			else // nSelectedCount > 1
			{
				ULONGLONG nTotalSize=0;
				int nIntex=m_pListCtrl->GetNextItem(-1,LVNI_SELECTED);
				while (nIntex!=-1)
				{
					CLocatedItem* pItem=(CLocatedItem*)m_pListCtrl->GetItemData(nIntex);
					if (pItem!=NULL)
					{
						if (pItem->IsFolder())
							nFolders++;
						else if (pItem->GetFileSizeLo()!=DWORD(-1))
							nTotalSize+=pItem->GetFileSize();
					}
					nIntex=m_pListCtrl->GetNextItem(nIntex,LVNI_SELECTED);
				}

				

				CStringW str;
				
				if (nFolders==0)
					str.Format(IDS_SELECTEDCOUNT,nSelecetedCount);
				else if (nFolders==nSelecetedCount)
					str.Format(IDS_SELECTEDCOUNT2,nSelecetedCount);
				else
					str.FormatEx(IDS_SELECTEDCOUNT3,nSelecetedCount-nFolders,nFolders);

				
				if (nTotalSize>0)
				{
					CStringW size;
					LPWSTR pFileSize=GetLocateApp()->FormatFileSizeString((DWORD)nTotalSize,(DWORD)(nTotalSize>>32));
					size.Format(IDS_TOTALSIZE,pFileSize);
					str << L", " << size;
					delete[] pFileSize;
				}

				
				m_pStatusCtrl->SetText(str,STATUSBAR_MISC,0);
				
			}
			break;
		}
	case ID_CLICKWAIT:
		KillTimer(ID_CLICKWAIT);
		m_ClickWait=FALSE;
		break;
	case ID_LOCATEANIM:
		EnterCriticalSection(&m_csLocateAnimBitmaps);
		m_nCurLocateAnimBitmap++;
		if (m_nCurLocateAnimBitmap>=COUNT_LOCATEANIMATIONS)
			m_nCurLocateAnimBitmap=0;
		if (m_pStatusCtrl!=NULL && m_pLocateAnimBitmaps!=NULL)
			m_pStatusCtrl->SetText((LPCSTR)m_pLocateAnimBitmaps[m_nCurLocateAnimBitmap],STATUSBAR_LOCATEICON,SBT_OWNERDRAW);
		LeaveCriticalSection(&m_csLocateAnimBitmaps);
		break;
	case ID_UPDATEANIM:
		EnterCriticalSection(&m_csUpdateAnimBitmaps);
		m_nCurUpdateAnimBitmap++;
		if (m_nCurUpdateAnimBitmap>=COUNT_UPDATEANIMATIONS)
			m_nCurUpdateAnimBitmap=0;
		if (m_pStatusCtrl!=NULL && m_pUpdateAnimBitmaps!=NULL)
			m_pStatusCtrl->SetText((LPCSTR)m_pUpdateAnimBitmaps[m_nCurUpdateAnimBitmap],STATUSBAR_UPDATEICON,SBT_OWNERDRAW);
		LeaveCriticalSection(&m_csUpdateAnimBitmaps);
		break;
	case ID_INSTANTSEARCHTIMER:
		KillTimer(ID_INSTANTSEARCHTIMER);
		InstantSearch();
		break;
	default:
		KillTimer(dwTimerID);
		PostMessage(WM_SETITEMFOCUS,(WPARAM)dwTimerID);
		break;
	}

		
}


LRESULT CLocateDlg::WindowProc(UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch (msg)
	{
	case WM_UPDATINGSTOPPED:
		// Stop update animation
		StopUpdateAnimation();
	
		// Reset Look In box
		m_NameDlg.InitDriveBox();
		break;
	case WM_SETOPERATIONSTATUSBAR:
		if (m_pStatusCtrl!=NULL)
		{
			if (lParam!=NULL)
				m_pStatusCtrl->SetText((LPCWSTR)lParam,STATUSBAR_OPERATIONSTATUS,0);
			if (wParam!=0)
				m_pStatusCtrl->SetText(LPCSTR(::LoadIcon(NULL,(LPCSTR)wParam)),STATUSBAR_UPDATEICON,SBT_OWNERDRAW);
		}
		break;
	case WM_GETMINMAXINFO:
		if (m_nMaxYMinimized==0)
			break;
		if (!(m_dwFlags&fgLargeMode))
		{
			if (m_nMaxYMaximized==0)
				m_nMaxYMaximized=WORD(((LPMINMAXINFO)lParam)->ptMaxTrackSize.y);
			((LPMINMAXINFO)lParam)->ptMaxTrackSize.y=m_nMaxYMinimized;
			((LPMINMAXINFO)lParam)->ptMinTrackSize.y=m_nMaxYMinimized;

			/*
			// Check if the tray is on right. If so, restirct the width
			HWND hShellTrayWnd=FindWindow("Shell_TrayWnd",NULL); // Whole tray window
			if (hShellTrayWnd!=NULL)
			{
				CRect rcTrayRect;
				::GetWindowRect(hShellTrayWnd,&rcTrayRect);
				if (rcTrayRect.top==0 && rcTrayRect.left>5)
					((LPMINMAXINFO)lParam)->ptMaxTrackSize.x=GetSystemMetrics(SM_CXFULLSCREEN);
			}
			*/
		}
		else
		{
			RECT rc;
			m_pStatusCtrl->GetClientRect(&rc);
			((LPMINMAXINFO)lParam)->ptMinTrackSize.y=m_nMaxYMinimized+rc.bottom-rc.top;
		}		
	
		DebugFormatMessage("CLocateDlg::WM_GETMINMAXINFO: ptMaxSize=(%d,%d) ptMinTrackSize=(%d,%d) ptMaxTrackSize=(%d,%d)",
				((LPMINMAXINFO)lParam)->ptMaxSize.x,((LPMINMAXINFO)lParam)->ptMaxSize.y,
				((LPMINMAXINFO)lParam)->ptMinTrackSize.x,((LPMINMAXINFO)lParam)->ptMinTrackSize.y,
				((LPMINMAXINFO)lParam)->ptMaxTrackSize.x,((LPMINMAXINFO)lParam)->ptMaxTrackSize.y);
		return FALSE;
	case WM_EXITMENULOOP:
		if (m_hSendToListFont!=NULL)
		{
			DebugCloseGdiObject(m_hSendToListFont);
			DeleteObject(m_hSendToListFont);
			m_hSendToListFont=NULL;
		}
		break;
	case WM_SETTINGCHANGE:
		if (wParam==0x2a && lParam==NULL && m_pListCtrl!=NULL) // Possibly shell icon cache is updeted
		{
			if (m_pListCtrl->GetItemCount()>0)
			{
				int nItem=m_pListCtrl->GetNextItem(-1,LVNI_ALL);
				while (nItem!=-1)
				{
					CLocatedItem* pItem=(CLocatedItem*)m_pListCtrl->GetItemData(nItem);

					pItem->RemoveFlags(LITEM_ICONOK|LITEM_PARENTICONOK);
					
					nItem=m_pListCtrl->GetNextItem(nItem,LVNI_ALL);
				}
				m_pListCtrl->RedrawItems(0,m_pListCtrl->GetItemCount()-1);
				m_pListCtrl->UpdateWindow();
			}
		}
		SetListSelStyle();
		if (m_pBackgroundUpdater!=NULL)
			m_pBackgroundUpdater->StopWaiting();
		break;
	case WM_DRAWCLIPBOARD:
		return CDialog::WindowProc(msg,wParam,lParam);
	case WM_DRAWITEM:
	case WM_MEASUREITEM:
		if (wParam)
			break;
	case WM_INITMENUPOPUP:
		if (m_pActiveContextMenu!=NULL)
		{
			m_pActiveContextMenu->CheckThread();

			if (m_pActiveContextMenu->pContextMenu3!=NULL)
			{
				HRESULT hRes;
				LRESULT lRet;
				hRes=m_pActiveContextMenu->pContextMenu3->HandleMenuMsg2(msg,wParam,lParam,&lRet);
				if (hRes==NOERROR)
					return lRet;
			}
			if (m_pActiveContextMenu->pContextMenu2!=NULL)
				m_pActiveContextMenu->pContextMenu2->HandleMenuMsg(msg,wParam,lParam);

			return (msg==WM_INITMENUPOPUP ? 0 : TRUE); // handled
		}
		break;
	case WM_GETICON:
	case WM_SETICON:
		DefWindowProc(*this,msg,wParam,lParam);
		break;
	case WM_RESETSHORTCUTS:
		SetShortcuts();
		break;
	case WM_RESULTLISTACTION:
		DebugMessage("CLocateDlg::WindowProc(WM_RESULTLISTACTION,...)");
		return OnExecuteResultAction((CAction::ActionResultList)wParam,(void*)lParam);
	case WM_GETSELECTEDITEMPATH:
		if (lParam!=NULL && wParam!=NULL)
		{
			INT nAllocSize=(*((INT*)wParam))&~(1<<31);
			if (nAllocSize<=0)
				return 0;

			// Copy the path of currently selected item to lparam

			int nItem=m_pListCtrl->GetNextItem(-1,LVNI_SELECTED);
			if (nItem==-1)
				return 0;

			CLocatedItem* pItem=(CLocatedItem*)m_pListCtrl->GetItemData(nItem);
			if (pItem==NULL)
				return 0;

			if ((UINT)nAllocSize>=pItem->GetPathLen()+1)
			{
				CopyMemory((LPSTR)lParam,pItem->GetPath(),pItem->GetPathLen()+1);
				*((UINT*)wParam)=pItem->GetPathLen();
			}
			else
			{
				nAllocSize--;
				CopyMemory((LPSTR)lParam,pItem->GetPath(),nAllocSize);
				((LPSTR)lParam)[nAllocSize]='\0';
				*((UINT*)wParam)=nAllocSize;
			}


		}
		return 0;
	case WM_ENABLEITEMS:
		EnableItems((BOOL)wParam);
		return 0;
	case WM_SETITEMFOCUS:
		::SetFocus((HWND)wParam);
		return 0;
	case WM_CLOSEDIALOG:
		DestroyWindow();
		break;
	case WM_SETFONT:
		m_hDialogFont=HFONT(wParam);
		break;
	case WM_REMOVEIGNORECLICKSFLAG:
		DebugMessage("DND: removing ignore flag");
		RemoveExtraFlags(efLVIgnoreListClicks);
		break;
	case WM_SETSTARTDATA:
		SetStartData((CLocateApp::CStartData*)lParam);
		break;
#ifdef _DEBUG
	case WM_SYSCOMMAND:
		if (wParam==0x76)
		{
			CInputDialog ib(IDD_INPUTBOX);
			ib.SetText("Insert HEX string");
			ib.SetTitle("Show schedule");
			ib.SetOKButtonText("OK");
			ib.SetCancelButtonText("Cancel");
			if (ib.DoModal(*this))
			{
				CStringW text;
				ib.GetInputText(text);
				DWORD dwLength;
				BYTE* pData=dataparser2(text,&dwLength);
				BYTE* pPtr=pData+6;
       			if (pData[1]==1)
				{
					if (dwLength>=6 && pData[0]==sizeof(CSchedule) && 
						dwLength==6+sizeof(CSchedule)*(*(DWORD*)(pData+2)))
					{
						for (DWORD n=0;n<*(DWORD*)(pData+2);n++)
						{
							CSettingsProperties::CAutoUpdateSettingsPage::CCheduledUpdateDlg sud;
							sud.m_pSchedule=new CSchedule(pPtr,1);
							sud.DoModal(*this);
							delete sud.m_pSchedule;
						}
					}		
				}
				else if (pData[1]==2)
				{
					if (dwLength>=6 && pData[0]==sizeof(CSchedule))
					{
						BYTE* pPtr=pData+6;
						for (DWORD n=0;n<*(DWORD*)(pData+2);n++)
						{
							if (pPtr+sizeof(CSchedule)>=pData+dwLength)
								break;

							CSettingsProperties::CAutoUpdateSettingsPage::CCheduledUpdateDlg sud;
							sud.m_pSchedule=new CSchedule(pPtr,2);
							sud.DoModal(*this);
							delete sud.m_pSchedule;
						}
					}
				}
				free(pData);
			}
		}
		else if (wParam==0x77)
		{
			WNDCLASSEX wce;
			ZeroMemory(&wce,sizeof(WNDCLASSEX));
			wce.cbSize=sizeof(wce);
			wce.style=CS_HREDRAW|CS_VREDRAW;
			wce.hInstance=GetInstanceHandle();
			wce.hbrBackground=(HBRUSH)COLOR_WINDOW;
			wce.lpszClassName="DEBUGWINDOWCLASS";
			wce.lpfnWndProc=DebugWindowProc;
			RegisterClassEx(&wce);
			
			SetLastError(0);
			if (CreateWindow("DEBUGWINDOWCLASS","DebugWindow",WS_OVERLAPPEDWINDOW|WS_VISIBLE,0,0,500,100,
				NULL,NULL,GetInstanceHandle(),NULL)==NULL)
				ReportSystemError();
            
		}
		break;
#endif
	}
	return CDialog::WindowProc(msg,wParam,lParam);
}



////////////////////////////////////////////////////////////
// CLocateDlg - Virtual Message Handlers
////////////////////////////////////////////////////////////


void CLocateDlg::OnOk(BOOL bShortcut,BOOL bSelectDatabases)
{
	DlgDebugMessage("CLocateDlg::OnOk BEGIN");
	
	CWaitCursor wait;

	CancelInstantSearch();
	
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
		m_pBackgroundUpdater->IgnoreItemsAndGoToSleep();

	// Deleting previous items and clear tooltips
	RemoveResultsFromList();
	if (m_pListTooltips!=NULL)
		DeleteTooltipTools();


	// If OnOk is not initialized using shortcuts and
	// control is pressed, open select databases dialog
	if (!bShortcut && GetKeyState(VK_CONTROL)&0x8000)
		bSelectDatabases=TRUE;


	


	// Clearing possible exclamation icons
	m_pStatusCtrl->SetText("",STATUSBAR_LOCATEICON,0);
	m_pStatusCtrl->SetText("",STATUSBAR_UPDATEICON,0);
	
	

	
	/////////////////////////////////////////////////////////////////////////////
	// STEP 2 RESOLVE PARAMETERS: Stopping background operatins, clearing list, etc
	/////////////////////////////////////////////////////////////////////////////
	
	CArrayFAP<LPWSTR> aExtensions,aDirectories,aNames;
	m_pLocater=ResolveParametersAndInitializeLocater(aExtensions,aDirectories,
		aNames,FALSE,bSelectDatabases);

	if (m_pLocater==NULL)
		return;
	
	// Set funtion pointers
	m_pLocater->SetFunctions(LocateProc,LocateFoundProc,LocateFoundProcW,DWORD(this));

	
	/////////////////////////////////////////////////////////////////////////////
	// STEP 3 SET DIALOG, START BACKGROUND OPERATIONS AND START LOCATING
	/////////////////////////////////////////////////////////////////////////////
	
	
	// If dialog is not large mode, change it
	SetDialogMode(TRUE);

	// Starting background operations (which are not yet started)
	StartBackgroundOperations();

	// Save last focus
	m_hLastFocus=GetFocus();


	// Starting location
	m_pLocater->LocateFiles(TRUE,(LPCWSTR*)aNames.GetData(),aNames.GetSize(),
		(LPCWSTR*)aExtensions.GetData(),aExtensions.GetSize(),
		(LPCWSTR*)aDirectories.GetData(),aDirectories.GetSize());

	DlgDebugMessage("CLocateDlg::OnOk END");
	
}


void CLocateDlg::OnStop()
{
	CWaitCursor wait;
	
	//if (m_pBackgroundUpdater!=NULL)
	//	m_pBackgroundUpdater->StopWaiting();

	if (IsLocating())
		m_pLocater->StopLocating();
}

void CLocateDlg::OnNewSearch()
{
	CWaitCursor wait;
		
	CancelInstantSearch();
	

	// Stop Locating
	if (IsLocating())
		m_pLocater->StopLocating();

	// Backgorund operation should end
	if (m_pFileNotificationsThread!=NULL)
		m_pFileNotificationsThread->Stop();
	if (m_pBackgroundUpdater!=NULL)
		m_pBackgroundUpdater->Stop();
	
	
	RemoveResultsFromList();
	if (m_pListTooltips!=NULL)
		DeleteTooltipTools();
	SetDialogMode(FALSE);

	// Sorting
	SetSorting();
	
    // Tell OnFieldChange to skip messages
	AddInstantSearchingFlags(isIgnoreChangeMessages);

	// Clear dialogs
	m_NameDlg.OnClear(FALSE);
	m_SizeDateDlg.OnClear(FALSE);
	m_AdvancedDlg.OnClear(FALSE);
	if (m_pTabCtrl->GetCurSel()!=0)
	{
		m_AdvancedDlg.ShowWindow(swHide);
		m_SizeDateDlg.ShowWindow(swHide);
		m_NameDlg.ShowWindow(swShow);
		m_pTabCtrl->SetCurSel(0);
	}
	
	// Do not use selected databases anymore
	RemoveExtraFlags(efUseLastSelectedDatabases);

	// OnFieldChange can work again
	RemoveInstantSearchingFlags(isIgnoreChangeMessages);

	// Make title
	CStringW title;
	title.LoadString(IDS_TITLE);
	title.AddString(IDS_ALLFILES);
	
	if (GetLocateApp()->m_nInstance>0)
		title << L" (" << DWORD(GetLocateApp()->m_nInstance+1) << L')';
	SetText(title);

	m_NameDlg.m_Name.SetFocus();

	
}

BOOL CLocateDlg::OnPresets()
{
	CMenu Menu;
	int nPresets=0;

	Menu.CreatePopupMenu();
	

	CLocateDlg::InsertMenuItemsFromTemplate(Menu,m_Menu.GetSubMenu(SUBMENU_PRESETSELECTION),0);
	
	
	CRegKey2 RegKey;

	if (RegKey.OpenKey(HKCU,"\\Dialogs\\SearchPresets",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
	{
		CRegKey PresetKey;
		WCHAR szBuffer[30];
		
		// Creating preset menu list
		MENUITEMINFOW mii;
		mii.cbSize=sizeof(MENUITEMINFOW);
		mii.wID=IDM_DEFMENUITEM;
		mii.fType=MFT_STRING;
		mii.fMask=MIIM_TYPE|MIIM_ID;
	

		for (int j=0;j<1000;j++)
		{
			StringCbPrintfW(szBuffer,30*sizeof(WCHAR),L"Preset %03d",j);

			if (PresetKey.OpenKey(RegKey,szBuffer,CRegKey::openExist|CRegKey::samRead)!=ERROR_SUCCESS)
				break;

			CStringW sCurrentName;
			if (!PresetKey.QueryValue(WSTRNULL,sCurrentName))
				continue;

			
			// Preset found
			mii.dwTypeData=sCurrentName.GetBuffer();
			mii.wID=IDM_DEFMENUITEM+j;
			
			Menu.InsertMenu(nPresets,TRUE,&mii);

            PresetKey.CloseKey();
			nPresets++;
		}		

		

		RegKey.CloseKey();
	}
	
	if (nPresets>0)
	{
		MENUITEMINFO mii;
		mii.cbSize=sizeof(MENUITEMINFO);
		mii.fType=MFT_SEPARATOR;
		mii.dwTypeData=NULL;
		mii.fMask=MIIM_TYPE;
		Menu.InsertMenu(nPresets,TRUE,&mii);
	}
	else	
		Menu.EnableMenuItem(IDM_PRESETREMOVE,MF_BYCOMMAND|MF_GRAYED);

	RECT rcButton;
	::GetWindowRect(GetDlgItem(IDC_PRESETS),&rcButton);
		
	SetForegroundWindow();
	int wID=TrackPopupMenu(Menu,TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_NONOTIFY|TPM_RETURNCMD,rcButton.left,rcButton.bottom,0,*this,NULL);	
	Menu.DestroyMenu();

	DebugFormatMessage("OnPresets: wID=%d",wID);

	BOOL bRet=FALSE;
	switch (wID)
	{
	case 0:
		return FALSE;
	case IDM_PRESETSAVE:
		bRet=OnPresetsSave();
		break;
	case IDM_PRESETREMOVE:
		{
			// Dialog handles everything
			CRemovePresetDlg RemovePreset;
			bRet=(BOOL)RemovePreset.DoModal();
			break;
		}
	default:
		if (wID>=IDM_DEFMENUITEM)
		{
			ASSERT(wID-IDM_DEFMENUITEM<nPresets);
			bRet=OnPresetsSelection(wID-IDM_DEFMENUITEM);
		}
		else 
			return FALSE;
		break;
	}

	HWND hFocus=GetFocus();
	if (hFocus==NULL || hFocus==GetDlgItem(IDC_PRESETS))
	{
		switch (m_pTabCtrl->GetCurFocus())
		{
		case 0:
			::SetFocus(m_NameDlg.GetNextDlgTabItem(NULL,FALSE));
			break;
		case 1:
			::SetFocus(m_SizeDateDlg.GetNextDlgTabItem(NULL,FALSE));
			break;
		case 2:
			::SetFocus(m_AdvancedDlg.GetNextDlgTabItem(NULL,FALSE));
			break;
		}
	}

	DebugFormatMessage("OnPresets: will return %d",bRet);
	return bRet;
}

void CLocateDlg::OnSelectAll()
{
	if (m_pListCtrl==NULL)
		return;
	if (m_pListCtrl->GetItemCount()==0)
		return;

	int nItem=m_pListCtrl->GetNextItem(-1,LVNI_ALL);
	while (nItem!=-1)
	{
		m_pListCtrl->SetItemState(nItem,LVIS_SELECTED,LVIS_SELECTED);
		nItem=m_pListCtrl->GetNextItem(nItem,LVNI_ALL);
	}
	m_pListCtrl->SetFocus();
}

void CLocateDlg::OnInvertSelection()
{
	if (m_pListCtrl==NULL)
		return;
	if (m_pListCtrl->GetItemCount()==0)
		return;

	int nItem=m_pListCtrl->GetNextItem(-1,LVNI_ALL);
	while (nItem!=-1)
	{
		if (m_pListCtrl->GetItemState(nItem,LVIS_SELECTED))
			m_pListCtrl->SetItemState(nItem,0,LVIS_SELECTED);
		else
			m_pListCtrl->SetItemState(nItem,LVIS_SELECTED,LVIS_SELECTED);
		nItem=m_pListCtrl->GetNextItem(nItem,LVNI_ALL);
	}
	m_pListCtrl->SetFocus();
}

void CLocateDlg::OnSettingsTool()
{
	// Save settings
	SaveResultlistActions();
	SaveDialogTexts();
	SaveRegistry();
	if ((m_pListCtrl->GetStyle() & LVS_TYPEMASK)==LVS_REPORT)
			m_pListCtrl->SaveColumnsState(HKCU,CRegKey2::GetCommonKey()+"\\General","ListWidths");
	GetTrayIconWnd()->SaveSchedules();
	GetLocateApp()->SaveRegistry();


	CStringW sExeName(GetApp()->GetExeNameW());
	ShellFunctions::ShellExecute(*this,NULL,sExeName.Left(sExeName.FindLast('\\')+1)+L"settool.exe",
		NULL,NULL,SW_SHOW);

}

void CLocateDlg::OnDeletePrivateData()
{	
	CDeletePrivateData dpd;
	if (dpd.DoModal(*this))
	{
		OnNewSearch();

		if (dpd.m_dwFlags&CDeletePrivateData::clearNamed)
		{	
			int nOrig=m_NameDlg.m_nMaxNamesInList;
			m_NameDlg.ChangeNumberOfItemsInLists(0,-1,-1);
			m_NameDlg.ChangeNumberOfItemsInLists(nOrig,-1,-1);
		}

		if (dpd.m_dwFlags&CDeletePrivateData::clearExtensions)
		{
			int nOrig=m_NameDlg.m_nMaxTypesInList;
			m_NameDlg.ChangeNumberOfItemsInLists(-1,0,-1);
			m_NameDlg.ChangeNumberOfItemsInLists(-1,nOrig,-1);
		}

		if (dpd.m_dwFlags&CDeletePrivateData::clearLookIn)
		{
			int nOrig=m_NameDlg.m_nMaxBrowse;
			m_NameDlg.ChangeNumberOfItemsInLists(-1,-1,0);
			m_NameDlg.ChangeNumberOfItemsInLists(-1,-1,nOrig);
		}

	}
}


BOOL CLocateDlg::OnProperties(int nItem)
{
	if (m_pListCtrl->GetSelectedCount()==0)
		return FALSE;

	
	int nItems;
	CAutoPtrA<CLocatedItem*> pItems=GetSelectedItems(nItems,nItem);
	ContextMenuInformation ci;
	ci.bForParents=m_pActiveContextMenu!=NULL?m_pActiveContextMenu->bForParents:FALSE;
	
	if (GetContextMenuForItems(&ci,nItems,pItems))
	{
		if (ci.nIDLParentLevel>1)
		{
			// Quote context menu items
			HMENU hMenu=CreatePopupMenu();
			HRESULT hRes=ci.pContextMenu->QueryContextMenu(hMenu,0,IDM_DEFCONTEXTITEM,IDM_DEFSENDTOITEM,CMF_EXPLORE|CMF_CANRENAME);
			if (SUCCEEDED(hRes))
			{
				CMINVOKECOMMANDINFO cii;
				cii.cbSize=sizeof(CMINVOKECOMMANDINFO);
				cii.fMask=0;
				cii.hwnd=*this;
				cii.lpVerb="properties";
				cii.lpParameters=NULL;
				cii.lpDirectory=NULL;
				cii.nShow=SW_SHOWDEFAULT;
				hRes=ci.pContextMenu->InvokeCommand(&cii);
			}
			DestroyMenu(hMenu);
			
			if (SUCCEEDED(hRes))
				return TRUE;
			
		}
	}

	
	CPropertiesSheet* fileprops=new CPropertiesSheet(nItems,pItems,ci.bForParents);

	return fileprops->Open();
}

void CLocateDlg::OnAutoArrange()
{
	CMenu menu(GetMenu());
	if (m_pListCtrl->GetStyle()&LVS_AUTOARRANGE)
	{
		menu.CheckMenuItem(IDM_AUTOARRANGE,MF_BYCOMMAND|MF_UNCHECKED);
		CheckMenuItem(m_Menu.GetSubMenu(SUBMENU_CONTEXTMENUNOITEMS),IDM_AUTOARRANGE,MF_BYCOMMAND|MF_UNCHECKED);
		m_pListCtrl->SetStyle(m_pListCtrl->GetStyle()&~LVS_AUTOARRANGE);
	}
	else
	{
		menu.CheckMenuItem(IDM_AUTOARRANGE,MF_BYCOMMAND|MF_CHECKED);
		CheckMenuItem(m_Menu.GetSubMenu(SUBMENU_CONTEXTMENUNOITEMS),IDM_AUTOARRANGE,MF_BYCOMMAND|MF_CHECKED);
		m_pListCtrl->SetStyle(m_pListCtrl->GetStyle()|LVS_AUTOARRANGE);
	}
}

void CLocateDlg::OnAlignToGrid()
{
	CMenu menu(GetMenu());

	if (m_pListCtrl->GetExtendedListViewStyle()&LVS_EX_SNAPTOGRID)
	{
		menu.CheckMenuItem(IDM_ALIGNTOGRID,MF_BYCOMMAND|MF_UNCHECKED);
		CheckMenuItem(m_Menu.GetSubMenu(SUBMENU_CONTEXTMENUNOITEMS),IDM_ALIGNTOGRID,MF_BYCOMMAND|MF_UNCHECKED);
		m_pListCtrl->SetExtendedListViewStyle(LVS_EX_SNAPTOGRID,0);
	}
	else
	{
		menu.CheckMenuItem(IDM_ALIGNTOGRID,MF_BYCOMMAND|MF_CHECKED);
		CheckMenuItem(m_Menu.GetSubMenu(SUBMENU_CONTEXTMENUNOITEMS),IDM_ALIGNTOGRID,MF_BYCOMMAND|MF_CHECKED);
		m_pListCtrl->SetExtendedListViewStyle(LVS_EX_SNAPTOGRID,LVS_EX_SNAPTOGRID);
		m_pListCtrl->Arrange(LVA_SNAPTOGRID);
		
	}
}

void CLocateDlg::OnRefresh()
{
	m_pListCtrl->RedrawItems(m_pListCtrl->GetTopIndex(),m_pListCtrl->GetTopIndex()+m_pListCtrl->GetCountPerPage());
	m_pListCtrl->UpdateWindow();
}	
		
BOOL CLocateDlg::OnDelete(CLocateDlg::DeleteFlag DeleteFlag,int nItem)
{
	if (DeleteFlag==BasedOnShift)
	{
		if (GetKeyState(VK_SHIFT)& 0x8000)
			DeleteFlag=Delete;
		else
			DeleteFlag=Recycle;
	}

	CWaitCursor wait;
	
	CArrayFAP<LPWSTR> aPaths;
	CUIntArray aPathLengths;
	CIntArray aSymlinksOrJunctions;
	CIntArray aIdsForSymlinksOrJunctions;
	

	BOOL bDeleteParent=m_pActiveContextMenu!=NULL?m_pActiveContextMenu->bForParents:FALSE;

	
	// First collect selected files
	int iItem=m_pListCtrl->GetNextItem(-1,LVNI_SELECTED);
	BOOL bOnlyOne=FALSE;
	if (iItem==-1)
	{
		if (nItem==-1)
			return FALSE;
		
		iItem=nItem;
		bOnlyOne=TRUE;
	}

	while (iItem!=-1)
	{
		CLocatedItem* pItem=(CLocatedItem*)m_pListCtrl->GetItemData(iItem);
		
		if (bDeleteParent)
		{
			if (FileSystem::IsDirectory(pItem->GetParent()))
			{
				if (GetFileAttributesW(pItem->GetParent())&FILE_ATTRIBUTE_REPARSE_POINT)
				{
					aSymlinksOrJunctions.Add(aPaths.GetSize());		
					aIdsForSymlinksOrJunctions.Add(iItem);
				}
				int nLen=istrlen(pItem->GetParent());
				aPaths.Add(alloccopy(pItem->GetParent(),nLen));
				aPathLengths.Add(nLen);
			}
		}
		else if (pItem->IsFolder())
		{
			if (FileSystem::IsDirectory(pItem->GetPath()))
			{
				if (pItem->IsJunkction() || pItem->IsSymlink())
				{
					aSymlinksOrJunctions.Add(aPaths.GetSize());		
					aIdsForSymlinksOrJunctions.Add(iItem);
				}
				int nLen=istrlen(pItem->GetPath());
				aPaths.Add(alloccopy(pItem->GetPath()));
				aPathLengths.Add(nLen);
			}
		}
		else if (FileSystem::IsFile(pItem->GetPath()))
		{
			if (pItem->IsJunkction() || pItem->IsSymlink())
			{
				aSymlinksOrJunctions.Add(aPaths.GetSize());	
				aIdsForSymlinksOrJunctions.Add(iItem);
			}
			int nLen=istrlen(pItem->GetPath());
			aPaths.Add(alloccopy(pItem->GetPath()));
			aPathLengths.Add(nLen);
		}
		
		if (bOnlyOne)
			break;
		
		iItem=m_pListCtrl->GetNextItem(iItem,LVNI_SELECTED);
	}
	



	
	if (aSymlinksOrJunctions.GetSize()>0 && !(GetSystemFeaturesFlag()&efWinVista) )
	{
		// Junctions and Windows Vista not in use
		int nRet=MessageBox(ID2W(IDS_ERRORJUNCTIONS),0,MB_ICONEXCLAMATION|MB_YESNOCANCEL);
		if (nRet==IDCANCEL)
			return FALSE;

		for (int i=aSymlinksOrJunctions.GetSize()-1;i>=0;i--)
		{
			int j=aSymlinksOrJunctions[i];
			if (nRet==IDYES)
				FileSystem::RemoveDirectory(aPaths[j]);

			aPaths.RemoveAt(j);	
			aPathLengths.RemoveAt(j);
			m_pListCtrl->DeleteItem(aIdsForSymlinksOrJunctions[i]);

			// Fix indeces
			for (int k=0;k<aIdsForSymlinksOrJunctions.GetSize();k++)
			{
				if (i!=k && aIdsForSymlinksOrJunctions[k]>aIdsForSymlinksOrJunctions[i])
					aIdsForSymlinksOrJunctions[k]--;
			}
		}
	}

	
	// No files anymore? then return
	if (aPaths.GetSize()==0) // No files
		return TRUE;


	// Filling OPSTRUCT fields
	SHFILEOPSTRUCTW fo;
	fo.hwnd=*this;
	fo.wFunc=FO_DELETE;
	fo.pTo=NULL;

	switch (DeleteFlag)
	{
	case Delete:
		fo.fFlags=0;
		break;
	case Recycle:
		fo.fFlags=FOF_ALLOWUNDO;
		break;
	default:
		if (GetKeyState(VK_SHIFT)& 0x8000)
			fo.fFlags=0;
		else
			fo.fFlags=FOF_ALLOWUNDO;
		break;
	}
	


	// Creating file buffer: file1\0file2\0...filen\0\0
	// Calculating the length of required buffer
	int nBufferLength=1; 
	for (int i=0;i<aPaths.GetSize();i++)
		nBufferLength+=aPathLengths[i]+1;
	WCHAR* pFiles=new WCHAR[nBufferLength];
	fo.pFrom=pFiles;
	for (int i=0;i<aPaths.GetSize();i++)
	{
		MemCopyW(pFiles,aPaths[i],aPathLengths[i]+1);
		pFiles+=aPathLengths[i]+1;
	}
	*pFiles='\0';
	
	
	// Stop background operations
	if (m_pFileNotificationsThread!=NULL)
		m_pFileNotificationsThread->Stop();
	if (m_pBackgroundUpdater!=NULL)
		m_pBackgroundUpdater->IgnoreItemsAndGoToSleep();
	
	
	// Delete files
	int iRet=ShellFunctions::FileOperation(&fo);

	
	delete[] fo.pFrom;

	
	
	if (iRet==0)
	{
		// Error maybe occur we don't clear this
		if (m_pListTooltips!=NULL)
			DeleteTooltipTools();
		
		// Todo: change this code to check items which are in deleted folders and
		// remove them		
		if (m_pListCtrl->GetItemCount()>0)
		{
			iItem=m_pListCtrl->GetNextItem(-1,LVNI_ALL);
			ASSERT(iItem==0); // Checking assumption

			while(iItem!=-1)
			{
				CLocatedItem* pItem=(CLocatedItem*)m_pListCtrl->GetItemData(iItem);
				if (pItem==NULL) 
				{
					iItem=m_pListCtrl->GetNextItem(iItem,LVNI_ALL);
					continue;
				}

				BOOL bDelete=FALSE;
				for (int i=0;i<aPaths.GetSize();i++)
				{
					if (aPathLengths[i]==pItem->GetPathLen())
					{
						if (_wcsnicmp(aPaths[i],pItem->GetPath(),aPathLengths[i])==0)
						{
							bDelete=TRUE;
							break;
						}
					}
					else if (aPathLengths[i]<pItem->GetPathLen())
					{
						if (pItem->GetPath()[aPathLengths[i]]==L'\\' &&
							_wcsnicmp(aPaths[i],pItem->GetPath(),aPathLengths[i])==0)
						{
							bDelete=TRUE;
							break;
						}
					}
				}

				if (bDelete)
					m_pListCtrl->DeleteItem(iItem); 
				else
					iItem=m_pListCtrl->GetNextItem(iItem,LVNI_ALL);
			}
		}
	}

	StartBackgroundOperations();

	
	
	m_pListCtrl->UpdateWindow();
	m_pBackgroundUpdater->StopWaiting();

	return TRUE;

}

void CLocateDlg::OnRemoveFromThisList()
{
	if (GetFocus()==GetDlgItem(IDC_FILELIST))
	{
		int iItem;
		while ((iItem=m_pListCtrl->GetNextItem(-1,LVNI_SELECTED))!=-1)
			m_pListCtrl->DeleteItem(iItem);
	}
}

void CLocateDlg::OnRemoveDeletedFiles()
{
	CIntArray aDeleted;

	if (m_pListCtrl==NULL)
		return;
	if (m_pListCtrl->GetItemCount()==0)
		return;

	int iPrevItem=-1;
	int iItem=m_pListCtrl->GetNextItem(-1,LVNI_ALL);
	while (iItem!=-1)
	{
		CLocatedItem* pItem=(CLocatedItem*)m_pListCtrl->GetItemData(iItem);
		if (pItem!=NULL)
		{
			if (pItem->IsDeleted())
			{
				m_pListCtrl->DeleteItem(iItem);
				iItem=m_pListCtrl->GetNextItem(iPrevItem,LVNI_ALL);
				continue;
			}
		}
		iItem=m_pListCtrl->GetNextItem(iItem,LVNI_ALL);
	}
}

BOOL CLocateDlg::OnRenameFile(int nItem)
{
	if (m_pListCtrl->GetSelectedCount()==0 && nItem==-1)
		return FALSE;
	

	if (!(GetFlags()&fgLVAllowInPlaceRenaming) || (m_pActiveContextMenu!=NULL && m_pActiveContextMenu->bForParents))
		OnChangeFileName();
	else if (m_pListCtrl->GetSelectedCount()>0)
		return m_pListCtrl->EditLabel(m_pListCtrl->GetNextItem(-1,LVNI_SELECTED))!=NULL;
	else if (nItem!=-1)
		return m_pListCtrl->EditLabel(nItem)!=NULL;
	return FALSE;
}
	


BOOL CLocateDlg::OnCopy(BOOL bCut,int nItem)
{
	if (m_pListCtrl->GetSelectedCount()==0 && nItem==-1)
		return FALSE;

	CWaitCursor wait;
	CArray<CLocatedItem*> aItems;
	CFileObject fo;
	
	if (m_pListCtrl->GetSelectedCount()>0)
		fo.SetFiles(m_pListCtrl,TRUE,m_pActiveContextMenu!=NULL?m_pActiveContextMenu->bForParents:FALSE);
    else
	{
		CLocatedItem* pItem=(CLocatedItem*)m_pListCtrl->GetItemData(nItem);
		
		if (m_pActiveContextMenu!=NULL && m_pActiveContextMenu->bForParents)
		{
			if (!FileSystem::IsDirectory(pItem->GetParent()))
				return TRUE;
			fo.SetFile(pItem->GetParent());
		}
		else
		{
			if (!FileSystem::IsFile(pItem->GetPath()) &&
				!FileSystem::IsDirectory(pItem->GetPath()))
				return TRUE;
			fo.SetFile(pItem->GetPath());
		}
	}


	// Opening clipboard
	OpenClipboard();
	EmptyClipboard();

	// Setting formats
	SetClipboardData(RegisterClipboardFormat(CFSTR_SHELLIDLIST),fo.GetItemIDList());
	
	HANDLE hData=(LPSTR)GlobalAlloc(GMEM_FIXED,4);
	if (bCut)
		((int*)hData)[0]=DROPEFFECT_MOVE;
	else
		((int*)hData)[0]=DROPEFFECT_COPY|DROPEFFECT_LINK;
	SetClipboardData(RegisterClipboardFormat(CFSTR_PREFERREDDROPEFFECT),hData);
	SetClipboardData(RegisterClipboardFormat(CFSTR_FILENAME),fo.GetFileNameA());
	SetClipboardData(RegisterClipboardFormat(CFSTR_FILENAMEW),fo.GetFileNameW());
	SetClipboardData(CF_HDROP,fo.GetHDrop());
	
	// Closing clipboard
	CloseClipboard();

	return TRUE;
}
	
typedef HRESULT (__stdcall * PFNSHGETFOLDERPATHA)(HWND, int, HANDLE, DWORD, LPSTR);  // "SHGetFolderPathA"
typedef HRESULT (__stdcall * PFNSHGETFOLDERPATHW)(HWND, int, HANDLE, DWORD, LPWSTR);  // "SHGetFolderPathW"

BOOL CLocateDlg::OnCreateShortcut()
{
	if (m_pListCtrl->GetSelectedCount()==0)
		return FALSE;
	
	CWaitCursor wait;
	CStringW sTargetFolder;
	
	CComPtr<IShellLinkW> pslw;
	CComPtr<IShellLink> psl;
	
	// Resolving desktop path
	if (IsUnicodeSystem())
	{
		CStringW sDesktopPathTmp;
		PFNSHGETFOLDERPATHW pGetFolderPath=(PFNSHGETFOLDERPATHW)GetProcAddress(GetModuleHandle("shell32.dll"),"SHGetFolderPathW");
		if (pGetFolderPath!=NULL)
			pGetFolderPath(*this,CSIDL_DESKTOPDIRECTORY,NULL,SHGFP_TYPE_CURRENT,sDesktopPathTmp.GetBuffer(MAX_PATH));
		else
		{
			CRegKey RegKey;
			if (RegKey.OpenKey(HKCU,"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
				RegKey.QueryValue(L"Desktop",sDesktopPathTmp);
			
		}
	
		if 	(!sDesktopPathTmp.IsEmpty())
		{
			sDesktopPathTmp.FreeExtra();
			sTargetFolder=sDesktopPathTmp;
		}
	}
	else
	{
		CString sDesktopPathTmp;
		PFNSHGETFOLDERPATHA pGetFolderPath=(PFNSHGETFOLDERPATHA)GetProcAddress(GetModuleHandle("shell32.dll"),"SHGetFolderPathA");
		if (pGetFolderPath!=NULL)
			pGetFolderPath(*this,CSIDL_DESKTOPDIRECTORY,NULL,SHGFP_TYPE_CURRENT,sDesktopPathTmp.GetBuffer(MAX_PATH));
		else
		{
			CRegKey RegKey;
			if (RegKey.OpenKey(HKCU,"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
				RegKey.QueryValue("Desktop",sDesktopPathTmp);
			
		}		

		if 	(!sDesktopPathTmp.IsEmpty())
		{
			sDesktopPathTmp.FreeExtra();
			sTargetFolder=sDesktopPathTmp;
		}
	}

	if (!sTargetFolder.IsEmpty())
	{
		switch(ShowErrorMessage(IDS_SHORTCUTTODESKTOP,IDS_SHORTCUT,MB_YESNOCANCEL|MB_ICONQUESTION))
		{
		case IDCANCEL:
			return FALSE;
		case IDNO:
			sTargetFolder.Empty();
			break;
		}
	}

	if (sTargetFolder.IsEmpty())
	{
		CFolderDialog fd(IDS_GETFOLDER,BIF_RETURNONLYFSDIRS|BIF_USENEWUI|BIF_NONEWFOLDERBUTTON);
		if (!fd.DoModal(*this))
			return FALSE;

		if (!fd.GetFolder(sTargetFolder))
			return FALSE;
	}

	if (sTargetFolder.LastChar()!=L'\\')
		sTargetFolder<<L'\\';



	CComPtr<IPersistFile> ppf;
	if (IsUnicodeSystem())
	{
		// Creating instance to shell link handler
		if (!SUCCEEDED(CoCreateInstance(CLSID_ShellLink,NULL,CLSCTX_INPROC_SERVER,IID_IShellLinkW,(void**)&pslw)))
			return FALSE; 

		// Creating instance to PersistFile interface
		if (!SUCCEEDED(pslw->QueryInterface(IID_IPersistFile,(void**)&ppf)))
			return FALSE;
	}
	else
	{
		// Creating instance to shell link handler
		if (!SUCCEEDED(CoCreateInstance(CLSID_ShellLink,NULL,CLSCTX_INPROC_SERVER,IID_IShellLink,(void**)&psl)))
			return FALSE;

		// Creating instance to PersistFile interface
		if (!SUCCEEDED(psl->QueryInterface(IID_IPersistFile,(void**)&ppf)))
			return FALSE;

	}
	

	int nItem=m_pListCtrl->GetNextItem(-1,LVNI_SELECTED);
	BOOL bMsgShowed=FALSE;
	
	if (m_pActiveContextMenu!=NULL?m_pActiveContextMenu->bForParents:FALSE)
	{
		// Create shortcuts for parents
	
		while (nItem!=-1)
		{
			CLocatedItem* pItem=(CLocatedItem*)m_pListCtrl->GetItemData(nItem);
			if (pItem!=NULL)
			{
				WCHAR szTitle[MAX_PATH];
				wcscpy_s(szTitle,MAX_PATH,pItem->GetParent()+LastCharIndex(pItem->GetParent(),L'\\')+1);
					
				if (IsUnicodeSystem())
				{
					if (!SUCCEEDED(pslw->SetPath(pItem->GetParent())))
						return FALSE;
					
					pslw->SetDescription(CStringW(IDS_SHORTCUTTO)+szTitle);
				}
				else
				{
					if (!SUCCEEDED(psl->SetPath(W2A(pItem->GetParent()))))
						return FALSE;
					
					psl->SetDescription(CStringA(IDS_SHORTCUTTO)+W2A(szTitle));
				}

				
				if (!SUCCEEDED(ppf->Save(sTargetFolder+szTitle+L".lnk",TRUE)))
					return FALSE;
				
			}
			nItem=m_pListCtrl->GetNextItem(nItem,LVNI_SELECTED);
		}
	}
	else
	{
		while (nItem!=-1)
		{
			CLocatedItem* pItem=(CLocatedItem*)m_pListCtrl->GetItemData(nItem);
			if (pItem!=NULL)
			{
		
				if (pItem->ShouldUpdateFileTitle())
					pItem->UpdateFileTitle();
					
				if (IsUnicodeSystem())
				{
					if (!SUCCEEDED(pslw->SetPath(pItem->GetPath())))
						return FALSE;

					pslw->SetDescription(CStringW(IDS_SHORTCUTTO)+pItem->GetFileTitle());
				}
				else
				{
					if (!SUCCEEDED(psl->SetPath(W2A(pItem->GetPath()))))
						return FALSE;
				
					psl->SetDescription(CStringA(IDS_SHORTCUTTO)+W2A(pItem->GetFileTitle()));
				}

				
				if (!SUCCEEDED(ppf->Save(sTargetFolder+pItem->GetFileTitle()+L".lnk",TRUE)))
					return FALSE;

			}
			nItem=m_pListCtrl->GetNextItem(nItem,LVNI_SELECTED);
		}
	}
	
	return TRUE;
}


void CLocateDlg::OnSaveResults(BOOL bClipboard)
{
	CSaveResultsDlg SaveResultsDlg(bClipboard);

	
	// Activating selected items feature if possible
	if (!bClipboard)
	{
		if (m_pListCtrl->GetSelectedCount()>0 && !bClipboard)
			SaveResultsDlg.m_nFlags|=RESULT_ACTIVATESELECTEDITEMS;
	}

	// Opening dialog
	if (!SaveResultsDlg.DoModal(*this))
		return;

	// Activating wait cursor
	CWaitCursor wait;

	try {
		// Initializing results
		CResults Results(SaveResultsDlg.m_nFlags|(bClipboard?RESULT_INCLUDESELECTEDITEMS:0),SaveResultsDlg.m_strDescription,TRUE);
		BOOL bHTML=SaveResultsDlg.IsHTML(); 

		Results.Create(m_pListCtrl,SaveResultsDlg.m_aDetails,SaveResultsDlg.m_aDetails.GetSize(),
			!bHTML || SaveResultsDlg.m_strTemplate.IsEmpty());

		if (SaveResultsDlg.m_nFlags&RESULT_CLIPBOARD)
		{
			CMemoryStream stream;
			Results.SaveToStream(&stream,bHTML,SaveResultsDlg.m_strTemplate.IsEmpty()?NULL:LPCWSTR(SaveResultsDlg.m_strTemplate));
	
			stream.Write(L"\0",1);

			// Opening clipboard
			OpenClipboard();
			EmptyClipboard();

			HGLOBAL hGlobal=GlobalAlloc(GMEM_MOVEABLE,stream.GetLength()+2);
			if (hGlobal!=NULL)
			{
				BYTE* pData=(BYTE*)GlobalLock(hGlobal);
				if (pData!=NULL)
				{
					CopyMemory(pData,stream.GetData(),stream.GetLength());
					*((WORD*)(pData+stream.GetLength()))=0;
					GlobalUnlock(hGlobal);
					SetClipboardData(CF_UNICODETEXT,hGlobal);
				}
				else
					GlobalFree(hGlobal);
			}

			
			// Closing clipboard
			CloseClipboard();
		}
		else
		{
			CStringW File;
			SaveResultsDlg.GetFilePath(File);
			Results.SaveToFile(File,bHTML,SaveResultsDlg.m_strTemplate.IsEmpty()?NULL:LPCWSTR(SaveResultsDlg.m_strTemplate));
		}
	}
	catch (CFileException ex)
	{
		if (ex.m_cause==CFileException::invalidFile)
		{
			ShowErrorMessage(IDS_SAVERESULTSINVALIDFORMAT,IDS_ERROR);
			return;
		}
		else if (ex.m_lOsError!=-1)
		{
			WCHAR* pError=CLocateApp::FormatLastOsError();
			if (pError!=NULL)
			{
				CStringW str;
				str.Format(IDS_SAVERESULTSCANNOTSAVERESULTS,pError);
				while (str.LastChar()=='\n' || str.LastChar()=='\r')
				str.DelLastChar();
				MessageBox(str,ID2W(IDS_ERROR),MB_ICONERROR|MB_OK);
				LocalFree(pError);
				return;
			}
			
		}
		
		char szError[2000];
		ex.GetErrorMessage(szError,2000);
		MessageBox(A2W(szError),ID2W(IDS_ERROR),MB_ICONERROR|MB_OK);
		return;

	}
	catch (CException ex)
	{
		if (ex.m_lOsError!=-1)
		{
			WCHAR* pError=CLocateApp::FormatLastOsError();
			if (pError!=NULL)
			{
				CStringW str;
				str.Format(IDS_SAVERESULTSCANNOTSAVERESULTS,pError);
				while (str.LastChar()=='\n' || str.LastChar()=='\r')
				str.DelLastChar();
				MessageBox(str,ID2W(IDS_ERROR),MB_ICONERROR|MB_OK);
				LocalFree(pError);
				return;
			}
			
		}
		
		char szError[2000];
		ex.GetErrorMessage(szError,2000);
		MessageBox(A2W(szError),ID2W(IDS_ERROR),MB_ICONERROR|MB_OK);
	}
	catch (...)
	{
		WCHAR* pError=CLocateApp::FormatLastOsError();
		CStringW str;
		if (pError!=NULL)
		{
			str.Format(IDS_SAVERESULTSCANNOTSAVERESULTS,pError);
			LocalFree(pError);
		}
		else
			str.Format(IDS_SAVERESULTSCANNOTSAVERESULTS,(LPCWSTR)ID2W(IDS_UNKNOWN));

		while (str.LastChar()=='\n' || str.LastChar()=='\r')
		str.DelLastChar();
		MessageBox(str,ID2W(IDS_ERROR),MB_ICONERROR|MB_OK);
		return;
	}
}

void CLocateDlg::OnCopyPathToClipboard(BOOL bShortPath)
{
	CStringW Text;

	int nItems=0;
	int nItem=m_pListCtrl->GetNextItem(-1,LVNI_SELECTED);
	while (nItem!=-1)
	{
		CLocatedItem* pItem=(CLocatedItem*)m_pListCtrl->GetItemData(nItem);

		if (nItems>0)
			Text << L"\r\n";
		if (!bShortPath)
		{
			if (m_pActiveContextMenu!=NULL && m_pActiveContextMenu->bForParents)
				Text<<pItem->GetParent();
			else
				Text<<pItem->GetPath();
		}
		else 
		{
			WCHAR szBuffer[MAX_PATH];
			LPCWSTR pPath=m_pActiveContextMenu!=NULL && m_pActiveContextMenu->bForParents?
				pItem->GetParent():pItem->GetPath();
			if (FileSystem::GetShortPathName(pPath,szBuffer,MAX_PATH))
				Text<<szBuffer;
			else
				Text<<pPath;
		}
		
		nItems++;
		nItem=m_pListCtrl->GetNextItem(nItem,LVNI_SELECTED);
	}
  

	if (IsUnicodeSystem())
	{
		HGLOBAL hMem=GlobalAlloc(GHND,(Text.GetLength()+1)*2);
		if (hMem==NULL)
		{
			ShowErrorMessage(IDS_ERRORCANNOTALLOCATE);
			return;
		}
		
		BYTE* pData=(BYTE*)GlobalLock(hMem);
		MemCopyW((LPWSTR)pData,LPCWSTR(Text),Text.GetLength()+1);
		GlobalUnlock(hMem);

		if (OpenClipboard())
		{
			EmptyClipboard();
			SetClipboardData(CF_UNICODETEXT,hMem);
			CloseClipboard();
		}
	}
	else
	{
		HGLOBAL hMem=GlobalAlloc(GHND,Text.GetLength()+1);
		if (hMem==NULL)
		{
			ShowErrorMessage(IDS_ERRORCANNOTALLOCATE);
			return;
		}

		BYTE* pData=(BYTE*)GlobalLock(hMem);
		WideCharToMultiByte(CP_ACP,0,LPCWSTR(Text),(int)Text.GetLength()+1,(LPSTR)pData,Text.GetLength()+1,NULL,NULL);
		GlobalUnlock(hMem);

		if (OpenClipboard())
		{
			EmptyClipboard();
			SetClipboardData(CF_TEXT,hMem);
			CloseClipboard();
		}

	}
}

void CLocateDlg::OnChangeFileName()
{
	CChangeFilenameDlg fnd;
	
	CRegKey2 RegKey;
	BOOL bNoExtension=FALSE;
	BOOL bChangeParentName=m_pActiveContextMenu!=NULL && m_pActiveContextMenu->bForParents;

	if (RegKey.OpenKey(HKCU,"\\Misc",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
	{
		DWORD dwTemp=0;;
		RegKey.QueryValue("NoExtensionInRename",dwTemp);
		if (dwTemp)
			bNoExtension=TRUE;
	}


	int iItem=m_pListCtrl->GetNextItem(-1,LVNI_SELECTED);
	while (iItem!=-1)
	{
		CLocatedItem* pItem=(CLocatedItem*)m_pListCtrl->GetItemData(iItem);
		if (pItem!=NULL)
		{
			if (bChangeParentName)
			{
				int nSlashIndex=LastCharIndex(pItem->GetParent(),L'\\');
				if (nSlashIndex==-1)
				{
					iItem=m_pListCtrl->GetNextItem(iItem,LVNI_SELECTED);
					continue;
				}

				fnd.m_sParent.Copy(pItem->GetParent(),nSlashIndex);
				fnd.m_sFileName=pItem->GetParent()+nSlashIndex+1;
			}
			else
			{
				fnd.m_sParent=pItem->GetParent();
				fnd.m_sFileName.Copy(pItem->GetName(),pItem->GetNameLen());
			}



			if (bChangeParentName || !bNoExtension || pItem->IsFolder())
				fnd.m_dwFlags&=~CChangeFilenameDlg::fNoExtension;
			else
				fnd.m_dwFlags|=CChangeFilenameDlg::fNoExtension;

			if (fnd.DoModal(*this))
			{
				if (bChangeParentName)
				{
					int nSlashIndex=LastCharIndex(pItem->GetParent(),L'\\');
					if (fnd.m_sFileName.Compare(pItem->GetParent()+nSlashIndex+1)!=0)
						pItem->ChangeParentName(this,fnd.m_sFileName,fnd.m_sFileName.GetLength());
				}
				else
				{
					if (fnd.m_sFileName.Compare(pItem->GetName())!=0)
						pItem->ChangeName(this,fnd.m_sFileName,fnd.m_sFileName.GetLength());
				}

				for (int iColumn=0;iColumn<m_pListCtrl->GetColumnCount();iColumn++)
					m_pListCtrl->SetItemText(iItem,iColumn,LPSTR_TEXTCALLBACKW);

				m_pListCtrl->Update(iItem);
				m_pListCtrl->RedrawItems(iItem,iItem);
			}
		}
		iItem=m_pListCtrl->GetNextItem(iItem,LVNI_SELECTED);
	}
	m_pListCtrl->UpdateWindow();
}

void CLocateDlg::OnChangeFileNameCase()
{
	if (m_pListCtrl->GetNextItem(-1,LVNI_SELECTED)==-1)
		return;

	CChangeCaseDlg cd;
	if (cd.DoModal(*this))
	{
		int iItem=m_pListCtrl->GetNextItem(-1,LVNI_SELECTED);
		while (iItem!=-1)
		{
			CLocatedItem* pItem=(CLocatedItem*)m_pListCtrl->GetItemData(iItem);
            if (pItem!=NULL)
			{
				int iLength;
				LPWSTR szName;
				if (m_pActiveContextMenu!=NULL && m_pActiveContextMenu->bForParents)
				{
					int iParentPos=LastCharIndex(pItem->GetParent(),L'\\');
					if (iParentPos==-1)
					{
						iItem=m_pListCtrl->GetNextItem(iItem,LVNI_SELECTED);
						continue;
					}

					szName=pItem->GetParent()+iParentPos+1;
					iLength=istrlen(szName);
				}
				else
				{
					szName=pItem->GetName();
					if (!cd.bForExtension)
					{
						iLength=LastCharIndex(szName,L'.')+1;
						if (iLength==0)
							iLength=pItem->GetNameLen();
					}
					else
						iLength=pItem->GetNameLen();
				}


				switch (cd.nSelectedCase)
				{
				case CChangeCaseDlg::Sentence:
					MakeLower(szName+1,iLength-1);
					MakeUpper(szName,1);
					break;
				case CChangeCaseDlg::Uppercase:
					MakeUpper(szName,iLength);
					break;
				case CChangeCaseDlg::Lowercase:
					MakeLower(szName,iLength);
					break;
				case CChangeCaseDlg::Title:
					for (int i=0;i<iLength;)
					{
						MakeUpper(szName+(i++),1);
						
						int nIndex=FirstCharIndex(szName+i,L' ');
						if (nIndex==-1)
						{
							MakeLower(szName+i,iLength-i);
							break;
						}
                        						
						MakeLower(szName+i,nIndex);
                        i+=nIndex+1;                        
					}
					break;
				case CChangeCaseDlg::Toggle:
					for (int i=0;i<iLength;i++)
					{
						if (IsCharUpper(szName[i]))
							MakeLower(szName+i,1);
						else if (IsCharLower(szName[i]))
							MakeUpper(szName+i,1);
					}
					break;
				}


				if (m_pActiveContextMenu!=NULL && m_pActiveContextMenu->bForParents)
					FileSystem::MoveFile(pItem->GetParent(),pItem->GetParent());
				else
					FileSystem::MoveFile(pItem->GetPath(),pItem->GetPath());
				pItem->RemoveFlags(LITEM_FILETITLEOK);
				m_pListCtrl->RedrawItems(iItem,iItem);
				

			}
			iItem=m_pListCtrl->GetNextItem(iItem,LVNI_SELECTED);
		}
		m_pListCtrl->UpdateWindow();
	}
}

void CLocateDlg::OnUpdateLocatedItem()
{
	int nItem=m_pListCtrl->GetNextItem(-1,LVNI_SELECTED);
	while (nItem!=-1)
	{
		CLocatedItem* pItem=(CLocatedItem*)m_pListCtrl->GetItemData(nItem);
		if (pItem!=NULL)
		{
			DWORD dwExtraTemp=m_dwExtraFlags;
			m_dwExtraFlags|=efEnableItemUpdating;

			for (int nDetail=0;nDetail<=LastType;nDetail++)
				pItem->UpdateByDetail((DetailType)nDetail);
			m_dwExtraFlags=dwExtraTemp;      

			m_pListCtrl->RedrawItems(nItem,nItem);
		}
	
        nItem=m_pListCtrl->GetNextItem(nItem,LVNI_SELECTED);
	}
}

void CLocateDlg::OnUpdateDatabasesOfSelectedFiles()
{
	// Collect database IDs	
	CArray<WORD> awDatabases;
	int nItem=m_pListCtrl->GetNextItem(-1,LVNI_SELECTED);
	while (nItem!=-1)
	{
		CLocatedItem* pItem=(CLocatedItem*)m_pListCtrl->GetItemData(nItem);
		if (pItem!=NULL)
		{
			WORD wID=pItem->GetDatabaseID();
			BOOL bFound=FALSE;
			for (int i=0;i<awDatabases.GetSize();i++)
			{
				if (awDatabases[i]==wID)
					bFound=TRUE;
			}
			if (!bFound)
				awDatabases.Add(wID);
		}
	
        nItem=m_pListCtrl->GetNextItem(nItem,LVNI_SELECTED);
	}

	// Find database names
	const CArray<PDATABASE>& raAllDatabases=GetLocateApp()->GetDatabases();
	int nValidDatabases=0,nStringLength=1,i;
	LPCWSTR* pDatabases=new LPCWSTR[awDatabases.GetSize()+1];
	for (i=0;i<awDatabases.GetSize();i++)
	{
		for (int j=0;j<raAllDatabases.GetSize();j++)
		{
			if (raAllDatabases[j]->GetID()==awDatabases[i])
			{
				pDatabases[nValidDatabases]=raAllDatabases[j]->GetName();
				nStringLength+=istrlen(pDatabases[nValidDatabases])+1;
				nValidDatabases++;
				break;
			}
		}
	}


	// Form zero separated database name string
	LPWSTR pString=new WCHAR[nStringLength],pPtr=pString;
	for (i=0;i<nValidDatabases;i++)
	{
		int nLen=istrlen(pDatabases[i])+1;
		MemCopyW(pPtr,pDatabases[i],nLen);
		pPtr+=nLen;
	}
	*pPtr='\0';

	// Start updating
	GetTrayIconWnd()->OnUpdate(FALSE,pString);

	delete[] pString;
	delete[] pDatabases;

}


void CLocateDlg::OnComputeMD5Sums(BOOL bForSameSizeFilesOnly)
{
	CWaitCursor e;
	if (bForSameSizeFilesOnly)
	{
		UINT uSelected=m_pListCtrl->GetSelectedCount();
		if (uSelected<2)
			return; 
        
		CLocatedItem** pItems=new CLocatedItem*[uSelected];
		int* pItemsID=new int[uSelected];

		int nItem=m_pListCtrl->GetNextItem(-1,LVNI_SELECTED);
		for (UINT i=0;i<uSelected;i++)
		{
			ASSERT(nItem!=-1);
			pItems[i]=(CLocatedItem*)m_pListCtrl->GetItemData(nItem);
			pItemsID[i]=nItem;
			nItem=m_pListCtrl->GetNextItem(nItem,LVNI_SELECTED);
		}

		for (UINT i=0;i<uSelected;i++)
		{
			if (pItems[i]==NULL)
				continue;

			BOOL bFound=FALSE;

			for (UINT j=i+1;j<uSelected;j++)
			{
				if (pItems[j]==NULL)
					continue;

				if (pItems[i]->GetFileSizeHi()==pItems[j]->GetFileSizeHi() &&
					pItems[i]->GetFileSizeLo()==pItems[j]->GetFileSizeLo())
				{
					bFound=TRUE;
					pItems[j]->ComputeMD5sum(TRUE);
					m_pListCtrl->RedrawItems(pItemsID[j],pItemsID[j]);
					pItems[j]=NULL;
				}
			}

			if (bFound)
			{
				pItems[i]->ComputeMD5sum(TRUE);
				m_pListCtrl->RedrawItems(pItemsID[i],pItemsID[i]);
				pItems[i]=NULL;
			}
		}

		delete[] pItems;
		delete[] pItemsID;
	}
	else
	{
		int nItem=m_pListCtrl->GetNextItem(-1,LVNI_SELECTED);
		while (nItem!=-1)
		{
			CLocatedItem* pItem=(CLocatedItem*)m_pListCtrl->GetItemData(nItem);
			if (pItem!=NULL)
			{
				pItem->ComputeMD5sum(TRUE);
				m_pListCtrl->RedrawItems(nItem,nItem);
			}
			nItem=m_pListCtrl->GetNextItem(nItem,LVNI_SELECTED);
		}
	
        
	}
}

void CLocateDlg::OnCopyMD5SumsToClipboard()
{
	CWaitCursor e;
	CStringW Text;
	int nSelectedFiles=0;
	
	// Count selected files
	int nItem=m_pListCtrl->GetNextItem(-1,LVNI_SELECTED);
	while (nItem!=-1)
	{
		CLocatedItem* pItem=(CLocatedItem*)m_pListCtrl->GetItemData(nItem);
		if (pItem!=NULL)
		{
			if (!pItem->IsFolder())
				nSelectedFiles++;
		}
		nItem=m_pListCtrl->GetNextItem(nItem,LVNI_SELECTED);
	}
	
	if (nSelectedFiles>0)
	{
		nItem=m_pListCtrl->GetNextItem(-1,LVNI_SELECTED);
		while (nItem!=-1)
		{
			CLocatedItem* pItem=(CLocatedItem*)m_pListCtrl->GetItemData(nItem);
			if (pItem!=NULL)
			{
				if (!pItem->IsFolder())
				{
					pItem->ComputeMD5sum(TRUE,TRUE);
					m_pListCtrl->RedrawItems(nItem,nItem);

					if (!Text.IsEmpty())
						Text << L"\r\n";

					if (nSelectedFiles>1)
						Text << pItem->GetName() << L' ';


					Text << pItem->GetExtraText(MD5sum);
				}
			}

			nItem=m_pListCtrl->GetNextItem(nItem,LVNI_SELECTED);
		}
	}

	
	if (IsUnicodeSystem())
	{
		HGLOBAL hMem=GlobalAlloc(GHND,(Text.GetLength()+1)*2);
		if (hMem==NULL)
		{
			ShowErrorMessage(IDS_ERRORCANNOTALLOCATE);
			return;
		}
		
		BYTE* pData=(BYTE*)GlobalLock(hMem);
		MemCopyW((LPWSTR)pData,LPCWSTR(Text),Text.GetLength()+1);
		GlobalUnlock(hMem);

		if (OpenClipboard())
		{
			EmptyClipboard();
			SetClipboardData(CF_UNICODETEXT,hMem);
			CloseClipboard();
		}
	}
	else
	{
		HGLOBAL hMem=GlobalAlloc(GHND,Text.GetLength()+1);
		if (hMem==NULL)
		{
			ShowErrorMessage(IDS_ERRORCANNOTALLOCATE);
			return;
		}

		BYTE* pData=(BYTE*)GlobalLock(hMem);
		WideCharToMultiByte(CP_ACP,0,LPCWSTR(Text),(int)Text.GetLength()+1,(LPSTR)pData,Text.GetLength()+1,NULL,NULL);
		GlobalUnlock(hMem);

		if (OpenClipboard())
		{
			EmptyClipboard();
			SetClipboardData(CF_TEXT,hMem);
			CloseClipboard();
		}

	}
}

void CLocateDlg::OnShowFileInformation()
{
	DWORD dwFiles=0,dwDirectories=0;
	LONGLONG llTotalSize=0;
	int nItem=m_pListCtrl->GetNextItem(-1,LVNI_SELECTED);
	while (nItem!=-1)
	{
		CLocatedItem* pItem=(CLocatedItem*)m_pListCtrl->GetItemData(nItem);
		if (pItem!=NULL)
		{
			if (pItem->IsFolder())
				dwDirectories++;
			else
			{
				dwFiles++;
				llTotalSize+=pItem->GetFileSize();
			}
		}

		nItem=m_pListCtrl->GetNextItem(nItem,LVNI_SELECTED);
	}

	if (dwFiles>0 || dwDirectories>0)
	{
		CStringW str;
		WCHAR number[200],*numberfmt=number;

		_ui64tow_s(llTotalSize,number,200,10);
		CRegKey RegKey;
		if (RegKey.OpenKey(HKCU,"Control Panel\\International",CRegKey::defRead)==ERROR_SUCCESS)
		{
			if (IsUnicodeSystem())
			{
				numberfmt=new WCHAR[200];
				WCHAR szTemp[10]=L".",szTemp2[10]=L" ";
				NUMBERFMTW fmt;
				
				// Defaults;
				fmt.NumDigits=0; 
				fmt.LeadingZero=1;
				fmt.Grouping=3; 
				fmt.lpDecimalSep=szTemp; 
				fmt.lpThousandSep=szTemp2; 
				fmt.NegativeOrder=1; 
				
				if (RegKey.QueryValue(L"iLZero",szTemp,10)>1)
					fmt.LeadingZero=_wtoi(szTemp);
				if (RegKey.QueryValue(L"sGrouping",szTemp,10)>1)
					fmt.Grouping=_wtoi(szTemp);
				RegKey.QueryValue(L"sDecimal",szTemp,10);
				RegKey.QueryValue(L"sThousand",szTemp2,10);

				
				if (GetNumberFormatW(LOCALE_USER_DEFAULT,0,number,&fmt,numberfmt,200)==0)
				{
					delete[] numberfmt;
					numberfmt=number;					
				}
			}
			else
			{
				char szBuffer[200],szTemp[10]=".",szTemp2[10]=" ";
				NUMBERFMT fmt;
				
				// Defaults;
				fmt.NumDigits=0; 
				fmt.LeadingZero=1;
				fmt.Grouping=3; 
				fmt.lpDecimalSep=szTemp; 
				fmt.lpThousandSep=szTemp2; 
				fmt.NegativeOrder=1; 
				
				if (RegKey.QueryValue("iLZero",szTemp,10)>1)
					fmt.LeadingZero=atoi(szTemp);
				if (RegKey.QueryValue("sGrouping",szTemp,10)>1)
					fmt.Grouping=atoi(szTemp);
				RegKey.QueryValue("sDecimal",szTemp,10);
				RegKey.QueryValue("sThousand",szTemp2,10);

				if (GetNumberFormat(LOCALE_USER_DEFAULT,0,W2A(number),&fmt,szBuffer,200)!=0)
					numberfmt=alloccopyAtoW(szBuffer);
			}
		}

		
		str.FormatEx(IDS_FILEINFORMATIONFMT,dwFiles,dwDirectories,numberfmt);
		if (numberfmt!=number)
			delete[] numberfmt;
		MessageBox(str,ID2W(IDS_FILEINFORMATION),MB_OK|MB_ICONINFORMATION);
	}
}

void CLocateDlg::OnSelectDetails()
{
	CSelectColumnsDlg dlg;
	

	int nColumns=m_pListCtrl->GetColumnCount();
	dlg.m_aSelectedCols.SetSize(m_pListCtrl->GetVisibleColumnCount());
	dlg.m_aWidths.SetSize(nColumns);
	dlg.m_aIDs.SetSize(nColumns);
	dlg.m_aAligns.SetSize(nColumns);
	dlg.m_aActions.SetSize(nColumns);
	
	m_pListCtrl->GetColumnOrderArray(m_pListCtrl->GetVisibleColumnCount(),
		dlg.m_aSelectedCols.GetData());

	

	LVCOLUMN lc;int iCol;
	lc.mask=LVCF_FMT|LVCF_WIDTH;
	for (iCol=0;iCol<nColumns;iCol++)
	{
		// Set column id
		dlg.m_aIDs[iCol]=m_pListCtrl->GetColumnID(iCol);
		m_pListCtrl->GetColumn(iCol,&lc);

		// Set column width
		dlg.m_aWidths[iCol]=lc.cx;

        // Set align
		dlg.m_aAligns[iCol]=(CSelectColumnsDlg::ColumnItem::Align)(lc.fmt&LVCFMT_JUSTIFYMASK);

		dlg.m_aActions[iCol]=new CSubAction*[ListActionCount];
		ZeroMemory(dlg.m_aActions[iCol],ListActionCount*sizeof(CSubAction*));
			
		if (dlg.m_aIDs[iCol]>LastType)
			continue;
		
		for (int nAct=0;nAct<ListActionCount;nAct++)
		{
			if (m_aResultListActions[dlg.m_aIDs[iCol]][nAct]!=NULL)
			{
				dlg.m_aActions[iCol][nAct]=new CSubAction(
					CAction::ResultListItems,*m_aResultListActions[dlg.m_aIDs[iCol]][nAct]);
			}
		}
        
	}

	if (!dlg.DoModal(*this))
		return; // Cancel
	
	m_pListCtrl->SetColumnOrderArray(dlg.m_aSelectedCols.GetSize(),
		dlg.m_aSelectedCols.GetData());

	ClearResultlistActions();

	for (iCol=0;iCol<nColumns;iCol++)
	{
		lc.mask=LVCF_FMT;
		m_pListCtrl->GetColumn(iCol,&lc);
			
		lc.mask=LVCF_FMT|LVCF_WIDTH;
		// Set column width
		lc.cx=dlg.m_aWidths[iCol];
        // Set align
		lc.fmt&=~LVCFMT_JUSTIFYMASK;
		lc.fmt|=dlg.m_aAligns[iCol];

		
		m_pListCtrl->SetColumn(iCol,&lc);

		if (dlg.m_aActions[iCol]==NULL)
			continue;

		for (int nAct=0;nAct<ListActionCount;nAct++)
		{
			
			if (dlg.m_aActions[iCol][nAct]!=NULL)
				m_aResultListActions[dlg.m_aIDs[iCol]][nAct]=dlg.m_aActions[iCol][nAct];
		}
		delete[] dlg.m_aActions[iCol];
	}

}



