////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////

#ifndef HFCWINDOWS_INL
#define HFCWINDOWS_INL

///////////////////////////
// Class CDC

inline CDC::CDC()
:	m_hDC(NULL),m_bPrinting(FALSE),m_hWnd(NULL),m_nFreeMethod(None)
{
}

inline CDC::CDC(HDC hDC,HWND hWnd)
:	m_hDC(hDC),m_bPrinting(FALSE),m_hWnd(hWnd),m_nFreeMethod(None)
{
}

inline CDC::~CDC()
{
	FreeDC();
}


inline HGDIOBJ CDC::SelectStockObject(int nIndex)
{
	return (HGDIOBJ)::SelectObject(m_hDC,::GetStockObject(nIndex));
}


inline void CDC::Attach(HDC hDC,HWND hWnd)
{
	ASSERT_VALID(hDC);

	FreeDC();

	m_hDC=hDC;
	m_hWnd=hWnd;
	m_nFreeMethod=None;
}

inline void CDC::Attach(CDC& dc)
{
	ASSERT_VALID(dc.m_hDC);

	FreeDC();

	m_hDC=dc.m_hDC;
	m_hWnd=dc.m_hWnd;
	m_nFreeMethod=dc.m_nFreeMethod;

	dc.m_hDC=NULL;
	dc.m_nFreeMethod=None;
}

	
inline int CDC::SaveDC()
{
	return ::SaveDC(m_hDC);
}

inline BOOL CDC::RestoreDC(int nSavedDC)
{
	return ::RestoreDC(m_hDC,nSavedDC);
}


inline int CDC::GetClipBox(LPRECT lpRect) const
{
	return ::GetClipBox(m_hDC,lpRect);
}

inline BOOL CDC::PtVisible(int x,int y) const
{
	return ::PtVisible(m_hDC,x,y);
}

inline BOOL CDC::RectVisible(LPCRECT lpRect) const
{
	return ::RectVisible(m_hDC,lpRect);
}

inline CPoint CDC::GetCurrentPosition() const
{
	CPoint pt;
	::GetCurrentPositionEx(m_hDC,&pt);
	return pt;
}

inline CPoint CDC::MoveTo(int x,int y)
{
	CPoint pt;
	::MoveToEx(m_hDC,x,y,&pt);
	return pt;
}

inline CPoint CDC::MoveTo(const POINT& point)
{
	CPoint pt;
	::MoveToEx(m_hDC,point.x,point.y,&pt);
	return pt;
}

inline BOOL CDC::TextOut(int x,int y,LPCTSTR lpszString,int nCount)
{
	return ::TextOut(m_hDC,x,y,lpszString,nCount);
}

inline BOOL CDC::ExtTextOut(int x,int y,UINT nOptions,LPCRECT lpRect,
	LPCTSTR lpszString,UINT nCount,LPINT lpDxWidths)
{
	return ::ExtTextOut(m_hDC,x,y,nOptions,lpRect,lpszString,nCount,lpDxWidths);
}

inline CPoint CDC::GetBrushOrg() const
{
	CPoint point;
	::GetBrushOrgEx(m_hDC,&point);
	return point;
}

inline CPoint CDC::SetBrushOrg(int x, int y)
{
	CPoint point;
	::SetBrushOrgEx(m_hDC,x,y,&point);
	return point;
}

inline CPoint CDC::SetBrushOrg(const POINT& point)
{
	CPoint pt;
	::SetBrushOrgEx(m_hDC,point.x,point.y,&pt);
	return pt;
}

inline CPoint CDC::GetViewportOrg() const
{
	CPoint pt;
	::GetViewportOrgEx(m_hDC,&pt);
	return pt;
}

inline COLORREF CDC::SetTextColor(COLORREF crColor)
{
	return ::SetTextColor(m_hDC,crColor);
}
	
inline int CDC::SetMapMode(int nMapMode)
{
	return ::SetMapMode(m_hDC,nMapMode);
}

inline CPoint CDC::SetViewportOrg(int x, int y)
{
	CPoint pt;
	::SetViewportOrgEx(m_hDC,x,y,&pt);
	return pt;
}

inline CPoint CDC::SetViewportOrg(const POINT& point)
{
	CPoint pt;
	::SetViewportOrgEx(m_hDC,point.x,point.y,&pt);
	return pt;
}

inline int CDC::DrawText(LPCSTR lpszString,int nCount,LPRECT lpRect,UINT nFormat)
{
	return ::DrawText(m_hDC,lpszString,nCount,lpRect,nFormat);
}


inline CSize CDC::GetTextExtent(LPCSTR lpszString,int nCount) const
{
	CSize sz;
	::GetTextExtentPoint32(m_hDC,lpszString,nCount,&sz);
	return sz;
}

inline CSize CDC::GetTextExtent(const CStringA& str) const
{
	CSize sz;
	::GetTextExtentPoint32A(m_hDC,(LPCSTR)str,(int)str.GetLength(),&sz);
	return sz;
}

inline BOOL CDC::GrayString(HBRUSH hBrush,
	BOOL (CALLBACK* lpfnOutput)(HDC,LPARAM,int),LPARAM lpData,
	int nCount,int x,int y,int nWidth,int nHeight)
{
	return ::GrayString(m_hDC,hBrush,(GRAYSTRINGPROC)lpfnOutput,lpData,nCount,x,y,nWidth,nHeight);
}

inline int CDC::GetTextFace(CStringA& rString) const
{
	int ret=::GetTextFaceA(m_hDC,2000,rString.GetBuffer(2000));
	rString.FreeExtra();
	return ret;
}

inline CSize CDC::GetAspectRatioFilter() const
{
	CSize sz;
	::GetAspectRatioFilterEx(m_hDC,&sz);
	return sz;
}

inline int CDC::Escape(int nEscape,int nCount,LPCTSTR lpszInData,LPVOID lpOutData)
{
	return ::Escape(m_hDC,nEscape,nCount,lpszInData,lpOutData);
}

inline int CDC::DrawEscape(int nEscape,int nInputSize,LPCTSTR lpszInputData)
{
	return ::DrawEscape(m_hDC,nEscape,nInputSize,lpszInputData);
}

inline CPoint CDC::OffsetViewportOrg(int nWidth,int nHeight)
{
	CPoint pt;
	::OffsetViewportOrgEx(m_hDC,nWidth,nHeight,&pt);
	return pt;
}


inline CSize CDC::GetViewportExt() const
{
	CSize sz;
	::GetViewportExtEx(m_hDC,&sz);
	return sz;
}

inline CSize CDC::SetViewportExt(int cx,int cy)
{
	CSize sz;
	::SetViewportExtEx(m_hDC,cx,cy,&sz);
	return sz;
}

inline CSize CDC::SetViewportExt(SIZE size)
{
	CSize sz;
	::SetViewportExtEx(m_hDC,size.cx,size.cy,&sz);
	return sz;
}
	
inline CSize CDC::ScaleViewportExt(int xNum,int xDenom,int yNum,int yDenom)
{
	CSize sz;
	::ScaleViewportExtEx(m_hDC,xNum,xDenom,yNum,yDenom,&sz);
	return sz;
}

inline CPoint CDC::GetWindowOrg() const
{
	CPoint pt;
	::GetWindowOrgEx(m_hDC,&pt);
	return pt;
}

inline CPoint CDC::SetWindowOrg(int x,int y)
{
	CPoint pt;
	::SetWindowOrgEx(m_hDC,x,y,&pt);
	return pt;
}

inline CPoint CDC::SetWindowOrg(const POINT& point)
{
	CPoint pt;
	::SetWindowOrgEx(m_hDC,point.x,point.y,&pt);
	return pt;
}

inline CPoint CDC::OffsetWindowOrg(int nWidth,int nHeight)
{
	CPoint pt;
	::OffsetWindowOrgEx(m_hDC,nWidth,nHeight,&pt);
	return pt;
}

inline CSize CDC::GetWindowExt() const
{
	CSize sz;
	::GetWindowExtEx(m_hDC,&sz);
	return sz;
}

