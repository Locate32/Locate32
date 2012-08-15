//
// Locate Shell Extension
// Copyright 2005 Janne Huttunen
//
// class id:  73ACE856-AD8F-4346-9A07-775E45221BC8
//
//


#ifndef _SHELLEXT_H
#define _SHELLEXT_H



DEFINE_GUID(CLSID_ShellExtension, 0x73ACE856L, 0xAD8F, 0x4346, 0x9A, 0x07, 0x77, 0x5E, 0x45, 0x22, 0x1B, 0xC8 );


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
class CLocateShellExtension : public IContextMenu,
                         IShellExtInit
                         //IExtractIcon,
                         //IPersistFile,
                         //IShellPropSheetExt,
                         //ICopyHook
{
protected:
    ULONG        m_cRef;
    LPDATAOBJECT m_pDataObj;
    
	CArrayFAP<LPWSTR> m_aComputers;
	CArrayFAP<LPWSTR> m_aDirectories;
	CArrayFAP<LPWSTR> m_aFiles;
    
	STDMETHODIMP DoGAKMenu1(HWND hParent,
                            LPCSTR pszWorkingDir,
                            LPCSTR pszCmd,
                            LPCSTR pszParam,
                            int iShowCmd);

	enum Flags {
		fSubMenu = 0x1,

		fDidnSuccess = 0x8000, // Used by ReadSettingsFromRegistry if failed
		fDefaults = 0
	};
public:
    CLocateShellExtension();
    ~CLocateShellExtension();

    //IUnknown members
    STDMETHODIMP            QueryInterface(REFIID, LPVOID FAR *);
    STDMETHODIMP_(ULONG)    AddRef();
    STDMETHODIMP_(ULONG)    Release();

    //IShell members
	STDMETHODIMP			Initialize(LPCITEMIDLIST pIDFolder,
                                   LPDATAOBJECT pDataObj,
                                   HKEY hRegKey);

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
private:
    DWORD ReadSettingsFromRegistry();
	int InsertMenuItems(HMENU hMenu,UINT nStartIndex,UINT idCmdFirst,UINT& nHighestUsedID);

};
typedef CLocateShellExtension *LPCSHELLEXT;



#define ResultFromShort(i)  ResultFromScode(MAKE_SCODE(SEVERITY_SUCCESS, 0, (USHORT)(i)))


#endif // _SHELLEXT_H
