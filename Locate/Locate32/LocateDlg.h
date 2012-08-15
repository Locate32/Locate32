/* Locate32 - Copyright (c) 1997-2010 Janne Huttunen */

#if !defined(LOCATEDLG_H)
#define LOCATEDLG_H

#if _MSC_VER >= 1000
#pragma once
#endif 




#define LOCATEDIALOG_TABS	3


////////////////////////////////////////////////////////////
// CLocateDlgThread - Thread class for the main dialog

class CLocateDlgThread : public CWinThread  
{
public:
	CLocateDlgThread();
	
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	virtual BOOL OnThreadMessage(MSG* pMsg);

public:
	CLocateDlg* m_pLocate;
};





////////////////////////////////////////////////////////////
// CLocateDlg - The main dialog

class CLocateDlg : public CDialog  
{
	////////////////////////////////////////////////////////////
	// Flags 
public:
	// First 32 bit set for settings
	enum LocateDialogFlags {
		
		// Dialog
		fgLargeMode=0x00000001,
		fgDialogRememberFields=0x0000002,
		fgDialogMinimizeToST=0x00000004,
		fgDialogCloseMinimizesDialog=0x20000000,
		fgDialogLeaveLocateBackground=0x00000008,
		fgDialogLargeModeOnly=0x40000000,
		fgDialogTopMost=0x80000000,
		fgDialogFlag=0xE000000F,
		fgDialogSave=0xE000000E, // mask to using when saving to registry
				
		// File list
		fgLVShowIcons=0x00000010,
		fgLVShowFileTypes=0x00000020,
		fgLVShowShellType=0x00000080,
		fgLVDontShowHiddenFiles=0x00000040,
		fgLVDontShowTooltips=0x00010000,
		fgLVShowFlag=0x000100F0,
		
		fgLVNoDoubleItems=0x00020000,
		fgLVComputeMD5Sums=0x00040000,
		fgLVFoldersFirst=0x00080000,
		fgLVActivateFirstResult=0x00100000,
		fgLVNoDelayedUpdate=0x00200000,
		fgLVSelectFullRow=0x00400000,
		fgLVAllowInPlaceRenaming=0x00800000,
		

		fgLVUseGetFileTitle=0x00000000,
		fgLVUseOwnMethod=0x00000100,
		fgLVMethodFlag=0x00000100,

		fgLVAlwaysShowExtensions=0x00000000,
		fgLVHideKnownExtensions=0x00000200,
		fgLVNeverShowExtensions=0x00000400,
		fgLV1stCharUpper=0x00000800,
		fgLVExtensionFlag=fgLVAlwaysShowExtensions|fgLVHideKnownExtensions|fgLVNeverShowExtensions,
		
		fgLVStyleSystemDefine=0x00001000,
		fgLVStylePointToSelect=0x00002000,
		fgLVStyleClickToSelect=0x00000000,
		fgLVStyleClickFlag=0x00003000,
		fgLVStyleNeverUnderline=0x00000000,
		fgLVStyleUnderLine=0x00004000,
		fgLVStyleAlwaysUnderline=0x00008000|fgLVStyleUnderLine,
		fgLVStyleUnderlineFlag=0x00008000|fgLVStyleUnderLine,
		fgLVStyleFlag=0x0000F000,
		fgLVFlag=0x00FFFFF0,
		fgLVSave=0x00FFFFF0,
			
		// Name tab
		fgNameMultibleDirectories=0x04000000,
		fgNameDontAddRoots=0x00000000,
		fgNameAddEnabledRoots=0x01000000,
		fgNameAddAllRoots=0x02000000,
		fgNameRootFlag=0x03000000,
		fgNameFlag=0x0F000000,
		fgNameSave=0x0F000000,

		// Time/date/size tab
		fgTimeDateSizeFlag=0x00000000,
		fgTimeDateSizeSave=0x00000000,

		// Advanced tab
		fgLoadRegistryTypes=0x10000000,
		fgAdvancedFlag=0x10000000,
		fgAdvancedSave=0x10000000,

		// other
		fgOtherFlag=0x00000000,
		fgOtherSave=0x00000000,

		fgDefault=fgLVStyleSystemDefine|fgLVUseOwnMethod|fgLVHideKnownExtensions|fgLV1stCharUpper|
			fgLVShowIcons|fgLVShowFileTypes|fgLoadRegistryTypes,
		fgSave=fgDialogSave|fgLVSave|fgNameSave|fgTimeDateSizeSave|fgAdvancedSave|fgOtherSave		// mask to using when saving to registry
	};

	// Another 32 set for settings
	enum LocateDialogExtraFlags {
		
		// Locate process
		efEnableLogicalOperations = 0x00000010,
		efAllowSpacesAsSeparators = 0x00000020,
		efMatchWhileNameIfAsterisks = 0x00000040,
		efAsteriskAtEndEvenIfExtensionExists = 0x00000080,
		efAndModeAlways = 0x00000100,
		efUseLastSelectedDatabases = 0x00000200,
		efLocateProcessDefaults = efEnableLogicalOperations|efAllowSpacesAsSeparators|efAsteriskAtEndEvenIfExtensionExists,
		efLocateProcessSave = 0x000001F0,

		// Locate dialog
		efFocusToResultListWhenAppActivated = 0x01000000,
		efLocateDialogDefault = 0,
		efLocateDialogSave = 0,
		
