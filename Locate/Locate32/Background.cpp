/* Locate32 - Copyright (c) 1997-2010 Janne Huttunen */

#include <HFCLib.h>
#include "Locate32.h"

////////////////////////////////////////////
// CCheckFileNotificationsThread

#define CHANGE_BUFFER_LEN		10000

inline CCheckFileNotificationsThread::~CCheckFileNotificationsThread()
{	
	FnDebugMessage("FN: destructor called");
	
	ASSERT((m_lState==sInitializing && m_nHandles==0) || m_lState==sStopping || m_lState==sTerminated);
	ASSERT(m_lState==sTerminated || m_hThread!=NULL);
	

	InterlockedExchangePointer((PVOID*)&GetLocateDlg()->m_pFileNotificationsThread,NULL);



	// Freeing handles
	DestroyHandles();
	
	

	// Delete file 
	if (m_pFile!=NULL)
		delete[] m_pFile;



	// Closing stop event
	CloseHandle(m_hStopEvent);
	DebugCloseEvent(m_hStopEvent);


	// Closing handle
	if (m_hThread!=NULL)
	{
		// We are still running
		CloseHandle(m_hThread);
		DebugCloseThread(m_hThread);
		m_hThread=NULL;
	}
}

BOOL CCheckFileNotificationsThread::Start()
{
	if (m_hThread!=NULL)
		return FALSE;
	
	DWORD dwThreadID;
	m_hThread=CreateThread(NULL,0,NotificationThreadProc,this,CREATE_SUSPENDED,&dwThreadID);
	DebugOpenHandle(dhtThread,m_hThread,"CheckFileNotifications");
	FnDebugMessage1("FN: thread started ID=%X",(DWORD)dwThreadID);
	
	if (m_hThread==NULL)
		return FALSE;
	SetThreadPriority(m_hThread,THREAD_PRIORITY_BELOW_NORMAL);
	ResumeThread(m_hThread);
	
	
	return TRUE;
}

BOOL CCheckFileNotificationsThread::Stop()
{
	// Stopping background notifier
	FnDebugMessage("FN: stopping");
	
	// First check whether thread is already stopped
	if (m_hThread==NULL)
	{
		FnDebugMessage("FN: already stopped");
		return FALSE;
	}

	// Creating a copy of thread handle and using
	HANDLE hThread;
	DuplicateHandle(GetCurrentProcess(),m_hThread,GetCurrentProcess(),
                    &hThread,0,FALSE,DUPLICATE_SAME_ACCESS);
	DebugOpenThread(hThread);
	
	
	ASSERT(m_lState==sInitializing || m_lState==sWaiting || 
		m_lState==sProcessing || m_lState==sStopping);

	// First, try some friendly ways to terminate thread

	switch (m_lState)
	{
	case sInitializing:
		// Still initializing, say that processing can be stopped
		FnDebugMessage("FN: thread still initializing");
		InterlockedExchange(&m_lFlags,m_lFlags|fwStopWhenPossible);
		
		// First wait that initialization is ended
		if (WaitForSingleObject(m_hStopEvent,500)!=WAIT_OBJECT_0)
			break;
		
		// Wait that ending processes are handled
		WaitForSingleObject(m_hThread,1000);
		break;
	case sWaiting:
		FnDebugMessage("FN: thread waiting");
		
		// Waiting events, so give it
		SetEvent(m_hStopEvent);
		
		// Wait that ending processes are handled
		WaitForSingleObject(m_hThread,1000);
		break;
	case sProcessing:
		FnDebugMessage("FN: thread is processing");
		InterlockedExchange(&m_lFlags,m_lFlags|fwStopWhenPossible);
		
		
		// Wait that ending processes are handled
		if (GetCurrentThreadId()==GetTrayIconWnd()->GetLocateDlgThread()->GetThreadId())
		{
			// This Stop() is called from CLocateDlg, it is possible
			// that RunninProcNew sens messages to this window.
			// Taking care of that
			for (int i=0;i<40;i++)
			{
				PostQuitMessage(0);
				GetTrayIconWnd()->GetLocateDlgThread()->ModalLoop();
				if (WaitForSingleObject(hThread,50)!=WAIT_TIMEOUT)
					break;
			}
		}
		else
			WaitForSingleObject(hThread,2000);
		break;
		
	};			

	
	
	if (GetLocateDlg()->m_pFileNotificationsThread!=NULL)
	{
		// So that didn't go very well, use harder ways
		InterlockedExchange(&m_lState,sTerminated);

		DWORD status=0;
		BOOL bRet=::GetExitCodeThread(hThread,&status);
		BOOL bTerminated=FALSE;

		
		if (bRet && status==STILL_ACTIVE)
		{
			if (::TerminateThread(hThread,1,TRUE))
				bTerminated=TRUE;
		}

		if (bTerminated)
			delete this;
	}

	// Closing dublicated handle
	CloseHandle(hThread);	
	DebugCloseThread(hThread);
	return TRUE;
}

inline void CCheckFileNotificationsThread::FileCreated(LPCWSTR szFile,DWORD dwLength,CLocateDlg* pLocateDlg)
{
	//BkgDebugNumMessage("File created: %S",DWORD(szFile));

	if (pLocateDlg->m_pListCtrl==NULL)
		return;
	
	BOOL bIsDirectory=FileSystem::IsDirectory(szFile);

	WCHAR szPath[MAX_PATH];
	int nItem=pLocateDlg->m_pListCtrl->GetNextItem(-1,LVNI_ALL);
	while (nItem!=-1)
	{
		if (m_lFlags&fwStopWhenPossible)
			break;

		CLocatedItem* pItem=(CLocatedItem*)pLocateDlg->m_pListCtrl->GetItemData(nItem);
		if (pItem!=NULL)
		{
			if (pItem->GetPathLen()==dwLength)
			{
				if (pItem->IsDeleted())
				{
					MemCopyW(szPath,pItem->GetPath(),pItem->GetPathLen()+1);
					MakeLower(szPath);
					if (wcsncmp(szPath,szFile,dwLength)==0)
					{
						if (pItem->RemoveFlagsForChanged())
							pLocateDlg->m_pListCtrl->RedrawItems(nItem,nItem);
					}
				}
			}
			else if (bIsDirectory && pItem->GetPathLen()>dwLength)
			{
				if (pItem->IsDeleted() && pItem->GetPath()[dwLength]=='\\')
				{
					MemCopyW(szPath,pItem->GetPath(),dwLength);
					MakeLower(szPath,dwLength);
				
					// Can be subdiretory
					if (wcsncmp(szPath,szFile,dwLength)==0)
					{
						if (pItem->RemoveFlagsForChanged())
							pLocateDlg->m_pListCtrl->RedrawItems(nItem,nItem);
					}
				}
			}
		}
		nItem=pLocateDlg->m_pListCtrl->GetNextItem(nItem,LVNI_ALL);
	}

	//BkgDebugMessage("File created ENF");
}

