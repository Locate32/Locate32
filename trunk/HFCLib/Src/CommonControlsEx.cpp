////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////

#include "HFCLib.h"



#if defined(DEF_WINDOWS) && defined(DEF_RESOURCES)

CListCtrlEx::COLUMNDATA::COLUMNDATA(int nID_,int nWidth,int nFormat,int nTitleID_,TypeOfResourceHandle bResourceType_)
:	bFlags(FlagTitleIsResource),nID(nID_),
	nTitleID(nTitleID_),bResourceType(bResourceType_)
{
	lc.mask=LVCF_FMT;
	lc.fmt=nFormat;
	if (nWidth>=0)
	{
		lc.cx=nWidth;
		lc.mask|=LVCF_WIDTH;
	}
}

CListCtrlEx::COLUMNDATA::COLUMNDATA(int nID_,int nWidth,int nFormat,LPCSTR pTitle)
:	bFlags(0),nID(nID_)
{
	lc.mask=LVCF_FMT;
	lc.fmt=nFormat;
	if (nWidth>=0)
	{
		lc.cx=nWidth;
		lc.mask|=LVCF_WIDTH;
	}

	pStrTitle=alloccopyAtoW(pTitle);
}

CListCtrlEx::COLUMNDATA::COLUMNDATA(int nID_,int nWidth,int nFormat,LPCWSTR pTitle)
:	bFlags(0),nID(nID_)
{
	lc.mask=LVCF_FMT;
	lc.fmt=nFormat;
	if (nWidth>=0)
	{
		lc.cx=nWidth;
		lc.mask|=LVCF_WIDTH;
	}

	pStrTitle=alloccopy(pTitle);
}

CListCtrlEx::COLUMNDATA::COLUMNDATA(int nID_,const LVCOLUMN* _lc)
:	bFlags(0),nID(nID_)
{
	MemCopy(&lc,_lc,sizeof(LVCOLUMN));

	if (lc.mask&LVCF_TEXT)
		pStrTitle=alloccopyAtoW(_lc->pszText);
	else
	{
		pStrTitle=new WCHAR[2];
		pStrTitle[0]='\0';
	}
}

CListCtrlEx::COLUMNDATA::COLUMNDATA(int nID_,const LVCOLUMNW* _lc)
:	bFlags(0),nID(nID_)
{
	MemCopy(&lc,_lc,sizeof(LVCOLUMN));

	if (lc.mask&LVCF_TEXT)
		pStrTitle=alloccopy(_lc->pszText);
	else
	{
		pStrTitle=new WCHAR[2];
		pStrTitle[0]='\0';
	}
}

int CListCtrlEx::InsertColumn(int nID,LPCSTR lpszColumnHeading,BOOL bShow,int nFormat,int nWidth)
{
	COLUMNDATA* pNew=new COLUMNDATA(nID,nWidth,nFormat,lpszColumnHeading);
    aColumns.Add(pNew);

	if (bShow)
		ShowColumn(aColumns.GetSize()-1);
	return aColumns.GetSize()-1;
}

int CListCtrlEx::InsertColumn(int nID,LPCWSTR lpszColumnHeading,BOOL bShow,int nFormat,int nWidth)
{
	COLUMNDATA* pNew=new COLUMNDATA(nID,nWidth,nFormat,lpszColumnHeading);
    aColumns.Add(pNew);

	if (bShow)
		ShowColumn(aColumns.GetSize()-1);
	return aColumns.GetSize()-1;
}

int CListCtrlEx::InsertColumn(int nID,int nTitleID,BOOL bShow,int nFormat,int nWidth,TypeOfResourceHandle bType)
{
	COLUMNDATA* pNew=new COLUMNDATA(nID,nWidth,nFormat,nTitleID,bType);
	aColumns.Add(pNew);

	if (bShow)
		ShowColumn(aColumns.GetSize()-1);
	return aColumns.GetSize()-1;
}

