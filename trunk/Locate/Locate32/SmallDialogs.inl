/* Locate32 - Copyright (c) 1997-2010 Janne Huttunen */

#if !defined(SMALLDIALOGS_INL)
#define SMALLDIALOGS_INL

#if _MSC_VER >= 1000
#pragma once
#endif



////////////////////////////////////////////////////////////
// CSelectColumnsDlg

inline CSelectColumnsDlg::CSelectColumnsDlg()
:	CDialog(IDD_SELECTDETAILS),m_pList(NULL)
{
}

inline CSelectColumnsDlg::ColumnItem::ColumnItem(int nCol,DetailType nType,int nResourceID,
												 int nWidth,Align nAlign,CSubAction** pActions)
:	m_nType(nType),m_nWidth(nWidth),m_nCol(nCol),m_nAlign(nAlign),m_pActions(pActions)
{
	m_strName.LoadString(nResourceID,LanguageSpecificResource);
}

inline CSelectDatabasesDlg::CSelectDatabasesDlg(const CArray<PDATABASE>& rOrigDatabases,CArray<PDATABASE>& rSelectedDatabases,BYTE bFlags,LPCSTR szRegKey)
:	CDialog(IDD_SELECTDATABASES),
	m_rOrigDatabases(rOrigDatabases),m_rSelectedDatabases(rSelectedDatabases),
	m_bFlags(bFlags),m_nThreadsCurrently(1),m_pSelectDatabases(NULL)
{
	if (szRegKey!=NULL)
		m_pRegKey=alloccopy(szRegKey);
	else
		m_pRegKey=NULL;


	if (bFlags&flagEnablePriority)
		m_nThreadPriority=THREAD_PRIORITY_NORMAL;
	else
	{
		m_bUseTemporally=FALSE;
	
		if (m_pRegKey!=NULL)
		{
			CRegKey RegKey;
			if(RegKey.OpenKey(HKCU,m_pRegKey,CRegKey::defRead)==ERROR_SUCCESS)
				RegKey.QueryValue("Use databases temporally",(LPSTR)&m_bUseTemporally,sizeof(DWORD));
		}
	}
}

inline BOOL CSelectDatabasesDlg::IsItemEnabled(const CDatabase* pDatabase)
{
	if (m_bFlags&flagSetUpdateState)
		return pDatabase->IsGloballyUpdated();
	else
		return pDatabase->IsEnabled();
}

inline BOOL CSelectDatabasesDlg::EnableItem(CDatabase* pDatabase,BOOL bEnable)
{
	if (m_bFlags&flagSetUpdateState)
	{
		if (pDatabase->IsGloballyUpdated()==bEnable?1:0)
			return FALSE;
		pDatabase->UpdateGlobally(bEnable);
		DebugFormatMessage("db %S is %s (update)",pDatabase->GetName(),bEnable?"enabled":"disabled");
	}
	else
	{
		if (pDatabase->IsEnabled()==bEnable?1:0)
			return FALSE;
		pDatabase->Enable(bEnable);
		DebugFormatMessage("db %S is %s (locate)",pDatabase->GetName(),bEnable?"enabled":"disabled");
	}
	return TRUE;	
}

inline void CSelectDatabasesDlg::SetThreadPriority(int nThreadPriority)
{
	m_nThreadPriority=nThreadPriority;
}


inline int CSelectDatabasesDlg::GetThreadPriority() const
{
	return m_nThreadPriority;
}


///////////////////////////////////////////////////////////
// CSavePresetDlg

inline CSavePresetDlg::CSavePresetDlg()
:	CDialog(IDD_PRESETNAME)
{
}



///////////////////////////////////////////////////////////
// CSelectDatabasesDlg::CSavePresetDlg

inline CSelectDatabasesDlg::CSavePresetDlg::CSavePresetDlg(CSelectDatabasesDlg* pParent)
:	::CSavePresetDlg(),m_pParent(pParent)
{
}



////////////////////////////////////////////////////////////
// CChangeCaseDlg

inline CChangeCaseDlg::CChangeCaseDlg()
:	CDialog(IDD_CHANGECASE), nSelectedCase(Sentence),bForExtension(FALSE)
{
}



////////////////////////////////////////////////////////////
// CChangeFilenameDlg

inline CChangeFilenameDlg::CChangeFilenameDlg()
:	CDialog(IDD_CHANGEFILENAME),m_dwFlags(0)
{
}




#endif