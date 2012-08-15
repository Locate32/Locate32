////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////

#ifndef HFCGENERAL_INL
#define HFCGENERAL_INL

///////////////////////////////////////////
// CTime

inline CTime::CTime()
:	m_time(0),msec(0)
{
}

inline CTime::CTime(time_t Time,WORD ms)
:	m_time(Time),msec(ms)
{
}

inline CTime PASCAL CTime::GetCurrentTime()
{
	CTime tm;
	::time(&tm.m_time);
	return tm;
}

inline CTime::operator time_t() const
{
	return m_time;
}

inline const CTime& CTime::operator=(time_t t)
{
	m_time=t;
	msec=0;
	return *this;
}

inline time_t CTime::GetTime() const
{
	return m_time;
}

inline int CTime::GetMilliSecond() const
{
	return msec;
}

inline BOOL CTime::IsLeapYear(WORD nYear)
{
   return (nYear%4==0 && (nYear%100 || nYear%400==0));
}

inline BOOL CTime::IsLeapYear() const
{
	return IsLeapYear(GetYear());
}

inline WORD CTime::LeapYears(WORD nYear)
{
	return ((nYear/4)-(nYear/100)+(nYear/400));
}

inline DWORD CTime::GetIndex(WORD nDay,WORD nMonth,WORD nYear)
{
	return ((nYear-1)*365+LeapYears(nYear-1)+DayOfYear(nDay,nMonth,nYear));
}

inline int CTime::GetDayOfWeek(WORD nDay,WORD nMonth,WORD nYear)
{ 
	return GetIndex(nDay,nMonth,nYear)%7; 
}   

////////////////////////////
// Class CSize

inline int CSize::GetCX() const
{
	return cx;
}

inline void CSize::SetCX(int initCX)
{
	cx=initCX;
}

inline int CSize::GetCY() const
{
	return cy;
}

inline void CSize::SetCY(int initCY)
{
	cy=initCY;
}

inline void CSize::SetSize(int initCX,int initCY)
{
	cx=initCX;
	cy=initCY;
}

inline void CSize::MakeLarger(CSize& sz)
{
	if (cx<sz.cx)
		cx=sz.cx;
	if (cy<sz.cy)
		cy=sz.cy;
}

inline void CSize::MakeSmaller(CSize& sz)
{
	if (cx>sz.cx)
		cx=sz.cx;
	if (cy>sz.cy)
		cy=sz.cy;
}

////////////////////////////
// Class CPoint

inline CSize::CSize()
{
	cx=0;
	cy=0;
}

inline CSize::CSize(int initCX,int initCY)
{
	cx=initCX;
	cy=initCY;
}

inline CSize::CSize(SIZE initSize)
{
	cx=initSize.cx;
	cy=initSize.cy;
}

inline CSize::CSize(DWORD dwSize)
{
	cx=LOWORD(dwSize);
	cy=HIWORD(dwSize);
}

inline CPoint::CPoint()
{
	x=0;
	y=0;
}

inline CPoint::CPoint(int initX,int initY)
{
	x=initX;
	y=initY;
}

inline CPoint::CPoint(const POINT& initPoint)
{
	x=initPoint.x;
	y=initPoint.y;
}

inline CPoint::CPoint(const SIZE& initSize)
{
	x=initSize.cx;
	y=initSize.cy;
}

inline CPoint::CPoint(DWORD dwPoint)
{
	x=LOWORD(dwPoint);
	y=HIWORD(dwPoint);
}

inline int CPoint::GetX() const
{
	return x;
}

inline void CPoint::SetX(int initX)
{
	x=initX;
}

inline int CPoint::GetY() const
{
	return y;
}

inline void CPoint::SetY(int initY)
{
	y=initY;
}

inline void CPoint::SetPoint(int initX,int initY)
{
	x=initX;
	y=initY;
}

inline BOOL CPoint::IsInRect(const RECT& rc)
{
	return x>=rc.left && x<rc.right && y>=rc.top && y<rc.bottom;
}

//////////////////////////////////
// CRect

inline CRect::CRect()

{
	left=0;
	top=0;
	right=0;
	bottom=0;
}

inline CRect::CRect(int l,int t,int r,int b)
{
	left=l;
	top=t;
	right=r;
	bottom=b;
}

inline CRect::CRect(const RECT& rc)
{
	left=rc.left;
	top=rc.top;
	right=rc.right;
	bottom=rc.bottom;
}

