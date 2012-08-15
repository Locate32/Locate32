/* Locate32 - Copyright (c) 1997-2010 Janne Huttunen */

#include <HFCLib.h>
#include "Locate32.h"

#include <md5.h>
#ifdef _DEBUG
#pragma comment(lib, "libmd5d.lib")
#else
#pragma comment(lib, "libmd5.lib")
#endif




void CLocatedItem::SetFolder(const CLocater* pLocater)
{
	ItemDebugMessage("CLocatedItem::SetFolder BEGIN");

	wDatabaseID=pLocater->GetCurrentDatabaseID();
	wRootID=pLocater->GetCurrentDatabaseRootID();
	
	// Setting path, name and extension
	DWORD nPathLen=pLocater->GetCurrentPathLen();
	bNameLength=BYTE(pLocater->GetFolderNameLen());
	szPath=new WCHAR[nPathLen+bNameLength+2];
	
	int nLen=MemCopyAtoW(szPath,nPathLen+bNameLength+2,pLocater->GetCurrentPath(),nPathLen++);
	szName=szPath+nLen+1;
	MemCopyAtoW(szName,bNameLength+1,pLocater->GetFolderName(),DWORD(bNameLength)+1);
	
	for (bExtensionPos=bNameLength-1; szName[bExtensionPos-1]!='.' && bExtensionPos>0 ;bExtensionPos--);
	if (bExtensionPos==0)
		bExtensionPos=bNameLength;
	
	// Settig title (maybe)
	if ((GetLocateDlg()->GetFlags()&(CLocateDlg::fgLVAlwaysShowExtensions|CLocateDlg::fgLV1stCharUpper))==CLocateDlg::fgLVAlwaysShowExtensions)
	{
		szFileTitle=szName;
		dwFlags|=LITEM_FILETITLEOK;
	}

	// Setting file size;
	dwFileSize=DWORD(-1);
	wFileSizeHi=0;

	// Setting file time
	wModifiedDate=pLocater->GetFolderModifiedDate();
    wModifiedTime=pLocater->GetFolderModifiedTime();
	wCreatedDate=pLocater->GetFolderCreatedDate();
	wCreatedTime=pLocater->GetFolderCreatedTime();
	wAccessedDate=pLocater->GetFolderAccessedDate();
    wAccessedTime=pLocater->GetFolderAccessedTime();
	
	// Setting attributes
	wAttribs=WORD(pLocater->GetFolderAttributes())&LITEMATTRIB_DBATTRIBFLAG;
	
	iIcon=DIR_IMAGE;
	if (nPathLen<=3)
		iParentIcon=DRIVE_IMAGE;
	else
		iParentIcon=DIR_IMAGE;


	ItemDebugMessage("CLocatedItem::SetFolder END");
}

void CLocatedItem::SetFolderW(const CLocater* pLocater)
{
	ItemDebugMessage("CLocatedItem::SetFolderW BEGIN");

	wDatabaseID=pLocater->GetCurrentDatabaseID();
	wRootID=pLocater->GetCurrentDatabaseRootID();
	
	// Setting path, name and extension
	DWORD nPathLen=pLocater->GetCurrentPathLen();
	bNameLength=BYTE(pLocater->GetFolderNameLen());
	szPath=new WCHAR[nPathLen+bNameLength+2];
	
	MemCopyW(szPath,pLocater->GetCurrentPathW(),nPathLen);
	szName=szPath+(++nPathLen);
	MemCopyW(szName,pLocater->GetFolderNameW(),DWORD(bNameLength)+1);
	
	for (bExtensionPos=bNameLength-1; szName[bExtensionPos-1]!='.' && bExtensionPos>0 ;bExtensionPos--);
	if (bExtensionPos==0)
		bExtensionPos=bNameLength;
	
	// Settig title (maybe)
	if ((GetLocateDlg()->GetFlags()&(CLocateDlg::fgLVAlwaysShowExtensions|CLocateDlg::fgLV1stCharUpper))==CLocateDlg::fgLVAlwaysShowExtensions)
	{
		szFileTitle=szName;
		dwFlags|=LITEM_FILETITLEOK;
	}

	// Setting file size;
	dwFileSize=DWORD(-1);
	wFileSizeHi=0;

	// Setting file time
	wModifiedDate=pLocater->GetFolderModifiedDateW();
    wModifiedTime=pLocater->GetFolderModifiedTimeW();
	wCreatedDate=pLocater->GetFolderCreatedDateW();
	wCreatedTime=pLocater->GetFolderCreatedTimeW();
	wAccessedDate=pLocater->GetFolderAccessedDateW();
    wAccessedTime=pLocater->GetFolderAccessedTimeW();
	
	// Setting attributes
	wAttribs=WORD(pLocater->GetFolderAttributes())&LITEMATTRIB_DBATTRIBFLAG;
	
	iIcon=DIR_IMAGE;
	if (nPathLen<=3)
		iParentIcon=DRIVE_IMAGE;
	else
		iParentIcon=DIR_IMAGE;


	ItemDebugMessage("CLocatedItem::SetFolder END");
}

void CLocatedItem::SetFile(const CLocater* pLocater)
{
	ItemDebugMessage("CLocatedItem::SetFile BEGIN");

	wDatabaseID=pLocater->GetCurrentDatabaseID();
	wRootID=pLocater->GetCurrentDatabaseRootID();
	
	// Setting path, name and extension
	DWORD nPathLen=pLocater->GetCurrentPathLen();
	bNameLength=BYTE(pLocater->GetFileNameLen());
	szPath=new WCHAR[nPathLen+bNameLength+2];
	int nLen=MemCopyAtoW(szPath,nPathLen+bNameLength+2,pLocater->GetCurrentPath(),nPathLen++);
	szName=szPath+nLen+1;
	MemCopyAtoW(szName,bNameLength+1,pLocater->GetFileName(),DWORD(bNameLength)+1);
	bExtensionPos=pLocater->GetFileExtensionPos();
	if (bExtensionPos==0 && *szName!='.')
		bExtensionPos=bNameLength;
	else
		bExtensionPos++;
	
	// Settig title (maybe)
	if ((GetLocateDlg()->GetFlags()&(CLocateDlg::fgLVAlwaysShowExtensions|CLocateDlg::fgLV1stCharUpper))==CLocateDlg::fgLVAlwaysShowExtensions)
	{
		szFileTitle=szName;
		dwFlags|=LITEM_FILETITLEOK;
	}

	// Setting file size;
	dwFileSize=pLocater->GetFileSizeLo();
	wFileSizeHi=pLocater->GetFileSizeHi();

	// Setting file time
	wModifiedDate=pLocater->GetFileModifiedDate();
	wModifiedTime=pLocater->GetFileModifiedTime();
	wCreatedDate=pLocater->GetFileCreatedDate();
	wCreatedTime=pLocater->GetFileCreatedTime();
	wAccessedDate=pLocater->GetFileAccessedDate();
	wAccessedTime=pLocater->GetFileAccessedTime();

	// Setting attributes
	wAttribs=WORD(pLocater->GetFileAttributes())&LITEMATTRIB_DBATTRIBFLAG;

	iIcon=DEF_IMAGE;
	if (nPathLen<=3)
		iParentIcon=DRIVE_IMAGE;
	else
		iParentIcon=DIR_IMAGE;

	ItemDebugMessage("CLocatedItem::SetFile END");

}

void CLocatedItem::SetFileW(const CLocater* pLocater)
{
	ItemDebugMessage("CLocatedItem::SetFileW BEGIN");

	wDatabaseID=pLocater->GetCurrentDatabaseID();
	wRootID=pLocater->GetCurrentDatabaseRootID();
	
	// Setting path, name and extension
	DWORD nPathLen=pLocater->GetCurrentPathLen();
	bNameLength=BYTE(pLocater->GetFileNameLen());
	szPath=new WCHAR[nPathLen+bNameLength+2];
	MemCopyW(szPath,pLocater->GetCurrentPathW(),nPathLen);
	szName=szPath+(++nPathLen);
	MemCopyW(szName,pLocater->GetFileNameW(),DWORD(bNameLength)+1);
	bExtensionPos=pLocater->GetFileExtensionPos();
	if (bExtensionPos==0 && *szName!='.')
		bExtensionPos=bNameLength;
	else
		bExtensionPos++;
	
	//ASSERT(!FileSystem::IsDirectory(GetPath()));

	// Settig title (maybe)
	if ((GetLocateDlg()->GetFlags()&(CLocateDlg::fgLVAlwaysShowExtensions|CLocateDlg::fgLV1stCharUpper))==CLocateDlg::fgLVAlwaysShowExtensions)
	{
		szFileTitle=szName;
		dwFlags|=LITEM_FILETITLEOK;
	}

	// Setting file size;
	dwFileSize=pLocater->GetFileSizeLoW();
	wFileSizeHi=pLocater->GetFileSizeHiW();

	// Setting file time
	wModifiedDate=pLocater->GetFileModifiedDateW();
	wModifiedTime=pLocater->GetFileModifiedTimeW();
	wCreatedDate=pLocater->GetFileCreatedDateW();
	wCreatedTime=pLocater->GetFileCreatedTimeW();
	wAccessedDate=pLocater->GetFileAccessedDateW();
	wAccessedTime=pLocater->GetFileAccessedTimeW();

	// Setting attributes
	wAttribs=WORD(pLocater->GetFileAttributes())&LITEMATTRIB_DBATTRIBFLAG;

	iIcon=DEF_IMAGE;
	if (nPathLen<=3)
		iParentIcon=DRIVE_IMAGE;
	else
		iParentIcon=DIR_IMAGE;

	ItemDebugMessage("CLocatedItem::SetFileW END");

}

void CLocatedItem::ClearData()
{
	//ItemDebugMessage("CLocatedItem::ClearData BEGIN");

	if (szFileTitle!=NULL && szFileTitle!=szName)
		delete[] szFileTitle;
		
	szFileTitle=NULL;
	if (szPath!=NULL)
	{
		delete[] szPath;
		szPath=NULL;
	}
	if (szType!=NULL)
	{
		delete[] szType;
		szType=NULL;
	}
	
	DeleteAllExtraFields();

	//ItemDebugMessage("CLocatedItem::ClearData END");
}

void CLocatedItem::UpdateByDetail(DetailType nDetail)
{
	switch(nDetail)
	{
	case Name:
		UpdateFileTitle();
		UpdateIcon();
		break;
	case InFolder:
		UpdateParentIcon();
		break;
	case FileSize:
	case DateModified:
	case DateCreated:
	case DateAccessed:
		UpdateFileSizeAndTime();
		break;
	case FileType:
		UpdateType();
		break;
	case Attributes:
		UpdateAttributes();
		break;
	case ImageDimensions:
		UpdateDimensions();
		break;
	case Owner:
		UpdateOwner();
		break;
	case ShortFileName:
		UpdateShortFileName();
		break;
	case ShortFilePath:
		UpdateShortFilePath();
		break;
	case MD5sum:
		ComputeMD5sum();
		break;
	case Author:
	case Title:
	case Subject:
	case Pages:
	case Comments:
	case Keywords:
		UpdateSummaryProperties();
		break;
	case Category:
		UpdateDocSummaryProperties();
		break;
	case Description:
	case FileVersion:
	case ProductName:
	case ProductVersion:
		UpdateVersionInformation();
		break;
	case Thumbnail:
		LoadThumbnail();
		break;
	case IfDeleted:
		CheckIfDeleted();
		break;
	}	
}


	
BOOL CLocatedItem::ShouldUpdateByDetail(DetailType nDetail) const
{
	switch(nDetail)
	{
	case FullPath:
	case Extension:
	case Database:
	case DatabaseDescription:
	case DatabaseArchive:
	case VolumeLabel:
	case VolumeSerial:
	case VolumeFileSystem:
		return FALSE;
	case Name:
		return ShouldUpdateFileTitle() || ShouldUpdateIcon();
	case InFolder:
		return ShouldUpdateParentIcon();
	case FileSize:
		return ShouldUpdateFileSize();
	case DateModified:
	case DateCreated:
	case DateAccessed:
		return ShouldUpdateTimeAndDate();
	case FileType:
		return ShouldUpdateType();
	case Attributes:
		return ShouldUpdateAttributes();
	default:
		return ShouldUpdateExtra(nDetail);
	}	
}




