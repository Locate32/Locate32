////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////

#include "HFCLib.h"
#include <vsstyle.h>

#if defined(DEF_RESOURCES) && defined(DEF_WINDOWS)

///////////////////////////
// Class CDialog
///////////////////////////

CDialog::CDialog(LPCSTR lpTemplate)
	: CTargetWnd(NULL)
{
	if (IS_INTRESOURCE(lpTemplate))
		m_lpszTemplateName=MAKEINTRESOURCE(lpTemplate);
	else if (IsUnicodeSystem())
		m_lpszTemplateNameW=alloccopyAtoW(lpTemplate);
	else
		m_lpszTemplateName=alloccopy(lpTemplate);
}

CDialog::CDialog(LPCWSTR lpTemplate)
	: CTargetWnd(NULL)
{
	if (IS_INTRESOURCE(lpTemplate))
		m_lpszTemplateName=MAKEINTRESOURCE(lpTemplate);
	else if (IsUnicodeSystem())
		m_lpszTemplateNameW=alloccopy(lpTemplate);
	else
		m_lpszTemplateName=alloccopyWtoA(lpTemplate);
}

CDialog::~CDialog()
{
	if (m_lpszTemplateName!=NULL && !IS_INTRESOURCE(m_lpszTemplateName))
	{
		if (IsUnicodeSystem())
			delete[] m_lpszTemplateNameW;
		else
			delete[] m_lpszTemplateName;
	}
}

BOOL CDialog::OnInitDialog(HWND hwndFocus)
{
	AddDebugMenuItems(*this);
	
	return FALSE;
}

LRESULT CDialog::WindowProc(UINT msg,WPARAM wParam,LPARAM lParam)
{
#ifdef _DEBUG
	void DebugCommandsProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	DebugCommandsProc(*this,msg,wParam,lParam);
#endif
	return FALSE;
}


///////////////////////////////////////////
// CPropertyPage
///////////////////////////////////////////

CPropertyPage::~CPropertyPage()
{
	if (IsUnicodeSystem())
	{
		if (m_pspw.pszTitle!=NULL)
			delete[] m_pspw.pszTitle;
	}
	else
	{
		if (m_psp.pszTitle!=NULL)
			delete[] m_psp.pszTitle;
	}
}

/*

UINT CALLBACK PropertySheetPageProcA(HWND hWnd,UINT uMsg,LPPROPSHEETPAGEA  ppsp)
{
	switch (uMsg)
	{
	case PSPCB_CREATE:
		return 1;
	case PSPCB_RELEASE:
		break;
	}
	return 0;
}

UINT CALLBACK PropertySheetPageProcW(HWND hWnd,UINT uMsg,LPPROPSHEETPAGEW  ppsp)
{
	switch (uMsg)
	{
	case PSPCB_CREATE:
		return 1;
	case PSPCB_RELEASE:
		break;
	}
	return 0;
}
*/

void CPropertyPage::Construct(UINT nIDCaption,TypeOfResourceHandle bType)
{
	if (IsUnicodeSystem())
	{
		ZeroMemory(&m_pspw,sizeof(PROPSHEETPAGEW));
		m_pspw.dwSize=sizeof(PROPSHEETPAGEW);
		m_pspw.dwFlags=PSP_DEFAULT /* |PSP_USECALLBACK */;
		if (m_lpszTemplateNameW!=NULL)
		{
			m_pspw.hInstance=GetResourceHandle(bType);
			m_pspw.pszTemplate=m_lpszTemplateNameW;
		}
		m_pspw.pfnDlgProc=(DLGPROC)CAppData::PropPageWndProc;
		m_pspw.lParam=(LPARAM)this;
		if (nIDCaption)
		{
			m_pspw.pszTitle=allocstringW(nIDCaption);
			m_pspw.dwFlags|=PSP_USETITLE;
		}
		m_pspw.pfnCallback=NULL; /*PropertySheetPageProcW;*/
		m_bFirstSetActive=TRUE;
	}
	else
	{
		ZeroMemory(&m_psp,sizeof(PROPSHEETPAGE));
		m_psp.dwSize=sizeof(PROPSHEETPAGE);
		m_psp.dwFlags=PSP_DEFAULT /*|PSP_USECALLBACK */;
		if (m_lpszTemplateName!=NULL)
		{
			m_psp.hInstance=GetResourceHandle(bType);
			m_psp.pszTemplate=m_lpszTemplateName;
		}
		m_psp.pfnDlgProc=(DLGPROC)CAppData::PropPageWndProc;
		m_psp.lParam=(LPARAM)this;
		if (nIDCaption)
		{
			m_psp.pszTitle=allocstring(nIDCaption);
			m_psp.dwFlags|=PSP_USETITLE;
		}
		m_psp.pfnCallback=NULL; /*PropertySheetPageProcA;*/
		m_bFirstSetActive=TRUE;
	}
}

void CPropertyPage::Construct(LPCSTR szTitle,TypeOfResourceHandle bType)
{
	if (IsUnicodeSystem())
	{
		ZeroMemory(&m_pspw,sizeof(PROPSHEETPAGEW));
		m_pspw.dwSize=sizeof(PROPSHEETPAGEW);
		m_pspw.dwFlags=PSP_DEFAULT ; //PSP_USECALLBACK;
		if (m_lpszTemplateNameW!=NULL)
		{
			m_pspw.hInstance=GetResourceHandle(bType);
			m_pspw.pszTemplate=m_lpszTemplateNameW;
		}
		m_pspw.pfnDlgProc=(DLGPROC)CAppData::PropPageWndProc;
		m_pspw.lParam=(LPARAM)this;
		m_pspw.pszTitle=alloccopyAtoW(szTitle);
		m_pspw.dwFlags|=PSP_USETITLE;
		m_pspw.pfnCallback=NULL; //PropertySheetPageProc;
		m_bFirstSetActive=TRUE;
	}
	else
	{
		ZeroMemory(&m_psp,sizeof(PROPSHEETPAGE));
		m_psp.dwSize=sizeof(PROPSHEETPAGE);
		m_psp.dwFlags=PSP_DEFAULT ; //PSP_USECALLBACK;
		if (m_lpszTemplateName!=NULL)
		{
			m_psp.hInstance=GetResourceHandle(bType);
			m_psp.pszTemplate=m_lpszTemplateName;
		}
		m_psp.pfnDlgProc=(DLGPROC)CAppData::PropPageWndProc;
		m_psp.lParam=(LPARAM)this;
		m_psp.pszTitle=alloccopy(szTitle);
		m_psp.dwFlags|=PSP_USETITLE;
		m_psp.pfnCallback=NULL; //PropertySheetPageProc;
		m_bFirstSetActive=TRUE;
	}
}

void CPropertyPage::Construct(LPCWSTR szTitle,TypeOfResourceHandle bType)
{
	if (IsUnicodeSystem())
	{
		ZeroMemory(&m_pspw,sizeof(PROPSHEETPAGEW));
		m_pspw.dwSize=sizeof(PROPSHEETPAGEW);
		m_pspw.dwFlags=PSP_DEFAULT ; //PSP_USECALLBACK;
		if (m_lpszTemplateNameW!=NULL)
		{
			m_pspw.hInstance=GetResourceHandle(bType);
			m_pspw.pszTemplate=m_lpszTemplateNameW;
		}
		m_pspw.pfnDlgProc=(DLGPROC)CAppData::PropPageWndProc;
		m_pspw.lParam=(LPARAM)this;
		m_pspw.pszTitle=alloccopy(szTitle);
		m_pspw.dwFlags|=PSP_USETITLE;
		m_pspw.pfnCallback=NULL; //PropertySheetPageProc;
		m_bFirstSetActive=TRUE;
	}
	else
	{
		ZeroMemory(&m_psp,sizeof(PROPSHEETPAGE));
		m_psp.dwSize=sizeof(PROPSHEETPAGE);
		m_psp.dwFlags=PSP_DEFAULT ; //PSP_USECALLBACK;
		if (m_lpszTemplateName!=NULL)
		{
			m_psp.hInstance=GetResourceHandle(bType);
			m_psp.pszTemplate=m_lpszTemplateName;
		}
		m_psp.pfnDlgProc=(DLGPROC)CAppData::PropPageWndProc;
		m_psp.lParam=(LPARAM)this;
		m_psp.pszTitle=alloccopyWtoA(szTitle);
		m_psp.dwFlags|=PSP_USETITLE;
		m_psp.pfnCallback=NULL; //PropertySheetPageProc;
		m_bFirstSetActive=TRUE;
	}
}

void CPropertyPage::SetModified(BOOL bChanged)
{
	if (m_hWnd==NULL)
		return;
	HWND hParentWnd=GetParent();
	if (bChanged)
		::SendMessage(hParentWnd,PSM_CHANGED,(WPARAM)m_hWnd,0);
	else
		::SendMessage(hParentWnd,PSM_UNCHANGED,(WPARAM)m_hWnd,0);
}

BOOL CPropertyPage::OnApply()
{
	OnOK();
	return TRUE;
}

void CPropertyPage::OnReset()
{
	OnCancel();
}

void CPropertyPage::OnOK()
{
}

void CPropertyPage::OnCancel()
{
}

BOOL CPropertyPage::OnSetActive()
{
	if (m_bFirstSetActive)
		m_bFirstSetActive=FALSE;
	return TRUE;
}

BOOL CPropertyPage::OnKillActive()
{
	return TRUE;
}

BOOL CPropertyPage::OnQueryCancel()
{
	return TRUE;
}

BOOL CPropertyPage::OnWizardBack()
{
	return TRUE;
}

BOOL CPropertyPage::OnWizardNext()
{
	return TRUE;
}

BOOL CPropertyPage::OnWizardFinish()
{
	return TRUE;
}

BOOL CPropertyPage::OnNotify(int idCtrl,LPNMHDR pnmh)
{
	BOOL ret;
	if (CDialog::OnNotify(idCtrl,pnmh))
		return TRUE;
	if (pnmh->hwndFrom!=m_hWnd && pnmh->hwndFrom!=::GetParent(m_hWnd))
		return FALSE;
	switch(pnmh->code)
	{
	case PSN_APPLY:
		if (!OnApply())
			ret=PSNRET_INVALID_NOCHANGEPAGE;
		else
			ret=PSNRET_NOERROR;
		break;
	case PSN_RESET:
		OnReset();
		return TRUE;
	case PSN_SETACTIVE:
		if (OnSetActive())
			ret=0;
		else
			ret=-1;
		break;
	case PSN_KILLACTIVE:
		ret=!OnKillActive();
		break;
	case PSN_QUERYCANCEL:
		ret=!OnQueryCancel();
		break;
	case PSN_WIZBACK:
		if (OnWizardBack())
			ret=0;
		else
			ret=-1;
		break;
	case PSN_WIZNEXT:
		if (OnWizardNext())
			ret=0;
		else
			ret=-1;
		break;
	case PSN_WIZFINISH:
		ret=!OnWizardFinish();
		break;
	case PSN_HELP:
		{
			HELPINFO hi;
			hi.cbSize=sizeof(HELPINFO);
			hi.iContextType=HELPINFO_WINDOW;
			hi.dwContextId=0;
			hi.iCtrlId=(int)pnmh->idFrom;
			hi.hItemHandle=pnmh->hwndFrom;
			GetCursorPos(&hi.MousePos);
			OnHelp(&hi);
			return TRUE;
		}
	default:
		return FALSE;
	}
	::SetWindowLongPtr(m_hWnd,DWLP_MSGRESULT,(ULONG_PTR)ret);
	return TRUE;
}

///////////////////////////////////////////
// CPropertySheet
///////////////////////////////////////////

/*
int CALLBACK PropertySheetProc(HWND hWndDlg,UINT uMsg,LPARAM lParam)
{
	switch (uMsg)
	{
	case PSCB_INITIALIZED:
		
		DebugMessage("PropertySheetProc(): PSCB_INITIALIZED");
		break;
	}
	return 0;
}
*/

CPropertySheet::~CPropertySheet()
{
	if (m_psh.phpage!=NULL)
		delete[] m_psh.phpage;
	if (IsUnicodeSystem())
	{
		if (m_pshw.pszCaption)
			delete[] m_pshw.pszCaption;
		
		if (m_psh.dwFlags&=PSH_USEICONID && !IS_INTRESOURCE(m_psh.pszIcon))
			delete[] m_pshw.pszIcon;
	}
	else
	{
		if (m_pshw.pszCaption)
			delete[] m_psh.pszCaption;
		
		if (m_psh.dwFlags&=PSH_USEICONID && !IS_INTRESOURCE(m_psh.pszIcon))
			delete[] m_psh.pszIcon;
	}
}

void CPropertySheet::Construct(UINT nIDCaption,HWND hParentWnd,UINT iSelectPage)
{
	Construct(ID2W(nIDCaption),hParentWnd,iSelectPage);
}

void CPropertySheet::Construct(LPCSTR pszCaption,HWND hParentWnd,UINT iSelectPage)
{
	m_hParentWnd=hParentWnd;

	if (IsUnicodeSystem())
	{
		ZeroMemory(&m_pshw,sizeof(PROPSHEETHEADERW));
		m_pshw.dwSize=PROPSHEETHEADERW_V1_SIZE; //sizeof(PROPSHEETHEADER);
		
		if (pszCaption!=NULL)
			m_pshw.pszCaption=alloccopyAtoW(pszCaption);
		else
			DebugMessage("CPropertySheet::Construct(): pszCaption==NULL");
		m_pshw.dwFlags=PSH_DEFAULT;
		m_pshw.nStartPage=iSelectPage;
		m_pshw.pfnCallback=NULL;
		m_pshw.hInstance=GetCommonResourceHandle();
	}
	else
	{
		ZeroMemory(&m_psh,sizeof(PROPSHEETHEADER));
		m_psh.dwSize=PROPSHEETHEADER_V1_SIZE; //sizeof(PROPSHEETHEADER);
		
		if (pszCaption!=NULL)
			m_psh.pszCaption=alloccopy(pszCaption);
		else
			DebugMessage("CPropertySheet::Construct(): pszCaption==NULL");
		m_psh.dwFlags=PSH_DEFAULT;
		m_psh.nStartPage=iSelectPage;
		m_psh.pfnCallback=NULL;
		m_psh.hInstance=GetCommonResourceHandle();
	}
}

void CPropertySheet::Construct(LPCWSTR pszCaption,HWND hParentWnd,UINT iSelectPage)
{
	m_hParentWnd=hParentWnd;
	if (IsUnicodeSystem())
	{
		ZeroMemory(&m_pshw,sizeof(PROPSHEETHEADERW));
		m_pshw.dwSize=PROPSHEETHEADERW_V1_SIZE; //sizeof(PROPSHEETHEADER);
		
		if (pszCaption!=NULL)
			m_pshw.pszCaption=alloccopy(pszCaption);
		else
			DebugMessage("CPropertySheet::Construct(): pszCaption==NULL");
		m_pshw.dwFlags=PSH_DEFAULT;
		m_pshw.nStartPage=iSelectPage;
		m_pshw.pfnCallback=NULL;
		m_pshw.hInstance=GetCommonResourceHandle();
	}
	else
	{
		ZeroMemory(&m_psh,sizeof(PROPSHEETHEADERW));
		m_psh.dwSize=PROPSHEETHEADER_V1_SIZE; //sizeof(PROPSHEETHEADER);
		
		if (pszCaption!=NULL)
			m_psh.pszCaption=alloccopyWtoA(pszCaption);
		else
			DebugMessage("CPropertySheet::Construct(): pszCaption==NULL");
		m_psh.dwFlags=PSH_DEFAULT;
		m_psh.nStartPage=iSelectPage;
		m_psh.pfnCallback=NULL;
		m_psh.hInstance=GetCommonResourceHandle();
	}
}

BOOL CPropertySheet::Create(HWND hParentWnd,DWORD dwStyle,DWORD dwExStyle)
{
	BuildPropPageArray();
	m_psh.dwFlags|=PSH_MODELESS;
	m_hWnd=(HWND)PropertySheet(&m_psh);
	if (m_hWnd==NULL)
	{
		DebugMessage("CPropertyPage::Create(): NULL returned by PropertySheet()");
		return FALSE;
	}
	DebugMessage("CProperyPage::Create(): PropertySheet()!=NULL NOERROR");
	return TRUE;
}

int CPropertySheet::GetActiveIndex() const
{
	if (m_hWnd==NULL)
		return -1;
	HWND hActivePage=(HWND)::SendMessage(m_hWnd,PSM_GETCURRENTPAGEHWND,0,0);
	for (int i=m_pages.GetSize()-1;i>=0;i--)
	{
		if (((CPropertyPage*)m_pages.GetAt(i))->m_hWnd==hActivePage)
			return i;
	}
	return -1;
}

int CPropertySheet::GetPageIndex(CPropertyPage* pPage)
{
	for (int i=m_pages.GetSize()-1;i>=0;i--)
	{
		if ((CPropertyPage*)m_pages.GetAt(i)==pPage)
			return i;
	}
	return -1;
}

BOOL CPropertySheet::SetActivePage(int nPage)
{
	if (m_hWnd!=NULL)
		return (BOOL)::SendMessage(m_hWnd,PSM_SETCURSEL,nPage,NULL);
	m_psh.nStartPage=nPage;
	return FALSE;
}

BOOL CPropertySheet::SetActivePage(CPropertyPage* pPage)
{
	if (m_hWnd!=NULL)
		return (BOOL)::SendMessage(m_hWnd,PSM_SETCURSEL,0,(LPARAM)pPage->m_hWnd);
	else
	{
		for (int i=m_pages.GetSize()-1;i>=0;i--)
		{
			if ((CPropertyPage*)m_pages.GetAt(i)==pPage)
			{
				m_psh.nStartPage=i;
				break;
			}
		}
	}
	return FALSE;
}

void CPropertySheet::SetTitle(LPCSTR lpszText,UINT nStyle)
{
	if (IsUnicodeSystem())
	{
		if (m_pshw.pszCaption!=NULL)
			delete[] m_pshw.pszCaption;
		m_pshw.pszCaption=lpszText!=NULL?alloccopyAtoW(lpszText):NULL;
		m_pshw.dwFlags&=~PSH_PROPTITLE;
		m_pshw.dwFlags|=nStyle;
		
		if (m_hWnd!=NULL)
			::SendMessageW(m_hWnd,PSM_SETTITLEW,nStyle,(LPARAM)(LPCWSTR)A2W(lpszText));
	}
	else
	{
		if (m_psh.pszCaption!=NULL)
			delete[] m_psh.pszCaption;
		m_psh.pszCaption=lpszText!=NULL?alloccopy(lpszText):NULL;
		m_psh.dwFlags&=~PSH_PROPTITLE;
		m_psh.dwFlags|=nStyle;
		
		if (m_hWnd!=NULL)
			::SendMessageA(m_hWnd,PSM_SETTITLEA,nStyle,(LPARAM)(LPCSTR)(lpszText));
	}
}

void CPropertySheet::SetIcon(HICON hIcon)
{
	if (m_psh.dwFlags&PSH_USEICONID && !IS_INTRESOURCE(m_psh.pszIcon))
	{
		if (IsUnicodeSystem())
			delete[] m_pshw.pszIcon;
		else
			delete[] m_psh.pszIcon;
		
	}

	m_psh.dwFlags&=~PSH_USEICONID;
	m_psh.dwFlags|=PSH_USEHICON;
	m_psh.hIcon=hIcon;
}

void CPropertySheet::SetIcon(LPCSTR szIcon)
{
	if (m_psh.dwFlags&PSH_USEICONID && !IS_INTRESOURCE(m_psh.pszIcon))
	{
		if (IsUnicodeSystem())
			delete[] m_pshw.pszIcon;
		else
			delete[] m_psh.pszIcon;
	}
	else
		m_psh.dwFlags&=~PSH_USEHICON;
	
	m_psh.dwFlags|=PSH_USEICONID;
	if (IsUnicodeSystem())
		m_pshw.pszIcon=alloccopyAtoW(szIcon);
	else
		m_psh.pszIcon=alloccopy(szIcon);
}

void CPropertySheet::SetIcon(LPCWSTR szIcon)
{
	if (m_psh.dwFlags&PSH_USEICONID && !IS_INTRESOURCE(m_psh.pszIcon))
	{
		if (IsUnicodeSystem())
			delete[] m_pshw.pszIcon;
		else
			delete[] m_psh.pszIcon;
	}
	else
		m_psh.dwFlags&=~PSH_USEHICON;
	
	m_psh.dwFlags|=PSH_USEICONID;
	if (IsUnicodeSystem())
		m_pshw.pszIcon=alloccopy(szIcon);
	else
		m_psh.pszIcon=alloccopyWtoA(szIcon);
}

void CPropertySheet::SetIcon(int nIcon)
{
	if (m_psh.dwFlags&PSH_USEICONID && !IS_INTRESOURCE(m_psh.pszIcon))
	{
		if (IsUnicodeSystem())
			delete[] m_pshw.pszIcon;
		else
			delete[] m_psh.pszIcon;
	}
	else
		m_psh.dwFlags&=~PSH_USEHICON;
	
	m_psh.dwFlags|=PSH_USEICONID;
	m_psh.pszIcon=MAKEINTRESOURCE(nIcon);
}

void CPropertySheet::SetTitle(LPCWSTR lpszText,UINT nStyle)
{
	if (IsUnicodeSystem())
	{
		if (m_pshw.pszCaption!=NULL)
			delete[] m_pshw.pszCaption;
		m_pshw.pszCaption=lpszText!=NULL?alloccopy(lpszText):NULL;
		m_pshw.dwFlags&=~PSH_PROPTITLE;
		m_pshw.dwFlags|=nStyle;
		
		if (m_hWnd!=NULL)
			::SendMessageW(m_hWnd,PSM_SETTITLEW,nStyle,(LPARAM)(LPCWSTR)(lpszText));
	}
	else
	{
		if (m_psh.pszCaption!=NULL)
			delete[] m_psh.pszCaption;
		m_psh.pszCaption=lpszText!=NULL?alloccopyWtoA(lpszText):NULL;
		m_psh.dwFlags&=~PSH_PROPTITLE;
		m_psh.dwFlags|=nStyle;
		
		if (m_hWnd!=NULL)
			::SendMessageA(m_hWnd,PSM_SETTITLEA,nStyle,(LPARAM)(LPCSTR)W2A(lpszText));
	}
}

INT_PTR CPropertySheet::DoModal()
{
	BuildPropPageArray();
	m_psh.hwndParent=m_hParentWnd;
	if (IsUnicodeSystem())
	{
		if (m_pshw.dwFlags&PSH_MODELESS)
		{
			m_hWnd=(HWND)::PropertySheetW(&m_pshw);
			return m_hWnd!=NULL;
		}
		return ::PropertySheetW(&m_pshw);
	}
	else
	{
		if (m_psh.dwFlags&PSH_MODELESS)
		{
			m_hWnd=(HWND)::PropertySheet(&m_psh);
			return m_hWnd!=NULL;
		}
		return ::PropertySheet(&m_psh);
	}

}

void CPropertySheet::EndDialog(int nEndID)
{
	if (m_hWnd==NULL)
	{
		DebugMessage("CProperySheet::EndDialog(): m_hWnd==NULL");
		return;
	}
	if (m_psh.dwFlags&PSH_MODELESS)
		::DestroyWindow(m_hWnd);
	else
		::PostMessage(m_hWnd,PSM_PRESSBUTTON,IDCANCEL,0);
}

