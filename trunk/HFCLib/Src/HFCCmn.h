////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////


#ifndef HFCCOMMONCONTROLS_H
#define HFCCOMMONCONTROLS_H

#ifdef DEF_WINDOWS

class CCommonCtrl : public CWnd
{
public:
	CCommonCtrl(HWND hWnd=NULL);

public:
	BOOL SetBkColor(COLORREF bkColor);
	BOOL SetColorScheme(const COLORSCHEME* pScheme);
	BOOL GetColorScheme(COLORSCHEME* pScheme) const;
	HWND GetDropTarget() const;
	BOOL SetUnicodeFormat(BOOL nFormat);
	BOOL GetUnicodeFormat() const;
	UINT SetVersion(UINT nVersion);
	UINT GetVersion() const;
	BOOL SetNotifyWindow(HWND hWndParent);
};

#ifdef DEF_RESOURCES
class CImageList
{
public:
	CImageList();
	CImageList(HIMAGELIST hImagelist);
	~CImageList();

	HIMAGELIST m_hImageList;          
	operator HIMAGELIST() const;
	HIMAGELIST GetSafeHandle() const;

	void Attach(HIMAGELIST hImagelist);

	BOOL Create(int cx, int cy, UINT nFlags, int nInitial, int nGrow);
	BOOL Create(UINT nBitmapID, int cx, int nGrow, COLORREF crMask);
	BOOL Create(LPCTSTR lpszBitmapID, int cx, int nGrow, COLORREF crMask);
	BOOL Create(HIMAGELIST imagelist1, int nImage1, HIMAGELIST imagelist2,
		int nImage2, int dx, int dy);
	BOOL Create(LPCTSTR lpbmp,int cx,int cGrow,COLORREF crMask,UINT uFlags,BOOL nOem);
	BOOL Create(UINT nBitmapID,int cx,int cGrow,COLORREF crMask,UINT uFlags,BOOL nOem);
   
	BOOL DeleteImageList();

	int GetImageCount() const;
	BOOL SetImageCount(UINT uNewCount);
	
	COLORREF SetBkColor(COLORREF cr);
	COLORREF GetBkColor() const;
	
	BOOL GetIconSize(int *cx,int *cy);
	BOOL SetIconSize(int cx,int cy);
	BOOL GetImageInfo(int nImage, IMAGEINFO* pImageInfo) const;
	HIMAGELIST Merge(int i1,HIMAGELIST himl2,int i2,int dx,int dy);
	BOOL Duplicate(HIMAGELIST himl);

	int Add(HBITMAP hbmImage, HBITMAP hbmMask);
	int Add(HBITMAP hbmImage, COLORREF crMask);
	BOOL Remove(int nImage);
	BOOL RemoveAll();
	BOOL Replace(int nImage, HBITMAP hbmImage, HBITMAP hbmMask);
	int  Add(HICON hIcon);
	int  Replace(int nImage, HICON hIcon);
	HICON ExtractIcon(int nImage);
	BOOL Draw(HDC hDC, int nImage,POINT pt, UINT nStyle);
	BOOL DrawEx(HDC hDC,int nImage,int x,int y,int dx,int dy,COLORREF rgbBk,COLORREF rgbFg,UINT fStyle);
	static BOOL DrawIndirect(IMAGELISTDRAWPARAMS* pimldp);

	HICON GetIcon(int i,UINT flags);	

	BOOL SetOverlayImage(int nImage, int nOverlay);

	BOOL BeginDrag(int nImage, POINT ptHotSpot);
	static void PASCAL EndDrag();
	static BOOL PASCAL DragMove(POINT pt);
	BOOL SetDragCursorImage(int nDrag, POINT ptHotSpot);
	static BOOL PASCAL DragShowNolock(BOOL bShow);
	static HIMAGELIST PASCAL GetDragImage(LPPOINT lpPoint, LPPOINT lpPointHotSpot);
	static BOOL PASCAL DragEnter(HWND hWndLock, POINT point);
	static BOOL PASCAL DragLeave(HWND hWndLock);
	
	BOOL Copy(int iDst,HIMAGELIST himlSrc,int iSrc,UINT uFlags);
	BOOL Read(LPSTREAM pstm);
	BOOL Write(LPSTREAM pstm) const;

	BOOL SetFlags(UINT flags);
};
#endif


class CStatusBarCtrl : public CCommonCtrl
{
public:
	CStatusBarCtrl();
	CStatusBarCtrl(HWND hWnd);
	BOOL Create(DWORD dwStyle,const RECT* rect,HWND hWndParent,UINT nID);

public:
	BOOL SetText(LPCSTR lpszText,int nPane,int nType);
	int GetText(CString& sText,int nPane,int* pType=NULL) const;
	int GetText(LPSTR lpszText,int nPane,int* pType=NULL) const;
	int GetTextLength(int nPane,int* pType=NULL) const;
	BOOL SetParts(int nParts,int* pWidths);
	int GetParts(int nParts,int* pParts) const;
	BOOL GetBorders(int* pBorders) const;
	void SetMinHeight(int nMin);
	BOOL SetSimple(BOOL bSimple=TRUE);
	BOOL IsSimple() const;
	BOOL GetRect(int nPane,LPRECT lpRect) const;
	BOOL SetIcon(int nPane,HICON hIcon);
	HICON GetIcon(int nPane) const;
	BOOL SetTipText(int n,LPCSTR szText);
	int GetTipText(int n,LPSTR szText,int nBufLen) const;
	BOOL SetUnicodeFormat(int nFormat);
	BOOL GetUnicodeFormat() const;

#ifdef DEF_WCHAR
	BOOL SetText(LPCWSTR lpszText,int nPane,int nType);
	int GetText(CStringW& sText,int nPane,int* pType=NULL) const;
	int GetText(LPWSTR lpszText,int nPane,int* pType=NULL) const;
	BOOL SetTipText(int n,LPCWSTR szText);
	int GetTipText(int n,LPWSTR szText,int nBufLen) const;
#endif

};