void CLocatedItem::UpdateFileTitle()
{
	ItemDebugMessage("CLocatedItem::UpdateFileTitle BEGIN");
	

	WCHAR* pNewFileTitle=NULL;
	BOOL bCheckCase=TRUE;

	if (IsFolder())
		pNewFileTitle=szName; // Show full name for folders
	else if ((GetLocateDlg()->GetFlags()&CLocateDlg::fgLVMethodFlag)==CLocateDlg::fgLVUseGetFileTitle)
	{
		WORD nLen=FileSystem::GetFileTitle(GetPath(),NULL,0);
		if (nLen!=0)
		{
			pNewFileTitle=new WCHAR[nLen];
			FileSystem::GetFileTitle(GetPath(),pNewFileTitle,nLen);
			bCheckCase=FALSE;
		}
		else 
			pNewFileTitle=szName;
	}
	else
	{
		switch (GetLocateDlg()->GetFlags()&CLocateDlg::fgLVExtensionFlag)
		{
		case CLocateDlg::fgLVAlwaysShowExtensions:
			pNewFileTitle=szName;
			break;
		case CLocateDlg::fgLVHideKnownExtensions:
			{
				if (bExtensionPos==bNameLength)
				{
					// No extension
					pNewFileTitle=szName;
					break;
				}
				BOOL bShowExtension=TRUE;
				CRegKey RegKey;
				CString Type;
				
				if (RegKey.OpenKey(HKCR,szName+bExtensionPos-1,CRegKey::openExist|CRegKey::samRead|CRegKey::samQueryValue)==ERROR_SUCCESS)
				{
					DWORD nTemp=0;
					// Trying first HKCR/.ext
					if (RegQueryValueEx(RegKey,"AlwaysShowExt",NULL,NULL,NULL,&nTemp)!=ERROR_SUCCESS)
					{
						// not found, trying HKCR/type
						RegKey.QueryValue("",Type);
						RegKey.CloseKey();
					
						if (!Type.IsEmpty())
						{
							if (RegKey.OpenKey(HKCR,Type,CRegKey::openExist|CRegKey::samRead|CRegKey::samQueryValue)==ERROR_SUCCESS)
							{
								if (RegQueryValueEx(RegKey,"AlwaysShowExt",NULL,NULL,NULL,&nTemp)!=ERROR_SUCCESS)
									bShowExtension=FALSE;
								else if (RegQueryValueEx(RegKey,"NeverShowExt",NULL,NULL,NULL,&nTemp)==ERROR_SUCCESS)
									bShowExtension=FALSE;

								/*
								if (ShouldUpdateType() && (GetLocateDlg()->GetFlags()&(CLocateDlg::fgLVShowFileTypes|CLocateDlg::fgLVShowShellType))==CLocateDlg::fgLVShowFileTypes)
								{
									// Taking type now
									DWORD nLength=RegKey.QueryValueLength("");
									if (nLength>0)
									{
										WCHAR* pNewType=new WCHAR[nLength+1];
										if (RegKey.QueryValue(L"",pNewType,nLength))
										{
											WCHAR* pTmp=szType;
											InterlockedExchangePointer((VOID**)&szType,pNewType);
											dwFlags|=LITEM_TYPEOK;
											if (pTmp!=NULL)
												delete[] pTmp;
										}
										else
											delete[] pNewType;
									}
								}
								*/
								RegKey.CloseKey();
							}
						}
					}
				}
				if (bShowExtension)
				{
					pNewFileTitle=szName;
					break;
				}		
				// Continuing
			}
		case CLocateDlg::fgLVNeverShowExtensions:
			if (bExtensionPos==bNameLength)
				pNewFileTitle=szName;
			else
				pNewFileTitle=alloccopy(GetName(),DWORD(bExtensionPos)-1);
			break;
		}
	}

	if (!bCheckCase && GetLocateDlg()->GetFlags()&CLocateDlg::fgLV1stCharUpper)
	{
		BOOL bAllUpper=TRUE;
		DWORD i;
		for (i=0;pNewFileTitle[i]!='\0';i++)
		{
			if (!IsCharUpper(pNewFileTitle[i]))
			{
				bAllUpper=FALSE;
				break;
			}
		}
		if (bAllUpper)
		{
			if (pNewFileTitle==szName)
			{
				pNewFileTitle=new WCHAR[i+1];
				MemCopyW(pNewFileTitle,szName,i+1);
			}
			MakeLower(pNewFileTitle+1,i-1);
		}
	}

	ASSERT(pNewFileTitle!=NULL);

	WCHAR* pTmp=szFileTitle;
	InterlockedExchangePointer((PVOID*)&szFileTitle,pNewFileTitle);
	if (pTmp!=szName && pTmp!=NULL)
		delete[] pTmp;
	
	dwFlags|=LITEM_FILETITLEOK;

	ItemDebugMessage("CLocatedItem::UpdateFileTitle END");
}

void CLocatedItem::UpdateType() 
{
	ItemDebugMessage("CLocatedItem::UpdateType BEGIN");
	
	WCHAR* pNewType=NULL;
		
	if (IsSymlink())
	{
		if (IsFolder())
			pNewType=allocstringW(IDS_SYMLINKDIR);
		else
			pNewType=allocstringW(IDS_SYMLINK);
	}
	else if (IsJunkction())
		pNewType=allocstringW(IDS_JUNCTION);
	else if (!(GetLocateDlg()->GetFlags()&CLocateDlg::fgLVShowFileTypes))
	{
		
		// No extension
		if (IsFolder())
			pNewType=allocstringW(IDS_DIRECTORYTYPE);
		else
		{
			WCHAR buf[300];
			DWORD dwLength=LoadString(IDS_UNKNOWNTYPE,buf,300)+1;		
			pNewType=new WCHAR[GetExtensionLength()+dwLength+1];
			MemCopyW(pNewType,GetExtension(),GetExtensionLength());
			pNewType[GetExtensionLength()]=L' ';
			MemCopyW(pNewType+GetExtensionLength()+1,buf,dwLength);
		}
	}
	else if (GetLocateDlg()->GetFlags()&CLocateDlg::fgLVShowShellType)
	{
		// Using shell functions
		SHFILEINFOW fi;
		if (GetLocateDlg()->GetExtraFlags()&CLocateDlg::efEnableItemUpdating)
		{
			if (ShellFunctions::GetFileInfo(GetPath(),0,&fi,SHGFI_TYPENAME))
				pNewType=alloccopy(fi.szTypeName);
			else
			{
				if (!(IsFolder()?FileSystem::IsDirectory(GetPath()):FileSystem::IsFile(GetPath())))
				{
					// File does not exist
					SetToDeleted();
					ItemDebugMessage("CLocatedItem::UpdateType END2");
					return;
				}
			}
		}
		
		// Try without accessing file
		if (pNewType==NULL)
		{
			if (ShellFunctions::GetFileInfo(GetPath(),GetSystemAttributesFromAttributes(GetAttributes()),&fi,SHGFI_TYPENAME|SHGFI_USEFILEATTRIBUTES))
				pNewType=alloccopy(fi.szTypeName);
			else
			{
				if (IsFolder())
					pNewType=allocstringW(IDS_DIRECTORYTYPE);
				else
				{
					WCHAR buf[300];
					DWORD dwLength=LoadString(IDS_UNKNOWNTYPE,buf,300)+1;		
					pNewType=new WCHAR[GetExtensionLength()+dwLength+1];
					MemCopyW(pNewType,GetExtension(),GetExtensionLength());
					pNewType[GetExtensionLength()]=L' ';
					MemCopyW(pNewType+GetExtensionLength()+1,buf,dwLength);
				}
			}
		}

		dwFlags|=SHGFI_USEFILEATTRIBUTES;
	}
	else if (IsFolder())
	{
		if (GetLocateDlg()->GetExtraFlags()&CLocateDlg::efEnableItemUpdating)
		{
			if (!FileSystem::IsDirectory(GetPath()))
			{
				// Folder does not exist
				SetToDeleted();
				ItemDebugMessage("CLocatedItem::UpdateType END3");
				return;
			}
		}
		pNewType=allocstringW(IDS_DIRECTORYTYPE);
	}
	else
	{
		if (GetLocateDlg()->GetExtraFlags()&CLocateDlg::efEnableItemUpdating)
		{
			if (!FileSystem::IsFile(GetPath()))
			{
				SetToDeleted();
				ItemDebugMessage("CLocatedItem::UpdateType END4");
				return;
			}
		}
	
		CRegKey RegKey;
		CStringW Type;
		BOOL bOK=FALSE;

		if (RegKey.OpenKey(HKCR,szName+bExtensionPos-1,CRegKey::openExist|CRegKey::samRead|CRegKey::samQueryValue)==ERROR_SUCCESS)
		{
			RegKey.QueryValue(L"",Type);
			RegKey.CloseKey();
			
			if (!Type.IsEmpty())
			{
				if (RegKey.OpenKey(HKCR,Type,CRegKey::openExist|CRegKey::samRead|CRegKey::samQueryValue)==ERROR_SUCCESS)
				{
					// Taking type now
					DWORD nLength=RegKey.QueryValueLength("");
					ItemDebugFormatMessage1("CLocatedItem::UpdateType Type from registry, %d",nLength);
					if (nLength>0)
					{
						pNewType=new WCHAR[nLength+1];
						if (RegKey.QueryValue(L"",pNewType,nLength))
						{
							ItemDebugMessage("CLocatedItem::UpdateType Type from registry2");
							ItemDebugMessage(pNewType);
							
							bOK=TRUE;
						}
						else
							delete[] pNewType;
					}
					RegKey.CloseKey();
				}
			}
		}

		if (!bOK)
		{
			WCHAR szBuffer[300];
			DWORD dwTextLen=LoadString(IDS_UNKNOWNTYPE,szBuffer,300)+1;

			if (bExtensionPos!=bNameLength)
			{
				pNewType=new WCHAR[dwTextLen+GetExtensionLength()+1];
				MemCopyW(pNewType,GetExtension(),GetExtensionLength());
				MakeUpper(pNewType,GetExtensionLength());
				pNewType[GetExtensionLength()]=L' ';
				MemCopyW(pNewType+GetExtensionLength()+1,szBuffer,dwTextLen);
			}
			else // No extension
				pNewType=alloccopy(szBuffer,dwTextLen);
		}	
	}
	
	WCHAR* pTmp=szType;
	InterlockedExchangePointer((PVOID*)&szType,pNewType);
	if (pTmp!=NULL)
		delete[] pTmp;

	dwFlags|=LITEM_TYPEOK;

	ItemDebugMessage("CLocatedItem::UpdateType END");
}

void CLocatedItem::CheckIfDeleted()
{
	if (!(GetLocateDlg()->GetExtraFlags()&CLocateDlg::efEnableItemUpdating))
		return;
	

	if (IsDeleted())
		return;
	if (IsFolder())
	{
		if (!FileSystem::IsDirectory(GetPath()))
			SetToDeleted();
	}
	else
	{
		if (!FileSystem::IsFile(GetPath()))
			SetToDeleted();
	}

	dwFlags|=LITEM_ISDELETEDOK;
}

void CLocatedItem::UpdateIcon()
{
	ItemDebugMessage("CLocatedItem::UpdateIcon BEGIN");
	if (GetTrayIconWnd()->GetLocateDlg()->GetFlags()&CLocateDlg::fgLVShowIcons)
	{
		SHFILEINFOW fi;
		if (!(GetLocateDlg()->GetExtraFlags()&CLocateDlg::efEnableItemUpdating) ||
			GetLocateApp()->GetProgramFlags()&CLocateApp::pfAvoidToAccessWhenReadingIcons)
		{
			if (ShellFunctions::GetFileInfo(GetPath(),GetSystemAttributesFromAttributes(GetAttributes()),&fi,SHGFI_SYSICONINDEX|SHGFI_USEFILEATTRIBUTES))
				iIcon=fi.iIcon;
			else
				iIcon=GetLocateApp()->m_nDefImage;
		}		
		else
		{
			if (ShellFunctions::GetFileInfo(GetPath(),0,&fi,SHGFI_SYSICONINDEX))
				iIcon=fi.iIcon;
			else if (IsFolder()?FileSystem::IsDirectory(GetPath()):FileSystem::IsFile(GetPath()))
			{
				// Try again without accessing file
				if (ShellFunctions::GetFileInfo(GetPath(),GetSystemAttributesFromAttributes(GetAttributes()),&fi,SHGFI_SYSICONINDEX|SHGFI_USEFILEATTRIBUTES))
					iIcon=fi.iIcon;
				else
					iIcon=GetLocateApp()->m_nDefImage;
			}
			else
				iIcon=GetLocateApp()->m_nDelImage;
		}		

	}
	else if (IsDeleted())
		iIcon=GetLocateApp()->m_nDelImage;
	else if (IsFolder())
		iIcon=GetLocateApp()->m_nDirImage;
	else
		iIcon=GetLocateApp()->m_nDefImage;

	dwFlags|=LITEM_ICONOK;

	ItemDebugMessage("CLocatedItem::UpdateIcon END");
	
}



