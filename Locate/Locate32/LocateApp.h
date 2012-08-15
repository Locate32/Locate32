/* Locate32 - Copyright (c) 1997-2010 Janne Huttunen */

#if !defined(LOCATEAPP_H)
#define LOCATEAPP_H

#if _MSC_VER >= 1000
#pragma once
#endif


////////////////////////////////////////////////////////////
// CLocateApp - Main application class

class CLocateApp : public CWinApp  
{
public:
	CLocateApp();
	virtual ~CLocateApp();
	
	virtual int ExitInstance();
	virtual BOOL InitInstance();

public:
	class CStartData
	{
	public:
		enum StatusFlags {
            statusRunAtStartUp=0x1,
			statusFindFileNames=0x8,
			statusFindFolderNames=0x10,
			statusFindFileAndFolderNames=statusFindFileNames|statusFindFolderNames,
			statusFindIsNotMatchCase=0x40,
			statusMatchWholeName=0x80,
			statusNoMatchWholeName=0x100,
			statusReplaceSpacesWithAsterisks=0x200,
			statusNoReplaceSpacesWithAsterisks=0x400,
			statusUseWholePath=0x800,
			statusNoUseWholePath=0x1000,
			statusNoExtension=0x2000,
			statusMatchCase=0x4000,
			statusNoMatchCase=0x8000
			
		};

		enum StartupFlags {
			startupLeaveBackground=0x1,
			startupUpdate=0x2,
			startupDoNotOpenDialog=0x4,
			startupExitAfterUpdating=0x8,
			startupNewInstance=0x10,
			startupDatabasesOverridden=0x20,
			startupExitedBeforeInitialization=0x40

		};

		enum Priority {
			priorityDontChange=0,
			priorityHigh=HIGH_PRIORITY_CLASS,
			priorityAbove=ABOVE_NORMAL_PRIORITY_CLASS,
			priorityNormal=NORMAL_PRIORITY_CLASS,
			priorityBelow=BELOW_NORMAL_PRIORITY_CLASS,
			priorityIdle=IDLE_PRIORITY_CLASS,
			priorityRealTime=REALTIME_PRIORITY_CLASS
		};

		enum Controls {
			None,
			Named,
			Type,
			LookIn,
			Results
		};


		CStartData();
		~CStartData();

		LPWSTR m_pStartPath;
		LPWSTR m_pStartString;
		LPWSTR m_pTypeString;
		LPWSTR m_pFindText;
		LPWSTR m_pLoadPreset;
		

		DWORD m_nStatus;
		Priority m_nPriority;
		Controls m_nActivateControl;
		BYTE m_nStartup;
		DWORD m_dwMaxFoundFiles;
		DWORD m_dwMaxFileSize;
		DWORD m_dwMinFileSize;
		char m_cMaxSizeType;
		char m_cMinSizeType;
		DWORD m_dwMaxDate;
		DWORD m_dwMinDate;
		char m_cMaxDateType;
		char m_cMinDateType;
		BYTE m_nSorting; 
		SHORT m_nActivateInstance; // 0 not set, -1 first instance, X instance no
		BYTE m_nListMode; // -1 not set, otherwise same as CLocateDlg::ListType


		CArrayFP<CDatabase*> m_aDatabases;



	};

	enum ProgramFlags {
		// Errors 
		pfShowCriticalErrors = 0x01, // Option
        pfShowNonCriticalErrors = 0x02, // Option
        pfErrorMask = 0x03,
		pfErrorDefault = pfShowCriticalErrors|pfShowNonCriticalErrors,
		pfErrorSave = pfShowCriticalErrors|pfShowNonCriticalErrors,
		
