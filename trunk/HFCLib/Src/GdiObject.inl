////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////

#ifndef HFCGDIOBJECT_INL
#define HFCGDIOBJECT_INL

/////////////////////////////////////////////////
// CGdiObject

#ifdef WIN32

inline CGdiObject::CGdiObject()
:	m_hObject(NULL)
{
}

inline CGdiObject::CGdiObject(HGDIOBJ hObject)
:	m_hObject(hObject)
{
}

inline CGdiObject::~CGdiObject()
{
	if (m_hObject!=NULL)
	{
		::DeleteObject(m_hObject);
		DebugCloseHandle(dhtGdiObject,m_hObject,STRNULL);
	}
}

inline CGdiObject::operator HGDIOBJ() const
{
	return m_hObject;
}

inline HGDIOBJ CGdiObject::GetSafeHandle() const
{
	return m_hObject;
}

inline int CGdiObject::GetObject(int nCount,LPVOID lpObject) const
{
	return ::GetObject(m_hObject,nCount,lpObject);
}

inline DWORD CGdiObject::GetObjectType() const
{
	return ::GetObjectType(m_hObject);
}

inline BOOL CGdiObject::UnrealizeObject()
{
	return ::UnrealizeObject(m_hObject);
}

inline BOOL CGdiObject::operator==(const CGdiObject& obj) const
{
	return (m_hObject==obj.m_hObject);
}

inline BOOL CGdiObject::operator!=(const CGdiObject& obj) const
{
	return (m_hObject!=obj.m_hObject);
}

inline BOOL CGdiObject::CreateStockObject(int nIndex)
{
	if (m_hObject!=NULL)
	{
		::DeleteObject(m_hObject);
		DebugCloseHandle(dhtGdiObject,m_hObject,STRNULL);
	}

	m_hObject=(HGDIOBJ)::GetStockObject(nIndex);
	DebugOpenHandle(dhtGdiObject,m_hObject,STRNULL);
	return m_hObject!=NULL;
}
	
///////////////////////////
// Class CPen

inline CPen::CPen()
:	CGdiObject(NULL)
{
}

inline CPen::CPen(HPEN hPen)
:	CGdiObject(hPen)
{
}

inline CPen::CPen(int nPenStyle,int nWidth,COLORREF crColor)
:	CGdiObject(NULL)
{
	m_hObject=(HGDIOBJ)::CreatePen(nPenStyle,nWidth,crColor);
	DebugOpenHandle(dhtGdiObject,m_hObject,STRNULL);
}

inline CPen::CPen(int nPenStyle,int nWidth,const LOGBRUSH* pLogBrush,int nStyleCount,const DWORD* lpStyle)
:	CGdiObject(NULL)
{
	m_hObject=(HGDIOBJ)::ExtCreatePen(nPenStyle,nWidth,pLogBrush,nStyleCount,lpStyle);
	DebugOpenHandle(dhtGdiObject,m_hObject,STRNULL);
}

inline CPen::operator HPEN() const
{
	return (HPEN) m_hObject;
}

inline int CPen::GetLogPen(LOGPEN* pLogPen)
{
	return ::GetObject(m_hObject,sizeof(LOGPEN),pLogPen);
}

inline int CPen::GetExtLogPen(EXTLOGPEN* pLogPen)
{
	return ::GetObject(m_hObject,sizeof(EXTLOGPEN),pLogPen);
}

inline BOOL CPen::CreatePen(int nPenStyle,int nWidth,COLORREF crColor)
{
	if (m_hObject!=NULL)
	{
		::DeleteObject(m_hObject);
		DebugCloseHandle(dhtGdiObject,m_hObject,STRNULL);
	}

	m_hObject=(HGDIOBJ)::CreatePen(nPenStyle,nWidth,crColor);
	DebugOpenHandle(dhtGdiObject,m_hObject,STRNULL);
	return m_hObject!=NULL;
}

