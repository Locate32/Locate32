/* Locate32 - Copyright (c) 1997-2010 Janne Huttunen */

#include <HFCLib.h>
#include "Locate32.h"

#include <rpcsal.h>  // Imapi.h requires this
#include <imapi.h>
#include <uxtheme.h>
#include <vssym32.h>

#define BUTTON_WIDTHORIG		18
#define BUTTON_WIDTHTHEME		13

#define IDC_EXPLICITIDATE		100
#define IDC_RELATIVEDATE		101
#define IDC_RELATIVEDATESPIN	102

////////////////////////////////////////////////////////////
// Buffer allocation
////////////////////////////////////////////////////////////

struct BUFFER {
	char* pData;
	DWORD id;
	BUFFER* pNextBuffer;
};
BUFFER* pFirstBuffer=NULL;
BUFFER* pLatestBuffer=NULL;
CRITICAL_SECTION csBuffers;

void InitializeBuffers()
{
	InitializeCriticalSection(&csBuffers);
}

void DeinitializeBuffers()
{
	EnterCriticalSection(&csBuffers);
	
	while (pFirstBuffer!=NULL)
	{
		BUFFER* pTmp=pFirstBuffer;
		pFirstBuffer=pTmp->pNextBuffer;

		if (pTmp->pData!=NULL)
			delete[] pTmp->pData;
		delete pTmp;
	}
	pLatestBuffer=NULL;

	LeaveCriticalSection(&csBuffers);
}

void AssignBuffer(void* pBuffer,DWORD id)
{
	if (id==0)
		id=GetCurrentThreadId();

	EnterCriticalSection(&csBuffers);

	if (pFirstBuffer==NULL)
	{
		pFirstBuffer=pLatestBuffer=new BUFFER;
		pFirstBuffer->pData=(char*)pBuffer;
		pFirstBuffer->id=id;
		pFirstBuffer->pNextBuffer=NULL;
		LeaveCriticalSection(&csBuffers);
		return;
	}

	if (pLatestBuffer->id==id)
	{
		if (pLatestBuffer->pData!=NULL)
		{
			ASSERT(pLatestBuffer->pData!=pBuffer);
			delete[] pLatestBuffer->pData;
		}
		pLatestBuffer->pData=(char*)pBuffer;
		LeaveCriticalSection(&csBuffers);
		return;
	}


	pLatestBuffer=pFirstBuffer;
	for (;;)
	{
		if (pLatestBuffer->id==id)
		{
			if (pLatestBuffer->pData!=NULL)
			{
				ASSERT(pLatestBuffer->pData!=pBuffer);
				delete[] pLatestBuffer->pData;
			}
			pLatestBuffer->pData=(char*)pBuffer;
			LeaveCriticalSection(&csBuffers);
			return;
		}

		if (pLatestBuffer->pNextBuffer==NULL)
		{
			pLatestBuffer=pLatestBuffer->pNextBuffer=new BUFFER;
			pLatestBuffer->pData=(char*)pBuffer;
			pLatestBuffer->id=id;
			pLatestBuffer->pNextBuffer=NULL;
			LeaveCriticalSection(&csBuffers);
			return;
		}

		pLatestBuffer=pLatestBuffer->pNextBuffer;
	}

	LeaveCriticalSection(&csBuffers);
}


void FreeBuffers(DWORD id)
{
	if (pFirstBuffer==NULL)
		return;

	if (id==0)
		id=GetCurrentThreadId();

	EnterCriticalSection(&csBuffers);

	if (pFirstBuffer->id==id)
	{
		BUFFER* pTmp=pFirstBuffer;
		pLatestBuffer=pFirstBuffer=pFirstBuffer->pNextBuffer;
		if (pTmp->pData!=NULL)
			delete[] pTmp->pData;
		delete pTmp;
		LeaveCriticalSection(&csBuffers);
		return;
	}
		
	pLatestBuffer=pFirstBuffer;
	while (pLatestBuffer->pNextBuffer!=NULL)
	{
		BUFFER* pTmp=pLatestBuffer->pNextBuffer;
		if (pTmp->id==id)
		{
			pLatestBuffer->pNextBuffer=pTmp->pNextBuffer;
			if (pTmp->pData!=NULL)
				delete[] pTmp->pData;
			delete pTmp;
			LeaveCriticalSection(&csBuffers);
			return;
		}

		pLatestBuffer=pLatestBuffer->pNextBuffer;
	}

	LeaveCriticalSection(&csBuffers);
}

////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////



