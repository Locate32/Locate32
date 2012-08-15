////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////

#ifndef HFCFILE_INL
#define HFCFILE_INL

/////////////////////////////////////////////////
// CFile

#ifdef WIN32
inline CFile::CFile(CFileException* e)
:	m_bCloseOnDelete(FALSE),m_nOpenFlags(0),m_hFile(FILE_NULL),m_pFileException(e)
{
}

inline CFile::CFile(BOOL bThrowExceptions,CFileException* e)
:	m_bCloseOnDelete(FALSE),m_nOpenFlags(0),m_hFile(FILE_NULL),
	CStream(bThrowExceptions),m_pFileException(e)
{
}

inline CFile::CFile(HANDLE hFile,BOOL bThrowExceptions,CFileException* e)
:	m_hFile(hFile),m_bCloseOnDelete(FALSE),m_nOpenFlags(0),
	CStream(bThrowExceptions),m_pFileException(e)
{
}


inline CFile::CFile(LPCSTR lpszFileName,int nOpenFlags,BOOL bThrowExceptions,CFileException* e)
:	m_hFile(FILE_NULL),m_bCloseOnDelete(FALSE),
	CStream(bThrowExceptions),m_pFileException(e)
{
	Open(lpszFileName,nOpenFlags);
}
#endif

#ifdef DEF_WCHAR
inline CFile::CFile(LPCWSTR lpszFileName,int nOpenFlags,BOOL bThrowExceptions,CFileException* e)
:	m_hFile(FILE_NULL),m_bCloseOnDelete(FALSE),
	CStream(bThrowExceptions),m_pFileException(e)
{
	Open(lpszFileName,nOpenFlags);
}
#endif


inline CFile::~CFile()
{
	if (m_hFile!=FILE_NULL && m_bCloseOnDelete)
	{
#ifdef WIN32
		CloseHandle(m_hFile);
#else
		fclose((FILE*)m_hFile);
#endif
		DebugCloseFile(m_hFile);

	}
}

inline CFile::operator HANDLE() const
{
	return m_hFile;
}

#ifndef WIN32
inline CFile::operator FILE*() const
{
	return (FILE*)m_hFile; 
}
#endif

inline CString CFile::GetFilePath() const
{
	return m_strFileName;
}

inline void CFile::SetFilePath(LPCSTR lpszNewName)
{
	m_strFileName=lpszNewName;
}

#ifdef DEF_WCHAR
inline CStringW CFile::GetFilePathW() const
{
	return m_strFileName;
}

inline void CFile::SetFilePath(LPCWSTR lpszNewName)
{
	m_strFileName=lpszNewName;
}
#endif


inline BOOL CFile::Write(const CStringA& str) 
{ 
	return this->Write((LPCSTR)str,(DWORD)(str.GetLength()+(m_nOpenFlags&otherStrNullTerminated?1:0))); 
}

#ifdef DEF_WCHAR
inline BOOL CFile::Write(const CStringW& str) 
{ 
	return this->Write((LPCWSTR)str,(DWORD)(str.GetLength()+(m_nOpenFlags&otherStrNullTerminated?1:0))); 
}
#endif

inline BOOL CFile::Write(LPCSTR szNullTerminatedString) 
{ 
	return this->Write(szNullTerminatedString,((DWORD)istrlen(szNullTerminatedString)+(m_nOpenFlags&otherStrNullTerminated?1:0))); 
}

#ifdef DEF_WCHAR
inline BOOL CFile::Write(LPCWSTR szNullTerminatedString) 
{ 
	return this->Write(szNullTerminatedString,(DWORD)(istrlenw(szNullTerminatedString)+(m_nOpenFlags&otherStrNullTerminated?1:0))); 
}
#endif

inline BOOL CFile::SetLength(DWORD dwNewLen,LONG* pHigh)
{
	this->Seek((LONG)dwNewLen,begin,pHigh);
	return ::SetEndOfFile(m_hFile);
}

