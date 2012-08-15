////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////
// Inline funktions for manipulating strings
////////////////////////////////////////////////////////////////////

#ifndef HFCSTR_INL
#define HFCSTR_INL

template<class CHARTYPE> 
inline int istrlen(const CHARTYPE* str)
{
	int len;
	for (len=0;(str)[len]!='\0';len++);
	return len;
}



#ifdef DEF_WCHAR
#define istrlenw istrlen

inline char W2Ac(WCHAR ch)
{
	char ret=0;
	WideCharToMultiByte(CP_ACP,0,&ch,1,&ret,1,NULL,NULL);
	return ret;
}


inline WCHAR A2Wc(char ch)
{
	WCHAR ret=0;
	MultiByteToWideChar(CP_ACP,0,&ch,1,&ret,1);
	return ret;
}

inline BOOL IsCharLower(WCHAR ch)
{
	if (IsUnicodeSystem())
		return IsCharLowerW(ch);
	else
		return iswlower(ch);
}

inline BOOL IsCharUpper(WCHAR ch)
{
	if (IsUnicodeSystem())
		return IsCharUpperW(ch);
	else
		return iswupper(ch);
}
#endif


inline void MemCopyW(LPWSTR dst,LPCWSTR src,int len)
{
	sMemCopy((LPVOID)dst,(LPCVOID)src,len*sizeof(WCHAR));
}

inline int LenWtoA(LPCWSTR src)
{
	int nLen=WideCharToMultiByte(CP_ACP,0,src,-1,NULL,0,NULL,NULL);
	return nLen>0?nLen-1:0;
}

inline int LenWtoA(LPCWSTR src,int len)
{
	return WideCharToMultiByte(CP_ACP,0,src,len,NULL,0,NULL,NULL);
}

inline int LenAtoW(LPCSTR src)
{
	int nLen=MultiByteToWideChar(CP_ACP,0,src,-1,NULL,0);
	return nLen>0?nLen-1:0;
}

inline int LenAtoW(LPCSTR src,int len)
{
	return MultiByteToWideChar(CP_ACP,0,src,len,NULL,0);
}

// copyers with allocation
inline char* alloccopy(const char* szString)
{
	int nLength=istrlen(szString);
	char* psz=new char[max(nLength,1)+1];
	CopyMemory(psz,szString,(nLength+1)*sizeof(char));
	return psz;
}

inline char* alloccopy(const char* szString,DWORD dwLength)
{
	char* psz=new char[max(dwLength,1)+1];
	CopyMemory(psz,szString,dwLength*sizeof(char));
	psz[dwLength]='\0';
	return psz;
}

inline LPSTR allocempty()
{
	char* psz=new char[2];
	*psz='\0';
	return psz;
}

template <class CHARTYPE>
inline CHARTYPE* alloccopymulti(const CHARTYPE* szMultiString)
{
	int nTotLen;
	for (nTotLen=0;szMultiString[nTotLen]!='\0' || szMultiString[nTotLen+1]!='\0';nTotLen++);
	CHARTYPE* psz=new CHARTYPE[nTotLen];
	CopyMemory(psz,szMultiString,(++nTotLen)*sizeof(CHARTYPE));
	return psz;
}

inline char* alloccopy(CString& sString)
{
	if (sString.m_nDataLen==0)
		return allocempty();
	char* psz=new char[sString.m_nDataLen+1];
	CopyMemory(psz,sString.m_pData,sString.m_nDataLen+1);
	return psz;
}


#ifdef DEF_WCHAR
inline WCHAR* alloccopy(const WCHAR* szString)
{
	int nLength=istrlen(szString);
	WCHAR* psz=new WCHAR[max(nLength,1)+1];
	CopyMemory(psz,szString,(nLength+1)*sizeof(WCHAR));
	return psz;
}

inline WCHAR* alloccopy(const WCHAR* szString,DWORD dwLength)
{
	WCHAR* psz=new WCHAR[max(dwLength,1)+1];
	CopyMemory(psz,szString,dwLength*sizeof(WCHAR));
	psz[dwLength]='\0';
	return psz;
}

