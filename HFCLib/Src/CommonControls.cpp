////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////

#include "HFCLib.h"



#ifdef DEF_WINDOWS

///////////////////////////
// Class CImageList
///////////////////////////

#ifdef DEF_RESOURCES
BOOL CImageList::DeleteImageList()
{
	if (m_hImageList==NULL)
		return TRUE;
	if (!ImageList_Destroy(m_hImageList))
		return FALSE;
	m_hImageList=NULL;
	return TRUE;
}

#endif

///////////////////////////
// Class CStatusBarCtrl
///////////////////////////

int CStatusBarCtrl::GetText(CString& str,int nPane,int* pType) const
{
	int ret=(int)::SendMessage(m_hWnd,SB_GETTEXT,(WPARAM)nPane,(LPARAM)str.GetBuffer(LOWORD(::SendMessage(m_hWnd,SB_GETTEXTLENGTH,(WPARAM)nPane,0))));
	if (pType!=NULL)
		*pType=HIWORD(ret);
	return ret;
}

int CStatusBarCtrl::GetText(LPSTR lpszText,int nPane,int* pType) const
{
	int ret=(int)::SendMessage(m_hWnd,SB_GETTEXT,(WPARAM)nPane,(LPARAM)lpszText);	
	if (pType!=NULL)
		*pType=HIWORD(ret);
	return LOWORD(ret);
}

int CStatusBarCtrl::GetTextLength(int nPane,int* pType) const
{
	int ret=(int)::SendMessage(m_hWnd,SB_GETTEXTLENGTH,(WPARAM)nPane,0);
	if (pType!=NULL)
		*pType=HIWORD(ret);
	return LOWORD(ret);
}

#ifdef DEF_WCHAR
int CStatusBarCtrl::GetText(CStringW& str,int nPane,int* pType) const
{
	int ret;
	int len=(int)::SendMessage(m_hWnd,SB_GETTEXTLENGTH,(WPARAM)nPane,0);
	
	if (IsUnicodeSystem())
		ret=(int)::SendMessageW(m_hWnd,SB_GETTEXTW,(WPARAM)nPane,(LPARAM)str.GetBuffer(len));
	else
	{
		char* pText=new char[len+2];
		ret=(int)::SendMessageA(m_hWnd,SB_GETTEXTA,(WPARAM)nPane,(LPARAM)pText);
		str.Copy(pText,len);
		delete[] pText;
	}

	if (pType!=NULL)
		*pType=HIWORD(ret);
	return ret;
}

int CStatusBarCtrl::GetText(LPWSTR lpszText,int nPane,int* pType) const
{
	int ret;
	if (IsUnicodeSystem())
		ret=(int)::SendMessageW(m_hWnd,SB_GETTEXTW,(WPARAM)nPane,(LPARAM)lpszText);	
	else
	{
		int len=(int)::SendMessage(m_hWnd,SB_GETTEXTLENGTH,(WPARAM)nPane,0);
		char* pText=new char[len+2];
		ret=(int)::SendMessageA(m_hWnd,SB_GETTEXTA,(WPARAM)nPane,(LPARAM)pText);
		if (ret)
			MultiByteToWideChar(CP_ACP,0,pText,(int)len+1,lpszText,(int)len+1);
		delete[] pText;
	}
	if (pType!=NULL)
		*pType=HIWORD(ret);
	return LOWORD(ret);
}

int CStatusBarCtrl::GetTipText(int n,LPWSTR szText,int nBufLen) const
{
	if (IsUnicodeSystem())
		return (int)::SendMessageW(m_hWnd,SB_GETTIPTEXTW,MAKEWPARAM(n,nBufLen),(LPARAM)szText);
	
	char* pText=new char[nBufLen+2];
	int ret=(int)::SendMessageW(m_hWnd,SB_GETTIPTEXTW,MAKEWPARAM(n,nBufLen),(LPARAM)pText);
	if (ret)
		MultiByteToWideChar(CP_ACP,0,pText,-1,szText,(int)nBufLen);
	delete[] pText;
	return ret;
}

#endif

///////////////////////////
// Class CToolTipCtrl
///////////////////////////

void CToolTipCtrl::GetText(CString& str,HWND hWnd,UINT nIDTool) const
{
	TOOLINFO ti;
	char szText[256];
	ti.cbSize=TTTOOLINFOA_V1_SIZE;
	ti.uFlags=0;
	ti.hwnd=hWnd;
	ti.uId=nIDTool;
	ti.lpszText=szText;
	::SendMessage(m_hWnd,TTM_GETTEXT,0,(LPARAM)&ti);
	str=szText;
}

BOOL CToolTipCtrl::GetToolInfo(LPTOOLINFO pToolInfo,HWND hWnd,UINT nIDTool) const
{
	pToolInfo->cbSize=TTTOOLINFOA_V1_SIZE;
	pToolInfo->hwnd=hWnd;
	pToolInfo->uId=nIDTool;
	return (BOOL)::SendMessage(m_hWnd,TTM_GETTOOLINFO,0,(LPARAM)pToolInfo);
}

void CToolTipCtrl::SetToolRect(HWND hWnd,UINT nIDTool,LPCRECT lpRect)
{
	TOOLINFO ti;
	ti.cbSize=TTTOOLINFOA_V1_SIZE;
	ti.uFlags=0;
	ti.hwnd=hWnd;
	ti.uId=nIDTool;
	ti.rect=*lpRect;
	::SendMessage(m_hWnd,TTM_NEWTOOLRECT,0,(LPARAM)&ti);
}

#ifdef DEF_RESOURCES
BOOL CToolTipCtrl::AddTool(HWND hWnd,UINT nIDText,LPCRECT lpRectTool,UINT nIDTool,LPARAM lParam)
{
	if (IsUnicodeSystem())
	{
		TOOLINFOW ti;
		ti.cbSize=TTTOOLINFOW_V2_SIZE;
		ti.uFlags=TTF_SUBCLASS;
		ti.hwnd=hWnd;
		ti.uId=nIDTool;
		ti.hinst=GetLanguageSpecificResourceHandle();
		ti.lpszText=MAKEINTRESOURCEW(nIDText);
		ti.lParam=lParam;	
		if (lpRectTool!=NULL)
			ti.rect=*lpRectTool;
		else if (hWnd!=NULL)
			::GetClientRect(hWnd,&ti.rect);
		else
		{
			ti.rect.left=0;
			ti.rect.right=0;
			ti.rect.top=0;
			ti.rect.bottom=0;
		}
		return (BOOL)::SendMessageW(m_hWnd,TTM_ADDTOOLW,0,(LPARAM)&ti);
	}
	else
	{
		TOOLINFO ti;
		ti.cbSize=TTTOOLINFOA_V2_SIZE;
		ti.uFlags=TTF_SUBCLASS;
		ti.hwnd=hWnd;
		ti.uId=nIDTool;
		ti.hinst=GetLanguageSpecificResourceHandle();
		ti.lpszText=MAKEINTRESOURCE(nIDText);
		ti.lParam=lParam;	
		if (lpRectTool!=NULL)
			ti.rect=*lpRectTool;
		else if (hWnd!=NULL)
			::GetClientRect(hWnd,&ti.rect);
		else
		{
			ti.rect.left=0;
			ti.rect.right=0;
			ti.rect.top=0;
			ti.rect.bottom=0;
		}
		return (BOOL)::SendMessage(m_hWnd,TTM_ADDTOOL,0,(LPARAM)&ti);
	}
}
#endif

