/* Locate32 - Copyright (c) 1997-2010 Janne Huttunen */

#include <HFCLib.h>
#include "Locate32.h"


////////////////////////////////////////////////////////////
// CLocateDlg - Menu - General
////////////////////////////////////////////////////////////

void CLocateDlg::SetMenus()
{
	HMENU hOldMenu=GetMenu();
	HMENU hMenu=::LoadMenu(IDR_MAINMENU);
    	
	ClearMenuVariables();
	

	// Load popup menus
	if ((HMENU)m_Menu!=NULL)
		m_Menu.DestroyMenu();
	m_Menu.LoadMenu(IDR_POPUPMENU);
	ASSERT(::GetSubMenu(m_Menu.GetSubMenu(SUBMENU_EXTRACONTEXTMENUITEMS),SUBMENU_SPECIALMENU)!=NULL);


	HMENU hSubMenu=GetSubMenu(hMenu,2);
	if (GetLocateApp()->m_wComCtrlVersion<0x0600) // Disabling features
	{
		::EnableMenuItem(hSubMenu,IDM_ALIGNTOGRID,MF_GRAYED|MF_BYCOMMAND);
		::EnableMenuItem(hSubMenu,IDM_LINEUPICONS,MF_GRAYED|MF_BYCOMMAND);
	}

	if (hSubMenu!=NULL && !(m_dwThumbnailFlags&tfVistaFeaturesAvailable))
	{
		::SetMenuItemBitmaps(hSubMenu,IDM_EXTRALARGEICONS,MF_BYCOMMAND,NULL,m_CircleBitmap);
		::SetMenuItemBitmaps(hSubMenu,IDM_LARGEICONS,MF_BYCOMMAND,NULL,m_CircleBitmap);
		::SetMenuItemBitmaps(hSubMenu,IDM_MEDIUMICONS,MF_BYCOMMAND,NULL,m_CircleBitmap);
		::SetMenuItemBitmaps(hSubMenu,IDM_SMALLICONS,MF_BYCOMMAND,NULL,m_CircleBitmap);
		::SetMenuItemBitmaps(hSubMenu,IDM_LIST,MF_BYCOMMAND,NULL,m_CircleBitmap);
		::SetMenuItemBitmaps(hSubMenu,IDM_DETAILS,MF_BYCOMMAND,NULL,m_CircleBitmap);
	}

	CShortcut::ModifyMenus(m_aShortcuts,hMenu,m_Menu);

	SetMenu(hMenu);
	if (hOldMenu!=NULL)
		DestroyMenu(hOldMenu);

	SetMenuCheckMarkForListType();
}

void CLocateDlg::OnInitMainMenu(HMENU hPopupMenu,UINT nIndex)
{
	switch (nIndex)
	{
	case 1:
		{
			HWND hFocus=GetFocus();
			char szClass[100];
			::GetClassName(hFocus,szClass,100);
			if (_stricmp(szClass,"EDIT")==0)
			{
				EnableMenuItem(hPopupMenu,IDM_CUT,MF_BYCOMMAND|MF_ENABLED);
				EnableMenuItem(hPopupMenu,IDM_COPY,MF_BYCOMMAND|MF_ENABLED);
				EnableMenuItem(hPopupMenu,IDM_PASTE,MF_BYCOMMAND|MF_ENABLED);
			} 
			else if (hFocus==*m_pListCtrl)
			{
				BOOL bEnable=m_pListCtrl->GetSelectedCount()>0;
				EnableMenuItem(hPopupMenu,IDM_CUT,MF_BYCOMMAND|(bEnable?MF_ENABLED:MF_GRAYED));
				EnableMenuItem(hPopupMenu,IDM_COPY,MF_BYCOMMAND|(bEnable?MF_ENABLED:MF_GRAYED));
				EnableMenuItem(hPopupMenu,IDM_PASTE,MF_BYCOMMAND|MF_GRAYED);
			} 
			else
			{
				EnableMenuItem(hPopupMenu,IDM_CUT,MF_BYCOMMAND|MF_GRAYED);
				EnableMenuItem(hPopupMenu,IDM_COPY,MF_BYCOMMAND|MF_GRAYED);
				EnableMenuItem(hPopupMenu,IDM_PASTE,MF_BYCOMMAND|MF_GRAYED);
			}
		}
		break;
	case 2:
		if (m_dwFlags&fgLargeMode)
			EnableMenuItem(hPopupMenu,IDM_REFRESH,MF_BYCOMMAND|MF_ENABLED);
		else
			EnableMenuItem(hPopupMenu,IDM_REFRESH,MF_BYCOMMAND|MF_GRAYED);
		break;

	}
}

