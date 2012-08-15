////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////

#include "HFCLib.h"




/////////////////////////////////////////////
// CFile

#ifndef WIN32

CFile::CFile()
:	m_bCloseOnDelete(FALSE),m_nOpenFlags(0)
{
	m_hFile=(HANDLE)hFileNull;
}

CFile::CFile(BOOL bThrowExceptions)
:	m_bCloseOnDelete(FALSE),m_nOpenFlags(0),CExceptionObject(bThrowExceptions)
{
	m_hFile=(HANDLE)hFileNull;
}

CFile::CFile(HANDLE hFile,BOOL bThrowExceptions)
:	m_hFile(hFile),m_bCloseOnDelete(FALSE),m_nOpenFlags(0),
	CExceptionObject(bThrowExceptions)
{
}

CFile::CFile(FILE* pFile,BOOL bThrowExceptions)
:	m_hFile(HANDLE(pFile)),m_bCloseOnDelete(FALSE),m_nOpenFlags(0),
	CExceptionObject(bThrowExceptions)
{
}

CFile::CFile(LPCSTR lpszFileName,int nOpenFlags,CFileException* e)
:	m_hFile(HANDLE(hFileNull)),m_bCloseOnDelete(FALSE)
{
	Open(lpszFileName,nOpenFlags,e);
}

CFile::CFile(LPCWSTR lpszFileName,int nOpenFlags,CFileException* e)
:	m_hFile(HANDLE(hFileNull)),m_bCloseOnDelete(FALSE)
{
	Open(lpszFileName,nOpenFlags,e);
}

CFile::CFile(LPCSTR lpszFileName,int nOpenFlags,bThrowExceptions)
:	m_hFile(HANDLE(hFileNull)),m_bCloseOnDelete(FALSE),
	CExceptionObject(bThrowExceptions)
{
	Open(lpszFileName,nOpenFlags,e);
}

CFile::CFile(LPCWSTR lpszFileName,int nOpenFlags,bThrowExceptions)
:	m_hFile(HANDLE(hFileNull)),m_bCloseOnDelete(FALSE),
	CExceptionObject(bThrowExceptions)
{
	Open(lpszFileName,nOpenFlags,e);
}

#endif




BOOL CFile::GetStatus(CFileStatus& rStatus) const
{
#ifdef WIN32
	BY_HANDLE_FILE_INFORMATION fi;
	if (!GetFileInformationByHandle(m_hFile,&fi))
		return FALSE;
	rStatus.m_ctime=fi.ftCreationTime;
	rStatus.m_mtime=fi.ftLastWriteTime;
	rStatus.m_atime=fi.ftLastAccessTime;
	rStatus.m_size=fi.nFileSizeLow;
	rStatus.m_attribute=fi.dwFileAttributes;
#else
	struct ffblk find;
	if (findfirst((LPCTSTR)m_strFileName,&find,FA_RDONLY|FA_HIDDEN|FA_SYSTEM|FA_LABEL|FA_DIREC|FA_ARCH))
		return FALSE;
	CTime tim(find.ff_fdate,find.ff_ftime);
	rStatus.m_ctime=tim;
	rStatus.m_mtime=tim;
	rStatus.m_atime=tim;
	rStatus.m_size=find.ff_fsize;
	rStatus.m_attribute=find.ff_attrib;
#endif
	StringCbCopy(rStatus.m_szFullName,MAX_PATH,W2A(m_strFileName));
	return TRUE;
}


CString CFile::GetFileTitle() const
{
#ifdef WIN32
	if (IsUnicodeSystem())
	{
		WCHAR szTitle[2000];
		::GetFileTitleW(m_strFileName,szTitle,2000);
		return szTitle;
	}
	else
	{
		char szTitle[2000];
		::GetFileTitleA(W2A(m_strFileName),szTitle,2000);
		return szTitle;

	}
	
#else
	CString str(m_strFileName+LastCharIndex(m_strFileName,'\\')+1);
	return str;
#endif
}

#ifdef DEF_WCHAR
CStringW CFile::GetFileTitleW() const
{
	if (IsUnicodeSystem())
	{
		WCHAR szTitle[2000];
		::GetFileTitleW(m_strFileName,szTitle,2000);
		return szTitle;
	}
	else
	{
		char szTitle[2000];
		::GetFileTitleA(W2A(m_strFileName),szTitle,2000);
		return szTitle;
	}


}
#endif


BOOL CFile::Open(LPCSTR lpszFileName, int nOpenFlags)
{
	if (m_bCloseOnDelete)
		Close();
	m_bCloseOnDelete=TRUE;
	m_nOpenFlags=nOpenFlags;
#ifdef WIN32
	
	// Obtaining full path name
	{
		LPSTR szTemp;
		char szFullPathA[MAX_PATH];
		DWORD dwLength=::GetFullPathName(lpszFileName,MAX_PATH,szFullPathA,&szTemp);
		if (dwLength==0)
			m_strFileName=lpszFileName;
		else
			m_strFileName.Copy(lpszFileName,dwLength);
	}
    

	DWORD dwShare;
	switch (nOpenFlags&shareFlags)
	{
	case shareCompat:
		dwShare=0;
		break;
	case shareDenyWrite:
		dwShare=FILE_SHARE_READ;
		break;
	case shareDenyRead:
		dwShare=FILE_SHARE_WRITE;
		break;
	case shareDenyNone:
		dwShare=FILE_SHARE_WRITE|FILE_SHARE_READ;
		break;
	default:
		if (m_bThrow)
			throw CException(CException::invalidParameter);
		dwShare=0;
		break;
	}


	if ((nOpenFlags&openFlags)==0)
	{
		if (nOpenFlags&modeWrite)
			nOpenFlags|=openCreateAlways;
		else
			nOpenFlags|=openExisting;
	}

	if (nOpenFlags&otherInherit)
	{
		SECURITY_ATTRIBUTES sa;
		sa.nLength=sizeof(sa);
		sa.lpSecurityDescriptor=NULL;
		sa.bInheritHandle=TRUE;
		m_hFile=::CreateFile(lpszFileName,nOpenFlags&modeFlags,dwShare,&sa,nOpenFlags&openFlags,FILE_ATTRIBUTE_NORMAL,NULL);
	}
	else
		m_hFile=::CreateFile(lpszFileName,nOpenFlags&modeFlags,dwShare,NULL,nOpenFlags&openFlags,FILE_ATTRIBUTE_NORMAL,NULL);

	
	if (m_hFile==INVALID_HANDLE_VALUE)
	{
		SetHFCError(HFC_CANNOTOPEN);
		if (m_pFileException!=NULL)
		{
			m_pFileException->m_lOsError = ::GetLastError();
			m_pFileException->m_cause = CFileException::OsErrorToException(m_pFileException->m_lOsError);
			m_pFileException->m_strFileName = lpszFileName;
			if (m_bThrow)
				throw *m_pFileException;
		}
		else if (m_bThrow)
			throw CFileException(CFileException::OsErrorToException(::GetLastError()),::GetLastError(),lpszFileName);
		return FALSE;
	}
	
	DebugOpenHandle(dhtFile,m_hFile,m_strFileName);

	if (m_pFileException!=NULL)
		m_pFileException->m_cause=CException::none;
	return TRUE;
#else
	char mode[4]={ 0,0,0,0 };

	m_strFileName = lpszFileName;
	
	switch (nOpenFlags&(openFlags|modeFlags))
	{
	case modeWrite|openCreateNew:
		if (IsFile(lpszFileName))
			return FALSE;
	case modeWrite|openCreateAlways:
		mode[0]='w';
		break;
	case modeWrite|openTruncate:
		if (!IsFile(lpszFileName))
			return FALSE;
		mode[0]='w';
		break;
	case modeWrite|openExisting:
		if (!IsFile(lpszFileName))
			return FALSE;
	case modeWrite|openAlways:
		mode[0]='a';
		break;
	case modeReadWrite|openTruncate:
		if (!IsFile(lpszFileName))
			return FALSE;
	case modeReadWrite|openCreateAlways:
		mode[0]='w';
		mode[1]='+';
        break;
	case modeReadWrite|openCreateNew:
		if (IsFile(lpszFileName))
			return FALSE;
		mode[0]='w';
		mode[1]='+';
        break;
	case modeReadWrite|openExisting:
		if (!IsFile(lpszFileName))
			return FALSE;
	case modeReadWrite|openAlways:
		mode[0]='r';
		mode[1]='+';
		break;
	case modeReadWrite|modeNoTruncate|modeCreate:
		mode[0]='a';
		mode[1]='+';
		break;
	default:
		mode[0]='r';
		break;
	}
	if (nOpenFlags&typeText)
		strcat(mode,"t");
	else
		strcat(mode,"b");

        m_hFile=(HANDLE)fopen(lpszFileName,mode);
	if (m_hFile==NULL)
	{
		SetHFCError(HFC_CANNOTCREATE);
		if (m_pFileException != NULL)
		{
			if (nOpenFlags&modeWrite)
				m_pFileException->m_cause = CFileException::invalidFile;
			else
				m_pFileException->m_cause = CFileException::fileNotFound;
			m_pFileException->m_strFileName = lpszFileName;
			if (m_bThrow)
				throw *pError;
		}
		else if (m_bThrow)
		{
			if (nOpenFlags&modeWrite)
				throw CFileException(CFileException::invalidFile,lpszFileName);
			else
				throw CFileException(CFileException::fileNotFound,lpszFileName);
		}
		return FALSE;
	}
	return TRUE;
#endif
}

#ifdef DEF_WCHAR

BOOL CFile::Open(LPCWSTR lpszFileName, int nOpenFlags)
{
	if (!IsUnicodeSystem())
		return Open(W2A(lpszFileName),nOpenFlags);

	if (m_bCloseOnDelete)
		Close();
	m_bCloseOnDelete=TRUE;
	m_nOpenFlags=nOpenFlags;
	
	// Obtaining full path name
	LPWSTR szTemp;
	DWORD dwLength=GetFullPathNameW(lpszFileName,MAX_PATH,m_strFileName.GetBuffer(MAX_PATH),&szTemp);
    if (dwLength==0)
		m_strFileName=lpszFileName;
	else if (dwLength>MAX_PATH)
		dwLength=GetFullPathNameW(lpszFileName,dwLength+2,m_strFileName.GetBuffer(dwLength+2),&szTemp);
	m_strFileName.FreeExtra(dwLength);
	

	DWORD dwShare;
	switch (nOpenFlags&shareFlags)
	{
	case shareCompat:
		dwShare=0;
		break;
	case shareDenyWrite:
		dwShare=FILE_SHARE_READ;
		break;
	case shareDenyRead:
		dwShare=FILE_SHARE_WRITE;
		break;
	case shareDenyNone:
		dwShare=FILE_SHARE_WRITE|FILE_SHARE_READ;
		break;
	default:
		if (m_bThrow)
			throw CException(CException::invalidParameter);
		dwShare=0;
		break;
	}


	if ((nOpenFlags&openFlags)==0)
	{
		if (nOpenFlags&modeWrite)
			nOpenFlags|=openCreateAlways;
		else
			nOpenFlags|=openExisting;
	}

	if (nOpenFlags&otherInherit)
	{
		SECURITY_ATTRIBUTES sa;
		sa.nLength=sizeof(sa);
		sa.lpSecurityDescriptor=NULL;
		sa.bInheritHandle=TRUE;
		m_hFile=::CreateFileW(lpszFileName,nOpenFlags&modeFlags,dwShare,&sa,nOpenFlags&openFlags,FILE_ATTRIBUTE_NORMAL,NULL);
	}
	else
		m_hFile=::CreateFileW(lpszFileName,nOpenFlags&modeFlags,dwShare,NULL,nOpenFlags&openFlags,FILE_ATTRIBUTE_NORMAL,NULL);

	
	if (m_hFile==INVALID_HANDLE_VALUE)
	{
		SetHFCError(HFC_CANNOTOPEN);
		if (m_pFileException!=NULL)
		{
			m_pFileException->m_lOsError = ::GetLastError();
			m_pFileException->m_cause = CFileException::OsErrorToException(m_pFileException->m_lOsError);
			m_pFileException->m_strFileName = lpszFileName;
			if (m_bThrow)
				throw *m_pFileException;
		}
		else if (m_bThrow)
			throw CFileException(CFileException::OsErrorToException(::GetLastError()),::GetLastError(),m_strFileName);
		return FALSE;
	}

	DebugOpenHandle(dhtFile,m_hFile,m_strFileName);

	if (m_pFileException!=NULL)
		m_pFileException->m_cause=CException::none;
	return TRUE;
}
#endif


