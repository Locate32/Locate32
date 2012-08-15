//
// Locate Find Extension
// Copyright 2005-2007 Janne Huttunen
//
// class id:  8A2F2E09-3209-4934-ACB1-DE5BB052950F
//
//           CShellExt::AddRef and CShellExt::Release code.


#ifndef STRICT
#define STRICT
#endif

#define INC_OLE2        // WIN32, get ole2 from windows.h

#include <windows.h>
#include <windowsx.h>
#include <shlobj.h>

#include "FindExtension.h"
#include "../Locate32/messages.h"

#ifndef ARRAYSIZE
#define ARRAYSIZE(x) (sizeof(x)/sizeof(x[0]))
#endif

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

STDMETHODIMP CFindExtension::QueryContextMenu(HMENU hMenu,
                                         UINT indexMenu,
                                         UINT idCmdFirst,
                                         UINT idCmdLast,
                                         UINT uFlags)
{
    ODS("CFindExtension::QueryContextMenu()\r\n");
	return NOERROR;
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

STDMETHODIMP CFindExtension::InvokeCommand(LPCMINVOKECOMMANDINFO lpcmi)
{
    ODS("CFindExtension::InvokeCommand()\r\n");

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
                hr = OpenLocate(lpcmi->hwnd,
                                lpcmi->lpDirectory,
                                lpcmi->lpVerb,
                                lpcmi->lpParameters,
                                lpcmi->nShow);
                break;

           
        }
    }
    return hr;
}




STDMETHODIMP CFindExtension::GetCommandString(UINT_PTR idCmd,
                                         UINT uFlags,
                                         UINT FAR *reserved,
                                         LPSTR pszName,
                                         UINT cchMax)
{
    ODS("CFindExtension::GetCommandString()\r\n");

    switch (idCmd)
    {
        case 0:
			pszName[0]='\0';
            break;
    }

    return NOERROR;
}

STDMETHODIMP CFindExtension::OpenLocate(HWND hParent,
                                   LPCSTR pszWorkingDir,
                                   LPCSTR pszCmd,
                                   LPCSTR pszParam,
                                   int iShowCmd)
{
	extern HINSTANCE g_hmodThisDll;
    ODS("CFindExtension::OpenLocate()\r\n");

    HWND hWnd=FindWindow("LOCATEAPPST","Locate ST");
	if (hWnd!=NULL)
	{
		UINT nLocateAppMessage=RegisterWindowMessage("Locate32Communication");
		::SendNotifyMessage(HWND_BROADCAST,nLocateAppMessage,LOCATEMSG_ACTIVATEINSTANCE,(LPARAM)NULL);
		return NOERROR;
	}	
	/*else
		MessageBox(NULL,"No locate active","Error",MB_OK);*/
	

	char szPath[MAX_PATH]="";
	GetModuleFileName(g_hmodThisDll,szPath,MAX_PATH);

	int nLast=-1;
    for (int i=0;szPath[i]!='\0';i++)
	{
		if (szPath[i]=='\\')
			nLast=i;
	}

    strcpy_s(szPath+nLast+1,MAX_PATH-nLast-1,"locate32.exe");

	// Just execute command
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	ZeroMemory(&si,sizeof(STARTUPINFO));
	si.cb=sizeof(STARTUPINFO);
	
	//MessageBox(NULL,szPath,"Executing",MB_OK);

	if (CreateProcess(NULL,szPath,NULL,
		NULL,FALSE,CREATE_DEFAULT_ERROR_MODE|NORMAL_PRIORITY_CLASS,
		NULL,NULL,&si,&pi))
	{
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		return NOERROR;
	}
	else
	{
		if (CreateProcess(NULL,"locate32.exe",NULL,
		NULL,FALSE,CREATE_DEFAULT_ERROR_MODE|NORMAL_PRIORITY_CLASS,
		NULL,NULL,&si,&pi))
		{
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
			return NOERROR;
		}
	}
    return E_FAIL;
}

