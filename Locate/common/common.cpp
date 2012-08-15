//
// Common routines for locate
//
// Copyright 2006-2007 Janne Huttunen


#include <HFCLib.h>
#include "common.h"

#include <parsers.h>
extern "C" {
	#include "JpegLib/jpeglib.h"
}
#ifdef _DEBUG
#pragma comment(lib, "libjpegd.lib")
#else
#pragma comment(lib, "libjpeg.lib")
#endif


#include <commoncontrols.h>



typedef HRESULT (__stdcall * PFNSHGETFOLDERPATH)(HWND, int, HANDLE, DWORD, LPWSTR);  // "SHGetFolderPathW"


LPWSTR GetDefaultFileLocation(LPCWSTR szFileName,BOOL bMustExists,DWORD* lpdwSize)
{
	int nFileNameLen=istrlen(szFileName);
	
	// Check first that is there 
	PFNSHGETFOLDERPATH pGetFolderPath=(PFNSHGETFOLDERPATH)GetProcAddress(GetModuleHandle("shell32.dll"),"SHGetFolderPathW");
	if (pGetFolderPath!=NULL)
	{
		WCHAR szAppDataPath[MAX_PATH];
		if (SUCCEEDED(pGetFolderPath(NULL,CSIDL_APPDATA,NULL,
			SHGFP_TYPE_CURRENT,szAppDataPath)))
		{
			int nPathLen=istrlen(szAppDataPath);

			// Insert \\ if already not there
			if (szAppDataPath[nPathLen-1]!=L'\\')
				szAppDataPath[nPathLen++]=L'\\';
			
			// Buffer for default path
			LPWSTR pStr=new WCHAR[nPathLen+9+nFileNameLen+1];
			MemCopyW(pStr,szAppDataPath,nPathLen);
			MemCopyW(pStr+nPathLen,L"Locate32",9);
			nPathLen+=8;

			// Now pStr contains X:\UsersFolder\AppPath\Locate32

			// Check whether directory exists
			if (FileSystem::IsDirectory(pStr))
			{
				// Directory does exist
				// Copy file name part at tail 
				pStr[nPathLen++]='\\';
				MemCopyW(pStr+nPathLen,szFileName,nFileNameLen+1);
				if (lpdwSize!=NULL)
					*lpdwSize=nPathLen+nFileNameLen;
				
				if (!bMustExists)
					return pStr;	

				// Checking file
				if (FileSystem::IsFile(pStr))
					return pStr;
			}
			else
			{
				// Create directory if does not already exists 
				// and bMustExists is not set (this function is called
				// for default databases with bMustExists=FALSE)
				if (!bMustExists)
				{
					FileSystem::CreateDirectory(pStr);
					// Copy file name part at tail 
					pStr[nPathLen++]='\\';
					MemCopyW(pStr+nPathLen,szFileName,nFileNameLen+1);
					if (lpdwSize!=NULL)
						*lpdwSize=nPathLen+nFileNameLen;
					
					return pStr;
				}	
			
			}
				

			delete[] pStr;
		}
	}

	// Check also Locate32's directory, maybe there is that file
	int iLen;
	LPWSTR pStr;
	WCHAR szExeName[MAX_PATH];
	FileSystem::GetModuleFileName(NULL,szExeName,MAX_PATH);
	iLen=LastCharIndex(szExeName,L'\\')+1;
	pStr=new WCHAR[iLen+nFileNameLen+1];
	MemCopyW(pStr,szExeName,iLen);
	
	MemCopyW(pStr+iLen,szFileName,nFileNameLen+1);
	if (lpdwSize!=NULL)
		*lpdwSize=iLen+nFileNameLen;

	if (!bMustExists)
		return pStr;	

	if (FileSystem::IsFile(pStr))
		return pStr;

	// Return NULL
	delete[] pStr;
	return NULL;
}