void CLocatedItem::UpdateParentIcon()
{
	ItemDebugMessage("CLocatedItem::UpdateParentIcon BEGIN");
	LPWSTR szParent=GetParent();
	if (GetTrayIconWnd()->GetLocateDlg()->GetFlags()&CLocateDlg::fgLVShowIcons)
	{
		LPWSTR pParent;
		if (szParent[1]==L':' && szParent[2]==L'\0')
		{
			pParent=new WCHAR[4];
			pParent[0]=szParent[0];
			pParent[1]=L':';
			pParent[2]=L'\\';
			pParent[3]=L'\0';
		}
		else
			pParent=szParent;
		

		SHFILEINFOW fi;
		if (!(GetLocateDlg()->GetExtraFlags()&CLocateDlg::efEnableItemUpdating) ||
			GetLocateApp()->GetProgramFlags()&CLocateApp::pfAvoidToAccessWhenReadingIcons)
		{
			if (ShellFunctions::GetFileInfo(pParent,FILE_ATTRIBUTE_DIRECTORY,&fi,SHGFI_SYSICONINDEX|SHGFI_USEFILEATTRIBUTES))
				iParentIcon=fi.iIcon;
			else
				iParentIcon=GetLocateApp()->m_nDirImage;
		}		
		else
		{
			if (ShellFunctions::GetFileInfo(pParent,0,&fi,SHGFI_SYSICONINDEX))
				iParentIcon=fi.iIcon;
			else if (FileSystem::IsDirectory(pParent))
			{
				// Try again without accessing file
				if (ShellFunctions::GetFileInfo(pParent,FILE_ATTRIBUTE_DIRECTORY,&fi,SHGFI_SYSICONINDEX|SHGFI_USEFILEATTRIBUTES))
					iParentIcon=fi.iIcon;
				else
					iParentIcon=GetLocateApp()->m_nDirImage;
			}
			else
				iParentIcon=GetLocateApp()->m_nDelImage;
		}		
		
		if (pParent!=szParent)
			delete[] pParent;
	}
	else if (IsDeleted())
		iParentIcon=GetLocateApp()->m_nDelImage;
	else
    	iParentIcon=GetLocateApp()->m_nDirImage;

	dwFlags|=LITEM_PARENTICONOK;

	ItemDebugMessage("CLocatedItem::UpdateParentIcon END");
}


void CLocatedItem::UpdateAttributes()
{
	if (!(GetLocateDlg()->GetExtraFlags()&CLocateDlg::efEnableItemUpdating))
		return;

	ItemDebugMessage("CLocatedItem::UpdateAttributes BEGIN");


	dwFlags|=LITEM_ATTRIBOK;
	if (IsDeleted())
		return;

	union {
		WIN32_FIND_DATAA fda;
		WIN32_FIND_DATAW fdw;
	};
	HANDLE hFind;
	if (IsUnicodeSystem())
		hFind=FindFirstFileW(GetPath(),&fdw);
	else
		hFind=FindFirstFileA(W2A(GetPath()),&fda);

	if (hFind==INVALID_HANDLE_VALUE)
	{
		SetToDeleted();
		return;
	}

	ASSERT(fda.dwFileAttributes==fdw.dwFileAttributes);
	ASSERT(fda.dwReserved0==fdw.dwReserved0);

	wAttribs=GetAttributesFromSystemAttributes(fda.dwFileAttributes)|(wAttribs&LITEMATTRIB_DIRECTORY);

	if (fda.dwFileAttributes&FILE_ATTRIBUTE_REPARSE_POINT)
	{
		switch (fda.dwReserved0)
		{
		case IO_REPARSE_TAG_MOUNT_POINT:
			wAttribs|=LITEMATTRIB_JUNCTION;
			break;
		case IO_REPARSE_TAG_SYMLINK:
			wAttribs|=LITEMATTRIB_SYMLINK;
			break;
		}		
	}

	FindClose(hFind);
	
	ItemDebugMessage("CLocatedItem::UpdateAttributes END");
}
void CLocatedItem::UpdateFileTime()
{
	if (!(GetLocateDlg()->GetExtraFlags()&CLocateDlg::efEnableItemUpdating))
		return;

	ItemDebugMessage("CLocatedItem::UpdateFileTime BEGIN");
	
	if (IsFolder())
	{
		union {
			WIN32_FIND_DATA fd;
			WIN32_FIND_DATAW fdw; // The beginning of the structures are equal
		};
		HANDLE hFind;
		if (IsUnicodeSystem())	
			hFind=FindFirstFileW(GetPath(),&fdw);
		else
			hFind=FindFirstFileA(W2A(GetPath()),&fd);
		DebugOpenHandle(dhtFileFind,hFind,GetPath());

		if (hFind==INVALID_HANDLE_VALUE)
			SetToDeleted();
		else
		{
			FindClose(hFind);
			DebugCloseHandle(dhtFileFind,hFind,GetPath());
			FILETIME ft2;
			
			FileTimeToLocalFileTime(&fd.ftLastWriteTime,&ft2);
			if (ft2.dwHighDateTime<27846560)// Date before 1.1.1980
			{
				wModifiedDate=-2;
				wModifiedTime=-2;
			}
			else
				FileTimeToDosDateTime(&ft2,&wModifiedDate,&wModifiedTime);
			
			FileTimeToLocalFileTime(&fd.ftCreationTime,&ft2);
			if (ft2.dwHighDateTime<27846560)// Date before 1.1.1980
			{
				wModifiedDate=-2;
				wModifiedTime=-2;
			}
			else
				FileTimeToDosDateTime(&ft2,&wCreatedDate,&wCreatedTime);
			
			FileTimeToLocalFileTime(&fd.ftLastAccessTime,&ft2);
			if (ft2.dwHighDateTime<27846560)// Date before 1.1.1980
			{
				wModifiedDate=-2;
				wModifiedTime=-2;
			}
			else
				FileTimeToDosDateTime(&ft2,&wAccessedDate,&wAccessedTime);
			
			if (wAccessedTime==0)
				wAccessedTime=WORD(-1);

			dwFlags|=LITEM_TIMEDATEOK|LITEM_FILESIZEOK;
		}

		ItemDebugMessage("CLocatedItem::UpdateFileTime END1");
		return;
	}

	HANDLE hFile;
	if (IsUnicodeSystem())
		hFile=CreateFileW(GetPath(),0,FILE_SHARE_READ|FILE_SHARE_WRITE,
			NULL,OPEN_EXISTING,0,NULL);
	else
		hFile=CreateFile(W2A(GetPath()),0,FILE_SHARE_READ|FILE_SHARE_WRITE,
			NULL,OPEN_EXISTING,0,NULL);
	DebugOpenHandle(dhtFile,hFile,GetPath());

	if (hFile==INVALID_HANDLE_VALUE)
	{
		if (GetLastError()==ERROR_SHARING_VIOLATION)
		{
			// Another method to query information
			union {
				WIN32_FIND_DATA fd;
				WIN32_FIND_DATAW fdw; // The beginning of the structures are equal
			};
			HANDLE hFind;
			if (IsUnicodeSystem())	
				hFind=FindFirstFileW(GetPath(),&fdw);
			else
				hFind=FindFirstFileA(W2A(GetPath()),&fd);
			DebugOpenHandle(dhtFileFind,hFind,GetPath());
			
			if (hFind!=INVALID_HANDLE_VALUE)
			{
				dwFileSize=fd.nFileSizeLow;
				wFileSizeHi=(BYTE)fd.nFileSizeHigh;


				FILETIME ft2;
				FileTimeToLocalFileTime(&fd.ftLastWriteTime,&ft2);
				FileTimeToDosDateTime(&ft2,&wModifiedDate,&wModifiedTime);
				FileTimeToLocalFileTime(&fd.ftCreationTime,&ft2);
				FileTimeToDosDateTime(&ft2,&wCreatedDate,&wCreatedTime);
				FileTimeToLocalFileTime(&fd.ftLastAccessTime,&ft2);
				FileTimeToDosDateTime(&ft2,&wAccessedDate,&wAccessedTime);
	
				dwFlags|=LITEM_TIMEDATEOK|LITEM_FILESIZEOK;

				FindClose(hFind);
				DebugCloseHandle(dhtFileFind,hFind,GetPath());
			
				ItemDebugMessage("CLocatedItem::UpdateFileTime END2");
				return;
			}
		}


		SetToDeleted();

		ItemDebugMessage("CLocatedItem::UpdateFileTime END3");
		return;
	}

	FILETIME ftCreated,ftModified,ftAccessed,ft2;
	GetFileTime(hFile,&ftCreated,&ftAccessed,&ftModified);
	FileTimeToLocalFileTime(&ftModified,&ft2);
	FileTimeToDosDateTime(&ft2,&wModifiedDate,&wModifiedTime);
	FileTimeToLocalFileTime(&ftCreated,&ft2);
	FileTimeToDosDateTime(&ft2,&wCreatedDate,&wCreatedTime);
	FileTimeToLocalFileTime(&ftAccessed,&ft2);
	FileTimeToDosDateTime(&ft2,&wAccessedDate,&wAccessedTime);
	
	if (wAccessedTime==0) // 0 wAccessedTime probably indicates that system does not save time
		wAccessedTime=WORD(-1);
	
	dwFlags|=LITEM_TIMEDATEOK;
	CloseHandle(hFile);
	DebugCloseHandle(dhtFile,hFile,GetPath());

	ItemDebugMessage("CLocatedItem::UpdateFileTime END");
}

void CLocatedItem::UpdateFileSize()
{
	if (!(GetLocateDlg()->GetExtraFlags()&CLocateDlg::efEnableItemUpdating))
		return;

	ItemDebugMessage("CLocatedItem::UpdateFileSize BEGIN");
	
	if (IsFolder())
	{
		dwFlags|=LITEM_FILESIZEOK;
		return;
	}

	HANDLE hFile;
	if (IsUnicodeSystem())
		hFile=CreateFileW(GetPath(),0,FILE_SHARE_READ|FILE_SHARE_WRITE,
			NULL,OPEN_EXISTING,0,NULL);
	else
		hFile=CreateFile(W2A(GetPath()),0,FILE_SHARE_READ|FILE_SHARE_WRITE,
			NULL,OPEN_EXISTING,0,NULL);
	DebugOpenHandle(dhtFile,hFile,GetPath());

	if (hFile==INVALID_HANDLE_VALUE)
	{
		if (GetLastError()==ERROR_SHARING_VIOLATION)
		{
			// Another method to query information
			union {
				WIN32_FIND_DATA fd;
				WIN32_FIND_DATAW fdw; // The beginning of the structures are equal
			};
			HANDLE hFind;
			if (IsUnicodeSystem())	
				hFind=FindFirstFileW(GetPath(),&fdw);
			else
				hFind=FindFirstFileA(W2A(GetPath()),&fd);
			DebugOpenHandle(dhtFileFind,hFind,GetPath());
			
			if (hFind!=INVALID_HANDLE_VALUE)
			{
				dwFileSize=fd.nFileSizeLow;
				wFileSizeHi=(BYTE)fd.nFileSizeHigh;


				FILETIME ft2;
				FileTimeToLocalFileTime(&fd.ftLastWriteTime,&ft2);
				FileTimeToDosDateTime(&ft2,&wModifiedDate,&wModifiedTime);
				FileTimeToLocalFileTime(&fd.ftCreationTime,&ft2);
				FileTimeToDosDateTime(&ft2,&wCreatedDate,&wCreatedTime);
				FileTimeToLocalFileTime(&fd.ftLastAccessTime,&ft2);
				FileTimeToDosDateTime(&ft2,&wAccessedDate,&wAccessedTime);
	
				dwFlags|=LITEM_TIMEDATEOK|LITEM_FILESIZEOK;

				FindClose(hFind);
				DebugCloseHandle(dhtFileFind,hFind,GetPath());
			
				ItemDebugMessage("CLocatedItem::UpdateFileSize END2");
				return;
			}
		}


		SetToDeleted();

		ItemDebugMessage("CLocatedItem::UpdateFileSize END3");
		return;
	}

	DWORD dwFileSizeHiTemp;
	dwFileSize=::GetFileSize(hFile,&dwFileSizeHiTemp);
	wFileSizeHi=static_cast<BYTE>(dwFileSizeHiTemp);

	dwFlags|=LITEM_FILESIZEOK;
	CloseHandle(hFile);
	DebugCloseHandle(dhtFile,hFile,GetPath());

	ItemDebugMessage("CLocatedItem::UpdateFileSize END");
}