class CToolTipCtrl : public CCommonCtrl
{
public:
	enum Duration {
		automatic=TTDT_AUTOMATIC,
		reshow=TTDT_RESHOW,
		autoPop=TTDT_AUTOPOP,
		initial=TTDT_INITIAL 
	};

	CToolTipCtrl();
	CToolTipCtrl(HWND hWnd);
	
	BOOL Create(HWND hParentWnd,DWORD dwStyle=WS_POPUP|TTS_NOPREFIX|TTS_ALWAYSTIP);

	void NewToolRect(LPTOOLINFO lpToolInfo);
	void GetText(CString& str,HWND hWnd,UINT nIDTool=0) const;
	BOOL GetToolInfo(LPTOOLINFO lpToolInfo,HWND hWnd,UINT nIDTool=0) const;
	void SetToolInfo(LPTOOLINFO lpToolInfo);
	void SetToolRect(HWND hWnd,UINT nIDTool,LPCRECT lpRect);
	
	

	int GetToolCount() const;
	BOOL GetCurrentTool(LPTOOLINFO lpToolInfo) const;

	void Activate(BOOL bActivate);
#ifdef DEF_RESOURCES
	BOOL AddTool(HWND hWnd,UINT nIDText,LPCRECT lpRectTool=NULL,UINT nIDTool=0,LPARAM lParam=0);
#endif
	BOOL AddTool(HWND hWnd,LPCSTR lpszText=LPSTR_TEXTCALLBACK,LPCRECT lpRectTool=NULL,UINT nIDTool=0,LPARAM lParam=0);
	BOOL AddTool(LPTOOLINFO lpToolInfo);
	BOOL AdjustRect(LPRECT lpRect,BOOL fLarger);
	
	void DelTool(HWND hWnd,UINT nIDTool=0);
	void DelTool(HWND hWnd,HWND hToolWnd);
	void DelTool(LPTOOLINFO lpToolInfo);

	BOOL HitTest(HWND hWnd,LPPOINT pt,LPTOOLINFO lpToolInfo) const;
	void RelayEvent(LPMSG lpMsg);
	void SetDelayTime(UINT nDelay,Duration nDuration=automatic);
	
	void GetMargin(LPRECT lprc) const;
	void SetMargin(LPCRECT lprc);
	int GetMaxTipWidth() const;
	int SetMaxTipWidth(int iWidth);

	BOOL EnumTools(UINT iTool,LPTOOLINFO lpToolInfo) const;
	void Pop();

	void UpdateTipText(LPCSTR lpszText,HWND hWnd,UINT nIDTool=0);
	
#ifdef DEF_RESOURCES
	void UpdateTipText(UINT nIDText,HWND hWnd,UINT nIDTool=0);
#endif

public:
	BOOL DestroyToolTipCtrl();

	BOOL TrackActivate(BOOL bStart,LPTOOLINFO lpToolInfo);
	BOOL TrackPosition(int xPos,int yPos);

#ifdef DEF_WCHAR
	void NewToolRect(LPTOOLINFOW lpToolInfo);
	void GetText(CStringW& str,HWND hWnd,UINT nIDTool=0) const;
	BOOL GetToolInfo(LPTOOLINFOW lpToolInfo,HWND hWnd,UINT nIDTool=0) const;
	void SetToolInfo(LPTOOLINFOW lpToolInfo);
	BOOL GetCurrentTool(LPTOOLINFOW lpToolInfo) const;
	BOOL AddTool(HWND hWnd,LPCWSTR lpszText,LPCRECT lpRectTool=NULL,UINT nIDTool=0,LPARAM lParam=0);
	BOOL AddTool(LPTOOLINFOW lpToolInfo);
	void UpdateTipText(LPCWSTR lpszText,HWND hWnd,UINT nIDTool=0);
	BOOL EnumTools(UINT iTool,LPTOOLINFOW lpToolInfo) const;
	void DelTool(LPTOOLINFOW lpToolInfo);
	
#endif

protected:
	friend class CTargetWnd;
	friend class CToolBarCtrl;
};

