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

#define ResultFromShort(i)  ResultFromScode(MAKE_SCODE(SEVERITY_SUCCESS, 0, (USHORT)(i)))

#pragma comment(lib, "shlwapi.lib")


//
// Initialize GUIDs (should be done only and at-least once per DLL/EXE)
//
#pragma data_seg(".text")
#define INITGUID
#include <initguid.h>
#include <shlguid.h>

#pragma data_seg()


#include "../lan_resources.h"
#include "LocateShellExtension.h"

extern INT IsDirectory(LPCWSTR szDirectoryName);

//
// Global variables
//
UINT      g_cRefThisDll = 0;    // Reference count of this DLL.
HINSTANCE g_hmodThisDll = NULL; // Handle to this DLL itself.



extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        // Extension DLL one-time initialization
        g_hmodThisDll = hInstance;

		StartDebugLogging();

		ODS("In DLLMain, DLL_PROCESS_ATTACH");
    }
    else if (dwReason == DLL_PROCESS_DETACH)
    {
        ODS("In DLLMain, DLL_PROCESS_DETACH");

		EndDebugLogging();
    }

    return 1;   // ok
}

//---------------------------------------------------------------------------
// DllCanUnloadNow
//---------------------------------------------------------------------------

STDAPI DllCanUnloadNow(void)
{
    ODS("In DLLCanUnloadNow");

    return (g_cRefThisDll == 0 ? S_OK : S_FALSE);
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppvOut)
{
    ODS("In DllGetClassObject");

    *ppvOut = NULL;

    if (IsEqualIID(rclsid, CLSID_ShellExtension))
    {
        CShellExtClassFactory *pcf = new CShellExtClassFactory;

        return pcf->QueryInterface(riid, ppvOut);
    }

    return CLASS_E_CLASSNOTAVAILABLE;
}

CShellExtClassFactory::CShellExtClassFactory()
{
    ODS("CShellExtClassFactory::CShellExtClassFactory()");

    m_cRef = 0L;

    g_cRefThisDll++;
}

CShellExtClassFactory::~CShellExtClassFactory()
{
    g_cRefThisDll--;
}