inline BOOL CFile::SetLength64(ULONGLONG ullNewLen)
{
	this->Seek64(ullNewLen,begin);
	return ::SetEndOfFile(m_hFile);
}

inline DWORD CFile::Seek64(ULONGLONG lOff, SeekPosition nFrom)
{
	LONG l=(LONG)(lOff>>32);
	this->Seek((LONG)lOff,nFrom,&l);
	return ::SetEndOfFile(m_hFile);
}

inline BOOL CFile::GetFileTime(LPFILETIME lpCreationTime,LPFILETIME lpLastAccessTime,LPFILETIME lpLastWriteTime) const
{
	return ::GetFileTime(m_hFile,lpCreationTime,lpLastAccessTime,lpLastWriteTime);
}

inline BOOL CFile::SetFileTime(const FILETIME* lpCreationTime,const FILETIME* lpLastAccessTime,const FILETIME* lpLastWriteTime)
{
	return ::SetFileTime(m_hFile,lpCreationTime,lpLastAccessTime,lpLastWriteTime);
}

inline DWORD CFile::GetFileType() const
{
	return ::GetFileType(m_hFile);
}

inline BOOL CFile::SetFileShortName(LPCSTR lpShortName)
{
	return ::SetFileShortNameA(m_hFile,lpShortName);
}

#ifdef DEF_WCHAR
inline BOOL CFile::SetFileShortName(LPCWSTR lpShortName)
{
	if (IsUnicodeSystem())
		return ::SetFileShortNameW(m_hFile,lpShortName);
	else
		return ::SetFileShortNameA(m_hFile,W2A(lpShortName));
}
#endif




/////////////////////////////////////////////////
// CFile
#ifdef DEF_WCHAR
inline CFileEncode::CFileEncode(CFileException* e)
:	m_nEncoding(ANSI),CFile(e)
{
}

inline CFileEncode::CFileEncode(BOOL bThrowExceptions,CFileException* e)
:	m_nEncoding(ANSI),CFile(bThrowExceptions,e)
{
}

inline CFileEncode::CFileEncode(HANDLE hFile,BOOL bThrowExceptions,CFileException* e)
:	m_nEncoding(ANSI),CFile(hFile,bThrowExceptions,e)
{
}

inline CFileEncode::CFileEncode(LPCSTR lpszFileName,int nOpenFlags,BOOL bThrowExceptions,CFileException* e)
:	m_nEncoding(ANSI),CFile(lpszFileName,nOpenFlags,bThrowExceptions,e)
{
}

inline CFileEncode::CFileEncode(LPCWSTR lpszFileName,int nOpenFlags,BOOL bThrowExceptions,CFileException* e)
:	m_nEncoding(ANSI),CFile(lpszFileName,nOpenFlags,bThrowExceptions,e)
{
}

#endif

////////////////////////////////////////
// namespace FileSystem

inline BOOL FileSystem::Rename(LPCSTR lpszOldName,LPCSTR lpszNewName)
{
	DWORD GetSystemFeaturesFlag();
#ifdef WIN32
	if (GetSystemFeaturesFlag()&(efWin2000|efWinXP|efWinVista))
		return ::MoveFileA(lpszOldName,lpszNewName);	
	else 
	{
		if (::CopyFileA(lpszOldName,lpszNewName,TRUE))
			return ::DeleteFileA(lpszOldName);
		return FALSE;	
	}
#else
	return (BYTE)_rename(lpszOldName,lpszNewName);	
#endif
}

inline BOOL FileSystem::Remove(LPCSTR lpszFileName)
{
#ifdef WIN32
	return ::DeleteFileA(lpszFileName);
#else
	return (BYTE)::remove(lpszFileName);
#endif
}

#ifdef DEF_WCHAR
inline BOOL FileSystem::Rename(LPCWSTR lpszOldName,LPCWSTR lpszNewName)
{
	if (IsUnicodeSystem())
		return ::MoveFileW(lpszOldName,lpszNewName);
	else
	{
		if (::CopyFileA(W2A(lpszOldName),W2A(lpszNewName),TRUE))
			return ::DeleteFileA(W2A(lpszOldName));
		return FALSE;	
	}
}

