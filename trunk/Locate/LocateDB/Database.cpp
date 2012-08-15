/* Copyright (c) 1997-2010 Janne Huttunen
   database updater v3.1.8.9210              */

#include <HFCLib.h>
#include "Locatedb.h"
#include "../common/common.h"

CDatabase::CDatabase(CDatabase& src)
{
	sMemCopy(this,&src,sizeof(CDatabase));
	// This cleares m_aExcludedDirectories to null to prevent invalid memory handling
    m_aExcludedDirectories.GiveBuffer();
	
	if (m_szName!=NULL)
		m_szName=alloccopy(m_szName);
	if (m_szCreator!=NULL)
		m_szCreator=alloccopy(m_szCreator);
	if (m_szDescription!=NULL)
		m_szDescription=alloccopy(m_szDescription);
	if (m_szArchiveName!=NULL)
		m_szArchiveName=alloccopy(m_szArchiveName);
	if (m_szIncludedFiles!=NULL)
		m_szIncludedFiles=alloccopy(m_szIncludedFiles);
	if (m_szIncludedDirectories!=NULL)
		m_szIncludedDirectories=alloccopy(m_szIncludedDirectories);
	if (m_szExcludedFiles!=NULL)
		m_szExcludedFiles=alloccopy(m_szExcludedFiles);
	if (m_szRootMaps!=NULL)
		m_szRootMaps=alloccopy(m_szRootMaps);

	
	if (m_szRoots!=NULL)
	{
		DWORD iLength;
		for (iLength=0;m_szRoots[iLength]!=L'\0' || m_szRoots[iLength+1]!=L'\0';iLength++);
		iLength+=2;
        m_szRoots=new WCHAR[iLength];
		MemCopyW(m_szRoots,src.m_szRoots,iLength);
	}

	for (int i=0;i<src.m_aExcludedDirectories.GetSize();i++)
		m_aExcludedDirectories.Add(alloccopy(src.m_aExcludedDirectories[i]));
}

BOOL CDatabase::LoadFromRegistry(HKEY hKeyRoot,LPCSTR szPath,CArray<CDatabase*>& aDatabases)
{
	CRegKey RegKey;
	CString Path(szPath);
	Path<<'\\';
	
	
	if (RegKey.OpenKey(hKeyRoot,Path,CRegKey::defRead|CRegKey::samEnumerateSubkeys)!=ERROR_SUCCESS)
		return FALSE;

    CString key;
	for (int i=0;RegKey.EnumKey(i,key);i++)
	{
		CDatabase* pDatabase=CDatabase::FromKey(RegKey,"",key);
		
		if (pDatabase!=NULL)
		{
			if (pDatabase->m_wID==0)
				pDatabase->m_wID=GetUniqueIndex(aDatabases);
			aDatabases.Add(pDatabase);
		}
	}

	return TRUE;
}

CDatabase* CDatabase::FromName(HKEY hKeyRoot,LPCSTR szPath,LPCWSTR szName,DWORD dwNameLength)
{
	CRegKey RegKey,RegKey2;
	CString Path(szPath);
	Path<<'\\';
	
	
	if (RegKey.OpenKey(hKeyRoot,Path,CRegKey::defRead|CRegKey::samEnumerateSubkeys)!=ERROR_SUCCESS)
		return NULL;

	if (dwNameLength==DWORD(-1))
		dwNameLength=istrlenw(szName);

    CString key;
	for (int i=0;RegKey.EnumKey(i,key);i++)
	{
		if (RegKey2.OpenKey(RegKey,key,CRegKey::defRead)==ERROR_SUCCESS)
		{
			DWORD dwLength=RegKey2.QueryValueLength();
			if (dwLength>1)
			{
				CAllocArrayTmpl<WCHAR> pName(dwLength);
				RegKey2.QueryValue(L"",pName,dwLength);
                
				if (strcasencmp(pName,szName,dwNameLength)==0)
					return CDatabase::FromKey(RegKey,"",key);		
			}
			RegKey2.CloseKey();
		}
	}
	return NULL;
}

BOOL CDatabase::SaveToRegistry(HKEY hKeyRoot,LPCSTR szPath,const PDATABASE* ppDatabases,int nDatabases)
{
	CRegKey RegKey;
	CString Path(szPath);
	Path<<'\\';
	
	if (RegKey.OpenKey(hKeyRoot,Path,CRegKey::samAll|CRegKey::createNew)!=ERROR_SUCCESS)
		return FALSE;
	
	CString name;
	// First, delete all existing keys
	for (;RegKey.EnumKey(0,name);)
		RegKey.DeleteKey(name);

	
	
	// Retrieving digits;
	int nDigits=1;
	for (int i=10;i<=nDatabases;i*=10,nDigits++);
	char* pNum=new char[nDigits+1];
	
	
    for (int i=0;i<nDatabases;i++)
	{
		char* pKeyName=ppDatabases[i]->GetValidKey(i+1);
		DWORD dwLength=istrlen(pKeyName);
		
		// Copying name
		char* pKey=new char[nDigits+dwLength+2];

        sMemCopy(pKey+nDigits+1,pKeyName,dwLength+1);
		pKey[nDigits]='_';
    
        // Formating number
		_itoa_s(i+1,pNum,nDigits+1,10);
		ASSERT(strlen(pNum)<=size_t(nDigits));

		dwLength=istrlen(pNum);
        sMemCopy(pKey+nDigits-dwLength,pNum,dwLength);
		sMemSet(pKey,'0',nDigits-dwLength);
        
		ppDatabases[i]->SaveToRegistry(hKeyRoot,szPath,pKey);
		
		delete[] pKey;
		delete[] pKeyName;
	}

	delete[] pNum;
	
	return TRUE;
}


