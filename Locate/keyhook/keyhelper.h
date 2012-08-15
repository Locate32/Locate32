/* Keyhook handler for Locate
Copyright (C) 2004-2005 Janne Huttunen				*/

#if !defined(KEYHOOK_H)
#define KEYHOOK_H

#if _MSC_VER >= 1000
#pragma once
#endif 

#ifdef KEYHOOK_EXPORTS
#define KEYHOOK_API __declspec(dllexport)
#else
#define KEYHOOK_API DECLSPEC_IMPORT
#endif

extern "C" {
	KEYHOOK_API HHOOK SetHook(HWND hTargetWndHHOOK,PSHORTCUT* pShortcuts,DWORD nShortcuts);
	KEYHOOK_API BOOL UnsetHook(HHOOK hHook);


	KEYHOOK_API LRESULT CALLBACK HookKeyboardProc(int code,WPARAM wParam,LPARAM lParam);
}





#endif