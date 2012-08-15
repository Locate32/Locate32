/* Locate32 - Copyright (c) 1997-2010 Janne Huttunen */

#if !defined(BACKGROUND_H)
#define BACKGROUND_H

#if _MSC_VER >= 1000
#pragma once
#endif

class CCheckFileNotificationsThread
{
public:
	CCheckFileNotificationsThread();
	~CCheckFileNotificationsThread();

public:
	BOOL Start();
	BOOL Stop();

	void CouldStop();

	static DWORD WINAPI NotificationThreadProc(LPVOID lpParameter);
	BOOL RunningProcNew();
	BOOL RunningProcOld();

	void FileCreated(LPCWSTR szFile,DWORD dwLength,CLocateDlg* pLocateDlg);
	void FileModified(LPCWSTR szFile,DWORD dwLength,CLocateDlg* pLocateDlg);
	void FileDeleted(LPCWSTR szFile,DWORD dwLength,CLocateDlg* pLocateDlg);
	void UpdateItemsInRoot(LPCWSTR szRoot,CLocateDlg* pLocateDlg);

protected:
	BOOL CreateHandlesOld();
	BOOL CreateHandlesNew();
	BOOL DestroyHandles();
	

	

public:
	// Handle to this thread
	HANDLE m_hThread;

	
	// Number of handles
	UINT m_nHandles;
	

	// Stucture containing required information for
	// new method (ReadDirectoryChangesW)
	struct DIRCHANGEDATA
	{
		DIRCHANGEDATA();
		~DIRCHANGEDATA();

		// Root and its length
		WCHAR* pRoot;
		DWORD dwRootLength;
		
		// Handle to root via CreateFile
		HANDLE hDirHandle;
		
		// Overlapped structure
		OVERLAPPED ol;

		// Buffer 
		BYTE* pBuffer;
	};
	


	// Stop event
	HANDLE m_hStopEvent;

	// Event handles, first event handle is stop event
	HANDLE* m_pEventHandles;
	
	// Arrays
	union {
		DIRCHANGEDATA** m_pDirDatas;
		LPWSTR* m_pRoots;
	};


	// Flags
	enum State {
		sInitializing = 0,
		sWaiting = 1,
		sProcessing = 2,
		sStopping = 3,
		sTerminated = 4
	};
	volatile LONG m_lState;

	enum Flags {
		fwStopWhenPossible = 0x1,
	};
	volatile LONG m_lFlags;
	




	LPWSTR m_pFile; // Used in RunningProcNew
	
	BOOL (WINAPI* m_pReadDirectoryChangesW)(HANDLE,LPVOID,DWORD,BOOL,DWORD,LPDWORD,LPOVERLAPPED,LPOVERLAPPED_COMPLETION_ROUTINE);

};

class CBackgroundUpdater
{
public:
	CBackgroundUpdater(CListCtrl* pList);
	~CBackgroundUpdater();

public:
	BOOL Start();
	BOOL Stop();
	
	
	void CouldStop();
	void IgnoreItemsAndGoToSleep();
	void StopIgnoringItems();

	static DWORD WINAPI UpdaterThreadProc(LPVOID lpParameter);
	BOOL RunningProc();

	void StopWaiting();
	BOOL IsWaiting() const;

	void AddToUpdateList(CLocatedItem* pItem, int iItem,DetailType nDetail);
	void RemoveFromUpdateList(CLocatedItem* pItem);
	DWORD GetUpdateListSize() const;

public:
	HANDLE m_hThread;
	HANDLE m_phEvents[2]; // First is end event, secont is don't wait event

	struct Item{
		CLocatedItem* m_pItem;
		int m_iItem;
		CArray<DetailType> m_aDetails;
		
		Item() {};
		Item(CLocatedItem* pItem, int iItem);
		Item(CLocatedItem* pItem, int iItem,DetailType nDetail);
		
	};
	
	CListCtrl* m_pList;

	volatile LONG m_lIsWaiting;
	volatile LONG m_lIgnoreItemsAndGoToSleep;


	// Creating m_aUpdateList as threadsafe
protected:
	CArrayFP<Item*> m_aUpdateList;
	CRITICAL_SECTION m_csUpdateList;
	
};

inline CCheckFileNotificationsThread::DIRCHANGEDATA::DIRCHANGEDATA()
:   pRoot(NULL),hDirHandle(NULL),pBuffer(NULL)
{
	ol.hEvent=NULL;
}


