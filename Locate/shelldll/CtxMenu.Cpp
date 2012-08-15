//
// Locate Shell Extension
// Copyright 2005 Janne Huttunen
//
// class id:  73ACE856-AD8F-4346-9A07-775E45221BC8
//
//           CShellExt::AddRef and CShellExt::Release code.


#ifndef STRICT
#define STRICT
#endif

#define INC_OLE2        // WIN32, get ole2 from windows.h

#define _CONSOLE
#include "shlwapi.h"
#include <hfclib.h>
#include <windowsx.h>
#include <shlobj.h>
#include "../lan_resources.h"
#include "LocateShellExtension.h"
#include "resource.h"

#pragma comment(lib, "shlwapi.lib")

extern HINSTANCE g_hmodThisDll;

INT IsDirectory(LPCWSTR szDirectoryName)
{
	

	HANDLE hFind;
	WIN32_FIND_DATAW fd;
	
	int ret=TRUE;
	
	if (szDirectoryName[0]==L'\0')
		return 0;
	
	if (szDirectoryName[1]==L'\0')
		return 0;
	
	if (szDirectoryName[2]==L'\0')
		return 0;
	
	if (szDirectoryName[1]==L':' && szDirectoryName[2]==L'\\' && szDirectoryName[3]==L'\0')
	{
		switch (GetDriveTypeW(szDirectoryName))
		{
		case DRIVE_UNKNOWN:
		case DRIVE_NO_ROOT_DIR:
			return 0;
		case DRIVE_FIXED:
			return 1;
		default:
			return 2;
		}
	}
	
	

	// Taking last '\\' 
	LPWSTR szPath;
	SIZE_T dwPathLen=istrlenw(szDirectoryName);
	if (szDirectoryName[dwPathLen-1]==L'\\' && dwPathLen>3)
	{
		szPath=new WCHAR[dwPathLen+5];
		--dwPathLen;
		MemCopyW(szPath,szDirectoryName,dwPathLen);
		szPath[dwPathLen]='\0';
	}
	else
		szPath=LPWSTR(szDirectoryName);

	

	hFind=FindFirstFileW(szPath,&fd);
	if (hFind!=INVALID_HANDLE_VALUE)
	{
		while (!(fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) && ret)
			ret=FindNextFileW(hFind,&fd);
	
		if (szPath!=szDirectoryName)
			delete[] szPath;

		FindClose(hFind);	
		if (ret)
		{
			if (szDirectoryName[0]==L'\\')
			{
				if (szDirectoryName[1]==L'\\')
					return 2;
				switch (GetDriveType(NULL))
				{
				case DRIVE_UNKNOWN:
				case DRIVE_NO_ROOT_DIR:
					return 0;
				case DRIVE_FIXED:
					return 1;
				default:
					return 2;
				}
 
			}
			if (szDirectoryName[1]==L':' && szDirectoryName[2]==L'\\')
			{
				WCHAR szTemp[4]=L"X:\\";
				szTemp[0]=szDirectoryName[0];

				switch (GetDriveTypeW(szTemp))
				{
				case DRIVE_UNKNOWN:
					return 0;
				case DRIVE_FIXED:
					return 1;
				default:
					return 2;
				}
			}
		}
		return 0;
	}
	else if (szDirectoryName[0]==L'\\' && szDirectoryName[1]==L'\\')
	{
		
		// UNC share name
		if (szPath==szDirectoryName)
		{
			szPath=new WCHAR[dwPathLen+5];
			CopyMemory(szPath,szDirectoryName,dwPathLen*2);
		}		
		CopyMemory(szPath+dwPathLen,L"\\*.*",5*2);

		hFind=FindFirstFileW(szPath,&fd);
		delete[] szPath;
		if (hFind==INVALID_HANDLE_VALUE)
			return 0;
		// Is this really needed, e.g. \\pc\c$ does not have '.' in directory list
		//ret=1;
		//while ((fd.cFileName[0]!='.' || fd.cFileName[1]!='\0') && ret==1)
		//	ret=FindNextFile(hFind,&fd);
		FindClose(hFind);
		//return ret?2:0;
		return 2;
	}
	
	
	
	if (szPath!=szDirectoryName)
		delete[] szPath;
	return 0;
}

HMODULE GetLanguageResource()
{
	HKEY hKey;
	LPSTR lanFile=NULL;
	if (RegOpenKeyEx(HKEY_CURRENT_USER,"Software\\Update\\Locate32",0,KEY_READ,&hKey)==ERROR_SUCCESS)
	{
		DWORD lLength;
		if (RegQueryValueEx(hKey,"Language",0,NULL,NULL,&lLength)==ERROR_SUCCESS)
		{
			lanFile=new char[lLength+1];
			if (RegQueryValueEx(hKey,"Language",0,NULL,(LPBYTE)lanFile,&lLength)!=ERROR_SUCCESS)
			{
				delete[] lanFile;
				lanFile=NULL;
			}
		}
		RegCloseKey(hKey);
	}


	char szPath[MAX_PATH];
	int nLen=GetModuleFileName(g_hmodThisDll,szPath,MAX_PATH);
	for (nLen--;nLen>=0 && szPath[nLen]!='\\';nLen--);
	nLen++;
    
	if (lanFile==NULL)
		StringCbCopy(szPath+nLen,MAX_PATH-nLen,"lan_en.dll");
	else
	{
        StringCbCopy(szPath+nLen,MAX_PATH-nLen,lanFile);
		delete[] lanFile;
	}

	return LoadLibrary(szPath);
}