		// List view (continued)
		efLVIgnoreListClicks = 0x02000000,
		efLVDontMoveTooltips = 0x04000000,
		efLVDontShowDeletedFiles = 0x10000000,
		efLVNoUpdateWhileSorting = 0x20000000,
		efLVRenamingActivated = 0x40000000,
		efLVHeaderInAllViews = 0x80000000,
		efLVDefault = efLVNoUpdateWhileSorting,
		efLVSave = 0xB4000000,

		// Name dialog
		efNameDontSaveNetworkDrivesAndDirectories =  0x00010000,
		efNameDontResolveIconAndDisplayNameForDrives = 0x00020000,
		efNameDontSaveNameTypeAndDirectories = 0x00040000,
		efNameDefault = 0,
		efNameSave = 0x000F0000,

		// Execute mode 
		efExecuteModeDefaultMode = 0,
		efExecuteModeBoldedContextMenuItemMode = 0x00100000,
		efExecuteModeMask = 0x00300000, // Reserving 0x00200000 for extra mode
		efExecuteModeDefaults = 0,
		efExecuteModeSave = efExecuteModeMask,

		// Background operations
		efDisableItemUpdating = 0x00000000,
        efEnableItemUpdating = 0x00000001,
		efItemUpdatingMask = 0x00000001,
		efItemUpdatingSave = 0x00000001,
		
		efDisableFSTracking = 0x00000000,
		efEnableFSTracking  = 0x00000002,
		efEnableFSTrackingOld = 0x00000004|efEnableFSTracking,
		efTrackingMask = efEnableFSTracking|efEnableFSTrackingOld,
		efTrackingSave = efEnableFSTracking|efEnableFSTrackingOld,
		
		efBackgroundDefault = efEnableItemUpdating|efEnableFSTracking,
		efBackgroundSave = efItemUpdatingSave|efTrackingSave,

		efDefault = efLocateProcessDefaults|efLocateDialogDefault|efLVDefault|efNameDefault|efExecuteModeDefaults|efBackgroundDefault,
		efSave = efLocateProcessSave|efLocateDialogSave|efLVSave|efNameSave|efExecuteModeSave|efBackgroundSave
	};

protected:
	DWORD m_dwFlags;
	DWORD m_dwExtraFlags;

public:
	// Accessors
	DWORD GetFlags() const { return m_dwFlags; }
	BOOL IsFlagSet(LocateDialogFlags nFlag) const { return m_dwFlags&nFlag?1:0; }
	void AddFlags(DWORD dwFlags) { m_dwFlags|=dwFlags; }
	void RemoveFlags(DWORD dwFlags) { m_dwFlags&=~dwFlags; }
	
	DWORD GetExtraFlags() const { return m_dwExtraFlags; }
	BOOL IsExtraFlagSet(LocateDialogExtraFlags nFlag) const { return m_dwExtraFlags&nFlag?1:0; }
	void AddExtraFlags(DWORD dwFlags) { m_dwExtraFlags|=dwFlags; }
	void RemoveExtraFlags(DWORD dwFlags) { m_dwExtraFlags&=~dwFlags; }





	////////////////////////////////////////////////////////////
	// Classes for tabs
public:
	class CNameDlg : public CDialog  
	{
	public:
		enum TypeOfItem
		{
			Everywhere=0,
			Special=1,
			Drive=2,
			Custom=3,
			Root=4,
			NotSelected=0xFFFF // This is used to indicate that any 
							   // item in LookIn combo is not selected
		};

		enum SpecialType
		{
			Documents=1,
			Desktop=2,
			Personal=3,
			MyComputer=4
		};

		enum EveryWhereType
		{
			Original=0,
			RootTitle=1
		};
	private:
		class DirSelection {
		public:
			DirSelection(BYTE bSelected);
			~DirSelection();

			void FreeData();
			void SetValuesFromControl(HWND hControl,const CStringW* pBrowse,int nBrowseDirs);
			LPARAM GetLParam(const CStringW* pBrowse,int nBrowseDirs) const;
			

			CLocateDlg::CNameDlg::TypeOfItem nType;
			union {
				SpecialType nSpecialType;
				EveryWhereType nEverywhereType;
				char cDriveLetter;
			};
			LPWSTR pTitleOrDirectory;
			BYTE bSelected:1;
		};


	public:
			
		CNameDlg();
		virtual ~CNameDlg();
		
		virtual BOOL OnInitDialog(HWND hwndFocus);
		virtual BOOL OnCommand(WORD wID,WORD wNotifyCode,HWND hControl);
		virtual void OnDestroy();
		virtual void OnSize(UINT nType, int cx, int cy);	
		virtual BOOL OnNotify(int idCtrl,LPNMHDR pnmh);
	
	public:
		BOOL InitDriveBox(BYTE nFirstTime=FALSE);
		void ChangeNumberOfItemsInLists(int iNumberOfNames,int iNumberOfTypes,int iNumberOfDirectories);
		
	private:
		// Get name, extensions and directories
		BOOL GetNameExtensionsAndDirectories(CStringW& sName,CArray<LPWSTR>& aExtensions,
			CArrayFAP<LPWSTR>& aDirectories,BOOL bForInstantSearch);
		
		

		void OnClear(BOOL bInitial=FALSE);

		void OnMoreDirectories();
		void OnLookInNewSelection();
		void OnLookInRemoveSelection();
		void OnLookInSelection(int nSelection);
		void OnLookInNextSelection(BOOL bNext);
		void LookInChangeSelection(int nCurrentSelection,int nNewSelection);


		BOOL IsChanged();
		void HilightTab(BOOL bHilight);

		BOOL SetPath(LPCWSTR szPath);

		void EnableItems(BOOL bEnable=TRUE);
		void SetStartData(const CLocateApp::CStartData* pStartData,DWORD& rdwChanged);
		