void CLocateDlg::OnInitFileMenu(HMENU hPopupMenu)
{
	EnableMenuItem(hPopupMenu,IDM_GLOBALUPDATEDB,!GetLocateApp()->IsUpdating()?MF_BYCOMMAND|MF_ENABLED:MF_BYCOMMAND|MF_GRAYED);
	EnableMenuItem(hPopupMenu,IDM_UPDATEDATABASES,!GetLocateApp()->IsUpdating()?MF_BYCOMMAND|MF_ENABLED:MF_BYCOMMAND|MF_GRAYED);
	EnableMenuItem(hPopupMenu,IDM_STOPUPDATING,GetLocateApp()->IsUpdating()?MF_BYCOMMAND|MF_ENABLED:MF_BYCOMMAND|MF_GRAYED);

	int iDatabaseMenu=CLocateApp::GetDatabaseMenuIndex(hPopupMenu);
	if (iDatabaseMenu!=-1)
		EnableMenuItem(hPopupMenu,iDatabaseMenu,!GetLocateApp()->IsUpdating()?MF_BYPOSITION|MF_ENABLED:MF_BYPOSITION|MF_GRAYED);

	
	if (m_pListCtrl->GetSelectedCount())
	{
		EnableMenuItem(hPopupMenu,IDM_CREATESHORTCUT,MF_BYCOMMAND|MF_ENABLED);
		EnableMenuItem(hPopupMenu,IDM_DELETE,MF_BYCOMMAND|MF_ENABLED);
		EnableMenuItem(hPopupMenu,IDM_RENAME,MF_BYCOMMAND|MF_ENABLED);
		EnableMenuItem(hPopupMenu,IDM_PROPERTIES,MF_BYCOMMAND|MF_ENABLED);
		EnableMenuItem(hPopupMenu,IDM_OPENCONTAININGFOLDER,MF_BYCOMMAND|MF_ENABLED);
	}
	else
	{
		EnableMenuItem(hPopupMenu,IDM_CREATESHORTCUT,MF_BYCOMMAND|MF_GRAYED);
		EnableMenuItem(hPopupMenu,IDM_RENAME,MF_BYCOMMAND|MF_GRAYED);
		EnableMenuItem(hPopupMenu,IDM_DELETE,MF_BYCOMMAND|MF_GRAYED);
		EnableMenuItem(hPopupMenu,IDM_PROPERTIES,MF_BYCOMMAND|MF_GRAYED);
		EnableMenuItem(hPopupMenu,IDM_OPENCONTAININGFOLDER,MF_BYCOMMAND|MF_GRAYED);
	}
}

