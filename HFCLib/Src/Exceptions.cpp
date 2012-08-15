////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////

#include "HFCLib.h"



HFCERROR ExceptionErrorFunc(HFCERROR nError,DWORD_PTR dwData)
{
	switch ((DWORD)nError)
	{
	case (DWORD)HFC_OK:
		break;
	case (DWORD)HFC_CANNOTALLOCATE:
		throw CException(CException::cannotAllocate);
		break;
	case (DWORD)HFC_CANNOTREAD:
		throw CFileException(CFileException::readFault);
		break;
	case (DWORD)HFC_CANNOTWRITE:
		throw CFileException(CFileException::writeFault);
		break;
	case (DWORD)HFC_CANNOTCREATE:
		throw CFileException(CFileException::fileCreate);
		break;
	case (DWORD)HFC_CANNOTOPEN:
		throw CFileException(CFileException::fileOpen);
		break;
	case (DWORD)HFC_ENDOFFILE:
		throw CFileException(CFileException::endOfFile);
		break;
	case (DWORD)HFC_CORRUPTDATA:
		throw CException(CException::corruptData);
		break;
	case (DWORD)HFC_OBSOLETEFUNCTION:
		throw CException(CException::obsoleteFunction);
		break;
	case (DWORD)HFC_NOTIMPLEMENTED:
		throw CException(CException::notImplemented);
		break;
	default:
		throw CException(CException::unknown);
		break;
	}
	return nError;
}


/////////////////////////////////////////////
// CException

#ifndef WIN32
CException::CException(CException::exceptionCode cause)
:	m_cause(cause)
{
}
#endif

BOOL CException::GetErrorMessage(LPTSTR lpszError,UINT nMaxError)
{
	LPTSTR msg;
	switch (m_cause)
	{
	case none:
		msg="No error.";
		break;
	case cannotAllocate:
		msg="Cannot allocate memory.";
		break;
	case cannotReadResource:
		msg="Cannot read resource.";
		break;
	case invalidFormat:
		msg="Invalid fileformat.";
		break;
	case cannotExecute:
		msg="Cannot execute.";
		break;
	case cannotCreateHeap:
		msg="Cannot create heap.";
		break;
	case obsoleteFunction:
		msg="Obsolete function";
		break;
	case cannotDiscard:
		msg="Cannot discard.";
		break;
	case notImplemented:
		msg="Not implemented.";
		break;
	case corruptData:
		msg="Corrupt data.";
		break;
	case invalidParameter:
		msg="Invalid parameter.";
		break;
	case noAccess:
		msg="No access.";
		break;
	default:
		msg="Unknown error.";
		break;
	}
	UINT msglen=istrlen(msg);

	if (nMaxError<=msglen)
	{
		StringCbCopy(lpszError,nMaxError-1,msg);
	}
	else
	{
		MemCopy(lpszError,msg,msglen+1);
		
		
		char szTmp2[100];
		if (StringCbPrintf(szTmp2,100," OS err: %d",m_lOsError)==S_OK)
		{
			UINT dwLen=(UINT)strlen(szTmp2);
			if (msglen+dwLen<nMaxError)
				MemCopy(lpszError+msglen,szTmp2,dwLen+1);
		}
	}
	return TRUE;
}

#ifdef WIN32
int CException::ReportError(UINT nType,UINT nMessageID)
{
	char msg[1000];
	if (!GetErrorMessage(msg,1000))
	{
#ifdef DEF_RESOURCES
		if (nMessageID)
			return LoadString(nMessageID,msg,1000);
		else
#endif
		StringCbCopy(msg,1000,"No error message is available.");
	}
	return MessageBox(GetActiveWindow(),msg,"File Error",nType);	
}
#else

int CException::ReportError()
{
	char msg[1000];
	if (!GetErrorMessage(msg,1000))
		return fprintf(stderr,"No error message is available.\n");
	else
		return fprintf(stderr,"%s\n",msg);
}
#endif