inline BOOL FileSystem::Remove(LPCWSTR lpszFileName)
{
	if (IsUnicodeSystem())
		return ::DeleteFileW(lpszFileName);
	else
		return ::DeleteFile(W2A(lpszFileName));
}
#endif

inline BOOL FileSystem::CreateDirectory(LPCSTR lpPathName,LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
	return ::CreateDirectory(lpPathName,lpSecurityAttributes);
}

inline BOOL FileSystem::RemoveDirectory(LPCSTR lpPathName)
{
	return ::RemoveDirectory(lpPathName);
}

inline DWORD FileSystem::GetFileAttributes(LPCSTR lpFileName)
{
	return ::GetFileAttributesA(lpFileName);
}

inline BOOL FileSystem::SetFileAttributes(LPCSTR lpFileName,DWORD dwFileAttributes)
{
	return ::SetFileAttributes(lpFileName,dwFileAttributes);
}

inline DWORD FileSystem::GetFullPathName(LPCSTR lpFileName,DWORD nBufferLength,LPSTR lpBuffer,LPTSTR* lpFilePart)
{
	return ::GetFullPathName(lpFileName,nBufferLength,lpBuffer,lpFilePart);
}

inline DWORD FileSystem::GetShortPathName(LPCSTR lpszLongPath,LPSTR lpszShortPath,DWORD cchBuffer)
{
	return ::GetShortPathName(lpszLongPath,lpszShortPath,cchBuffer);
}

inline DWORD FileSystem::GetCurrentDirectory(DWORD nBufferLength,LPSTR lpBuffer)
{
	return ::GetCurrentDirectory(nBufferLength,lpBuffer);
}

inline DWORD FileSystem::GetLongPathName(LPCSTR lpszShortPath,LPSTR lpszLongPath,DWORD cchBuffer)
{
	return ::GetLongPathName(lpszShortPath,lpszLongPath,cchBuffer);
}
inline DWORD FileSystem::GetTempPath(DWORD nBufferLength,LPSTR lpBuffer)
{
	return ::GetTempPath(nBufferLength,lpBuffer);
}

inline UINT FileSystem::GetTempFileName(LPCSTR lpPathName,LPCSTR lpPrefixString,UINT uUnique,LPSTR lpTempFileName)
{
	return ::GetTempFileName(lpPathName,lpPrefixString,uUnique,lpTempFileName);
}
	
inline short FileSystem::GetFileTitle(LPCSTR lpszFile,LPSTR lpszTitle,WORD cbBuf)
{
	return ::GetFileTitle(lpszFile,lpszTitle,cbBuf);
}
	
inline BOOL FileSystem::MoveFile(LPCSTR lpExistingFileName,LPCSTR lpNewFileName,DWORD dwFlags)
{
	DWORD GetSystemFeaturesFlag();
	if (GetSystemFeaturesFlag()&(efWin2000|efWinXP|efWinVista))
		return ::MoveFileEx(lpExistingFileName,lpNewFileName,dwFlags);	
	else 
	{
		if (::CopyFile(lpExistingFileName,lpNewFileName,TRUE))
			return ::DeleteFile(lpExistingFileName);
		return FALSE;	
	}
}
	
inline BOOL FileSystem::CopyFile(LPCSTR lpExistingFileName,LPCSTR lpNewFileName,BOOL bFailIfExists)
{
	return ::CopyFile(lpExistingFileName,lpNewFileName,bFailIfExists);
}

inline UINT FileSystem::GetDriveType(LPCSTR lpRootPathName)
{
	return ::GetDriveType(lpRootPathName);
}


inline DWORD FileSystem::GetLogicalDriveStrings(DWORD nBufferLength,LPSTR lpBuffer)
{
	return ::GetLogicalDriveStringsA(nBufferLength,lpBuffer);
}

