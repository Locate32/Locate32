/* Copyright (c) 1997-2010 Janne Huttunen
   database updater v3.1.9.6070                 */

#include <HFCLib.h>
#include "Locatedb.h"

#ifndef WIN32
#include "dtype.h"
#include "fcntl.h"
#endif

CDatabaseUpdater::CDatabaseUpdater(LPCWSTR szDatabaseFile,LPCWSTR szAuthor,LPCWSTR szComment,
		LPCWSTR* pszRoots,DWORD nNumberOfRoots,UPDATEPROC pProc,DWORD_PTR dwParam)
:	m_pCurrentRoot(NULL),sStatus(statusInitializing),m_dwFiles(0),m_dwDirectories(0),
	m_pProc(pProc),m_dwData(dwParam),m_dwCurrentDatabase(DWORD(-1)),dbFile(NULL)
#ifdef WIN32	
	,m_hThread(NULL),m_lForceQuit(FALSE)
#endif
{
	UpdDebugMessage("CDatabaseUpdater::CDatabaseUpdater(1)");

	m_aDatabases.Add(new DBArchive(szDatabaseFile,CDatabase::archiveFile,
		szAuthor,szComment,pszRoots,nNumberOfRoots,0,NULL,NULL,NULL,NULL,0,NULL));
}

CDatabaseUpdater::CDatabaseUpdater(const PDATABASE* ppDatabases,
		int nDatabases,UPDATEPROC pProc,DWORD_PTR dwParam)
:	m_pCurrentRoot(NULL),sStatus(statusInitializing),m_dwFiles(0),m_dwDirectories(0),
	m_pProc(pProc),m_dwData(dwParam),m_dwCurrentDatabase(DWORD(-1)),dbFile(NULL)
#ifdef WIN32	
	,m_hThread(NULL),m_lForceQuit(FALSE)
#endif
{
	UpdDebugMessage("CDatabaseUpdater::CDatabaseUpdater(2)");

	for (int i=0;i<nDatabases;i++)
		m_aDatabases.Add(new DBArchive(ppDatabases[i]));
}
	
CDatabaseUpdater::CDatabaseUpdater(const PDATABASE* ppDatabases,
		int nDatabases,UPDATEPROC pProc,WORD wThread,DWORD_PTR dwParam)
:	m_pCurrentRoot(NULL),sStatus(statusInitializing),m_dwFiles(0),m_dwDirectories(0),
	m_pProc(pProc),m_dwData(dwParam),m_dwCurrentDatabase(DWORD(-1)),dbFile(NULL)
#ifdef WIN32	
	,m_hThread(NULL),m_lForceQuit(FALSE)
#endif
{
	UpdDebugMessage("CDatabaseUpdater::CDatabaseUpdater(3)");

	for (int i=0;i<nDatabases;i++)
	{
		if (ppDatabases[i]->GetThreadId()==wThread)	
			m_aDatabases.Add(new DBArchive(ppDatabases[i]));
	}
}


CDatabaseUpdater::~CDatabaseUpdater()
{
	UpdDebugMessage("CDatabaseUpdater::~CDatabaseUpdater()");

	if (dbFile!=NULL)
	{
		dbFile->Close();
		delete dbFile;
		dbFile=NULL;
	}
	

#ifdef WIN32
	if (m_hThread!=NULL)
	{
		CloseHandle(m_hThread);
		DebugCloseThread(m_hThread);
		m_hThread=NULL;
	}
#endif
}