class CToolBarCtrl : public CCommonCtrl
{
public:
	CToolBarCtrl();
	CToolBarCtrl(HWND hWnd);
	BOOL Create(DWORD dwStyle,const RECT* rect,HWND hWndParent,UINT nID);

public:
	BOOL IsButtonEnabled(int nID) const;
	BOOL IsButtonChecked(int nID) const;
	BOOL IsButtonPressed(int nID) const;
	BOOL IsButtonHidden(int nID) const;
	BOOL IsButtonIndeterminate(int nID) const;
	BOOL SetState(int nID,UINT nState);
	int GetState(int nID) const;
	BOOL GetButton(int nIndex,LPTBBUTTON lpButton) const;
	int GetButtonCount() const;
	BOOL GetRect(int nID,LPRECT lpRect) const;
	BOOL GetItemRect(int nIndex,LPRECT lpRect) const;
	void SetButtonStructSize(int nSize);
	BOOL SetButtonSize(LPSIZE size);
	BOOL SetBitmapSize(LPSIZE size);
	HWND GetToolTips() const;
	void SetToolTips(HWND hTip);
	void SetRows(int nRows,BOOL bLarger,LPRECT lpRect);
	int GetRows() const;
	BOOL SetCmdID(int nIndex,UINT nID);
	UINT GetBitmapFlags() const;
	HWND SetParent(HWND hParentNew);
public:
	BOOL EnableButton(int nID,BOOL bEnable=TRUE);
	BOOL CheckButton(int nID,BOOL bCheck=TRUE);
	BOOL PressButton(int nID,BOOL bPress=TRUE);
	BOOL HideButton(int nID,BOOL bHide=TRUE);
	BOOL Indeterminate(int nID,BOOL bIndeterminate=TRUE);
#ifdef DEF_RESOURCES
	int AddBitmap(int nNumButtons,UINT nBitmapID);
#endif
	int AddBitmap(int nNumButtons,HBITMAP hBitmap);
	BOOL AddButtons(int nNumButtons,LPTBBUTTON lpButtons);
	BOOL InsertButton(int nIndex,LPTBBUTTON lpButton);
	BOOL DeleteButton(int nIndex);
	UINT CommandToIndex(UINT nID) const;
	void SaveState(HKEY hKeyRoot,LPCTSTR lpszSubKey,LPCTSTR lpszValueName);
	void RestoreState(HKEY hKeyRoot,LPCTSTR lpszSubKey,LPCTSTR lpszValueName);
	
	BOOL GetMaxSize(SIZE& rSize) const; 
	HRESULT GetObject(REFIID rID,void** ppvObject) const;

	DWORD GetPadding() const;
	BOOL GetPadding(WORD& cx,WORD& cy) const;
	void SetPadding(WORD cx,WORD cy);


	void Customize();
#ifdef DEF_RESOURCES
	int AddString(UINT nStringID);
#endif
	int AddStrings(LPCTSTR lpszStrings);
	void AutoSize();

	BOOL SetIndent(int nIndent);
	HIMAGELIST GetImageList() const;
	HIMAGELIST SetImageList(HIMAGELIST hImageList);
	HIMAGELIST GetHotImageList() const;
	HIMAGELIST SetHotImageList(HIMAGELIST hImageList);
	HIMAGELIST GetDisabledImageList() const;
	HIMAGELIST SetDisabledImageList(HIMAGELIST hImageList);
};

class CRebarCtrl : public CCommonCtrl
{
public:
	CRebarCtrl();
	CRebarCtrl(HWND hWnd);
	BOOL Create(DWORD dwStyle,const RECT* rect,HWND hParentWnd,UINT nID);
	BOOL CreateEx(DWORD dwStyle,const RECT* rect,HWND hParentWnd,UINT nID,DWORD dwExStyle);

public:
	int InsertBand(int nBand,const REBARBANDINFO* rbi);
	int DeleteBand(int nBand);
	
	int GetBandInfo(int nBand,REBARBANDINFO* rbi) const;
	int SetBandInfo(int nBand,const REBARBANDINFO* rbi);
	
	int GetBarInfo(REBARINFO* pri) const;
	int SetBarInfo(const REBARINFO* pri);
	
	int GetBandCount() const;
	int GetRowCount() const;
	int GetBarHeight() const;
	int GetRowHeight(int nRow) const;

	void GetBandBorders(int nBand,RECT& rc) const;
	//void GetBandMargins(PMARGINS margins) const;


	COLORREF GetBackColor() const;
	COLORREF SetBackColor(COLORREF col);

	BOOL GetColorScheme(LPCOLORSCHEME lpcs) const;
	void SetColorScheme(const LPCOLORSCHEME lpcs);
	
	int IdToIndex(UINT nBandID) const;
	void MaximizeBand(int nBand,BOOL bIdeal=FALSE);
	void MinimizeBand(int nBand);

	void MoveBand(int nFrom,int nTo);
	BOOL ShowBand(int nBand,BOOL bShow);

	BOOL SizeToRect(RECT& rc);
	BOOL GetRect(int nBand,RECT& rc);

	
	IDropTarget* GetDropTarget() const;
	void BeginDrag(int nBand,WORD wCorX=WORD(-1),WORD wCorY=WORD(-1));
	void DrawMove(WORD wCorX=WORD(-1),WORD wCorY=WORD(-1));
	void EndDrag();

	HWND GetTooltips() const;
	void SetTooltips(HWND hwnd);

	void SetWindowTheme(LPWSTR pwStr);

};

class CProgressCtrl : public CCommonCtrl
{
public:
	CProgressCtrl();
	CProgressCtrl(HWND hWnd);
	BOOL Create(DWORD dwStyle,const RECT* rect,HWND hParentWnd,UINT nID);

public:
	void SetRange(int nLower, int nUpper);
	int SetPos(int nPos);
	int OffsetPos(int nPos);
	int SetStep(int nStep);