int CListCtrlEx::InsertColumn(int nID,BOOL bShow,const LVCOLUMN* lc)
{
	COLUMNDATA* pNew=new COLUMNDATA(nID,lc);
	aColumns.Add(pNew);

	if (bShow)
		ShowColumn(aColumns.GetSize()-1);
	return aColumns.GetSize()-1;
}

int CListCtrlEx::InsertColumn(int nID,BOOL bShow,const LVCOLUMNW* lc)
{
	COLUMNDATA* pNew=new COLUMNDATA(nID,lc);
	aColumns.Add(pNew);

	if (bShow)
		ShowColumn(aColumns.GetSize()-1);
	return aColumns.GetSize()-1;
}

BOOL CListCtrlEx::DeleteColumn(int nCol)
{
	if (aColumns[nCol]->bFlags&COLUMNDATA::FlagVisible)
	{
		CListCtrl::DeleteColumn(GetVisibleColumnFromSubItem(
			aColumns[nCol]->lc.iSubItem));
	    RemoveSubitem(aColumns[nCol]->lc.iSubItem);
	}
	aColumns.RemoveAt(nCol);
	return TRUE;
}

BOOL CListCtrlEx::ShowColumn(int nCol)
{
	if (aColumns[nCol]->bFlags&COLUMNDATA::FlagVisible)
		return TRUE;

	aColumns[nCol]->lc.mask|=LVCF_SUBITEM|LVCF_TEXT;
	if (aColumns[nCol]->bFlags&COLUMNDATA::FlagTitleIsResource)
	{
		CStringW txt(UINT(aColumns[nCol]->nTitleID),aColumns[nCol]->bResourceType);
		aColumns[nCol]->lc.pszText=txt.GiveBuffer();
	}
	else
		aColumns[nCol]->lc.pszText=aColumns[nCol]->pStrTitle;



	if (nCol==0) // Should be label
	{
		aColumns[nCol]->lc.iSubItem=0;
		aSubItems.InsertAt(0,aColumns[nCol]->nID);
		CListCtrl::InsertColumn(0,&aColumns[nCol]->lc);
	
		for (int i=0;i<aColumns.GetSize();i++)
		{
			if (aColumns[i]->bFlags&COLUMNDATA::FlagVisible)
				aColumns[i]->lc.iSubItem++;
		}

		LVCOLUMN lc;
		lc.mask=LVCF_SUBITEM;
		for (int i=1;CListCtrl::GetColumn(i,&lc);i++)
		{
			lc.iSubItem++;
			CListCtrl::SetColumn(i,&lc);
		}	
	}
	else
	{
		aColumns[nCol]->lc.iSubItem=aSubItems.GetSize();
		aSubItems.Add(aColumns[nCol]->nID);
		CListCtrl::InsertColumn(aSubItems.GetSize(),&aColumns[nCol]->lc);
	}

	aColumns[nCol]->bFlags|=COLUMNDATA::FlagVisible;

	if (aColumns[nCol]->bFlags&COLUMNDATA::FlagTitleIsResource)
		delete[] aColumns[nCol]->lc.pszText;
	return TRUE;
}

BOOL CListCtrlEx::HideColumn(int nCol)
{
	if (aColumns[nCol]->bFlags&COLUMNDATA::FlagVisible)
	{
		aColumns[nCol]->lc.mask&=~LVCF_TEXT;
		int nActualID=GetVisibleColumnFromSubItem(aColumns[nCol]->lc.iSubItem);
		ASSERT(nActualID>=0);
		
		CListCtrl::GetColumn(nActualID,&aColumns[nCol]->lc);
		CListCtrl::DeleteColumn(nActualID);
		ASSERT(aColumns[nCol]->lc.iSubItem<aSubItems.GetSize());
		ASSERT(aSubItems[aColumns[nCol]->lc.iSubItem]==aColumns[nCol]->nID);

		RemoveSubitem(aColumns[nCol]->lc.iSubItem);

				
		aColumns[nCol]->bFlags&=~int(COLUMNDATA::FlagVisible);
	}
	return TRUE;
}