void CPropertySheet::BuildPropPageArray()
{
	if (m_psh.ppsp!=NULL)
	{
		DebugMessage("CPropertySheet::BuildPropPageArray(): m_psg.ppsp!=NULL");
	}
	
	if (IsUnicodeSystem())
	{
		m_psh.phpage=new HPROPSHEETPAGE[max(2,m_pages.GetSize())];
		for (int i=0;i<m_pages.GetSize();i++)
		{
			CPropertyPage* pPage=(CPropertyPage*)m_pages.GetAt(i);
			m_pshw.phpage[i]=CreatePropertySheetPageW(&pPage->m_pspw);
		}
		m_pshw.nPages=m_pages.GetSize();
	}
	else
	{
		m_psh.phpage=new HPROPSHEETPAGE[max(2,m_pages.GetSize())];
		for (int i=0;i<m_pages.GetSize();i++)
		{
			m_psh.phpage[i]=CreatePropertySheetPageA(
				&((CPropertyPage*)m_pages.GetAt(i))->m_psp);
		}
		m_psh.nPages=m_pages.GetSize();
	}
}



///////////////////////////
// Class CInputDialog
///////////////////////////

INT_PTR CInputDialog::DoModal(HWND hWndParent)
{
	INT_PTR ret=CDialog::DoModal(hWndParent);
	if (ret==IDC_OK)
		return 1;
	m_hWnd=NULL;
	return 0;
}

void CInputDialog::SetTitle(LPCSTR szTitle)
{
	m_Title=szTitle;
	if (m_hWnd!=NULL)
		SetWindowText(m_Title);
}

void CInputDialog::SetTitle(int iTitle)
{
	m_Title.LoadString(iTitle);
	if (m_hWnd!=NULL)
		SetWindowText(m_Title);
}

BOOL CInputDialog::SetText(LPCSTR szText)
{
	m_Text=szText;
	if (m_hWnd!=NULL)
		SetDlgItemText(IDC_TEXT,m_Text);
	return TRUE;
}

BOOL CInputDialog::SetText(int iText)
{
	m_Text.LoadString(iText);
	if (m_hWnd!=NULL)
		SetDlgItemText(IDC_TEXT,m_Text);
	return TRUE;
}

void CInputDialog::SetInputText(LPCSTR szText)
{
	m_Input=szText;
	if (m_hWnd!=NULL)
		SetDlgItemText(IDC_EDIT,m_Input);
}

int CInputDialog::GetInputText(LPSTR szText,int nTextLen) const
{
	return WideCharToMultiByte(CP_ACP,0,(LPCWSTR)m_Input,-1,szText,nTextLen,NULL,NULL);
}

void CInputDialog::SetOKButtonText(LPCSTR szText)
{
	m_OKButton=szText;
	if (m_hWnd!=NULL)
		SetDlgItemText(IDC_OK,m_OKButton);
}

void CInputDialog::SetOKButtonText(int nText)
{
	m_OKButton.LoadString(nText);
	if (m_hWnd!=NULL)
		SetDlgItemText(IDC_OK,m_OKButton);
}

void CInputDialog::SetCancelButtonText(LPCSTR szText)
{
	if (m_bFlags&ID_NOCANCELBUTTON)
		return;
	m_CancelButton=szText;
	if (m_hWnd!=NULL)
		SetDlgItemText(IDC_CANCEL,m_CancelButton);
}

void CInputDialog::SetCancelButtonText(int nText)
{
	if (m_bFlags&ID_NOCANCELBUTTON)
		return;
	m_CancelButton.LoadString(nText);
	if (m_hWnd!=NULL)
		SetDlgItemText(IDC_CANCEL,m_CancelButton);
}

#ifdef DEF_WCHAR
void CInputDialog::SetTitle(LPCWSTR szTitle)
{
	m_Title=szTitle;
	if (m_hWnd!=NULL)
		SetWindowText(m_Title);
}

BOOL CInputDialog::SetText(LPCWSTR szText)
{
	m_Text=szText;
	if (m_hWnd!=NULL)
		SetDlgItemText(IDC_TEXT,m_Text);
	return TRUE;
}

void CInputDialog::SetInputText(LPCWSTR szText)
{
	m_Input=szText;
	if (m_hWnd!=NULL)
		SetDlgItemText(IDC_EDIT,m_Input);
}

int CInputDialog::GetInputText(LPWSTR szText,int nTextLen) const
{
	int i=min(nTextLen-1,(int)m_Input.GetLength());
	MemCopyW(szText,(LPCWSTR)m_Input,i);
	szText[i]='\0';
	return i;
}

void CInputDialog::SetOKButtonText(LPCWSTR szText)
{
	m_OKButton=szText;
	if (m_hWnd!=NULL)
		SetDlgItemText(IDC_OK,m_OKButton);
}

void CInputDialog::SetCancelButtonText(LPCWSTR szText)
{
	if (m_bFlags&ID_NOCANCELBUTTON)
		return;
	m_CancelButton=szText;
	if (m_hWnd!=NULL)
		SetDlgItemText(IDC_CANCEL,m_CancelButton);
}

#endif

BOOL CInputDialog::OnInitDialog(HWND hwndFocus)
{
	SetDlgItemText(IDC_TEXT,m_Text);
	SetDlgItemText(IDC_EDIT,m_Input);
	SetWindowText(m_Title);
	SetDlgItemText(IDC_OK,m_OKButton);
	if (m_bFlags&ID_NOCANCELBUTTON)
		ShowDlgItem(IDC_CANCEL,swHide);
	else
		SetDlgItemText(IDC_CANCEL,m_CancelButton);
	SetFocus(IDC_EDIT);
	return CDialog::OnInitDialog(hwndFocus);
}

BOOL CInputDialog::OnCommand(WORD wID,WORD wNotifyCode,HWND hControl)
{
	switch (wID)
	{
	case IDOK:
	case IDC_OK:
		GetDlgItemText(IDC_EDIT,m_Input);
		if (m_bFlags&ID_DONTALLOWEMPTY && m_Input.IsEmpty())
			break;
		EndDialog(IDC_OK);
		break;
	case IDCANCEL:
	case IDC_CANCEL:
		GetDlgItemText(IDC_EDIT,m_Input);
		EndDialog(IDC_CANCEL);
		break;
	case IDC_EDIT:
		if (wNotifyCode==EN_SETFOCUS)
			::SendMessage(hControl,EM_SETSEL,0,-1);
		break;
	}
	return CDialog::OnCommand(wID,wNotifyCode,hControl);
}

BOOL CInputDialog::OnClose()
{
	GetDlgItemText(IDC_EDIT,m_Input);
	EndDialog(IDC_CANCEL);
	return CDialog::OnClose();
}

///////////////////////////
// Class CCommonDialog
///////////////////////////


LRESULT CALLBACK CAppData::CommonDialogProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	CCommonDialog* Wnd;
	if (uMsg==WM_INITDIALOG && lParam!=NULL)
	{
		switch (((DWORD*)lParam)[0])
		{
#if !(_WIN32_WINNT >= 0x0500)
		case sizeof (OPENFILENAME):
		case sizeof (OPENFILENAME)+2*sizeof(DWORD)+sizeof(void*): // Win2000 Extensions
			Wnd=(CCommonDialog*)((OPENFILENAME*)lParam)->lCustData;
			break;
#else		
		case sizeof (OPENFILENAME):
		case OPENFILENAME_SIZE_VERSION_400:
			Wnd=(CCommonDialog*)((OPENFILENAME*)lParam)->lCustData;
			break;
#endif		
		case sizeof (CHOOSEFONT):
			Wnd=(CCommonDialog*)((CHOOSEFONT*)lParam)->lCustData;
			break;
		case sizeof (CHOOSECOLOR):
			Wnd=(CCommonDialog*)((CHOOSECOLOR*)lParam)->lCustData;
			break;
		case sizeof (PAGESETUPDLG):
			Wnd=(CCommonDialog*)((PAGESETUPDLG*)lParam)->lCustData;
			break;
		case sizeof (PRINTDLG):
			Wnd=(CCommonDialog*)((PRINTDLG*)lParam)->lCustData;
			break;
		case sizeof (FINDREPLACE):
			Wnd=(CCommonDialog*)((FINDREPLACE*)lParam)->lCustData;
			break;
		default: 
			ASSERT(0);
			return FALSE;
		}
		
		if (Wnd==NULL)
			return FALSE;
		
		Wnd->Attach(hWnd);
		SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG_PTR)Wnd);
		return Wnd->OnInitDialog((HWND)wParam);
	}
	
	Wnd=(CCommonDialog*)GetWindowLongPtr(hWnd,GWLP_USERDATA);
	if (Wnd!=NULL && uMsg==WM_COMMAND)
	{
		switch(LOWORD(wParam))
		{
		case IDOK:
			Wnd->OnOK();
			return FALSE;
		case IDCANCEL:
			Wnd->OnCancel();
			return FALSE;
		}
	}
	return WndProc(hWnd,uMsg,wParam,lParam);
}

CCommonDialog::CCommonDialog()
:	CDialog(0)
{
}

void CCommonDialog::OnOK()
{
}

void CCommonDialog::OnCancel()
{
}

///////////////////////////
// Class CFileDialog
///////////////////////////


void CFileDialog::Init(LPCSTR lpszDefExt,LPCSTR lpszFileName,DWORD dwFlags,LPCSTR lpszFilter,BOOL bSetHook)
{
	DebugMessage("CFileDialog::Init BEGIN");

	if (IsUnicodeSystem())
	{

#if (_WIN32_WINNT < 0x0500)
		m_pwofn=(OPENFILENAMEW*)new char[sizeof(OPENFILENAMEW)+16];
		if (m_pwofn==NULL)
		{
			SetHFCError(HFC_CANNOTALLOC);
			return;
		}		
		iMemSet(m_pwofn,0,sizeof(OPENFILENAMEW)+16);
		m_pwofn->lStructSize=sizeof(OPENFILENAME);
#else
		m_pwofn=(OPENFILENAMEW*)new char[sizeof(OPENFILENAMEW)];
		if (m_pwofn==NULL)
		{
			SetHFCError(HFC_CANNOTALLOC);
			return;
		}		
		ZeroMemory(m_pwofn,sizeof(OPENFILENAMEW));
		if (GetSystemFeaturesFlag()&(efWin2000|efWinXP))
		{
			DebugMessage("CFileDialog::CFileDialog: m_pwofn->lStructSize=sizeof(OPENFILENAMEW)");
			m_pwofn->lStructSize=sizeof(OPENFILENAMEW);
		}
		else
		{
			DebugMessage("CFileDialog::CFileDialog: m_pwofn->lStructSize=sizeof(OPENFILENAME_SIZE_VERSION_400W)");
			m_pwofn->lStructSize=OPENFILENAME_SIZE_VERSION_400W;
		}
#endif


		m_pwofn->Flags=dwFlags|OFN_EXPLORER;
		if (bSetHook)
		{
			m_pwofn->lpfnHook=(LPOFNHOOKPROC)CAppData::CommonDialogProc;
			m_pwofn->lCustData=(LPARAM)this;
			m_pwofn->Flags|=OFN_ENABLEHOOK;
		}
		m_pwofn->lpstrFileTitle=new WCHAR[65];
		m_pwofn->lpstrFileTitle[0]='\0';
		m_pwofn->nMaxFileTitle=64;
		m_pwofn->hInstance=GetLanguageSpecificResourceHandle();
		m_pwofn->lpstrCustomFilter=NULL;
		m_pwofn->nMaxCustFilter=0;
		
		m_pwFileName=new WCHAR[MAX_PATH];
		if (m_pwFileName==NULL)
		{
			SetHFCError(HFC_CANNOTALLOC);
			DebugMessage("CFileDialog::CFileDialog END ERR1");
			return;
		}
		m_pwofn->nMaxFile=MAX_PATH;
		m_pwofn->lpstrFile=m_pwFileName;
		if (lpszFileName!=NULL)
		{
			int nRet=(int)istrlen(lpszFileName);
			if (nRet>MAX_PATH-2)
				nRet=MAX_PATH-2;
            
			MultiByteToWideChar(CP_ACP,0,lpszFileName,nRet,m_pwFileName,nRet);
			m_pwFileName[nRet]='\0';
			m_pwFileName[nRet+1]='\0';

		}
		else
		{
			m_pwFileName[0]='\0';
			m_pwFileName[1]='\0';
		}

		if (lpszFilter!=NULL)
			m_pwFilter=alloccopyAtoW(lpszFilter);
		else
			m_pwFilter=NULL;
		
		m_pwofn->nFilterIndex=0;
		m_pwofn->lpstrInitialDir=NULL;
		m_pwofn->lpstrTitle=NULL;
		m_pwofn->lpTemplateName=NULL;
		if (lpszDefExt!=NULL)
			m_pwofn->lpstrDefExt=alloccopyAtoW(lpszDefExt);
		else
			m_pwofn->lpstrDefExt=NULL;
	}
	else
	{
#if (_WIN32_WINNT < 0x0500)
		m_pofn=(OPENFILENAME*)new char[sizeof(OPENFILENAME)+16];
		if (m_pofn==NULL)
		{
			SetHFCError(HFC_CANNOTALLOC);
			return;
		}		
		iMemSet(m_pofn,0,sizeof(OPENFILENAME)+16);
		m_pofn->lStructSize=sizeof(OPENFILENAME);
#else
		m_pofn=(OPENFILENAME*)new char[sizeof(OPENFILENAME)];
		if (m_pofn==NULL)
		{
			SetHFCError(HFC_CANNOTALLOC);
			return;
		}		
		ZeroMemory(m_pofn,sizeof(OPENFILENAME));
		if (GetSystemFeaturesFlag()&(efWin2000|efWinXP))
		{
			DebugMessage("CFileDialog::CFileDialog: m_pofn->lStructSize=sizeof(OPENFILENAME)");
			m_pofn->lStructSize=sizeof(OPENFILENAME);
		}
		else
		{
			DebugMessage("CFileDialog::CFileDialog: m_pofn->lStructSize=sizeof(OPENFILENAME_SIZE_VERSION_400)");
			m_pofn->lStructSize=OPENFILENAME_SIZE_VERSION_400;
		}
#endif


		m_pofn->Flags=dwFlags|OFN_EXPLORER;
		if (bSetHook)
		{
			m_pofn->lpfnHook=(LPOFNHOOKPROC)CAppData::CommonDialogProc;
			m_pofn->lCustData=(LPARAM)this;
			m_pofn->Flags|=OFN_ENABLEHOOK;
		}
		m_pofn->lpstrFileTitle=new char[65];
		m_pofn->lpstrFileTitle[0]='\0';
		m_pofn->nMaxFileTitle=64;
		m_pofn->hInstance=GetLanguageSpecificResourceHandle();
		m_pofn->lpstrCustomFilter=NULL;
		m_pofn->nMaxCustFilter=0;
		
		m_pFileName=new CHAR[MAX_PATH];
		if (m_pFileName==NULL)
		{
			SetHFCError(HFC_CANNOTALLOC);
			DebugMessage("CFileDialog::CFileDialog END ERR1");
			return;
		}
		m_pofn->nMaxFile=MAX_PATH;
		if (lpszFileName!=NULL)
		{
			int nLen=(int)istrlen(lpszFileName);
			if (nLen>=MAX_PATH-1)
				nLen=MAX_PATH-2;
			MemCopy(m_pFileName,lpszFileName,nLen+1);
			m_pFileName[nLen+1]='\0';
		}
		else
		{
			m_pFileName[0]='\0';
			m_pFileName[1]='\0';
		}
		m_pofn->lpstrFile=m_pFileName;
		

		if (lpszFilter!=NULL)
			m_pFilter=alloccopy(lpszFilter);
		else
			m_pFilter=NULL;
		
		m_pofn->nFilterIndex=0;
		m_pofn->lpstrInitialDir=NULL;
		m_pofn->lpstrTitle=NULL;
		m_pofn->lpTemplateName=NULL;
		
		if (lpszDefExt!=NULL)
			m_pofn->lpstrDefExt=alloccopy(lpszDefExt);
		else
			m_pofn->lpstrDefExt=NULL;
	}
	DebugMessage("CFileDialog::CFileDialog END");
}


void CFileDialog::Init(LPCWSTR lpszDefExt,LPCWSTR lpszFileName,DWORD dwFlags,LPCWSTR lpszFilter,BOOL bSetHook)
{
	DebugMessage("CFileDialog::Init BEGIN");
	
	if (IsUnicodeSystem())
	{

#if (_WIN32_WINNT < 0x0500)
		m_pwofn=(OPENFILENAMEW*)new char[sizeof(OPENFILENAMEW)+16];
		if (m_pwofn==NULL)
		{
			SetHFCError(HFC_CANNOTALLOC);
			return;
		}		
		iMemSet(m_pwofn,0,sizeof(OPENFILENAMEW)+16);
		m_pwofn->lStructSize=sizeof(OPENFILENAME);
#else
		m_pwofn=(OPENFILENAMEW*)new char[sizeof(OPENFILENAMEW)];
		if (m_pwofn==NULL)
		{
			SetHFCError(HFC_CANNOTALLOC);
			return;
		}		
		ZeroMemory(m_pwofn,sizeof(OPENFILENAMEW));
		if (GetSystemFeaturesFlag()&(efWin2000|efWinXP))
		{
			DebugMessage("CFileDialog::CFileDialog: m_pwofn->lStructSize=sizeof(OPENFILENAMEW)");
			m_pwofn->lStructSize=sizeof(OPENFILENAMEW);
		}
		else
		{
			DebugMessage("CFileDialog::CFileDialog: m_pwofn->lStructSize=sizeof(OPENFILENAME_SIZE_VERSION_400W)");
			m_pwofn->lStructSize=OPENFILENAME_SIZE_VERSION_400W;
		}
#endif


		m_pwofn->Flags=dwFlags|OFN_EXPLORER;
		if (bSetHook)
		{
			m_pwofn->lpfnHook=(LPOFNHOOKPROC)CAppData::CommonDialogProc;
			m_pwofn->Flags|=OFN_ENABLEHOOK;
			m_pwofn->lCustData=(LPARAM)this;
		}
		m_pwofn->lpstrFileTitle=new WCHAR[65];
		m_pwofn->lpstrFileTitle[0]='\0';
		m_pwofn->nMaxFileTitle=64;
		m_pwofn->hInstance=GetLanguageSpecificResourceHandle();
		m_pwofn->lpstrCustomFilter=NULL;
		m_pwofn->nMaxCustFilter=0;
		
		m_pwFileName=new WCHAR[MAX_PATH];
		if (m_pwFileName==NULL)
		{
			SetHFCError(HFC_CANNOTALLOC);
			DebugMessage("CFileDialog::CFileDialog END ERR1");
			return;
		}
		m_pwofn->nMaxFile=MAX_PATH;
		if (lpszFileName!=NULL)
		{
			int nRet=(int)istrlenw(lpszFileName);
			if (nRet>MAX_PATH-2)
				nRet=MAX_PATH-2;
				
			MemCopyW(m_pwFileName,lpszFileName,nRet);
			m_pwFileName[nRet]='\0';
			m_pwFileName[nRet+1]='\0';
		}
		else
		{
			m_pwFileName[0]='\0';
			m_pwFileName[1]='\0';
		}
		m_pwofn->lpstrFile=m_pwFileName;
		

		if (lpszFilter!=NULL)
			m_pwFilter=alloccopy(lpszFilter);
		else
			m_pwFilter=NULL;
		
		m_pwofn->nFilterIndex=0;
		m_pwofn->lpstrInitialDir=NULL;
		m_pwofn->lpstrTitle=NULL;
		m_pwofn->lpTemplateName=NULL;
		if (lpszDefExt!=NULL)
			m_pwofn->lpstrDefExt=alloccopy(lpszDefExt);
		else
			m_pwofn->lpstrDefExt=NULL;
	}
	else
	{
#if (_WIN32_WINNT < 0x0500)
		m_pofn=(OPENFILENAME*)new char[sizeof(OPENFILENAME)+16];
		if (m_pofn==NULL)
		{
			SetHFCError(HFC_CANNOTALLOC);
			return;
		}		
		iMemSet(m_pofn,0,sizeof(OPENFILENAME)+16);
		m_pofn->lStructSize=sizeof(OPENFILENAME);
#else
		m_pofn=(OPENFILENAME*)new char[sizeof(OPENFILENAME)];
		if (m_pofn==NULL)
		{
			SetHFCError(HFC_CANNOTALLOC);
			return;
		}		
		ZeroMemory(m_pofn,sizeof(OPENFILENAME));
		if (GetSystemFeaturesFlag()&(efWin2000|efWinXP))
		{
			DebugMessage("CFileDialog::CFileDialog: m_pofn->lStructSize=sizeof(OPENFILENAME)");
			m_pofn->lStructSize=sizeof(OPENFILENAME);
		}
		else
		{
			DebugMessage("CFileDialog::CFileDialog: m_pofn->lStructSize=sizeof(OPENFILENAME_SIZE_VERSION_400)");
			m_pofn->lStructSize=OPENFILENAME_SIZE_VERSION_400;
		}
#endif


		m_pofn->Flags=dwFlags|OFN_EXPLORER;
		if (bSetHook)
		{
			m_pofn->lpfnHook=(LPOFNHOOKPROC)CAppData::CommonDialogProc;
			m_pofn->Flags|=OFN_ENABLEHOOK;
			m_pofn->lCustData=(LPARAM)this;
		}
		m_pofn->lpstrFileTitle=new char[65];
		m_pofn->lpstrFileTitle[0]='\0';
		m_pofn->nMaxFileTitle=64;
		m_pofn->hInstance=GetLanguageSpecificResourceHandle();
		m_pofn->lpstrCustomFilter=NULL;
		m_pofn->nMaxCustFilter=0;
		
		m_pFileName=new CHAR[MAX_PATH];
		if (m_pFileName==NULL)
		{
			SetHFCError(HFC_CANNOTALLOC);
			DebugMessage("CFileDialog::CFileDialog END ERR1");
			return;
		}
		m_pofn->nMaxFile=MAX_PATH;
		m_pofn->lpstrFile=m_pFileName;
		if (lpszFileName!=NULL)
		{
			int nLen=(int)istrlenw(lpszFileName);
			if (nLen>=MAX_PATH-1)
				nLen=MAX_PATH-2;
			WideCharToMultiByte(CP_ACP,0,lpszFileName,nLen+1,m_pFileName,nLen+1,NULL,NULL);
			m_pFileName[nLen+1]='\0';
		}
		else
		{
			m_pFileName[0]='\0';
			m_pFileName[1]='\0';
		}

		if (lpszFilter!=NULL)
			m_pFilter=alloccopyWtoA(lpszFilter);
		else
			m_pFilter=NULL;
		
		m_pofn->nFilterIndex=0;
		m_pofn->lpstrInitialDir=NULL;
		m_pofn->lpstrTitle=NULL;
		m_pofn->lpTemplateName=NULL;
		
		if (lpszDefExt!=NULL)
			m_pofn->lpstrDefExt=alloccopyWtoA(lpszDefExt);
		else
			m_pofn->lpstrDefExt=NULL;
	}
	DebugMessage("CFileDialog::CFileDialog END");
}

