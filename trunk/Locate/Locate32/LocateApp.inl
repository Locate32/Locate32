/* Locate32 - Copyright (c) 1997-2010 Janne Huttunen */

#if !defined(LOCATEAPP_INL)
#define LOCATEAPP_INL

#if _MSC_VER >= 1000
#pragma once
#endif


////////////////////////////////////////////////////////////
// CLocateApp

inline void CLocateApp::SetStartData(CStartData* pStarData)
{
	if (m_pStartData!=NULL)
		delete m_pStartData;
	m_pStartData=pStarData;
}

inline CDatabase* CLocateApp::GetDatabaseNonConst(WORD wID) const
{
	if (m_pLastDatabase->GetID()==wID)
		return m_pLastDatabase;

	for (int i=0;i<m_aDatabases.GetSize();i++)
	{
		m_pLastDatabase=m_aDatabases[i];
		if (m_pLastDatabase->GetID()==wID)
			return m_pLastDatabase;
	}
	return NULL;
}

inline const CDatabase* CLocateApp::GetDatabase(WORD wID) const
{
	return GetDatabaseNonConst(wID);
}

inline BOOL CLocateApp::IsDatabaseMenu(HMENU hMenu)
{
	UINT nID=GetMenuItemID(hMenu,0);
	return nID>=IDM_DEFUPDATEDBITEM && nID<IDM_DEFUPDATEDBITEM+1000;
}


inline BOOL CLocateApp::IsUpdating() const
{
	return m_ppUpdaters!=NULL;
}

inline DWORD CLocateApp::GetProgramFlags()
{
	extern CLocateApp theApp;
	return theApp.m_dwProgramFlags;
}



////////////////////////////////////////////////////////////
// CLocateApp::CStartData


inline CLocateApp::CStartData::CStartData()
:	m_nStatus(0),m_nStartup(0),m_dwMaxFoundFiles(DWORD(-1)),
	m_dwMaxFileSize(DWORD(-1)),m_dwMinFileSize(DWORD(-1)),
    m_dwMaxDate(DWORD(-1)),m_dwMinDate(DWORD(-1)),
    m_cMaxSizeType(0),m_cMinSizeType(0),
    m_cMaxDateType(0),m_cMinDateType(0),
    m_nSorting(BYTE(-1)),m_nPriority(priorityDontChange),
	m_pStartPath(NULL),m_pStartString(NULL),
	m_pTypeString(NULL),m_pFindText(NULL),m_pLoadPreset(NULL),
	m_nActivateInstance(0),m_nListMode(BYTE(-1)),
	m_nActivateControl(None)
{ 
}


inline CLocateApp::CStartData::~CStartData()
{
	if (m_pStartPath!=NULL)
		delete[] m_pStartPath;
	if (m_pStartString!=NULL)
		delete[] m_pStartString;
	if (m_pTypeString!=NULL)
		delete[] m_pTypeString;
	if (m_pFindText!=NULL)
		delete[] m_pFindText;
	if (m_pLoadPreset!=NULL)
		delete[] m_pLoadPreset;
}



////////////////////////////////////////////////////////////
// Global functions related to CLocateApp

inline CLocateApp* GetLocateApp()
{
	extern CLocateApp theApp;
	return &theApp;
}

inline CTrayIconWnd* GetTrayIconWnd()
{
	extern CLocateApp theApp;
	return (CTrayIconWnd*)&theApp.m_AppWnd;
}


////////////////////////////////////////////////////////////
// CRekKey2 - These two function access to theApp object 
// and need to be here

inline CString CRegKey2::GetCommonKey()
{
	extern CLocateApp theApp;
	return CString(theApp.m_szCommonRegKey)+="\\Locate32";
}

inline CStringW CRegKey2::GetCommonKeyW()
{
	extern CLocateApp theApp;
	return CStringW(theApp.m_szCommonRegKey)+="\\Locate32";
}


#endif