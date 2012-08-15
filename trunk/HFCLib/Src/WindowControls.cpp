////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////
#include "HFCLib.h"



#ifdef DEF_WINDOWS


///////////////////////////
// Class CListBox
///////////////////////////
int CListBox::GetText(int nIndex, CStringA& rString) const
{
	int nLen=(int)::SendMessage(m_hWnd,LB_GETTEXTLEN,(WPARAM)nIndex,0);
	if (nLen==CB_ERR)
		return CB_ERR;
	LPSTR pointer=new CHAR [nLen+2];
	if (pointer==NULL)
	{
		SetHFCError(HFC_CANNOTALLOC);
		return CB_ERR;
	}
	if (::SendMessage(m_hWnd,LB_GETTEXT,(WPARAM)nIndex,(LPARAM)pointer)==CB_ERR)
	{
		delete[] pointer;
		return CB_ERR;
	}
	rString=pointer;
	delete[] pointer;
	return rString.GetLength();
}

#ifdef DEF_WCHAR
int CListBox::GetText(int nIndex, LPWSTR lpszText) const
{ 
	if (IsUnicodeSystem())
		return (int)::SendMessageW(m_hWnd,LB_GETTEXT,nIndex,(LPARAM)lpszText);

	int nLen=(int)::SendMessageA(m_hWnd,LB_GETTEXTLEN,nIndex,0);
	char* pText=new char[nLen+2];
	int ret=(int)::SendMessageA(m_hWnd,LB_GETTEXT,nIndex,(LPARAM)pText);
	if (ret!=0)
	{
		MultiByteToWideChar(CP_ACP,0,pText,(int)ret,lpszText,(DWORD)ret+2);
		lpszText[ret]=L'\0';
	}
	delete pText;
	return ret;
}

int CListBox::GetText(int nIndex, CStringW& rString) const
{
	int nLen=(int)::SendMessage(m_hWnd,LB_GETTEXTLEN,(WPARAM)nIndex,0);
	if (nLen==CB_ERR)
		return CB_ERR;

	if (IsUnicodeSystem())
	{
		LPWSTR pointer=new WCHAR [nLen+2];
		if (pointer==NULL)
		{
			SetHFCError(HFC_CANNOTALLOC);
			return CB_ERR;
		}
		if (::SendMessageW(m_hWnd,LB_GETTEXT,(WPARAM)nIndex,(LPARAM)pointer)==CB_ERR)
		{
			delete[] pointer;
			return CB_ERR;
		}
		rString.Copy(pointer,nLen);
		delete[] pointer;
		return rString.GetLength();
	}
	else
	{
		LPSTR pointer=new CHAR [nLen+2];
		if (pointer==NULL)
		{
			SetHFCError(HFC_CANNOTALLOC);
			return CB_ERR;
		}
		if (::SendMessageA(m_hWnd,LB_GETTEXT,(WPARAM)nIndex,(LPARAM)pointer)==CB_ERR)
		{
			delete[] pointer;
			return CB_ERR;
		}
		rString.Copy(pointer,nLen);
		delete[] pointer;
		return rString.GetLength();
	}
}
#endif
///////////////////////////
// Class CComboBox
///////////////////////////


int CComboBox::GetLBText(int nIndex, CStringA& rString) const
{
	int nLen=(int)::SendMessage(m_hWnd,CB_GETLBTEXTLEN,(WPARAM)nIndex,0);
	if (nLen==CB_ERR)
		return CB_ERR;
	LPSTR pointer=new CHAR [nLen+2];
	if (pointer==NULL)
	{
		SetHFCError(HFC_CANNOTALLOC);
		return CB_ERR;
	}
	if (::SendMessage(m_hWnd,CB_GETLBTEXT,(WPARAM)nIndex,(LPARAM)pointer)==CB_ERR)
	{
		delete[] pointer;
		return CB_ERR;
	}
	rString=pointer;
	delete[] pointer;
	return rString.GetLength();
}

