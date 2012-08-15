/* Locate32 - Copyright (c) 1997-2010 Janne Huttunen */

// Included to Locate
#include <HFCLib.h>
#include "Locate32.h"

///////////////////////////////////////////////////////////
// CSelectColumnsDlg

BOOL CSelectColumnsDlg::OnInitDialog(HWND hwndFocus)
{
	ASSERT(m_aIDs.GetSize()==m_aWidths.GetSize());

	if (!LoadPosition(HKCU,CRegKey2::GetCommonKey()+"\\Dialogs","SelectColumnsWindowPos",fgOnlyNormalPosition|fgNoSize))
		CenterWindow();

	m_pList=new CListCtrl(GetDlgItem(IDC_COLUMNS));
	m_pList->SetExtendedListViewStyle(LVS_EX_CHECKBOXES,LVS_EX_CHECKBOXES);
	m_pList->InsertColumn(0,"",LVCFMT_LEFT,250);
	if (IsUnicodeSystem())
		m_pList->SetUnicodeFormat(TRUE);

	CLocateDlg::ViewDetails* pDetails=CLocateDlg::GetDefaultDetails();

    int nItem;
	for (nItem=0;nItem<m_aSelectedCols.GetSize();nItem++)
	{
		m_pList->InsertItem(LVIF_TEXT|LVIF_PARAM,nItem,LPSTR_TEXTCALLBACK,0,0,0,
			LPARAM(new ColumnItem(m_aSelectedCols[nItem],
			(DetailType)m_aIDs[m_aSelectedCols[nItem]],
			pDetails[m_aIDs[m_aSelectedCols[nItem]]].nString,
			m_aWidths[m_aSelectedCols[nItem]],
			m_aAligns[m_aSelectedCols[nItem]],m_aActions[m_aSelectedCols[nItem]])));
		m_pList->SetCheckState(nItem,TRUE);
	}

	for (int i=0;i<m_aIDs.GetSize();i++)
	{
		if (m_aSelectedCols.Find(i)==-1)
		{
			m_pList->InsertItem(LVIF_TEXT|LVIF_PARAM,nItem++,
				LPSTR_TEXTCALLBACK,0,0,0,LPARAM(new ColumnItem(i,
				(DetailType)m_aIDs[i],
				pDetails[m_aIDs[i]].nString,
				m_aWidths[i],m_aAligns[i],m_aActions[i])));
			m_pList->SetCheckState(nItem,FALSE);
		}
	}
	delete[] pDetails;

	
	CSpinButtonCtrl spin(GetDlgItem(IDC_WIDTHSPIN));
	spin.SetRange(10,10000);
	spin.SetBuddy(GetDlgItem(IDC_WIDTH));
	

	m_ActionCombo.AttachToDlgItem(*this,IDC_ACTION);
	m_WhenCombo.AttachToDlgItem(*this,IDC_WHEN);
	m_WhichFileCombo.AttachToDlgItem(*this,IDC_WHICHFILE);
	m_ContextMenuForCombo.AttachToDlgItem(*this,IDC_CONTEXTMENUFOR);
	m_VerbCombo.AttachToDlgItem(*this,IDC_VERB);

	
	m_ActionCombo.AddString(ID2W(IDS_NONE));
	for (UINT uSubItem=0;;uSubItem++)
	{
		int nID=CAction::GetResultItemActionLabelStringId((CAction::ActionResultList)uSubItem);
		if (nID==0)
			break;

		if (uSubItem==CAction::SelectNthFile || uSubItem==CAction::ExecuteNthFile)
		{
			CStringW str;
			str.Format(nID,L"N",(LPCWSTR)ID2W(IDS_NTH));
			m_ActionCombo.AddString(str);
		}
		else
			m_ActionCombo.AddString(ID2W(nID));
	}
	m_ActionCombo.SetCurSel(0);
	
	m_WhenCombo.AddString(ID2W(IDS_DETAILLEFTCLICK));
	m_WhenCombo.AddString(ID2W(IDS_DETAILLEFTDBLCLICK));
	m_WhenCombo.AddString(ID2W(IDS_DETAILRIGHTCLICK));
	m_WhenCombo.AddString(ID2W(IDS_DETAILRIGHTDBLCLICK));
	m_WhenCombo.AddString(ID2W(IDS_DETAILMIDDLECLICK));
	m_WhenCombo.AddString(ID2W(IDS_DETAILMIDDLEDLBCLICK));
	m_WhenCombo.SetCurSel(0);
	
	// Insert verbs
	m_VerbCombo.AddString(ID2W(IDS_DEFAULT));
	m_VerbCombo.AddString("open");
	m_VerbCombo.AddString("edit");
	m_VerbCombo.AddString("explore");
	m_VerbCombo.AddString("find");
	m_VerbCombo.AddString("print");
	m_VerbCombo.SetCurSel(0);

	// Item spin
	SendDlgItemMessage(IDC_ITEMSPIN,UDM_SETRANGE32,1,0xFFFFFFFF>>1);
	SendDlgItemMessage(IDC_ITEMSPIN,UDM_SETBUDDY,WPARAM(GetDlgItem(IDC_ITEM)),0);

	// Insert "next/prev file"s
	m_WhichFileCombo.AddString(ID2W(IDS_ACTIONRESITEMNEXTFILE));
	m_WhichFileCombo.AddString(ID2W(IDS_ACTIONRESITEMPREVFILE));
	m_WhichFileCombo.AddString(ID2W(IDS_ACTIONRESITEMNEXTNONDELETEDFILE));
	m_WhichFileCombo.AddString(ID2W(IDS_ACTIONRESITEMPREVNONDELETEDFILE));
	m_WhichFileCombo.SetCurSel(0);
	
	// Insert "file/folder"/"Parent"
	m_ContextMenuForCombo.AddString(ID2W(IDS_ACTIONCONTEXTMENUFORFILE));
	m_ContextMenuForCombo.AddString(ID2W(IDS_ACTIONCONTEXTMENUFORPARENT));
	
	
	
	EnableItems();
	
	return CDialog::OnInitDialog(hwndFocus);
}



void CSelectColumnsDlg::OnHelp(LPHELPINFO lphi)
{
	if (m_ActionCombo.GetCurSel()>0 &&
		(lphi->iCtrlId==IDC_ACTION || lphi->iCtrlId==IDC_VERB ||
		lphi->iCtrlId==IDC_COMMAND || lphi->iCtrlId==IDC_VALUE ||
		lphi->iCtrlId==IDC_WHICHFILE || lphi->iCtrlId==IDC_CONTEXTMENUFOR ||
		lphi->iCtrlId==IDC_ITEM || lphi->iCtrlId==IDC_ITEMSPIN))
	{
		char szPage[50];
		StringCbPrintf(szPage,50,"actions_resultslist.htm#alist%d",m_ActionCombo.GetCurSel());
		if (CLocateApp::OpenHelp(*this,szPage))
			return;		
		
	}

	CLocateApp::HelpID id[]= {
		{ IDC_COLUMNS,"sdd_list"},
		{ IDC_UP,"sdd_move" },
		{ IDC_DOWN,"sdd_move" },
		{ IDC_SHOW,"sdd_show" },
		{ IDC_HIDE,"sdd_hide" },
		{ IDC_RESET,"sdd_reset" },
		{ IDC_RESETACTIONS,"sdd_resetactions" },
		{ IDC_WIDTH,"sdd_width" },
		{ IDC_WIDTHSPIN,"sdd_width" },
		{ IDC_LEFT,"sdd_left" },
		{ IDC_RIGHT,"sdd_right" },
		{ IDC_CENTER,"sdd_center" },
		{ IDC_WHEN,"sdd_when" },
		{ IDC_ACTION,"sdd_action" }		
	};

	if (CLocateApp::OpenHelp(*this,"dialog_selectdetails.htm",lphi,id,sizeof(id)/sizeof(CLocateApp::HelpID)))
		return;

	if (HtmlHelp(HH_HELP_CONTEXT,HELP_SETTINGS_SHORTCUTS)==NULL)
		HtmlHelp(HH_DISPLAY_TOPIC,0);
}


void CSelectColumnsDlg::SaveActionFields(ColumnItem* pColumn)
{
	int nWhen=m_WhenCombo.GetCurSel();
	ASSERT(nWhen!=CB_ERR);

	int nAction=m_ActionCombo.GetCurSel();
	ASSERT(nAction!=CB_ERR);

	if (nAction==0)
	{
		if (pColumn->m_pActions[nWhen]!=NULL)
		{
			delete pColumn->m_pActions[nWhen];
			pColumn->m_pActions[nWhen]=NULL;
		}
	}
	else
	{
		if (pColumn->m_pActions[nWhen]==NULL)
			pColumn->m_pActions[nWhen]=new CSubAction(nAction-1);
		else
		{
			pColumn->m_pActions[nWhen]->m_nSubAction=nAction-1;
			pColumn->m_pActions[nWhen]->ClearExtraInfo(CAction::ResultListItems);
		}

		switch (pColumn->m_pActions[nWhen]->m_nResultList)
		{
		case CAction::Execute:
			{
				int nSelection=m_VerbCombo.GetCurSel();
				if (nSelection==CB_ERR)
				{
					UINT nLen=(UINT)m_VerbCombo.GetTextLength();
					pColumn->m_pActions[nWhen]->m_szVerb=new WCHAR[nLen+1];
					m_VerbCombo.GetText(pColumn->m_pActions[nWhen]->m_szVerb,nLen+1);
				}
				else if (nSelection!=0)
				{
					UINT nLen=(UINT)m_VerbCombo.GetLBTextLen(nSelection);
					pColumn->m_pActions[nWhen]->m_szVerb=new WCHAR[nLen+1];
					m_VerbCombo.GetLBText(nSelection,pColumn->m_pActions[nWhen]->m_szVerb);
				}
				break;
			}
		case CAction::ExecuteCommand:
			{
				// Get command
				UINT nLen=GetDlgItemTextLength(IDC_COMMAND);
				if (nLen>0)
				{
					pColumn->m_pActions[nWhen]->m_szCommand=new WCHAR[nLen+1];
					GetDlgItemText(IDC_COMMAND,pColumn->m_pActions[nWhen]->m_szCommand,nLen+1);
				}
				break;
			}
		case CAction::OpenContextMenu:
		case CAction::OpenContextMenuSimple:
			pColumn->m_pActions[nWhen]->m_nContextMenuFor=(CSubAction::ContextMenuFor)m_ContextMenuForCombo.GetCurSel();
			if (int(pColumn->m_pActions[nWhen]->m_nContextMenuFor)==CB_ERR)
				pColumn->m_pActions[nWhen]->m_nContextMenuFor=CSubAction::FileOrFolder;
			break;
		case CAction::SelectFile:
			pColumn->m_pActions[nWhen]->m_nSelectFileType=(CSubAction::SelectFileType)m_WhichFileCombo.GetCurSel();
			if (int(pColumn->m_pActions[nWhen]->m_nSelectFileType)==CB_ERR)
				pColumn->m_pActions[nWhen]->m_nSelectFileType=CSubAction::NextFile;
			break;
		case CAction::SelectNthFile:
		case CAction::ExecuteNthFile:
			{
				BOOL bError;
				pColumn->m_pActions[nWhen]->m_nItem=(int)SendDlgItemMessage(IDC_ITEMSPIN,UDM_GETPOS32,0,(LPARAM)&bError)-1;
				if (bError)
				{
					pColumn->m_pActions[nWhen]->m_nItem=GetDlgItemInt(IDC_ITEM,&bError,FALSE)-1;
					bError=!bError;
				}

				if (bError || pColumn->m_pActions[nWhen]->m_nItem<0)
					pColumn->m_pActions[nWhen]->m_nItem=0;
				break;
			}
		}
	}
}
	
void CSelectColumnsDlg::SetActionFields(ColumnItem* pColumn)
{
	int nWhen=m_WhenCombo.GetCurSel();;
	ASSERT(nWhen!=CB_ERR);

	m_VerbCombo.SetCurSel(0);
	m_WhichFileCombo.SetCurSel(0);
	m_ContextMenuForCombo.SetCurSel(0);
	SetDlgItemText(IDC_COMMAND,szEmpty);
	SendDlgItemMessage(IDC_ITEMSPIN,UDM_SETPOS32,0,1);


	if (pColumn->m_pActions[nWhen]==NULL)
		m_ActionCombo.SetCurSel(0); // 0 = none
	else
	{
        m_ActionCombo.SetCurSel(pColumn->m_pActions[nWhen]->m_nSubAction+1);
		
        switch (pColumn->m_pActions[nWhen]->m_nResultList)
		{
		case CSubAction::Execute:
			if (pColumn->m_pActions[nWhen]->m_szVerb==NULL)
				m_VerbCombo.SetCurSel(0);
			else
			{
				m_VerbCombo.SetCurSel(-1);
				m_VerbCombo.SetText(pColumn->m_pActions[nWhen]->m_szVerb);
			}
			break;
		case CSubAction::ExecuteCommand:
			if (pColumn->m_pActions[nWhen]->m_szCommand!=NULL)
				SetDlgItemText(IDC_COMMAND,pColumn->m_pActions[nWhen]->m_szCommand);
			break;
		case CSubAction::OpenContextMenu:
		case CSubAction::OpenContextMenuSimple:
			m_ContextMenuForCombo.SetCurSel(pColumn->m_pActions[nWhen]->m_nContextMenuFor);	
			break;
		case CSubAction::SelectFile:
			m_WhichFileCombo.SetCurSel(pColumn->m_pActions[nWhen]->m_nSelectFileType);	
			break;
		case CSubAction::SelectNthFile:
		case CSubAction::ExecuteNthFile:
			SendDlgItemMessage(IDC_ITEMSPIN,UDM_SETPOS32,0,pColumn->m_pActions[nWhen]->m_nItem+1);
			break;
		}
	}

}