inline CSize CDC::SetWindowExt(int cx,int cy)
{
	CSize sz;
	::SetWindowExtEx(m_hDC,cx,cy,&sz);
	return sz;
}

inline CSize CDC::SetWindowExt(const SIZE& size)
{
	CSize sz;
	::SetWindowExtEx(m_hDC,size.cx,size.cy,&sz);
	return sz;
}

inline CSize CDC::ScaleWindowExt(int xNum,int xDenom,int yNum,int yDenom)
{
	CSize sz;
	::ScaleWindowExtEx(m_hDC,xNum,xDenom,yNum,yDenom,&sz);
	return sz;
}

inline void CDC::ReleaseDC(HWND hWnd)
{
	ASSERT_VALID(m_hDC);

	::ReleaseDC(hWnd,m_hDC);
	DebugCloseHandle(dhtGdiObject,m_hDC,STRNULL);
	m_hWnd=NULL;
	m_hDC=NULL;
	m_nFreeMethod=None;
}

inline CDC::operator HDC() const
{
	return m_hDC;
}

inline HDC CDC::GetHandle() const
{
	return m_hDC;
}

inline HGDIOBJ CDC::SelectObject(HGDIOBJ hGdiObj)
{
	return (HGDIOBJ)::SelectObject(m_hDC,hGdiObj);
}

inline int CDC::SelectRegion(HRGN hRgn)
{
	return (int)(LONGLONG)::SelectObject(m_hDC,hRgn);
}

inline void CDC::SetWindow(HWND hWnd)
{
	m_hWnd=hWnd;
}

inline HWND CDC::GetWindow() const
{
	return m_hWnd;
}

inline BOOL CDC::IsPrinting() const
{
	return m_bPrinting;
}

inline HPEN CDC::GetCurrentPen() const
{
	return (HPEN)::GetCurrentObject(m_hDC,OBJ_PEN);
}

inline HBRUSH CDC::GetCurrentBrush() const
{
	return (HBRUSH)::GetCurrentObject(m_hDC,OBJ_BRUSH);
}

inline HPALETTE CDC::GetCurrentPalette() const
{
	return (HPALETTE)::GetCurrentObject(m_hDC,OBJ_PAL);
}

inline HFONT CDC::GetCurrentFont() const
{
	return (HFONT)::GetCurrentObject(m_hDC,OBJ_FONT);
}

inline HBITMAP CDC::GetCurrentBitmap() const
{
	return (HBITMAP)::GetCurrentObject(m_hDC,OBJ_BITMAP);
}

inline int CDC::GetDeviceCaps(int nIndex) const
{
	return ::GetDeviceCaps(m_hDC,nIndex);
}

inline UINT CDC::SetBoundsRect(LPCRECT lpRectBounds,UINT flags)
{
	return ::SetBoundsRect(m_hDC,lpRectBounds,flags);
}

inline UINT CDC::GetBoundsRect(LPRECT lpRectBounds,UINT flags)
{
	return ::GetBoundsRect(m_hDC,lpRectBounds,flags);
}

inline int CDC::EnumObjects(int nObjectType,int (CALLBACK* lpfn)(LPVOID,LPARAM),LPARAM lpData)
{
	return ::EnumObjects(m_hDC,nObjectType,(GOBJENUMPROC)lpfn,(LPARAM)lpData);
}
	
inline COLORREF CDC::GetNearestColor(COLORREF crColor) const
{
	return ::GetNearestColor(m_hDC,crColor);
}

inline HPALETTE CDC::SelectPalette(HPALETTE hPalette,BOOL bForceBackground)
{
	return ::SelectPalette(m_hDC,hPalette,bForceBackground);
}

inline UINT CDC::RealizePalette()
{
	return ::RealizePalette(m_hDC);
}

inline void CDC::UpdateColors()
{
	::UpdateColors(m_hDC);
}


inline COLORREF CDC::GetBkColor() const
{
	return ::GetBkColor(m_hDC);
}

inline int CDC::GetBkMode() const
{
	return ::GetBkMode(m_hDC);
}

inline int CDC::GetPolyFillMode() const
{
	return ::GetPolyFillMode(m_hDC);
}

inline int CDC::GetROP2() const
{
	return ::GetROP2(m_hDC);
}

inline int CDC::GetStretchBltMode() const
{
	return ::GetStretchBltMode(m_hDC);
}

inline COLORREF CDC::GetTextColor() const
{
	return ::GetTextColor(m_hDC);
}

inline COLORREF CDC::SetBkColor(COLORREF crColor)
{
	return ::SetBkColor(m_hDC,crColor);
}

inline int CDC::SetBkMode(int nBkMode)
{
	return ::SetBkMode(m_hDC,nBkMode);
}

inline int CDC::SetPolyFillMode(int nPolyFillMode)
{
	return ::SetPolyFillMode(m_hDC,nPolyFillMode);
}

inline int CDC::SetROP2(int nDrawMode)
{
	return ::SetROP2(m_hDC,nDrawMode);
}

inline int CDC::SetStretchBltMode(int nStretchMode)
{
	return ::SetStretchBltMode(m_hDC,nStretchMode);
}

inline BOOL CDC::GetColorAdjustment(LPCOLORADJUSTMENT lpColorAdjust) const
{
	return ::GetColorAdjustment(m_hDC,lpColorAdjust);
}

inline BOOL CDC::SetColorAdjustment(const COLORADJUSTMENT* lpColorAdjust)
{
	return ::SetColorAdjustment(m_hDC,lpColorAdjust);
}

inline int CDC::GetMapMode() const
{
	return ::GetMapMode(m_hDC);
}

inline void CDC::DPtoLP(LPPOINT lpPoints,int nCount) const
{
	::DPtoLP(m_hDC,lpPoints,nCount);
}

inline void CDC::LPtoDP(LPPOINT lpPoints,int nCount) const
{
	::LPtoDP(m_hDC,lpPoints,nCount);
}

inline BOOL CDC::FillRgn(HRGN hRgn,HBRUSH hBrush)
{
	return ::FillRgn(m_hDC,hRgn,hBrush);
}

inline BOOL CDC::FrameRgn(HRGN hRgn,HBRUSH hBrush,int nWidth,int nHeight)
{
	return ::FrameRgn(m_hDC,hRgn,hBrush,nWidth,nHeight);
}

inline BOOL CDC::InvertRgn(HRGN hRgn)
{
	return ::InvertRgn(m_hDC,hRgn);
}

inline BOOL CDC::PaintRgn(HRGN hRgn)
{
	return ::PaintRgn(m_hDC,hRgn);
}

inline BOOL CDC::PtVisible(const POINT& point) const
{
	return ::PtVisible(m_hDC,point.x,point.y);
}

inline int CDC::SelectClipRgn(HRGN hRgn)
{
	return ::SelectClipRgn(m_hDC,hRgn);
}

inline int CDC::ExcludeClipRect(int x1,int y1,int x2,int y2)
{
	return ::ExcludeClipRect(m_hDC,x1,y1,x2,y2);
}

inline int CDC::ExcludeClipRect(LPCRECT lpRect)
{
	return ::ExcludeClipRect(m_hDC,lpRect->left,lpRect->top,lpRect->right,lpRect->bottom);
}

inline int CDC::ExcludeUpdateRgn(HWND hWnd)
{
	return ::ExcludeUpdateRgn(m_hDC,hWnd);
}

inline int CDC::IntersectClipRect(int x1,int y1,int x2,int y2)
{
	return ::IntersectClipRect(m_hDC,x1,y1,x2,y2);
}

inline BOOL CDC::LineTo(int x,int y)
{
	return ::LineTo(m_hDC,x,y);
}

inline BOOL CDC::LineTo(const POINT& point)
{
	return ::LineTo(m_hDC,point.x,point.y);
}

inline BOOL CDC::Arc(int x1,int y1,int x2,int y2,int x3,int y3,int x4,int y4)
{
	return ::Arc(m_hDC,x1,y1,x2,y2,x3,y3,x4,y4);
}