inline CRect::CRect(const LPRECT rc)
{
	left=rc->left;
	top=rc->top;
	right=rc->right;
	bottom=rc->bottom;
}

inline CRect::CRect(const POINT& pt,const SIZE& sz)
{
	left=pt.x;
	top=pt.y;
	right=pt.x+sz.cx;
	bottom=pt.y+sz.cy;
}

inline CRect::CRect(const POINT& topLeft,const POINT& bottomRight)
{
	left=topLeft.x;
	top=topLeft.y;
	right=bottomRight.x;
	bottom=bottomRight.y;
}

inline int CRect::Width() const
{
	return right-left;
}

inline int CRect::Height() const
{
	return bottom-top;
}

inline CSize CRect::Size() const
{
	return CSize(right-left,bottom-top);
}

inline CPoint CRect::TopLeft()
{
	return CPoint(left,top);
}

inline CPoint CRect::BottomRight()
{
	return CPoint(right,bottom);
}

inline BOOL CRect::IsPtInRect(const POINT& pt)
{
	return pt.x>=left && pt.x<right && pt.y>=top && pt.y<bottom;
}

inline BOOL CRect::IsPtInRect(int x,int y)
{
	return x>=left && x<right && y>=top && y<bottom;
}


///////////////////////////
// Class CRegKey

#ifdef WIN32

inline CRegKey::CRegKey()
:	m_hKey(NULL)
{
}

inline CRegKey::CRegKey(HKEY hKey)
:	m_hKey(hKey)
{
}
	
inline CRegKey::CRegKey(HKEY hKey,LPCSTR lpszSubKey,DWORD fStatus,LPSECURITY_ATTRIBUTES lpSecurityAttributes)
:	m_hKey(NULL)
{
	OpenKey(hKey,lpszSubKey,fStatus,lpSecurityAttributes);
}

inline CRegKey::CRegKey(HKEY hKey,LPCWSTR lpszSubKey,DWORD fStatus,LPSECURITY_ATTRIBUTES lpSecurityAttributes)
:	m_hKey(NULL)
{
	OpenKey(hKey,lpszSubKey,fStatus,lpSecurityAttributes);
}

inline CRegKey::~CRegKey()
{
	if (m_hKey!=NULL)
	{
		RegCloseKey(m_hKey);
		DebugCloseHandle(dhtRegKey,m_hKey,STRNULL);
	}
	m_hKey=NULL;
}

inline LONG CRegKey::UnLoadKey(LPCTSTR lpszSubKey)
{
	return ::RegUnLoadKey(m_hKey,lpszSubKey);
}

inline BYTE CRegKey::FlushKey()
{
	return (::RegFlushKey(m_hKey)==ERROR_SUCCESS);
}

inline HKEY CRegKey::GetHandleKey() const
{
	return m_hKey;
}

inline CRegKey::operator HKEY() const
{
	return m_hKey;
}

inline DWORD CRegKey::QueryValue(LPCSTR lpszValueName,LPSTR lpbData,DWORD cbData,LPDWORD lpdwType) const
{
	return ::RegQueryValueEx(m_hKey,(LPSTR)lpszValueName,NULL,lpdwType,(LPBYTE)lpbData,&cbData)==ERROR_SUCCESS?cbData:0;
}

inline BOOL CRegKey::QueryValue(LPCSTR lpszValueName,DWORD& dwData) const
{
	DWORD cb=sizeof(DWORD);
	return ::RegQueryValueEx(m_hKey,lpszValueName,NULL,NULL,LPBYTE(&dwData),&cb)==ERROR_SUCCESS?cb==4:FALSE;
}

#ifdef DEF_WCHAR
inline BOOL CRegKey::QueryValue(LPCWSTR lpszValueName,DWORD& dwData) const
{
	DWORD cb=sizeof(DWORD);
	if (IsUnicodeSystem())
		return ::RegQueryValueExW(m_hKey,lpszValueName,NULL,NULL,LPBYTE(&dwData),&cb)==ERROR_SUCCESS?cb==4:FALSE;
	else
		return ::RegQueryValueExA(m_hKey,W2A(lpszValueName),NULL,NULL,LPBYTE(&dwData),&cb)==ERROR_SUCCESS?cb==4:FALSE;
}
#endif

