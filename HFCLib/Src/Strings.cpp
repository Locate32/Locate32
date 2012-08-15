////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////

#include "HFCLib.h"



#define STR_LOADSTRINGBUFLEN			1024

#ifdef WIN32
int strcasecmp(LPCSTR s1,LPCSTR s2)
{
	CHAR *tmp1,*tmp2;	
	int ret;
	DWORD nLen1=(DWORD)istrlen(s1);
	DWORD nLen2=(DWORD)istrlen(s2);
	
	tmp1=new CHAR[nLen1+2];
	if (tmp1==NULL)
		SetHFCError(HFC_CANNOTALLOC);
	CopyMemory(tmp1,s1,nLen1+1);
	
	tmp2=new CHAR[nLen2+2];
	if (tmp2==NULL)
		SetHFCError(HFC_CANNOTALLOC);
	CopyMemory(tmp2,s2,nLen2+1);
	
	CharLowerBuff(tmp1,nLen1);
	CharLowerBuff(tmp2,nLen2);
	ret=strcmp(tmp1,tmp2);
	delete[] tmp1;
	delete[] tmp2;
	return ret;
}
#endif

#ifdef DEF_WCHAR
int strcasecmp(LPCWSTR s1,LPCWSTR s2)
{
	WCHAR *tmp1,*tmp2;	
	int ret;
	DWORD nLen1=(DWORD)istrlenw(s1);
	DWORD nLen2=(DWORD)istrlenw(s2);
	
	tmp1=new WCHAR[nLen1+2];
	if (tmp1==NULL)
		SetHFCError(HFC_CANNOTALLOC);
	CopyMemory(tmp1,s1,nLen1*2+2);
	
	tmp2=new WCHAR[nLen2+2];
	if (tmp2==NULL)
		SetHFCError(HFC_CANNOTALLOC);
	CopyMemory(tmp2,s2,nLen2*2+2);
	
	CharLowerBuffW(tmp1,nLen1);
	CharLowerBuffW(tmp2,nLen2);
	ret=wcscmp(tmp1,tmp2);
	delete[] tmp1;
	delete[] tmp2;
	return ret;
}

LPSTR alloccopyWtoA(LPCWSTR szString)
{
	if (szString==NULL)
		return NULL;
	int nLen=WideCharToMultiByte(CP_ACP,0,szString,-1,NULL,0,NULL,NULL);
	CHAR* psz=new CHAR[nLen];
	WideCharToMultiByte(CP_ACP,0,szString,-1,psz,nLen,NULL,NULL);
	return psz;
}

LPSTR alloccopyWtoA(LPCWSTR szString,int iLength)
{
	ASSERT(iLength!=-1);
	if (szString==NULL)
		return NULL;
	int nLen=WideCharToMultiByte(CP_ACP,0,szString,iLength,NULL,0,NULL,NULL);
	CHAR* psz=new CHAR[nLen+1];
	WideCharToMultiByte(CP_ACP,0,szString,iLength,psz,nLen+1,NULL,NULL);
	psz[nLen]=L'\0';
	return psz;
}

LPSTR alloccopyWtoA(LPCWSTR szString,int iLength,int& iNewLen)
{
	ASSERT(iLength!=-1);
	if (szString==NULL)
		return NULL;
	iNewLen=WideCharToMultiByte(CP_ACP,0,szString,iLength,NULL,0,NULL,NULL);
	CHAR* psz=new CHAR[iNewLen+1];
	WideCharToMultiByte(CP_ACP,0,szString,iLength,psz,iNewLen+1,NULL,NULL);
	psz[iNewLen]=L'\0';
	return psz;
}

LPWSTR alloccopyAtoW(LPCSTR szString)
{
	if (szString==NULL)
		return NULL;
	int nLen=MultiByteToWideChar(CP_ACP,0,szString,-1,NULL,0);
	WCHAR* psz=new WCHAR[nLen];
	MultiByteToWideChar(CP_ACP,0,szString,-1,psz,nLen);
	return psz;
}

LPWSTR alloccopyAtoW(LPCSTR szString,int iLength)
{
	ASSERT(iLength!=-1);
	if (szString==NULL)
		return NULL;
	int nLen=MultiByteToWideChar(CP_ACP,0,szString,iLength,NULL,0);
	WCHAR* psz=new WCHAR[nLen+1];
	MultiByteToWideChar(CP_ACP,0,szString,iLength,psz,nLen+1);
	psz[nLen]=L'\0';
	return psz;
}

LPWSTR alloccopyAtoW(LPCSTR szString,int iLength,int& iNewLen)
{
	ASSERT(iLength!=-1);
	if (szString==NULL)
		return NULL;
	iNewLen=MultiByteToWideChar(CP_ACP,0,szString,iLength,NULL,0);
	WCHAR* psz=new WCHAR[iNewLen+1];
	MultiByteToWideChar(CP_ACP,0,szString,iLength,psz,iNewLen+1);
	psz[iNewLen]=L'\0';
	return psz;
}

LPSTR alloccopymultiWtoA(LPCWSTR szMultiString)
{
	int nTotLen;
	for (nTotLen=0;szMultiString[nTotLen]!='\0' || szMultiString[nTotLen+1]!='\0';nTotLen++);
	nTotLen+=2;

	int nLen=WideCharToMultiByte(CP_ACP,0,szMultiString,nTotLen,NULL,0,NULL,NULL);
	char* psz=new char[nLen];
	WideCharToMultiByte(CP_ACP,0,szMultiString,nTotLen,psz,nLen,NULL,NULL);
	return psz;
}

LPWSTR alloccopymultiAtoW(LPCSTR szMultiString)
{
	int nTotLen;
	for (nTotLen=0;szMultiString[nTotLen]!='\0' || szMultiString[nTotLen+1]!='\0';nTotLen++);
	nTotLen+=2;

	int nLen=MultiByteToWideChar(CP_ACP,0,szMultiString,nTotLen,NULL,0);
	WCHAR* psz=new WCHAR[nLen];
	MultiByteToWideChar(CP_ACP,0,szMultiString,nTotLen,psz,nLen);
	return psz;
}
#endif

int strcasencmp(LPCSTR s1,LPCSTR s2,DWORD n)
{
	if (int(n)<1)
		return 0;

	TCHAR *tmp1,*tmp2;	
	int ret;
	DWORD n1,n2;

	for (n1=0;n1<n && s1[n1]!='\0';n1++);
	for (n2=0;n2<n && s2[n2]!='\0';n2++);

	tmp1=new CHAR[n+1];
	if (tmp1==NULL)
		SetHFCError(HFC_CANNOTALLOC);
	tmp2=new CHAR[n+1];
	if (tmp2==NULL)
		SetHFCError(HFC_CANNOTALLOC);
	dMemCopy(tmp1,s1,n1);
	dMemCopy(tmp2,s2,n2);
    
	tmp1[n1]='\0';
	tmp2[n2]='\0';
	CharLower(tmp1);
	CharLower(tmp2);
	ret=strcmp(tmp1,tmp2);
	delete[] tmp1;
	delete[] tmp2;
	return ret;
}