inline BOOL FileSystem::GetVolumeInformation(LPCSTR lpRootPathName,LPSTR lpVolumeNameBuffer,
		DWORD nVolumeNameSize,LPDWORD lpVolumeSerialNumber,LPDWORD lpMaximumComponentLength,
		LPDWORD lpFileSystemFlags,LPSTR lpFileSystemNameBuffer,DWORD nFileSystemNameSize)
{
	return ::GetVolumeInformation(lpRootPathName,lpVolumeNameBuffer,
		nVolumeNameSize,lpVolumeSerialNumber,lpMaximumComponentLength,
		lpFileSystemFlags,lpFileSystemNameBuffer,nFileSystemNameSize);
}



inline BOOL FileSystem::GetFileSecurity(LPCSTR lpFileName,SECURITY_INFORMATION RequestedInformation,
	PSECURITY_DESCRIPTOR pSecurityDescriptor,DWORD nLength,LPDWORD lpnLengthNeeded)
{
	return ::GetFileSecurity(lpFileName,RequestedInformation,pSecurityDescriptor,nLength,lpnLengthNeeded);
}

inline BOOL FileSystem::LookupAccountName(LPCSTR lpSystemName,LPCSTR lpAccountName,PSID Sid,
	LPDWORD cbSid,LPSTR ReferencedDomainName,LPDWORD cchReferencedDomainName,
	PSID_NAME_USE peUse)
{
	return ::LookupAccountName(lpSystemName,lpAccountName,Sid,cbSid,ReferencedDomainName,cchReferencedDomainName,peUse);
}

inline BOOL FileSystem::LookupAccountSid(LPCSTR lpSystemName,PSID lpSid,LPSTR lpName,LPDWORD cchName,
	LPSTR lpReferencedDomainName,LPDWORD cchReferencedDomainName,PSID_NAME_USE peUse)
{
	return ::LookupAccountSid(lpSystemName,lpSid,lpName,cchName,lpReferencedDomainName,cchReferencedDomainName,peUse);
}

inline UINT FileSystem::GetWindowsDirectory(LPSTR lpBuffer,UINT uSize)
{
	return ::GetWindowsDirectoryA(lpBuffer,uSize);
}

inline UINT FileSystem::GetSystemDirectory(LPSTR lpBuffer,UINT uSize)
{
	return ::GetSystemDirectoryA(lpBuffer,uSize);
}

inline HMODULE FileSystem::LoadLibrary(LPCSTR lpFileName,DWORD dwFlags)
{
	return ::LoadLibraryExA(lpFileName,NULL,dwFlags);
}

inline DWORD WINAPI FileSystem::GetModuleFileName(HMODULE hModule,LPSTR lpFilename,DWORD nSize)
{
	return ::GetModuleFileNameA(hModule,lpFilename,nSize);
}
	
#ifdef DEF_WCHAR
inline BOOL FileSystem::CreateDirectory(LPCWSTR lpPathName,LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
	if (IsUnicodeSystem())
		return ::CreateDirectoryW(lpPathName,lpSecurityAttributes);
	else
		return ::CreateDirectoryA(W2A(lpPathName),lpSecurityAttributes);
}

inline BOOL FileSystem::RemoveDirectory(LPCWSTR lpPathName)
{
	if (IsUnicodeSystem())
		return ::RemoveDirectoryW(lpPathName);
	else
		return ::RemoveDirectoryA(W2A(lpPathName));
}

inline BOOL FileSystem::MoveFile(LPCWSTR lpExistingFileName,LPCWSTR lpNewFileName,DWORD dwFlags)
{
	if (IsUnicodeSystem()) 
		return ::MoveFileExW(lpExistingFileName,lpNewFileName,dwFlags);	
	else
	{
		// We don't use MoveFileEx here, because system without Unicode system probably does not have it
		if (::CopyFileA(W2A(lpExistingFileName),W2A(lpNewFileName),TRUE))
			return ::DeleteFileA(W2A(lpExistingFileName));
		return FALSE;	
	}
}

