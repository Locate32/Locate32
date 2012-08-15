////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////

#include "HFCLib.h"



#if defined(WIN32) && !defined (_CONSOLE)

namespace HFCControls {
	LRESULT CALLBACK Ctrl3DStaticProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	LRESULT CALLBACK Ctrl3DButtonProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	LRESULT CALLBACK HexViewControlProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	BOOL StaticPaint(HDC hDC,HWND hWnd,void* ctrl);
}

typedef struct _SCTRL
{
	BYTE* szText;
	HFONT hFont;
	BYTE* szBuffer;
	DWORD nBufferLen;
	DWORD nAllocLen;
} SCTRL,*LPSCTRL;

typedef struct _BCTRL
{
	LPSTR szText;
	HFONT hFont;
	BYTE nPressed;
	BYTE nMouseOver;
	BYTE nOtherDef;
	HBITMAP hBitmap;
	HBITMAP hDisBitmap;
	HBITMAP hHotBitmap;
	COLORREF* pColors;
} BCTRL,*LPBCTRL;

#define HEXVIEWCTRL_DEFALLOCEXTRA		10000
#define HEXVIEWCTRl_DEFBYTESPERLINE		12

enum Errors {
		noError=0,
		noMoreLines=1,
		errUnknown=2
	};

#define HEXVIEWCTRl_FLAGSCROLLTOEND		0x1
#define HEXVIEWCTRl_FLAGERASEBGK		0x2

typedef struct _HEXVIEWCTRL
{
	HFONT hFont;
	SIZE_T nLines;
	int nLine;

	BYTE* pData;
	SIZE_T nDataLen;
	SIZE_T nAllocLen;
	SIZE_T nAllocExtra;
	BYTE nBytesPerLine;
	BYTE nHalfLine;
	HBRUSH hbrBackground;
	BYTE bFlags;
} HEXVIEWCTRL,*LPHEXVIEWCTRL;