// Format
// ~DWORD(Ncolumns) DWORD(Nvisiblecolumns) DWORD(Nwidth)*Ncolumns DWORD(Norder)*Nviscolumns DWORD(flags)*Ncolumns
// Flags are:
enum CListCtrlExFlags {
	cefLeft = 0x0,
	cefRight = 0x1,
	cefCenter = 0x3,
	cefJustifyMask=0x3
};

CListCtrlEx::~CListCtrlEx()
{
	if (m_hWnd!=NULL)
		RemoveWindowSubclass(m_hWnd,SubclassProc,0);
}

BOOL CListCtrlEx::LoadColumnsState(HKEY hRootKey,LPCSTR lpKey,LPCSTR lpSubKey)
{
	CRegKey RegKey;
	if (lpKey==NULL)
		RegKey.m_hKey=hRootKey;
	else if (RegKey.OpenKey(hRootKey,lpKey,CRegKey::openExist|CRegKey::samRead)!=ERROR_SUCCESS)
		return FALSE;
	
	int nColumnCount=GetColumnCount();
	
	// Get data length
	DWORD nDataLength=RegKey.QueryValueLength(lpSubKey);
	
	// Check whether length is correct, data contains at least widths
	if (nDataLength<sizeof(int)*(2+nColumnCount) && 
		nDataLength%sizeof(int)!=0)
	{
		if (lpKey==NULL)
			RegKey.m_hKey=NULL;
		return FALSE;
	}
	nDataLength/=sizeof(int);

	// Query data
	int* pData=new int[nDataLength];
	DWORD dwRet=RegKey.QueryValue(lpSubKey,(LPSTR)pData,nDataLength*sizeof(int));
	if (lpKey==NULL)
		RegKey.m_hKey=NULL;
	
	
	if (dwRet<nDataLength || pData[0]!=~nColumnCount ||
		(nDataLength!=nColumnCount+pData[1]+2 && // older
		nDataLength!=2*nColumnCount+pData[1]+2)  // newer, also flags
		)
	{
		delete[] pData;
		return FALSE;
	}

    // Set column order
	SetColumnOrderArray(pData[1],pData+2+nColumnCount);
	
	if (nDataLength==2*nColumnCount+pData[1]+2)
	{
		// New format, contains also flags
		LVCOLUMN lc;
		lc.iSubItem=0;
	
		int* piWidth=pData+2;
		int* piFlags=pData+2+nColumnCount+pData[1];

		// Set other flags
		for (int iCol=0;iCol<nColumnCount;iCol++)
		{
			lc.mask=LVCF_FMT;
			GetColumn(iCol,&lc); // Retrieving current fmt
		
			lc.mask=LVCF_FMT|LVCF_WIDTH;
		
			// Set width
			lc.cx=piWidth[iCol];

			// Set flags
			lc.fmt&=~LVCFMT_JUSTIFYMASK;
			if ((piFlags[iCol]&cefJustifyMask)==cefRight)
				lc.fmt|=LVCFMT_RIGHT;
			else if ((piFlags[iCol]&cefJustifyMask)==cefCenter)
				lc.fmt|=LVCFMT_CENTER;
			else
				lc.fmt|=LVCFMT_LEFT;

			// Set column data
			SetColumn(iCol,&lc);
		}
	
	}
	else
		SetColumnWidthArray(nColumnCount,pData+2);

	
	delete[] pData;
	return TRUE;
}

