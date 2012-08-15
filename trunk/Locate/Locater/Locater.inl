/* Copyright (c) 1997-2010 Janne Huttunen
   database locater v3.1.10.8220              */

#if !defined(LOCATER_INL)
#define LOCATER_INL


#ifdef WIN32
#define CLOCATERINITIALIATIONS   \
:	m_dwMaxFoundFiles(DWORD(-1)), \
	m_ullMinSize(ULONGLONG(-1)),m_ullMaxSize(ULONGLONG(-1)), \
	m_wMinDate(WORD(-1)),m_wMaxDate(WORD(-1)), \
	m_dwFlags(LOCATE_FOLDERNAMES),szBuffer(NULL), \
	m_pContentSearcher(NULL), \
	m_hThread(NULL),dbFile(NULL)
#else
#define CLOCATERINITIALIATIONS   \
:	m_dwMaxFoundFiles(DWORD(-1)), \
	m_dwMinSize(DWORD(-1)),m_dwMaxSize(DWORD(-1)), \
	m_wMinDate(WORD(-1)),m_wMaxDate(WORD(-1)), \
	m_dwFlags(LOCATE_FOLDERNAMES),szBuffer(NULL), \
	m_pContentSearcher(NULL),dbFile(NULL)
#endif


inline CLocater::CLocater() 
CLOCATERINITIALIATIONS
{
}

inline CLocater::CLocater(LPCWSTR szDatabaseFile)
CLOCATERINITIALIATIONS
{
	SetDatabases(szDatabaseFile);
}

inline CLocater::CLocater(const PDATABASE* pDatabases,int nDatabases)
CLOCATERINITIALIATIONS
{
	SetDatabases(pDatabases,nDatabases);
}

inline CLocater::CLocater(const CDatabase* pDatabases,int nDatabases)
CLOCATERINITIALIATIONS
{
	SetDatabases(pDatabases,nDatabases);
}

inline CLocater::CLocater(const CArray<PDATABASE>& aDatabases)
CLOCATERINITIALIATIONS
{
	SetDatabases(aDatabases);
}


inline void CLocater::SetFunctions(LOCATEPROC pProc,LOCATEFOUNDPROC pFoundProc,LOCATEFOUNDPROC pFoundProcW,DWORD_PTR dwParam)
{
	m_pProc=pProc;
	m_pFoundProc=pFoundProc;
	m_pFoundProcW=pFoundProcW;
	m_dwData=dwParam;
}

inline void CLocater::SetDatabases(LPCWSTR szDatabaseFile)
{
	m_aDatabases.RemoveAll();
	m_aDatabases.Add(new DBArchive(L"custom",CDatabase::archiveFile,szDatabaseFile,0,NULL));
}

inline void CLocater::SetDatabases(const PDATABASE* pDatabases,int nDatabases)
{
	m_aDatabases.RemoveAll();
	for (int i=0;i<nDatabases;i++)
	{
		if (pDatabases[i]->IsEnabled())
		{
			BOOL bFree;
			LPWSTR pFile=pDatabases[i]->GetResolvedArchiveName(bFree);
			m_aDatabases.Add(new DBArchive(pDatabases[i]->GetName(),
				pDatabases[i]->GetArchiveType(),pFile,
				pDatabases[i]->GetID(),pDatabases[i]->GetRootMaps()));
			if (bFree)
				delete[] pFile;
		}
	}
}

inline void CLocater::SetDatabases(const CDatabase* pDatabases,int nDatabases)
{
	m_aDatabases.RemoveAll();
	for (int i=0;i<nDatabases;i++)
	{
		if (pDatabases[i].IsEnabled())
		{
			BOOL bFree;
			LPWSTR pFile=pDatabases[i].GetResolvedArchiveName(bFree);
			m_aDatabases.Add(new DBArchive(pDatabases[i].GetName(),
				pDatabases[i].GetArchiveType(),pFile,
				pDatabases[i].GetID(),pDatabases[i].GetRootMaps()));
			if (bFree)
				delete[] pFile;
		}
	}
}

