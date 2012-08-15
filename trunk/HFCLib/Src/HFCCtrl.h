////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////


#ifndef HFCCTRL_H
#define HFCCTRL_H

#ifdef DEF_WINDOWS

// Classes for windows basic controls

class CButton : public CWnd
{
public:
	CButton();
	CButton(HWND hWnd);
	BOOL Create(LPCTSTR lpszCaption,DWORD dwStyle,const RECT* rect,HWND hParentWnd,UINT nID);

public:
	UINT GetState() const;
	void SetState(BOOL bHighlight);
	int GetCheck() const;
	void SetCheck(int nCheck);
	UINT GetButtonStyle() const;
	void SetButtonStyle(UINT nStyle,BOOL bRedraw=TRUE);

	HICON SetIcon(HICON hIcon);
	HICON GetIcon() const;
	HBITMAP SetBitmap(HBITMAP hBitmap);
	HBITMAP GetBitmap() const;
	HCURSOR SetCursor(HCURSOR hCursor);
	HCURSOR GetCursor();
};

class CEdit : public CWnd
{
public:
	CEdit();
	CEdit(HWND hWnd);
	BOOL Create(DWORD dwStyle,const RECT* rect,HWND hParentWnd,UINT nID);

public:
	void LimitText(int iLimit);
	void SetLimitText(int iLimit);
	void SetSel(int iStart,int iEnd);

	BOOL Undo();
	void Clear();
	void Copy();
	void Cut();
	void Paste();
};

class CRichEditCtrl : public CWnd
{
public:
	CRichEditCtrl();
	CRichEditCtrl(HWND hWnd);

	BOOL Create(DWORD dwStyle, const RECT* rect, HWND hParentWnd, UINT nID);
	BOOL CreateV2(DWORD dwStyle, const RECT* rect, HWND hParentWnd, UINT nID);
	
	BOOL CanUndo() const;
	BOOL CanRedo() const;
	UNDONAMEID GetUndoName() const;
	UNDONAMEID GetRedoName() const;

	BOOL Undo();
	BOOL Redo();
	void Clear();
	void Copy();
	void Cut();
	void Paste();

	int GetLineCount() const;
	BOOL GetModify() const;
	
	void SetModify(BOOL bModified=TRUE);
	void GetRect(LPRECT lpRect) const;
	void SetRect(LPCRECT lpRect);
	
	CPoint GetCharPos(long lChar) const;
	void SetOptions(WORD wOp, DWORD dwFlags);

	int GetLine(int nIndex,LPTSTR lpszBuffer) const;
	BOOL CanPaste(UINT nFormat=0) const;
	void GetSel(long& nStartChar,long& nEndChar) const;
	void GetSel(CHARRANGE &cr) const;
	void LimitText(long nChars=0);
	long LineFromChar(long nIndex) const;
	void SetSel(long nStartChar,long nEndChar);
	void SetSel(CHARRANGE &cr);
	DWORD GetDefaultCharFormat(CHARFORMAT &cf) const;
	DWORD GetDefaultCharFormat(CHARFORMAT2 &cf) const;
	DWORD GetSelectionCharFormat(CHARFORMAT &cf) const;
	DWORD GetSelectionCharFormat(CHARFORMAT2 &cf) const;
	long GetEventMask() const;
	int GetLimitText() const;
	DWORD GetParaFormat(PARAFORMAT &pf) const;
	DWORD GetParaFormat(PARAFORMAT2 &pf) const;
	long GetSelText(LPSTR lpBuf) const;
	CString GetSelText() const;
	WORD GetSelectionType() const;
	COLORREF SetBackgroundColor(BOOL bSysColor,COLORREF cr);
	BOOL SetCharFormat(DWORD dwFlags,CHARFORMAT &cf);
	BOOL SetCharFormat(DWORD dwFlags,CHARFORMAT2 &cf);
	BOOL SetDefaultCharFormat(CHARFORMAT &cf);
	BOOL SetDefaultCharFormat(CHARFORMAT2 &cf);
	BOOL SetSelectionCharFormat(CHARFORMAT &cf);
	BOOL SetSelectionCharFormat(CHARFORMAT2 &cf);
	BOOL SetWordCharFormat(CHARFORMAT &cf);
	BOOL SetWordCharFormat(CHARFORMAT2 &cf);
	DWORD SetEventMask(DWORD dwEventMask);
	BOOL SetParaFormat(PARAFORMAT &pf);
	BOOL SetParaFormat(PARAFORMAT2 &pf);
	BOOL SetTargetDevice(HDC hDC,long lLineWidth);
	
	int GetTextLength() const;
	BOOL SetReadOnly(BOOL bReadOnly = TRUE);
	int GetFirstVisibleLine() const;
	BOOL SetTextEx(LPCSTR szText,DWORD dwFlags=ST_DEFAULT,UINT codepage=CP_ACP);
	BOOL SetTextEx(LPCWSTR szText,DWORD dwFlags=ST_DEFAULT,UINT codepage=1200);
	BOOL SetZoom(DWORD nNumerator,DWORD nDenominator);

