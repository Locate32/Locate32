/* Locate32 - Copyright (c) 1997-2010 Janne Huttunen */

#include <HFCLib.h>
#include "Locate32.h"

#include <Winternl.h>

BOOL CAboutDlg::OnCommand(WORD wID, WORD wNotifyCode, HWND hControl)
{
	switch(wID)
	{
	case IDCANCEL:
	case IDC_OK:
		{
			EndDialog(0);
			break;
		}
	case IDC_MAILME:
		{
			CWaitCursor wait;
			ShellFunctions::ShellExecute(*this,NULL,"http://locate32.net/content/view/51/43/",
				NULL,NULL,0);
			break;
		}
	case IDC_GOTOHOMEPAGE:
		{
			CWaitCursor wait;
			ShellFunctions::ShellExecute(*this,NULL,"http://www.locate32.net", //http://locate32.webhop.or",
				NULL,NULL,0);
			break;
		}
	case IDC_YOURRIGHT:
	case IDC_DONATE:
		{
			CWaitCursor wait;
			ShellFunctions::ShellExecute(*this,NULL,
				"https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=Janne%2eHuttunen%40locate32%2enet&lc=FI&item_name=Locate32&no_shipping=1&currency_code=EUR&bn=PP%2dDonationsBF%3abtn_donate_LG%2egif%3aNonHosted",
				NULL,NULL,0);
			break;
		}
	}
	return FALSE;
}

BOOL CAboutDlg::OnClose()
{
	CDialog::OnClose();
	EndDialog(0);
	return FALSE;
}

