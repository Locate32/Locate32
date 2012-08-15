/* Locate32 - Copyright (c) 1997-2010 Janne Huttunen */


#ifndef KEYHOOK_EXPORTS
// Included to Locate
#include <HFCLib.h>
#include "Locate32.h"

#else

// Included to keyhook
#include <windows.h>

#include <hfcdef.h>
#include <hfcarray.h>

#include "../lan_resources.h"
#include "../locate32/shortcut.h"
#include "../keyhook/keyhelper.h"
#include "../locate32/messages.h"

#endif

static BOOL _ContainString(LPCSTR s1,LPCSTR s2,int s2len) // Is s2 in the s1
{


    BOOL bBreakIfNotMatch;
	if (s2[0]=='*')
	{
		if (s2len==1)
			return TRUE;
		s2++;
		bBreakIfNotMatch=FALSE;
	}
	else
		bBreakIfNotMatch=TRUE;

	while (*s1!='\0')
	{
		for (int i=0;;i++)
		{
			// is s1 too short?
			if (s1[i]=='\0')
			{
				if (s2len<=i)
					return TRUE;
				return s2[i]=='*' && s2len<=i+1;
			}
			// string differ
			if (s1[i]!=s2[i])
			{
				if (s2[i]=='?')
					continue;
				
				if (s2[i]=='*')
				{
					if (s2len<=i+1)
						return TRUE;
					s2+=i+1;
					s1+=i-1;
					bBreakIfNotMatch=FALSE;
					break;
				}
				break;
			}
		}
		if (bBreakIfNotMatch)
			return FALSE;
		s1++;
	}
	return FALSE;
}

#ifndef KEYHOOK_EXPORTS


BOOL __ContainString(LPCWSTR s1,LPCWSTR s2) // Is s2 in the s1
{

	BOOL bBreakIfNotMatch;
	if (s2[0]=='*')
	{
		if (s2[1]=='\0')
			return TRUE;
		s2++;
		bBreakIfNotMatch=FALSE;
	}
	else
		bBreakIfNotMatch=TRUE;

	while (*s1!='\0')
	{
		for (int i=0;;i++)
		{
			// is s1 too short?
			if (s1[i]=='\0')
			{
				if (s2[i]=='\0')
					return TRUE;
				return s2[i]=='*' && s2[i+1]=='\0';
			}
			// string differ
			if (s1[i]!=s2[i])
			{
				if (s2[i]=='?')
					continue;
				
				if (s2[i]=='*')
				{
					if (s2[i+1]=='\0')
						return TRUE;
					s2+=i+1;
					s1+=i-1;
					bBreakIfNotMatch=FALSE;
					break;
				}
				break;
			}
		}
		if (bBreakIfNotMatch)
			return FALSE;
		s1++;
	}
	return FALSE;
}



/////////////////////////////////////////////
// CShortcut


CShortcut::CShortcut()
:	m_dwFlags(sfDefault),m_nDelay(0),
	m_bModifiers(0),m_bVirtualKey(0),
	m_pClass(NULL),m_pTitle(NULL) // This initializes union
{
	// At least one action is needed
	m_apActions.Add(new CAction);

	if ((m_dwFlags&sfKeyTypeMask)==sfLocal)
		m_wWhenPressed=wpDefault;
}

CShortcut::CShortcut(CShortcut& rCopyFrom)
{
	CopyMemory(this,&rCopyFrom,sizeof(CShortcut));

	if ((m_dwFlags&sfKeyTypeMask)!=sfLocal)
	{
		if (m_pClass!=NULL && m_pClass!=LPSTR(-1))
			m_pClass=alloccopy(m_pClass);
		if (m_pTitle!=NULL)
			m_pTitle=alloccopy(m_pTitle);
	}

	for (int i=0;i<m_apActions.GetSize();i++)
		m_apActions[i]=new CAction(*m_apActions[i]);
}
	

void CShortcut::ClearExtraInfo()
{
	if ((m_dwFlags&sfKeyTypeMask)!=sfLocal)
	{
		if (m_pClass!=NULL && m_pClass!=LPSTR(-1))
			delete[] m_pClass;
		if (m_pTitle!=NULL)
			delete[] m_pTitle;
	}
	
	m_pClass=NULL;m_pTitle=NULL;
}

BYTE CShortcut::HotkeyModifiersToModifiers(BYTE bHotkeyModifier)
{
	BYTE bModifiers=0;
	if (bHotkeyModifier&HOTKEYF_ALT)
		bModifiers|=ModifierAlt;
	if (bHotkeyModifier&HOTKEYF_CONTROL)
		bModifiers|=ModifierControl;
	if (bHotkeyModifier&HOTKEYF_SHIFT)
		bModifiers|=ModifierShift;
	return bModifiers;
}

BYTE CShortcut::ModifiersToHotkeyModifiers(BYTE bModifier)
{
	BYTE bRet=0;
	if (bModifier&ModifierAlt)
		bRet|=HOTKEYF_ALT;
	if (bModifier&ModifierControl)
		bRet|=HOTKEYF_CONTROL;
	if (bModifier&ModifierShift)
		bRet|=HOTKEYF_SHIFT;
	return bRet;
}

BOOL CShortcut::LoadShortcuts(CArrayFP<CShortcut*>& aShortcuts,BYTE bLoadFlags)
{
	CRegKey2 RegKey;
	if (RegKey.OpenKey(HKCU,"\\General",CRegKey::defRead)!=ERROR_SUCCESS)
		return FALSE;

	DWORD dwDataLength=RegKey.QueryValueLength("Shortcuts");
	if (dwDataLength<4)
		return FALSE;

	BYTE* pData=new BYTE[dwDataLength];
	RegKey.QueryValue("Shortcuts",(LPSTR)pData,dwDataLength,NULL);
	RegKey.CloseKey();

	BOOL bRet=LoadShortcuts(pData,dwDataLength,aShortcuts,bLoadFlags);
	delete[] pData;
	return bRet;
}

BOOL CShortcut::LoadShortcuts(const BYTE* pData,DWORD dwDataLength,CArrayFP<CShortcut*>& aShortcuts,BYTE bLoadFlag)
{
	DWORD dwShortcuts=0;

	const BYTE* pPtr=pData;
	while (dwDataLength>=4 && *((DWORD*)pPtr)!=NULL)
	{
		DWORD dwLength;

		if (dwShortcuts==43)
		{
			ASSERT(1);
		}

		CShortcut* pShortcut=CShortcut::FromData(pPtr,dwDataLength,dwLength);

		if (pShortcut!=NULL)
			dwShortcuts++;
		else
			ASSERT(0);


		if (pShortcut==NULL)
			return FALSE;
		
		BOOL bLoad=FALSE;

		switch (pShortcut->m_dwFlags&sfKeyTypeMask)
		{
		case sfLocal:
			if (bLoadFlag&loadLocal)
				bLoad=TRUE;
			break;
		case sfGlobalHotkey:
			if (bLoadFlag&loadGlobalHotkey)
				bLoad=TRUE;
			break;
		case sfGlobalHook:
			if (bLoadFlag&loadGlobalHook)
				bLoad=TRUE;
			break;
		}

		if (bLoad)
			aShortcuts.Add(pShortcut);
		else
			delete pShortcut;

		pPtr+=dwLength;
		dwDataLength-=dwLength;
	}

	return TRUE;
}

BOOL CShortcut::SaveShortcuts(const CArrayFP<CShortcut*>& aShortcuts)
{
	DWORD dwLength=sizeof(DWORD);
	int i;

	CRegKey2 RegKey;
	if (RegKey.OpenKey(HKCU,"\\General",CRegKey::defWrite)!=ERROR_SUCCESS)
		return FALSE;

	for (i=0;i<aShortcuts.GetSize();i++)
		dwLength+=aShortcuts[i]->GetDataLength();

    BYTE* pData=new BYTE[dwLength];
	DWORD dwUsed=0;

	for (i=0;i<aShortcuts.GetSize();i++)
		dwUsed+=aShortcuts[i]->GetData(pData+dwUsed);
	
	*((DWORD*)(pData+dwUsed))=NULL;

	ASSERT(dwUsed+sizeof(DWORD)==dwLength);

	BOOL bRet=RegKey.SetValue("Shortcuts",(LPSTR)pData,dwLength,REG_BINARY)==ERROR_SUCCESS;
    delete[] pData;

	return bRet;
}

DWORD CShortcut::GetData(BYTE* _pData) const
{
	BYTE* pData=_pData;
	
	*((WORD*)pData)=0xFFED; // Start mark
	pData+=sizeof(WORD);

	// Flags
	*((WORD*)pData)=m_dwFlags;

	// Virtual key
	*(pData+2)=m_bVirtualKey;
	
	// Modfiers
	*(pData+3)=m_bModifiers;

	// Class
	if (m_pClass==LPSTR(-1))
		*((DWORD*)(pData+4))=DWORD(-1);
	else
		*((DWORD*)(pData+4))=(DWORD)min(ULONG_PTR(m_pClass),MAXDWORD);
	
	// Title
	*((DWORD*)(pData+8))=(DWORD)min(ULONG_PTR(m_pTitle),MAXDWORD);
	
	// Delay
	*((DWORD*)(pData+12))=m_nDelay;

	
	// Array
	*((DWORD*)(pData+24))=m_apActions.GetSize();
		
	
	
	pData+=28 /*sizeof(CShortcut)*/;


	if ((m_dwFlags&sfKeyTypeMask)!=sfLocal)
	{
		if (m_pClass!=NULL && m_pClass!=LPSTR(-1))
		{
			int iUsed=istrlen(m_pClass)+1;
			CopyMemory(pData,m_pClass,iUsed);
			pData+=iUsed;
		}

		if (m_pTitle!=NULL)
		{
            int iUsed=istrlen(m_pTitle)+1;
			CopyMemory(pData,m_pTitle,iUsed);
			pData+=iUsed;
		}
	}

	for (int i=0;i<m_apActions.GetSize();i++)
	{
		pData+=m_apActions[i]->GetData(pData);
	}

	return DWORD(pData-_pData);
}