BOOL CToolTipCtrl::AddTool(HWND hWnd,LPCSTR lpszText,LPCRECT lpRectTool,UINT nIDTool,LPARAM lParam)
{
	TOOLINFO ti;
	ti.cbSize=TTTOOLINFOA_V2_SIZE;
	ti.uFlags=TTF_SUBCLASS;
	ti.hwnd=hWnd;
	ti.uId=nIDTool;
	ti.lpszText=(LPSTR)lpszText;
	ti.lParam=lParam;	
	if (lpRectTool!=NULL)
		ti.rect=*lpRectTool;
	else if (hWnd!=NULL)
		::GetClientRect(hWnd,&ti.rect);
	else
	{
		ti.rect.left=0;
		ti.rect.right=0;
		ti.rect.top=0;
		ti.rect.bottom=0;
	}
	return (BOOL)::SendMessage(m_hWnd,TTM_ADDTOOL,0,(LPARAM)&ti);
}

void CToolTipCtrl::DelTool(HWND hWnd,UINT nIDTool)
{
	TOOLINFO ti;
	ti.cbSize=TTTOOLINFOA_V1_SIZE;
	ti.uFlags=0;
	ti.hwnd=hWnd;
	ti.uId=nIDTool;
	::SendMessage(m_hWnd,TTM_DELTOOL,0,(LPARAM)&ti);
}

void CToolTipCtrl::DelTool(HWND hWnd,HWND hToolWnd)
{
	TOOLINFO ti;
	ti.cbSize=TTTOOLINFOA_V1_SIZE;
	ti.uFlags=TTF_IDISHWND;
	ti.hwnd=hWnd;
	ti.uId=(UINT_PTR)hToolWnd;
	::SendMessage(m_hWnd,TTM_DELTOOL,0,(LPARAM)&ti);
}


BOOL CToolTipCtrl::HitTest(HWND hWnd,LPPOINT pt,LPTOOLINFO lpToolInfo) const
{
	TTHITTESTINFO hi;
	hi.hwnd=hWnd;
	hi.pt=*pt;
	hi.ti.cbSize=TTTOOLINFOA_V1_SIZE;
	BOOL nRet=(BOOL)::SendMessage(m_hWnd,TTM_HITTEST,0,(LPARAM)&hi);
	*lpToolInfo=hi.ti;
	return nRet;
}

void CToolTipCtrl::UpdateTipText(LPCSTR lpszText,HWND hWnd,UINT nIDTool)
{
	TOOLINFO ti;
	char szText[256];
	ti.cbSize=TTTOOLINFOA_V1_SIZE;
	ti.uFlags=TTF_SUBCLASS;
	ti.hwnd=hWnd;
	ti.uId=nIDTool;
	ti.lpszText=szText;
	StringCbCopy(szText,256,lpszText);
	::SendMessage(m_hWnd,TTM_UPDATETIPTEXT,0,(LPARAM)&ti);
}

#ifdef DEF_RESOURCES
void CToolTipCtrl::UpdateTipText(UINT nIDText,HWND hWnd,UINT nIDTool)
{
	if (IsUnicodeSystem())
	{
		TOOLINFOW ti;
		ti.cbSize=TTTOOLINFOW_V1_SIZE;
		ti.uFlags=TTF_SUBCLASS;
		ti.hwnd=hWnd;
		ti.uId=nIDTool;
		ti.hinst=GetLanguageSpecificResourceHandle();
		ti.lpszText=MAKEINTRESOURCEW(nIDText);
		::SendMessageW(m_hWnd,TTM_UPDATETIPTEXTW,0,(LPARAM)&ti);
	}
	else
	{
		TOOLINFO ti;
		ti.cbSize=TTTOOLINFOA_V1_SIZE;
		ti.uFlags=TTF_SUBCLASS;
		ti.hwnd=hWnd;
		ti.uId=nIDTool;
		ti.hinst=GetLanguageSpecificResourceHandle();
		ti.lpszText=MAKEINTRESOURCE(nIDText);
		::SendMessage(m_hWnd,TTM_UPDATETIPTEXT,0,(LPARAM)&ti);
	}
}
#endif


#ifdef DEF_WCHAR
void CToolTipCtrl::GetText(CStringW& str,HWND hWnd,UINT nIDTool) const
{
	TOOLINFOW ti;
	WCHAR szText[256];
	ti.cbSize=TTTOOLINFOW_V1_SIZE;
	ti.uFlags=0;
	ti.hwnd=hWnd;
	ti.uId=nIDTool;
	ti.lpszText=szText;
	::SendMessage(m_hWnd,TTM_GETTEXTW,0,(LPARAM)&ti);
	str=szText;
}

BOOL CToolTipCtrl::GetToolInfo(LPTOOLINFOW pToolInfo,HWND hWnd,UINT nIDTool) const
{
	pToolInfo->cbSize=TTTOOLINFOW_V1_SIZE;
	pToolInfo->hwnd=hWnd;
	pToolInfo->uId=nIDTool;
	return (BOOL)::SendMessage(m_hWnd,TTM_GETTOOLINFOW,0,(LPARAM)pToolInfo);
}

BOOL CToolTipCtrl::AddTool(HWND hWnd,LPCWSTR lpszText,LPCRECT lpRectTool,UINT nIDTool,LPARAM lParam)
{
	TOOLINFOW ti;
	ti.cbSize=TTTOOLINFOW_V2_SIZE;
	ti.uFlags=TTF_SUBCLASS;
	ti.hwnd=hWnd;
	ti.uId=nIDTool;
	ti.lpszText=(LPWSTR)lpszText;
	ti.lParam=lParam;	
	if (lpRectTool!=NULL)
		ti.rect=*lpRectTool;
	else if (hWnd!=NULL)
		::GetClientRect(hWnd,&ti.rect);
	else
	{
		ti.rect.left=0;
		ti.rect.right=0;
		ti.rect.top=0;
		ti.rect.bottom=0;
	}
	return (BOOL)::SendMessage(m_hWnd,TTM_ADDTOOLW,0,(LPARAM)&ti);
}

void CToolTipCtrl::UpdateTipText(LPCWSTR lpszText,HWND hWnd,UINT nIDTool)
{
	TOOLINFOW ti;
	WCHAR szText[256];
	ti.cbSize=TTTOOLINFOW_V1_SIZE;
	ti.uFlags=TTF_SUBCLASS;
	ti.hwnd=hWnd;
	ti.uId=nIDTool;
	ti.lpszText=szText;
	StringCbCopyW(szText,256*2,lpszText);
	::SendMessage(m_hWnd,TTM_UPDATETIPTEXTW,0,(LPARAM)&ti);
}


#endif

///////////////////////////
// Class CReBarCtrl
/////////////////////


///////////////////////////
// Class CToolBarCtrl
///////////////////////////

BOOL CToolBarCtrl::Create(DWORD dwStyle,const RECT* rect,HWND hWndParent,UINT nID)
{
	m_hWnd=CreateWindowEx(0L,TOOLBARCLASSNAME,szEmpty,
      dwStyle,rect->left,rect->top,rect->right-rect->left,rect->bottom-rect->top,
      hWndParent,(HMENU)(ULONG_PTR)nID,GetInstanceHandle(),NULL);
	if (m_hWnd==NULL)
		return FALSE;
	::SendMessage(m_hWnd,TB_BUTTONSTRUCTSIZE,sizeof(TBBUTTON),0);
	return TRUE;
}

#ifdef DEF_RESOURCES
int CToolBarCtrl::AddBitmap(int nNumButtons,UINT nBitmapID)
{
	TBADDBITMAP ab;
	ab.hInst=GetLanguageSpecificResourceHandle();
	ab.nID=nBitmapID;
	return (int)::SendMessage(m_hWnd,TB_ADDBITMAP,(WPARAM)nNumButtons,(LPARAM)&ab);
}
#endif

int CToolBarCtrl::AddBitmap(int nNumButtons,HBITMAP hBitmap)
{
	TBADDBITMAP ab;
	ab.hInst=NULL;
	ab.nID=(UINT_PTR)hBitmap;
	return (int)::SendMessage(m_hWnd,TB_ADDBITMAP,(WPARAM)nNumButtons,(LPARAM)&ab);
}