inline BOOL CDC::Arc(LPCRECT lpRect,const POINT& ptStart,const POINT& ptEnd)
{
	return ::Arc(m_hDC,lpRect->left,lpRect->top,lpRect->right,lpRect->bottom,ptStart.x,ptStart.y,ptEnd.x,ptEnd.y);
}

inline BOOL CDC::Polyline(LPPOINT lpPoints,int nCount)
{
	return ::Polyline(m_hDC,lpPoints,nCount);
}

inline BOOL CDC::AngleArc(int x,int y,int nRadius,float fStartAngle,float fSweepAngle)
{
	return ::AngleArc(m_hDC,x,y,nRadius,fStartAngle,fSweepAngle);
}

inline BOOL CDC::ArcTo(int x1,int y1,int x2,int y2,int x3,int y3,int x4,int y4)
{
	return ::ArcTo(m_hDC,x1,y1,x2,y2,x3,y3,x4,y4);
}

inline BOOL CDC::ArcTo(LPCRECT lpRect,const POINT& ptStart,const POINT& ptEnd)
{
	return ::ArcTo(m_hDC,lpRect->left,lpRect->top,lpRect->right,lpRect->bottom,ptStart.x,ptStart.y,ptEnd.x,ptEnd.y);
}

inline int CDC::GetArcDirection() const
{
	return ::GetArcDirection(m_hDC);
}

inline int CDC::SetArcDirection(int nArcDirection)
{
	return ::SetArcDirection(m_hDC,nArcDirection);
}

inline BOOL CDC::PolyDraw(const POINT* lpPoints,const BYTE* lpTypes,int nCount)
{
	return ::PolyDraw(m_hDC,lpPoints,lpTypes,nCount);
}

inline BOOL CDC::PolylineTo(const POINT* lpPoints,int nCount)
{
	return ::PolylineTo(m_hDC,lpPoints,nCount);
}

inline BOOL CDC::PolyPolyline(const POINT* lpPoints,const DWORD* lpPolyPoints,int nCount)
{
	return ::PolyPolyline(m_hDC,lpPoints,lpPolyPoints,nCount);
}

inline BOOL CDC::PolyBezier(const POINT* lpPoints, int nCount)
{
	return ::PolyBezier(m_hDC,lpPoints,nCount);
}

inline BOOL CDC::PolyBezierTo(const POINT* lpPoints, int nCount)
{
	return ::PolyBezierTo(m_hDC,lpPoints,nCount);
}

inline void CDC::FillRect(LPCRECT lpRect,HBRUSH hBrush)
{
	::FillRect(m_hDC,lpRect,hBrush);
}

inline void CDC::FrameRect(LPCRECT lpRect,HBRUSH hBrush)
{
	::FrameRect(m_hDC,lpRect,hBrush);
}

inline void CDC::InvertRect(LPCRECT lpRect)
{
	::InvertRect(m_hDC,lpRect);
}

inline BOOL CDC::DrawIcon(int x,int y,HICON hIcon)
{
	return ::DrawIcon(m_hDC,x,y,hIcon);
}

inline BOOL CDC::DrawIcon(const POINT& point,HICON hIcon)
{
	return ::DrawIcon(m_hDC,point.x,point.y,hIcon);
}

inline BOOL CDC::DrawIcon(int xLeft,int yTop,HICON hIcon,int cxWidth, int cyWidth,UINT istepIfAniCur,HBRUSH hbrFlickerFreeDraw,UINT diFlags)
{
	return ::DrawIconEx(m_hDC,xLeft,yTop,hIcon,cxWidth,cyWidth,istepIfAniCur,hbrFlickerFreeDraw,diFlags);
}


inline BOOL CDC::DrawIcon(const POINT& point,HICON hIcon,const SIZE& sz,UINT istepIfAniCur,HBRUSH hbrFlickerFreeDraw,UINT diFlags)
{
	return ::DrawIconEx(m_hDC,point.x,point.y,hIcon,sz.cx,sz.cy,istepIfAniCur,hbrFlickerFreeDraw,diFlags);
}

inline BOOL CDC::DrawState(const POINT& pt,const SIZE& size,HGDIOBJ hGdiObj,UINT nFlags,HBRUSH hBrush)
{
	return ::DrawState(m_hDC,hBrush,NULL,(LPARAM)hGdiObj,0,pt.x,pt.y,size.cx,size.cy,nFlags);
}

inline BOOL CDC::DrawState(const CPoint& pt,const CSize& size,CBitmap* pBitmap,UINT nFlags,HBRUSH hBrush)
{
	return ::DrawState(m_hDC,hBrush,NULL,(LPARAM)(pBitmap==NULL?NULL:(HBITMAP)*pBitmap),0,pt.x,pt.y,size.cx,size.cy,nFlags|DST_BITMAP);
}

inline BOOL CDC::DrawState(const CPoint& pt,const CSize& size,DRAWSTATEPROC lpDrawProc,LPARAM lData,UINT nFlags,HBRUSH hBrush)
{
	return ::DrawState(m_hDC,hBrush,lpDrawProc,lData,0,pt.x,pt.y,size.cx,size.cy,nFlags|DST_COMPLEX);
}
	
inline BOOL CDC::DrawState(const CPoint& pt,const CSize& size,LPCSTR lpszText,UINT nFlags,BOOL bPrefixText,int nTextLen,HBRUSH hBrush)
{
	return ::DrawState(m_hDC,hBrush,NULL,(LPARAM)lpszText,0,pt.x,pt.y,size.cx,size.cy,nFlags|(bPrefixText?DST_PREFIXTEXT:DST_TEXT));
}

inline BOOL CDC::Chord(int x1,int y1,int x2,int y2,int x3,int y3,int x4,int y4)
{
	return ::Chord(m_hDC,x1,y1,x2,y2,x3,y3,x4,y4);
}

inline BOOL CDC::Chord(LPCRECT lpRect,const POINT& ptStart,const POINT& ptEnd)
{
	return ::Chord(m_hDC,lpRect->left,lpRect->top,lpRect->right,lpRect->bottom,ptStart.x,ptStart.y,ptEnd.x,ptEnd.y);
}

inline void CDC::DrawFocusRect(LPCRECT lpRect)
{
	::DrawFocusRect(m_hDC,lpRect);
}

inline BOOL CDC::Ellipse(int x1,int y1,int x2,int y2)
{
	return ::Ellipse(m_hDC,x1,y1,x2,y2);
}

inline BOOL CDC::Ellipse(LPCRECT lpRect)
{
	return ::Ellipse(m_hDC,lpRect->left,lpRect->top,lpRect->right,lpRect->bottom);
}

inline BOOL CDC::Pie(int x1,int y1,int x2,int y2,int x3,int y3,int x4,int y4)
{
	return ::Pie(m_hDC,x1,y1,x2,y2,x3,y3,x4,y4);
}

inline BOOL CDC::Pie(LPCRECT lpRect,const POINT& ptStart,const POINT& ptEnd)
{
	return ::Pie(m_hDC,lpRect->left,lpRect->top,lpRect->right,lpRect->bottom,ptStart.x,ptStart.y,ptEnd.x,ptStart.y);
}

inline BOOL CDC::Polygon(LPPOINT lpPoints,int nCount)
{
	return ::Polygon(m_hDC,lpPoints,nCount);
}

inline BOOL CDC::PolyPolygon(LPPOINT lpPoints,LPINT lpPolyCounts,int nCount)
{
	return ::PolyPolygon(m_hDC,lpPoints,lpPolyCounts,nCount);
}

inline BOOL CDC::Rectangle(int x1,int y1,int x2,int y2)
{
	return ::Rectangle(m_hDC,x1,y1,x2,y2);
}

inline BOOL CDC::Rectangle(LPCRECT lpRect)
{
	return ::Rectangle(m_hDC,lpRect->left,lpRect->top,lpRect->right,lpRect->bottom);
}

inline BOOL CDC::RoundRect(int x1,int y1,int x2,int y2,int x3,int y3)
{
	return ::RoundRect(m_hDC,x1,y1,x2,y2,x3,y3);
}