inline LPWSTR allocemptyW()
{
	WCHAR* psz=new WCHAR[2];
	*psz=L'\0';
	return psz;
}

inline WCHAR* alloccopy(CStringW& sString)
{
	if (sString.m_nDataLen==0)
		return allocemptyW();
	WCHAR* psz=new WCHAR[sString.m_nDataLen+1];
	MemCopyW(psz,sString.m_pData,sString.m_nDataLen+1);
	return psz;
}

#endif




inline void MakeLower(LPSTR szStr)
{
#ifdef WIN32
	CharLower(szStr);
#else
	strlwr(szStr);
#endif
}

inline void MakeUpper(LPSTR szStr)
{
#ifdef WIN32
	CharUpper(szStr);
#else
	strupr(szStr);
#endif
}
inline void MakeLower(LPSTR szStr,DWORD cchLength)
{
#ifdef WIN32
	CharLowerBuff(szStr,cchLength);
#else
	char* pTmp=new char[cchLength+2];
	MemCopy(pTmp,szStr,cchLength);
	pTmp[cchLength];
	strlwr(pTmp);
	MemCopy(szStr,pTmp,cchLength);
	delete[] pTmp;
#endif
}
inline void MakeUpper(LPSTR szStr,DWORD cchLength)
{
#ifdef WIN32
	CharUpperBuff(szStr,cchLength);
#else
	char* pTmp=new char[cchLength+2];
	MemCopy(pTmp,szStr,cchLength);
	pTmp[cchLength];
	strupr(pTmp);
	MemCopy(szStr,pTmp,cchLength);
	delete[] pTmp;
#endif
}


#ifdef DEF_WCHAR
inline void MakeLower(LPWSTR szStr)
{
	if (IsUnicodeSystem())
		CharLowerW(szStr);
	else
		_wcslwr_s(szStr,istrlenw(szStr)+1);
}

inline void MakeLower(LPWSTR szStr,DWORD cchLength)
{
	if (IsUnicodeSystem())
		CharLowerBuffW(szStr,cchLength);
	else
	{
		WCHAR* pTemp=alloccopy(szStr,cchLength);
		_wcslwr_s(pTemp,cchLength+1);
		CopyMemory(szStr,pTemp,cchLength*2);
		delete[] pTemp;
	}
}

inline void MakeUpper(LPWSTR szStr)
{
	if (IsUnicodeSystem())
		CharUpperW(szStr);
	else
		_wcsupr_s(szStr,istrlenw(szStr)+1);
}
inline void MakeUpper(LPWSTR szStr,DWORD cchLength)
{
	if (IsUnicodeSystem())
		CharUpperBuffW(szStr,cchLength);
	else
	{
		WCHAR* pTemp=alloccopy(szStr,cchLength);
		_wcsupr_s(pTemp,cchLength+1);
		CopyMemory(szStr,pTemp,cchLength*2);
		delete[] pTemp;
	}
}
#endif




// char replacers
template<class CHARTYPE>
inline void replacech(CHARTYPE* str,CHARTYPE from,CHARTYPE to)
{
	for (register UINT i=0;str[i]!='\0';i++)
	{
		if (str[i]==from)
			str[i]=to;
	}
}



template<class CHARTYPE>
int FirstCharIndex(const CHARTYPE* str,CHARTYPE ch)
{
	int i;
	for (i=0;str[i]!='\0';i++)
	{
		if (str[i]==ch)
			return i;
	}
	return -1;
}

template<class CHARTYPE>
int FirstCharIndex(const CHARTYPE* str,CHARTYPE ch,int iLength)
{
	int i;
	for (i=0;i<iLength;i++)
	{
		if (str[i]==ch)
			return i;
	}
	return -1;
}

template<class CHARTYPE>
int LastCharIndex(const CHARTYPE* str,CHARTYPE ch)
{
	int i,ret=-1;
	for (i=0;str[i]!='\0';i++)
	{
		if (str[i]==ch)
			ret=i;
	}
	return ret;
}