UpdateError CDatabaseUpdater::UpdatingProc()
{
	UpdDebugFormatMessage2("CDatabaseUpdater::UpdatingProc() BEGIN this=%lX m_pProc=%lX",ULONG_PTR(this),ULONG_PTR(m_pProc));
	
	UpdateError ueResult=ueSuccess;

	class StopUpdating{};

#ifdef WIN32
	InterlockedExchange(&m_lForceQuit,FALSE);
#endif

	// Set priority
	SetThreadPriority(GetCurrentThread(),m_nThreadPriority);
			

	ASSERT(dbFile==NULL);

	for (m_dwCurrentDatabase=0;m_dwCurrentDatabase<DWORD(m_aDatabases.GetSize());m_dwCurrentDatabase++)
	{
		BOOL bUnicode=m_aDatabases[m_dwCurrentDatabase]->IsFlagged(DBArchive::Unicode);


		// Starting to scan database
		try {
			m_pCurrentRoot=m_aDatabases[m_dwCurrentDatabase]->m_pFirstRoot;
			
			UpdDebugFormatMessage2("CDatabaseUpdater::UpdatingProc(): m_pCurrentDatabase=%lX szName=%s",ULONG_PTR(m_aDatabases[m_dwCurrentDatabase]),m_aDatabases[m_dwCurrentDatabase]->m_szName);
				
			m_pProc(m_dwData,StartedDatabase,ueResult,this);
			
			// Initilizating file and directory count
			m_dwFiles=0;
			m_dwDirectories=0;

			while (m_pCurrentRoot!=NULL
	#ifdef WIN32
				&& !m_lForceQuit
	#endif
				)
			{
				UpdDebugFormatMessage2("CDatabaseUpdater::UpdatingProc(): m_pCurrentRoot=%lX path=%s",ULONG_PTR(m_pCurrentRoot),(LPCSTR)W2A(m_pCurrentRoot->m_Path));
							
				sStatus=statusScanning;
				m_pProc(m_dwData,RootChanged,ueResult,this);
				
				
				// Scannin root
				if (bUnicode)
					ueResult=m_pCurrentRoot->ScanRootW(m_lForceQuit);
				else
					ueResult=m_pCurrentRoot->ScanRoot(m_lForceQuit);

				if (ueResult==ueFolderUnavailable)
				{
					switch(m_pProc(m_dwData,ErrorOccured,ueResult,this))
					{
					case 0:
						if (m_aDatabases[m_dwCurrentDatabase]->IsFlagged(DBArchive::StopIfUnuavailable))
							throw ueFolderUnavailable; // Next database
						break;
					case -1:
						throw ueFolderUnavailable; // Next database
					}
				}
				else if (ueResult!=ueSuccess)
					throw ueResult;
				
				m_dwFiles+=m_pCurrentRoot->m_dwFiles;
				m_dwDirectories+=m_pCurrentRoot->m_dwDirectories;
				
				// Next root
				m_pCurrentRoot=m_pCurrentRoot->m_pNext;
			}

	#ifdef WIN32
			if (m_lForceQuit)
				throw ueResult=ueStopped;
	#endif

			UpdDebugFormatMessage1("CDatabaseUpdater::UpdatingProc(): writing to %s",m_aDatabases[m_dwCurrentDatabase]->m_szArchive);
		
			// Begin to write database
			m_pCurrentRoot=NULL; 

			// First, tell to application that we are going to write database
			sStatus=statusInitializeWriting;
			m_pProc(m_dwData,InitializeWriting,ueResult,this);

			// Writing database
			sStatus=statusWritingDB;
			m_pProc(m_dwData,WritingDatabase,ueResult,this);
			
			
			BOOL bWriteHeader=TRUE;

			// Opening file
			switch (m_aDatabases[m_dwCurrentDatabase]->m_nArchiveType)
			{
			case CDatabase::archiveFile:
				if (m_aDatabases[m_dwCurrentDatabase]->IsFlagged(DBArchive::IncrementalUpdate))
				{
					UpdDebugMessage("CDatabaseUpdater::UpdatingProc(): trying to open database for incremental update");
					
					dbFile=OpenDatabaseFileForIncrementalUpdate(
						m_aDatabases[m_dwCurrentDatabase]->m_szArchive,
						m_dwFiles,m_dwDirectories,bUnicode);
					
					UpdDebugFormatMessage1("CDatabaseUpdater::UpdatingProc(): returns: %lX",(ULONG_PTR)dbFile);
					
					
					if (dbFile==(CFile*)-1)
					{
						dbFile=NULL;
						if (!m_pProc(m_dwData,ErrorOccured,ueCannotIncrement,this))
							throw ueStillWorking;
					}
					else if (dbFile==(CFile*)-2)
					{
						dbFile=NULL;
						if (!m_pProc(m_dwData,ErrorOccured,ueWrongCharset,this))
							throw ueStillWorking;
					}
					else if (dbFile!=NULL)
					{
						bWriteHeader=FALSE;
						break;
					}
				}
				
				if (dbFile==NULL)
				{
					UpdDebugMessage("CDatabaseUpdater::UpdatingProc(): trying to open database");
					dbFile=new CFile(m_aDatabases[m_dwCurrentDatabase]->m_szArchive,
						CFile::defWrite|CFile::otherStrNullTerminated,TRUE);
					dbFile->CloseOnDelete();
				}
				break;
			default:
				throw CFileException(CFileException::notImplemented,
					-1,m_aDatabases[m_dwCurrentDatabase]->m_szArchive);
			}
			
			if (bWriteHeader)
			{
				//////////////////////////////////
				// Writing header
				UpdDebugMessage("CDatabaseUpdater::UpdatingProc(): writing header");

		#ifdef WIN32
				// Writing identification, '\17=0x11=0x10|0x1' 0x1 = Long filenames and 0x10 = ANSI
				dbFile->Write("LOCATEDB20",10);
				dbFile->Write(BYTE(bUnicode?0x21:0x11));
		#else
				// Writing identification, '\0x0' = Short filenames and OEM
				dbFile->Write("LOCATEDB20",10);
				m_pCurrentRoot=m_pFirstRoot;
				BOOL bLFN=FALSE;
				while (m_pCurrentRoot!=NULL)
				{
					if (_use_lfn(m_pCurrentRoot->m_Path))
					{
						bLFN=TRUE;
						break;
					}
					m_pCurrentRoot=m_pCurrentRoot->m_pNext;
				}
				if (bLFN)
					File.Write(BYTE(1));
				else
					File.Write(BYTE(0));
		#endif
				DWORD dwExtraSize1=1,dwExtraSize2=1;
				if (m_aDatabases[m_dwCurrentDatabase]->m_szExtra1!=NULL)
					dwExtraSize1+=(DWORD)istrlenw(m_aDatabases[m_dwCurrentDatabase]->m_szExtra1);
				if (m_aDatabases[m_dwCurrentDatabase]->m_szExtra2!=NULL)
					dwExtraSize2+=(DWORD)istrlenw(m_aDatabases[m_dwCurrentDatabase]->m_szExtra2);
				
				

				if (bUnicode)
				{
					// Writing header size
					dbFile->Write(DWORD(
						(m_aDatabases[m_dwCurrentDatabase]->m_sAuthor.GetLength()+1)*2+ // Author data
						(m_aDatabases[m_dwCurrentDatabase]->m_sComment.GetLength()+1)*2+ // Comments data
						dwExtraSize1*2+dwExtraSize2*2+ // Extra
						4+ // Time
						4+ // Number of files
						4  // Number of directories
						)
					);

					// Writing author
					dbFile->Write(m_aDatabases[m_dwCurrentDatabase]->m_sAuthor);
			
					// Writing comments
					dbFile->Write(m_aDatabases[m_dwCurrentDatabase]->m_sComment);
					
					// Writing free data
					if (m_aDatabases[m_dwCurrentDatabase]->m_szExtra1!=NULL)
						dbFile->Write(m_aDatabases[m_dwCurrentDatabase]->m_szExtra1,dwExtraSize1);
					else
						dbFile->Write((WORD)0);

					if (m_aDatabases[m_dwCurrentDatabase]->m_szExtra2!=NULL)
						dbFile->Write(m_aDatabases[m_dwCurrentDatabase]->m_szExtra2,dwExtraSize2);
					else
						dbFile->Write((WORD)0);
				}
				else
				{
					// Writing header size
					dbFile->Write(DWORD(
						m_aDatabases[m_dwCurrentDatabase]->m_sAuthor.GetLength()+1+ // Author data
						m_aDatabases[m_dwCurrentDatabase]->m_sComment.GetLength()+1+ // Comments data
						dwExtraSize1+dwExtraSize2+ // Extra
						4+ // Time
						4+ // Number of files
						4  // Number of directories
						)
					);
					// Writing author
					dbFile->Write(W2A(m_aDatabases[m_dwCurrentDatabase]->m_sAuthor),
						(DWORD)m_aDatabases[m_dwCurrentDatabase]->m_sAuthor.GetLength()+1);
			
					// Writing comments
					dbFile->Write(W2A(m_aDatabases[m_dwCurrentDatabase]->m_sComment),
						(DWORD)m_aDatabases[m_dwCurrentDatabase]->m_sComment.GetLength()+1);
					
					// Writing free data
					if (m_aDatabases[m_dwCurrentDatabase]->m_szExtra1!=NULL)
						dbFile->Write(W2A(m_aDatabases[m_dwCurrentDatabase]->m_szExtra1,dwExtraSize1),dwExtraSize1);
					else
						dbFile->Write((BYTE)0);
					if (m_aDatabases[m_dwCurrentDatabase]->m_szExtra2!=NULL)
						dbFile->Write(W2A(m_aDatabases[m_dwCurrentDatabase]->m_szExtra2,dwExtraSize2),dwExtraSize2);
					else
						dbFile->Write((BYTE)0);
				}


				// Writing filedate
				{
					WORD wTime,wDate;
		#ifdef WIN32
					SYSTEMTIME st;
					FILETIME ft;
					GetLocalTime(&st);
					SystemTimeToFileTime(&st,&ft);
					FileTimeToDosDateTime(&ft,&wDate,&wTime);
		#else
					time_t tt;
					struct tm *t;
					time(&tt);
					t=localtime(&tt);
					wDate=(t->tm_mday&0x1F)|(((t->tm_mon+1)&0x0F)<<5)|(((t->tm_year-80)&0x7F)<<9);
					wTime=((t->tm_sec/2)&0x1F)|((t->tm_min&0x3F)<<5)|((t->tm_hour&0x1F)<<11);
		#endif
					dbFile->Write(wDate);
					dbFile->Write(wTime);
				}

				// Writing number of files and directories
				dbFile->Write(m_dwFiles);
				dbFile->Write(m_dwDirectories);
			
				UpdDebugMessage("CDatabaseUpdater::UpdatingProc(): writing header end");
			}

			UpdDebugMessage("CDatabaseUpdater::UpdatingProc(): writing directory data");

			// Writing root directory datas
			CRootDirectory* pWriteRoot=m_aDatabases[m_dwCurrentDatabase]->m_pFirstRoot;
			while (pWriteRoot!=NULL && (ueResult==ueSuccess || ueResult==ueFolderUnavailable))
			{
				sStatus=statusWritingDB;
				
				// Writing root data
				if (bUnicode)
					ueResult=pWriteRoot->WriteW(dbFile);
				else
					ueResult=pWriteRoot->Write(dbFile);

				pWriteRoot=pWriteRoot->m_pNext;
			}

			UpdDebugMessage("CDatabaseUpdater::UpdatingProc(): end of directory data");

			// End mark
			dbFile->Write((DWORD)0);

			delete dbFile;
			dbFile=NULL;

			UpdDebugFormatMessage1("CDatabaseUpdater::UpdatingProc(): DB %lX OK",ULONG_PTR(m_aDatabases[m_dwCurrentDatabase]));
		}
		catch (CFileException fe)
		{
			UpdDebugFormatMessage1("CDatabaseUpdater::UpdatingProc(): catch CFileException exception with cause %d",fe.m_cause);

			switch (fe.m_cause)
			{
			case CFileException::badPath:
			case CFileException::accessDenied:
			case CFileException::fileExist:
			case CFileException::sharingViolation:
			case CFileException::fileCreate:
			case CFileException::directoryFull:
			case CFileException::tooManyOpenFiles:
				m_pProc(m_dwData,ErrorOccured,ueResult=ueCreate,this);
				break;
			case CFileException::invalidFile:
			case CFileException::fileOpen:
				m_pProc(m_dwData,ErrorOccured,ueResult=ueOpen,this);
				break;
			case CFileException::endOfFile:
			case CFileException::writeProtected:
			case CFileException::writeFault:
			case CFileException::diskFull:
			case CFileException::lockViolation:
				m_pProc(m_dwData,ErrorOccured,ueResult=ueWrite,this);
				break;
			case CFileException::readFault:
				m_pProc(m_dwData,ErrorOccured,ueResult=ueRead,this);
				break;
			default:
				m_pProc(m_dwData,ErrorOccured,ueResult=ueUnknown,this);
				break;
			}
			break;
		}
		catch (CException ex)
		{
			UpdDebugFormatMessage1("CDatabaseUpdater::UpdatingProc(): catch CException exception with cause %d",ex.m_cause);

			switch (ex.m_cause)
			{
			case CException::cannotAllocate:
				m_pProc(m_dwData,ErrorOccured,ueResult=ueAlloc,this);
				break;
			default:
				m_pProc(m_dwData,ErrorOccured,ueResult=ueUnknown,this);
				break;
			}
			break;
		}
		catch (UpdateError ue)
		{
			UpdDebugFormatMessage1("CDatabaseUpdater::UpdatingProc(): catch uerror %d",DWORD(ue));

			if (ue!=ueStillWorking && ue!=ueSuccess && ue!=ueFolderUnavailable)
			{
				m_pProc(m_dwData,ErrorOccured,ueResult=ue,this);
				break;
			}
		}
		catch (...)
		{
			UpdDebugMessage("CDatabaseUpdater::UpdatingProc(): catch unknown exception");

			m_pProc(m_dwData,ErrorOccured,ueResult=ueUnknown,this);
			break;
		}
		m_pProc(m_dwData,FinishedDatabase,ueResult,this);
	}

	sStatus=statusFinishing;
	m_dwCurrentDatabase=DWORD(-1);
	
	// Closing file if needed
	if (dbFile!=NULL)
	{
		delete dbFile;
		dbFile=NULL;
	}
	m_pProc(m_dwData,FinishedUpdating,ueResult,this);
	m_pProc(m_dwData,ClassShouldDelete,ueResult,this);

	// This class is deleted in the previous call, do not access this anymore

	UpdDebugFormatMessage1("CDatabaseUpdater::UpdatingProc(): ALL DONE ueResult=%d",DWORD(ueResult));

	return ueResult;
}

#ifdef WIN32
DWORD WINAPI CDatabaseUpdater::UpdateThreadProc(LPVOID lpParameter)
{
	UpdDebugFormatMessage1("CDatabaseUpdater::UpdateThreadProc thread=%lX",GetCurrentThreadId());
	return ((CDatabaseUpdater*)lpParameter)->UpdatingProc()==ueSuccess?0:1;	
}
#endif

// Start updating database
#ifdef WIN32
UpdateError CDatabaseUpdater::Update(BOOL bThreaded,int nThreadPriority)
{
	m_pProc(m_dwData,Initializing,ueSuccess,this);
	m_nThreadPriority=nThreadPriority;
	if (bThreaded)
	{
		DWORD dwThreadID;
		UpdDebugFormatMessage1("CDatabaseUpdater::Update this=%lX",ULONG_PTR(this));
		m_hThread=CreateThread(NULL,0,UpdateThreadProc,this,CREATE_SUSPENDED,&dwThreadID);
		DebugOpenThread(m_hThread);
		DebugFormatMessage("UPD: thread started ID=%X",dwThreadID);

		if (m_hThread!=NULL)
		{
			ResumeThread(m_hThread);
			return ueSuccess;
		}
		m_pProc(m_dwData,ErrorOccured,ueCannotCreateThread,this);
		return ueCannotCreateThread;
	}
	else
		return UpdatingProc();
}

BOOL CDatabaseUpdater::StopUpdating(BOOL bForce)
{
	if (m_hThread==NULL)
		return TRUE;
	
	HANDLE hThread;
	DuplicateHandle(GetCurrentProcess(),m_hThread,GetCurrentProcess(),
                    &hThread,0,FALSE,DUPLICATE_SAME_ACCESS);
	DebugOpenThread(hThread);



	DWORD status;
	BOOL bRet=::GetExitCodeThread(hThread,&status);
	if (bRet && status==STILL_ACTIVE)
	{
		InterlockedExchange(&m_lForceQuit,TRUE);
		WaitForSingleObject(hThread,300);
		bRet=::GetExitCodeThread(hThread,&status);
		
		if (!bForce)
			return !(bRet && status==STILL_ACTIVE);

		if (bRet && status==STILL_ACTIVE)
		{
			::TerminateThread(hThread,1,TRUE);
			
			m_pProc(m_dwData,FinishedUpdating,ueStopped,this);
			
			// Class should deleted
			if (m_hThread!=NULL)
				m_pProc(m_dwData,ClassShouldDelete,ueStopped,this);
		}
	}

	CloseHandle(hThread);
	DebugCloseThread(hThread);
	return TRUE;
}
#else

