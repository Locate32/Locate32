/* Locate32 - Copyright (c) 1997-2010 Janne Huttunen */

#include <HFCLib.h>
#include "Locate32.h"

inline BOOL operator==(const SYSTEMTIME& s1,const SYSTEMTIME& s2)
{
	return (s1.wYear==s2.wYear && s1.wMonth==s2.wMonth && s1.wDay==s2.wDay &&
		s1.wHour==s2.wHour && s1.wMinute==s2.wMinute && s1.wSecond==s2.wSecond &&
		s1.wMilliseconds==s2.wMilliseconds);
}

inline BOOL operator!=(const SYSTEMTIME& s1,const SYSTEMTIME& s2)
{
	return !(s1.wYear==s2.wYear && s1.wMonth==s2.wMonth && s1.wDay==s2.wDay &&
		s1.wHour==s2.wHour && s1.wMinute==s2.wMinute && s1.wSecond==s2.wSecond &&
		s1.wMilliseconds==s2.wMilliseconds);
}



////////////////////////////////////////
// CSettingsProperties
////////////////////////////////////////

CSettingsProperties::CSettingsProperties(HWND hParent)
:	CPropertySheet(IDS_SETTINGS,hParent,0),
	m_nMaximumFoundFiles(0),
	m_nInstantSearchingLimit(DEFAULT_INSTANTSEARCHLIMIT),
	m_nInstantSearchingDelay(DEFAULT_INSTANTSEARCHDELAY),
	m_nInstantSearchingChars(DEFAULT_INSTANTSEARCHCHARS),
	m_nUpdateThreadPriority(THREAD_PRIORITY_NORMAL),
	m_dwLocateDialogFlags(CLocateDlg::fgDefault),
	m_dwLocateDialogExtraFlags(CLocateDlg::efDefault),
	m_dwInstantSearchingFlags(CLocateDlg::isDefault),
	m_dwThumbnailFlags(CLocateDlg::tfDefault),
	m_dwThumbnailLargeIconSize(DEFAULT_LARGEICONSIZE),
	m_dwThumbnailExtraLargeIconSize(DEFAULT_EXTRALARGEICONSIZE),
	m_bDefaultFlag(defaultDefault),m_bSorting(BYTE(-1)),
	m_dwSettingsFlags(settingsDefault),
	m_nNumberOfDirectories(DEFAULT_NUMBEROFDIRECTORIES),
	m_nNumberOfNames(DEFAULT_NUMBEROFNAMES),
	m_nNumberOfTypes(DEFAULT_NUMBEROFTYPES),
	m_nTransparency(0),m_nToolTipTransparency(0),
	m_dwTooltipDelayAutopop(DWORD(-1)),
	m_dwTooltipDelayInitial(DWORD(-1)),
	m_dwSchedulesDelay(0),
	m_dwSortingMethod(DEFAULT_SORTINGMETHOD)
{
	AddFlags(PSH_NOAPPLYNOW);

#ifdef _DEBUG
	AddFlags(PSH_HASHELP);
#endif

	m_pGeneral=new CGeneralSettingsPage;
	m_pAdvanced=new CAdvancedSettingsPage;
	m_pLanguage=new CLanguageSettingsPage;
	m_pDatabases=new CDatabasesSettingsPage;
	m_pAutoUpdate=new CAutoUpdateSettingsPage;
	m_pKeyboardShortcuts=new CKeyboardShortcutsPage;
	
	
	AddPage((CPropertyPage*)m_pGeneral);
	AddPage((CPropertyPage*)m_pAdvanced);
	AddPage((CPropertyPage*)m_pLanguage);
	AddPage((CPropertyPage*)m_pDatabases);
	if (GetLocateApp()->m_nInstance==0)
		AddPage((CPropertyPage*)m_pAutoUpdate);
	AddPage((CPropertyPage*)m_pKeyboardShortcuts);
	
	
	m_pGeneral->m_pSettings=m_pAdvanced->m_pSettings=this;
	m_pLanguage->m_pSettings=m_pAutoUpdate->m_pSettings=this;
	m_pDatabases->m_pSettings=m_pKeyboardShortcuts->m_pSettings=this;



	
	int nDeviceCaps;
	{
		// Getting device caps
		HDC hScreenDC=::GetDC(NULL);
		nDeviceCaps=::GetDeviceCaps(hScreenDC,LOGPIXELSY);
		::ReleaseDC(NULL,hScreenDC);
	}
	ZeroMemory(&m_lResultListFont,sizeof(LOGFONT));
	m_lResultListFont.lfHeight=-MulDiv(9, nDeviceCaps, 72);
	m_lResultListFont.lfWeight=FW_NORMAL;
	StringCbCopy(m_lResultListFont.lfFaceName,LF_FACESIZE,"Tahoma");
	

	CTrayIconWnd::CUpdateStatusWnd::FillFontStructs(&m_lToolTipTextFont,&m_lToolTipTitleFont);



	m_cToolTipBackColor=GetSysColor(COLOR_INFOBK);
	m_cToolTipTextColor=GetSysColor(COLOR_INFOTEXT);
	m_cToolTipTitleColor=GetSysColor(COLOR_INFOTEXT);
	m_cToolTipErrorColor=GetSysColor(COLOR_INFOTEXT);


	// Set default for AddRefFix
	SetSettingsFlags(settingsEnableAddRefFix,GetSystemFeaturesFlag()&efWinVista?FALSE:TRUE);

}

CSettingsProperties::~CSettingsProperties()
{
	delete m_pGeneral;
	delete m_pAdvanced;
	delete m_pLanguage;
	delete m_pDatabases;
	delete m_pAutoUpdate;
	delete m_pKeyboardShortcuts;
	
	m_Schedules.RemoveAll();


}

BOOL CSettingsProperties::LoadSettings()
{
	SdDebugMessage("CSettingsProperties::LoadSettings()");
	
	CRegKey GenRegKey;
	CRegKey2 LocRegKey;
	
	m_DateFormat=((CLocateApp*)GetApp())->m_strDateFormat;
	m_TimeFormat=((CLocateApp*)GetApp())->m_strTimeFormat;
	m_nFileSizeFormat=((CLocateApp*)GetApp())->m_nFileSizeFormat;
	
	// GetTrayIconWnd() is alwyas present
	m_dwProgramFlags=CLocateApp::GetProgramFlags();

	if (GetLocateDlg()!=NULL)
	{
		m_dwLocateDialogFlags=GetLocateDlg()->GetFlags();
		m_dwLocateDialogExtraFlags=GetLocateDlg()->GetExtraFlags();
		m_dwInstantSearchingFlags=GetLocateDlg()->GetInstantSearchingFlags();
		m_dwThumbnailFlags=GetLocateDlg()->GetThumbnailFlags();

		// read efLVNoUpdateWhileSorting from registry
		if (LocRegKey.OpenKey(HKCU,"\\General",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
		{
			DWORD temp=m_dwLocateDialogExtraFlags;
			LocRegKey.QueryValue("Program StatusExtra",temp);
			m_dwLocateDialogExtraFlags&=~CLocateDlg::efLVNoUpdateWhileSorting;
			m_dwLocateDialogExtraFlags|=temp&CLocateDlg::efLVNoUpdateWhileSorting;
		}
	}
	else if (LocRegKey.OpenKey(HKCU,"\\General",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
	{
		DWORD temp=m_dwLocateDialogFlags;
		LocRegKey.QueryValue("Program Status",temp);
		m_dwLocateDialogFlags&=~CLocateDlg::fgSave;
		m_dwLocateDialogFlags|=temp&CLocateDlg::fgSave;

		temp=m_dwLocateDialogExtraFlags;
		LocRegKey.QueryValue("Program StatusExtra",temp);
		m_dwLocateDialogExtraFlags&=~CLocateDlg::efSave;
		m_dwLocateDialogExtraFlags|=temp&CLocateDlg::efSave;

		temp=m_dwInstantSearchingFlags;
		LocRegKey.QueryValue("Instant Searching",temp);
		m_dwInstantSearchingFlags&=~CLocateDlg::isSave;
		m_dwInstantSearchingFlags|=temp&CLocateDlg::isSave;

		temp=m_dwThumbnailFlags;
		LocRegKey.QueryValue("Thumbnail Flags",temp);
		m_dwThumbnailFlags&=~CLocateDlg::tfSave;
		m_dwThumbnailFlags|=temp&CLocateDlg::tfSave;
	}

	if (LocRegKey.OpenKey(HKCU,"\\Recent Strings",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
	{
		LocRegKey.QueryValue("NumberOfDirectories",m_nNumberOfDirectories);
		LocRegKey.QueryValue("NumberOfNames",m_nNumberOfNames);
		LocRegKey.QueryValue("NumberOfTypes",m_nNumberOfTypes);
	}

	// Initializing values
	if (GetLocateDlg()==NULL)
	{
		if (LocRegKey.OpenKey(HKCU,"\\Locate",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
		{
			LocRegKey.QueryValue("MaximumFoundFiles",m_nMaximumFoundFiles);
			LocRegKey.QueryValue("Instant Search Limit",m_nInstantSearchingLimit);
			LocRegKey.QueryValue("Instant Search Delay",m_nInstantSearchingDelay);
			LocRegKey.QueryValue("Instant Search Chars",m_nInstantSearchingChars);
		}
	}
	else
	{
		m_nMaximumFoundFiles=GetLocateDlg()->GetMaxFoundFiles();
		m_nInstantSearchingLimit=GetLocateDlg()->GetInstantSearchingLimit();
		m_nInstantSearchingDelay=GetLocateDlg()->GetInstantSearchingDelay();
		m_nInstantSearchingChars=GetLocateDlg()->GetInstantSearchingChars();
	}

	if (LocRegKey.OpenKey(HKCU,"\\Locate",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
	{
		LocRegKey.QueryValue("Sorting Method",m_dwSortingMethod);
	}
    	
	
	// Retrieving databases
	m_aDatabases.RemoveAll();
	const CArray<PDATABASE>& rOrigDatabases=GetLocateApp()->GetDatabases();
	for (int i=0;i<rOrigDatabases.GetSize();i++)
		m_aDatabases.Add(new CDatabase(*rOrigDatabases[i]));
	
	SetSettingsFlags(settingsDatabasesOverridden,
		GetLocateApp()->GetStartupFlags()&CLocateApp::CStartData::startupDatabasesOverridden);
	
	
	// Loading shortcuts
	m_aShortcuts.RemoveAll();
	if (!CShortcut::LoadShortcuts(m_aShortcuts))
	{
        m_aShortcuts.RemoveAll();
		if (!CShortcut::GetDefaultShortcuts(m_aShortcuts))
		{
			ShowErrorMessage(IDS_ERRORCANNOTLOADDEFAULTSHORTUCS,IDS_ERROR);
			m_aShortcuts.RemoveAll();
		}		
	}

	
	if (GetLocateApp()->m_nInstance==0)
	{
		// Loading schedules
		POSITION pPos=GetTrayIconWnd()->m_Schedules.GetHeadPosition();
		while (pPos!=NULL)
		{
			m_Schedules.AddTail(new CSchedule(GetTrayIconWnd()->m_Schedules.GetAt(pPos)));
			pPos=GetTrayIconWnd()->m_Schedules.GetNextPosition(pPos);
		}	
	}

	// Loading some general settings
	if (LocRegKey.OpenKey(HKCU,"\\General",CRegKey::defRead)==ERROR_SUCCESS)
	{
		m_bDefaultFlag=0;
		DWORD nTemp=1;
		LocRegKey.QueryValue("Default CheckIn",nTemp);
		m_bDefaultFlag|=nTemp;
		SendDlgItemMessage(IDC_CHECKIN,CB_SETCURSEL,nTemp);
		
		nTemp=0;
		LocRegKey.QueryValue("Default MatchWholeName",nTemp);
		if (nTemp) m_bDefaultFlag|=defaultWholeName;
		
		nTemp=0;
		LocRegKey.QueryValue("Default MatchCase",nTemp);
		if (nTemp) m_bDefaultFlag|=defaultMatchCase;
		
		nTemp=1;
		LocRegKey.QueryValue("Default DataMatchCase",nTemp);
		if (nTemp) m_bDefaultFlag|=defaultDataMatchCase;
		
		nTemp=0;
		LocRegKey.QueryValue("Default ReplaceSpaces",nTemp);
		if (nTemp) m_bDefaultFlag|=defaultReplaceSpaces;

		nTemp=0;
		LocRegKey.QueryValue("Default UseWholePath",nTemp);
		if (nTemp) m_bDefaultFlag|=defaultUseWholePath;


		if (LocRegKey.QueryValue("Default Sorting",nTemp))
			m_bSorting=(BYTE)nTemp;		


		// Overrinding explorer for opening folders
		LocRegKey.QueryValue("Use other program to open folders",nTemp);
		SetSettingsFlags(settingsUseOtherProgramsToOpenFolders,nTemp);
		LocRegKey.QueryValue(L"Open folders with",m_OpenFoldersWith);

		if (LocRegKey.QueryValue("Transparency100",nTemp))
			m_nTransparency=min(nTemp,100);
		else if (LocRegKey.QueryValue("Transparency",nTemp))
			m_nTransparency=(min(nTemp,255)*100)/255;


		if (LocRegKey.QueryValue("TooltipDelayAutopop",m_dwTooltipDelayAutopop))
			m_dwSettingsFlags|=settingsSetTooltipDelays;
		if (LocRegKey.QueryValue("TooltipDelayInitial",m_dwTooltipDelayInitial))
			m_dwSettingsFlags|=settingsSetTooltipDelays;

		if (LocRegKey.QueryValue("Update Process Priority",nTemp))
			m_nUpdateThreadPriority=nTemp;


		if (LocRegKey.QueryValue("ResultListFont",(LPSTR)&m_lResultListFont,sizeof(LOGFONT))==sizeof(LOGFONT))
			m_dwSettingsFlags|=settingsUseCustomResultListFont;


		if (LocRegKey.QueryValue(L"CustomTrayIcon",m_CustomTrayIcon))
			m_dwSettingsFlags|=settingsUseCustomTrayIcon;

		if (LocRegKey.QueryValue(L"CustomDialogIcon",m_CustomDialogIcon))
			m_dwSettingsFlags|=settingsUseCustomDialogIcon;

		nTemp=0;
		LocRegKey.QueryValue("SimpleFileMenu",nTemp);
		if (nTemp)
			m_dwSettingsFlags|=settingsSimpleFileMenu;


		LocRegKey.QueryValue("Thumbnail Large Icon Size",m_dwThumbnailLargeIconSize);
		LocRegKey.QueryValue("Thumbnail Extra Large Icon Size",m_dwThumbnailExtraLargeIconSize);
		

		LocRegKey.QueryValue("Schedules Delay",m_dwSchedulesDelay);


		if (LocRegKey.QueryValue("EnableAddRefFix",nTemp))
			SetSettingsFlags(settingsEnableAddRefFix,nTemp);
	}

	// m_bAdvancedAndContextMenuFlag
	m_bAdvancedAndContextMenuFlag=0;
	if (GenRegKey.OpenKey(HKCR,"CLSID\\{20D04FE0-3AEA-1069-A2D8-08002B30309D}\\shell\\locate",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
		m_bAdvancedAndContextMenuFlag|=cmLocateOnMyComputer;
	if (GenRegKey.OpenKey(HKCR,"CLSID\\{450D8FBA-AD25-11D0-98A8-0800361B1103}\\shell\\locate",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
		m_bAdvancedAndContextMenuFlag|=cmLocateOnMyDocuments;
	if (GenRegKey.OpenKey(HKCR,"Drive\\shell\\locate",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
		m_bAdvancedAndContextMenuFlag|=cmLocateOnDrives;
	if (GenRegKey.OpenKey(HKCR,"Directory\\shell\\locate",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
		m_bAdvancedAndContextMenuFlag|=cmLocateOnFolders;
	if (GenRegKey.OpenKey(HKCR,"CLSID\\{20D04FE0-3AEA-1069-A2D8-08002B30309D}\\shell\\updatedb",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
		m_bAdvancedAndContextMenuFlag|=cmUpdateOnMyComputer;
	
	
	// m_strLanguage
	if (LocRegKey.OpenKey(HKCU,"",
		CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
	{
		LocRegKey.QueryValue(L"Language",m_strLangFile);
		LocRegKey.CloseKey();
	}
	if (m_strLangFile.IsEmpty())
	{
		m_strLangFile=L"lan_en.dll";
		SetSettingsFlags(settingsUseLanguageWithConsoleApps);
	}
	else if (GenRegKey.OpenKey(HKCU,CLocateApp::GetRegKey(""),
		CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
	{
		CStringW tmp;
		GenRegKey.QueryValue(L"Language",tmp);
		SetSettingsFlags(settingsUseLanguageWithConsoleApps,tmp.CompareNoCase(m_strLangFile)==0);
		GenRegKey.CloseKey();
	}
		
	// Checking wheter locate is runned at system startup
	{
		WCHAR szPath[MAX_PATH];
		if (GetStartupPath(szPath))
		{
			int nLen=istrlen(szPath);
			if (szPath[nLen-1]!='\\')
				szPath[nLen++]='\\';
			wcscpy_s(szPath+nLen,MAX_PATH-nLen,L"Locate32 Autorun.lnk");
	
			SetSettingsFlags(settingsStartLocateAtStartup,FileSystem::IsFile(szPath));
			
		}
	}
	
	// Update status tooltip
	if (LocRegKey.OpenKey(HKCU,"\\Dialogs\\Updatestatus",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
	{
		DWORD dwTemp;
		if (LocRegKey.QueryValue("Transparency100",dwTemp))
			m_nToolTipTransparency=min(dwTemp,255);
		else if (LocRegKey.QueryValue("Transparency",dwTemp))
			m_nToolTipTransparency=(min(dwTemp,255)*100)/255;
		
			
		if (LocRegKey.QueryValue("TextColor",dwTemp))
			m_cToolTipTextColor=dwTemp;
		if (LocRegKey.QueryValue("TitleColor",dwTemp))
			m_cToolTipTitleColor=dwTemp;
		if (LocRegKey.QueryValue("ErrorColor",dwTemp))
			m_cToolTipErrorColor=dwTemp;
		if (LocRegKey.QueryValue("BackColor",dwTemp))
			m_cToolTipBackColor=dwTemp;

	
		if (LocRegKey.QueryValue("TextFont",(LPSTR)&m_lToolTipTextFont,sizeof(LOGFONT))<sizeof(LOGFONT))
			CTrayIconWnd::CUpdateStatusWnd::FillFontStructs(&m_lToolTipTextFont,NULL);
		
		if (LocRegKey.QueryValue("TitleFont",(LPSTR)&m_lToolTipTitleFont,sizeof(LOGFONT))<sizeof(LOGFONT))
			CTrayIconWnd::CUpdateStatusWnd::FillFontStructs(NULL,&m_lToolTipTitleFont);
		
	}

	// Retrieve tooltip default times
	if (m_dwTooltipDelayAutopop==DWORD(-1))
		m_dwTooltipDelayAutopop=GetDoubleClickTime()*10;
	if (m_dwTooltipDelayInitial==DWORD(-1))
		m_dwTooltipDelayInitial=GetDoubleClickTime();



	// Update status tooltip
	if (LocRegKey.OpenKey(HKCU,"\\Misc",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
	{
		DWORD dwTemp=0;;
		LocRegKey.QueryValue("NoExtensionInRename",dwTemp);
		if (dwTemp)
			m_dwSettingsFlags|=settingsDontShowExtensionInRenameDialog;
	}
			
	
	return TRUE;
}

BOOL CSettingsProperties::GetStartupPath(LPWSTR szPath)
{
	if (GetSystemFeaturesFlag()&efWin2000)
	{
		HRESULT (STDAPICALLTYPE* pSHGetFolderPathW)(HWND,int,HANDLE,DWORD,LPWSTR)=
			(HRESULT (STDAPICALLTYPE*)(HWND,int,HANDLE,DWORD,LPWSTR))GetProcAddress(GetModuleHandle("shell32.dll"),"SHGetFolderPathW");
		if (pSHGetFolderPathW!=NULL)
		{
			if (pSHGetFolderPathW(*this,CSIDL_STARTUP,NULL,SHGFP_TYPE_CURRENT,szPath)==S_OK)
				return TRUE;
		}
	}


	CRegKey GenRegKey;
	if (GenRegKey.OpenKey(HKCU,"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
	{
		CStringW Path;
		if (GenRegKey.QueryValue(L"Startup",szPath,MAX_PATH)>1)
			return TRUE;
	}

	return FALSE;
}

BOOL CSettingsProperties::SaveSettings()
{
	SdDebugMessage("CSettingsProperties::SaveSettings()");
	
	CRegKey GenRegKey;
	CRegKey2 LocRegKey;

	if (LocRegKey.OpenKey(HKCU,"\\General",CRegKey::createNew|CRegKey::samAll)==ERROR_SUCCESS)
	{
		LocRegKey.SetValue("Program Status",m_dwLocateDialogFlags&CLocateDlg::fgSave);
		LocRegKey.SetValue("Program StatusExtra",m_dwLocateDialogExtraFlags&CLocateDlg::efSave);
		LocRegKey.SetValue("Instant Searching",m_dwInstantSearchingFlags&CLocateDlg::isSave);
		LocRegKey.SetValue("Thumbnail Flags",m_dwThumbnailFlags&CLocateDlg::tfSave);
		LocRegKey.SetValue("General Flags",m_dwProgramFlags&CLocateApp::pfSave);
		
		LocRegKey.SetValue(L"DateFormat",m_DateFormat);
		LocRegKey.SetValue(L"TimeFormat",m_TimeFormat);

		LocRegKey.SetValue("Thumbnail Large Icon Size",m_dwThumbnailLargeIconSize);
		LocRegKey.SetValue("Thumbnail Extra Large Icon Size",m_dwThumbnailExtraLargeIconSize);
		

		// Default flags
		LocRegKey.SetValue("Default CheckIn",m_bDefaultFlag&defaultCheckInFlag);
		LocRegKey.SetValue("Default MatchWholeName",m_bDefaultFlag&defaultWholeName?1:0);
		LocRegKey.SetValue("Default MatchCase",m_bDefaultFlag&defaultMatchCase?1:0);
		LocRegKey.SetValue("Default DataMatchCase",m_bDefaultFlag&defaultDataMatchCase?1:0);
		LocRegKey.SetValue("Default ReplaceSpaces",m_bDefaultFlag&defaultReplaceSpaces?1:0);
		LocRegKey.SetValue("Default UseWholePath",m_bDefaultFlag&defaultUseWholePath?1:0);
		LocRegKey.SetValue("Default Sorting",DWORD(m_bSorting));
			
		// Overrinding explorer for opening folders
		LocRegKey.SetValue("Use other program to open folders",(DWORD)IsSettingsFlagSet(settingsUseOtherProgramsToOpenFolders));
		LocRegKey.SetValue(L"Open folders with",m_OpenFoldersWith);

		LocRegKey.SetValue("Transparency100",m_nTransparency);
		LocRegKey.DeleteValue("Transparency");

		if (m_dwSettingsFlags&settingsSetTooltipDelays)
		{
			LocRegKey.SetValue("TooltipDelayAutopop",m_dwTooltipDelayAutopop);
			LocRegKey.SetValue("TooltipDelayInitial",m_dwTooltipDelayInitial);
		}
		else
		{
			LocRegKey.DeleteValue("TooltipDelayAutopop");
			LocRegKey.DeleteValue("TooltipDelayInitial");
		}


		LocRegKey.SetValue("Update Process Priority",(DWORD)m_nUpdateThreadPriority);




		if (m_dwSettingsFlags&settingsUseCustomResultListFont)
			LocRegKey.SetValue("ResultListFont",(LPSTR)&m_lResultListFont,sizeof(LOGFONT));
		else
			LocRegKey.DeleteValue("ResultListFont");

		if (m_dwSettingsFlags&settingsUseCustomTrayIcon)
			LocRegKey.SetValue(L"CustomTrayIcon",m_CustomTrayIcon);
		else
			LocRegKey.DeleteValue("CustomTrayIcon");

		if (m_dwSettingsFlags&settingsUseCustomDialogIcon)
			LocRegKey.SetValue(L"CustomDialogIcon",m_CustomDialogIcon);
		else
			LocRegKey.DeleteValue("CustomDialogIcon");

		LocRegKey.SetValue(L"SimpleFileMenu",m_dwSettingsFlags&settingsSimpleFileMenu?DWORD(1):DWORD(0));
		
		LocRegKey.SetValue("Schedules Delay",m_dwSchedulesDelay);


		LocRegKey.SetValue("EnableAddRefFix",(DWORD)IsSettingsFlagSet(settingsEnableAddRefFix));
	}


	((CLocateApp*)GetApp())->m_strDateFormat=m_DateFormat;
	((CLocateApp*)GetApp())->m_strTimeFormat=m_TimeFormat;
	((CLocateApp*)GetApp())->m_nFileSizeFormat=m_nFileSizeFormat;

	if (LocRegKey.OpenKey(HKCU,"\\Recent Strings",CRegKey::createNew|CRegKey::samAll)==ERROR_SUCCESS)
	{
		LocRegKey.SetValue("NumberOfDirectories",m_nNumberOfDirectories);
		LocRegKey.SetValue("NumberOfNames",m_nNumberOfNames);
		LocRegKey.SetValue("NumberOfTypes",m_nNumberOfTypes);
	}

	if (LocRegKey.OpenKey(HKCU,"\\Locate",CRegKey::createNew|CRegKey::samAll)==ERROR_SUCCESS)
	{
		LocRegKey.SetValue("MaximumFoundFiles",m_nMaximumFoundFiles);
		LocRegKey.SetValue("Instant Search Limit",m_nInstantSearchingLimit);
		LocRegKey.SetValue("Instant Search Delay",m_nInstantSearchingDelay);
		LocRegKey.SetValue("Instant Search Chars",m_nInstantSearchingChars);
		LocRegKey.SetValue("Sorting Method",m_dwSortingMethod);
	}

	if (GetLocateDlg()!=NULL)
	{
		GetLocateDlg()->SetMaxFoundFiles(m_nMaximumFoundFiles);
		GetLocateDlg()->SetInstantSearchLimit(m_nInstantSearchingLimit);
		GetLocateDlg()->SetInstantSearchDelay(m_nInstantSearchingDelay);
		GetLocateDlg()->SetInstantSearchChars(m_nInstantSearchingChars);
		GetLocateDlg()->m_NameDlg.ChangeNumberOfItemsInLists(m_nNumberOfNames,m_nNumberOfTypes,m_nNumberOfDirectories);
	}

	
	// Settings databases
	CDatabase::CheckIDs(m_aDatabases);
	if (!IsSettingsFlagSet(settingsDatabasesOverridden))
	{
		GetLocateApp()->SetDatabases(m_aDatabases);
		CDatabase::SaveToRegistry(HKCU,CLocateApp::GetRegKey("Databases"),GetLocateApp()->GetDatabases());

		GetLocateApp()->ClearStartupFlag(CLocateApp::CStartData::startupDatabasesOverridden);
	}
	else
		GetLocateApp()->SetDatabases(m_aDatabases);

	// Save shortcuts
	if (!CShortcut::SaveShortcuts(m_aShortcuts))
	{
		ShowErrorMessage(IDS_ERRORCANNOTSAVESHORTCUTS,IDS_ERROR);
	}
	GetTrayIconWnd()->PostMessage(WM_RESETSHORTCUTS);
	if (GetLocateDlg()!=NULL)
		GetLocateDlg()->PostMessage(WM_RESETSHORTCUTS);
	
    
	
	// Insert/Remove context menu items
	
	// Locate... on My Computer
	if (GenRegKey.OpenKey(HKCR,"CLSID\\{20D04FE0-3AEA-1069-A2D8-08002B30309D}\\shell\\locate",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
	{
		GenRegKey.CloseKey();
		if ((m_bAdvancedAndContextMenuFlag&cmLocateOnMyComputer)==0)
			CRegKey::DeleteKey(HKCR,"CLSID\\{20D04FE0-3AEA-1069-A2D8-08002B30309D}\\shell\\locate");
	}
	else if (m_bAdvancedAndContextMenuFlag&cmLocateOnMyComputer)
	{
		if (GenRegKey.OpenKey(HKCR,"CLSID\\{20D04FE0-3AEA-1069-A2D8-08002B30309D}\\shell\\locate",CRegKey::createNew|CRegKey::samAll)==ERROR_SUCCESS)
		{
			GenRegKey.SetValue(L"",ID2W(IDS_EXPLORERLOCATE));
			CRegKey CommandKey;
			if (CommandKey.OpenKey(GenRegKey,"command",CRegKey::createNew|CRegKey::samAll)==ERROR_SUCCESS)
			{
				CStringW command;
				command.Format(L"\"%s\" /P4",(LPCWSTR)GetApp()->GetExeNameW());
				CommandKey.SetValue(L"",command);
			}
		}
	}
	
	// Locate... on My Documents
	if (GenRegKey.OpenKey(HKCR,"CLSID\\{450D8FBA-AD25-11D0-98A8-0800361B1103}\\shell\\locate",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
	{
		GenRegKey.CloseKey();
		if ((m_bAdvancedAndContextMenuFlag&cmLocateOnMyDocuments)==0)
			CRegKey::DeleteKey(HKCR,"CLSID\\{450D8FBA-AD25-11D0-98A8-0800361B1103}\\shell\\locate");
	}
	else if (m_bAdvancedAndContextMenuFlag&cmLocateOnMyDocuments)
	{
		if (GenRegKey.OpenKey(HKCR,"CLSID\\{450D8FBA-AD25-11D0-98A8-0800361B1103}\\shell\\locate",CRegKey::createNew|CRegKey::samAll)==ERROR_SUCCESS)
		{
			GenRegKey.SetValue(L"",ID2W(IDS_EXPLORERLOCATE));
			CRegKey CommandKey;
			if (CommandKey.OpenKey(GenRegKey,"command",CRegKey::createNew|CRegKey::samAll)==ERROR_SUCCESS)
			{
				CStringW command;
				command.Format(L"\"%s\" /P3",(LPCWSTR)GetApp()->GetExeNameW());
				CommandKey.SetValue(L"",command);
			}
		}
	}

	// Locate... on Drives
	if (GenRegKey.OpenKey(HKCR,"Drive\\shell\\locate",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
	{
		GenRegKey.CloseKey();
		if ((m_bAdvancedAndContextMenuFlag&cmLocateOnDrives)==0)
			CRegKey::DeleteKey(HKCR,"Drive\\shell\\locate");
	}
	else if (m_bAdvancedAndContextMenuFlag&cmLocateOnDrives)
	{
		if (GenRegKey.OpenKey(HKCR,"Drive\\shell\\locate",CRegKey::createNew|CRegKey::samAll)==ERROR_SUCCESS)
		{
			GenRegKey.SetValue(L"",ID2W(IDS_EXPLORERLOCATE));
			CRegKey CommandKey;
			if (CommandKey.OpenKey(GenRegKey,"command",CRegKey::createNew|CRegKey::samAll)==ERROR_SUCCESS)
			{
				CStringW command;
				command.Format(L"\"%s\" /p \"%%1\"",(LPCWSTR)GetApp()->GetExeNameW());
				CommandKey.SetValue(L"",command);
			}
		}
	}

	// Locate... on Directories
	if (GenRegKey.OpenKey(HKCR,"Directory\\shell\\locate",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
	{
		GenRegKey.CloseKey();
		if ((m_bAdvancedAndContextMenuFlag&cmLocateOnFolders)==0)
			CRegKey::DeleteKey(HKCR,"Directory\\shell\\locate");
	}
	else if (m_bAdvancedAndContextMenuFlag&cmLocateOnFolders)
	{
		if (GenRegKey.OpenKey(HKCR,"Directory\\shell\\locate",CRegKey::createNew|CRegKey::samAll)==ERROR_SUCCESS)
		{
			GenRegKey.SetValue(L"",ID2W(IDS_EXPLORERLOCATE));
			CRegKey CommandKey;
			if (CommandKey.OpenKey(GenRegKey,"command",CRegKey::createNew|CRegKey::samAll)==ERROR_SUCCESS)
			{
				CStringW command;
				command.Format(L"\"%s\" /p \"%%1\"",(LPCWSTR)GetApp()->GetExeNameW());
				CommandKey.SetValue(L"",command);
			}
		}
	}

	// Update Database... on My Computer
	if (GenRegKey.OpenKey(HKCR,"CLSID\\{20D04FE0-3AEA-1069-A2D8-08002B30309D}\\shell\\updatedb",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
	{
		GenRegKey.CloseKey();
		if ((m_bAdvancedAndContextMenuFlag&cmUpdateOnMyComputer)==0)
			CRegKey::DeleteKey(HKCR,"CLSID\\{20D04FE0-3AEA-1069-A2D8-08002B30309D}\\shell\\updatedb");
	}
	else if (m_bAdvancedAndContextMenuFlag&cmUpdateOnMyComputer)
	{
		if (GenRegKey.OpenKey(HKCR,"CLSID\\{20D04FE0-3AEA-1069-A2D8-08002B30309D}\\shell\\updatedb",CRegKey::createNew|CRegKey::samAll)==ERROR_SUCCESS)
		{
			GenRegKey.SetValue(L"",ID2W(IDS_EXPLORERUPDATE));
			CRegKey CommandKey;
			if (CommandKey.OpenKey(GenRegKey,"command",CRegKey::createNew|CRegKey::samAll)==ERROR_SUCCESS)
			{
				CStringW sExeName(GetApp()->GetExeNameW());
				CStringW command(sExeName,sExeName.FindLast(L'\\')+1);
				command << L"updtdb32.exe\"";
				command.Insert(0,L'\"');
				CommandKey.SetValue(L"",command);
			}
		}
	}

	// Language
	if (LocRegKey.OpenKey(HKCU,"",
		CRegKey::createNew|CRegKey::samAll)==ERROR_SUCCESS)
	{
		LocRegKey.SetValue(L"Language",m_strLangFile);
		LocRegKey.CloseKey();
		
		if (GenRegKey.OpenKey(HKCU,CLocateApp::GetRegKey(""),
				CRegKey::createNew|CRegKey::samAll)==ERROR_SUCCESS)
		{
			if (IsSettingsFlagSet(settingsUseLanguageWithConsoleApps))
				GenRegKey.SetValue(L"Language",m_strLangFile);
			else
				GenRegKey.DeleteValue("Language");
		}

	}	

	// Creating or deleting shortcut to Startup mene if necessary
	{
		WCHAR szPath[MAX_PATH];
		if (GetStartupPath(szPath))
		{
			int nLen=istrlen(szPath);
			if (szPath[nLen-1]!='\\')
				szPath[nLen++]='\\';
			wcscpy_s(szPath+nLen,MAX_PATH-nLen,L"Locate32 Autorun.lnk");
			
			if (IsSettingsFlagSet(settingsStartLocateAtStartup))
			{
				if (!FileSystem::IsFile(szPath))
					ShellFunctions::CreateShortcut(szPath,GetApp()->GetExeNameW(),L"",L" /S");
			}
			else 
			{
				if (FileSystem::IsFile(szPath))
					FileSystem::Remove(szPath);
			}

		}	
	}

	// Update status tooltip
	if (LocRegKey.OpenKey(HKCU,"\\Dialogs\\Updatestatus",CRegKey::createNew|CRegKey::samAll)==ERROR_SUCCESS)
	{
		
		LocRegKey.SetValue("Transparency100",m_nToolTipTransparency);
		LocRegKey.DeleteValue("Transparency");
		LocRegKey.SetValue("TextFont",(LPSTR)&m_lToolTipTextFont,sizeof(LOGFONT));
		LocRegKey.SetValue("TitleFont",(LPSTR)&m_lToolTipTitleFont,sizeof(LOGFONT));
		LocRegKey.SetValue("TextColor",(DWORD)m_cToolTipTextColor);
		LocRegKey.SetValue("TitleColor",(DWORD)m_cToolTipTitleColor);
		LocRegKey.SetValue("ErrorColor",(DWORD)m_cToolTipErrorColor);
		LocRegKey.SetValue("BackColor",(DWORD)m_cToolTipBackColor);
	}

	// Misc settings
	if (LocRegKey.OpenKey(HKCU,"\\Misc",CRegKey::createNew|CRegKey::samAll)==ERROR_SUCCESS)
	{
		LocRegKey.SetValue("NoExtensionInRename",m_dwSettingsFlags&settingsDontShowExtensionInRenameDialog?1:0);
	}
		
	
	return TRUE;
}




////////////////////////////////////////
// CGeneralSettingsPage
////////////////////////////////////////


BOOL CSettingsProperties::CGeneralSettingsPage::OnInitDialog(HWND hwndFocus)
{
	CPropertyPage::OnInitDialog(hwndFocus);
	
	// Setting mouse behaviour options
	if (m_pSettings->m_dwLocateDialogFlags&CLocateDlg::fgLVStylePointToSelect)
	{
		CheckDlgButton(IDC_POINTTOSELECT,1);
		OnPointToSelect();
		if ((m_pSettings->m_dwLocateDialogFlags&CLocateDlg::fgLVStyleAlwaysUnderline)==CLocateDlg::fgLVStyleAlwaysUnderline)
			CheckDlgButton(IDC_ALWAYSUNDERLINE,1);
		else if ((m_pSettings->m_dwLocateDialogFlags&CLocateDlg::fgLVStyleAlwaysUnderline)==CLocateDlg::fgLVStyleUnderLine)
			CheckDlgButton(IDC_POINTUNDERLINE,1);
		else
			CheckDlgButton(IDC_NEVERUNDERLINE,1);
	}
	else
	{
		CheckDlgButton(IDC_CLICKTOSELECT,1);
		CheckDlgButton(IDC_NEVERUNDERLINE,1);
		OnClickToSelect();
	}
		
	
	if (m_pSettings->m_dwLocateDialogFlags&CLocateDlg::fgLVStyleSystemDefine)
	{
		CheckDlgButton(IDC_SYSTEMSETTINGS,1);
		OnSystemSettings();
	}


	// Remember states
	if (m_pSettings->m_dwLocateDialogFlags&CLocateDlg::fgDialogRememberFields)
		CheckDlgButton(IDC_REMEMBERSTATES,1);
	// Minimize to ST
	if (m_pSettings->m_dwLocateDialogFlags&CLocateDlg::fgDialogMinimizeToST)
		CheckDlgButton(IDC_MINIMIZETOSYSTEMTRAY,1);
	if (m_pSettings->m_dwLocateDialogFlags&CLocateDlg::fgDialogCloseMinimizesDialog)
		CheckDlgButton(IDC_CLOSEBUTTONMINIMIZESWINDOW,1);

	

	// Close to ST
	if (GetLocateApp()->IsStartupFlagSet(CLocateApp::CStartData::startupLeaveBackground))
	{
		EnableDlgItem(IDC_CLOSETOSYSTEMTRAY,FALSE);
		CheckDlgButton(IDC_CLOSETOSYSTEMTRAY,1);
	}
	else if (m_pSettings->m_dwLocateDialogFlags&CLocateDlg::fgDialogLeaveLocateBackground)
		CheckDlgButton(IDC_CLOSETOSYSTEMTRAY,1);
	
	CLocateDlg::ViewDetails* pViewDetails=CLocateDlg::GetDefaultDetails();
	
	// Adding details to sorting box
	if (IsUnicodeSystem())
	{
		SendDlgItemMessageW(IDC_SORTING,CB_ADDSTRING,0,(LPARAM)(LPCWSTR)ID2W(IDS_NOSORTNG));
		for (int iDetail=0;iDetail<TypeCount;iDetail++)
		{
			SendDlgItemMessageW(IDC_SORTING,CB_ADDSTRING,0,
				(LPARAM)(LPCWSTR)ID2W(pViewDetails[iDetail].nString));
		}
	}
	else
	{
		SendDlgItemMessage(IDC_SORTING,CB_ADDSTRING,0,(LPARAM)(LPCSTR)ID2A(IDS_NOSORTNG));
		for (int iDetail=0;iDetail<TypeCount;iDetail++)
		{
			SendDlgItemMessage(IDC_SORTING,CB_ADDSTRING,0,
				(LPARAM)(LPCSTR)ID2A(pViewDetails[iDetail].nString));
		}
	}	
	delete[] pViewDetails;
	
	// Defaults
	if (m_pSettings->m_bSorting!=BYTE(-1))
	{
		SendDlgItemMessage(IDC_SORTING,CB_SETCURSEL,(m_pSettings->m_bSorting&127)+1);
		CheckDlgButton((m_pSettings->m_bSorting&128)?IDC_DESCENDINGORDER:IDC_ASCENDINGORDER,TRUE);
	}
	else
	{
		SendDlgItemMessage(IDC_SORTING,CB_SETCURSEL,0);
		CheckDlgButton(IDC_ASCENDINGORDER,TRUE);
	}

	CComboBox Checkin(GetDlgItem(IDC_CHECKIN));
	Checkin.AddString(ID2W(IDS_FILENAMESONLY));
	Checkin.AddString(ID2W(IDS_FILEANDFOLDERNAMES));
	Checkin.AddString(ID2W(IDS_FOLDERNAMESONLY));
	Checkin.SetCurSel(m_pSettings->m_bDefaultFlag&CSettingsProperties::defaultCheckInFlag);
	
	CheckDlgButton(IDC_MATCHWHOLEFILENAMEONLY,m_pSettings->m_bDefaultFlag&CSettingsProperties::defaultWholeName);
	CheckDlgButton(IDC_MATCHCASE,m_pSettings->m_bDefaultFlag&CSettingsProperties::defaultMatchCase);
	CheckDlgButton(IDC_REPLACESPACES,m_pSettings->m_bDefaultFlag&CSettingsProperties::defaultReplaceSpaces);
	CheckDlgButton(IDC_USEWHOLEPATH,m_pSettings->m_bDefaultFlag&CSettingsProperties::defaultUseWholePath);
	CheckDlgButton(IDC_DATAMATCHCASE,m_pSettings->m_bDefaultFlag&CSettingsProperties::defaultDataMatchCase);
	
	return FALSE;
}

BOOL CSettingsProperties::CGeneralSettingsPage::OnCommand(WORD wID,WORD wNotifyCode,HWND hControl)
{
	CPropertyPage::OnCommand(wID,wNotifyCode,hControl);
	switch (wID)
	{
	case IDC_SYSTEMSETTINGS:
		OnSystemSettings();
		break;
	case IDC_POINTTOSELECT:
		OnPointToSelect();
		break;
	case IDC_CLICKTOSELECT:
		OnClickToSelect();
		break;
	case IDC_NEVERUNDERLINE:
		OnNeverUnderline();
		break;
	case IDC_POINTUNDERLINE:
		OnPointUnderline();
		break;
	case IDC_ALWAYSUNDERLINE:
		OnAlwaysUnderline();
		break;
	}
	return FALSE;
}

BOOL CSettingsProperties::CGeneralSettingsPage::OnApply()
{
	m_pSettings->m_dwLocateDialogFlags&=~(CLocateDlg::fgLVStyleFlag|CLocateDlg::fgDialogLeaveLocateBackground|
		CLocateDlg::fgDialogRememberFields|CLocateDlg::fgDialogMinimizeToST|CLocateDlg::fgDialogCloseMinimizesDialog);

	// Setting tree view mouse behaviour
	if (IsDlgButtonChecked(IDC_SYSTEMSETTINGS))
		m_pSettings->m_dwLocateDialogFlags|=CLocateDlg::fgLVStyleSystemDefine;
	else
	{
		if (IsDlgButtonChecked(IDC_POINTTOSELECT))
		{
			m_pSettings->m_dwLocateDialogFlags|=CLocateDlg::fgLVStylePointToSelect;
			if (IsDlgButtonChecked(IDC_ALWAYSUNDERLINE))
				m_pSettings->m_dwLocateDialogFlags|=CLocateDlg::fgLVStyleAlwaysUnderline;
			else if (IsDlgButtonChecked(IDC_POINTUNDERLINE))
				m_pSettings->m_dwLocateDialogFlags|=CLocateDlg::fgLVStyleUnderLine;
		}
	}
	
	// Remember dialog states
	if (IsDlgButtonChecked(IDC_REMEMBERSTATES))
		m_pSettings->m_dwLocateDialogFlags|=CLocateDlg::fgDialogRememberFields;
	// Minimize to system tray
	if (IsDlgButtonChecked(IDC_MINIMIZETOSYSTEMTRAY))
		m_pSettings->m_dwLocateDialogFlags|=CLocateDlg::fgDialogMinimizeToST;
	// Close minimizes window
	if (IsDlgButtonChecked(IDC_CLOSEBUTTONMINIMIZESWINDOW))
		m_pSettings->m_dwLocateDialogFlags|=CLocateDlg::fgDialogCloseMinimizesDialog;
	// Load file types from registry
	if (!GetLocateApp()->IsStartupFlagSet(CLocateApp::CStartData::startupLeaveBackground))
	{
		if (IsDlgButtonChecked(IDC_CLOSETOSYSTEMTRAY))
			m_pSettings->m_dwLocateDialogFlags|=CLocateDlg::fgDialogLeaveLocateBackground;
	}
		
	// Defaults
	int nSel=(int)SendDlgItemMessage(IDC_SORTING,CB_GETCURSEL);
	if (nSel<=0 || nSel>=TypeCount+1)
		m_pSettings->m_bSorting=BYTE(-1);
	else
	{
		m_pSettings->m_bSorting=nSel-1;
		if (IsDlgButtonChecked(IDC_DESCENDINGORDER))
			m_pSettings->m_bSorting|=128;
	}

	m_pSettings->m_bDefaultFlag=(BYTE)SendDlgItemMessage(IDC_CHECKIN,CB_GETCURSEL);
	if (IsDlgButtonChecked(IDC_MATCHWHOLEFILENAMEONLY))
		m_pSettings->m_bDefaultFlag|=CSettingsProperties::defaultWholeName;
	if (IsDlgButtonChecked(IDC_MATCHCASE))
		m_pSettings->m_bDefaultFlag|=CSettingsProperties::defaultMatchCase;
	if (IsDlgButtonChecked(IDC_REPLACESPACES))
		m_pSettings->m_bDefaultFlag|=CSettingsProperties::defaultReplaceSpaces;
	if (IsDlgButtonChecked(IDC_USEWHOLEPATH))
		m_pSettings->m_bDefaultFlag|=CSettingsProperties::defaultUseWholePath;
	if (IsDlgButtonChecked(IDC_DATAMATCHCASE))
		m_pSettings->m_bDefaultFlag|=CSettingsProperties::defaultDataMatchCase;
	
	CPropertyPage::OnApply();
	return TRUE;
}

void CSettingsProperties::CGeneralSettingsPage::OnCancel()
{
	m_pSettings->SetSettingsFlags(CSettingsProperties::settingsCancelled);
	
	CPropertyPage::OnCancel();
}


BYTE CSettingsProperties::CGeneralSettingsPage::OnSystemSettings()
{
	if (IsDlgButtonChecked(IDC_SYSTEMSETTINGS))
	{
		EnableDlgItem(IDC_POINTTOSELECT,FALSE);
		EnableDlgItem(IDC_CLICKTOSELECT,FALSE);
		EnableDlgItem(IDC_NEVERUNDERLINE,FALSE);
		EnableDlgItem(IDC_POINTUNDERLINE,FALSE);
		EnableDlgItem(IDC_ALWAYSUNDERLINE,FALSE);
	}
	else
	{
		EnableDlgItem(IDC_POINTTOSELECT,TRUE);
		EnableDlgItem(IDC_CLICKTOSELECT,TRUE);
		if (IsDlgButtonChecked(IDC_POINTTOSELECT))
		{
			EnableDlgItem(IDC_NEVERUNDERLINE,TRUE);
			EnableDlgItem(IDC_POINTUNDERLINE,TRUE);
			EnableDlgItem(IDC_ALWAYSUNDERLINE,TRUE);
		}
	}
	return TRUE;
}

BYTE CSettingsProperties::CGeneralSettingsPage::OnPointToSelect()
{
	CheckDlgButton(IDC_POINTTOSELECT,1);
	CheckDlgButton(IDC_CLICKTOSELECT,0);
	EnableDlgItem(IDC_NEVERUNDERLINE,TRUE);
	EnableDlgItem(IDC_POINTUNDERLINE,TRUE);
	EnableDlgItem(IDC_ALWAYSUNDERLINE,TRUE);
	return TRUE;
}

BYTE CSettingsProperties::CGeneralSettingsPage::OnClickToSelect()
{
	CheckDlgButton(IDC_POINTTOSELECT,0);
	CheckDlgButton(IDC_CLICKTOSELECT,1);
	EnableDlgItem(IDC_NEVERUNDERLINE,FALSE);
	EnableDlgItem(IDC_POINTUNDERLINE,FALSE);
	EnableDlgItem(IDC_ALWAYSUNDERLINE,FALSE);
	return TRUE;
}
	
BYTE CSettingsProperties::CGeneralSettingsPage::OnNeverUnderline()
{
	CheckDlgButton(IDC_NEVERUNDERLINE,1);
	CheckDlgButton(IDC_POINTUNDERLINE,0);
	CheckDlgButton(IDC_ALWAYSUNDERLINE,0);
	return TRUE;
}

BYTE CSettingsProperties::CGeneralSettingsPage::OnPointUnderline()
{
	CheckDlgButton(IDC_NEVERUNDERLINE,0);
	CheckDlgButton(IDC_POINTUNDERLINE,1);
	CheckDlgButton(IDC_ALWAYSUNDERLINE,0);
	return TRUE;
}

BYTE CSettingsProperties::CGeneralSettingsPage::OnAlwaysUnderline()
{
	CheckDlgButton(IDC_NEVERUNDERLINE,0);
	CheckDlgButton(IDC_POINTUNDERLINE,0);
	CheckDlgButton(IDC_ALWAYSUNDERLINE,1);
	return TRUE;
}

void CSettingsProperties::CGeneralSettingsPage::OnHelp(LPHELPINFO lphi)
{
	CLocateApp::HelpID id[]= {
		{ IDC_SYSTEMSETTINGS,"sg_sysset" },
		{ IDC_POINTTOSELECT,"sg_point" },
		{ IDC_NEVERUNDERLINE,"sg_neverunderline" },
		{ IDC_POINTUNDERLINE,"sg_hover" },
		{ IDC_ALWAYSUNDERLINE,"sg_alwaysunderline" },
		{ IDC_CLICKTOSELECT,"sg_click" },
		{ IDC_MINIMIZETOSYSTEMTRAY,"sg_min2st" },
		{ IDC_CLOSEBUTTONMINIMIZESWINDOW,"sg_close2min" },
		{ IDC_REMEMBERSTATES,"sg_savestates" },
		{ IDC_CLOSETOSYSTEMTRAY,"sg_close2st" },
		{ IDC_SORTINGLABEL,"sg_sorting" },
		{ IDC_SORTING,"sg_sorting" },
		{ IDC_ASCENDINGORDER,"sg_sorting" },
		{ IDC_DESCENDINGORDER,"sg_sorting" },
		{ IDC_MATCHWHOLEFILENAMEONLY,"sg_matchwn" },
		{ IDC_CHECKIN,"sg_check" },
		{ IDC_CHECKINLABEL,"sg_check" },
		{ IDC_REPLACESPACES,"sg_repspaces" },
		{ IDC_MATCHCASE,"sg_matchcase" },
		{ IDC_USEWHOLEPATH,"sg_wholepath" },
		{ IDC_DATAMATCHCASE,"sg_datamatchcase" }
	};
	
	if (CLocateApp::OpenHelp(*this,"settings_general.htm",lphi,id,sizeof(id)/sizeof(CLocateApp::HelpID)))
		return;

	if (HtmlHelp(HH_HELP_CONTEXT,HELP_SETTINGS_GENERAL)==NULL)
		HtmlHelp(HH_DISPLAY_TOPIC,0);
}


////////////////////////////////////////
// CAdvancedSettingsPage
////////////////////////////////////////


BOOL CSettingsProperties::CAdvancedSettingsPage::OnInitDialog(HWND hwndFocus)
{
	COptionsPropertyPage::OnInitDialog(hwndFocus);


	/////////////////////////////////////////////////////////////////////////
	// Items under LOCATING FILES AND RESULTS LIST category

	
	
	Item* LimitMaximumResults[]={
		CreateNumeric(IDS_ADVSETMAXNUMBEROFRESULTS,DefaultNumericProc,
			DWORD(-1),&m_pSettings->m_nMaximumFoundFiles,"sa_limit"),
		NULL
	};
	
	Item* InstantSearching[]={
		CreateNumeric(IDS_ADVSETISLIMIT,DefaultNumericProc,
			DWORD(-1),&m_pSettings->m_nInstantSearchingLimit,"sa_islimit"),
		CreateNumeric(IDS_ADVSETISDELAY,DefaultNumericProc,
			DWORD(-1),&m_pSettings->m_nInstantSearchingDelay,"sa_isdelay"),
		CreateNumeric(IDS_ADVSETISCHARS,DefaultNumericProc,
			MAKELONG(1,100),&m_pSettings->m_nInstantSearchingChars,"sa_ischars"),
		CreateCheckBox(IDS_ADVSETISUPDOWNGORESULTS,NULL,DefaultCheckBoxProc,
			CLocateDlg::isUpDownGoesToResults,
			&m_pSettings->m_dwInstantSearchingFlags,"sa_isupdown"),
		CreateCheckBox(IDS_ADVSETISUPDATETITLE,NULL,DefaultCheckBoxProc,
			CLocateDlg::isUpdateTitle,
			&m_pSettings->m_dwInstantSearchingFlags,"sa_isupdatetitle"),
		CreateCheckBox(IDS_ADVSETISDISABLEIFDATASEARCH,NULL,DefaultCheckBoxProc,
			CLocateDlg::isDisableIfDataSearch,
			&m_pSettings->m_dwInstantSearchingFlags,"sa_isdisifdata"),
		CreateCheckBox(IDS_ADVSETISNAMEDCHANGED,NULL,DefaultCheckBoxProc,
			CLocateDlg::isNameChanged,&m_pSettings->m_dwInstantSearchingFlags,"sa_isname"),
		CreateCheckBox(IDS_ADVSETISTYPECHANGED,NULL,DefaultCheckBoxProc,
			CLocateDlg::isTypeChanged,&m_pSettings->m_dwInstantSearchingFlags,"sa_istype"),
		CreateCheckBox(IDS_ADVSETISLOOKINCHANGED,NULL,DefaultCheckBoxProc,
			CLocateDlg::isLookInChanged,&m_pSettings->m_dwInstantSearchingFlags,"sa_islookin"),
		CreateCheckBox(IDS_ADVSETISSIZESCHANGED,NULL,DefaultCheckBoxProc,
			CLocateDlg::isSizesChanged,&m_pSettings->m_dwInstantSearchingFlags,"sa_issizes"),
		CreateCheckBox(IDS_ADVSETISDATESCHANGED,NULL,DefaultCheckBoxProc,
			CLocateDlg::isDatesChanged,&m_pSettings->m_dwInstantSearchingFlags,"sa_isdates"),
		CreateCheckBox(IDS_ADVSETISDATACHANGED,NULL,DefaultCheckBoxProc,
			CLocateDlg::isDataChanged,&m_pSettings->m_dwInstantSearchingFlags,"sa_isdata"),
		CreateCheckBox(IDS_ADVSETISOTHERCHANGED,NULL,DefaultCheckBoxProc,
			CLocateDlg::isOtherChanged,&m_pSettings->m_dwInstantSearchingFlags,"sa_isother"),
		CreateCheckBox(IDS_ADVSETISBYCOMMANDLINE,NULL,DefaultCheckBoxProc,
			CLocateDlg::isByCommandLine,&m_pSettings->m_dwInstantSearchingFlags,"sa_isbycommandline"),
		NULL
	};	

	Item* ResultListFontItems[]={
		CreateFont(IDS_ADVSETRESULTLISTFONT,DefaultFontProc,NULL,&m_pSettings->m_lResultListFont),
		NULL
	};
	
	Item* TitleMethodItems[]={
		CreateCheckBox(IDS_ADVSETFIRSTCHARUPPER,NULL,DefaultCheckBoxProc,
			CLocateDlg::fgLV1stCharUpper,&m_pSettings->m_dwLocateDialogFlags,"sa_1stcharup"),
		CreateRadioBox(IDS_ADVSETALWAYSSHOWEXT,NULL,DefaultRadioBoxProc,
			MAKELONG(CLocateDlg::fgLVAlwaysShowExtensions,CLocateDlg::fgLVExtensionFlag),
			&m_pSettings->m_dwLocateDialogFlags,"sa_extalways"),
		CreateRadioBox(IDS_ADVSETHIDEEXTFORKNOWN,NULL,DefaultRadioBoxProc,
			MAKELONG(CLocateDlg::fgLVHideKnownExtensions,CLocateDlg::fgLVExtensionFlag),
			&m_pSettings->m_dwLocateDialogFlags,"sa_extforknown"),
		CreateRadioBox(IDS_ADVSETNEVERSHOWEXT,NULL,DefaultRadioBoxProc,
			MAKELONG(CLocateDlg::fgLVNeverShowExtensions,CLocateDlg::fgLVExtensionFlag),
			&m_pSettings->m_dwLocateDialogFlags,"sa_extnever"),
		NULL
	};
	Item* TypeMethodItems[]={
		CreateRadioBox(IDS_ADVSETUSESHELLFORTYPE,NULL,DefaultRadioBoxProc,
			MAKELONG(CLocateDlg::fgLVShowShellType,CLocateDlg::fgLVShowShellType),
			&m_pSettings->m_dwLocateDialogFlags,"is_shellfortype"),
		CreateRadioBox(IDS_ADVSETUSEOWNMETHODFORTYPE,NULL,DefaultRadioBoxProc,
			MAKELONG(0,CLocateDlg::fgLVShowShellType),
			&m_pSettings->m_dwLocateDialogFlags,"is_ownfortype"),
		NULL
	};
	
	Item* TooltipDelayItems[]={
		CreateNumeric(IDS_ADVSETSHOWTOOLTIPDELAY,DefaultNumericProc,
			MAKELONG(0,1000000),&m_pSettings->m_dwTooltipDelayInitial,"sa_tooltipdelays"),
		CreateNumeric(IDS_ADVSETSHOWTOOLTIPDURATION,DefaultNumericProc,
			MAKELONG(0,1000000),&m_pSettings->m_dwTooltipDelayAutopop,"sa_tooltipdelays"),
		NULL
	};
	Item* OtherExplorerProgram[]={
		CreateFile(IDS_ADVSETOPENFOLDERWITH,ExternalCommandProc,0,
			&m_pSettings->m_OpenFoldersWith,"sa_ownprogforfolders"),
		NULL
		};
	Item* ResultsListItems[]={
		CreateCheckBox(IDS_ADVSETUSECUSTOMFONTINRESULTLIST,ResultListFontItems,DefaultCheckBoxProc,
			CSettingsProperties::settingsUseCustomResultListFont,
			&m_pSettings->m_dwSettingsFlags,"sa_customfont"),
		CreateCheckBox(IDS_ADVSETNODELAYEDUPDATING,NULL,DefaultCheckBoxProc,
			CLocateDlg::fgLVNoDelayedUpdate,
			&m_pSettings->m_dwLocateDialogFlags,"sa_immediateupdate"),
		CreateRadioBox(IDS_ADVSETUSEGETTITLE,NULL,DefaultRadioBoxProc,
			MAKELONG(CLocateDlg::fgLVUseGetFileTitle,
			CLocateDlg::fgLVMethodFlag),&m_pSettings->m_dwLocateDialogFlags,"sa_usegetfiletitle"),
		CreateRadioBox(IDS_ADVSETUSEOWNMETHODFORTITLE,TitleMethodItems,DefaultRadioBoxProc,
			MAKELONG(CLocateDlg::fgLVUseOwnMethod,CLocateDlg::fgLVMethodFlag),
			&m_pSettings->m_dwLocateDialogFlags,"sa_ownmethodfortitle"),
		CreateCheckBox(IDS_ADVSETUSESIMPLEICONS,NULL,DefaultInverseCheckBoxProc,
			CLocateDlg::fgLVShowIcons,
			&m_pSettings->m_dwLocateDialogFlags,"sa_simpleicons"),
		CreateCheckBox(IDS_ADVSETSHOWFILETYPES,TypeMethodItems,DefaultCheckBoxProc,
			CLocateDlg::fgLVShowFileTypes,&m_pSettings->m_dwLocateDialogFlags,"sa_filetypes"),
		CreateCheckBox(IDS_ADVSETSHOWTOOLTIPS,NULL,DefaultInverseCheckBoxProc,
			CLocateDlg::fgLVDontShowTooltips,
			&m_pSettings->m_dwLocateDialogFlags,"sa_showtooltips"),
		CreateCheckBox(IDS_ADVSETSETTOOLTIPDELAYS,TooltipDelayItems,DefaultCheckBoxProc,
			CSettingsProperties::settingsSetTooltipDelays,
			&m_pSettings->m_dwSettingsFlags,"sa_tooltipdelays"),
		CreateCheckBox(IDS_ADVSETDONTSHOWHIDDENFILES,NULL,DefaultCheckBoxProc,
			CLocateDlg::fgLVDontShowHiddenFiles,&m_pSettings->m_dwLocateDialogFlags,"sa_nosysorhid"),
		CreateCheckBox(IDS_ADVSETDONTSHOWDELETEDFILES,NULL,DefaultCheckBoxProc,
			CLocateDlg::efLVDontShowDeletedFiles,&m_pSettings->m_dwLocateDialogExtraFlags,"sa_nodeleted"),
		CreateCheckBox(IDS_ADVSETNODOUBLERESULTS,NULL,DefaultCheckBoxProc,
			CLocateDlg::fgLVNoDoubleItems,&m_pSettings->m_dwLocateDialogFlags,"sa_nodoubles"),
		CreateCheckBox(IDS_ADVSETFOLDERSFIRST,NULL,DefaultCheckBoxProc,
			CLocateDlg::fgLVFoldersFirst,&m_pSettings->m_dwLocateDialogFlags,"sa_foldersfirst"),
		CreateCheckBox(IDS_ADVSETFULLROWSELECT,NULL,DefaultCheckBoxProc,
			CLocateDlg::fgLVSelectFullRow,&m_pSettings->m_dwLocateDialogFlags,"sa_fullrowselect"),
		CreateCheckBox(IDS_ADVSETACTIVATEFIRSTITEM,NULL,DefaultCheckBoxProc,
			CLocateDlg::fgLVActivateFirstResult,&m_pSettings->m_dwLocateDialogFlags,"sa_act1st"),
		CreateComboBox(IDS_ADVSETSHOWDATESINFORMAT,DateFormatComboProc,0,0,"sa_dateformat"),
		CreateComboBox(IDS_ADVSETSHOWTIMESINFORMAT,TimeFormatComboProc,0,0,"sa_timeformat"),
		CreateListBox(IDS_ADVSETSHOWFILESIZESINFORMAT,FileSizeListProc,0,
			&m_pSettings->m_nFileSizeFormat,"sa_sizeformat"),
		CreateCheckBox(IDS_ADVSETFORMATWITHUSERLOCALE,NULL,DefaultCheckBoxProc,
			CLocateApp::pfFormatUseLocaleFormat,&m_pSettings->m_dwProgramFlags,"sa_localesize"),
		CreateListBox(IDS_ADVSETSORTINGMETHOD,SortingMethodProc,0,
			&m_pSettings->m_dwSortingMethod,"sa_sortingmethod"),
		CreateCheckBox(IDS_ADVSETUSEPROGRAMFORFOLDERS,OtherExplorerProgram,DefaultCheckBoxProc,
			CSettingsProperties::settingsUseOtherProgramsToOpenFolders,&m_pSettings->m_dwSettingsFlags,"sa_ownprogforfolders"),
		CreateCheckBox(IDS_ADVSETCOMPUTEMD5SUMS,NULL,DefaultCheckBoxProc,
			CLocateDlg::fgLVComputeMD5Sums,&m_pSettings->m_dwLocateDialogFlags,"sa_md5sums"),
		CreateCheckBox(IDS_ADVSETALLOWINPLACERENAMING,NULL,DefaultCheckBoxProc,
			CLocateDlg::fgLVAllowInPlaceRenaming,
			&m_pSettings->m_dwLocateDialogFlags,"sa_exprenaming"),
		NULL,
		NULL
	};

	if (GetSystemFeaturesFlag()&efWinVista)
	{
		ResultsListItems[sizeof(ResultsListItems)/sizeof(ResultsListItems[0])-2]=CreateCheckBox(IDS_ADVSETHEADERINALLVIEWMODES,NULL,DefaultCheckBoxProc,
			CLocateDlg::efLVHeaderInAllViews,&m_pSettings->m_dwLocateDialogExtraFlags,"sa_headerinallmodes");
	}
			
	Item* ViewModeItems[]={
		CreateCheckBox(IDS_ADVSETTHUMBSWITHMEDIUMICONS,NULL,DefaultCheckBoxProc,
			CLocateDlg::tfThumbnailsInMediumIcons,
			&m_pSettings->m_dwThumbnailFlags,"sa_thumbsinmedium"),
		CreateNumeric(IDS_ADVSETLARGEICONSIZE,DefaultNumericProc,
			MAKELONG(16,1024),&m_pSettings->m_dwThumbnailLargeIconSize,"sa_largeiconsize"),
		CreateNumeric(IDS_ADVSETEXTRALARGEICONSIZE,DefaultNumericProc,
			MAKELONG(16,1024),&m_pSettings->m_dwThumbnailExtraLargeIconSize,"sa_extlargeiconsize"),
		NULL
	};
	
	Item* FileBackgroundOperations[]={
		CreateRadioBox(IDS_ADVSETDISABLEFSCHANGETRACKING,NULL,DefaultRadioBoxProc,
			MAKELONG(CLocateDlg::efDisableFSTracking,CLocateDlg::efTrackingMask),
			&m_pSettings->m_dwLocateDialogExtraFlags,"sa_nofstracking"),
		CreateRadioBox(IDS_ADVSETENABLEFSCHANGETRACKING,NULL,DefaultRadioBoxProc,
			MAKELONG(CLocateDlg::efEnableFSTracking,CLocateDlg::efTrackingMask),
			&m_pSettings->m_dwLocateDialogExtraFlags,"sa_fstrackingnew"),
		CreateCheckBox(IDS_ADVSETWHILESORTINGDONOTREADINFO,NULL,DefaultCheckBoxProc,
			CLocateDlg::efLVNoUpdateWhileSorting,
			&m_pSettings->m_dwLocateDialogExtraFlags,"sa_noupdwhilesorting"),
		NULL,
		NULL
	};
	if (GetProcAddress(GetModuleHandle("kernel32.dll"),"ReadDirectoryChangesW")!=NULL)
	{
		FileBackgroundOperations[3]=FileBackgroundOperations[2];
		FileBackgroundOperations[2]=CreateRadioBox(IDS_ADVSETENABLEFSCHANGETRACKINGOLD,NULL,DefaultRadioBoxProc,
			MAKELONG(CLocateDlg::efEnableFSTrackingOld,CLocateDlg::efTrackingMask),
			&m_pSettings->m_dwLocateDialogExtraFlags,"sa_fstrackingold");
	}

	Item* UpdateResults[]={
		CreateRadioBox(IDS_ADVSETDISABLEUPDATING,NULL,DefaultRadioBoxProc,
			MAKELONG(CLocateDlg::efDisableItemUpdating,CLocateDlg::efItemUpdatingMask),
			&m_pSettings->m_dwLocateDialogExtraFlags,"sa_disableupdating"),
		CreateRadioBox(IDS_ADVSETENABLEUPDATING,FileBackgroundOperations,DefaultRadioBoxProc,
			MAKELONG(CLocateDlg::efEnableItemUpdating,CLocateDlg::efItemUpdatingMask),
			&m_pSettings->m_dwLocateDialogExtraFlags,"sa_enableupdating"),
		NULL
	};
		
	



	Item* LocateProcessAndResultsItems[]={
		CreateCheckBox(IDS_ADVSETLIMITRESULTS,LimitMaximumResults,LimitResultsCheckBoxProc,
			0,&m_pSettings->m_nMaximumFoundFiles,"sa_limit"),
		CreateCheckBox(IDS_ADVSETSPACEISSEPARATOR,NULL,DefaultCheckBoxProc,
			CLocateDlg::efAllowSpacesAsSeparators,
			&m_pSettings->m_dwLocateDialogExtraFlags,"sa_spaceisseparator"),
		CreateCheckBox(IDS_ADVSETLOGICALOPERATIONS,NULL,DefaultCheckBoxProc,
			CLocateDlg::efEnableLogicalOperations,
			&m_pSettings->m_dwLocateDialogExtraFlags,"sa_logicalops"),
		CreateCheckBox(IDS_ADVSETANDMODEALWAYS,NULL,DefaultCheckBoxProc,
			CLocateDlg::efAndModeAlways,
			&m_pSettings->m_dwLocateDialogExtraFlags,"sa_andmodealways"),
		CreateCheckBox(IDS_ADVSETMATCHWHOLENAMEIFASTERISKS,NULL,DefaultCheckBoxProc,
			CLocateDlg::efMatchWhileNameIfAsterisks,
			&m_pSettings->m_dwLocateDialogExtraFlags,"sa_matchwholenameifasterisks"),
		CreateCheckBox(IDS_ADVSETNOASTERISKATENDIFEXTENSIONGIVEN,NULL,DefaultInverseCheckBoxProc,
			CLocateDlg::efAsteriskAtEndEvenIfExtensionExists,
			&m_pSettings->m_dwLocateDialogExtraFlags,"sa_noasterisktoendifextension"),
		CreateCheckBox(IDS_ADVSETISENABLE,InstantSearching,DefaultCheckBoxProc,
			CLocateDlg::isEnable,&m_pSettings->m_dwInstantSearchingFlags,"sa_isenable"),
		CreateRoot(IDS_ADVSETRESULTSLIST,ResultsListItems,"sa_results"),
		CreateRoot(IDS_ADVSETVIEWMODES,ViewModeItems,"sa_viewmodes"),
		CreateRoot(IDS_ADVSETUPDATERESULTS,UpdateResults,"sa_updateresults"),
		NULL
	};
		

	
	
	/////////////////////////////////////////////////////////////////////////
	// Items under Locate dialog category

	Item* LookInItems[]={
		CreateNumeric(IDS_ADVSETNUMBEROFDIRECTORIES,DefaultNumericProc,
			MAKELONG(0,100),&m_pSettings->m_nNumberOfDirectories,"sa_nosavedfolders"),
		CreateCheckBox(IDS_ADVSETDONTSAVENETWORKDRIVES,NULL,DefaultCheckBoxProc,
			CLocateDlg::efNameDontSaveNetworkDrivesAndDirectories,
			&m_pSettings->m_dwLocateDialogExtraFlags,"sa_dontsaveUNC"),
		CreateCheckBox(IDS_ADVSETDONTRESOLVEDRIVEICONS,NULL,DefaultCheckBoxProc,
			CLocateDlg::efNameDontResolveIconAndDisplayNameForDrives,
			&m_pSettings->m_dwLocateDialogExtraFlags,"sa_nolocaldrivesicons"),
		CreateRadioBox(IDS_ADVSETADDSELECTEDROOTS,NULL,DefaultRadioBoxShiftProc,
			MAKELONG(CLocateDlg::fgNameAddEnabledRoots>>16,CLocateDlg::fgNameRootFlag>>16),
			&m_pSettings->m_dwLocateDialogFlags,"sa_drvsfromenablesdbs"),
		CreateRadioBox(IDS_ADVSETADDALLROOTS,NULL,DefaultRadioBoxShiftProc,
			MAKELONG(CLocateDlg::fgNameAddAllRoots>>16,CLocateDlg::fgNameRootFlag>>16),
			&m_pSettings->m_dwLocateDialogFlags,"sa_drvsfromalldbs"),
		CreateRadioBox(IDS_ADVSETDONTADDANYROOTS,NULL,DefaultRadioBoxShiftProc,
			MAKELONG(CLocateDlg::fgNameDontAddRoots>>16,CLocateDlg::fgNameRootFlag>>16),
			&m_pSettings->m_dwLocateDialogFlags,"sa_drvsfromnodb"),
		CreateCheckBox(IDS_ADVSETMULTIPLEDIRECTORIES,NULL,DefaultCheckBoxProc,
			CLocateDlg::fgNameMultibleDirectories,
			&m_pSettings->m_dwLocateDialogFlags,"sa_multidir"),
		NULL
	};

	Item* CustomDialogIconItems[]={
		CreateFile(IDS_ADVSETICONFILE,TrayIconProc,0,
			&m_pSettings->m_CustomDialogIcon,"sa_customdialogicon"),
		NULL
	};
	
	Item* LocateDialogItems[]={
		CreateCheckBox(IDS_ADVSETLARGEMODEONLY,NULL,DefaultCheckBoxProc,
			CLocateDlg::fgDialogLargeModeOnly,&m_pSettings->m_dwLocateDialogFlags,"sa_largemode"),
		CreateNumeric(IDS_ADVSETNUMBEROFNAMES,DefaultNumericProc,
			MAKELONG(0,256),&m_pSettings->m_nNumberOfNames,"sa_itemsinnamed"),
		CreateNumeric(IDS_ADVSETNUMBEROFTYPES,DefaultNumericProc,
			MAKELONG(0,256),&m_pSettings->m_nNumberOfTypes,"sa_itemsintype"),
		CreateRoot(IDS_ADVSETLOOKINCOMBO,LookInItems,"sa_lookin"),
		CreateCheckBox(IDS_ADVSETDONTSAVELISTITEMS,NULL,DefaultCheckBoxProc,
			CLocateDlg::efNameDontSaveNameTypeAndDirectories,
			&m_pSettings->m_dwLocateDialogExtraFlags,"sa_nosave"),
		CreateCheckBox(IDS_ADVSETLOADTYPES,NULL,DefaultCheckBoxProc,
			CLocateDlg::fgLoadRegistryTypes,
			&m_pSettings->m_dwLocateDialogFlags,"sa_regfiletypes"),
		CreateCheckBox(IDS_ADVSETTOPMOST,NULL,DefaultCheckBoxProc,
			CLocateDlg::fgDialogTopMost,
			&m_pSettings->m_dwLocateDialogFlags,"sa_alwaysontop"),
		CreateCheckBox(IDS_ADVSETCUSTOMDIALOGICON,CustomDialogIconItems,DefaultCheckBoxProc,
			CSettingsProperties::settingsUseCustomDialogIcon,
			&m_pSettings->m_dwSettingsFlags,"sa_customdialogicon"),
		NULL, // For transparency
		NULL
	};
	if (GetProcAddress(GetModuleHandle("user32.dll"),"SetLayeredWindowAttributes")!=NULL)
	{
		// Needs at least Win2k
		LocateDialogItems[sizeof(LocateDialogItems)/sizeof(LocateDialogItems[0])-2]=CreateNumeric(IDS_ADVSETTRANSPARENCY,DefaultNumericProc,
			MAKELONG(0,100),&m_pSettings->m_nTransparency,"sa_transparency");
	}



	/////////////////////////////////////////////////////////////////////////
	// Items under Update Process category

	
	Item* StatusTooltipItems[]={
		CreateColor(IDS_ADVSETTOOLTIPTEXTCOLOR,DefaultColorProc,NULL,
			&m_pSettings->m_cToolTipTextColor,"sa_tip_textcolor"),
		CreateColor(IDS_ADVSETTOOLTIPTITLECOLOR,DefaultColorProc,NULL,
			&m_pSettings->m_cToolTipTitleColor,"sa_tip_titlecolor"),
		CreateColor(IDS_ADVSETTOOLTIPERRORCOLOR,DefaultColorProc,NULL,
			&m_pSettings->m_cToolTipErrorColor,"sa_tip_errorcolor"),
		CreateColor(IDS_ADVSETTOOLTIPBACKCOLOR,DefaultColorProc,NULL,
			&m_pSettings->m_cToolTipBackColor,"sa_tip_bgcolor"),
		CreateFont(IDS_ADVSETTOOLTIPTEXTFONT,DefaultFontProc,NULL,
			&m_pSettings->m_lToolTipTextFont,"sa_tip_textfont"),
		CreateFont(IDS_ADVSETTOOLTIPTITLEFONT,DefaultFontProc,NULL,
			&m_pSettings->m_lToolTipTitleFont,"sa_tip_titlefont"),
		CreateListBox(IDS_ADVSETTOOLTIPWINDOWPOSITION,UpdateTooltipPositionProc,0,
			&m_pSettings->m_dwProgramFlags,"sa_tip_pos"),
		CreateListBox(IDS_ADVSETTOOLTIPWINDOWONTOP,UpdateTooltipTopmostProc,0,
			&m_pSettings->m_dwProgramFlags,"sa_tip_ontop"),
		NULL, // For transparency
		NULL
	};
	
	
	
	if (GetProcAddress(GetModuleHandle("user32.dll"),"SetLayeredWindowAttributes")!=NULL)
	{
		// Needs at least Win2k
		StatusTooltipItems[sizeof(StatusTooltipItems)/sizeof(StatusTooltipItems[0])-2]=CreateNumeric(IDS_ADVSETTOOLTIPTRANSPARENCY,DefaultNumericProc,
			MAKELONG(0,100),&m_pSettings->m_nToolTipTransparency,"sa_tip_trans");
	}
		

	Item* UpdateProcessItems[]={
		CreateCheckBox(IDS_ADVSETSHOWUPDATESTATUSTOOLTIP,StatusTooltipItems,
			DefaultCheckBoxProc,CLocateApp::pfEnableUpdateTooltip,
			&m_pSettings->m_dwProgramFlags,"sa_enableupdatetooltip"),
		CreateListBox(IDS_ADVSETUPDATETHREADPRIORITY,UpdateThreadPriorityProc,0,
			&m_pSettings->m_nUpdateThreadPriority,"sa_threadpriority"),
		NULL
	};
	
	

	/////////////////////////////////////////////////////////////////////////
	// Items under System Tray Icon category

	
	Item* SystemTrayIconItems[]={
		CreateFile(IDS_ADVSETICONFILE,TrayIconProc,0,
			&m_pSettings->m_CustomTrayIcon,"sa_customSTicon"),
		NULL
	};

	Item* SystemTrayIcon[]={
		CreateCheckBox(IDS_ADVSETDONTSHOWSTICON,NULL,DefaultCheckBoxProc,
			CLocateApp::pfDontShowSystemTrayIcon,
			&m_pSettings->m_dwProgramFlags,"sa_noSTicon"),
		CreateCheckBox(IDS_ADVSETCLICKACTIVATETRAYICON,NULL,
			DefaultCheckBoxProc,CLocateApp::pfTrayIconClickActivate,
			&m_pSettings->m_dwProgramFlags,"sa_singleclkopens"),
		CreateCheckBox(IDS_ADVSETCUSTOMTRAYICON,SystemTrayIconItems,DefaultCheckBoxProc,
			CSettingsProperties::settingsUseCustomTrayIcon,
			&m_pSettings->m_dwSettingsFlags,"sa_customSTicon"),
		NULL
	};



	/////////////////////////////////////////////////////////////////////////
	// Items under Miscellaneous category

	Item* MiscItems[]={
		CreateCheckBox(IDS_ADVSETDONTSHOWEXTINRENAME,NULL,DefaultCheckBoxProc,
			CSettingsProperties::settingsDontShowExtensionInRenameDialog,
			&m_pSettings->m_dwSettingsFlags,"sa_noextinrename"),
		CreateCheckBox(IDS_ADVSETSHOWCRITICALERRORS,NULL,
			DefaultCheckBoxProc,CLocateApp::pfShowCriticalErrors,
			&m_pSettings->m_dwProgramFlags,"sa_criticalerrors"),
		CreateCheckBox(IDS_ADVSETSHOWNONCRITICALERRORS,NULL,
			DefaultCheckBoxProc,CLocateApp::pfShowNonCriticalErrors,
			&m_pSettings->m_dwProgramFlags,"sa_noncriticalerrors"),
		CreateCheckBox(IDS_ADVSETCONFIRMATIONFORUPDATEDATABASES,NULL,DefaultCheckBoxProc,
			CLocateApp::pfAskConfirmationForUpdateDatabases,
			&m_pSettings->m_dwProgramFlags,"sa_confirmupdate"),
		CreateCheckBox(IDS_ADVSETICONSNOFILEACCESS,NULL,DefaultCheckBoxProc,
			CLocateApp::pfAvoidToAccessWhenReadingIcons,
			&m_pSettings->m_dwProgramFlags,"sa_iconsnofileaccess"),
		CreateCheckBox(IDS_ADVSETUSEDEFDIRECTORYICON,NULL,DefaultCheckBoxProc,
			CLocateApp::pfUseDefaultIconForDirectories,
			&m_pSettings->m_dwProgramFlags,"sa_defdiricons"),
		CreateCheckBox(IDS_ADVSETSIMPLEFILEMENU,NULL,DefaultCheckBoxProc,
			CSettingsProperties::settingsSimpleFileMenu,
			&m_pSettings->m_dwSettingsFlags,"sa_simplefilemenu"),
		CreateCheckBox(IDS_ADVSETDONTMOVETOOLTIPS,NULL,DefaultCheckBoxProc,
			CLocateDlg::efLVDontMoveTooltips,
			&m_pSettings->m_dwLocateDialogExtraFlags,"sa_dontmovetooltips"),
		CreateListBox(IDS_ADVSETEXECUTEMODE,ExecuteItemsModeProc,0,
			&m_pSettings->m_dwLocateDialogExtraFlags,"sa_executeitemsmode"),
		CreateCheckBox(IDS_ADVSETENABLEADDREFFIX,NULL,DefaultCheckBoxProc,
			CSettingsProperties::settingsEnableAddRefFix,
			&m_pSettings->m_dwSettingsFlags,"sa_enableaddreffix"),
		NULL
	};




	


	/////////////////////////////////////////////////////////////////////////
	// Items under system category
	
	Item* ShellContextMenuItems[]={
		CreateCheckBox(IDS_ADVSETLOCATEINMYCOMPUTER,NULL,
			DefaultCheckBoxProc,CSettingsProperties::cmLocateOnMyComputer,
			&m_pSettings->m_bAdvancedAndContextMenuFlag,"sa_locMyComp"),
		CreateCheckBox(IDS_ADVSETLOCATEINMYDOCUMENTS,NULL,
			DefaultCheckBoxProc,CSettingsProperties::cmLocateOnMyDocuments,
			&m_pSettings->m_bAdvancedAndContextMenuFlag,"sa_locMyDocu"),
		CreateCheckBox(IDS_ADVSETLOCATEINDRIVES,NULL,
			DefaultCheckBoxProc,CSettingsProperties::cmLocateOnDrives,
			&m_pSettings->m_bAdvancedAndContextMenuFlag,"sa_locdrives"),
		CreateCheckBox(IDS_ADVSETLOCATEINFOLFERS,NULL,
			DefaultCheckBoxProc,CSettingsProperties::cmLocateOnFolders,
			&m_pSettings->m_bAdvancedAndContextMenuFlag,"sa_locfolders"),
		CreateCheckBox(IDS_ADVSETUPDATEINMYCOMPUTER,NULL,
			DefaultCheckBoxProc,CSettingsProperties::cmUpdateOnMyComputer,
			&m_pSettings->m_bAdvancedAndContextMenuFlag,"sa_updMyComp"),
		NULL
	};

	Item* SystemItems[]={
		CreateCheckBox(IDS_ADVSETRUNATSYSTEMSTARTUP,NULL,
			DefaultCheckBoxProc,CSettingsProperties::settingsStartLocateAtStartup,
			&m_pSettings->m_dwSettingsFlags,"sa_locateatstartup"),
		CreateRoot(IDS_ADVSETSHELLCONTEXTMENU,ShellContextMenuItems,"sa_contextmenu"),
		NULL
	};

	

	/////////////////////////////////////////////////////////////////////////
	// Main sections
	
	Item* Parents[]={
		CreateRoot(IDS_ADVSETLOCATEPROCESS,LocateProcessAndResultsItems,"sa_locateandresults"),
		CreateRoot(IDS_ADVSETDIALOGS,LocateDialogItems,"sa_dialog"),
		CreateRoot(IDS_ADVSETUPDATEPROCESS,UpdateProcessItems,"sa_update"),
		CreateRoot(IDS_ADVSETSYSTEMTRAYICON,SystemTrayIcon,"sa_STicon"),
		CreateRoot(IDS_ADVSETMISCELLANEOUS,MiscItems,"sa_misc"),
		CreateRoot(IDS_ADVSETSYSTEM,SystemItems,"sa_system"),
		NULL};

	Initialize(Parents);

	CheckDlgButton(IDC_SEARCHDOWN,1);
	return FALSE;
}

BOOL CSettingsProperties::CAdvancedSettingsPage::OnCommand(WORD wID,WORD wNotifyCode,HWND hControl)
{
	int nid=GetDefID();
	switch (wID)
	{
	case IDC_FINDTEXT:
		switch (wNotifyCode)
		{
		case EN_SETFOCUS:
			SetDefID(IDC_FINDNEXT);
			::SendMessage(hControl,EM_SETSEL,0,-1);
			break;
		case EN_KILLFOCUS:
			SetDefID(IDOK);
			SetDlgItemStyle(IDC_FINDNEXT,GetDlgItemStyle(IDC_FINDNEXT)&~BS_DEFPUSHBUTTON);
			break;
		case EN_CHANGE:
			EnableDlgItem(IDC_FINDNEXT,GetDlgItemTextLength(IDC_FINDTEXT)>0);
			break;				
		case 1:
			DoSearch();
			break;
		}
		break;
	case IDC_FINDNEXT:
		DoSearch();
		break;
	}
	return COptionsPropertyPage::OnCommand(wID,wNotifyCode,hControl);
}



	


void CSettingsProperties::CAdvancedSettingsPage::DoSearch()
{
	
	CStringW SearchTerm;
	GetDlgItemText(IDC_FINDTEXT,SearchTerm);

	if (SearchTerm.IsEmpty())
		return;

	BOOL bDirection=IsDlgButtonChecked(IDC_SEARCHDOWN);
	
	HTREEITEM hSelectedItem=m_pTree->GetSelectedItem(),hItem;
	if (hSelectedItem!=NULL)
	{
		// Try starting from selected item
		hItem=FindItem(SearchTerm,!bDirection,TRUE,TRUE,hSelectedItem);
		if (hItem==NULL)
		{
			// Start from begining
			hItem=FindItem(SearchTerm,!bDirection,TRUE,TRUE,NULL,hSelectedItem);
		}
	}
	else
	{
		// Check the whole list
		hItem=FindItem(SearchTerm,!bDirection,TRUE,TRUE);
	}

	if (hItem==NULL)
	{
		CStringW Message;
		Message.Format(IDS_ADVSETCANNOTFINDOPTION,(LPCWSTR)SearchTerm);
		MessageBox(Message);
		SetFocus(IDC_FINDTEXT);		
	}
	else
	{
		m_pTree->SetFocus();
		m_pTree->SelectItem(hItem);
	}
}
		

void CSettingsProperties::CAdvancedSettingsPage::OnHelp(LPHELPINFO lphi)
{
	LPCWSTR szwHelpFile=GetApp()->m_szHelpFile;
	if (szwHelpFile!=NULL)
	{	
		LPCSTR pHelpID=GetHelpID(lphi);
		if (pHelpID!=NULL)
		{

			// Form path to help file
			CStringW sHelpFile=GetApp()->GetExeNameW();
			sHelpFile.FreeExtra(sHelpFile.FindLast(L'\\')+1);
			sHelpFile << szwHelpFile << "::/settings_advanced.htm#" << pHelpID;


			if (IsUnicodeSystem())
			{
				if (HtmlHelpW(*this,sHelpFile,HH_DISPLAY_TOPIC,NULL)!=NULL)
					return;
			}
			else
			{
				if (HtmlHelpA(*this,W2A(sHelpFile),HH_DISPLAY_TOPIC,NULL)!=NULL)
					return;
			}
		}		
	}
	
	
	if (HtmlHelp(HH_HELP_CONTEXT,HELP_SETTINGS_ADVANCED)==NULL)
		HtmlHelp(HH_DISPLAY_TOPIC,0);
}


void CSettingsProperties::CAdvancedSettingsPage::OnCancel()
{
	m_pSettings->SetSettingsFlags(CSettingsProperties::settingsCancelled);
	COptionsPropertyPage::OnCancel();
}


BOOL CALLBACK CSettingsProperties::CAdvancedSettingsPage::LimitResultsCheckBoxProc(COptionsPropertyPage::BASICPARAMS* pParams)
{
	switch (pParams->crReason)
	{
	case BASICPARAMS::Initialize:
		break;
	case BASICPARAMS::Get:
		pParams->bChecked=(*((LONG*)pParams->lParam))>0;
		break;
	case BASICPARAMS::Set:
		break;
	case BASICPARAMS::Apply:
		if (!pParams->bChecked)
			*((DWORD*)pParams->lParam)=0;
		break;
	case BASICPARAMS::ChangingValue:
		break;
	}
	return TRUE;
}

BOOL CALLBACK CSettingsProperties::CAdvancedSettingsPage::UpdateThreadPriorityProc(COptionsPropertyPage::BASICPARAMS* pParams)
{
	switch(pParams->crReason)
	{
	case BASICPARAMS::Initialize:
		if (((INITIALIZEPARAMS*)pParams)->hControl!=NULL)
		{
			CComboBox cb(((INITIALIZEPARAMS*)pParams)->hControl);
			cb.AddString(ID2W(IDS_PRIORITYHIGH));		
			cb.AddString(ID2W(IDS_PRIORITYABOVENORMAL));		
			cb.AddString(ID2W(IDS_PRIORITYNORMAL));		
			cb.AddString(ID2W(IDS_PRIORITYBELOWNORMAL));		
			cb.AddString(ID2W(IDS_PRIORITYLOW));		
			cb.AddString(ID2W(IDS_PRIORITYIDLE));		
			if (GetSystemFeaturesFlag()&efWinVista)
				cb.AddString(ID2W(IDS_PRIORITYBACKGROUND));		
				
		}
		break;
	case BASICPARAMS::Get:
		switch ( *((int*)pParams->lParam))
		{
		case THREAD_PRIORITY_HIGHEST:
            pParams->lValue=0;
			break;
		case THREAD_PRIORITY_ABOVE_NORMAL:
            pParams->lValue=1;
			break;
		case THREAD_PRIORITY_NORMAL:
            pParams->lValue=2;
			break;
		case THREAD_PRIORITY_BELOW_NORMAL:
            pParams->lValue=3;
			break;
		case THREAD_PRIORITY_LOWEST:
            pParams->lValue=4;
			break;
		case THREAD_PRIORITY_IDLE:
            pParams->lValue=5;
			break;
		case THREAD_MODE_BACKGROUND_BEGIN:
            pParams->lValue=6;
			break;
		default:
			pParams->lValue=2;
			break;
		}
		break;
	case BASICPARAMS::Set:
		break;
	case BASICPARAMS::Apply:
		switch (((COMBOAPPLYPARAMS*)pParams)->nCurSel)
		{
		case 0:
            *((int*)pParams->lParam)=THREAD_PRIORITY_HIGHEST;
			break;
		case 1:
            *((int*)pParams->lParam)=THREAD_PRIORITY_ABOVE_NORMAL;
			break;
		case 2:
            *((int*)pParams->lParam)=THREAD_PRIORITY_NORMAL;
			break;
		case 3:
            *((int*)pParams->lParam)=THREAD_PRIORITY_BELOW_NORMAL;
			break;
		case 4:
            *((int*)pParams->lParam)=THREAD_PRIORITY_LOWEST;
			break;
		case 5:
			*((int*)pParams->lParam)=THREAD_PRIORITY_IDLE;
			break;
		case 6:
			*((int*)pParams->lParam)=THREAD_MODE_BACKGROUND_BEGIN;
			break;
		}
		break;
	case BASICPARAMS::ChangingValue:
		break;
	}		
	return TRUE;
}

BOOL CALLBACK CSettingsProperties::CAdvancedSettingsPage::UpdateTooltipPositionProc(COptionsPropertyPage::BASICPARAMS* pParams)
{
	switch(pParams->crReason)
	{
	case BASICPARAMS::Initialize:
		if (((INITIALIZEPARAMS*)pParams)->hControl!=NULL)
		{
			CComboBox cb(((INITIALIZEPARAMS*)pParams)->hControl);

			cb.AddString(ID2W(IDS_ADVSETTOOLTIPPOSNEARCLOCK));		
			cb.AddString(ID2W(IDS_ADVSETTOOLTIPPOSUPPERLEFT));		
			cb.AddString(ID2W(IDS_ADVSETTOOLTIPPOSUPPERRIGHT));		
			cb.AddString(ID2W(IDS_ADVSETTOOLTIPPOSLOWERLEFT));		
			cb.AddString(ID2W(IDS_ADVSETTOOLTIPPOSLOWERRIGHT));		
			cb.AddString(ID2W(IDS_ADVSETTOOLTIPPOSLASTPOSITION));		
		}
		break;
	case BASICPARAMS::Get:
		switch ((*((DWORD*)pParams->lParam))&CLocateApp::pfUpdateTooltipPositionMask)
		{
		case CLocateApp::pfUpdateTooltipPositionUpLeft:
            pParams->lValue=1;
			break;
		case CLocateApp::pfUpdateTooltipPositionUpRight:
            pParams->lValue=2;
			break;
		case CLocateApp::pfUpdateTooltipPositionDownLeft:
            pParams->lValue=3;
			break;
		case CLocateApp::pfUpdateTooltipPositionDownRight:
            pParams->lValue=4;
			break;
		case CLocateApp::pfUpdateTooltipPositionLastPosition:
			pParams->lValue=5;
			break;
		default:
			pParams->lValue=0;
			break;
		}
		break;
	case BASICPARAMS::Set:
		break;
	case BASICPARAMS::Apply:
		(*((DWORD*)pParams->lParam))&=~CLocateApp::pfUpdateTooltipPositionMask;
		switch (((COMBOAPPLYPARAMS*)pParams)->nCurSel)
		{
		case 0:
            *((int*)pParams->lParam)|=CLocateApp::pfUpdateTooltipPositionDefault;
			break;
		case 1:
            *((int*)pParams->lParam)|=CLocateApp::pfUpdateTooltipPositionUpLeft;
			break;
		case 2:
            *((int*)pParams->lParam)|=CLocateApp::pfUpdateTooltipPositionUpRight;
			break;
		case 3:
            *((int*)pParams->lParam)|=CLocateApp::pfUpdateTooltipPositionDownLeft;
			break;
		case 4:
            *((int*)pParams->lParam)|=CLocateApp::pfUpdateTooltipPositionDownRight;
			break;
		case 5:
			*((int*)pParams->lParam)|=CLocateApp::pfUpdateTooltipPositionLastPosition;
			break;
		}
		break;
	case BASICPARAMS::ChangingValue:
		break;
	}		
	return TRUE;
}

BOOL CALLBACK CSettingsProperties::CAdvancedSettingsPage::UpdateTooltipTopmostProc(COptionsPropertyPage::BASICPARAMS* pParams)
{
	switch(pParams->crReason)
	{
	case BASICPARAMS::Initialize:
		if (((INITIALIZEPARAMS*)pParams)->hControl!=NULL)
		{
			CComboBox cb(((INITIALIZEPARAMS*)pParams)->hControl);
			
			cb.AddString(ID2W(IDS_ADVSETTOOLTIPALWAYSONTOP));		
			cb.AddString(ID2W(IDS_ADVSETTOOLTIPTOPIFFOREGROUNDNOTMAXIMIZED));		
			cb.AddString(ID2W(IDS_ADVSETTOOLTIPTOPIFFOREGROUNDNOTFULLSCREEN));		
			cb.AddString(ID2W(IDS_ADVSETTOOLTIPNEVERONTOP));		
		}
		break;
	case BASICPARAMS::Get:
		switch ((*((DWORD*)pParams->lParam))&CLocateApp::pfUpdateTooltipTopmostMask)
		{
		case CLocateApp::pfUpdateTooltipNoTopmostWhenForegroundWndMaximized:
            pParams->lValue=1;
			break;
		case CLocateApp::pfUpdateTooltipNoTopmostWhdnForegroundWndInFullScreen:
            pParams->lValue=2;
			break;
		case CLocateApp::pfUpdateTooltipNeverTopmost:
            pParams->lValue=3;
			break;
		default:
			pParams->lValue=0;
			break;
		}
		break;
	case BASICPARAMS::Set:
		break;
	case BASICPARAMS::Apply:
		(*((DWORD*)pParams->lParam))&=~CLocateApp::pfUpdateTooltipTopmostMask;
		switch (((COMBOAPPLYPARAMS*)pParams)->nCurSel)
		{
		case 0:
            *((int*)pParams->lParam)|=CLocateApp::pfUpdateTooltipAlwaysTopmost;
			break;
		case 1:
            *((int*)pParams->lParam)|=CLocateApp::pfUpdateTooltipNoTopmostWhenForegroundWndMaximized;
			break;
		case 2:
            *((int*)pParams->lParam)|=CLocateApp::pfUpdateTooltipNoTopmostWhdnForegroundWndInFullScreen;
			break;
		case 3:
            *((int*)pParams->lParam)|=CLocateApp::pfUpdateTooltipNeverTopmost;
			break;
		}
		break;
	case BASICPARAMS::ChangingValue:
		break;
	}		
	return TRUE;
}

BOOL CALLBACK CSettingsProperties::CAdvancedSettingsPage::TimeFormatComboProc(COptionsPropertyPage::BASICPARAMS* pParams)
{
	switch(pParams->crReason)
	{
	case BASICPARAMS::Initialize:
		// Date and time formats
		if (((INITIALIZEPARAMS*)pParams)->hControl!=NULL)
		{
			((CAdvancedSettingsPage*)pParams->pPage)->m_aBuffer.RemoveAll();
			EnumTimeFormats(EnumTimeFormatsProc,LOCALE_USER_DEFAULT,0);
			//EnumDateFormats(DateFormatsProc,LOCALE_USER_DEFAULT,DATE_SHORTDATE);
		
			if (IsUnicodeSystem())
				::SendMessageW(((INITIALIZEPARAMS*)pParams)->hControl,CB_ADDSTRING,0,(LPARAM)(LPCWSTR)ID2W(IDS_ADVSETSYSTEMDEFAULT));		
			else
				::SendMessage(((INITIALIZEPARAMS*)pParams)->hControl,CB_ADDSTRING,0,(LPARAM)(LPCSTR)ID2A(IDS_ADVSETSYSTEMDEFAULT));		

			::SendMessage(((INITIALIZEPARAMS*)pParams)->hControl,CB_SETCURSEL,0,0);		
	
			for (int i=0;i<((CAdvancedSettingsPage*)pParams->pPage)->m_aBuffer.GetSize();i++)
				::SendMessage(((INITIALIZEPARAMS*)pParams)->hControl,CB_ADDSTRING,0,(LPARAM)((CAdvancedSettingsPage*)pParams->pPage)->m_aBuffer[i]);
			
			((CAdvancedSettingsPage*)pParams->pPage)->m_aBuffer.RemoveAll();
		}
		break;
	case BASICPARAMS::Get:
		if (!((CAdvancedSettingsPage*)pParams->pPage)->m_pSettings->m_TimeFormat.IsEmpty())
			pParams->pData=alloccopy(((CAdvancedSettingsPage*)pParams->pPage)->m_pSettings->m_TimeFormat);
		else
			pParams->pData=NULL;
		break;
	case BASICPARAMS::Set:
		break;
	case BASICPARAMS::Apply:
		if (pParams->pData==NULL || ((COMBOAPPLYPARAMS*)pParams)->nCurSel==0)
			((CAdvancedSettingsPage*)pParams->pPage)->m_pSettings->m_TimeFormat.Empty();
		else
			((CAdvancedSettingsPage*)pParams->pPage)->m_pSettings->m_TimeFormat.Copy(pParams->pData);
		break;
	case BASICPARAMS::ChangingValue:
		break;
	}		
	return TRUE;
}

BOOL CALLBACK CSettingsProperties::CAdvancedSettingsPage::DateFormatComboProc(COptionsPropertyPage::BASICPARAMS* pParams)
{
	switch(pParams->crReason)
	{
	case BASICPARAMS::Initialize:
		// Date and time formats
		if (((INITIALIZEPARAMS*)pParams)->hControl!=NULL)
		{
			CComboBox cb(((INITIALIZEPARAMS*)pParams)->hControl);
			((CAdvancedSettingsPage*)pParams->pPage)->m_aBuffer.RemoveAll();
			EnumDateFormats(EnumDateFormatsProc,LOCALE_USER_DEFAULT,DATE_SHORTDATE);
		
			cb.AddString(ID2W(IDS_ADVSETSYSTEMDEFAULT));		
			cb.SetCurSel(0);		

			for (int i=0;i<((CAdvancedSettingsPage*)pParams->pPage)->m_aBuffer.GetSize();i++)
				cb.AddString(((CAdvancedSettingsPage*)pParams->pPage)->m_aBuffer[i]);
			
			((CAdvancedSettingsPage*)pParams->pPage)->m_aBuffer.RemoveAll();
		}
		break;
	case BASICPARAMS::Get:
		if (!((CAdvancedSettingsPage*)pParams->pPage)->m_pSettings->m_DateFormat.IsEmpty())
			pParams->pData=alloccopy(((CAdvancedSettingsPage*)pParams->pPage)->m_pSettings->m_DateFormat);
		else
			pParams->pData=NULL;
		break;
	case BASICPARAMS::Set:
		break;
	case BASICPARAMS::Apply:
		if (pParams->pData==NULL || ((COMBOAPPLYPARAMS*)pParams)->nCurSel==0)
			((CAdvancedSettingsPage*)pParams->pPage)->m_pSettings->m_DateFormat.Empty();
		else
			((CAdvancedSettingsPage*)pParams->pPage)->m_pSettings->m_DateFormat.Copy(pParams->pData);
		break;
	case BASICPARAMS::ChangingValue:
		break;
	}
	return TRUE;
}


BOOL CALLBACK CSettingsProperties::CAdvancedSettingsPage::FileSizeListProc(COptionsPropertyPage::BASICPARAMS* pParams)
{
	switch(pParams->crReason)
	{
	case BASICPARAMS::Initialize:
		// File size formats
		if (((INITIALIZEPARAMS*)pParams)->hControl!=NULL)
		{
			CComboBox cb(((INITIALIZEPARAMS*)pParams)->hControl);
			cb.AddString(ID2W(IDS_ADVSETFILESIZEFORMATLESS1KB));		
			cb.AddString(ID2W(IDS_ADVSETFILESIZEFORMATDEPENGINGSIZE));		
			cb.AddString(ID2W(IDS_ADVSETFILESIZEFORMATBYTES));		
			cb.AddString(ID2W(IDS_ADVSETFILESIZEFORMATBYTENOUNITS));		
			cb.AddString(ID2W(IDS_ADVSETFILESIZEFORMATKB));		
			cb.AddString(ID2W(IDS_ADVSETFILESIZEFORMATKBNOUNITS));		
			cb.AddString(ID2W(IDS_ADVSETFILESIZEFORMATMB));		
			cb.AddString(ID2W(IDS_ADVSETFILESIZEFORMATMBNOUNITS));		
			cb.AddString(ID2W(IDS_ADVSETFILESIZEFORMATLESS1MB));		
		}

		break;
	case BASICPARAMS::Get:
		pParams->lValue=*((CLocateApp::FileSizeFormats*)pParams->lParam);
		break;
	case BASICPARAMS::Set:
		break;
	case BASICPARAMS::Apply:
		*((LONG*)pParams->lParam)=((COMBOAPPLYPARAMS*)pParams)->nCurSel;
		break;
	case BASICPARAMS::ChangingValue:
		break;
	}
	return TRUE;
}

BOOL CALLBACK CSettingsProperties::CAdvancedSettingsPage::SortingMethodProc(COptionsPropertyPage::BASICPARAMS* pParams)
{
	switch(pParams->crReason)
	{
	case BASICPARAMS::Initialize:
		// File size formats
		if (((INITIALIZEPARAMS*)pParams)->hControl!=NULL)
		{
			CComboBox cb(((INITIALIZEPARAMS*)pParams)->hControl);
			cb.AddString(ID2W(IDS_ADVSETSORTINGSTANDARD));		
			cb.AddString(ID2W(IDS_ADVSETSORTINGSTANDARDCS));		
			cb.AddString(ID2W(IDS_ADVSETSORTINGNATURAL));		
			cb.AddString(ID2W(IDS_ADVSETSORTINGNATURALCS));		
		}
		break;
	case BASICPARAMS::Get:
		pParams->lValue=*((DWORD*)pParams->lParam);
		break;
	case BASICPARAMS::Set:
		break;
	case BASICPARAMS::Apply:
		*((DWORD*)pParams->lParam)=((COMBOAPPLYPARAMS*)pParams)->nCurSel;
		break;
	case BASICPARAMS::ChangingValue:
		break;
	}
	return TRUE;
}


BOOL CALLBACK CSettingsProperties::CAdvancedSettingsPage::EnumTimeFormatsProc(LPSTR lpTimeFormatString)
{
	GetTrayIconWnd()->GetSettingsDialog()->m_pAdvanced->m_aBuffer.Add(alloccopy(lpTimeFormatString));
	return TRUE;
}

BOOL CALLBACK CSettingsProperties::CAdvancedSettingsPage::EnumDateFormatsProc(LPSTR lpDateFormatString)
{
	GetTrayIconWnd()->GetSettingsDialog()->m_pAdvanced->m_aBuffer.Add(alloccopy(lpDateFormatString));
	return TRUE;
}

BOOL CALLBACK CSettingsProperties::CAdvancedSettingsPage::ExternalCommandProc(BASICPARAMS* pParams)
{
	if (pParams->crReason==BASICPARAMS::BrowseFile)
	{
		((BROWSEDLGPARAMS*)pParams)->szTitle=MAKEINTRESOURCEW(IDS_ADVSETSELECTPROGRAM);
		((BROWSEDLGPARAMS*)pParams)->szFilters=MAKEINTRESOURCEW(IDS_PROGRAMSFILTERS);
		return TRUE;
	}
	return DefaultEditStrWProc(pParams);
}


BOOL CALLBACK CSettingsProperties::CAdvancedSettingsPage::TrayIconProc(BASICPARAMS* pParams)
{
	if (pParams->crReason==BASICPARAMS::BrowseFile)
	{
		((BROWSEDLGPARAMS*)pParams)->szTitle=MAKEINTRESOURCEW(IDS_ADVSETSELECTICONFILE);
		((BROWSEDLGPARAMS*)pParams)->szFilters=MAKEINTRESOURCEW(IDS_ICONFILTERS);
		return TRUE;
	}
	return DefaultEditStrWProc(pParams);
}

BOOL CALLBACK CSettingsProperties::CAdvancedSettingsPage::ExecuteItemsModeProc(COptionsPropertyPage::BASICPARAMS* pParams)
{
	switch(pParams->crReason)
	{
	case BASICPARAMS::Initialize:
		if (((INITIALIZEPARAMS*)pParams)->hControl!=NULL)
		{
			CComboBox cb(((INITIALIZEPARAMS*)pParams)->hControl);
			cb.AddString(ID2W(IDS_ADVSETEXECUTEMODEDEFAULT));		
			cb.AddString(ID2W(IDS_ADVSETEXECUTEMODECONTEXT));		
			
		}
		break;
	case BASICPARAMS::Get:
		switch ((*((DWORD*)pParams->lParam))&CLocateDlg::efExecuteModeMask)
		{
		case CLocateDlg::efExecuteModeBoldedContextMenuItemMode:
            pParams->lValue=1;
			break;
		default:
            pParams->lValue=0;
			break;
		}
		break;
	case BASICPARAMS::Set:
		break;
	case BASICPARAMS::Apply:
		(*((DWORD*)pParams->lParam))&=~CLocateDlg::efExecuteModeMask;
		switch (((COMBOAPPLYPARAMS*)pParams)->nCurSel)
		{
		case 0:
			*((int*)pParams->lParam)|=CLocateDlg::efExecuteModeDefaultMode;
			break;
		case 1:
            *((int*)pParams->lParam)|=CLocateDlg::efExecuteModeBoldedContextMenuItemMode;
			break;
		}
		break;
	case BASICPARAMS::ChangingValue:
		break;
	}		
	return TRUE;
}

////////////////////////////////////////
// CLanguageSettingsPage
////////////////////////////////////////


BOOL CSettingsProperties::CLanguageSettingsPage::OnInitDialog(HWND hwndFocus)
{
	CPropertyPage::OnInitDialog(hwndFocus);

	m_pList=new CListCtrl(GetDlgItem(IDC_LANGUAGE));
	if (IsUnicodeSystem())
		m_pList->SetUnicodeFormat(TRUE);
	
	m_pList->InsertColumn(0,ID2W(IDS_LANGUAGE),LVCFMT_LEFT,90);
	m_pList->InsertColumn(1,ID2W(IDS_LANGUAGEFILE),LVCFMT_LEFT,80);
	m_pList->InsertColumn(2,ID2W(IDS_LANGUAGEDESC),LVCFMT_LEFT,150);
	m_pList->InsertColumn(3,ID2W(IDS_LANGUAGEVERSION),LVCFMT_LEFT,140);
	
	m_pList->SetExtendedListViewStyle(LVS_EX_HEADERDRAGDROP|LVS_EX_FULLROWSELECT ,LVS_EX_HEADERDRAGDROP|LVS_EX_FULLROWSELECT );
	m_pList->LoadColumnsState(HKCU,CRegKey2::GetCommonKey()+"\\Dialogs","Language Settings List Widths");

	DWORD dwTemp,nLen;
	CStringW sExeName=GetApp()->GetExeNameW();
	dwVersionHi=0;
	dwVersionLo=0;
	if (IsUnicodeSystem())
		nLen=GetFileVersionInfoSizeW(sExeName,&dwTemp);
	else
		nLen=GetFileVersionInfoSizeA(W2A(sExeName),&dwTemp);
	if (nLen>0)
	{
		BYTE* pVersion=new BYTE[nLen+2];
		if (IsUnicodeSystem())
			nLen=GetFileVersionInfoW(sExeName,0,nLen,pVersion);
		else
			nLen=GetFileVersionInfoA(W2A(sExeName),0,nLen,pVersion);
			
		if (nLen>0)
		{
			VS_FIXEDFILEINFO* pffi;
			UINT nflen;
			VerQueryValue(pVersion,"\\",(void**)&pffi,&nflen);
			dwVersionHi=pffi->dwFileVersionMS;
			dwVersionLo=pffi->dwFileVersionLS;
		}       
		delete[] pVersion;
	}

	

	FindLanguages();
	
	CheckDlgButton(IDC_USEWITHCONSOLEAPPS,
		m_pSettings->IsSettingsFlagSet(CSettingsProperties::settingsUseLanguageWithConsoleApps));
	return FALSE;
}

void CSettingsProperties::CLanguageSettingsPage::OnHelp(LPHELPINFO lphi)
{
	if (HtmlHelp(HH_HELP_CONTEXT,HELP_SETTINGS_LANGUAGES)==NULL)
		HtmlHelp(HH_DISPLAY_TOPIC,0);
}

BOOL CSettingsProperties::CLanguageSettingsPage::OnApply()
{
	CPropertyPage::OnApply();
	
    m_pSettings->SetSettingsFlags(settingsUseLanguageWithConsoleApps,IsDlgButtonChecked(IDC_USEWITHCONSOLEAPPS));

	int nItem=m_pList->GetNextItem(-1,LVNI_SELECTED);
	if (nItem!=-1)
	{
		LanguageItem* pli=(LanguageItem*)m_pList->GetItemData(nItem);
		
		if (m_pSettings->m_strLangFile.CompareNoCase(pli->sFile)!=0)
			ShowErrorMessage(IDS_LANGUAGENOTE,IDS_NOTE,MB_ICONINFORMATION|MB_OK);
		
		m_pSettings->m_strLangFile=pli->sFile;
	}
	return TRUE;
}

void CSettingsProperties::CLanguageSettingsPage::OnDestroy()
{
	CPropertyPage::OnDestroy();

	if (m_pList!=NULL)
	{
		m_pList->SaveColumnsState(HKCU,CRegKey2::GetCommonKey()+"\\Dialogs","Language Settings List Widths");
		delete m_pList;
		m_pList=NULL;
	}
}
		
void CSettingsProperties::CLanguageSettingsPage::OnCancel()
{
	m_pSettings->SetSettingsFlags(CSettingsProperties::settingsCancelled);

	CPropertyPage::OnCancel();
}

void CSettingsProperties::CLanguageSettingsPage::OnTimer(DWORD wTimerID)
{
	KillTimer(wTimerID);

	if (m_pList->GetNextItem(-1,LVNI_SELECTED)==-1)
		m_pList->SetItemState(nLastSel,LVIS_SELECTED,LVIS_SELECTED);
}

BOOL CSettingsProperties::CLanguageSettingsPage::OnNotify(int idCtrl,LPNMHDR pnmh)
{
	switch (idCtrl)
	{
	case IDC_LANGUAGE:
		return ListNotifyHandler((NMLISTVIEW*)pnmh);
	}
	return CPropertyPage::OnNotify(idCtrl,pnmh);
}


BOOL CSettingsProperties::CLanguageSettingsPage::ListNotifyHandler(NMLISTVIEW *pNm)
{
	switch(pNm->hdr.code)
	{
	case LVN_DELETEITEM:
		delete (LanguageItem*)pNm->lParam;
		break;
	case LVN_GETDISPINFO:
		{
			LV_DISPINFO *pLvdi=(LV_DISPINFO *)pNm;
			LanguageItem* li=(LanguageItem*)pLvdi->item.lParam;
            if (li==NULL)
				break;
			
			pLvdi->item.mask=LVIF_TEXT|LVIF_DI_SETITEM;

			switch (pLvdi->item.iSubItem)
			{
			case 0:
				pLvdi->item.pszText=alloccopyWtoA(li->sLanguage,li->sLanguage.GetLength());
				AssignBuffer(pLvdi->item.pszText);
				break;
			case 1:
				pLvdi->item.pszText=alloccopyWtoA(li->sFile,li->sFile.GetLength());
				AssignBuffer(pLvdi->item.pszText);
				break;
			case 2:
				pLvdi->item.pszText=alloccopyWtoA(li->sDescription,li->sDescription.GetLength());
				AssignBuffer(pLvdi->item.pszText);
				break;
			case 3:
				pLvdi->item.pszText=alloccopyWtoA(li->sForVersion,li->sForVersion.GetLength());
				AssignBuffer(pLvdi->item.pszText);
				break;
			}
			break;
		}
	case LVN_GETDISPINFOW:
		{
			LV_DISPINFOW *pLvdi=(LV_DISPINFOW *)pNm;
			LanguageItem* li=(LanguageItem*)pLvdi->item.lParam;
            if (li==NULL)
				break;
			
			pLvdi->item.mask=LVIF_TEXT|LVIF_DI_SETITEM;

			switch (pLvdi->item.iSubItem)
			{
			case 0:
				pLvdi->item.pszText=li->sLanguage.GetBuffer();
				break;
			case 1:
				pLvdi->item.pszText=li->sFile.GetBuffer();
				break;
			case 2:
				pLvdi->item.pszText=li->sDescription.GetBuffer();
				break;
			case 3:
				pLvdi->item.pszText=li->sForVersion.GetBuffer();
				break;
			}
			break;
		}
	case LVN_ITEMCHANGED:
		if ((pNm->uOldState&LVIS_SELECTED)==0 && (pNm->uNewState&LVIS_SELECTED))
		{
			nLastSel=pNm->iItem;
			KillTimer(0);
		}
		if ((pNm->uOldState&LVIS_SELECTED) && (pNm->uNewState&LVIS_SELECTED)==0)
			SetTimer(0,100,NULL);
		break;
	case NM_CUSTOMDRAW:
		{
			SetWindowLong(dwlMsgResult,
				ListCustomDrawHandler((NMLVCUSTOMDRAW*)pNm));
			return TRUE;
		}
	}
	return FALSE;
}


HRESULT CSettingsProperties::CLanguageSettingsPage::ListCustomDrawHandler(NMLVCUSTOMDRAW* pLVCD)
{
	switch (pLVCD->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		// We want notification messages
		return CDRF_NOTIFYITEMDRAW;
	case CDDS_ITEMPREPAINT:
		return CDRF_NOTIFYSUBITEMDRAW;
	case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
		if (pLVCD->iSubItem==3 &&  // Version
			pLVCD->nmcd.lItemlParam!=NULL && dwVersionHi!=0) 
		{
			LanguageItem* pItem=(LanguageItem*)pLVCD->nmcd.lItemlParam;
				
			if ((HFONT)m_fBoldFont==NULL)
			{
				char szFace[100];
				TEXTMETRIC tm;
				GetTextFace(pLVCD->nmcd.hdc,100,szFace);
				GetTextMetrics(pLVCD->nmcd.hdc,&tm);
				m_fBoldFont.CreateFont(tm.tmHeight,0,0,0,tm.tmWeight+300,tm.tmItalic,tm.tmUnderlined,tm.tmStruckOut,
					tm.tmCharSet,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,tm.tmPitchAndFamily,szFace);
			}

			if (pItem->dwForVersionHi!=dwVersionHi)
			{
				if (m_pList->GetItemState((int)pLVCD->nmcd.dwItemSpec,LVIS_SELECTED)&LVIS_SELECTED)
					SelectObject(pLVCD->nmcd.hdc,m_fBoldFont);
				pLVCD->clrText = RGB(0xFF, 0, 0 );
				return CDRF_NEWFONT;
			}
			else if (pItem->dwForVersionLo<dwVersionLo)
			{
				if (m_pList->GetItemState((int)pLVCD->nmcd.dwItemSpec,LVIS_SELECTED)&LVIS_SELECTED)
					SelectObject(pLVCD->nmcd.hdc,m_fBoldFont);
				pLVCD->clrText = RGB(0xFF, 0, 0 );
				return CDRF_NEWFONT;
			}
			else if (pItem->dwForVersionLo>dwVersionLo)
			{
				if (m_pList->GetItemState((int)pLVCD->nmcd.dwItemSpec,LVIS_SELECTED)&LVIS_SELECTED)
					SelectObject(pLVCD->nmcd.hdc,m_fBoldFont);
				pLVCD->clrText = RGB(0, 0, 0xFF );
				return CDRF_NEWFONT;
			}

		}
		return CDRF_DODEFAULT;
	default:
		return CDRF_DODEFAULT;
	}
}
		
void CSettingsProperties::CLanguageSettingsPage::FindLanguages()
{
	typedef void  (*LANGCALL)(
		LPSTR /* OUT */ szLanguage,
		DWORD /* IN  */ dwMaxLanguageLength,
		LPSTR /* OUT */ szDescription,
		DWORD /* IN  */ dwMaxDescriptionLength);
	typedef void  (*LANGCALLW)(
		LPWSTR /* OUT */ szLanguage,
		DWORD /* IN  */ dwMaxLanguageLength,
		LPWSTR /* OUT */ szDescription,
		DWORD /* IN  */ dwMaxDescriptionLength);

	CStringW Path(GetApp()->GetExeNameW());
	Path.FreeExtra(Path.FindLast('\\')+1);
	Path<<L"*.dll";

	
	LVITEM li;
	li.mask=LVIF_PARAM|LVIF_STATE|LVIF_TEXT;
	li.iItem=0;
	li.pszText=LPSTR_TEXTCALLBACK;
	li.stateMask=LVIS_SELECTED;

	CFileFind ff;
	BOOL bRet=ff.FindFile(Path);


	OSVERSIONINFO ver;
	ver.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
	BOOL bDataFileOk=GetVersionEx(&ver);
	bDataFileOk=bDataFileOk && ver.dwPlatformId==VER_PLATFORM_WIN32_NT && 
		(ver.dwMajorVersion>5 || (ver.dwMajorVersion==5 && ver.dwMinorVersion>=1));


    while (bRet)
	{
		WCHAR szPathTemp[MAX_PATH];
		ff.GetFilePath(szPathTemp,MAX_PATH);
		
		// Check whether file can be loaded as datafile
		HINSTANCE hLib=FileSystem::LoadLibrary(szPathTemp,
			bDataFileOk?LOAD_LIBRARY_AS_DATAFILE:NULL);
        if (hLib==NULL)
		{
			bRet=ff.FindNextFile();
			continue;
		}

		FreeLibrary(hLib);




		LanguageItem* pli=new LanguageItem;
				
		if (IsUnicodeSystem())
		{
			// Retrieving language from version resource
			if (!GetVersionText(szPathTemp,"ProvidesLanguage",pli->sLanguage.GetBuffer(200),200))
			{
				delete pli;
				bRet=ff.FindNextFile();
				continue;
			}
				
			pli->sLanguage.FreeExtra();


			// Retrieving description from version resource
			if (GetVersionText(szPathTemp,"FileDescription",pli->sDescription.GetBuffer(1000),1000))
				pli->sDescription.FreeExtra();
			else
				pli->sDescription.Empty();

			// Retrieving description from version resource
			if (GetVersionText(szPathTemp,"ProductVersion",pli->sForVersion.GetBuffer(1000),1000))
				pli->sForVersion.FreeExtra();
			else
				pli->sForVersion.Empty();
		}
		else
		{
			char szLanguage[200],szTemp[1000];
			// Retrieving language from version resource
			if (!GetVersionText(W2A(szPathTemp),"ProvidesLanguage",szLanguage,200))
			{
				delete pli;
				bRet=ff.FindNextFile();
				continue;
			}
				
				
			pli->sLanguage=szLanguage;

			// Retrieving description from version resource
			if (GetVersionText(W2A(szPathTemp),"FileDescription",szTemp,1000))
				pli->sDescription=szTemp;

			// Retrieving description from version resource
			if (GetVersionText(W2A(szPathTemp),"ProductVersion",szTemp,1000))
				pli->sForVersion=szTemp;
		}

		// Get numeric version
		DWORD dwTemp,nLen;
		pli->dwForVersionHi=0;
		pli->dwForVersionLo=0;
		if (IsUnicodeSystem())
			nLen=GetFileVersionInfoSizeW(szPathTemp,&dwTemp);
		else
			nLen=GetFileVersionInfoSizeA(W2A(szPathTemp),&dwTemp);
		if (nLen>0)
		{
			BYTE* pVersion=new BYTE[nLen+2];
			if (IsUnicodeSystem())
				nLen=GetFileVersionInfoW(szPathTemp,0,nLen,pVersion);
			else
				nLen=GetFileVersionInfoA(W2A(szPathTemp),0,nLen,pVersion);

			
			if (nLen>0)
			{
				VS_FIXEDFILEINFO* pffi;
				UINT nflen;
				VerQueryValue(pVersion,"\\",(void**)&pffi,&nflen);
				pli->dwForVersionHi=pffi->dwFileVersionMS;
				pli->dwForVersionLo=pffi->dwFileVersionLS;
			}       

			delete[] pVersion;
		}



		ff.GetFileName(pli->sFile);
		li.lParam=(LPARAM)pli;
		li.iSubItem=0;
		if (m_pSettings->m_strLangFile.CompareNoCase(pli->sFile)==0)
		{
			li.state=LVIS_SELECTED;
			nLastSel=li.iItem;
		}
		else
			li.state=0;


		
		m_pList->InsertItem(&li);
		li.iItem++;

		bRet=ff.FindNextFile();
	}
}




////////////////////////////////////////
// CDatabasesSettingsPage
////////////////////////////////////////


BOOL CSettingsProperties::CDatabasesSettingsPage::OnInitDialog(HWND hwndFocus)
{
	CPropertyPage::OnInitDialog(hwndFocus);

	// Initializing list control
	m_pList=new CListCtrl(GetDlgItem(IDC_DATABASES));
	m_pList->SetExtendedListViewStyle(LVS_EX_CHECKBOXES|LVS_EX_HEADERDRAGDROP|LVS_EX_FULLROWSELECT,
		LVS_EX_CHECKBOXES|LVS_EX_HEADERDRAGDROP|LVS_EX_FULLROWSELECT);
	m_pList->InsertColumn(0,ID2W(IDS_DATABASENAME),LVCFMT_LEFT,100,0);
	m_pList->InsertColumn(1,ID2W(IDS_DATABASEFILE),LVCFMT_LEFT,130,0);
	m_pList->InsertColumn(2,ID2W(IDS_GLOBALUPDATE),LVCFMT_LEFT,70,0);
	m_pList->InsertColumn(3,ID2W(IDS_LASTTIMEUPDATED),LVCFMT_LEFT,130,0);
	
	if (((CLocateApp*)GetApp())->m_wComCtrlVersion<0x0600)
	{
		m_pList->InsertColumn(4,ID2W(IDS_THREADID),LVCFMT_LEFT,40,0);
		int oa[]={3,0,1,2};
		m_pList->SetColumnOrderArray(4,oa);
	}
	if (IsUnicodeSystem())
		m_pList->SetUnicodeFormat(TRUE);

	m_pList->LoadColumnsState(HKCU,CRegKey2::GetCommonKey()+"\\Dialogs","Databases Settings List Widths");
	
	// Setting threads counter
	CSpinButtonCtrl Spin(GetDlgItem(IDC_THREADSPIN));
	Spin.SetBuddy(GetDlgItem(IDC_THREADS));
	Spin.SetRange(1,99);

	SetDatabasesToList();


	if (m_pSettings->IsSettingsFlagSet(CSettingsProperties::settingsDatabasesOverridden))
	{
		// Databases are overridden via command line parameters
		// Disabling new button
		EnableDlgItem(IDC_NEW,FALSE);

		EnableDlgItem(IDC_THREADS,FALSE);
		EnableDlgItem(IDC_THREADSPIN,FALSE);

		ShowDlgItem(IDC_THREADS,swHide);
		ShowDlgItem(IDC_THREADSPIN,swHide);
	}
	else
	{

		ShowDlgItem(IDC_OVERRIDETXT,swHide);
		ShowDlgItem(IDC_RESTORE,swHide);
	}

	EnableButtons();	
	return FALSE;
}

void CSettingsProperties::CDatabasesSettingsPage::OnHelp(LPHELPINFO lphi)
{
	CLocateApp::HelpID id[]= {
		{ IDC_DATABASES,"sd_list" },
		{ IDC_NEW,"sd_new" },
		{ IDC_EDIT,"sd_edit" },
		{ IDC_REMOVE,"sd_remove" },
		{ IDC_ENABLE,"sd_enable" },
		{ IDC_DISABLE,"sd_enable" },
		{ IDC_UPDATE,"sd_globalupdate" },
		{ IDC_UP,"sd_up" },
		{ IDC_DOWN,"sd_up" },
		{ IDC_IMPORT,"sd_import" },
		{ IDC_EXPORT,"sd_export" },
		{ IDC_THREADS,"sd_threads" },
		{ IDC_THREADSPIN,"sd_threads" },
		{ IDC_RESTORE,"sd_restore" }
	};
	
	if (CLocateApp::OpenHelp(*this,"settings_databases.htm",lphi,id,sizeof(id)/sizeof(CLocateApp::HelpID)))
		return;

	if (HtmlHelp(HH_HELP_CONTEXT,HELP_SETTINGS_DATABASES)==NULL)
		HtmlHelp(HH_DISPLAY_TOPIC,0);
}

void CSettingsProperties::CDatabasesSettingsPage::SetDatabasesToList()
{
	if (m_nThreadsCurrently>1)
		RemoveThreadGroups();
	
	m_nThreadsCurrently=CDatabase::CheckIDs(m_pSettings->m_aDatabases);
	
	SendDlgItemMessage(IDC_THREADSPIN,UDM_SETPOS,0,MAKELONG(m_nThreadsCurrently,0));
	

	LVITEM li;
	li.pszText=LPSTR_TEXTCALLBACK;
	if (m_nThreadsCurrently>1 && ((CLocateApp*)GetApp())->m_wComCtrlVersion>=0x0600)
	{
        li.mask=LVIF_TEXT|LVIF_PARAM|LVIF_GROUPID;

		EnableThreadGroups(m_nThreadsCurrently);
	}
	else
		li.mask=LVIF_TEXT|LVIF_PARAM;
			
	li.iSubItem=0;
	
	for (li.iItem=0;li.iItem<m_pSettings->m_aDatabases.GetSize();li.iItem++)
	{
		CDatabase* pDatabase=m_pSettings->m_aDatabases[li.iItem];
		li.lParam=LPARAM(pDatabase);
		li.iGroupId=pDatabase->GetThreadId();

		m_pList->SetCheckState(
			m_pList->InsertItem(&li),
			pDatabase->IsEnabled());
	}
	
	// Removing m_aDatabases without freeing cells
	delete[] m_pSettings->m_aDatabases.GiveBuffer();
}

void CSettingsProperties::CDatabasesSettingsPage::OnDestroy()
{
	if (m_pList!=NULL)
	{
		m_pList->SaveColumnsState(HKCU,CRegKey2::GetCommonKey()+"\\Dialogs","Databases Settings List Widths");
		delete m_pList;
		m_pList=NULL;
	}

	
	// Terminate existing modification date reading threads
	EnterCriticalSection(&m_csModDateThreads);
	for (int i=0;i<m_ModDateThreads.GetSize();i++)
	{
		if (::TerminateThread(m_ModDateThreads[i]->hThread,1,TRUE))
		{
			SdDebugMessage1("SDLG: terminated date reading thread for %S",m_ModDateThreads[i]->sDatabaseFile);
			delete m_ModDateThreads[i];
		}
	}
	m_ModDateThreads.RemoveAll();
	LeaveCriticalSection(&m_csModDateThreads);
	DeleteCriticalSection(&m_csModDateThreads);

	CPropertyPage::OnDestroy();
}

BOOL CSettingsProperties::CDatabasesSettingsPage::OnCommand(WORD wID,WORD wNotifyCode,HWND hControl)
{
	switch (wID)
	{
	case IDC_NEW:
		OnNew();
		break;
	case IDC_EDIT:
		OnEdit();
		break;
	case IDC_REMOVE:
		OnRemove();
		break;
	case IDC_ENABLE:
		OnEnable(TRUE);
		break;
	case IDC_DISABLE:
		OnEnable(FALSE);
		break;
	case IDC_UP:
		ItemUpOrDown(TRUE);
		break;
	case IDC_DOWN:
		ItemUpOrDown(FALSE);
		break;
	case IDC_UPDATE:
		OnUpdate();
		break;
	case IDC_RESTORE:
		OnRestore();
		break;
	case IDC_IMPORT:
		OnImport();
		break;
	case IDC_EXPORT:
		OnExport();
		break;
	case IDC_THREADS:
		if (wNotifyCode==EN_CHANGE)
			OnThreads();
		else if (wNotifyCode==EN_SETFOCUS)
			SendDlgItemMessage(IDC_THREADS,EM_SETSEL,0,-1);
		break;
	}
	return CPropertyPage::OnCommand(wID,wNotifyCode,hControl);
}

BOOL CSettingsProperties::CDatabasesSettingsPage::OnNew(CDatabase* pDatabaseTempl)
{
	CWaitCursor wait;

	CDatabaseDialog dbd;
	if (pDatabaseTempl!=NULL)
		dbd.m_pDatabase=pDatabaseTempl;
	else
		dbd.m_pDatabase=CDatabase::FromDefaults(FALSE);
	dbd.m_nMaximumNumbersOfThreads=m_nThreadsCurrently;
	
	if (m_pList->GetItemCount()>0)
	{
		int iItem=m_pList->GetNextItem(-1,LVNI_ALL);
		while (iItem!=-1)
		{
			CDatabase* pDatabase=(CDatabase*)m_pList->GetItemData(iItem);
			if (pDatabase!=NULL)
				dbd.m_aOtherDatabases.Add(pDatabase);
			iItem=m_pList->GetNextItem(iItem,LVNI_ALL);
		}
	}

	BOOL bRet=FALSE;

	if (dbd.DoModal(*this,LanguageSpecificResource))
	{
		LVITEM li;
		
		if (m_pList->GetItemCount()>0)
		{
			li.iItem=m_pList->GetNextItem(-1,LVNI_ALL);
			while (li.iItem!=-1)
			{
				CDatabase* pDatabase=(CDatabase*)m_pList->GetItemData(li.iItem);
				ASSERT(pDatabase!=NULL);

				if (pDatabase->GetThreadId()>dbd.m_pDatabase->GetThreadId())
					break;

				li.iItem=m_pList->GetNextItem(li.iItem,LVNI_ALL);
			}

			// Inserting to the end of list
			if (li.iItem==-1)
				li.iItem=m_pList->GetItemCount();

		}
		else
			li.iItem=0;

		
		li.pszText=LPSTR_TEXTCALLBACK;
		if (m_nThreadsCurrently>1 && ((CLocateApp*)GetApp())->m_wComCtrlVersion>=0x0600)
			li.mask=LVIF_TEXT|LVIF_PARAM|LVIF_STATE|LVIF_GROUPID;
		else
			li.mask=LVIF_TEXT|LVIF_PARAM|LVIF_STATE;
		li.iSubItem=0;
		li.lParam=LPARAM(dbd.m_pDatabase);
		li.iGroupId=dbd.m_pDatabase->GetThreadId();
		li.state=LVIS_SELECTED;
		li.stateMask=LVIS_SELECTED;
		
		// LVM_ITEMCHANGED notification (which will be send when
		// InsertItem is called) will change the status to disabled
		BOOL bEnabled=dbd.m_pDatabase->IsEnabled();
		
		li.iItem=m_pList->InsertItem(&li);

		m_pList->SetCheckState(li.iItem,bEnabled);

		m_pList->EnsureVisible(li.iItem,FALSE);

		bRet=TRUE;
	}
	else
		delete dbd.m_pDatabase;


	EnableButtons();
	m_pList->SetFocus();

	return bRet;
}

void CSettingsProperties::CDatabasesSettingsPage::OnEdit()
{
	CWaitCursor wait;

	ASSERT(m_pList->GetItemCount()>0);

	CDatabaseDialog dbd;

	if (m_pSettings->IsSettingsFlagSet(CSettingsProperties::settingsDatabasesOverridden))
		dbd.m_bDontEditName=TRUE;

	int nItem=m_pList->GetNextItem(-1,LVNI_SELECTED);
	if (nItem==-1)
		return;
	
	CDatabase* pOrigDatabase=(CDatabase*)m_pList->GetItemData(nItem);
	ASSERT(pOrigDatabase!=NULL);

	dbd.m_pDatabase=new CDatabase(*pOrigDatabase);
	dbd.m_nMaximumNumbersOfThreads=m_nThreadsCurrently;
	
	int iOtherItem=m_pList->GetNextItem(-1,LVNI_ALL);
	while (iOtherItem!=-1)
	{
		CDatabase* pDatabase=(CDatabase*)m_pList->GetItemData(iOtherItem);
		if (pDatabase!=NULL && pDatabase!=pOrigDatabase)
			dbd.m_aOtherDatabases.Add(pDatabase);
		iOtherItem=m_pList->GetNextItem(iOtherItem,LVNI_ALL);
	}


	if (dbd.DoModal(*this,LanguageSpecificResource))
	{
		delete (CDatabase*)m_pList->GetItemData(nItem);
		m_pList->SetItemData(nItem,LPARAM(dbd.m_pDatabase));

		m_pList->RedrawItems(nItem,nItem);
		m_pList->SetCheckState(nItem,dbd.m_pDatabase->IsEnabled());

		if (((CLocateApp*)GetApp())->m_wComCtrlVersion>=0x0600 && m_nThreadsCurrently>1)
		{
			LVITEM li;
			li.iItem=nItem;
			li.iSubItem=0;
			li.mask=LVIF_GROUPID;
			li.iGroupId=dbd.m_pDatabase->GetThreadId();
			m_pList->SetItem(&li);
		}
	}
	else
		delete dbd.m_pDatabase;


	if (m_nThreadsCurrently>1 && GetLocateApp()->m_wComCtrlVersion<0x0600)
		m_pList->SortItems(ThreadSortProc,NULL);

	EnableButtons();
	m_pList->SetFocus();
}

void CSettingsProperties::CDatabasesSettingsPage::OnRemove()
{
	int nItem=m_pList->GetNextItem(-1,LVNI_SELECTED);
	if (nItem==-1)
		return;

	CDatabase* pDatabase=(CDatabase*)m_pList->GetItemData(nItem);
	if (pDatabase->GetArchiveType()==CDatabase::archiveFile)
	{
		BOOL bFree;
		LPWSTR pFileName=pDatabase->GetResolvedArchiveName(bFree);
		if (FileSystem::IsFile(pFileName))
		{
			DWORD dwNoDelete=FALSE;

			CRegKey2 RegKey;
			if (RegKey.OpenKey(HKCU,"\\General",CRegKey::defRead)==ERROR_SUCCESS)
				RegKey.QueryValue("NoDeleteDatabaseFile",dwNoDelete);

			if (!dwNoDelete)
			{
				CStringW str;
				str.Format(IDS_DELETEDATABASEQUESTION,pFileName);
				int nRet=MessageBox(str,ID2W(IDS_DELETEDATABASE),MB_ICONQUESTION|MB_YESNO);
				if (nRet==IDYES)
					FileSystem::Remove(pFileName);
			}
		}

		if (bFree)
			delete[] pFileName;
	}
	m_pList->DeleteItem(nItem);

	EnableButtons();
	m_pList->SetFocus();
}

void CSettingsProperties::CDatabasesSettingsPage::OnEnable(BOOL bEnable)
{
	int nItem=m_pList->GetNextItem(-1,LVNI_SELECTED);
	if (nItem==-1)
		return;

	CDatabase* pDatabase=(CDatabase*)m_pList->GetItemData(nItem);
	ASSERT(pDatabase!=NULL);

	pDatabase->Enable(bEnable);
	m_pList->SetCheckState(nItem,bEnable);

	EnableDlgItem(IDC_ENABLE,!bEnable);
	EnableDlgItem(IDC_DISABLE,bEnable);

	m_pList->SetFocus();
}

BOOL CSettingsProperties::CDatabasesSettingsPage::ItemUpOrDown(BOOL bUp)
{
	int nSelected=m_pList->GetNextItem(-1,LVNI_SELECTED);
	ASSERT(nSelected!=-1);
	CDatabase* pSelected=(CDatabase*)m_pList->GetItemData(nSelected);
	
	int nOther=m_pList->GetNextItem(nSelected,bUp?LVNI_ABOVE:LVNI_BELOW);
	if (nOther==-1 || nOther==nSelected)
	{
		if (bUp && pSelected->GetThreadId()>0)
			return IncreaseThread(nSelected,pSelected,TRUE);
		else if (!bUp && pSelected->GetThreadId()<m_nThreadsCurrently-1)
			return IncreaseThread(nSelected,pSelected,FALSE);
		return FALSE;
	}

	CDatabase* pOther=(CDatabase*)m_pList->GetItemData(nOther);
	if (pOther->GetThreadId()!=pSelected->GetThreadId())
	{
		ASSERT(bUp?pSelected->GetThreadId()>0:pSelected->GetThreadId()<m_nThreadsCurrently-1);
		return IncreaseThread(nSelected,pSelected,bUp);
	}

	// This is working in this dialog! Wou
	LPARAM pParam=m_pList->GetItemData(nSelected);
	m_pList->SetItemData(nSelected,m_pList->GetItemData(nOther));
	m_pList->SetItemData(nOther,pParam);
	UINT nState=m_pList->GetItemState(nSelected,0xFFFFFFFF);
	m_pList->SetItemState(nSelected,m_pList->GetItemState(nOther,0xFFFFFFFF),0xFFFFFFFF);
	m_pList->SetItemState(nOther,nState,0xFFFFFFFF);

	m_pList->EnsureVisible(nOther,FALSE);
	m_pList->RedrawItems(min(nSelected,nOther),max(nSelected,nOther));

	m_pList->UpdateWindow();
	
	EnableButtons();
	m_pList->SetFocus();
	return TRUE;
}

BOOL CSettingsProperties::CDatabasesSettingsPage::IncreaseThread(int nItem,CDatabase* pDatabase,BOOL bDecrease)
{
	if ((bDecrease && pDatabase->GetThreadId()<1) || 
		(!bDecrease && pDatabase->GetThreadId()>=m_nThreadsCurrently-1))
		return FALSE;

	pDatabase->SetThreadId(pDatabase->GetThreadId()+(bDecrease?-1:1));

	if (GetLocateApp()->m_wComCtrlVersion>=0x0600)
	{
		LVITEM li;
		li.mask=LVIF_GROUPID;
		li.iItem=nItem;
		li.iSubItem=0;
		li.iGroupId=pDatabase->GetThreadId();
		m_pList->SetItem(&li);
	}
	else
		m_pList->RedrawItems(nItem,nItem);
	
	m_pList->EnsureVisible(nItem,FALSE);
	m_pList->SetFocus();
	EnableButtons();
	return TRUE;
}

int CALLBACK CSettingsProperties::CDatabasesSettingsPage::ThreadSortProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	if (((CDatabase*)lParam1)->GetThreadId()>((CDatabase*)lParam2)->GetThreadId())
		return 1;
	if (((CDatabase*)lParam1)->GetThreadId()<((CDatabase*)lParam2)->GetThreadId())
		return -1;
	return 0;
}

void CSettingsProperties::CDatabasesSettingsPage::OnUpdate()
{
	int nItem=m_pList->GetNextItem(-1,LVNI_SELECTED);
	if (nItem==-1)
		return;

	CDatabase* pDatabase=(CDatabase*)m_pList->GetItemData(nItem);
	pDatabase->UpdateGlobally(!pDatabase->IsGloballyUpdated());
	m_pList->RedrawItems(nItem,nItem);

	m_pList->SetFocus();
}

void CSettingsProperties::CDatabasesSettingsPage::OnRestore()
{
	// Removing current items
	m_pList->DeleteAllItems();

	// Loading databases from registry
	ASSERT(m_pSettings->m_aDatabases.GetSize()==0);
	
	CDatabase::LoadFromRegistry(HKCU,CLocateApp::GetRegKey("Databases"),m_pSettings->m_aDatabases);

	// If there is still no any available database, try to load old style db
	if (m_pSettings->m_aDatabases.GetSize()==0)
	{
		CDatabase* pDatabase=CDatabase::FromOldStyleDatabase(HKCU,"Software\\Update\\Database");
		if (pDatabase==NULL)
			pDatabase=CDatabase::FromDefaults(TRUE); 
		else
		{
			if (CDatabase::SaveToRegistry(HKCU,CLocateApp::GetRegKey("Databases"),&pDatabase,1))
				CRegKey::DeleteKey(HKCU,"Software\\Update\\Database");
		}
		m_pSettings->m_aDatabases.Add(pDatabase);
	}
	
	
	// Databases are not anymore overridden
	m_pSettings->ClearSettingsFlags(CSettingsProperties::settingsDatabasesOverridden);
	
	
	// Setting databases to list
	SetDatabasesToList();
	
	// Enabling dlg buttons
	EnableDlgItem(IDC_NEW,TRUE);
	
	ShowDlgItem(IDC_OVERRIDETXT,swHide);
	ShowDlgItem(IDC_RESTORE,swHide);
	
	ShowDlgItem(IDC_THREADS,swShow);
	ShowDlgItem(IDC_THREADSPIN,swShow);
	EnableDlgItem(IDC_THREADS,TRUE);
	EnableDlgItem(IDC_THREADSPIN,TRUE);
	
	EnableButtons();
	m_pList->SetFocus();
}

void CSettingsProperties::CDatabasesSettingsPage::OnThreads()
{
	BOOL bError;
	int nThreads=(int)SendDlgItemMessage(IDC_THREADSPIN,UDM_GETPOS32,0,LPARAM(&bError));
	if (bError)
	{
		nThreads=GetDlgItemInt(IDC_THREADS,&bError,FALSE);
		if (!bError)
			nThreads=1;
	}

	if (nThreads<1)
		SetDlgItemInt(IDC_THREADS,nThreads=1,FALSE);

	ChangeNumberOfThreads(nThreads);
	
	EnableButtons();
}

BOOL CSettingsProperties::CDatabasesSettingsPage::OnNotify(int idCtrl,LPNMHDR pnmh)
{
	switch (idCtrl)
	{
	case IDC_DATABASES:
		return ListNotifyHandler((NMLISTVIEW*)pnmh);
	}
	return CPropertyPage::OnNotify(idCtrl,pnmh);
}

void CSettingsProperties::CDatabasesSettingsPage::OnImport()
{
	// Set wait cursor
	CWaitCursor wait;
	
	// Initializing file name dialog
	CFileDialog fd(TRUE,L"*",szwEmpty,OFN_EXPLORER|OFN_HIDEREADONLY|OFN_NOREADONLYRETURN|OFN_ENABLESIZING,IDS_IMPORTDATABASEFILTERS);
	fd.EnableFeatures();
	fd.SetTitle(ID2W(IDS_IMPORTDATABASESETTINGS));
	
	
	// Ask file name
	if (!fd.DoModal(*this))
		return;

	// First, check whether file is database and read information
	CDatabase* pDatabase;
	CStringW Path;
	fd.GetFilePath(Path);
	CDatabaseInfo* pDatabaseInfo=CDatabaseInfo::GetFromFile(Path);
	if (pDatabaseInfo!=NULL)
	{
		if (!pDatabaseInfo->sExtra2.IsEmpty())
		{
			DebugFormatMessage(L"DBIMPORT: db from extra2 block: %s",(LPCWSTR)pDatabaseInfo->sExtra2);
			pDatabase=CDatabase::FromExtraBlock(pDatabaseInfo->sExtra2);
		}
		if (pDatabase==NULL && !pDatabaseInfo->sExtra1.IsEmpty())
		{
			DebugFormatMessage(L"DBIMPORT: db from extra block: %s",(LPCWSTR)pDatabaseInfo->sExtra1);
			pDatabase=CDatabase::FromExtraBlock(pDatabaseInfo->sExtra1);
		}

		if (pDatabase!=NULL)
		{
			pDatabase->SetArchiveType(CDatabase::archiveFile);
			pDatabase->SetArchiveName(Path);
		}

		delete pDatabaseInfo;
	}
	else
	{
		CFile* pFile=NULL;
		char* pFileContent=NULL;
		BOOL bError=FALSE;

		try {
			CStringW Path;
			fd.GetFilePath(Path);
			pFile=new CFile(Path,CFile::defRead|CFile::otherErrorWhenEOF,TRUE);
			pFile->CloseOnDelete();

			DWORD dwLength=pFile->GetLength();

			if (dwLength<2)
				throw 1;

			pFileContent=new char[dwLength+1];
			pFile->Read(pFileContent,dwLength);
			pFileContent[dwLength]='\0';
			pFile->Close();
		}
		catch (...)
		{
			bError=TRUE;
		}
        
		if (!bError)
		{
			if (pFileContent[1]=='\0')
			{
				// String is probable UNICODE
				pDatabase=CDatabase::FromExtraBlock((LPCWSTR)pFileContent);
			}
			else			
				pDatabase=CDatabase::FromExtraBlock(A2W(pFileContent));
		}

		if (pFile!=NULL)
			delete pFile;
		if (pFileContent!=NULL)
			delete[] pFileContent;
	}

	if (pDatabase==NULL)
	{
		CStringW msg,path;
		fd.GetFilePath(path);
		msg.Format(IDS_UNABLEREADSETTINGS,(LPCWSTR)path);
		MessageBox(msg,ID2W(IDS_ERROR),MB_OK|MB_ICONERROR);
		return;
	}

	if (pDatabase->GetThreadId()>=m_nThreadsCurrently)
	{
		if (MessageBox(ID2W(IDS_INCREASETHREADCOUNT),ID2W(IDS_IMPORTDATABASESETTINGS),MB_ICONQUESTION|MB_YESNO)==IDYES)
		{
			ChangeNumberOfThreads(pDatabase->GetThreadId()+1);
			SetDlgItemInt(IDC_THREADS,pDatabase->GetThreadId()+1,FALSE);
		}
		else
			pDatabase->SetThreadId(0);
	}

	OnNew(pDatabase);
}

void CSettingsProperties::CDatabasesSettingsPage::OnExport()
{
	int iItem=m_pList->GetNextItem(-1,LVNI_SELECTED);
	if (iItem==-1)
		return;
    
	CDatabase* pDatabase=(CDatabase*)m_pList->GetItemData(iItem);
	if (pDatabase==NULL)
		return;

	// Set wait cursor
	CWaitCursor wait;
	
	LPWSTR pArchiveFile;
	BOOL bFree;
	pArchiveFile=pDatabase->GetResolvedArchiveName(bFree);
	CAutoPtrA<WCHAR> ap(bFree?pArchiveFile:NULL);

	// Initializing file name dialog
	CFileDialog fd(FALSE,L"*",pArchiveFile,OFN_EXPLORER|OFN_HIDEREADONLY|OFN_NOREADONLYRETURN|OFN_ENABLESIZING,
		IDS_EXPORTDATABASEFILTERS);
	fd.EnableFeatures();
	fd.SetTitle(ID2W(IDS_EXPORTDATABASESETTINGS));
	
	
	// Ask file name
	if (!fd.DoModal(*this))
		return;


	CStringW Path;
	fd.GetFilePath(Path);
	
	if (FileSystem::IsFile(pArchiveFile))
	{
		if (Path.CompareNoCase(pArchiveFile)!=-1 &&
			fd.GetFilterIndex()==1)
		{
			// Destination is not same as the database file, copy content of file
			FileSystem::CopyFile(pArchiveFile,Path,FALSE);
		}
		
		if (FileSystem::IsFile(Path))
		{
			// Save extra block
			if (pDatabase->SaveExtraBlockToDbFile(Path))
				return;

			CStringW msg;
			msg.Format(IDS_FILEISNOTDATABASE,LPCWSTR(Path));
			if (MessageBox(msg,ID2W(IDS_EXPORTDATABASESETTINGS),MB_ICONQUESTION|MB_YESNO)==IDNO)
				return;			
		}
	}




	// Export extra block

	CFile* pFile=NULL;
	LPWSTR pExtra=pDatabase->ConstructExtraBlock();
	
	try {
		pFile=new CFile(Path,CFile::defWrite,TRUE);
		pFile->CloseOnDelete();
		pFile->Write(pExtra);
	}
	catch (...)
	{
		MessageBox(ID2W(IDS_CANNOTWRITESETTINGS),szwError,MB_ICONERROR|MB_OK);
	}
	if (pFile!=NULL)
		delete pFile;
	delete[] pExtra;
    
}

BOOL CSettingsProperties::CDatabasesSettingsPage::ListNotifyHandler(NMLISTVIEW *pNm)
{
	switch(pNm->hdr.code)
	{
	case LVN_ITEMCHANGING:
		if (m_pSettings->IsSettingsFlagSet(CSettingsProperties::settingsDatabasesOverridden))
			return TRUE; // No change allowed
		return FALSE;
	case LVN_ITEMCHANGED:
		if (pNm->lParam!=NULL &&
			(pNm->uNewState&LVIS_STATEIMAGEMASK)!=(pNm->uOldState&LVIS_STATEIMAGEMASK))
		{
			((CDatabase*)pNm->lParam)->Enable(((pNm->uNewState&LVIS_STATEIMAGEMASK)>>12)-1);
			EnableButtons();
		}
		break;
	case NM_CLICK:
		EnableButtons();
		break;
	case NM_DBLCLK:
		OnEdit();
		break;
	case LVN_DELETEITEM:
		if (pNm->lParam!=NULL)
			delete (CDatabase*)pNm->lParam;
		break;
	case LVN_GETDISPINFOA:
		{
			LV_DISPINFO *pLvdi=(LV_DISPINFO*)pNm;

			CDatabase* pDatabase=(CDatabase*)pLvdi->item.lParam;
			if (pDatabase==NULL)
				break;
				
			switch (pLvdi->item.iSubItem)
			{
			case 0:
				if (!m_pSettings->IsSettingsFlagSet(CSettingsProperties::settingsDatabasesOverridden))
				{
					pLvdi->item.pszText=alloccopyWtoA(pDatabase->GetName());
					AssignBuffer(pLvdi->item.pszText);
				}
				else
				{
					pLvdi->item.pszText=GetBuffer(40);
					LoadString(IDS_COMMANDLINEARGUMENT,pLvdi->item.pszText,40);
				}
				break;
			case 1:
				pLvdi->item.pszText=alloccopyWtoA(pDatabase->GetArchiveName());
				AssignBuffer(pLvdi->item.pszText);
				break;
			case 2:
				pLvdi->item.pszText=GetBuffer(100);
				LoadString(pDatabase->IsGloballyUpdated()?IDS_YES:IDS_NO,pLvdi->item.pszText,100);
				break;
			case 3:
				if (ReadModificationDate(pLvdi->item.iItem,pDatabase))
				{
					pLvdi->item.pszText=GetBuffer(100);
					LoadString(IDS_UNKNOWN,pLvdi->item.pszText,100);
				}
				else
					pLvdi->item.pszText=const_cast<LPSTR>(szEmpty);
				break;
			case 4:
				pLvdi->item.pszText=GetBuffer(20);
				_itoa_s(pDatabase->GetThreadId()+1,pLvdi->item.pszText,20,10);
				break;
			}
			break;
		}
	case LVN_GETDISPINFOW:
		{
			LV_DISPINFOW *pLvdi=(LV_DISPINFOW*)pNm;

			CDatabase* pDatabase=(CDatabase*)pLvdi->item.lParam;
			if (pDatabase==NULL)
				break;
				
			switch (pLvdi->item.iSubItem)
			{
			case 0:
				if (!m_pSettings->IsSettingsFlagSet(CSettingsProperties::settingsDatabasesOverridden))
					pLvdi->item.pszText=const_cast<LPWSTR>(pDatabase->GetName());
				else
				{
					pLvdi->item.pszText=GetBufferW(40);
					LoadString(IDS_COMMANDLINEARGUMENT,pLvdi->item.pszText,40);
				}
				break;
			case 1:
				pLvdi->item.pszText=const_cast<LPWSTR>(pDatabase->GetArchiveName());
				break;
			case 2:
				pLvdi->item.pszText=GetBufferW(100);
				LoadString(pDatabase->IsGloballyUpdated()?IDS_YES:IDS_NO,pLvdi->item.pszText,100);
				break;
			case 3:
				if (ReadModificationDate(pLvdi->item.iItem,pDatabase))
				{
					pLvdi->item.pszText=GetBufferW(100);
					LoadString(IDS_UNKNOWN,pLvdi->item.pszText,100);
				}
				else
					pLvdi->item.pszText=const_cast<LPWSTR>(szwEmpty);
							
				break;
			case 4: // Thread 
				pLvdi->item.pszText=GetBufferW(20);
				_itow_s(pDatabase->GetThreadId()+1,pLvdi->item.pszText,20,10);
				break;
			}
			break;
		}
	}
	return 0;
}
	
BOOL CSettingsProperties::CDatabasesSettingsPage::ReadModificationDate(int nItem,CDatabase* pDatabase)
{
	// Check if there is already a thread reading date
	EnterCriticalSection(&m_csModDateThreads);
	for (int i=0;i<m_ModDateThreads.GetSize();i++)
	{
		if (m_ModDateThreads[i]->nItem==nItem)
			return TRUE;
	}
	LeaveCriticalSection(&m_csModDateThreads);



	// Initiate thread for reading date
	ModificationDateData* pData=new ModificationDateData;
	pData->hListWnd=*m_pList;
	pData->nItem=nItem;
	pData->sDatabaseFile=pDatabase->GetArchiveName();
	pData->pPage=this;
	DWORD dwThreadID;
	pData->hThread=CreateThread(NULL,0,ReadModificationDateProc,(LPVOID)pData,0,&dwThreadID);
	if (pData->hThread!=NULL)
	{
		EnterCriticalSection(&m_csModDateThreads);
		m_ModDateThreads.Add(pData);
		LeaveCriticalSection(&m_csModDateThreads);
		
		return TRUE;
	}



	delete pData;
	return FALSE;
}

DWORD WINAPI CSettingsProperties::CDatabasesSettingsPage::ReadModificationDateProc(LPVOID lpParameter)
{
	ModificationDateData* pData=(ModificationDateData*)lpParameter;
	
	SdDebugMessage2("SDLG: ReadModificationData for %S BEGIN nItem=%d",pData->sDatabaseFile,pData->nItem);


	ASSERT(pData!=NULL);

	CDatabaseInfo::GetFromFile(pData->sDatabaseFile,pData->di);
	if (pData->di!=NULL)
	{
		if (pData->di->tCreationTime.m_time>0)
		{
			FILETIME ft;
			SYSTEMTIME st=pData->di->tCreationTime;
			WORD wDate,wTime;
			SystemTimeToFileTime(&st,&ft);
			FileTimeToDosDateTime(&ft,&wDate,&wTime);
			
			pData->pText=GetLocateApp()->FormatDateAndTimeString(wDate,wTime);
		}
		delete pData->di;
		pData->di=NULL;
	}
	
	
	if (pData->pText==NULL)
	{
		pData->pText=new WCHAR[100];
		LoadString(IDS_UNKNOWN,pData->pText,100);
	}

	
	LVITEMW li;
	li.mask=LVIF_TEXT;
	li.pszText=pData->pText;
	li.iItem=pData->nItem;
	li.iSubItem=3;
	::SendMessage(pData->hListWnd,LVM_SETITEMTEXTW,li.iItem,(LPARAM)&li);

	
	CDatabasesSettingsPage* pPage=pData->pPage;

	EnterCriticalSection(&pPage->m_csModDateThreads);
	int nIndex=pPage->m_ModDateThreads.Find(pData);
	if (nIndex!=-1)
		pPage->m_ModDateThreads.RemoveAt(nIndex);
	LeaveCriticalSection(&pData->pPage->m_csModDateThreads);
	
	SdDebugMessage1("SDLG: ReadModificationData for %S END",pData->sDatabaseFile);

	delete pData;
		
	return 0;
}



BOOL CSettingsProperties::CDatabasesSettingsPage::OnApply()
{
	CPropertyPage::OnApply();

	ASSERT(m_pSettings->m_aDatabases.GetSize()==0);

	// Get the first item
	//int nNext;
	int nItem=m_pList->GetNextItem(-1,LVNI_ALL);

	/*while ((nNext=m_pList->GetNextItem(nItem,LVNI_ABOVE))!=-1)
	{
		if (nNext==nItem)
			break; // This should not be like that, why is it?
		nItem=nNext;
	}*/
	
	while (nItem!=-1)
	{
		PDATABASE pDatabase=(PDATABASE)m_pList->GetItemData(nItem);
		ASSERT(pDatabase!=NULL);

		m_pSettings->m_aDatabases.Add(pDatabase);
		m_pList->SetItemData(nItem,NULL);

		/*nNext=m_pList->GetNextItem(nItem,LVNI_BELOW);
		if (nNext==nItem)
			break;
		nItem=nNext;*/

		nItem=m_pList->GetNextItem(nItem,LVNI_ALL);
	}
	
	return TRUE;
}

void CSettingsProperties::CDatabasesSettingsPage::OnCancel()
{
	m_pSettings->SetSettingsFlags(CSettingsProperties::settingsCancelled);


	CPropertyPage::OnCancel();
}

void CSettingsProperties::CDatabasesSettingsPage::EnableThreadGroups(int nThreadGroups)
{
	if (m_pList->IsGroupViewEnabled())
		return;

	m_pList->EnableGroupView(TRUE);
	m_nThreadsCurrently=nThreadGroups;
	
	// Creating groups
	CStringW str;
	LVGROUP lg;
	dMemSet(&lg,0,sizeof(LVGROUP));
	lg.cbSize=sizeof(LVGROUP);
	lg.mask=LVGF_HEADER|LVGF_GROUPID|LVGF_ALIGN;
	lg.state=LVGS_NORMAL;
	lg.uAlign=LVGA_HEADER_LEFT;

	for (lg.iGroupId=0;lg.iGroupId<nThreadGroups;lg.iGroupId++)
	{
		str.Format(IDS_THREADNAME,lg.iGroupId+1);
		lg.pszHeader=str.GetBuffer();

		m_pList->InsertGroup(lg.iGroupId,&lg);
	}

	// Setting groups IDs
	if (m_pList->GetItemCount()>0)
	{
		LVITEM li;
		li.mask=LVIF_GROUPID;
		li.iItem=m_pList->GetNextItem(-1,LVNI_ALL);
		li.iSubItem=0;
		while (li.iItem!=-1)
		{
			CDatabase* pDatabase=(CDatabase*)m_pList->GetItemData(li.iItem);
			ASSERT(pDatabase!=NULL);

			li.iGroupId=pDatabase->GetThreadId();
			
			m_pList->SetItem(&li);
			li.iItem=m_pList->GetNextItem(li.iItem,LVNI_ALL);
		}
	}
}

void CSettingsProperties::CDatabasesSettingsPage::RemoveThreadGroups()
{
	if (!m_pList->IsGroupViewEnabled())
		return;

	m_pList->RemoveAllGroups();
	m_pList->EnableGroupView(FALSE);
}

void CSettingsProperties::CDatabasesSettingsPage::ChangeNumberOfThreads(int nThreads)
{
	ASSERT(nThreads>=1);
	
	if (nThreads>m_nThreadsCurrently)
	{
		// Number is increased
		if (((CLocateApp*)GetApp())->m_wComCtrlVersion<0x0600)
		{
			m_nThreadsCurrently=nThreads;
			return;
		}

		if (m_nThreadsCurrently==1)
			EnableThreadGroups(nThreads);
		else
		{
			// Insertig new thread groups
			CStringW str;
			LVGROUP lg;
			dMemSet(&lg,0,sizeof(LVGROUP));
			lg.cbSize=sizeof(LVGROUP);
			lg.mask=LVGF_HEADER|LVGF_GROUPID|LVGF_ALIGN;
			lg.state=LVGS_NORMAL;
			lg.uAlign=LVGA_HEADER_LEFT;

			for (lg.iGroupId=m_nThreadsCurrently;lg.iGroupId<nThreads;lg.iGroupId++)
			{
				str.Format(IDS_THREADNAME,lg.iGroupId+1);
				lg.pszHeader=str.GetBuffer();

				m_pList->InsertGroup(lg.iGroupId,&lg);
			}
		
		}
		m_nThreadsCurrently=nThreads;
		m_pList->RedrawItems(0,m_pList->GetItemCount());
	}
	else if (nThreads<m_nThreadsCurrently)
	{
		// Ensuring that there is no any items with higher thread ID than available
		if (m_pList->GetItemCount()>0)
		{
			int nItem=m_pList->GetNextItem(-1,LVNI_ALL);
			while (nItem!=-1)
			{
				CDatabase* pDatabase=(CDatabase*)m_pList->GetItemData(nItem);
				ASSERT(pDatabase!=NULL);

				if (pDatabase->GetThreadId()>=nThreads)
				{
					pDatabase->SetThreadId(nThreads-1);
					
					LVITEM li;
					li.iItem=nItem;
					li.iSubItem=0;
					li.mask=LVIF_GROUPID;
					li.iGroupId=nThreads-1;
					m_pList->SetItem(&li);
				}
				nItem=m_pList->GetNextItem(nItem,LVNI_ALL);
			}
		}

		if (((CLocateApp*)GetApp())->m_wComCtrlVersion>=0x0600)
		{
			if (nThreads==1)
				RemoveThreadGroups();
			else
			{
				// Removing unused thread groups
				while (m_nThreadsCurrently>nThreads)
					m_pList->RemoveGroup(--m_nThreadsCurrently);
			}
		}
		m_nThreadsCurrently=nThreads;

		m_pList->RedrawItems(0,m_pList->GetItemCount());
	}
}

void CSettingsProperties::CDatabasesSettingsPage::EnableButtons()
{
	int nSelectedItem=m_pList->GetNextItem(-1,LVNI_SELECTED);
	
	BOOL bEnable=nSelectedItem!=-1 && !m_pSettings->IsSettingsFlagSet(CSettingsProperties::settingsDatabasesOverridden);
	
	EnableDlgItem(IDC_EDIT,nSelectedItem!=-1);
	EnableDlgItem(IDC_REMOVE,bEnable);
	EnableDlgItem(IDC_UPDATE,bEnable);
	
	
	EnableDlgItem(IDC_EXPORT,bEnable);
	EnableDlgItem(IDC_IMPORT,!m_pSettings->IsSettingsFlagSet(CSettingsProperties::settingsDatabasesOverridden));
	
	if (bEnable)
	{
		CDatabase* pDatabase=(CDatabase*)m_pList->GetItemData(nSelectedItem);
		ASSERT(pDatabase!=NULL);

		EnableDlgItem(IDC_ENABLE,!pDatabase->IsEnabled());
		EnableDlgItem(IDC_DISABLE,pDatabase->IsEnabled());

		// Checking item above
		int nAnother=m_pList->GetNextItem(nSelectedItem,LVNI_ABOVE);
		if (nAnother==-1 || nAnother==nSelectedItem)
			EnableDlgItem(IDC_UP,pDatabase->GetThreadId()>0);
		else
			EnableDlgItem(IDC_UP,TRUE);
		
		// Checking item below
		nAnother=m_pList->GetNextItem(nSelectedItem,LVNI_BELOW);
		if (nAnother==-1 || nAnother==nSelectedItem)
			EnableDlgItem(IDC_DOWN,pDatabase->GetThreadId()<m_nThreadsCurrently-1); 
		else
			EnableDlgItem(IDC_DOWN,TRUE);
	}
	else
	{
		EnableDlgItem(IDC_ENABLE,FALSE);
		EnableDlgItem(IDC_DISABLE,FALSE);
		EnableDlgItem(IDC_UP,FALSE);
		EnableDlgItem(IDC_DOWN,FALSE);
	}
}
	
/////////////////////////////////////////////////
// CDatabasesSettingsPage::CDatabaseDialog

BOOL CSettingsProperties::CDatabasesSettingsPage::CDatabaseDialog::OnInitDialog(HWND hwndFocus)
{
	CDialog::OnInitDialog(hwndFocus);
	
	// Initializing list control
	m_pList=new CListCtrl(GetDlgItem(IDC_FOLDERS));
	CLocateDlg::SetSystemImageLists(m_pList);
	m_pList->SetExtendedListViewStyle(LVS_EX_CHECKBOXES|LVS_EX_HEADERDRAGDROP,
		LVS_EX_CHECKBOXES|LVS_EX_HEADERDRAGDROP);
	if (IsUnicodeSystem())
		m_pList->SetUnicodeFormat(TRUE);
	m_pList->InsertColumn(0,ID2W(IDS_VOLUMELABEL),LVCFMT_LEFT,95,0);
	m_pList->InsertColumn(1,ID2W(IDS_VOLUMEPATH),LVCFMT_LEFT,75,1);
	m_pList->InsertColumn(2,ID2W(IDS_VOLUMETYPE),LVCFMT_LEFT,70,2);
	m_pList->InsertColumn(3,ID2W(IDS_VOLUMEFILESYSTEM),LVCFMT_LEFT,65,3);
	m_pList->LoadColumnsState(HKCU,CRegKey2::GetCommonKey()+"\\Dialogs","Database Dialog List Widths");
	
	for (int i=1;i<=m_nMaximumNumbersOfThreads;i++)
	{
		char num[5];
		_itoa_s(i,num,5,10);
		SendDlgItemMessage(IDC_USEDTHREAD,CB_ADDSTRING,0,LPARAM(num));
	}

    // Setting current information	
	if (m_bDontEditName)
	{
		EnableDlgItem(IDC_NAME,FALSE);
		SetDlgItemText(IDC_NAME,ID2W(IDS_COMMANDLINEARGUMENT));
	}
	else
		SetDlgItemText(IDC_NAME,m_pDatabase->GetName());
	SetDlgItemText(IDC_DBFILE,m_pDatabase->GetArchiveName());
	SetDlgItemText(IDC_CREATOR,m_pDatabase->GetCreator());
	SetDlgItemText(IDC_DESCRIPTION,m_pDatabase->GetDescription());
	SendDlgItemMessage(IDC_USEDTHREAD,CB_SETCURSEL,m_pDatabase->GetThreadId());

	CheckDlgButton(IDC_ENABLE,m_pDatabase->IsEnabled());
	CheckDlgButton(IDC_GLOBALUPDATE,m_pDatabase->IsGloballyUpdated());
	CheckDlgButton(IDC_STOPIFROOTUNAVAILABLE,m_pDatabase->IsFlagSet(CDatabase::flagStopIfRootUnavailable));
	CheckDlgButton(IDS_SCANSYMLINKSANDJUNCTIONS,m_pDatabase->IsFlagSet(CDatabase::flagScanSymLinksAndJunctions));
	CheckDlgButton(IDC_INCREMENTALUPDATE,m_pDatabase->IsFlagSet(CDatabase::flagIncrementalUpdate));
	
	if (IsUnicodeSystem())
		CheckDlgButton(IDC_UNICODE,!m_pDatabase->IsFlagSet(CDatabase::flagAnsiCharset));
	else
		EnableDlgItem(IDC_UNICODE,FALSE);

	// Inserting local drives to drive list
	DWORD nLength=GetLogicalDriveStrings(0,NULL)+1;
	if (nLength<2)
		return FALSE;

	WCHAR* szDrives=new WCHAR[nLength+1];
	FileSystem::GetLogicalDriveStrings(nLength,szDrives);
	for (LPWSTR szDrive=szDrives;szDrive[0]!=L'\0';szDrive+=4)
		AddDriveToList(szDrive);
	delete[] szDrives;

	
	
	// Setting local drives
	if (m_pDatabase->GetRoots()!=NULL)
	{
		SdDebugMessage("DBDIALOG: CUSTOM DRIVES");

		CheckDlgButton(IDC_CUSTOMDRIVES,1);
		
		EnableControls();

		LPCWSTR pPtr=m_pDatabase->GetRoots();

		while (*pPtr!='\0')
		{
			DWORD dwLength=istrlenw(pPtr);

			if (dwLength>2)
			{
				if (pPtr[0]=='\\' && pPtr[1]=='\\')
				{
					if (!FileSystem::IsDirectory(pPtr))
					{
						SdDebugMessage1(L"DBDIALOG: adding computer %s",pPtr);
						AddComputerToList(pPtr);
					}
					else
					{
						SdDebugMessage1(L"DBDIALOG: adding UNC directory %s",pPtr);
						AddDirectoryToList(pPtr,dwLength);
					}
				}			
				else
				{
					SdDebugMessage1(L"DBDIALOG: adding computer %s",pPtr);
					AddDirectoryToList(pPtr,dwLength);
				}
			}
			else if (pPtr[1]==':')
			{
				// Checking whether we point unavailable drive
				WCHAR root[]=L"X:\\";
				root[0]=pPtr[0];
				UINT uRet=FileSystem::GetDriveType(root);
				if (uRet==DRIVE_UNKNOWN || uRet==DRIVE_NO_ROOT_DIR)
				{
					SdDebugMessage1(L"DBDIALOG: adding drive %s",pPtr);
					AddDriveToList(root); // Unavailable
				}
				else
				{
					SdDebugMessage1(L"DBDIALOG: drive %s is not valid, ignored",pPtr);
				}
			}
			else
			{
				SdDebugMessage1(L"DBDIALOG: root %s too short and therefore ignored",pPtr);
			}

			pPtr+=dwLength+1;
		}
	}
	else
	{
		SdDebugMessage("DBDIALOG: LOCAL DRIVES");

		CheckDlgButton(IDC_LOCALDRIVES,1);
		EnableDlgItem(IDC_FOLDERS,FALSE);
		EnableDlgItem(IDC_ADDFOLDER,FALSE);
		EnableDlgItem(IDC_REMOVEFOLDER,FALSE);
	}
	return FALSE;
}

void CSettingsProperties::CDatabasesSettingsPage::CDatabaseDialog::OnHelp(LPHELPINFO lphi)
{
	if (lphi!=NULL)
	{
		CLocateApp::HelpID id[]= {
			{ IDC_ENABLE,"sdd_enable" },
			{ IDC_GLOBALUPDATE,"sdd_updglob" },
			{ IDC_USEDTHREADLABEL,"sdd_thread" },
			{ IDC_USEDTHREAD,"sdd_thread" },
			{ IDC_NAMELABEL,"sdd_name" },
			{ IDC_NAME,"sdd_name" },
			{ IDC_DBFILELABEL,"sdd_file" },
			{ IDC_DBFILE,"sdd_file" },
			{ IDC_BROWSE,"sdd_file" },
			{ IDC_UNICODE,"sdd_unicode" },
			{ IDC_INCREMENTALUPDATE,"sdd_noowerwrite" },
			{ IDC_CREATORLABEL,"sdd_creator" },
			{ IDC_CREATOR,"sdd_creator" },
			{ IDC_DESCRIPTIONLABEL,"sdd_description" },
			{ IDC_DESCRIPTION,"sdd_description" },
			{ IDC_LOCALDRIVES,"sdd_local" },
			{ IDC_CUSTOMDRIVES,"sdd_custom" },
			{ IDS_SCANSYMLINKSANDJUNCTIONS,"sdd_scanjunctions" },
			{ IDC_STOPIFROOTUNAVAILABLE,"sdd_stopifunavailable" },
			{ IDC_ADVANCED,"sdd_advanced" },
			{ IDC_FOLDERSLABEL,"sdd_drives" },
			{ IDC_FOLDERS,"sdd_drives" },
			{ IDC_ADDFOLDER,"sdd_adddir" },
			{ IDC_REMOVEFOLDER,"sdd_removedir" },
			{ IDC_UP,"sdd_updown" },
			{ IDC_DOWN,"sdd_updown" }
		};
		
		if (CLocateApp::OpenHelp(*this,"settings_databasedlg.htm",lphi,id,sizeof(id)/sizeof(CLocateApp::HelpID)))
			return;
	}

	if (HtmlHelp(HH_HELP_CONTEXT,HELP_SETTINGS_DATABASEDLG)==NULL)
		HtmlHelp(HH_DISPLAY_TOPIC,0);
}

void CSettingsProperties::CDatabasesSettingsPage::CDatabaseDialog::OnDestroy()
{
	if (m_pList!=NULL)
	{
		m_pList->SaveColumnsState(HKCU,CRegKey2::GetCommonKey()+"\\Dialogs","Database Dialog List Widths");
		delete m_pList;
		m_pList=NULL;
	}
	CDialog::OnDestroy();
}

BOOL CSettingsProperties::CDatabasesSettingsPage::CDatabaseDialog::OnClose()
{
	CDialog::OnClose();
	EndDialog(0);
	return 0;
}

BOOL CSettingsProperties::CDatabasesSettingsPage::CDatabaseDialog::OnNotify(int idCtrl,LPNMHDR pnmh)
{
	switch (idCtrl)
	{
	case IDC_FOLDERS:
		switch (((NMLISTVIEW*)pnmh)->hdr.code)
		{
		case LVN_ITEMCHANGED:
			if (((((NMLISTVIEW*)pnmh)->uNewState&LVIS_SELECTED)!=
				(((NMLISTVIEW*)pnmh)->uOldState&LVIS_SELECTED)))
				EnableControls();
			break;
		}
		break;
	}
	return CDialog::OnNotify(idCtrl,pnmh);
}


				



void CSettingsProperties::CDatabasesSettingsPage::CDatabaseDialog::OnOK()
{
	// Setting name
	int iLength=GetDlgItemTextLength(IDC_NAME)+1;
	WCHAR* pText=new WCHAR[iLength];
	GetDlgItemText(IDC_NAME,pText,iLength);
	
	if (!m_bDontEditName)
	{
		if (wcsncmp(pText,L"DEFAULTX",8)==0 || wcsncmp(pText,L"PARAMX",6)==0)
		{
			CStringW msg;
			msg.Format(IDS_INVALIDDBNAME,pText);
			MessageBox(msg,ID2W(IDS_DATABASESETTINGS),MB_OK|MB_ICONINFORMATION);
			SetFocus(IDC_NAME);
			delete[] pText;
			return;
		}
		if (!CDatabase::IsNameValid(pText))
		{
			CStringW msg;
			msg.Format(IDS_INVALIDDBNAME,pText);
			MessageBox(msg,ID2W(IDS_DATABASESETTINGS),MB_OK|MB_ICONINFORMATION);
			SetFocus(IDC_NAME);
			delete[] pText;
			return;
		}
		if (CDatabase::FindByName(m_aOtherDatabases,pText)!=NULL)
		{
			CStringW msg;
			msg.Format(IDS_NAMEALREADYEXISTS,pText);
			MessageBox(msg,ID2W(IDS_DATABASESETTINGS),MB_OK|MB_ICONINFORMATION);
			SetFocus(IDC_NAME);
			delete[] pText;
			return;
		}
		m_pDatabase->SetNamePtr(pText);
	}
	

    // Setting db file
	m_pDatabase->SetArchiveType(CDatabase::archiveFile);
	iLength=GetDlgItemTextLength(IDC_DBFILE)+1;
	if (iLength==2) // Specified text is too short
	{
		CStringW msg;
		msg.Format(IDS_INVALIDFILENAME,"");
		MessageBox(msg,ID2W(IDS_DATABASESETTINGS),MB_OK|MB_ICONINFORMATION);
		SetFocus(IDC_DBFILE);
		return;
	}
	pText=new WCHAR[iLength];
	GetDlgItemText(IDC_DBFILE,pText,iLength);
	m_pDatabase->SetArchiveNamePtr(CDatabase::GetCorrertFileName(pText));
	
	if (m_pDatabase->GetArchiveName(TRUE)==NULL)
	{
		// Path was not ok, is this intended
		CStringW msg;
		msg.Format(IDS_INVALIDFILENAMEISOK,pText);
		if (MessageBox(msg,ID2W(IDS_DATABASESETTINGS),MB_YESNO|MB_ICONINFORMATION|MB_DEFBUTTON2)==IDNO)
		{
			SetFocus(IDC_DBFILE);
			delete[] pText;
			return;
		}

		m_pDatabase->SetArchiveNamePtr(pText);
	}
	else
		delete[] pText;

	if (CDatabase::FindByFile(m_aOtherDatabases,m_pDatabase->GetArchiveName())!=NULL)
	{
		CStringW msg;
		msg.Format(IDS_FILEALREADYEXISTS,m_pDatabase->GetArchiveName());
		MessageBox(msg,ID2W(IDS_DATABASESETTINGS),MB_OK|MB_ICONINFORMATION);
		SetFocus(IDC_DBFILE);
		return;
	}
	
    // Setting creator
	iLength=GetDlgItemTextLength(IDC_CREATOR)+1;
	pText=new WCHAR[iLength];
	GetDlgItemText(IDC_CREATOR,pText,iLength);
	m_pDatabase->SetCreatorPtr(pText);

	// Setting description
	iLength=GetDlgItemTextLength(IDC_DESCRIPTION)+1;
	pText=new WCHAR[iLength];
	GetDlgItemText(IDC_DESCRIPTION,pText,iLength);
	m_pDatabase->SetDescriptionPtr(pText);
	

	// Settings flags
	m_pDatabase->Enable(IsDlgButtonChecked(IDC_ENABLE));
	m_pDatabase->UpdateGlobally(IsDlgButtonChecked(IDC_GLOBALUPDATE));
	m_pDatabase->SetFlag(CDatabase::flagStopIfRootUnavailable,IsDlgButtonChecked(IDC_STOPIFROOTUNAVAILABLE));
	m_pDatabase->SetFlag(CDatabase::flagScanSymLinksAndJunctions,IsDlgButtonChecked(IDS_SCANSYMLINKSANDJUNCTIONS));
	m_pDatabase->SetFlag(CDatabase::flagIncrementalUpdate,IsDlgButtonChecked(IDC_INCREMENTALUPDATE));
	m_pDatabase->SetFlag(CDatabase::flagAnsiCharset,!IsDlgButtonChecked(IDC_UNICODE));

	// Setting thread ID
	m_pDatabase->SetThreadId((WORD)SendDlgItemMessage(IDC_USEDTHREAD,CB_GETCURSEL));
    
	if (IsDlgButtonChecked(IDC_CUSTOMDRIVES))
	{
		CArrayFAP<LPWSTR> aRoots;
		
		LVITEMW li;
		li.iItem=m_pList->GetNextItem(-1,LVNI_ALL);
		while (li.iItem!=-1)
		{
			// Not selected
			if (!m_pList->GetCheckState(li.iItem))
			{
				li.iItem=m_pList->GetNextItem(li.iItem,LVNI_ALL);
				continue;
			}
			
			BOOL bDoNotAdd=FALSE;
			WCHAR* szPath=new WCHAR[MAX_PATH];
			li.mask=LVIF_TEXT;
			li.iSubItem=1;
			li.pszText=szPath;
			li.cchTextMax=MAX_PATH;
			m_pList->GetItem(&li);
			DWORD nPathLen=istrlenw(szPath);
			for (int i=0;i<aRoots.GetSize();i++)
			{
				LPWSTR pAddedPath=aRoots.GetAt(i);
				if (FileSystem::IsSubDirectory(szPath,pAddedPath))
				{
					CStringW str;
					str.FormatEx(IDS_SUBFOLDER,szPath,pAddedPath,szPath);
					MessageBox(str,ID2W(IDS_DATABASESETTINGS),MB_ICONINFORMATION|MB_OK);
					bDoNotAdd=TRUE;
					break;
				}
				else if (FileSystem::IsSubDirectory(pAddedPath,szPath))
				{
					CStringW str;
					str.FormatEx(IDS_SUBFOLDER,pAddedPath,szPath,pAddedPath);
					MessageBox(str,ID2W(IDS_DATABASESETTINGS),MB_ICONINFORMATION|MB_OK);
					aRoots.RemoveAt(i--);
					continue;
				}
			}
			if (!bDoNotAdd)
				aRoots.Add(szPath);
			else
				delete[] szPath;
			li.iItem=m_pList->GetNextItem(li.iItem,LVNI_ALL);
		}

		if (aRoots.GetSize()==0)
		{
			ShowErrorMessage(IDS_ERRORNODRIVES,IDS_DATABASESETTINGS);
			return;
		}
		m_pDatabase->SetRoots(aRoots);
	}
	else
		m_pDatabase->SetRootsPtr(NULL);

	EndDialog(1);
}

void CSettingsProperties::CDatabasesSettingsPage::CDatabaseDialog::OnAddFolder()
{
	CWaitCursor wait;
	
	// Ask folder
	CFolderDialog fd(IDS_ADDFOLDER,BIF_USENEWUI);
	if (fd.DoModal(*this))
	{
		// Insert folder to list
		CStringW Folder;
		if (fd.GetFolder(Folder))
		{
			WCHAR szNetHood[MAX_PATH];
			switch (Network::GetNethoodTarget(Folder,szNetHood,MAX_PATH))
			{
			case 1:
				AddComputerToList(szNetHood);
				break;
			case 2:
				AddDirectoryToListWithVerify(szNetHood);
				break;
			case 0:
				AddDirectoryToListWithVerify(Folder);
				break;
			}
		}
		else
		{
			// Checking type of folder
			IShellFolder *psf=NULL;
			
			try {
				HRESULT hRes=SHGetDesktopFolder(&psf);
	            if (!SUCCEEDED(hRes))
					throw COleException(hRes);
				
					
				SHDESCRIPTIONID di;
				hRes=SHGetDataFromIDList(psf,fd.m_lpil,SHGDFIL_DESCRIPTIONID,&di,sizeof(SHDESCRIPTIONID));
				if (!SUCCEEDED(hRes))
					throw COleException(hRes);

				
				if (di.clsid!=CLSID_NetworkPlaces && di.clsid!=CLSID_NetworkExplorerFolder)
					throw FALSE;
				

				STRRET str;
				LPWSTR pComputer;
				hRes=psf->GetDisplayNameOf(fd.m_lpil,SHGDN_FORPARSING,&str);
				if (FAILED(hRes))
					throw COleException(hRes);
				
				pComputer=ShellFunctions::StrRetToPtrW(str,fd.m_lpil);
				if (pComputer==NULL)
					throw COleException(hRes);
					
			
				if (pComputer[0]!='\\' && pComputer[1]!='\\')
				{
					if (pComputer[0]==':' && pComputer[1]==':')
						ShowErrorMessage(IDS_ERRORCANNOTADDITEM,IDS_ERROR,MB_ICONERROR|MB_OK);
					else
					{
						CStringW s;
						s.Format(IDS_ERRORCANNOTADDITEM2,pComputer);
						MessageBox(s,ID2W(IDS_ERROR),MB_ICONERROR|MB_OK);
					}
				}
				else
					AddComputerToList(pComputer);
				
				delete[] pComputer;
		
			}
	#ifdef _DEBUG_LOGGING
			catch (COleException exp)
			{
				char error[1000];
				exp.GetErrorMessage(error,1000);
				SdDebugMessage1("CDatabaseDialog::OnAddFolder() throwed OLE exception: %s",error);
				ShowErrorMessage(IDS_ERRORCANNOTADDITEM,IDS_ERROR,MB_ICONERROR|MB_OK);
			}
	#endif
			catch (...)
			{
				SdDebugMessage("CDatabaseDialog::OnAddFolder() throwed unknown exception");

				ShowErrorMessage(IDS_ERRORCANNOTADDITEM,IDS_ERROR,MB_ICONERROR|MB_OK);
			}

			if (psf!=NULL)
				psf->Release();

			
			
		}

		// Setting focus to list
		m_pList->SetFocus();
	}

	// Enable "Remove folder" button is needed
	EnableControls();
}

void CSettingsProperties::CDatabasesSettingsPage::CDatabaseDialog::OnRemoveFolder()
{
	// Removes current item from list
	int nItem=m_pList->GetNextItem(-1,LVNI_SELECTED);
	if (nItem!=-1)
		m_pList->DeleteItem(nItem);
	m_pList->SetFocus();


	// Enable "Remove folder" button is needed
	EnableControls();
}

BOOL CSettingsProperties::CDatabasesSettingsPage::CDatabaseDialog::OnCommand(WORD wID,WORD wNotifyCode,HWND hControl)
{
	CDialog::OnCommand(wID,wNotifyCode,hControl);
	switch (wID)
	{
	case IDOK:
	case IDC_OK:
		OnOK();
		break;
	case IDCANCEL:
	case IDC_CANCEL:
		EndDialog(0);
		break;
	case IDC_HELPBUTTON:
		OnHelp(NULL);
		break;
	case IDC_LOCALDRIVES:
		CheckDlgButton(IDC_CUSTOMDRIVES,0);
		EnableControls();
		break;
	case IDC_CUSTOMDRIVES:
		CheckDlgButton(IDC_LOCALDRIVES,0);
		EnableControls();
		break;
	case IDC_BROWSE:
		OnBrowse();
		break;
	case IDC_ADDFOLDER:
		OnAddFolder();
		break;
	case IDC_REMOVEFOLDER:
		OnRemoveFolder();
		break;
	case IDC_ADVANCED:
		OnAdvanced();
		break;
	case IDC_UP:
	case IDC_DOWN:
		ItemUpOrDown(wID==IDC_UP);
		break;
	case IDC_NAME:
	case IDC_DBFILE:
	case IDC_CREATOR:
	case IDC_DESCRIPTION:
		if (wNotifyCode==EN_SETFOCUS)
			SendDlgItemMessage(wID,EM_SETSEL,0,-1);
		break;
	}
	return FALSE;
}


void CSettingsProperties::CDatabasesSettingsPage::CDatabaseDialog::OnBrowse()
{
	// Set wait cursor
	CWaitCursor wait;

	// Initializing file name dialog
	CStringW File;
	GetDlgItemText(IDC_DBFILE,File);

	CFileDialog fd(FALSE,L"*",File,OFN_EXPLORER|OFN_HIDEREADONLY|OFN_NOREADONLYRETURN|OFN_ENABLESIZING,IDS_DATABASEFILTERS);
	fd.EnableFeatures();
	fd.SetTitle(ID2W(IDS_SELECTDATABASE));
	
	// Ask file name
	if (!fd.DoModal(*this))
		return;
	
	// Check filename and set
	fd.GetFilePath(File);
	int i=File.Find('*');
	if (i==-1)
		SetDlgItemText(IDC_DBFILE,File);
	else
	{
		if (File[i-1]=='.')
			i--;
		File.FreeExtra(i);
		SetDlgItemText(IDC_DBFILE,File);
	}

	// Set focus to file name edit box
	SetFocus(IDC_DBFILE);
	SendDlgItemMessage(IDC_DBFILE,EM_SETSEL,0,-1);
}

BOOL CSettingsProperties::CDatabasesSettingsPage::CDatabaseDialog::ItemUpOrDown(BOOL bUp)
{
	int nSelected=m_pList->GetNextItem(-1,LVNI_SELECTED);
	ASSERT(nSelected!=-1);
	
	int nOther=m_pList->GetNextItem(nSelected,bUp?LVNI_ABOVE:LVNI_BELOW);
	if (nOther==-1 || nOther==nSelected)
		return FALSE;

	LV_ITEMW li1,li2;
	WCHAR szBuffer1[MAX_PATH+10];
	WCHAR szBuffer2[MAX_PATH+10];
	li1.pszText=szBuffer1;
	li1.cchTextMax=MAX_PATH+10;
	li2.pszText=szBuffer2;
	li2.cchTextMax=MAX_PATH+10;
	li1.iItem=nSelected;
	li2.iItem=nOther;

	li1.mask=LVIF_TEXT|LVIF_IMAGE|LVIF_STATE;
	li2.mask=LVIF_TEXT|LVIF_IMAGE|LVIF_STATE;
	li1.stateMask=LVIS_FOCUSED|LVIS_SELECTED;


	// Subitem 0
	li1.iSubItem=li2.iSubItem=0;
	m_pList->GetItem(&li1);
	m_pList->GetItem(&li2);
	swap(li1.iItem,li2.iItem);
	m_pList->SetItem(&li1);
	m_pList->SetItem(&li2);

	li1.mask=LVIF_TEXT;
	li2.mask=LVIF_TEXT;
	
	// Subitem 1
	li1.iSubItem=li2.iSubItem=1;
	m_pList->GetItem(&li1);
	m_pList->GetItem(&li2);
	swap(li1.iItem,li2.iItem);
	m_pList->SetItem(&li1);
	m_pList->SetItem(&li2);

	// Subitem 2
	li1.iSubItem=li2.iSubItem=2;
	m_pList->GetItem(&li1);
	m_pList->GetItem(&li2);
	swap(li1.iItem,li2.iItem);
	m_pList->SetItem(&li1);
	m_pList->SetItem(&li2);

	// Subitem 3
	li1.iSubItem=li2.iSubItem=3;
	m_pList->GetItem(&li1);
	m_pList->GetItem(&li2);
	swap(li1.iItem,li2.iItem);
	m_pList->SetItem(&li1);
	m_pList->SetItem(&li2);

	// Check state
	BOOL bState=m_pList->GetCheckState(nSelected);
	m_pList->SetCheckState(nSelected,m_pList->GetCheckState(nOther));
	m_pList->SetCheckState(nOther,bState);

		
	m_pList->EnsureVisible(nOther,FALSE);

	EnableControls();
	m_pList->SetFocus();
	return TRUE;
}

void CSettingsProperties::CDatabasesSettingsPage::CDatabaseDialog::OnAdvanced()
{
	CAdvancedDialog edd(m_pDatabase->GetIncludedFiles(),
		m_pDatabase->GetIncludedDirectories(),
		m_pDatabase->GetExcludedFiles(),
		m_pDatabase->GetExcludedDirectories(),
		IsDlgButtonChecked(IDC_CUSTOMDRIVES)?m_pList:NULL,
		m_pDatabase->GetRootMaps(),
		m_pDatabase->IsFlagSet(CDatabase::flagExcludeContentOfDirsOnly)
		);
	
	if (edd.DoModal(*this))
	{
		m_pDatabase->SetIncludedFiles(edd.m_sIncludedFiles);
		m_pDatabase->SetIncludedDirectories(edd.m_sIncludedDirectories);
		m_pDatabase->SetExcludedFiles(edd.m_sExcludedFiles);
		m_pDatabase->SetExcludedDirectories(edd.m_aExcludedDirectories);
		m_pDatabase->SetRootMapsPtr(alloccopy(edd.m_sRootMaps));
		m_pDatabase->SetFlag(CDatabase::flagExcludeContentOfDirsOnly,edd.m_bExcludeOnlyContentOfDirectories);
	}
}

int CSettingsProperties::CDatabasesSettingsPage::CDatabaseDialog::FindPosInRootList(LPCWSTR szDrive)
{
	LPCWSTR pPtr=m_pDatabase->GetRoots();
	for (int i=0;*pPtr!='\0';i++)
	{
		if (_wcsnicmp(pPtr,szDrive,2)==0 && 
			(pPtr[2]=='\0' || (pPtr[2]=='\\' && pPtr[3]=='\0')))
			return i;
		while (*pPtr!='\0') 
			pPtr++;
		pPtr++;
	}
	return -1;
}

int CSettingsProperties::CDatabasesSettingsPage::CDatabaseDialog::AddDriveToList(LPWSTR szDrive)
{
	DWORD nType=FileSystem::GetDriveType(szDrive);
	//DebugFormatMessage(L"CSettingsProperties::CDatabaseSettingsPage::AddDriveToList(%s),type:%X",szDrive,nType);

	// Find position for drive
	LVITEMW li;
	li.iItem=m_pList->GetItemCount();
	int nPosInRootList=-1;
	if (m_pDatabase->GetRoots()!=NULL)
	{
		// If custom is selected, order should be same
		// as in the string given by GetRoots()
		nPosInRootList=FindPosInRootList(szDrive);
		if (nPosInRootList!=-1)
		{
			CStringW sText;
				
			for (int i=0;i<li.iItem;i++) // li.iItem contains number of items
			{
				m_pList->GetItemText(sText,i,1);
				int nPos=FindPosInRootList(sText);

				if (nPosInRootList<nPos)
				{
					li.iItem=i;
					break;
				}
			}
		}
	}
	

	CStringW Temp;
	WCHAR szLabel[100],szFileSystem[20]=L"";
	switch (nType)
	{
	case DRIVE_FIXED:
		Temp.LoadString(IDS_VOLUMETYPEFIXED);
		break;
	case DRIVE_REMOVABLE:
		Temp.LoadString(IDS_VOLUMETYPEREMOVABLE);
		break;
	case DRIVE_CDROM:
		Temp.LoadString(IDS_VOLUMETYPECDROM);
		break;
	case DRIVE_REMOTE:
		Temp.LoadString(IDS_VOLUMETYPEREMOTE);
		break;
	case DRIVE_RAMDISK:
		Temp.LoadString(IDS_VOLUMETYPERAMDISK);
		break;
	case DRIVE_NO_ROOT_DIR:
	default:
		Temp.LoadString(IDS_VOLUMETYPEUNKNOWN);
		break;
	}

	// Resolving label
	
	
	if (nType==DRIVE_REMOVABLE)
	{
		SHFILEINFOW fi;
		if (ShellFunctions::GetFileInfo(szDrive,0,&fi,SHGFI_DISPLAYNAME|SHGFI_USEFILEATTRIBUTES))
			wcscpy_s(szLabel,100,fi.szDisplayName);
		else
			LoadString(IDS_REMOVABLESTRING,szLabel,20);
	}
	else
	{
		DWORD dwTemp;
		UINT nOldMode=SetErrorMode(SEM_FAILCRITICALERRORS);
				
		if (!FileSystem::GetVolumeInformation(szDrive,szLabel,20,&dwTemp,&dwTemp,&dwTemp,szFileSystem,20))
		{
			szLabel[0]='\0';
			szFileSystem[0]='\0';
		}

		SetErrorMode(nOldMode);

		SHFILEINFOW fi;
		if (ShellFunctions::GetFileInfo(szDrive,0,&fi,SHGFI_DISPLAYNAME|SHGFI_USEFILEATTRIBUTES))
			wcscpy_s(szLabel,20,fi.szDisplayName);
		else if (nType==DRIVE_CDROM && szLabel[0]=='\0')
			LoadString(IDS_CDROMSTRING,szLabel,20);
	}

	// Resolving icon,
	if (GetLocateApp()->GetProgramFlags()&CLocateApp::pfUseDefaultIconForDirectories &&
		FileSystem::GetDriveType(szDrive)==DRIVE_REMOTE)
		li.iImage=DIR_IMAGE;
	else
	{
		SHFILEINFOW fi;
		DWORD dwFlags=SHGFI_SYSICONINDEX|SHGFI_SMALLICON;
		if (GetLocateApp()->GetProgramFlags()&CLocateApp::pfAvoidToAccessWhenReadingIcons)
			dwFlags|=SHGFI_USEFILEATTRIBUTES;
		if (ShellFunctions::GetFileInfo(szDrive,FILE_ATTRIBUTE_DIRECTORY,&fi,dwFlags))
			li.iImage=fi.iIcon;
		else
			li.iImage=DEL_IMAGE;
		
	}

	// Label
	li.iSubItem=0;
	li.mask=LVIF_TEXT|LVIF_IMAGE;
	li.pszText=szLabel;
	m_pList->InsertItem(&li);

	// Path
	li.mask=LVIF_TEXT;
	li.iSubItem=1;
	szDrive[2]=L'\0';
	li.pszText=szDrive;
	m_pList->SetItem(&li);
	
	// Type
	li.pszText=Temp.GetBuffer();
	li.iSubItem=2;
	m_pList->SetItem(&li);
	
	// FS
	li.pszText=szFileSystem;
	li.iSubItem=3;
	m_pList->SetItem(&li);
	
	if (m_pDatabase->GetRoots()!=NULL) 
	{
		if (nPosInRootList!=-1)
			m_pList->SetCheckState(li.iItem,TRUE);
	}
	else if (nType==DRIVE_FIXED)
		m_pList->SetCheckState(li.iItem,TRUE);


	return li.iItem;
}

int CSettingsProperties::CDatabasesSettingsPage::CDatabaseDialog::AddDirectoryToListWithVerify(LPCWSTR szFolder,INT iLength)
{
	CStringW rFolder(szFolder,iLength);

	// Checks  wherther rPath is OK i.e. removes \ at end
	// Further, checks whether folder is already in list 
	// or folder is subfolder for already exists folder
	
		
	// No \ at end
	while (rFolder.LastChar()=='\\')
		rFolder.DelLastChar();

	
	// Check if folder already exists in the list
	LVITEMW li;
	li.iItem=m_pList->GetNextItem(-1,LVNI_ALL);
	while (li.iItem!=-1)
	{
		WCHAR szPath[MAX_PATH];
		li.mask=LVIF_TEXT;
		li.iSubItem=1;
		li.pszText=szPath;
		li.cchTextMax=_MAX_PATH;
		m_pList->GetItem(&li);
		if (rFolder.CompareNoCase(szPath)==0)
		{
			CStringW str;
			str.Format(IDS_FOLDEREXIST,(LPCWSTR)rFolder);
			MessageBox(str,ID2W(IDS_ADDFOLDER),MB_ICONINFORMATION|MB_OK);
			m_pList->SetFocus();
			m_pList->SetCheckState(li.iItem,TRUE);
			m_pList->EnsureVisible(li.iItem,FALSE);
			return -1;
		}
		li.iItem=m_pList->GetNextItem(li.iItem,LVNI_ALL);
	}


	li.iItem=AddDirectoryToList(rFolder);
	m_pList->SetItemState(li.iItem,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
	m_pList->EnsureVisible(li.iItem,FALSE);
	return li.iItem;
}

		
int CSettingsProperties::CDatabasesSettingsPage::CDatabaseDialog::AddDirectoryToList(LPCWSTR szPath,int iLength)
{
	if (iLength==-1)
		iLength=istrlenw(szPath);
	
	WCHAR szLabel[20],szFileSystem[20];
	
	// Resolving label
	DWORD dwTemp;
	LVITEMW li;
	li.iItem=m_pList->GetItemCount();

	CStringW Drive;
	if (szPath[1]==':')
		Drive << szPath[0] << ":\\";
	else
	{
		int nIndex=FirstCharIndex(szPath,L'\\');
		if (nIndex==-1 || szPath[nIndex+1]!=L'\\')
		{
			SdDebugMessage("CDatabaseDialog::AddDirectoryToList: not valid path %s, ignoring");
			return -1;
		}
		
		nIndex=NextCharIndex(szPath,L'\\',nIndex+1);
		if (nIndex==-1)
		{
			Drive.Copy(szPath,iLength);
			Drive << '\\';
		}
		else
			Drive.Copy(szPath,nIndex+1);
	}
	
	
	
	UINT nOldMode=SetErrorMode(SEM_FAILCRITICALERRORS);
	if (!FileSystem::GetVolumeInformation(Drive,szLabel,20,&dwTemp,&dwTemp,&dwTemp,szFileSystem,20))
		szFileSystem[0]='\0';
	SetErrorMode(nOldMode);

	// Resolving icon,
	li.mask=LVIF_TEXT|LVIF_IMAGE;
	li.iSubItem=0;
		
	// Resolving icon,
	SHFILEINFOW fi;
	fi.szDisplayName[0]='\0';
	li.pszText=fi.szDisplayName;
	if (GetLocateApp()->GetProgramFlags()&CLocateApp::pfUseDefaultIconForDirectories)
	{
		li.iImage=DIR_IMAGE;
		li.pszText=const_cast<LPWSTR>(szPath)+LastCharIndex(szPath,L'\\')+1;	
	}
	else
	{
		if (ShellFunctions::GetFileInfo(szPath,FILE_ATTRIBUTE_DIRECTORY,&fi,SHGFI_DISPLAYNAME|SHGFI_SMALLICON|SHGFI_SYSICONINDEX|
			(GetLocateApp()->GetProgramFlags()&CLocateApp::pfAvoidToAccessWhenReadingIcons?SHGFI_USEFILEATTRIBUTES:0)))
			li.iImage=fi.iIcon;
		else
			li.iImage=DEL_IMAGE;
	}
	m_pList->InsertItem(&li);

	// Path
	li.mask=LVIF_TEXT;
	li.iSubItem=1;
	li.pszText=const_cast<LPWSTR>(szPath);
	m_pList->SetItem(&li);

	// Type
	LoadString(IDS_VOLUMETYPEDIRECTORY,szLabel,20);
	li.pszText=szLabel;
	li.iSubItem=2;
	m_pList->SetItem(&li);
	
	// FS
	li.pszText=szFileSystem;
	li.iSubItem=3;
	m_pList->SetItem(&li);

	m_pList->SetCheckState(li.iItem,TRUE);
	return li.iItem;
}

void CSettingsProperties::CDatabasesSettingsPage::CDatabaseDialog::EnableControls()
{
	if (IsDlgButtonChecked(IDC_CUSTOMDRIVES))
	{
		EnableDlgItem(IDC_FOLDERS,TRUE);
		EnableDlgItem(IDC_ADDFOLDER,TRUE);
	
		BOOL bEnableRemove=FALSE;
		// Check current item whether it is drive (drives cannot be removed)
		LVITEM li;
		li.iItem=m_pList->GetNextItem(-1,LVNI_SELECTED);
		if (li.iItem!=-1)
		{
			char szPath[_MAX_PATH];
			li.mask=LVIF_TEXT;
			li.iSubItem=1;
			li.pszText=szPath;
			li.cchTextMax=_MAX_PATH;
			m_pList->GetItem(&li);
		
			// Is the path form of "X:"?
			if (szPath[1]!=':' || szPath[2]!='\0')
				bEnableRemove=TRUE;
		}

		EnableDlgItem(IDC_REMOVEFOLDER,bEnableRemove);

		int nSelected=m_pList->GetNextItem(-1,LVNI_SELECTED);
		if (nSelected!=-1)
		{
			EnableDlgItem(IDC_UP,m_pList->GetNextItem(nSelected,LVNI_ABOVE)!=-1);
			EnableDlgItem(IDC_DOWN,m_pList->GetNextItem(nSelected,LVNI_BELOW)!=-1);
		}
		else
		{
			EnableDlgItem(IDC_UP,FALSE);
			EnableDlgItem(IDC_DOWN,FALSE);
		}
	}
	else
	{
		EnableDlgItem(IDC_FOLDERS,FALSE);
		EnableDlgItem(IDC_ADDFOLDER,FALSE);
		EnableDlgItem(IDC_UP,FALSE);
		EnableDlgItem(IDC_DOWN,FALSE);
	}
	


}







int CSettingsProperties::CDatabasesSettingsPage::CDatabaseDialog::AddComputerToList(LPCWSTR szName)
{
	WCHAR szLabel[100];
	
	
	// Check if the computer already exists in the list
	LVITEMW li;
	li.iItem=m_pList->GetNextItem(-1,LVNI_ALL);
	while (li.iItem!=-1)
	{
		WCHAR szPath[MAX_PATH];
		li.mask=LVIF_TEXT;
		li.iSubItem=1;
		li.pszText=szPath;
		li.cchTextMax=_MAX_PATH;
		m_pList->GetItem(&li);
		if (_wcsicmp(szName,szPath)==0)
		{
			CStringW str;
			str.Format(IDS_FOLDEREXIST,(LPCWSTR)szName);
			MessageBox(str,ID2W(IDS_ADDFOLDER),MB_ICONINFORMATION|MB_OK);
			m_pList->SetFocus();
			m_pList->SetCheckState(li.iItem,TRUE);
			m_pList->EnsureVisible(li.iItem,FALSE);
			return -1;
		}
		li.iItem=m_pList->GetNextItem(li.iItem,LVNI_ALL);
	}



	// Resolving label
	li.iItem=m_pList->GetItemCount();

	// Setting data
	// Label
	SHFILEINFOW fi;
	fi.szDisplayName[0]='\0';
	li.pszText=fi.szDisplayName;

	if (GetLocateApp()->GetProgramFlags()&CLocateApp::pfUseDefaultIconForDirectories)
	{
		li.iImage=DIR_IMAGE;
		li.pszText=const_cast<LPWSTR>(szName)+LastCharIndex(szName,L'\\')+1;	
	}
	else
	{
		// Resolving icon,
		if (ShellFunctions::GetFileInfo(szName,FILE_ATTRIBUTE_DIRECTORY,&fi,SHGFI_DISPLAYNAME|SHGFI_SMALLICON|SHGFI_SYSICONINDEX|
			(GetLocateApp()->GetProgramFlags()&CLocateApp::pfAvoidToAccessWhenReadingIcons?SHGFI_USEFILEATTRIBUTES:0)))
			li.iImage=fi.iIcon;
		else
			li.iImage=DIR_IMAGE;
	}

	li.iSubItem=0;
	li.mask=LVIF_TEXT|LVIF_IMAGE;
	m_pList->InsertItem(&li);

	// Path
	li.mask=LVIF_TEXT;
	li.iSubItem=1;
	li.pszText=const_cast<LPWSTR>(szName);
	m_pList->SetItem(&li);
	
	// Type
	LoadString(IDS_VOLUMETYPECOMPUTER,szLabel,100);
	li.pszText=szLabel;
	li.iSubItem=2;
	m_pList->SetItem(&li);
	
	// FS
	m_pList->SetItemState(li.iItem,0x2000,LVIS_STATEIMAGEMASK);
	return li.iItem;
}

/////////////////////////////////////////////////
// CDatabasesSettingsPage::CDatabaseDialog::CExcludedDirectoryDialog

BOOL CSettingsProperties::CDatabasesSettingsPage::CDatabaseDialog::CAdvancedDialog::OnInitDialog(HWND hwndFocus)
{
	CDialog::OnInitDialog(hwndFocus);
	CListBox List(GetDlgItem(IDC_DIRECTORIES));

	SetDlgItemText(IDC_INCLUDEFILES,m_sIncludedFiles);
	SetDlgItemText(IDC_INCLUDEDIRECTORIES,m_sIncludedDirectories);
	SetDlgItemText(IDC_EXCLUDEFILES,m_sExcludedFiles);
	CheckDlgButton(IDC_EXCLUDEONLYCONTENT,m_bExcludeOnlyContentOfDirectories);

	// Inserting strings
	for (int i=0;i<m_aExcludedDirectories.GetSize();i++)
		List.AddString(m_aExcludedDirectories[i]);
	
	
	// Initialize drive list
	m_pDriveList=new CListCtrl(GetDlgItem(IDC_FOLDERS));
	CLocateDlg::SetSystemImageLists(m_pDriveList);
	m_pDriveList->SetExtendedListViewStyle(LVS_EX_HEADERDRAGDROP|LVS_EX_FULLROWSELECT,
		LVS_EX_HEADERDRAGDROP|LVS_EX_FULLROWSELECT);
	if (IsUnicodeSystem())
		m_pDriveList->SetUnicodeFormat(TRUE);
	m_pDriveList->InsertColumn(0,ID2W(IDS_VOLUMELABEL),LVCFMT_LEFT,95,0);
	m_pDriveList->InsertColumn(1,ID2W(IDS_VOLUMEPATH),LVCFMT_LEFT,105,1);
	m_pDriveList->InsertColumn(2,ID2W(IDS_VOLUMEINDATABASE),LVCFMT_LEFT,115,1);
	m_pDriveList->LoadColumnsState(HKCU,CRegKey2::GetCommonKey()+"\\Dialogs","Database Advanced Dialog List Widths");
	
	// Copy items to list from m_pCopyItemsFrom list
	LVITEMW li;
	WCHAR szBuffer[MAX_PATH+100];
	
	if (m_pCopyItemsFrom==NULL)
	{
		// Local drives
		li.iItem=0;
		
		// Inserting local drives to drive list
		DWORD nLength=GetLogicalDriveStrings(0,NULL)+1;
		if (nLength<2)
			return FALSE;

		WCHAR* szDrives=new WCHAR[nLength+1];
		FileSystem::GetLogicalDriveStrings(nLength,szDrives);
		for (LPWSTR szDrive=szDrives;szDrive[0]!=L'\0';szDrive+=4)
		{
			if (FileSystem::GetDriveType(szDrive)!=DRIVE_FIXED)
				continue;


			DWORD dwTemp;
			UINT nOldMode=SetErrorMode(SEM_FAILCRITICALERRORS);
	
			if (!FileSystem::GetVolumeInformation(szDrive,szBuffer,MAX_PATH+100,&dwTemp,&dwTemp,&dwTemp,NULL,0))
				szBuffer[0]='\0';
			
			SetErrorMode(nOldMode);
	
			if (GetLocateApp()->GetProgramFlags()&CLocateApp::pfUseDefaultIconForDirectories)
				li.iImage=DIR_IMAGE;
			else
			{
				SHFILEINFOW fi;
				if (ShellFunctions::GetFileInfo(szDrive,FILE_ATTRIBUTE_DIRECTORY,&fi,SHGFI_SMALLICON|SHGFI_SYSICONINDEX|
					(GetLocateApp()->GetProgramFlags()&CLocateApp::pfAvoidToAccessWhenReadingIcons?SHGFI_USEFILEATTRIBUTES:0)))
					li.iImage=fi.iIcon;
				else
					li.iImage=DEL_IMAGE;
			}
			
			// Label
			li.iSubItem=0;
			li.mask=LVIF_TEXT|LVIF_IMAGE;
			li.pszText=szBuffer;
			m_pDriveList->InsertItem(&li);

			// Path
			li.mask=LVIF_TEXT;
			li.iSubItem=1;
			szDrive[2]=L'\0';
			li.pszText=szDrive;
			m_pDriveList->SetItem(&li);

		

			li.iItem++;
		}
		delete[] szDrives;
	}
	else
	{
		int iNewItem=0;
		li.pszText=szBuffer;
		li.cchTextMax=MAX_PATH+100;
	
		int iItem=m_pCopyItemsFrom->GetNextItem(-1,LVNI_ALL);
		while (iItem!=-1)
		{
			if (m_pCopyItemsFrom->GetCheckState(iItem))
			{
				// Label
				li.mask=LVIF_TEXT|LVIF_IMAGE;
				li.iSubItem=0;
				li.iItem=iItem;
				m_pCopyItemsFrom->GetItem(&li);
				li.iItem=iNewItem;
				m_pDriveList->InsertItem(&li);
				
				// Path
				li.mask=LVIF_TEXT;
				li.iSubItem=1;
				li.iItem=iItem;
				m_pCopyItemsFrom->GetItem(&li);
				li.iItem=iNewItem;
				m_pDriveList->SetItem(&li);

		
				iNewItem++;
			}

			iItem=m_pCopyItemsFrom->GetNextItem(iItem,LVNI_ALL);
		}
	}

	// Check active maps
	li.mask=LVIF_TEXT;
	li.pszText=szBuffer;
	li.cchTextMax=MAX_PATH+100;
	
	LPCWSTR pMap=m_sRootMaps;
	while (*pMap!='\0')
	{
		// Get length of source
		int nLen=0;
		for (;pMap[nLen]!=L'>' && pMap[nLen]!=L'|' && pMap[nLen]!='\0';nLen++);
		if (pMap[nLen]=='\0')
			break;

		// No target?
		if (pMap[nLen]==L'|')
		{
			pMap+=nLen+1;
			continue;
		}

		li.iSubItem=1;
		li.iItem=m_pDriveList->GetNextItem(-1,LVNI_ALL);
		while (li.iItem!=-1)
		{
			m_pDriveList->GetItem(&li);

			if (_wcsnicmp(pMap,szBuffer,nLen)==0)		
			{
				if (szBuffer[nLen]=='\0')
					break;
			}
			li.iItem=m_pDriveList->GetNextItem(li.iItem,LVNI_ALL);
		}

		if (li.iItem!=-1)
		{
			// Change pMap to point target
			pMap+=nLen+1;
			for (nLen=0;pMap[nLen]!='|' && pMap[nLen]!='\0';nLen++);
		
			// Set target to list
			MemCopyW(szBuffer,pMap,nLen);
			szBuffer[nLen]='\0';
			li.iSubItem=2;
			m_pDriveList->SetItem(&li);
		}
		else
		{
			// Insert new item to list
			li.iItem=m_pDriveList->GetItemCount();

			// Copy source to buffer
			MemCopyW(szBuffer,pMap,nLen);
			szBuffer[nLen]='\0';
			
			SHFILEINFOW fi;
			fi.szDisplayName[0]='\0';
			li.pszText=fi.szDisplayName;
			if (GetLocateApp()->GetProgramFlags()&CLocateApp::pfUseDefaultIconForDirectories)
			{
				li.iImage=DIR_IMAGE;
				li.pszText=szBuffer+LastCharIndex(szBuffer,L'\\')+1;
			}
			else
			{
				if (ShellFunctions::GetFileInfo(szBuffer,FILE_ATTRIBUTE_DIRECTORY,&fi,SHGFI_SMALLICON|SHGFI_SYSICONINDEX|SHGFI_DISPLAYNAME)|
					(GetLocateApp()->GetProgramFlags()&CLocateApp::pfAvoidToAccessWhenReadingIcons?SHGFI_USEFILEATTRIBUTES:0))
					li.iImage=fi.iIcon;
				else
					li.iImage=DEL_IMAGE;
			}
	

			// Label
			li.iSubItem=0;
			li.mask=LVIF_TEXT|LVIF_IMAGE;
			m_pDriveList->InsertItem(&li);

			// Path
			li.mask=LVIF_TEXT;
			li.iSubItem=1;
			li.pszText=szBuffer;
			m_pDriveList->SetItem(&li);

			// Change pMap to point target
			pMap+=nLen+1;
			for (nLen=0;pMap[nLen]!='|' && pMap[nLen]!='\0';nLen++);
			
			// Set target to list
			MemCopyW(szBuffer,pMap,nLen);
			szBuffer[nLen]='\0';
			li.iSubItem=2;
			m_pDriveList->SetItem(&li);
		}

		pMap+=nLen+1;				
	}

	// Insert (new map) item
	li.mask=LVIF_TEXT|LVIF_IMAGE;
	LoadString(IDS_CLKFORNEWMAP,szBuffer,MAX_PATH+100);
	li.pszText=szBuffer;
	li.iItem=m_pDriveList->GetItemCount();
	li.iSubItem=0;
	li.iImage=-1;
	m_pDriveList->InsertItem(&li);

	EnableControlsExclude();
	EnableControlsAndSetMaps();
	SetFocus(IDC_INCLUDEFILES);
	return FALSE;
}


void CSettingsProperties::CDatabasesSettingsPage::CDatabaseDialog::CAdvancedDialog::OnHelp(LPHELPINFO lphi)
{
	CLocateApp::HelpID id[]= {
		{ IDC_INCLUDEFILES,"sda_includefiles" },
		{ IDC_INCLUDEDIRECTORIES,"sda_includedirectories" },
		{ IDC_EXCLUDEFILES,"sda_excludefiles" },
		{ IDC_DIRECTORYNAME,"sda_excludedirectories" },
		{ IDC_BROWSE,"sda_excludedirectories" },
		{ IDC_ADDFOLDER,"sda_excludedirectories" },
		{ IDC_REMOVEFOLDER,"sda_excludedirectories" },
		{ IDC_DIRECTORIES,"sda_excludedirectories" },
		{ IDC_EXCLUDEONLYCONTENT,"sda_onlycontent" },
		{ IDC_FOLDERS,"sda_maps" },
		{ IDC_PATHINDATABASELABEL,"sda_maps" },
		{ IDC_PATHINDATABASE,"sda_maps" },
		{ IDC_SET,"sda_maps" }		
	};
	
	if (CLocateApp::OpenHelp(*this,"settings_databaseadv.htm",lphi,id,sizeof(id)/sizeof(CLocateApp::HelpID)))
		return;

	
	if (HtmlHelp(HH_HELP_CONTEXT,HELP_SETTINGS_DATABASEADVANCED)==NULL)
		HtmlHelp(HH_DISPLAY_TOPIC,0);
}


BOOL CSettingsProperties::CDatabasesSettingsPage::CDatabaseDialog::CAdvancedDialog::OnNotify(int idCtrl,LPNMHDR pnmh)
{
	switch (idCtrl)
	{
	case IDC_FOLDERS:
		switch (((NMLISTVIEW*)pnmh)->hdr.code)
		{
		case LVN_ITEMCHANGED:
			if ((((NMLISTVIEW*)pnmh)->uNewState&LVIS_SELECTED)!=
				(((NMLISTVIEW*)pnmh)->uOldState&LVIS_SELECTED))
				EnableControlsAndSetMaps();
			break;	
		case LVN_KEYDOWN:
			if (((LPNMLVKEYDOWN)pnmh)->wVKey!=VK_SPACE)
				break;
			// Continue
		case NM_CLICK:
			{
				WCHAR szBuffer[MAX_PATH+100];
				LVITEMW li;
				li.mask=LVIF_TEXT;
				li.iSubItem=1;
				li.pszText=szBuffer;
				li.cchTextMax=MAX_PATH+100;
				li.iItem=m_pDriveList->GetNextItem(-1,LVNI_SELECTED);
				if (li.iItem==-1)
					break;

				m_pDriveList->GetItem(&li);

				if (szBuffer[0]!='\0')
				{
					// Not (new item) 
					break;
				}

				// (new item) selected
				CInputDialog ib(IDD_INPUTBOX);
				ib.SetText(IDS_ENTERNEWDIRECTORY);
				ib.SetTitle(IDS_NEWMAP);
				ib.SetOKButtonText(IDS_OK);
				ib.SetCancelButtonText(IDS_CANCEL);
				if (ib.DoModal(*this))
				{
					ib.GetInputText(szBuffer,MAX_PATH+100);

					SHFILEINFOW fi;
					fi.szDisplayName[0]='\0';
					li.pszText=fi.szDisplayName;
					if (GetLocateApp()->GetProgramFlags()&CLocateApp::pfUseDefaultIconForDirectories)
					{
						li.pszText=szBuffer+LastCharIndex(szBuffer,L'\\')+1;	
						li.iImage=DIR_IMAGE;
					}
					else
					{
						if (ShellFunctions::GetFileInfo(szBuffer,FILE_ATTRIBUTE_DIRECTORY,&fi,
							SHGFI_SMALLICON|SHGFI_SYSICONINDEX|SHGFI_DISPLAYNAME|
							(GetLocateApp()->GetProgramFlags()&CLocateApp::pfAvoidToAccessWhenReadingIcons?SHGFI_USEFILEATTRIBUTES:0)))
							li.iImage=fi.iIcon;
						else
							li.iImage=DEL_IMAGE;
					}
					
					// Label
					li.iSubItem=0;
					li.mask=LVIF_TEXT|LVIF_IMAGE;
					m_pDriveList->SetItem(&li);

					// Path
					li.mask=LVIF_TEXT;
					li.iSubItem=1;
					li.pszText=szBuffer;
					m_pDriveList->SetItem(&li);
					
					// Insert (new map) item
					li.mask=LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM;
					LoadString(IDS_CLKFORNEWMAP,szBuffer,MAX_PATH+100);
					li.iItem=m_pDriveList->GetItemCount();
					li.iSubItem=0;
					li.iImage=-1;
					li.lParam=-1;
					m_pDriveList->InsertItem(&li);

					m_pDriveList->EnsureVisible(((LPNMITEMACTIVATE)pnmh)->iItem,FALSE);

					EnableControlsAndSetMaps();
				}	
				break;
			}
		}
		break;
	}
	return CDialog::OnNotify(idCtrl,pnmh);
}

BOOL CSettingsProperties::CDatabasesSettingsPage::CDatabaseDialog::CAdvancedDialog::OnCommand(WORD wID,WORD wNotifyCode,HWND hControl)
{
	CDialog::OnCommand(wID,wNotifyCode,hControl);
	switch(wID)
	{
	case IDOK:
		OnOK();
		break;
	case IDC_DIRECTORYNAME:
		if (wNotifyCode==EN_SETFOCUS)
		{
			::SendMessage(hControl,EM_SETSEL,0,MAKELPARAM(0,-1));
			
			if (::SendMessage(hControl,WM_GETTEXTLENGTH,0,0)>0)
				SetDefID(IDC_ADDFOLDER);
		}
		else if (wNotifyCode==EN_KILLFOCUS)
			SetDefID(IDC_OK);
		else if (wNotifyCode==EN_CHANGE)
		{
			EnableControlsExclude();
			SetDefID(::SendMessage(hControl,WM_GETTEXTLENGTH,0,0)>0?
				IDC_ADDFOLDER:IDC_OK);
		}
		break;
	case IDC_OK:
		OnOK();
		break;
	case IDCANCEL:
	case IDC_CANCEL:
		EndDialog(0);
		break;
	case IDC_BROWSE:
		OnBrowse();
		break;
	case IDC_ADDFOLDER:
		OnAddFolder(TRUE);
		break;
	case IDC_REMOVEFOLDER:
		OnRemove();
		break;
	case IDC_DIRECTORIES:
		if (wNotifyCode==LBN_SELCHANGE)
			EnableControlsExclude();
	case IDC_PATHINDATABASE:
		if (wNotifyCode==EN_SETFOCUS)
		{
			::SendMessage(hControl,EM_SETSEL,0,MAKELPARAM(0,-1));
			SetDefID(IDC_SET);
		}
		else if (wNotifyCode==EN_KILLFOCUS)
			SetDefID(IDC_OK);
		else if (wNotifyCode==EN_CHANGE)
			EnableControlsExclude();
		break;
	case IDC_SET:
		OnSet();
		break;
	}
	return FALSE;
}

void CSettingsProperties::CDatabasesSettingsPage::CDatabaseDialog::CAdvancedDialog::OnDestroy()
{
	if (m_pDriveList!=NULL)
	{
		m_pDriveList->SaveColumnsState(HKCU,CRegKey2::GetCommonKey()+"\\Dialogs","Database Advanced Dialog List Widths");
		delete m_pDriveList;
		m_pDriveList=NULL;
	}
	CDialog::OnDestroy();
}


BOOL CSettingsProperties::CDatabasesSettingsPage::CDatabaseDialog::CAdvancedDialog::OnClose()
{
	CDialog::OnClose();
	EndDialog(0);
	return 0;
}

void CSettingsProperties::CDatabasesSettingsPage::CDatabaseDialog::CAdvancedDialog::EnableControlsExclude()
{
	EnableDlgItem(IDC_REMOVEFOLDER,SendDlgItemMessage(IDC_DIRECTORIES,LB_GETSEL)!=LB_ERR?TRUE:FALSE);
	EnableDlgItem(IDC_ADDFOLDER,GetDlgItemTextLength(IDC_DIRECTORYNAME)>0);
}

void CSettingsProperties::CDatabasesSettingsPage::CDatabaseDialog::CAdvancedDialog::EnableControlsAndSetMaps()
{
	int nSelected=m_pDriveList->GetNextItem(-1,LVNI_SELECTED);
	if (nSelected!=-1)
	{
		// Check whether selection is (new item)
		WCHAR szBuffer[10]=L"";
		LVITEMW li;
		li.mask=LVIF_TEXT;
		li.iSubItem=1;
		li.pszText=szBuffer;
		li.cchTextMax=10;
		li.iItem=nSelected;
		m_pDriveList->GetItem(&li);
		if (szBuffer[0]=='\0')
			nSelected=-1;
	}		

	EnableDlgItem(IDC_PATHINDATABASE,nSelected!=-1);
	EnableDlgItem(IDC_PATHINDATABASELABEL,nSelected!=-1);
	EnableDlgItem(IDC_SET,nSelected!=-1);

	if (nSelected!=-1)
	{
		CStringW sPath;
		m_pDriveList->GetItemText(sPath,nSelected,2);
		SetDlgItemText(IDC_PATHINDATABASE,sPath);
	}
	else
		SetDlgItemText(IDC_PATHINDATABASE,"");
}

void CSettingsProperties::CDatabasesSettingsPage::CDatabaseDialog::CAdvancedDialog::OnOK()
{
	EndDialog(1);

	GetDlgItemText(IDC_INCLUDEFILES,m_sIncludedFiles);
	GetDlgItemText(IDC_INCLUDEDIRECTORIES,m_sIncludedDirectories);
	GetDlgItemText(IDC_EXCLUDEFILES,m_sExcludedFiles);
	m_bExcludeOnlyContentOfDirectories=IsDlgButtonChecked(IDC_EXCLUDEONLYCONTENT);
	
	
	
	
	m_aExcludedDirectories.RemoveAll();

	CListBox Directories(GetDlgItem(IDC_DIRECTORIES));
	int nCount=Directories.GetCount();
	for (int i=0;i<nCount;i++)
	{
		DWORD iLength=Directories.GetTextLen(i)+1;
		WCHAR* pText=new WCHAR[max(iLength,2)];
		Directories.GetText(i,pText);
		m_aExcludedDirectories.Add(pText);
	}

	m_sRootMaps.Empty();
	int nItem=m_pDriveList->GetNextItem(-1,LVNI_ALL);
	while (nItem!=-1)
	{
		CStringW sPathInDb;
		m_pDriveList->GetItemText(sPathInDb,nItem,2);
		if (!sPathInDb.IsEmpty())
		{
			CStringW sDrive;
			m_pDriveList->GetItemText(sDrive,nItem,1);
			
			if (!m_sRootMaps.IsEmpty())
				m_sRootMaps << L'|';
			m_sRootMaps << sDrive << L'>' << sPathInDb;
		}

		nItem=m_pDriveList->GetNextItem(nItem,LVNI_ALL);
	}
}

void CSettingsProperties::CDatabasesSettingsPage::CDatabaseDialog::CAdvancedDialog::OnSet()
{
	CStringW sPath;
	GetDlgItemText(IDC_PATHINDATABASE,sPath);

	if (sPath.FindOneOf(L">|")!=-1)
	{
		ShowErrorMessage(IDS_INVALIDCHARACTERS,IDS_ERROR);
		SetFocus(IDC_PATHINDATABASE);
		return;
	}

	int nItem=m_pDriveList->GetNextItem(-1,LVNI_SELECTED);
	if (nItem!=-1)
	{
		m_pDriveList->SetItemText(nItem,2,sPath);
	}

	SetDefID(IDC_OK);
}

BOOL CSettingsProperties::CDatabasesSettingsPage::CDatabaseDialog::CAdvancedDialog::OnAddFolder(BOOL bShowErrorMessageIfExists)
{
	CStringW sDirectoryPre,sDirectory;
	CEdit DirectoryName(GetDlgItem(IDC_DIRECTORYNAME));
	CListBox Directories(GetDlgItem(IDC_DIRECTORIES));
	
	DirectoryName.GetText(sDirectoryPre);
	sDirectoryPre.ReplaceChars('/','\\');
	if (sDirectoryPre.Find('*')==-1 && sDirectory.Find('.')==-1)
	{
		// Exact path
		if (!FileSystem::GetFullPathName(sDirectoryPre,400,sDirectory.GetBuffer(400),NULL))
		{
			CStringW str;
			str.Format(IDS_ERRORDIRECTORYNOTFOUND,LPCWSTR(sDirectory));
			MessageBox(str,ID2W(IDS_ERROR),MB_OK|MB_ICONERROR);
			DirectoryName.SetFocus();
			return FALSE;	
		}
		sDirectory.FreeExtra();


		if (!FileSystem::IsDirectory(sDirectory))
		{
			CStringW str;
			str.Format(IDS_ERRORDIRECTORYNOTFOUND,LPCWSTR(sDirectory));
			MessageBox(str,ID2W(IDS_ERROR),MB_OK|MB_ICONERROR);
			DirectoryName.SetFocus();
			return FALSE;
		}

		sDirectoryPre=sDirectory;
	}
	else
		sDirectory=sDirectoryPre;
		
	sDirectoryPre.MakeLower();

	
	for (int i=Directories.GetCount()-1;i>=0;i--)
	{
		int iLength=Directories.GetTextLen(i);
        WCHAR* szText=new WCHAR[iLength+2];
		Directories.GetText(i,szText);
		MakeLower(szText);

		if (sDirectoryPre.Compare(szText)==0)
		{
			if (bShowErrorMessageIfExists)
				ShowErrorMessage(IDS_ALREADYEXCLUDED,IDS_ERROR);
			return TRUE;
		}		
	}
    
	Directories.AddString(sDirectory);

	SetFocus(IDC_DIRECTORIES);
	EnableControlsExclude();
	
	SetDefID(IDC_OK);
	return FALSE;
}

void CSettingsProperties::CDatabasesSettingsPage::CDatabaseDialog::CAdvancedDialog::OnRemove()
{
	int nSel=(int)SendDlgItemMessage(IDC_DIRECTORIES,LB_GETCURSEL);
	if (nSel==LB_ERR)
		return;

	SendDlgItemMessage(IDC_DIRECTORIES,LB_DELETESTRING,nSel);

	EnableControlsExclude();
}

void CSettingsProperties::CDatabasesSettingsPage::CDatabaseDialog::CAdvancedDialog::OnBrowse()
{
	CWaitCursor;

	// Ask folder
	CFolderDialog fd(IDS_ADDFOLDER,BIF_RETURNONLYFSDIRS|BIF_USENEWUI|BIF_NONEWFOLDERBUTTON);
	if (fd.DoModal(*this))
	{
		// Insert folder to list
		CStringW Folder;
		if (!fd.GetFolder(Folder))
			ShowErrorMessage(IDS_CANNOTEXCLUDESELECTED,IDS_ERROR);
		else
			SetDlgItemText(IDC_DIRECTORYNAME,Folder);
		
		// Setting focus to list
		SetFocus(IDC_DIRECTORYNAME);
	}

	

}

////////////////////////////////////////
// CAutoUpdateSettingsPage
////////////////////////////////////////

BOOL CSettingsProperties::CAutoUpdateSettingsPage::OnInitDialog(HWND hwndFocus)
{
	CPropertyPage::OnInitDialog(hwndFocus);
	m_pSchedules=new CListBox(GetDlgItem(IDC_UPDATES));
	
	// Set spin
	CSpinButtonCtrl Spin(GetDlgItem(IDC_DELAYSPIN));
	Spin.SetBuddy(GetDlgItem(IDC_DELAY));
	Spin.SetRange(1,1000);
	Spin.SetPos(m_pSettings->m_dwSchedulesDelay);


	// Load schedules
	
	POSITION pPos=m_pSettings->m_Schedules.GetHeadPosition();
	while (pPos!=NULL)
	{
		m_pSchedules->InsertString(-1,(LPCSTR)m_pSettings->m_Schedules.GetAt(pPos));
		pPos=m_pSettings->m_Schedules.GetNextPosition(pPos);
	}
	return FALSE;
}

void CSettingsProperties::CAutoUpdateSettingsPage::OnHelp(LPHELPINFO lphi)
{
	if (HtmlHelp(HH_HELP_CONTEXT,HELP_SETTINGS_AUTOUPDATE)==NULL)
		HtmlHelp(HH_DISPLAY_TOPIC,0);
}

void CSettingsProperties::CAutoUpdateSettingsPage::EnableItems()
{
	int nCurSel=m_pSchedules->GetCurSel();
	if (nCurSel==-1)
	{
		EnableDlgItem(IDC_EDIT,FALSE);
		EnableDlgItem(IDC_DELETE,FALSE);

		EnableDlgItem(IDC_DOWN,FALSE);
		EnableDlgItem(IDC_UP,FALSE);
	}
	else
	{
		EnableDlgItem(IDC_EDIT,TRUE);
		EnableDlgItem(IDC_DELETE,TRUE);	

		EnableDlgItem(IDC_DOWN,nCurSel!=m_pSchedules->GetCount()-1);
		EnableDlgItem(IDC_UP,nCurSel!=0);
	}
}

BOOL CSettingsProperties::CAutoUpdateSettingsPage::OnCommand(WORD wID,WORD wNotifyCode,HWND hControl)
{
	CPropertyPage::OnCommand(wID,wNotifyCode,hControl);
	switch (wID)
	{
	case IDC_ADD:
		{
			CCheduledUpdateDlg sud;
			sud.m_pSchedule=new CSchedule;
			if (sud.DoModal(*this))
			{
				m_pSettings->m_Schedules.AddTail(sud.m_pSchedule);
				m_pSchedules->InsertString(-1,(LPCSTR)sud.m_pSchedule);
			}
			else
				delete sud.m_pSchedule;

			EnableItems();
			break;
		}
	case IDC_DELETE:
		{
			int nCurSel=m_pSchedules->GetCurSel();
			if (nCurSel==-1)
				break;
			CSchedule* tmp=(CSchedule*)m_pSchedules->GetItemData(nCurSel);
			m_pSchedules->DeleteString(nCurSel);
			m_pSettings->m_Schedules.RemoveAt(m_pSettings->m_Schedules.Find(tmp));

			EnableItems();
						
			break;
		}
	case IDC_EDIT:
		OnEdit();
		break;
	case IDC_UPDATES:
		if (wNotifyCode==LBN_DBLCLK)
			OnEdit();
		else if (wNotifyCode==LBN_SELCHANGE)
		{
			CStringW txt;
			EnableItems();

			int nCurSel=m_pSchedules->GetCurSel();
			if (nCurSel==-1)
				break;
		
			CSchedule* pSchedule=(CSchedule*)m_pSchedules->GetItemData(nCurSel);
			if (pSchedule==NULL)
				break;
			if (pSchedule->m_bFlags&CSchedule::flagRunned)
			{
				SYSTEMTIME st;
				st.wYear=pSchedule->m_tLastStartDate.wYear;
				st.wMonth=pSchedule->m_tLastStartDate.bMonth;
				st.wDay=pSchedule->m_tLastStartDate.bDay;
				st.wHour=pSchedule->m_tLastStartTime.bHour;
				st.wMinute=pSchedule->m_tLastStartTime.bMinute;
				st.wSecond=pSchedule->m_tLastStartTime.bSecond;
				st.wMilliseconds=0;
				
				if (IsUnicodeSystem())
				{
					WCHAR szDate[100],szTime[100];
					GetTimeFormatW(LOCALE_USER_DEFAULT,0,&st,NULL,szTime,100);
					GetDateFormatW(LOCALE_USER_DEFAULT,DATE_LONGDATE,&st,NULL,szDate,100);
					txt.FormatEx(IDS_LASTRUN,szDate,szTime);
				}
				else
				{
					char szDate[100],szTime[100];
					GetTimeFormat(LOCALE_USER_DEFAULT,0,&st,NULL,szTime,100);
					GetDateFormat(LOCALE_USER_DEFAULT,DATE_LONGDATE,&st,NULL,szDate,100);
					txt.FormatEx(IDS_LASTRUN,A2W(szDate),A2W(szTime));
				}
			}
			else
				txt.LoadString(IDS_LASTRUNNEVER);
			SetDlgItemText(IDC_LASTRUN,(LPCWSTR)txt);
		}
		break;
	case IDC_UP:
	case IDC_DOWN:
		{
			int nSelItem=m_pSchedules->GetCurSel();
			int nAltItem=nSelItem+(wID==IDC_DOWN?1:-1);



			if (nSelItem==-1 ||
				(wID==IDC_UP && nSelItem==0) ||
				(wID==IDC_DOWN && nSelItem==m_pSchedules->GetCount()-1))
				break;
			
			CSchedule* pSchedule=(CSchedule*)m_pSchedules->GetItemData(nSelItem);
			CSchedule* pAltSchedule=(CSchedule*)m_pSchedules->GetItemData(nAltItem);

			if (pSchedule==NULL || pAltSchedule==NULL)
				break;
            
			POSITION pPos=m_pSettings->m_Schedules.Find(pSchedule);
			POSITION pAltPos=m_pSettings->m_Schedules.Find(pAltSchedule);

			if (pPos==NULL || pAltPos==NULL)
				break;
            
			m_pSettings->m_Schedules.SetAt(pPos,pAltSchedule);
			m_pSchedules->SetItemData(nSelItem,DWORD(pAltSchedule));
			
			m_pSettings->m_Schedules.SetAt(pAltPos,pSchedule);
			m_pSchedules->SetItemData(nAltItem,DWORD(pSchedule));

			m_pSchedules->UpdateWindow();
			m_pSchedules->InvalidateRect(NULL,TRUE);
			m_pSchedules->SetCurSel(nAltItem);
			
			EnableItems();

			break;
		}


	}
	return FALSE;
}

void CSettingsProperties::CAutoUpdateSettingsPage::OnEdit()
{
	CCheduledUpdateDlg sud;
	int nCurSel=m_pSchedules->GetCurSel();
	if (nCurSel==-1)
		return;
	sud.m_pSchedule=(CSchedule*)m_pSchedules->GetItemData(nCurSel);
	sud.DoModal(*this);
	m_pSchedules->SetCurSel(nCurSel);
}

void CSettingsProperties::CAutoUpdateSettingsPage::OnDestroy()
{
	CPropertyPage::OnDestroy();
	if (m_pSchedules!=NULL)
	{
		delete m_pSchedules;
		m_pSchedules=NULL;
	}
}

void CSettingsProperties::CAutoUpdateSettingsPage::OnDrawItem(UINT idCtl,LPDRAWITEMSTRUCT lpdis)
{
	if (idCtl==IDC_UPDATES)
	{
		CDC dc(lpdis->hDC);
		HICON hIcon;
		CSchedule* pSchedule=(CSchedule*)lpdis->itemData;
		if (pSchedule==NULL)
			return;
		CBrush brush(GetSysColor(COLOR_WINDOW));
		HPEN hOldPen=(HPEN)dc.SelectObject(GetStockObject(WHITE_PEN));
		HBRUSH hOldBrush=(HBRUSH)dc.SelectObject(brush);
		dc.SetBkMode(TRANSPARENT);
		dc.Rectangle(&(lpdis->rcItem));
		if (lpdis->itemState&ODS_SELECTED)
		{
			CBrush Brush(GetSysColor(COLOR_HIGHLIGHT));
			CPen Pen(PS_SOLID,1,GetSysColor(COLOR_HIGHLIGHT));
			dc.SelectObject(Brush);
			dc.SelectObject(Pen);
			dc.Rectangle(lpdis->rcItem.left+1,lpdis->rcItem.top+1,lpdis->rcItem.right-1,lpdis->rcItem.bottom-1);
			dc.SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
			dc.SelectObject(brush);
		}
		else
			dc.SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
		
		if (pSchedule->m_bFlags&CSchedule::flagEnabled)
			hIcon=(HICON)LoadImage(IDI_YES,IMAGE_ICON,16,16,0);
		else
			hIcon=(HICON)LoadImage(IDI_NO,IMAGE_ICON,16,16,0);
		DebugOpenGdiObject(hIcon);

		CRect rc(lpdis->rcItem);
		dc.DrawState(CPoint(rc.left,rc.top),CSize(16,16),hIcon,DST_ICON);
		rc.left+=16;
		rc.top++;
	
		CStringW str;
		pSchedule->GetString(str);
		dc.DrawText(str,&rc,DT_LEFT|DT_VCENTER);

		dc.SelectObject(hOldPen);
		dc.SelectObject(hOldBrush);
	}
	CPropertyPage::OnDrawItem(idCtl,lpdis);
}

void CSettingsProperties::CAutoUpdateSettingsPage::OnMeasureItem(int nIDCtl,LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	switch(nIDCtl)
	{
	case IDC_UPDATES:
		{
			CDC dc(this);
			CSize sz;
			CStringW str;
			
			((CSchedule*)lpMeasureItemStruct->itemData)->GetString(str);
			sz=dc.GetTextExtent(str);
			
			lpMeasureItemStruct->itemWidth=sz.cx+2;
			lpMeasureItemStruct->itemHeight=max(sz.cy,16)+3;
			break;
		}
	}
	CPropertyPage::OnMeasureItem(nIDCtl,lpMeasureItemStruct);
}
		

BOOL CSettingsProperties::CAutoUpdateSettingsPage::OnApply()
{
	CPropertyPage::OnApply();

	BOOL bTranslated;
	m_pSettings->m_dwSchedulesDelay=GetDlgItemInt(IDC_DELAY,&bTranslated,FALSE);
	if (!bTranslated)
		m_pSettings->m_dwSchedulesDelay=0;

	return TRUE;
}

void CSettingsProperties::CAutoUpdateSettingsPage::OnCancel()
{
	m_pSettings->SetSettingsFlags(CSettingsProperties::settingsCancelled);

	CPropertyPage::OnCancel();
}


/////////////////////////////////////////////////
// CAutoUpdateSettingsPage::CCheduledUpdateDlg

BOOL CSettingsProperties::CAutoUpdateSettingsPage::CCheduledUpdateDlg::OnInitDialog(HWND hwndFocus)
{
	// Type combo
	int nSel[]={2,3,4,5,6,1,0};
	m_pTypeCombo=new CComboBox(GetDlgItem(IDC_TYPE));
	m_pTypeCombo->AddString(ID2W(IDS_MINUTELY));
	m_pTypeCombo->AddString(ID2W(IDS_HOURLY));
	m_pTypeCombo->AddString(ID2W(IDS_DAILY));
	m_pTypeCombo->AddString(ID2W(IDS_WEEKLY));
	m_pTypeCombo->AddString(ID2W(IDS_MONTHLY));
	m_pTypeCombo->AddString(ID2W(IDS_ONCE));
	m_pTypeCombo->AddString(ID2W(IDS_ATSTARTUP));
	m_pTypeCombo->SetCurSel(nSel[m_pSchedule->m_nType]);
	
	// Month type combo
	CComboBox Combo(GetDlgItem(IDC_MTYPE));
	Combo.AddString(ID2W(IDS_WEEKFIRST));
	Combo.AddString(ID2W(IDS_WEEKSECOND));
	Combo.AddString(ID2W(IDS_WEEKTHIRD));
	Combo.AddString(ID2W(IDS_WEEKFOURTH));
	Combo.AddString(ID2W(IDS_WEEKLAST));
	
	// Days combo
	Combo.AttachToDlgItem(*this,IDC_MDAYS);
	Combo.AddString(ID2W(IDS_MONDAY));
	Combo.AddString(ID2W(IDS_TUESDAY));
	Combo.AddString(ID2W(IDS_WEDNESDAY));
	Combo.AddString(ID2W(IDS_THURSDAY));
	Combo.AddString(ID2W(IDS_FRIDAY));
	Combo.AddString(ID2W(IDS_SATURDAY));
	Combo.AddString(ID2W(IDS_SUNDAY));

	// Time control
	SYSTEMTIME st;
	GetLocalTime(&st);
	st.wHour=m_pSchedule->m_tStartTime.bHour;
	st.wMinute=m_pSchedule->m_tStartTime.bMinute;
	st.wSecond=m_pSchedule->m_tStartTime.bSecond;
	CDateTimeCtrl TimeCtrl(GetDlgItem(IDC_TIME));
	TimeCtrl.SetSystemtime(GDT_VALID,&st);

	
	// Setting Every spin (and maybe other things)
	CSpinButtonCtrl SpinControl(GetDlgItem(IDC_EVERYSPIN));
	SpinControl.SetRange(1,32000);
	SpinControl.SetBuddy(GetDlgItem(IDC_EVERY));
	switch (m_pSchedule->m_nType)
	{
	case CSchedule::typeMinutely:
		SpinControl.SetPos(m_pSchedule->m_tMinutely.wEvery);
		break;
	case CSchedule::typeHourly:
		SpinControl.SetPos(m_pSchedule->m_tHourly.wEvery);
		break;
	case CSchedule::typeDaily:
		SpinControl.SetPos(m_pSchedule->m_tDaily.wEvery);
		break;
	case CSchedule::typeWeekly:
		SpinControl.SetPos(m_pSchedule->m_tWeekly.wEvery);
		if (m_pSchedule->m_tWeekly.bDays&CSchedule::SWEEKLYTYPE::Monday)
			CheckDlgButton(IDC_MON,BST_CHECKED);
		if (m_pSchedule->m_tWeekly.bDays&CSchedule::SWEEKLYTYPE::Tuesday)
			CheckDlgButton(IDC_TUE,BST_CHECKED);
		if (m_pSchedule->m_tWeekly.bDays&CSchedule::SWEEKLYTYPE::Wednesday)
			CheckDlgButton(IDC_WED,BST_CHECKED);
		if (m_pSchedule->m_tWeekly.bDays&CSchedule::SWEEKLYTYPE::Thursday)
			CheckDlgButton(IDC_THU,BST_CHECKED);
		if (m_pSchedule->m_tWeekly.bDays&CSchedule::SWEEKLYTYPE::Friday)
			CheckDlgButton(IDC_FRI,BST_CHECKED);
		if (m_pSchedule->m_tWeekly.bDays&CSchedule::SWEEKLYTYPE::Saturday)
			CheckDlgButton(IDC_SAT,BST_CHECKED);
		if (m_pSchedule->m_tWeekly.bDays&CSchedule::SWEEKLYTYPE::Sunday)
			CheckDlgButton(IDC_SUN,BST_CHECKED);
		break;
	default:
        SpinControl.SetPos(1);
		break;
	}

	// "Every Month" spin
	SpinControl.AttachToDlgItem(*this,IDC_MSPIN);
	SpinControl.SetRange(1,31);
	SpinControl.SetBuddy(GetDlgItem(IDC_MEVERY));
	if (m_pSchedule->m_nType==CSchedule::typeMonthly)
	{
		if (m_pSchedule->m_tMonthly.nType==CSchedule::SMONTHLYTYPE::Day)
		{
			CheckDlgButton(IDC_MDAY,BST_CHECKED);
			OnCommand(IDC_MDAY,CBN_SELCHANGE,NULL);
			SpinControl.SetPos(m_pSchedule->m_tMonthly.bDay);
			SendDlgItemMessage(IDC_MTYPE,CB_SETCURSEL,0,0);
			SendDlgItemMessage(IDC_MDAYS,CB_SETCURSEL,0,0);
		}
		else
		{
			CheckDlgButton(IDC_MTHE,BST_CHECKED);
			OnCommand(IDC_MTHE,CBN_SELCHANGE,NULL);
			SendDlgItemMessage(IDC_MTYPE,CB_SETCURSEL,m_pSchedule->m_tMonthly.nWeek,0);
			SendDlgItemMessage(IDC_MDAYS,CB_SETCURSEL,m_pSchedule->m_tMonthly.bDay,0);
			SpinControl.SetPos(1);
		}	
	}
	else
	{
		CheckDlgButton(IDC_MDAY,BST_CHECKED);
		OnCommand(IDC_MDAY,CBN_SELCHANGE,NULL);
		SpinControl.SetPos(1);
		SendDlgItemMessage(IDC_MTYPE,CB_SETCURSEL,0,0);
		SendDlgItemMessage(IDC_MDAYS,CB_SETCURSEL,0,0);
	}
	


	// Setting minute spin
	SpinControl.AttachToDlgItem(*this,IDC_MINUTESPIN);
	SpinControl.SetRange(0,59);
	SpinControl.SetBuddy(GetDlgItem(IDC_MINUTEONHOUR));
	SpinControl.SetPos(m_pSchedule->m_nType==CSchedule::typeHourly?
		m_pSchedule->m_tHourly.wMinute:0);
		
	
	// Once time control
	if (m_pSchedule->m_nType==CSchedule::typeOnce)
	{
		GetLocalTime(&st);
		st.wYear=m_pSchedule->m_dStartDate.wYear;
		st.wMonth=m_pSchedule->m_dStartDate.bMonth;
		st.wDay=m_pSchedule->m_dStartDate.bDay;
		CDateTimeCtrl DateCtrl(GetDlgItem(IDC_ONCETIME));
		DateCtrl.SetSystemtime(GDT_VALID,&st);
	}
	
	
	
	// Enabled, "delete after run" and "at this time only"	
	if (m_pSchedule->m_bFlags&CSchedule::flagEnabled)
		CheckDlgButton(IDC_ENABLED,BST_CHECKED);
	if (m_pSchedule->m_bFlags&CSchedule::flagDeleteAfterRun)
		CheckDlgButton(IDC_DELETEAFTERRUN,BST_CHECKED);
	if (!(m_pSchedule->m_bFlags&CSchedule::flagAtThisTime))
		CheckDlgButton(IDC_UPDATEWHENPOSSIBLE,BST_CHECKED);

	
	// CPU usage
	SpinControl.AttachToDlgItem(*this,IDC_CPUUSAGESPIN);
	SpinControl.SetRange(0,100);
	SpinControl.SetBuddy(GetDlgItem(IDC_CPUUSAGE));
	if (m_pSchedule->m_wCpuUsageTheshold!=WORD(-1))
	{
		CheckDlgButton(IDC_CPUUSAGECHECK,TRUE);
		SpinControl.SetPos(m_pSchedule->m_wCpuUsageTheshold);
	}
	else
	{
		EnableDlgItem(IDC_CPUUSAGE,FALSE);
		EnableDlgItem(IDC_CPUUSAGESPIN,FALSE);
		EnableDlgItem(IDC_CPUUSAGEEXTRALABEL,FALSE);
		SpinControl.SetPos(100);
	}


	// Thread priority
	Combo.AttachToDlgItem(*this,IDC_THREADPRIORITY);
	Combo.AddString(ID2W(IDS_PRIORITYHIGH));
	Combo.AddString(ID2W(IDS_PRIORITYABOVENORMAL));
	Combo.AddString(ID2W(IDS_PRIORITYNORMAL));
	Combo.AddString(ID2W(IDS_PRIORITYBELOWNORMAL));
	Combo.AddString(ID2W(IDS_PRIORITYLOW));
	Combo.AddString(ID2W(IDS_PRIORITYIDLE));
	if (GetSystemFeaturesFlag()&efWinVista)
		Combo.AddString(ID2W(IDS_PRIORITYBACKGROUND));


	switch (m_pSchedule->m_nThreadPriority)
	{
	case THREAD_PRIORITY_HIGHEST:
		Combo.SetCurSel(0);
		break;
	case THREAD_PRIORITY_ABOVE_NORMAL:
		Combo.SetCurSel(1);
		break;
	case THREAD_PRIORITY_NORMAL:
		Combo.SetCurSel(2);
		break;
	case THREAD_PRIORITY_BELOW_NORMAL:
		Combo.SetCurSel(3);
		break;
	case THREAD_PRIORITY_LOWEST:
		Combo.SetCurSel(4);
		break;
	case THREAD_PRIORITY_IDLE:
		Combo.SetCurSel(5);
		break;
	case THREAD_MODE_BACKGROUND_BEGIN:
		Combo.SetCurSel(6);
		break;
	default:
		Combo.SetCurSel(2);
		break;
	}
		


	// Last run text
	CStringW LastRun;
	if (m_pSchedule->m_bFlags&CSchedule::flagRunned &&
		!(m_pSchedule->m_tLastStartTime.bHour==0 && m_pSchedule->m_tLastStartTime.bMinute==0 && 
		m_pSchedule->m_tLastStartTime.bSecond==0 && m_pSchedule->m_tLastStartDate.wYear<1995 && 
		m_pSchedule->m_tLastStartDate.bMonth==0 && m_pSchedule->m_tLastStartDate.bDay==0))
	{
		st.wYear=m_pSchedule->m_tLastStartDate.wYear;
		st.wMonth=m_pSchedule->m_tLastStartDate.bMonth;
		st.wDay=m_pSchedule->m_tLastStartDate.bDay;
		st.wHour=m_pSchedule->m_tLastStartTime.bHour;
		st.wMinute=m_pSchedule->m_tLastStartTime.bMinute;
		st.wSecond=m_pSchedule->m_tLastStartTime.bSecond;
		st.wMilliseconds=0;
			
		if (IsUnicodeSystem())
		{
			WCHAR szDate[100],szTime[100];
			GetTimeFormatW(LOCALE_USER_DEFAULT,0,&st,NULL,szTime,100);
			GetDateFormatW(LOCALE_USER_DEFAULT,DATE_LONGDATE,&st,NULL,szDate,100);
			LastRun.FormatEx(IDS_LASTRUN,szDate,szTime);
		}
		else
		{
			char szDate[100],szTime[100];
			GetTimeFormat(LOCALE_USER_DEFAULT,0,&st,NULL,szTime,100);
			GetDateFormat(LOCALE_USER_DEFAULT,DATE_LONGDATE,&st,NULL,szDate,100);
			LastRun.FormatEx(IDS_LASTRUN,(LPCWSTR)A2W(szDate),(LPCWSTR)A2W(szTime));
		}
	}
	else
		LastRun.LoadString(IDS_LASTRUNNEVER);
	
/*#ifdef _DEBUG
	CStringW txt2;
	txt2.Format(L" F:%X STD:%d.%d.%d STT: %d:%d:",m_pSchedule->m_bFlags,
		m_pSchedule->m_tLastStartDate.bDay,m_pSchedule->m_tLastStartDate.bMonth,m_pSchedule->m_tLastStartDate.wYear,
		m_pSchedule->m_tLastStartTime.bHour,m_pSchedule->m_tLastStartTime.bMinute,m_pSchedule->m_tLastStartTime.bSecond);
	LastRun << txt2;
#endif*/

	SetDlgItemText(IDC_LASTRUN,LastRun);
	
	
	OnTypeChanged();
		
	m_bChanged=FALSE;
	return CDialog::OnInitDialog(hwndFocus);
}


void CSettingsProperties::CAutoUpdateSettingsPage::CCheduledUpdateDlg::OnHelp(LPHELPINFO lphi)
{
	if (HtmlHelp(HH_HELP_CONTEXT,HELP_SETTINGS_AUTOUPDATEDLG)==NULL)
		HtmlHelp(HH_DISPLAY_TOPIC,0);
}


BOOL CSettingsProperties::CAutoUpdateSettingsPage::CCheduledUpdateDlg::OnCommand(WORD wID,WORD wNotifyCode,HWND hControl)
{
	CDialog::OnCommand(wID,wNotifyCode,hControl);
	switch (wID)
	{
	case IDC_OK:
		OnOK();
		break;
	case IDCANCEL:
	case IDC_CANCEL:
		EndDialog(0);
		break;
	case IDC_TYPE:
		if (wNotifyCode==CBN_SELCHANGE)
			OnTypeChanged();
		break;
	case IDC_MDAY:
	case IDC_MTHE:
		CheckDlgButton(IDC_MDAY,wID==IDC_MDAY);
		CheckDlgButton(IDC_MTHE,wID==IDC_MTHE);
		
		EnableDlgItem(IDC_MEVERY,wID==IDC_MDAY);
		EnableDlgItem(IDC_MSPIN,wID==IDC_MDAY);
		EnableDlgItem(IDC_MTYPE,wID==IDC_MTHE);
		EnableDlgItem(IDC_MDAYS,wID==IDC_MTHE);
		m_bChanged=TRUE;
		break;
	case IDC_TIME:
	case IDC_MON:
	case IDC_TUE:
	case IDC_WED:
	case IDC_THU:
	case IDC_FRI:
	case IDC_SAT:
	case IDC_SUN:	
	case IDC_MTYPE:
	case IDC_MDAYS:
	case IDC_ONCETIME:
	case IDC_ENABLED:
	case IDC_DELETEAFTERRUN:
	case IDC_THREADPRIORITY:
		m_bChanged=TRUE;
		break;
	case IDC_DATABASES:
		OnDatabases();
		break;
	case IDC_CPUUSAGECHECK:
		{
			BOOL bEnable=IsDlgButtonChecked(IDC_CPUUSAGECHECK);
			EnableDlgItem(IDC_CPUUSAGE,bEnable);
			EnableDlgItem(IDC_CPUUSAGESPIN,bEnable);
			EnableDlgItem(IDC_CPUUSAGEEXTRALABEL,bEnable);
			
			SetFocus(IDC_CPUUSAGE);
			m_bChanged=TRUE;
			break;
		}
	case IDC_EVERY:
		if (wNotifyCode==EN_SETFOCUS)
			::SendMessage(hControl,EM_SETSEL,0,-1);
		else if (wNotifyCode==EN_CHANGE)
		{
			BOOL bTranslated=FALSE;
			UINT nVal=GetDlgItemInt(IDC_EVERY,&bTranslated,FALSE);
			if (nVal<1)
				SetDlgItemInt(IDC_EVERY,1,FALSE);
		
			m_bChanged=TRUE;
		}
		break;
	case IDC_MEVERY:
		if (wNotifyCode==EN_SETFOCUS)
			::SendMessage(hControl,EM_SETSEL,0,-1);
		else if (wNotifyCode==EN_CHANGE)
		{
			BOOL bTranslated=FALSE;
			UINT nVal=GetDlgItemInt(IDC_MEVERY,&bTranslated,FALSE);
			if (nVal<1 && bTranslated)
				SetDlgItemInt(IDC_MEVERY,1,FALSE);
			else if (nVal>31 && bTranslated)
				SetDlgItemInt(IDC_MEVERY,31,FALSE);

			m_bChanged=TRUE;
		}
		break;
	case IDC_MINUTEONHOUR:
		if (wNotifyCode==EN_SETFOCUS)
			::SendMessage(hControl,EM_SETSEL,0,-1);
		else if (wNotifyCode==EN_CHANGE)
		{
			BOOL bTranslated=FALSE;
			UINT nVal=GetDlgItemInt(IDC_MINUTEONHOUR,&bTranslated,FALSE);
			if (nVal>59  && bTranslated)
				SetDlgItemInt(IDC_MINUTEONHOUR,59,FALSE);
			m_bChanged=TRUE;
		}
		break;
	case IDC_CPUUSAGE:
		if (wNotifyCode==EN_SETFOCUS)
			::SendMessage(hControl,EM_SETSEL,0,-1);
		else if (wNotifyCode==EN_CHANGE)
		{
			BOOL bTranslated=FALSE;
			UINT nVal=GetDlgItemInt(IDC_CPUUSAGE,&bTranslated,FALSE);
			if (nVal>100 && bTranslated)
				SetDlgItemInt(IDC_CPUUSAGE,100,FALSE);
			m_bChanged=TRUE;
		}
		break;
	}
	return FALSE;
}

BOOL CSettingsProperties::CAutoUpdateSettingsPage::CCheduledUpdateDlg::OnDatabases()
{
	CArray<PDATABASE> aDatabases;
		
	CSelectDatabasesDlg dbd(GetLocateApp()->GetDatabases(),aDatabases,CSelectDatabasesDlg::flagShowThreads,
		CRegKey2::GetCommonKey()+"\\Dialogs\\SelectDatabases/Schedule");
	dbd.SelectDatabases(m_pSchedule->m_pDatabases);

	if (dbd.DoModal(*this))
	{
		if ((dbd.m_bFlags&CSelectDatabasesDlg::flagSelectedMask)==CSelectDatabasesDlg::flagLasestIsSelected)
			return TRUE;
		
		if ((dbd.m_bFlags&CSelectDatabasesDlg::flagSelectedMask)==CSelectDatabasesDlg::flagGlobalIsSelected)
		{
			if (m_pSchedule->m_pDatabases!=NULL)
			{
				delete[] m_pSchedule->m_pDatabases;
				m_pSchedule->m_pDatabases=NULL;
			}
		}
		else
		{
			if (m_pSchedule->m_pDatabases!=NULL)
				delete[] m_pSchedule->m_pDatabases;
			
			DWORD dwLength=1;
			int i=0;
			for (i=0;i<aDatabases.GetSize();i++)
			{
				// Space for name, '\\' (thread separator) and '\0'
				dwLength+=istrlenw(aDatabases[i]->GetName())+2;
				
				// Space for thread
				WCHAR szThread[10];
				_itow_s(aDatabases[i]->GetThreadId(),szThread,10,10);

				dwLength+=istrlenw(szThread);
			}

			m_pSchedule->m_pDatabases=new WCHAR[dwLength];
			LPWSTR pPtr=m_pSchedule->m_pDatabases;
			for (i=0;i<aDatabases.GetSize();i++)
			{
				int iStrlen=istrlenw(aDatabases[i]->GetName());
				MemCopyW(pPtr,aDatabases[i]->GetName(),iStrlen);
				pPtr+=iStrlen;

				*(pPtr++)=L'\\';

				// Space for thread
				WCHAR szThread[10];
				_itow_s(aDatabases[i]->GetThreadId(),szThread,10,10);
				iStrlen=istrlenw(szThread)+1;
				MemCopyW(pPtr,szThread,iStrlen);
				pPtr+=iStrlen;
			}
			*pPtr='\0';
		}

		m_bChanged=TRUE;
		return TRUE;
	}
	return FALSE;
}

BOOL CSettingsProperties::CAutoUpdateSettingsPage::CCheduledUpdateDlg::OnNotify(int idCtrl,LPNMHDR pnmh)
{
	switch (idCtrl)
	{
	case IDC_TIME:
	case IDC_ONCETIME:
		m_bChanged=TRUE;
		break;
	}
	return CDialog::OnNotify(idCtrl,pnmh);
}

void CSettingsProperties::CAutoUpdateSettingsPage::CCheduledUpdateDlg::OnDestroy()
{
	if (m_pTypeCombo!=NULL)
	{
		delete m_pTypeCombo;
		m_pTypeCombo=NULL;
	}
	CDialog::OnDestroy();
}

BOOL CSettingsProperties::CAutoUpdateSettingsPage::CCheduledUpdateDlg::OnClose()
{
	CDialog::OnClose();
	EndDialog(0);
	return 0;
}

BOOL CSettingsProperties::CAutoUpdateSettingsPage::CCheduledUpdateDlg::OnOK()
{
	BOOL bTranslated=FALSE;

	CSchedule::ScheduleType nType[]={
		CSchedule::typeMinutely,
		CSchedule::typeHourly,
		CSchedule::typeDaily,
		CSchedule::typeWeekly,
		CSchedule::typeMonthly,
		CSchedule::typeOnce,
		CSchedule::typeAtStartup,
	};
	CSchedule::ScheduleType nNewType=nType[m_pTypeCombo->GetCurSel()];
	if (m_pSchedule->m_nType!=nNewType)
	{
		m_bChanged=TRUE;
		m_pSchedule->m_nType=nNewType;
	}
	
	if (m_bChanged)
	{
		if (m_pSchedule->m_nType!=CSchedule::typeAtStartup)
			m_pSchedule->m_bFlags&=~CSchedule::flagRunned;
		CSchedule::GetCurrentDateAndTime(&m_pSchedule->m_tLastStartDate,&m_pSchedule->m_tLastStartTime);
	}

	if (IsDlgButtonChecked(IDC_ENABLED))
		m_pSchedule->m_bFlags|=CSchedule::flagEnabled;
	else
		m_pSchedule->m_bFlags&=~CSchedule::flagEnabled;
	if (IsDlgButtonChecked(IDC_DELETEAFTERRUN))
		m_pSchedule->m_bFlags|=CSchedule::flagDeleteAfterRun;
	else
		m_pSchedule->m_bFlags&=~CSchedule::flagDeleteAfterRun;
	if (IsDlgButtonChecked(IDC_UPDATEWHENPOSSIBLE))
		m_pSchedule->m_bFlags&=~CSchedule::flagAtThisTime;
	else
		m_pSchedule->m_bFlags|=CSchedule::flagAtThisTime;

	
	SYSTEMTIME st;
	CDateTimeCtrl TimeCtrl(GetDlgItem(IDC_TIME));
	TimeCtrl.GetSystemtime(&st);
	m_pSchedule->m_tStartTime=st;
	
	switch (m_pSchedule->m_nType)
	{
	case CSchedule::typeMinutely:
		m_pSchedule->m_tMinutely.wEvery=GetDlgItemInt(IDC_EVERY,&bTranslated,FALSE);
		if (!bTranslated || (int)m_pSchedule->m_tMinutely.wEvery<1)
			m_pSchedule->m_tMinutely.wEvery=1;
		break;
	case CSchedule::typeHourly:
		m_pSchedule->m_tHourly.wEvery=GetDlgItemInt(IDC_EVERY,&bTranslated,FALSE);
		if (!bTranslated || (int)m_pSchedule->m_tHourly.wEvery<1)
			m_pSchedule->m_tHourly.wEvery=1;
		
		m_pSchedule->m_tHourly.wMinute=GetDlgItemInt(IDC_MINUTEONHOUR,&bTranslated,FALSE);
		if (!bTranslated)
			m_pSchedule->m_tHourly.wMinute=0;
		else if (m_pSchedule->m_tHourly.wMinute>59)
			m_pSchedule->m_tHourly.wMinute=59;
		break;		
	case CSchedule::typeDaily:
		m_pSchedule->m_tDaily.wEvery=GetDlgItemInt(IDC_EVERY,&bTranslated,FALSE);
		if (!bTranslated || (int)m_pSchedule->m_tDaily.wEvery<1)
			m_pSchedule->m_tDaily.wEvery=1;
		break;
	case CSchedule::typeWeekly:
		m_pSchedule->m_tWeekly.wEvery=GetDlgItemInt(IDC_EVERY,&bTranslated,FALSE);
		if (!bTranslated || (int)m_pSchedule->m_tWeekly.wEvery<1)
			m_pSchedule->m_tWeekly.wEvery=1;
		m_pSchedule->m_tWeekly.bDays=0;
		if (IsDlgButtonChecked(IDC_MON))
			m_pSchedule->m_tWeekly.bDays|=CSchedule::SWEEKLYTYPE::Monday;
		if (IsDlgButtonChecked(IDC_TUE))
			m_pSchedule->m_tWeekly.bDays|=CSchedule::SWEEKLYTYPE::Tuesday;
		if (IsDlgButtonChecked(IDC_WED))
			m_pSchedule->m_tWeekly.bDays|=CSchedule::SWEEKLYTYPE::Wednesday;
		if (IsDlgButtonChecked(IDC_THU))
			m_pSchedule->m_tWeekly.bDays|=CSchedule::SWEEKLYTYPE::Thursday;
		if (IsDlgButtonChecked(IDC_FRI))
			m_pSchedule->m_tWeekly.bDays|=CSchedule::SWEEKLYTYPE::Friday;
		if (IsDlgButtonChecked(IDC_SAT))
			m_pSchedule->m_tWeekly.bDays|=CSchedule::SWEEKLYTYPE::Saturday;
		if (IsDlgButtonChecked(IDC_SUN))
			m_pSchedule->m_tWeekly.bDays|=CSchedule::SWEEKLYTYPE::Sunday;
		break;
	case CSchedule::typeMonthly:
		if (IsDlgButtonChecked(IDC_MDAY))
		{
			m_pSchedule->m_tMonthly.nType=CSchedule::SMONTHLYTYPE::Day;
			m_pSchedule->m_tMonthly.bDay=GetDlgItemInt(IDC_MEVERY,&bTranslated,FALSE);
			if (!bTranslated || (int)m_pSchedule->m_tMonthly.bDay<1)
				m_pSchedule->m_tMonthly.bDay=1;
		}
		else
		{
			m_pSchedule->m_tMonthly.nType=CSchedule::SMONTHLYTYPE::WeekDay;
			m_pSchedule->m_tMonthly.nWeek=(CSchedule::SMONTHLYTYPE::Week)SendDlgItemMessage(IDC_MTYPE,CB_GETCURSEL);
			m_pSchedule->m_tMonthly.bDay=(BYTE)SendDlgItemMessage(IDC_MDAYS,CB_GETCURSEL);
		}
		break;
	case CSchedule::typeOnce:
		{
			CDateTimeCtrl DateCtrl(GetDlgItem(IDC_ONCETIME));
			SYSTEMTIME st;
			DateCtrl.GetSystemtime(&st);
			m_pSchedule->m_dStartDate=st;
			break;
		}
	case CSchedule::typeAtStartup:
		break;
	}

	if (IsDlgButtonChecked(IDC_CPUUSAGECHECK))
	{
		m_pSchedule->m_wCpuUsageTheshold=GetDlgItemInt(IDC_CPUUSAGE,&bTranslated,FALSE);
		if (!bTranslated)
			m_pSchedule->m_wCpuUsageTheshold=WORD(-1);
	}
	else
		m_pSchedule->m_wCpuUsageTheshold=WORD(-1);


	switch (SendDlgItemMessage(IDC_THREADPRIORITY,CB_GETCURSEL))
	{
	case 0:
		m_pSchedule->m_nThreadPriority=THREAD_PRIORITY_HIGHEST;
        break;
	case 1:
        m_pSchedule->m_nThreadPriority=THREAD_PRIORITY_ABOVE_NORMAL;
        break;
	case 3:
		m_pSchedule->m_nThreadPriority=THREAD_PRIORITY_BELOW_NORMAL;
        break;
	case 4:
        m_pSchedule->m_nThreadPriority=THREAD_PRIORITY_LOWEST;
        break;
	case 5:
        m_pSchedule->m_nThreadPriority=THREAD_PRIORITY_IDLE;
        break;
	case 6:
		m_pSchedule->m_nThreadPriority=THREAD_MODE_BACKGROUND_BEGIN;
        break;
	default:
		m_pSchedule->m_nThreadPriority=THREAD_PRIORITY_NORMAL;
        break;
	}

	EndDialog(1);
	return TRUE;
}
	
BOOL CSettingsProperties::CAutoUpdateSettingsPage::CCheduledUpdateDlg::OnTypeChanged()
{
	CStringW Title;
	CStringW txt;
	
	CSchedule::ScheduleType nTypes[]={
		CSchedule::typeMinutely,
		CSchedule::typeHourly,
		CSchedule::typeDaily,
		CSchedule::typeWeekly,
		CSchedule::typeMonthly,
		CSchedule::typeOnce,
		CSchedule::typeAtStartup,
	};
		
	CSchedule::ScheduleType nType=nTypes[m_pTypeCombo->GetCurSel()];

	EnableDlgItem(IDC_TIME,nType!=CSchedule::typeMinutely && 
		nType!=CSchedule::typeHourly && nType!=CSchedule::typeAtStartup);
	EnableDlgItem(IDC_UPDATEWHENPOSSIBLE,nType!=CSchedule::typeMinutely && nType!=CSchedule::typeAtStartup);
	
    switch (nType)
	{
	case CSchedule::typeMinutely:
		txt.LoadString(IDS_MINUTELY);
		ShowDlgItem(IDC_EVERYTXT,swShow);
		ShowDlgItem(IDC_EVERY,swShow);
		ShowDlgItem(IDC_EVERYSPIN,swShow);
		ShowDlgItem(IDC_MINUTEONHOUR,swHide);
		ShowDlgItem(IDC_MINUTEONHOURLBL,swHide);
		ShowDlgItem(IDC_MINUTESPIN,swHide);
		ShowDlgItem(IDC_MINUTE,swShow);
		ShowDlgItem(IDC_DAYS,swHide);
		ShowDlgItem(IDC_HOURS,swHide);
		ShowDlgItem(IDC_WEEKS,swHide);
		ShowDlgItem(IDC_MON,swHide);
		ShowDlgItem(IDC_TUE,swHide);
		ShowDlgItem(IDC_WED,swHide);
		ShowDlgItem(IDC_THU,swHide);
		ShowDlgItem(IDC_FRI,swHide);
		ShowDlgItem(IDC_SAT,swHide);
		ShowDlgItem(IDC_SUN,swHide);
		ShowDlgItem(IDC_MDAY,swHide);
		ShowDlgItem(IDC_MTHE,swHide);
		ShowDlgItem(IDC_MEVERY,swHide);
		ShowDlgItem(IDC_MSPIN,swHide);
		ShowDlgItem(IDC_OFTHEMONTHS,swHide);
		ShowDlgItem(IDC_OFTHEMONTHS2,swHide);
		ShowDlgItem(IDC_MTYPE,swHide);
		ShowDlgItem(IDC_MDAYS,swHide);
		ShowDlgItem(IDC_ONCETIME,swHide);
		ShowDlgItem(IDC_RUNON,swHide);
		::SetWindowPos(GetDlgItem(IDC_FRAME),HWND_TOP,0,0,328,50,SWP_SHOWWINDOW|SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
		break;
	case CSchedule::typeHourly:
		txt.LoadString(IDS_HOURLY);
		ShowDlgItem(IDC_EVERYTXT,swShow);
		ShowDlgItem(IDC_EVERY,swShow);
		ShowDlgItem(IDC_EVERYSPIN,swShow);
		ShowDlgItem(IDC_MINUTEONHOUR,swShow);
		ShowDlgItem(IDC_MINUTEONHOURLBL,swShow);
		ShowDlgItem(IDC_MINUTESPIN,swShow); 
		ShowDlgItem(IDC_MINUTE,swHide);
		ShowDlgItem(IDC_DAYS,swHide);
		ShowDlgItem(IDC_HOURS,swShow);
		ShowDlgItem(IDC_WEEKS,swHide);
		ShowDlgItem(IDC_MON,swHide);
		ShowDlgItem(IDC_TUE,swHide);
		ShowDlgItem(IDC_WED,swHide);
		ShowDlgItem(IDC_THU,swHide);
		ShowDlgItem(IDC_FRI,swHide);
		ShowDlgItem(IDC_SAT,swHide);
		ShowDlgItem(IDC_SUN,swHide);
		ShowDlgItem(IDC_MDAY,swHide);
		ShowDlgItem(IDC_MTHE,swHide);
		ShowDlgItem(IDC_MEVERY,swHide);
		ShowDlgItem(IDC_MSPIN,swHide);
		ShowDlgItem(IDC_OFTHEMONTHS,swHide);
		ShowDlgItem(IDC_OFTHEMONTHS2,swHide);
		ShowDlgItem(IDC_MTYPE,swHide);
		ShowDlgItem(IDC_MDAYS,swHide);
		ShowDlgItem(IDC_ONCETIME,swHide);
		ShowDlgItem(IDC_RUNON,swHide);
		::SetWindowPos(GetDlgItem(IDC_FRAME),HWND_TOP,0,0,328,50,SWP_SHOWWINDOW|SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
		break;
	case CSchedule::typeDaily:
		txt.LoadString(IDS_DAILY);
		ShowDlgItem(IDC_EVERYTXT,swShow);
		ShowDlgItem(IDC_EVERY,swShow);
		ShowDlgItem(IDC_EVERYSPIN,swShow);
		ShowDlgItem(IDC_MINUTEONHOUR,swHide);
		ShowDlgItem(IDC_MINUTEONHOURLBL,swHide);
		ShowDlgItem(IDC_MINUTESPIN,swHide);
		ShowDlgItem(IDC_MINUTE,swHide);
		ShowDlgItem(IDC_DAYS,swShow);
		ShowDlgItem(IDC_HOURS,swHide);
		ShowDlgItem(IDC_WEEKS,swHide);
		ShowDlgItem(IDC_MON,swHide);
		ShowDlgItem(IDC_TUE,swHide);
		ShowDlgItem(IDC_WED,swHide);
		ShowDlgItem(IDC_THU,swHide);
		ShowDlgItem(IDC_FRI,swHide);
		ShowDlgItem(IDC_SAT,swHide);
		ShowDlgItem(IDC_SUN,swHide);
		ShowDlgItem(IDC_MDAY,swHide);
		ShowDlgItem(IDC_MTHE,swHide);
		ShowDlgItem(IDC_MEVERY,swHide);
		ShowDlgItem(IDC_MSPIN,swHide);
		ShowDlgItem(IDC_OFTHEMONTHS,swHide);
		ShowDlgItem(IDC_OFTHEMONTHS2,swHide);
		ShowDlgItem(IDC_MTYPE,swHide);
		ShowDlgItem(IDC_MDAYS,swHide);
		ShowDlgItem(IDC_ONCETIME,swHide);
		ShowDlgItem(IDC_RUNON,swHide);
		::SetWindowPos(GetDlgItem(IDC_FRAME),HWND_TOP,0,0,328,50,SWP_SHOWWINDOW|SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
		break;
	case CSchedule::typeWeekly:
		txt.LoadString(IDS_WEEKLY);
		ShowDlgItem(IDC_EVERYTXT,swShow);
		ShowDlgItem(IDC_EVERY,swShow);
		ShowDlgItem(IDC_EVERYSPIN,swShow);
		ShowDlgItem(IDC_MINUTEONHOUR,swHide);
		ShowDlgItem(IDC_MINUTEONHOURLBL,swHide);
		ShowDlgItem(IDC_MINUTESPIN,swHide);
		ShowDlgItem(IDC_MINUTE,swHide);
		ShowDlgItem(IDC_DAYS,swHide);
		ShowDlgItem(IDC_HOURS,swHide);
		ShowDlgItem(IDC_WEEKS,swShow);
		ShowDlgItem(IDC_MON,swShow);
		ShowDlgItem(IDC_TUE,swShow);
		ShowDlgItem(IDC_WED,swShow);
		ShowDlgItem(IDC_THU,swShow);
		ShowDlgItem(IDC_FRI,swShow);
		ShowDlgItem(IDC_SAT,swShow);
		ShowDlgItem(IDC_SUN,swShow);
		ShowDlgItem(IDC_MDAY,swHide);
		ShowDlgItem(IDC_MTHE,swHide);
		ShowDlgItem(IDC_MEVERY,swHide);
		ShowDlgItem(IDC_MSPIN,swHide);
		ShowDlgItem(IDC_OFTHEMONTHS,swHide);
		ShowDlgItem(IDC_OFTHEMONTHS2,swHide);
		ShowDlgItem(IDC_MTYPE,swHide);
		ShowDlgItem(IDC_MDAYS,swHide);
		ShowDlgItem(IDC_ONCETIME,swHide);
		ShowDlgItem(IDC_RUNON,swHide);
		::SetWindowPos(GetDlgItem(IDC_FRAME),HWND_TOP,0,0,328,115,SWP_SHOWWINDOW|SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
		break;
	case CSchedule::typeMonthly:
		txt.LoadString(IDS_MONTHLY);
		ShowDlgItem(IDC_EVERYTXT,swHide);
		ShowDlgItem(IDC_EVERY,swHide);
		ShowDlgItem(IDC_EVERYSPIN,swHide);
		ShowDlgItem(IDC_MINUTEONHOUR,swHide);
		ShowDlgItem(IDC_MINUTEONHOURLBL,swHide);
		ShowDlgItem(IDC_MINUTESPIN,swHide);
		ShowDlgItem(IDC_MINUTE,swHide);
		ShowDlgItem(IDC_DAYS,swHide);
		ShowDlgItem(IDC_HOURS,swHide);
		ShowDlgItem(IDC_WEEKS,swHide);
		ShowDlgItem(IDC_MON,swHide);
		ShowDlgItem(IDC_TUE,swHide);
		ShowDlgItem(IDC_WED,swHide);
		ShowDlgItem(IDC_THU,swHide);
		ShowDlgItem(IDC_FRI,swHide);
		ShowDlgItem(IDC_SAT,swHide);
		ShowDlgItem(IDC_SUN,swHide);
		ShowDlgItem(IDC_MDAY,swShow);
		ShowDlgItem(IDC_MTHE,swShow);
		ShowDlgItem(IDC_MEVERY,swShow);
		ShowDlgItem(IDC_MSPIN,swShow);
		ShowDlgItem(IDC_OFTHEMONTHS,swShow);
		ShowDlgItem(IDC_OFTHEMONTHS2,swShow);
		ShowDlgItem(IDC_MTYPE,swShow);
		ShowDlgItem(IDC_MDAYS,swShow);
		ShowDlgItem(IDC_ONCETIME,swHide);
		ShowDlgItem(IDC_RUNON,swHide);
		::SetWindowPos(GetDlgItem(IDC_FRAME),HWND_TOP,0,0,328,115,SWP_SHOWWINDOW|SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
		break;
	case CSchedule::typeOnce:
		txt.LoadString(IDS_ONCE);
		ShowDlgItem(IDC_EVERYTXT,swHide);
		ShowDlgItem(IDC_EVERY,swHide);
		ShowDlgItem(IDC_EVERYSPIN,swHide);
		ShowDlgItem(IDC_MINUTEONHOUR,swHide);
		ShowDlgItem(IDC_MINUTEONHOURLBL,swHide);
		ShowDlgItem(IDC_MINUTESPIN,swHide);
		ShowDlgItem(IDC_MINUTE,swHide);
		ShowDlgItem(IDC_DAYS,swHide);
		ShowDlgItem(IDC_HOURS,swHide);
		ShowDlgItem(IDC_WEEKS,swHide);
		ShowDlgItem(IDC_MON,swHide);
		ShowDlgItem(IDC_TUE,swHide);
		ShowDlgItem(IDC_WED,swHide);
		ShowDlgItem(IDC_THU,swHide);
		ShowDlgItem(IDC_FRI,swHide);
		ShowDlgItem(IDC_SAT,swHide);
		ShowDlgItem(IDC_SUN,swHide);
		ShowDlgItem(IDC_MDAY,swHide);
		ShowDlgItem(IDC_MTHE,swHide);
		ShowDlgItem(IDC_MEVERY,swHide);
		ShowDlgItem(IDC_MSPIN,swHide);
		ShowDlgItem(IDC_OFTHEMONTHS,swHide);
		ShowDlgItem(IDC_OFTHEMONTHS2,swHide);
		ShowDlgItem(IDC_MTYPE,swHide);
		ShowDlgItem(IDC_MDAYS,swHide);
		ShowDlgItem(IDC_ONCETIME,swShow);
		ShowDlgItem(IDC_RUNON,swShow);
		::SetWindowPos(GetDlgItem(IDC_FRAME),HWND_TOP,0,0,328,50,SWP_SHOWWINDOW|SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
		break;
	case CSchedule::typeAtStartup:
		txt.LoadString(IDS_ATSTARTUP);
		ShowDlgItem(IDC_EVERYTXT,swHide);
		ShowDlgItem(IDC_EVERY,swHide);
		ShowDlgItem(IDC_EVERYSPIN,swHide);
		ShowDlgItem(IDC_MINUTEONHOUR,swHide);
		ShowDlgItem(IDC_MINUTEONHOURLBL,swHide);
		ShowDlgItem(IDC_MINUTESPIN,swHide);
		ShowDlgItem(IDC_MINUTE,swHide);
		ShowDlgItem(IDC_DAYS,swHide);
		ShowDlgItem(IDC_HOURS,swHide);
		ShowDlgItem(IDC_WEEKS,swHide);
		ShowDlgItem(IDC_MON,swHide);
		ShowDlgItem(IDC_TUE,swHide);
		ShowDlgItem(IDC_WED,swHide);
		ShowDlgItem(IDC_THU,swHide);
		ShowDlgItem(IDC_FRI,swHide);
		ShowDlgItem(IDC_SAT,swHide);
		ShowDlgItem(IDC_SUN,swHide);
		ShowDlgItem(IDC_MDAY,swHide);
		ShowDlgItem(IDC_MTHE,swHide);
		ShowDlgItem(IDC_MEVERY,swHide);
		ShowDlgItem(IDC_MSPIN,swHide);
		ShowDlgItem(IDC_OFTHEMONTHS,swHide);
		ShowDlgItem(IDC_OFTHEMONTHS2,swHide);
		ShowDlgItem(IDC_MTYPE,swHide);
		ShowDlgItem(IDC_MDAYS,swHide);
		ShowDlgItem(IDC_ONCETIME,swHide);
		ShowDlgItem(IDC_RUNON,swHide);
		::SetWindowPos(GetDlgItem(IDC_FRAME),HWND_TOP,0,0,0,0,SWP_HIDEWINDOW|SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOSIZE);
		break;
	}
	Title.Format(IDS_SCHEDULEUPDATE,(LPCWSTR)txt);
	SetDlgItemText(IDC_FRAME,Title);
	return TRUE;
}

















		
////////////////////////////////////////
// CKeyboardShortcutsPage
////////////////////////////////////////



CSettingsProperties::CKeyboardShortcutsPage::CKeyboardShortcutsPage()
:	CPropertyPage(IDD_KEYBOARDSHORTCUTS,IDS_SHORTCUTSETTINGS),
	m_pList(NULL),m_pToolBar(NULL),m_ToolBarBitmaps(NULL),
	m_ToolBarBitmapsDisabled(NULL),m_ToolBarBitmapsHot(NULL),
	m_pCurrentShortcut(NULL)
{
	AddFlags(PSP_HASHELP);

	m_pPossibleControlsToActivate=CAction::GetPossibleControlValuesToActivate();
	m_pPossibleControlsToChange=CAction::GetPossibleControlValuesToChange();
	m_pPossibleMenuCommands=CAction::GetPossibleMenuCommands();
	m_pVirtualKeyNames=CShortcut::GetVirtualKeyNames();

	

	CLocateDlg* pLocateDlg=GetLocateDlg();
	if (pLocateDlg!=NULL)
	{
		bFreeDialogs=FALSE;
		hDialogs[0]=*pLocateDlg;
		hDialogs[1]=pLocateDlg->m_NameDlg;
		hDialogs[2]=pLocateDlg->m_SizeDateDlg;
		hDialogs[3]=pLocateDlg->m_AdvancedDlg;
		hDialogs[4]=NULL;
	}
	else
	{
		bFreeDialogs=TRUE;
		hDialogs[0]=CreateDialog(GetLanguageSpecificResourceHandle(),
			MAKEINTRESOURCE(IDD_MAIN),NULL,(DLGPROC)CLocateApp::DummyDialogProc);
		hDialogs[1]=CreateDialog(GetLanguageSpecificResourceHandle(),
			MAKEINTRESOURCE(IDD_NAME),hDialogs[0],(DLGPROC)CLocateApp::DummyDialogProc);
		hDialogs[2]=CreateDialog(GetLanguageSpecificResourceHandle(),
			MAKEINTRESOURCE(IDD_SIZEDATE),hDialogs[0],(DLGPROC)CLocateApp::DummyDialogProc);
		hDialogs[3]=CreateDialog(GetLanguageSpecificResourceHandle(),
			MAKEINTRESOURCE(IDD_ADVANCED),hDialogs[0],(DLGPROC)CLocateApp::DummyDialogProc);
		hDialogs[4]=NULL;
	}

	hMainMenu=::LoadMenu(IDR_MAINMENU);
	hPopupMenu=::LoadMenu(IDR_POPUPMENU);
}

CSettingsProperties::CKeyboardShortcutsPage::~CKeyboardShortcutsPage()
{

	delete[] m_pPossibleControlsToActivate;
	delete[] m_pPossibleControlsToChange;
	delete[] m_pPossibleMenuCommands;
	for (int i=0;m_pVirtualKeyNames[i].bKey!=0;i++)
		delete[] m_pVirtualKeyNames[i].pName;
	delete[] m_pVirtualKeyNames;

	m_aPossiblePresets.RemoveAll();

	if (bFreeDialogs)
	{
		for (int i=0;hDialogs[i]!=NULL;i++)
            ::DestroyWindow(hDialogs[i]);
	}

	::DestroyMenu(hMainMenu);
	::DestroyMenu(hPopupMenu);
}

BOOL CSettingsProperties::CKeyboardShortcutsPage::OnInitDialog(HWND hwndFocus)
{
	CPropertyPage::OnInitDialog(hwndFocus);

	m_pList=new CListCtrl(GetDlgItem(IDC_KEYLIST));

	m_pList->InsertColumn(0,ID2W(IDS_SHORTCUTLISTLABELSHORTCUT),LVCFMT_LEFT,80);
	m_pList->InsertColumn(1,ID2W(IDS_SHORTCUTLISTLABELTYPE),LVCFMT_LEFT,60);
	m_pList->InsertColumn(2,ID2W(IDS_SHORTCUTLISTLABELACTION),LVCFMT_LEFT,200);
	
	m_pList->SetExtendedListViewStyle(LVS_EX_HEADERDRAGDROP|LVS_EX_FULLROWSELECT ,LVS_EX_HEADERDRAGDROP|LVS_EX_FULLROWSELECT );
	m_pList->LoadColumnsState(HKCU,CRegKey2::GetCommonKey()+"\\Dialogs","Shortcuts Settings List Widths");

	m_ToolBarBitmaps.Create(IDB_SHORTCUTACTIONSBITMAPS,15,0,RGB(255,255,255));
	m_ToolBarBitmapsHot.Create(IDB_SHORTCUTACTIONSBITMAPSH,15,0,RGB(255,255,255));
	m_ToolBarBitmapsDisabled.Create(IDB_SHORTCUTACTIONSBITMAPSD,15,0,RGB(255,255,255));

	m_pToolBar=new CToolBarCtrl(GetDlgItem(IDC_ACTIONTOOLBAR));
	m_pToolBar->SetImageList(m_ToolBarBitmaps);
	m_pToolBar->SetDisabledImageList(m_ToolBarBitmapsDisabled);
	m_pToolBar->SetHotImageList(m_ToolBarBitmapsHot);
	
	TBBUTTON toolbuttons[]={
#pragma warning (disable :4305 4309)
		{0,IDC_ADDACTION,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0},
		{1,IDC_REMOVEACTION,0,TBSTYLE_BUTTON,0,0,0,0},
		{2,IDC_PREV,0,TBSTYLE_BUTTON,0,0,0,0},
		{3,IDC_NEXT,0,TBSTYLE_BUTTON,0,0,0,0},
		{4,IDC_SWAPWITHPREVIOUS,0,TBSTYLE_BUTTON,0,0,0,0},
		{5,IDC_SWAPWITHNEXT,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0}
#pragma warning (default :4305 4309)
	};
	m_pToolBar->AddButtons(6,toolbuttons);
	m_pToolBar->SetWindowPos(HWND_TOP,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);

	
	RECT rc;
	m_pWhenPressedList=new CListCtrl(GetDlgItem(IDC_WHENPRESSED));
	m_pWhenPressedList->GetClientRect(&rc);
	m_pWhenPressedList->InsertColumn(0,"",LVCFMT_LEFT,rc.right-GetSystemMetrics(SM_CXVSCROLL));
	m_pWhenPressedList->SetExtendedListViewStyle(LVS_EX_CHECKBOXES,LVS_EX_CHECKBOXES);
	if (IsUnicodeSystem())
		m_pWhenPressedList->SetUnicodeFormat(TRUE);
	
	m_pWhenPressedList->InsertItem(LVIF_TEXT|LVIF_PARAM,0,ID2W(IDS_SHORTCUTWHENFOCUSINRESULTLIST),0,0,0,CShortcut::wpFocusInResultList);
	m_pWhenPressedList->InsertItem(LVIF_TEXT|LVIF_PARAM,1,ID2W(IDS_SHORTCUTWHENFOCUSNOTINRESULTLIST),0,0,0,CShortcut::wpFocusNotInResultList);
	m_pWhenPressedList->InsertItem(LVIF_TEXT|LVIF_PARAM,2,ID2W(IDS_SHORTCUTWHENNAMETABSHOWN),0,0,0,CShortcut::wpNameTabShown);
	m_pWhenPressedList->InsertItem(LVIF_TEXT|LVIF_PARAM,3,ID2W(IDS_SHORTCUTWHENSIZEDATETABSHOWN),0,0,0,CShortcut::wpSizeDateTabShown);
	m_pWhenPressedList->InsertItem(LVIF_TEXT|LVIF_PARAM,4,ID2W(IDS_SHORTCUTWHENADVANCEDTABSHOWN),0,0,0,CShortcut::wpAdvancedTabShown);
	m_pWhenPressedList->InsertItem(LVIF_TEXT|LVIF_PARAM,5,ID2W(IDS_SHORTCUTWHENIFISRUNNING),0,0,0,CShortcut::wpDisableWhenISRunning|CShortcut::wpInvert);
	m_pWhenPressedList->InsertItem(LVIF_TEXT|LVIF_PARAM,6,ID2W(IDS_SHORTCUTWHENIFISNOTRUNNING),0,0,0,CShortcut::wpDisableWhenISNotRunning|CShortcut::wpInvert);
	
	
	// Check wheter Advanced items should be added
	m_ActionCombo.AttachToDlgItem(*this,IDC_ACTION);
	m_SubActionCombo.AttachToDlgItem(*this,IDC_SUBACTION);
	m_VerbCombo.AttachToDlgItem(*this,IDC_VERB);
	m_WhichFileCombo.AttachToDlgItem(*this,IDC_WHICHFILE);
	m_ContextMenuForCombo.AttachToDlgItem(*this,IDC_CONTEXTMENUFOR);

	// Insert action categories
	m_ActionCombo.AddString(ID2W(IDS_NONE));
	m_ActionCombo.AddString(ID2W(IDS_ACTIONCATACTIVATECONTROL));
	m_ActionCombo.AddString(ID2W(IDS_ACTIONCATACTIVATETAB));
	m_ActionCombo.AddString(ID2W(IDS_ACTIONCATMENUCOMMAND));
	m_ActionCombo.AddString(ID2W(IDS_ACTIONCATSHOWHIDEDIALOG));
	m_ActionCombo.AddString(ID2W(IDS_ACTIONCATTRESULTLIST));
	m_ActionCombo.AddString(ID2W(IDS_ACTIONCATMISC));
	m_ActionCombo.AddString(ID2W(IDS_ACTIONCATCHANGEVALUE));
	m_ActionCombo.AddString(ID2W(IDS_ACTIONCATPRESETS));
	m_ActionCombo.AddString(ID2W(IDS_ACTIONCATHELP));
	m_ActionCombo.AddString(ID2W(IDS_ACTIONCATSETTINGS));

	m_VerbCombo.AddString(ID2W(IDS_DEFAULT));

	// Insert "next/prev file"s
	m_WhichFileCombo.AddString(ID2W(IDS_ACTIONRESITEMNEXTFILE));
	m_WhichFileCombo.AddString(ID2W(IDS_ACTIONRESITEMPREVFILE));
	m_WhichFileCombo.AddString(ID2W(IDS_ACTIONRESITEMNEXTNONDELETEDFILE));
	m_WhichFileCombo.AddString(ID2W(IDS_ACTIONRESITEMPREVNONDELETEDFILE));

	// Insert "file/folder"/"Parent"
	m_ContextMenuForCombo.AddString(ID2W(IDS_ACTIONCONTEXTMENUFORFILE));
	m_ContextMenuForCombo.AddString(ID2W(IDS_ACTIONCONTEXTMENUFORPARENT));
	
	// Insert verbs
	m_VerbCombo.AddString("open");
	m_VerbCombo.AddString("edit");
	m_VerbCombo.AddString("explore");
	m_VerbCombo.AddString("find");
	m_VerbCombo.AddString("print");

	// Item spin
	SendDlgItemMessage(IDC_ITEMSPIN,UDM_SETRANGE32,1,0xFFFFFFFF>>1);
	SendDlgItemMessage(IDC_ITEMSPIN,UDM_SETBUDDY,WPARAM(GetDlgItem(IDC_ITEM)),0);

	// Enumerate presets
	CRegKey2 RegKey;
	if (RegKey.OpenKey(HKCU,"\\Dialogs\\SearchPresets",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
	{
		CRegKey PresetKey;
		CComboBox Combo(GetDlgItem(IDC_PRESETS));
		char szBuffer[30];

		for (int nPreset=0;nPreset<1000;nPreset++)
		{
			StringCbPrintf(szBuffer,30,"Preset %03d",nPreset);
	
			if (PresetKey.OpenKey(RegKey,szBuffer,CRegKey::openExist|CRegKey::samRead)!=ERROR_SUCCESS)
				break;
	
			DWORD dwLength=PresetKey.QueryValueLength("");
			if (dwLength>0)
			{
				CStringW PresetName;
				//WCHAR* pPresetName=new WCHAR[dwLength+1];
				//PresetKey.QueryValue(L"",pPresetName,dwLength);
				//m_aPossiblePresets.Add(pPresetName);
				PresetKey.QueryValue(L"",PresetName);
				m_aPossiblePresets.Add(PresetName.GiveBuffer());				
			}
	
			PresetKey.CloseKey();
		}		
	}

	// Current selections
	m_ActionCombo.SetCurSel(0);
	m_VerbCombo.SetCurSel(0);
	m_WhichFileCombo.SetCurSel(0);
	m_ContextMenuForCombo.SetCurSel(0);

	// Inserting items
	InsertShortcuts();
	
	ClearActionFields();
	EnableItems();


		
	return FALSE;
}


void CSettingsProperties::CKeyboardShortcutsPage::OnHelp(LPHELPINFO lphi)
{
	if (lphi->iCtrlId==IDC_SUBACTION || lphi->iCtrlId==IDC_VERB ||
		lphi->iCtrlId==IDC_WINDOW || lphi->iCtrlId==IDC_MESSAGE ||
		lphi->iCtrlId==IDC_WPARAM || lphi->iCtrlId==IDC_LPARAM ||
		lphi->iCtrlId==IDC_COMMAND || lphi->iCtrlId==IDC_VALUE ||
		lphi->iCtrlId==IDC_VALUEHELPTEXT || lphi->iCtrlId==IDC_WHICHFILE ||
		lphi->iCtrlId==IDC_CONTEXTMENUFOR ||
		lphi->iCtrlId==IDC_ITEM || lphi->iCtrlId==IDC_ITEMSPIN)
	{
		LPCSTR pPage=NULL;
		switch (GetSelectedAction())
		{
		case CAction::ShowHideDialog:
			pPage="actions_showhide.htm#ash%d";
			break;
		case CAction::ResultListItems:
			pPage="actions_resultslist.htm#alist%d";
			break;
		case CAction::Misc:
			pPage="actions_misc.htm#amisc%d";
			break;
		case CAction::ChangeValue:
			pPage="actions_changefields.htm";
			break;
		case CAction::Help:
			pPage="actions_help.htm#ahlp%d";
			break;
		case CAction::Settings:
			pPage="actions_settings.htm#aset%d";
			break;
		}

		if (pPage!=NULL)
		{
			char szPage[50];
			StringCbPrintf(szPage,50,pPage,m_SubActionCombo.GetCurSel()+1);
			if (CLocateApp::OpenHelp(*this,szPage))
				return;		
		}				
	}

	CLocateApp::HelpID id[]= {
		{ IDC_KEYLIST,"sk_list" },
		{ IDC_NEW,"sk_new" },
		{ IDC_REMOVE,"sk_remove" },
		{ IDC_UP,"sk_up" },
		{ IDC_DOWN,"sk_down" },
		{ IDC_RESET,"sk_reset" },
		{ IDC_STATICSHORTCUT,"sk_shortcutgroup" },
		{ IDC_HOTKEYRADIO,"sk_key" },
		{ IDC_SHORTCUTKEY,"sk_key" },
		{ IDC_FROMMNEMONIC,"sk_frommnemonic" },
		{ IDC_CODERADIO,"sk_virtualkey" },
		{ IDC_CODE,"sk_virtualkey" },
		{ IDC_MODCTRL,"sk_modifiers" },
		{ IDC_MODALT,"sk_modifiers" },
		{ IDC_MODSHIFT,"sk_modifiers" },
		{ IDC_MODWIN,"sk_modifiers" },
		{ IDC_STATICWHENPRESSED,"sk_whenpressed" },
		{ IDC_WHENPRESSED,"sk_whenpressed" },
		{ IDC_ADVANCED,"sk_advanced" },
		{ IDC_STATICACTIONS,"sk_actionsgroup" },
		{ IDC_ACTIONTOOLBAR,"sk_actionstoolbar" },
		{ IDC_ACTION,"sk_action" },
		{ IDC_SUBACTION,"sk_action" }
	};

	if (CLocateApp::OpenHelp(*this,"settings_shortcuts.htm",lphi,id,sizeof(id)/sizeof(CLocateApp::HelpID)))
		return;

	if (HtmlHelp(HH_HELP_CONTEXT,HELP_SETTINGS_SHORTCUTS)==NULL)
		HtmlHelp(HH_DISPLAY_TOPIC,0);
}



void CSettingsProperties::CKeyboardShortcutsPage::InsertShortcuts()
{
	LVITEM li;
	li.pszText=LPSTR_TEXTCALLBACK;
	li.mask=LVIF_TEXT|LVIF_PARAM;
	li.iSubItem=0;
	li.iItem=0;
	
	for (int i=0;i<m_pSettings->m_aShortcuts.GetSize();i++)
	{
		li.lParam=LPARAM(m_pSettings->m_aShortcuts[i]);
		
		m_pList->InsertItem(&li);
		li.iItem++;
	}
	delete[] m_pSettings->m_aShortcuts.GiveBuffer();
}

void CSettingsProperties::CKeyboardShortcutsPage::InsertSubActions()
{
	ASSERT(m_pCurrentShortcut!=NULL);
	ASSERT(m_nCurrentAction>=0 && m_nCurrentAction<m_pCurrentShortcut->m_apActions.GetSize());
	
	m_SubActionCombo.ResetContent();

	CAction::Action nAction=GetSelectedAction();
	
	CStringW SubActionLabel;
	switch (nAction)
	{
	case CAction::ActivateControl:
	case CAction::ChangeValue:
		SubActionLabel.LoadString(IDS_SHORTCUTSUBACTIONCONTROL);
		break;
	case CAction::ActivateTab:
		SubActionLabel.LoadString(IDS_SHORTCUTSUBACTIONTAB);
		break;
	case CAction::MenuCommand:
		SubActionLabel.LoadString(IDS_SHORTCUTSUBACTIONMENUITEM);
		break;
	case CAction::Presets:
		SubActionLabel.LoadString(IDS_SHORTCUTSUBACTIONPRESET);
		break;
	default:
		SubActionLabel.LoadString(IDS_SHORTCUTSUBACTION);
		break;
	}
	SetDlgItemText(IDC_STATICSUBACTION,SubActionLabel);

	UINT nSubAction;
	for (int nIndex=0;(nSubAction=IndexToSubAction(nAction,nIndex))!=(UINT)-1;nIndex++)
	{
		CStringW Title;
		if (GetSubActionLabel(Title,nAction,nSubAction))
			m_SubActionCombo.AddString(Title);
		else
			m_SubActionCombo.AddString(ID2W(IDS_UNKNOWN));
	}

	m_SubActionCombo.SetCurSel(0);

	if (m_pCurrentShortcut->m_apActions[m_nCurrentAction]->m_nAction==nAction)
	{
		UINT nIndex=SubActionToIndex(nAction,m_pCurrentShortcut->m_apActions[m_nCurrentAction]->m_nSubAction);
		if (nIndex!=(UINT)-1)
			m_SubActionCombo.SetCurSel(nIndex);
	}
}


UINT CSettingsProperties::CKeyboardShortcutsPage::IndexToSubAction(CAction::Action nAction,UINT nIndex) const
{
	switch (nAction)
	{
	case CAction::None:
		break;
	case CAction::ActivateControl:
		if (m_pPossibleControlsToActivate[nIndex]==CAction::NullControl)
			return (UINT)-1;
		return m_pPossibleControlsToActivate[nIndex];
	case CAction::ActivateTab:
		if (nIndex>CAction::ActivateTabsLast)
			return (UINT)-1;
		return nIndex;
	case CAction::MenuCommand:
		if (m_pPossibleMenuCommands[nIndex]==CAction::NullMenuCommand)
			return (UINT)-1;
		return m_pPossibleMenuCommands[nIndex];
	case CAction::ShowHideDialog:
		if (nIndex>CAction::ShowHideDialogLast)
			return (UINT)-1;
		return nIndex;
	case CAction::ResultListItems:
		if (nIndex>CAction::ResultListLast)
			return (UINT)-1;
		return nIndex;
	case CAction::Misc:
		if (nIndex>CAction::MiscLast)
			return (UINT)-1;
		return nIndex;
	case CAction::ChangeValue:
		if (m_pPossibleControlsToChange[nIndex]==CAction::NullControl)
			return (UINT)-1;
		return m_pPossibleControlsToChange[nIndex];
	case CAction::Presets:
		if (nIndex>=(UINT)m_aPossiblePresets.GetSize())
			return (UINT)-1;
		return nIndex;
	case CAction::Help:
		if (nIndex>CAction::HelpLast)
			return (UINT)-1;
		return nIndex;
	case CAction::Settings:
		if (nIndex>CAction::SettingsLast)
			return (UINT)-1;
		return nIndex;
	}
	return (UINT)-1;
}

UINT CSettingsProperties::CKeyboardShortcutsPage::SubActionToIndex(CAction::Action nAction,UINT nSubAction) const
{
	switch (nAction)
	{
	case CAction::None:
		return (UINT)-1;
	case CAction::ActivateControl:
		{
			for (int nIndex=0;m_pPossibleControlsToActivate[nIndex]!=CAction::NullControl;nIndex++)
			{
				if (m_pPossibleControlsToActivate[nIndex]==nSubAction)
					return nIndex;
			}
			return (UINT)-1;
		}
	case CAction::ActivateTab:
		if (nSubAction>CAction::ActivateTabsLast)
			return (UINT)-1;
		return nSubAction;
	case CAction::MenuCommand:
		{
			for (int nIndex=0;m_pPossibleMenuCommands[nIndex]!=CAction::NullMenuCommand;nIndex++)
			{
				if (m_pPossibleMenuCommands[nIndex]==nSubAction)
					return nIndex;
			}
			return (UINT)-1;
		}
	case CAction::ShowHideDialog:
		if (nSubAction>CAction::ShowHideDialogLast)
			return (UINT)-1;
		return nSubAction;
	case CAction::ResultListItems:
		if (nSubAction>CAction::ResultListLast)
			return (UINT)-1;
		return nSubAction;
	case CAction::Misc:
		if (nSubAction>CAction::MiscLast)
			return (UINT)-1;
		return nSubAction;
	case CAction::Presets:
		return nSubAction;
	case CAction::ChangeValue:
		{
			for (int nIndex=0;m_pPossibleControlsToChange[nIndex]!=CAction::NullControl;nIndex++)
			{
				if (m_pPossibleControlsToChange[nIndex]==nSubAction)
					return nIndex;
			}
			return (UINT)-1;
		}
	case CAction::Help:
		if (nSubAction>CAction::HelpLast)
			return (UINT)-1;
		return nSubAction;
	case CAction::Settings:
		if (nSubAction>CAction::SettingsLast)
			return (UINT)-1;
		return nSubAction;
	default:
		ASSERT(0);
		break;
	}
	return (UINT)-1;
	
}

BOOL CSettingsProperties::CKeyboardShortcutsPage::GetSubActionLabel(CStringW& str,CAction::Action nAction,
																	UINT uSubAction,void* pExtraInfo) const
{
	switch (nAction)
	{
	case CAction::None:
		str.LoadString(IDS_NONE);
		return TRUE;
	case CAction::ActivateControl:
	case CAction::ChangeValue:
		{
			// Check which dialog contains control
			WORD wControlID=HIWORD(uSubAction);
			
			if (wControlID&(1<<15)) 
			{
				// First bit up, id corresponds to string
				str.LoadString(~wControlID);
				return !str.IsEmpty();
			}
			else if (wControlID!=0)
			{
				// Checking wheter dialog contains control 
				CWnd Control;
				for (int i=0;hDialogs[i]!=NULL;i++)
				{
					Control.AttachToDlgItem(hDialogs[i],wControlID);
					if (HWND(Control)!=NULL)
						break;
				}

				if (HWND(Control)!=NULL)
				{
					int nIndex;
					Control.GetWindowText(str);
					while ((nIndex=str.Find('&'))!=-1)
						str.DelChar(nIndex);
					
					
					return TRUE;
				}
			}
			return FALSE;
		}
	case CAction::ActivateTab:
		{
			str.LoadString(CAction::GetActivateTabsActionLabelStringId(
				(CAction::ActionActivateTabs)IndexToSubAction(CAction::ActivateTab,uSubAction)));		

			int nIndex;
			while ((nIndex=str.Find(L"&&"))!=-1)
				str.DelChar(nIndex);
			return TRUE;
		}
	case CAction::MenuCommand:
		{
			WCHAR szLabel[1000];
			MENUITEMINFOW mii;
			BYTE nSubMenu=CAction::GetMenuAndSubMenu((CAction::ActionMenuCommands)uSubAction);
			CMenu SubMenu(GetSubMenu((nSubMenu&128)?hMainMenu:hPopupMenu,nSubMenu&~128));
			
			mii.cbSize=sizeof(MENUITEMINFOW);
			mii.fMask=MIIM_ID|MIIM_TYPE;
			mii.dwTypeData=szLabel;
			mii.cch=1000;
				
			
			BOOL bRet=SubMenu.GetMenuItemInfo(LOWORD(uSubAction),FALSE,&mii);

			if (bRet && mii.fType==MFT_STRING)
			{
				str.Format((INT)HIWORD(uSubAction),szLabel);
				int nIndex=str.FindFirst(L'\t');
				if (nIndex!=-1)
					str.FreeExtra(nIndex);

				while ((nIndex=str.Find(L'&'))!=-1)
                    str.DelChar(nIndex);
				return TRUE;
			}
			return FALSE;
		}
	case CAction::ShowHideDialog:
		str.LoadString(CAction::GetShowHideDialogActionLabelStringId(
			(CAction::ActionShowHideDialog)IndexToSubAction(CAction::ShowHideDialog,uSubAction)));		
		break;
	case CAction::ResultListItems:
		str.LoadString(CAction::GetResultItemActionLabelStringId(
			(CAction::ActionResultList)IndexToSubAction(CAction::ResultListItems,uSubAction)));		
		if (uSubAction==CAction::SelectNthFile ||
			uSubAction==CAction::ExecuteNthFile)
		{
			CStringW str2;
			int nExtId=IDS_NTH;
			WCHAR szValue[10]=L"N";
			if (pExtraInfo!=INVALID_HANDLE_VALUE)
			{
				switch ((int)pExtraInfo)
				{
				case 0:
					nExtId=IDS_FIRST;
					break;
				case 1:
					nExtId=IDS_SECOND;
					break;
				case 2:
					nExtId=IDS_THIRD;
					break;
				}
				_ltow_s((int)pExtraInfo+1,szValue,10,10);
			}
			
			str2.Format(str,szValue,(LPCWSTR)ID2W(nExtId));
			str=str2;
		}
		break;
	case CAction::Misc:
		str.LoadString(CAction::GetMiscActionLabelStringId(
			(CAction::ActionMisc)IndexToSubAction(CAction::Misc,uSubAction)));
		break;
	case CAction::Presets:
		ASSERT(uSubAction<(UINT)m_aPossiblePresets.GetSize());
		str.Copy(m_aPossiblePresets[(int)uSubAction]);
		break;
	case CAction::Help:
		str.LoadString(CAction::GetHelpActionLabelStringId(
			(CAction::ActionHelp)IndexToSubAction(CAction::Help,uSubAction)));		
		break;
	case CAction::Settings:
		CAction::GetSettingsActionLabelString(
			(CAction::ActionSettings)IndexToSubAction(CAction::Help,uSubAction),str);
		break;
	default:
		ASSERT(0);
		return FALSE;
	}

	return TRUE;
}


BOOL CSettingsProperties::CKeyboardShortcutsPage::OnApply()
{
	CPropertyPage::OnApply();
	
	SdDebugMessage("CKeyboardShortcutsPage::OnApply() BEGIN");

	if (m_pCurrentShortcut!=NULL)
		SaveFieldsForShortcut(m_pCurrentShortcut);

	m_pSettings->m_aShortcuts.RemoveAll();
	
	if (m_pList->GetItemCount()>0)
	{
		int nItem=m_pList->GetNextItem(-1,LVNI_ALL);
		while (nItem!=-1)
		{
			CShortcut* pShortcut=(CShortcut*)m_pList->GetItemData(nItem);
			if (pShortcut!=NULL)
			{
				m_pSettings->m_aShortcuts.Add(pShortcut);
				m_pList->SetItemData(nItem,NULL);
			}
			nItem=m_pList->GetNextItem(nItem,LVNI_ALL);
		}
	}

	SdDebugMessage("CKeyboardShortcutsPage::OnApply() END");
	
	return TRUE;
}

void CSettingsProperties::CKeyboardShortcutsPage::OnDestroy()
{
	SdDebugMessage("CKeyboardShortcutsPage::OnDestroy() BEGIN");

	CPropertyPage::OnDestroy();

	

	if (m_pList!=NULL)
	{
		m_pList->SaveColumnsState(HKCU,CRegKey2::GetCommonKey()+"\\Dialogs","Shortcuts Settings List Widths");
		delete m_pList;
		m_pList=NULL;
	}

	if (m_pToolBar!=NULL)
	{
		delete m_pToolBar;
		m_pToolBar=NULL;
	}

	if (m_pWhenPressedList!=NULL)
	{
		delete m_pWhenPressedList;
		m_pWhenPressedList=NULL;
	}

	SdDebugMessage("CKeyboardShortcutsPage::OnDestroy() END");
}
		
void CSettingsProperties::CKeyboardShortcutsPage::OnCancel()
{
	m_pSettings->SetSettingsFlags(CSettingsProperties::settingsCancelled);

	CPropertyPage::OnCancel();
}

void CSettingsProperties::CKeyboardShortcutsPage::OnTimer(DWORD wTimerID)
{
	/*KillTimer(wTimerID);

	if (m_pList->GetNextItem(-1,LVNI_SELECTED)==-1)
		m_pList->SetItemState(nLastSel,LVIS_SELECTED,LVIS_SELECTED);*/
}

BOOL CSettingsProperties::CKeyboardShortcutsPage::OnNotify(int idCtrl,LPNMHDR pnmh)
{
	ASSERT(idCtrl==pnmh->idFrom);
	
	switch (idCtrl)
	{
	case IDC_KEYLIST:
		ListNotifyHandler((NMLISTVIEW*)pnmh);
		break;
	case IDC_WHEREPRESSED:
		WherePressedNotifyHandler((NMLISTVIEW*)pnmh);
		break;
	default:
		if (pnmh->code==TTN_NEEDTEXT)
		{
			switch (pnmh->idFrom)
			{
			case IDC_ADDACTION:
				((LPTOOLTIPTEXT)pnmh)->lpszText=allocstring(IDS_SHORTCUTADDACTION);
				break;
			case IDC_REMOVEACTION:
				((LPTOOLTIPTEXT)pnmh)->lpszText=allocstring(IDS_SHORTCUTREMOVEACTION);
				break;
			case IDC_NEXT:
				((LPTOOLTIPTEXT)pnmh)->lpszText=allocstring(IDS_SHORTCUTNEXTACTION);
				break;
			case IDC_PREV:
				((LPTOOLTIPTEXT)pnmh)->lpszText=allocstring(IDS_SHORTCUTPREVACTION);
				break;
			case IDC_SWAPWITHPREVIOUS:
				((LPTOOLTIPTEXT)pnmh)->lpszText=allocstring(IDS_SHORTCUTSWAPWITHPREVIOUS);
				break;
			case IDC_SWAPWITHNEXT:
				((LPTOOLTIPTEXT)pnmh)->lpszText=allocstring(IDS_SHORTCUTSWAPWITHNEXT);
				break;
			default:
				((LPTOOLTIPTEXT)pnmh)->lpszText=allocstring(IDS_UNKNOWN);
				break;
			}
			AssignBuffer(((LPTOOLTIPTEXT)pnmh)->lpszText);
		}
		else if (pnmh->code==TTN_NEEDTEXTW)
		{
			switch (pnmh->idFrom)
			{
			case IDC_ADDACTION:
				((LPTOOLTIPTEXTW)pnmh)->lpszText=allocstringW(IDS_SHORTCUTADDACTION);
				break;
			case IDC_REMOVEACTION:
				((LPTOOLTIPTEXTW)pnmh)->lpszText=allocstringW(IDS_SHORTCUTREMOVEACTION);
				break;
			case IDC_NEXT:
				((LPTOOLTIPTEXTW)pnmh)->lpszText=allocstringW(IDS_SHORTCUTNEXTACTION);
				break;
			case IDC_PREV:
				((LPTOOLTIPTEXTW)pnmh)->lpszText=allocstringW(IDS_SHORTCUTPREVACTION);
				break;
			case IDC_SWAPWITHPREVIOUS:
				((LPTOOLTIPTEXTW)pnmh)->lpszText=allocstringW(IDS_SHORTCUTSWAPWITHPREVIOUS);
				break;
			case IDC_SWAPWITHNEXT:
				((LPTOOLTIPTEXTW)pnmh)->lpszText=allocstringW(IDS_SHORTCUTSWAPWITHNEXT);
				break;
			default:
				((LPTOOLTIPTEXTW)pnmh)->lpszText=allocstringW(IDS_UNKNOWN);
				break;
			}
			AssignBuffer(((LPTOOLTIPTEXTW)pnmh)->lpszText);				
		}
	}
	return CPropertyPage::OnNotify(idCtrl,pnmh);
}


		

BOOL CSettingsProperties::CKeyboardShortcutsPage::ListNotifyHandler(NMLISTVIEW *pNm)
{
	switch(pNm->hdr.code)
	{
	case LVN_DELETEITEM:
		if (pNm->lParam!=NULL)
			delete (CShortcut*)pNm->lParam;
		break;
	case LVN_GETDISPINFO:
		{
			LV_DISPINFO *pLvdi=(LV_DISPINFO *)pNm;
			//CShortcut* pShortcut=(CShortcut*)pLvdi->item.lParam;
			if (pLvdi->item.lParam==NULL)
				break;
			LPSTR szBuffer=NULL;
			
			// Retrieving state
			pLvdi->item.mask=LVIF_STATE;
			pLvdi->item.stateMask=LVIS_SELECTED;
			m_pList->GetItem(&pLvdi->item);
			pLvdi->item.mask=LVIF_TEXT;

			switch (pLvdi->item.iSubItem)
			{
			case 0:// Shortcut
				if (pLvdi->item.state&LVIS_SELECTED)
				{
					// Item is selected and not overrided, retrieving shortcut from hotkey control
					if (IsDlgButtonChecked(IDC_HOTKEYRADIO))
					{
						CStringW Buffer;
						WORD wKey=(WORD)SendDlgItemMessage(IDC_SHORTCUTKEY,HKM_GETHOTKEY,0,0);
						CShortcut::FormatKeyLabel(m_pVirtualKeyNames,LOBYTE(wKey),CShortcut::HotkeyModifiersToModifiers(HIBYTE(wKey)),
							(m_pCurrentShortcut->m_dwFlags&CShortcut::sfVirtualKeyIsScancode)?TRUE:FALSE,Buffer);
						szBuffer=alloccopyWtoA(Buffer,Buffer.GetLength());
					}
					else
					{
						BOOL bScancode=(m_pCurrentShortcut->m_dwFlags&CShortcut::sfVirtualKeyIsScancode)?TRUE:FALSE;
						BYTE bVKey=GetVirtualCode(m_pCurrentShortcut->m_dwFlags&CShortcut::sfVirtualKeyIsScancode);
						BYTE bModifiers=0;
							
						if (IsDlgButtonChecked(IDC_MODCTRL))
							bModifiers|=CShortcut::ModifierControl;
						if (IsDlgButtonChecked(IDC_MODALT))
							bModifiers|=CShortcut::ModifierAlt;
						if (IsDlgButtonChecked(IDC_MODSHIFT))
							bModifiers|=CShortcut::ModifierShift;
						if (IsDlgButtonChecked(IDC_MODWIN))
							bModifiers|=CShortcut::ModifierWin;
						CStringW Buffer;
						CShortcut::FormatKeyLabel(m_pVirtualKeyNames,bVKey,bModifiers,bScancode,Buffer);
						szBuffer=alloccopyWtoA(Buffer,Buffer.GetLength());
					}
				}
				else
				{
					CStringW Buffer;
					((CShortcut*)pLvdi->item.lParam)->FormatKeyLabel(m_pVirtualKeyNames,Buffer);
					szBuffer=alloccopyWtoA(Buffer,Buffer.GetLength());
				}
				break;
			case 1: // Type
				switch (((CShortcut*)pLvdi->item.lParam)->m_dwFlags&CShortcut::sfKeyTypeMask)
				{
				case CShortcut::sfGlobalHotkey:
					szBuffer=allocstring(IDS_ADVSHORTCUTGLOBALHOTKEY);
					break;
				case CShortcut::sfGlobalHook:
					szBuffer=allocstring(IDS_ADVSHORTCUTGLOBALHOOK);
					break;
				case CShortcut::sfLocal:
					szBuffer=allocstring(IDS_ADVSHORTCUTLOCAL);
					break;
				}
				break;
			case 2: // Action
				if (pLvdi->item.state&LVIS_SELECTED)
				{
					if (((CShortcut*)pLvdi->item.lParam)->m_apActions.GetSize()>1)
					{
						szBuffer=allocstring(IDS_SHORTCUTMULTIPLEACTIONS);
						break;
					}
					
					int nCurSel=m_SubActionCombo.GetCurSel();
					UINT nSubAction=0;
					CAction::Action nAction=GetSelectedAction();
					if (nAction==CAction::None)
					{
						szBuffer=allocstring(IDS_NONE);
						break;
					}
					if ((nSubAction=IndexToSubAction(nAction,nCurSel))!=UINT(-1))
					{
						CStringW Buffer;
						void* pExtraInfo=INVALID_HANDLE_VALUE;
						if (nAction==CAction::ResultListItems && 
							(nSubAction==CAction::SelectNthFile ||
							nSubAction==CAction::ExecuteNthFile))
						{
							BOOL bError;
							int nItem=(int)SendDlgItemMessage(IDC_ITEMSPIN,UDM_GETPOS32,0,(LPARAM)&bError);
							if (bError)
							{
								nItem=GetDlgItemInt(IDC_ITEM,&bError,FALSE);
								bError=!bError;
							}
							if (!bError && nItem>0)
								pExtraInfo=(void*)(nItem-1);							
						}
						FormatActionLabel(Buffer,nAction,nSubAction,pExtraInfo);
						szBuffer=alloccopyWtoA(Buffer,Buffer.GetLength());
						break;
					}
					
					szBuffer=allocstring(IDS_UNKNOWN);
				}				
				else
				{
					CStringA Buffer;
					CShortcut* pShortcut=((CShortcut*)pLvdi->item.lParam);
					for (int i=0;i<pShortcut->m_apActions.GetSize();i++)
					{
						CStringW Action;
						FormatActionLabel(Action,(CAction::Action)pShortcut->m_apActions[i]->m_nAction,
							pShortcut->m_apActions[i]->m_nSubAction,pShortcut->m_apActions[i]->m_pExtraInfo);
						
						if (i>0)
							Buffer << ", ";
						Buffer << Action;
					}

					szBuffer=Buffer.GiveBuffer();
				}
				break;
			default:
				szBuffer=allocstring(IDS_UNKNOWN);
				break;
			}
			AssignBuffer(szBuffer);
			pLvdi->item.pszText=szBuffer;
			break;
		}
	case LVN_GETDISPINFOW:
		{
			LV_DISPINFOW *pLvdi=(LV_DISPINFOW *)pNm;
			//CShortcut* pShortcut=(CShortcut*)pLvdi->item.lParam;
			if (pLvdi->item.lParam==NULL)
				break;
			LPWSTR szwBuffer=NULL;

			// Retrieving state
			pLvdi->item.mask=LVIF_STATE;
			pLvdi->item.stateMask=LVIS_SELECTED;
			m_pList->GetItem(&pLvdi->item);
			pLvdi->item.mask=LVIF_TEXT;

			switch (pLvdi->item.iSubItem)
			{
			case 0:// Shortcut
				if (pLvdi->item.state&LVIS_SELECTED)
				{
					// Item is selected and not overrided, retrieving shortcut from hotkey control
					if (IsDlgButtonChecked(IDC_HOTKEYRADIO))
					{
						WORD wKey=(WORD)SendDlgItemMessage(IDC_SHORTCUTKEY,HKM_GETHOTKEY,0,0);
						CStringW Buffer;
						CShortcut::FormatKeyLabel(m_pVirtualKeyNames,LOBYTE(wKey),CShortcut::HotkeyModifiersToModifiers(HIBYTE(wKey)),
							(m_pCurrentShortcut->m_dwFlags&CShortcut::sfVirtualKeyIsScancode)?TRUE:FALSE,Buffer);
						szwBuffer=Buffer.GiveBuffer();
					}
					else
					{
						BOOL bScancode=(m_pCurrentShortcut->m_dwFlags&CShortcut::sfVirtualKeyIsScancode)?TRUE:FALSE;
						BYTE bVKey=GetVirtualCode(m_pCurrentShortcut->m_dwFlags&CShortcut::sfVirtualKeyIsScancode);
						BYTE bModifiers=0;
							
						if (IsDlgButtonChecked(IDC_MODCTRL))
							bModifiers|=CShortcut::ModifierControl;
						if (IsDlgButtonChecked(IDC_MODALT))
							bModifiers|=CShortcut::ModifierAlt;
						if (IsDlgButtonChecked(IDC_MODSHIFT))
							bModifiers|=CShortcut::ModifierShift;
						if (IsDlgButtonChecked(IDC_MODWIN))
							bModifiers|=CShortcut::ModifierWin;
						CStringW Buffer;
						CShortcut::FormatKeyLabel(m_pVirtualKeyNames,bVKey,bModifiers,bScancode,Buffer);
						szwBuffer=Buffer.GiveBuffer();
					}
				}
				else
				{
					CStringW Buffer;
					((CShortcut*)pLvdi->item.lParam)->FormatKeyLabel(m_pVirtualKeyNames,Buffer);
					szwBuffer=Buffer.GiveBuffer();
				}
				break;
			case 1: // Type
				switch (((CShortcut*)pLvdi->item.lParam)->m_dwFlags&CShortcut::sfKeyTypeMask)
				{
				case CShortcut::sfGlobalHotkey:
					szwBuffer=allocstringW(IDS_ADVSHORTCUTGLOBALHOTKEY);
					break;
				case CShortcut::sfGlobalHook:
					szwBuffer=allocstringW(IDS_ADVSHORTCUTGLOBALHOOK);
					break;
				case CShortcut::sfLocal:
					szwBuffer=allocstringW(IDS_ADVSHORTCUTLOCAL);
					break;
				}
				break;
			case 2: // Action
				if (pLvdi->item.state&LVIS_SELECTED)
				{
					if (((CShortcut*)pLvdi->item.lParam)->m_apActions.GetSize()>1)
					{
						szwBuffer=allocstringW(IDS_SHORTCUTMULTIPLEACTIONS);
						break;
					}
				
					int nCurSel=m_SubActionCombo.GetCurSel();
					UINT nSubAction=0;
					CAction::Action nAction=GetSelectedAction();
					if (nAction==CAction::None)
					{
						szwBuffer=allocstringW(IDS_NONE);
						break;
					}
					if ((nSubAction=IndexToSubAction(nAction,nCurSel))!=UINT(-1))
					{
						CStringW Buffer;
						void* pExtraInfo=INVALID_HANDLE_VALUE;
						if (nAction==CAction::ResultListItems && 
							(nSubAction==CAction::SelectNthFile ||
							nSubAction==CAction::ExecuteNthFile))
						{
							BOOL bError;
							int nItem=(int)SendDlgItemMessage(IDC_ITEMSPIN,UDM_GETPOS32,0,(LPARAM)&bError);
							if (bError)
							{
								nItem=GetDlgItemInt(IDC_ITEM,&bError,FALSE);
								bError=!bError;
							}
							if (!bError && nItem>0)
								pExtraInfo=(void*)(nItem-1);					
						}
						FormatActionLabel(Buffer,nAction,nSubAction,pExtraInfo);
						szwBuffer=Buffer.GiveBuffer();
						break;
					}
					
					szwBuffer=allocstringW(IDS_UNKNOWN);
				}				
				else
				{
					CStringW Buffer;
					CShortcut* pShortcut=((CShortcut*)pLvdi->item.lParam);
					for (int i=0;i<pShortcut->m_apActions.GetSize();i++)
					{
						CStringW Action;
						FormatActionLabel(Action,(CAction::Action)pShortcut->m_apActions[i]->m_nAction,
							pShortcut->m_apActions[i]->m_nSubAction,pShortcut->m_apActions[i]->m_pExtraInfo);
						if (i>0)
							Buffer << L", ";
						Buffer << Action;
					}
					szwBuffer=Buffer.GiveBuffer();
				}
				break;
			default:
				szwBuffer=allocstringW(IDS_UNKNOWN);
				break;
			}
			AssignBuffer(szwBuffer);
			pLvdi->item.pszText=szwBuffer;
			break;
		}
	case LVN_ITEMCHANGED:
		OnChangeItem(pNm);
		break;
	case LVN_ITEMCHANGING:
		OnChangingItem(pNm);
		break;
	}
	return TRUE;
}

BOOL CSettingsProperties::CKeyboardShortcutsPage::WherePressedNotifyHandler(NMLISTVIEW *pNm)
{
	switch(pNm->hdr.code)
	{
	case LVN_ITEMCHANGING:
		if (pNm->uNewState&LVIS_SELECTED && !(pNm->uOldState&LVIS_SELECTED))
		{
			pNm->uNewState&=~LVIS_SELECTED;
			
			SetWindowLong(CWnd::dwlMsgResult,TRUE);
		}
		break;
	case LVN_ITEMCHANGED:
		if (pNm->uNewState&LVIS_SELECTED)
			m_pWhenPressedList->SetItemState(pNm->iItem,0,LVIS_SELECTED);
		break;
	}
	return TRUE;
}
BOOL CSettingsProperties::CKeyboardShortcutsPage::OnCommand(WORD wID,WORD wNotifyCode,HWND hControl)
{
	CDialog::OnCommand(wID,wNotifyCode,hControl);

	switch (wID)
	{
	case IDC_NEW:
		OnNewShortcut();
		break;
	case IDC_REMOVE:
		OnRemoveShortcut();
		break;
	case IDC_UP:
	case IDC_DOWN:
		ItemUpOrDown(wID==IDC_UP);
		break;
	case IDC_RESET:
		OnResetToDefaults();
		break;
	case IDC_ADVANCED:
		OnAdvanced();
		break;
	case IDC_ADDACTION:
		OnAddAction();
		break;
	case IDC_FROMMNEMONIC:
		SetFieldsRelativeToMnemonics();
		EnableItems();
		RefreshShortcutListLabels();
		break;
	case IDC_REMOVEACTION:
		OnRemoveAction();
		break;
	case IDC_NEXT:
	case IDC_PREV:
		OnNextAction(wID==IDC_NEXT);
		break;
	case IDC_SWAPWITHPREVIOUS:
	case IDC_SWAPWITHNEXT:
		OnSwapAction(wID==IDC_SWAPWITHNEXT);
		break;
	case IDC_ACTION:
	case IDC_SUBACTION:
		if (wNotifyCode==CBN_SELCHANGE)
		{
			SaveFieldsForAction(m_pCurrentShortcut->m_apActions[m_nCurrentAction]);
			if (wID==IDC_ACTION)
				InsertSubActions();

			
			if (m_pCurrentShortcut->m_dwFlags&CShortcut::sfUseMemonic)
			{
				if (m_pCurrentShortcut->GetMnemonicForAction(hDialogs)==0)
				{
					m_pCurrentShortcut->m_dwFlags&=~CShortcut::sfUseMemonic;
					CheckDlgButton(IDC_FROMMNEMONIC,FALSE);
				}
				else
					SetFieldsRelativeToMnemonics();
			}

			EnableItems();
			RefreshShortcutListLabels();
		}
		break;		
	case IDC_HOTKEYRADIO:
	case IDC_CODERADIO:
		if (IsDlgButtonChecked(wID))
			break;

		CheckDlgButton(IDC_HOTKEYRADIO,wID==IDC_HOTKEYRADIO);
		CheckDlgButton(IDC_CODERADIO,wID==IDC_CODERADIO);
		

		EnableDlgItem(IDC_SHORTCUTKEY,wID==IDC_HOTKEYRADIO);
		EnableDlgItem(IDC_CODE,wID==IDC_CODERADIO);
		EnableDlgItem(IDC_MODCTRL,wID==IDC_CODERADIO);
		EnableDlgItem(IDC_MODALT,wID==IDC_CODERADIO);
		EnableDlgItem(IDC_MODSHIFT,wID==IDC_CODERADIO);
		EnableDlgItem(IDC_MODWIN,wID==IDC_CODERADIO);

		InsertKeysToVirtualKeyCombo();
		RefreshShortcutListLabels();

		SetFocus(wID==IDC_HOTKEYRADIO?IDC_SHORTCUTKEY:IDC_CODE);
		break;
	case IDC_MODSHIFT:
	case IDC_MODCTRL:
	case IDC_MODALT:
	case IDC_MODWIN:
	case IDC_CODE:
		SetShortcutKeyWhenVirtualKeyChanged();
		RefreshShortcutListLabels();
		break;
	case IDC_SHORTCUTKEY:
		SetVirtualKeyWhenShortcutKeyChanged();
		RefreshShortcutListLabels();
		break;
	case IDC_VALUEHELPTEXT:
		{
			CStringW sHelpPage;
			if (GetApp()->m_szHelpFile!=NULL)
			{	
				// Form path to help file
				CStringW sHelpFile=GetApp()->GetExeNameW();
				sHelpFile.FreeExtra(sHelpFile.FindLast(L'\\')+1);
				sHelpFile << GetApp()->m_szHelpFile << L"::/actions_changefields.htm";
				
				HtmlHelpW(*this,sHelpFile,HH_DISPLAY_TOPIC,NULL);
			
			}
			break;
		}
	}
	return FALSE;
}

BOOL CSettingsProperties::CKeyboardShortcutsPage::ItemUpOrDown(BOOL bUp)
{
	ASSERT(m_pCurrentShortcut!=NULL);
	SaveFieldsForShortcut(m_pCurrentShortcut);

	int nSelected=m_pList->GetNextItem(-1,LVNI_SELECTED);
	ASSERT(nSelected!=-1);
	ASSERT(m_pCurrentShortcut==(CShortcut*)m_pList->GetItemData(nSelected));
	CShortcut* pSelected=m_pCurrentShortcut;
	
	
	int nOther=m_pList->GetNextItem(nSelected,bUp?LVNI_ABOVE:LVNI_BELOW);
	CShortcut* pOther=(CShortcut*)m_pList->GetItemData(nOther);
	ASSERT(pOther!=NULL);
	
	// Swapping

    // First set data to NULL
	m_pList->SetItemData(nSelected,NULL);
	m_pList->SetItemData(nOther,NULL);
	
	// Swap states
    UINT nState=m_pList->GetItemState(nSelected,0xFFFFFFFF);
	m_pList->SetItemState(nSelected,m_pList->GetItemState(nOther,0xFFFFFFFF),0xFFFFFFFF);
	m_pList->SetItemState(nOther,nState,0xFFFFFFFF);

	// Changes data
	m_pList->SetItemData(nSelected,(LPARAM)pOther);
	m_pList->SetItemData(nOther,(LPARAM)pSelected);
	
	// At this moment m_pCurrentShortcut points to pOther
	SetFieldsForShortcut(m_pCurrentShortcut=pSelected);
	
	
	
	m_pList->EnsureVisible(nOther,FALSE);
	m_pList->RedrawItems(min(nSelected,nOther),max(nSelected,nOther));
	m_pList->UpdateWindow();
	
	EnableItems();
	RefreshShortcutListLabels();
	return TRUE;
}


void CSettingsProperties::CKeyboardShortcutsPage::SetShortcutKeyWhenVirtualKeyChanged()
{
	ASSERT(m_pCurrentShortcut!=NULL);

	if (m_pCurrentShortcut->m_dwFlags&CShortcut::sfVirtualKeyIsScancode)
		return;

	
	BYTE bKey=GetVirtualCode(FALSE);
    BYTE bHotkey=0;
	if (IsDlgButtonChecked(IDC_MODALT))
		bHotkey=HOTKEYF_ALT;
	if (IsDlgButtonChecked(IDC_MODCTRL))
		bHotkey=HOTKEYF_CONTROL;
	if (IsDlgButtonChecked(IDC_MODSHIFT))
		bHotkey=HOTKEYF_SHIFT;

	SetHotKey(bKey,bHotkey);
	
}

void CSettingsProperties::CKeyboardShortcutsPage::SetVirtualKeyWhenShortcutKeyChanged()
{
	
	DWORD dwKey=(DWORD)SendDlgItemMessage(IDC_SHORTCUTKEY,HKM_GETHOTKEY,0,0);

    if (dwKey==0)
		return;


	SetVirtualCode(LOBYTE(dwKey),FALSE);
	
	CheckDlgButton(IDC_MODALT,(dwKey&(HOTKEYF_ALT<<8))?1:0);
	CheckDlgButton(IDC_MODSHIFT,(dwKey&(HOTKEYF_SHIFT<<8))?1:0);
	CheckDlgButton(IDC_MODCTRL,(dwKey&(HOTKEYF_CONTROL<<8))?1:0);
}

void CSettingsProperties::CKeyboardShortcutsPage::SetVirtualCode(BYTE bCode,BOOL bScanCode)
{
	if (!bScanCode)
	{
		// Check for VK_ code
		for (int i=0;m_pVirtualKeyNames[i].bKey!=0;i++)
		{
			if (m_pVirtualKeyNames[i].bKey==bCode)
			{
				SendDlgItemMessage(IDC_CODE,CB_SETCURSEL,i);
				SetDlgItemText(IDC_CODE,m_pVirtualKeyNames[i].pName);
				return;
			}
		}


		// First, check whether code is just ascii item
		BYTE pKeyState[256];
		GetKeyboardState(pKeyState);
		WORD wChar;
		int nRet=ToAscii(bCode,0,pKeyState,&wChar,0);
		if (nRet==1)
		{
			char text[]="\"X\"";
			text[1]=BYTE(wChar);
			MakeUpper(text+1,1);

			SendDlgItemMessage(IDC_CODE,CB_SETCURSEL,-1);
			SetDlgItemText(IDC_CODE,text);
			return;
		}

	}

	
	SetDlgItemInt(IDC_CODE,bCode,FALSE);
}

BYTE CSettingsProperties::CKeyboardShortcutsPage::GetVirtualCode(BOOL bScanCode) const
{
	if (!bScanCode)
	{
		int nSel=(int)SendDlgItemMessage(IDC_CODE,CB_GETCURSEL);
		if (nSel!=CB_ERR)
			return m_pVirtualKeyNames[nSel].bKey;
		
		DWORD dwTextLen=GetDlgItemTextLength(IDC_CODE);
        if (dwTextLen>2) 
		{
            char* pText=new char[dwTextLen+2];
			GetDlgItemText(IDC_CODE,pText,dwTextLen+2);
			
			if (pText[0]=='\"') // Has form "X"
			{
				SHORT sRet=VkKeyScan(pText[1]);
				return BYTE(sRet);
			}
		}
	}

	BOOL pTranslated;
	UINT nKey=GetDlgItemInt(IDC_CODE,&pTranslated,FALSE);
	if (!pTranslated || nKey>255)
		return 0;
	
	return (BYTE)nKey;
}


void CSettingsProperties::CKeyboardShortcutsPage::InsertKeysToVirtualKeyCombo()
{
	if (m_pCurrentShortcut->m_dwFlags&CShortcut::sfVirtualKeyIsScancode)
	{
		if (SendDlgItemMessage(IDC_CODE,CB_GETCURSEL)!=CB_ERR)
		{
			// Just reset
			SendDlgItemMessage(IDC_CODE,CB_RESETCONTENT);
			return;
		}

		// Save old value
		BYTE bCode=GetVirtualCode(FALSE);
		
		// Reset content
		SendDlgItemMessage(IDC_CODE,CB_RESETCONTENT);
		
		// Set old value
		SetVirtualCode(bCode,TRUE);
	}
	else
	{
		if (SendDlgItemMessage(IDC_CODE,CB_GETCOUNT)>0)
			return; // Do nothing, items are already listed

		// Save old value
		BYTE bCode=GetVirtualCode(TRUE);
		
		// Reset content
		SendDlgItemMessage(IDC_CODE,CB_RESETCONTENT);

		// Insert items
		for (int i=0;m_pVirtualKeyNames[i].bKey!=0;i++)
			SendDlgItemMessage(IDC_CODE,CB_ADDSTRING,0,(LPARAM)m_pVirtualKeyNames[i].pName);
		
		// Set old value
		SetVirtualCode(bCode,FALSE);

	}
	
}

void CSettingsProperties::CKeyboardShortcutsPage::OnNewShortcut()
{
	LVITEM li;
	li.pszText=LPSTR_TEXTCALLBACK;
	li.mask=LVIF_TEXT|LVIF_PARAM|LVIF_STATE;
	li.iSubItem=0;
	li.iItem=m_pList->GetItemCount();
	li.lParam=LPARAM(new CShortcut);
	li.stateMask=li.state=LVIS_SELECTED;
	
	int nItem=m_pList->InsertItem(&li);
	
	if (nItem!=-1)
		m_pList->EnsureVisible(nItem,FALSE);

	EnableItems();

	SetFocus(IDC_SHORTCUTKEY);

}

void CSettingsProperties::CKeyboardShortcutsPage::OnRemoveShortcut()
{
	int nItem=m_pList->GetNextItem(-1,LVNI_SELECTED);
	ASSERT(nItem!=-1);
	
	m_pCurrentShortcut=NULL;
	
	CShortcut* pShortcut=(CShortcut*)m_pList->GetItemData(nItem);
	m_pList->SetItemData(nItem,NULL);
	if (pShortcut!=NULL)
		delete pShortcut;

	m_pList->DeleteItem(nItem);
	
	EnableItems();
}

void CSettingsProperties::CKeyboardShortcutsPage::OnResetToDefaults()
{
	m_pCurrentShortcut=NULL;
	m_pList->DeleteAllItems();

	m_pSettings->m_aShortcuts.RemoveAll();

	OSVERSIONINFOEX oi;
	oi.dwOSVersionInfoSize=sizeof(OSVERSIONINFOEX);
	BOOL bRet=GetVersionEx((LPOSVERSIONINFO) &oi);
	BOOL bCanHook=!(bRet && oi.dwPlatformId!=VER_PLATFORM_WIN32_NT ||
		!(oi.dwMajorVersion>=5 || (oi.dwMajorVersion==4 && oi.wServicePackMajor>=3) ));


	if (!CShortcut::GetDefaultShortcuts(m_pSettings->m_aShortcuts,
		CShortcut::loadLocal|CShortcut::loadGlobalHotkey|(bCanHook?CShortcut::loadGlobalHook:0)))
	{
		ShowErrorMessage(IDS_ERRORCANNOTLOADDEFAULTSHORTUCS,IDS_ERROR);
		m_pSettings->m_aShortcuts.RemoveAll();
	}		
	else
		InsertShortcuts();


	RefreshShortcutListLabels();
	EnableItems();
}

void CSettingsProperties::CKeyboardShortcutsPage::OnAdvanced()
{
	ASSERT(m_pCurrentShortcut!=NULL);
	
    CAdvancedDlg dlg(m_pCurrentShortcut);
	SaveFieldsForShortcut(m_pCurrentShortcut);
	
	if (dlg.DoModal(*this))
	{
		InsertKeysToVirtualKeyCombo();
		SetFieldsForShortcut(m_pCurrentShortcut);
	}

	RefreshShortcutListLabels();
	EnableItems();
}

void CSettingsProperties::CKeyboardShortcutsPage::SetFieldsRelativeToMnemonics()
{
	ASSERT(m_pCurrentShortcut!=NULL);
	
	if (IsDlgButtonChecked(IDC_FROMMNEMONIC))
	{
		char cMnemonic=m_pCurrentShortcut->GetMnemonicForAction(hDialogs);
		
		if (cMnemonic==0)
		{
			m_pCurrentShortcut->m_dwFlags&=~CShortcut::sfUseMemonic;
			CheckDlgButton(IDC_FROMMNEMONIC,FALSE);
			return;
		}

		m_pCurrentShortcut->m_dwFlags|=CShortcut::sfUseMemonic;
		
		// Convert to virtual key
		m_pCurrentShortcut->m_bVirtualKey=LOBYTE(VkKeyScan(cMnemonic));
		if (m_pCurrentShortcut->m_bVirtualKey==0)
			return;
		m_pCurrentShortcut->m_bModifiers=CShortcut::ModifierAlt;

        // Turn off scancode		
		m_pCurrentShortcut->m_dwFlags&=~CShortcut::sfVirtualKeyIsScancode;
		CheckDlgButton(IDC_VKISSCANCODE,FALSE);

        
		SetVirtualCode(m_pCurrentShortcut->m_bVirtualKey,FALSE);
		SetHotKeyForShortcut(m_pCurrentShortcut);

		
		CheckDlgButton(IDC_MODCTRL,FALSE);
		CheckDlgButton(IDC_MODALT,TRUE);
		CheckDlgButton(IDC_MODSHIFT,FALSE);
		CheckDlgButton(IDC_MODWIN,FALSE);

		
	}
	else
		m_pCurrentShortcut->m_dwFlags&=~CShortcut::sfUseMemonic;

}


void CSettingsProperties::CKeyboardShortcutsPage::OnAddAction()
{
	ASSERT(m_pCurrentShortcut!=NULL);
	ASSERT(m_nCurrentAction>=0 && m_nCurrentAction<m_pCurrentShortcut->m_apActions.GetSize());
	
	// Saving current fields
	SaveFieldsForAction(m_pCurrentShortcut->m_apActions[m_nCurrentAction]);

    // Creating new action
	m_pCurrentShortcut->m_apActions.Add(new CAction);

	// Activating it
	m_nCurrentAction=m_pCurrentShortcut->m_apActions.GetSize()-1;
	SetFieldsForAction(m_pCurrentShortcut->m_apActions[m_nCurrentAction]);

	EnableItems();
	RefreshShortcutListLabels();

	m_ActionCombo.SetFocus();
}

void CSettingsProperties::CKeyboardShortcutsPage::OnRemoveAction()
{
	ASSERT(m_pCurrentShortcut!=NULL);
	ASSERT(m_pCurrentShortcut->m_apActions.GetSize()>1);
	ASSERT(m_nCurrentAction>=0 && m_nCurrentAction<m_pCurrentShortcut->m_apActions.GetSize());
	
	
	// Delete shortcut
	m_pCurrentShortcut->m_apActions.RemoveAt(m_nCurrentAction);

	// If not the first action, activate previous
	if (m_nCurrentAction>0)
		m_nCurrentAction--;
	SetFieldsForAction(m_pCurrentShortcut->m_apActions[m_nCurrentAction]);

	EnableItems();
	RefreshShortcutListLabels();
}

void CSettingsProperties::CKeyboardShortcutsPage::OnNextAction(BOOL bNext)
{
	ASSERT(m_pCurrentShortcut!=NULL);
	ASSERT(m_nCurrentAction>=0 && m_nCurrentAction<m_pCurrentShortcut->m_apActions.GetSize());
	// If swap with previous, check that not first action
	ASSERT(m_nCurrentAction>0 || bNext);
	// If swap with next, check that not last action
	ASSERT(m_nCurrentAction<m_pCurrentShortcut->m_apActions.GetSize()-1 || !bNext);

	// Save current fields
	SaveFieldsForAction(m_pCurrentShortcut->m_apActions[m_nCurrentAction]);

	// Activate next/previous
	m_nCurrentAction+=bNext?+1:-1;
    SetFieldsForAction(m_pCurrentShortcut->m_apActions[m_nCurrentAction]);

	EnableItems();
}

void CSettingsProperties::CKeyboardShortcutsPage::OnSwapAction(BOOL bWithNext)
{
	ASSERT(m_pCurrentShortcut!=NULL);
	ASSERT(m_nCurrentAction>=0 && m_nCurrentAction<m_pCurrentShortcut->m_apActions.GetSize());
	// If swap with previous, check that not first action
	ASSERT(m_nCurrentAction>0 || bWithNext);
	// If swap with next, check that not last action
	ASSERT(m_nCurrentAction<m_pCurrentShortcut->m_apActions.GetSize()-1 || !bWithNext);

	// Save current fields
	SaveFieldsForAction(m_pCurrentShortcut->m_apActions[m_nCurrentAction]);

	// Swapping
	int m_nAnotherAction=m_nCurrentAction+(bWithNext?+1:-1);
	CAction* pTmp=m_pCurrentShortcut->m_apActions[m_nAnotherAction];
	m_pCurrentShortcut->m_apActions[m_nAnotherAction]=m_pCurrentShortcut->m_apActions[m_nCurrentAction];
	m_pCurrentShortcut->m_apActions[m_nCurrentAction]=pTmp;

	// Set current fields
	SetFieldsForAction(m_pCurrentShortcut->m_apActions[m_nCurrentAction=m_nAnotherAction]);
	
	EnableItems();
}


void CSettingsProperties::CKeyboardShortcutsPage::EnableItems()
{
	int nItem=m_pList->GetNextItem(-1,LVNI_SELECTED);

	// Enable/disable remove button
	EnableDlgItem(IDC_REMOVE,nItem!=-1);
	
	// Enable/disable static group box
	EnableDlgItem(IDC_STATICSHORTCUT,nItem!=-1);
	
	


	// Enable/disable advanced button
	EnableDlgItem(IDC_ADVANCED,nItem!=-1);
	
	// Enable/disable action items
	EnableDlgItem(IDC_STATICACTIONS,nItem!=-1); // Groupbox
	EnableDlgItem(IDC_ACTIONTOOLBAR,nItem!=-1); // Toolbar
	EnableDlgItem(IDC_STATICACTION,nItem!=-1); // Action static text
	m_ActionCombo.EnableWindow(nItem!=-1); // Action combo
	EnableDlgItem(IDC_STATICSUBACTION,nItem!=-1); // Subaction static text
	m_SubActionCombo.EnableWindow(nItem!=-1); // Subaction combo

	ShowState ssVerb=swHide,ssMessage=swHide,ssCommand=swHide,ssWhichFile=swHide;
	ShowState ssChangeValue=swHide,ssItem=swHide,ssContextMenuFor=swHide;
	
	if (nItem!=-1)
	{
	
		ASSERT(m_pCurrentShortcut==(CShortcut*)m_pList->GetItemData(nItem));
		ASSERT(m_nCurrentAction>=0 && m_nCurrentAction<m_pCurrentShortcut->m_apActions.GetSize());

		EnableDlgItem(IDC_FROMMNEMONIC,m_pCurrentShortcut->GetMnemonicForAction(hDialogs)!=0);
		
		BOOL bCodeChecked=IsDlgButtonChecked(IDC_CODERADIO);	
		// Enable/disable shortcut
		EnableDlgItem(IDC_HOTKEYRADIO,!(m_pCurrentShortcut->m_dwFlags&CShortcut::sfUseMemonic));
		EnableDlgItem(IDC_CODERADIO,!(m_pCurrentShortcut->m_dwFlags&CShortcut::sfUseMemonic));
		EnableDlgItem(IDC_SHORTCUTKEY,!bCodeChecked && !(m_pCurrentShortcut->m_dwFlags&CShortcut::sfUseMemonic));
		EnableDlgItem(IDC_CODE,bCodeChecked && !(m_pCurrentShortcut->m_dwFlags&CShortcut::sfUseMemonic));
		EnableDlgItem(IDC_MODCTRL,bCodeChecked && !(m_pCurrentShortcut->m_dwFlags&CShortcut::sfUseMemonic));
		EnableDlgItem(IDC_MODALT,bCodeChecked && !(m_pCurrentShortcut->m_dwFlags&CShortcut::sfUseMemonic));
		EnableDlgItem(IDC_MODSHIFT,bCodeChecked && !(m_pCurrentShortcut->m_dwFlags&CShortcut::sfUseMemonic));
		EnableDlgItem(IDC_MODWIN,bCodeChecked && !(m_pCurrentShortcut->m_dwFlags&CShortcut::sfUseMemonic));

		
		m_pToolBar->EnableButton(IDC_REMOVEACTION,m_pCurrentShortcut->m_apActions.GetSize()>=2);
		m_pToolBar->EnableButton(IDC_NEXT,m_nCurrentAction<=m_pCurrentShortcut->m_apActions.GetSize()-2);
		m_pToolBar->EnableButton(IDC_PREV,m_nCurrentAction>0);
		m_pToolBar->EnableButton(IDC_SWAPWITHNEXT,m_nCurrentAction<=m_pCurrentShortcut->m_apActions.GetSize()-2);
		m_pToolBar->EnableButton(IDC_SWAPWITHPREVIOUS,m_nCurrentAction>0);

		EnableDlgItem(IDC_STATICWHEREPRESSED,
			(m_pCurrentShortcut->m_dwFlags&CShortcut::sfKeyTypeMask)==CShortcut::sfLocal);
		EnableDlgItem(IDC_WHEREPRESSED,
			(m_pCurrentShortcut->m_dwFlags&CShortcut::sfKeyTypeMask)==CShortcut::sfLocal);

		int nOther=m_pList->GetNextItem(nItem,LVNI_ABOVE);
		EnableDlgItem(IDC_UP,nOther!=-1 && nOther!=nItem);
		nOther=m_pList->GetNextItem(nItem,LVNI_BELOW);
		EnableDlgItem(IDC_DOWN,nOther!=-1 && nOther!=nItem);

		switch (GetSelectedAction())
		{
		case CAction::None:
			EnableDlgItem(IDC_STATICSUBACTION,FALSE);
			m_SubActionCombo.EnableWindow(FALSE);
			break;
		case CAction::ResultListItems:
			switch (m_SubActionCombo.GetCurSel())
			{
			case CAction::Execute:
				ssVerb=swShow;
				break;
			case CAction::ExecuteCommand:
                ssCommand=swShow;
				break;
			case CAction::SelectFile:
				ssWhichFile=swShow;
				break;
			case CAction::SelectNthFile:
			case CAction::ExecuteNthFile:
				ssItem=swShow;
				break;
			case CAction::OpenContextMenu:
			case CAction::OpenContextMenuSimple:
				ssContextMenuFor=swShow;
				break;
			}
            break;
		case CAction::Misc:
			switch (m_SubActionCombo.GetCurSel())
			{
			case CAction::SendMessage:
			case CAction::PostMessage:
				ssMessage=swShow;
				break;
			case CAction::ExecuteCommandMisc:
				ssCommand=swShow;
				break;				
			}
			break;
		case CAction::ChangeValue:
			ssChangeValue=swShow;
			break;
		}
	}
	else
	{


		EnableDlgItem(IDC_HOTKEYRADIO,FALSE);
		EnableDlgItem(IDC_CODERADIO,FALSE);
		
		EnableDlgItem(IDC_SHORTCUTKEY,FALSE);
		EnableDlgItem(IDC_CODE,FALSE);
		EnableDlgItem(IDC_MODCTRL,FALSE);
		EnableDlgItem(IDC_MODALT,FALSE);
		EnableDlgItem(IDC_MODSHIFT,FALSE);
		EnableDlgItem(IDC_MODWIN,FALSE);

		EnableDlgItem(IDC_STATICWHEREPRESSED,FALSE);
		EnableDlgItem(IDC_WHEREPRESSED,FALSE);
		EnableDlgItem(IDC_FROMMNEMONIC,FALSE);

		EnableDlgItem(IDC_UP,FALSE);
		EnableDlgItem(IDC_DOWN,FALSE);


	}

	ShowDlgItem(IDC_STATICVERB,ssVerb);
	m_VerbCombo.ShowWindow(ssVerb);
		
	ShowDlgItem(IDC_STATICWINDOW,ssMessage);
	ShowDlgItem(IDC_WINDOW,ssMessage);
	ShowDlgItem(IDC_STATICMESSAGE,ssMessage);
	ShowDlgItem(IDC_MESSAGE,ssMessage);
	ShowDlgItem(IDC_STATICWPARAM,ssMessage);
	ShowDlgItem(IDC_WPARAM,ssMessage);
	ShowDlgItem(IDC_STATICLPARAM,ssMessage);
	ShowDlgItem(IDC_LPARAM,ssMessage);

	ShowDlgItem(IDC_STATICCOMMAND,ssCommand);
	ShowDlgItem(IDC_COMMAND,ssCommand);

	ShowDlgItem(IDC_STATICVALUE,ssChangeValue);
	ShowDlgItem(IDC_VALUE,ssChangeValue);
	ShowDlgItem(IDC_VALUEHELPTEXT,ssChangeValue);

	ShowDlgItem(IDC_STATICWHICHFILE,ssWhichFile);
	m_WhichFileCombo.ShowWindow(ssWhichFile);

	ShowDlgItem(IDC_STATICCONTEXTMENUFOR,ssContextMenuFor);
	m_ContextMenuForCombo.ShowWindow(ssContextMenuFor);

	
	ShowDlgItem(IDC_STATICITEM,ssItem);
	ShowDlgItem(IDC_ITEM,ssItem);
	ShowDlgItem(IDC_ITEMSPIN,ssItem);
	
	

}

void CSettingsProperties::CKeyboardShortcutsPage::OnChangeItem(NMLISTVIEW *pNm)
{
	if (pNm->iItem==-1)
		return;

	if (!(pNm->uChanged&LVIF_STATE))
		return;
	
    if (!(pNm->uOldState&LVIS_SELECTED) &&
		pNm->uNewState&LVIS_SELECTED)
	{
        // Item is selected
		CShortcut* pShortcut=(CShortcut*)m_pList->GetItemData(pNm->iItem);
		if (pShortcut==NULL)
			return;

		SetFieldsForShortcut(m_pCurrentShortcut=pShortcut);
	}

	EnableItems();
}

void CSettingsProperties::CKeyboardShortcutsPage::OnChangingItem(NMLISTVIEW *pNm)
{
	if (pNm->iItem==-1)
		return;

	if (!(pNm->uChanged&LVIF_STATE))
		return;

	
    if (pNm->uOldState&LVIS_SELECTED &&
		!(pNm->uNewState&LVIS_SELECTED))
	{
		// Item is deselected
		CShortcut* pShortcut=(CShortcut*)m_pList->GetItemData(pNm->iItem);
		if (pShortcut==NULL)
			return;
		SaveFieldsForShortcut(pShortcut);
		ClearActionFields();

		m_pList->RedrawItems(pNm->iItem,pNm->iItem);
	}

}

void CSettingsProperties::CKeyboardShortcutsPage::SetFieldsForShortcut(CShortcut* pShortcut)
{
	if (pShortcut->m_dwFlags&CShortcut::sfVirtualKeySpecified)
	{
		CheckDlgButton(IDC_HOTKEYRADIO,FALSE);
		CheckDlgButton(IDC_CODERADIO,TRUE);
	}
	else
	{
		CheckDlgButton(IDC_CODERADIO,FALSE);
		CheckDlgButton(IDC_HOTKEYRADIO,TRUE);
	}

		
	InsertKeysToVirtualKeyCombo();
	SetVirtualCode(pShortcut->m_bVirtualKey,(pShortcut->m_dwFlags&CShortcut::sfVirtualKeyIsScancode)?TRUE:FALSE);
	if (!(pShortcut->m_dwFlags&CShortcut::sfVirtualKeyIsScancode))
		SetHotKeyForShortcut(pShortcut);

	CheckDlgButton(IDC_MODCTRL,pShortcut->m_bModifiers&CShortcut::ModifierControl?1:0);
	CheckDlgButton(IDC_MODALT,pShortcut->m_bModifiers&CShortcut::ModifierAlt?1:0);
	CheckDlgButton(IDC_MODSHIFT,pShortcut->m_bModifiers&CShortcut::ModifierShift?1:0);
	CheckDlgButton(IDC_MODWIN,pShortcut->m_bModifiers&CShortcut::ModifierWin?1:0);

	CheckDlgButton(IDC_FROMMNEMONIC,(pShortcut->m_dwFlags&CShortcut::sfUseMemonic)?TRUE:FALSE);
	SetFieldsRelativeToMnemonics();
	

	m_nCurrentAction=0;
	ASSERT(pShortcut->m_apActions.GetSize()>0);
	SetFieldsForAction(pShortcut->m_apActions[0]);

	ASSERT(m_pWhenPressedList->GetItemCount()>0);

	int nItem=m_pWhenPressedList->GetNextItem(-1,LVNI_ALL);
	while (nItem!=-1)
	{
		if ((pShortcut->m_dwFlags&CShortcut::sfKeyTypeMask)==CShortcut::sfLocal)
		{
			WORD wpMask=(CShortcut::WhenPresssed)m_pWhenPressedList->GetItemData(nItem);
			if (wpMask&CShortcut::wpInvert)
			{
				wpMask&=~CShortcut::wpInvert;
				m_pWhenPressedList->SetCheckState(nItem,(pShortcut->m_wWhenPressed&wpMask)?FALSE:TRUE);
			}
			else
				m_pWhenPressedList->SetCheckState(nItem,(pShortcut->m_wWhenPressed&wpMask)?TRUE:FALSE);
		}
		else
			m_pWhenPressedList->SetCheckState(nItem,TRUE);
			
		nItem=m_pWhenPressedList->GetNextItem(nItem,LVNI_ALL);
	}

}

void CSettingsProperties::CKeyboardShortcutsPage::SaveFieldsForShortcut(CShortcut* pShortcut)
{
	if (IsDlgButtonChecked(IDC_HOTKEYRADIO))
	{
		GetHotKeyForShortcut(pShortcut);
		pShortcut->m_dwFlags&=~(CShortcut::sfVirtualKeySpecified|CShortcut::sfVirtualKeyIsScancode);
	}
	else
	{
		pShortcut->m_dwFlags|=CShortcut::sfVirtualKeySpecified;

		// Set modifiers
		pShortcut->m_bModifiers=0;
		if (IsDlgButtonChecked(IDC_MODCTRL))
			pShortcut->m_bModifiers|=CShortcut::ModifierControl;
		if (IsDlgButtonChecked(IDC_MODALT))
			pShortcut->m_bModifiers|=CShortcut::ModifierAlt;
		if (IsDlgButtonChecked(IDC_MODSHIFT))
			pShortcut->m_bModifiers|=CShortcut::ModifierShift;
		if (IsDlgButtonChecked(IDC_MODWIN))
			pShortcut->m_bModifiers|=CShortcut::ModifierWin;

		
		pShortcut->m_bVirtualKey=GetVirtualCode((pShortcut->m_dwFlags&CShortcut::sfVirtualKeyIsScancode)?TRUE:FALSE);
	}


	ASSERT(m_nCurrentAction>=0 && m_nCurrentAction<pShortcut->m_apActions.GetSize());
	SaveFieldsForAction(pShortcut->m_apActions[m_nCurrentAction]);


	if ((pShortcut->m_dwFlags&CShortcut::sfKeyTypeMask)==CShortcut::sfLocal)
	{
		ASSERT(m_pWhenPressedList->GetItemCount()>0);
		
		int nItem=m_pWhenPressedList->GetNextItem(-1,LVNI_ALL);
		while (nItem!=-1)
		{	
			WORD wpMask=(CShortcut::WhenPresssed)m_pWhenPressedList->GetItemData(nItem);
			if (wpMask&CShortcut::wpInvert)
			{
				wpMask&=~CShortcut::wpInvert;
				if (m_pWhenPressedList->GetCheckState(nItem))
					pShortcut->m_wWhenPressed&=~wpMask;
				else
					pShortcut->m_wWhenPressed|=wpMask;

			}
			else
			{
				if (m_pWhenPressedList->GetCheckState(nItem))
					pShortcut->m_wWhenPressed|=wpMask;
				else
					pShortcut->m_wWhenPressed&=~wpMask;
			}

			
			nItem=m_pWhenPressedList->GetNextItem(nItem,LVNI_ALL);
		}
	}

}

void CSettingsProperties::CKeyboardShortcutsPage::SetFieldsForAction(CAction* pAction)
{
	ASSERT(m_pCurrentShortcut!=NULL);
	ASSERT(m_nCurrentAction>=0 && m_nCurrentAction<m_pCurrentShortcut->m_apActions.GetSize());
	
	// Change groupbox title
	if (m_pCurrentShortcut->m_apActions.GetSize()>1)
	{
		CStringW str;
		str.FormatEx(IDS_SHORTCUTACTION2,m_nCurrentAction+1,m_pCurrentShortcut->m_apActions.GetSize());
		SetDlgItemText(IDC_STATICACTIONS,str);
	}
	else
		SetDlgItemText(IDC_STATICACTIONS,ID2W(IDS_SHORTCUTACTION));

	// Setting action, InsertSubActions does selection of subaction
	m_ActionCombo.SetCurSel(pAction->m_nAction+1);
	InsertSubActions();

	// Clear sub action fields fisrt
	m_VerbCombo.SetCurSel(0);
	SetDlgItemText(IDC_WINDOW,szEmpty);
	SetDlgItemInt(IDC_MESSAGE,0,FALSE);
	SetDlgItemText(IDC_WPARAM,szEmpty);
	SetDlgItemText(IDC_LPARAM,szEmpty);
	SetDlgItemText(IDC_COMMAND,szEmpty);
	SetDlgItemText(IDC_VALUE,szEmpty);
	SendDlgItemMessage(IDC_ITEMSPIN,UDM_SETPOS32,0,1);
	m_WhichFileCombo.SetCurSel(0);
	m_ContextMenuForCombo.SetCurSel(0);


	switch (pAction->m_nAction)
	{
	case CAction::ResultListItems:
		switch (pAction->m_nResultList)
		{
		case CAction::Execute:
			if (pAction->m_szVerb==NULL)
				m_VerbCombo.SetCurSel(0);
			else
			{
				m_VerbCombo.SetCurSel(-1);
				m_VerbCombo.SetText(pAction->m_szVerb);
			}
			break;
		case CAction::ExecuteCommand:
			if (pAction->m_szCommand!=NULL)
				SetDlgItemText(IDC_COMMAND,pAction->m_szCommand);
			break;
		case CAction::SelectFile:
			m_WhichFileCombo.SetCurSel(pAction->m_nSelectFileType);	
			break;
		case CAction::OpenContextMenu:
		case CAction::OpenContextMenuSimple:
			m_ContextMenuForCombo.SetCurSel(pAction->m_nContextMenuFor);	
			break;
		case CAction::SelectNthFile:
		case CAction::ExecuteNthFile:
			SendDlgItemMessage(IDC_ITEMSPIN,UDM_SETPOS32,0,LPARAM(pAction->m_nItem+1));
			break;
		}
		break;
	case CAction::Misc:
		if ((pAction->m_nMisc==CAction::SendMessage || pAction->m_nMisc==CAction::PostMessage) &&
			pAction->m_pSendMessage!=NULL)
		{
			SetDlgItemText(IDC_WINDOW,pAction->m_pSendMessage->szWindow!=NULL?
				pAction->m_pSendMessage->szWindow:szwEmpty);
			SetDlgItemInt(IDC_MESSAGE,pAction->m_pSendMessage->nMessage,FALSE);
			SetDlgItemText(IDC_WPARAM,pAction->m_pSendMessage->szWParam!=NULL?
				pAction->m_pSendMessage->szWParam:szwEmpty);
			SetDlgItemText(IDC_LPARAM,pAction->m_pSendMessage->szLParam!=NULL?
				pAction->m_pSendMessage->szLParam:szwEmpty);
		}
		else if (pAction->m_nMisc==CAction::ExecuteCommandMisc && pAction->m_szCommand!=NULL)
			SetDlgItemText(IDC_COMMAND,pAction->m_szCommand);
		
		break;
	case CAction::Presets:
		if (pAction->m_szPreset!=NULL)
		{
			for (int i=0;i<m_aPossiblePresets.GetSize();i++)
			{
				if (wcscmp(m_aPossiblePresets[i],pAction->m_szPreset)==0)
					m_SubActionCombo.SetCurSel(i);
			}
		}
		break;
	case CAction::ChangeValue:
		if (pAction->m_szValue!=NULL)
			SetDlgItemText(IDC_VALUE,pAction->m_szValue);
		break;
	}
}

void CSettingsProperties::CKeyboardShortcutsPage::SaveFieldsForAction(CAction* pAction)
{
	// Clearing previous extra information
	pAction->ClearExtraInfo();
	
	// Setting action
	pAction->m_nAction=GetSelectedAction();;
	
	switch (pAction->m_nAction)
	{
	case CAction::None:
		pAction->m_nSubAction=0;
		break;
	case CAction::ActivateControl:
		{
			int nCurSel=m_SubActionCombo.GetCurSel();
			if (nCurSel!=CB_ERR)
				pAction->m_nControl=m_pPossibleControlsToActivate[nCurSel];
			else
				pAction->m_nControl=m_pPossibleControlsToActivate[0];
			break;
		}
	case CAction::MenuCommand:
		{
			int nCurSel=m_SubActionCombo.GetCurSel();
			if (nCurSel!=CB_ERR)
				pAction->m_nMenuCommand=m_pPossibleMenuCommands[nCurSel];
			else
				pAction->m_nMenuCommand=m_pPossibleMenuCommands[0];
			break;
		}
	case CAction::ActivateTab:
	case CAction::ShowHideDialog:
	case CAction::Help:
	case CAction::Settings:
		pAction->m_nSubAction=m_SubActionCombo.GetCurSel();
		if ((int)pAction->m_nSubAction==CB_ERR)
			pAction->m_nSubAction=0;
		break;
	case CAction::ResultListItems:
		pAction->m_nResultList=(CAction::ActionResultList)m_SubActionCombo.GetCurSel();
		if ((int)pAction->m_nResultList==CB_ERR)
			pAction->m_nResultList=CAction::Execute;
		else 
		{
			switch (pAction->m_nResultList)
			{
			case CAction::Execute:
				{
					int nSelection=m_VerbCombo.GetCurSel();
					if (nSelection==CB_ERR)
					{
						UINT nLen=(UINT)m_VerbCombo.GetTextLength();
						pAction->m_szVerb=new WCHAR[nLen+1];
						m_VerbCombo.GetText(pAction->m_szVerb,nLen+1);
					}
					else if (nSelection!=0)
					{
						UINT nLen=(UINT)m_VerbCombo.GetLBTextLen(nSelection);
						pAction->m_szVerb=new WCHAR[nLen+1];
						m_VerbCombo.GetLBText(nSelection,pAction->m_szVerb);
					}
					break;
				}
			case CAction::ExecuteCommand:
				{
					// Get command
					UINT nLen=GetDlgItemTextLength(IDC_COMMAND);
					if (nLen>0)
					{
						pAction->m_szCommand=new WCHAR[nLen+1];
						GetDlgItemText(IDC_COMMAND,pAction->m_szCommand,nLen+1);
					}
					break;
				}
			case CAction::OpenContextMenu:
			case CAction::OpenContextMenuSimple:
				pAction->m_nContextMenuFor=(CSubAction::ContextMenuFor)m_ContextMenuForCombo.GetCurSel();
				if (int(pAction->m_nContextMenuFor)==CB_ERR)
					pAction->m_nContextMenuFor=CSubAction::FileOrFolder;
				break;
			case CAction::SelectFile:
				pAction->m_nSelectFileType=(CSubAction::SelectFileType)m_WhichFileCombo.GetCurSel();
				if (int(pAction->m_nSelectFileType)==CB_ERR)
					pAction->m_nSelectFileType=CSubAction::NextFile;
				break;
			case CAction::SelectNthFile:
			case CAction::ExecuteNthFile:
				{
					BOOL bError;
					pAction->m_nItem=(int)SendDlgItemMessage(IDC_ITEMSPIN,UDM_GETPOS32,0,(LPARAM)&bError)-1;
					if (bError)
					{
						pAction->m_nItem=GetDlgItemInt(IDC_ITEM,&bError,FALSE)-1;
						bError=!bError;
					}

					if (bError || pAction->m_nItem<0)
						pAction->m_nItem=0;
					break;
				}
			}
		}
		break;
	case CAction::Misc:
		pAction->m_nMisc=(CAction::ActionMisc)m_SubActionCombo.GetCurSel();
		if ((int)pAction->m_nMisc==CB_ERR)
			pAction->m_nMisc=CAction::SendMessage;

		
		if (pAction->m_nMisc==CAction::SendMessage || 
			pAction->m_nMisc==CAction::PostMessage)
		{
			pAction->m_pSendMessage=new CAction::SendMessageInfo;
			
			// Get message
			BOOL bTranslated;
			pAction->m_pSendMessage->nMessage=GetDlgItemInt(IDC_MESSAGE,&bTranslated,FALSE);
			if (!bTranslated)
				pAction->m_pSendMessage->nMessage=0;
			
			// Get window			
			UINT nLen=GetDlgItemTextLength(IDC_WINDOW);
			if (nLen>0)
			{
				pAction->m_pSendMessage->szWindow=new WCHAR[nLen+1];
				GetDlgItemText(IDC_WINDOW,pAction->m_pSendMessage->szWindow,nLen+1);
			}
		
			// Get WParam
			nLen=GetDlgItemTextLength(IDC_WPARAM);
			if (nLen>0)
			{
				pAction->m_pSendMessage->szWParam=new WCHAR[nLen+1];
				GetDlgItemText(IDC_WPARAM,pAction->m_pSendMessage->szWParam,nLen+1);
			}
			
			// Get LParam
			nLen=GetDlgItemTextLength(IDC_LPARAM);
			if (nLen>0)
			{
				pAction->m_pSendMessage->szLParam=new WCHAR[nLen+1];
				GetDlgItemText(IDC_LPARAM,pAction->m_pSendMessage->szLParam,nLen+1);
			}
		}
		else if (pAction->m_nMisc==CAction::ExecuteCommandMisc)
		{
			// Get command
			UINT nLen=GetDlgItemTextLength(IDC_COMMAND);
			if (nLen>0)
			{
				pAction->m_szCommand=new WCHAR[nLen+1];
				GetDlgItemText(IDC_COMMAND,pAction->m_szCommand,nLen+1);
			}
		}
		break;
	case CAction::Presets:
		{
			pAction->m_nSubAction=m_SubActionCombo.GetCurSel();
			if((int)pAction->m_nSubAction>=0 && (int)pAction->m_nSubAction<m_aPossiblePresets.GetSize())
				pAction->m_szPreset=alloccopy(m_aPossiblePresets[(int)pAction->m_nSubAction]);
			break;
		}
	case CAction::ChangeValue:
		{
			int nCurSel=m_SubActionCombo.GetCurSel();
			if (nCurSel!=CB_ERR)
				pAction->m_nControl=m_pPossibleControlsToChange[nCurSel];
			else
				pAction->m_nControl=m_pPossibleControlsToChange[0];
			
			// Get command
			UINT nLen=GetDlgItemTextLength(IDC_VALUE);
			if (nLen>0)
			{
				pAction->m_szCommand=new WCHAR[nLen+1];
				GetDlgItemText(IDC_VALUE,pAction->m_szValue,nLen+1);
			}
			break;

		}
	default:
		ASSERT(0);
		break;
	
	};

}

void CSettingsProperties::CKeyboardShortcutsPage::ClearActionFields()
{
	// Change groupbox title
	SetDlgItemText(IDC_STATICACTIONS,ID2W(IDS_SHORTCUTACTION));
}


void CSettingsProperties::CKeyboardShortcutsPage::FormatActionLabel(CStringW& str,CAction::Action nAction,
																	UINT uSubAction,void* pExtraInfo) const
{
	// Insert action code
	switch (nAction)
	{
	case CAction::None:
		str.LoadString(IDS_NONE);
		return;
	case CAction::ActivateControl:
		str.LoadString(IDS_ACTIONCATACTIVATECONTROL);
		break;
	case CAction::ActivateTab:
		str.LoadString(IDS_ACTIONCATACTIVATETAB);
		break;
	case CAction::MenuCommand:
		str.LoadString(IDS_ACTIONCATMENUCOMMAND);
		break;
	case CAction::Presets:
		str.LoadString(IDS_ACTIONCATPRESETS);
		break;
	case CAction::ChangeValue:
		str.LoadString(IDS_ACTIONCATCHANGEVALUE);
		break;
	case CAction::Help:
		str.LoadString(IDS_ACTIONCATHELP);
		break;
	case CAction::Settings:
		str.LoadString(IDS_ACTIONCATSETTINGS);
		break;
	case CAction::ShowHideDialog:
	case CAction::ResultListItems:
	case CAction::Misc:
		break;
	}

	// Insert subaction code
	CStringW subaction;
	if (GetSubActionLabel(subaction,nAction,uSubAction,pExtraInfo))
	{
		if (str.IsEmpty())
			str=subaction;
		else
			str << L'/' << subaction;
	}
}

///////////////////////////////////////////////////////////////
// CSettingsProperties::CKeyboardShortcutsPage::CAdvancedDlg

BOOL CSettingsProperties::CKeyboardShortcutsPage::CAdvancedDlg::OnInitDialog(HWND hwndFocus)
{
	CDialog::OnInitDialog(hwndFocus);

	// Adding items to combo
	SendDlgItemMessage(IDC_TYPE,CB_ADDSTRING,0,(LPARAM)(LPCSTR)ID2A(IDS_ADVSHORTCUTLOCAL));
	SendDlgItemMessage(IDC_TYPE,CB_ADDSTRING,0,(LPARAM)(LPCSTR)ID2A(IDS_ADVSHORTCUTGLOBALHOTKEY));

	OSVERSIONINFOEX oi;
	oi.dwOSVersionInfoSize=sizeof(OSVERSIONINFOEX);
	BOOL bRet=GetVersionEx((LPOSVERSIONINFO) &oi);
	if (!(bRet && oi.dwPlatformId!=VER_PLATFORM_WIN32_NT ||
		!(oi.dwMajorVersion>=5 || (oi.dwMajorVersion==4 && oi.wServicePackMajor>=3) )))
		SendDlgItemMessage(IDC_TYPE,CB_ADDSTRING,0,(LPARAM)(LPCSTR)ID2A(IDS_ADVSHORTCUTGLOBALHOOK));

	// Set where
	switch (m_pShortcut->m_dwFlags&CShortcut::sfKeyTypeMask)
	{
	case CShortcut::sfGlobalHotkey:
		SendDlgItemMessage(IDC_TYPE,CB_SETCURSEL,1);
		break;
	case CShortcut::sfGlobalHook:
		SendDlgItemMessage(IDC_TYPE,CB_SETCURSEL,2);
		break;
	default:
		SendDlgItemMessage(IDC_TYPE,CB_SETCURSEL,0);
		break;
	}

	// Set virtual key is scancode
	CheckDlgButton(IDC_VKISSCANCODE,(m_pShortcut->m_dwFlags&CShortcut::sfVirtualKeyIsScancode)?TRUE:FALSE);

	// Where pressed field (works only with global keys)
	if ((m_pShortcut->m_dwFlags&CShortcut::sfKeyTypeMask)!=CShortcut::sfLocal)
	{
		if (m_pShortcut->m_pClass==LPSTR(-1))
			CheckDlgButton(IDC_LOCATEDIALOG,1);
		else
		{
			if (m_pShortcut->m_pTitle!=NULL)
				SetDlgItemText(IDC_WINDOWTITLE,m_pShortcut->m_pTitle);
			if (m_pShortcut->m_pClass!=NULL)
				SetDlgItemText(IDC_WINDOWCLASS,m_pShortcut->m_pClass);
		}
	}		

	
	// Global hook specifig stuff
	if ((m_pShortcut->m_dwFlags&CShortcut::sfKeyTypeMask)==CShortcut::sfGlobalHook)
	{
		if ((m_pShortcut->m_dwFlags&CShortcut::sfExecuteMask)!=CShortcut::sfExecuteWhenDown)
			CheckDlgButton(IDC_EXECUTEWHENKEYISUP,1);
		else
			CheckDlgButton(IDC_EXECUTEWHENKEYISDOWN,1);

		if (m_pShortcut->m_dwFlags&CShortcut::sfRemoveKeyDownMessage)
			CheckDlgButton(IDC_REMOVEDOWNMESSAGE,TRUE);
		if (m_pShortcut->m_dwFlags&CShortcut::sfRemoveKeyUpMessage)
			CheckDlgButton(IDC_REMOVEUPMESSAGE,TRUE);
		if (m_pShortcut->m_dwFlags&CShortcut::sfSendKeyEventBeforeWinRelaseIsHandled)
			CheckDlgButton(IDC_SENDKEYRELEASEBEFOREWIN,TRUE);
	}
	else
	{
		CheckDlgButton(IDC_EXECUTEWHENKEYISDOWN,1);
	}
	

	if (m_pShortcut->m_nDelay==0)
		CheckDlgButton(IDC_WAITNONE,1);
	else if (m_pShortcut->m_nDelay==UINT(-1))
		CheckDlgButton(IDC_WAITPOST,1);
	else
	{
		CheckDlgButton(IDC_WAITDELAY,1);
		SetDlgItemInt(IDC_WAITMS,m_pShortcut->m_nDelay,FALSE);
	}



	EnableItems();
	return FALSE;
}

void CSettingsProperties::CKeyboardShortcutsPage::CAdvancedDlg::OnHelp(LPHELPINFO lphi)
{
		CLocateApp::HelpID id[]= {
		{ IDC_TYPE,"ska_type" },
		{ IDC_VKISSCANCODE,"ska_scancode" },
		{ IDC_STATICWINDOWTITLE,"ska_window" },
		{ IDC_WINDOWTITLE,"ska_window" },
		{ IDC_STATICCLASS,"ska_class" },
		{ IDC_WINDOWCLASS,"ska_class" },
		{ IDC_LOCATEDIALOG,"ska_locatedlg" },
		{ IDC_EXECUTEWHENSTATIC,"ska_execute" },
		{ IDC_EXECUTEWHENKEYISDOWN,"ska_execute" },
		{ IDC_EXECUTEWHENKEYISUP,"ska_execute" },
		{ IDC_REMOVEMESSAGESTATIC,"ska_remove" },
		{ IDC_REMOVEDOWNMESSAGE,"ska_remove" },
		{ IDC_REMOVEUPMESSAGE,"ska_remove" },
		{ IDC_SENDKEYRELEASEBEFOREWIN,"ska_beforewin" },
		{ IDC_STATICWAITBEFOREEXECUTE,"ska_wait" },
		{ IDC_WAITNONE,"ska_wait" },
		{ IDC_WAITNONE,"ska_wait" },
		{ IDC_WAITMS,"ska_wait" },
		{ IDC_WAITPOST,"ska_wait" }
	};
	
	if (CLocateApp::OpenHelp(*this,"settings_shortcutsadvanced.htm",lphi,id,sizeof(id)/sizeof(CLocateApp::HelpID)))
		return;

	
	if (HtmlHelp(HH_HELP_CONTEXT,HELP_SETTINGS_SHORTCUTADVANCED)==NULL)
		HtmlHelp(HH_DISPLAY_TOPIC,0);
}


void CSettingsProperties::CKeyboardShortcutsPage::CAdvancedDlg::EnableItems()
{
	if (SendDlgItemMessage(IDC_TYPE,CB_GETCURSEL)!=0)
	{
		//Type is not local
		EnableDlgItem(IDC_LOCATEDIALOG,TRUE);
		
		BOOL bEnableWindowAndClass=!IsDlgButtonChecked(IDC_LOCATEDIALOG);

		EnableDlgItem(IDC_WINDOWTITLE,bEnableWindowAndClass);
		EnableDlgItem(IDC_WINDOWCLASS,bEnableWindowAndClass);
		EnableDlgItem(IDC_STATICWINDOWTITLE,bEnableWindowAndClass);
		EnableDlgItem(IDC_STATICCLASS,bEnableWindowAndClass);

		EnableDlgItem(IDC_EXECUTEWHENSTATIC,TRUE);
		EnableDlgItem(IDC_EXECUTEWHENKEYISDOWN,TRUE);
		EnableDlgItem(IDC_EXECUTEWHENKEYISUP,TRUE);
		EnableDlgItem(IDC_REMOVEMESSAGESTATIC,TRUE);
		EnableDlgItem(IDC_REMOVEDOWNMESSAGE,TRUE);
		EnableDlgItem(IDC_REMOVEUPMESSAGE,TRUE);

		EnableDlgItem(IDC_SENDKEYRELEASEBEFOREWIN,TRUE);
	}
	else
	{
		EnableDlgItem(IDC_LOCATEDIALOG,FALSE);
		EnableDlgItem(IDC_WINDOWTITLE,FALSE);
		EnableDlgItem(IDC_WINDOWCLASS,FALSE);
		EnableDlgItem(IDC_STATICWINDOWTITLE,FALSE);
		EnableDlgItem(IDC_STATICCLASS,FALSE);

		EnableDlgItem(IDC_EXECUTEWHENSTATIC,FALSE);
		EnableDlgItem(IDC_EXECUTEWHENKEYISDOWN,FALSE);
		EnableDlgItem(IDC_EXECUTEWHENKEYISUP,FALSE);
		EnableDlgItem(IDC_REMOVEMESSAGESTATIC,FALSE);
		EnableDlgItem(IDC_REMOVEDOWNMESSAGE,FALSE);
		EnableDlgItem(IDC_REMOVEUPMESSAGE,FALSE);

		EnableDlgItem(IDC_SENDKEYRELEASEBEFOREWIN,FALSE);
	}

	EnableDlgItem(IDC_WAITMS,IsDlgButtonChecked(IDC_WAITDELAY));
	EnableDlgItem(IDC_VKISSCANCODE,(m_pShortcut->m_dwFlags&CShortcut::sfVirtualKeySpecified)?TRUE:FALSE);

}

BOOL CSettingsProperties::CKeyboardShortcutsPage::CAdvancedDlg::OnCommand(WORD wID,WORD wNotifyCode,HWND hControl)
{
	CDialog::OnCommand(wID,wNotifyCode,hControl);

	switch (wID)
	{
	case IDC_OK:
		OnOK();
		break;
	case IDCANCEL:
	case IDC_CANCEL:
		EndDialog(0);
		break;
	case IDC_LOCATEDIALOG:
	case IDC_WAITNONE:
	case IDC_WAITPOST:
	case IDC_TYPE:
		EnableItems();
		break;
	case IDC_WAITDELAY:
		EnableItems();
		SetFocus(IDC_WAITMS);
		break;
	}	
	return FALSE;
}

BOOL CSettingsProperties::CKeyboardShortcutsPage::CAdvancedDlg::OnClose()
{
	CDialog::OnClose();
	EndDialog(0);
	return FALSE;
}


void CSettingsProperties::CKeyboardShortcutsPage::CAdvancedDlg::OnOK()
{
	// Clear old values
	m_pShortcut->ClearExtraInfo();
	
	// Type
	m_pShortcut->m_dwFlags&=~CShortcut::sfKeyTypeMask;
	switch (SendDlgItemMessage(IDC_TYPE,CB_GETCURSEL))
	{
	case 1:
		m_pShortcut->m_dwFlags|=CShortcut::sfGlobalHotkey;
		break;
	case 2:
		m_pShortcut->m_dwFlags|=CShortcut::sfGlobalHook;
		break;
	default:
		m_pShortcut->m_dwFlags|=CShortcut::sfLocal;
		break;
	}


	if ((m_pShortcut->m_dwFlags&CShortcut::sfKeyTypeMask)!=CShortcut::sfLocal)
	{
		if (IsDlgButtonChecked(IDC_LOCATEDIALOG))
		{
			m_pShortcut->m_pClass=LPSTR(-1);
			m_pShortcut->m_pTitle=NULL;
		}
		else
		{
			CString Text;
			if (GetDlgItemText(IDC_WINDOWTITLE,Text)>0)
				m_pShortcut->m_pTitle=alloccopy(Text,Text.GetLength());
			else
				m_pShortcut->m_pTitle=NULL;

			if (GetDlgItemText(IDC_WINDOWCLASS,Text)>0)
				m_pShortcut->m_pClass=alloccopy(Text,Text.GetLength());
			else
				m_pShortcut->m_pClass=NULL;

		}
	}		
	
	// Global hook specifig stuff
	if ((m_pShortcut->m_dwFlags&CShortcut::sfKeyTypeMask)==CShortcut::sfGlobalHook)
	{
		m_pShortcut->m_dwFlags&=~CShortcut::sfExecuteMask;
		if (IsDlgButtonChecked(IDC_EXECUTEWHENKEYISUP))
			m_pShortcut->m_dwFlags|=CShortcut::sfExecuteWhenUp;


		if (IsDlgButtonChecked(IDC_REMOVEDOWNMESSAGE))
			m_pShortcut->m_dwFlags|=CShortcut::sfRemoveKeyDownMessage;
		else
			m_pShortcut->m_dwFlags&=~CShortcut::sfRemoveKeyDownMessage;
			
		if (IsDlgButtonChecked(IDC_REMOVEUPMESSAGE))
			m_pShortcut->m_dwFlags|=CShortcut::sfRemoveKeyUpMessage;
		else
			m_pShortcut->m_dwFlags&=~CShortcut::sfRemoveKeyUpMessage;

		if (IsDlgButtonChecked(IDC_SENDKEYRELEASEBEFOREWIN))
			m_pShortcut->m_dwFlags|=CShortcut::sfSendKeyEventBeforeWinRelaseIsHandled;
		else
			m_pShortcut->m_dwFlags&=~CShortcut::sfSendKeyEventBeforeWinRelaseIsHandled;
	}
	

	// Setting wait time
	if (IsDlgButtonChecked(IDC_WAITNONE))
		m_pShortcut->m_nDelay=0;
	else if (IsDlgButtonChecked(IDC_WAITPOST))
		m_pShortcut->m_nDelay=UINT(-1);
	else
	{
		BOOL bTranslated;
		m_pShortcut->m_nDelay=GetDlgItemInt(IDC_WAITMS,&bTranslated,FALSE);
		if (!bTranslated)
			m_pShortcut->m_nDelay=0;
	}

	// VK is scancode
	if (IsDlgButtonChecked(IDC_VKISSCANCODE))
		m_pShortcut->m_dwFlags|=CShortcut::sfVirtualKeyIsScancode;
	else
		m_pShortcut->m_dwFlags&=~CShortcut::sfVirtualKeyIsScancode;

	EndDialog(1);
}
