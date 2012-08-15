/* Locate32 - Copyright (c) 1997-2010 Janne Huttunen */

#include <HFCLib.h>
#include "Locate32.h"


#define DEF_FORMATS		2



HRESULT STDMETHODCALLTYPE CFileObject::CEnumFORMATETC::Next(ULONG celt,FORMATETC __RPC_FAR *rgelt,ULONG __RPC_FAR *pceltFetched)
{
	FoDebugFormatMessage4("CFileObject::CEnumFORMATETC::Next(%X,%X,%X)",celt,rgelt,pceltFetched,NULL);
	
	ULONG counter=0;
	while (counter<celt)
	{
		rgelt[counter].lindex=-1;
		rgelt[counter].ptd=NULL;
		rgelt[counter].dwAspect=DVASPECT_CONTENT;
		rgelt[counter].tymed=1;

		if (m_format>DEF_FORMATS)
		{
			rgelt[counter].tymed=0;
			rgelt[counter].cfFormat=0;
			break;
		}
		
		switch (m_format)
		{
		case 1:
			rgelt[counter].cfFormat=RegisterClipboardFormat(CFSTR_SHELLIDLIST);
			FoDebugFormatMessage2("CFileObject::CEnumFORMATETC::Next, rgelt[%d].cfFormat=%X='CFSTR_SHELLIDLIST'",counter,rgelt[counter].cfFormat);
			break;
		case 2:
			rgelt[counter].cfFormat=CF_HDROP;
			FoDebugFormatMessage2("CFileObject::CEnumFORMATETC::Next, rgelt[%d].cfFormat=CF_HDROP",counter,0);
			break;
		/*case 3:
			rgelt[counter].cfFormat=RegisterClipboardFormat(CFSTR_PREFERREDDROPEFFECT);
			FoDebugFormatMessage2("CFileObject::CEnumFORMATETC::Next, rgelt[%d].cfFormat=%X='CFSTR_SHELLIDLISTOFFSET'",counter,rgelt[counter].cfFormat);
			break;*/
		default:
			FoDebugFormatMessage2("CFileObject::CEnumFORMATETC::Next, format %d is unknown",m_format,0);
			break;
		}
		m_format++;
		counter++;
	}
	if (pceltFetched!=NULL)
		*pceltFetched=counter;
	if (counter!=celt)
	{
		FoDebugMessage("CFileObject::CEnumFORMATETC::Next, will return S_FALSE");
		return S_FALSE;
	}
	FoDebugMessage("CFileObject::CEnumFORMATETC::Next, will return S_OK");
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CFileObject::CEnumFORMATETC::Skip(ULONG celt)
{
	FoDebugFormatMessage2("CFileObject::CEnumFORMATETC::Skip(%X)",celt,NULL);
	
	m_format+=celt;
	if (m_format>DEF_FORMATS)
		return S_FALSE;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CFileObject::CEnumFORMATETC::Reset(void)
{
	FoDebugMessage("CFileObject::CEnumFORMATETC::Reset");

	m_format=1;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CFileObject::CEnumFORMATETC::Clone(IEnumFORMATETC __RPC_FAR *__RPC_FAR *ppenum)
{
	FoDebugFormatMessage2("CFileObject::CEnumFORMATETC::Clone(%X)",ppenum,NULL);

	*ppenum=new CEnumFORMATETC;
	if (*ppenum==NULL)
		return E_OUTOFMEMORY;
	((CEnumFORMATETC*)*ppenum)->AddRef();
	((CEnumFORMATETC*)*ppenum)->m_format=m_format;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CFileObject::GetData(FORMATETC *pformatetcIn,STGMEDIUM *pmedium)
{

	FoDebugFormatMessage4("CFileObject::GetData(%X,%X), cfFormat=%X",pformatetcIn,pmedium,pformatetcIn->cfFormat,NULL);

	if (pformatetcIn==NULL || pmedium==NULL)
	{
		FoDebugMessage("CFileObject::GetData: Returning E_INVALIDARG");
		return E_INVALIDARG;
	}
	if (pformatetcIn->lindex!=-1)
	{
		FoDebugMessage("CFileObject::GetData: Returning DV_E_LINDEX");
		return DV_E_LINDEX;
	}
	if ((pformatetcIn->tymed&TYMED_HGLOBAL)==0)
	{
		FoDebugMessage("CFileObject::GetData: Returning DV_E_TYMED");
		return DV_E_TYMED;
	}
	if (pformatetcIn->dwAspect!=DVASPECT_CONTENT)
	{
		FoDebugMessage("CFileObject::GetData: Returning DV_E_DVASPECT");
		return DV_E_DVASPECT;
	}
	
	pmedium->tymed=TYMED_HGLOBAL;
	pmedium->pUnkForRelease=NULL;
	
	if (pformatetcIn->cfFormat==0)
		pformatetcIn->cfFormat=CF_HDROP;
	else if (pformatetcIn->cfFormat==CF_HDROP)
	{
		if (m_Files.GetSize()==0)
			return DV_E_FORMATETC;
		pmedium->hGlobal=GetHDrop();
	}
	else if (pformatetcIn->cfFormat>0x100)
	{
		char szFormat[100];
		GetClipboardFormatName(pformatetcIn->cfFormat,szFormat,100);
		FoDebugFormatMessage2("CFileObject::GetData: format is %s",szFormat,0);
		if (strcasecmp(CFSTR_SHELLIDLIST,szFormat)==0)
		{
			if (m_Files.GetSize()==0)
				return DV_E_FORMATETC;
			pmedium->hGlobal=GetItemIDList();
		}
		/*else if (strcasecmp(CFSTR_PREFERREDDROPEFFECT,szFormat)==0)
		{
			pmedium->hGlobal=GlobalAlloc(GHND|GMEM_SHARE,4);
			int* pDropTarget=(int*)GlobalLock(pmedium->hGlobal);
			*pDropTarget=DROPEFFECT_COPY|DROPEFFECT_LINK;
			GlobalUnlock(pmedium->hGlobal);
		}*/
		else if (strcasecmp("Locate Item positions",szFormat)==0)
		{
			pmedium->hGlobal=GlobalAlloc(GHND|GMEM_SHARE,sizeof(POINT));
			POINT* pPt=(POINT*)GlobalLock(pmedium->hGlobal);
			sMemCopy(pPt,&m_StartPosition,sizeof(POINT));
			GlobalUnlock(pmedium->hGlobal);
		}
		else
		{
			FoDebugMessage("CFileObject::GetData: Returning DV_E_FORMATETC (1)");
			return DV_E_FORMATETC;
		}
	}
	else
	{
		FoDebugMessage("CFileObject::GetData: Returning DV_E_FORMATETC (2)");
		return DV_E_FORMATETC;
	}

	FoDebugMessage("CFileObject::GetData: Returning S_OK");
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CFileObject::GetDataHere(FORMATETC *pformatetc,STGMEDIUM *pmedium)
{
	/*
	FoDebugFormatMessage4("CFileObject::GetDataHere(%X,%X), cfFormat=%X",pformatetc,pmedium,pformatetc->cfFormat,NULL);

	HRESULT hRes;
	STGMEDIUM med;
	if (pformatetc->tymed!=TYMED_HGLOBAL)
		return DV_E_TYMED;
	med.pUnkForRelease=pmedium->pUnkForRelease;
	hRes=GetData(pformatetc,&med);
	pmedium->tymed=med.tymed;
	sMemCopy(pmedium->hGlobal,med.hGlobal,GlobalSize(med.hGlobal));
	GlobalFree(med.hGlobal);
	return hRes;
	*/
	return E_NOTIMPL;
}
	
HRESULT STDMETHODCALLTYPE CFileObject::QueryGetData(FORMATETC *pformatetc)
{
	FoDebugFormatMessage2("CFileObject::QueryGetData(%X), cfFormat=%X",pformatetc,pformatetc->cfFormat);

	if (pformatetc==NULL)
	{
		FoDebugMessage("CFileObject::QueryGetData: invalid argument");
		return E_INVALIDARG;
	}
	
	if (pformatetc->lindex!=-1)
	{
		FoDebugMessage("CFileObject::QueryGetData: invalid lindex");
		return DV_E_LINDEX;
	}
	

	if (pformatetc->tymed==0xFFFFFFFF)
		pformatetc->tymed=TYMED_HGLOBAL;
	else if (pformatetc->tymed!=TYMED_HGLOBAL)
	{
#ifdef _DEBUG
		char szFormat[100];
		if (GetClipboardFormatName(pformatetc->cfFormat,szFormat,100))
			FoDebugFormatMessage2("CFileObject::QueryGetData: invalid tymed %X, format=%s",pformatetc->tymed,szFormat);
		else
			FoDebugFormatMessage2("CFileObject::QueryGetData: invalid tymed %X",pformatetc->tymed,NULL);
#endif
		return DV_E_TYMED;
	}

	if (pformatetc->dwAspect!=DVASPECT_CONTENT)
	{
		FoDebugMessage("CFileObject::QueryGetData: invalid dwAspect");
		return DV_E_DVASPECT;
	}
	if (pformatetc->cfFormat==CF_HDROP)
	{
		FoDebugMessage("CFileObject::QueryGetData: format is CF_HDROP");
		return S_OK;
	}
	char szFormat[100];
	if (GetClipboardFormatName(pformatetc->cfFormat,szFormat,100))
	{
		FoDebugFormatMessage2("CFileObject::QueryGetData:, format is '%s'",szFormat,NULL);

		if (strcasecmp(szFormat,CFSTR_SHELLIDLIST)==0)
			return S_OK;
		if (strcasecmp(szFormat,"Locate Item positions")==0)
			return S_OK;
	}
	else
	{
		FoDebugFormatMessage2("CFileObject::QueryGetData:Failed to get clipboard format name for %X",pformatetc->cfFormat,NULL);
	}
	return DV_E_FORMATETC;
}

HRESULT STDMETHODCALLTYPE CFileObject::GetCanonicalFormatEtc(FORMATETC *pformatectIn,FORMATETC *pformatetcOut)
{
	//FoDebugFormatMessage2("CFileObject::QueryGetData(%X,%X)",pformatectIn,pformatetcOut);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CFileObject::SetData(FORMATETC *pformatetc,STGMEDIUM *pmedium,BOOL fRelease)
{	
	FoDebugFormatMessage4("CFileObject::SetData(%X,%X,%X), pformatetc->cfFormat=%X",pformatetc,pmedium,fRelease,pformatetc->cfFormat);

	if (pformatetc==NULL || pmedium==NULL)
		return E_INVALIDARG;
	if (pformatetc->lindex!=-1)
		return DV_E_LINDEX;
	if (pformatetc->tymed!=TYMED_HGLOBAL)
		return DV_E_TYMED;
	if (pformatetc->dwAspect!=DVASPECT_CONTENT)
		return DV_E_DVASPECT;
	if (pformatetc->cfFormat==CF_HDROP)
	{
		char szPath[_MAX_PATH];
		m_Files.RemoveAll();
		m_Points.RemoveAll();
		for (int i=DragQueryFile((HDROP)pmedium->hGlobal,0xFFFFFFFF,NULL,0)-1;i>=0;i--)
		{
			DragQueryFile((HDROP)pmedium->hGlobal,i,szPath,_MAX_PATH);
			m_Files.Add(new CStringW(szPath));
		}
		DragFinish((HDROP)pmedium->hGlobal);
	}
	else if (pformatetc->cfFormat>0x100)
	{
		char szFormat[100];
		GetClipboardFormatName(pformatetc->cfFormat,szFormat,100);
		if (_stricmp(CFSTR_FILENAME,szFormat)==0)
		{
			m_Files.RemoveAll();
			m_Points.RemoveAll();
			m_Files.Add(new CStringW((LPCSTR)pmedium->hGlobal));
		}
		else if (_stricmp(CFSTR_FILENAMEW,szFormat)==0)
		{
			m_Files.RemoveAll();
			m_Points.RemoveAll();
			m_Files.Add(new CStringW((LPCWSTR)pmedium->hGlobal));
		}
		else if (_stricmp(CFSTR_TARGETCLSID,szFormat)==0)
			return S_OK;
		else if (_stricmp(CFSTR_LOGICALPERFORMEDDROPEFFECT,szFormat)==0)
			return S_OK;
		else if (_stricmp(CFSTR_PERFORMEDDROPEFFECT,szFormat)==0)
			return S_OK;
		else if (_stricmp(CFSTR_PREFERREDDROPEFFECT,szFormat)==0)
			return S_OK;
		else
			return DV_E_FORMATETC;
	}
	else
		return DV_E_FORMATETC;
	if (fRelease)
		ReleaseStgMedium(pmedium);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CFileObject::EnumFormatEtc(DWORD dwDirection,IEnumFORMATETC** ppenumFormatEtc)
{
	FoDebugFormatMessage2("CFileObject::EnumFormatEtc(%X,%X)",dwDirection,ppenumFormatEtc);

	if (dwDirection==DATADIR_GET)
	{
		*ppenumFormatEtc=new CEnumFORMATETC;
		((CEnumFORMATETC*)*ppenumFormatEtc)->AutoDelete();
		if (*ppenumFormatEtc==NULL)
			return E_OUTOFMEMORY;
		(*ppenumFormatEtc)->AddRef();
		return S_OK;
	}
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CFileObject::DAdvise(FORMATETC *pformatetc,DWORD advf,IAdviseSink *pAdvSink,DWORD *pdwConnection)
{
	FoDebugFormatMessage4("CFileObject::DAdvise(%X,%X,%X,%X)",pformatetc,advf,pAdvSink,pdwConnection);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CFileObject::DUnadvise(DWORD dwConnection)
{
	FoDebugFormatMessage2("CFileObject::DUnadvise(%X)",dwConnection,NULL);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CFileObject::EnumDAdvise(IEnumSTATDATA** ppenumAdvise)
{
	FoDebugFormatMessage2("CFileObject::EnumDAdvise(%X)",ppenumAdvise,NULL);
	return E_NOTIMPL;
}

BYTE CFileObject::SetFile(LPCWSTR szFile)
{
	m_Files.RemoveAll();
	m_Points.RemoveAll();
	m_Files.Add(new CStringW(szFile));
	return TRUE;
}

BYTE CFileObject::SetFiles(CListCtrl* pList,BOOL bNoDeleted,BOOL bForParents)
{
	m_Files.RemoveAll();
	m_Points.RemoveAll();
	BOOL bGetPoints=((pList->GetStyle() & LVS_TYPEMASK)==LVS_ICON) || (pList->GetSelectedCount()==1);

	int nItem=pList->GetNextItem(-1,LVNI_SELECTED);
	while (nItem!=-1)
	{
		CLocatedItem* pData=(CLocatedItem*)pList->GetItemData(nItem);
		if (pData!=NULL)
		{
			if (bNoDeleted)
			{
				BOOL bDeleted;
				if (bForParents)
					bDeleted=!FileSystem::IsDirectory(pData->GetParent());
				else if (pData->IsFolder())
					bDeleted=!FileSystem::IsDirectory(pData->GetPath());
				else	
					bDeleted=!FileSystem::IsFile(pData->GetPath());

				if (bDeleted)
				{
					nItem=pList->GetNextItem(nItem,LVNI_SELECTED);
					continue;
				}
			}

			m_Files.Add(new CStringW(bForParents?pData->GetParent():pData->GetPath()));
			if (bGetPoints)
			{
				CPoint* pt=new CPoint;
				pList->GetItemPosition(nItem,pt);
				pList->ClientToScreen(pt);
				m_Points.Add(pt);
			}
		}
		nItem=pList->GetNextItem(nItem,LVNI_SELECTED);
	}
	return TRUE;
}

HGLOBAL CFileObject::GetHDrop()
{
	HGLOBAL hGlobal;
	DWORD nDataLength=sizeof(DROPFILES)+2;
	
	if (IsUnicodeSystem())
	{
		// Win2000/XP needs Unicode
		int i;
		for (i=0;i<m_Files.GetSize();i++)
			nDataLength+=((DWORD)m_Files[i]->GetLength()+1)*2;
		hGlobal=GlobalAlloc(GHND|GMEM_SHARE,nDataLength);
		BYTE* pLock=(BYTE*)GlobalLock(hGlobal);


		if (pLock==NULL)
			return NULL;
		((DROPFILES*)pLock)->fNC=TRUE;
		GetCursorPos(&((DROPFILES*)pLock)->pt);
		((DROPFILES*)pLock)->fWide=TRUE;
		((DROPFILES*)pLock)->pFiles=sizeof(DROPFILES);
		LPWSTR pDst=(LPWSTR)(pLock+sizeof(DROPFILES));
		for (i=0;i<m_Files.GetSize();i++)
		{
			MemCopyW(pDst,*m_Files[i],m_Files[i]->GetLength()+1);
			pDst+=m_Files[i]->GetLength()+1;
		}
		*pDst=L'\0';
		
		GlobalUnlock(hGlobal);
	}
	else
	{
		for (int i=0;i<m_Files.GetSize();i++)
			nDataLength+=(DWORD)LenWtoA((LPCWSTR)*m_Files[i])+1;
		hGlobal=GlobalAlloc(GHND|GMEM_SHARE,nDataLength);
		BYTE* pLock=(BYTE*)GlobalLock(hGlobal);

		if (pLock==NULL)
			return NULL;
		((DROPFILES*)pLock)->fNC=TRUE;
		GetCursorPos(&((DROPFILES*)pLock)->pt);
		((DROPFILES*)pLock)->fWide=FALSE;
		((DROPFILES*)pLock)->pFiles=sizeof(DROPFILES);
		LPSTR pDst=(LPSTR)pLock+sizeof(DROPFILES);
		for (int i=0;i<m_Files.GetSize();i++)
			pDst+=MemCopyWtoA(pDst,nDataLength,(LPCWSTR)*m_Files[i],m_Files[i]->GetLength()+1);
		*pDst='\0';

		GlobalUnlock(hGlobal);
	}
	
	
	return hGlobal;
}

HGLOBAL CFileObject::GetFileNameA()
{
	if (m_Files.GetSize()==0)
		return NULL;

	int nLen=LenWtoA((LPCWSTR)*m_Files.GetAt(0))+1;
	HGLOBAL hGlobal=GlobalAlloc(GHND|GMEM_SHARE,nLen);
	LPSTR pStr=(LPSTR)GlobalLock(hGlobal);
	MemCopyWtoA(pStr,nLen,m_Files.GetAt(0)->GetBuffer(),m_Files.GetAt(0)->GetLength()+1);
	GlobalUnlock(hGlobal);
	return hGlobal;
}

HGLOBAL CFileObject::GetFileNameW()
{
	if (m_Files.GetSize()==0)
		return NULL;

	HGLOBAL hGlobal=GlobalAlloc(GPTR,(m_Files.GetAt(0)->GetLength()+1)*2);
	LPWSTR pStr=(LPWSTR)GlobalLock(hGlobal);
	MemCopyW(pStr,m_Files.GetAt(0)->GetBuffer(),m_Files.GetAt(0)->GetLength()+1);
	GlobalUnlock(hGlobal);
	return hGlobal;
}

HGLOBAL CFileObject::GetItemIDList()
{
	int i,nFiles=0;
	LPITEMIDLIST* pItemLists=new LPITEMIDLIST[m_Files.GetSize()+1];
	DWORD* pItemLengths=new DWORD[m_Files.GetSize()+1];
	UINT nDataLength=sizeof(CIDA)+m_Files.GetSize()*sizeof(UINT);
	
	pItemLists[0]=NULL;
	nDataLength+=pItemLengths[0]=2;
	
	for (i=0;i<m_Files.GetSize();i++)
	{
		pItemLists[nFiles+1]=ShellFunctions::GetIDList(*m_Files[i]);
		if (pItemLists[nFiles+1]!=NULL)
		{
			nDataLength+=pItemLengths[nFiles+1]=ShellFunctions::GetIDListSize(pItemLists[nFiles+1]);
			nFiles++;
		}
	}
	HGLOBAL hGlobal=GlobalAlloc(GHND|GMEM_SHARE,nDataLength);
	CIDA* pida=(CIDA*)GlobalLock(hGlobal);
	pida->cidl=nFiles;
	DWORD nOffset=pida->aoffset[0]=sizeof(CIDA)+nFiles*sizeof(UINT);
	
	// Parent item
	*(WORD*)((LPSTR)pida+nOffset)=0;
	nOffset+=2;
		
	for (i=1;i<=nFiles;i++)
	{
		pida->aoffset[i]=nOffset;
		sMemCopy((LPSTR)pida+nOffset,pItemLists[i],pItemLengths[i]);
		CoTaskMemFree(pItemLists[i]);
		nOffset+=pItemLengths[i];
	}

	delete[] pItemLists;
	delete[] pItemLengths;

	GlobalUnlock(hGlobal);
	return hGlobal;
}


HRESULT STDMETHODCALLTYPE CFileSource::QueryContinueDrag(BOOL fEscapePressed,DWORD grfKeyState)
{
	if (fEscapePressed)
		return DRAGDROP_S_CANCEL;
	if (!(grfKeyState&MK_LBUTTON) && !(grfKeyState&MK_RBUTTON))
		return DRAGDROP_S_DROP;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CFileSource::GiveFeedback(DWORD dwEffect)
{
	return DRAGDROP_S_USEDEFAULTCURSORS;
}


HRESULT STDMETHODCALLTYPE CFileTarget::DragEnter(IDataObject __RPC_FAR *pDataObj,
		DWORD grfKeyState,POINTL pt,DWORD __RPC_FAR *pdwEffect)
{
#ifdef DEBUGMSG_FILEOBJECT
	// Quoery formats
	IEnumFORMATETC* pEnumFormatETC;
	if (SUCCEEDED(pDataObj->EnumFormatEtc(DATADIR_GET,&pEnumFormatETC)))
	{
		DebugMessage("IEnumFORMATETC gives following GET formats");

		ULONG nFethecd;
		FORMATETC formatetc;
		while (pEnumFormatETC->Next(1,&formatetc,&nFethecd)==S_OK)
		{
			char szFormat[100]="";
			GetClipboardFormatName(formatetc.cfFormat,szFormat,100);
			DebugFormatMessage("cfFormat=%X (%s) tymed=%X lindex=%X",
				formatetc.cfFormat,szFormat,formatetc.tymed,formatetc.lindex);
		}

		pEnumFormatETC->Release();
	}

	if (SUCCEEDED(pDataObj->EnumFormatEtc(DATADIR_SET,&pEnumFormatETC)))
	{
		DebugMessage("IEnumFORMATETC gives following SET formats");

		ULONG nFethecd;
		FORMATETC formatetc;
		while (pEnumFormatETC->Next(1,&formatetc,&nFethecd)==S_OK)
		{
			char szFormat[100]="";
			GetClipboardFormatName(formatetc.cfFormat,szFormat,100);
			DebugFormatMessage("cfFormat=%X (%s) tymed=%X lindex=%X",
				formatetc.cfFormat,szFormat,formatetc.tymed,formatetc.lindex);
		}

		pEnumFormatETC->Release();
	}

#endif


	if (grfKeyState&MK_LBUTTON)
	{
		FORMATETC FormatEtc;
		FormatEtc.cfFormat=RegisterClipboardFormat("Locate Item positions");
		FormatEtc.ptd=NULL;
		FormatEtc.dwAspect=DVASPECT_CONTENT;
		FormatEtc.lindex=-1;
		FormatEtc.tymed=TYMED_HGLOBAL;
		if (pDataObj->QueryGetData(&FormatEtc)!=S_OK)
		{
			*pdwEffect=DROPEFFECT_NONE;
			return S_OK;
		}

		m_pDataObjectInWindow=pDataObj;
		m_pDataObjectInWindow->AddRef();

		*pdwEffect=DROPEFFECT_MOVE;
		return S_OK;			
	}
	*pdwEffect=DROPEFFECT_NONE;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CFileTarget::DragOver(DWORD grfKeyState,
		POINTL pt,DWORD __RPC_FAR *pdwEffect)
{
	if (m_pDataObjectInWindow!=NULL && grfKeyState&MK_LBUTTON)
	{
		FORMATETC FormatEtc;
		FormatEtc.cfFormat=RegisterClipboardFormat("Locate Item positions");
		FormatEtc.ptd=NULL;
		FormatEtc.dwAspect=DVASPECT_CONTENT;
		FormatEtc.lindex=-1;
		FormatEtc.tymed=TYMED_HGLOBAL;
		if (m_pDataObjectInWindow->QueryGetData(&FormatEtc)==S_OK)
		{
			*pdwEffect=DROPEFFECT_MOVE;
			return S_OK;
		}
	}
	*pdwEffect=DROPEFFECT_NONE;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CFileTarget::DragLeave(void)
{
	if (m_pDataObjectInWindow!=NULL)
	{
		m_pDataObjectInWindow->Release();
		m_pDataObjectInWindow=NULL;
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CFileTarget::Drop(IDataObject __RPC_FAR *pDataObj,
		DWORD grfKeyState,POINTL pt,DWORD __RPC_FAR *pdwEffect)
{
	DebugMessage("CFileTarget::Drop(IDataObject __RPC_FAR *pDataObj,DWORD grfKeyState,POINTL pt,DWORD __RPC_FAR *pdwEffect)");
	
	if (m_pDataObjectInWindow!=NULL)
	{
		ASSERT(m_pDataObjectInWindow==pDataObj);

		m_pDataObjectInWindow->Release();
		m_pDataObjectInWindow=NULL;
	}

	
	
	CListCtrl* pList=GetLocateDlg()->m_pListCtrl;
	if (pList->GetStyle()&LVS_REPORT)
		return S_OK;
	FORMATETC FormatEtc;
	FormatEtc.cfFormat=RegisterClipboardFormat("Locate Item positions");
	FormatEtc.ptd=NULL;
	FormatEtc.dwAspect=DVASPECT_CONTENT;
	FormatEtc.lindex=-1;
	FormatEtc.tymed=TYMED_HGLOBAL;
	STGMEDIUM Medium;
	if (pDataObj->GetData(&FormatEtc,&Medium)!=S_OK)
		return E_UNEXPECTED;
	POINT* pPoint=(POINT*)GlobalLock(Medium.hGlobal);
	int nItem=pList->GetNextItem(-1,LVNI_SELECTED);
	while (nItem!=-1)
	{
		POINT pos;
		pList->GetItemPosition(nItem,&pos);
		pos.x+=pt.x-pPoint->x;
		pos.y+=pt.y-pPoint->y;
		pList->SetItemPosition(nItem,pos);
		nItem=pList->GetNextItem(nItem,LVNI_SELECTED);
	}
	GlobalUnlock(Medium.hGlobal);
	ReleaseStgMedium(&Medium);
	return S_OK;
}
