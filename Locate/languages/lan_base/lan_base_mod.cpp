#include <windows.h>

/* 
An example of source code for locate32 language file 
Copyright (C) 2003-2004 Janne Huttunen <jmhuttun@venda.uku.fi>
*/

/*
Locate32 uses function GetLocateLanguageFileInfo to check whether dll is provides language information
So this function have to be implemented!

Parameters:

szLanguage:				Pointer to a string buffer that receives the null-terminated string 
						specifying language

dwMaxLanguageLength:	Maximum characters in szLanguage ('\0' is included)

szDescription:			Pointer to a string buffer that receives the description of this file

dwMaxDescriptionLength:	Maximum characters in szDescription ('\0' is included)


*/
extern "C" void __declspec(dllexport) GetLocateLanguageFileInfo(
	LPSTR /* OUT */ szLanguage,
	DWORD /* IN  */ dwMaxLanguageLength,
	LPSTR /* OUT */ szDescription,
	DWORD /* IN  */ dwMaxDescriptionLength)
{
	if (dwMaxDescriptionLength>=56)
		dwMaxDescriptionLength=56;
	strncpy(szDescription,"Identifiers, only for the development of language files",dwMaxDescriptionLength-1);
	szDescription[dwMaxDescriptionLength-1]='\0';

	if (dwMaxLanguageLength>=12)
		dwMaxLanguageLength=12;
	strncpy(szLanguage,"IDENTIFIERS",dwMaxLanguageLength-1);
	szLanguage[dwMaxLanguageLength-1]='\0';
}

/* Standard DllMain function, this does nothing but sets hInstance */

BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason,LPVOID lpvReserved)
{
	return TRUE;
}