DWORD CShortcut::GetDataLength() const
{
	DWORD dwLen=sizeof(WORD)+28 /*sizeof(CShortcut)*/;
	if ((m_dwFlags&sfKeyTypeMask)!=sfLocal)
	{
		if (m_pClass!=NULL && m_pClass!=LPSTR(-1))
			dwLen+=DWORD(istrlen(m_pClass)+1);

		if (m_pTitle!=NULL)
			dwLen+=DWORD(istrlen(m_pTitle)+1);
	}

	for (int i=0;i<m_apActions.GetSize();i++)
		dwLen+=m_apActions[i]->GetDataLength();

	return dwLen;
}

CShortcut* CShortcut::FromData(const BYTE* pData,DWORD dwDataLen,DWORD& dwUsed)
{
	DWORD dwLen;

	if (dwDataLen<28+sizeof(WORD))
		return NULL;

    if (*((WORD*)pData)!=0xFFED)
		return NULL;
	pData+=sizeof(WORD);

	CShortcut* pShortcut=new CShortcut((void*)NULL);
	
	// Flags
	pShortcut->m_dwFlags=*((WORD*)pData);

	// Virtual key
	pShortcut->m_bVirtualKey=*(pData+2);
	
	// Modfiers
	pShortcut->m_bModifiers=*(pData+3);

	if ((pShortcut->m_dwFlags&sfKeyTypeMask)==sfLocal)
		pShortcut->m_wWhenPressed=*((WORD*)(pData+4));
	else
	{
		// Class
		if (*((DWORD*)(pData+4))==DWORD(-1))
			pShortcut->m_pClass=LPSTR(-1);
		else
			pShortcut->m_pClass=(LPSTR)*((DWORD*)(pData+4));
		
		// Title
		pShortcut->m_pTitle=(LPSTR)*((DWORD*)(pData+8));
	}

	// Delay
	pShortcut->m_nDelay=*((DWORD*)(pData+12));

	
	// Array
	DWORD dwActions=*((DWORD*)(pData+24));
		
	pData+=28; // sizeof(CShortcut)==28 in Win32
	dwUsed=sizeof(WORD)+28; 
	dwDataLen-=sizeof(WORD)+28;

	if ((pShortcut->m_dwFlags&sfKeyTypeMask)!=sfLocal) // Load class and title 
	{
		if (pShortcut->m_pClass!=NULL && pShortcut->m_pClass!=LPSTR(-1))
		{
			for (dwLen=0;dwLen<dwDataLen && pData[dwLen]!='\0';dwLen++);

			if (dwLen==dwDataLen)
			{
				delete pShortcut;
				return NULL;
			}

			dwLen++;
			pShortcut->m_pClass=new char[dwLen];
			CopyMemory(pShortcut->m_pClass,pData,dwLen);
			dwUsed+=dwLen;
			pData+=dwLen;
			dwDataLen-=dwLen;
		}
		
		if (pShortcut->m_pTitle!=NULL)
		{
			for (dwLen=0;dwLen<dwDataLen && pData[dwLen]!='\0';dwLen++);

			if (dwLen==dwDataLen)
			{
				delete pShortcut;
				return NULL;
			}

			dwLen++;
			pShortcut->m_pTitle=new char[dwLen];
			CopyMemory(pShortcut->m_pTitle,pData,dwLen);
			dwUsed+=dwLen;
			pData+=dwLen;
			dwDataLen-=dwLen;
		}
	}

	for (DWORD i=0;i<dwActions;i++)
	{
		CAction* pAction=CAction::FromData(pData,dwDataLen,dwLen);

		if (pAction==NULL)
		{
			delete pShortcut;
			return NULL;
		}

		pShortcut->m_apActions.Add(pAction);
		pData+=dwLen;
		dwUsed+=dwLen;
		dwDataLen-=dwLen;

	}
	return pShortcut;
}

CSubAction* CSubAction::FromData(DWORD nAction,const BYTE* pData,DWORD dwDataLen,DWORD& dwUsed)
{
	if (dwDataLen<sizeof(WORD))
		return NULL;

	if (*((WORD*)pData)!=0xFFEA)
		return NULL;
	
	CSubAction* pSubAction=new CSubAction((void*)NULL);
	dwUsed=pSubAction->FillFromData(nAction,pData+sizeof(WORD),dwDataLen-sizeof(WORD));
	if (dwUsed==0)
	{
		delete pSubAction;
		return NULL;
	}


	dwUsed+=sizeof(WORD);
	return pSubAction;
}
	
DWORD CSubAction::FillFromData(DWORD nAction,const BYTE* pData,DWORD dwDataLen)
{
	if (dwDataLen<2*sizeof(DWORD))
		return 0;
	
	DWORD dwUsed=0;
		
	
	m_nSubAction=*((DWORD*)(pData));
	m_pExtraInfo=(void*)*((DWORD*)(pData+sizeof(DWORD)));

	pData+=2*sizeof(DWORD);
	dwUsed=2*sizeof(DWORD);
	dwDataLen-=2*sizeof(DWORD);

	switch (nAction)
	{
	case CAction::ResultListItems:
		if (m_nResultList==Execute && m_szVerb!=NULL)
		{
			DWORD dwLen;

			for (dwLen=0;dwLen<dwDataLen && ((LPCWSTR)pData)[dwLen]!='\0';dwLen++);

			if (dwLen==dwDataLen)
				return 0;

			dwLen++;
			m_szVerb=new WCHAR[dwLen];
            MemCopyW(m_szVerb,(LPCWSTR)pData,dwLen);
			dwUsed+=dwLen*2;
			pData+=dwLen*2;
			dwDataLen-=dwLen;
		}
		else if (m_nResultList==ExecuteCommand && m_szCommand!=NULL)
		{
			DWORD dwLen;

			for (dwLen=0;dwLen<dwDataLen && ((LPCWSTR)pData)[dwLen]!='\0';dwLen++);

			if (dwLen==dwDataLen)
				return 0;
			
			dwLen++;
			m_szCommand=new WCHAR[dwLen];
            MemCopyW(m_szCommand,(LPCWSTR)pData,dwLen);
			dwUsed+=dwLen*2;
			pData+=dwLen*2;
			dwDataLen-=dwLen;
		}
		break;
	case CAction::Misc:
		if ((m_nMisc==SendMessage || m_nMisc==PostMessage) && 			
			m_pSendMessage!=NULL)
		{
			DWORD dwLen;

			m_pSendMessage=SendMessageInfo::FromData(pData,dwDataLen,dwLen);

			if (m_pSendMessage==NULL)
				return 0;
			
			pData+=dwLen;
			dwUsed+=dwLen;
			dwDataLen-=dwLen;
		}
		else if (m_nMisc==ExecuteCommandMisc && m_szCommand!=NULL)
		{
			DWORD dwLen;

			for (dwLen=0;dwLen<dwDataLen && ((LPCWSTR)pData)[dwLen]!='\0';dwLen++);

			if (dwLen==dwDataLen)
				return 0;
			
			dwLen++;
			m_szCommand=new WCHAR[dwLen];
            MemCopyW(m_szCommand,(LPCWSTR)pData,dwLen);
			dwUsed+=dwLen*2;
			pData+=dwLen*2;
			dwDataLen-=dwLen;
		}
		break;
	case CAction::Presets:
	case CAction::ChangeValue:
		if (m_szPreset!=NULL)
		{
			DWORD dwLen;

			for (dwLen=0;dwLen<dwDataLen && ((LPCWSTR)pData)[dwLen]!='\0';dwLen++);

			if (dwLen==dwDataLen)
				return 0;
			
			dwLen++;
			m_szPreset=new WCHAR[dwLen];
            MemCopyW(m_szPreset,(LPCWSTR)pData,dwLen);
			dwUsed+=dwLen*2;
			pData+=dwLen*2;
			dwDataLen-=dwLen;
		}
		break;
	}
	return dwUsed;
}
	

CAction* CAction::FromData(const BYTE* pData,DWORD dwDataLen,DWORD& dwUsed)
{
	if (dwDataLen<sizeof(DWORD)+sizeof(WORD))
		return NULL;
	
	if (*((WORD*)pData)!=0xFFEC)
		return NULL;
	
	CAction* pAction=new CAction((void*)NULL);
	pAction->m_dwAction=*((DWORD*)(pData+sizeof(WORD)));
		
	dwUsed=pAction->FillFromData(pAction->m_dwAction,
		pData+sizeof(WORD)+sizeof(DWORD),dwDataLen-sizeof(WORD)-sizeof(DWORD));
	if (dwUsed==0)
	{
		delete pAction;
		return NULL;
	}

	dwUsed+=sizeof(WORD)+sizeof(DWORD);

	return pAction;
}



	