BOOL CDatabase::SaveToRegistry(HKEY hKeyRoot,LPCSTR szPath,LPCSTR szKey)
{
	CRegKey RegKey;
	CString Path(szPath);
	Path<<'\\'<<szKey;
	
	if (RegKey.OpenKey(hKeyRoot,Path,CRegKey::defWrite)!=ERROR_SUCCESS)
		return FALSE;

	RegKey.SetValue(NULL,m_szName);
	RegKey.SetValue("Flags",DWORD(m_wFlags));
	RegKey.SetValue("Thread",DWORD(m_wThread));
	RegKey.SetValue("ID",DWORD(m_wID));
	
	RegKey.SetValue("ArchiveType",DWORD(m_ArchiveType));
	if (m_szArchiveName!=NULL)
		RegKey.SetValue(L"ArchiveName",m_szArchiveName);
	else
		RegKey.SetValue("ArchiveName",szEmpty);


	if (m_szCreator!=NULL)
		RegKey.SetValue(L"Creator",m_szCreator);
	else
		RegKey.SetValue("Creator",szEmpty);

	if (m_szDescription!=NULL)
		RegKey.SetValue(L"Description",m_szDescription);
	else
		RegKey.SetValue("Description",szEmpty);

	if (m_szRoots!=NULL)
	{
		int i;
		for (i=0;m_szRoots[i]!='\0' || m_szRoots[i+1]!='\0';i++);
		RegKey.SetValue(L"Roots",m_szRoots,i+1,REG_MULTI_SZ);
		
	}
	else
		RegKey.SetValue("Roots",szEmpty);


	// Excluded directories
	DWORD dwLength=1;
	for (int i=0;i<m_aExcludedDirectories.GetSize();i++)
		dwLength+=(DWORD)(istrlenw(m_aExcludedDirectories[i])+1);

	WCHAR* pString=new WCHAR[dwLength+1];
	LPWSTR pPtr=pString;
	for (int i=0;i<m_aExcludedDirectories.GetSize();i++)
	{
		if (i>0)
			*(pPtr++)=L';';

		int iLength=(int)istrlenw(m_aExcludedDirectories[i]);
		MemCopyW(pPtr,m_aExcludedDirectories[i],iLength);
		pPtr+=iLength;
	}
	*pPtr=L'\0';
		
	if (m_szIncludedFiles!=NULL)
		RegKey.SetValue(L"Included Files",m_szIncludedFiles);
	else
		RegKey.SetValue("Included Files",szEmpty);

	if (m_szIncludedDirectories!=NULL)
		RegKey.SetValue(L"Included Directories",m_szIncludedDirectories);
	else
		RegKey.SetValue("Included Directories",szEmpty);

	if (m_szExcludedFiles!=NULL)
		RegKey.SetValue(L"Excluded Files",m_szExcludedFiles);
	else
		RegKey.SetValue("Excluded Files",szEmpty);

	RegKey.SetValue(L"Excluded Directories",pString);
    delete[] pString;

	if (m_szRootMaps!=NULL)
		RegKey.SetValue(L"RootMaps",m_szRootMaps);
	else
		RegKey.SetValue("RootMaps",szEmpty);

	return TRUE;
}

CDatabase* CDatabase::FromKey(HKEY hKeyRoot,LPCSTR szPath,LPCSTR szKey)
{
	CRegKey RegKey;
	CString Path(szPath);
	if (!Path.IsEmpty() && Path.LastChar()!='\\')
		Path<<'\\';
	Path << szKey;
	
	if (RegKey.OpenKey(hKeyRoot,Path,CRegKey::defRead)!=ERROR_SUCCESS)
		return NULL;

	DWORD dwLength=RegKey.QueryValueLength();
	if (int(dwLength)<=1)
		return NULL;

	CDatabase* pDatabase=new CDatabase;
	
	// Copying name
	pDatabase->m_szName=new WCHAR[dwLength];
	RegKey.QueryValue(L"",pDatabase->m_szName,dwLength);
		
	// Retrieving flags and archive type
	DWORD dwTemp=0;
	RegKey.QueryValue("ID",dwTemp);
	pDatabase->m_wID=LOWORD(dwTemp);
	RegKey.QueryValue("Flags",dwTemp);
	pDatabase->m_wFlags=LOWORD(dwTemp);
	RegKey.QueryValue("Thread",dwTemp);
	pDatabase->m_wThread=LOWORD(dwTemp);
	
	RegKey.QueryValue("ArchiveType",*((DWORD*)&pDatabase->m_ArchiveType));
	
	
	// Copying archive name info
	dwLength=RegKey.QueryValueLength("ArchiveName");
	if (dwLength>1)
	{
		pDatabase->m_szArchiveName=new WCHAR[dwLength];
		RegKey.QueryValue(L"ArchiveName",pDatabase->m_szArchiveName,dwLength);
	}
	else
		pDatabase->m_szArchiveName=allocemptyW();


    // Copying creator info
	dwLength=RegKey.QueryValueLength("Creator");
	if (dwLength>1)
	{
		pDatabase->m_szCreator=new WCHAR[dwLength];
		RegKey.QueryValue(L"Creator",pDatabase->m_szCreator,dwLength);
	}

	// Copying description
	dwLength=RegKey.QueryValueLength("Description");
	if (dwLength>1)
	{
		pDatabase->m_szDescription=new WCHAR[dwLength];
		RegKey.QueryValue(L"Description",pDatabase->m_szDescription,dwLength);
	}

	// Copying roots
	dwLength=RegKey.QueryValueLength("Roots");
	if (dwLength>1)
	{
		pDatabase->m_szRoots=new WCHAR[dwLength+1];
		RegKey.QueryValue(L"Roots",pDatabase->m_szRoots,dwLength+1);
		pDatabase->m_szRoots[dwLength]='\0';
	}

	// Included files
	dwLength=RegKey.QueryValueLength("Included Files");
	if (dwLength>1)
	{
		pDatabase->m_szIncludedFiles=new WCHAR[dwLength];
		RegKey.QueryValue(L"Included Files",pDatabase->m_szIncludedFiles,dwLength);
	}

	// Included directories
	dwLength=RegKey.QueryValueLength("Included Directories");
	if (dwLength>1)
	{
		pDatabase->m_szIncludedDirectories=new WCHAR[dwLength];
		RegKey.QueryValue(L"Included Directories",pDatabase->m_szIncludedDirectories,dwLength);
	}

	// Excluded files
	dwLength=RegKey.QueryValueLength("Excluded Files");
	if (dwLength>1)
	{
		pDatabase->m_szExcludedFiles=new WCHAR[dwLength];
		RegKey.QueryValue(L"Excluded Files",pDatabase->m_szExcludedFiles,dwLength);
	}

	// Excluded directories
	dwLength=RegKey.QueryValueLength("Excluded Directories");
	if (dwLength>1)
	{
		LPWSTR pString=new WCHAR[dwLength];
		RegKey.QueryValue(L"Excluded Directories",pString,dwLength);

		LPCWSTR pPtr=pString;
		while (*pPtr!=L'\0')
		{
			int nIndex=(int)FirstCharIndex(pPtr,L';');
			if (nIndex!=-1)
			{
				pDatabase->m_aExcludedDirectories.Add(alloccopy(pPtr,nIndex));
                pPtr+=nIndex+1;
			}
			else
			{
				pDatabase->m_aExcludedDirectories.Add(alloccopy(pPtr));
				break;
			}
		}

		delete[] pString;
	}

	// Copying description
	dwLength=RegKey.QueryValueLength("RootMaps");
	if (dwLength>1)
	{
		pDatabase->m_szRootMaps=new WCHAR[dwLength];
		RegKey.QueryValue(L"RootMaps",pDatabase->m_szRootMaps,dwLength);
	}


	return pDatabase;
}

