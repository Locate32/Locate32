////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////


#ifndef HFCGENERAL_H
#define HFCGENERAL_H





class CTime
{
public:
	static CTime PASCAL GetCurrentTime();
	
	CTime();
	CTime(time_t time,WORD ms=0);
	CTime(int nYear,int nMonth,int nDay,int nHour,int nMin,int nSec,int nMSec,int nDST=-1);
	CTime(WORD wDosDate, WORD wDosTime, int nDST=-1);
	CTime(const CTime& timeSrc);

#ifdef WIN32
	CTime(const SYSTEMTIME& sysTime, int nDST=-1);
	CTime(const FILETIME& fileTime, int nDST=-1);
#endif

	time_t m_time;
	WORD msec;
	
	operator time_t() const;
#ifdef WIN32
	operator SYSTEMTIME() const;
	operator FILETIME() const;
#endif

	const CTime& operator=(const CTime& timeSrc);
	const CTime& operator=(time_t t);
#ifdef WIN32
	const CTime& operator=(SYSTEMTIME& timeSrc);
	const CTime& operator=(FILETIME& timeSrc);
#endif

	struct tm* GetGmtTm(struct tm* ptm = NULL) const;
	struct tm* GetLocalTm(struct tm* ptm = NULL) const;

	time_t GetTime() const;
	int GetYear() const;
	int GetMonth() const;
	int GetDay() const;
	int GetHour() const;
	int GetMinute() const;
	int GetSecond() const;
	int GetMilliSecond() const;
	int GetDayOfWeek() const;   // 1=Sun, 2=Mon, ..., 7=Sat
	
	BOOL IsLeapYear() const;
	WORD DayOfYear() const;
	DWORD GetIndex() const;
	BYTE GetWeek(BYTE nFirstIsMonday=TRUE) const;
	BYTE GetDaysInMonth() const;
	
	static BOOL IsLeapYear(WORD nYear);
	static WORD LeapYears(WORD nYear);
	static WORD DayOfYear(WORD nDay,WORD nMonth,WORD nYear);
	static DWORD GetIndex(WORD nDay,WORD nMonth,WORD nYear);
	static CTime GetDayFromDayOfYear(WORD nDay,WORD nYear);
	static CTime GetDayFromIndex(DWORD nIndex);
#ifdef WIN32
	static void DecreaseDaysInSystemTime(LPSYSTEMTIME pst,int nDays);
#endif
	
	static void GetDayFromDayOfYear(WORD nDayOfYear,WORD nYear,WORD& nDay,WORD& nMonth);
	static void GetDayFromIndex(DWORD nIndex,WORD& nDay,WORD& nMonth,WORD& nYear);
	
	static BYTE GetDaysInMonth(BYTE nMonth,WORD nYear);
	static BYTE GetWeek(WORD nDay,WORD nMonth,WORD nYear,BYTE nFirstIsMonday=TRUE);
	static int GetDayOfWeek(WORD nDay,WORD nMonth,WORD nYear);  // 0=Sun, 1=Mon, ..., 6=Sat
			
	BOOL operator==(CTime time) const;
	BOOL operator!=(CTime time) const;
	BOOL operator<(CTime time) const;
	BOOL operator>(CTime time) const;
	BOOL operator<=(CTime time) const;
	BOOL operator>=(CTime time) const;

	CString Format(LPCSTR pFormat) const;
	CString FormatGmt(LPCSTR pFormat) const;
#ifdef DEF_RESOURCES
	CString Format(UINT nFormatID) const;
	CString FormatGmt(UINT nFormatID) const;
#endif

#ifdef DEF_WCHAR
	CStringW Format(LPCWSTR pFormat) const;
	CStringW FormatGmt(LPCWSTR pFormat) const;

#ifdef DEF_RESOURCES
	CStringW FormatW(UINT nFormatID) const;
	CStringW FormatGmtW(UINT nFormatID) const;
#endif

#endif

public:
	static BYTE Wait(DWORD nTime);
};

class CFileStatus
{
public:
	CTime m_ctime;
	CTime m_mtime;
	CTime m_atime;
	LONG m_size;
	DWORD m_attribute;
	TCHAR m_szFullName[MAX_PATH];
};

class CObject
{
};

class CExceptionObject : public CObject
{
public:
	CExceptionObject();
	CExceptionObject(BOOL bThrow);
	void SetToThrow(BOOL bThrow=TRUE);
	BOOL IsThrowing() const;

protected:
	BOOL m_bThrow;
};


// Abstarct stream class
class CStream : public CExceptionObject
{
public:
	CStream();
	CStream(BOOL bThrow);
	virtual ~CStream();

public:
	// Set length
	virtual DWORD GetLength(DWORD* pHigh=NULL) const=0;
	virtual ULONGLONG GetLength64() const=0;
	virtual BOOL SetLength(DWORD dwNewLen,LONG* pHigh=NULL)=0;
	virtual BOOL SetLength64(ULONGLONG ullNewLen)=0;

	// Set position
	enum SeekPosition { begin = 0x0, current = 0x1, end = 0x2 };
	DWORD SeekToEnd() { return this->Seek(0,end); }
	BOOL SeekToBegin() { return this->Seek(0,begin)>0; }
	virtual DWORD Seek(LONG lOff, SeekPosition nFrom,LONG* pHighPos=NULL)=0;
	virtual DWORD Seek64(ULONGLONG lOff, SeekPosition nFrom)=0;

	// Get position
	virtual ULONG GetPosition(PLONG pHigh=NULL) const=0;
	virtual ULONGLONG GetPosition64() const=0;

	// Reading/writing
	virtual DWORD Read(void* lpBuf, DWORD nCount) const=0;
	virtual BOOL Write(const void* lpBuf, DWORD nCount)=0;
#ifdef DEF_WCHAR
	virtual BOOL Write(LPCWSTR lpString, DWORD nCount)=0;
#endif