CAction::SendMessageInfo* CAction::SendMessageInfo::FromData(const BYTE* pData,DWORD dwDataLen,DWORD& dwUsed)
{
	if (dwDataLen<sizeof(WORD)+sizeof(DWORD)+sizeof(3))
		return NULL;
	
	if (*((WORD*)pData)!=0xFFEB)
		return NULL;

	SendMessageInfo* pSendMessage=new SendMessageInfo;
	
	// Set nMessage
	pSendMessage->nMessage=*((DWORD*)(pData+2));
	
	pData+=sizeof(DWORD)+sizeof(WORD);
	dwUsed=sizeof(DWORD)+sizeof(WORD);
	dwDataLen-=sizeof(DWORD)+sizeof(WORD);

	
	// Set szWindow
	DWORD dwLen;
	for (dwLen=0;dwLen*2<dwDataLen && ((WCHAR*)pData)[dwLen]!='\0';dwLen++);
	if (dwLen*2==dwDataLen)
	{
		delete pSendMessage;
		return NULL;
	}
	dwLen++;
	if (dwLen>1)
	{
		pSendMessage->szWindow=new WCHAR[dwLen];
		MemCopyW(pSendMessage->szWindow,(LPCWSTR)pData,dwLen);
	}
	else
		pSendMessage->szWindow=NULL;

	dwUsed+=dwLen*2;
	pData+=dwLen*2;
	dwDataLen-=dwLen*2;

	// Set szParam
	for (dwLen=0;dwLen*2<dwDataLen && ((WCHAR*)pData)[dwLen]!='\0';dwLen++);
	if (dwLen*2==dwDataLen)
	{
		delete pSendMessage;
		return NULL;
	}
	dwLen++;
	if (dwLen>1)
	{
		pSendMessage->szWParam=new WCHAR[dwLen];
		MemCopyW(pSendMessage->szWParam,(LPCWSTR)pData,dwLen);
	}
	else
		pSendMessage->szWParam=NULL;

	dwUsed+=dwLen*2;
	pData+=dwLen*2;
	dwDataLen-=dwLen*2;


	// Set szLParam
	for (dwLen=0;dwLen*2<dwDataLen && ((WCHAR*)pData)[dwLen]!='\0';dwLen++);
	if (dwLen*2==dwDataLen)
	{
		delete pSendMessage;
		return NULL;
	}
	dwLen++;
	if (dwLen>1)
	{
		pSendMessage->szLParam=new WCHAR[dwLen];
		MemCopyW(pSendMessage->szLParam,(LPCWSTR)pData,dwLen);
	}
	else
		pSendMessage->szLParam=NULL;

	dwUsed+=dwLen*2;
	pData+=dwLen*2;
	dwDataLen-=dwLen*2;


	return pSendMessage;
}



DWORD CSubAction::GetData(DWORD nAction,BYTE* pData_,BOOL bHeader) const
{
	BYTE* pData=pData_;
	
	if (bHeader)
	{
		*((WORD*)pData)=0xFFEA;
		pData+=sizeof(WORD);
	}

	*((DWORD*)pData)=m_nSubAction;
	*((DWORD*)(pData+sizeof(DWORD)))=(DWORD)min(LONG_PTR(m_pExtraInfo),MAXDWORD);
	pData+=2*sizeof(DWORD);
	

	switch (nAction)
	{
	case CAction::ResultListItems:
		if (m_nResultList==Execute && m_szVerb!=NULL)
		{
			DWORD dwUsed=DWORD(istrlenw(m_szVerb)+1);
			MemCopyW((LPWSTR)pData,m_szVerb,dwUsed);
			pData+=dwUsed*2;
		}
		else if (m_nResultList==ExecuteCommand && m_szCommand!=NULL)
		{
			DWORD dwUsed=DWORD(istrlenw(m_szCommand)+1);
			MemCopyW((LPWSTR)pData,m_szCommand,dwUsed);
			pData+=dwUsed*2;
		}
		break;
	case CAction::Misc:
		if ((m_nMisc==SendMessage || m_nMisc==PostMessage) && 			
			m_pSendMessage!=NULL)
		{
			DWORD dwUsed=m_pSendMessage->GetData(pData);
			pData+=dwUsed;
		}
		else if (m_nMisc==ExecuteCommandMisc && m_szCommand!=NULL)
		{
			DWORD dwUsed=DWORD(istrlenw(m_szCommand)+1);
			MemCopyW((LPWSTR)pData,m_szCommand,dwUsed);
			pData+=dwUsed*2;
		}
		break;
	case CAction::Presets:
	case CAction::ChangeValue:
		if (m_szPreset!=NULL)
		{
			DWORD dwUsed=DWORD(istrlenw(m_szPreset)+1);
			MemCopyW((LPWSTR)pData,m_szPreset,dwUsed);
			pData+=dwUsed*2;
		}
		break;
	}

	return DWORD(pData-pData_);
}




BOOL CShortcut::GetDefaultShortcuts(CArrayFP<CShortcut*>& aShortcuts,BYTE bLoadFlag)
{
	// This code saves currently saved shortcuts to correct file
	// Uncommend and run once

	
	/*
	// BEGIN 
	{
	LPCSTR szFile="C:\\Users\\jmhuttun\\Programming\\projects\\Locate\\Locate32\\commonres\\defaultshortcuts.dat";
	CRegKey2 RegKey;
	if (RegKey.OpenKey(HKCU,"\\General",CRegKey::defRead)!=ERROR_SUCCESS)
		return FALSE;
	DWORD dwDataLength=RegKey.QueryValueLength("Shortcuts");
	if (dwDataLength<4)
		return FALSE;
	BYTE* pData=new BYTE[dwDataLength];
	RegKey.QueryValue("Shortcuts",(LPSTR)pData,dwDataLength,NULL);
	RegKey.CloseKey();
	CFile File(TRUE);
	File.CloseOnDelete();
		

	try {
		File.Open(szFile,CFile::defWrite);
		File.Write(pData,dwDataLength);
		File.Close();
	}
	catch (...)
	{
	}
	}
	// END
	*/
	

	// Check file
	CStringW Path(GetLocateApp()->GetExeNameW());
	Path.FreeExtra(Path.FindLast(L'\\')+1);
	Path << L"defshrtc.dat";
    
	BYTE* pData=NULL;
	DWORD dwLength;
	try {
		CFile File(Path,CFile::defRead|CFile::otherErrorWhenEOF,TRUE);
		File.CloseOnDelete();

		dwLength=File.GetLength();
		pData=new BYTE[dwLength];
		File.Read(pData,dwLength);
		File.Close();
		BOOL bRet=LoadShortcuts(pData,dwLength,aShortcuts,bLoadFlag);
		delete[] pData;
		
		if (bRet)
            return TRUE;
	}
	catch (...)
	{
		if (pData!=NULL)
			delete[] pData;
	}



	HRSRC hRsrc=FindResource(GetInstanceHandle(),MAKEINTRESOURCE(IDR_DEFAULTSHORTCUTS),"DATA");
    if (hRsrc==NULL)
		return FALSE;
    	
	dwLength=SizeofResource(GetInstanceHandle(),hRsrc);
	if (dwLength<4)
		return FALSE;

	HGLOBAL hGlobal=LoadResource(GetInstanceHandle(),hRsrc);
	if (hGlobal==NULL)
		return FALSE;

	return LoadShortcuts((const BYTE*)LockResource(hGlobal),dwLength,aShortcuts,bLoadFlag);
}

char CShortcut::GetMnemonicForAction(HWND* hDialogs) const
{

	for (int i=0;i<m_apActions.GetSize();i++)
	{
		CAction* pAction=m_apActions[i];

		if (pAction->m_nAction==CAction::ActivateControl)
		{
			for (int j=0;hDialogs[j]!=NULL;j++)
			{	
				if (HIWORD(pAction->m_nControl)& (1<<15))
					continue;


				HWND hControl=::GetDlgItem(hDialogs[j],HIWORD(pAction->m_nControl));
				if (hControl!=NULL)
				{
					DWORD dwTextLen=(DWORD)::SendMessage(hControl,WM_GETTEXTLENGTH,0,0);
					char* pText=new char[dwTextLen+2];
					::SendMessage(hControl,WM_GETTEXT,dwTextLen+2,LPARAM(pText));

					for (DWORD k=0;k<dwTextLen-1;k++)
					{
						if (pText[k]=='&')
						{
							char cRet=pText[k+1];
							delete[] pText;
							MakeUpper(&cRet,1);
							return cRet;
						}
					}

					delete[] pText;
					break;
				}
			}
		}
	}

	return 0;
}

void CSubAction::GetCopyFrom(DWORD nAction,CSubAction& rCopyFrom)
{
	CopyMemory(this,&rCopyFrom,sizeof(CSubAction));

	switch (nAction)
	{
	case CAction::ResultListItems:
		if (m_nResultList==Execute && m_szVerb!=NULL)
			m_szVerb=alloccopy(m_szVerb);
		else if (m_nResultList==ExecuteCommand && m_szCommand!=NULL)
			m_szCommand=alloccopy(m_szCommand);
		break;
	case CAction::Misc:
		if ((m_nMisc==SendMessage || m_nMisc==PostMessage ) &&
			m_pSendMessage!=NULL)
			m_pSendMessage=new SendMessageInfo(*m_pSendMessage);
		else if (m_nMisc==ExecuteCommandMisc && m_szCommand!=NULL)
			m_szCommand=alloccopy(m_szCommand);
		break;
	case CAction::Presets:
	case CAction::ChangeValue:
		if (m_szPreset!=NULL)
			m_szPreset=alloccopy(m_szPreset);
		break;
	}
	
}