inline void CCheckFileNotificationsThread::FileModified(LPCWSTR szFile,DWORD dwLength,CLocateDlg* pLocateDlg)
{
	//BkgDebugNumMessage("File modified: %S",DWORD(szFile));

	if (pLocateDlg->m_pListCtrl==NULL)
		return;
	
	
	WCHAR szPath[MAX_PATH];
	int nItem=pLocateDlg->m_pListCtrl->GetNextItem(-1,LVNI_ALL);
	while (nItem!=-1)
	{
		if (m_lFlags&fwStopWhenPossible)
			break;

		CLocatedItem* pItem=(CLocatedItem*)pLocateDlg->m_pListCtrl->GetItemData(nItem);
		if (pItem!=NULL)
		{
			if (pItem->GetPathLen()==dwLength)
			{
				MemCopyW(szPath,pItem->GetPath(),pItem->GetPathLen()+1);
				MakeLower(szPath);
			    if (wcsncmp(szPath,szFile,dwLength)==0)
				{
					if (pItem->RemoveFlagsForChanged())
						pLocateDlg->m_pListCtrl->RedrawItems(nItem,nItem);
				}
			}
		}
		nItem=pLocateDlg->m_pListCtrl->GetNextItem(nItem,LVNI_ALL);
	}
	//BkgDebugMessage("File modified END");
}

inline void CCheckFileNotificationsThread::FileDeleted(LPCWSTR szFile,DWORD dwLength,CLocateDlg* pLocateDlg)
{
	//BkgDebugNumMessage("File deleted: %S",DWORD(szFile));

	if (pLocateDlg->m_pListCtrl==NULL)
		return;
	
	WCHAR szPath[MAX_PATH];
	int nItem=pLocateDlg->m_pListCtrl->GetNextItem(-1,LVNI_ALL);
	while (nItem!=-1)
	{
		if (m_lFlags&fwStopWhenPossible)
			break;

		CLocatedItem* pItem=(CLocatedItem*)pLocateDlg->m_pListCtrl->GetItemData(nItem);
		if (pItem!=NULL)
		{
			if (pItem->GetPathLen()==dwLength)
			{
				if (!pItem->IsDeleted())
				{
					MemCopyW(szPath,pItem->GetPath(),pItem->GetPathLen()+1);
					MakeLower(szPath);
					if (wcsncmp(szPath,szFile,dwLength)==0)
					{
						pItem->SetToDeleted();
						pLocateDlg->m_pListCtrl->RedrawItems(nItem,nItem);
					}
				}
			}
			else if (pItem->GetPathLen()>dwLength)
			{
				if (!pItem->IsDeleted() && pItem->GetPath()[dwLength]=='\\')
				{
					MemCopyW(szPath,pItem->GetPath(),dwLength);
					MakeLower(szPath,dwLength);
				
					// Can be subdiretory
					if (wcsncmp(szPath,szFile,dwLength)==0)
					{
						if (pItem->RemoveFlagsForChanged())
							pLocateDlg->m_pListCtrl->RedrawItems(nItem,nItem);
					}
				}
			}
		}
		nItem=pLocateDlg->m_pListCtrl->GetNextItem(nItem,LVNI_ALL);
	}

	//BkgDebugMessage("File deleted END");
}