HFCERROR CException::GetHFCErrorCode() const
{
	switch (m_cause)
	{
	case none:
		return HFC_NOERROR;
	case cannotAllocate:
		return HFC_CANNOTALLOCATE;
	case cannotReadResource:
		return HFC_CANNOTREADRESOURCE;
	case invalidFormat:
		return HFC_INVALIDFORMAT;
	case cannotExecute:
		return HFC_CANNNOTEXECUTE;
	case cannotCreateHeap:
		return HFC_CANNOTCREATEHEAP;
	case cannotDiscard:
		return HFC_CANNOTDISCARD;
	case obsoleteFunction:
		return HFC_OBSOLETEFUNCTION;
	case notImplemented:
		return HFC_NOTIMPLEMENTED;
	case corruptData:
		return HFC_CORRUPTDATA;
	case invalidParameter:
		return HFC_INVALIDPARAMETER;
	case noAccess:
		return HFC_ACCESSDENIED;
	case unknown:
	default:
		return HFC_UNKNOWNERROR;
	}
}


/////////////////////////////////////////////
// CFileException



#ifdef WIN32
CException::exceptionCode CFileException::OsErrorToException(LONG lOsError)
{
	switch(lOsError)
	{
	case NO_ERROR:
		return CException::none;
	case ERROR_FILE_NOT_FOUND:
		return (CException::exceptionCode)fileNotFound;
	case ERROR_CURRENT_DIRECTORY:
		return (CException::exceptionCode)removeCurrentDir;
	case ERROR_DIRECTORY:
	case ERROR_PATH_NOT_FOUND:
		return (CException::exceptionCode)badPath;
	case ERROR_TOO_MANY_OPEN_FILES:
		return (CException::exceptionCode)tooManyOpenFiles;
	case ERROR_ACCESS_DENIED:
		return (CException::exceptionCode)accessDenied;
	case ERROR_INVALID_HANDLE:
		return (CException::exceptionCode)invalidFile;
	case ERROR_GEN_FAILURE:
		return (CException::exceptionCode)generic;
	case ERROR_WRITE_FAULT:
		return (CException::exceptionCode)writeFault;
	case ERROR_READ_FAULT:
		return (CException::exceptionCode)readFault;
	case ERROR_SHARING_VIOLATION:
		return (CException::exceptionCode)sharingViolation;
	case ERROR_LOCK_VIOLATION:
		return (CException::exceptionCode)lockViolation;
	case ERROR_DISK_FULL:
		return (CException::exceptionCode)diskFull;
	case ERROR_HANDLE_EOF:
		return (CException::exceptionCode)endOfFile;
	case ERROR_IO_DEVICE:
	case ERROR_IO_INCOMPLETE:
	case ERROR_IO_PENDING:
	case ERROR_IOPL_NOT_ENABLED:
	case ERROR_BUS_RESET:
		return (CException::exceptionCode)hardIO;
	case ERROR_FILE_CORRUPT:
		return (CException::exceptionCode)fileCorrupt;
	case ERROR_FILE_EXISTS:
		return (CException::exceptionCode)fileExist;
	case ERROR_CANNOT_MAKE:
		return (CException::exceptionCode)fileCreate;
	case ERROR_OPEN_FAILED:
		return (CException::exceptionCode)fileOpen;
	case ERROR_WRITE_PROTECT:
		return (CException::exceptionCode)writeProtected;
	case ERROR_BAD_PATHNAME:
		return (CException::exceptionCode)badPath;
	case ERROR_NOACCESS:
		return CException::noAccess;
	}
	return CException::unknown;
}
#endif	

HFCERROR CFileException::GetHFCErrorCode() const
{
	switch (m_cause)
	{
		case none:
			return HFC_NOERROR;
		case generic:
			return HFC_NOERROR;
		case fileNotFound:
			return HFC_FILENOTFOUND;
		case badPath:
			return HFC_BADPATH;
		case tooManyOpenFiles:
			return HFC_TOOMANYOPENFILES;
		case accessDenied:
		case writeProtected:
			return HFC_ACCESSDENIED;
		case invalidFile:
		case fileCorrupt:
			return HFC_INVALIDFILE;
		case fileExist:
			return HFC_FILEEXISTS;
		case removeCurrentDir:
			return HFC_NOERROR;
		case directoryFull:
			return HFC_DIRECTORYFULL;
		case badSeek:
			return HFC_BADSEEK;
		case hardIO:
			return HFC_HARDIO;
		case sharingViolation:
			return HFC_SHARINGVIOLATION;
		case lockViolation:
			return HFC_LOCKVIOLATION;
		case writeFault:
			return HFC_CANNOTWRITE;
		case readFault:
			return HFC_CANNOTREAD;
		case fileCreate:
			return HFC_CANNOTCREATE;
		case diskFull:
			return HFC_DISKFULL;
		case endOfFile:
			return HFC_ENDOFFILE;
		case fileOpen:
			return HFC_CANNOTOPEN;
		case unknown:
			return HFC_NOERROR;
	default:
		return CException::GetHFCErrorCode();
	}
}