void CSubAction::ClearExtraInfo(DWORD nAction)
{
	switch (nAction)
	{
	case CAction::ResultListItems:
		if (m_nResultList==Execute && m_pExtraInfo!=NULL)
			delete[] m_szVerb;
		else if (m_nResultList==ExecuteCommand && m_szCommand!=NULL)
			delete[] m_szCommand;			
		break;
	case CAction::Misc:
		if ((m_nMisc==SendMessage || m_nMisc==PostMessage ) &&
			m_pSendMessage!=NULL)
			delete m_pSendMessage;
		else if (m_nMisc==ExecuteCommandMisc && m_szCommand!=NULL)
			delete[] m_szCommand;			
		break;
	case CAction::Presets:
		if (m_szPreset!=NULL)
			delete[] m_szPreset;			
		break;
	case CAction::ChangeValue:
		if (m_szValue!=NULL)
			delete[] m_szValue;			
		break;
	}
	m_pExtraInfo=NULL;
}

BOOL CAction::ExecuteAction()
{
	DebugFormatMessage("CAction::ExecuteAction() this=%X",DWORD(this));
	DebugFormatMessage("m_nAction=%d",m_nAction);
	switch (m_nAction)
	{
	case None:
		return TRUE;
	case ActivateControl:
		return DoActivateControl();
	case ActivateTab:
		return DoActivateTab();
	case MenuCommand:
		return DoMenuCommand();
	case ShowHideDialog:
		return DoShowHideDialog();
	case ResultListItems:
		return DoResultListItems();
	case Misc:
		return DoMisc();
	case ChangeValue:
		return DoChangeValue();
	case Presets:
		return DoPresets();
	case Help:
		return DoHelp();
	case Settings:
		return DoSettings();
	default:
		ASSERT(0);
		return FALSE;
	}
}

BOOL CSubAction::DoActivateControl()
{
	CLocateDlg* pLocateDlg=GetLocateDlg();
	if (pLocateDlg==NULL)
		return FALSE;
	DebugFormatMessage("DoActivateControl: activating control %d",m_nControl);

	if (GetCurrentThreadId()==GetTrayIconWnd()->GetLocateDlgThread()->GetThreadId())
	{
		int nRet=(int)pLocateDlg->OnCommand(LOWORD(m_nControl),1,NULL);
		DebugFormatMessage("DoActivateControl: OnCommand returned %d",nRet);
		return !nRet;
	}
	else
	{
		int nRet=(int)pLocateDlg->SendMessage(WM_COMMAND,MAKEWPARAM(LOWORD(m_nControl),1),0);
		DebugFormatMessage("DoActivateControl: WM_COMMAND returned %d",nRet);
		return !nRet;
	}
}


BOOL CSubAction::DoMenuCommand()
{
	CLocateDlg* pLocateDlg=GetLocateDlg();
	if (pLocateDlg==NULL)
		return FALSE;

	if (GetCurrentThreadId()==GetTrayIconWnd()->GetLocateDlgThread()->GetThreadId())
		pLocateDlg->OnCommand(LOWORD(m_nMenuCommand),0,NULL);
	else
		pLocateDlg->SendMessage(WM_COMMAND,MAKEWPARAM(LOWORD(m_nMenuCommand),0),0);
	return TRUE;
}

BOOL CSubAction::DoShowHideDialog()
{
	CLocateDlg* pLocateDlg=GetLocateDlg();
	
	switch(m_nDialogCommand)
	{
	case ShowDialog:
		GetTrayIconWnd()->OnLocate();
		break;
	case MinimizeDialog:
		if (pLocateDlg!=NULL)
			pLocateDlg->ShowWindow(CWnd::swMinimize);
		break;
	case CloseDialog:
		if (pLocateDlg!=NULL)
			pLocateDlg->PostMessage(WM_CLOSE);
		break;
	case ShowOrHideDialog:
		if (pLocateDlg!=NULL)
		{
			WINDOWPLACEMENT wp;
			wp.length=sizeof(WINDOWPLACEMENT);
			pLocateDlg->GetWindowPlacement(&wp);
			if (wp.showCmd!=SW_SHOWMINIMIZED &&wp.showCmd!=SW_HIDE)
				pLocateDlg->ShowWindow(CWnd::swMinimize);
			else 
				GetTrayIconWnd()->OnLocate();
		}
		break;
	case OpenOrCloseDialog:
		if (pLocateDlg!=NULL)
		{
			WINDOWPLACEMENT wp;
			wp.length=sizeof(WINDOWPLACEMENT);
			pLocateDlg->GetWindowPlacement(&wp);
			if (wp.showCmd!=SW_SHOWMINIMIZED && wp.showCmd!=SW_HIDE)
				pLocateDlg->PostMessage(WM_CLOSE);
			else
				GetTrayIconWnd()->OnLocate();
		}
		else
			GetTrayIconWnd()->OnLocate();
		break;
	case RestoreDialog:
		if (pLocateDlg!=NULL)
			pLocateDlg->ShowWindow(CWnd::swRestore);
		break;
	case MaximizeDialog:
		if (pLocateDlg!=NULL)
			pLocateDlg->ShowWindow(CWnd::swMaximize);
		break;
	case MaximizeOrRestoreDialog:
		if (pLocateDlg!=NULL)
		{
			WINDOWPLACEMENT wp;
			wp.length=sizeof(WINDOWPLACEMENT);
			pLocateDlg->GetWindowPlacement(&wp);
			if (wp.showCmd!=SW_SHOWMAXIMIZED)
				pLocateDlg->ShowWindow(CWnd::swMaximize);
			else
				pLocateDlg->ShowWindow(CWnd::swRestore);
		}
		break;        
	case ShowOpenOrHideDialog:  
		if (pLocateDlg==NULL)
			GetTrayIconWnd()->OnLocate();
		else if (pLocateDlg!=NULL)
		{
			WINDOWPLACEMENT wp;
			wp.length=sizeof(WINDOWPLACEMENT);
			pLocateDlg->GetWindowPlacement(&wp);
			if (wp.showCmd!=SW_SHOWMINIMIZED &&wp.showCmd!=SW_HIDE)
				pLocateDlg->ShowWindow(CWnd::swMinimize);
			else 
				GetTrayIconWnd()->OnLocate();
		}
		break;
	case ShowDialogAndGetDirFromExplorer:
		{
			LPWSTR pPath=GetPathFromExplorer();
			GetTrayIconWnd()->OnLocate();

			if (pPath!=NULL)
			{
				CLocateDlg* pLocateDlg=GetLocateDlg();
				if (pLocateDlg!=NULL)
					pLocateDlg->SetPath(pPath);
				delete[] pPath;
			}
			break;
		}
		break;
	case StopLocatingOrCloseWindow:
		if (pLocateDlg!=NULL)
		{
			if (pLocateDlg->IsLocating())
				pLocateDlg->OnStop();
			else
				pLocateDlg->PostMessage(WM_CLOSE);
		}
		break;
	}

	return TRUE;
}


BOOL CSubAction::DoHelp()
{
	CTargetWnd* pWnd=GetLocateDlg();
	if (pWnd==NULL)
		pWnd=GetTrayIconWnd();
	
	switch(m_nHelp)
	{
	case HelpShowHelp:
		{
			HELPINFO h;
			h.cbSize=sizeof(HELPINFO);
			h.iContextType=HELPINFO_WINDOW;
			h.hItemHandle=GetFocus();
			
			if (pWnd==GetLocateDlg())
			{
				// If comboboxes are selected, the edit in size of combo
				// may have focus. 
				HWND hParent=GetParent((HWND)h.hItemHandle);
				while (hParent!=NULL && hParent!=*pWnd &&
					hParent!=((CLocateDlg*)pWnd)->m_NameDlg &&
					hParent!=((CLocateDlg*)pWnd)->m_SizeDateDlg &&
					hParent!=((CLocateDlg*)pWnd)->m_AdvancedDlg)
				{
					h.hItemHandle=hParent;
					hParent=GetParent(hParent);
				}
			}


			h.iCtrlId=GetDlgCtrlID((HWND)h.hItemHandle);
			h.dwContextId=0;
			GetCursorPos(&h.MousePos);
			pWnd->OnHelp(&h);
			return TRUE;
		}
	case HelpCloseHelp:
		pWnd->HtmlHelp(HH_CLOSE_ALL,0);
		return TRUE;
	case HelpShowTopics:
		return pWnd->HtmlHelp(HH_DISPLAY_TOPIC,0)!=NULL;
	case HelpIndex:
		return pWnd->HtmlHelp(HH_DISPLAY_INDEX,0)!=NULL;
	case HelpTOC:
		return pWnd->HtmlHelp(HH_DISPLAY_TOC,0)!=NULL;
	case HelpSearch:
		{
			HH_FTS_QUERY q;
			ZeroMemory(&q,sizeof(HH_FTS_QUERY));
			q.cbStruct=sizeof(HH_FTS_QUERY);

			return pWnd->HtmlHelp(HH_DISPLAY_SEARCH,(DWORD_PTR)&q)!=NULL;
		}
	};
	return FALSE;

}

BOOL CSubAction::DoSettings()
{
	CTrayIconWnd* pWnd=GetTrayIconWnd();
	if (pWnd!=NULL)
		return pWnd->OnSettings(m_nSettings);
	return FALSE;
}