LRESULT CALLBACK HFCControls::Ctrl3DButtonProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	LPBCTRL ctrl;
	if (msg==WM_CREATE)
	{
		ctrl=new BCTRL;
		if (ctrl==NULL)
		{
			SetHFCError(HFC_CANNOTALLOC);
			return FALSE;
		}
		SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG_PTR)ctrl);
		LPCSTR temp=(LPCSTR)((CREATESTRUCT*)lParam)->lpszName;
		int templen=istrlen(temp);
		ctrl->szText=new char[templen+2];
		if (ctrl->szText==NULL)
		{
			SetHFCError(HFC_CANNOTALLOC);
			delete ctrl;
			return FALSE;
		}
		MemCopy(ctrl->szText,temp,templen+1);
		ctrl->hFont=(HFONT)GetStockObject(DEFAULT_GUI_FONT);
		ctrl->nPressed=0;
		ctrl->nMouseOver=FALSE;
		ctrl->hBitmap=NULL;
		ctrl->hDisBitmap=NULL;
		ctrl->hHotBitmap=NULL;
		ctrl->nOtherDef=FALSE;
		ctrl->pColors=new COLORREF[6];
		if (ctrl->pColors==NULL)
		{
			SetHFCError(HFC_CANNOTALLOC);
			delete ctrl;
			return FALSE;
		}
		ctrl->pColors[0]=RGB(128,128,128);
		ctrl->pColors[1]=RGB(255,255,255);
		ctrl->pColors[2]=GetSysColor(COLOR_BTNTEXT);
		ctrl->pColors[3]=GetSysColor(COLOR_3DLIGHT);
		ctrl->pColors[4]=RGB(64,64,64);
		ctrl->pColors[5]=GetSysColor(COLOR_3DHILIGHT);
		if (((LPCREATESTRUCT)lParam)->style&BS_DEFPUSHBUTTON)
			SendMessage(GetParent(hWnd),DM_SETDEFID,(WPARAM)((LPCREATESTRUCT)lParam)->hMenu,0);
		return FALSE;
	}
	ctrl=(LPBCTRL)GetWindowLongPtr(hWnd,GWLP_USERDATA);	
	switch(msg)
	{
	case WM_USER+8:
		{
			HWND hFocus=GetFocus();
			if (hFocus!=hWnd)
			{
				if (SendMessage(hFocus,WM_GETDLGCODE,0,0)&DLGC_BUTTON)
				{
					if (!ctrl->nOtherDef)
					{
						ctrl->nOtherDef=TRUE;
						InvalidateRect(hWnd,NULL,TRUE);
					}
				}
				else
				{
					if (ctrl->nOtherDef)
					{
						ctrl->nOtherDef=FALSE;
						InvalidateRect(hWnd,NULL,TRUE);
					}
				}
			}
		}
		break;
	case WM_WININICHANGE:
	case WM_ENABLE:
		InvalidateRect(hWnd,NULL,TRUE);
		break;
	case WM_GETDLGCODE:
		if (GetWindowLong(hWnd,GWL_STYLE)&BS_DEFPUSHBUTTON)
			PostMessage(hWnd,WM_USER+8,0,0);
		return DLGC_BUTTON;
	case WM_MOUSEMOVE:
		if (!ctrl->nMouseOver && !ctrl->nPressed && !(wParam&MK_LBUTTON))
		{	
			ctrl->nMouseOver=TRUE;
			SetTimer(hWnd,1,10,NULL);
			SendMessage(GetParent(hWnd),WM_COMMAND,MAKEWPARAM(GetDlgCtrlID(hWnd),B3DN_MOUSEOVER),(LPARAM)hWnd);
			InvalidateRect(hWnd,NULL,FALSE);
		}
		else if (wParam&MK_LBUTTON)
		{
			RECT rect;
			GetClientRect(hWnd,&rect);
			POINT pt={(long)(short)LOWORD(lParam),(long)(short)HIWORD(lParam)};
			if (pt.x>=rect.left && pt.x<=rect.right &&
				pt.y>=rect.top && pt.y<=rect.bottom)
			{
				if (!(ctrl->nPressed&1))
				{
					ctrl->nPressed|=1;
					if (!(ctrl->nPressed&2))
					{
						InvalidateRect(hWnd,NULL,TRUE);
						SendMessage(GetParent(hWnd),WM_COMMAND,MAKEWPARAM(GetDlgCtrlID(hWnd),B3DN_HILITE),(LPARAM)hWnd);
					}
				}
			}
			else
			{
				if (ctrl->nPressed&1)
				{
					ctrl->nPressed&=~1;
					if (!ctrl->nPressed)
					{
						InvalidateRect(hWnd,NULL,TRUE);
						SendMessage(GetParent(hWnd),WM_COMMAND,MAKEWPARAM(GetDlgCtrlID(hWnd),B3DN_UNHILITE),(LPARAM)hWnd);
					}
				}
			}
		}
		break;
	case WM_TIMER:
		switch (wParam)
		{
		case 1:
			{
				POINT pt;
				RECT rect;
				GetCursorPos(&pt);
				GetWindowRect(hWnd,&rect);
				if (pt.x<rect.left || pt.x>rect.right ||
					pt.y<rect.top || pt.y>rect.bottom)
				{
					KillTimer(hWnd,1);
					ctrl->nMouseOver=FALSE;
					SendMessage(GetParent(hWnd),WM_COMMAND,MAKEWPARAM(GetDlgCtrlID(hWnd),B3DN_MOUSEOUT),(LPARAM)hWnd);
					if (!ctrl->nPressed)
						InvalidateRect(hWnd,NULL,TRUE);
				}
			}
			break;
		}
		break;
	case B3DM_SETSTYLE:
		{
			DWORD nStyle=(DWORD)GetWindowLong(hWnd,GWL_STYLE);
			nStyle&=~(BS_PUSHBUTTON|BS_DEFPUSHBUTTON);
			nStyle|=(DWORD)wParam;
			ctrl->nOtherDef=FALSE;
			SetWindowLong(hWnd,GWL_STYLE,(LONG)nStyle);
			if (LOWORD(lParam))
				InvalidateRect(hWnd,NULL,TRUE);
			return 0;
		}
	case B3DM_GETSTYLE:
		return (BOOL)GetWindowLong(hWnd,GWL_STYLE)&(BS_PUSHBUTTON|BS_DEFPUSHBUTTON);
	case B3DM_SETBITMAP:
		ctrl->hBitmap=(HBITMAP)lParam;
		InvalidateRect(hWnd,NULL,TRUE);
		return TRUE;
	case B3DM_SETDISABLEDBITMAP:
		ctrl->hDisBitmap=(HBITMAP)lParam;
		InvalidateRect(hWnd,NULL,TRUE);
		return TRUE;
	case B3DM_SETHOTBITMAP:
		ctrl->hHotBitmap=(HBITMAP)lParam;
		InvalidateRect(hWnd,NULL,TRUE);
		return TRUE;
	case B3DM_GETBITMAP:
		return (LRESULT)ctrl->hBitmap;
	case B3DM_GETDISABLEDBITMAP:
		return (LRESULT)ctrl->hDisBitmap;
	case B3DM_GETHOTBITMAP:
		return (LRESULT)ctrl->hHotBitmap;
	case B3DM_ISPRESSED:
		return ctrl->nPressed;
	case B3DM_SETCOLOR:
		if ((DWORD)wParam<6)
		{
			ctrl->pColors[wParam]=(COLORREF)lParam;
			InvalidateRect(hWnd,NULL,TRUE);
			return TRUE;
		}
		return FALSE;
	case B3DM_GETCOLOR:
		if ((DWORD)wParam<6)
			return ctrl->pColors[wParam];
		else
			return (BOOL)(COLORREF)-1;
	case WM_LBUTTONDOWN:
		SetCapture(hWnd);
		if (!ctrl->nPressed)
		{
			ctrl->nPressed|=1;
			SetFocus(hWnd);
			SendMessage(GetParent(hWnd),WM_COMMAND,MAKEWPARAM(GetDlgCtrlID(hWnd),B3DN_HILITE),(LPARAM)hWnd);
			InvalidateRect(hWnd,NULL,TRUE);
		}
		break;
	case WM_LBUTTONUP:
		ReleaseCapture();
		if (ctrl->nPressed)
		{
			ctrl->nPressed&=~1;
			InvalidateRect(hWnd,NULL,TRUE);
			if (!ctrl->nPressed)
			{
				SendMessage(GetParent(hWnd),WM_COMMAND,MAKEWPARAM(GetDlgCtrlID(hWnd),B3DN_CLICKED),(LPARAM)hWnd);
				SendMessage(GetParent(hWnd),WM_COMMAND,MAKEWPARAM(GetDlgCtrlID(hWnd),B3DN_UNHILITE),(LPARAM)hWnd);
			}
		}
		break;
	case WM_LBUTTONDBLCLK:
		SendMessage(GetParent(hWnd),WM_COMMAND,MAKEWPARAM(GetDlgCtrlID(hWnd),B3DN_DOUBLECLICKED),(LPARAM)hWnd);
		break;
	case WM_SIZE:
		InvalidateRect(hWnd,NULL,TRUE);
		return 0;
	case WM_SETFOCUS:
		ctrl->nOtherDef=FALSE;
		InvalidateRect(hWnd,NULL,TRUE);
		SendMessage(GetParent(hWnd),WM_COMMAND,MAKEWPARAM(GetDlgCtrlID(hWnd),B3DN_SETFOCUS),(LPARAM)hWnd);
		return 0;
	case WM_KILLFOCUS:
		if (SendMessage((HWND)wParam,WM_GETDLGCODE,0,0)&DLGC_BUTTON)
			ctrl->nOtherDef=TRUE;			
		InvalidateRect(hWnd,NULL,TRUE);
		SendMessage(GetParent(hWnd),WM_COMMAND,MAKEWPARAM(GetDlgCtrlID(hWnd),B3DN_KILLFOCUS),(LPARAM)hWnd);
		return 0;
	case WM_KEYDOWN:
		if (wParam==' ' && !(ctrl->nPressed&2))
		{
			ctrl->nPressed|=2;
			if (!(ctrl->nPressed&1))
			{
				InvalidateRect(hWnd,NULL,TRUE);
				SendMessage(GetParent(hWnd),WM_COMMAND,MAKEWPARAM(GetDlgCtrlID(hWnd),B3DN_HILITE),(LPARAM)hWnd);
			}
		}
		return 0;
	case WM_KEYUP:
		if (wParam==' ' && ctrl->nPressed&2)
		{
			ctrl->nPressed&=~2;
			if (!ctrl->nPressed)
			{
				InvalidateRect(hWnd,NULL,TRUE);
				SendMessage(GetParent(hWnd),WM_COMMAND,MAKEWPARAM(GetDlgCtrlID(hWnd),B3DN_CLICKED),(LPARAM)hWnd);
				SendMessage(GetParent(hWnd),WM_COMMAND,MAKEWPARAM(GetDlgCtrlID(hWnd),B3DN_UNHILITE),(LPARAM)hWnd);
			}
		}
		return 0;
	case WM_ERASEBKGND:
		return 0;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hDC=BeginPaint(hWnd,&ps);
			RECT rect;
			GetClientRect(hWnd,&rect);
			if (rect.right<ps.rcPaint.right)
				rect.right=ps.rcPaint.right;
			if (rect.bottom<ps.rcPaint.bottom)
				rect.bottom=ps.rcPaint.bottom;
			HBRUSH hFaceBrush=GetSysColorBrush(COLOR_BTNFACE);
			HBRUSH hOldBrush=(HBRUSH)SelectObject(hDC,hFaceBrush);
			HPEN hWhitePen=CreatePen(PS_SOLID,1,GetSysColor(COLOR_3DHILIGHT));
			HPEN hLTGrayPen=CreatePen(PS_SOLID,1,GetSysColor(COLOR_3DLIGHT));
			HPEN hGrayPen=CreatePen(PS_SOLID,1,GetSysColor(COLOR_3DSHADOW));
			HPEN hBlackPen=CreatePen(PS_SOLID,1,GetSysColor(COLOR_3DDKSHADOW));

			HPEN hOldPen=(HPEN)SelectObject(hDC,GetStockObject(NULL_PEN));
			HFONT hOldFont=(HFONT)SelectObject(hDC,ctrl->hFont);
			DWORD nStyle=(DWORD)GetWindowLong(hWnd,GWL_STYLE);

			if (nStyle&C3DS_BORDER && !ctrl->nPressed)
			{
				if (((nStyle&BS_DEFPUSHBUTTON)==BS_DEFPUSHBUTTON && !ctrl->nOtherDef) || GetFocus()==hWnd)
				{
					SelectObject(hDC,hBlackPen);
					Rectangle(hDC,rect.left,rect.top,rect.right+1,rect.bottom+1);
					rect.left++;
					rect.top++;
					rect.right--;
					rect.bottom--;
				}
				else
					Rectangle(hDC,rect.left,rect.top,rect.right+1,rect.bottom+1);
				SelectObject(hDC,hWhitePen);
				MoveToEx(hDC,rect.left,rect.bottom-1,NULL);
				LineTo(hDC,rect.left,rect.top);
				LineTo(hDC,rect.right,rect.top);
				SelectObject(hDC,hBlackPen);
				MoveToEx(hDC,rect.left,rect.bottom,NULL);
				LineTo(hDC,rect.right,rect.bottom);
				LineTo(hDC,rect.right,rect.top-1);
				if ((nStyle&B3DS_MOUSEHILIGHT)==0 || ctrl->nMouseOver)
				{
					SelectObject(hDC,hGrayPen);
					MoveToEx(hDC,rect.left+1,rect.bottom-1,NULL);
					LineTo(hDC,rect.right-1,rect.bottom-1);
					LineTo(hDC,rect.right-1,rect.top);
					SelectObject(hDC,hLTGrayPen);
					MoveToEx(hDC,rect.left+1,rect.bottom-1,NULL);
					LineTo(hDC,rect.left+1,rect.top+1);
					LineTo(hDC,rect.right-2,rect.top+1);
				}
			}
			else if (nStyle&C3DS_BORDER && ctrl->nPressed)
			{
				SelectObject(hDC,hBlackPen);
				Rectangle(hDC,rect.left,rect.top,rect.right+1,rect.bottom+1);
				SelectObject(hDC,hGrayPen);
				Rectangle(hDC,rect.left+1,rect.top+1,rect.right,rect.bottom);
			}
			else
				Rectangle(hDC,rect.left,rect.top,rect.right+1,rect.bottom+1);

			if (GetFocus()==hWnd)
			{
				RECT rc;
				rc=rect;
				rc.left+=3;
				rc.top+=3;
				rc.bottom-=2;
				rc.right-=2;
				DrawFocusRect(hDC,&rc);
			}
			
			if (ctrl->nPressed)
			{
				rect.left++;
				rect.top++;
				rect.right++;
				rect.bottom++;
			}
			HBITMAP hBitmap=NULL;
			if (!IsWindowEnabled(hWnd))
				hBitmap=ctrl->hDisBitmap;
			else if (nStyle&B3DS_MOUSEHILIGHT && ctrl->nMouseOver)
			{
				if (ctrl->hHotBitmap!=NULL)
					hBitmap=ctrl->hHotBitmap;
				else
					hBitmap=ctrl->hBitmap;
			}
			else
				hBitmap=ctrl->hBitmap;
			if (hBitmap!=NULL)
			{	
				BITMAPINFO bi;
				bi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
				bi.bmiHeader.biBitCount=0;
				GetDIBits(hDC,(HBITMAP)hBitmap,0,0,NULL,&bi,DIB_RGB_COLORS);
				SIZE sz={ bi.bmiHeader.biWidth , bi.bmiHeader.biHeight };
				if (nStyle&B3DS_LEFTBITMAP)
				{
					DrawState(hDC,NULL,NULL,(LPARAM)hBitmap,0,
						rect.left+4,(rect.top+rect.bottom-sz.cy)/2,
						sz.cx,sz.cy,DST_BITMAP);
					rect.left+=4+sz.cx;
				}
				else
				{
					DrawState(hDC,NULL,NULL,(LPARAM)hBitmap,0,
						(rect.left+rect.right-sz.cx)/2,rect.top+4,
						sz.cx,sz.cy,DST_BITMAP);
					rect.top+=4+sz.cy;
				}
			}
			COLORREF cr1,cr2;

			if (!IsWindowEnabled(hWnd))
			{
				cr1=ctrl->pColors[0];
				cr2=ctrl->pColors[1];
			}
			else if ((nStyle&B3DS_MOUSEHILIGHT)==0 || ctrl->nMouseOver)
			{
				cr1=ctrl->pColors[2];
				cr2=ctrl->pColors[3];
			}
			else
			{
				cr1=ctrl->pColors[4];
				cr2=ctrl->pColors[5];
			}
			
			SetBkMode(hDC,TRANSPARENT);
			SetTextColor(hDC,cr2);
			rect.left++;
			rect.right++;
			rect.top++;
			rect.bottom++;
			if (nStyle&B3DS_TEXTLEFT)
				DrawText(hDC,ctrl->szText,-1,&rect,DT_LEFT|(nStyle&B3DS_MULTILINE?0:DT_SINGLELINE|DT_VCENTER));
			else if (nStyle&B3DS_TEXTRIGHT)
				DrawText(hDC,ctrl->szText,-1,&rect,DT_RIGHT|(nStyle&B3DS_MULTILINE?0:DT_SINGLELINE|DT_VCENTER));
			else
				DrawText(hDC,ctrl->szText,-1,&rect,DT_CENTER|(nStyle&B3DS_MULTILINE?0:DT_SINGLELINE|DT_VCENTER));
			SetTextColor(hDC,cr1);
			rect.left--;
			rect.right--;
			rect.top--;
			rect.bottom--;
			if (nStyle&B3DS_TEXTLEFT)
				DrawText(hDC,ctrl->szText,-1,&rect,DT_LEFT|(nStyle&B3DS_MULTILINE?0:DT_SINGLELINE|DT_VCENTER));
			else if (nStyle&B3DS_TEXTRIGHT)
				DrawText(hDC,ctrl->szText,-1,&rect,DT_RIGHT|(nStyle&B3DS_MULTILINE?0:DT_SINGLELINE|DT_VCENTER));
			else
				DrawText(hDC,ctrl->szText,-1,&rect,DT_CENTER|(nStyle&B3DS_MULTILINE?0:DT_SINGLELINE|DT_VCENTER));
			
			SelectObject(hDC,hOldBrush);
			SelectObject(hDC,hOldPen);
			SelectObject(hDC,hOldFont);
			DeleteObject(hWhitePen);
			EndPaint(hWnd,&ps);
   		}
		return 0;
	case WM_SETTEXT:
		{
			if (ctrl->szText!=NULL)
				delete[] ctrl->szText;
			LPCSTR temp=(LPCSTR)lParam;
			int templen=istrlen(temp);
			ctrl->szText=new char[templen+2];
			if (ctrl->szText==NULL)
			{
				SetHFCError(HFC_CANNOTALLOC);
				return FALSE;
			}
			MemCopy(ctrl->szText,temp,templen+1);
			InvalidateRect(hWnd,NULL,TRUE);
			return TRUE;
		}
	case WM_SETFONT:
		{
			ctrl->hFont=(HFONT)wParam;
			if (LOWORD(lParam))
				InvalidateRect(hWnd,NULL,TRUE);
			break;
		}
	case WM_DESTROY:
		if (ctrl!=NULL)
		{
			if (ctrl->szText!=NULL)
				delete[] ctrl->szText;
			if (ctrl->pColors!=NULL)
				delete[] ctrl->pColors;
			delete ctrl;
		}
		SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG_PTR)NULL);
		return FALSE;
	default:
		return DefWindowProc(hWnd,msg,wParam,lParam);
	}
	return 0;
}

