////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////

#ifndef HFCCONTROLS_INL
#define HFCCONTROLS_INL



///////////////////////////
// Class CButton

inline CButton::CButton()
:	CWnd(NULL)
{
}

inline CButton::CButton(HWND hWnd)
:	CWnd(hWnd)
{
}

inline BOOL CButton::Create(LPCTSTR lpszCaption,DWORD dwStyle,const RECT* rect,HWND hParentWnd,UINT nID)
{
	return (m_hWnd=CreateWindowEx(0L,"BUTTON",lpszCaption,dwStyle,
		rect->left,rect->top,rect->right-rect->left,rect->bottom-rect->top,
		hParentWnd,(HMENU)(LONG_PTR)nID,GetInstanceHandle(), 0L))!=NULL;
}

inline UINT CButton::GetState() const
{
	return (UINT)::SendMessage(m_hWnd,BM_GETSTATE,0,0);
}

inline void CButton::SetState(BOOL bHighlight)
{
	::SendMessage(m_hWnd,BM_SETSTATE,(WPARAM)bHighlight,0);
}

inline int CButton::GetCheck() const
{
	return (int)::SendMessage(m_hWnd,BM_GETCHECK,0,0);
}

inline void CButton::SetCheck(int nCheck)
{
	::SendMessage(m_hWnd,BM_SETCHECK,(WPARAM)nCheck,0);
}

inline UINT CButton::GetButtonStyle() const
{
	return ::GetWindowLong(m_hWnd,GWL_STYLE);
}

inline void CButton::SetButtonStyle(UINT nStyle,BOOL bRedraw)
{
	::SendMessage(m_hWnd,BM_SETSTYLE,(WPARAM)nStyle,(LPARAM)bRedraw);
}

inline HICON CButton::SetIcon(HICON hIcon)
{
	return (HICON)::SendMessage(m_hWnd,BM_SETIMAGE,0,(LPARAM)hIcon);
}

inline HICON CButton::GetIcon() const
{
	return (HICON)::SendMessage(m_hWnd,BM_GETIMAGE,0,0);
}

inline HBITMAP CButton::SetBitmap(HBITMAP hBitmap)
{
	return (HBITMAP)::SendMessage(m_hWnd,BM_SETIMAGE,0,(LPARAM)hBitmap);
}

inline HBITMAP CButton::GetBitmap() const
{
	return (HBITMAP)::SendMessage(m_hWnd,BM_GETIMAGE,0,0);
}

inline HCURSOR CButton::SetCursor(HCURSOR hCursor)
{
	return (HCURSOR)::SendMessage(m_hWnd,BM_SETIMAGE,0,(LPARAM)hCursor);
}

inline HCURSOR CButton::GetCursor()
{
	return (HCURSOR)::SendMessage(m_hWnd,BM_GETIMAGE,0,0);
}

///////////////////////////
// Class CEdit


inline CEdit::CEdit() 
:	CWnd(NULL)
{
}

inline CEdit::CEdit(HWND hWnd) 
:	CWnd(hWnd)
{
}

inline BOOL CEdit::Create(DWORD dwStyle,const RECT* rect,HWND hParentWnd,UINT nID)
{
	return (m_hWnd=CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT",szEmpty,dwStyle, 
		rect->left,rect->top,rect->right-rect->left,rect->bottom-rect->top,
		hParentWnd,(HMENU)(LONG_PTR)nID,GetInstanceHandle(), 0L))!=NULL;
}

inline void CEdit::LimitText(int iLimit)
{
	::SendMessage(m_hWnd,EM_LIMITTEXT,(WPARAM)iLimit,0);
}

inline void CEdit::SetLimitText(int iLimit)
{
	::SendMessage(m_hWnd,EM_LIMITTEXT,(WPARAM)iLimit,0);
}

inline void CEdit::SetSel(int iStart,int iEnd)
{
	::SendMessage(m_hWnd,EM_SETSEL,(WPARAM)iStart,(LPARAM)iEnd);
}

inline BOOL CEdit::Undo()
{
	return (BOOL)::SendMessage(m_hWnd,EM_UNDO,0,0);
}

inline void CEdit::Clear()
{
	::SendMessage(m_hWnd,WM_CLEAR,0,0);
}

inline void CEdit::Copy()
{
	::SendMessage(m_hWnd,WM_COPY,0,0);
}

inline void CEdit::Cut()
{
	::SendMessage(m_hWnd,WM_CUT,0,0);
}

inline void CEdit::Paste()
{
	::SendMessage(m_hWnd,WM_PASTE,0,0);
}

///////////////////////////////////////////
// CRichEditCtrl

inline CRichEditCtrl::CRichEditCtrl()
:	CWnd(NULL)
{
}

inline CRichEditCtrl::CRichEditCtrl(HWND hWnd)
:	CWnd(hWnd)
{
}

inline BOOL CRichEditCtrl::Create(DWORD dwStyle, const RECT* rect, HWND hParentWnd, UINT nID)
{
	return (m_hWnd=CreateWindowEx(0L,"RichEdit",szEmpty,
      dwStyle,rect->left,rect->top,rect->right-rect->left,rect->bottom-rect->top,
      hParentWnd,(HMENU)(LONG_PTR)nID,GetInstanceHandle(),NULL))!=NULL;
}

inline BOOL CRichEditCtrl::CreateV2(DWORD dwStyle, const RECT* rect, HWND hParentWnd, UINT nID)
{
	return (m_hWnd=CreateWindowEx(0L,RICHEDIT_CLASS,szEmpty,
      dwStyle,rect->left,rect->top,rect->right-rect->left,rect->bottom-rect->top,
      hParentWnd,(HMENU)(LONG_PTR)nID,GetInstanceHandle(),NULL))!=NULL;
}

