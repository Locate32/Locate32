////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////


#ifndef HFCFUNC_H
#define HFCFUNC_H

///////////////////////////////////////////////
// Global functions

//HFC Library functions

DWORD GetHFCLibVerMS(void);
DWORD GetHFCLibVerLS(void);
ULONGLONG GetHFCLibVer(void);
LPCSTR GetHFCLibStr(void);
DWORD GetHFCLibFlags(void);

//Error handling
HFCERROR SetHFCError(HFCERROR nErr);
HFCERROR SetHFCError2(HFCERROR nErr,int line,char* file);
#ifdef _DEBUG
#define SetHFCError(_a)		SetHFCError2(_a,__LINE__,__FILE__)
#endif

HFCERROR GetHFCError(void);
HFCERRCALLBACK SetHFCErrorCallback(HFCERRCALLBACK pfCallbackFunc,DWORD dwData);
LPCSTR GetStdErrorStr(HFCERROR nError);
HFCERROR ExceptionErrorFunc(HFCERROR nError,DWORD_PTR dwData);
void SysErrorMessage(CString& str,DWORD dwSystemError);
HFCERROR StdHFCErrorCallbackStdout(HFCERROR nError,DWORD_PTR dwData);
HFCERROR StdHFCErrorCallbackStderr(HFCERROR nError,DWORD_PTR dwData);
HFCERROR StdHFCErrorCallbackMsgBox(HFCERROR nError,DWORD_PTR dwData);


//Only tests if HFC Library works correctly, returning nonzero if OK
inline BOOL InitializeHFC(void)
{
#ifdef WIN32
	if ((GetHFCLibFlags()&HFCFLAG_WIN32)==0)
		return FALSE;
#else
	if ((GetHFCLibFlags()&HFCFLAG_WIN32)==HFCFLAG_WIN32)
		return FALSE;
#endif
#ifdef CONSOLE
	if ((GetHFCLibFlags()&HFCFLAG_CONSOLE)==0)
		return FALSE;
#else
	if ((GetHFCLibFlags()&HFCFLAG_CONSOLE)==HFCFLAG_CONSOLE)
		return FALSE;
#endif
#ifdef DEF_WCHAR
	if ((GetHFCLibFlags()&HFCFLAG_WCHAR)==0)
		return FALSE;
#else
	if ((GetHFCLibFlags()&HFCFLAG_WCHAR)==HFCFLAG_WCHAR)
		return FALSE;
#endif
#ifdef DEF_RESOURCES
	if ((GetHFCLibFlags()&HFCFLAG_RESOURCES)==0)
		return FALSE;
#else
	if ((GetHFCLibFlags()&HFCFLAG_RESOURCES)==HFCFLAG_RESOURCES)
		return FALSE;
#endif
#ifdef DEF_WINDOWS
	if ((GetHFCLibFlags()&HFCFLAG_WINDOWS)==0)
		return FALSE;
#else
	if ((GetHFCLibFlags()&HFCFLAG_WINDOWS)==HFCFLAG_WINDOWS)
		return FALSE;
#endif
#ifdef DEF_COM
	if ((GetHFCLibFlags()&HFCFLAG_COM)==0)
		return FALSE;
#else
	if ((GetHFCLibFlags()&HFCFLAG_COM)==HFCFLAG_COM)
		return FALSE;
#endif
	return TRUE;
}


// System information
BOOL SystemInfo(LPSYSTEMINFO);
DWORD GetSystemFeaturesFlag();
#ifdef WIN32
COLORREF GetSystemColor(LPCSTR szKey);
DWORD GetFileVersion(LPCSTR szFile,DWORD* dwFileVersionLo=NULL);
#endif

// Process and thread functions
#ifdef DEF_APP
CWinApp* GetApp();
CTargetWnd* GetMainWnd();
CWinThread* GetCurrentWinThread();
CWinThread* GetWinThread(DWORD nThreadID);
HINSTANCE GetInstanceHandle();
#endif

// Window and control functions
#ifdef DEF_WINDOWS
BOOL RegisterWndClass(LPCTSTR lpszClassName,UINT nClassStyle=CS_HREDRAW|CS_VREDRAW,HCURSOR hCursor=NULL,HBRUSH hbrBackground=0,HICON hIcon=0,HICON hSmallIcon=NULL);
BOOL RegisterMDIChildClass(LPCTSTR lpszClassName,UINT nClassStyle=CS_HREDRAW|CS_VREDRAW,HCURSOR hCursor=NULL,HBRUSH hbrBackground=0,HICON hIcon=0,HICON hSmallIcon=NULL);
BYTE InitHFCControls();
BOOL WaitForWindow(CWnd* pWnd,DWORD dwMilliseconds=INFINITE);
BOOL ForceForegroundAndFocus(HWND hWindow);
int ReportSystemError(HWND hWnd,LPCSTR szTitle=NULL,DWORD dwError=DWORD(-1),DWORD dwExtra=0,LPCSTR szPrefix=NULL);
HBITMAP ScaleImage(HBITMAP hBitmap,int nMaxX,int nMaxY);
#endif

// Resource functions
#ifdef DEF_RESOURCES
inline CString LoadString(UINT nID)
{
	return CString((UINT)nID);
}
inline CString LoadString(UINT nID,TypeOfResourceHandle bType)
{
	return CString((UINT)nID,bType);
}

#ifdef DEF_WCHAR
inline CStringW LoadStringW(UINT nID)
{
	return CStringW((UINT)nID);
}
inline CStringW LoadStringW(UINT nID,TypeOfResourceHandle bType)
{
	return CStringW((UINT)nID,bType);
}
#endif

