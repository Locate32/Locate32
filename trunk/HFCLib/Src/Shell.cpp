////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////

#include "HFCLib.h"


#ifdef WIN32
BOOL ShellFunctions::StrRetToStr(STRRET& strret,LPITEMIDLIST lpiil,LPSTR szString,DWORD cchBufferLen)
{
	switch (strret.uType)
	{
	case STRRET_OFFSET:
		return strcpy_s(szString,cchBufferLen,(LPSTR)((LPBYTE)lpiil+strret.uOffset))==0;
	case STRRET_CSTR:
		return strcpy_s(szString,cchBufferLen,strret.cStr)==0;
	case STRRET_WSTR:
		BOOL bRet=WideCharToMultiByte(CP_ACP,0,strret.pOleStr,-1,szString,cchBufferLen,NULL,NULL)>0;
		if (!bRet)
			bRet=*strret.pOleStr=='\0';
		CoTaskMemFree(strret.pOleStr);
		return bRet;
	}
	return FALSE;
}

void ShellFunctions::StrRetToStr(STRRET& strret,LPITEMIDLIST lpiil,CString& sString)
{
	switch (strret.uType)
	{
	case STRRET_OFFSET:
		sString.Copy((LPSTR)((LPBYTE)lpiil+strret.uOffset));
		break;
	case STRRET_CSTR:
		sString.Copy(strret.cStr);
		break;
	case STRRET_WSTR:
		sString.Copy(strret.pOleStr);
		CoTaskMemFree(strret.pOleStr);
		break;
	}	
}

LPSTR ShellFunctions::StrRetToPtr(STRRET& strret,LPITEMIDLIST lpiil)
{
	LPSTR pRet=NULL;
	switch (strret.uType)
	{
	case STRRET_OFFSET:
		pRet=alloccopy((LPSTR)((LPBYTE)lpiil+strret.uOffset));
		break;
	case STRRET_CSTR:
		pRet=alloccopy(strret.cStr);
		break;
	case STRRET_WSTR:
		pRet=alloccopyWtoA(strret.pOleStr);
		CoTaskMemFree(strret.pOleStr);
		break;
	}	
	return pRet;
}

#ifdef DEF_WCHAR
BOOL ShellFunctions::StrRetToStr(STRRET& strret,LPITEMIDLIST lpiil,LPWSTR szString,DWORD cchBufferLen)
{
	switch (strret.uType)
	{
	case STRRET_OFFSET:
		if (MultiByteToWideChar(CP_ACP,0,(LPSTR)((LPBYTE)lpiil+strret.uOffset),-1,szString,cchBufferLen)>0)
			return TRUE;
		return *(LPSTR)((LPBYTE)lpiil+strret.uOffset)=='\0';
	case STRRET_CSTR:
		if (MultiByteToWideChar(CP_ACP,0,strret.cStr,-1,szString,cchBufferLen)>0)
			return TRUE;
		return *strret.cStr=='\0';
	case STRRET_WSTR:
		BOOL bRet=wcscpy_s(szString,cchBufferLen,strret.pOleStr)==0;
		CoTaskMemFree(strret.pOleStr);
		return bRet;
	}
	return FALSE;
}

void ShellFunctions::StrRetToStr(STRRET& strret,LPITEMIDLIST lpiil,CStringW& sString)
{
	switch (strret.uType)
	{
	case STRRET_OFFSET:
		sString.Copy((LPSTR)((LPBYTE)lpiil+strret.uOffset));
		break;
	case STRRET_CSTR:
		sString.Copy(strret.cStr);
		break;
	case STRRET_WSTR:
		sString.Copy(strret.pOleStr);
		CoTaskMemFree(strret.pOleStr);
		break;
	}	
}

LPWSTR ShellFunctions::StrRetToPtrW(STRRET& strret,LPITEMIDLIST lpiil)
{
	LPWSTR pRet=NULL;
	switch (strret.uType)
	{
	case STRRET_OFFSET:
		pRet=alloccopyAtoW((LPSTR)((LPBYTE)lpiil+strret.uOffset));
		break;
	case STRRET_CSTR:
		pRet=alloccopyAtoW(strret.cStr);
		break;
	case STRRET_WSTR:
		pRet=alloccopy(strret.pOleStr);
		CoTaskMemFree(strret.pOleStr);
		break;
	}	
	return pRet;
}
#endif