BOOL GetIMAPIBurningDevices(CArray<LPWSTR>& aDevicePaths)
{
	CRegKey RegKey;
	if (RegKey.OpenKey(HKCR,"IMAPI.MSDiscMasterObj\\CLSID",CRegKey::defRead)!=ERROR_SUCCESS)
		return FALSE;

	WCHAR szCLSID[50];
	if (RegKey.QueryValue(L"",szCLSID,50)==0)
		return FALSE;

	CLSID clsid;
	if (CLSIDFromString(szCLSID,&clsid)!=NO_ERROR)
		return FALSE;

	
	HRESULT hRes;
	IDiscMaster* pdm;
	hRes=CoCreateInstance(clsid,NULL,CLSCTX_INPROC_SERVER|CLSCTX_LOCAL_SERVER,IID_IDiscMaster,(void**)&pdm);
	if (FAILED(hRes))
		return FALSE;

	hRes=pdm->Open();
	if (FAILED(hRes))
	{
		pdm->Release();
		return FALSE;
	}

	IEnumDiscRecorders* pedr;
	hRes=pdm->EnumDiscRecorders(&pedr);
	if (SUCCEEDED(hRes))
	{
		IDiscRecorder* pdr;
		DWORD dwReturned;
		while ((hRes=pedr->Next(1,&pdr,&dwReturned))==S_OK)
		{
			BSTR bPath;
			hRes=pdr->GetPath(&bPath);
			if (SUCCEEDED(bPath))
			{
				if (bPath[0]=='\\')
				{
					WCHAR szName[MAX_PATH];
					WCHAR szTemp[MAX_PATH]=L"";
					WCHAR drive[]=L" :";
					GetLogicalDriveStringsW(MAX_PATH,szTemp);

					LPWSTR pPtr=szTemp;
					while (*pPtr!='\0')
					{
						*drive=*pPtr;
						if (QueryDosDeviceW(drive, szName,MAX_PATH))
						{
							if (wcscmp(szName,bPath)==0)
								aDevicePaths.Add(alloccopy(pPtr));
						}

						pPtr+=istrlenw(pPtr)+1;
					}
				}
				else
					aDevicePaths.Add(alloccopy(bPath));
			}

			pdr->Release();
		}
		pedr->Release();
	}


	pdm->Close();
	pdm->Release();

	return TRUE;
}


////////////////////////////////////////////////////////////
// CDateTimeCtrlEx
////////////////////////////////////////////////////////////


CDateTimeCtrlEx::CDateTimeCtrlEx(HWND hWnd)
:	CDateTimeCtrl(hWnd),m_hTimePickerWnd(NULL),m_hEditWnd(NULL),
	m_hSpinWnd(NULL),m_hTheme(NULL),m_pDrawThemeBackground(NULL),
	m_bDeleteOnDestroy(FALSE),m_dwFlags(ModeExplicit|Normal|DontSendNotifications)
{
	m_hUxTheme=LoadLibrary("uxtheme.dll");
	if (m_hUxTheme!=NULL)
	{
		m_pDrawThemeBackground=(HRESULT(STDAPICALLTYPE *)(HTHEME,HDC,int,int,const RECT*,const RECT*))
			GetProcAddress(m_hUxTheme,"DrawThemeBackground");


		HTHEME(STDAPICALLTYPE * pOpenThemeData)(HWND,LPCWSTR)=(HTHEME(STDAPICALLTYPE*)(HWND,LPCWSTR))
			GetProcAddress(m_hUxTheme,"OpenThemeData");
		if (pOpenThemeData!=NULL)
		{
			m_hTheme=pOpenThemeData(hWnd,L"COMBOBOX");
		}
	}
}

void CDateTimeCtrlEx::CreateControls()
{
	RECT rcClientRect;
	GetClientRect(&rcClientRect);
	DWORD dwStyle=WS_GROUP | WS_TABSTOP | WS_CHILD;
	if (GetStyle()&WS_DISABLED)
		dwStyle|=WS_DISABLED;

	if (m_hTheme!=NULL && m_pDrawThemeBackground!=NULL)
		rcClientRect.right-=BUTTON_WIDTHTHEME;
	else
		rcClientRect.right-=BUTTON_WIDTHORIG;

	if (m_hTimePickerWnd==NULL)
	{
		m_hTimePickerWnd=CreateWindowEx(WS_EX_NOPARENTNOTIFY|WS_EX_CLIENTEDGE,
			"SysDateTimePick32","",dwStyle|DTS_RIGHTALIGN|WS_VISIBLE,
			rcClientRect.left,rcClientRect.top,rcClientRect.right,rcClientRect.bottom,
			*this,(HMENU)IDC_EXPLICITIDATE,GetInstanceHandle(),NULL);
	}
	if (m_hEditWnd==NULL)
	{
		m_hEditWnd=CreateWindowEx(WS_EX_NOPARENTNOTIFY|WS_EX_CLIENTEDGE|WS_EX_RIGHT,
			"EDIT","",dwStyle|ES_AUTOHSCROLL|ES_NUMBER,
			rcClientRect.left,rcClientRect.top,rcClientRect.right,rcClientRect.bottom,
			*this,(HMENU)IDC_RELATIVEDATE,GetInstanceHandle(),NULL);
	}
	if (m_hSpinWnd==NULL)
	{
		m_hSpinWnd=CreateWindowEx(WS_EX_NOPARENTNOTIFY|WS_EX_CLIENTEDGE|WS_EX_RIGHT,
			"msctls_updown32","",dwStyle|UDS_ALIGNRIGHT|UDS_ARROWKEYS,
			rcClientRect.left,rcClientRect.top,rcClientRect.right,rcClientRect.bottom,
			*this,(HMENU)IDC_RELATIVEDATESPIN,GetInstanceHandle(),NULL);
		::SendMessage(m_hSpinWnd,UDM_SETBUDDY,WPARAM(m_hEditWnd),0);
		::SendMessage(m_hSpinWnd,UDM_SETRANGE,0,UD_MAXVAL);
	}

	
	SetRelativeDate(0,DTXF_NOMODECHANGE);

	m_dwFlags&=~DontSendNotifications;
}