	int StepIt();

	void SetBarColor(COLORREF bColor);
};

class CSpinButtonCtrl : public CCommonCtrl
{
public:
	CSpinButtonCtrl();
	CSpinButtonCtrl(HWND hWnd);
	BOOL Create(DWORD dwStyle,const RECT* rect,HWND hParentWnd,UINT nID);

public:
	BOOL SetAccel(int nAccel,UDACCEL* pAccel);
	UINT GetAccel(int nAccel,UDACCEL* pAccel) const;
	int SetBase(int nBase);
	UINT GetBase() const;
	HWND SetBuddy(HWND hWndBuddy);
	HWND GetBuddy() const;
	int SetPos(int nPos);
	int GetPos() const;
	void SetRange(int nLower,int nUpper);
	DWORD GetRange() const;
	void GetRange(int &lower,int& upper) const;
};

class CTabCtrl : public CCommonCtrl
{
public:
	CTabCtrl();
	CTabCtrl(HWND hWnd);
	
	BOOL Create(DWORD dwStyle,const RECT* rect,HWND hParentWnd,UINT nID);

	HIMAGELIST GetImageList() const;
	HIMAGELIST SetImageList(HIMAGELIST hImageList);
	int GetItemCount() const;
	BOOL GetItem(int nItem,TC_ITEM* pTabCtrlItem) const;
	BOOL SetItem(int nItem,TC_ITEM* pTabCtrlItem);
	BOOL GetItemRect(int nItem,LPRECT lpRect) const;
	int GetCurSel() const;
	int SetCurSel(int nItem);
	CSize SetItemSize(CSize size);
	void SetPadding(CSize size);
	int GetRowCount() const;
	HWND GetTooltips() const;
	void SetTooltips(HWND hWndTip);
	int GetCurFocus() const;

	BOOL InsertItem(int nItem,TC_ITEM* pTabCtrlItem);
	BOOL DeleteItem(int nItem);
	BOOL DeleteAllItems();
	void AdjustRect(BOOL bLarger,LPRECT lpRect);
	void RemoveImage(int nImage);
	int HitTest(TC_HITTESTINFO* pHitTestInfo) const;

#ifdef DEF_WCHAR
	BOOL InsertItem(int nItem,TC_ITEMW* pTabCtrlItem);
	BOOL GetItem(int nItem,TC_ITEMW* pTabCtrlItem) const;
	BOOL SetItem(int nItem,TC_ITEMW* pTabCtrlItem);
	
#endif
};

class CHeaderCtrl : public CCommonCtrl
{
public:
	CHeaderCtrl();
	CHeaderCtrl(HWND hWnd);
	
	BOOL Create(DWORD dwStyle, const RECT* rect, HWND hParentWnd, UINT nID);

	int GetItemCount() const;
	BOOL GetItem(int nPos, HD_ITEM* pHeaderItem) const;
	BOOL SetItem(int nPos, HD_ITEM* pHeaderItem);

	int InsertItem(int nPos, HD_ITEM* phdi);
	BOOL DeleteItem(int nPos);
	BOOL Layout(HD_LAYOUT* pHeaderLayout);

//Extendet
	BOOL GetItemRect(int iItem,LPRECT lprc) const;
	HIMAGELIST SetImageList(HIMAGELIST hIml);
	HIMAGELIST GetImageList() const;

	int OrderToIndex(int iItem);
	HIMAGELIST CreateDragImage(int iItem);
	
	BOOL GetOrderArray(int iCount,int* lpi) const;
	BOOL SetOrderArray(int iCount,int* lpi);

	int SetHotDivider(int fPos,DWORD dw);
};

class CListCtrl : public CCommonCtrl
{
public:
	CListCtrl();
	CListCtrl(HWND hWnd);
	
	BOOL Create(DWORD dwStyle,const RECT* rect,HWND hParentWnd,UINT nID);

	COLORREF GetBkColor() const;
	BOOL SetBkColor(COLORREF cr);
	HIMAGELIST GetImageList(int nImageList) const;
	HIMAGELIST SetImageList(HIMAGELIST hImageList,int nImageListType);
	int GetItemCount() const;
	BOOL GetItem(LV_ITEM* pItem) const;
	BOOL SetItem(const LV_ITEM* pItem);
	BOOL SetItem(int nItem, int nSubItem, UINT nMask, LPCSTR lpszItem,
		int nImage, UINT nState, UINT nStateMask, LPARAM lParam);
	UINT GetCallbackMask() const;
	BOOL SetCallbackMask(UINT nMask);
	int GetNextItem(int nItem, int nFlags) const;
	BOOL GetItemRect(int nItem, LPRECT lpRect, UINT nCode) const;
	BOOL SetItemPosition(int nItem, POINT pt);
	BOOL GetItemPosition(int nItem, LPPOINT lpPoint) const;
	int GetStringWidth(LPCSTR lpsz) const;
	HWND GetEditControl() const;
	HWND GetHeader() const;
	
