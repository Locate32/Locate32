#include <windows.h>
#include <stdio.h>

#include "../../common/win95crtfixes.h"

/* 
An example of source code for locate32 language file 
Copyright (C) 2003-2006 Janne Huttunen <jmhuttun@venda.uku.fi>
*/

/* 
Locate32 does execute any code from language dll files anymore (for security reasons).
*/

/* Standard DllMain function, this does nothing but sets hInstance */

extern "C"
BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason,LPVOID lpvReserved)
{
	return TRUE;
}


