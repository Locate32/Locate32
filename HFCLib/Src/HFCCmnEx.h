////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////


#ifndef HFCCOMMONCONTROLSEX_H
#define HFCCOMMONCONTROLSEX_H

#if defined(DEF_WINDOWS) && defined(DEF_RESOURCES)


#ifdef DEF_WCHAR

class CListCtrlEx	:	public CListCtrl
{
public:
	CListCtrlEx();
	CListCtrlEx(HWND hWnd);
	virtual ~CListCtrlEx();

	BOOL Create(DWORD dwStyle,const RECT* rect,HWND hParentWnd,UINT nID);

	int InsertColumn(int nID,LPCSTR lpszColumnHeading,BOOL bShow,int nFormat = LVCFMT_LEFT, 
		int nWidth = -1);
	int InsertColumn(int nID,LPCWSTR lpszColumnHeading,BOOL bShow,int nFormat = LVCFMT_LEFT, 
		int nWidth = -1);
	int InsertColumn(int nID,int nTitleID,BOOL bShow,int nFormat = LVCFMT_LEFT, 
		int nWidth = -1,TypeOfResourceHandle bType=LanguageSpecificResource);
	int InsertColumn(int nID,BOOL bShow,const LVCOLUMN* lc);
	int InsertColumn(int nID,BOOL bShow,const LVCOLUMNW* lc);
	BOOL DeleteColumn(int nCol);


public:
	BOOL ShowColumn(int nCol);
	BOOL HideColumn(int nCol);

	BOOL SetColumnOrderArray(int iCount,LPINT pi);
	BOOL GetColumnOrderArray(int iCount,LPINT pi) const;
	
	BOOL SetColumnWidthArray(int iCount,LPINT pi);
	BOOL GetColumnWidthArray(int iCount,LPINT pi) const;

	BOOL GetColumnIDArray(int iCount,LPINT pi) const;

	int GetColumnCount() const;
	int GetVisibleColumnCount() const;
	int GetColumnID(int nCol) const;
	int GetColumnFromID(int nID) const;
	int GetColumnFromSubItem(int nSubItem) const;
	int GetColumnIDFromSubItem(int nSubItem) const;
	int GetColumnSubItemFromID(int nID) const;
	
	BOOL GetColumn(int nCol, LV_COLUMN* pColumn) const;
	BOOL SetColumn(int nCol, const LV_COLUMN* pColumn);
	BOOL GetColumn(int nCol, LV_COLUMNW* pColumn) const;
	BOOL SetColumn(int nCol, const LV_COLUMNW* pColumn);
	
	int GetColumnWidth(int nCol) const;
	BOOL SetColumnWidth(int nCol, int cx);

	
	HMENU CreateColumnSelectionMenu(int nFirstID) const;
	void ColumnSelectionMenuProc(int wID,int nFirstID);

	BOOL LoadColumnsState(HKEY hRootKey,LPCSTR lpKey,LPCSTR lpSubKey);
	BOOL SaveColumnsState(HKEY hRootKey,LPCSTR lpKey,LPCSTR lpSubKey) const;

	BOOL GetColumnTitle(CString& sTitle,int nCol);

	int GetVisibleColumn(int nCol) const; // Return id for actual column index in ListCtrl
	int GetVisibleColumnFromSubItem(int nSubItem) const;

private:
	void RemoveSubitem(int nSubItem);

	struct COLUMNDATA {
		COLUMNDATA(int nID,int nWidth,int nFormat,int nTitleID,TypeOfResourceHandle bResourceType);
		COLUMNDATA(int nID,int nWidth,int nFormat,LPCSTR pTitle);
		COLUMNDATA(int nID,int nWidth,int nFormat,LPCWSTR pTitle);
		COLUMNDATA(int nID,const LVCOLUMN* lc);
		COLUMNDATA(int nID,const LVCOLUMNW* lc);
		~COLUMNDATA();

		enum {
			FlagTitleIsResource=0x1,
			FlagVisible=0x2
		};

		int nID;
		union { // Caption
			struct {
				WORD nTitleID;
				TypeOfResourceHandle bResourceType;
			};		
			LPWSTR pStrTitle;
		};
		BYTE bFlags;
		LVCOLUMNW lc;
	};
	CArrayFP<COLUMNDATA*> aColumns;
	CIntArray aSubItems; // Contains ID for visible columns
	
	static LRESULT CALLBACK SubclassProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam,
		UINT_PTR uIdSubclass,DWORD_PTR dwRefData);

};

#endif

#include "CommonControlsEx.inl"

#endif
#endif