LPITEMIDLIST ShellFunctions::GetIDList(LPCSTR lpszFileName)
{
	LPITEMIDLIST pidl=NULL;
	IShellFolder *pDesktop;
	if (FAILED(SHGetDesktopFolder(&pDesktop)))
		return NULL;

	
	if (FAILED(pDesktop->ParseDisplayName(NULL,NULL,(LPOLESTR)(LPCWSTR)A2W(lpszFileName),NULL,&pidl,NULL)))
	{
		pDesktop->Release();
		return NULL;
	}
	return pidl;
}

#ifdef DEF_WCHAR
LPITEMIDLIST ShellFunctions::GetIDList(LPCWSTR lpszFileName)
{
	LPITEMIDLIST pidl=NULL;
	IShellFolder *pDesktop;
	if (FAILED(SHGetDesktopFolder(&pDesktop)))
		return NULL;

	HRESULT hRes=pDesktop->ParseDisplayName(NULL,NULL,(LPOLESTR)lpszFileName,NULL,&pidl,NULL);
	pDesktop->Release();
	
	return FAILED(hRes)?NULL:pidl;
}
#endif

LPITEMIDLIST ShellFunctions::GetIDListForParent(LPCSTR lpszFileName)
{
		int temp=LastCharIndex(lpszFileName,'\\');
	LPCWSTR szFolder;
	if (temp==-1)
		szFolder=alloccopyAtoW(lpszFileName);
	else
		szFolder=alloccopyAtoW(lpszFileName,temp+1);
	
	LPITEMIDLIST pidl=NULL;
	IShellFolder *pDesktop;
	if (FAILED(SHGetDesktopFolder(&pDesktop)))
	{
		delete[] szFolder;
		return NULL;
	}
	
	if (FAILED(pDesktop->ParseDisplayName(NULL,NULL,(LPOLESTR)szFolder,NULL,&pidl,NULL)))
	{
		pDesktop->Release();
		delete[] szFolder;
		return NULL;
	}
	return pidl;
}

#ifdef DEF_WCHAR
LPITEMIDLIST ShellFunctions::GetIDListForParent(LPCWSTR lpszFileName)
{
	int temp=LastCharIndex(lpszFileName,L'\\');
	LPCWSTR szFolder;
	if (temp==-1)
		szFolder=alloccopy(lpszFileName);
	else
		szFolder=alloccopy(lpszFileName,temp+1);
	
	LPITEMIDLIST pidl=NULL;
	IShellFolder *pDesktop;
	if (FAILED(SHGetDesktopFolder(&pDesktop)))
	{
		delete[] szFolder;
		return NULL;
	}
	
	HRESULT hRes=pDesktop->ParseDisplayName(NULL,NULL,(LPOLESTR)szFolder,NULL,&pidl,NULL);
	pDesktop->Release();
	
	return FAILED(hRes)?NULL:pidl;
}
#endif

DWORD ShellFunctions::GetIDListSize(LPITEMIDLIST lpil)
{
	DWORD nSize=0;
	while(*((WORD*)((LPCSTR)lpil+nSize))!=0)
		nSize+=*((WORD*)((LPCSTR)lpil+nSize));
	return nSize+2;
}


HRESULT ShellFunctions::CreateShortcut(LPCSTR pszShortcutFile,LPCSTR pszLink,LPCSTR pszDesc,LPCSTR pszParams)
{
	HRESULT hres;
	IShellLink* psl;
	hres=CoCreateInstance(CLSID_ShellLink,NULL,CLSCTX_INPROC_SERVER,IID_IShellLink,(void**)&psl);
	if (SUCCEEDED(hres))
	{
		IPersistFile* ppf;
		hres=psl->QueryInterface(IID_IPersistFile,(void**)&ppf);
		if (SUCCEEDED(hres))
		{
			hres=psl->SetPath(pszLink);
			if (SUCCEEDED(hres))
			{
				LONG_PTR nIndex=LastCharIndex(pszLink,'\\');
				if (nIndex>=0)
				{
					char szWDir[MAX_PATH];
					MemCopy(szWDir,pszLink,nIndex);
					szWDir[nIndex]='\0';
					psl->SetWorkingDirectory(szWDir);
				}

				if (pszDesc!=NULL)
					psl->SetDescription(pszDesc);
				if (pszParams!=NULL)
					psl->SetArguments(pszParams);
				
				WCHAR wsz[MAX_PATH];
				MultiByteToWideChar(CP_ACP,0,pszShortcutFile,-1,wsz,MAX_PATH);
				hres=ppf->Save(wsz,TRUE);    
			}
			ppf->Release(); 
		}
		psl->Release();
	}
	return hres;
}

