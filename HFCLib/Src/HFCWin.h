////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////


#ifndef HFCWINDOWS_H
#define HFCWINDOWS_H

#if defined (DEF_WINDOWS)

class CDC : public CObject
{
public:
	CDC();
	CDC(HDC hDC,HWND hWnd=NULL);
	CDC(CWnd* pWnd);
	virtual ~CDC();
	
	HDC m_hDC;
	HWND m_hWnd; 
	BOOL m_bPrinting;
	operator HDC() const;

	enum DeleteMethod { None, CallReleaseDC, CallDeleteDC };
	DeleteMethod m_nFreeMethod;

	
	void Attach(HDC hDC,HWND hWnd=NULL);
	void Attach(CDC& dc);
	void SetFreeMethod(DeleteMethod nFreeMethod) { m_nFreeMethod=nFreeMethod; }

	HGDIOBJ SelectStockObject(int nIndex);
	HGDIOBJ SelectObject(HGDIOBJ hGdiObj); 
	int SelectRegion(HRGN hRgn);

	HDC GetHandle() const;
	
	void SetWindow(HWND hWnd);
	HWND GetWindow() const;
	void ReleaseDC();
	void ReleaseDC(HWND hWnd);

	BOOL IsPrinting() const;

	HPEN GetCurrentPen() const;
	HBRUSH GetCurrentBrush() const;
	HPALETTE GetCurrentPalette() const;
	HFONT GetCurrentFont() const;
	HBITMAP GetCurrentBitmap() const;

	BOOL GetDC(HWND hWnd);
	BOOL CreateDC(LPCTSTR lpszDriverName,LPCTSTR lpszDeviceName,
		LPCTSTR lpszOutput,const void* lpInitData);
	BOOL CreateIC(LPCTSTR lpszDriverName, LPCTSTR lpszDeviceName,
		LPCTSTR lpszOutput,const void* lpInitData);
	BOOL CreateCompatibleDC(HDC hDC);

	BOOL DeleteDC();

	// Calls ReleaseDC or DeleteDC based on m_nFreeMethod
	void FreeDC(); 

	int SaveDC();
	BOOL RestoreDC(int nSavedDC);
	int GetDeviceCaps(int nIndex) const;
	UINT SetBoundsRect(LPCRECT lpRectBounds,UINT flags);
	UINT GetBoundsRect(LPRECT lpRectBounds,UINT flags);
	BOOL ResetDC(const DEVMODE* lpDevMode);

	CPoint GetBrushOrg() const;
	CPoint SetBrushOrg(int x, int y);
	CPoint SetBrushOrg(const POINT& point);
	int EnumObjects(int nObjectType,int (CALLBACK* lpfn)(LPVOID,LPARAM),LPARAM lpData);
		
	COLORREF GetNearestColor(COLORREF crColor) const;
	HPALETTE SelectPalette(HPALETTE hPalette,BOOL bForceBackground);
	UINT RealizePalette();
	void UpdateColors();

	COLORREF GetBkColor() const;
	int GetBkMode() const;
	int GetPolyFillMode() const;
	int GetROP2() const;
	int GetStretchBltMode() const;
	COLORREF GetTextColor() const;

	COLORREF SetBkColor(COLORREF crColor);
	int SetBkMode(int nBkMode);
	int SetPolyFillMode(int nPolyFillMode);
	int SetROP2(int nDrawMode);
	int SetStretchBltMode(int nStretchMode);
	COLORREF SetTextColor(COLORREF crColor);

	BOOL GetColorAdjustment(LPCOLORADJUSTMENT lpColorAdjust) const;
	BOOL SetColorAdjustment(const COLORADJUSTMENT* lpColorAdjust);

	int GetMapMode() const;
	CPoint GetViewportOrg() const;
	int SetMapMode(int nMapMode);
	CPoint SetViewportOrg(int x, int y);
	CPoint SetViewportOrg(const POINT& point);
	CPoint OffsetViewportOrg(int nWidth,int nHeight);

	CSize GetViewportExt() const;
	CSize SetViewportExt(int cx,int cy);
	CSize SetViewportExt(SIZE size);
	CSize ScaleViewportExt(int xNum,int xDenom,int yNum,int yDenom);

	CPoint GetWindowOrg() const;
	CPoint SetWindowOrg(int x,int y);
	CPoint SetWindowOrg(const POINT& point);
	CPoint OffsetWindowOrg(int nWidth,int nHeight);

	CSize GetWindowExt() const;
	CSize SetWindowExt(int cx,int cy);
	CSize SetWindowExt(const SIZE& size);
	CSize ScaleWindowExt(int xNum,int xDenom,int yNum,int yDenom);

	void DPtoLP(LPPOINT lpPoints,int nCount = 1) const;
	void DPtoLP(LPRECT lpRect) const;
	void DPtoLP(LPSIZE lpSize) const;
	void LPtoDP(LPPOINT lpPoints,int nCount = 1) const;
	void LPtoDP(LPRECT lpRect) const;
	void LPtoDP(LPSIZE lpSize) const;

	BOOL FillRgn(HRGN hRgn,HBRUSH hBrush);
	BOOL FrameRgn(HRGN hRgn,HBRUSH hBrush,int nWidth,int nHeight);
	BOOL InvertRgn(HRGN hRgn);
	BOOL PaintRgn(HRGN hRgn);

	int GetClipBox(LPRECT lpRect) const;
	BOOL PtVisible(int x,int y) const;
	BOOL PtVisible(const POINT& point) const;
	BOOL RectVisible(LPCRECT lpRect) const;
	int SelectClipRgn(HRGN hRgn);
	int ExcludeClipRect(int x1,int y1,int x2,int y2);
	int ExcludeClipRect(LPCRECT lpRect);
	int ExcludeUpdateRgn(HWND hWnd);
	int IntersectClipRect(int x1,int y1,int x2,int y2);
	int IntersectClipRect(LPCRECT lpRect);
	int OffsetClipRgn(int x, int y);
	int OffsetClipRgn(const SIZE& size);
	int SelectClipRgn(HRGN hRgn,int nMode);

