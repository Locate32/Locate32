/* Locate32 - Copyright (c) 1997-2010 Janne Huttunen */

#if !defined(SHORTCUTS_INL)
#define SHORTCUTS_INL

#if _MSC_VER >= 1000
#pragma once
#endif

inline CSubAction::CSubAction()
:	m_nControl(FindNow),m_pExtraInfo(NULL)
{
}

inline CSubAction::CSubAction(DWORD nSubAction)
:	m_nSubAction(nSubAction),m_pExtraInfo(NULL)
{
}

inline CSubAction::CSubAction(DWORD nAction,CSubAction& rCopyFrom)
{
	GetCopyFrom(nAction,rCopyFrom);
}


inline CAction::CAction()
:	m_nAction(None)
{
}

inline void CAction::ClearExtraInfo()
{
	CSubAction::ClearExtraInfo(m_nAction);
}

inline CAction::~CAction()
{
	ClearExtraInfo();
}

inline CAction::CAction(CAction& rCopyFrom)
:	m_dwAction(rCopyFrom.m_dwAction)
{
	GetCopyFrom(m_dwAction,rCopyFrom);
}

inline CAction::SendMessageInfo::SendMessageInfo()
:	nMessage(0),szWindow(NULL),szWParam(NULL),szLParam(NULL)
{
}

inline CAction::SendMessageInfo::~SendMessageInfo()
{
	if (szWindow!=NULL)
		delete[] szWindow;
	if (szWParam!=NULL)
		delete[] szWParam;
	if (szLParam!=NULL)
		delete[] szLParam;
}

inline BYTE CShortcut::GetHotkeyModifiers() const
{
    return ModifiersToHotkeyModifiers(m_bModifiers);	
}

inline void CShortcut::SetHotkeyModifiers(BYTE bHotkeyModifier)
{
	m_bModifiers=HotkeyModifiersToModifiers(bHotkeyModifier);
}

inline CShortcut::CShortcut(void* pVoid)
:	m_dwFlags(sfDefault),m_pClass(NULL),m_pTitle(NULL) // This initializes union
{
}


inline CShortcut::~CShortcut()
{
	ClearExtraInfo();
	m_apActions.RemoveAll();
}

inline CSubAction::CSubAction(void* pVoid)
:	m_pExtraInfo(NULL)
{
}

inline CAction::CAction(void* pVoid)
:	CSubAction(pVoid)
{
}

inline DWORD CSubAction::GetDataLength(DWORD nAction,BOOL bHeader) const
{
	DWORD dwLength=(bHeader?sizeof(WORD):0)+2*sizeof(DWORD);
	
	switch (nAction)
	{
	case CAction::ResultListItems:
		if (m_nResultList==Execute && m_szVerb!=NULL)
			dwLength+=(DWORD)(wcslen(m_szVerb)+1)*2;
		else if (m_nResultList==ExecuteCommand && m_szCommand!=NULL)
			dwLength+=(DWORD)(wcslen(m_szCommand)+1)*2;
		break;
	case CAction::Misc:
		if ((m_nMisc==SendMessage || m_nMisc==PostMessage ) &&
			m_pSendMessage!=NULL)
			dwLength+=m_pSendMessage->GetDataLength();
		else if (m_nMisc==ExecuteCommandMisc && m_szCommand!=NULL)
			dwLength+=(DWORD)(wcslen(m_szCommand)+1)*2;
		break;
	case CAction::Presets:
		if (m_szPreset!=NULL)
			dwLength+=(DWORD)(wcslen(m_szPreset)+1)*2;
		break;
	case CAction::ChangeValue:
		if (m_szPreset!=NULL)
			dwLength+=(DWORD)(wcslen(m_szValue)+1)*2;
		break;
	}

	return dwLength;
}

inline DWORD CAction::GetData(BYTE* pData) const
{
	*((WORD*)pData)=0xFFEC;
	*((DWORD*)(pData+sizeof(WORD)))=m_dwAction;
	return CSubAction::GetData(m_dwAction,pData+sizeof(WORD)+sizeof(DWORD),FALSE)+sizeof(WORD)+sizeof(DWORD);
}

inline DWORD CAction::GetDataLength() const
{
	return sizeof(WORD)+sizeof(DWORD)+CSubAction::GetDataLength(m_nAction,FALSE);
}


inline DWORD CAction::SendMessageInfo::GetDataLength() const
{
	DWORD dwLength=sizeof(WORD)+sizeof(DWORD)+3*sizeof(WCHAR); // 3*'\0'

	if (szWindow!=NULL)
		dwLength+=(DWORD)wcslen(szWindow)*2; // '\0' already included
	

	if (szWParam!=NULL)
		dwLength+=(DWORD)wcslen(szWParam)*2; // '\0' already included

	if (szLParam!=NULL)
		dwLength+=(DWORD)wcslen(szLParam)*2; // '\0' already included

	return dwLength;
}

inline BOOL CShortcut::IsModifiersOk(BOOL bAltDown,BOOL bControlDown,BOOL bShiftDown,BOOL bWinDown) const
{
	if (m_bModifiers&ModifierAlt)
	{
		if (!bAltDown) return FALSE;
	}
	else
		if (bAltDown) return FALSE;

	if (m_bModifiers&ModifierControl)
	{
		if (!bControlDown) return FALSE;
	}
	else
		if (bControlDown) return FALSE;

	if (m_bModifiers&ModifierShift)
	{
		if (!bShiftDown) return FALSE;
	}
	else
		if (bShiftDown) return FALSE;

	if (m_bModifiers&ModifierWin)
	{
		if (!bWinDown) return FALSE;
	}
	else
		if (bWinDown) return FALSE;

	return TRUE;
}


#ifndef KEYHOOK_EXPORTS

inline void CShortcut::ExecuteAction()
{
	DebugMessage("CShortcut::ExecuteAction()");
	for (int i=0;i<m_apActions.GetSize();i++)
	{
		if (!m_apActions[i]->ExecuteAction())
			break;
	}
}


inline BOOL CSubAction::DoActivateTab()
{
	if (GetTrayIconWnd()->GetLocateDlgThread()==NULL)
		return FALSE;

	if (m_nActivateTab==PrevTab)
		GetTrayIconWnd()->GetLocateDlgThread()->m_pLocate->OnActivateNextTab(TRUE);
	else if (m_nActivateTab==NextTab)
		GetTrayIconWnd()->GetLocateDlgThread()->m_pLocate->OnActivateNextTab(FALSE);
	else
		GetTrayIconWnd()->GetLocateDlgThread()->m_pLocate->OnActivateTab((int)m_nActivateTab);

	return TRUE;
}

inline void CShortcut::ResolveMnemonics(CArrayFP<CShortcut*>& aShortcuts,HWND* hDialogs)
{
	for (int i=0;i<aShortcuts.GetSize();i++)
	{
		if (aShortcuts[i]->m_dwFlags&CShortcut::sfUseMemonic)
			aShortcuts[i]->m_bVirtualKey=aShortcuts[i]->GetMnemonicForAction(hDialogs);
	}

}

inline void CShortcut::FormatKeyLabel(VirtualKeyName* pVirtualKeyNames,CStringW& str) const
{
    FormatKeyLabel(pVirtualKeyNames,m_bVirtualKey,m_bModifiers,(m_dwFlags&CShortcut::sfVirtualKeyIsScancode)?TRUE:FALSE,str);
}

#endif

#endif