/* Locate32 - Copyright (c) 1997-2010 Janne Huttunen */

#if !defined(MISC_H)
#define MISC_H

#if _MSC_VER >= 1000
#pragma once
#endif


////////////////////////////////////////////////////////////
// Messages
////////////////////////////////////////////////////////////

#define DTXM_SETRELDATE			DTM_FIRST+20 // wParam=date, lParam flags
#define DTXM_GETRELDATE			DTM_FIRST+21 
#define DTXM_GETCLASS			DTM_FIRST+22 // Returns pointer to CDateTimePickEx
#define DTXM_CHANGEMODE			DTM_FIRST+23 // wParam: 0 for explicit mode, 1 for relative mode
#define DTXM_GETMODE			DTM_FIRST+24 // returns 0 for explicit mode, 1 for relative mode
#define DTXX_GETSYSTEMTIME		DTM_GETSYSTEMTIME
#define DTXX_SETSYSTEMTIME		DTM_SETSYSTEMTIME


// For lParam of DTMX_SETRELDATE & DTMX_SETSYSTEMTIME
#define DTXF_NOMODECHANGE		0x10000000
#define DTXF_NOSPINCHANGE		0x20000000

// For wParam of DTMX_GETSYSTEMTIME 
#define DTXF_FORSAVE			0x80000000

#define DTXF_MSGMASK			0xF0000000


////////////////////////////////////////////////////////////
// Notifications
////////////////////////////////////////////////////////////
#define DTXN_CHANGE				0x300
#define DTXN_MODECHANGED		0x400



////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////

BOOL GetIMAPIBurningDevices(CArray<LPWSTR>& aDevicePaths);
BOOL RegisterDataTimeExCltr();


////////////////////////////////////////////////////////////
// Buffer allocation
void InitializeBuffers();
void DeinitializeBuffers();

void AssignBuffer(void* pBuffer,DWORD id=0); // id=0, use thread ID
void FreeBuffers(DWORD id=0);

inline LPSTR GetBuffer(size_t len,DWORD id=0)
{
	LPSTR pBuffer=new char[len];
	AssignBuffer(pBuffer,id);
	return pBuffer;
}

inline LPWSTR GetBufferW(size_t len,DWORD id=0)
{
	LPWSTR pBuffer=new WCHAR[len];
	AssignBuffer(pBuffer,id);
	return pBuffer;
}

////////////////////////////////////////////////////////////
// Classes
////////////////////////////////////////////////////////////




// CDateTimeCtrlEx - Used in Size & Date tab

class CDateTimeCtrlEx : public CDateTimeCtrl 
{
public:
	enum Flags {
		ModeExplicit=0x0000,
		ModeRelative=0x0001,
		ModeMask=0x0001,

		Normal=0x0000,
		Hot=0x0002,
		Pressed=0x0004,
		ButtonStateMask=0x0006,

		SpinBoxIsUpdating=0x0008,
		DontSendNotifications=0x0010
	};

public:
	CDateTimeCtrlEx(HWND hWnd);
	~CDateTimeCtrlEx();

public:
	int GetRelativeDate() const;
	int GetExplicitDate(LPSYSTEMTIME pSystemTime,DWORD dwFlags) const;
	int SetExplicitDate(LPSYSTEMTIME pSystemTime,DWORD dwFlags);
	void SetRelativeDate(int nNewPos,DWORD dwFlags);
	void ChangeMode(BOOL bToRelative);
	BOOL GetMode() const; // TRUE if relative, FALSE is explicit

	static CDateTimeCtrlEx* GetClass(HWND hWnd);
	

private:
	void OnPaint();
	static LRESULT CALLBACK WndProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	void CreateControls();

	static int GetValueFromText(LPCWSTR szText);

private:
	HWND m_hTimePickerWnd;
	HWND m_hEditWnd;
	HWND m_hSpinWnd;
	
	
	HMODULE m_hUxTheme;
	HRESULT(STDAPICALLTYPE *m_pDrawThemeBackground)(HANDLE,HDC,int,int,const RECT*,const RECT*);
	HANDLE m_hTheme;
	BOOL m_bDeleteOnDestroy;