UpdateError CDatabaseUpdater::Update()
{
	m_pProc(m_dwData,Initializing,ueSuccess,this);
	return UpdatingProc();
}
#endif




CDatabaseUpdater::CRootDirectory::~CRootDirectory()
{
	while (m_pFirstBuffer!=NULL)
	{
		pCurrentBuffer=m_pFirstBuffer->pNext;
		delete m_pFirstBuffer;
		m_pFirstBuffer=pCurrentBuffer;
	}

	POSITION pPos=m_aOpenHandles.GetHeadPosition();
	while (pPos!=NULL)
	{
		FindClose(m_aOpenHandles.GetAt(pPos));
		DebugCloseHandle(dhtFileFind,m_aOpenHandles.GetAt(pPos),STRNULL);

		pPos=m_aOpenHandles.GetNextPosition(pPos);
	}
	m_aOpenHandles.RemoveAll();
}

UpdateError CDatabaseUpdater::CRootDirectory::ScanRoot(volatile LONG& lForceQuit)
{
	UpdDebugFormatMessage1("CDatabaseUpdater::CRootDirectory::ScanRoot: scanning root %s",m_Path);

	// Initializing buffer
	pCurrentBuffer=m_pFirstBuffer=new CBuffer;
	if (pCurrentBuffer==NULL)
		throw CException(CException::cannotAllocate);

	pPoint=*pCurrentBuffer;

	ASSERT(m_Path.GetLength()<=MAX_PATH);

	// Scanning folder
	char szPath[MAX_PATH+20];
	MemCopyWtoA(szPath,MAX_PATH+20,(LPCWSTR)m_Path,m_Path.GetLength()+1);
	
	UpdDebugMessage("CDatabaseUpdater::CRootDirectory::ScanRoot: started to scan root folder");
	UpdateError ueResult=ScanFolder(szPath,m_Path.GetLength(),lForceQuit);
	UpdDebugMessage("CDatabaseUpdater::CRootDirectory::ScanRoot: ended to scan root folder");
	
	
	pCurrentBuffer->nLength=(DWORD)(pPoint-pCurrentBuffer->pData);

	UpdDebugFormatMessage1("CDatabaseUpdater::CRootDirectory::ScanRoot: END scanning root %s",m_Path);
	return ueResult;
}

UpdateError CDatabaseUpdater::CRootDirectory::ScanRootW(volatile LONG& lForceQuit)
{
	UpdDebugFormatMessage1("CDatabaseUpdater::CRootDirectory::ScanRoot: scanning root %s",m_Path);

	// Initializing buffer
	pCurrentBuffer=m_pFirstBuffer=new CBuffer;
	if (pCurrentBuffer==NULL)
		throw CException(CException::cannotAllocate);

	pPoint=*pCurrentBuffer;

	ASSERT(m_Path.GetLength()<=MAX_PATH);

	// Scanning folder
	WCHAR szPath[MAX_PATH+20];
	MemCopyW(szPath,(LPCWSTR)m_Path,m_Path.GetLength()+1);
	
	UpdDebugMessage("CDatabaseUpdater::CRootDirectory::ScanRootW: started to scan root folder");
	UpdateError ueResult=ScanFolder(szPath,m_Path.GetLength(),lForceQuit);
	UpdDebugMessage("CDatabaseUpdater::CRootDirectory::ScanRootW: ended to scan root folder");
	
	
	pCurrentBuffer->nLength=(DWORD)(pPoint-pCurrentBuffer->pData);

	UpdDebugFormatMessage1("CDatabaseUpdater::CRootDirectory::ScanRoot: END scanning root %s",m_Path);
	return ueResult;
}


UpdateError CDatabaseUpdater::CRootDirectory::ScanFolder(LPSTR szFolder,DWORD nLength,volatile LONG& lForceQuit)
{
	szFolder[nLength++]='\\';
	szFolder[nLength]='*';
	szFolder[nLength+1]='.';
	szFolder[nLength+2]='*';
	szFolder[nLength+3]='\0';

	

	FIND_DATA fd;
	
	HFIND hFind=_FindFirstFile(szFolder,&fd);
	

	if (!VALID_HFIND(hFind))
	{
		// It is possible that directory has no files, 
		// checking whether directory actually exists
		szFolder[nLength-1]='\0';
		if (FileSystem::IsDirectory(szFolder))
			return ueSuccess;	
		return ueFolderUnavailable;
	}

	for(;;)
	{
		if (_FindGetName(&fd)[0]=='.' && (_FindGetName(&fd)[1]=='\0' || _FindGetName(&fd)[1]=='.'))
		{
			if(!_FindNextFile(hFind,&fd))
				break;
			continue;
		}


		if (lForceQuit)
		{
			_FindClose(hFind);
			throw ueStopped;
		}

		DWORD sNameLength=istrlen(_FindGetName(&fd));
		ASSERT(sNameLength<256);
		



		if (_FindIsFolder(&fd))
		{
			// Get the length of directory name and checks that length is 
			// less than MAX_PATH, otherwise ignore
			if (nLength+sNameLength<MAX_PATH)
			{
				// First checks
				// Include files pattern used
				if (m_aIncludeDirectoriesPatternsA!=NULL)
				{
					// Make name lower
					char* pNameLower=alloccopy(_FindGetName(&fd));
					CharLower(pNameLower);
					
					BOOL bIncluded=FALSE;
					LPSTR* pPtr=m_aIncludeDirectoriesPatternsA;
					while (*pPtr!=NULL)
					{
						if (ContainString(pNameLower,*pPtr))
						{
							bIncluded=TRUE;
							break;
						}

						pPtr++;
					}

					delete[] pNameLower;

					if (!bIncluded)
					{
						if(!_FindNextFile(hFind,&fd))
							break;
						continue;
					}	
				}

				// Append folder to form full path
				sMemCopy(szFolder+nLength,_FindGetName(&fd),sNameLength+1);
				
				// Checking whether directory is excluded
				BOOL bExcludeDirectory=FALSE;
				if (m_aExcludedDirectories.GetSize()>0)
				{
					// Construct full path
					char* pLowerFolder=alloccopy(szFolder,nLength+sNameLength);
					CharLower(pLowerFolder);
					
					
					for (int i=0;i<m_aExcludedDirectories.GetSize();i++)
					{
						if (ContainString(pLowerFolder,m_aExcludedDirectories[i]))
						{
							bExcludeDirectory=TRUE;
							break;
						}
					}
					delete[] pLowerFolder;

					if (bExcludeDirectory)
					{
						if (!(m_bFlags&CDatabaseUpdater::DBArchive::ExcludeOnlyContentOfDirs))
						{
							if(!_FindNextFile(hFind,&fd))
								break;
							continue;
						}
					}		
				}	

				
				// Check whether there is enough space in buffer
				if (pPoint+_MAX_PATH+10>pCurrentBuffer->pData+BFSIZE)
				{
					// New buffer
					pCurrentBuffer->nLength=(DWORD)(pPoint-pCurrentBuffer->pData);
					pCurrentBuffer=pCurrentBuffer->pNext=new CBuffer;
					if (pCurrentBuffer==NULL)
						throw CException(CException::cannotAllocate);
					
					pPoint=*pCurrentBuffer;
				}
				
				*pPoint=UDBATTRIB_DIRECTORY|_FindGetAttribFlag(&fd); // Directory
				BYTE* pSizePointer=pPoint+1;
				pPoint+=5;

				
				

				// Set file name length
				*(pPoint++)=(BYTE)sNameLength;
				
				// Copy path
				sMemCopy(pPoint,_FindGetName(&fd),sNameLength+1);

				// Move pointer
				pPoint+=sNameLength+1;


				// File time
				_FindGetLastWriteDosDateTime(&fd,(WORD*)pPoint,(WORD*)pPoint+1);
				_FindGetCreationDosDate(&fd,(WORD*)pPoint+2);
				_FindGetLastAccessDosDate(&fd,(WORD*)pPoint+3);
				pPoint+=sizeof(WORD)*4;
			
				if (!bExcludeDirectory &&
					(m_bFlags&CDatabaseUpdater::DBArchive::ScanJunctions ||	!(fd.dwFileAttributes&FILE_ATTRIBUTE_REPARSE_POINT) || 
					(fd.dwReserved0!=IO_REPARSE_TAG_MOUNT_POINT && fd.dwReserved0!=IO_REPARSE_TAG_SYMLINK)))
				{
					ScanFolder(szFolder,nLength+sNameLength,lForceQuit);
					//szFolder[nLength+sTemp]='\0';
				}
				*pPoint='\0'; // No more files and directories
				pPoint++;

				// Calculating directory data size
				if (pSizePointer>=pCurrentBuffer->pData && pSizePointer<pCurrentBuffer->pData+BFSIZE)
					((DWORD*)pSizePointer)[0]=(DWORD)(pPoint-pSizePointer);
				else
				{
					// Buffer has changed
					CBuffer* pTmp=m_pFirstBuffer;
					
					// old buffer to pTmp
					while (pSizePointer<pTmp->pData || pSizePointer>=pTmp->pData+BFSIZE)
						pTmp=pTmp->pNext;
					
					// Decreasing first buffer
					((LONG*)pSizePointer)[0]=(LONG)(pTmp->pData-pSizePointer);
					
					// Adding length between pCurrentbuffer and pTmp
					for (;pTmp!=pCurrentBuffer;pTmp=pTmp->pNext)
						((LONG*)pSizePointer)[0]+=pTmp->nLength;

					// Adding this buffer len
					((LONG*)pSizePointer)[0]+=(DWORD)(pPoint-pCurrentBuffer->pData);
				}

				// Increase directories count
				m_dwDirectories++;
			}
		}
		else
		{
			// File name
			
			if(nLength+sNameLength<MAX_PATH)
			{
				// Include files pattern used
				if (m_aIncludeFilesPatternsA!=NULL)
				{
					// Make name lower
					char* pNameLower=alloccopy(_FindGetName(&fd));
					CharLower(pNameLower);

					BOOL bIncluded=FALSE;
					LPSTR* pPtr=m_aIncludeFilesPatternsA;
					while (*pPtr!=NULL)
					{
						if (ContainString(pNameLower,*pPtr))
						{
							bIncluded=TRUE;
							break;
						}

						pPtr++;
					}

					delete[] pNameLower;

					if (!bIncluded)
					{
						if(!_FindNextFile(hFind,&fd))
							break;
						continue;
					}	
				}

				// Exclude files pattern used
				if (m_aExcludeFilesPatternsA!=NULL)
				{
					// Make name lower
					char* pNameLower=alloccopy(_FindGetName(&fd));
					CharLower(pNameLower);
					
					BOOL bExcluded=FALSE;
					LPSTR* pPtr=m_aExcludeFilesPatternsA;
					while (*pPtr!=NULL)
					{
						if (ContainString(pNameLower,*pPtr))
						{
							bExcluded=TRUE;
							break;
						}

						pPtr++;
					}

					delete[] pNameLower;

					if (bExcluded)
					{
						if(!_FindNextFile(hFind,&fd))
							break;
						continue;
					}				
				}
				
				if (pPoint+MAX_PATH+15>pCurrentBuffer->pData+BFSIZE)
				{
					// New buffer
					pCurrentBuffer->nLength=(DWORD)(pPoint-pCurrentBuffer->pData);
					pCurrentBuffer=pCurrentBuffer->pNext=new CBuffer;
					pPoint=*pCurrentBuffer;
				}

				*(pPoint++)=UDBATTRIB_FILE|_FindGetAttribFlag(&fd); // File
				
				// File name length
				*(pPoint++)=(BYTE)sNameLength; 
				// Extension position (or 0 if no extension)
				for (*pPoint=BYTE(sNameLength)-1;*pPoint>0 && _FindGetName(&fd)[*pPoint]!='.';(*pPoint)--); 
				pPoint++;
				
				// Copying filename
				sMemCopy(pPoint,_FindGetName(&fd),sNameLength+1);
				pPoint+=sNameLength+1;
				
				// File size
				((DWORD*)pPoint)[0]=_FindGetFileSizeLo(&fd);
				pPoint[4]=(BYTE)_FindGetFileSizeHi(&fd);
				pPoint+=5;

				// File time
				_FindGetLastWriteDosDateTime(&fd,(WORD*)pPoint,(WORD*)pPoint+1);
				_FindGetCreationDosDate(&fd,(WORD*)pPoint+2);
				_FindGetLastAccessDosDate(&fd,(WORD*)pPoint+3);
				
				pPoint+=sizeof(WORD)*4;
				
				// Increase files count
				m_dwFiles++;
			}
		}
		
		if(!_FindNextFile(hFind,&fd))
			break;
	}
	_FindClose(hFind);

	return ueSuccess;
}


