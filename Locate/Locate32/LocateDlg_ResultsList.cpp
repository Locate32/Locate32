/* Locate32 - Copyright (c) 1997-2010 Janne Huttunen */

#include <HFCLib.h>
#include "Locate32.h"

#include <commoncontrols.h>
#include <shlwapi.h>
#include <olectl.h>


#include "3rdparty/strnatcmp.h"

#pragma comment(lib,"shlwapi.lib")

////////////////////////////////////////////////////////////
// CLocateDlg - Results list - List style and outlook
////////////////////////////////////////////////////////////

void CLocateDlg::SetResultListFont()
{
	if (m_hDialogFont==NULL)
		m_hDialogFont=m_pListCtrl->GetFont();

	
	HFONT hOldFont=m_pListCtrl->GetFont();
	HFONT hNewFont=m_hDialogFont;

	CRegKey2 RegKey;
	if (RegKey.OpenKey(HKCU,"\\General",CRegKey::defRead)==ERROR_SUCCESS)
	{
		LOGFONT lFont;
		if (RegKey.QueryValue("ResultListFont",(LPSTR)&lFont,sizeof(LOGFONT))==sizeof(LOGFONT))
		{
			hNewFont=CreateFontIndirect(&lFont);
			DebugOpenGdiObject(hNewFont);
		}
		
	}
	
	m_pListCtrl->SetFont(hNewFont);

	if (m_pListTooltips!=NULL)
	{
		if (m_pListTooltips->GetFont()==hOldFont)
			m_pListTooltips->SetFont(hNewFont);
	}

	if (hOldFont!=m_hDialogFont)
	{
		DebugCloseGdiObject(hOldFont);
		DeleteObject(hOldFont);
	}
}

