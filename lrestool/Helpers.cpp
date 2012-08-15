#include <hfclib.h>
#include <conio.h>
#include "lrestool.h"


PIDENTIFIER CIdentifiers::AddHead()
{
	CNode* tmp=new CNode;
	if (tmp==NULL)
	{
		if (m_bThrow)
			throw CException(CException::cannotAllocate);
		else
			SetHFCError(HFC_CANNOTALLOC);
		return NULL;
	}
	tmp->pPrev=NULL;
	tmp->pNext=m_pNodeHead;
	if (m_pNodeHead!=NULL)
		m_pNodeHead->pPrev=tmp;
	else
		m_pNodeTail=tmp;
	m_pNodeHead=tmp;
	m_nCount++;
	return &tmp->data;
}

PIDENTIFIER CIdentifiers::AddTail()
{
	CNode* tmp=new CNode;
	if (tmp==NULL)
	{
		if (m_bThrow)
			throw CException(CException::cannotAllocate);
		else
			SetHFCError(HFC_CANNOTALLOC);
		return NULL;
	}
	tmp->pNext=NULL;
	tmp->pPrev=m_pNodeTail;
	if (m_pNodeTail!=NULL)
		m_pNodeTail->pNext=tmp;
	else
		m_pNodeHead=tmp;
	m_pNodeTail=tmp;
	m_nCount++;
	return &tmp->data;
}

PIDENTIFIER CIdentifiers::InsertAfter(LPCSTR szIdentifier)
{
	CNode* tmp=new CNode;
	if (tmp==NULL)
	{
		if (m_bThrow)
			throw CException(CException::cannotAllocate);
		else
			SetHFCError(HFC_CANNOTALLOC);
		return NULL;
	}
	POSITION position=FindIdentifierPos(szIdentifier);

	tmp->pNext=((CNode*)position)->pNext;
	if (tmp->pNext!=NULL)
		tmp->pNext->pPrev=tmp;
	else
		m_pNodeTail=tmp;
	tmp->pPrev=((CNode*)position);
	((CNode*)position)->pNext=tmp;
	m_nCount++;
	return &tmp->data;
}

PIDENTIFIER CIdentifiers::InsertBefore(LPCSTR szIdentifier)
{
	CNode* tmp=new CNode;
	if (tmp==NULL)
	{
		if (m_bThrow)
			throw CException(CException::cannotAllocate);
		else
			SetHFCError(HFC_CANNOTALLOC);
		return NULL;
	}
	POSITION position=FindIdentifierPos(szIdentifier);

	tmp->pPrev=((CNode*)position)->pPrev;
	if (tmp->pPrev!=NULL)
		tmp->pPrev->pNext=tmp;
	else
		m_pNodeHead=tmp;
	tmp->pNext=((CNode*)position);
	((CNode*)position)->pPrev=tmp;
	m_nCount++;
	return &tmp->data;
}

void CIdentifiers::RemoveAll(BOOL bShowUnused)
{
	CNode* tmp;
	while(m_pNodeHead!=NULL)
	{
		tmp=m_pNodeHead;
		m_pNodeHead=tmp->pNext;
		if (bShowUnused && !tmp->data.bUsed)
			printf("Identifier %s is not used.\n",LPCSTR(tmp->data.name));
		delete tmp;
	}
	m_nCount=0;
	m_pNodeTail=NULL;
}

PIDENTIFIER CIdentifiers::FindIdentifier(LPCSTR szName)
{
	POSITION pPos=GetHeadPosition();
	while (pPos!=NULL)
	{
		PIDENTIFIER pIdentifier=GetAtPtr(pPos);
		if (pIdentifier->name.Compare(szName)==0)
			return pIdentifier;
		pPos=GetNextPosition(pPos);
	}
	return NULL;
}