UpdateError CDatabaseUpdater::CRootDirectory::ScanFolder(LPWSTR szFolder,DWORD nLength,volatile LONG& lForceQuit)
{
	szFolder[nLength++]=L'\\';
	szFolder[nLength]=L'*';
	szFolder[nLength+1]=L'.';
	szFolder[nLength+2]=L'*';
	szFolder[nLength+3]=L'\0';

	

	FIND_DATAW fd;
	
	HFIND hFind=_FindFirstFile(szFolder,&fd);
	
	

	if (!VALID_HFIND(hFind))
	{
		// It is possible that directory has no files, 
		// checking whether directory actually exists
		szFolder[nLength-1]='\0';
		if (FileSystem::IsDirectory(szFolder))
			return ueSuccess;	
		return ueFolderUnavailable;
	}


	for(;;)
	{

		if (wcscmp(_FindGetName(&fd),L"TEST.avi")==0)
		{
			CAppData::stdfunc();
		}

		if (_FindGetName(&fd)[0]==L'.' && (_FindGetName(&fd)[1]==L'\0' || _FindGetName(&fd)[1]==L'.'))
		{
			if(!_FindNextFile(hFind,&fd))
				break;
			continue;
		}

		if (lForceQuit)
		{
			_FindClose(hFind);
			throw ueStopped;
		}

		DWORD sNameLength=istrlenw(_FindGetName(&fd));
		ASSERT(sNameLength<256);

				
			

		if (_FindIsFolder(&fd))
		{
			// Get the length of directory name and checks that length is 
			// less than MAX_PATH, otherwise ignore
			if (nLength+sNameLength<MAX_PATH)
			{
				// First checks
				// Include directories pattern used
				if (m_aIncludeDirectoriesPatternsW!=NULL)
				{
					// Make name lower
					WCHAR* pNameLower=alloccopy(_FindGetName(&fd),sNameLength);
					CharLowerW(pNameLower);
					
					BOOL bIncluded=FALSE;
					LPWSTR* pPtr=m_aIncludeDirectoriesPatternsW;
					while (*pPtr!=NULL)
					{
						if (ContainString(pNameLower,*pPtr))
						{
							bIncluded=TRUE;
							break;
						}

						pPtr++;
					}

					delete[] pNameLower;

					if (!bIncluded)
					{
						if(!_FindNextFile(hFind,&fd))
							break;
						continue;
					}				
				}

				// Append folder to form full path
				MemCopyW(szFolder+nLength,_FindGetName(&fd),sNameLength+1);
				
				// Checking whether directory is excluded
				BOOL bExcludeDirectory=FALSE;
				if (m_aExcludedDirectories.GetSize()>0)
				{
					// Construct full path
					WCHAR* pLowerFolder=alloccopy(szFolder,nLength+sNameLength);
					CharLowerW(pLowerFolder);
					
					
					for (int i=0;i<m_aExcludedDirectories.GetSize();i++)
					{
						if (ContainString(pLowerFolder,m_aExcludedDirectories[i]))
						{
							bExcludeDirectory=TRUE;
							break;
						}
					}
					delete[] pLowerFolder;

					if (bExcludeDirectory)
					{
						if (!(m_bFlags&CDatabaseUpdater::DBArchive::ExcludeOnlyContentOfDirs))
						{
							if(!_FindNextFile(hFind,&fd))
								break;
							continue;
						}
					}		
				}	


				
				// Check whether there is enough space in buffer
				if (pPoint+MAX_PATH*2+14>pCurrentBuffer->pData+BFSIZE)
				{
					// New buffer
					pCurrentBuffer->nLength=(DWORD)(pPoint-pCurrentBuffer->pData);
					pCurrentBuffer=pCurrentBuffer->pNext=new CBuffer;
					if (pCurrentBuffer==NULL)
						throw CException(CException::cannotAllocate);
					
					pPoint=*pCurrentBuffer;
				}
				
				*pPoint=UDBATTRIB_DIRECTORY|_FindGetAttribFlag(&fd); // Directory
				BYTE* pSizePointer=pPoint+1;
				pPoint+=5;

				
				

				// Set file name length
				*(pPoint++)=(BYTE)sNameLength;
				
				// Copy path
				MemCopyW((LPWSTR)pPoint,_FindGetName(&fd),sNameLength+1);

				// Move pointer
				pPoint+=sNameLength*2+2;


				// File time
				_FindGetLastWriteDosDateTime(&fd,(WORD*)pPoint,(WORD*)pPoint+1);
				_FindGetCreationDosDate(&fd,(WORD*)pPoint+2,(WORD*)pPoint+3);
				_FindGetLastAccessDosDate(&fd,(WORD*)pPoint+4,(WORD*)pPoint+5);
				pPoint+=sizeof(WORD)*6;
			
				if (!bExcludeDirectory &&
					(m_bFlags&CDatabaseUpdater::DBArchive::ScanJunctions || !(fd.dwFileAttributes&FILE_ATTRIBUTE_REPARSE_POINT) || 
					(fd.dwReserved0!=IO_REPARSE_TAG_MOUNT_POINT && fd.dwReserved0!=IO_REPARSE_TAG_SYMLINK)))
				{
					// Scan files and directories if not reparse point
					ScanFolder(szFolder,nLength+sNameLength,lForceQuit);
					//szFolder[nLength+sTemp]='\0';
				}
				*pPoint='\0'; // No more files and directories
				pPoint++;

				// Calculating directory data size
				if (pSizePointer>=pCurrentBuffer->pData && pSizePointer<pCurrentBuffer->pData+BFSIZE)
					((DWORD*)pSizePointer)[0]=(DWORD)(pPoint-pSizePointer);
				else
				{
					// Buffer has changed
					CBuffer* pTmp=m_pFirstBuffer;
					
					// old buffer to pTmp
					while (pSizePointer<pTmp->pData || pSizePointer>=pTmp->pData+BFSIZE)
						pTmp=pTmp->pNext;
					
					// Decreasing first buffer
					((LONG*)pSizePointer)[0]=(LONG)(pTmp->pData-pSizePointer);
					
					// Adding length between pCurrentbuffer and pTmp
					for (;pTmp!=pCurrentBuffer;pTmp=pTmp->pNext)
						((LONG*)pSizePointer)[0]+=pTmp->nLength;

					// Adding this buffer len
					((LONG*)pSizePointer)[0]+=(DWORD)(pPoint-pCurrentBuffer->pData);
				}

				// Increase directories count
				m_dwDirectories++;
			}
		}
		else
		{
			// File name
			if(nLength+sNameLength<MAX_PATH)
			{
				// Checks

				// Include files pattern used
				if (m_aIncludeFilesPatternsW!=NULL)
				{
					// Make name lower
					WCHAR* pNameLower=alloccopy(_FindGetName(&fd),sNameLength);
					CharLowerW(pNameLower);
					
					BOOL bIncluded=FALSE;
					LPWSTR* pPtr=m_aIncludeFilesPatternsW;
					while (*pPtr!=NULL)
					{
						if (ContainString(pNameLower,*pPtr))
						{
							bIncluded=TRUE;
							break;
						}

						pPtr++;
					}

					delete[] pNameLower;

					if (!bIncluded)
					{
						if(!_FindNextFile(hFind,&fd))
							break;
						continue;
					}				
				}

				// Exclude files pattern used
				if (m_aExcludeFilesPatternsW!=NULL)
				{
					// Make name lower
					WCHAR* pNameLower=alloccopy(_FindGetName(&fd),sNameLength);
					CharLowerW(pNameLower);
					
					BOOL bExcluded=FALSE;
					LPWSTR* pPtr=m_aExcludeFilesPatternsW;
					while (*pPtr!=NULL)
					{
						if (ContainString(pNameLower,*pPtr))
						{
							bExcluded=TRUE;
							break;
						}

						pPtr++;
					}

					delete[] pNameLower;

					if (bExcluded)
					{
						if(!_FindNextFile(hFind,&fd))
							break;
						continue;
					}				
				}

				// Check whether there is enough space in buffer
				if (pPoint+MAX_PATH*2+20>pCurrentBuffer->pData+BFSIZE)
				{
					// New buffer
					pCurrentBuffer->nLength=(DWORD)(pPoint-pCurrentBuffer->pData);
					pCurrentBuffer=pCurrentBuffer->pNext=new CBuffer;
					pPoint=*pCurrentBuffer;
				}

				*(pPoint++)=UDBATTRIB_FILE|_FindGetAttribFlag(&fd); // File
				
				// File name length
				*(pPoint++)=(BYTE)sNameLength; 
				// Extension position (or 0 if no extension)
				for (*pPoint=BYTE(sNameLength)-1;*pPoint>0 && _FindGetName(&fd)[*pPoint]!='.';(*pPoint)--); 
				pPoint++;

				// Copying filename
				MemCopyW((LPWSTR)pPoint,_FindGetName(&fd),sNameLength+1);
				pPoint+=sNameLength*2+2;
				
				// File size
				*((DWORD*)pPoint)=_FindGetFileSizeLo(&fd);
				*((WORD*)(pPoint+4))=(WORD)_FindGetFileSizeHi(&fd);
				pPoint+=6;

				// File time
				_FindGetLastWriteDosDateTime(&fd,(WORD*)pPoint,(WORD*)pPoint+1);
				_FindGetCreationDosDate(&fd,(WORD*)pPoint+2,(WORD*)pPoint+3);
				_FindGetLastAccessDosDate(&fd,(WORD*)pPoint+4,(WORD*)pPoint+5);
				
				pPoint+=sizeof(WORD)*6;
				
				// Increase files count
				m_dwFiles++;
			}
		}

		if(!_FindNextFile(hFind,&fd))
			break;
	}
	_FindClose(hFind);

	return ueSuccess;
}

