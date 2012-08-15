/* Locate32 - Copyright (c) 1997-2010 Janne Huttunen */

#ifndef SHORTCUT_H
#define SHORTCUT_H

// Actions
class CSubAction {
public:
	CSubAction();
	CSubAction(DWORD nSubAction);
	CSubAction(DWORD nAction,CSubAction& rCopyFrom);

	void GetCopyFrom(DWORD nAction,CSubAction& rCopyFrom);	
	void ClearExtraInfo(DWORD nAction);

	enum ActionControls { // First is control to be activated, second is for memonics
		NullControl = 0,
		
		// Next and previous control
		NextControl = MAKELONG(IDC_NEXTCONTROL,~IDS_KEYNEXTCONTROL),
		PrevControl = MAKELONG(IDC_PREVCONTROL,~IDS_KEYPREVCONTROL),

		// Dialog itself
		FindNow = MAKELONG(IDC_OK,IDC_OK),
		Stop = MAKELONG(IDC_STOP,IDC_STOP),
		NewSearch = MAKELONG(IDC_NEWSEARCH,IDC_NEWSEARCH),
		ResultList = MAKELONG(IDC_FILELIST,~IDS_KEYRESULTLIST),
		Presets = MAKELONG(IDC_PRESETS,IDC_PRESETS),

		// Name and Location
		Name = MAKELONG(IDC_NAME,IDC_NAMESTATIC),
		Type = MAKELONG(IDC_TYPE,IDC_TYPESTATIC),
        LookIn = MAKELONG(IDC_LOOKIN,IDC_LOOKINSTATIC),
		MoreDirectories = MAKELONG(IDC_MOREDIRECTORIES,~IDS_KEYMOREDIRECTORIES),
		NoSubdirectories = MAKELONG(IDC_NOSUBDIRECTORIES,IDC_NOSUBDIRECTORIES),
		Browse = MAKELONG(IDC_BROWSE,IDC_BROWSE),
		
		// Size and Data
		MinimumSize = MAKELONG(IDC_CHECKMINIMUMSIZE,IDC_CHECKMINIMUMSIZE),
		MinimumSizeValue = MAKELONG(IDC_MINIMUMSIZE,~IDS_KEYMINSIZEVALUE),
		MinimumSizeType = MAKELONG(IDC_MINSIZETYPE,~IDS_KEYMINSIZETYPE),
		MaximumSize = MAKELONG(IDC_CHECKMAXIMUMSIZE,IDC_CHECKMAXIMUMSIZE),
		MaximumSizeValue = MAKELONG(IDC_MAXIMUMSIZE,~IDS_KEYMAXSIZEVALUE),
		MaximumSizeType = MAKELONG(IDC_MAXSIZETYPE,~IDS_KEYMAXSIZETYPE),
		MinimumDate = MAKELONG(IDC_CHECKMINDATE,IDC_CHECKMINDATE),
		MinimumDateValue = MAKELONG(IDC_MINDATE,~IDS_KEYMINDATEVALUE),
		MinimumDateMode = MAKELONG(IDC_MINDATEMODE,~IDS_KEYMINDATEMODE),
		MinimumDateType = MAKELONG(IDC_MINTYPE,~IDS_KEYMINDATETYPE),
		MaximumDate = MAKELONG(IDC_CHECKMAXDATE,IDC_CHECKMAXDATE),
		MaximumDateValue = MAKELONG(IDC_MAXDATE,~IDS_KEYMAXDATEVALUE),
		MaximumDateMode = MAKELONG(IDC_MAXDATEMODE,~IDS_KEYMAXDATEMODE),
		MaximumDateType = MAKELONG(IDC_MAXTYPE,~IDS_KEYMAXDATETYPE),
		