CDatabase* CDatabase::FromOldStyleDatabase(HKEY hKeyRoot,LPCSTR szPath)
{
	CRegKey RegKey;
	
	// Try to open key
	if (RegKey.OpenKey(hKeyRoot,szPath,CRegKey::defRead)!=ERROR_SUCCESS)
		return NULL;
	
	// Try to retrieve database file
	DWORD dwLength=RegKey.QueryValueLength("DatabaseFile");
	if (dwLength==0)
		return NULL;

    // Old style database info found, loading it
	CDatabase* pDatabase=new CDatabase;
	
	pDatabase->m_ArchiveType=CDatabase::archiveFile;
	pDatabase->m_szArchiveName=new WCHAR[dwLength];
	RegKey.QueryValue(L"DatabaseFile",pDatabase->m_szArchiveName,dwLength);

	
	// Setting name to "default"
	pDatabase->m_szName=new WCHAR[8];
	MemCopyW(pDatabase->m_szName,L"default",8);
	
	// Copying creator info
	dwLength=RegKey.QueryValueLength("Default Creator");
	if (dwLength>1)
	{
		pDatabase->m_szCreator=new WCHAR[dwLength];
		RegKey.QueryValue(L"Default Creator",pDatabase->m_szCreator,dwLength);
	}
	
	// Copying description
	dwLength=RegKey.QueryValueLength("Default Description");
	if (dwLength>1)
	{
		pDatabase->m_szDescription=new WCHAR[dwLength];
		RegKey.QueryValue(L"Default Description",pDatabase->m_szDescription,dwLength);
	}

	// Copying roots
	dwLength=RegKey.QueryValueLength(L"Drives");
	if (dwLength>1)
	{
		pDatabase->m_szRoots=new WCHAR[dwLength];
		RegKey.QueryValue(L"Drives",pDatabase->m_szRoots,dwLength);
	}
	
	return pDatabase;
}

CDatabase* CDatabase::FromFile(LPCWSTR szFileName,int dwNameLength)
{
	LPWSTR szFile=GetCorrertFileName(szFileName,dwNameLength);
	if (szFile==NULL)
		return NULL;

    CDatabase* pDatabase=new CDatabase;

	
	
	pDatabase->m_szName=new WCHAR[6];
	MemCopyW(pDatabase->m_szName,L"param",6);

	pDatabase->m_ArchiveType=archiveFile;
	pDatabase->m_szArchiveName=szFile;

	return pDatabase;
}

CDatabase* CDatabase::FromDefaults(BOOL bDefaultFileName)
{
	CDatabase* pDatabase=new CDatabase; // This default dwFlags and description and drives to NULL

	pDatabase->m_szName=new WCHAR[8];
	MemCopyW(pDatabase->m_szName,L"default",8);

	pDatabase->m_ArchiveType=CDatabase::archiveFile;
	
	if (bDefaultFileName)
		pDatabase->m_szArchiveName=GetDefaultFileLocation(L"files.dbs");
	else
		pDatabase->m_szArchiveName=allocemptyW();
	return pDatabase;
}

/* Example
 $$LDBSET$T:0000$F:E1G1S0I0$N:local$AF:C:\Utils\db\files.dbs$C:jmj@iik$D:All local files$R:1$$
*/

CDatabase* CDatabase::FromExtraBlock(LPCWSTR szExtraBlock)
{
	
	for (int i=0;szExtraBlock[i]!='\0';i++)
	{
		// Finding "$$LDBSET$"
		if (wcsncmp(szExtraBlock+i,L"$$LDBSET$",9)!=0)
			continue;
		
        // Found
		LPCWSTR pPtr=szExtraBlock+i+9;

		CDatabase* pDatabase=new CDatabase;
		CArrayFAP<LPWSTR> aRoots;
		

		for (;;)
		{
			LPCWSTR pKey=pPtr;		
			int length;
			CStringW sValue;

			// Determine key length, i.e., find ':'
			for (length=0;pPtr[length]!=L':' && pPtr[length]!=L'$' && pPtr[length]!=L'\0';length++);
			if (pPtr[length]!=L':')
			{
				// Not correct key
				break;
			}
			pPtr+=length+1;
		    
			
			while (*pPtr!='$') 
			{
				if (*pPtr=='*')
					++pPtr;
				sValue << *(pPtr++);
			}

			switch (*pKey)
			{
			case L'T': // Thread
			case L't': 
				{
					// Checking zeroes
					int i;
					for (i=0;sValue[i]=='0';i++);
					LPWSTR pTemp;
					pDatabase->m_wThread=(WORD)wcstoul(LPCWSTR(sValue)+i,&pTemp,16);
                    break;
				}
			case L'F': // Flags
			case L'f':
				{
					LPCWSTR pTemp=sValue;
					while (*pTemp!=L'\0')
					{
						switch (*(pTemp++))
						{
						case L'E':
						case L'e':
							if (*pTemp==L'1')
							{
								pDatabase->m_wFlags|=flagEnabled;
								pTemp++;
							}
							else if (*pTemp=='0')
							{
								pDatabase->m_wFlags&=~flagEnabled;
								pTemp++;
							}
							else
								pDatabase->m_wFlags|=flagEnabled;
							break;
						case L'G':
						case L'g':
							if (*pTemp==L'1')
							{
								pDatabase->m_wFlags|=flagGlobalUpdate;
								pTemp++;
							}
							else if (*pTemp==L'0')
							{
								pDatabase->m_wFlags&=~flagGlobalUpdate;
								pTemp++;
							}
							else
								pDatabase->m_wFlags|=flagGlobalUpdate;
							break;
							break;
						case L'J':
						case L'j':
							if (*pTemp==L'1')
							{
								pDatabase->m_wFlags|=flagScanSymLinksAndJunctions;
								pTemp++;
							}
							else if (*pTemp==L'0')
							{
								pDatabase->m_wFlags&=~flagScanSymLinksAndJunctions;
								pTemp++;
							}
							else
								pDatabase->m_wFlags|=flagScanSymLinksAndJunctions;
							break;
						case L'S':
						case L's':
							if (*pTemp==L'1')
							{
								pDatabase->m_wFlags|=flagStopIfRootUnavailable;
								pTemp++;
							}
							else if (*pTemp==L'0')
							{
								pDatabase->m_wFlags&=~flagStopIfRootUnavailable;
								pTemp++;
							}
							else
								pDatabase->m_wFlags|=flagStopIfRootUnavailable;
							break;
						case L'I':
						case L'i':
							if (*pTemp==L'1')
							{
								pDatabase->m_wFlags|=flagIncrementalUpdate;
								pTemp++;
							}
							else if (*pTemp==L'0')
							{
								pDatabase->m_wFlags&=~flagIncrementalUpdate;
								pTemp++;
							}
							else
								pDatabase->m_wFlags|=flagIncrementalUpdate;
							break;
						case L'A':
						case L'a':
							if (*pTemp==L'1')
							{
								pDatabase->m_wFlags|=flagAnsiCharset;
								pTemp++;
							}
							else if (*pTemp==L'0')
							{
								pDatabase->m_wFlags&=~flagAnsiCharset;
								pTemp++;
							}
							else
								pDatabase->m_wFlags|=flagAnsiCharset;
							break;
						}
					}
					break;
				}
			case L'N': // Name
			case L'n':
				pDatabase->m_szName=sValue.GiveBuffer();
				break;
			case L'A': // Archive
			case L'a':
				if (pKey[1]==L'F')
				{
					// File:
					pDatabase->m_ArchiveType=CDatabase::archiveFile;
					pDatabase->SetArchiveNamePtr(sValue.GiveBuffer());
				}
				break;
			case L'C': // Creator
			case L'c':
				pDatabase->m_szCreator=sValue.GiveBuffer();
				break;
			case L'D': // Description
			case L'd':
				pDatabase->m_szDescription=sValue.GiveBuffer();
				break;
			case L'M': // Description
			case L'm':
				pDatabase->m_szRootMaps=sValue.GiveBuffer();
				break;
			case L'R':
			case L'r':
				if (sValue.Compare(L"1")==0)
				{
					DebugMessage("DBIMPORT: using local roots");
					aRoots.RemoveAll();
				}
				else
				{
					DebugFormatMessage(L"DBIMPORT: found root: %s",(LPCWSTR)sValue);
					aRoots.Add(sValue.GiveBuffer());
				}
				break;
			case L'I':
			case L'i':
                if (pKey[1]==L'F')
					pDatabase->m_szIncludedFiles=sValue.GiveBuffer();
				else if (pKey[1]==L'D')
					pDatabase->m_szIncludedDirectories=sValue.GiveBuffer();
				break;
			case L'E':
			case L'e':
                if (pKey[1]==L'F')
					pDatabase->m_szExcludedFiles=sValue.GiveBuffer();
				else
					pDatabase->m_aExcludedDirectories.Add(sValue.GiveBuffer());
				break;
			}

			pPtr++;
				
		}

		pDatabase->SetRoots(aRoots);

		return pDatabase;
	}
	return NULL;
}