inline BOOL CRichEditCtrl::CanUndo() const
{
	return (BOOL)::SendMessage(m_hWnd,EM_CANUNDO,0,0);
}

inline BOOL CRichEditCtrl::CanRedo() const
{
	return (BOOL)::SendMessage(m_hWnd,EM_CANREDO,0,0);
}

inline UNDONAMEID CRichEditCtrl::GetUndoName() const
{
	return (UNDONAMEID)::SendMessage(m_hWnd,EM_GETUNDONAME,0,0);
}

inline UNDONAMEID CRichEditCtrl::GetRedoName() const
{
	return (UNDONAMEID)::SendMessage(m_hWnd,EM_GETREDONAME,0,0);
}

inline int CRichEditCtrl::GetLineCount() const
{
	return (int)::SendMessage(m_hWnd,EM_GETLINECOUNT,0,0);
}

inline BOOL CRichEditCtrl::GetModify() const
{
	return (BOOL)::SendMessage(m_hWnd,EM_GETMODIFY,0,0);
}

inline void CRichEditCtrl::SetModify(BOOL bModified)
{
	::SendMessage(m_hWnd,EM_SETMODIFY,(WPARAM)bModified,0);
}

inline void CRichEditCtrl::GetRect(LPRECT lpRect) const
{
	::SendMessage(m_hWnd,EM_GETRECT,0,(LPARAM)lpRect);
}

inline void CRichEditCtrl::SetOptions(WORD wOp, DWORD dwFlags)
{
	::SendMessage(m_hWnd,EM_SETOPTIONS,wOp,dwFlags);
}

inline int CRichEditCtrl::GetLine(int nIndex,LPTSTR lpszBuffer) const
{
	return (int)::SendMessage(m_hWnd,EM_GETLINE,nIndex,(LPARAM)lpszBuffer);
}

inline BOOL CRichEditCtrl::CanPaste(UINT nFormat) const
{
	return (BOOL)::SendMessage(m_hWnd,EM_CANPASTE,nFormat,0);
}

inline void CRichEditCtrl::GetSel(long& nStartChar,long& nEndChar) const
{
	CHARRANGE cr;
	::SendMessage(m_hWnd,EM_EXGETSEL,(WPARAM)0,(LPARAM)&cr);
	nStartChar=cr.cpMin;
	nEndChar=cr.cpMax;
}

inline void CRichEditCtrl::GetSel(CHARRANGE &cr) const
{
	::SendMessage(m_hWnd,EM_EXGETSEL,0,(LPARAM)&cr);
}

inline void CRichEditCtrl::LimitText(long nChars)
{
	::SendMessage(m_hWnd,EM_LIMITTEXT,nChars,0);
}

inline long CRichEditCtrl::LineFromChar(long nIndex) const
{
	return (long)::SendMessage(m_hWnd,EM_LINEFROMCHAR,nIndex,0);
}

inline void CRichEditCtrl::SetSel(long nStartChar,long nEndChar)
{
	CHARRANGE cr;
	cr.cpMin=nStartChar;
	cr.cpMax=nEndChar;
	::SendMessage(m_hWnd,EM_EXSETSEL,0,(LPARAM)&cr);
}

inline void CRichEditCtrl::SetSel(CHARRANGE &cr)
{
	::SendMessage(m_hWnd,EM_EXSETSEL,0,(LPARAM)&cr);
}

inline DWORD CRichEditCtrl::GetDefaultCharFormat(CHARFORMAT &cf) const
{
	return (DWORD)::SendMessage(m_hWnd,EM_GETCHARFORMAT,0,(LPARAM)&cf);
}

inline DWORD CRichEditCtrl::GetSelectionCharFormat(CHARFORMAT &cf) const
{
	return (DWORD)::SendMessage(m_hWnd,EM_GETCHARFORMAT,1,(LPARAM)&cf);
}

inline DWORD CRichEditCtrl::GetSelectionCharFormat(CHARFORMAT2 &cf) const
{
	return (DWORD)::SendMessage(m_hWnd,EM_GETCHARFORMAT,1,(LPARAM)&cf);
}

inline long CRichEditCtrl::GetEventMask() const
{
	return (long)::SendMessage(m_hWnd,EM_GETEVENTMASK,0,0);
}

inline int CRichEditCtrl::GetLimitText() const
{
	return (long)::SendMessage(m_hWnd,EM_GETLIMITTEXT,0,0);
}

inline DWORD CRichEditCtrl::GetParaFormat(PARAFORMAT &pf) const
{
	return (DWORD)::SendMessage(m_hWnd,EM_GETPARAFORMAT,0,(LPARAM)&pf);
}

inline DWORD CRichEditCtrl::GetParaFormat(PARAFORMAT2 &pf) const
{
	return (DWORD)::SendMessage(m_hWnd,EM_GETPARAFORMAT,0,(LPARAM)&pf);
}

inline long CRichEditCtrl::GetSelText(LPSTR lpBuf) const
{
	return (long)::SendMessage(m_hWnd,EM_GETSELTEXT,0,(LPARAM)lpBuf);
}

inline WORD CRichEditCtrl::GetSelectionType() const
{
	return (WORD)::SendMessage(m_hWnd,EM_SELECTIONTYPE,0,0);
}

inline COLORREF CRichEditCtrl::SetBackgroundColor(BOOL bSysColor,COLORREF cr)
{
	return (COLORREF)::SendMessage(m_hWnd,EM_SETBKGNDCOLOR,bSysColor,(LPARAM)cr);
}