	BOOL GetColumn(int nCol, LV_COLUMN* pColumn) const;
	BOOL SetColumn(int nCol, const LV_COLUMN* pColumn);
	int GetColumnWidth(int nCol) const;
	BOOL SetColumnWidth(int nCol, int cx);
	BOOL GetViewRect(LPRECT lpRect) const;
	COLORREF GetTextColor() const;
	BOOL SetTextColor(COLORREF cr);
	COLORREF GetTextBkColor() const;
	BOOL SetTextBkColor(COLORREF cr);
	int GetTopIndex() const;
	int GetCountPerPage() const;
	BOOL GetOrigin(LPPOINT lpPoint) const;
	BOOL SetItemState(int nItem, LV_ITEM* pItem);
	BOOL SetItemState(int nItem, UINT nState, UINT nMask);
	UINT GetItemState(int nItem, UINT nMask) const;
	UINT GetCheckState(int nItem) const;
	void SetCheckState(int nItem,UINT nState);

	BOOL GetItemText(CString& sText,int nItem, int nSubItem) const;
	int GetItemText(int nItem, int nSubItem, LPSTR lpszText, int nLen) const;
	BOOL SetItemText(int nItem, int nSubItem, LPCSTR lpszText);
	void SetItemCount(int nItems,DWORD dwFlags=0);
	BOOL SetItemData(int nItem, DWORD_PTR dwData);
	DWORD_PTR GetItemData(int nItem) const;
	UINT GetSelectedCount() const;

	int InsertItem(const LV_ITEM* pItem);
	int InsertItem(int nItem, LPCSTR lpszItem);
	int InsertItem(int nItem, LPCSTR lpszItem, int nImage);
	int InsertItem(UINT nMask, int nItem, LPCSTR lpszItem, UINT nState,
		UINT nStateMask,int nImage, LPARAM lParam);
	
	BOOL DeleteItem(int nItem);
	BOOL DeleteAllItems();
	int FindItem(LV_FINDINFO* pFindInfo, int nStart = -1) const;
	int HitTest(LV_HITTESTINFO* pHitTestInfo) const;
	int HitTest(POINT pt, UINT* pFlags = NULL) const;
	BOOL EnsureVisible(int nItem, BOOL bPartialOK);
	BOOL Scroll(SIZE size);
	BOOL RedrawItems(int nFirst, int nLast);
	BOOL Arrange(UINT nCode);
	HWND EditLabel(int nItem);
	int InsertColumn(int nCol, const LV_COLUMN* pColumn);
	int InsertColumn(int nCol, LPCSTR lpszColumnHeading,
		int nFormat = LVCFMT_LEFT, int nWidth = -1, int nSubItem = -1);
	inline int GetColumnCount() const;
	BOOL DeleteColumn(int nCol);
	HIMAGELIST CreateDragImage(int nItem, LPPOINT lpPoint);
	BOOL Update(int nItem);
	BOOL SortItems(PFNLVCOMPARE pfnCompare, DWORD dwData);
	
//Extendet
	DWORD GetItemSpacing(int fSmall) const;
	BOOL GetISearchString(LPSTR lpsz) const;
	DWORD SetIconSpacing(int cx, int cy);

	DWORD SetExtendedListViewStyle(DWORD dwMask,DWORD dwStyle);
	DWORD GetExtendedListViewStyle() const;

	BOOL GetSubItemRect(int iItem,int  iSubItem, LONG code,LPRECT prc) const;
	int SubItemHitTest(LPLVHITTESTINFO plvhti);
	BOOL SetColumnOrderArray(int iCount,LPINT pi);
	BOOL GetColumnOrderArray(int iCount,LPINT pi) const;
	int SetHotItem(int iItem);
	int GetHotItem();
	HCURSOR SetHotCursor(HCURSOR hCursor);
	HCURSOR GetHotCursor() const;
	DWORD ApproximateViewRect(int iWidth,int iHeight,int iCount);

	BOOL SetWorkAreas(int nAreas,const LPRECT prc);
	BOOL GetWorkAreas(int nAreas,LPRECT prc) const;
	BOOL GetNumberOfWorkAreas(int pnWorkAreas);
	int GetSelectionMark() const;
	int SetSelectionMark(int nMark);
	DWORD SetHoverTime(DWORD dwHoverTimeMs);
	DWORD GetHoverTime() const;
	HWND SetToolTips(HWND hTooltips);
	HWND GetToolTips() const;
	BOOL SortItemsEx(PFNLVCOMPARE pfnCompare,LPARAM lParam);
	BOOL SetBkImage(const LVBKIMAGE* plvbki);
    BOOL GetBkImage(LVBKIMAGE* plvbki) const;
    
	BOOL LoadColumnsState(HKEY hRootKey,LPCSTR lpKey,LPCSTR lpSubKey);
	BOOL SaveColumnsState(HKEY hRootKey,LPCSTR lpKey,LPCSTR lpSubKey) const;

	DWORD GetUnicodeFormat() const;
	void SetUnicodeFormat(DWORD fUnicode);
	
	void SetTileWidth(const int* cpWidth);
	UINT GetSelectedColumn() const;
	void SetSelectedColumn(UINT iCol);
	DWORD SetView(DWORD iView);
	DWORD GetView() const;
	COLORREF GetOutlineColor() const;
	COLORREF SetOutlineColor(COLORREF crOutline);
	void CancelEditLabel();
	
	// Group support (requires version 6 of common controls dll
	int EnableGroupView(BOOL fEnable);
	int IsGroupViewEnabled() const;
	
	int InsertGroup(int nIndex,const LVGROUP* pGroup);
	void InsertGroupSorted(const LVINSERTGROUPSORTED* pGroupSorted);

