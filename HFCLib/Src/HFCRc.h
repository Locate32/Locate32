////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////

#ifndef HFCLIB_H // Building rc file
#include <windows.h>
#endif

#ifndef IDC_OK
#define IDC_OK					1000
#endif

#ifndef IDC_CANCEL
#define IDC_CANCEL				1001
#endif 

#ifndef IDC_EDIT
#define IDC_EDIT				1002
#endif

#ifndef IDC_TEXT
#define IDC_TEXT				1003
#endif

#ifndef IDC_STATIC
#define IDC_STATIC				-1
#endif

// For COptionsPropertyPage
#define IDC_SETTINGS                    1299
#define IDB_OPTIONSPROPERTYPAGEBITMAPS	1299

//Common 3D controls' styles
#define C3DS_BORDER					0x00008000

//3D button's styles
#define B3DS_PUSHBUTTON				BS_PUSHBUTTON	// 0x0000
#define B3DS_DEFPUSHBUTTON			BS_DEFPUSHBUTTON // 0x0001
#define B3DS_TEXTCENTER				0x00000000
#define B3DS_TEXTLEFT				BS_LEFT			// 0x0100
#define B3DS_TEXTRIGHT				BS_RIGHT		// 0x0200
#define B3DS_MULTILINE				BS_MULTILINE	// 0x2000
#define B3DS_MOUSEHILIGHT			0x00000800
#define B3DS_TOPBITMAP				0x00000000
#define B3DS_LEFTBITMAP				0x00000400

//3D button's messages
#define B3DM_SETSTYLE				BM_SETSTYLE
#define B3DM_GETSTYLE				WM_USER+9
#define B3DM_SETBITMAP				WM_USER+10
#define B3DM_SETDISABLEDBITMAP		WM_USER+11
#define B3DM_SETHOTBITMAP			WM_USER+12
#define B3DM_GETBITMAP				WM_USER+13
#define B3DM_GETDISABLEDBITMAP		WM_USER+14
#define B3DM_GETHOTBITMAP			WM_USER+15
#define B3DM_SETCOLOR				WM_USER+16
#define B3DM_GETCOLOR				WM_USER+17
#define B3DM_ISPRESSED				WM_USER+18

//3D button's notification messages
#define B3DN_CLICKED				0
#define B3DN_MOUSEOVER				10
#define B3DN_MOUSEOUT				11
#define B3DN_HILITE					12
#define B3DN_UNHILITE				13
#define B3DN_DOUBLECLICKED			14
#define B3DN_PUSHED					B3DN_HILITE
#define B3DN_UNPUSHED				B3DN_UNHILITE
#define B3DN_DBLCLK					B3DN_DOUBLECLICKED
#define B3DN_SETFOCUS				15
#define B3DN_KILLFOCUS				16

//3D button's color flags
#define B3DC_DISABLEDTEXT			0
#define B3DC_DISABLEDSHADE			1
#define B3DC_NORMALTEXT				2
#define B3DC_NORMALSHADE			3
#define B3DC_MOUSEOUTTEXT			4
#define B3DC_MOUSEOUTSHADE			5

//HEXVIEW control messages		
#define HVM_SETDATA					WM_USER+8
#define HVM_GETDATA					WM_USER+9
#define HVM_GETDATALENGTH			WM_USER+10
#define HVM_ADDDATA					WM_USER+11
#define HVM_SETEXTRAALLOCSIZE		WM_USER+12
#define HVM_GETEXTRAALLOCSIZE		WM_USER+13
#define HVM_GETALLOCLEN				WM_USER+14
#define HVM_REALLOC					WM_USER+15
#define HVM_SETBYTESPERLINE			WM_USER+16
#define HVM_GETBYTESPERLINE			WM_USER+17
#define HVM_ADDDATAANDDELETE		WM_USER+18