inline void CLocater::SetDatabases(const CArray<PDATABASE>& aDatabases)
{
	m_aDatabases.RemoveAll();
	for (int i=0;i<aDatabases.GetSize();i++)
	{
		if (aDatabases[i]->IsEnabled())
		{
			BOOL bFree;
			LPWSTR pFile=aDatabases[i]->GetResolvedArchiveName(bFree);
			m_aDatabases.Add(new DBArchive(aDatabases[i]->GetName(),aDatabases[i]->GetArchiveType(),
				pFile,aDatabases[i]->GetID(),aDatabases[i]->GetRootMaps()));
			if (bFree)
				delete[] pFile;
		}
	}
}



inline void CLocater::SetSizeAndDate(DWORD dwFlags,ULONGLONG ullMinSize,ULONGLONG ullMaxSize,
									 WORD wMinDate,WORD wMaxDate)
{
	m_dwFlags=(m_dwFlags&~(LOCATE_MINCREATIONDATE|LOCATE_MINACCESSDATE|LOCATE_MAXCREATIONDATE|LOCATE_MAXACCESSDATE))|dwFlags;
	m_ullMinSize=ullMinSize;
	m_ullMaxSize=ullMaxSize;
	m_wMinDate=wMinDate;
	m_wMaxDate=wMaxDate;
}


inline bool _strncmp(const WCHAR* p1,const char* p2,int a)
{
	for (a--;a>=0;a--)
		if (p1[a]!=A2Wc(p2[a]))
			return FALSE;
	return TRUE;
}

inline bool _strncmp(const WCHAR* p1,const WCHAR* p2,int a)
{
	for (a--;a>=0;a--)
		if (p1[a]!=p2[a])
			return FALSE;
	return TRUE;
}

inline BOOL CLocater::IsDirectoryExcluded(DWORD nPathLen) const
{
	for (int i=0;i<m_aExcludedDirectories.GetSize();i++)
	{
		if ((DWORD)m_aExcludedDirectories.GetAt(i)->GetLength()<nPathLen)
		{
			if (_strncmp(*m_aExcludedDirectories.GetAt(i),szCurrentPathLower,m_aExcludedDirectories.GetAt(i)->GetLength()) && 
				szCurrentPathLowerW[m_aExcludedDirectories.GetAt(i)->GetLength()]==L'\\')
				return TRUE;
		}
		else if ((DWORD)m_aExcludedDirectories.GetAt(i)->GetLength()==nPathLen)
		{
			if (_strncmp(*m_aExcludedDirectories.GetAt(i),szCurrentPathLower,nPathLen))
				return TRUE;
		}
	}
	return FALSE;
}

inline BOOL CLocater::IsDirectoryExcludedW(DWORD nPathLen) const
{
	for (int i=0;i<m_aExcludedDirectories.GetSize();i++)
	{
		if ((DWORD)m_aExcludedDirectories.GetAt(i)->GetLength()<nPathLen)
		{
			if (_strncmp(*m_aExcludedDirectories.GetAt(i),szCurrentPathLowerW,m_aExcludedDirectories.GetAt(i)->GetLength()) && 
				szCurrentPathLowerW[m_aExcludedDirectories.GetAt(i)->GetLength()]==L'\\')
				return TRUE;
		}
		else if ((DWORD)m_aExcludedDirectories.GetAt(i)->GetLength()==nPathLen)
		{
			if (_strncmp(*m_aExcludedDirectories.GetAt(i),szCurrentPathLowerW,nPathLen))
				return TRUE;
		}
	}
	return FALSE;
}