LPSTR ReadIniFile(LPSTR* pFile,LPCSTR szSection,BYTE& bFileIsReg)
{
	CAutoPtrA<WCHAR> pIniFile=GetDefaultFileLocation(L"locate.ini",TRUE);
	if (pIniFile==NULL)
		return NULL;

	bFileIsReg=TRUE;

	char* pFileContent=NULL;
	try
	{
		CFile Ini(pIniFile,CFile::defRead|CFile::otherErrorWhenEOF,TRUE);
		DWORD dwSize=Ini.GetLength();
		pFileContent=new char[dwSize+1];
		Ini.Read(pFileContent,dwSize);
		pFileContent[dwSize]='\0';
		Ini.Close();
	}
	catch (...)
	{
		if (pFileContent!=NULL)
			delete[] pFileContent;
		return NULL;
	}
		

	LPCSTR pPtr=NULL;
	LPSTR pKeyName=NULL;

	CString Key,Value;

	if (szSection!=NULL)
		pPtr=FindSectionStart(pFileContent,szSection);
	if (pPtr==NULL)
		pPtr=FindSectionStart(pFileContent,"DEFAULT");

	while (pPtr!=NULL)
	{
		pPtr=FindNextValue(pPtr,Key,Value);
		if (Key.IsEmpty())
			break;

		while (Value.LastChar()==' ')
			Value.DelLastChar();

		if (Key.CompareNoCase("KEY")==0)
			pKeyName=Value.GiveBuffer();
		else if (Key.CompareNoCase("FILE")==0)
		{
			if (pFile!=NULL)
			{
				if (Value.FindFirst('\\')==-1)
				{
					int nDirLength=LastCharIndex((LPCWSTR)pIniFile,L'\\')+1;
					int nDirLengthA=WideCharToMultiByte(CP_ACP,0,(LPCWSTR)pIniFile,nDirLength,0,NULL,NULL,NULL);
					
					*pFile=new CHAR[nDirLengthA+Value.GetLength()+1];
					MemCopyWtoA(*pFile,nDirLengthA,pIniFile,nDirLength);
					MemCopy(*pFile+nDirLengthA,(LPCSTR)Value,Value.GetLength()+1);

				}
				else
					*pFile=Value.GiveBuffer();
			}
		}
		else if (Key.CompareNoCase("FILETYPE")==0)
		{
			if (Value.CompareNoCase("BIN")==0)
				bFileIsReg=FALSE;
			else if (Value.CompareNoCase("REG")==0)
				bFileIsReg=TRUE;
		}
	}
	
	delete[] pFileContent;
	return pKeyName;		
}

BOOL LoadSettingsFromFile(LPCSTR szKey,LPCSTR szFile,BYTE bFileIsReg)
{
	HKEY hKey;
	LONG lRet=RegOpenKeyEx(HKEY_CURRENT_USER,szKey,
		0,KEY_READ,&hKey);
	
	// Check wheter key exists
	if (lRet!=ERROR_FILE_NOT_FOUND)
	{
		// Key exists, using it
		RegCloseKey(hKey);
		return TRUE;
	}

	if (bFileIsReg)
	{
				
		// Restore key
		char szCommand[2000];
		sprintf_s(szCommand,2000,"regedit /s \"%s\"",szFile);

		PROCESS_INFORMATION pi;
		STARTUPINFO si; // Ansi and Unicode versions are same
		ZeroMemory(&si,sizeof(STARTUPINFO));
		si.cb=sizeof(STARTUPINFO);
		
		if (CreateProcess(NULL,szCommand,NULL,
			NULL,FALSE,CREATE_DEFAULT_ERROR_MODE|NORMAL_PRIORITY_CLASS,
			NULL,NULL,&si,&pi))
		{
			WaitForSingleObject(pi.hProcess,2000);
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);	
		}
		else
			return FALSE;

		return TRUE;
	}

	// Acquiring required privileges	
	HANDLE hToken;
	if (OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES,&hToken))
	{
		PTOKEN_PRIVILEGES ns=(PTOKEN_PRIVILEGES)new BYTE[sizeof(DWORD)+sizeof(LUID_AND_ATTRIBUTES)+2];
		if (LookupPrivilegeValue(NULL,SE_BACKUP_NAME,&(ns->Privileges[0].Luid)))
		{
			ns->PrivilegeCount=1;
			ns->Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
			if (!AdjustTokenPrivileges(hToken,FALSE,ns,0,NULL,NULL))
			{
				//ShowError(NULL,IDS_ERRORCANNOTENABLEPRIVILEGE,GetLastError());
			}
		}
		if (LookupPrivilegeValue(NULL,SE_RESTORE_NAME,&(ns->Privileges[0].Luid)))
		{
			ns->PrivilegeCount=1;
			ns->Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
			if (!AdjustTokenPrivileges(hToken,FALSE,ns,0,NULL,NULL))
			{
				//ShowError(NULL,IDS_ERRORCANNOTENABLEPRIVILEGE,GetLastError());
			}

		}
		delete[] (BYTE*)ns;
		CloseHandle(hToken);
	}

	// First, check that we can restore key
	
	lRet=RegCreateKeyEx(HKEY_CURRENT_USER,szKey,
		0,NULL,REG_OPTION_BACKUP_RESTORE,KEY_ALL_ACCESS,NULL,&hKey,NULL);
	if (lRet!=ERROR_SUCCESS)
	{
		//ShowError(hWnd,IDS_ERRORCANNOTCREATEKEY,lRet);
		return FALSE;
	}
	lRet=RegRestoreKey(hKey,szFile,0);
	RegCloseKey(hKey);		
	
	return lRet==ERROR_SUCCESS;
}