BOOL CSelectColumnsDlg::OnCommand(WORD wID,WORD wNotifyCode,HWND hControl)
{
	switch (wID)
	{
	case IDC_OK:
		OnOK();
		break;
	case IDCANCEL:
	case IDC_CANCEL:
		OnCancel();
		break;
	case IDC_SHOW:
	case IDC_HIDE:
		ASSERT(m_pList->GetNextItem(-1,LVNI_SELECTED)!=-1);

		m_pList->SetCheckState(m_pList->GetNextItem(-1,LVNI_SELECTED),wID==IDC_SHOW);
		m_pList->SetFocus();
		break;
	case IDC_LEFT:
		{
			int nItem=m_pList->GetNextItem(-1,LVNI_SELECTED);
			ASSERT(nItem!=-1);

			ColumnItem* pItem=(ColumnItem*)m_pList->GetItemData(nItem);
			if (pItem==NULL)
				break;

			pItem->m_nAlign=ColumnItem::Left;
			break;
		}
	case IDC_RIGHT:
		{
			int nItem=m_pList->GetNextItem(-1,LVNI_SELECTED);
			ASSERT(nItem!=-1);

			ColumnItem* pItem=(ColumnItem*)m_pList->GetItemData(nItem);
			if (pItem==NULL)
				break;

			pItem->m_nAlign=ColumnItem::Right;
			break;
		}
	case IDC_CENTER:
		{
			int nItem=m_pList->GetNextItem(-1,LVNI_SELECTED);
			ASSERT(nItem!=-1);

			ColumnItem* pItem=(ColumnItem*)m_pList->GetItemData(nItem);
			if (pItem==NULL)
				break;

			pItem->m_nAlign=ColumnItem::Center;
			break;
		}
	case IDC_UP:
		ItemUpOrDown(TRUE);
		break;
	case IDC_DOWN:
		ItemUpOrDown(FALSE);
		break;
	case IDC_WIDTH:
		if (wNotifyCode==EN_CHANGE)
		{
			int nItem=m_pList->GetNextItem(-1,LVNI_SELECTED);
			ASSERT(nItem!=-1);

			ColumnItem* pItem=(ColumnItem*)m_pList->GetItemData(nItem);
			if (pItem==NULL)
				break;

			BOOL bError;
			pItem->m_nWidth=(int)SendDlgItemMessage(IDC_WIDTHSPIN,UDM_GETPOS32,0,LPARAM(&bError));
			if (bError)
				pItem->m_nWidth=GetDlgItemInt(IDC_WIDTH,NULL,FALSE);

		}
		break;
	case IDC_WHEN:
		if (wNotifyCode==CBN_SELCHANGE)
		{
			int nItem=m_pList->GetNextItem(-1,LVNI_SELECTED);
			ASSERT(nItem!=-1);

			ColumnItem* pItem=(ColumnItem*)m_pList->GetItemData(nItem);
			if (pItem==NULL)
				break;

			SetActionFields(pItem);

			EnableItems();
		}
		break;
	case IDC_ACTION:
		if (wNotifyCode==CBN_SELCHANGE)
		{
			int nItem=m_pList->GetNextItem(-1,LVNI_SELECTED);
			ASSERT(nItem!=-1);

			ColumnItem* pItem=(ColumnItem*)m_pList->GetItemData(nItem);
			if (pItem==NULL)
				break;

			SaveActionFields(pItem);
			
			EnableItems();
		}
		break;
	case IDC_COMMAND:
	case IDC_VERB:
	case IDC_WHICHFILE:
	case IDC_CONTEXTMENUFOR:
		if (wNotifyCode==CBN_SELCHANGE || wNotifyCode==CBN_EDITCHANGE)
		{
			int nItem=m_pList->GetNextItem(-1,LVNI_SELECTED);
			ASSERT(nItem!=-1);

			ColumnItem* pItem=(ColumnItem*)m_pList->GetItemData(nItem);
			if (pItem==NULL)
				break;

			SaveActionFields(pItem);
		}
		break;
	case IDC_RESET:
		OnReset();
		break;
	case IDC_RESETACTIONS:
		OnResetActions();
		break;
	}

	return CDialog::OnCommand(wID,wNotifyCode,hControl);
}

BOOL CSelectColumnsDlg::ItemUpOrDown(BOOL bUp)
{
	int nSelected=m_pList->GetNextItem(-1,LVNI_SELECTED);
	ASSERT(nSelected!=-1);

	int nOther=m_pList->GetNextItem(nSelected,bUp?LVNI_ABOVE:LVNI_BELOW);
	if (nOther==-1 || nOther==nSelected)
		return FALSE;

	// This is found to be the best way to do this
	LVITEM li;
	BOOL bSelected=m_pList->GetCheckState(nSelected);
	li.mask=LVIF_STATE|LVIF_PARAM;
	li.stateMask=0xFFFFFFFF;
	li.iItem=nSelected;
	li.iSubItem=0;
	m_pList->GetItem(&li);
	m_pList->SetItemData(nSelected,NULL);
	m_pList->DeleteItem(nSelected);
	li.iItem=nOther;
	li.mask=LVIF_PARAM|LVIF_STATE|LVIF_TEXT;
	li.pszText=LPSTR_TEXTCALLBACK;
	nOther=m_pList->InsertItem(&li);
	m_pList->SetCheckState(nOther,bSelected);
	m_pList->EnsureVisible(nOther,FALSE);
	m_pList->SetFocus();
	return TRUE;
}

BOOL CSelectColumnsDlg::OnClose()
{
	CDialog::OnClose();
	EndDialog(0);
	return 0;
}

void CSelectColumnsDlg::OnDestroy()
{
	if (m_pList!=NULL)
	{
		delete m_pList;
		m_pList=NULL;
	}
	
	SavePosition(HKCU,CRegKey2::GetCommonKey()+"\\Dialogs","SelectColumnsWindowPos");
		
	return CDialog::OnDestroy();
}

BOOL CSelectColumnsDlg::OnNotify(int idCtrl,LPNMHDR pnmh)
{
	switch (idCtrl)
	{
	case IDC_COLUMNS:
		return ListNotifyHandler((NMLISTVIEW*)pnmh);
	}
	return CDialog::OnNotify(idCtrl,pnmh);
}

BOOL CSelectColumnsDlg::ListNotifyHandler(NMLISTVIEW *pNm)
{
	switch(pNm->hdr.code)
	{
	case LVN_GETDISPINFO:
		{
			LV_DISPINFO *pLvdi=(LV_DISPINFO *)pNm;
			ColumnItem* pItem=(ColumnItem*)pLvdi->item.lParam;
			if (pItem==NULL)
				break;
			pLvdi->item.pszText=alloccopyWtoA(pItem->m_strName,pItem->m_strName.GetLength());
			AssignBuffer(pLvdi->item.pszText);
			break;
		}
	case LVN_GETDISPINFOW:
		{
			LV_DISPINFOW *pLvdi=(LV_DISPINFOW*)pNm;
			ColumnItem* pItem=(ColumnItem*)pLvdi->item.lParam;
			if (pItem==NULL)
				break;
			pLvdi->item.pszText=pItem->m_strName.GetBuffer();
			break;
		}
	case LVN_DELETEITEM:
		if (pNm->lParam!=NULL)
			delete (ColumnItem*) pNm->lParam;
		break;
	case LVN_ITEMCHANGED:
		if (pNm->uNewState&(1<<12))
		{
			if (m_pList->GetItemState(pNm->iItem,LVNI_SELECTED))
			{
				EnableDlgItem(IDC_SHOW,TRUE);
				EnableDlgItem(IDC_HIDE,FALSE);
			}
			break;
		}
		else if (pNm->uOldState&(1<<12))		
		{
			if (m_pList->GetItemState(pNm->iItem,LVNI_SELECTED))
			{
				EnableDlgItem(IDC_SHOW,FALSE);
				EnableDlgItem(IDC_HIDE,TRUE);
			}
			break;
		}
		
		if (pNm->uNewState&LVIS_SELECTED && !(pNm->uOldState&LVIS_SELECTED))
		{
			// Item is selected
			ColumnItem* pItem=(ColumnItem*)pNm->lParam;
			if (pItem==NULL)
				break;
				
			

			::InvalidateRect(GetDlgItem(IDC_WIDTHSPIN),NULL,TRUE);
			SendDlgItemMessage(IDC_WIDTHSPIN,UDM_SETPOS,0,pItem->m_nWidth);


			CheckDlgButton(IDC_LEFT,pItem->m_nAlign==ColumnItem::Left);
			CheckDlgButton(IDC_RIGHT,pItem->m_nAlign==ColumnItem::Right);
			CheckDlgButton(IDC_CENTER,pItem->m_nAlign==ColumnItem::Center);

			m_WhenCombo.SetCurSel(0);
			SetActionFields(pItem);
		}
		else if (!(pNm->uNewState&LVIS_SELECTED) && (pNm->uOldState&LVIS_SELECTED))
		{
			// Item is deselected
			ColumnItem* pItem=(ColumnItem*)pNm->lParam;
			if (pItem==NULL)
				break;
			
			SaveActionFields(pItem);
		}
		
		EnableItems();
		break;
	}
	return FALSE;
}

void CSelectColumnsDlg::EnableItems()
{
	ShowState ssCommand=swHide,ssVerb=swHide,ssWhichFile=swHide,ssItem=swHide,ssContextMenuFor=swHide;
	
	int nItem=m_pList->GetNextItem(-1,LVNI_SELECTED);
	
	EnableDlgItem(IDC_TEXT,nItem!=-1);
	EnableDlgItem(IDC_WIDTH,nItem!=-1);
	
	if (nItem!=-1)
	{
        EnableDlgItem(IDC_UP,m_pList->GetNextItem(nItem,LVNI_ABOVE)!=-1);
		EnableDlgItem(IDC_DOWN,m_pList->GetNextItem(nItem,LVNI_BELOW)!=-1);

		BOOL bChecked=m_pList->GetCheckState(nItem);
		EnableDlgItem(IDC_SHOW,!bChecked);
		EnableDlgItem(IDC_HIDE,bChecked);

		int nAction=m_ActionCombo.GetCurSel();
		switch (nAction-1)
		{
		case CSubAction::Execute:
			ssVerb=swShow;
			break;
		case CSubAction::ExecuteCommand:
			ssCommand=swShow;
			break;
		case CSubAction::SelectFile:
			ssWhichFile=swShow;
			break;
		case CSubAction::SelectNthFile:
		case CSubAction::ExecuteNthFile:
			ssItem=swShow;
			break;
		case CSubAction::OpenContextMenu:
		case CSubAction::OpenContextMenuSimple:
			ssContextMenuFor=swShow;
			break;
		}

	}
	else
	{
		EnableDlgItem(IDC_UP,FALSE);
		EnableDlgItem(IDC_DOWN,FALSE);

		EnableDlgItem(IDC_HIDE,FALSE);
		EnableDlgItem(IDC_SHOW,FALSE);
	}


	
	::InvalidateRect(GetDlgItem(IDC_WIDTHSPIN),NULL,TRUE);

	EnableDlgItem(IDC_ALIGN,nItem!=-1);
	EnableDlgItem(IDC_LEFT,nItem!=-1);
	EnableDlgItem(IDC_RIGHT,nItem!=-1);
	EnableDlgItem(IDC_CENTER,nItem!=-1);

	EnableDlgItem(IDC_STATICACTIONS,nItem!=-1);
	EnableDlgItem(IDC_STATICWHEN,nItem!=-1);
	m_WhenCombo.EnableWindow(nItem!=-1);
	EnableDlgItem(IDC_STATICACTION,nItem!=-1);
	m_ActionCombo.EnableWindow(nItem!=-1);

	ShowDlgItem(IDC_STATICCOMMAND,ssCommand);
	ShowDlgItem(IDC_COMMAND,ssCommand);

	ShowDlgItem(IDC_STATICITEM,ssItem);
	ShowDlgItem(IDC_ITEM,ssItem);
	ShowDlgItem(IDC_ITEMSPIN,ssItem);

	ShowDlgItem(IDC_STATICVERB,ssVerb);
	m_VerbCombo.ShowWindow(ssVerb);

	ShowDlgItem(IDC_STATICWHICHFILE,ssWhichFile);
	m_WhichFileCombo.ShowWindow(ssWhichFile);
	
	ShowDlgItem(IDC_STATICCONTEXTMENUFOR,ssContextMenuFor);
	m_ContextMenuForCombo.ShowWindow(ssContextMenuFor);
}

void CSelectColumnsDlg::OnOK()
{
	m_aSelectedCols.RemoveAll();
	
	ASSERT(m_aWidths.GetSize()>0);
	ASSERT(m_aWidths.GetSize()==m_pList->GetItemCount());
	ASSERT(m_aAligns.GetSize()==m_pList->GetItemCount());
	ASSERT(m_aActions.GetSize()==m_pList->GetItemCount());

	int nIndex=m_pList->GetNextItem(-1,LVNI_SELECTED);
	if (nIndex!=-1)
	{
		// Save current state
		ColumnItem* pColumnItem=(ColumnItem*)m_pList->GetItemData(nIndex);
		if (pColumnItem!=NULL)
			SaveActionFields(pColumnItem);
	}

    nIndex=m_pList->GetNextItem(-1,LVNI_ALL);
	if (nIndex==-1)
	{
		ShowErrorMessage(IDS_ERRORNOCOLUMNSSELECTED,IDS_ERROR,MB_OK|MB_ICONINFORMATION);
		return;
	}

	while (nIndex!=-1)
	{
		ColumnItem* pItem=(ColumnItem*)m_pList->GetItemData(nIndex);
		m_aWidths[pItem->m_nCol]=pItem->m_nWidth; // Setting width
		m_aAligns[pItem->m_nCol]=pItem->m_nAlign; // Setting align
		m_aActions[pItem->m_nCol]=pItem->m_pActions;

		if (m_pList->GetCheckState(nIndex))
			m_aSelectedCols.Add(pItem->m_nCol);
		nIndex=m_pList->GetNextItem(nIndex,LVNI_ALL);
	}

	

	EndDialog(1);
}

void CSelectColumnsDlg::OnCancel()
{
	for (int i=0;i<m_aActions.GetSize();i++)
		delete[] m_aActions[i];
	m_aActions.RemoveAll();

	EndDialog(0);
}