BOOL CCheckFileNotificationsThread::RunningProcNew()
{
	FnDebugMessage("FN: RunningProcNew started");
	
	// Delete this when this functions ends
	CAutoPtr<CCheckFileNotificationsThread> thisPtr=this;

	// Creating handles
	if (!CreateHandlesNew())
		return FALSE;
	
	
	DWORD dwOut;

	for (;;)
	{
		FnDebugMessage("FN goes to sleep");
		InterlockedExchange(&m_lState,sWaiting);
		DWORD nRet=WaitForMultipleObjects(m_nHandles,m_pEventHandles,FALSE,INFINITE);
		InterlockedExchange(&m_lState,sProcessing);
		FnDebugMessage1("FN waked nRet=%X",nRet);


		if (nRet==WAIT_OBJECT_0) 
		{
			// First event signalled, this event is for stopping
			// So do nothing here and exit
			FnDebugMessage("FN stopping event signalled, exiting");
			break;
		}
		else if (nRet>WAIT_OBJECT_0 && nRet<WAIT_OBJECT_0+m_nHandles)
		{
			CLocateDlg* pLocateDlg=GetLocateDlg();
			
            // Locate dialog is also closed, stopping this process
			if (pLocateDlg==NULL)
				break;

			
			DIRCHANGEDATA* pChangeData=m_pDirDatas[nRet-WAIT_OBJECT_0];
			
			// Asking changes
			if (!pLocateDlg->IsLocating()) // if locating in process, do nothing
			{
				if (GetOverlappedResult(pChangeData->hDirHandle,&pChangeData->ol,&dwOut,FALSE))
				{
					// If GetOverlappedResults took so long that Stop() has given during this time
					// stop right now
					if (m_lFlags&fwStopWhenPossible)
						break;
					
					while (pLocateDlg->m_pBackgroundUpdater!=NULL &&
						!pLocateDlg->m_pBackgroundUpdater->m_lIsWaiting)
						Sleep(200);
					
					
					if (dwOut==0)
						UpdateItemsInRoot(pChangeData->pRoot,pLocateDlg);
					else
					{
						FILE_NOTIFY_INFORMATION* pStruct=(FILE_NOTIFY_INFORMATION*)pChangeData->pBuffer;
						while (1)
						{
							// Check stop state again
							if (m_lFlags&fwStopWhenPossible)
								break;

							DWORD dwLength=pStruct->FileNameLength/2;

							// Skip log files
							BOOL bSkipThisFile=FALSE;
							int nFilePart;
							for (nFilePart=dwLength-1;nFilePart>=0 && pStruct->FileName[nFilePart]!=L'\\';nFilePart--);
							nFilePart++;

							// Skipping ntuser.dat.log file
							if (dwLength-nFilePart==14)
							{
								if (_wcsnicmp(pStruct->FileName+nFilePart,L"ntuser.dat.log",14)==0)
									bSkipThisFile=TRUE;
							}

#ifdef _DEBUG
							// Skipping HFCDebug.log
							if (dwLength-nFilePart==12)
							{
								if (_wcsnicmp(pStruct->FileName+nFilePart,L"HFCDebug.log",12)==0)
									bSkipThisFile=TRUE;
							}
#endif

							if (!bSkipThisFile)
							{
								m_pFile=new WCHAR[pChangeData->dwRootLength+dwLength+2];
								MemCopyW(m_pFile,pChangeData->pRoot,pChangeData->dwRootLength);
								MemCopyW(m_pFile+pChangeData->dwRootLength,pStruct->FileName,dwLength);
								dwLength+=pChangeData->dwRootLength;
								m_pFile[dwLength]='\0';
								MakeLower(m_pFile);

								
								//DebugFormatMessage("BN: file=%S action=%d",m_pFile,pStruct->Action);

								switch(pStruct->Action)
								{
								case FILE_ACTION_ADDED:
								case FILE_ACTION_RENAMED_NEW_NAME:
									FileCreated(m_pFile,dwLength,pLocateDlg);
									break;
								case FILE_ACTION_REMOVED:
								case FILE_ACTION_RENAMED_OLD_NAME:
									FileDeleted(m_pFile,dwLength,pLocateDlg);
									break;
								case FILE_ACTION_MODIFIED:
									FileModified(m_pFile,dwLength,pLocateDlg);
									break;
								}
								
								delete[] m_pFile;
								m_pFile=NULL;
							}

							if (pStruct->NextEntryOffset==0)
								break;
							*((char**)&pStruct)+=pStruct->NextEntryOffset;

							
						}		
					}
				}
			}
			
			// Check this again 
			if (m_lFlags&fwStopWhenPossible)
				break;

			
			// Coing to listen changes
			FnDebugMessage("FN: going to listen changes");
			BOOL bRet=m_pReadDirectoryChangesW(pChangeData->hDirHandle,pChangeData->pBuffer,CHANGE_BUFFER_LEN,TRUE,
				FILE_NOTIFY_CHANGE_FILE_NAME|FILE_NOTIFY_CHANGE_DIR_NAME|
				FILE_NOTIFY_CHANGE_ATTRIBUTES|FILE_NOTIFY_CHANGE_SIZE|FILE_NOTIFY_CHANGE_LAST_WRITE|
				FILE_NOTIFY_CHANGE_CREATION|FILE_NOTIFY_CHANGE_SECURITY,
				&dwOut,&pChangeData->ol,NULL);
			FnDebugMessage("FN: m_pReadDirectoryChangesW returned");
			

			if (!bRet)
			{
				// ReadDirectoryChangesW failed, reset event manually
				FnDebugMessage("FN: ReadDirectoryChangesW failed");
				ResetEvent(pChangeData->ol.hEvent);
			}


			// Finally, before going to sleep, check that shell we continue
			if (m_lFlags&fwStopWhenPossible)
				break;

		}
	}


	InterlockedExchange(&m_lState,sStopping);

	FnDebugMessage("FN RunningProcNew ends");
	return FALSE;
}



BOOL CCheckFileNotificationsThread::RunningProcOld()
{
	// Delete this when this functions ends
	CAutoPtr<CCheckFileNotificationsThread> thisPtr=this;

	if (!CreateHandlesOld())
		return FALSE;

	
	
	for (;;)
	{
		FnDebugMessage("FN goes to sleep");
		InterlockedExchange(&m_lState,sWaiting);
		DWORD nRet=WaitForMultipleObjects(m_nHandles,m_pEventHandles,FALSE,INFINITE);
		InterlockedExchange(&m_lState,sProcessing);
		FnDebugMessage1("FN waked nRet=%X",nRet);

		
		
		if (nRet==WAIT_OBJECT_0) // The first is end event
		{
			// First event signalled, this event is for stopping
			// So do nothing here and exit
			FnDebugMessage("FN stopping event signalled, exiting");
			break;
		}
		else if (nRet>WAIT_OBJECT_0 && nRet<WAIT_OBJECT_0+m_nHandles)
		{
			//BkgDebugFormatMessage4("Something is changed in %S",m_pRoots[nRet-WAIT_OBJECT_0],0,0,0);

			CLocateDlg* pLocateDlg=GetLocateDlg();
			
			// Locate dialog is also closed, stopping this process
			if (pLocateDlg==NULL)
				break;

			if (!pLocateDlg->IsLocating()) // if locating in process, do nothing
			{
				while (pLocateDlg->m_pBackgroundUpdater!=NULL &&
					!pLocateDlg->m_pBackgroundUpdater->m_lIsWaiting)
					Sleep(200);
				
				// Stop if required
				if (m_lFlags&fwStopWhenPossible)
					break;
					
				// Updating changed items by checking all items
				UpdateItemsInRoot(m_pRoots[nRet-WAIT_OBJECT_0],pLocateDlg);		
			}

			// Check this again 
			if (m_lFlags&fwStopWhenPossible)
				break;

#ifdef _DEBUG_LOGGING
			BOOL bRet=FindNextChangeNotification(m_pEventHandles[nRet-WAIT_OBJECT_0]);
			FnDebugMessage3("CCheckFileNotificationsThread::RunningProcOld(): FindNextChangeNotification returns %X, nret=%X, nRet-WAIT_OBJECT_0=%X",bRet,nRet,nRet-WAIT_OBJECT_0);
#else	
			FindNextChangeNotification(m_pEventHandles[nRet-WAIT_OBJECT_0]);
#endif		

			// Finally, before going to sleep, check that shell we continue
			if (m_lFlags&fwStopWhenPossible)
				break;
		}
		else 
		{
			FnDebugMessage3("FN nRet not handled, nRet=0x%X, handles=%d, GetLastError()=0x%X",nRet,m_nHandles,GetLastError());
		}
	}
	


	InterlockedExchange(&m_lState,sStopping);
			

	
	
	//BkgDebugMessage("CCheckFileNotificationsThread::RunningProcOld() END");
	return FALSE;
}

