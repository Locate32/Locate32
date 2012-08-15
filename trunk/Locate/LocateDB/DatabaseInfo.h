/* Copyright (c) 1997-2010 Janne Huttunen
   database updater v3.1.9.6070              */

#if !defined(DATABASEINFO_H)
#define DATABASEINFO_H

#if _MSC_VER >= 1000
#pragma once
#endif

class CDatabaseInfo
{
public:
	enum RootType {
		Unknown=0,
		Fixed=0x10,
		Removable=0x20,
		CDRom=0x30,
		Remote=0x40,
		Ramdisk=0x50,
		Directory=0xF0
	};
		
	struct CRoot
	{
		CRoot() : rtType(Unknown) {}
		CRoot(char drive) 
		:	sPath(drive),dwVolumeSerial(0),dwNumberOfFiles(DWORD(-1)),
			dwNumberOfDirectories(DWORD(-1)),rtType(Unknown) 
		{
			sPath <<':';
		}
		
		CStringW sPath;
		CStringW sVolumeName;
		DWORD dwVolumeSerial;
		CStringW sFileSystem;
		DWORD dwNumberOfFiles;
		DWORD dwNumberOfDirectories;
		RootType rtType;
		CStringW sRootMap;
	};
	
protected:
	CDatabaseInfo();

	BOOL GetInfo(const CDatabase* pDatabase);
	BOOL GetInfo(CDatabase::ArchiveType nArchiveType,LPCWSTR szArchivePath,LPCWSTR szRootMaps);

public:
	~CDatabaseInfo();

	BYTE bVersion;
	BYTE bLongFilenames;
	enum Charset { OEM,	Ansi,Unicode } cCharset;
		
	CStringW sCreator;
	CStringW sDescription;
	CTime tCreationTime;
	DWORD dwNumberOfFiles;
	DWORD dwNumberOfDirectories;
	CArrayFP<CRoot*> aRootFolders;
	DWORD dwFileSize;

	CStringW sExtra1;
	CStringW sExtra2;

public:
	static CDatabaseInfo* GetFromDatabase(const CDatabase* pDatabase);
	static CDatabaseInfo* GetFromFile(LPCWSTR szArchivePath);
	static void GetFromFile(LPCWSTR szArchivePath,CDatabaseInfo*& pInfo);
		
	static BOOL GetRootsFromDatabase(CArray<LPWSTR>& aRoots,const CDatabase* pDatabase);
	
	// Fonctions do not clear aRoots
	static BOOL GetRootsFromDatabases(CArray<LPWSTR>& aRoots,const PDATABASE* pDatabases,int nDatabases,BOOL bOnlyEnabled=FALSE);
	static BOOL GetRootsFromDatabases(CArray<LPWSTR>& aRoots,const CArray<CDatabase*>& aDatabases,BOOL bOnlyEnabled=FALSE);

	static BOOL ReadFilesAndDirectoriesCount(CDatabase::ArchiveType,LPCWSTR szArchive,DWORD& dwFiles,DWORD& dwDirectories);

private:
	// We keep these variables as members, because GetInfo may take long time and it can be terminated
	CFile* m_pFile;
	BYTE* m_szBuffer;
	
};

inline 	CDatabaseInfo::CDatabaseInfo()
:	bVersion(0), bLongFilenames(0),dwNumberOfDirectories(DWORD(-1)),dwNumberOfFiles(DWORD(-1)),m_pFile(NULL),m_szBuffer(NULL)
{
}


inline CDatabaseInfo* CDatabaseInfo::GetFromDatabase(const CDatabase* pDatabase)
{
	CDatabaseInfo* pRet=new CDatabaseInfo;
	
	if (!pRet->GetInfo(pDatabase))
	{
		delete pRet;
		return NULL;
	}
	return pRet;
}

inline CDatabaseInfo* CDatabaseInfo::GetFromFile(LPCWSTR szArchivePath)
{
	CDatabaseInfo* pRet=new CDatabaseInfo;
	
	if (!pRet->GetInfo(CDatabase::archiveFile,szArchivePath,NULL))
	{
		delete pRet;
		return NULL;
	}
	return pRet;
}

inline void CDatabaseInfo::GetFromFile(LPCWSTR szArchivePath,CDatabaseInfo*& pInfo)
{
	pInfo=new CDatabaseInfo;
	
	if (!pInfo->GetInfo(CDatabase::archiveFile,szArchivePath,NULL))
	{
		delete pInfo;
		pInfo=NULL;
	}
}

inline BOOL CDatabaseInfo::GetInfo(const CDatabase* pDatabase)
{
	BOOL bFree;
	LPWSTR pFile=pDatabase->GetResolvedArchiveName(bFree);
	BOOL bRet=GetInfo(pDatabase->GetArchiveType(),pFile,pDatabase->GetRootMaps());
	if (bFree)
		delete[] pFile;
	return bRet;
} 


inline BOOL CDatabaseInfo::GetRootsFromDatabases(CArray<LPWSTR>& aRoots,const CArray<CDatabase*>& aDatabases,BOOL bOnlyEnabled)
{
	return GetRootsFromDatabases(aRoots,aDatabases,aDatabases.GetSize(),bOnlyEnabled);
}


#endif