CDateTimeCtrlEx::~CDateTimeCtrlEx()
{
	if (m_hUxTheme!=NULL)
	{
		HRESULT(STDAPICALLTYPE *pCloseThemeData)(HTHEME)=(HRESULT(STDAPICALLTYPE *)(HTHEME))GetProcAddress(m_hUxTheme,"CloseThemeData");
		if (pCloseThemeData!=NULL)
		{
			if (m_hTheme!=NULL)     
				pCloseThemeData(m_hTheme);
		}
		FreeLibrary(m_hUxTheme);
	}
}


void CDateTimeCtrlEx::OnPaint()
{
	CPaintDC dc(this);

	CRect rcRect;
	GetClientRect(&rcRect);
	
	if (m_pDrawThemeBackground!=NULL && m_hTheme!=NULL)
	{
		rcRect.left=rcRect.right-BUTTON_WIDTHTHEME;

		int nState=CBXS_NORMAL;
		if (GetStyle()&WS_DISABLED)
			nState=CBXS_DISABLED;
		else if (m_dwFlags&Hot)
			nState=CBXS_HOT;
		else if (m_dwFlags&Pressed)
			nState=CBXS_PRESSED;
		m_pDrawThemeBackground(m_hTheme,dc,CP_DROPDOWNBUTTON,nState,&rcRect,NULL);
		return;
	}

	rcRect.left=rcRect.right-BUTTON_WIDTHORIG+1;
	rcRect.top++;
	rcRect.right--;
	rcRect.bottom-=2;

	UINT nState=DFCS_SCROLLDOWN;
	if (GetStyle()&WS_DISABLED)
		nState|=DFCS_INACTIVE;
	else if (m_dwFlags&Pressed)
		nState|=DFCS_PUSHED;

	DrawFrameControl(dc,&rcRect,DFC_SCROLL,nState);
}

