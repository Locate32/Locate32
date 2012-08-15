/* Copyright (c) 1997-2010 Janne Huttunen
   database updater v3.1.9.6070		*/

#if !defined(DATABASE_H)
#define DATABASE_H

#if _MSC_VER >= 1000
#pragma once
#endif


//#define UpdDebugMessage(a) DebugMessage(a)
//#define UpdDebugFormatMessage1(a,b) DebugFormatMessage(a,b)
//#define UpdDebugFormatMessage2(a,b,c) DebugFormatMessage(a,b,c)
//#define UpdDebugFormatMessage5(a,b,c,d,e,f,g) DebugFormatMessage(a,b,c,d,e,f,g)

#define UpdDebugMessage(a)
#define UpdDebugFormatMessage1(a,b) 
#define UpdDebugFormatMessage2(a,b,c) 
#define UpdDebugFormatMessage5(a,b,c,d,e,f,g)



class CDatabase;
typedef CDatabase* PDATABASE;

class CDatabase
{
public:
	enum DatabaseFlags {
		flagEnabled=0x1,
		flagGlobalUpdate=0x2,
		flagStopIfRootUnavailable=0x4,
		flagIncrementalUpdate=0x8,
		flagAnsiCharset=0x10, // Unicode is default
		flagScanSymLinksAndJunctions=0x20,
		
		
		// If some directory is chosen to be excluded, exclude only the content of directory and add directory
		flagExcludeContentOfDirsOnly=0x40, 
	};
		
	enum ArchiveType {
		archiveFile
	};

private:
    CDatabase();

public:
	
	// Copyer
	CDatabase(CDatabase& src);
	
	~CDatabase();

	

	WORD GetID() const;
	LPCWSTR GetName() const;
	LPCWSTR GetCreator() const;
	LPCWSTR GetDescription() const;
	LPCWSTR GetRoots() const;
	LPCWSTR GetRootMaps() const;
	void GetRoots(CArray<LPWSTR>& aRoots) const;
	WORD GetFlags() const;
	WORD GetThreadId() const;
	
	void SetFlags(WORD wFlags);
	void SetThreadId(WORD wThreadID);

	void SetNamePtr(LPWSTR szName);
	void SetCreatorPtr(LPWSTR szCreator);
	void SetDescriptionPtr(LPWSTR szDescription);

	void SetRootsPtr(LPWSTR szRoots);
	void SetRoots(CArray<LPWSTR>& aRoots);
	void SetRoots(LPWSTR* pRoots,int nCount);
	void SetRootMapsPtr(LPWSTR szRootMaps);
	void AddRoot(LPCWSTR pRoot);
	void AddLocalRoots();

	BOOL IsEnabled() const;
	BOOL IsGloballyUpdated() const;
	
	BOOL IsFlagSet(DatabaseFlags nFlag) const;
	void SetFlag(DatabaseFlags nFlag,BOOL bSet=TRUE);

	void Enable(BOOL bEnable=TRUE);
	void UpdateGlobally(BOOL bUpdate=TRUE);

	ArchiveType GetArchiveType() const;
	LPCWSTR GetArchiveName(BOOL bCanReturnNULL=FALSE) const;
	LPWSTR GetResolvedArchiveName(BOOL& bFree) const;

	void SetArchiveName(LPCWSTR szArchiveName);
	void SetArchiveNamePtr(LPWSTR szArchiveName);
	void SetArchiveType(ArchiveType nType);

	LPSTR GetValidKey(DWORD dwUniqueNum=0) const; // free returned pointer with delete[]

	LPCWSTR GetIncludedFiles() const;
	LPCWSTR GetIncludedDirectories() const;
	LPCWSTR GetExcludedFiles() const;
	void SetIncludedFiles(LPCWSTR szIncludedFiles);
	void SetIncludedDirectories(LPCWSTR szIncludedDirectories);
	void SetExcludedFiles(LPCWSTR szExcludedFiles);