HRESULT ShellFunctions::ResolveShortcut(HWND hWnd,LPCSTR pszShortcutFile,LPSTR pszPath)
{
	HRESULT hres;  
	IShellLink* psl;
	pszPath[0]='\0';

	hres=CoCreateInstance(CLSID_ShellLink,NULL,CLSCTX_INPROC_SERVER,IID_IShellLink,(void**)&psl);
	if (SUCCEEDED(hres))
	{
		IPersistFile* ppf;
		hres=psl->QueryInterface(IID_IPersistFile,(void**)&ppf);
		if (SUCCEEDED(hres))
		{
			WCHAR wsz[MAX_PATH];
			MultiByteToWideChar(CP_ACP,0,pszShortcutFile,-1,wsz,MAX_PATH);
			hres=ppf->Load(wsz,STGM_READ);
			if (SUCCEEDED(hres))
			{
				hres=psl->Resolve(hWnd,SLR_ANY_MATCH);
				if (pszPath!=NULL && SUCCEEDED(hres))
					hres=psl->GetPath(pszPath,MAX_PATH,NULL,0);
			}
			ppf->Release();
		}
		psl->Release();  
	}
	return hres;
}

HRESULT ShellFunctions::GetShortcutTarget(LPCSTR pszShortcutFile,LPSTR pszTarget,DWORD nBufSize)
{
	HRESULT hres;  
	IShellLink* psl;
	
	hres=CoCreateInstance(CLSID_ShellLink,NULL,CLSCTX_INPROC_SERVER,IID_IShellLink,(void**)&psl);
	if (SUCCEEDED(hres))
	{
		IPersistFile* ppf;
		hres=psl->QueryInterface(IID_IPersistFile,(void**)&ppf);
		if (SUCCEEDED(hres))
		{
			WCHAR wsz[MAX_PATH];
			MultiByteToWideChar(CP_ACP,0,pszShortcutFile,-1,wsz,MAX_PATH);
			hres=ppf->Load(wsz,STGM_READ);
			if (SUCCEEDED(hres))
				hres=psl->GetPath(pszTarget,nBufSize,NULL,0);
			ppf->Release();
		}
		psl->Release();  
	}
	return hres;
}

#ifdef DEF_WCHAR