void CLocatedItem::UpdateFileSizeAndTime()
{
	if (!(GetLocateDlg()->GetExtraFlags()&CLocateDlg::efEnableItemUpdating))
		return;

	ItemDebugMessage("CLocatedItem::UpdateFileSizeAndTime BEGIN");
	
	if (IsFolder())
	{
		union {
			WIN32_FIND_DATA fd;
			WIN32_FIND_DATAW fdw; // The beginning of the structures are equal
		};
		HANDLE hFind;
		if (IsUnicodeSystem())	
			hFind=FindFirstFileW(GetPath(),&fdw);
		else
			hFind=FindFirstFileA(W2A(GetPath()),&fd);
		DebugOpenHandle(dhtFileFind,hFind,GetPath());

		if (hFind==INVALID_HANDLE_VALUE)
			SetToDeleted();
		else
		{
			FindClose(hFind);
			DebugCloseHandle(dhtFileFind,hFind,GetPath());
			FILETIME ft2;
			
			
			FileTimeToLocalFileTime(&fd.ftLastWriteTime,&ft2);
			FileTimeToDosDateTime(&ft2,&wModifiedDate,&wModifiedTime);
			FileTimeToLocalFileTime(&fd.ftCreationTime,&ft2);
			FileTimeToDosDateTime(&ft2,&wCreatedDate,&wCreatedTime);
			FileTimeToLocalFileTime(&fd.ftLastAccessTime,&ft2);
			FileTimeToDosDateTime(&ft2,&wAccessedDate,&wAccessedTime);
			
			if (wAccessedTime==0)
				wAccessedTime=WORD(-1);

			dwFlags|=LITEM_TIMEDATEOK|LITEM_FILESIZEOK;
		}

		ItemDebugMessage("CLocatedItem::UpdateFileSizeAndTime END1");
		return;
	}

	HANDLE hFile;
	if (IsUnicodeSystem())
		hFile=CreateFileW(GetPath(),0,FILE_SHARE_READ|FILE_SHARE_WRITE,
			NULL,OPEN_EXISTING,0,NULL);
	else
		hFile=CreateFile(W2A(GetPath()),0,FILE_SHARE_READ|FILE_SHARE_WRITE,
			NULL,OPEN_EXISTING,0,NULL);
	DebugOpenHandle(dhtFile,hFile,GetPath());

	if (hFile==INVALID_HANDLE_VALUE)
	{
		if (GetLastError()==ERROR_SHARING_VIOLATION)
		{
			// Another method to query information
			union {
				WIN32_FIND_DATA fd;
				WIN32_FIND_DATAW fdw; // The beginning of the structures are equal
			};
			HANDLE hFind;
			if (IsUnicodeSystem())	
				hFind=FindFirstFileW(GetPath(),&fdw);
			else
				hFind=FindFirstFileA(W2A(GetPath()),&fd);
			DebugOpenHandle(dhtFileFind,hFind,GetPath());
			
			if (hFind!=INVALID_HANDLE_VALUE)
			{
				dwFileSize=fd.nFileSizeLow;
				wFileSizeHi=(BYTE)fd.nFileSizeHigh;


				FILETIME ft2;
				FileTimeToLocalFileTime(&fd.ftLastWriteTime,&ft2);
				FileTimeToDosDateTime(&ft2,&wModifiedDate,&wModifiedTime);
				FileTimeToLocalFileTime(&fd.ftCreationTime,&ft2);
				FileTimeToDosDateTime(&ft2,&wCreatedDate,&wCreatedTime);
				FileTimeToLocalFileTime(&fd.ftLastAccessTime,&ft2);
				FileTimeToDosDateTime(&ft2,&wAccessedDate,&wAccessedTime);
	
				dwFlags|=LITEM_TIMEDATEOK|LITEM_FILESIZEOK;

				FindClose(hFind);
				DebugCloseHandle(dhtFileFind,hFind,GetPath());
			
				ItemDebugMessage("CLocatedItem::UpdateFileSizeAndTime END2");
				return;
			}
		}


		SetToDeleted();

		ItemDebugMessage("CLocatedItem::UpdateFileSizeAndTime END3");
		return;
	}

	DWORD dwFileSizeHiTemp;
	dwFileSize=::GetFileSize(hFile,&dwFileSizeHiTemp);
	wFileSizeHi=static_cast<BYTE>(dwFileSizeHiTemp);

	FILETIME ftCreated,ftModified,ftAccessed,ft2;
	GetFileTime(hFile,&ftCreated,&ftAccessed,&ftModified);
	FileTimeToLocalFileTime(&ftModified,&ft2);
	FileTimeToDosDateTime(&ft2,&wModifiedDate,&wModifiedTime);
	FileTimeToLocalFileTime(&ftCreated,&ft2);
	FileTimeToDosDateTime(&ft2,&wCreatedDate,&wCreatedTime);
	FileTimeToLocalFileTime(&ftAccessed,&ft2);
	FileTimeToDosDateTime(&ft2,&wAccessedDate,&wAccessedTime);
	
	if (wAccessedTime==0) // 0 wAccessedTime probably indicates that system does not save time
		wAccessedTime=WORD(-1);
	
	dwFlags|=LITEM_TIMEDATEOK|LITEM_FILESIZEOK;
	CloseHandle(hFile);
	DebugCloseHandle(dhtFile,hFile,GetPath());

	ItemDebugMessage("CLocatedItem::UpdateFileSizeAndTime END");
}

void CLocatedItem::UpdateDimensions()
{
	if (!(GetLocateDlg()->GetExtraFlags()&CLocateDlg::efEnableItemUpdating))
		return;

	ItemDebugMessage("CLocatedItem::UpdateDimensions BEGIN");
	

	ExtraInfo* pField=CreateExtraInfoField(ImageDimensions);
	pField->bShouldUpdate=FALSE;

	if (GetLocateDlg()->m_pImageHandler==NULL)
		return;

	SIZE dim;
	if (!GetLocateDlg()->m_pImageHandler->pGetImageDimensionsW(GetPath(),&dim))
	{
		pField->szImageDimension.cx=0;
		pField->szImageDimension.cy=0;
	}
	else
		pField->szImageDimension=dim;

	ItemDebugMessage("CLocatedItem::UpdateDimensions END");
}

void CLocatedItem::ComputeMD5sum(BOOL bForce,BOOL bOnlyIfNotComputedYet)
{
	if (!(GetLocateDlg()->GetExtraFlags()&CLocateDlg::efEnableItemUpdating))
		return;

	ItemDebugMessage("CLocatedItem::ComputeMD5sum BEGIN");

	ExtraInfo* pField=CreateExtraInfoField(MD5sum);
	
	if (bOnlyIfNotComputedYet && pField->szText!=NULL && !pField->bShouldUpdate)
		return;
	
	pField->bShouldUpdate=FALSE;

	if (!bForce && !(GetLocateDlg()->GetFlags()&CLocateDlg::fgLVComputeMD5Sums))
		return; 

	
	if (pField->szText!=NULL)
	{
		WCHAR* pTmp=pField->szText;
		InterlockedExchangePointer((PVOID*)&pField->szText,NULL);
		delete[] pTmp;
	}

	if (IsFolder() || IsDeleted())
		return; // No need



	// Computing MD5 sum
	md5_state_t state;
	md5_byte_t digest[16];
	md5_init(&state);

	BOOL bOK=TRUE;
	CFile* pFile=NULL;
	try {
		pFile=new CFile(GetPath(),CFile::defRead,TRUE);
		pFile->CloseOnDelete();
	
		md5_byte_t* pData=new md5_byte_t[1024];
	
		for (;;)
		{	
			DWORD dwRead=pFile->Read(pData,1024*sizeof(md5_byte_t));
			if (dwRead==0)
				break;
			md5_append(&state, pData, dwRead);
		}
        
		delete[] pData;		
		pFile->Close();
	}
	catch (...)
	{
		bOK=FALSE;
	}

	if (pFile!=NULL)
		delete pFile;

    if (bOK)
	{
		md5_finish(&state, digest);

		WCHAR* pNewText=new WCHAR[2*16+1];

		for (int i=0;i<16;i++)
		{
			BYTE bHi=BYTE(digest[i]>>4)&0xF;
			BYTE bLo=BYTE(digest[i]&0xF);

			pNewText[i*2]=bHi>=10?bHi-10+L'a':bHi+L'0';
			pNewText[i*2+1]=bLo>=10?bLo-10+L'a':bLo+L'0';
		}
		pNewText[16*2]=L'\0';

		InterlockedExchangePointer((PVOID*)&pField->szText,pNewText);
	}

	ItemDebugMessage("CLocatedItem::ComputeMD5sum END");
}

void CLocatedItem::UpdateOwner()
{
	if (!(GetLocateDlg()->GetExtraFlags()&CLocateDlg::efEnableItemUpdating))
		return;
	
	ItemDebugMessage("CLocatedItem::UpdateOwner BEGIN");
	
	ExtraInfo* pField=CreateExtraInfoField(Owner);
	pField->bShouldUpdate=FALSE;

	
	if (pField->szText!=NULL)
	{
		WCHAR* pTmp=pField->szText;
		InterlockedExchangePointer((PVOID*)&pField->szText,NULL);
		delete[] pTmp;
	}
		
	if (IsDeleted())
	{
		ItemDebugMessage("CLocatedItem::UpdateOwner END1");
		return;
	}

    	
	DWORD dwNeeded=0;
	if (!FileSystem::GetFileSecurity(GetPath(),OWNER_SECURITY_INFORMATION,NULL,0,&dwNeeded))
	{
		if (GetLastError()!=ERROR_INSUFFICIENT_BUFFER)
		{
			ItemDebugMessage("CLocatedItem::UpdateOwner END2");
			return;
		}
	}

    PSECURITY_DESCRIPTOR pDesc=(PSECURITY_DESCRIPTOR)new BYTE[dwNeeded+2];

    if (!FileSystem::GetFileSecurity(szPath,OWNER_SECURITY_INFORMATION,pDesc,dwNeeded+2,&dwNeeded))
	{
		delete[] (BYTE*) pDesc;
		ItemDebugMessage("CLocatedItem::UpdateOwner END3");
			
		return;
	}
	
	PSID psid;
	BOOL bDefaulted=TRUE;
	if (!GetSecurityDescriptorOwner(pDesc,&psid,&bDefaulted))
	{
		delete[] (BYTE*) pDesc;
		ItemDebugMessage("CLocatedItem::UpdateOwner END4");
			
		return;
	}
	

	WCHAR szOwner[200],szDomain[200];
	DWORD dwOwnerLen=199,dwDomainLen=199;
	SID_NAME_USE sUse;
	
	BOOL bRet;
	if (szPath[0]==L'\\' && szPath[1]==L'\\')
	{
		DWORD dwLength=2+(DWORD)FirstCharIndex(szPath+2,L'\\');
		WCHAR* szServer=new WCHAR[dwLength+1];
		MemCopyW(szServer,szPath,dwLength);
		szServer[dwLength]='\0';
		bRet=FileSystem::LookupAccountSid(szServer,psid,szOwner,&dwOwnerLen,szDomain,&dwDomainLen,&sUse);
		delete[] szServer;
	}
	else
		bRet=FileSystem::LookupAccountSid(NULL,psid,szOwner,&dwOwnerLen,szDomain,&dwDomainLen,&sUse);

	if (bRet)
	{
		WCHAR* pNewText=new WCHAR[dwOwnerLen+dwDomainLen+2];
		if (dwDomainLen>0)
		{
			MemCopyW(pNewText,szDomain,dwDomainLen);
			pNewText[dwDomainLen]=L'\\';
			MemCopyW(pNewText+dwDomainLen+1,szOwner,dwOwnerLen+1);                
		}
		else
			MemCopyW(pNewText,szOwner,dwOwnerLen+1);
		
		InterlockedExchangePointer((PVOID*)&pField->szText,pNewText);
	}
	delete[] (BYTE*) pDesc;

	ItemDebugMessage("CLocatedItem::UpdateOwner END");
}