void CCheckFileNotificationsThread::UpdateItemsInRoot(LPCWSTR szRoot,CLocateDlg* pLocateDlg)
{
	//BkgDebugFormatMessage("CCheckFileNotificationsThread::UpdateItemsInRoot BEGIN szRoot=%s",szRoot);
	
	if (pLocateDlg->m_pListCtrl==NULL)
		return;
	if (pLocateDlg->m_pListCtrl->GetItemCount()==0)
		return;

	
	// Updating changed items by checking all items
	if (szRoot[1]=='\0')
	{
		//BkgDebugMessage("CCheckFileNotificationsThread::UpdateItemsInRoot 1a");

		WCHAR szDriveLower=szRoot[0];
		WCHAR szDriveUpper=szRoot[0];
		MakeUpper(&szDriveUpper,1);

		//BkgDebugMessage("CCheckFileNotificationsThread::UpdateItemsInRoot 2a");

		int nItem=pLocateDlg->m_pListCtrl->GetNextItem(-1,LVNI_ALL);
		while (nItem!=-1)
		{
			if (m_lFlags&fwStopWhenPossible)
				break;

			CLocatedItem* pItem=(CLocatedItem*)pLocateDlg->m_pListCtrl->GetItemData(nItem);

			if (pItem!=NULL)
			{
				// Checking whether path is in changed volume
				LPCWSTR szPath=pItem->GetPath();
				if ((szPath[0]==szDriveLower || szPath[0]==szDriveUpper) &&
					szPath[1]==L':')
				{
					// Just disabling flags, let background thread do the rest
					if (pItem->RemoveFlagsForChanged())
						pLocateDlg->m_pListCtrl->RedrawItems(nItem,nItem);
					
				}
			}

			nItem=pLocateDlg->m_pListCtrl->GetNextItem(nItem,LVNI_ALL);
		}
	}
	else
	{
		DWORD dwLength=(DWORD)istrlenw(szRoot);

		//BkgDebugMessage("CCheckFileNotificationsThread::UpdateItemsInRoot 1b");

		int nItem=pLocateDlg->m_pListCtrl->GetNextItem(-1,LVNI_ALL);
		while (nItem!=-1)
		{
			if (m_lFlags&fwStopWhenPossible)
				break;

			CLocatedItem* pItem=(CLocatedItem*)pLocateDlg->m_pListCtrl->GetItemData(nItem);

			if (pItem!=NULL)
			{
				if (pItem->GetPathLen()>=dwLength)
				{
					LPWSTR szPath=alloccopy(pItem->GetPath(),pItem->GetPathLen());
					MakeLower(szPath);
		                
					if (wcsncmp(szPath,szRoot,dwLength)==0)
					{
						// Just disabling flags, let background thread do the rest
						if (pItem->RemoveFlagsForChanged())
							pLocateDlg->m_pListCtrl->RedrawItems(nItem,nItem);
						
					}
					delete[] szPath;
				}
			}
			nItem=pLocateDlg->m_pListCtrl->GetNextItem(nItem,LVNI_ALL);
		}
	}

	//DebugMessage("CCheckFileNotificationsThread::UpdateItemsInRoot END");
}

DWORD WINAPI CCheckFileNotificationsThread::NotificationThreadProc(LPVOID lpParameter)
{
	if (((CCheckFileNotificationsThread*)lpParameter)->m_pReadDirectoryChangesW!=NULL)
		return ((CCheckFileNotificationsThread*)lpParameter)->RunningProcNew();
	return ((CCheckFileNotificationsThread*)lpParameter)->RunningProcOld();
}

