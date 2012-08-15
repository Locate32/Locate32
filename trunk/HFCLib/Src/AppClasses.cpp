////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////

#include "HFCLib.h"

CAppData m_AppData;

#ifdef DEF_APP

CWinThread* GetCurrentWinThread()
{
	DWORD nThreadID=GetCurrentThreadId();
	POSITION pPos=GetAppData()->m_pThreads->GetHeadPosition();
	CWinThread* pThread=NULL;
	while (pPos!=NULL)
	{
		pThread=GetAppData()->m_pThreads->GetAt(pPos);
		if (pThread!=NULL)
		{
			if (pThread->GetThreadId()==nThreadID)
				return pThread;
		}
		pPos=GetAppData()->m_pThreads->GetNextPosition(pPos);
	}
	return NULL;
}

CWinThread* GetWinThread(DWORD nThreadID)
{
	POSITION pPos=GetAppData()->m_pThreads->GetHeadPosition();
	CWinThread* pThread=NULL;
	while (pPos!=NULL)
	{
		pThread=GetAppData()->m_pThreads->GetAt(pPos);
		if (pThread!=NULL)
		{
			if (pThread->GetThreadId()==nThreadID)
				return pThread;
		}
		pPos=GetAppData()->m_pThreads->GetNextPosition(pPos);
	}
	return NULL;
}


////////////////////////////
// Class CWinThread
////////////////////////////


CWinThread::CWinThread()
:	m_hThread(NULL),m_pMainWnd(NULL),
	m_nThreadID(0),m_bAutoDelete(FALSE),m_pMouseMessagesTo(NULL)
{
	if (GetAppData()->m_pThreads!=NULL)
		GetAppData()->m_pThreads->AddTail(this);
}

CWinThread::~CWinThread()
{
	if (m_hThread!=NULL)
	{
		CloseHandle(m_hThread);
		DebugCloseThread(m_hThread);

		if (GetAppData()->m_pThreads!=NULL)
		{
			POSITION pPos=GetAppData()->m_pThreads->Find(this);
			if (pPos==NULL)
			{
				DebugMessage("CWinThread::~CWinThread(): Thread not found from m_pThreads");
			}
			else
				GetAppData()->m_pThreads->RemoveAt(pPos);
		}
		else
		{
			DebugMessage("CWinThread::~CWinThread(): m_pThread is NULL");
		}
	}		
	m_hThread=NULL;
}

BOOL CWinThread::TerminateThread(DWORD dwExitCode)
{
	BOOL nRet=::TerminateThread(m_hThread,dwExitCode,TRUE);
	this->ExitInstance();
	return nRet;	
}

BOOL CWinThread::GetExitCodeThread(LPDWORD lpExitCode) const
{
	if (m_hThread==NULL)
		return FALSE;
	return ::GetExitCodeThread(m_hThread,lpExitCode);
}

BOOL CWinThread::IsRunning() const
{
	DWORD status;
	if (m_hThread==NULL)
		return FALSE;
	::GetExitCodeThread(m_hThread,&status);
	if (status==STILL_ACTIVE)
		return TRUE;
	return FALSE;
}

BOOL CWinThread::InitInstance()
{
	m_nThreadID=GetCurrentThreadId();
	return FALSE;
}

int CWinThread::ExitInstance()
{
	if (m_bAutoDelete)
		delete this;
	return TRUE;
}