void CSelectColumnsDlg::OnReset()
{
	m_pList->DeleteAllItems();


	CLocateDlg::ViewDetails* pDetails=CLocateDlg::GetDefaultDetails();

	int nItem=0,i;
	for (i=0;i<TypeCount;i++)
	{
		if (pDetails[i].bShow)
		{
			if (m_pList->InsertItem(LVIF_TEXT|LVIF_PARAM,nItem,LPSTR_TEXTCALLBACK,0,0,0,
				LPARAM(new ColumnItem(i,DetailType(i),
				pDetails[i].nString,pDetails[i].nWidth,
				(ColumnItem::Align)pDetails[i].nAlign,m_aActions[i])))>=0)
			{
				m_pList->SetCheckState(nItem++,TRUE);
			}
		}
	}
	for (i=0;i<TypeCount;i++)
	{
		if (!pDetails[i].bShow)
		{
			if (m_pList->InsertItem(LVIF_TEXT|LVIF_PARAM,nItem,LPSTR_TEXTCALLBACK,0,0,0,
				LPARAM(new ColumnItem(i,DetailType(i),
				pDetails[i].nString,pDetails[i].nWidth,
				(ColumnItem::Align)pDetails[i].nAlign,m_aActions[i])))>0)
			{
				m_pList->SetCheckState(nItem++,FALSE);
			}
		}
	}

	delete[] pDetails;

	EnableItems();
}

void CSelectColumnsDlg::OnResetActions()
{
	for (int i=0;i<m_aActions.GetSize();i++)
	{
		for (int j=0;j<CLocateDlg::ListActionCount;j++)
		{
			if (m_aActions[i][j]!=NULL)
			{
				delete m_aActions[i][j];
				m_aActions[i][j]=NULL;
			}
		}
	}
	
	GetLocateDlg()->SetDefaultActions(m_aActions);

	int nItem=m_pList->GetNextItem(-1,LVNI_SELECTED);
	if (nItem!=-1)
	{
		ColumnItem* pItem=(ColumnItem*)m_pList->GetItemData(nItem);
		if (pItem!=NULL)
			SetActionFields(pItem);
	}

	EnableItems();
}


///////////////////////////////////////////////////////////
// CSelectDatabasesDlg


CSelectDatabasesDlg::~CSelectDatabasesDlg()
{
	if (m_pRegKey!=NULL)
		delete[] m_pRegKey;
	if (m_pSelectDatabases!=NULL)
	{
		delete[] m_pSelectDatabases;
		m_pSelectDatabases=NULL;
	}
}


void CSelectDatabasesDlg::OnHelp(LPHELPINFO lphi)
{
	if (HtmlHelp(HH_HELP_CONTEXT,lphi->iCtrlId)==NULL)
		HtmlHelp(HH_DISPLAY_TOPIC,0);
}


