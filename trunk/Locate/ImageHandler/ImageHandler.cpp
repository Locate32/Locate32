/* Image property handler front-end for GDI+ 
Copyright (C) 2003-2007 Janne Huttunen				*/

#include <windows.h>
#include <gdiplus.h>

#include "ImageHandler.h"

#include "../common/win95crtfixes.h"

using namespace Gdiplus;

IMAGEHANDLER_API BOOL GetImageDimensionsA(LPCSTR szFile,SIZE* dim)
{
	size_t dwStrLen=strlen(szFile);
	WCHAR* szWFile=new WCHAR[dwStrLen+2];
	if (szWFile==NULL)
		return FALSE;
	MultiByteToWideChar(CP_ACP,0,szFile,int(dwStrLen+1),szWFile,int(dwStrLen)+2);
	BOOL bRet=GetImageDimensionsW(szWFile,dim);
	delete[] szWFile;
	return bRet;
}

IMAGEHANDLER_API BOOL GetImageDimensionsW(LPCWSTR szFile,SIZE* dim)
{
	Image* pImage=Image::FromFile(szFile,FALSE);
	if (pImage==NULL)
		return FALSE;
 
	dim->cx=pImage->GetWidth();
	dim->cy=pImage->GetHeight();

	delete pImage;
	return dim->cx>0 && dim->cy>0;
}

IMAGEHANDLER_API BOOL InitLibrary(ULONG_PTR* pToken)
{
	GdiplusStartupInput gdiplusStartupInput;
	return GdiplusStartup(pToken,&gdiplusStartupInput,NULL)==0;
}

IMAGEHANDLER_API void UninitLibrary(ULONG_PTR uToken)
{
	GdiplusShutdown(uToken);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason,LPVOID lpvReserved)
{
	return TRUE;
}