BOOL CCheckFileNotificationsThread::CreateHandlesNew()
{
	ASSERT(m_pEventHandles==NULL);
	ASSERT(m_lState==sInitializing);

	FnDebugMessage("FN: creating handles");

	CLocateDlg* pLocateDlg=GetLocateDlg();
	ASSERT(pLocateDlg!=NULL);

	// Loads roods from databases so that we know what to listen
	CArrayFAP<LPWSTR> aRoots;
	const CArray<PDATABASE>& aAllDatabases=GetLocateApp()->GetDatabases();
	CArray<PDATABASE> aUsedDatabases;
	for (int i=0;i<aAllDatabases.GetSize();i++)
	{
		if (pLocateDlg->IsDatabaseUsedInSearch(aAllDatabases[i]->GetID()))
			aUsedDatabases.Add(aAllDatabases[i]);
	}
	if (aUsedDatabases.GetSize()==0)
		return FALSE;
	CDatabaseInfo::GetRootsFromDatabases(aRoots,aUsedDatabases);
	if (aRoots.GetSize()==0)
		return FALSE;
	
	
	// Create arrays for event handles and data structures
	//
	// The first handle in m_pEventHandles is stop event, the rest of
	// handles are events which are used in overlay structure (m_pDirDatas[i].ol)
	// The first pointer in m_pDirDatas is NULL, the rest are pointers
	// to DIRCHANGEDATA structures. 
	// The lists are terminated with NULL
    
	// Allocating arraysn, note that the size of the list is not 
	// necessary aRoots.GetSize()+2 if CreateFileW or m_pReadDirectoryChangesW
	// return error
	m_pEventHandles=new HANDLE[aRoots.GetSize()+1];
	ASSERT(m_pEventHandles!=NULL);
	
	m_pDirDatas=new DIRCHANGEDATA*[aRoots.GetSize()+1];
	ASSERT(m_pDirDatas!=NULL);
	
	

	// First event in events array is stop event and first pointer to 
	// DIRCHANGEDATA structure is NULL, so that each element in m_pEventHandles (with index >0) 
	// corresponds to element in m_pChangeDatas with the same index
	m_pEventHandles[0]=m_hStopEvent;
	m_pDirDatas[0]=NULL;



	// Creating handles and DIRCHANGEDATA structures for directories in aRoots array
	m_nHandles=1; // Number of handles currently in arrays, first element is stop event / NULL
	DIRCHANGEDATA* pDirData=NULL; 
	
	for (int i=0;i<aRoots.GetSize();i++)
	{
		if (m_lFlags&fwStopWhenPossible)
		{
			// Notify to Stop() that we are going to stop what 
			// we are doing
			InterlockedExchange(&m_lState,sStopping);
			SetEvent(m_hStopEvent);
			break;
		}


		CStringW sRoot=aRoots.GetAt(i);

		// If root of the type "X:", change it to "X:\"
		if (sRoot[1]==':' && sRoot[2]=='\0')
			sRoot << L'\\';
		

	/*
#ifdef _DEBUG_LOGGING
		// If logging is on, do not use change notifications for root containing log file
		LPCSTR pLogFile=GetDebugLoggingFile();
		if (pLogFile!=NULL)
		{
			// No debug logging for drive containing hfcdebug.log

			char* szPath=alloccopyWtoA(sRoot);
			MakeLower(szPath);
			BOOL bSame=strncmp(szPath,pLogFile,sRoot.GetLength())==0;
			delete[] szPath;
            if (bSame)
				continue;
		}
#endif
	*/	

		// Allocating new DIRCHANGEDATA struct
		if (pDirData==NULL)
		{
			pDirData=new DIRCHANGEDATA;
			// Create event for overlay structure
			pDirData->ol.hEvent=CreateEvent(NULL,FALSE,FALSE,NULL);
			DebugOpenEvent(pDirData->ol.hEvent);
			
			// Allocate buffer
			pDirData->pBuffer=new BYTE[CHANGE_BUFFER_LEN];
		}

		// Create handle to directory
		if (IsUnicodeSystem())
			pDirData->hDirHandle=CreateFileW(sRoot,GENERIC_READ /*FILE_LIST_DIRECTORY*/,FILE_SHARE_READ|FILE_SHARE_DELETE|FILE_SHARE_WRITE,
				NULL,OPEN_EXISTING,FILE_FLAG_BACKUP_SEMANTICS|FILE_FLAG_OVERLAPPED,NULL);
		else
			pDirData->hDirHandle=CreateFile(W2A(sRoot),GENERIC_READ /*FILE_LIST_DIRECTORY*/,FILE_SHARE_READ|FILE_SHARE_DELETE|FILE_SHARE_WRITE,
				NULL,OPEN_EXISTING,FILE_FLAG_BACKUP_SEMANTICS|FILE_FLAG_OVERLAPPED,NULL);
		
		// If pDirData handle is invalid, skip this root
		if (pDirData->hDirHandle==INVALID_HANDLE_VALUE)
			continue;

		
		DebugOpenHandle(dhtFile,pDirData->hDirHandle,sRoot);


		// Test this again
		if (m_lFlags&fwStopWhenPossible)
		{
			// Notify to Stop() that we are going to stop what 
			// we are doing
			InterlockedExchange(&m_lState,sStopping);
			SetEvent(m_hStopEvent);
			break;
		}



		
		// Start to read directory changes, asynchronous mode
		BOOL bRet=m_pReadDirectoryChangesW(pDirData->hDirHandle,pDirData->pBuffer,CHANGE_BUFFER_LEN,TRUE,
			FILE_NOTIFY_CHANGE_FILE_NAME|FILE_NOTIFY_CHANGE_DIR_NAME|
            FILE_NOTIFY_CHANGE_ATTRIBUTES|FILE_NOTIFY_CHANGE_SIZE|FILE_NOTIFY_CHANGE_LAST_WRITE|
            FILE_NOTIFY_CHANGE_CREATION|FILE_NOTIFY_CHANGE_SECURITY,
			NULL,&pDirData->ol,NULL);
		
				
		if (!bRet)
		{
			// Cannot read directory changes (maybe UNC path), closing directory handle and skipping 
			// this directory. Allocated pDirData can be left untouched for the next try
			CloseHandle(pDirData->hDirHandle);
			DebugCloseHandle(dhtFile,pDirData->hDirHandle,sRoot);
			continue;
		}


		// And yet again
		if (m_lFlags&fwStopWhenPossible)
		{
			// Notify to Stop() that we are going to stop what 
			// we are doing
			SetEvent(m_hStopEvent);
			InterlockedExchange(&m_lState,sStopping);
			break;
		}



		// Copy root path to pDirData structure
		if (sRoot.LastChar()!=L'\\')
			sRoot << L'\\';
		sRoot.MakeLower();
		sRoot.FreeExtra();
		pDirData->dwRootLength=sRoot.GetLength();
		pDirData->pRoot=sRoot.GiveBuffer();


		// Handle in m_pEventHandles was the event used in overlay structure, set it
		m_pEventHandles[m_nHandles]=pDirData->ol.hEvent;

		// Add pointer to m_pDirDatas structure
		ASSERT(m_nHandles<UINT(aRoots.GetSize()+2));
		m_pDirDatas[m_nHandles]=pDirData;
		m_nHandles++;


		// New DIRCHANGEDATA structure should be allocated
		pDirData=NULL;


	}

	
	// Free extra DIRCHANGEDATA structure
	if (pDirData!=NULL)
		delete pDirData;

	


	FnDebugMessage("FN handles created");
	return TRUE;
}