#ifndef WIN32
BYTE _GetDriveType(LPCSTR szPath)
{
	return getdrivetype(szPath);
}
#else
inline BYTE _GetDriveType(CString& sPath)
{
	switch (FileSystem::GetDriveType(sPath+'\\'))
	{
	case DRIVE_UNKNOWN:
		return 0x00;
	case DRIVE_NO_ROOT_DIR:
		return 0xF0;
	case DRIVE_REMOVABLE:
		return 0x20;
	case DRIVE_FIXED:
		return 0x10;
	case DRIVE_REMOTE:
		return 0x40;
	case DRIVE_CDROM:
		return 0x30;
	case DRIVE_RAMDISK:
		return 0x50;
	}
	return 0;
}

inline BYTE _GetDriveType(CStringW& sPath)
{
	switch (FileSystem::GetDriveType(sPath+L'\\'))
	{
	case DRIVE_UNKNOWN:
		return 0x00;
	case DRIVE_NO_ROOT_DIR:
		return 0xF0;
	case DRIVE_REMOVABLE:
		return 0x20;
	case DRIVE_FIXED:
		return 0x10;
	case DRIVE_REMOTE:
		return 0x40;
	case DRIVE_CDROM:
		return 0x30;
	case DRIVE_RAMDISK:
		return 0x50;
	}
	return 0;
}
inline BYTE _GetDriveTypeW(CStringW& sPath)
{
	switch (GetDriveTypeW(sPath+L'\\'))
	{
	case DRIVE_UNKNOWN:
		return 0x00;
	case DRIVE_NO_ROOT_DIR:
		return 0xF0;
	case DRIVE_REMOVABLE:
		return 0x20;
	case DRIVE_FIXED:
		return 0x10;
	case DRIVE_REMOTE:
		return 0x40;
	case DRIVE_CDROM:
		return 0x30;
	case DRIVE_RAMDISK:
		return 0x50;
	}
	return 0;
}

#endif

UpdateError CDatabaseUpdater::CRootDirectory::Write(CFile* dbFile)
{
	CString sVolumeName,sFSName;
	DWORD dwSerial=0;
	BYTE nType;
	
	{
		if (m_Path.GetLength()==2 || (m_Path[0]=='\\' && m_Path[1]=='\\'))
			nType=_GetDriveType(m_Path);
		else
			nType=0xF0;
		// resolving label,fs and serial
#ifdef WIN32
		// Resolving information
		DWORD dwTemp;
		UINT nOldMode=SetErrorMode(SEM_FAILCRITICALERRORS);
		BOOL nRet;
		if (m_Path[0]!=L'\\')
		{
			char szDrive[4]="X:\\";
			szDrive[0]=W2Ac(m_Path[0]);
			nRet=GetVolumeInformation(szDrive,sVolumeName.GetBuffer(50),50,&dwSerial,
				&dwTemp,&dwTemp,sFSName.GetBuffer(50),50);
		}
		else // network, UNC path
			nRet=GetVolumeInformation(W2A(m_Path),sVolumeName.GetBuffer(50),50,&dwSerial,
				&dwTemp,&dwTemp,sFSName.GetBuffer(50),50);
		
		if (nRet)
		{
			sVolumeName.FreeExtra();
			sFSName.FreeExtra();
		}
		else
		{
			sVolumeName.Empty();
			sFSName.Empty();
			dwSerial=0;
		}

		SetErrorMode(nOldMode);
#else
		// getting fs
		if (getvolumeinfo(m_Path,&dwSerial,sVolumeName.GetBuffer(12),sFSName.GetBuffer(10)))
		{
			sVolumeName.FreeExtra();
			sFSName.FreeExtra();
		}
		else
		{
			sVolumeName.Empty();
			sFSName.Empty();
			dwSerial=0;
		}
#endif
	}

	// Calculating data length
	DWORD nLength=1 // Type
		+(DWORD)m_PathInDatabase.GetLength()+1 // Path
		+(DWORD)sVolumeName.GetLength()+1 // Volume name
		+sizeof(DWORD)	//Serial
		+(DWORD)sFSName.GetLength()+1 // Filesystem
		+2*sizeof(DWORD) // Number of files and directories
		+2; // End, 0x0000
	
	pCurrentBuffer=m_pFirstBuffer;
	while (pCurrentBuffer!=NULL)
	{
		nLength+=pCurrentBuffer->nLength;
		pCurrentBuffer=pCurrentBuffer->pNext;
	}
	
	// Writing data length
	dbFile->Write(nLength);

	// Writing volume type
	dbFile->Write(nType);

	// Writing path
	dbFile->Write(W2A(m_PathInDatabase));

	// Writing volume name
	dbFile->Write(sVolumeName);

	// Writing volume serial
	dbFile->Write(dwSerial);

	// Writing filesystem
	dbFile->Write(sFSName);

	// Writing number of files and directories
	dbFile->Write(m_dwFiles);
	dbFile->Write(m_dwDirectories);
       
	// Why?
	dbFile->Flush();

	while (m_pFirstBuffer!=NULL)
	{
		pCurrentBuffer=m_pFirstBuffer->pNext;
		dbFile->Write(m_pFirstBuffer->pData,m_pFirstBuffer->nLength);
		delete m_pFirstBuffer;
		m_pFirstBuffer=pCurrentBuffer;
	}

	// End mark
	dbFile->Write((WORD)0);

	return ueSuccess;
}

UpdateError CDatabaseUpdater::CRootDirectory::WriteW(CFile* dbFile)
{
	CStringW sVolumeName;
	CStringW sFSName;

	DWORD dwSerial=0;
	BYTE nType;
	
	{
		if (m_Path.GetLength()==2 || (m_Path[0]==L'\\' && m_Path[1]==L'\\'))
			nType=_GetDriveTypeW(m_Path);
		else
			nType=0xF0;

		// resolving label,fs and serial
		// Resolving information
		DWORD dwTemp;
		UINT nOldMode=SetErrorMode(SEM_FAILCRITICALERRORS);
		BOOL nRet;
		if (m_Path[0]!=L'\\')
		{
			WCHAR szDrive[4]=L"X:\\";
			szDrive[0]=m_Path[0];
			nRet=GetVolumeInformationW(szDrive,sVolumeName.GetBuffer(50),50,&dwSerial,
				&dwTemp,&dwTemp,sFSName.GetBuffer(50),50);
		}
		else // network, UNC path
			nRet=GetVolumeInformationW(m_Path,sVolumeName.GetBuffer(50),50,&dwSerial,
				&dwTemp,&dwTemp,sFSName.GetBuffer(50),50);
		
		if (nRet)
		{
			sVolumeName.FreeExtra();
			sFSName.FreeExtra();
		}
		else
		{
			sVolumeName.Empty();
			sFSName.Empty();
			dwSerial=0;
		}

		SetErrorMode(nOldMode);
	}

	// Calculating data length
	DWORD nLength=1 // Type
		+(DWORD)((m_PathInDatabase.GetLength()+1)*2) // Path
		+(DWORD)((sVolumeName.GetLength()+1)*2) // Volume name
		+sizeof(DWORD)	//Serial
		+(DWORD)((sFSName.GetLength()+1)*2) // Filesystem
		+2*sizeof(DWORD) // Number of files and directories
		+2; // End, 0x0000
	
	pCurrentBuffer=m_pFirstBuffer;
	while (pCurrentBuffer!=NULL)
	{
		nLength+=pCurrentBuffer->nLength;
		pCurrentBuffer=pCurrentBuffer->pNext;
	}
	
	// Writing data length
	dbFile->Write(nLength);

	// Writing volume type
	dbFile->Write(nType);

	// Writing path
	dbFile->Write(m_PathInDatabase);

	// Writing volume name
	dbFile->Write(sVolumeName);

	// Writing volume serial
	dbFile->Write(dwSerial);

	// Writing filesystem
	dbFile->Write(sFSName);

	// Writing number of files and directories
	dbFile->Write(m_dwFiles);
	dbFile->Write(m_dwDirectories);
       
	// Why?
	dbFile->Flush();

	while (m_pFirstBuffer!=NULL)
	{
		pCurrentBuffer=m_pFirstBuffer->pNext;
		dbFile->Write(m_pFirstBuffer->pData,m_pFirstBuffer->nLength);
		delete m_pFirstBuffer;
		m_pFirstBuffer=pCurrentBuffer;
	}

	// End mark
	dbFile->Write((WORD)0);

	return ueSuccess;
}