BOOL SaveSettingsToFile(LPCSTR szKey,LPCSTR szFile,BYTE bFileIsReg)
{
	if (bFileIsReg)
	{
		// Registry script
		char szCommand[2000];
		sprintf_s(szCommand,2000,"regedit /ea \"%s\" HKEY_CURRENT_USER\\%s",szFile,szKey);

		if (FileSystem::IsFile(szFile))
			DeleteFile(szFile);

		PROCESS_INFORMATION pi;
		STARTUPINFO si; // Ansi and Unicode versions are same
		ZeroMemory(&si,sizeof(STARTUPINFO));
		si.cb=sizeof(STARTUPINFO);
		
		
		if (CreateProcess(NULL,szCommand,NULL,
			NULL,FALSE,CREATE_DEFAULT_ERROR_MODE|NORMAL_PRIORITY_CLASS,
			NULL,NULL,&si,&pi))
		{
			WaitForSingleObject(pi.hProcess,2000);		
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);	
		}
		else
			return FALSE;

		if (FileSystem::IsFile(szFile))
		{
			CRegKey::DeleteKey(HKCU,szKey);
		}

		return TRUE;		
	}

	// Acquiring required privileges	
	HANDLE hToken;
	if (OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES,&hToken))
	{
		PTOKEN_PRIVILEGES ns=(PTOKEN_PRIVILEGES)new BYTE[sizeof(DWORD)+sizeof(LUID_AND_ATTRIBUTES)+2];
		if (LookupPrivilegeValue(NULL,SE_BACKUP_NAME,&(ns->Privileges[0].Luid)))
		{
			ns->PrivilegeCount=1;
			ns->Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
			if (!AdjustTokenPrivileges(hToken,FALSE,ns,0,NULL,NULL))
			{
				//ShowError(NULL,IDS_ERRORCANNOTENABLEPRIVILEGE,GetLastError());
			}
		}
		if (LookupPrivilegeValue(NULL,SE_RESTORE_NAME,&(ns->Privileges[0].Luid)))
		{
			ns->PrivilegeCount=1;
			ns->Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
			if (!AdjustTokenPrivileges(hToken,FALSE,ns,0,NULL,NULL))
			{
				//ShowError(NULL,IDS_ERRORCANNOTENABLEPRIVILEGE,GetLastError());
			}

		}
		delete[] (BYTE*)ns;
	}

	// Data format
	HKEY hRegKey;
	LONG lRet=RegOpenKeyEx(HKEY_CURRENT_USER,szKey,
		0,KEY_READ,&hRegKey);

	if (lRet!=ERROR_SUCCESS)
	{
		//ShowError(hWnd,IDS_ERRORCANNOTOPENKEY,lRet);
		return FALSE;
	}

	DeleteFile(szFile);
	lRet=RegSaveKey(hRegKey,szFile,NULL);
	RegCloseKey(hRegKey);		
	
	if (lRet==ERROR_SUCCESS)
		CRegKey::DeleteKey(HKCU,szKey);
	
	return TRUE;
}




