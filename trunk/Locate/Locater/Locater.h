/* Copyright (c) 1997-2010 Janne Huttunen
   database locater v3.1.10.8220              */

#if !defined(LOCATER_H)
#define LOCATER_H

#if _MSC_VER >= 1000
#pragma once
#endif

#include "../locatedb/definitions.h"


//#define LocaterDebugMessage(a) DebugMessage(a)
//#define LocaterDebugNumMessage(a,b) DebugFormatMessage(a,(DWORD)(b))
//#define LocaterDebugMessage5(a,b,c,d,e,f,g) DebugFormatMessage(a,b,c,d,e,f,g)

#define LocaterDebugMessage(a)
#define LocaterDebugNumMessage(a,b) 
#define LocaterDebugMessage5(a,b,c,d,e,f,g)


class CLocater;
		
/* dwInfo is:
BeginningDatabase:	(DWORD)DB mame
Otherwise:			number of found files
*/

typedef BOOL (CALLBACK* LOCATEPROC)(DWORD_PTR dwParam,CallingReason crReason,UpdateError ueError,DWORD_PTR dwInfo,const CLocater* pLocater);
typedef BOOL (CALLBACK* LOCATEFOUNDPROC)(DWORD_PTR dwParam,BOOL bFolder,const CLocater* pLocater);

#define LOCATE_FILENAMES				0x00000001
#define LOCATE_FOLDERNAMES				0x00000002
#define LOCATE_NOSUBDIRECTORIES			0x00000010
#define LOCATE_MINCREATIONDATE			0x00000020
#define LOCATE_MINACCESSDATE			0x00000040
#define LOCATE_MAXCREATIONDATE			0x00000080
#define LOCATE_MAXACCESSDATE			0x00000100
#define LOCATE_CONTAINTEXTISMATCHCASE	0x00000200
#define LOCATE_EXTENSIONWITHNAME		0x00000400
#define LOCATE_NAMEREGULAREXPRESSION	0x00000800
#define LOCATE_CHECKWHOLEPATH			0x00001000
#define LOCATE_REGULAREXPRESSIONSEARCH  0x00002000 // Contain data is regular expression
#define LOCATE_NAMEREGEXPISUTF8			0x00004000 
#define LOCATE_NAMEMATCHCASE			0x00008000 
#define LOCATE_LOGICALOPERATIONS		0x00010000 
#define LOCATE_REGEXPCASESENSITIVE		0x00020000 // Regular expression for name is case sensitive
#define LOCATE_LOGICALOPERATIONSINEXT	0x00100000 // Extensions may contain - to mean that exclude extension

#define SYSTEMTIMETODOSDATE(st)	((((st).wDay&0x1F))|(((st).wMonth&0x0F)<<5)|((((st).wYear-1980))<<9))
#define DOSDATETODAY(dt)		BYTE((dt)&0x1F)
#define DOSDATETOMONTH(dt)		BYTE(((dt)>>5)&0x0F)
#define DOSDATETOYEAR(dt)		(WORD((dt)>>9)+1980)
#define DOSTIMETOSECOND(dt)		(BYTE((dt)&0x1F)*2)
#define DOSTIMETOMINUTE(dt)		BYTE(((dt)>>5)&0x3F)
#define DOSTIMETO24HOUR(dt)		BYTE((dt)>>11)
inline BYTE DOSTIMETO12HOUR(WORD dt)
{
	BYTE t=DOSTIMETO24HOUR(dt);
	if (t==0)
		return 12;
	if (t>12)
		return t-12;
	return t;
}

#include "../locatedb/Database.h"

void InitLocaterLibrary();

class CLocater
{
public:
	CLocater();
	CLocater(LPCWSTR szDatabaseFile);
	CLocater(const PDATABASE* pDatabases,int nDatabases);
	CLocater(const CDatabase* pDatabases,int nDatabases);
	CLocater(const CArray<PDATABASE>& aDatabases);
	

	~CLocater();
	
	void SetDatabases(LPCWSTR szDatabaseFile);
	void SetDatabases(const PDATABASE* pDatabases,int nDatabases);
	void SetDatabases(const CDatabase* pDatabases,int nDatabases);
	void SetDatabases(const CArray<PDATABASE>& aDatabases);
	

protected:
	// Thread procs
	static DWORD WINAPI LocateThreadProc(LPVOID lpParameter);
	BOOL LocatingProc();

public:

	HANDLE GetThreadHandle(BOOL bDuplicate);
	
	void SetFunctions(LOCATEPROC pProc,LOCATEFOUNDPROC pFoundProc,LOCATEFOUNDPROC pFoundProcW,DWORD_PTR dwParam=0);
	