inline BOOL CRichEditCtrl::SetCharFormat(DWORD dwFlags,CHARFORMAT &cf)
{
	return (BOOL)::SendMessage(m_hWnd,EM_SETCHARFORMAT,dwFlags,(LPARAM)&cf);
}

inline BOOL CRichEditCtrl::SetCharFormat(DWORD dwFlags,CHARFORMAT2 &cf)
{
	return (BOOL)::SendMessage(m_hWnd,EM_SETCHARFORMAT,dwFlags,(LPARAM)&cf);
}
	
inline BOOL CRichEditCtrl::SetDefaultCharFormat(CHARFORMAT &cf)
{
	return (BOOL)::SendMessage(m_hWnd,EM_SETCHARFORMAT,0,(LPARAM)&cf);
}

inline BOOL CRichEditCtrl::SetDefaultCharFormat(CHARFORMAT2 &cf)
{
	return (BOOL)::SendMessage(m_hWnd,EM_SETCHARFORMAT,0,(LPARAM)&cf);
}

inline BOOL CRichEditCtrl::SetSelectionCharFormat(CHARFORMAT &cf)
{
	return (BOOL)::SendMessage(m_hWnd,EM_SETCHARFORMAT,SCF_SELECTION,(LPARAM)&cf);
}

inline BOOL CRichEditCtrl::SetSelectionCharFormat(CHARFORMAT2 &cf)
{
	return (BOOL)::SendMessage(m_hWnd,EM_SETCHARFORMAT,SCF_SELECTION,(LPARAM)&cf);
}

inline BOOL CRichEditCtrl::SetWordCharFormat(CHARFORMAT &cf)
{
	return (BOOL)::SendMessage(m_hWnd,EM_SETCHARFORMAT,SCF_SELECTION|SCF_WORD,(LPARAM)&cf);
}

inline DWORD CRichEditCtrl::SetEventMask(DWORD dwEventMask)
{
	return (BOOL)::SendMessage(m_hWnd,EM_SETEVENTMASK,0,(LPARAM)dwEventMask);
}

inline BOOL CRichEditCtrl::SetParaFormat(PARAFORMAT &pf)
{
	return (BOOL)::SendMessage(m_hWnd,EM_SETPARAFORMAT,0,(LPARAM)&pf);
}

inline BOOL CRichEditCtrl::SetTargetDevice(HDC hDC,long lLineWidth)
{
	return (BOOL)::SendMessage(m_hWnd,EM_SETTARGETDEVICE,(WPARAM)hDC,(LPARAM)lLineWidth);
}

inline int CRichEditCtrl::GetTextLength() const
{
	return (int)::SendMessage(m_hWnd,WM_GETTEXTLENGTH,0,0);
}

inline BOOL CRichEditCtrl::SetReadOnly(BOOL bReadOnly)
{
	return (BOOL)::SendMessage(m_hWnd,EM_SETREADONLY,(WPARAM)bReadOnly,0);
}

inline int CRichEditCtrl::GetFirstVisibleLine() const
{
	return (int)::SendMessage(m_hWnd,EM_GETFIRSTVISIBLELINE,0,0);
}

inline void CRichEditCtrl::EmptyUndoBuffer()
{
	::SendMessage(m_hWnd,EM_EMPTYUNDOBUFFER,0,0);
}

inline int CRichEditCtrl::LineIndex(int nLine) const
{
	return (int)::SendMessage(m_hWnd,EM_LINEINDEX,nLine,0);
}

inline int CRichEditCtrl::LineLength(int nLine) const
{
	return (int)::SendMessage(m_hWnd,EM_LINELENGTH,(WPARAM)nLine,0);
}

inline void CRichEditCtrl::LineScroll(int nLines,int nChars)
{
	::SendMessage(m_hWnd,EM_LINESCROLL,(WPARAM)nChars,(LPARAM)nLines);
}

inline void CRichEditCtrl::ReplaceSel(LPCTSTR lpszNewText,BOOL bCanUndo)
{
	::SendMessage(m_hWnd,EM_REPLACESEL,bCanUndo,(LPARAM)lpszNewText);
}

inline void CRichEditCtrl::SetRect(LPCRECT lpRect)
{
	::SendMessage(m_hWnd,EM_SETRECT,0,(LPARAM)lpRect);
}

inline BOOL CRichEditCtrl::DisplayBand(LPRECT pDisplayRect)
{
	return (BOOL)::SendMessage(m_hWnd,EM_DISPLAYBAND,0,(LPARAM)pDisplayRect);
}

inline LONG_PTR CRichEditCtrl::FindText(DWORD dwFlags,FINDTEXTEX* pFindText) const
{
	return (long)::SendMessage(m_hWnd,EM_FINDTEXTEX,dwFlags,(LPARAM)pFindText);
}

inline LONG_PTR CRichEditCtrl::FormatRange(FORMATRANGE* pfr,BOOL bDisplay)
{
	return ::SendMessage(m_hWnd,EM_FORMATRANGE,bDisplay,(LPARAM)pfr);
}

inline void CRichEditCtrl::HideSelection(BOOL bHide,BOOL bPerm)
{
	::SendMessage(m_hWnd,EM_HIDESELECTION,bHide,bPerm);
}

inline void CRichEditCtrl::PasteSpecial(UINT nClipFormat)
{
	::SendMessage(m_hWnd,EM_PASTESPECIAL,nClipFormat,0);
}