BOOL CListCtrlEx::SaveColumnsState(HKEY hRootKey,LPCSTR lpKey,LPCSTR lpSubKey) const
{
	CRegKey RegKey;
	if (lpKey==NULL)
		RegKey.m_hKey=hRootKey;
	else if (RegKey.OpenKey(hRootKey,lpKey,CRegKey::createNew|CRegKey::samAll)!=ERROR_SUCCESS)
		return FALSE;
	
	int nColumnCount=GetColumnCount();
	int nVisibleCount=GetVisibleColumnCount();

	int* pData=new int[2+2*nColumnCount+nVisibleCount];
	
	
	// Set Ncolumns
	pData[0]=~nColumnCount; 
	
	// Set Nviscount
	pData[1]=nVisibleCount;

	// Retvieve orders
	GetColumnOrderArray(nVisibleCount,pData+2+nColumnCount);

	// Retvieve flags
	LVCOLUMN lc;
	lc.mask=LVCF_FMT|LVCF_WIDTH;
	lc.iSubItem=0;
	
	int* piWidth=pData+2;
	int* piFlags=pData+2+nColumnCount+nVisibleCount;
		
	for (int iCol=0;iCol<nColumnCount;iCol++)
	{
		GetColumn(iCol,&lc);
		
		// Set width
		piWidth[iCol]=lc.cx;

		// Set flags
		if ((lc.fmt&LVCFMT_JUSTIFYMASK)==LVCFMT_RIGHT)
			piFlags[iCol]=cefRight;
		else if ((lc.fmt&LVCFMT_JUSTIFYMASK)==LVCFMT_CENTER)
			piFlags[iCol]=cefCenter;
		else
			piFlags[iCol]=cefLeft;
	}

        
	BOOL bRet=RegKey.SetValue(lpSubKey,(LPSTR)pData,sizeof(int)*(2+2*nColumnCount+nVisibleCount),REG_BINARY)==ERROR_SUCCESS;
	RegKey.CloseKey();
	delete[] pData;
	if (lpKey==NULL)
		RegKey.m_hKey=NULL;
	return bRet;
}

HMENU CListCtrlEx::CreateColumnSelectionMenu(int nFirstID) const
{
	HMENU hMenu=CreatePopupMenu();

	if (IsUnicodeSystem())
	{
		MENUITEMINFOW mii;
		mii.cbSize=sizeof(MENUITEMINFOW);
		mii.fMask=MIIM_STATE|MIIM_ID|MIIM_TYPE;
		mii.fType=MFT_STRING;
		for (int i=0;i<aColumns.GetSize();i++)
		{
			if (aColumns[i]->bFlags&COLUMNDATA::FlagTitleIsResource)
			{
				CStringW txt(UINT(aColumns[i]->nTitleID),aColumns[i]->bResourceType);
				mii.dwTypeData=txt.GiveBuffer();
			}
			else
				mii.dwTypeData=aColumns[i]->pStrTitle;
			mii.wID=nFirstID+aColumns[i]->nID;
			if (aColumns[i]->bFlags&COLUMNDATA::FlagVisible)
				mii.fState=MFS_CHECKED;
			else
				mii.fState=MFS_ENABLED;
			InsertMenuItemW(hMenu,i,TRUE,&mii);
			if (aColumns[i]->bFlags&COLUMNDATA::FlagTitleIsResource)
				delete[] (char*) mii.dwTypeData;
		
			mii.wID++;
		}
	}
	else
	{
		MENUITEMINFO mii;
		mii.cbSize=sizeof(MENUITEMINFO);
		mii.fMask=MIIM_STATE|MIIM_ID|MIIM_TYPE;
		mii.fType=MFT_STRING;
		for (int i=0;i<aColumns.GetSize();i++)
		{
			if (aColumns[i]->bFlags&COLUMNDATA::FlagTitleIsResource)
			{
				CString txt(UINT(aColumns[i]->nTitleID),aColumns[i]->bResourceType);
				mii.dwTypeData=txt.GiveBuffer();
			}
			else
				mii.dwTypeData=alloccopyWtoA(aColumns[i]->pStrTitle);
			mii.wID=nFirstID+aColumns[i]->nID;
			if (aColumns[i]->bFlags&COLUMNDATA::FlagVisible)
				mii.fState=MFS_CHECKED;
			else
				mii.fState=MFS_ENABLED;
			InsertMenuItem(hMenu,i,TRUE,&mii);
			
			delete[] (char*) mii.dwTypeData;
			mii.wID++;
		}
	}
	return hMenu;		
}

