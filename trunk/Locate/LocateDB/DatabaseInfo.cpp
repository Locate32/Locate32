/* Copyright (c) 1997-2010 Janne Huttunen
   database updater v3.1.8.6070              */

#include <HFCLib.h>
#include "Locatedb.h"

CDatabaseInfo::~CDatabaseInfo()
{
	if (m_pFile!=NULL)
	{
		CancelIo(*m_pFile);
		delete m_pFile;
	}
	if (m_szBuffer!=NULL)
		delete[] m_szBuffer;

}

BOOL CDatabaseInfo::GetInfo(CDatabase::ArchiveType nArchiveType,LPCWSTR szArchivePath,LPCWSTR szRootMaps)
{
	sCreator.Empty();
	sDescription.Empty();
	aRootFolders.RemoveAll();

	BOOL bRet=TRUE;

	
	try
	{
		switch (nArchiveType)
		{
		case CDatabase::archiveFile:
			m_pFile=new CFile(TRUE);
			m_pFile->Open(szArchivePath,CFile::defRead|CFile::otherErrorWhenEOF);
			break;
		default:
			throw CFileException(CFileException::notImplemented,-1,szArchivePath);
		}
			
		m_szBuffer=new BYTE[10];
		
		dwFileSize=m_pFile->GetLength();
		
		m_pFile->Read(m_szBuffer,9);

		if (m_szBuffer[0]!='L' || m_szBuffer[1]!='O' || 
			m_szBuffer[2]!='C' || m_szBuffer[3]!='A' || 
			m_szBuffer[4]!='T' || m_szBuffer[5]!='E' || 
			m_szBuffer[6]!='D' || m_szBuffer[7]!='B')
		{
			throw CFileException(CFileException::invalidFormat,-1,szArchivePath);
		}
	
		if (m_szBuffer[8]>='0') // New database format
		{
			DWORD dwTemp;
			bVersion=(m_szBuffer[8]-'0')*10;
			
			m_pFile->Read(m_szBuffer,2);
			bVersion+=(m_szBuffer[0]-'0');
			bLongFilenames=m_szBuffer[1]&0x1;
			if (m_szBuffer[1]&0x20)
				cCharset=Unicode;
			else if (m_szBuffer[1]&0x10)
				cCharset=Ansi;
			else
				cCharset=OEM;
			delete[] m_szBuffer;
			m_szBuffer=NULL;


			// Reading header size
			m_pFile->Read(dwTemp);

			if (cCharset==Unicode)
			{
				// Reading creator and description
				m_pFile->Read(sCreator);
				m_pFile->Read(sDescription);
				m_pFile->Read(sExtra1);
				m_pFile->Read(sExtra2);
			}
			else
			{
				CString strA;
				// Reading creator and description
				m_pFile->Read(strA);
				sCreator=strA;

				m_pFile->Read(strA);
				sDescription=strA;

				// Reading extra data, for future use
				m_pFile->Read(strA);
				sExtra1=strA;
				m_pFile->Read(strA);
				sExtra2=strA;
			}

			

			// Reading time
			m_pFile->Read(dwTemp);
			tCreationTime=CTime(LOWORD(dwTemp),HIWORD(dwTemp));

			// Reading number of files and directories
			m_pFile->Read(dwNumberOfFiles);
			m_pFile->Read(dwNumberOfDirectories);

			// Reading drive/directory information
			DWORD dwBlockSize;
			m_pFile->Read(dwBlockSize);
			while (dwBlockSize>0)
			{
				CRoot* pRoot=new CRoot;
				
				DWORD dwSeekLength=dwBlockSize-1-4-4-4;

				// Reading type and path
				m_pFile->Read((BYTE*)&pRoot->rtType,1);
				
				if (cCharset==Unicode)
				{
					m_pFile->Read(pRoot->sPath);
	
					m_pFile->Read(pRoot->sVolumeName);

					m_pFile->Read(pRoot->dwVolumeSerial);
				
					m_pFile->Read(pRoot->sFileSystem);

					dwSeekLength-=((DWORD)pRoot->sPath.GetLength()+1+
						(DWORD)pRoot->sVolumeName.GetLength()+1+(DWORD)pRoot->sFileSystem.GetLength()+1)*2;

					
				}
				else
				{
					CString strA;
					m_pFile->Read(strA);
					pRoot->sPath=strA;
				
					// Reading volume name and serial and filesystem
					m_pFile->Read(strA);
					pRoot->sVolumeName=strA;

					m_pFile->Read(pRoot->dwVolumeSerial);
				
					m_pFile->Read(strA);
					pRoot->sFileSystem=strA;

					dwSeekLength-=(DWORD)pRoot->sPath.GetLength()+1+
						(DWORD)pRoot->sVolumeName.GetLength()+1+(DWORD)pRoot->sFileSystem.GetLength()+1;

				}
				
		
				// Reading number of files and directories
				m_pFile->Read(pRoot->dwNumberOfFiles);
				m_pFile->Read(pRoot->dwNumberOfDirectories);
				

				// Resolve map
				int nActualPathLen;
				BOOL bFree;
				LPCWSTR pActualPath=CDatabase::FindActualPathForMap(
					szRootMaps,pRoot->sPath,nActualPathLen,bFree);
				if (pActualPath!=NULL)
				{
					pRoot->sRootMap.Copy(pActualPath,nActualPathLen);
					if (bFree)
						delete[] pActualPath;
				}

				aRootFolders.Add(pRoot);

				
				m_pFile->Seek(dwSeekLength,CFile::current);
				m_pFile->Read(dwBlockSize);
			}

			m_pFile->Close();
		}
		else if (m_szBuffer[8]>=1 && m_szBuffer[8]<=4)
		{
			bVersion=m_szBuffer[8];
			delete[] m_szBuffer;
			m_szBuffer=NULL;

			cCharset=OEM;
			switch (bVersion)
			{
			case 2:
			case 4:
				bLongFilenames=TRUE;
				break;
			default:
				bLongFilenames=FALSE;
				break;
			}				
			
			CString strA;
			// Reading creator and description
			m_pFile->Read(strA);
			sCreator=strA;
			m_pFile->Read(strA);
			sDescription=strA;
			
			// Resolving drives
			CString sDrives;
			m_pFile->Read(sDrives);
			
			for (int i=0;i<sDrives.GetLength();i++)
				aRootFolders.Add(new CRoot(sDrives[i]));	
				
			DWORD nTime;
			m_pFile->Read(&nTime,4);
			tCreationTime=(time_t)nTime;
			m_pFile->Close();
		}
		else
			throw CFileException(CFileException::invalidFormat,-1,szArchivePath);
	}
	catch (...)
	{
		bRet=FALSE;
	}
	if (m_pFile!=NULL)
	{
		delete m_pFile;
		m_pFile=NULL;
	}
	if (m_szBuffer!=NULL)
	{
		delete[] m_szBuffer;
		m_szBuffer=NULL;
	}
	return bRet;
}




