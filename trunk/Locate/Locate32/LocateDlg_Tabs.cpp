/* Locate32 - Copyright (c) 1997-2010 Janne Huttunen */

#include <HFCLib.h>
#include "Locate32.h"

typedef HRESULT (__stdcall * PFNSHGETFOLDERPATH)(HWND, int, HANDLE, DWORD, LPWSTR);  // "SHGetFolderPathW"

//////////////////////////////////////////////////////////////////////////////
// CLocateDlg::CNameDlg
//////////////////////////////////////////////////////////////////////////////


CLocateDlg::CNameDlg::~CNameDlg()
{
	DeleteCriticalSection(&m_cBrowse);
}

BOOL CLocateDlg::CNameDlg::OnInitDialog(HWND hwndFocus)
{
	CDialog::OnInitDialog(hwndFocus);
	GetLocateDlg()->AddInstantSearchingFlags(isIgnoreChangeMessages);

	m_Name.AttachToDlgItem(*this,IDC_NAME);
	m_Type.AttachToDlgItem(*this,IDC_TYPE);
	m_LookIn.AttachToDlgItem(*this,IDC_LOOKIN);

		
	LoadRegistry();
	//InitDriveBox(TRUE);

	RECT rc1,rc2;
	GetWindowRect(&rc1);
	m_Name.GetWindowRect(&rc2);
	m_nFieldLeft=WORD(rc2.left-rc1.left);
	
	::GetWindowRect(GetDlgItem(IDC_BROWSE),&rc2);
	m_nButtonWidth=WORD(rc2.right-rc2.left);
	m_nBrowseTop=WORD(rc2.top-rc1.top);

	::GetWindowRect(GetDlgItem(IDC_NOSUBDIRECTORIES),&rc2);
	m_nCheckWidth=WORD(rc2.right-rc2.left);
	

	::GetWindowRect(GetDlgItem(IDC_MOREDIRECTORIES),&rc2);	
	m_nMoreDirsTop=WORD(rc2.top-rc1.top);
	m_nMoreDirsWidth=BYTE(rc2.right-rc2.left);

	// Subclassing "Named:" combo
	NameControlData* pNameData=new NameControlData;
	pNameData->pLocateDlg=GetLocateDlg();
	pNameData->pNameCombo=&m_Name;
	if (IsUnicodeSystem())
		pNameData->pOldWndProc=(WNDPROC)m_Name.SetWindowLong(gwlWndProc,(LONG_PTR)NameWindowProc);

	if (pNameData->pOldWndProc==NULL)
	{
		// Subclassing didn't success
		delete pNameData;
	}
	else
		m_Name.SetWindowLong(gwlUserData,(LONG_PTR)pNameData);

	EnumChildWindows(m_Name,EnumAndSubclassNameChilds,(LPARAM)&m_Name);
	
	GetLocateDlg()->RemoveInstantSearchingFlags(isIgnoreChangeMessages);
	
	
	return FALSE;
}

		

BOOL CALLBACK CLocateDlg::CNameDlg::EnumAndSubclassNameChilds(HWND hwnd,LPARAM lParam)
{
	// Subclassing "Named:" combo
	NameControlData* pNameData=new NameControlData;
	pNameData->pLocateDlg=GetLocateDlg();
	pNameData->pNameCombo=(CComboBox*)lParam;
	if (IsUnicodeSystem())
		pNameData->pOldWndProc=(WNDPROC)::SetWindowLongPtrW(hwnd,GWLP_WNDPROC,(LONG_PTR)NameWindowProc);
	else
		pNameData->pOldWndProc=(WNDPROC)::SetWindowLongPtrA(hwnd,GWLP_WNDPROC,(LONG_PTR)NameWindowProc);

	if (pNameData->pOldWndProc==NULL)
	{
		// Subclassing didn't success
		delete pNameData;
	}
	else
		::SetWindowLongPtr(hwnd,GWLP_USERDATA,(LONG_PTR)pNameData);

	return TRUE;
}


void CLocateDlg::CNameDlg::ChangeNumberOfItemsInLists(int iNumberOfNames,int iNumberOfTypes,int iNumberOfDirectories)
{
	if (iNumberOfNames!=-1)
	{
		if (iNumberOfNames<=0)
		{
			m_Name.ResetContent();
			m_nMaxNamesInList=0;
		}
		else if (iNumberOfNames!=m_nMaxNamesInList)
		{
			while (m_Name.GetCount()>iNumberOfNames)
				m_Name.DeleteString(iNumberOfNames);
			m_nMaxNamesInList=iNumberOfNames;
		}
	}

	if (iNumberOfTypes!=-1)
	{
		if (iNumberOfTypes<=0)
		{
			m_Type.ResetContent();
			m_Type.AddString(ID2A(IDS_NOEXTENSION));
			m_nMaxTypesInList=0;
		}
		else if (iNumberOfTypes!=m_nMaxTypesInList)
		{
			while (m_Type.GetCount()>iNumberOfTypes+1)
				m_Type.DeleteString(iNumberOfTypes+1);
			m_nMaxTypesInList=iNumberOfTypes;
		}
	}

	if (iNumberOfDirectories!=-1)
	{
		EnterCriticalSection(&m_cBrowse);
		if (iNumberOfDirectories<=0)
		{
			if (m_pBrowse!=NULL)
			{
				for (int i=0;i<m_LookIn.GetCount();i++)
				{
					LPARAM lParam=m_LookIn.GetItemData(i);
					if (LOWORD(lParam)==Custom)
					{
						m_LookIn.DeleteItem(i);
						i--;
					}
				}
				
				for (int i=0;i<int(m_nMaxBrowse);i++)
					m_pBrowse[i].Empty();
				delete[] m_pBrowse;
				m_pBrowse=NULL;
			}
			m_nMaxBrowse=0;
		}
		else if (iNumberOfDirectories!=m_nMaxBrowse)
		{
			CStringW* pBrowseNew=new CStringW[iNumberOfDirectories];
			int i;
			for (i=0;i<iNumberOfDirectories && i<int(m_nMaxBrowse);i++)
				pBrowseNew[i].Swap(m_pBrowse[i]);
			
			if (iNumberOfDirectories<int(m_nMaxBrowse))
			{
				for (;i<int(m_nMaxBrowse);i++)
					m_pBrowse[i].Empty();

				// Removing items from combobox
				for (i=0;i<m_LookIn.GetCount();i++)
				{
					LPARAM lParam=m_LookIn.GetItemData(i);
					if (HIWORD(lParam)>=iNumberOfDirectories && LOWORD(lParam)==Custom)
					{
						m_LookIn.DeleteItem(i);
						i--;
					}
				}
			}
			delete[] m_pBrowse;
			m_pBrowse=pBrowseNew;
			m_nMaxBrowse=iNumberOfDirectories;
		}
		LeaveCriticalSection(&m_cBrowse);
	}
}

