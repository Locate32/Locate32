////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////
#ifndef HFCCOMMONCONTROLS_INL
#define HFCCOMMONCONTROLS_INL

///////////////////////////////////////////////
// CCommonCtrl

inline CCommonCtrl::CCommonCtrl(HWND hWnd)
:	CWnd(hWnd)
{
}

inline BOOL CCommonCtrl::SetBkColor(COLORREF bkColor)
{
	return (BOOL)::SendMessage(m_hWnd,CCM_SETBKCOLOR,0,(LPARAM)bkColor);
}

inline BOOL CCommonCtrl::SetColorScheme(const COLORSCHEME* pScheme)
{
	return (BOOL)::SendMessage(m_hWnd,CCM_SETCOLORSCHEME,0,(LPARAM)pScheme);
}

inline BOOL CCommonCtrl::GetColorScheme(COLORSCHEME* pScheme) const
{
	return (BOOL)::SendMessage(m_hWnd,CCM_GETCOLORSCHEME,0,(LPARAM)pScheme);
}

inline HWND CCommonCtrl::GetDropTarget() const
{
	return (HWND)::SendMessage(m_hWnd,CCM_GETDROPTARGET,0,0);
}

inline BOOL CCommonCtrl::SetUnicodeFormat(BOOL nFormat)
{
	return (BOOL)::SendMessage(m_hWnd,CCM_SETUNICODEFORMAT,(LPARAM)nFormat,0);
}

inline BOOL CCommonCtrl::GetUnicodeFormat() const
{
	return (BOOL)::SendMessage(m_hWnd,CCM_GETUNICODEFORMAT,0,0);
}

inline UINT CCommonCtrl::SetVersion(UINT nVersion)
{
	return (UINT)::SendMessage(m_hWnd,CCM_SETVERSION,nVersion,0);
}

inline UINT CCommonCtrl::GetVersion() const
{
	return (UINT)::SendMessage(m_hWnd,CCM_GETVERSION,0,0);
}

inline BOOL CCommonCtrl::SetNotifyWindow(HWND hWndParent)
{
	return (BOOL)::SendMessage(m_hWnd,CCM_SETNOTIFYWINDOW,(WPARAM)hWndParent,0);
}

///////////////////////////////////////////////
// CImageList

#ifdef DEF_RESOURCES
inline CImageList::CImageList()
{
	m_hImageList=NULL;
}

inline CImageList::CImageList(HIMAGELIST hImagelist)
{
	m_hImageList=hImagelist;
}

inline CImageList::~CImageList()
{
	if (m_hImageList!=NULL)
		ImageList_Destroy(m_hImageList);
}

inline void CImageList::Attach(HIMAGELIST hImagelist)
{
	if (m_hImageList!=NULL)
		ImageList_Destroy(m_hImageList);
	m_hImageList=hImagelist;
}

inline BOOL CImageList::Create(UINT nBitmapID, int cx, int nGrow, COLORREF crMask)
{
	if (m_hImageList!=NULL)
		ImageList_Destroy(m_hImageList);
	return (m_hImageList=ImageList_LoadBitmap(GetCommonResourceHandle(),MAKEINTRESOURCE(nBitmapID),cx,nGrow,crMask))!=NULL;
}

inline BOOL CImageList::Create(LPCTSTR lpszBitmapID, int cx, int nGrow, COLORREF crMask)
{
	if (m_hImageList!=NULL)
		ImageList_Destroy(m_hImageList);
	return (m_hImageList=ImageList_LoadBitmap(GetCommonResourceHandle(),lpszBitmapID,cx,nGrow,crMask))!=NULL;
}

inline BOOL CImageList::Create(int cx,int cy,UINT nFlags,int nInitial,int nGrow)
{
	if (m_hImageList!=NULL)
		ImageList_Destroy(m_hImageList);
	return (m_hImageList=ImageList_Create(cx,cy,nFlags,nInitial,nGrow))!=NULL;
}

inline BOOL CImageList::Create(HIMAGELIST imagelist1, int nImage1, HIMAGELIST imagelist2,
	int nImage2, int dx, int dy)
{
	if (m_hImageList!=NULL)
		ImageList_Destroy(m_hImageList);
	return (m_hImageList=ImageList_Merge(imagelist1,nImage1,imagelist2,nImage2,dx,dy))!=NULL;
}

inline BOOL CImageList::Create(LPCTSTR lpbmp,int cx,int cGrow,COLORREF crMask,UINT uFlags,BOOL nOem)
{
	if (m_hImageList!=NULL)
		ImageList_Destroy(m_hImageList);
	return (m_hImageList=ImageList_LoadImage(nOem?NULL:GetCommonResourceHandle(),lpbmp,cx,cGrow,crMask,IMAGE_BITMAP,uFlags))!=NULL;
}

inline BOOL CImageList::Create(UINT uBitmapID,int cx,int cGrow,COLORREF crMask,UINT uFlags,BOOL nOem)
{
	if (m_hImageList!=NULL)
		ImageList_Destroy(m_hImageList);
	return (m_hImageList=ImageList_LoadImage(nOem?NULL:GetCommonResourceHandle(),MAKEINTRESOURCE(uBitmapID),cx,cGrow,crMask,IMAGE_BITMAP,uFlags))!=NULL;
}

inline BOOL CImageList::Duplicate(HIMAGELIST himl)
{
	if (m_hImageList!=NULL)
		ImageList_Destroy(m_hImageList);
	return (m_hImageList=ImageList_Duplicate(himl))!=NULL;
}

inline BOOL CImageList::Read(LPSTREAM pstm)
{
	if (m_hImageList!=NULL)
		ImageList_Destroy(m_hImageList);
	return (m_hImageList=ImageList_Read(pstm))!=NULL;
}
   
inline HICON CImageList::ExtractIcon(int nImage)
{
	return ImageList_ExtractIcon(GetResourceHandle(),m_hImageList,nImage);
}

inline CImageList::operator HIMAGELIST() const
{
	return m_hImageList;
}

inline HIMAGELIST CImageList::GetSafeHandle() const
{
	return m_hImageList;
}

inline int CImageList::GetImageCount() const
{
	return ImageList_GetImageCount(m_hImageList);
}

inline BOOL CImageList::SetImageCount(UINT uNewCount)
{
	return ImageList_SetImageCount(m_hImageList,uNewCount);
}

inline COLORREF CImageList::SetBkColor(COLORREF cr)
{
	return ImageList_SetBkColor(m_hImageList,cr);
}

inline COLORREF CImageList::GetBkColor() const
{
	return ImageList_GetBkColor(m_hImageList);
}

inline BOOL CImageList::GetIconSize(int *cx,int *cy)
{
	return ImageList_GetIconSize(m_hImageList,cx,cy);
}

inline BOOL CImageList::SetIconSize(int cx,int cy)
{
	return ImageList_SetIconSize(m_hImageList,cx,cy);
}
	
inline BOOL CImageList::GetImageInfo(int nImage, IMAGEINFO* pImageInfo) const
{
	return ImageList_GetImageInfo(m_hImageList,nImage,pImageInfo);
}

inline HIMAGELIST CImageList::Merge(int i1,HIMAGELIST himl2,int i2,int dx,int dy)
{
	return ImageList_Merge(m_hImageList,i1,himl2,i2,dx,dy);
}
	
inline int CImageList::Add(HBITMAP hbmImage, HBITMAP hbmMask)
{
	return ImageList_Add(m_hImageList,hbmImage,hbmMask);
}


inline int CImageList::Add(HBITMAP hbmImage, COLORREF crMask)
{
	return ImageList_AddMasked(m_hImageList,hbmImage,crMask);
}

inline BOOL CImageList::Remove(int nImage)
{
	return ImageList_Remove(m_hImageList,nImage);
}

inline BOOL CImageList::RemoveAll()
{
	return ImageList_RemoveAll(m_hImageList);
}
	
inline BOOL CImageList::Replace(int nImage, HBITMAP hbmImage, HBITMAP hbmMask)
{
	return ImageList_Replace(m_hImageList,nImage,hbmImage,hbmMask);
}

inline int CImageList::Add(HICON hIcon)
{
	return ImageList_AddIcon(m_hImageList,hIcon);
}

inline int CImageList::Replace(int nImage, HICON hIcon)
{
	return ImageList_ReplaceIcon(m_hImageList,nImage,hIcon);
}

inline BOOL CImageList::Draw(HDC hDC, int nImage, POINT pt, UINT nStyle)
{
	return ImageList_Draw(m_hImageList,nImage,hDC,pt.x,pt.y,nStyle);
}

inline BOOL CImageList::DrawEx(HDC hDC,int nImage,int x,int y,int dx,int dy,COLORREF rgbBk,COLORREF rgbFg,UINT fStyle)
{
	return ImageList_DrawEx(m_hImageList,nImage,hDC,x,y,dx,dy,rgbBk,rgbFg,fStyle);
}

inline BOOL CImageList::DrawIndirect(IMAGELISTDRAWPARAMS* pimldp)
{
	return ImageList_DrawIndirect(pimldp);
}

inline HICON CImageList::GetIcon(int i,UINT flags)
{
	return ImageList_GetIcon(m_hImageList,i,flags);
}

inline BOOL CImageList::SetOverlayImage(int nImage, int nOverlay)
{
	return ImageList_SetOverlayImage(m_hImageList,nImage,nOverlay);
}

inline BOOL CImageList::BeginDrag(int nImage, POINT ptHotSpot)
{
	return ImageList_BeginDrag(m_hImageList,nImage,ptHotSpot.x,ptHotSpot.y);
}

inline void PASCAL CImageList::EndDrag()
{
	ImageList_EndDrag();
}

inline BOOL PASCAL CImageList::DragMove(POINT pt)
{
	return ImageList_DragMove(pt.x,pt.y);
}

inline BOOL CImageList::SetDragCursorImage(int nDrag, POINT ptHotSpot)
{
	return ImageList_SetDragCursorImage(m_hImageList,nDrag,ptHotSpot.x,ptHotSpot.y);
}

inline BOOL PASCAL CImageList::DragShowNolock(BOOL bShow)
{
	return ImageList_DragShowNolock(bShow);
}

inline HIMAGELIST PASCAL CImageList::GetDragImage(LPPOINT lpPoint, LPPOINT lpPointHotSpot)
{
	return ImageList_GetDragImage(lpPoint,lpPointHotSpot);
}

inline BOOL PASCAL CImageList::DragEnter(HWND hWndLock, POINT point)
{
	return ImageList_DragEnter(hWndLock,point.x,point.y);
}

inline BOOL PASCAL CImageList::DragLeave(HWND hWndLock)
{
	return ImageList_DragLeave(hWndLock);
}

inline BOOL CImageList::Copy(int iDst,HIMAGELIST himlSrc,int iSrc,UINT uFlags)
{
	return ImageList_Copy(m_hImageList,iDst,himlSrc,iSrc,uFlags);
}

inline BOOL CImageList::Write(LPSTREAM pstm) const
{
	return ImageList_Write(m_hImageList,pstm);
}

inline BOOL CImageList::SetFlags(UINT flags)
{
	//Not implemented anymore
	//return ImageList_SetFlags(m_hImageList,flags);
	SetHFCError(HFC_OBSOLETEFUNCTION);
	return FALSE;
}

#endif
///////////////////////////
// Class CStatusBarCtrl

inline CStatusBarCtrl::CStatusBarCtrl()
:	CCommonCtrl(NULL)
{
}

inline CStatusBarCtrl::CStatusBarCtrl(HWND hWnd)
:	CCommonCtrl(hWnd)
{
}

inline BOOL CStatusBarCtrl::Create(DWORD dwStyle,const RECT* rect,HWND hWndParent,UINT nID)
{
	return (m_hWnd=CreateWindowEx(0L,STATUSCLASSNAME,szEmpty,
      dwStyle,rect->left,rect->top,rect->right-rect->left,rect->bottom-rect->top,
      hWndParent,(HMENU)(LONG_PTR)nID,GetInstanceHandle(),NULL))!=NULL;
}

inline BOOL CStatusBarCtrl::SetText(LPCSTR lpszText,int nPane,int nType)
{
	return (BOOL)::SendMessage(m_hWnd,SB_SETTEXT,(WPARAM)nPane|nType,(LPARAM)lpszText); 
}

inline BOOL CStatusBarCtrl::SetParts(int nParts,int* pWidths)
{
	return (BOOL)::SendMessage(m_hWnd,SB_SETPARTS,(WPARAM)nParts,(LPARAM)pWidths);
}

