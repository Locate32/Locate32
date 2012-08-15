////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////


#ifndef CMNCTLEX_INL
#define CMNCTLEX_INL

inline CListCtrlEx::CListCtrlEx()
{
}

inline CListCtrlEx::CListCtrlEx(HWND hWnd)
:	CListCtrl(hWnd)
{
	if (hWnd!=NULL) // Subclassing
		SetWindowSubclass(hWnd,SubclassProc,0,(DWORD_PTR)this);
}

inline int CListCtrlEx::GetColumnCount() const
{
	return aColumns.GetSize();
}

inline int CListCtrlEx::GetVisibleColumnCount() const
{
	return aSubItems.GetSize();
}

inline int CListCtrlEx::GetColumnIDFromSubItem(int nSubItem) const
{
	ASSERT(nSubItem<aSubItems.GetSize());
	return aSubItems[nSubItem];
}

inline int CListCtrlEx::GetColumnSubItemFromID(int nID) const
{
	for (int i=0;i<aSubItems.GetSize();i++)
	{
		if (aSubItems[i]==nID)
			return i;
	}
	return -1;
}


inline int CListCtrlEx::GetColumnID(int nCol) const
{
	ASSERT(nCol<aColumns.GetSize());
	return aColumns[nCol]->nID;
}
	
inline int CListCtrlEx::GetVisibleColumn(int nCol) const
{
	ASSERT(nCol<aColumns.GetSize());
	if (aColumns[nCol]->bFlags&COLUMNDATA::FlagVisible)
		return GetVisibleColumnFromSubItem(aColumns[nCol]->lc.iSubItem);
	return -1;
}

inline CListCtrlEx::COLUMNDATA::~COLUMNDATA()
{
	if (!(bFlags&FlagTitleIsResource) && pStrTitle!=NULL)
	{
		delete[] pStrTitle;
	}
}

inline BOOL CListCtrlEx::Create(DWORD dwStyle, const RECT* rect, HWND hParentWnd, UINT nID)
{
	if (!CListCtrl::Create(dwStyle,rect,hParentWnd,nID))
		return FALSE;

	// Subclassing
	SetWindowSubclass(m_hWnd,SubclassProc,0,(DWORD_PTR)this);
}

inline int CListCtrlEx::GetColumnWidth(int nCol) const
{
	if (aColumns[nCol]->bFlags&COLUMNDATA::FlagVisible)
		return CListCtrl::GetColumnWidth(GetVisibleColumnFromSubItem(aColumns[nCol]->lc.iSubItem));
	else
		return aColumns[nCol]->lc.cx;
}


inline BOOL CListCtrlEx::SetColumnWidth(int nCol, int cx)
{
	if (aColumns[nCol]->bFlags&COLUMNDATA::FlagVisible)
		return CListCtrl::SetColumnWidth(GetVisibleColumnFromSubItem(aColumns[nCol]->lc.iSubItem),cx);
	else
		aColumns[nCol]->lc.cx=cx;
	return TRUE;
}

inline BOOL CListCtrlEx::GetColumnIDArray(int iCount,LPINT pi) const
{
	for (int i=min(aColumns.GetSize(),iCount)-1;i>=0;i--)
		pi[i]=aColumns[i]->nID;
	return TRUE;
}

inline int CListCtrlEx::GetColumnFromID(int nID) const
{
	for (int i=0;i<aColumns.GetSize();i++)
	{
		if (aColumns[i]->nID==nID)
			return i;
	}
	return -1;
}


inline int CListCtrlEx::GetColumnFromSubItem(int nSubItem) const
{
	for (int i=0;i<aColumns.GetSize();i++)
	{
		if (aColumns[i]->bFlags&COLUMNDATA::FlagVisible &&
			aColumns[i]->lc.iSubItem==nSubItem)
			return i;
	}
	return -1;
}

#endif