CFileDialog::~CFileDialog()
{
	if (IsUnicodeSystem())
	{
		if (m_pwofn!=NULL)
		{
			if (m_pwofn->lpstrTitle!=NULL)
				delete[] (LPWSTR)m_pwofn->lpstrTitle;
			if (m_pwofn->lpstrDefExt!=NULL)
				delete[] (LPWSTR)m_pwofn->lpstrDefExt;
			if (m_pwofn->lpstrFileTitle!=NULL)
				delete[] m_pwofn->lpstrFileTitle;
			delete[] (CHAR*)m_pwofn;
			m_pwofn=NULL;
		}
		if (m_pwFilter!=NULL)
			delete[] m_pwFilter;
		if (m_pwFileName!=NULL)
			delete[] m_pwFileName;
		
	}
	else
	{
		if (m_pofn!=NULL)
		{
			if (m_pofn->lpstrTitle!=NULL)
				delete[] (LPSTR)m_pofn->lpstrTitle;
			if (m_pofn->lpstrDefExt!=NULL)
				delete[] (LPSTR)m_pofn->lpstrDefExt;
			if (m_pofn->lpstrFileTitle!=NULL)
				delete[] m_pofn->lpstrFileTitle;
			delete[] (char*)m_pofn;
			m_pofn=NULL;
		}
		if (m_pFilter!=NULL)
			delete[] m_pFilter;
		if (m_pFileName!=NULL)
			delete[] m_pFileName;
	}
	
}

BOOL CFileDialog::EnableFeatures(DWORD nFlags)
{
	if (nFlags==efCheck)
		nFlags=GetSystemFeaturesFlag();

	if (IsUnicodeSystem())
	{
#if (_WIN32_WINNT >= 0x0500)
		if (nFlags&(efWin2000|efWinME) && m_pofn!=NULL)
		{
			m_pwofn->lStructSize=sizeof(OPENFILENAMEW);
		}
#else
		if (nFlags&(efWin2000|efWinME) && m_pofn!=NULL)
		{
			m_pwofn->lStructSize=sizeof(OPENFILENAMEW)+2*sizeof(DWORD)+sizeof(void*);
		}
#endif
	}
	else
	{
#if (_WIN32_WINNT >= 0x0500)
		if (nFlags&(efWin2000|efWinME) && m_pofn!=NULL)
		{
			m_pofn->lStructSize=sizeof(OPENFILENAME);
		}
#else
		if (nFlags&(efWin2000|efWinME) && m_pofn!=NULL)
		{
			m_pofn->lStructSize=sizeof(OPENFILENAME)+2*sizeof(DWORD)+sizeof(void*);
		}
#endif
	}
	return TRUE;
}

BOOL CFileDialog::DoModal(HWND hParentWnd)
{
	BOOL bError;
	int i;
		
	if (hParentWnd!=NULL)
		::EnableWindow(hParentWnd,FALSE);
	
	if (IsUnicodeSystem())
	{
		m_pwofn->hwndOwner=hParentWnd;
		m_pwofn->lpstrFilter=m_pwFilter;
		if (m_pwFilter!=NULL)
		{
			for (i=0;m_pwFilter[i]!='\0';i++)
			{
				if (m_pwFilter[i]=='|')
					m_pwFilter[i]='\0';
			}
		}
		
		
		if (m_bOpenFileDialog)
			bError=::GetOpenFileNameW(m_pwofn);
		else
			bError=::GetSaveFileNameW(m_pwofn);
	}
	else
	{
		m_pofn->hwndOwner=hParentWnd;
		m_pofn->lpstrFilter=m_pFilter;
		if (m_pFilter!=NULL)
		{
			for (i=0;m_pFilter[i]!='\0';i++)
			{
				if (m_pFilter[i]=='|')
					m_pFilter[i]='\0';
			}
		}
		
		
		if (m_bOpenFileDialog)
			bError=::GetOpenFileName(m_pofn);
		else
			bError=::GetSaveFileName(m_pofn);
	}


	m_hWnd=NULL;
	if (hParentWnd!=NULL)
	{
		::EnableWindow(hParentWnd,TRUE);
		::SetFocus(hParentWnd);
	}
	return bError;	
}


BOOL CFileDialog::GetFilePath(CString& name) const
{
	if (IsUnicodeSystem())
	{
		if (m_hWnd!=NULL)
		{
			WCHAR path[MAX_PATH];
			int nRet=(int)::SendMessage(::GetParent(m_hWnd),CDM_GETFILEPATH,MAX_PATH,(LPARAM)path);
			if (nRet<0)
				return FALSE;
			else
				name.Copy(path,nRet-1);
		}
		else
			name=m_pwFileName;
	}
	else
	{
		if (m_hWnd!=NULL)
		{
			CHAR path[MAX_PATH];
			int nRet=(int)::SendMessage(::GetParent(m_hWnd),CDM_GETFILEPATH,MAX_PATH,(LPARAM)path);
			if (nRet<0)
				return FALSE;
			else
				name.Copy(path,nRet-1);
		}
		else
			name=m_pFileName;
	}
	return TRUE;
}

BOOL CFileDialog::GetFilePath(LPSTR pFilePath,DWORD nMaxLen) const
{
	if (IsUnicodeSystem())
	{
		if (m_hWnd!=NULL)
		{
			WCHAR path[MAX_PATH];
			int nRet=(int)::SendMessage(::GetParent(m_hWnd),CDM_GETFILEPATH,MAX_PATH,(LPARAM)path);
			if (nRet<0)
				return FALSE;
			else
				WideCharToMultiByte(CP_ACP,0,path,nRet,pFilePath,nMaxLen,NULL,NULL);
		}
		else
			WideCharToMultiByte(CP_ACP,0,m_pwFileName,-1,pFilePath,nMaxLen,NULL,NULL);
	}
	else
	{
		if (m_hWnd!=NULL)
		{
			int nRet=(int)::SendMessage(::GetParent(m_hWnd),CDM_GETFILEPATH,nMaxLen,(LPARAM)pFilePath);
			if (nRet<0)
				return FALSE;
		}
		else
			strcpy_s(pFilePath,nMaxLen,m_pFileName);
	}
	return TRUE;
}


BOOL CFileDialog::GetFileName(CString& name) const
{
	if (IsUnicodeSystem())
	{
		if (m_hWnd!=NULL)
		{
			WCHAR path[MAX_PATH];
			int nRet=(int)::SendMessage(::GetParent(m_hWnd),CDM_GETSPEC,MAX_PATH,(LPARAM)path);
			if (nRet<0)
				return FALSE;
			else
				name.Copy(path,nRet-1);
		}
		else
			name=m_pwFileName+m_pwofn->nFileOffset;
	}
	else
	{
		if (m_hWnd!=NULL)
		{
			CHAR path[MAX_PATH];
			int nRet=(int)::SendMessage(::GetParent(m_hWnd),CDM_GETSPEC,MAX_PATH,(LPARAM)path);
			if (nRet<0)
				return FALSE;
			else
				name.Copy(path,nRet-1);
		}
		else
			name=m_pwFileName+m_pofn->nFileOffset;
	}
	return TRUE;
}

BOOL CFileDialog::GetFileName(LPSTR pFileName,DWORD nMaxLen) const
{
	if (IsUnicodeSystem())
	{
		if (m_hWnd!=NULL)
		{
			WCHAR path[MAX_PATH];
			int nRet=(int)::SendMessage(::GetParent(m_hWnd),CDM_GETSPEC,MAX_PATH,(LPARAM)path);
			if (nRet<0)
				return FALSE;
			else
				WideCharToMultiByte(CP_ACP,0,path,nRet,pFileName,nMaxLen,NULL,NULL);
		}
		else
			WideCharToMultiByte(CP_ACP,0,m_pwFileName+m_pwofn->nFileOffset,-1,pFileName,nMaxLen,NULL,NULL);
	}
	else
	{
		if (m_hWnd!=NULL)
		{
			int nRet=(int)::SendMessage(::GetParent(m_hWnd),CDM_GETSPEC,nMaxLen,(LPARAM)pFileName);
			if (nRet<0)
				return FALSE;
		}
		else
			strcpy_s(pFileName+m_pofn->nFileOffset,nMaxLen,m_pFileName);
	}
	return TRUE;
}



#ifdef DEF_WCHAR
BOOL CFileDialog::GetFilePath(CStringW& name) const
{
	if (IsUnicodeSystem())
	{
		if (m_hWnd!=NULL)
		{
			WCHAR path[MAX_PATH];
			int nRet=(int)::SendMessage(::GetParent(m_hWnd),CDM_GETFILEPATH,MAX_PATH,(LPARAM)path);
			if (nRet<0)
				return FALSE;
			else
				name.Copy(path,nRet-1);
		}
		else
			name=m_pwFileName;
	}
	else
	{
		if (m_hWnd!=NULL)
		{
			CHAR path[MAX_PATH];
			int nRet=(int)::SendMessage(::GetParent(m_hWnd),CDM_GETFILEPATH,MAX_PATH,(LPARAM)path);
			if (nRet<0)
				return FALSE;
			else
				name.Copy(path,nRet-1);
		}
		else
			name=m_pFileName;
	}
	return TRUE;
}

BOOL CFileDialog::GetFilePath(LPWSTR pFilePath,DWORD nMaxLen) const
{
	if (IsUnicodeSystem())
	{
		if (m_hWnd!=NULL)
		{
			int nRet=(int)::SendMessage(::GetParent(m_hWnd),CDM_GETFILEPATH,nMaxLen,(LPARAM)pFilePath);
			if (nRet<0)
				return FALSE;
		}
		else
			wcscpy_s(pFilePath,nMaxLen,m_pwFileName);
	}
	else
	{
		if (m_hWnd!=NULL)
		{
			CHAR path[MAX_PATH];
			int nRet=(int)::SendMessage(::GetParent(m_hWnd),CDM_GETFILEPATH,MAX_PATH,(LPARAM)path);
			if (nRet<0)
				return FALSE;
			else
				MultiByteToWideChar(CP_ACP,0,path,nRet,pFilePath,nMaxLen);
		}
		else
			MultiByteToWideChar(CP_ACP,0,m_pFileName,-1,pFilePath,nMaxLen);
	}
	return TRUE;
}

BOOL CFileDialog::GetFileName(CStringW& name) const
{
	if (IsUnicodeSystem())
	{
		if (m_hWnd!=NULL)
		{
			WCHAR path[MAX_PATH];
			int nRet=(int)::SendMessage(::GetParent(m_hWnd),CDM_GETSPEC,MAX_PATH,(LPARAM)path);
			if (nRet<0)
				return FALSE;
			else
				name.Copy(path,nRet-1);
		}
		else
			name=m_pwFileName+m_pofn->nFileOffset;
	}
	else
	{
		if (m_hWnd!=NULL)
		{
			CHAR path[MAX_PATH];
			int nRet=(int)::SendMessage(::GetParent(m_hWnd),CDM_GETSPEC,MAX_PATH,(LPARAM)path);
			if (nRet<0)
				return FALSE;
			else
				name.Copy(path,nRet-1);
		}
		else
			name=m_pFileName+m_pofn->nFileOffset;
	}
	return TRUE;
}

BOOL CFileDialog::GetFileName(LPWSTR pFileName,DWORD nMaxLen) const
{
	if (IsUnicodeSystem())
	{
		if (m_hWnd!=NULL)
		{
			int nRet=(int)::SendMessage(::GetParent(m_hWnd),CDM_GETSPEC,nMaxLen,(LPARAM)pFileName);
			if (nRet<0)
				return FALSE;
		}
		else
			wcscpy_s(pFileName+m_pwofn->nFileOffset,nMaxLen,m_pwFileName);
	}
	else
	{
		if (m_hWnd!=NULL)
		{
			CHAR path[MAX_PATH];
			int nRet=(int)::SendMessage(::GetParent(m_hWnd),CDM_GETSPEC,MAX_PATH,(LPARAM)path);
			if (nRet<0)
				return FALSE;
			MultiByteToWideChar(CP_ACP,0,path,nRet,pFileName,nMaxLen);
		}
		else
			MultiByteToWideChar(CP_ACP,0,m_pFileName+m_pofn->nFileOffset,-1,pFileName,nMaxLen);
	}
	return TRUE;
}



#endif

BOOL CFileDialog::GetReadOnlyPref() const
{
	if (m_pofn->Flags&OFN_READONLY)
		return TRUE;
	return FALSE;
}

void CFileDialog::SetTemplate(LPCTSTR lpID,TypeOfResourceHandle bType)
{
	m_pofn->lpTemplateName=lpID;
	m_pofn->hInstance=GetResourceHandle(bType);
	if (lpID==NULL)
		m_pofn->Flags&=~OFN_ENABLETEMPLATE;
	else
		m_pofn->Flags|=OFN_ENABLETEMPLATE;
}

BOOL CFileDialog::GetFolderPath(CString& name) const
{
	if (IsUnicodeSystem())
	{
		if (m_hWnd!=NULL)
		{
			WCHAR path[MAX_PATH];
			int nRet=(int)::SendMessage(::GetParent(m_hWnd),CDM_GETFOLDERPATH,MAX_PATH,(LPARAM)path);
			if (nRet<0)
				return FALSE;
			else
				name.Copy(path,nRet-1);
		}
		else
			name.Copy(m_pwFileName,m_pofn->nFileOffset);
	}
	else
	{
		if (m_hWnd!=NULL)
		{
			CHAR path[MAX_PATH];
			int nRet=(int)::SendMessage(::GetParent(m_hWnd),CDM_GETFOLDERPATH,MAX_PATH,(LPARAM)path);
			if (nRet<0)
				return FALSE;
			else
				name.Copy(path,nRet-1);
		}
		else
			name.Copy(m_pFileName,m_pofn->nFileOffset);
	}
	return TRUE;
}

#ifdef DEF_WCHAR
BOOL CFileDialog::GetFolderPath(CStringW& name) const
{
	if (IsUnicodeSystem())
	{
		if (m_hWnd!=NULL)
		{
			WCHAR path[MAX_PATH];
			int nRet=(int)::SendMessage(::GetParent(m_hWnd),CDM_GETFOLDERPATH,MAX_PATH,(LPARAM)path);
			if (nRet<0)
				return FALSE;
			else
				name.Copy(path,nRet-1);
		}
		else
			name.Copy(m_pwFileName,m_pofn->nFileOffset);
	}
	else
	{
		if (m_hWnd!=NULL)
		{
			CHAR path[MAX_PATH];
			int nRet=(int)::SendMessage(::GetParent(m_hWnd),CDM_GETFOLDERPATH,MAX_PATH,(LPARAM)path);
			if (nRet<0)
				return FALSE;
			else
				name.Copy(path,nRet-1);
		}
		else
			name.Copy(m_pFileName,m_pofn->nFileOffset);
	}
	return TRUE;
}
#endif

void CFileDialog::SetDefExt(LPCSTR lpsz)
{
	if (IsUnicodeSystem())
	{
		if (m_hWnd!=NULL)
		{
			::SendMessage(::GetParent(m_hWnd),CDM_SETDEFEXT,0,(LPARAM)(LPCWSTR)A2W(lpsz));
			return;
		}

		if (m_pwofn->lpstrDefExt!=NULL)
			delete[] (LPWSTR)m_pwofn->lpstrDefExt;
		
		if (lpsz!=NULL)
		{
			int nLen=(int)istrlen(lpsz)+1;
			m_pwofn->lpstrDefExt=new WCHAR[nLen];
			if (m_pwofn->lpstrDefExt==NULL)
			{
				SetHFCError(HFC_CANNOTALLOC);
				return;
			}
			MultiByteToWideChar(CP_ACP,0,lpsz,(int)nLen,(LPWSTR)m_pwofn->lpstrDefExt,(int)nLen);
		}
		else
			m_pwofn->lpstrDefExt=NULL;
	}
	else
	{
		if (m_hWnd!=NULL)
		{
			::SendMessage(::GetParent(m_hWnd),CDM_SETDEFEXT,0,(LPARAM)lpsz);
			return;
		}

		if (m_pofn->lpstrDefExt!=NULL)
			delete[] (LPSTR)m_pofn->lpstrDefExt;
		
		if (lpsz!=NULL)
		{
			size_t nLen=istrlen(lpsz)+1;
			m_pofn->lpstrDefExt=new char[nLen];
			if (m_pwofn->lpstrDefExt==NULL)
			{
				SetHFCError(HFC_CANNOTALLOC);
				return;
			}
			CopyMemory((LPSTR)m_pofn->lpstrDefExt,lpsz,nLen);
		}
		else
			m_pwofn->lpstrDefExt=NULL;
	}

}

UINT CFileDialog::OnShareViolation(LPCTSTR lpszPathName)
{
	return OFN_SHAREWARN;
}

BOOL CFileDialog::OnFileNameOK()
{
	return 0;
}

void CFileDialog::OnInitDone()
{
}

void CFileDialog::OnFileNameChange()
{
}

void CFileDialog::OnFolderChange()
{
}

void CFileDialog::OnTypeChange()
{
}

BOOL CFileDialog::OnNotify(int idCtrl,LPNMHDR pnmh)
{
	if (CCommonDialog::OnNotify(idCtrl,pnmh))
		return TRUE;
	switch(pnmh->code)
	{
	case CDN_SHAREVIOLATION:
		return OnShareViolation(((LPOFNOTIFY)pnmh)->pszFile);
	case CDN_FILEOK:
		return OnFileNameOK();
	case CDN_SELCHANGE:
		OnFileNameChange();
		break;
	case CDN_INITDONE:
		OnInitDone();
		break;
	case CDN_TYPECHANGE:
		OnTypeChange();
		break;
	case CDN_FOLDERCHANGE:
		OnFolderChange();
		break;
	}
	return 0;
}

///////////////////////////
// Class CFontDialog
///////////////////////////

CFontDialog::CFontDialog(LPLOGFONT lplfInitial,DWORD dwFlags,HDC hdcPrinter,BOOL bSetHook)
:	CCommonDialog()
{
	ZeroMemory(&m_cf,sizeof(CHOOSEFONT));

	m_cf.lStructSize=sizeof(CHOOSEFONT);
	m_cf.Flags=dwFlags;
	if (lplfInitial!=NULL)
	{
		m_lf=*lplfInitial;
		m_cf.Flags|=CF_INITTOLOGFONTSTRUCT;
	}
	m_cf.lpLogFont=&m_lf;
	m_cf.rgbColors=NULL;
	if (bSetHook)
	{
		m_cf.lCustData=(LPARAM)this;
		m_cf.Flags|=CF_ENABLEHOOK;
		m_cf.lpfnHook=(LPOFNHOOKPROC)CAppData::CommonDialogProc;
	}
	m_cf.lpTemplateName=NULL;
	m_cf.hInstance=GetLanguageSpecificResourceHandle();
	m_cf.lpszStyle=m_szStyleName;
	if (hdcPrinter!=NULL)
	{
		m_cf.hDC=hdcPrinter;
		m_cf.Flags|=CF_PRINTERFONTS;
	}
	m_szStyleName[0]='\0';
}

CFontDialog::CFontDialog(const CHARFORMAT& charformat,DWORD dwFlags,HDC hdcPrinter,BOOL bSetHook)
:	CCommonDialog()
{
	ZeroMemory(&m_cf,sizeof(CHOOSEFONT));

	m_cf.lStructSize=sizeof(CHOOSEFONT);
	m_cf.Flags=dwFlags|CF_INITTOLOGFONTSTRUCT;
	FillInLogFont(charformat);
	m_cf.lpLogFont=&m_lf;
	m_cf.rgbColors=NULL;
	if (bSetHook)
	{
		m_cf.lCustData=(LPARAM)this;
		m_cf.Flags|=CF_ENABLEHOOK;
		m_cf.lpfnHook=(LPOFNHOOKPROC)CAppData::CommonDialogProc;
	}
	m_cf.lpTemplateName=NULL;
	m_cf.hInstance=GetLanguageSpecificResourceHandle();
	m_cf.lpszStyle=m_szStyleName;
	m_szStyleName[0]='\0';
	if (hdcPrinter!=NULL)
	{
		m_cf.hDC=hdcPrinter;
		m_cf.Flags|=CF_PRINTERFONTS;
	}
}

BOOL CFontDialog::DoModal(HWND hParentWnd)
{
	BOOL ret;
	m_cf.hwndOwner=hParentWnd;
	ret=ChooseFont(&m_cf);
	m_hWnd=NULL;
	return ret;
}

void CFontDialog::GetCurrentFont(LPLOGFONT lplf)
{
	if (lplf!=NULL)
	{
		if (m_hWnd!=NULL)
			::SendMessage(m_hWnd,WM_CHOOSEFONT_GETLOGFONT,0,(LPARAM)lplf);
		else
			*lplf=m_lf;
	}
}

void CFontDialog::GetCharFormat(CHARFORMAT& cf) const
{
	cf.cbSize=sizeof(CHARFORMAT);
	cf.dwMask=CFM_BOLD|CFM_COLOR|CFM_FACE|CFM_ITALIC|CFM_SIZE|CFM_STRIKEOUT|CFM_UNDERLINE;
	cf.dwEffects=0;
	if (m_lf.lfItalic)
		cf.dwEffects|=CFE_ITALIC;
	if (m_lf.lfWeight>=600)
		cf.dwEffects|=CFE_BOLD;
	if (m_lf.lfStrikeOut)
		cf.dwEffects|=CFE_STRIKEOUT;
	if (m_lf.lfUnderline)
		cf.dwEffects|=CFE_UNDERLINE;
	cf.yHeight=m_cf.iPointSize;
	cf.crTextColor=m_cf.rgbColors;
	cf.bCharSet=m_lf.lfCharSet;
	cf.bPitchAndFamily=m_lf.lfPitchAndFamily;
	StringCbCopy(cf.szFaceName,LF_FACESIZE,m_lf.lfFaceName);
}

DWORD CFontDialog::FillInLogFont(const CHARFORMAT& cf)
{
	m_cf.rgbColors=(cf.dwMask&CFM_COLOR?cf.crTextColor:0);
	m_cf.Flags|=CF_EFFECTS;
	
	if (cf.dwMask&CFM_SIZE)
	{
		HDC hDC=CreateDC("DISPLAY",NULL,NULL,NULL);
		m_lf.lfHeight=-MulDiv(cf.yHeight,::GetDeviceCaps(hDC,LOGPIXELSY),72);
		DeleteDC(hDC);
	}
	else
		m_lf.lfHeight=0;
	m_lf.lfWidth=0;
	m_lf.lfEscapement=0;
	m_lf.lfOrientation=0;
	m_lf.lfWeight=((cf.dwMask&CFM_BOLD && cf.dwEffects&CFE_BOLD)?700:500);
	m_lf.lfItalic=(cf.dwMask&CFM_ITALIC && cf.dwEffects&CFE_ITALIC);
	m_lf.lfUnderline=(cf.dwMask&CFM_UNDERLINE && cf.dwEffects&CFE_UNDERLINE);
	m_lf.lfStrikeOut=(cf.dwMask&CFM_STRIKEOUT && cf.dwEffects&CFE_STRIKEOUT);
	m_lf.lfCharSet=(cf.dwMask&CFM_CHARSET?cf.bCharSet:DEFAULT_CHARSET);
	m_lf.lfOutPrecision=OUT_DEFAULT_PRECIS;
	m_lf.lfClipPrecision=CLIP_DEFAULT_PRECIS;
	m_lf.lfQuality=DEFAULT_QUALITY;
	if (cf.dwMask&CFM_FACE)
	{
		m_lf.lfPitchAndFamily=cf.bPitchAndFamily;
		StringCbCopy(m_lf.lfFaceName,LF_FACESIZE,cf.szFaceName);
	}
	else
	{
		m_lf.lfPitchAndFamily=DEFAULT_PITCH|FF_DONTCARE;
		m_lf.lfFaceName[0]='\0';
	}
	return TRUE;
}