void CLocatedItem::UpdateShortFileName()
{
	if (!(GetLocateDlg()->GetExtraFlags()&CLocateDlg::efEnableItemUpdating))
		return;

	ItemDebugFormatMessage4("CLocatedItem::UpdateShortFileName BEGIN %s",GetPath(),0,0,0);
	
	ExtraInfo* pField=CreateExtraInfoField(ShortFileName);
	pField->bShouldUpdate=FALSE;

	if (pField->szText!=NULL)
	{
		WCHAR* pTmp=pField->szText;
		InterlockedExchangePointer((PVOID*)&pField->szText,NULL);
		delete[] pTmp;
	}
	
	if (IsDeleted())
	{
		ItemDebugMessage("CLocatedItem::UpdateShortFileName END2");
		return;
	}

	

	WCHAR szShortPath[MAX_PATH];
	DWORD nLength=FileSystem::GetShortPathName(GetPath(),szShortPath,MAX_PATH);
	if (nLength==0)
	{
		//pField->szText=allocemptyW();
		ItemDebugMessage("CLocatedItem::UpdateShortFileName END2");
		return;
	}

	int nStart;
	for (nStart=nLength-1;nStart>=0 && szShortPath[nStart]!='\\';nStart--);
	if (nStart>0)
		nStart++;
    nLength-=nStart;

	InterlockedExchangePointer((PVOID*)&pField->szText,alloccopy(szShortPath+nStart,nLength));

	ItemDebugMessage("CLocatedItem::UpdateShortFileName END");
}

void CLocatedItem::UpdateShortFilePath()
{
	if (!(GetLocateDlg()->GetExtraFlags()&CLocateDlg::efEnableItemUpdating))
		return;

	ItemDebugMessage("CLocatedItem::UpdateShortFilePath BEGIN");
	
	ExtraInfo* pField=CreateExtraInfoField(ShortFilePath);
	pField->bShouldUpdate=FALSE;
	
	if (pField->szText!=NULL)
	{
		WCHAR* pTmp=pField->szText;
		InterlockedExchangePointer((PVOID*)&pField->szText,NULL);
		delete[] pTmp;
	}

	if (IsDeleted())
	{
		ItemDebugMessage("CLocatedItem::UpdateShortFilePath END1");
		return;
	}

	WCHAR szShortPath[MAX_PATH];
	DWORD nLength=FileSystem::GetShortPathName(GetPath(),szShortPath,MAX_PATH);
	if (nLength==0)
	{
		ItemDebugMessage("CLocatedItem::UpdateShortFilePath END2");
		return;
	}
	
	InterlockedExchangePointer((PVOID*)&pField->szText,alloccopy(szShortPath,nLength));

	ItemDebugMessage("CLocatedItem::UpdateShortFilePath END");
}

void CLocatedItem::UpdateSummaryProperties()
{
	if (!(GetLocateDlg()->GetExtraFlags()&CLocateDlg::efEnableItemUpdating))
		return;
	
	ItemDebugMessage("CLocatedItem::UpdateSummaryProperties BEGIN");
	
	ExtraInfo* pFields[6];
	pFields[0]=CreateExtraInfoField(Author);
	pFields[1]=CreateExtraInfoField(Title);
	pFields[2]=CreateExtraInfoField(Subject);
	pFields[3]=CreateExtraInfoField(Keywords);
	pFields[4]=CreateExtraInfoField(Comments);
	pFields[5]=CreateExtraInfoField(Pages);
	

	for (int i=0;i<5;i++)
	{
		pFields[i]->bShouldUpdate=FALSE;
		if (pFields[i]->szText!=NULL)
		{
			WCHAR* pTmp=pFields[i]->szText;
			InterlockedExchangePointer((PVOID*)&pFields[i]->szText,NULL);
			delete[] pTmp;
		}
	}	
	pFields[5]->bShouldUpdate=FALSE;
	pFields[5]->nPages=0;

	if (IsDeleted())
		return;

	// Fisrt, check if there is descript.ion file and read comment from the file

	// Read file
	CFile File(TRUE);
	BYTE* pContent=NULL;
	DWORD dwFileSize;
	File.CloseOnDelete();
	try {
		// Read file
		File.Open((CStringW(GetParent())+L"\\descript.ion"),CFile::defRead);
		dwFileSize=File.GetLength();
		pContent=new BYTE[dwFileSize+2];
		File.Read(pContent,dwFileSize);
		*((WORD*)(pContent+dwFileSize))=0;
		File.Close();
	}
	catch (...)
	{
		if (pContent!=NULL)
			delete[] pContent;
		pContent=NULL;
	}


	// Parse file		
	if (pContent!=NULL && dwFileSize>=4)
	{
		LPCWSTR pPtr;
		
		// Check encoding
		if (pContent[1]=='\0' || (pContent[0]==0xff && pContent[1]==0xfe)) 
		{
			// This is UTF16
			pPtr=(LPCWSTR)pContent;
		}
		else
		{
			// UTF8 or ANSI
			UINT cp=(pContent[0]==0xef && pContent[1]==0xbb && pContent[2]==0xbf)?CP_UTF8:CP_ACP;
			int nDataLen=MultiByteToWideChar(cp,0,(char*)pContent,dwFileSize,NULL,0);
			LPWSTR pNewData=(LPWSTR)new char[(nDataLen+2)*sizeof(WCHAR)];
			MultiByteToWideChar(cp,0,(char*)pContent,dwFileSize,pNewData,nDataLen);
			pNewData[nDataLen]=L'\0';
			delete[] pContent;
			pContent=(BYTE*)pNewData;		
			pPtr=pNewData;
		}

		if (pPtr[0]==0xff && pPtr[1]==0xfe)
			pPtr+=2;

		for (;;)
		{
			for (;*pPtr==L' ' || *pPtr==L'\t' || *pPtr==L'\r' || *pPtr==L'\n';pPtr++)
				pPtr++;
			if (*pPtr==L'\0')
				break;

			BOOL bFileFound;
			if (*pPtr==L'\"')
			{
				// Long file name 
				pPtr++;
				int nFileNameLen;
				for (nFileNameLen=0;pPtr[nFileNameLen]!=L'\0' && pPtr[nFileNameLen]!=L'\"';nFileNameLen++);

				if (pPtr[nFileNameLen]==L'\0')
					break;
				
				bFileFound=nFileNameLen==GetNameLen()?_wcsnicmp(pPtr,GetName(),nFileNameLen)==0:FALSE;
				pPtr+=nFileNameLen+1;
			} 
			else
			{
				// Short file name
				int nFileNameLen;
				for (nFileNameLen=0;pPtr[nFileNameLen]!=L'\0' && pPtr[nFileNameLen]!=L' ';nFileNameLen++);

				if (pPtr[nFileNameLen]==L'\0')
					break;
				
				bFileFound=nFileNameLen==GetNameLen()?_wcsnicmp(pPtr,GetName(),nFileNameLen)==0:FALSE;
				pPtr+=nFileNameLen;
			}

			// Scan to end of line
			int nCommentLen;
			if (*pPtr==L' ') pPtr++;
			for (nCommentLen=0;pPtr[nCommentLen]!=L'\0' && pPtr[nCommentLen]!=L'\r' && pPtr[nCommentLen]!=L'\n';nCommentLen++);

			if (bFileFound)
			{
				// Insert comment
				if (nCommentLen>2 && pPtr[nCommentLen-2]==0x04 && pPtr[nCommentLen-1]==0xc2)
					nCommentLen-=2;
				InterlockedExchangePointer((PVOID*)&pFields[4]->szText,alloccopy(pPtr,nCommentLen));
				break;
			}

			pPtr+=nCommentLen;			
		}

		delete[] pContent;
	}
	

	IPropertySetStorage* ppss;
	IPropertyStorage* pps;
	
	STGOPTIONS stgo;
	stgo.usVersion=STGOPTIONS_VERSION;
	

	HRESULT(STDAPICALLTYPE* pStgOpenStorageEx)(const WCHAR*,DWORD,DWORD,DWORD,STGOPTIONS*,
		  PSECURITY_DESCRIPTOR,REFIID,void**);
	pStgOpenStorageEx=(HRESULT(STDAPICALLTYPE *)(const WCHAR*,DWORD,DWORD,DWORD,STGOPTIONS*,
		  PSECURITY_DESCRIPTOR,REFIID,void**))GetProcAddress(GetModuleHandle("Ole32.dll"),"StgOpenStorageEx");
	if (pStgOpenStorageEx==NULL)
		return;

	HRESULT hRes=pStgOpenStorageEx(GetPath(),STGM_READ|STGM_SHARE_EXCLUSIVE,STGFMT_ANY,0,NULL,0,
			IID_IPropertySetStorage,(void**)&ppss);

	if (!SUCCEEDED(hRes))
		return;
	
	hRes=ppss->Open(FMTID_SummaryInformation,STGM_READ|STGM_SHARE_EXCLUSIVE,&pps);
	if (!SUCCEEDED(hRes))
	{
		ppss->Release();
		return;
	}

	
	PROPSPEC rgpspec[6];
	PROPVARIANT rgpropvar[6];
	
	for (int i=0;i<6;i++)
	{
		rgpspec[i].ulKind=PRSPEC_PROPID;
		PropVariantInit(&rgpropvar[i]);
	}

	rgpspec[0].propid=PIDSI_AUTHOR;
	rgpspec[1].propid=PIDSI_TITLE;
	rgpspec[2].propid=PIDSI_SUBJECT;
	rgpspec[3].propid=PIDSI_KEYWORDS;
	rgpspec[4].propid=PIDSI_COMMENT;
	rgpspec[5].propid=PIDSI_PAGECOUNT;
	
	hRes=pps->ReadMultiple(6,rgpspec,rgpropvar);
	if (SUCCEEDED(hRes))
	{
		for (int i=0;i<4;i++) // Last two are comment and number of pages, handling those separately
		{
			switch (rgpropvar[i].vt)
			{
			case VT_LPSTR:
				InterlockedExchangePointer((PVOID*)&pFields[i]->szText,
					alloccopyAtoW(rgpropvar[i].pszVal));
				break;
			case VT_LPWSTR:
				InterlockedExchangePointer((PVOID*)&pFields[i]->szText,
					alloccopy(rgpropvar[i].pwszVal));
				break;
			}
			PropVariantClear(&rgpropvar[i]);
		}

		// Handle comment
		LPWSTR pComment=NULL;
		switch (rgpropvar[4].vt)
		{
		case VT_LPSTR:
			pComment=alloccopyAtoW(rgpropvar[4].pszVal);
			break;
		case VT_LPWSTR:
			pComment=alloccopy(rgpropvar[4].pwszVal);
			break;
		}
		PropVariantClear(&rgpropvar[4]);
		
		if (pComment!=NULL)
		{
			if (pFields[4]->szText!=NULL)
			{
				// Combine both comments
				LPWSTR pOldComment=pFields[4]->szText;
				InterlockedExchangePointer((PVOID*)&pFields[4]->szText,NULL);

				int nComment1Len=istrlen(pComment);
				int nComment2Len=istrlen(pOldComment);
				

				LPWSTR pNewComment=new WCHAR[nComment1Len+nComment2Len+4];
				MemCopyW(pNewComment,pComment,nComment1Len);
				MemCopyW(pNewComment+nComment1Len,L" | ",3);
				MemCopyW(pNewComment+nComment1Len+3,pOldComment,nComment2Len+1);
				
				delete[] pComment;
				delete[] pOldComment;

				InterlockedExchangePointer((PVOID*)&pFields[4]->szText,pNewComment);
			}
			else
				InterlockedExchangePointer((PVOID*)&pFields[4]->szText,pComment);
		}


		// Handle pages			
		switch (rgpropvar[5].vt)
		{
		case VT_I2:
			pFields[5]->nPages=rgpropvar[5].iVal;
			break;
		case VT_I4:
		case VT_INT:
			pFields[5]->nPages=rgpropvar[5].lVal;
			break;
		case VT_I8:
			pFields[5]->nPages=(INT)rgpropvar[5].hVal.LowPart;
			break;
		case VT_UI2:
			pFields[5]->nPages=rgpropvar[5].uiVal;
			break;
		case VT_UI4:
		case VT_UINT:
			pFields[5]->nPages=rgpropvar[5].ulVal;
			break;
		case VT_UI8:
			pFields[5]->nPages=(DWORD)rgpropvar[5].uhVal.LowPart;
			break;
		}
		PropVariantClear(&rgpropvar[5]);
		
	}


	pps->Release();
	ppss->Release();
				

}