void CToolBarCtrl::SaveState(HKEY hKeyRoot,LPCTSTR lpszSubKey,LPCTSTR lpszValueName)
{
	TBSAVEPARAMS sp;
	sp.hkr=hKeyRoot;
	sp.pszSubKey=lpszSubKey;
	sp.pszValueName=lpszValueName;
	::SendMessage(m_hWnd,TB_SAVERESTORE,TRUE,(LPARAM)&sp);
}

void CToolBarCtrl::RestoreState(HKEY hKeyRoot,LPCTSTR lpszSubKey,LPCTSTR lpszValueName)
{
	TBSAVEPARAMS sp;
	sp.hkr=hKeyRoot;
	sp.pszSubKey=lpszSubKey;
	sp.pszValueName=lpszValueName;
	::SendMessage(m_hWnd,TB_SAVERESTORE,FALSE,(LPARAM)&sp);
}

///////////////////////////
// Class CSpinButtonCtrl
///////////////////////////

void CSpinButtonCtrl::GetRange(int &lower,int& upper) const
{
	int ret=(int)::SendMessage(m_hWnd,UDM_GETRANGE,0,0);
	lower=HIWORD(ret);
	upper=LOWORD(ret);
}

///////////////////////////
// Class CTabCtrl
///////////////////////////

BOOL CTabCtrl::Create(DWORD dwStyle,const RECT* rect,HWND hParentWnd,UINT nID)
{
	m_hWnd=CreateWindowEx(0L,WC_TABCONTROL,szEmpty,dwStyle,
		rect->left,rect->top,rect->right-rect->left,rect->bottom-rect->top,
		hParentWnd,(HMENU)(ULONG_PTR)nID,GetInstanceHandle(),NULL);
	if (m_hWnd==NULL)
		return FALSE;
	return TRUE;
}

CSize CTabCtrl::SetItemSize(CSize size)
{
	int ret;
	CSize temp;
	ret=(int)::SendMessage(m_hWnd,TCM_SETITEMSIZE,0,MAKELPARAM(size.cx,size.cy));
	temp.cx=LOWORD(ret);
	temp.cy=HIWORD(ret);
	return temp;
}

#ifdef DEF_WCHAR
BOOL CTabCtrl::GetItem(int nItem,TC_ITEMW* pTabCtrlItem) const
{
	if (IsUnicodeSystem())
		return (BOOL)::SendMessageW(m_hWnd,TCM_GETITEMW,(WPARAM)nItem,(LPARAM)pTabCtrlItem);
	else if (pTabCtrlItem->mask&TCIF_TEXT && pTabCtrlItem->pszText!=NULL)
	{
		WCHAR* pText=pTabCtrlItem->pszText;
		pTabCtrlItem->pszText=(WCHAR*)new CHAR[pTabCtrlItem->cchTextMax+1];
		BOOL bRet=(BOOL)::SendMessageA(m_hWnd,TCM_GETITEMA,(WPARAM)nItem,(LPARAM)pTabCtrlItem);
		if (bRet)
			MultiByteToWideChar(CP_ACP,0,(char*)pTabCtrlItem->pszText,-1,pText,pTabCtrlItem->cchTextMax);
		delete[] pTabCtrlItem->pszText;
		pTabCtrlItem->pszText=pText;
		return bRet;
	}
	else
		return (BOOL)::SendMessageA(m_hWnd,TCM_GETITEMA,(WPARAM)nItem,(LPARAM)pTabCtrlItem);
}

BOOL CTabCtrl::SetItem(int nItem,TC_ITEMW* pTabCtrlItem)
{
	if (IsUnicodeSystem())
		return (BOOL)::SendMessageW(m_hWnd,TCM_SETITEMW,(WPARAM)nItem,(LPARAM)pTabCtrlItem);
	else if (pTabCtrlItem->mask&TCIF_TEXT && pTabCtrlItem->pszText!=NULL)
	{
		WCHAR* pText=pTabCtrlItem->pszText;
		pTabCtrlItem->pszText=(WCHAR*)alloccopyWtoA(pText);
		BOOL bRet=(BOOL)::SendMessageA(m_hWnd,TCM_SETITEMA,(WPARAM)nItem,(LPARAM)pTabCtrlItem);
		delete[] pTabCtrlItem->pszText;
		pTabCtrlItem->pszText=pText;
		return bRet;
	}
	else
		return (BOOL)::SendMessageA(m_hWnd,TCM_SETITEMW,(WPARAM)nItem,(LPARAM)pTabCtrlItem);
	
}

BOOL CTabCtrl::InsertItem(int nItem,TC_ITEMW* pTabCtrlItem)
{
	if (IsUnicodeSystem())
		return (BOOL)::SendMessageW(m_hWnd,TCM_INSERTITEMW,(WPARAM)nItem,(LPARAM)pTabCtrlItem);
	else if (pTabCtrlItem->mask&TCIF_TEXT && pTabCtrlItem->pszText!=NULL)
	{
		WCHAR* pText=pTabCtrlItem->pszText;
		pTabCtrlItem->pszText=(WCHAR*)alloccopyWtoA(pText);
		BOOL bRet=(BOOL)::SendMessageA(m_hWnd,TCM_INSERTITEMA,(WPARAM)nItem,(LPARAM)pTabCtrlItem);
		delete[] pTabCtrlItem->pszText;
		pTabCtrlItem->pszText=pText;
		return bRet;
	}
	else
		return (BOOL)::SendMessageA(m_hWnd,TCM_INSERTITEMA,(WPARAM)nItem,(LPARAM)pTabCtrlItem);
}
#endif

///////////////////////////////////////////
// CRichEditCtrl
///////////////////////////////////////////

CPoint CRichEditCtrl::GetCharPos(long lChar) const
{
	CPoint pt;
	int ret=(int)::SendMessage(m_hWnd,EM_POSFROMCHAR,(WPARAM)lChar,0);
	pt.x=LOWORD(ret);
	pt.y=HIWORD(ret);
	return pt;
}

CString CRichEditCtrl::GetSelText() const
{
	CString str;
	::SendMessage(m_hWnd,EM_GETSELTEXT,0,(LPARAM)str.GetBuffer(1000));
	str.FreeExtra();
	return str;
}

BOOL CRichEditCtrl::SetAlignmentForSelection(WORD wAlignment)
{
	PARAFORMAT2 pf;

	pf.cbSize = sizeof(PARAFORMAT2);

	::SendMessage(m_hWnd,EM_GETPARAFORMAT,0,(LPARAM)&pf);
	if(!(pf.dwMask&PFM_ALIGNMENT) || pf.wAlignment!=wAlignment)
	{
		pf.dwMask=PFM_ALIGNMENT;		// only change the alignment
		pf.wAlignment=wAlignment;
		return (BOOL)::SendMessage(m_hWnd,EM_SETPARAFORMAT,0,(LPARAM) &pf);
	}
	return TRUE;
}

BOOL CRichEditCtrl::SetLineSpacingRuleForSelection(BYTE bLineSpacingRule,LONG dyLineSpacing)
{
	PARAFORMAT2 pf;

	pf.cbSize = sizeof(PARAFORMAT2);

	::SendMessage(m_hWnd,EM_GETPARAFORMAT,0,(LPARAM)&pf);
	if(!(pf.dwMask&PFM_LINESPACING) || pf.bLineSpacingRule!=bLineSpacingRule)
	{
		pf.dwMask=PFM_LINESPACING;		// only change the alignment
		pf.bLineSpacingRule=bLineSpacingRule;
		pf.dyLineSpacing=dyLineSpacing;
		return (BOOL)::SendMessage(m_hWnd,EM_SETPARAFORMAT,0,(LPARAM) &pf);
	}
	return TRUE;
}
	