POSITION CIdentifiers::FindIdentifierPos(LPCSTR szName) const
{
	POSITION pPos=GetHeadPosition();
	while (pPos!=NULL)
	{
		PIDENTIFIER pIdentifier=GetAtPtr(pPos);
		if (pIdentifier->name.Compare(szName)==0)
			return pPos;
		pPos=GetNextPosition(pPos);
	}
	return NULL;
}
	

PCIDENTIFIER CIdentifiers::FindIdentifier(LPCSTR szName) const
{
	POSITION pPos=GetHeadPosition();
	while (pPos!=NULL)
	{
		PIDENTIFIER pIdentifier=GetAtPtr(pPos);
		if (pIdentifier->name.Compare(szName)==0)
			return pIdentifier;
		pPos=GetNextPosition(pPos);
	}
	return NULL;
}


BOOL CIdentifiers::FindNextIdentifierInRCFile(LPCSTR& pPointer)
{
	while (*pPointer!='$' && *pPointer!='\0')
	{
		if (*pPointer=='/' && *(pPointer+1)=='/')
		{
			// To the end of line
			while (*pPointer!='\n' && *pPointer!='\0')
				pPointer++;
			if (*pPointer=='\0')
				return TRUE;
		}
		else if (strncmp(pPointer,"TEXTINCLUDE",11)==0)
		{
			// Skipping TEXTINCLUDE
			pPointer+=11;
			
			// To end of line
			while (*pPointer!='\n' && *pPointer!='\0')
				pPointer++;
			if (*pPointer=='\0')
				return TRUE;

			while (*pPointer==' ' || *pPointer=='\t' || *pPointer=='\r' || *pPointer=='\n')
				pPointer++;

			// Expecting BEGIN
			if (strncmp(pPointer,"BEGIN",5)!=0)
				return FALSE;

			pPointer+=5;

			// To end of line
			while (*pPointer!='\n' && *pPointer!='\0')
				pPointer++;
			if (*pPointer=='\0')
				return FALSE;
			BOOL bFound=FALSE;
			while (!bFound)
			{
				// Removing white marks
				while (*pPointer==' ' || *pPointer=='\t' || *pPointer=='\r' || *pPointer=='\n')
					pPointer++;
				
				bFound=strncmp(pPointer,"END",3)==0;
				
				// To end of line
				while (*pPointer!='\n' && *pPointer!='\0')
					pPointer++;
				if (*pPointer=='\0')
					return bFound;

				// Removing line marks
				while (*pPointer=='\r' || *pPointer=='\n')
					pPointer++;
			}
		}
		else
			pPointer++;
	}
	return TRUE;
}



BOOL SetArgString(int& i,CString& str,int argc,char* argv[])
{
	if (argv[i][2]=='\0')
	{
		if (i+1>=argc)
			return FALSE;
		str=argv[i+1];
		i++;
	}
	else
		str.Copy(argv[i]+2);
	return TRUE;
}

BOOL SetBaseFileToRegistry(LPCSTR szName,LPCSTR szFile,BOOL bRawHelpPage)
{
	CRegKey RegKey;
	LPCSTR szKey=bRawHelpPage?"Software\\lrestool\\rawHTMLfiles":"Software\\lrestool\\basefiles";
	if (RegKey.OpenKey(HKCU,szKey,CRegKey::createNew|CRegKey::samAll)==ERROR_SUCCESS)
		return RegKey.SetValue(szName,szFile)==ERROR_SUCCESS;
	return FALSE;
}