inline BOOL FileSystem::CopyFile(LPCWSTR lpExistingFileName,LPCWSTR lpNewFileName,BOOL bFailIfExists)
{
	if (IsUnicodeSystem())
		return ::CopyFileW(lpExistingFileName,lpNewFileName,bFailIfExists);
	else
		return ::CopyFileA(W2A(lpExistingFileName),W2A(lpNewFileName),bFailIfExists);
}

inline UINT FileSystem::GetDriveType(LPCWSTR lpRootPathName)
{
	if (IsUnicodeSystem())
		return ::GetDriveTypeW(lpRootPathName);
	else
		return ::GetDriveTypeA(W2A(lpRootPathName));
}

inline DWORD FileSystem::GetFileAttributes(LPCWSTR lpFileName)
{
	if (IsUnicodeSystem())
		return ::GetFileAttributesW(lpFileName);
	else
		return ::GetFileAttributesA(W2A(lpFileName));
}

inline BOOL FileSystem::SetFileAttributes(LPCWSTR lpFileName,DWORD dwFileAttributes)
{
	if (IsUnicodeSystem())
		return ::SetFileAttributesW(lpFileName,dwFileAttributes);
	else
		return ::SetFileAttributesA(W2A(lpFileName),dwFileAttributes);
}

inline BOOL FileSystem::GetFileSecurity(LPCWSTR lpFileName,SECURITY_INFORMATION RequestedInformation,
	PSECURITY_DESCRIPTOR pSecurityDescriptor,DWORD nLength,LPDWORD lpnLengthNeeded)
{
	if (IsUnicodeSystem())
		return ::GetFileSecurityW(lpFileName,RequestedInformation,pSecurityDescriptor,nLength,lpnLengthNeeded);
	else
		return ::GetFileSecurityA(W2A(lpFileName),RequestedInformation,pSecurityDescriptor,nLength,lpnLengthNeeded);
}

inline HMODULE FileSystem::LoadLibrary(LPCWSTR lpFileName,DWORD dwFlags)
{
	if (IsUnicodeSystem())
		return ::LoadLibraryExW(lpFileName,NULL,dwFlags);
	else
		return ::LoadLibraryExA(W2A(lpFileName),NULL,dwFlags);
}


#endif

////////////////////////////////////////
// CFileFind

#ifdef WIN32

inline CFileFind::CFileFind()
{
#ifdef WIN32
	m_hFind=NULL;
#endif
}

#ifndef WIN32
inline void CFileFind::Close()
{
}
#endif

inline CFileFind::~CFileFind()
{
	Close();
}

inline void CFileFind::GetFileName(LPSTR szName,DWORD nMaxLen) const
{
#ifdef WIN32
	if (IsUnicodeSystem())
		WideCharToMultiByte(CP_ACP,0,m_fdw.cFileName,-1,szName,(int)nMaxLen,0,0);
	else
		strcpy_s(szName,nMaxLen,m_fd.cFileName);

#else
	strcpy_s(szName,nMaxLen,m_fd.ff_name);
#endif
}

inline BOOL CFileFind::IsRootDirectory() const
{
	if (IsUnicodeSystem())
		return m_fdw.cFileName[0]==L'.' && m_fdw.cFileName[1]==L'\0';
	else
		return m_fd.cFileName[0]=='.' && m_fd.cFileName[1]=='\0';
}

inline BOOL CFileFind::IsParentDirectory() const
{
	if (IsUnicodeSystem())
		return m_fdw.cFileName[0]==L'.' && m_fdw.cFileName[1]==L'.' && m_fdw.cFileName[2]==L'\0';
	else
		return m_fd.cFileName[0]=='.' && m_fd.cFileName[1]=='.' && m_fd.cFileName[2]=='\0';
}


inline void CFileFind::GetFileName(CString& name) const
{
#ifdef WIN32
	if (IsUnicodeSystem())
		name=m_fdw.cFileName;
	else
		name=m_fd.cFileName;
#else
	name=m_fd.ff_name;
#endif
}