		// Advanced
		CheckFilesOrFolders = MAKELONG(IDC_CHECK,IDC_CHECKSTATIC),
		MatchWholeName = MAKELONG(IDC_MATCHWHOLENAME,IDC_MATCHWHOLENAME),
		MatchCase = MAKELONG(IDC_MATCHCASE,IDC_MATCHCASE),
		ReplaceSpaces = MAKELONG(IDC_REPLACESPACES,IDC_REPLACESPACES),
		UseWholePath = MAKELONG(IDC_USEWHOLEPATH,IDC_USEWHOLEPATH),
		TypeOfFile = MAKELONG(IDC_FILETYPE,IDC_FILETYPESTATIC),
		ContainingText = MAKELONG(IDC_CONTAINDATACHECK,IDC_CONTAINDATACHECK),
		ContainingTextValue = MAKELONG(IDC_CONTAINDATA,~IDS_KEYCONTAINDATA),
		TextMatchCase = MAKELONG(IDC_DATAMATCHCASE,IDC_DATAMATCHCASE),
		TextHelp = MAKELONG(IDC_HELPTOOLBAR,~IDS_KEYTEXTHELPBUTTON)
	};

	static ActionControls* GetPossibleControlValuesToActivate() {
		ActionControls a[]={NextControl,PrevControl,
			FindNow,Stop,NewSearch,ResultList,
			Presets,Name,Type,LookIn,MoreDirectories,NoSubdirectories,Browse,
            MinimumSize,MinimumSizeValue,MinimumSizeType,
			MaximumSize,MaximumSizeValue,MaximumSizeType,
			MinimumDate,MinimumDateValue,MinimumDateMode,MinimumDateType,
			MaximumDate,MaximumDateValue,MaximumDateMode,MaximumDateType,
			CheckFilesOrFolders,MatchWholeName,ReplaceSpaces,
            UseWholePath,TypeOfFile,ContainingText,ContainingTextValue,
			TextMatchCase,NullControl};
		ActionControls* b=new ActionControls[sizeof(a)/sizeof(ActionControls)];
		CopyMemory(b,a,sizeof(a));
		return b;
	}

	static ActionControls* GetPossibleControlValuesToChange() {
		ActionControls a[]={
			Name,Type,LookIn,NoSubdirectories,
            MinimumSize,MinimumSizeValue,MinimumSizeType,
			MaximumSize,MaximumSizeValue,MaximumSizeType,
			MinimumDate,MinimumDateValue,MinimumDateType,
			MaximumDate,MaximumDateValue,MaximumDateType,
			CheckFilesOrFolders,MatchWholeName,ReplaceSpaces,
            UseWholePath,TypeOfFile,ContainingText,ContainingTextValue,
			TextMatchCase,TextHelp,NullControl};
		ActionControls* b=new ActionControls[sizeof(a)/sizeof(ActionControls)];
		CopyMemory(b,a,sizeof(a));
		return b;
	}

	enum ActionMenuCommands { // First is menu identifier, second is specifies menu
		NullMenuCommand = 0,
		FileOpenContainingFolder = MAKELONG(IDM_OPENCONTAININGFOLDER,IDS_SHORTCUTMENUFILEITEM),
		FileRemoveFromThisList = MAKELONG(IDM_REMOVEFROMTHISLIST,IDS_SHORTCUTMENUFILEITEM),
        FileCreateShortcut = MAKELONG(IDM_CREATESHORTCUT,IDS_SHORTCUTMENUFILEITEM),
		FileDelete = MAKELONG(IDM_DELETE,IDS_SHORTCUTMENUFILEITEM),
		FileRename = MAKELONG(IDM_RENAME,IDS_SHORTCUTMENUFILEITEM),
		FileProperties = MAKELONG(IDM_PROPERTIES,IDS_SHORTCUTMENUFILEITEM),
		FileSaveResults = MAKELONG(IDM_SAVERESULT,IDS_SHORTCUTMENUFILENOITEM),
		FileFindUsingDatabase = MAKELONG(IDM_FINDUSINGDBS,IDS_SHORTCUTMENUFILENOITEM),
		FileUpdateDatabases = MAKELONG(IDM_GLOBALUPDATEDB,IDS_SHORTCUTMENUFILENOITEM),
		FileUpdateSelectedDatabase = MAKELONG(IDM_UPDATEDATABASES,IDS_SHORTCUTMENUFILENOITEM),
		FileStopUpdating = MAKELONG(IDM_STOPUPDATING,IDS_SHORTCUTMENUFILENOITEM),
		FileDatabaseInfo = MAKELONG(IDM_DATABASEINFO,IDS_SHORTCUTMENUFILENOITEM),
		FileClose = MAKELONG(IDM_CLOSE,IDS_SHORTCUTMENUFILENOITEM),
		FileExit = MAKELONG(IDM_EXIT,IDS_SHORTCUTMENUFILENOITEM),
		
