/* Locate32 - Copyright (c) 1997-2010 Janne Huttunen */

#if !defined(SETTINGSDLG_H)
#define SETTINGSDLG_H

#if _MSC_VER >= 1000
#pragma once
#endif 




class CSettingsProperties : public CPropertySheet  
{
public:
	CSettingsProperties(HWND hParent);
	virtual ~CSettingsProperties();

public:
	BOOL LoadSettings();
	BOOL SaveSettings();
	CList<CSchedule*>* GetSchedules() { return &m_Schedules; }

	void SetSettingsFlags(DWORD dwFlags,BOOL bState);
	void SetSettingsFlags(DWORD dwFlags);
	void ClearSettingsFlags(DWORD dwFlags);
	BOOL IsSettingsFlagSet(DWORD dwFlags);
	BOOL IsAllFlagsSet(DWORD dwFlags);

	BOOL GetStartupPath(LPWSTR szPath);

	 
public:
	class CGeneralSettingsPage : public CPropertyPage 
	{
	public:
		CGeneralSettingsPage();
		virtual BOOL OnInitDialog(HWND hwndFocus);
		virtual BOOL OnCommand(WORD wID,WORD wNotifyCode,HWND hControl);
		virtual BOOL OnApply();
		virtual void OnCancel();
		virtual void OnHelp(LPHELPINFO lphi);
				
	protected:
		BYTE OnSystemSettings();
		BYTE OnPointToSelect();
		BYTE OnClickToSelect();
		BYTE OnNeverUnderline();
		BYTE OnPointUnderline();
		BYTE OnAlwaysUnderline();
		
	protected:
		friend CSettingsProperties;
		CSettingsProperties* m_pSettings;
	

	};
	
	class CAdvancedSettingsPage : public COptionsPropertyPage 
	{
	public:
		CAdvancedSettingsPage();
		virtual BOOL OnInitDialog(HWND hwndFocus);
		virtual void OnCancel();
		virtual void OnHelp(LPHELPINFO lphi);
		virtual BOOL OnCommand(WORD wID,WORD wNotifyCode,HWND hControl);
		
		void DoSearch();

	protected:
		friend CSettingsProperties;
		CSettingsProperties* m_pSettings;

		static BOOL CALLBACK TimeFormatComboProc(COptionsPropertyPage::BASICPARAMS* pParams);
		static BOOL CALLBACK DateFormatComboProc(COptionsPropertyPage::BASICPARAMS* pParams);
		static BOOL CALLBACK UpdateThreadPriorityProc(COptionsPropertyPage::BASICPARAMS* pParams);
		static BOOL CALLBACK FileSizeListProc(COptionsPropertyPage::BASICPARAMS* pParams);
		static BOOL CALLBACK SortingMethodProc(COptionsPropertyPage::BASICPARAMS* pParams);
		static BOOL CALLBACK LimitResultsCheckBoxProc(COptionsPropertyPage::BASICPARAMS* pParams); 
		static BOOL CALLBACK UpdateTooltipPositionProc(COptionsPropertyPage::BASICPARAMS* pParams);
		static BOOL CALLBACK UpdateTooltipTopmostProc(COptionsPropertyPage::BASICPARAMS* pParams);
		static BOOL CALLBACK ExternalCommandProc(BASICPARAMS* pParams); 
		static BOOL CALLBACK TrayIconProc(BASICPARAMS* pParams); 
		static BOOL CALLBACK ExecuteItemsModeProc(COptionsPropertyPage::BASICPARAMS* pParams);
		

		static BOOL CALLBACK EnumDateFormatsProc(LPSTR lpDateFormatString);
		static BOOL CALLBACK EnumTimeFormatsProc(LPSTR lpDateFormatString);

		CArrayFAP<LPSTR> m_aBuffer;

	};
	
	class CLanguageSettingsPage : public CPropertyPage 
	{
	public:
		CLanguageSettingsPage();
		virtual BOOL OnInitDialog(HWND hwndFocus);
		virtual BOOL OnApply();
		virtual void OnCancel();
		virtual void OnDestroy();
		virtual BOOL OnNotify(int idCtrl,LPNMHDR pnmh);
		virtual void OnTimer(DWORD wTimerID); 
		virtual void OnHelp(LPHELPINFO lphi);
			
