////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////

#include "HFCLib.h"



#ifdef DEF_WINDOWS

//////////////////////////
// Class CGdiObject

BOOL CGdiObject::DeleteObject()
{
	if (m_hObject==NULL)
		return FALSE;
	if (::DeleteObject(m_hObject))
	{
		DebugCloseHandle(dhtGdiObject,m_hObject,STRNULL);
		m_hObject=NULL;
		return TRUE;
	}
	return FALSE;
}
///////////////////////////
// Class CBitmap

BOOL CBitmap::GetBitmapSize(CSize& sz,HDC hDC) const
{
	BOOL bRet;
	BITMAPINFO bi;
	bi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biBitCount=0;
	if (hDC==NULL)
	{
		hDC=CreateCompatibleDC(NULL);
		bRet=::GetDIBits(hDC,(HBITMAP)m_hObject,0,0,NULL,&bi,DIB_RGB_COLORS);
		DeleteDC(hDC);	
	}
	else
		bRet=::GetDIBits(hDC,(HBITMAP)m_hObject,0,0,NULL,&bi,DIB_RGB_COLORS);

	if (bRet)
	{
		sz.cx=bi.bmiHeader.biWidth;
		sz.cy=bi.bmiHeader.biHeight;
		return TRUE;
	}
	return FALSE;
}


///////////////////////////
// Class CFont

BOOL CFont::CreateFont(int nHeight,int nWidth,int nEscapement,
	int nOrientation,int nWeight,BYTE bItalic,BYTE bUnderline,
	BYTE cStrikeOut,BYTE nCharSet,BYTE nOutPrecision,
	BYTE nClipPrecision,BYTE nQuality,BYTE nPitchAndFamily,
	LPCTSTR lpszFacename)
{
	if (m_hObject!=NULL)
	{
		::DeleteObject(m_hObject);
		DebugCloseHandle(dhtGdiObject,m_hObject,STRNULL);
	}
	m_hObject=(HFONT)::CreateFont(nHeight,nWidth,nEscapement,
		nOrientation,nWeight,bItalic,bUnderline,cStrikeOut,
		nCharSet,nOutPrecision,nClipPrecision,nQuality,nPitchAndFamily,
		lpszFacename);
	DebugOpenHandle(dhtGdiObject,m_hObject,STRNULL);
	return m_hObject!=NULL;
}

BOOL CFont::CreatePointFont(int nPointSize,LPCTSTR lpszFaceName,HDC hDC)
{
	LOGFONT logFont;
	ZeroMemory(&logFont, sizeof(LOGFONT));
	logFont.lfCharSet = DEFAULT_CHARSET;
	logFont.lfHeight = nPointSize;
	StringCbCopy(logFont.lfFaceName,32,lpszFaceName);
	return CreatePointFontIndirect(&logFont, hDC);
}

BOOL CFont::CreatePointFontIndirect(const LOGFONT* lpLogFont,HDC hDC)
{
	LOGFONT logFont = *lpLogFont;
	POINT pt;
	pt.y = ::GetDeviceCaps(hDC, LOGPIXELSY) * logFont.lfHeight;
	pt.y /= 720;
	::DPtoLP(hDC, &pt, 1);
	POINT ptOrg = { 0, 0 };
	::DPtoLP(hDC, &ptOrg, 1);
	logFont.lfHeight = -abs(pt.y - ptOrg.y);
	if (hDC == NULL)
		ReleaseDC(NULL, hDC);
	return CreateFontIndirect(&logFont);
}



#ifndef _CONSOLE

///////////////////////////
// Class CDC

CDC::CDC(CWnd* pWnd)
{
	if (pWnd!=NULL)
	{
		m_hWnd=*pWnd;
		m_hDC=::GetDC(m_hWnd);
		m_nFreeMethod=CallReleaseDC;
		DebugOpenHandle(dhtGdiObject,m_hDC,STRNULL);
	}
	else 
	{
		m_hDC=NULL;
		m_hWnd=NULL;
		m_nFreeMethod=None;
	}
	m_bPrinting=FALSE;
}



BOOL CDC::CreateDC(LPCTSTR lpszDriverName,LPCTSTR lpszDeviceName,
	LPCTSTR lpszOutput,const void* lpInitData)
{
	FreeDC();

	m_hDC=::CreateDC(lpszDriverName,lpszDeviceName,lpszOutput,(CONST DEVMODE*)lpInitData);
	DebugOpenHandle(dhtGdiObject,m_hDC,STRNULL);
	if (m_hDC==NULL)
		return FALSE;
	m_hWnd=NULL;
	m_nFreeMethod=CallDeleteDC;
	return TRUE;
}