inline CLocater::ValidType CLocater::IsFolderValid(DWORD nPathLen)
{
	// Check first if root is excluded
	if (IsDirectoryExcluded(nPathLen))
		return NoValidFolders;

	if (m_aDirectories.GetSize()>0)
	{
		for (int i=0;i<m_aDirectories.GetSize();i++)
		{
			if ((DWORD)m_aDirectories[i]->GetLength()>nPathLen)
			{
				if (_strncmp(*m_aDirectories[i],szCurrentPathLower,nPathLen) && 
					m_aDirectories[i]->GetAt(nPathLen)=='\\')
					return SomeValidFolders;
			}
			else if ((DWORD)m_aDirectories[i]->GetLength()==nPathLen)
			{
				if (_strncmp(*m_aDirectories[i],szCurrentPathLower,nPathLen))
					return ValidFolders;
			}
			else // if (m_aDirectories[i]->GetLength()<nPathLen)
			{
				if (_strncmp(*m_aDirectories[i],szCurrentPathLower,m_aDirectories[i]->GetLength()) && 
					szCurrentPathLower[m_aDirectories[i]->GetLength()]==L'\\')
					return ValidFolders;
			}
		}
		return NoValidFolders;
	}
	else
		return ValidFolders;					
}

inline CLocater::ValidType CLocater::IsFolderValidW(DWORD nPathLen)
{
	// Check first if root is excluded
	if (IsDirectoryExcludedW(nPathLen))
		return NoValidFolders;


	// Check if directory is one of wanted
	if (m_aDirectories.GetSize()>0)
	{
		for (int i=0;i<m_aDirectories.GetSize();i++)
		{
			if ((DWORD)m_aDirectories[i]->GetLength()>nPathLen)
			{
				if (_strncmp(*m_aDirectories[i],szCurrentPathLowerW,nPathLen) && 
					m_aDirectories[i]->GetAt(nPathLen)==L'\\')
					return SomeValidFolders;
			}
			else if (m_aDirectories[i]->GetLength()==nPathLen)
			{
				if (_strncmp(*m_aDirectories[i],szCurrentPathLowerW,nPathLen))
					return ValidFolders;
			}
			else // if (m_aDirectories[i]->GetLength()<nPathLen)
			{
				if (_strncmp(*m_aDirectories[i],szCurrentPathLowerW,m_aDirectories[i]->GetLength()) && 
					szCurrentPathLowerW[m_aDirectories[i]->GetLength()]==L'\\')
					return ValidFolders;
			}
		}
		return NoValidFolders;
	}
	else
		return ValidFolders;					
}

inline LPCSTR CLocater::GetFolderName() const
{
	return LPCSTR(pPoint);
}

inline BYTE CLocater::GetFolderAttributes() const
{
	return *(pPoint-6);
}

inline DWORD CLocater::GetFolderNameLen() const
{
	return (DWORD)*(pPoint-1);
}

inline WORD CLocater::GetFolderModifiedTime() const
{
	return *(WORD*)(pPoint+pPoint[-1]+1+2);
}

inline WORD CLocater::GetFolderModifiedDate() const
{
	return *(WORD*)(pPoint+pPoint[-1]+1);
}

inline WORD CLocater::GetFolderCreatedDate() const
{
	return *(WORD*)(pPoint+pPoint[-1]+1+4);
}

inline WORD CLocater::GetFolderCreatedTime() const
{
	return WORD(-1);
}

inline WORD CLocater::GetFolderAccessedDate() const
{
	return *(WORD*)(pPoint+pPoint[-1]+1+4+2);
}

inline WORD CLocater::GetFolderAccessedTime() const
{
	return WORD(-1);
}



inline LPCSTR CLocater::GetFileName() const
{
	return LPCSTR(pPoint+3);
}

inline BYTE CLocater::GetFileAttributes() const
{
	return pPoint[0];
}

inline DWORD CLocater::GetFileNameLen() const
{
	return pPoint[1];
}

inline BYTE CLocater::GetFileExtensionPos() const
{
	return pPoint[2];
}

inline DWORD CLocater::GetFileSizeLo() const
{
	return *(DWORD*)(pPoint+1+1+1+(pPoint[1]+1));
}

inline WORD CLocater::GetFileSizeHi() const
{
	return WORD(*(pPoint+1+1+1+(pPoint[1]+1)+sizeof(DWORD)));
}

inline ULONGLONG CLocater::GetFileSize() const
{
	return ULONGLONG(GetFileSizeHi())<<32|ULONGLONG(GetFileSizeLo());
}
	

inline WORD CLocater::GetFileModifiedDate() const
{
	return *(WORD*)(pPoint+1+1+1+(pPoint[1]+1)+5);
}