BOOL HFCControls::StaticPaint(HDC hDC,HWND hWnd,void* ctrl)
{
	RECT rect;
	RECT rt;
	SIZE size;
	DWORD i,j=0;
	HBRUSH hBrush=CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
	HBRUSH hOldBrush=(HBRUSH)SelectObject(hDC,hBrush);
	HFONT hOldFont;
	HPEN hWhitePen=CreatePen(PS_SOLID,1,GetSysColor(COLOR_3DHILIGHT));
	HPEN hGrayPen=CreatePen(PS_SOLID,1,GetSysColor(COLOR_3DSHADOW));
	HPEN hOldPen=(HPEN)SelectObject(hDC,hWhitePen);
	GetClientRect(hWnd,&rect);

	if (GetWindowLong(hWnd,GWL_STYLE)&C3DS_BORDER)
	{
		Rectangle(hDC,rect.left,rect.top,rect.right,rect.bottom);
		SelectObject(hDC,hGrayPen);
		Rectangle(hDC,rect.left,rect.top,rect.right-1,rect.bottom-1);
		SelectObject(hDC,hWhitePen);
		MoveToEx(hDC,rect.right-3,rect.top+1,NULL);
		LineTo(hDC,rect.left+1,rect.top+1);
		LineTo(hDC,rect.left+1,rect.bottom-2);
	}
	else
	{
		SelectObject(hDC,GetSysColorBrush(COLOR_BTNFACE));
		SelectObject(hDC,GetStockObject(NULL_PEN));
		Rectangle(hDC,rect.left,rect.top,rect.right+1,rect.bottom+1);
	}
	if (GetFocus()==hWnd)
	{
		RECT rc;
		rc=rect;
		rc.left++;
		rc.top++;
		rc.bottom-=2;
		rc.right-=2;
		DrawFocusRect(hDC,&rc);
	}
	
	rt.left=rect.left+4;
	rt.top=rect.top+2;
	rt.right=rect.right-4;
	rt.bottom=rect.bottom-2;
	
	SetBkColor(hDC,GetSysColor(COLOR_BTNFACE));
	SetTextColor(hDC,GetSysColor(COLOR_BTNTEXT));

	hOldFont=(HFONT)SelectObject(hDC,((LPSCTRL)ctrl)->hFont);
	
	while(1)
	{
		for (i=0;((LPSCTRL)ctrl)->szText[i+j]>3;i++);
		GetTextExtentPoint32(hDC,(LPCTSTR)(((LPSCTRL)ctrl)->szText+j),i,&size);
		while (rt.left+size.cx>rect.right-3 && i>1)
			GetTextExtentPoint32(hDC,(LPCTSTR)(((LPSCTRL)ctrl)->szText+j),--i,&size);
		if (rt.left+size.cx>rect.right-3)
			break;
		DrawText(hDC,(LPCTSTR)(((LPSCTRL)ctrl)->szText+j),i,&rt,DT_VCENTER|DT_SINGLELINE|DT_LEFT|DT_EXPANDTABS);	
		if (((LPSCTRL)ctrl)->szText[j+i]=='\0')
			break;
		rt.left+=size.cx;
		j+=i;
		switch(((LPSCTRL)ctrl)->szText[j])
		{
		case '\1':
			SetTextColor(hDC,RGB(((LPSCTRL)ctrl)->szText[j+1],
				((LPSCTRL)ctrl)->szText[j+2],
				((LPSCTRL)ctrl)->szText[j+3]));
			j+=4;
			break;
		case '\2':
			SetBkColor(hDC,RGB(((LPSCTRL)ctrl)->szText[j+1],
				((LPSCTRL)ctrl)->szText[j+2],
				((LPSCTRL)ctrl)->szText[j+3]));
			j+=4;
			break;
		case '\3':
			SelectObject(hDC,
				*((HFONT*)(&((LPSCTRL)((LPSCTRL)ctrl))->szText[j+1])));
			j+=5;
			break;
		}
	}
	SelectObject(hDC,hOldFont);
	SelectObject(hDC,hOldPen);
	SelectObject(hDC,hOldBrush);
	DeleteObject(hGrayPen);
	DeleteObject(hWhitePen);
	DeleteObject(hBrush);
	return TRUE;
}

