////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////


#ifndef HFCDEF_H
#define HFCDEF_H

typedef int					BOOL;

#ifndef WIN32

#define FAR					far
#define NEAR				near
#define far
#define near

#define PASCAL
#define WINAPI
#define CALLBACK

#define CONST				const

typedef int					INT;
typedef char				CHAR;
typedef short				SHORT;
typedef long				LONG;
typedef int					BOOL;

typedef unsigned int		UINT;
typedef unsigned char		UCHAR;
typedef unsigned short		USHORT;
typedef unsigned long		ULONG;

typedef unsigned char		BYTE;
typedef unsigned short		WORD;
typedef unsigned long		DWORD;

typedef CHAR				TCHAR;

typedef CHAR*				LPSTR;
typedef CONST CHAR*			LPCSTR;
typedef TCHAR*				LPTSTR;
typedef CONST TCHAR*		LPCTSTR;

typedef void*				LPVOID;
typedef const void*			LPCVOID;
typedef INT*				PINT;
typedef UINT*				PUINT;
typedef CHAR*				PCHAR;
typedef UCHAR*				PUCHAR;
typedef SHORT*				PSHORT;
typedef USHORT*				PUSHORT;
typedef LONG*				PLONG;
typedef ULONG*				PULONG;

typedef BYTE*				PBYTE;
typedef WORD*				PWORD;
typedef DWORD*				PDWORD;
typedef PBYTE				LPBYTE;

typedef LPVOID				WPARAM;
typedef LPVOID				LPARAM;

typedef LPVOID				HANDLE;
typedef HANDLE				HWND;

typedef float				FLOAT;
typedef double				DOUBLE;

#define _MAX_PATH			260
#define MAX_PATH			260

#define TRUE				1
#define FALSE				0



typedef struct tagRECT
{
    LONG    left;
    LONG    top;
    LONG    right;
    LONG    bottom;
} RECT, *PRECT, NEAR *NPRECT, FAR *LPRECT;

typedef const RECT FAR* LPCRECT;

typedef struct tagPOINT
{
    LONG  x;
    LONG  y;
} POINT, *PPOINT, NEAR *NPPOINT, FAR *LPPOINT;

typedef struct tagSIZE
{
    LONG        cx;
    LONG        cy;
} SIZE, *PSIZE, *LPSIZE;

typedef SIZE					SIZEL;
typedef SIZE					*PSIZEL, *LPSIZEL;

#define MAKEWORD(a, b)			((WORD)(((BYTE)(a)) | ((WORD)((BYTE)(b))) << 8))
#define MAKELONG(a, b)			((LONG)(((WORD)(a)) | ((DWORD)((WORD)(b))) << 16))
#define LOWORD(l)				((WORD)(l))
#define HIWORD(l)				((WORD)(((DWORD)(l) >> 16) & 0xFFFF))
#define LOBYTE(w)				((BYTE)(w))
#define HIBYTE(w)				((BYTE)(((WORD)(w) >> 8) & 0xFF))

#define FIRSTBYTE(l)			((BYTE)(l))
#define SECONDBYTE(l)			((BYTE)(((DWORD)(l) >> 8) & 0xFF))
#define THIRHBYTE(l)			((BYTE)(((DWORD)(l) >> 16) & 0xFF))
#define FOURTHBYTE(l)			((BYTE)(((DWORD)(l) >> 24) & 0xFF))

#define max(a,b)				(((a) > (b)) ? (a) : (b))
#define min(a,b)				(((a) < (b)) ? (a) : (b))

#define CharLower(x)			strlwr(x)
#define CharUpper(x)			strupr(x)		
#define CharUpperA(x)           strupr(x)
#define CharLower(x)            strlwr(x)

typedef long long				LONGLONG;
typedef unsigned long long		ULONGLONG;
#endif // !WIN32

#define MAKEVERSION(major,minor)		(DWORD(major)<<16|WORD(minor))

struct __POSITION { void* pPosition; };
typedef __POSITION* POSITION;


typedef FLOAT				*PFLOAT;
typedef DOUBLE				*PDOUBLE;

#define STRNULL				((LPSTR)NULL)
#define CSTRNULL			((LPCSTR)NULL)
#define WSTRNULL			((LPWSTR)NULL)
#define CWSTRNULL			((LPCWSTR)NULL)