inline WORD CLocater::GetFileModifiedTime() const
{
	return *(WORD*)(pPoint+1+1+1+(pPoint[1]+1)+5+2);
}


inline WORD CLocater::GetFileCreatedDate() const
{
	return *(WORD*)(pPoint+1+1+1+(pPoint[1]+1)+5+4);
}

inline WORD CLocater::GetFileCreatedTime() const
{
	return WORD(-1);
}

inline WORD CLocater::GetFileAccessedDate() const
{
	return *(WORD*)(pPoint+1+1+1+(pPoint[1]+1)+5+4+2);
}

inline WORD CLocater::GetFileAccessedTime() const
{
	return WORD(-1);
}


inline BOOL CLocater::HaveFileExtension() const
{
	//return GetFileExtensionPos()!=0 ||*GetFileName()[0]=='.';
	return pPoint[2]!=0 || pPoint[3]=='.';
}



inline LPCWSTR CLocater::GetFolderNameW() const
{
	return LPCWSTR(pPoint);
}


inline WORD CLocater::GetFolderModifiedDateW() const
{
	return *(WORD*)(pPoint+pPoint[-1]*2+2);
}

inline WORD CLocater::GetFolderModifiedTimeW() const
{
	return *(WORD*)(pPoint+pPoint[-1]*2+2+2);
}

inline WORD CLocater::GetFolderCreatedDateW() const
{
	return *(WORD*)(pPoint+pPoint[-1]*2+2+4);
}

inline WORD CLocater::GetFolderCreatedTimeW() const
{
	return *(WORD*)(pPoint+pPoint[-1]*2+2+4+2);
}

inline WORD CLocater::GetFolderAccessedDateW() const
{
	return *(WORD*)(pPoint+pPoint[-1]*2+2+4+4);
}

inline WORD CLocater::GetFolderAccessedTimeW() const
{
	return *(WORD*)(pPoint+pPoint[-1]*2+2+4+4+2);
}

inline LPCWSTR CLocater::GetFileNameW() const
{
	return LPCWSTR(pPoint+3);
}


inline DWORD CLocater::GetFileSizeLoW() const
{
	return *(DWORD*)(pPoint+1+1+1+(pPoint[1]*2+2));
}

inline WORD CLocater::GetFileSizeHiW() const
{
	return *(WORD*)(pPoint+1+1+1+(pPoint[1]*2+2)+sizeof(DWORD));
}

inline ULONGLONG CLocater::GetFileSizeW() const
{
	return ULONGLONG(GetFileSizeHiW())<<32|ULONGLONG(GetFileSizeLoW());
}

inline WORD CLocater::GetFileModifiedDateW() const
{
	return *(WORD*)(pPoint+1+1+1+(pPoint[1]*2+2)+6);
}

inline WORD CLocater::GetFileModifiedTimeW() const
{
	return *(WORD*)(pPoint+1+1+1+(pPoint[1]*2+2)+6+2);
}


inline WORD CLocater::GetFileCreatedDateW() const
{
	return *(WORD*)(pPoint+1+1+1+(pPoint[1]*2+2)+6+4);
}

inline WORD CLocater::GetFileCreatedTimeW() const
{
	return *(WORD*)(pPoint+1+1+1+(pPoint[1]*2+2)+6+4+2);
}

inline WORD CLocater::GetFileAccessedDateW() const
{
	return *(WORD*)(pPoint+1+1+1+(pPoint[1]*2+2)+6+4+4);
}

inline WORD CLocater::GetFileAccessedTimeW() const
{
	return *(WORD*)(pPoint+1+1+1+(pPoint[1]*2+2)+6+4+4+2);
}

inline BOOL CLocater::HaveFileExtensionW() const
{
	//return GetFileExtensionPos()!=0 ||*GetFileName()[0]=='.';
	return pPoint[2]!=0 || *LPCWSTR(pPoint+3)==L'.';
}



	
inline DWORD CLocater::GetNumberOfResults() const
{
	return m_dwFoundFiles+m_dwFoundDirectories;
}

inline DWORD CLocater::GetNumberOfFoundFiles() const
{
	return m_dwFoundFiles;
}