LRESULT CALLBACK HFCControls::Ctrl3DStaticProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	LPSCTRL ctrl;
	if (msg==WM_CREATE)
	{
		BYTE* temp=(BYTE*)((CREATESTRUCT*)lParam)->lpszName;
		ctrl=new SCTRL;
		if (ctrl==NULL)
		{
			SetHFCError(HFC_CANNOTALLOC);
			return FALSE;
		}
		ctrl->szBuffer=NULL;
		ctrl->nBufferLen=0;
		ctrl->nAllocLen=0;
		
		SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG_PTR)ctrl);
		
		DWORD i;
		for (i=0;temp[i]!='\0';i++)
		{
			switch(temp[i])
			{
			case '\1':
				i+=3;
				break;
			case '\2':
				i+=3;
				break;
			case '\3':
				i+=4;
				break;
			}
		}
		ctrl->szText=new BYTE[i+2];
		if (ctrl->szText==NULL)
		{
			SetHFCError(HFC_CANNOTALLOC);
			return FALSE;
		}
		MemCopy(ctrl->szText,temp,i+1);
		ctrl->hFont=(HFONT)GetStockObject(DEFAULT_GUI_FONT);
		return FALSE;
	}

	ctrl=(LPSCTRL)GetWindowLongPtr(hWnd,GWLP_USERDATA);	
	switch(msg)
	{
	case WM_SIZE:
	case WM_SETFOCUS:
	case WM_KILLFOCUS:
		InvalidateRect(hWnd,NULL,TRUE);
		return 0;
	case WM_ERASEBKGND:
		return 1;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hDC=BeginPaint(hWnd,&ps);
			if (hDC==NULL)
				return 0;
			StaticPaint(hDC,hWnd,ctrl);
			EndPaint(hWnd,&ps);
   		}
		return 0;
	case WM_SETTEXT:
		{
			if (ctrl->szText!=NULL)
				delete[] ctrl->szText;
			BYTE* temp=(BYTE*)lParam;
			DWORD i;
			for (i=0;temp[i]!='\0';i++)
			{
				switch(temp[i])
				{
				case '\1':
					i+=3;
					break;
				case '\2':
					i+=3;
					break;
				case '\3':
					i+=4;
					break;
				}
			}
			ctrl->szText=new BYTE[i+2];
			if (ctrl->szText==NULL)
			{
				SetHFCError(HFC_CANNOTALLOC);
				return FALSE;
			}
			MemCopy(ctrl->szText,temp,i+1);
			InvalidateRect(hWnd,NULL,TRUE);
		}
		break;
	case WM_SETFONT:
		{
			ctrl->hFont=(HFONT)wParam;
			if (LOWORD(lParam))
				InvalidateRect(hWnd,NULL,TRUE);
		}
		break;
	case WM_DESTROY:
		if (ctrl!=NULL)
		{
			if (ctrl->szText!=NULL)
				delete[] ctrl->szText;
			if (ctrl->szBuffer!=NULL)
				delete[] ctrl->szBuffer;
			delete ctrl;
		}
		SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG_PTR)NULL);
		return FALSE;
	default:
		return DefWindowProc(hWnd,msg,wParam,lParam);
	}
	return 0;
}