BOOL CWinThread::OnIdle(LONG lCount)
{
	return FALSE; // More idle time??
}


		
DWORD CWinThread::ModalLoop()
{
	BOOL bIdle=TRUE;
	HACCEL hAccel;
	DWORD nIdleTime=0;
	int nAccel;
	if (m_Accels.GetSize()>=1)
	{
		if (m_pMainWnd!=NULL && m_Accels[0].hWnd==NULL)
		{
			m_Accels[0].hWnd=*m_pMainWnd;
			m_Accels[0].hWndTo=*m_pMainWnd;
			SetAccelTableForChilds(*m_pMainWnd,m_Accels[0].hAccel,TRUE);
		}
		else if (m_pMainWnd==NULL)
			m_Accels.RemoveAll();
	}

	if (IsUnicodeSystem())
	{
		/////////////////////////////////////////////////
		// Unicode modal loop

		while (1)
		{
			while (!::PeekMessageW(&m_currentMessage,NULL,0,0,PM_NOREMOVE) && bIdle)
			{
				if (nIdleTime)
				{
					if (!OnIdle(nIdleTime))
						bIdle=FALSE;
				}
				nIdleTime++;
			}
			while (::PeekMessageW(&m_currentMessage,NULL,0,0,PM_NOREMOVE) || !bIdle)
			{
				if (nIdleTime)
				{
					OnIdle(0);
					nIdleTime=0;
				}
				if (!GetMessageW(&m_currentMessage,NULL,0,0))
					return (DWORD)m_currentMessage.wParam;
				
				// If OnThreadMessage returns true, do nothing
				if (OnThreadMessage(&m_currentMessage))
					continue;
				
				


				switch (m_currentMessage.message)
				{
				case WM_NCDESTROY:
					for (nAccel=0;nAccel<m_Accels.GetSize();nAccel++)
					{
						if (m_Accels[nAccel].hWnd==m_currentMessage.hwnd)
						{
							m_Accels.RemoveAt(nAccel);
							break;
						}
					}
					for (nAccel=0;nAccel<m_Dialogs.GetSize();nAccel++)
					{
						if (m_Dialogs[nAccel]==m_currentMessage.hwnd)
						{
							m_Dialogs.RemoveAt(nAccel);
							break;
						}
					}
					
					TranslateMessage(&m_currentMessage);
					DispatchMessageW(&m_currentMessage);
					break;
				case WM_KEYDOWN:
				case WM_KEYUP:
				case WM_SYSKEYDOWN:
				case WM_SYSKEYUP:
					{
						hAccel=NULL;
						for (nAccel=0;nAccel<m_Accels.GetSize();nAccel++)
						{
							if (m_Accels[nAccel].hWnd==m_currentMessage.hwnd)
							{
								if (m_currentMessage.message==WM_NCDESTROY)
									m_Accels.RemoveAt(nAccel);
								else
									hAccel=m_Accels[nAccel].hAccel;
								break;
							}
						}
						if (hAccel!=NULL)
						{
							if (!TranslateAcceleratorW(m_Accels[nAccel].hWndTo,hAccel,&m_currentMessage))
							{
								// Check dialog messages
								for (nAccel=0;nAccel<m_Dialogs.GetSize();nAccel++)
								{
									if (IsDialogMessageW(m_Dialogs[nAccel],&m_currentMessage))
										break;
								}
								if (nAccel==m_Dialogs.GetSize())
								{			
									TranslateMessage(&m_currentMessage);
									DispatchMessageW(&m_currentMessage);
									
								}
							}
						}
						else
						{
							// Check dialog messages
							for (nAccel=0;nAccel<m_Dialogs.GetSize();nAccel++)
							{
								if (IsDialogMessageW(m_Dialogs[nAccel],&m_currentMessage))
									break;
							}
							if (nAccel==m_Dialogs.GetSize())
							{			
								TranslateMessage(&m_currentMessage);
								DispatchMessageW(&m_currentMessage);
							}
						}
						break;
					}
				default:
					// Check dialog messages
					for (nAccel=0;nAccel<m_Dialogs.GetSize();nAccel++)
					{
						if (IsDialogMessageW(m_Dialogs[nAccel],&m_currentMessage))
							break;
					}
					if (nAccel==m_Dialogs.GetSize())
					{			
						TranslateMessage(&m_currentMessage);
						DispatchMessageW(&m_currentMessage);
					}
					break;

				}
				if (m_currentMessage.message!=WM_PAINT && m_currentMessage.message!=0x0118)
					bIdle=TRUE;
			}
		}
	}
	else
	{
		/////////////////////////////////////////////////
		// ANSI modal loop

		while (1)
		{
			while (!::PeekMessageA(&m_currentMessage,NULL,0,0,PM_NOREMOVE) && bIdle)
			{
				if (nIdleTime)
				{
					if (!OnIdle(nIdleTime))
						bIdle=FALSE;
				}
				nIdleTime++;
			}
			while (::PeekMessageA(&m_currentMessage,NULL,0,0,PM_NOREMOVE) || !bIdle)
			{
				if (nIdleTime)
				{
					OnIdle(0);
					nIdleTime=0;
				}
				if (!GetMessageA(&m_currentMessage,NULL,0,0))
					return (DWORD)m_currentMessage.wParam;
				
				// If OnThreadMessage returns true, do nothing
				if (OnThreadMessage(&m_currentMessage))
					continue;
				
				


				switch (m_currentMessage.message)
				{
				case WM_NCDESTROY:
					for (nAccel=0;nAccel<m_Accels.GetSize();nAccel++)
					{
						if (m_Accels[nAccel].hWnd==m_currentMessage.hwnd)
						{
							m_Accels.RemoveAt(nAccel);
							break;
						}
					}
					for (nAccel=0;nAccel<m_Dialogs.GetSize();nAccel++)
					{
						if (m_Dialogs[nAccel]==m_currentMessage.hwnd)
						{
							m_Dialogs.RemoveAt(nAccel);
							break;
						}
					}
					
					TranslateMessage(&m_currentMessage);
					DispatchMessageA(&m_currentMessage);
					break;
				case WM_KEYDOWN:
				case WM_KEYUP:
				case WM_SYSKEYDOWN:
				case WM_SYSKEYUP:
					{
						hAccel=NULL;
						for (nAccel=0;nAccel<m_Accels.GetSize();nAccel++)
						{
							if (m_Accels[nAccel].hWnd==m_currentMessage.hwnd)
							{
								if (m_currentMessage.message==WM_NCDESTROY)
									m_Accels.RemoveAt(nAccel);
								else
									hAccel=m_Accels[nAccel].hAccel;
								break;
							}
						}
						if (hAccel!=NULL)
						{
							if (!TranslateAcceleratorA(m_Accels[nAccel].hWndTo,hAccel,&m_currentMessage))
							{
								// Check dialog messages
								for (nAccel=0;nAccel<m_Dialogs.GetSize();nAccel++)
								{
									if (IsDialogMessageA(m_Dialogs[nAccel],&m_currentMessage))
										break;
								}
								if (nAccel==m_Dialogs.GetSize())
								{			
									TranslateMessage(&m_currentMessage);
									DispatchMessageA(&m_currentMessage);
								}
							}
						}
						else
						{
							// Check dialog messages
							for (nAccel=0;nAccel<m_Dialogs.GetSize();nAccel++)
							{
								if (IsDialogMessageA(m_Dialogs[nAccel],&m_currentMessage))
									break;
							}
							if (nAccel==m_Dialogs.GetSize())
							{			
								TranslateMessage(&m_currentMessage);
								DispatchMessageA(&m_currentMessage);
							}
						}
						break;
					}
				default:
					// Check dialog messages
					for (nAccel=0;nAccel<m_Dialogs.GetSize();nAccel++)
					{
						if (IsDialogMessageA(m_Dialogs[nAccel],&m_currentMessage))
							break;
					}
					if (nAccel==m_Dialogs.GetSize())
					{			
						TranslateMessage(&m_currentMessage);
						DispatchMessageA(&m_currentMessage);
					}
					break;

				}
				if (m_currentMessage.message!=WM_PAINT && m_currentMessage.message!=0x0118)
					bIdle=TRUE;
			}
		}
	}

	return (DWORD)m_currentMessage.wParam;
}