inline BOOL CDC::RoundRect(LPCRECT lpRect,const POINT& point)
{
	return ::RoundRect(m_hDC,lpRect->left,lpRect->top,lpRect->right,lpRect->bottom,point.x,point.y);
}

inline BOOL CDC::PatBlt(int x,int y,int nWidth,int nHeight,DWORD dwRop)
{
	return ::PatBlt(m_hDC,x,y,nWidth,nHeight,dwRop);
}

inline BOOL CDC::BitBlt(int x,int y,int nWidth,int nHeight,HDC hSrcDC,int xSrc,int ySrc,DWORD dwRop)
{
	return ::BitBlt(m_hDC,x,y,nWidth,nHeight,hSrcDC,xSrc,ySrc,dwRop);
}

inline BOOL CDC::StretchBlt(int x,int y,int nWidth,int nHeight,HDC hSrcDC,int xSrc,
	int ySrc,int nSrcWidth,int nSrcHeight,DWORD dwRop)
{
	return ::StretchBlt(m_hDC,x,y,nWidth,nHeight,hSrcDC,
		xSrc,ySrc,nSrcWidth,nSrcHeight,dwRop);
}

inline COLORREF CDC::GetPixel(int x,int y) const
{
	return ::GetPixel(m_hDC,x,y);
}

inline COLORREF CDC::GetPixel(const POINT& point) const
{
	return ::GetPixel(m_hDC,point.x,point.y);
}

inline COLORREF CDC::SetPixel(int x,int y,COLORREF crColor)
{
	return ::SetPixel(m_hDC,x,y,crColor);
}

inline COLORREF CDC::SetPixel(const POINT& point,COLORREF crColor)
{
	return ::SetPixel(m_hDC,point.x,point.y,crColor);
}

inline BOOL CDC::FloodFill(int x,int y,COLORREF crColor)
{
	return ::FloodFill(m_hDC,x,y,crColor);
}

inline BOOL CDC::ExtFloodFill(int x,int y,COLORREF crColor,UINT nFillType)
{
	return ::ExtFloodFill(m_hDC,x,y,crColor,nFillType);
}

inline BOOL CDC::MaskBlt(int x,int y,int nWidth,int nHeight,HDC hSrcDC,
	int xSrc,int ySrc,HBITMAP maskBitmap,int xMask,int yMask,DWORD dwRop)
{
	return ::MaskBlt(m_hDC,x,y,nWidth,nHeight,hSrcDC,
		xSrc,ySrc,maskBitmap,xMask,yMask,dwRop);
}

inline BOOL CDC::PlgBlt(LPPOINT lpPoint,HDC hSrcDC,int xSrc,int ySrc,
	int nWidth,int nHeight,HBITMAP maskBitmap,int xMask,int yMask)
{
	return ::PlgBlt(m_hDC,lpPoint,hSrcDC,xSrc,ySrc,
		nWidth,nHeight,maskBitmap,xMask,yMask);
}

inline BOOL CDC::SetPixelV(int x,int y,COLORREF crColor)
{
	return ::SetPixelV(m_hDC,x,y,crColor);
}

inline BOOL CDC::SetPixelV(const POINT& point,COLORREF crColor)
{
	return ::SetPixelV(m_hDC,point.x,point.y,crColor);
}

inline BOOL CDC::TextOut(int x,int y,const CStringA& str)
{
	return ::TextOutA(m_hDC,x,y,(LPCSTR)str,(int)str.GetLength());
}



inline BOOL CDC::ExtTextOut(int x,int y,UINT nOptions,LPCRECT lpRect,
	const CString& str,LPINT lpDxWidths)
{
	return ::ExtTextOutA(m_hDC,x,y,nOptions,lpRect,(LPCSTR)str,(int)str.GetLength(),lpDxWidths);
}

inline int CDC::DrawText(const CString& str,LPRECT lpRect,UINT nFormat)
{
	return ::DrawTextA(m_hDC,(LPCSTR)str,(int)str.GetLength(),lpRect,nFormat);
}


inline UINT CDC::GetTextAlign() const
{
	return ::GetTextAlign(m_hDC);
}

inline UINT CDC::SetTextAlign(UINT nFlags)
{
	return ::SetTextAlign(m_hDC,nFlags);
}

inline int CDC::GetTextFace(int nCount,LPTSTR lpszFacename) const
{
	return ::GetTextFace(m_hDC,nCount,lpszFacename);
}

inline BOOL CDC::GetTextMetrics(LPTEXTMETRIC lpMetrics) const
{
	return ::GetTextMetrics(m_hDC,lpMetrics);
}

inline BOOL CDC::GetOutputTextMetrics(LPTEXTMETRIC lpMetrics) const
{
	return ::GetTextMetrics(m_hDC,lpMetrics);
}

inline int CDC::SetTextJustification(int nBreakExtra,int nBreakCount)
{
	return ::SetTextJustification(m_hDC,nBreakExtra,nBreakCount);
}

inline int CDC::GetTextCharacterExtra() const
{
	return ::GetTextCharacterExtra(m_hDC);
}

inline int CDC::SetTextCharacterExtra(int nCharExtra)
{
	return ::SetTextCharacterExtra(m_hDC,nCharExtra);
}

inline BOOL CDC::DrawEdge(LPRECT lpRect,UINT nEdge,UINT nFlags)
{
	return ::DrawEdge(m_hDC,lpRect,nEdge,nFlags);
}

inline BOOL CDC::DrawFrameControl(LPRECT lpRect,UINT nType,UINT nState)
{
	return ::DrawFrameControl(m_hDC,lpRect,nType,nState);
}

inline BOOL CDC::ScrollDC(int dx,int dy,LPCRECT lpRectScroll,LPCRECT lpRectClip,
	HRGN hRgnUpdate,LPRECT lpRectUpdate)
{
	return ::ScrollDC(m_hDC,dx,dy,lpRectScroll,lpRectClip,hRgnUpdate,lpRectUpdate);
}

inline BOOL CDC::GetCharWidth(UINT nFirstChar,UINT nLastChar,LPINT lpBuffer) const
{
	return ::GetCharWidth(m_hDC,nFirstChar,nLastChar,lpBuffer);
}

inline BOOL CDC::GetOutputCharWidth(UINT nFirstChar,UINT nLastChar,LPINT lpBuffer) const
{
	return ::GetCharWidth(m_hDC,nFirstChar,nLastChar,lpBuffer);
}

inline DWORD CDC::SetMapperFlags(DWORD dwFlag)
{
	return ::SetMapperFlags(m_hDC,dwFlag);
}

inline BOOL CDC::GetCharABCWidths(UINT nFirstChar,UINT nLastChar,LPABC lpabc) const
{
	return ::GetCharABCWidths(m_hDC,nFirstChar,nLastChar,lpabc);
}

inline DWORD CDC::GetFontData(DWORD dwTable,DWORD dwOffset,LPVOID lpData,DWORD cbData) const
{
	return ::GetFontData(m_hDC,dwTable,dwOffset,lpData,cbData);
}

inline int CDC::GetKerningPairs(int nPairs,LPKERNINGPAIR lpkrnpair) const
{
	return ::GetKerningPairs(m_hDC,nPairs,lpkrnpair);
}

inline UINT CDC::GetOutlineTextMetrics(UINT cbData,LPOUTLINETEXTMETRIC lpotm) const
{
	return ::GetOutlineTextMetrics(m_hDC,cbData,lpotm);
}

inline DWORD CDC::GetGlyphOutline(UINT nChar,UINT nFormat,LPGLYPHMETRICS lpgm,
	DWORD cbBuffer, LPVOID lpBuffer,const MAT2* lpmat2) const
{
	return ::GetGlyphOutline(m_hDC,nChar,nFormat,lpgm,cbBuffer,lpBuffer,lpmat2);
}

