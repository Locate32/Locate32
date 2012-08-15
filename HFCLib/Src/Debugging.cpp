////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////

#define HFC_NODEBUGNEW // no "#define new DEBUG_NEW"
#include "HFCLib.h"

#if defined(_DEBUG)
#include <crtdbg.h>

#define HFC_NORMAL_BLOCK (_CLIENT_BLOCK|(0xc0<<16)) // Normal
#define HFC_ARRAY_BLOCK (_CLIENT_BLOCK|(0xd0<<16)) // Array

#endif


#ifdef _DEBUG_LOGGING

#define LOGGING_NONE			1
#define LOGGING_FILE			2


DWORD dwDebugFlags=0;

void DebugSetFlags(DebugFlags bDebugFlag,BOOL bRemove)
{
	if (bRemove)
		dwDebugFlags&=~bDebugFlag;
	else
		dwDebugFlags|=bDebugFlag;
}

static HANDLE hLogFile=NULL;
#ifdef WIN32
static char* pLogFile=NULL;
#endif
static DWORD nLoggingType=LOGGING_FILE;


struct DebugHandleInfo {
	DebugHandleType bType;
	void* pValue;
	LPCSTR szInfo;
	int iLine;
	LPCSTR szFile;
	DWORD hThreadId;

	DebugHandleInfo* pNext;
};

static void ClearDebugHandleInfo(DebugHandleInfo* pInfo)
{
	if (pInfo->szInfo!=NULL)
		free((void*)pInfo->szInfo);
	if (pInfo->szFile!=NULL)
		free((void*)pInfo->szFile);
}

DebugHandleInfo* pFirstDebugHandle=NULL;
DebugHandleInfo* pLastDebugHandle=NULL;
CRITICAL_SECTION cHandleCriticalSection={0,0,0,0,0};


#ifdef WIN32
LPCSTR GetDebugLoggingFile()
{
	return pLogFile;
}
#endif