STDMETHODIMP CShellExtClassFactory::QueryInterface(REFIID riid,
                                                   LPVOID FAR *ppv)
{
    ODS("CShellExtClassFactory::QueryInterface()");

    *ppv = NULL;

    // Any interface on this object is the object pointer

    if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
    {
        *ppv = (LPCLASSFACTORY)this;

        AddRef();

        return NOERROR;
    }

    return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CShellExtClassFactory::AddRef()
{
    ODS("CShellExtClassFactory::AddRef()");

	char szNum[100];
	StringCbPrintf(szNum,100,"m_cRef is after this call %d",m_cRef+1);
	DebugMessage(szNum);

	return ++m_cRef;
}

STDMETHODIMP_(ULONG) CShellExtClassFactory::Release()
{
	ODS("CShellExtClassFactory::Release()");

	char szNum[100];
	StringCbPrintf(szNum,100,"m_cRef is after this call %d",m_cRef-1);
	DebugMessage(szNum);

    if (--m_cRef)
        return m_cRef;

	DebugMessage("Removing class"); 
	delete this;

    return 0L;
}

STDMETHODIMP CShellExtClassFactory::CreateInstance(LPUNKNOWN pUnkOuter,
                                                      REFIID riid,
                                                      LPVOID *ppvObj)
{
    ODS("CShellExtClassFactory::CreateInstance()");

    *ppvObj = NULL;

    // Shell extensions typically don't support aggregation (inheritance)

    if (pUnkOuter)
        return CLASS_E_NOAGGREGATION;

    // Create the main shell extension object.  The shell will then call
    // QueryInterface with IID_IShellExtInit--this is how shell extensions are
    // initialized.

    LPCSHELLEXT pShellExt = new CLocateShellExtension();  //Create the CShellExt object

    if (NULL == pShellExt)
        return E_OUTOFMEMORY;

    return pShellExt->QueryInterface(riid, ppvObj);
}


STDMETHODIMP CShellExtClassFactory::LockServer(BOOL fLock)
{
    return NOERROR;
}

// *********************** CShellExt *************************
CLocateShellExtension::CLocateShellExtension()
{
    ODS("CShellExt::CShellExt()");

    m_cRef = 0L;
    m_pDataObj = NULL;

    g_cRefThisDll++;
}

CLocateShellExtension::~CLocateShellExtension()
{
    if (m_pDataObj)
        m_pDataObj->Release();

    g_cRefThisDll--;
}

STDMETHODIMP CLocateShellExtension::QueryInterface(REFIID riid, LPVOID FAR *ppv)
{
    *ppv = NULL;

    if (IsEqualIID(riid, IID_IShellExtInit) || IsEqualIID(riid, IID_IUnknown))
    {
        ODS("CShellExt::QueryInterface()==>IID_IShellExtInit");

        *ppv = (LPSHELLEXTINIT)this;
    }
    else if (IsEqualIID(riid, IID_IContextMenu))
    {
        ODS("CShellExt::QueryInterface()==>IID_IContextMenu");

        *ppv = (LPCONTEXTMENU)this;
    }
    /*else if (IsEqualIID(riid, IID_IPersistFile))
    {
        ODS("CShellExt::QueryInterface()==>IPersistFile\r\n");

        *ppv = (LPPERSISTFILE)this;
    }*/
    /*else if (IsEqualIID(riid, IID_IShellPropSheetExt))
    {
        ODS("CShellExt::QueryInterface()==>IShellPropSheetExt\r\n");

        *ppv = (LPSHELLPROPSHEETEXT)this;
    }*/
    
    if (*ppv)
    {
        AddRef();

        return NOERROR;
    }

    ODS("CShellExt::QueryInterface()==>Unknown Interface!");

    return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CLocateShellExtension::AddRef()
{
	
    ODS("CShellExt::AddRef()");

	char szNum[100];
	StringCbPrintf(szNum,100,"m_cRef is after this call %d",m_cRef+1);
	DebugMessage(szNum);
    
	return ++m_cRef;
}

STDMETHODIMP_(ULONG) CLocateShellExtension::Release()
{
    ODS("CShellExt::Release()");

	char szNum[100];
	StringCbPrintf(szNum,100,"m_cRef is after this call %d",m_cRef-1);
	DebugMessage(szNum);

    if (--m_cRef)
        return m_cRef;

    DebugMessage("delete this");
	delete this;

    return 0L;
}

STDMETHODIMP CLocateShellExtension::Initialize(LPCITEMIDLIST pIDFolder,
                                   LPDATAOBJECT pDataObj,
                                   HKEY hRegKey)
{
	DebugFormatMessage("CLocateShellExtension::Initialize(%X,%X,%X)",pIDFolder,pDataObj,hRegKey);

	if (pDataObj==NULL)
		return E_INVALIDARG;

	FORMATETC FormatEtc;
	STGMEDIUM med;
	FormatEtc.cfFormat=RegisterClipboardFormat(CFSTR_SHELLIDLIST);
	FormatEtc.ptd=NULL;
	FormatEtc.dwAspect=DVASPECT_CONTENT;
	FormatEtc.lindex=-1;
    FormatEtc.tymed=TYMED_HGLOBAL;
	
	if (pDataObj->GetData(&FormatEtc,&med)==S_OK)
	{
		// May be computer?
		IShellFolder *psf;
		if (!SUCCEEDED(SHGetDesktopFolder(&psf)))
		{
			DebugMessage("No IShellFolder interface");
			return E_FAIL;
		}
		
		// Retrieving data from item id list
		BYTE* pData=(BYTE*)GlobalLock(med.hGlobal);
		
		UINT nFiles=((int*)pData)[0];
		UINT i,nLines=1,nSubIDListLen,nIDListLen;
		WCHAR szPath[_MAX_PATH];

		nSubIDListLen=GetIDListSize((LPITEMIDLIST)(pData+((int*)pData)[1]))-2;
		
		for (i=0;i<nFiles;i++)
		{
			nIDListLen=GetIDListSize((LPITEMIDLIST)(pData+((int*)pData)[2+i]));

			// Constructing IDList from parent item and item
            LPITEMIDLIST pItemIDList=(LPITEMIDLIST)new BYTE[nSubIDListLen+nIDListLen+2];
			CopyMemory((BYTE*)pItemIDList,(LPCSTR)(pData+((int*)pData)[1]),nSubIDListLen);
			CopyMemory((BYTE*)(pItemIDList)+nSubIDListLen,(LPCSTR)(pData+((int*)pData)[2+i]),nIDListLen);
			
			if (SHGetPathFromIDListW(pItemIDList,szPath))
			{
				DebugFormatMessage(L"SHGetPathFromIDListW gives %s",szPath);

				if (IsDirectory(szPath))
				{
					WCHAR szTarget[MAX_PATH];
					switch (GetNethoodTarget(szPath,szTarget,MAX_PATH))
					{
					case 1:
						m_aComputers.Add(alloccopy(szTarget));
						break;
					case 2:
						m_aDirectories.Add(alloccopy(szTarget));
						break;
					case 0:
						m_aDirectories.Add(alloccopy(szPath));
						break;
					}
				}
				else
					m_aFiles.Add(alloccopy(szPath));
			}
			else
			{
				DebugHexDump("idlist",(BYTE*)pItemIDList,nSubIDListLen+nIDListLen);
				
				STRRET str;
				if (SUCCEEDED(psf->GetDisplayNameOf(pItemIDList,SHGDN_NORMAL|SHGDN_FORPARSING,&str)))
				{
					if (SUCCEEDED(StrRetToBufW(&str,pItemIDList,szPath,MAX_PATH)))
					{
						if (szPath[0]!='\\' && szPath[1]!='\\')
							DebugFormatMessage("Cannot add item %s",szPath);
						else
							m_aComputers.Add(alloccopy(szPath));
					}
					else
						DebugMessage("StrRetToStr failed");
				}
				else
					DebugMessage("GetDisplayNameOf failed");

			}
			
			delete[] (BYTE*)pItemIDList;
		}
		
		GlobalUnlock(med.hGlobal);
		if (med.pUnkForRelease==NULL)
            GlobalFree(med.hGlobal);
		else 
			med.pUnkForRelease->Release();
	
		psf->Release();
	}
	else 
	{
		FormatEtc.cfFormat=CF_HDROP;
		if (m_pDataObj->GetData(&FormatEtc,&med)==S_OK)
		{
			LPDROPFILES df=(LPDROPFILES)GlobalLock(med.hGlobal);
			if (df->fWide)
			{
				LPWSTR lpPtr=(LPWSTR)((BYTE*)df+df->pFiles);
				SIZE_T nLen;
				while (nLen=wcslen(lpPtr))
				{
					DebugFormatMessage(L"HDROP contains file %s",(LPCSTR)(lpPtr));
					if (IsDirectory(lpPtr))
					{
						WCHAR szTarget[MAX_PATH];
						switch (GetNethoodTarget(lpPtr,szTarget,MAX_PATH))
						{
						case 1:
							m_aComputers.Add(alloccopy(szTarget));
							break;
						case 2:
							m_aDirectories.Add(alloccopy(szTarget));
							break;
						case 0:
							m_aDirectories.Add(alloccopy(lpPtr,nLen));
							break;
						}
					}
					else
						m_aFiles.Add(alloccopy(lpPtr,nLen));
					
					lpPtr+=nLen+1;
				}
			}
			else
			{
				LPSTR lpPtr=(LPSTR)df+df->pFiles;
				SIZE_T nLen;
				while (nLen=strlen(lpPtr))
				{
					LPWSTR lpFile=alloccopyAtoW(lpPtr,nLen);					

					if (IsDirectory(lpFile))
						m_aDirectories.Add(lpFile);
					else
						m_aFiles.Add(lpFile);

					lpPtr+=nLen+1;
				}
			}	
			
			GlobalUnlock(med.hGlobal);
			
			if (med.pUnkForRelease==NULL)
                GlobalFree(med.hGlobal);
			else 
				med.pUnkForRelease->Release();
		}
	}

	
    // Initialize can be called more than once

    if (m_pDataObj)
        m_pDataObj->Release();

    // duplicate the object pointer and registry handle

    if (pDataObj)
    {
        m_pDataObj = pDataObj;
        pDataObj->AddRef();
    }

    return NOERROR;
}