		// Tooltip
		pfEnableUpdateTooltip = 0x10, // Option
		pfUpdateTooltipNeverTopmost = 0,
		pfUpdateTooltipAlwaysTopmost = 0x20, // Option
		pfUpdateTooltipNoTopmostWhenForegroundWndMaximized = 0x40, // Option
		pfUpdateTooltipNoTopmostWhdnForegroundWndInFullScreen = 0x60, // Option
		pfUpdateTooltipTopmostMask = 0x60,
		pfUpdateTooltipPositionDefault = 0x00,
		pfUpdateTooltipPositionUp = 0x080,
		pfUpdateTooltipPositionDown = 0x180,
		pfUpdateTooltipPositionLeft = 0x080,
		pfUpdateTooltipPositionRight = 0x280,
		pfUpdateTooltipPositionLastPosition = 0x400,
		pfUpdateTooltipPositionUpLeft = pfUpdateTooltipPositionUp|pfUpdateTooltipPositionLeft,
		pfUpdateTooltipPositionUpRight = pfUpdateTooltipPositionUp|pfUpdateTooltipPositionRight,
		pfUpdateTooltipPositionDownLeft = pfUpdateTooltipPositionDown|pfUpdateTooltipPositionLeft,
		pfUpdateTooltipPositionDownRight = pfUpdateTooltipPositionDown|pfUpdateTooltipPositionRight,
		pfUpdateTooltipPositionMask = 0x0780,
        pfTooltipMask = 0x7F0,

		pfTooltipDefault = pfEnableUpdateTooltip|pfUpdateTooltipAlwaysTopmost|pfUpdateTooltipPositionDefault,
		pfTooltipSave = pfEnableUpdateTooltip|pfUpdateTooltipTopmostMask|pfUpdateTooltipPositionMask,

		// Misc
		pfTrayIconClickActivate = 0x1000,
		pfUseDefaultIconForDirectories =  0x2000,
		pfDontShowSystemTrayIcon = 0x4000,
		pfAvoidToAccessWhenReadingIcons = 0x8000,
		pfAskConfirmationForUpdateDatabases = 0x10000,
		pfMiscDefault = 0,
		pfMiscSave = 0x1F000,

		// Filesize/time/date format
		pfFormatUseLocaleFormat = 0x4, // Option
		pfFormatMask = 0x4,
		pfFormatDefault = pfFormatUseLocaleFormat,
		pfFormatSave = pfFormatUseLocaleFormat,

		pfDefault = pfErrorDefault|pfTooltipDefault|pfFormatDefault|pfMiscDefault,
		pfSave = pfErrorSave|pfTooltipSave|pfFormatSave|pfMiscSave
	};

	enum FileSizeFormats {
		fsfOverKBasKB = 0,
		fsfBestUnit = 1,
		fsfBytes = 2,
		fsfBytesNoUnit = 3,
		fsfKiloBytes = 4,
		fsfKiloBytesNoUnit = 5,
		fsfMegaBytesMegaBytes = 6,
		fsfMegaBytesMegaBytesNoUnit = 7,
		fsfOverMBasMB = 8
	};

	

protected:
	BYTE CheckDatabases();
	BYTE SetDeleteAndDefaultImage();
	
	
public:
	static BOOL ParseParameters(LPCWSTR lpCmdLine,CStartData* pStartData);
	static BOOL ParseSettingsBranch(LPCWSTR lpCmdLine,LPWSTR& rpSettingBranch);
	static BOOL CALLBACK EnumLocateSTWindows(HWND hwnd,LPARAM lParam);

	BOOL ActivateOtherInstances(LPCWSTR pCmdLine);
	
	void SaveRegistry() const;
	void LoadRegistry();
	BOOL UpdateSettings();

	LPWSTR FormatDateAndTimeString(WORD wDate,WORD wTime=WORD(-1));
	LPWSTR FormatFileSizeString(DWORD dwFileSizeLo,DWORD bFileSizeHi) const;
	BOOL SetLanguageSpecifigHandles();


	BOOL StopUpdating(BOOL bForce=TRUE);
    BOOL IsUpdating() const;
	BOOL IsWritingDatabases();
	
	
	BOOL GlobalUpdate(CArray<PDATABASE>* paDatabases=NULL,int nThreadPriority=THREAD_PRIORITY_NORMAL);

	

	// Database menu functions
	static BOOL IsDatabaseMenu(HMENU hMenu);
	void OnInitDatabaseMenu(CMenu& PopupMenu);
	void OnDatabaseMenuItem(WORD wID);
	static int GetDatabaseMenuIndex(HMENU hPopupMenu);

	static DWORD GetProgramFlags();
    	
private:
	static BOOL CALLBACK UpdateProc(DWORD_PTR dwParam,CallingReason crReason,UpdateError ueCode,CDatabaseUpdater* pUpdater);
	

protected:
	DWORD m_dwProgramFlags;

public:
	WORD m_wComCtrlVersion;
	WORD m_wShellDllVersion;
	