	const CArrayFAP<LPWSTR>& GetExcludedDirectories() const;
	void SetExcludedDirectories(const CArrayFAP<LPWSTR>& aExcludedDirectories);
	BOOL AddExcludedDirectory(LPCWSTR szDirectory);


	BOOL IsFileNamesOEM() const;

	BOOL DoDatabaseFileExist() const;

	

private:
	BOOL SaveToRegistry(HKEY hKeyRoot,LPCSTR szPath,LPCSTR szKey);

public:
	LPWSTR ConstructExtraBlock(DWORD* pdwLen=NULL) const;
	BOOL SaveExtraBlockToDbFile(LPCWSTR szArchive);


public:
	static BOOL LoadFromRegistry(HKEY hKeyRoot,LPCSTR szPath,CArray<PDATABASE>& aDatabases); // Returns the number or retrieved dbs
	static INT LoadFromRegistry(HKEY hKeyRoot,LPCSTR szPath,PDATABASE*& ppDatabases); // Returns the number or retrieved dbs
	static BOOL SaveToRegistry(HKEY hKeyRoot,LPCSTR szPath,const CArray<PDATABASE>& aDatabases);
	static BOOL SaveToRegistry(HKEY hKeyRoot,LPCSTR szPath,const PDATABASE* ppDatabases,int nDatabases);

	static CDatabase* FromName(HKEY hKeyRoot,LPCSTR szPath,LPCWSTR szName,DWORD dwNameLength=-1);
	static CDatabase* FromKey(HKEY hKeyRoot,LPCSTR szPath,LPCSTR szKey);
	static CDatabase* FromFile(LPCWSTR szFileName,int dwNameLength=-1);
    static CDatabase* FromOldStyleDatabase(HKEY hKeyRoot,LPCSTR szPath);
	static CDatabase* FromDefaults(BOOL bDefaultFileName);
	static CDatabase* FromExtraBlock(LPCWSTR szExtraBlock);
	
	static void CheckValidNames(CArray<PDATABASE>& aDatabases);
	static void CheckValidNames(PDATABASE* ppDatabases,int nDatabases);
	static void CheckDoubleNames(CArray<PDATABASE>& aDatabases);
	static void CheckDoubleNames(PDATABASE* ppDatabases,int nDatabases);
	static WORD CheckIDs(CArray<PDATABASE>& aDatabases);
	static WORD CheckIDs(PDATABASE* ppDatabases,int nDatabases);
	
	static BOOL IsNameValid(LPCWSTR szName);
	static void MakeNameValid(LPWSTR szName);

	// Return correct file name (new allocated string), or NULL if not valid filename
	static LPWSTR GetCorrertFileName(LPCWSTR szFileName,DWORD dwNameLength=-1);

	// Replaces %locatedir% with actual path. Returns szFileName or new allocated string (if changed). 
	static LPWSTR ResolveLocateDir(LPCWSTR szFileName,DWORD dwNameLength);

	static WORD GetUniqueIndex(CArray<PDATABASE>& aDatabases);
	static WORD GetUniqueIndex(PDATABASE* ppDatabases,int nDatabases);


	static CDatabase* FindByName(CArray<PDATABASE>& aDatabases,LPCWSTR szName,int iLength=-1);
	static CDatabase* FindByName(PDATABASE* ppDatabases,int nDatabases,LPCWSTR szName,int iLength=-1);

	static CDatabase* FindByFile(CArray<PDATABASE>& aDatabases,LPCWSTR szFile,int iLength=-1);
	static CDatabase* FindByFile(PDATABASE* ppDatabases,int nDatabases,LPCWSTR szFile,int iLength=-1);

