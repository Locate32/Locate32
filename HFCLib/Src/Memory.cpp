////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////

#include "HFCLib.h"


#define MEMORYSTREAM_EXTRAALLOC					64


#ifdef WIN32
////////////////////////////////////////////
// CGlobalAlloc
////////////////////////////////////////////


BOOL CGlobalAlloc::Alloc(SIZE_T nSize,allocFlags nFlags)
{
	if (m_hGlobal!=NULL)
		Free();
	m_hGlobal=GlobalAlloc(nFlags,nSize);
	DebugOpenHandle(dhtMemoryBlock,m_hGlobal,STRNULL);
	if (m_hGlobal==NULL)
	{
		if (m_bThrow)
			throw CException(CException::cannotAllocate);
		return FALSE;
	}
	m_nFlags=nFlags;
	if ((m_nFlags&moveable)==0)
		m_pData=(BYTE*)m_hGlobal;
	else
		m_pData=NULL;
	return TRUE;
}

BOOL CGlobalAlloc::ReAlloc(SIZE_T nSize,allocFlags nFlags)
{
	if (nFlags&moveable && m_pData!=NULL)
		GlobalUnlock(m_hGlobal);
	HGLOBAL hGlobal=GlobalReAlloc(m_hGlobal,nSize,nFlags);
	if (hGlobal==NULL)
	{
		if (m_bThrow)
			throw CException(CException::cannotAllocate);
		return FALSE;
	}
	m_hGlobal=hGlobal;
	m_nFlags=nFlags;
	if (m_nFlags&moveable)
		m_pData=NULL;
	else
		m_pData=(BYTE*)hGlobal;
	return TRUE;
}

void CGlobalAlloc::Free()
{
	if ((m_nFlags&moveable)==0 && m_pData!=NULL)
		GlobalUnlock(m_hGlobal);
	GlobalFree(m_hGlobal);
	DebugCloseHandle(dhtMemoryBlock,m_hGlobal,STRNULL);
	m_pData=NULL;
	m_hGlobal=NULL;
}

BOOL CGlobalAlloc::Discard()
{
	Unlock();
	HGLOBAL hGlobal=GlobalDiscard(m_hGlobal);
	if (hGlobal==NULL)
	{
		if (m_bThrow)
			throw CException(CException::cannotDiscard);
		return FALSE;
	}
	m_hGlobal=hGlobal;
	return TRUE;
}

////////////////////////////////////////////
// CHeap
////////////////////////////////////////////

BOOL CHeap::Create(SIZE_T dwInitialSize,SIZE_T dwMaximumSize,attributes nAttributes)
{
	m_hHeap=HeapCreate(nAttributes|m_bThrow?HEAP_GENERATE_EXCEPTIONS:0,dwInitialSize,dwMaximumSize);
	if (m_hHeap==NULL)
	{
		if (m_bThrow)
			throw CException(CException::cannotCreateHeap);
		return FALSE;
	}
	m_bDestroy=TRUE;
	return TRUE;
}

#endif //WIN32

////////////////////////////////////////////
// CMemoryStream
////////////////////////////////////////////

DWORD CMemoryStream::GetLength(DWORD* pHigh) const 
{
	if (pHigh!=NULL)
		*pHigh=0;
	return m_dwSize;
}

ULONGLONG CMemoryStream::GetLength64() const
{
	return m_dwSize;
}

BOOL CMemoryStream::SetLength(DWORD dwNewLen,LONG* pHigh)
{
	if (pHigh!=NULL)
	{
		if (*pHigh!=NULL && m_bThrow)
			throw CException(CException::invalidParameter);
	}

	if (dwNewLen==0)
	{
		if (m_pData!=NULL)
		{
			delete[] m_pData;
			m_pData=NULL;
		}
		m_dwAllocSize=0;
		m_dwSize=0;
		m_dwPosition=0;
		return TRUE;
	}

	if (dwNewLen>m_dwAllocSize)
	{
		BYTE* pNew=new BYTE[m_dwAllocSize=dwNewLen+MEMORYSTREAM_EXTRAALLOC];
		ZeroMemory(pNew,m_dwAllocSize);
		if (m_pData!=NULL)
		{
			CopyMemory(pNew,m_pData,m_dwSize);
			delete[] m_pData;
		}
		m_pData=pNew;
		m_dwSize=dwNewLen;
		return TRUE;
	}

	m_dwSize=dwNewLen;
	if (m_dwPosition>dwNewLen)
		m_dwPosition=dwNewLen;
	return TRUE;
}