inline BOOL CDC::GetCharABCWidths(UINT nFirstChar,UINT nLastChar,LPABCFLOAT lpABCF) const
{
	return ::GetCharABCWidthsFloat(m_hDC,nFirstChar,nLastChar,lpABCF);
}

inline BOOL CDC::GetCharWidth(UINT nFirstChar,UINT nLastChar,float* lpFloatBuffer) const
{
	return ::GetCharWidthFloat(m_hDC,nFirstChar,nLastChar,lpFloatBuffer);
}

inline int CDC::Escape(int nEscape,int nInputSize,LPCTSTR lpszInputData,
	int nOutputSize,LPTSTR lpszOutputData)
{
	return ::ExtEscape(m_hDC,nEscape,nInputSize,lpszInputData,nOutputSize,lpszOutputData);
}

inline int CDC::StartPage()
{
	return ::StartPage(m_hDC);
}

inline int CDC::EndPage()
{
	return ::EndPage(m_hDC);
}

inline int CDC::SetAbortProc(BOOL (CALLBACK* lpfn)(HDC, int))
{
	return ::SetAbortProc(m_hDC,(ABORTPROC)lpfn);
}

inline int CDC::IntersectClipRect(LPCRECT lpRect)
{
	return ::IntersectClipRect(m_hDC,lpRect->left,lpRect->top,lpRect->right,lpRect->bottom);
}

inline int CDC::OffsetClipRgn(int x,int y)
{
	return ::OffsetClipRgn(m_hDC,x,y);
}

inline int CDC::OffsetClipRgn(const SIZE& size)
{
	return ::OffsetClipRgn(m_hDC,size.cx,size.cy);
}

inline int CDC::SelectClipRgn(HRGN hRgn,int nMode)
{
	return ::ExtSelectClipRgn(m_hDC,hRgn,nMode);
}

inline BOOL CDC::PlayMetaFile(HMETAFILE hMF)
{
	return ::PlayMetaFile(m_hDC,hMF);
}

inline BOOL CDC::PlayMetaFile(HENHMETAFILE hEnhMetaFile,LPCRECT lpBounds)
{
	return ::PlayEnhMetaFile(m_hDC,hEnhMetaFile,lpBounds);
}

inline BOOL CDC::AddMetaFileComment(UINT nDataSize,const BYTE* pCommentData)
{
	return ::GdiComment(m_hDC,nDataSize,pCommentData);
}

inline BOOL CDC::AbortPath()
{
	return ::AbortPath(m_hDC);
}

inline BOOL CDC::BeginPath()
{
	return ::BeginPath(m_hDC);
}

inline BOOL CDC::CloseFigure()
{
	return ::CloseFigure(m_hDC);
}

inline BOOL CDC::EndPath()
{
	return ::EndPath(m_hDC);
}

inline BOOL CDC::FillPath()
{
	return ::FillPath(m_hDC);
}

inline BOOL CDC::FlattenPath()
{
	return ::FlattenPath(m_hDC);
}

inline BOOL CDC::StrokeAndFillPath()
{
	return ::StrokeAndFillPath(m_hDC);
}

inline BOOL CDC::StrokePath()
{
	return ::StrokePath(m_hDC);
}

inline BOOL CDC::WidenPath()
{
	return ::WidenPath(m_hDC);
}

inline BOOL CDC::SetMiterLimit(float fMiterLimit)
{
	return ::SetMiterLimit(m_hDC,fMiterLimit,NULL);
}

inline int CDC::GetPath(LPPOINT lpPoints,LPBYTE lpTypes,int nCount) const
{
	return ::GetPath(m_hDC,lpPoints,lpTypes,nCount);
}

inline BOOL CDC::SelectClipPath(int nMode)
{
	return ::SelectClipPath(m_hDC,nMode);
}

#ifdef DEF_WCHAR
inline BOOL CDC::ExtTextOut(int x,int y,UINT nOptions,LPCRECT lpRect,
	LPCWSTR lpszString,UINT nCount,LPINT lpDxWidths)
{
	if (IsUnicodeSystem())
		return ::ExtTextOutW(m_hDC,x,y,nOptions,lpRect,lpszString,nCount,lpDxWidths);
	else
		return ::ExtTextOutA(m_hDC,x,y,nOptions,lpRect,W2A(lpszString),nCount,lpDxWidths);
}

inline int CDC::DrawText(LPCWSTR lpszString,int nCount,LPRECT lpRect,UINT nFormat)
{
	if (IsUnicodeSystem())
		return ::DrawTextW(m_hDC,lpszString,nCount,lpRect,nFormat);
	else
		return ::DrawTextA(m_hDC,W2A(lpszString),nCount,lpRect,nFormat);
}

inline BOOL CDC::DrawState(const CPoint& pt,const CSize& size,LPCWSTR lpszText,UINT nFlags,BOOL bPrefixText,int nTextLen,HBRUSH hBrush)
{
	if (IsUnicodeSystem())
		return ::DrawStateW(m_hDC,hBrush,NULL,(LPARAM)lpszText,0,pt.x,pt.y,size.cx,size.cy,nFlags|(bPrefixText?DST_PREFIXTEXT:DST_TEXT));
	else
		return ::DrawStateA(m_hDC,hBrush,NULL,(LPARAM)(LPCSTR)W2A(lpszText),0,pt.x,pt.y,size.cx,size.cy,nFlags|(bPrefixText?DST_PREFIXTEXT:DST_TEXT));
}

inline BOOL CDC::TextOut(int x,int y,const CStringW& str)
{
	return ::TextOutW(m_hDC,x,y,(LPCWSTR)str,(int)str.GetLength());
}

inline BOOL CDC::ExtTextOut(int x,int y,UINT nOptions,LPCRECT lpRect,
	const CStringW& str,LPINT lpDxWidths)
{
	return ExtTextOut(x,y,nOptions,lpRect,str,(int)str.GetLength(),lpDxWidths);
}

inline int CDC::DrawText(const CStringW& str,LPRECT lpRect,UINT nFormat)
{
	return DrawText((LPCWSTR)str,(int)str.GetLength(),lpRect,nFormat);
}
#endif
///////////////////////////
// Class CMenu

inline CMenu::CMenu()
:	m_hMenu(NULL)
{
}

inline CMenu::CMenu(HMENU hMenu)
:	m_hMenu(hMenu)
{
}

inline BOOL CMenu::CreateMenu()
{	
	m_hMenu=::CreateMenu();
	DebugOpenHandle(dhtMenu,m_hMenu,STRNULL);
	if (m_hMenu==NULL)
		return FALSE;
	return TRUE;
}

inline BOOL CMenu::CreatePopupMenu()
{
	m_hMenu=::CreatePopupMenu();
	DebugOpenHandle(dhtMenu,m_hMenu,STRNULL);
	if (m_hMenu==NULL)
		return FALSE;
	return TRUE;
}

#ifdef DEF_RESOURCES
inline BOOL CMenu::LoadMenu(LPCTSTR lpszResourceName)
{
	m_hMenu=::LoadMenu(lpszResourceName);
	DebugOpenHandle(dhtMenu,m_hMenu,STRNULL);
	if (m_hMenu==NULL)
		return FALSE;
	return TRUE;
}
#endif

inline BOOL CMenu::LoadMenuIndirect(const void* lpMenuTemplate)
{
	m_hMenu=::LoadMenuIndirect(lpMenuTemplate);
	DebugOpenHandle(dhtMenu,m_hMenu,STRNULL);
	if (m_hMenu==NULL)
		return FALSE;
	return TRUE;
}

#ifdef DEF_RESOURCES
inline BOOL CMenu::LoadMenu(UINT nIDResource)
{
	return LoadMenu(MAKEINTRESOURCE(nIDResource));
}
#endif


inline CMenu::operator HMENU() const
{
	return m_hMenu;
}

inline BOOL CMenu::DeleteMenu(UINT nPosition,UINT nFlags)
{
	return ::DeleteMenu(m_hMenu,nPosition,nFlags);
}