DWORD CFile::Seek(LONG lOff, SeekPosition nFrom,LONG* pHighPos)
{
	DWORD dwNew=::SetFilePointer(m_hFile,lOff,pHighPos,(DWORD)nFrom);
	if (dwNew  == (DWORD)-1)
	{
		SetHFCError(HFC_BADSEEK);
		if (m_pFileException != NULL)
		{
			m_pFileException->m_lOsError = ::GetLastError();
			m_pFileException->m_cause = CFileException::OsErrorToException(m_pFileException->m_lOsError);
			m_pFileException->m_strFileName = m_strFileName;
			if (m_bThrow)
				throw *m_pFileException;
		}
		else if (m_bThrow)
			throw CFileException(CFileException::OsErrorToException(::GetLastError()),::GetLastError(),m_strFileName);
		return FALSE;
	}
	return dwNew;
}



DWORD CFile::Read(void* lpBuf, DWORD nCount) const
{
	if (nCount == 0)
		return 0;

	DWORD dwRead;
	BOOL ret=::ReadFile(m_hFile,lpBuf,nCount,&dwRead,NULL);
	if (ret && dwRead<nCount && m_nOpenFlags&otherErrorWhenEOF) 
	{
		SetHFCError(HFC_ENDOFFILE);
		if (m_pFileException!=NULL)
		{
			m_pFileException->m_lOsError = ERROR_HANDLE_EOF;
			m_pFileException->m_cause = (CException::exceptionCode)CFileException::endOfFile;
			m_pFileException->m_strFileName = m_strFileName;
			if (m_bThrow)
				throw *m_pFileException;
		}
		else if (m_bThrow)
			throw CFileException(CFileException::endOfFile,ERROR_HANDLE_EOF,m_strFileName);
		return 0;
	}
	else if (!ret)
	{
		SetHFCError(HFC_CANNOTREAD);
		if (m_pFileException!=NULL)
		{
			m_pFileException->m_lOsError = ::GetLastError();
			m_pFileException->m_cause = CFileException::OsErrorToException(m_pFileException->m_lOsError);
			m_pFileException->m_strFileName = m_strFileName;
			if (m_bThrow)
				throw *m_pFileException;
		}
		else if (m_bThrow)
			throw CFileException (CFileException::OsErrorToException(::GetLastError()),::GetLastError(),m_strFileName);
		return 0;
	}
	return (UINT)dwRead;
}

BOOL CFile::Read(CStringA& str) const
{
	str.Empty();
	CHAR bChar;

	DWORD dwRead;
	
	if (m_nOpenFlags&typeText)
	{
		BOOL ret=::ReadFile(m_hFile,&bChar,1,&dwRead,NULL);
		if (ret && dwRead)
		{
			do {
				if (bChar=='\r' || bChar=='\n' || bChar=='\t' || bChar==' ')
					return TRUE;	
				str<< bChar;
				ret=::ReadFile(m_hFile,&bChar,1,&dwRead,NULL);
			}
			while (ret && dwRead);        			
			return TRUE;
		}
		if (ret) // End of file
		{
			if (str.IsEmpty() && m_nOpenFlags&otherErrorWhenEOF)
			{
				SetHFCError(HFC_ENDOFFILE);
				if (m_pFileException!=NULL)
				{
					m_pFileException->m_lOsError = ERROR_HANDLE_EOF;
					m_pFileException->m_cause = (CException::exceptionCode)CFileException::endOfFile;
					m_pFileException->m_strFileName = m_strFileName;
					if (m_bThrow)
						throw *m_pFileException;
				}
				else if (m_bThrow)
					throw CFileException(CFileException::endOfFile,ERROR_HANDLE_EOF,m_strFileName);
                return 0;
			}
			return TRUE;
		}
	}
	else
	{
		BOOL ret=::ReadFile(m_hFile,&bChar,1,&dwRead,NULL);
		while (ret && dwRead)
		{
			if (bChar=='\0')
				return TRUE;
			str << bChar;
			ret=::ReadFile(m_hFile,&bChar,1,&dwRead,NULL);
		}
		if (ret) // End of file
		{
			if (str.IsEmpty() && m_nOpenFlags&otherErrorWhenEOF)
			{
				SetHFCError(HFC_ENDOFFILE);
				if (m_pFileException!=NULL)
				{
					m_pFileException->m_lOsError = ERROR_HANDLE_EOF;
					m_pFileException->m_cause = (CException::exceptionCode)CFileException::endOfFile;
					m_pFileException->m_strFileName = m_strFileName;
					if (m_bThrow)
						throw *m_pFileException;
				}
				else if (m_bThrow)
					throw CFileException(CFileException::endOfFile,ERROR_HANDLE_EOF,m_strFileName);
                return 0;
			}
			return TRUE;
		}
	}

	if (m_pFileException!=NULL)
	{
		m_pFileException->m_lOsError = ::GetLastError();
		m_pFileException->m_cause = CFileException::OsErrorToException(m_pFileException->m_lOsError);
		m_pFileException->m_strFileName = m_strFileName;
		if (m_bThrow)
			throw *m_pFileException;
	}
	else if (m_bThrow)
		throw CFileException (CFileException::OsErrorToException(::GetLastError()),::GetLastError(),m_strFileName);
	
	return FALSE;

}

#ifdef DEF_WCHAR
BOOL CFile::Read(CStringW& str) const
{
	str.Empty();
	WCHAR bChar;

	DWORD dwRead;
	
	if (m_nOpenFlags&typeText)
	{
		BOOL ret=::ReadFile(m_hFile,&bChar,2,&dwRead,NULL);
		if (ret && dwRead)
		{
			do {
				if (bChar==L'\r' || bChar==L'\n' || bChar==L'\t' || bChar==L' ')
					return TRUE;	
				str<< bChar;
				ret=::ReadFile(m_hFile,&bChar,2,&dwRead,NULL);
			}
			while (ret && dwRead);           			
			return TRUE;
		}
		if (ret) // End of file
		{
			if (str.IsEmpty() && m_nOpenFlags&otherErrorWhenEOF)
			{
				SetHFCError(HFC_ENDOFFILE);
				if (m_pFileException!=NULL)
				{
					m_pFileException->m_lOsError = ERROR_HANDLE_EOF;
					m_pFileException->m_cause = (CException::exceptionCode)CFileException::endOfFile;
					m_pFileException->m_strFileName = m_strFileName;
					if (m_bThrow)
						throw *m_pFileException;
				}
				else if (m_bThrow)
					throw CFileException(CFileException::endOfFile,ERROR_HANDLE_EOF,m_strFileName);
                return 0;
			}
			return TRUE;
		}
	}
	else
	{
		BOOL ret=::ReadFile(m_hFile,&bChar,2,&dwRead,NULL);
		while (ret && dwRead)
		{
			if (bChar=='\0')
				return TRUE;
			str << bChar;
			ret=::ReadFile(m_hFile,&bChar,2,&dwRead,NULL);
		}
		if (ret) // End of file
		{
			if (str.IsEmpty() && m_nOpenFlags&otherErrorWhenEOF)
			{
				SetHFCError(HFC_ENDOFFILE);
				if (m_pFileException!=NULL)
				{
					m_pFileException->m_lOsError = ERROR_HANDLE_EOF;
					m_pFileException->m_cause = (CException::exceptionCode)CFileException::endOfFile;
					m_pFileException->m_strFileName = m_strFileName;
					if (m_bThrow)
						throw *m_pFileException;
				}
				else if (m_bThrow)
					throw CFileException(CFileException::endOfFile,ERROR_HANDLE_EOF,m_strFileName);
                return 0;
			}
			return TRUE;
		}
	}

	if (m_pFileException!=NULL)
	{
		m_pFileException->m_lOsError = ::GetLastError();
		m_pFileException->m_cause = CFileException::OsErrorToException(m_pFileException->m_lOsError);
		m_pFileException->m_strFileName = m_strFileName;
		if (m_bThrow)
			throw *m_pFileException;
	}
	else if (m_bThrow)
		throw CFileException (CFileException::OsErrorToException(::GetLastError()),::GetLastError(),m_strFileName);
	
	return FALSE;

}
#endif

BOOL CFile::Write(const void* lpBuf, DWORD nCount)
{
	if (nCount == 0)
		return TRUE;

	DWORD nWritten;
	BOOL ret=::WriteFile(m_hFile,lpBuf,nCount,&nWritten,NULL);
	if (!ret || nWritten<nCount)
	{
		if (m_pFileException!=NULL)
		{
			m_pFileException->m_lOsError = ::GetLastError();
			m_pFileException->m_cause = CFileException::OsErrorToException(m_pFileException->m_lOsError);
			m_pFileException->m_strFileName = m_strFileName;
			if (m_bThrow)
				throw *m_pFileException;
		}
		else if (m_bThrow)
			throw CFileException(CFileException::OsErrorToException(::GetLastError()),::GetLastError(),m_strFileName);
		return FALSE;
	}
	return TRUE;
}

#ifdef DEF_WCHAR
BOOL CFile::Write(LPCWSTR szString,DWORD dwCount) 
{ 
	return CFile::Write((const void*)szString,(DWORD)(2*dwCount)); 
}
#endif 

BOOL CFile::Close()
{
	BOOL bError = FALSE;
	if (m_hFile != FILE_NULL)
		bError = !::CloseHandle(m_hFile);
	DebugCloseHandle(dhtFile,m_hFile,W2A(m_strFileName));
	m_hFile = FILE_NULL;
	m_bCloseOnDelete = FALSE;
	m_strFileName.Empty();
	return bError;
}

BOOL CFile::IsEndOfFile() const
{
	DWORD dwPos=::SetFilePointer(m_hFile,0,NULL,FILE_CURRENT);
    if (dwPos==(DWORD)-1)
	{
		SetHFCError(HFC_ERROR);
		if (m_bThrow)
			throw CFileException(CFileException::badSeek,GetLastError(),m_strFileName);
		return FALSE;
	}
	return dwPos==::GetFileSize(m_hFile,NULL);
}


DWORD CFile::GetLength(DWORD* pHigh) const
{
	return ::GetFileSize(m_hFile,pHigh);
}

ULONGLONG CFile::GetLength64() const
{
	DWORD high;
	DWORD low=::GetFileSize(m_hFile,&high);

	return (((ULONGLONG)high)<<32)|((ULONGLONG)low);
}

#ifdef WIN32
void CFile::LockRange(DWORD dwPos, DWORD dwCount)
{
	::LockFile(m_hFile,dwPos,0,dwCount,0);
}

void CFile::UnlockRange(DWORD dwPos, DWORD dwCount)
{
	::UnlockFile(m_hFile,dwPos,0,dwCount,0);
}
#endif

BOOL CFile::Flush()
{
	return ::FlushFileBuffers(m_hFile);
}


	

ULONG CFile::GetPosition(PLONG pHigh) const
{
	DWORD dwPos=::SetFilePointer(m_hFile,0,pHigh,FILE_CURRENT);
	if (dwPos==(DWORD)-1)
	{
		SetHFCError(HFC_ERROR);
		if (m_bThrow)
			throw CFileException(CFileException::badSeek,GetLastError(),m_strFileName);
		return FALSE;
	}
	return dwPos;
}