CDatabaseUpdater::DBArchive::DBArchive(const CDatabase* pDatabase)
:	m_sAuthor(pDatabase->GetCreator()),m_sComment(pDatabase->GetDescription()),
	m_nArchiveType(pDatabase->GetArchiveType()),m_pFirstRoot(NULL),m_nFlags(0),
	m_szExtra1(NULL),m_szExtra2(NULL),
	m_aIncludeFilesPatternsA(NULL),m_aIncludeDirectoriesPatternsA(NULL),
	m_aExcludeFilesPatternsA(NULL)
{
	BOOL bFree;
	m_szArchive=pDatabase->GetResolvedArchiveName(bFree);
	if (!bFree)
		m_szArchive=alloccopy(m_szArchive);

	m_dwNameLength=(DWORD)wcslen(pDatabase->GetName());
	m_szName=alloccopy(pDatabase->GetName(),m_dwNameLength);
	m_wID=pDatabase->GetID();

	// Retrieving flags
	if (pDatabase->IsFlagSet(CDatabase::flagStopIfRootUnavailable))
		m_nFlags|=StopIfUnuavailable;
	if (pDatabase->IsFlagSet(CDatabase::flagScanSymLinksAndJunctions))
		m_nFlags|=ScanJunctions;
	if (pDatabase->IsFlagSet(CDatabase::flagIncrementalUpdate))
		m_nFlags|=IncrementalUpdate;
	if (IsUnicodeSystem() && !pDatabase->IsFlagSet(CDatabase::flagAnsiCharset))
		m_nFlags|=Unicode;
	if (pDatabase->IsFlagSet(CDatabase::flagExcludeContentOfDirsOnly))
		m_nFlags|=ExcludeOnlyContentOfDirs;
	
	LPCWSTR pPtr=pDatabase->GetRoots();
	if (pPtr==NULL)
	{
		WCHAR drive[3]=L"X:";
		CRootDirectory* tmp;
			
		DWORD dwBufferLen=GetLogicalDriveStrings(0,NULL)+1;
		WCHAR* szDrives=new WCHAR[dwBufferLen];
		FileSystem::GetLogicalDriveStrings(dwBufferLen,szDrives);

		for (int i=0;szDrives[i*4]!=L'\0';i++)
		{
			if (FileSystem::GetDriveType(szDrives+i*4)==DRIVE_FIXED)
			{
				drive[0]=szDrives[i*4];
				
				int nMapLen;
				LPCWSTR pDriveInDb=pDatabase->FindRootMap(drive,nMapLen);
				CRootDirectory* pNewDirectory=new CRootDirectory(drive,pDriveInDb,
					nMapLen,m_nFlags);

				if (m_pFirstRoot==NULL)
					tmp=m_pFirstRoot=pNewDirectory;
				else
					tmp=tmp->m_pNext=pNewDirectory;
			}
		}
		delete[] szDrives;
        
		if (m_pFirstRoot!=NULL)
			tmp->m_pNext=NULL;
	}
	else
	{
		CRootDirectory* pCurrent=NULL;
		
		while (*pPtr!=L'\0')
		{
			DWORD dwLength=(DWORD)istrlenw(pPtr);
			CreateRootDirectories(pCurrent,pPtr,dwLength,pDatabase->GetRootMaps());
			
			pPtr+=dwLength+1;
		}

		if (m_pFirstRoot!=NULL)
			pCurrent->m_pNext=NULL;
	}


	// Resolve expected directories and files
	CDatabaseInfo::ReadFilesAndDirectoriesCount(m_nArchiveType,m_szArchive,
		m_dwExpectedFiles,m_dwExpectedDirectories);

	// Excluded directories
	ParseFilePatternsAndExcludedDirectories(pDatabase->GetIncludedFiles(),
		pDatabase->GetIncludedDirectories(),
		pDatabase->GetExcludedFiles(),
		pDatabase->GetExcludedDirectories().GetData(),
		pDatabase->GetExcludedDirectories().GetSize());
	
	m_szExtra2=pDatabase->ConstructExtraBlock();
}
		
CDatabaseUpdater::DBArchive::DBArchive(LPCWSTR szArchiveName,CDatabase::ArchiveType nArchiveType,
											  LPCWSTR szAuthor,LPCWSTR szComment,LPCWSTR* pszRoots,DWORD nNumberOfRoots,BYTE nFlags,
											  LPCWSTR szIncludedFiles,LPCWSTR szIncludedDirectories,LPCWSTR szExcludedFiles,LPCWSTR* ppExcludedDirectories,int nExcludedDirectories,LPCWSTR szRootMaps)
:	m_sAuthor(szAuthor),m_sComment(szComment),m_nArchiveType(nArchiveType),
	m_szName(NULL),m_dwNameLength(0),m_nFlags(nFlags),
	m_szExtra1(NULL),m_szExtra2(NULL),
	m_aIncludeFilesPatternsA(NULL),m_aIncludeDirectoriesPatternsA(NULL),
	m_aExcludeFilesPatternsA(NULL),m_wID(0)
{
	m_szArchive=alloccopy(szArchiveName);
	
	if (!IsUnicodeSystem())
		m_nFlags&=~Unicode;

	if (nNumberOfRoots==0)
	{
		m_pFirstRoot=NULL;
		return;
	}

	int nMapLen;
	LPCWSTR pDriveInDb=CDatabase::FindRootMap(szRootMaps,pszRoots[0],nMapLen);
	CRootDirectory* pCurrent=NULL;
	CreateRootDirectories(pCurrent,pszRoots[0],istrlen(pszRoots[0]),szRootMaps);
	for (DWORD i=1;i<nNumberOfRoots;i++)
		CreateRootDirectories(pCurrent,pszRoots[i],istrlen(pszRoots[i]),szRootMaps);
	pCurrent->m_pNext=NULL;

	ParseFilePatternsAndExcludedDirectories(szIncludedFiles,szIncludedDirectories,szExcludedFiles,ppExcludedDirectories,nExcludedDirectories);

	CDatabaseInfo::ReadFilesAndDirectoriesCount(m_nArchiveType,m_szArchive,
		m_dwExpectedFiles,m_dwExpectedDirectories);

	
}

CDatabaseUpdater::DBArchive::~DBArchive()
{
	CRootDirectory* m_pCurrentRoot;

	// Cleaning data
	while (m_pFirstRoot!=NULL)
	{
		m_pCurrentRoot=m_pFirstRoot->m_pNext;
		delete m_pFirstRoot;
		m_pFirstRoot=m_pCurrentRoot;
	}

	
	if (m_szName!=NULL)
        delete[] m_szName;

	if (m_szArchive!=NULL)
        delete[] m_szArchive;

	if (m_szExtra1!=NULL)
		delete[] m_szExtra1;
	if (m_szExtra2!=NULL)
		delete[] m_szExtra2;

	if (m_nFlags&Unicode)
	{
		if (m_aIncludeFilesPatternsA!=NULL)
		{
			for (int i=0;m_aIncludeFilesPatternsW[i]!=NULL;i++)
				delete[] m_aIncludeFilesPatternsW[i];
			delete[] m_aIncludeFilesPatternsW;
		}
		if (m_aIncludeDirectoriesPatternsA!=NULL)
		{
			for (int i=0;m_aIncludeDirectoriesPatternsW[i]!=NULL;i++)
				delete[] m_aIncludeDirectoriesPatternsW[i];
			delete[] m_aIncludeDirectoriesPatternsW;
		}
		if (m_aExcludeFilesPatternsA!=NULL)
		{
			for (int i=0;m_aExcludeFilesPatternsW[i]!=NULL;i++)
				delete[] m_aExcludeFilesPatternsW[i];
			delete[] m_aExcludeFilesPatternsW;
		}
	}
	else
	{
		if (m_aIncludeFilesPatternsA!=NULL)
		{
			for (int i=0;m_aIncludeFilesPatternsA[i]!=NULL;i++)
				delete[] m_aIncludeFilesPatternsA[i];
			delete[] m_aIncludeFilesPatternsA;
		}
		if (m_aIncludeDirectoriesPatternsA!=NULL)
		{
			for (int i=0;m_aIncludeDirectoriesPatternsA[i]!=NULL;i++)
				delete[] m_aIncludeDirectoriesPatternsA[i];
			delete[] m_aIncludeDirectoriesPatternsA;
		}
		if (m_aExcludeFilesPatternsA!=NULL)
		{
			for (int i=0;m_aExcludeFilesPatternsA[i]!=NULL;i++)
				delete[] m_aExcludeFilesPatternsA[i];
			delete[] m_aExcludeFilesPatternsA;
		}
	}
}