LPWSTR CDatabase::ResolveLocateDir(LPCWSTR szFileName,DWORD dwNameLength)
{
	if (dwNameLength==DWORD(-1))
		dwNameLength=(DWORD)istrlenw(szFileName);
	
	int nIndex=FirstCharIndex(szFileName,L'%',(int)dwNameLength);
	if (nIndex==-1)
		return (LPWSTR)szFileName;

	if (_wcsnicmp(szFileName+nIndex+1,L"locatedir%",10)!=0)
		return NULL;

	// Resolve locatedir
	WCHAR szLocateDir[MAX_PATH+2];
	int nLength=FileSystem::GetModuleFileName(NULL,szLocateDir,MAX_PATH+2);
	nLength=LastCharIndex(szLocateDir,L'\\',nLength);
	if (nLength==-1)
		return NULL;

	WCHAR* pNewPath=new WCHAR[dwNameLength-11+nLength+1];
	MemCopyW(pNewPath,szFileName,nIndex);
	MemCopyW(pNewPath+nIndex,szLocateDir,nLength);
	MemCopyW(pNewPath+nIndex+nLength,szFileName+nIndex+11,dwNameLength-nIndex-11+1);

	return pNewPath;
}

LPWSTR CDatabase::GetCorrertFileName(LPCWSTR szFileName,DWORD dwNameLength)
{
	if (dwNameLength==DWORD(-1))
		dwNameLength=(DWORD)istrlenw(szFileName);
	
	if (FirstCharIndex(szFileName,L'%',(int)dwNameLength)!=-1)
	{
		// There is '%' in name, check that is that for %locatedir%
		LPCWSTR szResolvedPath=ResolveLocateDir(szFileName,dwNameLength);

		if (szResolvedPath==NULL)
			return NULL; // Not valid

		if (!FileSystem::IsValidFileName(szResolvedPath))
		{
			delete[] szResolvedPath;
			return NULL;
		}

		LPWSTR pRet=alloccopy(szFileName,dwNameLength);
		
		//Replace '/' with '\\'
		for (DWORD i=0;i<dwNameLength;i++)
		{
			if (pRet[i]=='/')
				pRet[i]='\\';
		}
		return pRet;
	}

	LPWSTR szFile;
	if (szFileName[0]!=L'\\' && szFileName[0]!=L'/' && szFileName[1]!=L':') // Not UNC or on drive
		szFile=GetDefaultFileLocation(szFileName);
	else
		szFile=alloccopy(szFileName,dwNameLength);

	//Replace '/' with '\\'
	for (DWORD i=0;szFile[i]!='\0';i++)
	{
		if (szFile[i]=='/')
			szFile[i]='\\';
	}

	// Checking whether file name is valid
	if (!FileSystem::IsValidFileName(szFile))
	{
		delete[] szFile;
		return NULL;
	}
	return szFile;
}

void CDatabase::CheckDoubleNames(PDATABASE* ppDatabases,int nDatabases)
{
	for (int i=1;i<nDatabases;i++)
	{
		DWORD dwLength=istrlenw(ppDatabases[i]->m_szName);
		
		for (int j=0;j<i;j++)
		{
			if (strcasencmp(ppDatabases[i]->m_szName,ppDatabases[j]->m_szName,DWORD(dwLength+1))==0)
			{
				if (ppDatabases[i]->m_szName[dwLength-1]>='0' && ppDatabases[i]->m_szName[dwLength-1]<'9')
					ppDatabases[i]->m_szName[dwLength-1]++;
				else if (ppDatabases[i]->m_szName[dwLength-1]=='9')
				{
					if (ppDatabases[i]->m_szName[dwLength-2]>='1' && ppDatabases[i]->m_szName[dwLength-2]<'9')
						ppDatabases[i]->m_szName[dwLength-2]++;
					else
					{
						WCHAR* tmp=new WCHAR[dwLength+2];
						MemCopyW(tmp,ppDatabases[i]->m_szName,dwLength-1);
						delete[] ppDatabases[i]->m_szName;
						tmp[dwLength-1]=L'1';
						tmp[dwLength]=L'0';
						tmp[dwLength+1]=L'\0';
						ppDatabases[i]->m_szName=tmp;
					}
				}
				else
				{
					WCHAR* tmp=new WCHAR[dwLength+2];
					MemCopyW(tmp,ppDatabases[i]->m_szName,dwLength);
					tmp[dwLength++]=L'1';
                    tmp[dwLength]=L'\0';
					delete[] ppDatabases[i]->m_szName;
                    ppDatabases[i]->m_szName=tmp;
				}
				
				dwLength=istrlenw(ppDatabases[i]->m_szName);
				j=-1;
			}			
		}
	}
}