	// Helpers
	BOOL Read(BYTE& bNum) const;
	BOOL Read(WORD& wNum) const;
	BOOL Read(DWORD& dwNum) const;
	

	BOOL Write(const CStringA& str);
#ifdef DEF_WCHAR
	BOOL Write(const CStringW& str);
#endif
	BOOL Write(BYTE bNum);
	BOOL Write(WORD wNum);
	BOOL Write(DWORD dwNum);
	BOOL Write(char ch);
	BOOL Write(LPCSTR szNullTerminatedString);
#ifdef DEF_WCHAR
	BOOL Write(LPCWSTR szNullTerminatedString);
#endif
};

#ifdef WIN32
#define FILE_NULL	INVALID_HANDLE_VALUE
#else
#define FILE_NULL	NULL
#endif



class CFile : public CStream
{
public:
	enum OpenFlags {
		// Flags corresponds with dwCreationDisposition param of CreateFile
		openCreateNew=			0x1,
		openCreateAlways=		0x2,
		openExisting=			0x3,
		openAlways=				0x4,
		openTruncate=			0x5,
		openFlags=				0xF,
		
		// Shage flags
		shareCompat=			0x0,
		shareDenyWrite=			0x10,
		shareDenyRead=			0x20,
		shareDenyNone=			0x30,
		shareFlags=				0xF0,
		
		// Flags corresponds with dwDesiredAccess param of CreateFile
		modeWrite=				0x40000000,
		modeRead=				0x80000000,
		modeReadWrite=			0x40000000|0x80000000,
		modeFlags=				0xF0000000,
		
		
		typeText=				0x4000,
		typeBinary=				0,
		typeFlags=				0xF000,
		
		otherInherit=			0x100,
		otherErrorWhenEOF=		0x200,
		otherStrNullTerminated= 0x400,
		
		otherFlags=				0xF00,

		defRead=				modeRead|shareDenyWrite|openExisting,
		defWrite=				modeWrite|shareDenyWrite|openCreateAlways
	};

	enum Attribute {
		readOnly=	0x1,
		hidden=		0x2,
		system=		0x04,
		volume=		0x08,
		directory=	0x10,
		archive=	0x20,
		normal=		0x80,
		tempotary=	0x100,
		comressed=	0x200,
		encrypted=	0x400
	};

	
	CFile(CFileException* e=NULL);
	CFile(BOOL bThrowExceptions,CFileException* e=NULL);
	CFile(HANDLE hFile,BOOL bThrowExceptions=TRUE,CFileException* e=NULL);
	CFile(LPCSTR lpszFileName,int nOpenFlags,BOOL bThrowExceptions=TRUE,CFileException* e=NULL);
#ifdef DEF_WCHAR
	CFile(LPCWSTR lpszFileName,int nOpenFlags,BOOL bThrowExceptions=TRUE,CFileException* e=NULL);
#endif

#ifndef WIN32
	CFile(FILE* pFile,BOOL bThrowExceptions=FALSE,CFileException* e=NULL);
#endif
	virtual ~CFile();
	
	HANDLE m_hFile;
	BOOL m_bCloseOnDelete;
	CStringW m_strFileName;
	int m_nOpenFlags;
	CFileException* m_pFileException;

	operator HANDLE() const;
#ifndef WIN32
	operator FILE*() const;
#endif
	BOOL IsOpen() const { return m_hFile!=FILE_NULL; }

	virtual ULONG GetPosition(PLONG pHigh=NULL) const;
	virtual ULONGLONG GetPosition64() const;

	void CloseOnDelete(BOOL bCloseOnDelete=TRUE) { m_bCloseOnDelete=bCloseOnDelete; }
	BOOL IsClosingOnDelete() const { return m_bCloseOnDelete; }

	BOOL GetStatus(CFileStatus& rStatus) const;
	virtual CString GetFileTitle() const;
	virtual CString GetFilePath() const;
	virtual void SetFilePath(LPCSTR lpszNewName);

	virtual BOOL Open(LPCSTR lpszFileName, int nOpenFlags);
#ifdef DEF_WCHAR
	virtual BOOL Open(LPCWSTR lpszFileName, int nOpenFlags);

	virtual CStringW GetFilePathW() const;
	virtual CStringW GetFileTitleW() const;
	virtual void SetFilePath(LPCWSTR lpszNewName);
#endif	
	BOOL OpenRead(LPCSTR lpszFileName) { return this->Open(lpszFileName,CFile::defRead); }
	BOOL OpenWrite(LPCSTR lpszFileName) { return this->Open(lpszFileName,CFile::defWrite); }
#ifdef DEF_WCHAR
	BOOL OpenRead(LPCWSTR lpszFileName) { return this->Open(lpszFileName,CFile::defRead); }
	BOOL OpenWrite(LPCWSTR lpszFileName) { return this->Open(lpszFileName,CFile::defWrite); }
#endif
	
	virtual DWORD Seek(LONG lOff, SeekPosition nFrom,LONG* pHighPos=NULL);
	virtual DWORD Seek64(ULONGLONG lOff, SeekPosition nFrom);

	virtual BOOL SetLength(DWORD dwNewLen,LONG* pHigh=NULL);
	virtual BOOL SetLength64(ULONGLONG ullNewLen);

	virtual DWORD GetLength(DWORD* pHigh=NULL) const;
	virtual ULONGLONG GetLength64() const;

	virtual DWORD Read(void* lpBuf, DWORD nCount) const;
	virtual BOOL Write(const void* lpBuf, DWORD nCount);
#ifdef DEF_WCHAR
	virtual BOOL Write(LPCWSTR lpString, DWORD nCount);
#endif