	void EmptyUndoBuffer();

	int LineIndex(int nLine=-1) const;
	int LineLength(int nLine=-1) const;
	void LineScroll(int nLines,int nChars=0);
	void ReplaceSel(LPCTSTR lpszNewText,BOOL bCanUndo=FALSE);
	
	BOOL DisplayBand(LPRECT pDisplayRect);
	LONG_PTR FindText(DWORD dwFlags,FINDTEXTEX* pFindText) const;
	LONG_PTR FormatRange(FORMATRANGE* pfr,BOOL bDisplay=TRUE);
	void HideSelection(BOOL bHide,BOOL bPerm=0);
	void PasteSpecial(UINT nClipFormat);
	void RequestResize();
	LONG_PTR StreamIn(int nFormat,EDITSTREAM &es);
	LONG_PTR StreamOut(int nFormat,EDITSTREAM &es);

	DWORD SetOptions(DWORD dwOptions,DWORD dwOperation=ECOOP_SET);
	DWORD GetOptions() const;

	BOOL GetOleInterface(LPVOID* ppOleInterface) const; // Release must call
	IRichEditOle* GetOleInterface() const; // Release must call
	BOOL SetOleCallback(IRichEditOleCallback *pOleCallback); // Release must call

	BOOL SetAlignmentForSelection(WORD wAlignment);
	BOOL SetLineSpacingRuleForSelection(BYTE bLineSpacingRule,LONG dyLineSpacing=0);
	BOOL SetEffectForSelection(DWORD dwEffects,DWORD dwMask);
	BOOL SetUnderlineTypeForSelection(BYTE bUnderlineType);
		
};


class CListBox : public CWnd
{
public:
	CListBox();
	CListBox(HWND hWnd);

	BOOL Create(DWORD dwStyle,const RECT* rect,HWND hParentWnd,UINT nID);
public:

	int GetCount() const;
	int GetHorizontalExtent() const;
	void SetHorizontalExtent(int cxExtent);
	int GetTopIndex() const;
	int SetTopIndex(int nIndex);
	LCID GetLocale() const;
	LCID SetLocale(LCID nNewLocale);
	SIZE_T InitStorage(int nItems,SIZE_T nBytes);
	UINT ItemFromPoint(CPoint pt,BOOL& bOutside) const;

	int GetCurSel() const;
	int SetCurSel(int nSelect);

	int GetSel(int nIndex) const;
	int SetSel(int nIndex,BOOL bSelect=TRUE);
	int GetSelCount() const;
	int GetSelItems(int nMaxItems,LPINT rgIndex) const;
	void SetAnchorIndex(int nIndex);
	int GetAnchorIndex() const;

	DWORD_PTR GetItemData(int nIndex) const;
	int SetItemData(int nIndex,DWORD_PTR dwItemData);
	int GetItemRect(int nIndex,LPRECT lpRect) const;
	int GetText(int nIndex,LPSTR lpszBuffer) const;
	int GetText(int nIndex,CString& rString) const;
	int GetTextLen(int nIndex) const;

	void SetColumnWidth(int cxWidth);
	BOOL SetTabStops(int nTabStops,LPINT rgTabStops);
	void SetTabStops();
	BOOL SetTabStops(const int& cxEachStop);

	int SetItemHeight(int nIndex,UINT cyItemHeight);
	int GetItemHeight(int nIndex) const;
	int FindStringExact(int nIndexStart,LPCSTR lpszFind) const;
	int GetCaretIndex() const;
	int SetCaretIndex(int nIndex,BOOL bScroll=TRUE);

	int AddString(LPCTSTR lpszItem);
	int DeleteString(UINT nIndex);
	int InsertString(int nIndex,LPCSTR lpszItem);
	void ResetContent();
	int Dir(UINT attr,LPCSTR lpszWildCard);

	int FindString(int nStartAfter,LPCSTR lpszItem) const;
	int SelectString(int nStartAfter,LPCSTR lpszItem);
	int SelItemRange(BOOL bSelect,int nFirstItem,int nLastItem);

#ifdef DEF_WCHAR
	int Dir(UINT attr,LPCWSTR lpszWildCard);
	int GetText(int nIndex,LPWSTR lpszBuffer) const;
	int GetText(int nIndex,CStringW& rString) const;
	int AddString(LPCWSTR lpszItem);
	int InsertString(int nIndex,LPCWSTR lpszItem);
	int FindString(int nStartAfter,LPCWSTR lpszItem) const;
	int FindStringExact(int nIndexStart,LPCWSTR lpszFind) const;
	int SelectString(int nStartAfter,LPCWSTR lpszItem);
#endif
};

class CComboBox : public CWnd
{
public:
	CComboBox();
	CComboBox(HWND hWnd);

	BOOL Create(DWORD dwStyle,const RECT* rect,HWND hParentWnd,UINT nID);

