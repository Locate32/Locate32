////////////////////////////////////////////////////////////////////////////
// Parsers.h		version 1.0.6.11250
// Copyright (C) 2001-2005 Janne Huttunen

#include <HFCLib.h>
#include "Parsers.h"

char* _parsestr(LPCSTR& pPtr)
{
	DWORD dwLength=istrlen(pPtr);
	dwLength++;
	char* pRet=new char[dwLength];
	dMemCopy(pRet,pPtr,dwLength);
	pPtr+=dwLength;
	return pRet;
}

char* _parseto(LPCSTR& lpPtr,char ch)
{
	CString str;
	while (*lpPtr!=ch && *lpPtr!='\0')
	{
		str << *lpPtr;
		lpPtr++;
	}
	if (str.IsEmpty())
		return NULL;
	return alloccopy(str,str.GetLength());	
}

void _parseto(LPCSTR& lpPtr,CString& str,char ch)
{
	str.Empty();
	while (*lpPtr!=ch && *lpPtr!='\0')
	{
		str << *lpPtr;
		lpPtr++;
	}
}

char* _parseto_ccm(LPCSTR& lpPtr)
{
	CString str;
	while (*lpPtr!=',' && *lpPtr!='\0')
	{
		if (*lpPtr=='\\')
		{
			lpPtr++;
			switch (*lpPtr)
			{
			case 'n':
				str << '\n';
				break;
			case 't':
				str << '\t';
				break;
			case 'r':
				str << '\r';
				break;
			default:
				str << *lpPtr;
				break;
			}
		}
		else
			str << *lpPtr;
		lpPtr++;
	}
	if (str.IsEmpty())
		return NULL;
	return alloccopy(str,str.GetLength());	
}

void _parseto_ccm(LPCSTR& lpPtr,CString& str)
{
	str.Empty();
	while (*lpPtr!=',' && *lpPtr!='\0')
	{
		if (*lpPtr=='\\')
		{
			lpPtr++;
			switch (*lpPtr)
			{
			case 'n':
				str << '\n';
				break;
			case 't':
				str << '\t';
				break;
			case 'r':
				str << '\r';
				break;
			default:
				str << *lpPtr;
				break;
			}
		}
		else
			str << *lpPtr;
		lpPtr++;
	}
}

char* _parseto_string(LPCSTR& lpPtr)
{
	if (*lpPtr!='\"')
		return NULL;
	lpPtr++;
	CString str;
	while (*lpPtr!='\"' && *lpPtr!='\0')
	{
		if (*lpPtr=='\\')
		{
			lpPtr++;
			switch (*lpPtr)
			{
			case 'n':
				str << '\n';
				break;
			case 't':
				str << '\t';
				break;
			case 'r':
				str << '\r';
				break;
			default:
				str << *lpPtr;
				break;
			}
		}
		else
			str << *lpPtr;
		lpPtr++;
	}
	lpPtr++;
	if (str.IsEmpty())
		return NULL;
	return alloccopy(str,str.GetLength());	
}

void _parseto_string(LPCSTR& lpPtr,CString& str)
{
	if (*lpPtr!='\"')
		return;
	lpPtr++;
	str.Empty();
	while (*lpPtr!='\"' && *lpPtr!='\0')
	{
		if (*lpPtr=='\\')
		{
			lpPtr++;
			switch (*lpPtr)
			{
			case 'n':
				str << '\n';
				break;
			case 't':
				str << '\t';
				break;
			case 'r':
				str << '\r';
				break;
			default:
				str << *lpPtr;
				break;
			}
		}
		else
			str << *lpPtr;
		lpPtr++;
	}
	lpPtr++;
}


char* _parseto_space(LPCSTR& lpPtr)
{
	CString str;
	while (*lpPtr!=',' && *lpPtr!='\0' && *lpPtr!=' ')
	{
		str << *lpPtr;
		lpPtr++;
	}
	if (str.IsEmpty())
		return NULL;
	return alloccopy(str,str.GetLength());	
}

void _parseto_space(LPCSTR& lpPtr,CString& str)
{
	str.Empty();
	while (*lpPtr!=',' && *lpPtr!='\0' && *lpPtr!=' ')
	{
		str << *lpPtr;
		lpPtr++;
	}
}