///////////////////////////
// Class CColorDialog
///////////////////////////

CColorDialog::CColorDialog(COLORREF clrInit,DWORD dwFlags,BOOL bSetHook)
:	CCommonDialog()
{
	ZeroMemory(&m_cc,sizeof(CHOOSECOLOR));
	m_cc.lStructSize=sizeof(CHOOSECOLOR);
	m_cc.Flags=dwFlags;
	m_cc.hInstance=(HWND)GetLanguageSpecificResourceHandle();
	if (m_cc.rgbResult=clrInit)
		m_cc.Flags|=CC_RGBINIT;
	
	if (bSetHook)
	{
		m_cc.lCustData=(LPARAM)this;
		m_cc.Flags|=CC_ENABLEHOOK;
		m_cc.lpfnHook=(LPOFNHOOKPROC)CAppData::CommonDialogProc;
	}


	//m_cc.lpTemplateName=NULL;

	
}



BOOL CColorDialog::DoModal(HWND hParentWnd)
{
	BOOL ret;
	m_cc.hwndOwner=hParentWnd;
	COLORREF aCustomColors[16];
	FillMemory(aCustomColors,sizeof(COLORREF)*16,255);

	if (m_cc.lpCustColors==NULL)
		m_cc.lpCustColors=aCustomColors;

	ret=ChooseColor(&m_cc);
	
	if (m_cc.lpCustColors==aCustomColors)
		m_cc.lpCustColors=NULL;

	m_hWnd=NULL;
	return ret;
}

///////////////////////////
// Class CPageSetupDialog
///////////////////////////

LRESULT CALLBACK CAppData::PagePaintProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	return ((CPageSetupDialog*)GetAppData()->m_pCommonDialog)->OnDrawPage((HDC)wParam,uMsg,(LPRECT)lParam);
}

CPageSetupDialog::CPageSetupDialog(DWORD dwFlags,BOOL bSetHook)
:	CCommonDialog()
{
	ZeroMemory(&m_psd,sizeof(PAGESETUPDLG));

	m_psd.lStructSize=sizeof(PAGESETUPDLG);
	m_psd.hDevMode=NULL;
	m_psd.hDevNames=NULL;
	m_psd.Flags=dwFlags|PSD_ENABLEPAGESETUPHOOK|PSD_ENABLEPAGEPAINTHOOK;
	fMemSet(&m_psd.rtMinMargin,0,sizeof(RECT));
	fMemSet(&m_psd.rtMargin,0,sizeof(RECT));
	m_psd.hInstance=GetLanguageSpecificResourceHandle();
	if (bSetHook)
	{
		m_psd.lCustData=(LPARAM)this;
		m_psd.lpfnPageSetupHook=(LPPAGESETUPHOOK)CAppData::CommonDialogProc;
		m_psd.lpfnPagePaintHook=(LPPAGEPAINTHOOK)CAppData::PagePaintProc;
	}
	GetAppData()->m_pCommonDialog=this;
	m_psd.lpPageSetupTemplateName=NULL;
	m_psd.hPageSetupTemplate=NULL;
}

CPageSetupDialog::~CPageSetupDialog()
{
	if (m_psd.hDevMode!=NULL)
		GlobalFree(m_psd.hDevMode);
	if (m_psd.hDevNames!=NULL)
		GlobalFree(m_psd.hDevNames);
}

LPDEVMODE CPageSetupDialog::GetDevMode() const
{
	if (m_psd.hDevMode==NULL)
		return NULL;
	return (LPDEVMODE)GlobalLock(m_psd.hDevMode);
}

CString CPageSetupDialog::GetDriverName() const
{
	if (m_psd.hDevNames==NULL)
		return szEmpty;
	LPDEVNAMES lpdn=(LPDEVNAMES)GlobalLock(m_psd.hDevNames);
	CString str((LPCSTR)lpdn+lpdn->wDriverOffset);
	GlobalUnlock(m_psd.hDevNames);
	return str;
}

CString CPageSetupDialog::GetDeviceName() const
{
	if (m_psd.hDevNames==NULL)
		return szEmpty;
	LPDEVNAMES lpdn=(LPDEVNAMES)GlobalLock(m_psd.hDevNames);
	CString str((LPCSTR)lpdn+lpdn->wDeviceOffset);
	GlobalUnlock(m_psd.hDevNames);
	return str;
}

CString CPageSetupDialog::GetPortName() const
{
	if (m_psd.hDevNames==NULL)
		return szEmpty;
	LPDEVNAMES lpdn=(LPDEVNAMES)GlobalLock(m_psd.hDevNames);
	CString str((LPCSTR)lpdn+lpdn->wOutputOffset);
	GlobalUnlock(m_psd.hDevNames);
	return str;
}

HDC CPageSetupDialog::CreatePrinterDC()
{
	return CreateDC((LPCSTR)m_psd.hDevNames+((DEVNAMES*)m_psd.hDevNames)->wDriverOffset,
		(LPCSTR)m_psd.hDevNames+((DEVNAMES*)m_psd.hDevNames)->wDeviceOffset,
		NULL,(CONST DEVMODE*)&m_psd.hDevMode);
}

CSize CPageSetupDialog::GetPaperSize() const
{
	return (m_psd.rtMargin.right-m_psd.rtMargin.left,m_psd.rtMargin.bottom-m_psd.rtMargin.top);
}

void CPageSetupDialog::GetMargins(LPRECT lpRectMargins,LPRECT lpRectMinMargins) const
{
	if (lpRectMargins!=NULL)
		*lpRectMargins=m_psd.rtMargin;
	if (lpRectMinMargins!=NULL)
		*lpRectMinMargins=m_psd.rtMinMargin;
}

BOOL CPageSetupDialog::DoModal(HWND hParentWnd)
{
	BOOL ret;
	m_psd.hwndOwner=hParentWnd;
	ret=PageSetupDlg(&m_psd);
	GetAppData()->m_pCommonDialog=NULL;
	m_hWnd=NULL;
	return ret;
}

UINT CPageSetupDialog::OnDrawPage(HDC hDC,UINT nMessage,LPRECT lpRect)
{
	return FALSE;
}

///////////////////////////
// Class CPrintDialog
///////////////////////////

CPrintDialog::CPrintDialog(BOOL bPrintSetupOnly,DWORD dwFlags,BOOL bSetHook)
{
	ZeroMemory(&m_pd,sizeof(PRINTDLG));

	m_pd.lStructSize=sizeof(PRINTDLG);
	m_pd.hDC=NULL;
	m_pd.Flags=dwFlags|PD_ENABLEPRINTHOOK;
	if (bPrintSetupOnly)
		m_pd.Flags|=PD_PRINTSETUP;
	else
		m_pd.Flags|=PD_RETURNDC;
	m_pd.hInstance=GetLanguageSpecificResourceHandle();
	if (bSetHook)
	{
		m_pd.lCustData=(LPARAM)this;
		m_pd.lpfnPrintHook=(LPPRINTHOOKPROC)CAppData::CommonDialogProc;
	}
	m_pd.lpfnSetupHook=NULL;
	m_pd.lpPrintTemplateName=NULL;
	m_pd.lpSetupTemplateName=NULL;
	m_pd.nFromPage=1;
	m_pd.nToPage=1;
	m_pd.nMinPage=1;
	m_pd.nMaxPage=1;
	m_pd.nCopies=1;
}

CPrintDialog::~CPrintDialog()
{
	if (m_pd.hDevMode!=NULL)
		GlobalFree(m_pd.hDevMode);
	if (m_pd.hDevNames!=NULL)
		GlobalFree(m_pd.hDevNames);
}

BOOL CPrintDialog::DoModal(HWND hParentWnd)
{
	BOOL ret;
	m_pd.hwndOwner=hParentWnd;
	ret=PrintDlg(&m_pd);
	m_hWnd=NULL;
	return ret;
}

BOOL CPrintDialog::GetDefaults()
{
	BOOL ret;
	m_pd.Flags|=PD_RETURNDEFAULT;
	ret=::PrintDlg(&m_pd);
	m_pd.Flags&=~PD_RETURNDEFAULT;
	return ret;
}

int CPrintDialog::GetCopies() const
{
	if (m_pd.Flags&PD_USEDEVMODECOPIES && m_pd.hDevMode!=NULL)
	{
		LPDEVMODE lpdm=(LPDEVMODE)GlobalLock(m_pd.hDevMode);
		int ret=lpdm->dmCopies;
		GlobalUnlock(m_pd.hDevMode);
		return ret;
	}
	return m_pd.nCopies;
}

LPDEVMODE CPrintDialog::GetDevMode() const
{
	LPDEVMODE lpdm=(LPDEVMODE)GlobalAlloc(GMEM_FIXED,sizeof(DEVMODE));
	LPDEVMODE lpmdm=(LPDEVMODE)GlobalLock(m_pd.hDevMode);
	MemCopy(lpdm,lpmdm,sizeof(DEVMODE));
	GlobalUnlock(m_pd.hDevMode);
	return lpdm;
}

CString CPrintDialog::GetDriverName() const
{
	if (m_pd.hDevNames==NULL)
		return szEmpty;
	LPDEVNAMES lpdn=(LPDEVNAMES)GlobalLock(m_pd.hDevNames);
	CString str((LPCSTR)lpdn+lpdn->wDriverOffset);
	GlobalUnlock(m_pd.hDevNames);
	return str;
}

CString CPrintDialog::GetDeviceName() const
{
	if (m_pd.hDevNames==NULL)
		return szEmpty;
	LPDEVNAMES lpdn=(LPDEVNAMES)GlobalLock(m_pd.hDevNames);
	CString str((LPCSTR)lpdn+lpdn->wDeviceOffset);
	GlobalUnlock(m_pd.hDevNames);
	return str;
}

CString CPrintDialog::GetPortName() const
{
	if (m_pd.hDevNames==NULL)
		return szEmpty;
	LPDEVNAMES lpdn=(LPDEVNAMES)GlobalLock(m_pd.hDevNames);
	CString str((LPCSTR)lpdn+lpdn->wOutputOffset);
	GlobalUnlock(m_pd.hDevNames);
	return str;
}

HDC CPrintDialog::CreatePrinterDC()
{
	HDC ret;
	LPDEVNAMES lpdn=(LPDEVNAMES)GlobalLock(m_pd.hDevNames);
	LPDEVMODE lpdm=(LPDEVMODE)GlobalLock(m_pd.hDevMode);
	ret=CreateDC((LPCSTR)lpdn+lpdn->wDriverOffset,(LPCSTR)lpdn+lpdn->wDeviceOffset,
		NULL,lpdm);
	GlobalUnlock(m_pd.hDevNames);
	GlobalUnlock(m_pd.hDevMode);
	return ret;
}

///////////////////////////
// Class CFindReplaceDialog
///////////////////////////

CFindReplaceDialog::CFindReplaceDialog(BOOL bSetHook)
{	
	ZeroMemory(&m_fr,sizeof(FINDREPLACE));
	m_fr.lStructSize=sizeof(FINDREPLACE);
	m_fr.hInstance=GetLanguageSpecificResourceHandle();
	m_fr.lpstrFindWhat=m_szFindWhat;
	m_fr.lpstrReplaceWith=m_szReplaceWith;
	m_fr.wFindWhatLen=128;
	m_fr.wReplaceWithLen=128;
	if (bSetHook)
	{
		m_fr.Flags|=FR_ENABLEHOOK;
		m_fr.lCustData=(LPARAM)this;
		m_fr.lpfnHook=(LPFRHOOKPROC)CAppData::CommonDialogProc;
	}
	m_fr.lpTemplateName=NULL;
}

BOOL CFindReplaceDialog::Create(BOOL bFindDialogOnly,LPCTSTR lpszFindWhat,LPCTSTR lpszReplaceWith,DWORD dwFlags,HWND hParentWnd)
{
	m_fr.hwndOwner=hParentWnd;
	m_fr.Flags|=dwFlags;
	StringCbCopy(m_szFindWhat,128,lpszFindWhat);
	StringCbCopy(m_szReplaceWith,128,lpszReplaceWith);
	if (bFindDialogOnly)
		m_hWnd=FindText(&m_fr);
	else
		m_hWnd=ReplaceText(&m_fr);
	if (m_hWnd==NULL)
		return FALSE;
	return TRUE;
}


///////////////////////////
// Class CFolderDialog
///////////////////////////

LRESULT CALLBACK CAppData::FolderDialogProc(HWND hWnd,UINT uMsg,LPARAM lParam,LPARAM lpData)
{
	CFolderDialog* pDlg=(CFolderDialog*)lpData;
	switch (uMsg)
	{
	case BFFM_INITIALIZED:
		pDlg->m_hWnd=hWnd;
		return (pDlg->OnInitialized()?0:1);
	case BFFM_SELCHANGED:
		return (pDlg->OnSelChanged((LPITEMIDLIST)lParam)?0:1);
	case BFFM_VALIDATEFAILED :
		return (pDlg->OnValidateFailed((LPITEMIDLIST)lParam)?0:1);
	}
	return FALSE;
}

CFolderDialog::CFolderDialog(LPCSTR lpszTitle,UINT ulFlags,LPCITEMIDLIST pidlRoot)
:	m_hWnd(NULL),m_lpil(NULL),m_lpDefaultIL(NULL),
	m_szTitle(NULL),m_szDefaultFolder(NULL)
{
	CoInitialize(NULL);
	m_szwDisplayName[0]='\0';
	
	
	if (IsUnicodeSystem())
	{
		m_szwTitle=alloccopyAtoW(lpszTitle);
		m_biw.pszDisplayName=m_szwDisplayName;
		m_biw.ulFlags=ulFlags;
		m_biw.pidlRoot=pidlRoot;
		m_biw.lpfn=(BFFCALLBACK)CAppData::FolderDialogProc;
		m_biw.iImage=0;
	}
	else
	{
		m_szTitle=alloccopy(lpszTitle);
		m_bi.pszDisplayName=m_szDisplayName;
		m_bi.ulFlags=ulFlags;
		m_bi.pidlRoot=pidlRoot;
		m_bi.lpfn=(BFFCALLBACK)CAppData::FolderDialogProc;
		m_bi.iImage=0;
	}

}

CFolderDialog::CFolderDialog(LPCWSTR lpszTitle,UINT ulFlags,LPCITEMIDLIST pidlRoot)
:	m_hWnd(NULL),m_lpil(NULL),m_lpDefaultIL(NULL),
	m_szTitle(NULL),m_szDefaultFolder(NULL)
{
	CoInitialize(NULL);
	m_szwDisplayName[0]='\0';
	
	
	if (IsUnicodeSystem())
	{
		m_szwTitle=alloccopy(lpszTitle);
		m_biw.pszDisplayName=m_szwDisplayName;
		m_biw.ulFlags=ulFlags;
		m_biw.pidlRoot=pidlRoot;
		m_biw.lpfn=(BFFCALLBACK)CAppData::FolderDialogProc;
		m_biw.iImage=0;
	}
	else
	{
		m_szTitle=alloccopyWtoA(lpszTitle);
		m_bi.pszDisplayName=m_szDisplayName;
		m_bi.ulFlags=ulFlags;
		m_bi.pidlRoot=pidlRoot;
		m_bi.lpfn=(BFFCALLBACK)CAppData::FolderDialogProc;
		m_bi.iImage=0;
	}

}

CFolderDialog::CFolderDialog(UINT nTitleID,UINT ulFlags,LPCITEMIDLIST pidlRoot)
:	m_hWnd(NULL),m_lpil(NULL),m_lpDefaultIL(NULL),
	m_szTitle(NULL),m_szDefaultFolder(NULL)
{
	CoInitialize(NULL);
	m_szwDisplayName[0]='\0';

	if (IsUnicodeSystem())
	{
		m_szwTitle=allocstringW(nTitleID);
		m_biw.pszDisplayName=m_szwDisplayName;
		m_biw.ulFlags=ulFlags;
		m_biw.pidlRoot=pidlRoot;
		m_biw.lpfn=(BFFCALLBACK)CAppData::FolderDialogProc;
		m_biw.iImage=0;
	}
	else
	{
		m_szTitle=allocstring(nTitleID);
		m_bi.pszDisplayName=m_szDisplayName;
		m_bi.ulFlags=ulFlags;
		m_bi.pidlRoot=pidlRoot;
		m_bi.lpfn=(BFFCALLBACK)CAppData::FolderDialogProc;
		m_bi.iImage=0;
	}
}
	
CFolderDialog::~CFolderDialog()
{
	if (IsUnicodeSystem())
	{
		if (m_szwTitle!=NULL)
			delete[] m_szwTitle;
		if (m_szwDefaultFolder!=NULL)
			delete[] m_szwDefaultFolder;
	}
	else
	{
		if (m_szTitle!=NULL)
			delete[] m_szTitle;
		if (m_szDefaultFolder!=NULL)
			delete[] m_szDefaultFolder;
	}

	if (m_lpil!=NULL)
	{
		CoTaskMemFree(m_lpil);
		m_lpil=NULL;
	}
	CoUninitialize();
}

BOOL CFolderDialog::DoModal(HWND hOwner)
{
	if (m_lpil!=NULL)
	{
		CoTaskMemFree(m_lpil);
		m_lpil=NULL;
	}
	
	if (IsUnicodeSystem())
	{
		m_biw.lpszTitle=m_szwTitle;
		m_biw.lParam=(LPARAM)this;
		m_biw.hwndOwner=hOwner;
		m_lpil=SHBrowseForFolderW(&m_biw);
	}
	else
	{
		m_bi.lpszTitle=m_szTitle;
		m_bi.lParam=(LPARAM)this;
		m_bi.hwndOwner=hOwner;
		m_lpil=SHBrowseForFolder(&m_bi);
	}

	if (m_lpil==NULL)
		return FALSE;
	return TRUE;
}


BOOL CFolderDialog::GetDisplayName(CString& strDisplayName) const
{
	if (IsUnicodeSystem())
		strDisplayName.Copy(m_szwDisplayName);
	else
		strDisplayName.Copy(m_szDisplayName);
	return TRUE;
}

BOOL CFolderDialog::GetDisplayName(CStringW& strDisplayName) const
{
	if (IsUnicodeSystem())
		strDisplayName.Copy(m_szwDisplayName);
	else
		strDisplayName.Copy(m_szDisplayName);
	return TRUE;
}


BOOL CFolderDialog::GetDisplayName(LPSTR szDisplayName,DWORD nSize)
{
	if (!nSize)
		return FALSE;
	
	if (IsUnicodeSystem())
		WideCharToMultiByte(CP_ACP,0,m_szwDisplayName,-1,szDisplayName,nSize,NULL,NULL);
	else
	{
		int nDisplayNameLen=istrlen(m_szDisplayName);
		if ((int)nSize<=nDisplayNameLen)
		{
			MemCopy(szDisplayName,m_szDisplayName,nSize-1);
			szDisplayName[nSize-1]='\0';
		}
		else
			MemCopy(szDisplayName,m_szDisplayName,nDisplayNameLen+1);
	}

	return TRUE;
}

BOOL CFolderDialog::GetDisplayName(LPWSTR szDisplayName,DWORD nSize)
{
	if (!nSize)
		return FALSE;
	
	if (IsUnicodeSystem())
	{
		int nDisplayNameLen=istrlenw(m_szwDisplayName);

		if ((int)nSize<=nDisplayNameLen)
		{
			MemCopyW(szDisplayName,m_szwDisplayName,nSize-1);
			szDisplayName[nSize-1]='\0';
		}
		else
			MemCopyW(szDisplayName,m_szwDisplayName,nDisplayNameLen+1);
	}
	else
		MultiByteToWideChar(CP_ACP,0,m_szDisplayName,-1,szDisplayName,nSize);

	return TRUE;
}


BOOL CFolderDialog::EnableOK(BOOL bEnable)
{
	if (m_hWnd==NULL)
		return FALSE;
	SendMessage(m_hWnd,BFFM_ENABLEOK,(WPARAM)bEnable,0);
	return TRUE;
}

BOOL CFolderDialog::SetSelection(LPITEMIDLIST lpil)
{
	if (m_hWnd==NULL)
	{
		m_lpDefaultIL=lpil;
		if (IsUnicodeSystem())
		{
			if (m_szwDefaultFolder!=NULL)
				delete[] m_szwDefaultFolder;
		}
		else
		{
			if (m_szDefaultFolder!=NULL)
				delete[] m_szDefaultFolder;
		}
		m_szDefaultFolder=NULL;
		return TRUE;
	}
	SendMessage(m_hWnd,BFFM_SETSELECTION,FALSE,(LPARAM)lpil);
	return TRUE;
}

BOOL CFolderDialog::SetSelection(LPCSTR lpFolder)
{
	if (m_hWnd==NULL)
	{
		m_lpDefaultIL=NULL;
		if (IsUnicodeSystem())
		{
			if (m_szwDefaultFolder!=NULL)
				delete[] m_szwDefaultFolder;
			m_szwDefaultFolder=alloccopyAtoW(lpFolder);
		}
		else
		{
			if (m_szDefaultFolder!=NULL)
				delete[] m_szDefaultFolder;
			m_szDefaultFolder=alloccopy(lpFolder);
		}
		return TRUE;
	}
	SendMessage(m_hWnd,BFFM_SETSELECTION,TRUE,(LPARAM)lpFolder);
	return TRUE;
}

BOOL CFolderDialog::SetSelection(LPCWSTR lpFolder)
{
	if (m_hWnd==NULL)
	{
		m_lpDefaultIL=NULL;
		if (IsUnicodeSystem())
		{
			if (m_szwDefaultFolder!=NULL)
				delete[] m_szwDefaultFolder;
			m_szwDefaultFolder=alloccopy(lpFolder);
		}
		else
		{
			if (m_szDefaultFolder!=NULL)
				delete[] m_szDefaultFolder;
			m_szDefaultFolder=alloccopyWtoA(lpFolder);
		}
		return TRUE;
	}
	if (IsUnicodeSystem())
		SendMessageW(m_hWnd,BFFM_SETSELECTIONW,TRUE,(LPARAM)lpFolder);
	else
		SendMessage(m_hWnd,BFFM_SETSELECTION,TRUE,(LPARAM)(LPCSTR)W2A(lpFolder));
	return TRUE;
}

BOOL CFolderDialog::SetStatusText(LPCSTR lpStatus)
{
	if (m_hWnd==NULL)
		return FALSE;
	SendMessage(m_hWnd,BFFM_SETSTATUSTEXT,0,(LPARAM)lpStatus);
	return TRUE;
}

BOOL CFolderDialog::SetStatusText(LPCWSTR lpStatus)
{
	if (m_hWnd==NULL)
		return FALSE;
	if (IsUnicodeSystem())
		SendMessageW(m_hWnd,BFFM_SETSTATUSTEXTW,0,(LPARAM)lpStatus);
	else
		SendMessage(m_hWnd,BFFM_SETSTATUSTEXT,0,(LPARAM)(LPCSTR)W2A(lpStatus));

	return TRUE;
}