inline int CStatusBarCtrl::GetParts(int nParts,int* pParts) const
{
	return (int)::SendMessage(m_hWnd,SB_GETPARTS,(WPARAM)nParts,(LPARAM)pParts); 
}

inline BOOL CStatusBarCtrl::GetBorders(int* pBorders) const
{
	return (BOOL)::SendMessage(m_hWnd,SB_GETBORDERS,0,(LPARAM)pBorders);
}

inline void CStatusBarCtrl::SetMinHeight(int nMin)
{
	::SendMessage(m_hWnd,SB_SETMINHEIGHT,(WPARAM)nMin,0);
}

inline BOOL CStatusBarCtrl::SetSimple(BOOL bSimple)
{
	return (BOOL)::SendMessage(m_hWnd,SB_SIMPLE,(WPARAM)bSimple,0);
}

inline BOOL CStatusBarCtrl::IsSimple() const
{
	return (BOOL)::SendMessage(m_hWnd,SB_ISSIMPLE,0,0); 
}
	
inline BOOL CStatusBarCtrl::GetRect(int nPane,LPRECT lpRect) const
{
	return (BOOL)::SendMessage(m_hWnd,SB_GETRECT,(WPARAM)nPane,(LPARAM)lpRect); 
}

inline BOOL CStatusBarCtrl::SetIcon(int nPane,HICON hIcon)
{
	return (BOOL)::SendMessage(m_hWnd,SB_SETICON,nPane,(LPARAM)hIcon);
}

inline HICON CStatusBarCtrl::GetIcon(int nPane) const
{
	return (HICON)::SendMessage(m_hWnd,SB_GETICON,nPane,0);
}

inline BOOL CStatusBarCtrl::SetTipText(int n,LPCSTR szText)
{
	return (BOOL)::SendMessage(m_hWnd,SB_SETTIPTEXT,n,(LPARAM)szText);
}

inline int CStatusBarCtrl::GetTipText(int n,LPSTR szText,int nSize) const
{
	return (int)::SendMessage(m_hWnd,SB_GETTIPTEXT,MAKEWPARAM(n,nSize),(LPARAM)szText);
}

inline BOOL CStatusBarCtrl::SetUnicodeFormat(int nFormat)
{
	return (BOOL)::SendMessage(m_hWnd,SB_SETUNICODEFORMAT,nFormat,0);
}

inline BOOL CStatusBarCtrl::GetUnicodeFormat() const
{
	return (BOOL)::SendMessage(m_hWnd,SB_GETUNICODEFORMAT,0,0);
}

#ifdef DEF_WCHAR
inline BOOL CStatusBarCtrl::SetText(LPCWSTR lpszText,int nPane,int nType)
{
	if (IsUnicodeSystem())
		return (BOOL)::SendMessageW(m_hWnd,SB_SETTEXTW,(WPARAM)nPane|nType,(LPARAM)lpszText); 
	else
		return (BOOL)::SendMessage(m_hWnd,SB_SETTEXTA,(WPARAM)nPane|nType,(LPARAM)(LPCSTR)W2A(lpszText)); 
}
inline BOOL CStatusBarCtrl::SetTipText(int n,LPCWSTR szText)
{
	if (IsUnicodeSystem())
		return (BOOL)::SendMessageW(m_hWnd,SB_SETTIPTEXTW,n,(LPARAM)szText);
	else
		return (BOOL)::SendMessage(m_hWnd,SB_SETTIPTEXTA,n,(LPARAM)(LPCSTR)W2A(szText));
}

#endif

///////////////////////////
// Class CToolTipCtrl

inline CToolTipCtrl::CToolTipCtrl()
:	CCommonCtrl(NULL)
{
}

inline CToolTipCtrl::CToolTipCtrl(HWND hWnd)
:	CCommonCtrl(hWnd)
{
}