inline DWORD CFileFind::GetFileSize(DWORD* pHigh) const
{
	if (pHigh!=NULL)
		*pHigh=m_fd.nFileSizeHigh;
	return m_fd.nFileSizeLow;
}

inline ULONGLONG CFileFind::GetFileSize64() const
{
	return ULONGLONG(m_fd.nFileSizeHigh)<<32|ULONGLONG(m_fd.nFileSizeLow);
}
	



inline BOOL CFileFind::GetLastWriteTime(FILETIME& rTimeStamp) const
{
	rTimeStamp=m_fd.ftLastWriteTime;
	return TRUE;
}

inline BOOL CFileFind::GetLastAccessTime(FILETIME& rTimeStamp) const
{
	rTimeStamp=m_fd.ftLastAccessTime;
	return TRUE;
}

inline BOOL CFileFind::GetCreationTime(FILETIME& rTimeStamp) const
{
	rTimeStamp=m_fd.ftCreationTime;
	return TRUE;
}

inline BOOL CFileFind::GetLastWriteTime(CTime& refTime) const
{
	refTime=m_fd.ftLastWriteTime;
	return TRUE;
}

inline BOOL CFileFind::GetLastAccessTime(CTime& refTime) const
{
	refTime=m_fd.ftLastAccessTime;
	return TRUE;
}

inline BOOL CFileFind::GetCreationTime(CTime& refTime) const
{
	refTime=m_fd.ftCreationTime;
	return TRUE;
}
#else

inline USHORT CFileFind::GetFileTime() const
{
	return m_fd.ff_ftime;
}

inline USHORT CFileFind::GetFileDate() const
{
	return m_fd.ff_fdate;
}
#endif

inline BOOL CFileFind::MatchesMask(DWORD dwMask) const
{
#ifdef WIN32
	return (m_fd.dwFileAttributes&dwMask)?TRUE:FALSE;
#else
	return (m_fd.ff_attrib&dwMask)?TRUE:FALSE;
#endif
}

inline BOOL CFileFind::IsReadOnly() const
{
#ifdef WIN32
	return MatchesMask(FILE_ATTRIBUTE_READONLY);
#else
	return MatchesMask(FA_RDONLY);
#endif
}

inline BOOL CFileFind::IsDirectory() const
{
#ifdef WIN32
	return MatchesMask(FILE_ATTRIBUTE_DIRECTORY);
#else
	return MatchesMask(FA_DIREC);
#endif
}

inline BOOL CFileFind::IsSystem() const
{
#ifdef WIN32
	return MatchesMask(FILE_ATTRIBUTE_SYSTEM);
#else
	return MatchesMask(FA_SYSTEM);
#endif
}

inline BOOL CFileFind::IsHidden() const
{
#ifdef WIN32
	return MatchesMask(FILE_ATTRIBUTE_HIDDEN);
#else
	return MatchesMask(FA_HIDDEN);
#endif
}

inline BOOL CFileFind::IsNormal() const
{
#ifdef WIN32
	return MatchesMask(FILE_ATTRIBUTE_NORMAL);
#else
	return MatchesMask(0);
#endif
}

inline BOOL CFileFind::IsArchived() const
{
#ifdef WIN32
	return MatchesMask(FILE_ATTRIBUTE_ARCHIVE);
#else
	return MatchesMask(FA_ARCH);
#endif
}

#ifdef WIN32
inline BOOL CFileFind::IsCompressed() const
{
	return MatchesMask(FILE_ATTRIBUTE_COMPRESSED);
}

inline BOOL CFileFind::IsTemporary() const
{
	return MatchesMask(FILE_ATTRIBUTE_TEMPORARY);
}
#endif


#ifdef DEF_WCHAR
inline void CFileFind::GetFileName(LPWSTR szName,DWORD nMaxLen) const
{
	if (IsUnicodeSystem())
		StringCbCopyW(szName,nMaxLen*2,m_fdw.cFileName);
	else
		MultiByteToWideChar(CP_ACP,0,m_fd.cFileName,-1,szName,(int)nMaxLen);
	
}