	int m_nDelImage;
	int m_nDefImage;
	int m_nDirImage;
	int m_nDriveImage;

	// Date & Time format string
	CStringW m_strDateFormat;
	CStringW m_strTimeFormat;
	FileSizeFormats m_nFileSizeFormat;


	DWORD(WINAPI* m_pGetLongPathName)(LPCWSTR,LPWSTR,DWORD);

	static DWORD WINAPI GetLongPathName(LPCWSTR lpszShortPath,LPWSTR lpszLongPath,DWORD cchBuffer);
	static DWORD WINAPI GetLongPathNameNoUni(LPCWSTR lpszShortPath,LPWSTR lpszLongPath,DWORD cchBuffer);

	UINT m_nInstance;

	// Registered messages
	static UINT m_nHFCInstallationMessage;
	static UINT m_nTaskbarCreated;
	static UINT m_nLocateAppMessage;


	// Locale number information
	struct LocaleNumberFormat {
		LocaleNumberFormat();		
		~LocaleNumberFormat()
		{
			delete[] pDecimal;
			delete[] pThousand;
		}
			
		UINT uLeadingZero;
		UINT uGrouping;
		WCHAR* pDecimal;
		WCHAR* pThousand;
	} *m_pLocaleNumberFormat;
	

protected:
	CStartData* m_pStartData;
	CTrayIconWnd m_AppWnd;

	BYTE m_nStartup;
	
	CRITICAL_SECTION m_cUpdatersPointersInUse;
	CDatabaseUpdater** m_ppUpdaters;
	

	
	CArrayFP<CDatabase*> m_aDatabases;
	mutable CDatabase* m_pLastDatabase;

	static void ChangeAndAlloc(LPWSTR& pVar,LPCWSTR szText);
	static void ChangeAndAlloc(LPWSTR& pVar,LPCWSTR szText,DWORD dwLength);
	static int ParseSearchProtocolString(LPWSTR* ppVar,LPCWSTR szText);




	BOOL InitCommonRegKey(LPCWSTR lpCmdLine=NULL);
	void FinalizeCommonRegKey();

	LPSTR m_szCommonRegKey;
	LPSTR m_szCommonRegFile;
	BYTE m_bFileIsReg;
	

public:
	static CAutoPtrA<CHAR> GetRegKey(LPCSTR szSubKey);
	
	const CArray<PDATABASE>& GetDatabases() const { return m_aDatabases; }
	CArray<PDATABASE>* GetDatabasesPtr() { return &m_aDatabases; }
	void SetDatabases(const CArray<CDatabase*>& rDatabases);
	WORD GetDatabasesNumberOfThreads() const;

	BYTE GetStartupFlags() const { return m_nStartup; }
	BOOL IsStartupFlagSet(CStartData::StartupFlags flag) { return m_nStartup&flag?1:0; }
	void SetStartupFlag(CStartData::StartupFlags flag) { m_nStartup|=BYTE(flag); }
	void ClearStartupFlag(CStartData::StartupFlags flag) { m_nStartup&=~BYTE(flag); }

	const CStartData* GetStartData() const { return m_pStartData; }
	void ClearStartData();
	void SetStartData(CStartData* pStarData);
	
	static int ErrorBox(int nError,UINT uType=MB_ICONERROR|MB_OK);
	static int ErrorBox(LPCWSTR szError,UINT uType=MB_ICONERROR|MB_OK);
	static LPWSTR FormatLastOsError();
	
	const CDatabase* GetDatabase(WORD wID) const;

	static INT_PTR CALLBACK DummyDialogProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);


private:
	CDatabase* GetDatabaseNonConst(WORD wID) const;

public:
	// Help system
	struct HelpID { int nID;LPCSTR lpName; };
	static BOOL OpenHelp(HWND hWnd,LPCSTR szHelpPage,HELPINFO* pInfo=NULL,HelpID* pHelpID=NULL,int nIDs=0);
   



	// Friends
	friend CTrayIconWnd;
	friend CTrayIconWnd::CUpdateStatusWnd;
	friend CTrayIconWnd* GetTrayIconWnd();
	friend CLocateDlg* GetLocateDlg();
	friend CRegKey2;

};


#include "LocateApp.inl"

#endif