//
//  FUNCTION: CShellExt::QueryContextMenu(HMENU, UINT, UINT, UINT, UINT)
//
//  PURPOSE: Called by the shell just before the context menu is displayed.
//           This is where you add your specific menu items.
//
//  PARAMETERS:
//    hMenu      - Handle to the context menu
//    indexMenu  - Index of where to begin inserting menu items
//    idCmdFirst - Lowest value for new menu ID's
//    idCmtLast  - Highest value for new menu ID's
//    uFlags     - Specifies the context of the menu event
//
//  RETURN VALUE:
//
//
//  COMMENTS:
//

STDMETHODIMP CLocateShellExtension::QueryContextMenu(HMENU hMenu,
                                         UINT indexMenu,
                                         UINT idCmdFirst,
                                         UINT idCmdLast,
                                         UINT uFlags)
{
#ifdef _LOGGING
	char szTemp[100];
    
	sprintf_s(szTemp, 100,"CShellExt::QueryContextMenu(), uFlags==>%d", uFlags);
    ODS(szTemp);
#endif    
	
	if (uFlags & CMF_DEFAULTONLY)
	{
		ODS("CMF_DEFAULTONLY...");
			return NOERROR;
	}
    
	HMODULE hLanDll=GetLanguageResource();
	if (hLanDll==NULL)
		return NOERROR;

	DWORD dwSettings=ReadSettingsFromRegistry();

	if (dwSettings&fSubMenu)
	{
		HMENU hSubMenu=CreatePopupMenu();
		UINT uHighestId=0;
		if (InsertMenuItems(hSubMenu,0,idCmdFirst,uHighestId))
		{
			MENUITEMINFO mii;
			char szTitle[200];
			LoadString(hLanDll,IDS_EXTLOCATEMENU,szTitle,200);

			mii.cbSize=sizeof(MENUITEMINFO);
			mii.fMask=MIIM_SUBMENU|MIIM_TYPE;
			mii.fType=MFT_STRING;
			mii.dwTypeData=szTitle;
			mii.hSubMenu=hSubMenu;
			InsertMenuItem(hMenu,indexMenu,TRUE,&mii);

			FreeLibrary(hLanDll);
			return ResultFromShort(uHighestId+1-idCmdFirst);
		}
		
		DestroyMenu(hSubMenu);
		FreeLibrary(hLanDll);
		return NOERROR;	
	}
	else
	{
		UINT uHighestId=0;
		if (InsertMenuItems(hMenu,indexMenu,idCmdFirst,uHighestId))
		{
			FreeLibrary(hLanDll);
			return ResultFromShort(uHighestId+1-idCmdFirst);
		}
		FreeLibrary(hLanDll);
		return NOERROR;
	}
}

//
//  FUNCTION: CShellExt::ReadSettingsFromRegistry()
//
//  PURPOSE: Reads settings from registry
//
//  RETURN VALUE:
//	  flags
//
//

DWORD CLocateShellExtension::ReadSettingsFromRegistry()
{

	char szRegpath[200];
	LoadString(g_hmodThisDll,IDS_REGPLACE,szRegpath,200);

	HKEY hKey;
	
	if (RegOpenKeyEx(HKEY_CURRENT_USER,szRegpath,0,KEY_READ,&hKey)!=ERROR_SUCCESS)
		return fDidnSuccess|fDefaults;

    DWORD dwData=fDefaults,dwLength=sizeof(DWORD);

	LONG lRet=RegQueryValueEx(hKey,"Flags",NULL,NULL,(LPBYTE)&dwData,&dwLength);
	
	RegCloseKey(hKey);

	if (lRet!=ERROR_SUCCESS || dwLength!=sizeof(DWORD))
		return fDidnSuccess|fDefaults;
	return dwData;
}
    
//
//  FUNCTION: CShellExt::InvokeCommand(LPCMINVOKECOMMANDINFO)
//
//  PURPOSE: Called by the shell after the user has selected on of the
//           menu items that was added in QueryContextMenu().
//
//  PARAMETERS:
//    lpcmi - Pointer to an CMINVOKECOMMANDINFO structure
//
//  RETURN VALUE:
//
//
//  COMMENTS:
//