	static void GetLogicalDrives(CArrayFAP<LPWSTR>* paRoots);

	
	LPCWSTR FindRootMap(LPCWSTR szRoot,int& iRetLen) const;
	LPCWSTR FindActualPathForMap(LPCWSTR szMap,int& iRetLen,BOOL& bFree) const;
	static LPCWSTR FindRootMap(LPCWSTR szRootMaps,LPCWSTR szRoot,int& iRetLen);
	static LPCWSTR FindActualPathForMap(LPCWSTR szRootMaps,LPCWSTR szMap,int& iRetLen,BOOL& bFree);
	



private:
	LPWSTR m_szName;
	WORD m_wFlags;
	WORD m_wThread;
	WORD m_wID;

	LPWSTR m_szCreator;    
	LPWSTR m_szDescription;

	LPWSTR m_szRoots; // Roots included, NULL means all local
	LPWSTR m_szRootMaps; // string is of the form drive>path_in_db|drive>path_in_db|...
	
	// Archive type, at that moment only file is supported
	// in future, possiple types may be tcp/ip stream, SQL, ...
	ArchiveType m_ArchiveType;
	LPWSTR m_szArchiveName;

	LPWSTR m_szIncludedFiles; // Include files pattern, empty=include all
	LPWSTR m_szIncludedDirectories; // Include files pattern, empty=include all
	LPWSTR m_szExcludedFiles; // Exclude files pattern
	CArrayFAP<LPWSTR> m_aExcludedDirectories;
};


// Inline functions
inline CDatabase::CDatabase()
// Default values:
:	m_szCreator(NULL),m_szDescription(NULL),m_szRoots(NULL),
	m_wFlags(flagEnabled|flagGlobalUpdate|flagStopIfRootUnavailable),m_wThread(0),m_szArchiveName(NULL),
	m_ArchiveType(archiveFile),m_wID(0),
	m_szIncludedFiles(NULL),m_szIncludedDirectories(NULL),
	m_szExcludedFiles(NULL),m_szRootMaps(NULL)
{
	if (!IsUnicodeSystem())
		m_wFlags|=flagAnsiCharset;
}

inline CDatabase::~CDatabase()
{
	if (m_szName!=NULL)
		delete[] m_szName;
	if (m_szDescription!=NULL)
		delete[] m_szDescription;
	if (m_szCreator!=NULL)
		delete[] m_szCreator;
	if (m_szArchiveName!=NULL)
		delete[] m_szArchiveName;
	if (m_szRoots!=NULL)
		delete[] m_szRoots;
	if (m_szIncludedFiles!=NULL)
		delete[] m_szIncludedFiles;
	if (m_szIncludedDirectories!=NULL)
		delete[] m_szIncludedDirectories;
	if (m_szExcludedFiles!=NULL)
		delete[] m_szExcludedFiles;
	if (m_szRootMaps!=NULL)
		delete[] m_szRootMaps;
}

inline LPCWSTR CDatabase::GetName() const 
{ 
	return m_szName; 
}

inline LPCWSTR CDatabase::GetRoots() const 
{ 
	return m_szRoots; 
}

inline LPCWSTR CDatabase::GetRootMaps() const 
{ 
	return m_szRootMaps; 
}

inline LPCWSTR CDatabase::GetCreator() const 
{
	if (m_szCreator==NULL)
		return szwEmpty;
	return m_szCreator; 
}

inline LPCWSTR CDatabase::GetDescription() const 
{ 
	if (m_szDescription==NULL)
		return szwEmpty;
	return m_szDescription; 
}

inline WORD CDatabase::GetFlags() const 
{ 
	return m_wFlags; 
}

inline WORD CDatabase::GetThreadId() const 
{ 
	return m_wThread; 
}

inline CDatabase::ArchiveType CDatabase::GetArchiveType() const 
{ 
	return m_ArchiveType; 
}

inline LPCWSTR CDatabase::GetArchiveName(BOOL bCanReturnNULL) const 
{ 
	if (!bCanReturnNULL && m_szArchiveName==NULL)
		szwEmpty;
	return m_szArchiveName; 
}