BOOL CAboutDlg::OnInitDialog(HWND hwndFocus)
{
	CDialog::OnInitDialog(hwndFocus);
	CWaitCursor wait;
	OSVERSIONINFO ver;
	MEMORYSTATUS mem;
	CStringW text,text2;

	// Setting banner and donate button
	SendDlgItemMessage(IDC_ABOUTBANNER,STM_SETIMAGE,IMAGE_BITMAP,
		(LPARAM)LoadImage(IDB_ABOUTBANNER,IMAGE_BITMAP,0,0,LR_SHARED|LR_DEFAULTSIZE));
	SendDlgItemMessage(IDC_DONATE,STM_SETIMAGE,IMAGE_BITMAP,
		(LPARAM)LoadImage(IDB_DONATE,IMAGE_BITMAP,0,0,LR_SHARED|LR_DEFAULTSIZE));
	
	// Creating copyright and version strings
	{
		CStringW str;
#ifdef _DEBUG
		str.Format(L"%s © 1997-2010 Janne Huttunen\nTHIS IS DEBUG VERSION, %s %s",
			(LPCWSTR)ID2W(IDS_COPYRIGHT),(LPCWSTR)A2W(__DATE__),(LPCWSTR)A2W(__TIME__));
#else
		str.Format(L"%s © 1997-2010 Janne Huttunen",(LPCWSTR)ID2W(IDS_COPYRIGHT));
#endif
		SetDlgItemText(IDC_COPYRIGHT,str);

		if (IsUnicodeSystem())
		{
			CStringW sExeName=GetApp()->GetExeNameW();
			UINT iDataLength=GetFileVersionInfoSizeW(sExeName,NULL);
			BYTE* pData=new BYTE[iDataLength];
			GetFileVersionInfoW(sExeName,NULL,iDataLength,pData);
			VOID* pTranslations,* pProductVersion=NULL;
			VerQueryValueW(pData,L"VarFileInfo\\Translation",&pTranslations,&iDataLength);
			WCHAR szTranslation[100];
			StringCbPrintfW(szTranslation,100*sizeof(WCHAR),L"\\StringFileInfo\\%04X%04X\\ProductVersion",LPWORD(pTranslations)[0],LPWORD(pTranslations)[1]);
			if (!VerQueryValueW(pData,szTranslation,&pProductVersion,&iDataLength))
				VerQueryValueW(pData,L"\\StringFileInfo\\040904b0\\ProductVersion",&pProductVersion,&iDataLength);
			
			if (pProductVersion!=NULL)
				SetDlgItemText(IDC_VERSION,CStringW(IDS_VERSION)+LPCWSTR(pProductVersion));

			delete[] pData;
		}
		else
		{
			UINT iDataLength=GetFileVersionInfoSize(GetApp()->GetExeName(),NULL);
			BYTE* pData=new BYTE[iDataLength];
			GetFileVersionInfo(GetApp()->GetExeName(),NULL,iDataLength,pData);
			VOID* pTranslations,* pProductVersion=NULL;
			VerQueryValue(pData,"VarFileInfo\\Translation",&pTranslations,&iDataLength);
			char szTranslation[100];
			StringCbPrintf(szTranslation,100,"\\StringFileInfo\\%04X%04X\\ProductVersion",LPWORD(pTranslations)[0],LPWORD(pTranslations)[1]);
			if (!VerQueryValue(pData,szTranslation,&pProductVersion,&iDataLength))
				VerQueryValue(pData,"\\StringFileInfo\\040904b0\\ProductVersion",&pProductVersion,&iDataLength);
			
			if (pProductVersion!=NULL)
				SetDlgItemText(IDC_VERSION,CString(IDS_VERSION)+LPCSTR(pProductVersion));

			delete[] pData;
		}
		

	}
	
	ver.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
	if(GetVersionEx(&ver))
	{
		switch(ver.dwPlatformId)
		{
		case VER_PLATFORM_WIN32s:
			text2.LoadString(IDS_SYSWIN32S);
			break;
		case VER_PLATFORM_WIN32_WINDOWS:
			{
				if (ver.dwMajorVersion>=4 && ver.dwMinorVersion>=90)
					text.FormatEx(IDS_SYSWINME,ver.dwMajorVersion,ver.dwMinorVersion,(WORD)ver.dwBuildNumber);
				else if (ver.dwMajorVersion==4 && ver.dwMinorVersion>=10)
					text.FormatEx(IDS_SYSWIN98,ver.dwMajorVersion,ver.dwMinorVersion,(WORD)ver.dwBuildNumber);
				else
					text.FormatEx(IDS_SYSWIN95,ver.dwMajorVersion,ver.dwMinorVersion,(WORD)ver.dwBuildNumber);
				text << ver.szCSDVersion;
				break;
			}
		case VER_PLATFORM_WIN32_NT:
			{
				if (ver.dwMajorVersion==6)
				{
					// Windows Vista products
					OSVERSIONINFOEX osx;
					osx.dwOSVersionInfoSize=sizeof(OSVERSIONINFOEX);
					GetVersionEx((OSVERSIONINFO*)&osx);
					if (osx.wProductType==VER_NT_SERVER)
						text.FormatEx(IDS_SYSWINSERVERLONGHORN,osx.dwMajorVersion,osx.dwMinorVersion,osx.dwBuildNumber);
					else if (ver.dwMinorVersion>=1)
						text.FormatEx(IDS_SYSWIN7,osx.dwMajorVersion,osx.dwMinorVersion,osx.dwBuildNumber);
					else 
						text.FormatEx(IDS_SYSWINVISTA,osx.dwMajorVersion,osx.dwMinorVersion,osx.dwBuildNumber);
					text << ' ' << osx.szCSDVersion;
				}
				else if (ver.dwMajorVersion==5 && ver.dwMinorVersion>=1)
				{
					// Windows XP products
					OSVERSIONINFOEX osx;
					osx.dwOSVersionInfoSize=sizeof(OSVERSIONINFOEX);
					GetVersionEx((OSVERSIONINFO*)&osx);
					if (osx.wProductType==VER_NT_SERVER)
						text.FormatEx(IDS_SYSWIN2003SERVER,osx.dwMajorVersion,osx.dwMinorVersion,osx.dwBuildNumber);
					else
						text.FormatEx(IDS_SYSWINXP,osx.dwMajorVersion,osx.dwMinorVersion,osx.dwBuildNumber);
					text << ' ' << osx.szCSDVersion;
				}
				else if (ver.dwMajorVersion==5)
				{
					// Windows 2000 products
					OSVERSIONINFOEX osx;
					osx.dwOSVersionInfoSize=sizeof(OSVERSIONINFOEX);
					GetVersionEx((OSVERSIONINFO*)&osx);

					if (osx.wProductType==VER_NT_SERVER && osx.wSuiteMask&VER_SUITE_ENTERPRISE)
						text.FormatEx(IDS_SYSWIN2000ENTEPRISE,osx.dwMajorVersion,osx.dwMinorVersion,osx.dwBuildNumber);
					else if (osx.wProductType==VER_NT_SERVER)
						text.FormatEx(IDS_SYSWIN2000SERVER,osx.dwMajorVersion,osx.dwMinorVersion,osx.dwBuildNumber);
					else
						text.FormatEx(IDS_SYSWIN2000WORKSTATION,osx.dwMajorVersion,osx.dwMinorVersion,osx.dwBuildNumber);
					text << ' ' << osx.szCSDVersion;
				}
				else
				{
					text.FormatEx(IDS_SYSWINNT,ver.dwMajorVersion,ver.dwMinorVersion,ver.dwBuildNumber);
					text << ver.szCSDVersion;
				}
				break;
			}
		}
	}
	mem.dwLength=sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&mem);
	text2.FormatEx(IDS_SYSPHYSMEM,mem.dwTotalPhys>>10,mem.dwAvailPhys>>10);
	text<<text2;
	text2.FormatEx(IDS_SYSPAGEDMEM,mem.dwTotalPageFile>>10,mem.dwAvailPageFile>>10);
	text<<text2;
	SetDlgItemText(IDC_SYSINFO,text);
	SetIcon(NULL,TRUE);
	SetIcon(NULL,FALSE);
	return FALSE;
}

