////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////

#include "HFCLib.h"


BOOL SystemInfo(LPSYSTEMINFO info)
{
#ifdef WIN32
	OSVERSIONINFO ver;
	info->is32BIT=TRUE;
	ver.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
	GetVersionEx(&ver);
	switch(ver.dwPlatformId)
	{
		case VER_PLATFORM_WIN32s:
			info->System=osWin32S;
			break;
		case VER_PLATFORM_WIN32_WINDOWS:
			info->System=osWin95;
			break;
		case VER_PLATFORM_WIN32_NT:
			info->System=osWinNT;
			break;
	}
	info->hiWINVer=ver.dwMajorVersion;
	info->loWINVer=ver.dwMinorVersion;
    info->hiOSVer=0;
    info->loOSVer=0;
	info->hiIEVer=0;
	info->loIEVer=0;
	CRegKey Key;
	if (Key.OpenKey(HKLM,"SOFTWARE\\Microsoft\\Internet Explorer",CRegKey::openExist|CRegKey::samRead)==NOERROR)
	{
		CString Version;
		if (Key.QueryValue("Version",Version))
		{
			int hi=Version.FindFirst('.');
			int lo=Version.FindNext('.',hi);
			info->hiIEVer=atoi(Version.Left(hi));
			info->loIEVer=atoi(Version.Mid(hi,lo));
		}
		return FALSE;
	}	
	return TRUE;
#else
    WORD _ax=0;
    __asm__("
      movw $0x1600,%%ax\n
      int $0x2f\n
      movw %%ax,%0"
      :"=g" (_ax)
      :
      : "memory","ax","bx","cx","dx"
    );
    info->hiWINVer=LOBYTE(_ax);
    info->loWINVer=HIBYTE(_ax);
    __asm__("
      movw $0x3000,%%ax\n
      int $0x21\n
      movw %%ax,%0"
      :"=g" (_ax)
      :
      : "memory","ax","bx","cx","dx"
    );
    info->hiOSVer=LOBYTE(_ax);
    info->loOSVer=HIBYTE(_ax);
    info->hiIEVer=0;
    info->loIEVer=0;
    info->is32BIT=FALSE;
    info->System=osDOS;
    return TRUE;
#endif
}

DWORD GetSystemFeaturesFlag()
{
#ifdef WIN32
	SYSTEMINFO info;
	SystemInfo(&info);
	DWORD nFlags=0;
	switch (info.System)
	{
	case osWin95:
		if (info.hiWINVer>=5 || (info.hiWINVer==4 && info.loWINVer>=90))
			nFlags|=efWinME;
		if (info.hiWINVer>=5 || (info.hiWINVer==4 && info.loWINVer>=10))
			nFlags|=efWin98|efWin95;
		nFlags|=efWin95;
		break;
	case osWinNT:
		if (info.hiWINVer>=6)
			nFlags|=efWinXP|efWin2000|efWinVista;
		else if (info.hiWINVer>=5)
		{
			if (info.loWINVer>=1)
				nFlags|=efWinXP;
			nFlags|=efWin2000;
		}
		nFlags|=efWinNT4;
		break;
	default:
		break;
	}
	if (info.hiIEVer>=4)
		nFlags|=efIE4;
	if (info.hiIEVer>=5)
		nFlags|=efIE5;
	if (info.hiIEVer>=6)
		nFlags|=efIE6;
	if (info.hiIEVer>=7)
		nFlags|=efIE7;
	return nFlags;
#else
	return 0;
#endif
}


/* 

Function for retrieving data from version resource 

Parameters:

szBlock:				Name of version block

szText:					Pointer to a string buffer that receives the data

dwMaxTextLen:			Maximum characters in szText ('\0' is included)
 
*/

BOOL GetVersionText(
	LPCSTR /* IN */ szModulePath,
	LPCSTR /* IN */ szBlock,
	LPSTR /* OUT */ szText, 
	DWORD /* IN  */ dwMaxTextLen
	)
{
	// Copying version information to buffer
	UINT iDataLength=GetFileVersionInfoSize(szModulePath,NULL);
	if (iDataLength<2)
		return FALSE;
	BYTE* pData=new BYTE[iDataLength+2];
	if (pData==NULL)
		return FALSE;

	if (!GetFileVersionInfo(szModulePath,NULL,iDataLength,pData))
	{
		delete[] pData;
		return FALSE;
	}
	
	VOID* pTranslations,* pProductVersion=NULL;
	char szTranslation[200];
	
	
	// Checking first translation block
	if (!VerQueryValue(pData,"VarFileInfo\\Translation",&pTranslations,&iDataLength))
	{
		delete[] pData;
		return FALSE;
	}
	sprintf_s(szTranslation,200,"\\StringFileInfo\\%04X%04X\\%s",LPWORD(pTranslations)[0],LPWORD(pTranslations)[1],szBlock);
	
	
	
	if (!VerQueryValue(pData,szTranslation,&pProductVersion,&iDataLength))
	{
		// Checking english if nothing else does not found
		sprintf_s(szTranslation,200,"\\StringFileInfo\\040904b0\\%s",szBlock);
		
		if (!VerQueryValue(pData,szTranslation,&pProductVersion,&iDataLength))
		{
			delete[] pData;
			return FALSE;
		}
	}


	
	// Copying information from pProductVersion to szText
	strcpy_s(szText,dwMaxTextLen,(LPCSTR)pProductVersion);
	
	delete[] pData;
	return TRUE;
}
	

#ifdef DEF_WCHAR
BOOL GetVersionText(
	LPCWSTR /* IN */ szModulePath,
	LPCSTR /* IN */ szBlock,
	LPWSTR /* OUT */ szText, 
	DWORD /* IN  */ dwMaxTextLen
	)
{
	// Copying version information to buffer
	UINT iDataLength=GetFileVersionInfoSizeW(szModulePath,NULL);
	if (iDataLength<2)
		return FALSE;
	BYTE* pData=new BYTE[iDataLength+2];
	if (pData==NULL)
		return FALSE;

	if (!GetFileVersionInfoW(szModulePath,NULL,iDataLength,pData))
	{
		delete[] pData;
		return FALSE;
	}
	
	VOID* pTranslations,* pProductVersion=NULL;
	WCHAR szTranslation[200];
	
	
	// Checking first translation block
	if (!VerQueryValueW(pData,L"VarFileInfo\\Translation",&pTranslations,&iDataLength))
	{
		delete[] pData;
		return FALSE;
	}
	swprintf_s(szTranslation,200,L"\\StringFileInfo\\%04X%04X\\%S",LPWORD(pTranslations)[0],LPWORD(pTranslations)[1],szBlock);
	
	
	
	if (!VerQueryValueW(pData,szTranslation,&pProductVersion,&iDataLength))
	{
		// Checking english if nothing else does not found
		swprintf_s(szTranslation,200,L"\\StringFileInfo\\040904b0\\%S",szBlock);
		
		if (!VerQueryValueW(pData,szTranslation,&pProductVersion,&iDataLength))
		{
			delete[] pData;
			return FALSE;
		}
	}


	
	// Copying information from pProductVersion to szText
	wcscpy_s(szText,dwMaxTextLen,(LPCWSTR)pProductVersion);
	
	delete[] pData;
	return TRUE;
}

#endif