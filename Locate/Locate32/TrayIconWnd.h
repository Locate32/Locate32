/* Locate32 - Copyright (c) 1997-2010 Janne Huttunen */

#if !defined(LOCATEAPPWND_H)
#define LOCATEAPPWND_H

#if _MSC_VER >= 1000
#pragma once
#endif


class CCpuUsage;
class CAboutDlg;
class CSettingsProperties;
class CLocateDlgThread;



////////////////////////////////////////////////////////////
// Definitions for SetUpdateStatusInformation
#define	DEFAPPICON		(HICON)INVALID_HANDLE_VALUE  



////////////////////////////////////////////////////////////
// CTrayIconWnd - Message window for system tray icon

class CTrayIconWnd : public CFrameWnd
{
public:
	
	struct RootInfo {
		LPWSTR pName;
		LPWSTR pRoot;
		DWORD dwNumberOfDatabases;
		DWORD dwCurrentDatabase;
		WORD wProgressState;
		UpdateError ueError;
		UpdateStatus usStatus;
		
	};

	class CUpdateStatusWnd : public CFrameWnd 
	{
	public:
		CUpdateStatusWnd();
		virtual ~CUpdateStatusWnd();
	
		virtual int OnCreate(LPCREATESTRUCT lpcs);
		virtual void OnDestroy();
		virtual void OnNcDestroy();
		virtual void OnTimer(DWORD wTimerID);
		virtual void OnPaint();
		virtual void OnMouseMove(UINT fwKeys,WORD xPos,WORD yPos);
		
	
		virtual LRESULT WindowProc(UINT msg,WPARAM wParam,LPARAM lParam);
	
		

	public:
		void Update();
		void Update(WORD wThreads,WORD wRunning,RootInfo* pRootInfos);
		void IdleClose();
		virtual BOOL DestroyWindow();
		void CheckForegroundWindow();

		void SetFonts();
		void SetPosition();
		void FormatErrorForStatusTooltip(UpdateError ueError,CDatabaseUpdater* pUpdater);
		void FormatStatusTextLine(CStringW& str,const CTrayIconWnd::RootInfo& pRootInfo,int nThreadID=-1,int nThreads=1);
		static void EnlargeSizeForText(CDC& dc,CStringW& str,CSize& szSize);
		static void EnlargeSizeForText(CDC& dc,LPCWSTR szText,int nLength,CSize& szSize);
		static void FillFontStructs(LOGFONT* pTextFont,LOGFONT* pTitleFont);

			
		CStringW m_sStatusText;
		CArrayFAP<LPWSTR> m_aErrors;
		
		CFont m_TitleFont,m_Font;
		CSize m_WindowSize;

		COLORREF m_cTextColor;
		COLORREF m_cTitleColor;
		COLORREF m_cErrorColor;
		COLORREF m_cBackColor;
		
		CRITICAL_SECTION m_cUpdate;


		// Mouse move
		struct MouseMove {
			SHORT nStartPointX; // Point in client in which cursor is pressed
			SHORT nStartPointY;
		};
		MouseMove* m_pMouseMove;

	};

public:
	CTrayIconWnd();
	virtual ~CTrayIconWnd();

public:

	virtual int OnCreate(LPCREATESTRUCT lpcs);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs);
	virtual BOOL OnCommand(WORD wID,WORD wNotifyCode,HWND hControl);
	virtual void OnDestroy();
	virtual void OnTimer(DWORD wTimerID); 
	virtual void OnInitMenuPopup(HMENU hPopupMenu,UINT nIndex,BOOL bSysMenu);
	virtual void OnHelp(LPHELPINFO lphi);
	virtual LRESULT WindowProc(UINT msg,WPARAM wParam,LPARAM lParam);
	
	DWORD OnActivateAnotherInstance(ATOM aCommandLine);
	DWORD OnSystemTrayMessage(UINT uID,UINT msg);
	BOOL SetTrayIcon(HICON hIcon=NULL,UINT uTip=0,LPCWSTR szText=NULL);
	BOOL GetRootInfos(WORD& wThreads,WORD& wRunning,RootInfo*& pRootInfos);
	static void FreeRootInfos(WORD wThreads,RootInfo* pRootInfos);

	void AddTaskbarIcon(BOOL bForce=FALSE);
	void DeleteTaskbarIcon(BOOL bForce=FALSE);
	void LoadAppIcon();
	
	DWORD SetSchedules(CList<CSchedule*>* pSchedules=NULL,BOOL bRunStartupSchedulses=FALSE);
	BOOL SaveSchedules();
	void CheckSchedules();
	BOOL RunStartupSchedules(); // return value = Should run again?

	BOOL StartUpdateStatusNotification();
	BOOL StopUpdateStatusNotification();
	void NotifyFinishingUpdating();

	BOOL TurnOnShortcuts();
	BOOL TurnOffShortcuts();

	void SetLocateDlgThreadToNull();

	BYTE OnAbout();
	BYTE OnSettings(int nPage=-1);
	BYTE OnLocate();
	
	// If pDatabases is:
	//  NULL: Global update
	//  0xFFFFFFFF: Select databases
	//  otherwise Use databases in pDatabase array which is 
	//  double zero terminated seqeuence of strings
	BYTE OnUpdate(BOOL bStopIfProcessing,LPWSTR pDatabases=NULL); 
	BYTE OnUpdate(BOOL bStopIfProcessing,LPWSTR pDatabases,int nThreadPriority); 

	static DWORD WINAPI KillUpdaterProc(LPVOID lpParameter);
    

public:
	CLocateDlg* GetLocateDlg() const;
	CLocateDlgThread* GetLocateDlgThread() const { return m_pLocateDlgThread; }
	POSITION GetSchedules() const { return m_Schedules.GetHeadPosition(); }
	BOOL IsUpdateAnimationRunning() const { return m_pUpdateAnimIcons!=NULL; }
	CSettingsProperties* GetSettingsDialog() const { return m_pSettings; }


	CUpdateStatusWnd* m_pUpdateStatusWnd;
	CCpuUsage* m_pCpuUsage;
	CListFP <CSchedule*> m_Schedules;

protected:
	CMenu m_Menu;
	CAboutDlg* m_pAbout;
	CSettingsProperties* m_pSettings;
	

	
	CLocateDlgThread* m_pLocateDlgThread;
	CRITICAL_SECTION m_csLocateThread;
	
	
	HICON m_hAppIcon;
	HICON* m_pUpdateAnimIcons;
	WORD m_nCurUpdateAnimBitmap;
	CRITICAL_SECTION m_csAnimBitmaps;
	


	// Keyboard shortcuts
	CArrayFP<CShortcut*> m_aShortcuts;
	HHOOK m_hHook;

	
	friend inline CLocateDlg* GetLocateWnd();
	
};





#endif