inline void CRichEditCtrl::RequestResize()
{
	::SendMessage(m_hWnd,EM_REQUESTRESIZE,0,0);
}

inline LONG_PTR CRichEditCtrl::StreamIn(int nFormat,EDITSTREAM &es)
{
	return ::SendMessage(m_hWnd,EM_STREAMIN,nFormat,(LPARAM)&es);
}

inline LONG_PTR CRichEditCtrl::StreamOut(int nFormat,EDITSTREAM &es)
{
	return ::SendMessage(m_hWnd,EM_STREAMOUT,nFormat,(LPARAM)&es);
}

inline BOOL CRichEditCtrl::Undo()
{
	return (BOOL)::SendMessage(m_hWnd,EM_UNDO,0,0);
}

inline BOOL CRichEditCtrl::Redo()
{
	return (BOOL)::SendMessage(m_hWnd,EM_REDO,0,0);
}

inline void CRichEditCtrl::Clear()
{
	::SendMessage(m_hWnd,WM_CLEAR,0,0);
}

inline void CRichEditCtrl::Copy()
{
	::SendMessage(m_hWnd,WM_COPY,0,0);
}

inline void CRichEditCtrl::Cut()
{
	::SendMessage(m_hWnd,WM_CUT,0,0);
}

inline void CRichEditCtrl::Paste()
{
	::SendMessage(m_hWnd,WM_PASTE,0,0);
}

inline BOOL CRichEditCtrl::SetTextEx(LPCSTR szText,DWORD dwFlags,UINT codepage)
{
	SETTEXTEX ste;
	ste.flags=dwFlags;
	ste.codepage=codepage;
	return (BOOL)::SendMessage(m_hWnd,EM_SETTEXTEX,(LPARAM)&ste,(LPARAM)szText);
}

inline BOOL CRichEditCtrl::SetTextEx(LPCWSTR szText,DWORD dwFlags,UINT codepage)
{
	SETTEXTEX ste;
	ste.flags=dwFlags;
	ste.codepage=codepage;
	return (BOOL)::SendMessage(m_hWnd,EM_SETTEXTEX,(LPARAM)&ste,(LPARAM)szText);
}

inline BOOL CRichEditCtrl::SetZoom(DWORD nNumerator,DWORD nDenominator)
{
	return (BOOL)::SendMessage(m_hWnd,EM_SETZOOM,nNumerator,nDenominator);
}

inline DWORD CRichEditCtrl::SetOptions(DWORD dwOptions,DWORD dwOperation)
{
	return (DWORD)::SendMessage(m_hWnd,EM_SETOPTIONS,dwOperation,(LPARAM)dwOptions);
}

inline DWORD CRichEditCtrl::GetOptions() const
{
	return (DWORD)::SendMessage(m_hWnd,EM_GETOPTIONS,0,0);
}

inline BOOL CRichEditCtrl::GetOleInterface(LPVOID* ppOleInterface) const
{
	return (BOOL)::SendMessage(m_hWnd,EM_GETOLEINTERFACE,0,(LPARAM)ppOleInterface);
}
	
inline IRichEditOle* CRichEditCtrl::GetOleInterface() const
{
	IRichEditOle* ioo;
	if (::SendMessage(m_hWnd,EM_GETOLEINTERFACE,0,(LPARAM)&ioo))
		return ioo;
	return NULL;
}

inline BOOL CRichEditCtrl::SetOleCallback(IRichEditOleCallback *pOleCallback)
{
	return (BOOL)::SendMessage(m_hWnd,EM_SETOLECALLBACK,0,(LPARAM)pOleCallback);
}


///////////////////////////
// Class CListBox

inline CListBox::CListBox()
:	CWnd(NULL)
{
}

inline CListBox::CListBox(HWND hWnd)
:	CWnd(hWnd)
{
}

inline UINT CListBox::ItemFromPoint(CPoint pt,BOOL& bOutside) const
{
	UINT nRet=(UINT)::SendMessage(m_hWnd,LB_ITEMFROMPOINT,0,MAKELPARAM(pt.x,pt.y));
	bOutside=HIWORD(nRet);
	return LOWORD(nRet);
}

inline BOOL CListBox::Create(DWORD dwStyle,const RECT* rect,HWND hParentWnd,UINT nID)
{
	return (m_hWnd=CreateWindowEx(0L,"LISTBOX",szEmpty,
		dwStyle,rect->left,rect->top,rect->right-rect->left,rect->bottom-rect->top,
		hParentWnd,(HMENU)(LONG_PTR)nID,GetInstanceHandle(),NULL))!=NULL;
}

inline int CListBox::GetCount() const
{
	return (int)::SendMessage(m_hWnd,LB_GETCOUNT,0,0);
}

inline int CListBox::GetHorizontalExtent() const
{
	return (int)::SendMessage(m_hWnd,LB_GETHORIZONTALEXTENT,0,0);
}

inline void CListBox::SetHorizontalExtent(int cxExtent)
{
	::SendMessage(m_hWnd,LB_SETHORIZONTALEXTENT,(WPARAM)cxExtent,0);
}

inline int CListBox::GetTopIndex() const
{
	return (int)::SendMessage(m_hWnd,LB_GETTOPINDEX,0,0);
}

inline int CListBox::SetTopIndex(int nIndex)
{
	return (int)::SendMessage(m_hWnd,LB_SETTOPINDEX,(WPARAM)nIndex,0);
}

inline LCID CListBox::GetLocale() const
{
	return (LCID)::SendMessage(m_hWnd,LB_GETLOCALE,0,0);
}