#define RGBSET(r,g,b)       (((RGB)((BYTE)(b))) | ((RGB)((BYTE)(g) << 8)) | ((RGB)((BYTE)(r) << 16)))
#define RGBRED(col)         ((BYTE)(((RGB)(col) >> 16) & 0xFF))
#define RGBGREEN(col)       ((BYTE)(((RGB)(col) >> 8) & 0xFF))
#define RGBBLUE(col)        ((BYTE)(((RGB)(col)) & 0xFF))

//Library Ffags
#define HFCFLAG_WIN32		0x1
#define HFCFLAG_DLL			0x2
#define HFCFLAG_DEBUG		0x4
#define HFCFLAG_CONSOLE		0x8
#define HFCFLAG_WCHAR		0x10
#define HFCFLAG_RESOURCES	0x20
#define HFCFLAG_WINDOWS		0x40
#define HFCFLAG_COM			0x80

//System flag
enum systemType {
	osDOS=1,
	osWin31=2,
	osWin95=3,
	osWinNT=4,
	osWin32S=5,
	osLinux=6
};

//Enhanced features
enum efFlags {
	efCheck=0x0000,
	efWin95=0x0001,
	efWin98=0x0002,
	efWin2000=0x0004,
	efWinNT4=0x0008,
	efWinME=0x0010,
	efWinXP=0x0020,
	efWinVista=0x0040,
	efIE4=0x10000,
	efIE5=0x20000,
	efIE6=0x40000,
	efIE7=0x80000
};

typedef struct {
	systemType System;
	DWORD hiOSVer;
	DWORD loOSVer;
   	DWORD hiWINVer;
   	DWORD loWINVer;
	BYTE is32BIT;
	DWORD hiIEVer;
	DWORD loIEVer;
} SYSTEMINFO,*LPSYSTEMINFO;

enum DockingType {
	dockNone = 0,
	dockTop = 1,
	dockBottom = 2,
	dockLeft = 3,
	dockRight =4
};

#ifdef DEF_RESOURCES
enum TypeOfResourceHandle {
	SetBoth=0,
	CommonResource=1,
	LanguageSpecificResource=2
};
#endif

// System messages
#define WM_FIRSTHFC				0xA000
#define WM_FIRSTMDI				0xB000

#define WM_CHILDNOTIFY			WM_FIRSTHFC  // wParam = msg , lParam=(DWORD[2]){wParam,lParam}
#define WM_ISINDOCKINGAREA		WM_FIRSTHFC+1  // lParam = point (POINT*) in client coordinates
#define WM_SETCONTROLPOSITIONS	WM_FIRSTHFC+2

#define WM_MDICLOSED			WM_FIRSTMDI
#define WM_MDIPOSCHANGED		WM_FIRSTMDI+1

// For CPropertyPage
#define WM_REDRAWSELITEMCONTROL		WM_APP+100
#define WM_FOCUSSELITEMCONTROL		WM_APP+101



// Error Handling
typedef DWORD					HFCERROR;

typedef HFCERROR (*HFCERRCALLBACK)(
    HFCERROR nError,DWORD dwData
    );

typedef void * (__cdecl *MALLOC_FUNC)(size_t);


#define HFC_FIRSTDEFERROR			(HFCERROR)0x0
#define HFC_FIRSTMEMERROR			(HFCERROR)0x10000
#define HFC_FIRSTFILEERROR			(HFCERROR)0x20000
#define HFC_FIRSTDATAERROR			(HFCERROR)0x30000
#define HFC_FIRSTFUNCTIONERROR		(HFCERROR)0x40000

//Default

#define HFC_OK						HFC_FIRSTDEFERROR
#define HFC_NOERROR					HFC_FIRSTDEFERROR
#define HFC_SUCCESS					HFC_FIRSTDEFERROR
#define HFC_ERROR					HFC_FIRSTDEFERROR+0x1
#define HFC_UNKNOWNERROR			HFC_FIRSTDEFERROR+0x2
#define HFC_OLEERROR				HFC_FIRSTDEFERROR+0x3