inline BOOL CPen::CreatePen(int nPenStyle,int nWidth,const LOGBRUSH* pLogBrush,int nStyleCount,const DWORD* lpStyle)
{
	if (m_hObject!=NULL)
	{
		::DeleteObject(m_hObject);
		DebugCloseHandle(dhtGdiObject,m_hObject,STRNULL);
	}

	m_hObject=(HGDIOBJ)::ExtCreatePen(nPenStyle,nWidth,pLogBrush,nStyleCount,lpStyle);
	DebugOpenHandle(dhtGdiObject,m_hObject,STRNULL);
	return m_hObject!=NULL;

}

inline BOOL CPen::CreatePenIndirect(LPLOGPEN lpLogPen)
{
	if (m_hObject!=NULL)
	{
		::DeleteObject(m_hObject);
		DebugCloseHandle(dhtGdiObject,m_hObject,STRNULL);
	}
	
	m_hObject=(HGDIOBJ)::CreatePenIndirect(lpLogPen);
	DebugOpenHandle(dhtGdiObject,m_hObject,STRNULL);
	return m_hObject!=NULL;
}

///////////////////////////
// Class CBrush

inline CBrush::CBrush()
:	CGdiObject(NULL)
{
}

inline CBrush::CBrush(HBRUSH hObject)
:	CGdiObject(hObject)
{
}

inline CBrush::CBrush(COLORREF crColor)
:	CGdiObject(NULL)
{
	m_hObject=(HGDIOBJ)::CreateSolidBrush(crColor);
	DebugOpenHandle(dhtGdiObject,m_hObject,STRNULL);
}

inline CBrush::CBrush(int nIndex,COLORREF crColor)
:	CGdiObject(NULL)
{
	m_hObject=(HGDIOBJ)::CreateHatchBrush(nIndex,crColor);
	DebugOpenHandle(dhtGdiObject,m_hObject,STRNULL);
}

inline CBrush::operator HBRUSH() const
{
	return (HBRUSH) m_hObject;
}

inline int CBrush::GetLogBrush(LOGBRUSH* pLogBrush)
{
	return ::GetObjectA(m_hObject,sizeof(LOGBRUSH),pLogBrush);
}

inline BOOL CBrush::CreateSolidBrush(COLORREF crColor)
{
	if (m_hObject!=NULL)
	{
		::DeleteObject(m_hObject);
		DebugCloseHandle(dhtGdiObject,m_hObject,STRNULL);
	}

	m_hObject=(HGDIOBJ)::CreateSolidBrush(crColor);
	DebugOpenHandle(dhtGdiObject,m_hObject,STRNULL);
	return m_hObject!=NULL;
}

inline BOOL CBrush::CreateHatchBrush(int nIndex,COLORREF crColor)
{
	if (m_hObject!=NULL)
	{
		::DeleteObject(m_hObject);
		DebugCloseHandle(dhtGdiObject,m_hObject,STRNULL);
	}

	m_hObject=(HGDIOBJ)::CreateHatchBrush(nIndex,crColor);
	DebugOpenHandle(dhtGdiObject,m_hObject,STRNULL);
	return m_hObject!=NULL;

}

inline BOOL CBrush::CreateBrushIndirect(const LOGBRUSH* lpLogBrush)
{
	if (m_hObject!=NULL)
	{
		::DeleteObject(m_hObject);
		DebugCloseHandle(dhtGdiObject,m_hObject,STRNULL);
	}

	m_hObject=(HGDIOBJ)::CreateBrushIndirect(lpLogBrush);
	DebugOpenHandle(dhtGdiObject,m_hObject,STRNULL);
	return m_hObject!=NULL;
}

inline BOOL CBrush::CreatePatternBrush(HBITMAP hBitmap)
{
	if (m_hObject!=NULL)
	{
		::DeleteObject(m_hObject);
		DebugCloseHandle(dhtGdiObject,m_hObject,STRNULL);
	}

	m_hObject=(HGDIOBJ)::CreatePatternBrush(hBitmap);
	DebugOpenHandle(dhtGdiObject,m_hObject,STRNULL);
	return m_hObject!=NULL;
}