BOOL CDC::CreateIC(LPCTSTR lpszDriverName, LPCTSTR lpszDeviceName,
	LPCTSTR lpszOutput,const void* lpInitData)
{
	FreeDC();

	m_hDC=::CreateIC(lpszDriverName,lpszDeviceName,lpszOutput,(CONST DEVMODE*)lpInitData);
	if (m_hDC==NULL)
		return FALSE;
	DebugOpenHandle(dhtGdiObject,m_hDC,STRNULL);
	m_hWnd=NULL;
	m_nFreeMethod=CallDeleteDC;
	return TRUE;
}

BOOL CDC::GetDC(HWND hWnd)
{
	FreeDC();

	m_hDC=::GetDC(hWnd);
	if (m_hDC==NULL)
		return FALSE;
	DebugOpenHandle(dhtGdiObject,m_hDC,STRNULL);
	m_hWnd=hWnd;
	m_nFreeMethod=CallReleaseDC;
	return TRUE;
}

BOOL CDC::CreateCompatibleDC(HDC hDC)
{
	FreeDC();

	m_hDC=::CreateCompatibleDC(hDC);
	if (m_hDC==NULL)
		return FALSE;
	DebugOpenHandle(dhtGdiObject,m_hDC,STRNULL);
	m_hWnd=NULL;
	m_nFreeMethod=CallDeleteDC;
	return TRUE;
}

void CDC::FreeDC() 
{
	if (m_hDC==NULL)
		return;
	switch (m_nFreeMethod)
	{
	case CallReleaseDC:
		::ReleaseDC(m_hWnd,m_hDC);
		DebugCloseHandle(dhtGdiObject,m_hDC,STRNULL);
		break;
	case CallDeleteDC:
		::DeleteDC(m_hDC);
		DebugCloseHandle(dhtGdiObject,m_hDC,STRNULL);
		break;
	}	
	m_hDC=NULL;
	m_hWnd=NULL;
}

BOOL CDC::DeleteDC()
{
	if (m_hDC==NULL)
		return FALSE;
	if (!::DeleteDC(m_hDC))
		return FALSE;
	DebugCloseHandle(dhtGdiObject,m_hDC,STRNULL);
	m_hDC=NULL;
	m_hWnd=NULL;
	m_nFreeMethod=None;
	return TRUE;
}

void CDC::ReleaseDC()
{
	ASSERT_VALID(m_hDC);
	
	::ReleaseDC(m_hWnd,m_hDC);
	DebugCloseHandle(dhtGdiObject,m_hDC,STRNULL);
	m_hWnd=NULL;
	m_hDC=NULL;
	m_nFreeMethod=None;
}


BOOL CDC::ResetDC(const DEVMODE* lpDevMode)
{
	if (m_hDC==NULL)
		return FALSE;
	HDC hDC=::ResetDC(m_hDC,lpDevMode);
	DebugCloseHandle(dhtGdiObject,m_hDC,STRNULL);
	if (hDC==NULL)
		return FALSE;
	DebugOpenHandle(dhtGdiObject,hDC,STRNULL);
	m_hDC=hDC;
	return TRUE;
}

void CDC::DPtoLP(LPRECT lpRect) const
{
	CPoint p1(lpRect->left,lpRect->top);
	CPoint p2(lpRect->right,lpRect->bottom);
	::DPtoLP(m_hDC,&p1,1);
	::DPtoLP(m_hDC,&p2,1);
	lpRect->left=p1.x;
	lpRect->top=p1.y;
	lpRect->right=p2.x;
	lpRect->bottom=p2.y;
}

void CDC::DPtoLP(LPSIZE lpSize) const
{
	CPoint p1(lpSize->cx,lpSize->cy);
	::DPtoLP(m_hDC,&p1,1);
	lpSize->cx=p1.x;
	lpSize->cy=p1.y;
}

void CDC::LPtoDP(LPRECT lpRect) const
{
	CPoint p1(lpRect->left,lpRect->top);
	CPoint p2(lpRect->right,lpRect->bottom);
	::LPtoDP(m_hDC,&p1,1);
	::LPtoDP(m_hDC,&p2,1);
	lpRect->left=p1.x;
	lpRect->top=p1.y;
	lpRect->right=p2.x;
	lpRect->bottom=p2.y;
}