char* _parseto_equality(LPCSTR& lpPtr)
{
	CString str;
	while (*lpPtr!=',' && *lpPtr!='='  && *lpPtr!='<'  && *lpPtr!='>' && *lpPtr!='!' && *lpPtr!='\0' && *lpPtr!=' ')
	{
		str << *lpPtr;
		lpPtr++;
	}
	if (str.IsEmpty())
		return NULL;
	return alloccopy(str,str.GetLength());	
}

void _parseto_equality(LPCSTR& lpPtr,CString& str)
{
	str.Empty();
	while (*lpPtr!=',' && *lpPtr!='='  && *lpPtr!='<'  && *lpPtr!='>' && *lpPtr!='!' && *lpPtr!='\0' && *lpPtr!=' ')
	{
		str << *lpPtr;
		lpPtr++;
	}
}

int _parseto_int(LPCSTR& lpPtr)
{
	CString str;
	while (*lpPtr!=',' && *lpPtr!='=' && *lpPtr!='!' && *lpPtr!='\0' && *lpPtr!=' ')
	{
		str << *lpPtr;
		lpPtr++;
	}
	str.MakeUpper();
	if (str.Compare("TRUE")==0)
		return TRUE;
	if (str.Compare("FALSE")==0)
		return FALSE;
	int iRet=atoi(str);
	if (iRet==0)
		return str[0]=='0'?0:-1;
	return iRet;
}



LPCSTR FindSectionStart(LPCSTR lpData,LPCSTR lpSection)
{
	LPCSTR szLine=lpData;
	while (*szLine!='\0')
	{
		while (*szLine==' ' || *szLine=='\r' || *szLine=='\n') 
			szLine++;
		if (*szLine=='\0')
			return NULL;
		if (*szLine=='[')
		{
			CString Section;
			szLine++;
			while (*szLine!=']')
			{
				if (*szLine=='\0' || *szLine=='\n')
					return NULL;
				Section << *szLine;
				szLine++;
			}
			szLine++;
			if (Section.CompareNoCase(lpSection)==0)
			{
				while (*szLine==' ' || *szLine=='\r' || *szLine=='\n')
					szLine++;
				return szLine;
			}
		}
		else
		{
			while (*szLine!='\n' && *szLine!='\0') 
				szLine++;
		}
	}
	return NULL;
}

LPCSTR FindNextValue(LPCSTR lpData,CString& Key,CString& Value)
{
	LPCSTR szLine=lpData;
	Value.Empty();
	Key.Empty();
	while (*szLine=='\r' || *szLine=='\n' || *szLine==' ' || *szLine=='#')
	{
		if (*szLine=='#')
		{
			while (*szLine!='\r' && *szLine!='\n' && *szLine!='\0')
				szLine++;
		}
		else
			szLine++;
	}

	if (*szLine=='\0')
		return NULL;
	while (szLine[0]=='/' && szLine[1]=='/')
	{
		szLine+=2;
		while (*szLine!='\n' && *szLine!='\0')
			szLine++;
		while (*szLine=='\r' || *szLine=='\n' || *szLine==' ')
			szLine++;
	}
	if (*szLine=='\0')
		return NULL;
	while (*szLine!='=')
	{
		if (*szLine==' ')
		{
			if (szLine[1]=='=')
				szLine++;
			break;
		}
		if (*szLine=='\0' || *szLine=='[')
			return NULL;
		if (*szLine=='\r' || *szLine=='\n')
		{
			szLine++;
			while (*szLine=='\r' || *szLine=='\n')
				szLine++;
			return szLine;
		}
		Key << *szLine;
		szLine++;
	}
	while (Key[Key.GetLength()-1]==' ')
		Key.DelChar(Key.GetLength()-1);
	szLine++;
	while (*szLine==' ')
		szLine++;
	while (*szLine!='\n' && *szLine!='\r')
	{
		if (*szLine=='\0')
			return NULL;
		Value << *szLine;
		szLine++;
	}
	while (*szLine=='\n' || *szLine=='\r')
		szLine++;
	return szLine;
}