inline BOOL CBrush::CreateDIBPatternBrush(HGLOBAL hPackedDIB,UINT nUsage)
{
	if (m_hObject!=NULL)
	{
		::DeleteObject(m_hObject);
		DebugCloseHandle(dhtGdiObject,m_hObject,STRNULL);
	}

	m_hObject=(HGDIOBJ)::CreateDIBPatternBrush(hPackedDIB,nUsage);
	DebugOpenHandle(dhtGdiObject,m_hObject,STRNULL);
	return m_hObject!=NULL;
}

inline BOOL CBrush::CreateDIBPatternBrush(const void* lpPackedDIB,UINT nUsage)
{
	if (m_hObject!=NULL)
	{
		::DeleteObject(m_hObject);
		DebugCloseHandle(dhtGdiObject,m_hObject,STRNULL);
	}

	m_hObject=(HGDIOBJ)::CreateDIBPatternBrushPt(lpPackedDIB,nUsage);
	DebugOpenHandle(dhtGdiObject,m_hObject,STRNULL);
	return m_hObject!=NULL;
}

inline BOOL CBrush::CreateSysColorBrush(int nIndex)
{
	if (m_hObject!=NULL)
	{
		::DeleteObject(m_hObject);
		DebugCloseHandle(dhtGdiObject,m_hObject,STRNULL);
	}

	m_hObject=(HGDIOBJ)::GetSysColorBrush(nIndex);
	DebugOpenHandle(dhtGdiObject,m_hObject,STRNULL);
	return m_hObject!=NULL;
}

///////////////////////////
// Class CFont

inline CFont::CFont()
:	CGdiObject(NULL)
{
}

inline CFont::CFont(HFONT hObject)
:	CGdiObject(hObject)
{
}

inline CFont::operator HFONT() const
{
	return (HFONT) m_hObject;
}

inline int CFont::GetLogFont(LOGFONT* pLogFont)
{
	return ::GetObject(m_hObject,sizeof(LOGFONT),pLogFont);
}

inline BOOL CFont::CreateFontIndirect(const LOGFONTA* lpLogFont)
{
	if (m_hObject!=NULL)
	{
		::DeleteObject(m_hObject);
		DebugCloseHandle(dhtGdiObject,m_hObject,STRNULL);
	}
	
	m_hObject=(HFONT)::CreateFontIndirectA(lpLogFont);
	DebugOpenHandle(dhtGdiObject,m_hObject,STRNULL);
	return m_hObject!=NULL;
}

inline BOOL CFont::CreateFontIndirect(const LOGFONTW* lpLogFont)
{
	if (m_hObject!=NULL)
	{
		::DeleteObject(m_hObject);
		DebugCloseHandle(dhtGdiObject,m_hObject,STRNULL);
	}

	m_hObject=(HFONT)::CreateFontIndirectW(lpLogFont);
	DebugOpenHandle(dhtGdiObject,m_hObject,STRNULL);
	return m_hObject!=NULL;
}

///////////////////////////
// Class CBitmap

inline CBitmap::CBitmap()
:	CGdiObject()
{
}

inline CBitmap::CBitmap(HBITMAP hObject)
:	CGdiObject(hObject)
{
}

inline BOOL CBitmap::LoadBitmap(UINT nIDResource)
{
	return LoadBitmap(MAKEINTRESOURCE(nIDResource));
}

inline CBitmap::operator HBITMAP() const
{
	return (HBITMAP) m_hObject;
}

inline DWORD CBitmap::SetBitmapBits(DWORD dwCount, const void* lpBits)
{
	return ::SetBitmapBits((HBITMAP)m_hObject,dwCount,lpBits);
}