void CDatabaseUpdater::DBArchive::CreateRootDirectories(CRootDirectory*& pCurrent,LPCWSTR pRoot,
														DWORD dwLength,LPCWSTR szRootMaps)
{
	if ((dwLength==2 || dwLength==3) && pRoot[1]==L':') // Root is drive
	{
		WCHAR drive[]=L"X:\\";
		drive[0]=pRoot[0];

		int nMapLen;
		LPCWSTR pDriveInDb=CDatabase::FindRootMap(szRootMaps,pRoot,nMapLen);
		CRootDirectory* pNewDirectory=new CRootDirectory(drive,dwLength,pDriveInDb,
			nMapLen,m_nFlags);

		if (pCurrent==NULL)
			pCurrent=m_pFirstRoot=pNewDirectory;
		else
			pCurrent=pCurrent->m_pNext=pNewDirectory;
	}
	else if (pRoot[0]=='\\' && pRoot[1]=='\\')
	{
		int nMapLen;
		LPCWSTR pDriveInDb=CDatabase::FindRootMap(szRootMaps,pRoot,nMapLen);
			
		if (FileSystem::IsDirectory(pRoot))
		{
			CRootDirectory* pNewDirectory=new CRootDirectory(pRoot,dwLength,
				pDriveInDb,nMapLen,m_nFlags);

			if (pCurrent==NULL)
				pCurrent=m_pFirstRoot=pNewDirectory;
			else
				pCurrent=pCurrent->m_pNext=pNewDirectory;
		}
		else
		{
			// May be computer, since it is not directory
			// So retrieve shares
			DWORD dwEntries=-1,cbBuffer=16384,dwRet;
			HANDLE hEnum;
			
			if (IsUnicodeSystem())
			{
				NETRESOURCEW nr;
				nr.dwScope=RESOURCE_CONNECTED;
				nr.dwType=RESOURCETYPE_DISK;
				nr.dwDisplayType=RESOURCEDISPLAYTYPE_SHARE;
				nr.dwUsage=RESOURCEUSAGE_CONTAINER;
				nr.lpLocalName=NULL;
				nr.lpRemoteName=const_cast<LPWSTR>(pRoot);
				nr.lpComment=NULL;
				nr.lpProvider=NULL;
				dwRet=WNetOpenEnumW(RESOURCE_GLOBALNET,RESOURCETYPE_DISK,0,&nr,&hEnum);
			}
			else
			{
				NETRESOURCE nr;
				nr.dwScope=RESOURCE_CONNECTED;
				nr.dwType=RESOURCETYPE_DISK;
				nr.dwDisplayType=RESOURCEDISPLAYTYPE_SHARE;
				nr.dwUsage=RESOURCEUSAGE_CONTAINER;
				nr.lpLocalName=NULL;
				nr.lpRemoteName=alloccopyWtoA(pRoot);
				nr.lpComment=NULL;
				nr.lpProvider=NULL;
				dwRet=WNetOpenEnumA(RESOURCE_GLOBALNET,RESOURCETYPE_DISK,0,&nr,&hEnum);
				delete[] nr.lpRemoteName;
			}

			if (dwRet!=NOERROR)
			{
				CRootDirectory* pNewDirectory=new CRootDirectory(pRoot,dwLength,
					pDriveInDb,nMapLen,m_nFlags);

				if (pCurrent==NULL)
					pCurrent=m_pFirstRoot=pNewDirectory;
				else
					pCurrent=pCurrent->m_pNext=pNewDirectory;
				return;
			}


			union {
				NETRESOURCEW* nrow;
				NETRESOURCE* nro;
			};
				
			nro=(NETRESOURCE*)GlobalAlloc(GPTR,cbBuffer);
			DebugOpenMemBlock(nro);

			for(;;)
			{
				if (IsUnicodeSystem())
					dwRet=WNetEnumResourceW(hEnum,&dwEntries,nrow,&cbBuffer);
				else
					dwRet=WNetEnumResource(hEnum,&dwEntries,nro,&cbBuffer);

				ASSERT(cbBuffer==16384);

				if (dwRet==ERROR_NO_MORE_ITEMS)
					break;

				if (dwRet!=ERROR_MORE_DATA && dwRet!=NOERROR)
				{
					CRootDirectory* pNewDirectory=new CRootDirectory(pRoot,dwLength,pDriveInDb,
						nMapLen,m_nFlags);

					if (pCurrent==NULL)
						pCurrent=m_pFirstRoot=pNewDirectory;
					else
						pCurrent=pCurrent->m_pNext=pNewDirectory;

					break;
				}

				for (DWORD i=0;i<dwEntries;i++)
				{
					LPWSTR pRemoteName=IsUnicodeSystem()?
						nrow[i].lpRemoteName:alloccopyAtoW(nro[i].lpRemoteName);

					CRootDirectory* pNewDirectory;
					BOOL bAdded=FALSE;

					if (pDriveInDb!=NULL)
					{
						if (_wcsnicmp(pRemoteName,pRoot,dwLength)==0)
						{
							// Begin of remote name is ok
							int nLen=istrlenw(pRemoteName+dwLength);
							WCHAR* szMap=new WCHAR[nMapLen+nLen+1];
							MemCopyW(szMap,pDriveInDb,nMapLen);
							MemCopyW(szMap+nMapLen,pRemoteName+dwLength,nLen+1);
							pNewDirectory=new CRootDirectory(pRemoteName,szMap,nMapLen+nLen,m_nFlags);
							bAdded=TRUE;
						}
					}

					if (!bAdded)
					{
						// Check chares individially
						int nMapLen2;
						LPCWSTR pDriveInDb2=CDatabase::FindRootMap(szRootMaps,pRemoteName,nMapLen2);
						pNewDirectory=new CRootDirectory(pRemoteName,pDriveInDb2,nMapLen2,m_nFlags);
					}
					
					if (pCurrent==NULL)
						pCurrent=m_pFirstRoot=pNewDirectory;
					else
						pCurrent=pCurrent->m_pNext=pNewDirectory;

					if (!IsUnicodeSystem())
						delete[] pRemoteName;
				}

			}
	
			DebugCloseMemBlock(nro);
			GlobalFree((HGLOBAL)nro);
			WNetCloseEnum(hEnum);	
		}
	}
	else
	{
		int nMapLen;
		LPCWSTR pDriveInDb=CDatabase::FindRootMap(szRootMaps,pRoot,nMapLen);
		CRootDirectory* pNewDirectory=new CRootDirectory(pRoot,dwLength,pDriveInDb,nMapLen,m_nFlags);

		if (pCurrent==NULL)
			pCurrent=m_pFirstRoot=pNewDirectory;
		else
			pCurrent=pCurrent->m_pNext=pNewDirectory;
	}

}

LPSTR* CDatabaseUpdater::DBArchive::ParsePatternListFromStringA(LPCWSTR szString)
{
	// Parsing included files
	if (szString==NULL)
		return NULL;

	if (szString[0]=='\0')
		return NULL;

	// Counting
	DWORD nPatterns=0;
	LPCWSTR pPtr;
	BOOL bPatternEmpty=TRUE;

	for (pPtr=szString;*pPtr!='\0';pPtr++)
	{
		if (*pPtr==L';' || *pPtr==L',')
		{
			if (!bPatternEmpty)
			{
				nPatterns++;
				bPatternEmpty=TRUE;
			}
		}
		else if (*pPtr!=L' ')
			bPatternEmpty=FALSE;
	}
	// Last pattern ok
	if (!bPatternEmpty)
		nPatterns++;

	if (nPatterns==0)
		return NULL;


	LPSTR* pRet=new CHAR*[nPatterns+1];

	nPatterns=0;
	pPtr=szString;

	for (;;)
	{
		DWORD nLength,nActualLength;
		bPatternEmpty=TRUE;
		
		while (*pPtr==L' ')
			pPtr++;
		
		for (nLength=0;pPtr[nLength]!='\0' && pPtr[nLength]!=';' && pPtr[nLength]!=',';nLength++);

		for (nActualLength=nLength;nActualLength>0 && pPtr[nActualLength-1]==L' ';nActualLength--);


		if (nActualLength>0)
		{

			pRet[nPatterns]=alloccopyWtoA(pPtr,nActualLength);
			CharLowerBuffA(pRet[nPatterns],nActualLength);
			nPatterns++;
		}

		pPtr+=nLength;

		if (*pPtr=='\0')
			break;

		pPtr++;
	}

	pRet[nPatterns]=NULL;

	return pRet;
}

LPWSTR* CDatabaseUpdater::DBArchive::ParsePatternListFromStringW(LPCWSTR szString)
{
	// Parsing included files
	if (szString==NULL)
		return NULL;

	if (szString[0]=='\0')
		return NULL;

	// Counting
	DWORD nPatterns=0;
	LPCWSTR pPtr;
	BOOL bPatternEmpty=TRUE;

	for (pPtr=szString;*pPtr!='\0';pPtr++)
	{
		if (*pPtr==L';' || *pPtr==L',')
		{
			if (!bPatternEmpty)
			{
				nPatterns++;
				bPatternEmpty=TRUE;
			}
		}
		else if (*pPtr!=L' ')
			bPatternEmpty=FALSE;
	}
	// Last pattern ok
	if (!bPatternEmpty)
		nPatterns++;

	if (nPatterns==0)
		return NULL;


	LPWSTR* pRet=new WCHAR*[nPatterns+1];

	nPatterns=0;
	pPtr=szString;

	for (;;)
	{
		DWORD nLength,nActualLength;
		bPatternEmpty=TRUE;
		
		while (*pPtr==L' ')
			pPtr++;
		
		for (nLength=0;pPtr[nLength]!='\0' && pPtr[nLength]!=';' && pPtr[nLength]!=',';nLength++);

		for (nActualLength=nLength;nActualLength>0 && pPtr[nActualLength-1]==L' ';nActualLength--);


		if (nActualLength>0)
		{
			pRet[nPatterns]=alloccopy(pPtr,nActualLength);
			CharLowerBuffW(pRet[nPatterns],nActualLength);
			nPatterns++;
		}

		pPtr+=nLength;

		if (*pPtr=='\0')
			break;

		pPtr++;
	}

	pRet[nPatterns]=NULL;

	return pRet;
}