static void clear(LPHEXVIEWCTRL ctrl)
{
	if (ctrl->pData!=NULL)
		delete[] ctrl->pData;
	ctrl->pData=NULL;
	ctrl->nDataLen=0;
	ctrl->nAllocLen=0;
	ctrl->nLine=0;
	ctrl->nLines=0;
}

static BYTE GetString(LPHEXVIEWCTRL ctrl,CString& str,int nStartPos)
{
	char szBuf[5];
	SIZE_T nLen=ctrl->nDataLen-nStartPos;
	SIZE_T i;
	BYTE ret=noError;
	
	if (INT(nLen)<=0)
		return noMoreLines;
	if (nLen>ctrl->nBytesPerLine)
		nLen=ctrl->nBytesPerLine;
	else
		ret=noMoreLines;
	str.Format("%08x  ",nStartPos);
	str.MakeUpper();
	for (i=0;i<nLen;i++)
	{
		StringCbPrintf(szBuf,5,"%02X ",(int)ctrl->pData[nStartPos+i]);
		str << szBuf;
		if (i==ctrl->nHalfLine)
			str << ' ';
	}
	for (;i<ctrl->nBytesPerLine;i++)
	{
		str << "   ";
		if (i==ctrl->nHalfLine)
			str << ' ';
	}
	str << ' ';
	for (i=0;i<nLen;i++)
	{
		if (ctrl->pData[nStartPos+i]<=0x20 || ctrl->pData[nStartPos+i]>0x7E)
			str << '.';
		else
			str << (char)ctrl->pData[nStartPos+i];
	}
	for (;i<ctrl->nBytesPerLine;i++)
		str << ' ';
	str << ' ';
	return ret;
}

