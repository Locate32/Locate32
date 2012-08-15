////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////
// All definitions for manipulating strings
////////////////////////////////////////////////////////////////////

#ifndef HFCSTR_H
#define HFCSTR_H

#ifdef DEF_WCHAR
inline BOOL IsUnicodeSystem()
{
#if defined(_WIN64) // Good assumption
	return TRUE;
#elif defined(MICROSOFT_LAYER_FOR_UNICODE) 
	return TRUE;
#else
	extern BOOL bIsFullUnicodeSupport;
	return bIsFullUnicodeSupport;
#endif
}
#endif


////////////////////////////////////////
// Definitions

#define sMemCopy(dst,src,len)	CopyMemory(dst,src,len)
#define sMemZero(dst,len)		ZeroMemory(dst,len)
#define sMemSet(dst,val,len)	FillMemory(dst,len,val)

#define sMemCopyW				MemCopyW
#define sstrlenW				dwstrlen

#ifdef DEF_WCHAR


#define MemCopyWtoA(dst,dstlen,src,len) \
	WideCharToMultiByte(CP_ACP,0,(src),(int)(len),(dst),(int)(dstlen),NULL,NULL)
#define MemCopyAtoW(dst,dstlen,src,len) \
	MultiByteToWideChar(CP_ACP,0,(src),(int)(len),(dst),(int)(dstlen))

// No type check
#define _MemCopyW(dst,src,len) \
	sMemCopy((dst),(src),(len)*sizeof(WCHAR))
#define _MemCopyWtoA(dst,dstlen,src,len) \
	WideCharToMultiByte(CP_ACP,0,(LPCWSTR)(src),(int)(len),(LPSTR)(dst),(int)(dstlen),NULL,NULL)
#define _MemCopyAtoW(dst,dstlen,src,len) \
	MultiByteToWideChar(CP_ACP,0,(LPCSTR)(src),(int)(len),(LPWSTR)(dst),(int)(dstlen))
#endif




////////////////////////////////////////
// String functions

BOOL ContainString(LPCSTR,LPCSTR);
#ifdef DEF_WCHAR
BOOL ContainString(LPCWSTR,LPCWSTR);
BOOL ContainString(LPCSTR,LPCWSTR);
#endif

#ifdef WIN32
int strcasecmp(LPCSTR,LPCTSTR);
#endif
int strcasencmp(LPCSTR s1,LPCSTR s2,DWORD n);

#ifdef DEF_WCHAR
int strcasecmp(LPCWSTR,LPCWSTR);
int strcasencmp(LPCWSTR s1,LPCWSTR s2,DWORD n);

LPWSTR alloccopyAtoW(LPCSTR szString);
LPWSTR alloccopyAtoW(LPCSTR szString,int iLength);
LPWSTR alloccopyAtoW(LPCSTR szString,int iLength,int& iNewLen);
LPSTR alloccopyWtoA(LPCWSTR szString);
LPSTR alloccopyWtoA(LPCWSTR szString,int iLength);
LPSTR alloccopyWtoA(LPCWSTR szString,int iLength,int& iNewLen);
LPSTR alloccopymultiWtoA(LPCWSTR szMultiString);
LPWSTR alloccopymultiAtoW(LPCSTR szMultiString);

#endif

BYTE* dataparser(LPCSTR pString,DWORD dwStrLen,MALLOC_FUNC pMalloc,DWORD* pdwDataLength=NULL);
BYTE* dataparser2(LPCSTR pStr,DWORD* pdwDataLength);
BOOL IsCharNumeric(char cChar,BYTE bBase);

LPSTR allocstring(UINT nID,TypeOfResourceHandle bType=LanguageSpecificResource);

#ifdef DEF_WCHAR
BYTE* dataparser(LPCWSTR pString,DWORD dwStrLen,MALLOC_FUNC pMalloc,DWORD* pdwDataLength=NULL);
BYTE* dataparser2(LPCWSTR pStr,DWORD* pdwDataLength);

LPWSTR allocstringW(UINT nID,TypeOfResourceHandle bType=LanguageSpecificResource);
#endif


// Extended sprintf style handlers
int vsprintfex( char *buffer, int buffersize, const char *format, va_list argptr );
#ifdef DEF_WCHAR
int vswprintfex( wchar_t *buffer, int buffersize, const wchar_t *format, va_list argptr );
#endif