void CDatabase::CheckValidNames(PDATABASE* ppDatabases,int nDatabases)
{
	for (int i=0;i<nDatabases;i++)
	{
		if (!IsNameValid(ppDatabases[i]->m_szName))
		{
			MakeNameValid(ppDatabases[i]->m_szName);
			
			// Unallocaling unnecessary memory
			WCHAR* pNew=alloccopy(ppDatabases[i]->m_szName);
			delete[] ppDatabases[i]->m_szName;
			ppDatabases[i]->m_szName=pNew;
		}
	}
}


#define ISVALIDFORNAME(a) \
	( (a)!=L'\\' && (a)!=L'\"'  )

#define ISVALIDFORKEY(a) \
	( ((a)>=L'0' && (a)<=L'9') || \
	  ((a)>=L'a' && (a)<=L'z') || \
	  ((a)>=L'A' && (a)<=L'Z') || \
	  (a)==L' ' || \
	  (a)==L'#' || \
	  (a)==L'@' || \
	  (a)==L'%' || \
	  (a)==L'[' || \
	  (a)==L']' || \
	  (a)==L'~' || \
	  (a)==L'+' || \
	  (a)==L'-' || \
	  (a)==L'_' || \
	  (a)==L'(' || \
	  (a)==L')' || \
	  (a)==L'!' || \
	  (a)==L'\"' || \
	  (a)==L'\'' )
	
BOOL CDatabase::IsNameValid(LPCWSTR szName)
{
	int i;
	for (i=0;szName[i]!=L'\0';i++)
	{
		if (!ISVALIDFORNAME(szName[i]))
			return FALSE;
	}
	return i>0;
}

void CDatabase::MakeNameValid(LPWSTR szName)
{
	int i,j=0;
	for (i=0;szName[i]!=L'\0';i++)
	{
		if (ISVALIDFORNAME(szName[i]))
			szName[j++]=szName[i];
	}
	szName[j]=L'\0';
}

LPSTR CDatabase::GetValidKey(DWORD dwUniqueNum) const
{
	DWORD dwValid=0;
	for (int i=0;m_szName[i]!='\0';i++)
	{
		if (ISVALIDFORKEY(m_szName[i]))
			dwValid++;
	}

	if (dwValid==0)
	{
		if (dwUniqueNum>0)
		{
			char* key=new char[25];
			StringCbPrintf(key,25,"db%d",dwUniqueNum);
			return key;            
		}
		else
			return alloccopy("db");
	}

	CHAR* key=new char[dwValid+1];
	int i,j;
	for (i=0,j=0;m_szName[i]!='\0';i++)
	{
		if (ISVALIDFORKEY(m_szName[i]))
		{
			MemCopyWtoA(key+j,dwValid+1-j,m_szName+i,1);
			j++;
		}
	}
	key[j]='\0';
	return key;
}

WORD CDatabase::CheckIDs(PDATABASE* ppDatabases,int nDatabases)
{
	int nCurrentlyMustBe=0,index;
	
	for (index=0;index<nDatabases;index++)
	{
		if (ppDatabases[index]->m_wID==0)
			ppDatabases[index]->m_wID=GetUniqueIndex(ppDatabases,nDatabases);
	}

	index=0;
	while (index<nDatabases)
	{
		int nCurrentThreadID=ppDatabases[index]->m_wThread;
		if (nCurrentThreadID!=nCurrentlyMustBe)
		{
			// Make databases with current ID to nCurrentlyMustBe
			ppDatabases[index++]->m_wThread=nCurrentlyMustBe;

            while (index<nDatabases && ppDatabases[index]->m_wThread==nCurrentThreadID)
				ppDatabases[index++]->m_wThread=nCurrentlyMustBe;
		}
		else
		{
			for (;index<nDatabases && ppDatabases[index]->m_wThread==nCurrentThreadID;index++);
		}
		nCurrentlyMustBe++;
	}
	return nCurrentlyMustBe;
}

void CDatabase::GetRoots(CArray<LPWSTR>& aRoots) const
{
	if (m_szRoots!=NULL)
	{
		LPCWSTR pPtr=m_szRoots;
		while (*pPtr!=L'\0')
		{
			DWORD dwLength=istrlenw(pPtr);
			aRoots.Add(alloccopy(pPtr,dwLength));
			pPtr+=dwLength+1;
		}
	}
}

void CDatabase::SetRoots(LPWSTR* pRoots,int nCount)
{
	if (m_szRoots!=NULL)
		delete[] m_szRoots;

	if (nCount==0)
	{
		m_szRoots=NULL;
		return;
	}

	// Counting required buffer size
	DWORD dwBufferSize=0;
	DWORD* dwLengths=new DWORD[nCount];

	int i;
	for (i=0;i<nCount;i++)
	{
		dwLengths[i]=(DWORD)istrlenw(pRoots[i]);
		dwBufferSize+=++dwLengths[i];
	}
	
	m_szRoots=new WCHAR[dwBufferSize+1];
	LPWSTR pPtr=m_szRoots;
	for (i=0;i<nCount;i++)
	{
		MemCopyW(pPtr,pRoots[i],dwLengths[i]);
		pPtr+=dwLengths[i];
	}
	*pPtr=L'\0';

	delete[] dwLengths;
}

CDatabase* CDatabase::FindByName(PDATABASE* ppDatabases,int nDatabases,LPCWSTR szName,int iLength)
{
	CStringW sName(szName,iLength);
	sName.MakeLower();

	for (int i=0;i<nDatabases;i++)
	{
		CStringW str(ppDatabases[i]->GetName());
		str.MakeLower();        

		if (sName.Compare(str)==0)
			return ppDatabases[i];
	}
	return NULL;
}