void CLocatedItem::UpdateDocSummaryProperties()
{
	if (!(GetLocateDlg()->GetExtraFlags()&CLocateDlg::efEnableItemUpdating))
		return;
	
	ItemDebugMessage("CLocatedItem::UpdateDocSummaryProperties BEGIN");
	
	ExtraInfo* pField=CreateExtraInfoField(Category);
	pField->bShouldUpdate=FALSE;

	if (pField->szText!=NULL)
	{
		WCHAR* pTmp=pField->szText;
		InterlockedExchangePointer((PVOID*)&pField->szText,NULL);
		delete[] pTmp;
	}
			
	if (IsDeleted())
		return;

	IPropertySetStorage* ppss;
	IPropertyStorage* pps;
	
	STGOPTIONS stgo;
	stgo.usVersion=STGOPTIONS_VERSION;
	
	HRESULT(STDAPICALLTYPE* pStgOpenStorageEx)(const WCHAR*,DWORD,DWORD,DWORD,STGOPTIONS*,
		  PSECURITY_DESCRIPTOR,REFIID,void**);
	pStgOpenStorageEx=(HRESULT(STDAPICALLTYPE *)(const WCHAR*,DWORD,DWORD,DWORD,STGOPTIONS*,
		  PSECURITY_DESCRIPTOR,REFIID,void**))GetProcAddress(GetModuleHandle("Ole32.dll"),"StgOpenStorageEx");
	if (pStgOpenStorageEx==NULL)
		return;

	HRESULT hRes=pStgOpenStorageEx(GetPath(),STGM_READ|STGM_SHARE_EXCLUSIVE,STGFMT_ANY,0,NULL,0,
		IID_IPropertySetStorage,(void**)&ppss);
	if (!SUCCEEDED(hRes))
		return;
	
	hRes=ppss->Open(FMTID_DocSummaryInformation,STGM_READ|STGM_SHARE_EXCLUSIVE,&pps);
	if (!SUCCEEDED(hRes))
	{
		ppss->Release();
		return;
	}

	
	PROPSPEC rgpspec[1];
	PROPVARIANT rgpropvar[1];
	
	PropVariantInit(&rgpropvar[0]);
	rgpspec[0].ulKind=PRSPEC_PROPID;
	rgpspec[0].propid=PIDDSI_CATEGORY;

	hRes=pps->ReadMultiple(1,rgpspec,rgpropvar);
	if (SUCCEEDED(hRes))
	{
		switch (rgpropvar[0].vt)
		{
		case VT_LPSTR:
			InterlockedExchangePointer((PVOID*)&pField->szText,
				alloccopyAtoW(rgpropvar[0].pszVal));
			break;
		case VT_LPWSTR:
			InterlockedExchangePointer((PVOID*)&pField->szText,
				alloccopy(rgpropvar[0].pwszVal));
			break;
		}
		PropVariantClear(&rgpropvar[0]);
	}


	pps->Release();
	ppss->Release();
}

void CLocatedItem::UpdateVersionInformation()
{
	if (!(GetLocateDlg()->GetExtraFlags()&CLocateDlg::efEnableItemUpdating))
		return;
	
	ItemDebugMessage("CLocatedItem::UpdateVersionInformation BEGIN");
	
	ExtraInfo* pFields[4];
	pFields[0]=CreateExtraInfoField(Description);
	pFields[1]=CreateExtraInfoField(FileVersion);
	pFields[2]=CreateExtraInfoField(ProductName);
	pFields[3]=CreateExtraInfoField(ProductVersion);
	
	for (int i=0;i<4;i++)
	{
		pFields[i]->bShouldUpdate=FALSE;
		if (pFields[i]->szText!=NULL)
		{
			WCHAR* pTmp=pFields[i]->szText;
			InterlockedExchangePointer((PVOID*)&pFields[i]->szText,NULL);
			delete[] pTmp;
		}
	}	

	if (IsDeleted())
		return;

	if (IsUnicodeSystem())
	{
		UINT iDataLength=GetFileVersionInfoSizeW(GetPath(),NULL);
		if (iDataLength>0)
		{
			VOID *pTranslations;
			DWORD DefTranslations[2]={0x0490,0x04b0};
			
			LPWSTR pProductVersion=NULL,pProductName=NULL;
			LPWSTR pFileVersion=NULL,pDescription=NULL;
			
			BYTE* pData=new BYTE[iDataLength];
			GetFileVersionInfoW(GetPath(),NULL,iDataLength,pData);
		
			if (!VerQueryValueW(pData,L"VarFileInfo\\Translation",&pTranslations,&iDataLength))
				pTranslations=DefTranslations;

			WCHAR szTranslation[100];
			
			StringCbPrintfW(szTranslation,100*sizeof(WCHAR),L"\\StringFileInfo\\%04X%04X\\ProductVersion",LPWORD(pTranslations)[0],LPWORD(pTranslations)[1]);
			if (!VerQueryValueW(pData,szTranslation,(void**)&pProductVersion,&iDataLength))
				VerQueryValueW(pData,L"\\StringFileInfo\\040904b0\\ProductVersion",(void**)&pProductVersion,&iDataLength);

			StringCbPrintfW(szTranslation,100*sizeof(WCHAR),L"\\StringFileInfo\\%04X%04X\\FileVersion",LPWORD(pTranslations)[0],LPWORD(pTranslations)[1]);
			if (!VerQueryValueW(pData,szTranslation,(void**)&pFileVersion,&iDataLength))
				VerQueryValueW(pData,L"\\StringFileInfo\\040904b0\\FileVersion",(void**)&pFileVersion,&iDataLength);

			StringCbPrintfW(szTranslation,100*sizeof(WCHAR),L"\\StringFileInfo\\%04X%04X\\FileDescription",LPWORD(pTranslations)[0],LPWORD(pTranslations)[1]);
			if (!VerQueryValueW(pData,szTranslation,(void**)&pDescription,&iDataLength))
				VerQueryValueW(pData,L"\\StringFileInfo\\040904b0\\FileDescription",(void**)&pDescription,&iDataLength);

			StringCbPrintfW(szTranslation,100*sizeof(WCHAR),L"\\StringFileInfo\\%04X%04X\\ProductName",LPWORD(pTranslations)[0],LPWORD(pTranslations)[1]);
			if (!VerQueryValueW(pData,szTranslation,(void**)&pProductName,&iDataLength))
				VerQueryValueW(pData,L"\\StringFileInfo\\040904b0\\ProductName",(void**)&pProductName,&iDataLength);

			if (pDescription!=NULL)
				pFields[0]->szText=alloccopy(pDescription);
			if (pFileVersion!=NULL)
				pFields[1]->szText=alloccopy(pFileVersion);
			if (pProductName!=NULL)
				pFields[2]->szText=alloccopy(pProductName);
			if (pProductVersion!=NULL)
				pFields[3]->szText=alloccopy(pProductVersion);
			
			delete[] pData;
		}
	}
	else
	{
		UINT iDataLength=GetFileVersionInfoSize(W2A(GetPath()),NULL);
		if (iDataLength>0)
		{
			VOID *pTranslations;
			DWORD DefTranslations[2]={0x0490,0x04b0};
			
			LPSTR pProductVersion=NULL,pProductName=NULL;
			LPSTR pFileVersion=NULL,pDescription=NULL;
			
			BYTE* pData=new BYTE[iDataLength];
			GetFileVersionInfo(W2A(GetPath()),NULL,iDataLength,pData);
		
			if (!VerQueryValue(pData,"VarFileInfo\\Translation",&pTranslations,&iDataLength))
				pTranslations=DefTranslations;

			CHAR szTranslation[100];
			
			StringCbPrintf(szTranslation,100,"\\StringFileInfo\\%04X%04X\\ProductVersion",LPWORD(pTranslations)[0],LPWORD(pTranslations)[1]);
			if (!VerQueryValue(pData,szTranslation,(void**)&pProductVersion,&iDataLength))
				VerQueryValue(pData,"\\StringFileInfo\\040904b0\\ProductVersion",(void**)&pProductVersion,&iDataLength);

			StringCbPrintf(szTranslation,100,"\\StringFileInfo\\%04X%04X\\FileVersion",LPWORD(pTranslations)[0],LPWORD(pTranslations)[1]);
			if (!VerQueryValue(pData,szTranslation,(void**)&pFileVersion,&iDataLength))
				VerQueryValue(pData,"\\StringFileInfo\\040904b0\\FileVersion",(void**)&pFileVersion,&iDataLength);

			StringCbPrintf(szTranslation,100,"\\StringFileInfo\\%04X%04X\\FileDescription",LPWORD(pTranslations)[0],LPWORD(pTranslations)[1]);
			if (!VerQueryValue(pData,szTranslation,(void**)&pDescription,&iDataLength))
				VerQueryValue(pData,"\\StringFileInfo\\040904b0\\FileDescription",(void**)&pDescription,&iDataLength);

			StringCbPrintf(szTranslation,100,"\\StringFileInfo\\%04X%04X\\ProductName",LPWORD(pTranslations)[0],LPWORD(pTranslations)[1]);
			if (!VerQueryValue(pData,szTranslation,(void**)&pProductName,&iDataLength))
				VerQueryValue(pData,"\\StringFileInfo\\040904b0\\ProductName",(void**)&pProductName,&iDataLength);

			if (pDescription!=NULL)
				InterlockedExchangePointer((PVOID*)&pFields[0]->szText,alloccopyAtoW(pDescription));
			if (pFileVersion!=NULL)
				InterlockedExchangePointer((PVOID*)&pFields[1]->szText,alloccopyAtoW(pFileVersion));
			if (pProductName!=NULL)
				InterlockedExchangePointer((PVOID*)&pFields[2]->szText,alloccopyAtoW(pProductName));
			if (pProductVersion!=NULL)
				InterlockedExchangePointer((PVOID*)&pFields[3]->szText,alloccopyAtoW(pProductVersion));
			
			delete[] pData;
		}
	}
}



void CLocatedItem::LoadThumbnail()
{
	ItemDebugFormatMessage1("CLocatedItem::LoadThumbnail for %S",GetPath());

	ExtraInfo* pField;
	pField=CreateExtraInfoField(Thumbnail);
	pField->bShouldUpdate=FALSE;
		
	// LoadThumbnail already called?
	if (pField->pThumbnail!=NULL)
		return;

	pField->pThumbnail=new ExtraInfo::ThumbnailData(CreateMutex(NULL,TRUE,NULL));
	
	

	// No thumbnails for icons and cursors
	if (_wcsicmp(GetExtension(),L"ico")==0 || _wcsicmp(GetExtension(),L"cur")==0)
	{
		pField->pThumbnail->CloseMutex();
		return;
	}
	
	CLocateDlg* pLocateDlg=GetLocateDlg();
	if (pLocateDlg!=NULL)
	{
		pField->pThumbnail->hBitmap=pLocateDlg->CreateThumbnail(GetPath(),
			&pLocateDlg->m_sCurrentIconSize,&pField->pThumbnail->sThumbnailSize);
	}

	pField->pThumbnail->CloseMutex();
	
	ItemDebugMessage("CLocatedItem::LoadThumbnail end");

}