#ifdef DEF_WCHAR
int CComboBox::GetLBText(int nIndex, LPWSTR lpszText) const
{ 
	if (IsUnicodeSystem())
		return (int)::SendMessageW(m_hWnd,CB_GETLBTEXT,nIndex,(LPARAM)lpszText);

	int nLen=(int)::SendMessageA(m_hWnd,CB_GETLBTEXTLEN,nIndex,0);
	char* pText=new char[nLen+2];
	int ret=(int)::SendMessageA(m_hWnd,CB_GETLBTEXT,nIndex,(LPARAM)pText);
	if (ret!=0)
	{
		MultiByteToWideChar(CP_ACP,0,pText,(int)ret,lpszText,(int)ret+2);
		lpszText[ret]=L'\0';
	}
	delete[] pText;
	return ret;
}

int CComboBox::GetLBText(int nIndex, CStringW& rString) const
{
	int nLen=(int)::SendMessage(m_hWnd,CB_GETLBTEXTLEN,(WPARAM)nIndex,0);
	if (nLen==CB_ERR)
		return CB_ERR;

	if (IsUnicodeSystem())
	{
		LPWSTR pointer=new WCHAR [nLen+2];
		if (pointer==NULL)
		{
			SetHFCError(HFC_CANNOTALLOC);
			return CB_ERR;
		}
		if (::SendMessageW(m_hWnd,CB_GETLBTEXT,(WPARAM)nIndex,(LPARAM)pointer)==CB_ERR)
		{
			delete[] pointer;
			return CB_ERR;
		}
		rString.Copy(pointer,nLen);
		delete[] pointer;
		return rString.GetLength();
	}
	else
	{
		LPSTR pointer=new CHAR [nLen+2];
		if (pointer==NULL)
		{
			SetHFCError(HFC_CANNOTALLOC);
			return CB_ERR;
		}
		if (::SendMessageA(m_hWnd,CB_GETLBTEXT,(WPARAM)nIndex,(LPARAM)pointer)==CB_ERR)
		{
			delete[] pointer;
			return CB_ERR;
		}
		rString.Copy(pointer,nLen);
		delete[] pointer;
		return rString.GetLength();
	}
}

int CComboBox::FindString(int nStartAfter, LPCWSTR lpszString) const
{
	if (IsUnicodeSystem())
		return (int)::SendMessageW(m_hWnd,CB_FINDSTRING,(WPARAM)nStartAfter,(LPARAM)(LPCWSTR)lpszString);
	else
		return (int)::SendMessageA(m_hWnd,CB_FINDSTRING,(WPARAM)nStartAfter,(LPARAM)(LPCSTR)W2A(lpszString));
}

int CComboBox::AddString(LPCWSTR lpszString)
{
	if (IsUnicodeSystem())
		return (int)::SendMessageW(m_hWnd,CB_ADDSTRING,0,(LPARAM)(LPCWSTR)lpszString);
	else
		return (int)::SendMessageA(m_hWnd,CB_ADDSTRING,0,(LPARAM)(LPCSTR)W2A(lpszString));
}

int CComboBox::InsertString(int nIndex,LPCWSTR lpszString)
{
	if (IsUnicodeSystem())
		return (int)::SendMessageW(m_hWnd,CB_INSERTSTRING,nIndex,(LPARAM)(LPCWSTR)lpszString);
	else
		return (int)::SendMessageA(m_hWnd,CB_INSERTSTRING,nIndex,(LPARAM)(LPCSTR)W2A(lpszString));
}

int CComboBox::Dir(UINT attr, LPCWSTR lpszWildCard)
{
	if (IsUnicodeSystem())
		return (int)::SendMessageW(m_hWnd,CB_DIR,(WPARAM)attr,(LPARAM)lpszWildCard);
	else
		return (int)::SendMessageA(m_hWnd,CB_DIR,(WPARAM)attr,(LPARAM)(LPCSTR)W2A(lpszWildCard));
}