	CPoint GetCurrentPosition() const;
	CPoint MoveTo(int x,int y);
	CPoint MoveTo(const POINT& point);
	BOOL LineTo(int x,int y);
	BOOL LineTo(const POINT& point);
	BOOL Arc(int x1,int y1,int x2,int y2,int x3,int y3,int x4,int y4);
	BOOL Arc(LPCRECT lpRect,const POINT& ptStart,const POINT& ptEnd);
	BOOL Polyline(LPPOINT lpPoints,int nCount);

	BOOL AngleArc(int x,int y,int nRadius,float fStartAngle,float fSweepAngle);
	BOOL ArcTo(int x1,int y1,int x2,int y2,int x3,int y3,int x4,int y4);
	BOOL ArcTo(LPCRECT lpRect,const POINT& ptStart,const POINT& ptEnd);
	int GetArcDirection() const;
	int SetArcDirection(int nArcDirection);

	BOOL PolyDraw(const POINT* lpPoints,const BYTE* lpTypes,int nCount);
	BOOL PolylineTo(const POINT* lpPoints,int nCount);
	BOOL PolyPolyline(const POINT* lpPoints,const DWORD* lpPolyPoints,int nCount);

	BOOL PolyBezier(const POINT* lpPoints, int nCount);
	BOOL PolyBezierTo(const POINT* lpPoints, int nCount);

	void FillRect(LPCRECT lpRect,HBRUSH hBrush);
	void FrameRect(LPCRECT lpRect,HBRUSH hBrush);
	void InvertRect(LPCRECT lpRect);
	BOOL DrawIcon(int x,int y,HICON hIcon);
	BOOL DrawIcon(const POINT& point,HICON hIcon);
	BOOL DrawIcon(int xLeft,int yTop,HICON hIcon,int cxWidth, int cyWidth,UINT istepIfAniCur,HBRUSH hbrFlickerFreeDraw,UINT diFlags);
	BOOL DrawIcon(const POINT& point,HICON hIcon,const SIZE& sz,UINT istepIfAniCur,HBRUSH hbrFlickerFreeDraw,UINT diFlags);

	BOOL DrawState(const POINT& pt,const SIZE& size,HGDIOBJ hGdiObj,UINT nFlags,HBRUSH hBrush=NULL);
	BOOL DrawState(const CPoint& pt,const CSize& size,CBitmap* pBitmap,UINT nFlags,HBRUSH hBrush=NULL);
	BOOL DrawState(const CPoint& pt,const CSize& size,LPCSTR lpszText,UINT nFlags,BOOL bPrefixText=TRUE,int nTextLen=0,HBRUSH hBrush=NULL);
	BOOL DrawState(const CPoint& pt,const CSize& size,DRAWSTATEPROC lpDrawProc,LPARAM lData,UINT nFlags,HBRUSH hBrush=NULL);
	
	BOOL Chord(int x1,int y1,int x2,int y2,int x3,int y3,int x4,int y4);
	BOOL Chord(LPCRECT lpRect,const POINT& ptStart,const POINT& ptEnd);
	void DrawFocusRect(LPCRECT lpRect);
	BOOL Ellipse(int x1,int y1,int x2,int y2);
	BOOL Ellipse(LPCRECT lpRect);
	BOOL Pie(int x1,int y1,int x2,int y2,int x3,int y3,int x4,int y4);
	BOOL Pie(LPCRECT lpRect,const POINT& ptStart,const POINT& ptEnd);
	BOOL Polygon(LPPOINT lpPoints,int nCount);
	BOOL PolyPolygon(LPPOINT lpPoints,LPINT lpPolyCounts,int nCount);
	BOOL Rectangle(int x1,int y1,int x2,int y2);
	BOOL Rectangle(LPCRECT lpRect);
	BOOL RoundRect(int x1,int y1,int x2,int y2,int x3,int y3);
	BOOL RoundRect(LPCRECT lpRect,const POINT& point);

	BOOL PatBlt(int x,int y,int nWidth,int nHeight,DWORD dwRop);
	BOOL BitBlt(int x,int y,int nWidth,int nHeight,HDC hSrcDC,int xSrc,int ySrc,DWORD dwRop);
	BOOL StretchBlt(int x,int y,int nWidth,int nHeight,HDC hSrcDC,int xSrc,
		int ySrc,int nSrcWidth,int nSrcHeight,DWORD dwRop);
	COLORREF GetPixel(int x,int y) const;
	COLORREF GetPixel(const POINT& point) const;
	COLORREF SetPixel(int x,int y,COLORREF crColor);
	COLORREF SetPixel(const POINT& point,COLORREF crColor);
	BOOL FloodFill(int x,int y,COLORREF crColor);
	BOOL ExtFloodFill(int x,int y,COLORREF crColor,UINT nFillType);
	BOOL MaskBlt(int x,int y,int nWidth,int nHeight,HDC hSrcDC,
		int xSrc,int ySrc,HBITMAP maskBitmap,int xMask,int yMask,DWORD dwRop);
	BOOL PlgBlt(LPPOINT lpPoint,HDC hSrcDC,int xSrc,int ySrc,
		int nWidth,int nHeight,HBITMAP maskBitmap,int xMask,int yMask);
	BOOL SetPixelV(int x,int y,COLORREF crColor);
	BOOL SetPixelV(const POINT& point,COLORREF crColor);

	BOOL TextOut(int x,int y,LPCTSTR lpszString,int nCount);
	BOOL TextOut(int x,int y,const CStringA& str);

	BOOL ExtTextOut(int x,int y,UINT nOptions,LPCRECT lpRect,
		LPCSTR lpszString,UINT nCount,LPINT lpDxWidths);
	BOOL ExtTextOut(int x,int y,UINT nOptions,LPCRECT lpRect,
		const CStringA& str,LPINT lpDxWidths);
	CSize TabbedTextOut(int x,int y,LPCTSTR lpszString,int nCount,
		int nTabPositions,LPINT lpnTabStopPositions,int nTabOrigin);
	CSize TabbedTextOut(int x,int y,const CStringA& str,int nTabPositions,
		LPINT lpnTabStopPositions,int nTabOrigin);
	int DrawText(LPCSTR lpszString,int nCount,LPRECT lpRect,UINT nFormat);
	int DrawText(const CStringA& str,LPRECT lpRect,UINT nFormat);
	