inline DWORD CBitmap::GetBitmapBits(DWORD dwCount, LPVOID lpBits) const
{
	return ::GetBitmapBits((HBITMAP)m_hObject,dwCount,lpBits);
}

HINSTANCE GetResourceHandle(TypeOfResourceHandle bType);
HINSTANCE GetCommonResourceHandle();
HINSTANCE GetLanguageSpecificResourceHandle();

inline BOOL CBitmap::LoadBitmap(LPCTSTR lpszResourceName)
{
	if (m_hObject!=NULL)
	{
		::DeleteObject(m_hObject);
		DebugCloseHandle(dhtGdiObject,m_hObject,STRNULL);
	}

#ifdef DEF_RESOURCES
	m_hObject=(HGDIOBJ)::LoadBitmap(GetCommonResourceHandle(),lpszResourceName);
	DebugOpenHandle(dhtGdiObject,m_hObject,STRNULL);
#endif
	return m_hObject!=NULL;
}

inline BOOL CBitmap::LoadOEMBitmap(UINT nIDBitmap)
{
	if (m_hObject!=NULL)
	{
		::DeleteObject(m_hObject);
		DebugCloseHandle(dhtGdiObject,m_hObject,STRNULL);
	}

	m_hObject=(HGDIOBJ)::LoadBitmap(NULL,MAKEINTRESOURCE(nIDBitmap));
	DebugOpenHandle(dhtGdiObject,m_hObject,STRNULL);
	return m_hObject!=NULL;
}

inline BOOL CBitmap::LoadOEMBitmap(UINT nIDBitmap,int sizeX,int sizeY)
{
	if (m_hObject!=NULL)
	{
		::DeleteObject(m_hObject);
		DebugCloseHandle(dhtGdiObject,m_hObject,STRNULL);
	}

	m_hObject=(HGDIOBJ)::LoadImage(NULL,MAKEINTRESOURCE(nIDBitmap),
		IMAGE_BITMAP,sizeX,sizeY,LR_DEFAULTCOLOR);
	DebugOpenHandle(dhtGdiObject,m_hObject,STRNULL);
	return m_hObject!=NULL;
}


inline BOOL CBitmap::CreateBitmap(int nWidth, int nHeight, UINT nPlanes, UINT nBitcount,
	const void* lpBits)
{
	if (m_hObject!=NULL)
	{
		::DeleteObject(m_hObject);
		DebugCloseHandle(dhtGdiObject,m_hObject,STRNULL);
	}

	m_hObject=(HGDIOBJ)::CreateBitmap(nWidth,nHeight,nPlanes,nBitcount,lpBits);
	DebugOpenHandle(dhtGdiObject,m_hObject,STRNULL);
	return m_hObject!=NULL;
}

inline BOOL CBitmap::CreateBitmapIndirect(LPBITMAP lpBitmap)
{
	if (m_hObject!=NULL)
	{
		::DeleteObject(m_hObject);
		DebugCloseHandle(dhtGdiObject,m_hObject,STRNULL);
	}

	m_hObject=(HGDIOBJ)::CreateBitmapIndirect(lpBitmap);
	DebugOpenHandle(dhtGdiObject,m_hObject,STRNULL);
	return m_hObject!=NULL;
}

inline BOOL CBitmap::CreateCompatibleBitmap(HDC hDC, int nWidth, int nHeight)
{
	if (m_hObject!=NULL)
	{
		::DeleteObject(m_hObject);
		DebugCloseHandle(dhtGdiObject,m_hObject,STRNULL);
	}

	m_hObject=(HGDIOBJ)::CreateCompatibleBitmap(hDC,nWidth,nHeight);
	DebugOpenHandle(dhtGdiObject,m_hObject,STRNULL);
	return m_hObject!=NULL;
}