void CDateTimeCtrlEx::ChangeMode(BOOL bToRelativeMode)
{
	if (!bToRelativeMode)
	{
		if ((m_dwFlags&ModeMask)==ModeExplicit)
			return;

		::ShowWindow(m_hTimePickerWnd,SW_SHOW);
		::ShowWindow(m_hEditWnd,SW_HIDE);
		::ShowWindow(m_hSpinWnd,SW_HIDE);

		::SetFocus(m_hTimePickerWnd);

		m_dwFlags&=~ModeMask;
		m_dwFlags|=ModeExplicit;
	}
	else // mode==Relative
	{
		if ((m_dwFlags&ModeMask)==ModeRelative)
			return;

		::ShowWindow(m_hTimePickerWnd,SW_HIDE);
		::ShowWindow(m_hEditWnd,SW_SHOW);
		::ShowWindow(m_hSpinWnd,SW_SHOW);

		::SetFocus(m_hEditWnd);

		m_dwFlags&=~ModeMask;
		m_dwFlags|=ModeRelative;
	}
}

	
LRESULT CALLBACK CDateTimeCtrlEx::WndProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	CDateTimeCtrlEx* pData;
	if (msg==WM_CREATE)
	{
		CREATESTRUCT* pCreateStruct=(CREATESTRUCT*)lParam;
		if (pCreateStruct->lpCreateParams==NULL)
		{
			pData=new CDateTimeCtrlEx(hWnd);
			if (pData==NULL)
			{
				SetHFCError(HFC_CANNOTALLOC);
				return FALSE;
			}
		}
		else
		{
			pData=(CDateTimeCtrlEx*)pCreateStruct->lpCreateParams;
			pData->Attach(hWnd);
		}
		
		::SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG_PTR)pData);

		pData->CreateControls();		
		return FALSE;
	}

	pData=(CDateTimeCtrlEx*)::GetWindowLongPtr(hWnd,GWLP_USERDATA);	
	if (pData==NULL)
		return DefWindowProc(hWnd,msg,wParam,lParam);


	switch (msg)
	{
	case WM_DESTROY:
		delete pData;
		::SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG_PTR)NULL);
		return 0;
	case WM_ENABLE:
		::EnableWindow(pData->m_hTimePickerWnd,(BOOL)wParam);
		::EnableWindow(pData->m_hEditWnd,(BOOL)wParam);
		::EnableWindow(pData->m_hSpinWnd,(BOOL)wParam);
		pData->Invalidate();
		break;	
	case DTM_GETSYSTEMTIME:
		return pData->GetExplicitDate((LPSYSTEMTIME)lParam,(DWORD)wParam);
	case DTM_SETSYSTEMTIME:
		return pData->SetExplicitDate((LPSYSTEMTIME)lParam,(DWORD)wParam);
	case DTM_GETRANGE:
	case DTM_SETRANGE:
	case DTM_SETFORMATA:
	case DTM_SETFORMATW:
	case DTM_SETMCCOLOR:
	case DTM_GETMCCOLOR:
	case DTM_GETMONTHCAL:
	case DTM_SETMCFONT:
	case DTM_GETMCFONT:
		return ::SendMessage(pData->m_hTimePickerWnd,msg,wParam,lParam);
	case WM_PAINT:
		pData->OnPaint();
		break;
	case WM_MOUSEMOVE:
		if ((pData->m_dwFlags&ButtonStateMask)==Normal)
		{
			pData->m_dwFlags&=~ButtonStateMask;
			pData->m_dwFlags|=Hot;
			
			pData->Invalidate();
			pData->SetCapture();
		}
		else if ((pData->m_dwFlags&ButtonStateMask)==Hot)
		{
			CRect rcButtonArea;
			pData->GetClientRect(&rcButtonArea);
			if (pData->m_hTheme!=NULL && pData->m_pDrawThemeBackground!=NULL)
				rcButtonArea.left=rcButtonArea.right-BUTTON_WIDTHTHEME;
			else
				rcButtonArea.left=rcButtonArea.right-BUTTON_WIDTHORIG;


			if (!rcButtonArea.IsPtInRect(LOWORD(lParam),HIWORD(lParam)))
			{
				ReleaseCapture();
				pData->m_dwFlags&=~ButtonStateMask;
				pData->m_dwFlags|=Normal;
				pData->Invalidate();
			}
		}		
		return 0;
	case WM_LBUTTONDOWN:
		{
			if ((pData->m_dwFlags&ButtonStateMask)==Hot)
				ReleaseCapture();
			pData->m_dwFlags&=~ButtonStateMask;
			pData->m_dwFlags|=Pressed;
			pData->Invalidate();
	
			RECT rcWindowRect;
			::GetWindowRect(hWnd,&rcWindowRect);
			
			CMenu PopupMenus;
			PopupMenus.LoadMenu(IDR_POPUPMENU);
			
			if ((pData->m_dwFlags&ModeMask)==ModeExplicit)
				PopupMenus.CheckMenuItem(IDM_EXPLICITDATE,MF_CHECKED|MF_BYCOMMAND);
			else
				PopupMenus.CheckMenuItem(IDM_RELATIVEDATE,MF_CHECKED|MF_BYCOMMAND);


			int nCmd=TrackPopupMenu(PopupMenus.GetSubMenu(SUBMENU_DATETIMEPICKEXMENU),
				TPM_RIGHTALIGN|TPM_TOPALIGN|TPM_RIGHTBUTTON|TPM_NONOTIFY|TPM_RETURNCMD,
				rcWindowRect.right,rcWindowRect.bottom,0,hWnd,NULL);
			PopupMenus.DestroyMenu();

			switch (nCmd)
			{
			case IDM_EXPLICITDATE:
				pData->ChangeMode(FALSE);
				break;
			case IDM_RELATIVEDATE:
				pData->ChangeMode(TRUE);
				break;
			}	

			if (!(pData->m_dwFlags&DontSendNotifications))
				::PostMessage(::GetParent(hWnd),WM_COMMAND,MAKEWPARAM(::GetWindowLong(hWnd,GWL_ID),DTXN_MODECHANGED),(LPARAM)hWnd);

			pData->m_dwFlags&=~ButtonStateMask;
			pData->m_dwFlags|=Normal;
			pData->Invalidate();
			
			return 0;
		}
		
	case WM_LBUTTONUP:
		if ((pData->m_dwFlags&ButtonStateMask)==Pressed)
		{
			ReleaseCapture();
			pData->m_dwFlags&=~ButtonStateMask;
			pData->m_dwFlags|=Normal;
			pData->Invalidate();
		}
		return 0;
	case WM_SETFONT:
		if (pData!=NULL)
		{
			::SendMessage(pData->m_hTimePickerWnd,WM_SETFONT,wParam,lParam);
			::SendMessage(pData->m_hEditWnd,WM_SETFONT,wParam,lParam);
			::SendMessage(pData->m_hSpinWnd,WM_SETFONT,wParam,lParam);
		}
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_RELATIVEDATE:
			switch (HIWORD(wParam))
			{
			case EN_KILLFOCUS:
				pData->SetRelativeDate((int)::SendMessage(pData->m_hSpinWnd,UDM_GETPOS32,0,0),
					DTXF_NOSPINCHANGE|DTXF_NOMODECHANGE);
				break;
			case EN_CHANGE:
				if (!(pData->m_dwFlags&SpinBoxIsUpdating))
				{
					int nVal=0;
					int nLen=(int)::SendMessage(pData->m_hEditWnd,WM_GETTEXTLENGTH,0,0);
						
					if (nLen>0)
					{
						if (IsUnicodeSystem())
						{
							WCHAR* pText=new WCHAR[nLen+1];
							::SendMessageW(pData->m_hEditWnd,WM_GETTEXT,nLen+1,(LPARAM)pText);
							nVal=GetValueFromText(pText);
							delete[] pText;
						}
						else
						{
							char* pText=new char[nLen+1];
							::SendMessage(pData->m_hEditWnd,WM_GETTEXT,nLen+1,(LPARAM)pText);
							nVal=GetValueFromText(A2W(pText));
							delete[] pText;
						}
						

					}
					::SendMessage(pData->m_hSpinWnd,UDM_SETPOS32,0,nVal);

					if (!(pData->m_dwFlags&DontSendNotifications))
						::SendMessage(::GetParent(hWnd),WM_COMMAND,MAKEWPARAM(::GetWindowLong(hWnd,GWL_ID),DTXN_CHANGE),(LPARAM)hWnd);
				}
				break;
			}
			break;
		}
		break;
	case WM_NOTIFY:
		if (((LPNMHDR)lParam)->idFrom==IDC_RELATIVEDATESPIN && 
			((LPNMHDR)lParam)->code==UDN_DELTAPOS)
		{
			LPNMUPDOWN pUD=(LPNMUPDOWN)lParam;
			int nNewPos=pUD->iPos+pUD->iDelta;
			
			if (nNewPos<0)
			{
				pUD->iDelta=-pUD->iPos;
				nNewPos=0;
			}

			pData->m_dwFlags|=SpinBoxIsUpdating;

			//::SendMessage(pData->m_hEditWnd,WM_SETTEXT,0,(LPARAM)szEmpty);
			pData->SetRelativeDate(nNewPos,DTXF_NOSPINCHANGE|DTXF_NOMODECHANGE);			

			
			pData->m_dwFlags&=~SpinBoxIsUpdating;

			if (!(pData->m_dwFlags&DontSendNotifications))
				::SendMessage(::GetParent(hWnd),WM_COMMAND,MAKEWPARAM(::GetWindowLong(hWnd,GWL_ID),DTXN_CHANGE),(LPARAM)hWnd);

		}
		else if (((LPNMHDR)lParam)->idFrom==IDC_EXPLICITIDATE && 
			((LPNMHDR)lParam)->code==DTN_DATETIMECHANGE)
		{
			if (!(pData->m_dwFlags&DontSendNotifications))
				::SendMessage(::GetParent(hWnd),WM_COMMAND,MAKEWPARAM(::GetWindowLong(hWnd,GWL_ID),DTXN_CHANGE),(LPARAM)hWnd);
		}
		break;
	case DTXM_SETRELDATE:
		// wParam is new pos, lparam contains flags (DTXF_*)
		pData->SetRelativeDate((int)wParam,(DWORD)lParam);
		break;
	case DTXM_GETRELDATE:
		return pData->GetRelativeDate();
	case DTXM_GETCLASS:
		return (LRESULT)pData;
	case DTXM_CHANGEMODE:
		pData->ChangeMode((BOOL)wParam);
		break;
	case DTXM_GETMODE:
		return pData->GetMode();
	case WM_SETFOCUS:
		if ((pData->m_dwFlags&ModeMask)==ModeExplicit)
			::SetFocus(pData->m_hTimePickerWnd);
		else
		{
			::SetFocus(pData->m_hEditWnd);
			::SendMessage(pData->m_hEditWnd,EM_SETSEL,0,-1);
		}

		return 0;
	}
	return DefWindowProc(hWnd,msg,wParam,lParam);
}

	
int CDateTimeCtrlEx::SetExplicitDate(LPSYSTEMTIME pSystemTime,DWORD dwFlags)
{
	m_dwFlags|=DontSendNotifications;
	if (pSystemTime->wYear==0xFFFF && pSystemTime->wMonth==0xFFFF)
	{
		// Relative
		if (!(dwFlags&DTXF_NOMODECHANGE))
			ChangeMode(TRUE);
		SetRelativeDate(pSystemTime->wDay,dwFlags);
		
		m_dwFlags&=~DontSendNotifications;
		return 1;
	}
	else
	{
		if (!(dwFlags&DTXF_NOMODECHANGE))
			ChangeMode(FALSE);
		m_dwFlags&=~DontSendNotifications;
		return (int)::SendMessage(m_hTimePickerWnd,DTM_SETSYSTEMTIME,dwFlags&~DTXF_MSGMASK,(LPARAM)pSystemTime);
	}
}
	