ULONGLONG CFile::GetPosition64() const
{
	LONG high=0;
	DWORD dwPos=::SetFilePointer(m_hFile,0,&high,FILE_CURRENT);
	if (dwPos==(DWORD)-1)
	{
		SetHFCError(HFC_ERROR);
		if (m_bThrow)
			throw CFileException(CFileException::badSeek,GetLastError(),m_strFileName);
		return 0;
	}
	return (((ULONGLONG)high)<<32)|((ULONGLONG)dwPos);
}

BOOL CFile::GetFileTime(LPSYSTEMTIME lpCreationTime,LPSYSTEMTIME lpLastAccessTime,LPSYSTEMTIME lpLastWriteTime) const
{
	FILETIME cr,la,lw;
	if (!::GetFileTime(m_hFile,&cr,&la,&lw))
		return FALSE;

	if (lpCreationTime!=NULL)
		FileTimeToSystemTime(&cr,lpCreationTime);
	if (lpLastAccessTime!=NULL)
		FileTimeToSystemTime(&la,lpLastAccessTime);
	if (lpLastWriteTime!=NULL)
		FileTimeToSystemTime(&lw,lpLastWriteTime);

	return TRUE;
}


BOOL CFile::SetFileTime(const SYSTEMTIME* lpCreationTime,const SYSTEMTIME* lpLastAccessTime,const SYSTEMTIME* lpLastWriteTime)
{
	FILETIME cr,la,lw;
	if (lpCreationTime!=NULL)
		SystemTimeToFileTime(lpCreationTime,&cr);
	if (lpLastAccessTime!=NULL)
		SystemTimeToFileTime(lpLastAccessTime,&la);
	if (lpLastWriteTime!=NULL)
		SystemTimeToFileTime(lpLastWriteTime,&lw);
	return ::SetFileTime(m_hFile,lpCreationTime!=NULL?&cr:NULL,
		lpLastAccessTime!=NULL?&la:NULL,lpLastWriteTime!=NULL?&lw:NULL);
}
///////////////////////////////////////////
// CFileEncoding
///////////////////////////////////////////


#ifdef DEF_WCHAR



CFileEncode::~CFileEncode()
{
}

BOOL CFileEncode::Write(WCHAR ch)
{
	if (m_nEncoding==Unicode)
		return CFile::Write((const void*)&ch,2);
	else
	{
		int iNewLen=WideCharToMultiByte(m_nEncoding==UTF8?CP_UTF8:CP_ACP,0,&ch,1,NULL,0,NULL,NULL);
		CHAR* psz=new CHAR[iNewLen+1];
		WideCharToMultiByte(m_nEncoding==UTF8?CP_UTF8:CP_ACP,0,&ch,1,psz,iNewLen+1,NULL,NULL);
		psz[iNewLen]=L'\0';
		int iRet=CFile::Write(psz,iNewLen);
		delete[] psz;
		return iRet;
	}
}

BOOL CFileEncode::Write(const CStringW& str)
{
	if (m_nEncoding==Unicode)
		return CFile::Write((const void*)(LPCWSTR)str,(str.GetLength()+(m_nOpenFlags&otherStrNullTerminated?1:0))*2);
	else
	{
		int nLen=str.GetLength()+(m_nOpenFlags&otherStrNullTerminated?1:0);
		int iNewLen=WideCharToMultiByte(m_nEncoding==UTF8?CP_UTF8:CP_ACP,0,(LPCWSTR)str,nLen,NULL,0,NULL,NULL);
		CHAR* psz=new CHAR[iNewLen+1];
		WideCharToMultiByte(m_nEncoding==UTF8?CP_UTF8:CP_ACP,0,(LPCWSTR)str,nLen,psz,iNewLen+1,NULL,NULL);
		psz[iNewLen]=L'\0';
		int iRet=CFile::Write(psz,iNewLen);
		delete[] psz;
		return iRet;
	}
}

BOOL CFileEncode::Write(LPCWSTR szNullTerminatedString)
{
	if (m_nEncoding==Unicode)
		return CFile::Write((const void*)(LPCWSTR)szNullTerminatedString,(istrlen(szNullTerminatedString)+(m_nOpenFlags&otherStrNullTerminated?1:0))*2);
	else
	{
		int iNewLen=WideCharToMultiByte(m_nEncoding==UTF8?CP_UTF8:CP_ACP,0,szNullTerminatedString,-1,NULL,0,NULL,NULL);
		CHAR* psz=new CHAR[iNewLen+1];
		WideCharToMultiByte(m_nEncoding==UTF8?CP_UTF8:CP_ACP,0,szNullTerminatedString,-1,psz,iNewLen+1,NULL,NULL);
		psz[iNewLen]=L'\0';
		int iRet=CFile::Write(psz,iNewLen);
		delete[] psz;
		return iRet;
	}
}

BOOL CFileEncode::Write(LPCWSTR szString,DWORD nCount)
{
	if (m_nEncoding==Unicode)
		return CFile::Write((const void*)(LPCWSTR)szString,nCount*2);
	else 
	{
		int iNewLen=WideCharToMultiByte(m_nEncoding==UTF8?CP_UTF8:CP_ACP,0,szString,nCount,NULL,0,NULL,NULL);
		CHAR* psz=new CHAR[iNewLen+1];
		WideCharToMultiByte(m_nEncoding==UTF8?CP_UTF8:CP_ACP,0,szString,nCount,psz,iNewLen+1,NULL,NULL);
		psz[iNewLen]=L'\0';
		int iRet=CFile::Write(psz,iNewLen);
		delete[] psz;
		return iRet;
	}	
}

#endif



///////////////////////////////////////////
// CFileFind
///////////////////////////////////////////

#ifdef WIN32
void CFileFind::Close()
{
	if (m_hFind!=NULL)
	{
		FindClose(m_hFind);
		DebugCloseHandle(dhtFileFind,m_hFind,STRNULL);
		m_hFind=NULL;
	}
}
#endif

BOOL CFileFind::FindFile(LPCSTR pstrName)
{
	strRoot.Copy(pstrName,LastCharIndex(pstrName,'\\')+1);
#ifdef WIN32
	if (m_hFind!=NULL)
	{
		::FindClose(m_hFind);
		DebugCloseHandle(dhtFileFind,m_hFind,STRNULL);
	}
	
	if (IsUnicodeSystem())
	{
		if (pstrName!=NULL)
			m_hFind=::FindFirstFileW(A2W(pstrName),&m_fdw);
		else
			m_hFind=::FindFirstFileW(L"*.*",&m_fdw);
	}
	else
	{
		if (pstrName!=NULL)
			m_hFind=::FindFirstFile(pstrName,&m_fd);
		else
			m_hFind=::FindFirstFile("*.*",&m_fd);
	}
	
	if (m_hFind==INVALID_HANDLE_VALUE)
		return FALSE;

	DebugOpenHandle(dhtFileFind,m_hFind,STRNULL);
	return TRUE;
#else
	if (pstrName==NULL)
        return !findfirst("*.*",&m_fd,FA_RDONLY|FA_HIDDEN|FA_SYSTEM|FA_DIREC|FA_ARCH);
	else
        return !findfirst(pstrName,&m_fd,FA_RDONLY|FA_HIDDEN|FA_SYSTEM|FA_DIREC|FA_ARCH);
#endif
}


BOOL CFileFind::FindNextFile()
{
#ifdef WIN32
	if (m_hFind==NULL)
		return FindFile();
	if (IsUnicodeSystem())
		return ::FindNextFileW(m_hFind,&m_fdw);
	else
		return ::FindNextFileA(m_hFind,&m_fd);
#else
	return !findnext(&m_fd);
#endif
}

void CFileFind::GetFilePath(LPSTR szPath,DWORD nMaxLen) const
{
#ifdef WIN32
	int nRet=WideCharToMultiByte(CP_ACP,0,(LPCWSTR)strRoot,(int)strRoot.GetLength(),szPath,nMaxLen,0,0);

	if (IsUnicodeSystem())
		WideCharToMultiByte(CP_ACP,0,m_fdw.cFileName,-1,szPath+nRet,nMaxLen-nRet,0,0);
	else
		StringCbCopy(szPath+nRet,nMaxLen-nRet,m_fd.cFileName);
#else
	memcpy_s(szPath,nMaxLen,strRoot,strRoot.GetLength());
	strcpy_s(szPath+strRoot.GetLength(),nMaxLen-strRoot.GetLength(),m_fd.ff_name);
#endif
}




void CFileFind::GetFilePath(CString& path) const
{
	path=strRoot;
#ifdef WIN32
	if (IsUnicodeSystem())
		path<<m_fdw.cFileName;
	else
		path<<m_fd.cFileName;
#else
	path<<m_fd.ff_name;
#endif
}

#ifdef WIN32