inline LCID CListBox::SetLocale(LCID nNewLocale)
{
	return (LCID)::SendMessage(m_hWnd,LB_SETLOCALE,(WPARAM)nNewLocale,0);
}
	
inline SIZE_T CListBox::InitStorage(int nItems,SIZE_T nBytes)
{
	return ::SendMessage(m_hWnd,LB_INITSTORAGE,(WPARAM)nItems,(LPARAM)nBytes);
}

inline int CListBox::GetCurSel() const
{
	return (int)::SendMessage(m_hWnd,LB_GETCURSEL,0,0);
}

inline int CListBox::SetCurSel(int nSelect)
{
	return (int)::SendMessage(m_hWnd,LB_SETCURSEL,(WPARAM)nSelect,0);
}

inline int CListBox::GetSel(int nIndex) const
{
	return (int)::SendMessage(m_hWnd,LB_GETSEL,(WPARAM)nIndex,0);
}

inline int CListBox::SetSel(int nIndex,BOOL bSelect)
{
	return (int)::SendMessage(m_hWnd,LB_SETSEL,(WPARAM)bSelect,(LPARAM)nIndex);
}

inline int CListBox::GetSelCount() const
{
	return (int)::SendMessage(m_hWnd,LB_GETSELCOUNT,0,0);
}

inline int CListBox::GetSelItems(int nMaxItems,LPINT rgIndex) const
{
	return (int)::SendMessage(m_hWnd,LB_GETSELITEMS,(WPARAM)nMaxItems,(LPARAM)rgIndex);
}

inline void CListBox::SetAnchorIndex(int nIndex)
{
	::SendMessage(m_hWnd,LB_SETANCHORINDEX,(WPARAM)nIndex,0);
}

inline int CListBox::GetAnchorIndex() const
{
	return (int)::SendMessage(m_hWnd,LB_SETANCHORINDEX,0,0);
}

inline DWORD_PTR CListBox::GetItemData(int nIndex) const
{
	return (DWORD_PTR)::SendMessage(m_hWnd,LB_GETITEMDATA,(WPARAM)nIndex,0);
}

inline int CListBox::SetItemData(int nIndex,DWORD_PTR dwItemData)
{
	return (int)::SendMessage(m_hWnd,LB_SETITEMDATA,(WPARAM)nIndex,(LPARAM)dwItemData);
}

inline int CListBox::GetItemRect(int nIndex,LPRECT lpRect) const
{
	return (int)::SendMessage(m_hWnd,LB_GETITEMRECT,(WPARAM)nIndex,(LPARAM)lpRect);
}

inline int CListBox::GetText(int nIndex,LPSTR lpszBuffer) const
{
	return (int)::SendMessage(m_hWnd,LB_GETTEXT,(WPARAM)nIndex,(LPARAM)lpszBuffer);
}

inline int CListBox::GetTextLen(int nIndex) const
{
	return (int)::SendMessage(m_hWnd,LB_GETTEXTLEN,(WPARAM)nIndex,0);
}

inline void CListBox::SetColumnWidth(int cxWidth)
{
	::SendMessage(m_hWnd,LB_SETCOLUMNWIDTH,(WPARAM)cxWidth,0);
}

inline BOOL CListBox::SetTabStops(int nTabStops,LPINT rgTabStops)
{
	return (BOOL)::SendMessage(m_hWnd,LB_SETTABSTOPS,(WPARAM)nTabStops,(LPARAM)rgTabStops);
}

inline void CListBox::SetTabStops()
{
	::SendMessage(m_hWnd,LB_SETTABSTOPS,0,NULL);
}

inline BOOL CListBox::SetTabStops(const int& cxEachStop)
{
	return (BOOL)::SendMessage(m_hWnd,LB_SETTABSTOPS,(WPARAM)1,(LPARAM)&cxEachStop);
}

inline int CListBox::SetItemHeight(int nIndex,UINT cyItemHeight)
{
	return (int)::SendMessage(m_hWnd,LB_SETITEMHEIGHT,(WPARAM)nIndex,MAKELPARAM(cyItemHeight,0));
}

inline int CListBox::GetItemHeight(int nIndex) const
{
	return (int)::SendMessage(m_hWnd,LB_GETITEMHEIGHT,(WPARAM)nIndex,0);
}

inline int CListBox::FindStringExact(int nIndexStart,LPCSTR lpszFind) const
{
	return (int)::SendMessage(m_hWnd,LB_FINDSTRINGEXACT,(WPARAM)nIndexStart,(LPARAM)lpszFind);
}

inline int CListBox::GetCaretIndex() const
{
	return (int)::SendMessage(m_hWnd,LB_GETCARETINDEX,0,0);
}

inline int CListBox::SetCaretIndex(int nIndex,BOOL bScroll)
{
	return (int)::SendMessage(m_hWnd,LB_SETCARETINDEX,nIndex,MAKELPARAM(bScroll,0));
}

inline int CListBox::AddString(LPCSTR lpszItem)
{
	return (int)::SendMessage(m_hWnd,LB_ADDSTRING,0,(LPARAM)lpszItem);
}



inline int CListBox::DeleteString(UINT nIndex)
{
	return (int)::SendMessage(m_hWnd,LB_DELETESTRING,nIndex,0);
}

inline int CListBox::InsertString(int nIndex,LPCSTR lpszItem)
{
	return (int)::SendMessage(m_hWnd,LB_INSERTSTRING,nIndex,(LPARAM)lpszItem);
}

inline void CListBox::ResetContent()
{
	::SendMessage(m_hWnd,LB_RESETCONTENT,0,0);
}