	DWORD m_dwFlags;


	friend BOOL RegisterDataTimeExCltr();
};

/* CComboBoxAutoComplete - This is a class for autocompleting combo boxes. But it's still 
unfinished and therefore not used anywhere */

class CComboBoxAutoComplete : public CComboBox
{
public:
	CComboBoxAutoComplete();
	CComboBoxAutoComplete(HWND hWnd);
	~CComboBoxAutoComplete();

	void EnableAutoComplete(BOOL bEnable);
	BOOL IsAutoCompleteEnabled() const;

	int GetCount() const;
	int GetCurSel() const;
	int SetCurSel(int nSelect);
	int GetTopIndex() const;
	int SetTopIndex(int nIndex);
	
	int GetLBText(int nIndex, LPSTR lpszText) const;
	int GetLBText(int nIndex, CStringA& rString) const;
	int GetLBTextLen(int nIndex) const;

	int FindStringExact(int nIndexStart, LPCSTR lpszFind) const;
	BOOL GetDroppedState() const;

	void ShowDropDown(BOOL bShowIt = TRUE);

	int AddString(LPCSTR lpszString);
	int DeleteString(UINT nIndex);
	int InsertString(int nIndex, LPCSTR lpszString);
	void ResetContent();
	
	int FindString(int nStartAfter,LPCSTR lpszString) const;
	int SelectString(int nStartAfter,LPCSTR lpszString);

	BOOL HandleOnCommand(WORD wNotifyCode);

#ifdef DEF_WCHAR
	int GetLBText(int nIndex, LPWSTR lpszText) const;
	int GetLBText(int nIndex, CStringW& rString) const;
	int FindStringExact(int nIndexStart, LPCWSTR lpszFind) const;
	int AddString(LPCWSTR lpszString);
	int InsertString(int nIndex, LPCWSTR lpszString);
	int FindString(int nStartAfter,LPCWSTR lpszString) const;
	int SelectString(int nStartAfter,LPCWSTR lpszString);
#endif


private:
	struct ACDATA {
		enum ACFLags {
			afAutoCompleting = 0x1
		};
		BYTE bFlags;

		CArrayFAP<LPWSTR> aItems;	
		CIntArray aItemsInList;

	};	

	ACDATA* m_pACData;
};




// CRegKey2 - Registry operations relative to "Software\Update" (or whatever is the common key)


class CRegKey2 : public CRegKey
{
public:
	CRegKey2();
	CRegKey2(HKEY hKey);
	CRegKey2(HKEY hKey,LPCSTR lpszSubKey,DWORD fStatus=CRegKey::defWrite,LPSECURITY_ATTRIBUTES lpSecurityAttributes=NULL);
	CRegKey2(HKEY hKey,LPCWSTR lpszSubKey,DWORD fStatus=CRegKey::defWrite,LPSECURITY_ATTRIBUTES lpSecurityAttributes=NULL);


	// Open hKey/CommonKey/lpszSubKey
	LONG OpenKey(HKEY hKey,LPCSTR lpszSubKey,DWORD fStatus=CRegKey::createNew|CRegKey::samAll,LPSECURITY_ATTRIBUTES lpSecurityAttributes=NULL);
	LONG OpenKey(HKEY hKey,LPCWSTR lpszSubKey,DWORD fStatus=CRegKey::createNew|CRegKey::samAll,LPSECURITY_ATTRIBUTES lpSecurityAttributes=NULL);


	// Deletekey HKCU/CommonKey/lpszSubKey
	static LONG DeleteCommonKey(LPCSTR lpszSubKey);
	static LONG DeleteCommonKey(LPCWSTR lpszSubKey);
	

	static CString GetCommonKey();
	static CStringW GetCommonKeyW();
};


// Alternative _StrCmp 
int STDAPICALLTYPE _StrCmpW(LPCWSTR,LPCWSTR);

#include "misc.inl"



#endif