// Secure library extensions, let StringCBXXX does the job, because strxxx_s functions can't do that
inline errno_t _strcpy_s(LPSTR dst,size_t len,LPCSTR src)
{
	HRESULT hRet=StringCbCopyA(dst,len,src);
	ASSERT(hRet==S_OK);
	return (errno_t)hRet;
}

inline errno_t _strncpy_s(LPSTR dst,size_t len,LPCSTR src,size_t srclen)
{
	HRESULT hRet=StringCbCopyNA(dst,len,src,srclen);
	ASSERT(hRet==S_OK);
	return (errno_t)hRet;
}

inline errno_t _strcat_s(LPSTR dst,size_t len,LPCSTR src)
{
	HRESULT hRet=StringCbCatA(dst,len,src);
	ASSERT(hRet==S_OK);
	return (errno_t)hRet;
}

#ifdef DEF_WCHAR
inline errno_t _wcscpy_s(LPWSTR dst,size_t len,LPCWSTR src)
{
	HRESULT hRet=StringCbCopyW(dst,len*2,src);
	ASSERT(hRet==S_OK);
	return (errno_t)hRet;
}

inline errno_t _wcsncpy_s(LPWSTR dst,size_t len,LPCWSTR src,size_t srclen)
{
	HRESULT hRet=StringCbCopyNW(dst,len*2,src,srclen*2);
	ASSERT(hRet==S_OK);
	return (errno_t)hRet;
}

inline errno_t _wcscat_s(LPWSTR dst,size_t len,LPCWSTR src)
{
	HRESULT hRet=StringCbCatW(dst,len*2,src);
	ASSERT(hRet==S_OK);
	return (errno_t)hRet;
}
#endif

#ifndef DEF_NOSECUREOVERDRIVE
#define strcpy_s _strcpy_s
#define strncpy_s _strncpy_s
#define strcat_s _strcat_s


#ifdef DEF_WCHAR
#define wcscpy_s _wcscpy_s
#define wcsncpy_s _wcsncpy_s
#define wcscat_s _wcscat_s
#endif
#endif

//////////////////////////////////////////////////
// Class CString

#define CStringA CString
class CString
{
protected:
	LPSTR m_pData;
	int m_nDataLen;
	int m_nAllocLen;
	BYTE m_nBase;
public:
//Constructors
	CString();
	CString(const CString& str);
	CString(CHAR ch,int nRepeat=1);
	CString(LPCSTR lpsz);
	CString(LPCSTR lpsz,int nLength);
	CString(const unsigned char * lpsz);
	~CString();

public:
//Attributes & Operations
	int GetLength() const { return m_nDataLen; }
	BOOL IsEmpty() const { return (m_pData==NULL || !m_nDataLen); }
	void Empty();

	CString& SetBase(BYTE nBase);
	BYTE GetBase() { return m_nBase; }

	CHAR GetAt(int nIndex) const;
	CHAR operator[](int nIndex) const;
	CHAR& operator[](int nIndex);
	CHAR operator[](UINT nIndex) const;
	CHAR& operator[](UINT nIndex);
	CHAR operator[](DWORD nIndex) const;
	CHAR& operator[](DWORD nIndex);
	CHAR operator[](LONG nIndex) const;
	CHAR& operator[](LONG nIndex);
#ifdef _WIN64
	CHAR operator[](LONG_PTR nIndex) const;
	CHAR& operator[](LONG_PTR nIndex);
	CHAR operator[](ULONG_PTR nIndex) const;
	CHAR& operator[](ULONG_PTR nIndex);
#endif
	void SetAt(int nIndex,CHAR ch);
	
	inline operator LPCSTR() const	{ if (m_pData==NULL)	return szEmpty;	return m_pData;}
	LPSTR GetPData() const { return m_pData; } // Use with caution, may be null
	

	CString& Copy(LPCSTR src);
	CString& Copy(LPCSTR src,int iLength);
	CString& Copy(const BYTE* src);
	CString& Copy(const BYTE* src,int iLength);

	
	const CString& operator=(const CString& str);
	const CString& operator=(CHAR ch);
	const CString& operator=(LPCSTR str);
	const CString& operator=(unsigned char * str);
	const CString& operator=(DWORD iNum);
	const CString& operator=(int iNum);