BOOL CMemoryStream::SetLength64(ULONGLONG dwNewLen)
{
	if (dwNewLen>0xFFFFFFFF && m_bThrow)
		throw CException(CException::invalidParameter);

	return SetLength((DWORD)dwNewLen,NULL);
}


// Set position
DWORD CMemoryStream::Seek(LONG lOff, SeekPosition nFrom,LONG* pHighPos)
{
	if (pHighPos!=NULL)
	{
		if (*pHighPos!=NULL && m_bThrow)
			throw CException(CException::invalidParameter);
	}
	
	DWORD dwNewPos;
	switch (nFrom)
	{
	case begin:
		if (lOff<0)
		{
			if (m_bThrow)
				throw CException(CException::invalidParameter);
			return 0;
		}
		dwNewPos=lOff;
		break;
	case end:
		dwNewPos=m_dwSize+lOff;
		break;
	case current:
		if (lOff<0)
		{
			if (-lOff>(LONG)m_dwPosition)
			{
				if (m_bThrow)
					throw CException(CException::invalidParameter);
				return 0;
			}
		}
		dwNewPos=m_dwPosition+lOff;
		break;
	}


	if (dwNewPos>m_dwAllocSize)
	{
		BYTE* pNew=new BYTE[m_dwAllocSize=dwNewPos+MEMORYSTREAM_EXTRAALLOC];
		ZeroMemory(pNew,m_dwAllocSize);
		if (m_pData!=NULL)
		{
			CopyMemory(pNew,m_pData,m_dwSize);
			delete[] m_pData;
		}
		m_pData=pNew;
		m_dwSize=m_dwPosition=dwNewPos;
		return TRUE;
	}

	m_dwPosition=dwNewPos;
	if (dwNewPos>m_dwSize)
		m_dwSize=dwNewPos;
		
	return TRUE;
}

DWORD CMemoryStream::Seek64(ULONGLONG lOff, SeekPosition nFrom)
{
	if (lOff+m_dwPosition>0xFFFFFFFF && m_bThrow)
		throw CException(CException::invalidParameter);

	return Seek((DWORD)lOff,nFrom,NULL);
}


// Get position
ULONG CMemoryStream::GetPosition(PLONG pHigh) const
{
	if (pHigh!=NULL)
		*pHigh=0;
	return m_dwPosition;
}

ULONGLONG CMemoryStream::GetPosition64() const
{
	return m_dwPosition;
}


// Reading/writing
DWORD CMemoryStream::Read(void* lpBuf, DWORD nCount) const
{
	if (m_dwPosition+nCount>m_dwSize)
	{
		if (m_bThrow)
			throw CException(CException::invalidParameter);
		
		DWORD dwLength=m_dwSize-m_dwPosition;
		CopyMemory(lpBuf,m_pData+m_dwPosition,dwLength);
		m_dwPosition=m_dwSize;
		return dwLength;
	}

	CopyMemory(lpBuf,m_pData+m_dwPosition,nCount);
	m_dwPosition+=nCount;
	return nCount;
}

BOOL CMemoryStream::Write(const void* lpBuf, DWORD nCount)
{
	if (m_dwPosition+nCount>m_dwAllocSize)
	{
		BYTE* pNew=new BYTE[m_dwAllocSize=m_dwPosition+nCount+MEMORYSTREAM_EXTRAALLOC];
		ZeroMemory(pNew,m_dwAllocSize);
		if (m_pData!=NULL)
		{
			CopyMemory(pNew,m_pData,m_dwSize);
			delete[] m_pData;
		}
		m_pData=pNew;
	}

	CopyMemory(m_pData+m_dwPosition,lpBuf,nCount);
	m_dwPosition+=nCount;
	
	if (m_dwPosition>m_dwSize)
		m_dwSize=m_dwPosition;
	return TRUE;		
}

#ifdef DEF_WCHAR
BOOL CMemoryStream::Write(LPCWSTR szString,DWORD dwCount) 
{ 
	return CMemoryStream::Write((const void*)szString,(DWORD)(2*dwCount)); 
}
#endif 

void CMemoryStream::FreeExtraAlloc()
{
	if (m_dwSize==m_dwAllocSize)
		return;

	BYTE* pNew=new BYTE[m_dwAllocSize=m_dwSize];
	CopyMemory(pNew,m_pData,m_dwSize);
	delete[] m_pData;
	m_pData=pNew;
}


