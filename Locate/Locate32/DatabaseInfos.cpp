/* Locate32 - Copyright (c) 1997-2010 Janne Huttunen */

#include <HFCLib.h>
#include "Locate32.h"

CDatabaseInfos::CDatabaseInfos(HWND hParent)
:	CPropertySheet(IDS_DATABASEINFOCAPTION,hParent,0)
{
	const CArray<PDATABASE>& rDatabases=GetLocateApp()->GetDatabases();

	for (int i=0;i<rDatabases.GetSize();i++)
	{
		CDatabaseInfoPage* pPage=new CDatabaseInfoPage(rDatabases[i]);
		
		m_aInfoPages.Add(pPage);
		AddPage((CPropertyPage*)pPage);
	}

	m_psh.dwFlags|=PSH_NOAPPLYNOW|PSH_NOCONTEXTHELP;
}

BOOL CDatabaseInfos::CDatabaseInfoPage::OnCommand(WORD wID,WORD wNotifyCode,HWND hControl)
{
	switch(wID)
	{
	case IDC_OK:
		EndDialog(0);
		break;
	}
	return CDialog::OnCommand(wID,wNotifyCode,hControl);
}

BOOL CDatabaseInfos::CDatabaseInfoPage::OnInitDialog(HWND hwndFocus)
{
	CStringW Drives;
	CStringW Temp;
	
	SetDlgItemText(IDC_FILE,m_pDatabase->GetArchiveName());

	
	if (m_pDatabase->DoDatabaseFileExist())
	{
		CDatabaseInfo* di=CDatabaseInfo::GetFromDatabase(m_pDatabase);
		
		//Resolving information
		if (di==NULL)
		{
			SendDlgItemMessage(IDC_DATABASEFILEICON,STM_SETICON,(WPARAM)LoadIcon(IDI_INVALIDDBFILE),0);
		
			Temp.LoadString(IDS_DBNOTVALID);
			SetDlgItemText(IDC_CREATED,Temp);
			Temp.LoadString(IDS_UNKNOWN);
			SetDlgItemText(IDC_CREATOR,Temp);
			SetDlgItemText(IDC_DESCRIPTION,Temp);
			SetDlgItemText(IDC_FILESIZE,Temp);
			SetDlgItemText(IDC_VERSION,Temp);
			SetDlgItemText(IDC_FILES,Temp);
			SetDlgItemText(IDC_DIRECTORIES,Temp);
			SetDlgItemText(IDC_CHARSET,Temp);
			return CDialog::OnInitDialog(hwndFocus);
		}
	
		// Setting file icon
		SendDlgItemMessage(IDC_DATABASEFILEICON,STM_SETICON,(WPARAM)LoadIcon(IDI_DBFILE),0);
		
		// Initializing list control
		m_pList=new CListCtrl(GetDlgItem(IDC_FOLDERS));
		CLocateDlg::SetSystemImageLists(m_pList);
		m_pList->SetExtendedListViewStyle(LVS_EX_HEADERDRAGDROP,LVS_EX_HEADERDRAGDROP);
		if (IsUnicodeSystem())
			m_pList->SetUnicodeFormat(TRUE);
	
		if (di->bVersion>=20)
		{
			m_pList->InsertColumn(0,ID2W(IDS_VOLUMEPATH),LVCFMT_LEFT,100,0);
			m_pList->InsertColumn(1,ID2W(IDS_VOLUMETYPE),LVCFMT_LEFT,50,1);
			m_pList->InsertColumn(2,ID2W(IDS_VOLUMELABEL),LVCFMT_LEFT,80,2);
			m_pList->InsertColumn(3,ID2W(IDS_VOLUMESERIAL),LVCFMT_LEFT,70,3);
			m_pList->InsertColumn(4,ID2W(IDS_VOLUMEFILESYSTEM),LVCFMT_LEFT,55,4);
			m_pList->InsertColumn(5,ID2W(IDS_VOLUMEFILES),LVCFMT_RIGHT,50,5);
			m_pList->InsertColumn(6,ID2W(IDS_VOLUMEDIRECTORIES),LVCFMT_RIGHT,50,6);
		}
		else
		{
			m_pList->InsertColumn(0,ID2W(IDS_VOLUMEPATH),LVCFMT_LEFT,300,0);
			m_bOldDB=TRUE;
		}

		CString str("Database Info List Widths for ");
		str << m_pDatabase->GetName();
		m_pList->LoadColumnsState(HKCU,CRegKey2::GetCommonKey()+"\\Dialogs",str);
		
		//Settting creator and description
		SetDlgItemText(IDC_CREATOR,di->sCreator);
		SetDlgItemText(IDC_DESCRIPTION,di->sDescription);
		
		//Setting created date
		if (di->tCreationTime.m_time>0)
		{
			FILETIME ft;
			SYSTEMTIME st=di->tCreationTime;
			WORD wDate,wTime;
			SystemTimeToFileTime(&st,&ft);
			FileTimeToDosDateTime(&ft,&wDate,&wTime);
			
			LPWSTR szDate=GetLocateApp()->FormatDateAndTimeString(wDate,wTime);
			SetDlgItemText(IDC_CREATED,szDate);
			delete[] szDate;
		}
		else
			SetDlgItemText(IDC_CREATED,ID2W(IDS_UNKNOWN));
		
		// Setting filesize
		if (di->dwFileSize>50000)
		{
			Temp=di->dwFileSize/1024;
			Temp.AddString(IDS_KB);
		}
		else
		{
			Temp=di->dwFileSize;
			Temp.AddString(IDS_BYTES);
		}
		SetDlgItemText(IDC_FILESIZE,Temp);
		
		// Setting number of files and directories
		if (di->dwNumberOfDirectories!=(DWORD)-1)
			Temp=di->dwNumberOfFiles;
		else
			Temp.LoadString(IDS_UNKNOWN);
		SetDlgItemText(IDC_FILES,Temp);
		if (di->dwNumberOfDirectories!=(DWORD)-1)
			Temp=di->dwNumberOfDirectories;
		else
			Temp.LoadString(IDS_UNKNOWN);
		SetDlgItemText(IDC_DIRECTORIES,Temp);
		
		// Setting version
		switch (di->bVersion)
		{
		case 1:
			Temp.Format(IDS_DBVERSION1,di->bVersion);
			break;
		case 2:
			Temp.Format(IDS_DBVERSION2,di->bVersion);
			break;
		case 3:
			Temp.Format(IDS_DBVERSIONSFN,di->bVersion);
			break;
		case 4:
			Temp.Format(IDS_DBVERSIONLFN,di->bVersion);
			break;
		default:
			if (di->bLongFilenames)
				Temp.Format(IDS_DBVERSIONLFN,di->bVersion);
			else
				Temp.Format(IDS_DBVERSIONSFN,di->bVersion);
			break;
		}
		SetDlgItemText(IDC_VERSION,Temp);
		switch (di->cCharset)
		{
		case CDatabaseInfo::OEM:
			Temp.LoadString(IDS_OEMCHARSET);
			break;
		case CDatabaseInfo::Ansi:
			Temp.LoadString(IDS_ANSICHARSET);
			break;
		case CDatabaseInfo::Unicode:
			Temp.LoadString(IDS_UNICODECHARSET);
			break;
		}
		SetDlgItemText(IDC_CHARSET,Temp);

		
		// Setting drive/folder information
		for (int i=0;i<di->aRootFolders.GetSize();i++)
		{
			switch (di->aRootFolders.GetAt(i)->rtType)
			{
			case CDatabaseInfo::Unknown:
				Temp.LoadString(IDS_VOLUMETYPEUNKNOWN);
				break;
			case CDatabaseInfo::Fixed:
				Temp.LoadString(IDS_VOLUMETYPEFIXED);
				break;
			case CDatabaseInfo::Removable:
				Temp.LoadString(IDS_VOLUMETYPEREMOVABLE);
				break;
			case CDatabaseInfo::CDRom:
				Temp.LoadString(IDS_VOLUMETYPECDROM);
				break;
			case CDatabaseInfo::Remote:
				Temp.LoadString(IDS_VOLUMETYPEREMOTE);
				break;
			case CDatabaseInfo::Ramdisk:
				Temp.LoadString(IDS_VOLUMETYPERAMDISK);
				break;
			case CDatabaseInfo::Directory:
				Temp.LoadString(IDS_VOLUMETYPEDIRECTORY);
				break;
			}

			LVITEMW li;
			li.mask=LVIF_TEXT|LVIF_IMAGE;
			li.iItem=i;
			SHFILEINFOW fi;
			if (ShellFunctions::GetFileInfo(di->aRootFolders[i]->sRootMap.IsEmpty()?
				di->aRootFolders.GetAt(i)->sPath+L'\\':
				di->aRootFolders.GetAt(i)->sRootMap+L'\\',
				FILE_ATTRIBUTE_NORMAL,&fi,SHGFI_SMALLICON|SHGFI_SYSICONINDEX|
				(GetLocateApp()->GetProgramFlags()&CLocateApp::pfAvoidToAccessWhenReadingIcons?SHGFI_USEFILEATTRIBUTES:0)))
				li.iImage=fi.iIcon;
			else
				li.iImage=DEL_IMAGE;
			li.iSubItem=0;
			if (!di->aRootFolders[i]->sRootMap.IsEmpty())
			{
				CStringW tmp;
				tmp << di->aRootFolders[i]->sPath << L" -> " << di->aRootFolders[i]->sRootMap;
				li.pszText=tmp.GetBuffer();
				m_pList->InsertItem(&li);
			}
			else
			{
				li.pszText=di->aRootFolders[i]->sPath.GetBuffer();
				m_pList->InsertItem(&li);
			}
			
			if (!m_bOldDB)
			{
				li.mask=LVIF_TEXT;
				li.iSubItem=1;
				li.pszText=Temp.GetBuffer();
				m_pList->SetItem(&li);

				li.iSubItem=2;
				li.pszText=di->aRootFolders.GetAt(i)->sVolumeName.GetBuffer();
				m_pList->SetItem(&li);

				if (di->aRootFolders.GetAt(i)->dwVolumeSerial!=0 &&
					di->aRootFolders.GetAt(i)->dwVolumeSerial!=0xFFFF)
				{
					li.iSubItem=3;
					Temp.Format(L"%0X-%0X",HIWORD(di->aRootFolders.GetAt(i)->dwVolumeSerial),
						LOWORD(di->aRootFolders.GetAt(i)->dwVolumeSerial));
					li.pszText=Temp.GetBuffer();
					m_pList->SetItem(&li);
				}

				li.iSubItem=4;
				li.pszText=di->aRootFolders.GetAt(i)->sFileSystem.GetBuffer();
				m_pList->SetItem(&li);

				li.iSubItem=5;
				Temp=di->aRootFolders.GetAt(i)->dwNumberOfFiles;
				li.pszText=Temp.GetBuffer();
				m_pList->SetItem(&li);

				li.iSubItem=6;
				Temp=di->aRootFolders.GetAt(i)->dwNumberOfDirectories;
				li.pszText=Temp.GetBuffer();
				m_pList->SetItem(&li);
			}			
		}
	
		delete di;
	}
	else
	{
		// Setting file icon
		SendDlgItemMessage(IDC_DATABASEFILEICON,STM_SETICON,(WPARAM)LoadIcon(IDI_DELETEDFILE),0);
		
		Temp.LoadString(IDS_NOTEXIST);
		SetDlgItemText(IDC_CREATED,Temp);
		Temp.LoadString(IDS_UNKNOWN);
		SetDlgItemText(IDC_CREATOR,Temp);
		SetDlgItemText(IDC_DESCRIPTION,Temp);
		SetDlgItemText(IDC_FILESIZE,Temp);
		SetDlgItemText(IDC_VERSION,Temp);
		SetDlgItemText(IDC_FILES,Temp);
		SetDlgItemText(IDC_DIRECTORIES,Temp);
	}
	
	return CPropertyPage::OnInitDialog(hwndFocus);
}

void CDatabaseInfos::CDatabaseInfoPage::OnDestroy()
{
	if (m_pList!=NULL)
	{
		CString str("Database Info List Widths for ");
		str << m_pDatabase->GetName();

		m_pList->SaveColumnsState(HKCU,CRegKey2::GetCommonKey()+"\\Dialogs",str);

		delete m_pList;
		m_pList=NULL;
	}

	CDialog::OnDestroy();
}