		SpecialCopyPathToClibboard = MAKELONG(IDM_COPYPATHTOCB,IDS_SHORTCUTMENUSPECIAL),
		SpecialCopyShortPathToClibboard = MAKELONG(IDM_COPYSHORTPATHTOCB,IDS_SHORTCUTMENUSPECIAL),
		SpecialCopyDataToClipboard = MAKELONG(IDM_COPYDATATOCB,IDS_SHORTCUTMENUSPECIAL),
		SpecialChangeFileName = MAKELONG(IDM_CHANGEFILENAME,IDS_SHORTCUTMENUSPECIAL),
        SpecialChangeCase = MAKELONG(IDM_CHANGECASE,IDS_SHORTCUTMENUSPECIAL),
		SpecialShowFileInformation = MAKELONG(IDM_SHOWFILEINFORMATION,IDS_SHORTCUTMENUSPECIAL),
		SpecialForceUpdate = MAKELONG(IDM_FORCEUPDATE,IDS_SHORTCUTMENUSPECIAL),
		SpecialComputeMD5Sum = MAKELONG(IDM_COMPUTEMD5SUM,IDS_SHORTCUTMENUSPECIAL),
		SpecialComputeMD5SumsForSameSizeFiles = MAKELONG(IDM_MD5SUMSFORSAMESIZEFILES,IDS_SHORTCUTMENUSPECIAL),
		SpecialCopyMD5SumsToClipboard = MAKELONG(IDM_COPYMD5SUMTOCLIPBOARD,IDS_SHORTCUTMENUSPECIAL),
		SpecialRemoveDeletedFiles = MAKELONG(IDM_REMOVEDELETEDFILES,IDS_SHORTCUTMENUSPECIAL),
		SpecialUpdateDatabasesOfSelectedFiles = MAKELONG(IDM_UPDATEDATABASESOFSELECTEDFILES,IDS_SHORTCUTMENUSPECIAL),

		EditCut = MAKELONG(IDM_CUT,IDS_SHORTCUTMENUEDIT),
		EditCopy = MAKELONG(IDM_COPY,IDS_SHORTCUTMENUEDIT),
		EditPaste = MAKELONG(IDM_PASTE,IDS_SHORTCUTMENUEDIT),
		EditSelectAll = MAKELONG(IDM_EDIT_SELECTALL,IDS_SHORTCUTMENUEDIT),
		EditInvertSelection = MAKELONG(IDM_EDIT_INVERTSELECTION,IDS_SHORTCUTMENUEDIT),

		ViewExtraLargeIcons = MAKELONG(IDM_EXTRALARGEICONS,IDS_SHORTCUTMENUVIEW),
		ViewLargeIcons = MAKELONG(IDM_LARGEICONS,IDS_SHORTCUTMENUVIEW),
		ViewMediumIcons = MAKELONG(IDM_MEDIUMICONS,IDS_SHORTCUTMENUVIEW),
		ViewSmallIcons = MAKELONG(IDM_SMALLICONS,IDS_SHORTCUTMENUVIEW),
		ViewList = MAKELONG(IDM_LIST,IDS_SHORTCUTMENUVIEW),
		ViewDetails = MAKELONG(IDM_DETAILS,IDS_SHORTCUTMENUVIEW),
		ViewArrangeIconsByName = MAKELONG(IDM_ARRANGENAME,IDS_SHORTCUTMENUVIEWARRANGEICONS),
		ViewArrangeIconsByFolder = MAKELONG(IDM_ARRANGEFOLDER,IDS_SHORTCUTMENUVIEWARRANGEICONS),
		ViewArrangeIconsByType = MAKELONG(IDM_ARRANGETYPE,IDS_SHORTCUTMENUVIEWARRANGEICONS),
		ViewArrangeIconsByDate = MAKELONG(IDM_ARRANGESIZE,IDS_SHORTCUTMENUVIEWARRANGEICONS),
		ViewArrangeIconsBySize = MAKELONG(IDM_ARRANGEDATE,IDS_SHORTCUTMENUVIEWARRANGEICONS),
		ViewArrangeIconsByExtension = MAKELONG(IDM_ARRANGEEXTENSION,IDS_SHORTCUTMENUVIEWARRANGEICONS),
		ViewArrangeIconsAutoArrange = MAKELONG(IDM_AUTOARRANGE,IDS_SHORTCUTMENUVIEWARRANGEICONS),
		ViewArrangeIconsAlignToGrid = MAKELONG(IDM_ALIGNTOGRID,IDS_SHORTCUTMENUVIEWARRANGEICONS),
		ViewLineUpIcons = MAKELONG(IDM_LINEUPICONS,IDS_SHORTCUTMENUVIEW),
		ViewSelectDetails= MAKELONG(IDM_SELECTDETAILS,IDS_SHORTCUTMENUVIEW),
		ViewRefresh = MAKELONG(IDM_REFRESH,IDS_SHORTCUTMENUVIEW),