BOOL CRichEditCtrl::SetEffectForSelection(DWORD dwEffects,DWORD dwMask)
{
	CHARFORMAT2 cf;
	cf.cbSize=sizeof(CHARFORMAT2);
	cf.dwMask=dwMask;
	cf.dwEffects=dwEffects;
	return (BOOL)::SendMessage(m_hWnd,EM_SETCHARFORMAT,SCF_SELECTION,(LPARAM)&cf);
}
	
BOOL CRichEditCtrl::SetUnderlineTypeForSelection(BYTE bUnderlineType)
{
	CHARFORMAT2 cf;
	cf.cbSize=sizeof(CHARFORMAT2);
	cf.dwMask=CFM_UNDERLINETYPE;
	cf.bUnderlineType=bUnderlineType;
	return (BOOL)::SendMessage(m_hWnd,EM_SETCHARFORMAT,SCF_SELECTION,(LPARAM)&cf);
}
	
///////////////////////////
// Class CListCtrl
///////////////////////////

BOOL CListCtrl::SetItem(int nItem, int nSubItem, UINT nMask, LPCTSTR lpszItem,
	int nImage, UINT nState, UINT nStateMask, LPARAM lParam)
{
	LV_ITEM li;
	li.mask=nMask;
	li.iItem=nItem;
	li.iSubItem=nSubItem;
	li.state=nState;
	li.stateMask=nStateMask;
	li.pszText=(LPSTR)lpszItem;
	li.iImage=nImage;
	li.lParam=lParam;
	return (BOOL)::SendMessage(m_hWnd,LVM_SETITEM,0,(LPARAM)&li);
}

BOOL CListCtrl::GetItemRect(int nItem, LPRECT lpRect, UINT nCode) const
{
	if (lpRect!=NULL)
		lpRect->left=nCode;
	return (BOOL)::SendMessage(m_hWnd,LVM_GETITEMRECT,(WPARAM)nItem,(LPARAM)lpRect);
}

BOOL CListCtrl::SetItemState(int nItem, UINT nState, UINT nMask)
{
	LV_ITEM li;
	ZeroMemory(&li,sizeof(LV_ITEM));
	li.mask=LVIF_STATE;
	li.iItem=nItem;
	li.iSubItem=0;
	li.state=nState;
	li.stateMask=nMask;
	return(BOOL) ::SendMessage(m_hWnd,LVM_SETITEMSTATE,nItem,(LPARAM)&li);
}	

BOOL CListCtrl::GetItemText(CString& str,int nItem, int nSubItem) const
{
	LV_ITEM li;
	ZeroMemory(&li,sizeof(LV_ITEM));
	li.mask=LVIF_TEXT;
	li.iItem=nItem;
	li.iSubItem=nSubItem;
	li.pszText=str.GetBuffer(1000);
	li.cchTextMax=1000;
	int nRet=(int)::SendMessage(m_hWnd,LVM_GETITEMTEXT,nItem,(LPARAM)&li);
	if (nRet==0)
		str.Empty();
	else 
		str.FreeExtra(nRet);
	return nRet>0;
}
	
int CListCtrl::GetItemText(int nItem, int nSubItem, LPTSTR lpszText, int nLen) const
{
	LV_ITEM li;
	ZeroMemory(&li,sizeof(LV_ITEM));
	li.mask=LVIF_TEXT;
	li.iItem=nItem;
	li.iSubItem=nSubItem;
	li.pszText=lpszText;
	li.cchTextMax=nLen;
	return (int)::SendMessage(m_hWnd,LVM_GETITEMTEXT,nItem,(LPARAM)&li);
}

BOOL CListCtrl::SetItemText(int nItem, int nSubItem, LPCTSTR lpszText)
{
	LV_ITEM li;
	ZeroMemory(&li,sizeof(LV_ITEM));
	li.mask=LVIF_TEXT;
	li.iItem=nItem;
	li.iSubItem=nSubItem;
	li.pszText=(LPSTR)lpszText;
	return (BOOL)::SendMessage(m_hWnd,LVM_SETITEMTEXT,nItem,(LPARAM)&li);
}

BOOL CListCtrl::SetItemData(int nItem, DWORD_PTR dwData)
{
	LV_ITEM li;
	li.mask=LVIF_PARAM;
	li.lParam=(LPARAM)dwData;
	li.iItem=nItem;
	li.iSubItem=0;
	return (BOOL)::SendMessage(m_hWnd,LVM_SETITEM,0,(LPARAM)&li);
}

DWORD_PTR CListCtrl::GetItemData(int nItem) const
{
	LV_ITEM li;
	li.mask=LVIF_PARAM;
	li.iItem=nItem;
	li.iSubItem=0;
	if (::SendMessage(m_hWnd,LVM_GETITEM,0,(LPARAM)&li))
		return li.lParam;
	return NULL;
}

int CListCtrl::InsertItem(int nItem, LPCSTR lpszItem)
{
	LV_ITEM li;
	li.mask=LVIF_TEXT;
	li.iItem=nItem;
	li.pszText=(LPSTR)lpszItem;
	li.iSubItem=0;
	return (int)::SendMessage(m_hWnd,LVM_INSERTITEM,0,(LPARAM)&li);
}

int CListCtrl::InsertItem(int nItem, LPCSTR lpszItem, int nImage)
{
	LV_ITEM li;
	li.mask=LVIF_TEXT|LVIF_IMAGE;
	li.iImage=nImage;
	li.iItem=nItem;
	li.iSubItem=0;
	li.pszText=(LPSTR)lpszItem;
	return (int)::SendMessage(m_hWnd,LVM_INSERTITEM,0,(LPARAM)&li);
}

int CListCtrl::HitTest(POINT pt, UINT* pFlags) const
{
	LV_HITTESTINFO lh;
	lh.pt=pt;
	if (pFlags!=NULL)
		lh.flags=*pFlags;
	if (::SendMessage(m_hWnd,LVM_HITTEST,0,(LPARAM)&lh)==-1)
		return -1;
	if (pFlags!=NULL)
		*pFlags=lh.flags;
	return lh.iItem;
}

int CListCtrl::InsertColumn(int nCol, LPCSTR lpszColumnHeading,
	int nFormat, int nWidth, int nSubItem)
{
	LV_COLUMN lc;
	lc.mask=LVCF_TEXT|LVCF_FMT;
    lc.fmt=nFormat;
	if (nWidth>=0)
	{
		lc.mask|=LVCF_WIDTH;
		lc.cx=nWidth;
	}
	lc.pszText=(LPSTR)lpszColumnHeading;
	if (nSubItem>=0)
	{
		lc.mask|=LVCF_SUBITEM;
		lc.iSubItem=nSubItem;
	}
	return (int)::SendMessage(m_hWnd,LVM_INSERTCOLUMN,nCol,(LPARAM)&lc);
}

int CListCtrl::InsertItem(UINT nMask, int nItem, LPCTSTR lpszItem, UINT nState,
	UINT nStateMask,int nImage, LPARAM lParam)
{
	LV_ITEM li;
	li.mask=nMask;
	li.iImage=nImage;
	li.iItem=nItem;
	li.state=nState;
	li.stateMask=nStateMask;
	li.iSubItem=0;
	li.pszText=(LPSTR)lpszItem;
	li.lParam=lParam;
	return (int)::SendMessage(m_hWnd,LVM_INSERTITEM,0,(LPARAM)&li);
}