BOOL CDatabaseInfo::GetRootsFromDatabases(CArray<LPWSTR>& aRoots,const PDATABASE* pDatabases,int nDatabases,BOOL bOnlyEnabled)
{
	for (int nDB=0;nDB<nDatabases;nDB++)
	{
		if (bOnlyEnabled && !pDatabases[nDB]->IsEnabled())
			continue;

		CArray<LPWSTR> aDBRoots;
		if (!GetRootsFromDatabase(aDBRoots,pDatabases[nDB]))
			continue;
		
		// Checking that root does not already exists
		for (int i=0;i<aRoots.GetSize();i++)
		{
			int j=0;
			while (j<aDBRoots.GetSize())
			{
				if (strcasecmp(aDBRoots[j],aRoots[i])==0)
				{
					// Already exists, deleting
					delete[] aDBRoots[j];
					aDBRoots.RemoveAt(j);
				}
				else
					j++;
			}
		}

		// Inserting new roots
		for (int i=0;i<aDBRoots.GetSize();i++)
			aRoots.Add(aDBRoots[i]);
	}		
	return TRUE;
}


BOOL CDatabaseInfo::GetRootsFromDatabase(CArray<LPWSTR>& aRoots,const CDatabase* pDatabase)
{
	BYTE* szBuffer=NULL;
	
	CFile* dbFile=NULL;
	
	BOOL bRet=TRUE;
		
	try
	{
		switch (pDatabase->GetArchiveType())
		{
		case CDatabase::archiveFile:
			{
				BOOL bFree;
				LPWSTR pFile=pDatabase->GetResolvedArchiveName(bFree);
				dbFile=new CFile(pFile,CFile::defRead|CFile::otherErrorWhenEOF,TRUE);
				if (bFree)
					delete[] pFile;
				break;
			}
		default:
			throw CFileException(CFileException::notImplemented,
					-1,pDatabase->GetArchiveName());
		}
		
		szBuffer=new BYTE[11];
		
		dbFile->Read(szBuffer,11);

		if (szBuffer[0]!='L' || szBuffer[1]!='O' || 
			szBuffer[2]!='C' || szBuffer[3]!='A' || 
			szBuffer[4]!='T' || szBuffer[5]!='E' || 
			szBuffer[6]!='D' || szBuffer[7]!='B')
		{
			throw CFileException(CFileException::invalidFormat,-1,pDatabase->GetArchiveName());
		}
	
		if (szBuffer[8]!='2' && szBuffer[9]!='0') // Not supported file format
		{
			throw CFileException(CFileException::invalidFormat,-1,pDatabase->GetArchiveName());
		}

		BOOL bUnicode=szBuffer[10]&0x20;

		delete[] szBuffer;
		szBuffer=NULL;

		// Skipping other header fields
		DWORD dwBlockSize;
		dbFile->Read(dwBlockSize);


		dbFile->Seek(dwBlockSize,CFile::current);

		// Reading drive/directory information
		if (bUnicode)
		{
			CStringW Path;
			BYTE bPathLen;
				
			dbFile->Read(dwBlockSize);
			while (dwBlockSize>0)
			{
				// Reading type and path
				dbFile->Read(bPathLen);
				dbFile->Read(Path);
					
				// Resolve map
				int nActualPathLen;
				BOOL bFree;
				LPCWSTR pActualPath=CDatabase::FindActualPathForMap(
					pDatabase->GetRootMaps(),Path,nActualPathLen,bFree);
				if (pActualPath!=NULL)
				{
					aRoots.Add(alloccopy(pActualPath,nActualPathLen));
					if (bFree)
						delete[] pActualPath;
				}
				else
					aRoots.Add(alloccopy(Path,Path.GetLength()));
							
				dbFile->Seek(dwBlockSize-1-DWORD((Path.GetLength()+1)*2),
					CFile::current);
				
				dbFile->Read(dwBlockSize);
			}
		}
		else
		{
			CString Path;
			BYTE bPathLen;

			dbFile->Read(dwBlockSize);
			while (dwBlockSize>0)
			{
				// Reading type and path
				dbFile->Read(bPathLen);
				dbFile->Read(Path);

				// Resolve map
				int nActualPathLen;
				BOOL bFree;
				LPCWSTR pActualPath=CDatabase::FindActualPathForMap(
					pDatabase->GetRootMaps(),A2W(Path),nActualPathLen,bFree);
				if (pActualPath!=NULL)
				{
					aRoots.Add(alloccopy(pActualPath,nActualPathLen));
					if (bFree)
						delete[] pActualPath;
				}
				else
					aRoots.Add(alloccopyAtoW(Path,Path.GetLength()));
				
							
				dbFile->Seek(dwBlockSize-1-DWORD((Path.GetLength()+1)),
					CFile::current);
				
				dbFile->Read(dwBlockSize);
			}
		}
		dbFile->Close();
	}
	catch (CFileException ex)
	{
#ifdef _DEBUG
		switch (ex.m_cause)
		{
		case CFileException::fileOpen:
		case CFileException::badPath:
		case CFileException::fileNotFound:
			DebugFormatMessage(L"GetRootsFromDatabase:FILEOPEN/BADPATH/NOTFOUND: %s",
				pDatabase->GetArchiveName());
			break;
		case CFileException::readFault:
		case CFileException::fileCorrupt:
		case CFileException::sharingViolation:
		case CFileException::lockViolation:
		case CFileException::accessDenied:
			DebugFormatMessage(L"GetRootsFromDatabase:READFAULT/CORRUPT/SHARING/LOCK: %s",
				pDatabase->GetArchiveName());
			break;
		case CFileException::endOfFile:
		case CFileException::badSeek:
		case CFileException::invalidFile:
			DebugFormatMessage(L"GetRootsFromDatabase:EOF/BADSEEK/INVALID: %s",
				pDatabase->GetArchiveName());
			break;
		default:
			DebugFormatMessage(L"GetRootsFromDatabase:UNKNOWN: %s",
				pDatabase->GetArchiveName());
			break;
		}
#endif
		bRet=FALSE;

	}
	catch (...)
	{
		DebugFormatMessage(L"GetRootsFromDatabase: Unknown error, %s",
			pDatabase->GetArchiveName());
		bRet=FALSE;
	}

	if (dbFile!=NULL)
		delete dbFile;
	if (szBuffer!=NULL)
		delete[] szBuffer;
	return bRet;
}