BOOL CFolderDialog::OnInitialized()
{
	if (m_hWnd==NULL)
		return TRUE;
	if (m_szwDefaultFolder!=NULL)
	{
		if (IsUnicodeSystem())
			SendMessageW(m_hWnd,BFFM_SETSELECTIONW,TRUE,(LPARAM)m_szwDefaultFolder);
		else
			SendMessageW(m_hWnd,BFFM_SETSELECTION,TRUE,(LPARAM)m_szDefaultFolder);
	}
	else if (m_lpDefaultIL!=NULL)
		SendMessage(m_hWnd,BFFM_SETSELECTION,FALSE,(LPARAM)m_lpDefaultIL);
	
	return TRUE;
}

BOOL CFolderDialog::OnSelChanged(LPITEMIDLIST lpil)
{
	return TRUE;
}

BOOL CFolderDialog::OnValidateFailed(LPITEMIDLIST lpil)
{
	return TRUE;
}


BOOL CFolderDialog::GetFolder(CStringW& Folder) const
{
	if (IsUnicodeSystem())
	{
		if (!SHGetPathFromIDListW(m_lpil,Folder.GetBuffer(_MAX_PATH)))
			return FALSE;
		Folder.FreeExtra();
		return TRUE;
	}
	else
	{
		char szFolderA[MAX_PATH];
		if (!SHGetPathFromIDListA(m_lpil,szFolderA))
			return FALSE;
		Folder=szFolderA;
		return TRUE;
	}
	
}

BOOL CFolderDialog::GetFolder(LPWSTR szFolder) const
{
	if (IsUnicodeSystem())
		return SHGetPathFromIDListW(m_lpil,szFolder);
	else
	{
		char szFolderA[MAX_PATH];
		if (!SHGetPathFromIDListA(m_lpil,szFolderA))
			return FALSE;
		MultiByteToWideChar(CP_ACP,0,szFolderA,-1,szFolder,MAX_PATH);
		return TRUE;
	}
}

///////////////////////////
// Class COptionsPropertyPage
///////////////////////////

#define IDC_EDITCONTROLFORSELECTEDITEM  64300
#define IDC_SPINCONTROLFORSELECTEDITEM  64301
#define IDC_COMBOCONTROLFORSELECTEDITEM 64302
#define IDC_COLORBUTTONFORSELECTEDITEM	64303
#define IDC_FONTBUTTONFORSELECTEDITEM	64304
#define IDC_BROWSEBUTTONFORSELECTEDITEM	64305

COptionsPropertyPage::Item::Item(
	ItemType nType_,Item* pParent_,Item** pChilds_,LPWSTR pString_,
	CALLBACKPROC pProc_,DWORD wParam_,void* lParam_,LPCSTR pHelpId_)
:	nType(nType_),pParent(pParent_),pData(NULL),bEnabled(TRUE),
	wParam(wParam_),lParam(lParam_),pProc(pProc_),
	m_nStateIcon(-1),hControl(NULL),hControl2(NULL)
{
	pString=pString_!=NULL?alloccopy(pString_):NULL;
	pHelpID=pHelpId_!=NULL?alloccopy(pHelpId_):NULL;
	
	if (pChilds_!=NULL)
	{
		int i;
		for (i=0;pChilds_[i]!=NULL;i++);
		if (i>0)
		{
			pChilds=new Item*[i+1];
			CopyMemory(pChilds,pChilds_,sizeof(Item*)*(i+1));
			return;
		}
	}
	pChilds=NULL;
}

COptionsPropertyPage::Item::Item(
	ItemType nType_,Item* pParent_,Item** pChilds_,UINT nStringID,
	CALLBACKPROC pProc_,DWORD wParam_,void* lParam_,LPCSTR pHelpId_)
:	nType(nType_),pParent(pParent_),pData(NULL),bEnabled(TRUE),
	wParam(wParam_),lParam(lParam_),pProc(pProc_),
	m_nStateIcon(-1),hControl(NULL),hControl2(NULL)
{
	int nCurLen=50;
	int iLength;
	
	pHelpID=pHelpId_!=NULL?alloccopy(pHelpId_):NULL;
	
	if (!IsUnicodeSystem())
	{
		// Non-unicode
		char* szText=new char[nCurLen];
		while ((iLength=::LoadString(GetResourceHandle(LanguageSpecificResource),nStringID,szText,nCurLen)+1)>=nCurLen)
		{
			delete[] szText;
			nCurLen+=50;
			szText=new char[nCurLen];
		}
		pString=alloccopyAtoW(szText,iLength);
		delete[] szText;
	}
	else
	{
		// Unicode
		WCHAR* szText=new WCHAR[nCurLen];
		while ((iLength=::LoadStringW(GetResourceHandle(LanguageSpecificResource),nStringID,szText,nCurLen)+1)>=nCurLen)
		{
			delete[] szText;
			nCurLen+=50;
			szText=new WCHAR[nCurLen];
		}
		pString=new WCHAR[iLength];
		MemCopyW(pString,szText,iLength);
		delete[] szText;
	}

	if (pChilds_!=NULL)
	{
		int i;
		for (i=0;pChilds_[i]!=NULL;i++);
		if (i>0)
		{
			pChilds=new Item*[i+1];
			CopyMemory(pChilds,pChilds_,sizeof(Item*)*(i+1));
			return;
		}
	}
	pChilds=NULL;

	

}


COptionsPropertyPage::Item::~Item()
{
	if (pChilds!=NULL)
	{
		for (int i=0;pChilds[i]!=NULL;i++)
			delete pChilds[i];
		delete[] pChilds;
	}
	if (pString!=NULL)
		delete[] pString;
	if (pHelpID!=NULL)
		delete[] pHelpID;

	switch (nType)
	{
	case Combo:
	case Edit:
	case File:
		if (pData!=NULL)
			delete[] pData;
		break;
	case Font:
		if (pLogFont!=NULL)
			delete pLogFont;
		break;
	}
}

void COptionsPropertyPage::Construct(const OPTIONPAGE* pOptionPage,TypeOfResourceHandle bType)
{
	if (pOptionPage->dwFlags&OPTIONPAGE::opTemplateIsID)
		m_lpszTemplateName=MAKEINTRESOURCE(pOptionPage->nIDTemplate);
	else
	{
		if (IsUnicodeSystem())
			m_lpszTemplateNameW=alloccopy(pOptionPage->lpszTemplateName);
		else
			m_lpszTemplateName=alloccopyWtoA(pOptionPage->lpszTemplateName);
	}


	if (pOptionPage->dwFlags&OPTIONPAGE::opCaptionIsID)
		CPropertyPage::Construct(pOptionPage->nIDCaption,bType);
	else
		CPropertyPage::Construct(pOptionPage->lpszCaption,bType);

	m_nTreeID=pOptionPage->nTreeCtrlID;


	if (pOptionPage->dwFlags&OPTIONPAGE::opChangeIsID)
		m_ChangeText.LoadString(pOptionPage->nIDChangeText);
	else
		m_ChangeText=pOptionPage->lpszChangeText;


}

BOOL COptionsPropertyPage::Initialize(COptionsPropertyPage::Item** pItems)
{
	if (m_pTree==NULL)
	{
		m_pTree=new CTreeCtrl(GetDlgItem(IDC_SETTINGS));
		m_Images.Create(IDB_OPTIONSPROPERTYPAGEBITMAPS,16,0,RGB(255,255,255),LR_SHARED|LR_CREATEDIBSECTION,FALSE);
		m_pTree->SetImageList(m_Images,TVSIL_STATE);
		if (IsUnicodeSystem())
			m_pTree->SetUnicodeFormat(TRUE);

		// Subclassing tree control
		UserData* pUserData=new UserData;
		pUserData->pDialog=this;
		pUserData->pOldWndProc=(WNDPROC)m_pTree->SetWindowLong(gwlWndProc,(LONG_PTR)TreeSubClassFunc);
		

		if (pUserData->pOldWndProc==NULL)
		{
			// Subclassing didn't success
			delete pUserData;
		}
		else
			m_pTree->SetWindowLong(gwlUserData,(LONG_PTR)pUserData);

		
		// Replace images in the image list with themed/frame control images
		HANDLE hTheme=NULL;
		HMODULE hUxTheme=GetModuleHandle("uxtheme.dll");

		HRESULT(STDAPICALLTYPE *pDrawThemeBackground)(HANDLE,HDC,int,int,const RECT*,const RECT*)=NULL;
		HANDLE(STDAPICALLTYPE * pOpenThemeData)(HWND,LPCWSTR)=NULL;
		HRESULT(STDAPICALLTYPE * pCloseThemeData)(HANDLE)=NULL;

			
		if (hUxTheme!=NULL)
		{
			pDrawThemeBackground=(HRESULT(STDAPICALLTYPE *)(HANDLE,HDC,int,int,const RECT*,const RECT*))GetProcAddress(hUxTheme,"DrawThemeBackground");
			pOpenThemeData=(HANDLE(STDAPICALLTYPE*)(HWND,LPCWSTR))GetProcAddress(hUxTheme,"OpenThemeData");
			pCloseThemeData=(HRESULT(STDAPICALLTYPE *)(HANDLE))GetProcAddress(GetModuleHandle("uxtheme.dll"),"CloseThemeData");
			if (pOpenThemeData!=NULL && pDrawThemeBackground!=NULL)
				hTheme=pOpenThemeData(*this,L"BUTTON");
		}


		CBitmap Bitmap,Mask;
		BYTE* pBitmap,*pMask;
		BITMAPINFO  dibInfo;
		CDC dc;
		CBrush white(RGB(255,255,255));
		CRect rc(0,0,16,16);
		
		dibInfo.bmiHeader.biBitCount = 32;
		dibInfo.bmiHeader.biClrImportant = 0;
		dibInfo.bmiHeader.biClrUsed = 0;
		dibInfo.bmiHeader.biCompression = 0;
		dibInfo.bmiHeader.biHeight = 16;
		dibInfo.bmiHeader.biPlanes = 1;
		dibInfo.bmiHeader.biSize = 40;
		dibInfo.bmiHeader.biWidth = ( ( 16 + 3 ) / 4 ) * 4;
		dibInfo.bmiHeader.biSizeImage = 16*16*4;
		dibInfo.bmiHeader.biXPelsPerMeter = 3780;
		dibInfo.bmiHeader.biYPelsPerMeter = 3780;
		dibInfo.bmiColors[0].rgbBlue = 0;
		dibInfo.bmiColors[0].rgbGreen = 0;
		dibInfo.bmiColors[0].rgbRed = 0;
		dibInfo.bmiColors[0].rgbReserved = 0;

		dc.GetDC(NULL);
		Bitmap.CreateDIBSection(dc,(const BITMAPINFO*)&dibInfo,DIB_RGB_COLORS,(void**)&pBitmap,NULL,0);
		Mask.CreateDIBSection(dc,(const BITMAPINFO*)&dibInfo,DIB_RGB_COLORS,(void**)&pMask,NULL,0);
		dc.ReleaseDC();

		dc.CreateCompatibleDC(NULL);

		// Draw first bitmap
		HBITMAP hOldBitmap = (HBITMAP)dc.SelectObject(Bitmap);
		dc.FillRect(&rc,white);
		if (hTheme!=NULL)
			pDrawThemeBackground(hTheme,dc,BP_CHECKBOX,CBS_UNCHECKEDNORMAL,&rc,NULL);
		else
			DrawFrameControl(dc,&rc,DFC_BUTTON,DFCS_BUTTONCHECK);
		dc.SelectObject(hOldBitmap);
		// Fill out mask
		for (int i=0;i<16*16;i++)
			((DWORD*)pMask)[i]=((DWORD*)pBitmap)[i]&0x00FFFFFF?0xFFFFFFFF:0;
		m_Images.Replace(1,Bitmap,Mask);
		
		
		hOldBitmap = (HBITMAP)dc.SelectObject(Bitmap);
		dc.FillRect(&rc,white);
		if (hTheme!=NULL)
			pDrawThemeBackground(hTheme,dc,BP_CHECKBOX,CBS_CHECKEDNORMAL,&rc,NULL);
		else
			DrawFrameControl(dc,&rc,DFC_BUTTON,DFCS_BUTTONCHECK|DFCS_CHECKED);
		dc.SelectObject(hOldBitmap);
		for (int i=0;i<16*16;i++)
			((DWORD*)pMask)[i]=((DWORD*)pBitmap)[i]&0x00FFFFFF?0xFFFFFFFF:0;
		m_Images.Replace(2,Bitmap,Mask);
		
		hOldBitmap = (HBITMAP)dc.SelectObject(Bitmap);
		dc.FillRect(&rc,white);
		if (hTheme)
			pDrawThemeBackground(hTheme,dc,BP_RADIOBUTTON,RBS_UNCHECKEDNORMAL,&rc,NULL);
		else
			DrawFrameControl(dc,&rc,DFC_BUTTON,DFCS_BUTTONRADIOIMAGE);
		dc.SelectObject(hOldBitmap);
		for (int i=0;i<16*16;i++)
			((DWORD*)pMask)[i]=((DWORD*)pBitmap)[i]&0x00FFFFFF?0xFFFFFFFF:0;
		m_Images.Replace(3,Bitmap,Mask);

		hOldBitmap = (HBITMAP)dc.SelectObject(Bitmap);
		dc.FillRect(&rc,white);
		if (hTheme!=NULL)
			pDrawThemeBackground(hTheme,dc,BP_RADIOBUTTON,RBS_CHECKEDNORMAL,&rc,NULL);
		else
			DrawFrameControl(dc,&rc,DFC_BUTTON,DFCS_BUTTONRADIOIMAGE|DFCS_CHECKED);
		dc.SelectObject(hOldBitmap);
		for (int i=0;i<16*16;i++)
			((DWORD*)pMask)[i]=((DWORD*)pBitmap)[i]&0x00FFFFFF?0xFFFFFFFF:0;
		m_Images.Replace(4,Bitmap,Mask);
		
		if (pCloseThemeData!=NULL && hTheme!=NULL)
			pCloseThemeData(hTheme);


		HMODULE hModule=::LoadLibrary("imageres.dll");
		if (hModule!=NULL) 
		{
			HICON hIcon=(HICON)::LoadImage(hModule,MAKEINTRESOURCE(3),IMAGE_ICON,16,16,LR_DEFAULTCOLOR);
			if (hIcon!=NULL) 
			{	
				hOldBitmap = (HBITMAP)dc.SelectObject(Bitmap);
				dc.FillRect(&rc,white);
				dc.DrawIcon(0,0,hIcon,16,16,0,NULL,DI_NORMAL);
				dc.SelectObject(hOldBitmap);
				for (int i=0;i<16*16;i++)
					((DWORD*)pMask)[i]=((DWORD*)pBitmap)[i]&0x00FFFFFF?0xFFFFFFFF:0;
				m_Images.Replace(5,Bitmap,Mask);
				
				DestroyIcon(hIcon);
			}
		}	
		
	}

	if (pItems==NULL)
		return FALSE;
	
	// Counting items
	int iItems;
	for (iItems=0;pItems[iItems]!=NULL;iItems++);
	
	m_pItems=new Item*[max(iItems+1,2)];
	m_pItems[iItems]=NULL;
	CopyMemory(m_pItems,pItems,sizeof(Item*)*(iItems+1));
	return InsertItemsToTree(NULL,m_pItems,NULL);
	
}

BOOL COptionsPropertyPage::SetBitmaps(LPCTSTR lpBitmap,int cx,COLORREF crMask)
{
	m_Images.DeleteImageList();
	if (!m_Images.Create(lpBitmap,cx,0,crMask,LR_SHARED|LR_CREATEDIBSECTION,FALSE))
		return FALSE;
	m_pTree->SetImageList(m_Images,TVSIL_STATE);
	return TRUE;
}

BOOL COptionsPropertyPage::SetBitmaps(int iBitmap,int cx,COLORREF crMask)
{
	m_Images.DeleteImageList();
	if (!m_Images.Create(iBitmap,cx,0,crMask,LR_SHARED|LR_CREATEDIBSECTION,FALSE))
		return FALSE;
	m_pTree->SetImageList(m_Images,TVSIL_STATE);
	return TRUE;
}

BOOL COptionsPropertyPage::SetBitmaps(HIMAGELIST hImagelist,BOOL bTakeList)
{
	m_Images.DeleteImageList();
	if (bTakeList)
		m_Images.Attach(hImagelist);

	m_pTree->SetImageList(m_Images,TVSIL_STATE);
	return TRUE;
}

