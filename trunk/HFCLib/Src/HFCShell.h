////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////


#ifndef HFCSHELL_H
#define HFCSHELL_H

#ifdef WIN32

namespace ShellFunctions {
	// Str manipulation
	BOOL StrRetToStr(STRRET& strret,LPITEMIDLIST lpiil,LPSTR szString,DWORD cchBufferLen);
	void StrRetToStr(STRRET& strret,LPITEMIDLIST lpiil,CString& sString);
	LPSTR StrRetToPtr(STRRET& strret,LPITEMIDLIST lpiil);
#ifdef DEF_WCHAR
	BOOL StrRetToStr(STRRET& strret,LPITEMIDLIST lpiil,LPWSTR szString,DWORD cchBufferLen);
	void StrRetToStr(STRRET& strret,LPITEMIDLIST lpiil,CStringW& sString);
	LPWSTR StrRetToPtrW(STRRET& strret,LPITEMIDLIST lpiil);
#endif

	// ID lists manipulation
	LPITEMIDLIST GetIDList(LPCSTR lpszFileName);
	LPITEMIDLIST GetIDListForParent(LPCSTR lpszFileName);
	DWORD GetIDListSize(LPITEMIDLIST lpil);
#ifdef DEF_WCHAR
	LPITEMIDLIST GetIDList(LPCWSTR lpszFileName);
	LPITEMIDLIST GetIDListForParent(LPCWSTR lpszFileName);
#endif
	

	// Shortcuts
	HRESULT CreateShortcut(LPCSTR pszShortcutFile,LPCSTR pszLink,LPCSTR pszDesc=NULL,LPCSTR pszParams=NULL);
	HRESULT ResolveShortcut(HWND hWnd,LPCSTR pszShortcutFile,LPSTR pszPath=NULL);
	HRESULT GetShortcutTarget(LPCSTR pszShortcutFile,LPSTR pszTarget,DWORD nBufSize);
#ifdef DEF_WCHAR
	HRESULT CreateShortcut(LPCWSTR pszShortcutFile,LPCWSTR pszLink,LPCWSTR pszDesc,LPCWSTR pszParams);
	HRESULT ResolveShortcut(HWND hWnd,LPCWSTR pszShortcutFile,LPWSTR pszPath);
	HRESULT GetShortcutTarget(LPCWSTR pszShortcutFile,LPWSTR pszTarget,DWORD nBufSize);
#endif


	// Takes registry key like HKEY_CLASSES_ROOT/txtfile/shell/open and 
	// executes its command (%1 is replaced with szFile)
	BOOL RunRegistryCommand(HKEY hKey,LPCSTR szFile);
#ifdef DEF_WCHAR
	BOOL RunRegistryCommand(HKEY hKey,LPCWSTR szFile);
#endif


	// Unicode/ANSI wrappers
	DWORD_PTR GetFileInfo(LPCSTR pszPath,DWORD dwFileAttributes,SHFILEINFO *psfi,UINT uFlags);
	DWORD_PTR GetFileInfo(LPITEMIDLIST piil,DWORD dwFileAttributes,SHFILEINFO *psfi,UINT uFlags);
	int FileOperation(LPSHFILEOPSTRUCT lpFileOp);
	int ShellExecute(HWND hwnd,LPCSTR lpOperation,LPCSTR lpFile,LPCSTR lpParameters,LPCSTR lpDirectory,INT nShowCmd);
	BOOL ShellExecuteEx(LPSHELLEXECUTEINFO lpExecInfo);

#ifdef DEF_WCHAR
	DWORD_PTR GetFileInfo(LPCWSTR pszPath,DWORD dwFileAttributes,SHFILEINFOW *psfi,UINT uFlags);
	DWORD_PTR GetFileInfo(LPITEMIDLIST piil,DWORD dwFileAttributes,SHFILEINFOW *psfi,UINT uFlags);
	int FileOperation(LPSHFILEOPSTRUCTW lpFileOp);
	int ShellExecute(HWND hwnd,LPCWSTR lpOperation,LPCWSTR lpFile,LPCWSTR lpParameters,LPCWSTR lpDirectory,INT nShowCmd);
	BOOL ShellExecuteEx(LPSHELLEXECUTEINFOW lpExecInfo);
#endif





};



inline DWORD_PTR ShellFunctions::GetFileInfo(LPCSTR pszPath,DWORD dwFileAttributes,SHFILEINFO *psfi,UINT uFlags)
{
	return ::SHGetFileInfo(pszPath,dwFileAttributes,psfi,sizeof(SHFILEINFO),uFlags);
}

inline DWORD_PTR ShellFunctions::GetFileInfo(LPITEMIDLIST piil,DWORD dwFileAttributes,SHFILEINFO *psfi,UINT uFlags)
{
	return ::SHGetFileInfo((LPCSTR)piil,dwFileAttributes,psfi,sizeof(SHFILEINFO),uFlags|SHGFI_PIDL);
}

inline int ShellFunctions::FileOperation(LPSHFILEOPSTRUCT lpFileOp)
{
	return ::SHFileOperation(lpFileOp);
}

inline int ShellFunctions::ShellExecute(HWND hwnd,LPCSTR lpOperation,LPCSTR lpFile,LPCSTR lpParameters,LPCSTR lpDirectory,INT nShowCmd)
{
	if (lpOperation!=NULL)
	{
		if (lpOperation[0]=='\0')
			lpOperation=NULL;
	}
	return (int)::ShellExecute(hwnd,lpOperation,lpFile,lpParameters,lpDirectory,nShowCmd);
}

inline BOOL ShellFunctions::ShellExecuteEx(LPSHELLEXECUTEINFO lpExecInfo)
{
	return ::ShellExecuteExA(lpExecInfo);
}



#endif
#endif