BOOL CSelectDatabasesDlg::OnInitDialog(HWND hwndFocus)
{
	CDialog::OnInitDialog(hwndFocus);
	
	if (!LoadPosition(HKCU,CRegKey2::GetCommonKey()+"\\Dialogs","SelectDatabasesWindowPos",fgOnlyNormalPosition|fgNoSize))
		CenterWindow();



	m_PresetCombo.AttachToDlgItem(*this,IDC_PRESETS);

	// Creating list control
	m_List.AttachToDlgItem(*this,IDC_DATABASES);
	if (IsUnicodeSystem())
		m_List.SetUnicodeFormat(TRUE);
	m_List.SetExtendedListViewStyle(LVS_EX_CHECKBOXES|LVS_EX_HEADERDRAGDROP|LVS_EX_FULLROWSELECT,
		LVS_EX_CHECKBOXES|LVS_EX_HEADERDRAGDROP|LVS_EX_FULLROWSELECT);
	
	m_List.InsertColumn(0,ID2W(IDS_DATABASENAME),LVCFMT_LEFT,100,0);
	m_List.InsertColumn(1,ID2W(IDS_DATABASEFILE),LVCFMT_LEFT,130,0);
	m_List.InsertColumn(2,ID2W(IDS_DATABASECREATOR),LVCFMT_LEFT,70,0);
	m_List.InsertColumn(3,ID2W(IDS_DATABASEDESCRIPTION),LVCFMT_LEFT,100,0);
	
	if (!(m_bFlags&flagShowThreads))
	{
		// Hiding controls related to threads
		ShowDlgItem(IDC_THREADSLABEL,swHide);
		ShowDlgItem(IDC_THREADS,swHide);
		ShowDlgItem(IDC_THREADSPIN,swHide);
		ShowDlgItem(IDC_UP,swHide);
		ShowDlgItem(IDC_DOWN,swHide);

		CRect rect;
		::GetWindowRect(GetDlgItem(IDC_DATABASES),&rect);
		SetDlgItemPos(IDC_DATABASES,NULL,0,0,rect.Width(),rect.Height()+28,
			SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
		
	}
	else 
	{
		if (((CLocateApp*)GetApp())->m_wComCtrlVersion<0x0600)
		{
			m_List.InsertColumn(4,ID2A(IDS_THREADID),LVCFMT_LEFT,40,0);
			int oa[]={3,0,1,2};
			m_List.SetColumnOrderArray(4,oa);
		}

		// Setting threads counter
		CSpinButtonCtrl Spin(GetDlgItem(IDC_THREADSPIN));
		Spin.SetBuddy(GetDlgItem(IDC_THREADS));
		Spin.SetRange(1,99);
	}

	if (m_bFlags&flagDisablePresets)
	{
		EnableDlgItem(IDC_PRESETSLABEL,FALSE);
		m_PresetCombo.EnableWindow(FALSE);
		EnableDlgItem(IDC_PRESETSLABEL,FALSE);
		EnableDlgItem(IDC_SAVE,FALSE);
		EnableDlgItem(IDC_DELETE,FALSE);
	}

	if (m_bFlags&flagEnablePriority)
	{
		CComboBox Combo(GetDlgItem(IDC_THREADPRIORITY));
		Combo.AddString(ID2W(IDS_PRIORITYHIGH));		
		Combo.AddString(ID2W(IDS_PRIORITYABOVENORMAL));		
		Combo.AddString(ID2W(IDS_PRIORITYNORMAL));		
		Combo.AddString(ID2W(IDS_PRIORITYBELOWNORMAL));		
		Combo.AddString(ID2W(IDS_PRIORITYLOW));		
		Combo.AddString(ID2W(IDS_PRIORITYIDLE));	
		if (GetSystemFeaturesFlag()&efWinVista)
			Combo.AddString(ID2W(IDS_PRIORITYBACKGROUND));	

		switch (m_nThreadPriority)
		{
		case THREAD_PRIORITY_HIGHEST:
			Combo.SetCurSel(0);
			break;
		case THREAD_PRIORITY_ABOVE_NORMAL:
            Combo.SetCurSel(1);
			break;
		case THREAD_PRIORITY_NORMAL:
            Combo.SetCurSel(2);
			break;
		case THREAD_PRIORITY_BELOW_NORMAL:
            Combo.SetCurSel(3);
			break;
		case THREAD_PRIORITY_LOWEST:
            Combo.SetCurSel(4);
			break;
		case THREAD_PRIORITY_IDLE:
            Combo.SetCurSel(5);
			break;
		case THREAD_MODE_BACKGROUND_BEGIN:
			Combo.SetCurSel(6);
			break;
		default:
		    Combo.SetCurSel(2);
			break;
		}
	}
	else
	{
		ShowDlgItem(IDC_THREADPRIORITY,swHide);
		ShowDlgItem(IDC_THREADPRIORITYLABEL,swHide);
	}
	
	
	if (m_bFlags&flagEnableUseDatabases)
		CheckDlgButton(m_bUseTemporally?IDC_USEDATABASESTEMPORALLY:IDC_USEDATABASESONCE,TRUE);
	else
	{
		ShowDlgItem(IDC_USEDATABASESLABEL,swHide);
		ShowDlgItem(IDC_USEDATABASESONCE,swHide);
		ShowDlgItem(IDC_USEDATABASESTEMPORALLY,swHide);
	}

	m_List.LoadColumnsState(HKCU,m_pRegKey,"Database List Widths");
	
	LoadPresets();

	EnableButtons();    
	return FALSE;
}

BOOL CSelectDatabasesDlg::OnCommand(WORD wID,WORD wNotifyCode,HWND hControl)
{
	CDialog::OnCommand(wID,wNotifyCode,hControl);
	switch (wID)
	{
	case IDC_OK:
		OnOK();
		break;
	case IDCANCEL:
	case IDC_CANCEL:
		EndDialog(0);
		break;
	case IDC_DOWN:
	case IDC_UP:
		if (ItemUpOrDown(wID==IDC_UP))
			m_PresetCombo.SetCurSel(CUSTOM_PRESET);
		break;
	case IDC_SAVE:
		{
			CSavePresetDlg pd(this);
			pd.DoModal(*this);
			break;
		}
	case IDC_PRESETS:
		if (wNotifyCode==CBN_SELCHANGE)
			OnPresetCombo();
		break;
	case IDC_DELETE:
		OnDeletePreset();
		break;
	case IDC_THREADS:
		if (wNotifyCode==EN_CHANGE)
			OnThreads();
		else if (wNotifyCode==EN_SETFOCUS)
			SendDlgItemMessage(IDC_THREADS,EM_SETSEL,0,-1);
		break;
	}
	return FALSE;
}

void CSelectDatabasesDlg::SelectDatabases(LPCWSTR pNames)
{
	if (m_pSelectDatabases!=NULL)
		delete[] m_pSelectDatabases;
    if (pNames==NULL)
	{
		m_pSelectDatabases=NULL;
		m_bFlags&=~flagSelectedMask;
		m_bFlags|=flagGlobalIsSelected;
		return;
	}
	
	DWORD dwLength=1;
	LPCWSTR pPtr=pNames;
	while (*pPtr!='\0')
	{
		int iStrLen=int(istrlenw(pPtr)+1);
		dwLength+=iStrLen;
		pPtr+=iStrLen;
	}
	m_pSelectDatabases=alloccopy(pNames,dwLength);
}

BOOL CSelectDatabasesDlg::OnClose()
{
	CDialog::OnClose();
	EndDialog(0);
	return FALSE;
}

void CSelectDatabasesDlg::OnDestroy()
{
	m_List.SaveColumnsState(HKCU,m_pRegKey,"Database List Widths");

	SavePosition(HKCU,CRegKey2::GetCommonKey()+"\\Dialogs","SelectDatabasesWindowPos");

	CDialog::OnDestroy();
}

BOOL CSelectDatabasesDlg::OnNotify(int idCtrl,LPNMHDR pnmh)
{
	switch (idCtrl)
	{
	case IDC_DATABASES:
		return ListNotifyHandler((NMLISTVIEW*)pnmh);
	}
	return CDialog::OnNotify(idCtrl,pnmh);
}

void CSelectDatabasesDlg::OnOK()
{
	//DebugFormatMessage("CSelectDatabasesDlg::OnOK() BEGIN, Update: %d, bReturnNotSelected: %d",
	//	m_bFlags&flagSetUpdateState?TRUE:FALSE,m_bFlags&flagReturnNotSelected?TRUE:FALSE);

	// Saves last set
	if (!(m_bFlags&flagDisablePresets))
		SavePreset(NULL);

	// Retrieve priority
	if (m_bFlags&flagEnablePriority)
	{
		switch (SendDlgItemMessage(IDC_THREADPRIORITY,CB_GETCURSEL))
		{
		case 0:
			m_nThreadPriority=THREAD_PRIORITY_HIGHEST;
            break;
		case 1:
            m_nThreadPriority=THREAD_PRIORITY_ABOVE_NORMAL;
            break;
		case 3:
			m_nThreadPriority=THREAD_PRIORITY_BELOW_NORMAL;
            break;
		case 4:
            m_nThreadPriority=THREAD_PRIORITY_LOWEST;
            break;
		case 5:
            m_nThreadPriority=THREAD_PRIORITY_IDLE;
            break;
		case 6:
			m_nThreadPriority=THREAD_MODE_BACKGROUND_BEGIN;
            break;
		default:
		    m_nThreadPriority=THREAD_PRIORITY_NORMAL;
            break;
		}
	}
	else if (m_bFlags&flagEnableUseDatabases)
	{
		m_bUseTemporally=IsDlgButtonChecked(IDC_USEDATABASESTEMPORALLY);

		CRegKey RegKey;
		if(RegKey.OpenKey(HKCU,m_pRegKey,CRegKey::defWrite)==ERROR_SUCCESS)
			RegKey.SetValue("Use databases temporally",(DWORD)m_bUseTemporally);
	}
	
	if (m_List.GetItemCount()>0)
	{
		// Get the first item
		//int nNext;
		int nItem=m_List.GetNextItem(-1,LVNI_ALL);

		/*while ((nNext=m_List.GetNextItem(nItem,LVNI_ABOVE))!=-1)
		{
			if (nNext==nItem)
				break; // This should not be like that, why is it?
			nItem=nNext;
		}*/
		
		while (nItem!=-1)
		{
			PDATABASE pDatabase=(PDATABASE)m_List.GetItemData(nItem);
			ASSERT(pDatabase!=NULL);

			if (m_bFlags&flagReturnNotSelected || IsItemEnabled(pDatabase))
			{
				//DebugFormatMessage("Database %S is selected",pDatabase->GetName());

				m_rSelectedDatabases.Add(pDatabase);
				m_List.SetItemData(nItem,NULL);
			}

			/*nNext=m_List.GetNextItem(nItem,LVNI_BELOW);
			if (nNext==nItem)
				break;
			nItem=nNext;*/

			nItem=m_List.GetNextItem(nItem,LVNI_ALL);
		}
	}

	int nCurSel=m_PresetCombo.GetCurSel();
	//DebugFormatMessage("Selection: %d",nCurSel);
	m_bFlags&=~flagSelectedMask;
	switch (nCurSel)
	{
	case GLOBAL_PRESET:
		m_bFlags|=flagGlobalIsSelected;
		//DebugMessage("preset chosen: global selected");
		break;
	case CUSTOM_PRESET:
        m_bFlags|=flagCustomIsSelected;
		//DebugMessage("preset chosen: preset");
		break;
	case LATEST_PRESET:
		m_bFlags|=flagLasestIsSelected;
		//DebugMessage("preset chosen: latest");
		break;
	}
	EndDialog(1);

	//DebugMessage("CSelectDatabasesDlg::OnOK() END");
}

void CSelectDatabasesDlg::OnDeletePreset()
{
	int nCurSel=(int)SendDlgItemMessage(IDC_PRESETS,CB_GETCURSEL);
	if (nCurSel==CB_ERR)
		return;
	
	
	int nTextLen=(int)SendDlgItemMessage(IDC_PRESETS,CB_GETLBTEXTLEN,nCurSel);
	if (nTextLen==CB_ERR)
		return;
	char* pText=new char[nTextLen+1];
    if (SendDlgItemMessage(IDC_PRESETS,CB_GETLBTEXT,nCurSel,LPARAM(pText))==CB_ERR)
	{
		delete[] pText;
		return;
	}
	
	SendDlgItemMessage(IDC_PRESETS,CB_DELETESTRING,nCurSel);
	

	CRegKey RegKey;
	if (RegKey.OpenKey(HKCU,m_pRegKey,CRegKey::openExist|CRegKey::samAll)==ERROR_SUCCESS)
	{
		CString sName;
		BOOL bBreaked=FALSE;
		for (int i=0;RegKey.EnumValue(i,sName)>0;i++)
		{
			if (strncmp(sName,"Preset ",7)==0)
			{
				int nIndex=int(sName.FindFirst(':'))+1;

                if (nIndex>0 &&  nIndex<sName.GetLength()) 
				{
					if (strcasecmp(LPCSTR(sName)+nIndex,pText)==0)
					{
						RegKey.DeleteValue(sName);
						break;
					}
				}
			}
		}

		while (CheckRegistryIntegrity(RegKey));
		
	}
	delete[] pText;

	SendDlgItemMessage(IDC_PRESETS,CB_SETCURSEL,CUSTOM_PRESET);

	// Enable/disable delete
	EnableButtons();
}

BOOL CSelectDatabasesDlg::CheckRegistryIntegrity(CRegKey& RegKey)
{
	int nShouldBe=0;
	CString sName;
	for (int i=0;RegKey.EnumValue(i,sName)>0;i++)
	{
		if (strncmp(sName,"Preset ",7)==0)
		{
			int nIndex=int(sName.FindFirst(':'))+1;
			if (nIndex>0 &&  nIndex<sName.GetLength()) 
			{
				int nIs=atoi(LPCSTR(sName)+7);
				if (nIs!=nShouldBe)
				{
					// Renaming
					DWORD dwLength=RegKey.QueryValueLength(sName);
					char* pData=new char[dwLength+2];
					DWORD dwType;
					RegKey.QueryValue(sName,pData,dwLength,&dwType);
					RegKey.DeleteValue(sName);
						CString name(LPCSTR(sName)+nIndex);
					sName.Format("Preset %03d:%s",nShouldBe,LPCSTR(name));
					RegKey.SetValue(sName,pData,dwLength,dwType);

					return TRUE;
				}
				nShouldBe++;
			}
		}
	}
	return FALSE;
}

void CSelectDatabasesDlg::OnThreads()
{
	int nThreads=GetDlgItemInt(IDC_THREADS);

	if (nThreads<1)
		SetDlgItemInt(IDC_THREADS,nThreads=1,FALSE);

	ChangeNumberOfThreads(nThreads);
	EnableButtons();
}

void CSelectDatabasesDlg::OnPresetCombo()
{
	// Enable/disable delete
	EnableButtons();

	int nCurSel=(int)SendDlgItemMessage(IDC_PRESETS,CB_GETCURSEL);
	if (nCurSel==CB_ERR)
		return;

	if (nCurSel==CUSTOM_PRESET)
		return;
	if (nCurSel==GLOBAL_PRESET) // Global
		InsertDatabases();
	else if (nCurSel==LATEST_PRESET)
		LoadPreset(NULL);
	else if (nCurSel==SELECTED_PRESET && m_pSelectDatabases!=NULL )
		InsertSelected();
	else
	{
		int nTextLen=m_PresetCombo.GetLBTextLen(nCurSel);
		if (nTextLen==CB_ERR)
			return;
		WCHAR* pText=new WCHAR[nTextLen+1];
		if (m_PresetCombo.GetLBText(nCurSel,pText))
			LoadPreset(pText);
		delete[] pText;
	}
}

BOOL CSelectDatabasesDlg::ListNotifyHandler(NMLISTVIEW *pNm)
{
	switch(pNm->hdr.code)
	{
	case LVN_ITEMCHANGED:
		if (pNm->lParam!=NULL && (pNm->uNewState&0x00002000)!=(pNm->uOldState&0x00002000))
		{
			if (EnableItem((CDatabase*)pNm->lParam,m_List.GetCheckState(pNm->iItem)))
				m_PresetCombo.SetCurSel(CUSTOM_PRESET);
		}
		break;
	case NM_CLICK:
		EnableButtons();
		break;
	case LVN_DELETEITEM:
		if (pNm->lParam!=NULL)
			delete (CDatabase*)pNm->lParam;
		break;
	case LVN_GETDISPINFOA:
		{
			LV_DISPINFO *pLvdi=(LV_DISPINFO *)pNm;

			CDatabase* pDatabase=(CDatabase*)pLvdi->item.lParam;
			if (pDatabase==NULL)
				break;
				
			switch (pLvdi->item.iSubItem)
			{
			case 0:
				m_Buffer=pDatabase->GetName();
				break;
			case 1:
				m_Buffer=pDatabase->GetArchiveName();
				break;
			case 2:
				m_Buffer=pDatabase->GetCreator();
				break;
			case 3:
				m_Buffer=pDatabase->GetDescription();
				break;
			case 4:
				m_Buffer=pDatabase->GetThreadId()+1;
				break;
			}
			pLvdi->item.pszText=m_Buffer.GetBuffer();
				
			break;
		}
	case LVN_GETDISPINFOW:
		{
			LV_DISPINFOW *pLvdi=(LV_DISPINFOW *)pNm;

			CDatabase* pDatabase=(CDatabase*)pLvdi->item.lParam;
			if (pDatabase==NULL)
				break;
				
			switch (pLvdi->item.iSubItem)
			{
			case 0:
				pLvdi->item.pszText=const_cast<LPWSTR>(pDatabase->GetName());
				break;
			case 1:
				pLvdi->item.pszText=const_cast<LPWSTR>(pDatabase->GetArchiveName());
				break;
			case 2:
				pLvdi->item.pszText=const_cast<LPWSTR>(pDatabase->GetCreator());
				break;
			case 3:
				pLvdi->item.pszText=const_cast<LPWSTR>(pDatabase->GetDescription());
				break;
			case 4:
				m_BufferW=pDatabase->GetThreadId()+1;
				pLvdi->item.pszText=m_BufferW.GetBuffer();
				break;
			}
			break;
		}
	}
	return 0;
}


BOOL CSelectDatabasesDlg::ItemUpOrDown(BOOL bUp)
{
	int nSelected=m_List.GetNextItem(-1,LVNI_SELECTED);
	ASSERT(nSelected!=-1);
	CDatabase* pSelected=(CDatabase*)m_List.GetItemData(nSelected);
	
	int nOther=m_List.GetNextItem(nSelected,bUp?LVNI_ABOVE:LVNI_BELOW);
	if (nOther==-1 || nOther==nSelected)
	{
		if (!(m_bFlags&flagShowThreads))
			return FALSE;
		if (bUp && pSelected->GetThreadId()>0)
			return IncreaseThread(nSelected,pSelected,TRUE);
		else if (!bUp && pSelected->GetThreadId()<m_nThreadsCurrently-1)
			return IncreaseThread(nSelected,pSelected,FALSE);
		return FALSE;
	}

	CDatabase* pOther=(CDatabase*)m_List.GetItemData(nOther);
	if (m_bFlags&flagShowThreads && pOther->GetThreadId()!=pSelected->GetThreadId())
	{
		ASSERT(bUp?pSelected->GetThreadId()>0:pSelected->GetThreadId()<m_nThreadsCurrently-1);
		return IncreaseThread(nSelected,pSelected,bUp);
	}

	// This is working in this dialog! Wou
	LPARAM pParam=m_List.GetItemData(nSelected);
	m_List.SetItemData(nSelected,m_List.GetItemData(nOther));
	m_List.SetItemData(nOther,pParam);
	UINT nState=m_List.GetItemState(nSelected,0xFFFFFFFF);
	m_List.SetItemState(nSelected,m_List.GetItemState(nOther,0xFFFFFFFF),0xFFFFFFFF);
	m_List.SetItemState(nOther,nState,0xFFFFFFFF);

	m_List.EnsureVisible(nOther,FALSE);
	m_List.RedrawItems(min(nSelected,nOther),max(nSelected,nOther));

	m_List.UpdateWindow();
	
	EnableButtons();
	m_List.SetFocus();
	return TRUE;
}	

BOOL CSelectDatabasesDlg::InsertDatabases()
{
	m_List.DeleteAllItems();

	// Counting highest thread
	if (m_bFlags&flagShowThreads)
	{
		WORD wHighestThread=0;
		for (int i=0;i<m_rOrigDatabases.GetSize();i++)
		{
			if (wHighestThread<m_rOrigDatabases[i]->GetThreadId())
				wHighestThread=m_rOrigDatabases[i]->GetThreadId();
		}
		ChangeNumberOfThreads(wHighestThread+1);
		SendDlgItemMessage(IDC_THREADSPIN,UDM_SETPOS,0,MAKELONG(m_nThreadsCurrently,0));
	}
	
	

	// Checking whether groups should be taken care of
	LVITEM li;
	li.pszText=LPSTR_TEXTCALLBACK;
	if (m_bFlags&flagShowThreads && m_nThreadsCurrently>1 && ((CLocateApp*)GetApp())->m_wComCtrlVersion>=0x0600)
        li.mask=LVIF_TEXT|LVIF_PARAM|LVIF_GROUPID;
	else
		li.mask=LVIF_TEXT|LVIF_PARAM;
	li.iSubItem=0;
	
	for (li.iItem=0;li.iItem<m_rOrigDatabases.GetSize();li.iItem++)
	{
		CDatabase* pDatabase=new CDatabase(*m_rOrigDatabases[li.iItem]);
		li.lParam=LPARAM(pDatabase);
		if (m_bFlags&flagShowThreads)
			li.iGroupId=pDatabase->GetThreadId();
		else
		{
			li.iGroupId=0;
			pDatabase->SetThreadId(0);
		}

		m_List.SetCheckState(m_List.InsertItem(&li),IsItemEnabled(pDatabase));
	}

	return TRUE;
}

BOOL CSelectDatabasesDlg::InsertSelected()
{
	if (m_pSelectDatabases==NULL)
		return FALSE;

	CArray<WORD> aDatabaseIDs;
	CArray<WORD> aThreads;
	WORD wThreads=1;

	LPWSTR pPtr=m_pSelectDatabases;
	while (*pPtr!=L'\0')
	{
		int iStrLen=istrlenw(pPtr);
		int iNameLen=FirstCharIndex(pPtr,L'\\');
		
		if (iNameLen==-1)
			iNameLen=iStrLen;

	
		for (int i=0;i<m_rOrigDatabases.GetSize();i++)
		{
			if (wcsncmp(pPtr,m_rOrigDatabases[i]->GetName(),iNameLen)==0)
			{
				if (pPtr[iNameLen]==L'\0' || pPtr[iNameLen]==L'\\')
				{
					aDatabaseIDs.Add(m_rOrigDatabases[i]->GetID());
					WORD wThread=0;
					if (iNameLen!=iStrLen)
						wThread=_wtoi(pPtr+iNameLen+1);
					aThreads.Add(wThread);
					if (wThreads<=wThread)
						wThreads=wThread+1;
					break;
				}
			}
		}
		pPtr+=iStrLen+1;
	}

	for (int i=0;i<m_rOrigDatabases.GetSize();i++)
	{
		if (wThreads<=m_rOrigDatabases[i]->GetThreadId())
			wThreads=m_rOrigDatabases[i]->GetThreadId()+1;
	}

	InsertDatabases(aDatabaseIDs.GetSize(),wThreads,aDatabaseIDs.GetData(),
		aThreads.GetData(),aDatabaseIDs.GetSize(),aDatabaseIDs.GetData());
	return TRUE;
}
	

BOOL CSelectDatabasesDlg::InsertDatabases(WORD wCount,WORD wThreads,const WORD* pwDatabaseIDs,const WORD* pwThreads,
										  WORD wSelectedCount,const WORD* pwSelectedIds)
{
	m_List.DeleteAllItems();

	ChangeNumberOfThreads(wThreads);
	SendDlgItemMessage(IDC_THREADSPIN,UDM_SETPOS,0,MAKELONG(m_nThreadsCurrently,0));

	// Handled dbs
	BYTE* pTaken=new BYTE[max(m_rOrigDatabases.GetSize(),2)];
	ZeroMemory(pTaken,m_rOrigDatabases.GetSize());
    
	LVITEM li;
	li.pszText=LPSTR_TEXTCALLBACK;
	if (m_bFlags&flagShowThreads && m_nThreadsCurrently>1 && ((CLocateApp*)GetApp())->m_wComCtrlVersion>=0x0600)
        li.mask=LVIF_TEXT|LVIF_PARAM|LVIF_GROUPID;
	else
		li.mask=LVIF_TEXT|LVIF_PARAM;
	li.iSubItem=0;
	li.iItem=0;

	for (int i=0;i<wCount;i++)
	{
		CDatabase* pDatabase=NULL;

		// Searching database
		for (int j=0;j<m_rOrigDatabases.GetSize();j++)
		{
			if (m_rOrigDatabases[j]->GetID()==pwDatabaseIDs[i])
			{
				pDatabase=new CDatabase(*m_rOrigDatabases[j]);
				pTaken[j]=TRUE;
				break;
			}
		}

		if (pDatabase==NULL)
			continue; // Deleted database

		// Inserting item
		li.lParam=LPARAM(pDatabase);
		if (m_bFlags&flagShowThreads)
		{
			li.iGroupId=pwThreads[i];
			pDatabase->SetThreadId(pwThreads[i]);
		}
		else
		{
			li.iGroupId=0;
			pDatabase->SetThreadId(0);
		}
		li.iItem=m_List.InsertItem(&li);

		BOOL bSelected=FALSE;
		for (int j=0;j<wSelectedCount;j++)
		{
			if (pDatabase->GetID()==pwSelectedIds[j])
			{
				bSelected=TRUE;
				break;
			}
		}

		EnableItem(pDatabase,bSelected);
		m_List.SetCheckState(li.iItem,bSelected);
		li.iItem++;
	}


	// Inserting databases which are not eat inserted
	for (int i=0;i<m_rOrigDatabases.GetSize();i++)
	{
		if (pTaken[i]==0)
		{
			CDatabase* pDatabase=new CDatabase(*m_rOrigDatabases[i]);
			
			// Inserting item
			li.lParam=LPARAM(pDatabase);
			if (m_bFlags&flagShowThreads)
				li.iGroupId=pDatabase->GetThreadId();
			else
			{
				li.iGroupId=0;
				pDatabase->SetThreadId(0);
			}
			li.iItem=m_List.InsertItem(&li);

			EnableItem(pDatabase,FALSE);
			m_List.SetCheckState(li.iItem,FALSE);
			li.iItem++;
		}
	}

	delete[] pTaken;

	return TRUE;
}




void CSelectDatabasesDlg::EnableThreadGroups(int nThreadGroups)
{
	if (m_List.IsGroupViewEnabled())
		return;

	m_List.EnableGroupView(TRUE);
	m_nThreadsCurrently=nThreadGroups;
	
	// Creating groups
	CStringW str;
	LVGROUP lg;
	dMemSet(&lg,0,sizeof(LVGROUP));
	lg.cbSize=sizeof(LVGROUP);
	lg.mask=LVGF_HEADER|LVGF_GROUPID|LVGF_ALIGN;
	lg.state=LVGS_NORMAL;
	lg.uAlign=LVGA_HEADER_LEFT;

	for (lg.iGroupId=0;lg.iGroupId<nThreadGroups;lg.iGroupId++)
	{
		str.Format(IDS_THREADNAME,lg.iGroupId+1);
		lg.pszHeader=str.GetBuffer();

		m_List.InsertGroup(lg.iGroupId,&lg);
	}

	if (m_List.GetItemCount()>0)
	{
		// Setting groups IDs
		LVITEM li;
		li.mask=LVIF_GROUPID;
		li.iItem=m_List.GetNextItem(-1,LVNI_ALL);
		li.iSubItem=0;
		while (li.iItem!=-1)
		{
			CDatabase* pDatabase=(CDatabase*)m_List.GetItemData(li.iItem);
			ASSERT(pDatabase!=NULL);

			li.iGroupId=pDatabase->GetThreadId();
			
			m_List.SetItem(&li);
			li.iItem=m_List.GetNextItem(li.iItem,LVNI_ALL);
		}
	}
}

void CSelectDatabasesDlg::RemoveThreadGroups()
{
	if (!m_List.IsGroupViewEnabled())
		return;

	m_List.RemoveAllGroups();
	m_List.EnableGroupView(FALSE);
}

void CSelectDatabasesDlg::ChangeNumberOfThreads(int nThreads)
{
	if (nThreads>1 && !(m_bFlags&flagShowThreads))
		return;
	
	if (nThreads>m_nThreadsCurrently)
	{
		// Number is increased
		if (((CLocateApp*)GetApp())->m_wComCtrlVersion<0x0600)
		{
			m_nThreadsCurrently=nThreads;
			return;
		}

		if (m_nThreadsCurrently==1)
			EnableThreadGroups(nThreads);
		else
		{
			// Insertig new thread groups
			CStringW str;
			LVGROUP lg;
			dMemSet(&lg,0,sizeof(LVGROUP));
			lg.cbSize=sizeof(LVGROUP);
			lg.mask=LVGF_HEADER|LVGF_GROUPID|LVGF_ALIGN;
			lg.state=LVGS_NORMAL;
			lg.uAlign=LVGA_HEADER_LEFT;

			for (lg.iGroupId=m_nThreadsCurrently;lg.iGroupId<nThreads;lg.iGroupId++)
			{
				str.Format(IDS_THREADNAME,lg.iGroupId+1);
				lg.pszHeader=str.GetBuffer();

				m_List.InsertGroup(lg.iGroupId,&lg);
			}
		
		}
		m_nThreadsCurrently=nThreads;
		m_List.RedrawItems(0,m_List.GetItemCount());
	}
	else if (nThreads<m_nThreadsCurrently)
	{
		if (m_List.GetItemCount()>0)
		{
			// Ensuring that there is no any items with higher thread ID than available
			int nItem=m_List.GetNextItem(-1,LVNI_ALL);
			while (nItem!=-1)
			{
				CDatabase* pDatabase=(CDatabase*)m_List.GetItemData(nItem);
				ASSERT(pDatabase!=NULL);

				if (pDatabase->GetThreadId()>=nThreads)
				{
					pDatabase->SetThreadId(nThreads-1);
					
					LVITEM li;
					li.iItem=nItem;
					li.iSubItem=0;
					li.mask=LVIF_GROUPID;
					li.iGroupId=nThreads-1;
					m_List.SetItem(&li);
				}
				nItem=m_List.GetNextItem(nItem,LVNI_ALL);
			}
		}

		if (((CLocateApp*)GetApp())->m_wComCtrlVersion>=0x0600)
		{
			if (nThreads==1)
				RemoveThreadGroups();
			else
			{
				// Removing unused thread groups
				while (m_nThreadsCurrently>nThreads)
					m_List.RemoveGroup(--m_nThreadsCurrently);
			}
		}
		m_nThreadsCurrently=nThreads;

		m_List.RedrawItems(0,m_List.GetItemCount());
	}
}


BOOL CSelectDatabasesDlg::IncreaseThread(int nItem,CDatabase* pDatabase,BOOL bDecrease)
{
	if ((bDecrease && pDatabase->GetThreadId()<1) || 
		(!bDecrease && pDatabase->GetThreadId()>=m_nThreadsCurrently-1))
		return FALSE;

	pDatabase->SetThreadId(pDatabase->GetThreadId()+(bDecrease?-1:1));

	if (GetLocateApp()->m_wComCtrlVersion>=0x0600)
	{
		LVITEM li;
		li.mask=LVIF_GROUPID;
		li.iItem=nItem;
		li.iSubItem=0;
		li.iGroupId=pDatabase->GetThreadId();
		m_List.SetItem(&li);
	}
	else
		m_List.RedrawItems(nItem,nItem);
	
	m_List.EnsureVisible(nItem,FALSE);
	m_List.SetFocus();
	EnableButtons();
	return TRUE;
}

BOOL CSelectDatabasesDlg::SetThread(int nItem,CDatabase* pDatabase,WORD wThread)
{
	if (pDatabase->GetThreadId()>=m_nThreadsCurrently)
		ChangeNumberOfThreads(wThread+1);

	pDatabase->SetThreadId(wThread);

	if (GetLocateApp()->m_wComCtrlVersion>=0x0600)
	{
		LVITEM li;
		li.mask=LVIF_GROUPID;
		li.iItem=nItem;
		li.iSubItem=0;
		li.iGroupId=pDatabase->GetThreadId();
		m_List.SetItem(&li);
	}
	else
		m_List.RedrawItems(nItem,nItem);
	
	return TRUE;
}


void CSelectDatabasesDlg::EnableButtons()
{
	switch (m_PresetCombo.GetCurSel())
	{
	case SELECTED_PRESET:
		EnableDlgItem(IDC_DELETE,m_pSelectDatabases==NULL);
		break;
	case CUSTOM_PRESET:
	case GLOBAL_PRESET:
	case LATEST_PRESET:
		EnableDlgItem(IDC_DELETE,FALSE);
		break;
	default:
		EnableDlgItem(IDC_DELETE,TRUE);
		break;
	}
	
	int nSelectedItem=m_List.GetNextItem(-1,LVNI_SELECTED);
	
	if (nSelectedItem!=-1)
	{
		CDatabase* pDatabase=(CDatabase*)m_List.GetItemData(nSelectedItem);
		ASSERT(pDatabase!=NULL);

		// Checking item above
		int nAnother=m_List.GetNextItem(nSelectedItem,LVNI_ABOVE);
		if (nAnother==-1 || nAnother==nSelectedItem)
			EnableDlgItem(IDC_UP,pDatabase->GetThreadId()>0);
		else
			EnableDlgItem(IDC_UP,TRUE);
		
		// Checking item below
		nAnother=m_List.GetNextItem(nSelectedItem,LVNI_BELOW);
		if (nAnother==-1 || nAnother==nSelectedItem)
			EnableDlgItem(IDC_DOWN,pDatabase->GetThreadId()<m_nThreadsCurrently-1); 
		else
			EnableDlgItem(IDC_DOWN,TRUE);
	}
	else
	{
		EnableDlgItem(IDC_UP,FALSE);
		EnableDlgItem(IDC_DOWN,FALSE);
	}
}


void CSelectDatabasesDlg::LoadPresets()
{
	m_PresetCombo.AddString(ID2W(IDS_PRESETCUSTOM));
	m_PresetCombo.AddString(ID2W(IDS_PRESETGLOBAL));
	m_PresetCombo.AddString(ID2W(IDS_PRESETLATEST));
	
	if (m_pSelectDatabases!=NULL)
		m_PresetCombo.AddString(ID2W(IDS_PRESETSELECTED));
	
	if (m_bFlags&flagDisablePresets)
	{
		m_PresetCombo.SetCurSel(CUSTOM_PRESET);
		return;
	}

	
	CRegKey RegKey;
	if (RegKey.OpenKey(HKCU,m_pRegKey,CRegKey::openExist|CRegKey::samRead|CRegKey::samQueryValue)==ERROR_SUCCESS)
	{
		CStringW sName;
		for (int i=0;RegKey.EnumValue(i,sName)>0;i++)
		{
			if (wcsncmp(sName,L"Preset ",7)==0)
			{
				int nIndex=sName.FindFirst(':')+1;
				
				if (nIndex>0 &&  nIndex<sName.GetLength()) 
					m_PresetCombo.AddString(LPCWSTR(sName)+nIndex);
			}
		}
	}

	// Leading previous preset
	if (InsertSelected())
		m_PresetCombo.SetCurSel(SELECTED_PRESET);
	else if (m_bFlags&flagGlobalIsSelected)
	{
		InsertDatabases();
		m_PresetCombo.SetCurSel(GLOBAL_PRESET);
	}
	else if (LoadPreset(NULL))
		m_PresetCombo.SetCurSel(LATEST_PRESET);
	else
	{
		InsertDatabases();
		m_PresetCombo.SetCurSel(GLOBAL_PRESET);
	}
}

BOOL CSelectDatabasesDlg::SavePreset(LPCWSTR szName,BOOL bAskOverwrite)
{
	
	// First, check whether name already exists
	int iOverwriteItem=-1;

	if (szName!=NULL)
	{
		if (szName[0]==L'\0' ||  
			wcscmp(szName,L"Database List Widths")==0 || 
			wcscmp(szName,L"LastPreset")==0)
		{
			CString str;
			str.Format(IDS_PRESETNAMENOTVALID,W2A(szName));
			MessageBox(str,ID2A(IDS_ERROR),MB_OK|MB_ICONERROR);
			return FALSE;
		}

	
		// First item is global, second is last
		for (int i=m_PresetCombo.GetCount()-1;i>=2;i--)
		{
			int nItemTextLength=m_PresetCombo.GetLBTextLen(i);
			if (nItemTextLength==CB_ERR)
				continue;

            LPWSTR pItemText=new WCHAR[nItemTextLength+1];
			if (m_PresetCombo.GetLBText(i,pItemText)!=CB_ERR)
			{
				if (strcasecmp(pItemText,szName)==0)
				{
					if (bAskOverwrite)
					{
						CStringW str;
						str.Format(IDS_OVERWRITEPRESET,szName);
						if (MessageBox(str,ID2W(IDS_PRESETSAVETITLE),MB_YESNO)==IDNO)
							return FALSE;
					}

					iOverwriteItem=i-(m_pSelectDatabases!=NULL?4:3);
					break;
				}
			}
			delete[] pItemText;
		}
	}


	CRegKey RegKey;
	if(RegKey.OpenKey(HKCU,m_pRegKey,CRegKey::createNew|CRegKey::samAll)==ERROR_SUCCESS)
	{
		// Registry value constains
		// wNumberOfDatabases (WORD)
		// wNumberOfThreads (WORD)
		// wNumberOfSelectedDbs (WORD)
		// database ID (WORD)
		// ..
		// database ID (WORD)
		// thread ID (WORD)
		// ..
		// thread ID (WORD)
		// selected DB (WORD)
		// ...
		// selected DB (WORD)
		
		// Get enabled databases
		WORD* pThreads=new WORD[m_List.GetItemCount()];
		WORD* pIDs=new WORD[m_List.GetItemCount()];
        CWordArray aSelected;

		int nDatabases=0;
		
		if (m_List.GetItemCount()>0)
		{
			// Get the first item
			//int nNext;
			int nItem=m_List.GetNextItem(-1,LVNI_ALL);
			/*while ((nNext=m_List.GetNextItem(nItem,LVNI_ABOVE))!=-1)
			{
				if (nNext==nItem)
					break; // This should not be like that, why is it?
				nItem=nNext;
			}*/

			// Now we have top index
			while (nItem!=-1)
			{
				PDATABASE pDatabase=(PDATABASE)m_List.GetItemData(nItem);
				if (pDatabase!=NULL)
				{
					pIDs[nDatabases]=pDatabase->GetID();
					pThreads[nDatabases]=pDatabase->GetThreadId();
					nDatabases++;

					if (IsItemEnabled(pDatabase))
						aSelected.Add(pDatabase->GetID());
				}

				/*nNext=m_List.GetNextItem(nItem,LVNI_BELOW);
				if (nNext==nItem)
					break;
				nItem=nNext;*/

				nItem=m_List.GetNextItem(nItem,LVNI_ALL);
			}
		}

		DWORD dwLength=sizeof(WORD)*(3+2*nDatabases+aSelected.GetSize());
		char* pData=new char [dwLength];
		WORD* pPtr=(WORD*)pData;
		*(pPtr++)=nDatabases;
		*(pPtr++)=m_nThreadsCurrently;
		*(pPtr++)=aSelected.GetSize();

		CopyMemory(pPtr,pIDs,nDatabases*sizeof(WORD));
		pPtr+=nDatabases;
		CopyMemory(pPtr,pThreads,nDatabases*sizeof(WORD));
		pPtr+=nDatabases;
		CopyMemory(pPtr,aSelected.GetData(),aSelected.GetSize()*sizeof(WORD));
		
		if (szName!=NULL)
		{
			int nLen=15+istrlenw(szName);
			WCHAR* szText=new WCHAR[nLen];
			if (iOverwriteItem>=0)
				StringCbPrintfW(szText,nLen*sizeof(WCHAR),L"Preset %03d:%s",iOverwriteItem,szName);
			else
			{
				StringCbPrintfW(szText,nLen*sizeof(WCHAR),L"Preset %03d:%s",
					m_PresetCombo.GetCount()-(m_pSelectDatabases!=NULL?4:3),szName);
			}
			RegKey.SetValue(szText,pData,dwLength,REG_BINARY);
		}
		else
			RegKey.SetValue("LastPreset",pData,dwLength,REG_BINARY);
        delete[] pData;
		delete[] pIDs;
		delete[] pThreads;
	}

	if (szName!=NULL)
	{
        if (iOverwriteItem==-1)
			iOverwriteItem=m_PresetCombo.AddString(szName);
		else
		{
			iOverwriteItem+=m_pSelectDatabases!=NULL?4:3;
			m_PresetCombo.SetItemData(iOverwriteItem,DWORD(szName));

		}

		if (iOverwriteItem!=-1)
			m_PresetCombo.SetCurSel(iOverwriteItem);
	}

	EnableButtons();
	return TRUE;
}

BOOL CSelectDatabasesDlg::LoadPreset(LPCWSTR szName)
{
	WORD wDatabases,wThreads,wSelectedDatabases;
	WORD *pDatabaseIDs,*pThreadsIDs,*pSelectedIDs;

	if (!LoadPreset(m_pRegKey,szName,wDatabases,pDatabaseIDs,wThreads,pThreadsIDs,wSelectedDatabases,pSelectedIDs))
		return FALSE;

	BOOL bRet=InsertDatabases(wDatabases,wThreads,pDatabaseIDs,pThreadsIDs,wSelectedDatabases,pSelectedIDs);
	
	delete[] pDatabaseIDs;
	delete[] pThreadsIDs;
	delete[] pSelectedIDs;
	
	return bRet;
}

BOOL CSelectDatabasesDlg::LoadPreset(LPCSTR szRegKey,LPCWSTR szName,WORD& wDatabases,WORD*& pDatabaseIDs,WORD& wThreads,WORD*& pThreadsIDs,WORD& wSelectedDatabases,WORD*& pSelectedIDs)
{
	CRegKey RegKey;
	if (RegKey.OpenKey(HKCU,szRegKey,CRegKey::openExist|CRegKey::samRead|CRegKey::samQueryValue)!=ERROR_SUCCESS)
		return FALSE;

	CStringW sName;
	if (szName!=NULL)
	{
		BOOL bFound=FALSE;
		for (int i=0;RegKey.EnumValue(i,sName)>0;i++)
		{
			if (wcsncmp(sName,L"Preset ",7)==0)
			{
				int nIndex=sName.FindFirst(':')+1;
				
				if (nIndex>0 &&  nIndex<sName.GetLength()) 
				{
					if (strcasecmp(LPCWSTR(sName)+nIndex,szName)==0)
					{
						bFound=TRUE;
						break;
					}
				}
			}
		}
		if (!bFound)
			return FALSE;
	}
	else
		sName=L"LastPreset";

	DWORD dwLength=RegKey.QueryValueLength(sName);
    if (dwLength<2 || dwLength%2==1)
		return FALSE;

	WORD* pData=new WORD[dwLength/2];
	DWORD dwType;
	
	DWORD dwRet=RegKey.QueryValue(sName,(LPSTR)pData,dwLength,&dwType);

	if (dwType==REG_BINARY && dwRet>0)
	{	
		wDatabases=pData[0];
		wThreads=pData[1];
		wSelectedDatabases=pData[2];

		if (sizeof(WORD)*(3+2*wDatabases+wSelectedDatabases)!=dwLength)
		{
			delete[] pData;
			return FALSE;
		}

		pDatabaseIDs=new WORD[max(wDatabases,2)];
		CopyMemory(pDatabaseIDs,pData+3,wDatabases*sizeof(WORD));
		pThreadsIDs=new WORD[max(wDatabases,2)];
		CopyMemory(pThreadsIDs,pData+3+wDatabases,wDatabases*sizeof(WORD));
			
		pSelectedIDs=new WORD[max(wSelectedDatabases,2)];
		CopyMemory(pSelectedIDs,pData+3+2*wDatabases,wSelectedDatabases*sizeof(WORD));

		
	}

	delete[] pData;
	return dwType==REG_BINARY && dwRet>0;
}	


BOOL CSelectDatabasesDlg::GetLastSelectedDatabases(LPCSTR szRegKey,const CArray<PDATABASE>& rOrigDatabases,CArray<PDATABASE>& rSelectedDatabases)
{
	WORD wDatabases,wThreads,wSelectedDatabases;
	WORD *pDatabaseIDs,*pThreadsIDs,*pSelectedIDs;

	if (!LoadPreset(szRegKey,NULL,wDatabases,pDatabaseIDs,wThreads,pThreadsIDs,wSelectedDatabases,pSelectedIDs))
		return FALSE;

	

	for (int i=0;i<wDatabases;i++)
	{
		// Check first whether database is selected
		BOOL bSelected=FALSE;
		for (int j=0;j<wSelectedDatabases;j++)
		{
			if (pDatabaseIDs[i]==pSelectedIDs[j])
			{
				bSelected=TRUE;
				break;
			}
		}
		if (!bSelected)
			continue;



		
		// Searching database
		CDatabase* pDatabase=NULL;
		for (int j=0;j<rOrigDatabases.GetSize();j++)
		{
			if (rOrigDatabases[j]->GetID()==pDatabaseIDs[i])
			{
				CDatabase* pDatabase=new CDatabase(*rOrigDatabases[j]);
				pDatabase->Enable();
				rSelectedDatabases.Add(pDatabase);
				break;
			}
		}

	}

	delete[] pDatabaseIDs;
	delete[] pThreadsIDs;
	delete[] pSelectedIDs;

	return TRUE;
}
///////////////////////////////////////////////////////////
// CSelectDatabasesDlg::CPresetNameDlg

BOOL CSavePresetDlg::OnInitDialog(HWND hwndFocus)
{
	CDialog::OnInitDialog(hwndFocus);

	if (!LoadPosition(HKCU,CRegKey2::GetCommonKey()+"\\Dialogs","SavePresetWindowPos",fgOnlyNormalPosition|fgNoSize))
		CenterWindow();

	return FALSE;
}

void CSavePresetDlg::OnDestroy()
{
	CDialog::OnDestroy();
	SavePosition(HKCU,CRegKey2::GetCommonKey()+"\\Dialogs","SavePresetWindowPos");
}

BOOL CSavePresetDlg::OnCommand(WORD wID,WORD wNotifyCode,HWND hControl)
{
	CDialog::OnCommand(wID,wNotifyCode,hControl);
	switch (wID)
	{
	case IDC_OK:
		OnOK();
		break;
	case IDCANCEL:
	case IDC_CANCEL:
		OnCancel();
		break;
	case IDC_EDIT:
		if (wNotifyCode==EN_SETFOCUS)
			::SendMessage(hControl,EM_SETSEL,0,MAKELPARAM(0,-1));
		break;
	}
	return FALSE;
}

void CSavePresetDlg::OnOK()
{
    GetDlgItemText(IDC_EDIT,m_sReturnedPreset);

	if (m_sReturnedPreset.IsEmpty())
	{
		CStringW msg;
		msg.Format(IDS_PRESETNAMENOTVALID,LPCWSTR(m_sReturnedPreset));

		MessageBox(msg,ID2W(IDS_PRESETSAVETITLE),MB_OK|MB_ICONEXCLAMATION);
		SetFocus(IDC_EDIT);
	}
	else
		EndDialog(1);
}

void CSavePresetDlg::OnCancel()
{
	EndDialog(0);
}

///////////////////////////////////////////////////////////
// CSelectDatabasesDlg::CSavePresetDlg

void CSelectDatabasesDlg::CSavePresetDlg::OnOK()
{
	CStringW sName;
	
    GetDlgItemText(IDC_EDIT,sName);
	
	if (m_pParent->SavePreset(sName,SendDlgItemMessage(IDC_EDIT,CB_GETCURSEL)==CB_ERR))
		EndDialog(1);
}

BOOL CSelectDatabasesDlg::CSavePresetDlg::OnInitDialog(HWND hwndFocus)
{
	::CSavePresetDlg::OnInitDialog(hwndFocus);
	
	CRegKey RegKey;
	CComboBox PresetCombo(GetDlgItem(IDC_EDIT));

	if (RegKey.OpenKey(HKCU,m_pParent->m_pRegKey,CRegKey::openExist|CRegKey::samRead|CRegKey::samQueryValue)==ERROR_SUCCESS)
	{
		CStringW sName;
		for (int i=0;RegKey.EnumValue(i,sName)>0;i++)
		{
			if (wcsncmp(sName,L"Preset ",7)==0)
			{
				int nIndex=sName.FindFirst(':')+1;
				
				if (nIndex>0 &&  nIndex<sName.GetLength()) 
					PresetCombo.AddString(LPCWSTR(sName)+nIndex);
			}
		}
	}
	return FALSE;
}


///////////////////////////////////////////////////////////
// CChangeCaseDlg

BOOL CChangeCaseDlg::OnInitDialog(HWND hwndFocus)
{
	CDialog::OnInitDialog(hwndFocus);
	CenterWindow();
	switch (nSelectedCase)
	{
	case Sentence:
		CheckDlgButton(IDC_SENTENCECASE,TRUE);
		break;
	case Uppercase:
		CheckDlgButton(IDC_UPPERCASE,TRUE);
		break;
	case Lowercase:
		CheckDlgButton(IDC_LOWERCASE,TRUE);
		break;
	case Title:
		CheckDlgButton(IDC_TITLECASE,TRUE);
		break;
	case Toggle:
		CheckDlgButton(IDC_TOGGLECASE,TRUE);
		break;
	}
	CheckDlgButton(IDC_EXTENSIONS,bForExtension);

	if (!LoadPosition(HKCU,CRegKey2::GetCommonKey()+"\\Dialogs","ChangeCaseWindowPos",fgOnlyNormalPosition|fgNoSize))
		CenterWindow();

	return FALSE;
}

void CChangeCaseDlg::OnDestroy()
{
	CDialog::OnDestroy();
	SavePosition(HKCU,CRegKey2::GetCommonKey()+"\\Dialogs","ChangeCaseWindowPos");
}

BOOL CChangeCaseDlg::OnCommand(WORD wID,WORD wNotifyCode,HWND hControl)
{
	switch (wID)
	{
	case IDC_OK:
		if (IsDlgButtonChecked(IDC_SENTENCECASE))
			nSelectedCase=Sentence;
		else if (IsDlgButtonChecked(IDC_LOWERCASE))
			nSelectedCase=Lowercase;
		else if (IsDlgButtonChecked(IDC_UPPERCASE))
			nSelectedCase=Uppercase;
		else if (IsDlgButtonChecked(IDC_TITLECASE))
			nSelectedCase=Title;
		else if (IsDlgButtonChecked(IDC_TOGGLECASE))
			nSelectedCase=Toggle;
		
		bForExtension=IsDlgButtonChecked(IDC_EXTENSIONS);

		EndDialog(1);
		break;
	case IDCANCEL:
	case IDC_CANCEL:
		EndDialog(0);
		break;
	}
	return CDialog::OnCommand(wID,wNotifyCode,hControl);
}

BOOL CChangeCaseDlg::OnClose()
{
	CDialog::OnClose();
	EndDialog(0);
	return FALSE;
}



///////////////////////////////////////////////////////////
// CChangeFilenameDlg

BOOL CChangeFilenameDlg::OnInitDialog(HWND hwndFocus)
{
	CDialog::OnInitDialog(hwndFocus);
	
	if (m_dwFlags&fNoExtension)
		SetDlgItemText(IDC_EDIT,CStringW(m_sFileName,m_sFileName.FindLast('.')));
	else
		SetDlgItemText(IDC_EDIT,m_sFileName);


	if (!LoadPosition(HKCU,CRegKey2::GetCommonKey()+"\\Dialogs","ChangeFilenameWindowPos",fgOnlyNormalPosition|fgNoSize))
		CenterWindow();
	return FALSE;
}

void CChangeFilenameDlg::OnDestroy()
{
	CDialog::OnDestroy();
	SavePosition(HKCU,CRegKey2::GetCommonKey()+"\\Dialogs","ChangeFilenameWindowPos");
}

BOOL CChangeFilenameDlg::OnCommand(WORD wID,WORD wNotifyCode,HWND hControl)
{
	switch (wID)
	{
	case IDC_OK:
		{
			CStringW Name;
			if (GetDlgItemText(IDC_EDIT,Name)>0)
			{
				if (m_dwFlags&fNoExtension)
				{
					LONG_PTR nExtPos=m_sFileName.FindLast('.');
					if (nExtPos!=-1)
						Name << (LPCWSTR(m_sFileName)+nExtPos);
				}

				CStringW Path(m_sParent);
				Path << L'\\' << Name;
				
			
				if (m_sFileName.CompareNoCase(Name)!=0)
				{
					if (FileSystem::IsFile(Path))
					{
						CStringW msg;
						msg.Format(IDS_CHANGEFILENAMEALREADYEXISTS,(LPCWSTR)Name);
						MessageBox(msg,ID2W(IDS_ERROR),MB_OK|MB_ICONERROR);
						break;
					}
				}
				m_sFileName=Name;
				EndDialog(1);
			}
		}
		break;
	case IDCANCEL:
	case IDC_CANCEL:
		EndDialog(0);
		break;
	case IDC_EDIT:
		if (wNotifyCode==EN_SETFOCUS)
			::SendMessage(hControl,EM_SETSEL,0,-1);
		break;
	}
	return CDialog::OnCommand(wID,wNotifyCode,hControl);
}

BOOL CChangeFilenameDlg::OnClose()
{
	CDialog::OnClose();
	EndDialog(0);
	return FALSE;
}

///////////////////////////////////////////////////////////
// CLocateDlg::CRemovePresetDlg


BOOL CLocateDlg::CRemovePresetDlg::OnInitDialog(HWND hwndFocus)
{
	CDialog::OnInitDialog(hwndFocus);
	if (!LoadPosition(HKCU,CRegKey2::GetCommonKey()+"\\Dialogs","RemovePresetWindowPos",fgOnlyNormalPosition|fgNoSize))
		CenterWindow();



	// First, find indentifiers
	CRegKey2 RegKey;
	if (RegKey.OpenKey(HKCU,"\\Dialogs\\SearchPresets",CRegKey::openExist|CRegKey::samRead)!=ERROR_SUCCESS)
		return FALSE;
	
	
	CRegKey PresetKey;
	CComboBox Combo(GetDlgItem(IDC_PRESETS));
	char szBuffer[30];

	for (int nPreset=0;nPreset<1000;nPreset++)
	{
		StringCbPrintf(szBuffer,30,"Preset %03d",nPreset);

		if (PresetKey.OpenKey(RegKey,szBuffer,CRegKey::openExist|CRegKey::samRead)!=ERROR_SUCCESS)
			break;

		CStringW sCurrentName;
		PresetKey.QueryValue(L"",sCurrentName);

		Combo.AddString(sCurrentName);

		PresetKey.CloseKey();
	}		

	// Choosing first
	SendDlgItemMessage(IDC_PRESETS,CB_SETCURSEL,0,0);		
	return FALSE;
}

void CLocateDlg::CRemovePresetDlg::OnDestroy()
{
	CDialog::OnDestroy();
	SavePosition(HKCU,CRegKey2::GetCommonKey()+"\\Dialogs","RemovePresetWindowPos");
}

BOOL CLocateDlg::CRemovePresetDlg::OnCommand(WORD wID,WORD wNotifyCode,HWND hControl)
{
	CDialog::OnCommand(wID,wNotifyCode,hControl);
	switch (wID)
	{
	case IDC_OK:
		OnOK();
		break;
	case IDCANCEL:
	case IDC_CANCEL:
		EndDialog(0);
		break;
	case IDC_EDIT:
		if (wNotifyCode==EN_SETFOCUS)
			::SendMessage(hControl,EM_SETSEL,0,MAKELPARAM(0,-1));
		break;
	}
	return FALSE;
}


BOOL CLocateDlg::CRemovePresetDlg::OnClose()
{
	CDialog::OnClose();
	EndDialog(0);
	return FALSE;
}




void CLocateDlg::CRemovePresetDlg::OnOK()
{
	int nSelection=(int)SendDlgItemMessage(IDC_PRESETS,CB_GETCURSEL);
	if (nSelection==CB_ERR)
		return;

	CRegKey2 RegKey;

	LONG lErr=RegKey.OpenKey(HKCU,"\\Dialogs\\SearchPresets",CRegKey::openExist|CRegKey::samAll);
	if (lErr!=ERROR_SUCCESS)
	{
		ReportSystemError(NULL,lErr,0);
		return;
	}
	
	char szKeyName[30];
	StringCbPrintf(szKeyName,30,"Preset %03d",nSelection);

	if ((lErr=RegKey.DeleteKey(szKeyName))!=ERROR_SUCCESS)
	{
		ReportSystemError(NULL,lErr,0);
		return;
	}

    for (nSelection++;;nSelection++)
	{
		char szRenameKey[30];
		StringCbPrintf(szRenameKey,30,"Preset %03d",nSelection);
		
		if (RegKey.RenameSubKey(szRenameKey,szKeyName)!=ERROR_SUCCESS)
			break;
	
		StringCbCopy(szKeyName,30,szRenameKey);
	}

	EndDialog(1);
}


///////////////////////////////////////////////////////////
// CLocateDlg::CDeletePrivateData


BOOL CLocateDlg::CDeletePrivateData::OnInitDialog(HWND hwndFocus)
{
	CDialog::OnInitDialog(hwndFocus);
	if (!LoadPosition(HKCU,CRegKey2::GetCommonKey()+"\\Dialogs","DeletePrivateData",fgOnlyNormalPosition|fgNoSize))
		CenterWindow();

	CheckDlgButton(IDC_CLEARNAMED,TRUE);
	CheckDlgButton(IDC_CLEAREXTENSTIONS,TRUE);
	CheckDlgButton(IDC_CLEARLOOKIN,TRUE);
	
	return FALSE;
}

void CLocateDlg::CDeletePrivateData::OnDestroy()
{
	CDialog::OnDestroy();
	SavePosition(HKCU,CRegKey2::GetCommonKey()+"\\Dialogs","DeletePrivateData");
}


BOOL CLocateDlg::CDeletePrivateData::OnClose()
{
	CDialog::OnClose();
	EndDialog(0);
	return FALSE;
}

BOOL CLocateDlg::CDeletePrivateData::OnCommand(WORD wID,WORD wNotifyCode,HWND hControl)
{
	CDialog::OnCommand(wID,wNotifyCode,hControl);
	switch (wID)
	{
	case IDC_OK:
		OnOK();
		break;
	case IDCANCEL:
	case IDC_CANCEL:
		EndDialog(0);
		break;
	}
	return FALSE;
}



void CLocateDlg::CDeletePrivateData::OnOK()
{
	m_dwFlags=0;
	if (IsDlgButtonChecked(IDC_CLEARNAMED))
		m_dwFlags|=clearNamed;
	if (IsDlgButtonChecked(IDC_CLEAREXTENSTIONS))
		m_dwFlags|=clearExtensions;
	if (IsDlgButtonChecked(IDC_CLEARLOOKIN))
		m_dwFlags|=clearLookIn;


	EndDialog(1);
}


///////////////////////////////////////////////////////////
// CPropertiesSheet

CPropertiesSheet::CPropertiesSheet(int nItems,CLocatedItem** pItems,BOOL bForParents)
:	m_hThread(NULL)
{
	CStringW Title;
	if (pItems[0]->ShouldUpdateFileTitle())
		pItems[0]->UpdateFileTitle();

	Title.Format(IDS_FILEPROPERTIESSHEETCAPTION,pItems[0]->GetFileTitle());
	
	Construct(Title,NULL,0);

	m_pPropertiesPage=new CPropertiesPage(nItems,pItems,bForParents);
	AddPage((CPropertyPage*)m_pPropertiesPage);	
}

CPropertiesSheet::~CPropertiesSheet()
{
	CloseHandle(m_hThread);
	DebugCloseThread(m_hThread);
	delete m_pPropertiesPage;
	
}

DWORD WINAPI CPropertiesSheet::StartThreadProc(LPVOID lpParameter)
{
	(DWORD)((CPropertiesSheet*)lpParameter)->DoModal();
	delete ((CPropertiesSheet*)lpParameter);
	return 0;
}

BOOL CPropertiesSheet::Open()
{
	DWORD dwThreadID;
	m_hThread=CreateThread(NULL,0,StartThreadProc,this,0,&dwThreadID);
	DebugOpenThread(m_hThread);

	DebugFormatMessage("PROPERTIES: thread started ID=%X",dwThreadID);
	return TRUE;
}
	
CPropertiesSheet::CPropertiesPage::CPropertiesPage(int nItems,CLocatedItem** pItems,BOOL bForParents)
:	CPropertyPage(IDD_FILEPROPERTIES,IDS_FILEPROPERTIESPAGETITLE),
	m_nItems(nItems),m_nFiles(0),m_nDirectories(0),m_nSize(0),m_nSizeOnDisk(0),
	m_bIsSameType(TRUE),m_bReadOnly(0),m_bHidden(0),m_bArchive(0),
	m_pGetVolumePathName(NULL)
{
	m_ppFiles=new LPWSTR[nItems];
	m_pbIsDirectory=new BOOL[nItems];
	
	if (bForParents)
	{
		for (int i=0;i<nItems;i++)
		{
			m_ppFiles[i]=alloccopy(pItems[i]->GetParent());
			m_pbIsDirectory[i]=TRUE;
		}
	}
	else
	{
		for (int i=0;i<nItems;i++)
		{
			m_ppFiles[i]=alloccopy(pItems[i]->GetPath());
			m_pbIsDirectory[i]=pItems[i]->IsFolder();
		}
	}


	DWORD dwThread;
	m_hThread=CreateThread(NULL,0,CountingThreadProc,this,0,&dwThread);
	DebugOpenThread(m_hThread);

	DebugFormatMessage("COUNTING: thread started ID=%X",dwThread);



	DWORD dwError=GetLastError();

	m_pGetVolumePathName=(BOOL (WINAPI*)(LPCWSTR,LPWSTR,DWORD))GetProcAddress(GetModuleHandle("kernel32.dll"),"GetVolumePathNameW");
	
	if (m_pGetVolumePathName==NULL)
		m_pGetVolumePathName=GetVolumePathNameAlt;
}


CPropertiesSheet::CPropertiesPage::~CPropertiesPage()
{
	if (m_ppFiles!=NULL)
	{
		for (int i=0;i<m_nItems;i++)
			delete[] m_ppFiles[i];
		delete[] m_ppFiles;
		delete[] m_pbIsDirectory;
	}

}

BOOL CPropertiesSheet::CPropertiesPage::OnInitDialog(HWND hwndFocus)
{
	CPropertyPage::OnInitDialog(hwndFocus);

	SendDlgItemMessage(IDC_FILEICON,STM_SETIMAGE,IMAGE_ICON,
		(LPARAM)LoadImage(IDI_FILES,IMAGE_ICON,0,0,LR_SHARED|LR_DEFAULTSIZE));

	UpdateFields();
	SetTimer(0,250);

	return FALSE;
}

BOOL CPropertiesSheet::CPropertiesPage::OnCommand(WORD wID,WORD wNotifyCode,HWND hControl)
{
	CPropertyPage::OnCommand(wID,wNotifyCode,hControl);
	
	return FALSE;
}

void CPropertiesSheet::CPropertiesPage::OnDestroy()
{
	CPropertyPage::OnDestroy();

	if (m_hThread!=NULL)
	{
		DWORD status;
		BOOL bRet=::GetExitCodeThread(m_hThread,&status);
		if (bRet && status==STILL_ACTIVE)
		{
			TerminateThread(m_hThread,1,TRUE);
			KillTimer(0);
		}

		CloseHandle(m_hThread);
		DebugCloseThread(m_hThread);
		m_hThread=NULL;
	}
}


BOOL CPropertiesSheet::CPropertiesPage::OnApply()
{
	CPropertyPage::OnApply();

	enum { DisableArchive=0x1,EnableArchive=0x2,
		DisableHidden=0x4,EnableHidden=0x8,
		DisableReadOnly=0x10,EnableReadOnly=0x20};
	DWORD dwDo=0;

	switch (IsDlgButtonChecked(IDC_ARCHIVE))
	{
	case 0:
		if (m_bArchive!=0)
			dwDo|=DisableArchive;
		break;
	case 1:
		if (m_bArchive!=1)
			dwDo|=EnableArchive;
		break;
	}

	switch (IsDlgButtonChecked(IDC_HIDDEN))
	{
	case 0:
		if (m_bHidden!=0)
			dwDo|=DisableHidden;
		break;
	case 1:
		if (m_bHidden!=1)
			dwDo|=EnableHidden;
		break;
	}

	switch (IsDlgButtonChecked(IDC_READONLY))
	{
	case 0:
		if (m_bReadOnly!=0)
			dwDo|=DisableReadOnly;
		break;
	case 1:
		if (m_bReadOnly!=1)
			dwDo|=EnableReadOnly;
		break;
	}

	if (dwDo)
	{
		for (int i=0;i<m_nItems;i++)
		{
			DWORD dwAttributesOld=FileSystem::GetFileAttributes(m_ppFiles[i]);
			DWORD dwAttributes=dwAttributesOld;
			
			if (dwDo&EnableArchive)
				dwAttributes|=FILE_ATTRIBUTE_ARCHIVE;
			else if (dwDo&DisableArchive)
				dwAttributes&=~FILE_ATTRIBUTE_ARCHIVE;

			if (dwDo&EnableHidden)
				dwAttributes|=FILE_ATTRIBUTE_HIDDEN;
			else if (dwDo&DisableHidden)
				dwAttributes&=~FILE_ATTRIBUTE_HIDDEN;

			if (dwDo&EnableReadOnly)
				dwAttributes|=FILE_ATTRIBUTE_READONLY;
			else if (dwDo&DisableReadOnly)
				dwAttributes&=~FILE_ATTRIBUTE_READONLY;

			if (dwAttributes!=dwAttributesOld)
				FileSystem::SetFileAttributes(m_ppFiles[i],dwAttributes);
		}
	}

	return TRUE;
}

void CPropertiesSheet::CPropertiesPage::OnCancel()
{
	CPropertyPage::OnCancel();
}

void CPropertiesSheet::CPropertiesPage::OnTimer(DWORD wTimerID)
{
	if (m_hThread!=NULL)
	{
		DWORD status;
		BOOL bRet=::GetExitCodeThread(m_hThread,&status);
		if (bRet && status!=STILL_ACTIVE)
		{
			KillTimer(0);	
			CloseHandle(m_hThread);
			DebugCloseThread(m_hThread);
			m_hThread=NULL;
		}
	}

	UpdateFields();
}
					
void CPropertiesSheet::CPropertiesPage::UpdateFields()
{
	CStringW Text;
	Text.Format(IDS_FILEPROPERTIESPAGEFILESFMT,m_nFiles,m_nDirectories);
	SetDlgItemText(IDC_FILESANDDIRECTORIES,Text);

	if (m_bIsSameType)
	{
		SHFILEINFOW fi;
		if (ShellFunctions::GetFileInfo(m_ppFiles[0],0,&fi,SHGFI_TYPENAME))
			Text.Format(IDS_FILEPROPERTIESALLOFTYPE,fi.szTypeName);
		else
			Text.Format(IDS_FILEPROPERTIESALLOFTYPE,(LPCWSTR)ID2W(IDS_UNKNOWN));
	}
	else 
		Text.LoadString(IDS_FILEPROPERTIESPAGEMULTIPLETYPES);
	SetDlgItemText(IDC_TYPES,Text);

	WCHAR szBestSize[100],szBytes[100];
	
	FormatSizes(m_nSize,szBestSize,szBytes);
	Text.Format(IDS_FILEPROPERTIESPAGESIZEFMT,szBestSize,szBytes);
	SetDlgItemText(IDC_SIZEOFILES,Text);

	FormatSizes(m_nSizeOnDisk,szBestSize,szBytes);
	Text.Format(IDS_FILEPROPERTIESPAGESIZEFMT,szBestSize,szBytes);
	SetDlgItemText(IDC_SIZEONDISK,Text);

	if (m_bReadOnly==2)
	{
		SetDlgItemStyle(IDC_READONLY,
			(GetDlgItemStyle(IDC_READONLY)&~BS_AUTOCHECKBOX)|BS_AUTO3STATE);
	}
	if (m_bHidden==2)
	{
		SetDlgItemStyle(IDC_HIDDEN,
			(GetDlgItemStyle(IDC_HIDDEN)&~BS_AUTOCHECKBOX)|BS_AUTO3STATE);
	}
	if (m_bArchive==2)
	{
		SetDlgItemStyle(IDC_ARCHIVE,
			(GetDlgItemStyle(IDC_ARCHIVE)&~BS_AUTOCHECKBOX)|BS_AUTO3STATE);
	}
	
	CheckDlgButton(IDC_READONLY,m_bReadOnly);
	CheckDlgButton(IDC_HIDDEN,m_bHidden);
	CheckDlgButton(IDC_ARCHIVE,m_bArchive);
}


BOOL WINAPI CPropertiesSheet::CPropertiesPage::GetVolumePathNameAlt(LPCWSTR lpszFileName,LPWSTR lpszVolumePathName,DWORD cchBufferLength)
{
	if (cchBufferLength<4)
		return FALSE;
	if (lpszFileName[0]=='\\' && lpszFileName[1])
	{
		// Network share
		int nIndex=FirstCharIndex(lpszFileName+2,L'\\');
		if (nIndex==-1)
			return FALSE;
		nIndex+=2;

		while ((DWORD)nIndex<cchBufferLength)
		{
			MemCopyW(lpszVolumePathName,lpszFileName,nIndex);
			lpszVolumePathName[nIndex]='\0';

			if (FileSystem::IsDirectory(lpszVolumePathName))
				return TRUE;

			int nIndex2=FirstCharIndex(lpszFileName+nIndex+1,L'\\');
			if (nIndex2==-1)
			{
				if (FileSystem::IsDirectory(lpszFileName))
				{
					wcscpy_s(lpszVolumePathName,cchBufferLength,lpszFileName);
					wcscat_s(lpszVolumePathName,cchBufferLength,L"\\");
					return TRUE;
				}
				return FALSE;
			}
			nIndex+=nIndex2+1;
		}
	}
	if (lpszFileName[1]==':')
	{
		// Drive
		lpszVolumePathName[0]=lpszFileName[0];
		lpszVolumePathName[1]=L':';
		lpszVolumePathName[2]=L'\\';
		lpszVolumePathName[3]=L'\0';
		return TRUE;
	}
	return FALSE;
}



void CPropertiesSheet::CPropertiesPage::CheckFiles()
{
	m_nExPos0=LastCharIndex(m_ppFiles[0],L'.')+1;

	

	for (int i=0;i<m_nItems;i++)
	{
		if (m_pbIsDirectory[i])
		{
			m_nDirectories++;
			if (i>0 && m_bIsSameType && !m_pbIsDirectory[0])
				m_bIsSameType=FALSE;
			
			CheckDirectory(m_ppFiles[i],i==0);
		}
		else
		{
			m_nFiles++;
			
			CheckFile(m_ppFiles[i],i==0);			
		}
	}
}

void CPropertiesSheet::CPropertiesPage::CheckFile(LPCWSTR szFile,BOOL bIsFirst)
{
	union {
		WIN32_FIND_DATA fd;
		WIN32_FIND_DATAW fdw; // The beginning of the structures are equal
	};

	HANDLE hFind;
	if (IsUnicodeSystem())	
		hFind=FindFirstFileW(szFile,&fdw);
	else
		hFind=FindFirstFileA(W2A(szFile),&fd);
	DebugOpenHandle(dhtFileFind,hFind,szFile);
			
	if (hFind!=INVALID_HANDLE_VALUE)
	{
		FindClose(hFind);
		DebugCloseHandle(dhtFileFind,hFind,szFile);
			

		if (bIsFirst)
		{
			m_bArchive=fd.dwFileAttributes&FILE_ATTRIBUTE_ARCHIVE?1:0;
			m_bHidden=fd.dwFileAttributes&FILE_ATTRIBUTE_HIDDEN?1:0;
			m_bReadOnly=fd.dwFileAttributes&FILE_ATTRIBUTE_READONLY?1:0;
		}
		else
		{
			if ((m_bArchive==1 && !(fd.dwFileAttributes&FILE_ATTRIBUTE_ARCHIVE)) ||
				(m_bArchive==0 && fd.dwFileAttributes&FILE_ATTRIBUTE_ARCHIVE))
				m_bArchive=2;
			if ((m_bHidden==1 && !(fd.dwFileAttributes&FILE_ATTRIBUTE_HIDDEN)) ||
				(m_bHidden==0 && fd.dwFileAttributes&FILE_ATTRIBUTE_HIDDEN))
				m_bHidden=2;
			if ((m_bReadOnly==1 && !(fd.dwFileAttributes&FILE_ATTRIBUTE_READONLY)) ||
				(m_bReadOnly==0 && fd.dwFileAttributes&FILE_ATTRIBUTE_READONLY))
				m_bReadOnly=2;
		}

		ULONGLONG ulFileSize=ULONGLONG(fd.nFileSizeHigh)<<32|ULONGLONG(fd.nFileSizeLow);
		m_nSize+=ulFileSize;
		
		WCHAR szVolume[MAX_PATH];
			
		if (m_pGetVolumePathName(szFile,szVolume,MAX_PATH))
		{
			DWORD dwSectorsPerCluster,dwBytesPerSector,
				dwNumberOfFreeClusters,dwTotalNumberOfCluster;
			BOOL bRet;
			if (IsUnicodeSystem())
			{
				bRet=GetDiskFreeSpaceW(szVolume,&dwSectorsPerCluster,&dwBytesPerSector,
					&dwNumberOfFreeClusters,&dwTotalNumberOfCluster);
			}
			else
			{
				bRet=GetDiskFreeSpaceA(W2A(szVolume),&dwSectorsPerCluster,&dwBytesPerSector,
					&dwNumberOfFreeClusters,&dwTotalNumberOfCluster);
			}
			if (bRet)
			{
				DWORD dwClusterSize=dwBytesPerSector*dwSectorsPerCluster;
				if (ulFileSize%dwClusterSize==0)
					m_nSizeOnDisk+=ulFileSize;
				else
					m_nSizeOnDisk+=(ulFileSize/dwClusterSize+1)*dwClusterSize;
			}
			else
				m_nSizeOnDisk+=ulFileSize;
		}
		else
			m_nSizeOnDisk+=ulFileSize;
	}

	// Check type
	if (!bIsFirst && m_bIsSameType)
	{
		if (m_pbIsDirectory[0])
			m_bIsSameType=FALSE;
		else
		{
			int nExPos=LastCharIndex(szFile,L'.')+1;
			if (nExPos!=0)
			{
				if (m_nExPos0==0)
					m_bIsSameType=FALSE;
				else if (_wcsicmp(szFile+nExPos,m_ppFiles[0]+m_nExPos0)!=0)
					m_bIsSameType=FALSE;
			}
			else if (m_nExPos0!=-1)
				m_bIsSameType=FALSE;
		}
	}	
}


void CPropertiesSheet::CPropertiesPage::CheckDirectory(LPCWSTR szDirectory,BOOL bIsFirst)
{
	WCHAR szMask[MAX_PATH];
	DWORD nLength=istrlenw(szDirectory);
	MemCopyW(szMask,szDirectory,nLength);
	MemCopyW(szMask+nLength,L"\\*.*",5);


	CFileFind ff;
	
	BOOL bRet=ff.FindFile(szMask);
	while (bRet)
	{
		if (ff.IsParentDirectory())
		{
			bRet=ff.FindNextFile();
			continue;
		}

		if (ff.IsRootDirectory())
		{
			if (bIsFirst)
			{
				m_bArchive=ff.IsArchived();
				m_bHidden=ff.IsHidden();
				m_bReadOnly=ff.IsReadOnly();
			}
			else
			{
				if ((m_bArchive==1 && !ff.IsArchived()) ||
					(m_bArchive==0 && ff.IsArchived()))
					m_bArchive=2;
				if ((m_bHidden==1 && !ff.IsHidden()) ||
					(m_bHidden==0 && ff.IsHidden()))
					m_bHidden=2;
				if ((m_bReadOnly==1 && !ff.IsReadOnly()) ||
					(m_bReadOnly==0 && ff.IsReadOnly()))
					m_bReadOnly=2;
			}
		}
		else 
		{
			WCHAR szPath[MAX_PATH];
			ff.GetFilePath(szPath,MAX_PATH);
			
			int i;
			for (i=0;i<m_nItems;i++)
			{
				if (_wcsicmp(szPath,m_ppFiles[i])==0)
					break;
			}

			if (i==m_nItems)
			{
				if (ff.IsDirectory())
				{
					m_nDirectories++;
					CheckDirectory(szPath,FALSE);
				}
				else
				{
					m_nFiles++;
					CheckFile(szPath,FALSE);
				}
			}
		}		
		bRet=ff.FindNextFile();

	}

	// Check type
	if (!bIsFirst && m_bIsSameType)
	{
		if (!m_pbIsDirectory[0])
			m_bIsSameType=FALSE;
	}
}


DWORD WINAPI CPropertiesSheet::CPropertiesPage::CountingThreadProc(LPVOID lpParameter)
{
	((CPropertiesPage*)lpParameter)->CheckFiles();
	return 0;
}


void CPropertiesSheet::CPropertiesPage::FormatSizes(ULONGLONG ulSize,WCHAR* szBestFit,WCHAR* szBytes)
{
	WCHAR unit[10],unit2[10],szBestFitPre[100],szBytesPre[100];
	BOOL bDigits=0;

	if (ulSize>1024*1024*1024)
	{
		DWORD num=DWORD(ulSize/(1024*1024));
		if (num>=100*1024)
			_ultow_s(num/1024,szBestFitPre,40,10);
		else
		{
			StringCbPrintfW(szBestFitPre,100*sizeof(WCHAR),L"%1.1f",double(num)/1024);
			bDigits=1;
		}
		
		LoadString(IDS_GB,unit,10);
	}
	else if (ulSize>1048576) // As MB
	{
		DWORD num=DWORD(ulSize/1024);
		
		if (num>=100*1024)
			_ultow_s(num/1024,szBestFitPre,100,10);
		else
		{
			StringCbPrintfW(szBestFitPre,100*sizeof(WCHAR),L"%1.1f",double(num)/1024);
			bDigits=1;
		}
		LoadString(IDS_MB,unit,10);
	}
	else if (ulSize>1024) // As KB
	{
		if (ulSize>=100*1024)
			_ultow_s(DWORD(ulSize/1024),szBestFitPre,100,10);
		else
		{
			StringCbPrintfW(szBestFitPre,100*sizeof(WCHAR),L"%1.1f",double(ulSize)/1024);
			bDigits=1;
		}
		
		LoadString(IDS_KB,unit,10);
	}
	else // As B
	{
		_ultow_s(DWORD(ulSize),szBestFitPre,100,10);
	
		LoadString(IDS_BYTES,unit,10);
	}

	LoadString(IDS_BYTES,unit2,10);
	_ui64tow_s(ulSize,szBytesPre,100,10);
	
	CRegKey RegKey;
	if (RegKey.OpenKey(HKCU,"Control Panel\\International",CRegKey::defRead)==ERROR_SUCCESS)
	{
		
		if (IsUnicodeSystem())
		{
			WCHAR szTemp[10]=L".",szTemp2[10]=L" ";

			NUMBERFMTW fmt;
		
			// Defaults;
			fmt.NumDigits=bDigits; 
			fmt.LeadingZero=1;
			fmt.Grouping=3; 
			fmt.lpDecimalSep=szTemp; 
			fmt.lpThousandSep=szTemp2; 
			fmt.NegativeOrder=1; 
			
			if (RegKey.QueryValue(L"iLZero",szTemp,10)>1)
				fmt.LeadingZero=_wtoi(szTemp);
			if (RegKey.QueryValue(L"sGrouping",szTemp,10)>1)
				fmt.Grouping=_wtoi(szTemp);
			RegKey.QueryValue(L"sDecimal",szTemp,10);
			RegKey.QueryValue(L"sThousand",szTemp2,10);

			
			if (GetNumberFormatW(LOCALE_USER_DEFAULT,0,szBestFitPre,&fmt,szBestFit,100)==0)
				wcscpy_s(szBestFit,100,szBestFitPre);

			fmt.NumDigits=0; 
			
			if (GetNumberFormatW(LOCALE_USER_DEFAULT,0,szBytesPre,&fmt,szBytes,100)==0)
				wcscpy_s(szBytes,100,szBestFitPre);
		}
		else
		{
			char szFormattedStr[100];
			char szTemp[10]=".",szTemp2[10]=" ";

			NUMBERFMT fmt;
		
			// Defaults;
			fmt.NumDigits=bDigits; 
			fmt.LeadingZero=1;
			fmt.Grouping=3; 
			fmt.lpDecimalSep=szTemp; 
			fmt.lpThousandSep=szTemp2; 
			fmt.NegativeOrder=1; 
			
			if (RegKey.QueryValue("iLZero",szTemp,10)>1)
				fmt.LeadingZero=atoi(szTemp);
			if (RegKey.QueryValue("sGrouping",szTemp,10)>1)
				fmt.Grouping=atoi(szTemp);
			RegKey.QueryValue("sDecimal",szTemp,10);
			RegKey.QueryValue("sThousand",szTemp2,10);

			
			if (GetNumberFormat(LOCALE_USER_DEFAULT,0,W2A(szBestFitPre),&fmt,szFormattedStr,100)>0)
				MultiByteToWideChar(CP_ACP,0,szFormattedStr,-1,szBestFit,100);
			else
				wcscpy_s(szBestFit,100,szBestFitPre);

			fmt.NumDigits=0; 
			
			if (GetNumberFormat(LOCALE_USER_DEFAULT,0,W2A(szBytesPre),&fmt,szFormattedStr,100)>0)
				MultiByteToWideChar(CP_ACP,0,szFormattedStr,-1,szBytes,100);
			else
				wcscpy_s(szBytes,100,szBytesPre);
		}
	}
	else
	{
		wcscpy_s(szBestFit,100,szBestFitPre);
		wcscpy_s(szBytes,100,szBytesPre);
	}

	wcscat_s(szBestFit,100,unit);
	wcscat_s(szBytes,100,unit2);
	
}