void CLocatedItem::SetToDeleted()
{
	ItemDebugMessage("CLocatedItem::SetToDeleted BEGIN");
	
	if (GetLocateDlg()->GetExtraFlags()&CLocateDlg::efEnableItemUpdating)
	{
		WCHAR* pTmp=szType;
		InterlockedExchangePointer((PVOID*)&szType,allocstringW(IDS_DELETEDFILE));
		if (pTmp!=NULL)
			delete[] pTmp;
		iIcon=DEL_IMAGE;
		
		dwFileSize=DWORD(-1);
		wModifiedDate=WORD(-1);
		wModifiedTime=WORD(-1);
		wCreatedDate=WORD(-1);
		wCreatedTime=WORD(-1);
		wAccessedDate=WORD(-1);
		wAccessedTime=WORD(-1);

		DeleteAllExtraFields();

		dwFlags&=~LITEM_PARENTICONOK;
		dwFlags|=LITEM_ICONOK|LITEM_TYPEOK|LITEM_TIMEDATEOK|LITEM_FILESIZEOK;
		//DebugFormatMessage(L"dwFlags|=LITEM_ICONOK by SetToDeleted for %s",GetPath());
	}

	ItemDebugMessage("CLocatedItem::SetToDeleted END");
}


BOOL CLocatedItem::RemoveFlagsForChanged()
{
	if (!(GetLocateDlg()->GetExtraFlags()&CLocateDlg::efEnableItemUpdating))
		return FALSE;

	ItemDebugFormatMessage4("CLocatedItem::RemoveFlagsForChanged BEGIN, item:%S, flags already=%X",GetPath(),dwFlags,0,0);
	

	union {
		WIN32_FIND_DATA fd;
		WIN32_FIND_DATAW fdw;
	};
	
	// Checking whether file is available
	HANDLE hFind;
	if (IsUnicodeSystem())
		hFind=FindFirstFileW(GetPath(),&fdw);
	else
		hFind=FindFirstFile(W2A(GetPath()),&fd);
	DebugOpenHandle(dhtFileFind,hFind,GetPath());
			
	if (hFind==INVALID_HANDLE_VALUE)
	{
		if (!IsDeleted())
		{
			SetToDeleted();
			ItemDebugMessage("CLocatedItem::RemoveFlagsForChanged ENDUD");
			return TRUE;
		}
		ItemDebugMessage("CLocatedItem::RemoveFlagsForChanged ENDDD");
		return FALSE;
	}
	FindClose(hFind);
	DebugCloseHandle(dhtFileFind,hFind,GetPath());
			
    
	// Checking whether filename and title is correct
	WCHAR szFullPath[MAX_PATH];
	DWORD dwLength=GetLocateApp()->m_pGetLongPathName(GetPath(),szFullPath,MAX_PATH);
	if (dwLength==GetPathLen())
	{
		// Checking assumptions, i.e. length of file name and extension does not change 
		// and extension is same
		ASSERT(szFullPath[DWORD(szName-szPath)-1]==L'\\' && 
			(szFullPath[DWORD(szName-szPath)+bExtensionPos-1]==L'.' || 
			szFullPath[DWORD(szName-szPath)+bExtensionPos]==L'\0'));

        //This fixes case
		if (wcsncmp(szPath,szFullPath,dwLength)!=0)
		{
			RemoveFlags(LITEM_FILETITLEOK);   
			MemCopyW(szPath,szFullPath,dwLength);
		}
	}
	else if (dwLength>0)
	{
		WCHAR* pTmp=szPath;      
		InterlockedExchangePointer((PVOID*)&szPath,alloccopy(szFullPath,dwLength));       
		if (pTmp!=NULL)
			delete[] pTmp;
		
		InterlockedExchangePointer((PVOID*)&szName,szPath+LastCharIndex(szPath,L'\\')+1);
		bNameLength=BYTE(dwLength-DWORD(szName-szPath));

		for (bExtensionPos=bNameLength-1; szName[bExtensionPos-1]!=L'.' && bExtensionPos>0 ;bExtensionPos--);
		if (bExtensionPos==0)
			bExtensionPos=bNameLength;
	
		RemoveFlags(LITEM_FILETITLEOK);
	}

	if (IsDeleted())
	{
		// File has come back
		dwFlags&=~(LITEM_COULDCHANGE|LITEM_FILETITLEOK);

		// Settings information obtained from WIN32_FIND_DATA structure
		if (!IsFolder())
		{
			dwFileSize=fd.nFileSizeLow;
			wFileSizeHi=static_cast<BYTE>(fd.nFileSizeHigh);
		}

				
		FILETIME ftLocal;
		FileTimeToLocalFileTime(&fd.ftLastWriteTime,&ftLocal);
		FileTimeToDosDateTime(&ftLocal,&wModifiedDate,&wModifiedTime);
	
		FileTimeToLocalFileTime(&fd.ftCreationTime,&ftLocal);
		FileTimeToDosDateTime(&ftLocal,&wCreatedDate,&wCreatedTime);
	
		FileTimeToLocalFileTime(&fd.ftLastAccessTime,&ftLocal);
		FileTimeToDosDateTime(&ftLocal,&wAccessedDate,&wAccessedTime);
		
		wAttribs=GetAttributesFromSystemAttributes(fd.dwFileAttributes)|(wAttribs&LITEMATTRIB_DIRECTORY);
				
		dwFlags|=LITEM_TIMEDATEOK|LITEM_FILESIZEOK|LITEM_ATTRIBOK;

		// Read extra fields again
		DeleteAllExtraFields();
		
		ItemDebugFormatMessage4("CLocatedItem::RemoveFlagsForChanged END2, flagsnow=%X",dwFlags,0,0,0);
		return TRUE;
	}

	

	BOOL bRet=FALSE;

	BYTE wAttribFlag=GetAttributesFromSystemAttributes(fd.dwFileAttributes)|(wAttribs&LITEMATTRIB_DIRECTORY);
	if (wAttribFlag!=wAttribs)
	{
		bRet=TRUE;
		wAttribs=wAttribFlag|(wAttribs&LITEMATTRIB_CUTTED);
		dwFlags|=LITEM_ATTRIBOK;
	}

	if (!IsFolder() && fd.nFileSizeLow!=dwFileSize)
	{
		dwFileSize=fd.nFileSizeLow;
		wFileSizeHi=static_cast<BYTE>(fd.nFileSizeHigh);

		dwFlags|=LITEM_FILESIZEOK;
		ExtraSetUpdateWhenFileSizeChanged();
		bRet=TRUE;
	}
		

	WORD wTempCreationTime,wTempCreationDate;
	WORD wTempModificationTime,wTempModificationDate;
	WORD wTempAccessedTime,wTempAccessedDate;

	FILETIME ftLocal;
	FileTimeToLocalFileTime(&fd.ftLastWriteTime,&ftLocal);
	FileTimeToDosDateTime(&ftLocal,&wTempModificationDate,&wTempModificationTime);
	
	FileTimeToLocalFileTime(&fd.ftCreationTime,&ftLocal);
	FileTimeToDosDateTime(&ftLocal,&wTempCreationDate,&wTempCreationTime);
	
	FileTimeToLocalFileTime(&fd.ftLastAccessTime,&ftLocal);
	FileTimeToDosDateTime(&ftLocal,&wTempAccessedDate,&wTempAccessedTime);

	if (wTempModificationDate!=wModifiedDate || wTempModificationTime!=wModifiedTime ||
		wTempCreationDate!=wCreatedDate || wTempCreationTime!=wCreatedTime)
	{
		bRet=TRUE;
		wModifiedDate=wTempModificationDate;
		wModifiedTime=wTempModificationTime;
		wCreatedTime=wTempCreationTime;
		wCreatedDate=wTempCreationDate;
		wAccessedTime=wTempAccessedTime;
		wAccessedDate=wTempAccessedDate;
		ExtraSetUpdateWhenFileSizeChanged();
		dwFlags|=LITEM_TIMEDATEOK;
	}
	else if (wTempAccessedDate!=wAccessedDate || wTempAccessedTime!=wAccessedDate)
	{
		wAccessedTime=wTempAccessedTime;
		wAccessedDate=wTempAccessedDate;
		dwFlags|=LITEM_TIMEDATEOK;
	}

	ItemDebugFormatMessage4("CLocatedItem::RemoveFlagsForChanged END, flagsnow=%X",dwFlags,0,0,0);
	
	return bRet;
}
	
BOOL CLocatedItem::IsItemShortcut() const
{
	CRegKey RegKey;
	CString Type;
	if (RegKey.OpenKey(HKCR,szName+bExtensionPos-1,CRegKey::openExist|CRegKey::samRead|CRegKey::samQueryValue)==ERROR_SUCCESS)
	{
		if (RegKey.QueryValueLength("IsShortcut")>0)
			return TRUE;

		RegKey.QueryValue("",Type);
		RegKey.CloseKey();
		
		if (!Type.IsEmpty())
		{
			if (RegKey.OpenKey(HKCR,Type,CRegKey::openExist|CRegKey::samRead|CRegKey::samQueryValue)==ERROR_SUCCESS)
				return RegKey.QueryValueLength("IsShortcut")>0;
		}
	}
	return FALSE;
}

void CLocatedItem::DeleteExtraInfoField(DetailType nType)
{
	if (pFirstExtraInfo==NULL)
		return;
	if (pFirstExtraInfo->nType==nType)
	{
		ExtraInfo* pTmp=pFirstExtraInfo;
		pFirstExtraInfo=pTmp->pNext;
		delete pTmp;
	}
	else
	{
		ExtraInfo* pTmp=pFirstExtraInfo;
		while (pTmp->pNext!=NULL)
		{
			if (pTmp->pNext->nType==nType)
			{
				ExtraInfo* pTmp2=pTmp->pNext;
				pTmp->pNext=pTmp2->pNext;
				delete pTmp2;
				return;
			}
			pTmp=pTmp->pNext;
		}
	}
}

BOOL CLocatedItem::ChangeName(CWnd* pWnd,LPCWSTR szNewName,int iLength)
{
	if (iLength==-1)
		iLength=(int)istrlenw(szNewName);

	DWORD dwDirectoryLen=DWORD(szName-szPath);
	ASSERT(dwDirectoryLen>0);

	WCHAR* szOldPath=GetPath();;
	WCHAR* szNewPath=new WCHAR[dwDirectoryLen+iLength+2];
	
	// Copying directory
	MemCopyW(szNewPath,szOldPath,dwDirectoryLen);
	MemCopyW(szNewPath+dwDirectoryLen,szNewName,iLength);
	szNewPath[dwDirectoryLen+iLength]=L'\0';
	

	if (!FileSystem::MoveFile(szOldPath,szNewPath))
	{
		
		CHAR* pError;
		CStringW str;
			
		if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,NULL,
			GetLastError(),LANG_USER_DEFAULT,(LPSTR)&pError,0,NULL))
		{
			str.Format(IDS_ERRORCANNOTRENAME,(LPCWSTR)A2W(pError));
			LocalFree(pError);
		}
		else
			str.Format(IDS_ERRORCANNOTRENAME,L"");

		pWnd->MessageBox(str,ID2W(IDS_ERROR),MB_OK|MB_ICONERROR);

		delete[] szNewPath;
		return FALSE;
	}





	
	// Do fileoperation first
	if (szFileTitle!=szName && szFileTitle!=NULL)
	{
		WCHAR* pTemp=szFileTitle;
		InterlockedExchangePointer((PVOID*)&szFileTitle,NULL);
		delete[] pTemp;
	}
	else
		InterlockedExchangePointer((PVOID*)&szFileTitle,szNewPath+dwDirectoryLen);


	InterlockedExchangePointer((PVOID*)&szName,szNewPath+dwDirectoryLen);
	bNameLength=iLength;
	InterlockedExchangePointer((PVOID*)&szPath,szNewPath);
	
	// Finding extension
	for (bExtensionPos=bNameLength-1; szName[bExtensionPos-1]!=L'.' && bExtensionPos>0 ;bExtensionPos--);
	if (bExtensionPos==0)
		bExtensionPos=bNameLength;

	delete[] szOldPath;

	UpdateFileTitle();

	return TRUE;
}