	int GetCount() const;
	int GetCurSel() const;
	int SetCurSel(int nSelect);
	LCID GetLocale() const;
	LCID SetLocale(LCID nNewLocale);
	int GetTopIndex() const;
	int SetTopIndex(int nIndex);
	SIZE_T InitStorage(int nItems, SIZE_T nBytes);
	void SetHorizontalExtent(UINT nExtent);
	UINT GetHorizontalExtent() const;
	int SetDroppedWidth(UINT nWidth);
	int GetDroppedWidth() const;

	DWORD GetEditSel() const;
	BOOL LimitText(int nMaxChars);
	BOOL SetEditSel(int nStartChar, int nEndChar);

	DWORD_PTR GetItemData(int nIndex) const;
	int SetItemData(int nIndex, DWORD_PTR dwItemData);
	int GetLBText(int nIndex, LPSTR lpszText) const;
	int GetLBText(int nIndex, CStringA& rString) const;
	int GetLBTextLen(int nIndex) const;

	int SetItemHeight(int nIndex, UINT cyItemHeight);
	int GetItemHeight(int nIndex) const;
	int FindStringExact(int nIndexStart, LPCSTR lpszFind) const;
	int SetExtendedUI(BOOL bExtended = TRUE);
	BOOL GetExtendedUI() const;
	void GetDroppedControlRect(LPRECT lprect) const;
	BOOL GetDroppedState() const;

	void ShowDropDown(BOOL bShowIt = TRUE);

	int AddString(LPCSTR lpszString);
	int DeleteString(UINT nIndex);
	int InsertString(int nIndex, LPCSTR lpszString);
	void ResetContent();
	int Dir(UINT attr, LPCSTR lpszWildCard);

	int FindString(int nStartAfter,LPCSTR lpszString) const;
	int SelectString(int nStartAfter,LPCSTR lpszString);

	void Clear();
	void Copy();
	void Cut();
	void Paste();

#ifdef DEF_WCHAR
	int GetLBText(int nIndex, LPWSTR lpszText) const;
	int GetLBText(int nIndex, CStringW& rString) const;
	int FindStringExact(int nIndexStart, LPCWSTR lpszFind) const;
	int AddString(LPCWSTR lpszString);
	int InsertString(int nIndex, LPCWSTR lpszString);
	int Dir(UINT attr, LPCWSTR lpszWildCard);
	int FindString(int nStartAfter,LPCWSTR lpszString) const;
	int SelectString(int nStartAfter,LPCWSTR lpszString);
#endif
};

class CScrollBar : public CWnd
{
public:
	CScrollBar();
	CScrollBar(HWND hWnd,int nBar=SB_CTL);
	
	BOOL Create(DWORD dwStyle,const RECT* rect,HWND hParentWnd,UINT nID);

	int GetScrollPos() const;
	int SetScrollPos(int nPos,BOOL bRedraw=TRUE);
	int SetScrollPage(UINT nPage,BOOL bRedraw=TRUE);
	void GetScrollRange(LPINT lpMinPos,LPINT lpMaxPos) const;
	void SetScrollRange(int nMinPos,int nMaxPos,BOOL bRedraw=TRUE);
	void ShowScrollBar(BOOL bShow=TRUE);

	BOOL EnableScrollBar(UINT nArrowFlags=ESB_ENABLE_BOTH);

	BOOL SetScrollInfo(LPSCROLLINFO lpScrollInfo,BOOL bRedraw=TRUE);
	BOOL SetScrollInfo(int nMin=0,int nMax=0,UINT nPage=1,int nPos=0,UINT fMask=SIF_ALL,BOOL bRedraw=TRUE);
	BOOL GetScrollInfo(LPSCROLLINFO lpScrollInfo,UINT nMask=SIF_ALL);
	int GetScrollLimit();
protected:
	int m_nBar;
};

class C3DStaticCtrl : public CWnd
{
public:
	C3DStaticCtrl();
	C3DStaticCtrl(HWND hWnd);
	BOOL Create(LPCTSTR lpszCaption,DWORD dwStyle,const RECT* rect,HWND hParentWnd,UINT nID);

public:
};

class C3DButtonCtrl : public CWnd
{
public:
	C3DButtonCtrl();
	C3DButtonCtrl(HWND hWnd);
	BOOL Create(LPCTSTR lpszCaption,DWORD dwStyle,const RECT* rect,HWND hParentWnd,UINT nID);

public:
	BOOL SetBitmap(HBITMAP hBitmap);
	BOOL SetHotBitmap(HBITMAP hBitmap);
	BOOL SetDisabledBitmap(HBITMAP hBitmap);
	HBITMAP GetBitmap() const;
	HBITMAP GetHotBitmap() const;
	HBITMAP GetDisabledBitmap() const;
	BOOL SetColor(DWORD nFlags,COLORREF cr);
	COLORREF GetColor(DWORD nFlags) const;

	BOOL IsPressed() const;
	UINT GetButtonStyle() const;
	void SetButtonStyle(UINT nStyle,BOOL bRedraw=TRUE);
};


#include "Controls.inl"

#endif
#endif