	// Creates new thread and start to locate files
	/*BOOL LocateFiles(BOOL bThreaded,LPCSTR* szName,DWORD dwNames,
		LPCSTR* szExtensions,DWORD nExtensions,
		LPCSTR* szDirectories,DWORD nDirectories); */
	BOOL LocateFiles(BOOL bThreaded,LPCWSTR* szName,DWORD dwNames,
		LPCWSTR* szExtensions,DWORD nExtensions,
		LPCWSTR* szDirectories,DWORD nDirectories); 
	BOOL LocateFiles(BOOL bThreaded,LPCWSTR szRegExp,BOOL bCaseSensitive,
		LPCWSTR* szDirectories,DWORD nDirectories); 
	

#if defined(WIN32) & !defined(LOCATER_NOTHREAD)
	BOOL StopLocating();
	//void CouldStop();
#endif

	void SetSizeAndDate(DWORD dwFlags,
		ULONGLONG ullMinSize=(ULONGLONG)-1, // -1 is no min size
		ULONGLONG ullMaxSize=(ULONGLONG)-1, // -1 is no max size
		WORD wMinDate=(WORD)-1, // DOS format, -1 is no mindate
		WORD wMaxDate=(WORD)-1 // DOS format, -1 is no maxdata
	);
	void SetAdvanced(DWORD dwFlags,
		BYTE* pContainData=NULL,
		DWORD dwContainDataLength=0,
		DWORD dwMaxFoundFiles=(DWORD)-1 // -1 is no limit
	);
	

	DWORD GetAdvancedFlags() const;
	DWORD SetAdvancedFlags(DWORD dwFlags);
	DWORD AddAdvancedFlags(DWORD dwNewFlag);
    DWORD RemoveAdvancedFlags(DWORD dwRemoveFlag);


private:
	void LocateValidFolder(DWORD nPathLen);
	void CheckFolder(DWORD nPathLen);
	void LocateValidFolderW(DWORD nPathLen);
	void CheckFolderW(DWORD nPathLen);

	//BOOL SetDirectoriesAndStartToLocate(BOOL bThreaded,LPCSTR* szDirectories,DWORD nDirectories);
	BOOL SetDirectoriesAndStartToLocate(BOOL bThreaded,LPCWSTR* szDirectories,DWORD nDirectories);
	
	template<class CHARTYPE> void Check(CHARTYPE* test) const;

	BOOL IsFileNameWhatAreWeLookingFor() const;
	BOOL IsFileNameWhatAreWeLookingForW() const;
	BOOL IsFolderNameWhatAreWeLookingFor() const;
	BOOL IsFolderNameWhatAreWeLookingForW() const;
	
	BOOL IsFileAdvancedWhatAreWeLookingFor() const;
	BOOL IsFileAdvancedWhatAreWeLookingForW() const;
	BOOL IsFolderAdvancedWhatAreWeLookingFor() const;
	BOOL IsFolderAdvancedWhatAreWeLookingForW() const;

	BOOL IsDirectoryExcluded(DWORD nPathLen) const;
	BOOL IsDirectoryExcludedW(DWORD nPathLen) const;
	

	enum ValidType {
		NoValidFolders,
		SomeValidFolders,
		ValidFolders
	};
	ValidType IsFolderValid(DWORD nPathLen);
	ValidType IsFolderValidW(DWORD nPathLen);

public:
	// These are possible to call only in pFoundProc
	LPCSTR GetFolderName() const;
	DWORD GetFolderNameLen() const;
	WORD GetFolderModifiedTime() const;
	WORD GetFolderModifiedDate() const;
	WORD GetFolderCreatedDate() const;
	WORD GetFolderCreatedTime() const;
	WORD GetFolderAccessedDate() const;
	WORD GetFolderAccessedTime() const;
	BYTE GetFolderAttributes() const;

	LPCSTR GetFileName() const;
	DWORD GetFileNameLen() const;
	BYTE GetFileExtensionPos() const;
	ULONGLONG GetFileSize() const;
	DWORD GetFileSizeLo() const;
	WORD GetFileSizeHi() const;
	WORD GetFileModifiedTime() const;
	WORD GetFileModifiedDate() const;
	WORD GetFileCreatedDate() const;
	WORD GetFileCreatedTime() const;
	WORD GetFileAccessedDate() const;
	WORD GetFileAccessedTime() const;
	BYTE GetFileAttributes() const;
	BOOL HaveFileExtension() const;

	

	// These are possible to call only in pFoundProcW
	LPCWSTR GetFolderNameW() const;
	WORD GetFolderModifiedTimeW() const;
	WORD GetFolderModifiedDateW() const;
	WORD GetFolderCreatedDateW() const;
	WORD GetFolderCreatedTimeW() const;
	WORD GetFolderAccessedDateW() const;
	WORD GetFolderAccessedTimeW() const;
	