template<class CHARTYPE>
int LastCharIndex(const CHARTYPE* str,CHARTYPE ch,int iLength)
{
	int i;
	for (i=iLength-1;i>=0 && str[i]!=ch;i--);
	return i;
}

template<class CHARTYPE>
int NextCharIndex(const CHARTYPE* str,CHARTYPE ch,int oldidx)
{
   int i;
   for (i=oldidx+1;str[i]!='\0';i++)
   {
      if (str[i]==ch)
         return i;
   }
   return -1;
}

template<class CHARTYPE>
int NextCharIndex(const CHARTYPE* str,CHARTYPE ch,int oldidx,int iLength)
{
   int i;
   for (i=oldidx+1;i<iLength;i++)
   {
      if (str[i]==ch)
         return i;
   }
   return -1;
}

inline int sprintfex( char *buffer, int buffersize, const char *format,...)
{
	va_list argList;
	va_start(argList,format);
	int nRet=vsprintfex(buffer,buffersize,format,argList);
	va_end(argList);
	return nRet;
}

#ifdef DEF_WCHAR
inline int swprintfex( wchar_t *buffer, int buffersize, const wchar_t *format,...)
{
	va_list argList;
	va_start(argList,format);
	int nRet=vswprintfex(buffer,buffersize,format,argList);
	va_end(argList);
	return nRet;
}
#endif



////////////////////////////////////////////////////////////////
// dataparsers

template<class TYPE>
inline bool _1stcontain2nd(const TYPE* s1,const TYPE* s2)
{
	for (register int i=0;s2[i]!='\0';i++)
		if (s1[i]!=s2[i])
			return FALSE;
	return TRUE;
}

// szString will change
template<class CHARTYPE>
int _readnum(int base,const CHARTYPE*& str,int length=-1)
{
	int num=0;
	BOOL bToNegative=FALSE;
	for (;*str=='-';str++)
		bToNegative=!bToNegative;
	if (length==-1)
		length=istrlen(str);

	if (base==16)
	{
		for (;*str!='\0' && length>0;length--,str++)
		{
			if (*str>='0' && *str<='9')
			{
				num<<=4;
				num+=*str-'0';
			}
			else if (*str>='a' && *str<='f')
			{
				num<<=4;
				num+=*str-'a'+0xa;
			}
			else if (*str>='A' && *str<='F')
			{
				num<<=4;
				num+=*str-'A'+0xa;
			}
			else
				break;
		}
		if (bToNegative)
			return -num;
		return num;
	}
	for (;*str!='\0' && length>0;length--,str++)
	{
		int n=chartonum(*str);
		if (n>=base)
			break;
		if (n==0 && *str!='0')
			break;
		num*=base;
		num+=n;
	}
	if (bToNegative)
		return -num;
	return num;
}

template<class CHARTYPE>
// szString will not change
inline int readnum(int base,const CHARTYPE*& str,int length)
{
	return _readnum(base,str,length);
}

template<class CHARTYPE>
inline int chartonum(CHARTYPE ch)
{
	if (ch>='0' && ch<='9')
		return ch-'0';
	if (ch>='a' && ch<='z')
		return ch-'a'+10;
	if (ch>='A' && ch<='Z')
		return ch-'a'+10;
	return 0;
}

template<class CHARTYPE> 
inline int parseto(const CHARTYPE* str,CHARTYPE ch)
{
	int len;
	for (len=0;(str)[len]!=(ch);len++);
	return len;
}

template<class CHARTYPE> 
inline int parseto2(const CHARTYPE* str,CHARTYPE ch1,CHARTYPE ch2)
{
	int len;
	for (len=0;str[len]!=ch1 && str[len]!=ch2;len++);
	return len;
}

template<class CHARTYPE> 
inline int parseto3(const CHARTYPE* str,CHARTYPE ch1,CHARTYPE ch2,CHARTYPE ch3)
{
	int len;
	for (len=0;str[len]!=ch1 && str[len]!=ch2 && str[len]!=ch3;len++);
	return len;
}