		BOOL ListNotifyHandler(NMLISTVIEW *pNm);
		HRESULT ListCustomDrawHandler(NMLVCUSTOMDRAW* pLVCD);
				
	protected:
		friend CSettingsProperties;
		CSettingsProperties* m_pSettings;

		void FindLanguages();
        
		struct LanguageItem {
			CStringW sLanguage;
			CStringW sFile;
			CStringW sDescription;
			CStringW sForVersion;

			DWORD dwForVersionHi;
			DWORD dwForVersionLo;
		};

	private:
		CListCtrl* m_pList;
		int nLastSel;
		CFont m_fBoldFont;

		DWORD dwVersionHi;
		DWORD dwVersionLo;
	};

	class CDatabasesSettingsPage : public CPropertyPage 
	{
	public:
		CDatabasesSettingsPage();
		
		virtual BOOL OnInitDialog(HWND hwndFocus);
		virtual BOOL OnCommand(WORD wID,WORD wNotifyCode,HWND hControl);
		virtual void OnDestroy();
		virtual BOOL OnApply();
		virtual void OnCancel();
		virtual BOOL OnNotify(int idCtrl,LPNMHDR pnmh);
		virtual void OnHelp(LPHELPINFO lphi);
			
	protected:
        BOOL ListNotifyHandler(NMLISTVIEW *pNm);
		static int CALLBACK ThreadSortProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

		BOOL OnNew(CDatabase* pDatabaseTempl=NULL);
		void OnEdit();
		void OnRemove();
		void OnEnable(BOOL nEnable);
		void OnUpdate();
		void OnRestore();
		void OnThreads();
		void OnImport();
		void OnExport();

		void SetDatabasesToList();
		

		BOOL ItemUpOrDown(BOOL bUp);		
		BOOL IncreaseThread(int nItem,CDatabase* pDatabase,BOOL bDecrease=FALSE);

		void EnableThreadGroups(int nThreadGroups);
		void RemoveThreadGroups();
		void ChangeNumberOfThreads(int nThreads);
	
		void EnableButtons();

		
		class CDatabaseDialog : public CDialog
		{
		public:
			CDatabaseDialog();

			virtual BOOL OnInitDialog(HWND hwndFocus);
			virtual BOOL OnCommand(WORD wID,WORD wNotifyCode,HWND hControl);
			virtual void OnDestroy();
			virtual BOOL OnNotify(int idCtrl,LPNMHDR pnmh);
			virtual BOOL OnClose();
			virtual void OnHelp(LPHELPINFO lphi);
			
			
		
		protected:
			
			void OnOK();
			void OnBrowse();
			void OnAddFolder();
			void OnRemoveFolder();
			void OnAdvanced();

			BOOL ItemUpOrDown(BOOL bUp);
			void EnableControls();
			int AddDriveToList(LPWSTR szDrive);
			int AddDirectoryToListWithVerify(LPCWSTR szPath,INT iLength=-1);
			int AddDirectoryToList(LPCWSTR szPath,int iLength=-1);
			int AddComputerToList(LPCWSTR szName);
			
			int FindPosInRootList(LPCWSTR szDrive);

		public:
			BOOL m_bDontEditName;
			
			CDatabase* m_pDatabase;
			int m_nMaximumNumbersOfThreads;

			// For checking whether name or database file already exist
			CArray<CDatabase*> m_aOtherDatabases; 

		protected:
			CListCtrl* m_pList;

			class CAdvancedDialog : public CDialog
			{
			public:
				CAdvancedDialog(LPCWSTR szIncludedFiles,
					LPCWSTR szIncludedDirectories,
					LPCWSTR szExcludedFiles,
					const CArrayFAP<LPWSTR>& rDirectories,
					const CListCtrl* pCopyItemsFrom,
					LPCWSTR szRootMaps,
					BOOL bExcludeOnlyContentOfDirectories);

				virtual BOOL OnInitDialog(HWND hwndFocus);
				virtual BOOL OnCommand(WORD wID,WORD wNotifyCode,HWND hControl);
				virtual BOOL OnClose();
				virtual void OnDestroy();
				virtual BOOL OnNotify(int idCtrl,LPNMHDR pnmh);
				virtual void OnHelp(LPHELPINFO lphi);
		
				
				void OnOK();
				void OnSet();
				BOOL OnAddFolder(BOOL bShowErrorMessageIfExists);
				void OnRemove();
				void OnBrowse();

