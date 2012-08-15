/* Locate32 - Copyright (c) 1997-2010 Janne Huttunen */

#if !defined(LOCATEAPPWND_INL)
#define LOCATEAPPWND_INL

#if _MSC_VER >= 1000
#pragma once
#endif

////////////////////////////////////////////////////////////
// CTrayIconWnd

inline BYTE CTrayIconWnd::OnUpdate(BOOL bStopIfProcessing,LPWSTR pDatabases)
{
	DWORD nThreadPriority=THREAD_PRIORITY_NORMAL;

	CRegKey2 RegKey;
	if (RegKey.OpenKey(HKCU,"\\General",CRegKey::defRead)==ERROR_SUCCESS)
		RegKey.QueryValue("Update Process Priority",nThreadPriority);
	
	return OnUpdate(bStopIfProcessing,pDatabases,(int)nThreadPriority);
}

inline CLocateDlg* CTrayIconWnd::GetLocateDlg() const 
{ 
	return m_pLocateDlgThread==NULL?NULL:m_pLocateDlgThread->m_pLocate; 
}


////////////////////////////////////////////////////////////
// CTrayIconWnd::CUpdateStatusWnd

inline void CTrayIconWnd::CUpdateStatusWnd::EnlargeSizeForText(CDC& dc,CStringW& str,CSize& szSize)
{
	EnlargeSizeForText(dc,str,(int)str.GetLength(),szSize);
}

inline void CTrayIconWnd::CUpdateStatusWnd::EnlargeSizeForText(CDC& dc,LPCWSTR szText,int nLength,CSize& szSize)
{
	CRect rc(0,0,0,0);
	dc.DrawText(szText,nLength,&rc,DT_SINGLELINE|DT_CALCRECT);
	if (szSize.cx<rc.Width())
		szSize.cx=rc.Width();
	if (szSize.cy<rc.Height())
		szSize.cy=rc.Height();
}


#endif