LPWSTR CSubAction::GetPathFromExplorer()
{
	if (!IsUnicodeSystem())
	{
		// Global shortcuts won't even work in non-unicode systems
		return NULL;
	}

	HWND hCurWindow=GetForegroundWindow();
	char szClass[100]="";
	GetClassName(hCurWindow,szClass,100);
	
	if (strcmp(szClass,"ExploreWClass")!=0  && // WinXP
		strcmp(szClass,"CabinetWClass")!=0)  // WinNT
		return NULL;
	
	WCHAR* pPath=new WCHAR[MAX_PATH];
	pPath[0]='\0';
	EnumChildWindows(hCurWindow,EnumExplorerChilds,(LPARAM)pPath);

	if (pPath[0]=='\0')
	{
		delete[] pPath;
		return NULL;
	}
	return pPath;
}

BOOL CALLBACK CSubAction::EnumExplorerChilds(HWND hWnd,LPARAM lParam)
{
	char szClass[100]="";
	GetClassName(hWnd,szClass,100);

	if (strcmp(szClass,"ComboBoxEx32")!=0)
		return TRUE;

	if (::SendMessageW(hWnd,WM_GETTEXT,MAX_PATH,lParam)==0)
		return TRUE;
	
	if (FileSystem::IsDirectory((LPCWSTR)lParam))
		return FALSE; // Path is OK

	// Check wheter lParam is Desktop
	LPITEMIDLIST idl;
	SHGetSpecialFolderLocation(NULL,CSIDL_DESKTOP,&idl);
	SHFILEINFOW fi;
	if (ShellFunctions::GetFileInfo(idl,0,&fi,SHGFI_DISPLAYNAME))
	{
		if (wcscmp(fi.szDisplayName,(LPCWSTR)lParam)==0)
		{
			((LPWSTR)lParam)[0]=L'2';
			((LPWSTR)lParam)[1]=L'\0';
			CoTaskMemFree(idl);
			return FALSE;
		}
	
		CoTaskMemFree(idl);		
	}

	// Check wheter lParam is My Computer
	SHGetSpecialFolderLocation(NULL,CSIDL_DRIVES,&idl);
	if (ShellFunctions::GetFileInfo(idl,0,&fi,SHGFI_DISPLAYNAME))
	{
		if (wcscmp(fi.szDisplayName,(LPCWSTR)lParam)==0)
		{
			((LPWSTR)lParam)[0]=L'4';
			((LPWSTR)lParam)[1]=L'\0';
			CoTaskMemFree(idl);
			return FALSE;
		}
	
		CoTaskMemFree(idl);		
	}

	// Check wheter lParam is My Documents
	CRegKey RegKey;
	if (RegKey.OpenKey(HKCU,"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
	{
		WCHAR temp[MAX_PATH];
		RegKey.QueryValue(L"Personal",temp,MAX_PATH);
		RegKey.CloseKey();

		if	(FileSystem::IsDirectory(temp))
		{
			if (ShellFunctions::GetFileInfo(temp,0,&fi,SHGFI_DISPLAYNAME))
			{
				if (wcscmp(fi.szDisplayName,(LPCWSTR)lParam)==0)
				{
					((LPWSTR)lParam)[0]=L'3';
					((LPWSTR)lParam)[1]=L'\0';
			
					return FALSE;
				}
			}
		}
	}

	((LPWSTR)lParam)[0]='\0';
	return TRUE;
}

BOOL CSubAction::DoChangeValue()
{
	CLocateDlg* pLocateDlg=GetLocateDlg();
	if (pLocateDlg==NULL || m_szValue==NULL)
		return FALSE;
	
	// Get handle to control
	CWnd Control(pLocateDlg->GetDlgItem(LOWORD(m_nControl)));
	HWND hInDialog=*pLocateDlg;

	if (Control.GetHandle()==NULL)
		Control.AttachToDlgItem(pLocateDlg->m_NameDlg,LOWORD(m_nControl));
	if (Control.GetHandle()==NULL)
		Control.AttachToDlgItem(pLocateDlg->m_SizeDateDlg,LOWORD(m_nControl));
	if (Control.GetHandle()==NULL)
		Control.AttachToDlgItem(pLocateDlg->m_AdvancedDlg,LOWORD(m_nControl));

	ASSERT(Control.GetHandle()!=NULL);

	if (!Control.IsWindowEnabled())
		return TRUE;

	char szClass[100];
	GetClassName(Control,szClass,100);
	if (_stricmp(szClass,"EDIT")==0)
	{
		Control.SetWindowText(m_szValue);
		::SendMessage(Control.GetParent(),WM_COMMAND,MAKEWPARAM(m_nControl,EN_CHANGE),LPARAM((HWND)Control));
	}
	else if (_stricmp(szClass,"COMBOBOX")==0)
	{
		if ((Control.GetStyle()&CBS_DROPDOWNLIST)!=CBS_DROPDOWNLIST)
		{
			Control.SetWindowText(m_szValue);
			::SendMessage(Control.GetParent(),WM_COMMAND,MAKEWPARAM(m_nControl,CBN_EDITCHANGE),LPARAM((HWND)Control));
		}
		else
		{
			Control.SendMessage(CB_SETCURSEL,_wtoi(m_szValue));
			::SendMessage(Control.GetParent(),WM_COMMAND,MAKEWPARAM(m_nControl,CBN_SELCHANGE),LPARAM((HWND)Control));
		}
	}
	else if (_stricmp(szClass,"BUTTON")==0)
	{
		if (Control.GetStyle()&BS_CHECKBOX)  // This also takes radiobuttons
		{
			Control.SendMessage(BM_SETCHECK,_wtoi(m_szValue));
			::SendMessage(Control.GetParent(),WM_COMMAND,MAKEWPARAM(m_nControl,BN_CLICKED),LPARAM((HWND)Control));
		}
	}
	else if (_stricmp(szClass,"ComboBoxEx32")==0)
	{
		CComboBoxEx cb(Control);
		cb.SetCurSel(-1);
		cb.SetItemText(-1,m_szValue);
	}
	else if (_stricmp(szClass,"SysDateTimePick32")==0)
	{
		SYSTEMTIME st;
		GetLocalTime(&st);
		st.wHour=0;
		st.wMinute=0;
		st.wSecond=0;
		st.wMilliseconds=0;
		
		if (wcslen(m_szValue)>=8)
		{
			
		
			WCHAR szTemp[5];
			MemCopyW(szTemp,m_szValue,4);
			szTemp[4]='\0';
			st.wYear=(WORD)_wtol(szTemp);

			MemCopyW(szTemp,m_szValue+4,2);
			szTemp[2]='\0';
			st.wMonth=(WORD)_wtol(szTemp);

			MemCopyW(szTemp,m_szValue+6,2);
			szTemp[2]='\0';
			st.wDay=(WORD)_wtol(szTemp);

		}
		else
		{
			int nIndex=CTime::GetIndex(st.wDay,st.wMonth,st.wYear);
			if (m_szValue[0]='-')
				nIndex-=(WORD)_wtol(m_szValue+1);
			else 
				nIndex+=(WORD)_wtol(m_szValue);

			CTime::GetDayFromIndex(nIndex,st.wDay,st.wMonth,st.wYear);
		}
		Control.SendMessage(DTM_SETSYSTEMTIME,GDT_VALID,LPARAM(&st));
	}

	return TRUE;
}


BOOL CSubAction::DoPresets()
{
	CLocateDlg* pLocateDlg=GetLocateDlg();
	if (pLocateDlg!=NULL && m_szPreset!=NULL)
		return pLocateDlg->LoadPreset(m_szPreset);
	return FALSE;
}


BOOL CSubAction::DoResultListItems()
{
	DebugFormatMessage("CSubAction::DoResultListItems() this=%X",DWORD(this));
	DebugFormatMessage("m_nResultsList=%d",m_nResultList);
	if (m_nResultList==ExecuteCommand)
	{
		if (m_szCommand!=NULL)
			return CLocateDlg::ExecuteCommand(m_szCommand);
		return TRUE;
	}
	
	CLocateDlg* pLocateDlg=GetLocateDlg();
	if (pLocateDlg==NULL)
		return FALSE;

	DebugFormatMessage("Sending message, pExtraInfo=%X",DWORD(m_pExtraInfo));
	return (int)pLocateDlg->SendMessage(WM_RESULTLISTACTION,m_nSubAction,(LPARAM)m_pExtraInfo);
}

void * __cdecl gmalloc(size_t size) { return GlobalAlloc(GPTR,size+1); }
BOOL CALLBACK WindowEnumProc(HWND hwnd,LPARAM lParam)
{
	CWnd wnd(hwnd);
		
	if  (((LPWSTR*)lParam)[0]!=NULL)
	{
		// Class is specified
		WCHAR szClass[200];
		if (wnd.GetClassName(szClass,200)>0)
		{
			if (!__ContainString(szClass,((LPWSTR*)lParam)[0]))
				return TRUE;
		}
		else if (((LPWSTR*)lParam)[0][0]!='*' || ((LPWSTR*)lParam)[0][1]!='\0')
			return TRUE;
	}

	if  (((LPWSTR*)lParam)[1]!=NULL)
	{
		// Title is specified
		WCHAR szTitle[400];
		if (wnd.GetWindowText(szTitle,400)>0)
		{
			if (!__ContainString(szTitle,((LPWSTR*)lParam)[1]))
				return TRUE;
		}
		else if (((LPSTR*)lParam)[1][0]!='*' || ((LPWSTR*)lParam)[1][1]!='\0')
			return TRUE;
	}


	((HWND*)lParam)[2]=hwnd;
	return FALSE;
}

BOOL CSubAction::DoMisc()
{
	if (m_nMisc==ExecuteCommandMisc)
		return CLocateDlg::ExecuteCommand(m_szCommand);
	else if (m_nMisc==InsertAsterisks)
	{
		CLocateDlg* pLocateDlg=GetLocateDlg();
		if (pLocateDlg==NULL)
			return FALSE;

		CStringW Text;
		pLocateDlg->m_NameDlg.m_Name.GetText(Text);
		DWORD dwSelStart=pLocateDlg->m_NameDlg.m_Name.GetEditSel();
		WORD wSelEnd=HIWORD(dwSelStart);
		dwSelStart&=0xFFFF;

		// If asterisks are already at the beginning and the end, replace spaces
		if (Text[0]==L'*' && Text.LastChar()==L'*')
			Text.ReplaceChars(L' ',L'*');
		else 
		{
			if (Text[0]!=L'*')
			{
				Text.InsChar(0,L'*');
				
				// Update selection
				if (dwSelStart==wSelEnd)
				{
					dwSelStart++;
					wSelEnd++;
				}
				else 
				{
					if (dwSelStart>0)
						dwSelStart++;
					wSelEnd++;
				}
			}
			
			if (Text.LastChar()!=L'*')
			{
				// Update selection first
				if (wSelEnd==Text.GetLength())
				{
					if (dwSelStart==wSelEnd)
						dwSelStart++;
					wSelEnd++; 
				}

				Text.Append(L'*');
			}
		}

		pLocateDlg->m_NameDlg.m_Name.SetText(Text);
		pLocateDlg->m_NameDlg.m_Name.SetEditSel(dwSelStart,wSelEnd);
		pLocateDlg->OnFieldChange(CLocateDlg::isNameChanged);
		return TRUE;
	}
	

	// Send/Post Message

	BOOL bFreeWParam=FALSE,bFreeLParam=FALSE;
	
	HWND hWnd=NULL;
	WPARAM wParam=NULL,lParam=NULL;

	if (m_pSendMessage->szWindow[0]=='0')
	{
		if (m_pSendMessage->szWindow[1]=='x' || 
			m_pSendMessage->szWindow[1]=='X')
		{
			// Hex value
			LPWSTR szTemp;
			hWnd=(HWND)wcstoul(m_pSendMessage->szWindow+2,&szTemp,16);
		}
	}
	else if (strcasecmp(m_pSendMessage->szWindow,L"HWND_BROADCAST")==0)
		hWnd=HWND_BROADCAST;
	else if (GetLocateDlg()!=NULL && strcasecmp(m_pSendMessage->szWindow,L"LOCATEDLG")==0)
		hWnd=*GetLocateDlg();
	else if (strcasecmp(m_pSendMessage->szWindow,L"LOCATEST")==0)
		hWnd=*GetTrayIconWnd();
	else if (wcsncmp(m_pSendMessage->szWindow,L"Find",4)==0)
	{
		int nIndex=(int)FirstCharIndex(m_pSendMessage->szWindow,L'(');
		if (nIndex!=-1)
		{
			LPCWSTR pText=m_pSendMessage->szWindow+nIndex+1;
			LPWSTR pClassAndWindow[3]={NULL,NULL,NULL};
			
			nIndex=(int)FirstCharIndex(pText,L',');
			if (nIndex==-1)
			{
				nIndex=(int)FirstCharIndex(pText,L')');
				if (nIndex==-1)
					pClassAndWindow[0]=alloccopy(pText);
				else
					pClassAndWindow[0]=alloccopy(pText,nIndex);
			}
			else
			{
				pClassAndWindow[0]=alloccopy(pText,nIndex);
				pText+=nIndex+1;

				nIndex=(int)FirstCharIndex(pText,L')');
				pClassAndWindow[1]=alloccopy(pText,nIndex);
			}

			EnumWindows(WindowEnumProc,LPARAM(pClassAndWindow));

			// Third cell is handle to window
			hWnd=(HWND)pClassAndWindow[2];

			delete[] pClassAndWindow[0];
			if (pClassAndWindow[1])
				delete[] pClassAndWindow[1];
		}
	}
	


	// Parse wParam
	if (m_pSendMessage->szWParam!=NULL)
	{
		if (m_pSendMessage->szWParam[0]=='0')
		{
			if (m_pSendMessage->szWParam[1]=='x' || 
				m_pSendMessage->szWParam[1]=='X')
			{
				// Hex value
				LPWSTR szTemp;
				wParam=(WPARAM)wcstoul(m_pSendMessage->szWParam+2,&szTemp,16);
			}
			else if (m_pSendMessage->szWParam[1]!='\0')
			{
				DWORD dwLength;
				wParam=(WPARAM)dataparser(m_pSendMessage->szWParam,istrlen(m_pSendMessage->szWParam),gmalloc,&dwLength);
				*((BYTE*)wParam+dwLength)=0;
				bFreeWParam=TRUE;
			}
		}
		else if ((wParam=_wtoi(m_pSendMessage->szWParam))==0)
		{
			DWORD dwLength;
			wParam=(WPARAM)dataparser(m_pSendMessage->szWParam,istrlen(m_pSendMessage->szWParam),gmalloc,&dwLength);
			*((BYTE*)wParam+dwLength)=0;
			bFreeWParam=TRUE;
		}
	}

	// Parse lParam
	if (m_pSendMessage->szLParam!=NULL)
	{
		if (m_pSendMessage->szLParam[0]=='0')
		{
			if (m_pSendMessage->szLParam[1]=='x' || 
				m_pSendMessage->szLParam[1]=='X')
			{
				// Hex value
				LPWSTR szTemp;
				lParam=(WPARAM)wcstoul(m_pSendMessage->szLParam+2,&szTemp,16);
			}
			else if (m_pSendMessage->szLParam[1]!='\0')
			{
				DWORD dwLength;
				lParam=(WPARAM)dataparser(m_pSendMessage->szLParam,istrlen(m_pSendMessage->szLParam),gmalloc,&dwLength);
				*((BYTE*)lParam+dwLength)=0;
				bFreeLParam=TRUE;
			}
		}
		else if ((lParam=_wtoi(m_pSendMessage->szLParam))==0)
		{
			DWORD dwLength;
			lParam=(WPARAM)dataparser(m_pSendMessage->szLParam,istrlen(m_pSendMessage->szLParam),gmalloc,&dwLength);
			*((BYTE*)lParam+dwLength)=0;
            bFreeLParam=TRUE;
		}
	}

	if (hWnd!=NULL)
	{
		if (m_nMisc==PostMessage)
			::PostMessage(hWnd,m_pSendMessage->nMessage,wParam,lParam);
		else
			::SendMessage(hWnd,m_pSendMessage->nMessage,wParam,lParam);
	}

	if (bFreeWParam)
		GlobalFree((HANDLE)wParam);
	if (bFreeLParam)
		GlobalFree((HANDLE)lParam);



	return TRUE;
}


CSubAction::SendMessageInfo::SendMessageInfo(SendMessageInfo& rCopyFrom)
{
	CopyMemory(this,&rCopyFrom,sizeof(SendMessageInfo));

	if (szWindow!=NULL)
		szWindow=alloccopy(szWindow);
	if (szWParam!=NULL)
		szWParam=alloccopy(szWParam);
	if (szLParam!=NULL)
		szLParam=alloccopy(szLParam);
}
		
DWORD CSubAction::SendMessageInfo::GetData(BYTE* pData_) const
{
	BYTE* pData=pData_;
	
	*((WORD*)pData)=0xFFEB;
	pData+=sizeof(WORD);
    *((DWORD*)pData)=nMessage;
	pData+=sizeof(DWORD);

	if (szWindow!=NULL)
	{
        DWORD dwUsed=(istrlen(szWindow)+1)*2;
		CopyMemory(pData,szWindow,dwUsed);
		pData+=dwUsed;
	}
	else
	{
		*(pData++)='\0';
		*(pData++)='\0';
	}
		
	
	if (szWParam!=NULL)
	{
        DWORD dwUsed=(istrlen(szWParam)+1)*2;
		CopyMemory(pData,szWParam,dwUsed);
		pData+=dwUsed;
	}
	else
	{
		*(pData++)='\0';
		*(pData++)='\0';
	}

	
	if (szLParam!=NULL)
	{
        DWORD dwUsed=(istrlen(szLParam)+1)*2;
		CopyMemory(pData,szLParam,dwUsed);
		pData+=dwUsed;
	}
	else
	{
		*(pData++)='\0';
		*(pData++)='\0';
	}	

	return DWORD(pData-pData_);
}

void CShortcut::ModifyMenus(CArrayFP<CShortcut*>& aShortcuts,HMENU hMainMenu,HMENU hSubMenu)
{
	VirtualKeyName* pVirtKeyNames=GetVirtualKeyNames();

	for (int i=0;i<aShortcuts.GetSize();i++)
	{
		// Check whether it is menu item
		for (int a=0;a<aShortcuts[i]->m_apActions.GetSize();a++)
		{
			CAction* pAction=aShortcuts[i]->m_apActions[a];

			if (pAction->m_nAction==CAction::MenuCommand)
			{
				BYTE bSubMenu=CAction::GetMenuAndSubMenu(pAction->m_nMenuCommand);
				CMenu Menu(GetSubMenu((bSubMenu&128)?hMainMenu:hSubMenu,bSubMenu&~128));

				MENUITEMINFOW mii;
				mii.cbSize=sizeof(MENUITEMINFOW);
				mii.fMask=MIIM_FTYPE|MIIM_STRING;
				mii.cch=0;
				if (!Menu.GetMenuItemInfo(LOWORD(pAction->m_nMenuCommand),FALSE,&mii))
					continue;

				if (mii.fType!=MFT_STRING)
					continue;

				CStringW Title,Key;
				mii.dwTypeData=Title.GetBuffer(mii.cch++);
				if (!Menu.GetMenuItemInfo(LOWORD(pAction->m_nMenuCommand),FALSE,&mii))
					continue;

				if (Title.FindFirst(L'\t')!=-1)
					continue;

				aShortcuts[i]->FormatKeyLabel(pVirtKeyNames,Key);
								
				Title << L'\t' << Key;
				mii.dwTypeData=Title.GetBuffer();
				
				Menu.SetMenuItemInfo(LOWORD(pAction->m_nMenuCommand),FALSE,&mii);


			}

		}
	}

	for (int i=0;pVirtKeyNames[i].bKey!=0;i++)
		delete[] pVirtKeyNames[i].pName;
	delete[] pVirtKeyNames;
}



BYTE CSubAction::GetMenuAndSubMenu(CAction::ActionMenuCommands nMenuCommand)
{
	switch (HIWORD(nMenuCommand))
	{
	case IDS_SHORTCUTMENUFILENOITEM:
		return SUBMENU_FILEMENUNOITEMS;
	case IDS_SHORTCUTMENUFILEITEM:
		if (nMenuCommand==CAction::FileOpenContainingFolder ||
			nMenuCommand==CAction::FileRemoveFromThisList)
			return SUBMENU_EXTRACONTEXTMENUITEMS;
		else
			return SUBMENU_FILEMENU;
	case IDS_SHORTCUTMENUEDIT:
		return 128|1;
		break;
	case IDS_SHORTCUTMENUVIEW:
	case IDS_SHORTCUTMENUVIEWARRANGEICONS:
		return 128|2;
	case IDS_SHORTCUTMENUTOOLS:
		return 128|3;		
	case IDS_SHORTCUTMENUHELP:
		return 128|4;
	case IDS_SHORTCUTMENUSPECIAL:
		return SUBMENU_EXTRACONTEXTMENUITEMS;
	case IDS_SHORTCUTMENUPRESETS:
		return SUBMENU_PRESETSELECTION;
	case IDS_SHORTCUTMENUDIRECTORIES:
		return SUBMENU_MULTIDIRSELECTION;
	}

	return 0;
}

void CShortcut::FormatKeyLabel(VirtualKeyName* pVirtualKeyNames,BYTE bKey,BYTE bModifiers,BOOL bScancode,CStringW& str)
{
	if (bKey==0)
	{
		str.LoadString(IDS_SHORTCUTNONE);
		return;
	}


	// Formatting modifiers
	if (bModifiers&MOD_WIN)
		str.LoadString(IDS_SHORTCUTMODEXT);
	else
		str.Empty();
	if (bModifiers&MOD_CONTROL)
		str.AddString(IDS_SHORTCUTMODCTRL);
	if (bModifiers&MOD_ALT)
		str.AddString(IDS_SHORTCUTMODALT);
	if (bModifiers&MOD_SHIFT)
		str.AddString(IDS_SHORTCUTMODSHIFT);
	
	if (bScancode)
	{
		CStringW str2;
		str2.Format(IDS_SHORTCUTSCANCODE,(int)bKey);
		str << str2;
		return;
	}

	int i;
	for (i=0;pVirtualKeyNames[i].bKey!=0 && pVirtualKeyNames[i].bKey!=bKey;i++);
	if (pVirtualKeyNames[i].iFriendlyNameId!=0)
	{
		str.AddString(pVirtualKeyNames[i].iFriendlyNameId);
		return;
	}

	BYTE pKeyState[256];
	ZeroMemory(pKeyState,256);

	WORD wChar;
	int nRet=ToAscii(bKey,0,pKeyState,&wChar,0);
	if (nRet==1)
	{
		MakeUpper((LPSTR)&wChar,1);
		str << char(wChar);
	} 
	else if (nRet==2)
	{
		MakeUpper((LPSTR)&wChar,2);
		str << (LPSTR(&wChar))[0] << (LPSTR(&wChar))[0];
	}
	else if (pVirtualKeyNames[i].pName!=NULL)
		str << pVirtualKeyNames[i].pName;
	else
		str << (int) bKey;
}


CShortcut::VirtualKeyName* CShortcut::GetVirtualKeyNames()
{
	VirtualKeyName aVirtualKeys[]={
		{VK_BACK,"VK_BACK",IDS_KEYBACKSPACE},
		{VK_ESCAPE,"VK_ESCAPE",IDS_KEYESC},
		{VK_TAB,"VK_TAB",IDS_KEYTAB},
		{VK_CAPITAL,"VK_CAPITAL",IDS_KEYCAPSLOCK},
		{VK_RETURN,"VK_RETURN",IDS_KEYENTER},
		{VK_SPACE,"VK_SPACE",IDS_KEYSPACE},
		{VK_PRIOR,"VK_PRIOR",IDS_KEYPAGEUP},
		{VK_NEXT,"VK_NEXT",IDS_KEYPAGEDOWN},
		{VK_END,"VK_END",IDS_KEYEND},
		{VK_HOME,"VK_HOME",IDS_KEYHOME},
		{VK_LEFT,"VK_LEFT",IDS_KEYLEFT},
		{VK_UP,"VK_UP",IDS_KEYUP},
		{VK_RIGHT,"VK_RIGHT",IDS_KEYRIGHT},
		{VK_DOWN,"VK_DOWN",IDS_KEYDOWN},
		{VK_SNAPSHOT,"VK_SNAPSHOT",IDS_KEYPRINTSCREEN},
		{VK_SCROLL,"VK_SCROLL",IDS_KEYSCROLLLOCK},
		{VK_PAUSE,"VK_PAUSE",IDS_KEYPAUSE},
		{VK_INSERT,"VK_INSERT",IDS_KEYINS},
		{VK_DELETE,"VK_DELETE",IDS_KEYDEL},
		{VK_NUMLOCK,"VK_NUMLOCK",IDS_KEYNUMLOCK},
		{VK_NUMPAD0,"VK_NUMPAD0",IDS_KEYNUM0},
		{VK_NUMPAD1,"VK_NUMPAD1",IDS_KEYNUM1},
		{VK_NUMPAD2,"VK_NUMPAD2",IDS_KEYNUM2},
		{VK_NUMPAD3,"VK_NUMPAD3",IDS_KEYNUM3},
		{VK_NUMPAD4,"VK_NUMPAD4",IDS_KEYNUM4},
		{VK_NUMPAD5,"VK_NUMPAD5",IDS_KEYNUM5},
		{VK_NUMPAD6,"VK_NUMPAD6",IDS_KEYNUM6},
		{VK_NUMPAD7,"VK_NUMPAD7",IDS_KEYNUM7},
		{VK_NUMPAD8,"VK_NUMPAD8",IDS_KEYNUM8},
		{VK_NUMPAD9,"VK_NUMPAD9",IDS_KEYNUM9},
		{VK_MULTIPLY,"VK_MULTIPLY",IDS_KEYNUMMUL},
		{VK_ADD,"VK_ADD",IDS_KEYNUMADD},
		{VK_SEPARATOR,"VK_SEPARATOR",0},
		{VK_SUBTRACT,"VK_SUBTRACT",IDS_KEYNUMSUB},
		{VK_DECIMAL,"VK_DECIMAL",IDS_KEYNUMDECIMAL},
		{VK_DIVIDE,"VK_DIVIDE",IDS_KEYNUMDIV},
		{VK_F1,"VK_F1",IDS_KEYF1},
		{VK_F2,"VK_F2",IDS_KEYF2},
		{VK_F3,"VK_F3",IDS_KEYF3},
		{VK_F4,"VK_F4",IDS_KEYF4},
		{VK_F5,"VK_F5",IDS_KEYF5},
		{VK_F6,"VK_F6",IDS_KEYF6},
		{VK_F7,"VK_F7",IDS_KEYF7},
		{VK_F8,"VK_F8",IDS_KEYF8},
		{VK_F9,"VK_F9",IDS_KEYF9},
		{VK_F10,"VK_F10",IDS_KEYF10},
		{VK_F11,"VK_F11",IDS_KEYF11},
		{VK_F12,"VK_F12",IDS_KEYF12},
		{VK_F13,"VK_F13",IDS_KEYF13},
		{VK_F14,"VK_F14",IDS_KEYF14},
		{VK_F15,"VK_F15",0},
		{VK_F16,"VK_F16",0},
		{VK_F17,"VK_F17",0},
		{VK_F18,"VK_F18",0},
		{VK_F19,"VK_F19",0},
		{VK_F20,"VK_F20",0},
		{VK_F21,"VK_F21",0},
		{VK_F22,"VK_F22",0},
		{VK_F23,"VK_F23",0},
		{VK_F24,"VK_F24",0}
	};

	VirtualKeyName* pRet=new VirtualKeyName[sizeof(aVirtualKeys)/sizeof(VirtualKeyName)+1];

	int i;
	for (i=0;i<sizeof(aVirtualKeys)/sizeof(VirtualKeyName);i++)
	{
		pRet[i].bKey=aVirtualKeys[i].bKey;
		pRet[i].pName=alloccopy(aVirtualKeys[i].pName);
		pRet[i].iFriendlyNameId=aVirtualKeys[i].iFriendlyNameId;
	}
	pRet[i].bKey=0;
	pRet[i].pName=NULL;
	pRet[i].iFriendlyNameId=0;
    return pRet;
}

int CSubAction::GetActivateTabsActionLabelStringId(CAction::ActionActivateTabs uSubAction)
{
	switch (uSubAction)
	{
	case NameAndLocationTab:
		return IDS_NAME;
	case SizeAndDataTab:
		return IDS_SIZEDATE;
	case AdvancedTab:
		return IDS_ADVANCED;
	case NextTab:
		return IDS_KEYNEXTTAB;
	case PrevTab:
		return IDS_KEYPREVTAB;
	default:
		return 0;
	}
}
	
int CSubAction::GetShowHideDialogActionLabelStringId(CAction::ActionShowHideDialog uSubAction)
{
	switch (uSubAction)
	{
	case ShowDialog:
		return IDS_ACTIONSWDIALOGSHOW;
	case MinimizeDialog:
		return IDS_ACTIONSWDIALOGMINIMINZE;
	case CloseDialog:
		return IDS_ACTIONSWDIALOGCLOSE;
	case ShowOrHideDialog:
		return IDS_ACTIONSWDIALOGSHOWORHIDE;
	case OpenOrCloseDialog:
		return IDS_ACTIONSWDIALOGOPENORCLOSE;
	case RestoreDialog:
		return IDS_ACTIONSWDIALOGRESTORE;
	case MaximizeDialog:
		return IDS_ACTIONSWDIALOGMAXIMIZE;
	case MaximizeOrRestoreDialog:
		return IDS_ACTIONSWDIALOGMAXIMIZEORRESTORE;
	case ShowOpenOrHideDialog:
		return IDS_ACTIONSWDIALOGSHOWOPENORHIDE;
	case ShowDialogAndGetDirFromExplorer:
		return IDS_ACTIONSWDIALOGSHOWANDDIRFROMEXPLORER;
	case StopLocatingOrCloseWindow:
		return IDS_ACTIONSWDIALOGSSTOPLOCATINGORCLOSEDIALOG;
	default:
		return 0;
	}
}

int CSubAction::GetResultItemActionLabelStringId(CAction::ActionResultList uSubAction)
{
	switch (uSubAction)
	{
	case Execute:
		return IDS_ACTIONRESITEMEXECUTE;
	case Copy:
		return IDS_ACTIONRESITEMCOPY;
	case Cut:
		return IDS_ACTIONRESITEMCUT;
	case MoveToRecybleBin:
		return IDS_ACTIONRESITEMRECYCLE;
	case DeleteFile:
		return IDS_ACTIONRESITEMDELETE;
	case OpenContextMenu:
		return IDS_ACTIONRESITEMOPENCONTEXTMENU;
	case OpenContextMenuSimple:
		return IDS_ACTIONRESITEMOPENCONTEXTMENUSIMPLE;
	case OpenFolder:
		return IDS_ACTIONRESITEMOPENFOLDER;
	case OpenContainingFolder:
		return IDS_ACTIONRESITEMOPENCONTFOLDER;
	case Properties:
		return IDS_ACTIONRESITEMPROPERTIES;
	case ShowSpecialMenu:
		return IDS_ACTIONRESITEMSPECIALMENU;
	case ExecuteCommand:
		return IDS_ACTIONRESITEMEXECUTECOMMAND;
	case SelectFile:
		return IDS_ACTIONRESITEMSELECTFILE;
	case RenameFile:
		return IDS_ACTIONRESITEMRENAMEFILE;
	case SelectNthFile:
		return IDS_ACTIONRESITEMSELECTNTHITEM;
	case ExecuteNthFile:
		return IDS_ACTIONRESITEMEXECUTENTHITEM;
	default:
		return 0;
	}
}

int CSubAction::GetMiscActionLabelStringId(CAction::ActionMisc uSubAction)
{
	switch (uSubAction)
	{
	case CAction::SendMessage:
		return IDS_ACTIONADVSENDMESSAGE;
	case CAction::PostMessage:
		return IDS_ACTIONADVPOSTMESSAGE;
	case CAction::ExecuteCommandMisc:
		return IDS_ACTIONRESITEMEXECUTECOMMAND;
	case CAction::InsertAsterisks:
		return IDS_ACTIONADVINSERTASTERISKS;
	default:
		return 0;
	}
}

int CSubAction::GetHelpActionLabelStringId(CAction::ActionHelp uSubAction)
{
	switch (uSubAction)
	{
	case HelpShowHelp:
		return IDS_ACTIONHELPSHOW;
	case HelpCloseHelp:
		return IDS_ACTIONHELPCLOSE;
	case HelpShowTopics:
		return IDS_ACTIONHELPTOPICS;
	case HelpIndex:
		return IDS_ACTIONHELPINDEX;
	case HelpTOC:
		return IDS_ACTIONHELPTOC;
	case HelpSearch:
		return IDS_ACTIONHELPSEARCH;
	default:
		return 0;
	}
}

void CSubAction::GetSettingsActionLabelString(ActionSettings uSubAction,CStringW& str)
{
	switch (uSubAction)
	{
	case OpenGeneralSettings:
		str.Format(IDS_ACTIONOPENSETTINGSTAB,(LPCWSTR)ID2W(IDS_GENERALSETTINGS));
		break;
	case OpenAdvancedSettings:
		str.Format(IDS_ACTIONOPENSETTINGSTAB,(LPCWSTR)ID2W(IDS_ADVANCEDSETTINGS));
		break;
	case OpenLanguageSettings:
		str.Format(IDS_ACTIONOPENSETTINGSTAB,(LPCWSTR)ID2W(IDS_LANGUAGESETTINGS));
		break;
	case OpenDatabaseSettings:
		str.Format(IDS_ACTIONOPENSETTINGSTAB,(LPCWSTR)ID2W(IDS_DATABASESETTINGS));
		break;
	case OpenAutoUpdateSettings:
		str.Format(IDS_ACTIONOPENSETTINGSTAB,(LPCWSTR)ID2W(IDS_AUTOUPDATESETTINGS));
		break;
	case OpenKeyboardShortcutSettings:
		str.Format(IDS_ACTIONOPENSETTINGSTAB,(LPCWSTR)ID2W(IDS_SHORTCUTSETTINGS));
		break;
	}
}

#endif

BOOL CShortcut::DoClassOrTitleMatch(LPCSTR pClass,LPCSTR pCondition)
{
	int nIndex;
    
	for (;;)
	{
		for (nIndex=0;pCondition[nIndex]!='\0' && pCondition[nIndex]!='|';nIndex++);
		
		if (_ContainString(pClass,pCondition,nIndex))
			return TRUE;

        if (pCondition[nIndex]=='\0')
			return FALSE;

        pCondition+=nIndex+1;
	}
}

BOOL CShortcut::IsWhenAndWhereSatisfied(HWND hSystemTrayWnd)  const
{
	if ((m_dwFlags&sfKeyTypeMask)==sfLocal)
	{
#ifndef KEYHOOK_EXPORTS
		CLocateDlg* pLocateDlg=GetLocateDlg();
		if (pLocateDlg!=NULL)
		{
			// Check if desired tab is chosen
			switch (pLocateDlg->GetCurrentTab())
			{
			case 0: // Name tab
				if (!(m_wWhenPressed&wpNameTabShown))
					return FALSE;
				break;
			case 1: // Size and Date tab
				if (!(m_wWhenPressed&wpSizeDateTabShown))
					return FALSE;
				break;
			case 2: // Advanced tab
				if (!(m_wWhenPressed&wpAdvancedTabShown))
					return FALSE;
				break;
			}

			// Check if "Find as you tybe" is running when not desired and vise verca
			if (pLocateDlg->IsInstantSearchingFlagSet(CLocateDlg::isRunning))
			{
				if (m_wWhenPressed&wpDisableWhenISRunning)
					return FALSE;
			}
			else
			{
				if (m_wWhenPressed&wpDisableWhenISNotRunning)
					return FALSE;
			}

		}
		return TRUE;
#else
		return FALSE;
#endif
	}

	if (m_pClass==NULL && m_pTitle==NULL)
		return TRUE;

	HWND hWnd=GetForegroundWindow();
	if (hWnd==NULL)
		return FALSE;

	// Checking class 
	if (m_pClass!=NULL)
	{
        if (m_pClass==LPCSTR(-1))
			return HWND(SendMessage(hSystemTrayWnd,WM_GETLOCATEDLG,0,0))==hWnd;
		

		char szClassName[200]="";
		GetClassName(hWnd,szClassName,200);

		if (!DoClassOrTitleMatch(szClassName,m_pClass))
			return FALSE;
	}

	// Checking class name
	if (m_pTitle!=NULL)
	{
		char szTitleName[200];
		GetWindowText(hWnd,szTitleName,200);
		
		if (!DoClassOrTitleMatch(szTitleName,m_pTitle))
			return FALSE;
	}
	return TRUE;
}


void CShortcut::SendEventBackToControl()
{
#ifndef KEYHOOK_EXPORTS
	CWinThread* pThread=GetTrayIconWnd()->GetLocateDlgThread();
	if (pThread!=NULL)
	{
		const MSG* pMsg=pThread->GetCurrentMessage();
		ASSERT(pMsg!=NULL);

		TranslateMessage(pMsg);
		if (IsUnicodeSystem())
			DispatchMessageW(pMsg);
		else
			DispatchMessageA(pMsg);
	}
#endif
}