void CListCtrlEx::ColumnSelectionMenuProc(int nID,int nFirstID)
{
	nID-=nFirstID;

	for (int i=0;i<aColumns.GetSize();i++)
	{
		if (aColumns[i]->nID==nID)
		{
			if (aColumns[i]->bFlags&COLUMNDATA::FlagVisible)
			{
				if (GetVisibleColumnCount()>1) // Do not hide last column
					HideColumn(i);
			}
			else
				ShowColumn(i);
			break;
		}
	}
}


int CListCtrlEx::GetVisibleColumnFromSubItem(int nSubItem) const
{
	LVCOLUMN lc;
	lc.mask=LVCF_SUBITEM;
	
	for (int i=0;CListCtrl::GetColumn(i,&lc);i++)
	{
		if (lc.iSubItem==nSubItem)
		{
			// TODO: Jos tästä ei tule valitusta palauta suoraan nSubItem
			ASSERT(i==nSubItem);
			return i;
		}
	}
	return -1;
}

BOOL CListCtrlEx::GetColumnTitle(CString& sTitle,int nCol)
{
	if (nCol<0 || nCol>=aColumns.GetSize())
		return FALSE;
	

	if (aColumns[nCol]->bFlags&COLUMNDATA::FlagTitleIsResource)
		sTitle.LoadStringA(UINT(aColumns[nCol]->nTitleID),aColumns[nCol]->bResourceType);
	else
		sTitle=aColumns[nCol]->pStrTitle;
	return TRUE;
}


void CListCtrlEx::RemoveSubitem(int nSubItem)
{
	for (int i=0;i<aColumns.GetSize();i++)
	{
		if (aColumns[i]->bFlags&COLUMNDATA::FlagVisible &&
			aColumns[i]->lc.iSubItem>nSubItem)
			aColumns[i]->lc.iSubItem--;
	}

	LVCOLUMN lc;
	lc.mask=LVCF_SUBITEM;
	for (int i=0;CListCtrl::GetColumn(i,&lc);i++)
	{
		if (lc.iSubItem>nSubItem)
		{
			lc.iSubItem--;
			CListCtrl::SetColumn(i,&lc);
		}
	}
	aSubItems.RemoveAt(nSubItem);
}


BOOL CListCtrlEx::SetColumnOrderArray(int iCount,LPINT pi)
{
	int i,j;
	// First show/hide correct columns
	for (i=0;i<aColumns.GetSize();i++)
	{
		for (j=0;j<iCount;j++)
		{
			if (pi[j]==i)
				break;
		}
		if (j<iCount)
			ShowColumn(i);
		else
			HideColumn(i);
	}
	
	for (i=0;i<iCount;i++)
		pi[i]=GetVisibleColumn(pi[i]);		

	CListCtrl::SetColumnOrderArray(iCount,pi);
	return TRUE;
}

BOOL CListCtrlEx::GetColumnOrderArray(int iCount,LPINT pi) const
{
	if (iCount<GetVisibleColumnCount())
		return FALSE;
	iCount=GetVisibleColumnCount();
	
	// Obtaining actual IDs
	CListCtrl::GetColumnOrderArray(iCount,pi);

	// Converting actual ID to column index
	for (int i=0;i<iCount;i++)
		pi[i]=GetColumnFromSubItem(pi[i]);
	return TRUE;
}




BOOL CListCtrlEx::SetColumnWidthArray(int iCount,LPINT pi)
{
	for (int i=min(iCount,aColumns.GetSize())-1;i>=0;i--)
	{
		if (aColumns[i]->bFlags&COLUMNDATA::FlagVisible)
			CListCtrl::SetColumnWidth(GetVisibleColumnFromSubItem(aColumns[i]->lc.iSubItem),pi[i]);
		else
			aColumns[i]->lc.cx=pi[i];
	}
	return TRUE;
}

BOOL CListCtrlEx::GetColumnWidthArray(int iCount,LPINT pi) const
{
	for (int i=min(iCount,aColumns.GetSize())-1;i>=0;i--)
	{
		if (aColumns[i]->bFlags&COLUMNDATA::FlagVisible)
			pi[i]=CListCtrl::GetColumnWidth(GetVisibleColumnFromSubItem(aColumns[i]->lc.iSubItem));
		else
			pi[i]=aColumns[i]->lc.cx;
	}
	return TRUE;
}