BOOL CListCtrl::LoadColumnsState(HKEY hRootKey,LPCSTR lpKey,LPCSTR lpSubKey)
{
	CRegKey RegKey;
	if (lpKey==NULL)
		RegKey.m_hKey=hRootKey;
	else if (RegKey.OpenKey(hRootKey,lpKey,CRegKey::openExist|CRegKey::samRead)!=ERROR_SUCCESS)
		return FALSE;
	DWORD nDataLength=RegKey.QueryValueLength(lpSubKey);
	if (nDataLength<4)
	{
		if (lpKey==NULL)
			RegKey.m_hKey=NULL;
		return FALSE;
	}
	int* pData=(int*)new BYTE[nDataLength];
	DWORD dwRet=RegKey.QueryValue(lpSubKey,(LPSTR)pData,nDataLength);
	if (lpKey==NULL)
		RegKey.m_hKey=NULL;
	if (dwRet<nDataLength)
	{
		delete[] (BYTE*)pData;
		return FALSE;
	}

	if ((pData[0]*2+1)*sizeof(int)!=nDataLength)
	{
		delete[] (BYTE*)pData;
		return FALSE;
	}
	BOOL nOrderOK=FALSE;
	for (int i=0;i<pData[0];i++)
	{
		SetColumnWidth(i,pData[1+i]);
		if (pData[1+pData[0]+i]!=0)
			nOrderOK=TRUE;
	}	
	if (nOrderOK)
		SetColumnOrderArray(pData[0],pData+1+pData[0]);
	delete[] (BYTE*)pData;
	return TRUE;
}

BOOL CListCtrl::SaveColumnsState(HKEY hRootKey,LPCSTR lpKey,LPCSTR lpSubKey) const
{
	CRegKey RegKey;
	if (lpKey==NULL)
		RegKey.m_hKey=hRootKey;
	else if (RegKey.OpenKey(hRootKey,lpKey,CRegKey::createNew|CRegKey::samAll)!=ERROR_SUCCESS)
		return FALSE;
	LVCOLUMN lc;
	fMemSet(&lc,0,sizeof(LVCOLUMN));
	lc.mask=LVCF_WIDTH;
	
	int nColumns=GetColumnCount();
	if (nColumns==0)
	{
		if (lpKey==NULL)
			RegKey.m_hKey=NULL;
		return FALSE;
	}
	
	int* pData=new int[1+2*nColumns];
	if (pData==NULL)
	{
		if (lpKey==NULL)
			RegKey.m_hKey=NULL;
		return FALSE;
	}
	pData[0]=nColumns;
	

	for (int i=0;i<nColumns;i++)
	{
		if (::SendMessage(m_hWnd,LVM_GETCOLUMN,i,LPARAM(&lc)))
			pData[1+i]=lc.cx;
		else
			pData[1+i]=100;
	}
	
	if (!::SendMessage(m_hWnd,LVM_GETCOLUMNORDERARRAY,nColumns,LPARAM(pData+1+nColumns)))
	{
		for (int i=0;i<nColumns;i++)
			pData[1+nColumns+i]=i;
	}

	BOOL bRet=RegKey.SetValue(lpSubKey,(LPSTR)pData,sizeof(int)*(1+2*nColumns),REG_BINARY)==ERROR_SUCCESS;
	RegKey.CloseKey();
	delete[] pData;
	if (lpKey==NULL)
		RegKey.m_hKey=NULL;
	return bRet;
}


#ifdef DEF_WCHAR
BOOL CListCtrl::SetItem(int nItem, int nSubItem, UINT nMask, LPCWSTR lpszItem,
	int nImage, UINT nState, UINT nStateMask, LPARAM lParam)
{
	LV_ITEMW li;
	li.mask=nMask;
	li.iItem=nItem;
	li.iSubItem=nSubItem;
	li.state=nState;
	li.stateMask=nStateMask;
	li.pszText=(LPWSTR)lpszItem;
	li.iImage=nImage;
	li.lParam=lParam;
	return (BOOL)::SendMessage(m_hWnd,LVM_SETITEMW,0,(LPARAM)&li);
}

int CListCtrl::InsertColumn(int nCol, LPCWSTR lpszColumnHeading,
	int nFormat, int nWidth, int nSubItem)
{
	LV_COLUMNW lc;
	lc.mask=LVCF_TEXT|LVCF_FMT;
    lc.fmt=nFormat;
	if (nWidth>=0)
	{
		lc.mask|=LVCF_WIDTH;
		lc.cx=nWidth;
	}
	lc.pszText=(LPWSTR)lpszColumnHeading;
	if (nSubItem>=0)
	{
		lc.mask|=LVCF_SUBITEM;
		lc.iSubItem=nSubItem;
	}
	return (int)::SendMessage(m_hWnd,LVM_INSERTCOLUMNW,nCol,(LPARAM)&lc);
}

int CListCtrl::InsertItem(UINT nMask, int nItem, LPCWSTR lpszItem, UINT nState,
	UINT nStateMask,int nImage, LPARAM lParam)
{
	LV_ITEMW li;
	li.mask=nMask;
	li.iImage=nImage;
	li.iItem=nItem;
	li.state=nState;
	li.stateMask=nStateMask;
	li.iSubItem=0;
	li.pszText=(LPWSTR)lpszItem;
	li.lParam=lParam;
	return (int)::SendMessage(m_hWnd,LVM_INSERTITEMW,0,(LPARAM)&li);
}

BOOL CListCtrl::GetItemText(CStringW& str,int nItem, int nSubItem) const
{
	LV_ITEMW li;
	li.mask=LVIF_TEXT;
	li.iItem=nItem;
	li.iSubItem=nSubItem;
	li.pszText=str.GetBuffer(1000);
	li.cchTextMax=1000;
	int nRet=(int)::SendMessage(m_hWnd,LVM_GETITEMTEXTW,nItem,(LPARAM)&li);
	if (nRet>0)
		str.FreeExtra(nRet);
	else
		str.Empty();
	return nRet>0;
}
	
int CListCtrl::GetItemText(int nItem, int nSubItem, LPWSTR lpszText, int nLen) const
{
	LV_ITEMW li;
	li.mask=LVIF_TEXT;
	li.iItem=nItem;
	li.iSubItem=nSubItem;
	li.pszText=lpszText;
	li.cchTextMax=nLen;
	return (int)::SendMessage(m_hWnd,LVM_GETITEMTEXTW,nItem,(LPARAM)&li);
}

BOOL CListCtrl::SetItemText(int nItem, int nSubItem, LPCWSTR lpszText)
{
	LV_ITEMW li;
	li.mask=LVIF_TEXT;
	li.iItem=nItem;
	li.iSubItem=nSubItem;
	li.pszText=(LPWSTR)lpszText;
	return(BOOL) ::SendMessage(m_hWnd,LVM_SETITEMTEXTW,nItem,(LPARAM)&li);
}


int CListCtrl::InsertItem(int nItem, LPCWSTR lpszItem)
{
	LV_ITEMW li;
	li.mask=LVIF_TEXT;
	li.iItem=nItem;
	li.pszText=(LPWSTR)lpszItem;
	li.iSubItem=0;
	return (int)::SendMessage(m_hWnd,LVM_INSERTITEMW,0,(LPARAM)&li);
}

int CListCtrl::InsertItem(int nItem, LPCWSTR lpszItem, int nImage)
{
	LV_ITEMW li;
	li.mask=LVIF_TEXT|LVIF_IMAGE;
	li.iImage=nImage;
	li.iItem=nItem;
	li.iSubItem=0;
	li.pszText=(LPWSTR)lpszItem;
	return (int)::SendMessage(m_hWnd,LVM_INSERTITEMW,0,(LPARAM)&li);
}
#endif

///////////////////////////////////////////
// CTreeCtrl
///////////////////////////////////////////

CString CTreeCtrl::GetItemText(HTREEITEM hItem) const
{
	CString str;
	TV_ITEM ti;
	ti.mask=TVIF_TEXT;
	ti.hItem=hItem;
	ti.pszText=str.GetBuffer(1000);
	ti.cchTextMax=1000;
	int nRet=(int)::SendMessage(m_hWnd,TVM_GETITEM,0,(LPARAM)&ti);
	if (nRet>0)
		str.FreeExtra(nRet);
	else
		str.Empty();
	return str;
}