	CSize GetTextExtent(LPCSTR lpszString,int nCount) const;
	CSize GetTextExtent(const CStringA& str) const;
	CSize GetOutputTextExtent(LPCTSTR lpszString,int nCount) const;
	CSize GetOutputTextExtent(const CStringA& str) const;
	CSize GetTabbedTextExtent(LPCTSTR lpszString,int nCount,
		int nTabPositions,LPINT lpnTabStopPositions) const;
	CSize GetTabbedTextExtent(const CStringA& str,
		int nTabPositions,LPINT lpnTabStopPositions) const;
	CSize GetOutputTabbedTextExtent(LPCTSTR lpszString, int nCount,
		int nTabPositions,LPINT lpnTabStopPositions) const;
	CSize GetOutputTabbedTextExtent(const CStringA& str,
		int nTabPositions,LPINT lpnTabStopPositions) const;
	BOOL GrayString(HBRUSH hBrush,
		BOOL (CALLBACK* lpfnOutput)(HDC,LPARAM,int),LPARAM lpData,
			int nCount,int x,int y,int nWidth,int nHeight);
	UINT GetTextAlign() const;
	UINT SetTextAlign(UINT nFlags);
	int GetTextFace(int nCount,LPTSTR lpszFacename) const;
	int GetTextFace(CStringA& rString) const;
	BOOL GetTextMetrics(LPTEXTMETRIC lpMetrics) const;
	BOOL GetOutputTextMetrics(LPTEXTMETRIC lpMetrics) const;
	int SetTextJustification(int nBreakExtra,int nBreakCount);
	int GetTextCharacterExtra() const;
	int SetTextCharacterExtra(int nCharExtra);

	BOOL DrawEdge(LPRECT lpRect,UINT nEdge,UINT nFlags);
	BOOL DrawFrameControl(LPRECT lpRect,UINT nType,UINT nState);

	BOOL ScrollDC(int dx,int dy,LPCRECT lpRectScroll,LPCRECT lpRectClip,
		HRGN hRgnUpdate,LPRECT lpRectUpdate);

	BOOL GetCharWidth(UINT nFirstChar,UINT nLastChar,LPINT lpBuffer) const;
	BOOL GetOutputCharWidth(UINT nFirstChar,UINT nLastChar,LPINT lpBuffer) const;
	DWORD SetMapperFlags(DWORD dwFlag);
	CSize GetAspectRatioFilter() const;

	BOOL GetCharABCWidths(UINT nFirstChar,UINT nLastChar,LPABC lpabc) const;
	DWORD GetFontData(DWORD dwTable,DWORD dwOffset,LPVOID lpData,DWORD cbData) const;
	int GetKerningPairs(int nPairs,LPKERNINGPAIR lpkrnpair) const;
	UINT GetOutlineTextMetrics(UINT cbData,LPOUTLINETEXTMETRIC lpotm) const;
	DWORD GetGlyphOutline(UINT nChar,UINT nFormat,LPGLYPHMETRICS lpgm,
		DWORD cbBuffer, LPVOID lpBuffer,const MAT2* lpmat2) const;

	BOOL GetCharABCWidths(UINT nFirstChar,UINT nLastChar,
		LPABCFLOAT lpABCF) const;
	BOOL GetCharWidth(UINT nFirstChar,UINT nLastChar,
		float* lpFloatBuffer) const;

	int Escape(int nEscape,int nCount,LPCTSTR lpszInData,LPVOID lpOutData);
	int Escape(int nEscape,int nInputSize,LPCTSTR lpszInputData,
		int nOutputSize,LPTSTR lpszOutputData);
	int DrawEscape(int nEscape,int nInputSize,LPCTSTR lpszInputData);

	int StartDoc(LPCTSTR lpszDocName);
	int StartDoc(LPDOCINFO lpDocInfo);
	int StartPage();
	int EndPage();
	int SetAbortProc(BOOL (CALLBACK* lpfn)(HDC, int));
	int AbortDoc();
	int EndDoc();

	BOOL PlayMetaFile(HMETAFILE hMF);
	BOOL PlayMetaFile(HENHMETAFILE hEnhMetaFile,LPCRECT lpBounds);
	BOOL AddMetaFileComment(UINT nDataSize,const BYTE* pCommentData);
	
	BOOL AbortPath();
	BOOL BeginPath();
	BOOL CloseFigure();
	BOOL EndPath();
	BOOL FillPath();
	BOOL FlattenPath();
	BOOL StrokeAndFillPath();
	BOOL StrokePath();
	BOOL WidenPath();
	float GetMiterLimit() const;
	BOOL SetMiterLimit(float fMiterLimit);
	int GetPath(LPPOINT lpPoints,LPBYTE lpTypes,int nCount) const;
	BOOL SelectClipPath(int nMode);

#ifdef DEF_WCHAR
	//Widechar support
	BOOL TextOut(int x,int y,const CStringW& str);
	BOOL ExtTextOut(int x,int y,UINT nOptions,LPCRECT lpRect,
		LPCWSTR lpszString,UINT nCount,LPINT lpDxWidths);
	BOOL ExtTextOut(int x,int y,UINT nOptions,LPCRECT lpRect,
		const CStringW& str,LPINT lpDxWidths);
	CSize TabbedTextOut(int x,int y,const CStringW& str,int nTabPositions,
		LPINT lpnTabStopPositions,int nTabOrigin);
	BOOL DrawState(const CPoint& pt,const CSize& size,LPCWSTR lpszText,UINT nFlags,BOOL bPrefixText=TRUE,int nTextLen=0,HBRUSH hBrush=NULL);
	int DrawText(LPCWSTR lpszString,int nCount,LPRECT lpRect,UINT nFormat);
	int DrawText(const CStringW& str,LPRECT lpRect,UINT nFormat);
	CSize GetTextExtent(LPCWSTR lpszString,int nCount) const;
	CSize GetTextExtent(const CStringW& str) const;
	CSize GetOutputTextExtent(const CStringW& str) const;
	CSize GetTabbedTextExtent(const CStringW& str,
		int nTabPositions,LPINT lpnTabStopPositions) const;
	CSize GetOutputTabbedTextExtent(const CStringW& str,
		int nTabPositions,LPINT lpnTabStopPositions) const;
	int GetTextFace(CStringW& rString) const;
#endif

};