BOOL COptionsPropertyPage::InsertItemsToTree(HTREEITEM hParent,COptionsPropertyPage::Item** pItems,COptionsPropertyPage::Item* pParent)
{
	INITIALIZEPARAMS bp;
	bp.pPage=this;

	

	union {
		TVINSERTSTRUCTA tisa;
		TVINSERTSTRUCTW tisw;
	};
	
	if (!IsUnicodeSystem())
	{
		// Windows 9x
		tisa.hInsertAfter=TVI_LAST;
		tisa.hParent=hParent;
		tisa.itemex.stateMask=TVIS_STATEIMAGEMASK|TVIS_EXPANDED;
		tisa.itemex.mask=TVIF_STATE|TVIF_CHILDREN|TVIF_TEXT|TVIF_PARAM;
	}
	else
	{
		// Windows NT/2000/XP
		tisw.hInsertAfter=TVI_LAST;
		tisw.hParent=hParent;
		tisw.itemex.stateMask=TVIS_STATEIMAGEMASK|TVIS_EXPANDED;
		tisw.itemex.mask=TVIF_STATE|TVIF_CHILDREN|TVIF_TEXT|TVIF_PARAM;
	}

	HTREEITEM hSelectedRadioItem=NULL;
	
	int nItemHeight=18;
    for (int i=0;pItems[i]!=NULL;i++)
	{
		if (pItems[i]->pProc!=NULL)
		{
			bp.crReason=BASICPARAMS::Get;
			pItems[i]->SetValuesForBasicParams(&bp);
			if (pItems[i]->pProc(&bp))
				pItems[i]->GetValuesFromBasicParams(&bp);
		}
        			
		if (!IsUnicodeSystem())
		{
			tisa.itemex.pszText=alloccopyWtoA(pItems[i]->pString);
			tisa.itemex.cChildren=pItems[i]->pChilds==0?0:1;
			tisa.itemex.lParam=LPARAM(pItems[i]);
			tisa.itemex.state=TVIS_EXPANDED|INDEXTOSTATEIMAGEMASK(pItems[i]->GetStateImage(&m_Images));
			tisa.hInsertAfter=m_pTree->InsertItem(&tisa);
			delete[] tisa.itemex.pszText;
		}
		else
		{
			tisw.itemex.pszText=pItems[i]->pString;
			tisw.itemex.cChildren=pItems[i]->pChilds==0?0:1;
			tisw.itemex.lParam=LPARAM(pItems[i]);
			tisw.itemex.state=TVIS_EXPANDED|INDEXTOSTATEIMAGEMASK(pItems[i]->GetStateImage(&m_Images));
			tisw.hInsertAfter=m_pTree->InsertItem(&tisw);
		}


		if (pItems[i]->pChilds!=NULL)
			InsertItemsToTree(!IsUnicodeSystem()?tisa.hInsertAfter:tisw.hInsertAfter,pItems[i]->pChilds,pItems[i]);

		// Type specified actions
		switch (pItems[i]->nType)
		{
		case Item::RadioBox:
			if (pItems[i]->bChecked)
				hSelectedRadioItem=!IsUnicodeSystem()?tisa.hInsertAfter:tisw.hInsertAfter;
			// Continuing
		case Item::CheckBox:
			EnableChilds(!IsUnicodeSystem()?tisa.hInsertAfter:tisw.hInsertAfter,pItems[i]->bChecked);
			break;
		case Item::Edit:
		case Item::File:
			{
				if (IsUnicodeSystem())
				{
					pItems[i]->hControl=CreateWindowW(L"EDIT",L"",
						ES_AUTOHSCROLL|WS_CHILDWINDOW|WS_BORDER,
						10,10,100,13,*this,(HMENU)IDC_EDITCONTROLFORSELECTEDITEM,GetInstanceHandle(),NULL);
				}
				else
				{
					pItems[i]->hControl=CreateWindow("EDIT","",
						ES_AUTOHSCROLL|WS_CHILDWINDOW|WS_BORDER,
						10,10,100,13,*this,(HMENU)IDC_EDITCONTROLFORSELECTEDITEM,GetInstanceHandle(),NULL);
				}
				::SendMessage(pItems[i]->hControl,WM_SETFONT,SendMessage(WM_GETFONT),TRUE);

				// Subclassing control
				UserData* pUserData=new UserData;
				pUserData->pDialog=this;
				if (IsUnicodeSystem())
				{
					pUserData->pOldWndProc=(WNDPROC)::SetWindowLongPtrW(pItems[i]->hControl,
						GWLP_WNDPROC,(LONG_PTR)EditSubClassFunc);
				}
				else
				{
					pUserData->pOldWndProc=(WNDPROC)::SetWindowLongPtr(pItems[i]->hControl,
						GWLP_WNDPROC,(LONG_PTR)EditSubClassFunc);
				}

				if (pUserData->pOldWndProc==NULL)
				{
					// Subclassing didn't success
					delete pUserData;
				}
				else
					::SetWindowLongPtr(pItems[i]->hControl,GWLP_USERDATA,(LONG_PTR)pUserData);

				
				// Initializing
				if (pItems[i]->pProc!=NULL)
				{
					bp.crReason=BASICPARAMS::Initialize;
					bp.hControl=pItems[i]->hControl;
					pItems[i]->pProc(&bp);
				}
				if (pItems[i]->pData!=NULL)
				{
					if (IsUnicodeSystem())
						::SendMessageW(pItems[i]->hControl,WM_SETTEXT,0,LPARAM(pItems[i]->pData));
					else
						::SendMessage(pItems[i]->hControl,WM_SETTEXT,0,LPARAM((LPCSTR)W2A(pItems[i]->pData)));
				}
				break;
			}
		case Item::Numeric:
			{
				if (IsUnicodeSystem())
				{
					pItems[i]->hControl=CreateWindowW(L"EDIT",L"",
						ES_AUTOHSCROLL|WS_CHILDWINDOW|WS_BORDER|ES_NUMBER,
						10,10,50,20,*this,(HMENU)IDC_EDITCONTROLFORSELECTEDITEM,GetInstanceHandle(),NULL);
				}
				else
				{
					pItems[i]->hControl=CreateWindow("EDIT","",
						ES_AUTOHSCROLL|WS_CHILDWINDOW|WS_BORDER|ES_NUMBER,
						10,10,50,20,*this,(HMENU)IDC_EDITCONTROLFORSELECTEDITEM,GetInstanceHandle(),NULL);
				}
				::SendMessage(pItems[i]->hControl,WM_SETFONT,SendMessage(WM_GETFONT),TRUE);

				// Setting subclass info struct
				UserData* pUserData=new UserData;
				pUserData->pDialog=this;
				pUserData->pOldWndProc=NULL;
				::SetWindowLongPtr(pItems[i]->hControl,GWLP_USERDATA,(LONG_PTR)pUserData);
	
				// Initializing
				if (pItems[i]->pProc!=NULL)
				{
					bp.crReason=BASICPARAMS::Initialize;
					bp.hControl=pItems[i]->hControl;
					pItems[i]->pProc(&bp);
				}
				if (IsUnicodeSystem())
				{
					WCHAR szText[100];
					_itow_s(pItems[i]->lValue,szText,100,10);
					::SendMessageW(pItems[i]->hControl,WM_SETTEXT,0,LPARAM(szText));
				}
				else
				{
					char szText[100];
					_itoa_s(pItems[i]->lValue,szText,100,10);
					::SendMessage(pItems[i]->hControl,WM_SETTEXT,0,LPARAM(szText));
				}
			}
			break;
		case Item::List:
			{
				if (IsUnicodeSystem())
				{
					pItems[i]->hControl=CreateWindowW(L"COMBOBOX",L"",
						CBS_DROPDOWNLIST|WS_VSCROLL|WS_CHILDWINDOW|WS_BORDER,
						10,10,100,100,*this,(HMENU)IDC_COMBOCONTROLFORSELECTEDITEM,GetInstanceHandle(),NULL);
				}
				else
				{
					pItems[i]->hControl=CreateWindow("COMBOBOX","",
						CBS_DROPDOWNLIST|WS_VSCROLL|WS_CHILDWINDOW|WS_BORDER,
						10,10,100,100,*this,(HMENU)IDC_COMBOCONTROLFORSELECTEDITEM,GetInstanceHandle(),NULL);
				}
				::SendMessage(pItems[i]->hControl,WM_SETFONT,SendMessage(WM_GETFONT),TRUE);
				
				// Subclassing control
				UserData* pUserData=new UserData;
				pUserData->pDialog=this;
				if (IsUnicodeSystem())
				{
					pUserData->pOldWndProc=(WNDPROC)::SetWindowLongPtrW(pItems[i]->hControl,
						GWLP_WNDPROC,(LONG_PTR)ComboSubClassFunc);
				}
				else
				{
					pUserData->pOldWndProc=(WNDPROC)::SetWindowLongPtr(pItems[i]->hControl,
						GWLP_WNDPROC,(LONG_PTR)ComboSubClassFunc);
				}

				if (pUserData->pOldWndProc==NULL)
				{
					// Subclassing didn't success
					delete pUserData;
				}
				else
					::SetWindowLongPtr(pItems[i]->hControl,GWLP_USERDATA,(LONG_PTR)pUserData);

				// Initializing
				if (pItems[i]->pProc!=NULL)
				{
					bp.crReason=BASICPARAMS::Initialize;
					bp.hControl=pItems[i]->hControl;
					pItems[i]->pProc(&bp);
				}
				::SendMessage(pItems[i]->hControl,CB_SETCURSEL,pItems[i]->lValue,NULL);
				break;
			}			
		case Item::Combo:
			{
				if (IsUnicodeSystem())
				{
					pItems[i]->hControl=CreateWindowW(L"COMBOBOX",L"",
						CBS_DROPDOWN|WS_VSCROLL|WS_CHILDWINDOW|WS_BORDER,
						10,10,100,100,*this,(HMENU)IDC_COMBOCONTROLFORSELECTEDITEM,GetInstanceHandle(),NULL);
				}
				else
				{
					pItems[i]->hControl=CreateWindow("COMBOBOX","",
						CBS_DROPDOWN|WS_VSCROLL|WS_CHILDWINDOW|WS_BORDER,
						10,10,100,100,*this,(HMENU)IDC_COMBOCONTROLFORSELECTEDITEM,GetInstanceHandle(),NULL);
				}
				::SendMessage(pItems[i]->hControl,WM_SETFONT,SendMessage(WM_GETFONT),TRUE);

				// Subclassing control
				UserData* pUserData=new UserData;
				pUserData->pDialog=this;
				if (IsUnicodeSystem())
				{
					pUserData->pOldWndProc=(WNDPROC)::SetWindowLongPtrW(pItems[i]->hControl,
						GWLP_WNDPROC,(LONG_PTR)ComboSubClassFunc);
				}
				else
				{
					pUserData->pOldWndProc=(WNDPROC)::SetWindowLongPtr(pItems[i]->hControl,
						GWLP_WNDPROC,(LONG_PTR)ComboSubClassFunc);
				}


				if (pUserData->pOldWndProc==NULL)
				{
					// Subclassing didn't success
					delete pUserData;
				}
				else
					::SetWindowLongPtr(pItems[i]->hControl,GWLP_USERDATA,(LONG_PTR)pUserData);

				HWND hEdit=GetWindow(pItems[i]->hControl,GW_CHILD);
				if (hEdit!=NULL)
				{
					UserData* pUserData=new UserData;
					pUserData->pDialog=this;
					if (IsUnicodeSystem())
					{
						pUserData->pOldWndProc=(WNDPROC)::SetWindowLongPtrW(hEdit,
							GWLP_WNDPROC,(LONG_PTR)ComboSubClassFunc);
					}
					else
					{
						pUserData->pOldWndProc=(WNDPROC)::SetWindowLongPtr(hEdit,
							GWLP_WNDPROC,(LONG_PTR)ComboSubClassFunc);
					}
						

					if (pUserData->pOldWndProc==NULL)
					{
						// Subclassing didn't success
						delete pUserData;
					}
					else
						::SetWindowLongPtr(hEdit,GWLP_USERDATA,(LONG_PTR)pUserData);
				}

				// Initializing
				if (pItems[i]->pProc!=NULL)
				{
					bp.crReason=BASICPARAMS::Initialize;
					bp.hControl=pItems[i]->hControl;
					pItems[i]->pProc(&bp);
				}

				
				if (pItems[i]->pData!=NULL)
				{
					// Checking whether value is found in combo
					int nFind=(int)::SendMessage(pItems[i]->hControl,CB_FINDSTRINGEXACT,0,LPARAM(pItems[i]->pData));
					::SendMessage(pItems[i]->hControl,CB_SETCURSEL,nFind,0);
					if (nFind==CB_ERR)
					{
						if (IsUnicodeSystem())
							::SendMessageW(pItems[i]->hControl,WM_SETTEXT,0,LPARAM(pItems[i]->pData));
						else
							::SendMessage(pItems[i]->hControl,WM_SETTEXT,0,LPARAM((LPCSTR)W2A(pItems[i]->pData)));
					}
				}
				break;
			}
		case Item::Font:
			{
				if (IsUnicodeSystem())
				{
					pItems[i]->hControl=CreateWindowW(L"BUTTON",m_ChangeText,BS_PUSHBUTTON|WS_CHILDWINDOW,
						10,10,100,13,*this,(HMENU)IDC_FONTBUTTONFORSELECTEDITEM,GetInstanceHandle(),NULL);
				}
				else
				{
					pItems[i]->hControl=CreateWindow("BUTTON",W2A(m_ChangeText),BS_PUSHBUTTON|WS_CHILDWINDOW,
						10,10,100,13,*this,(HMENU)IDC_FONTBUTTONFORSELECTEDITEM,GetInstanceHandle(),NULL);
				}
				::SendMessage(pItems[i]->hControl,WM_SETFONT,SendMessage(WM_GETFONT),TRUE);

				// Subclassing control
				UserData* pUserData=new UserData;
				pUserData->pDialog=this;
				if (IsUnicodeSystem())
				{
					pUserData->pOldWndProc=(WNDPROC)::SetWindowLongPtrW(pItems[i]->hControl,
						GWLP_WNDPROC,(LONG_PTR)ButtonSubClassFunc);
				}
				else
				{
					pUserData->pOldWndProc=(WNDPROC)::SetWindowLongPtr(pItems[i]->hControl,
						GWLP_WNDPROC,(LONG_PTR)ButtonSubClassFunc);
				}

				if (pUserData->pOldWndProc==NULL)
				{
					// Subclassing didn't success
					delete pUserData;
				}
				else
					::SetWindowLongPtr(pItems[i]->hControl,GWLP_USERDATA,(LONG_PTR)pUserData);

				// Initializing
				if (pItems[i]->pProc!=NULL)
				{
					bp.crReason=BASICPARAMS::Initialize;
					bp.hControl=pItems[i]->hControl;
					pItems[i]->pProc(&bp);
				}
				break;
			}		
		case Item::Color:
			{
				if (IsUnicodeSystem())
				{
					pItems[i]->hControl=CreateWindowW(L"BUTTON",m_ChangeText,BS_PUSHBUTTON|WS_CHILDWINDOW,
						10,10,100,13,*this,(HMENU)IDC_COLORBUTTONFORSELECTEDITEM,GetInstanceHandle(),NULL);
				}
				else
				{
					pItems[i]->hControl=CreateWindow("BUTTON",W2A(m_ChangeText),BS_PUSHBUTTON|WS_CHILDWINDOW,
						10,10,100,13,*this,(HMENU)IDC_COLORBUTTONFORSELECTEDITEM,GetInstanceHandle(),NULL);
				}
				::SendMessage(pItems[i]->hControl,WM_SETFONT,SendMessage(WM_GETFONT),TRUE);

				// Subclassing control
				UserData* pUserData=new UserData;
				pUserData->pDialog=this;
				if (IsUnicodeSystem())
				{
					pUserData->pOldWndProc=(WNDPROC)::SetWindowLongPtrW(pItems[i]->hControl,
						GWLP_WNDPROC,(LONG_PTR)ButtonSubClassFunc);
				}
				else
				{
					pUserData->pOldWndProc=(WNDPROC)::SetWindowLongPtr(pItems[i]->hControl,
						GWLP_WNDPROC,(LONG_PTR)ButtonSubClassFunc);
				}

				if (pUserData->pOldWndProc==NULL)
				{
					// Subclassing didn't success
					delete pUserData;
				}
				else
					::SetWindowLongPtr(pItems[i]->hControl,GWLP_USERDATA,(LONG_PTR)pUserData);

				// Initializing
				if (pItems[i]->pProc!=NULL)
				{
					bp.crReason=BASICPARAMS::Initialize;
					bp.hControl=pItems[i]->hControl;
					pItems[i]->pProc(&bp);
				}
				break;
			}			
		}
		
		// Setting text
		LPWSTR pCurText=pItems[i]->GetText();
		if (pCurText!=pItems[i]->pString)
		{
			if (!IsUnicodeSystem())
				m_pTree->SetItemText(tisa.hInsertAfter,W2A(pCurText));
			else
				m_pTree->SetItemText(tisw.hInsertAfter,pCurText);
			
		}
		pItems[i]->FreeText(pCurText);


		
		if (pItems[i]->hControl!=NULL)
		{
			CRect rc;
			::GetWindowRect(pItems[i]->hControl,&rc);
			if (rc.Height()-2>nItemHeight)
				nItemHeight=rc.Height()-2;
		}
	}

	// Ensuring that one radio is at least selected
	if (hSelectedRadioItem==NULL)
	{
		HTREEITEM hItem;
		if (hParent==NULL)
			hItem=m_pTree->GetNextItem(NULL,TVGN_ROOT);
		else
			hItem=m_pTree->GetNextItem(hParent,TVGN_CHILD);

		while (hItem!=NULL)
		{
			Item* pItem=(Item*)m_pTree->GetItemData(hItem);
			if (pItem!=NULL)
			{
				if (pItem->nType==Item::RadioBox)
				{
					hSelectedRadioItem=hItem;
					SetCheckState(hItem,pItem,Checked);					
					break;
				}
			}            			
			hItem=m_pTree->GetNextItem(hItem,TVGN_NEXT);
		}
	}
	else
		UncheckOtherRadioButtons(hSelectedRadioItem,hParent);

	if (nItemHeight>m_pTree->GetItemHeight())
		m_pTree->SetItemHeight(nItemHeight);
	return TRUE;
}

BOOL COptionsPropertyPage::OnCommand(WORD wID,WORD wNotifyCode,HWND hControl)
{
	CPropertyPage::OnCommand(wID,wNotifyCode,hControl);

	switch (wID)
	{
	case IDC_EDITCONTROLFORSELECTEDITEM:
		switch (wNotifyCode)
		{
		case EN_CHANGE:
			{
				HTREEITEM hItem=m_pTree->GetNextItem(NULL,TVGN_CARET);
				if (hItem==NULL)
					break;
				
				Item* pItem=(Item*)m_pTree->GetItemData(hItem);
				if (pItem==NULL)
					break;
				if (pItem->hControl!=hControl)
					break;
				
				if (pItem->nType==Item::Numeric)
					SetNumericValue(pItem);
				else if (pItem->nType==Item::Edit || pItem->nType==Item::File)
					SetTextValue(pItem);
				
				break;
			}
		case EN_SETFOCUS:
			::SendMessage(hControl,EM_SETSEL,0,MAKELPARAM(0,-1));
			break;
		}
		break;
	case IDC_SPINCONTROLFORSELECTEDITEM:
		break;
	case IDC_COMBOCONTROLFORSELECTEDITEM:
		switch (wNotifyCode)
		{
		case CBN_SELCHANGE:
			{
				HTREEITEM hItem=m_pTree->GetNextItem(NULL,TVGN_CARET);
				if (hItem==NULL)
					break;
				Item* pItem=(Item*)m_pTree->GetItemData(hItem);
				if (pItem==NULL)
					break;
				if (pItem->hControl!=hControl)
					break;
				if (pItem->nType==Item::Combo)
					SetTextValue(pItem);
				else if (pItem->nType==Item::List)
					SetListValue(pItem);
				break;
			}
		case CBN_EDITCHANGE:
			{
				HTREEITEM hItem=m_pTree->GetNextItem(NULL,TVGN_CARET);
				if (hItem==NULL)
					break;
				Item* pItem=(Item*)m_pTree->GetItemData(hItem);
				if (pItem==NULL)
					break;
				if (pItem->hControl!=hControl)
					break;

				if (pItem->nType==Item::Combo)
					SetTextValue(pItem);
				break;
			}
		case CBN_SETFOCUS:
			::SendMessage(hControl,CB_SETEDITSEL,0,MAKELPARAM(0,-1));
			break;
		default:
			CAppData::stdfunc();
			break;
		}
		break;
	case IDC_COLORBUTTONFORSELECTEDITEM:
		{
			HTREEITEM hItem=m_pTree->GetNextItem(NULL,TVGN_CARET);
			if (hItem==NULL)
				break;
			Item* pItem=(Item*)m_pTree->GetItemData(hItem);
			if (pItem==NULL)
				break;
			if (pItem->hControl!=hControl)
				break;

			if (pItem->nType==Item::Color)
			{
				CColorDialog cd(pItem->cColor);
				if (cd.DoModal(*this))
				{
					SetColorValue(pItem,cd.GetColor());

					m_pTree->RedrawWindow();
				}
				break;
			}
			break;
		}
	case IDC_FONTBUTTONFORSELECTEDITEM:
		{
			HTREEITEM hItem=m_pTree->GetNextItem(NULL,TVGN_CARET);
			if (hItem==NULL)
				break;
			Item* pItem=(Item*)m_pTree->GetItemData(hItem);
			if (pItem==NULL)
				break;
			if (pItem->hControl!=hControl)
				break;

			if (pItem->nType==Item::Font)
			{
				CFontDialog fd(pItem->pLogFont,CF_SCREENFONTS);
                
				if (fd.DoModal(*this))
				{
					SetFontValue(pItem,&fd.m_lf);

					WCHAR* pText=pItem->GetText(TRUE);
					m_pTree->SetItemText(hItem,pText);
					pItem->FreeText(pText);
				}

				break;
			}
			break;
		}
	case IDC_BROWSEBUTTONFORSELECTEDITEM:
		{
			HTREEITEM hItem=m_pTree->GetNextItem(NULL,TVGN_CARET);
			if (hItem==NULL)
				break;
			Item* pItem=(Item*)m_pTree->GetItemData(hItem);
			if (pItem==NULL)
				break;
			
			if (pItem->nType==Item::File)
			{
				BROWSEDLGPARAMS bp;
				pItem->SetValuesForBasicParams(&bp);
				bp.crReason=BASICPARAMS::BrowseFile;
				bp.pPage=this;
				bp.szTitle=NULL;
				bp.szFilters=NULL;
				pItem->pProc(&bp);
				
				CFileDialog* pfd;
				if (IS_INTRESOURCE(bp.szFilters))
				{
					pfd=new CFileDialog(TRUE,L"*",szwEmpty,OFN_EXPLORER|OFN_HIDEREADONLY|
						OFN_NOREADONLYRETURN|OFN_ENABLESIZING,(UINT)(ULONG_PTR)(bp.szFilters));
				}
				else
				{
					pfd=new CFileDialog(TRUE,L"*",szwEmpty,OFN_EXPLORER|OFN_HIDEREADONLY|
						OFN_NOREADONLYRETURN|OFN_ENABLESIZING,bp.szFilters);
				}
				
				pfd->EnableFeatures();
				if (IS_INTRESOURCE(bp.szTitle))
					pfd->SetTitle(ID2W((UINT)(ULONG_PTR)bp.szTitle));
				else
					pfd->SetTitle(bp.szTitle);
	
				if (pfd->DoModal(*this))
				{
					WCHAR szPath[MAX_PATH];
					if (pfd->GetFilePath(szPath,MAX_PATH))
					{
						if (IsUnicodeSystem())
							::SendMessageW(pItem->hControl,WM_SETTEXT,0,(LPARAM)szPath);
						else
							::SendMessage(pItem->hControl,WM_SETTEXT,0,(LPARAM)(LPCSTR)W2A(szPath));
						::SendMessage(pItem->hControl,EM_SETSEL,0,-1);
						::SetFocus(pItem->hControl);
					}
				}


				delete pfd;
			}
			break;
		}
	}
	return FALSE;
}

BOOL COptionsPropertyPage::OnApply()
{
	CPropertyPage::OnApply();
	if (m_pItems!=NULL)
		CallApply(m_pItems);
	return TRUE;
}

void COptionsPropertyPage::CallApply(Item** pItems)
{
	COMBOAPPLYPARAMS bp;
	bp.pPage=this;
	bp.crReason=BASICPARAMS::Apply;

	for (int i=0;pItems[i]!=NULL;i++)
	{
		if (pItems[i]->bEnabled)
		{
			if (pItems[i]->pProc!=NULL)
			{
				pItems[i]->SetValuesForBasicParams(&bp);
				if (pItems[i]->nType==Item::Combo || pItems[i]->nType==Item::List)
					bp.nCurSel=(LONG)::SendMessage(pItems[i]->hControl,CB_GETCURSEL,0,0);
				pItems[i]->pProc(&bp);
			}
			if (pItems[i]->pChilds!=NULL)
				CallApply(pItems[i]->pChilds);
		}
	}
}

void COptionsPropertyPage::OnDestroy()
{
	CPropertyPage::OnDestroy();
	
	if (m_pTree!=NULL)
	{
		m_pTree->DeleteAllItems();
		delete m_pTree;
		m_pTree=NULL;
	}
	
	if (m_pItems!=NULL)
	{
		for (int i=0;m_pItems[i]!=NULL;i++)
			delete m_pItems[i];
		delete[] m_pItems;
	}

	
}

	
void COptionsPropertyPage::OnActivate(WORD fActive,BOOL fMinimized,HWND hwnd)
{
	CPropertyPage::OnActivate(fActive,fMinimized,hwnd);

	if (fActive!=WA_INACTIVE)
		PostMessage(WM_REDRAWSELITEMCONTROL);
}

/*void COptionsPropertyPage::OnTimer(DWORD wTimerID)
{
	switch (wTimerID)
	{
	case 0:
		KillTimer(0);
		PostMessage(WM_REDRAWSELITEMCONTROL);
		break;
	}
	CPropertyPage::OnTimer(wTimerID);
}*/

int COptionsPropertyPage::Item::IconFromColor(CImageList* pImageList,int nReplace) const
{
	int cx=16,cy=16;
	pImageList->GetIconSize(&cx,&cy);

	HDC hScreenDC=::GetDC(NULL);
	HDC memDC=::CreateCompatibleDC(hScreenDC);
    HBITMAP memBM=CreateCompatibleBitmap(hScreenDC,cx,cy);
    HBITMAP memBM2=CreateCompatibleBitmap(hScreenDC,cx,cy);
    
	// Creating first image
	SelectObject(memDC,memBM);
    HBRUSH hBrush=CreateSolidBrush(cColor);
	FillRect(memDC,&CRect(2,0,cx-2,cy-3),hBrush);
	DeleteObject(hBrush);
	
	// Crating second image
	SelectObject(memDC,memBM2);
    hBrush=CreateSolidBrush(RGB(255,255,255));
	FillRect(memDC,&CRect(0,0,cx,cy),hBrush);
	DeleteObject(hBrush);
	hBrush=CreateSolidBrush(RGB(0,0,0));
	FillRect(memDC,&CRect(2,0,cx-2,cy-3),hBrush);
	DeleteObject(hBrush);
	
	DeleteDC(memDC);
	
	
	int nImage=-1;
	
	if (nReplace==-1)
		nImage=pImageList->Add(memBM,memBM2);
	else
		nImage=pImageList->Replace(nReplace,memBM,memBM2)?nReplace:-1;
    
	DeleteObject(memBM);
	DeleteObject(memBM2);
	::ReleaseDC(NULL,hScreenDC);
	
	return nImage;
}
	
LRESULT COptionsPropertyPage::WindowProc(UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
	case WM_REDRAWSELITEMCONTROL:
		{
			HTREEITEM hActiveItem=m_pTree->GetNextItem(NULL,TVGN_CARET);
			if (hActiveItem==NULL)
				return TRUE;

			Item* pItem=(Item*)m_pTree->GetItemData(hActiveItem);
			if (pItem==NULL)
				return TRUE;

			if (pItem->hControl!=NULL)
			{
				// Checking that should position change
				CRect rcItem,rcOrig,rcTree,rcOther;
				m_pTree->GetClientRect(&rcTree);
				m_pTree->GetItemRect(hActiveItem,&rcItem,TRUE);
				BOOL bNotVisible=rcItem.top<0 || rcItem.bottom>rcTree.bottom;
				m_pTree->ClientToScreen(&rcItem);
				
				::GetWindowRect(pItem->hControl,&rcOrig);
				if (pItem->hControl2!=NULL)
				{
					::GetWindowRect(pItem->hControl2,&rcOther);
					rcOther.left-=rcOrig.left;
					rcOther.top-=rcOrig.top;
				}


				//if (rcOrig.left!=rcItem.right+1 || rcOrig.top!=rcItem.top-1)
				{
					// Moving control to correct place
					ScreenToClient(&rcItem);
					::SetWindowPos(pItem->hControl,HWND_TOP,rcItem.right+1,rcItem.top-1,0,0,
						(bNotVisible?SWP_HIDEWINDOW:SWP_SHOWWINDOW)|SWP_NOSIZE|SWP_NOACTIVATE);
					

					if (pItem->hControl2!=NULL)
					{
						if (pItem->nType==Item::Numeric)
						{
							::SetWindowPos(pItem->hControl2,HWND_TOP,rcItem.right+1+rcOther.left,rcItem.top-1+rcOther.top,0,0,
								(bNotVisible?SWP_HIDEWINDOW:SWP_SHOWWINDOW)|SWP_NOSIZE|SWP_NOACTIVATE);
							::InvalidateRect(pItem->hControl2,NULL,FALSE);
						}
						else if (pItem->nType==Item::File)
						{
							::SetWindowPos(pItem->hControl2,HWND_TOP,rcItem.right+1+rcOrig.Width(),rcItem.top-1,0,0,
								(bNotVisible?SWP_HIDEWINDOW:SWP_SHOWWINDOW)|SWP_NOSIZE|SWP_NOACTIVATE);
							::InvalidateRect(pItem->hControl2,NULL,FALSE);
						}
					}

				}

				::InvalidateRect(pItem->hControl,NULL,FALSE);
				if (pItem->hControl2!=NULL)
					::InvalidateRect(pItem->hControl2,NULL,FALSE);
			}
			break;
		}
	case WM_FOCUSSELITEMCONTROL:
		{
			HTREEITEM hActiveItem=m_pTree->GetNextItem(NULL,TVGN_CARET);
			if (hActiveItem==NULL)
				return TRUE;

			Item* pItem=(Item*)m_pTree->GetItemData(hActiveItem);
			if (pItem==NULL)
				return TRUE;
			
			if (pItem->hControl!=NULL)
			{
				::InvalidateRect(pItem->hControl,NULL,FALSE);
				::SetFocus(pItem->hControl);
			}

			break;
		}
	}
	return CPropertyPage::WindowProc(msg,wParam,lParam);
}