BOOL CTreeCtrl::GetItemImage(HTREEITEM hItem,int& nImage, int& nSelectedImage) const
{
	TV_ITEM ti;
	BOOL ret;
	ti.mask=TVIF_SELECTEDIMAGE|TVIF_IMAGE;
	ti.hItem=hItem;
	ret=(BOOL)::SendMessage(m_hWnd,TVM_GETITEM,0,(LPARAM)&ti);
	nImage=ti.iImage;
	nSelectedImage=ti.iSelectedImage;
	return ret;
}

UINT CTreeCtrl::GetItemState(HTREEITEM hItem,UINT nStateMask) const
{
	TV_ITEM ti;
	ti.mask=TVIF_STATE;
	ti.hItem=hItem;
	ti.stateMask=nStateMask;
	if (::SendMessage(m_hWnd,TVM_GETITEM,0,(LPARAM)&ti))
		return ti.state;
	return 0;
}

DWORD_PTR CTreeCtrl::GetItemData(HTREEITEM hItem) const
{
	TV_ITEM ti;
	ti.mask=TVIF_PARAM	;
	ti.hItem=hItem;
	if (::SendMessage(m_hWnd,TVM_GETITEM,0,(LPARAM)&ti))
		return ti.lParam;
	return NULL;
}

BOOL CTreeCtrl::SetItem(HTREEITEM hItem,UINT nMask,LPCSTR lpszItem,int nImage,int nSelectedImage,UINT nState,UINT nStateMask,LPARAM lParam)
{
	TV_ITEM ti;
	ti.mask=nMask;
	ti.hItem=hItem;
	ti.state=nState;
	ti.stateMask=nStateMask;
	ti.pszText=(LPSTR)lpszItem;
	ti.iImage=nImage;
	ti.iSelectedImage=nSelectedImage;
	ti.cChildren=0;
	ti.lParam=lParam;
	return (BOOL)::SendMessage(m_hWnd,TVM_SETITEM,0,(LPARAM)&ti);
}

BOOL CTreeCtrl::SetItem(HTREEITEM hItem,UINT nMask,LPCWSTR lpszItem,int nImage,int nSelectedImage,UINT nState,UINT nStateMask,LPARAM lParam)
{
	TV_ITEMW ti;
	ti.mask=nMask;
	ti.hItem=hItem;
	ti.state=nState;
	ti.stateMask=nStateMask;
	ti.pszText=(LPWSTR)lpszItem;
	ti.iImage=nImage;
	ti.iSelectedImage=nSelectedImage;
	ti.cChildren=0;
	ti.lParam=lParam;
	return (BOOL)::SendMessage(m_hWnd,TVM_SETITEMW,0,(LPARAM)&ti);
}

BOOL CTreeCtrl::SetItemText(HTREEITEM hItem,LPCSTR lpszItem)
{
	TV_ITEMA ti;
	ti.mask=TVIF_TEXT;
	ti.pszText=(LPSTR)lpszItem;
	ti.hItem=hItem;
	return (BOOL)::SendMessage(m_hWnd,TVM_SETITEMA,0,(LPARAM)&ti);
}

BOOL CTreeCtrl::SetItemText(HTREEITEM hItem,LPCWSTR lpszItem)
{
	TV_ITEMW ti;
	ti.mask=TVIF_TEXT;
	ti.pszText=(LPWSTR)lpszItem;
	ti.hItem=hItem;
	return(BOOL) ::SendMessage(m_hWnd,TVM_SETITEMW,0,(LPARAM)&ti);
}

BOOL CTreeCtrl::SetItemImage(HTREEITEM hItem,int nImage,int nSelectedImage)
{
	TV_ITEM ti;
	ti.mask=TVIF_IMAGE|TVIF_SELECTEDIMAGE;
	ti.iImage=nImage;
	ti.iSelectedImage=nSelectedImage;
	ti.hItem=hItem;
	return (BOOL)::SendMessage(m_hWnd,TVM_SETITEM,0,(LPARAM)&ti);
}

BOOL CTreeCtrl::SetItemState(HTREEITEM hItem,UINT nState,UINT nStateMask)
{
	TV_ITEM ti;
	ti.mask=TVIF_STATE;
	ti.state=nState;
	ti.stateMask=nStateMask;
	ti.hItem=hItem;
	return (BOOL)::SendMessage(m_hWnd,TVM_SETITEM,0,(LPARAM)&ti);
}

BOOL CTreeCtrl::SetItemData(HTREEITEM hItem,DWORD_PTR dwData)
{
	TV_ITEM ti;
	ti.mask=TVIF_PARAM;
	ti.lParam=dwData;
	ti.hItem=hItem;
	return (BOOL)::SendMessage(m_hWnd,TVM_SETITEM,0,(LPARAM)&ti);
}

HTREEITEM CTreeCtrl::InsertItem(UINT nMask,LPCTSTR lpszItem,int nImage,int nSelectedImage,UINT nState,UINT nStateMask,LPARAM lParam,HTREEITEM hParent,HTREEITEM hInsertAfter)
{
	TV_INSERTSTRUCT is;
	is.item.mask=nMask;
	is.item.hItem=NULL;
	is.item.state=nState;
	is.item.stateMask=nStateMask;
	is.item.pszText=(LPSTR)lpszItem;
	is.item.iImage=nImage;
	is.item.iSelectedImage=nSelectedImage;
	is.item.cChildren=0;
	is.item.lParam=lParam;
	is.hParent=hParent;
	is.hInsertAfter=hInsertAfter;
	return (HTREEITEM)::SendMessage(m_hWnd,TVM_INSERTITEM,0,(LPARAM)&is);
}

HTREEITEM CTreeCtrl::InsertItem(LPCSTR lpszItem,HTREEITEM hParent,HTREEITEM hInsertAfter)
{
	TV_INSERTSTRUCTA is;
	is.item.mask=TVIF_TEXT;
	is.item.hItem=NULL;
	is.item.pszText=(LPSTR)lpszItem;
	is.hParent=hParent;
	is.hInsertAfter=hInsertAfter;
	return (HTREEITEM)::SendMessage(m_hWnd,TVM_INSERTITEM,0,(LPARAM)&is);
}

HTREEITEM CTreeCtrl::InsertItem(LPCWSTR lpszItem,HTREEITEM hParent,HTREEITEM hInsertAfter)
{
	TV_INSERTSTRUCTW is;
	is.item.mask=TVIF_TEXT;
	is.item.hItem=NULL;
	is.item.pszText=(LPWSTR)lpszItem;
	is.hParent=hParent;
	is.hInsertAfter=hInsertAfter;
	return (HTREEITEM)::SendMessage(m_hWnd,TVM_INSERTITEMW,0,(LPARAM)&is);
}

HTREEITEM CTreeCtrl::InsertItem(LPCSTR lpszItem,int nImage,int nSelectedImage,HTREEITEM hParent,HTREEITEM hInsertAfter)
{
	TV_INSERTSTRUCT is;
	is.item.mask=TVIF_TEXT|TVIF_IMAGE;
	is.item.hItem=NULL;
	is.item.pszText=(LPSTR)lpszItem;
	is.item.iImage=nImage;
	is.item.iSelectedImage=nSelectedImage;
	is.hParent=hParent;
	is.hInsertAfter=hInsertAfter;
	return (HTREEITEM)::SendMessage(m_hWnd,TVM_INSERTITEM,0,(LPARAM)&is);
}