BOOL CCheckFileNotificationsThread::CreateHandlesOld()
{
	ASSERT(m_pEventHandles==NULL);
	ASSERT(m_lState==sInitializing);

	FnDebugMessage("FN: creating handles (old method)");
	

	CLocateDlg* pLocateDlg=GetLocateDlg();
	ASSERT(pLocateDlg!=NULL);
	
	// Loads roods from databases so that we know what to listen
	CArrayFAP<LPWSTR> aRoots;
	const CArray<PDATABASE>& aAllDatabases=GetLocateApp()->GetDatabases();
	CArray<PDATABASE> aUsedDatabases;
	for (int i=0;i<aAllDatabases.GetSize();i++)
	{
		if (pLocateDlg->IsDatabaseUsedInSearch(aAllDatabases[i]->GetID()))
			aUsedDatabases.Add(aAllDatabases[i]);
	}
	if (aUsedDatabases.GetSize()==0)
		return FALSE;
	CDatabaseInfo::GetRootsFromDatabases(aRoots,aUsedDatabases);
	if (aRoots.GetSize()==0)
		return FALSE;
	

	// Create arrays for event handles and data structures
	//
	// The first handle in m_pEventHandles is stop event, the rest are change notification 
	// objects returned by FindFirstChangeNotification function.
	// The first pointer in m_pRoots is NULL, the rest are pointers
	// to root directory. The lists are terminated with NULL
    
	// Allocating arraysn, note that the size of the list is not 
	// necessary aRoots.GetSize()+2 if FindFirstChangeNotification returns error
	m_pEventHandles=new HANDLE[aRoots.GetSize()+1];
	ASSERT(m_pEventHandles!=NULL);
	
	m_pRoots=new LPWSTR[aRoots.GetSize()+1];
	ASSERT(m_pRoots!=NULL);
	
	
	// First handle in event handles array is stop handle and 
	// first pointer to root directory is NULL,
	// so that each element in m_pEventHandles (with index >0) 
	// corresponds to element in m_pRoots array with the same index
	m_pEventHandles[0]=m_hStopEvent;
	m_pRoots[0]=NULL;


	// Creating handles for directories in aRoots array using FindFirstChangeNotification
	m_nHandles=1;
	
	for (int i=0;i<aRoots.GetSize();i++)
	{
		if (m_lFlags&fwStopWhenPossible)
		{
			// Notify to Stop() that we are going to stop what 
			// we are doing
			SetEvent(m_hStopEvent);
			InterlockedExchange(&m_lState,sStopping);
			break;
		}


		CStringW sRoot=aRoots.GetAt(i);

		// If root of the type "X:", change it to "X:\"
		if (sRoot[1]==':' && sRoot[2]=='\0')
			sRoot << L'\\';
		
				
#ifdef _DEBUG_LOGGING
		// If logging is on, do not use change notifications for root containing log file
		LPCSTR pLogFile=GetDebugLoggingFile();
		if (pLogFile!=NULL)
		{
			// No debug logging for drive containing hfcdebug.log

			char* szPath=alloccopyWtoA(sRoot);
			MakeLower(szPath);
			BOOL bSame=strncmp(szPath,pLogFile,sRoot.GetLength())==0;
			delete[] szPath;
            if (bSame)
				continue;
		}
#endif
		

		// Create find change notification objects
		if (IsUnicodeSystem())
			m_pEventHandles[m_nHandles]=FindFirstChangeNotificationW(sRoot,TRUE,
				FILE_NOTIFY_CHANGE_FILE_NAME|FILE_NOTIFY_CHANGE_DIR_NAME|FILE_NOTIFY_CHANGE_SIZE|FILE_NOTIFY_CHANGE_LAST_WRITE);
		else
			m_pEventHandles[m_nHandles]=FindFirstChangeNotification(W2A(sRoot),TRUE,
				FILE_NOTIFY_CHANGE_FILE_NAME|FILE_NOTIFY_CHANGE_DIR_NAME|FILE_NOTIFY_CHANGE_SIZE|FILE_NOTIFY_CHANGE_LAST_WRITE);
		
		if (m_pEventHandles[m_nHandles]==INVALID_HANDLE_VALUE)
		{
			// FindFirstChangeNotification returned error, skipping this directory
			continue;
		}

		DebugOpenEvent(m_pEventHandles[m_nHandles]);


		sRoot.MakeLower();
		sRoot.FreeExtra();
		m_pRoots[m_nHandles]=sRoot.GiveBuffer();
		m_nHandles++;

	}


	

	FnDebugMessage("FN handles created");
	return TRUE;
}

BOOL CCheckFileNotificationsThread::DestroyHandles()
{
	FnDebugMessage("FN: starting to destroy handles");
	
	// Various tests
	ASSERT(m_lState==sStopping || m_lState==sTerminated || (m_lState==sInitializing && m_nHandles==0));

	if (m_pEventHandles==NULL)
		return TRUE;

	if (m_pReadDirectoryChangesW!=NULL)
	{
		ASSERT(m_pDirDatas[0]==NULL);
	

		// ReadDirectoryChangesW used, deleting DIRCHANGEDATA structures
		for (UINT n=1;n<m_nHandles;n++)
		{
			//Handle in event array and event in overlay structure should be same
			ASSERT(m_pEventHandles[n]==m_pDirDatas[n]->ol.hEvent);

			// Destructor of DIRCHANGEDATA closes ol.hEvent (i.e. handle in m_pEventHandles)
			delete m_pDirDatas[n];

			// Destructor also closes this handle
			//CloseHandle(pHandles[n]);

		}

		delete[] m_pDirDatas;
	}
	else
	{
		// Close change notifaction objects returned by FindFirstChangeNotifcation 
		// and free root directory string
		for (UINT n=1;n<m_nHandles;n++)
		{
			FindCloseChangeNotification(m_pEventHandles[n]);
			DebugCloseEvent(m_pEventHandles[n]);

			delete[] m_pRoots[n];
		}
		delete[] m_pRoots;
	}
	

	// Free event handle array
	delete[] m_pEventHandles;

	
	// Just for sure
	m_pEventHandles=NULL;
	m_pDirDatas=NULL;

	
	FnDebugMessage("FN: handles destroyed");
	return TRUE;
}