int CDateTimeCtrlEx::GetExplicitDate(LPSYSTEMTIME pSystemTime,DWORD dwFlags) const
{
	if ((m_dwFlags&ModeMask)==ModeExplicit)
	{
		dwFlags&=~DTXF_FORSAVE;
		return (int)::SendMessage(m_hTimePickerWnd,DTM_GETSYSTEMTIME,dwFlags,(LPARAM)pSystemTime);
	}
	else if (dwFlags&DTXF_FORSAVE)
	{
		pSystemTime->wYear=0xFFFF;
		pSystemTime->wMonth=0xFFFF;
		pSystemTime->wDay=GetRelativeDate();
		return GDT_VALID|DTXF_FORSAVE;
	}
	else
	{
		GetLocalTime(pSystemTime);
		CTime::DecreaseDaysInSystemTime(pSystemTime,GetRelativeDate());
		return GDT_VALID;
	}
}

int CDateTimeCtrlEx::GetRelativeDate() const
{
	if ((m_dwFlags&ModeMask)==ModeExplicit)
		return -1;

	return (int)::SendMessage(m_hSpinWnd,UDM_GETPOS32,0,0);
}

void CDateTimeCtrlEx::SetRelativeDate(int nNewPos,DWORD dwFlags)
{
	m_dwFlags|=DontSendNotifications;
	
	if (!(dwFlags&DTXF_NOMODECHANGE))
		ChangeMode(TRUE);

	WCHAR szStringBuffer[100];
	int nLen=-1;
	if (nNewPos==0)
		LoadString(IDS_DATETODAY,szStringBuffer,100);
	else if (nNewPos==1)
		LoadString(IDS_DATEDAY,szStringBuffer,100);
	else
	{
		if (_itow_s(nNewPos,szStringBuffer,100,10)!=0)
			szStringBuffer[0]='\0';
		nLen=istrlen(szStringBuffer);
		
		if (nLen+1<100)
		{
			szStringBuffer[nLen]=L' ';
			LoadString(IDS_DATENDAYS,szStringBuffer+nLen+1,100-nLen-1);
		}
	}

	if (IsUnicodeSystem())
		::SendMessageW(m_hEditWnd,WM_SETTEXT,0,(LPARAM)szStringBuffer);
	else
		::SendMessage(m_hEditWnd,WM_SETTEXT,0,(LPARAM)(LPCSTR)W2A(szStringBuffer));

	//::SendMessage(m_hEditWnd,EM_SETSEL,0,nLen);

	if (!(dwFlags&DTXF_NOSPINCHANGE))
		::SendMessage(m_hSpinWnd,UDM_SETPOS32,0,nNewPos);

	m_dwFlags&=~DontSendNotifications;
}