CDatabase* CDatabase::FindByFile(PDATABASE* ppDatabases,int nDatabases,LPCWSTR szFile,int iLength)
{
	WCHAR* pPath1=NULL;
	WCHAR szPath1[MAX_PATH]; 

	if (iLength==0)
		return NULL;

	DWORD dwRet;

	if (szFile[0]!=L'\\' && szFile[1]!=L':')
	{
		if (iLength==-1)
			iLength=(int)istrlenw(szFile);
			
		DWORD dwLength=GetCurrentDirectory(0,NULL);
		if (dwLength==0)
			return NULL;
       
		pPath1=new WCHAR[dwLength+iLength+2];
		FileSystem::GetCurrentDirectory(dwLength,pPath1);
		dwLength--;
		if (pPath1[dwLength-1]!=L'\\')
			pPath1[dwLength++]=L'\\';

		MemCopyW(pPath1+dwLength,szFile,DWORD(iLength));
		pPath1[dwLength+iLength]=L'\0';

		dwRet=FileSystem::GetShortPathName(pPath1,szPath1,MAX_PATH);
	}
	else if (iLength!=-1)
	{
		pPath1=new WCHAR[iLength+1];
		MemCopyW(pPath1,szFile,DWORD(iLength));
		pPath1[iLength]=L'\0';
		dwRet=FileSystem::GetShortPathName(pPath1,szPath1,MAX_PATH);
	}
	else
		dwRet=FileSystem::GetShortPathName(szFile,szPath1,MAX_PATH);
	
	// File does not exists
	if (dwRet==0)
	{
		if (pPath1==NULL)
			pPath1=const_cast<LPWSTR>(szFile);
		
		for (int i=0;i<nDatabases;i++)
		{
			ASSERT_VALID(ppDatabases[i]->m_szArchiveName)
			if (!FileSystem::IsFile(ppDatabases[i]->m_szArchiveName))
			{
				if (_wcsicmp(pPath1,ppDatabases[i]->m_szArchiveName)==0)
				{
					if (pPath1!=szFile)
						delete[] pPath1;
					return ppDatabases[i];
				}
			}
		}
		if (pPath1!=szFile)
			delete[] pPath1;
		return NULL;
	}

	if (pPath1!=NULL)
		delete[] pPath1;
	for (int i=0;i<nDatabases;i++)
	{
		ASSERT_VALID(ppDatabases[i]->m_szArchiveName);
		if (strcasecmp(szPath1,ppDatabases[i]->m_szArchiveName)==0)
			return ppDatabases[i];
	}
	return NULL;
}

WORD CDatabase::GetUniqueIndex(PDATABASE* ppDatabases,int nDatabases)
{
	WORD wNewIndex,i;

	do
	{
		wNewIndex=LOWORD(GetTickCount());

		if (wNewIndex==0)
			continue;

        for (i=0;i<WORD(nDatabases);i++)
		{
			if (ppDatabases[i]->m_wID==wNewIndex)
				break;
		}
	}
	while (i<WORD(nDatabases));
	return wNewIndex;
}

BOOL CDatabase::IsFileNamesOEM() const
{
#ifdef WIN32
	BYTE* szBuffer=new BYTE[11];
	BOOL bOEM=-1;
	CFile* dbFile=NULL;

	try 
	{
		switch (m_ArchiveType)
		{
		case archiveFile:
			{
				BOOL bFree;
				LPWSTR szFileName=GetResolvedArchiveName(bFree);
				dbFile=new CFile(szFileName,CFile::defRead|CFile::otherErrorWhenEOF,TRUE);
				dbFile->CloseOnDelete();

				if (bFree)
					delete[] szFileName;

				break;
			}
		default:
			return -1;
		}
		
		// Reading and verifing header
		dbFile->Read(szBuffer,11);
		if (szBuffer[0]=='L' && szBuffer[1]=='O' &&
			szBuffer[2]=='C' && szBuffer[3]=='A' && 
			szBuffer[4]=='T' && szBuffer[5]=='E' && 
			szBuffer[6]=='D' && szBuffer[7]=='B' &&
			szBuffer[8]=='2' && szBuffer[9]=='0' )
		{		
			bOEM=!(szBuffer[10]&0x10);
		}
		dbFile->Close();
	}
	catch (...)
	{
	}
	
	delete[] szBuffer;
	
	if (dbFile!=NULL)
		delete dbFile;
	return bOEM;
#else
	return TRUE;
#endif
}

void CDatabase::AddLocalRoots()
{
	CArrayFAP<LPWSTR> aLocalRoots;
	GetLogicalDrives(&aLocalRoots);

	if (m_szRoots==NULL)
	{
		CIntArray aLengths;
		DWORD dwDataLength=0;
		int i;
		for (i=0;i<aLocalRoots.GetSize();i++)
		{
			DWORD iLength=(DWORD)istrlenw(aLocalRoots[i]);
			aLengths.Add(++iLength);
			dwDataLength+=iLength;
		}
		m_szRoots=new WCHAR[++dwDataLength];
		LPWSTR ptr=m_szRoots;
		for (i=0;i<aLocalRoots.GetSize();i++)
		{
			MemCopyW(ptr,aLocalRoots[i],aLengths[i]);
			ptr+=aLengths[i];
		}
		*ptr=L'\0';
		return;
	}

	for (int i=0;i<aLocalRoots.GetSize();i++)
		AddRoot(aLocalRoots[i]);
}

void CDatabase::AddRoot(LPCWSTR pRoot)
{
	DWORD dwLength=(DWORD)istrlenw(pRoot);
	
	if (m_szRoots==NULL)
	{
		m_szRoots=new WCHAR[dwLength+2];
		MemCopyW(m_szRoots,pRoot,dwLength);
		m_szRoots[dwLength++]=L'\0';
		m_szRoots[dwLength++]=L'\0';
		return;
	}
	
	// Counting previous data length;
	DWORD dwDataLength;
	for (dwDataLength=0;m_szRoots[dwDataLength]!=L'\0' || m_szRoots[dwDataLength+1]!=L'\0';dwDataLength++)
	{
		if (strcasecmp(m_szRoots+dwDataLength,pRoot)==0)
			return; // Already exist
	}
	dwDataLength++;

	LPWSTR pNew=new WCHAR[dwDataLength+dwLength+2];
    MemCopyW(pNew,m_szRoots,dwDataLength);
	MemCopyW(pNew+dwDataLength,pRoot,dwLength);
	dwDataLength+=dwLength;
	pNew[dwDataLength++]='\0';
	pNew[dwDataLength++]='\0';

	delete[] m_szRoots;
	m_szRoots=pNew;
}

#ifdef WIN32