static inline void HexViewControlPaintProc(HWND hWnd,LPHEXVIEWCTRL ctrl)
{
	CRect rect;
	CSize size;
	BYTE err;
	PAINTSTRUCT ps;
	HDC hDC=BeginPaint(hWnd,&ps);
	CString hexdata;
	int i,nScrLines;
	
	
	
	SCROLLINFO sci;
	sci.cbSize=sizeof(SCROLLINFO);
	SetBkMode(hDC,TRANSPARENT);
	SelectObject(hDC,ctrl->hFont);
	GetClientRect(hWnd,&rect);
	err=GetString(ctrl,hexdata,ctrl->nBytesPerLine*ctrl->nLine);
	GetTextExtentPoint32(hDC,hexdata,(int)hexdata.GetLength(),&size);
	size.cx+=2;
	ctrl->nLines=ctrl->nDataLen/ctrl->nBytesPerLine;
	if ((i=size.cx-rect.right)>0)
	{
		sci.fMask=SIF_PAGE|SIF_RANGE;
		sci.nMin=0;
		sci.nMax=i;
		sci.nPage=8;
		SetScrollInfo(hWnd,SB_HORZ,&sci,TRUE);
	}
	else
	{
		sci.fMask=SIF_ALL;
		sci.nMin=0;
		sci.nMax=0;
		sci.nPage=1;
		sci.nPos=0;
		SetScrollInfo(hWnd,SB_HORZ,&sci,TRUE);
	}
	GetClientRect(hWnd,&rect);
	if (size.cy<1)
		size.cy=1;
	nScrLines=rect.bottom/size.cy;
	if ((i=int(ctrl->nLines-nScrLines+(ctrl->nDataLen%ctrl->nBytesPerLine==0?0:1)))>0)
	{
		sci.fMask=SIF_POS|SIF_RANGE;
		sci.nPos=0;
		GetScrollInfo(hWnd,SB_VERT,&sci);
		
		if (sci.nMax!=i || sci.nPos!=ctrl->nLine)
		{
			sci.fMask=SIF_ALL;
			sci.nMin=0;
			sci.nPage=1;
		
			if (ctrl->bFlags&HEXVIEWCTRl_FLAGSCROLLTOEND && i==sci.nMax+1 && sci.nPos==sci.nMax)
			{
				ctrl->bFlags|=HEXVIEWCTRl_FLAGERASEBGK;
				sci.nPos=ctrl->nLine=sci.nMax=i;
			}
			else
			{
				sci.nMax=i;
				if (sci.nPos!=ctrl->nLine)
				{
					ctrl->bFlags|=HEXVIEWCTRl_FLAGERASEBGK;
					sci.nPos=ctrl->nLine;
				}
			}
			SetScrollInfo(hWnd,SB_VERT,&sci,TRUE);
		}
		if (ctrl->nLine>i)
		{
			ctrl->nLine=i;
			err=GetString(ctrl,hexdata,ctrl->nBytesPerLine*ctrl->nLine);
		}
	}
	else
	{
		sci.fMask=SIF_ALL;
		sci.nMin=0;
		sci.nMax=0;
		sci.nPage=1;
		sci.nPos=0;
		SetScrollInfo(hWnd,SB_VERT,&sci,TRUE);
		ctrl->nLine=0;
	}
	
	// Erasing background if necessary
	if (ctrl->bFlags&HEXVIEWCTRl_FLAGERASEBGK)
	{
		FillRect(hDC,&ps.rcPaint,ctrl->hbrBackground);
		ctrl->bFlags&=~HEXVIEWCTRl_FLAGERASEBGK;
	}
	
	
	GetClientRect(hWnd,&rect);
	nScrLines=rect.bottom/size.cy;
	i=1;
	
	sci.fMask=SIF_POS;
	sci.nPos=0;
	::GetScrollInfo(hWnd,SB_HORZ,&sci);
	rect.left-=sci.nPos;
	
	//TODO: this might be unnesescary
	err=GetString(ctrl,hexdata,ctrl->nLine*ctrl->nBytesPerLine);
	rect.top+=DrawText(hDC,hexdata,(int)hexdata.GetLength(),&rect,DT_SINGLELINE|DT_LEFT|DT_TOP);
	while (err==noError && i<nScrLines)
	{
		err=GetString(ctrl,hexdata,(i+ctrl->nLine)*ctrl->nBytesPerLine);
		DrawText(hDC,hexdata,(int)hexdata.GetLength(),&rect,DT_SINGLELINE|DT_LEFT|DT_TOP);
		rect.top+=size.cy;
		i++;
	}
}