class CPaintDC : public CDC
{
protected:
	HWND m_hWnd;
public:
	PAINTSTRUCT m_ps;

public:
	CPaintDC(CWnd * pWnd);
	virtual ~CPaintDC();
};

class CMenu : public CObject
{
public:
	CMenu();
	CMenu(HMENU hMenu);
	
	BOOL CreateMenu();
	BOOL CreatePopupMenu();
#ifdef DEF_RESOURCES
	BOOL LoadMenu(LPCSTR lpszResourceName);
	BOOL LoadMenu(UINT nIDResource);
#endif
	BOOL LoadMenuIndirect(const void* lpMenuTemplate);
	BOOL DestroyMenu();
	
	HMENU m_hMenu;
	operator HMENU() const;
	
	BOOL DeleteMenu(UINT nPosition, UINT nFlags);
	BOOL TrackPopupMenu(UINT nFlags,int x,int y,HWND hWnd,LPCRECT lpRect=0);

	BOOL AppendMenu(UINT nFlags,UINT nIDNewItem=0,LPCSTR lpszNewItem=NULL);
	BOOL AppendMenu(UINT nFlags,UINT nIDNewItem,const CBitmap* pBmp);
	UINT CheckMenuItem(UINT nIDCheckItem,UINT nCheck);
	UINT EnableMenuItem(UINT nIDEnableItem,UINT nEnable);
	UINT GetMenuItemCount() const;
	UINT GetMenuItemID(int nPos) const;
	UINT GetMenuState(UINT nID,UINT nFlags) const;
	
	int GetMenuString(UINT nIDItem,LPSTR lpString,int nMaxCount,UINT nFlags) const;
	int GetMenuString(UINT nIDItem,CStringA& rString,UINT nFlags) const;
	HMENU GetSubMenu(int nPos) const;
	BOOL InsertMenu(UINT nPosition,UINT nFlags,UINT nIDNewItem=0,LPCSTR lpszNewItem=NULL);
	BOOL InsertMenu(UINT nPosition,UINT nFlags,UINT nIDNewItem,const CBitmap* pBmp);
	BOOL InsertMenu(UINT nItem,BOOL fByPosition,LPMENUITEMINFO lpmii);
	BOOL ModifyMenu(UINT nPosition,UINT nFlags,UINT nIDNewItem=0,LPCTSTR lpszNewItem=NULL);
	BOOL ModifyMenu(UINT nPosition,UINT nFlags,UINT nIDNewItem,const CBitmap* pBmp);
	BOOL RemoveMenu(UINT nPosition,UINT nFlags);
	BOOL SetMenuItemBitmaps(UINT nPosition,UINT nFlags,const CBitmap* pBmpUnchecked, const CBitmap* pBmpChecked);
	BOOL CheckMenuRadioItem(UINT nIDFirst,UINT nIDLast,UINT nIDItem,UINT nFlags);

	BOOL GetMenuItemInfo(UINT uItem,BOOL fByPosition,LPMENUITEMINFO lpmii) const;
	BOOL SetMenuItemInfo(UINT uItem,BOOL fByPosition,LPMENUITEMINFO lpmii);
	UINT GetMenuDefaultItem(UINT fByPos,UINT gmdiFlags) const;
	BOOL SetMenuDefaultItem(UINT uItem,UINT fByPos);
#ifdef DEF_WCHAR
	// For widechar support
	int GetMenuString(UINT nIDItem,LPWSTR lpString,int nMaxCount,UINT nFlags) const;
	int GetMenuString(UINT nIDItem,CStringW& rString,UINT nFlags) const;
	
	BOOL AppendMenu(UINT nFlags,UINT nIDNewItem,LPCWSTR lpszNewItem);
	BOOL InsertMenu(UINT nPosition,UINT nFlags,UINT nIDNewItem,LPCWSTR lpszNewItem);
	BOOL ModifyMenu(UINT nPosition,UINT nFlags,UINT nIDNewItem,LPCWSTR lpszNewItem);
	BOOL InsertMenu(UINT nItem,BOOL fByPosition,LPMENUITEMINFOW lpmii);
	BOOL GetMenuItemInfo(UINT uItem,BOOL fByPosition,LPMENUITEMINFOW lpmii) const;
	BOOL SetMenuItemInfo(UINT uItem,BOOL fByPosition,LPMENUITEMINFOW lpmii);

#endif
};

// For GetWindowLong/SetWindowLong
#ifdef _WIN64
#define LONGPTR	LONG_PTR 
#else
#define LONGPTR	LONG 
#endif

class CWnd : public CObject
{
public:
	enum ShowState { 
		swHide=SW_HIDE,
		swShowNormal=SW_SHOWNORMAL,
		swNormal=SW_NORMAL,
		swShowMinimized=SW_SHOWMINIMIZED,
		swShowMaximized=SW_SHOWMAXIMIZED,
		swMaximize=SW_MAXIMIZE,
		swShowNoActivate=SW_SHOWNOACTIVATE,
		swShow=SW_SHOW,
		swMinimize=SW_MINIMIZE,
		swShowMinNiActive=SW_SHOWMINNOACTIVE,
		swShowNA=SW_SHOWNA,
		swRestore=SW_RESTORE,
		swShowDefault=SW_SHOWDEFAULT,
		swForceMinimize=SW_FORCEMINIMIZE,
		swMax=SW_MAX
	};

	enum PositionFlags {
		fgAllowMinimized = 0x1,
		fgAllowMaximized = 0x2,
		fgAllowHide = 0x4,
		fgOnlyNormalPosition = 0x8,
		fgOnlySpecifiedPosition = 0x10,
		fgNoSize = 0x20
	};

	enum WindowLongIndex {
		gwlWndProc=GWLP_WNDPROC,
		gwlHInstance=GWLP_HINSTANCE,
		gwlHWndParent=GWLP_HWNDPARENT,
		gwlStyle=GWL_STYLE,
		gwlExStyle=GWL_EXSTYLE,
		gwlUserData=GWLP_USERDATA,
		gwlID=GWL_ID,
		dwlMsgResult=DWLP_MSGRESULT,
		dwlDlgProc=DWLP_DLGPROC,
		dwlUser=DWLP_USER
	};