inline BOOL CBitmap::CreateDIBitmap(HDC hdc,CONST BITMAPINFOHEADER *lpbmih,DWORD fdwInit,CONST VOID *lpbInit,CONST BITMAPINFO *lpbmi,UINT fuUsage)
{
	if (m_hObject!=NULL)
	{
		::DeleteObject(m_hObject);
		DebugCloseHandle(dhtGdiObject,m_hObject,STRNULL);
	}

	m_hObject=(HGDIOBJ)::CreateDIBitmap(hdc,lpbmih,fdwInit,lpbInit,lpbmi,fuUsage);
	DebugOpenHandle(dhtGdiObject,m_hObject,STRNULL);
	return m_hObject!=NULL;
}
	
inline BOOL CBitmap::CreateDIBSection(HDC hdc,CONST BITMAPINFO *pbmi,UINT iUsage,VOID **ppvBits,HANDLE hSection,DWORD dwOffset)
{
	if (m_hObject!=NULL)
	{
		::DeleteObject(m_hObject);
		DebugCloseHandle(dhtGdiObject,m_hObject,STRNULL);
	}

	m_hObject=(HGDIOBJ)::CreateDIBSection(hdc,pbmi,iUsage,ppvBits,hSection,dwOffset);
	DebugOpenHandle(dhtGdiObject,m_hObject,STRNULL);
	return m_hObject!=NULL;
}


inline CSize CBitmap::SetBitmapDimension(int nWidth, int nHeight)
{
	CSize size;
	::SetBitmapDimensionEx((HBITMAP)m_hObject,nWidth,nHeight,&size);
	return size;
}

inline CSize CBitmap::GetBitmapDimension() const
{
	CSize size;
	::GetBitmapDimensionEx((HBITMAP)m_hObject,&size);
	return size;
}

inline BOOL CBitmap::GetBitmapDimension(CSize& dim) const
{
	return ::GetBitmapDimensionEx((HBITMAP)m_hObject,&dim);
}

inline CSize CBitmap::GetBitmapSize(HDC hDC) const
{
	CSize sz;
	GetBitmapSize(sz,hDC);
	return sz;
}

///////////////////////////
// Class CPalette

inline CPalette::operator HPALETTE() const
{
	return (HPALETTE)m_hObject;
}

inline int CPalette::GetEntryCount()
{
	return ::GetPaletteEntries((HPALETTE)m_hObject,0,0,NULL);
}

inline UINT CPalette::GetPaletteEntries(UINT nStartIndex, UINT nNumEntries,
		LPPALETTEENTRY lpPaletteColors) const
{
	return ::GetPaletteEntries((HPALETTE)m_hObject,nStartIndex,nNumEntries,lpPaletteColors);
}

inline UINT CPalette::SetPaletteEntries(UINT nStartIndex, UINT nNumEntries,
		LPPALETTEENTRY lpPaletteColors)
{
	return ::SetPaletteEntries((HPALETTE)m_hObject,nStartIndex,nNumEntries,lpPaletteColors);
}

inline void CPalette::AnimatePalette(UINT nStartIndex, UINT nNumEntries,
		LPPALETTEENTRY lpPaletteColors)
{
	::AnimatePalette((HPALETTE)m_hObject,nStartIndex,nNumEntries,lpPaletteColors);
}

inline UINT CPalette::GetNearestPaletteIndex(COLORREF crColor) const
{
	return ::GetNearestPaletteIndex((HPALETTE)m_hObject,crColor);
}

inline BOOL CPalette::ResizePalette(UINT nNumEntries)
{
	return ::ResizePalette((HPALETTE)m_hObject,nNumEntries);
}

inline CPalette::CPalette()
:	CGdiObject(NULL)
{
}

inline CPalette::CPalette(HPALETTE hObject)
:	CGdiObject(hObject)
{
}

inline BOOL CPalette::CreatePalette(LPLOGPALETTE lpLogPalette)
{
	if (m_hObject!=NULL)
	{
		::DeleteObject(m_hObject);
		DebugCloseHandle(dhtGdiObject,m_hObject,STRNULL);
	}
	m_hObject=(HGDIOBJ)::CreatePalette(lpLogPalette);
	DebugOpenHandle(dhtGdiObject,m_hObject,STRNULL);
	return m_hObject!=NULL;
}