int strcasencmp(LPCWSTR s1,LPCWSTR s2,DWORD n)
{
	if (int(n)<1)
		return 0;

	WCHAR *tmp1,*tmp2;	
	int ret;
	DWORD n1,n2;

	for (n1=0;n1<n && s1[n1]!='\0';n1++);
	for (n2=0;n2<n && s2[n2]!='\0';n2++);

	tmp1=new WCHAR[n+1];
	if (tmp1==NULL)
		SetHFCError(HFC_CANNOTALLOC);
	tmp2=new WCHAR[n+1];
	if (tmp2==NULL)
		SetHFCError(HFC_CANNOTALLOC);
	dMemCopy(tmp1,s1,n1*2);
	dMemCopy(tmp2,s2,n2*2);
    
	tmp1[n1]='\0';
	tmp2[n2]='\0';
	if (IsUnicodeSystem())
	{
		CharLowerW(tmp1);
		CharLowerW(tmp2);
	}
	else
	{
		_wcslwr_s(tmp1,n+1);
		_wcslwr_s(tmp2,n+1);
	}
	ret=wcscmp(tmp1,tmp2);
	delete[] tmp1;
	delete[] tmp2;
	return ret;
}


BOOL ContainString(LPCSTR s1,LPCSTR s2) // Is s2 in the s1
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

