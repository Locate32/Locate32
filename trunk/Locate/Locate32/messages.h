/* Locate32 - Copyright (c) 1997-2010 Janne Huttunen */

#ifndef MESSAGES_H
#define MESSAGES_H


#define WM_SYSTEMTRAY				WM_APP+100
#define WM_SETOPERATIONSTATUSBAR	WM_APP+101 //wParam is ID to icon, lParam is text
#define WM_UPDATINGSTOPPED			WM_APP+104
#define WM_GETLOCATEDLG				WM_APP+105 // wParam==0: return HWND, wParam==1: return PTR, wParam==2: return PTR to ST
#define WM_FREEUPDATESTATUSPOINTERS	WM_APP+106
#define WM_EXECUTESHORTCUT			WM_APP+107 // wParam is index to shortcut 
#define WM_RESETSHORTCUTS			WM_APP+108
#define WM_RESULTLISTACTION			WM_APP+109
#define WM_GETSELECTEDITEMPATH		WM_APP+110
#define WM_ENABLEITEMS				WM_APP+111
#define WM_SETITEMFOCUS				WM_APP+112 // wParam is handle to window
#define WM_CLOSEDIALOG				WM_APP+113 
#define WM_OPENDIALOG				WM_APP+114 
#define WM_REMOVEIGNORECLICKSFLAG	WM_APP+115
#define WM_SETSTARTDATA				WM_APP+116 // lParam is a pointer to CStartData struct




// LOWORD(wParam) for Locate32Communication
#define LOCATEMSG_ACTIVATEINSTANCE			0
#define LOCATEMSG_INSTANCEEXITED			1
#define LOCATEMSG_EXITPROCESSS				2

#endif