BOOL CDatabaseInfo::ReadFilesAndDirectoriesCount(CDatabase::ArchiveType nArchiveType,LPCWSTR szArchive,DWORD& dwFiles,DWORD& dwDirectories)
{
	CFile* dbFile=NULL;
	BOOL bRet=TRUE;
	
	dwFiles=DWORD(-1);
	dwDirectories=DWORD(-1);

	if (szArchive==NULL)
		return FALSE;
		
	try
	{
		switch (nArchiveType)
		{
		case CDatabase::archiveFile:
			dbFile=new CFile(szArchive,CFile::defRead|CFile::otherErrorWhenEOF,TRUE);
			dbFile->CloseOnDelete();
			break;
		default:
			throw CFileException(CFileException::notImplemented,
					-1,szArchive);
		}
			
		BYTE szBuffer[11];
		
		dbFile->Read(szBuffer,11);
		if (szBuffer[0]!='L' || szBuffer[1]!='O' || 
			szBuffer[2]!='C' || szBuffer[3]!='A' || 
			szBuffer[4]!='T' || szBuffer[5]!='E' || 
			szBuffer[6]!='D' || szBuffer[7]!='B' ||
			szBuffer[8]<'0')
		{
			throw CFileException(CFileException::invalidFormat,-1,szArchive);
		}
	
		DWORD dwHeaderSize;
		dbFile->Read(dwHeaderSize);

		dbFile->Seek(dwHeaderSize-2*sizeof(DWORD),CFile::current);

		/*// Reading creator, description and reserved data
		CString Temp;
		dbFile->Read(Temp);
		dbFile->Read(Temp);
		dbFile->Read(Temp);
		dbFile->Read(Temp);

		dbFile->Seek(4,CFile::current);*/
		
		dbFile->Read(dwFiles);
		dbFile->Read(dwDirectories);

		dbFile->Close();
		
	}
	catch (...)
	{
		bRet=FALSE;
	}
	if (dbFile!=NULL)
		delete dbFile;
	return bRet;
}