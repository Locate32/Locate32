////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////


#ifndef HFCDEBUG_H
#define HFCDEBUG_H



#ifdef _DEBUG
void MsgToText(DWORD msg,LPSTR text,DWORD maxlen);

void AddDebugMenuItems(HWND hWnd);
void AddDebugNote(HWND hWndParent);
void ViewDebugLog(HWND hWndParent);
void ViewOpenHandles(HWND hWndParent);
void Assert(BOOL bIsOK,int line,char* file);

#define ASSERT(_a)			Assert(_a,__LINE__,__FILE__);
#define ASSERT_VALID(_a)	Assert(_a!=NULL && _a!=(void*)-1,__LINE__,__FILE__);
#define ASSERT_RET(_a,_b) 	Assert(_a==_b,__LINE__,__FILE__),_b; // Check return value

#define _DEBUG_LOGGING

#define DEBUGVIRTUAL	virtual
#define DEBUGINLINE		inline
#define NDEBUGINLINE

#else

#define MsgToText(_a,_b,_c)
#define AddDebugMenuItems(_a)
#define AddDebugNote(_a)
#define ViewDebugLog(_a)
#define ViewOpenHandles(_a)

#define ASSERT(_a)
#define ASSERT_VALID(_a)
#define ASSERT_RET(_a,_b) 	_a

#define DEBUGVIRTUAL
#define DEBUGINLINE
#define NDEBUGINLINE	inline

#endif

enum DebugFlags { 
	DebugLogHandleOperations = 0x1
};

enum DebugHandleType {
	dhtUnknown=0,
	dhtMemoryBlock=1,
	dhtFile=2,
	dhtFileFind=3,
	dhtThread=4,
	dhtProcess=5,
	dhtEvent=6,
	dhtGdiObject=7,
	dhtWindow=8,
	dhtRegKey=9,
	dhtMenu=10,
	dhtMutex=11,
	dhtMisc=255
};


#ifdef _DEBUG_LOGGING
// Enable/disable debug loggins
void StartDebugLogging();
void StopDebugLogging();

// Enable/disable logging features, see DebugFlags
void DebugSetFlags(DebugFlags bDebugFlag,BOOL bRemove=FALSE);


// Will return the used debug log
LPCSTR GetDebugLoggingFile();

// Write message into the debug log
void DebugMessage(LPCSTR szMessage);
#ifdef DEF_WCHAR
void DebugMessage(LPCWSTR szMessage);
#endif

// Dump hex string into the debug log
void DebugHexDump(LPCSTR szDescription,BYTE* pData,SIZE_T datalen);
#ifdef DEF_WCHAR
void DebugHexDump(LPCWSTR szDescription,BYTE* pData,SIZE_T datalen);
#endif

// Write information about window message
void DebugWndMessage(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);

// Write message into the debug log, parameters are as with printf
void DebugFormatMessage(LPCSTR text,...);
#ifdef DEF_WCHAR
void DebugFormatMessage(LPCWSTR text,...);
#endif

// Marks handle as open, szInfo can be NULL
void DebugOpenHandle2(DebugHandleType bType,void* pValue,LPCSTR szInfo,int iLine,LPCSTR szFile);
#ifdef DEF_WCHAR
void DebugOpenHandle2(DebugHandleType bType,void* pValue,LPCWSTR szInfo,int iLine,LPCSTR szFile);
#endif

// Marks handle as closed, szInfo, iLine and szFile are only used in the debug log
// if bType==dthUnknown, the function will mark any handle mathching with pValue as closed
// if bType!=dthUnknown, bType and pValue need to be matched
// DebugCloseHandle2 will generate log message if pValue is marked as opened 
// handle, DebugCloseHandle3 does not do that
void DebugCloseHandle2(DebugHandleType bType,void* pValue,LPCSTR szInfo,int iLine,LPCSTR szFile);
void DebugCloseHandle3(DebugHandleType bType,void* pValue,LPCSTR szInfo,int iLine,LPCSTR szFile);
#ifdef DEF_WCHAR
void DebugCloseHandle2(DebugHandleType bType,void* pValue,LPCWSTR szInfo,int iLine,LPCSTR szFile);
void DebugCloseHandle3(DebugHandleType bType,void* pValue,LPCWSTR szInfo,int iLine,LPCSTR szFile);
#endif

// Dump information about open handles to debug log
void DebugLogOpenHandles();
			

// Functions will handle logging correctly, use these instead
BOOL WINAPI TerminateThread(HANDLE hThread,DWORD dwExitCode,BOOL bWaitUntilExited);
inline BOOL WINAPI _TerminateThread(HANDLE hThread,DWORD dwExitCode)
{
	TerminateThread(hThread,dwExitCode,FALSE);
}





/////////////////////////////////////////////
// Memory allocation with tracking

// Memory alloction with tracking, corresponds to DEBUG_NEW
void* operator new(size_t nSize, LPCSTR lpszFileName, int nLine);
#define DEBUG_NEW new(__FILE__, __LINE__)