	const CString& operator+=(const CString& str);
	const CString& operator+=(CHAR ch);
	const CString& operator+=(LPCSTR str);
	const CString& operator+=(DWORD iNum);
	const CString& operator+=(int iNum);
	const CString& operator+=(LONGLONG iNum);
	const CString& operator+=(ULONGLONG iNum);

	CString operator+(const CString& str);
	CString operator+(const LPCSTR str);
	CString operator+(const CHAR ch);

	CString& operator<<(const CString& str);
	CString& operator<<(CHAR ch);
	CString& operator<<(LPCSTR str);
	CString& operator<<(DWORD iNum);
	CString& operator<<(int iNum);
	CString& operator<<(LONGLONG iNum);
	CString& operator<<(ULONGLONG iNum);

	void Append(LPCSTR str,int iLength=-1);
    void Append(const CString& str);
    void Append(CHAR ch);

	int Compare(LPCSTR lpsz) const;
	int CompareNoCase(LPCSTR lpsz) const;

	BOOL operator==(const CString& str);
	BOOL operator==(LPCSTR str);
	BOOL ContainString(LPCSTR str,UINT start=0);

	CString Mid(int nFirst,int nCount) const;
	CString Mid(int nFirst) const;
	CString Left(int nCount) const;
	CString Right(int nCount) const;
	CHAR LastChar() const;
	void DelLastChar();

	void MakeUpper();
	void MakeLower();
	void MakeReverse();

	int Find(CHAR ch) const;
	int FindOneOf(LPCSTR lpszCharSet) const;
	int FindFirst(CHAR ch) const;
	int FindLast(CHAR ch) const;
	int FindNext(CHAR ch,int idx) const;
	int Find(LPCSTR lpszSub) const;

	LPSTR GetBuffer(int nMinBufLength=-1,BOOL bStoreData=FALSE);
	DWORD GetAllocLength() const { return m_nAllocLen; }
	void FreeExtra(int nNewLength=-1); // Call after GetBuffer()
	void Compact(); // Makes memory usage as small as possible
					// don't call after GetBuffer()
	LPSTR GiveBuffer();
	
	
		
	BOOL InsChar(int idx,CHAR ch);
	BOOL Insert(int idx,CHAR ch) { return InsChar(idx,ch); }
	BOOL DelChar(int idx);
	BOOL Delete(int idx,int nCount=1);
	void ReplaceChars(char from,char to);

	void Trim(); // Deletes spaces from begin and end

	void Swap(CString& str);

	void Format(LPCSTR lpszFormat,...);
	void FormatEx(LPCSTR lpszFormat,...);

	void FormatV(LPCSTR lpszFormat,va_list);
	
#ifdef DEF_RESOURCES
	CString(int nID);
	CString(int nID,TypeOfResourceHandle bType);
	CString(UINT nID);
	CString(UINT nID,TypeOfResourceHandle bType);
	
	CString(int nID,BOOL bLoadAsUnicodeIfPossible);
	CString(UINT nID,BOOL bLoadAsUnicodeIfPossible);
	
	void Format(UINT nFormatID,...);
	void FormatEx(UINT nFormatID,...);
	void FormatC(UINT nFormatID,...);
	void FormatExC(UINT nFormatID,...);
	
	BOOL LoadString(UINT nID);
	BOOL LoadString(UINT nID,TypeOfResourceHandle bType);
	BOOL AddString(UINT nID);
	BOOL AddString(UINT nID,TypeOfResourceHandle bType);
	
#endif
#ifdef WIN32
	void AnsiToOem();
	void OemToAnsi();
#endif	

#ifdef DEF_WCHAR	
	// Widechar support

	CString(const CStringW& str);
	CString(LPCWSTR lpsz);
	CString(WCHAR ch,int nRepeat=1);
	
	CString& Copy(LPCWSTR src);
	CString& Copy(LPCWSTR src,int iLength);

	const CString& operator=(const CStringW& str);
	const CString& operator=(WCHAR ch);
	const CString& operator=(LPCWSTR str);
	const CString& operator+=(const CStringW& str);
	const CString& operator+=(WCHAR ch);
	const CString& operator+=(LPCWSTR str);

	void Append(LPCWSTR str,int iLength=-1);
	void Append(const CStringW& str);
	void Append(WCHAR ch);

	CString operator+(const CStringW& str);
	CString operator+(const LPCWSTR str);
	CString operator+(const WCHAR ch);

	CString& operator<<(const CStringW& str);
	CString& operator<<(WCHAR ch);
	CString& operator<<(LPCWSTR str);

