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

#include "../common/win95crtfixes.h"

#define ResultFromShort(i)  ResultFromScode(MAKE_SCODE(SEVERITY_SUCCESS, 0, (USHORT)(i)))


//
// Initialize GUIDs (should be done only and at-least once per DLL/EXE)
//
#pragma data_seg(".text")
#define INITGUID
#include <initguid.h>
#include <shlguid.h>
#include "FindExtension.h"
#pragma data_seg()

//
// Global variables
//
UINT      g_cRefThisDll = 0;    // Reference count of this DLL.
HINSTANCE g_hmodThisDll = NULL; // Handle to this DLL itself.

#ifdef _LOGGING
HANDLE hOutput=NULL;

void DebugMessage(LPCSTR szMessage)
{
	if (hOutput==NULL)
	{
		hOutput=CreateFile("c:\\loc_fndx.log",GENERIC_WRITE,
			FILE_SHARE_READ,NULL,
			OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		if (hOutput!=INVALID_HANDLE_VALUE)
		{
			SetFilePointer(hOutput,0,NULL,FILE_END);
			DWORD dwWrite;
			WriteFile(hOutput,"Logging started\r\n",17,&dwWrite,NULL);
		}
		else
		{
			hOutput=NULL;
			return;
		}
	}

	DWORD dwWritten;
	DWORD dwLen=(DWORD)strlen(szMessage);
	WriteFile(hOutput,szMessage,dwLen,&dwWritten,NULL);
	
	if (dwLen>0 && szMessage[dwLen-1]!='\n')
		WriteFile(hOutput,"\r\n",2,&dwWritten,NULL);

	FlushFileBuffers(hOutput);

}
#endif

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        
        // Extension DLL one-time initialization

        g_hmodThisDll = hInstance;

		ODS("In DLLMain, DLL_PROCESS_ATTACH\r\n");
    }
    else if (dwReason == DLL_PROCESS_DETACH)
    {
        ODS("In DLLMain, DLL_PROCESS_DETACH\r\n");

#ifdef _LOGGING		
		if (hOutput!=NULL)
		{
			DebugMessage("End of logging");
			CloseHandle(hOutput);
			hOutput=NULL;
		}
#endif
    }

    return 1;   // ok
}

//---------------------------------------------------------------------------
// DllCanUnloadNow
//---------------------------------------------------------------------------

STDAPI DllCanUnloadNow(void)
{
    ODS("In DLLCanUnloadNow\r\n");

    return (g_cRefThisDll == 0 ? S_OK : S_FALSE);
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppvOut)
{
    ODS("In DllGetClassObject\r\n");

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
    ODS("CShellExtClassFactory::CShellExtClassFactory()\r\n");

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
    ODS("CShellExtClassFactory::QueryInterface()\r\n");

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
    ODS("CShellExtClassFactory::AddRef()\r\n");

	char szNum[100];
	wsprintf(szNum,"m_cRef is after this call %d",m_cRef+1);
	DebugMessage(szNum);

	return ++m_cRef;
}

STDMETHODIMP_(ULONG) CShellExtClassFactory::Release()
{
	ODS("CShellExtClassFactory::Release()\r\n");

	char szNum[100];
	wsprintf(szNum,"m_cRef is after this call %d",m_cRef-1);
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
    ODS("CShellExtClassFactory::CreateInstance()\r\n");

    *ppvObj = NULL;

    // Shell extensions typically don't support aggregation (inheritance)

    if (pUnkOuter)
        return CLASS_E_NOAGGREGATION;

    // Create the main shell extension object.  The shell will then call
    // QueryInterface with IID_IShellExtInit--this is how shell extensions are
    // initialized.

    LPCSHELLEXT pShellExt = new CFindExtension();  //Create the CShellExt object

    if (NULL == pShellExt)
        return E_OUTOFMEMORY;

    return pShellExt->QueryInterface(riid, ppvObj);
}


STDMETHODIMP CShellExtClassFactory::LockServer(BOOL fLock)
{
    return NOERROR;
}

// *********************** CShellExt *************************
CFindExtension::CFindExtension()
{
    ODS("CShellExt::CShellExt()\r\n");

    m_cRef = 0L;

    g_cRefThisDll++;
}

CFindExtension::~CFindExtension()
{
    g_cRefThisDll--;
}

STDMETHODIMP CFindExtension::QueryInterface(REFIID riid, LPVOID FAR *ppv)
{
    ODS("CFindExtension::QueryInterface()");
	
	*ppv = NULL;

    if (IsEqualIID(riid, IID_IShellExtInit) || IsEqualIID(riid, IID_IUnknown))
    {
        ODS("CFindExtension::QueryInterface()==>IID_IShellExtInit\r\n");

        *ppv = (LPSHELLEXTINIT)this;
    }
    else if (IsEqualIID(riid, IID_IContextMenu))
    {
        ODS("CFindExtension::QueryInterface()==>IID_IContextMenu\r\n");

        *ppv = (LPCONTEXTMENU)this;
    }
 
    
    if (*ppv)
    {
        AddRef();

        return NOERROR;
    }

    ODS("CFindExtension::QueryInterface()==>Unknown Interface!\r\n");

    return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CFindExtension::AddRef()
{
	
    ODS("CFindExtension::AddRef()\r\n");

	char szNum[100];
	wsprintf(szNum,"m_cRef is after this call %d",m_cRef+1);
	DebugMessage(szNum);
    
	return ++m_cRef;
}

STDMETHODIMP_(ULONG) CFindExtension::Release()
{
    ODS("CFindExtension::Release()\r\n");

	char szNum[100];
	wsprintf(szNum,"m_cRef is after this call %d",m_cRef-1);
	DebugMessage(szNum);

    if (--m_cRef)
        return m_cRef;

    DebugMessage("delete this");
	delete this;

    return 0L;
}

STDMETHODIMP CFindExtension::Initialize(LPCITEMIDLIST pIDFolder,
                                   LPDATAOBJECT pDataObj,
                                   HKEY hRegKey)
{
	ODS("CFindExtension::Initialize()\r\n");
	
    return NOERROR;
}