BOOL RegisterDataTimeExCltr()
{
	WNDCLASSEX wc;
	ZeroMemory(&wc,sizeof(WNDCLASSEX));
	wc.cbSize=sizeof(WNDCLASSEX);
	wc.style=CS_DBLCLKS|CS_VREDRAW|CS_HREDRAW|CS_PARENTDC;
	wc.hInstance=GetInstanceHandle();
	wc.lpszClassName="DATETIMEPICKEX";
	wc.hbrBackground=(HBRUSH)(COLOR_BTNFACE+1);
	wc.lpfnWndProc=CDateTimeCtrlEx::WndProc;
	if (RegisterClassEx(&wc)==NULL)
		return FALSE;

	ZeroMemory(&wc,sizeof(WNDCLASSEX));
	wc.cbSize=sizeof(WNDCLASSEX);
	wc.style=CS_DBLCLKS|CS_VREDRAW|CS_HREDRAW|CS_PARENTDC;
	wc.hInstance=GetResourceHandle(LanguageSpecificResource);
	wc.lpszClassName="DATETIMEPICKEX";
	wc.hbrBackground=(HBRUSH)(COLOR_BTNFACE+1);
	wc.lpfnWndProc=CDateTimeCtrlEx::WndProc;
	return RegisterClassEx(&wc)!=NULL;
}



////////////////////////////////////////////////////////////
// CComboBoxAutoComplete
////////////////////////////////////////////////////////////

CComboBoxAutoComplete::CComboBoxAutoComplete()
:	CComboBox(),m_pACData(NULL)
{
}	

CComboBoxAutoComplete::CComboBoxAutoComplete(HWND hWnd)
:	CComboBox(hWnd),m_pACData(NULL)
{
}

CComboBoxAutoComplete::~CComboBoxAutoComplete()
{
	if (m_pACData!=NULL)
		delete m_pACData;
}

void CComboBoxAutoComplete::EnableAutoComplete(BOOL bEnable)
{
	// Do not work with ownerdraw items
	ASSERT(!(GetStyle()&(CBS_OWNERDRAWFIXED|CBS_OWNERDRAWVARIABLE)))

	if (bEnable)
	{
		if (m_pACData!=NULL)
			return;

		// Initializing structure
		m_pACData=new ACDATA;
		m_pACData->bFlags=0;

		// Probing current items
		int nCount=CComboBox::GetCount();
		for (int i=0;i<nCount;i++)
		{
			int nLength=(int)CComboBox::GetLBTextLen(i);
			WCHAR* pText=new WCHAR[max(nLength+1,2)];
			CComboBox::GetLBText(i,pText);
			m_pACData->aItems.Add(pText);		
		}
	}
	else if (m_pACData!=NULL)
	{
		if (m_pACData->bFlags&ACDATA::afAutoCompleting)
		{
			CComboBox::ResetContent();
			for (int i=0;i<m_pACData->aItems.GetSize();i++)
				CComboBox::AddString(m_pACData->aItems[i]);
		}

		delete m_pACData;
		m_pACData=NULL;
	}
}

BOOL CComboBoxAutoComplete::IsAutoCompleteEnabled() const
{
	return m_pACData!=NULL;
}

int CComboBoxAutoComplete::GetCount() const
{
	if (m_pACData==NULL)
		return CComboBox::GetCount();

	return m_pACData->aItems.GetSize();
}

int CComboBoxAutoComplete::GetCurSel() const
{
	if (m_pACData!=NULL)
	{
		if (m_pACData->bFlags&ACDATA::afAutoCompleting)
		{
			int nCurSel=CComboBox::GetCurSel();
			if (nCurSel==CB_ERR || nCurSel>=m_pACData->aItemsInList.GetSize())
				return CB_ERR;
			return m_pACData->aItemsInList[nCurSel];
		}
	}	
	return CComboBox::GetCurSel();
}

int CComboBoxAutoComplete::SetCurSel(int nSelect)
{
	if (m_pACData!=NULL)
	{
		if (m_pACData->bFlags&ACDATA::afAutoCompleting)
		{
			int nIndex=m_pACData->aItemsInList.Find(nSelect);
			if (nIndex==-1)
				return CB_ERR;
			return CComboBox::SetCurSel(nIndex);
		}	
	}
		
	return CComboBox::SetCurSel(nSelect);
}

int CComboBoxAutoComplete::GetTopIndex() const
{
	if (m_pACData!=NULL)
	{
		if (m_pACData->bFlags&ACDATA::afAutoCompleting)
		{
			int nTopIndex=CComboBox::GetTopIndex();
			if (nTopIndex==CB_ERR || nTopIndex>=m_pACData->aItemsInList.GetSize())
				return CB_ERR;
			return m_pACData->aItemsInList[nTopIndex];
		}
	}			
	return CComboBox::GetTopIndex();
}