HTREEITEM CTreeCtrl::InsertItem(LPCWSTR lpszItem,int nImage,int nSelectedImage,HTREEITEM hParent,HTREEITEM hInsertAfter)
{
	TV_INSERTSTRUCTW is;
	is.item.mask=TVIF_TEXT|TVIF_IMAGE;
	is.item.hItem=NULL;
	is.item.pszText=(LPWSTR)lpszItem;
	is.item.iImage=nImage;
	is.item.iSelectedImage=nSelectedImage;
	is.hParent=hParent;
	is.hInsertAfter=hInsertAfter;
	return (HTREEITEM)::SendMessage(m_hWnd,TVM_INSERTITEMW,0,(LPARAM)&is);
}

HTREEITEM CTreeCtrl::HitTest(const POINT& pt,UINT* pFlags) const
{
	TV_HITTESTINFO hi;
	MemCopy(&hi.pt,&pt,sizeof(POINT));
	if (::SendMessage(m_hWnd,TVM_HITTEST,0,(LPARAM)&hi)==-1)
		return NULL;
	if (pFlags!=NULL)
		*pFlags=hi.flags;
	return hi.hItem;
}

HTREEITEM CTreeCtrl::GetNextItem(HTREEITEM hItem) const
{
	if (hItem==NULL)
		return GetRootItem();

	// First check childs
	HTREEITEM hNewItem=GetNextItem(hItem,TVGN_CHILD);
	if (hNewItem!=NULL)
	{
		// Item has childs, return first child
		return hNewItem;
	}

	// Check next item
	hNewItem=GetNextItem(hItem,TVGN_NEXT);
	if (hNewItem!=NULL)
	{
		// Item has siblings below, return first such sibling
		return hNewItem;
	}

	// Check parent
	HTREEITEM hParent=GetParentItem(hItem);
	while (hParent!=NULL)
	{
		hNewItem=GetNextItem(hParent,TVGN_NEXT);
		if (hNewItem!=NULL)
			return hNewItem;

		// Get parent to parent
		hParent=GetParentItem(hParent);
	}

	return NULL;
}

	
HTREEITEM CTreeCtrl::GetPrevItem(HTREEITEM hItem) const
{
	if (hItem==NULL)
		return GetLastItem();

	// First check sibling on above
	HTREEITEM hSibling=GetNextItem(hItem,TVGN_PREVIOUS);
	if (hSibling!=NULL)
	{
		// Item has siblings above
		for (;;)
		{
			// Now check wheter this sibling has childs
			HTREEITEM hChild=GetNextItem(hSibling,TVGN_CHILD);
			if (hChild==NULL)
			{
				// No childs, return this sibling
				return hSibling;
			}

			// Get last child to hSibling
			do {
				hSibling=hChild;
			}
			while ((hChild=GetNextItem(hChild,TVGN_NEXT))!=NULL);
				
		}		
	}

	// Return parent item
	return GetParentItem(hItem);
}

HTREEITEM CTreeCtrl::FindItem(LPCSTR pText,BOOL bBackwardDirection,BOOL bPartial,HTREEITEM hItem,HTREEITEM hEnd) const
{
	TV_ITEMA ti;
	char szBuffer[1000];
	ti.mask=TVIF_TEXT;
	ti.hItem=hItem;
	ti.pszText=szBuffer;
	ti.cchTextMax=1000;

	LPSTR pTextMod;
	
	if (bPartial)
	{
		int iStrLen=istrlen(pText);
		pTextMod=new char[iStrLen+3];
		if (pText[0]!='*')
		{
			pTextMod[0]='*';
			MemCopy(pTextMod+1,pText,iStrLen++);
		}
		else
			MemCopy(pTextMod,pText,iStrLen);
		if (pTextMod[iStrLen-1]!='*')
			pTextMod[iStrLen++]='*';
		pTextMod[iStrLen]='\0';
		MakeLower(pTextMod);
	}
		
	while ((ti.hItem=bBackwardDirection?GetPrevItem(ti.hItem):GetNextItem(ti.hItem))!=NULL)
	{
		// Get next item
		if (ti.hItem==hEnd)
			return NULL;

		// Check text
		if (!GetItem(&ti))
			continue;

		

		if (bPartial)
		{
			MakeLower(szBuffer);
			if (ContainString(szBuffer,pTextMod))
			{
				delete[] pTextMod;
				return ti.hItem;
			}
		}
		else if (_stricmp(szBuffer,pText)==0)
			return ti.hItem;		
	}

	if (bPartial)
		delete[] pTextMod;
	return NULL;
}


HTREEITEM CTreeCtrl::FindItem(LPCWSTR pText,BOOL bBackwardDirection,BOOL bPartial,HTREEITEM hItem,HTREEITEM hEnd) const
{
	TV_ITEMW ti;
	WCHAR szBuffer[1000];
	ti.mask=TVIF_TEXT;
	ti.hItem=hItem;
	ti.pszText=szBuffer;
	ti.cchTextMax=1000;

	LPWSTR pTextMod;
	
	if (bPartial)
	{
		int iStrLen=istrlen(pText);
		pTextMod=new WCHAR[iStrLen+3];
		if (pText[0]!='*')
		{
			pTextMod[0]='*';
			MemCopyW(pTextMod+1,pText,iStrLen++);
		}
		else
			MemCopyW(pTextMod,pText,iStrLen);
		if (pTextMod[iStrLen-1]!='*')
			pTextMod[iStrLen++]='*';
		pTextMod[iStrLen]='\0';
		MakeLower(pTextMod);
	}
		
	while ((ti.hItem=bBackwardDirection?GetPrevItem(ti.hItem):GetNextItem(ti.hItem))!=NULL)
	{
		// Get next item
		if (ti.hItem==hEnd)
			return NULL;

		// Check text
		if (!GetItem(&ti))
			continue;

		

		if (bPartial)
		{
			MakeLower(szBuffer);
			if (ContainString(szBuffer,pTextMod))
			{
				delete[] pTextMod;
				return ti.hItem;
			}
		}
		else if (_wcsicmp(szBuffer,pText)==0)
			return ti.hItem;		
	}

	if (bPartial)
		delete[] pTextMod;
	return NULL;
}

HTREEITEM CTreeCtrl::GetLastItem() const
{
	// Begin from the root item
	HTREEITEM hItem=GetRootItem(),hTemp;
	
	for (;;)
	{
		// Find last sibling
		while ((hTemp=GetNextItem(hItem,TVGN_NEXT))!=NULL)
			hItem=hTemp;

		hTemp=GetNextItem(hItem,TVGN_CHILD);
		if (hTemp==NULL)
		{
			// No childs, return this item
			return hItem;
		}
		
		// Check all childs
		hItem=hTemp;
	}
}


///////////////////////////////////////////
// CComboBoxEx
///////////////////////////////////////////

BOOL CComboBoxEx::Create(DWORD dwStyle, const RECT* rect, HWND hParentWnd, UINT nID)
{
	CCommonCtrl::m_hWnd=CreateWindowEx(0L,WC_COMBOBOXEX,szEmpty,dwStyle,
		rect->left,rect->top,rect->right-rect->left,rect->bottom-rect->top,
		hParentWnd,(HMENU)(ULONG_PTR)nID,GetInstanceHandle(),NULL);
	if (CCommonCtrl::m_hWnd==NULL)
		return FALSE;
	CComboBox::m_hWnd=(HWND)::SendMessage(CCommonCtrl::m_hWnd,CBEM_GETCOMBOCONTROL,0,0);
	CEdit::m_hWnd=(HWND)::SendMessage(CCommonCtrl::m_hWnd,CBEM_GETEDITCONTROL,0,0);
	return TRUE;
}