BOOL CLocateDlg::CNameDlg::InitDriveBox(BYTE nFirstTime)
{
	//DebugFormatMessage("CNameDlg::InitDriveBox BEGIN, items in list %d",m_LookIn.GetCount());

	// Handle to locate dialog
	CLocateDlg* pLocateDlg=GetTrayIconWnd()->GetLocateDlg();
	pLocateDlg->AddInstantSearchingFlags(isIgnoreChangeMessages);

	CRegKey RegKey;
	COMBOBOXEXITEMW ci;
	LPITEMIDLIST idl;
	SHFILEINFOW fi;
	DWORD dwGetIconFlags=SHGFI_SYSICONINDEX|SHGFI_SMALLICON;
	if (GetLocateApp()->GetProgramFlags()&CLocateApp::pfAvoidToAccessWhenReadingIcons)
		dwGetIconFlags|=SHGFI_USEFILEATTRIBUTES;
	
	// Buffers
	CStringW temp;
	WCHAR szBuf[]=L"X:\\";
	WCHAR Buffer[100];
	
	
	LPARAM lParam;
	int nSelection=-1;
	
	
	if (!nFirstTime)
	{
		ci.mask=CBEIF_LPARAM;
		ci.iItem=m_LookIn.GetCurSel();
		if (ci.iItem!=CB_ERR)
		{
			m_LookIn.GetItem(&ci);
			lParam=ci.lParam;
		}
		else
			lParam=LPARAM(-1);
		m_LookIn.ResetContent();
	}
	else
		lParam=MAKELPARAM(Everywhere,0);

	

	m_LookIn.SetImageList((HIMAGELIST)ShellFunctions::GetFileInfo(szwEmpty,0,&fi,SHGFI_SYSICONINDEX|SHGFI_SMALLICON));

	// Everywhere, icon from Network Neighborhood
	SHGetSpecialFolderLocation(*this,CSIDL_NETWORK,&idl);
	ShellFunctions::GetFileInfo(idl,0,&fi,SHGFI_SYSICONINDEX|SHGFI_SMALLICON);
	ci.iImage=fi.iIcon;
	ShellFunctions::GetFileInfo(idl,0,&fi,SHGFI_SYSICONINDEX|SHGFI_SMALLICON|SHGFI_OPENICON);
	ci.iSelectedImage=fi.iIcon;
	LoadString(IDS_EVERYWERE,Buffer,100);
	ci.pszText=Buffer;
	ci.iItem=0;
	ci.iIndent=0;
	ci.lParam=MAKELPARAM(Everywhere,Original);
	if (lParam==ci.lParam || nSelection==-1)
		nSelection=(int)ci.iItem;
	ci.mask=CBEIF_TEXT|CBEIF_IMAGE|CBEIF_INDENT|CBEIF_SELECTEDIMAGE|CBEIF_LPARAM;
	m_LookIn.InsertItem(&ci);
	CoTaskMemFree(idl);
	

	// Document folders
	SHGetSpecialFolderLocation(*this,CSIDL_PERSONAL,&idl);
	ShellFunctions::GetFileInfo(idl,0,&fi,SHGFI_SYSICONINDEX|SHGFI_SMALLICON);
	ci.iImage=fi.iIcon;
	ShellFunctions::GetFileInfo(idl,0,&fi,SHGFI_SYSICONINDEX|SHGFI_SMALLICON|SHGFI_OPENICON);
	ci.iSelectedImage=fi.iIcon;
	LoadString(IDS_DOCUMENTFOLDERS,Buffer,80);
	ci.pszText=Buffer;
	ci.iItem++;
	ci.iIndent=0;
	ci.lParam=MAKELPARAM(Special,Documents);
	if (lParam==ci.lParam)
		nSelection=(int)ci.iItem;
	ci.mask=CBEIF_TEXT|CBEIF_IMAGE|CBEIF_INDENT|CBEIF_SELECTEDIMAGE|CBEIF_LPARAM;
	m_LookIn.InsertItem(&ci);
	CoTaskMemFree(idl);
	
	// Desktop
	SHGetSpecialFolderLocation(*this,CSIDL_DESKTOP,&idl);
	ShellFunctions::GetFileInfo(idl,0,&fi,SHGFI_SYSICONINDEX|SHGFI_SMALLICON);
	ci.iImage=fi.iIcon;
	ShellFunctions::GetFileInfo(idl,0,&fi,SHGFI_SYSICONINDEX|SHGFI_SMALLICON|SHGFI_OPENICON);
	ci.iSelectedImage=fi.iIcon;
	ShellFunctions::GetFileInfo(idl,0,&fi,SHGFI_DISPLAYNAME);
	ci.pszText=fi.szDisplayName;
	ci.iItem++;
	ci.iIndent=1;
	ci.lParam=MAKELPARAM(Special,Desktop);
	if (lParam==ci.lParam)
		nSelection=(int)ci.iItem;
	ci.mask=CBEIF_TEXT|CBEIF_IMAGE|CBEIF_INDENT|CBEIF_SELECTEDIMAGE|CBEIF_LPARAM;
	m_LookIn.InsertItem(&ci);
	CoTaskMemFree(idl);
	
	// My Documents
	temp.Empty();
	if (RegKey.OpenKey(HKCU,"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
	{
		RegKey.QueryValue(L"Personal",temp);
		RegKey.CloseKey();
	}	
	if	(FileSystem::IsDirectory(temp))
	{
		ShellFunctions::GetFileInfo(temp,0,&fi,SHGFI_SYSICONINDEX|SHGFI_SMALLICON);
		ci.iImage=fi.iIcon;
		ShellFunctions::GetFileInfo(temp,0,&fi,SHGFI_SYSICONINDEX|SHGFI_SMALLICON|SHGFI_OPENICON);
		ci.iSelectedImage=fi.iIcon;
		ShellFunctions::GetFileInfo(temp,0,&fi,SHGFI_DISPLAYNAME);
		ci.pszText=fi.szDisplayName;
		ci.iItem++;
		ci.iIndent=1;
		ci.lParam=MAKELPARAM(Special,Personal);
		if (lParam==ci.lParam)
			nSelection=(int)ci.iItem;
		ci.mask=CBEIF_TEXT|CBEIF_IMAGE|CBEIF_INDENT|CBEIF_SELECTEDIMAGE|CBEIF_LPARAM;
		m_LookIn.InsertItem(&ci);
	}
	
	// My Computer
	SHGetSpecialFolderLocation(*this,CSIDL_DRIVES,&idl);
	ShellFunctions::GetFileInfo(idl,0,&fi,SHGFI_SYSICONINDEX|SHGFI_SMALLICON);
	ci.iImage=fi.iIcon;
	ShellFunctions::GetFileInfo(idl,0,&fi,SHGFI_SYSICONINDEX|SHGFI_SMALLICON|SHGFI_OPENICON);
	ci.iSelectedImage=fi.iIcon;
	ShellFunctions::GetFileInfo(idl,0,&fi,SHGFI_DISPLAYNAME);
	ci.pszText=fi.szDisplayName;
	ci.iItem++;
	ci.iIndent=0;
	ci.lParam=MAKELPARAM(Special,MyComputer);
	if (lParam==ci.lParam)
		nSelection=(int)ci.iItem;
	ci.mask=CBEIF_TEXT|CBEIF_IMAGE|CBEIF_INDENT|CBEIF_SELECTEDIMAGE|CBEIF_LPARAM;
	m_LookIn.InsertItem(&ci);
	CoTaskMemFree(idl);
	
	// Drives
	CArrayFAP<LPWSTR> aRoots;
	CDatabaseInfo::GetRootsFromDatabases(aRoots,GetLocateApp()->GetDatabases(),TRUE);
	
	for (int j=0;j<aRoots.GetSize();j++)
	{
		BOOL bAdd=TRUE;
		if (aRoots.GetAt(j)[0]=='\\' && aRoots.GetAt(j)[1]=='\\')
			continue;

		szBuf[0]=aRoots.GetAt(j)[0];
		MakeUpper(szBuf);
		
		if (FileSystem::GetDriveType(szBuf)<2)
			continue;
		
		// Checking whether drive exists
		for (int k=0;k<j;k++)
		{
			WCHAR cDrive=aRoots.GetAt(k)[0];
			MakeUpper(&cDrive,1);

			if (cDrive==szBuf[0])
			{
				bAdd=FALSE;
				break;
			}
		}
		if (!bAdd)
			continue;

		if (GetLocateDlg()->GetExtraFlags()&efNameDontResolveIconAndDisplayNameForDrives)
		{
			// Icon from system drive
			WCHAR szWindowsDir[MAX_PATH]=L"C:\\";
			FileSystem::GetWindowsDirectory(szWindowsDir,MAX_PATH);
			if (szWindowsDir[1]==L':' && szWindowsDir[2]==L'\\')
				szWindowsDir[3]='\0';
			ShellFunctions::GetFileInfo(szWindowsDir,0,&fi,dwGetIconFlags|SHGFI_OPENICON);
			ci.iSelectedImage=fi.iIcon;
			ShellFunctions::GetFileInfo(szWindowsDir,0,&fi,dwGetIconFlags);
			ci.iImage=fi.iIcon;
			
			fi.szDisplayName[0]='\0';
			FileSystem::GetVolumeInformation(szBuf,fi.szDisplayName,MAX_PATH-7,NULL,
				NULL,NULL,NULL,0);
			int nLen=istrlen(fi.szDisplayName);
			fi.szDisplayName[nLen++]=L' ';
			fi.szDisplayName[nLen++]=L'(';
			fi.szDisplayName[nLen++]=szBuf[0];
			fi.szDisplayName[nLen++]=L':';
			fi.szDisplayName[nLen++]=L')';
			fi.szDisplayName[nLen++]=L'\0';

		}
		else
		{
			ShellFunctions::GetFileInfo(szBuf,FILE_ATTRIBUTE_DIRECTORY,&fi,dwGetIconFlags|SHGFI_OPENICON);
			ci.iSelectedImage=fi.iIcon;
			ShellFunctions::GetFileInfo(szBuf,FILE_ATTRIBUTE_DIRECTORY,&fi,dwGetIconFlags|SHGFI_DISPLAYNAME);
			ci.iImage=fi.iIcon;
		}

		ci.pszText=fi.szDisplayName;
		ci.iItem++;
		ci.iIndent=1;
		ci.lParam=MAKELPARAM(Drive,szBuf[0]);
		if (lParam==ci.lParam)
			nSelection=(int)ci.iItem;
		ci.mask=CBEIF_TEXT|CBEIF_IMAGE|CBEIF_INDENT|CBEIF_SELECTEDIMAGE|CBEIF_LPARAM;
		m_LookIn.InsertItem(&ci);

	}

	
	if ((pLocateDlg->GetFlags()&CLocateDlg::fgNameRootFlag)!=CLocateDlg::fgNameDontAddRoots)
	{
		if ((pLocateDlg->GetFlags()&CLocateDlg::fgNameRootFlag)==CLocateDlg::fgNameAddEnabledRoots)
		{
			// Currently aRoots contains all roots
			aRoots.RemoveAll();
			CDatabaseInfo::GetRootsFromDatabases(aRoots,GetLocateApp()->GetDatabases(),TRUE);	
		}

		// Get drives which are already added
		CArray<char> aCurrentlyAddedDrives;
		{
			COMBOBOXEXITEM ci;
			ci.mask=CBEIF_LPARAM;
			for (ci.iItem=m_LookIn.GetCount()-1;ci.iItem>=0;ci.iItem--)
			{
				m_LookIn.GetItem(&ci);
				if (static_cast<TypeOfItem>(LOWORD(ci.lParam))==Drive)
					aCurrentlyAddedDrives.Add((char)HIWORD(ci.lParam));
			}
			// Make drives upper
			MakeUpper(aCurrentlyAddedDrives.GetData(),aCurrentlyAddedDrives.GetSize());
		}
		
		for (int i=0;i<aRoots.GetSize();)
		{
			LPCWSTR pRoot=aRoots[i];
			if (pRoot[1]==L':' && pRoot[2]==L'\0')
			{
				WCHAR cDrive=pRoot[0];
				MakeUpper(&cDrive,1);
				
				// Checking whether drive is already added
				int j;
				for (j=0;j<aCurrentlyAddedDrives.GetSize() && cDrive!=aCurrentlyAddedDrives[j];j++);
				
				if (j<aCurrentlyAddedDrives.GetSize())
				{
					aRoots.RemoveAt(i);
                    continue;
				}
			}

			i++;
		}


		// Checking which one are non-local drives
		if (aRoots.GetSize()>0)
		{
			EnterCriticalSection(&m_cBrowse);
			
			SHGetSpecialFolderLocation(*this,CSIDL_NETWORK,&idl);
			ShellFunctions::GetFileInfo(idl,0,&fi,SHGFI_SYSICONINDEX|SHGFI_SMALLICON|SHGFI_OPENICON);
			ci.iImage=fi.iIcon;
			ci.iSelectedImage=fi.iIcon;
			LoadString(IDS_ROOTS,Buffer,100);
			ci.pszText=Buffer;
			ci.iItem++;
			ci.iIndent=0;
			ci.lParam=MAKELPARAM(Everywhere,RootTitle);
			if (lParam==ci.lParam)
				nSelection=(int)ci.iItem;
			ci.mask=CBEIF_TEXT|CBEIF_IMAGE|CBEIF_INDENT|CBEIF_SELECTEDIMAGE|CBEIF_LPARAM;
			m_LookIn.InsertItem(&ci);
			CoTaskMemFree(idl);

			LeaveCriticalSection(&m_cBrowse);

			for (int i=0;i<aRoots.GetSize();i++)
			{
				if (GetLocateApp()->GetProgramFlags()&CLocateApp::pfUseDefaultIconForDirectories)
				{
					ci.iImage=DIR_IMAGE;
					ci.iSelectedImage=DIR_IMAGE;
				}
				else
				{
					ShellFunctions::GetFileInfo(aRoots[i],FILE_ATTRIBUTE_DIRECTORY,&fi,dwGetIconFlags);
					ci.iImage=fi.iIcon;
					ShellFunctions::GetFileInfo(aRoots[i],FILE_ATTRIBUTE_DIRECTORY,&fi,dwGetIconFlags|SHGFI_OPENICON);
					ci.iSelectedImage=fi.iIcon;
				}
				ci.pszText=aRoots[i];
				ci.iItem++;
				ci.iIndent=1;
				
				// Computing cheksum from directory
				// which is used to save control state
				ci.lParam=MAKELPARAM(Root,ComputeChecksumFromDir(aRoots[i]));
					
				if (lParam==ci.lParam)
					nSelection=(int)ci.iItem;
				ci.mask=CBEIF_TEXT|CBEIF_IMAGE|CBEIF_INDENT|CBEIF_SELECTEDIMAGE|CBEIF_LPARAM;
				m_LookIn.InsertItem(&ci);

				//DebugFormatMessage("DriveList: root %S added, i=%d",aRoots.GetAt(i),i);

			}

			
		}
	}

	EnterCriticalSection(&m_cBrowse);
	if (m_pBrowse!=NULL) // NULL is possible is locate is just closing
	{
		// Remembered directories
		for (int j=0;j<int(m_nMaxBrowse);j++)
		{
			if (m_pBrowse[j].IsEmpty())
				break;

			if (GetLocateApp()->GetProgramFlags()&CLocateApp::pfUseDefaultIconForDirectories)
			{
				ci.iImage=DIR_IMAGE;
				ci.iSelectedImage=DIR_IMAGE;
			}
			else
			{
				ShellFunctions::GetFileInfo(m_pBrowse[j],FILE_ATTRIBUTE_DIRECTORY,&fi,dwGetIconFlags);
				ci.iImage=fi.iIcon;
				ShellFunctions::GetFileInfo(m_pBrowse[j],FILE_ATTRIBUTE_DIRECTORY,&fi,dwGetIconFlags|SHGFI_OPENICON);
				ci.iSelectedImage=fi.iIcon;
			}
			ci.pszText=m_pBrowse[j].GetBuffer();
			ci.iItem++;
			ci.iIndent=0;
			ci.lParam=MAKELPARAM(Custom,j);
			if (lParam==ci.lParam)
				nSelection=(int)ci.iItem;
			ci.mask=CBEIF_TEXT|CBEIF_IMAGE|CBEIF_INDENT|CBEIF_SELECTEDIMAGE|CBEIF_LPARAM;
			m_LookIn.InsertItem(&ci);

			//DebugFormatMessage("DriveList: directory %s added",m_pBrowse[j].GetBuffer());
		}
	}
	LeaveCriticalSection(&m_cBrowse);

	if (lParam!=LPARAM(-1))
	{
		m_LookIn.SetCurSel(nSelection);
		m_LookIn.SetItemText(-1,m_LookIn.GetItemTextW(nSelection));
	}

	pLocateDlg->RemoveInstantSearchingFlags(isIgnoreChangeMessages);
	return TRUE;
}
	
BOOL CLocateDlg::CNameDlg::SelectByLParam(LPARAM lParam)
{
	COMBOBOXEXITEM ci;
	ci.mask=CBEIF_LPARAM;
	
	for (ci.iItem=m_LookIn.GetCount()-1;ci.iItem>=0;ci.iItem--)
	{
		m_LookIn.GetItem(&ci);
		if (ci.lParam==lParam)
		{
			m_LookIn.SetCurSel((int)ci.iItem);
			m_LookIn.SetItemText(-1,m_LookIn.GetItemTextW((int)ci.iItem));
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CLocateDlg::CNameDlg::SelectByRootName(LPCWSTR szDirectory)
{
	COMBOBOXEXITEMW ci;
	ci.mask=CBEIF_LPARAM;
	
	for (ci.iItem=m_LookIn.GetCount()-1;ci.iItem>=0;ci.iItem--)
	{
		m_LookIn.GetItem(&ci);
		if (static_cast<TypeOfItem>(LOWORD(ci.lParam))==Root)
		{
			WCHAR szPath[MAX_PATH+10];
			ci.pszText=szPath;
			ci.cchTextMax=MAX_PATH+10;
			ci.mask=CBEIF_TEXT;
			m_LookIn.GetItem(&ci);

			if (strcasecmp(ci.pszText,szDirectory)==0)
			{
				m_LookIn.SetCurSel((int)ci.iItem);
				m_LookIn.SetItemText(-1,szPath);
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL CLocateDlg::CNameDlg::SelectByCustomName(LPCWSTR szDirectory)
{
	COMBOBOXEXITEM ci;
	ci.mask=CBEIF_LPARAM;
	
	for (ci.iItem=m_LookIn.GetCount()-1;ci.iItem>=0;ci.iItem--)
	{
		m_LookIn.GetItem(&ci);
		if (static_cast<TypeOfItem>(LOWORD(ci.lParam))==Custom)
		{
			EnterCriticalSection(&m_cBrowse);
			if (m_pBrowse!=NULL && HIWORD(ci.lParam)<m_nMaxBrowse)
			{
				if (m_pBrowse[HIWORD(ci.lParam)].CompareNoCase(szDirectory)==0)
				{
					m_LookIn.SetCurSel((int)ci.iItem);
					m_LookIn.SetItemText(-1,m_pBrowse[HIWORD(ci.lParam)]);
					LeaveCriticalSection(&m_cBrowse);
					return TRUE;
				}
			}
			LeaveCriticalSection(&m_cBrowse);
		}
	}

	m_LookIn.SetCurSel(-1);
	m_LookIn.SetItemText(-1,szDirectory);
	return TRUE;
}

BOOL CLocateDlg::CNameDlg::OnCommand(WORD wID,WORD wNotifyCode,HWND hControl)
{
	CDialog::OnCommand(wID,wNotifyCode,hControl);

	// I don't understand how GetLocateDlg() could return NULL, but crashes occrus anyway if not checked
	CLocateDlg* pLocateDlg=GetLocateDlg();
	if (pLocateDlg==NULL)
		return FALSE;
				
	switch (wID)
	{
	case IDC_BROWSE:
		if (!::IsWindowEnabled(GetDlgItem(IDC_BROWSE)))
			break;
		switch (wNotifyCode)
		{
		case 1:
		case BN_CLICKED:
			OnBrowse();
			break;
		}
		break;
	case IDC_NAME:
		switch (wNotifyCode)
		{
		case 1: /* ACCEL or CBN_SELCHANGE */
			if (hControl==NULL)
				m_Name.SetFocus();
			else
			{
				HilightTab(IsChanged());
				pLocateDlg->OnFieldChange(isNameChanged);
			}

			break;
		case CBN_SETFOCUS:
			m_Name.SetEditSel(0,-1);
			break;
		case CBN_EDITCHANGE:
			if (pLocateDlg->IsExtraFlagSet(efMatchWhileNameIfAsterisks))
			{
				CStringW Name;
				m_Name.GetText(Name);
				pLocateDlg->m_AdvancedDlg.EnableDlgItem(IDC_MATCHWHOLENAME,Name.Find(L'*')==-1);
			}
			HilightTab(m_Name.GetTextLength()>0);
			pLocateDlg->OnFieldChange(isNameChanged);
			break;
		case CBN_KILLFOCUS:
			if (pLocateDlg->IsInstantSearchingFlagSet(isRunning))
				AddCurrentNameToList();
			break;
		}
		break;
	case IDC_TYPE:
		switch (wNotifyCode)
		{
		case 1: /* ACCEL or CBN_SELCHANGE */
			if (hControl==NULL)
				m_Type.SetFocus();
			else
			{
				pLocateDlg->m_AdvancedDlg.ChangeEnableStateForCheck();
				HilightTab(IsChanged());
				pLocateDlg->OnFieldChange(isTypeChanged);
			}
			break;
		case CBN_SETFOCUS:
			m_Type.SetEditSel(0,-1);
			break;
		case CBN_EDITCHANGE:
			pLocateDlg->m_AdvancedDlg.ChangeEnableStateForCheck();
			HilightTab(IsChanged());
			pLocateDlg->OnFieldChange(isTypeChanged);
			break;
		case CBN_KILLFOCUS:
			if (pLocateDlg->IsInstantSearchingFlagSet(isRunning))
				AddCurrentTypeToList();
			break;
		}
		break;
	case IDC_LOOKIN:
		switch (wNotifyCode)
		{
		case 1: /* ACCEL or CBN_SELCHANGE */
			if (hControl==NULL)
				m_LookIn.SetFocus();
			else
			{
				HilightTab(IsChanged());
				pLocateDlg->OnFieldChange(isLookInChanged);
			}
			break;
		case CBN_EDITCHANGE:
			//DebugMessage("LOOKIN EDITCHANGE");
			HilightTab(IsChanged());
			pLocateDlg->OnFieldChange(isLookInChanged);
			break;
		}
		break;	
	case IDC_MOREDIRECTORIES:
		OnMoreDirectories();
		break;
	case IDM_LOOKINNEWSELECTION:
		OnLookInNewSelection();
		break;
	case IDM_LOOKINREMOVESELECTION:
		OnLookInRemoveSelection();
		break;
	case IDM_LOOKINNEXTSELECTION:
	case IDM_LOOKINPREVSELECTION:
		OnLookInNextSelection(wID==IDM_LOOKINNEXTSELECTION);
		break;
	case IDC_NOSUBDIRECTORIES:
		if (hControl==NULL && wNotifyCode==1) // Accelerator
		{
			CheckDlgButton(wID,!IsDlgButtonChecked(wID));
			SetFocus(wID);
		}
		HilightTab(IsChanged());
		pLocateDlg->OnFieldChange(isOtherChanged);
		break;
	}
	return FALSE;
}

BOOL CLocateDlg::CNameDlg::OnNotify(int idCtrl,LPNMHDR pnmh)
{
	if (idCtrl==IDC_LOOKIN)
	{
		// I don't understand how GetLocateDlg() could return NULL, but crashes occrus anyway if not checked
		CLocateDlg* pLocateDlg=GetLocateDlg();
		if (pLocateDlg==NULL)
			return FALSE;
	
		if (((pnmh->code==CBEN_ENDEDITA && ((PNMCBEENDEDITA)pnmh)->iWhy==CBENF_KILLFOCUS) ||
			(pnmh->code==CBEN_ENDEDITW && ((PNMCBEENDEDITW)pnmh)->iWhy==CBENF_KILLFOCUS)) &&
			pLocateDlg->IsInstantSearchingFlagSet(isRunning))
		{
			if (GetCurrentlySelectedComboItem()==DWORD(CB_ERR))
			{
				// Getting directory from combo
				DWORD dwBufferSize=MAX_PATH;
				WCHAR* pFolder;
				
				DWORD dwLength;
				for(;;)
				{
					pFolder=new WCHAR[dwBufferSize];
					
					if (!m_LookIn.GetItemText(-1,pFolder,dwBufferSize))
					{
						delete[] pFolder;
						return FALSE;
					}

					dwLength=istrlenw(pFolder);
					
					if (dwLength==dwBufferSize-1)
					{
						delete[] pFolder;
						dwBufferSize*=2;
					}
					else
						break;
				}

				CArrayFAP<LPWSTR> aDirectories;
				pLocateDlg->AddInstantSearchingFlags(isIgnoreChangeMessages);
				BOOL bRet=GetDirectoriesFromCustomText(aDirectories,pFolder,dwLength,FALSE,TRUE);
				pLocateDlg->RemoveInstantSearchingFlags(isIgnoreChangeMessages);
			
				delete[] pFolder;
				
				return bRet;
			}
			//return TRUE;
		}
	}

	return CDialog::OnNotify(idCtrl,pnmh);
}
	
LRESULT CALLBACK CLocateDlg::CNameDlg::NameWindowProc(HWND hWnd,UINT uMsg,
													  WPARAM wParam,LPARAM lParam)
{
	NameControlData* pNameData=(NameControlData*)::GetWindowLongPtr(hWnd,GWLP_USERDATA);
	LRESULT lRet=FALSE;

	if (pNameData==NULL)
		return FALSE;

	ASSERT_VALID(pNameData);
	ASSERT_VALID(pNameData->pOldWndProc);

	
	switch (uMsg)
	{
	case WM_DESTROY:
		if (IsUnicodeSystem())
		{
			// Calling original window procedure
			lRet=CallWindowProcW(pNameData->pOldWndProc,hWnd,uMsg,wParam,lParam);

			// Desubclassing
			::SetWindowLongPtrW(hWnd,GWLP_WNDPROC,(LONG_PTR)pNameData->pOldWndProc);
		}
		else
		{
			// Calling original window procedure
			lRet=CallWindowProcA(pNameData->pOldWndProc,hWnd,uMsg,wParam,lParam);

			// Desubclassing
			::SetWindowLongPtrA(hWnd,GWLP_WNDPROC,(LONG_PTR)pNameData->pOldWndProc);
		}
		
		
		// Free memory
		delete pNameData;
		return lRet;
	case WM_KEYDOWN:
		if (wParam==VK_DOWN || wParam==VK_UP)
		{
			if (pNameData->pLocateDlg->IsInstantSearchingFlagSet(CLocateDlg::isUpDownGoesToResults) && 
				pNameData->pLocateDlg->IsInstantSearchingFlagSet(CLocateDlg::isRunning) &&
				!((GetKeyState(VK_CONTROL)|GetKeyState(VK_SHIFT)|GetKeyState(VK_MENU)) & 0x8000))
			{
				if (!pNameData->pNameCombo->GetDroppedState() && 
					pNameData->pLocateDlg->m_pListCtrl->GetItemCount()>0)
				{
					pNameData->pLocateDlg->PostMessage(WM_SETITEMFOCUS,
						(WPARAM)(HWND)*pNameData->pLocateDlg->m_pListCtrl);
					return lRet;
				}
			}
		}
		// Continuing to default section
	default:
		if (IsUnicodeSystem())
			lRet=CallWindowProcW(pNameData->pOldWndProc,hWnd,uMsg,wParam,lParam);
		else
			lRet=CallWindowProcA(pNameData->pOldWndProc,hWnd,uMsg,wParam,lParam);
		break;
	}

	return lRet;
}



BOOL CLocateDlg::CNameDlg::IsChanged()
{
	int nSel=m_Name.GetCurSel();
	if (nSel==CB_ERR)
	{
		if (m_Name.GetTextLength()>0)
			return TRUE;
	}
	else
	{
		if (m_Name.GetLBTextLen(nSel)>0)
			return TRUE;
	}

	if (m_Type.IsWindowEnabled())
	{
		nSel=m_Type.GetCurSel();
		if (nSel==CB_ERR)
		{
			if (m_Type.GetTextLength()>0)
				return TRUE;
		}
		else
		{
			if (m_Type.GetLBTextLen(nSel)>0)
				return TRUE;
		}
	}

	if (IsDlgButtonChecked(IDC_NOSUBDIRECTORIES))
		return TRUE;

	if (m_pMultiDirs!=NULL)
	{
		if (m_pMultiDirs[1]!=NULL)
			return TRUE;
	}
	
	int nCurSel=m_LookIn.GetCurSel();
	if (nCurSel==CB_ERR)
		return TRUE; // Type directory

	return m_LookIn.GetItemData(nCurSel)!=MAKELPARAM(Everywhere,Original);
}

void CLocateDlg::CNameDlg::OnDestroy()
{
	SaveRegistry();	
	CDialog::OnDestroy();

	EnterCriticalSection(&m_cBrowse);
	if (m_pBrowse!=NULL)
	{
		for (DWORD i=0;i<m_nMaxBrowse;i++)
			m_pBrowse[i].Empty();
		delete[] m_pBrowse;
		m_pBrowse=NULL;
	}
	LeaveCriticalSection(&m_cBrowse);
	
	if (m_pMultiDirs!=NULL)
	{
		for (DWORD i=0;m_pMultiDirs[i]!=NULL;i++)
			delete m_pMultiDirs[i];
		delete[] m_pMultiDirs;
		m_pMultiDirs=NULL;
	}

}

void CLocateDlg::CNameDlg::AddCurrentNameToList()
{
	// Save name to the list
	CStringW sName,Buffer;
	m_Name.GetText(sName);

	for (int i=m_Name.GetCount()-1;i>=0;i--)
	{
		m_Name.GetLBText(i,Buffer);
		if (sName.CompareNoCase(Buffer)==0)
			m_Name.DeleteString(i);
	}	
	m_Name.InsertString(0,sName);
	m_Name.SetText(sName);

	if (m_Name.GetCount()>int(m_nMaxNamesInList))
		m_Name.DeleteString(m_nMaxNamesInList);
}

void CLocateDlg::CNameDlg::AddCurrentTypeToList()
{
	if (m_Type.GetCurSel()==0) // Empty extension
		return;
	
	// Save extension to the list
	CStringW sType,Buffer;
	m_Type.GetText(sType);
		
	for (int i=m_Type.GetCount()-1;i>=1;i--)
	{
		m_Type.GetLBText(i,Buffer);
		if (sType.CompareNoCase(Buffer)==0)
			m_Type.DeleteString(i);
	}	

	m_Type.InsertString(1,sType); // 0 == (none)
	m_Type.SetText(sType);
	if (m_Type.GetCount()>int(m_nMaxTypesInList)+1)
		m_Type.DeleteString(m_nMaxTypesInList+1);
}


BOOL CLocateDlg::CNameDlg::GetNameExtensionsAndDirectories(CStringW& sName,CArray<LPWSTR>& aExtensions,CArrayFAP<LPWSTR>& aDirectories,BOOL bForInstantSearch)
{
	// Setting recent combobox for name
	m_Name.GetText(sName);

	if (!bForInstantSearch)
		AddCurrentNameToList();


	// Setting recent combobox for type
	if (m_Type.GetCurSel()==0) // Empty extension
		aExtensions.Add(allocemptyW());
	else
	{
		CStringW sType;
		m_Type.GetText(sType);
	
		if (!bForInstantSearch)
			AddCurrentTypeToList();
		
		// Parsing extensions
		LPCWSTR pType=sType;
		for (;pType[0]==L' ';pType++);
		while (*pType!=L'\0')
		{
			// Calculate length, separators are ' ', ',' and ';'
			DWORD nLength;
			for (nLength=0;pType[nLength]!=L'\0' && pType[nLength]!=L' '&& pType[nLength]!=L','&& pType[nLength]!=L';';nLength++);		

			// If dot '.' found, use only the part after the dot
			for (int i=nLength-1;i>=0;i--)
			{
				if (pType[i]=='.')
				{
					pType+=i+1;
					nLength-=i+1;
					break;
				}
			}


			// Add to extensions list
			aExtensions.Add(alloccopy(pType,nLength));

			pType+=nLength;
			for (;pType[0]==L' ' || pType[0]==L',' || pType[0]==L';';pType++);
		}
	}

	// Resolving directories
	if (m_pMultiDirs!=NULL)
	{
		// Check contradictions
		BOOL bEverywhereSelected=FALSE;
		BOOL bOtherSelected=FALSE;

		for (int i=0;m_pMultiDirs[i]!=NULL;i++)
		{
			if (m_pMultiDirs[i]->bSelected)
			{
				TypeOfItem nType;
				if (!GetDirectoriesForActiveSelection(aDirectories,bForInstantSearch,&nType,FALSE))
					return FALSE;

				if (nType==Everywhere)
					bEverywhereSelected=TRUE;
				else
					bOtherSelected=TRUE;

			}
			else
			{
				if (!GetDirectoriesForNonActiveSelection(aDirectories,m_pMultiDirs[i],FALSE))
					return FALSE;

				if (m_pMultiDirs[i]->nType==Everywhere)
					bEverywhereSelected=TRUE;
				else
					bOtherSelected=TRUE;
			}
		}

		if (bEverywhereSelected && bOtherSelected && !bForInstantSearch)
		{
			switch(ShowErrorMessage(IDS_LOOKINCONTRADICTION,IDS_LOOKIN,MB_ICONQUESTION|MB_YESNOCANCEL))
			{
			case IDYES:
				break;
			case IDNO: // Everywhere
				aDirectories.RemoveAll();
				break;
			case IDCANCEL:
				return FALSE;
			}
		}
	}
	else if (!GetDirectoriesForActiveSelection(aDirectories,bForInstantSearch,NULL,FALSE))
		return FALSE;


	if (!bForInstantSearch)
	{
		// Saves searches
		SaveRegistry();
	}
	
	return TRUE;
}

DWORD CLocateDlg::CNameDlg::GetCurrentlySelectedComboItem() const
{
	DWORD nCurSel=m_LookIn.GetCurSel();
	
	if (nCurSel!=DWORD(CB_ERR))
	{
		WCHAR szTmp1[MAX_PATH],szTmp2[MAX_PATH];
		m_LookIn.GetItemText(nCurSel,szTmp1,MAX_PATH);
		m_LookIn.GetItemText(-1,szTmp2,MAX_PATH);
		
		if (wcscmp(szTmp1,szTmp2)!=0)
			nCurSel=DWORD(CB_ERR);
	}	

	return nCurSel;
}

BOOL CLocateDlg::CNameDlg::GetDirectoriesForActiveSelection(CArray<LPWSTR>& aDirectories,BOOL bInstantSearch,TypeOfItem* pType,BOOL bNoWarningIfNotExists)
{
	DWORD nCurSel=GetCurrentlySelectedComboItem();
	
	if (nCurSel==DWORD(CB_ERR))
	{
		// Getting directory from combo
		DWORD dwBufferSize=MAX_PATH;
		WCHAR* pFolder;
		
		DWORD dwLength;
		for(;;)
		{
			pFolder=new WCHAR[dwBufferSize];
			
			if (!m_LookIn.GetItemText(-1,pFolder,dwBufferSize))
			{
				delete[] pFolder;
				return FALSE;
			}
			
			dwLength=istrlenw(pFolder);
			
			if (dwLength==dwBufferSize-1)
			{
				delete[] pFolder;
				dwBufferSize*=2;
			}
			else
                break;
		}

		// Replace '/' with '\\'
		for (DWORD i=0;i<dwLength;i++)
		{
			if (pFolder[i]=='/')
				pFolder[i]='\\';
		}

		BOOL bRet=GetDirectoriesFromCustomText(aDirectories,pFolder,dwLength,!bInstantSearch,bNoWarningIfNotExists);
        delete[] pFolder;
		
		if (pType!=NULL)
			*pType=Custom;

		return bRet;
	}
	else
	{
		COMBOBOXEXITEMW ci;
		ci.mask=CBEIF_LPARAM;
		ci.iItem=nCurSel;
		m_LookIn.GetItem(&ci);

		if (pType!=NULL)
			*pType=static_cast<TypeOfItem>(LOWORD(ci.lParam));

		if (static_cast<TypeOfItem>(LOWORD(ci.lParam))==Root)
		{
			WCHAR szPath[MAX_PATH+10];
			ci.pszText=szPath;
			ci.cchTextMax=MAX_PATH+10;
			ci.mask=CBEIF_TEXT;
			m_LookIn.GetItem(&ci);
			ParseGivenDirectoryForMultipleDirectories(aDirectories,szPath);
			return TRUE;
		}
		else
			return GetDirectoriesFromLParam(aDirectories,ci.lParam);
		
	}

}

BOOL CLocateDlg::CNameDlg::GetDirectoriesFromCustomText(CArray<LPWSTR>& aDirectories,LPCWSTR szCustomText,DWORD dwLength,BOOL bSaveAndSet,BOOL bNoWarning)
{
	// Removing spaces and \\ from beginning and end
	LPCWSTR pPtr=szCustomText;
	while (*pPtr==' ' && dwLength>0)
	{
		pPtr++;
		dwLength--;
	}
	while (dwLength>0 && pPtr[dwLength-1]==' ')
		dwLength--;
	//if (dwLength>0 && pPtr[dwLength-1]=='\\')
	//	dwLength--;

	if (!CheckAndAddDirectory(pPtr,dwLength,bSaveAndSet,bNoWarning))
		return FALSE;
	ParseGivenDirectoryForMultipleDirectories(aDirectories,pPtr,dwLength);
	return TRUE;
}



BOOL CLocateDlg::CNameDlg::GetDirectoriesFromLParam(CArray<LPWSTR>& aDirectories,LPARAM lParam)
{
	switch (static_cast<TypeOfItem>(LOWORD(lParam)))
	{
	case Special:
		{
			switch (static_cast<SpecialType>(HIWORD(lParam)))
			{
			case Documents:
				{
					PFNSHGETFOLDERPATH pGetFolderPath=(PFNSHGETFOLDERPATH)GetProcAddress(GetModuleHandle("shell32.dll"),"SHGetFolderPathW");
					if (pGetFolderPath!=NULL)
					{
						WCHAR szDir[MAX_PATH];
						if (SUCCEEDED(pGetFolderPath(NULL,CSIDL_DESKTOP,NULL,
							SHGFP_TYPE_CURRENT,szDir)))
						{
							int nLength=istrlen(szDir);
							while (szDir[nLength-2]==L'\\')
							{
								szDir[nLength-2]='\0';
								nLength--;
							}
							ParseGivenDirectoryForMultipleDirectories(aDirectories,szDir);
						}
		
						if (SUCCEEDED(pGetFolderPath(NULL,CSIDL_PERSONAL,NULL,
							SHGFP_TYPE_CURRENT,szDir)))
						{
							int nLength=istrlen(szDir);
							while (szDir[nLength-2]==L'\\')
							{
								szDir[nLength-2]='\0';
								nLength--;
							}
							ParseGivenDirectoryForMultipleDirectories(aDirectories,szDir);
						}

						break;
		
					}


					CRegKey RegKey;
					if (RegKey.OpenKey(HKCU,"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
					{
						DWORD nLength=RegKey.QueryValueLength("Desktop");
						WCHAR* szDir=new WCHAR[nLength];
						RegKey.QueryValue(L"Desktop",szDir,nLength);
						while (szDir[nLength-2]==L'\\')
						{
							szDir[nLength-2]='\0';
							nLength--;
						}
						ParseGivenDirectoryForMultipleDirectories(aDirectories,szDir);
						
						nLength=RegKey.QueryValueLength(L"Personal");
						szDir=new WCHAR[nLength];
						RegKey.QueryValue(L"Personal",szDir,nLength);
						while (szDir[nLength-2]==L'\\')
						{
							szDir[nLength-2]='\0';
							nLength--;
						}
						ParseGivenDirectoryForMultipleDirectories(aDirectories,szDir);
						
					}
					break;
				}
			case Desktop:
				{
					PFNSHGETFOLDERPATH pGetFolderPath=(PFNSHGETFOLDERPATH)GetProcAddress(GetModuleHandle("shell32.dll"),"SHGetFolderPathW");
					if (pGetFolderPath!=NULL)
					{
						WCHAR szDir[MAX_PATH];
						if (SUCCEEDED(pGetFolderPath(NULL,CSIDL_DESKTOP,NULL,
							SHGFP_TYPE_CURRENT,szDir)))
						{
							int nLength=istrlen(szDir);
							while (szDir[nLength-2]==L'\\')
							{
								szDir[nLength-2]='\0';
								nLength--;
							}
							ParseGivenDirectoryForMultipleDirectories(aDirectories,szDir);
							break;
						}
		
					}

					
					CRegKey RegKey;
					if (RegKey.OpenKey(HKCU,"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
					{
						DWORD nLength=RegKey.QueryValueLength("Desktop");
						WCHAR* szDir=new WCHAR[nLength+1];
						RegKey.QueryValue(L"Desktop",szDir,nLength);
						while (szDir[nLength-2]==L'\\')
						{
							szDir[nLength-2]=L'\0';
							nLength--;
						}
						ParseGivenDirectoryForMultipleDirectories(aDirectories,szDir);
					}
					break;
				}
			case Personal:
				{
					PFNSHGETFOLDERPATH pGetFolderPath=(PFNSHGETFOLDERPATH)GetProcAddress(GetModuleHandle("shell32.dll"),"SHGetFolderPathW");
					if (pGetFolderPath!=NULL)
					{
						WCHAR szDir[MAX_PATH];
						if (SUCCEEDED(pGetFolderPath(NULL,CSIDL_PERSONAL,NULL,
							SHGFP_TYPE_CURRENT,szDir)))
						{
							int nLength=istrlen(szDir);
							while (szDir[nLength-2]==L'\\')
							{
								szDir[nLength-2]='\0';
								nLength--;
							}
							ParseGivenDirectoryForMultipleDirectories(aDirectories,szDir);
							break;
						}

		
					}

					CRegKey RegKey;
					if (RegKey.OpenKey(HKCU,"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
					{
						DWORD nLength=RegKey.QueryValueLength("Personal");
						WCHAR* szDir=new WCHAR[nLength+1];
						RegKey.QueryValue(L"Personal",szDir,nLength);
						while (szDir[nLength-2]==L'\\')
						{
							szDir[nLength-2]=L'\0';
							nLength--;
						}
						ParseGivenDirectoryForMultipleDirectories(aDirectories,szDir,nLength);
					}
					break;
				}
			case MyComputer:
				{
					DWORD nLength=GetLogicalDriveStrings(0,NULL)+1;
					if (nLength>=2)
					{
						WCHAR* szDrives=new WCHAR[nLength+1];
						FileSystem::GetLogicalDriveStrings(nLength,szDrives);
						for (LPWSTR szDrive=szDrives;szDrive[0]!=L'\0';szDrive+=4)
						{
							if (FileSystem::GetDriveType(szDrive)!=DRIVE_REMOTE)
								ParseGivenDirectoryForMultipleDirectories(aDirectories,szDrive,2);
						}
						delete[] szDrives;
					}
					break;
				}
			}
			break;
		}
	case Drive:
		{
			WCHAR* szDir=new WCHAR[3];
			szDir[0]=static_cast<WCHAR>(HIWORD(lParam));
			szDir[1]=':';
			szDir[2]=L'\0';
			ParseGivenDirectoryForMultipleDirectories(aDirectories,szDir);
			break;
		}
	case Custom:
		EnterCriticalSection(&m_cBrowse);	
		if (m_pBrowse!=NULL && HIWORD(lParam)<m_nMaxBrowse)
			ParseGivenDirectoryForMultipleDirectories(aDirectories,(LPCWSTR)m_pBrowse[HIWORD(lParam)],m_pBrowse[HIWORD(lParam)].GetLength());
		LeaveCriticalSection(&m_cBrowse);
		break;
	case Root:
		break;
	case Everywhere:
		// None
		break;
	}
	return TRUE;
}



void CLocateDlg::CNameDlg::ParseGivenDirectoryForMultipleDirectories(CArray<LPWSTR>& aDirectories,LPCWSTR szDirectory)
{
	CStringW sDirectory;
	int nLength;


	// Multiple directories
	LPCWSTR pPtr=szDirectory;
	while (*pPtr==';' || *pPtr==' ')
		pPtr++;

	while (*pPtr!='\0')
	{
		// Check how long first string is
		if (pPtr[0]=='\"')
		{
			nLength=FirstCharIndex(pPtr+1,L'\"');
			sDirectory.Copy(pPtr+1,nLength);

			if (nLength!=-1)
			{
				for (nLength++;pPtr[nLength]!=';' && pPtr[nLength]!='\0';nLength++);
			}
			else
				nLength=istrlenw(pPtr);
		}
		else
		{
			nLength=FirstCharIndex(pPtr,L';');
			if (nLength==-1)
				nLength=istrlenw(pPtr);
			sDirectory.Copy(pPtr,nLength);

			while (sDirectory.LastChar()==' ')
				sDirectory.DelLastChar();
		}


		BOOL bFound=FALSE;
		for (int i=0;i<aDirectories.GetSize();i++)
		{
			if (sDirectory.Compare(aDirectories[i])==0)
			{
				// Directory already in the list
				bFound=TRUE;
			}
		}

		if (!bFound)
			aDirectories.Add(sDirectory.GiveBuffer());
		
		pPtr+=nLength;

		while (*pPtr==';' || *pPtr==' ')
			pPtr++;
	}
}

void CLocateDlg::CNameDlg::OnMoreDirectories()
{
	if (m_pMultiDirs==NULL)
		return;

	CMenu Menu;
	CStringW str;
	MENUITEMINFOW mii;
	
	Menu.CreatePopupMenu();

	CLocateDlg::InsertMenuItemsFromTemplate(Menu,GetLocateDlg()->m_Menu.GetSubMenu(SUBMENU_MULTIDIRSELECTION),0);
	
	mii.cbSize=sizeof(MENUITEMINFOW);
	// Inserting separator
	mii.fMask=MIIM_TYPE|MIIM_ID;
	mii.wID=IDM_DEFMENUITEM;
	mii.fType=MFT_STRING;
	int i;
	for (i=0;m_pMultiDirs[i]!=NULL;i++)
	{
		if (m_pMultiDirs[i]->bSelected)
		{
			EnterCriticalSection(&m_cBrowse);
			m_pMultiDirs[i]->SetValuesFromControl(m_LookIn,m_pBrowse,m_nMaxBrowse);
			LeaveCriticalSection(&m_cBrowse);
			mii.fMask=MIIM_TYPE|MIIM_ID|MIIM_STATE;
			mii.fState=MFS_ENABLED|MFS_CHECKED;
		}
		else
			mii.fMask=MIIM_TYPE|MIIM_ID;

		int nLen=10+istrlenw(m_pMultiDirs[i]->pTitleOrDirectory);
		WCHAR* pString=new WCHAR[nLen];
		StringCbPrintfW(pString,nLen*sizeof(WCHAR),L"%d: %s",i+1,m_pMultiDirs[i]->pTitleOrDirectory);
		mii.dwTypeData=pString;
	
		if (Menu.InsertMenu(i,TRUE,&mii))
			mii.wID++;

		delete[] pString;
	}

	if (i==1) // Only one item, disabling remove
		Menu.EnableMenuItem(IDM_LOOKINREMOVESELECTION,MF_BYCOMMAND|MF_GRAYED);

	RECT rcButton;
	::GetWindowRect(GetDlgItem(IDC_MOREDIRECTORIES),&rcButton);
		
	SetForegroundWindow();
	int wID=TrackPopupMenu(Menu,TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_NONOTIFY|TPM_RETURNCMD,rcButton.left,rcButton.bottom,0,*this,NULL);	
	Menu.DestroyMenu();

	switch (wID)
	{
	case IDM_LOOKINNEWSELECTION:
		OnLookInNewSelection();
		break;
	case IDM_LOOKINREMOVESELECTION:
		OnLookInRemoveSelection();
		break;
	case IDM_LOOKINNEXTSELECTION:
	case IDM_LOOKINPREVSELECTION:
		OnLookInNextSelection(wID==IDM_LOOKINNEXTSELECTION);
		break;
	default:
		if (wID>=IDM_DEFMENUITEM)
			OnLookInSelection(wID-IDM_DEFMENUITEM);
		break;
	}
}

void CLocateDlg::CNameDlg::LookInChangeSelection(int nCurrentSelection,int nNewSelection)
{
	// Current selection is chosen
    if (nCurrentSelection==nNewSelection)
		return;

	// Saving current selection
	EnterCriticalSection(&m_cBrowse);
	m_pMultiDirs[nCurrentSelection]->SetValuesFromControl(m_LookIn,m_pBrowse,m_nMaxBrowse);
	LeaveCriticalSection(&m_cBrowse);
	m_pMultiDirs[nCurrentSelection]->bSelected=FALSE;
			
	// Set new selection
	if (m_pMultiDirs[nNewSelection]->nType==Root)
		SelectByRootName(m_pMultiDirs[nNewSelection]->pTitleOrDirectory);
	else if (m_pMultiDirs[nNewSelection]->nType==Custom)
		SelectByCustomName(m_pMultiDirs[nNewSelection]->pTitleOrDirectory);
	else
	{
		EnterCriticalSection(&m_cBrowse);
		SelectByLParam(m_pMultiDirs[nNewSelection]->GetLParam(m_pBrowse,m_nMaxBrowse));
		LeaveCriticalSection(&m_cBrowse);
	}
	m_pMultiDirs[nNewSelection]->bSelected=TRUE;

	char szName[10];
	StringCbPrintf(szName,10,"#%d",nNewSelection+1);
	SetDlgItemText(IDC_MOREDIRECTORIES,szName);
}

void CLocateDlg::CNameDlg::OnLookInNextSelection(BOOL bNext)
{
	if (m_pMultiDirs==NULL)
		return;

	// Getting current selection
	int nCurrentSelection=-1,nSelection,nDirs;
	for (nDirs=0;m_pMultiDirs[nDirs]!=NULL;nDirs++)
	{
		if (m_pMultiDirs[nDirs]->bSelected)
			nCurrentSelection=nDirs;
	}

	ASSERT(nCurrentSelection!=-1);
		
	if (bNext)
	{
		if (nCurrentSelection+1>=nDirs)
			nSelection=0;
		else
			nSelection=nCurrentSelection+1;
	}
	else
	{
		if (nCurrentSelection<=0)
			nSelection=nDirs-1;
		else
			nSelection=nCurrentSelection-1;
	}

	LookInChangeSelection(nCurrentSelection,nSelection);
}
	
void CLocateDlg::CNameDlg::OnLookInSelection(int nSelection)
{
	if (m_pMultiDirs==NULL)
		return;

	int nCurrentSelection=-1,nDirs;
	for (nDirs=0;m_pMultiDirs[nDirs]!=NULL;nDirs++)
	{
		if (m_pMultiDirs[nDirs]->bSelected)
			nCurrentSelection=nDirs;
	}

	ASSERT(nCurrentSelection!=-1);
	
    // Item nSelection does not exists
	if (nSelection>=nDirs)
		return;
	LookInChangeSelection(nCurrentSelection,nSelection);
}


void CLocateDlg::CNameDlg::OnLookInNewSelection()
{
	if (m_pMultiDirs==NULL)
		return;

	int nSelected=0,nDirs;
	for (nDirs=0;m_pMultiDirs[nDirs]!=NULL;nDirs++)
	{
		if (m_pMultiDirs[nDirs]->bSelected)
		{
			nSelected=nDirs;
			m_pMultiDirs[nDirs]->bSelected=FALSE;
		}			
	}

	EnterCriticalSection(&m_cBrowse);
	m_pMultiDirs[nSelected]->SetValuesFromControl(m_LookIn,m_pBrowse,m_nMaxBrowse);
	LeaveCriticalSection(&m_cBrowse);
			
    DirSelection** pMultiDirsNew=new DirSelection*[nDirs+2];
	CopyMemory(pMultiDirsNew,m_pMultiDirs,sizeof(DirSelection*)*nDirs);
    delete[] m_pMultiDirs;
	m_pMultiDirs=pMultiDirsNew;

	m_pMultiDirs[nDirs++]=new DirSelection(TRUE);
    m_pMultiDirs[nDirs]=NULL;

	SelectByLParam(MAKELPARAM(Everywhere,Original));

	char szName[10];
	StringCbPrintf(szName,10,"#%d",nDirs);
	SetDlgItemText(IDC_MOREDIRECTORIES,szName);

	HilightTab(TRUE);
	GetLocateDlg()->OnFieldChange(isLookInChanged);
			
}

void CLocateDlg::CNameDlg::OnLookInRemoveSelection()
{
	if (m_pMultiDirs==NULL)
		return;
	
	int nCurrentSelection=-1,nDirs;
	for (nDirs=0;m_pMultiDirs[nDirs]!=NULL;nDirs++)
	{
		if (m_pMultiDirs[nDirs]->bSelected)
			nCurrentSelection=nDirs;
	}

    ASSERT(nCurrentSelection!=-1);
	if (nDirs<=1)
		return;

	DirSelection** pMultiDirsNew=new DirSelection*[nDirs];
	CopyMemory(pMultiDirsNew,m_pMultiDirs,sizeof(DirSelection*)*nCurrentSelection);
	CopyMemory(pMultiDirsNew+nCurrentSelection,m_pMultiDirs+nCurrentSelection+1,sizeof(DirSelection*)*(nDirs-nCurrentSelection));
    delete[] m_pMultiDirs;
	m_pMultiDirs=pMultiDirsNew;

	if (m_pMultiDirs[nCurrentSelection]==NULL)
		nCurrentSelection--;

	// Set new selection
	if (m_pMultiDirs[nCurrentSelection]->nType==Root)
		SelectByRootName(m_pMultiDirs[nCurrentSelection]->pTitleOrDirectory);
	else if (m_pMultiDirs[nCurrentSelection]->nType==Custom)
		SelectByCustomName(m_pMultiDirs[nCurrentSelection]->pTitleOrDirectory);
	else
	{
		EnterCriticalSection(&m_cBrowse);
		SelectByLParam(m_pMultiDirs[nCurrentSelection]->GetLParam(m_pBrowse,m_nMaxBrowse));
		LeaveCriticalSection(&m_cBrowse);
	}
	m_pMultiDirs[nCurrentSelection]->bSelected=TRUE;

	char szName[10];
	StringCbPrintf(szName,10,"#%d",nCurrentSelection+1);
	SetDlgItemText(IDC_MOREDIRECTORIES,szName);

	HilightTab(IsChanged());
	GetLocateDlg()->OnFieldChange(isLookInChanged);
			
}
		

void CLocateDlg::CNameDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType,cx,cy);
	
	CRect rc;
	m_Name.GetWindowRect(&rc);
	m_Name.SetWindowPos(HWND_BOTTOM,0,0,cx-m_nFieldLeft-8,rc.Height(),SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
	m_Type.SetWindowPos(HWND_BOTTOM,0,0,cx-m_nFieldLeft-8,rc.Height(),SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
	SetDlgItemPos(IDC_NOSUBDIRECTORIES,HWND_BOTTOM,cx-m_nButtonWidth-m_nCheckWidth-12,m_nBrowseTop+3,0,0,SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER|SWP_NOCOPYBITS);
	SetDlgItemPos(IDC_BROWSE,HWND_BOTTOM,cx-m_nButtonWidth-8,m_nBrowseTop,0,0,SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER|SWP_NOCOPYBITS);
	
	CLocateDlg* pLocateDlg=GetLocateDlg();
	if (pLocateDlg!=NULL && pLocateDlg->GetFlags()&CLocateDlg::fgNameMultibleDirectories)
	{
		m_LookIn.SetWindowPos(HWND_BOTTOM,0,0,cx-m_nFieldLeft-m_nMoreDirsWidth-11,rc.Height(),SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOCOPYBITS);
		SetDlgItemPos(IDC_MOREDIRECTORIES,HWND_BOTTOM,cx-m_nMoreDirsWidth-8,m_nMoreDirsTop,0,0,SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER|SWP_NOCOPYBITS);
	}
	else
		m_LookIn.SetWindowPos(HWND_BOTTOM,0,0,cx-m_nFieldLeft-8,rc.Height(),SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
	
}

void CLocateDlg::CNameDlg::OnClear(BOOL bInitial)
{
	m_Name.SetText(szEmpty);
	m_Type.SetText(szEmpty);
	m_Type.EnableWindow(TRUE);
	CheckDlgButton(IDC_NOSUBDIRECTORIES,FALSE);

	if (m_pMultiDirs!=NULL)
	{
		for (int i=0;m_pMultiDirs[i]!=NULL;i++)
			delete m_pMultiDirs[i];
		delete[] m_pMultiDirs;
		
		// Initializing struct
		m_pMultiDirs=new DirSelection*[2];
        m_pMultiDirs[0]=new DirSelection(TRUE);
		m_pMultiDirs[1]=NULL;

		SetDlgItemText(IDC_MOREDIRECTORIES,"#1");
	}
	
	for (int i=0;m_LookIn.GetCount();i++)
	{
		if (m_LookIn.GetItemData(i)==MAKELPARAM(Everywhere,Original))
		{
			m_LookIn.SetCurSel(i);
			m_LookIn.SetItemText(-1,m_LookIn.GetItemTextW(i));
			break;
		}
	}

	HilightTab(FALSE);
}

void CLocateDlg::CNameDlg::SaveRegistry() const
{
	if (GetLocateDlg()->GetExtraFlags()&efNameDontSaveNameTypeAndDirectories)
	{
		// Do not save list items, just delete key
		CRegKey2::DeleteCommonKey("\\Recent Strings");
		return;
	}

	CRegKey2 RegKey;
	if(RegKey.OpenKey(HKCU,"\\Recent Strings",CRegKey::defWrite)==ERROR_SUCCESS)
	{
		CStringW buffer,bfr;
		int i=0;

		// Remove existing items
		while(RegKey.EnumValue(0,buffer))
			RegKey.DeleteValue(buffer);
		
	
		RegKey.SetValue("NumberOfNames",m_nMaxNamesInList);
		for (i=m_Name.GetCount()-1;i>=0;i--)
		{
			bfr="Name";
			bfr<<int(i);
			m_Name.GetLBText(i,buffer);
			RegKey.SetValue(bfr,buffer);
		}
		
	
		RegKey.SetValue("NumberOfTypes",m_nMaxTypesInList);
		for (i=m_Type.GetCount()-1;i>0;i--) // 0 is (none)
		{
			bfr="Type";
			bfr<<int(i-1);
			m_Type.GetLBText(i,buffer);
			RegKey.SetValue(bfr,buffer);
		}
		
		EnterCriticalSection(&m_cBrowse);
		if (m_pBrowse!=NULL)
		{
			RegKey.SetValue("NumberOfDirectories",m_nMaxBrowse);
			for (i=0;i<int(m_nMaxBrowse);i++)
			{
				bfr="Directory";
				bfr<<(int)i;
				RegKey.SetValue(bfr,m_pBrowse[i]);
			}
		}
		LeaveCriticalSection(&m_cBrowse);
			
	}


}

void CLocateDlg::CNameDlg::LoadRegistry()
{
	CRegKey2 RegKey;
	m_Name.ResetContent();
	m_Type.ResetContent();
	m_Type.AddString(ID2W(IDS_NOEXTENSION));
	

	if (RegKey.OpenKey(HKCU,"\\Recent Strings",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
	{
		WORD i;
		CStringW name,buffer;
		
		RegKey.QueryValue("NumberOfNames",m_nMaxNamesInList);
		if (m_nMaxNamesInList>255)
			m_nMaxNamesInList=DEFAULT_NUMBEROFNAMES;

		RegKey.QueryValue("NumberOfTypes",m_nMaxTypesInList);
		if (m_nMaxTypesInList>255)
			m_nMaxTypesInList=DEFAULT_NUMBEROFTYPES;

		RegKey.QueryValue("NumberOfDirectories",m_nMaxBrowse);
		if (m_nMaxBrowse>255)
			m_nMaxBrowse=DEFAULT_NUMBEROFDIRECTORIES;

		


		for (i=0;i<m_nMaxNamesInList;i++)
		{
			name=L"Name";
			name<<(int)i;
			if (RegKey.QueryValue(name,buffer))
				m_Name.AddString(buffer);
		}
		for (i=0;i<m_nMaxTypesInList;i++)
		{
			name=L"Type";
			name<<(int)i;
			if (RegKey.QueryValue(name,buffer))
				m_Type.AddString(buffer);
		}

		EnterCriticalSection(&m_cBrowse);
		if (m_nMaxBrowse>0)
			m_pBrowse=new CStringW[m_nMaxBrowse];

		for (DWORD i=0;i<m_nMaxBrowse;i++)
		{
			name=L"Directory";
			name<<(int)i;
			if (!RegKey.QueryValue(name,m_pBrowse[i]))
				m_pBrowse[i].Empty();
		}
		LeaveCriticalSection(&m_cBrowse);

	}
	else
	{
		EnterCriticalSection(&m_cBrowse);
		if (m_pBrowse==NULL && m_nMaxBrowse>0)
			m_pBrowse=new CStringW[m_nMaxBrowse];
		LeaveCriticalSection(&m_cBrowse);
	}
}

BOOL CLocateDlg::CNameDlg::CheckAndAddDirectory(LPCWSTR pFolder,DWORD dwLength,BOOL bSaveAndSet,BOOL bNoWarning)
{
	CStringW FolderLower(pFolder,dwLength);
	FolderLower.MakeLower();

	DebugFormatMessage(L"Directory to add: %s",(LPCWSTR)FolderLower);

	COMBOBOXEXITEMW ci;


	// Checking whether path is a drive which already exists
	if (dwLength==2 && pFolder[1]==L':')
	{
		for (int i=0;i<m_LookIn.GetCount();i++)
		{
			LPARAM lParam=static_cast<TypeOfItem>(m_LookIn.GetItemData(i));
			if (static_cast<TypeOfItem>(LOWORD(lParam))==Drive)
			{
				WCHAR cDrive=(WCHAR)HIWORD(lParam);
				MakeLower(&cDrive,1);
				if (cDrive==FolderLower[0] && bSaveAndSet)
				{
					m_LookIn.SetCurSel(i);
					m_LookIn.SetItemText(-1,m_LookIn.GetItemTextW(i));
					return TRUE;
				}
			}
		}
	}

	if (!bSaveAndSet)
		return TRUE;

	if (GetLocateDlg()->GetExtraFlags()&CLocateDlg::efNameDontSaveNetworkDrivesAndDirectories)
	{
		if (pFolder[0]=='\\' && pFolder[1]=='\\')
		{
			// UNC path
			return TRUE;
		}

		if (pFolder[1]==':')
		{
			WCHAR pDrive[]=L"X:\\";
			pDrive[0]=pFolder[0];
			if (FileSystem::GetDriveType(pDrive)==DRIVE_REMOTE)
			{
				// Mapped drive
				return TRUE;
			}
		}
	}

	EnterCriticalSection(&m_cBrowse);

	if (m_pBrowse!=NULL)
	{
		// Check whether folder already exists in other directory list
		for (DWORD i=0;i<m_nMaxBrowse;i++)
		{
			CStringW str(m_pBrowse[i]);
			str.MakeLower();
			if (str.Compare(FolderLower)==0)
			{
				// Deleting previous one
				for (DWORD j=i+1;j<m_nMaxBrowse;j++)
					m_pBrowse[j-1].Swap(m_pBrowse[j]);
				m_pBrowse[m_nMaxBrowse-1].Empty();
			}
		}

		// moving previous folders one down
		for (int i=m_nMaxBrowse-1;i>0;i--)
			m_pBrowse[i].Swap(m_pBrowse[i-1]);
		
		m_pBrowse[0].Copy(pFolder,dwLength);

		
			
		// If selection is on, following code may mess it
		int nCurSel=m_LookIn.GetCurSel();
		m_LookIn.SetCurSel(-1);


		// Removing old items from list
		ci.mask=CBEIF_LPARAM;
		for (ci.iItem=m_LookIn.GetCount()-1;ci.iItem>=0;ci.iItem--)
		{
			m_LookIn.GetItem(&ci);
			if (static_cast<TypeOfItem>(LOWORD(ci.lParam))!=Custom)
				break;
			m_LookIn.DeleteItem((int)ci.iItem);
		}
		
		// Adding new items
		int nSel=(int)++ci.iItem;
		ci.iIndent=0;
		ci.mask=CBEIF_TEXT|CBEIF_IMAGE|CBEIF_INDENT|CBEIF_SELECTEDIMAGE|CBEIF_LPARAM;
		for (DWORD i=0;i<m_nMaxBrowse;i++)
		{
			if (m_pBrowse[i].IsEmpty())
				break;

			if (GetLocateApp()->GetProgramFlags()&CLocateApp::pfUseDefaultIconForDirectories)
			{
				ci.iImage=DIR_IMAGE;
				ci.iSelectedImage=DIR_IMAGE;
			}
			else
			{
				SHFILEINFOW fi;
				DWORD dwFlags=SHGFI_SYSICONINDEX|SHGFI_SMALLICON;
				if (GetLocateApp()->GetProgramFlags()&CLocateApp::pfAvoidToAccessWhenReadingIcons)
					dwFlags|=SHGFI_USEFILEATTRIBUTES;
				LPCWSTR pPath=m_pBrowse[i];
				if (pPath[0]==L'-')
					pPath++;
				ShellFunctions::GetFileInfo(pPath,FILE_ATTRIBUTE_DIRECTORY,&fi,dwFlags);
				ci.iImage=fi.iIcon;
				ShellFunctions::GetFileInfo(pPath,FILE_ATTRIBUTE_DIRECTORY,&fi,dwFlags|SHGFI_OPENICON);
				ci.iSelectedImage=fi.iIcon;
			}

		
			ci.pszText=m_pBrowse[i].GetBuffer();
			ci.lParam=MAKELPARAM(Custom,i);
			m_LookIn.InsertItem(&ci);
			ci.iItem++;
		}

		CLocateDlg* pLocateDlg=GetLocateDlg();
		BOOL bActivateInstantSearchListening=!pLocateDlg->IsInstantSearchingFlagSet(CLocateDlg::isIgnoreChangeMessages);
		pLocateDlg->AddInstantSearchingFlags(CLocateDlg::isIgnoreChangeMessages);

		m_LookIn.SetCurSel(nSel);
		m_LookIn.SetItemText(-1,m_LookIn.GetItemTextW(nSel));

		if (bActivateInstantSearchListening)
			pLocateDlg->RemoveInstantSearchingFlags(CLocateDlg::isIgnoreChangeMessages);

		
	}			
	
	LeaveCriticalSection(&m_cBrowse);
		
	return TRUE;		
}

void CLocateDlg::CNameDlg::OnBrowse()
{
	CWaitCursor wait;

	GetLocateDlg()->CancelInstantSearch();
	

	CFolderDialog fd(IDS_GETFOLDER,BIF_RETURNONLYFSDIRS|BIF_USENEWUI|BIF_NONEWFOLDERBUTTON);
	if (fd.DoModal(*this))
	{
		CStringW Folder;
		if (!fd.GetFolder(Folder))
		{
			DWORD dwLength=Network::GetComputerNameFromIDList(fd.m_lpil,Folder.GetBuffer(500),500);
			if (dwLength)
			{
				Folder.FreeExtra(dwLength);
				if (Folder[0]!=L'\\' || Folder[1]!=L'\\')
					Folder.Empty();
			}
		}

		if (!Folder.IsEmpty())
		{
			CStringW sCurrentLookIn;
			int nCurSel=m_LookIn.GetCurSel();
			if (nCurSel!=CB_ERR)
			{
				LPARAM lParam=m_LookIn.GetItemData(nCurSel);
				if (static_cast<TypeOfItem>(LOWORD(lParam))==Custom)
					sCurrentLookIn=m_LookIn.GetItemTextW(nCurSel);
			}
			else
				sCurrentLookIn=m_LookIn.GetItemTextW(-1);
			
					
			if (sCurrentLookIn.Find(L';')!=-1)
			{
				if (sCurrentLookIn.LastChar()!=L';')
					sCurrentLookIn<<L';';
				sCurrentLookIn << Folder;
				m_LookIn.SetItemText(-1,sCurrentLookIn);
			}
			else
				CheckAndAddDirectory(Folder,Folder.GetLength(),TRUE,FALSE);
		}

		m_LookIn.SetFocus();

		HilightTab(TRUE);
		GetLocateDlg()->OnFieldChange(isLookInChanged);
			
		return;
	}
	
}

void CLocateDlg::CNameDlg::EnableItems(BOOL bEnable)
{
	m_Name.EnableWindow(bEnable);
	m_Type.EnableWindow(bEnable && 
		GetLocateDlg()->m_AdvancedDlg.SendDlgItemMessage(IDC_FILETYPE,CB_GETCURSEL)==0);
	m_LookIn.EnableWindow(bEnable);
	EnableDlgItem(IDC_MOREDIRECTORIES,bEnable);
	EnableDlgItem(IDC_NOSUBDIRECTORIES,bEnable);
	EnableDlgItem(IDC_BROWSE,bEnable);
}

	
BOOL CLocateDlg::CNameDlg::EnableMultiDirectorySupport(BOOL bEnable)
{
	if (bEnable)
	{
		if (m_pMultiDirs!=NULL)
			return FALSE;

		// Showing control
		ShowDlgItem(IDC_MOREDIRECTORIES,swShow);
		
		// Initializing struct
		m_pMultiDirs=new DirSelection*[2];
        m_pMultiDirs[0]=new DirSelection(TRUE);
		m_pMultiDirs[1]=NULL;
		
		SetDlgItemText(IDC_MOREDIRECTORIES,"#1");
		return TRUE;
	}
	else
	{
		// Showing control
		ShowDlgItem(IDC_MOREDIRECTORIES,swHide);

		if (m_pMultiDirs==NULL)
			return FALSE;

		
		// Freeing memory
		if (m_pMultiDirs!=NULL)
		{
			for (int i=0;m_pMultiDirs[i]!=NULL;i++)
				delete m_pMultiDirs[i];
			delete[] m_pMultiDirs;
			m_pMultiDirs=NULL;
		}
		return TRUE;
	}
}

void CLocateDlg::CNameDlg::SetStartData(const CLocateApp::CStartData* pStartData,DWORD& rdwChanged)
{
	// Set string
	if (pStartData->m_pStartString!=NULL)
	{
		m_Name.SetText(pStartData->m_pStartString);
		rdwChanged|=isNameChanged;
	}

	if (pStartData->m_nStatus&CLocateApp::CStartData::statusNoExtension)
	{	
		m_Type.SetCurSel(0);
		rdwChanged|=isTypeChanged;
	}
	else if (pStartData->m_pTypeString!=NULL)
	{
		m_Type.SetText(pStartData->m_pTypeString);
		rdwChanged|=isTypeChanged;
	}

	if (pStartData->m_pStartPath!=NULL)
	{
		if (pStartData->m_pStartPath[1]==L'\0')
		{
			switch (pStartData->m_pStartPath[0])
			{
			case '0': // 'Everywhere'
				SelectByLParam(MAKELPARAM(Everywhere,Original));
				break;
			case '1': // 'Document folders'
				SelectByLParam(MAKELPARAM(Special,Documents));
				break;
			case '2': // 'Desktop'
				SelectByLParam(MAKELPARAM(Special,Desktop));
				break;
			case '3': // 'Documents'
				SelectByLParam(MAKELPARAM(Special,Personal));
				break;
			case '4': // 'Computer'
				SelectByLParam(MAKELPARAM(Special,MyComputer));
				break;
			default:
				SetPath(pStartData->m_pStartPath);
				break;
			}
		}
		else
			SetPath(pStartData->m_pStartPath);
		rdwChanged|=isLookInChanged;
	}
}

BOOL CLocateDlg::CNameDlg::SetPath(LPCWSTR szPath)
{
	WCHAR temp[MAX_PATH];
	LPWSTR tmp;
	int ret=_wtoi(szPath);


	if (szPath[0]==L':' && szPath[1]==':')
	{
		// szPath is CLSID
		CLSID clsid;
		if (SUCCEEDED(CLSIDFromString(LPWSTR(szPath)+2,&clsid)))
		{
			if (IsEqualCLSID(clsid,CLSID_MyComputer))
				SelectByLParam(MAKELPARAM(Special,MyComputer));
			else if (IsEqualCLSID(clsid,CLSID_MyDocuments))
				SelectByLParam(MAKELPARAM(Special,Personal));

		}
		return TRUE;

	}
	
	if ((ret>0 && ret<=4) || szPath[0]==L'0')
	{
		if (ret==0)
			SelectByLParam(MAKELPARAM(Everywhere,Original));
		else
			SelectByLParam(MAKELPARAM(Special,ret));
		return TRUE;	
	}

	ret=FileSystem::GetFullPathName(szPath,MAX_PATH,temp,&tmp);
	if (ret>1)
	{
		if (ret<4)
		{
			MakeUpper(temp);
			if (temp[1]==':')
			{
				COMBOBOXEXITEM ci;
				ci.mask=CBEIF_LPARAM;
				for (ci.iItem=m_LookIn.GetCount()-1;ci.iItem>=0;ci.iItem--)
				{
					m_LookIn.GetItem(&ci);
					if (static_cast<TypeOfItem>(LOWORD(ci.lParam))!=Drive)
						continue;
					if ((char)HIWORD(ci.lParam)==temp[0])
					{
						m_LookIn.SetCurSel((int)ci.iItem);
						m_LookIn.SetItemText(-1,m_LookIn.GetItemTextW((int)ci.iItem));
						return TRUE;
					}
				}
					
			}
		}
		

		EnterCriticalSection(&m_cBrowse);

		if (m_pBrowse!=NULL /* && FileSystem::IsDirectory(szPath) */) 
		{
			COMBOBOXEXITEMW ci;
			CStringW FolderLower(szPath);
			FolderLower.MakeLower();

			// Check whether folder already exists in other directory list
			for (DWORD i=0;i<m_nMaxBrowse;i++)
			{
				CStringW str(m_pBrowse[i]);
				str.MakeLower();
				if (str.Compare(FolderLower)==0)
				{
					// Deleting previous one
					for (DWORD j=i+1;j<m_nMaxBrowse;j++)
						m_pBrowse[j-1].Swap(m_pBrowse[j]);
					m_pBrowse[m_nMaxBrowse-1].Empty();
				}
			}

			// moving previous folders one down
			for (int i=m_nMaxBrowse-1;i>0;i--)
				m_pBrowse[i].Swap(m_pBrowse[i-1]);
			
			m_pBrowse[0].Copy(szPath);

			
				
			// If selection is on, following code may mess it
			m_LookIn.SetCurSel(-1);


			// Removing old items from list
			ci.mask=CBEIF_LPARAM;
			for (ci.iItem=m_LookIn.GetCount()-1;ci.iItem>=0;ci.iItem--)
			{
				m_LookIn.GetItem(&ci);
				if (static_cast<TypeOfItem>(LOWORD(ci.lParam))!=Custom)
					break;
				m_LookIn.DeleteItem((int)ci.iItem);
			}
			
			// Adding new items
			int nSel=(int)++ci.iItem;
			ci.iIndent=0;
			ci.mask=CBEIF_TEXT|CBEIF_IMAGE|CBEIF_INDENT|CBEIF_SELECTEDIMAGE|CBEIF_LPARAM;
			for (DWORD i=0;i<m_nMaxBrowse;i++)
			{
				if (m_pBrowse[i].IsEmpty())
					break;

				if (GetLocateApp()->GetProgramFlags()&CLocateApp::pfUseDefaultIconForDirectories)
				{
					ci.iImage=DIR_IMAGE;
					ci.iSelectedImage=DIR_IMAGE;
				}
				else
				{
					SHFILEINFOW fi;
					DWORD dwFlags=SHGFI_SYSICONINDEX|SHGFI_SMALLICON;
					if (GetLocateApp()->GetProgramFlags()&CLocateApp::pfAvoidToAccessWhenReadingIcons)
						dwFlags|=SHGFI_USEFILEATTRIBUTES;
					ShellFunctions::GetFileInfo(m_pBrowse[i],FILE_ATTRIBUTE_DIRECTORY,&fi,dwFlags);
					ci.iImage=fi.iIcon;
					ShellFunctions::GetFileInfo(m_pBrowse[i],FILE_ATTRIBUTE_DIRECTORY,&fi,dwFlags|SHGFI_OPENICON);
					ci.iSelectedImage=fi.iIcon;
				}

			
				ci.pszText=m_pBrowse[i].GetBuffer();
				ci.lParam=MAKELPARAM(Custom,i);
				m_LookIn.InsertItem(&ci);
				ci.iItem++;
			}

			m_LookIn.SetCurSel(nSel);
			m_LookIn.SetItemText(-1,m_LookIn.GetItemTextW(nSel));
			
			LeaveCriticalSection(&m_cBrowse);
			return TRUE;
		}

		LeaveCriticalSection(&m_cBrowse);
	}

	for (int i=0;m_LookIn.GetCount();i++)
	{
		if (m_LookIn.GetItemData(i)==MAKELPARAM(Everywhere,Original))
		{
			m_LookIn.SetCurSel(i);
			m_LookIn.SetItemText(-1,m_LookIn.GetItemTextW(i));
			break;
		}
	}
	return TRUE;
}

BOOL CLocateDlg::CNameDlg::GetDirectoriesForNonActiveSelection(CArray<LPWSTR>& aDirectories,const DirSelection* pSelection,BOOL bNoWarnings)
{
	ASSERT(!pSelection->bSelected);

	switch (pSelection->nType)
	{
	case Everywhere:
		return GetDirectoriesFromLParam(aDirectories,MAKELPARAM(Everywhere,pSelection->nEverywhereType));
	case Special:
		return GetDirectoriesFromLParam(aDirectories,MAKELPARAM(Special,pSelection->nSpecialType));
	case Drive:
		return GetDirectoriesFromLParam(aDirectories,MAKELPARAM(Drive,pSelection->cDriveLetter));
	case Custom:
		return GetDirectoriesFromCustomText(aDirectories,pSelection->pTitleOrDirectory,istrlenw(pSelection->pTitleOrDirectory),FALSE,bNoWarnings);
	case Root:
		aDirectories.Add(alloccopy(pSelection->pTitleOrDirectory));
		return TRUE;
	default:
		return FALSE;
	}
}

void CLocateDlg::CNameDlg::LoadControlStates(CRegKey& RegKey)
{
	CStringW str;
	RegKey.QueryValue(L"Name/Name",str);
	m_Name.SetText(str);

	DWORD dwDataLength,dwType;
	dwDataLength=RegKey.QueryValue("Name/Type",NULL,0,&dwType);

	if (dwType==REG_DWORD)
	{
		DWORD dwSel;
		RegKey.QueryValue("Name/Type",dwSel);
		m_Type.SetCurSel(dwSel);
	}
	else if (dwType==REG_SZ)
	{
		WCHAR* pData=new WCHAR[dwDataLength+2];
		if (RegKey.QueryValue(L"Name/Type",pData,dwDataLength+2)>0)
			m_Type.SetText(pData);	
		else
			m_Type.SetText(szEmpty);
		delete[] pData;
	}

	if (m_pMultiDirs!=NULL)
	{
		// Checking how many directories is in list
		char szName[200];
		CIntArray aSelections;
		
        for (int i=0;RegKey.EnumValue(i,szName,200);i++)
		{
			if (strncmp("Name/LookIn",szName,11)!=0)
				continue;

			int nSel=atoi(szName+11);
			
			// Checking whethe ID is number
			if (nSel==0 && szName[11]!='0')
				continue;

			aSelections.Add(nSel);
		}
	

        // Sorting, bubble sort
		int i;
		for (i=0;i<aSelections.GetSize();i++)
		{
			for (int j=0;j<i;j++)
			{
				if (aSelections[j]>aSelections[i])
				{
					int nTemp=aSelections[j];
					aSelections[j]=aSelections[i];
					aSelections[i]=nTemp;
				}
			}
		}

		for (int i=0;m_pMultiDirs[i]!=NULL;i++)
			delete m_pMultiDirs[i];
		delete[] m_pMultiDirs;
		if (aSelections.GetSize()>0)
		{
			m_pMultiDirs=new DirSelection*[aSelections.GetSize()+1];
			m_pMultiDirs[aSelections.GetSize()]=NULL;

			for (int i=aSelections.GetSize()-1;i>=0;i--)
			{
				StringCbPrintf(szName,200,"Name/LookIn%04d",aSelections[i]);
				
				LONG lLength=RegKey.QueryValueLength(szName);
				CHAR* pData=new CHAR[lLength+2];
				RegKey.QueryValue(szName,pData,lLength);
					
					
				if (*((WORD*)pData)==CNameDlg::NotSelected ||
					(*((WORD*)pData)==CNameDlg::Custom && lLength>4))
				{
					*((LPWSTR)(pData+lLength))=L'\0';
					
					SelectByCustomName((LPWSTR)(pData+4));
	
				}
				else if (lLength==4) 
				{
					DWORD dwLParam;
					RegKey.QueryValue(szName,dwLParam);
	                
					SelectByLParam(MAKELPARAM(HIWORD(dwLParam),LOWORD(dwLParam)));
				}
				
				delete[] pData;
				m_pMultiDirs[i]=new DirSelection(i==0);
				EnterCriticalSection(&m_cBrowse);
				m_pMultiDirs[i]->SetValuesFromControl(m_LookIn,m_pBrowse,m_nMaxBrowse);
				LeaveCriticalSection(&m_cBrowse);
			}
		}
		else
		{
			// It is possible that multidir mode was 
			// not enabled when state is saved, trying it (at least create empty selection)
			m_pMultiDirs=new DirSelection*[2];
			m_pMultiDirs[1]=NULL;
            
			LONG lLength=RegKey.QueryValueLength("Name/LookIn");
			CHAR* pData=new CHAR[lLength+2];
			RegKey.QueryValue(szName,pData,lLength);
			
			
			if (*((WORD*)pData)==CNameDlg::NotSelected ||
				(*((WORD*)pData)==CNameDlg::Custom && lLength>4))
			{
				*((LPWSTR)(pData+lLength))=L'\0';
				SelectByCustomName(LPWSTR(pData+4));
			}
			else if (lLength==4) 
			{
				DWORD dwLParam;
				RegKey.QueryValue(szName,dwLParam);
                
				SelectByLParam(MAKELPARAM(HIWORD(dwLParam),LOWORD(dwLParam)));
			}
				
			delete[] pData;

			m_pMultiDirs[i]=new DirSelection(TRUE);
			EnterCriticalSection(&m_cBrowse);
			m_pMultiDirs[i]->SetValuesFromControl(m_LookIn,m_pBrowse,m_nMaxBrowse);
			LeaveCriticalSection(&m_cBrowse);

		}

		DWORD nSelection=0;
		RegKey.QueryValue("Name/LookInSel",nSelection);

		// Marking #1 selected
		SetDlgItemText(IDC_MOREDIRECTORIES,"#1");

		OnLookInSelection(nSelection);
	}
	else
	{
        LONG lLength=RegKey.QueryValueLength("Name/LookIn");
		if (lLength<=0)
		{
			// It is possible that multidirecory mode was enabled
			// when state is saved
			lLength=RegKey.QueryValueLength("Name/LookIn0000");
		}

		if (lLength>=4) // Must be at least 4 bytes long
		{
	        CHAR* pData=new CHAR[lLength+2];
			RegKey.QueryValue("Name/LookIn",pData,lLength);
			if (*((WORD*)pData)==CNameDlg::NotSelected ||
				(*((WORD*)pData)==CNameDlg::Custom && lLength>4))
			{
				// Data is REG_BINARY so '\' is not added by default
				*((LPWSTR)(pData+lLength))=L'\0';
				SelectByCustomName(LPWSTR(pData+4));
			}
			else
			{
				// Choose by lParam

				LPARAM lParam=MAKELPARAM(((WORD*)pData)[1],((WORD*)pData)[0]);
				for (int i=0;i<m_LookIn.GetCount();i++)
				{
					if (lParam==m_LookIn.GetItemData(i))
					{
						m_LookIn.SetCurSel(i);
						m_LookIn.SetItemText(-1,m_LookIn.GetItemTextW(i));
						break;
					}
				}
			}
			delete[] pData;
		}
	}


	DWORD dwTemp;
	if (!RegKey.QueryValue("Name/NoSubDirectories",dwTemp))
		dwTemp=0;
	CheckDlgButton(IDC_NOSUBDIRECTORIES,dwTemp);



	HilightTab(IsChanged());
	
}

void CLocateDlg::CNameDlg::SaveControlStates(CRegKey& RegKey)
{
	CStringW str;
		
	// Name dialog
	m_Name.GetText(str);
	RegKey.SetValue(L"Name/Name",str);
	
	if (m_Type.GetCurSel()==0) // (none)
		RegKey.SetValue("Name/Type",DWORD(0));
	else
	{
		m_Type.GetText(str);
		RegKey.SetValue(L"Name/Type",str);
	}
	
	
	// Lookin Combo
	
	// Deleting old items
	char szName[200];
	for (int i=0;RegKey.EnumValue(i,szName,200);)
	{
		if (strncmp("Name/LookIn",szName,11)==0)
			RegKey.DeleteValue(szName);
		else
			i++;
	}



	if (m_pMultiDirs==NULL)
	{
		int nCurSel=m_LookIn.GetCurSel();
		LPARAM lParam=MAKELPARAM(NotSelected,0);
		if (nCurSel!=CB_ERR)
            lParam=m_LookIn.GetItemData(nCurSel);
			
		if (LOWORD(lParam)==NotSelected || 
			LOWORD(lParam)==Custom)
		{
			m_LookIn.GetItemText(-1,str.GetBuffer(2000),2000);
			str.FreeExtra();
			char* pData=new char[str.GetLength()*2+4];
			*((DWORD*)pData)=(DWORD)lParam;
			CopyMemory(pData+4,LPCWSTR(str),str.GetLength()*2);
			RegKey.SetValue("Name/LookIn",pData,str.GetLength()*2+4,REG_BINARY);
		}
		else
			RegKey.SetValue("Name/LookIn",(DWORD)MAKELONG(HIWORD(lParam),LOWORD(lParam)));            
	}
	else
	{
		for (int i=0;m_pMultiDirs[i]!=NULL;i++)
		{
			StringCbPrintf(szName,200,"Name/LookIn%04d",i);
					
			if (m_pMultiDirs[i]->bSelected)
			{
				// Current selection
				int nCurSel=m_LookIn.GetCurSel();
				LPARAM lParam=MAKELPARAM(NotSelected,0);
				if (nCurSel!=CB_ERR)
					lParam=m_LookIn.GetItemData(nCurSel);
				
				if (LOWORD(lParam)==NotSelected || 
					LOWORD(lParam)==Custom)
				{
					m_LookIn.GetItemText(-1,str.GetBuffer(2000),2000);
					str.FreeExtra();
					char* pData=new char[str.GetLength()*2+4];
					*((DWORD*)pData)=(DWORD)lParam;
					CopyMemory(pData+4,LPCWSTR(str),str.GetLength()*2);
					RegKey.SetValue(szName,pData,DWORD(str.GetLength()*2+4),REG_BINARY);
					delete[] pData;
				}
				else
				{
					LPARAM lParam=m_LookIn.GetItemData(nCurSel);
					RegKey.SetValue(szName,MAKELONG(HIWORD(lParam),LOWORD(lParam)));            
				}

				RegKey.SetValue("Name/LookInSel",DWORD(i));
			}
			else if (m_pMultiDirs[i]->nType==Custom)
			{	
				DWORD dwLength=istrlenw(m_pMultiDirs[i]->pTitleOrDirectory)*2;
				
				char* pData=new char[dwLength+4];
				*((DWORD*)pData)=DWORD(Custom);
				CopyMemory(pData+4,m_pMultiDirs[i]->pTitleOrDirectory,dwLength);
				RegKey.SetValue(szName,pData,dwLength+4,REG_BINARY);
				delete[] pData;
			}
			else
			{
				EnterCriticalSection(&m_cBrowse);
				LPARAM lParam=m_pMultiDirs[i]->GetLParam(m_pBrowse,m_nMaxBrowse);
				LeaveCriticalSection(&m_cBrowse);
				RegKey.SetValue(szName,(DWORD)MAKELONG(HIWORD(lParam),LOWORD(lParam)));
			}


		}
	}


	// No subdirectories
	RegKey.SetValue("Name/NoSubDirectories",IsDlgButtonChecked(IDC_NOSUBDIRECTORIES));

}

//////////////////////////////////////////////////////////////////////////////
// CLocateDlg::CNameDlg::DirSelection
//////////////////////////////////////////////////////////////////////////////

void CLocateDlg::CNameDlg::DirSelection::SetValuesFromControl(HWND hControl,const CStringW* pBrowse,int nBrowseDirs)
{
	FreeData();

	int nCurSel=(int)::SendMessage((HWND)::SendMessage(hControl,CBEM_GETCOMBOCONTROL,0,0),CB_GETCURSEL,0,0);

	COMBOBOXEXITEMW ci;
	ci.mask=CBEIF_LPARAM|CBEIF_TEXT;
	ci.iItem=nCurSel;
	WCHAR szTitle[MAX_PATH+10]=L"";
	ci.pszText=szTitle;
	ci.cchTextMax=MAX_PATH+10;
	


	if (!::SendMessage(hControl,CBEM_GETITEMW,0,(LPARAM)&ci))
		return;
	
	if (nCurSel==-1)
	{
		// No selection, custom
		DWORD dwLength=istrlenw(szTitle);

		if (dwLength<MAX_PATH+9)
		{
            // Whole string is in szTitle
			nType=CLocateDlg::CNameDlg::Custom;
			pTitleOrDirectory=alloccopy(szTitle,dwLength);
		}
		else
		{
            DWORD dwBufferSize=2*MAX_PATH;
			WCHAR* pFolder;
		
			for(;;)
			{	
				pFolder=new WCHAR[dwBufferSize];
				ci.mask=CBEIF_TEXT;
				ci.pszText=pFolder;

				if (!::SendMessage(hControl,CBEM_GETITEMW,0,(LPARAM)&ci))
				{
					delete[] pFolder;
					return;
				}

				dwLength=istrlenw(pFolder);
			
				if (dwLength==dwBufferSize-1)
				{
					delete[] pFolder;
					dwBufferSize*=2;
				}
				else
					break;
			}

			pTitleOrDirectory=alloccopy(pFolder,dwLength);
			delete[] pFolder;
		}		
	}
	else
	{
		// Selection
		nType=static_cast<CLocateDlg::CNameDlg::TypeOfItem>(LOWORD(ci.lParam));
		
		if (nType!=CLocateDlg::CNameDlg::Custom)
			pTitleOrDirectory=alloccopy(szTitle);

		switch (nType)
		{
		case CLocateDlg::CNameDlg::Everywhere:
			nEverywhereType=static_cast<CLocateDlg::CNameDlg::EveryWhereType>(HIWORD(ci.lParam));
			break;
		case CLocateDlg::CNameDlg::Special:
			nSpecialType=static_cast<CLocateDlg::CNameDlg::SpecialType>(HIWORD(ci.lParam));
			break;
		case CLocateDlg::CNameDlg::Drive:
			cDriveLetter=static_cast<char>(HIWORD(ci.lParam));
			break;
		case CLocateDlg::CNameDlg::Custom:
			if (int(HIWORD(ci.lParam))>=0 && HIWORD(ci.lParam)<nBrowseDirs)
				pTitleOrDirectory=alloccopy(pBrowse[HIWORD(ci.lParam)]);
			break;
		case CLocateDlg::CNameDlg::Root:
			break;
		}
	}
}

LPARAM CLocateDlg::CNameDlg::DirSelection::GetLParam(const CStringW* pBrowse,int nBrowseDirs) const
{
	switch (nType)
	{
	case CLocateDlg::CNameDlg::Everywhere:
		return MAKELPARAM(CLocateDlg::CNameDlg::Everywhere,nEverywhereType);
	case CLocateDlg::CNameDlg::Special:
		return MAKELPARAM(CLocateDlg::CNameDlg::Special,nSpecialType);
	case CLocateDlg::CNameDlg::Drive:
		return MAKELPARAM(CLocateDlg::CNameDlg::Drive,cDriveLetter);
	case CLocateDlg::CNameDlg::Custom:
		for (WORD i=0;i<nBrowseDirs;i++)
		{
			if (pBrowse[0].CompareNoCase(pTitleOrDirectory)==0)
				return MAKELPARAM(CLocateDlg::CNameDlg::Custom,i);
		}
        return MAKELPARAM(CLocateDlg::CNameDlg::Custom,0xFFFF);
	case CLocateDlg::CNameDlg::Root:
		return MAKELPARAM(CLocateDlg::CNameDlg::Root,CLocateDlg::CNameDlg::ComputeChecksumFromDir(pTitleOrDirectory));
	default:
		return MAKELPARAM(CLocateDlg::CNameDlg::NotSelected,0xFFFF);
	}
}


//////////////////////////////////////////////////////////////////////////////
// CLocateDlg::CSizeDateDlg
//////////////////////////////////////////////////////////////////////////////

BOOL CLocateDlg::CSizeDateDlg::OnInitDialog(HWND hwndFocus)
{
	CDialog::OnInitDialog(hwndFocus);
	
	GetLocateDlg()->AddInstantSearchingFlags(isIgnoreChangeMessages);

	// Set spins
	SendDlgItemMessage(IDC_MINIMUMSIZESPIN,UDM_SETBUDDY,(WPARAM)GetDlgItem(IDC_MINIMUMSIZE),0);
	SendDlgItemMessage(IDC_MAXIMUMSIZESPIN,UDM_SETBUDDY,(WPARAM)GetDlgItem(IDC_MAXIMUMSIZE),0);
	SendDlgItemMessage(IDC_MINIMUMSIZESPIN,UDM_SETRANGE,0,MAKELPARAM(UD_MAXVAL,0));
	SendDlgItemMessage(IDC_MAXIMUMSIZESPIN,UDM_SETRANGE,0,MAKELPARAM(UD_MAXVAL,0));
	UDACCEL ua[] = {
		{ 1,1 },
		{ 2,5 },
		{ 3,10 },
		{ 4,50 },
		{ 5,250 },
		{ 6,500 },
		{ 7,1000 },
		{ 8,2000 },
		{ 9,5000 },
		{ 10, 10000 }
	};
	SendDlgItemMessage(IDC_MINIMUMSIZESPIN,UDM_SETACCEL,10,(LPARAM)ua);
	SendDlgItemMessage(IDC_MAXIMUMSIZESPIN,UDM_SETACCEL,10,(LPARAM)ua);

	CComboBox Min(GetDlgItem(IDC_MINTYPE)),Max(GetDlgItem(IDC_MAXTYPE));
	CStringW str;
	str.LoadString(IDS_MODIFIED);
	Min.AddString(str);
	Max.AddString(str);
	str.LoadString(IDS_CREATED);
	Min.AddString(str);
	Max.AddString(str);
	str.LoadString(IDS_LASTACCESSED);
	Min.AddString(str);
	Max.AddString(str);
	
	Min.AttachToDlgItem(*this,IDC_MINSIZETYPE);
	Max.AttachToDlgItem(*this,IDC_MAXSIZETYPE);
	str.LoadString(IDS_SIZEBYTES);
	Min.AddString(str);
	Max.AddString(str);
	str.LoadString(IDS_SIZEKB);
	Min.AddString(str);
	Max.AddString(str);
	str.LoadString(IDS_SIZEMB);
	Min.AddString(str);
	Max.AddString(str);
	
	OnClear(TRUE);
	
	GetLocateDlg()->RemoveInstantSearchingFlags(isIgnoreChangeMessages);
	return FALSE;
}

BOOL CLocateDlg::CSizeDateDlg::OnCommand(WORD wID,WORD wNotifyCode,HWND hControl)
{
	CLocateDlg* pLocateDlg=GetLocateDlg();
	if (pLocateDlg==NULL)
		return FALSE;

	CDialog::OnCommand(wID,wNotifyCode,hControl);
	switch (wID)
	{
	case IDC_CHECKMINIMUMSIZE:
		if (hControl==NULL && wNotifyCode==1) // Accelerator
			CheckDlgButton(IDC_CHECKMINIMUMSIZE,!IsDlgButtonChecked(IDC_CHECKMINIMUMSIZE));

		if (IsDlgButtonChecked(IDC_CHECKMINIMUMSIZE))
		{
			EnableDlgItem(IDC_MINIMUMSIZE,TRUE);
			EnableDlgItem(IDC_MINIMUMSIZESPIN,TRUE);
			EnableDlgItem(IDC_MINSIZETYPE,TRUE);
			SetFocus(IDC_MINIMUMSIZE);

			HilightTab(TRUE);
		}
		else
		{
			EnableDlgItem(IDC_MINIMUMSIZE,FALSE);
			EnableDlgItem(IDC_MINIMUMSIZESPIN,FALSE);
			EnableDlgItem(IDC_MINSIZETYPE,FALSE);

			if (hControl==NULL && wNotifyCode==1) // Accelerator
				SetFocus(IDC_CHECKMINIMUMSIZE);

			HilightTab(IsChanged());
		}
		pLocateDlg->m_AdvancedDlg.ChangeEnableStateForCheck();
		
		pLocateDlg->OnFieldChange(isSizesChanged);
		break;
	case IDC_CHECKMAXIMUMSIZE:
		if (hControl==NULL && wNotifyCode==1) // Accelerator
			CheckDlgButton(IDC_CHECKMAXIMUMSIZE,!IsDlgButtonChecked(IDC_CHECKMAXIMUMSIZE));

		if (IsDlgButtonChecked(IDC_CHECKMAXIMUMSIZE))
		{
			EnableDlgItem(IDC_MAXIMUMSIZE,TRUE);
			EnableDlgItem(IDC_MAXIMUMSIZESPIN,TRUE);
			EnableDlgItem(IDC_MAXSIZETYPE,TRUE);
			SetFocus(IDC_MAXIMUMSIZE);
			HilightTab(TRUE);
		
		}
		else
		{
			EnableDlgItem(IDC_MAXIMUMSIZE,FALSE);
			EnableDlgItem(IDC_MAXIMUMSIZESPIN,FALSE);
			EnableDlgItem(IDC_MAXSIZETYPE,FALSE);

			if (hControl==NULL && wNotifyCode==1) // Accelerator
				SetFocus(IDC_CHECKMAXIMUMSIZE);

			HilightTab(IsChanged());
		}
		pLocateDlg->m_AdvancedDlg.ChangeEnableStateForCheck();
		
		pLocateDlg->OnFieldChange(isSizesChanged);
		break;
	case IDC_CHECKMINDATE:
		if (hControl==NULL && wNotifyCode==1) // Accelerator
			CheckDlgButton(IDC_CHECKMINDATE,!IsDlgButtonChecked(IDC_CHECKMINDATE));

		if (IsDlgButtonChecked(IDC_CHECKMINDATE))
		{
			EnableDlgItem(IDC_MINDATE,TRUE);
			EnableDlgItem(IDC_MINTYPE,TRUE);
			SetFocus(IDC_MINDATE);
			
			HilightTab(TRUE);
		}
		else
		{
			EnableDlgItem(IDC_MINDATE,FALSE);
			EnableDlgItem(IDC_MINTYPE,FALSE);
			
			if (hControl==NULL && wNotifyCode==1) // Accelerator
				SetFocus(IDC_CHECKMINDATE);
			HilightTab(IsChanged());
		}
		
		pLocateDlg->OnFieldChange(isDatesChanged);
		break;
	case IDC_CHECKMAXDATE:
		if (hControl==NULL && wNotifyCode==1) // Accelerator
			CheckDlgButton(IDC_CHECKMAXDATE,!IsDlgButtonChecked(IDC_CHECKMAXDATE));

		if (IsDlgButtonChecked(IDC_CHECKMAXDATE))
		{
			EnableDlgItem(IDC_MAXDATE,TRUE);
			EnableDlgItem(IDC_MAXTYPE,TRUE);

			SetFocus(IDC_MAXDATE);
			HilightTab(TRUE);
		}
		else
		{
			EnableDlgItem(IDC_MAXDATE,FALSE);
			EnableDlgItem(IDC_MAXTYPE,FALSE);

			if (hControl==NULL && wNotifyCode==1) // Accelerator
				SetFocus(IDC_CHECKMAXDATE);
			HilightTab(IsChanged());
		}

		pLocateDlg->OnFieldChange(isDatesChanged);
		break;
	case IDC_MINIMUMSIZE:
	case IDC_MAXIMUMSIZE:
		if (hControl==NULL && wNotifyCode==1) // Accelerator
			SetFocus(wID);
		else if (wNotifyCode==EN_SETFOCUS)
			::SendMessage(hControl,EM_SETSEL,0,MAKELPARAM(0,-1));
		else if (wNotifyCode==EN_CHANGE)
			pLocateDlg->OnFieldChange(isSizesChanged);
		break;
	case IDC_MAXDATEMODE:
	case IDC_MINDATEMODE:
		{
			CDateTimeCtrlEx* pCtrl=(CDateTimeCtrlEx*)SendDlgItemMessage(
				wID==IDC_MAXDATEMODE?IDC_MAXDATE:IDC_MINDATE,DTXM_GETCLASS);
			if (pCtrl->IsWindowEnabled())
				pCtrl->ChangeMode(!pCtrl->GetMode());

			pLocateDlg->OnFieldChange(isDatesChanged);
			break;
		}
	case IDC_MINDATE:
	case IDC_MAXDATE:
		if (hControl==NULL && wNotifyCode==1) // Accelerator
			SetFocus(wID);
		else if (wNotifyCode==DTXN_CHANGE || wNotifyCode==DTXN_MODECHANGED)
			pLocateDlg->OnFieldChange(isDatesChanged);
		break;
	case IDC_MINSIZETYPE:
	case IDC_MAXSIZETYPE:
		if (hControl==NULL && wNotifyCode==1) // Accelerator
			SetFocus(wID);
		else if (wNotifyCode==CBN_SELCHANGE)
			pLocateDlg->OnFieldChange(isSizesChanged);
		break;
	case IDC_MINTYPE:
	case IDC_MAXTYPE:
		if (hControl==NULL && wNotifyCode==1) // Accelerator
			SetFocus(wID);
		else if (wNotifyCode==CBN_SELCHANGE)
			pLocateDlg->OnFieldChange(isDatesChanged);
		break;
		
	}
	return FALSE;
}




BOOL CLocateDlg::CSizeDateDlg::SetSizesAndDaterForLocater(CLocater* pLocater)
{
	DWORD dwFlags=0;
	ULONGLONG ullMinSize=ULONGLONG(-1),ullMaxSize=ULONGLONG(-1);
	WORD wMinDate=WORD(-1),wMaxDate=WORD(-1);

	if (IsDlgButtonChecked(IDC_CHECKMINIMUMSIZE) &&
		GetDlgItemTextLength(IDC_MINIMUMSIZE)>0)
	{
		BOOL bError;
		WCHAR szBuffer[128],*pEnd;
		GetDlgItemText(IDC_MINIMUMSIZE,szBuffer,128);
		ullMinSize=_wcstoi64(szBuffer,&pEnd,10);
		if (ullMinSize==0)
		{
			ullMinSize=(ULONGLONG)SendDlgItemMessage(IDC_MINIMUMSIZESPIN,UDM_GETPOS32,0,(LPARAM)&bError);
			if (bError)
			{
				ullMinSize=GetDlgItemInt(IDC_MINIMUMSIZE,&bError,FALSE);
				if (!bError)
					ullMinSize=ULONGLONG(-1);
			}
		}
		
		if (ullMinSize!=ULONGLONG(-1))
		{
			int nCurSel=(int)SendDlgItemMessage(IDC_MINSIZETYPE,CB_GETCURSEL,0,0);
			if (nCurSel)
				ullMinSize*=1024;
			if (nCurSel==2)
				ullMinSize*=1024;
		}
	}
	if (IsDlgButtonChecked(IDC_CHECKMAXIMUMSIZE) && 
		GetDlgItemTextLength(IDC_MAXIMUMSIZE)>0)
	{
		BOOL bError;
		WCHAR szBuffer[128],*pEnd;
		GetDlgItemText(IDC_MAXIMUMSIZE,szBuffer,128);
		ullMaxSize=_wcstoi64(szBuffer,&pEnd,10);
		if (ullMaxSize==0)
		{
			ullMaxSize=(ULONGLONG)SendDlgItemMessage(IDC_MAXIMUMSIZESPIN,UDM_GETPOS32,0,(LPARAM)&bError);
			if (bError)
			{
				ullMaxSize=GetDlgItemInt(IDC_MAXIMUMSIZE,&bError,FALSE);
				if (!bError)
					ullMaxSize=ULONGLONG(-1);
			}
		}

		if (ullMaxSize!=ULONGLONG(-1))
		{
			int nCurSel=(int)SendDlgItemMessage(IDC_MAXSIZETYPE,CB_GETCURSEL,0,0);
			if (nCurSel)
				ullMaxSize*=1024;
			if (nCurSel==2)
				ullMaxSize*=1024;
		}
	}
	if (IsDlgButtonChecked(IDC_CHECKMINDATE))
	{
		SYSTEMTIME st;
		SendDlgItemMessage(IDC_MINDATE,DTM_GETSYSTEMTIME,0,(LPARAM)&st);
		wMinDate=st.wYear<1980?WORD(-2):SYSTEMTIMETODOSDATE(st);
		int nCurSel=(int)SendDlgItemMessage(IDC_MINTYPE,CB_GETCURSEL,0,0);
		if (nCurSel==1)
			dwFlags|=LOCATE_MINCREATIONDATE;
		else if (nCurSel==2)
			dwFlags|=LOCATE_MINACCESSDATE;
	}
	if (IsDlgButtonChecked(IDC_CHECKMAXDATE))
	{
		SYSTEMTIME st;
		SendDlgItemMessage(IDC_MAXDATE,DTM_GETSYSTEMTIME,0,(LPARAM)&st);
		wMaxDate=st.wYear<1980?WORD(-2):SYSTEMTIMETODOSDATE(st);
		
		int nCurSel=(int)SendDlgItemMessage(IDC_MAXTYPE,CB_GETCURSEL,0,0);
		if (nCurSel==1)
			dwFlags|=LOCATE_MAXCREATIONDATE;
		else if (nCurSel==2)
			dwFlags|=LOCATE_MAXACCESSDATE;
	}

	pLocater->SetSizeAndDate(dwFlags,ullMinSize,ullMaxSize,wMinDate,wMaxDate);
	
	return TRUE;
}
	
void CLocateDlg::CSizeDateDlg::OnClear(BOOL bInitial)
{
	CheckDlgButton(IDC_CHECKMINIMUMSIZE,0);
	CheckDlgButton(IDC_CHECKMAXIMUMSIZE,0);
	CheckDlgButton(IDC_CHECKMINDATE,0);
	CheckDlgButton(IDC_CHECKMAXDATE,0);
	EnableDlgItem(IDC_MINIMUMSIZE,FALSE);
	EnableDlgItem(IDC_MAXIMUMSIZE,FALSE);
	EnableDlgItem(IDC_MINIMUMSIZESPIN,FALSE);
	EnableDlgItem(IDC_MAXIMUMSIZESPIN,FALSE);
	EnableDlgItem(IDC_MINSIZETYPE,FALSE);
	EnableDlgItem(IDC_MAXSIZETYPE,FALSE);
	EnableDlgItem(IDC_MINDATE,FALSE);
	EnableDlgItem(IDC_MINTYPE,FALSE);
	EnableDlgItem(IDC_MAXDATE,FALSE);
	EnableDlgItem(IDC_MAXTYPE,FALSE);
	SendDlgItemMessage(IDC_MINIMUMSIZESPIN,UDM_SETPOS,0,MAKELPARAM(0,0));
	SendDlgItemMessage(IDC_MAXIMUMSIZESPIN,UDM_SETPOS,0,MAKELPARAM(0,0));

	CDateTimeCtrlEx* pMinDateCtrl=CDateTimeCtrlEx::GetClass(GetDlgItem(IDC_MINDATE));
	CDateTimeCtrlEx* pMaxDateCtrl=CDateTimeCtrlEx::GetClass(GetDlgItem(IDC_MAXDATE));

	SYSTEMTIME st;
	GetLocalTime(&st);
	if (pMinDateCtrl!=NULL)
	{
		pMinDateCtrl->SetExplicitDate(&st,GDT_VALID|DTXF_NOMODECHANGE);
		pMinDateCtrl->SetRelativeDate(0,DTXF_NOMODECHANGE);
	}
	if (pMaxDateCtrl!=NULL)
	{
		pMaxDateCtrl->SetExplicitDate(&st,GDT_VALID|DTXF_NOMODECHANGE);
		pMaxDateCtrl->SetRelativeDate(0,DTXF_NOMODECHANGE);
	}

	if (bInitial)
	{
		DWORD dwMinType=0,dwMaxType=0,dwMinSizeType=0,dwMaxSizeType=0;
		CRegKey2 RegKey;
		if(RegKey.OpenKey(HKCU,"\\Dialogs",CRegKey::defRead)==ERROR_SUCCESS)
		{
			RegKey.QueryValue("SizeDate/MinimumSizeType",dwMinSizeType);
			RegKey.QueryValue("SizeDate/MaximumSizeType",dwMaxSizeType);
			
			char szTemp[sizeof(SYSTEMTIME)+4];
			DWORD dwLength=RegKey.QueryValue("SizeDate/MaximumDate",szTemp,sizeof(SYSTEMTIME)+4);
			if (dwLength>=sizeof(DWORD))
			{
				dwMaxType=LOWORD(*((DWORD*)szTemp));
				if (pMaxDateCtrl!=NULL)
					pMaxDateCtrl->ChangeMode(HIWORD(*((DWORD*)szTemp)));

			}

			dwLength=RegKey.QueryValue("SizeDate/MinimumDate",szTemp,sizeof(SYSTEMTIME)+4);
			if (dwLength>=sizeof(DWORD))
			{
				dwMinType=LOWORD(*((DWORD*)szTemp));
				if (pMinDateCtrl!=NULL)
					pMinDateCtrl->ChangeMode(HIWORD(*((DWORD*)szTemp)));
			}
		}
		
		SendDlgItemMessage(IDC_MINTYPE,CB_SETCURSEL,dwMinType,0);
		SendDlgItemMessage(IDC_MAXTYPE,CB_SETCURSEL,dwMaxType,0);
		SendDlgItemMessage(IDC_MINSIZETYPE,CB_SETCURSEL,dwMinSizeType,0);
		SendDlgItemMessage(IDC_MAXSIZETYPE,CB_SETCURSEL,dwMaxSizeType,0);
		
	}


	HilightTab(FALSE);
}

void CLocateDlg::CSizeDateDlg::EnableItems(BOOL bEnable_)
{
	EnableDlgItem(IDC_CHECKMINIMUMSIZE,bEnable_);
	BOOL bEnable=bEnable_ && IsDlgButtonChecked(IDC_CHECKMINIMUMSIZE);
	EnableDlgItem(IDC_MINIMUMSIZE,bEnable);
	EnableDlgItem(IDC_MINIMUMSIZESPIN,bEnable);
	EnableDlgItem(IDC_MINSIZETYPE,bEnable);
	
	EnableDlgItem(IDC_CHECKMAXIMUMSIZE,bEnable_);
	bEnable=bEnable_ && IsDlgButtonChecked(IDC_CHECKMAXIMUMSIZE);
	EnableDlgItem(IDC_MAXIMUMSIZE,bEnable);
	EnableDlgItem(IDC_MAXIMUMSIZESPIN,bEnable);
	EnableDlgItem(IDC_MAXSIZETYPE,bEnable);
	
	EnableDlgItem(IDC_CHECKMINDATE,bEnable_);
	bEnable=bEnable_ && IsDlgButtonChecked(IDC_CHECKMINDATE);
	EnableDlgItem(IDC_MINDATE,bEnable);
	EnableDlgItem(IDC_MINTYPE,bEnable);
	
	EnableDlgItem(IDC_CHECKMAXDATE,bEnable_);
	bEnable=bEnable_ && IsDlgButtonChecked(IDC_CHECKMAXDATE);
	EnableDlgItem(IDC_MAXDATE,bEnable);
	EnableDlgItem(IDC_MAXTYPE,bEnable);
}


void CLocateDlg::CSizeDateDlg::SetStartData(const CLocateApp::CStartData* pStartData,DWORD& rdwChanged)
{
	if (pStartData->m_dwMinFileSize!=DWORD(-1))
	{
		CheckDlgButton(IDC_CHECKMINIMUMSIZE,1);
		SetDlgItemInt(IDC_MINIMUMSIZE,pStartData->m_dwMinFileSize);
		switch (pStartData->m_cMinSizeType)
		{
		case 'k':
		case 'K':
			SendDlgItemMessage(IDC_MINSIZETYPE,CB_SETCURSEL,1);
			break;
		case 'm':
		case 'M':
			SendDlgItemMessage(IDC_MINSIZETYPE,CB_SETCURSEL,2);
			break;
		}
		rdwChanged|=isSizesChanged;
	}

	if (pStartData->m_dwMaxFileSize!=DWORD(-1))
	{
		CheckDlgButton(IDC_CHECKMAXIMUMSIZE,1);
		SetDlgItemInt(IDC_MAXIMUMSIZE,pStartData->m_dwMaxFileSize);
		switch (pStartData->m_cMaxSizeType)
		{
		case 'k':
		case 'K':
			SendDlgItemMessage(IDC_MAXSIZETYPE,CB_SETCURSEL,1);
			break;
		case 'm':
		case 'M':
			SendDlgItemMessage(IDC_MAXSIZETYPE,CB_SETCURSEL,2);
			break;
		}

		rdwChanged|=isSizesChanged;
	}

	if (pStartData->m_dwMinDate!=DWORD(-1))
	{
		CheckDlgButton(IDC_CHECKMINDATE,1);
        SYSTEMTIME st;
		st.wHour=1;
		st.wMinute=1;
		st.wSecond=1;
		st.wMilliseconds=1;
		if (pStartData->m_dwMinDate&(1<<31))
		{
			st.wDay=WORD(pStartData->m_dwMinDate);
			st.wMonth=0xFFFF;
			st.wYear=0xFFFF;
		}
		else
		{			
			st.wDay=BYTE(pStartData->m_dwMinDate);
			st.wMonth=BYTE(pStartData->m_dwMinDate>>8);
			st.wYear=WORD(pStartData->m_dwMinDate>>16);
		}
		SendDlgItemMessage(IDC_MINDATE,DTM_SETSYSTEMTIME,GDT_VALID,LPARAM(&st));

		switch (pStartData->m_cMinDateType)
		{
		case 'C':
		case 'c':
			SendDlgItemMessage(IDC_MINTYPE,CB_SETCURSEL,1);
			break;
		case 'A':
		case 'a':
			SendDlgItemMessage(IDC_MINTYPE,CB_SETCURSEL,2);
			break;
		default:
			SendDlgItemMessage(IDC_MINTYPE,CB_SETCURSEL,0);
			break;
		}

		rdwChanged|=isDatesChanged;
	}

	if (pStartData->m_dwMaxDate!=DWORD(-1))
	{
		CheckDlgButton(IDC_CHECKMAXDATE,1);
        
		SYSTEMTIME st;
		st.wHour=1;
		st.wMinute=1;
		st.wSecond=1;
		st.wMilliseconds=1;
		
		if (pStartData->m_dwMaxDate&(1<<31))
		{
			st.wDay=WORD(pStartData->m_dwMaxDate);
			st.wMonth=0xFFFF;
			st.wYear=0xFFFF;
		}
		else
		{			
			st.wDay=BYTE(pStartData->m_dwMaxDate);
			st.wMonth=BYTE(pStartData->m_dwMaxDate>>8);
			st.wYear=WORD(pStartData->m_dwMaxDate>>16);
		}
		SendDlgItemMessage(IDC_MAXDATE,DTM_SETSYSTEMTIME,GDT_VALID,LPARAM(&st));

		switch (pStartData->m_cMaxDateType)
		{
		case 'C':
		case 'c':
			SendDlgItemMessage(IDC_MAXTYPE,CB_SETCURSEL,1);
			break;
		case 'a':
		case 'A':
			SendDlgItemMessage(IDC_MAXTYPE,CB_SETCURSEL,2);
			break;
		default:
			SendDlgItemMessage(IDC_MAXTYPE,CB_SETCURSEL,0);
			break;
		}

		rdwChanged|=isDatesChanged;
	}

	EnableItems(TRUE);
}

void CLocateDlg::CSizeDateDlg::LoadControlStates(CRegKey& RegKey,BOOL bPreset)
{
	DWORD dwTemp=0;
	if (RegKey.QueryValue("SizeDate/MinimumSize",dwTemp))
	{
		CheckDlgButton(IDC_CHECKMINIMUMSIZE,TRUE);
		SetDlgItemInt(IDC_MINIMUMSIZE,dwTemp);
		dwTemp=0;
		RegKey.QueryValue("SizeDate/MinimumSizeType",dwTemp);
		SendDlgItemMessage(IDC_MINSIZETYPE,CB_SETCURSEL,dwTemp,0);
	}
	else
		CheckDlgButton(IDC_CHECKMINIMUMSIZE,FALSE);

	dwTemp=0;
	if (RegKey.QueryValue("SizeDate/MaximumSize",dwTemp))
	{
		CheckDlgButton(IDC_CHECKMAXIMUMSIZE,TRUE);
		SetDlgItemInt(IDC_MAXIMUMSIZE,dwTemp);
		dwTemp=0;
		RegKey.QueryValue("SizeDate/MaximumSizeType",dwTemp);
		SendDlgItemMessage(IDC_MAXSIZETYPE,CB_SETCURSEL,dwTemp,0);
	}
	else
		CheckDlgButton(IDC_CHECKMAXIMUMSIZE,FALSE);
		



	char szData[sizeof(SYSTEMTIME)+4];
    DWORD dwType;
	DWORD dwLen=RegKey.QueryValue("SizeDate/MinimumDate",szData,sizeof(SYSTEMTIME)+4,&dwType);
	if (dwLen>=sizeof(SYSTEMTIME)+4 && dwType==REG_BINARY)
	{
		CheckDlgButton(IDC_CHECKMINDATE,TRUE);
		SendDlgItemMessage(IDC_MINDATE,DTM_SETSYSTEMTIME,0,(LPARAM)(szData+4));
		SendDlgItemMessage(IDC_MINTYPE,CB_SETCURSEL,LOWORD(*((LONG*)szData)),0);
	}
	else 
	{
		if (!bPreset)
		{
			if (dwType==REG_DWORD && dwLen>=sizeof(DWORD))
			{
				SendDlgItemMessage(IDC_MINTYPE,CB_SETCURSEL,LOWORD(*((DWORD*)szData)),0);
				SendDlgItemMessage(IDC_MINDATE,DTXM_CHANGEMODE,HIWORD(*((DWORD*)szData)),0);
			}
			else
				SendDlgItemMessage(IDC_MINDATE,DTXM_CHANGEMODE,0,0);
		}
		CheckDlgButton(IDC_CHECKMINDATE,FALSE);
	}
		
	dwLen=RegKey.QueryValue("SizeDate/MaximumDate",szData,sizeof(SYSTEMTIME)+4,&dwType);
	if (dwLen>=sizeof(SYSTEMTIME)+4 && dwType==REG_BINARY)
	{
		CheckDlgButton(IDC_CHECKMAXDATE,TRUE);
		SendDlgItemMessage(IDC_MAXDATE,DTM_SETSYSTEMTIME,0,(LPARAM)(szData+4));
		SendDlgItemMessage(IDC_MAXTYPE,CB_SETCURSEL,LOWORD(*((LONG*)szData)),0);
	}
	else 
	{
		if (!bPreset)
		{
			if (dwType==REG_DWORD && dwLen>=sizeof(DWORD))
			{
				SendDlgItemMessage(IDC_MAXTYPE,CB_SETCURSEL,LOWORD(*((DWORD*)szData)),0);
				SendDlgItemMessage(IDC_MAXDATE,DTXM_CHANGEMODE,HIWORD(*((DWORD*)szData)),0);
			}
			else
				SendDlgItemMessage(IDC_MAXDATE,DTXM_CHANGEMODE,0,0);
		}
		CheckDlgButton(IDC_CHECKMAXDATE,FALSE);
	}
		
	HilightTab(IsChanged());

}

void CLocateDlg::CSizeDateDlg::SaveControlStates(CRegKey& RegKey)
{
	if (IsDlgButtonChecked(IDC_CHECKMINIMUMSIZE))
	{
		BOOL bError;
		DWORD dwMinSize=(DWORD)SendDlgItemMessage(IDC_MINIMUMSIZESPIN,UDM_GETPOS32,0,(LPARAM)&bError);
		if (bError)
		{
			dwMinSize=GetDlgItemInt(IDC_MINIMUMSIZE,&bError,FALSE);
			if (!bError)
				dwMinSize=0;
		}
		RegKey.SetValue("SizeDate/MinimumSize",dwMinSize);
	}
	else
		RegKey.DeleteValue("SizeDate/MinimumSize");
	RegKey.SetValue("SizeDate/MinimumSizeType",DWORD(SendDlgItemMessage(IDC_MINSIZETYPE,CB_GETCURSEL,0,0)));

	if (IsDlgButtonChecked(IDC_CHECKMAXIMUMSIZE))
	{
		BOOL bError;
		DWORD dwMaxSize=(DWORD)SendDlgItemMessage(IDC_MAXIMUMSIZESPIN,UDM_GETPOS32,0,(LPARAM)&bError);
		if (bError)
		{
			dwMaxSize=GetDlgItemInt(IDC_MAXIMUMSIZE,&bError,FALSE);
			if (!bError)
				dwMaxSize=0;
		}
		RegKey.SetValue("SizeDate/MaximumSize",dwMaxSize);
	}
	else
		RegKey.DeleteValue("SizeDate/MaximumSize");
	RegKey.SetValue("SizeDate/MaximumSizeType",DWORD(SendDlgItemMessage(IDC_MAXSIZETYPE,CB_GETCURSEL,0,0)));
	
	if (IsDlgButtonChecked(IDC_CHECKMINDATE))
	{
		char szTemp[sizeof(SYSTEMTIME)+4];
		SendDlgItemMessage(IDC_MINDATE,DTM_GETSYSTEMTIME,DTXF_FORSAVE,(LPARAM)(szTemp+4));
		*((LONG*)szTemp)=MAKELONG(SendDlgItemMessage(IDC_MINTYPE,CB_GETCURSEL),
			SendDlgItemMessage(IDC_MINDATE,DTXM_GETMODE));
		RegKey.SetValue("SizeDate/MinimumDate",szTemp,sizeof(SYSTEMTIME)+4,REG_BINARY);
	}
	else
	{
		RegKey.SetValue("SizeDate/MinimumDate",
			(DWORD)MAKELONG(SendDlgItemMessage(IDC_MINTYPE,CB_GETCURSEL),
			SendDlgItemMessage(IDC_MINDATE,DTXM_GETMODE)));
	}

	if (IsDlgButtonChecked(IDC_CHECKMAXDATE))
	{
		char szTemp[sizeof(SYSTEMTIME)+4];
		SendDlgItemMessage(IDC_MAXDATE,DTM_GETSYSTEMTIME,DTXF_FORSAVE,(LPARAM)(szTemp+4));
		*((LONG*)szTemp)=MAKELONG(SendDlgItemMessage(IDC_MAXTYPE,CB_GETCURSEL),
			SendDlgItemMessage(IDC_MAXDATE,DTXM_GETMODE));
		RegKey.SetValue("SizeDate/MaximumDate",szTemp,sizeof(SYSTEMTIME)+4,REG_BINARY);
	}
	else
	{
		RegKey.SetValue("SizeDate/MaximumDate",
			(DWORD)MAKELONG(SendDlgItemMessage(IDC_MAXTYPE,CB_GETCURSEL),
			SendDlgItemMessage(IDC_MAXDATE,DTXM_GETMODE)));
	}
}



//////////////////////////////////////////////////////////////////////////////
// CLocateDlg::CAdvancedDlg
//////////////////////////////////////////////////////////////////////////////

BOOL CLocateDlg::CAdvancedDlg::OnInitDialog(HWND hwndFocus)
{
	CDialog::OnInitDialog(hwndFocus);
	GetLocateDlg()->AddInstantSearchingFlags(isIgnoreChangeMessages);

	// Initializing Imagelists
	m_ToolbarIL.Create(IDB_TOOLBARBITMAPS,12,1,RGB(255,255,255),LR_CREATEDIBSECTION,FALSE);
	m_ToolbarILHover.Create(IDB_TOOLBARBITMAPSH,12,1,RGB(255,255,255),LR_CREATEDIBSECTION,FALSE);
	m_ToolbarILDisabled.Create(IDB_TOOLBARBITMAPSD,12,1,RGB(255,255,255),LR_CREATEDIBSECTION,FALSE);
	
	// Sets imagelists for toolbar
	SendDlgItemMessage(IDC_HELPTOOLBAR,TB_SETIMAGELIST,0,LPARAM(HIMAGELIST(m_ToolbarIL)));
	SendDlgItemMessage(IDC_HELPTOOLBAR,TB_SETHOTIMAGELIST,0,LPARAM(HIMAGELIST(m_ToolbarILHover)));
	SendDlgItemMessage(IDC_HELPTOOLBAR,TB_SETDISABLEDIMAGELIST,0,LPARAM(HIMAGELIST(m_ToolbarILDisabled)));

	// Inserting Help button to toolbar
	TBBUTTON tb;
	memset(&tb,0,sizeof(TBBUTTON));
	tb.idCommand=IDC_TEXTHELP;
	tb.fsStyle=TBSTYLE_BUTTON;
	tb.fsState=TBSTATE_ENABLED;
	SendDlgItemMessage(IDC_HELPTOOLBAR,TB_INSERTBUTTON,0,LPARAM(&tb));

	CComboBox Check(GetDlgItem(IDC_CHECK));
	Check.AddString(ID2W(IDS_FILENAMESONLY));
	Check.AddString(ID2W(IDS_FILEANDFOLDERNAMES));
	Check.AddString(ID2W(IDS_FOLDERNAMESONLY));
	

	// Loading m_aReplaceChars array and change label
	LoadRegistry();
	RenameReplaceSpaces();


	// Adding (by extension)
	SendDlgItemMessage(IDC_FILETYPE,CB_ADDSTRING,0,LPARAM(szwEmpty));
	//AddBuildInFileTypes();
	OnClear(TRUE);

	// Subclassing type combo
	TypeControlData* pTypeData=new TypeControlData;
	pTypeData->pParent=this;
	pTypeData->pOldWndProc=(WNDPROC)::SetWindowLongPtr(GetDlgItem(IDC_FILETYPE),
		GWLP_WNDPROC,(LONG_PTR)TypeWindowProc);

	if (pTypeData->pOldWndProc==NULL)
	{
		// Subclassing didn't success
		delete pTypeData;
	}
	else
		::SetWindowLongPtr(GetDlgItem(IDC_FILETYPE),GWLP_USERDATA,(LONG_PTR)pTypeData);
	


	// Resolve control sizes
	RECT rc1,rc2;
	GetWindowRect(&rc1);
	::GetWindowRect(GetDlgItem(IDC_CHECK),&rc2);
	m_nCheckPos=WORD(rc2.left-rc1.left);
	::GetWindowRect(GetDlgItem(IDC_DATAMATCHCASE),&rc2);
	m_nMatchCaseTop=WORD(rc2.top-rc1.top);
	::GetClientRect(GetDlgItem(IDC_DATAMATCHCASE),&rc2);
	m_nMatchCaseWidth=WORD(rc2.right);
	

	GetLocateDlg()->RemoveInstantSearchingFlags(isIgnoreChangeMessages);
	return FALSE;
}

void CLocateDlg::CAdvancedDlg::AddBuildInFileTypes()
{

	DebugMessage("CAdvancedDlg::AddBuildInFileTypes() BEGIN");

	if (m_dwFlags&fgBuildInTypesAdded)
		return;

	CStringW strTypes;
	CRegKey2 RegKey;
	DWORD bRet=FALSE;

	// Loading buildin types from registry if needed	
	if (RegKey.OpenKey(HKCU,"\\Locate",
		CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
	{
		RegKey.QueryValue(L"Load Buildin Types",bRet);
		if (bRet)
			RegKey.QueryValue(L"Buildin Types",strTypes);
	}

	// Saving buildin types to registry
	if (strTypes.IsEmpty())
	{
		strTypes.LoadString(IDS_BUILDINTYPES);

        if (RegKey.OpenKey(HKCU,"\\Locate",
			CRegKey::createNew|CRegKey::samAll)==ERROR_SUCCESS)
		{
			if (!bRet)		
				RegKey.SetValue(L"Load Buildin Types",DWORD(0));
			RegKey.SetValue(L"Buildin Types",strTypes);
		}			
	}
	RegKey.CloseKey();

	// This is not very best way to do this
	strTypes.ReplaceChars(L'|',L'\0');
	LPCWSTR pPtr=strTypes;


	// Load image list and replace some ions
	CImageList il;
	il.Create(IDB_DEFAULTTYPEICONS,16,0,RGB(255,0,255),LR_CREATEDIBSECTION,FALSE);

	CBitmap Bitmap,Mask;
	BYTE* pBitmap,*pMask;
	BITMAPINFO  dibInfo;
	CDC dc;
	CBrush white(RGB(255,255,255));
	CRect rc(0,0,16,16);
		
	dibInfo.bmiHeader.biBitCount = 32;
	dibInfo.bmiHeader.biClrImportant = 0;
	dibInfo.bmiHeader.biClrUsed = 0;
	dibInfo.bmiHeader.biCompression = 0;
	dibInfo.bmiHeader.biHeight = 16;
	dibInfo.bmiHeader.biPlanes = 1;
	dibInfo.bmiHeader.biSize = 40;
	dibInfo.bmiHeader.biWidth = ( ( 16 + 3 ) / 4 ) * 4;
	dibInfo.bmiHeader.biSizeImage = 16*16*4;
	dibInfo.bmiHeader.biXPelsPerMeter = 3780;
	dibInfo.bmiHeader.biYPelsPerMeter = 3780;
	dibInfo.bmiColors[0].rgbBlue = 0;
	dibInfo.bmiColors[0].rgbGreen = 0;
	dibInfo.bmiColors[0].rgbRed = 0;
	dibInfo.bmiColors[0].rgbReserved = 0;

	dc.GetDC(NULL);
	Bitmap.CreateDIBSection(dc,(const BITMAPINFO*)&dibInfo,DIB_RGB_COLORS,(void**)&pBitmap,NULL,0);
	Mask.CreateDIBSection(dc,(const BITMAPINFO*)&dibInfo,DIB_RGB_COLORS,(void**)&pMask,NULL,0);
	dc.ReleaseDC();
	dc.CreateCompatibleDC(NULL);	

	HMODULE hModule=LoadLibrary("shell32.dll");
	if (hModule!=NULL)
	{
		struct PAIR { int a;int b; };
		PAIR pairs[]={ { 224,4},{246,3}};

		for (int p=0;p<sizeof(pairs)/sizeof(pairs[0]);p++)
		{
			HICON hIcon=(HICON)::LoadImage(hModule,MAKEINTRESOURCE(pairs[p].a),IMAGE_ICON,16,16,LR_DEFAULTCOLOR|LR_LOADTRANSPARENT);
			if (hIcon!=NULL) 
			{
				HBITMAP hOldBitmap = (HBITMAP)dc.SelectObject(Bitmap);
				dc.FillRect(&rc,white);
				dc.DrawIcon(0,0,hIcon,16,16,0,NULL,DI_NORMAL);
				dc.SelectObject(hOldBitmap);
				for (int i=0;i<16*16;i++)
					((DWORD*)pMask)[i]=((DWORD*)pBitmap)[i]&0x00FFFFFF?0xFFFFFFFF:0;
				il.Replace(pairs[p].b,Bitmap,Mask);
			}
		}
	}

	hModule=LoadLibrary("imageres.dll");
	if (hModule!=NULL)
	{
		struct PAIR { int a;int b; };
		PAIR pairs[]={ { 15,5} , {90,2}, {72,0},{102,1} };

		for (int p=0;p<sizeof(pairs)/sizeof(pairs[0]);p++)
		{
			HICON hIcon=(HICON)::LoadImage(hModule,MAKEINTRESOURCE(pairs[p].a),IMAGE_ICON,16,16,LR_DEFAULTCOLOR|LR_LOADTRANSPARENT);
			if (hIcon!=NULL) 
			{
				HBITMAP hOldBitmap = (HBITMAP)dc.SelectObject(Bitmap);
				dc.FillRect(&rc,white);
				dc.DrawIcon(0,0,hIcon,16,16,0,NULL,DI_NORMAL);
				dc.SelectObject(hOldBitmap);
				for (int i=0;i<16*16;i++)
					((DWORD*)pMask)[i]=((DWORD*)pBitmap)[i]&0x00FFFFFF?0xFFFFFFFF:0;
				il.Replace(pairs[p].b,Bitmap,Mask);
			}
		}
	}


	while (*pPtr!=L'\0')
		SendDlgItemMessage(IDC_FILETYPE,CB_ADDSTRING,0,LPARAM(new FileType(pPtr,il)));
	
	if (GetLocateDlg()->m_dwFlags&CLocateDlg::fgLoadRegistryTypes) 
	{
		// Add some NULL items to add some space
		SendDlgItemMessage(IDC_FILETYPE,CB_ADDSTRING,0,LPARAM(NULL));
		SendDlgItemMessage(IDC_FILETYPE,CB_ADDSTRING,0,LPARAM(NULL));
		SendDlgItemMessage(IDC_FILETYPE,CB_ADDSTRING,0,LPARAM(NULL));
		SendDlgItemMessage(IDC_FILETYPE,CB_ADDSTRING,0,LPARAM(NULL));
		SendDlgItemMessage(IDC_FILETYPE,CB_ADDSTRING,0,LPARAM(NULL));
		SendDlgItemMessage(IDC_FILETYPE,CB_ADDSTRING,0,LPARAM(NULL));
		SendDlgItemMessage(IDC_FILETYPE,CB_ADDSTRING,0,LPARAM(NULL));
	}
	
	il.DeleteImageList();

	m_dwFlags|=fgBuildInTypesAdded;

	DebugMessage("CAdvancedDlg::AddBuildInFileTypes() END");
}

LRESULT CLocateDlg::CAdvancedDlg::WindowProc(UINT msg,WPARAM wParam,LPARAM lParam)
{
	if (msg==WM_COMPAREITEM && wParam==IDC_FILETYPE)
	{
		FileType* ft1=((FileType*)((COMPAREITEMSTRUCT*)lParam)->itemData1);
		FileType* ft2=((FileType*)((COMPAREITEMSTRUCT*)lParam)->itemData2);

		// NULLs to end
		if (ft1==NULL)
			return 1; 
		if (ft2==NULL)
			return -1;

		// (No extension) first
		if (ft1==(FileType*)szwEmpty)
			return -1; 
		if (ft2==(FileType*)szwEmpty)
			return 1;


		if (ft1->szType==NULL)
		{
			if (ft2->szType!=NULL)
				return -1;
		}
		else if (ft2->szType==NULL) // ft2 is buildin
			return 1;

		return _wcsicmp(
			((FileType*)((COMPAREITEMSTRUCT*)lParam)->itemData1)->szTitle,
			((FileType*)((COMPAREITEMSTRUCT*)lParam)->itemData2)->szTitle);
	}
	return CDialog::WindowProc(msg,wParam,lParam);
}

BOOL CLocateDlg::CAdvancedDlg::OnCommand(WORD wID,WORD wNotifyCode,HWND hControl)
{
	CLocateDlg* pLocateDlg=GetLocateDlg();
	if (pLocateDlg==NULL)
		return FALSE;

	switch (wID)
	{
	case IDC_TEXTHELP:
		// Menu item
		HtmlHelp(HH_HELP_CONTEXT,HELP_ISSUE_DATASEARCH);
		break;
	case IDC_CONTAINDATACHECK:
		if (hControl==NULL && wNotifyCode==1) // Accelerator
			CheckDlgButton(IDC_CONTAINDATACHECK,!IsDlgButtonChecked(IDC_CONTAINDATACHECK));

		if (IsDlgButtonChecked(IDC_CONTAINDATACHECK))
		{
			EnableDlgItem(IDC_DATAMATCHCASE,TRUE);
			EnableDlgItem(IDC_CONTAINDATA,TRUE);
			EnableDlgItem(IDC_HELPTOOLBAR,TRUE);
			SetFocus(IDC_CONTAINDATA);
			HilightTab(TRUE);		
		}
		else
		{
			EnableDlgItem(IDC_DATAMATCHCASE,FALSE);
			EnableDlgItem(IDC_CONTAINDATA,FALSE);
			EnableDlgItem(IDC_HELPTOOLBAR,FALSE);

			if (hControl==NULL && wNotifyCode==1) // Accelerator
				SetFocus(IDC_CONTAINDATACHECK);

			HilightTab(IsChanged());
		}
		ChangeEnableStateForCheck();

		pLocateDlg->OnFieldChange(isDataChanged);
		break;
	case IDC_USEWHOLEPATH:
		if (hControl==NULL && wNotifyCode==1) // Accelerator
		{
			CheckDlgButton(wID,!IsDlgButtonChecked(wID));
			SetFocus(wID);
		}
		
		HilightTab(IsChanged());

		pLocateDlg->OnFieldChange(isOtherChanged);
		break;
	case IDC_FILETYPE:
		DebugFormatMessage("IDC_FILETYPE, wNotifuCode=%d, hControl=%X this=%X",wNotifyCode,hControl,this);
		switch (wNotifyCode)
		{
		case 1: // Accel or CBN_SELCHANGE
			if (hControl==NULL)
			{
				UpdateTypeList();
				SetFocus(IDC_FILETYPE);
			}
			else
			{
				CComboBox FileTypeCombo(GetDlgItem(IDC_FILETYPE));
				FileType* ft=NULL;
				int nSel=FileTypeCombo.GetCurSel();
				if (nSel!=CB_ERR)
				{
					ft=(FileType*)FileTypeCombo.GetItemData(nSel);
					if (ft==NULL || ft==(FileType*)szwEmpty)
					{
						ft=NULL;
						if (!pLocateDlg->m_NameDlg.m_Type.IsWindowEnabled())
							pLocateDlg->m_NameDlg.m_Type.SetText("");
					}
					else
					{
						WCHAR* pEx=new WCHAR[max(ft->dwExtensionLength,2)];
						MemCopyW(pEx,ft->szExtensions,ft->dwExtensionLength);
						for (int i=0;pEx[i]!=L'\0' || pEx[i+1]!=L'\0';i++)
						{
							if (pEx[i]==L'\0')
								pEx[i]=L' ';
						}
						pLocateDlg->m_NameDlg.m_Type.SetText(pEx);
						delete[] pEx;
					}
					
				}
				pLocateDlg->m_NameDlg.m_Type.EnableWindow(ft==NULL);
				ChangeEnableStateForCheck();
			
			}
			HilightTab(IsChanged());

			pLocateDlg->OnFieldChange(isOtherChanged);
			break;
		case CBN_DROPDOWN:
			UpdateTypeList();
			break;
		case CBN_CLOSEUP:
			if (m_hTypeUpdaterThread==NULL)
			{
				int nSelected=(int)SendDlgItemMessage(IDC_FILETYPE,CB_GETCURSEL);

				// Destroying unnecessary icons
				for (int i=(int)SendDlgItemMessage(IDC_FILETYPE,CB_GETCOUNT)-1;i>0;i--)
				{
					if (i!=nSelected)
					{
						FileType* pFileType=(FileType*)SendDlgItemMessage(IDC_FILETYPE,CB_GETITEMDATA,i);
						if (pFileType!=NULL)
						{
							if (pFileType->szIconPath!=NULL && 
								pFileType->hIcon!=NULL && 
								pFileType->hIcon!=m_hDefaultTypeIcon)
							{
								DestroyIcon(pFileType->hIcon);
								pFileType->hIcon=NULL;
							}
						}
					}
				}

				
			}
			break;
		}
		break;
	case IDC_CONTAINDATA:
		switch (wNotifyCode)
		{
		case 1:
			SetFocus(wID);
			break;
		case EN_SETFOCUS:
			::SendMessage(hControl,EM_SETSEL,0,MAKELPARAM(0,-1));
			break;
		case EN_CHANGE:
			pLocateDlg->OnFieldChange(isDataChanged);
			break;
		}		
		break;
	case IDC_CHECK:
		switch (wNotifyCode)
		{
		case CBN_SELCHANGE:
			if (hControl==NULL && wNotifyCode==1 && IsDlgItemEnabled(IDC_CHECK)) // Accelerator
				SetFocus(IDC_CHECK);
			else
			{
				HilightTab(IsChanged());
				pLocateDlg->OnFieldChange(isOtherChanged);
			}
			break;
		}
		break;
	case IDC_REPLACESPACES:
		if (GetKeyState(VK_CONTROL) & 0x8000)
		{
			CheckDlgButton(IDC_REPLACESPACES,TRUE);
			CReplaceCharsDlg d(m_aReplaceChars,m_dwFlags&fgReplaceUseQuestionMark?TRUE:FALSE);
			if (d.DoModal(*this))
			{
				if (d.UseQuestionMark())
					m_dwFlags|=fgReplaceUseQuestionMark;
				else
					m_dwFlags&=~fgReplaceUseQuestionMark;
				RenameReplaceSpaces();
			}
		}
		else
		{
			CheckDlgButton(IDC_REPLACESPACES,!IsDlgButtonChecked(IDC_REPLACESPACES));
			if (hControl==NULL && wNotifyCode==1) // Accelerator
				SetFocus(IDC_REPLACESPACES);
		}
		HilightTab(IsChanged());
		pLocateDlg->OnFieldChange(isOtherChanged);
		break;
	case IDC_MATCHWHOLENAME:
	case IDC_MATCHCASE:
	case IDC_DATAMATCHCASE:
		if (hControl==NULL && wNotifyCode==1) // Accelerator
		{
			CheckDlgButton(wID,!IsDlgButtonChecked(wID));
			SetFocus(wID);
		}
		HilightTab(IsChanged());
		pLocateDlg->OnFieldChange(isOtherChanged);
		break;
	}
	
	return CDialog::OnCommand(wID,wNotifyCode,hControl);
}


LRESULT CALLBACK CLocateDlg::CAdvancedDlg::TypeWindowProc(HWND hWnd,UINT uMsg,
														  WPARAM wParam,LPARAM lParam)
{
	TypeControlData* pTypeData=(TypeControlData*)::GetWindowLongPtr(hWnd,GWLP_USERDATA);
	LRESULT lRet=FALSE;

	if (pTypeData==NULL)
		return FALSE;

	
	switch (uMsg)
	{
	case WM_DESTROY:
		// Calling original window procedure
		lRet=CallWindowProc(pTypeData->pOldWndProc,hWnd,uMsg,wParam,lParam);

		// Desubclassing
		::SetWindowLongPtr(hWnd,GWLP_WNDPROC,(LONG_PTR)pTypeData->pOldWndProc);
		
		// Free memory
		delete pTypeData;
		return lRet;
	case WM_KEYDOWN:
		if (wParam==VK_DOWN || wParam==VK_UP)
			pTypeData->pParent->UpdateTypeList();
		
		lRet=CallWindowProc(pTypeData->pOldWndProc,hWnd,uMsg,wParam,lParam);
		break;
	case WM_CHAR:
	case WM_UNICHAR:
		{
			WCHAR cPressed;
			if (uMsg==WM_CHAR)
				cPressed=A2Wc((char)wParam);
			else
				cPressed=(WCHAR)wParam;

			
			// Key pressed, selecting item based on this key
			pTypeData->pParent->UpdateTypeList();
			CharLowerBuffW(&cPressed,1);


			// Checking selected item
			int nIndex=(int)CallWindowProc(pTypeData->pOldWndProc,hWnd,CB_GETCURSEL,0,0);
			int nCount=(int)CallWindowProc(pTypeData->pOldWndProc,hWnd,CB_GETCOUNT,0,0);
			
			if (nIndex!=CB_ERR)
			{
				WCHAR cFirstChar;
				if (nIndex==0)
				{
					WCHAR szTemp[2];
					LoadString(IDS_BYEXTENSION,szTemp,2);
					cFirstChar=szTemp[0];
				}
				else
				{
					FileType* pType=(FileType*)CallWindowProc(pTypeData->pOldWndProc,hWnd,CB_GETITEMDATA,nIndex,0);
					if (pType!=NULL)
						cFirstChar=pType->szTitle[0];
				}

				CharLowerBuffW(&cFirstChar,1);

				if (cFirstChar==wParam)
					nIndex++;
				else
					nIndex=0;
			}
			else
				nIndex=0;

			

			// Finding item which starts with this key
			for (;nIndex<nCount;nIndex++)
			{
				WCHAR cFirstChar;
				if (nIndex==0)
				{
					WCHAR szTemp[2];
					LoadString(IDS_BYEXTENSION,szTemp,2);
					cFirstChar=szTemp[0];
				}else
				{
					FileType* pType=(FileType*)CallWindowProc(pTypeData->pOldWndProc,hWnd,CB_GETITEMDATA,nIndex,0);
					if (pType!=NULL)
						cFirstChar=pType->szTitle[0];
				}

				CharLowerBuffW(&cFirstChar,1);

				if (cFirstChar==wParam)
				{
					CallWindowProc(pTypeData->pOldWndProc,hWnd,CB_SETCURSEL,nIndex,0);
					pTypeData->pParent->OnCommand(IDC_FILETYPE,1,hWnd);
					return 0;
				}
			}



			lRet=CallWindowProc(pTypeData->pOldWndProc,hWnd,uMsg,wParam,lParam);
			break;
		}
	
	default:
		lRet=CallWindowProc(pTypeData->pOldWndProc,hWnd,uMsg,wParam,lParam);
		break;
	}

	return lRet;
}

BOOL CLocateDlg::CAdvancedDlg::IsChanged()
{
	DWORD dwDefaultCheck=1,dwDefaultMatchCase=0,dwDefaultMatchWholeName=0,dwDefaultReplaceSpaces=0,dwDefaultUseWholePath=0;

	CRegKey2 RegKey;
	if (RegKey.OpenKey(HKCU,"\\General",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
	{
		RegKey.QueryValue("Default CheckIn",dwDefaultCheck);
		RegKey.QueryValue("Default MatchCase",dwDefaultMatchCase);
		RegKey.QueryValue("Default MatchWholeName",dwDefaultMatchWholeName);
		RegKey.QueryValue("Default ReplaceSpaces",dwDefaultReplaceSpaces);
		RegKey.QueryValue("Default UseWholePath",dwDefaultUseWholePath);
		RegKey.CloseKey();
	}

	if (IsDlgItemEnabled(IDC_CHECK))
	{
		if (SendDlgItemMessage(IDC_CHECK,CB_GETCURSEL)!=dwDefaultCheck)
			return TRUE;
	}

	if (IsDlgButtonChecked(IDC_MATCHCASE)!=dwDefaultMatchCase)
		return TRUE;

	if (IsDlgButtonChecked(IDC_MATCHWHOLENAME)!=dwDefaultMatchWholeName)
		return TRUE;

	if (IsDlgButtonChecked(IDC_REPLACESPACES)!=dwDefaultReplaceSpaces)
		return TRUE;

	if (IsDlgButtonChecked(IDC_USEWHOLEPATH)!=dwDefaultUseWholePath)
		return TRUE;

	
	if (IsDlgItemEnabled(IDC_FILETYPE))
	{
		if (SendDlgItemMessage(IDC_FILETYPE,CB_GETCURSEL)>0)
			return TRUE;
	}
	
	
	return FALSE;
}



void CLocateDlg::CAdvancedDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType,cx,cy);
	
	SetDlgItemPos(IDC_CHECK,HWND_BOTTOM,0,0,cx-m_nCheckPos-19,21,SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
	SetDlgItemPos(IDC_FILETYPE,HWND_BOTTOM,0,0,cx-34,21,SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
	SetDlgItemPos(IDC_CONTAINDATA,HWND_BOTTOM,0,0,cx-34,21,SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
	SetDlgItemPos(IDC_DATAMATCHCASE,HWND_BOTTOM,max(cx-m_nMatchCaseWidth-45,125),m_nMatchCaseTop,0,0,SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER);
	SetDlgItemPos(IDC_HELPTOOLBAR,HWND_BOTTOM,max(cx-43,125),m_nMatchCaseTop-3,0,0,SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER);
}

DWORD CLocateDlg::CAdvancedDlg::SetAdvancedFlagsForLocater(CLocater* pLocater,BOOL bForInstantSearch)
{
	DWORD dwFlags=0,dwMaxFiles=(DWORD)-1;
	if (bForInstantSearch)
	{
		if (GetLocateDlg()->m_dwInstantLimit>0)
			dwMaxFiles=GetLocateDlg()->m_dwInstantLimit;
	}
	else if (GetLocateDlg()->m_dwMaxFoundFiles>0)
		dwMaxFiles=GetLocateDlg()->m_dwMaxFoundFiles;


	if (IsDlgItemEnabled(IDC_CHECK))
	{
		switch (SendDlgItemMessage(IDC_CHECK,CB_GETCURSEL))
		{
		case 0:
			dwFlags=LOCATE_FILENAMES;
			break;
		case 1:
			dwFlags=LOCATE_FILENAMES|LOCATE_FOLDERNAMES;
			break;
		case 2:
			dwFlags=LOCATE_FOLDERNAMES;
			break;
		}
	}
	else
		dwFlags=LOCATE_FILENAMES; // Assumption

	if (IsDlgButtonChecked(IDC_CONTAINDATACHECK))
	{
		if (IsDlgButtonChecked(IDC_DATAMATCHCASE))
			dwFlags|=LOCATE_CONTAINTEXTISMATCHCASE;
		CStringW str;
		GetDlgItemText(IDC_CONTAINDATA,str);
		
		
		DWORD dwDataLength;
		BYTE* pData=NULL;
		if (_wcsnicmp(str,L"regexp:",7)==0)
		{
			dwDataLength=str.GetLength()-7+1;
            if (dwDataLength>1)
			{
				pData=(BYTE*)malloc(dwDataLength+1);
				WideCharToMultiByte(CP_ACP,0,LPCWSTR(str)+7,dwDataLength,(LPSTR)pData,dwDataLength+1,NULL,NULL);
				dwFlags|=LOCATE_REGULAREXPRESSIONSEARCH;
			}
		}
		else
			pData=dataparser(str,&dwDataLength);
			

		
		pLocater->SetAdvanced(dwFlags,pData,dwDataLength,dwMaxFiles);
		if (pData!=NULL)
			free(pData);
	}
	else
		pLocater->SetAdvanced(dwFlags,NULL,0,dwMaxFiles);

	if (IsDlgButtonChecked(IDC_USEWHOLEPATH))
		pLocater->AddAdvancedFlags(LOCATE_CHECKWHOLEPATH);

	return (IsDlgButtonChecked(IDC_MATCHWHOLENAME)?flagMatchWholeNameOnly:0)|
		(IsDlgButtonChecked(IDC_MATCHCASE)?flagMatchCase:0)|
		(IsDlgButtonChecked(IDC_REPLACESPACES)?flagReplaceSpaces:0);

}

	
void CLocateDlg::CAdvancedDlg::OnClear(BOOL bInitial)
{
	CRegKey2 RegKey;
	if (RegKey.OpenKey(HKCU,"\\General",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
	{
		DWORD nTemp=1;
		RegKey.QueryValue("Default CheckIn",nTemp);
		SendDlgItemMessage(IDC_CHECK,CB_SETCURSEL,nTemp);
        nTemp=0;
		RegKey.QueryValue("Default MatchWholeName",nTemp);
		CheckDlgButton(IDC_MATCHWHOLENAME,nTemp);
		nTemp=0;
		RegKey.QueryValue("Default MatchCase",nTemp);
		CheckDlgButton(IDC_MATCHCASE,nTemp);
		nTemp=1;
		RegKey.QueryValue("Default DataMatchCase",nTemp);
		CheckDlgButton(IDC_DATAMATCHCASE,nTemp);
		nTemp=0;
		RegKey.QueryValue("Default ReplaceSpaces",nTemp);
		CheckDlgButton(IDC_REPLACESPACES,nTemp);
		nTemp=0;
		RegKey.QueryValue("Default UseWholePath",nTemp);
		CheckDlgButton(IDC_USEWHOLEPATH,nTemp);
	}
	else
	{
		SendDlgItemMessage(IDC_CHECK,CB_SETCURSEL,1);
		CheckDlgButton(IDC_MATCHWHOLENAME,0);
		CheckDlgButton(IDC_MATCHCASE,0);
		CheckDlgButton(IDC_DATAMATCHCASE,1);
		CheckDlgButton(IDC_REPLACESPACES,0);
		CheckDlgButton(IDC_USEWHOLEPATH,0);
	}

	SendDlgItemMessage(IDC_FILETYPE,CB_SETCURSEL,0);
	
	CheckDlgButton(IDC_CONTAINDATACHECK,0);
	EnableDlgItem(IDC_DATAMATCHCASE,FALSE);
	EnableDlgItem(IDC_CONTAINDATA,FALSE);
	EnableDlgItem(IDC_HELPTOOLBAR,FALSE);
	SetDlgItemText(IDC_CONTAINDATA,"");	


	ChangeEnableStateForCheck();
	HilightTab(FALSE);
}

void CLocateDlg::CAdvancedDlg::SetStartData(const CLocateApp::CStartData* pStartData,DWORD& rdwChanged)
{
	int sel=-1;
	if ((pStartData->m_nStatus&CLocateApp::CStartData::statusFindFileAndFolderNames)==CLocateApp::CStartData::statusFindFileAndFolderNames)
		sel=1;
	else if (pStartData->m_nStatus&CLocateApp::CStartData::statusFindFileNames)
		sel=0;
	else if (pStartData->m_nStatus&CLocateApp::CStartData::statusFindFolderNames)
		sel=2;
	if (sel!=-1)
	{
		SendDlgItemMessage(IDC_CHECK,CB_SETCURSEL,sel);
		rdwChanged|=isOtherChanged;
	}
    
	if (pStartData->m_pFindText!=NULL)
	{
		EnableDlgItem(IDC_DATAMATCHCASE,TRUE);
		EnableDlgItem(IDC_CONTAINDATA,TRUE);
		CheckDlgButton(IDC_CONTAINDATACHECK,1);
		SetDlgItemText(IDC_CONTAINDATA,pStartData->m_pFindText);
		CheckDlgButton(IDC_DATAMATCHCASE,pStartData->m_nStatus&CLocateApp::CStartData::statusFindIsNotMatchCase?0:1);
		rdwChanged|=isDataChanged;
	}

	if (pStartData->m_nStatus&CLocateApp::CStartData::statusMatchWholeName)
	{
		CheckDlgButton(IDC_MATCHWHOLENAME,1);
		rdwChanged|=isOtherChanged;
	}
	else if (pStartData->m_nStatus&CLocateApp::CStartData::statusNoMatchWholeName)
	{
		CheckDlgButton(IDC_MATCHWHOLENAME,0);
		rdwChanged|=isOtherChanged;
	}

	if (pStartData->m_nStatus&CLocateApp::CStartData::statusMatchCase)
	{
		CheckDlgButton(IDC_MATCHCASE,1);
		rdwChanged|=isOtherChanged;
	}
	else if (pStartData->m_nStatus&CLocateApp::CStartData::statusNoMatchCase)
	{
		CheckDlgButton(IDC_MATCHCASE,0);
		rdwChanged|=isOtherChanged;
	}

	if (pStartData->m_nStatus&CLocateApp::CStartData::statusReplaceSpacesWithAsterisks)
	{
		CheckDlgButton(IDC_REPLACESPACES,1);
		rdwChanged|=isOtherChanged;
	}
	else if (pStartData->m_nStatus&CLocateApp::CStartData::statusNoReplaceSpacesWithAsterisks)
	{
		CheckDlgButton(IDC_REPLACESPACES,0);
		rdwChanged|=isOtherChanged;
	}

	if (pStartData->m_nStatus&CLocateApp::CStartData::statusUseWholePath)
	{
		CheckDlgButton(IDC_USEWHOLEPATH,1);
		rdwChanged|=isOtherChanged;
	}
	else if (pStartData->m_nStatus&CLocateApp::CStartData::statusNoUseWholePath)
	{
		CheckDlgButton(IDC_USEWHOLEPATH,0);
		rdwChanged|=isOtherChanged;
	}

	ChangeEnableStateForCheck();
}
void CLocateDlg::CAdvancedDlg::EnableItems(BOOL bEnable)
{
	if (GetLocateDlg()->IsExtraFlagSet(efMatchWhileNameIfAsterisks) && bEnable)
	{
		CStringW Name;
		GetLocateDlg()->m_NameDlg.m_Name.GetText(Name);
		EnableDlgItem(IDC_MATCHWHOLENAME,Name.Find(L'*')==-1);
	}
	else
		EnableDlgItem(IDC_MATCHWHOLENAME,bEnable);
	EnableDlgItem(IDC_REPLACESPACES,bEnable);
	EnableDlgItem(IDC_USEWHOLEPATH,bEnable);
	EnableDlgItem(IDC_MATCHCASE,bEnable);


	EnableDlgItem(IDC_FILETYPE,bEnable);

	EnableDlgItem(IDC_CONTAINDATACHECK,bEnable);
	EnableDlgItem(IDC_DATAMATCHCASE,bEnable && IsDlgButtonChecked(IDC_CONTAINDATACHECK));
	EnableDlgItem(IDC_CONTAINDATA,bEnable && IsDlgButtonChecked(IDC_CONTAINDATACHECK));
	EnableDlgItem(IDC_HELPTOOLBAR,bEnable && IsDlgButtonChecked(IDC_CONTAINDATACHECK));

	
	if (bEnable)
		ChangeEnableStateForCheck();
	else
		EnableDlgItem(IDC_CHECK,FALSE);
}

void CLocateDlg::CAdvancedDlg::ChangeEnableStateForCheck()
{
	BOOL bEnable=TRUE;
	if (GetLocateDlg()->m_SizeDateDlg.LookOnlyFiles())
		bEnable=FALSE;
	else if (IsDlgButtonChecked(IDC_CONTAINDATACHECK))
		bEnable=FALSE;
	else if (SendDlgItemMessage(IDC_FILETYPE,CB_GETCURSEL)!=0)
		bEnable=FALSE;
	
	int nCurSel=GetLocateDlg()->m_NameDlg.m_Type.GetCurSel();
	if (nCurSel==0)
		bEnable=FALSE; // (none)
	else 
	{
		// If extensions are specified, let 'Check' field be enabled if all extensions are excluded
		CStringW Extensions;
		
		if (nCurSel==CB_ERR)
			GetLocateDlg()->m_NameDlg.m_Type.GetText(Extensions);
		else
			GetLocateDlg()->m_NameDlg.m_Type.GetLBText(nCurSel,Extensions);

		LPCWSTR pExt=Extensions;
		while (*pExt!='\0')
		{
			if (*pExt==' ')
			{
				pExt++;
				continue;
			}

			if (*pExt!='-')
			{
				// Extension without '-' found
				bEnable=FALSE;
				break;
			}
			for (;*pExt!='\0' && *pExt!=' ';pExt++);
		}
	}
	
	if (bEnable)
		EnableDlgItem(IDC_CHECK,TRUE);
	else
	{
		//SendDlgItemMessage(IDC_CHECK,CB_SETCURSEL,0);
		EnableDlgItem(IDC_CHECK,FALSE);
	}
}

void CLocateDlg::CAdvancedDlg::OnDrawItem(UINT idCtl,LPDRAWITEMSTRUCT lpdis)
{
	CDialog::OnDrawItem(idCtl,lpdis);

	if (idCtl==IDC_FILETYPE && lpdis->itemID!=CB_ERR)
	{
		CRect rc(lpdis->rcItem);
		CDC dc(lpdis->hDC);
		HBRUSH hHighLight=GetSysColorBrush(COLOR_HIGHLIGHT);
		CStringW Text;
		
		// Drawing background
		if (lpdis->itemState&ODS_DISABLED)
			dc.FillRect(&lpdis->rcItem,GetSysColorBrush(COLOR_3DFACE));
		else
			dc.FillRect(&lpdis->rcItem,GetSysColorBrush(COLOR_WINDOW));
		
		if (lpdis->itemID==0)
		{
			ASSERT(lpdis->itemData==LPARAM(szwEmpty));
			Text.LoadString(IDS_BYEXTENSION);
		}
		else if (lpdis->itemData!=NULL)
		{
			Text=((FileType*)lpdis->itemData)->szTitle;
			if (((FileType*)lpdis->itemData)->hIcon==NULL)
				((FileType*)lpdis->itemData)->ExtractIconFromPath();

			dc.DrawIcon(rc.left,rc.top,((FileType*)lpdis->itemData)->hIcon,
				16,16,0,lpdis->itemAction&ODA_FOCUS?hHighLight:NULL,DI_NORMAL);
		}
		
		dc.SetBkMode(TRANSPARENT);
		rc.left+=18;
				
		if (lpdis->itemState&ODS_DISABLED)
			dc.SetTextColor(GetSysColor(COLOR_GRAYTEXT));
		else if (lpdis->itemAction&ODA_FOCUS ||
			lpdis->itemState&ODS_SELECTED)
		{
			// Filling text shade
			dc.SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
			CSize sz=dc.GetTextExtent(Text);
			dc.FillRect(&CRect(rc.left,rc.top+1,rc.left+sz.cx+1,rc.bottom-1),hHighLight);

			if (lpdis->itemState&ODS_FOCUS && !(lpdis->itemState&ODS_NOFOCUSRECT))
				dc.DrawFocusRect(&CRect(rc.left,rc.top+1,rc.left+sz.cx+1,rc.bottom-1));
		}
		else 
			dc.SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
			
		dc.DrawText(Text,Text.GetLength(),&rc,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
	}
}

void CLocateDlg::CAdvancedDlg::OnMeasureItem(int nIDCtl,LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	CDialog::OnMeasureItem(nIDCtl,lpMeasureItemStruct);
		
	if (nIDCtl==IDC_FILETYPE)
	{
		CDC dc(&CWnd(GetDlgItem(IDC_FILETYPE)));
		CSize sz;
		if (lpMeasureItemStruct->itemID==0 || lpMeasureItemStruct->itemData==NULL)
			sz=dc.GetTextExtent(CStringW(IDS_BYEXTENSION));
		else
			sz=dc.GetTextExtent(((FileType*)lpMeasureItemStruct->itemData)->szTitle,istrlenw(((FileType*)lpMeasureItemStruct->itemData)->szTitle));
		lpMeasureItemStruct->itemHeight=max(sz.cy+1,16);
		lpMeasureItemStruct->itemWidth=sz.cx+18;
	}
}

void CLocateDlg::CAdvancedDlg::OnDestroy()
{
	CDialog::OnDestroy();
	if (m_hTypeUpdaterThread!=NULL)
	{
		TerminateThread(m_hTypeUpdaterThread,0,TRUE);
		CloseHandle(m_hTypeUpdaterThread);
		DebugCloseThread(m_hTypeUpdaterThread);
		m_hTypeUpdaterThread=NULL;
	}

	for (int i=(int)SendDlgItemMessage(IDC_FILETYPE,CB_GETCOUNT)-1;i>0;i--)
	{
		FileType* pFileType=(FileType*)SendDlgItemMessage(IDC_FILETYPE,CB_GETITEMDATA,i);
		if (pFileType!=NULL)
			delete pFileType;
	}

	if (m_hDefaultTypeIcon!=NULL)
	{
		DestroyIcon(m_hDefaultTypeIcon);
		m_hDefaultTypeIcon=NULL;
	}
	SendDlgItemMessage(IDC_FILETYPE,CB_RESETCONTENT);

	m_ToolbarIL.DeleteImageList();


	SaveRegistry();
	m_aReplaceChars.RemoveAll();

}

void CLocateDlg::CAdvancedDlg::LoadControlStates(CRegKey& RegKey)
{
	CStringW str;

	DWORD dwTemp;
	// Advanced dialog
	if (!RegKey.QueryValue("Advanced/Check",dwTemp))
		dwTemp=1;
    SendDlgItemMessage(IDC_CHECK,CB_SETCURSEL,dwTemp);

	if (!RegKey.QueryValue("Advanced/MatchWholeName",dwTemp))
		dwTemp=0;
	CheckDlgButton(IDC_MATCHWHOLENAME,dwTemp);
	
	if (!RegKey.QueryValue("Advanced/MatchCase",dwTemp))
		dwTemp=0;
	CheckDlgButton(IDC_MATCHCASE,dwTemp);
	
	if (!RegKey.QueryValue("Advanced/ReplaceSpaces",dwTemp))
		dwTemp=0;
    CheckDlgButton(IDC_REPLACESPACES,dwTemp);

	if (!RegKey.QueryValue("Advanced/UseWholePath",dwTemp))
		dwTemp=0;
    CheckDlgButton(IDC_USEWHOLEPATH,dwTemp);

	if (!RegKey.QueryValue("Advanced/TextIsMatchCase",dwTemp))
		dwTemp=1;	
	CheckDlgButton(IDC_DATAMATCHCASE,dwTemp);
	
	if (RegKey.QueryValue(L"Advanced/ContainData",str))
	{
		CheckDlgButton(IDC_CONTAINDATACHECK,TRUE);
		SetDlgItemText(IDC_CONTAINDATA,str);
		EnableDlgItem(IDC_DATAMATCHCASE,TRUE);
		EnableDlgItem(IDC_CONTAINDATA,TRUE);
		EnableDlgItem(IDC_HELPTOOLBAR,TRUE);
	}
	else
	{
		CheckDlgButton(IDC_CONTAINDATACHECK,FALSE);
		SetDlgItemText(IDC_CONTAINDATA,"");
		EnableDlgItem(IDC_DATAMATCHCASE,FALSE);
		EnableDlgItem(IDC_CONTAINDATA,FALSE);
		EnableDlgItem(IDC_HELPTOOLBAR,FALSE);
	}


	SendDlgItemMessage(IDC_FILETYPE,CB_SETCURSEL,0);

	DWORD dwType;
	DWORD dwLength=RegKey.QueryValueLength("Advanced/TypeOfFile");
	BYTE* pData=new BYTE[dwLength];
	RegKey.QueryValue("Advanced/TypeOfFile",(LPSTR)pData,dwLength,&dwType);
	if (dwType==REG_DWORD && dwLength==sizeof(DWORD))
	{
		if (*((int*)pData)>0)
		{
			AddBuildInFileTypes();
			SendDlgItemMessage(IDC_FILETYPE,CB_SETCURSEL,*((DWORD*)pData));
			GetLocateDlg()->m_NameDlg.m_Type.EnableWindow(FALSE);
		}
	}
	else if (dwType==REG_BINARY)
	{
		AddBuildInFileTypes();
		int nItem=AddTypeToList(pData);
		if (nItem>=0)
		{
			SendDlgItemMessage(IDC_FILETYPE,CB_SETCURSEL,nItem);
			GetLocateDlg()->m_NameDlg.m_Type.EnableWindow(FALSE);
		}
	}
	delete[] pData;

	HilightTab(IsChanged());
	
}

void CLocateDlg::CAdvancedDlg::SaveControlStates(CRegKey& RegKey)
{
	// Advanced dialog
	RegKey.SetValue("Advanced/Check",(DWORD)SendDlgItemMessage(IDC_CHECK,CB_GETCURSEL));
	RegKey.SetValue("Advanced/MatchWholeName",IsDlgButtonChecked(IDC_MATCHWHOLENAME));
	RegKey.SetValue("Advanced/MatchCase",IsDlgButtonChecked(IDC_MATCHCASE));
	RegKey.SetValue("Advanced/ReplaceSpaces",IsDlgButtonChecked(IDC_REPLACESPACES));
	RegKey.SetValue("Advanced/UseWholePath",IsDlgButtonChecked(IDC_USEWHOLEPATH));
	if (IsDlgButtonChecked(IDC_CONTAINDATACHECK))
	{
		CStringW str;
		GetDlgItemText(IDC_CONTAINDATA,str);
		RegKey.SetValue(L"Advanced/ContainData",str);
	}
	else
		RegKey.DeleteValue("Advanced/ContainData");
	RegKey.SetValue("Advanced/TextIsMatchCase",IsDlgButtonChecked(IDC_DATAMATCHCASE));

	// Type box
	int nCurSel=(int)SendDlgItemMessage(IDC_FILETYPE,CB_GETCURSEL);
	FileType* pType=(FileType*)SendDlgItemMessage(IDC_FILETYPE,CB_GETITEMDATA,nCurSel);
	if (pType==NULL || pType==(CAdvancedDlg::FileType*)szwEmpty || pType->szType==NULL)
		RegKey.SetValue("Advanced/TypeOfFile",DWORD(nCurSel));
	else
	{
		DWORD dwLength=istrlenw(pType->szType);
		WCHAR* szData=new WCHAR[++dwLength+pType->dwExtensionLength];
		MemCopyW(szData,pType->szType,dwLength);
		MemCopyW(szData+dwLength,pType->szExtensions,pType->dwExtensionLength);
        RegKey.SetValue("Advanced/TypeOfFile",(LPCSTR)szData,(dwLength+pType->dwExtensionLength)*2,REG_BINARY);
		delete[] szData;
	}
}

void CLocateDlg::CAdvancedDlg::UpdateTypeList()
{
	if (m_hTypeUpdaterThread!=NULL) // Already running
		return;

	AddBuildInFileTypes();

	if (!(m_dwFlags&fgOtherTypeAdded) && GetLocateDlg()->m_dwFlags&CLocateDlg::fgLoadRegistryTypes)
	{
		DWORD dwID;
		m_hTypeUpdaterThread=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)UpdaterProc,this,0,&dwID);
		DebugOpenThread(m_hTypeUpdaterThread);

		DebugFormatMessage("LISTUPDATE: thread started ID=%X",dwID);

		if (m_hTypeUpdaterThread==NULL)
			DebugFormatMessage("m_hTypeUpdaterThread==NULL, last error=%d",GetLastError());
	}
}

int CLocateDlg::CAdvancedDlg::AddTypeToList(LPCWSTR szKey,CArray<FileType*>& aFileTypes)
{
	CRegKey RegKey;
	if (RegKey.OpenKey(HKCR,szKey,CRegKey::openExist|CRegKey::samQueryValue|CRegKey::samExecute)!=ERROR_SUCCESS)
		return CB_ERR;

	CStringW sType,sTitle;
	if (!RegKey.QueryValue(L"",sType))
		return CB_ERR;
	sType.MakeLower();

	for (int i=0;i<aFileTypes.GetSize();i++)
	{
		FileType* pType=aFileTypes.GetAt(i);
		if (sType.Compare(pType->szType)==0)
		{
			pType->AddExtension(szKey+1,istrlenw(szKey));
			return -2;
		}
	}

	if (RegKey.OpenKey(HKCR,sType,CRegKey::openExist|CRegKey::samQueryValue|CRegKey::samExecute)!=ERROR_SUCCESS)
		return CB_ERR;
	
	
	if (!RegKey.QueryValue(L"",sTitle))
		return CB_ERR;
	if (sTitle.IsEmpty())
		return CB_ERR;

	FileType* pType=new FileType(alloccopy(sType,sType.GetLength()),alloccopy(sTitle,sTitle.GetLength()));
	pType->AddExtension(szKey+1,istrlenw(szKey));
	aFileTypes.Add(pType);
	pType->SetIcon(RegKey);

	
	return (int)SendDlgItemMessage(IDC_FILETYPE,CB_ADDSTRING,0,LPARAM(pType));
}

int CLocateDlg::CAdvancedDlg::AddTypeToList(BYTE* pTypeAndExtensions)
{
	CRegKey RegKey;
	
	DWORD dwLength=istrlenw((LPCWSTR)pTypeAndExtensions);
	if (dwLength==0)
		return CB_ERR;
	WCHAR* szType=new WCHAR[++dwLength];
	MemCopyW(szType,(LPCWSTR)pTypeAndExtensions,dwLength);
	pTypeAndExtensions+=dwLength*2;
	
	if (RegKey.OpenKey(HKCR,szType,CRegKey::openExist|CRegKey::samQueryValue|CRegKey::samExecute)!=ERROR_SUCCESS)
	{
		delete[] szType;
		return CB_ERR;
	}
	
	CStringW sTitle;
	if (!RegKey.QueryValue(L"",sTitle))
	{
		delete szType;
		return CB_ERR;
	}
	
	FileType* pFileType=new FileType(szType,sTitle.GiveBuffer());
	while (*pTypeAndExtensions!=L'\0')
	{
        dwLength=istrlenw((LPCWSTR)pTypeAndExtensions);
		pFileType->AddExtension((LPCWSTR)pTypeAndExtensions,dwLength++);
		pTypeAndExtensions+=dwLength*2;
	}
	
	pFileType->SetIcon(RegKey);

	return (int)SendDlgItemMessage(IDC_FILETYPE,CB_ADDSTRING,0,LPARAM(pFileType));
}

DWORD WINAPI CLocateDlg::CAdvancedDlg::UpdaterProc(CLocateDlg::CAdvancedDlg* pAdvancedDlg)
{
	DebugMessage("CLocateDlg::CAdvancedDlg::UpdaterProc BEGIN");

	CArray<FileType*> aFileTypes;

	for (int i=(int)pAdvancedDlg->SendDlgItemMessage(IDC_FILETYPE,CB_GETCOUNT)-1;i>=0;i--)
	{
		FileType* pParam=(FileType*)pAdvancedDlg->SendDlgItemMessage(IDC_FILETYPE,CB_GETITEMDATA,i);
		if (pParam==NULL)
			pAdvancedDlg->SendDlgItemMessage(IDC_FILETYPE,CB_DELETESTRING,i);
		else if (pParam!=(FileType*)szwEmpty && pParam->szType!=NULL)
			aFileTypes.Add(pParam);
	}

	if (IsUnicodeSystem())
	{
		WCHAR szKey[1000];
		DWORD dwIndex=0,dwKeyLength=1000;
		while (RegEnumKeyExW(HKCR,dwIndex,szKey,&dwKeyLength,NULL,NULL,NULL,NULL)==ERROR_SUCCESS)
		{
			if (szKey[0]==L'.') // Is Extension
				pAdvancedDlg->AddTypeToList(szKey,aFileTypes);

			dwIndex++;
			dwKeyLength=1000;
		}
	}
	else
	{
		CHAR szKey[1000];
		DWORD dwIndex=0,dwKeyLength=1000;
		DebugMessage("Enumerating the root of HKCR");
		while (RegEnumKeyEx(HKCR,dwIndex,szKey,&dwKeyLength,NULL,NULL,NULL,NULL)==ERROR_SUCCESS)
		{
			DebugFormatMessage("find %s",szKey);
			if (szKey[0]=='.') // Is Extension
				pAdvancedDlg->AddTypeToList(A2W(szKey),aFileTypes);

			dwIndex++;
			dwKeyLength=1000;
		}
		DebugMessage("Enumerating the root of END");
		
	}
	pAdvancedDlg->m_dwFlags|=fgOtherTypeAdded;

	CloseHandle(pAdvancedDlg->m_hTypeUpdaterThread);
	DebugCloseThread(pAdvancedDlg->m_hTypeUpdaterThread);
	pAdvancedDlg->m_hTypeUpdaterThread=NULL;
	
	DebugMessage("CLocateDlg::CAdvancedDlg::UpdaterProc END");
	return 0;
}

void CLocateDlg::CAdvancedDlg::RenameReplaceSpaces()
{
	CStringW Label,Chars;
	if (m_aReplaceChars.GetSize()>0)
	{
		for (int i=0;i<m_aReplaceChars.GetSize();i++)
		{
			if (i+1==m_aReplaceChars.GetSize())
				Chars << L' ' << ID2W(IDS_AND) << L' ';
			else if (i!=0)
				Chars << L", ";
			Chars << L'\'' << m_aReplaceChars[i] << L'\'';
		}
	}
	else
		Chars.LoadString(IDS_REPLACESPACES);

	Label.FormatEx(IDS_REPLACECHARS,(LPCWSTR)Chars,
		(LPCWSTR)ID2W(m_dwFlags&fgReplaceUseQuestionMark?IDS_REPLACEQMARK:IDS_REPLACEASTERISKS));
	SetDlgItemText(IDC_REPLACESPACES,Label);
}


BOOL CLocateDlg::CAdvancedDlg::LoadRegistry()
{

	m_aReplaceChars.RemoveAll();

	LPWSTR pData=NULL;
	CRegKey2 RegKey;
	if (RegKey.OpenKey(HKCU,"\\Locate",CRegKey::defRead)!=ERROR_SUCCESS)
		return FALSE;

	// Flags
	DWORD dwTemp=0;
	if (RegKey.QueryValue("AdvancedDlgFlags",dwTemp))
		m_dwFlags=BYTE((m_dwFlags&~fgSave)|dwTemp);

	// ReplaceCharsWithAsterisks
	DWORD dwLength=RegKey.QueryValueLength("ReplaceCharsWithAsterisks");

	if (dwLength==0)
		return TRUE; // No "ReplaceCharsWithAsterisks"


	pData=new WCHAR[dwLength+1];
	if (RegKey.QueryValue(L"ReplaceCharsWithAsterisks",pData,dwLength+1)<dwLength)
	{
		delete[] pData;
		return FALSE;
	}

	LPCWSTR pStr=pData;
	while (*pStr!='\0')
	{
		int nLen=istrlen(pStr);
		m_aReplaceChars.Add(alloccopy(pStr,nLen));
		pStr+=nLen+1;
	}


	delete[] pData;
	return TRUE;
}

BOOL CLocateDlg::CAdvancedDlg::SaveRegistry()
{
	CRegKey2 RegKey;
	if (RegKey.OpenKey(HKCU,"\\Locate",CRegKey::defWrite)!=ERROR_SUCCESS)
		return FALSE;

	RegKey.SetValue("AdvancedDlgFlags",m_dwFlags&fgSave);

	if (m_aReplaceChars.GetSize()>0)
	{
		DWORD dwLength=1;
		for (int i=0;i<m_aReplaceChars.GetSize();i++)
			dwLength+=istrlen(m_aReplaceChars[i])+1;
			
		LPWSTR pData=new WCHAR[dwLength];
		LPWSTR pPtr=pData;
		for (int i=0;i<m_aReplaceChars.GetSize();i++)
		{
			int iLen=istrlen(m_aReplaceChars[i])+1;
			MemCopyW(pPtr,m_aReplaceChars[i],iLen);
			pPtr+=iLen;
		}
		*pPtr='\0';
	
		return RegKey.SetValue(L"ReplaceCharsWithAsterisks",pData,dwLength,REG_MULTI_SZ)==dwLength;
	}

	return RegKey.DeleteValue("ReplaceCharsWithAsterisks")==ERROR_SUCCESS;
}


void CLocateDlg::CAdvancedDlg::ReplaceCharsWithAsterisks(CStringW& sString)
{
	// Replace all desired char in sString
	WCHAR ch=m_dwFlags&fgReplaceUseQuestionMark?L'?':L'*';
	if (m_aReplaceChars.GetSize()==0)
	{
		// Just replace space
		sString.ReplaceChars(L' ',ch);
	}
	else
	{
		// Go trhough all desired chars
		for (int i=0;i<m_aReplaceChars.GetSize();i++)
		{
			if (m_aReplaceChars[i][1]=='\0')
				sString.ReplaceChars(m_aReplaceChars[i][0],L'*');
			else
			{
				// "Character" is actually a string
				int nLen=istrlen(m_aReplaceChars[i]);
				int nIndex=sString.Find(m_aReplaceChars[i]);
				while (nIndex!=-1)
				{
					CStringW sNewString(sString,nIndex);
					sNewString << ch << ((LPCWSTR)sString)+nIndex+nLen;
					sString.Swap(sNewString);
					nIndex=sString.Find(m_aReplaceChars[i]);
				}
			}
		}
	}
	
}



//////////////////////////////////////////////////////////////////////////////
// CLocateDlg::CAdvancedDlg::CReplaceCharsDlg
//////////////////////////////////////////////////////////////////////////////


BOOL CLocateDlg::CAdvancedDlg::CReplaceCharsDlg::OnInitDialog(HWND hwndFocus)
{
	CDialog::OnInitDialog(hwndFocus);
	
	if (m_raChars.GetSize()>0)
	{
		CStringW Custom;
		for (int i=0;i<m_raChars.GetSize();i++)
		{
			if (m_raChars[i][1]=='\0')
			{
				switch (m_raChars[i][0])
				{
				case ' ':
					CheckDlgButton(IDC_SPACE,TRUE);
					break;
				case '-':
					CheckDlgButton(IDC_HYPHEN,TRUE);
					break;
				case '_':
					CheckDlgButton(IDC_UNDERLINE,TRUE);
					break;
				default:
					Custom << m_raChars[i][0] << ';';
				}
			}
			else
			{
				Custom << m_raChars[i] << ';';
			}
		}


		if (Custom.LastChar()==';')
			Custom.DelLastChar();

		SetDlgItemText(IDC_CUSTOM,Custom);
	}
	else
		CheckDlgButton(IDC_SPACE,TRUE);

	CheckDlgButton(IDC_QUESTIONMARK,m_bUseQuestionMark);

	if (!LoadPosition(HKCU,CRegKey2::GetCommonKey()+"\\Dialogs","ReplaceCharsPos",fgOnlyNormalPosition|fgNoSize))
		CenterWindow();

	return FALSE;
}

void CLocateDlg::CAdvancedDlg::CReplaceCharsDlg::OnDestroy()
{
	CDialog::OnDestroy();
	SavePosition(HKCU,CRegKey2::GetCommonKey()+"\\Dialogs","ReplaceCharsPos");
}

BOOL CLocateDlg::CAdvancedDlg::CReplaceCharsDlg::OnCommand(WORD wID,WORD wNotifyCode,HWND hControl)
{
	switch (wID)
	{
	case IDC_OK:
		{
			m_raChars.RemoveAll();

			
			if (IsDlgButtonChecked(IDC_SPACE))
				m_raChars.Add(alloccopy(L" "));
			if (IsDlgButtonChecked(IDC_HYPHEN))
				m_raChars.Add(alloccopy(L"-"));
			if (IsDlgButtonChecked(IDC_UNDERLINE))
				m_raChars.Add(alloccopy(L"_"));

			CStringW sCustom;
			GetDlgItemText(IDC_CUSTOM,sCustom);

			LPCWSTR pPtr=sCustom;
			
			for (;;)
			{
				// Ignore extra semi-colons
				while (*pPtr==';') 
					pPtr++;

				if (*pPtr=='\0')
					break;

				int nLen;
				for (nLen=0;pPtr[nLen]!='\0' && pPtr[nLen]!=';';nLen++);

				WCHAR* pNew=alloccopy(pPtr,nLen);

				// Check whether already in list
				int i;
				for (i=0;i<m_raChars.GetSize();i++)
				{
					if (wcscmp(m_raChars[i],pNew)==0)
						break;
				}

				if (i<m_raChars.GetSize())
					delete[] pNew;
				else
					m_raChars.Add(pNew);

				pPtr+=nLen;
			}

			if (m_raChars.GetSize()==1)
			{
				if (wcscmp(m_raChars[0],L" ")==0)
				{
					// Only space, remove the list
					m_raChars.RemoveAll();
				}
			}

			m_bUseQuestionMark=IsDlgButtonChecked(IDC_QUESTIONMARK);

			EndDialog(1);
			break;
		}
	case IDCANCEL:
	case IDC_CANCEL:
		EndDialog(0);
		break;
	}
	return CDialog::OnCommand(wID,wNotifyCode,hControl);
}

BOOL CLocateDlg::CAdvancedDlg::CReplaceCharsDlg::OnClose()
{
	CDialog::OnClose();
	EndDialog(0);
	return FALSE;
}




//////////////////////////////////////////////////////////////////////////////
// CLocateDlg::CAdvancedDlg::FileType
//////////////////////////////////////////////////////////////////////////////

CLocateDlg::CAdvancedDlg::FileType::FileType(LPCWSTR& szBuildIn,HIMAGELIST hImageList)
:	szType(NULL),szIconPath(NULL)
{
	
	
	// First string is title
	DWORD dwLength=istrlenw(szBuildIn);
	szTitle=new WCHAR[++dwLength];
	MemCopyW(szTitle,szBuildIn,dwLength);
	szBuildIn+=dwLength;
	
	// Next string is extension
	dwLength=istrlenw(szBuildIn);
	dwExtensionLength=(++dwLength)+1;
	szExtensions=new WCHAR[dwExtensionLength];
	MemCopyW(szExtensions,szBuildIn,dwLength);
	szExtensions[dwLength]=L'\0';
	szBuildIn+=dwLength;
	replacech(szExtensions,L' ',L'\0');
	
	// Third is icon index, if available
	if (*szBuildIn!=L'\0')
	{
		hIcon=ImageList_ExtractIcon(NULL,hImageList,_wtoi(szBuildIn));
		for (;*szBuildIn!=L'\0';szBuildIn++);
	}
	else
		hIcon=GetLocateDlg()->m_AdvancedDlg.m_hDefaultTypeIcon;
	
	szBuildIn++;
}

CLocateDlg::CAdvancedDlg::FileType::~FileType()
{
	if (szType!=NULL)
		delete[] szType;
	if (szTitle!=NULL)
		delete[] szTitle;
	if (szExtensions!=NULL)
		delete[] szExtensions;
	if (hIcon!=NULL && hIcon!=GetLocateDlg()->m_AdvancedDlg.m_hDefaultTypeIcon)
		DestroyIcon(hIcon);
	if (szIconPath!=NULL)
		delete[] szIconPath;
}

void CLocateDlg::CAdvancedDlg::FileType::AddExtension(LPCWSTR szExtension,DWORD dwNewExtensionLength)
{
	if (szExtensions==NULL)
	{
		dwExtensionLength=dwNewExtensionLength+1;
		szExtensions=new WCHAR[dwExtensionLength];
		MemCopyW(szExtensions,szExtension,dwNewExtensionLength);
		szExtensions[dwNewExtensionLength]=L'\0';
	}
	else
	{
		WCHAR* szNewExtensions=new WCHAR[dwExtensionLength+dwNewExtensionLength];
		MemCopyW(szNewExtensions,szExtensions,dwExtensionLength);
		MemCopyW(szNewExtensions+dwExtensionLength-1,szExtension,dwNewExtensionLength);
		dwExtensionLength+=dwNewExtensionLength;
		szNewExtensions[dwExtensionLength-1]=L'\0';
		delete[] szExtensions;
		szExtensions=szNewExtensions;
	}
}

void CLocateDlg::CAdvancedDlg::FileType::SetIcon(CRegKey& rKey,BOOL toHandle)
{
	// Cleaning previous values
	if (szIconPath!=NULL)
		delete[] szIconPath;
	if (hIcon!=NULL && hIcon!=GetLocateDlg()->m_AdvancedDlg.m_hDefaultTypeIcon)
	{
		DestroyIcon(hIcon);
		hIcon=NULL;
	}

	CRegKey RegKey;
	
	if (RegKey.OpenKey(rKey,"DefaultIcon",CRegKey::openExist|CRegKey::samQueryValue|CRegKey::samExecute)!=ERROR_SUCCESS)
		return;
	CStringW sIconPath;
	if (!RegKey.QueryValue(L"",sIconPath))
		return;

	szIconPath=alloccopy(sIconPath,sIconPath.GetLength());

	if (toHandle)
		ExtractIconFromPath();

}

void CLocateDlg::CAdvancedDlg::FileType::ExtractIconFromPath()
{
	if (hIcon!=NULL)
		return;
	if (szIconPath==NULL) // Maybe general item
	{
		hIcon=GetLocateDlg()->m_AdvancedDlg.m_hDefaultTypeIcon;
		return;
	}

	int iIndex=0;

	WCHAR* szIconIndex;
	for (szIconIndex=szIconPath;*szIconIndex!=L',' && *szIconIndex!=L'\0';szIconIndex++);
	if (*szIconIndex!=L'\0')
	{
		iIndex=_wtoi(szIconIndex+1);
		*szIconIndex=L'\0';
	}

	if (IsUnicodeSystem())
	{
		WCHAR szExpanded[MAX_PATH];
		if (ExpandEnvironmentStringsW(szIconPath,szExpanded,MAX_PATH)==0)
		{
			// Error
			hIcon=GetLocateDlg()->m_AdvancedDlg.m_hDefaultTypeIcon;
			return;
		}
		
		ExtractIconExW(szExpanded,iIndex,NULL,&hIcon,1);
		if (hIcon==NULL)
			hIcon=GetLocateDlg()->m_AdvancedDlg.m_hDefaultTypeIcon;
	}
	else
	{
		char szExpanded[MAX_PATH];
		if (ExpandEnvironmentStrings(W2A(szIconPath),szExpanded,MAX_PATH)==0)
		{
			// Error
			hIcon=GetLocateDlg()->m_AdvancedDlg.m_hDefaultTypeIcon;
			return;
		}
		
		ExtractIconEx(szExpanded,iIndex,NULL,&hIcon,1);
		if (hIcon==NULL)
			hIcon=GetLocateDlg()->m_AdvancedDlg.m_hDefaultTypeIcon;
	}
}