				void EnableControlsExclude();
				void EnableControlsAndSetMaps();

			public:
				CStringW m_sIncludedFiles;
				CStringW m_sIncludedDirectories;
				CStringW m_sExcludedFiles;
				CArrayFAP<LPWSTR> m_aExcludedDirectories;
				CStringW m_sRootMaps;
				BOOL m_bExcludeOnlyContentOfDirectories;
			protected:
				CListCtrl* m_pDriveList;
				const CListCtrl* m_pCopyItemsFrom;

			};

		};

		// Threaded database modification date reading
		struct ModificationDateData 
		{
			ModificationDateData() { pText=NULL; di=NULL;}
			~ModificationDateData() { if (pText!=NULL) delete[] pText; if (hThread!=NULL) CloseHandle(hThread); if (di!=NULL) delete di; }

			CStringW sDatabaseFile;
			LPWSTR pText;
			HWND hListWnd;
			int nItem;
			HANDLE hThread;
			CDatabasesSettingsPage* pPage;
			CDatabaseInfo* di;
		};
		BOOL ReadModificationDate(int nItem,CDatabase* pDatabase);
		static DWORD WINAPI ReadModificationDateProc(LPVOID lpParameter);

		CRITICAL_SECTION m_csModDateThreads;
		CArray<ModificationDateData*> m_ModDateThreads;


	protected:
		
		friend CSettingsProperties;
		friend class CSelectDatabasesDlg;

		CSettingsProperties* m_pSettings;
		CListCtrl* m_pList;
		int m_nThreadsCurrently;

	};

	class CAutoUpdateSettingsPage : public CPropertyPage 
	{
	public:
		CAutoUpdateSettingsPage();
		virtual BOOL OnInitDialog(HWND hwndFocus);
		virtual BOOL OnCommand(WORD wID,WORD wNotifyCode,HWND hControl);
		virtual void OnDrawItem(UINT idCtl,LPDRAWITEMSTRUCT lpdis);
		virtual void OnMeasureItem(int nIDCtl,LPMEASUREITEMSTRUCT lpMeasureItemStruct);
		virtual void OnHelp(LPHELPINFO lphi);
		virtual void OnDestroy();
		virtual BOOL OnApply();
		virtual void OnCancel();
		void OnEdit();

		void EnableItems();
				
	public:
		class CCheduledUpdateDlg : public CDialog
		{
		public:
			CCheduledUpdateDlg();

		public:
			virtual BOOL OnInitDialog(HWND hwndFocus);
			virtual BOOL OnCommand(WORD wID,WORD wNotifyCode,HWND hControl);
			virtual BOOL OnNotify(int idCtrl,LPNMHDR pnmh);
			virtual void OnDestroy();
			virtual BOOL OnClose();
			virtual void OnHelp(LPHELPINFO lphi);
			

		private:
			BOOL OnOK();
			BOOL OnTypeChanged();
			BOOL OnDatabases();

		public:
			CSchedule* m_pSchedule;
		
		private:
			CComboBox* m_pTypeCombo;
			BYTE m_bChanged;


		};

	protected:
		friend CSettingsProperties;
		CSettingsProperties* m_pSettings;
		CListBox* m_pSchedules;

	};

	class CKeyboardShortcutsPage : public CPropertyPage 
	{
	protected:
		class CAdvancedDlg: public CDialog  
		{
		public:
			CAdvancedDlg(CShortcut* pShortcut);

			virtual BOOL OnInitDialog(HWND hwndFocus);
			virtual BOOL OnCommand(WORD wID,WORD wNotifyCode,HWND hControl);
			virtual BOOL OnClose();
			virtual void OnHelp(LPHELPINFO lphi);
			
			
			void OnOK();
			void EnableItems();

			CShortcut* m_pShortcut;

		};



	public:
		CKeyboardShortcutsPage();
		virtual ~CKeyboardShortcutsPage();