void CLocateDlg::OnInitSendToMenu(HMENU hPopupMenu)
{
	// Removing default items
	for(int i=GetMenuItemCount(hPopupMenu)-1;i>=0;i--)
	{
		DebugCloseGdiObject(hPopupMenu);
		DeleteMenu(hPopupMenu,i,MF_BYPOSITION);
	}

	if (m_hSendToListFont!=NULL)
	{
		DebugCloseGdiObject(m_hSendToListFont);
		DeleteObject(m_hSendToListFont);
	}
	
	// Initializing fonts
	CRegKey RegKey;
	if (RegKey.OpenKey(HKCU,"Control Panel\\Desktop\\WindowMetrics",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
	{
		LOGFONTW font;
		RegKey.QueryValue("MenuFont",(LPSTR)&font,sizeof(LOGFONTW));
		m_hSendToListFont=CreateFontIndirectW(&font);
	}
	else
		m_hSendToListFont=(HFONT)GetStockObject(DEFAULT_GUI_FONT);

	AddSendToMenuItems(CMenu(hPopupMenu),NULL,IDM_DEFSENDTOITEM);
}



////////////////////////////////////////////////////////////
// CLocateDlg - Menu - Context menu/file menu helpers
////////////////////////////////////////////////////////////

inline CLocateDlg::ContextMenuInformation::~ContextMenuInformation()
{
	ReleaseShellInterfaces();

	// Releasing memory
	if (hPopupMenu!=NULL && bFreeMenu)
		DestroyMenu(hPopupMenu);
	
}


inline void CLocateDlg::ContextMenuInformation::ReleaseShellInterfaces()
{
	CheckThread();
	
	if (pContextMenu3!=NULL)
	{
		pContextMenu3->Release();
		pContextMenu3=NULL;
	}
	if (pContextMenu2!=NULL)
	{
		pContextMenu2->Release();
		pContextMenu2=NULL;
	}
	if (pContextMenu!=NULL)
	{
		pContextMenu->Release();
		pContextMenu=NULL;
	}
	if (pParentFolder!=NULL)
	{
		pParentFolder->Release();
		pParentFolder=NULL;
	}

}


BOOL CLocateDlg::ContextMenuInformation::AddContextMenuItems(UINT uFirstID,UINT uLastID,UINT uFlags)
{
	ASSERT_VALID(pContextMenu);

	HRESULT hRes;
	if (pContextMenu2!=NULL)
	{
		try {
			hRes=pContextMenu2->QueryContextMenu(hPopupMenu,0,uFirstID,uLastID,uFlags);
			
		}
		catch (...)
		{
			return FALSE;
		}

		if (!SUCCEEDED(hRes))
			return FALSE;


		return TRUE;
	}

	
	try {
		hRes=pContextMenu->QueryContextMenu(hPopupMenu,0,uFirstID,uLastID,uFlags);
	}
	catch (...)
	{
		return FALSE;
	}

	return (SUCCEEDED(hRes));
	
}

BOOL CLocateDlg::CreateFileContextMenu(HMENU hFileMenu,CLocatedItem** pItems,int nItems,BOOL bSimple,BOOL bForParents)
{
	// Free old structure and create new
	ClearMenuVariables();
	m_pActiveContextMenu=new ContextMenuInformation;
	m_pActiveContextMenu->bForParents=bForParents;
	
	if (hFileMenu!=NULL)
	{
		CMenu FileMenu(hFileMenu);

		m_pActiveContextMenu->hPopupMenu=hFileMenu;
		m_pActiveContextMenu->bFreeMenu=FALSE;

		// Removing all items
		for (int i=FileMenu.GetMenuItemCount()-1;i>=0;i--)
			FileMenu.DeleteMenu(i,MF_BYPOSITION);
		
		// Copying menu from template menu in resource
		if (nItems==0)
		{
			InsertMenuItemsFromTemplate(FileMenu,m_Menu.GetSubMenu(SUBMENU_FILEMENUNOITEMS),0);
			return TRUE;
		}
		InsertMenuItemsFromTemplate(FileMenu,m_Menu.GetSubMenu(SUBMENU_FILEMENU),0);
		
	}
	
	if (!bSimple)
	{
		// Creating context menu for file items
		if (GetContextMenuForItems(m_pActiveContextMenu,nItems,pItems))
		{
			m_pActiveContextMenu->CheckThread();

			// IContextMenu interface has created succesfully,
			// so they can insert their own menu items

			UINT uFlags=CMF_EXPLORE|CMF_CANRENAME;
			if (m_pActiveContextMenu->hPopupMenu==NULL)
			{
				m_pActiveContextMenu->hPopupMenu=CreatePopupMenu();
				m_pActiveContextMenu->bFreeMenu=TRUE;
				
				if (HIBYTE(GetKeyState(VK_SHIFT)))
					uFlags|=CMF_EXTENDEDVERBS;
			}
			else
				uFlags|=CMF_VERBSONLY;
			
			if (m_pActiveContextMenu->AddContextMenuItems(IDM_DEFCONTEXTITEM,IDM_DEFSENDTOITEM,uFlags))
			{
				// Insert special menu, ...
				InsertMenuItemsFromTemplate(CMenu(m_pActiveContextMenu->hPopupMenu),m_Menu.GetSubMenu(SUBMENU_EXTRACONTEXTMENUITEMS),0);

				DeleteImpossibleItemsInContextMenu(pItems,nItems);
				return TRUE;
			}

			// Didn't succees, release interfaces
			m_pActiveContextMenu->ReleaseShellInterfaces();
		}
	} 


	if (m_pActiveContextMenu->hPopupMenu==NULL)
	{
		CMenu Menu;
		Menu.CreatePopupMenu();
		InsertMenuItemsFromTemplate(Menu,m_Menu.GetSubMenu(SUBMENU_CONTEXTMENUPLAIN),0,IDM_DEFOPEN);
		InsertMenuItemsFromTemplate(Menu,m_Menu.GetSubMenu(SUBMENU_EXTRACONTEXTMENUITEMS),0);


		m_pActiveContextMenu->hPopupMenu=Menu;
		m_pActiveContextMenu->bFreeMenu=TRUE;
	}
	else {
		InsertMenuItemsFromTemplate(CMenu(m_pActiveContextMenu->hPopupMenu),m_Menu.GetSubMenu(SUBMENU_OPENITEMFORFILEMENU),0);
		InsertMenuItemsFromTemplate(CMenu(m_pActiveContextMenu->hPopupMenu),m_Menu.GetSubMenu(SUBMENU_EXTRACONTEXTMENUITEMS),0);
	}
		

	DeleteImpossibleItemsInContextMenu(pItems,nItems);
	return TRUE;
	
}

BOOL CLocateDlg::GetContextMenuForItems(CLocateDlg::ContextMenuInformation* pContextMenuInfo,int nItems,CLocatedItem** ppItems)
{
	if (m_pDesktopFolder==NULL)
		return FALSE;
	
	if (nItems==0)
		return FALSE;

	LPITEMIDLIST* ppFullIDLs=new LPITEMIDLIST[nItems];
	LPITEMIDLIST pParentIDL;

	int iItem=0;
	for (int i=0;i<nItems;i++)
	{
		if (SUCCEEDED(m_pDesktopFolder->ParseDisplayName(*this,NULL,
			pContextMenuInfo->bForParents?ppItems[i]->GetParent():ppItems[i]->GetPath(),NULL,&ppFullIDLs[iItem],NULL)))
			iItem++;
	}
	nItems=iItem;

	if (nItems==0)
	{
		delete[] ppFullIDLs;
		return FALSE;
	}

	LPITEMIDLIST* ppSimpleIDLs=new LPITEMIDLIST[nItems];
	int nParentIDLLevel=-1;

	BOOL bRet=FALSE;
	if (GetSimpleIDLsandParentfromIDLs(nItems,ppFullIDLs,&pParentIDL,ppSimpleIDLs,&nParentIDLLevel))
	{
		bRet=GetContextMenuForFiles(pContextMenuInfo,nItems,pParentIDL,ppSimpleIDLs,nParentIDLLevel);

		// Free ID lists
		CoTaskMemFree(pParentIDL);
		for (int i=0;i<nItems;i++)
			CoTaskMemFree(ppSimpleIDLs[i]);
		delete[] ppSimpleIDLs;
	}

				
	// Free ID lists
	for (int i=0;i<nItems;i++)
		CoTaskMemFree(ppFullIDLs[i]);
	delete[] ppFullIDLs;
	
	return bRet;
}
	
	
BOOL CLocateDlg::GetContextMenuForFiles(CLocateDlg::ContextMenuInformation* pContextMenuInfo,int nItems,LPITEMIDLIST pParentIDL,LPITEMIDLIST* ppSimpleIDLs,int nParentIDLlevel)
{
	ASSERT(pContextMenuInfo!=NULL);

	if (m_pDesktopFolder==NULL)
		return FALSE;

	if (nParentIDLlevel<=1)
		return FALSE; // Not enough?
	
	ASSERT(nItems!=0);


	pContextMenuInfo->nIDLParentLevel=nParentIDLlevel;
	
	// Querying IShellFolder interface for parent
	HRESULT hRes=m_pDesktopFolder->BindToObject(pParentIDL,NULL,IID_IShellFolder,(void**)&pContextMenuInfo->pParentFolder);
	if (!SUCCEEDED(hRes))
		return FALSE;
	
	// Check if AddRef fix is enabled
	DWORD bForceAddRef=GetSystemFeaturesFlag()&efWinVista?FALSE:TRUE;
	CRegKey2 RegKey;
	if (RegKey.OpenKey(HKCU,"\\General",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
	{
		RegKey.QueryValue("EnableAddRefFix",bForceAddRef);
		RegKey.CloseKey();
	}
	

	pContextMenuInfo->pParentFolder->GetUIObjectOf(*this,nItems,(LPCITEMIDLIST*)ppSimpleIDLs,
		IID_IContextMenu,NULL,(void**)&pContextMenuInfo->pContextMenu);
	if (!SUCCEEDED(hRes))
		return FALSE;
	
	if (bForceAddRef)
	{
		// Force call AddRef?, QueryInterface SHOULD call this, but for some reason, 
		// this helps much 
		pContextMenuInfo->pContextMenu->AddRef();
	}
	
	
	hRes=pContextMenuInfo->pContextMenu->QueryInterface(IID_IContextMenu2,(void**)&pContextMenuInfo->pContextMenu2);
	if (SUCCEEDED(hRes))
	{
		// Force call AddRef?
		if (bForceAddRef)
			pContextMenuInfo->pContextMenu2->AddRef();
	}
	else
		pContextMenuInfo->pContextMenu2=NULL;

	hRes=pContextMenuInfo->pContextMenu->QueryInterface(IID_IContextMenu3,(void**)&pContextMenuInfo->pContextMenu3);
	if (SUCCEEDED(hRes))
	{
		// QueryInterface should call this, but for some reason, this helps much 
		if (bForceAddRef)
			pContextMenuInfo->pContextMenu3->AddRef();
	}
	else
		pContextMenuInfo->pContextMenu3=NULL;

	
	return TRUE;
} 



BOOL CLocateDlg::HandleShellCommands(WORD wID)
{
	if (m_pActiveContextMenu==NULL || 
		!(wID>=IDM_DEFCONTEXTITEM && wID<IDM_DEFCONTEXTITEM+1000))
		return FALSE;

	if (m_pListCtrl->GetSelectedCount()==0)
		return FALSE;


	ASSERT_VALID(m_pActiveContextMenu->hPopupMenu);

	CWaitCursor wait;
	
	
	m_pActiveContextMenu->CheckThread();
	
	WCHAR szVerb[221];  
	
	if (IsUnicodeSystem())
	{
		if (m_pActiveContextMenu->pContextMenu->GetCommandString(wID-IDM_DEFCONTEXTITEM,
			GCS_VERBW,NULL,(LPSTR)szVerb,200)!=NOERROR)
			szVerb[0]=L'\0';
	}
	else
	{
		char szVerbA[401];   // Some stupid context menu handlers tries to put help text as UNICODE anyway
		if (m_pActiveContextMenu->pContextMenu->GetCommandString(wID-IDM_DEFCONTEXTITEM,
			GCS_VERBA,NULL,szVerbA,200)!=NOERROR)
			szVerb[0]=L'\0';
		else
			MultiByteToWideChar(CP_ACP,0,szVerbA,-1,szVerb,401);
	}

	// Overriding these command, works better
	if (wcscmp(szVerb,L"open")==0 && !m_pActiveContextMenu->bForParents)
	{
		// Opening URLs to Avant Browser fails if this is not handled this way.
		// However, I'm not sure that do this way work in all cases.
		OnExecuteFile(szVerb);
		ClearMenuVariables();
		return TRUE;
	}
	if (wcscmp(szVerb,L"copy")==0)
	{
		OnCopy(FALSE);
		ClearMenuVariables();
		return TRUE;
	}
	if (wcscmp(szVerb,L"cut")==0)
	{
		OnCopy(TRUE);
		ClearMenuVariables();
		return  TRUE;
	}
	if (wcscmp(szVerb,L"link")==0)
	{
		OnCreateShortcut();
		ClearMenuVariables();
		return TRUE;
	}
	if (wcscmp(szVerb,L"delete")==0)
	{
		OnDelete();
		ClearMenuVariables();
		return TRUE;
	}
	if (wcscmp(szVerb,L"rename")==0)
	{
		OnRenameFile();
		ClearMenuVariables();
		return TRUE;
	}
	if (wcscmp(szVerb,L"properties")==0 && m_pActiveContextMenu->nIDLParentLevel<=1)
	{
		ClearMenuVariables();
		
		int nItems;
		CLocatedItem** pItems=GetSelectedItems(nItems);
		CPropertiesSheet* fileprops=new CPropertiesSheet(nItems,pItems,m_pActiveContextMenu->bForParents);
		delete[] pItems;
		fileprops->Open();
		return TRUE;
	}


	int nSelected;
	CAutoPtrA<CLocatedItem*> pItems=GetSelectedItems(nSelected);

	CMINVOKECOMMANDINFOEX cii;
	ZeroMemory(&cii,sizeof(CMINVOKECOMMANDINFOEX));
	cii.cbSize=sizeof(CMINVOKECOMMANDINFOEX);
	cii.fMask=CMIC_MASK_UNICODE;
	cii.hwnd=*this;
	cii.lpVerb=(LPCSTR)MAKELONG(wID-IDM_DEFCONTEXTITEM,0);
	cii.lpVerbW=(LPCWSTR)MAKELONG(wID-IDM_DEFCONTEXTITEM,0);
	cii.nShow=SW_SHOWDEFAULT;
	if (nSelected==1)
	{
		cii.lpDirectoryW=pItems[0]->GetParent();
		cii.lpDirectory=alloccopyWtoA(cii.lpDirectoryW);
	}
	HRESULT hRes=m_pActiveContextMenu->pContextMenu->InvokeCommand((CMINVOKECOMMANDINFO*)&cii);
	if (cii.lpDirectory!=NULL)
		delete[] (LPSTR)cii.lpDirectory;

	if (!SUCCEEDED(hRes))
	{
		// Try shell execute
		for (int i=0;i<nSelected;i++)
			ShellFunctions::ShellExecute(*this,szVerb[0]!='\0'?szVerb:NULL,pItems[i]->GetPath(),NULL,pItems[i]->GetParentSafe(),SW_SHOW);
	}
	
	
	//ClearMenuVariables();
	return TRUE;
}


void CLocateDlg::ClearMenuVariables()
{
	if (m_pActiveContextMenu!=NULL)
	{
		m_pActiveContextMenu->CheckThread();
		
		if (m_pActiveContextMenu->hPopupMenu!=NULL)
			FreeSendToMenuItems(m_pActiveContextMenu->hPopupMenu);
		
		delete m_pActiveContextMenu;
		m_pActiveContextMenu=NULL;
	}
	
}

void CLocateDlg::DeleteImpossibleItemsInContextMenu(CLocatedItem** pItems,int nItems)
{
	// Check if only folders are selected and, if so, disable menu items which 
	// which requires files

	ASSERT_VALID(m_pActiveContextMenu);
	ASSERT_VALID(m_pActiveContextMenu->hPopupMenu);

	BOOL bFoldersOnly=TRUE;
	if (!m_pActiveContextMenu->bForParents)
	{
		for (int i=0;i<nItems;i++)
		{
			if (!pItems[i]->IsFolder())
			{
				bFoldersOnly=FALSE;
				break;
			}
		}
	}

	if (bFoldersOnly)
	{
		DeleteMenu(m_pActiveContextMenu->hPopupMenu,IDM_COMPUTEMD5SUM,MF_BYCOMMAND);
		DeleteMenu(m_pActiveContextMenu->hPopupMenu,IDM_MD5SUMSFORSAMESIZEFILES,MF_BYCOMMAND);
		DeleteMenu(m_pActiveContextMenu->hPopupMenu,IDM_COPYMD5SUMTOCLIPBOARD,MF_BYCOMMAND);
	}

}


////////////////////////////////////////////////////////////
// CLocateDlg - Menu - Send To menu helpers
////////////////////////////////////////////////////////////

UINT CLocateDlg::AddSendToMenuItems(CMenu& Menu,LPITEMIDLIST pIDListToPath,UINT wStartID)
{
	MENUITEMINFOW mi;
	mi.cbSize=sizeof(MENUITEMINFOW);
	mi.fMask=MIIM_DATA|MIIM_ID|MIIM_STATE|MIIM_TYPE|MIIM_SUBMENU;
	mi.fType=MFT_OWNERDRAW;
	mi.fState=MFS_ENABLED;
	mi.wID=wStartID;
	
			
	BOOL bUseFileFind=TRUE;
	if (m_pDesktopFolder!=NULL)
	{
		HRESULT hRes;
		IShellFolder* psf=NULL;
		BOOL bFreeIDList=FALSE;

		DWORD dwAddLogicalDrives=0;

		if (pIDListToPath==NULL)
		{
			hRes=SHGetSpecialFolderLocation(*this,CSIDL_SENDTO,&pIDListToPath);
			if (SUCCEEDED(hRes)) 
			{
				dwAddLogicalDrives=GetLogicalDrives();
				bFreeIDList=TRUE;
			}
			else
				pIDListToPath=NULL;
		}


		if (pIDListToPath!=NULL)
		{
			hRes=m_pDesktopFolder->BindToObject(pIDListToPath,NULL,IID_IShellFolder,(LPVOID *)&psf);
			if (FAILED(hRes)) 
				psf=NULL;
		}


		
		if (psf!=NULL)
		{
			IEnumIDList* peidl;
			DWORD dwParentListSize=ShellFunctions::GetIDListSize(pIDListToPath)-sizeof(WORD);

			hRes=psf->EnumObjects(NULL,SHCONTF_FOLDERS|SHCONTF_NONFOLDERS,&peidl);
			if (SUCCEEDED(hRes))
			{
				LPITEMIDLIST pidl,pidlFull;
				
				while (peidl->Next(1,&pidl,NULL)==S_OK)
				{
					DWORD dwListSize=ShellFunctions::GetIDListSize(pidl);
					// Form full ID list
					pidlFull=(LPITEMIDLIST)CoTaskMemAlloc(dwParentListSize+dwListSize);
					CopyMemory(pidlFull,pIDListToPath,dwParentListSize);
					CopyMemory(((BYTE*)pidlFull)+dwParentListSize,pidl,dwListSize);
										
					mi.hSubMenu=NULL;
					BOOL bDontAdd=FALSE;
					
					SFGAOF aof=SFGAO_DROPTARGET|SFGAO_FOLDER|SFGAO_STREAM;
					hRes=psf->GetAttributesOf(1,(LPCITEMIDLIST*)&pidl,&aof);
					if (SUCCEEDED(hRes))
					{
						if (aof&SFGAO_DROPTARGET)
						{
							if (aof&SFGAO_FOLDER && !(aof&SFGAO_STREAM))
							{
								CMenu Menu;
								Menu.CreateMenu();
								mi.wID+=AddSendToMenuItems(Menu,pidlFull,mi.wID);
								mi.hSubMenu=Menu;
							}
						}
						else 
							bDontAdd=TRUE;
					}
					
					if (!bDontAdd)
					{
						if (dwAddLogicalDrives)
						{
							if (IsUnicodeSystem())
							{
								IShellLinkW* psl;
								hRes=psf->GetUIObjectOf(*this,1,(LPCITEMIDLIST*)&pidl,IID_IShellLinkW,NULL,(void**)&psl);
								if (FAILED(hRes))
								{
									// Alternative way
									STRRET str;
									hRes=psf->GetDisplayNameOf(pidl,SHGDN_FORPARSING,&str);
									if (SUCCEEDED(hRes))
									{
										LPWSTR pStr=ShellFunctions::StrRetToPtrW(str,pidl);
										if (pStr!=NULL)
										{
											hRes=CoCreateInstance(CLSID_ShellLink,NULL,CLSCTX_INPROC_SERVER,IID_IShellLinkW,(void**)&psl);
											if (SUCCEEDED(hRes))
											{
												IPersistFile* ppf;
												hRes=psl->QueryInterface(IID_IPersistFile,(void**)&ppf);
												if (SUCCEEDED(hRes))
												{
													hRes=ppf->Load(pStr,STGM_READ);
													ppf->Release();
												}

												if (FAILED(hRes))
													psl->Release();
											}
											delete[] pStr;
										}
										else
											hRes=E_FAIL;
									}

									
								}
								
								if (SUCCEEDED(hRes))
								{
									WCHAR szPath[MAX_PATH];
									hRes=psl->GetPath(szPath,MAX_PATH,0,0);
									if (hRes==NO_ERROR)
									{
										if (szPath[1]==L':' && 
											(szPath[2]==L'\0' || (szPath[2]==L'\\' && szPath[3]==L'\0')))
										{
											CharUpperBuffW(szPath,1);
											dwAddLogicalDrives&=~(1<<BYTE(szPath[0]-L'A'));
										}
									}
									psl->Release();
								}
							}
							else
							{
								IShellLinkA* psl;
								hRes=psf->GetUIObjectOf(*this,1,(LPCITEMIDLIST*)&pidl,IID_IShellLinkA,NULL,(void**)&psl);
								if (FAILED(hRes))
								{
									// Alternative way
									STRRET str;
									hRes=psf->GetDisplayNameOf(pidl,SHGDN_FORPARSING,&str);
									if (SUCCEEDED(hRes))
									{
										LPWSTR pStr=ShellFunctions::StrRetToPtrW(str,pidl);
										if (pStr!=NULL)
										{
											hRes=CoCreateInstance(CLSID_ShellLink,NULL,CLSCTX_INPROC_SERVER,IID_IShellLink,(void**)&psl);
											if (SUCCEEDED(hRes))
											{
												IPersistFile* ppf;
												hRes=psl->QueryInterface(IID_IPersistFile,(void**)&ppf);
												if (SUCCEEDED(hRes))
												{
													hRes=ppf->Load(pStr,STGM_READ);
													ppf->Release();
												}

												if (FAILED(hRes))
													psl->Release();

											}
											delete[] pStr;
										}
										else
											hRes=E_FAIL;
									}
								}

								
								if (SUCCEEDED(hRes))
								{
									char szPath[MAX_PATH];
									hRes=psl->GetPath(szPath,MAX_PATH,0,0);
									if (hRes==NO_ERROR)
									{
										if (szPath[1]==':' && 
											(szPath[2]=='\0' || (szPath[2]=='\\' && szPath[3]=='\0')))
										{
											CharUpperBuffA(szPath,1);
											dwAddLogicalDrives&=~(1<<BYTE(szPath[0]-'A'));
										}
									}
									psl->Release();
								}
							}

							
						}
						DebugOpenMemBlock((LPVOID)pidlFull);
						mi.dwItemData=(DWORD)pidlFull;

						Menu.InsertMenu(mi.wID,FALSE,&mi);
						mi.wID++;
					}
					else
						CoTaskMemFree(pidlFull);

					CoTaskMemFree(pidl);
					
					
				}
				peidl->Release();

				if (dwAddLogicalDrives)
				{
					// Add removable drives
					char drive[]="X:\\";
					for (int i=0;i<='Z'-'A';i++)
					{
						if (dwAddLogicalDrives&(1<<i))
						{
							drive[0]='A'+i;
							DWORD dwDriveType=FileSystem::GetDriveType(drive);
							if (dwDriveType==DRIVE_REMOVABLE)
							{
								mi.hSubMenu=NULL;
								mi.dwItemData=(DWORD)ShellFunctions::GetIDList(drive);
								DebugOpenMemBlock((LPVOID)mi.dwItemData);
									
								if (mi.dwItemData!=NULL)
								{
									Menu.InsertMenu(mi.wID,FALSE,&mi);
									mi.wID++;
								}
							}
						}
					}

					// Determine IMAPI burning device
					CArrayFAP<LPWSTR> Burners;
					if (GetIMAPIBurningDevices(Burners))
					{
						for (int i=0;i<Burners.GetSize();i++)
						{
							mi.hSubMenu=NULL;
							mi.dwItemData=(DWORD)ShellFunctions::GetIDList(Burners[i]);
							DebugOpenMemBlock((LPVOID)mi.dwItemData);

							if (mi.dwItemData!=NULL)
							{
								Menu.InsertMenu(mi.wID,FALSE,&mi);
								mi.wID++;
							}
						}
					}
				}
				bUseFileFind=FALSE;
			}
		}

		if (bFreeIDList)
			CoTaskMemFree(pIDListToPath);
	}
	
	if (bUseFileFind)
	{
		CFileFind Find;
		BOOL bErr;

		WCHAR szSendToPath[MAX_PATH];
		if (pIDListToPath==NULL)
		{
			// Resolving Send To -directory location
			CRegKey RegKey;
			if (RegKey.OpenKey(HKCU,"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
			{
				RegKey.QueryValue(L"SendTo",szSendToPath,MAX_PATH);
				RegKey.CloseKey();
			}
		}
		else
		{
			if (IsUnicodeSystem())
				SHGetPathFromIDListW(pIDListToPath,szSendToPath);
			else
			{
				char szSendToPathA[MAX_PATH];
				SHGetPathFromIDListW(pIDListToPath,szSendToPath);
				MultiByteToWideChar(CP_ACP,0,szSendToPathA,-1,szSendToPath,MAX_PATH);
			}
		}

		wcscat_s(szSendToPath,MAX_PATH,L"\\*.*");
		
		bErr=Find.FindFile(szSendToPath);
		while (bErr)
		{
			WCHAR szPath[MAX_PATH];
			Find.GetFileName(szPath,MAX_PATH);

			
			if (szPath[0]!=L'.' && !Find.IsSystem() && !Find.IsHidden())
			{
				Find.GetFilePath(szPath,MAX_PATH);
				mi.dwItemData=(DWORD)ShellFunctions::GetIDList(szPath);
				DebugOpenMemBlock((LPVOID)mi.dwItemData);
				
				if (Find.IsDirectory())
				{
					CMenu Menu;
					Menu.CreateMenu();
					mi.wID+=AddSendToMenuItems(Menu,(LPITEMIDLIST)mi.dwItemData,mi.wID);
					mi.hSubMenu=Menu;
				}
				else
					mi.hSubMenu=NULL;
				Menu.InsertMenu(mi.wID,FALSE,&mi);
				mi.wID++;
			}
			bErr=Find.FindNextFile();
		}
		Find.Close();
	}
	
	if (mi.wID==wStartID)
	{
		// Inserting default menu items
		ID2W EmptyTitle(IDS_EMPTY);
		mi.dwTypeData=(LPWSTR)(LPCWSTR)EmptyTitle;
		mi.dwItemData=0;
		mi.fState=MFS_GRAYED;
		mi.fType=MFT_STRING;
		Menu.InsertMenu(mi.wID,FALSE,&mi);
		mi.wID++;
	}
	return mi.wID-wStartID;
}

void CLocateDlg::FreeSendToMenuItems(HMENU hMenu)
{
	UINT wID=IDM_DEFSENDTOITEM;
	MENUITEMINFO mii;
	mii.cbSize=sizeof(MENUITEMINFO);
	mii.fMask=MIIM_DATA;
	while (GetMenuItemInfo(hMenu,wID,FALSE,&mii))
	{
		if (mii.dwItemData!=NULL)
		{
			DebugCloseMemBlock((LPVOID)mii.dwItemData);
			CoTaskMemFree((LPVOID)mii.dwItemData);
			mii.dwItemData=NULL;
			SetMenuItemInfo(hMenu,wID,FALSE,&mii);
		}
		wID++;
	}
}

int CLocateDlg::GetSendToMenuPos(HMENU hMenu)
{
	int nPos=0;
	UINT nID=GetMenuItemID(hMenu,nPos);
	while (nID!=IDM_CUT && nID!=IDM_CREATESHORTCUT)
	{
		if (nID==DWORD(-1))
		{		
			if (IsSendToMenu(GetSubMenu(hMenu,nPos)))
				return nPos;
		}
		nID=GetMenuItemID(hMenu,++nPos);
	}
	return nPos;
}


BOOL CLocateDlg::HandleSendToCommand(WORD wID)
{

	if (wID<IDM_DEFSENDTOITEM || wID>=IDM_DEFSENDTOITEM+1000)
		return FALSE;
		
	if (m_pDesktopFolder==NULL)
		return FALSE;

	ASSERT_VALID(m_pActiveContextMenu);
	ASSERT_VALID(m_pActiveContextMenu->hPopupMenu);

	CWaitCursor wait;
	MENUITEMINFO mii;
	mii.cbSize=sizeof(MENUITEMINFO);
	mii.fMask=MIIM_DATA;
	if (m_pActiveContextMenu!=NULL && m_pActiveContextMenu->hPopupMenu!=NULL)
		GetMenuItemInfo(m_pActiveContextMenu->hPopupMenu,wID,FALSE,&mii);
	else
		GetMenuItemInfo(GetSubMenu(GetMenu(),0),wID,FALSE,&mii);
	if (mii.dwItemData==NULL)
		return TRUE;

	IDropTarget* pdt=GetDropTarget((LPITEMIDLIST)mii.dwItemData);
	if (pdt==NULL)
		return TRUE;

	ASSERT_VALID(m_pActiveContextMenu);
	
	CFileObject *pfoSrc=new CFileObject;
	pfoSrc->AutoDelete();
	pfoSrc->AddRef();
	pfoSrc->SetFiles(m_pListCtrl,TRUE,m_pActiveContextMenu->bForParents);
		
	
	DWORD dwEffect=DROPEFFECT_COPY | DROPEFFECT_MOVE | DROPEFFECT_LINK;
	POINTL pt={0,0};
	
	HRESULT hRes=pdt->DragEnter(pfoSrc,MK_LBUTTON,pt,&dwEffect);
	if (SUCCEEDED(hRes) && dwEffect)
	{
		// Drop file
		hRes=pdt->Drop(pfoSrc,MK_LBUTTON,pt,&dwEffect);
	}
	else
	{
		// Drop target didn't like file object
		hRes = pdt->DragLeave();
	}

	// Releasing FileObjects
	pfoSrc->Release();
	
	pdt->Release();

	return TRUE;
}



////////////////////////////////////////////////////////////
// CLocateDlg - Menu - Misc funtions
////////////////////////////////////////////////////////////


BOOL CLocateDlg::InsertMenuItemsFromTemplate(CMenu& Menu,HMENU hTemplate,UINT uStartPosition,int nDefaultItem)
{
	CMenu Template(hTemplate);

	MENUITEMINFOW mii;
	WCHAR szName[1000];
	mii.cbSize=sizeof(MENUITEMINFOW);
	int nMenuLength=Template.GetMenuItemCount();
	for (int i=0;i<nMenuLength;i++)
	{
		mii.fMask=MIIM_ID|MIIM_TYPE|MIIM_STATE|MIIM_SUBMENU;
		mii.dwTypeData=szName;
		mii.cch=1000;
		
		// Checking whether popupmenu is popup menu or item
		if (!Template.GetMenuItemInfo(i,TRUE,&mii))
			return FALSE;

		if (mii.wID==nDefaultItem)
			mii.fState|=MFS_DEFAULT;

		if (mii.hSubMenu!=NULL)
		{
			// It is popup menu
            CMenu NewMenu;
			NewMenu.CreatePopupMenu();
			if (!InsertMenuItemsFromTemplate(NewMenu,CMenu(mii.hSubMenu),0))
			{
				NewMenu.DestroyMenu();
				return FALSE;
			}
			mii.hSubMenu=NewMenu;
			DebugCloseHandle(dhtMenu,mii.hSubMenu,STRNULL);
		}
		
		if (!Menu.InsertMenu(i+uStartPosition,TRUE,&mii))
		{
			if (mii.hSubMenu!=NULL)
				DestroyMenu(mii.hSubMenu);
			return FALSE;
		}
	}
	return TRUE;
}