inline HANDLE LoadImage(LPCSTR lpszName,UINT uType,int cxDesired,int cyDesired,UINT fuLoad)
{
	return (HANDLE)::LoadImage(GetCommonResourceHandle(),lpszName,uType,cxDesired,cyDesired,fuLoad);
}

#ifdef DEF_WCHAR
inline HANDLE LoadImage(LPCWSTR lpszName,UINT uType,int cxDesired,int cyDesired,UINT fuLoad)
{
	if (IsUnicodeSystem())
		return (HANDLE)::LoadImageW(GetCommonResourceHandle(),lpszName,uType,cxDesired,cyDesired,fuLoad);
	else
		return (HANDLE)::LoadImageA(GetCommonResourceHandle(),W2A(lpszName),uType,cxDesired,cyDesired,fuLoad);
}
#endif

inline HANDLE LoadImage(UINT nID,UINT uType,int cxDesired,int cyDesired,UINT fuLoad)
{
	return (HANDLE)::LoadImage(GetCommonResourceHandle(),MAKEINTRESOURCE(nID),uType,cxDesired,cyDesired,fuLoad);
}
inline HICON LoadIcon(LPCTSTR lpIconName)
{
	return (HICON)::LoadIcon(GetCommonResourceHandle(),lpIconName);
}
inline HICON LoadIcon(UINT nIconID)
{
	return (HICON)::LoadIcon(GetCommonResourceHandle(),MAKEINTRESOURCE(nIconID));
}
inline HBITMAP LoadBitmap(LPCTSTR lpBitmapName)
{
	return (HBITMAP)::LoadBitmap(GetCommonResourceHandle(),lpBitmapName);
}
inline HBITMAP LoadBitmap(UINT nBitmapID)
{
	return (HBITMAP)::LoadBitmap(GetCommonResourceHandle(),MAKEINTRESOURCE(nBitmapID));
}
inline HMENU LoadMenu(LPCTSTR lpMenuName)
{
	return (HMENU)::LoadMenu(GetLanguageSpecificResourceHandle(),lpMenuName);
}
inline HMENU LoadMenu(LPCTSTR lpMenuName,TypeOfResourceHandle bType)
{
	return (HMENU)::LoadMenu(GetResourceHandle(bType),lpMenuName);
}
inline HMENU LoadMenu(UINT nMenuID)
{
	return (HMENU)::LoadMenu(GetLanguageSpecificResourceHandle(),MAKEINTRESOURCE(nMenuID));
}
inline HMENU LoadMenu(UINT nMenuID,TypeOfResourceHandle bType)
{
	return (HMENU)::LoadMenu(GetResourceHandle(bType),MAKEINTRESOURCE(nMenuID));
}
inline HCURSOR LoadCursor(LPCTSTR lpCursorName)
{
	return (HCURSOR)::LoadMenu(GetCommonResourceHandle(),lpCursorName);
}
inline HCURSOR LoadCursor(UINT nCursorID)
{
	return (HCURSOR)::LoadMenu(GetCommonResourceHandle(),MAKEINTRESOURCE(nCursorID));
}
inline int LoadString(UINT uID,LPSTR lpBuffer,int nBufferMax)
{
	return (int)::LoadStringA(GetLanguageSpecificResourceHandle(),uID,lpBuffer,nBufferMax);
}
inline int LoadString(UINT uID,LPSTR lpBuffer,int nBufferMax,TypeOfResourceHandle bType)
{
	return (int)::LoadStringA(GetResourceHandle(bType),uID,lpBuffer,nBufferMax);
}

#ifdef DEF_WCHAR
int LoadString(UINT uID,LPWSTR lpBuffer,int nBufferMax,HINSTANCE hInstance);
int LoadString(UINT uID,LPWSTR lpBuffer,int nBufferMax,TypeOfResourceHandle bType);
int LoadString(UINT uID,LPWSTR lpBuffer,int nBufferMax);
#endif





/* Function for retrieving text from text resource, defined in System.cpp
Parameters:

hInstance				Instance to module
szBlock:				Name of version block
szText:					Pointer to a string buffer that receives the data
dwMaxTextLen:			Maximum characters in szText ('\0' is included)
*/
BOOL GetVersionText(LPCSTR szModulePath,LPCSTR szBlock,LPSTR szText,DWORD dwMaxTextLen);
#ifdef DEF_WCHAR
BOOL GetVersionText(LPCWSTR szModulePath,LPCSTR szBlock,LPWSTR szText,DWORD dwMaxTextLen);
#endif

#endif



// Variable manipulation
UINT BinToInt(LPCSTR);
void IntToBin(UINT,LPSTR,UINT);

// Inline functions

inline void swapu(unsigned int& a,unsigned int& b)
{
	unsigned int tmp=a;
	a=b;
	b=tmp;
}

inline void swapf(double& a,double& b)
{
	double tmp=a;
	a=b;
	b=tmp;
}

template <class type1 ,class type2> 
inline void swap(type1& a,type2& b)
{
	type1 tmp=a;
	a=b;
	b=tmp;
}

// Simultaneusly access data helpers, return 0 is success, 
// 1 if timeout, 2 if abandomed, otherwise 3
inline DWORD WaitForMutex(HANDLE hMutex,DWORD dwTimeOut=5000)
{
	DWORD dwWaitResult=WaitForSingleObject(hMutex,dwTimeOut);

	switch (dwWaitResult) 
    {
	case WAIT_OBJECT_0: 
		return 0;
	case WAIT_TIMEOUT:
		return 1;
	case WAIT_ABANDONED:
		return 2;
	}
	return 3;
}

#ifdef DEF_WINDOWS
inline LONG GetWindowStyle(HWND hWnd)
{
	return GetWindowLong(hWnd,GWL_STYLE);
}
#endif


#endif