		BOOL EnableMultiDirectorySupport(BOOL bEnable);
		BOOL SelectByLParam(LPARAM lParam);
		BOOL SelectByRootName(LPCWSTR szDirectory);
		BOOL SelectByCustomName(LPCWSTR szDirectory);
		DWORD GetCurrentlySelectedComboItem() const;

		BOOL GetDirectoriesForActiveSelection(CArray<LPWSTR>& aDirectories,BOOL bInstantSearch,TypeOfItem* pType=NULL,BOOL bNoWarningIfNotExists=FALSE);
		BOOL GetDirectoriesFromCustomText(CArray<LPWSTR>& aDirectories,LPCWSTR szCustomText,DWORD dwLength,BOOL bSaveAndSet,BOOL bNoWarningIfNotExists=FALSE);
		BOOL GetDirectoriesFromLParam(CArray<LPWSTR>& aDirectories,LPARAM lParam);
		BOOL GetDirectoriesForNonActiveSelection(CArray<LPWSTR>& aDirectories,const DirSelection* pSelection,BOOL bNoWarnings=FALSE);

		
		// Do various checks (database contains directory, is a drive, ...) and saves directory to Look In list
		BOOL CheckAndAddDirectory(LPCWSTR pFolder,DWORD dwLength,BOOL bSaveAndSet=TRUE,BOOL bNoWarning=FALSE);
		
			
		// Checks szDirectory if multiple directories is specifieds and adds directories to aDirectories
		static void ParseGivenDirectoryForMultipleDirectories(CArray<LPWSTR>& aDirectories,LPCWSTR szDirectory);
		static void ParseGivenDirectoryForMultipleDirectories(CArray<LPWSTR>& aDirectories,LPCWSTR szDirectory,DWORD sLength);
		
		void AddCurrentNameToList();
		void AddCurrentTypeToList();


		static WORD ComputeChecksumFromDir(LPCWSTR szDir);

		void LoadControlStates(CRegKey& RegKey);
		void SaveControlStates(CRegKey& RegKey);

	
	protected:
		
		void OnBrowse();
		void SaveRegistry() const;
		void LoadRegistry();


		// Subclassing "Named:" control
		struct NameControlData {
			WNDPROC pOldWndProc;
			CComboBox* pNameCombo;
			CLocateDlg* pLocateDlg;			
		};
		static LRESULT CALLBACK NameWindowProc(HWND hWnd,UINT uMessage,
			WPARAM wParam,LPARAM lParam);
		static BOOL CALLBACK EnumAndSubclassNameChilds(HWND hwnd,LPARAM lParam);

	
	private:
		WORD m_nFieldLeft;
		WORD m_nButtonWidth;
		WORD m_nCheckWidth;
		WORD m_nBrowseTop;
		WORD m_nMoreDirsTop;
		BYTE m_nMoreDirsWidth;
		
		DWORD m_nMaxNamesInList;
		DWORD m_nMaxTypesInList;

	private:
		DWORD m_nMaxBrowse;
		CStringW* m_pBrowse;
		mutable CRITICAL_SECTION m_cBrowse;

		DirSelection** m_pMultiDirs;
		
		BOOL bStopDebug;


	public:
		
		CComboBox m_Name;
		CComboBox m_Type;
		CComboBoxEx m_LookIn;

		friend CLocateDlg;

	};

	class CSizeDateDlg : public CDialog  
	{
	public:
		CSizeDateDlg();
		
		virtual BOOL OnInitDialog(HWND hwndFocus);
		virtual BOOL OnCommand(WORD wID,WORD wNotifyCode,HWND hControl);
		
	public:
		BOOL SetSizesAndDaterForLocater(CLocater* pLocater);
		void OnClear(BOOL bInitial=FALSE);
		
		void EnableItems(BOOL bEnable);
		BOOL LookOnlyFiles() const;
		void SetStartData(const CLocateApp::CStartData* pStartData,DWORD& rdwChanged);

		void LoadControlStates(CRegKey& RegKey,BOOL bPreset);
		void SaveControlStates(CRegKey& RegKey);

		BOOL IsChanged();
		void HilightTab(BOOL bHilight);



	};

	class CAdvancedDlg : public CDialog  
	{
	private:
		class CReplaceCharsDlg: public CDialog  
		{
		public:
			CReplaceCharsDlg(CArrayFAP<LPWSTR>& raChars,BOOL bUseQuestionMark);

			virtual BOOL OnInitDialog(HWND hwndFocus);
			virtual BOOL OnCommand(WORD wID,WORD wNotifyCode,HWND hControl);
			virtual BOOL OnClose();
			virtual void OnDestroy();

			BOOL UseQuestionMark() const { return m_bUseQuestionMark; }
		private:
			CArrayFAP<LPWSTR>& m_raChars;
			BOOL m_bUseQuestionMark;

		};

	public:
		CAdvancedDlg();
		
		virtual BOOL OnInitDialog(HWND hwndFocus);
		virtual BOOL OnCommand(WORD wID,WORD wNotifyCode,HWND hControl);
		virtual void OnSize(UINT nType, int cx, int cy);	
		virtual void OnDrawItem(UINT idCtl,LPDRAWITEMSTRUCT lpdis);
		virtual void OnMeasureItem(int nIDCtl,LPMEASUREITEMSTRUCT lpMeasureItemStruct);
		virtual void OnDestroy();
		virtual LRESULT WindowProc(UINT msg,WPARAM wParam,LPARAM lParam);
		
		BOOL IsChanged();
		void HilightTab(BOOL bHilight);