inline DWORD CRegKey::QueryValueLength(LPCSTR lpszValueName) const
{
	DWORD nLength=0;
	return ::RegQueryValueEx(m_hKey,(LPSTR)lpszValueName,NULL,NULL,NULL,&nLength)==ERROR_SUCCESS?nLength:0;
}

inline DWORD CRegKey::QueryValueLength(LPCSTR lpszValueName,BOOL& bIsOk) const
{
	DWORD nLength=0;
	if (::RegQueryValueEx(m_hKey,(LPSTR)lpszValueName,NULL,NULL,NULL,&nLength)==ERROR_SUCCESS)
	{
		bIsOk=TRUE;
		return nLength;
	}

	bIsOk=FALSE;
    return 0;

}

inline DWORD CRegKey::QueryValueLength(LPCWSTR lpszValueName) const
{
	DWORD nLength=0;
	if (IsUnicodeSystem())
		return ::RegQueryValueExW(m_hKey,(LPWSTR)lpszValueName,NULL,NULL,NULL,&nLength)==ERROR_SUCCESS?nLength:0;
	else
		return ::RegQueryValueExA(m_hKey,(LPSTR)(LPCSTR)W2A(lpszValueName),NULL,NULL,NULL,&nLength)==ERROR_SUCCESS?nLength:0;
}



inline LONG CRegKey::SetValue(LPCSTR lpValueName,LPCSTR lpData,DWORD cbData,DWORD dwType)
{
	return ::RegSetValueEx(m_hKey,lpValueName,0,dwType,(CONST BYTE*)lpData,cbData);
}

inline BOOL CRegKey::SetValue(LPCSTR lpValueName,CString& strData)
{
	return ::RegSetValueEx(m_hKey,lpValueName,0,REG_SZ,(CONST BYTE*)(LPCTSTR)strData,(DWORD)strData.GetLength()+1)==ERROR_SUCCESS;
}

inline LONG CRegKey::SetValue(LPCSTR lpValueName,LPCSTR strData)
{
	return ::RegSetValueEx(m_hKey,lpValueName,0,REG_SZ,(CONST BYTE*)strData,(DWORD)strlen(strData)+1);
}
	
inline BOOL CRegKey::SetValue(LPCSTR lpValueName,DWORD dwData)
{
	return ::RegSetValueEx(m_hKey,lpValueName,0,REG_DWORD,(CONST BYTE*)&dwData,sizeof(DWORD))==ERROR_SUCCESS;
}




inline BOOL CRegKey::SetValue(LPCWSTR lpValueName,DWORD dwData)
{
	if (IsUnicodeSystem())
		return ::RegSetValueExW(m_hKey,lpValueName,0,REG_DWORD,(CONST BYTE*)&dwData,sizeof(DWORD))==ERROR_SUCCESS;
	else
		return ::RegSetValueExA(m_hKey,W2A(lpValueName),0,REG_DWORD,(CONST BYTE*)&dwData,sizeof(DWORD))==ERROR_SUCCESS;
}


inline DWORD CRegKey::EnumKey(DWORD iSubkey,LPSTR lpszName,DWORD cchName,LPTSTR lpszClass,LPDWORD lpcchClass,PFILETIME lpftLastWrite) const
{
	return ::RegEnumKeyEx(m_hKey,iSubkey,lpszName,&cchName,0,lpszClass,lpcchClass,lpftLastWrite)==ERROR_SUCCESS?cchName+1:0;
}

inline DWORD CRegKey::EnumValue(DWORD iValue,LPSTR lpszValue,DWORD cchValue,LPDWORD lpdwType,LPBYTE lpbData,LPDWORD lpcbData) const
{
	return ::RegEnumValue(m_hKey,iValue,lpszValue,&cchValue,0,lpdwType,lpbData,lpcbData)==ERROR_SUCCESS?cchValue+1:0;
}





inline LONG CRegKey::DeleteKey(LPCSTR lpszSubKey)
{
	return ::RegDeleteKey(m_hKey,lpszSubKey);
}

inline LONG CRegKey::DeleteValue(LPCSTR lpszValue)
{
	return ::RegDeleteValue(m_hKey,lpszValue);
}

inline LONG CRegKey::DeleteKey(LPCWSTR lpszSubKey)
{
	if (IsUnicodeSystem())
		return ::RegDeleteKeyW(m_hKey,lpszSubKey);
	else
		return ::RegDeleteKeyA(m_hKey,W2A(lpszSubKey));
}