////////////////////////////////////////////
// CBackgroundUpdater

BOOL CBackgroundUpdater::Start()
{
    BuDebugMessage("CBackgroundUpdater::Start() BEGIN");

	if (m_hThread!=NULL)
	{
		BuDebugMessage("CBackgroundUpdater::Start() ALREADY STARTED");
		return FALSE;
	}

	DWORD dwThreadID;
	HANDLE hThread=CreateThread(NULL,0,UpdaterThreadProc,this,CREATE_SUSPENDED,&dwThreadID);
	DebugOpenHandle(dhtThread,hThread,"BackgroundUpdater");
	DebugFormatMessage("BU started, thread ID=%X",dwThreadID);


	if (hThread==NULL)
		return FALSE;

	InterlockedExchangePointer(&m_hThread,hThread);
	SetThreadPriority(m_hThread,THREAD_PRIORITY_BELOW_NORMAL);
	
	InterlockedExchange(&m_lIsWaiting,FALSE);
	ResumeThread(m_hThread);

	BuDebugMessage("CBackgroundUpdater::Start() END");
	return TRUE;
}


CBackgroundUpdater::~CBackgroundUpdater()
{	
	InterlockedExchangePointer((PVOID*)&GetLocateDlg()->m_pBackgroundUpdater,NULL);
	

	
	CloseHandle(m_phEvents[0]);
	DebugCloseEvent(m_phEvents[0]);
	CloseHandle(m_phEvents[1]);
	DebugCloseEvent(m_phEvents[1]);

	DeleteCriticalSection(&m_csUpdateList);


	// Closing handle
	if (m_hThread!=NULL)
	{
		// We are still running
		CloseHandle(m_hThread);
		DebugCloseThread(m_hThread);
		m_hThread=NULL;
	}
}


BOOL CBackgroundUpdater::Stop()
{
#ifdef THREADDISABLE_BACKROUNDUPDATER
	return TRUE;
#else
	BuDebugMessage("BU: Stop");

	if (m_hThread==NULL)
	{
		if (GetLocateDlg()->m_pBackgroundUpdater!=NULL)
			delete this;
		return FALSE;
	}
		
		
	// Creating a copy of thread handle and using
	HANDLE hThread;
	DuplicateHandle(GetCurrentProcess(),m_hThread,GetCurrentProcess(),
                    &hThread,0,FALSE,DUPLICATE_SAME_ACCESS);
	DebugOpenThread(hThread);


	DWORD status;
	BOOL bRet=::GetExitCodeThread(hThread,&status);
	if (bRet && status==STILL_ACTIVE)
	{
		InterlockedExchange(&m_lIgnoreItemsAndGoToSleep,TRUE);

		// Signal stop event
		SetEvent(m_phEvents[0]);
		
		// Wait for ending
		WaitForSingleObject(hThread,750);

		if (GetLocateDlg()->m_pBackgroundUpdater!=NULL)
		{
			BOOL bTerminated=FALSE;

			status=0;
			bRet=::GetExitCodeThread(hThread,&status);
			if (bRet && status==STILL_ACTIVE)
			{
				if (TerminateThread(hThread,1,TRUE))
					bTerminated=TRUE;
			}
	
			if (bTerminated)
				delete this;
		}
	}

	CloseHandle(hThread);
	DebugCloseThread(hThread);
	return TRUE;
#endif
}


void CBackgroundUpdater::IgnoreItemsAndGoToSleep()
{
	InterlockedExchange(&m_lIgnoreItemsAndGoToSleep,TRUE);

	ISDLGTHREADOK

	BuDebugMessage("BU: IgnoreItemsAndGoToSleep, executing modal loop");

	PostQuitMessage(0);
	GetTrayIconWnd()->GetLocateDlgThread()->ModalLoop();
		
	while (!m_lIsWaiting)
	{
		Sleep(20);
		PostQuitMessage(0);
		GetTrayIconWnd()->GetLocateDlgThread()->ModalLoop();
	}
	

	BuDebugMessage("BU: modal loop out");

	if (m_lIgnoreItemsAndGoToSleep)
	{
		EnterCriticalSection(&m_csUpdateList);
		m_aUpdateList.RemoveAll();
		LeaveCriticalSection(&m_csUpdateList);
	}

	
	BuDebugMessage("BU: IgnoreItemsAndGoToSleep END");

}