int CComboBox::SelectString(int nStartAfter, LPCWSTR lpszString)
{
	if (IsUnicodeSystem())
		return (int)::SendMessageW(m_hWnd,CB_SELECTSTRING,(WPARAM)nStartAfter,(LPARAM)lpszString);
	else
		return (int)::SendMessageA(m_hWnd,CB_SELECTSTRING,(WPARAM)nStartAfter,(LPARAM)(LPCSTR)W2A(lpszString));
}

#endif

///////////////////////////
// Class ScrollBar
///////////////////////////

int CScrollBar::GetScrollPos() const
{
	SCROLLINFO lpsi;
	lpsi.cbSize=sizeof(SCROLLINFO);
	lpsi.fMask=SIF_POS;
	lpsi.nPos=0;
	::GetScrollInfo(m_hWnd,m_nBar,&lpsi);
	return lpsi.nPos;
}

int CScrollBar::SetScrollPos(int nPos,BOOL bRedraw)
{
	SCROLLINFO lpsi;
	lpsi.cbSize=sizeof(SCROLLINFO);
	lpsi.fMask=SIF_POS;
	lpsi.nPos=nPos;
	return ::SetScrollInfo(m_hWnd,m_nBar,&lpsi,bRedraw);
}

int CScrollBar::SetScrollPage(UINT nPage,BOOL bRedraw)
{
	SCROLLINFO lpsi;
	lpsi.cbSize=sizeof(SCROLLINFO);
	lpsi.fMask=SIF_PAGE;
	lpsi.nPage=nPage;
	return ::SetScrollInfo(m_hWnd,m_nBar,&lpsi,bRedraw);
}

void CScrollBar::GetScrollRange(LPINT lpMinPos,LPINT lpMaxPos) const
{
	SCROLLINFO lpsi;
	lpsi.cbSize=sizeof(SCROLLINFO);
	lpsi.fMask=SIF_RANGE;
	lpsi.nMin=0;
	lpsi.nMax=0;
	::GetScrollInfo(m_hWnd,m_nBar,&lpsi);
	*lpMinPos=lpsi.nMin;
	*lpMaxPos=lpsi.nMax;
}

void CScrollBar::SetScrollRange(int nMinPos,int nMaxPos,BOOL bRedraw)
{
	SCROLLINFO lpsi;
	lpsi.cbSize=sizeof(SCROLLINFO);
	lpsi.fMask=SIF_RANGE;
	lpsi.nMin=nMinPos;
	lpsi.nMax=nMaxPos;
	::SetScrollInfo(m_hWnd,m_nBar,&lpsi,bRedraw);
}

BOOL CScrollBar::SetScrollInfo(int nMin,int nMax,UINT nPage,int nPos,UINT fMask,BOOL bRedraw)
{
	SCROLLINFO lpsi;
	lpsi.cbSize=sizeof(SCROLLINFO);
	lpsi.fMask=fMask;
	lpsi.nMin=nMin;
	lpsi.nMax=nMax;
	lpsi.nPage=nPage;
	lpsi.nPos=nPos;
	return ::SetScrollInfo(m_hWnd,m_nBar,&lpsi,bRedraw);
}

BOOL CScrollBar::GetScrollInfo(LPSCROLLINFO lpScrollInfo,UINT nMask)
{
	fMemSet(lpScrollInfo,0,sizeof(SCROLLINFO));
	lpScrollInfo->cbSize=sizeof(SCROLLINFO);
	lpScrollInfo->fMask=nMask;
	return ::GetScrollInfo(m_hWnd,m_nBar,lpScrollInfo);
}

int CScrollBar::GetScrollLimit()
{
	SCROLLINFO lpsi;
	lpsi.cbSize=sizeof(SCROLLINFO);
	lpsi.fMask=SIF_RANGE;
	lpsi.nMax=0;
	::GetScrollInfo(m_hWnd,m_nBar,&lpsi);
	return lpsi.nMax;
}

#endif