		ToolsDeletePrivateData = MAKELONG(IDM_DELETEPRIVATEDATA,IDS_SHORTCUTMENUTOOLS),
		ToolsSettings = MAKELONG(IDM_SETTINGS,IDS_SHORTCUTMENUTOOLS),
		ToolsSettingsTool = MAKELONG(IDM_SETTOOL,IDS_SHORTCUTMENUTOOLS),

		HelpTopics = MAKELONG(IDM_HELPTOPICS,IDS_SHORTCUTMENUHELP),
		HelpAbout = MAKELONG(IDM_ABOUT,IDS_SHORTCUTMENUHELP),

		PresetSave = MAKELONG(IDM_PRESETSAVE,IDS_SHORTCUTMENUPRESETS),
		PresetRemove = MAKELONG(IDM_PRESETREMOVE,IDS_SHORTCUTMENUPRESETS),

		MultidirNextSelection = MAKELONG(IDM_LOOKINNEXTSELECTION,IDS_SHORTCUTMENUDIRECTORIES),
		MultidirPrevSelection = MAKELONG(IDM_LOOKINPREVSELECTION,IDS_SHORTCUTMENUDIRECTORIES),
		MultidirNewDirectory = MAKELONG(IDM_LOOKINNEWSELECTION,IDS_SHORTCUTMENUDIRECTORIES),
		MultidirRemoveSelection = MAKELONG(IDM_LOOKINREMOVESELECTION,IDS_SHORTCUTMENUDIRECTORIES)
		
	};

	static ActionMenuCommands* GetPossibleMenuCommands() {
		ActionMenuCommands a[]={FileOpenContainingFolder,FileRemoveFromThisList,
			FileCreateShortcut,FileRename,FileDelete,FileProperties,FileSaveResults,
            FileFindUsingDatabase,FileUpdateDatabases,FileUpdateSelectedDatabase,
            FileStopUpdating,FileDatabaseInfo,FileClose,FileExit,SpecialCopyPathToClibboard,
			SpecialCopyShortPathToClibboard,SpecialCopyDataToClipboard,
			SpecialChangeFileName,SpecialChangeCase,
			SpecialShowFileInformation,SpecialForceUpdate,SpecialComputeMD5Sum,
			SpecialComputeMD5SumsForSameSizeFiles,SpecialCopyMD5SumsToClipboard,
			SpecialRemoveDeletedFiles,SpecialUpdateDatabasesOfSelectedFiles,
			EditCut,EditCopy,EditPaste,EditSelectAll,EditInvertSelection,ViewExtraLargeIcons,
			ViewLargeIcons,ViewMediumIcons,ViewSmallIcons,
            ViewList,ViewDetails,ViewArrangeIconsByName,ViewArrangeIconsByFolder,
            ViewArrangeIconsByType,ViewArrangeIconsByDate,ViewArrangeIconsBySize,
			ViewArrangeIconsByExtension,ViewArrangeIconsAutoArrange,
			ViewArrangeIconsAlignToGrid,ViewLineUpIcons,
			ViewSelectDetails,ViewRefresh,
			ToolsDeletePrivateData,ToolsSettings,ToolsSettingsTool,
			HelpTopics,HelpAbout,PresetSave,PresetRemove,MultidirNextSelection,MultidirPrevSelection,
			MultidirNewDirectory,MultidirRemoveSelection,NullMenuCommand};
		ActionMenuCommands* b=new ActionMenuCommands[sizeof(a)/sizeof(ActionMenuCommands)];
		CopyMemory(b,a,sizeof(a));
		return b;
	}