STDMETHODIMP CLocateShellExtension::InvokeCommand(LPCMINVOKECOMMANDINFO lpcmi)
{
    ODS("CShellExt::InvokeCommand()	");

    HRESULT hr = E_INVALIDARG;

    //If HIWORD(lpcmi->lpVerb) then we have been called programmatically
    //and lpVerb is a command that should be invoked.  Otherwise, the shell
    //has called us, and LOWORD(lpcmi->lpVerb) is the menu ID the user has
    //selected.  Actually, it's (menu ID - idCmdFirst) from QueryContextMenu().
    if (!HIWORD(lpcmi->lpVerb))
    {
        UINT idCmd = LOWORD(lpcmi->lpVerb);

        switch (idCmd)
        {
            case 0:
                hr = DoGAKMenu1(lpcmi->hwnd,
                                lpcmi->lpDirectory,
                                lpcmi->lpVerb,
                                lpcmi->lpParameters,
                                lpcmi->nShow);
                break;

           
        }
    }
    return hr;
}

//
//  FUNCTION: CShellExt::InsertMenuItems(HMENU hMenu,int nStartID)
//
//  PURPOSE: Creates menu items
//
//  RETURN VALUE: how many items are inserted

int CLocateShellExtension::InsertMenuItems(HMENU hMenu,UINT nStartIndex,UINT idCmdFirst,UINT& nHighestUsedID)
{
	ODS("CShellExt::InsertMenuItems()");
	
	if (m_pDataObj==NULL)
		return 0;

	
	

	

	
	MENUITEMINFOW mii;
	mii.cbSize=sizeof(MENUITEMINFOW);
	mii.fMask=MIIM_ID|MIIM_TYPE;
	mii.fType=MFT_STRING;
	mii.dwTypeData=L"Files:";
	mii.wID=idCmdFirst;
    InsertMenuItemW(hMenu,nStartIndex++,TRUE,&mii);

	for (int i=0;i<m_aFiles.GetSize();i++)
	{
		mii.wID++;
		mii.dwTypeData=m_aFiles[i];
		InsertMenuItemW(hMenu,nStartIndex++,TRUE,&mii);
	}

	mii.wID++;
	mii.dwTypeData=L"Directories:";
	InsertMenuItemW(hMenu,nStartIndex++,TRUE,&mii);
	
	for (int i=0;i<m_aDirectories.GetSize();i++)
	{
		mii.wID++;
		mii.dwTypeData=m_aDirectories[i];
		InsertMenuItemW(hMenu,nStartIndex++,TRUE,&mii);
	}


	mii.wID++;
	mii.dwTypeData=L"Computers:";
	InsertMenuItemW(hMenu,nStartIndex++,TRUE,&mii);
	
	for (int i=0;i<m_aComputers.GetSize();i++)
	{
		mii.wID++;
		mii.dwTypeData=m_aComputers[i];
		InsertMenuItemW(hMenu,nStartIndex++,TRUE,&mii);
	}

	
	nHighestUsedID=mii.wID;
	
	return mii.wID-idCmdFirst+1;
}

//
//  FUNCTION: CShellExt::InvokeCommand(LPCMINVOKECOMMANDINFO)
//
//  PURPOSE: Called by the shell after the user has selected on of the
//           menu items that was added in QueryContextMenu().
//
//  PARAMETERS:
//    lpcmi - Pointer to an CMINVOKECOMMANDINFO structure
//
//  RETURN VALUE:
//
//
//  COMMENTS:
//

STDMETHODIMP CLocateShellExtension::GetCommandString(UINT_PTR idCmd,
                                         UINT uFlags,
                                         UINT FAR *reserved,
                                         LPSTR pszName,
                                         UINT cchMax)
{
    ODS("CShellExt::GetCommandString()");

	HMODULE hLanDll=GetLanguageResource();
	if (hLanDll==NULL)
		return NOERROR;
	

	if (uFlags==GCS_HELPTEXTW)
	{
        switch (idCmd)
		{
	        case 0:
				LoadStringW(hLanDll,IDS_EXTLOCATEMENU,(LPWSTR)pszName,cchMax);
				break;
	
		}
	}
	else if (uFlags==GCS_HELPTEXTA)
	{
        switch (idCmd)
		{
	        case 0:
				LoadString(hLanDll,IDS_EXTLOCATEMENU,pszName,cchMax);
				break;
	
		}
	}
	else
	{
		pszName[0]='\0';
		pszName[1]='\0';
	}

	FreeLibrary(hLanDll);
    return NOERROR;
}

STDMETHODIMP CLocateShellExtension::DoGAKMenu1(HWND hParent,
                                   LPCSTR pszWorkingDir,
                                   LPCSTR pszCmd,
                                   LPCSTR pszParam,
                                   int iShowCmd)
{
    ODS("CShellExt::DoGAKMenu1()");

    MessageBox(hParent, "Menu item 1!", "Shell Extension Sample", MB_OK);

    return NOERROR;
}