static inline void HexViewControlVScrollProc(HWND hWnd,LPHEXVIEWCTRL ctrl,UINT nSBCode,UINT nPos)
{
	SCROLLINFO lpsi;
	lpsi.cbSize=sizeof(SCROLLINFO);
	lpsi.fMask=SIF_RANGE|SIF_POS;
	lpsi.nPos=0;
	::GetScrollInfo(hWnd,SB_VERT,&lpsi);

	switch (nSBCode)
	{
	case SB_TOP:
		ctrl->nLine=0;
		break;
	case SB_BOTTOM:
		ctrl->nLine=lpsi.nMax;
		break;
	case SB_LINEDOWN:
		ctrl->nLine++;
		break;
	case SB_LINEUP:
		ctrl->nLine--;
		break;
	case SB_PAGEDOWN:
		ctrl->nLine+=5;
		break;
	case SB_PAGEUP:
		ctrl->nLine-=5;
		break;
	case SB_THUMBPOSITION:
		ctrl->nLine=nPos;
		break;
	case SB_THUMBTRACK:
		ctrl->nLine=nPos;
		break;
	}
	if (ctrl->nLine<0)
		ctrl->nLine=0;
	if (ctrl->nLine>lpsi.nMax)
		ctrl->nLine=lpsi.nMax;
	
	if (lpsi.nPos!=ctrl->nLine)
	{
		lpsi.fMask=SIF_POS;
		lpsi.nPos=ctrl->nLine;
		::SetScrollInfo(hWnd,SB_VERT,&lpsi,TRUE);
		ctrl->bFlags|=HEXVIEWCTRl_FLAGERASEBGK;
		InvalidateRect(hWnd,NULL,TRUE);
	}
}

static inline void HexViewControlHScrollProc(HWND hWnd,LPHEXVIEWCTRL ctrl,UINT nSBCode,UINT nPos)
{
	SCROLLINFO lpsi;
	lpsi.cbSize=sizeof(SCROLLINFO);
	lpsi.fMask=SIF_POS|SIF_RANGE;
	lpsi.nPos=0;
	::GetScrollInfo(hWnd,SB_HORZ,&lpsi);
	int nPrevPos=lpsi.nPos;
	
	switch (nSBCode)
	{
	case SB_TOP:
		lpsi.nPos=0;
		break;
	case SB_BOTTOM:
		lpsi.nPos=lpsi.nMax;
		break;
	case SB_LINEDOWN:
		lpsi.nPos+=8;
		break;
	case SB_LINEUP:
		lpsi.nPos-=8;
		break;
	case SB_PAGEDOWN:
		lpsi.nPos+=24;
		break;
	case SB_PAGEUP:
		lpsi.nPos-=24;
		break;
	case SB_THUMBPOSITION:
		lpsi.nPos=nPos;
		break;
	case SB_THUMBTRACK:
		lpsi.nPos=nPos;
		break;
	}
	if (lpsi.nPos<0)
		lpsi.nPos=0;
	if (lpsi.nPos>lpsi.nMax)
		lpsi.nPos=lpsi.nMax;
	
	if (nPrevPos!=lpsi.nPos)
	{
		lpsi.fMask=SIF_POS;
		::SetScrollInfo(hWnd,SB_HORZ,&lpsi,TRUE);
		ctrl->bFlags|=HEXVIEWCTRl_FLAGERASEBGK;
		InvalidateRect(hWnd,NULL,TRUE);
	}
}