	enum ActionActivateTabs {
		NameAndLocationTab = 0,
		SizeAndDataTab = 1,
		AdvancedTab = 2,
		NextTab = 3,
		PrevTab = 4,

		ActivateTabsLast = PrevTab
	};

	enum ActionShowHideDialog {
		ShowDialog = 0,
		MinimizeDialog = 1,
		CloseDialog = 2,
		ShowOrHideDialog = 3,
		OpenOrCloseDialog = 4,
		RestoreDialog = 5,
		MaximizeDialog = 6,
		MaximizeOrRestoreDialog = 7,
		ShowOpenOrHideDialog = 8,
		ShowDialogAndGetDirFromExplorer = 9,
		StopLocatingOrCloseWindow = 10,

		ShowHideDialogLast = StopLocatingOrCloseWindow
	};

	enum ActionResultList { 
		Execute = 0,
		Copy = 1,
		Cut = 2,
		MoveToRecybleBin = 3,
		DeleteFile = 4,
		OpenContextMenu = 5,
		OpenContextMenuSimple = 6,
		OpenFolder = 7,
		OpenContainingFolder = 8,
		Properties = 9,
		ShowSpecialMenu = 10,			
		ExecuteCommand = 11,
		SelectFile = 12,
		RenameFile = 13,
		SelectNthFile = 14,
		ExecuteNthFile = 15,
		
		ResultListLast = ExecuteNthFile
	};

	
		

	enum ActionMisc {
		SendMessage = 0,
		PostMessage = 1,
		ExecuteCommandMisc = 2,
		InsertAsterisks = 3,
		
		MiscLast = InsertAsterisks
	};

	enum ActionHelp {
		HelpShowHelp = 0,
		HelpCloseHelp = 1,
		HelpShowTopics = 2,
		HelpIndex = 3,
		HelpTOC = 4,
		HelpSearch = 5,

		HelpLast = HelpSearch
	};

	enum ActionSettings {
		OpenGeneralSettings = 0,
		OpenAdvancedSettings = 1,
		OpenLanguageSettings = 2,
		OpenDatabaseSettings = 3,
		OpenAutoUpdateSettings = 4,
		OpenKeyboardShortcutSettings = 5,

		SettingsLast = 5
	};
		
	union { // Action specifig type
		DWORD m_nSubAction;
		ActionControls m_nControl;
		ActionActivateTabs m_nActivateTab;
		ActionMenuCommands m_nMenuCommand;
		ActionShowHideDialog m_nDialogCommand;
		ActionResultList m_nResultList;
		ActionMisc m_nMisc;
		ActionHelp m_nHelp;
		ActionSettings m_nSettings;
	};

	struct SendMessageInfo {
		SendMessageInfo();
		SendMessageInfo(SendMessageInfo& rCopyFrom);
		~SendMessageInfo(); 

		DWORD GetData(BYTE* pData) const;
		DWORD GetDataLength() const;
		static SendMessageInfo* FromData(const BYTE* pData,DWORD dwDataLen,DWORD& dwUsed);
		
		LPWSTR szWindow;
		DWORD nMessage;
		LPWSTR szWParam;
		LPWSTR szLParam;
	};

	enum SelectFileType {
		NextFile=0,
		PrevFile=1,
		NextNonDeletedFile=2,
		PrevNonDeletedFile=3
	};

	enum ContextMenuFor {
		FileOrFolder=0,
		Parent=1
	};

	union { // Extra action data
		void* m_pExtraInfo;	
		