void CDatabase::GetLogicalDrives(CArrayFAP<LPWSTR>* paRoots)
{
	paRoots->RemoveAll();
	DWORD dwBufferLen=GetLogicalDriveStrings(0,NULL)+1;
	
	WCHAR* szDrives=new WCHAR[dwBufferLen];
	FileSystem::GetLogicalDriveStrings(dwBufferLen,szDrives);

	for (int i=0;szDrives[i*4]!=L'\0';i++)
	{
		if (FileSystem::GetDriveType(szDrives+i*4)==DRIVE_FIXED)
		{
			WCHAR* tmp=new WCHAR[3];
			tmp[0]=szDrives[i*4];
			tmp[1]=':';
			tmp[2]='\0';
			paRoots->Add(tmp);
		}
	}
	delete[] szDrives;
}

#else

void CDatabase::GetLogicalDrives(CArrayFAP<LPSTR>* paRoots)
{            
	char szDrives[27];
	getlocaldrives(szDrives);
	for (int i=0;i<27;i++)
	{
		if (szDrives[i])
		{
			char* pRoot=new char[3];
			pRoot[0]=i+'A';
			pRoot[1]=':';
			pRoot[2]='\0';
			paRoots->Add(pRoot);
		}
	}
}


/*
void CDatabase::ReadIniFile(BYTE* pData,CArrayFAP<LPSTR>* paRoots,CString* pCreator,CString* pDescription,CString* pDatabaseFile)
{
    int i;
    while (1)
    {
        LPCSTR szData=LPCSTR(pData);
    	while (szData[0]==' ' || szData[0]=='\n' || szData[0]=='\r') szData++;
        for (i=0;szData[i]!='=' && szData[i]!='\0';i++);
        if (szData[i]=='\0')
            return;    
        pData[i]='\0';
        if (strcasecmp(szData,"DatabaseFile")==0)
        {
            szData+=i+1;
            if (pDatabaseFile==NULL)
            {
				while (szData[0]!='\n' && szData[0]!='\0') szData++;
			}
            else
            {
				pDatabaseFile->Empty();
				for (i=0;szData[i]!='\0' && szData[i]!='\n'&& szData[i]!='\r';i++)
                    *pDatabaseFile<<szData[i];
            }
            if (szData[0]=='\0')
                return;
            szData++;
        }
        else if (strcasecmp(szData,"Creator")==0)
        {
            szData+=i+1;
            if (pCreator==NULL)
            {
				while (szData[0]!='\n' && szData[0]!='\0') szData++;
			}
            else
            {
				pCreator->Empty();
				for (i=0;szData[i]!='\0' && szData[i]!='\n'&& szData[i]!='\r';i++)
                    *pCreator<<szData[i];
            }
            if (szData[0]=='\0')
                return;
            szData++;
        }
        else if (strcasecmp(szData,"Description")==0)
        {
            szData+=i+1;
            if (pDescription==NULL)
            {
				while (szData[0]!='\n' && szData[0]!='\0') szData++;
			}
            else
            {
				pDescription->Empty();
				for (i=0;szData[i]!='\0' && szData[i]!='\n'&& szData[i]!='\r';i++)
                    *pDescription<<szData[i];
            }
            if (szData[0]=='\0')
                return;
            szData++;
        }
        else if (strcasecmp(szData,"Drives")==0)
        {
            szData+=i+1;
            if (paRoots==NULL)
            {
                while (szData[0]!='\n' && szData[0]!='\0') szData++;
            }
            else
            {
                paRoots->RemoveAll();

				while (szData[i]!='\n' && szData[i]!='\r' && szData[i]!='\0')
				{
					DWORD dwLength;
					for (dwLength=0;szData[dwLength]!='\r' && szData[dwLength]!='\n' && szData[dwLength]!='\0' && szData[dwLength]!=';';dwLength++);
					LPSTR pRoot=new char[dwLength+1];
					sMemCopy(pRoot,szData,dwLength);
					pRoot[dwLength]='\0';
					paRoots->Add(pRoot);
					szData+=dwLength;
					if (szData[i]==';')
						szData++;
				}
            }
            if (szData[0]=='\0')
                return;
            szData++;
        }
        while (szData[0]=='\n' || szData[0]=='\r')
        {
            if (szData[0]=='\0')
                return;
            szData++;   
        }     
    }
}
*/

#endif

LPWSTR CDatabase::ConstructExtraBlock(DWORD* pdwLen) const
{
	CStringW str;
	str.Format(L"$$LDBSET$T:%04X$",m_wThread);
	
	// Flags
	if (m_wFlags!=0)
	{
		str << L"F:";
		
		if (m_wFlags&flagEnabled)
			str << L"E1";
		else
			str << L"E0";
		if (m_wFlags&flagGlobalUpdate)
			str << L"G1";
		else
			str << L"G0";
		if (m_wFlags&flagScanSymLinksAndJunctions)
			str << L"J1";
		else
			str << L"J0";
		if (m_wFlags&flagStopIfRootUnavailable)
			str << L"S1";
		else
			str << L"S0";
		if (m_wFlags&flagIncrementalUpdate)
			str << L"I1";
		else
			str << L"I0";
		if (m_wFlags&flagAnsiCharset)
			str << L"A1";
		else
			str << L"A0";
		
		str << L'$';
	}

	if (m_szName!=NULL)
	{
		// Name
		str << L"N:";
		for (int i=0;m_szName[i]!=L'\0';i++)
		{
			if (m_szName[i]==L'$' || m_szName[i]==L'*')
				str << L'*';
			str << m_szName[i];
		}
		str << L'$';
	}

	if (m_ArchiveType==archiveFile && m_szArchiveName!=NULL && m_szArchiveName[0]!='\0')
	{
		// Archive file
		str << L"AF:";
		for (int i=0;m_szArchiveName[i]!=L'\0';i++)
		{
			if (m_szArchiveName[i]==L'$' || m_szArchiveName[i]==L'*')
				str << L'*';
			str << m_szArchiveName[i];
		}
		str << L'$';
	}

	if (m_szCreator!=NULL)
	{
		// Creator
		str << L"C:";
		for (int i=0;m_szCreator[i]!=L'\0';i++)
		{
			if (m_szCreator[i]==L'$' || m_szCreator[i]==L'*')
				str << L'*';
			str << m_szCreator[i];
		}
		str << L'$';
	}
	if (m_szDescription!=NULL)
	{
		// Description
		str << L"D:";
		for (int i=0;m_szDescription[i]!=L'\0';i++)
		{
			if (m_szDescription[i]==L'$' || m_szDescription[i]==L'*')
				str << L'*';
			str << m_szDescription[i];
		}
		str << L'$';
	}

	// Roots
	if (m_szRoots==NULL)
		str << L"R:1$";
	else
	{
		LPWSTR pStr=m_szRoots;
		while (*pStr!=L'\0')
		{
			str << L"R:";
			while (*pStr!=L'\0')
			{
				if (*pStr==L'$' || *pStr==L'*')
					str << L'*';
				str << *pStr;
				pStr++;
			}
			str << L'$';
			pStr++;
		}
	}

	// Included files
	if (m_szIncludedFiles!=NULL)
	{
		// Include files
		str << L"IF:";
		for (int i=0;m_szIncludedFiles[i]!=L'\0';i++)
		{
			if (m_szIncludedFiles[i]==L'$' || m_szIncludedFiles[i]==L'*')
				str << L'*';
			str << m_szIncludedFiles[i];
		}
		str << L'$';
	}

	// Included files
	if (m_szIncludedDirectories!=NULL)
	{
		// Include directories
		str << L"ID:";
		for (int i=0;m_szIncludedDirectories[i]!=L'\0';i++)
		{
			if (m_szIncludedDirectories[i]==L'$' || m_szIncludedDirectories[i]==L'*')
				str << L'*';
			str << m_szIncludedDirectories[i];
		}
		str << L'$';
	}

	// Excluded files
	if (m_szExcludedFiles!=NULL)
	{
		// Exclude files
		str << L"EF:";
		for (int i=0;m_szExcludedFiles[i]!=L'\0';i++)
		{
			if (m_szExcludedFiles[i]==L'$' || m_szExcludedFiles[i]==L'*')
				str << L'*';
			str << m_szExcludedFiles[i];
		}
		str << L'$';
	}



	// Excluded directories
	for (int i=0;i<m_aExcludedDirectories.GetSize();i++)
	{
		str << L"E:";
		for (int j=0;m_aExcludedDirectories[i][j]!=L'\0';j++)
		{
			if (m_aExcludedDirectories[i][j]==L'$' || m_aExcludedDirectories[i][j]==L'*')
				str << L'*';
			str << m_aExcludedDirectories[i][j];
		}
		str << L'$';
	
	}

	// Roots maps
	if (m_szRootMaps!=NULL)
	{
		str << L"M:";
		for (int i=0;m_szRootMaps[i]!=L'\0';i++)
		{
			if (m_szRootMaps[i]==L'$' || m_szRootMaps[i]==L'*')
				str << L'*';
			str << m_szRootMaps[i];
		}
		str << L'$';
	}

	str << L'$';
	str.FreeExtra();

	if (pdwLen!=NULL)
		*pdwLen=(DWORD)str.GetLength();
	return str.GiveBuffer();
}