	enum ClassLongIndex {
		gclMenuName=GCLP_MENUNAME,
		gclHBRBackGround=GCLP_HBRBACKGROUND,
		gclHCursor=GCLP_HCURSOR,
		gclHIcon=GCLP_HICON,
		gclHIconSm=GCLP_HICONSM,
		gclHModule=GCLP_HMODULE,
		gclCBWndExtra=GCL_CBWNDEXTRA,
		gclCBClsExtra=GCL_CBCLSEXTRA,
		gclWndProc=GCLP_WNDPROC,
		gclStyle=GCL_STYLE,
		gcwAtom=GCW_ATOM
	};

public:
	CWnd(HWND hWnd=NULL) { m_hWnd=hWnd; }

	HWND GetHandle() const { return m_hWnd; }
	void Attach(HWND hWnd) { m_hWnd=hWnd; }
	void AttachToDlgItem(HWND hDialog,int nID) { m_hWnd=::GetDlgItem(hDialog,nID); }
	operator HWND() const { return m_hWnd; }
	
	BOOL operator==(const CWnd& wnd) const { return (m_hWnd==wnd.m_hWnd); }
	BOOL operator!=(const CWnd& wnd) const { return (m_hWnd!=wnd.m_hWnd); }
	
	DWORD GetStyle() const { return ::GetWindowLong(m_hWnd,GWL_STYLE); }
	DWORD SetStyle(DWORD dwStyle) { return ::SetWindowLong(m_hWnd,GWL_STYLE,dwStyle); }
	BOOL ModifyStyle(DWORD dwRemove,DWORD dwAdd,UINT nFlags=0);
	DWORD GetExStyle() const { return ::GetWindowLong(m_hWnd,GWL_EXSTYLE); }
	DWORD SetExStyle(DWORD dwExStyle) { return ::SetWindowLong(m_hWnd,GWL_EXSTYLE,dwExStyle); }
	BOOL ModifyStyleEx(DWORD dwRemove,DWORD dwAdd,UINT nFlags=0);

	LONGPTR GetWindowLong(WindowLongIndex nIndex) const;
	LONGPTR SetWindowLong(WindowLongIndex nIndex,LONGPTR lNewLong);
	LONGPTR GetClassLong(ClassLongIndex nIndex) const;
	LONGPTR SetClassLong(ClassLongIndex nIndex,LONGPTR lNewVal);

	BOOL Create(LPCTSTR lpszClassName,
		LPCTSTR lpszWindowName, DWORD dwStyle,
		const RECT* rect,
		HWND hParentWnd, UINT nID);

	BOOL CreateEx(DWORD dwExStyle, LPCTSTR lpszClassName,
		LPCTSTR lpszWindowName, DWORD dwStyle,
		int x, int y, int nWidth, int nHeight,
		HWND hWndParent, UINT nID, LPVOID lpParam = NULL);

	BOOL CreateEx(DWORD dwExStyle, LPCTSTR lpszClassName,
		LPCTSTR lpszWindowName, DWORD dwStyle,
		const RECT* rect,
		HWND hParentWnd, UINT nID,
		LPVOID lpParam = NULL);

	BOOL DestroyWindow();
	
	int GetDlgCtrlID() const {return ::GetDlgCtrlID(m_hWnd);}
	int SetDlgCtrlID(int nID) {return ::SetWindowLong(m_hWnd,GWL_ID,(LONG)nID);}
	HWND GetParent() const { return ::GetParent(m_hWnd); }
	HWND SetParent(HWND hwndNewParent) {return ::SetParent(m_hWnd,hwndNewParent);}
	
	BOOL PostMessage(UINT uMsg,WPARAM wParam=0,LPARAM lParam=0) const { return ::PostMessage(m_hWnd,uMsg,wParam,lParam); }
	LRESULT SendMessage(UINT uMsg,WPARAM wParam=0,LPARAM lParam=0) const { return ::SendMessage(m_hWnd,uMsg,wParam,lParam); }
	
	BOOL SetWindowText(LPCSTR lpsz) {return ::SetWindowText(m_hWnd,lpsz); }
	int GetWindowText(LPSTR lpString,int nMaxCount) const { return ::GetWindowText(m_hWnd,lpString,nMaxCount); }
	int GetWindowText(CString& str) const;
	int GetWindowTextLength() const { return ::GetWindowTextLength(m_hWnd); }
	void SetFont(HFONT hFont,BOOL bRedraw=TRUE) { ::SendMessage(m_hWnd,WM_SETFONT,(WPARAM)hFont,MAKELPARAM(bRedraw,0)); }
	HFONT GetFont() const { return (HFONT)::SendMessage(m_hWnd,WM_GETFONT,0,0); }
	
	int GetClassName(LPSTR lpString,int nMaxCount) const { return ::GetClassName(m_hWnd,lpString,nMaxCount); }
	
	void MoveWindow(int x,int y,int nWidth,int nHeight,BOOL bRepaint=TRUE) { ::MoveWindow(m_hWnd,x,y,nWidth,nHeight,bRepaint); }
	void MoveWindow(LPCRECT lpRect,BOOL bRepaint=TRUE) {::MoveWindow(m_hWnd,lpRect->left,lpRect->top,lpRect->right-lpRect->left,lpRect->bottom-lpRect->top,bRepaint);}
	int SetWindowRgn(HRGN hRgn, BOOL bRedraw) { return ::SetWindowRgn(m_hWnd,hRgn,bRedraw); }
	int GetWindowRgn(HRGN hRgn) const {return ::GetWindowRgn(m_hWnd,hRgn);}
	BOOL SetWindowPos(HWND hWndInsertAfter,int x,int y,int cx,int cy,UINT nFlags) {return ::SetWindowPos(m_hWnd,hWndInsertAfter,x,y,cx,cy,nFlags);}
	void GetWindowRect(LPRECT lpRect) const { ::GetWindowRect(m_hWnd,lpRect); }
	void GetClientRect(LPRECT lpRect) const { ::GetClientRect(m_hWnd,lpRect); }