void CDC::LPtoDP(LPSIZE lpSize) const
{
	CPoint p1(lpSize->cx,lpSize->cy);
	::LPtoDP(m_hDC,&p1,1);
	lpSize->cx=p1.x;
	lpSize->cy=p1.y;
}

CSize CDC::TabbedTextOut(int x,int y,LPCTSTR lpszString,int nCount,
	int nTabPositions,LPINT lpnTabStopPositions,int nTabOrigin)
{
	LONG ret= ::TabbedTextOut(m_hDC,x,y,lpszString,nCount,
		nTabPositions,lpnTabStopPositions,nTabOrigin);
	return CSize(LOWORD(ret),HIWORD(ret));
}

CSize CDC::TabbedTextOut(int x,int y,const CStringA& str,int nTabPositions,
	LPINT lpnTabStopPositions,int nTabOrigin)
{
	LONG ret= ::TabbedTextOutA(m_hDC,x,y,(LPCSTR)str,(int)str.GetLength(),
		nTabPositions,lpnTabStopPositions,nTabOrigin);
	return CSize(LOWORD(ret),HIWORD(ret));
}

CSize CDC::GetOutputTextExtent(LPCTSTR lpszString,int nCount) const
{
	CSize sz,szt;
	int nPtr=0,nStartPtr=0;
	while (lpszString[nStartPtr+nPtr]!='\0' && nStartPtr+nPtr<nCount)
	{
		if (lpszString[nStartPtr+nPtr]=='\n')
		{
			::GetTextExtentPoint32(m_hDC,lpszString+nStartPtr,nPtr+1,&szt);
			if (szt.cx>sz.cx)
				sz.cx=szt.cx;
			sz.cy+=szt.cy;
			nStartPtr+=nPtr+1;
			nPtr=0;
		}
		else
			nPtr++;
	}
	::GetTextExtentPoint32(m_hDC,lpszString+nStartPtr,max(nPtr,nCount-nStartPtr)+1,&szt);
	if (szt.cx>sz.cx)
		sz.cx=szt.cx;
	sz.cy+=szt.cy;
	return sz;
}

CSize CDC::GetOutputTextExtent(const CStringA& str) const
{
	CSize sz,szt;
	LPCSTR lpszString=str;
	int nPtr=0,nStartPtr=0;
	while (lpszString[nStartPtr+nPtr]!='\0')
	{
		if (lpszString[nStartPtr+nPtr]=='\n')
		{
			::GetTextExtentPoint32A(m_hDC,lpszString+nStartPtr,nPtr+1,&szt);
			if (szt.cx>sz.cx)
				sz.cx=szt.cx;
			sz.cy+=szt.cy;
			nStartPtr+=nPtr+1;
			nPtr=0;
		}
		else
			nPtr++;
	}
	::GetTextExtentPoint32A(m_hDC,lpszString+nStartPtr,nPtr+1,&szt);
	if (szt.cx>sz.cx)
		sz.cx=szt.cx;
	sz.cy+=szt.cy;
	return sz;
}

CSize CDC::GetTabbedTextExtent(LPCTSTR lpszString,int nCount,
	int nTabPositions,LPINT lpnTabStopPositions) const
{
	DWORD ret= ::GetTabbedTextExtent(m_hDC,lpszString,nCount,
		nTabPositions,lpnTabStopPositions);
	CSize sz(LOWORD(ret),HIWORD(ret));
	return sz;
}

CSize CDC::GetTabbedTextExtent(const CStringA& str,
	int nTabPositions,LPINT lpnTabStopPositions) const
{
	DWORD ret= ::GetTabbedTextExtentA(m_hDC,(LPCSTR)str,(int)str.GetLength()+1,
		nTabPositions,lpnTabStopPositions);
	CSize sz(LOWORD(ret),HIWORD(ret));
	return sz;
}

CSize CDC::GetOutputTabbedTextExtent(LPCTSTR lpszString, int nCount,
	int nTabPositions,LPINT lpnTabStopPositions) const
{
	DWORD ret= ::GetTabbedTextExtent(m_hDC,lpszString,nCount,
		nTabPositions,lpnTabStopPositions);
	CSize sz(LOWORD(ret),HIWORD(ret));
	return sz;
}