BOOL CDatabase::SaveExtraBlockToDbFile(LPCWSTR szArchive)
{
	
	CFile *pInFile=NULL,*pOutFile=NULL;
	char* szBuffer=NULL;

	LPWSTR szExtra=ConstructExtraBlock();
	DWORD dwExtraLen=(DWORD)(istrlenw(szExtra)+1);

	// Constructing temp file name	
	WCHAR szTempFile[MAX_PATH];
	{
		CStringW CurPath(szArchive,LastCharIndex(szArchive,L'\\'));
		
		if (!FileSystem::GetTempFileName(CurPath,L"_dbtmp",0,szTempFile))
			return FALSE;
	}

	BOOL bRet=TRUE;
	DWORD dwTemp;
	

	try {
		// Opening database and trying to read header
		pInFile=new CFile(szArchive,CFile::defRead|CFile::otherStrNullTerminated|CFile::otherErrorWhenEOF,TRUE);
		pInFile->CloseOnDelete();

		ULONGLONG ulFileSize=pInFile->GetLength64();

		szBuffer=new char[12];
		pInFile->Read(szBuffer,11);

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

		// Is unicode character set in use
		BOOL bUnicode=szBuffer[10]&0x20;

		// Reading header size
		pInFile->Read(dwTemp);

		// Open output file		
		pOutFile=new CFile(szTempFile,CFile::defWrite|CFile::otherStrNullTerminated,TRUE);
		pOutFile->CloseOnDelete();
		
		// Writing "LOCATE20" and character set idenfication
		pOutFile->Write(szBuffer,11);
		

		// Copy creator, description and extra fields
		if (bUnicode)
		{
			CStringW Creator,Description,Extra1,Extra2;
			
			pInFile->Read(Creator);
			
			pInFile->Read(Description);
			pInFile->Read(Extra1);
			pInFile->Read(Extra2);

			 // Writing header size
			pOutFile->Write(DWORD(
				(Creator.GetLength()+1)*2+ // Author data
				(Description.GetLength()+1)*2+ // Comments data
				(Extra1.GetLength()+1)*2+
				dwExtraLen*2+ // Extra
				4+ // Time
				4+ // Number of files
				4  // Number of directories
				));

			pOutFile->Write(Creator);
			pOutFile->Write(Description);
			pOutFile->Write(Extra1);
			pOutFile->Write(szExtra,dwExtraLen);
		}
		else
		{
			CString Creator,Description,Extra1,Extra2;
			
			pInFile->Read(Creator);
			pInFile->Read(Description);
			pInFile->Read(Extra1);
			pInFile->Read(Extra2);

			// Writing header size
			pOutFile->Write(DWORD(
				Creator.GetLength()+1+ // Author data
				Description.GetLength()+1+ // Comments data
				dwExtraLen+ // Extra
				4+ // Time
				4+ // Number of files
				4  // Number of directories
				));

			pOutFile->Write(Creator);
			pOutFile->Write(Description);
			pOutFile->Write(Extra1);
			pOutFile->Write(W2A(szExtra),dwExtraLen);
		}

		
   
		
		// TIME, FILE and DIRECTORY counts
		pInFile->Read(szBuffer,3*sizeof(DWORD));
		pOutFile->Write(szBuffer,3*sizeof(DWORD));

        // Roots
		pInFile->Read(dwTemp);
		while (dwTemp!=0)
		{
			if (dwTemp>ulFileSize-pInFile->GetPosition64())
			{
				// Invalid database
				throw CFileException(CFileException::invalidFile);
			}

			pOutFile->Write(dwTemp);

			delete[] szBuffer;
			szBuffer=new char[dwTemp];
			pInFile->Read(szBuffer,dwTemp);
			pOutFile->Write(szBuffer,dwTemp);

			pInFile->Read(dwTemp);
		}

		pOutFile->Write((DWORD)0);

		pInFile->Close();
		pOutFile->Close();

		FileSystem::Remove(szArchive);
		FileSystem::MoveFile(szTempFile,szArchive);
	}
	catch (...)
	{
		bRet=FALSE;
	}
    
	if (pInFile!=NULL)
		delete pInFile;
	if (pOutFile!=NULL)
		delete pOutFile;
	if (szBuffer!=NULL)
		delete[] szBuffer;
	delete[] szExtra;

	
	// Delete temp file 
	FileSystem::Remove(szTempFile);
	return bRet;
}