	BOOL GetFileTime(LPFILETIME lpCreationTime,LPFILETIME lpLastAccessTime,LPFILETIME lpLastWriteTime) const;
	BOOL GetFileTime(LPSYSTEMTIME lpCreationTime,LPSYSTEMTIME lpLastAccessTime,LPSYSTEMTIME lpLastWriteTime) const;
	BOOL SetFileTime(const FILETIME* lpCreationTime,const FILETIME* lpLastAccessTime,const FILETIME* lpLastWriteTime);
	BOOL SetFileTime(const SYSTEMTIME* lpCreationTime,const SYSTEMTIME* lpLastAccessTime,const SYSTEMTIME* lpLastWriteTime);
	DWORD GetFileType() const;


	BOOL SetFileShortName(LPCSTR lpShortName);
#ifdef DEF_WCHAR
	BOOL SetFileShortName(LPCWSTR lpShortName);
#endif



	// Helpers
	BOOL Read(BYTE& bNum) const { return CStream::Read(bNum); }
	BOOL Read(WORD& wNum) const { return CStream::Read(wNum); }
	BOOL Read(DWORD& dwNum) const { return CStream::Read(dwNum); }
	BOOL Read(CStringA& str) const;
	
	BOOL Write(BYTE bNum) { return CStream::Write(bNum); }
	BOOL Write(WORD wNum) { return CStream::Write(wNum); }
	BOOL Write(DWORD dwNum) { return CStream::Write(dwNum); }
	BOOL Write(char ch)  { return CStream::Write(ch); }
	
	BOOL Write(const CStringA& str);
	BOOL Write(LPCSTR szNullTerminatedString);
#ifdef DEF_WCHAR
	BOOL Read(CStringW& str) const;
	BOOL Write(const CStringW& str);
	BOOL Write(LPCWSTR szNullTerminatedString);
#endif

	virtual BOOL IsEndOfFile() const;

#ifdef WIN32
	virtual void LockRange(DWORD dwPos, DWORD dwCount);
	virtual void UnlockRange(DWORD dwPos, DWORD dwCount);
#endif
	virtual BOOL Flush();
	virtual BOOL Close();

};

#ifdef DEF_WCHAR
class CFileEncode : public CFile
{
public:
	enum Encoding {
		ANSI=0,
		Unicode=1,
		UTF8=2
	};
private:
	Encoding m_nEncoding;

public:
	CFileEncode(CFileException* e=NULL);
	CFileEncode(BOOL bThrowExceptions,CFileException* e=NULL);
	CFileEncode(HANDLE hFile,BOOL bThrowExceptions=TRUE,CFileException* e=NULL);
	CFileEncode(LPCSTR lpszFileName,int nOpenFlags,BOOL bThrowExceptions=TRUE,CFileException* e=NULL);
	CFileEncode(LPCWSTR lpszFileName,int nOpenFlags,BOOL bThrowExceptions=TRUE,CFileException* e=NULL);
	virtual ~CFileEncode();

	void SetEncoding(Encoding nEncoding) { m_nEncoding=nEncoding; }

	//BOOL Read(CStringW& str) const;
	//BOOL Read(LPWSTR szBuffer,DWORD nBufferLength);

	BOOL Write(WCHAR ch);
	BOOL Write(const CStringW& str);
	BOOL Write(LPCWSTR szNullTerminatedString);
	virtual BOOL Write(LPCWSTR szString,DWORD nCount);

};
#endif

namespace FileSystem {
	#ifndef WIN32
	BYTE CopyFile(LPCSTR src,LPCSTR dst);
	#endif
	LONGLONG GetDiskFreeSpace(LPCSTR szDrive);


	BOOL Rename(LPCSTR lpszOldName,LPCSTR lpszNewName);
	BOOL MoveFile(LPCSTR lpExistingFileName,LPCSTR lpNewFileName,DWORD dwFlags=0);	
	BOOL Remove(LPCSTR lpszFileName);
	BOOL CopyFile(LPCSTR lpExistingFileName,LPCSTR lpNewFileName,BOOL bFailIfExists=FALSE);

	BOOL GetStatus(LPCSTR lpszFileName,CFileStatus& rStatus);
	BOOL SetStatus(LPCSTR lpszFileName,const CFileStatus& status);
	
	
	BOOL IsFile(LPCSTR szFileName);
	INT IsDirectory(LPCSTR szDirectoryName); // return: 0 not dir, 1 fixed, 2 remote
	BOOL IsValidFileName(LPCSTR szFile,LPSTR szShortName=NULL); // Parent directory must exist
	BOOL IsValidPath(LPCSTR szPath,BOOL bAsDirectory=FALSE);
	BOOL IsSamePath(LPCSTR szDir1,LPCSTR szDir2);
	BOOL IsSubDirectory(LPCSTR szSubDir,LPCSTR szPath);
	
	// Last '//' is not counted, if exists
	DWORD ParseExistingPath(LPCSTR szPath);
	
	
	BOOL CreateDirectory(LPCSTR lpPathName,LPSECURITY_ATTRIBUTES lpSecurityAttributes=NULL);
	BOOL CreateDirectoryRecursive(LPCSTR szPath,LPSECURITY_ATTRIBUTES plSecurityAttributes);
	BOOL RemoveDirectory(LPCSTR lpPathName);
	
	DWORD GetFullPathName(LPCSTR lpFileName,DWORD nBufferLength,LPSTR lpBuffer,LPSTR* lpFilePart);
	DWORD GetShortPathName(LPCSTR lpszLongPath,LPSTR lpszShortPath,DWORD cchBuffer);
	DWORD GetCurrentDirectory(DWORD nBufferLength,LPSTR lpBuffer);
	DWORD GetLongPathName(LPCSTR lpszShortPath,LPSTR lpszLongPath,DWORD cchBuffer);
	DWORD GetTempPath(DWORD nBufferLength,LPSTR lpBuffer);
	UINT GetTempFileName(LPCSTR lpPathName,LPCSTR lpPrefixString,UINT uUnique,LPSTR lpTempFileName);
	
	short GetFileTitle(LPCSTR lpszFile,LPSTR lpszTitle,WORD cbBuf);
	DWORD GetFileAttributes(LPCSTR lpFileName);
	BOOL SetFileAttributes(LPCSTR lpFileName,DWORD dwFileAttributes);