	void ClientToScreen(LPPOINT lpPoint) const {::ClientToScreen(m_hWnd,lpPoint); }
	void ClientToScreen(LPRECT lpRect) const;
	void ScreenToClient(LPPOINT lpPoint) const { ::ScreenToClient(m_hWnd,lpPoint); }
	void ScreenToClient(LPRECT lpRect) const;
	void MapWindowPoints(HWND hwndTo, LPPOINT lpPoint, UINT nCount) const { ::MapWindowPoints(m_hWnd,hwndTo,lpPoint,nCount); }
	void MapWindowPoints(HWND hwndTo, LPRECT lpRect) const;

	BOOL GetWindowPlacement(WINDOWPLACEMENT* lpwndpl) const;
	BOOL SetWindowPlacement(const WINDOWPLACEMENT* lpwndpl);

	BOOL IsIconic() const { return ::IsIconic(m_hWnd); }
	BOOL IsZoomed() const { return ::IsZoomed(m_hWnd); }
	UINT ArrangeIconicWindows() { return ::ArrangeIconicWindows(m_hWnd); }
	void BringWindowToTop() { ::BringWindowToTop(m_hWnd); }
	
	BOOL GetUpdateRect(LPRECT lpRect,BOOL bErase=FALSE);
	int GetUpdateRgn(HRGN hRgn,BOOL bErase=FALSE);
	void Invalidate(BOOL bErase=TRUE);
	BOOL InvalidateRect(CONST RECT* lpRect,BOOL bErase);
	void InvalidateRgn(HRGN hRgn,BOOL bErase=TRUE);
	BOOL ShowWindow(ShowState nCmdShow) const { return ::ShowWindow(m_hWnd,nCmdShow); }
	BOOL IsWindowVisible() const { return ::IsWindowVisible(m_hWnd); }

	BOOL RedrawWindow(LPCRECT lpRectUpdate=NULL,HRGN hrgnUpdate=NULL,UINT flags=RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE) { return ::RedrawWindow(m_hWnd,lpRectUpdate,hrgnUpdate,flags); }
	BOOL UpdateWindow() const { return ::UpdateWindow(m_hWnd); }
	

	HDC GetDC() const { return ::GetDC(m_hWnd); }
	CDC* GetCDC() { return new CDC(this); }
	HDC GetWindowDC() const { return ::GetWindowDC(m_hWnd); }
	int ReleaseDC(HDC hDC) { return ::ReleaseDC(m_hWnd,hDC); }
	void ReleaseCDC(CDC* pDC) { delete pDC; }
	

	BOOL IsWindowEnabled() const { return ::IsWindowEnabled(m_hWnd); }
	BOOL EnableWindow(BOOL bEnable=TRUE) { return ::EnableWindow(m_hWnd,bEnable); }
	HWND SetFocus() const { return ::SetFocus(m_hWnd); }
	static HWND GetFocus();
	BOOL ForceForegroundAndFocus();
	
	int GetTextLength() const { return (UINT)::SendMessage(m_hWnd,WM_GETTEXTLENGTH,0,0); } 
	int GetText(LPSTR lpszText,int cchTextMax) const { return (UINT)::SendMessage(m_hWnd,WM_GETTEXT,(WPARAM)cchTextMax,(LPARAM)lpszText); } 
	int GetText(CStringA& str) const;
	BOOL SetText(LPCSTR lpsz) { return (BOOL)::SendMessage(m_hWnd,WM_SETTEXT,0,(LPARAM)lpsz); }

	static HWND GetDesktopWindow();

	static HWND GetActiveWindow();
	HWND SetActiveWindow();
	BOOL SetForegroundWindow();
	static HWND GetForegroundWindow();
	static HWND GetCapture();
	HWND SetCapture();

	HICON SetIcon(HICON hIcon,BOOL bBigIcon);
	HICON GetIcon(BOOL bBigIcon) const;


	BOOL SetMenu(HMENU hMenu) { return ::SetMenu(m_hWnd,hMenu); }
	HMENU GetMenu() const { return ::GetMenu(m_hWnd); }
	HMENU GetSystemMenu(BOOL bRevert=FALSE) const { return ::GetSystemMenu(m_hWnd,bRevert); }

	void ShowOwnedPopups(BOOL bShow=TRUE);
	BOOL EnableScrollBar(int nSBFlags,UINT nArrowFlags=ESB_ENABLE_BOTH);

	UINT SetTimer(UINT idTimer,UINT uTimeout,TIMERPROC tmprc=NULL);	
	BOOL KillTimer(UINT uIDEvent);

	HWND GetDlgItem(int nID) const { return ::GetDlgItem(m_hWnd,nID); }
	BOOL IsDialogMessage(LPMSG lpMsg) { return ::IsDialogMessage(m_hWnd,lpMsg); }
	BOOL SetDlgItemPos(int iDlgItemID,HWND hWndInsertAfter,int x,int y,int cx,int cy,UINT nFlags) {return ::SetWindowPos(::GetDlgItem(m_hWnd,iDlgItemID),hWndInsertAfter,x,y,cx,cy,nFlags);}
	BOOL UpdateDlgItem(int iDlgItemID) const { return ::UpdateWindow(::GetDlgItem(m_hWnd,iDlgItemID)); }
	BOOL IsDlgItemEnabled(int nDlgItemID) const { return ::IsWindowEnabled(::GetDlgItem(m_hWnd,nDlgItemID)); }
	BOOL EnableDlgItem(int nDlgItemID,BOOL bEnable=TRUE) { return ::EnableWindow(::GetDlgItem(m_hWnd,nDlgItemID),bEnable); }
	BOOL ShowDlgItem(int nDlgItemID,ShowState nCmdShow) const { return ::ShowWindow(::GetDlgItem(m_hWnd,nDlgItemID),nCmdShow); }
	HWND SetFocus(int nDlgItemID) const { return ::SetFocus(::GetDlgItem(m_hWnd,nDlgItemID)); }
	
	
	BOOL CheckDlgButton(int nIDButton,UINT uCheck) const;
	void CheckRadioButton(int nIDFirstButton,int nIDLastButton,int nIDCheckButton);
	int GetCheckedRadioButton(int nIDFirstButton,int nIDLastButton);