BOOL COptionsPropertyPage::OnNotify(int idCtrl,LPNMHDR pnmh)
{
	if (idCtrl==m_nTreeID)
	{
		CPropertyPage::OnNotify(idCtrl,pnmh);
		BOOL bRet=TreeNotifyHandler((NMTVDISPINFO*)pnmh);
		if (bRet)
			SetWindowLong(dwlMsgResult,bRet);
		return bRet;
	}			
	return CPropertyPage::OnNotify(idCtrl,pnmh);
}


BOOL COptionsPropertyPage::TreeNotifyHandler(NMTVDISPINFO *pTvdi)
{
	switch (pTvdi->hdr.code)
	{
	case TVN_KEYDOWN:
		if (((NMTVKEYDOWN*)pTvdi)->wVKey==VK_SPACE)
		{
			HTREEITEM hItem=m_pTree->GetNextItem(NULL,TVGN_CARET);
			if (hItem==NULL)
				break;
			Item* pItem=(Item*)m_pTree->GetItemData(hItem);
			if (pItem==NULL)
				break;
			if (pItem->bEnabled && (pItem->nType==Item::RadioBox || pItem->nType==Item::CheckBox))
				SetCheckState(hItem,pItem,Toggle);
			
		}
		break;
	case NM_CLICK:
	case NM_DBLCLK:
		{
			TVHITTESTINFO ht;
			GetCursorPos(&ht.pt);
			m_pTree->ScreenToClient(&ht.pt);
			HTREEITEM hItem=m_pTree->HitTest(&ht);
			if (hItem==NULL)
				break;
			Item* pItem=(Item*)m_pTree->GetItemData(hItem);
			if (pItem==NULL)
				break;
			if (pItem->nType==Item::RadioBox || pItem->nType==Item::CheckBox)
			{
				if (pItem->bEnabled && (ht.flags&TVHT_ONITEMSTATEICON || pTvdi->hdr.code==NM_DBLCLK))
					SetCheckState(hItem,pItem,Toggle);
			}
			break;
		}
	case TVN_SELCHANGINGA:
	case TVN_SELCHANGINGW:
		{
			NMTREEVIEWA *pNm=(NMTREEVIEWA*)pTvdi;
			// Procedure does not access to any LPTSTR elements of NMTREEVIEWW,
			// so no need for different unicode implementation

			// Checking if selection cannot be changed
			if (pNm->itemNew.lParam!=NULL)
			{
				if (!((Item*)pNm->itemNew.lParam)->bEnabled)
					return TRUE;
			}

			// Hiding control for previous item
			if (pNm->itemOld.lParam!=NULL)
			{
				Item* pItem=(Item*)pNm->itemOld.lParam;
				if (pItem->hControl!=NULL)
				{
					// Hiding window and ensuring that that part of tree is redrawn
					RECT rc;
					::GetWindowRect(pItem->hControl,&rc);
					::ShowWindow(pItem->hControl,SW_HIDE);
					m_pTree->ScreenToClient(&rc);
					::InvalidateRect(*m_pTree,&rc,FALSE);
					
					// Setting text
					WCHAR* pText=pItem->GetText(FALSE);
					if (!IsUnicodeSystem())
						m_pTree->SetItemText(pNm->itemOld.hItem,W2A(pText));
					else
						m_pTree->SetItemText(pNm->itemOld.hItem,pText);
					pItem->FreeText(pText);
					
					// Deleting another control
					if (pItem->hControl2!=NULL)
					{
						::DestroyWindow(pItem->hControl2);
						pItem->hControl2=NULL;
					}

					if (pItem->nType==Item::Numeric)
					{
						// Desubclassing
						UserData* pUserData=(UserData*)::GetWindowLongPtr(pItem->hControl,GWLP_USERDATA);
						if (pUserData!=NULL)
						{
							if (pUserData->pOldWndProc!=NULL)
							{
								if (IsUnicodeSystem())
									::SetWindowLongPtrW(pItem->hControl,GWLP_WNDPROC,(LONG_PTR)pUserData->pOldWndProc);
								else
									::SetWindowLongPtr(pItem->hControl,GWLP_WNDPROC,(LONG_PTR)pUserData->pOldWndProc);
							}
							pUserData->pOldWndProc=NULL;
						}
					}
				}
			}
			// Showing control for previous item
			if (pNm->itemNew.lParam!=NULL)
			{
				Item* pItem=(Item*)pNm->itemNew.lParam;
				if (pItem->hControl!=NULL)
				{
					// Changing text
					// Setting text
					WCHAR* pText=pItem->GetText(TRUE);
					if (!IsUnicodeSystem())
						m_pTree->SetItemText(pNm->itemNew.hItem,W2A(pText));
					else
						m_pTree->SetItemText(pNm->itemNew.hItem,pText);
					
					
					// Show control
					::ShowWindow(((Item*)pNm->itemNew.lParam)->hControl,SW_SHOW);
	                
					// Moving it
					RECT rc;
					m_pTree->GetItemRect(pNm->itemNew.hItem,&rc,TRUE);
					m_pTree->ClientToScreen(&rc);
					ScreenToClient(&rc);
					
					int nWidth=60; // 60 is for numeric
					if (pItem->nType==Item::Font || pItem->nType==Item::Color) 
					{
						CDC dc(this);
						HGDIOBJ hOldFont=dc.SelectObject((HFONT)SendMessage(WM_GETFONT));
						CSize sz=dc.GetTextExtent(m_ChangeText);
						nWidth=sz.cx+10;
						dc.SelectObject(hOldFont);
					}
					else if (pItem->nType!=Item::Numeric)
					{
						RECT rcTree;
						m_pTree->GetClientRect(&rcTree);
						nWidth=rcTree.right-rc.right;
						if (pItem->nType==Item::File)
							nWidth-=20;
					}
					::SetWindowPos(pItem->hControl,HWND_TOP,0,0,nWidth,20,SWP_SHOWWINDOW|SWP_NOMOVE);

							

					
					if (pItem->nType==Item::Numeric)
					{
						// Creating Up/Down control
						if (IsUnicodeSystem())
						{
							pItem->hControl2=CreateWindowW(L"msctls_updown32",L"",
								UDS_SETBUDDYINT|UDS_ALIGNRIGHT|UDS_ARROWKEYS|WS_CHILDWINDOW|WS_VISIBLE|UDS_NOTHOUSANDS,
								rc.right+20,rc.top-1,10,10,*this,(HMENU)IDC_SPINCONTROLFORSELECTEDITEM,GetInstanceHandle(),NULL);
							::SendMessageW(pItem->hControl2,UDM_SETBUDDY,WPARAM(pItem->hControl),NULL);
						}
						else
						{
							pItem->hControl2=CreateWindow("msctls_updown32","",
								UDS_SETBUDDYINT|UDS_ALIGNRIGHT|UDS_ARROWKEYS|WS_CHILDWINDOW|WS_VISIBLE|UDS_NOTHOUSANDS,
								rc.right+20,rc.top-1,10,10,*this,(HMENU)IDC_SPINCONTROLFORSELECTEDITEM,GetInstanceHandle(),NULL);
							::SendMessage(pItem->hControl2,UDM_SETBUDDY,WPARAM(pItem->hControl),NULL);
						}
						
						// Subclassing edit control
						UserData* pUserData=(UserData*)::GetWindowLongPtr(pItem->hControl,GWLP_USERDATA);
						if (pUserData!=NULL)
						{
							if (IsUnicodeSystem())
							{
								pUserData->pOldWndProc=(WNDPROC)::SetWindowLongPtrW(pItem->hControl,
									GWLP_WNDPROC,(LONG_PTR)EditSubClassFunc);
							}
							else
							{
								pUserData->pOldWndProc=(WNDPROC)::SetWindowLongPtr(pItem->hControl,
									GWLP_WNDPROC,(LONG_PTR)EditSubClassFunc);
							}
						}


						// Subclassing updown control
						pUserData=new UserData;
						pUserData->pDialog=this;
						if (IsUnicodeSystem())
						{
							pUserData->pOldWndProc=(WNDPROC)::SetWindowLongPtrW(pItem->hControl2,
								GWLP_WNDPROC,(LONG_PTR)EditSubClassFunc);
						}
						else
						{
							pUserData->pOldWndProc=(WNDPROC)::SetWindowLongPtr(pItem->hControl2,
								GWLP_WNDPROC,(LONG_PTR)EditSubClassFunc);
						}

						if (pUserData->pOldWndProc==NULL)
						{
							// Subclassing didn't success
							delete pUserData;
						}
						else
							::SetWindowLongPtr(pItem->hControl2,GWLP_USERDATA,(LONG_PTR)pUserData);

						if (pItem->pProc!=NULL)
						{
							SPINPOXPARAMS spb;
							spb.iLow=0;
							spb.iHigh=MAXLONG;
							pItem->SetValuesForBasicParams(&spb);
							spb.crReason=SPINPOXPARAMS::SetSpinRange;
							spb.pPage=this;
							pItem->pProc(&spb);
							::SendMessage(pItem->hControl2,UDM_SETRANGE32,spb.iLow,spb.iHigh);
						}
						else
							::SendMessage(pItem->hControl2,UDM_SETRANGE32,0,MAXLONG);

						::SetWindowPos(pItem->hControl2,HWND_TOP,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW);
					}
					else if (pItem->nType==Item::File)
					{
						// Browse control
						if (IsUnicodeSystem())
						{
							pItem->hControl2=CreateWindowW(L"BUTTON",L"...",
								BS_PUSHBUTTON|WS_TABSTOP|WS_CHILDWINDOW|WS_VISIBLE,
								rc.right+20,rc.top-1,20,21,*this,(HMENU)IDC_BROWSEBUTTONFORSELECTEDITEM,GetInstanceHandle(),NULL);
						}
						else
						{
							pItem->hControl2=CreateWindow("BUTTON","...",
								BS_PUSHBUTTON|WS_TABSTOP|WS_CHILDWINDOW|WS_VISIBLE,
								rc.right+20,rc.top-1,20,21,*this,(HMENU)IDC_BROWSEBUTTONFORSELECTEDITEM,GetInstanceHandle(),NULL);
						}
						::SendMessage(pItem->hControl2,WM_SETFONT,SendMessage(WM_GETFONT),0);
						
						::SetWindowPos(pItem->hControl2,HWND_TOP,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW);

						// Subclassing control
						UserData* pUserData=new UserData;
						pUserData->pDialog=this;
						if (IsUnicodeSystem())
						{
							pUserData->pOldWndProc=(WNDPROC)::SetWindowLongPtrW(pItem->hControl2,
								GWLP_WNDPROC,(LONG_PTR)EditSubClassFunc);
						}
						else
						{
							pUserData->pOldWndProc=(WNDPROC)::SetWindowLongPtr(pItem->hControl2,
								GWLP_WNDPROC,(LONG_PTR)EditSubClassFunc);
						}
	
						if (pUserData->pOldWndProc==NULL)
						{
							// Subclassing didn't success
							delete pUserData;
						}
						else
							::SetWindowLongPtr(pItem->hControl2,GWLP_USERDATA,(LONG_PTR)pUserData);
					}

					if (pItem->nType!=Item::Color && pItem->nType!=Item::Font)
						PostMessage(WM_FOCUSSELITEMCONTROL);
				}
			}
			break;
		}
	case TVN_SELCHANGEDA:
	case TVN_SELCHANGEDW:
		/*if (pNm->itemNew.lParam!=NULL)
		{
			if (((Item*)pNm->itemNew.lParam)->hControl!=NULL)
				::SetFocus(((Item*)pNm->itemNew.lParam)->hControl);
		}*/
		break;
	case TVN_ITEMEXPANDINGA:
	case TVN_ITEMEXPANDINGW:
		if (((NMTREEVIEWA*)pTvdi)->action==TVE_COLLAPSE || ((NMTREEVIEWA*)pTvdi)->action==TVE_TOGGLE)
			return TRUE;
		return FALSE;
	case TVN_DELETEITEMA:
	case TVN_DELETEITEMW:
		if (((NMTREEVIEW*)pTvdi)->itemOld.lParam!=NULL)
		{
			Item* pItem=(Item*)((NMTREEVIEW*)pTvdi)->itemOld.lParam;
			if (pItem->hControl2!=NULL)
			{
				::DestroyWindow(pItem->hControl2);
				pItem->hControl2=NULL;
			}

			if (pItem->hControl!=NULL)
			{
				if (pItem->nType==Item::Numeric)
				{
					UserData* pData=(UserData*)::GetWindowLongPtr(pItem->hControl,GWLP_USERDATA);
					if (pData!=NULL)
					{
						::SetWindowLongPtr(pItem->hControl,GWLP_USERDATA,NULL);
						if (pData->pOldWndProc!=NULL)
						{
							if (IsUnicodeSystem())
								::SetWindowLongPtrW(pItem->hControl,GWLP_WNDPROC,(LONG_PTR)pData->pOldWndProc);
							else
								::SetWindowLongPtr(pItem->hControl,GWLP_WNDPROC,(LONG_PTR)pData->pOldWndProc);
						}

						delete pData;
					}
					
				}
				::DestroyWindow(pItem->hControl);

				pItem->hControl=NULL;
			}
		}
		break;
	case NM_CUSTOMDRAW:
		{
			NMTVCUSTOMDRAW* pCustomDraw=(NMTVCUSTOMDRAW*)pTvdi;
			if (pCustomDraw->nmcd.dwDrawStage==CDDS_PREPAINT)
				return CDRF_NOTIFYITEMDRAW|CDRF_NOTIFYPOSTPAINT;
			else if (pCustomDraw->nmcd.dwDrawStage==CDDS_POSTPAINT)
			{
				PostMessage(WM_REDRAWSELITEMCONTROL);
				return CDRF_NOTIFYITEMDRAW;
			}
			else if (pCustomDraw->nmcd.dwDrawStage&CDDS_ITEMPREPAINT)
			{
				Item* pItem=(Item*)pCustomDraw->nmcd.lItemlParam;
				if (!pItem->bEnabled)
					pCustomDraw->clrText=GetSysColor(COLOR_GRAYTEXT);
				return CDRF_DODEFAULT;
			}
			break;
		}
	}
	return FALSE;
}


BOOL COptionsPropertyPage::SetCheckState(HTREEITEM hItem,COptionsPropertyPage::Item* pItem,
										 COptionsPropertyPage::NewState nNewState)
{
	if (nNewState==Toggle && pItem->nType==Item::RadioBox)
		nNewState=Checked;

	if (pItem->pProc!=NULL)
	{
		CHANGINGVALPARAMS cp;
		cp.crReason=BASICPARAMS::ChangingValue;
		cp.pPage=this;
		pItem->SetValuesForBasicParams(&cp);
		cp.nNewState=nNewState;
		if (!pItem->pProc(&cp))
			return FALSE;
	}

	if (pItem->nType==Item::CheckBox || pItem->nType==Item::RadioBox)
	{
		if (nNewState==Toggle)
		    pItem->bChecked=!pItem->bChecked;
		else if (nNewState==Checked)
		{
			if (pItem->bChecked)
				return FALSE;
			pItem->bChecked=TRUE;
		}
		else
		{
			if (!pItem->bChecked)
				return FALSE;
			pItem->bChecked=FALSE;
		}
		m_pTree->SetItemState(hItem,INDEXTOSTATEIMAGEMASK(pItem->GetStateImage(&m_Images)),TVIS_STATEIMAGEMASK);

		if (pItem->nType==Item::RadioBox && pItem->bChecked)
			UncheckOtherRadioButtons(hItem,m_pTree->GetParentItem(hItem));
		
		EnableChilds(hItem,pItem->bChecked);
		
		
		if (pItem->pProc!=NULL)
		{
			BASICPARAMS bp;
			bp.crReason=BASICPARAMS::Set;
			bp.pPage=this;
			pItem->SetValuesForBasicParams(&bp);
			pItem->pProc(&bp);
		}
		return TRUE;
	}
	return FALSE;
}

BOOL COptionsPropertyPage::SetNumericValue(Item* pItem)
{
	int iTextLen=(int)::SendMessage(pItem->hControl,WM_GETTEXTLENGTH,0,0)+1;
	char* szText=new char[iTextLen+1];
	::SendMessage(pItem->hControl,WM_GETTEXT,iTextLen,LPARAM(szText));

	CHANGINGVALPARAMS cp;
	cp.lNewValue=atol(szText);
	delete[] szText;

	// Asking wheter value can be changed
	if (pItem->pProc!=NULL)
	{
		cp.crReason=BASICPARAMS::ChangingValue;
		pItem->SetValuesForBasicParams(&cp);
		cp.pPage=this;
		if (!pItem->pProc(&cp))
			return FALSE;
	}
	pItem->lValue=cp.lNewValue;
	if (pItem->pProc!=NULL)
	{
		cp.crReason=BASICPARAMS::Set;
		cp.lValue=pItem->lValue;
		pItem->pProc(&cp);
	}
	return TRUE;
}

BOOL COptionsPropertyPage::SetTextValue(Item* pItem)
{
	

	CHANGINGVALPARAMS cp;
	int iTextLen,iCurSel;
	switch (pItem->nType)
	{
	case Item::Combo:
	case Item::List:
		iCurSel=(int)::SendMessage(pItem->hControl,CB_GETCURSEL,0,0);
		if (iCurSel!=CB_ERR)
		{
			iTextLen=(int)::SendMessage(pItem->hControl,CB_GETLBTEXTLEN,iCurSel,0)+2;			
			cp.pNewData=new WCHAR[iTextLen];
				
			if (IsUnicodeSystem())
				::SendMessageW(pItem->hControl,CB_GETLBTEXT,iCurSel,LPARAM(cp.pNewData));
			else
			{
				char* pAText=new char[iTextLen];
				::SendMessage(pItem->hControl,CB_GETLBTEXT,iCurSel,LPARAM(pAText));
				MultiByteToWideChar(CP_ACP,0,pAText,-1,cp.pNewData,iTextLen);
				delete[] pAText;
			}			
		}
		else
		{
			iTextLen=(int)::SendMessage(pItem->hControl,WM_GETTEXTLENGTH,iCurSel,0)+2;			
			cp.pNewData=new WCHAR[iTextLen];
			
			
			if (IsUnicodeSystem())
				::SendMessageW(pItem->hControl,WM_GETTEXT,iTextLen,LPARAM(cp.pNewData));
			else
			{
				char* pAText=new char[iTextLen];
				::SendMessage(pItem->hControl,WM_GETTEXT,iTextLen,LPARAM(pAText));
				MultiByteToWideChar(CP_ACP,0,pAText,-1,cp.pNewData,iTextLen);
				delete[] pAText;
			}		
			
			
		}

		break;
	default:
		iTextLen=(int)::SendMessage(pItem->hControl,WM_GETTEXTLENGTH,0,0)+1;
		cp.pNewData=new WCHAR[max(iTextLen,2)];
		if (IsUnicodeSystem())
			::SendMessageW(pItem->hControl,WM_GETTEXT,iTextLen,LPARAM(cp.pNewData));
		else
		{
			char* pAText=new char[iTextLen];
			::SendMessage(pItem->hControl,WM_GETTEXT,iTextLen,LPARAM(pAText));
			MultiByteToWideChar(CP_ACP,0,pAText,-1,cp.pNewData,iTextLen);
			delete[] pAText;
		}	
		
		break;
	}
	
	

	// Asking wheter value can be changed
	if (pItem->pProc!=NULL)
	{
		cp.crReason=BASICPARAMS::ChangingValue;
		pItem->SetValuesForBasicParams(&cp);
		cp.pPage=this;
		if (!pItem->pProc(&cp))
		{
			delete[] cp.pNewData;
			return FALSE;
		}
	}

	
	if (pItem->pData!=NULL)
		delete[] pItem->pData;

	
	pItem->pData=cp.pNewData;
	if (pItem->pProc!=NULL)
	{
		cp.crReason=BASICPARAMS::Set;
		cp.pData=cp.pData;
		pItem->pProc(&cp);
	}
	
	return TRUE;
}

BOOL COptionsPropertyPage::SetListValue(Item* pItem)
{
	CHANGINGVALPARAMS cp;
	cp.lNewValue=(LONG)::SendMessage(pItem->hControl,CB_GETCURSEL,0,0);
	
	// Asking wheter value can be changed
	if (pItem->pProc!=NULL)
	{
		cp.crReason=BASICPARAMS::ChangingValue;
		pItem->SetValuesForBasicParams(&cp);
		cp.pPage=this;
		if (!pItem->pProc(&cp))
			return FALSE;
	}
	pItem->lValue=cp.lNewValue;
	if (pItem->pProc!=NULL)
	{
		cp.crReason=BASICPARAMS::Set;
		cp.lValue=pItem->lValue;
		pItem->pProc(&cp);
	}
	return TRUE;
}

BOOL COptionsPropertyPage::SetColorValue(Item* pItem,COLORREF cNewColor)
{
	CHANGINGVALPARAMS cp;
	cp.cNewColor=cNewColor;
	
	// Asking wheter value can be changed
	if (pItem->pProc!=NULL)
	{
		cp.crReason=BASICPARAMS::ChangingValue;
		pItem->SetValuesForBasicParams(&cp);
		cp.pPage=this;
		if (!pItem->pProc(&cp))
			return FALSE;
	}
	pItem->cColor=cp.cNewColor;
	if (pItem->pProc!=NULL)
	{
		cp.crReason=BASICPARAMS::Set;
		cp.cColor=pItem->cColor;
		pItem->pProc(&cp);
	}

	pItem->m_nStateIcon=pItem->IconFromColor(&m_Images,pItem->m_nStateIcon);
	return TRUE;
}