	UINT GetDriveType(LPCSTR lpRootPathName);
	DWORD GetLogicalDriveStrings(DWORD nBufferLength,LPSTR lpBuffer);
	BOOL GetVolumeInformation(LPCSTR lpRootPathName,LPSTR lpVolumeNameBuffer,
		DWORD nVolumeNameSize,LPDWORD lpVolumeSerialNumber,LPDWORD lpMaximumComponentLength,
		LPDWORD lpFileSystemFlags,LPSTR lpFileSystemNameBuffer,DWORD nFileSystemNameSize);

	BOOL GetFileSecurity(LPCSTR lpFileName,SECURITY_INFORMATION RequestedInformation,
		PSECURITY_DESCRIPTOR pSecurityDescriptor,DWORD nLength,LPDWORD lpnLengthNeeded);
	BOOL LookupAccountName(LPCSTR lpSystemName,LPCSTR lpAccountName,PSID Sid,
		LPDWORD cbSid,LPSTR ReferencedDomainName,LPDWORD cchReferencedDomainName,
		PSID_NAME_USE peUse);
	BOOL LookupAccountSid(LPCSTR lpSystemName,PSID lpSid,LPSTR lpName,LPDWORD cchName,
		LPSTR lpReferencedDomainName,LPDWORD cchReferencedDomainName,PSID_NAME_USE peUse);

	UINT GetWindowsDirectory(LPSTR lpBuffer,UINT uSize);
	UINT GetSystemDirectory(LPSTR lpBuffer,UINT uSize);
	HMODULE LoadLibrary(LPCSTR lpFileName,DWORD dwFlags=NULL);
	
	DWORD WINAPI GetModuleFileName(HMODULE hModule,LPSTR lpFilename,DWORD nSize);




#ifdef DEF_WCHAR
	LONGLONG GetDiskFreeSpace(LPCWSTR szDrive);

	BOOL IsFile(LPCWSTR szFileName);
	INT IsDirectory(LPCWSTR szDirectoryName); // return: 0 not dir, 1 fixed, 2 remote
	BOOL IsValidFileName(LPCWSTR szFile,LPWSTR szShortName=NULL); // Parent directory must exist
	BOOL IsValidPath(LPCWSTR szPath,BOOL bAsDirectory=FALSE);
	BOOL IsSamePath(LPCWSTR szDir1,LPCWSTR szDir2);
	BOOL IsSubDirectory(LPCWSTR szSubDir,LPCWSTR szPath);
	
	// Last '//' is not counted, if exists
	DWORD ParseExistingPath(LPCWSTR szPath);

	
	BOOL Rename(LPCWSTR lpszOldName,LPCWSTR lpszNewName);
	BOOL MoveFile(LPCWSTR lpExistingFileName,LPCWSTR lpNewFileName,DWORD dwFlags=0);	
	BOOL Remove(LPCWSTR lpszFileName);
	BOOL CopyFile(LPCWSTR lpExistingFileName,LPCWSTR lpNewFileName,BOOL bFailIfExists=FALSE);

	
	
	BOOL CreateDirectory(LPCWSTR lpPathName,LPSECURITY_ATTRIBUTES lpSecurityAttributes=NULL);
	BOOL RemoveDirectory(LPCWSTR lpPathName);
	BOOL CreateDirectoryRecursive(LPCWSTR szPath,LPSECURITY_ATTRIBUTES plSecurityAttributes);

	DWORD GetFullPathName(LPCWSTR lpFileName,DWORD nBufferLength,LPWSTR lpBuffer,LPWSTR* lpFilePart);
	DWORD GetShortPathName(LPCWSTR lpszLongPath,LPWSTR lpszShortPath,DWORD cchBuffer);
	DWORD GetCurrentDirectory(DWORD nBufferLength,LPWSTR lpBuffer);
	DWORD GetLongPathName(LPCWSTR lpszShortPath,LPWSTR lpszLongPath,DWORD cchBuffer);
	DWORD GetTempPath(DWORD nBufferLength,LPWSTR lpBuffer);
	UINT GetTempFileName(LPCWSTR lpPathName,LPCWSTR lpPrefixString,UINT uUnique,LPWSTR lpTempFileName);

	short GetFileTitle(LPCWSTR lpszFile,LPWSTR lpszTitle,WORD cbBuf);
	DWORD GetFileAttributes(LPCWSTR lpFileName);
	BOOL SetFileAttributes(LPCWSTR lpFileName,DWORD dwFileAttributes);


	UINT GetDriveType(LPCWSTR lpRootPathName);
	DWORD GetLogicalDriveStrings(DWORD nBufferLength,LPWSTR lpBuffer);
	BOOL GetVolumeInformation(LPCWSTR lpRootPathName,LPWSTR lpVolumeNameBuffer,
		DWORD nVolumeNameSize,LPDWORD lpVolumeSerialNumber,LPDWORD lpMaximumComponentLength,
		LPDWORD lpFileSystemFlags,LPWSTR lpFileSystemNameBuffer,DWORD nFileSystemNameSize);



	BOOL GetFileSecurity(LPCWSTR lpFileName,SECURITY_INFORMATION RequestedInformation,
		PSECURITY_DESCRIPTOR pSecurityDescriptor,DWORD nLength,LPDWORD lpnLengthNeeded);
	BOOL LookupAccountName(LPCWSTR lpSystemName,LPCWSTR lpAccountName,PSID Sid,
		LPDWORD cbSid,LPWSTR ReferencedDomainName,LPDWORD cchReferencedDomainName,
		PSID_NAME_USE peUse);
	BOOL LookupAccountSid(LPCWSTR lpSystemName,PSID lpSid,LPWSTR lpName,LPDWORD cchName,
		LPWSTR lpReferencedDomainName,LPDWORD cchReferencedDomainName,PSID_NAME_USE peUse);