		void RenameReplaceSpaces();
		BOOL LoadRegistry();
		BOOL SaveRegistry();
		void ReplaceCharsWithAsterisks(CStringW& sString);

	public:
		// Return codes for OnOk
		enum {
			flagMatchWholeNameOnly=0x1,
			flagReplaceSpaces=0x2,
			flagMatchCase=0x4
		};
		DWORD SetAdvancedFlagsForLocater(CLocater* pLocater,BOOL bForInstantSearch);
		void OnClear(BOOL bInitial=FALSE);
		
		void EnableItems(BOOL bEnable);

		void ChangeEnableStateForCheck();
		void UpdateTypeList();
		void AddBuildInFileTypes();
		void SetStartData(const CLocateApp::CStartData* pStartData,DWORD& rdwChanged);

		static DWORD WINAPI UpdaterProc(CAdvancedDlg* pAdvancedDlg);
	
		void LoadControlStates(CRegKey& RegKey);
		void SaveControlStates(CRegKey& RegKey);


		// Subclassing Type control
		struct TypeControlData {
			WNDPROC pOldWndProc;
			CAdvancedDlg* pParent;
		};
		static LRESULT CALLBACK TypeWindowProc(HWND hWnd,UINT uMessage,
			WPARAM wParam,LPARAM lParam);
	

	private:
		struct FileType
		{
			WCHAR* szExtensions;
			WCHAR* szTitle;
			WCHAR* szType;
			DWORD dwExtensionLength;
			
			HICON hIcon;
			LPWSTR szIconPath;
			

			FileType();
			FileType(LPWSTR frType,LPWSTR frTitle); // This constructod owns pointers
			FileType(LPCWSTR& szBuildIn,HIMAGELIST hImageList);
			~FileType();
			
			void AddExtension(LPCWSTR szExtension,DWORD dwExtensionLength);
			void SetIcon(CRegKey& rKey,BOOL toHandle=FALSE);
			void ExtractIconFromPath();

		};

		int AddTypeToList(LPCWSTR szKey,CArray<FileType*>& aFileTypes);
		int AddTypeToList(BYTE* pTypeAndExtensions);
		
	private:
		HANDLE m_hTypeUpdaterThread;
		HICON m_hDefaultTypeIcon;
		CImageList m_ToolbarIL,m_ToolbarILHover,m_ToolbarILDisabled;

		friend FileType;
		friend CLocateDlg;

		WORD m_nMatchCaseWidth;
		WORD m_nMatchCaseTop;
		WORD m_nCheckPos;
		
		enum {
			fgBuildInTypesAdded=0x1,
			fgOtherTypeAdded=0x2,
			fgReplaceUseQuestionMark=0x4,

			fgDefault=0,
			fgSave=fgReplaceUseQuestionMark
		};
		BYTE m_dwFlags;

		CArrayFAP<LPWSTR> m_aReplaceChars;

		CSubAction* m_aResultListActions[TypeCount];


	};

private:

	
	////////////////////////////////////////////////////////////
	// Classes for miscellaneous dialogs
	
	class CSavePresetDlg: public ::CSavePresetDlg  
	{
	public:
		virtual void OnOK();
		virtual BOOL OnInitDialog(HWND hwndFocus);

	};

	class CRemovePresetDlg: public CDialog  
	{
	public:
		CRemovePresetDlg();

		virtual BOOL OnInitDialog(HWND hwndFocus);
		virtual BOOL OnCommand(WORD wID,WORD wNotifyCode,HWND hControl);
		virtual BOOL OnClose();
		virtual void OnDestroy();
	
		
		void OnOK();
	};

	class CDeletePrivateData: public CDialog  
	{
	public:
		CDeletePrivateData();

		virtual void OnOK();
		virtual BOOL OnClose();
		virtual BOOL OnCommand(WORD wID,WORD wNotifyCode,HWND hControl);
		virtual BOOL OnInitDialog(HWND hwndFocus);
		virtual void OnDestroy();
	
		enum {
			clearNamed=0x1,
			clearExtensions=0x2,
			clearLookIn=0x4
		};

		DWORD m_dwFlags;
	};

	////////////////////////////////////////////////////////////
	// Initialization / deinitialization
public:
	CLocateDlg();
	virtual ~CLocateDlg();

	// Command line is parsed to pStartData
	void SetStartData(const CLocateApp::CStartData* pStartData); 
	void SaveRegistry();
	void LoadRegistry();
	BOOL UpdateSettings(); // Called when Settings dialog is closed

protected:
	// General variables
	IShellFolder* m_pDesktopFolder;




