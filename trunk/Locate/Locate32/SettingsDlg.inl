/* Locate32 - Copyright (c) 1997-2010 Janne Huttunen */

#if !defined(SETTINGSDLG_INL)
#define SETTINGSDLG_INL

#if _MSC_VER >= 1000
#pragma once
#endif 




inline BOOL CSettingsProperties::IsSettingsFlagSet(DWORD dwFlags)
{
	return (m_dwSettingsFlags&dwFlags)?TRUE:FALSE;
}

inline BOOL CSettingsProperties::IsAllFlagsSet(DWORD dwFlags)
{
	return (m_dwSettingsFlags&dwFlags)==dwFlags?TRUE:FALSE;
}

inline void CSettingsProperties::SetSettingsFlags(DWORD dwFlags,BOOL bState)
{
	if (bState)
		m_dwSettingsFlags|=dwFlags;
	else
		m_dwSettingsFlags&=~dwFlags;
}

inline void CSettingsProperties::SetSettingsFlags(DWORD dwFlags)
{
	m_dwSettingsFlags|=dwFlags;
}

inline void CSettingsProperties::ClearSettingsFlags(DWORD dwFlags)
{
	m_dwSettingsFlags&=~dwFlags;
}

inline CSettingsProperties::CGeneralSettingsPage::CGeneralSettingsPage()
:	CPropertyPage(IDD_GENERALSETTINGS,IDS_GENERALSETTINGS)
{
	AddFlags(PSP_HASHELP);
}

inline CSettingsProperties::CAdvancedSettingsPage::CAdvancedSettingsPage()
{	
	//DebugMessage("CSettingsProperties::CAdvancedSettingsPage::CAdvancedSettingsPage()"); 

	
	OPTIONPAGE op;
	op.dwFlags=COptionsPropertyPage::OPTIONPAGE::opTemplateIsID|
		COptionsPropertyPage::OPTIONPAGE::opCaptionIsID|
		COptionsPropertyPage::OPTIONPAGE::opChangeIsID;
	
	
	
	op.nIDTemplate=IDD_ADVANCEDSETTINGS;
	op.nIDCaption=IDS_ADVANCEDSETTINGS;
	op.nTreeCtrlID=IDC_SETTINGS;
	op.nIDChangeText=IDS_ADVSETCHANGE;
	
	Construct(&op);


	AddFlags(PSP_HASHELP);

}

inline CSettingsProperties::CLanguageSettingsPage::CLanguageSettingsPage()
:	CPropertyPage(IDD_LANGUAGESETTINGS,IDS_LANGUAGESETTINGS),m_pList(NULL),nLastSel(0)
{
	AddFlags(PSP_HASHELP);
}

inline CSettingsProperties::CDatabasesSettingsPage::CDatabasesSettingsPage()
:	CPropertyPage(IDD_DATABASESETTINGS,IDS_DATABASESETTINGS),m_pList(NULL)
{
	AddFlags(PSP_HASHELP);

	InitializeCriticalSection(&m_csModDateThreads);
}



inline CSettingsProperties::CDatabasesSettingsPage::CDatabaseDialog::CDatabaseDialog()
:	CDialog(IDD_DATABASEDIALOG),m_pList(NULL),m_bDontEditName(FALSE)
{
}


inline CSettingsProperties::CDatabasesSettingsPage::CDatabaseDialog::CAdvancedDialog::CAdvancedDialog(
	LPCWSTR szIncludedFiles,LPCWSTR szIncludedDirectories,
	LPCWSTR szExcludedFiles,const CArrayFAP<LPWSTR>& rDirectories,
	const CListCtrl* pCopyItemsFrom,LPCWSTR szRootMaps,
	BOOL bExcludeOnlyContentOfDirectories)
:	CDialog(IDD_DBADVANCED),
	m_sIncludedFiles(szIncludedFiles),m_sIncludedDirectories(szIncludedDirectories),
	m_sExcludedFiles(szExcludedFiles),m_pDriveList(NULL),
	m_pCopyItemsFrom(pCopyItemsFrom),m_sRootMaps(szRootMaps),
	m_bExcludeOnlyContentOfDirectories(bExcludeOnlyContentOfDirectories)
{
	for (int i=0;i<rDirectories.GetSize();i++)
		m_aExcludedDirectories.Add(alloccopy(rDirectories[i]));
}

inline CSettingsProperties::CAutoUpdateSettingsPage::CAutoUpdateSettingsPage()
:	CPropertyPage(IDD_AUTOUPDATESETTINGS,IDS_AUTOUPDATESETTINGS)
{
	AddFlags(PSP_HASHELP);
}

inline CSettingsProperties::CAutoUpdateSettingsPage::CCheduledUpdateDlg::CCheduledUpdateDlg()
:	CDialog(IDD_SCHEDULEDUPDATE),m_pTypeCombo(NULL),m_pSchedule(NULL),m_bChanged(FALSE)
{
}



inline CSettingsProperties::CKeyboardShortcutsPage::CAdvancedDlg::CAdvancedDlg(CShortcut* pShortcut)
:	CDialog(IDD_SHORTCUTADVANCED),m_pShortcut(pShortcut)
{
}

inline void CSettingsProperties::CKeyboardShortcutsPage::RefreshShortcutListLabels()
{
	int nItem=m_pList->GetNextItem(-1,LVNI_SELECTED);
	m_pList->RedrawItems(nItem,nItem);
}

inline void CSettingsProperties::CKeyboardShortcutsPage::SetHotKey(BYTE bKey,BYTE bModifiers)
{
	SendDlgItemMessage(IDC_SHORTCUTKEY,HKM_SETHOTKEY,MAKEWORD(bKey,bModifiers),0);
	
	// Check whether MOD_EXT is needed
	if (LOBYTE(SendDlgItemMessage(IDC_SHORTCUTKEY,HKM_GETHOTKEY))!=bKey)
		SendDlgItemMessage(IDC_SHORTCUTKEY,HKM_SETHOTKEY,MAKEWORD(bKey,bModifiers|HOTKEYF_EXT),0);
}

inline void CSettingsProperties::CKeyboardShortcutsPage::SetHotKeyForShortcut(CShortcut* pShortcut)
{
	return SetHotKey(pShortcut->m_bVirtualKey,pShortcut->GetHotkeyModifiers());
}

inline void CSettingsProperties::CKeyboardShortcutsPage::GetHotKeyForShortcut(CShortcut* pShortcut) const
{
	// Using hotkey control
	WORD wKey=(WORD)SendDlgItemMessage(IDC_SHORTCUTKEY,HKM_GETHOTKEY,0,0);
	pShortcut->m_bVirtualKey=LOBYTE(wKey);
    pShortcut->SetHotkeyModifiers(HIBYTE(wKey));
}

inline CAction::Action CSettingsProperties::CKeyboardShortcutsPage::GetSelectedAction() const
{
	int nAction=(CAction::Action)SendDlgItemMessage(IDC_ACTION,CB_GETCURSEL,0,0);
	if ((INT)nAction==CB_ERR || nAction==0)
		return CAction::None;
	
	return (CAction::Action)(nAction-1);
}
#endif