inline DWORD CLocater::GetNumberOfFoundDirectories() const
{
	return m_dwFoundDirectories;
}

inline LPCSTR CLocater::GetCurrentPath() const
{
	return szCurrentPath;
}

inline LPCWSTR CLocater::GetCurrentPathW() const
{
	return szCurrentPathW;
}

inline DWORD CLocater::GetCurrentPathLen() const
{
	return dwCurrentPathLen;
}



inline DWORD CLocater::GetAdvancedFlags() const
{
	return m_dwFlags;
}

inline DWORD CLocater::SetAdvancedFlags(DWORD dwFlags)
{
	return (m_dwFlags=dwFlags);
}

inline DWORD CLocater::AddAdvancedFlags(DWORD dwNewFlag)
{
	return (m_dwFlags|=dwNewFlag);
}

inline DWORD CLocater::RemoveAdvancedFlags(DWORD dwRemoveFlag)
{
	return (m_dwFlags&=~dwRemoveFlag);
}



inline CLocater::DBArchive::DBArchive(LPCWSTR szName_,CDatabase::ArchiveType nArchiveType_,
									  LPCWSTR szArchive_,WORD wID_,LPCWSTR szRootMaps_)
:	nArchiveType(nArchiveType_),wID(wID_),bUnicode(FALSE)
{
	szName=alloccopy(szName_);
	szArchive=alloccopy(szArchive_);
	if (szRootMaps_!=NULL)
		szRootMaps=alloccopy(szRootMaps_);
	else
		szRootMaps=NULL;
}

inline CLocater::DBArchive::~DBArchive()
{
	if (szName!=NULL)
		delete[] szName;
	if (szArchive!=NULL)
		delete[] szArchive;
	if (szRootMaps!=NULL)
		delete[] szRootMaps;
}

inline WORD CLocater::GetCurrentDatabaseID() const
{
	return m_wCurrentDatabaseID;
}

inline WORD CLocater::GetCurrentDatabaseRootID() const
{
	return m_wCurrentRootIndex;
}	
	
inline LPCWSTR CLocater::GetCurrentDatabaseName() const
{
	return m_pCurrentDatabase->szName;
}

inline LPCWSTR CLocater::GetLastErrorDatabaseName() const
{
	if (m_pLastErrorDatabase!=NULL)
		return m_pLastErrorDatabase->szName;
	return NULL;
}

inline void CLocater::GetCurrentDatabaseName(LPWSTR& szName) const
{
	szName=alloccopy(m_pCurrentDatabase->szName,m_pCurrentDatabase->dwNameLength);
}

inline BOOL CLocater::IsCurrentDatabaseUnicode() const
{
	return m_pCurrentDatabase->bUnicode;
}

inline LPCWSTR CLocater::GetCurrentDatabaseFile() const
{
	return m_pCurrentDatabase->szArchive;
}

inline DWORD CLocater::GetNumberOfDatabases() const
{
	return m_aDatabases.GetSize();
}

inline BYTE CLocater::GetCurrentDatabaseRootType() const
{
	return m_bCurrentRootType;
}

inline LPCWSTR CLocater::GetCurrentDatabaseVolumeLabel() const
{
	return m_sVolumeName;
}

inline DWORD CLocater::GetCurrentDatabaseVolumeSerial() const
{
	return m_dwVolumeSerial;
}

inline LPCWSTR CLocater::GetCurrentDatabaseFileSystem() const
{
	return m_sFileSystem;
}

inline void CLocater::IgnoreThisResult(BOOL bIsFolder) const
{
	if (bIsFolder)
		m_dwFoundDirectories--;
	else
		m_dwFoundFiles--;
}

inline HANDLE CLocater::GetThreadHandle(BOOL bDuplicate)
{
	if (m_hThread!=NULL && bDuplicate)
	{
		HANDLE hCopy;
		if (!DuplicateHandle(GetCurrentProcess(),m_hThread,GetCurrentProcess(),
			&hCopy,0,FALSE,DUPLICATE_SAME_ACCESS))
			return NULL;
		return hCopy;
	}
	return m_hThread;
}

#endif
