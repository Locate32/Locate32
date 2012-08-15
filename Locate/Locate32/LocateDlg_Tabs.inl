/* Locate32 - Copyright (c) 1997-2010 Janne Huttunen */

#if !defined(LOCATEDLG_TABS_INL)
#define LOCATEDLG_TABS_INL

#if _MSC_VER >= 1000
#pragma once
#endif


//////////////////////////////////////////////////////////////////////////////
// CLocateDlg::CNameDlg
//////////////////////////////////////////////////////////////////////////////


inline CLocateDlg::CNameDlg::CNameDlg()
:	CDialog(IDD_NAME),m_nMaxBrowse(DEFAULT_NUMBEROFDIRECTORIES),
	m_nMaxNamesInList(DEFAULT_NUMBEROFNAMES),
	m_nMaxTypesInList(DEFAULT_NUMBEROFTYPES),
	m_pBrowse(NULL),m_pMultiDirs(NULL),bStopDebug(FALSE)
{
	InitializeCriticalSection(&m_cBrowse);
}

inline WORD CLocateDlg::CNameDlg::ComputeChecksumFromDir(LPCWSTR szDir)
{				
	WORD wCheksum=0;
	for (int j=0;szDir[j]!=L'\0';j++)
		wCheksum+=WORD(szDir[j]*(j+1));
	return wCheksum;
}


inline void CLocateDlg::CNameDlg::ParseGivenDirectoryForMultipleDirectories(CArray<LPWSTR>& aDirectories,LPCWSTR szDirectory,DWORD sLen)
{
	LPWSTR pDirectories=alloccopy(szDirectory,sLen);
	ParseGivenDirectoryForMultipleDirectories(aDirectories,pDirectories);
	delete[] pDirectories;
}

inline void CLocateDlg::CNameDlg::HilightTab(BOOL bHilight)
{
	GetLocateDlg()->HilightTab(0,IDS_NAME,bHilight);
}


//////////////////////////////////////////////////////////////////////////////
// CLocateDlg::CNameDlg::DirSelection
//////////////////////////////////////////////////////////////////////////////


inline CLocateDlg::CNameDlg::DirSelection::DirSelection(BYTE bSelected_)
:	nType(CLocateDlg::CNameDlg::NotSelected),pTitleOrDirectory(NULL),bSelected(bSelected_)
{
}

inline CLocateDlg::CNameDlg::DirSelection::~DirSelection()
{
	FreeData();
}

inline void CLocateDlg::CNameDlg::DirSelection::FreeData()
{
	if (pTitleOrDirectory!=NULL)
	{
		delete[] pTitleOrDirectory;
		pTitleOrDirectory=NULL;
	}
}

//////////////////////////////////////////////////////////////////////////////
// CLocateDlg::CSizeDateDlg
//////////////////////////////////////////////////////////////////////////////


inline CLocateDlg::CSizeDateDlg::CSizeDateDlg()
:	CDialog(IDD_SIZEDATE)
{
}

inline BOOL CLocateDlg::CSizeDateDlg::IsChanged()
{
	return (IsDlgButtonChecked(IDC_CHECKMINIMUMSIZE) ||
			IsDlgButtonChecked(IDC_CHECKMAXIMUMSIZE) ||
			IsDlgButtonChecked(IDC_CHECKMINDATE) ||
			IsDlgButtonChecked(IDC_CHECKMAXDATE));
}

inline void CLocateDlg::CSizeDateDlg::HilightTab(BOOL bHilight)
{
	GetLocateDlg()->HilightTab(1,IDS_SIZEDATE,bHilight);
}

inline BOOL CLocateDlg::CSizeDateDlg::LookOnlyFiles() const
{
	return IsDlgButtonChecked(IDC_CHECKMINIMUMSIZE) ||
		IsDlgButtonChecked(IDC_CHECKMAXIMUMSIZE);
}



//////////////////////////////////////////////////////////////////////////////
// CLocateDlg::CAdvancedDlg
//////////////////////////////////////////////////////////////////////////////

inline CLocateDlg::CAdvancedDlg::CAdvancedDlg()
:	CDialog(IDD_ADVANCED),m_hTypeUpdaterThread(NULL),m_hDefaultTypeIcon(NULL),m_dwFlags(fgDefault)
{
}


inline CLocateDlg::CAdvancedDlg::CReplaceCharsDlg::CReplaceCharsDlg(CArrayFAP<LPWSTR>& raChars,BOOL bUseQuestionMark)
:	CDialog(IDD_REPLACECHARS),m_raChars(raChars),m_bUseQuestionMark(bUseQuestionMark)
{
}

inline void CLocateDlg::CAdvancedDlg::HilightTab(BOOL bHilight)
{
	GetLocateDlg()->HilightTab(2,IDS_ADVANCED,bHilight);
}



//////////////////////////////////////////////////////////////////////////////
// CLocateDlg::CAdvancedDlg::FileType
//////////////////////////////////////////////////////////////////////////////

inline CLocateDlg::CAdvancedDlg::FileType::FileType()
:	szExtensions(NULL),szTitle(NULL),szType(NULL),hIcon(NULL),szIconPath(NULL)
{
}
			
inline CLocateDlg::CAdvancedDlg::FileType::FileType(LPWSTR frType,LPWSTR frTitle)
:	szExtensions(NULL),szTitle(frTitle),szType(frType),szIconPath(NULL),hIcon(NULL)
{
}



#endif