BOOL CFileException::GetErrorMessage(LPTSTR lpszError,UINT nMaxError)
{
	LPTSTR msg;
	DWORD len;
	switch (m_cause)
	{
	case none:
		msg="No error.";
		break;
	case generic:
		msg="Generic error.";
		break;
	case fileNotFound:
		msg="File not found.";
		break;
	case badPath:
		msg="Invalid path.";
		break;
	case tooManyOpenFiles:
		msg="Too many open files.";
		break;
	case accessDenied:
		msg="Access denied.";
		break;
	case invalidFile:
		msg="Invalid file.";
		break;
	case fileCorrupt:
		msg="File corrupt.";
		break;
	case fileExist:
		msg="File exist.";
		break;
	case fileCreate:
		msg="Cannot create file.";
		break;
	case fileOpen:
		msg="Cannot open the specified file.";
		break;
	case removeCurrentDir:
		msg="Removing current directory.";
		break;
	case directoryFull:
		msg="Directory full.";
		break;
	case badSeek:
		msg="Bad seek.";
		break;
	case hardIO:
		msg="Invalid IO operation.";
		break;
	case sharingViolation:
		msg="Sharing violation.";
		break;
	case lockViolation:
		msg="Lock violation";
		break;
	case writeProtected:
		msg="Disk is write protected.";
		break;
	case writeFault:
		msg="Write fault.";
		break;
	case readFault:
		msg="Read fault.";
		break;
	case diskFull:
		msg="Disk full.";
		break;
	case endOfFile:
		msg="End of file.";
		break;
	case unknown:
		msg="Unknown error.";
		break;
	default:
		return CException::GetErrorMessage(lpszError,nMaxError);
	}
	len=(DWORD)istrlen(msg);
	if (nMaxError<=len)
	{
		MemCopy(lpszError,msg,nMaxError-1);
		lpszError[nMaxError-1]='\0';
	}
	else
	{
		MemCopy(lpszError,msg,len);
		if (nMaxError<len+m_strFileName.GetLength()+8)
			lpszError[len]='\0';
		else if (!m_strFileName.IsEmpty())
		{
			fMemCopy(lpszError+len," path: ",7);
#ifdef DEF_WCHAR
			WideCharToMultiByte(CP_ACP,0,(LPCWSTR)m_strFileName,(int)m_strFileName.GetLength()+1,lpszError+len+7,nMaxError-len-7,NULL,NULL);
#else
			StringCbCopy(lpszError+len+7,nMaxError-len-7,m_strFileName);
#endif
			len+=(DWORD)m_strFileName.GetLength()+7;
		}


		char szTmp2[100];
		if (StringCbPrintf(szTmp2,100," OS err: %d",m_lOsError)==S_OK)
		{
			UINT dwLen=(UINT)strlen(szTmp2);
			if (len+dwLen<nMaxError)
				MemCopy(lpszError+len,szTmp2,dwLen+1);
		}

	}
	return TRUE;
}


HFCERROR COleException::GetHFCErrorCode() const
{
	return HFC_OLEERROR;
}

BOOL COleException::GetErrorMessage(LPTSTR lpszError,UINT nMaxError)
{
	char text[1000];
	StringCbPrintf(text,1000,"OLE error code=%X facility=%X severity=%s HRESULT=%X",LOWORD(m_hresError),(DWORD(m_hresError)>>16)&0x4F,SUCCEEDED(m_hresError)?"succeeded":"failed",m_hresError);

	lstrcpyn(lpszError,text,nMaxError-1);
	return TRUE;
}