template<class CHARTYPE> 
inline int parseto4(const CHARTYPE* str,CHARTYPE ch1,CHARTYPE ch2,CHARTYPE ch3,CHARTYPE ch4)
{
	int len;
	for (len=0;str[len]!=ch1 && str[len]!=ch2 && str[len]!=ch3 && str[len]!=ch4;len++);
	return len;
}



template<class CHARTYPE>
inline BYTE* dataparser(const CHARTYPE* pString,DWORD dwStrLen,DWORD* pdwDataLength=NULL)
{
	return dataparser(pString,dwStrLen,malloc,pdwDataLength);
}

template<class CHARTYPE>
inline BYTE* dataparser(const CHARTYPE* str,DWORD* pdwDataLength=NULL)
{
	return dataparser(str,istrlen(str),pdwDataLength);
}

inline BYTE* dataparser(const CString& str,DWORD* pdwDataLength=NULL)
{
	return dataparser(LPCSTR(str),str.GetLength(),pdwDataLength);
}


#ifdef DEF_WCHAR
inline BYTE* dataparser(const CStringW& str,DWORD* pdwDataLength=NULL)
{
	return dataparser(LPCWSTR(str),str.GetLength(),pdwDataLength);
}
#endif





////////////////////////////////////////////////////////////////////////////
// Classes
////////////////////////////////////////////////////////////////////////////


#ifdef DEF_WCHAR

///////////////////////////////////////////
// W2A

inline W2A::W2A(LPCWSTR sA)
{
	pAStr=alloccopyWtoA(sA);
}

inline W2A::W2A(LPCWSTR sA,int len)
{
	pAStr=alloccopyWtoA(sA,len);
}


inline W2A::W2A(CStringW& sA)
{
	pAStr=alloccopyWtoA(sA,sA.GetLength());
}


inline W2A::~W2A()
{
	if (pAStr!=NULL)
		delete[] pAStr;
}

inline W2A::operator LPCSTR() const
{
	return pAStr;
}

///////////////////////////////////////////
// W2UTF8

inline W2UTF8::W2UTF8(LPCWSTR sA)
{
	int nLen=WideCharToMultiByte(CP_UTF8,0,sA,-1,NULL,0,NULL,NULL);
	pAStr=new char[nLen+1];
	WideCharToMultiByte(CP_UTF8,0,sA,-1,pAStr,nLen+1,NULL,NULL);
}

inline W2UTF8::W2UTF8(LPCWSTR sA,int len)
{
	int nLen=WideCharToMultiByte(CP_UTF8,0,sA,len,NULL,0,NULL,NULL);
	pAStr=new char[nLen+1];
	WideCharToMultiByte(CP_UTF8,0,sA,-1,pAStr,nLen+1,NULL,NULL);
	pAStr[nLen]='\0';
}


inline W2UTF8::W2UTF8(CStringW& sA)
{
	int nLen=WideCharToMultiByte(CP_UTF8,0,sA,sA.GetLength(),NULL,0,NULL,NULL);
	pAStr=new char[nLen+1];
	WideCharToMultiByte(CP_UTF8,0,sA,-1,pAStr,nLen+1,NULL,NULL);
	pAStr[nLen]='\0';
}


inline W2UTF8::~W2UTF8()
{
	if (pAStr!=NULL)
		delete[] pAStr;
}

inline W2UTF8::operator LPCSTR() const
{
	return pAStr;
}
///////////////////////////////////////////
// A2W

inline A2W::A2W(LPCSTR sA)
{
	pWStr=alloccopyAtoW(sA);
}

inline A2W::A2W(LPCSTR sA,int len)
{
	pWStr=alloccopyAtoW(sA,len);
}


inline A2W::A2W(CString& sA)
{
	pWStr=alloccopyAtoW(sA,sA.GetLength());
}

inline A2W::~A2W()
{
	if (pWStr!=NULL)
		delete[] pWStr;
}

inline A2W::operator LPCWSTR() const
{
	return pWStr;
}

#endif



///////////////////////////////////////////
// ID2A