		virtual BOOL OnInitDialog(HWND hwndFocus);
		virtual BOOL OnApply();
		virtual void OnCancel();
		virtual BOOL OnCommand(WORD wID,WORD wNotifyCode,HWND hControl);
		virtual void OnDestroy();
		virtual BOOL OnNotify(int idCtrl,LPNMHDR pnmh);
		virtual void OnTimer(DWORD wTimerID); 
		virtual void OnHelp(LPHELPINFO lphi);
			
			
		BOOL ListNotifyHandler(NMLISTVIEW *pNm);
		BOOL WherePressedNotifyHandler(NMLISTVIEW *pNm);
		BOOL ItemUpOrDown(BOOL bUp);

		void InsertSubActions();
		void InsertKeysToVirtualKeyCombo();
		void RefreshShortcutListLabels();
		void SetShortcutKeyWhenVirtualKeyChanged();
		void SetVirtualKeyWhenShortcutKeyChanged();
		
		
		void OnNewShortcut();
		void OnRemoveShortcut();
		void OnResetToDefaults();
		void OnAdvanced();
		void OnAddAction();
		void OnRemoveAction();
		void OnNextAction(BOOL bNext);
		void OnSwapAction(BOOL bWithNext);
		
		void OnChangeItem(NMLISTVIEW *pNm);
		void OnChangingItem(NMLISTVIEW *pNm);

		CAction::Action GetSelectedAction() const;
		void SetFieldsForShortcut(CShortcut* pShortcut);
		void SaveFieldsForShortcut(CShortcut* pShortcut);
		void SetFieldsForAction(CAction* pAction);
		void SaveFieldsForAction(CAction* pAction);
		void ClearActionFields();
		void EnableItems();
		void SetFieldsRelativeToMnemonics();
		void InsertShortcuts();


		
		void FormatActionLabel(CStringW& str,CAction::Action nAction,UINT uSubAction,void* pExtraInfo=INVALID_HANDLE_VALUE) const;
		BOOL GetSubActionLabel(CStringW& str,CAction::Action nAction,UINT uSubAction,void* pExtraInfo=INVALID_HANDLE_VALUE) const;
		UINT IndexToSubAction(CAction::Action nAction,UINT nIndex) const;
		UINT SubActionToIndex(CAction::Action nAction,UINT nSubAction) const;

		void SetVirtualCode(BYTE bCode,BOOL bScanCode);
		BYTE GetVirtualCode(BOOL bScanCode) const;
		void SetHotKeyForShortcut(CShortcut* pShortcut);
		void GetHotKeyForShortcut(CShortcut* pShortcut) const;
		
		void SetHotKey(BYTE bKey,BYTE bModifiers);
		




	protected:
		friend CSettingsProperties;
		CSettingsProperties* m_pSettings;

		

	private:
		CListCtrl* m_pList,*m_pWhenPressedList;
		CToolBarCtrl* m_pToolBar;
		CImageList m_ToolBarBitmaps;
		CImageList m_ToolBarBitmapsDisabled;
		CImageList m_ToolBarBitmapsHot;

		CShortcut* m_pCurrentShortcut;
		int m_nCurrentAction;
		
		CAction::ActionControls* m_pPossibleControlsToActivate;
		CAction::ActionControls* m_pPossibleControlsToChange;
		CAction::ActionMenuCommands* m_pPossibleMenuCommands;
		CShortcut::VirtualKeyName* m_pVirtualKeyNames;
		CArrayFAP<LPWSTR> m_aPossiblePresets;

		
		CComboBox m_ActionCombo,m_SubActionCombo,m_VerbCombo,m_WhichFileCombo,m_ContextMenuForCombo;

		HWND hDialogs[5];
		BOOL bFreeDialogs;
		HMENU hMainMenu;
		HMENU hPopupMenu;

	};

public:
	//General
	DWORD m_dwLocateDialogFlags;  // Used with CLocateDlg::LocateDialogFlags
	DWORD m_dwLocateDialogExtraFlags;  // Used with CLocateDlg::LocateDialogExtraFlags
	DWORD m_dwProgramFlags; // Used with CTrayIconWnd::ProgramFlags
	
	DWORD m_nMaximumFoundFiles; // Maximum number of results for normal searches
	
	DWORD m_dwInstantSearchingFlags;  // Used with CLocateDlg::InstantSearchingFlags
	DWORD m_nInstantSearchingLimit; // Maximum number of results for search while typing
	DWORD m_nInstantSearchingDelay; // Delay after typing before searching is started
	DWORD m_nInstantSearchingChars; // Required chars in name before starting searching