inline BOOL CToolTipCtrl::Create(HWND hParentWnd,DWORD dwStyle)
{
	m_hWnd=CreateWindowEx(WS_EX_TOPMOST,TOOLTIPS_CLASS,szEmpty,dwStyle,
	  CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,hParentWnd,(HMENU)0,GetInstanceHandle(),NULL);
	if (m_hWnd==NULL)
		return FALSE;
	::SetWindowPos(m_hWnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	return TRUE;

}

inline void CToolTipCtrl::NewToolRect(LPTOOLINFO lpToolInfo)
{
	::SendMessage(m_hWnd,TTM_NEWTOOLRECT,0,(LPARAM)lpToolInfo);
}
	
inline BOOL CToolTipCtrl::AdjustRect(LPRECT lpRect,BOOL fLarger)
{
	return (BOOL)::SendMessage(m_hWnd,TTM_ADJUSTRECT,fLarger,LPARAM(lpRect));
}

inline int CToolTipCtrl::GetToolCount() const
{
	return (int)::SendMessage(m_hWnd,TTM_GETTOOLCOUNT,0,0);
}

inline void CToolTipCtrl::Activate(BOOL bActivate)
{
	::SendMessage(m_hWnd,TTM_ACTIVATE,0,0);
}



inline void CToolTipCtrl::RelayEvent(LPMSG lpMsg)
{
	::SendMessage(m_hWnd,TTM_RELAYEVENT,0,(LPARAM)lpMsg);
}

inline void CToolTipCtrl::SetDelayTime(UINT nDelay,Duration nDuration)
{
	::SendMessage(m_hWnd,TTM_SETDELAYTIME,(WPARAM)nDuration,(LPARAM)nDelay);
}

inline BOOL CToolTipCtrl::DestroyToolTipCtrl()
{
	return ::DestroyWindow(m_hWnd);
}

inline void CToolTipCtrl::DelTool(LPTOOLINFO lpToolInfo)
{
	::SendMessage(m_hWnd,TTM_DELTOOL,0,(LPARAM)lpToolInfo);
}


inline BOOL CToolTipCtrl::TrackActivate(BOOL bStart,LPTOOLINFO lpToolInfo)
{
	return (BOOL)::SendMessage(m_hWnd,TTM_TRACKACTIVATE,(WPARAM)bStart,(LPARAM)lpToolInfo);
}

inline BOOL CToolTipCtrl::TrackPosition(int xPos,int yPos)
{
	return (BOOL)::SendMessage(m_hWnd,TTM_TRACKPOSITION,0,MAKELPARAM(xPos,yPos));
}

inline void CToolTipCtrl::GetMargin(LPRECT lprc) const
{
	::SendMessage(m_hWnd,TTM_GETMARGIN,0,(LPARAM)lprc);
}

inline void CToolTipCtrl::SetMargin(LPCRECT lprc)
{
	::SendMessage(m_hWnd,TTM_GETMARGIN,0,(LPARAM)lprc);
}

inline int CToolTipCtrl::GetMaxTipWidth() const
{
	return (int)::SendMessage(m_hWnd,TTM_SETMAXTIPWIDTH,0,0);
}

inline int CToolTipCtrl::SetMaxTipWidth(int iWidth)
{
	return (int)::SendMessage(m_hWnd,TTM_SETMAXTIPWIDTH,0,iWidth);
}

inline BOOL CToolTipCtrl::EnumTools(UINT iTool,LPTOOLINFO lpToolInfo) const
{
	return (BOOL)::SendMessage(m_hWnd,TTM_ENUMTOOLS,iTool,(LPARAM)lpToolInfo);
}

inline void CToolTipCtrl::Pop()
{
	::SendMessage(m_hWnd,TTM_POP,0,0);
}

inline void CToolTipCtrl::SetToolInfo(LPTOOLINFO lpToolInfo)
{
	::SendMessage(m_hWnd,TTM_SETTOOLINFO,0,(LPARAM)lpToolInfo);
}

inline BOOL CToolTipCtrl::AddTool(LPTOOLINFO lpToolInfo)
{
	return (BOOL)::SendMessage(m_hWnd,TTM_ADDTOOL,0,(LPARAM)lpToolInfo);
}
	

#ifdef DEF_WCHAR
inline void CToolTipCtrl::NewToolRect(LPTOOLINFOW lpToolInfo)
{
	::SendMessage(m_hWnd,TTM_NEWTOOLRECTW,0,(LPARAM)lpToolInfo);
}

inline BOOL CToolTipCtrl::AddTool(LPTOOLINFOW lpToolInfo)
{
	return (BOOL)::SendMessage(m_hWnd,TTM_ADDTOOLW,0,(LPARAM)lpToolInfo);
}

inline void CToolTipCtrl::SetToolInfo(LPTOOLINFOW lpToolInfo)
{
	::SendMessage(m_hWnd,TTM_SETTOOLINFOW,0,(LPARAM)lpToolInfo);
}

inline BOOL CToolTipCtrl::GetCurrentTool(LPTOOLINFOW lpToolInfo) const
{
	return (BOOL)::SendMessage(m_hWnd,TTM_GETCURRENTTOOLW,0,LPARAM(lpToolInfo));
}

inline void CToolTipCtrl::DelTool(LPTOOLINFOW lpToolInfo)
{
	::SendMessage(m_hWnd,TTM_DELTOOLW,0,(LPARAM)lpToolInfo);
}

inline BOOL CToolTipCtrl::EnumTools(UINT iTool,LPTOOLINFOW lpToolInfo) const
{
	return (BOOL)::SendMessage(m_hWnd,TTM_ENUMTOOLSW,iTool,(LPARAM)lpToolInfo);
}
#endif


///////////////////////////
// Class CReBarCtrl


inline CRebarCtrl::CRebarCtrl()
:	CCommonCtrl(NULL)
{
}

inline CRebarCtrl::CRebarCtrl(HWND hWnd)
:	CCommonCtrl(hWnd)
{
}

inline BOOL CRebarCtrl::Create(DWORD dwStyle,const RECT* rect,HWND hWndParent,UINT nID)
{
	return (m_hWnd=CreateWindowEx(0L,REBARCLASSNAME,szEmpty,
      dwStyle,rect->left,rect->top,rect->right-rect->left,rect->bottom-rect->top,
      hWndParent,(HMENU)(LONG_PTR)nID,GetInstanceHandle(),NULL))!=NULL;
}

inline BOOL CRebarCtrl::CreateEx(DWORD dwStyle,const RECT* rect,HWND hWndParent,UINT nID,DWORD dwExStyle)
{
	return (m_hWnd=CreateWindowEx(dwExStyle,REBARCLASSNAME,szEmpty,
      dwStyle,rect->left,rect->top,rect->right-rect->left,rect->bottom-rect->top,
      hWndParent,(HMENU)(LONG_PTR)nID,GetInstanceHandle(),NULL))!=NULL;
}

inline int CRebarCtrl::InsertBand(int nBand,const REBARBANDINFO* rbi)
{
	return (int)::SendMessage(m_hWnd,RB_INSERTBAND,(WPARAM)nBand,(LPARAM)rbi);
}

inline int CRebarCtrl::DeleteBand(int nBand)
{
	return (int)::SendMessage(m_hWnd,RB_DELETEBAND,(WPARAM)nBand,0);
}

inline int CRebarCtrl::GetBandInfo(int nBand,REBARBANDINFO* rbi) const
{
	return (int)::SendMessage(m_hWnd,RB_GETBANDINFO,(WPARAM)nBand,(LPARAM)rbi);
}

inline int CRebarCtrl::SetBandInfo(int nBand,const REBARBANDINFO* rbi)
{
	return (int)::SendMessage(m_hWnd,RB_SETBANDINFO,(WPARAM)nBand,(LPARAM)rbi);
}
	
inline int CRebarCtrl::GetBarInfo(REBARINFO* pri) const
{
	return (int)::SendMessage(m_hWnd,RB_GETBARINFO,(WPARAM)0,(LPARAM)pri);
}

inline int CRebarCtrl::SetBarInfo(const REBARINFO* pri)
{
	return (int)::SendMessage(m_hWnd,RB_SETBARINFO,(WPARAM)0,(LPARAM)pri);
}

inline int CRebarCtrl::GetBandCount() const
{
	return (int)::SendMessage(m_hWnd,RB_GETBANDCOUNT,0,0);
}

inline int CRebarCtrl::GetRowCount() const
{
	return (int)::SendMessage(m_hWnd,RB_GETROWCOUNT,0,0);
}

inline int CRebarCtrl::GetRowHeight(int nRow) const
{
	return (int)::SendMessage(m_hWnd,RB_GETROWHEIGHT,nRow,0);
}

inline void CRebarCtrl::GetBandBorders(int nBand,RECT& rc) const
{
	::SendMessage(m_hWnd,RB_GETBARHEIGHT,0,LPARAM(&rc));
}

/*inline void CRebarCtrl::GetBandMargins(PMARGINS margins) const
{
	return (int)::SendMessage(m_hWnd,RB_GETBANDMARGINS,0,LPARAM(margins));
}*/

inline int CRebarCtrl::GetBarHeight() const
{
	return (int)::SendMessage(m_hWnd,RB_GETBARHEIGHT,0,0);
}

inline void CRebarCtrl::BeginDrag(int nBand,WORD wCorX,WORD wCorY)
{
	::SendMessage(m_hWnd,RB_GETBKCOLOR,nBand,MAKELPARAM(wCorX,wCorY));
}
	
inline void CRebarCtrl::DrawMove(WORD wCorX,WORD wCorY)
{
	::SendMessage(m_hWnd,RB_DRAGMOVE,0,MAKELPARAM(wCorX,wCorY));
}

inline void CRebarCtrl::EndDrag()
{
	::SendMessage(m_hWnd,RB_ENDDRAG,0,0);
}


inline COLORREF CRebarCtrl::GetBackColor() const
{
	return (COLORREF)::SendMessage(m_hWnd,RB_GETBKCOLOR,0,0);
}

inline COLORREF CRebarCtrl::SetBackColor(COLORREF col)
{
	return (COLORREF)::SendMessage(m_hWnd,RB_SETBKCOLOR,0,LPARAM(col));
}

inline void CRebarCtrl::SetColorScheme(LPCOLORSCHEME lpcs)
{
	::SendMessage(m_hWnd,RB_SETCOLORSCHEME,0,LPARAM(lpcs));
}

inline BOOL  CRebarCtrl::GetColorScheme(LPCOLORSCHEME lpcs) const
{
	return (BOOL)::SendMessage(m_hWnd,RB_GETCOLORSCHEME,0,LPARAM(lpcs));
}

inline IDropTarget* CRebarCtrl::GetDropTarget() const
{
	IDropTarget* pDropTarget;
	::SendMessage(m_hWnd,RB_GETDROPTARGET,0,(LPARAM)&pDropTarget);
	return pDropTarget;
}

inline int CRebarCtrl::IdToIndex(UINT nBandID) const
{
	return (int)::SendMessage(m_hWnd,RB_IDTOINDEX,nBandID,0);
}

inline void CRebarCtrl::MaximizeBand(int nBand,BOOL bIdeal)
{
	::SendMessage(m_hWnd,RB_MAXIMIZEBAND,nBand,bIdeal);
}

inline void CRebarCtrl::MinimizeBand(int nBand)
{
	::SendMessage(m_hWnd,RB_MINIMIZEBAND,nBand,0);
}

inline void CRebarCtrl::MoveBand(int nFrom,int nTo)
{
	::SendMessage(m_hWnd,RB_MOVEBAND,nFrom,nTo);
}

inline BOOL CRebarCtrl::ShowBand(int nBand,BOOL bShow)
{
	return (BOOL)::SendMessage(m_hWnd,RB_SHOWBAND,nBand,bShow);
}

inline BOOL CRebarCtrl::SizeToRect(RECT& rc)
{
	return (BOOL)::SendMessage(m_hWnd,RB_SIZETORECT,0,LPARAM(&rc));
}

inline BOOL CRebarCtrl::GetRect(int nBand,RECT& rc)
{
	return (BOOL)::SendMessage(m_hWnd,RB_GETRECT,0,LPARAM(& rc));
}

inline HWND CRebarCtrl::GetTooltips() const
{
	return (HWND)::SendMessage(m_hWnd,RB_GETTOOLTIPS,0,0);
}

inline void CRebarCtrl::SetTooltips(HWND hwnd)
{
	::SendMessage(m_hWnd,RB_SETTOOLTIPS,0,LPARAM(hwnd));
}

inline void CRebarCtrl::SetWindowTheme(LPWSTR pwStr)
{
	::SendMessage(m_hWnd,RB_SETWINDOWTHEME,0,LPARAM(pwStr));
}


///////////////////////////
// Class CToolBarCtrl

inline CToolBarCtrl::CToolBarCtrl()
:	CCommonCtrl(NULL)
{
}

inline CToolBarCtrl::CToolBarCtrl(HWND hWnd)
:	CCommonCtrl(hWnd)
{
}

inline BOOL CToolBarCtrl::IsButtonEnabled(int nID) const
{
	return (BOOL)::SendMessage(m_hWnd,TB_ISBUTTONENABLED,(WPARAM)nID,0);
}

inline BOOL CToolBarCtrl::IsButtonChecked(int nID) const
{
	return (BOOL)::SendMessage(m_hWnd,TB_ISBUTTONCHECKED,(WPARAM)nID,0);
}

inline BOOL CToolBarCtrl::IsButtonPressed(int nID) const
{
	return (BOOL)::SendMessage(m_hWnd,TB_ISBUTTONPRESSED,(WPARAM)nID,0);
}

inline BOOL CToolBarCtrl::IsButtonHidden(int nID) const
{
	return (BOOL)::SendMessage(m_hWnd,TB_ISBUTTONHIDDEN,(WPARAM)nID,0);
}

inline BOOL CToolBarCtrl::IsButtonIndeterminate(int nID) const
{
	return (BOOL)::SendMessage(m_hWnd,TB_ISBUTTONINDETERMINATE,(WPARAM)nID,0);
}

inline BOOL CToolBarCtrl::SetState(int nID,UINT nState)
{
	return (BOOL)::SendMessage(m_hWnd,TB_SETSTATE,(WPARAM)nID,(LPARAM)MAKELONG(nState,0));
}

inline int CToolBarCtrl::GetState(int nID) const
{
	return (int)::SendMessage(m_hWnd,TB_GETSTATE,(WPARAM)nID,0);
}

inline BOOL CToolBarCtrl::GetButton(int nIndex,LPTBBUTTON lpButton) const
{
	return (BOOL)::SendMessage(m_hWnd,TB_GETBUTTON,(WPARAM)nIndex,(LPARAM)lpButton);
}

inline int CToolBarCtrl::GetButtonCount() const
{
	return (int)::SendMessage(m_hWnd,TB_BUTTONCOUNT,0,0);
}

inline BOOL CToolBarCtrl::GetRect(int nID,LPRECT lpRect) const
{
	return (BOOL)::SendMessage(m_hWnd,TB_GETRECT,(WPARAM)nID,(LPARAM)lpRect);
}
	
inline BOOL CToolBarCtrl::GetItemRect(int nIndex,LPRECT lpRect) const
{
	return (BOOL)::SendMessage(m_hWnd,TB_GETITEMRECT,(WPARAM)nIndex,(LPARAM)lpRect);
}

inline void CToolBarCtrl::SetButtonStructSize(int nSize)
{
	::SendMessage(m_hWnd,TB_BUTTONSTRUCTSIZE,(WPARAM)nSize,0);
}

inline BOOL CToolBarCtrl::SetButtonSize(LPSIZE size)
{
	return (BOOL)::SendMessage(m_hWnd,TB_SETBUTTONSIZE,0,(LPARAM)MAKELONG(size->cx,size->cy));
}

inline BOOL CToolBarCtrl::SetBitmapSize(LPSIZE size)
{
	return (BOOL)::SendMessage(m_hWnd,TB_SETBITMAPSIZE,0,(LPARAM)MAKELONG(size->cx,size->cy));
}

inline HWND CToolBarCtrl::GetToolTips() const
{
	return (HWND) ::SendMessage(m_hWnd,TB_GETTOOLTIPS,0,0);
}

inline void CToolBarCtrl::SetToolTips(HWND hTip)
{
	::SendMessage(m_hWnd,TB_SETTOOLTIPS,(WPARAM)hTip,0);
}

inline void CToolBarCtrl::SetRows(int nRows,BOOL bLarger,LPRECT lpRect)
{
	::SendMessage(m_hWnd,TB_SETROWS,(WPARAM)MAKEWPARAM(nRows,bLarger),(LPARAM)lpRect);
}

inline int CToolBarCtrl::GetRows() const
{
	return (int)::SendMessage(m_hWnd,TB_GETROWS,0,0);
}

inline BOOL CToolBarCtrl::SetCmdID(int nIndex,UINT nID)
{
	return (BOOL)::SendMessage(m_hWnd,TB_SETCMDID,(WPARAM)nIndex,(LPARAM)nID);
}

inline UINT CToolBarCtrl::GetBitmapFlags() const
{
	return (UINT)::SendMessage(m_hWnd,TB_GETBITMAPFLAGS,0,0);
}

inline HWND CToolBarCtrl::SetParent(HWND hParentNew)
{
	return (HWND)::SendMessage(m_hWnd,TB_SETPARENT,(WPARAM)hParentNew,0);
}

inline BOOL CToolBarCtrl::EnableButton(int nID,BOOL bEnable)
{
	return (BOOL)::SendMessage(m_hWnd,TB_ENABLEBUTTON,(WPARAM)nID,(LPARAM)MAKELONG(bEnable,0));
}

inline BOOL CToolBarCtrl::CheckButton(int nID,BOOL bCheck)
{
	return (BOOL)::SendMessage(m_hWnd,TB_CHECKBUTTON,(WPARAM)nID,(LPARAM)MAKELONG(bCheck,0));
}

inline BOOL CToolBarCtrl::PressButton(int nID,BOOL bPress)
{
	return (BOOL)::SendMessage(m_hWnd,TB_PRESSBUTTON,(WPARAM)nID,(LPARAM)MAKELONG(bPress,0));
}

inline BOOL CToolBarCtrl::HideButton(int nID,BOOL bHide)
{
	return (BOOL)::SendMessage(m_hWnd,TB_HIDEBUTTON,(WPARAM)nID,(LPARAM)MAKELONG(bHide,0));
}

inline BOOL CToolBarCtrl::Indeterminate(int nID,BOOL bIndeterminate)
{
	return (BOOL)::SendMessage(m_hWnd,TB_INDETERMINATE,(WPARAM)nID,(LPARAM)MAKELONG(bIndeterminate,0));
}

inline BOOL CToolBarCtrl::AddButtons(int nNumButtons,LPTBBUTTON lpButtons)
{
	return (BOOL)::SendMessage(m_hWnd,TB_ADDBUTTONS,(WPARAM)nNumButtons,(LPARAM)lpButtons);
}

inline BOOL CToolBarCtrl::InsertButton(int nIndex,LPTBBUTTON lpButton)
{
	return (BOOL)::SendMessage(m_hWnd,TB_INSERTBUTTON,(WPARAM)nIndex,(LPARAM)lpButton);
}

inline BOOL CToolBarCtrl::DeleteButton(int nIndex)
{
	return (BOOL)::SendMessage(m_hWnd,TB_DELETEBUTTON,(WPARAM)nIndex,0);
}

inline UINT CToolBarCtrl::CommandToIndex(UINT nID) const
{
	return (UINT)::SendMessage(m_hWnd,TB_COMMANDTOINDEX,(WPARAM)nID,0);
}

inline void CToolBarCtrl::Customize()
{
	::SendMessage(m_hWnd,TB_CUSTOMIZE,0,0);
}

#ifdef DEF_RESOURCES
inline int CToolBarCtrl::AddString(UINT nStringID)
{
	return (int)::SendMessage(m_hWnd,TB_ADDSTRING,(WPARAM)GetLanguageSpecificResourceHandle(),(LPARAM)nStringID);
}
#endif

inline int CToolBarCtrl::AddStrings(LPCTSTR lpszStrings)
{
	return (int)::SendMessage(m_hWnd,TB_ADDSTRING,(WPARAM)NULL,(LPARAM)lpszStrings);
}

inline void CToolBarCtrl::AutoSize()
{
	::SendMessage(m_hWnd,TB_AUTOSIZE,0,0);
}

inline BOOL CToolBarCtrl::SetIndent(int nIndent)
{
	return (BOOL)::SendMessage(m_hWnd,TB_SETINDENT,nIndent,0);
}

inline HIMAGELIST CToolBarCtrl::GetImageList() const
{
	return (HIMAGELIST)::SendMessage(m_hWnd,TB_GETIMAGELIST,0,0);
}

inline HIMAGELIST CToolBarCtrl::SetImageList(HIMAGELIST hImageList)
{
	return (HIMAGELIST)::SendMessage(m_hWnd,TB_SETIMAGELIST,0,(LPARAM)hImageList);
}

inline HIMAGELIST CToolBarCtrl::GetHotImageList() const
{
	return (HIMAGELIST)::SendMessage(m_hWnd,TB_GETHOTIMAGELIST,0,0);
}

inline HIMAGELIST CToolBarCtrl::SetHotImageList(HIMAGELIST hImageList)
{
	return (HIMAGELIST)::SendMessage(m_hWnd,TB_SETHOTIMAGELIST,0,(LPARAM)hImageList);
}

inline HIMAGELIST CToolBarCtrl::GetDisabledImageList() const
{
	return (HIMAGELIST)::SendMessage(m_hWnd,TB_GETDISABLEDIMAGELIST,0,0);
}

inline HIMAGELIST CToolBarCtrl::SetDisabledImageList(HIMAGELIST hImageList)
{
	return (HIMAGELIST)::SendMessage(m_hWnd,TB_SETDISABLEDIMAGELIST,0,(LPARAM)hImageList);
}

inline BOOL CToolBarCtrl::GetMaxSize(SIZE& rSize) const
{
	return (BOOL)::SendMessage(m_hWnd,TB_GETMAXSIZE,0,(LPARAM)&rSize);
}

inline HRESULT CToolBarCtrl::GetObject(REFIID rID,void** ppvObject) const
{
	return (HRESULT)::SendMessage(m_hWnd,TB_GETOBJECT,(WPARAM)&rID,(LPARAM)ppvObject);
}

inline DWORD CToolBarCtrl::GetPadding() const
{
	return (DWORD)::SendMessage(m_hWnd,TB_GETPADDING,0,0);
}

inline BOOL CToolBarCtrl::GetPadding(WORD& cx,WORD& cy) const
{
	DWORD nPadding=(DWORD)::SendMessage(m_hWnd,TB_GETPADDING,0,0);
	cx=LOWORD(nPadding);
	cy=HIWORD(nPadding);
	return TRUE;
}

inline void CToolBarCtrl::SetPadding(WORD cx,WORD cy)
{
	::SendMessage(m_hWnd,TB_SETPADDING,0,MAKELPARAM(cx,cy));
}

///////////////////////////
// Class CProgressCtrl

inline CProgressCtrl::CProgressCtrl() 
:	CCommonCtrl(NULL)
{
}

inline CProgressCtrl::CProgressCtrl(HWND hWnd) 
:	CCommonCtrl(hWnd)
{
}

inline BOOL CProgressCtrl::Create(DWORD dwStyle,const RECT* rect,HWND hWndParent,UINT nID)
{
	return (m_hWnd=CreateWindowEx(0L,PROGRESS_CLASS,szEmpty,
      dwStyle,rect->left,rect->top,rect->right-rect->left,rect->bottom-rect->top,
      hWndParent,(HMENU)(LONG_PTR)nID,GetInstanceHandle(),NULL))!=NULL;
}

inline void CProgressCtrl::SetRange(int nLower, int nUpper)
{
	SendMessage(PBM_SETRANGE,0,MAKELONG((WORD)nLower,(WORD)nUpper));
}

inline int CProgressCtrl::SetPos(int nPos)
{
	return (int)SendMessage(PBM_SETPOS,nPos,0);
}

inline int CProgressCtrl::OffsetPos(int nPos)
{
	return (int)SendMessage(PBM_DELTAPOS,nPos,0);	
}

inline int CProgressCtrl::SetStep(int nStep)
{
	return (int)SendMessage(PBM_SETSTEP,nStep,0);
}

inline int CProgressCtrl::StepIt()
{
	return (int)SendMessage(PBM_STEPIT,0,0);
}

inline void CProgressCtrl::SetBarColor(COLORREF bColor)
{
	SendMessage(PBM_SETBARCOLOR,0,LPARAM(bColor));
}

///////////////////////////
// Class CSpinButtonCtrl

inline CSpinButtonCtrl::CSpinButtonCtrl()
:	CCommonCtrl(NULL)
{
}

inline CSpinButtonCtrl::CSpinButtonCtrl(HWND hWnd)
:	CCommonCtrl(hWnd)
{
}

inline BOOL CSpinButtonCtrl::Create(DWORD dwStyle,const RECT* rect,HWND hParentWnd,UINT nID)
{
	return (m_hWnd=CreateWindowEx(0L,UPDOWN_CLASS,szEmpty,dwStyle,rect->left,rect->top,rect->right-rect->left,rect->bottom-rect->top,
      hParentWnd,(HMENU)(LONG_PTR)nID,GetInstanceHandle(),NULL))!=NULL;
}	

inline BOOL CSpinButtonCtrl::SetAccel(int nAccel, UDACCEL* pAccel)
{
	return (BOOL)::SendMessage(m_hWnd,UDM_SETACCEL,(WPARAM)nAccel,(LPARAM)pAccel); 
}

inline UINT CSpinButtonCtrl::GetAccel(int nAccel, UDACCEL* pAccel) const
{
	return (UINT)::SendMessage(m_hWnd,UDM_GETACCEL,(WPARAM)nAccel,(LPARAM)pAccel); 
}

inline int CSpinButtonCtrl::SetBase(int nBase)
{
	return (int)::SendMessage(m_hWnd,UDM_SETBASE,(WPARAM)nBase,0);
}

inline UINT CSpinButtonCtrl::GetBase() const
{
	return (UINT)::SendMessage(m_hWnd,UDM_GETBASE,0,0);
}

inline HWND CSpinButtonCtrl::SetBuddy(HWND hWndBuddy)
{
	return (HWND)::SendMessage(m_hWnd,UDM_SETBUDDY,(WPARAM)hWndBuddy,0);
}

inline HWND CSpinButtonCtrl::GetBuddy() const
{
	return (HWND)::SendMessage(m_hWnd,UDM_GETBUDDY,0,0);
}

inline int CSpinButtonCtrl::SetPos(int nPos)
{
	return(int) ::SendMessage(m_hWnd,UDM_SETPOS,0,(LPARAM)nPos);
}

inline int CSpinButtonCtrl::GetPos() const
{
	return LOWORD(::SendMessage(m_hWnd,UDM_GETPOS,0,0));
}

inline void CSpinButtonCtrl::SetRange(int nLower,int nUpper)
{
	::SendMessage(m_hWnd,UDM_SETRANGE,0,(LPARAM)MAKELONG((short)nUpper,(short)nLower));
}

inline DWORD CSpinButtonCtrl::GetRange() const
{
	return (DWORD)::SendMessage(m_hWnd,UDM_GETRANGE,0,0);
}

///////////////////////////
// Class CTabCtrl

inline CTabCtrl::CTabCtrl()
:	CCommonCtrl(NULL)
{
}

inline CTabCtrl::CTabCtrl(HWND hWnd)
:	CCommonCtrl(hWnd)
{
}

inline HIMAGELIST CTabCtrl::GetImageList() const
{
	return (HIMAGELIST)::SendMessage(m_hWnd,TCM_GETIMAGELIST,0,0);
}

inline HIMAGELIST CTabCtrl::SetImageList(HIMAGELIST hImageList)
{
	return (HIMAGELIST)::SendMessage(m_hWnd,TCM_SETIMAGELIST,0,(LPARAM)hImageList);
}

inline int CTabCtrl::GetItemCount() const
{
	return (int)::SendMessage(m_hWnd,TCM_GETITEMCOUNT,0,0);
}

inline BOOL CTabCtrl::GetItem(int nItem,TC_ITEM* pTabCtrlItem) const
{
	return (BOOL)::SendMessage(m_hWnd,TCM_GETITEM,(WPARAM)nItem,(LPARAM)pTabCtrlItem);
}

inline BOOL CTabCtrl::SetItem(int nItem,TC_ITEM* pTabCtrlItem)
{
	return (BOOL)::SendMessage(m_hWnd,TCM_SETITEM,(WPARAM)nItem,(LPARAM)pTabCtrlItem);
}


inline BOOL CTabCtrl::GetItemRect(int nItem,LPRECT lpRect) const
{
	return (BOOL)::SendMessage(m_hWnd,TCM_GETITEMRECT,(WPARAM)nItem,(LPARAM)lpRect);
}


inline int CTabCtrl::GetCurSel() const
{
	return (int)::SendMessage(m_hWnd,TCM_GETCURSEL,0,0);
}

inline int CTabCtrl::SetCurSel(int nItem)
{
	return(int) ::SendMessage(m_hWnd,TCM_SETCURSEL,(WPARAM)nItem,0);
}

inline void CTabCtrl::SetPadding(CSize size)
{
	::SendMessage(m_hWnd,TCM_SETPADDING,0,MAKELPARAM(size.cx,size.cy));
}

inline int CTabCtrl::GetRowCount() const
{
	return (int)::SendMessage(m_hWnd,TCM_GETROWCOUNT,0,0);
}

inline HWND CTabCtrl::GetTooltips() const
{
	return (HWND)::SendMessage(m_hWnd,TCM_GETTOOLTIPS,0,0);
}

inline void CTabCtrl::SetTooltips(HWND hWndTip)
{
	::SendMessage(m_hWnd,TCM_SETTOOLTIPS,(WPARAM)hWndTip,0);
}

inline int CTabCtrl::GetCurFocus() const
{
	return (int)::SendMessage(m_hWnd,TCM_GETCURFOCUS,0,0);
}

inline BOOL CTabCtrl::InsertItem(int nItem,TC_ITEM* pTabCtrlItem)
{
	return (BOOL)::SendMessage(m_hWnd,TCM_INSERTITEM,(WPARAM)nItem,(LPARAM)pTabCtrlItem);
}

inline BOOL CTabCtrl::DeleteItem(int nItem)
{
	return (BOOL)::SendMessage(m_hWnd,TCM_DELETEITEM,(WPARAM)nItem,0);
}

inline BOOL CTabCtrl::DeleteAllItems()
{
	return (BOOL)::SendMessage(m_hWnd,TCM_DELETEALLITEMS,0,0);
}

inline void CTabCtrl::AdjustRect(BOOL bLarger,LPRECT lpRect)
{
	::SendMessage(m_hWnd,TCM_ADJUSTRECT,(WPARAM)bLarger,(LPARAM)lpRect);
}

inline void CTabCtrl::RemoveImage(int nImage)
{
	::SendMessage(m_hWnd,TCM_REMOVEIMAGE,(WPARAM)nImage,0);
}

inline int CTabCtrl::HitTest(TC_HITTESTINFO* pHitTestInfo) const
{
	return (int)::SendMessage(m_hWnd,TCM_HITTEST,0,(LPARAM)pHitTestInfo);
}

///////////////////////////
// Class CHeaderCtrl

inline CHeaderCtrl::CHeaderCtrl()
:	CCommonCtrl(NULL)
{
}

inline CHeaderCtrl::CHeaderCtrl(HWND hWnd)
:	CCommonCtrl(hWnd)
{
}

inline BOOL CHeaderCtrl::Create(DWORD dwStyle, const RECT* rect, HWND hParentWnd, UINT nID)
{
	return (m_hWnd=CreateWindowEx(0L,WC_HEADER,szEmpty,dwStyle,
		rect->left,rect->top,rect->right-rect->left,rect->bottom-rect->top,
		hParentWnd,(HMENU)(LONG_PTR)nID,GetInstanceHandle(),NULL))!=NULL;
}

inline int CHeaderCtrl::GetItemCount() const
{
	return (int)::SendMessage(m_hWnd,HDM_GETITEMCOUNT,0,0);
}

inline BOOL CHeaderCtrl::GetItem(int nPos, HD_ITEM* pHeaderItem) const
{
	return (BOOL)::SendMessage(m_hWnd,HDM_GETITEM,nPos,(LPARAM)pHeaderItem);
}

inline BOOL CHeaderCtrl::SetItem(int nPos, HD_ITEM* pHeaderItem)
{
	return (BOOL)::SendMessage(m_hWnd,HDM_SETITEM,nPos,(LPARAM)pHeaderItem);
}

inline int CHeaderCtrl::InsertItem(int nPos, HD_ITEM* phdi)
{
	return (int)::SendMessage(m_hWnd,HDM_INSERTITEM,nPos,(LPARAM)phdi);
}

inline BOOL CHeaderCtrl::DeleteItem(int nPos)
{
	return (BOOL)::SendMessage(m_hWnd,HDM_DELETEITEM,nPos,0);
}

inline BOOL CHeaderCtrl::Layout(HD_LAYOUT* pHeaderLayout)
{
	return (BOOL)::SendMessage(m_hWnd,HDM_LAYOUT,0,(LPARAM)pHeaderLayout);
}

inline BOOL CHeaderCtrl::GetItemRect(int iItem,LPRECT lprc) const
{
	return (BOOL)::SendMessage(m_hWnd,HDM_GETITEMRECT,iItem,(LPARAM)lprc);
}

inline HIMAGELIST CHeaderCtrl::SetImageList(HIMAGELIST hIml)
{
	return (HIMAGELIST)::SendMessage(m_hWnd,HDM_SETIMAGELIST,0,(LPARAM)hIml);
}

inline HIMAGELIST CHeaderCtrl::GetImageList() const
{
	return (HIMAGELIST)::SendMessage(m_hWnd,HDM_GETIMAGELIST,0,0);
}

inline int CHeaderCtrl::OrderToIndex(int iItem)
{
	return (int)::SendMessage(m_hWnd,HDM_ORDERTOINDEX,iItem,0);
}

inline HIMAGELIST CHeaderCtrl::CreateDragImage(int iItem)
{
	return (HIMAGELIST)::SendMessage(m_hWnd,HDM_CREATEDRAGIMAGE,iItem,0);
}

inline BOOL CHeaderCtrl::GetOrderArray(int iCount,int* lpi) const
{
	return (BOOL)::SendMessage(m_hWnd,HDM_GETORDERARRAY,iCount,(LPARAM)lpi);
}

inline BOOL CHeaderCtrl::SetOrderArray(int iCount,int* lpi)
{
	return (BOOL)::SendMessage(m_hWnd,HDM_SETORDERARRAY,iCount,(LPARAM)lpi);
}

inline int CHeaderCtrl::SetHotDivider(int fPos,DWORD dw)
{
	return (int)::SendMessage(m_hWnd,HDM_SETHOTDIVIDER,fPos,dw);
}

///////////////////////////
// Class CListCtrl

inline CListCtrl::CListCtrl()
:	CCommonCtrl(NULL)
{
}

inline CListCtrl::CListCtrl(HWND hWnd)
:	CCommonCtrl(hWnd)
{
}

inline BOOL CListCtrl::Create(DWORD dwStyle, const RECT* rect, HWND hParentWnd, UINT nID)
{
	return (m_hWnd=CreateWindowEx(0L,WC_LISTVIEW,szEmpty,dwStyle,
		rect->left,rect->top,rect->right-rect->left,rect->bottom-rect->top,
		hParentWnd,(HMENU)(ULONG_PTR)nID,GetInstanceHandle(),NULL))!=NULL;
}

inline COLORREF CListCtrl::GetBkColor() const
{
	return (COLORREF)::SendMessage(m_hWnd,LVM_GETBKCOLOR,0,0);
}

inline BOOL CListCtrl::SetBkColor(COLORREF cr)
{
	return (BOOL)::SendMessage(m_hWnd,LVM_SETBKCOLOR,0,(LPARAM)cr);
}

inline HIMAGELIST CListCtrl::GetImageList(int nImageList) const
{
	return (HIMAGELIST)::SendMessage(m_hWnd,LVM_GETIMAGELIST,nImageList,0);
}

inline HIMAGELIST CListCtrl::SetImageList(HIMAGELIST hImageList, int nImageListType)
{
	return (HIMAGELIST)::SendMessage(m_hWnd,LVM_SETIMAGELIST,nImageListType,(LPARAM)hImageList);
}

inline int CListCtrl::GetItemCount() const
{
	return (int)::SendMessage(m_hWnd,LVM_GETITEMCOUNT,0,0);
}

inline BOOL CListCtrl::GetItem(LV_ITEM* pItem) const
{
	return (BOOL)::SendMessage(m_hWnd,LVM_GETITEM,0,(LPARAM)pItem);
}




inline BOOL CListCtrl::SetItem(const LV_ITEM* pItem)
{
	return (BOOL)::SendMessage(m_hWnd,LVM_SETITEM,0,(LPARAM)pItem);
}

inline UINT CListCtrl::GetCallbackMask() const
{
	return (UINT)::SendMessage(m_hWnd,LVM_GETCALLBACKMASK,0,0);
}

inline BOOL CListCtrl::SetCallbackMask(UINT nMask)
{
	return (BOOL)::SendMessage(m_hWnd,LVM_SETCALLBACKMASK,(WPARAM)nMask,0);
}

inline int CListCtrl::GetNextItem(int nItem, int nFlags) const
{
	return (int)::SendMessage(m_hWnd,LVM_GETNEXTITEM,(WPARAM)nItem,MAKELPARAM(nFlags,0));
}

inline BOOL CListCtrl::SetItemPosition(int nItem, POINT pt)
{
	return (BOOL)::SendMessage(m_hWnd,LVM_SETITEMPOSITION,(LPARAM)nItem,MAKELPARAM(pt.x,pt.y));
}

inline BOOL CListCtrl::GetItemPosition(int nItem, LPPOINT lpPoint) const
{
	return (BOOL)::SendMessage(m_hWnd,LVM_GETITEMPOSITION,(WPARAM)nItem,(LPARAM)lpPoint);
}

inline int CListCtrl::GetStringWidth(LPCSTR lpsz) const
{
	return (int)::SendMessage(m_hWnd,LVM_GETSTRINGWIDTH,0,(LPARAM)lpsz);
}

inline HWND CListCtrl::GetEditControl() const
{
	return (HWND)::SendMessage(m_hWnd,LVM_GETEDITCONTROL,0,0);
}

inline BOOL CListCtrl::GetColumn(int nCol, LV_COLUMN* pColumn) const
{
	return (BOOL)::SendMessage(m_hWnd,LVM_GETCOLUMN,(WPARAM)nCol,(LPARAM)pColumn);
}

inline BOOL CListCtrl::SetColumn(int nCol, const LV_COLUMN* pColumn)
{
	return (BOOL)::SendMessage(m_hWnd,LVM_SETCOLUMN,(WPARAM)nCol,(LPARAM)pColumn);
}

inline int CListCtrl::GetColumnWidth(int nCol) const
{
	return (int)::SendMessage(m_hWnd,LVM_GETCOLUMNWIDTH,(WPARAM)nCol,0);
}

inline BOOL CListCtrl::SetColumnWidth(int nCol, int cx)
{
	return (BOOL)::SendMessage(m_hWnd,LVM_SETCOLUMNWIDTH,(WPARAM)nCol,MAKELPARAM(cx,0));
}

inline BOOL CListCtrl::GetViewRect(LPRECT lpRect) const
{
	return (BOOL)::SendMessage(m_hWnd,LVM_GETVIEWRECT,0,(LPARAM)lpRect);
}

inline COLORREF CListCtrl::GetTextColor() const
{
	return (COLORREF)::SendMessage(m_hWnd,LVM_GETTEXTCOLOR,0,0);
}

inline BOOL CListCtrl::SetTextColor(COLORREF cr)
{
	return (BOOL)::SendMessage(m_hWnd,LVM_SETTEXTCOLOR,0,(LPARAM)cr);
}

inline COLORREF CListCtrl::GetTextBkColor() const
{
	return (COLORREF)::SendMessage(m_hWnd,LVM_GETTEXTBKCOLOR,0,0);
}

inline BOOL CListCtrl::SetTextBkColor(COLORREF cr)
{
	return (BOOL)::SendMessage(m_hWnd,LVM_SETTEXTBKCOLOR,0,(LPARAM)cr);
}

inline int CListCtrl::GetTopIndex() const
{
	return (int)::SendMessage(m_hWnd,LVM_GETTOPINDEX,0,0);
}

inline int CListCtrl::GetCountPerPage() const
{
	return (int)::SendMessage(m_hWnd,LVM_GETCOUNTPERPAGE,0,0);
}

inline BOOL CListCtrl::GetOrigin(LPPOINT lpPoint) const
{
	return (BOOL)::SendMessage(m_hWnd,LVM_GETORIGIN,0,(LPARAM)lpPoint);
}

inline BOOL CListCtrl::SetItemState(int nItem, LV_ITEM* pItem)
{
	return (BOOL)::SendMessage(m_hWnd,LVM_SETITEMSTATE,nItem,(LPARAM)pItem);
}

inline UINT CListCtrl::GetItemState(int nItem, UINT nMask) const
{
	return (UINT)::SendMessage(m_hWnd,LVM_GETITEMSTATE,nItem,nMask);
}

inline void CListCtrl::SetItemCount(int nItems,DWORD dwFlags)
{
	::SendMessage(m_hWnd,LVM_SETITEMCOUNT,nItems,dwFlags);
}

inline UINT CListCtrl::GetSelectedCount() const
{
	return (UINT)::SendMessage(m_hWnd,LVM_GETSELECTEDCOUNT,0,0);
}

inline int CListCtrl::InsertItem(const LV_ITEM* pItem)
{
	return (int)::SendMessage(m_hWnd,LVM_INSERTITEM,0,(LPARAM)pItem);
}



inline BOOL CListCtrl::DeleteItem(int nItem)
{
	return (BOOL)::SendMessage(m_hWnd,LVM_DELETEITEM,nItem,0);
}

inline BOOL CListCtrl::DeleteAllItems()
{
	return (BOOL)::SendMessage(m_hWnd,LVM_DELETEALLITEMS,0,0);
}

inline int CListCtrl::FindItem(LV_FINDINFO* pFindInfo, int nStart) const
{
	return (int)::SendMessage(m_hWnd,LVM_FINDITEM,nStart,(LPARAM)pFindInfo);
}

inline int CListCtrl::HitTest(LV_HITTESTINFO* pHitTestInfo) const
{
	return (int)::SendMessage(m_hWnd,LVM_HITTEST,0,(LPARAM)pHitTestInfo);
}

inline BOOL CListCtrl::EnsureVisible(int nItem, BOOL bPartialOK)
{
	return (BOOL)::SendMessage(m_hWnd,LVM_ENSUREVISIBLE,nItem,bPartialOK);
}

inline BOOL CListCtrl::Scroll(SIZE size)
{
	return (BOOL)::SendMessage(m_hWnd,LVM_SCROLL,size.cx,size.cy);
}

inline BOOL CListCtrl::RedrawItems(int nFirst, int nLast)
{
	return (BOOL)::SendMessage(m_hWnd,LVM_REDRAWITEMS,nFirst,nLast);
}

inline BOOL CListCtrl::Arrange(UINT nCode)
{
	return (BOOL)::SendMessage(m_hWnd,LVM_ARRANGE,nCode,0);
}

inline HWND CListCtrl::EditLabel(int nItem)
{
	return (HWND)::SendMessage(m_hWnd,LVM_EDITLABEL,nItem,0);
}

inline int CListCtrl::InsertColumn(int nCol, const LV_COLUMN* pColumn)
{
	return (int)::SendMessage(m_hWnd,LVM_INSERTCOLUMN,nCol,(LPARAM)pColumn);
}


inline int CListCtrl::GetColumnCount() const
{
	LV_COLUMN lc;
	int iCount;
	lc.mask=LVCF_WIDTH;
	for (iCount=0;::SendMessage(m_hWnd,LVM_GETCOLUMN,(WPARAM)iCount,(LPARAM)&lc);iCount++);
	return iCount;
}

inline BOOL CListCtrl::DeleteColumn(int nCol)
{
	return (BOOL)::SendMessage(m_hWnd,LVM_DELETECOLUMN,nCol,0);
}

inline HIMAGELIST CListCtrl::CreateDragImage(int nItem, LPPOINT lpPoint)
{
	return (HIMAGELIST)::SendMessage(m_hWnd,LVM_CREATEDRAGIMAGE,nItem,(LPARAM)lpPoint);
}

inline BOOL CListCtrl::Update(int nItem)
{
	return (BOOL)::SendMessage(m_hWnd,LVM_UPDATE,nItem,0);
}

inline BOOL CListCtrl::SortItems(PFNLVCOMPARE pfnCompare, DWORD dwData)
{
	return (BOOL)::SendMessage(m_hWnd,LVM_SORTITEMS,(WPARAM)dwData,(LPARAM)pfnCompare);
}

inline void CListCtrl::RemoveImageList(int nImageList)
{
	::SendMessage(m_hWnd,LVM_SETIMAGELIST,nImageList,NULL);
}

inline HWND CListCtrl::GetHeader() const
{
	return (HWND)::SendMessage(m_hWnd,LVM_GETHEADER,0,0);
}

inline UINT CListCtrl::GetCheckState(int nItem) const
{
	return ListView_GetCheckState(m_hWnd,nItem);
}

inline void CListCtrl::SetCheckState(int nItem,UINT nState)
{
	ListView_SetCheckState(m_hWnd,nItem,nState);
}

inline DWORD CListCtrl::GetItemSpacing(int fSmall) const
{
	return (UINT)::SendMessage(m_hWnd,LVM_GETITEMSPACING,fSmall,0);
}

inline BOOL CListCtrl::GetISearchString(LPSTR lpsz) const
{
	return (BOOL)::SendMessage(m_hWnd,LVM_GETISEARCHSTRING,0,(LPARAM)lpsz);
}

inline DWORD CListCtrl::SetExtendedListViewStyle(DWORD dwMask,DWORD dwStyle)
{
	return (DWORD)::SendMessage(m_hWnd,LVM_SETEXTENDEDLISTVIEWSTYLE,dwMask,dwStyle);
}

inline DWORD CListCtrl::GetExtendedListViewStyle() const
{
	return (DWORD)::SendMessage(m_hWnd,LVM_GETEXTENDEDLISTVIEWSTYLE,0,0);
}

inline DWORD CListCtrl::SetIconSpacing(int cx, int cy)
{
	return (DWORD)::SendMessage(m_hWnd,LVM_SETICONSPACING,0,MAKELPARAM(cx,cy));
}

inline BOOL CListCtrl::GetSubItemRect(int iItem,int  iSubItem, LONG code,LPRECT prc) const
{
	return (BOOL)ListView_GetSubItemRect(m_hWnd,iItem,iSubItem,code,prc);
}
    
inline int CListCtrl::SubItemHitTest(LPLVHITTESTINFO plvhti)
{
	return (int)::SendMessage(m_hWnd,LVM_SUBITEMHITTEST,0,(LPARAM)plvhti);
}

inline BOOL CListCtrl::SetColumnOrderArray(int iCount,LPINT pi)
{
	return (BOOL)::SendMessage(m_hWnd,LVM_SETCOLUMNORDERARRAY,iCount,(LPARAM)pi);
}

inline BOOL CListCtrl::GetColumnOrderArray(int iCount,LPINT pi) const
{
	return (BOOL)::SendMessage(m_hWnd,LVM_GETCOLUMNORDERARRAY,iCount,(LPARAM)pi);
}

inline int CListCtrl::SetHotItem(int iItem)
{
	return (int)::SendMessage(m_hWnd,LVM_SETHOTITEM,iItem,0);
}

inline int CListCtrl::GetHotItem()
{
	return (int)::SendMessage(m_hWnd,LVM_GETHOTITEM,0,0);
}

inline HCURSOR CListCtrl::SetHotCursor(HCURSOR hCursor)
{
	return (HCURSOR)::SendMessage(m_hWnd,LVM_SETHOTCURSOR,0,(LPARAM)hCursor);
}

inline HCURSOR CListCtrl::GetHotCursor() const
{
	return (HCURSOR)::SendMessage(m_hWnd,LVM_GETHOTCURSOR,0,0);
}

inline DWORD CListCtrl::ApproximateViewRect(int iWidth,int iHeight,int iCount)
{
	return (DWORD)::SendMessage(m_hWnd,LVM_APPROXIMATEVIEWRECT, iCount, MAKELPARAM(iWidth, iHeight));
}

inline BOOL CListCtrl::SetWorkAreas(int nAreas,const LPRECT prc)
{
	return (BOOL)::SendMessage(m_hWnd,LVM_SETWORKAREAS,nAreas,(LPARAM)prc);
}

inline BOOL CListCtrl::GetWorkAreas(int nAreas,LPRECT prc) const
{
	return (BOOL)::SendMessage(m_hWnd,LVM_GETWORKAREAS,nAreas,(LPARAM)prc);
}

inline BOOL CListCtrl::GetNumberOfWorkAreas(int pnWorkAreas)
{
	return (BOOL)::SendMessage(m_hWnd,LVM_GETNUMBEROFWORKAREAS,0,(LPARAM)pnWorkAreas);
}

inline int CListCtrl::GetSelectionMark() const
{
	return (int)::SendMessage(m_hWnd,LVM_GETSELECTIONMARK,0,0);
}

inline int CListCtrl::SetSelectionMark(int nMark)
{
	return (int)::SendMessage(m_hWnd,LVM_SETSELECTIONMARK,0,(LPARAM)nMark);
}

inline DWORD CListCtrl::SetHoverTime(DWORD dwHoverTimeMs)
{
	return (DWORD)::SendMessage(m_hWnd,LVM_SETHOVERTIME,0,(LPARAM)dwHoverTimeMs);
}

inline DWORD CListCtrl::GetHoverTime() const
{
	return (DWORD)::SendMessage(m_hWnd,LVM_GETHOVERTIME,0,0);
}

inline HWND CListCtrl::SetToolTips(HWND hTooltips)
{
	return (HWND)::SendMessage(m_hWnd,LVM_SETTOOLTIPS,(WPARAM)hTooltips,0);
}

inline HWND CListCtrl::GetToolTips() const
{
	return (HWND)::SendMessage(m_hWnd,LVM_GETTOOLTIPS,0,0);
}

inline BOOL CListCtrl::SortItemsEx(PFNLVCOMPARE pfnCompare,LPARAM lParam)
{
	return (BOOL)::SendMessage(m_hWnd,LVM_SORTITEMSEX,(WPARAM)lParam,(LPARAM)pfnCompare);
}

inline BOOL CListCtrl::SetBkImage(const LVBKIMAGE* plvbki)
{
	return (BOOL)::SendMessage(m_hWnd,LVM_SETBKIMAGE,0,(LPARAM)plvbki);
}

inline BOOL CListCtrl::GetBkImage(LVBKIMAGE* plvbki) const
{
	return (BOOL)::SendMessage(m_hWnd,LVM_GETBKIMAGE,0,(LPARAM)plvbki);
}

inline DWORD CListCtrl::GetUnicodeFormat() const
{
	return (BOOL)::SendMessage(m_hWnd,LVM_GETUNICODEFORMAT,0,0);
}

inline void CListCtrl::SetUnicodeFormat(DWORD fUnicode)
{
	::SendMessage(m_hWnd,LVM_SETUNICODEFORMAT,(LPARAM)fUnicode,0);
}

inline void CListCtrl::SetTileWidth(const int* cpWidth)
{
	::SendMessage(m_hWnd,LVM_FIRST + 141 /*LVM_SETTILEWIDTH*/,(WPARAM)cpWidth,0);
}

inline void CListCtrl::SetSelectedColumn(UINT iCol)
{
	::SendMessage(m_hWnd,LVM_FIRST + 140 /*LVM_SETSELECTEDCOLUMN*/,iCol,0);
}

inline DWORD CListCtrl::SetView(DWORD iView)
{
	return (DWORD)::SendMessage(m_hWnd,LVM_FIRST + 142 /*LVM_SETVIEW*/,0,(LPARAM)iView);
}

inline DWORD CListCtrl::GetView() const
{
	return (DWORD)::SendMessage(m_hWnd,LVM_FIRST + 143 /*LVM_GETVIEW*/,0,0);
}
    
inline UINT CListCtrl::GetSelectedColumn() const
{
	return (UINT)::SendMessage(m_hWnd,LVM_FIRST + 174 /*LVM_GETSELECTEDCOLUMN*/,0,0);
}
    
    
inline COLORREF CListCtrl::GetOutlineColor() const
{
	return (COLORREF)::SendMessage(m_hWnd,LVM_FIRST + 176 /*LVM_GETOUTLINECOLOR*/,0,0);
}
    
inline COLORREF CListCtrl::SetOutlineColor(COLORREF crOutline)
{
	return (COLORREF)::SendMessage(m_hWnd,LVM_FIRST + 177 /*LVM_SETOUTLINECOLOR*/,0,LPARAM(crOutline));
}
    
inline void CListCtrl::CancelEditLabel()
{
	::SendMessage(m_hWnd,LVM_FIRST + 179 /*LVM_CANCELEDITLABEL*/,0,0);
}
    
inline int CListCtrl::EnableGroupView(BOOL fEnable)
{
	return (int)ListView_EnableGroupView(m_hWnd,fEnable);
}

inline int CListCtrl::IsGroupViewEnabled() const
{
	return (int)ListView_IsGroupViewEnabled(m_hWnd);
}

inline int CListCtrl::InsertGroup(int nIndex,const LVGROUP* pGroup)
{
	return (int)ListView_InsertGroup(m_hWnd,nIndex,pGroup);
}

inline void CListCtrl::InsertGroupSorted(const LVINSERTGROUPSORTED* pGroupSorted)
{
	ListView_InsertGroupSorted(m_hWnd,pGroupSorted);
}

inline int CListCtrl::GetGroupInfo(int iID,LVGROUP* pGroup)
{
	return (int)ListView_SetGroupInfo(m_hWnd,iID,pGroup);
}

inline void CListCtrl::GetGroupMetrics(LVGROUPMETRICS* pGroupMetrics)
{
	ListView_GetGroupMetrics(m_hWnd,pGroupMetrics);
}

inline int CListCtrl::SetGroupInfo(int iID,const LVGROUP* pGroup)
{
	return (int)ListView_SetGroupInfo(m_hWnd,iID,pGroup);
}

inline void CListCtrl::SetGroupMetrics(const LVGROUPMETRICS* pGroupMetrics)
{
	ListView_SetGroupMetrics(m_hWnd,pGroupMetrics);
}

inline void CListCtrl::RemoveAllGroups()
{
	ListView_RemoveAllGroups(m_hWnd);
}

inline int CListCtrl::RemoveGroup(int iID)
{
	return (int)ListView_RemoveGroup(m_hWnd,iID);
}

inline int CListCtrl::SortGroups(PFNLVGROUPCOMPARE pfnGroupCompare,void* plv)
{
	return (int)ListView_SortGroups(m_hWnd,pfnGroupCompare,plv);
}

#ifdef DEF_WCHAR
inline BOOL CListCtrl::GetColumn(int nCol, LV_COLUMNW* pColumn) const
{
	return (BOOL)::SendMessage(m_hWnd,LVM_GETCOLUMNW,(WPARAM)nCol,(LPARAM)pColumn);
}

inline BOOL CListCtrl::SetColumn(int nCol, const LV_COLUMNW* pColumn)
{
	return (BOOL)::SendMessage(m_hWnd,LVM_SETCOLUMNW,(WPARAM)nCol,(LPARAM)pColumn);
}

inline int CListCtrl::InsertColumn(int nCol, const LV_COLUMNW* pColumn)
{
	return (int)::SendMessage(m_hWnd,LVM_INSERTCOLUMNW,nCol,(LPARAM)pColumn);
}



inline BOOL CListCtrl::GetItem(LV_ITEMW* pItem) const
{
	return (BOOL)::SendMessage(m_hWnd,LVM_GETITEMW,0,(LPARAM)pItem);
}

inline BOOL CListCtrl::SetItem(const LV_ITEMW* pItem)
{
	return (BOOL)::SendMessage(m_hWnd,LVM_SETITEMW,0,(LPARAM)pItem);
}


inline int CListCtrl::InsertItem(const LV_ITEMW* pItem)
{
	return (int)::SendMessage(m_hWnd,LVM_INSERTITEMW,0,(LPARAM)pItem);
}


inline int CListCtrl::GetStringWidth(LPCWSTR lpsz) const
{
	return (int)::SendMessage(m_hWnd,LVM_GETSTRINGWIDTHW,0,(LPARAM)lpsz);
}


#endif

///////////////////////////////////////////
// CTreeCtrl

inline CTreeCtrl::CTreeCtrl()
:	CCommonCtrl(NULL)
{
}

inline CTreeCtrl::CTreeCtrl(HWND hWnd)
:	CCommonCtrl(hWnd)
{
}

inline BOOL CTreeCtrl::Create(DWORD dwStyle,const RECT* rect,HWND hParentWnd,UINT nID)
{
	return (m_hWnd=CreateWindowEx(0L,WC_TREEVIEW,szEmpty,dwStyle,
		rect->left,rect->top,rect->right-rect->left,rect->bottom-rect->top,
		hParentWnd,(HMENU)(ULONG_PTR)nID,GetInstanceHandle(),NULL))!=NULL;
}

inline UINT CTreeCtrl::GetCount() const
{
	return (UINT)::SendMessage(m_hWnd,TVM_GETCOUNT,0,0);
}

inline UINT CTreeCtrl::GetIndent() const
{
	return (UINT)::SendMessage(m_hWnd,TVM_GETINDENT,0,0);
}

inline void CTreeCtrl::SetIndent(UINT nIndent)
{
	::SendMessage(m_hWnd,TVM_SETINDENT,nIndent,0);
}

inline HIMAGELIST CTreeCtrl::GetImageList(UINT nImageList) const
{
	return (HIMAGELIST)::SendMessage(m_hWnd,TVM_GETIMAGELIST,(WPARAM)nImageList,0);
}

inline HIMAGELIST CTreeCtrl::SetImageList(HIMAGELIST hImageList,int nImageListType)
{
	return (HIMAGELIST)::SendMessage(m_hWnd,TVM_SETIMAGELIST,(WPARAM)nImageListType,(LPARAM)hImageList);
}

inline HTREEITEM CTreeCtrl::GetNextItem(HTREEITEM hItem,UINT nCode) const
{
	return (HTREEITEM)::SendMessage(m_hWnd,TVM_GETNEXTITEM,nCode,(LPARAM)hItem);
}

inline HTREEITEM CTreeCtrl::GetChildItem(HTREEITEM hItem) const
{
	return (HTREEITEM)::SendMessage(m_hWnd,TVM_GETNEXTITEM,TVGN_CHILD,(WPARAM)hItem);
}

inline HTREEITEM CTreeCtrl::GetNextSiblingItem(HTREEITEM hItem) const
{
	return (HTREEITEM)::SendMessage(m_hWnd,TVM_GETNEXTITEM,TVGN_NEXT,(WPARAM)hItem);
}

inline HTREEITEM CTreeCtrl::GetPrevSiblingItem(HTREEITEM hItem) const
{
	return (HTREEITEM)::SendMessage(m_hWnd,TVM_GETNEXTITEM,TVGN_PREVIOUS,(WPARAM)hItem);
}

inline HTREEITEM CTreeCtrl::GetParentItem(HTREEITEM hItem) const
{
	return (HTREEITEM)::SendMessage(m_hWnd,TVM_GETNEXTITEM,TVGN_PARENT,(WPARAM)hItem);
}

inline HTREEITEM CTreeCtrl::GetFirstVisibleItem() const
{
	return (HTREEITEM)::SendMessage(m_hWnd,TVM_GETNEXTITEM,TVGN_FIRSTVISIBLE,(WPARAM)NULL);
}

inline BOOL CTreeCtrl::GetItemRect(HTREEITEM hItem,LPRECT lpRect,BOOL bTextOnly) const
{
	return TreeView_GetItemRect(m_hWnd,hItem,lpRect,bTextOnly);
}

inline HTREEITEM CTreeCtrl::GetNextVisibleItem(HTREEITEM hItem) const
{
	return (HTREEITEM)::SendMessage(m_hWnd,TVM_GETNEXTITEM,TVGN_NEXTVISIBLE,(WPARAM)hItem);
}

inline HTREEITEM CTreeCtrl::GetPrevVisibleItem(HTREEITEM hItem) const
{
	return (HTREEITEM)::SendMessage(m_hWnd,TVM_GETNEXTITEM,TVGN_PREVIOUSVISIBLE,(WPARAM)hItem);
}

inline HTREEITEM CTreeCtrl::GetSelectedItem() const
{
	return (HTREEITEM)::SendMessage(m_hWnd,TVM_GETNEXTITEM,TVGN_CARET,(WPARAM)NULL);
}

inline HTREEITEM CTreeCtrl::GetDropHilightItem() const
{
	return (HTREEITEM)::SendMessage(m_hWnd,TVM_GETNEXTITEM,TVGN_DROPHILITE,(WPARAM)NULL);
}

inline HTREEITEM CTreeCtrl::GetRootItem() const
{
	return (HTREEITEM)::SendMessage(m_hWnd,TVM_GETNEXTITEM,TVGN_ROOT,(WPARAM)NULL);
}

inline BOOL CTreeCtrl::GetItem(TV_ITEMA* pItem) const
{
	return (BOOL)::SendMessage(m_hWnd,TVM_GETITEMA,0,(LPARAM)pItem);
}

inline BOOL CTreeCtrl::GetItem(TV_ITEMW* pItem) const
{
	return (BOOL)::SendMessage(m_hWnd,TVM_GETITEMW,0,(LPARAM)pItem);
}

inline BOOL CTreeCtrl::SetItem(TV_ITEMA* pItem)
{
	return (BOOL)::SendMessage(m_hWnd,TVM_SETITEMA,0,(LPARAM)pItem);
}

inline BOOL CTreeCtrl::SetItem(TV_ITEMW* pItem)
{
	return (BOOL)::SendMessage(m_hWnd,TVM_SETITEMW,0,(LPARAM)pItem);
}

inline BOOL CTreeCtrl::ItemHasChildren(HTREEITEM hItem) const
{
	return (::SendMessage(m_hWnd,TVM_GETNEXTITEM,TVGN_CHILD,(LPARAM)hItem)!=NULL);
}

inline HWND CTreeCtrl::GetEditControl() const
{
	return (HWND)::SendMessage(m_hWnd,TVM_GETEDITCONTROL,0,0);
}

inline UINT CTreeCtrl::GetVisibleCount() const
{
	return (UINT)::SendMessage(m_hWnd,TVM_GETVISIBLECOUNT,0,0);
}

inline HTREEITEM CTreeCtrl::InsertItem(LPTV_INSERTSTRUCTA lpInsertStruct)
{
	return (HTREEITEM)::SendMessage(m_hWnd,TVM_INSERTITEMA,0,(LPARAM)lpInsertStruct);
}

inline HTREEITEM CTreeCtrl::InsertItem(LPTV_INSERTSTRUCTW lpInsertStruct)
{
	return (HTREEITEM)::SendMessage(m_hWnd,TVM_INSERTITEMW,0,(LPARAM)lpInsertStruct);
}

inline BOOL CTreeCtrl::DeleteItem(HTREEITEM hItem)
{
	return (BOOL)::SendMessage(m_hWnd,TVM_DELETEITEM,0,(LPARAM)hItem);
}

inline BOOL CTreeCtrl::DeleteAllItems()
{
	return (BOOL)::SendMessage(m_hWnd,TVM_DELETEITEM,0,(LPARAM)TVI_ROOT);
}

inline BOOL CTreeCtrl::Expand(HTREEITEM hItem,UINT nCode)
{
	return (BOOL)::SendMessage(m_hWnd,TVM_EXPAND,(WPARAM)nCode,(LPARAM)hItem);
}

inline BOOL CTreeCtrl::Select(HTREEITEM hItem,UINT nCode)
{
	return (BOOL)::SendMessage(m_hWnd,TVM_SELECTITEM,(WPARAM)nCode,(LPARAM)hItem);
}

inline BOOL CTreeCtrl::SelectItem(HTREEITEM hItem)
{
	return (BOOL)::SendMessage(m_hWnd,TVM_SELECTITEM,TVGN_CARET,(LPARAM)hItem);
}

inline BOOL CTreeCtrl::SelectDropTarget(HTREEITEM hItem)
{
	return (BOOL)::SendMessage(m_hWnd,TVM_SELECTITEM,TVGN_DROPHILITE,(LPARAM)hItem);
}

inline BOOL CTreeCtrl::SelectSetFirstVisible(HTREEITEM hItem)
{
	return (BOOL)::SendMessage(m_hWnd,TVM_SELECTITEM,TVGN_FIRSTVISIBLE,(LPARAM)hItem);
}

inline HWND CTreeCtrl::EditLabel(HTREEITEM hItem)
{
	return (HWND)::SendMessage(m_hWnd,TVM_EDITLABEL,0,(LPARAM)hItem);
}

inline HTREEITEM CTreeCtrl::HitTest(TV_HITTESTINFO* pHitTestInfo) const
{
	return (HTREEITEM)::SendMessage(m_hWnd,TVM_HITTEST,0,(LPARAM)pHitTestInfo);
}

inline HIMAGELIST CTreeCtrl::CreateDragImage(HTREEITEM hItem)
{
	return (HIMAGELIST)::SendMessage(m_hWnd,TVM_CREATEDRAGIMAGE,0,(LPARAM)hItem);
}

inline BOOL CTreeCtrl::SortChildren(HTREEITEM hItem)
{
	return (BOOL)::SendMessage(m_hWnd,TVM_SORTCHILDREN,0,(LPARAM)hItem);
}

inline BOOL CTreeCtrl::EnsureVisible(HTREEITEM hItem)
{
	return (BOOL)::SendMessage(m_hWnd,TVM_ENSUREVISIBLE,0,(LPARAM)hItem);
}

inline BOOL CTreeCtrl::SortChildrenCB(LPTV_SORTCB pSort)
{
	return (BOOL)::SendMessage(m_hWnd,TVM_SORTCHILDRENCB,0,(LPARAM)pSort);
}

inline UINT CTreeCtrl::GetCheckState(HTREEITEM hItem) const
{
	return ((((UINT)(SNDMSG((m_hWnd), TVM_GETITEMSTATE, (WPARAM)(hItem), TVIS_STATEIMAGEMASK))) >> 12) -1);
}

inline void CTreeCtrl::SetCheckState(HTREEITEM hItem,UINT nState)
{
	TreeView_SetItemState(m_hWnd, hItem, INDEXTOSTATEIMAGEMASK((nState)?2:1), TVIS_STATEIMAGEMASK);
}

inline UINT CTreeCtrl::GetRadioState(HTREEITEM hItem) const
{
	return ((((UINT)(SNDMSG((m_hWnd), TVM_GETITEMSTATE, (WPARAM)(hItem), TVIS_STATEIMAGEMASK))) >> 12) -3);
}

inline void CTreeCtrl::SetRadioState(HTREEITEM hItem,UINT nState)
{
	TreeView_SetItemState(m_hWnd, hItem, INDEXTOSTATEIMAGEMASK((nState)?4:3), TVIS_STATEIMAGEMASK);
}

inline int CTreeCtrl::GetItemHeight() const
{
	return TreeView_GetItemHeight(m_hWnd);
}

inline int CTreeCtrl::SetItemHeight(SHORT cuHeight)
{
	return TreeView_SetItemHeight(m_hWnd,cuHeight);
}


///////////////////////////////////////////
// CComboBoxEx

inline CComboBoxEx::CComboBoxEx()
:	CCommonCtrl(NULL),CComboBox(NULL),CEdit(NULL)
{
}

inline CComboBoxEx::CComboBoxEx(HWND hWnd)
:	CCommonCtrl(hWnd),CComboBox(NULL),CEdit(NULL)
{
	CComboBox::m_hWnd=(HWND)::SendMessage(hWnd,CBEM_GETCOMBOCONTROL,0,0);
	CEdit::m_hWnd=(HWND)::SendMessage(hWnd,CBEM_GETEDITCONTROL,0,0);
}

inline void CComboBoxEx::AttachToDlgItem(HWND hDialog,int nID)
{ 
	CCommonCtrl::m_hWnd=::GetDlgItem(hDialog,nID); 
	CComboBox::m_hWnd=(HWND)::SendMessage(CCommonCtrl::m_hWnd,CBEM_GETCOMBOCONTROL,0,0);
	CEdit::m_hWnd=(HWND)::SendMessage(CCommonCtrl::m_hWnd,CBEM_GETEDITCONTROL,0,0);
}

inline void CComboBoxEx::Attach(HWND hWnd) 
{
	CCommonCtrl::m_hWnd=hWnd; 
	CComboBox::m_hWnd=(HWND)::SendMessage(hWnd,CBEM_GETCOMBOCONTROL,0,0);
	CEdit::m_hWnd=(HWND)::SendMessage(hWnd,CBEM_GETEDITCONTROL,0,0);
}
	
	

inline int CComboBoxEx::InsertItem(const COMBOBOXEXITEM* pItem)
{
	return (int)::SendMessage(CCommonCtrl::m_hWnd,CBEM_INSERTITEM,0,(LPARAM)pItem);
}

inline int CComboBoxEx::InsertItem(const COMBOBOXEXITEMW* pItem)
{
	return (int)::SendMessage(CCommonCtrl::m_hWnd,CBEM_INSERTITEMW,0,(LPARAM)pItem);
}

inline HIMAGELIST CComboBoxEx::GetImageList() const
{
	return (HIMAGELIST)::SendMessage(CCommonCtrl::m_hWnd,CBEM_GETIMAGELIST,0,0);
}

inline HIMAGELIST CComboBoxEx::SetImageList(HIMAGELIST hImageList)
{
	return (HIMAGELIST)::SendMessage(CCommonCtrl::m_hWnd,CBEM_SETIMAGELIST,0,(LPARAM)hImageList);
}

inline BOOL CComboBoxEx::GetItem(COMBOBOXEXITEM* pItem) const
{
	return (BOOL)::SendMessage(CCommonCtrl::m_hWnd,CBEM_GETITEM,0,(LPARAM)pItem);
}

inline BOOL CComboBoxEx::GetItem(COMBOBOXEXITEMW* pItem) const
{
	return (BOOL)::SendMessage(CCommonCtrl::m_hWnd,CBEM_GETITEMW,0,(LPARAM)pItem);
}

inline BOOL CComboBoxEx::SetItem(const COMBOBOXEXITEM* pItem)
{
	return (BOOL)::SendMessage(CCommonCtrl::m_hWnd,CBEM_SETITEM,0,(LPARAM)pItem);
}

inline BOOL CComboBoxEx::SetItem(const COMBOBOXEXITEMW* pItem)
{
	return (BOOL)::SendMessage(CCommonCtrl::m_hWnd,CBEM_SETITEMW,0,(LPARAM)pItem);
}

inline BOOL CComboBoxEx::DeleteItem(int nItem)
{
	return (BOOL)::SendMessage(CCommonCtrl::m_hWnd,CBEM_DELETEITEM,nItem,0);
}

inline HWND CComboBoxEx::GetComboControl() const
{
	return CComboBox::m_hWnd;
}

inline HWND CComboBoxEx::GetEditControl() const
{
	return CEdit::m_hWnd;
}

inline DWORD CComboBoxEx::GetExStyle() const
{
	return (DWORD)::SendMessage(CCommonCtrl::m_hWnd,CBEM_GETEXSTYLE,0,0);
}

inline DWORD CComboBoxEx::SetExStyle(DWORD dwExStyle)
{
	return (DWORD)::SendMessage(CCommonCtrl::m_hWnd,CBEM_GETEXSTYLE,0,dwExStyle);
}

inline BOOL CComboBoxEx::HasEditChanged() const
{
	return (BOOL)::SendMessage(CCommonCtrl::m_hWnd,CBEM_HASEDITCHANGED,0,0);
}

///////////////////////////////////////////
// CAnimateCtrl

#ifdef DEF_RESOURCES
inline CAnimateCtrl::CAnimateCtrl()
:	CCommonCtrl(NULL)
{
}

inline CAnimateCtrl::CAnimateCtrl(HWND hWnd)
:	CCommonCtrl(hWnd)
{
}

inline BOOL CAnimateCtrl::Create(DWORD dwStyle, const RECT* rect, HWND hParentWnd, UINT nID)
{
	return (m_hWnd=CreateWindowEx(0L,ANIMATE_CLASS,szEmpty,dwStyle,
		rect->left,rect->top,rect->right-rect->left,rect->bottom-rect->top,
		hParentWnd,(HMENU)(ULONG_PTR)nID,GetInstanceHandle(),NULL))!=NULL;
}

inline BOOL CAnimateCtrl::Open(LPTSTR szName)
{
	return (BOOL)::SendMessage(m_hWnd,ACM_OPEN,0,(LPARAM)szName);
}

inline BOOL CAnimateCtrl::OpenEx(LPTSTR szName)
{
	return (BOOL)::SendMessage(m_hWnd,ACM_OPEN,(WPARAM)GetLanguageSpecificResourceHandle(),(LPARAM)szName);
}

inline BOOL CAnimateCtrl::Play(WORD from, WORD to, DWORD rep)
{
	return (BOOL)::SendMessage(m_hWnd,ACM_PLAY, (WPARAM)rep, MAKELPARAM(from, to));
}

inline BOOL CAnimateCtrl::Stop()
{
	return (BOOL)::SendMessage(m_hWnd,ACM_STOP,0,0);
}

inline BOOL CAnimateCtrl::Close()
{
	return (BOOL)::SendMessage(m_hWnd,ACM_OPEN,NULL,NULL);
}

inline BOOL CAnimateCtrl::Seek(DWORD frame)
{
	return (BOOL)::SendMessage(m_hWnd,ACM_PLAY, (WPARAM)1, MAKELPARAM(frame, frame));
}
#endif
///////////////////////////////////////////
// CMonthCalCtrl

inline CMonthCalCtrl::CMonthCalCtrl()
:	CCommonCtrl(NULL)
{
}

inline CMonthCalCtrl::CMonthCalCtrl(HWND hWnd)
:	CCommonCtrl(hWnd)
{
}

inline BOOL CMonthCalCtrl::Create(DWORD dwStyle, const RECT* rect, HWND hParentWnd, UINT nID)
{
	return (m_hWnd=CreateWindowEx(0L,MONTHCAL_CLASS,szEmpty,dwStyle,
		rect->left,rect->top,rect->right-rect->left,rect->bottom-rect->top,
		hParentWnd,(HMENU)(ULONG_PTR)nID,GetInstanceHandle(),NULL))!=NULL;
}

inline BOOL CMonthCalCtrl::GetCurSel(LPSYSTEMTIME pst) const
{
	return (BOOL)::SendMessage(m_hWnd,MCM_GETCURSEL,0,(LPARAM)pst);
}

inline BOOL CMonthCalCtrl::SetCurSel(LPSYSTEMTIME pst)
{
	return (BOOL)::SendMessage(m_hWnd,MCM_SETCURSEL,0,(LPARAM)pst);
}

inline DWORD CMonthCalCtrl::GetMaxSelCount() const
{
	return (DWORD)::SendMessage(m_hWnd,MCM_GETMAXSELCOUNT,0,0);
}

inline BOOL CMonthCalCtrl::SetMaxSelCount(UINT n)
{
	return (BOOL)::SendMessage(m_hWnd,MCM_SETMAXSELCOUNT,(WPARAM)n,0);
}

inline BOOL CMonthCalCtrl::GetSelRange(LPSYSTEMTIME rgst) const
{
	return (BOOL)::SendMessage(m_hWnd,MCM_GETSELRANGE,0,(LPARAM)rgst);
}

inline BOOL CMonthCalCtrl::SetSelRange(LPSYSTEMTIME rgst)
{
	return (BOOL)::SendMessage(m_hWnd,MCM_SETSELRANGE,0,(LPARAM)rgst);
}

inline DWORD CMonthCalCtrl::GetMonthRange(DWORD gmr, LPSYSTEMTIME rgst) const
{
	return (DWORD)::SendMessage(m_hWnd,MCM_GETMONTHRANGE,(WPARAM)gmr,(LPARAM)rgst);
}

inline BOOL CMonthCalCtrl::SetDayState(int cbds, LPMONTHDAYSTATE rgds)
{
	return (BOOL)::SendMessage(m_hWnd,MCM_SETDAYSTATE,cbds,(LPARAM)rgds);
}

inline BOOL CMonthCalCtrl::GetMinReqRect(LPRECT prc) const
{
	return (BOOL)::SendMessage(m_hWnd,MCM_GETMINREQRECT,0,(LPARAM)prc);
}

inline BOOL CMonthCalCtrl::SetColor(int iColor,COLORREF clr)
{
	return (BOOL)::SendMessage(m_hWnd,MCM_SETCOLOR,iColor,(LPARAM)clr);
}

inline COLORREF CMonthCalCtrl::GetColor(int iColor) const
{
	return (COLORREF)::SendMessage(m_hWnd,MCM_GETCOLOR,iColor,0);
}

inline BOOL CMonthCalCtrl::SetToday(LPSYSTEMTIME pst)
{
	return (BOOL)::SendMessage(m_hWnd,MCM_SETTODAY,0,(LPARAM)pst);
}

inline BOOL CMonthCalCtrl::GetToday(LPSYSTEMTIME pst)
{
	return (BOOL)::SendMessage(m_hWnd,MCM_GETTODAY,0,(LPARAM)pst);
}

inline BOOL CMonthCalCtrl::HitTest(PMCHITTESTINFO pinfo)
{
	return (BOOL)::SendMessage(m_hWnd,MCM_HITTEST,0,(LPARAM)pinfo);
}

inline BOOL CMonthCalCtrl::SetFirstDayOfWeek(int iDay)
{
	return (BOOL)::SendMessage(m_hWnd,MCM_SETFIRSTDAYOFWEEK,0,(LPARAM)iDay);
}

inline DWORD CMonthCalCtrl::GetFirstDayOfWeek() const
{
	return (DWORD)::SendMessage(m_hWnd,MCM_GETFIRSTDAYOFWEEK,0,0);
}

inline DWORD CMonthCalCtrl::GetRange(LPSYSTEMTIME rgst) const
{
	return (DWORD)::SendMessage(m_hWnd,MCM_GETRANGE,0,(LPARAM)rgst);
}

inline BOOL CMonthCalCtrl::SetRange(DWORD gdtr, LPSYSTEMTIME rgst)
{
	return (BOOL)::SendMessage(m_hWnd,MCM_SETRANGE,(WPARAM)gdtr,(LPARAM)rgst);
}

inline int CMonthCalCtrl::GetMonthDelta() const
{
	return (int)::SendMessage(m_hWnd,MCM_GETMONTHDELTA,0,0);
}

inline int CMonthCalCtrl::SetMonthDelta(int n)
{
	return (int)::SendMessage(m_hWnd,MCM_SETMONTHDELTA,(WPARAM)n,0);
}

inline DWORD CMonthCalCtrl::GetMaxTodayWidth(LPSIZE psz) const
{
	return (DWORD)::SendMessage(m_hWnd,MCM_GETCURSEL,0,(LPARAM)psz);
}

///////////////////////////////////////////
// CDateTimeCtrl

inline CDateTimeCtrl::CDateTimeCtrl()
:	CCommonCtrl(NULL)
{
}

inline CDateTimeCtrl::CDateTimeCtrl(HWND hWnd)
:	CCommonCtrl(hWnd)
{
}

inline BOOL CDateTimeCtrl::Create(DWORD dwStyle, const RECT* rect, HWND hParentWnd, UINT nID)
{
	return (m_hWnd=CreateWindowEx(0L,DATETIMEPICK_CLASS,szEmpty,dwStyle,
		rect->left,rect->top,rect->right-rect->left,rect->bottom-rect->top,
		hParentWnd,(HMENU)(ULONG_PTR)nID,GetInstanceHandle(),NULL))!=NULL;
}

inline DWORD CDateTimeCtrl::GetSystemtime(LPSYSTEMTIME pst) const
{
	return (DWORD)::SendMessage(m_hWnd,DTM_GETSYSTEMTIME,0,(LPARAM)pst);
}

inline BOOL CDateTimeCtrl::SetSystemtime(DWORD gd, LPSYSTEMTIME pst)
{
	return (BOOL)::SendMessage(m_hWnd,DTM_SETSYSTEMTIME,(WPARAM)gd,(LPARAM)pst);
}

inline DWORD CDateTimeCtrl::GetRange(LPSYSTEMTIME rgst) const
{
	return (DWORD)::SendMessage(m_hWnd,DTM_GETRANGE,0,(LPARAM)rgst);
}

inline BOOL CDateTimeCtrl::SetRange(DWORD gdtr, LPSYSTEMTIME rgst)
{
	return (BOOL)::SendMessage(m_hWnd,DTM_SETRANGE,(WPARAM)gdtr,(LPARAM)rgst);
}

inline BOOL CDateTimeCtrl::SetFormat(LPCTSTR sz)
{
	return (BOOL)::SendMessage(m_hWnd,DTM_SETFORMAT,0,(LPARAM)sz);
}

inline BOOL CDateTimeCtrl::SetMonthCalColor(int iColor, COLORREF clr)
{
	return (BOOL)::SendMessage(m_hWnd,DTM_SETMCCOLOR,iColor,(LPARAM)clr);
}

inline COLORREF CDateTimeCtrl::GetMonthCalColor(int iColor) const
{
	return (COLORREF)::SendMessage(m_hWnd,DTM_GETMCCOLOR,iColor,0);
}

inline HWND CDateTimeCtrl::GetMonthCal()
{
	return (HWND)::SendMessage(m_hWnd,DTM_GETMONTHCAL,0,0);
}

inline BOOL CDateTimeCtrl::SetMonthCalFont(HFONT hfont,DWORD fRedraw)
{
	return (BOOL)::SendMessage(m_hWnd,DTM_SETMCFONT,(WPARAM)hfont,(LPARAM)fRedraw);
}

inline HFONT CDateTimeCtrl::GetMonthCalFont() const
{
	return (HFONT)::SendMessage(m_hWnd,DTM_GETMCFONT,0,0);
}



#endif