// Save JPEG 
BOOL SaveToJpegFile(HBITMAP hBitmap,LPCWSTR szFile,int nQuality)
{
	HBITMAP hNewBitmap;
	BITMAP bm;
	GetObject(hBitmap, sizeof(BITMAP), &bm);


	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	FILE * outfile;
	JSAMPLE* scanline;
	COLORREF pixel;

	CDC dc1,dc2;
	dc1.GetDC(NULL);
	hNewBitmap = ::CreateCompatibleBitmap(dc1,bm.bmWidth,bm.bmHeight);
	dc1.ReleaseDC();
	if(hNewBitmap==NULL) 
		return FALSE;
	
	dc1.CreateCompatibleDC(NULL);
	dc1.SelectObject(hBitmap);

	dc2.CreateCompatibleDC(NULL);
	dc2.SelectObject(hNewBitmap);
	dc2.FillRect(&CRect(0,0,bm.bmWidth,bm.bmHeight),(HBRUSH)GetStockObject(GRAY_BRUSH));
	dc2.DrawState(CPoint(0,0),CSize(bm.bmWidth,bm.bmHeight),hNewBitmap,DST_BITMAP,(HBRUSH)GetStockObject(WHITE_BRUSH));
	//dc2.BitBlt(0, 0, bm.bmWidth, bm.bmHeight, dc1, 0, 0, SRCCOPY);

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	if (_wfopen_s(&outfile,szFile,L"wb")!=0) {
		return FALSE;
	}
	
	jpeg_stdio_dest(&cinfo, outfile);
	cinfo.image_width = bm.bmWidth;
	cinfo.image_height = bm.bmHeight;
	cinfo.input_components = 3;
	cinfo.in_color_space = JCS_RGB;
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, nQuality, FALSE);
	jpeg_start_compress(&cinfo, TRUE);
	scanline = new JSAMPLE[bm.bmWidth*3+4];
	for(int posy = 0; posy < bm.bmHeight; posy++) 
	{
		for(int posx = 0; posx < bm.bmWidth; posx++)
		{
			pixel = dc2.GetPixel(posx,posy);
			*((DWORD*)(scanline+posx*3))=pixel;
			/*scanline[posx*3+0] = GetRValue(pixel);
			scanline[posx*3+1] = GetGValue(pixel);
			scanline[posx*3+2] = GetBValue(pixel);*/
		}
		jpeg_write_scanlines(&cinfo, &scanline, 1);
	}
	jpeg_finish_compress(&cinfo);

	jpeg_destroy_compress(&cinfo);
	delete[] scanline;
	fclose(outfile);
	
	dc1.DeleteDC();
	dc2.DeleteDC();
	DeleteObject(hNewBitmap);
	return TRUE;

	
}