BOOL CListCtrlEx::GetColumn(int nCol, LV_COLUMN* pColumn) const
{
	if (aColumns[nCol]->bFlags&COLUMNDATA::FlagVisible)
		return CListCtrl::GetColumn(GetVisibleColumnFromSubItem(aColumns[nCol]->lc.iSubItem),pColumn);
	else
	{
		if (pColumn->mask&LVCF_ORDER)
			pColumn->iOrder=-1;
		if (pColumn->mask&LVCF_IMAGE)
			pColumn->iImage=aColumns[nCol]->lc.mask&LVCF_IMAGE?aColumns[nCol]->lc.iImage:0;
		if (pColumn->mask&LVCF_WIDTH)
			pColumn->cx=aColumns[nCol]->lc.mask&LVCF_WIDTH?aColumns[nCol]->lc.cx:100;
		if (pColumn->mask&LVCF_SUBITEM)
			pColumn->iSubItem=-1;
		if (pColumn->mask&LVCF_FMT)
			pColumn->fmt=aColumns[nCol]->lc.mask&LVCF_FMT?aColumns[nCol]->lc.fmt:LVCFMT_LEFT;
		if (pColumn->mask&LVCF_TEXT)
		{
			if (aColumns[nCol]->bFlags&COLUMNDATA::FlagTitleIsResource)
			{
				::LoadString(aColumns[nCol]->nTitleID,pColumn->pszText,
					pColumn->cchTextMax,aColumns[nCol]->bResourceType);
			}
			else
				WideCharToMultiByte(CP_ACP,0,aColumns[nCol]->pStrTitle,-1,pColumn->pszText,pColumn->cchTextMax,0,0);
		}
		return TRUE;
	}
}

BOOL CListCtrlEx::GetColumn(int nCol, LV_COLUMNW* pColumn) const
{
	if (aColumns[nCol]->bFlags&COLUMNDATA::FlagVisible)
		return CListCtrl::GetColumn(GetVisibleColumnFromSubItem(aColumns[nCol]->lc.iSubItem),pColumn);
	else
	{
		if (pColumn->mask&LVCF_ORDER)
			pColumn->iOrder=-1;
		if (pColumn->mask&LVCF_IMAGE)
			pColumn->iImage=aColumns[nCol]->lc.mask&LVCF_IMAGE?aColumns[nCol]->lc.iImage:0;
		if (pColumn->mask&LVCF_WIDTH)
			pColumn->cx=aColumns[nCol]->lc.mask&LVCF_WIDTH?aColumns[nCol]->lc.cx:100;
		if (pColumn->mask&LVCF_SUBITEM)
			pColumn->iSubItem=-1;
		if (pColumn->mask&LVCF_FMT)
			pColumn->fmt=aColumns[nCol]->lc.mask&LVCF_FMT?aColumns[nCol]->lc.fmt:LVCFMT_LEFT;
		if (pColumn->mask&LVCF_TEXT)
		{
			if (aColumns[nCol]->bFlags&COLUMNDATA::FlagTitleIsResource)
			{
				::LoadString(aColumns[nCol]->nTitleID,pColumn->pszText,
					pColumn->cchTextMax,aColumns[nCol]->bResourceType);
			}
			else
			{
				if (StringCbCopyW(pColumn->pszText,pColumn->cchTextMax*2,aColumns[nCol]->pStrTitle)!=S_OK)
					return FALSE;
			}
		}
		return TRUE;
	}
}