HRESULT ShellFunctions::CreateShortcut(LPCWSTR pszShortcutFile,LPCWSTR pszLink,LPCWSTR pszDesc,LPCWSTR pszParams)
{
	HRESULT hres;
	
	if (IsUnicodeSystem())
	{
		IShellLinkW* psl;
		hres=CoCreateInstance(CLSID_ShellLink,NULL,CLSCTX_INPROC_SERVER,IID_IShellLinkW,(void**)&psl);
		if (SUCCEEDED(hres))
		{
			IPersistFile* ppf;
			hres=psl->QueryInterface(IID_IPersistFile,(void**)&ppf);
			if (SUCCEEDED(hres))
			{
				hres=psl->SetPath(pszLink);
				if (SUCCEEDED(hres))
				{
					
					int nIndex=LastCharIndex(pszLink,L'\\');
					if (nIndex>=0)
					{
						WCHAR szWDir[MAX_PATH];
						MemCopyW(szWDir,pszLink,nIndex);
						szWDir[nIndex]='\0';
						psl->SetWorkingDirectory(szWDir);
					}
										
					if (pszDesc!=NULL)
						psl->SetDescription(pszDesc);
					if (pszParams!=NULL)
						psl->SetArguments(pszParams);
					
					hres=ppf->Save(pszShortcutFile,TRUE);    
				}
				ppf->Release(); 
			}
			psl->Release();
		}
	}
	else
	{
		IShellLink* psl;
		hres=CoCreateInstance(CLSID_ShellLink,NULL,CLSCTX_INPROC_SERVER,IID_IShellLink,(void**)&psl);
		if (SUCCEEDED(hres))
		{
			IPersistFile* ppf;
			hres=psl->QueryInterface(IID_IPersistFile,(void**)&ppf);
			if (SUCCEEDED(hres))
			{
				hres=psl->SetPath(W2A(pszLink));
				if (SUCCEEDED(hres))
				{
					LONG_PTR nIndex=LastCharIndex(pszLink,L'\\');
					if (nIndex>=0)
					{
						char szWDir[MAX_PATH];
						WideCharToMultiByte(CP_ACP,0,pszLink,(int)nIndex,szWDir,MAX_PATH,0,0);
						szWDir[nIndex]='\0';
						psl->SetWorkingDirectory(szWDir);
					}

					if (pszDesc!=NULL)
						psl->SetDescription(W2A(pszDesc));
					if (pszParams!=NULL)
						psl->SetArguments(W2A(pszParams));

					hres=ppf->Save(pszShortcutFile,TRUE);    
				}
				ppf->Release(); 
			}
			psl->Release();
		}
	}
	return hres;
}

HRESULT ShellFunctions::ResolveShortcut(HWND hWnd,LPCWSTR pszShortcutFile,LPWSTR pszPath)
{
	HRESULT hres;  
	pszPath[0]='\0';

	if (IsUnicodeSystem())
	{
		IShellLinkW* psl;
		WIN32_FIND_DATAW wfd;
		
		hres=CoCreateInstance(CLSID_ShellLink,NULL,CLSCTX_INPROC_SERVER,IID_IShellLinkW,(void**)&psl);
		if (SUCCEEDED(hres))
		{
			IPersistFile* ppf;
			hres=psl->QueryInterface(IID_IPersistFile,(void**)&ppf);
			if (SUCCEEDED(hres))
			{
				hres=ppf->Load(pszShortcutFile,STGM_READ);
				if (SUCCEEDED(hres))
				{
					hres=psl->Resolve(hWnd,SLR_ANY_MATCH);
					if (pszPath!=NULL && SUCCEEDED(hres))
						hres=psl->GetPath(pszPath,MAX_PATH,(WIN32_FIND_DATAW*)&wfd,0);
				}
				ppf->Release();
			}
			psl->Release();  
		}
	}
	else
	{
		IShellLink* psl;
		WIN32_FIND_DATA wfd;
		
		hres=CoCreateInstance(CLSID_ShellLink,NULL,CLSCTX_INPROC_SERVER,IID_IShellLink,(void**)&psl);
		if (SUCCEEDED(hres))
		{
			IPersistFile* ppf;
			hres=psl->QueryInterface(IID_IPersistFile,(void**)&ppf);
			if (SUCCEEDED(hres))
			{
				hres=ppf->Load(pszShortcutFile,STGM_READ);
				if (SUCCEEDED(hres))
				{
					hres=psl->Resolve(hWnd,SLR_ANY_MATCH);
					if (pszPath!=NULL && SUCCEEDED(hres))
					{
						char szPathA[MAX_PATH];
						hres=psl->GetPath(szPathA,MAX_PATH,(WIN32_FIND_DATA*)&wfd,0);
						if (SUCCEEDED(hres))
							MultiByteToWideChar(CP_ACP,0,szPathA,-1,pszPath,MAX_PATH);
					}
				}
				ppf->Release();
			}
			psl->Release();  
		}
	}
	
	return hres;
}