	UINT GetWindowsDirectory(LPWSTR lpBuffer,UINT uSize);
	UINT GetSystemDirectory(LPWSTR lpBuffer,UINT uSize);
	HMODULE LoadLibrary(LPCWSTR lpFileName,DWORD dwFlags=NULL);


	DWORD WINAPI GetModuleFileName(HMODULE hModule,LPWSTR lpFilename,DWORD nSize);

#endif	



};



class CFileFind : public CObject
{
public:
#ifdef WIN32
	HANDLE m_hFind;
	union {
		WIN32_FIND_DATA m_fd;
#ifdef DEF_WCHAR
		WIN32_FIND_DATAW m_fdw;
#endif
	};

#else
	struct ffblk m_fd;
#endif
	CStringW strRoot;

public:
	CFileFind();
	~CFileFind();

	void Close();
	BOOL FindFile(LPCSTR pstrName=NULL);
	BOOL FindNextFile();
	
	void GetFileName(LPSTR szName,DWORD nMaxLen) const;
	void GetFileName(CString& name) const;
	void GetFilePath(LPSTR szPath,DWORD nMaxLen) const;
	void GetFilePath(CString& path) const;
	DWORD GetFileSize(DWORD* pHigh=NULL) const;
	ULONGLONG GetFileSize64() const;
	
#ifdef WIN32
	BOOL GetFileTitle(CString& title) const;
	BOOL GetFileTitle(LPSTR szFileTitle,DWORD nMaxLen) const;
#endif

#ifdef WIN32
	BOOL GetLastWriteTime(FILETIME& rTimeStamp) const;
	BOOL GetLastAccessTime(FILETIME& rTimeStamp) const;
	BOOL GetCreationTime(FILETIME& rTimeStamp) const;
	BOOL GetLastWriteTime(CTime& refTime) const;
	BOOL GetLastAccessTime(CTime& refTime) const;
	BOOL GetCreationTime(CTime& refTime) const;
#else
	USHORT GetFileTime() const;
	USHORT GetFileDate() const;
	BOOL GetFileTime(CTime& refTime) const;
#endif

	BOOL MatchesMask(DWORD dwMask) const;

	BOOL IsReadOnly() const;
	BOOL IsDirectory() const;
	BOOL IsSystem() const;
	BOOL IsHidden() const;
	BOOL IsNormal() const;
	BOOL IsArchived() const;
#ifdef WIN32
	BOOL IsTemporary() const;
	BOOL IsCompressed() const;
#endif

	BOOL IsRootDirectory() const;
	BOOL IsParentDirectory() const;


#ifdef DEF_WCHAR
	BOOL FindFile(LPCWSTR pstrName);
	void GetFileName(LPWSTR szName,DWORD nMaxLen) const;
	void GetFileName(CStringW& name) const;
	void GetFilePath(LPWSTR szPath,DWORD nMaxLen) const;
	void GetFilePath(CStringW& path) const;

	BOOL GetFileTitle(CStringW& title) const;
	BOOL GetFileTitle(LPWSTR szFileTitle,DWORD nMaxLen) const;
#endif
};

class CSearchFromFile // Pure virtual class
{
public:
	CSearchFromFile();
	virtual ~CSearchFromFile();
	
	virtual BOOL Search(LPCSTR szFile)=0; // if return value is TRUE if any found
#ifdef DEF_WCHAR
	virtual BOOL Search(LPCWSTR szFile)=0; // if return value is TRUE if any found
#endif
	virtual ULONG_PTR GetFoundPosition() const=0;
	
	virtual void OpenFile(LPCSTR szFile);
#ifdef DEF_WCHAR
	virtual void OpenFile(LPCWSTR szFile);
#endif	
	virtual void CloseFile();

protected:
#ifdef WIN32
	HANDLE hFile;
#else
	FILE* hFile;
#endif

};

class CSearchHexFromFile : public CSearchFromFile
{
public:
	CSearchHexFromFile(const BYTE* pData,DWORD dwLength);
	CSearchHexFromFile(LPCSTR szString,BOOL bMatchCase=TRUE);
	CSearchHexFromFile(LPCSTR szString,DWORD dwLength,BOOL bMatchCase=TRUE);
	CSearchHexFromFile(BYTE bNumber);
	CSearchHexFromFile(WORD wNumber);
	CSearchHexFromFile(DWORD dwNumber);
	virtual ~CSearchHexFromFile();
	virtual void CloseFile();

	
	virtual BOOL Search(LPCSTR szFile); // if return value is TRUE if any found
#ifdef DEF_WCHAR
	virtual BOOL Search(LPCWSTR szFile); // if return value is TRUE if any found
#endif
	virtual ULONG_PTR GetFoundPosition() const;

	

private:

	BOOL DoSearching();
	
	BYTE* pSearchValue;
	DWORD dwLength;


	BYTE* pBuffer;
	DWORD dwBufferLen;
	ULONG_PTR dwBufferPtr;
	ULONG_PTR dwFilePtr; 
	SIZE_T dwFileSize;
	BOOL bMatchCase;
};

class CSize : public tagSIZE
{
public:
	CSize();
	CSize(int initCX,int initCY);
	CSize(SIZE initSize);
	CSize(DWORD dwSize);
	
	int GetCX() const;
	void SetCX(int initCX);
	int GetCY() const;
	void SetCY(int initCY);
	void SetSize(int initCX,int initCY);

	void MakeLarger(CSize& sz);
	void MakeSmaller(CSize& sz);

};

class CPoint : public tagPOINT
{
public:
	CPoint();
	CPoint(int initX, int initY);
	CPoint(const POINT& initPt);
	CPoint(const SIZE& initSize);
	CPoint(DWORD dwPoint);

	BOOL IsInRect(const RECT& rc);

	int GetX() const;
	void SetX(int initX);
	int GetY() const;
	void SetY(int initY);
	void SetPoint(int initX,int initY);
};