inline BOOL CPalette::CreateHalftonePalette(HDC hDC)
{
	if (m_hObject!=NULL)
	{
		::DeleteObject(m_hObject);
		DebugCloseHandle(dhtGdiObject,m_hObject,STRNULL);
	}
	m_hObject=(HGDIOBJ)::CreateHalftonePalette(hDC);
	DebugOpenHandle(dhtGdiObject,m_hObject,STRNULL);
	return m_hObject!=NULL;
}

///////////////////////////
// Class CRgn

inline CRgn::operator HRGN() const
{
	return (HRGN) m_hObject;
}

inline void CRgn::SetRectRgn(int x1, int y1, int x2, int y2)
{
	::SetRectRgn((HRGN)m_hObject,x1,y1,x2,y2);
}

inline void CRgn::SetRectRgn(LPCRECT lpRect)
{
	::SetRectRgn((HRGN)m_hObject,lpRect->left,lpRect->top,lpRect->right,lpRect->bottom);
}

inline int CRgn::CombineRgn(HRGN hRgn1,HRGN hRgn2,int nCombineMode)
{
	return ::CombineRgn((HRGN)m_hObject,hRgn1,hRgn2,nCombineMode);
}

inline BOOL CRgn::EqualRgn(HRGN hRgn) const
{
	return ::EqualRgn((HRGN)m_hObject,hRgn);
}

inline int CRgn::OffsetRgn(int x, int y)
{
	return ::OffsetRgn((HRGN)m_hObject,x,y);
}

inline int CRgn::OffsetRgn(POINT point)
{
	return ::OffsetRgn((HRGN)m_hObject,point.x,point.y);
}

inline int CRgn::GetRgnBox(LPRECT lpRect) const
{
	return ::GetRgnBox((HRGN)m_hObject,lpRect);
}

inline BOOL CRgn::PtInRegion(int x, int y) const
{
	return ::PtInRegion((HRGN)m_hObject,x,y);
}

inline BOOL CRgn::PtInRegion(POINT point) const
{
	return ::PtInRegion((HRGN)m_hObject,point.x,point.y);
}

inline BOOL CRgn::RectInRegion(LPCRECT lpRect) const
{
	return ::RectInRegion((HRGN)m_hObject,lpRect);
}

inline int CRgn::GetRegionData(LPRGNDATA lpRgnData, int nCount) const
{
	return ::GetRegionData((HRGN)m_hObject,nCount,lpRgnData);
}

inline CRgn::CRgn()
:	CGdiObject(NULL)
{
}

inline CRgn::CRgn(HRGN hObject)
:	CGdiObject(hObject)
{
}

inline BOOL CRgn::CreateRectRgn(int x1, int y1, int x2, int y2)
{
	if (m_hObject!=NULL)
	{
		::DeleteObject(m_hObject);
		DebugCloseHandle(dhtGdiObject,m_hObject,STRNULL);
	}
	m_hObject=(HGDIOBJ)::CreateRectRgn(x1,y1,x2,y2);
	DebugOpenHandle(dhtGdiObject,m_hObject,STRNULL);
	return m_hObject!=NULL;
}

inline BOOL CRgn::CreateRectRgnIndirect(LPCRECT lpRect)
{
	if (m_hObject!=NULL)
	{
		::DeleteObject(m_hObject);
		DebugCloseHandle(dhtGdiObject,m_hObject,STRNULL);
	}
	m_hObject=(HGDIOBJ)::CreateRectRgnIndirect(lpRect);
	DebugOpenHandle(dhtGdiObject,m_hObject,STRNULL);
	return m_hObject!=NULL;
}