HRESULT ShellFunctions::GetShortcutTarget(LPCWSTR pszShortcutFile,LPWSTR pszTarget,DWORD nBufSize)
{
	HRESULT hres;
	if (IsUnicodeSystem())
	{
		IShellLinkW* psl;
		WIN32_FIND_DATAW wfd;

		hres=CoCreateInstance(CLSID_ShellLink,NULL,CLSCTX_INPROC_SERVER,IID_IShellLinkW,(void**)&psl);
		if (SUCCEEDED(hres))
		{
			IPersistFile* ppf;
			hres=psl->QueryInterface(IID_IPersistFile,(void**)&ppf);
			if (SUCCEEDED(hres))
			{
				hres=ppf->Load(pszShortcutFile,STGM_READ);
				if (SUCCEEDED(hres))
					hres=psl->GetPath(pszTarget,nBufSize,(WIN32_FIND_DATAW*)&wfd,0);
				ppf->Release();
			}
			psl->Release();  
		}
	}
	else
	{
		IShellLinkA* psl;
		WIN32_FIND_DATA wfd;

		hres=CoCreateInstance(CLSID_ShellLink,NULL,CLSCTX_INPROC_SERVER,IID_IShellLinkA,(void**)&psl);
		if (SUCCEEDED(hres))
		{
			IPersistFile* ppf;
			hres=psl->QueryInterface(IID_IPersistFile,(void**)&ppf);
			if (SUCCEEDED(hres))
			{
				char* pTargetTmp=new char[nBufSize+2];
				hres=ppf->Load(pszShortcutFile,STGM_READ);
				if (SUCCEEDED(hres))
					hres=psl->GetPath(pTargetTmp,nBufSize,(WIN32_FIND_DATA*)&wfd,0);
				MultiByteToWideChar(CP_ACP,0,pTargetTmp,-1,pszTarget,nBufSize);
				delete[] pTargetTmp;
				ppf->Release();
			}
			psl->Release();  
		}
	}
	return hres;
}
#endif


BOOL ShellFunctions::RunRegistryCommand(HKEY hKey,LPCSTR szFile)
{
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	CRegKey CommandKey;
	CString ExecuteStr;
	CString CommandLine;
	int i;

	if (CommandKey.OpenKey(hKey,"command",CRegKey::openExist|CRegKey::samAll)!=ERROR_SUCCESS)
		return FALSE;
	if (CommandKey.QueryValue(szEmpty,ExecuteStr)<2)
		return FALSE;
	i=ExecuteStr.FindFirst('%');
	while (i!=-1)
	{
		if (ExecuteStr[i+1]=='1')
		{
			CommandLine.Copy(ExecuteStr,i);
			CommandLine<<szFile;
			CommandLine<<((LPCSTR)ExecuteStr+i+2);
			break;
		}
		i=ExecuteStr.FindNext('%',i);
	}
	if (i==-1)
		CommandLine=ExecuteStr;
	if (!ExpandEnvironmentStrings(CommandLine,ExecuteStr.GetBuffer(1000),1000))
		ExecuteStr.Swap(CommandLine);
	si.cb=sizeof(STARTUPINFO);
	si.lpReserved=NULL;
	si.cbReserved2=0;
	si.lpReserved2=NULL;
	si.lpDesktop=NULL;
	si.lpTitle=NULL;
	si.dwFlags=STARTF_USESHOWWINDOW;
	si.wShowWindow=SW_SHOWDEFAULT;
	if (!CreateProcess(NULL,ExecuteStr.GetBuffer(),NULL,
		NULL,FALSE,CREATE_DEFAULT_ERROR_MODE|NORMAL_PRIORITY_CLASS,
		NULL,NULL,&si,&pi))
	{
		CommandKey.CloseKey();
		return FALSE;
	}
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	CommandKey.CloseKey();
	return TRUE;
}