CSize CDC::GetOutputTabbedTextExtent(const CStringA& str,
	int nTabPositions,LPINT lpnTabStopPositions) const
{
	DWORD ret= ::GetTabbedTextExtentA(m_hDC,(LPCSTR)str,(int)str.GetLength(),
		nTabPositions,lpnTabStopPositions);
	CSize sz(LOWORD(ret),HIWORD(ret));
	return sz;
}

int CDC::StartDoc(LPCTSTR lpszDocName)
{
	DOCINFO di;
	di.cbSize=sizeof(DOCINFO);
	di.lpszDocName=lpszDocName;
	di.lpszOutput=NULL;
	di.lpszDatatype=NULL;
	di.fwType=0;
	int ret=::StartDoc(m_hDC,&di);
	if (!ret)
		return FALSE;
	m_bPrinting=TRUE;
	return ret;
}

int CDC::StartDoc(LPDOCINFO lpDocInfo)
{
	int ret=::StartDoc(m_hDC,lpDocInfo);
	if (!ret)
		return FALSE;
	m_bPrinting=TRUE;
	return ret;
}

int CDC::AbortDoc()
{
	int ret=::AbortDoc(m_hDC);
	if (ret==SP_ERROR)
		return FALSE;
	m_bPrinting=FALSE;
	return ret;
}
	
int CDC::EndDoc()
{
	int ret=::EndDoc(m_hDC);
	if (ret==SP_ERROR)
		return FALSE;
	m_bPrinting=FALSE;
	return ret;
}

float CDC::GetMiterLimit() const
{
	float ft;
	::GetMiterLimit(m_hDC,&ft);
	return ft;
}

#ifdef DEF_WCHAR
CSize CDC::TabbedTextOut(int x,int y,const CStringW& str,int nTabPositions,
	LPINT lpnTabStopPositions,int nTabOrigin)
{
	LONG ret= ::TabbedTextOutW(m_hDC,x,y,(LPCWSTR)str,(int)str.GetLength(),
		nTabPositions,lpnTabStopPositions,nTabOrigin);
	return CSize(LOWORD(ret),HIWORD(ret));
}

CSize CDC::GetTextExtent(const CStringW& str) const
{
	CSize sz;
	if (IsUnicodeSystem())
		::GetTextExtentPoint32W(m_hDC,(LPCWSTR)str,(int)str.GetLength(),&sz);
	else
		::GetTextExtentPoint32A(m_hDC,W2A(str),(int)str.GetLength(),&sz);
	return sz;
}

CSize CDC::GetTextExtent(LPCWSTR lpszString,int nCount) const
{
	CSize sz;
	if (IsUnicodeSystem())
		::GetTextExtentPoint32W(m_hDC,lpszString,nCount,&sz);
	else
		::GetTextExtentPoint32(m_hDC,W2A(lpszString),nCount,&sz);
	return sz;
}

CSize CDC::GetOutputTextExtent(const CStringW& str) const
{
	CSize sz,szt;
	LPCWSTR lpszString=str;
	int nPtr=0,nStartPtr=0;
	while (lpszString[nStartPtr+nPtr]!='\0')
	{
		if (lpszString[nStartPtr+nPtr]=='\n')
		{
			::GetTextExtentPoint32W(m_hDC,lpszString+nStartPtr,nPtr+1,&szt);
			if (szt.cx>sz.cx)
				sz.cx=szt.cx;
			sz.cy+=szt.cy;
			nStartPtr+=nPtr+1;
			nPtr=0;
		}
		else
			nPtr++;
	}
	::GetTextExtentPoint32W(m_hDC,lpszString+nStartPtr,nPtr+1,&szt);
	if (szt.cx>sz.cx)
		sz.cx=szt.cx;
	sz.cy+=szt.cy;
	return sz;
}

CSize CDC::GetTabbedTextExtent(const CStringW& str,
	int nTabPositions,LPINT lpnTabStopPositions) const
{
	DWORD ret= ::GetTabbedTextExtentW(m_hDC,(LPCWSTR)str,(int)str.GetLength()+1,
		nTabPositions,lpnTabStopPositions);
	CSize sz(LOWORD(ret),HIWORD(ret));
	return sz;
}