inline int CListBox::Dir(UINT attr,LPCSTR lpszWildCard)
{
	return (int)::SendMessage(m_hWnd,LB_DIR,attr,(LPARAM)lpszWildCard);
}

inline int CListBox::FindString(int nStartAfter,LPCSTR lpszItem) const
{
	return (int)::SendMessage(m_hWnd,LB_FINDSTRING,nStartAfter,(LPARAM)lpszItem);
}

inline int CListBox::SelectString(int nStartAfter,LPCSTR lpszItem)
{
	return (int)::SendMessage(m_hWnd,LB_SELECTSTRING,nStartAfter,(LPARAM)lpszItem);
}

inline int CListBox::SelItemRange(BOOL bSelect,int nFirstItem,int nLastItem)
{
	return (int)::SendMessage(m_hWnd,LB_SELITEMRANGE,bSelect,MAKELPARAM(nFirstItem,nLastItem));
}

#ifdef DEF_WCHAR
inline int CListBox::FindStringExact(int nIndexStart,LPCWSTR lpszFind) const
{
	if (IsUnicodeSystem())
		return (int)::SendMessageW(m_hWnd,LB_FINDSTRINGEXACT,(WPARAM)nIndexStart,(LPARAM)lpszFind);
	else
		return (int)::SendMessage(m_hWnd,LB_FINDSTRINGEXACT,(WPARAM)nIndexStart,(LPARAM)(LPCSTR)W2A(lpszFind));
}
inline int CListBox::AddString(LPCWSTR lpszItem)
{
	if (IsUnicodeSystem())
		return (int)::SendMessageW(m_hWnd,LB_ADDSTRING,0,(LPARAM)lpszItem);
	else
		return (int)::SendMessage(m_hWnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR)W2A(lpszItem));
}
inline int CListBox::InsertString(int nIndex,LPCWSTR lpszItem)
{
	if (IsUnicodeSystem())
		return (int)::SendMessageW(m_hWnd,LB_INSERTSTRING,nIndex,(LPARAM)lpszItem);
	else
		return (int)::SendMessage(m_hWnd,LB_INSERTSTRING,nIndex,(LPARAM)(LPCSTR)W2A(lpszItem));
}
inline int CListBox::FindString(int nStartAfter,LPCWSTR lpszItem) const
{
	if (IsUnicodeSystem())
		return (int)::SendMessageW(m_hWnd,LB_FINDSTRING,nStartAfter,(LPARAM)lpszItem);
	else
		return (int)::SendMessage(m_hWnd,LB_FINDSTRING,nStartAfter,(LPARAM)(LPCSTR)W2A(lpszItem));
}
inline int CListBox::Dir(UINT attr,LPCWSTR lpszWildCard)
{
	if (IsUnicodeSystem())
		return (int)::SendMessageW(m_hWnd,LB_DIR,attr,(LPARAM)lpszWildCard);
	else
		return (int)::SendMessage(m_hWnd,LB_DIR,attr,(LPARAM)(LPCSTR)W2A(lpszWildCard));
}
inline int CListBox::SelectString(int nStartAfter,LPCWSTR lpszItem)
{
	if (IsUnicodeSystem())
		return (int)::SendMessageW(m_hWnd,LB_SELECTSTRING,nStartAfter,(LPARAM)lpszItem);
	else
		return (int)::SendMessage(m_hWnd,LB_SELECTSTRING,nStartAfter,(LPARAM)(LPCSTR)W2A(lpszItem));
}
#endif

///////////////////////////
// Class CComboBox

inline CComboBox::CComboBox()
:	CWnd(NULL)
{
}

inline CComboBox::CComboBox(HWND hWnd)
:	CWnd(hWnd)
{
}

inline BOOL CComboBox::Create(DWORD dwStyle,const RECT* rect,HWND hParentWnd,UINT nID)
{
	return (m_hWnd=CreateWindowEx(0L,"COMBOBOX",szEmpty,
		dwStyle,rect->left,rect->top,rect->right-rect->left,rect->bottom-rect->top,
		hParentWnd,(HMENU)(ULONG_PTR)nID,GetInstanceHandle(),NULL))!=NULL;
}

inline int CComboBox::GetCount() const
{
	return (int)::SendMessage(m_hWnd,CB_GETCOUNT,0,0);
}		

inline int CComboBox::GetCurSel() const
{
	return (int)::SendMessage(m_hWnd,CB_GETCURSEL,0,0);
}

inline int CComboBox::SetCurSel(int nSelect)
{
	return (int)::SendMessage(m_hWnd,CB_SETCURSEL,(WPARAM)nSelect,0);
}

inline LCID CComboBox::GetLocale() const
{
	return (LCID)::SendMessage(m_hWnd,CB_GETLOCALE,0,0);
}

inline LCID CComboBox::SetLocale(LCID nNewLocale)
{
	return (LCID)::SendMessage(m_hWnd,CB_SETLOCALE,(WPARAM)nNewLocale,0);
}

inline int CComboBox::GetTopIndex() const
{
	return (int)::SendMessage(m_hWnd,CB_GETTOPINDEX,0,0);
}

inline int CComboBox::SetTopIndex(int nIndex)
{
	return (int)::SendMessage(m_hWnd,CB_SETTOPINDEX,(WPARAM)nIndex,0);
}

inline SIZE_T CComboBox::InitStorage(int nItems, SIZE_T nBytes)
{
	return ::SendMessage(m_hWnd,CB_INITSTORAGE,nItems,nBytes);
}