class CRect : public tagRECT
{
public:
	CRect();
	CRect(int l, int t, int r, int b);
	CRect(const RECT& rc);
	CRect(const LPRECT rc);
	CRect(const POINT& pt, const SIZE& sz);
	CRect(const POINT& topLeft, const POINT& bottomRight);

	int Width() const;
	int Height() const;
	CSize Size() const;
	
	BOOL IsPtInRect(const POINT& pt);
	BOOL IsPtInRect(int x,int y);

	CPoint TopLeft();
	CPoint BottomRight();
};


#ifdef WIN32

class CGdiObject : public CObject
{
public:
	HGDIOBJ m_hObject;
	
public:
	CGdiObject();
	CGdiObject(HGDIOBJ hObject);
	virtual ~CGdiObject();

	BOOL DeleteObject();
	
	operator HGDIOBJ() const;
	HGDIOBJ GetSafeHandle() const;
	
	
	int GetObject(int nCount,LPVOID lpObject) const;
	DWORD GetObjectType() const;
	BOOL CreateStockObject(int nIndex);
	BOOL UnrealizeObject();
	BOOL operator==(const CGdiObject& obj) const;
	BOOL operator!=(const CGdiObject& obj) const;
};

class CPen : public CGdiObject
{
public:
	CPen();
	CPen(HPEN hPen);
	CPen(int nPenStyle,int nWidth,COLORREF crColor);
	CPen(int nPenStyle,int nWidth,const LOGBRUSH* pLogBrush,
		int nStyleCount=0,const DWORD* lpStyle=NULL);
	
	BOOL CreatePen(int nPenStyle,int nWidth,COLORREF crColor);
	BOOL CreatePen(int nPenStyle,int nWidth,const LOGBRUSH* pLogBrush,
		int nStyleCount=0,const DWORD* lpStyle=NULL);
	BOOL CreatePenIndirect(LPLOGPEN lpLogPen);

	operator HPEN() const;
	int GetLogPen(LOGPEN* pLogPen);
	int GetExtLogPen(EXTLOGPEN* pLogPen);
};

class CBrush : public CGdiObject
{
public:
	CBrush();
	CBrush(HBRUSH hObject);
	CBrush(COLORREF crColor);
	CBrush(int nIndex,COLORREF crColor);
	
	BOOL CreateSolidBrush(COLORREF crColor);
	BOOL CreateHatchBrush(int nIndex,COLORREF crColor);
	BOOL CreateBrushIndirect(const LOGBRUSH* lpLogBrush);
	BOOL CreatePatternBrush(HBITMAP hBitmap);
	BOOL CreateDIBPatternBrush(HGLOBAL hPackedDIB,UINT nUsage);
	BOOL CreateDIBPatternBrush(const void* lpPackedDIB,UINT nUsage);
	BOOL CreateSysColorBrush(int nIndex);

	operator HBRUSH() const;
	int GetLogBrush(LOGBRUSH* pLogBrush);
};

class CFont : public CGdiObject
{
public:
	CFont();
	CFont(HFONT hObject);
	
	BOOL CreateFontIndirect(const LOGFONTA* lpLogFont);
	BOOL CreateFontIndirect(const LOGFONTW* lpLogFont);
	
	BOOL CreateFont(int nHeight,int nWidth,int nEscapement,
		int nOrientation,int nWeight,BYTE bItalic,BYTE bUnderline,
		BYTE cStrikeOut,BYTE nCharSet,BYTE nOutPrecision,
		BYTE nClipPrecision,BYTE nQuality,BYTE nPitchAndFamily,
		LPCTSTR lpszFacename);
	BOOL CreatePointFont(int nPointSize,LPCTSTR lpszFaceName,HDC hDC=NULL);
	BOOL CreatePointFontIndirect(const LOGFONT* lpLogFont,HDC hDC=NULL);

	operator HFONT() const;
	int GetLogFont(LOGFONT* pLogFont);
};

class CBitmap : public CGdiObject
{
public:
	CBitmap();
	CBitmap(HBITMAP hObject);

	BOOL LoadBitmap(LPCTSTR lpszResourceName);
	BOOL LoadBitmap(UINT nIDResource);
	BOOL LoadOEMBitmap(UINT nIDBitmap);
	BOOL LoadOEMBitmap(UINT nIDBitmap,int sizeX,int sizeY);


	BOOL CreateBitmap(int nWidth,int nHeight,UINT nPlanes,UINT nBitcount,const void* lpBits);
	BOOL CreateBitmapIndirect(LPBITMAP lpBitmap);
	BOOL CreateCompatibleBitmap(HDC hDC,int nWidth,int nHeight);

	BOOL CreateDIBitmap(HDC hdc,CONST BITMAPINFOHEADER *lpbmih,DWORD fdwInit,CONST VOID *lpbInit,CONST BITMAPINFO *lpbmi,UINT fuUsage);
	BOOL CreateDIBSection(HDC hdc,CONST BITMAPINFO *pbmi,UINT iUsage,VOID **ppvBits,HANDLE hSection,DWORD dwOffset);

	operator HBITMAP() const;
	
	DWORD SetBitmapBits(DWORD dwCount, const void* lpBits);
	DWORD GetBitmapBits(DWORD dwCount, LPVOID lpBits) const;
	CSize SetBitmapDimension(int nWidth, int nHeight);
	CSize GetBitmapDimension() const;
	BOOL GetBitmapDimension(CSize& dim) const;
	CSize GetBitmapSize(HDC hDC=NULL) const;
	BOOL GetBitmapSize(CSize& sz,HDC hDC=NULL) const;

};

class CPalette : public CGdiObject
{
public:
	CPalette();
	CPalette(HPALETTE hObject);
	
	BOOL CreatePalette(LPLOGPALETTE lpLogPalette);
	BOOL CreateHalftonePalette(HDC hDC);