	LPCWSTR GetFileNameW() const;
	ULONGLONG GetFileSizeW() const;
	DWORD GetFileSizeLoW() const;
	WORD GetFileSizeHiW() const;
	WORD GetFileModifiedTimeW() const;
	WORD GetFileModifiedDateW() const;
	WORD GetFileCreatedDateW() const;
	WORD GetFileCreatedTimeW() const;
	WORD GetFileAccessedDateW() const;
	WORD GetFileAccessedTimeW() const;
	BOOL HaveFileExtensionW() const;

	

	LPCSTR GetCurrentPath() const;
	LPCWSTR GetCurrentPathW() const;
	DWORD GetCurrentPathLen() const;
	

	DWORD GetNumberOfResults() const;
	DWORD GetNumberOfFoundFiles() const;
	DWORD GetNumberOfFoundDirectories() const;
	
	WORD GetCurrentDatabaseID() const;
	LPCWSTR GetCurrentDatabaseName() const;
	LPCWSTR GetCurrentDatabaseFile() const;
	BOOL IsCurrentDatabaseUnicode() const;
	WORD GetCurrentDatabaseRootID() const;
	BYTE GetCurrentDatabaseRootType() const;
	LPCWSTR GetCurrentDatabaseVolumeLabel() const;
	DWORD GetCurrentDatabaseVolumeSerial() const;
	LPCWSTR GetCurrentDatabaseFileSystem() const;

	// allocates memory to szName & copies database name to szName
	void GetCurrentDatabaseName(LPWSTR& szName) const; 

	LPCWSTR GetLastErrorDatabaseName() const;

	DWORD GetNumberOfDatabases() const;

	void IgnoreThisResult(BOOL bIsFolder) const; // Use in LOCATEFOUNDPROC

private:
	// Locate information
	union {
		struct {
			LPWSTR* m_ppNames;
			int* m_piNameLengths;
			DWORD m_dwNamesCount;
			LPWSTR* m_ppExtensions;
			int* m_piExtLengths;
			DWORD m_dwExtCount;
			DWORD m_dwExclusiveExtCount;
		};
		struct {
#ifdef _PCRE_H
			pcre* m_regexp;
			pcre_extra* m_regextra;
#else
			void* m_regexp;
			void* m_regextra;
#endif
		};
	};
	CArrayFP<CStringW*> m_aDirectories;
	CArrayFP<CStringW*> m_aExcludedDirectories;

	//  Needed information from CDatabase
	struct DBArchive {
		DBArchive(LPCWSTR szName,CDatabase::ArchiveType nArchiveType,
			LPCWSTR szArchive,WORD wID,LPCWSTR szRootMaps);
		~DBArchive();
		
		LPWSTR szName;
		DWORD dwNameLength;

		LPWSTR szArchive;
		CDatabase::ArchiveType nArchiveType;

		WORD wID;

		BYTE bUnicode:1;

		LPCWSTR szRootMaps;

	};
	CArrayFP<DBArchive*> m_aDatabases;
	DBArchive* m_pCurrentDatabase;
	DBArchive* m_pLastErrorDatabase;
	WORD m_wCurrentDatabaseID;
	WORD m_wCurrentRootIndex;
	
	// Volume information
	BYTE m_bCurrentRootType;
	DWORD m_dwVolumeSerial;
	CStringW m_sVolumeName;
	CStringW m_sFileSystem;


private:
	// Advanced
	DWORD m_dwMaxFoundFiles;

	
	// File size and date
	ULONGLONG m_ullMinSize;
	ULONGLONG m_ullMaxSize;
	DWORD m_dwFlags;
	WORD m_wMinDate;
	WORD m_wMaxDate;

private:
	// Functions
	LOCATEPROC m_pProc;
	LOCATEFOUNDPROC m_pFoundProc;
	LOCATEFOUNDPROC m_pFoundProcW;
	DWORD_PTR m_dwData;
	CSearchFromFile* m_pContentSearcher;
	CFile* dbFile;

private:
	mutable DWORD m_dwFoundFiles;
	mutable DWORD m_dwFoundDirectories;

#ifdef WIN32
	HANDLE m_hThread;
	volatile LONG m_lForceQuit;
#endif

	BYTE* szBuffer;
	BYTE* pPoint;
	union {
		mutable char szCurrentPath[MAX_PATH];
		mutable WCHAR szCurrentPathW[MAX_PATH];
	};
	union {
		char szCurrentPathLower[MAX_PATH];
		WCHAR szCurrentPathLowerW[MAX_PATH];
	};

	
	DWORD dwCurrentPathLen;

};

// String copyers
#define sMemCopy(dst,src,len)	CopyMemory(dst,src,len)
#define sMemZero(dst,len)		ZeroMemory(dst,len)
#define sMemSet(dst,val,len)	FillMemory(dst,len,val)
#define sstrlen(str,len)		dstrlen(str,len)

#define sstrlenW				dwstrlen

#include "Locater.inl"


#endif