inline void CComboBox::SetHorizontalExtent(UINT nExtent)
{
	::SendMessage(m_hWnd,CB_SETHORIZONTALEXTENT,(WPARAM)nExtent,0);
}

inline UINT CComboBox::GetHorizontalExtent() const
{
	return (UINT) ::SendMessage(m_hWnd,CB_GETHORIZONTALEXTENT,0,0);
}

inline int CComboBox::SetDroppedWidth(UINT nWidth)
{
	return (int)::SendMessage(m_hWnd,CB_SETDROPPEDWIDTH,(WPARAM)nWidth,0);
}

inline int CComboBox::GetDroppedWidth() const
{
	return (int)::SendMessage(m_hWnd,CB_GETDROPPEDWIDTH,0,0);
}

inline DWORD CComboBox::GetEditSel() const
{
	return (DWORD)::SendMessage(m_hWnd,CB_GETEDITSEL,0,0);
}

inline BOOL CComboBox::LimitText(int nMaxChars)
{
	return (BOOL)::SendMessage(m_hWnd,CB_LIMITTEXT,nMaxChars,0);
}

inline BOOL CComboBox::SetEditSel(int nStartChar, int nEndChar)
{
	return (BOOL)::SendMessage(m_hWnd,CB_SETEDITSEL,0,MAKELPARAM(nStartChar,nEndChar));
}

inline DWORD_PTR CComboBox::GetItemData(int nIndex) const
{
	return (DWORD_PTR)::SendMessage(m_hWnd,CB_GETITEMDATA,nIndex,0);
}

inline int CComboBox::SetItemData(int nIndex, DWORD_PTR dwItemData)
{
	return (int)::SendMessage(m_hWnd,CB_SETITEMDATA,nIndex,dwItemData);
}


inline int CComboBox::GetLBText(int nIndex, LPSTR lpszText) const
{ 
	return (int)::SendMessage(m_hWnd,CB_GETLBTEXT,nIndex,(LPARAM)lpszText);
}



inline int CComboBox::GetLBTextLen(int nIndex) const
{
	return (int)::SendMessage(m_hWnd,CB_GETLBTEXTLEN,(WPARAM)nIndex,0);
}

inline int CComboBox::SetItemHeight(int nIndex, UINT cyItemHeight)
{
	return (int)::SendMessage(m_hWnd,CB_SETITEMHEIGHT,nIndex,cyItemHeight);
}

inline int CComboBox::GetItemHeight(int nIndex) const
{
	return (int)::SendMessage(m_hWnd,CB_SETITEMHEIGHT,nIndex,0);
}

inline int CComboBox::FindStringExact(int nIndexStart, LPCSTR lpszFind) const
{
	return (int)::SendMessage(m_hWnd,CB_FINDSTRINGEXACT,nIndexStart,(LPARAM)lpszFind);
}

#ifdef DEF_WCHAR
inline int CComboBox::FindStringExact(int nIndexStart, LPCWSTR lpszFind) const
{
	return (int)::SendMessage(m_hWnd,CB_FINDSTRINGEXACT,nIndexStart,(LPARAM)lpszFind);
}
#endif

inline int CComboBox::SetExtendedUI(BOOL bExtended)
{
	return (int)::SendMessage(m_hWnd,CB_SETEXTENDEDUI,bExtended,0);
}

inline BOOL CComboBox::GetExtendedUI() const
{
	return(BOOL) ::SendMessage(m_hWnd,CB_GETEXTENDEDUI,0,0);
}

inline void CComboBox::GetDroppedControlRect(LPRECT lprect) const
{
	::SendMessage(m_hWnd,CB_GETDROPPEDCONTROLRECT,0,(LPARAM)lprect);
}

inline BOOL CComboBox::GetDroppedState() const
{
	return (BOOL)::SendMessage(m_hWnd,CB_GETDROPPEDSTATE,0,0);
}

inline void CComboBox::ShowDropDown(BOOL bShowIt)
{
	::SendMessage(m_hWnd,CB_SHOWDROPDOWN,(WPARAM)bShowIt,0);
}

inline int CComboBox::AddString(LPCSTR lpszString)
{
	return (int)::SendMessageA(m_hWnd,CB_ADDSTRING,0,(LPARAM)lpszString);
}


inline int CComboBox::DeleteString(UINT nIndex)
{
	return (int)::SendMessage(m_hWnd,CB_DELETESTRING,nIndex,0);
}

inline int CComboBox::InsertString(int nIndex,LPCSTR lpszString)
{
	return (int)::SendMessageA(m_hWnd,CB_INSERTSTRING,nIndex,(LPARAM)lpszString);
}


inline void CComboBox::ResetContent()
{
	::SendMessage(m_hWnd,CB_RESETCONTENT,0,0);
}

inline int CComboBox::Dir(UINT attr, LPCSTR lpszWildCard)
{
	return (int)::SendMessageA(m_hWnd,CB_DIR,(WPARAM)attr,(LPARAM)lpszWildCard);
}



inline int CComboBox::FindString(int nStartAfter, LPCSTR lpszString) const
{
	return (int)::SendMessageA(m_hWnd,CB_FINDSTRING,(WPARAM)nStartAfter,(LPARAM)lpszString);
}



inline int CComboBox::SelectString(int nStartAfter, LPCSTR lpszString)
{
	return(int) ::SendMessageA(m_hWnd,CB_SELECTSTRING,(WPARAM)nStartAfter,(LPARAM)lpszString);
}

inline void CComboBox::Clear()
{
	::SendMessage(m_hWnd,WM_CLEAR,0,0);
}

inline void CComboBox::Copy()
{
	::SendMessage(m_hWnd,WM_COPY,0,0);
}

