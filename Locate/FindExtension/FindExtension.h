//
// Locate Shell Extension
// Copyright 2005-2007 Janne Huttunen
//
// class id:  8A2F2E09-3209-4934-ACB1-DE5BB052950F
//
//


#ifndef _FINDEXT_H
#define _FINDEXT_H



// {8A2F2E09-3209-4934-ACB1-DE5BB052950F}
DEFINE_GUID(CLSID_ShellExtension, 
0x8a2f2e09, 0x3209, 0x4934, 0xac, 0xb1, 0xde, 0x5b, 0xb0, 0x52, 0x95, 0xf);


#ifdef _LOGGING
void DebugMessage(LPCSTR szMessage);
#else 
#define DebugMessage
#endif
#define ODS(sz) DebugMessage(sz)


// this class factory object creates context menu handlers for Windows 95 shell
class CShellExtClassFactory : public IClassFactory
{
protected:
    ULONG   m_cRef;

public:
    CShellExtClassFactory();
    ~CShellExtClassFactory();

    //IUnknown members
	STDMETHODIMP            QueryInterface(REFIID, LPVOID FAR *);
    STDMETHODIMP_(ULONG)    AddRef();
    STDMETHODIMP_(ULONG)    Release();

    //IClassFactory members
    STDMETHODIMP        CreateInstance(LPUNKNOWN, REFIID, LPVOID FAR *);
    STDMETHODIMP        LockServer(BOOL);

};
typedef CShellExtClassFactory *LPCSHELLEXTCLASSFACTORY;

// this is the actual OLE Shell context menu handler
class CFindExtension : public IContextMenu,
                         IShellExtInit
                         //IExtractIcon,
                         //IPersistFile,
                         //IShellPropSheetExt,
                         //ICopyHook
{
protected:
    ULONG        m_cRef;

    STDMETHODIMP OpenLocate(HWND hParent,LPCSTR pszWorkingDir,LPCSTR pszCmd,LPCSTR pszParam,int iShowCmd);

public:
    CFindExtension();
    ~CFindExtension();

    //IUnknown members
    STDMETHODIMP            QueryInterface(REFIID, LPVOID FAR *);
    STDMETHODIMP_(ULONG)    AddRef();
    STDMETHODIMP_(ULONG)    Release();

    //IShell members
    STDMETHODIMP            QueryContextMenu(HMENU hMenu,
                                             UINT indexMenu,
                                             UINT idCmdFirst,
                                             UINT idCmdLast,
                                             UINT uFlags);

    STDMETHODIMP            InvokeCommand(LPCMINVOKECOMMANDINFO lpcmi);

    STDMETHODIMP            GetCommandString(UINT_PTR idCmd,
                                             UINT uFlags,
                                             UINT FAR *reserved,
                                             LPSTR pszName,
                                             UINT cchMax);

    //IShellExtInit methods
    STDMETHODIMP            Initialize(LPCITEMIDLIST pIDFolder,
                                       LPDATAOBJECT pDataObj,
                                       HKEY hKeyID);

    
};

typedef CFindExtension *LPCSHELLEXT;



#define ResultFromShort(i)  ResultFromScode(MAKE_SCODE(SEVERITY_SUCCESS, 0, (USHORT)(i)))


#endif // _SHELLEXT_H