BOOL ContainString(LPCSTR s1,LPCWSTR s2) // Is s2 in the s1
{
    BOOL bBreakIfNotMatch;
	if (s2[0]==L'*')
	{
		if (s2[1]==L'\0')
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
				if (s2[i]==L'\0')
					return TRUE;
				return s2[i]==L'*' && s2[i+1]==L'\0';
			}
			// string differ
			if (A2Wc(s1[i])!=s2[i])
			{
				if (s2[i]==L'?')
					continue;
				
				if (s2[i]==L'*')
				{
					if (s2[i+1]==L'\0')
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

BOOL ContainString(LPCWSTR s1,LPCWSTR s2) // Is s2 in the s1
{
    BOOL bBreakIfNotMatch;
	if (s2[0]==L'*')
	{
		if (s2[1]==L'\0')
			return TRUE;
		s2++;
		bBreakIfNotMatch=FALSE;
	}
	else
		bBreakIfNotMatch=TRUE;

	while (*s1!=L'\0')
	{
		for (int i=0;;i++)
		{
			// is s1 too short?
			if (s1[i]==L'\0')
			{
				if (s2[i]==L'\0')
					return TRUE;
				return s2[i]==L'*' && s2[i+1]==L'\0';
			}
			// string differ
			if (s1[i]!=s2[i])
			{
				if (s2[i]==L'?')
					continue;
				
				if (s2[i]==L'*')
				{
					if (s2[i+1]==L'\0')
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




template<class CHARTYPE>
static int _getbase(const CHARTYPE*& str)
{
	if (*str==':')
		return 16;
	
	int base=0;
	if (*str=='(')
	{
		for (;*str=='(';str++);
		for (;*str>='0' && *str<='9';str++)
		{
			base*=10;
			base+=*str-'0';

		}
		for (;*str==')';str++);
	}
	else
	{
		for (;*str>='0' && *str<='9';str++)
		{
			base*=10;
			base+=*str-'0';
		}
	}
	return base>0?base:16;
}


static int _readstring(BYTE*& pRet,LPCSTR pStr,DWORD dwStrLen,MALLOC_FUNC pMalloc)
{
	pRet=(BYTE*)pMalloc(dwStrLen);
	int i;
	for (i=0;*pStr!='\0';i++,pStr++)
	{
		if (*pStr=='\\' && pStr[1]!='\0')
		{
			pStr++;
			switch (*pStr)
			{
			case '0':
				pRet[i]='\0';
				break;
			case 'n':
				pRet[i]='\n';
				break;
			case 'r':
				pRet[i]='\r';
				break;
			case 't':
				pRet[i]='\t';
				break;
			case 'b':
				pRet[i]='\b';
				break;
			default:
				pRet[i]=BYTE(_readnum(16,pStr,2));
				pStr--;
				break;
			}
		}
		else
			pRet[i]=*pStr;
	}
	return i;
}

static int _readstringW(WCHAR*& pRet,LPCSTR pStr,DWORD dwStrLen,MALLOC_FUNC pMalloc)
{
	pRet=(WCHAR*)pMalloc(dwStrLen*2);

	int i;
	for (i=0;*pStr!='\0';i++,pStr++)
	{
		if (*pStr=='\\' && pStr[1]!='\0')
		{
			pStr++;
			switch (*pStr)
			{
			case '0':
				pRet[i]=L'\0';
				break;
			case 'n':
				pRet[i]=L'\n';
				break;
			case 'r':
				pRet[i]=L'\r';
				break;
			case 't':
				pRet[i]=L'\t';
				break;
			case 'b':
				pRet[i]=L'\b';
				break;
			default:
				pRet[i]=WCHAR(_readnum(16,pStr,4));
				pStr--;
				break;
			}
		}
		else
			pRet[i]=A2Wc(*pStr);
	}
	return i;
}

static int _readstringW(WCHAR*& pRet,LPCWSTR pStr,DWORD dwStrLen,MALLOC_FUNC pMalloc)
{
	pRet=(WCHAR*)pMalloc(dwStrLen*2);

	int i;
	for (i=0;*pStr!='\0';i++,pStr++)
	{
		if (*pStr=='\\' && pStr[1]!='\0')
		{
			pStr++;
			switch (*pStr)
			{
			case '0':
				pRet[i]=L'\0';
				break;
			case 'n':
				pRet[i]=L'\n';
				break;
			case 'r':
				pRet[i]=L'\r';
				break;
			case 't':
				pRet[i]=L'\t';
				break;
			case 'b':
				pRet[i]=L'\b';
				break;
			default:
				pRet[i]=WCHAR(_readnum(16,pStr,4));
				pStr--;
				break;
			}
		}
		else
			pRet[i]=*pStr;
	}
	return i;
}

/*
BYTE* dataparser(LPCSTR pString,DWORD dwStrLen,DWORD* pdwDataLength);

Converts str to BYTE* pointer and returns it. 
Returned pointer should delete with delete[](BYTE*) operator.

if str is:
  "Hello", data will be "Hello" and *pdwDataLength will be 5
  "str:Hello", data will be "Hello" and *pdwDataLength will be 5
  "str:Hello\0", data will be "Hello\0" and *pdwDataLength will be 6
  "oem:Hello", data will be "Hello" and *pdwDataLength will be 5, ansi to oem conversion will be done
  "unicode:Hello", data will be L"Hello" and *pdwDataLength will be 10, ansi to unicode conversion will be done
  "wstr:Hello", same as above
  "utf16:Hello", same as above
  "utf8:Hello", conversion to utf8
  "utf7:Hello", conversion to utf7
  "dword(16):12345678", *(DWORD*)data will be 0x12345678 (hex) and *pdwDataLength will be 4
  "dword(16):12345678", *(DWORD*)data will be 0x12345678 (hex) and *pdwDataLength will be 4
  "dword(10):12345678", *(DWORD*)data will be 12345678 (decimal) and *pdwDataLength will be 4
  "dword:10" and "dword16:10" are same as "dword(16):10" (16 is default)
  "int:10" is same as "dword(10):10"
  "word(16):1234", *(WORD*)data will be 0x1234 and *pdwDataLength will be 2
  "byte(16):12", *(BYTE*)data will be 0x12 and *pdwDataLength will be 1

if str begins with "hex:" or "bin:" following data will be hex data e.g:
  "hex:12 34 56 78" (same as "dword(16):78563412")
  "hex:12 34" (same as "word(16):3412")
  "hex:123456789abcde"
  "hex:0000000000" (5 zero bytes)
  "hex:0 0 0 0 0" (5 zero bytes)

*/

BYTE* dataparser(LPCSTR pStr,DWORD dwStrLen,MALLOC_FUNC pMalloc,DWORD* pdwDataLength)
{
	if (pStr[0]=='\0')
		return NULL;
	
	if (_1stcontain2nd(pStr,"int:"))
	{
		pStr+=4;
		BYTE* pRet=(BYTE*)pMalloc(4);
		*((DWORD*)pRet)=DWORD(_readnum(10,pStr));
		if (pdwDataLength!=NULL)
			*pdwDataLength=4;
		return pRet;
	}
	else if (_1stcontain2nd(pStr,"dword"))
	{
		pStr+=5;
		int base=_getbase(pStr);
		if (*pStr!=':')
			return NULL;
		pStr++;
		BYTE* pRet=(BYTE*)pMalloc(4);
		*((DWORD*)pRet)=DWORD(_readnum(base,pStr));
		if (pdwDataLength!=NULL)
			*pdwDataLength=4;
		return pRet;
	}
	else if (_1stcontain2nd(pStr,"word"))
	{
		pStr+=4;		
		int base=_getbase(pStr);
		if (*pStr!=':')
			return NULL;
		pStr++;
		BYTE* pRet=(BYTE*)pMalloc(2);
		*((WORD*)pRet)=WORD(_readnum(base,pStr));
		if (pdwDataLength!=NULL)
			*pdwDataLength=2;
		return pRet;
	}
	else if (_1stcontain2nd(pStr,"byte"))
	{
		pStr+=4;		
		int base=_getbase(pStr);
		if (*pStr!=':')
			return NULL;
		pStr++;
		BYTE* pRet=(BYTE*)pMalloc(2);
		*pRet=BYTE(_readnum(base,pStr));
		if (pdwDataLength!=NULL)
			*pdwDataLength=1;
		return pRet;
	}
	else if (_1stcontain2nd(pStr,"hex:") || _1stcontain2nd(pStr,"bin:"))
	{
		pStr+=4;
		int i=0;
		
		// Calculating reqiured size
		for (LPCSTR pStr2=pStr;*pStr2!='\0';i++)
		{
			if (!((*pStr2>='0' && *pStr2<='9') || 
				(*pStr2>='a' && *pStr2<='f') ||
				(*pStr2>='A' && *pStr2<='F')))
				break;
				
			pStr2++;

			if ((*pStr2>='0' && *pStr2<='9') || 
				(*pStr2>='a' && *pStr2<='f') ||
				(*pStr2>='A' && *pStr2<='F'))
				pStr2++;

			for (;*pStr2==' ';pStr2++);
		}

		if (i==0)
			return NULL;
			
		BYTE* pRet=(BYTE*)pMalloc(max(i,2));

		for (i=0;*pStr!='\0';i++)
		{
			if (*pStr>='0' && *pStr<='9')
				pRet[i]=*pStr-'0';
			else if (*pStr>='a' && *pStr<='f')
				pRet[i]=*pStr-'a'+0xa;
			else if (*pStr>='A' && *pStr<='F')
				pRet[i]=*pStr-'A'+0xa;
			else
				break;

			pStr++;

			if (*pStr>='0' && *pStr<='9')
			{
				pRet[i]<<=4;
				pRet[i]+=*pStr-'0';
				pStr++;
			}
			else if (*pStr>='a' && *pStr<='f')
			{
				pRet[i]<<=4;
				pRet[i]+=*pStr-'a'+0xa;
				pStr++;
			}
			else if (*pStr>='A' && *pStr<='F')
			{
				pRet[i]<<=4;
				pRet[i]+=*pStr-'A'+0xa;
				pStr++;
			}

			for (;*pStr==' ';pStr++);
		}
		if (pdwDataLength!=NULL)
			*pdwDataLength=i;
		return pRet;
	}
	else if (_1stcontain2nd(pStr,"str:"))
	{
		dwStrLen-=4;
		pStr+=4;
		if (int(dwStrLen)<=0)
			return NULL;

		BYTE* pRet;
		int len=_readstring(pRet,pStr,dwStrLen,pMalloc);
		
		if (pdwDataLength!=NULL)
			*pdwDataLength=len;
		return pRet;
	}
#ifdef WIN32
	else if (_1stcontain2nd(pStr,"wstr:") || _1stcontain2nd(pStr,"uni:") || _1stcontain2nd(pStr,"utf16:"))
	{
		for (pStr+=3,dwStrLen-=3;*pStr!=':';pStr++,dwStrLen--);
		pStr++;dwStrLen--;

		if (int(dwStrLen)<=0)
			return NULL;
	
		WCHAR* pRet;
		int len=_readstringW(pRet,pStr,dwStrLen,pMalloc);

		if (pdwDataLength!=NULL)
			*pdwDataLength=len*2;
		return (BYTE*)pRet;
	}
	else if (_1stcontain2nd(pStr,"utf8:"))
	{
		dwStrLen-=5;
		pStr+=5;

		if (int(dwStrLen)<=0)
			return NULL;
	
		WCHAR* pUnicode;
		int len=_readstringW(pUnicode,pStr,dwStrLen,malloc);

		// Get Length
		char* pRet=(char*)pMalloc((len+1)*2);
		len=WideCharToMultiByte(CP_UTF8,0,pUnicode,len,pRet,(len+1)*2,NULL,NULL);

		free(pUnicode);
		
		if (pdwDataLength!=NULL)
			*pdwDataLength=len;
		return (BYTE*)pRet;
	}
	else if (_1stcontain2nd(pStr,"utf7:"))
	{
		dwStrLen-=5;
		pStr+=5;

		if (int(dwStrLen)<=0)
			return NULL;
	
		WCHAR* pUnicode;
		int len=_readstringW(pUnicode,pStr,dwStrLen,malloc);

		// Get Length
		char* pRet=(char*)pMalloc((len+1)*5);		
		len=WideCharToMultiByte(CP_UTF7,0,pUnicode,len,pRet,(len+1)*5,NULL,NULL);

		free(pUnicode);
		
		if (pdwDataLength!=NULL)
			*pdwDataLength=len;
		return (BYTE*)pRet;
	}
	else if (_1stcontain2nd(pStr,"oem:"))
	{
		dwStrLen-=4;
		pStr+=4;

		if (int(dwStrLen)<=0)
			return NULL;
	
		WCHAR* pUnicode;
		int len=_readstringW(pUnicode,pStr,dwStrLen,malloc);

		// Get Length
		len=WideCharToMultiByte(CP_OEMCP,0,pUnicode,len,NULL,0,NULL,NULL);
		char* pRet=(char*)pMalloc(len+1);		
		len=WideCharToMultiByte(CP_OEMCP,0,pUnicode,len,pRet,len+1,NULL,NULL);

		free(pUnicode);
		
		if (pdwDataLength!=NULL)
			*pdwDataLength=len;
		return (BYTE*)pRet;
	}
#endif
	else
	{
		if (int(dwStrLen)<=0)
			return NULL;
		
		BYTE* pRet;
		pRet=(BYTE*)pMalloc(dwStrLen);
		dMemCopy(pRet,pStr,dwStrLen);
		if (pdwDataLength!=NULL)
			*pdwDataLength=dwStrLen;
		return pRet;
	}
}

#ifdef DEF_WCHAR
BYTE* dataparser(LPCWSTR pStr,DWORD dwStrLen,MALLOC_FUNC pMalloc,DWORD* pdwDataLength)
{
	if (pStr[0]==L'\0')
		return NULL;
	
	if (_1stcontain2nd(pStr,L"int:"))
	{
		pStr+=4;
		BYTE* pRet=(BYTE*)pMalloc(4);
		*((DWORD*)pRet)=DWORD(_readnum(10,pStr));
		if (pdwDataLength!=NULL)
			*pdwDataLength=4;
		return pRet;
	}
	else if (_1stcontain2nd(pStr,L"dword"))
	{
		pStr+=5;
		int base=_getbase(pStr);
		if (*pStr!=':')
			return NULL;
		pStr++;
		BYTE* pRet=(BYTE*)pMalloc(4);
		*((DWORD*)pRet)=DWORD(_readnum(base,pStr));
		if (pdwDataLength!=NULL)
			*pdwDataLength=4;
		return pRet;
	}
	else if (_1stcontain2nd(pStr,L"word"))
	{
		pStr+=4;		
		int base=_getbase(pStr);
		if (*pStr!=':')
			return NULL;
		pStr++;
		BYTE* pRet=(BYTE*)pMalloc(2);
		*((WORD*)pRet)=WORD(_readnum(base,pStr));
		if (pdwDataLength!=NULL)
			*pdwDataLength=2;
		return pRet;
	}
	else if (_1stcontain2nd(pStr,L"byte"))
	{
		pStr+=4;		
		int base=_getbase(pStr);
		if (*pStr!=':')
			return NULL;
		pStr++;
		BYTE* pRet=(BYTE*)pMalloc(2);
		*pRet=BYTE(_readnum(base,pStr));
		if (pdwDataLength!=NULL)
			*pdwDataLength=1;
		return pRet;
	}
	else if (_1stcontain2nd(pStr,L"hex:") || _1stcontain2nd(pStr,L"bin:"))
	{
		pStr+=4;
		int i=0;
		
		// Calculating reqiured size
		for (LPCWSTR pStr2=pStr;*pStr2!='\0';i++)
		{
			if (!((*pStr2>='0' && *pStr2<='9') || 
				(*pStr2>='a' && *pStr2<='f') ||
				(*pStr2>='A' && *pStr2<='F')))
				break;
				
			pStr2++;

			if ((*pStr2>='0' && *pStr2<='9') || 
				(*pStr2>='a' && *pStr2<='f') ||
				(*pStr2>='A' && *pStr2<='F'))
				pStr2++;

			for (;*pStr2==' ';pStr2++);
		}

		if (i==0)
			return NULL;
			
		BYTE* pRet=(BYTE*)pMalloc(max(i,2));

		for (i=0;*pStr!='\0';i++)
		{
			if (*pStr>='0' && *pStr<='9')
				pRet[i]=*pStr-'0';
			else if (*pStr>='a' && *pStr<='f')
				pRet[i]=*pStr-'a'+0xa;
			else if (*pStr>='A' && *pStr<='F')
				pRet[i]=*pStr-'A'+0xa;
			else
				break;

			pStr++;

			if (*pStr>='0' && *pStr<='9')
			{
				pRet[i]<<=4;
				pRet[i]+=*pStr-'0';
				pStr++;
			}
			else if (*pStr>='a' && *pStr<='f')
			{
				pRet[i]<<=4;
				pRet[i]+=*pStr-'a'+0xa;
				pStr++;
			}
			else if (*pStr>='A' && *pStr<='F')
			{
				pRet[i]<<=4;
				pRet[i]+=*pStr-'A'+0xa;
				pStr++;
			}

			for (;*pStr==' ';pStr++);
		}
		if (pdwDataLength!=NULL)
			*pdwDataLength=i;
		return pRet;
	}
	else if (_1stcontain2nd(pStr,L"str:"))
	{
		dwStrLen-=4;
		pStr+=4;
		if (int(dwStrLen)<=0)
			return NULL;

		WCHAR* pUnicode;
		int len=_readstringW(pUnicode,pStr,dwStrLen,malloc);

		// Get Length
		len=WideCharToMultiByte(CP_ACP,0,pUnicode,len,NULL,0,NULL,NULL);
		char* pRet=(char*)pMalloc(len+1);		
		len=WideCharToMultiByte(CP_ACP,0,pUnicode,len,pRet,len+1,NULL,NULL);

		free(pUnicode);
		
		if (pdwDataLength!=NULL)
			*pdwDataLength=len;
		return (BYTE*)pRet;
	}
	else if (_1stcontain2nd(pStr,L"oem:"))
	{
		dwStrLen-=4;
		pStr+=4;
		if (int(dwStrLen)<=0)
			return NULL;
		
		WCHAR* pUnicode;
		int len=_readstringW(pUnicode,pStr,dwStrLen,malloc);

		// Get Length
		len=WideCharToMultiByte(CP_OEMCP,0,pUnicode,len,NULL,0,NULL,NULL);
		char* pRet=(char*)pMalloc(len+1);		
		len=WideCharToMultiByte(CP_OEMCP,0,pUnicode,len,pRet,len+1,NULL,NULL);

		free(pUnicode);
		
		if (pdwDataLength!=NULL)
			*pdwDataLength=len;
		return (BYTE*)pRet;
	}
	else if (_1stcontain2nd(pStr,L"wstr:") || _1stcontain2nd(pStr,L"uni:") || _1stcontain2nd(pStr,L"utf16:"))
	{
		for (pStr+=3,dwStrLen-=3;*pStr!=':';pStr++,dwStrLen--);
		pStr++;dwStrLen--;

		if (int(dwStrLen)<=0)
			return NULL;
	
		WCHAR* pRet;
		int len=_readstringW(pRet,pStr,dwStrLen,pMalloc);
		
		if (pdwDataLength!=NULL)
			*pdwDataLength=len*2;
		return (BYTE*)pRet;
	}
	else if (_1stcontain2nd(pStr,L"utf8:"))
	{
		dwStrLen-=5;
		pStr+=5;
		if (int(dwStrLen)<=0)
			return NULL;
		
		WCHAR* pUnicode;
		int len=_readstringW(pUnicode,pStr,dwStrLen,malloc);

		// Get Length
		char* pRet=(char*)pMalloc((len+1)*2);		
		len=WideCharToMultiByte(CP_UTF8,0,pUnicode,len,pRet,(len+1)*2,NULL,NULL);

		free(pUnicode);
		
		if (pdwDataLength!=NULL)
			*pdwDataLength=len;
		return (BYTE*)pRet;
	}
	else if (_1stcontain2nd(pStr,L"utf7:"))
	{
		dwStrLen-=5;
		pStr+=5;
		if (int(dwStrLen)<=0)
			return NULL;
		
		WCHAR* pUnicode;
		int len=_readstringW(pUnicode,pStr,dwStrLen,malloc);

		// Get Length
		char* pRet=(char*)pMalloc((len+1)*5);		
		len=WideCharToMultiByte(CP_UTF7,0,pUnicode,len,pRet,(len+1)*5,NULL,NULL);

		free(pUnicode);
		
		if (pdwDataLength!=NULL)
			*pdwDataLength=len;
		return (BYTE*)pRet;
	}
	else
	{
		if (int(dwStrLen)<=0)
			return NULL;
		
		if (pdwDataLength!=NULL)
			*pdwDataLength=dwStrLen;

		int nAllocLen=LenWtoA(pStr,dwStrLen);
		char* pNew=(char*)pMalloc(nAllocLen+1);
		MemCopyWtoA(pNew,nAllocLen+1,pStr,dwStrLen);
		pNew[nAllocLen]='\0';
		return (BYTE*)pNew;
	}
}
#endif

template<class CHARTYPE>
static int _getbase2(const CHARTYPE*& str,int def)
{
	if (*str=='(')
		return def;
	
	int base=0;
	for (;*str>='0' && *str<='9';str++)
	{
		base*=10;
		base+=*str-'0';
	}
	return base>0?base:def;
}

/*
BYTE* dataparser2(LPCSTR pString,DWORD* pdwDataLength);

Converts str to BYTE* pointer and returns it. 
Returned pointer should delete with delete[](BYTE*) operator.

hex is default style, usage e.g. "FF EE oem(Hello)" of "11 int(10) dword(1234,16)"
if str is:

  "str(Hello)", data will be "Hello" and *pdwDataLength will be 5
  "str(Hello\0)", data will be "Hello\0" and *pdwDataLength will be 6
  "oem(Hello)", data will be "Hello" and *pdwDataLength will be 5, ansi to oem conversion will be done
  "unicode(Hello)", data will be L"Hello" and *pdwDataLength will be 10, ansi to unicode conversion will be done
  "dword16(12345678)", *(DWORD*)data will be 0x12345678 (hex) and *pdwDataLength will be 4
  "dword10(12345678)", *(DWORD*)data will be 12345678 (decimal) and *pdwDataLength will be 4
  "dword(X)" is same as "dword16(X)" (16 is default in case of dword)
  "int(X)" is same as "int10(X)" (10 is default in case of int)
  "word16(1234)", *(WORD*)data will be 0x1234 and *pdwDataLength will be 2
  "byte16(12)", *(BYTE*)data will be 0x12 and *pdwDataLength will be 1
*/

inline void _allocmore(BYTE*& pStr,BYTE*& pStrPtr,DWORD& nAllocLen,int nRequired)
{
	DWORD nLen=DWORD(pStrPtr-pStr);
	if (nAllocLen<nLen+nRequired)
	{
		BYTE* pNewPtr=new BYTE[nAllocLen=nLen+nRequired+10];
        MemCopy(pNewPtr,pStr,nLen);
		delete[] pStr;
		pStr=pNewPtr;
		pStrPtr=pStr+nLen;
	}
}

BYTE* dataparser2(LPCSTR pStr,DWORD* pdwDataLength)
{
	if (pStr[0]=='\0')
		return NULL;
	
	BYTE* pData=new BYTE[10];
	BYTE* pDataPtr=pData;
	DWORD nAllocLen=10;

	// Removing spaces 
	while (*pStr==' ') pStr++;

	while (*pStr!='\0')
	{
		if (_1stcontain2nd(pStr,"int"))
		{
			pStr+=3;
			int base=_getbase2(pStr,10);
			if (*pStr!='(')
				break;
			pStr++;
			
			_allocmore(pData,pDataPtr,nAllocLen,4);
			*((DWORD*)pDataPtr)=DWORD(_readnum(base,pStr));
			while (*pStr!=')' && *pStr!='\0') pStr++;
			if (*pStr==')') pStr++;
			pDataPtr+=4;
		}
		else if (_1stcontain2nd(pStr,"dword"))
		{
			pStr+=5;
			int base=_getbase2(pStr,16);
			if (*pStr!='(')
				break;
			pStr++;
			
			_allocmore(pData,pDataPtr,nAllocLen,4);
			*((DWORD*)pDataPtr)=DWORD(_readnum(base,pStr));
			while (*pStr!=')' && *pStr!='\0') pStr++;
			if (*pStr==')') pStr++;
			pDataPtr+=4;
		}
		else if (_1stcontain2nd(pStr,"word"))
		{
			pStr+=4;
			int base=_getbase2(pStr,16);
			if (*pStr!='(')
				break;
			pStr++;
			
			_allocmore(pData,pDataPtr,nAllocLen,2);
			*((WORD*)pDataPtr)=WORD(_readnum(base,pStr));
			while (*pStr!=')' && *pStr!='\0') pStr++;
			if (*pStr==')') pStr++;
			pDataPtr+=2;
		}
		else if (_1stcontain2nd(pStr,"byte"))
		{
			pStr+=4;
			int base=_getbase2(pStr,16);
			if (*pStr!='(')
				break;
			pStr++;
			
			_allocmore(pData,pDataPtr,nAllocLen,1);
			*((BYTE*)pDataPtr)=BYTE(_readnum(base,pStr));
			while (*pStr!=')' && *pStr!='\0') pStr++;
			if (*pStr==')') pStr++;
			pDataPtr+=1;
		}
		else if (_1stcontain2nd(pStr,"str"))
		{
			pStr+=3;
			if (*pStr!='(')
				break;
			pStr++;
			
			for (;*pStr!=')' && *pStr!='\0';pStr++)
			{
				_allocmore(pData,pDataPtr,nAllocLen,1);
				
				if (*pStr=='\\' && pStr[1]!='\0')
				{
					pStr++;
					switch (*pStr)
					{
					case '0':
						*(pDataPtr++)='\0';
						break;
					case 'n':
						*(pDataPtr++)='\n';
						break;
					case 'r':
						*(pDataPtr++)='\r';
						break;
					case 't':
						*(pDataPtr++)='\t';
						break;
					case 'b':
						*(pDataPtr++)='\b';
						break;
					case ')':
						*(pDataPtr++)=')';
						break;
					default:
						*(pDataPtr++)=BYTE(_readnum(16,pStr,2));;
						pStr--;
						break;
					}
				}
				else
					*(pDataPtr++)=*pStr;
			}
			if (*pStr==')') pStr++;
		}
	#ifdef WIN32
		else if (_1stcontain2nd(pStr,"oem"))
		{
			pStr+=3;
			if (*pStr!='(')
				break;
			pStr++;
			
			int iStart=DWORD(pDataPtr-pData);
			
			for (;*pStr!=')' && *pStr!='\0';pStr++)
			{
				_allocmore(pData,pDataPtr,nAllocLen,1);
				
				if (*pStr=='\\' && pStr[1]!='\0')
				{
					pStr++;
					switch (*pStr)
					{
					case '0':
						*(pDataPtr++)='\0';
						break;
					case 'n':
						*(pDataPtr++)='\n';
						break;
					case 'r':
						*(pDataPtr++)='\r';
						break;
					case 't':
						*(pDataPtr++)='\t';
						break;
					case 'b':
						*(pDataPtr++)='\b';
						break;
					case ')':
						*(pDataPtr++)=')';
						break;
					default:
						*(pDataPtr++)=BYTE(_readnum(16,pStr,2));;
						pStr--;
						break;
					}
				}
				else
					*(pDataPtr++)=*pStr;
			}
			CharToOemBuff(LPSTR(pData+iStart),LPSTR(pData+iStart),DWORD(pDataPtr-pData)-iStart);
			if (*pStr==')') pStr++;
		}
	#endif
	#ifdef DEF_WCHAR
		else if (_1stcontain2nd(pStr,"wstr") || _1stcontain2nd(pStr,"uni"))
		{
			pStr+=3;
			while (*pStr!='(') pStr++;
			pStr++;
			
			for (;*pStr!=')' && *pStr!='\0';pStr++)
			{
				_allocmore(pData,pDataPtr,nAllocLen,2);
				
				if (*pStr=='\\' && pStr[1]!='\0')
				{
					pStr++;
					switch (*pStr)
					{
					case '0':
						*LPWORD(pDataPtr)=L'\0';
						break;
					case 'n':
						*LPWORD(pDataPtr)=L'\n';
						break;
					case 'r':
						*LPWORD(pDataPtr)=L'\r';
						break;
					case 't':
						*LPWORD(pDataPtr)=L'\t';
						break;
					case 'b':
						*LPWORD(pDataPtr)=L'\b';
						break;
					case ')':
						*LPWORD(pDataPtr)=L')';
						break;
					default:
						*LPWORD(pDataPtr)=WORD(_readnum(16,pStr,4));;
						pStr--;
						break;
					}
					pDataPtr+=2;
				}
				else
				{	
					_MemCopyAtoW(pDataPtr,nAllocLen,pStr,1);
					pDataPtr+=2;
				}
			}
			if (*pStr==')') pStr++;
		}
	#endif
		else
		{
			_allocmore(pData,pDataPtr,nAllocLen,1);
			LPCSTR pStrOld=pStr;
			*pDataPtr=_readnum(16,pStr,2);
			if (pStr==pStrOld)
			{
				*pdwDataLength=DWORD(pDataPtr-pData);
				return pData;
			}
			pDataPtr++;
		}
		while (*pStr==' ') pStr++;
	}
	*pdwDataLength=DWORD(pDataPtr-pData);
	return pData;
}

#ifdef DEF_WCHAR
BYTE* dataparser2(LPCWSTR pStr,DWORD* pdwDataLength)
{
	if (pStr[0]=='\0')
		return NULL;
	
	BYTE* pData=new BYTE[10];
	BYTE* pDataPtr=pData;
	DWORD nAllocLen=10;

	// Removing spaces 
	while (*pStr==' ') pStr++;

	while (*pStr!='\0')
	{
		if (_1stcontain2nd(pStr,L"int"))
		{
			pStr+=3;
			int base=_getbase2(pStr,10);
			if (*pStr!='(')
				break;
			pStr++;
			
			_allocmore(pData,pDataPtr,nAllocLen,4);
			*((DWORD*)pDataPtr)=DWORD(_readnum(base,pStr));
			while (*pStr!=')' && *pStr!='\0') pStr++;
			if (*pStr==')') pStr++;
			pDataPtr+=4;
		}
		else if (_1stcontain2nd(pStr,L"dword"))
		{
			pStr+=5;
			int base=_getbase2(pStr,16);
			if (*pStr!='(')
				break;
			pStr++;
			
			_allocmore(pData,pDataPtr,nAllocLen,4);
			*((DWORD*)pDataPtr)=DWORD(_readnum(base,pStr));
			while (*pStr!=')' && *pStr!='\0') pStr++;
			if (*pStr==')') pStr++;
			pDataPtr+=4;
		}
		else if (_1stcontain2nd(pStr,L"word"))
		{
			pStr+=4;
			int base=_getbase2(pStr,16);
			if (*pStr!='(')
				break;
			pStr++;
			
			_allocmore(pData,pDataPtr,nAllocLen,2);
			*((WORD*)pDataPtr)=WORD(_readnum(base,pStr));
			while (*pStr!=')' && *pStr!='\0') pStr++;
			if (*pStr==')') pStr++;
			pDataPtr+=2;
		}
		else if (_1stcontain2nd(pStr,L"byte"))
		{
			pStr+=4;
			int base=_getbase2(pStr,16);
			if (*pStr!='(')
				break;
			pStr++;
			
			_allocmore(pData,pDataPtr,nAllocLen,1);
			*((BYTE*)pDataPtr)=BYTE(_readnum(base,pStr));
			while (*pStr!=')' && *pStr!='\0') pStr++;
			if (*pStr==')') pStr++;
			pDataPtr+=1;
		}
		else if (_1stcontain2nd(pStr,L"str"))
		{
			pStr+=3;
			if (*pStr!='(')
				break;
			pStr++;
			
			for (;*pStr!=')' && *pStr!='\0';pStr++)
			{
				_allocmore(pData,pDataPtr,nAllocLen,1);
				
				if (*pStr=='\\' && pStr[1]!='\0')
				{
					pStr++;
					switch (*pStr)
					{
					case '0':
						*(pDataPtr++)='\0';
						break;
					case 'n':
						*(pDataPtr++)='\n';
						break;
					case 'r':
						*(pDataPtr++)='\r';
						break;
					case 't':
						*(pDataPtr++)='\t';
						break;
					case 'b':
						*(pDataPtr++)='\b';
						break;
					case ')':
						*(pDataPtr++)=')';
						break;
					default:
						*(pDataPtr++)=BYTE(_readnum(16,pStr,2));;
						pStr--;
						break;
					}
				}
				else
					*(pDataPtr++)=W2Ac(*pStr);
			}
			if (*pStr==')') pStr++;
		}
		else if (_1stcontain2nd(pStr,L"oem"))
		{
			pStr+=3;
			if (*pStr!='(')
				break;
			pStr++;
			
			int iStart=DWORD(pDataPtr-pData);
			
			for (;*pStr!=')' && *pStr!='\0';pStr++)
			{
				_allocmore(pData,pDataPtr,nAllocLen,1);
				
				if (*pStr=='\\' && pStr[1]!='\0')
				{
					pStr++;
					switch (*pStr)
					{
					case '0':
						*(pDataPtr++)='\0';
						break;
					case 'n':
						*(pDataPtr++)='\n';
						break;
					case 'r':
						*(pDataPtr++)='\r';
						break;
					case 't':
						*(pDataPtr++)='\t';
						break;
					case 'b':
						*(pDataPtr++)='\b';
						break;
					case ')':
						*(pDataPtr++)=')';
						break;
					default:
						*(pDataPtr++)=BYTE(_readnum(16,pStr,2));;
						pStr--;
						break;
					}
				}
				else
					*(pDataPtr++)=W2Ac(*pStr);
			}
			CharToOemBuff(LPSTR(pData+iStart),LPSTR(pData+iStart),DWORD(pDataPtr-pData)-iStart);
			if (*pStr==')') pStr++;
		}
		else if (_1stcontain2nd(pStr,L"wstr") || _1stcontain2nd(pStr,L"uni"))
		{
			pStr+=3;
			while (*pStr!='(') pStr++;
			pStr++;
			
			for (;*pStr!=')' && *pStr!='\0';pStr++)
			{
				_allocmore(pData,pDataPtr,nAllocLen,2);
				
				if (*pStr=='\\' && pStr[1]!='\0')
				{
					pStr++;
					switch (*pStr)
					{
					case '0':
						*LPWORD(pDataPtr)=L'\0';
						break;
					case 'n':
						*LPWORD(pDataPtr)=L'\n';
						break;
					case 'r':
						*LPWORD(pDataPtr)=L'\r';
						break;
					case 't':
						*LPWORD(pDataPtr)=L'\t';
						break;
					case 'b':
						*LPWORD(pDataPtr)=L'\b';
						break;
					case ')':
						*LPWORD(pDataPtr)=L')';
						break;
					default:
						*LPWORD(pDataPtr)=WORD(_readnum(16,pStr,4));;
						pStr--;
						break;
					}
					pDataPtr+=2;
				}
				else
				{	
					*((WCHAR*)pDataPtr)=*pStr;
					pDataPtr+=2;
				}
			}
			if (*pStr==')') 
				pStr++;
		}
		else
		{
			_allocmore(pData,pDataPtr,nAllocLen,1);
			LPCWSTR pStrOld=pStr;
			*pDataPtr=_readnum(16,pStr,2);
			if (pStr==pStrOld)
			{
				*pdwDataLength=DWORD(pDataPtr-pData);
				return pData;
			}
			pDataPtr++;
		}
		while (*pStr==' ') pStr++;
	}
	*pdwDataLength=DWORD(pDataPtr-pData);
	return pData;
}

#endif

BOOL IsCharNumeric(char cChar,BYTE bBase)
{
	if (bBase<=10)
	{
		if (cChar>='0' && cChar<='0'+bBase-1)
			return TRUE;
		return FALSE;
	}
	if (cChar>='0' && cChar<='9')
		return TRUE;
	bBase-=11;
	if (cChar>='A' && cChar<='A'+bBase)
		return TRUE;
	if (cChar>='a' && cChar<='a'+bBase)
		return TRUE;
	return FALSE;
}


inline void* va_getarg(va_list argList,int count)
{
	for (int i=0;i<count-1;i++)
		va_arg(argList,void*);
	return va_arg(argList,void*);
}



int vsprintfex( char *buffer, int buffersize,const char *format, va_list argList )
{
	int ptr=0;
	const char* in=format;
	LPSTR end;

	int nNextArg=0,index,length;

    while (*in!='\0')
	{
		if (*in=='%')
		{
			in++;
			
			// Finding first non number
			for(index=0;in[index]>='0' && in[index]<='9';index++);

			// Now index points to nonnumberic character
			if (in[index]==':')
			{
				// ok number was argument place
				if (index==0)
					return 0;
				nNextArg=atoi(in);
                
				// finding next '%'
				in+=index+1;
				for (length=0;in[length]!='\0' && in[length]!='%';length++);

				char* pTemp=new char[length+2];
				pTemp[0]='%';
				CopyMemory(pTemp+1,in,length);
				pTemp[length+1]='\0';
				HRESULT hRes=StringCbPrintfExA(buffer+ptr,buffersize-ptr,&end,NULL,STRSAFE_IGNORE_NULLS,pTemp,va_getarg(argList,nNextArg));
				if (FAILED(hRes))
					return 0;
                ptr=int(end-buffer);
				delete[] pTemp;

				if (in[length]=='\0')
					break;
				
				in+=length;
			}
			else
			{
				nNextArg++;
				for (;in[index]!='\0' && in[index]!='%';index++);
			

				char* pTemp=new char[index+2];
				pTemp[0]='%';
				CopyMemory(pTemp+1,in,index);
				pTemp[index+1]='\0';
				HRESULT hRes=StringCbPrintfExA(buffer+ptr,buffersize-ptr,&end,NULL,STRSAFE_IGNORE_NULLS,pTemp,va_getarg(argList,nNextArg));
				if (FAILED(hRes))
					return 0;
                ptr=int(end-buffer);
				delete[] pTemp;
				
				if (in[index]=='\0')
					break;
				
				in+=index;                
			}
		}
		else
			buffer[ptr++]=*(in++);
	}
	buffer[ptr]='\0';
	return ptr;
}


#ifdef DEF_WCHAR
int vswprintfex( wchar_t *buffer, int buffersize, const wchar_t *format, va_list argList )
{
	int ptr=0;
	const wchar_t* in=format;
	LPWSTR end;

	int nNextArg=0,index,length;

    while (*in!='\0')
	{
		if (*in=='%')
		{
			in++;
			
			// Finding first non number
			for(index=0;in[index]>=L'0' && in[index]<=L'9';index++);

			// Now index points to nonnumberic character
			if (in[index]==L':')
			{
				// ok number was argument place
				if (index==0)
					return 0;
				nNextArg=_wtoi(in);
                
				// finding next '%'
				in+=index+1;
				for (length=0;in[length]!=L'\0' && in[length]!=L'%';length++);

				wchar_t* pTemp=new wchar_t[length+2];
				pTemp[0]=L'%';
				MemCopyW(pTemp+1,in,length);
				pTemp[length+1]=L'\0';
				HRESULT hRes=StringCbPrintfExW(buffer+ptr,(buffersize-ptr)*sizeof(wchar_t),&end,
					NULL,STRSAFE_IGNORE_NULLS,pTemp,va_getarg(argList,nNextArg));
				if (FAILED(hRes))
					return 0;
                ptr=int(end-buffer);
				delete[] pTemp;

				if (in[length]==L'\0')
					break;
				
				in+=length;
			}
			else
			{
				nNextArg++;
				for (;in[index]!=L'\0' && in[index]!=L'%';index++);
			

				wchar_t* pTemp=new wchar_t[index+2];
				pTemp[0]='%';
				MemCopyW(pTemp+1,in,index);
				pTemp[index+1]=L'\0';
				
                
				HRESULT hRes=StringCbPrintfExW(buffer+ptr,(buffersize-ptr)*sizeof(wchar_t),&end,
					NULL,STRSAFE_IGNORE_NULLS,pTemp,va_getarg(argList,nNextArg));
				if (FAILED(hRes))
					return 0;
                ptr=int(end-buffer);

				delete[] pTemp;
				
				if (in[index]==L'\0')
					break;
				
				in+=index;                
			}
		}
		else
			buffer[ptr++]=*(in++);
	}
	buffer[ptr]='\0';
	return ptr;
}
#endif


#ifdef DEF_WCHAR
int LoadString(UINT uID,LPWSTR lpBuffer,int nBufferMax)
{
	if (!IsUnicodeSystem())
	{
		char* pStr=new char[nBufferMax+1];
		int nRet=::LoadStringA(GetLanguageSpecificResourceHandle(),uID,pStr,nBufferMax);
		MultiByteToWideChar(CP_ACP,0,pStr,nRet+1,lpBuffer,nBufferMax);
		delete[] pStr;
		return nRet;
	}
	return (int)::LoadStringW(GetLanguageSpecificResourceHandle(),uID,lpBuffer,nBufferMax);
}

int LoadString(UINT uID,LPWSTR lpBuffer,int nBufferMax,TypeOfResourceHandle bType)
{
	if (!IsUnicodeSystem())
	{
		char* pStr=new char[nBufferMax+1];
		int nRet=::LoadStringA(GetResourceHandle(bType),uID,pStr,nBufferMax);
		MultiByteToWideChar(CP_ACP,0,pStr,nRet+1,lpBuffer,nBufferMax);
		delete[] pStr;
		return nRet;
	}
	return (int)::LoadStringW(GetResourceHandle(bType),uID,lpBuffer,nBufferMax);
}

int LoadString(UINT uID,LPWSTR lpBuffer,int nBufferMax,HINSTANCE hInstance)
{
	if (!IsUnicodeSystem())
	{
		char* pStr=new char[nBufferMax+1];
		int nRet=::LoadStringA(hInstance,uID,pStr,nBufferMax);
		MultiByteToWideChar(CP_ACP,0,pStr,nRet+1,lpBuffer,nBufferMax);
		delete[] pStr;
		return nRet;
	}
	return (int)::LoadStringW(hInstance,uID,lpBuffer,nBufferMax);
}
#endif



LPSTR allocstring(UINT nID,TypeOfResourceHandle bType)
{
	LPSTR szBuffer;
	szBuffer=new CHAR[STR_LOADSTRINGBUFLEN];
	if (szBuffer==NULL)
	{
		SetHFCError(HFC_CANNOTALLOCATE);
		return FALSE;
	}
	UINT nDataLen=::LoadStringA(GetResourceHandle(bType),nID,szBuffer,STR_LOADSTRINGBUFLEN);
	if (nDataLen>=STR_LOADSTRINGBUFLEN-2)
	{
		for (DWORD i=2;nDataLen>=i*STR_LOADSTRINGBUFLEN-2;i++)
		{
			delete[] szBuffer;
			szBuffer=new CHAR[i*STR_LOADSTRINGBUFLEN];
			nDataLen=::LoadStringA(GetResourceHandle(bType),nID,szBuffer,i*STR_LOADSTRINGBUFLEN);
		}
	}

	char* pText=new char[nDataLen+1];
	MemCopy(pText,szBuffer,nDataLen+1);
	delete[] szBuffer;
	return pText;
}

ID2A::ID2A(UINT nID,TypeOfResourceHandle bType)
{
	pStr=new CHAR[STR_LOADSTRINGBUFLEN];
	if (pStr==NULL)
	{
		SetHFCError(HFC_CANNOTALLOCATE);
		return;
	}
	UINT nDataLen=::LoadStringA(GetResourceHandle(bType),nID,pStr,STR_LOADSTRINGBUFLEN);
	if (nDataLen>=STR_LOADSTRINGBUFLEN-2)
	{
		for (DWORD i=2;nDataLen>=i*STR_LOADSTRINGBUFLEN-2;i++)
		{
			delete[] pStr;
			pStr=new CHAR[i*STR_LOADSTRINGBUFLEN];
			nDataLen=::LoadStringA(GetResourceHandle(bType),nID,pStr,i*STR_LOADSTRINGBUFLEN);
		}
	}
}

#ifdef DEF_WCHAR
LPWSTR allocstringW(UINT nID,TypeOfResourceHandle bType)
{
	LPWSTR szBuffer;
	szBuffer=new WCHAR[STR_LOADSTRINGBUFLEN];
	if (szBuffer==NULL)
	{
		SetHFCError(HFC_CANNOTALLOCATE);
		return NULL;
	}
	UINT nDataLen=::LoadString(nID,szBuffer,STR_LOADSTRINGBUFLEN,bType);
	if (nDataLen>=STR_LOADSTRINGBUFLEN-2)
	{
		for (DWORD i=2;nDataLen>=i*STR_LOADSTRINGBUFLEN-2;i++)
		{
			delete[] szBuffer;
			szBuffer=new WCHAR[i*STR_LOADSTRINGBUFLEN];
			nDataLen=::LoadString(nID,szBuffer,i*STR_LOADSTRINGBUFLEN,bType);
		}
	}

	WCHAR* pText=new WCHAR[nDataLen+1];
	MemCopy(pText,szBuffer,(nDataLen+1)*2);
	delete[] szBuffer;
	return pText;
}


ID2W::ID2W(UINT nID,TypeOfResourceHandle bType)
{
	pWStr=new WCHAR[STR_LOADSTRINGBUFLEN];
	if (pWStr==NULL)
	{
		SetHFCError(HFC_CANNOTALLOCATE);
		return;
	}
	UINT nDataLen=::LoadString(nID,pWStr,STR_LOADSTRINGBUFLEN,bType);
	if (nDataLen>=STR_LOADSTRINGBUFLEN-2)
	{
		for (DWORD i=2;nDataLen>=i*STR_LOADSTRINGBUFLEN-2;i++)
		{
			delete[] pWStr;
			pWStr=new WCHAR[i*STR_LOADSTRINGBUFLEN];
			nDataLen=::LoadString(nID,pWStr,i*STR_LOADSTRINGBUFLEN,bType);
		}
	}
}

#endif