void CDatabaseUpdater::DBArchive::ParseFilePatternsAndExcludedDirectories(LPCWSTR szIncludedFiles,
																		  LPCWSTR szIncludedDirectories,
																		  LPCWSTR szExcludedFiles,
																		  const LPCWSTR* ppExcludedDirs,
																		  int nExcludedDirectories)
{
	// Parse included files and directories and excluded files
	if (m_nFlags&Unicode)
	{
		m_aIncludeFilesPatternsW=ParsePatternListFromStringW(szIncludedFiles);
		m_aIncludeDirectoriesPatternsW=ParsePatternListFromStringW(szIncludedDirectories);
		m_aExcludeFilesPatternsW=ParsePatternListFromStringW(szExcludedFiles);
	}
	else
	{
		m_aIncludeFilesPatternsA=ParsePatternListFromStringA(szIncludedFiles);
		m_aIncludeDirectoriesPatternsA=ParsePatternListFromStringA(szIncludedDirectories);
		m_aExcludeFilesPatternsA=ParsePatternListFromStringA(szExcludedFiles);
	}



	// Parsing included/excluded files and directories
	if (nExcludedDirectories==0)
	{
		// Just place excluded files and exit
		if (m_aIncludeFilesPatternsA!=NULL || m_aIncludeDirectoriesPatternsA!=NULL || m_aExcludeFilesPatternsA!=NULL)
		{
			CRootDirectory* pRoot=m_pFirstRoot;
			while (pRoot!=NULL)
			{
				pRoot->m_aIncludeFilesPatternsA=m_aIncludeFilesPatternsA;
				pRoot->m_aIncludeDirectoriesPatternsA=m_aIncludeDirectoriesPatternsA;
				pRoot->m_aExcludeFilesPatternsA=m_aExcludeFilesPatternsA;
				pRoot=pRoot->m_pNext;
			}
		}
		return;
	}

	// Parsing directories
	// First, check that directory names are valid
	LPWSTR* ppExcludedDirectories=new LPWSTR[max(nExcludedDirectories,2)];
	DWORD* pdwExcludedDirectoriesLen=new DWORD[max(nExcludedDirectories,2)];
	BOOL* pbIsPattern=new BOOL[max(nExcludedDirectories,2)];
	int i,j;
	for (i=0,j=0;i<nExcludedDirectories;i++)
	{
		if (ppExcludedDirs[i][0]==L'\0')
			continue;
		if (ppExcludedDirs[i][1]==L'\0')
			continue;

		if (ppExcludedDirs[i][1]==L':' && ppExcludedDirs[i][2]==L'\0')
		{
			ppExcludedDirectories[j]=new WCHAR[3];
			ppExcludedDirectories[j][0]=ppExcludedDirs[i][0];
			ppExcludedDirectories[j][1]=':';
			ppExcludedDirectories[j][2]='\0';
			MakeLower(ppExcludedDirectories[j]);
			
			pdwExcludedDirectoriesLen[j]=2;
			pbIsPattern[j]=FALSE;

			j++;
			continue;
		}

		if (FirstCharIndex(ppExcludedDirs[i],L'*')==-1 && FirstCharIndex(ppExcludedDirs[i],L'?')==-1)
		{
			WCHAR szBuffer[400];
			int nRet=FileSystem::GetFullPathName(ppExcludedDirs[i],400,szBuffer,NULL);

			if (!nRet)
				continue;

			if (szBuffer[nRet-1]==L'\\')
				szBuffer[--nRet]=L'\0';
	
			MakeLower(szBuffer);
			pdwExcludedDirectoriesLen[j]=nRet;
			ppExcludedDirectories[j]=alloccopy(szBuffer,nRet);
			pbIsPattern[j]=FALSE;
			j++;
		}
		else
		{
			int nLen=istrlen(ppExcludedDirs[i]);
			if (ppExcludedDirs[i][nLen-1]==L'\\')
				nLen--;
			
			pdwExcludedDirectoriesLen[j]=nLen;
			ppExcludedDirectories[j]=alloccopy(ppExcludedDirs[i],nLen);
			MakeLower(ppExcludedDirectories[j]);
			pbIsPattern[j]=TRUE;
			j++;
		}

	}
	nExcludedDirectories=j;



	// Then set included/excluded files and directories for roots
	CRootDirectory* pRoot=m_pFirstRoot;
	
	while (pRoot!=NULL)
	{
		DWORD nRootLength=(DWORD)pRoot->m_Path.GetLength();
		if (pRoot->m_Path.LastChar()=='\\')
			nRootLength--;

		// Copying text to another buffer to get lowercase version
		WCHAR* pLowerRoot=alloccopy(pRoot->m_Path,nRootLength);
		MakeLower(pLowerRoot);

		BOOL bDeleteRoot=FALSE;

		for (int i=0;i<nExcludedDirectories;i++)
		{
			if (pbIsPattern[i])
			{
				// Excluded directory is pattern
				pRoot->m_aExcludedDirectories.Add(alloccopy(ppExcludedDirectories[i],
					pdwExcludedDirectoriesLen[i]));
				continue;
			}

			// Excluded path is shorter than root, excluded dir cannot be subdir of root
			if (pdwExcludedDirectoriesLen[i]<nRootLength)
				continue;

			// Beginning of paths are not same
			if (wcsncmp(pLowerRoot,ppExcludedDirectories[i],nRootLength)!=0)
				continue;
			
			if (nRootLength==pdwExcludedDirectoriesLen[i])
			{
				bDeleteRoot=TRUE;
				break;
			}
			
			if (ppExcludedDirectories[i][nRootLength]=='\\')
			{
				// Excluded directory is inside of root
				pRoot->m_aExcludedDirectories.Add(alloccopy(ppExcludedDirectories[i],
					pdwExcludedDirectoriesLen[i]));
			}
		}
		delete[] pLowerRoot;

		if (bDeleteRoot)
		{
			// Root is excluded, removing whole root
			if (pRoot==m_pFirstRoot)
			{
				m_pFirstRoot=pRoot->m_pNext;
				delete pRoot;
				pRoot=m_pFirstRoot;
			}
			else
			{
				// Resolving previous item
				CRootDirectory* pPrevRoot=m_pFirstRoot;
				while (pPrevRoot->m_pNext!=pRoot)
				pPrevRoot=pPrevRoot->m_pNext;
		
				pPrevRoot->m_pNext=pRoot->m_pNext;
				delete pRoot;
				pRoot=pPrevRoot->m_pNext;
			}
			continue;
		}
		
		// Inserting included/excluded files pattern
		pRoot->m_aIncludeFilesPatternsA=m_aIncludeFilesPatternsA;
		pRoot->m_aIncludeDirectoriesPatternsA=m_aIncludeDirectoriesPatternsA;
		pRoot->m_aExcludeFilesPatternsA=m_aExcludeFilesPatternsA;
			
		// Next root
		pRoot=pRoot->m_pNext;
	}

	for (int i=0;i<nExcludedDirectories;i++)
		delete[] ppExcludedDirectories[i];
	delete[] ppExcludedDirectories;
	delete[] pdwExcludedDirectoriesLen;
	delete[] pbIsPattern;
}

CFile* CDatabaseUpdater::OpenDatabaseFileForIncrementalUpdate(LPCWSTR szArchive,DWORD dwFiles,DWORD dwDirectories,BOOL bUnicode)
{
	DebugFormatMessage("CDatabaseUpdater::OpenDatabaseFileForIncrementalUpdate(): BEGIN, archive='%s'",szArchive);
	
	CFile* dbFile=NULL;
	
	try {
		dbFile=new CFile(szArchive,
			CFile::modeReadWrite|CFile::openExisting|CFile::shareDenyWrite|CFile::shareDenyRead|CFile::otherStrNullTerminated,TRUE);
		if (dbFile==NULL)
		{
			// Cannot open file, incremental update not possible
			return NULL;
		}
		dbFile->CloseOnDelete();
	
		dbFile->SeekToBegin();
	
		// Reading and verifing header
		char szBuffer[11];
		dbFile->Read(szBuffer,11);

		if (szBuffer[0]!='L' || szBuffer[1]!='O' || 
			szBuffer[2]!='C' || szBuffer[3]!='A' || 
			szBuffer[4]!='T' || szBuffer[5]!='E' || 
			szBuffer[6]!='D' || szBuffer[7]!='B' ||
			szBuffer[8]!='2' || szBuffer[9]!='0' )
		{
			throw CFileException(CFileException::invalidFile,
#ifdef WIN32
				-1,
#endif
				szArchive);
		}

		if ((bUnicode && !(szBuffer[10]&0x20)) ||
			(!bUnicode && (szBuffer[10]&0x20)))
		{
			throw CFileException(CFileException::invalidFormat,
#ifdef WIN32
				-1,
#endif
				szArchive);

		}

	
		// Updating file and directory counts
		DWORD dwBlockSize;
		dbFile->Read(dwBlockSize);
		dbFile->Seek(dwBlockSize-2*sizeof(DWORD),CFile::current);
		
		DWORD dwTemp1,dwTemp2;
		dbFile->Read(dwTemp1);
		dbFile->Read(dwTemp2);

		dwTemp1+=dwFiles;
		dwTemp2+=dwDirectories;
		dbFile->Seek(-2*LONG(sizeof(DWORD)),CFile::current);

		dbFile->Write(dwTemp1);
		dbFile->Write(dwTemp2);

		// Searching enf of file
		dbFile->Read(dwBlockSize);
		while (dwBlockSize>0)
		{
			dbFile->Seek(dwBlockSize,CFile::current);
			dbFile->Read(dwBlockSize);
		}


		// Now we should be in the end of file
		ASSERT(dbFile->IsEndOfFile());

		dbFile->Seek(-LONG(sizeof(DWORD)),CFile::current);
	}
	catch (CFileException fe)
	{
		DebugFormatMessage("CDatabaseUpdater::OpenDatabaseFileForIncrementalUpdate(): fe.m_cause=%d",fe.m_cause);
			
		if (dbFile!=NULL)
			delete dbFile;
		switch (fe.m_cause)
		{
		case CFileException::invalidFile:
		case CFileException::sharingViolation:
		case CFileException::endOfFile:
		case CFileException::writeProtected:
		case CFileException::writeFault:
		case CFileException::lockViolation:
		case CFileException::accessDenied:
		case CFileException::readFault:
			DebugMessage("CDatabaseUpdater::OpenDatabaseFileForIncrementalUpdate(): END2");
			return (CFile*)-1;
		case CFileException::invalidFormat:
			DebugMessage("CDatabaseUpdater::OpenDatabaseFileForIncrementalUpdate(): END4");
			return (CFile*)-2;
		default:
			DebugMessage("CDatabaseUpdater::OpenDatabaseFileForIncrementalUpdate(): END3");
			return NULL;
		}
	}
	catch (...)
	{
		DebugFormatMessage("CDatabaseUpdater::OpenDatabaseFileForIncrementalUpdate(): caugh unknown exception");

		if (dbFile!=NULL)
			delete dbFile;
	
		DebugMessage("CDatabaseUpdater::OpenDatabaseFileForIncrementalUpdate(): END4");
		return NULL;
	}

	DebugFormatMessage("CDatabaseUpdater::OpenDatabaseFileForIncrementalUpdate(): END, will return='%X'",dbFile);
	return dbFile;
}