inline BOOL CMenu::TrackPopupMenu(UINT nFlags, int x, int y,
						   HWND hWnd, LPCRECT lpRect)
{
	return ::TrackPopupMenu(m_hMenu,nFlags,x,y,0,hWnd,lpRect);
}

inline BOOL CMenu::AppendMenu(UINT nFlags, UINT nIDNewItem,LPCSTR lpszNewItem)
{
	return ::AppendMenu(m_hMenu,nFlags,nIDNewItem,lpszNewItem);
}

inline BOOL CMenu::AppendMenu(UINT nFlags, UINT nIDNewItem, const CBitmap* pBmp)
{
	return ::AppendMenu(m_hMenu,nFlags,nIDNewItem,(pBmp==NULL?NULL:(LPCTSTR)HBITMAP(*pBmp)));
}

inline UINT CMenu::CheckMenuItem(UINT nIDCheckItem, UINT nCheck)
{
	return ::CheckMenuItem(m_hMenu,nIDCheckItem,nCheck);
}

inline UINT CMenu::EnableMenuItem(UINT nIDEnableItem, UINT nEnable)
{
	return ::EnableMenuItem(m_hMenu,nIDEnableItem,nEnable);
}

inline UINT CMenu::GetMenuItemCount() const
{
	return ::GetMenuItemCount(m_hMenu);
}

inline UINT CMenu::GetMenuItemID(int nPos) const
{
	return ::GetMenuItemID(m_hMenu,nPos);
}

inline UINT CMenu::GetMenuState(UINT nID, UINT nFlags) const
{
	return ::GetMenuState(m_hMenu,nID,nFlags);
}

inline int CMenu::GetMenuString(UINT nIDItem, LPSTR lpString, int nMaxCount, UINT nFlags) const
{
	return ::GetMenuString(m_hMenu,nIDItem,lpString,nMaxCount,nFlags);
}

inline HMENU CMenu::GetSubMenu(int nPos) const
{
	return ::GetSubMenu(m_hMenu,nPos);
}

inline BOOL CMenu::InsertMenu(UINT nPosition, UINT nFlags, UINT nIDNewItem,LPCSTR lpszNewItem)
{
	return ::InsertMenu(m_hMenu,nPosition,nFlags,nIDNewItem,lpszNewItem);
}

inline BOOL CMenu::InsertMenu(UINT nPosition, UINT nFlags, UINT nIDNewItem,const CBitmap* pBmp)
{
	return ::InsertMenu(m_hMenu,nPosition,nFlags,nIDNewItem,(pBmp==NULL?NULL:(LPCTSTR)HBITMAP(*pBmp)));
}

inline BOOL CMenu::InsertMenu(UINT nItem,BOOL fByPosition,LPMENUITEMINFO lpmii)
{
	return ::InsertMenuItem(m_hMenu,nItem,fByPosition,lpmii);
}
	
inline BOOL CMenu::ModifyMenu(UINT nPosition, UINT nFlags, UINT nIDNewItem, LPCSTR lpszNewItem)
{
	return ::ModifyMenu(m_hMenu,nPosition,nFlags,nIDNewItem,lpszNewItem);
}

inline BOOL CMenu::ModifyMenu(UINT nPosition, UINT nFlags, UINT nIDNewItem,const CBitmap* pBmp)
{
	return ::ModifyMenu(m_hMenu,nPosition,nFlags,nIDNewItem,(pBmp==NULL?NULL:(LPCTSTR)HBITMAP(*pBmp)));
}

inline BOOL CMenu::RemoveMenu(UINT nPosition, UINT nFlags)
{
	return ::RemoveMenu(m_hMenu,nPosition,nFlags);
}

inline BOOL CMenu::SetMenuItemBitmaps(UINT nPosition, UINT nFlags,	const CBitmap* pBmpUnchecked, const CBitmap* pBmpChecked)
{
	return ::SetMenuItemBitmaps(m_hMenu,nPosition,nFlags,
		(pBmpUnchecked==NULL?NULL:HBITMAP(*pBmpUnchecked)),
		(pBmpChecked==NULL?NULL:HBITMAP(*pBmpChecked)));
}

inline BOOL CMenu::CheckMenuRadioItem(UINT nIDFirst, UINT nIDLast, UINT nIDItem, UINT nFlags)
{
	return ::CheckMenuRadioItem(m_hMenu,nIDFirst,nIDLast,nIDItem,nFlags);
}

inline BOOL CMenu::GetMenuItemInfo(UINT uItem,BOOL fByPosition,LPMENUITEMINFO lpmii) const
{
	return ::GetMenuItemInfo(m_hMenu,uItem,fByPosition,lpmii);
}

inline BOOL CMenu::SetMenuItemInfo(UINT uItem,BOOL fByPosition,LPMENUITEMINFO lpmii)
{
	return ::SetMenuItemInfo(m_hMenu,uItem,fByPosition,lpmii);
}

inline UINT CMenu::GetMenuDefaultItem(UINT fByPos,UINT gmdiFlags) const
{
	return ::GetMenuDefaultItem(m_hMenu,fByPos,gmdiFlags);
}

inline BOOL CMenu::SetMenuDefaultItem(UINT uItem,UINT fByPos)
{
	return ::SetMenuDefaultItem(m_hMenu,uItem,fByPos);
}

#ifdef DEF_WCHAR
inline BOOL CMenu::AppendMenu(UINT nFlags, UINT nIDNewItem,LPCWSTR lpszNewItem)
{
	if (IsUnicodeSystem())
		return ::AppendMenuW(m_hMenu,nFlags,nIDNewItem,lpszNewItem);
	else
		return ::AppendMenuA(m_hMenu,nFlags,nIDNewItem,W2A(lpszNewItem));
}

inline BOOL CMenu::InsertMenu(UINT nPosition, UINT nFlags, UINT nIDNewItem,LPCWSTR lpszNewItem)
{
	if (IsUnicodeSystem())
		return ::InsertMenuW(m_hMenu,nPosition,nFlags,nIDNewItem,lpszNewItem);
	else
		return ::InsertMenuA(m_hMenu,nPosition,nFlags,nIDNewItem,W2A(lpszNewItem));
}

inline BOOL CMenu::ModifyMenu(UINT nPosition, UINT nFlags, UINT nIDNewItem, LPCWSTR lpszNewItem)
{
	if (IsUnicodeSystem())
		return ::ModifyMenuW(m_hMenu,nPosition,nFlags,nIDNewItem,lpszNewItem);
	else
		return ::ModifyMenuA(m_hMenu,nPosition,nFlags,nIDNewItem,W2A(lpszNewItem));
}

#endif

///////////////////////////
// Class CWin

inline BOOL CWnd::Create(LPCTSTR lpszClassName,
		LPCTSTR lpszWindowName, DWORD dwStyle,
		const RECT* rect,HWND hParentWnd, UINT nID)
{
	int x=CW_USEDEFAULT,y=CW_USEDEFAULT,cx=CW_USEDEFAULT,cy=CW_USEDEFAULT;
	if (rect!=NULL)
	{
		x=rect->left;
		y=rect->top;
		cx=rect->right-rect->left;
		cy=rect->bottom-rect->top;
	}
	return (m_hWnd=CreateWindow(lpszClassName,lpszWindowName,
		dwStyle,x,y,cx,cy,hParentWnd,(HMENU)(LONG_PTR)nID,GetInstanceHandle(),NULL))!=NULL;
}

inline BOOL CWnd::CreateEx(DWORD dwExStyle, LPCTSTR lpszClassName,
		LPCTSTR lpszWindowName, DWORD dwStyle,
		int x, int y, int nWidth, int nHeight,
		HWND hWndParent, UINT nID, LPVOID lpParam)
{
	return (m_hWnd=CreateWindowEx(dwExStyle,lpszClassName,
		lpszWindowName,dwStyle,x,y,nWidth,nHeight,
		hWndParent,(HMENU)(LONG_PTR)nID,GetInstanceHandle(),lpParam))!=NULL;
}