BOOL CFileFind::GetFileTitle(CString& title) const
{

	if (IsUnicodeSystem())
	{
		WCHAR szTitle[MAX_PATH],szPath[MAX_PATH];
		if (StringCbCopyNW(szPath,MAX_PATH*2,strRoot,strRoot.GetLength()*2)!=S_OK)
			return FALSE;

		if (StringCbCopyW(szPath+strRoot.GetLength(),(MAX_PATH-strRoot.GetLength())*2,m_fdw.cFileName)!=S_OK)
			return FALSE;

		int len=::GetFileTitleW(szPath,szTitle,MAX_PATH);
		if (len==0)
			title=szTitle;
		else
		{
			title.Empty();
			return FALSE;
		}
	}
	else
	{
		CHAR szPath[MAX_PATH];
		if (WideCharToMultiByte(CP_ACP,0,strRoot,(int)strRoot.GetLength(),szPath,MAX_PATH,0,0)==0)
			return FALSE;
		
		if (StringCbCopy(szPath+strRoot.GetLength(),MAX_PATH-strRoot.GetLength(),m_fd.cFileName)!=S_OK)
			return FALSE;
		int len=::GetFileTitle(szPath,title.GetBuffer(MAX_PATH),MAX_PATH);
		
		if (len==0)
			title.FreeExtra();
		else
		{
			title.Empty();
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CFileFind::GetFileTitle(LPSTR szFileTitle,DWORD nMaxLen) const
{
	if (IsUnicodeSystem())
	{
		WCHAR szTitle[MAX_PATH],szPath[MAX_PATH];
		if (StringCbCopyNW(szPath,MAX_PATH*2,strRoot,strRoot.GetLength()*2)!=S_OK)
			return FALSE;
		if (StringCbCopyW(szPath+strRoot.GetLength(),(MAX_PATH-strRoot.GetLength())*2,m_fdw.cFileName)!=S_OK)
			return FALSE;
		short ret=::GetFileTitleW(szPath,szTitle,MAX_PATH);
		if (ret!=0)
			return FALSE;
		
		WideCharToMultiByte(CP_ACP,0,szTitle,-1,szFileTitle,nMaxLen,0,0);
		return TRUE;
	}
	else
	{
		CHAR szPath[MAX_PATH];
		if (WideCharToMultiByte(CP_ACP,0,strRoot,(int)strRoot.GetLength(),szPath,MAX_PATH,0,0)==0)
			return FALSE;

		if (StringCbCopy(szPath+(int)strRoot.GetLength(),MAX_PATH-strRoot.GetLength(),m_fd.cFileName)!=S_OK)
			return FALSE;
		return ::GetFileTitle(szPath,szFileTitle,(WORD)min(nMaxLen,0xFFFF))==0;
	}
}


#endif

#ifndef WIN32

BOOL CFileFind::GetFileTime(CTime& refTime) const
{
	struct tm lt;
	lt.tm_sec=(m_fd.ff_ftime&~0xFFE0)<<1;
	lt.tm_min=(m_fd.ff_ftime&~0xF800)>>5;
	lt.tm_hour=m_fd.ff_ftime>>11;
	lt.tm_mday=m_fd.ff_fdate&~0xFFE0;
	lt.tm_mon=((m_fd.ff_fdate&~0xFE00)>>5)-1;
	lt.tm_year=(m_fd.ff_fdate>>9)+80;
	lt.tm_isdst=0;
	refTime.msec=0;
	refTime.m_time=mktime(&lt);
	return TRUE;
}
#endif


#ifdef DEF_WCHAR
void CFileFind::GetFilePath(CStringW& path) const
{
	path=strRoot;
	if (IsUnicodeSystem())
		path<<m_fdw.cFileName;
	else
		path<<m_fd.cFileName;
}

BOOL CFileFind::FindFile(LPCWSTR pstrName)
{
	strRoot.Copy(pstrName,LastCharIndex(pstrName,L'\\')+1);
	if (m_hFind!=NULL)
	{	
		::FindClose(m_hFind);
		DebugCloseHandle(dhtFileFind,m_hFind,STRNULL);
	}
	
	if (IsUnicodeSystem())
	{
		if (pstrName!=NULL)
			m_hFind=::FindFirstFileW(pstrName,&m_fdw);
		else
			m_hFind=::FindFirstFileW(L"*.*",&m_fdw);
	}
	else
	{
		if (pstrName!=NULL)
			m_hFind=::FindFirstFile(W2A(pstrName),&m_fd);
		else
			m_hFind=::FindFirstFile("*.*",&m_fd);
	}
	DebugOpenHandle(dhtFileFind,m_hFind,STRNULL);
		
	if (m_hFind==INVALID_HANDLE_VALUE)
		return FALSE;
	return TRUE;
}

void CFileFind::GetFilePath(LPWSTR szPath,DWORD nMaxLen) const
{
	StringCbCopyNW(szPath,nMaxLen*sizeof(WCHAR),(LPCWSTR)strRoot,strRoot.GetLength()*sizeof(WCHAR));
	if (IsUnicodeSystem())	
		StringCbCopyW(szPath+(int)strRoot.GetLength(),(nMaxLen-(int)strRoot.GetLength())*sizeof(WCHAR),m_fdw.cFileName);
	else
		MultiByteToWideChar(CP_ACP,0,m_fd.cFileName,-1,szPath+(int)strRoot.GetLength(),nMaxLen-(int)strRoot.GetLength());
}


BOOL CFileFind::GetFileTitle(CStringW& title) const
{

	if (IsUnicodeSystem())
	{
		WCHAR szPath[MAX_PATH];
		if (StringCbCopyNW(szPath,MAX_PATH*2,strRoot,strRoot.GetLength()*2)!=S_OK)
			return FALSE;
		if (StringCbCopyW(szPath+strRoot.GetLength(),(MAX_PATH-strRoot.GetLength())*2,m_fdw.cFileName)!=S_OK)
			return FALSE;
		short ret=::GetFileTitleW(szPath,title.GetBuffer(),MAX_PATH);
		if (ret==0)
			title.FreeExtra();
		else
		{
			title.Empty();
			return FALSE;
		}
	}
	else
	{
		CHAR szPath[MAX_PATH],szTitle[MAX_PATH];
		if (WideCharToMultiByte(CP_ACP,0,strRoot,(int)strRoot.GetLength(),szPath,MAX_PATH,0,0)==0)
			return FALSE;
		if (StringCbCopy(szPath+(int)strRoot.GetLength(),MAX_PATH-(int)strRoot.GetLength(),m_fd.cFileName)!=S_OK)
			return FALSE;
		short ret=::GetFileTitle(szPath,szTitle,MAX_PATH);
		
		if (ret==0)
			title=szTitle;
		else
		{
			title.Empty();
			return FALSE;
		}
	}

	return TRUE;
}


BOOL CFileFind::GetFileTitle(LPWSTR szFileTitle,DWORD nMaxLen) const
{

	if (IsUnicodeSystem())
	{
		WCHAR szPath[MAX_PATH];
		if (StringCbCopyNW(szPath,MAX_PATH*2,strRoot,strRoot.GetLength()*2)!=S_OK)
			return FALSE;
		if (StringCbCopyW(szPath+strRoot.GetLength(),(MAX_PATH-strRoot.GetLength())*2,m_fdw.cFileName)!=S_OK)
			return FALSE;
		return ::GetFileTitleW(szPath,szFileTitle,(WORD)min(nMaxLen,0xFFFF))==0;
	}
	else
	{
		CHAR szPath[MAX_PATH],szTitle[MAX_PATH];
		if (WideCharToMultiByte(CP_ACP,0,strRoot,(int)strRoot.GetLength(),szPath,MAX_PATH,0,0)!=0)
			return FALSE;
		if (StringCbCopy(szPath+(int)strRoot.GetLength(),MAX_PATH-(int)strRoot.GetLength(),m_fd.cFileName)!=S_OK)
			return FALSE;
		short ret=::GetFileTitle(szPath,szTitle,MAX_PATH);
		if (ret!=0)
			return FALSE;
		MultiByteToWideChar(CP_ACP,0,szTitle,-1,szFileTitle,nMaxLen);
		return TRUE;
	}

	
}
#endif

///////////////////////////////////////////
// CSearchFromFile
///////////////////////////////////////////

void CSearchFromFile::OpenFile(LPCSTR szFile)
{
	if (hFile!=NULL)
		this->CloseFile();
#ifdef WIN32
	hFile=CreateFile(szFile,GENERIC_READ,FILE_SHARE_READ,NULL,
		OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (hFile==INVALID_HANDLE_VALUE)
		hFile=NULL;
#else
	hFile=fopen(szFile,"rb");
#endif

	if (hFile==NULL)
		SetHFCError2(HFC_CANNOTOPEN,__LINE__,__FILE__);
	else
	{
		DebugOpenHandle(dhtFile,hFile,szFile);
	}

}

#ifdef DEF_WCHAR
void CSearchFromFile::OpenFile(LPCWSTR szFile)
{
	if (!IsUnicodeSystem())
	{
		OpenFile(W2A(szFile));
		return;
	}

	if (hFile!=NULL)
		this->CloseFile();
	hFile=CreateFileW(szFile,GENERIC_READ,FILE_SHARE_READ,NULL,
		OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (hFile==INVALID_HANDLE_VALUE)
		hFile=NULL;
	else
		DebugOpenHandle(dhtFile,hFile,szFile);




	if (hFile==NULL)
		SetHFCError2(HFC_CANNOTOPEN,__LINE__,__FILE__);

}
#endif

void CSearchFromFile::CloseFile()
{
	if (hFile!=NULL)
	{
#ifdef WIN32
		CloseHandle(hFile);
#else
		fclose(hFile);
#endif
		DebugCloseHandle(dhtFile,hFile,STRNULL);
	}
		
	hFile=NULL;

}



///////////////////////////////////////////
// CSearchHexFromFile
///////////////////////////////////////////

#define SEARCH_BUFFERLEN	10000

CSearchHexFromFile::~CSearchHexFromFile()
{
	CloseFile();
	if (pSearchValue!=NULL)
		delete[] pSearchValue;
}

CSearchFromFile::~CSearchFromFile()
{
	this->CloseFile();
}

ULONG_PTR CSearchHexFromFile::GetFoundPosition() const
{
	return dwFilePtr+dwBufferPtr;
}

void CSearchHexFromFile::CloseFile()
{
	if (hFile!=NULL)
	{
#ifdef WIN32
		CloseHandle(hFile);
#else
		fclose(hFile);
#endif
		DebugCloseHandle(dhtFile,hFile,STRNULL);
	}
	hFile=NULL;

	if (pBuffer!=NULL)
	{
		// Ensures that buffer is clear
		delete[] pBuffer;
		pBuffer=NULL;
	}
}

BOOL CSearchHexFromFile::Search(LPCSTR szFile)
{
    if (hFile==NULL)
	{
		OpenFile(szFile);
				
		
		if (hFile==NULL)
			return FALSE;
	}
	return DoSearching();
}

#ifdef DEF_WCHAR
BOOL CSearchHexFromFile::Search(LPCWSTR szFile)
{
    if (hFile==NULL)
	{
		OpenFile(szFile);
				
		
		if (hFile==NULL)
			return FALSE;
	}
	return DoSearching();
}
#endif

BOOL CSearchHexFromFile::DoSearching()
{
	if (pBuffer==NULL)
	{
#ifdef WIN32
		DWORD dwFileSizeHi;
		dwFileSize=GetFileSize(hFile,&dwFileSizeHi);
#ifdef _WIN64
		dwFileSize|=(SIZE_T)dwFileSizeHi<<32;
#endif
#else
		dwFileSize=filelength(fileno(hFile));
#endif
		
		if (dwFileSize<dwLength)
			return FALSE; // File size if smaller than length of search value

		dwBufferLen=(DWORD)min(SEARCH_BUFFERLEN,dwFileSize);
		pBuffer=new BYTE[max(dwBufferLen,1)+1];
		DWORD dwReaded;
#ifdef WIN32
		BOOL bRet=ReadFile(hFile,pBuffer,dwBufferLen,&dwReaded,NULL);
		if (!bRet || dwReaded<dwBufferLen)
		{
			SetHFCError(HFC_CANNOTREAD);
			return FALSE;
		}
#else
		dwReaded=fread(pBuffer,1,dwBufferLen,hFile);
		if (dwReaded<dwBufferLen)
		{
			SetHFCError(HFC_CANNOTREAD);
			return FALSE;
		}
#endif
		if (!bMatchCase)
		{
#ifdef WIN32
			CharLowerBuff(LPSTR(pBuffer),dwBufferLen);
#else
			pBuffer[dwBufferLen]='\0';
			strlwr(LPSTR(pBuffer));
#endif
		}
		
		dwBufferLen-=(dwLength-1);
#ifdef WIN32
		SetFilePointer(hFile,1-int(dwLength),NULL,FILE_CURRENT);
#else
		fseek(hFile,1-int(dwLength),SEEK_CUR);
#endif
		dwFilePtr=0;
		dwBufferPtr=0;
	}
	else
		dwBufferPtr++;

	while (1)
	{
		for (;dwBufferPtr<dwBufferLen;dwBufferPtr++)
		{
			DWORD i=0;
			for (;i<dwLength;i++)
			{
				if (pBuffer[dwBufferPtr+i]!=pSearchValue[i])
					break;
			}
			if (i==dwLength)
				return TRUE;
		}

		// Marking buffer readed
		dwFilePtr+=dwBufferLen;

		// Not enough of file left
		if (dwFileSize-dwFilePtr<dwLength)
			return FALSE;
		
		// Allocating new buffer if necessary
		if (dwFilePtr+SEARCH_BUFFERLEN>dwFileSize)
		{
			delete[] pBuffer;
			dwBufferLen=DWORD(dwFileSize-dwFilePtr);
			pBuffer=new BYTE[max(1,dwBufferLen)+1];
		}
		else
			dwBufferLen=SEARCH_BUFFERLEN;
		
		DWORD dwReaded;
#ifdef WIN32
		BOOL bRet=ReadFile(hFile,pBuffer,dwBufferLen,&dwReaded,NULL);
		if (!bRet || dwReaded<dwBufferLen)
		{
			SetHFCError(HFC_CANNOTREAD);
			return FALSE;
		}
#else
		dwReaded=fread(pBuffer,1,dwBufferLen,hFile);
		if (dwReaded<dwBufferLen)
		{
			SetHFCError(HFC_CANNOTREAD);
			return FALSE;
		}

#endif
		if (!bMatchCase)
		{
#ifdef WIN32
			CharLowerBuff(LPSTR(pBuffer),dwBufferLen);
#else
            pBuffer[dwBufferLen]='\0';
            strlwr(LPSTR(pBuffer));
#endif
		}
		
		dwBufferLen-=(dwLength-1);
#ifdef WIN32
		SetFilePointer(hFile,1-int(dwLength),NULL,FILE_CURRENT);
#else
		fseek(hFile,1-int(dwLength),SEEK_CUR);
#endif
		dwBufferPtr=0;
	}
	return FALSE;
}




///////////////////////////////////////////
// namespace FileSystem
///////////////////////////////////////////


LONGLONG FileSystem::GetDiskFreeSpace(LPCSTR szDrive)
{
#ifdef WIN32
	ULARGE_INTEGER i64FreeBytesToCaller;
	BOOL (CALLBACK *pGetDiskFreeSpaceEx)(LPCSTR lpDirectoryName,
		PULARGE_INTEGER lpFreeBytesAvailable,PULARGE_INTEGER lpTotalNumberOfBytes,
		PULARGE_INTEGER lpTotalNumberOfFreeBytes);
	*((FARPROC*)&pGetDiskFreeSpaceEx) = GetProcAddress( GetModuleHandle("kernel32.dll"),
                         "GetDiskFreeSpaceExA");
	if (pGetDiskFreeSpaceEx!=NULL)
	{
		ULARGE_INTEGER i64TotalBytes,i64FreeBytes;
		if (!pGetDiskFreeSpaceEx (szDrive,&i64FreeBytesToCaller,&i64TotalBytes,&i64FreeBytes))
			return 0;
	}
	else
	{
		DWORD nSectors,nBytes,nFreeClusters,nTolalClusters;
		if (!::GetDiskFreeSpace(szDrive,&nSectors,
			&nBytes,&nFreeClusters,&nTolalClusters))
			return 0;
		i64FreeBytesToCaller.QuadPart=nBytes*nSectors*nFreeClusters;
	}
	return i64FreeBytesToCaller.QuadPart;
#else
	UCHAR drive[2];
	drive[0]=szDrive[0];
	drive[1]='\0';
	strupr((char*)drive);
	
	struct dfree df;
	getdfree(drive[0]-'A'+1,&df);
	return df.df_bsec*df.df_sclus*df.df_avail;
#endif
}


#ifdef DEF_WCHAR
LONGLONG FileSystem::GetDiskFreeSpace(LPCWSTR szDrive)
{
	if (!IsUnicodeSystem())
		return FileSystem::GetDiskFreeSpace(W2A(szDrive));

	ULARGE_INTEGER i64FreeBytesToCaller;
	BOOL (CALLBACK *pGetDiskFreeSpaceExW)(LPCWSTR lpDirectoryName,
		PULARGE_INTEGER lpFreeBytesAvailable,PULARGE_INTEGER lpTotalNumberOfBytes,
		PULARGE_INTEGER lpTotalNumberOfFreeBytes);
	*((FARPROC*)&pGetDiskFreeSpaceExW) = GetProcAddress( GetModuleHandle("kernel32.dll"),
                         "GetDiskFreeSpaceExW");
	if (pGetDiskFreeSpaceExW!=NULL)
	{
		ULARGE_INTEGER i64TotalBytes,i64FreeBytes;
		if (!pGetDiskFreeSpaceExW(szDrive,&i64FreeBytesToCaller,&i64TotalBytes,&i64FreeBytes))
			return 0;
	}
	else
	{
		DWORD nSectors,nBytes,nFreeClusters,nTolalClusters;
		if (!GetDiskFreeSpaceW(szDrive,&nSectors,
			&nBytes,&nFreeClusters,&nTolalClusters))
			return 0;
		i64FreeBytesToCaller.QuadPart=nBytes*nSectors*nFreeClusters;
	}
	return i64FreeBytesToCaller.QuadPart;
}
#endif


#ifndef WIN32

BYTE FileSystem::CopyFile(LPCTSTR src,LPCTSTR dst)
{
	LPSTR buffer;
	FILE *fp;
	DWORD Size;
	UINT temp,temp2;

	fp=fopen(src,"rb");
	if (fp==NULL)
	{
		SetHFCError(HFC_CANNOTOPEN);
		return FALSE;
	}
	DebugOpenHandle(File,fp,src);
	Size=filelength(fileno(fp));
	buffer=new char[Size+2];
	if (buffer==NULL)
	{
		fclose(fp);
		DebugCloseHandle(dhtFile,fp,src);
		SetHFCError(HFC_CANNOTALLOC);
		return FALSE;
	}
	if (!fread(buffer,1,Size,fp))
	{
		delete[] buffer;
		fclose(fp);
		DebugCloseHandle(dhtFile,fp,src);
		SetHFCError(HFC_CANNOTREAD);
		return FALSE;
	}
	_dos_getftime(fileno(fp),&temp,&temp2);
	fclose(fp);
	DebugCloseHandle(dhtFile,fp,src);

	fp=fopen(src,"wb");
	
	if (fp==NULL)
	{
		delete[] buffer;
		SetHFCError(HFC_CANNOTCREATE);
		return FALSE;
	}
	DebugOpenHandle(dhtFile,fp,src);
	
	if (fwrite(buffer,1,Size,fp))
	{
		delete[] buffer;
		fclose(fp);
		DebugCloseHandle(dhtFile,fp,src);
		SetHFCError(HFC_CANNOTWRITE);
		return FALSE;
	}
	_dos_setftime(fileno(fp),temp,temp2);
	fclose(fp);
	DebugCloseHandle(dhtFile,fp,src);
	_dos_getfileattr(src,&temp);
	_dos_setfileattr(dst,temp);
	return TRUE;
}

#endif



BOOL FileSystem::IsFile(LPCSTR szFileName)
{
	if (szFileName[0]=='\0')
		return FALSE;
#ifdef WIN32
	HANDLE hFind;
	WIN32_FIND_DATA fd;
	int ret=TRUE;
	hFind=FindFirstFile(szFileName,&fd);
	DebugOpenHandle(dhtFileFind,hFind,STRNULL);
	if (hFind!=INVALID_HANDLE_VALUE)
	{
		while (fd.dwFileAttributes==FILE_ATTRIBUTE_DIRECTORY && ret)
			ret=FindNextFile(hFind,&fd);
		FindClose(hFind);	
		DebugCloseHandle(dhtFileFind,hFind,STRNULL);
		return ret;
	}
	return FALSE;
#else
	struct ffblk fd;
	return !findfirst(szFileName,&fd,FA_RDONLY|FA_HIDDEN|FA_SYSTEM|FA_ARCH);
#endif
}

INT FileSystem::IsDirectory(LPCSTR szDirectoryName)
{
#ifdef WIN32
	HANDLE hFind;
	WIN32_FIND_DATA fd;
	int ret=TRUE;
	if (szDirectoryName[0]=='\0')
		return 0;
	if (szDirectoryName[1]=='\0')
		return 0;
	if (szDirectoryName[2]=='\0')
		return 0;
	if (szDirectoryName[1]==':' && szDirectoryName[2]=='\\' && szDirectoryName[3]=='\0')
	{
		switch (GetDriveType(szDirectoryName))
		{
		case DRIVE_UNKNOWN:
		case DRIVE_NO_ROOT_DIR:
			return 0;
		case DRIVE_FIXED:
			return 1;
		default:
			return 2;
		}
	}
	
	// Taking last '\\' 
	LPSTR szPath;
	int dwPathLen=istrlen(szDirectoryName);
	if (szDirectoryName[dwPathLen-1]=='\\' && dwPathLen>3)
	{
		szPath=new char[dwPathLen+5];
		MemCopy(szPath,szDirectoryName,--dwPathLen);
		szPath[dwPathLen]='\0';
	}
	else
		szPath=LPSTR(szDirectoryName);

	hFind=FindFirstFile(szPath,&fd);
	DebugOpenHandle(dhtFileFind,hFind,szPath);
	if (hFind!=INVALID_HANDLE_VALUE)
	{
		while (!(fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) && ret)
			ret=FindNextFile(hFind,&fd);
	
		if (szPath!=szDirectoryName)
			delete[] szPath;

		FindClose(hFind);	
		DebugCloseHandle(dhtFileFind,hFind,szPath);
		if (ret)
		{
			if (szDirectoryName[0]=='\\')
			{
				if (szDirectoryName[1]=='\\')
					return 2;
				switch (::GetDriveType(NULL))
				{
				case DRIVE_UNKNOWN:
				case DRIVE_NO_ROOT_DIR:
					return 0;
				case DRIVE_FIXED:
					return 1;
				default:
					return 2;
				}
 
			}
			if (szDirectoryName[1]==':' && szDirectoryName[2]=='\\')
			{
				char szTemp[4]="X:\\";
				szTemp[0]=szDirectoryName[0];

				switch (GetDriveType(szTemp))
				{
				case DRIVE_UNKNOWN:
					return 0;
				case DRIVE_FIXED:
					return 1;
				default:
					return 2;
				}
			}
		}
		return 0;
	}
	else if (szDirectoryName[0]=='\\' && szDirectoryName[1]=='\\')
	{
		// UNC share name
		if (szPath==szDirectoryName)
		{
			szPath=new char[dwPathLen+5];
			sMemCopy(szPath,szDirectoryName,dwPathLen);
		}		
		dMemCopy(szPath+dwPathLen,"\\*.*",5);

		hFind=FindFirstFile(szPath,&fd);
		DebugOpenHandle(dhtFileFind,hFind,szPath);
		delete[] szPath;
		if (hFind==INVALID_HANDLE_VALUE)
			return 0;
		// Is this really needed, e.g. \\pc\c$ does not have '.' in directory list
		//ret=1;
		//while ((fd.cFileName[0]!='.' || fd.cFileName[1]!='\0') && ret==1)
		//	ret=FindNextFile(hFind,&fd);
		FindClose(hFind);
		DebugCloseHandle(dhtFileFind,hFind,szPath);
		//return ret?2:0;
		return 2;
	}
	
	
	if (szPath!=szDirectoryName)
		delete[] szPath;
	return 0;
#else
	struct ffblk fd;
	return !findfirst(szDirectoryName,&fd,FA_DIREC);
#endif
}

#ifdef DEF_WCHAR
BOOL FileSystem::IsFile(LPCWSTR szFileName)
{
	if (!IsUnicodeSystem())
		return FileSystem::IsFile(W2A(szFileName));

	if (szFileName[0]==L'\0')
		return FALSE;
#ifdef WIN32
	HANDLE hFind;
	WIN32_FIND_DATAW fd;
	int ret=TRUE;
	hFind=FindFirstFileW(szFileName,&fd);
	DebugOpenHandle(dhtFileFind,hFind,szFileName);
	if (hFind!=INVALID_HANDLE_VALUE)
	{
		while (fd.dwFileAttributes==FILE_ATTRIBUTE_DIRECTORY && ret)
			ret=FindNextFileW(hFind,&fd);
		FindClose(hFind);	
		DebugCloseHandle(dhtFileFind,hFind,szFileName);
		DebugFormatMessage("FileSystem::IsFile: ret for %S is %d",szFileName,ret);
		return ret;
	}
	DebugFormatMessage("FileSystem::IsFile: no file %S",szFileName);
	return FALSE;
#else
	struct ffblk fd;
	return !findfirst(szFileName,&fd,FA_RDONLY|FA_HIDDEN|FA_SYSTEM|FA_ARCH);
#endif
}

INT FileSystem::IsDirectory(LPCWSTR szDirectoryName)
{
	if (!IsUnicodeSystem())
		return FileSystem::IsDirectory(W2A(szDirectoryName));


	HANDLE hFind;
	WIN32_FIND_DATAW fd;
	int ret=TRUE;
	if (szDirectoryName[0]==L'\0')
		return 0;
	if (szDirectoryName[1]==L'\0')
		return 0;
	if (szDirectoryName[2]==L'\0')
		return 0;
	if (szDirectoryName[1]==L':' && szDirectoryName[2]==L'\\' && szDirectoryName[3]==L'\0')
	{
		switch (GetDriveTypeW(szDirectoryName))
		{
		case DRIVE_UNKNOWN:
		case DRIVE_NO_ROOT_DIR:
			return 0;
		case DRIVE_FIXED:
			return 1;
		default:
			return 2;
		}
	}
	
	// Taking last '\\' 
	LPWSTR szPath;
	int dwPathLen=istrlenw(szDirectoryName);
	if (szDirectoryName[dwPathLen-1]==L'\\' && dwPathLen>3)
	{
		szPath=new WCHAR[dwPathLen+5];
		MemCopyW(szPath,szDirectoryName,--dwPathLen);
		szPath[dwPathLen]=L'\0';
	}
	else
		szPath=LPWSTR(szDirectoryName);

	hFind=FindFirstFileW(szPath,&fd);
	if (hFind!=INVALID_HANDLE_VALUE)
	{
		DebugOpenHandle(dhtFileFind,hFind,szPath);

		while (!(fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) && ret)
			ret=FindNextFileW(hFind,&fd);
	
		if (szPath!=szDirectoryName)
			delete[] szPath;

		FindClose(hFind);	
		DebugCloseHandle(dhtFileFind,hFind,szPath);
		if (ret)
		{
			if (szDirectoryName[0]==L'\\')
			{
				if (szDirectoryName[1]==L'\\')
					return 2;
				switch (::GetDriveType(NULL))
				{
				case DRIVE_UNKNOWN:
				case DRIVE_NO_ROOT_DIR:
					return 0;
				case DRIVE_FIXED:
					return 1;
				default:
					return 2;
				}
 
			}
			if (szDirectoryName[1]==':' && szDirectoryName[2]==L'\\')
			{
				WCHAR szTemp[4]=L"X:\\";
				szTemp[0]=szDirectoryName[0];

				switch (GetDriveTypeW(szTemp))
				{
				case DRIVE_UNKNOWN:
					return 0;
				case DRIVE_FIXED:
					return 1;
				default:
					return 2;
				}
			}
		}
		return 0;
	}
	else if (szDirectoryName[0]==L'\\' && szDirectoryName[1]==L'\\')
	{
		// UNC share name
		if (szPath==szDirectoryName)
		{
			szPath=new WCHAR[dwPathLen+5];
			MemCopyW(szPath,szDirectoryName,dwPathLen);
		}		
		MemCopyW(szPath+dwPathLen,L"\\*.*",5);

		hFind=FindFirstFileW(szPath,&fd);
		DebugOpenHandle(dhtFileFind,hFind,szPath);
		delete[] szPath;
		if (hFind==INVALID_HANDLE_VALUE)
			return 0;
		// Is this really needed, e.g. \\pc\c$ does not have '.' in directory list
		//ret=1;
		//while ((fd.cFileName[0]!='.' || fd.cFileName[1]!='\0') && ret==1)
		//	ret=FindNextFile(hFind,&fd);
		FindClose(hFind);
		DebugCloseHandle(dhtFileFind,hFind,STRNULL);
		//return ret?2:0;
		return 2;
	}
	
	
	if (szPath!=szDirectoryName)
		delete[] szPath;
	return 0;
}
#endif

BOOL FileSystem::IsValidFileName(LPCSTR szFile,LPSTR szShortName)
{
#ifdef WIN32
	if (szFile[0]=='\0')
		return FALSE;
	if (FileSystem::IsFile(szFile))
	{
		if (szShortName!=NULL)
			GetShortPathName(szFile,szShortName,_MAX_PATH);
		return TRUE;
	}
	HANDLE hFile=CreateFile(szFile,GENERIC_WRITE,
		FILE_SHARE_READ,NULL,CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,NULL);
	if (hFile==INVALID_HANDLE_VALUE)
		return FALSE;
	DebugOpenHandle(dhtFile,hFile,szFile);
	CloseHandle(hFile);
	DebugCloseHandle(dhtFile,hFile,szFile);
	if (szShortName!=NULL)
		GetShortPathName(szFile,szShortName,_MAX_PATH);
	DeleteFile(szFile);
	return TRUE;
#else
	if (szFile[0]=='\0')
		return FALSE;
	if (CFile::IsFile(szFile))
		return TRUE;
	FILE* fp=fopen(szFile,"wb");
	if (fp==NULL)
		return FALSE;
	fclose(fp);
	remove(szFile);
	return TRUE;
#endif
}

#ifdef DEF_WCHAR
BOOL FileSystem::IsValidFileName(LPCWSTR szFile,LPWSTR szShortName)
{
	if (szFile[0]==L'\0')
		return FALSE;
	if (FileSystem::IsFile(szFile))
	{
		if (szShortName!=NULL)
		{
			if (IsUnicodeSystem())
				::GetShortPathNameW(szFile,szShortName,_MAX_PATH);
			else
			{
				char szShortPathA[MAX_PATH];
				::GetShortPathName(W2A(szFile),szShortPathA,_MAX_PATH);
				MultiByteToWideChar(CP_ACP,0,szShortPathA,-1,szShortName,MAX_PATH);
			}
		}

		return TRUE;
	}
	HANDLE hFile;
	if (IsUnicodeSystem())
		hFile=CreateFileW(szFile,GENERIC_WRITE,
			FILE_SHARE_READ,NULL,CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,NULL);
	else
		hFile=CreateFileA(W2A(szFile),GENERIC_WRITE,
			FILE_SHARE_READ,NULL,CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,NULL);

	if (hFile==INVALID_HANDLE_VALUE)
		return FALSE;
	DebugOpenHandle(dhtFile,hFile,szFile);
	CloseHandle(hFile);
	DebugCloseHandle(dhtFile,hFile,szFile);
	if (szShortName!=NULL)
	{	
		if (IsUnicodeSystem())
			::GetShortPathNameW(szFile,szShortName,_MAX_PATH);
		else
		{
			char szShortPathA[MAX_PATH];
			::GetShortPathName(W2A(szFile),szShortPathA,_MAX_PATH);
			MultiByteToWideChar(CP_ACP,0,szShortPathA,-1,szShortName,MAX_PATH);
		}
	}	
	FileSystem::Remove(szFile);
	return TRUE;
}
#endif

// Last '//' is not counted, if exists
DWORD FileSystem::ParseExistingPath(LPCSTR szPath)
{
	DWORD dwLength=(DWORD)strlen(szPath);

	if (dwLength<2)
		return 0;
	if (dwLength==2 || dwLength==3)
	{
		if (szPath[1]==':')
		{
			char szTemp[]="X:\\";
			szTemp[0]=szPath[0];
			return GetDriveType(szTemp)==DRIVE_NO_ROOT_DIR?0:2;
		}
		return 0;
	}
	
	char* pTempPath=new char[dwLength+2];
	CopyMemory(pTempPath,szPath,dwLength+1);
	if (pTempPath[dwLength-1]!='\\')
	{
		pTempPath[dwLength]='\\';
		pTempPath[++dwLength]='\0';
	}
	
	while (!IsDirectory(pTempPath))
	{
		dwLength--;
		while (dwLength>0 && pTempPath[dwLength]=='\\')
			dwLength--;

		// Findling next '\\'
		while (dwLength>0 && pTempPath[dwLength]!='\\')
			dwLength--;
        if (dwLength==0)
		{
			delete[] pTempPath;
			return 0;
		}

		pTempPath[dwLength+1]='\0';
	}

	while (dwLength>0 && pTempPath[dwLength-1]=='\\')
		dwLength--;
	delete[] pTempPath;
	return dwLength;
}

BOOL FileSystem::IsValidPath(LPCSTR szPath,BOOL bAsDirectory)
{
	BOOL bRet=FALSE;

	int nExisting=ParseExistingPath(szPath);
	if (nExisting==0)
		return FALSE;

	if (szPath[nExisting]!='\\')
	{
		if (szPath[nExisting]!='\0')
			return FALSE; // Should not be possible
		return TRUE;
	}

	int nStart=nExisting,nFirstNonExisting=-1;
	for(;;)
	{
		// Next '\\' or '\0'
        int i;
		for (i=nStart+1;szPath[i]!='\0' && szPath[i]!='\\';i++);
			
		if (szPath[i]=='\0')
		{
			// Is possible to create file?
			if (bAsDirectory)
			{
				if (CreateDirectory(szPath,NULL))
				{
					bRet=TRUE;
					RemoveDirectory(szPath);
				}
			}
			else	
			{
				HANDLE hFile=CreateFile(szPath,GENERIC_WRITE,
					FILE_SHARE_READ,NULL,CREATE_ALWAYS,
					FILE_ATTRIBUTE_NORMAL,NULL);
				DebugOpenHandle(dhtFile,hFile,szPath);
				if (hFile!=INVALID_HANDLE_VALUE)
				{
					CloseHandle(hFile);
					DebugCloseHandle(dhtFile,hFile,szPath);
					DeleteFile(szPath);

					bRet=TRUE;
				}
			}
			break;
		}

		char* pTemp=new char[i+2];
		CopyMemory(pTemp,szPath,i);
		pTemp[i]='\0';

		if (!CreateDirectory(pTemp,NULL))
		{
			delete[] pTemp;
			break;
		}

		delete[] pTemp;
		nStart=i;

	}
	
	// Removing created directories
	while (nStart>nExisting)
	{
		for (;szPath[nStart]!='\\' && nExisting<nStart;nStart--);

		if (nStart<=nExisting)
			break;

		char* pTemp=new char[nStart+2];
		CopyMemory(pTemp,szPath,nStart);
		pTemp[nStart]='\0';
		RemoveDirectory(pTemp);
		delete[] pTemp;
        
		nStart--;
	}

	return bRet;
}

#ifdef DEF_WCHAR
// Last '//' is not counted, if exists
DWORD FileSystem::ParseExistingPath(LPCWSTR szPath)
{
	DWORD dwLength=(DWORD)wcslen(szPath);

	if (dwLength<2)
		return 0;
	if (dwLength==2 || dwLength==3)
	{
		if (szPath[1]==':')
		{
			if (IsUnicodeSystem())
			{
				WCHAR szTemp[]=L"X:\\";
				szTemp[0]=szPath[0];
				return GetDriveTypeW(szTemp)==DRIVE_NO_ROOT_DIR?0:2;
			}
			else
			{
				CHAR szTemp[]="X:\\";
				MemCopyWtoA(szTemp,sizeof(szTemp),szPath,1);
				return GetDriveTypeA(szTemp)==DRIVE_NO_ROOT_DIR?0:2;
			}
		}
		return 0;
	}
	
	WCHAR* pTempPath=new WCHAR[dwLength+2];
	MemCopyW(pTempPath,szPath,dwLength+1);
	if (pTempPath[dwLength-1]!='\\')
	{
		pTempPath[dwLength]='\\';
		pTempPath[++dwLength]='\0';
	}
	
	while (!IsDirectory(pTempPath))
	{
		dwLength--;
		while (dwLength>0 && pTempPath[dwLength]=='\\')
			dwLength--;

		// Findling next '\\'
		while (dwLength>0 && pTempPath[dwLength]!='\\')
			dwLength--;
        if (dwLength==0)
		{
			delete[] pTempPath;
			return 0;
		}

		pTempPath[dwLength+1]='\0';
	}

	while (dwLength>0 && pTempPath[dwLength-1]=='\\')
		dwLength--;
	delete[] pTempPath;
	return dwLength;
}

BOOL FileSystem::IsValidPath(LPCWSTR szPath,BOOL bAsDirectory)
{
	if (!IsUnicodeSystem())
		return FileSystem::IsValidPath(W2A(szPath),bAsDirectory);

	BOOL bRet=FALSE;

	int nExisting=ParseExistingPath(szPath);
	if (nExisting==0)
		return FALSE;

	if (szPath[nExisting]!='\\')
	{
		if (szPath[nExisting]!='\0')
			return FALSE; // Should not be possible
		return TRUE;
	}

	int nStart=nExisting,nFirstNonExisting=-1;
	for(;;)
	{
		// Next '\\' or '\0'
        int i;
		for (i=nStart+1;szPath[i]!='\0' && szPath[i]!='\\';i++);
			
		if (szPath[i]=='\0')
		{
			// Is possible to create file?
			if (bAsDirectory)
			{
				if (CreateDirectoryW(szPath,NULL))
				{
					bRet=TRUE;
					RemoveDirectoryW(szPath);
				}
			}
			else	
			{
				HANDLE hFile=CreateFileW(szPath,GENERIC_WRITE,
					FILE_SHARE_READ,NULL,CREATE_ALWAYS,
					FILE_ATTRIBUTE_NORMAL,NULL);
				if (hFile!=INVALID_HANDLE_VALUE)
				{
					DebugOpenHandle(dhtFile,hFile,szPath);
					CloseHandle(hFile);
					DebugCloseHandle(dhtFile,hFile,szPath);
					DeleteFileW(szPath);

					bRet=TRUE;
				}
			}
			break;
		}

		WCHAR* pTemp=new WCHAR[i+2];
		MemCopyW(pTemp,szPath,i);
		pTemp[i]=L'\0';

		if (!CreateDirectoryW(pTemp,NULL))
		{
			delete[] pTemp;
			break;
		}

		delete[] pTemp;
		nStart=i;

	}
	
	// Removing created directories
	while (nStart>nExisting)
	{
		for (;szPath[nStart]!=L'\\' && nExisting<nStart;nStart--);

		if (nStart<=nExisting)
			break;

		WCHAR* pTemp=new WCHAR[nStart+2];
		MemCopyW(pTemp,szPath,nStart);
		pTemp[nStart]=L'\0';
		RemoveDirectoryW(pTemp);
		delete[] pTemp;
        
		nStart--;
	}

	return bRet;
}
#endif
BOOL FileSystem::CreateDirectoryRecursive(LPCSTR szPath,LPSECURITY_ATTRIBUTES plSecurityAttributes)
{
	BOOL bRet=FALSE;

	int nExisting=ParseExistingPath(szPath);
	if (nExisting==0)
		return FALSE;

	if (szPath[nExisting]!='\\')
	{
		if (szPath[nExisting]!='\0')
			return FALSE; // Should not be possible
		return TRUE;
	}



	int nStart=nExisting,nFirstNonExisting=-1;
	for(;;)
	{
		// Next '\\' or '\0'
		int i;
        for (i=nStart+1;szPath[i]!='\0' && szPath[i]!='\\';i++);
					
		if (szPath[i]=='\0')
			return CreateDirectory(szPath,NULL);

		char* pTemp=new char[i+2];
		CopyMemory(pTemp,szPath,i);
		pTemp[i]='\0';

		if (!CreateDirectory(pTemp,NULL))
		{
			delete[] pTemp;
			return FALSE;
		}

		delete[] pTemp;
		nStart=i;

	}
	return TRUE;
}

BOOL FileSystem::IsSamePath(LPCSTR szDir1,LPCSTR szDir2)
{
#ifdef WIN32
	char path1[MAX_PATH],path2[MAX_PATH];
	int nRet1,nRet2;
	nRet1=GetShortPathName(szDir1,path1,MAX_PATH);
	if (!nRet1)
		return strcasecmp(szDir1,szDir2)==0;
	nRet2=GetShortPathName(szDir2,path2,MAX_PATH);
	if (!nRet2 || nRet1<nRet2-1 || nRet1>nRet2+1)
		return FALSE;
	if (path1[nRet1-1]=='\\')
		path1[nRet1-1]='\0';
	if (path2[nRet2-1]=='\\')
		path2[nRet2-1]='\0';
	return strcasecmp(path1,path2)==0;	
#else
	char path1[MAX_PATH],path2[MAX_PATH];
	int nRet1,nRet2;
	nRet1=istrlen(szDir1);
	nRet2=istrlen(szDir2);

	MemCopy(path1,szDir1,nRet1);
	MemCopy(path2,szDir2,nRet2);
	if (path1[nRet1-1]=='\\')
		path1[nRet1-1]='\0';
	if (path2[nRet2-1]=='\\')
		path2[nRet2-1]='\0';
	return strcasecmp(path1,path2)==0;	
#endif
}


// Checking whether szSubDir is sub directory of the szPath
BOOL FileSystem::IsSubDirectory(LPCSTR szSubDir,LPCSTR szPath)
{
	char sSubDir[MAX_PATH],sPath[MAX_PATH];
#ifdef WIN32
	if (!GetShortPathName(szSubDir,sSubDir,MAX_PATH))
		StringCbCopy(sSubDir,MAX_PATH,szSubDir);
	if (!GetShortPathName(szPath,sPath,MAX_PATH))
		StringCbCopy(sPath,MAX_PATH,szPath);
#else
	{
		char* p1;
		for (p1=szSubDir,int i=0;*p1!=0;p1++,i++)
		{
			if (*p1=='\\')
			{
				while (*p1=='\\')
					p1++;
				sSubDir[i]=='\';
			}
			else
				sSubDir[i]=*p1;
		}
		for (p1=szPath,int i=0;*p1!=0;p1++,i++)
		{
			if (*p1=='\\')
			{
				while (*p1=='\\')
					p1++;
				sPath[i]=='\';
			}
			else
				sPath[i]=*p1;
		}
	}
#endif
    int nSlashes=0,i;
	// Counting '\\' characters in szPath
	for (i=0;sPath[i]!='\0';i++)
	{
		if (sPath[i]=='\\')
			nSlashes++;
	}

	// Break szSubDir after nSlashes pcs of '\\'
	for (i=0;sSubDir[i]!='\0';i++)
	{
		if (sSubDir[i]=='\\')
		{
			nSlashes--;
			if (nSlashes<0)
				break;
		}
	}

	if (nSlashes>=0) // Not enough '\\' directories, cannot be subdir
		return FALSE;
	sSubDir[i]='\0';

	return strcasecmp(sSubDir,sPath)==0;
}

#ifdef DEF_WCHAR
BOOL FileSystem::CreateDirectoryRecursive(LPCWSTR szPath,LPSECURITY_ATTRIBUTES plSecurityAttributes)
{
	if (!IsUnicodeSystem())
		return CreateDirectoryRecursive(W2A(szPath),plSecurityAttributes);

	BOOL bRet=FALSE;

	int nExisting=ParseExistingPath(szPath);
	if (nExisting==0)
		return FALSE;

	if (szPath[nExisting]!=L'\\')
	{
		if (szPath[nExisting]!=L'\0')
			return FALSE; // Should not be possible
		return TRUE;
	}



	int nStart=nExisting,nFirstNonExisting=-1;
	for(;;)
	{
		// Next '\\' or '\0'
		int i;
        for (i=nStart+1;szPath[i]!='\0' && szPath[i]!='\\';i++);
					
		if (szPath[i]=='\0')
			return CreateDirectoryW(szPath,NULL);

		WCHAR* pTemp=new WCHAR[i+2];
		MemCopyW(pTemp,szPath,i);
		pTemp[i]='\0';

		if (!CreateDirectory(pTemp,NULL))
		{
			delete[] pTemp;
			return FALSE;
		}

		delete[] pTemp;
		nStart=i;

	}
	return TRUE;
}


BOOL FileSystem::IsSamePath(LPCWSTR szDir1,LPCWSTR szDir2)
{
	if (!IsUnicodeSystem())
		return FileSystem::IsSamePath(W2A(szDir1),W2A(szDir2));

	WCHAR path1[MAX_PATH],path2[MAX_PATH];
	int nRet1,nRet2;
	nRet1=GetShortPathNameW(szDir1,path1,MAX_PATH);
	if (!nRet1)
		return strcasecmp(szDir1,szDir2)==0;
	nRet2=GetShortPathNameW(szDir2,path2,MAX_PATH);
	if (!nRet2 || nRet1<nRet2-1 || nRet1>nRet2+1)
		return FALSE;
	if (path1[nRet1-1]==L'\\')
		path1[nRet1-1]=L'\0';
	if (path2[nRet2-1]==L'\\')
		path2[nRet2-1]=L'\0';
	return strcasecmp(path1,path2)==0;	
}
// Checking whether szSubDir is sub directory of the szPath
BOOL FileSystem::IsSubDirectory(LPCWSTR szSubDir,LPCWSTR szPath)
{
	if (!IsUnicodeSystem())
		return FileSystem::IsSubDirectory(W2A(szSubDir),W2A(szPath));

	WCHAR sSubDir[MAX_PATH],sPath[MAX_PATH];
	if (!GetShortPathNameW(szSubDir,sSubDir,MAX_PATH))
	{
		if (StringCbCopyW(sSubDir,MAX_PATH*2,szSubDir)!=S_OK)
			return FALSE;
	}
	if (!GetShortPathNameW(szPath,sPath,MAX_PATH))
	{
		if (StringCbCopyW(sPath,MAX_PATH*2,szPath)!=S_OK)
			return FALSE;
	}
    int nSlashes=0,i;
	// Counting '\\' characters in szPath
	for (i=0;sPath[i]!=L'\0';i++)
	{
		if (sPath[i]==L'\\')
			nSlashes++;
	}

	// Break szSubDir after nSlashes pcs of '\\'
	for (i=0;sSubDir[i]!=L'\0';i++)
	{
		if (sSubDir[i]==L'\\')
		{
			nSlashes--;
			if (nSlashes<0)
				break;
		}
	}

	if (nSlashes>=0) // Not enough '\\' directories, cannot be subdir
		return FALSE;
	sSubDir[i]='\0';

	return strcasecmp(sSubDir,sPath)==0;
}

DWORD FileSystem::GetFullPathName(LPCWSTR lpFileName,DWORD nBufferLength,LPWSTR lpBuffer,LPWSTR* lpFilePart)
{
	if (IsUnicodeSystem())
		return ::GetFullPathNameW(lpFileName,nBufferLength,lpBuffer,lpFilePart);
	else
	{
		CHAR sPathA[MAX_PATH+10];
		CHAR* szTemp;
		DWORD dwRet=::GetFullPathName(W2A(lpFileName),MAX_PATH+10,sPathA,&szTemp);
		if (dwRet==0)
			return 0;
		MultiByteToWideChar(CP_ACP,0,sPathA,dwRet+1,lpBuffer,nBufferLength);
		if (lpFilePart!=NULL)
			*lpFilePart=lpBuffer+DWORD(sPathA-szTemp);
		return dwRet;
	}
}

DWORD FileSystem::GetShortPathName(LPCWSTR lpszLongPath,LPWSTR lpszShortPath,DWORD cchBuffer)
{
	if (IsUnicodeSystem())
		return ::GetShortPathNameW(lpszLongPath,lpszShortPath,cchBuffer);
	else
	{
		CHAR sPathA[MAX_PATH+10];
		DWORD dwRet=::GetShortPathName(W2A(lpszLongPath),sPathA,MAX_PATH+10);
		if (dwRet==0)
			return 0;
		MultiByteToWideChar(CP_ACP,0,sPathA,dwRet+1,lpszShortPath,cchBuffer);
		return dwRet;
	}
}

DWORD FileSystem::GetCurrentDirectory(DWORD nBufferLength,LPWSTR lpBuffer)
{
	if (IsUnicodeSystem())
		return ::GetCurrentDirectoryW(nBufferLength,lpBuffer);
	else
	{
		CHAR sPathA[MAX_PATH+10];
		DWORD dwRet=::GetCurrentDirectoryA(MAX_PATH+10,sPathA);
		if (dwRet==0)
			return 0;
		MultiByteToWideChar(CP_ACP,0,sPathA,dwRet+1,lpBuffer,nBufferLength);
		return dwRet;
	}
}

DWORD FileSystem::GetLongPathName(LPCWSTR lpszShortPath,LPWSTR lpszLongPath,DWORD cchBuffer)
{
	if (IsUnicodeSystem())
		return ::GetLongPathNameW(lpszShortPath,lpszLongPath,cchBuffer);
	else
	{
		CHAR sPathA[MAX_PATH+10];
		DWORD dwRet=::GetLongPathNameA(W2A(lpszShortPath),sPathA,MAX_PATH+10);
		if (dwRet==0)
			return 0;
		MultiByteToWideChar(CP_ACP,0,sPathA,dwRet+1,lpszLongPath,cchBuffer);
		return dwRet;
	}
}

DWORD FileSystem::GetTempPath(DWORD nBufferLength,LPWSTR lpBuffer)
{
	if (IsUnicodeSystem())
		return ::GetTempPathW(nBufferLength,lpBuffer);
	else
	{
		CHAR sPathA[MAX_PATH+10];
		DWORD dwRet=::GetTempPathA(MAX_PATH+10,sPathA);
		if (dwRet==0)
			return 0;
		MultiByteToWideChar(CP_ACP,0,sPathA,dwRet+1,lpBuffer,nBufferLength);
		return dwRet;
	}
}

UINT FileSystem::GetTempFileName(LPCWSTR lpPathName,LPCWSTR lpPrefixString,UINT uUnique,LPWSTR lpTempFileName)
{
	if (IsUnicodeSystem())
		return ::GetTempFileNameW(lpPathName,lpPrefixString,uUnique,lpTempFileName);
	else
	{
		CHAR sPathA[MAX_PATH+10];
		UINT dwRet=::GetTempFileName(W2A(lpPathName),W2A(lpPrefixString),uUnique,sPathA);
		if (dwRet==0)
			return 0;
		MultiByteToWideChar(CP_ACP,0,sPathA,dwRet+1,lpTempFileName,MAX_PATH);
		return dwRet;
	}
}

UINT FileSystem::GetWindowsDirectory(LPWSTR lpBuffer,UINT uSize)
{
	if (IsUnicodeSystem())
		return ::GetWindowsDirectoryW(lpBuffer,uSize);
	else
	{
		LPSTR lpBufferA=new char[uSize];
		UINT uRet=::GetWindowsDirectoryA(lpBufferA,uSize);
		if (uRet==0)
			return 0;
		MultiByteToWideChar(CP_ACP,0,lpBufferA,uRet+1,lpBuffer,uSize);
		return uRet;
	}
}

UINT FileSystem::GetSystemDirectory(LPWSTR lpBuffer,UINT uSize)
{
	if (IsUnicodeSystem())
		return ::GetSystemDirectoryW(lpBuffer,uSize);
	else
	{
		LPSTR lpBufferA=new char[uSize];
		UINT uRet=::GetSystemDirectoryA(lpBufferA,uSize);
		if (uRet==0)
			return 0;
		MultiByteToWideChar(CP_ACP,0,lpBufferA,uRet+1,lpBuffer,uSize);
		delete[] lpBufferA;
		return uRet;
	}
}

DWORD FileSystem::GetLogicalDriveStrings(DWORD nBufferLength,LPWSTR lpBuffer)
{
	if (IsUnicodeSystem())
		return ::GetLogicalDriveStringsW(nBufferLength,lpBuffer);
	else
	{
		DWORD nLen=::GetLogicalDriveStringsA(0,NULL)+1;
		if (nLen==0)
			return 0;
		CHAR* pBuffer=new CHAR[nLen+1];
		nLen=::GetLogicalDriveStringsA(nLen,pBuffer);
		MultiByteToWideChar(CP_ACP,0,pBuffer,nLen+1,lpBuffer,nBufferLength);
		delete[] pBuffer;
		return nLen;
	}
}

BOOL FileSystem::GetVolumeInformation(LPCWSTR lpRootPathName,LPWSTR lpVolumeNameBuffer,
		DWORD nVolumeNameSize,LPDWORD lpVolumeSerialNumber,LPDWORD lpMaximumComponentLength,
		LPDWORD lpFileSystemFlags,LPWSTR lpFileSystemNameBuffer,DWORD nFileSystemNameSize)
{
	if (IsUnicodeSystem())
		return ::GetVolumeInformationW(lpRootPathName,lpVolumeNameBuffer,
			nVolumeNameSize,lpVolumeSerialNumber,lpMaximumComponentLength,
			lpFileSystemFlags,lpFileSystemNameBuffer,nFileSystemNameSize);
	else
	{
		char* lpVolumeNameBufferA=NULL,*lpFileSystemNameBufferA=NULL;
		if (lpVolumeNameBuffer!=NULL)
			lpVolumeNameBufferA=new char[max(nVolumeNameSize,2)];
		if (lpFileSystemNameBuffer!=NULL)
			lpFileSystemNameBufferA=new char[max(nFileSystemNameSize,2)];
		BOOL bRet=::GetVolumeInformationA(W2A(lpRootPathName),lpVolumeNameBufferA,
			nVolumeNameSize,lpVolumeSerialNumber,lpMaximumComponentLength,
			lpFileSystemFlags,lpFileSystemNameBufferA,nFileSystemNameSize);
		
		if (lpVolumeNameBufferA!=NULL)
		{
			if (bRet)			
				MultiByteToWideChar(CP_ACP,0,lpVolumeNameBufferA,-1,lpVolumeNameBuffer,nVolumeNameSize);
			delete[] lpVolumeNameBufferA;
		}
		if (lpFileSystemNameBufferA!=NULL)
		{
			if (bRet)			
				MultiByteToWideChar(CP_ACP,0,lpFileSystemNameBufferA,-1,lpFileSystemNameBuffer,nFileSystemNameSize);
			delete[] lpFileSystemNameBufferA;
		}
		return bRet;
	}
}

short FileSystem::GetFileTitle(LPCWSTR lpszFile,LPWSTR lpszTitle,WORD cbBuf)
{
	if (IsUnicodeSystem())
		return ::GetFileTitleW(lpszFile,lpszTitle,cbBuf);
	else
	{
		char* pBuf=new char[cbBuf+2];
		short sRet=::GetFileTitle(W2A(lpszFile),pBuf,cbBuf);
		if (sRet==0)
			MultiByteToWideChar(CP_ACP,0,pBuf,-1,lpszTitle,cbBuf);
		delete[] pBuf;
		return sRet;
	}
}

BOOL FileSystem::LookupAccountName(LPCWSTR lpSystemName,LPCWSTR lpAccountName,PSID Sid,
	LPDWORD cbSid,LPWSTR ReferencedDomainName,LPDWORD cchReferencedDomainName,
	PSID_NAME_USE peUse)
{
	if (IsUnicodeSystem())
		return ::LookupAccountNameW(lpSystemName,lpAccountName,Sid,cbSid,ReferencedDomainName,cchReferencedDomainName,peUse);
	else
	{
		DWORD dwReferencedOrig=*cchReferencedDomainName;
		char* aReferencedDomainName=new char[dwReferencedOrig+2];
		BOOL bRet=::LookupAccountNameA(W2A(lpSystemName),W2A(lpAccountName),Sid,cbSid,aReferencedDomainName,cchReferencedDomainName,peUse);
		if (bRet && dwReferencedOrig>0)
			MultiByteToWideChar(CP_ACP,0,aReferencedDomainName,*cchReferencedDomainName,ReferencedDomainName,dwReferencedOrig);
		delete[] aReferencedDomainName;
		return bRet;
	}
}

BOOL FileSystem::LookupAccountSid(LPCWSTR lpSystemName,PSID lpSid,LPWSTR lpName,LPDWORD cchName,
	LPWSTR lpReferencedDomainName,LPDWORD cchReferencedDomainName,PSID_NAME_USE peUse)
{
	if (IsUnicodeSystem())
		return ::LookupAccountSidW(lpSystemName,lpSid,lpName,cchName,lpReferencedDomainName,cchReferencedDomainName,peUse);
	else
	{
		DWORD dwNameOrig=*cchName,dwReferencedOrig=*cchReferencedDomainName;

		char* aName=new char[dwNameOrig+2];
		char* aReferencedDomainName=new char[dwReferencedOrig+2];
		BOOL bRet=::LookupAccountSidA(W2A(lpSystemName),lpSid,aName,cchName,aReferencedDomainName,cchReferencedDomainName,peUse);
		if (bRet)
		{
			if (dwNameOrig>0)
				MultiByteToWideChar(CP_ACP,0,aName,*cchName,lpName,dwNameOrig);
			if (dwReferencedOrig>0)
				MultiByteToWideChar(CP_ACP,0,aReferencedDomainName,*cchReferencedDomainName,lpReferencedDomainName,dwReferencedOrig);
		}
		delete[] aReferencedDomainName;
		delete[] aName;
		return bRet;
	}
}

DWORD WINAPI FileSystem::GetModuleFileName(HMODULE hModule,LPWSTR lpFilename,DWORD nSize)
{
	if (IsUnicodeSystem())
		return ::GetModuleFileNameW(hModule,lpFilename,nSize);
	else
	{
		CHAR sPathA[MAX_PATH+10];
		DWORD dwRet=::GetModuleFileNameA(hModule,sPathA,MAX_PATH+10);
		if (dwRet==0)
			return 0;
		MultiByteToWideChar(CP_ACP,0,sPathA,dwRet+1,lpFilename,nSize);
		return dwRet;
	}
}
#endif

BOOL FileSystem::GetStatus(LPCSTR lpszFileName,CFileStatus& rStatus)
{
#ifdef WIN32
	HANDLE hFile;
	BY_HANDLE_FILE_INFORMATION fi;
	LPTSTR temp;
	if (!GetFullPathName(lpszFileName,_MAX_PATH,rStatus.m_szFullName,&temp))
		return FALSE;
	
	hFile=CreateFile(rStatus.m_szFullName,0,
		FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,NULL);
	if (hFile==INVALID_HANDLE_VALUE)
		return FALSE;
	DebugOpenHandle(dhtFile,hFile,rStatus.m_szFullName);

	if (!GetFileInformationByHandle(hFile,&fi))
	{
		CloseHandle(hFile);
		DebugCloseHandle(dhtFile,hFile,rStatus.m_szFullName);
		return FALSE;
	}
	CloseHandle(hFile);
	DebugCloseHandle(dhtFile,hFile,rStatus.m_szFullName);
		
	rStatus.m_ctime=fi.ftCreationTime;
	rStatus.m_mtime=fi.ftLastWriteTime;
	rStatus.m_atime=fi.ftLastAccessTime;
	rStatus.m_size=fi.nFileSizeLow;
	rStatus.m_attribute=fi.dwFileAttributes;
#else
	struct ffblk find;
	if (findfirst(lpszFileName,&find,FA_RDONLY|FA_HIDDEN|FA_SYSTEM|FA_LABEL|FA_DIREC|FA_ARCH))
		return FALSE;
	CTime tim(find.ff_fdate,find.ff_ftime);
	rStatus.m_ctime=tim;
	rStatus.m_mtime=tim;
	rStatus.m_atime=tim;
	rStatus.m_size=find.ff_fsize;
	rStatus.m_attribute=find.ff_attrib;
	strcpy(rStatus.m_szFullName,lpszFileName);
#endif
	return TRUE;
}

BOOL  FileSystem::SetStatus(LPCSTR lpszFileName,const CFileStatus& status)
{
#ifdef WIN32
	HANDLE hFile;
	hFile=CreateFile(lpszFileName,GENERIC_WRITE,
		FILE_SHARE_READ,NULL,OPEN_EXISTING,
		status.m_attribute,NULL);
	if (hFile==INVALID_HANDLE_VALUE)
		return FALSE;
	DebugOpenHandle(dhtFile,hFile,lpszFileName);
		
	if (!SetFileTime(hFile,&((FILETIME)status.m_ctime),&((FILETIME)status.m_atime),&((FILETIME)status.m_mtime)))
	{
		CloseHandle(hFile);
		DebugCloseHandle(dhtFile,hFile,lpszFileName);
		return FALSE;
	}
	CloseHandle(hFile);
	DebugCloseHandle(dhtFile,hFile,lpszFileName);
		
	return SetFileAttributes(lpszFileName,status.m_attribute);
#else
	UINT ftime,fdate;
	FILE* fp=fopen(lpszFileName,"ab");
	if (fp==NULL)
		return FALSE;
	fdate=(status.m_ctime.GetYear()-80)<<9+status.m_ctime.GetMonth()<<5+status.m_ctime.GetDay();
	ftime=status.m_ctime.GetHour()<<11+status.m_ctime.GetMinute()<<5+status.m_ctime.GetSecond()>>1;
	_dos_setfileattr(lpszFileName,status.m_attribute);
	_dos_setftime(fileno(fp),fdate,ftime);
	fclose(fp);
	return TRUE;
#endif
}