LRESULT CALLBACK HFCControls::HexViewControlProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	LPHEXVIEWCTRL ctrl;
	if (msg==WM_CREATE)
	{
		ctrl=new HEXVIEWCTRL;
		if (ctrl==NULL)
		{
			SetHFCError(HFC_CANNOTALLOC);
			return FALSE;
		}
		SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG_PTR)ctrl);
		ctrl->hFont=(HFONT)GetStockObject(ANSI_FIXED_FONT);
		ctrl->nLine=0;
		ctrl->nLines=0;
		ctrl->nAllocExtra=HEXVIEWCTRL_DEFALLOCEXTRA;
		ctrl->pData=NULL;
		ctrl->nDataLen=0;
		ctrl->nAllocLen=0;
		ctrl->nBytesPerLine=HEXVIEWCTRl_DEFBYTESPERLINE;
		ctrl->nHalfLine=HEXVIEWCTRl_DEFBYTESPERLINE/2-1;
		ctrl->hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
		ctrl->bFlags=HEXVIEWCTRl_FLAGSCROLLTOEND|HEXVIEWCTRl_FLAGERASEBGK;
	}
	else
		ctrl=(LPHEXVIEWCTRL)GetWindowLongPtr(hWnd,GWLP_USERDATA);	

	switch(msg)
	{
	case WM_SIZE:
	case WM_SETFOCUS:
	case WM_KILLFOCUS:
		InvalidateRect(hWnd,NULL,TRUE);
		return 0;
	case WM_ERASEBKGND:
		ctrl->bFlags|=HEXVIEWCTRl_FLAGERASEBGK;
		return 1;
	case WM_DESTROY:
		if (ctrl!=NULL)
			delete ctrl;
		SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG_PTR)NULL);
		return FALSE;
	case WM_PAINT:
		HexViewControlPaintProc(hWnd,ctrl);	
		break;
	case WM_VSCROLL:
		HexViewControlVScrollProc(hWnd,ctrl,LOWORD(wParam),HIWORD(wParam));
		return 0;
	case WM_HSCROLL:
		HexViewControlHScrollProc(hWnd,ctrl,LOWORD(wParam),HIWORD(wParam));
		return 0;
	case HVM_SETDATA: // WPARAM=datalen LPARAM=data
		if (wParam==NULL) // Clear data
		{
			clear(ctrl);
	
			ctrl->bFlags|=HEXVIEWCTRl_FLAGERASEBGK;
			InvalidateRect(hWnd,NULL,TRUE);
			return TRUE;
		}
		if (wParam>ctrl->nAllocLen || wParam+ctrl->nAllocExtra<ctrl->nAllocLen)
		{
			if (ctrl->pData!=NULL)
				delete[] ctrl->pData;
			ctrl->pData=new BYTE[ctrl->nAllocLen=wParam+ctrl->nAllocExtra];
			if (ctrl->pData==NULL)
				return FALSE;
		}
		MemCopy(ctrl->pData,(void*)lParam,ctrl->nDataLen=wParam);
		ctrl->nDataLen=wParam;
		ctrl->bFlags|=HEXVIEWCTRl_FLAGERASEBGK;
		InvalidateRect(hWnd,NULL,TRUE);
		return TRUE;
	case HVM_GETDATA: // LPARAM=data
		MemCopy((void*)lParam,ctrl->pData,ctrl->nDataLen);
		return TRUE;
	case HVM_GETDATALENGTH:
		return ctrl->nDataLen;
	case HVM_ADDDATA: // WPARAM=datalen LPARAM=new data
		if (wParam+ctrl->nDataLen<=ctrl->nAllocLen)
			MemCopy(ctrl->pData+ctrl->nDataLen,(void*)lParam,wParam);
		else
		{
			BYTE* pNewData=new BYTE[ctrl->nAllocLen=ctrl->nDataLen+wParam+ctrl->nAllocExtra];
			if (pNewData==NULL)
				return FALSE;
			MemCopy(pNewData,ctrl->pData,ctrl->nDataLen);
			MemCopy(pNewData,(void*)lParam,wParam);
			delete[] ctrl->pData;
			ctrl->pData=pNewData;
		}
		ctrl->nDataLen+=wParam;
		InvalidateRect(hWnd,NULL,FALSE);
		return TRUE;
	case HVM_ADDDATAANDDELETE: // WPARAM=datalen LPARAM=new data,GlobalAlloc  object
		{
			void* pData=GlobalLock(HGLOBAL(lParam));

			if (wParam+ctrl->nDataLen<=ctrl->nAllocLen)
				MemCopy(ctrl->pData+ctrl->nDataLen,(void*)pData,wParam);
			else
			{
				BYTE* pNewData=new BYTE[ctrl->nAllocLen=ctrl->nDataLen+wParam+ctrl->nAllocExtra];
				if (pNewData==NULL)
					return FALSE;
				MemCopy(pNewData,ctrl->pData,ctrl->nDataLen);
				MemCopy(pNewData,(void*)pData,wParam);
				delete[] ctrl->pData;
				ctrl->pData=pNewData;
			}
			ctrl->nDataLen+=wParam;
			InvalidateRect(hWnd,NULL,FALSE);

			GlobalUnlock(HGLOBAL(lParam));
			GlobalFree(HGLOBAL(lParam));
			return TRUE;
		}
	case HVM_SETEXTRAALLOCSIZE:// WPARAM=extraalloc
		ctrl->nAllocExtra=wParam;
		return 1;
	case HVM_GETEXTRAALLOCSIZE:
		return ctrl->nAllocExtra;
	case HVM_GETALLOCLEN:
		return ctrl->nAllocLen;
	case HVM_REALLOC: // WPARAM=new alloc size
		{
			if (wParam==0)
			{
				clear(ctrl);
				InvalidateRect(hWnd,NULL,TRUE);
				return TRUE;
			}	
			BYTE* pNewData=new BYTE[wParam];
			if (pNewData==NULL)
				return FALSE;
			ctrl->nAllocLen=wParam;
			if (ctrl->nAllocLen<ctrl->nDataLen)
			{
				MemCopy(pNewData,ctrl->pData,ctrl->nAllocLen);
				ctrl->nDataLen=wParam;
				InvalidateRect(hWnd,NULL,TRUE);
			}
			else
				MemCopy(pNewData,ctrl->pData,ctrl->nDataLen);
			if (ctrl->pData!=NULL)
				delete[] ctrl->pData;
			ctrl->pData=pNewData;
			ctrl->bFlags|=HEXVIEWCTRl_FLAGERASEBGK;
			InvalidateRect(hWnd,NULL,FALSE);
			return TRUE;
		}
	case HVM_SETBYTESPERLINE: // WPARAM=byter per line
		ctrl->nBytesPerLine=(BYTE)wParam;
		if (ctrl->nBytesPerLine>1)
			ctrl->nHalfLine=ctrl->nBytesPerLine/2-1;
		else
			ctrl->nHalfLine=0;
		ctrl->bFlags|=HEXVIEWCTRl_FLAGERASEBGK;
		InvalidateRect(hWnd,NULL,TRUE);
		return 1;
	case HVM_GETBYTESPERLINE:
		return ctrl->nBytesPerLine;
	default:
		return DefWindowProc(hWnd,msg,wParam,lParam);
	}
	return 0;
}

BYTE InitHFCControls()
{
	WNDCLASSEX wc;
	wc.cbSize=sizeof(WNDCLASSEX);
	wc.style=CS_PARENTDC|CS_DBLCLKS|CS_HREDRAW|CS_VREDRAW;
	wc.lpfnWndProc=(WNDPROC)HFCControls::Ctrl3DButtonProc;
    wc.cbClsExtra=0;
    wc.cbWndExtra=0;
    wc.hInstance=GetInstanceHandle();
    wc.hIcon=NULL; 
    wc.hCursor=NULL; 
    wc.hbrBackground=(HBRUSH)COLOR_BTNFACE;
    wc.lpszMenuName=NULL; 
    wc.lpszClassName="HFC3DBUTTON"; 
    wc.hIconSm=NULL; 
	if (!RegisterClassEx(&wc))
		return FALSE;
	
	wc.style=CS_PARENTDC|CS_DBLCLKS;
	wc.lpfnWndProc=(WNDPROC)HFCControls::Ctrl3DStaticProc;
    wc.cbClsExtra=0;
    wc.cbWndExtra=0;
    wc.hInstance=GetInstanceHandle();
    wc.hIcon=NULL; 
    wc.hCursor=NULL; 
    wc.hbrBackground=(HBRUSH)COLOR_BTNFACE;
    wc.lpszMenuName=NULL; 
    wc.lpszClassName="HFC3DSTATIC"; 
    wc.hIconSm=NULL; 
	if (!RegisterClassEx(&wc))
		return FALSE;

	wc.style=CS_PARENTDC|CS_DBLCLKS|CS_HREDRAW|CS_VREDRAW;
	wc.lpfnWndProc=(WNDPROC)HFCControls::HexViewControlProc;
    wc.cbClsExtra=0;
    wc.cbWndExtra=0;
    wc.hInstance=GetInstanceHandle();
    wc.hIcon=NULL; 
    wc.hCursor=NULL; 
    wc.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName=NULL; 
    wc.lpszClassName="HFCHEXVIEWCTL"; 
    wc.hIconSm=NULL; 
	if (!RegisterClassEx(&wc))
		return FALSE;
	return TRUE;
}

#endif