	int GetGroupInfo(int iID,LVGROUP* pGroup);
	void GetGroupMetrics(LVGROUPMETRICS* pGroupMetrics);
	int SetGroupInfo(int iID,const LVGROUP* pGroup);
	void SetGroupMetrics(const LVGROUPMETRICS* pGroupMetrics);

	void RemoveAllGroups();
	int RemoveGroup(int iID);

	int SortGroups(PFNLVGROUPCOMPARE pfnGroupCompare,void* plv);
    
#ifdef DEF_WCHAR
	BOOL GetColumn(int nCol, LV_COLUMNW* pColumn) const;
	BOOL SetColumn(int nCol, const LV_COLUMNW* pColumn);
	int InsertColumn(int nCol, const LV_COLUMNW* pColumn);
	int InsertColumn(int nCol, LPCWSTR lpszColumnHeading,
		int nFormat = LVCFMT_LEFT, int nWidth = -1, int nSubItem = -1);
	
	BOOL GetItem(LV_ITEMW* pItem) const;
	BOOL SetItem(const LV_ITEMW* pItem);
	BOOL SetItem(int nItem, int nSubItem, UINT nMask, LPCWSTR lpszItem,
		int nImage, UINT nState, UINT nStateMask, LPARAM lParam);
	int InsertItem(const LV_ITEMW* pItem);
	int InsertItem(int nItem, LPCWSTR lpszItem);
	int InsertItem(int nItem, LPCWSTR lpszItem, int nImage);
	int InsertItem(UINT nMask, int nItem, LPCWSTR lpszItem, UINT nState,
		UINT nStateMask,int nImage, LPARAM lParam);
	

	int GetStringWidth(LPCWSTR lpsz) const;
	BOOL GetItemText(CStringW& sText,int nItem, int nSubItem) const;
	int GetItemText(int nItem, int nSubItem, LPWSTR lpszText, int nLen) const;
	BOOL SetItemText(int nItem, int nSubItem, LPCWSTR lpszText);
#endif

protected:
	void RemoveImageList(int nImageList);
};

class CTreeCtrl : public CCommonCtrl
{
public:
	CTreeCtrl();
	CTreeCtrl(HWND hWnd);

	BOOL Create(DWORD dwStyle,const RECT* rect,HWND hParentWnd,UINT nID);
	
	BOOL GetItemRect(HTREEITEM hItem,LPRECT lpRect,BOOL bTextOnly) const;
	UINT GetCount() const;
	UINT GetIndent() const;
	void SetIndent(UINT nIndent);
	HIMAGELIST GetImageList(UINT nImageList) const;
	HIMAGELIST SetImageList(HIMAGELIST hImageList,int nImageListType);
	HTREEITEM GetNextItem(HTREEITEM hItem,UINT nCode) const;
	HTREEITEM GetChildItem(HTREEITEM hItem) const;
	HTREEITEM GetNextSiblingItem(HTREEITEM hItem) const;
	HTREEITEM GetPrevSiblingItem(HTREEITEM hItem) const;
	HTREEITEM GetParentItem(HTREEITEM hItem) const;
	HTREEITEM GetFirstVisibleItem() const;
	HTREEITEM GetNextVisibleItem(HTREEITEM hItem) const;
	HTREEITEM GetPrevVisibleItem(HTREEITEM hItem) const;
	HTREEITEM GetSelectedItem() const;
	HTREEITEM GetDropHilightItem() const;
	HTREEITEM GetRootItem() const;
	BOOL GetItem(TV_ITEMA* pItem) const;
	BOOL GetItem(TV_ITEMW* pItem) const;
	CString GetItemText(HTREEITEM hItem) const;
	BOOL GetItemImage(HTREEITEM hItem,int& nImage, int& nSelectedImage) const;
	UINT GetItemState(HTREEITEM hItem,UINT nStateMask) const;
	DWORD_PTR GetItemData(HTREEITEM hItem) const;
	BOOL SetItem(TV_ITEMA* pItem);
	BOOL SetItem(TV_ITEMW* pItem);
	BOOL SetItem(HTREEITEM hItem,UINT nMask,LPCSTR lpszItem,int nImage,
		int nSelectedImage,UINT nState=0,UINT nStateMask=0, LPARAM lParam=0);
	BOOL SetItem(HTREEITEM hItem,UINT nMask,LPCWSTR lpszItem,int nImage,
		int nSelectedImage,UINT nState=0,UINT nStateMask=0, LPARAM lParam=0);
	BOOL SetItemText(HTREEITEM hItem,LPCSTR lpszItem);
	BOOL SetItemText(HTREEITEM hItem,LPCWSTR lpszItem);
	BOOL SetItemImage(HTREEITEM hItem,int nImage,int nSelectedImage);
	BOOL SetItemState(HTREEITEM hItem,UINT nState,UINT nStateMask);
	BOOL SetItemData(HTREEITEM hItem,DWORD_PTR dwData);
	BOOL ItemHasChildren(HTREEITEM hItem) const;
	HWND GetEditControl() const;
	UINT GetVisibleCount() const;
	UINT GetCheckState(HTREEITEM hItem) const;
	void SetCheckState(HTREEITEM hItem,UINT nState);
	UINT GetRadioState(HTREEITEM hItem) const;
	void SetRadioState(HTREEITEM hItem,UINT nState);