	int Compare(LPCWSTR lpsz) const;
	int CompareNoCase(LPCWSTR lpsz) const;

	BOOL operator==(const CStringW& str);
	BOOL operator==(LPCWSTR str);

	friend class CStringW;
	friend char* alloccopy(CString& sString);
#endif

#ifdef _DEBUG_LOGGING
	void DebugDumpInfo();
#else
	void DebugDumpInfo() {};
#endif
};


//////////////////////////////////////////////////
// Class CStringW

#ifdef DEF_WCHAR	

class CStringW
{
protected:
	LPWSTR m_pData;
	int m_nDataLen;
	int m_nAllocLen;
	BYTE m_nBase;
public:
//Constructors
	CStringW();
	CStringW(const CStringW& str);
	CStringW(WCHAR ch,int nRepeat=1);
	CStringW(LPCWSTR lpsz);
	CStringW(LPCWSTR lpsz,int nLength);
	CStringW(const short * lpsz);
	
	CStringW(const CString& str);
	CStringW(CHAR ch,int nRepeat=1);
	CStringW(LPCSTR lpsz);
	CStringW(LPCSTR lpsz,int nLength);
	
	~CStringW();

public:
//Attributes & Operations
	int GetLength() const { return m_nDataLen; }
	BOOL IsEmpty() const { return (m_pData==NULL || !m_nDataLen); }
	void Empty();

	CStringW& SetBase(BYTE nBase);
	BYTE GetBase() { return m_nBase; }

	WCHAR GetAt(int nIndex) const;
	WCHAR operator[](int nIndex) const;
	WCHAR& operator[](int nIndex);
	WCHAR operator[](UINT nIndex) const;
	WCHAR& operator[](UINT nIndex);
	WCHAR operator[](DWORD nIndex) const;
	WCHAR& operator[](DWORD nIndex);
	WCHAR operator[](LONG nIndex) const;
	WCHAR& operator[](LONG nIndex);
#ifdef _WIN64
	WCHAR operator[](LONG_PTR nIndex) const;
	WCHAR& operator[](LONG_PTR nIndex);
	WCHAR operator[](ULONG_PTR nIndex) const;
	WCHAR& operator[](ULONG_PTR nIndex);
#endif
	void SetAt(int nIndex,WCHAR ch);
	operator LPCWSTR() const;
	LPWSTR GetPData() const { return m_pData; } // Use with caution, may be null
	
	CStringW& Copy(LPCSTR src);
	CStringW& Copy(LPCSTR src,int iLength);
	CStringW& Copy(const BYTE* src);
	CStringW& Copy(const BYTE* src,int iLength);
	CStringW& Copy(LPCWSTR src);
	CStringW& Copy(LPCWSTR src,int iLength);
	
	const CStringW& operator=(const CStringW& str);
	const CStringW& operator=(WCHAR ch);
	const CStringW& operator=(LPCWSTR str);
	const CStringW& operator=(unsigned short * str);
	const CStringW& operator=(DWORD iNum);
	const CStringW& operator=(int iNum);
	const CStringW& operator=(const CString& str);
	const CStringW& operator=(CHAR ch);
	const CStringW& operator=(LPCSTR str);
	
	const CStringW& operator+=(const CStringW& str);
	const CStringW& operator+=(WCHAR ch);
	const CStringW& operator+=(LPCWSTR str);
	const CStringW& operator+=(DWORD iNum);
	const CStringW& operator+=(int iNum);
	const CStringW& operator+=(LONGLONG iNum);
	const CStringW& operator+=(ULONGLONG iNum);
	const CStringW& operator+=(const CString& str);
	const CStringW& operator+=(CHAR ch);
	const CStringW& operator+=(LPCSTR str);
	
	void Append(LPCSTR str,int iLength=-1);
    void Append(LPCWSTR str,int iLength=-1);
    void Append(const CString& str);
    void Append(const CStringW& str);
    void Append(CHAR ch);
	void Append(WCHAR ch);

	CStringW operator+(const CStringW& str);
	CStringW operator+(const LPCWSTR str);
	CStringW operator+(const WCHAR ch);
	CStringW operator+(const CString& str);
	CStringW operator+(const LPCSTR str);
	CStringW operator+(const CHAR ch);
	