int CComboBoxAutoComplete::SetTopIndex(int nSelect)
{
	if (m_pACData!=NULL)
	{
		if (m_pACData->bFlags&ACDATA::afAutoCompleting)
		{
			int nIndex=m_pACData->aItemsInList.Find(nSelect);
			if (nIndex==-1)
				return CB_ERR;
			return CComboBox::SetTopIndex(nIndex);
		}	
	}
		
	return CComboBox::SetTopIndex(nSelect);
}

/*

int CComboBoxAutoComplete::GetLBText(int nIndex, LPSTR lpszText) const
{
	if (m_pACData==NULL)
		return CComboBox::GetLBText(nIndex, lpszText);

	if (nIndex>=m_pACData->aItems.GetSize())
		return CB_ERR;
	
	DWORD nLength=istrlenw(m_pACData->aItems[nIndex]);
	MemCopyWtoA(lpszText,m_pACData->aItems[nIndex],nLength+1);
	return nLength;
}

int CComboBoxAutoComplete::GetLBText(int nIndex, CStringA& rString) const
{
	if (m_pACData==NULL)
		return CComboBox::GetLBText(nIndex,rString);

	if (nIndex>=m_pACData->aItems.GetSize())
		return CB_ERR;
	
	rString.Copy(m_pACData->aItems[nIndex]);
	return rString.GetLength();
}*/

#ifdef DEF_WCHAR
int CComboBoxAutoComplete::GetLBText(int nIndex, LPWSTR lpszText) const
{
	if (m_pACData==NULL)
		return CComboBox::GetLBText(nIndex, lpszText);

	if (nIndex>=m_pACData->aItems.GetSize())
		return CB_ERR;
	
	DWORD nLength=istrlenw(m_pACData->aItems[nIndex]);
	MemCopy(lpszText,m_pACData->aItems[nIndex],nLength+1);
	return nLength;
}

int CComboBoxAutoComplete::GetLBText(int nIndex, CStringW& rString) const
{
	if (m_pACData==NULL)
		return CComboBox::GetLBText(nIndex,rString);

	if (nIndex>=m_pACData->aItems.GetSize())
		return CB_ERR;
	
	rString.Copy(m_pACData->aItems[nIndex]);
	return rString.GetLength();
}
#endif

int CComboBoxAutoComplete::GetLBTextLen(int nIndex) const
{
	if (m_pACData==NULL)
		return CComboBox::GetLBTextLen(nIndex);

	if (nIndex>=m_pACData->aItems.GetSize())
		return CB_ERR;
	
	return istrlenw(m_pACData->aItems[nIndex]);
}



int CComboBoxAutoComplete::FindStringExact(int nIndex, LPCSTR lpszFind) const
{
	if (m_pACData==NULL)
		return CComboBox::FindStringExact(nIndex, lpszFind);

	for (;nIndex<m_pACData->aItems.GetSize();nIndex++)
	{
		if (wcscmp(m_pACData->aItems[nIndex],A2W(lpszFind))==0)
			return nIndex;
	}
	return -1;
}

#ifdef DEF_WCHAR
int CComboBoxAutoComplete::FindStringExact(int nIndex, LPCWSTR lpszFind) const
{
	if (m_pACData==NULL)
		return CComboBox::FindStringExact(nIndex, lpszFind);

	for (;nIndex<m_pACData->aItems.GetSize();nIndex++)
	{
		if (wcscmp(m_pACData->aItems[nIndex],lpszFind)==0)
			return nIndex;
	}
	return -1;
}
#endif

BOOL CComboBoxAutoComplete::GetDroppedState() const
{
	if (m_pACData!=NULL)
	{
		if (m_pACData->bFlags&ACDATA::afAutoCompleting)
			return FALSE;
	}
	return CComboBox::GetDroppedState();
}


void CComboBoxAutoComplete::ShowDropDown(BOOL bShowIt)
{
	if (m_pACData!=NULL)
	{
		if (m_pACData->bFlags&ACDATA::afAutoCompleting)
		{
			// TODO: You know
		
		}
	}
	return CComboBox::ShowDropDown(bShowIt);
}


int CComboBoxAutoComplete::AddString(LPCSTR lpszString)
{
	if (m_pACData==NULL)
		return CComboBox::AddString(lpszString);

	return 0;
}

int CComboBoxAutoComplete::DeleteString(UINT nIndex)
{
	if (m_pACData==NULL)
		return CComboBox::DeleteString(nIndex);

	return 0;
}

int CComboBoxAutoComplete::InsertString(int nIndex, LPCSTR lpszString)
{
	if (m_pACData==NULL)
		return CComboBox::InsertString(nIndex, lpszString);

	return 0;
}

void CComboBoxAutoComplete::ResetContent()
{
	if (m_pACData==NULL)
		return CComboBox::ResetContent();

}


int CComboBoxAutoComplete::FindString(int nStartAfter,LPCSTR lpszString) const
{
	if (m_pACData==NULL)
		return CComboBox::FindString(nStartAfter,lpszString);

	return 0;
}

int CComboBoxAutoComplete::SelectString(int nStartAfter,LPCSTR lpszString)
{
	if (m_pACData==NULL)
		return CComboBox::SelectString(nStartAfter,lpszString);

	return 0;
}


BOOL CComboBoxAutoComplete::HandleOnCommand(WORD wNotifyCode)
{
	if (m_pACData==NULL)
		return FALSE;

	return FALSE;
}


#ifdef DEF_WCHAR