	int GetItemHeight() const;
	int SetItemHeight(SHORT cuHeight); 


	HTREEITEM InsertItem(LPTV_INSERTSTRUCTA lpInsertStruct);
	HTREEITEM InsertItem(LPTV_INSERTSTRUCTW lpInsertStruct);
	HTREEITEM InsertItem(UINT nMask,LPCTSTR lpszItem,int nImage,
		int nSelectedImage,UINT nState,UINT nStateMask,LPARAM lParam,
		HTREEITEM hParent, HTREEITEM hInsertAfter);
	HTREEITEM InsertItem(LPCSTR lpszItem,HTREEITEM hParent=TVI_ROOT,
		HTREEITEM hInsertAfter=TVI_LAST);
	HTREEITEM InsertItem(LPCWSTR lpszItem,HTREEITEM hParent=TVI_ROOT,
		HTREEITEM hInsertAfter=TVI_LAST);
	HTREEITEM InsertItem(LPCSTR lpszItem,int nImage,int nSelectedImage,
		HTREEITEM hParent=TVI_ROOT,HTREEITEM hInsertAfter=TVI_LAST);
	HTREEITEM InsertItem(LPCWSTR lpszItem,int nImage,int nSelectedImage,
		HTREEITEM hParent=TVI_ROOT,HTREEITEM hInsertAfter=TVI_LAST);
	BOOL DeleteItem(HTREEITEM hItem);
	BOOL DeleteAllItems();
	BOOL Expand(HTREEITEM hItem,UINT nCode);
	BOOL Select(HTREEITEM hItem,UINT nCode);
	BOOL SelectItem(HTREEITEM hItem);
	BOOL SelectDropTarget(HTREEITEM hItem);
	BOOL SelectSetFirstVisible(HTREEITEM hItem);
	HWND EditLabel(HTREEITEM hItem);
	HTREEITEM HitTest(const POINT& pt,UINT* pFlags=NULL) const;
	HTREEITEM HitTest(TV_HITTESTINFO* pHitTestInfo) const;
	HIMAGELIST CreateDragImage(HTREEITEM hItem);
	BOOL SortChildren(HTREEITEM hItem);
	BOOL EnsureVisible(HTREEITEM hItem);
	BOOL SortChildrenCB(LPTV_SORTCB pSort);


	// Get last item
	HTREEITEM GetLastItem() const;
	
	// Returns next item which is: first child (if have childs), next sibling (if have any), 
	// or next item for parent. Calling GetNextItem until it returns gives items in tree
	// in the same order as in the tree. If hItem is NULL, returns the first item in the list
	HTREEITEM GetNextItem(HTREEITEM hItem) const; 
	
	// Returns previous item. Works similarly as GetNextItem but to opposite direction (i.e. 
	// GetNextItem(GetPrevItem(pItem))==pItem and GetPrevItem(GetNextItem(pItem))==pItem).
	// If hItem is NULL, returns the last item in the list
	HTREEITEM GetPrevItem(HTREEITEM hItem) const; 


	// Finds item based on text, set bPartial to TRUE if you want that partial match is OK
	HTREEITEM FindItem(LPCSTR pText,BOOL bBackwardDirection=FALSE,BOOL bPartial=TRUE,HTREEITEM hBegin=NULL,HTREEITEM hEnd=NULL) const; 
	HTREEITEM FindItem(LPCWSTR pText,BOOL bBackwardDirection=FALSE,BOOL bPartial=TRUE,HTREEITEM hBegin=NULL,HTREEITEM hEnd=NULL) const; 
	
	
};

class CComboBoxEx : public CCommonCtrl, public CComboBox, public CEdit
{
public:
	CComboBoxEx();
	CComboBoxEx(HWND hWnd);

	BOOL Create(DWORD dwStyle, const RECT* rect, HWND hParentWnd, UINT nID);
	
	int InsertItem(const COMBOBOXEXITEM* pItem);
	int InsertItem(const COMBOBOXEXITEMW* pItem);
	int InsertItem(LPCSTR pszText,int iImage=-1,int iSelectedImage=-1,int iOverlay=-1,int iIndent=0,LPARAM lParam=0,UINT mask=0);
	int InsertItem(LPCWSTR pszText,int iImage=-1,int iSelectedImage=-1,int iOverlay=-1,int iIndent=0,LPARAM lParam=0,UINT mask=0);
	
	HIMAGELIST GetImageList() const;
	HIMAGELIST SetImageList(HIMAGELIST hImageList);
	BOOL GetItem(COMBOBOXEXITEM* pItem) const;
	BOOL GetItem(COMBOBOXEXITEMW* pItem) const;
	BOOL SetItem(const COMBOBOXEXITEM* pItem);
	BOOL SetItem(const COMBOBOXEXITEMW* pItem);
	BOOL DeleteItem(int nItem);
	HWND GetComboControl() const;
	HWND GetEditControl() const;
	
	CString GetItemText(int nItem) const;
	CStringW GetItemTextW(int nItem) const;
	int GetItemText(int nItem,LPSTR lpszText,int nLen) const;
	int GetItemText(int nItem,LPWSTR lpszText,int nLen) const;
	BOOL SetItemText(int nItem,LPCSTR lpszText);
	BOOL SetItemText(int nItem,LPCWSTR lpszText);
	