inline BOOL CWnd::CreateEx(DWORD dwExStyle, LPCTSTR lpszClassName,
		LPCTSTR lpszWindowName, DWORD dwStyle,
		const RECT* rect,HWND hParentWnd, UINT nID,
		LPVOID lpParam)
{
	int x=CW_USEDEFAULT,y=CW_USEDEFAULT,cx=CW_USEDEFAULT,cy=CW_USEDEFAULT;
	if (rect!=NULL)
	{
		x=rect->left;
		y=rect->top;
		cx=rect->right-rect->left;
		cy=rect->bottom-rect->top;
	}
	return (m_hWnd=CreateWindowEx(dwExStyle,lpszClassName,
		lpszWindowName,dwStyle,x,y,cx,cy,
		hParentWnd,(HMENU)(LONG_PTR)nID,GetInstanceHandle(),lpParam))!=NULL;
}

inline BOOL CWnd::DestroyWindow() 
{
	return ::DestroyWindow(m_hWnd);
}


inline LONGPTR CWnd::GetWindowLong(WindowLongIndex nIndex) const 
{
	if (IsUnicodeSystem())
		return ::GetWindowLongPtrW(m_hWnd,nIndex); 
	else
		return ::GetWindowLongPtrA(m_hWnd,nIndex); 
}	

inline LONGPTR CWnd::SetWindowLong(WindowLongIndex nIndex,LONGPTR lNewLong)
{
	if (IsUnicodeSystem())
		return ::SetWindowLongPtrW(m_hWnd,nIndex,lNewLong); 
	else
		return ::SetWindowLongPtrA(m_hWnd,nIndex,lNewLong);
}


inline LONGPTR CWnd::GetClassLong(ClassLongIndex nIndex) const
{
	if (IsUnicodeSystem())
		return ::GetClassLongPtrW(m_hWnd,nIndex); 
	else
		return ::GetClassLongPtrA(m_hWnd,nIndex); 
}	

inline LONGPTR CWnd::SetClassLong(ClassLongIndex nIndex,LONGPTR lNewVal)
{ 
	if (IsUnicodeSystem())
		return ::SetClassLongPtrW(m_hWnd,nIndex,lNewVal); 
	else
		return ::SetClassLongPtrA(m_hWnd,nIndex,lNewVal); 
}


inline BOOL CWnd::GetWindowPlacement(WINDOWPLACEMENT* lpwndpl) const
{
	lpwndpl->length=sizeof(WINDOWPLACEMENT);
	return ::GetWindowPlacement(m_hWnd,lpwndpl);
}

inline void CWnd::ClientToScreen(LPRECT lpRect) const
{
	::ClientToScreen(m_hWnd,(POINT*)lpRect);
	::ClientToScreen(m_hWnd,((POINT*)lpRect)+1);
}

inline void CWnd::ScreenToClient(LPRECT lpRect) const
{
	::ScreenToClient(m_hWnd,(POINT*)lpRect);
	::ScreenToClient(m_hWnd,((POINT*)lpRect)+1);
}

inline BOOL CWnd::SetWindowPlacement(const WINDOWPLACEMENT* lpwndpl)
{
	return ::SetWindowPlacement(m_hWnd,lpwndpl);
}

inline BOOL CWnd::GetUpdateRect(LPRECT lpRect,BOOL bErase)
{
	return ::GetUpdateRect(m_hWnd,lpRect,bErase);
}

inline int CWnd::GetUpdateRgn(HRGN hRgn,BOOL bErase)
{
	return ::GetUpdateRgn(m_hWnd,hRgn,bErase);
}

inline void CWnd::Invalidate(BOOL bErase)
{
	::InvalidateRect(m_hWnd,NULL,bErase);
}

inline BOOL CWnd::InvalidateRect(CONST RECT * lpRect,BOOL bErase)
{
	return ::InvalidateRect(m_hWnd,lpRect,bErase);
}
	
inline void CWnd::InvalidateRgn(HRGN hRgn, BOOL bErase)
{
	::InvalidateRgn(m_hWnd,hRgn,bErase);
}

inline HWND CWnd::GetActiveWindow()
{
	return ::GetActiveWindow();
}

inline HWND CWnd::SetActiveWindow()
{
	return ::SetActiveWindow(m_hWnd);
}

inline BOOL CWnd::SetForegroundWindow()
{
	return ::SetForegroundWindow(m_hWnd);
}

inline HWND CWnd::GetForegroundWindow()
{
	return ::GetForegroundWindow();
}

inline HWND CWnd::GetCapture()
{
	return ::GetCapture();
}

inline HWND CWnd::SetCapture()
{
	return ::SetCapture(m_hWnd);
}
	
inline HWND CWnd::GetFocus()
{
	return (HWND)::GetFocus();
}

inline BOOL CWnd::ForceForegroundAndFocus()
{
	return ::ForceForegroundAndFocus(m_hWnd);
}

inline HWND CWnd::GetDesktopWindow()
{
	return ::GetDesktopWindow();
}

inline HICON CWnd::SetIcon(HICON hIcon,BOOL bBigIcon)
{
	return (HICON)::SendMessage(m_hWnd,WM_SETICON,(WPARAM)bBigIcon,(LPARAM)hIcon);
}

inline HICON CWnd::GetIcon(BOOL bBigIcon) const
{
	return (HICON)::SendMessage(m_hWnd,WM_GETICON,(WPARAM)bBigIcon,0);
}

inline int CWnd::MessageBox(LPCSTR lpText,LPCSTR lpCaption,UINT uType)
{
	return (int)::MessageBoxA(m_hWnd,lpText,lpCaption,uType);
}

inline int CWnd::MessageBox(LPCWSTR lpText,LPCWSTR lpCaption,UINT uType)
{
	if (IsUnicodeSystem())
		return ::MessageBoxW(m_hWnd,lpText,lpCaption,uType);
	else
		return ::MessageBoxA(m_hWnd,W2A(lpText),W2A(lpCaption),uType);
}

inline int CWnd::MessageBox(HWND hWnd,LPCWSTR lpText,LPCWSTR lpCaption,UINT uType)
{
	if (IsUnicodeSystem())
		return ::MessageBoxW(hWnd,lpText,lpCaption,uType);
	else
		return ::MessageBoxA(hWnd,W2A(lpText),W2A(lpCaption),uType);
}

inline int CWnd::ReportSystemError(LPCSTR szTitle,DWORD dwError,DWORD dwExtra,LPCSTR szPrefix)
{
	return ::ReportSystemError(m_hWnd,szTitle,dwError,dwExtra,szPrefix);
}

inline void CWnd::ShowOwnedPopups(BOOL bShow)
{
	::ShowOwnedPopups(m_hWnd,bShow);
}

inline BOOL CWnd::EnableScrollBar(int nSBFlags,UINT nArrowFlags)
{
	return ::EnableScrollBar(m_hWnd,nSBFlags,nArrowFlags);
}

inline UINT CWnd::SetTimer(UINT idTimer,UINT uTimeout,TIMERPROC tmprc)
{
	return (UINT)::SetTimer(m_hWnd,(UINT_PTR)idTimer,uTimeout,tmprc);
}

inline BOOL CWnd::KillTimer(UINT uIDEvent)
{
	return ::KillTimer(m_hWnd,uIDEvent);
}

inline BOOL CWnd::CheckDlgButton(int nIDButton,UINT  uCheck) const
{
	return ::CheckDlgButton(m_hWnd,nIDButton,uCheck);
}

inline void CWnd::CheckRadioButton(int nIDFirstButton,int nIDLastButton,int nIDCheckButton)
{
	::CheckRadioButton(m_hWnd,nIDFirstButton,nIDLastButton,nIDCheckButton);
}

inline UINT CWnd::GetDlgItemInt(int nIDDlgItem,BOOL* lpTranslated,BOOL bSigned) const
{
	return ::GetDlgItemInt(m_hWnd,nIDDlgItem,lpTranslated,bSigned);
}

inline UINT CWnd::GetDlgItemText(int nIDDlgItem,LPSTR lpString,int nMaxCount) const
{
	return ::GetDlgItemText(m_hWnd,nIDDlgItem,lpString,nMaxCount);
}