	CStringW& operator<<(const CStringW& str);
	CStringW& operator<<(WCHAR ch);
	CStringW& operator<<(LPCWSTR str);
	CStringW& operator<<(DWORD iNum);
	CStringW& operator<<(int iNum);
	CStringW& operator<<(LONGLONG iNum);
	CStringW& operator<<(ULONGLONG iNum);
	CStringW& operator<<(const CString& str);
	CStringW& operator<<(CHAR ch);
	CStringW& operator<<(LPCSTR str);

	int Compare(LPCWSTR lpsz) const;
	int CompareNoCase(LPCWSTR lpsz) const;
	int Compare(LPCSTR lpsz) const;
	int CompareNoCase(LPCSTR lpsz) const;
	
	BOOL operator==(const CStringW& str);
	BOOL operator==(LPCWSTR str);
	BOOL operator==(const CString& str);
	BOOL operator==(LPCSTR str);
	BOOL ContainString(LPCWSTR str,UINT start=0);

	CStringW Mid(int nFirst,int nCount) const;
	CStringW Mid(int nFirst) const;
	CStringW Left(int nCount) const;
	CStringW Right(int nCount) const;
	WCHAR LastChar() const;
	void DelLastChar();

	void MakeUpper();
	void MakeLower();
	void MakeReverse();

	int Find(WCHAR ch) const;
	int FindOneOf(LPCWSTR lpszCharSet) const;
	int FindFirst(WCHAR ch) const;
	int FindLast(WCHAR ch) const;
	int FindNext(WCHAR ch,int idx) const;
	int Find(LPCWSTR lpszSub) const;

	LPWSTR GetBuffer(int nMinBufLength=-1,BOOL bStoreData=FALSE);
	void FreeExtra(int nNewLength=-1);
	DWORD GetAllocLength() const { return m_nAllocLen; }
	void Compact();
	LPWSTR GiveBuffer(); 

	BOOL InsChar(int idx,WCHAR ch);
	BOOL Insert(int idx,WCHAR ch) { return InsChar(idx,ch); }
	BOOL DelChar(int idx);
	BOOL Delete(int idx,int nCount=1);
	void ReplaceChars(WCHAR from,WCHAR to);
	void Trim(); // Deletes spaces from begin and end

	void Swap(CStringW& str);

	void Format(LPCWSTR lpszFormat,...);
	void FormatV(LPCWSTR lpszFormat,va_list);
	void FormatEx(LPCWSTR lpszFormat,...);

#ifdef DEF_RESOURCES
	CStringW(int nID);
	CStringW(int nID,TypeOfResourceHandle bType);
	CStringW(UINT nID);
	CStringW(UINT nID,TypeOfResourceHandle bType);

	void Format(UINT nFormatID,...);
	void FormatC(UINT nFormatID,...);
	void FormatEx(UINT nFormatID,...);
	void FormatExC(UINT nFormatID,...);

	BOOL LoadString(UINT nID);
	BOOL LoadString(UINT nID,TypeOfResourceHandle bType);
	BOOL AddString(UINT nID);
	BOOL AddString(UINT nID,TypeOfResourceHandle bType);
#endif
	friend class CString;
	friend WCHAR* alloccopy(CStringW& sString);

};


//////////////////////////////////////////////////
// Conversion classes
class W2A 
{
private:
	char* pAStr;

public:
	W2A(LPCWSTR sA);
	W2A(LPCWSTR sA,int len);
	W2A(CStringW& sA);

	~W2A();

	operator LPCSTR() const;
};

class A2W 
{
private:
	WCHAR* pWStr;

public:
	A2W(LPCSTR sA);
	A2W(LPCSTR sA,int len);
	A2W(CString& sA);

	~A2W();

	operator LPCWSTR() const;
};

class W2UTF8 
{
private:
	char* pAStr;

public:
	W2UTF8(LPCWSTR sA);
	W2UTF8(LPCWSTR sA,int len);
	W2UTF8(CStringW& sA);

	~W2UTF8();

	operator LPCSTR() const;
};

class ID2A
{
private:
	CHAR* pStr;

public:
	ID2A(UINT nID,TypeOfResourceHandle bType=LanguageSpecificResource);
	~ID2A();
	operator LPCSTR() const;
};

class ID2W
{
private:
	WCHAR* pWStr;

public:
	ID2W(UINT nID,TypeOfResourceHandle bType=LanguageSpecificResource);
	~ID2W();
	operator LPCWSTR() const;
};

#endif


#include "Strings.inl"

#endif