BOOL GetBaseFileFromRegistry(LPCSTR szName,CString& sFile,BOOL bRawHelpPage)
{
	CRegKey RegKey;
	LPCSTR szKey=bRawHelpPage?"Software\\lrestool\\rawHTMLfiles":"Software\\lrestool\\basefiles";
	if (RegKey.OpenKey(HKCU,szKey,CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
	{
		RegKey.QueryValue(szName,sFile);
		return FileSystem::IsFile(sFile);
	}
	return FALSE;
}

BOOL DeleteBaseFileFromRegistry(LPCSTR szName,BOOL bRawHelpPage)
{
	CRegKey RegKey;
	LPCSTR szKey=bRawHelpPage?"Software\\lrestool\\rawHTMLfiles":"Software\\lrestool\\basefiles";
	if (RegKey.OpenKey(HKCU,szKey,CRegKey::createNew|CRegKey::samAll)==ERROR_SUCCESS)
		return RegKey.DeleteValue(szName);
	return FALSE;	
}

BOOL CIdentifiers::FindProgramNameFromLResFile(LPCSTR szFile,CString& sName)
{
	char* pBuffer=NULL;
	DWORD dwLength;
	
	if (!ReadFileContent(szFile,pBuffer,dwLength))
		return FALSE;

	LPCSTR pPointer=pBuffer;
	DWORD dwReadedLines=1;
	BOOL bComments;
	CString name,text;


	for(;;)
	{
		if (!FindNextIdentifierInLresFile(pPointer,dwReadedLines,bComments))
			break;

		if (!SeperateIdentifierAndText(pPointer,name,text))
		{
			fprintf(_stderr,"Error in line %d in file %s.\n",dwReadedLines,szFile);
			delete[] pBuffer;
			return FALSE;
		}
	
		if (name.CompareNoCase("FOR_PROGRAM")==0)
		{
			sName.Swap(text);
			delete[] pBuffer;
			return TRUE;
		}
	}
	
	delete[] pBuffer;
	sName.Empty();
	return FALSE;
}

void CIdentifiers::CheckDifferences(CIdentifiers& rAnother,BOOL bCheckChanges)
{
	// First check bUsed to FALSE in another structure
	POSITION pPos=rAnother.GetHeadPosition();
	while (pPos!=NULL)
	{
		rAnother.GetAtRef(pPos).bUsed=FALSE;
		pPos=rAnother.GetNextPosition(pPos);
	}

	pPos=GetHeadPosition();
    while (pPos!=NULL)
	{
		PIDENTIFIER pID=GetAtPtr(pPos);
		PIDENTIFIER pAnotherId=rAnother.FindIdentifier(pID->name);


		if (pAnotherId==NULL)
			pID->bUsed=FALSE;
		else
		{
			pID->bUsed=TRUE;
			pAnotherId->bUsed=TRUE;
			
			if (bCheckChanges)
			{
				if (pID->text.Compare(pAnotherId->text)!=0)
				{
					pID->bChanged=TRUE;
					pAnotherId->bChanged=TRUE;
				}
			}
		}

		pPos=GetNextPosition(pPos);
	}

}

void CIdentifiers::PrintUnused(BOOL bLineNumbers)
{
	BYTE bNumbers;

	if (bLineNumbers)
	{
		DWORD dwMaxLines=0;
		char szNumber[40];
	
		POSITION pPos=GetHeadPosition();
		while (pPos!=NULL)
		{
			IDENTIFIER& rID=GetAtRef(pPos);
			if (rID.bChanged && rID.dwLineNumber>dwMaxLines)
				dwMaxLines=rID.dwLineNumber;				

			pPos=GetNextPosition(pPos);
		}

		bNumbers=sprintf_s(szNumber,40,"%d",dwMaxLines);
	}
	
	POSITION pPos=GetHeadPosition();
	while (pPos!=NULL)
	{
		IDENTIFIER& rID=GetAtRef(pPos);

		if (!rID.bUsed)
		{
			if (bLineNumbers)
			{
				char szBuffer[10];
				char* szLinuNumber=new char[bNumbers+2];
				sprintf_s(szBuffer,10,"00%dd",bNumbers);
				szBuffer[0]='%';
				sprintf_s(szLinuNumber,bNumbers+2,szBuffer,rID.dwLineNumber);				

				printf("%s %s: '%s'\n",szLinuNumber,LPCSTR(rID.name),LPCSTR(rID.text));
				delete[] szLinuNumber;
			}
			else
				printf("%s: '%s'\n",LPCSTR(rID.name),LPCSTR(rID.text));
		}

		
		pPos=GetNextPosition(pPos);
	}
}

void CIdentifiers::PrintChanged(CIdentifiers* pAnother,BOOL bLineNumbers)
{
	BYTE bNumbers,bNumbers2;
	if (bLineNumbers)
	{
		DWORD dwMaxLines=0,dwMaxLines2=0;
		char szNumber[40];
	
		POSITION pPos=GetHeadPosition();
		while (pPos!=NULL)
		{
			IDENTIFIER& rID=GetAtRef(pPos);
			if (rID.bChanged && rID.dwLineNumber>dwMaxLines)
				dwMaxLines=rID.dwLineNumber;				

			pPos=GetNextPosition(pPos);
		}
	
		pPos=pAnother->GetHeadPosition();
		while (pPos!=NULL)
		{
			IDENTIFIER& rID=pAnother->GetAtRef(pPos);
			if (rID.bChanged && rID.dwLineNumber>dwMaxLines2)
				dwMaxLines2=rID.dwLineNumber;				

			pPos=pAnother->GetNextPosition(pPos);
		}
	
		bNumbers=sprintf_s(szNumber,40,"%d",dwMaxLines);
		bNumbers2=sprintf_s(szNumber,40,"%d",dwMaxLines2);
	}
	
	POSITION pPos=GetHeadPosition();
	while (pPos!=NULL)
	{
		IDENTIFIER& rID=GetAtRef(pPos);

		if (rID.bChanged)
		{
			IDENTIFIER* pAnotherID=NULL;
			if (pAnother!=NULL)
				pAnotherID=pAnother->FindIdentifier(rID.name);

			if (bLineNumbers)
			{
				char szBuffer[10];
				char* szLinuNumber=new char[bNumbers+2];
				sprintf_s(szBuffer,10,"00%dd",bNumbers);
				szBuffer[0]='%';
				sprintf_s(szLinuNumber,bNumbers+2,szBuffer,rID.dwLineNumber);	

				if (pAnotherID==NULL)			
					printf("%04d %s: '%s'\n",rID.dwLineNumber,LPCSTR(rID.name),LPCSTR(rID.text));
				else
				{
					char* szLinuNumber2=new char[bNumbers2+2];
					sprintf_s(szBuffer,bNumbers2+2,"00%dd",bNumbers2);
					szBuffer[0]='%';
					sprintf_s(szLinuNumber2,bNumbers2+2,szBuffer,pAnotherID->dwLineNumber);	


					printf("%s %s: '%s'<-->%s '%s'\n",szLinuNumber,LPCSTR(rID.name),LPCSTR(rID.text),
						szLinuNumber2,LPCSTR(pAnotherID->text));
					delete[] szLinuNumber2;
				}

				delete[] szLinuNumber;
			}
			else
			{							
				if (pAnotherID==NULL)			
					printf("%s: '%s'\n",LPCSTR(rID.name),LPCSTR(rID.text));
				else
					printf("%s: '%s'<-->'%s'\n",LPCSTR(rID.name),LPCSTR(rID.text),LPCSTR(pAnotherID->text));
			}

		}
		
		pPos=GetNextPosition(pPos);
	}
}

BOOL CIdentifiers::ReadFileContent(LPCSTR szFile,char*& pBuffer,DWORD& dwLength)
{
	try{
        CFile file;
		file.SetToThrow();
		file.Open(szFile,CFile::defRead|CFile::typeText);

		dwLength=file.GetLength();
		if (dwLength<3)
		{
			fprintf(_stderr,"error: empty file '%s'\n",szFile);
			return FALSE;
		}
		pBuffer=new char[dwLength+1];
		file.Read(pBuffer,dwLength);
		pBuffer[dwLength]='\0';
		file.Close();
	}
	catch(...)
	{
		if (pBuffer!=NULL)
		{
			delete[] pBuffer;
			pBuffer=NULL;
		}
		fprintf(_stderr,"error: cannot read file '%s'\n",szFile);
		return FALSE;
	}
	return TRUE;
}


BOOL CIdentifiers::WriteFileContent(LPCSTR szFile,char* pData,DWORD dwLength)
{
	try{
        CFile file;
		file.SetToThrow();
		file.Open(szFile,CFile::defWrite|CFile::typeText);
		file.Write(pData,dwLength);
		file.Close();
	}
	catch(...)
	{
		fprintf(_stderr,"error: cannot write file '%s'\n",szFile);
		return FALSE;
	}
	return TRUE;
}

BOOL CIdentifiers::FindIdentifierInLresFile(LPCSTR& pPointer,LPCSTR szIdName,int iIdNameLength)
{
	DWORD dwReadedLines=0;
	BOOL bIsComments;
	LPCSTR szName;
	DWORD dwNameLength;
	if (iIdNameLength==-1)
		iIdNameLength=istrlen(szIdName);

	while (FindNextIdentifierInLresFile(pPointer,dwReadedLines,bIsComments))
	{
		LPCSTR pTemp=pPointer;
		
		if (!SeperateIdentifierAndText(pTemp,szName,dwNameLength))
			return FALSE;

		if (iIdNameLength==dwNameLength)
		{
			if (strncmp(szName,szIdName,dwNameLength)==0)
				return TRUE;
		}

		pPointer=pTemp;
	}
	return FALSE;
}
	
BOOL CIdentifiers::DeleteIdentifier(LPSTR& pData,DWORD& dwLength,LPCSTR szId,BOOL bCommentOnly)
{
	LPCSTR pPointer=pData;
	if (!FindIdentifierInLresFile(pPointer,szId))
	{
		fprintf(_stderr,"error: cannot delete identifier '%s': not found\n",szId);
		return FALSE;
	}

	
	if (bCommentOnly)
		InsertTextToIndex(pData,dwLength,DWORD(pPointer-pData),"#OBSOLETE ",10);
	else
	{
		int iLength;
		for (iLength=0;pPointer[iLength]!='\n' && pPointer[iLength]!='\0';iLength++);

		if (pPointer[iLength]=='\0')
			pData[dwLength=DWORD(pPointer-pData)]='\0';
		else
			RemoveTextFromIndex(pData,dwLength,DWORD(pPointer-pData),iLength);
	}
	return TRUE;
}

BOOL CIdentifiers::InsertAfterIdentifier(LPSTR& pData,DWORD& dwLength,LPCSTR szId,LPCSTR szText,LPCSTR szAfterID,BOOL bComment)
{
	LPCSTR pPointer=pData;

	if (!FindIdentifierInLresFile(pPointer,szAfterID))
	{
		fprintf(_stderr,"error: cannot insert new identifier '%s': '%s' is not found\n",szId,szAfterID);
		return FALSE;
	}
	
	// Finding next line
	while (*pPointer!='\n') pPointer++;
	pPointer++;
	
	CString txt;
	if (bComment)
		txt << "# New ID:\r\n";
	txt<<szId<<'='<<szText<<"\r\n";

	InsertTextToIndex(pData,dwLength,DWORD(pPointer-pData),txt,txt.GetLength());
	return TRUE;
}

BOOL CIdentifiers::InsertBeforeIdentifier(LPSTR& pData,DWORD& dwLength,LPCSTR szId,LPCSTR szText,LPCSTR szBeforeID,BOOL bComment)
{
	LPCSTR pPointer=pData;

	if (!FindIdentifierInLresFile(pPointer,szBeforeID))
	{
		fprintf(_stderr,"error: cannot insert new identifier '%s': '%s' is not found\n",szId,szBeforeID);
		return FALSE;
	}

	CString txt;
	if (bComment)
		txt << "# New ID:\r\n";
	txt<<szId<<'='<<szText<<"\r\n";

	InsertTextToIndex(pData,dwLength,DWORD(pPointer-pData),txt,txt.GetLength());
	return TRUE;
}

BOOL CIdentifiers::InsertToEndOfFile(LPSTR& pData,DWORD& dwLength,LPCSTR szId,LPCSTR szText,BOOL bComment)
{
	CString txt;
	if (bComment)
		txt << "\r\n# New ID(s):\r\n";
	txt<<szId<<'='<<szText<<"\r\n";


	InsertTextToIndex(pData,dwLength,dwLength,txt,txt.GetLength());
	
	return TRUE;
}

BOOL CIdentifiers::FindNextIdentifierInLresFile(LPCSTR& pPointer,DWORD& dwReadedLines,BOOL& bIsThereComments)
{
	bIsThereComments=FALSE;
	while (*pPointer==' ' || *pPointer=='\t' || *pPointer=='\r' || 
			*pPointer=='\n' || *pPointer=='#') 
	{													
		if (*pPointer=='\n')								
			dwReadedLines++;								
		if (*pPointer=='#')								
		{												
			bIsThereComments=TRUE;
			while (*pPointer!='\n' && *pPointer!='\0') 
				pPointer++;
			if (*pPointer!='\0') 
				pPointer++; 
			dwReadedLines++;
		}												
		else											
			pPointer++;										
	}
	
	return *pPointer!='\0';
}

BOOL CIdentifiers::SeperateIdentifierAndText(LPCSTR& pPointer,LPCSTR& szName,DWORD& dwNameLength)
{
	// Good assumptions is that a next string is an identifier
	int nIndex=0;
	for (;pPointer[nIndex]!='=';nIndex++)
	{
		if (!ISVALIDCHAR(pPointer[nIndex]))
			return FALSE;
	}
	
	szName=pPointer;
	dwNameLength=nIndex;
	
	pPointer+=nIndex+1;
	for (nIndex=0;pPointer[nIndex]!='\n' && pPointer[nIndex]!='\r' && 
		pPointer[nIndex]!='\0';nIndex++);
	pPointer+=nIndex+1;
	return TRUE;
}
	

BOOL CIdentifiers::SeperateIdentifierAndText(LPCSTR& pPointer,CString& name,CString& text)
{
	// Good assumptions is that a next string is an identifier
	int nIndex=0;
	for (;pPointer[nIndex]!='=';nIndex++)
	{
		if (!ISVALIDCHAR(pPointer[nIndex]))
			return FALSE;
	}
	
	name.Copy(pPointer,nIndex);
	pPointer+=nIndex+1;
	for (nIndex=0;pPointer[nIndex]!='\n' && pPointer[nIndex]!='\r' && 
		pPointer[nIndex]!='\0';nIndex++);
	text.Copy(pPointer,nIndex);		
    pPointer+=nIndex+1;
	return TRUE;
}


BOOL CIdentifiers::InsertTextToIndex(LPSTR& pData,DWORD& dwLength,DWORD nIndex,LPCSTR szText,DWORD dwTextLength)
{
	DWORD dwOldLength=dwLength;
	dwLength+=dwTextLength;

	LPSTR pOldData=pData;
	pData=new char[dwLength+2];
	if (pData==NULL)
		return FALSE;

	CopyMemory(pData,pOldData,nIndex);
	CopyMemory(pData+nIndex,szText,dwTextLength);
	CopyMemory(pData+nIndex+dwTextLength,pOldData+nIndex,dwOldLength-nIndex);
	
	pData[dwLength]='\0';
	delete[] pOldData;

	return TRUE;
}

BOOL CIdentifiers::RemoveTextFromIndex(LPSTR& pData,DWORD& dwLength,DWORD nIndex,DWORD dwTextLength)
{
	DWORD dwOldLength=dwLength;
	dwLength-=dwTextLength;

	LPSTR pOldData=pData;
	pData=new char[dwLength+2];
	if (pData==NULL)
		return FALSE;

	CopyMemory(pData,pOldData,nIndex);
	CopyMemory(pData+nIndex,pOldData+nIndex+dwTextLength,dwLength-nIndex);
	pData[dwLength]='\0';
	return TRUE;
}