#ifdef DEF_WCHAR
BOOL ShellFunctions::RunRegistryCommand(HKEY hKey,LPCWSTR szFile)
{
	if (!IsUnicodeSystem())
		return RunRegistryCommand(hKey,W2A(szFile));


	PROCESS_INFORMATION pi;
	STARTUPINFOW si;
	CRegKey CommandKey;
	CStringW ExecuteStr;
	CStringW CommandLine;
	int i;

	if (CommandKey.OpenKey(hKey,"command",CRegKey::openExist|CRegKey::samAll)!=ERROR_SUCCESS)
		return FALSE;
	if (CommandKey.QueryValue(L"",ExecuteStr)<2)
		return FALSE;
	i=ExecuteStr.FindFirst(L'%');
	while (i!=-1)
	{
		if (ExecuteStr[i+1]==L'1')
		{
			CommandLine.Copy(ExecuteStr,i);
			CommandLine<<szFile;
			CommandLine<<((LPCWSTR)ExecuteStr+i+2);
			break;
		}
		i=ExecuteStr.FindNext(L'%',i);
	}
	if (i==-1)
		CommandLine=ExecuteStr;
	if (!ExpandEnvironmentStringsW(CommandLine,ExecuteStr.GetBuffer(1000),1000))
		ExecuteStr.Swap(CommandLine);
	si.cb=sizeof(STARTUPINFOW);
	si.lpReserved=NULL;
	si.cbReserved2=0;
	si.lpReserved2=NULL;
	si.lpDesktop=NULL;
	si.lpTitle=NULL;
	si.dwFlags=STARTF_USESHOWWINDOW;
	si.wShowWindow=SW_SHOWDEFAULT;
	if (!CreateProcessW(NULL,ExecuteStr.GetBuffer(),NULL,
		NULL,FALSE,CREATE_DEFAULT_ERROR_MODE|NORMAL_PRIORITY_CLASS,
		NULL,NULL,&si,&pi))
	{
		CommandKey.CloseKey();
		return FALSE;
	}
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	CommandKey.CloseKey();
	return TRUE;
}
#endif


#ifdef DEF_WCHAR


DWORD_PTR ShellFunctions::GetFileInfo(LPCWSTR pszPath,DWORD dwFileAttributes,SHFILEINFOW *psfi,UINT uFlags)
{
	if (IsUnicodeSystem())
		return ::SHGetFileInfoW(pszPath,dwFileAttributes,psfi,sizeof(SHFILEINFOW),uFlags);

	SHFILEINFO fi;
	DWORD_PTR ret=::SHGetFileInfoA(W2A(pszPath),dwFileAttributes,&fi,sizeof(SHFILEINFO),uFlags);
	if (ret==0)
		return 0;

	if (uFlags&SHGFI_DISPLAYNAME)
		MultiByteToWideChar(CP_ACP,0,fi.szDisplayName,-1,psfi->szDisplayName,MAX_PATH);
	if (uFlags&SHGFI_TYPENAME)
		MultiByteToWideChar(CP_ACP,0,fi.szTypeName,-1,psfi->szTypeName,80);
	
	psfi->hIcon=fi.hIcon;
	psfi->iIcon=fi.iIcon;
	psfi->dwAttributes=fi.dwAttributes;
	return ret;	
}

DWORD_PTR ShellFunctions::GetFileInfo(LPITEMIDLIST piil,DWORD dwFileAttributes,SHFILEINFOW *psfi,UINT uFlags)
{
	if (IsUnicodeSystem())
		return ::SHGetFileInfoW((LPCWSTR)piil,dwFileAttributes,psfi,sizeof(SHFILEINFOW),uFlags|SHGFI_PIDL);

	SHFILEINFO fi;
	DWORD_PTR ret=::SHGetFileInfoA((LPCSTR)piil,dwFileAttributes,&fi,sizeof(SHFILEINFO),uFlags|SHGFI_PIDL);
	if (ret==0)
		return 0;

	if (uFlags&SHGFI_DISPLAYNAME)
		MultiByteToWideChar(CP_ACP,0,fi.szDisplayName,-1,psfi->szDisplayName,MAX_PATH);
	if (uFlags&SHGFI_TYPENAME)
		MultiByteToWideChar(CP_ACP,0,fi.szTypeName,-1,psfi->szTypeName,80);
	
	psfi->hIcon=fi.hIcon;
	psfi->iIcon=fi.iIcon;
	psfi->dwAttributes=fi.dwAttributes;
	return ret;	
}