BOOL CListCtrlEx::SetColumn(int nCol, const LV_COLUMN* pColumn)
{
	if (aColumns[nCol]->bFlags&COLUMNDATA::FlagVisible)
		return CListCtrl::SetColumn(GetVisibleColumnFromSubItem(aColumns[nCol]->lc.iSubItem),pColumn);
	else
	{
		if (pColumn->mask&LVCF_IMAGE)
		{
			aColumns[nCol]->lc.mask|=LVCF_IMAGE;
			aColumns[nCol]->lc.iImage=pColumn->iImage;
		}
		if (pColumn->mask&LVCF_WIDTH)
		{
			aColumns[nCol]->lc.mask|=LVCF_WIDTH;
			aColumns[nCol]->lc.cx=pColumn->cx;
		}
		if (pColumn->mask&LVCF_FMT)
		{
			aColumns[nCol]->lc.mask|=LVCF_FMT;
			aColumns[nCol]->lc.fmt=pColumn->fmt;
		}
		if (pColumn->mask&LVCF_TEXT)
		{
			if (!(aColumns[nCol]->bFlags&COLUMNDATA::FlagTitleIsResource) &&
				aColumns[nCol]->pStrTitle!=NULL)
				delete[] aColumns[nCol]->pStrTitle;

			aColumns[nCol]->bFlags&=~DWORD(COLUMNDATA::FlagTitleIsResource);
			aColumns[nCol]->pStrTitle=alloccopyAtoW(pColumn->pszText);
		}
		return TRUE;
	}
}

BOOL CListCtrlEx::SetColumn(int nCol, const LV_COLUMNW* pColumn)
{
	if (aColumns[nCol]->bFlags&COLUMNDATA::FlagVisible)
		return CListCtrl::SetColumn(GetVisibleColumnFromSubItem(aColumns[nCol]->lc.iSubItem),pColumn);
	else
	{
		if (pColumn->mask&LVCF_IMAGE)
		{
			aColumns[nCol]->lc.mask|=LVCF_IMAGE;
			aColumns[nCol]->lc.iImage=pColumn->iImage;
		}
		if (pColumn->mask&LVCF_WIDTH)
		{
			aColumns[nCol]->lc.mask|=LVCF_WIDTH;
			aColumns[nCol]->lc.cx=pColumn->cx;
		}
		if (pColumn->mask&LVCF_FMT)
		{
			aColumns[nCol]->lc.mask|=LVCF_FMT;
			aColumns[nCol]->lc.fmt=pColumn->fmt;
		}
		if (pColumn->mask&LVCF_TEXT)
		{
			if (!(aColumns[nCol]->bFlags&COLUMNDATA::FlagTitleIsResource) &&
				aColumns[nCol]->pStrTitle!=NULL)
				delete[] aColumns[nCol]->pStrTitle;

			aColumns[nCol]->bFlags&=~DWORD(COLUMNDATA::FlagTitleIsResource);
			aColumns[nCol]->pStrTitle=alloccopy(pColumn->pszText);
		}
		return TRUE;
	}
}

LRESULT CALLBACK CListCtrlEx::SubclassProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam,
										   UINT_PTR uIdSubclass,DWORD_PTR dwRefData)
{
	LRESULT lRes=DefSubclassProc(hwnd,uMsg,wParam,lParam);
	NMHDR_MOUSE nm;
	
	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
		nm.code=NMX_CLICK;
		break;
	case WM_LBUTTONDBLCLK:
		nm.code=NMX_DBLCLICK;
		break;
	case WM_RBUTTONDOWN:
		nm.code=NMX_RCLICK;
		break;
	case WM_RBUTTONDBLCLK:
		nm.code=NMX_RDBLCLICK;
		break;
	case WM_MBUTTONDOWN:
		nm.code=NMX_MCLICK;
		break;
	case WM_MBUTTONDBLCLK:
		nm.code=NMX_MDBLCLICK;
		break;
	default:
		return lRes;
	}
	
	nm.pt.x=LOWORD(lParam);
	nm.pt.y=HIWORD(lParam);
	
	if (::SendMessage(hwnd,LVM_SUBITEMHITTEST,0,(LPARAM)&nm.pt)!=-1)
	{
		nm.hwndFrom=hwnd;
		nm.idFrom=::GetWindowLong(hwnd,GWL_ID);
		::SendMessage((HWND)::GetWindowLongPtr(hwnd,GWLP_HWNDPARENT),WM_NOTIFY,nm.idFrom,(LPARAM)&nm);	
	}
	return lRes;
}

#endif