inline BOOL CRgn::CreateEllipticRgn(int x1, int y1, int x2, int y2)
{
	if (m_hObject!=NULL)
	{
		::DeleteObject(m_hObject);
		DebugCloseHandle(dhtGdiObject,m_hObject,STRNULL);
	}
	m_hObject=(HGDIOBJ)::CreateEllipticRgn(x1,y1,x2,y2);
	DebugOpenHandle(dhtGdiObject,m_hObject,STRNULL);
	return m_hObject!=NULL;
}

inline BOOL CRgn::CreateEllipticRgnIndirect(LPCRECT lpRect)
{
	if (m_hObject!=NULL)
	{
		::DeleteObject(m_hObject);
		DebugCloseHandle(dhtGdiObject,m_hObject,STRNULL);
	}
	m_hObject=(HGDIOBJ)::CreateEllipticRgnIndirect(lpRect);
	DebugOpenHandle(dhtGdiObject,m_hObject,STRNULL);
	return m_hObject!=NULL;
}

inline BOOL CRgn::CreatePolygonRgn(LPPOINT lpPoints, int nCount, int nMode)
{
	if (m_hObject!=NULL)
	{
		::DeleteObject(m_hObject);
		DebugCloseHandle(dhtGdiObject,m_hObject,STRNULL);
	}
	m_hObject=(HGDIOBJ)::CreatePolygonRgn(lpPoints,nCount,nMode);
	DebugOpenHandle(dhtGdiObject,m_hObject,STRNULL);
	return m_hObject!=NULL;
}

inline BOOL CRgn::CreatePolyPolygonRgn(LPPOINT lpPoints, LPINT lpPolyCounts,
		int nCount, int nPolyFillMode)
{
	if (m_hObject!=NULL)
	{
		::DeleteObject(m_hObject);
		DebugCloseHandle(dhtGdiObject,m_hObject,STRNULL);
	}
	m_hObject=(HGDIOBJ)::CreatePolyPolygonRgn(lpPoints,lpPolyCounts,nCount,nPolyFillMode);
	DebugOpenHandle(dhtGdiObject,m_hObject,STRNULL);
	return m_hObject!=NULL;
}

inline BOOL CRgn::CreateRoundRectRgn(int x1,int y1,int x2,int y2,int x3, int y3)
{
	if (m_hObject!=NULL)
	{
		::DeleteObject(m_hObject);
		DebugCloseHandle(dhtGdiObject,m_hObject,STRNULL);
	}
	m_hObject=(HGDIOBJ)::CreateRoundRectRgn(x1,y1,x2,y2,x3,y3);
	DebugOpenHandle(dhtGdiObject,m_hObject,STRNULL);
	return m_hObject!=NULL;
}

inline BOOL CRgn::CreateFromPath(HDC hDC)
{
	if (m_hObject!=NULL)
	{
		::DeleteObject(m_hObject);
		DebugCloseHandle(dhtGdiObject,m_hObject,STRNULL);
	}
	m_hObject=(HGDIOBJ)::PathToRegion(hDC);
	DebugOpenHandle(dhtGdiObject,m_hObject,STRNULL);
	return m_hObject!=NULL;
}
	
inline BOOL CRgn::CreateFromData(const XFORM* lpXForm, int nCount,
	const RGNDATA* pRgnData)
{
	if (m_hObject!=NULL)
	{
		::DeleteObject(m_hObject);
		DebugCloseHandle(dhtGdiObject,m_hObject,STRNULL);
	}
	m_hObject=(HGDIOBJ)::ExtCreateRegion(lpXForm,nCount,pRgnData);
	DebugOpenHandle(dhtGdiObject,m_hObject,STRNULL);
	return m_hObject!=NULL;
}

///////////////////////////
// Class CWaitCursor


inline CWaitCursor::CWaitCursor()
{
	m_hOldCursor=SetCursor(LoadCursor(0,IDC_WAIT));
}

inline CWaitCursor::~CWaitCursor()
{
	Restore();
}

inline void CWaitCursor::Restore()
{
	SetCursor(m_hOldCursor);
}

#endif

#endif