inline LPWSTR CDatabase::GetResolvedArchiveName(BOOL& bFree) const
{
	if (m_szArchiveName==NULL)
	{
		bFree=FALSE;
		return (LPWSTR)szwEmpty;
	}
	LPWSTR szResolved=ResolveLocateDir(m_szArchiveName,DWORD(-1));
	if (szResolved==NULL || szResolved==m_szArchiveName)
	{
		bFree=FALSE;
		return m_szArchiveName;
	}
	bFree=TRUE;
	return szResolved;
}

inline INT CDatabase::LoadFromRegistry(HKEY hKeyRoot,LPCSTR szPath,CDatabase**& ppDatabases)
{
	CArray<CDatabase*> aDatabases;
	INT nDBs=LoadFromRegistry(hKeyRoot,szPath,aDatabases);
	if (nDBs)
		ppDatabases=aDatabases;
	return nDBs;
}

inline BOOL CDatabase::SaveToRegistry(HKEY hKeyRoot,LPCSTR szPath,const CArray<CDatabase*>& aDatabases)
{
	return SaveToRegistry(hKeyRoot,szPath,aDatabases,aDatabases.GetSize());
}

inline void CDatabase::CheckValidNames(CArray<PDATABASE>& aDatabases)
{
	CheckValidNames(aDatabases,aDatabases.GetSize());
}

inline void CDatabase::CheckDoubleNames(CArray<PDATABASE>& aDatabases)
{
	CheckDoubleNames(aDatabases,aDatabases.GetSize());
}

inline WORD CDatabase::CheckIDs(CArray<PDATABASE>& aDatabases)
{
	return CheckIDs(aDatabases,aDatabases.GetSize());
}

inline void CDatabase::Enable(BOOL bEnable)
{
	if (bEnable)
		m_wFlags|=flagEnabled;
	else
		m_wFlags&=~flagEnabled;
}

inline void CDatabase::UpdateGlobally(BOOL bUpdate)
{
	if (bUpdate)
		m_wFlags|=flagGlobalUpdate;
	else
		m_wFlags&=~flagGlobalUpdate;
}
	
inline void CDatabase::SetFlags(WORD wFlags)
{
	m_wFlags=wFlags;
}

inline void CDatabase::SetThreadId(WORD wThreadID)
{
	m_wThread=wThreadID;
}

inline BOOL CDatabase::IsEnabled() const
{
	return (m_wFlags&flagEnabled)?1:0;
}

inline BOOL CDatabase::IsGloballyUpdated() const
{
	return (m_wFlags&flagGlobalUpdate)?1:0;
}

inline void CDatabase::SetNamePtr(LPWSTR szName)
{
	if (m_szName!=NULL)
		delete[] m_szName;
	m_szName=szName;
}

inline void CDatabase::SetCreatorPtr(LPWSTR szCreator)
{
	if (m_szCreator!=NULL)
		delete[] m_szCreator;
	m_szCreator=szCreator;
}

inline void CDatabase::SetDescriptionPtr(LPWSTR szDescription)
{
	if (m_szDescription!=NULL)
		delete[] m_szDescription;
	m_szDescription=szDescription;
}

inline void CDatabase::SetArchiveName(LPCWSTR szArchiveName)
{
	if (m_szArchiveName!=NULL)
		delete[] m_szArchiveName;
	ASSERT_VALID(szArchiveName);
	m_szArchiveName=alloccopy(szArchiveName);
}

inline void CDatabase::SetArchiveNamePtr(LPWSTR szArchiveName)
{
	if (m_szArchiveName!=NULL)
		delete[] m_szArchiveName;
	m_szArchiveName=szArchiveName;
}

inline void CDatabase::SetArchiveType(ArchiveType nType)
{
	m_ArchiveType=nType;
}