inline void CFileFind::GetFileName(CStringW& name) const
{
	if (IsUnicodeSystem())
		name=m_fdw.cFileName;
	else
		name=m_fd.cFileName;
}
#endif



////////////////////////////////////////
// CSearchFromFile

HFCERROR SetHFCError2(HFCERROR,int,char*);

inline CSearchFromFile::CSearchFromFile()
:	hFile(NULL)
{
}

inline CSearchHexFromFile::CSearchHexFromFile(const BYTE* pData,DWORD dwLength)
:	CSearchFromFile(),pBuffer(NULL),bMatchCase(TRUE)
{
	// Setting data
	pSearchValue=new BYTE[this->dwLength=dwLength];
	for (register int i=dwLength-1;i>=0;i--)
		pSearchValue[i]=pData[i];
}

inline CSearchHexFromFile::CSearchHexFromFile(LPCSTR szString,BOOL bMatchCase)
:	CSearchFromFile(),pBuffer(NULL)
{
	this->bMatchCase=bMatchCase;

	// Setting data
	dwLength=(DWORD)istrlen(szString);
#ifdef WIN32
	pSearchValue=new BYTE[dwLength];
	for (register LONG_PTR i=dwLength-1;i>=0;i--)
		pSearchValue[i]=szString[i];
	if (!bMatchCase)
		CharLowerBuff(LPSTR(pSearchValue),(DWORD)dwLength);
#else
	if (!bMatchCase)
	{
		pSearchValue=new BYTE[dwLength+1];
		for (register int i=dwLength;i>=0;i--)
			pSearchValue[i]=szString[i];
		strlwr(LPSTR(pSearchValue));
		return;
	}

	pSearchValue=new BYTE[dwLength];
	for (register int i=dwLength-1;i>=0;i--)
		pSearchValue[i]=szString[i];
#endif
}

inline CSearchHexFromFile::CSearchHexFromFile(LPCSTR szString,DWORD dwLength,BOOL bMatchCase)
:	CSearchFromFile(),pBuffer(NULL),bMatchCase(TRUE)
{
	this->bMatchCase=bMatchCase;

#ifdef WIN32
	// Setting data
	pSearchValue=new BYTE[this->dwLength=dwLength];
	for (register LONG_PTR i=this->dwLength-1;i>=0;i--)
		pSearchValue[i]=szString[i];
	if (!bMatchCase)
		CharLowerBuff(LPSTR(pSearchValue),dwLength);
#else
	// Setting data
	if (!bMatchCase)
	{
		this->dwLength=dwLength;
		pSearchValue=new BYTE[dwLength+1];
		for (register int i=this->dwLength-1;i>=0;i--)
			pSearchValue[i]=szString[i];  
		pSearchValue[dwLength]='\0';
		strlwr(LPSTR(pSearchValue));
		return;
	}	
	pSearchValue=new BYTE[this->dwLength=dwLength];
	for (register int i=this->dwLength-1;i>=0;i--)
		pSearchValue[i]=szString[i];
#endif
}

inline CSearchHexFromFile::CSearchHexFromFile(BYTE bNumber)
:	CSearchFromFile(),pBuffer(NULL),bMatchCase(TRUE)
{
	// Setting data
	pSearchValue=new BYTE[2];
	dwLength=1;
	*pSearchValue=bNumber;
}

inline CSearchHexFromFile::CSearchHexFromFile(WORD wNumber)
:	CSearchFromFile(),pBuffer(NULL),bMatchCase(TRUE)
{
	// Setting data
	pSearchValue=new BYTE[2];
	dwLength=2;
	*PWORD(pSearchValue)=wNumber;
}

inline CSearchHexFromFile::CSearchHexFromFile(DWORD dwNumber)
:	CSearchFromFile(),pBuffer(NULL),bMatchCase(TRUE)
{
	// Setting data
	pSearchValue=new BYTE[4];
	dwLength=4;
	*PDWORD(pSearchValue)=dwNumber;
}


#endif
