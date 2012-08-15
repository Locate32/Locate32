////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////

#include "HFCLib.h"


DWORD Network::GetComputerNameFromIDList(LPITEMIDLIST lpiil,LPSTR szName,DWORD dwBufferLen)
{
	// May be computer?
	IShellFolder *psf;
	
	if (!SUCCEEDED(SHGetDesktopFolder(&psf)))
		return 0;

	SHDESCRIPTIONID di;
	if (!SUCCEEDED(SHGetDataFromIDList(psf,lpiil,SHGDFIL_DESCRIPTIONID,&di,sizeof(SHDESCRIPTIONID))))
	{
		psf->Release();
		return 0;
	}

	if (di.clsid!=CLSID_NetworkPlaces)
	{
		psf->Release();
		return 0;
	}

	STRRET str;
	if (!SUCCEEDED(psf->GetDisplayNameOf(lpiil,SHGDN_FORPARSING,&str)))
	{
		psf->Release();
		return 0;
	}
	psf->Release();

	if (ShellFunctions::StrRetToStr(str,lpiil,szName,dwBufferLen))
		return istrlen(szName);
	return 0;
}

#ifdef DEF_WCHAR
DWORD Network::GetComputerNameFromIDList(LPITEMIDLIST lpiil,LPWSTR szName,DWORD dwBufferLen)
{
	// May be computer?
	IShellFolder *psf;
	
	if (!SUCCEEDED(SHGetDesktopFolder(&psf)))
		return 0;

	SHDESCRIPTIONID di;
	if (!SUCCEEDED(SHGetDataFromIDList(psf,lpiil,SHGDFIL_DESCRIPTIONID,&di,sizeof(SHDESCRIPTIONID))))
	{
		psf->Release();
		return 0;
	}

	if (di.clsid!=CLSID_NetworkPlaces)
	{
		psf->Release();
		return 0;
	}

	STRRET str;
	if (!SUCCEEDED(psf->GetDisplayNameOf(lpiil,SHGDN_NORMAL | SHGDN_FORPARSING,&str)))
	{
		psf->Release();
		return 0;
	}
	psf->Release();

	if (ShellFunctions::StrRetToStr(str,lpiil,szName,dwBufferLen))
		return istrlenw(szName);
	return 0;
}


BOOL Network::GetNethoodTarget(LPCWSTR szFolder,LPWSTR szTarget,DWORD nBufferLen)
{
	CStringW file(szFolder);
	if (file.LastChar()!=L'\\')
		file << L'\\';
	file << L"desktop.ini";

	WCHAR cls[300];
	if (IsUnicodeSystem())
	{
		if (!GetPrivateProfileStringW(L".ShellClassInfo",L"CLSID2",szwEmpty,cls,300,file))
			return FALSE;
	}
	else
	{
		char clsA[300];
		if (!GetPrivateProfileString(".ShellClassInfo","CLSID2",szEmpty,clsA,300,W2A(file)))
			return FALSE;
		MultiByteToWideChar(CP_ACP,0,clsA,-1,cls,300);
	}


	if (wcscmp(cls,L"{0AFACED1-E828-11D1-9187-B532F1E9575D}")!=0)
		return FALSE; // Folder shortcut

	
	IShellLink* psl;
	if (!SUCCEEDED(CoCreateInstance(CLSID_ShellLink,NULL,CLSCTX_INPROC_SERVER,IID_IShellLink,(void**)&psl)))
		return FALSE;

	IPersistFile* ppf;
	if (!SUCCEEDED(psl->QueryInterface(IID_IPersistFile,(void**)&ppf)))
	{
		psl->Release();
		return FALSE;
	}

	IShellFolder* psf;
	if (!SUCCEEDED(SHGetDesktopFolder(&psf)))
	{
		ppf->Release();
		psl->Release();
		return FALSE;
	}

	file=szFolder;
	if (file.LastChar()!=L'\\')
		file << L'\\';
	file << L"target.lnk";


	BOOL bRet=FALSE;

	if (SUCCEEDED(ppf->Load(file,0)))
	{
		LPITEMIDLIST il;
		if (SUCCEEDED(psl->GetIDList(&il)))
		{
			STRRET str;
			if (SUCCEEDED(psf->GetDisplayNameOf(il,SHGDN_FORPARSING,&str)))
			{
				if (ShellFunctions::StrRetToStr(str,il,szTarget,nBufferLen))
					bRet=2;
			}
			else
			{
				SHDESCRIPTIONID di;
				if (SUCCEEDED(SHGetDataFromIDList(psf,il,SHGDFIL_DESCRIPTIONID,&di,sizeof(SHDESCRIPTIONID))))
				{
                    if (di.clsid==CLSID_NetworkPlaces)
					{		
						if (SUCCEEDED(psf->GetDisplayNameOf(il,SHGDN_FORPARSING,&str)))
						{
							if (ShellFunctions::StrRetToStr(str,il,szTarget,nBufferLen))
								bRet=szTarget[0]=='\\' && szTarget[1]=='\\';
						}
					}
				}
			}
			CoTaskMemFree(il);
		}
	}
	
	psf->Release();
	ppf->Release();
	psl->Release();
	return bRet;
}

#endif