BOOL CWinThread::OnThreadMessage(MSG* pMsg)
{
	return FALSE;
}

HACCEL CWinThread::GetAccelTable() const
{
	if (m_Accels.GetSize()>=1)
		return m_Accels[0].hAccel;
	return NULL;
}

void CWinThread::SetAccelTable(HACCEL hAccel)
{
	if (m_pMainWnd==NULL)
	{
		m_Accels.RemoveAll();
		m_Accels.Add(CAccelNode(NULL,hAccel,NULL));
	}
	else if ((HWND)*m_pMainWnd!=NULL)
	{
		SetAccelTableForWindow(*m_pMainWnd,hAccel,TRUE);
		SetAccelTableForChilds(*m_pMainWnd,hAccel,TRUE);
	}
	else if (m_Accels.GetSize()==0)
		m_Accels.Add(CAccelNode(NULL,hAccel,NULL));		
}

HACCEL CWinThread::GetAccelTableForWindow(HWND hWnd)
{
	for (int i=0;i<m_Accels.GetSize();i++)
	{
		if (m_Accels[i].hWnd==hWnd)
			return m_Accels[i].hAccel;
	}
	return NULL;
}

BOOL CWinThread::SetAccelTableForWindow(HWND hWnd,HACCEL hTable,BOOL bDontChangeIfExist,HWND hWndTo)
{
	if (hWndTo==(HWND)-1)
		hWndTo=hWnd;
	for (int i=0;i<m_Accels.GetSize();i++)
	{
		if (m_Accels[i].hWnd==hWnd)
		{
			if (bDontChangeIfExist)
				return TRUE;
			if (hTable==NULL)
			{
				m_Accels.RemoveAt(i);
				return TRUE;
			}
			m_Accels[i].hAccel=hTable;
			if (hWndTo!=NULL)
				m_Accels[i].hWndTo=hWndTo;
			return TRUE;
		}
	}
	if (hTable==NULL)
		return FALSE;
	m_Accels.Add(CAccelNode(hWnd,hTable,hWndTo));
	return TRUE;
}

struct EProcData
{
	CWinThread* pThread;
	DWORD dwStyleFlags;
	HACCEL hTable;
	HWND hWndTo;
	BOOL bDontChangeIfExist;
};

static BOOL CALLBACK TableEnumChildProc(HWND hwnd,LPARAM lParam)
{
	EProcData* pData=(EProcData*)lParam;
	if (GetWindowLong(hwnd,GWL_STYLE)&pData->dwStyleFlags)
	{
		pData->pThread->SetAccelTableForWindow(hwnd,pData->hTable,pData->bDontChangeIfExist,pData->hWndTo==NULL?hwnd:pData->hWndTo);
		EnumChildWindows(hwnd,TableEnumChildProc,(LPARAM)pData);
	}
	return TRUE;
}

BOOL CWinThread::SetAccelTableForChilds(HWND hParent,HACCEL hTable,BOOL bDontChangeIfExist,HWND hWndTo,DWORD dwStyleFlags)
{
	EProcData data;
	data.pThread=this;
	data.hTable=hTable;
	data.dwStyleFlags=dwStyleFlags;
	data.bDontChangeIfExist=bDontChangeIfExist;
	if (hWndTo==(HWND)-1)
		data.hWndTo=hParent;
	else
		data.hWndTo=hWndTo;
	return EnumChildWindows(hParent,TableEnumChildProc,(LPARAM)&data);
}

#endif