inline CCheckFileNotificationsThread::DIRCHANGEDATA::~DIRCHANGEDATA()
{
	// Closing directory handle
	if (hDirHandle!=NULL)
	{
		BOOL (WINAPI *pCancelIo)(HANDLE)=
			(BOOL (WINAPI *)(HANDLE))GetProcAddress(GetModuleHandle("kernel32.dll"),"CancelIo");

		if (pCancelIo!=NULL)
            pCancelIo(hDirHandle);
		CloseHandle(hDirHandle);
		DebugCloseFile(hDirHandle);
	}

	// Closing event handle
	if (ol.hEvent!=NULL)
	{
		CloseHandle(ol.hEvent);
		DebugCloseEvent(ol.hEvent);
	}

	// Free Buffer
	if (pBuffer!=NULL)
		delete[] pBuffer;

	// Free root
	if (pRoot!=NULL)
		delete[] pRoot;
}


inline CCheckFileNotificationsThread::CCheckFileNotificationsThread()
:	m_hThread(NULL),m_lFlags(0),m_lState(sInitializing),m_pEventHandles(NULL),
	m_pDirDatas(NULL),m_pFile(NULL),m_nHandles(0)
{
	if ((GetLocateDlg()->GetExtraFlags()&CLocateDlg::efTrackingMask)==CLocateDlg::efEnableFSTrackingOld)
		m_pReadDirectoryChangesW=NULL;
	else
		m_pReadDirectoryChangesW=(BOOL (WINAPI*)(HANDLE,LPVOID,DWORD,BOOL,DWORD,LPDWORD,LPOVERLAPPED,LPOVERLAPPED_COMPLETION_ROUTINE))
			GetProcAddress(GetModuleHandle("kernel32.dll"),"ReadDirectoryChangesW");
	

	m_hStopEvent=CreateEvent(NULL,FALSE,FALSE,STRNULL);
	DebugOpenEvent(m_hStopEvent);
}



inline void CCheckFileNotificationsThread::CouldStop()
{
	if (m_pEventHandles==NULL)
		return;
	
	// Set stop event
	SetEvent(m_hStopEvent);
}


inline CBackgroundUpdater::CBackgroundUpdater(CListCtrl* pList)
:	m_pList(pList),m_hThread(NULL),m_lIgnoreItemsAndGoToSleep(FALSE)
{
	m_phEvents[0]=CreateEvent(NULL,TRUE,FALSE,NULL);
	DebugOpenEvent(m_phEvents[0]);
	m_phEvents[1]=CreateEvent(NULL,TRUE,FALSE,NULL);
	DebugOpenEvent(m_phEvents[1]);

	InitializeCriticalSection(&m_csUpdateList);
}


inline void CBackgroundUpdater::StopWaiting()
{
	ASSERT(this!=NULL);
	ASSERT(m_hThread!=NULL);

	if (m_aUpdateList.GetSize()==0)
	{
		//BkgDebugMessage("CBackgroundUpdater::StopWaiting() END (no wake)");
	
		return; // No reason to wake
	}

	if (m_lIsWaiting)
		SetEvent(m_phEvents[1]);

}

inline void CBackgroundUpdater::CouldStop()
{
	if (m_hThread==NULL)
		return;
	
	InterlockedExchange(&m_lIgnoreItemsAndGoToSleep,TRUE);
	SetEvent(m_phEvents[0]); // 0 = end envent, so stop processing
}

inline BOOL CBackgroundUpdater::IsWaiting() const
{
	return m_lIsWaiting;
}


inline void CBackgroundUpdater::StopIgnoringItems()
{
	BuDebugMessage("BU StopIgnoringItems");
	InterlockedExchange(&m_lIgnoreItemsAndGoToSleep,FALSE);
}

inline DWORD CBackgroundUpdater::GetUpdateListSize() const
{
	return m_aUpdateList.GetSize();
}
	
inline CBackgroundUpdater::Item::Item(CLocatedItem* pItem, int iItem)
:	m_pItem(pItem),m_iItem(iItem)
{
}

inline CBackgroundUpdater::Item::Item(CLocatedItem* pItem, int iItem,DetailType nDetail)
:	m_pItem(pItem),m_iItem(iItem)
{
	m_aDetails.Add(nDetail);
}


#endif