void CAboutDlg::OnDrawItem(UINT idCtl,LPDRAWITEMSTRUCT lpdis)
{
	CDialog::OnDrawItem(idCtl,lpdis);
	CDC dc(lpdis->hDC);
	CFont FontRegular,FontUnderline;
	TEXTMETRIC tm;
	char szFace[100];
	dc.GetTextMetrics(&tm);
	dc.GetTextFace(100,szFace);
	
	FontRegular.CreateFont(tm.tmHeight,0,0,0,
		tm.tmWeight,tm.tmItalic,0,tm.tmStruckOut,
		tm.tmCharSet,OUT_CHARACTER_PRECIS,
		CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
		tm.tmPitchAndFamily,szFace);
	
	FontUnderline.CreateFont(tm.tmHeight,0,0,0,
		tm.tmWeight,tm.tmItalic,1,tm.tmStruckOut,
		tm.tmCharSet,OUT_CHARACTER_PRECIS,
		CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
		tm.tmPitchAndFamily,szFace);
	
	HFONT hOldFont=(HFONT)dc.SelectObject(FontRegular);
	
	
	CStringW text;
	GetDlgItemText(idCtl,text);
	
	RECT rc=lpdis->rcItem;

	LPCWSTR pPtr=text;
	while (*pPtr!=L'\0')
	{
		int nLength=FirstCharIndex(pPtr,L'[');
		
		
		// Paint regular part
		dc.SetTextColor(RGB(0,0,0));
		RECT rc2=rc;
		dc.DrawText(pPtr,nLength,&rc2,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_CALCRECT);
		dc.DrawText(pPtr,nLength,&rc2,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
		rc.left=rc2.right;
		
		if (nLength==-1)
			break;

		pPtr+=nLength+1;

		// URL part
		nLength=FirstCharIndex(pPtr,L']');

		
		dc.SetTextColor(RGB(0,0,255));
		dc.SelectObject(FontUnderline);
		rc2=rc;
		dc.DrawText(pPtr,nLength,&rc2,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_CALCRECT);
		dc.DrawText(pPtr,nLength,&rc2,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
		rc.left=rc2.right;

		if (nLength==-1)
			break;

		dc.SelectObject(FontRegular);

		pPtr+=nLength+1;
				
	}
			

	dc.SelectObject(hOldFont);

	FontRegular.DeleteObject();
	FontUnderline.DeleteObject();
}

LRESULT CAboutDlg::WindowProc(UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch (msg)
	{
	case WM_ERASEBKGND:
		{
			CRect rect;
			CBrush br;
			GetClientRect(&rect);

			br.CreateSolidBrush(RGB(252,248,240));
			
			FillRect((HDC)wParam,&CRect(0,68,rect.right,rect.bottom),GetSysColorBrush(COLOR_3DFACE));
			FillRect((HDC)wParam,&CRect(0,0,68,rect.bottom),br);
			return 1;
		}
	}
	return CDialog::WindowProc(msg,wParam,lParam);
}