CSize CDC::GetOutputTabbedTextExtent(const CStringW& str,
	int nTabPositions,LPINT lpnTabStopPositions) const
{
	DWORD ret= ::GetTabbedTextExtentW(m_hDC,(LPCWSTR)str,(int)str.GetLength(),
		nTabPositions,lpnTabStopPositions);
	CSize sz(LOWORD(ret),HIWORD(ret));
	return sz;
}


int CDC::GetTextFace(CStringW& rString) const
{
	int ret;
	if (IsUnicodeSystem())
	{
		ret=::GetTextFaceW(m_hDC,2000,rString.GetBuffer(2000));
		rString.FreeExtra();
	}
	else
	{
		char szFace[2000];
		ret=::GetTextFaceA(m_hDC,2000,szFace);
		rString.Copy(szFace,ret);
	}
	return ret;
}


#endif


///////////////////////////
// Class CPaintDC
///////////////////////////

CPaintDC::CPaintDC(CWnd* pWnd)
{
	m_hWnd=pWnd->GetHandle();
	m_hDC=BeginPaint(m_hWnd,&m_ps);
	DebugOpenHandle(dhtGdiObject,m_hDC,STRNULL);
}

CPaintDC::~CPaintDC()
{
	EndPaint(m_hWnd,&m_ps);
	DebugCloseHandle(dhtGdiObject,m_hDC,STRNULL);
}

///////////////////////////
// Class CMenu
///////////////////////////

BOOL CMenu::DestroyMenu()
{
	if (m_hMenu==NULL)
		return FALSE;
	if (!::DestroyMenu(m_hMenu))
		return FALSE;
	DebugCloseHandle(dhtMenu,m_hMenu,STRNULL);
	m_hMenu=NULL;
	return TRUE;
}

int CMenu::GetMenuString(UINT nIDItem, CStringA& rString, UINT nFlags) const
{
	int len=::GetMenuString(m_hMenu,nIDItem,NULL,0,nFlags);
	LPSTR szBuffer=new CHAR[len+2];
	if (szBuffer!=NULL)
	{
		DebugMessage("CMenu::GetMenuString(): szBuffer is NULL");
		SetHFCError(HFC_CANNOTALLOC);
		return -1;
	}
	len=::GetMenuStringA(m_hMenu,nIDItem,szBuffer,len+1,nFlags);
	rString=szBuffer;	
	delete[] szBuffer;
	return len;
}

#ifdef DEF_WCHAR
int CMenu::GetMenuString(UINT nIDItem, LPWSTR lpString, int nMaxCount, UINT nFlags) const
{
	if (IsUnicodeSystem())
		return ::GetMenuStringW(m_hMenu,nIDItem,lpString,nMaxCount,nFlags);
	else
	{
		char* aString=new char[nMaxCount+2];
		int ret=::GetMenuStringA(m_hMenu,nIDItem,aString,nMaxCount,nFlags);
		MultiByteToWideChar(CP_ACP,0,aString,ret+1,lpString,nMaxCount);
		delete[] aString;
		return ret;
	}
}

int CMenu::GetMenuString(UINT nIDItem, CStringW& rString, UINT nFlags) const
{
	int len=::GetMenuString(m_hMenu,nIDItem,NULL,0,nFlags);
	if (IsUnicodeSystem())
	{
		LPWSTR szBuffer=new WCHAR[len+2];
		if (szBuffer!=NULL)
		{
			DebugMessage("CMenu::GetMenuString(): szBuffer is NULL");
			SetHFCError(HFC_CANNOTALLOC);
			return -1;
		}
		len=::GetMenuStringW(m_hMenu,nIDItem,szBuffer,len+1,nFlags);
		if (len>0)
			rString=szBuffer;	
		else
			rString.Empty();
		delete[] szBuffer;
	}
	else
	{
		LPSTR szBuffer=new CHAR[len+2];
		if (szBuffer!=NULL)
		{
			DebugMessage("CMenu::GetMenuString(): szBuffer is NULL");
			SetHFCError(HFC_CANNOTALLOC);
			return -1;
		}
		len=::GetMenuStringA(m_hMenu,nIDItem,szBuffer,len+1,nFlags);
		if (len>0)
			rString=szBuffer;	
		else
			rString.Empty();
		delete[] szBuffer;
	}
	return len;
}