	UINT GetDlgItemInt(int nIDDlgItem,BOOL* lpTranslated=NULL,BOOL bSigned=TRUE) const;
	UINT GetDlgItemText(int nIDDlgItem,LPSTR lpString,int nMaxCount) const;	
	UINT GetDlgItemText(int nIDDlgItem,CStringA& str) const;
	UINT GetDlgItemTextLength(int nIDDlgItem) const;

	HWND GetNextDlgGroupItem(HWND hWndCtl,BOOL bPrevious=FALSE) const;
	HWND GetNextDlgTabItem(HWND hWndCtl,BOOL bPrevious=FALSE) const;
	UINT IsDlgButtonChecked(int nIDButton) const;
	LRESULT SendDlgItemMessage(int idControl,UINT uMsg,WPARAM wParam=0,LPARAM lParam=0) const;
	BOOL SetDlgItemInt(int idControl,UINT uValue,BOOL fSigned=TRUE) const;
	BOOL SetDlgItemText(int idControl,LPCSTR lpsz) const;	
	LONG GetDlgItemStyle(int idControl) const { return ::GetWindowLong(GetDlgItem(idControl),GWL_STYLE); }
	LONG SetDlgItemStyle(int idControl,LONG lStyle) const { return ::SetWindowLong(GetDlgItem(idControl),GWL_STYLE,lStyle); }
	
	void DragAcceptFiles(BOOL bAccept);
	BOOL OpenClipboard();

	void CenterWindow();
	BOOL LoadPosition(HKEY hRootKey,LPCSTR lpKey,LPCSTR lpSubKey,DWORD fFlags);
	BOOL SavePosition(HKEY hRootKey,LPCSTR lpKey,LPCSTR lpSubKey) const;

	int MessageBox(LPCSTR lpText,LPCSTR lpCaption=NULL,UINT uType=MB_OK);
#ifdef DEF_RESOURCES
	int ShowErrorMessage(UINT nIDMsgStr,UINT nIDTitleStr=0,UINT uType=MB_OK|MB_ICONSTOP) const;
#endif
	int ReportSystemError(LPCSTR szTitle=NULL,DWORD dwError=DWORD(-1),DWORD dwExtra=0,LPCSTR szPrefix=NULL);

	HWND HtmlHelp(UINT uCommand,DWORD_PTR dwData,LPCSTR szHelpFile=NULL);
	

#ifdef DEF_WCHAR
	BOOL SetWindowText(LPCWSTR lpsz);
	int GetWindowText(LPWSTR lpString,int nMaxCount) const;
	int GetWindowText(CStringW& str) const;

	int GetClassName(LPWSTR lpString,int nMaxCount) const;

	UINT GetDlgItemText(int nIDDlgItem,CStringW& str);
	UINT GetDlgItemText(int nIDDlgItem,LPWSTR lpString,int nMaxCount) const;	
	BOOL SetDlgItemText(int idControl,LPCWSTR lpsz) const;	
	//widechar support
	int GetText(CStringW& str) const;
	int GetText(LPWSTR lpszText,int cchTextMax) const;
	BOOL SetText(LPCWSTR lpsz);

	BOOL PostMessageW(UINT uMsg,WPARAM wParam=0,LPARAM lParam=0) const { return ::PostMessageW(m_hWnd,uMsg,wParam,lParam); }
	LRESULT SendMessageW(UINT uMsg,WPARAM wParam=0,LPARAM lParam=0) const { return ::SendMessageW(m_hWnd,uMsg,wParam,lParam); }
	LRESULT SendDlgItemMessageW(int idControl,UINT uMsg,WPARAM wParam=0,LPARAM lParam=0) const;

	int MessageBox(LPCWSTR lpText,LPCWSTR lpCaption=NULL,UINT uType=MB_OK);
	static int MessageBox(HWND hWnd,LPCWSTR lpText,LPCWSTR lpCaption=NULL,UINT uType=MB_OK);


	HWND HtmlHelp(UINT uCommand,DWORD_PTR dwData,LPCWSTR szHelpFile);
#endif

protected:
	HWND m_hWnd;
};

class CTargetWnd : public CWnd
{
public:
	CTargetWnd (HWND hWnd=NULL);
	virtual ~CTargetWnd();

public:

	virtual BOOL Create(LPCTSTR lpszClassName,
		LPCTSTR lpszWindowName, DWORD dwStyle,
		const RECT* rect,
		HWND hParentWnd, UINT nID);

	BOOL CreateEx(DWORD dwExStyle, LPCTSTR lpszClassName,
		LPCTSTR lpszWindowName, DWORD dwStyle,
		int x, int y, int nWidth, int nHeight,
		HWND hWndParent, HMENU nIDorHMenu, LPVOID lpParam = NULL);

	BOOL CreateEx(DWORD dwExStyle, LPCTSTR lpszClassName,
		LPCTSTR lpszWindowName, DWORD dwStyle,
		const RECT* rect,
		HWND hParentWnd, UINT nID,
		LPVOID lpParam = NULL);