BOOL CLocateDlg::ResolveSystemLVStatus()
{
	m_dwFlags=(m_dwFlags&~fgLVStyleFlag)|fgLVStyleSystemDefine;

	SHELLFLAGSTATE sfs;
	SHGetSettings(&sfs,SSF_DOUBLECLICKINWEBVIEW);

	if (sfs.fDoubleClickInWebView==0)
	{
		CRegKey RegKey;
		if (RegKey.OpenKey(HKCU,"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
		{
			char temp[4];
			m_dwFlags|=fgLVStylePointToSelect;
			RegKey.QueryValue("IconUnderline",(LPTSTR)temp,4);
			if (temp[0]==2)
				m_dwFlags|=fgLVStyleUnderLine;
			else if (temp[0]==3)
			{
				CRegKey Key;
				if (Key.OpenKey(HKCU,"Software\\Microsoft\\Internet Explorer\\Main",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
				{
					CString str;
					Key.QueryValue("Anchor Underline",str);
					if (str.CompareNoCase("yes")==0)
						m_dwFlags|=fgLVStyleAlwaysUnderline;
					else if (str.CompareNoCase("hover")==0)
						m_dwFlags|=fgLVStyleUnderLine;
				}
			}
		}
	}
	return TRUE;
}


BOOL CLocateDlg::SetListSelStyle()
{
	if (m_pListCtrl==NULL)
		return FALSE;
	if (m_dwFlags&fgLVStyleSystemDefine)
		ResolveSystemLVStatus();
	if (m_dwFlags&fgLVStylePointToSelect)
	{
		if ((m_dwFlags&fgLVStyleAlwaysUnderline)==fgLVStyleAlwaysUnderline)
			m_pListCtrl->SetExtendedListViewStyle(
				LVS_EX_UNDERLINECOLD|LVS_EX_UNDERLINEHOT|LVS_EX_TWOCLICKACTIVATE|LVS_EX_ONECLICKACTIVATE|LVS_EX_TRACKSELECT,
				LVS_EX_UNDERLINECOLD|LVS_EX_UNDERLINEHOT|LVS_EX_ONECLICKACTIVATE|LVS_EX_TRACKSELECT);
		else if ((m_dwFlags&fgLVStyleAlwaysUnderline)==fgLVStyleUnderLine)
			m_pListCtrl->SetExtendedListViewStyle(
				LVS_EX_UNDERLINECOLD|LVS_EX_UNDERLINEHOT|LVS_EX_TWOCLICKACTIVATE|LVS_EX_ONECLICKACTIVATE|LVS_EX_TRACKSELECT,
				LVS_EX_UNDERLINEHOT|LVS_EX_ONECLICKACTIVATE|LVS_EX_TRACKSELECT);
		else
			m_pListCtrl->SetExtendedListViewStyle(
				LVS_EX_UNDERLINECOLD|LVS_EX_UNDERLINEHOT|LVS_EX_TWOCLICKACTIVATE|LVS_EX_ONECLICKACTIVATE|LVS_EX_TRACKSELECT,
				LVS_EX_ONECLICKACTIVATE|LVS_EX_TRACKSELECT);
	}		
	else
		m_pListCtrl->SetExtendedListViewStyle(
			LVS_EX_UNDERLINECOLD|LVS_EX_UNDERLINEHOT|LVS_EX_TWOCLICKACTIVATE|LVS_EX_ONECLICKACTIVATE|LVS_EX_TRACKSELECT,0);
	
	m_pListCtrl->SetExtendedListViewStyle(LVS_EX_FULLROWSELECT,
		m_dwFlags&fgLVSelectFullRow?LVS_EX_FULLROWSELECT:0);
	m_pListCtrl->SetExtendedListViewStyle(0x02000000 /* LVS_EX_HEADERINALLVIEWS */,
		m_dwExtraFlags&efLVHeaderInAllViews?0x02000000:0);
	return TRUE;
}





////////////////////////////////////////////////////////////
// CLocateDlg - Results list - Viewing items
////////////////////////////////////////////////////////////

BOOL CLocateDlg::ListNotifyHandler(NMLISTVIEW *pNm)
{
	switch(pNm->hdr.code)
	{
	case NMX_CLICK:
		DebugMessage("NMX_CLICK");
		if (!IsExtraFlagSet(efLVIgnoreListClicks))
		{
			DetailType nDetail=DetailType(m_pListCtrl->GetColumnIDFromSubItem(((NMHDR_MOUSE*)pNm)->iSubItem));
			if (nDetail>LastType || nDetail<0)
				break;

			if (m_aResultListActions[nDetail][LeftMouseButtonClick]!=NULL)
			{
				CWaitCursor wait;
				OnExecuteResultAction(m_aResultListActions[nDetail][LeftMouseButtonClick]->m_nResultList,
					m_aResultListActions[nDetail][LeftMouseButtonClick]->m_pExtraInfo,((NMHDR_MOUSE*)pNm)->iItem,nDetail);
			}
		}
		break;
	case NMX_RCLICK:
		DebugMessage("NMX_RCLICK");
		if (!IsExtraFlagSet(efLVIgnoreListClicks))		
		{
			DetailType nDetail=DetailType(m_pListCtrl->GetColumnIDFromSubItem(((NMHDR_MOUSE*)pNm)->iSubItem));
			if (nDetail>LastType || nDetail<0)
				break;

			if (m_aResultListActions[nDetail][RightMouseButtonClick]!=NULL)
			{
				CWaitCursor wait;
				OnExecuteResultAction(m_aResultListActions[nDetail][RightMouseButtonClick]->m_nResultList,
					m_aResultListActions[nDetail][RightMouseButtonClick]->m_pExtraInfo,((NMHDR_MOUSE*)pNm)->iItem,nDetail);
			}
		}
		break;
	case NMX_DBLCLICK:
		if (!IsExtraFlagSet(efLVIgnoreListClicks))		
		{
            if (m_ClickWait)
				break;

			DetailType nDetail=DetailType(m_pListCtrl->GetColumnIDFromSubItem(((NMHDR_MOUSE*)pNm)->iSubItem));
			if (nDetail>LastType || nDetail<0)
				break;

			if (m_aResultListActions[nDetail][LeftMouseButtonDblClick]!=NULL)
			{
				CWaitCursor wait;
	            m_ClickWait=TRUE;
				OnExecuteResultAction(m_aResultListActions[nDetail][LeftMouseButtonDblClick]->m_nResultList,
					m_aResultListActions[nDetail][LeftMouseButtonDblClick]->m_pExtraInfo,((NMHDR_MOUSE*)pNm)->iItem,nDetail);

				SetTimer(ID_CLICKWAIT,500,NULL);
			}
		}
		break;
	case NMX_RDBLCLICK:
		if (!IsExtraFlagSet(efLVIgnoreListClicks))		
		{
            if (m_ClickWait)
				break;

			DetailType nDetail=DetailType(m_pListCtrl->GetColumnIDFromSubItem(((NMHDR_MOUSE*)pNm)->iSubItem));
			if (nDetail>LastType || nDetail<0)
				break;

			if (m_aResultListActions[nDetail][RightMouseButtonDblClick]!=NULL)
			{
				CWaitCursor wait;
	            m_ClickWait=TRUE;
				OnExecuteResultAction(m_aResultListActions[nDetail][RightMouseButtonDblClick]->m_nResultList,
					m_aResultListActions[nDetail][RightMouseButtonDblClick]->m_pExtraInfo,((NMHDR_MOUSE*)pNm)->iItem,nDetail);

				SetTimer(ID_CLICKWAIT,500,NULL);
			}
		}
		break;
	case NMX_MCLICK:
		if (!IsExtraFlagSet(efLVIgnoreListClicks))		
		{
			DetailType nDetail=DetailType(m_pListCtrl->GetColumnIDFromSubItem(((NMHDR_MOUSE*)pNm)->iSubItem));
			if (nDetail>LastType || nDetail<0)
				break;

			if (m_aResultListActions[nDetail][MiddleMouseButtonClick]!=NULL)
			{
				CWaitCursor wait;
				OnExecuteResultAction(m_aResultListActions[nDetail][MiddleMouseButtonClick]->m_nResultList,
					m_aResultListActions[nDetail][MiddleMouseButtonClick]->m_pExtraInfo,((NMHDR_MOUSE*)pNm)->iItem,nDetail);
			}
		}
		break;
	case NMX_MDBLCLICK:
		if (!IsExtraFlagSet(efLVIgnoreListClicks))		
		{
            if (m_ClickWait)
				break;

			DetailType nDetail=DetailType(m_pListCtrl->GetColumnIDFromSubItem(((NMHDR_MOUSE*)pNm)->iSubItem));
			if (nDetail>LastType || nDetail<0)
				break;

			if (m_aResultListActions[nDetail][MiddleMouseButtonDblClick]!=NULL)
			{
				CWaitCursor wait;
	            m_ClickWait=TRUE;
				OnExecuteResultAction(m_aResultListActions[nDetail][MiddleMouseButtonDblClick]->m_nResultList,
					m_aResultListActions[nDetail][MiddleMouseButtonDblClick]->m_pExtraInfo,((NMHDR_MOUSE*)pNm)->iItem,nDetail);

				SetTimer(ID_CLICKWAIT,500,NULL);
			}
		}
		break;
	case LVN_COLUMNCLICK:
		SortItems(DetailType(m_pListCtrl->GetColumnIDFromSubItem(pNm->iSubItem)),-1,TRUE);
		break;
	case LVN_GETDISPINFOA:
		{
			LV_DISPINFOA *pLvdi=(LV_DISPINFOA*)pNm;

			CLocatedItem* pItem=(CLocatedItem*)pLvdi->item.lParam;
			if (pItem==NULL)
				break;

			
			DetailType nDetail=DetailType(m_pListCtrl->GetColumnIDFromSubItem(pLvdi->item.iSubItem));

			

			if (GetFlags()&fgLVNoDelayedUpdate) 
			{
				ItemDebugFormatMessage1("LVN_GETDISPINFOA %d BEGIN1",nDetail);
			
				switch (nDetail)
				{
				// Title and parent are special since they have icons
				case Name:
					if (pItem->ShouldUpdateFileTitle())
						pItem->UpdateFileTitle();
					if (pItem->ShouldUpdateIcon())
                        pItem->UpdateIcon();
					if (pItem->ShouldCheckIfDeleted())
                        pItem->CheckIfDeleted();

					pLvdi->item.mask=LVIF_TEXT|LVIF_IMAGE;
					if (pItem->GetFileTitle()!=NULL)
						pLvdi->item.pszText=alloccopyWtoA(pItem->GetFileTitle());
					else
						pLvdi->item.pszText=alloccopyWtoA(pItem->GetName());
					AssignBuffer(pLvdi->item.pszText);
					
					pLvdi->item.iImage=pItem->GetIcon();

					if (pItem->GetAttributes()&(LITEMATTRIB_CUTTED|LITEMATTRIB_HIDDEN))
					{
						pLvdi->item.mask|=LVIF_STATE;
						pLvdi->item.state=LVIS_CUT;
						pLvdi->item.stateMask=LVIS_CUT;
					}
					break;
				case InFolder:
					if (pItem->ShouldUpdateParentIcon())
						pItem->UpdateParentIcon();

					pLvdi->item.mask=LVIF_TEXT|LVIF_IMAGE;
					pLvdi->item.pszText=alloccopyWtoA(pItem->GetParent());
					AssignBuffer(pLvdi->item.pszText);
					pLvdi->item.iImage=pItem->GetParentIcon();
					break;
				default:
					ASSERT (nDetail<=LastType);
			
					if (pItem->ShouldUpdateByDetail(nDetail))
						pItem->UpdateByDetail(nDetail);
					pLvdi->item.mask=LVIF_TEXT;
					pLvdi->item.pszText=alloccopyWtoA(pItem->GetDetailText(nDetail));
					AssignBuffer(pLvdi->item.pszText);
					break;
				}
				
				ItemDebugMessage("LVN_GETDISPINFOA END1");
			}
			else
			{
				ItemDebugFormatMessage1("LVN_GETDISPINFOA %d BEGIN2",nDetail);
			
				// Delayed updating
				switch (nDetail)
				{
				// Title and parent are special since they have icons
				case Name:
					if (pItem->ShouldUpdateFileTitle() || pItem->ShouldUpdateIcon())
					{
						ASSERT (m_pBackgroundUpdater!=NULL);

						m_pBackgroundUpdater->AddToUpdateList(pItem,pLvdi->item.iItem,Name);
						
						if (!IsLocating()) // Locating in process
							m_pBackgroundUpdater->StopWaiting();
					}
					
					if (pItem->ShouldCheckIfDeleted())
					{
						ASSERT (m_pBackgroundUpdater!=NULL);

						m_pBackgroundUpdater->AddToUpdateList(pItem,pLvdi->item.iItem,IfDeleted);
						
						if (!IsLocating()) // Locating in process
							m_pBackgroundUpdater->StopWaiting();
					}
					
					pLvdi->item.mask=LVIF_TEXT|LVIF_IMAGE;
					if (pItem->GetFileTitle()!=NULL)
						pLvdi->item.pszText=alloccopyWtoA(pItem->GetFileTitle());
					else
						pLvdi->item.pszText=alloccopyWtoA(pItem->GetName());
					AssignBuffer(pLvdi->item.pszText);
					pLvdi->item.iImage=pItem->GetIcon();

					if (pItem->GetAttributes()&(LITEMATTRIB_CUTTED|LITEMATTRIB_HIDDEN))
					{
						pLvdi->item.mask|=LVIF_STATE;
						pLvdi->item.state=LVIS_CUT;
						pLvdi->item.stateMask=LVIS_CUT;
					}
					break;
				case InFolder:
					if (pItem->ShouldUpdateParentIcon())
					{
						ASSERT (m_pBackgroundUpdater!=NULL);

						m_pBackgroundUpdater->AddToUpdateList(pItem,pLvdi->item.iItem,InFolder);
						if (!IsLocating()) // Locating is not in process
							m_pBackgroundUpdater->StopWaiting();
					}

					pLvdi->item.mask=LVIF_TEXT|LVIF_IMAGE;
					pLvdi->item.pszText=alloccopyWtoA(pItem->GetParent());
					AssignBuffer(pLvdi->item.pszText);
					pLvdi->item.iImage=pItem->GetParentIcon();
					break;
				default:
					ASSERT (nDetail<=LastType);
			
					if (pItem->ShouldUpdateByDetail(nDetail))
					{
						ASSERT (m_pBackgroundUpdater!=NULL);

						m_pBackgroundUpdater->AddToUpdateList(pItem,pLvdi->item.iItem,nDetail);
						if (!IsLocating()) // Locating is not in process
							m_pBackgroundUpdater->StopWaiting();
					}
					pLvdi->item.mask=LVIF_TEXT;
					pLvdi->item.pszText=alloccopyWtoA(pItem->GetDetailText(nDetail));
					AssignBuffer(pLvdi->item.pszText);
					break;
				}
				ItemDebugMessage("LVN_GETDISPINFOA END2");
			}
			break;
		}
	case LVN_GETDISPINFOW:
		{
			LV_DISPINFOW *pLvdi=(LV_DISPINFOW*)pNm;

			CLocatedItem* pItem=(CLocatedItem*)pLvdi->item.lParam;
			if (pItem==NULL)
				break;

			
			DetailType nDetail=DetailType(m_pListCtrl->GetColumnIDFromSubItem(pLvdi->item.iSubItem));

			if (GetFlags()&fgLVNoDelayedUpdate) 
			{
				ItemDebugFormatMessage1("LVN_GETDISPINFOW %d BEGIN1",nDetail);
				// Update detail instantaneously

				switch (nDetail)
				{
				// Title and parent are special since they have icons
				case Name:
					if (pItem->ShouldUpdateFileTitle())
						pItem->UpdateFileTitle();
					if (pItem->ShouldUpdateIcon())
                        pItem->UpdateIcon();
					if (pItem->ShouldCheckIfDeleted())
						pItem->CheckIfDeleted();

					pLvdi->item.mask=LVIF_TEXT|LVIF_IMAGE;
					if (pItem->GetFileTitle()!=NULL)
						pLvdi->item.pszText=pItem->GetFileTitle();
					else
						pLvdi->item.pszText=pItem->GetName();
					
					pLvdi->item.iImage=pItem->GetIcon();

					if (pItem->GetAttributes()&(LITEMATTRIB_CUTTED|LITEMATTRIB_HIDDEN))
					{
						pLvdi->item.mask|=LVIF_STATE;
						pLvdi->item.state=LVIS_CUT;
						pLvdi->item.stateMask=LVIS_CUT;
					}
					break;
				case InFolder:
					if (pItem->ShouldUpdateParentIcon())
						pItem->UpdateParentIcon();

					pLvdi->item.mask=LVIF_TEXT|LVIF_IMAGE;
					pLvdi->item.pszText=pItem->GetParent();
					pLvdi->item.iImage=pItem->GetParentIcon();
					break;
				default:
					ASSERT (nDetail<=LastType);
			
					if (pItem->ShouldUpdateByDetail(nDetail))
						pItem->UpdateByDetail(nDetail);
					pLvdi->item.mask=LVIF_TEXT;
					pLvdi->item.pszText=pItem->GetDetailText(nDetail);
					break;
				}
				ItemDebugMessage("LVN_GETDISPINFOW END1");
			}
			else
			{
				ItemDebugFormatMessage1("LVN_GETDISPINFOW %d BEGIN2",nDetail);
			
				// Delayed updating
				switch (nDetail)
				{
				// Title and parent are special since they have icons
				case Name:
					if (pItem->ShouldUpdateFileTitle() || pItem->ShouldUpdateIcon())
					{
						ASSERT (m_pBackgroundUpdater!=NULL);

						m_pBackgroundUpdater->AddToUpdateList(pItem,pLvdi->item.iItem,Name);
						
						if (!IsLocating()) // Locating in process
							m_pBackgroundUpdater->StopWaiting();
					}
					
					if (pItem->ShouldCheckIfDeleted())
					{
						ASSERT (m_pBackgroundUpdater!=NULL);

						m_pBackgroundUpdater->AddToUpdateList(pItem,pLvdi->item.iItem,IfDeleted);
						
						if (!IsLocating()) // Locating in process
							m_pBackgroundUpdater->StopWaiting();
					}
					
					pLvdi->item.mask=LVIF_TEXT|LVIF_IMAGE;
					if (pItem->GetFileTitle()!=NULL)
						pLvdi->item.pszText=pItem->GetFileTitle();
					else
						pLvdi->item.pszText=pItem->GetName();
					pLvdi->item.iImage=pItem->GetIcon();

					if (pItem->GetAttributes()&(LITEMATTRIB_CUTTED|LITEMATTRIB_HIDDEN))
					{
						pLvdi->item.mask|=LVIF_STATE;
						pLvdi->item.state=LVIS_CUT;
						pLvdi->item.stateMask=LVIS_CUT;
					}
					break;
				case InFolder:
					if (pItem->ShouldUpdateParentIcon())
					{
						ASSERT (m_pBackgroundUpdater!=NULL);

						//DebugFormatMessage("Calling %X->AddToUpdateList(%X,%X,InFolder)",m_pBackgroundUpdater,pItem,pLvdi->item.iItem);
						m_pBackgroundUpdater->AddToUpdateList(pItem,pLvdi->item.iItem,InFolder);
						if (!IsLocating()) // Locating is not in process
							m_pBackgroundUpdater->StopWaiting();
					}

					pLvdi->item.mask=LVIF_TEXT|LVIF_IMAGE;
					pLvdi->item.pszText=pItem->GetParent();
					pLvdi->item.iImage=pItem->GetParentIcon();
					break;
				default:
					ASSERT (nDetail<=LastType);
			
					if (pItem->ShouldUpdateByDetail(nDetail))
					{
						ASSERT (m_pBackgroundUpdater!=NULL);

						m_pBackgroundUpdater->AddToUpdateList(pItem,pLvdi->item.iItem,nDetail);
						if (!IsLocating()) // Locating is not in process
							m_pBackgroundUpdater->StopWaiting();
					}
					pLvdi->item.mask=LVIF_TEXT;
					pLvdi->item.pszText=pItem->GetDetailText(nDetail);
					break;
				}
				ItemDebugMessage("LVN_GETDISPINFOW END2");
			}
			break;
		}
	case LVN_DELETEITEM:
		if (pNm->lParam!=NULL)
		{
			if (m_pBackgroundUpdater!=NULL)
				m_pBackgroundUpdater->RemoveFromUpdateList((CLocatedItem*)pNm->lParam);
			delete (CLocatedItem*)pNm->lParam;
		}
		break;
	case LVN_BEGINDRAG:
	case LVN_BEGINRDRAG:
		if (m_pListCtrl->GetNextItem(-1,LVNI_SELECTED)!=-1)
			BeginDragFiles(m_pListCtrl);
		break;
	case LVN_BEGINLABELEDITA:
	case LVN_BEGINLABELEDITW:
		{
			NMLVDISPINFOA* pDistInfo=(NMLVDISPINFOA*)pNm;
			if (m_pListCtrl->GetColumnIDFromSubItem(pDistInfo->item.iSubItem)!=Name)
			{
				// Not allowed for other fields than Name
				SetWindowLong(dwlMsgResult,TRUE);
				return TRUE;
			}

			CLocatedItem* pItem=(CLocatedItem*)m_pListCtrl->GetItemData(pDistInfo->item.iItem);
			if (pItem==NULL)
			{
				// Not allowed for other fields than Name
				SetWindowLong(dwlMsgResult,TRUE);
				return TRUE;
			}

			ASSERT(!pItem->ShouldUpdateFileTitle());
			
			if (pItem->GetFileTitle()!=pItem->GetName())
			{
				int nTitleLen=istrlen(pItem->GetFileTitle());
				if (_wcsnicmp(pItem->GetName(),pItem->GetFileTitle(),nTitleLen)!=0)
				{
					// We can't determine which part should be changed in filename
					SetWindowLong(dwlMsgResult,TRUE);
					return TRUE;
				}
			}

			AddExtraFlags(efLVRenamingActivated);

			SetWindowLong(dwlMsgResult,FALSE);
			return FALSE;
		}
		break;
	case LVN_ENDLABELEDITA:
		{
			RemoveExtraFlags(efLVRenamingActivated);

			NMLVDISPINFOA* pDistInfo=(NMLVDISPINFOA*)pNm;
			if (pDistInfo->item.pszText==NULL)
				return TRUE;

			if (m_pListCtrl->GetColumnIDFromSubItem(pDistInfo->item.iSubItem)!=Name)
			{
				// Not allowed for other fields than Name
				SetWindowLong(dwlMsgResult,FALSE);
				return FALSE;
			}

			CLocatedItem* pItem=(CLocatedItem*)m_pListCtrl->GetItemData(pDistInfo->item.iItem);
			if (pItem==NULL)
			{
				// Unexpected situation
				SetWindowLong(dwlMsgResult,FALSE);
				return FALSE;
			}

			BOOL bOK=FALSE;

			if (pItem->GetFileTitle()!=pItem->GetName())
			{
				int nOldTitleLen=istrlen(pItem->GetFileTitle());
				if (_wcsnicmp(pItem->GetName(),pItem->GetFileTitle(),nOldTitleLen)!=0)
				{
					// We can't determine which part should be changed in filename
					SetWindowLong(dwlMsgResult,FALSE);
					return FALSE;
				}
				int nNewTitleLen=istrlen(pDistInfo->item.pszText);
				if (nNewTitleLen>0)
				{
					ASSERT(pItem->GetNameLen()>=(UINT)nOldTitleLen);

					WCHAR* pNewName=new WCHAR[nNewTitleLen+pItem->GetNameLen()-nOldTitleLen+1];
					nNewTitleLen=MemCopyAtoW(pNewName,nNewTitleLen,pDistInfo->item.pszText,nNewTitleLen);
					MemCopyW(pNewName+nNewTitleLen,pItem->GetName()+nOldTitleLen,pItem->GetNameLen()-nOldTitleLen+1);
					bOK=pItem->ChangeName(this,pNewName,nNewTitleLen+pItem->GetNameLen()-nOldTitleLen+1);
					delete[] pNewName;
				}
			}
			else if (pDistInfo->item.pszText[0]!='\0')
				bOK=pItem->ChangeName(this,A2W(pDistInfo->item.pszText));

			
			SetWindowLong(dwlMsgResult,bOK);
			return bOK;
		}
	case LVN_ENDLABELEDITW:
		{
			RemoveExtraFlags(efLVRenamingActivated);


			NMLVDISPINFOW* pDistInfo=(NMLVDISPINFOW*)pNm;
			if (pDistInfo->item.pszText==NULL)
				return TRUE;

			if (m_pListCtrl->GetColumnIDFromSubItem(pDistInfo->item.iSubItem)!=Name)
			{
				// Not allowed for other fields than Name
				SetWindowLong(dwlMsgResult,FALSE);
				return FALSE;
			}

			CLocatedItem* pItem=(CLocatedItem*)m_pListCtrl->GetItemData(pDistInfo->item.iItem);
			if (pItem==NULL)
			{
				// This sould not happen
				SetWindowLong(dwlMsgResult,FALSE);
				return FALSE;
			}

			BOOL bOK=FALSE;

			if (pItem->GetFileTitle()!=pItem->GetName())
			{
				int nOldTitleLen=istrlen(pItem->GetFileTitle());
				if (_wcsnicmp(pItem->GetName(),pItem->GetFileTitle(),nOldTitleLen)!=0)
				{
					// We can't determine which part should be changed in filename
					SetWindowLong(dwlMsgResult,FALSE);
					return FALSE;
				}
				int nNewTitleLen=istrlen(pDistInfo->item.pszText);
				if (nNewTitleLen>0)
				{
					ASSERT(pItem->GetNameLen()>=(UINT)nOldTitleLen);

					WCHAR* pNewName=new WCHAR[nNewTitleLen+pItem->GetNameLen()-nOldTitleLen+1];
					MemCopyW(pNewName,pDistInfo->item.pszText,nNewTitleLen);
					MemCopyW(pNewName+nNewTitleLen,pItem->GetName()+nOldTitleLen,pItem->GetNameLen()-nOldTitleLen+1);
					bOK=pItem->ChangeName(this,pNewName,nNewTitleLen+pItem->GetNameLen()-nOldTitleLen+1);
					delete[] pNewName;
				}
			}
			else if (pDistInfo->item.pszText[0]!='\0')
				bOK=pItem->ChangeName(this,pDistInfo->item.pszText);
				

			SetWindowLong(dwlMsgResult,bOK);
			return bOK;
		}
	case LVN_GETINFOTIP:
		break;
	case LVN_ITEMCHANGED:
		if ((!(pNm->uOldState&LVIS_SELECTED) && pNm->uNewState&LVIS_SELECTED)||
			(!(pNm->uNewState&LVIS_SELECTED) && pNm->uOldState&LVIS_SELECTED))
		{
			SetTimer(ID_UPDATESELECTED,100,NULL);
		}
		break;
	case NM_CUSTOMDRAW:
		{
			SetWindowLong(dwlMsgResult,
				ListCustomDrawHandler((NMLVCUSTOMDRAW*)pNm));
			return TRUE;
		}
			
	}
	return FALSE;
}


HRESULT CLocateDlg::ListCustomDrawHandler(NMLVCUSTOMDRAW* pLVCD)
{
	switch (pLVCD->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		// We want notification messages
		return CDRF_NOTIFYITEMDRAW;
	case CDDS_ITEMPREPAINT:
		// Let control paint item if in Details, List or Small Icons  view mode
		if (m_nCurrentListType==ltMediumIcons &&
			!IsThumbnailFlagSet(tfThumbnailsInMediumIcons))
			return CDRF_DODEFAULT;
		
		/*
		
		DebugFormatMessage("pLVCD: clrText=%X clrTextBk=%X dwItemType=%X clrFace=%X iIconEffect=%d iIconPhase=%d",
			pLVCD->clrText,
			pLVCD->clrTextBk,
			pLVCD->dwItemType,
			pLVCD->clrFace,
			pLVCD->iIconEffect,
			pLVCD->iIconPhase);
		
		DebugFormatMessage("pLVCD(cont): iPartId=%d iStateId=%d rcText=(%d,%d,%d,%d) uAlign=%X",
			pLVCD->iPartId,
			pLVCD->iStateId,
			pLVCD->rcText.left,pLVCD->rcText.right,
			pLVCD->rcText.top,pLVCD->rcText.bottom,
			pLVCD->uAlign);

		DebugFormatMessage("nmcd: dwItemSpec=%d, dwDrawStage=%X rc=(%d,%d,%d,%d) uItemState=%X lItemlParam=%X",
			pLVCD->nmcd.dwItemSpec,
			pLVCD->nmcd.dwDrawStage,
			pLVCD->nmcd.rc.left,pLVCD->nmcd.rc.right,
			pLVCD->nmcd.rc.top,pLVCD->nmcd.rc.bottom,
			pLVCD->nmcd.uItemState,
			pLVCD->nmcd.lItemlParam);
		
		
		*/

		if (m_sCurrentIconSize.cx!=0)			
		{
			// Large image mode, lets paint item itself
			
			// First check some anomalies (may we skip this)
			int iItem=static_cast<int>(pLVCD->nmcd.dwItemSpec);
			
			// In vista, these may be 0 when we should skip this
			if (pLVCD->clrText==0 && pLVCD->clrTextBk==0 && pLVCD->clrFace==0)
				return CDRF_DODEFAULT;


			CDC dc(pLVCD->nmcd.hdc);
			CRect rcItem,rcCalculated;
			HBRUSH hBrush;
			BOOL bListHasFocus=::GetFocus()==*m_pListCtrl;
			int nRoundRectExtra=1;

			// Get Icon area to rcItem
			m_pListCtrl->GetItemRect(iItem,&rcItem,LVIR_ICON);
			
			// In Windows XP these can be 0, then skip
			if (rcItem.left==0 && rcItem.right==0 && rcItem.top==0 && rcItem.bottom==0)
				return CDRF_DODEFAULT;
		
			
			// Get item state and FileItem (lParam), the state is not 
			// necessaryly the same as in pLVCD->nmcd.uItemState

			LVITEM li;
			li.mask=LVIF_PARAM|LVIF_STATE;
			li.iItem=iItem;
			li.iSubItem=0;
			li.stateMask=LVIS_SELECTED|LVIS_FOCUSED;
			m_pListCtrl->GetItem(&li);
			CLocatedItem* pItem=(CLocatedItem*)li.lParam;

			ASSERT(li.lParam==pLVCD->nmcd.lItemlParam);

			
			
			
			// LVN_GETDISPINFO is not called, requesting update here
			if (GetFlags()&fgLVNoDelayedUpdate) 
			{
				if (pItem->ShouldUpdateFileTitle())
					pItem->UpdateFileTitle();
				if (pItem->ShouldUpdateIcon())
					pItem->UpdateIcon();
				if (pItem->ShouldCheckIfDeleted())
					pItem->CheckIfDeleted();
				if (pItem->ShouldUpdateExtra(Thumbnail))
					pItem->LoadThumbnail();
			}
			else
			{
				if (pItem->ShouldUpdateFileTitle() || pItem->ShouldUpdateIcon())
				{
					ASSERT (m_pBackgroundUpdater!=NULL);

					m_pBackgroundUpdater->AddToUpdateList(pItem,iItem,Name);
					
					if (!IsLocating()) // Locating in process
						m_pBackgroundUpdater->StopWaiting();
				}
				if (pItem->ShouldCheckIfDeleted())
				{
					ASSERT (m_pBackgroundUpdater!=NULL);

					m_pBackgroundUpdater->AddToUpdateList(pItem,iItem,IfDeleted);
					
					if (!IsLocating()) // Locating in process
						m_pBackgroundUpdater->StopWaiting();			
				}
				if (pItem->ShouldUpdateExtra(Thumbnail))
				{
					ASSERT (m_pBackgroundUpdater!=NULL);

					m_pBackgroundUpdater->AddToUpdateList(pItem,iItem,Thumbnail);
					
					if (!IsLocating()) // Locating in process
						m_pBackgroundUpdater->StopWaiting();			
				}
			}
			
			// Loading thumbnail if necessary
			SIZE sThumbnailSize;
			HBITMAP hThumbnail=pItem->GetThumbnail(sThumbnailSize);
			if (hThumbnail==NULL)
				sThumbnailSize=m_sSystemImageList;
		
			BOOL bDrawBoundingBox=(/*pFileItem->m_nThumbnailType==FileItem::Bitmap &&*/ hThumbnail!=NULL) ||
				(sThumbnailSize.cx<m_sCurrentIconSize.cx &&
				sThumbnailSize.cy<m_sCurrentIconSize.cy);


			

			// Calculate area for thumbnail rect
			POINT ptThumb={rcItem.left+(rcItem.Width()-sThumbnailSize.cx)/2,
					  rcItem.top+(rcItem.Height()-sThumbnailSize.cy)/2} ;
		
			// Drawing icon/thumbnail
			if (hThumbnail!=NULL)
			{
				// Valid thumbnail, drawing it
				
				//if (pFileItem->m_nThumbnailType==FileItem::Icon)
				//	dc.DrawState(ptThumb,pFileItem->m_sThumbnailSize,pFileItem->m_hIcon,DST_ICON);
				//else 
				
				if (m_dwThumbnailFlags&tfVistaFeaturesAvailable)
				{
					// In Windows Vista this seems to be the only suitable way to 
					// draw images, otherwise some wierd RGBA images (such as 
					// folder thumbnails get black background)
					CImageList List;
					List.Create(sThumbnailSize.cx,sThumbnailSize.cy,ILC_COLOR32,1,0);
					int nIndex=List.Add(hThumbnail,(HBITMAP)NULL);
					List.Draw(dc,nIndex,ptThumb,ILD_NORMAL);
					List.DeleteImageList();
			
				}
				else
					dc.DrawState(ptThumb,sThumbnailSize,hThumbnail,DST_BITMAP);
					
			}
			else 
			{
				UINT fStyle=ILD_NORMAL|ILD_TRANSPARENT;
				if (!bDrawBoundingBox)
					fStyle|=(li.state&LVIS_SELECTED?ILD_SELECTED:0)|(li.state&LVIS_FOCUSED?ILD_FOCUS:0);


				ImageList_Draw(m_dwThumbnailFlags&tfSystemImageListIsInterface?
					IImageListToHIMAGELIST(m_pSystemImageList):m_hSystemImageList,
					pItem->GetIcon(),dc,ptThumb.x,ptThumb.y,fStyle);
			}
			
			
	
			// Draw bounding box if mode is large or extra large mode or medium and thumbnail is image
			if (bDrawBoundingBox)
			{

				// Pen for rounding rect;
				CPen Pen;
				if (bListHasFocus && li.state&LVIS_SELECTED)
				{
					Pen.CreatePen(PS_SOLID,nRoundRectExtra=3,RGB(0,0,0));
					nRoundRectExtra=2;
				}
				else
					Pen.CreatePen(PS_SOLID,1,RGB(128,128,128));
				
				// Calculate area for thumbnail and rounding rect
				rcCalculated.left=rcItem.left+(rcItem.Width()-m_sCurrentIconSize.cx)/2-nRoundRectExtra;
				rcCalculated.top=rcItem.top+(rcItem.Height()-m_sCurrentIconSize.cy)/2-nRoundRectExtra;
				rcCalculated.right=rcCalculated.left+m_sCurrentIconSize.cx+2*nRoundRectExtra;
				rcCalculated.bottom=rcCalculated.top+m_sCurrentIconSize.cy+2*nRoundRectExtra;

					
				// Draw rounding rect
				dc.SelectStockObject(NULL_BRUSH);
				dc.SelectObject(Pen);
				dc.Rectangle(&rcCalculated);
			}
			
			// Get area for label
			m_pListCtrl->GetItemRect(iItem,&rcItem,LVIR_LABEL);
			

			// Choose text and background colors
			if (li.state&LVIS_SELECTED)
            {
				if (bListHasFocus) 
				{
					hBrush=(HBRUSH)(COLOR_HIGHLIGHT+1);
					dc.SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
				}
				else
				{
					hBrush=(HBRUSH)(COLOR_BTNFACE+1);
					dc.SetTextColor(GetSysColor(COLOR_BTNTEXT));
				}
            }
			else
            {
				hBrush=(HBRUSH)(COLOR_WINDOW+1);
				dc.SetTextColor(GetSysColor(COLOR_BTNTEXT));
            }
			
			// Fill background and set background mode to transparent
			dc.FillRect(&rcItem,hBrush);
			dc.SetBkMode(TRANSPARENT);



			// Draw text
			LPCWSTR lpText=pItem->GetFileTitleSafe();
			int iStrlen=istrlen(pItem->GetFileTitleSafe());

			rcCalculated=rcItem;
			dc.DrawText(lpText,iStrlen,&rcCalculated,DT_VCENTER|DT_CENTER|DT_CALCRECT|DT_WORDBREAK);

			if (rcCalculated.right<=rcItem.right && rcCalculated.bottom<=rcItem.bottom)
			{
				// Text fits to control, draw it as is
				dc.DrawText(lpText,iStrlen,&rcItem,DT_VCENTER|DT_CENTER|DT_WORDBREAK);
			}
			else
			{
				// Text do not fit to control

				// Calculate size as a single line (to check how many lines we can use)
				ZeroMemory(&rcCalculated,sizeof(CRect));
				dc.DrawText(lpText,iStrlen,&rcCalculated,DT_VCENTER|DT_CENTER|DT_CALCRECT|DT_SINGLELINE);

				int nLines=rcItem.Height()/rcCalculated.Height();

				if (nLines==1)
				{
					// Just one line, take so line as possible

					// Estimated cut position
					int iCutPos=(rcItem.Width()*iStrlen)/rcCalculated.Width()+4;
					CStringW Text(lpText,iCutPos);
					Text << L"....";

					while (iCutPos>0)
					{
						// Calculate new width
						rcCalculated=rcItem;
						dc.DrawText(Text,&rcCalculated,DT_VCENTER|DT_CENTER|DT_CALCRECT);


						if (rcCalculated.right<=rcItem.right)
						{
							// Fits to area
							break;
						}

						// Reduce cut position and try again
						Text.DelChar(iCutPos);
						iCutPos--;
					}
					
					dc.DrawText(Text,&rcItem,DT_VCENTER|DT_CENTER);
				}
				else
				{
					// Find line breaks
					CStringW Text(lpText,iStrlen);

					int nStartPos=0;

					for (;;)
					{
						// Calculate line break position
						
						// Estimatee for  cut position
						int iCutPos=(rcItem.Width()*iStrlen)/rcCalculated.Width()+4;
						CStringW Temp((LPCWSTR)Text+nStartPos,iCutPos);
						Temp << L"\ni";
						
							
						while (iCutPos>0)
						{
							// Calculate new width
							ZeroMemory(&rcCalculated,sizeof(CRect));
							dc.DrawText(Temp,&rcCalculated,DT_VCENTER|DT_CENTER|DT_CALCRECT);

							if (rcCalculated.right<=rcItem.Width())
							{
								// Fits to area, position is ok
								break;
							}

							// Reduce cut position and try again
							Temp.DelChar(--iCutPos);
						}

						// Check if there is a space in this line
						int iSpacePos;
						for (iSpacePos=iCutPos;iSpacePos>0 && Temp[iSpacePos]!=' ';iSpacePos--);

						if (iSpacePos>0)
						{
							// There is a space
							Text[nStartPos+iSpacePos]=L'\n';
							nStartPos+=iSpacePos+1;
							iStrlen-=iSpacePos+1;
						}
						else
						{
							// Insert line break
							Text.InsChar(nStartPos+iCutPos,L'\n');

							// Set start position
							nStartPos+=iCutPos+1;
							iStrlen-=iCutPos;	

							// Ignore one space if it is just after the line break
							if (Text[nStartPos]==' ')
							{
								Text.DelChar(nStartPos);
								iStrlen--;
							}
						}

						// Should not happen but just for sure 
						ASSERT(iStrlen>0);
						
						// Calculate length for the rest of the string
						ZeroMemory(&rcCalculated,sizeof(CRect));
						dc.DrawText((LPCWSTR)Text+nStartPos,iStrlen,&rcCalculated,DT_VCENTER|DT_CENTER|DT_CALCRECT|DT_SINGLELINE);

						if (rcCalculated.right<=rcItem.Width())
						{
							// There is enough space for the rest of string
							break;
						}

					}

					dc.DrawText(Text,&rcItem,DT_VCENTER|DT_CENTER);
				}
			}

			// Draw a focus rect aroung the item 
			if (bListHasFocus && li.state&LVIS_FOCUSED)
				dc.DrawFocusRect(&rcItem);
			

			return CDRF_SKIPDEFAULT;
		}

		// Let control paint item if in Details, List or Small Icons  view mode
		return CDRF_DODEFAULT;
	}
	return NOERROR;
}

////////////////////////////////////////////////////////////
// CLocateDlg - Results list - Sorting
////////////////////////////////////////////////////////////

void CLocateDlg::SortItems(DetailType nDetail,BYTE bDescending,BOOL bNoneIsPossible)
{
	DebugFormatMessage("CLocateDlg::SortItems(%X,%d) BEGIN",int(nDetail),bDescending);

	// no sorting: m_nSorting=0xFF
	// descent=m_nSorting&128
	// detail=m_nSorting&127

	
	CWaitCursor wait;

	if ((m_nSorting&127)!=nDetail)
	{
		// Not same column, remove arrow
		SetSortArrowToHeader(DetailType(m_nSorting&127),TRUE,FALSE); 
	}

	// Toggle?        
	if (bDescending==BYTE(-1))
	{
		if ((m_nSorting&127)!=nDetail) // Different columnt, always ascending
			bDescending=FALSE;
		else if (bNoneIsPossible && m_nSorting&128)
		{
			// Disable sorting

			// Remove arrow
			SetSortArrowToHeader(DetailType(m_nSorting&127),TRUE,FALSE); 

			m_nSorting=BYTE(-1);	
			return;
		}
		else
			bDescending=(m_nSorting&128)==0;
	}

	if (nDetail!=Extension)
		SetSortArrowToHeader(nDetail,FALSE,bDescending);

	if (!bDescending)
	{ 
		// Ascending
		DebugFormatMessage("Going to sort(1), nColumn is %X",LPARAM(nDetail));
		BOOL bRet=m_pListCtrl->SortItems(ListCompareProc,(DWORD)(nDetail));
		DebugFormatMessage("bRet=%X",bRet);
		m_nSorting=nDetail&127;
	}
	else
	{
		// Descending
		DebugFormatMessage("Going to sort(2), nColumn is %X",LPARAM(nDetail));
		BOOL bRet=m_pListCtrl->SortItems(ListCompareProc,(DWORD)(nDetail|128));
		DebugFormatMessage("bRet=%X",bRet);
		m_nSorting=nDetail|128;
	}

	if (nDetail!=Extension)
	{
		int nColumn=m_pListCtrl->GetVisibleColumn(m_pListCtrl->GetColumnFromID(nDetail));
		if (nColumn!=-1)
			m_pListCtrl->SendMessage(LVM_FIRST+140/* LVM_SETSELECTEDCOLUMN */,nColumn,0);
	}

	DebugMessage("CLocateDlg::SortItems END");
}

void CLocateDlg::SetSorting(BYTE bSorting)
{
	if (bSorting==BYTE(-2))
	{
		CRegKey2 RegKey;
		bSorting=BYTE(-1);
		
		if (RegKey.OpenKey(HKCU,"\\General",CRegKey::defRead)==ERROR_SUCCESS)
		{
			DWORD nTemp=BYTE(-1);
			if (RegKey.QueryValue("Default Sorting",nTemp))
				bSorting=(BYTE)nTemp;
		}
	}


	if (bSorting==m_nSorting)
		return;

	if ((m_nSorting&127)!=(bSorting&127))
	{
		// Not same column, remove arrow
		SetSortArrowToHeader(DetailType(m_nSorting&127),TRUE,FALSE); 
	}

	m_nSorting=bSorting;
	
	SetSortArrowToHeader(DetailType(bSorting&127),FALSE,(bSorting&128)?TRUE:FALSE);
}

int CALLBACK CLocateDlg::ListCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CLocatedItem* pItem1=(CLocatedItem*)lParam1;
	CLocatedItem* pItem2=(CLocatedItem*)lParam2;
	
	CLocateDlg* pLocateDlg=GetLocateDlg();

	// If "Show folders first" is chosen, separate files and directories
	if (pLocateDlg->GetFlags()&fgLVFoldersFirst)
	{
		if (pItem1->IsFolder() && !pItem2->IsFolder())
			return -1;
		else if (!pItem1->IsFolder() && pItem2->IsFolder())
			return 1;
	}

	

	DetailType nDetail=DetailType(lParamSort&127);
	switch (nDetail)
	{
	case Name:
		if (pItem1->ShouldUpdateFileTitle())
			pItem1->UpdateFileTitle();
		if (pItem2->ShouldUpdateFileTitle())
			pItem2->UpdateFileTitle();
		
		if (lParamSort&128)
			return pLocateDlg->m_pStrCmp(pItem2->GetFileTitle(),pItem1->GetFileTitle());
		return pLocateDlg->m_pStrCmp(pItem1->GetFileTitle(),pItem2->GetFileTitle());
	case InFolder:
		if (lParamSort&128)
			return pLocateDlg->m_pStrCmp(pItem2->GetParent(),pItem1->GetParent());
		return pLocateDlg->m_pStrCmp(pItem1->GetParent(),pItem2->GetParent());
	case FullPath:
		if (lParamSort&128)
			return pLocateDlg->m_pStrCmp(pItem2->GetPath(),pItem1->GetPath());
		return pLocateDlg->m_pStrCmp(pItem1->GetPath(),pItem2->GetPath());
	case Extension:
		// Sort by extension
		if (lParamSort&128)
			return pLocateDlg->m_pStrCmp(pItem2->GetExtension(),pItem1->GetExtension());
		return pLocateDlg->m_pStrCmp(pItem1->GetExtension(),pItem2->GetExtension());
	case FileSize:
		if (!(GetLocateDlg()->GetExtraFlags()&efLVNoUpdateWhileSorting))
		{
			if (pItem1->ShouldUpdateFileSize())
				pItem1->UpdateFileSize();
			if (pItem2->ShouldUpdateFileSize())
				pItem2->UpdateFileSize();
		}

		if (pItem2->IsFolder())
		{
			if (pItem1->IsFolder())
				return 0;
			return lParamSort&128?-1:1;
		}
		else if (pItem1->IsFolder())
			return lParamSort&128?1:-1;
		
		if (pItem2->GetFileSizeHi()!=pItem1->GetFileSizeHi())
		{		
			if (pItem2->GetFileSizeHi()>pItem1->GetFileSizeHi())
				return lParamSort&128?1:-1;
			else
				return lParamSort&128?-1:1;

		}
		if (pItem2->GetFileSizeLo()==pItem1->GetFileSizeLo())
			return 0;
		else if (pItem2->GetFileSizeLo()>pItem1->GetFileSizeLo())
			return lParamSort&128?1:-1;
		else
			return lParamSort&128?-1:1;
	case FileType:
		if (pItem1->ShouldUpdateType())
			pItem1->UpdateType();
		if (pItem2->ShouldUpdateType())
			pItem2->UpdateType();
		if (lParamSort&128)
			return pLocateDlg->m_pStrCmp(pItem2->GetType(),pItem1->GetType());
		return pLocateDlg->m_pStrCmp(pItem1->GetType(),pItem2->GetType());
	case DateModified:
		if (!(GetLocateDlg()->GetExtraFlags()&efLVNoUpdateWhileSorting))
		{
			if (pItem1->ShouldUpdateTimeAndDate())
				pItem1->UpdateFileTime();
			if (pItem2->ShouldUpdateTimeAndDate())
				pItem2->UpdateFileTime();
		}		
		if (pItem1->GetModifiedDate()==pItem2->GetModifiedDate())
		{
			// Same day
			if (pItem2->GetModifiedTime()==pItem1->GetModifiedTime())
				return 0;
			else if (pItem2->GetModifiedTime()>pItem1->GetModifiedTime())
				return lParamSort&128?1:-1;
			else
				return lParamSort&128?-1:1;
		}
		if (pItem2->GetModifiedDate()>pItem1->GetModifiedDate())
			return lParamSort&128?1:-1;
		else
			return lParamSort&128?-1:1;
	case DateCreated:
		if (!(GetLocateDlg()->GetExtraFlags()&efLVNoUpdateWhileSorting))
		{
			if (pItem1->ShouldUpdateTimeAndDate())
				pItem1->UpdateFileTime();
			if (pItem2->ShouldUpdateTimeAndDate())
				pItem2->UpdateFileTime();
		}
		
		if (pItem1->GetCreatedDate()==pItem2->GetCreatedDate())
		{
			// Same day
			if (pItem2->GetModifiedTime()==pItem1->GetCreatedTime())
				return 0;
			else if (pItem2->GetCreatedTime()>pItem1->GetCreatedTime())
				return lParamSort&128?1:-1;
			else
				return lParamSort&128?-1:1;
		}
		if (pItem2->GetCreatedDate()>pItem1->GetCreatedDate())
			return lParamSort&128?1:-1;
		else
			return lParamSort&128?-1:1;
	case DateAccessed:
		if (!(GetLocateDlg()->GetExtraFlags()&efLVNoUpdateWhileSorting))
		{
			if (pItem1->ShouldUpdateTimeAndDate())
				pItem1->UpdateFileTime();
			if (pItem2->ShouldUpdateTimeAndDate())
				pItem2->UpdateFileTime();
		}

		if (pItem1->GetAccessedDate()==pItem2->GetAccessedDate())
		{
			// Same day
			if (pItem2->GetAccessedTime()==pItem1->GetAccessedTime())
				return 0;
			else if (pItem2->GetAccessedTime()>pItem1->GetAccessedTime())
				return lParamSort&128?1:-1;
			else
				return lParamSort&128?-1:1;
		}
		if (pItem2->GetAccessedDate()>pItem1->GetAccessedDate())
			return lParamSort&128?1:-1;
		else
			return lParamSort&128?-1:1;
	case Attributes:
		if (pItem1->ShouldUpdateAttributes())
			pItem1->UpdateAttributes();
		if (pItem2->ShouldUpdateAttributes())
			pItem2->UpdateAttributes();
		
		if (pItem2->IsDeleted())
		{
			if (pItem1->IsDeleted())
				return 0;
			return lParamSort&128?-1:1;
		}
		else if (pItem2->IsDeleted())
			return lParamSort&128?1:-1;
		if (lParamSort&128)
			return int(pItem2->GetAttributes())-int(pItem1->GetAttributes());
		return int(pItem1->GetAttributes())-int(pItem2->GetAttributes());
	case ImageDimensions:
		if (pItem1->ShouldUpdateExtra(ImageDimensions))
			pItem1->UpdateDimensions();
		if (pItem2->ShouldUpdateExtra(ImageDimensions))
			pItem2->UpdateDimensions();
		
		if (pItem2->GetImageDimensionsProduct()==pItem1->GetImageDimensionsProduct())
			return 0;
		else if (pItem2->GetImageDimensionsProduct()>pItem1->GetImageDimensionsProduct())
			return lParamSort&128?1:-1;
		else
			return lParamSort&128?-1:1;
	case Pages:
		if (pItem1->ShouldUpdateExtra(Pages))
			pItem1->UpdateSummaryProperties();
		if (pItem2->ShouldUpdateExtra(Pages))
			pItem2->UpdateSummaryProperties();
		
		if (pItem2->GetPages()==pItem1->GetPages())
			return 0;
		else if (pItem2->GetPages()>pItem1->GetPages())
			return lParamSort&128?1:-1;
		else
			return lParamSort&128?-1:1;
	case Owner:
	case ShortFileName:	
	case ShortFilePath:
	case MD5sum:
	case Author:
	case Title:
	case Subject:
	case Category:
	case Comments:
	case Description:
	case FileVersion:
	case ProductName:
	case ProductVersion:
	case Keywords:
		{
			if (pItem1->ShouldUpdateExtra(nDetail))
				pItem1->UpdateByDetail(nDetail);
			if (pItem2->ShouldUpdateExtra(nDetail))
				pItem2->UpdateByDetail(nDetail);
	
			LPCWSTR pText1=pItem1->GetExtraText(nDetail);
			LPCWSTR pText2=pItem2->GetExtraText(nDetail);

			if (pText2==NULL)
			{
				if (pText1==NULL)
					return 0;
				return lParamSort&128?-1:0;
			}
			else if (pText1==NULL)
				return lParamSort&128?1:-1;
			
			if (lParamSort&128)
				return pLocateDlg->m_pStrCmp(pText2,pText1);
			return pLocateDlg->m_pStrCmp(pText1,pText2);
		}
	case Database:
		if (lParamSort&128)
		{
			return pLocateDlg->m_pStrCmp(GetLocateApp()->GetDatabase(pItem2->GetDatabaseID())->GetName(),
				GetLocateApp()->GetDatabase(pItem1->GetDatabaseID())->GetName());
		}
		return pLocateDlg->m_pStrCmp(GetLocateApp()->GetDatabase(pItem1->GetDatabaseID())->GetName(),
			GetLocateApp()->GetDatabase(pItem2->GetDatabaseID())->GetName());
	case DatabaseDescription:
		if (lParamSort&128)
		{
			return pLocateDlg->m_pStrCmp(GetLocateApp()->GetDatabase(pItem2->GetDatabaseID())->GetDescription(),
				GetLocateApp()->GetDatabase(pItem1->GetDatabaseID())->GetDescription());
		}
		return pLocateDlg->m_pStrCmp(GetLocateApp()->GetDatabase(pItem1->GetDatabaseID())->GetDescription(),
			GetLocateApp()->GetDatabase(pItem2->GetDatabaseID())->GetDescription());
	case DatabaseArchive:
		if (lParamSort&128)
		{
			return pLocateDlg->m_pStrCmp(GetLocateApp()->GetDatabase(pItem2->GetDatabaseID())->GetArchiveName(),
				GetLocateApp()->GetDatabase(pItem1->GetDatabaseID())->GetArchiveName());
		}
		return pLocateDlg->m_pStrCmp(GetLocateApp()->GetDatabase(pItem1->GetDatabaseID())->GetArchiveName(),
			GetLocateApp()->GetDatabase(pItem2->GetDatabaseID())->GetArchiveName());
	case VolumeLabel:
		if (lParamSort&128)
		{
			return pLocateDlg->m_pStrCmp(CLocateDlg::GetDBVolumeLabel(pItem2->GetDatabaseID(),pItem2->GetRootID()),
				CLocateDlg::GetDBVolumeLabel(pItem1->GetDatabaseID(),pItem1->GetRootID()));
		}
		return pLocateDlg->m_pStrCmp(CLocateDlg::GetDBVolumeLabel(pItem1->GetDatabaseID(),pItem1->GetRootID()),
			CLocateDlg::GetDBVolumeLabel(pItem2->GetDatabaseID(),pItem2->GetRootID()));
	case VolumeSerial:
		if (lParamSort&128)
		{
			return pLocateDlg->m_pStrCmp(CLocateDlg::GetDBVolumeSerial(pItem2->GetDatabaseID(),pItem2->GetRootID()),
				CLocateDlg::GetDBVolumeSerial(pItem1->GetDatabaseID(),pItem1->GetRootID()));
		}
		return pLocateDlg->m_pStrCmp(CLocateDlg::GetDBVolumeSerial(pItem1->GetDatabaseID(),pItem1->GetRootID()),
			CLocateDlg::GetDBVolumeSerial(pItem2->GetDatabaseID(),pItem2->GetRootID()));
	case VolumeFileSystem:
		if (lParamSort&128)
		{
			return pLocateDlg->m_pStrCmp(CLocateDlg::GetDBVolumeFileSystem(pItem2->GetDatabaseID(),pItem2->GetRootID()),
				CLocateDlg::GetDBVolumeFileSystem(pItem1->GetDatabaseID(),pItem1->GetRootID()));
		}
		return pLocateDlg->m_pStrCmp(CLocateDlg::GetDBVolumeFileSystem(pItem1->GetDatabaseID(),pItem1->GetRootID()),
			CLocateDlg::GetDBVolumeFileSystem(pItem2->GetDatabaseID(),pItem2->GetRootID()));
	}
	
	return 0;
}

int CLocateDlg::SortNewItem(CListCtrl* pList,CLocatedItem* pNewItem,BYTE bSorting)
{
	int dwMaxItems=pList->GetItemCount();
	if (dwMaxItems==0)
		return 0;

	int a=0; // start index
	int b=dwMaxItems-1; // end idex
	int c=(a+b)/2;
	for (;;)
	{
		ASSERT(c>=0 && c<int(dwMaxItems));
		CLocatedItem* pItem=(CLocatedItem*)pList->GetItemData(c);
		if (pItem==NULL || DWORD(pItem)==DWORD(-1))
		{
			::MessageBox(NULL,"CLocateDlg::SortNewItem:Something is wrong! Contact jmhuttun@venda.uku.fi",NULL,MB_OK);
			return 0;
		}

		int nRet=ListCompareProc(LPARAM(pItem),LPARAM(pNewItem),bSorting);
		if (nRet<0) // New item should be later
		{
			a=c+1;
			c=(a+b)/2;

			if (a>b)
			{
				ASSERT(a>=0 && a<=int(dwMaxItems));
				return a;
			}
		}
		else if (nRet>0) // New item should be previous
		{
			b=c;
			c=(a+b)/2;
			if (a==b)
			{
				ASSERT(a>=0 && a<=int(dwMaxItems));
				return a;
			}
		}
		else
		{
			do
			{
				if (++c>=int(dwMaxItems))
					break;
				pItem=(CLocatedItem*)pList->GetItemData(c);
			}
			while (ListCompareProc(LPARAM(pItem),LPARAM(pNewItem),bSorting)==0);
			
			ASSERT(c>=0 && c<=int(dwMaxItems));
			return c;
		}
	}
}
	
void CLocateDlg::SetSortArrowToHeader(DetailType nType,BOOL bRemove,BOOL bDownArrow)
{
	if (((CLocateApp*)GetApp())->m_wComCtrlVersion<0x0600)
		return;

	if (int(nType)>int(LastType))
		return;

	int nColumn=m_pListCtrl->GetVisibleColumn(m_pListCtrl->GetColumnFromID(nType));
	if (nColumn==-1)
		return;

	HWND hHeader=m_pListCtrl->GetHeader();
	HDITEM hi;
	hi.mask=HDI_FORMAT;
	::SendMessage(hHeader,HDM_GETITEM,nColumn,(LPARAM)&hi);
	hi.fmt&=~(0x0400|0x0200);
	if (!bRemove)
	{
		if (!bDownArrow)
			hi.fmt|=0x400;
		else
			hi.fmt|=0x200;
	}
	::SendMessage(hHeader,HDM_SETITEM,nColumn,(LPARAM)&hi);

}

void CLocateDlg::SetListCompareFunction()
{
	DWORD dwMethod=DEFAULT_SORTINGMETHOD;
	CRegKey2 LocRegKey;
	if (LocRegKey.OpenKey(HKCU,"\\Locate",CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
		LocRegKey.QueryValue("Sorting Method",dwMethod);
	
	switch (dwMethod)
	{
	case 0: // Standard ASCII
		m_pStrCmp=StrCmpIW;
		break;
	case 1: // Standard ASCII (case sensitive)
		m_pStrCmp=_StrCmpW;
		break;
	case 2: // Natural order 
		m_pStrCmp=(int (STDAPICALLTYPE*)(LPCWSTR,LPCWSTR))GetProcAddress(GetModuleHandle("Shlwapi.dll"),"StrCmpLogicalW");
		if (m_pStrCmp==NULL)
			m_pStrCmp=strnatcasecmp;
		break;
	case 3: // Natural order (case sensitive)
		m_pStrCmp=strnatcmp;
		break;
	default:
		m_pStrCmp=strnatcasecmp;
		break;
	}
}


////////////////////////////////////////////////////////////
// CLocateDlg - Results list - Drag and drop
////////////////////////////////////////////////////////////

void CLocateDlg::BeginDragFiles(CListCtrl* pList)
{
	// Initializing COM objects
	CFileObject* pfo=new CFileObject;
	CFileSource* pfs=new CFileSource;
	
	pfo->AutoDelete();
	pfo->AddRef();
	pfo->SetFiles(pList,TRUE);

	pfs->AutoDelete();
	pfs->AddRef();
	
	// Inserting selected items to array
	int i=0,nSelectedItems=pList->GetSelectedCount();
	int* pSelectedItems=new int[max(nSelectedItems,2)];
	int nItem=m_pListCtrl->GetNextItem(-1,LVNI_SELECTED);
	while (nItem!=-1 && i<nSelectedItems)
	{
		pSelectedItems[i++]=nItem;
		nItem=m_pListCtrl->GetNextItem(nItem,LVNI_SELECTED);
	}
	ASSERT(nSelectedItems==i); // If didn't get all selected items
    
	

	DWORD nEffect;
	AddExtraFlags(efLVIgnoreListClicks);
	DebugMessage("DND started");
	HRESULT hRes=DoDragDrop(pfo,pfs,DROPEFFECT_COPY|DROPEFFECT_MOVE|DROPEFFECT_LINK|DROPEFFECT_SCROLL,&nEffect);
	DebugMessage("DND succeeded");
	PostMessage(WM_REMOVEIGNORECLICKSFLAG);
	
	pfo->Release();
	pfs->Release();

	for (int i=0;i<nSelectedItems;i++)
	{
		CLocatedItem* pItem=(CLocatedItem*)m_pListCtrl->GetItemData(pSelectedItems[i]);
		
		// Just disabling flags, let background thread do the rest
		if (pItem->RemoveFlagsForChanged())
			m_pListCtrl->RedrawItems(pSelectedItems[i],pSelectedItems[i]);

	}
	
	delete[] pSelectedItems;
}


////////////////////////////////////////////////////////////
// CLocateDlg - Results list - Results list actions
////////////////////////////////////////////////////////////

/* CLocateDlg::LoadResultlistActions and CLocateDlg::SaveResultlistActions:
Format in registry: all non "zero" (do something) actions are stored 
one after another in the following format

1. column index (DetailType) as DWORD value 
2. action (CLocateDlg::ResultListAction) as DWORD value 
3. data given by CSubAction::GetData and CSubAction::FromData

*/

void CLocateDlg::LoadResultlistActions()
{
	// Going to set default actions
	ClearResultlistActions();

	CRegKey2 RegKey;
	if (RegKey.OpenKey(HKCU,"\\General",CRegKey::defRead)==ERROR_SUCCESS)
	{
		BOOL bOk;
		DWORD dwLength=RegKey.QueryValueLength("ResultListActions",bOk);
		if (bOk)
		{
			//DebugFormatMessage("CLocateDlg::LoadResultlistActions(): ResultListActions length: %d",dwLength);

			if (dwLength==0)
				return;


			BYTE *pData=new BYTE[dwLength];
			if (RegKey.QueryValue("ResultListActions",LPSTR(pData),dwLength)==dwLength)
			{
				// Check all actions
                BYTE* pPtr=pData;
				while (dwLength>0)
				{
					// Stop if action is not suitable
					if (*((WORD*)pPtr)>LastType || *((WORD*)pPtr+1)>=ListActionCount)
						break;

					DWORD dwUsed;
					CSubAction* pSubAction=CSubAction::FromData(CAction::ResultListItems,pPtr+2*sizeof(WORD),dwLength-2*sizeof(WORD),dwUsed);
					if (pSubAction==NULL)
						break;

					m_aResultListActions[*((WORD*)pPtr)][*((WORD*)pPtr+1)]=pSubAction;

					pPtr+=2*sizeof(WORD)+dwUsed;
					dwLength-=2*sizeof(WORD)+dwUsed;
                    
				}

				delete[] pData;

				// If dwLength==0, all actions was OK
				if (dwLength==0)
					return;

				
				// Going to set default actions
				ClearResultlistActions();

			}
			else
				delete[] pData;

			
		}
	}

	// Set default actions, 2D array is converted to 
	// 1D array for SetDefaultActions
	CSubAction*** ppActions=new CSubAction**[TypeCount];
	for (int i=0;i<TypeCount;i++)
		ppActions[i]=m_aResultListActions[i];
	SetDefaultActions(ppActions);
	delete[] ppActions;
}

void CLocateDlg::SaveResultlistActions()
{
	// Counting length of data
	DWORD dwLength=0;
	for (int iCol=0;iCol<TypeCount;iCol++)
	{
		for (int iAct=0;iAct<ListActionCount;iAct++)
		{
			if (m_aResultListActions[iCol][iAct]!=NULL)
				dwLength+=2*sizeof(WORD)+m_aResultListActions[iCol][iAct]->GetDataLength(CAction::ResultListItems);
		}
	}

	// Constuct data
	BYTE* pData=new BYTE[dwLength];
	BYTE* pPtr=pData;
	for (int iCol=0;iCol<TypeCount;iCol++)
	{
		for (int iAct=0;iAct<ListActionCount;iAct++)
		{
			if (m_aResultListActions[iCol][iAct]!=NULL)
			{
				*((WORD*)pPtr)=iCol;
				*((WORD*)pPtr+1)=iAct;
                pPtr+=2*sizeof(WORD);

				DWORD dwUsed=m_aResultListActions[iCol][iAct]->GetData(CAction::ResultListItems,pPtr,TRUE);
                
				pPtr+=dwUsed;
			}
		}
	}

	ASSERT(DWORD(pPtr-pData)==dwLength);

	//DebugFormatMessage("CLocateDlg::SaveResultlistActions(): ResultListActions length: %d",dwLength);

	// Save to registry
	CRegKey2 RegKey;
	if (RegKey.OpenKey(HKCU,"\\General",CRegKey::defWrite)==ERROR_SUCCESS)
	{
		
		RegKey.SetValue("ResultListActions",LPCSTR(pData),dwLength,REG_BINARY);
		RegKey.CloseKey();
	}	

	delete[] pData;

}

BOOL CLocateDlg::OnExecuteResultAction(CAction::ActionResultList m_nResultAction,void* pExtraInfo,int nItem,DetailType nDetail)
{
	DebugFormatMessage("CLocateDlg::OnExecuteResultAction(%d,%X,%d,%d)",m_nResultAction,pExtraInfo,nItem,nDetail);
	switch (m_nResultAction)
	{
	case CAction::Execute:
		return OnExecuteFile((LPCWSTR)pExtraInfo,nItem);
	case CAction::Copy:
		return OnCopy(FALSE,nItem);
	case CAction::Cut:
		return OnCopy(TRUE,nItem);
	case CAction::MoveToRecybleBin:
		return OnDelete(Recycle,nItem);
	case CAction::DeleteFile:
		return OnDelete(Delete,nItem);
	case CAction::OpenContextMenu:
	case CAction::OpenContextMenuSimple:
		{
			ClearMenuVariables();

			int nSelectedItems;
			MakeItemSelected(nItem);
			CAutoPtrA<CLocatedItem*> pSelectedItems=GetSelectedItems(nSelectedItems);
			if (!CreateFileContextMenu(NULL,pSelectedItems,nSelectedItems,
				m_nResultAction==CAction::OpenContextMenuSimple,int(pExtraInfo)==CAction::Parent))
				return FALSE;

			POINT pos;
			GetCursorPos(&pos);
			int nCmd=TrackPopupMenu(*m_pActiveContextMenu,TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD,
				pos.x,pos.y,0,*this,NULL);	

			BOOL bRet=FALSE;
			if (nCmd>0)
				bRet=HandleContextMenuCommand(nCmd);

			ClearMenuVariables();
			return bRet;
		}
	case CAction::OpenFolder:
		return OpenSelectedFolder(FALSE,nItem,FALSE);
	case CAction::OpenContainingFolder:
		return OpenSelectedFolder(TRUE,nItem,FALSE);
	case CAction::Properties:
		return OnProperties(nItem);
	case CAction::ShowSpecialMenu:
		if (m_pListCtrl->GetSelectedCount()==0)
			m_pListCtrl->SetItemState(nItem,LVIS_SELECTED,LVIS_SELECTED);

		if (pExtraInfo!=NULL)
		{
			TrackPopupMenu(::GetSubMenu(m_Menu.GetSubMenu(SUBMENU_EXTRACONTEXTMENUITEMS),SUBMENU_SPECIALMENU),TPM_LEFTALIGN|TPM_RIGHTBUTTON,
				((POINT*)pExtraInfo)->x,((POINT*)pExtraInfo)->y,0,*this,NULL);	
		}
		else
		{
			POINT pos;
			GetCursorPos(&pos);
			TrackPopupMenu(::GetSubMenu(m_Menu.GetSubMenu(SUBMENU_EXTRACONTEXTMENUITEMS),SUBMENU_SPECIALMENU),TPM_LEFTALIGN|TPM_RIGHTBUTTON,
				pos.x,pos.y,0,*this,NULL);	
		}
		return TRUE;
	case CAction::ExecuteCommand:
		return ExecuteCommand((LPCWSTR)pExtraInfo,nItem);
	case CAction::SelectFile:
		{
			int nItem,nSelectedItem=m_pListCtrl->GetNextItem(-1,LVNI_SELECTED);
			
			switch ((CAction::SelectFileType)(DWORD)pExtraInfo)
			{
			case CAction::NextFile:
				if ((m_pListCtrl->GetStyle()&LVS_TYPEMASK)!=LVS_REPORT)
				{
					nItem=m_pListCtrl->GetNextItem(nSelectedItem,LVNI_TORIGHT);
					if (nItem==nSelectedItem)
						nItem=m_pListCtrl->GetNextItem(nSelectedItem,LVNI_BELOW);
				}
				else
					nItem=m_pListCtrl->GetNextItem(nSelectedItem,LVNI_BELOW);
				break;
			case CAction::PrevFile:
				if ((m_pListCtrl->GetStyle()&LVS_TYPEMASK)!=LVS_REPORT)
				{
					nItem=m_pListCtrl->GetNextItem(nSelectedItem,LVNI_TOLEFT);
					if (nItem==nSelectedItem)
						nItem=m_pListCtrl->GetNextItem(nSelectedItem,LVNI_ABOVE);
				}
				else
					nItem=m_pListCtrl->GetNextItem(nSelectedItem,LVNI_ABOVE);
				break;
			case CAction::NextNonDeletedFile:
				nItem=nSelectedItem;
				for (;;)
				{
					// Next item
					if ((m_pListCtrl->GetStyle()&LVS_TYPEMASK)!=LVS_REPORT)
					{
						nItem=m_pListCtrl->GetNextItem(nItem,LVNI_TORIGHT);
						if (nItem==nSelectedItem)
							nItem=m_pListCtrl->GetNextItem(nItem,LVNI_BELOW);
					}
					else
						nItem=m_pListCtrl->GetNextItem(nItem,LVNI_BELOW);

					if (nItem==-1|| nSelectedItem==nItem)
						return FALSE;

					CLocatedItem* pItem=(CLocatedItem*)m_pListCtrl->GetItemData(nItem);
					if (pItem!=NULL)
					{
						if (!pItem->IsDeleted())
							break;
                   	}                    								
				}
				break;
			case CAction::PrevNonDeletedFile:
				nItem=nSelectedItem;
				for (;;)
				{
					// Next item
					if ((m_pListCtrl->GetStyle()&LVS_TYPEMASK)!=LVS_REPORT)
					{
						nItem=m_pListCtrl->GetNextItem(nItem,LVNI_TOLEFT);
						if (nItem==nSelectedItem)
							nItem=m_pListCtrl->GetNextItem(nItem,LVNI_ABOVE);
					}
					else
						nItem=m_pListCtrl->GetNextItem(nItem,LVNI_ABOVE);

					if (nItem==-1|| nSelectedItem==nItem)
						return FALSE;

					CLocatedItem* pItem=(CLocatedItem*)m_pListCtrl->GetItemData(nItem);
					if (pItem!=NULL)
					{
						if (!pItem->IsDeleted())
							break;
                   	}                    								
				}
				break;
			}

			if (nItem==-1 || nSelectedItem==nItem)
				return FALSE;
			
			if (nSelectedItem!=-1)
				m_pListCtrl->SetItemState(nSelectedItem,0,LVIS_SELECTED|LVIS_FOCUSED);
			m_pListCtrl->SetItemState(nItem,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
			m_pListCtrl->EnsureVisible(nItem,FALSE);
			return TRUE;
		} 
	case CAction::RenameFile:
		return OnRenameFile(nItem);
	case CAction::SelectNthFile:
	case CAction::ExecuteNthFile:
		{
			// Unselect other items
			int nItem=m_pListCtrl->GetNextItem(-1,LVNI_SELECTED);
			while (nItem!=-1)
			{
				if (nItem!=(int)pExtraInfo)					
					m_pListCtrl->SetItemState(nItem,0,LVIS_SELECTED|LVIS_FOCUSED);
				nItem=m_pListCtrl->GetNextItem(nItem,LVNI_SELECTED);
			}

			// Selcet item
			m_pListCtrl->SetItemState((int)pExtraInfo,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
			m_pListCtrl->EnsureVisible((int)pExtraInfo,FALSE);
			
			
			// Execute item
			if (m_nResultAction==CAction::ExecuteNthFile)
				return OnExecuteFile(NULL,(int)pExtraInfo);

			return TRUE;
		}
	}
	return TRUE;
}

BOOL CLocateDlg::OnExecuteFile(LPCWSTR szVerb,int nItem)
{
	CWaitCursor wait;

	int nSelectedItems;
	CAutoPtrA<CLocatedItem*> pItems=GetSelectedItems(nSelectedItems,nItem);
	
	for (int i=0;i<nSelectedItems;i++)
	{
		if (pItems[i]==NULL)
			continue;

		if (!pItems[i]->IsFolder() && !FileSystem::IsFile(pItems[i]->GetPath()))
			continue;
		if (pItems[i]->IsFolder() && !FileSystem::IsDirectory(pItems[i]->GetPath()))
			continue;
		
		if (pItems[i]->IsFolder())
			OpenFolder(pItems[i]->GetPath());
		else 
		{
			BOOL bOnlyNULLAsVerb=FALSE;
			if ((GetExtraFlags()&efExecuteModeMask)==efExecuteModeDefaultMode)
			{
				// Default mode, use ShellExecute
				if (ShellFunctions::ShellExecute(*this,szVerb,pItems[i]->GetPath(),NULL,pItems[i]->GetParentSafe(),SW_SHOW)>32)
				{
					// Succeeded, next item...
					continue;
				}

				// If ShellExecute didn't success, using InvokeCommand
				// this usually brings a window which ask that in which 
				// application the document is opened
				bOnlyNULLAsVerb=TRUE;
			}

			ContextMenuInformation ci;
			if (!GetContextMenuForItems(&ci,1,&pItems[i]))
			{
				// Error, use ShellExecute anyway
				ShellFunctions::ShellExecute(*this,szVerb,pItems[i]->GetPath(),NULL,pItems[i]->GetParentSafe(),SW_SHOW);
				continue;
			}

			// Populate context menu
			CMINVOKECOMMANDINFOEX cii;
			ZeroMemory(&cii,sizeof(CMINVOKECOMMANDINFOEX));
			cii.cbSize=sizeof(CMINVOKECOMMANDINFOEX);
			cii.fMask=CMIC_MASK_UNICODE;
			cii.hwnd=*this;
			cii.nShow=SW_SHOWDEFAULT;
			
			CMenu Menu;
			Menu.CreatePopupMenu();
			
			if (!SUCCEEDED(ci.pContextMenu->QueryContextMenu(Menu,0,IDM_DEFCONTEXTITEM,IDM_DEFSENDTOITEM,CMF_DEFAULTONLY|CMF_VERBSONLY)))
			{
				// Error, free allocated data and use ShellExecute 
				ShellFunctions::ShellExecute(*this,szVerb,pItems[i]->GetPath(),NULL,pItems[i]->GetParentSafe(),SW_SHOW);
				continue;
			}

			if (szVerb==NULL || bOnlyNULLAsVerb)
			{
				// Try to retvieve default verb
				BOOL bFound=FALSE;
				
				if (!bOnlyNULLAsVerb)
				{
					MENUITEMINFO mii;
					mii.cbSize=sizeof(MENUITEMINFO);
					mii.fMask=MIIM_ID|MIIM_STATE;
					for (int iItem=0;Menu.GetMenuItemInfo(iItem,TRUE,&mii);iItem++)
					{
						if (mii.fState&MFS_DEFAULT)
						{
							cii.lpVerb=(LPCSTR)MAKELONG(mii.wID-IDM_DEFCONTEXTITEM,0);
							cii.lpVerbW=(LPCWSTR)MAKELONG(mii.wID-IDM_DEFCONTEXTITEM,0);
							bFound=TRUE;
							break;
						}
					}
				}

				if (bFound)
				{
					if (!SUCCEEDED(ci.pContextMenu->InvokeCommand((CMINVOKECOMMANDINFO*)&cii)) && cii.lpVerb!=NULL)
					{
						// Try execute default
						cii.lpVerbW=NULL;
						cii.lpVerb=NULL;
						ci.pContextMenu->InvokeCommand((CMINVOKECOMMANDINFO*)&cii);
					}
				}
				else
				{
					// No default item, execute with lpVerb=NULL and, if it fails, use ShellExecute
					cii.lpVerbW=NULL;
					cii.lpVerb=NULL;
					if (!SUCCEEDED(ci.pContextMenu->InvokeCommand((CMINVOKECOMMANDINFO*)&cii)))
						ShellFunctions::ShellExecute(*this,NULL,pItems[i]->GetPath(),NULL,pItems[i]->GetParentSafe(),SW_SHOW);
				}
			}
			else
			{
				// Verb given, using it
				cii.lpVerbW=szVerb;
				cii.lpVerb=alloccopyWtoA(szVerb);
				if (!SUCCEEDED(ci.pContextMenu->InvokeCommand((CMINVOKECOMMANDINFO*)&cii)))
					ShellFunctions::ShellExecute(*this,szVerb,pItems[i]->GetPath(),NULL,pItems[i]->GetParentSafe(),SW_SHOW);
				delete[] (LPSTR)cii.lpVerb;
			}
				
		}

	}

	return TRUE;
}




////////////////////////////////////////////////////////////
// CLocateDlg - Results list - Helpers
////////////////////////////////////////////////////////////


BOOL CLocateDlg::OpenFolder(LPCWSTR szFolder,LPCWSTR szSelectedFile)
{
	CStringW sProgram;
	
	// Loading some general settings
	CRegKey2 RegKey;
	if (RegKey.OpenKey(HKCU,"\\General",CRegKey::defRead)==ERROR_SUCCESS)
	{
		DWORD dwTemp=0;
		RegKey.QueryValue("Use other program to open folders",dwTemp);
		if (dwTemp)	
			RegKey.QueryValue(L"Open folders with",sProgram);
	}
	
	if (sProgram.IsEmpty())
	{
		SHELLEXECUTEINFOW sxi;
		sxi.cbSize=sizeof(SHELLEXECUTEINFO);
		sxi.fMask=SEE_MASK_NOCLOSEPROCESS|SEE_MASK_IDLIST;
		sxi.hwnd=*this;
		sxi.nShow=SW_SHOWNORMAL;
		sxi.lpParameters=szwEmpty;
		sxi.lpDirectory=szwEmpty;
		sxi.lpFile=NULL;
		sxi.lpIDList=ShellFunctions::GetIDList(szFolder);
		sxi.lpVerb=L"open";
		
		BOOL bRet=ShellFunctions::ShellExecuteEx(&sxi);	
		CoTaskMemFree(sxi.lpIDList);
		return bRet;

	}
	else
	{
		CStringW sTemp;
		int nIndex=sProgram.FindFirst('%');
		int nAdded=0;
        while (nIndex!=-1)
		{
			sTemp.Append(sProgram,nIndex);
			if (sProgram[nIndex+1]=='d')
			{
				sTemp<< szFolder;
				nIndex+=2;
			}
			else if (sProgram[nIndex+1]=='f')
			{
				if (szSelectedFile!=NULL)
					sTemp<< szSelectedFile;
				else
					sTemp<< szFolder;
				nIndex+=2;
			}
			else 
			{
				sTemp<<'%';
				nIndex++;
			}
			nAdded=nIndex;
			nIndex=sProgram.FindNext('%',nIndex);
		}
		sTemp<<(LPCWSTR(sProgram)+nAdded);

		PROCESS_INFORMATION pi;
		STARTUPINFO si;
		si.cb=sizeof(STARTUPINFO);
		si.lpReserved=NULL;
		si.cbReserved2=0;
		si.lpReserved2=NULL;
		si.lpDesktop=NULL;
		si.lpTitle=NULL;
		si.dwFlags=STARTF_USESHOWWINDOW;
		si.wShowWindow=SW_SHOWDEFAULT;
		
		if (IsUnicodeSystem())
		{
			if (!CreateProcessW(NULL,sTemp.GetBuffer(),NULL,
				NULL,FALSE,CREATE_DEFAULT_ERROR_MODE|NORMAL_PRIORITY_CLASS,
				NULL,NULL,(STARTUPINFOW*)&si,&pi))
				return FALSE;
		}
		else
		{
			if (!CreateProcessA(NULL,(LPSTR)(LPCSTR)W2A(sTemp.GetBuffer()),NULL,
				NULL,FALSE,CREATE_DEFAULT_ERROR_MODE|NORMAL_PRIORITY_CLASS,
				NULL,NULL,&si,&pi))
				return FALSE;
		}

		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}
	return TRUE;
}


BOOL CLocateDlg::OpenSelectedFolder(BOOL bContaining,int nItem,BOOL bForParents)
{
	CWaitCursor wait;

	DebugMessage("CLocateDlg::OpenSelectedFolder");

	int nSelectedItems;
	CAutoPtrA<CLocatedItem*> pItems=GetSelectedItems(nSelectedItems,nItem);

	if (nSelectedItems==0 || pItems==NULL)
		return FALSE;
	
	DebugFormatMessage("nSelectedItems=%d bContaining=%d nItem=%d bForParents=%d",
		nSelectedItems,bContaining,nItem,bForParents);
	
	if (bContaining)
	{
		// Open containing folders
				
		// Collect folders
		struct FolderInfo{
			CStringW sFolder;
			CArrayFAP<LPCWSTR> aItems;
			FolderInfo(LPCWSTR szFolder) { sFolder=szFolder;  }
		};
		CArrayFP<FolderInfo*> aFolders;

		if (bForParents)
		{
			for (int i=0;i<nSelectedItems;i++)
			{
				int j;
				int nParentLen=LastCharIndex(pItems[i]->GetParent(),L'\\');
				if (nParentLen==-1)
					continue;

				CAutoPtrA<WCHAR> sParent=alloccopy(pItems[i]->GetParent(),nParentLen);
				DebugFormatMessage("Parent is %S",(LPCWSTR)sParent);


				for (j=0;j<aFolders.GetSize();j++)
				{
					if (aFolders[j]->sFolder.Compare(sParent)==0)
					{
						int k;
						for (k=0;k<aFolders[j]->aItems.GetSize();k++)
						{
							if (wcscmp(aFolders[j]->aItems[k],pItems[i]->GetParent()+nParentLen+1)==0)
								break;
						}
						if (k==aFolders[j]->aItems.GetSize())
						{
							DebugFormatMessage("Adding directory %S for existing parent",pItems[i]->GetParent()+nParentLen+1);
							aFolders[j]->aItems.Add(alloccopy(pItems[i]->GetParent()+nParentLen+1));
						}
						break;
					}
				}

				if (j==aFolders.GetSize())
				{
					DebugFormatMessage("New parent for directory %S",pItems[i]->GetParent()+nParentLen+1);
					aFolders.Add(new FolderInfo(sParent));
					aFolders.GetLast()->aItems.Add(alloccopy(pItems[i]->GetParent()+nParentLen+1));
				}
			}
		}
		else
		{
			for (int i=0;i<nSelectedItems;i++)
			{
				int j;
				for (j=0;j<aFolders.GetSize();j++)
				{
					if (aFolders[j]->sFolder.Compare(pItems[i]->GetParent())==0)
					{
						DebugFormatMessage("New dir for existing item: %S",pItems[i]->GetName());
						aFolders[j]->aItems.Add(alloccopy(pItems[i]->GetName()));
						break;
					}
				}

				if (j==aFolders.GetSize())
				{
					DebugFormatMessage("New parent/dir pair %S / %S",(LPCWSTR)pItems[i]->GetParentSafe(),pItems[i]->GetName());
					aFolders.Add(new FolderInfo(pItems[i]->GetParent()));
					aFolders.GetLast()->aItems.Add(alloccopy(pItems[i]->GetName()));
				}
			}
		}

		// Loading some general settings
		CRegKey2 RegKey;
		if (RegKey.OpenKey(HKCU,"\\General",CRegKey::defRead)==ERROR_SUCCESS)
		{	
			DWORD dwTemp=0;
			RegKey.QueryValue("Use other program to open folders",dwTemp);
			RegKey.CloseKey();

			if (!dwTemp)	
			{
				DebugMessage("Trying SHOpenFolderAndItems");

				// No program specified, using explorer
				HRESULT(STDAPICALLTYPE * pSHOpenFolderAndSelectItems)(LPCITEMIDLIST,UINT,LPCITEMIDLIST*,DWORD)=
					(HRESULT(STDAPICALLTYPE *)(LPCITEMIDLIST,UINT,LPCITEMIDLIST*,DWORD))GetProcAddress(GetModuleHandle("shell32.dll"),"SHOpenFolderAndSelectItems");
				
				if (pSHOpenFolderAndSelectItems!=NULL && m_pDesktopFolder!=NULL)
				{
					// Initializing pDesktopFolder
					IShellFolder* pParentFolder;
					LPITEMIDLIST pParentIDList;
					HRESULT hRes;
					BOOL bAllOK=TRUE;

					
								
					// Open folders
					for (int i=0;i<aFolders.GetSize();i++)
					{
						CStringW sFolder(aFolders[i]->sFolder);


						if (sFolder[1]==':' && sFolder[2]=='\0')
							sFolder << L'\\';
	
						// Getting ID list of parent
						hRes=m_pDesktopFolder->ParseDisplayName(*this,NULL,(LPOLESTR)(LPCWSTR)sFolder,NULL,&pParentIDList,NULL);
						if (!SUCCEEDED(hRes))
							continue;
						
						// Querying IShellFolder interface for parent
						hRes=m_pDesktopFolder->BindToObject(pParentIDList,NULL,IID_IShellFolder,(void**)&pParentFolder);
						if (!SUCCEEDED(hRes))
						{
							CoTaskMemFree(pParentIDList);
							continue;
						}
						

                        // Querying id lists for files
						LPCITEMIDLIST* pItemPids=new LPCITEMIDLIST[aFolders[i]->aItems.GetSize()];
						int nPids=0;
                        for (int j=0;j<aFolders[i]->aItems.GetSize();j++)
						{
							hRes=pParentFolder->ParseDisplayName(*this,NULL,(LPOLESTR)aFolders[i]->aItems[j],NULL,(LPITEMIDLIST*)&pItemPids[nPids],NULL);
							if (SUCCEEDED(hRes))
								nPids++;
						}


						// Opening folder and selecting items
						if (nPids>0)
							hRes=pSHOpenFolderAndSelectItems(pParentIDList,nPids,pItemPids,0);
						else 
						{
							if (aFolders[i]->aItems.GetSize()>0) 
								OpenFolder(sFolder); // All files deleted, just open directory
							hRes=S_OK;
						}
						
						// Free 
						for (int j=0;j<nPids;j++)
							CoTaskMemFree((void*)pItemPids[j]);

						CoTaskMemFree(pParentIDList);

						pParentFolder->Release();


						if (!SUCCEEDED(hRes))
						{
							bAllOK=FALSE;
							break;
						}
					}

					if (bAllOK)
						return TRUE;
				}
				
				CStringW sArg;
				SHELLEXECUTEINFOW sxi;
				sxi.cbSize=sizeof(SHELLEXECUTEINFOW);
				sxi.fMask=SEE_MASK_NOCLOSEPROCESS;
				sxi.hwnd=*this;
				sxi.lpVerb=L"open";
				sxi.lpFile=L"explorer.exe";
				sxi.lpDirectory=szwEmpty;
				sxi.nShow=SW_SHOWNORMAL;
						
				for (int i=0;i<aFolders.GetSize();i++)
				{	
					if (aFolders[i]->aItems.GetSize()==0)
						OpenFolder(aFolders[i]->sFolder);
					else
					{
						sArg.Format(L"/e,/select,\"%s\\%s\"",(LPCWSTR)aFolders[i]->sFolder,aFolders[i]->aItems[0]);
						sxi.lpParameters=sArg;
						ShellFunctions::ShellExecuteEx(&sxi);
					}
				}
				
				return TRUE;
			}
			
		}

        // Retrieving folders
		for (int i=0;i<aFolders.GetSize();i++)
		{
			DebugMessage("Calling OpenFolder...");
				
			if (aFolders[i]->aItems.GetSize()==0)
			{
				DebugFormatMessage("OpenFolder for %S",(LPCWSTR)aFolders[i]->sFolder);
				OpenFolder(aFolders[i]->sFolder);
			}
			else
			{
				DebugMessage("Multiple directories");
				for (int j=0;j<aFolders[i]->aItems.GetSize();j++)
				{
					DebugFormatMessage("OpenFolder for %S \\ %S",(LPCWSTR)aFolders[i]->sFolder,(LPCWSTR)aFolders[i]->aItems[j]);
					OpenFolder(aFolders[i]->sFolder,aFolders[i]->sFolder+L'\\'+aFolders[i]->aItems[j]);
				}
			}

			DebugMessage("... done");
			
		}
	}
	else
	{
		// Open folders
		for (int i=0;i<nSelectedItems;i++)
		{
			if (pItems[i]!=NULL)
				OpenFolder(bForParents?pItems[i]->GetParent():pItems[i]->GetPath());
		}
	}

	return TRUE;

}

CLocatedItem** CLocateDlg::GetSelectedItems(int& nItems,int nIncludeIfNoneSelected)
{
	nItems=m_pListCtrl->GetSelectedCount();

	if (nItems==0)
	{
		CLocatedItem** pRet=new CLocatedItem*[nItems=1];
		if (nIncludeIfNoneSelected!=-1)
		{	
			pRet[0]=(CLocatedItem*)m_pListCtrl->GetItemData(nIncludeIfNoneSelected);
			nItems=pRet[0]==NULL?0:1;
			return pRet;
		}

		nItems=0;
		pRet[0]=NULL;
		return pRet;
	}

	CLocatedItem** pRet=new CLocatedItem*[nItems];

	int iItem=m_pListCtrl->GetNextItem(-1,LVNI_SELECTED);
	for (int i=0;i<nItems;i++)
	{
		pRet[i]=(CLocatedItem*)m_pListCtrl->GetItemData(iItem);
		iItem=m_pListCtrl->GetNextItem(iItem,LVNI_SELECTED);
	}   

	return pRet;
}

void CLocateDlg::MakeItemSelected(int nSelectItem)
{
	if (m_pListCtrl->GetItemState(nSelectItem,LVNI_SELECTED)&LVNI_SELECTED)
	{
		// Item is already selected, all OK
		return;
	}

	// Item is not selected, deselect all other items and selected this
	int nItem;
	while ((nItem=m_pListCtrl->GetNextItem(-1,LVNI_SELECTED))!=-1)
		m_pListCtrl->SetItemState(nItem,0,LVNI_SELECTED|LVNI_FOCUSED);

	m_pListCtrl->SetItemState(nSelectItem,LVNI_SELECTED|LVNI_FOCUSED,LVNI_SELECTED|LVNI_FOCUSED);
}


void CLocateDlg::SetSystemImageLists(CListCtrl* pList,HICON* phIcon)
{
	SHFILEINFO fi;
	HIMAGELIST hList=(HIMAGELIST)SHGetFileInfo(szEmpty,0,&fi,sizeof(SHFILEINFO),SHGFI_SYSICONINDEX);
	pList->SetImageList(hList,LVSIL_NORMAL);
	hList=(HIMAGELIST)SHGetFileInfo(szEmpty,0,&fi,sizeof(SHFILEINFO),SHGFI_SMALLICON|SHGFI_SYSICONINDEX);
	pList->SetImageList(hList,LVSIL_SMALL);
	if (phIcon!=NULL)
		*phIcon=ImageList_ExtractIcon(NULL,hList,DEF_IMAGE);
}

BOOL CLocateDlg::LoadSystemImageList(ImageListSize& iImageList,SIZE& rIconSize)
{
	HRESULT (STDAPICALLTYPE* pSHGetImageList)(int,REFIID riid,void **ppv)=
		(HRESULT (STDAPICALLTYPE*)(int,REFIID riid,void **ppv))GetProcAddress(GetModuleHandle("shell32.dll"),"SHGetImageList");

	
	if (iImageList==ilMedium || pSHGetImageList==0)
	{
		m_dwThumbnailFlags&=~tfSystemImageListIsInterface;
		iImageList=ilMedium;
		SHFILEINFO fi;
		m_hSystemImageList=(HIMAGELIST)ShellFunctions::GetFileInfo("",0,&fi,SHGFI_LARGEICON|SHGFI_SYSICONINDEX);
		if (m_hSystemImageList==NULL)
			return FALSE;
		
		HICON hIconForSize=ImageList_ExtractIcon(GetInstanceHandle(),m_hSystemImageList,fi.iIcon);
		if (hIconForSize!=NULL)
		{
			ICONINFO ii;
			if (GetIconInfo(hIconForSize,&ii))
			{
				BITMAP bi;
				GetObject(ii.hbmColor,sizeof(BITMAP),&bi);

				rIconSize.cx=bi.bmWidth;
				rIconSize.cy=bi.bmHeight;

				DeleteObject(ii.hbmColor);
				DeleteObject(ii.hbmMask);
			}

			DestroyIcon(hIconForSize);
		}
		else
		{
			rIconSize.cx=GetSystemMetrics(SM_CXICON);
			rIconSize.cy=GetSystemMetrics(SM_CYICON);
		}
		return TRUE;
	}
	

	m_dwThumbnailFlags|=tfSystemImageListIsInterface;
	if (iImageList==ilExtraLarge)
	{
		if (SUCCEEDED(pSHGetImageList(SHIL_JUMBO,IID_IImageList,(void**)&m_pSystemImageList)))
		{
			ASSERT(sizeof(int)==sizeof(LONG));
			m_pSystemImageList->GetIconSize((int*)&rIconSize.cx,(int*)&rIconSize.cy);
			return TRUE;
		}

		iImageList=ilLarge;	
	}

	if (SUCCEEDED(pSHGetImageList(SHIL_EXTRALARGE,IID_IImageList,(void**)&m_pSystemImageList)))
	{
		ASSERT(sizeof(int)==sizeof(LONG));
		m_pSystemImageList->GetIconSize((int*)&rIconSize.cx,(int*)&rIconSize.cy);
		return TRUE;
	}
	

	iImageList=ilMedium;
	return LoadSystemImageList(iImageList,rIconSize);
}


CLocateDlg::ViewDetails* CLocateDlg::GetDefaultDetails()
{
	ViewDetails aDetails[]={
		{/*Name,*/IDS_LISTNAME,TRUE,LVCFMT_LEFT,200},
		{/*InFolder,*/IDS_LISTINFOLDER,TRUE,LVCFMT_LEFT,300},
		{/*FullPath,*/IDS_LISTFULLPATH,FALSE,LVCFMT_LEFT,300},
		{/*ShortFileName,*/IDS_LISTSHORTFILENAME,FALSE,LVCFMT_LEFT,200},
		{/*ShortFilePath,*/IDS_LISTSHORTFILEPATH,FALSE,LVCFMT_LEFT,300},
		{/*FileSize,*/IDS_LISTSIZE,TRUE,LVCFMT_RIGHT,70},
		{/*FileType,*/IDS_LISTTYPE,TRUE,LVCFMT_LEFT,130},
		{/*DateModified,*/IDS_LISTMODIFIED,TRUE,LVCFMT_LEFT,100},
		{/*DateCreated,*/IDS_LISTCREATED,FALSE,LVCFMT_LEFT,100},
		{/*DateAccessed,*/IDS_LISTACCESSED,FALSE,LVCFMT_LEFT,100},
		{/*Attributes,*/IDS_LISTATTRIBUTES,FALSE,LVCFMT_LEFT,70},
		{/*ImageDimensions,*/IDS_LISTIMAGEINFO,FALSE,LVCFMT_LEFT,150},
		{/*Owner,*/IDS_LISTOWNER,FALSE,LVCFMT_LEFT,130},
		{/*Database,*/IDS_LISTDATABASE,FALSE,LVCFMT_LEFT,70},
		{/*DatabaseDescription,*/IDS_LISTDATABASEDESC,FALSE,LVCFMT_LEFT,150},
		{/*DatabaseArchive,*/IDS_LISTDATABASEFILE,FALSE,LVCFMT_LEFT,150},
		{/*VolumeLabel,*/IDS_LISTVOLUMELABEL,FALSE,LVCFMT_LEFT,100},
		{/*VolumeSerial,*/IDS_LISTVOLUMESERIAL,FALSE,LVCFMT_LEFT,90},
		{/*VOlumeFileSystem,*/IDS_LISTVOLUMEFILESYSTEM,FALSE,LVCFMT_LEFT,90},
		{/*MD5sum,*/IDS_LISTMD5SUM,FALSE,LVCFMT_LEFT,100},
		{/*Author,*/IDS_LISTAUTHOR,FALSE,LVCFMT_LEFT,100},
		{/*Title,*/IDS_LISTTITLE,FALSE,LVCFMT_LEFT,100},
		{/*Subject,*/IDS_LISTSUBJECT,FALSE,LVCFMT_LEFT,100},
		{/*Category,*/IDS_LISTCATEGORY,FALSE,LVCFMT_LEFT,100},
		{/*Pages,*/IDS_LISTPAGES,FALSE,LVCFMT_LEFT,100},
		{/*Comments,*/IDS_LISTCOMMENTS,FALSE,LVCFMT_LEFT,100},
		{/*Description,*/IDS_LISTDESCRIPTION,FALSE,LVCFMT_LEFT,100},
		{/*FileVersion,*/IDS_LISTFILEVERSION,FALSE,LVCFMT_LEFT,100},
		{/*ProductName,*/IDS_LISTPRODUCTNAME,FALSE,LVCFMT_LEFT,100},
		{/*ProductVersion,*/IDS_LISTPRODUCTVERSION,FALSE,LVCFMT_LEFT,100},
		{/*Extension,*/IDS_LISTEXTENSION,FALSE,LVCFMT_LEFT,60},
		{/*Keywords,*/IDS_LISTKEYWORDS,FALSE,LVCFMT_LEFT,100}
	};

	ViewDetails* pRet=new ViewDetails[sizeof(aDetails)/sizeof(ViewDetails)];
	CopyMemory(pRet,aDetails,sizeof(aDetails));
	return pRet;
}

////////////////////////////////////////////////////////////
// CLocateDlg - Results list - List type
////////////////////////////////////////////////////////////


void CLocateDlg::SetListType(ListType nType,BOOL bResetIfSame)
{
	if (m_nCurrentListType==nType && !bResetIfSame)
		return;

	if (m_nCurrentListType==ltDetails && nType!=ltDetails)
	{
		// Changing from details view, saving column states
		m_pListCtrl->SaveColumnsState(HKCU,
			CRegKey2::GetCommonKey()+"\\General","ListWidths");
	}

	DWORD dwStyle=m_pListCtrl->GetStyle()&~LVS_TYPEMASK;


	// Delete existing image list
	m_ilOwnImageList.DeleteImageList();


	// Releasing m_pSystemImageList
	if (m_pSystemImageList!=NULL && m_dwThumbnailFlags&tfSystemImageListIsInterface)
		m_pSystemImageList->Release();
	m_pSystemImageList=NULL;
	
	m_sCurrentIconSize.cx=0; // Icon size is set to 0 in SmallIcons, 
	m_sCurrentIconSize.cy=0; // List and Details mode (when thumbnail not needed)
	m_nBorders=0;

	// Remove thumbnail images
	int iItem=m_pListCtrl->GetNextItem(-1,LVNI_ALL);
	while (iItem!=-1)
	{
		CLocatedItem* pItem=(CLocatedItem*)m_pListCtrl->GetItemData(iItem);
		if (pItem!=NULL)
		{
			// Free thumbnail 
			pItem->DeleteThumbnail();
		}
		iItem=m_pListCtrl->GetNextItem(iItem,LVNI_ALL);
	}

	switch (nType)
	{
	case ltList:
		dwStyle|=LVS_LIST;
		break;
	case ltDetails:
		dwStyle|=LVS_REPORT;
		break;
	case ltSmallIcons:
		dwStyle|=LVS_SMALLICON;
		break;
	case ltMediumIcons:
		{
			dwStyle|=LVS_ICON;
			ImageListSize iSize=ilMedium;
			LoadSystemImageList(iSize,m_sSystemImageList);
			m_sCurrentIconSize=m_sSystemImageList;
			m_pListCtrl->SetImageList(m_dwThumbnailFlags&tfSystemImageListIsInterface?
				IImageListToHIMAGELIST(m_pSystemImageList):m_hSystemImageList,LVSIL_NORMAL);
			
		}
		break;
	case ltLargeIcons:
		{
			dwStyle|=LVS_ICON;
			ImageListSize iSize=ilLarge;
			LoadSystemImageList(iSize,m_sSystemImageList);
			m_sCurrentIconSize.cx=DEFAULT_LARGEICONSIZE;
	
			CRegKey2 RegKey;
			if (RegKey.OpenKey(HKCU,"\\General",CRegKey::defRead)==ERROR_SUCCESS)
				RegKey.QueryValue("Thumbnail Large Icon Size",(LPSTR)&m_sCurrentIconSize.cx,sizeof(LONG));
			m_sCurrentIconSize.cy=m_sCurrentIconSize.cx;
	
			m_nBorders=2;
			m_ilOwnImageList.Create(m_sCurrentIconSize.cx+2,m_sCurrentIconSize.cy+2,ILC_COLOR32,1,1);
			m_pListCtrl->SetImageList(m_ilOwnImageList,LVSIL_NORMAL);
		}
		break;
	case ltExtraLargeIcons:
		{
			dwStyle|=LVS_ICON;
			ImageListSize iSize=ilLarge;
			LoadSystemImageList(iSize,m_sSystemImageList);
			m_sCurrentIconSize.cx=DEFAULT_EXTRALARGEICONSIZE;
			CRegKey2 RegKey;
			if (RegKey.OpenKey(HKCU,"\\General",CRegKey::defRead)==ERROR_SUCCESS)
				RegKey.QueryValue("Thumbnail Extra Large Icon Size",(LPSTR)&m_sCurrentIconSize.cx,sizeof(LONG));
			m_sCurrentIconSize.cy=m_sCurrentIconSize.cx;
			m_nBorders=2;
			m_ilOwnImageList.Create(m_sCurrentIconSize.cx+2,m_sCurrentIconSize.cy+2,ILC_COLOR32,1,1);
			m_pListCtrl->SetImageList(m_ilOwnImageList,LVSIL_NORMAL);
			break;
		}
	}
	
		

		
	m_pListCtrl->SetStyle(dwStyle&~LVS_NOSCROLL);
	


	// Arrange list
	m_pListCtrl->Arrange(LVA_DEFAULT);
	m_pListCtrl->RedrawItems(0,m_pListCtrl->GetItemCount());
	m_pListCtrl->UpdateWindow();
	if (nType==ltLargeIcons || nType==ltExtraLargeIcons)
		m_pListCtrl->Invalidate();
		
	m_nCurrentListType=nType;

	SetMenuCheckMarkForListType();
}


void CLocateDlg::SetMenuCheckMarkForListType()
{
	ASSERT(m_pListCtrl!=NULL);

	CMenu menu(GetMenu());

	// Check marks for menu
	menu.CheckMenuItem(IDM_EXTRALARGEICONS,MF_BYCOMMAND|(m_nCurrentListType==ltExtraLargeIcons?MF_CHECKED:MF_UNCHECKED));
	menu.CheckMenuItem(IDM_LARGEICONS,MF_BYCOMMAND|(m_nCurrentListType==ltLargeIcons?MF_CHECKED:MF_UNCHECKED));
	menu.CheckMenuItem(IDM_MEDIUMICONS,MF_BYCOMMAND|(m_nCurrentListType==ltMediumIcons?MF_CHECKED:MF_UNCHECKED));
	menu.CheckMenuItem(IDM_SMALLICONS,MF_BYCOMMAND|(m_nCurrentListType==ltSmallIcons?MF_CHECKED:MF_UNCHECKED));
	menu.CheckMenuItem(IDM_LIST,MF_BYCOMMAND|(m_nCurrentListType==ltList?MF_CHECKED:MF_UNCHECKED));
	menu.CheckMenuItem(IDM_DETAILS,MF_BYCOMMAND|(m_nCurrentListType==ltDetails?MF_CHECKED:MF_UNCHECKED));
	
	CheckMenuItem(m_Menu.GetSubMenu(SUBMENU_CONTEXTMENUNOITEMS),IDM_EXTRALARGEICONS,MF_BYCOMMAND|(m_nCurrentListType==ltExtraLargeIcons?MF_CHECKED:MF_UNCHECKED));
	CheckMenuItem(m_Menu.GetSubMenu(SUBMENU_CONTEXTMENUNOITEMS),IDM_LARGEICONS,MF_BYCOMMAND|(m_nCurrentListType==ltLargeIcons?MF_CHECKED:MF_UNCHECKED));
	CheckMenuItem(m_Menu.GetSubMenu(SUBMENU_CONTEXTMENUNOITEMS),IDM_MEDIUMICONS,MF_BYCOMMAND|(m_nCurrentListType==ltMediumIcons?MF_CHECKED:MF_UNCHECKED));
	CheckMenuItem(m_Menu.GetSubMenu(SUBMENU_CONTEXTMENUNOITEMS),IDM_SMALLICONS,MF_BYCOMMAND|(m_nCurrentListType==ltSmallIcons?MF_CHECKED:MF_UNCHECKED));
	CheckMenuItem(m_Menu.GetSubMenu(SUBMENU_CONTEXTMENUNOITEMS),IDM_LIST,MF_BYCOMMAND|(m_nCurrentListType==ltList?MF_CHECKED:MF_UNCHECKED));
	CheckMenuItem(m_Menu.GetSubMenu(SUBMENU_CONTEXTMENUNOITEMS),IDM_DETAILS,MF_BYCOMMAND|(m_nCurrentListType==ltDetails?MF_CHECKED:MF_UNCHECKED));
			
	
	// Enable/disable "Line up icons" and "Auto arrange"
	if (m_nCurrentListType==ltList || m_nCurrentListType==ltDetails)
	{
		menu.EnableMenuItem(IDM_LINEUPICONS,MF_BYCOMMAND|MF_GRAYED);
		menu.EnableMenuItem(IDM_AUTOARRANGE,MF_BYCOMMAND|MF_GRAYED);
		EnableMenuItem(m_Menu.GetSubMenu(SUBMENU_CONTEXTMENUNOITEMS),IDM_LINEUPICONS,MF_BYCOMMAND|MF_GRAYED);
		EnableMenuItem(m_Menu.GetSubMenu(SUBMENU_CONTEXTMENUNOITEMS),IDM_AUTOARRANGE,MF_BYCOMMAND|MF_GRAYED);
	
		menu.EnableMenuItem(IDM_ALIGNTOGRID,MF_BYCOMMAND|MF_GRAYED);
		EnableMenuItem(m_Menu.GetSubMenu(SUBMENU_CONTEXTMENUNOITEMS),IDM_ALIGNTOGRID,MF_BYCOMMAND|MF_GRAYED);
	}
	else
	{
		menu.EnableMenuItem(IDM_LINEUPICONS,MF_BYCOMMAND|MF_ENABLED);
		menu.EnableMenuItem(IDM_AUTOARRANGE,MF_BYCOMMAND|MF_ENABLED);

		EnableMenuItem(m_Menu.GetSubMenu(SUBMENU_CONTEXTMENUNOITEMS),IDM_LINEUPICONS,MF_BYCOMMAND|MF_ENABLED);
		EnableMenuItem(m_Menu.GetSubMenu(SUBMENU_CONTEXTMENUNOITEMS),IDM_AUTOARRANGE,MF_BYCOMMAND|MF_ENABLED);

		if (((CLocateApp*)GetApp())->m_wComCtrlVersion>=0x0600)
		{
			menu.EnableMenuItem(IDM_ALIGNTOGRID,MF_BYCOMMAND|MF_ENABLED);
			EnableMenuItem(m_Menu.GetSubMenu(SUBMENU_CONTEXTMENUNOITEMS),IDM_ALIGNTOGRID,MF_BYCOMMAND|MF_ENABLED);
		}
		else
		{
			menu.EnableMenuItem(IDM_ALIGNTOGRID,MF_BYCOMMAND|MF_GRAYED);
			EnableMenuItem(m_Menu.GetSubMenu(SUBMENU_CONTEXTMENUNOITEMS),IDM_ALIGNTOGRID,MF_BYCOMMAND|MF_GRAYED);
		}
	}
			
	
	if (m_pListCtrl->GetStyle()&LVS_AUTOARRANGE)
	{
		menu.CheckMenuItem(IDM_AUTOARRANGE,MF_BYCOMMAND|MF_CHECKED);
		CheckMenuItem(m_Menu.GetSubMenu(SUBMENU_CONTEXTMENUNOITEMS),IDM_AUTOARRANGE,MF_BYCOMMAND|MF_CHECKED);
	}

	if (m_pListCtrl->GetExtendedListViewStyle()&LVS_EX_SNAPTOGRID)
	{
		menu.CheckMenuItem(IDM_ALIGNTOGRID,MF_BYCOMMAND|MF_CHECKED);
		CheckMenuItem(m_Menu.GetSubMenu(SUBMENU_CONTEXTMENUNOITEMS),IDM_ALIGNTOGRID,MF_BYCOMMAND|MF_CHECKED);
	}
}