int CComboBoxEx::InsertItem(LPCSTR pszText,int iImage,int iSelectedImage,int iOverlay,int iIndent,LPARAM lParam,UINT mask)
{
	COMBOBOXEXITEM cbei;
	if (mask)
		cbei.mask=mask;
	else
	{
		cbei.mask=CBEIF_LPARAM;
		if (pszText!=NULL)
			cbei.mask|=CBEIF_TEXT;
		if (iImage!=-1)
			cbei.mask|=CBEIF_IMAGE;
		if (iSelectedImage!=-1)
			cbei.mask|=CBEIF_IMAGE;
		if (iOverlay!=-1)
			cbei.mask|=CBEIF_OVERLAY;
		if (iIndent!=-1)
			cbei.mask|=CBEIF_INDENT;
	}
	cbei.iItem=::SendMessage(CComboBox::m_hWnd,CB_GETCOUNT,0,0);
	cbei.iImage=iImage;
	cbei.iSelectedImage=iSelectedImage;
	cbei.iOverlay=iOverlay;
	cbei.iIndent=iIndent;
	cbei.lParam=lParam;
	cbei.pszText=(LPSTR)pszText;
	return (int)::SendMessage(CCommonCtrl::m_hWnd,CBEM_INSERTITEM,0,(LPARAM)&cbei);
}
	
int CComboBoxEx::InsertItem(LPCWSTR pszText,int iImage,int iSelectedImage,int iOverlay,int iIndent,LPARAM lParam,UINT mask)
{
	COMBOBOXEXITEMW cbei;
	if (mask)
		cbei.mask=mask;
	else
	{
		cbei.mask=CBEIF_LPARAM;
		if (pszText!=NULL)
			cbei.mask|=CBEIF_TEXT;
		if (iImage!=-1)
			cbei.mask|=CBEIF_IMAGE;
		if (iSelectedImage!=-1)
			cbei.mask|=CBEIF_IMAGE;
		if (iOverlay!=-1)
			cbei.mask|=CBEIF_OVERLAY;
		if (iIndent!=-1)
			cbei.mask|=CBEIF_INDENT;
	}
	cbei.iItem=::SendMessage(CComboBox::m_hWnd,CB_GETCOUNT,0,0);
	cbei.iImage=iImage;
	cbei.iSelectedImage=iSelectedImage;
	cbei.iOverlay=iOverlay;
	cbei.iIndent=iIndent;
	cbei.lParam=lParam;
	cbei.pszText=(LPWSTR)pszText;
	return (int)::SendMessage(CCommonCtrl::m_hWnd,CBEM_INSERTITEMW,0,(LPARAM)&cbei);
}
	
CString CComboBoxEx::GetItemText(int nItem) const
{
	char szBuffer[2000];
	COMBOBOXEXITEM ce;
	ce.iItem=nItem;
	ce.cchTextMax=2000;
	ce.pszText=szBuffer;
	ce.mask=CBEIF_TEXT;
	if (::SendMessage(CCommonCtrl::m_hWnd,CBEM_GETITEM,0,(LPARAM)&ce))
		return CString(szBuffer);
	return CString();
}

CStringW CComboBoxEx::GetItemTextW(int nItem) const
{
	if (IsUnicodeSystem())
	{
		COMBOBOXEXITEMW ce;
		WCHAR szBuffer[2000];
		ce.iItem=nItem;
		ce.cchTextMax=2000;
		ce.pszText=szBuffer;
		ce.mask=CBEIF_TEXT;
		if (::SendMessageW(CCommonCtrl::m_hWnd,CBEM_GETITEMW,0,(LPARAM)&ce))
			return CStringW(szBuffer);
	}
	else
	{
		COMBOBOXEXITEM ce;
		char szBuffer[2000];
		ce.iItem=nItem;
		ce.cchTextMax=2000;
		ce.pszText=szBuffer;
		ce.mask=CBEIF_TEXT;
		if (::SendMessageA(CCommonCtrl::m_hWnd,CBEM_GETITEMA,0,(LPARAM)&ce))
			return CStringW(szBuffer);
	}
	return CStringW();
}

int CComboBoxEx::GetItemText(int nItem,LPSTR lpszText,int nLen) const
{
	COMBOBOXEXITEM ce;
	ce.iItem=nItem;
	ce.cchTextMax=nLen;
	ce.pszText=lpszText;
	ce.mask=CBEIF_TEXT;
	return (int)::SendMessage(CCommonCtrl::m_hWnd,CBEM_GETITEM,0,(LPARAM)&ce);
}

int CComboBoxEx::GetItemText(int nItem,LPWSTR lpszText,int nLen) const
{
	if (IsUnicodeSystem())
	{
		COMBOBOXEXITEMW ce;
		ce.iItem=nItem;
		ce.cchTextMax=nLen;
		ce.pszText=lpszText;
		ce.mask=CBEIF_TEXT;
		return (int)::SendMessageW(CCommonCtrl::m_hWnd,CBEM_GETITEMW,0,(LPARAM)&ce);
	}
	else
	{
		COMBOBOXEXITEM ce;
		ce.iItem=nItem;
		ce.cchTextMax=nLen;
		ce.pszText=new char[nLen+1];
		ce.mask=CBEIF_TEXT;
		int nRet=(int)::SendMessageA(CCommonCtrl::m_hWnd,CBEM_GETITEMA,0,(LPARAM)&ce);

		if (nRet)
			MultiByteToWideChar(CP_ACP,0,ce.pszText,-1,lpszText,nLen);
		delete[] ce.pszText;
		return nRet;
	}
}

BOOL CComboBoxEx::SetItemText(int nItem,LPCSTR lpszText)
{
	COMBOBOXEXITEM ce;
	ce.pszText=(LPSTR)lpszText;
	ce.iItem=nItem;
	ce.mask=CBEIF_TEXT;
	return(BOOL) ::SendMessageA(CCommonCtrl::m_hWnd,CBEM_SETITEMA,0,(LPARAM)&ce);
}


BOOL CComboBoxEx::SetItemText(int nItem,LPCWSTR lpszText)
{
	if (IsUnicodeSystem())
	{
		COMBOBOXEXITEMW ce;
		ce.pszText=(LPWSTR)lpszText;
		ce.iItem=nItem;
		ce.mask=CBEIF_TEXT;
		return (BOOL)::SendMessageW(CCommonCtrl::m_hWnd,CBEM_SETITEMW,0,(LPARAM)&ce);
	}
	else
	{
		COMBOBOXEXITEM ce;
		CString str(lpszText);
		ce.pszText=(LPSTR)(LPCSTR)str;
		ce.iItem=nItem;
		ce.mask=CBEIF_TEXT;
		return (BOOL)::SendMessage(CCommonCtrl::m_hWnd,CBEM_SETITEM,0,(LPARAM)&ce);
	}
}

DWORD_PTR CComboBoxEx::GetItemData(int nIndex) const
{
	COMBOBOXEXITEM ce;
	ce.mask=CBEIF_LPARAM;
	ce.iItem=nIndex;
	if (!::SendMessage(CCommonCtrl::m_hWnd,CBEM_GETITEM,0,(LPARAM)&ce))
		return 0;
	return ce.lParam;
}

int CComboBoxEx::SetItemData(int nIndex, DWORD_PTR dwItemData)
{
	COMBOBOXEXITEM ce;
	ce.mask=CBEIF_LPARAM;
	ce.iItem=nIndex;
	ce.lParam=dwItemData;
	return (int)::SendMessage(CCommonCtrl::m_hWnd,CBEM_SETITEM,0,(LPARAM)&ce);
}

void* CComboBoxEx::GetItemDataPtr(int nIndex) const
{
	COMBOBOXEXITEM ce;
	ce.mask=CBEIF_LPARAM;
	ce.iItem=nIndex;
	if (!::SendMessage(CCommonCtrl::m_hWnd,CBEM_GETITEM,0,(LPARAM)&ce))
		return 0;
	return (void*)ce.lParam;
}


#endif