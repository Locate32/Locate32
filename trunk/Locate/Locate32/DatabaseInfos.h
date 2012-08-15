/* Locate32 - Copyright (c) 1997-2010 Janne Huttunen */

#if !defined(DATABASEINFOS_H)
#define DATABASEINFOS_H

#if _MSC_VER >= 1000
#pragma once
#endif 

class CDatabaseInfos : public CPropertySheet
{
public:
	CDatabaseInfos(HWND hParent);

	class CDatabaseInfoPage : public CPropertyPage  
	{
	public:
		CDatabaseInfoPage(const CDatabase* pDatabase);

		virtual BOOL OnCommand(WORD wID,WORD wNotifyCode,HWND hControl);
		virtual BOOL OnInitDialog(HWND hwndFocus);
		virtual void OnDestroy();

	private:
		CListCtrl* m_pList;
		BOOL m_bOldDB;
		
		const CDatabase* m_pDatabase;
	};

	CArrayFP<CDatabaseInfoPage*> m_aInfoPages;

};

inline CDatabaseInfos::CDatabaseInfoPage::CDatabaseInfoPage(const CDatabase* pDatabase)
:	CPropertyPage(IDD_DATABASEINFO,pDatabase->GetName()),m_pList(NULL),m_bOldDB(FALSE),
	m_pDatabase(pDatabase)
{
}

#endif