BOOL COptionsPropertyPage::SetFontValue(Item* pItem,LOGFONT* pLogFont)
{
	CHANGINGVALPARAMS cp;
	cp.pNewLogFont=pLogFont;
	
	// Asking wheter value can be changed
	if (pItem->pProc!=NULL)
	{
		cp.crReason=BASICPARAMS::ChangingValue;
		pItem->SetValuesForBasicParams(&cp);
		cp.pPage=this;
		if (!pItem->pProc(&cp))
			return FALSE;
	}
	if (pItem->pLogFont==NULL)
		pItem->pLogFont=new LOGFONT;
    CopyMemory(pItem->pLogFont,cp.pNewLogFont,sizeof(LOGFONT));
	
	if (pItem->pProc!=NULL)
	{
		cp.crReason=BASICPARAMS::Set;
		cp.pLogFont=pItem->pLogFont;
		pItem->pProc(&cp);
	}
	return TRUE;
}
	
	
void COptionsPropertyPage::EnableChilds(HTREEITEM hItem,BOOL bEnable)
{
	HTREEITEM hChildItem=m_pTree->GetNextItem(hItem,TVGN_CHILD);
    while (hChildItem!=NULL)
	{
		Item* pItem=(Item*)m_pTree->GetItemData(hChildItem);
		if (pItem!=NULL)
			pItem->bEnabled=bEnable;
		
		m_pTree->SetItemState(hChildItem,bEnable?0:TVIS_CUT,TVIS_CUT);
		
		hChildItem=m_pTree->GetNextItem(hChildItem,TVGN_NEXT);
	}
}

void COptionsPropertyPage::UncheckOtherRadioButtons(HTREEITEM hItem,HTREEITEM hParent)
{
	if (hParent==NULL)
		return;
	
	HTREEITEM hChilds;
	if (hParent==NULL)
		hChilds=m_pTree->GetNextItem(NULL,TVGN_ROOT);
	else
		hChilds=m_pTree->GetNextItem(hParent,TVGN_CHILD);



	while (hChilds!=NULL)
	{
		Item* pItem=(Item*)m_pTree->GetItemData(hChilds);
		if (pItem!=NULL)
		{
			if (pItem->nType==Item::RadioBox && hChilds!=hItem)
				SetCheckState(hChilds,(Item*)m_pTree->GetItemData(hChilds),Unchecked);
		}
		hChilds=m_pTree->GetNextItem(hChilds,TVGN_NEXT);
	}
}

WCHAR* COptionsPropertyPage::Item::GetText(BOOL bActive) const
{
	switch (nType)
	{
	case Numeric:
		if (hControl!=NULL && !bActive)
		{
			WCHAR szText[100];
			_itow_s(lValue,szText,100,10);
			int iLength=(int)istrlenw(szText)+1;
			int iLabelLen=(int)istrlenw(pString);
			
			WCHAR* pText=new WCHAR[iLabelLen+iLength+2];
			MemCopyW(pText,pString,iLabelLen);
			pText[iLabelLen++]=' ';
			MemCopyW(pText+iLabelLen,szText,iLength);
			return pText;
		}
		return pString;
	case List:
    case Combo:
		if (hControl!=NULL && !bActive)
		{
			
			CComboBox cb(hControl);

			int nCurSel=cb.GetCurSel();
			int iLength=(int)((nCurSel!=-1)?cb.GetLBTextLen(nCurSel)+1:cb.GetTextLength()+1);
			int iLabelLen=(int)istrlenw(pString);
			
			WCHAR* pText=new WCHAR[iLabelLen+iLength+2];
			MemCopyW(pText,pString,iLabelLen);
			pText[iLabelLen++]=' ';
				
			if (nCurSel!=-1)
				cb.GetLBText(nCurSel,pText+iLabelLen);
			else
				cb.GetWindowText(pText+iLabelLen,iLength+1);

			return pText;
		}
		return pString;
	case Edit:
	case File:
		if (hControl!=NULL && !bActive)
		{
			
			int iLength=(int)::SendMessage(hControl,WM_GETTEXTLENGTH,0,0)+1;
			int iLabelLen=(int)istrlenw(pString);
			
			WCHAR* pText=new WCHAR[iLabelLen+iLength+1];
			MemCopyW(pText,pString,iLabelLen);
			pText[iLabelLen++]=' ';
				
			if (!IsUnicodeSystem())
			{
				// 9x
				char* pTemp=new char[iLength+2];
				::GetWindowText(hControl,pTemp,iLength);
				MemCopyAtoW(pText+iLabelLen,iLength+2-iLabelLen,pTemp,iLength);
				delete[] pTemp;
			}
			else
				::GetWindowTextW(hControl,pText+iLabelLen,iLength);
				
			return pText;
		}
		return pString;
	case Font:
		if (pLogFont!=NULL)
		{
			
			CStringW str(pString);
			str << L' ' << pLogFont->lfFaceName;

			if (pLogFont->lfHeight<0)
			{
				// Getting device caps
				HDC hScreenDC=::GetDC(NULL);
				int pt=MulDiv(-pLogFont->lfHeight, 72,::GetDeviceCaps(hScreenDC,LOGPIXELSY));
				::ReleaseDC(NULL,hScreenDC);
				
				str << L' ' << pt;
			}

			
			return str.GiveBuffer();
		}
		return pString;
	case RadioBox:
	case CheckBox:
	case Root:
	case Color:
	default:
		return pString;
	}
}

// lParam is pointer to DWORD value which is will be set
// wParam is used mask
BOOL CALLBACK COptionsPropertyPage::DefaultCheckBoxProc(COptionsPropertyPage::BASICPARAMS* pParams)
{
	switch (pParams->crReason)
	{
	case BASICPARAMS::Initialize:
		break;
	case BASICPARAMS::Get:
		pParams->bChecked=(*((DWORD*)pParams->lParam))&pParams->wParam?TRUE:FALSE;
		break;
	case BASICPARAMS::Set:
		break;
	case BASICPARAMS::Apply:
		if (pParams->bChecked)
			*((DWORD*)pParams->lParam)|=DWORD(pParams->wParam);
		else
			*((DWORD*)pParams->lParam)&=~DWORD(pParams->wParam);
		break;
	case BASICPARAMS::ChangingValue:
		break;
	}
	return TRUE;
}

// lParam is pointer to DWORD value which is will be set
// wParam is used mask
BOOL CALLBACK COptionsPropertyPage::DefaultInverseCheckBoxProc(COptionsPropertyPage::BASICPARAMS* pParams)
{
	switch (pParams->crReason)
	{
	case BASICPARAMS::Initialize:
		break;
	case BASICPARAMS::Get:
		pParams->bChecked=(*((DWORD*)pParams->lParam))&pParams->wParam?FALSE:TRUE;
		break;
	case BASICPARAMS::Set:
		break;
	case BASICPARAMS::Apply:
		if (pParams->bChecked)
			*((DWORD*)pParams->lParam)&=~DWORD(pParams->wParam);
		else
			*((DWORD*)pParams->lParam)|=DWORD(pParams->wParam);
		break;
	case BASICPARAMS::ChangingValue:
		break;
	}
	return TRUE;
}

// lParam is pointer to DWORD value which is will be set
// HIWORD of wParam is mask to be setted, LOWORD is value
BOOL CALLBACK COptionsPropertyPage::DefaultRadioBoxProc(COptionsPropertyPage::BASICPARAMS* pParams)
{
	switch (pParams->crReason)
	{
	case BASICPARAMS::Initialize:
		break;
	case BASICPARAMS::Get:
		if (pParams->lParam!=NULL)
			pParams->bChecked=((*((DWORD*)pParams->lParam))&(HIWORD(pParams->wParam)))==LOWORD(pParams->wParam);
		else
			pParams->bChecked=0;
		break;
	case BASICPARAMS::Set:
		/*
		*((DWORD*)pParams->lParam)&=~HIWORD(pParams->wParam);
		if (pParams->bChecked)
			*((DWORD*)pParams->lParam)|=LOWORD(pParams->wParam);
		*/
		break;
	case BASICPARAMS::Apply:
		if (pParams->bChecked && pParams->lParam!=NULL)
		{
			*((DWORD*)pParams->lParam)&=~HIWORD(pParams->wParam);
			*((DWORD*)pParams->lParam)|=LOWORD(pParams->wParam);
		}
		break;		
	case BASICPARAMS::ChangingValue:
		break;
	}
	return TRUE;
}

// lParam is pointer to DWORD value which is will be set
// HIWORD of wParam is mask (shifted 16 bits) to be setted, LOWORD is value (shifted 16 bit)
BOOL CALLBACK COptionsPropertyPage::DefaultRadioBoxShiftProc(COptionsPropertyPage::BASICPARAMS* pParams)
{
	switch (pParams->crReason)
	{
	case BASICPARAMS::Initialize:
		break;
	case BASICPARAMS::Get:
		pParams->bChecked=((*((DWORD*)pParams->lParam))&(HIWORD(pParams->wParam)<<16))==LOWORD(pParams->wParam)<<16;
		break;
	case BASICPARAMS::Set:
		/*
		*((DWORD*)pParams->lParam)&=~HIWORD(pParams->wParam);
		if (pParams->bChecked)
			*((DWORD*)pParams->lParam)|=LOWORD(pParams->wParam);
		*/
		break;
	case BASICPARAMS::Apply:
		if (pParams->bChecked)
		{
			*((DWORD*)pParams->lParam)&=~(HIWORD(pParams->wParam)<<16);
			*((DWORD*)pParams->lParam)|=LOWORD(pParams->wParam)<<16;
		}
		break;		
	case BASICPARAMS::ChangingValue:
		break;
	}
	return TRUE;
}

// lParam is pointer to DWORD value which is will be set
// if wParam==0, all values are accepted
// if wParam==-1, positive values are accepted
// otherwise HIWORD is maximum, LOWORD is minimum
BOOL CALLBACK COptionsPropertyPage::DefaultNumericProc(COptionsPropertyPage::BASICPARAMS* pParams)
{
	switch (pParams->crReason)
	{
	case BASICPARAMS::Initialize:
		break;
	case BASICPARAMS::Get:
		pParams->lValue=*((DWORD*)pParams->lParam);
		if (pParams->wParam==0) 
			break; // Accept all values
		else if (pParams->wParam==DWORD(-1))
		{
			// -1: Accept only nonnegative values
			if (pParams->lValue<0)
				pParams->lValue=0;
		}
		else if (pParams->lValue>int(HIWORD(pParams->wParam)))
			pParams->lValue=int(HIWORD(pParams->wParam));
		else if (pParams->lValue<int(LOWORD(pParams->wParam)))
			pParams->lValue=int(LOWORD(pParams->wParam));
		break;
	case BASICPARAMS::Set:
		/*
		*((DWORD*)pParams->lParam)&=~HIWORD(pParams->wParam);
		if (pParams->bChecked)
			*((DWORD*)pParams->lParam)|=LOWORD(pParams->wParam);
		*/
		break;
	case BASICPARAMS::Apply:
		*((DWORD*)pParams->lParam)=pParams->lValue;
		break;		
	case BASICPARAMS::SetSpinRange:
		if (pParams->wParam==0)
		{
			((SPINPOXPARAMS*)pParams)->iLow=MINLONG;
			((SPINPOXPARAMS*)pParams)->iHigh=MAXLONG;
		}
		else if (pParams->wParam==DWORD(-1))
		{
			((SPINPOXPARAMS*)pParams)->iLow=0;
			((SPINPOXPARAMS*)pParams)->iHigh=MAXLONG;
		}
		else
		{
			((SPINPOXPARAMS*)pParams)->iLow=LOWORD(pParams->wParam);
			((SPINPOXPARAMS*)pParams)->iHigh=HIWORD(pParams->wParam);
		}
		break;
	case BASICPARAMS::ChangingValue:
		if (pParams->wParam==0) // 
			break;
		else if (pParams->wParam==DWORD(-1))
		{
			if (((CHANGINGVALPARAMS*)pParams)->lNewValue<0)
				return FALSE;
		}
		else if (((CHANGINGVALPARAMS*)pParams)->lNewValue<int(LOWORD(pParams->wParam)) || 
			((CHANGINGVALPARAMS*)pParams)->lNewValue>int(HIWORD(pParams->wParam)))
			return FALSE;
		break;
	}
	return TRUE;
}

// lParam is pointer to string class which will be set
BOOL CALLBACK COptionsPropertyPage::DefaultEditStrProc(BASICPARAMS* pParams)
{
	switch(pParams->crReason)
	{
	case BASICPARAMS::Initialize:
		break;
	case BASICPARAMS::Get:
		ASSERT(pParams->pData==NULL);
		pParams->pData=alloccopyAtoW(*(CString*)pParams->lParam);
		break;
	case BASICPARAMS::Set:
		break;
	case BASICPARAMS::Apply:
		if (pParams->pData==NULL)
			((CString*)pParams->lParam)->Empty();
		else
			((CString*)pParams->lParam)->Copy(pParams->pData);
		break;
	case BASICPARAMS::ChangingValue:
		break;
	}		
	return TRUE;
}

// lParam is pointer to CStringW which will be set
BOOL CALLBACK COptionsPropertyPage::DefaultEditStrWProc(BASICPARAMS* pParams)
{
	switch(pParams->crReason)
	{
	case BASICPARAMS::Initialize:
		break;
	case BASICPARAMS::Get:
		ASSERT(pParams->pData==NULL);
		pParams->pData=alloccopy(*(CStringW*)pParams->lParam);
		break;
	case BASICPARAMS::Set:
		break;
	case BASICPARAMS::Apply:
		if (pParams->pData==NULL)
			((CStringW*)pParams->lParam)->Empty();
		else
			((CStringW*)pParams->lParam)->Copy(pParams->pData);
		break;
	case BASICPARAMS::ChangingValue:
		break;
	}		
	return TRUE;
}

// lParam is pointer to LOGFONT strcut
BOOL CALLBACK COptionsPropertyPage::DefaultFontProc(BASICPARAMS* pParams)
{
	switch(pParams->crReason)
	{
	case BASICPARAMS::Initialize:
		break;
	case BASICPARAMS::Get:
		if (pParams->pLogFont==NULL)
			pParams->pLogFont=new LOGFONT;
		CopyMemory(pParams->pLogFont,pParams->lParam,sizeof(LOGFONT));
		break;
	case BASICPARAMS::Set:
		break;
	case BASICPARAMS::Apply:
		CopyMemory(pParams->lParam,pParams->pLogFont,sizeof(LOGFONT));	
		break;
	case BASICPARAMS::ChangingValue:
		break;
	}		
	return TRUE;
}

// lParam is pointer to COLORREF
BOOL CALLBACK COptionsPropertyPage::DefaultColorProc(BASICPARAMS* pParams)
{
	switch(pParams->crReason)
	{
	case BASICPARAMS::Initialize:
		break;
	case BASICPARAMS::Get:
		pParams->cColor=*((COLORREF*)pParams->lParam);
		break;
	case BASICPARAMS::Set:
		break;
	case BASICPARAMS::Apply:
		*((COLORREF*)pParams->lParam)=pParams->cColor;
		break;
	case BASICPARAMS::ChangingValue:
		break;
	}		
	return TRUE;
}


LRESULT CALLBACK COptionsPropertyPage::TreeSubClassFunc(HWND hWnd,UINT uMsg,
															  WPARAM wParam,LPARAM lParam)
{
	UserData* pUserData=(UserData*)::GetWindowLongPtr(hWnd,GWLP_USERDATA);
	LRESULT lRet=FALSE;

	if (pUserData==NULL)
		return FALSE;

	
	switch (uMsg)
	{
	case WM_DESTROY:
		// Calling original window procedure
		lRet=CallWindowProc(pUserData->pOldWndProc,hWnd,uMsg,wParam,lParam);

		// Desubclassing
		if (IsUnicodeSystem())
			::SetWindowLongPtrW(hWnd,GWLP_WNDPROC,(LONG_PTR)pUserData->pOldWndProc);
		else
			::SetWindowLongPtr(hWnd,GWLP_WNDPROC,(LONG_PTR)pUserData->pOldWndProc);
		
		// Free memory
		delete pUserData;
		return lRet;
	case WM_KEYDOWN:
		if ((wParam==VK_DOWN || wParam==VK_UP) &&
			!(GetKeyState(VK_CONTROL)&0x8000))
		{
			HTREEITEM hCurrentItem=TreeView_GetSelection(hWnd);
			if (hCurrentItem!=NULL)
			{
				HTREEITEM hNextItem;
				
				for (;;)
				{
					if (wParam==VK_DOWN)
						hNextItem=TreeView_GetNextVisible(hWnd,hCurrentItem);
					else
						hNextItem=TreeView_GetPrevVisible(hWnd,hCurrentItem);

					if (hNextItem==NULL)
						break;

					TVITEM tv;
					tv.hItem=hNextItem;
					tv.mask=TVIF_HANDLE|TVIF_PARAM;
					TreeView_GetItem(hWnd,&tv);					

					if (((Item*)tv.lParam)->bEnabled)
						break;

					hCurrentItem=hNextItem;
				}
				
				if (hNextItem!=NULL)
				{
					::SetFocus(hWnd);
					TreeView_SelectItem(hWnd,hNextItem);
				}
				break;
			}			
		}
		// Continue to default
	default:
		lRet=CallWindowProc(pUserData->pOldWndProc,hWnd,uMsg,wParam,lParam);
		break;
	}

	return lRet;
}

LRESULT CALLBACK COptionsPropertyPage::ButtonSubClassFunc(HWND hWnd,UINT uMsg,
															  WPARAM wParam,LPARAM lParam)
{
	

	UserData* pUserData=(UserData*)::GetWindowLongPtr(hWnd,GWLP_USERDATA);
	LRESULT lRet=FALSE;

	if (pUserData==NULL)
		return FALSE;

	
	switch (uMsg)
	{
	case WM_DESTROY:
		// Calling original window procedure
		lRet=CallWindowProc(pUserData->pOldWndProc,hWnd,uMsg,wParam,lParam);

		// Desubclassing
		if (IsUnicodeSystem())
			::SetWindowLongPtrW(hWnd,GWLP_WNDPROC,(LONG_PTR)pUserData->pOldWndProc);
		else
			::SetWindowLongPtr(hWnd,GWLP_WNDPROC,(LONG_PTR)pUserData->pOldWndProc);
		
		// Free memory
		delete pUserData;
		return lRet;
	case WM_KEYDOWN:
		if ((wParam==VK_DOWN || wParam==VK_UP) &&
			!(GetKeyState(VK_CONTROL)&0x8000))
			pUserData->pDialog->m_pTree->SendMessage(WM_KEYDOWN,wParam,lParam);

		// Continue to default
	default:
		lRet=CallWindowProc(pUserData->pOldWndProc,hWnd,uMsg,wParam,lParam);
		break;
	}

	return lRet;
}

LRESULT CALLBACK COptionsPropertyPage::EditSubClassFunc(HWND hWnd,UINT uMsg,
															  WPARAM wParam,LPARAM lParam)
{
	UserData* pUserData=(UserData*)::GetWindowLongPtr(hWnd,GWLP_USERDATA);
	LRESULT lRet=FALSE;

	if (pUserData==NULL)
		return FALSE;

	
	switch (uMsg)
	{
	case WM_DESTROY:
		// Calling original window procedure
		lRet=CallWindowProc(pUserData->pOldWndProc,hWnd,uMsg,wParam,lParam);

		// Desubclassing
		if (IsUnicodeSystem())
			::SetWindowLongPtrW(hWnd,GWLP_WNDPROC,(LONG_PTR)pUserData->pOldWndProc);
		else
			::SetWindowLongPtr(hWnd,GWLP_WNDPROC,(LONG_PTR)pUserData->pOldWndProc);
		
		// Free memory
		delete pUserData;
		return lRet;
	case WM_KEYDOWN:
		if (wParam==VK_DOWN || wParam==VK_UP)
			return pUserData->pDialog->m_pTree->SendMessage(WM_KEYDOWN,wParam,lParam);


		// Continue to default
	default:
		lRet=CallWindowProc(pUserData->pOldWndProc,hWnd,uMsg,wParam,lParam);
		break;
	}

	return lRet;
}

LRESULT CALLBACK COptionsPropertyPage::ComboSubClassFunc(HWND hWnd,UINT uMsg,
															  WPARAM wParam,LPARAM lParam)
{
	UserData* pUserData=(UserData*)::GetWindowLongPtr(hWnd,GWLP_USERDATA);
	LRESULT lRet=FALSE;

	if (pUserData==NULL)
		return FALSE;

	
	switch (uMsg)
	{
	case WM_DESTROY:
		// Calling original window procedure
		lRet=CallWindowProc(pUserData->pOldWndProc,hWnd,uMsg,wParam,lParam);

		// Desubclassing
		if (IsUnicodeSystem())
			::SetWindowLongPtrW(hWnd,GWLP_WNDPROC,(LONG_PTR)pUserData->pOldWndProc);
		else
			::SetWindowLongPtr(hWnd,GWLP_WNDPROC,(LONG_PTR)pUserData->pOldWndProc);
		
		// Free memory
		delete pUserData;
		return lRet;
	case WM_KEYDOWN:
		if ((wParam==VK_DOWN || wParam==VK_UP) && 
			!(GetKeyState(VK_MENU)&0x8000))
		{
			if (GetWindow(hWnd,GW_CHILD)==NULL)
			{
				// Edit control
				if (!::SendMessage(::GetParent(hWnd),CB_GETDROPPEDSTATE,0,0))	
					return pUserData->pDialog->m_pTree->SendMessage(WM_KEYDOWN,wParam,lParam);
			}
			else if (!::SendMessage(hWnd,CB_GETDROPPEDSTATE,0,0))	
				return pUserData->pDialog->m_pTree->SendMessage(WM_KEYDOWN,wParam,lParam);
		}

		// Continue to default
	default:
		lRet=CallWindowProc(pUserData->pOldWndProc,hWnd,uMsg,wParam,lParam);
		break;
	}

	return lRet;
}




LPCSTR COptionsPropertyPage::GetHelpID(HELPINFO* pHelpInfo) const
{
	// Not the tree control
	if (pHelpInfo->iContextType!=HELPINFO_WINDOW && pHelpInfo->iCtrlId!=m_nTreeID)
		return NULL;

	HTREEITEM hItem;
	if (GetKeyState(VK_F1) & 0x8000)
	{
		// F1 pressed to open help, use selected item
		hItem=m_pTree->GetSelectedItem();
	}
	else
	{
		// '?' used to open file, using cursor
		POINT ptWrtClient=pHelpInfo->MousePos;
		m_pTree->ScreenToClient(&ptWrtClient);
		hItem=m_pTree->HitTest(ptWrtClient);
	}


	if (hItem!=NULL)
	{
		Item* pItem=(Item*)m_pTree->GetItemData(hItem);
		return pItem!=NULL?pItem->pHelpID:NULL;
	}
	return NULL;

}


HTREEITEM COptionsPropertyPage::FindItem(LPCSTR pText,BOOL bBackwardDirection,BOOL bPartial,BOOL bNoDisabled,HTREEITEM hItem,HTREEITEM hEnd) const
{
	if (!bNoDisabled)
		return m_pTree->FindItem(pText,bBackwardDirection,bPartial,hItem,hEnd);

	while ((hItem=m_pTree->FindItem(pText,bBackwardDirection,bPartial,hItem,hEnd))!=NULL)
	{
		if (IsItemEnabled(hItem))
			return hItem;
	}
	return NULL;
}

HTREEITEM COptionsPropertyPage::FindItem(LPCWSTR pText,BOOL bBackwardDirection,BOOL bPartial,BOOL bNoDisabled,HTREEITEM hItem,HTREEITEM hEnd) const
{
	if (!bNoDisabled)
		return m_pTree->FindItem(pText,bBackwardDirection,bPartial,hItem,hEnd);

	while ((hItem=m_pTree->FindItem(pText,bBackwardDirection,bPartial,hItem,hEnd))!=NULL)
	{
		if (IsItemEnabled(hItem))
			return hItem;
	}
	return NULL;
}

	

#endif