		LPWSTR m_szVerb; // with, ResultListItems::Execute, NULL means default
		SendMessageInfo* m_pSendMessage; // with, Advanced::SendMessage and Advanced::PostMessage
		LPWSTR m_szCommand;
		LPWSTR m_szPreset;
		LPWSTR m_szValue;
		SelectFileType m_nSelectFileType;
		int m_nItem;
		ContextMenuFor m_nContextMenuFor;
	};

	// highest bit = 1 : mainmenu
	// highest bit = 0 : supmenu
	// other bits = nSubMenu
	static BYTE GetMenuAndSubMenu(ActionMenuCommands nMenuCommand);

#ifdef HFCLIB
	static int GetActivateTabsActionLabelStringId(ActionActivateTabs uSubAction);
	static int GetShowHideDialogActionLabelStringId(ActionShowHideDialog uSubAction);
	static int GetResultItemActionLabelStringId(ActionResultList uSubAction);
	static int GetMiscActionLabelStringId(ActionMisc uSubAction);
	static int GetHelpActionLabelStringId(ActionHelp uSubAction);
	static void GetSettingsActionLabelString(ActionSettings uSubAction,CStringW& str);
#endif

	BOOL DoActivateControl();
	BOOL DoActivateTab();
	BOOL DoMenuCommand();
    BOOL DoShowHideDialog();
	BOOL DoResultListItems();
	BOOL DoMisc();
	BOOL DoChangeValue();
	BOOL DoPresets();
	BOOL DoHelp();
	BOOL DoSettings();


	DWORD GetData(DWORD nAction,BYTE* pData,BOOL bHeader=TRUE) const;
	DWORD GetDataLength(DWORD nAction,BOOL bHeader=TRUE) const;
	
	static CSubAction* FromData(DWORD nAction,const BYTE* pData,DWORD dwDataLen,DWORD& dwUsed);

	static LPWSTR GetPathFromExplorer();
	static BOOL CALLBACK EnumExplorerChilds(HWND hWnd,LPARAM lParam);

	
protected:
	DWORD FillFromData(DWORD nAction,const BYTE* pData,DWORD dwDataLen);
	CSubAction(void* pVoid); // Empty constructor
};

class CAction : public CSubAction {
public:
	CAction();
	CAction(CAction& rCopyFrom);
	~CAction();
	
	void ClearExtraInfo();


	enum Action {
		None = -1,

		ActivateControl = 0,
		ActivateTab = 1,
		MenuCommand = 2,
		ShowHideDialog = 3,
		ResultListItems = 4,
		Misc = 5,
		ChangeValue = 6,
		Presets = 7,
		Help = 8,
		Settings = 9
	};
	
	union {
        Action m_nAction;
		DWORD m_dwAction; // Make sure that action takes four bytes
	};

	

		

protected:
	DWORD GetData(BYTE* pData) const;
	DWORD GetDataLength() const;
	
	static CAction* FromData(const BYTE* pData,DWORD dwDataLen,DWORD& dwUsed);
	CAction(void* pVoid); // Empty constructor

	BOOL ExecuteAction();


	friend class CShortcut;

};

// Keyboard shortcut
class CShortcut {
public:
	CShortcut();
	CShortcut(CShortcut& rCopyFrom);
	~CShortcut();

	void ClearExtraInfo();

	BYTE GetHotkeyModifiers() const;
	void SetHotkeyModifiers(BYTE nHotkeyModifier);
	char GetMnemonicForAction(HWND* hDialogs) const;
	
	static BYTE HotkeyModifiersToModifiers(BYTE bHotkeyModifiers);
	static BYTE ModifiersToHotkeyModifiers(BYTE bModifiers);

		
	enum LoadShortcuts {
		loadLocal =  0x1,
		loadGlobalHotkey = 0x2,
		loadGlobalHook = 0x4,

		loadAll = loadLocal|loadGlobalHotkey|loadGlobalHook
	};