inline void CDatabase::SetIncludedFiles(LPCWSTR szIncludedFiles)
{
	if (m_szIncludedFiles!=NULL)
		delete[] m_szIncludedFiles;
	if (szIncludedFiles[0]!='\0')
		m_szIncludedFiles=alloccopy(szIncludedFiles);
	else
		m_szIncludedFiles=NULL;
}

inline void CDatabase::SetIncludedDirectories(LPCWSTR szIncludedDirectories)
{
	if (m_szIncludedDirectories!=NULL)
		delete[] m_szIncludedDirectories;
	if (szIncludedDirectories[0]!='\0')
		m_szIncludedDirectories=alloccopy(szIncludedDirectories);
	else
		m_szIncludedDirectories=NULL;
}

inline void CDatabase::SetExcludedFiles(LPCWSTR szExcludedFiles)
{
	if (m_szExcludedFiles!=NULL)
		delete[] m_szExcludedFiles;
	if (szExcludedFiles[0]!='\0')
		m_szExcludedFiles=alloccopy(szExcludedFiles);
	else
		m_szExcludedFiles=NULL;
}


inline void CDatabase::SetRootsPtr(LPWSTR szRoots)
{
	if (m_szRoots!=NULL)
		delete[] m_szRoots;
	m_szRoots=szRoots;
}

inline void CDatabase::SetRootMapsPtr(LPWSTR szRootMaps)
{
	if (m_szRootMaps!=NULL)
		delete[] m_szRootMaps;
	m_szRootMaps=szRootMaps;
}

inline void CDatabase::SetRoots(CArray<LPWSTR>& aRoots)
{
	return SetRoots(aRoots,aRoots.GetSize());
}

inline CDatabase* CDatabase::FindByName(CArray<PDATABASE>& aDatabases,LPCWSTR szName,INT iLength)
{
	return FindByName(aDatabases,aDatabases.GetSize(),szName,iLength);
}

inline CDatabase* CDatabase::FindByFile(CArray<PDATABASE>& aDatabases,LPCWSTR szFile,INT iLength)
{
	return FindByFile(aDatabases,aDatabases.GetSize(),szFile,iLength);
}

inline WORD CDatabase::GetID() const
{
	return m_wID;
}

inline WORD CDatabase::GetUniqueIndex(CArray<PDATABASE>& aDatabases)
{
	return GetUniqueIndex(aDatabases,aDatabases.GetSize());
}
	
inline BOOL CDatabase::DoDatabaseFileExist() const
{
	switch (m_ArchiveType)
	{
	case archiveFile:
		{
			ASSERT_VALID(m_szArchiveName);
			BOOL bFree;
			LPWSTR pFile=GetResolvedArchiveName(bFree);
			BOOL bRet=FileSystem::IsFile(pFile);
			if (bFree)
				delete[] pFile;
			return bRet;
		}
	}
	return FALSE;
}

inline BOOL CDatabase::IsFlagSet(DatabaseFlags nFlag) const
{
	return (m_wFlags&nFlag)?TRUE:FALSE;
}

inline void CDatabase::SetFlag(DatabaseFlags nFlag,BOOL bSet)
{
	if (bSet)
		m_wFlags|=nFlag;
	else
		m_wFlags&=~WORD(nFlag);
}

inline LPCWSTR CDatabase::GetIncludedFiles() const
{
	return m_szIncludedFiles;
}

inline LPCWSTR CDatabase::GetIncludedDirectories() const
{
	return m_szIncludedDirectories;
}

inline LPCWSTR CDatabase::GetExcludedFiles() const
{
	return m_szExcludedFiles;
}

inline const CArrayFAP<LPWSTR>& CDatabase::GetExcludedDirectories() const
{
	return m_aExcludedDirectories;
}

inline void CDatabase::SetExcludedDirectories(const CArrayFAP<LPWSTR>& aExcludedDirectories)
{
	m_aExcludedDirectories.RemoveAll();
	for (int i=0;i<aExcludedDirectories.GetSize();i++)
		m_aExcludedDirectories.Add(alloccopy(aExcludedDirectories[i]));
}