// Save JPEG from imagelist
BOOL SaveToJpegFile(HIMAGELIST hImageList,int nIndex,LPCWSTR szFile,int nQuality,SIZE* pSize)
{
	HBITMAP hNewBitmap;
	int cxOrig=32,cyOrig=32;
	ImageList_GetIconSize(hImageList,&cxOrig,&cyOrig);
	
	int cx=cxOrig,cy=cyOrig;
	if (pSize!=NULL &&  pSize->cx>=cx && pSize->cy>=cy)
	{
		cx=pSize->cx;
		cy=pSize->cy;
		pSize=NULL;
	}
	

	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	FILE * outfile;
	JSAMPLE* scanline;
	COLORREF pixel;

	CDC dc;
	dc.GetDC(NULL);
	hNewBitmap = ::CreateCompatibleBitmap(dc,cx,cy);
	dc.ReleaseDC();
	if(hNewBitmap==NULL) 
		return FALSE;
	
	dc.CreateCompatibleDC(NULL);
	dc.SelectObject(hNewBitmap);
	dc.FillRect(&CRect(0,0,cx,cy),(HBRUSH)GetStockObject(WHITE_BRUSH));

	ImageList_Draw(hImageList,nIndex,dc,(cx-cxOrig)/2,(cy-cyOrig)/2,ILD_NORMAL);

	if (pSize!=NULL)
	{
		// We need to rescale
		HBITMAP hOrigBitmap=hNewBitmap;
		CDC dcOrig;
		dcOrig.Attach(dc);
		cx=pSize->cx;
		cy=pSize->cy;

		BITMAPINFO  dibInfo;
		BYTE* pBuffer;
		dibInfo.bmiHeader.biBitCount = 24;
		dibInfo.bmiHeader.biClrImportant = 0;
		dibInfo.bmiHeader.biClrUsed = 0;
		dibInfo.bmiHeader.biCompression = 0;
		dibInfo.bmiHeader.biHeight = cy;
		dibInfo.bmiHeader.biPlanes = 1;
		dibInfo.bmiHeader.biSize = 40;
		dibInfo.bmiHeader.biWidth = cx;
		dibInfo.bmiHeader.biSizeImage = cx*cy*3;
		dibInfo.bmiHeader.biXPelsPerMeter = 3780;
		dibInfo.bmiHeader.biYPelsPerMeter = 3780;
		dibInfo.bmiColors[0].rgbBlue = 0;
		dibInfo.bmiColors[0].rgbGreen = 0;
		dibInfo.bmiColors[0].rgbRed = 0;
		dibInfo.bmiColors[0].rgbReserved = 0;
		dc.GetDC(NULL);
		hNewBitmap = CreateDIBSection(dc,(const BITMAPINFO*)&dibInfo,DIB_RGB_COLORS,(void**)&pBuffer,NULL,0);
		dc.ReleaseDC();

		dc.CreateCompatibleDC(NULL);
		dc.SelectObject(hNewBitmap);

		dc.StretchBlt(0,0,cx,cy,dcOrig,0,0,cxOrig,cyOrig,SRCCOPY);

		DeleteObject(hOrigBitmap);
	}

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	if (_wfopen_s(&outfile,szFile,L"wb")!=0) {
		return FALSE;
	}
	
	jpeg_stdio_dest(&cinfo, outfile);
	cinfo.image_width = cx;
	cinfo.image_height = cy;
	cinfo.input_components = 3;
	cinfo.in_color_space = JCS_RGB;
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, nQuality, FALSE);
	jpeg_start_compress(&cinfo, TRUE);
	scanline = new JSAMPLE[cx*3+4];
	for(int posy = 0; posy < cy; posy++) 
	{
		for(int posx = 0; posx < cx; posx++)
		{
			pixel = dc.GetPixel(posx,posy);
			*((DWORD*)(scanline+posx*3))=pixel;
			/*scanline[posx*3+0] = GetRValue(pixel);
			scanline[posx*3+1] = GetGValue(pixel);
			scanline[posx*3+2] = GetBValue(pixel);*/
		}
		jpeg_write_scanlines(&cinfo, &scanline, 1);
	}
	jpeg_finish_compress(&cinfo);

	jpeg_destroy_compress(&cinfo);
	delete[] scanline;
	fclose(outfile);
	
	dc.DeleteDC();
	DeleteObject(hNewBitmap);
	return TRUE;

	
}