inline LONG CRegKey::DeleteValue(LPCWSTR lpszValue)
{
	if (IsUnicodeSystem())
		return ::RegDeleteValueW(m_hKey,lpszValue);
	else
		return ::RegDeleteValueA(m_hKey,W2A(lpszValue));
}

inline LONG CRegKey::GetKeySecurity(SECURITY_INFORMATION SecInf,PSECURITY_DESCRIPTOR pSecDesc,LPDWORD lpcbSecDesc) const
{
	return ::RegGetKeySecurity(m_hKey,SecInf,pSecDesc,lpcbSecDesc);
}

inline LONG CRegKey::SetKeySecurity(SECURITY_INFORMATION si,PSECURITY_DESCRIPTOR psd)
{
	return ::RegSetKeySecurity(m_hKey,si,psd);
}

inline LONG CRegKey::LoadKey(LPCSTR lpszSubKey,LPCSTR lpszFile)
{
	return ::RegLoadKey(m_hKey,lpszSubKey,lpszFile);
}

inline LONG CRegKey::SaveKey(LPCSTR lpszFile,LPSECURITY_ATTRIBUTES lpsa)
{
	return ::RegSaveKey(m_hKey,lpszFile,lpsa);
}

inline LONG CRegKey::LoadKey(LPCWSTR lpszSubKey,LPCWSTR lpszFile)
{
	if (IsUnicodeSystem())
		return ::RegLoadKeyW(m_hKey,lpszSubKey,lpszFile);
	else
		return ::RegLoadKeyA(m_hKey,W2A(lpszSubKey),W2A(lpszFile));

}

inline LONG CRegKey::SaveKey(LPCWSTR lpszFile,LPSECURITY_ATTRIBUTES lpsa)
{
	if (IsUnicodeSystem())
		return ::RegSaveKeyW(m_hKey,lpszFile,lpsa);
	else
		return ::RegSaveKeyA(m_hKey,W2A(lpszFile),lpsa);

}

inline LONG CRegKey::ReplaceKey(LPCSTR lpSubKey,LPCSTR lpNewFile,LPCSTR lpOldFile)
{
	return ::RegReplaceKey(m_hKey,lpSubKey,lpNewFile,lpOldFile);
}

inline LONG CRegKey::RestoreKey(LPCSTR lpszFile,DWORD fdw)
{
	return ::RegRestoreKey(m_hKey,lpszFile,fdw);
}

inline LONG CRegKey::ReplaceKey(LPCWSTR lpSubKey,LPCWSTR lpNewFile,LPCWSTR lpOldFile)
{
	if (IsUnicodeSystem())
		return ::RegReplaceKeyW(m_hKey,lpSubKey,lpNewFile,lpOldFile);
	else
		return ::RegReplaceKeyA(m_hKey,W2A(lpSubKey),W2A(lpNewFile),W2A(lpOldFile));
}

inline LONG CRegKey::RestoreKey(LPCWSTR lpszFile,DWORD fdw)
{
	if (IsUnicodeSystem())
		return ::RegRestoreKeyW(m_hKey,lpszFile,fdw);
	else
		return ::RegRestoreKeyA(m_hKey,W2A(lpszFile),fdw);
}

inline LONG CRegKey::NotifyChangeKeyValue(BOOL fWatchSubTree,DWORD fdwNotifyFilter,HANDLE hEvent,BOOL fAsync)
{
	return ::RegNotifyChangeKeyValue(m_hKey,fWatchSubTree,fdwNotifyFilter,hEvent,fAsync);
}

inline LONG CRegKey::QueryInfoKey(LPTSTR lpszClass,LPDWORD lpcchClass,LPDWORD lpcSubKeys,LPDWORD lpcchMaxSubkey,
	LPDWORD lpcchMaxClass,LPDWORD lpcValues,LPDWORD lpcchMaxValueName,LPDWORD lpcbMaxValueData,
	LPDWORD lpcbSecurityDescriptor,PFILETIME lpftLastWriteTime) const
{
	return ::RegQueryInfoKey(m_hKey,lpszClass,lpcchClass,NULL,lpcSubKeys,lpcchMaxSubkey,lpcchMaxClass,
		lpcValues,lpcchMaxValueName,lpcbMaxValueData,lpcbSecurityDescriptor,lpftLastWriteTime);
}

inline LONG CRegKey::CopyKey(HKEY hDestination)
{
	return CopyKey(m_hKey,hDestination);
}