	operator HPALETTE() const;
	int GetEntryCount();
	UINT GetPaletteEntries(UINT nStartIndex, UINT nNumEntries,
			LPPALETTEENTRY lpPaletteColors) const;
	UINT SetPaletteEntries(UINT nStartIndex, UINT nNumEntries,
			LPPALETTEENTRY lpPaletteColors);

	void AnimatePalette(UINT nStartIndex, UINT nNumEntries,
			LPPALETTEENTRY lpPaletteColors);
	UINT GetNearestPaletteIndex(COLORREF crColor) const;
	BOOL ResizePalette(UINT nNumEntries);
};

class CRgn : public CGdiObject
{
public:
	CRgn();
	CRgn(HRGN hObject);

	operator HRGN() const;

	BOOL CreateRectRgn(int x1, int y1, int x2, int y2);
	BOOL CreateRectRgnIndirect(LPCRECT lpRect);
	BOOL CreateEllipticRgn(int x1, int y1, int x2, int y2);
	BOOL CreateEllipticRgnIndirect(LPCRECT lpRect);
	BOOL CreatePolygonRgn(LPPOINT lpPoints, int nCount, int nMode);
	BOOL CreatePolyPolygonRgn(LPPOINT lpPoints, LPINT lpPolyCounts,
			int nCount, int nPolyFillMode);
	BOOL CreateRoundRectRgn(int x1, int y1, int x2, int y2, int x3, int y3);
	BOOL CreateFromPath(HDC hDC);
	BOOL CreateFromData(const XFORM* lpXForm, int nCount,
		const RGNDATA* pRgnData);

	void SetRectRgn(int x1, int y1, int x2, int y2);
	void SetRectRgn(LPCRECT lpRect);
	int CombineRgn(HRGN hRgn1,HRGN hRgn2,int nCombineMode);
	BOOL EqualRgn(HRGN hRgn) const;
	int OffsetRgn(int x, int y);
	int OffsetRgn(POINT point);
	int GetRgnBox(LPRECT lpRect) const;
	BOOL PtInRegion(int x, int y) const;
	BOOL PtInRegion(POINT point) const;
	BOOL RectInRegion(LPCRECT lpRect) const;
	int GetRegionData(LPRGNDATA lpRgnData, int nCount) const;
};

class CWaitCursor
{
public:
	CWaitCursor();
	~CWaitCursor();
	void Restore();
protected:
	HCURSOR m_hOldCursor;
};

class CRegKey
{
public:
	enum Status {
		createNew=0x0,
		openExist=0x1,
		samCreateLink=0x2,
		samCreateSubkey=0x4,
		samEnumerateSubkeys=0x8,
		samExecute=0x10,
		samNotify=0x20,
		samQueryValue=0x40,
		samSetValue=0x80,
		
		samRead=samQueryValue|samNotify|samEnumerateSubkeys,
		samWrite=samCreateSubkey|samSetValue,
		samAll=0xFE,
		
		optionVolatile=0x100,
		optionNoVolatile=0x200,

		defRead=openExist|samRead,
		defWrite=createNew|samAll
	};

	CRegKey();
	CRegKey(HKEY hKey);
	CRegKey(HKEY hKey,LPCSTR lpszSubKey,DWORD fStatus=CRegKey::defWrite,LPSECURITY_ATTRIBUTES lpSecurityAttributes=NULL);
	virtual ~CRegKey();

	LONG OpenKey(HKEY hKey,LPCSTR lpszSubKey,DWORD fStatus=CRegKey::createNew|CRegKey::samAll,LPSECURITY_ATTRIBUTES lpSecurityAttributes=NULL);
	BOOL OpenRead(HKEY hKey,LPCSTR lpszSubKey) { return OpenKey(hKey,lpszSubKey,CRegKey::defRead,NULL)==ERROR_SUCCESS; }
	BOOL OpenWrite(HKEY hKey,LPCSTR lpszSubKey) { return OpenKey(hKey,lpszSubKey,CRegKey::defWrite,NULL)==ERROR_SUCCESS; }
	
	
	LONG CloseKey();
	LONG UnLoadKey(LPCTSTR lpszSubKey);
	BYTE FlushKey();
	
	HKEY m_hKey;
	HKEY GetHandleKey() const;
	operator HKEY() const;

	DWORD QueryValue(LPCSTR lpszValueName,LPSTR lpbData,DWORD cbData,LPDWORD lpdwType=NULL) const;	
	BOOL QueryValue(LPCSTR lpszValueName,CString& strData) const;	
	BOOL QueryValue(LPCSTR lpszValueName,DWORD& dwData) const;	
	
	DWORD QueryValueLength(LPCSTR lpszValueName=NULL) const;
	DWORD QueryValueLength(LPCSTR lpszValueName,BOOL& bIsOk) const;
	
	LONG SetValue(LPCSTR lpValueName,LPCSTR lpData,DWORD cbData,DWORD dwType=REG_BINARY);
	BOOL SetValue(LPCSTR lpValueName,CString& strData);
	LONG SetValue(LPCSTR lpValueName,LPCSTR strData);
	BOOL SetValue(LPCSTR lpValueName,DWORD dwData);
	
	

	DWORD EnumKey(DWORD iSubkey,LPSTR lpszName,DWORD cchName,LPSTR lpszClass=NULL,LPDWORD lpcchClass=NULL,PFILETIME lpftLastWrite=NULL) const;
	BOOL EnumKey(DWORD iSubkey,CString& strName,LPSTR lpszClass=NULL,LPDWORD lpcchClass=NULL,PFILETIME lpftLastWrite=NULL) const;
	DWORD EnumValue(DWORD iValue,LPSTR lpszValue,DWORD cchValue,LPDWORD lpdwType=NULL,LPBYTE lpbData=NULL,LPDWORD lpcbData=0) const;
	BOOL EnumValue(DWORD iValue,CString& strName,LPDWORD lpdwType=NULL,LPBYTE lpbData=NULL,LPDWORD lpcbData=0) const;
	
	