	virtual BOOL DestroyWindow();
		
	
	virtual int GetText(LPSTR lpszText,int cchTextMax) const;
	virtual int GetText(CStringA& str) const;
	virtual int GetTextLength() const;
	virtual BOOL SetText(LPCSTR lpsz);

		
	
	

#ifdef DEF_WCHAR
	//widechar support
	virtual BOOL SetText(LPCWSTR lpsz);
	virtual int GetText(LPWSTR lpszText,int cchTextMax) const;
	virtual int GetText(CStringW& str) const;
#endif	

public:
	//Message handlers
	virtual void OnActivate(WORD fActive,BOOL fMinimized,HWND hwnd);
	virtual void OnActivateApp(BOOL fActive,DWORD dwThreadID); 
	virtual void OnCancelMode();
	virtual void OnCaptureChanged(HWND hwndNewCapture);
	virtual void OnChangeCbChain(HWND hWndRemove,HWND hWndAfter );
	virtual void OnChar(TCHAR chCharCode,DWORD lKeyData);
	virtual BOOL OnClose();
	virtual BOOL OnCommand(WORD wID,WORD wNotifyCode,HWND hControl);
	virtual void OnContextMenu(HWND hWnd,CPoint& pos);
	virtual int OnCreate(LPCREATESTRUCT lpcs);
	virtual void OnDestroy();
	virtual void OnDestroyClipboard();
	virtual void OnDrawClipboard();
	virtual void OnDrawItem(UINT idCtl,LPDRAWITEMSTRUCT lpdis);
	virtual void OnDropFiles(HDROP hDrop);
	virtual void OnHelp(LPHELPINFO lphi);
	virtual void OnHScroll(UINT nSBCode,UINT nPos,HWND hScrollBar);
	virtual void OnInitMenu(HMENU hMenu);
	virtual void OnInitMenuPopup(HMENU hPopupMenu,UINT nIndex,BOOL bSysMenu);
	virtual void OnKeyDown(int nVirtKey,LONG lKeyData);
	virtual void OnKeyUp(int nVirtKey,LONG lKeyData);
	virtual void OnKillFocus(HWND hwndGetFocus);
	virtual void OnMeasureItem(int nIDCtl,LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual LRESULT OnMenuChar(UINT nChar,UINT nFlags,HMENU hMenu);
	virtual void OnMenuSelect(UINT uItem,UINT fuFlags,HMENU hmenu);     
	virtual int OnMouseActivate(HWND hDesktopWnd,UINT nHitTest,UINT message);
	virtual void OnMouseMove(UINT fwKeys,WORD xPos,WORD yPos);
	virtual void OnMove(int x, int y);
	virtual BOOL OnNcActivate(BOOL bActive);
	virtual BOOL OnNcCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnNcDestroy();
	virtual BOOL OnNotify(int idCtrl,LPNMHDR pnmh);
	virtual void OnPaint();
	virtual void OnPaintClipboard(HWND hClipAppWnd,HGLOBAL hPaintStruct);
	virtual void OnSize(UINT nType, int cx, int cy);	
	virtual BOOL OnSizing(UINT nSide,LPRECT lpRect);
	virtual void OnSysCommand(UINT nID,LPARAM lParam);
	virtual void OnTimer(DWORD wTimerID); 
	virtual void OnVScroll(UINT nSBCode,UINT nPos,HWND hScrollBar);
	virtual void OnWindowPosChanged(LPWINDOWPOS lpWndPos);
	virtual void OnWindowPosChanging(LPWINDOWPOS lpWndPos);
	virtual LRESULT WindowProc(UINT msg,WPARAM wParam,LPARAM lParam);
	
};

#if defined(DEF_RESOURCES)

class CFrameWnd : public CTargetWnd
{
public:
	CFrameWnd();
	virtual ~CFrameWnd();

	BOOL Create(LPCTSTR lpszClassName,LPCTSTR lpszWindowName,DWORD dwStyle=WS_OVERLAPPEDWINDOW,
				const RECT* rect=NULL,HWND hParentWnd=NULL,LPCTSTR lpszMenuName=NULL,
				DWORD dwExStyle=0);
	BOOL Create(LPCTSTR lpszClassName,LPCTSTR lpszWindowName,DWORD dwStyle=WS_OVERLAPPEDWINDOW,
				const RECT* rect=NULL,HWND hParentWnd=NULL,UINT nMenuID=0,DWORD dwExStyle=0);
	
	virtual int OnCreate(LPCREATESTRUCT lpcs);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs);
	virtual void ActivateFrame(int nCmdShow=1);

	virtual LRESULT WindowProc(UINT msg,WPARAM wParam,LPARAM lParam);
protected:
	HMENU m_hMenu;
};

class CMDIFrameWnd : public CFrameWnd
{
public:
	CMDIFrameWnd();

public:
	HWND m_hWndMDIClient;

	void MDIActivate(HWND hWndActivate);
	HWND MDIGetActive(BOOL* pbMaximized=NULL) const;
	void MDIIconArrange();
	void MDIMaximize(HWND hWnd);
	void MDINext();
	void MDIRestore(HWND hWnd);
	HMENU MDISetMenu(HMENU hFrameMenu,HMENU hWindowMenu);
	void MDITile();
	void MDICascade();
	void MDITile(int nType);
	void MDICascade(int nType);

	virtual BOOL CreateClient(LPCREATESTRUCT lpCreateStruct,HMENU hWindowMenu);
	virtual HMENU GetWindowMenuPopup(HMENU hMenuBar);

	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs);
	virtual HWND GetActiveFrame();

public:
	virtual LRESULT WindowProc(UINT msg,WPARAM wParam,LPARAM lParam);
	virtual BOOL OnCommand(WORD wID,WORD wNotifyCode,HWND hControl);
	virtual void OnWindowNew();
};

class CMDIChildWnd : public CFrameWnd
{
public:
	CMDIChildWnd();

	virtual BOOL Create(LPCTSTR lpszClassName,LPCTSTR lpszWindowName,
		DWORD dwStyle=WS_CHILD|WS_VISIBLE|WS_OVERLAPPEDWINDOW,
		const RECT* rect=NULL,CMDIFrameWnd* pParentWnd=NULL);
	
protected:
	CMDIFrameWnd* GetMDIFrame();
	CMDIFrameWnd* m_pParent;

public:
	void MDIDestroy();
	void MDIActivate();
	void MDIMaximize();
	void MDIRestore();

public:
	virtual BOOL DestroyWindow();
	virtual void ActivateFrame(int nCmdShow=-1);

public:
	virtual LRESULT WindowProc(UINT msg,WPARAM wParam,LPARAM lParam);
	BOOL UpdateClientEdge(LPRECT lpRect=NULL);

	virtual BOOL OnCommand(WORD wID,WORD wNotifyCode,HWND hControl);
	virtual void OnWindowNew();
	virtual LRESULT OnMenuChar(UINT nChar,UINT nFlags,HMENU hMenu);
	virtual BOOL OnNcCreate(LPCREATESTRUCT lpCreateStruct);
	virtual int OnCreate(LPCREATESTRUCT lpcs);
	virtual void OnMDIActivate(HWND hAct,HWND hDeAct);
	virtual int OnMouseActivate(HWND hDesktopWnd,UINT nHitTest,UINT message);
	virtual void OnWindowPosChanging(LPWINDOWPOS lpWndPos);
	virtual BOOL OnNcActivate(BOOL bActive);
	virtual void OnDestroy();
};

#endif

#include "Windows.inl"


#endif
#endif