inline void CComboBox::Cut()
{
	::SendMessage(m_hWnd,WM_CUT,0,0);
}

inline void CComboBox::Paste()
{
	::SendMessage(m_hWnd,WM_PASTE,0,0);
}

///////////////////////////
// Class ScrollBar

inline CScrollBar::CScrollBar()
:	CWnd(NULL),m_nBar(SB_CTL)
{
}

inline CScrollBar::CScrollBar(HWND hWnd,int nBar)
:	CWnd(hWnd),m_nBar(nBar)
{
}
	
inline BOOL CScrollBar::Create(DWORD dwStyle,const RECT* rect,HWND hParentWnd,UINT nID)
{
	return (m_hWnd=CreateWindowEx(0L,"SCROLLBAR",szEmpty,
		dwStyle,rect->left,rect->top,rect->right-rect->left,rect->bottom-rect->top,
		hParentWnd,(HMENU)(LONG_PTR)nID,GetInstanceHandle(),NULL))!=NULL;
}

inline void CScrollBar::ShowScrollBar(BOOL bShow)
{
	::ShowScrollBar(m_hWnd,m_nBar,bShow);
}

inline BOOL CScrollBar::EnableScrollBar(UINT nArrowFlags)
{
	return ::EnableScrollBar(m_hWnd,m_nBar,nArrowFlags);
}

inline BOOL CScrollBar::SetScrollInfo(LPSCROLLINFO lpScrollInfo,BOOL bRedraw)
{
	return ::SetScrollInfo(m_hWnd,m_nBar,lpScrollInfo,bRedraw);
}

///////////////////////////
// Class C3DStaticCtrl

inline C3DStaticCtrl::C3DStaticCtrl()
:	CWnd(NULL)
{
}

inline C3DStaticCtrl::C3DStaticCtrl(HWND hWnd)
:	CWnd(hWnd)
{
}

inline BOOL C3DStaticCtrl::Create(LPCTSTR lpszCaption,DWORD dwStyle,const RECT* rect,HWND hParentWnd,UINT nID)
{
	return (m_hWnd=CreateWindowEx(0L,"HFC3DSTATIC",lpszCaption,dwStyle,
		rect->left,rect->top,rect->right-rect->left,rect->bottom-rect->top,
		hParentWnd,(HMENU)(LONG_PTR) nID,GetInstanceHandle(), 0L))!=NULL;
}

///////////////////////////
// Class C3DButtonCtrl

inline C3DButtonCtrl::C3DButtonCtrl()
:	CWnd(NULL)
{
}

inline C3DButtonCtrl::C3DButtonCtrl(HWND hWnd)
:	CWnd(hWnd)
{
}

inline BOOL C3DButtonCtrl::Create(LPCTSTR lpszCaption,DWORD dwStyle,const RECT* rect,HWND hParentWnd,UINT nID)
{
	return (m_hWnd=CreateWindowEx(0L,"HFC3DBUTTON",lpszCaption,dwStyle,
		rect->left,rect->top,rect->right-rect->left,rect->bottom-rect->top,
		hParentWnd,(HMENU)(LONG_PTR) nID,GetInstanceHandle(), 0L))!=NULL;
}

inline BOOL C3DButtonCtrl::SetBitmap(HBITMAP hBitmap)
{
	return (BOOL)::SendMessage(m_hWnd,B3DM_SETBITMAP,0,(LPARAM)hBitmap);
}

inline BOOL C3DButtonCtrl::SetHotBitmap(HBITMAP hBitmap)
{
	return (BOOL)::SendMessage(m_hWnd,B3DM_SETHOTBITMAP,0,(LPARAM)hBitmap);
}

inline BOOL C3DButtonCtrl::SetDisabledBitmap(HBITMAP hBitmap)
{
	return (BOOL)::SendMessage(m_hWnd,B3DM_SETDISABLEDBITMAP,0,(LPARAM)hBitmap);
}

inline HBITMAP C3DButtonCtrl::GetBitmap() const
{
	return (HBITMAP)::SendMessage(m_hWnd,B3DM_GETBITMAP,0,0);
}

inline HBITMAP C3DButtonCtrl::GetHotBitmap() const
{
	return (HBITMAP)::SendMessage(m_hWnd,B3DM_GETHOTBITMAP,0,0);
}

inline HBITMAP C3DButtonCtrl::GetDisabledBitmap() const
{
	return (HBITMAP)::SendMessage(m_hWnd,B3DM_GETDISABLEDBITMAP,0,0);
}

inline BOOL C3DButtonCtrl::SetColor(DWORD nFlags,COLORREF cr)
{
	return (BOOL)::SendMessage(m_hWnd,B3DM_SETCOLOR,nFlags,(LPARAM)cr);
}

inline COLORREF C3DButtonCtrl::GetColor(DWORD nFlags) const
{
	return (COLORREF)::SendMessage(m_hWnd,B3DM_GETCOLOR,nFlags,0);
}

inline BOOL C3DButtonCtrl::IsPressed() const
{
	return (BOOL)::SendMessage(m_hWnd,B3DM_ISPRESSED,0,0);
}

inline UINT C3DButtonCtrl::GetButtonStyle() const
{
	return (UINT)::SendMessage(m_hWnd,B3DM_GETSTYLE,0,0);
}

inline void C3DButtonCtrl::SetButtonStyle(UINT nStyle,BOOL bRedraw)
{
	::SendMessage(m_hWnd,B3DM_SETSTYLE,(WPARAM)nStyle,MAKELPARAM(bRedraw,0));
}

#endif