int ShellFunctions::FileOperation(LPSHFILEOPSTRUCTW lpFileOp)
{
	if (IsUnicodeSystem())
		return ::SHFileOperationW(lpFileOp);

	LPCWSTR pFrom=lpFileOp->pFrom;
	LPCWSTR pTo=lpFileOp->pTo;
	LPCWSTR pProgressTitle=lpFileOp->fFlags&FOF_SIMPLEPROGRESS?lpFileOp->lpszProgressTitle:NULL;

	if (pFrom!=NULL)
		lpFileOp->pFrom=(LPCWSTR)alloccopymultiWtoA(pFrom);

	if (pTo!=NULL)
	{
		if (lpFileOp->fFlags&FOF_MULTIDESTFILES)
			lpFileOp->pTo=(LPCWSTR)alloccopymultiWtoA(pTo);
		else
			lpFileOp->pTo=(LPCWSTR)alloccopyWtoA(pTo,istrlenw(pTo)+1);
	}

	if (pProgressTitle!=NULL)
		lpFileOp->lpszProgressTitle=(LPCWSTR)alloccopyWtoA(pProgressTitle);

	int nRet=::SHFileOperationA((LPSHFILEOPSTRUCTA)lpFileOp);
	
	if (pFrom!=NULL)
	{
		delete[] (LPSTR)lpFileOp->pFrom;
		lpFileOp->pFrom=pFrom;
	}

	if (pTo!=NULL)
	{
		delete[] (LPSTR)lpFileOp->pTo;
		lpFileOp->pTo=pTo;
	}

	if (pProgressTitle!=NULL)
	{
		delete[] (LPSTR)lpFileOp->lpszProgressTitle;
		lpFileOp->lpszProgressTitle=pProgressTitle;
	}

	return nRet;
}


int ShellFunctions::ShellExecute(HWND hwnd,LPCWSTR lpOperation,LPCWSTR lpFile,LPCWSTR lpParameters,LPCWSTR lpDirectory,INT nShowCmd)
{
	if (lpOperation!=NULL)
	{
		if (lpOperation[0]=='\0')
			lpOperation=NULL;
	}

	if (IsUnicodeSystem())
		return (int)::ShellExecuteW(hwnd,lpOperation,lpFile,lpParameters,lpDirectory,nShowCmd);
	else
	{
		return (int)::ShellExecuteA(hwnd,(LPCSTR)W2A(lpOperation),(LPCSTR)W2A(lpFile),
			(LPCSTR)W2A(lpParameters),(LPCSTR)W2A(lpDirectory),
			nShowCmd);
	}
}

BOOL ShellFunctions::ShellExecuteEx(LPSHELLEXECUTEINFOW lpExecInfo)
{
	if (IsUnicodeSystem())
		return ::ShellExecuteExW(lpExecInfo);

	ASSERT(sizeof(LPSHELLEXECUTEINFOA)==sizeof(LPSHELLEXECUTEINFOW));
	ASSERT(lpExecInfo->cbSize>sizeof(LPSHELLEXECUTEINFOA));

	SHELLEXECUTEINFO eia;
	CopyMemory(&eia,lpExecInfo,lpExecInfo->cbSize);

	if (lpExecInfo->lpVerb!=NULL)
		eia.lpVerb=alloccopyWtoA(lpExecInfo->lpVerb);
	if (lpExecInfo->lpFile!=NULL)
		eia.lpFile=alloccopyWtoA(lpExecInfo->lpFile);
	if (lpExecInfo->lpParameters!=NULL)
		eia.lpParameters=alloccopyWtoA(lpExecInfo->lpParameters);
	if (lpExecInfo->lpDirectory!=NULL)
		eia.lpDirectory=alloccopyWtoA(lpExecInfo->lpDirectory);
	if (lpExecInfo->fMask&SEE_MASK_CLASSNAME && lpExecInfo->lpClass!=NULL)
		eia.lpClass=alloccopyWtoA(lpExecInfo->lpClass);

	BOOL bRet=::ShellExecuteExA(&eia);

	if (lpExecInfo->lpVerb!=NULL)
		delete[] eia.lpVerb;
	if (lpExecInfo->lpFile!=NULL)
		delete[] eia.lpFile;
	if (lpExecInfo->lpParameters!=NULL)
		delete[] eia.lpParameters;
	if (lpExecInfo->lpDirectory!=NULL)
		delete[] eia.lpDirectory;
	if (lpExecInfo->fMask&SEE_MASK_CLASSNAME && lpExecInfo->lpClass!=NULL)
		delete[] eia.lpClass;
	
	return bRet;
}

#endif


#endif