BOOL CLocatedItem::ChangeParentName(CWnd* pWnd,LPCWSTR szNewName,int iLength)
{
	if (iLength==-1)
		iLength=(int)istrlenw(szNewName);

	int iParentNamePos=LastCharIndex(GetParent(),L'\\')+1;
	if (iParentNamePos==0)
		return FALSE; // Cannot change drive letter

	WCHAR* szOldPath=GetParent();;
	WCHAR* szNewPath=new WCHAR[iParentNamePos+iLength+GetNameLen()+2];
	
	// Copying directory
	MemCopyW(szNewPath,szOldPath,iParentNamePos);
	MemCopyW(szNewPath+iParentNamePos,szNewName,iLength);
	szNewPath[iParentNamePos+iLength]=L'\0';	

	if (!FileSystem::MoveFile(szOldPath,szNewPath))
	{
		
		CHAR* pError;
		CStringW str;
			
		if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,NULL,
			GetLastError(),LANG_USER_DEFAULT,(LPSTR)&pError,0,NULL))
		{
			str.Format(IDS_ERRORCANNOTRENAME,(LPCWSTR)A2W(pError));
			LocalFree(pError);
		}
		else
			str.Format(IDS_ERRORCANNOTRENAME,L"");

		pWnd->MessageBox(str,ID2W(IDS_ERROR),MB_OK|MB_ICONERROR);

		delete[] szNewPath;
		return FALSE;
	}

	// Copy name part to file
	iLength+=iParentNamePos+1;
	MemCopyW(szNewPath+iLength,GetName(),GetNameLen());
	szNewPath[iLength+GetNameLen()]='\0';


	// Do fileoperation first
	if (szFileTitle!=szName && szFileTitle!=NULL)
	{	
		// Nullify title
		WCHAR* pTemp=szFileTitle;
		InterlockedExchangePointer((PVOID*)&szFileTitle,NULL);
		delete[] pTemp;
	}
	else
		InterlockedExchangePointer((PVOID*)&szFileTitle,szNewPath+iLength);


	InterlockedExchangePointer((PVOID*)&szName,szNewPath+iLength);
	InterlockedExchangePointer((PVOID*)&szPath,szNewPath);
	
	// Finding extension
	for (bExtensionPos=bNameLength-1; szName[bExtensionPos-1]!=L'.' && bExtensionPos>0 ;bExtensionPos--);
	if (bExtensionPos==0)
		bExtensionPos=bNameLength;

	delete[] szOldPath;

	UpdateFileTitle();

	return TRUE;
}

LPWSTR CLocatedItem::GetToolTipText() const
{
	ISDLGTHREADOK

	if (IsDeleted())
	{
		CStringW str(IsFolder()?IDS_TOOLTIPFORDIRECTORYDELETED:IDS_TOOLTIPFORFILEDELETED);
		int nLen=(int)str.GetLength()+GetPathLen()+(int)istrlenw(GetType())+2;
		LPWSTR szwBuffer=GetBufferW(nLen);
		swprintfex(szwBuffer,nLen,str,GetName(),GetParent(),GetType(),GetLocateApp()->GetDatabase(GetDatabaseID())->GetName());
		return szwBuffer;
	}

	
		
	if (ShouldUpdateFileSizeOrDate())
		((CLocatedItem*)this)->UpdateFileSizeAndTime();
	if (ShouldUpdateType())
		((CLocatedItem*)this)->UpdateType();
	
	WCHAR* szDate=GetLocateApp()->FormatDateAndTimeString(GetModifiedDate(),GetModifiedTime());
	
	LPWSTR szwBuffer;
	if (IsFolder())
	{
		CStringW str(IDS_TOOLTIPFORDIRECTORY);
		int nLen=(int)str.GetLength()+GetPathLen()+(int)istrlenw(GetType())+(int)istrlenw(szDate)+2;
		szwBuffer=GetBufferW(nLen);
		swprintfex(szwBuffer,nLen,str,GetName(),GetParent(),GetType(),szDate,GetLocateApp()->GetDatabase(GetDatabaseID())->GetName());
	}
	else 
	{
		CStringW text;
		
		WCHAR szSize[25];

		if (GetFileSize()>LONGLONG(1024*1024*1024)) // Over 1 Gb
		{
			StringCbPrintfW(szSize,25*sizeof(WCHAR),L"%1.2f",double(GetFileSize())/(1024*1024*1024));
			int nLength=(int)istrlenw(szSize);
			while (szSize[nLength-1]=='0')
				nLength--;
			if (szSize[nLength-1]=='.')
				nLength--;
			::LoadString(IDS_GB,szSize+nLength,25-nLength);
		}	
		else if (GetFileSize()>LONGLONG(1024*1024)) // Over 1 Mb
		{
			StringCbPrintfW(szSize,25*sizeof(WCHAR),L"%1.2f",double(GetFileSize())/(1024*1024));
			int nLength=(int)wcslen(szSize);
			while (szSize[nLength-1]=='0')
				nLength--;
			if (szSize[nLength-1]=='.')
				nLength--;
			::LoadString(IDS_MB,szSize+nLength,25-nLength);
		}	
		else if (GetFileSize()>LONGLONG(1024)) // Over 1 Gb
		{
			StringCbPrintfW(szSize,25*sizeof(WCHAR),L"%1.2f",double(GetFileSize())/(1024));
			int nLength=(int)wcslen(szSize);
			while (szSize[nLength-1]=='0')
				nLength--;
			if (szSize[nLength-1]=='.')
				nLength--;
			::LoadString(IDS_KB,szSize+nLength,25-nLength);
		}	
		else
		{
			HRESULT hRes=StringCbPrintfW(szSize,25*sizeof(WCHAR),L"%d",GetFileSizeLo());
			int nLength=(int)wcslen(szSize);
			if (SUCCEEDED(hRes))
				::LoadString(IDS_BYTES,szSize+nLength,25-nLength);
			else
				szSize[0]='\0';
		}

		text.FormatEx(IDS_TOOLTIPFORFILE,GetName(),GetParent(),GetType(),szDate,szSize,GetLocateApp()->GetDatabase(GetDatabaseID())->GetName());
			
		if (IsItemShortcut())
		{
			// Get command and target
			
			union {
				IShellLinkW* pslw;
				IShellLink* psl;
			};
			IPersistFile* ppf=NULL;
			psl=NULL;
			
			if (IsUnicodeSystem())
			{
				if (SUCCEEDED(CoCreateInstance(CLSID_ShellLink,NULL,CLSCTX_INPROC_SERVER,IID_IShellLinkW,(void**)&pslw)))
				{
					if (FAILED(pslw->QueryInterface(IID_IPersistFile,(void**)&ppf)))
						ppf=NULL;
				}
			}
			else
			{
				if (SUCCEEDED(CoCreateInstance(CLSID_ShellLink,NULL,CLSCTX_INPROC_SERVER,IID_IShellLink,(void**)&psl)))
				{
					if (FAILED(psl->QueryInterface(IID_IPersistFile,(void**)&ppf)))
						ppf=NULL;
				}
			}

			if (ppf!=NULL)
			{
				if (SUCCEEDED(ppf->Load(GetPath(),STGM_READ)))
				{
					WCHAR szBuffer[INFOTIPSIZE+2];
					HRESULT hRes=pslw->GetPath(szBuffer,INFOTIPSIZE+2,NULL,0);
					if (SUCCEEDED(hRes))
					{
						text << L"\r\n";
						text.AddString(IDS_TOOLTIPTARGET);
						text << L' ' << szBuffer;
					}

					szBuffer[0]='\0';
					hRes=pslw->GetDescription(szBuffer,INFOTIPSIZE+2);
					if (SUCCEEDED(hRes) && szBuffer[0]!='\0')
					{
						text << L"\r\n";
						text.AddString(IDS_TOOLTIPDESCRIPTION);
						text << L' ' << szBuffer;
					}

	
				}
				ppf->Release();
			}

			if (psl!=NULL)
			{
				if (IsUnicodeSystem())
					pslw->Release();
				else
					psl->Release();
			}
		}

		// Add comment
		

		if (ShouldUpdateExtra(Author) || ShouldUpdateExtra(Comments))
			((CLocatedItem*)this)->UpdateSummaryProperties();
		
		if (ShouldUpdateExtra(FileVersion) || ShouldUpdateExtra(ProductVersion) || ShouldUpdateExtra(Description))
			((CLocatedItem*)this)->UpdateVersionInformation();
		
		LPCWSTR pText=GetExtraText(Description);
		if (pText!=NULL)
			text << L"\r\n" << ID2W(IDS_TOOLTIPDESCRIPTION) << L' ' << pText;
		
		pText=GetExtraText(FileVersion);
		if (pText!=NULL)
			text << L"\r\n" << ID2W(IDS_TOOLTIPFILEVERSION) << L' ' << pText;
		
		pText=GetExtraText(ProductVersion);
		if (pText!=NULL)
			text << L"\r\n" << ID2W(IDS_TOOLTIPPRODUCTVERSION) << L' ' << pText;
		
		pText=GetExtraText(Author);
		if (pText!=NULL)
			text << L"\r\n" << ID2W(IDS_TOOLTIPAUTHOR) << L' ' << pText;
		
		pText=GetExtraText(Title);
		if (pText!=NULL)
			text << L"\r\n" << ID2W(IDS_TOOLTIPTITLE) << L' ' << pText;
		
		
		pText=GetExtraText(Comments);
		if (pText!=NULL)
		{
			text << L"\r\n" << ID2W(IDS_TOOLTIPCOMMENTS);

			int nIndex=FirstCharIndex(pText,L'\\');
			if (nIndex!=-1) 
			{
				// Parse line feeds
				for (;;)
				{
					text << L"\r\n";
					text.Append(pText,nIndex);

					if (nIndex==-1)
						break;

					pText+=nIndex+1;

					if (*pText=='n')
						pText++;

					nIndex=FirstCharIndex(pText,L'\\');
				}
				
			}
			else	
				text << pText;
		}
		
		// Finally, copy text to a buffer
		szwBuffer=GetBufferW(text.GetLength()+1);
		MemCopyW(szwBuffer,LPCWSTR(text),text.GetLength()+1);
	}

	delete[] szDate;
	return szwBuffer;
}

WORD CLocatedItem::GetAttributesFromSystemAttributes(DWORD dwSystemAttributes)
{
	WORD wRet=0;
	if (dwSystemAttributes&FILE_ATTRIBUTE_ARCHIVE)
		wRet|=LITEMATTRIB_ARCHIVE;
	if (dwSystemAttributes&FILE_ATTRIBUTE_HIDDEN)
		wRet|=LITEMATTRIB_HIDDEN;
	if (dwSystemAttributes&FILE_ATTRIBUTE_READONLY)
		wRet|=LITEMATTRIB_READONLY;
	if (dwSystemAttributes&FILE_ATTRIBUTE_SYSTEM)
		wRet|=LITEMATTRIB_SYSTEM;
	if (dwSystemAttributes&FILE_ATTRIBUTE_COMPRESSED)
		wRet|=LITEMATTRIB_COMPRESSED;
	if (dwSystemAttributes&FILE_ATTRIBUTE_ENCRYPTED)
		wRet|=LITEMATTRIB_ENCRYPTED;
	return wRet;
}

DWORD CLocatedItem::GetSystemAttributesFromAttributes(WORD wAttributes)
{
	DWORD dwRet=0;
	if (wAttributes&LITEMATTRIB_DIRECTORY)
		dwRet|=FILE_ATTRIBUTE_DIRECTORY;
	if (wAttributes&LITEMATTRIB_ARCHIVE)
		dwRet|=FILE_ATTRIBUTE_ARCHIVE;
	if (wAttributes&LITEMATTRIB_HIDDEN)
		dwRet|=FILE_ATTRIBUTE_HIDDEN;
	if (wAttributes&LITEMATTRIB_READONLY)
		dwRet|=FILE_ATTRIBUTE_READONLY;
	if (wAttributes&LITEMATTRIB_SYSTEM)
		dwRet|=FILE_ATTRIBUTE_SYSTEM;
	if (wAttributes&LITEMATTRIB_COMPRESSED)
		dwRet|=FILE_ATTRIBUTE_COMPRESSED;
	if (wAttributes&LITEMATTRIB_ENCRYPTED)
		dwRet|=FILE_ATTRIBUTE_ENCRYPTED;
	if (wAttributes&(LITEMATTRIB_JUNCTION|LITEMATTRIB_SYMLINK))
		dwRet|=FILE_ATTRIBUTE_REPARSE_POINT;
	return dwRet;
}
	