inline BOOL CBackgroundUpdater::RunningProc()
{
	BuDebugMessage1("BU RunningProc() BEGIN, running thread 0x%X",GetCurrentThreadId());

	// CLocatedItem::LoadThumbnail needs this
	CoInitializeEx(NULL,COINIT_APARTMENTTHREADED);
	
	for (;;)
	{
		InterlockedExchange(&m_lIsWaiting,TRUE);

		BuDebugMessage("BU: Going to sleep.");
		
		DWORD nRet=WaitForMultipleObjects(2,m_phEvents,FALSE,INFINITE);
		InterlockedExchange(&m_lIsWaiting,FALSE);
		InterlockedExchange(&m_lIgnoreItemsAndGoToSleep,FALSE);
		
		BuDebugMessage2("BU: nRet=%d, %d item to be updated",nRet,m_aUpdateList.GetSize());

		if (nRet==WAIT_TIMEOUT)
			continue;
		if (nRet==WAIT_OBJECT_0) // 0 = end event
			break;
		else if (nRet==WAIT_OBJECT_0+1)
		{
			// Resolve list style, list control size and icon size
			// Do this everytime because size of dialog or list style may be changed or 
			RECT rcViewRect;
			int nIconSizeX,nIconSizeY;

			DWORD nListStyle=m_pList->GetStyle()&LVS_TYPEMASK;
			m_pList->GetClientRect(&rcViewRect);
			ImageList_GetIconSize(
				m_pList->GetImageList(nListStyle==LVS_ICON?LVSIL_NORMAL:LVSIL_SMALL),
				&nIconSizeX,&nIconSizeY);

			

			for (;;)
			{
				// We use local variable for the array so that
				// other operations are not delayed
				
				EnterCriticalSection(&m_csUpdateList);
				
				CArrayFP<Item*> aLocalUpdateList;
				aLocalUpdateList.Swap(m_aUpdateList);

				LeaveCriticalSection(&m_csUpdateList);
                
				// No items in list, there is no need to do anything
				if (aLocalUpdateList.GetSize()==0)
					break;
			
				
				for (int i=0;i<aLocalUpdateList.GetSize() && !m_lIgnoreItemsAndGoToSleep;i++)
				{
					Item* pItem=aLocalUpdateList.GetAt(i);

					// At first, check that item is visible and threfore needs to be updated
					//BuDebugMessage1("Checking whether item %S needs to be updated ",pItem->m_pItem->GetName());

					POINT pt;
					if (m_pList->GetItemPosition(pItem->m_iItem,&pt))
					{
						// X axes does not need to be checked when report mode is on
						if (nListStyle==LVS_REPORT)
						{
							if (pt.y >= -nIconSizeY && pt.y<=rcViewRect.bottom)
							{
								BOOL bReDraw=FALSE;
								
								BuDebugMessage1("BU: Refreshing %s (1)",pItem->m_pItem->GetName());
								pItem->m_pItem->ReFresh(pItem->m_aDetails,bReDraw); // Item is visible

								if (bReDraw)
									m_pList->PostMessage(LVM_REDRAWITEMS,pItem->m_iItem,pItem->m_iItem);
							}
						}
						else
						{
							POINT ptOrigin;
							if (!m_pList->GetOrigin(&ptOrigin))
							{
								ptOrigin.x=0;
								ptOrigin.y=0;
							}

							if (pt.y >= ptOrigin.y-nIconSizeY && pt.y<=rcViewRect.bottom+ptOrigin.y &&
								pt.x >= ptOrigin.x-nIconSizeX && pt.x<=rcViewRect.right+ptOrigin.x)
							{
								BOOL bReDraw=FALSE;
								
								BuDebugMessage1("BU: Refreshing %S (2)",pItem->m_pItem->GetName());
								pItem->m_pItem->ReFresh(pItem->m_aDetails,bReDraw); // Item is visible

								if (bReDraw)
									m_pList->PostMessage(LVM_REDRAWITEMS,pItem->m_iItem,pItem->m_iItem);
							}
						}
					}
					
				}
				aLocalUpdateList.RemoveAll();
				
			}
			ResetEvent(m_phEvents[1]);
			
		}
	}
	delete this;
	
	BuDebugMessage("BU: RunningProc() END");

	CoUninitialize();
	return TRUE;
}

DWORD WINAPI CBackgroundUpdater::UpdaterThreadProc(LPVOID lpParameter)
{
	return ((CBackgroundUpdater*)lpParameter)->RunningProc();
}

void CBackgroundUpdater::AddToUpdateList(CLocatedItem* pItem, int iItem,DetailType nDetail)
{
	//BuDebugMessage1("CBackgroundUpdater::AddToUpdateList BEGIN this is %X",this);
	

	if (m_lIgnoreItemsAndGoToSleep)
	{
		// Thread is asked to ignore all items and go to sleep/quit
		return; 
	}

	EnterCriticalSection(&m_csUpdateList);
	

	for (int i=m_aUpdateList.GetSize()-1;i>=0;i--)
	{
		Item* pListItem=m_aUpdateList.GetAt(i);

		//BuDebugMessage4("CBackgroundUpdater::AddToUpdateList Checking whether pItem=%X is pListItem=%X, i=%d, listsize=%d",
		//	(DWORD_PTR)pItem,(DWORD_PTR)pListItem,i,pUpdateList->GetSize());

		if (pListItem==NULL)
			continue;
		
		if (pListItem->m_pItem==pItem)
		{
			//BuDebugMessage1("CBackgroundUpdater::AddToUpdateList checking wheter detail %d exists",nDetail);
				
			int j;
			for (j=pListItem->m_aDetails.GetSize()-1;j>=0;j--)
			{
				if (pListItem->m_aDetails[j]==nDetail)
					break;
			}
			if (j<0)
			{
				// Not found
				//BuDebugMessage2("CBackgroundUpdater::AddToUpdateList Adding new(2) detail %d to item %X",nDetail,(DWORD_PTR)pListItem);
				pListItem->m_aDetails.Add(nDetail);
			}
			LeaveCriticalSection(&m_csUpdateList);
			
			//BuDebugMessage("CBackgroundUpdater::AddToUpdateList END (loop)");
			return;
		}
	}
	

	Item* pNewItem=new Item(pItem,iItem,nDetail);
	//BuDebugMessage3("CBackgroundUpdater::AddToUpdateList Adding new item (%X) %X list size=%d",(DWORD_PTR)pItem,(DWORD_PTR)pNewItem,pUpdateList->GetSize());
	m_aUpdateList.Add(pNewItem);
	LeaveCriticalSection(&m_csUpdateList);
}

void CBackgroundUpdater::RemoveFromUpdateList(CLocatedItem* pItem)
{
	EnterCriticalSection(&m_csUpdateList);

	for (int i=m_aUpdateList.GetSize()-1;i>=0;i--)
	{
		Item* pListItem=m_aUpdateList.GetAt(i);
		if (pListItem->m_pItem==pItem)
		{
			m_aUpdateList.RemoveAt(i);
			break;
		}
	}

	LeaveCriticalSection(&m_csUpdateList);
}