	struct VirtualKeyName {
		BYTE bKey;
		LPSTR pName;
		int iFriendlyNameId;
	};

#ifdef HFCLIB
	static BOOL LoadShortcuts(CArrayFP<CShortcut*>& aShortcuts,BYTE bLoadFlag=loadAll);
	static BOOL SaveShortcuts(const CArrayFP<CShortcut*>& aShortcuts);
	static BOOL GetDefaultShortcuts(CArrayFP<CShortcut*>& aShortcuts,BYTE bLoadFlag=loadAll);
	static void ResolveMnemonics(CArrayFP<CShortcut*>& aShortcuts,HWND* hDialogs);
	static void ModifyMenus(CArrayFP<CShortcut*>& aShortcuts,HMENU hMainMenu,HMENU hSubMenu);
	
	static VirtualKeyName* GetVirtualKeyNames();
	void FormatKeyLabel(VirtualKeyName* pVirtualKeyNames,CStringW& str) const;
	static void FormatKeyLabel(VirtualKeyName* pVirtualKeyNames,BYTE bKey,BYTE bModifiers,BOOL bScancode,CStringW& str);


#endif

	BOOL IsModifiersOk(BOOL bAltDown,BOOL bControlDown,BOOL bShiftDown,BOOL bWinDown) const;
	BOOL IsWhenAndWhereSatisfied(HWND hSystemTrayWnd) const;
	
	void ExecuteAction();
	void SendEventBackToControl();

	// Flags
	enum Flags {
		sfLocal	 = 0x00,
		sfGlobalHotkey = 0x01,
		sfGlobalHook = 0x03,
		sfKeyTypeMask = 0x03,

		sfUseMemonic = 0x04,
		
		sfExecuteWhenDown = 0x00,
		sfExecuteWhenUp = 0x08,
		sfExecuteMask = 0x08,

		sfRemoveKeyDownMessage = 0x10,
		sfRemoveKeyUpMessage = 0x20,
		sfKeyCurrentlyDown = 0x40, // For hook
		sfSendKeyEventBeforeWinRelaseIsHandled = 0x80, // For hook
		
		sfVirtualKeySpecified = 0x1000,
		sfVirtualKeyIsScancode = 0x2000,


		sfDefault = sfLocal|sfExecuteWhenDown
	};
	WORD m_dwFlags;


	
	// Shortcut
	BYTE m_bVirtualKey;
	BYTE m_bModifiers;
	enum Modifiers {
		ModifierAlt= MOD_ALT,
		ModifierControl = MOD_CONTROL,
		ModifierWin = MOD_WIN,
		ModifierShift = MOD_SHIFT
	};
	enum WhenPresssed {
		wpFocusInResultList = 0x0001,
		wpFocusNotInResultList = 0x0010,
		wpNameTabShown = 0x0002,
		wpSizeDateTabShown = 0x0004,
		wpAdvancedTabShown = 0x0008,
		wpDisableWhenISRunning = 0x0100,
		wpDisableWhenISNotRunning = 0x0200,
		

		wpInvert = 0x8000, // If set, meaning in "When Pressed:" is inverted
		
		wpDefault = wpFocusInResultList|wpFocusNotInResultList|wpNameTabShown|wpSizeDateTabShown|wpAdvancedTabShown

	};
	union {
		WORD m_wWhenPressed; // When pressed, combination of values of WherePressed enum

		struct {
			LPSTR m_pClass; // NULL=none, -1==locate dialog
			LPSTR m_pTitle; // NULL=none
		};
	};
	DWORD m_nDelay; // 0=none, -1=post, otherwise it is delay in ms
	
	
	

	CArrayFP<CAction*> m_apActions;

protected:
	DWORD GetData(BYTE* pData) const;
	DWORD GetDataLength() const;
	static CShortcut* FromData(const BYTE* pData,DWORD dwDataLen,DWORD& dwUsed);
	CShortcut(void* pVoid); // Empty constructor

	static BOOL LoadShortcuts(const BYTE* pData,DWORD dwDataLength,CArrayFP<CShortcut*>& aShortcuts,BYTE bLoadFlag);


	static BOOL DoClassOrTitleMatch(LPCSTR pClass,LPCSTR pCondition);

};

typedef CShortcut SHORTCUT,*PSHORTCUT;



#endif