int CComboBoxAutoComplete::AddString(LPCWSTR lpszString)
{
	if (m_pACData==NULL)
		return CComboBox::AddString(lpszString);

	return 0;
}

int CComboBoxAutoComplete::InsertString(int nIndex, LPCWSTR lpszString)
{
	if (m_pACData==NULL)
		return CComboBox::InsertString(nIndex, lpszString);

	return 0;
}

int CComboBoxAutoComplete::FindString(int nStartAfter,LPCWSTR lpszString) const
{
	if (m_pACData==NULL)
		return CComboBox::FindString(nStartAfter,lpszString);

	return 0;
}

int CComboBoxAutoComplete::SelectString(int nStartAfter,LPCWSTR lpszString)
{
	if (m_pACData==NULL)
		return CComboBox::SelectString(nStartAfter,lpszString);

	return 0;
}

#endif



////////////////////////////////////////////////////////////
// CRegKey2
////////////////////////////////////////////////////////////




LONG CRegKey2::OpenKey(HKEY hKey,LPCSTR lpszSubKey,DWORD fStatus,LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
	extern CLocateApp theApp;
	
	int nCommonKeyLen=istrlen(theApp.m_szCommonRegKey);
	int nKeyLen=istrlen(lpszSubKey)+1;
	
	char* pNewKey=new char[nCommonKeyLen+nKeyLen+(lpszSubKey[0]!='\\')+9];
	
	MemCopy(pNewKey,theApp.m_szCommonRegKey,nCommonKeyLen);
	
	MemCopy(pNewKey+nCommonKeyLen,"\\Locate32",9);
	nCommonKeyLen+=9;

	if (lpszSubKey[0]!='\\')
		pNewKey[nCommonKeyLen++]='\\';
	
	MemCopy(pNewKey+nCommonKeyLen,lpszSubKey,nKeyLen);
	
	LONG lRet=CRegKey::OpenKey(hKey,pNewKey,fStatus,lpSecurityAttributes);

	delete[] pNewKey;

	return lRet;
}


LONG CRegKey2::OpenKey(HKEY hKey,LPCWSTR lpszSubKey,DWORD fStatus,LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
	extern CLocateApp theApp;
	
	int nCommonKeyLen=istrlen(theApp.m_szCommonRegKey);
	int nKeyLen=istrlenw(lpszSubKey)+1;
	
	WCHAR* pNewKey=new WCHAR[nCommonKeyLen+nKeyLen+(lpszSubKey[0]!='\\')];
	MultiByteToWideChar(CP_ACP,0,theApp.m_szCommonRegKey,nCommonKeyLen,pNewKey,nCommonKeyLen+nKeyLen);
	
	MemCopyW(pNewKey+nCommonKeyLen,L"\\Locate32",9);
	nCommonKeyLen+=9;

	if (lpszSubKey[0]!=L'\\')
		pNewKey[nCommonKeyLen++]=L'\\';
	
	MemCopy(pNewKey+nCommonKeyLen,lpszSubKey,nKeyLen);
	
	LONG lRet=CRegKey::OpenKey(hKey,pNewKey,fStatus,lpSecurityAttributes);

	delete[] pNewKey;

	return lRet;
}

LONG CRegKey2::DeleteCommonKey(LPCSTR lpszSubKey)
{
	extern CLocateApp theApp;
	
	int nCommonKeyLen=istrlen(theApp.m_szCommonRegKey);
	int nKeyLen=istrlen(lpszSubKey)+1;
	
	char* pNewKey=new char[nCommonKeyLen+nKeyLen+(lpszSubKey[0]!='\\')+9];
	
	MemCopy(pNewKey,theApp.m_szCommonRegKey,nCommonKeyLen);
	
	MemCopy(pNewKey+nCommonKeyLen,"\\Locate32",9);
	nCommonKeyLen+=9;

	if (lpszSubKey[0]!='\\')
		pNewKey[nCommonKeyLen++]='\\';
	
	MemCopy(pNewKey+nCommonKeyLen,lpszSubKey,nKeyLen);
	
	LONG lRet=CRegKey::DeleteKey(HKCU,pNewKey);

	delete[] pNewKey;

	return lRet;
}

LONG CRegKey2::DeleteCommonKey(LPCWSTR lpszSubKey)
{
	extern CLocateApp theApp;
	
	int nCommonKeyLen=istrlen(theApp.m_szCommonRegKey);
	int nKeyLen=istrlenw(lpszSubKey)+1;
	
	WCHAR* pNewKey=new WCHAR[nCommonKeyLen+nKeyLen+(lpszSubKey[0]!='\\')];
	MultiByteToWideChar(CP_ACP,0,theApp.m_szCommonRegKey,nCommonKeyLen,pNewKey,nCommonKeyLen+nKeyLen);
	
	MemCopyW(pNewKey+nCommonKeyLen,L"\\Locate32",9);
	nCommonKeyLen+=9;

	if (lpszSubKey[0]!=L'\\')
		pNewKey[nCommonKeyLen++]=L'\\';
	
	MemCopy(pNewKey+nCommonKeyLen,lpszSubKey,nKeyLen);
	
	LONG lRet=CRegKey::DeleteKey(HKCU,pNewKey);

	delete[] pNewKey;

	return lRet;
}


int STDAPICALLTYPE _StrCmpW(LPCWSTR a,LPCWSTR b)
{
	return wcscmp(a,b);
}