	////////////////////////////////////////////////////////////
	// Virtual message handlers
public:
	virtual void OnActivate(WORD fActive,BOOL fMinimized,HWND hwnd);
	virtual void OnActivateApp(BOOL fActive,DWORD dwThreadID); 
	virtual void OnChangeCbChain(HWND hWndRemove,HWND hWndAfter );
	virtual BOOL OnClose();
	virtual BOOL OnCommand(WORD wID,WORD wNotifyCode,HWND hControl);
	virtual void OnContextMenu(HWND hWnd,CPoint& pos);
	virtual void OnDestroy();
	virtual void OnDrawClipboard();
	virtual void OnDrawItem(UINT idCtl,LPDRAWITEMSTRUCT lpdis);
	virtual void OnHelp(LPHELPINFO lphi);
	virtual BOOL OnInitDialog(HWND hwndFocus);
	virtual void OnInitMenuPopup(HMENU hPopupMenu,UINT nIndex,BOOL bSysMenu);
	virtual void OnMeasureItem(int nIDCtl,LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual BOOL OnNotify(int idCtrl,LPNMHDR pnmh);
	virtual void OnSize(UINT nType, int cx, int cy);	
	virtual void OnTimer(DWORD wTimerID); 
	virtual LRESULT WindowProc(UINT msg,WPARAM wParam,LPARAM lParam);



	////////////////////////////////////////////////////////////
	// Events
protected:
	// Dialog buttons
	void OnOk(BOOL bShortcut,BOOL bForceSelectDatabases);
	void OnStop();
	void OnNewSearch();
	BOOL OnPresets();
	

	// Menu events
	void OnAutoArrange();
	void OnAlignToGrid();
	void OnRefresh();
	void OnSettings() { GetTrayIconWnd()->OnSettings(); }
	void OnSettingsTool();
	void OnDeletePrivateData();
	BOOL OnProperties(int nItem=1);
	void OnRemoveFromThisList();
	void OnSelectAll();
	void OnInvertSelection();
	void OnSaveResults(BOOL bClipboard=FALSE);
	void OnSelectDetails();
	void OnCopyPathToClipboard(BOOL bShortPath);
	void OnChangeFileName();
	void OnChangeFileNameCase();
	void OnUpdateLocatedItem();
	void OnComputeMD5Sums(BOOL bForSameSizeFilesOnly);
	void OnCopyMD5SumsToClipboard();
	void OnShowFileInformation();
	void OnRemoveDeletedFiles();
	void OnUpdateDatabasesOfSelectedFiles();
	BOOL OnRenameFile(int nItem=-1);
	BOOL OnCopy(BOOL bCut,int nItem=-1);
	BOOL OnCreateShortcut();
	enum DeleteFlag {Recycle = 0,Delete = 1,BasedOnShift = 2};
	BOOL OnDelete(DeleteFlag DeleteFlag=BasedOnShift,int nItem=-1);
	
		
	////////////////////////////////////////////////////////////
	// Dialog, tabs and items
protected:
	void SetDialogMode(BOOL bLarge,BOOL bForceRepositionIfMaximized=FALSE);
	void EnableItems(BOOL bEnable=TRUE);
	void LoadDialogIcon();
	void SetControlPositions(UINT nType,int cx, int cy); // OnSize calls this to resize/reposition controls
	void SetVisibleWindowInTab();
	void OnActivateTab(int nIndex);
	void OnActivateNextTab(BOOL bPrev=FALSE);
	void HilightTab(int nTab,int nID,BOOL bHilight);
	BOOL SetPath(LPCWSTR szPath) { return m_NameDlg.SetPath(szPath); }
public:
	void SetDialogTransparency();
	int GetCurrentTab() const { return m_pTabCtrl->GetCurSel(); }
	
	// Variables
protected:
	CTabCtrl* m_pTabCtrl;
	CStatusBarCtrl* m_pStatusCtrl;
	CNameDlg m_NameDlg;
	CSizeDateDlg m_SizeDateDlg;
	CAdvancedDlg m_AdvancedDlg;

	// Fonts and icons
	HFONT m_hDialogFont;
	HICON m_hLargeDialogIcon;
	HICON m_hSmallDialogIcon;

	// Dialog and button sizes
	WORD m_nMaxYMinimized;
	WORD m_nMaxYMaximized;
	DWORD m_nLargeY;
	BYTE m_nButtonWidth;
	char m_nPresetsButtonOffset;
	BYTE m_nPresetsButtonWidth;
	BYTE m_nPresetsButtonHeight;
	BYTE m_nButtonSpacing;
	WORD m_nTabbedDialogHeight;
	BYTE m_nTabHeaderHeight;

	// Control having last focus
	HWND m_hLastFocus;




	////////////////////////////////////////////////////////////
	// Tooltip support
protected:
	void InitTooltips();
	void DeleteTooltipTools();

protected:
	// Variables
	CToolTipCtrl* m_pListTooltips;
	int m_iTooltipItem;
	int m_iTooltipSubItem;
	BOOL m_bTooltipActive;
	



	


	////////////////////////////////////////////////////////////
	// Presets
protected:
	BOOL OnPresetsSave();
	BOOL OnPresetsSelection(int nPreset);
	BOOL LoadPreset(LPCWSTR szPreset);
	static DWORD CheckExistenceOfPreset(LPCWSTR szName,DWORD* pdwPresets); // Returns index to preset or FFFFFFFF
	void LoadDialogTexts();
	void SaveDialogTexts();




	////////////////////////////////////////////////////////////
	// Menu related functions
protected:
	class ContextMenuInformation {
	public:
		ContextMenuInformation();
		~ContextMenuInformation();
		
		void ReleaseShellInterfaces();
		BOOL AddContextMenuItems(UINT uFirstID,UINT uLastID,UINT uFlags);
		
		operator HMENU() const { return hPopupMenu; }

	public:
        IContextMenu* pContextMenu;
		IContextMenu2* pContextMenu2;
		IContextMenu3* pContextMenu3;
		IShellFolder* pParentFolder;

		int nIDLParentLevel;
		BOOL bForParents;

		HMENU hPopupMenu;
		BOOL bFreeMenu; // Free menu when deleted if not file menu
	
		
	public:
#ifdef _DEBUG
		void CheckThread() { ASSERT(m_dwThreadId==GetCurrentThreadId()); }
	private:
		DWORD m_dwThreadId;
#else
		void CheckThread() {  }
#endif



	};

protected:
	// General
	void SetMenus();

	// Initializations
	void OnInitMainMenu(HMENU hPopupMenu,UINT nIndex);
	void OnInitFileMenu(HMENU hPopupMenu);
	void OnInitSendToMenu(HMENU hPopupMenu);
	
	// Context menu/file menu helpers
	BOOL CreateFileContextMenu(HMENU hFileMenu,CLocatedItem** pSelectedItems,int nSelectedItems,BOOL bSimple=FALSE,BOOL bForParents=FALSE);
	BOOL GetContextMenuForItems(ContextMenuInformation* pContextMenuInfo,int nItems,CLocatedItem** ppItems);
	BOOL GetContextMenuForFiles(ContextMenuInformation* pContextMenuInfo,int nItems,LPITEMIDLIST pParentIDL,LPITEMIDLIST* ppSimpleIDLs,int nParentIDLlevel=-1);
	void ClearMenuVariables();
	void DeleteImpossibleItemsInContextMenu(CLocatedItem** pItems,int nItems);
	
	BOOL HandleContextMenuCommand(WORD wID);
	BOOL HandleShellCommands(WORD wID);
	BOOL HandleSendToCommand(WORD wID);
	
	// Send To menu helpers
	UINT AddSendToMenuItems(CMenu& Menu,LPITEMIDLIST sIDListToPath,UINT wStartID);
	static void FreeSendToMenuItems(HMENU hMenu);
	static BOOL IsSendToMenu(HMENU hMenu);
	static int GetSendToMenuPos(HMENU hMenu);
	
	// Misc funtions
	static BOOL InsertMenuItemsFromTemplate(CMenu& Menu,HMENU hTemplate,UINT uStartPosition,int nDefaultItem=-1);
	
	// Variables
protected:
	CMenu m_Menu;
	ContextMenuInformation* m_pActiveContextMenu;
	HFONT m_hSendToListFont;
	CBitmap m_CircleBitmap; // Used in menu
	



	////////////////////////////////////////////////////////////
	// Results list
protected:
	// List style and outlook
	void SetResultListFont();
	BOOL ResolveSystemLVStatus();
	BOOL SetListSelStyle();
	void RemoveResultsFromList();


protected:
	// Viewing items
	BOOL ListNotifyHandler(NMLISTVIEW *pNm);
	HRESULT ListCustomDrawHandler(NMLVCUSTOMDRAW* pLVCD);
	
protected:
	// Sorting
	void SortItems(DetailType nDetail,BYTE bDescending=-1,BOOL bNoneIsPossible=FALSE); // bDescending:0=ascending order, 1=desc, -1=default
	void SetSorting(BYTE bSorting=BYTE(-2)); // bSorting==BYTE(-2): default
	static int CALLBACK ListCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	static int SortNewItem(CListCtrl* pList,CLocatedItem* pNewItem,BYTE bSorting);
	void SetSortArrowToHeader(DetailType nDetail,BOOL bRemove,BOOL bDownArrow);
	void SetListCompareFunction();

	// Drag and drop
	void BeginDragFiles(CListCtrl* pList);
	
	// Results list actions
	void LoadResultlistActions();
	void SaveResultlistActions();
	void ClearResultlistActions();
	void SetDefaultActions(CSubAction*** pActions) const;

	BOOL OnExecuteResultAction(CAction::ActionResultList m_nResultAction,void* pExtraInfo,int nItem=-1,DetailType nDetail=Name);
	BOOL OnExecuteFile(LPCWSTR szVerb,int nItem=-1);
			
	
	// Helpers
	BOOL OpenFolder(LPCWSTR szFolder,LPCWSTR szSelectedFile=NULL);
	BOOL OpenSelectedFolder(BOOL bContaining,int nItem=-1,BOOL bForParents=FALSE);
	CLocatedItem** GetSelectedItems(int& nItems,int nIncludeIfNoneSelected=-1);
	void MakeItemSelected(int nItem);
public:
	static LPCWSTR GetDBVolumeLabel(WORD wDB,WORD wRootID);
	static LPCWSTR GetDBVolumeSerial(WORD wDB,WORD wRootID);
	static LPCWSTR GetDBVolumeFileSystem(WORD wDB,WORD wRootID);
	
	static void SetSystemImageLists(CListCtrl* pList,HICON* phIcon=NULL);
	enum ImageListSize { ilMedium, ilLarge, ilExtraLarge };
	BOOL LoadSystemImageList(ImageListSize& iImageList,SIZE& rsIconSize);
	
	
	
public:
	// Default details
	struct ViewDetails {
		/* DetailType nDetail; */
		int nString;
		BOOL bShow;
		int nAlign;
		int nWidth;
	};
    static ViewDetails* GetDefaultDetails();

	// Variables
protected:
	CListCtrlEx* m_pListCtrl;
	CFileTarget* m_pDropTarget; // Drop target class for the results list
	
	BYTE m_nSorting;	// used for sorting list item, 0-6 bits: detail type, 7 bit: if 1 ascend sorting
	int (STDAPICALLTYPE * m_pStrCmp)(LPCWSTR,LPCWSTR); 	// Function used to compare strings, can be _wcsicmp, StrCmpLogicalW or strnatcasecmp

	



	BYTE m_ClickWait;   // If m_ClickWait is TRUE, result list actions are ignored
						// this is set to TRUE when item in the results list is double
						// clicked and timer sets it to FALSE after 500 ms
					
	WORD m_WaitEvery30; // Delay (in ms) after 30 items are added to the retults list
	WORD m_WaitEvery60; // Delay (in ms) after 60 items are added to the retults list

public:
	// List type
	enum ListType {
		ltSmallIcons = 0,
		ltMediumIcons = 1,
		ltLargeIcons = 2,
		ltExtraLargeIcons = 3,
		ltList = 4,
		ltDetails = 5,
	} m_nCurrentListType;
protected:
	void SetListType(ListType nType,BOOL bResetIfSame=FALSE);
	void SetMenuCheckMarkForListType();

	// Thumbnail support:
	enum ThumbnailFlags {
		tfVistaFeaturesAvailable=0x1,
		tfSystemImageListIsInterface=0x2,
		tfThumbnailsInMediumIcons=0x4,

		tfDefault = tfThumbnailsInMediumIcons,
		tfSave = tfThumbnailsInMediumIcons
	};
	DWORD m_dwThumbnailFlags;

	
	DWORD GetThumbnailFlags() const { return m_dwThumbnailFlags; }
	BOOL IsThumbnailFlagSet(ThumbnailFlags nFlag) const { return m_dwThumbnailFlags&nFlag?1:0; }
	void AddThumbnailFlags(DWORD dwFlags) { m_dwThumbnailFlags|=dwFlags; }
	void RemoveThumbnailFlags(DWORD dwFlags) { m_dwThumbnailFlags&=~dwFlags; }



	// System image list
	union { 
		IImageList* m_pSystemImageList; // Large and extra large icons (SHGetImageList needed)
		HIMAGELIST m_hSystemImageList; // Medium size icons (got using SHGetFileInfo)
	};
	SIZE m_sSystemImageList;
	
	// Custom image list
	CImageList m_ilOwnImageList; // The list is used only to set correct icon size if 
	SIZE m_sCurrentIconSize;    // m_hSystemImageList of that size is not available 
	UINT m_nBorders;


	
	// Results list mouse actions
	enum ResultListAction {
		LeftMouseButtonClick = 0,
		LeftMouseButtonDblClick = 1,
		RightMouseButtonClick = 2,
		RightMouseButtonDblClick = 3,
		MiddleMouseButtonClick = 4,
		MiddleMouseButtonDblClick = 5,

		ListActionCount = 6
	};
	CSubAction* m_aResultListActions[TypeCount][ListActionCount];

	// For volume serial and label information 
	struct VolumeInformation {
		WORD wDB;
		WORD wRootID;
		LPWSTR szVolumeSerial;
		LPWSTR szVolumeLabel;
		LPWSTR szFileSystem;
		BYTE bType;

		VolumeInformation(WORD wDB,WORD wRootID,BYTE bType,DWORD dwVolumeSerial,LPCWSTR szVolumeLabel,LPCWSTR szFileSystem);
		~VolumeInformation();
	};
	CArrayFP<VolumeInformation*> m_aVolumeInformation;

public:
	// GDI+ support for image list (reading image size)
	struct ImageHandlerDll {
		ImageHandlerDll();
		~ImageHandlerDll();
		
		BOOL IsLoaded();
		HMODULE hModule;
		IH_GETIMAGEDIMENSIONSW pGetImageDimensionsW;
		ULONG_PTR uToken;
	};
	ImageHandlerDll* m_pImageHandler;

	
	
	
	////////////////////////////////////////////////////////////
	// Keyboard shortcuts
protected:
	void SetShortcuts();
	void ClearShortcuts();
	
protected:
	// Variables
	CArrayFP<CShortcut*> m_aShortcuts;
	// if WM_COMMAND with wID-IDM_DEFSHORTCUTITEM is got,
	// m_aActiveShortcuts[wID-IDM_DEFSHORTCUTITEM] is NULL terminated list 
	// to shortcuts which should be executed
	CArrayFAP<CShortcut**> m_aActiveShortcuts; 

	




	////////////////////////////////////////////////////////////
	// DB Updater / Locaters related stuff
protected:
	CLocater* ResolveParametersAndInitializeLocater(CArrayFAP<LPWSTR>& aExtensions,CArrayFAP<LPWSTR>& aDirectories,
													CArrayFAP<LPWSTR>& aNames,BOOL bForInstantSearch,BOOL bShowDatabasesDialog);

	BOOL IsLocating() const { return m_pLocater!=NULL; }

public:
	static BOOL CALLBACK LocateProc(DWORD_PTR dwParam,CallingReason crReason,UpdateError ueCode,DWORD_PTR dwFoundFiles,const CLocater* pLocater);
	static BOOL CALLBACK LocateInstantProc(DWORD_PTR dwParam,CallingReason crReason,UpdateError ueCode,DWORD_PTR dwFoundFiles,const CLocater* pLocater);
	static BOOL CALLBACK LocateFoundProc(DWORD_PTR dwParam,BOOL bFolder,const CLocater* pLocater);
	static BOOL CALLBACK LocateFoundProcW(DWORD_PTR dwParam,BOOL bFolder,const CLocater* pLocater);

	
	BOOL StartLocateAnimation();
	BOOL StopLocateAnimation();
	BOOL StartUpdateAnimation();
	BOOL StopUpdateAnimation();
	BOOL IsDatabaseUsedInSearch(WORD wID) const;
	
protected:
	DWORD GetMaxFoundFiles() const { return m_dwMaxFoundFiles; }
	void SetMaxFoundFiles(DWORD dwValue) { m_dwMaxFoundFiles=dwValue; }


	// Variables
protected:
	DWORD m_dwMaxFoundFiles;
	CLocater* m_pLocater;
	
	HICON* m_pUpdateAnimBitmaps;
	WORD m_nCurUpdateAnimBitmap;
	CRITICAL_SECTION m_csUpdateAnimBitmaps;

	HICON* m_pLocateAnimBitmaps;
	WORD m_nCurLocateAnimBitmap;
	CRITICAL_SECTION m_csLocateAnimBitmaps;

	CArray<WORD> m_DatabasesUsedInSearch;
	

	////////////////////////////////////////////////////////////
	// Background operations
protected:
	void StartBackgroundOperations();
	void ChangeBackgroundOperationsPriority(BOOL bLower);
	BOOL CheckClipboard();

protected:
	// Variables
	CCheckFileNotificationsThread* m_pFileNotificationsThread;
	CBackgroundUpdater* m_pBackgroundUpdater;
	HWND m_hNextClipboardViewer;


	

	////////////////////////////////////////////////////////////
	// Instant search related stuff
public:
	enum InstantSearchingFlags {
		isEnable = 0x000000001,
		isSearching = 0x000000002, 
		isRunning = 0x00000004,
		isDisableIfDataSearch	= 0x00000008,
		isIgnoreChangeMessages  = 0x00000010,
		isUpDownGoesToResults   = 0x00000020,
		isUpdateTitle			= 0x00000040,

		isGeneralDefault		= isEnable | isDisableIfDataSearch | isUpdateTitle,
		isGeneralSave			= isEnable | isDisableIfDataSearch | isUpDownGoesToResults | isUpdateTitle,


		isNameChanged			= 0x00010000,
		isTypeChanged			= 0x00020000,
		isLookInChanged			= 0x00040000,
		isSizesChanged			= 0x00080000,
		isDatesChanged			= 0x00100000,
		isDataChanged			= 0x00200000,
		isOtherChanged			= 0x00400000,
		isAllChanged  			= 0x00FF0000,
		
		isByCommandLine			= 0x01000000,
		
		isSearchIfDefault	    = isNameChanged|isTypeChanged|isLookInChanged|isSizesChanged|isDatesChanged|isDataChanged|isOtherChanged,
		isSearchIfSave		    = 0x01FF0000,
		

		isDefault =	isGeneralDefault | isSearchIfDefault,
		isSave = isGeneralSave | isSearchIfSave

	};

protected:
	DWORD m_dwInstantFlags;
	DWORD m_dwInstantLimit;
	DWORD m_dwInstantDelay;
	DWORD m_dwInstantChars;

	void OnFieldChange(DWORD dwWhatChanged); // Used to initialize "instant searches"
	void InstantSearch();
	void CancelInstantSearch();

public:
	DWORD GetInstantSearchingFlags() const { return m_dwInstantFlags; }
	BOOL IsInstantSearchingFlagSet(InstantSearchingFlags nFlag) const { return m_dwInstantFlags&nFlag?1:0; }
	void AddInstantSearchingFlags(DWORD dwFlags) { m_dwInstantFlags|=dwFlags; }
	void RemoveInstantSearchingFlags(DWORD dwFlags) { m_dwInstantFlags&=~dwFlags; }

	DWORD GetInstantSearchingLimit() const { return m_dwInstantLimit; }
	void SetInstantSearchLimit(DWORD dwValue) { m_dwInstantLimit=dwValue; }

	DWORD GetInstantSearchingDelay() const { return m_dwInstantDelay; }
	void SetInstantSearchDelay(DWORD dwValue) { m_dwInstantDelay=dwValue; }

	DWORD GetInstantSearchingChars() const { return m_dwInstantChars; }
	void SetInstantSearchChars(DWORD dwValue) { m_dwInstantChars=dwValue; }




	////////////////////////////////////////////////////////////
	// Misc helpers
protected:
	static BOOL ExecuteCommand(LPCWSTR szCommand,int nItem=-1);
	// Returns IDropTarget for item id list
	IDropTarget* GetDropTarget(LPITEMIDLIST pidl) const;

	// Thumbnail support
	IExtractImage* GetExtractImageInterface(LPCWSTR szFile) const;
	IThumbnailProvider* GetThumbnailProvider(LPCWSTR szFile) const;
	
public:
	HANDLE GetLocaterThread(BOOL bDuplicate=FALSE);
	UINT GetSimpleIDLsandParentForFiles(LPCWSTR* ppFiles,UINT nFiles,LPITEMIDLIST& rpParentIDL,LPITEMIDLIST*& rpSimpleIDLs);
	BOOL GetSimpleIDLsandParentfromIDLs(int nItems,LPITEMIDLIST* pFullIDLs,LPITEMIDLIST* rpParentIDL,LPITEMIDLIST* rpSimpleIDLs,int* pParentIDLLevel=NULL);
	
	
	HBITMAP CreateThumbnail(LPCWSTR szFile,SIZE* pDesiredSize,SIZE* pActualSize=NULL) const;


	////////////////////////////////////////////////////////////
	// Debug related stuff
#ifdef _DEBUG
public:
	static LRESULT CALLBACK DebugWindowProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
#endif



	////////////////////////////////////////////////////////////
	// Friends
	friend class CCheckFileNotificationsThread;
	friend class CBackgroundUpdater;
	friend class CSelectColumnsDlg;
	friend class CSubAction;
	friend class CFileTarget;
	friend class CSettingsProperties;
	friend class CSettingsProperties::CKeyboardShortcutsPage;
	friend BOOL CTrayIconWnd::TurnOnShortcuts();
	friend BOOL CLocateDlgThread::OnThreadMessage(MSG* pMsg);
	friend class CLocatedItem;
	
};

inline CLocateDlg* GetLocateDlg()
{
	extern CLocateApp theApp;
	
	if (theApp.m_AppWnd.GetLocateDlgThread()==NULL)
		return NULL;
	return theApp.m_AppWnd.GetLocateDlgThread()->m_pLocate;
}


#endif
