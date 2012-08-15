////////////////////////////////////////////////////////////////////////////
// Parsers.h		version 1.0.6.11250
// Copyright (C) 2001-2005 Janne Huttunen

#ifndef PARSERS_H
#define PARSERS_H

#if !defined (HFC_NOFORCELIBS)
	#if defined(_DEBUG)
		#pragma comment(lib,"parsersd.lib")
	#else 
		#pragma comment(lib,"parsers.lib")
	#endif
#endif





char* _parsestr(LPCSTR& pPtr);
char* _parseto(LPCSTR& lpPtr,char ch=',');
void _parseto(LPCSTR& lpPtr,CString& str,char ch=',');
char* _parseto_space(LPCSTR& lpPtr);
void _parseto_space(LPCSTR& lpPtr,CString& str);
char* _parseto_equality(LPCSTR& lpPtr);
void _parseto_equality(LPCSTR& lpPtr,CString& str);
int _parseto_int(LPCSTR& lpPtr);
char* _parseto_ccm(LPCSTR& lpPtr);
void _parseto_ccm(LPCSTR& lpPtr,CString& str);
char* _parseto_string(LPCSTR& lpPtr);
void _parseto_string(LPCSTR& lpPtr,CString& str);

LPCSTR FindSectionStart(LPCSTR lpData,LPCSTR lpSection);
LPCSTR FindNextValue(LPCSTR lpData,CString& Key,CString& Value);
	

inline void _parsestr(LPCSTR& pPtr,CString& pStr)
{
	pStr.Copy(pPtr);
	pPtr+=pStr.GetLength()+1;
}


inline DWORD _parsedword(LPCSTR& pPtr)
{
	DWORD dwRet=*((DWORD*)pPtr);
	pPtr+=sizeof(DWORD);
	return dwRet;
}

inline DWORD _parseword(LPCSTR& pPtr)
{
	WORD wRet=*((WORD*)pPtr);
	pPtr+=sizeof(WORD);
	return wRet;
}

inline DWORD _parsebyte(LPCSTR& pPtr)
{
	BYTE wRet=*pPtr;
	pPtr+=sizeof(BYTE);
	return wRet;
}

inline char* _alloccopy(char* sSrc)
{
	if (sSrc==NULL)
		return NULL;
	return alloccopy(sSrc);
}


#endif