inline ID2A::~ID2A()
{
	delete[] pStr;
}

inline ID2A::operator LPCSTR() const
{
	return pStr;
}

///////////////////////////////////////////
// ID2W

#ifdef DEF_WCHAR
inline ID2W::~ID2W()
{
	delete[] pWStr;
}

inline ID2W::operator LPCWSTR() const
{
	return pWStr;
}
#endif





///////////////////////////////////////////
// CString

inline CString::CString()
:	m_pData(NULL),m_nDataLen(0),m_nAllocLen(0),m_nBase(10)
{
}

inline CString::~CString()
{
	if (m_pData!=NULL)
		delete[] m_pData;
}



inline void CString::Empty()
{
	if (m_pData!=NULL)
	{
		delete[] m_pData;
		m_pData=NULL;
		m_nDataLen=0;
		m_nAllocLen=0;
	}
}

inline CString& CString::SetBase(BYTE nBase)
{
	if (nBase>=2 && nBase<=36)
		m_nBase=nBase;
	return *this;
}

inline CHAR CString::GetAt(int nIndex) const
{
	if (nIndex<m_nDataLen && m_pData!=NULL)
		return m_pData[nIndex];
	else
		return 0;
}



inline CHAR CString::operator[](int nIndex) const
{
	if (nIndex>=0 && nIndex<m_nAllocLen && m_pData!=NULL)
		return m_pData[nIndex];
	else
		return 0;
}

inline CHAR CString::operator[](LONG nIndex) const
{
	if (nIndex>=0 && nIndex<m_nAllocLen && m_pData!=NULL)
		return m_pData[nIndex];
	else
		return 0;
}


inline CHAR CString::operator[](DWORD nIndex) const
{
	if (nIndex<(DWORD)m_nAllocLen && m_pData!=NULL)
		return m_pData[nIndex];
	else
		return 0;
}

inline CHAR CString::operator[](UINT nIndex) const
{
	if (nIndex<(UINT)m_nAllocLen && m_pData!=NULL)
		return m_pData[nIndex];
	else
		return 0;
}

#ifdef _WIN64
inline CHAR CString::operator[](ULONG_PTR nIndex) const
{
	if (nIndex<m_nAllocLen && m_pData!=NULL)
		return m_pData[nIndex];
	else
		return 0;
}

inline CHAR CString::operator[](LONG_PTR nIndex) const
{
	if (nIndex>=0 && (ULONG_PTR)nIndex<m_nAllocLen && m_pData!=NULL)
		return m_pData[nIndex];
	else
		return 0;
}
#endif

#ifdef DEF_WCHAR
inline const CString& CString::operator=(LPCWSTR str)
{
	return Copy(str);
}
#endif

inline const CString& CString::operator=(LPCSTR str)
{
	return Copy(str);
}

inline const CString& CString::operator=(unsigned char * str)
{
	return Copy(str);
}

inline void CString::SetAt(int nIndex,CHAR ch)
{
	if (nIndex<m_nAllocLen)
		m_pData[nIndex]=ch;
}

inline CString CString::operator+(const CString& str)
{
	CString temp(m_pData);
	temp+=str;
	return temp;
}

inline CString CString::operator+(const LPCSTR str)
{
	CString temp(m_pData);
	temp+=str;
	return temp;
}

inline CString CString::operator+(const CHAR ch)
{
	CString temp(m_pData);
	temp+=ch;
	return temp;
}

#ifdef DEF_WCHAR
inline CString CString::operator+(const CStringW& str)
{
	CString temp(m_pData);
	temp+=str;
	return temp;
}

inline CString CString::operator+(const LPCWSTR str)
{
	CString temp(m_pData);
	temp+=str;
	return temp;
}

inline CString CString::operator+(const WCHAR ch)
{
	CString temp(m_pData);
	temp+=ch;
	return temp;
}
#endif
 

inline void CString::Append(const CString& str)
{
	*this << str;
}


inline void CString::Append(CHAR ch)
{
	*this << ch;
}