//Memory errors
#define HFC_CANNOTALLOCATE			HFC_FIRSTMEMERROR
#define HFC_CANNOTALLOC				HFC_FIRSTMEMERROR
#define HFC_CANNOTDISCARD			HFC_FIRSTMEMERROR+0x1
#define HFC_CANNOTCREATEHEAP		HFC_FIRSTMEMERROR+0x2
#define HFC_CANNOTREADRESOURCE		HFC_FIRSTMEMERROR+0x3


//File errors
#define HFC_CANNOTREAD				HFC_FIRSTFILEERROR
#define HFC_CANNOTWRITE				HFC_FIRSTFILEERROR+0x1
#define HFC_CANNOTCREATE			HFC_FIRSTFILEERROR+0x2
#define HFC_CANNOTOPEN				HFC_FIRSTFILEERROR+0x3
#define HFC_EOF						HFC_FIRSTFILEERROR+0x4
#define HFC_ENDOFFILE				HFC_FIRSTFILEERROR+0x4
#define HFC_CANNNOTEXECUTE			HFC_FIRSTFILEERROR+0x5
#define HFC_DISKFULL				HFC_FIRSTFILEERROR+0x6
#define HFC_SHARINGVIOLATION		HFC_FIRSTFILEERROR+0x7
#define HFC_LOCKVIOLATION			HFC_FIRSTFILEERROR+0x8
#define HFC_ACCESSDENIED			HFC_FIRSTFILEERROR+0x9
#define HFC_HARDIO					HFC_FIRSTFILEERROR+0xA
#define HFC_BADSEEK					HFC_FIRSTFILEERROR+0xB
#define HFC_DIRECTORYFULL			HFC_FIRSTFILEERROR+0xC
#define HFC_FILEEXISTS				HFC_FIRSTFILEERROR+0xD
#define HFC_FILENOTFOUND			HFC_FIRSTFILEERROR+0xE
#define HFC_BADPATH					HFC_FIRSTFILEERROR+0x10
#define HFC_TOOMANYOPENFILES		HFC_FIRSTFILEERROR+0x11
#define HFC_INVALIDFILE				HFC_FIRSTFILEERROR+0x12


//Data errors
#define HFC_CORRUPTDATA				HFC_FIRSTDATAERROR
#define HFC_INVALIDFORMAT			HFC_FIRSTDATAERROR+0x1

//Function errors
#define HFC_OBSOLETEFUNCTION		HFC_FIRSTFUNCTIONERROR
#define HFC_NOTIMPLEMENTED			HFC_FIRSTFUNCTIONERROR+0x1
#define HFC_INVALIDPARAMETER		HFC_FIRSTFUNCTIONERROR+0x2


#ifdef WIN32

#define HKCR				HKEY_CLASSES_ROOT
#define HKCU				HKEY_CURRENT_USER
#define HKLM				HKEY_LOCAL_MACHINE
#define HKU					HKEY_USERS

#define IDM_FIRST_MDICHILD	0xFF00
#define IDM_WINDOW_FIRST	0xE130
#define IDM_WINDOW_LAST		0xE13F
#define IDW_PANE_FIRST		0xE900
#define IDW_PANE_SAVE		0xEA21

// CInputDialog
#define ID_DONTALLOWEMPTY	0x1
#define ID_NOCANCELBUTTON	0x2
#endif

// For CListCtrlEx
#ifdef _INC_COMMCTRL
struct NMHDR_MOUSE : public NMHDR,LVHITTESTINFO {};

#define NMX_CLICK					NM_FIRST-220U // This is area for property sheets
#define NMX_DBLCLICK				NM_FIRST-221U 
#define NMX_RCLICK					NM_FIRST-222U 
#define NMX_RDBLCLICK				NM_FIRST-223U 
#define NMX_MCLICK					NM_FIRST-224U 
#define NMX_MDBLCLICK				NM_FIRST-225U 
#endif 

extern LPCSTR				szEmpty;
extern LPCSTR				szError;

#ifdef DEF_WCHAR
extern LPCWSTR				szwEmpty;
extern LPCWSTR				szwError;
#endif

// some extras
#define foreach(ptr,len)	for(DWORD f=0;f<(len);f++) ptr[f]

// usage char sz[4]; foreach(sz,4)=4;
#endif