inline UINT CWnd::GetDlgItemTextLength(int nIDDlgItem) const
{
	return (UINT)::SendDlgItemMessage(m_hWnd,nIDDlgItem,WM_GETTEXTLENGTH,0,0);
}


inline HWND CWnd::GetNextDlgGroupItem(HWND hWndCtl,BOOL bPrevious) const
{
	return ::GetNextDlgGroupItem(m_hWnd,hWndCtl,bPrevious);
}

inline HWND CWnd::GetNextDlgTabItem(HWND hWndCtl,BOOL bPrevious) const
{
	return ::GetNextDlgTabItem(m_hWnd,hWndCtl,bPrevious);
}

inline UINT CWnd::IsDlgButtonChecked(int nIDButton) const
{
	return ::IsDlgButtonChecked(m_hWnd,nIDButton);
}
inline LRESULT CWnd::SendDlgItemMessage(int idControl,UINT uMsg,WPARAM wParam,LPARAM lParam) const
{
	return ::SendDlgItemMessageA(m_hWnd,idControl,uMsg,wParam,lParam);
}
inline LRESULT CWnd::SendDlgItemMessageW(int idControl,UINT uMsg,WPARAM wParam,LPARAM lParam) const
{
	return ::SendDlgItemMessageW(m_hWnd,idControl,uMsg,wParam,lParam);
}

inline BOOL CWnd::SetDlgItemInt(int idControl,UINT uValue,BOOL fSigned) const
{
	return (BOOL)::SetDlgItemInt(m_hWnd,idControl,uValue,fSigned);
}

inline BOOL CWnd::SetDlgItemText(int idControl,LPCSTR lpsz) const
{
	return (BOOL)::SetDlgItemTextA(m_hWnd,idControl,lpsz);
}

inline BOOL CWnd::SetDlgItemText(int idControl,LPCWSTR lpsz) const
{
	if (IsUnicodeSystem())
		return (BOOL)::SetDlgItemTextW(m_hWnd,idControl,lpsz);
	else
		return (BOOL)::SetDlgItemText(m_hWnd,idControl,W2A(lpsz));
}

inline void CWnd::DragAcceptFiles(BOOL bAccept)
{
	::DragAcceptFiles(m_hWnd,bAccept);
}

inline BOOL CWnd::OpenClipboard()
{
	return (BOOL)::OpenClipboard(m_hWnd);
}
	

////////////////////////////////////////
// CTargetWnd 

inline CTargetWnd::CTargetWnd(HWND hWnd)
{
	m_hWnd=hWnd;
}



inline BOOL CTargetWnd::DestroyWindow()
{
	return ::DestroyWindow(m_hWnd);
}

inline BOOL CTargetWnd::Create(LPCTSTR lpszClassName,
		LPCTSTR lpszWindowName, DWORD dwStyle,
		const RECT* rect,HWND hParentWnd, UINT nID)
{
	return (m_hWnd=CreateWindow(lpszClassName,lpszWindowName,
		dwStyle,rect->left,rect->top,rect->right-rect->left,rect->bottom-rect->top,
		hParentWnd,(HMENU)(LONG_PTR)nID,GetInstanceHandle(),NULL))!=NULL;
}

inline BOOL CTargetWnd::CreateEx(DWORD dwExStyle, LPCTSTR lpszClassName,
		LPCTSTR lpszWindowName, DWORD dwStyle,
		const RECT* rect,HWND hParentWnd, UINT nID,
		LPVOID lpParam)
{
	return (m_hWnd=CreateWindowEx(dwExStyle,lpszClassName,
		lpszWindowName,dwStyle,rect->left,rect->top,rect->right-rect->left,rect->bottom-rect->top,
		hParentWnd,(HMENU)(LONG_PTR)nID,GetInstanceHandle(),lpParam))!=NULL;
}






///////////////////////////
// Class CFrameDlg

inline CFrameWnd::CFrameWnd()
:	CTargetWnd(NULL),m_hMenu(NULL)
{
}

inline CFrameWnd::~CFrameWnd()
{
	if (m_hMenu!=NULL)
	{
		DestroyMenu(m_hMenu);
		m_hMenu=NULL;
	}
}

#if defined(DEF_RESOURCES)
inline BOOL CFrameWnd::Create(LPCTSTR lpszClassName,LPCTSTR lpszWindowName,DWORD dwStyle,
	const RECT* rect,HWND hParentWnd,UINT nMenuID,DWORD dwExStyle)
{
	return Create(lpszClassName,lpszWindowName,dwStyle,rect,hParentWnd,MAKEINTRESOURCE(nMenuID),dwExStyle);
}

inline void CFrameWnd::ActivateFrame(int nCmdShow)
{
	::ShowWindow(m_hWnd,nCmdShow);
}

///////////////////////////
// Class CMDIFrameDlg


inline CMDIFrameWnd::CMDIFrameWnd()
{
	m_hWndMDIClient=NULL;
}

inline void CMDIFrameWnd::MDIActivate(HWND hWndActivate)
{
	::SendMessage(m_hWndMDIClient,WM_MDIACTIVATE,(WPARAM)hWndActivate,0);
}

inline void CMDIFrameWnd::MDIIconArrange()
{
	::SendMessage(m_hWndMDIClient,WM_MDIICONARRANGE,0,0);
}

inline void CMDIFrameWnd::MDIMaximize(HWND hWnd)
{
	::SendMessage(m_hWndMDIClient,WM_MDIMAXIMIZE,(WPARAM)hWnd,0);
}

inline void CMDIFrameWnd::MDINext()
{
	::SendMessage(m_hWndMDIClient,WM_MDINEXT,NULL,0);
}

inline void CMDIFrameWnd::MDIRestore(HWND hWnd)
{
	::SendMessage(m_hWndMDIClient,WM_MDIRESTORE,(WPARAM)hWnd,0);
}

inline HMENU CMDIFrameWnd::MDISetMenu(HMENU hFrameMenu,HMENU hWindowMenu)
{
	return (HMENU)::SendMessage(m_hWndMDIClient,WM_MDISETMENU,(WPARAM)hFrameMenu,(LPARAM)hWindowMenu);
}

inline void CMDIFrameWnd::MDITile()
{
	::SendMessage(m_hWndMDIClient,WM_MDITILE,MDITILE_HORIZONTAL,0);
}

inline void CMDIFrameWnd::MDICascade()
{
	::SendMessage(m_hWndMDIClient,WM_MDICASCADE,MDITILE_SKIPDISABLED,0);
}

inline void CMDIFrameWnd::MDITile(int nType)
{
	::SendMessage(m_hWndMDIClient,WM_MDITILE,(WPARAM)nType,0);
}

inline void CMDIFrameWnd::MDICascade(int nType)
{
	::SendMessage(m_hWndMDIClient,WM_MDICASCADE,(WPARAM)nType,0);
}

///////////////////////////
// Class CMDIChildWnd

inline CMDIChildWnd::CMDIChildWnd()
{
	m_pParent=NULL;
}

inline CMDIFrameWnd* CMDIChildWnd::GetMDIFrame()
{
	return m_pParent;
}

inline void CMDIChildWnd::MDIDestroy()
{
	::SendMessage(m_pParent->m_hWndMDIClient,WM_MDIDESTROY,(WPARAM)m_hWnd,0);
}

inline void CMDIChildWnd::MDIActivate()
{
	m_pParent->MDIActivate(m_hWnd);
}

inline void CMDIChildWnd::MDIMaximize()
{
	m_pParent->MDIMaximize(m_hWnd);
}

inline void CMDIChildWnd::MDIRestore()
{
	m_pParent->MDIRestore(m_hWnd);
}

///////////////////////////
// Class CMDIChildWndList

inline CMDIChildWndList::CMDIChildWndList()
{
	m_pNodeHead=NULL;
	m_pNodeTail=NULL;
	m_nCount=0;
}

inline CMDIChildWndList::~CMDIChildWndList()
{
	RemoveAllWindows();
}


#endif

#endif