void* operator new[](size_t nSize, LPCSTR lpszFileName, int nLine);
void operator delete(void* p, LPCSTR lpszFileName, int nLine);
void operator delete[](void* p, LPCSTR lpszFileName, int nLine);

#ifndef HFC_NOALLOCTRACKING
void* operator new(size_t nSize);
void* operator new[](size_t nSize);
void operator delete(void *p);
void operator delete[](void *p);
#endif


#if !defined(HFC_NODEBUGNEW) && !defined(HFC_NOALLOCTRACKING)
	#define new DEBUG_NEW
#endif

#else
// These makes possible to use these as normally like functions 
inline void StartDebugLogging() {}
inline void DebugMessage(LPCSTR msg) {}
inline void DebugHexDump(LPCSTR desc,BYTE* pData,SIZE_T datalen) {}
inline void DebugWndMessage(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam) {}
inline void DebugFormatMessage(LPCSTR text,...)  {}
inline void StopDebugLogging()  {}
#ifdef DEF_WCHAR
inline void DebugMessage(LPCWSTR msg) {}
inline void DebugHexDump(LPCWSTR desc,BYTE* pData,SIZE_T datalen) {}
inline void DebugNumMessage(LPCWSTR text,DWORD num) {}
inline void DebugFormatMessage(LPCWSTR text,...)  {}
#endif

inline void DebugSetFlags(DebugFlags bDebugFlag,BOOL bRemove=FALSE) {}
inline void DebugLogOpenHandles() {}
inline void DebugOpenHandle2(DebugHandleType bType,void* pValue,LPCSTR szInfo,int iLine,LPCSTR szFile) {}
inline void DebugCloseHandle2(DebugHandleType bType,void* pValue,LPCSTR szInfo,int iLine,LPCSTR szFile) {}
inline void DebugCloseHandle3(DebugHandleType bType,void* pValue,LPCSTR szInfo,int iLine,LPCSTR szFile) {}
#ifdef DEF_WCHAR
inline void DebugOpenHandle2(DebugHandleType bType,void* pValue,LPCWSTR szInfo,int iLine,LPCSTR szFile) {}
inline void DebugCloseHandle2(DebugHandleType bType,void* pValue,LPCWSTR szInfo,int iLine,LPCSTR szFile) {}
inline void DebugCloseHandle3(DebugHandleType bType,void* pValue,LPCWSTR szInfo,int iLine,LPCSTR szFile) {}
#endif

// Terminate thread
inline BOOL WINAPI TerminateThread(HANDLE hThread,DWORD dwExitCode,BOOL bWaitUntilExited)
{
	BOOL bRet=::TerminateThread(hThread,dwExitCode);
	WaitForSingleObject(hThread,1000);
	return bRet;
}
#define _TerminateThread	TerminateThread

#define DEBUG_NEW new

#endif




#define DebugOpenHandle(bType,pValue,szInfo) DebugOpenHandle2(bType,pValue,szInfo,__LINE__,__FILE__);
#define DebugCloseHandle(bType,pValue,szInfo) DebugCloseHandle2(bType,pValue,szInfo,__LINE__,__FILE__);


// Most common types
#define DebugOpenMemBlock(pValue)		DebugOpenHandle(dhtMemoryBlock,pValue,STRNULL)
#define DebugOpenFile(hFile)			DebugOpenHandle(dhtFile,hFile,STRNULL)
#define DebugOpenThread(hThread)		DebugOpenHandle(dhtThread,hThread,STRNULL)
#define DebugOpenGdiObject(hObject)		DebugOpenHandle(dhtGdiObject,hObject,STRNULL)
#define DebugOpenRegKey(hRegKey)		DebugOpenHandle(dhtRegKey,hRegKey,STRNULL)
#define DebugOpenEvent(hEvent)			DebugOpenHandle(dhtEvent,hEvent,STRNULL)
#define DebugOpenMutex(hMutex)			DebugOpenHandle(dhtMutex,hMutex,STRNULL)

#define DebugCloseMemBlock(pValue)		DebugCloseHandle(dhtMemoryBlock,pValue,STRNULL)
#define DebugCloseFile(hFile)			DebugCloseHandle(dhtFile,hFile,STRNULL)
#define DebugCloseThread(hThread)		DebugCloseHandle(dhtThread,hThread,STRNULL)
#define DebugCloseGdiObject(hObject)	DebugCloseHandle(dhtGdiObject,hObject,STRNULL)
#define DebugCloseRegKey(hRegKey)		DebugCloseHandle(dhtRegKey,hRegKey,STRNULL)
#define DebugCloseEvent(hEvent)			DebugCloseHandle(dhtEvent,hEvent,STRNULL)
#define DebugCloseMutex(hMutex)			DebugCloseHandle(dhtMutex,hMutex,STRNULL)


#endif