inline LONG CRegKey::RenameSubKey(LPCSTR szOldName,LPCSTR szNewName)
{
	return RenameSubKey(m_hKey,szOldName,szNewName);
}

inline LONG CRegKey::RenameSubKey(LPCWSTR szOldName,LPCWSTR szNewName)
{
	return RenameSubKey(m_hKey,szOldName,szNewName);
}

inline LONG CRegKey::SetValue(LPCWSTR lpValueName,LPCSTR lpData,DWORD cbData,DWORD dwType)
{
	if (IsUnicodeSystem())
		return ::RegSetValueExW(m_hKey,lpValueName,0,dwType,(CONST BYTE*)lpData,(DWORD)cbData);
	else
		return ::RegSetValueExA(m_hKey,W2A(lpValueName),0,dwType,(CONST BYTE*)lpData,(DWORD)cbData);
}

inline DWORD CRegKey::QueryValue(LPCWSTR lpszValueName,LPSTR lpbData,DWORD cbDataLen,LPDWORD lpdwType) const
{
	if (IsUnicodeSystem())
		return ::RegQueryValueExW(m_hKey,(LPWSTR)lpszValueName,NULL,lpdwType,(LPBYTE)lpbData,&cbDataLen)!=ERROR_SUCCESS?0:cbDataLen;
	else
		return ::RegQueryValueExA(m_hKey,W2A(lpszValueName),NULL,lpdwType,(LPBYTE)lpbData,&cbDataLen)!=ERROR_SUCCESS?0:cbDataLen;
}
#endif

///////////////////////////////////////
// CExceptionObject

inline CExceptionObject::CExceptionObject()
:	m_bThrow(TRUE)
{
}

inline CExceptionObject::CExceptionObject(BOOL bThrow)
:	m_bThrow(bThrow)
{
}

inline void CExceptionObject::SetToThrow(BOOL bThrow)
{
	m_bThrow=bThrow;
}

inline BOOL CExceptionObject::IsThrowing() const
{
	return m_bThrow;
}



///////////////////////////
// Class CStream


inline CStream::CStream()
{
}

inline CStream::CStream(BOOL bThrow)
:	CExceptionObject(bThrow)
{
}

inline BOOL CStream::Read(BYTE& bNum) const
{ 
	return this->Read(&bNum,sizeof(BYTE))==sizeof(BYTE); 
}

inline BOOL CStream::Read(WORD& wNum) const
{ 
	return this->Read(&wNum,sizeof(WORD))==sizeof(WORD); 
}

inline BOOL CStream::Read(DWORD& dwNum) const
{ 
	return this->Read(&dwNum,sizeof(DWORD))==sizeof(DWORD); 
}

inline BOOL CStream::Write(const CStringA& str) 
{ 
	return this->Write((LPCSTR)str,(DWORD)str.GetLength()); 
}

#ifdef DEF_WCHAR
inline BOOL CStream::Write(const CStringW& str) 
{ 
	return this->Write((LPCWSTR)str,(DWORD)(str.GetLength())); 
}
#endif



inline BOOL CStream::Write(BYTE bNum) 
{ 
	return this->Write(&bNum,sizeof(BYTE)); 
}

inline BOOL CStream::Write(WORD wNum)
{ 
	return this->Write(&wNum,sizeof(WORD)); 
}

inline BOOL CStream::Write(DWORD dwNum) 
{ 
	return this->Write(&dwNum,sizeof(DWORD)); 
}

inline BOOL CStream::Write(char ch) 
{ 
	return this->Write(&ch,sizeof(char)); 
}

inline BOOL CStream::Write(LPCSTR szNullTerminatedString) 
{ 
	return this->Write(szNullTerminatedString,(DWORD)istrlen(szNullTerminatedString)); 
}

#ifdef DEF_WCHAR
inline BOOL CStream::Write(LPCWSTR szNullTerminatedString) 
{ 
	return this->Write(szNullTerminatedString,(DWORD)istrlenw(szNullTerminatedString)); 
}
#endif

template<class TYPE>
inline CStream& operator <<(CStream& stream,TYPE data)
{
	(&stream)->Write(&data,sizeof(TYPE));
	return stream;
}

template<class TYPE>
inline const CStream& operator >>(const CStream& stream,TYPE& data)
{
	(&stream)->Read(&data,sizeof(TYPE));
	return stream;
}


#endif