#ifdef DEF_WCHAR
inline void CString::Append(const CStringW& str)
{
	*this << str;
}

inline void CString::Append(WCHAR ch)
{
	*this << ch;
}
#endif 

inline void CString::MakeUpper()
{
	if (m_pData!=NULL)
		CharUpperA(m_pData);
}

inline void CString::MakeLower()
{
	if (m_pData!=NULL)
		CharLower(m_pData);
}

inline BOOL CString::operator==(LPCSTR str)
{
	return (Compare(str)==0);
}

#ifdef DEF_WCHAR
inline BOOL CString::operator==(LPCWSTR str)
{
	return (Compare(str)==0);
}
#endif

inline CString CString::Mid(int nFirst,int nCount) const
{
	if (nFirst+nCount<m_nDataLen)
		return CString (&m_pData[nFirst],nCount);
	else
		return CString (&m_pData[m_nDataLen-nCount]);
}

inline CString CString::Mid(int nFirst) const
{
	if (m_nDataLen>nFirst)
		return CString(&m_pData[nFirst]);
	else
		return CString(&m_pData[m_nDataLen]);
}

inline CString CString::Left(int nCount) const
{
	return CString(m_pData,nCount);
}

inline CString CString::Right(int nCount) const
{
	return CString(&m_pData[m_nDataLen-nCount]);
}

#ifdef DEF_RESOURCES
inline BOOL CString::AddString(UINT nID)
{
	ID2A str((UINT)nID);
	*this << (LPCSTR)str;
	return TRUE;
}

inline BOOL CString::AddString(UINT nID,TypeOfResourceHandle bType)
{
	ID2A str((UINT)nID,bType);
	*this << (LPCSTR)str;
	return TRUE;
}
#ifdef DEF_WCHAR
inline BOOL CStringW::AddString(UINT nID)
{
	ID2W str((UINT)nID);
	*this << (LPCWSTR)str;
	return TRUE;
}

inline BOOL CStringW::AddString(UINT nID,TypeOfResourceHandle bType)
{
	ID2W str((UINT)nID,bType);
	*this << (LPCWSTR)str;
	return TRUE;
}
#endif
#endif

inline CHAR CString::LastChar() const
{
	return m_nDataLen>0?m_pData[m_nDataLen-1]:'\0';
}

inline void CString::DelLastChar()
{
	m_pData[--m_nDataLen]='\0';
}	

inline LPSTR CString::GiveBuffer()
{
	LPSTR pRet=m_pData;
	m_pData=NULL;
	m_nDataLen=0;
	m_nAllocLen=0;
	return pRet;
}

///////////////////////////////////////////
// inlines for CStringW

#ifdef DEF_WCHAR

inline CStringW::CStringW()
:	m_pData(NULL),m_nDataLen(0),m_nAllocLen(0),m_nBase(10)
{
}

inline CStringW::~CStringW()
{
	if (m_pData!=NULL)
		delete[] m_pData;
}

inline void CStringW::Empty()
{
	if (m_pData!=NULL)
	{
		delete[] m_pData;
		m_pData=NULL;
		m_nDataLen=0;
		m_nAllocLen=0;
	}
}

inline CStringW& CStringW::SetBase(BYTE nBase)
{
	if (nBase>=2 && nBase<=36)
		m_nBase=nBase;
	return *this;
}

inline WCHAR CStringW::GetAt(int nIndex) const
{
	if (nIndex<m_nAllocLen && m_pData!=NULL)
		return m_pData[nIndex];
	else
		return 0;
}



inline WCHAR CStringW::operator[](int nIndex) const
{
	if (nIndex>=0 && nIndex<m_nAllocLen && m_pData!=NULL)
		return m_pData[nIndex];
	else
		return 0;
}

inline WCHAR CStringW::operator[](LONG nIndex) const
{
	if (nIndex>=0 && nIndex<m_nAllocLen && m_pData!=NULL)
		return m_pData[nIndex];
	else
		return 0;
}

inline WCHAR CStringW::operator[](DWORD nIndex) const
{
	if (nIndex<(DWORD)m_nAllocLen && m_pData!=NULL)
		return m_pData[nIndex];
	else
		return 0;
}


