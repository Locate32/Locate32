/* Locate32 - Copyright (c) 1997-2010 Janne Huttunen */

#if !defined(MISC_INL)
#define MISC_INL

#if _MSC_VER >= 1000
#pragma once
#endif

//////////////////////////////////////////////////////////////////
// CDateTimeCtrlEx
//////////////////////////////////////////////////////////////////

inline int CDateTimeCtrlEx::GetValueFromText(LPCWSTR szText)
{
	return _wtoi(szText);
}

inline BOOL CDateTimeCtrlEx::GetMode() const
{
	return (m_dwFlags&ModeMask)==ModeRelative;
}

inline CDateTimeCtrlEx* CDateTimeCtrlEx::GetClass(HWND hWnd)
{
	return (CDateTimeCtrlEx*)::GetWindowLongPtr(hWnd,GWLP_USERDATA);
}



//////////////////////////////////////////////////////////////////
// CRegKey2
//////////////////////////////////////////////////////////////////



inline CRegKey2::CRegKey2()
:	CRegKey()
{
}

inline CRegKey2::CRegKey2(HKEY hKey)
:	CRegKey(hKey)
{
}

inline CRegKey2::CRegKey2(HKEY hKey,LPCSTR lpszSubKey,DWORD fStatus,LPSECURITY_ATTRIBUTES lpSecurityAttributes)
:	CRegKey()
{
	OpenKey(hKey,lpszSubKey,fStatus,lpSecurityAttributes);
}

inline CRegKey2::CRegKey2(HKEY hKey,LPCWSTR lpszSubKey,DWORD fStatus,LPSECURITY_ATTRIBUTES lpSecurityAttributes)
:	CRegKey()
{
	OpenKey(hKey,lpszSubKey,fStatus,lpSecurityAttributes);
}




#endif