void StartDebugLogging()
{
#ifdef WIN32
	CRegKey RegKey;
	CString File("HFCDebug.log");
	if (RegKey.OpenKey(HKCU,"Software\\HFCLib\\Debuging",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
	{
		RegKey.QueryValue("Type",nLoggingType);
		RegKey.QueryValue("File",File);
		RegKey.CloseKey();
	}
	if (nLoggingType==LOGGING_FILE)
	{
		if (hLogFile!=NULL)
			DebugMessage("HFCLIB: StartDebugLogging(): Logging is already running");
		else
		{
			char szFullPath[MAX_PATH],*temp;
			int iLen=GetFullPathName(File,MAX_PATH,szFullPath,&temp)+1;
			pLogFile=(char*)malloc(iLen);
			CopyMemory(pLogFile,szFullPath,iLen);
			CharLower(pLogFile);
			
			hLogFile=CreateFile(File,GENERIC_WRITE|GENERIC_READ,
				FILE_SHARE_READ|FILE_SHARE_WRITE,
				NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
			if (hLogFile==INVALID_HANDLE_VALUE)
			{
				MessageBox(NULL,"Cannot create HFCDEBUG.LOG file. Logging disabled","LOGGING FAILED",MB_OK|MB_ICONSTOP);
				nLoggingType=LOGGING_NONE;
				return;
			}
			
			SetFilePointer(hLogFile,0,NULL,FILE_END);
			char Text[400];
			DWORD writed;
			DWORD verMS=GetHFCLibVerMS();
			DWORD verLS=GetHFCLibVerLS();
			DWORD verFlags=GetHFCLibFlags();

			char szHeader[]="\r\n#################################################################\r\n";
			WriteFile(hLogFile,szHeader,(DWORD)strlen(szHeader),&writed,NULL);
			StringCbPrintf(Text,400,"# LOGGING STARTED. HFCLIB Ver: %d.%d.%d.%d  %s %s %s",
				HIWORD(verMS),LOWORD(verMS),HIWORD(verLS),LOWORD(verLS),(verFlags&HFCFLAG_DEBUG?"DEBUG":"RELEASE"),(verFlags&HFCFLAG_DLL?"DLL":"STATIC"),(verFlags&HFCFLAG_WIN32?"WIN32":"DOS")); 
			WriteFile(hLogFile,Text,(DWORD)strlen(Text),&writed,NULL);
			WriteFile(hLogFile,szHeader,(DWORD)strlen(szHeader),&writed,NULL);
			FlushFileBuffers(hLogFile);
			
			CAppData* pAppData=GetAppData();
#ifdef DEF_APP
			if (pAppData!=NULL)
			{
				if (pAppData->pAppClass!=NULL)
				{
					StringCbPrintf(Text,400,"APP: %s EXE: %s",
						LPCSTR(pAppData->pAppClass->GetAppName()),
						LPCSTR(pAppData->pAppClass->GetExeName())); 
					DebugMessage(Text);
				}
			}
#endif	
		}
	}
#else
	if (nLoggingType!=LOGGING_NONE)
	{
		if (hLogFile!=NULL)
			DebugMessage("HFCLIB: TRY TO START DEGUG LOGGIN SECOND TIME.");
		else
		{
			hLogFile=CreateFile("HFCDEBUG.LOG",GENERIC_WRITE,
				FILE_SHARE_READ|FILE_SHARE_WRITE,
				NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
			if (hLogFile==INVALID_HANDLE_VALUE)
			{
				MessageBox(NULL,"Cannot create HFCDEBUG.LOG file. Logging disabled","LOGGING FAILED",MB_OK|MB_ICONSTOP);
				nLoggingType=LOGGING_NONE;
				return;
			}
			SetFilePointer(hLogFile,0,NULL,FILE_END);
			char Text[400];
			DWORD verMS=GetHFCLibVerMS();
			DWORD verLS=GetHFCLibVerLS();
			DWORD verFlags=GetHFCLibFlags();
			sprintf(Text,"HFCLIB: LOGGING STARTED. HFCLIB Ver: %d.%d.%d.%d  %s %s %s",
				HIWORD(verMS),LOWORD(verMS),HIWORD(verLS),HIWORD(verLS),(verFlags&HFCFLAG_DEBUG)?"DEBUG":"RELEASE"),(verFlags&HFCFLAG_DLL?"DLL":"STATIC"),(verFlags&HFCFLAG_WIN32?"WIN32":"DOS")); 
			DebugMessage(Text);
		}
	}
#endif
}

void DebugMessage(LPCSTR msg)
{
#ifdef WIN32
	if (nLoggingType==LOGGING_FILE)
	{
		if (hLogFile==NULL)
		{
			StartDebugLogging();
			if (hLogFile==NULL)
				return;
		}
		char szBufr[2000];
		SYSTEMTIME st;
		DWORD writed;
		GetLocalTime(&st);
		if (StringCbPrintf (szBufr,2000,"%02d%02d%02d %02d:%02d:%02d.%03d TID=%4X: %s\r\n",
			st.wYear%100,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond,
			st.wMilliseconds,GetCurrentThreadId(),msg)==S_OK )
			WriteFile(hLogFile,szBufr,(DWORD)strlen(szBufr),&writed,NULL);
		FlushFileBuffers(hLogFile);
	}
#else
	if (nLoggingType==LOGGING_FILE)
	{
		if (hLogFile==NULL)
		{
			StartDebugLogging();
			if (hLogFile==NULL)
				return;
		}
		char szBufr[2000];
		time_t ct=time(NULL);
		struct tm* Time=localtime(&ct);
		int len=sprintf(szBufr,"DATE %4d.%2d.%2d TIME: %2d.%2d.%2d.%3d MSG: %s\r\n",
			Time->tm_year+1900,Time->tm_mon+1,Time->tm_mday,Time->tm_hour,Time->tm_min,Time->tm_sec,0,msg);
		fwrite(szBufr,1,len,(FILE*)hLogFile);
		fflush((FILE*)hLogFile);
	}
#endif
}


#define LINELEN	16

static int formhexline(char* line,BYTE* pData,SIZE_T datalen)
{
	if (datalen==0)
		return 0;

    if (datalen>LINELEN)
		datalen=LINELEN;

	SIZE_T i;
	char* ptr=line;
	for (i=0;i<datalen;i++)
	{
		if (i==LINELEN/2)
			*(ptr++)=' ';
		
		StringCbPrintf(ptr,5,"%02X ",pData[i]);
		while(*ptr!='\0')
			ptr++;
			
	}
	for (;i<LINELEN;i++)
	{
		if (i==LINELEN/2)
			*(ptr++)=' ';
		*(ptr++)=' ';
		*(ptr++)=' ';
		*(ptr++)=' ';
	}
	*(ptr++)=' ';
	*(ptr++)=' ';

	
	for (i=0;i<datalen;i++)
	{
		if (IsCharAlphaNumeric(pData[i]))
			*(ptr++)=pData[i];
		else
			*(ptr++)='.';
	}

	*(ptr++)='\r';
	*(ptr++)='\n';
	*(ptr++)='\0';
	return (int)datalen;
}


void DebugHexDump(LPCSTR desc,BYTE* pData,SIZE_T datalen)
{
#ifdef WIN32
	if (nLoggingType==LOGGING_FILE)
	{
		if (hLogFile==NULL)
		{
			StartDebugLogging();
			if (hLogFile==NULL)
				return;
		}
		char szBufr[2000];
		SYSTEMTIME st;
		DWORD writed;
		GetLocalTime(&st);
		if (StringCbPrintf (szBufr,2000,"%02d%02d%02d %02d:%02d:%02d.%03d TID=%4X HEXDUMP %s len=%d:\r\n",
			st.wYear%100,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond,
			st.wMilliseconds,GetCurrentThreadId(),desc,datalen)==S_OK)
            WriteFile(hLogFile,szBufr,(DWORD)strlen(szBufr),&writed,NULL);
		
		char line[3*LINELEN+LINELEN+10];
		int ret;
		int index=0;
		while ((ret=formhexline(line,pData,datalen)))
		{
			char indstr[12];
			if (StringCbPrintf(indstr,12,"%08X ",index)==S_OK)
				WriteFile(hLogFile,indstr,(DWORD)strlen(indstr),&writed,NULL);
			WriteFile(hLogFile,line,(DWORD)strlen(line),&writed,NULL);
            			
			pData+=ret;
			datalen-=ret;
			index+=ret;
		}
		
		FlushFileBuffers(hLogFile);
	}
#else
	if (nLoggingType==LOGGING_FILE)
	{
		if (hLogFile==NULL)
		{
			StartDebugLogging();
			if (hLogFile==NULL)
				return;
		}
		char szBufr[2000];
		time_t ct=time(NULL);
		struct tm* Time=localtime(&ct);
		int len=sprintf(szBufr,"%4d.%2d.%2d %2d:%2d.%2d.%3d HEX: %s\r\n",
			Time->tm_year+1900,Time->tm_mon+1,Time->tm_mday,Time->tm_hour,Time->tm_min,Time->tm_sec,0,desc);
		fwrite(szBufr,1,len,(FILE*)hLogFile);

		char line[3*LINELEN+LINELEN+10];
		int ret;
		size_t index=0;
		while ((ret=formhexline(line,pData,datalen)))
		{
			char indstr[12];
			size_t ret2=wsprintf(indstr,"%08X ",index);
			
			WriteFile(hLogFile,indstr,ret2,&writed,NULL);
            WriteFile(hLogFile,line,strlen(line),&writed,NULL);
            			
			pData+=ret;
			datalen-=ret;
			index+=ret;
		}

		fflush((FILE*)hLogFile);
	}
#endif
}

#ifdef WIN32
void DebugWndMessage(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	if (nLoggingType==LOGGING_FILE)
	{
		char buf[2000];
		char buf2[100];
		MsgToText(msg,buf2,100);
		StringCbPrintf(buf,2000,"HWND: %lX MSG: %s WPARAM: %lX LPARAM: %lX",(ULONG_PTR)hWnd,buf2,(ULONG_PTR)wParam,(ULONG_PTR)lParam);
		DebugMessage(buf);
	}
}
#endif


void DebugNumMessage(LPCSTR text,DWORD num)
{
	if (nLoggingType==LOGGING_FILE)
	{
		char buf[2000];
		StringCbPrintf(buf,2000,text,num);
		DebugMessage(buf);
	}
}

void DebugFormatMessage(LPCSTR text,...)
{
	if (nLoggingType==LOGGING_FILE)
	{
		va_list argList;
		va_start(argList,text);
		
		char buf[2000];
		StringCbVPrintf(buf,2000,text,argList);
		DebugMessage(buf);

		va_end(argList);
		
	}
}

#ifdef DEF_WCHAR
void DebugMessage(LPCWSTR msg)
{
	if (nLoggingType==LOGGING_FILE)
	{
		if (hLogFile==NULL)
		{
			StartDebugLogging();
			if (hLogFile==NULL)
				return;
		}
		char szBufr[2000];

		int iLen=istrlenw(msg)+1;
		char* msgA=(char*)malloc(iLen);
		WideCharToMultiByte(CP_ACP,0,msg,iLen,msgA,iLen,0,0);
		SYSTEMTIME st;
		DWORD writed;
		GetLocalTime(&st);
		if (StringCbPrintf (szBufr,2000,"%02d%02d%02d %02d:%02d:%02d.%03d TID=%4X: %s\r\n",
			st.wYear%100,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond,
			st.wMilliseconds,GetCurrentThreadId(),msgA)==S_OK )
			WriteFile(hLogFile,szBufr,(DWORD)strlen(szBufr),&writed,NULL);
		FlushFileBuffers(hLogFile);
		free(msgA);
	}
}

void DebugNumMessage(LPCWSTR text,DWORD num)
{
	if (nLoggingType==LOGGING_FILE)
	{
		char buf[2000];
		StringCbPrintf(buf,2000,(LPCSTR)W2A(text),num);
		DebugMessage(buf);
	}
}


void DebugFormatMessage(LPCWSTR text,...)
{
	if (nLoggingType==LOGGING_FILE)
	{
		va_list argList;
		va_start(argList,text);
		WCHAR buf[2000];
		StringCbVPrintfW(buf,2000,text,argList);
		DebugMessage(buf);
		
		va_end(argList);
	}
}
#endif

void StopDebugLogging()
{
#ifdef WIN32
	if (nLoggingType==LOGGING_FILE)
	{
		DebugMessage("HFCLIB: LOGGING STOPPED");
		CloseHandle(hLogFile);
		if (pLogFile!=NULL)
		{
			free(pLogFile);
			pLogFile=NULL;
		}
		hLogFile=NULL;

		nLoggingType=LOGGING_NONE;
	}
#else
	if (nLoggingType!=LOGGING_NONE)
	{
		DebugMessage("HFCLIB: LOGGING STOPPED");
		fclose((FILE*)hLogFile);
		hLogFile=NULL;
	}
#endif
}

LPCSTR DebugHandleTypeToStr(DebugHandleType bType)
{
	switch (bType)
	{
	case dhtMemoryBlock:
		return "Memory Block";
	case dhtFile:
		return "File";
	case dhtFileFind:
		return "File find";
	case dhtThread:
		return "Thread";
	case dhtProcess:
		return "Process";
	case dhtEvent:
		return "Event";
	case dhtGdiObject:
		return "GDI object";
	case dhtWindow:
		return "Window";
	case dhtRegKey:
		return "Regkey";
	case dhtMenu:
		return "Menu";
	case dhtMutex:
		return "Mutex";
	case dhtMisc:
		return "Misc";
	default:
		return "Unknown";
	}
}

void DebugOpenHandle2(DebugHandleType bType,void* pValue,LPCSTR szInfo,int iLine,LPCSTR szFile)
{
	if (pValue==NULL || pValue==INVALID_HANDLE_VALUE)
		return;

	if (dwDebugFlags&DebugLogHandleOperations)
	{
		DebugFormatMessage("%s handle %X opened on line %d in file %s, szInfo=%s",
			DebugHandleTypeToStr(bType),pValue,iLine,szFile,szInfo!=NULL?szInfo:"(NULL)");
	}

	DebugHandleInfo* pNewInfo=(DebugHandleInfo*)malloc(sizeof(DebugHandleInfo));
	pNewInfo->pNext=NULL;
	pNewInfo->bType=bType;
	pNewInfo->pValue=pValue;
	pNewInfo->hThreadId=GetCurrentThreadId();
	if (szInfo!=NULL)
	{
		int iLen=istrlen(szInfo)+1;
		pNewInfo->szInfo=(char*)malloc(iLen);
		CopyMemory((void*)pNewInfo->szInfo,szInfo,iLen);
	}
	else
		pNewInfo->szInfo=NULL;

	pNewInfo->iLine=iLine;
	if (szFile!=NULL)
	{
		int iLen=istrlen(szFile)+1;
		pNewInfo->szFile=(char*)malloc(iLen);
		CopyMemory((void*)pNewInfo->szFile,szFile,iLen);
	}
	else
		pNewInfo->szFile=NULL;

	if (cHandleCriticalSection.DebugInfo!=NULL || 
		cHandleCriticalSection.LockCount!=0)
		EnterCriticalSection(&cHandleCriticalSection);

	if (pFirstDebugHandle!=NULL)
		pLastDebugHandle=pLastDebugHandle->pNext=pNewInfo;
	else
		pLastDebugHandle=pFirstDebugHandle=pNewInfo;
	
	if (cHandleCriticalSection.DebugInfo!=NULL || 
		cHandleCriticalSection.LockCount!=0)
		LeaveCriticalSection(&cHandleCriticalSection);
	
}

void DebugOpenHandle2(DebugHandleType bType,void* pValue,LPCWSTR szInfo,int iLine,LPCSTR szFile)
{
	if (pValue==NULL || pValue==INVALID_HANDLE_VALUE)
		return;

	if (dwDebugFlags&DebugLogHandleOperations)
	{
		DebugFormatMessage("%s handle %X opened on line %d in file %s, szInfo=%S",
			DebugHandleTypeToStr(bType),pValue,iLine,szFile,szInfo!=NULL?szInfo:L"(NULL)");
	}

	DebugHandleInfo* pNewInfo=(DebugHandleInfo*)malloc(sizeof(DebugHandleInfo));
	pNewInfo->pNext=NULL;
	pNewInfo->bType=bType;
	pNewInfo->pValue=pValue;
	pNewInfo->hThreadId=GetCurrentThreadId();
	if (szInfo!=NULL)
	{
		int iLen=istrlenw(szInfo)+1;
		pNewInfo->szInfo=(char*)malloc(iLen);
		WideCharToMultiByte(CP_ACP,0,szInfo,iLen,(LPSTR)pNewInfo->szInfo,iLen,0,0);
	}
	else
		pNewInfo->szInfo=NULL;

	pNewInfo->iLine=iLine;
	if (szFile!=NULL)
	{
		int iLen=istrlen(szFile)+1;
		pNewInfo->szFile=(char*)malloc(iLen);
		CopyMemory((void*)pNewInfo->szFile,szFile,iLen);
	}
	else
		pNewInfo->szFile=NULL;

	if (cHandleCriticalSection.DebugInfo!=NULL || 
		cHandleCriticalSection.LockCount!=0)
		EnterCriticalSection(&cHandleCriticalSection);
	if (pFirstDebugHandle!=NULL)
		pLastDebugHandle=pLastDebugHandle->pNext=pNewInfo;
	else
		pLastDebugHandle=pFirstDebugHandle=pNewInfo;
	if (cHandleCriticalSection.DebugInfo!=NULL || 
		cHandleCriticalSection.LockCount!=0)
		LeaveCriticalSection(&cHandleCriticalSection);
}

void DebugCloseHandle2(DebugHandleType bType,void* pValue,LPCSTR szInfo,int iLine,LPCSTR szFile)
{
	if (pValue==NULL || pValue==INVALID_HANDLE_VALUE)
		return;


	if (cHandleCriticalSection.DebugInfo!=NULL || 
		cHandleCriticalSection.LockCount!=0)
		EnterCriticalSection(&cHandleCriticalSection);


	DebugHandleInfo* pInfo=pFirstDebugHandle;
	DebugHandleInfo* pPrev=NULL;
	while (pInfo!=NULL)
	{
		if (pInfo->pValue==pValue && (pInfo->bType==bType || pInfo->bType==dhtUnknown))
		{
			if (pPrev==NULL)
			{
				// Remove first
				if (pLastDebugHandle==pFirstDebugHandle)
					pLastDebugHandle=NULL;
				pFirstDebugHandle=pInfo->pNext;
			}
			else
			{
				if (pLastDebugHandle==pInfo)
					pLastDebugHandle=pPrev;
				pPrev->pNext=pInfo->pNext;				
			}

			if (dwDebugFlags&DebugLogHandleOperations)
			{
				DebugFormatMessage("%s handle %X closed on line %d in file %s, szInfo=%s",
					DebugHandleTypeToStr(bType),pValue,iLine,szFile,szInfo!=NULL?szInfo:"(NULL)");
			}

			ClearDebugHandleInfo(pInfo);
			free(pInfo);

			LeaveCriticalSection(&cHandleCriticalSection);
			return;
		}


		pPrev=pInfo;
		pInfo=pInfo->pNext;
	}

	if (cHandleCriticalSection.DebugInfo!=NULL || 
		cHandleCriticalSection.LockCount!=0)
		LeaveCriticalSection(&cHandleCriticalSection);


	DebugFormatMessage("DebugCloseHandle2: Could not locate %s entry %X, szInfo=%s, line=%d, file=%s.",
		DebugHandleTypeToStr(bType),pValue,szInfo!=NULL?szInfo:"NULL",iLine,szFile); 
}

void DebugCloseHandle3(DebugHandleType bType,void* pValue,LPCSTR szInfo,int iLine,LPCSTR szFile)
{
	if (pValue==NULL || pValue==INVALID_HANDLE_VALUE)
		return;

	if (cHandleCriticalSection.DebugInfo!=NULL || 
		cHandleCriticalSection.LockCount!=0)
		EnterCriticalSection(&cHandleCriticalSection);


	DebugHandleInfo* pInfo=pFirstDebugHandle;
	DebugHandleInfo* pPrev=NULL;
	while (pInfo!=NULL)
	{
		if (pInfo->pValue==pValue && (pInfo->bType==bType || pInfo->bType==dhtUnknown))
		{
			if (pPrev==NULL)
			{
				// Remove first
				if (pLastDebugHandle==pFirstDebugHandle)
					pLastDebugHandle=NULL;
				pFirstDebugHandle=pInfo->pNext;
			}
			else
			{
				if (pLastDebugHandle==pInfo)
					pLastDebugHandle=pPrev;
				pPrev->pNext=pInfo->pNext;				
			}

			
			if (dwDebugFlags&DebugLogHandleOperations)
			{
				DebugFormatMessage("%s handle %X closed on line %d in file %s, szInfo=%s",
					DebugHandleTypeToStr(bType),pValue,iLine,szFile,szInfo!=NULL?szInfo:"(NULL)");
			}

			ClearDebugHandleInfo(pInfo);
			free(pInfo);


			LeaveCriticalSection(&cHandleCriticalSection);
			return;
		}


		pPrev=pInfo;
		pInfo=pInfo->pNext;
	}

	if (cHandleCriticalSection.DebugInfo!=NULL || 
		cHandleCriticalSection.LockCount!=0)
		LeaveCriticalSection(&cHandleCriticalSection);
}


void DebugCloseHandle2(DebugHandleType bType,void* pValue,LPCWSTR szInfo,int iLine,LPCSTR szFile)
{
	if (szInfo==NULL)
		DebugCloseHandle2(bType,pValue,STRNULL,iLine,szFile); 
	else
		DebugCloseHandle2(bType,pValue,W2A(szInfo),iLine,szFile); 
}

void DebugCloseHandle3(DebugHandleType bType,void* pValue,LPCWSTR szInfo,int iLine,LPCSTR szFile)
{
	if (szInfo==NULL)
		DebugCloseHandle3(bType,pValue,STRNULL,iLine,szFile); 
	else
		DebugCloseHandle3(bType,pValue,W2A(szInfo),iLine,szFile); 
}

void DebugLogOpenHandles()
{
	EnterCriticalSection(&cHandleCriticalSection);
	
	if (pFirstDebugHandle!=NULL)
	{
		DebugMessage("Following handles are marked open:");
		DebugHandleInfo* pInfo=pFirstDebugHandle;
		while (pInfo!=NULL)
		{
			DebugFormatMessage("Open %s handle %X info=%s line=%d file=%s thread id=%X",
				DebugHandleTypeToStr(pInfo->bType),pInfo->pValue,
				pInfo->szInfo!=NULL?pInfo->szInfo:"NULL",pInfo->iLine,
				pInfo->szFile,pInfo->hThreadId);

			pInfo=pInfo->pNext;
		}
		DebugMessage("Open handle list end.");
	}
	LeaveCriticalSection(&cHandleCriticalSection);
}

void DebugClearOpenHandlesList()
{
	EnterCriticalSection(&cHandleCriticalSection);
	
	while (pFirstDebugHandle!=NULL)
	{
		DebugHandleInfo* pTmp=pFirstDebugHandle;
		pFirstDebugHandle=pTmp->pNext;

		ClearDebugHandleInfo(pTmp);
		free(pTmp);
	}
	pLastDebugHandle=NULL;

	LeaveCriticalSection(&cHandleCriticalSection);
}

BOOL WINAPI  TerminateThread(HANDLE hThread,DWORD dwExitCode,BOOL bWaitUntilExited)
{
	// Terminate thread	
	if (!::TerminateThread(hThread,dwExitCode))
		return FALSE;

	/* FIX THIS
	
	
	if (bWaitUntilExited || cHandleCriticalSection.OwningThread==hThread)
		WaitForSingleObject(hThread,1000);


	DWORD dwThreadId=GetThreadId(hThread);

	DebugFormatMessage("Thread %X (id=%X) terminated",hThread,dwThreadId);

	if (GetThreadId(cHandleCriticalSection.OwningThread)==dwThreadId)
	{
		// Close critical section
		cHandleCriticalSection.OwningThread=GetCurrentThread();
		LeaveCriticalSection(&cHandleCriticalSection);
	}

	*/

	
	return TRUE;
}

#ifdef _DEBUG
static void* hfc_debugnew(size_t nSize, LPCSTR lpszFileName, int nLine,DWORD wBlockType)
{
	BYTE* pBlock=(BYTE*)::operator new(nSize+10,wBlockType,lpszFileName,nLine);
	// Writing header
	pBlock[0]=0xab;
	pBlock[5]=0xba;
	*((DWORD*)(pBlock+1))=DWORD(min(nSize,0xFFFFFFFF));
	MemSet(pBlock+nSize+6,0xfa,4);
	return pBlock+6;
}

static void hfc_debugdelete(void* p, LPCSTR lpszFileName, int nLine,DWORD wBlockType)
{
	// Header is valid?
	ASSERT(((BYTE*)p)[-6]==0xab && ((BYTE*)p)[-1]==0xba);
	DWORD dwLen=*(DWORD*)(((BYTE*)p)-5);
	if (dwLen!=0xFFFFFFFF)
	{
		ASSERT(((BYTE*)p)[dwLen]==0xfa && ((BYTE*)p)[dwLen+1]==0xfa &&
			((BYTE*)p)[dwLen+2]==0xfa && ((BYTE*)p)[dwLen+3]==0xfa);
	}

	((BYTE*)p)[-6]=0xaf;
	((BYTE*)p)[-5]=0xaf;
	((BYTE*)p)[-4]=0xaf;
	((BYTE*)p)[-3]=0xaf;
	((BYTE*)p)[-2]=0xaf;
	((BYTE*)p)[-1]=0xaf;

	_free_dbg(((BYTE*)p)-6,wBlockType);
}
#endif


// Memory alloction with tracking, corresponds to DEBUG_NEW
void* operator new(size_t nSize, LPCSTR lpszFileName, int nLine)
{
#ifdef _DEBUG
	void* pBlock=hfc_debugnew(nSize,lpszFileName,nLine,HFC_NORMAL_BLOCK);
#else
	void* pBlock=::operator new(nSize);
#endif
	DebugOpenHandle2(dhtMemoryBlock,pBlock,STRNULL,nLine,lpszFileName);
	return pBlock;
}


void* operator new[](size_t nSize, LPCSTR lpszFileName, int nLine)
{
#ifdef _DEBUG	
	void* pBlock= hfc_debugnew(nSize,lpszFileName,nLine,HFC_ARRAY_BLOCK);
#else
	void* pBlock=::operator new(nSize);
#endif
	DebugOpenHandle2(dhtMemoryBlock,pBlock,STRNULL,nLine,lpszFileName);
	return pBlock;
}

void operator delete(void* p, LPCSTR lpszFileName, int nLine)
{
	DebugCloseHandle2(dhtMemoryBlock,p,STRNULL,nLine,lpszFileName);
#ifdef _DEBUG	
	hfc_debugdelete(p,lpszFileName,nLine,HFC_NORMAL_BLOCK);
#else
	::operator delete(p);
#endif
}

void operator delete[](void* p, LPCSTR lpszFileName, int nLine)
{
	DebugCloseHandle2(dhtMemoryBlock,p,STRNULL,nLine,lpszFileName);
#ifdef _DEBUG	
	hfc_debugdelete(p,lpszFileName,nLine,HFC_ARRAY_BLOCK);
#else
	::operator delete(p);
#endif
}

#ifndef HFC_NOALLOCTRACKING

// Memory alloction, no tracking
void * operator new(size_t nSize)
{
#ifdef _DEBUG
	void* pBlock=hfc_debugnew(nSize,NULL,0,HFC_NORMAL_BLOCK);
#else
	void* pBlock=malloc(nSize);
#endif
	
	DebugOpenHandle2(dhtMemoryBlock,pBlock,STRNULL,__LINE__,__FILE__);
	return pBlock;
}



// Memory alloction, no tracking
void * operator new[](size_t nSize)
{
#ifdef _DEBUG
	void* pBlock=hfc_debugnew(nSize,NULL,0,HFC_ARRAY_BLOCK);
#else
	void* pBlock=::operator new(nSize);
#endif
	DebugOpenHandle2(dhtMemoryBlock,pBlock,STRNULL,__LINE__,__FILE__);
	return pBlock;
}

void operator delete(void *p)
{
	DebugCloseHandle2(dhtMemoryBlock,p,STRNULL,0,NULL);
#ifdef _DEBUG	
	hfc_debugdelete(p,NULL,0,HFC_NORMAL_BLOCK);
#else
	::operator delete(p);
#endif
}

void operator delete[](void *p )
{
	DebugCloseHandle2(dhtMemoryBlock,p,STRNULL,0,NULL);
#ifdef _DEBUG	
	hfc_debugdelete(p,NULL,0,HFC_ARRAY_BLOCK);
#else
	::operator delete(p);
#endif
}

#endif

#endif










//////////////////////////////////////////////////////////////////
// Debug related windows function


#ifdef _DEBUG

#ifdef DEF_WINDOWS
void AddDebugMenuItems(HWND hWnd)
{
	HMENU hMenu=GetSystemMenu(hWnd,FALSE);
	MENUITEMINFO mii;
	mii.cbSize=sizeof(MENUITEMINFO);
	mii.fMask=MIIM_TYPE|MIIM_SUBMENU;
	mii.fType=MFT_STRING;
	mii.hSubMenu=CreatePopupMenu();
	mii.dwTypeData="Debug";
	InsertMenuItem(hMenu,0,TRUE,&mii);

	hMenu=mii.hSubMenu;
	mii.cbSize=sizeof(MENUITEMINFO);
	mii.fMask=MIIM_ID|MIIM_TYPE;
	mii.fType=MFT_STRING;
	
	mii.wID=0x62;
	mii.dwTypeData="Show HFCLib Version";
	InsertMenuItem(hMenu,0,TRUE,&mii);

	mii.fMask=MIIM_TYPE;
	mii.fType=MFT_SEPARATOR;
	mii.dwTypeData="""";
	InsertMenuItem(hMenu,1,TRUE,&mii);
	
	mii.fMask=MIIM_ID|MIIM_TYPE;
	mii.fType=MFT_STRING;
	mii.wID=0x66;
	mii.dwTypeData="Show open handles";
	InsertMenuItem(hMenu,3,TRUE,&mii);
	
	
	mii.fMask=MIIM_TYPE;
	mii.fType=MFT_SEPARATOR;
	mii.dwTypeData="""";
	InsertMenuItem(hMenu,4,TRUE,&mii);
	
	mii.fMask=MIIM_ID|MIIM_TYPE;
	mii.fType=MFT_STRING;
	mii.dwTypeData="View Debug Log";
	mii.wID=0x63;
	InsertMenuItem(hMenu,5,TRUE,&mii);
	mii.wID=0x60;
	mii.dwTypeData="Add Debug Note";
	InsertMenuItem(hMenu,2,TRUE,&mii);
	mii.dwTypeData="Start Debug Logging";
	mii.wID=0x64;
	InsertMenuItem(hMenu,6,TRUE,&mii);
	mii.dwTypeData="Stop Debug Logging";
	mii.wID=0x65;
	InsertMenuItem(hMenu,7,TRUE,&mii);
	
	mii.fMask=MIIM_TYPE;
	mii.fType=MFT_SEPARATOR;
	mii.dwTypeData="""";
	InsertMenuItem(hMenu,8,TRUE,&mii);
	
	mii.fMask=MIIM_TYPE|MIIM_SUBMENU;
	mii.fType=MFT_STRING;
	mii.hSubMenu=CreatePopupMenu();
	mii.dwTypeData="Post message";
	InsertMenuItem(hMenu,9,TRUE,&mii);
	
	mii.fMask=MIIM_ID|MIIM_TYPE;
	mii.fType=MFT_STRING;
	mii.wID=0x70;
	mii.dwTypeData="WM_QUIT";
	InsertMenuItem(mii.hSubMenu,0,TRUE,&mii);
	mii.wID=0x71;
	mii.dwTypeData="WM_DESTROY";
	InsertMenuItem(mii.hSubMenu,1,TRUE,&mii);
	mii.wID=0x72;
	mii.dwTypeData="WM_CLOSE";
	InsertMenuItem(mii.hSubMenu,2,TRUE,&mii);
	mii.wID=0x73;
	mii.dwTypeData="WM_USERCHANGE";
	InsertMenuItem(mii.hSubMenu,3,TRUE,&mii);
	mii.wID=0x74;
	mii.dwTypeData="WM_WININICHANGE";
	InsertMenuItem(mii.hSubMenu,4,TRUE,&mii);

}

void DebugCommandsProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	if (msg==WM_SYSCOMMAND)
	{
		switch (wParam)
		{
		case 0x60:
			AddDebugNote(hWnd);
			break;
		case 0x62:
			MessageBox(hWnd,GetHFCLibStr(),"HFC Library version.",MB_ICONINFORMATION|MB_OK);
			break;
		case 0x63:
			ViewDebugLog(hWnd);
			break;
		case 0x64:
			StartDebugLogging();
			break;
		case 0x65:
			StopDebugLogging();
			break;
		case 0x66:
			ViewOpenHandles(hWnd);
			break;
		case 0x70:
			PostQuitMessage(1);
			break;
		case 0x71:
			PostMessage(hWnd,WM_DESTROY,0,0);
			break;
		case 0x72:
			PostMessage(hWnd,WM_CLOSE,0,0);
			break;
		case 0x73:
			PostMessage(hWnd,WM_USERCHANGED,0,0);
			break;
		case 0x74:
			PostMessage(hWnd,WM_WININICHANGE,0,0);
			break;
		}
	}
}

LRESULT CALLBACK AddDebugNoteWndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		{
			RECT rect;
			GetClientRect(hWnd,&rect);
			HWND hCtrl=CreateWindow("EDIT",szEmpty,WS_VISIBLE|WS_CHILD|
				ES_AUTOHSCROLL|ES_AUTOVSCROLL|ES_MULTILINE|WS_VSCROLL|WS_HSCROLL,
				0,0,rect.right,rect.bottom-20,hWnd,(HMENU)100,GetInstanceHandle(),0);
			SendMessage(hCtrl,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),1);
			hCtrl=CreateWindow("BUTTON","Add To &Note Log",WS_VISIBLE|WS_CHILD|
				BS_DEFPUSHBUTTON|BS_TEXT,0,rect.bottom-20,rect.right/3,20,
				hWnd,(HMENU)101,GetInstanceHandle(),0);
			SendMessage(hCtrl,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),1);
			hCtrl=CreateWindow("BUTTON","Add To &Debug Log",WS_VISIBLE|WS_CHILD|
				BS_PUSHBUTTON|BS_TEXT,rect.right/3,rect.bottom-20,rect.right/3,20,
				hWnd,(HMENU)102,GetInstanceHandle(),0);
			SendMessage(hCtrl,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),1);
			hCtrl=CreateWindow("BUTTON","&Cancel",WS_VISIBLE|WS_CHILD|
				BS_PUSHBUTTON|BS_TEXT,(rect.right/3)*2,rect.bottom-20,rect.right/3,20,
				hWnd,(HMENU)103,GetInstanceHandle(),0);
			SendMessage(hCtrl,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),1);
			SetFocus(GetDlgItem(hWnd,100));
		}
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case 100:
			break;
		case 101:
			{
				CString File;
				CString str;
				CEdit edit(GetDlgItem(hWnd,100));
				{
					CRegKey RegKey;
					if (RegKey.OpenKey(HKLM,"Software\\HFC\\Notify files",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
						RegKey.QueryValue(GetApp()->GetAppName(),File);
				}
				if (!FileSystem::IsValidFileName(File))
				{
					CFileDialog fd(FALSE,"log",NULL,OFN_EXPLORER|OFN_HIDEREADONLY|OFN_NOREADONLYRETURN|OFN_ENABLESIZING,"Log files|*.log|Text files|*.txt||");
					fd.m_pofn->lpstrTitle="Set notify log file";
					fd.EnableFeatures();

					if (!fd.DoModal(hWnd))
						break;
					fd.GetFilePath(File);
					CRegKey RegKey;
					if(RegKey.OpenKey(HKLM,"Software\\HFC\\Notify files",CRegKey::createNew|CRegKey::samAll)==ERROR_SUCCESS)
					{
						CRect rect;
						RegKey.SetValue(GetApp()->GetAppName(),File);
					}
				}
				HANDLE hFile=CreateFile(File,GENERIC_READ|GENERIC_WRITE,
					FILE_SHARE_READ,NULL,OPEN_ALWAYS,
					FILE_ATTRIBUTE_NORMAL,NULL);
				DebugOpenHandle(dhtFile,hFile,File);
				if (hFile==NULL)
					break;
				
				SetFilePointer(hFile,0,NULL,FILE_END);
				DWORD nWrited=0;
				CTime now(CTime::GetCurrentTime());
				str.Format("DEBUG NOTIFY (%s,%s)\r\nBEGIN\r\n",(LPCSTR)GetApp()->GetAppName(),(LPCSTR)now.Format("%x %X"));
				WriteFile(hFile,str,(DWORD)str.GetLength(),&nWrited,NULL);
				if (nWrited<(DWORD)str.GetLength())
				{
					CloseHandle(hFile);
					break;
				}
				edit.GetText(str);
				WriteFile(hFile,str,(DWORD)str.GetLength(),&nWrited,NULL);
				if (nWrited<(DWORD)str.GetLength())
				{
					CloseHandle(hFile);
					DebugCloseHandle(dhtFile,hFile,File);
				
					break;
				}
				WriteFile(hFile,"\r\nEND\r\n",7,&nWrited,NULL);
				if (nWrited<7)
				{
					CloseHandle(hFile);
					DebugOpenHandle(dhtFile,hFile,File);
				
					break;
				}
				CloseHandle(hFile);
				DebugOpenHandle(dhtFile,hFile,File);
				
				DestroyWindow(hWnd);
				break;
			}
		case 102:
			{
				CString str,str2("USER NOTIFY: ");
				CEdit edit(GetDlgItem(hWnd,100));
				edit.GetText(str);
				str2 << str;
				DebugMessage(str2);
				DestroyWindow(hWnd);
				break;
			}
		case 103:
			DestroyWindow(hWnd);
			break;
		}
		break;
	}
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}
 

LRESULT CALLBACK ViewOpenHandlesWndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		{
			RECT rect;
			GetClientRect(hWnd,&rect);
			HWND hCtrl=CreateWindow("BUTTON","&Refresh",WS_VISIBLE|WS_CHILD|BS_PUSHBUTTON|BS_TEXT,
				0,0,rect.right,15,hWnd,(HMENU)101,GetInstanceHandle(),0);
			SendMessage(hCtrl,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),1);
			
			hCtrl=CreateWindow("LISTBOX",szEmpty,WS_VISIBLE|WS_CHILD|
				LBS_NOSEL|WS_VSCROLL|WS_HSCROLL,
				0,15,rect.right,rect.bottom,
				hWnd,(HMENU)100,GetInstanceHandle(),0);
			SendMessage(hCtrl,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),1);
			
			PostMessage(hWnd,WM_COMMAND,101,NULL);			
		}
		break;
	case WM_COMMAND:
		if (LOWORD(wParam)==101)
		{
			HWND hCtrl=GetDlgItem(hWnd,100);
			::SendMessage(hCtrl,LB_RESETCONTENT,0,0);
			
			EnterCriticalSection(&cHandleCriticalSection);
			DebugHandleInfo* pInfo=pFirstDebugHandle;
			while (pInfo!=NULL)
			{
				char szBuffer[1000];
				StringCbPrintf(szBuffer,1000,"%s entry %X, info=%s line=%d file=%s",
					DebugHandleTypeToStr(pInfo->bType),pInfo->pValue,
					pInfo->szInfo!=NULL?pInfo->szInfo:"NULL",pInfo->iLine,pInfo->szFile);

				::SendMessage(hCtrl,LB_ADDSTRING,0,(LPARAM)szBuffer);

				pInfo=pInfo->pNext;
			}
			LeaveCriticalSection(&cHandleCriticalSection);
		}
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SIZE:
		if (wParam!=SIZE_MINIMIZED)
		{
			::SetWindowPos(GetDlgItem(hWnd,100),HWND_TOP,0,15,LOWORD(lParam),HIWORD(lParam),SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOMOVE);
			::SetWindowPos(GetDlgItem(hWnd,101),HWND_TOP,0,0,LOWORD(lParam),15,SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOMOVE);
		}
		break;
	}
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}



static void RefreshDebugLogViewer(HWND hWnd)
{
	if (hLogFile==NULL)
	{
		SetDlgItemText(hWnd,106,"Logging: OFF");
		SetDlgItemText(hWnd,107,"Cannot show log file. Logging is off.");
		return;
	}
	else
		SetDlgItemText(hWnd,106,"Logging: ON");

	SetFilePointer(hLogFile,0,NULL,FILE_BEGIN);
	DWORD dwLength=GetFileSize(hLogFile,NULL);
	char* pData=new char[dwLength];
	DWORD dwReaded;
	BOOL bRet=ReadFile(hLogFile,pData,dwLength,&dwReaded,NULL);
	if (!bRet || dwReaded<dwLength)
	{
		delete[] pData;
		SetDlgItemText(hWnd,107,"Cannot read log file");
		return;
	}
	CString str;
	for (DWORD i=0;i<dwLength;i++)
	{
		if (pData[i]=='\n')
			str << '\r';
		str << pData[i];
	}
	delete[] pData;
	SetDlgItemText(hWnd,107,str);

	SendDlgItemMessage(hWnd,107,EM_SETSEL,dwLength-1,-1);
	SendDlgItemMessage(hWnd,107,EM_SCROLLCARET,0,0);
	SendDlgItemMessage(hWnd,107,EM_SETSEL,-1,-1);
}
			
LRESULT CALLBACK ViewDebugLogWndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		{
			RECT rect;
			GetClientRect(hWnd,&rect);
			
			HWND hCtrl=CreateWindow("BUTTON","&Start logging",WS_VISIBLE|WS_CHILD|
				BS_PUSHBUTTON|BS_TEXT,0,0,80,19,
				hWnd,(HMENU)101,GetInstanceHandle(),0);
			SendMessage(hCtrl,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),1);
			
			hCtrl=CreateWindow("BUTTON","&End logging",WS_VISIBLE|WS_CHILD|
				BS_PUSHBUTTON|BS_TEXT,80,0,80,19,
				hWnd,(HMENU)102,GetInstanceHandle(),0);
			SendMessage(hCtrl,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),1);
			
			hCtrl=CreateWindow("BUTTON","&Clean",WS_VISIBLE|WS_CHILD|
				BS_PUSHBUTTON|BS_TEXT,160,0,50,19,
				hWnd,(HMENU)103,GetInstanceHandle(),0);
			SendMessage(hCtrl,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),1);
			
			hCtrl=CreateWindow("BUTTON","&Refresh",WS_VISIBLE|WS_CHILD|
				BS_PUSHBUTTON|BS_TEXT,210,0,50,19,
				hWnd,(HMENU)104,GetInstanceHandle(),0);
			SendMessage(hCtrl,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),1);
			
			hCtrl=CreateWindow("BUTTON","&Close",WS_VISIBLE|WS_CHILD|BS_PUSHBUTTON|
				BS_TEXT,260,0,50,19,hWnd,(HMENU)105,GetInstanceHandle(),0);
			SendMessage(hCtrl,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),1);
			
			hCtrl=CreateWindow("STATIC","",WS_VISIBLE|WS_CHILD|SS_LEFT,
				312,3,1,1,hWnd,(HMENU)106,GetInstanceHandle(),0);
			SendMessage(hCtrl,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),1);
			
			hCtrl=CreateWindow("EDIT",szEmpty,WS_VISIBLE|WS_CHILD|
				ES_AUTOHSCROLL|ES_AUTOVSCROLL|ES_MULTILINE|ES_READONLY|WS_VSCROLL|WS_HSCROLL,
				0,20,1,1,hWnd,(HMENU)107,GetInstanceHandle(),0);
			SendMessage(hCtrl,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),1);
			
			hCtrl=CreateWindow("EDIT",szEmpty,WS_VISIBLE|WS_CHILD|WS_BORDER,
				0,rect.bottom-20,rect.right-40,19,hWnd,(HMENU)108,GetInstanceHandle(),0);
			SendMessage(hCtrl,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),1);
			
			hCtrl=CreateWindow("BUTTON","&Add",WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON|BS_TEXT,
				0,rect.bottom-20,40,19,hWnd,(HMENU)109,GetInstanceHandle(),0);
			SendMessage(hCtrl,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),1);

			SetFocus(GetDlgItem(hWnd,108));

			
			RefreshDebugLogViewer(hWnd);
			
		}
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SIZE:
		if (wParam!=SIZE_MINIMIZED)
		{
			::SetWindowPos(GetDlgItem(hWnd,106),HWND_TOP,0,0,LOWORD(lParam)-320,15,SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOMOVE);
			
			::SetWindowPos(GetDlgItem(hWnd,107),HWND_TOP,0,0,LOWORD(lParam),HIWORD(lParam)-40,SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOMOVE);
			
			::SetWindowPos(GetDlgItem(hWnd,108),HWND_TOP,0,HIWORD(lParam)-20,LOWORD(lParam)-42,20,SWP_NOACTIVATE|SWP_NOZORDER);
			::SetWindowPos(GetDlgItem(hWnd,109),HWND_TOP,LOWORD(lParam)-40,HIWORD(lParam)-19,0,0,SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOSIZE);
		}
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case 101:
			StartDebugLogging();
			RefreshDebugLogViewer(hWnd);
			SetFocus(GetDlgItem(hWnd,108));
			break;
		case 102:
			StopDebugLogging();
			RefreshDebugLogViewer(hWnd);
			SetFocus(GetDlgItem(hWnd,108));
			break;
		case 103:
			SetFilePointer(hLogFile,0,NULL,FILE_BEGIN);
			SetEndOfFile(hLogFile);
			RefreshDebugLogViewer(hWnd);
			SetFocus(GetDlgItem(hWnd,108));
			break;
		case 104:
			RefreshDebugLogViewer(hWnd);
			SetFocus(GetDlgItem(hWnd,108));
			break;
		case 105:
			DestroyWindow(hWnd);
			break;
		case 108:
			if (HIWORD(wParam)==EN_SETFOCUS)
				SendDlgItemMessage(hWnd,108,EM_SETSEL,0,-1);
			if (HIWORD(wParam)==EN_CHANGE)
				EnableWindow(GetDlgItem(hWnd,109),SendDlgItemMessage(hWnd,108,WM_GETTEXTLENGTH,0,0)>0);
			break;
		case 109:
			{
				char szMessage[1000];
				GetDlgItemText(hWnd,108,szMessage,1000);
				DebugMessage(szMessage);
				RefreshDebugLogViewer(hWnd);
				SetDlgItemText(hWnd,108,"");
				SetFocus(GetDlgItem(hWnd,108));
				break;
			}
		}
		break;
	}
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

void AddDebugNote(HWND hWndParent)
{
	WNDCLASSEX wc;

    wc.cbSize=sizeof(WNDCLASSEX); 
    wc.style=CS_HREDRAW|CS_VREDRAW;
    wc.lpfnWndProc=AddDebugNoteWndProc; 
    wc.cbClsExtra=0;
    wc.cbWndExtra=0; 
    wc.hInstance=GetInstanceHandle();
    wc.hIcon=NULL;
    wc.hCursor=LoadCursor(NULL,IDC_ARROW); 
    wc.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName=0;
    wc.lpszClassName="ADDDEBUGNOTEWND"; 
    wc.hIconSm=NULL;
	RegisterClassEx(&wc);

	HWND hWnd=CreateWindow("ADDDEBUGNOTEWND","Add Debug Note",
		WS_VISIBLE|WS_OVERLAPPED|WS_CAPTION|WS_DLGFRAME|WS_SYSMENU,
		CW_USEDEFAULT,CW_USEDEFAULT,400,300,
		hWndParent,NULL,GetInstanceHandle(),NULL);
	ShowWindow(hWnd,SW_SHOW);
	UpdateWindow(hWnd);
	
	if (GetCurrentWinThread()==NULL)
	{
		MSG msg;
		while (GetMessage(&msg,NULL,0,0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	else
		GetCurrentWinThread()->ModalLoop();
}


void ViewOpenHandles(HWND hWndParent)
{
	WNDCLASSEX wc;

    wc.cbSize=sizeof(WNDCLASSEX); 
    wc.style=CS_HREDRAW|CS_VREDRAW;
    wc.lpfnWndProc=ViewOpenHandlesWndProc; 
    wc.cbClsExtra=0;
    wc.cbWndExtra=0; 
    wc.hInstance=GetInstanceHandle();
    wc.hIcon=NULL;
    wc.hCursor=LoadCursor(NULL,IDC_ARROW); 
    wc.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName=0;
    wc.lpszClassName="VIEWOPENHANDLESWND"; 
    wc.hIconSm=NULL;
	RegisterClassEx(&wc);

	HWND hWnd=CreateWindow("VIEWOPENHANDLESWND","View open handles",
		WS_VISIBLE|WS_OVERLAPPED|WS_CAPTION|WS_THICKFRAME|WS_SYSMENU,
		CW_USEDEFAULT,CW_USEDEFAULT,600,500,
		hWndParent,NULL,GetInstanceHandle(),NULL);
	ShowWindow(hWnd,SW_SHOW);
	UpdateWindow(hWnd);

	if (GetCurrentWinThread()==NULL)
	{
		MSG msg;
		while (GetMessage(&msg,NULL,0,0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	else
		GetCurrentWinThread()->ModalLoop();
}

void ViewDebugLog(HWND hWndParent)
{
	WNDCLASSEX wc;

    wc.cbSize=sizeof(WNDCLASSEX); 
    wc.style=CS_HREDRAW|CS_VREDRAW;
    wc.lpfnWndProc=ViewDebugLogWndProc; 
    wc.cbClsExtra=0;
    wc.cbWndExtra=0; 
    wc.hInstance=GetInstanceHandle();
    wc.hIcon=NULL;
    wc.hCursor=LoadCursor(NULL,IDC_ARROW); 
    wc.hbrBackground=(HBRUSH)(COLOR_BTNFACE+1);
    wc.lpszMenuName=0;
    wc.lpszClassName="VIEWDEBUGLOGWND"; 
    wc.hIconSm=NULL;
	RegisterClassEx(&wc);

	HWND hWnd=CreateWindow("VIEWDEBUGLOGWND","View Debug Log",
		WS_VISIBLE|WS_OVERLAPPED|WS_CAPTION|WS_THICKFRAME|WS_SYSMENU,
		CW_USEDEFAULT,CW_USEDEFAULT,600,550,
		hWndParent,NULL,GetInstanceHandle(),NULL);
	ShowWindow(hWnd,SW_SHOW);
	UpdateWindow(hWnd);

	if (GetCurrentWinThread()==NULL)
	{
		MSG msg;
		while (GetMessage(&msg,NULL,0,0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	else
		GetCurrentWinThread()->ModalLoop();
}

#endif

void Assert(BOOL bIsOK,int line,char* file)
{
	if (bIsOK)
		return;
#ifdef DEF_WINDOWS
	MSG msg;
	BOOL bQuit = PeekMessage(&msg, NULL, WM_QUIT, WM_QUIT, PM_REMOVE);
	_CrtDbgReport(_CRT_ASSERT, file, line, NULL, NULL);
	if (bQuit)
		PostQuitMessage((int)msg.wParam);
#else
	fprintf(stderr,"Assertation failed at %s line %d.",file,line);
#endif
}

#endif