// Save JPEG from imagelist
BOOL SaveToJpegFile(IImageList* pImageList,int nIndex,LPCWSTR szFile,int nQuality,SIZE* pSize)
{
	HBITMAP hNewBitmap;
	int cxOrig=32,cyOrig=32;
	pImageList->GetIconSize(&cxOrig,&cyOrig);
	
	int cx=cxOrig,cy=cyOrig;
	if (pSize!=NULL &&  pSize->cx>=cx && pSize->cy>=cy)
	{
		cx=pSize->cx;
		cy=pSize->cy;
		pSize=NULL;
	}
	

	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	FILE * outfile;
	JSAMPLE* scanline;
	COLORREF pixel;

	CDC dc;
	dc.GetDC(NULL);
	hNewBitmap = ::CreateCompatibleBitmap(dc,cx,cy);
	dc.ReleaseDC();
	if(hNewBitmap==NULL) 
		return FALSE;
	
	dc.CreateCompatibleDC(NULL);
	dc.SelectObject(hNewBitmap);
	dc.FillRect(&CRect(0,0,cx,cy),(HBRUSH)GetStockObject(WHITE_BRUSH));

	IMAGELISTDRAWPARAMS ip;
	ZeroMemory(&ip,sizeof(IMAGELISTDRAWPARAMS));
	ip.cbSize=sizeof(IMAGELISTDRAWPARAMS);
	ip.i=nIndex;
	ip.hdcDst=dc;
	ip.fStyle=ILD_TRANSPARENT;
	ip.dwRop=WHITENESS;
	ip.rgbBk=CLR_NONE;
	ip.rgbFg=CLR_NONE;
	ip.x=(cx-cxOrig)/2;
	ip.y=(cy-cyOrig)/2;
	ip.cx=cxOrig;
	ip.cy=cyOrig;
	
	pImageList->Draw(&ip);

	if (pSize!=NULL)
	{
		// We need to rescale
		HBITMAP hOrigBitmap=hNewBitmap;
		CDC dcOrig;
		dcOrig.Attach(dc);
		cx=pSize->cx;
		cy=pSize->cy;

		BITMAPINFO  dibInfo;
		BYTE* pBuffer;
		dibInfo.bmiHeader.biBitCount = 24;
		dibInfo.bmiHeader.biClrImportant = 0;
		dibInfo.bmiHeader.biClrUsed = 0;
		dibInfo.bmiHeader.biCompression = 0;
		dibInfo.bmiHeader.biHeight = cy;
		dibInfo.bmiHeader.biPlanes = 1;
		dibInfo.bmiHeader.biSize = 40;
		dibInfo.bmiHeader.biWidth = cx;
		dibInfo.bmiHeader.biSizeImage = cx*cy*3;
		dibInfo.bmiHeader.biXPelsPerMeter = 3780;
		dibInfo.bmiHeader.biYPelsPerMeter = 3780;
		dibInfo.bmiColors[0].rgbBlue = 0;
		dibInfo.bmiColors[0].rgbGreen = 0;
		dibInfo.bmiColors[0].rgbRed = 0;
		dibInfo.bmiColors[0].rgbReserved = 0;
		dc.GetDC(NULL);
		hNewBitmap = CreateDIBSection(dc,(const BITMAPINFO*)&dibInfo,DIB_RGB_COLORS,(void**)&pBuffer,NULL,0);
		dc.ReleaseDC();

		dc.CreateCompatibleDC(NULL);
		dc.SelectObject(hNewBitmap);

		dc.StretchBlt(0,0,cx,cy,dcOrig,0,0,cxOrig,cyOrig,SRCCOPY);

		DeleteObject(hOrigBitmap);
	}

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	if (_wfopen_s(&outfile,szFile,L"wb")!=0) {
		return FALSE;
	}
	
	jpeg_stdio_dest(&cinfo, outfile);
	cinfo.image_width = cx;
	cinfo.image_height = cy;
	cinfo.input_components = 3;
	cinfo.in_color_space = JCS_RGB;
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, nQuality, FALSE);
	jpeg_start_compress(&cinfo, TRUE);
	scanline = new JSAMPLE[cx*3+4];
	for(int posy = 0; posy < cy; posy++) 
	{
		for(int posx = 0; posx < cx; posx++)
		{
			pixel = dc.GetPixel(posx,posy);
			*((DWORD*)(scanline+posx*3))=pixel;
			/*scanline[posx*3+0] = GetRValue(pixel);
			scanline[posx*3+1] = GetGValue(pixel);
			scanline[posx*3+2] = GetBValue(pixel);*/
		}
		jpeg_write_scanlines(&cinfo, &scanline, 1);
	}
	jpeg_finish_compress(&cinfo);

	jpeg_destroy_compress(&cinfo);
	delete[] scanline;
	fclose(outfile);
	
	dc.DeleteDC();
	DeleteObject(hNewBitmap);
	return TRUE;

	
}