	DWORD m_dwThumbnailFlags;  // Used with CLocateDlg::ThumbnailFlags
	DWORD m_dwThumbnailLargeIconSize; // Icon size in "Large icon" mode
	DWORD m_dwThumbnailExtraLargeIconSize; // Icon size in "Large icon" mode


	DWORD m_nNumberOfNames; // Directoried in NameDlg 
	DWORD m_nNumberOfTypes; // Directoried in NameDlg 
	DWORD m_nNumberOfDirectories; // Directoried in NameDlg 
	int  m_nUpdateThreadPriority;
	
	LOGFONT m_lResultListFont;
	

	CStringW m_TimeFormat;
	CStringW m_DateFormat;
	CLocateApp::FileSizeFormats m_nFileSizeFormat;
	DWORD m_dwSortingMethod;
	
	CStringW m_OpenFoldersWith;
	CStringW m_CustomTrayIcon;
	CStringW m_CustomDialogIcon;
	
 	// Database
	CArrayFP<PDATABASE> m_aDatabases;
	
	// Shortcuts
	CArrayFP <CShortcut*> m_aShortcuts;

	// Schedules
	CListFP <CSchedule*> m_Schedules;
	DWORD m_dwSchedulesDelay;
	

	// General settings flags	
	enum SettingsFlags {
		settingsCancelled=0x0001,
		settingsUseLanguageWithConsoleApps=0x0002,
		settingsIsUsedDatabaseChanged=0x0004,
		settingsUseOtherProgramsToOpenFolders=0x0008,
		settingsDatabasesOverridden=0x0010,
		settingsStartLocateAtStartup=0x0020,
		settingsSetTooltipDelays=0x0040,
		settingsUseCustomResultListFont=0x0080,
		settingsDontShowExtensionInRenameDialog=0x0100,
		settingsUseCustomTrayIcon = 0x0200,
		settingsUseCustomDialogIcon = 0x0400,
		settingsSimpleFileMenu=0x800,
		settingsEnableAddRefFix=0x1000,
				
		settingsDefault=settingsUseLanguageWithConsoleApps
	};


	// For m_bAdvancedAndContextMenuFlag
	enum AdvancedAndContextMenuFlags {
		cmLocateOnMyComputer=0x1,
		cmLocateOnMyDocuments=0x2,
		cmLocateOnDrives=0x4,
		cmLocateOnFolders=0x8,
		cmUpdateOnMyComputer=0x10
	};

	// For m_bDefaultFlag
	enum DefaultFlags {
		defaultFileNames=0,
		defaultFileAndFolderNames=1,
		defaultFolderNames=2,
		defaultCheckInFlag=0x3,
		defaultWholeName=0x4,
		defaultDataMatchCase=0x8,
		defaultReplaceSpaces=0x10,
		defaultTextIsCaseSensetive=0x20,
		defaultUseWholePath=0x40,
		defaultMatchCase=0x80,

		defaultDefault=defaultDataMatchCase|defaultFileAndFolderNames
	};
	DWORD m_dwSettingsFlags;
	BYTE m_bAdvancedAndContextMenuFlag;
	BYTE m_bDefaultFlag;
	BYTE m_bSorting;

	CStringW m_strLangFile;
	DWORD m_nTransparency;
    
	// Update tooltip
	DWORD m_nToolTipTransparency;
    COLORREF m_cToolTipTextColor;
    COLORREF m_cToolTipTitleColor;
    COLORREF m_cToolTipErrorColor;
	COLORREF m_cToolTipBackColor;
	LOGFONT m_lToolTipTextFont;
	LOGFONT m_lToolTipTitleFont;

	DWORD m_dwTooltipDelayInitial;
	DWORD m_dwTooltipDelayAutopop;
	

private:
	CGeneralSettingsPage* m_pGeneral;
	CAdvancedSettingsPage* m_pAdvanced;
	CLanguageSettingsPage* m_pLanguage;
	CDatabasesSettingsPage* m_pDatabases;
	CAutoUpdateSettingsPage* m_pAutoUpdate;
	CKeyboardShortcutsPage* m_pKeyboardShortcuts;

	friend CGeneralSettingsPage;


};



#endif 