BOOL CMenu::InsertMenu(UINT nItem,BOOL fByPosition,LPMENUITEMINFOW lpmii)
{
	if (IsUnicodeSystem())
		return ::InsertMenuItemW(m_hMenu,nItem,fByPosition,lpmii);

	if (lpmii->fMask&MIIM_STRING || lpmii->fMask&MIIM_TYPE)
	{
		if (lpmii->fType!=MFT_BITMAP && lpmii->fType!=MFT_OWNERDRAW && lpmii->fType!=MFT_SEPARATOR)
		{
			LPWSTR sTemp=lpmii->dwTypeData;
			lpmii->dwTypeData=(LPWSTR)alloccopyWtoA(sTemp);
			BOOL bRet=::InsertMenuItemA(m_hMenu,nItem,fByPosition,(MENUITEMINFOA*)lpmii);
			delete[] (LPSTR)lpmii->dwTypeData;
			lpmii->dwTypeData=sTemp;
			return bRet;
		}
	}
	
	return ::InsertMenuItemA(m_hMenu,nItem,fByPosition,(MENUITEMINFOA*)lpmii);
}

BOOL CMenu::SetMenuItemInfo(UINT uItem,BOOL fByPosition,LPMENUITEMINFOW lpmii)
{
	if (IsUnicodeSystem())
		return ::SetMenuItemInfoW(m_hMenu,uItem,fByPosition,lpmii);
		
	if (lpmii->fMask&MIIM_STRING || lpmii->fMask&MIIM_TYPE)
	{
		if (lpmii->fType!=MFT_BITMAP && lpmii->fType!=MFT_OWNERDRAW && lpmii->fType!=MFT_SEPARATOR)
		{
			LPWSTR sTemp=lpmii->dwTypeData;
			lpmii->dwTypeData=(LPWSTR)alloccopyWtoA(sTemp);
			BOOL bRet=::SetMenuItemInfoA(m_hMenu,uItem,fByPosition,(MENUITEMINFOA*)lpmii);
			delete[] (LPSTR)lpmii->dwTypeData;
			lpmii->dwTypeData=sTemp;
			return bRet;
		}
	}
	return ::SetMenuItemInfoA(m_hMenu,uItem,fByPosition,(MENUITEMINFOA*)lpmii);
}

BOOL CMenu::GetMenuItemInfo(UINT uItem,BOOL fByPosition,LPMENUITEMINFOW lpmii) const
{
	if (IsUnicodeSystem())
		return ::GetMenuItemInfoW(m_hMenu,uItem,fByPosition,lpmii);

	
	if ((lpmii->fMask&MIIM_STRING || lpmii->fMask&MIIM_TYPE) && lpmii->cch>0)
	{
		// Get type
		MENUITEMINFOA miia;
		miia.cbSize=sizeof(MENUITEMINFOA);
		miia.fMask=MIIM_FTYPE;

		if (!::GetMenuItemInfoA(m_hMenu,uItem,fByPosition,&miia))
			return FALSE;

		if (miia.fType==MFT_BITMAP || miia.fType==MFT_OWNERDRAW || miia.fType==MFT_SEPARATOR)
			return ::GetMenuItemInfoA(m_hMenu,uItem,fByPosition,(MENUITEMINFOA*)lpmii);

		ASSERT(sizeof(MENUITEMINFOA)==sizeof(MENUITEMINFOW));

		CopyMemory(&miia,lpmii,sizeof(MENUITEMINFOA));
		
		miia.dwTypeData=new char[lpmii->cch+2];
		BOOL bRet=::GetMenuItemInfoA(m_hMenu,uItem,fByPosition,&miia);
		if (bRet)
		{
			lpmii->fType=miia.fType;
			lpmii->fState=miia.fState;
			lpmii->wID=miia.wID;
			lpmii->hSubMenu=miia.hSubMenu;
			lpmii->hbmpChecked=miia.hbmpChecked;
			lpmii->hbmpUnchecked=miia.hbmpUnchecked;
			lpmii->dwItemData=miia.dwItemData;
			lpmii->cch=miia.cch;
			lpmii->hbmpItem=miia.hbmpItem;
			MemCopyAtoW(lpmii->dwTypeData,lpmii->cch+1,miia.dwTypeData,miia.cch);
		}
		delete[] miia.dwTypeData;
		return bRet;
	}
	else
		return ::GetMenuItemInfoA(m_hMenu,uItem,fByPosition,(MENUITEMINFOA*)lpmii);
}
#endif

#endif
#endif
