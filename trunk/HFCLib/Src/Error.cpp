////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////

#include "HFCLib.h"



static HFCERROR HFCErrorCode=0;

static HFCERRCALLBACK pfErrorCallback=NULL;
static DWORD dwErrorData=0;

#undef SetHFCError

HFCERROR SetHFCError(HFCERROR nErr)
{
	HFCErrorCode=nErr;
	DebugFormatMessage("HFC Error code %X:\"%s\"",(DWORD)nErr,GetStdErrorStr(nErr));
	if (pfErrorCallback!=NULL)
		return HFCErrorCode=pfErrorCallback(HFCErrorCode,dwErrorData);
	return HFCErrorCode;
}	

HFCERROR SetHFCError2(HFCERROR nErr,int line,char* file)
{
	HFCErrorCode=nErr;
	DebugFormatMessage("HFC Error code %X:\"%s\" at line %d int file %s",(DWORD)nErr,GetStdErrorStr(nErr),line,file);
	if (pfErrorCallback!=NULL)
		return HFCErrorCode=pfErrorCallback(HFCErrorCode,dwErrorData);
	return HFCErrorCode;
}	



HFCERROR GetHFCError(void)
{
	return HFCErrorCode;
}

HFCERRCALLBACK SetHFCErrorCallback(HFCERRCALLBACK pfCallbackFunc,DWORD dwData)
{
	HFCERRCALLBACK pfOldFunc=pfErrorCallback;
	pfErrorCallback=pfCallbackFunc;
	dwErrorData=dwData;
	return pfOldFunc;
}

// Standard callbacks
HFCERROR StdHFCErrorCallbackStdout(HFCERROR nError,DWORD_PTR dwData)
{
	if (nError!=HFC_NOERROR)
	{
		if (dwData==NULL)
			fprintf(stdout,"HFCERROR %X:%s\n",(DWORD)nError,GetStdErrorStr(nError));
		else
			fprintf(stdout,(char*)dwData,(DWORD)nError,GetStdErrorStr(nError));
	}
	return nError;
}

HFCERROR StdHFCErrorCallbackStderr(HFCERROR nError,DWORD_PTR dwData)
{
	if (nError!=HFC_NOERROR)
	{
		if (dwData==NULL)
			fprintf(stderr,"HFCERROR %X:%s\n",(DWORD)nError,GetStdErrorStr(nError));
		else
			fprintf(stderr,(char*)dwData,(DWORD)nError,GetStdErrorStr(nError));
	}
	return nError;
}

HFCERROR StdHFCErrorCallbackMsgBox(HFCERROR nError,DWORD_PTR dwData)
{
	if (nError!=HFC_NOERROR)
	{
		char text[1000];
		StringCbPrintf(text,1000,"Code %X:%s",(DWORD)nError,GetStdErrorStr(nError));
		MessageBox((HWND)dwData,text,"HFC Error",MB_ICONERROR|MB_OK);
	}
	return nError;
}

LPCSTR GetStdErrorStr(HFCERROR nError)
{
	switch ((DWORD)nError)
	{
	case (DWORD)HFC_OK:
		return "No error";
	case (DWORD)HFC_ERROR:
		return "Error";
	case (DWORD)HFC_CANNOTALLOC:
		return "Cannot allocate memory";
	case (DWORD)HFC_CANNOTREAD:
		return "Cannot read from file";
	case (DWORD)HFC_CANNOTDISCARD:
		return "Cannot discard";
	case (DWORD)HFC_CANNOTCREATEHEAP:
		return "Cannog create heap";
    case (DWORD)HFC_CANNOTREADRESOURCE:
		return "Cannot read resource";
	case (DWORD)HFC_CANNOTWRITE:
		return "Cannot write to file";
	case (DWORD)HFC_CANNOTCREATE:
		return "Cannot create file";
	case (DWORD)HFC_CANNOTOPEN:
		return "Cannot open file";
	case (DWORD)HFC_EOF:
		return "End of file";
	case (DWORD)HFC_CANNNOTEXECUTE:
		return "Cannot execute";
	case (DWORD)HFC_DISKFULL:
		return "Disk full";
	case (DWORD)HFC_SHARINGVIOLATION:
		return "Sharing violation";
	case (DWORD)HFC_LOCKVIOLATION:
		return "Lock violation";
	case (DWORD)HFC_ACCESSDENIED:
		return "Access denied";
	case (DWORD)HFC_HARDIO:
		return "Hard I/O";
	case (DWORD)HFC_BADSEEK:
		return "Bad seek";
	case (DWORD)HFC_DIRECTORYFULL:
		return "Directory full";
	case (DWORD)HFC_FILEEXISTS:
		return "File already exists";
	case (DWORD)HFC_FILENOTFOUND:
		return "File not found";
	case (DWORD)HFC_BADPATH:
		return "Bad path";
	case (DWORD)HFC_TOOMANYOPENFILES:
		return "Too many open files";
	case (DWORD)HFC_CORRUPTDATA:
		return "Corrupt data";
	case (DWORD)HFC_INVALIDFORMAT:
		return "Invalid format";
	case (DWORD)HFC_OBSOLETEFUNCTION:
		return "Obsolete funtion";
	case (DWORD)HFC_NOTIMPLEMENTED:
		return "Funtion not implemented";
	case (DWORD)HFC_INVALIDPARAMETER:
		return "Invalid parameter";
	case (DWORD)HFC_UNKNOWNERROR:
	default:
		return "Unknown error";
	}
	return szEmpty;
}

void SysErrorMessage(CString& str,DWORD dwSystemError)
{
	LPVOID lpMsgBuf;
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwSystemError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
	);
	str.Copy(LPCSTR(lpMsgBuf));
	LocalFree( lpMsgBuf );
}