	LONG DeleteKey(LPCSTR lpszSubKey);
	LONG DeleteValue(LPCSTR lpszValue); // does not remove subkeys on WinNT
	static BOOL DeleteKey(HKEY hKey,LPCSTR szKey); // Removes also subkeys
	
	LONG GetKeySecurity(SECURITY_INFORMATION SecInf,PSECURITY_DESCRIPTOR pSecDesc,LPDWORD lpcbSecDesc) const;
	LONG SetKeySecurity(SECURITY_INFORMATION si,PSECURITY_DESCRIPTOR psd);

	LONG LoadKey(LPCSTR lpszSubKey,LPCSTR lpszFile);
	LONG SaveKey(LPCSTR lpszFile,LPSECURITY_ATTRIBUTES lpsa);
	LONG ReplaceKey(LPCSTR lpSubKey,LPCSTR lpNewFile,LPCSTR lpOldFile);
	LONG RestoreKey(LPCSTR lpszFile,DWORD fdw);

	
	static LONG CopyKey(HKEY hSource,HKEY hDestination);
	static LONG RenameSubKey(HKEY hKey,LPCSTR szOldName,LPCSTR szNewName);
	
	LONG CopyKey(HKEY hDestination);
	LONG RenameSubKey(LPCSTR szOldName,LPCSTR szNewName);

	LONG NotifyChangeKeyValue(BOOL fWatchSubTree,DWORD fdwNotifyFilter,HANDLE hEvent,BOOL fAsync);
	LONG QueryInfoKey(LPTSTR lpszClass,LPDWORD lpcchClass,LPDWORD lpcSubKeys,LPDWORD lpcchMaxSubkey,
		LPDWORD lpcchMaxClass,LPDWORD lpcValues,LPDWORD lpcchMaxValueName,LPDWORD lpcbMaxValueData,
		LPDWORD lpcbSecurityDescriptor,PFILETIME lpftLastWriteTime) const;

#ifdef DEF_WCHAR
	CRegKey(HKEY hKey,LPCWSTR lpszSubKey,DWORD fStatus=CRegKey::defWrite,LPSECURITY_ATTRIBUTES lpSecurityAttributes=NULL);

	LONG OpenKey(HKEY hKey,LPCWSTR lpszSubKey,DWORD fStatus=CRegKey::createNew|CRegKey::samAll,LPSECURITY_ATTRIBUTES lpSecurityAttributes=NULL);
	BOOL OpenRead(HKEY hKey,LPCWSTR lpszSubKey) { return OpenKey(hKey,lpszSubKey,CRegKey::defRead,NULL)==ERROR_SUCCESS; }
	BOOL OpenWrite(HKEY hKey,LPCWSTR lpszSubKey) { return OpenKey(hKey,lpszSubKey,CRegKey::defWrite,NULL)==ERROR_SUCCESS; }
	
	DWORD QueryValue(LPCWSTR lpszValueName,LPSTR lpbData,DWORD cbDataLen,LPDWORD lpdwType=NULL) const;	
	DWORD QueryValue(LPCWSTR lpszValueName,LPWSTR lpStr,DWORD cbLenAsChars) const;	
	BOOL QueryValue(LPCWSTR lpszValueName,CStringW& strData) const;	
	BOOL QueryValue(LPCWSTR lpszValueName,DWORD& dwData) const;	
	
	DWORD QueryValueLength(LPCWSTR lpszValueName) const;
	DWORD QueryValueLength(LPCWSTR lpszValueName,BOOL& bIsOk) const;

	BOOL SetValue(LPCWSTR lpValueName,CStringW& strData);
	LONG SetValue(LPCWSTR lpValueName,LPCSTR lpData,DWORD cbData,DWORD dwType=REG_BINARY);
	LONG SetValue(LPCWSTR lpValueName,LPCWSTR strData,DWORD cbDataAsChars,DWORD dwType=REG_SZ);
	LONG SetValue(LPCWSTR lpValueName,LPCWSTR strData);
	BOOL SetValue(LPCWSTR lpValueName,DWORD dwData);

	DWORD EnumKey(DWORD iSubkey,LPWSTR lpszName,DWORD cchName,LPWSTR lpszClass=NULL,DWORD* lpcchClass=NULL,PFILETIME lpftLastWrite=NULL) const;
	BOOL EnumKey(DWORD iSubkey,CStringW& strName,LPWSTR lpszClass=NULL,LPDWORD lpcchClass=NULL,PFILETIME lpftLastWrite=NULL) const;
	DWORD EnumValue(DWORD iValue,LPWSTR lpszValue,DWORD cchValue,LPDWORD lpdwType=NULL,LPBYTE lpbData=NULL,LPDWORD lpcbData=0) const;
	BOOL EnumValue(DWORD iValue,CStringW& strName,LPDWORD lpdwType=NULL,LPBYTE lpbData=NULL,LPDWORD lpcbData=0) const;

	LONG DeleteKey(LPCWSTR lpszSubKey);
	LONG DeleteValue(LPCWSTR lpszValue); // does not remove subkeys on WinNT
	static BOOL DeleteKey(HKEY hKey,LPCWSTR szKey); // Removes also subkeys

	LONG LoadKey(LPCWSTR lpszSubKey,LPCWSTR lpszFile);
	LONG SaveKey(LPCWSTR lpszFile,LPSECURITY_ATTRIBUTES lpsa);
	LONG ReplaceKey(LPCWSTR lpSubKey,LPCWSTR lpNewFile,LPCWSTR lpOldFile);
	LONG RestoreKey(LPCWSTR lpszFile,DWORD fdw);

	static LONG RenameSubKey(HKEY hKey,LPCWSTR szOldName,LPCWSTR szNewName);
	
	LONG RenameSubKey(LPCWSTR szOldName,LPCWSTR szNewName);

#endif

};

#endif

/////////////////////////////////////////////////
// Inline functions
/////////////////////////////////////////////////

#include "General.inl"
#include "GdiObject.inl"
#include "File.inl"



#endif