inline BOOL CDatabase::AddExcludedDirectory(LPCWSTR szDirectory)
{
	if (szDirectory[1]!=':' || szDirectory[2]!='\0')
	{
		if (!FileSystem::IsDirectory(szDirectory))
			return FALSE;	
	}
	
	for (int i=0;i<m_aExcludedDirectories.GetSize();i++)
	{
		if (strcasecmp(m_aExcludedDirectories[i],szDirectory)==0)
			return TRUE;
	}
	m_aExcludedDirectories.Add(alloccopy(szDirectory));
	return TRUE;
}


inline LPCWSTR CDatabase::FindRootMap(LPCWSTR szRoot,int& iRetLen) const
{
	return FindRootMap(m_szRootMaps,szRoot,iRetLen);
}

inline LPCWSTR CDatabase::FindActualPathForMap(LPCWSTR szMap,int& iRetLen,BOOL& bFree) const
{
	return FindActualPathForMap(m_szRootMaps,szMap,iRetLen,bFree);
}

	
inline LPCWSTR CDatabase::FindRootMap(LPCWSTR szRootMaps,LPCWSTR szRoot,int& iRetLen)
{
	if (szRootMaps==NULL)
		return NULL;

	for (;;)
	{
		// Get Length
		int nLen=0;
		for (;szRootMaps[nLen]!=L'>' && szRootMaps[nLen]!=L'|';nLen++)
		{
			if (szRootMaps[nLen]=='\0')
				return NULL;
		}

		if (szRootMaps[nLen]==L'|')
		{
			szRootMaps+=nLen+1;
			continue;
		}

		BOOL bFound=_wcsnicmp(szRootMaps,szRoot,nLen)==0;
		if (bFound)		
			bFound=szRoot[nLen]=='\0';

		szRootMaps+=nLen+1;

		for (iRetLen=0;szRootMaps[iRetLen]!='|' && szRootMaps[iRetLen]!='\0';iRetLen++);
		
		if (bFound)
			return szRootMaps;
		
		if (szRootMaps[iRetLen]=='\0')
			return NULL;

		szRootMaps+=iRetLen+1;				
	}
}

inline LPCWSTR CDatabase::FindActualPathForMap(LPCWSTR szRootMaps,LPCWSTR szMap,int& iRetLen,BOOL& bFree)
{
	bFree=FALSE;

	if (szRootMaps==NULL)
		return NULL;

	for (;;)
	{
		// Get Length
		for (iRetLen=0;szRootMaps[iRetLen]!=L'>' && szRootMaps[iRetLen]!=L'|';iRetLen++)
		{
			if (szRootMaps[iRetLen]=='\0')
				return NULL;
		}

		if (szRootMaps[iRetLen]==L'|')
		{
			szRootMaps+=iRetLen+1;
			continue;
		}

		LPCWSTR pActualpath=szRootMaps;
		szRootMaps+=iRetLen+1;

		int nLen=0;
		for (;szRootMaps[nLen]!='|' && szRootMaps[nLen]!='\0';nLen++);

		if (_wcsnicmp(szRootMaps,szMap,nLen)==0)
		{
			if (szMap[nLen]=='\0')
				return pActualpath;

			szMap+=nLen;
			int nMapLen=istrlenw(szMap);
			LPWSTR pRet=new WCHAR[iRetLen+nMapLen+1];
			MemCopyW(pRet,pActualpath,iRetLen);
			MemCopyW(pRet+iRetLen,szMap,nMapLen+1);
			bFree=TRUE;
			iRetLen+=nMapLen;
			return pRet;
		}

		if (szRootMaps[nLen]=='\0')
			return NULL;

		szRootMaps+=nLen+1;				
	}
}



#endif