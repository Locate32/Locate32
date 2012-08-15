////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////


#ifndef HFCLIB_H
#define HFCLIB_H
#define HFCLIB

#ifndef __cplusplus
#error You need C++ compiler...
#endif

#ifdef _CONSOLE
#define CONSOLE
#endif
#ifdef _DEBUG
#define DEBUG
#endif

#ifndef NODEFAULTDEFINITIONS

#ifdef WIN32
#define DEF_WCHAR
#define DEF_RESOURCES
#define DEF_COM

#ifndef CONSOLE
#define DEF_APP
#define DEF_WINDOWS

#define WINVER			0x0600
#define _WIN32_WINNT	0x0600
#define _WIN32_IE		0x0700
#define OEMRESOURCE


#endif // CONSOLE

#else //WIN32


#endif //WIN32

#endif //NODEFAULTDEFINITIONS
///////////////////////////////////////////////
// Includes

#ifdef WIN32
#include <windows.h>
#include <commctrl.h>
#include <shlobj.h>
#include <richedit.h>
#include <richole.h>
#include <htmlhelp.h>
#undef HtmlHelp

#else
#include <string.h>
#include <dir.h>
#include <io.h>
#include <dos.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <tchar.h>
#include <strsafe.h>
#include "HFCDef.h"

///////////////////////////////////////////////
// Classes

class CString;
#ifdef DEF_WCHAR
class CStringW;
#endif

class CTime;
class CFileStatus;
class CWaitCursor;
class CSize;
class CPoint;
class CRect;
#ifdef DEF_RESOURCES
class CImageList;
#endif
class CToolInfo;
class CObject;
	template<class TYPE>
	class CArray;
		class CCharArray;
		class CByteArray;
		class CShortArray;
		class CWordArray;
		class CDWordArray;
		class CIntArray;
		class CUIntArray;
		class CStringArray;
		class CObArray;
		class CPtrArray;
	class CException;
		class CFileException;
	class CFile;
	class CFileFind;
	class CFiles;
	class CGdiObject;
		class CPen;
		class CBrush;
		class CFont;
		class CBitmap;
		class CPalette;
		class CRgn;
	class CDC;
		class CPaintDC;
	class CMenu;
	class CRegKey;
	class CWinThread;
		class CWinApp;
	class CWnd;
		class CTargetWnd;
#ifdef DEF_RESOURCES
			class CFrameWnd;
					class CMDIChildWnd;
					class CMDIFrameWnd;
				class CDialog;
					class CCommonDialog;
						class CColorDialog;
						class CFileDialog;
						class CFindReplaceDialog;
						class CFontDialog;
						class CPageSetupDialog;
						class CPrintDialog;
					class CPropertyPage;
				class CPropertySheet;
#endif
		class CButton;
		class CComboBox;
		class CComboBoxEx;
		class CEdit;
		class CHeaderCtrl;
		class CListCtrl;
		class CProgressCtrl;
		class CRichEditCtrl;
		class CScrollBar;
		class CSpinButtonCtrl;
		class CStatusBarCtrl;
		class CTabCtrl;
		class CToolBarCtrl;
		class CToolTipCtrl;

/////////////////////////////////////////////
// Including class infos

#include "HFCDebug.h"
#include "HFCStr.h"
#include "HFCGen.h"
#include "HFCExcp.h"
#include "HFCMem.h"
#include "HFCFunc.h"
#include "HFCNmsp.h"
#include "HFCArray.h"
#include "HFCList.h"

#include "HFCRc.h"
#include "HFCWin.h"
#include "HFCCtrl.h"
#include "HFCCmn.h"
#include "HFCDlgs.h"
#include "HFCCmnEx.h"
#include "HFCApp.h"
#include "HFCMath.h"
#include "HFCShell.h"
#include "HFCNetwork.h"

#include "HFCCom.h"

#if defined (WIN32) && !defined (HFC_NOFORCELIBS)
	#ifndef _CONSOLE
		#if defined(_DEBUG)
			#pragma comment(lib,"HFCLibd.lib")
		#elif defined(_DEBUG_LOGGING)
			#pragma comment(lib,"HFCLibl.lib")
		#else 
			#pragma comment(lib,"HFCLib.lib")
		#endif
	#else
		#if defined(_DEBUG)
			#pragma comment(lib,"HFCCond.lib")
		#elif defined(_DEBUG_LOGGING)
			#pragma comment(lib,"HFCConl.lib")
		#else 
			#pragma comment(lib,"HFCCon.lib")
		#endif
	#endif
	#pragma comment(lib, "kernel32.lib")
	#pragma comment(lib, "user32.lib")
	#pragma comment(lib, "gdi32.lib")
	#pragma comment(lib, "strsafe.lib")
#ifndef HFC_DISABLECOMDLG32
	#pragma comment(lib, "comdlg32.lib")
#endif
	#pragma comment(lib, "winspool.lib")
#ifndef HFC_DISABLEADVAPI32
	#pragma comment(lib, "advapi32.lib")
#endif
	#pragma comment(lib, "shell32.lib")
	#pragma comment(lib, "comctl32.lib")
#ifndef HFC_DISABLEOLE32
	#pragma comment(lib, "ole32.lib")
#endif
#ifndef HFC_DISABLEOLEAUT32
	#pragma comment(lib, "oleaut32.lib")
#endif
#ifndef HFC_DISABLEUUID32
	#pragma comment(lib, "uuid.lib")
#endif
#ifndef HFC_DISABLEVERSION32
	#pragma comment(lib, "version.lib")
#endif
#ifndef HFC_DISABLEHTMLHELP
	#pragma comment(lib,"htmlhelp.lib")
#endif
#endif

//Any errors
#ifdef DEF_APP
	#ifndef DEF_RESOURCES
		#error Resources required for applications
	#endif
#endif


#endif