inline WCHAR CStringW::operator[](UINT nIndex) const
{
	if (nIndex<(UINT)m_nAllocLen && m_pData!=NULL)
		return m_pData[nIndex];
	else
		return 0;
}

#ifdef _WIN64
inline WCHAR CStringW::operator[](ULONG_PTR nIndex) const
{
	if (nIndex<m_nAllocLen && m_pData!=NULL)
		return m_pData[nIndex];
	else
		return 0;
}

inline WCHAR CStringW::operator[](LONG_PTR nIndex) const
{
	if (nIndex>=0 && nIndex<(LONG_PTR)m_nAllocLen && m_pData!=NULL)
		return m_pData[nIndex];
	else
		return 0;
}
#endif

inline void CStringW::SetAt(int nIndex,WCHAR ch)
{
	if (nIndex<m_nAllocLen)
		m_pData[nIndex]=ch;
}

inline CStringW::operator LPCWSTR() const
{
	if (m_pData==NULL)
		return szwEmpty;
	return m_pData;
}

inline CStringW CStringW::operator+(const CStringW& str)
{
	CStringW temp(m_pData);
	temp+=str;
	return temp;
}

inline CStringW CStringW::operator+(const LPCWSTR str)
{
	CStringW temp(m_pData);
	temp+=str;
	return temp;
}

inline CStringW CStringW::operator+(const WCHAR ch)
{
	CStringW temp(m_pData);
	temp+=ch;
	return temp;
}

inline CStringW CStringW::operator+(const CString& str)
{
	CStringW temp(m_pData);
	temp+=str;
	return temp;
}

inline CStringW CStringW::operator+(const LPCSTR str)
{
	CStringW temp(m_pData);
	temp+=str;
	return temp;
}

inline CStringW CStringW::operator+(const CHAR ch)
{
	CStringW temp(m_pData);
	temp+=ch;
	return temp;
}


inline void CStringW::Append(const CString& str)
{
	*this << str;
}

inline void CStringW::Append(const CStringW& str)
{
	*this << str;
}

inline void CStringW::Append(CHAR ch)
{
	*this << ch;
}

inline void CStringW::Append(WCHAR ch)
{
	*this << ch;
}

inline const CStringW& CStringW::operator=(LPCSTR str)
{
	return Copy(str);
}

inline const CStringW& CStringW::operator=(LPCWSTR str)
{
	return Copy(str);
}

inline const CStringW& CStringW::operator=(unsigned short * str)
{
	return Copy((LPWSTR)str);
}

inline BOOL CStringW::operator==(LPCWSTR str)
{
	return (Compare(str)==0);
}

inline BOOL CStringW::operator==(LPCSTR str)
{
	return (Compare(str)==0);
}

inline CStringW CStringW::Mid(int nFirst,int nCount) const
{
	if (nFirst+nCount<m_nDataLen)
		return CStringW (&m_pData[nFirst],nCount);
	else
		return CStringW (&m_pData[m_nDataLen-nCount]);
}

inline CStringW CStringW::Mid(int nFirst) const
{
	if (m_nDataLen>nFirst)
		return CStringW(&m_pData[nFirst]);
	else
		return CStringW(&m_pData[m_nDataLen]);
}

inline CStringW CStringW::Left(int nCount) const
{
	return CStringW(m_pData,nCount);
}

inline CStringW CStringW::Right(int nCount) const
{
	return CStringW(&m_pData[m_nDataLen-nCount]);
}


inline WCHAR CStringW::LastChar() const
{
	return m_nDataLen>0?m_pData[m_nDataLen-1]:'\0';
}

inline void CStringW::DelLastChar()
{
	m_pData[--m_nDataLen]='\0';
}	

inline LPWSTR CStringW::GiveBuffer()
{
	LPWSTR pRet=m_pData;
	m_pData=NULL;
	m_nDataLen=0;
	m_nAllocLen=0;
	return pRet;
}

#endif 



#endif