	DWORD_PTR GetItemData(int nIndex) const;
	DWORD_PTR GetItemDataSafe(int nIndex) const;
	void* GetItemDataPtr(int nIndex) const;
	int SetItemData(int nIndex, DWORD_PTR dwItemData);
	
	using CCommonCtrl::GetWindowLong;
	using CCommonCtrl::SetWindowLong;
	using CCommonCtrl::ModifyStyle;
	using CCommonCtrl::ModifyStyleEx;

	DWORD GetExStyle() const;
	DWORD SetExStyle(DWORD dwExStyle);
	
	BOOL HasEditChanged() const;

	HWND GetHandle() const { return CCommonCtrl::m_hWnd; }
	void Attach(HWND hWnd);
	void AttachToDlgItem(HWND hDialog,int nID);
	
	using CCommonCtrl::DestroyWindow;
	
	using CCommonCtrl::GetDlgCtrlID;
	using CCommonCtrl::SetDlgCtrlID;
	using CCommonCtrl::GetParent;
	using CCommonCtrl::SetParent;
	using CCommonCtrl::PostMessage;
	using CCommonCtrl::SendMessage;
	using CCommonCtrl::GetFont;
	using CCommonCtrl::MoveWindow;
	using CCommonCtrl::SetWindowRgn;
	using CCommonCtrl::GetWindowRgn;
	using CCommonCtrl::SetWindowPos;
	using CCommonCtrl::GetWindowRect;
	using CCommonCtrl::GetClientRect;
	using CCommonCtrl::ClientToScreen;
	using CCommonCtrl::ScreenToClient;
	using CCommonCtrl::GetDC;
	using CCommonCtrl::GetWindowDC;
	using CCommonCtrl::ReleaseDC; 
	using CCommonCtrl::UpdateWindow;
	using CCommonCtrl::ShowWindow;
	using CCommonCtrl::IsWindowVisible;
	using CCommonCtrl::RedrawWindow;
	using CCommonCtrl::IsWindowEnabled;
	using CCommonCtrl::EnableWindow;
	using CCommonCtrl::SetFocus; 
};

#ifdef DEF_RESOURCES
class CAnimateCtrl : public CCommonCtrl
{
public:
	CAnimateCtrl();
	CAnimateCtrl(HWND hWnd);

	BOOL Create(DWORD dwStyle, const RECT* rect, HWND hParentWnd, UINT nID);

	BOOL Open(LPTSTR szName);
	BOOL OpenEx(LPTSTR szName);
	BOOL Play(WORD from, WORD to, DWORD rep);
	BOOL Stop();
	BOOL Close();
	BOOL Seek(DWORD frame);
};
#endif

class CMonthCalCtrl : public CCommonCtrl
{
public:
	CMonthCalCtrl();
	CMonthCalCtrl(HWND hWnd);

	BOOL Create(DWORD dwStyle, const RECT* rect, HWND hParentWnd, UINT nID);

	BOOL GetCurSel(LPSYSTEMTIME pst) const;
    BOOL SetCurSel(LPSYSTEMTIME pst);
	
	DWORD GetMaxSelCount() const;
	BOOL SetMaxSelCount(UINT n);
	
	BOOL GetSelRange(LPSYSTEMTIME rgst) const;
	BOOL SetSelRange(LPSYSTEMTIME rgst);
	DWORD GetMonthRange(DWORD gmr, LPSYSTEMTIME rgst) const;
	
	BOOL SetDayState(int cbds, LPMONTHDAYSTATE rgds);
	BOOL GetMinReqRect(LPRECT prc) const;
	BOOL SetColor(int iColor,COLORREF clr);
	COLORREF GetColor(int iColor) const;

	BOOL SetToday(LPSYSTEMTIME pst);
	BOOL GetToday(LPSYSTEMTIME pst);

	BOOL HitTest(PMCHITTESTINFO pinfo);

	BOOL SetFirstDayOfWeek(int iDay);
	DWORD GetFirstDayOfWeek() const;
	DWORD GetRange(LPSYSTEMTIME rgst) const;
	BOOL SetRange(DWORD gdtr, LPSYSTEMTIME rgst);
	int GetMonthDelta() const;
	int SetMonthDelta(int n);
	DWORD GetMaxTodayWidth(LPSIZE psz) const;

};	

class CDateTimeCtrl : public CCommonCtrl
{
public:
	CDateTimeCtrl();
	CDateTimeCtrl(HWND hWnd);

	BOOL Create(DWORD dwStyle, const RECT* rect, HWND hParentWnd, UINT nID);

	DWORD GetSystemtime(LPSYSTEMTIME pst) const;
	BOOL SetSystemtime(DWORD gd, LPSYSTEMTIME pst);

	DWORD GetRange(LPSYSTEMTIME rgst) const;
	BOOL SetRange(DWORD gdtr, LPSYSTEMTIME rgst);
	BOOL SetFormat(LPCTSTR sz);


	BOOL SetMonthCalColor(int iColor, COLORREF clr);
	COLORREF GetMonthCalColor(int iColor) const;
	HWND GetMonthCal();

	BOOL SetMonthCalFont(HFONT hfont,DWORD fRedraw);
	HFONT GetMonthCalFont() const;

};	

#include "CommonControls.inl"

#endif
#endif

