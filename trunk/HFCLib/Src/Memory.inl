////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////

#ifndef HFCMEMORY_INL
#define HFCMEMORY_INL



inline void fMemCopy(void* dst,const void* src,SIZE_T len) 
{
	for (register ULONG_PTR i=0;i<len;i++)
		((BYTE*)dst)[i]=((BYTE*)src)[i];
}


inline void fMemSet(void* dst,BYTE byte,SIZE_T count) // faster when count is small
{
	for (register ULONG_PTR i=0;i<count;i++)
		((BYTE*)dst)[i]=byte;
}

/////////////////////////////////////////////////
// Inline functions
/////////////////////////////////////////////////

////////////////////////////////////////////////
// CAutoPtr

template<class TYPE> 
inline CAutoPtr<TYPE>::~CAutoPtr()
{
	if (m_data!=NULL && m_bFree)
		delete m_data;
}

template<class TYPE> 
inline TYPE* CAutoPtr<TYPE>::UnAttach()
{
	m_bFree=FALSE;
	return m_data;
}

template<class TYPE> 
inline void CAutoPtr<TYPE>::Attach(TYPE* data,BOOL bFree)
{
	if (m_data!=NULL && m_bFree)
		delete m_data;
	m_data=data;
	m_bFree=bFree;
}

template<class TYPE> 
inline void CAutoPtr<TYPE>::Attach(CAutoPtr<TYPE>& another)
{
	if (m_data!=NULL && m_bFree)
		delete m_data;
	m_bFree=another.m_bFree;
	m_data=another.UnAttach();
}

template<class TYPE> 
inline CAutoPtr<TYPE>& CAutoPtr<TYPE>::operator=(TYPE* data)
{
	Attach(data);
	return *this;
}

	
template<class TYPE> 
inline CAutoPtr<TYPE>& CAutoPtr<TYPE>::operator=(CAutoPtr<TYPE>& another)
{
	Attach(another);
	return *this;
}



////////////////////////////////////////////////
// CAutoPtrA

template<class TYPE> 
inline CAutoPtrA<TYPE>::~CAutoPtrA()
{
	if (m_data!=NULL && m_bFree)
		delete[] m_data;
}

template<class TYPE> 
inline TYPE* CAutoPtrA<TYPE>::UnAttach()
{
	m_bFree=FALSE;
	return m_data;
}

template<class TYPE> 
inline void CAutoPtrA<TYPE>::Attach(TYPE* data,BOOL bFree)
{
	if (m_data!=NULL && m_bFree)
		delete[] m_data;
	m_data=data;
	m_bFree=bFree;
}

template<class TYPE> 
inline void CAutoPtrA<TYPE>::Attach(CAutoPtr<TYPE>& another)
{
	if (m_data!=NULL && m_bFree)
		delete[] m_data;
	m_bFree=another.m_bFree;
	m_data=another.UnAttach();
}

template<class TYPE> 
inline CAutoPtrA<TYPE>& CAutoPtrA<TYPE>::operator=(TYPE* data)
{
	Attach(data);
	return *this;
}
	
template<class TYPE> 
inline CAutoPtrA<TYPE>& CAutoPtrA<TYPE>::operator=(CAutoPtrA<TYPE>& another)
{
	Attach(another);
	return *this;
}


////////////////////////////////////////////////
// CAutoPtrGlobal

template<class TYPE> 
inline CAutoPtrGlobal<TYPE>::~CAutoPtrGlobal()
{
	Unlock();
	if (m_bFree)
		GlobalFree(m_hGlobal);
}


////////////////////////////////////////////////
// CDataContainer

template<class TYPE> 
CDataContainer<TYPE>::CDataContainer(TYPE* data)
:	m_data(data),m_dwCount(1)
{
}

template<class TYPE> 
CDataContainer<TYPE>::~CDataContainer()
{
	if (m_data!=NULL)
		delete m_data;
}

template<class TYPE> 
CDataContainer<TYPE>* CDataContainer<TYPE>::Release()
{
	--m_dwCount;
	if (m_dwCount==0)
	{
		delete this;
		return NULL;
	}
	return this;
}

////////////////////////////////////////////////
// CDataContainerA

template<class TYPE> 
CDataContainerA<TYPE>::CDataContainerA(TYPE* data)
:	m_data(data),m_dwCount(1)
{
}

template<class TYPE> 
CDataContainerA<TYPE>::~CDataContainerA()
{
	if (m_data!=NULL)
		delete[] m_data;
}

template<class TYPE> 
CDataContainerA<TYPE>* CDataContainerA<TYPE>::Release()
{
	--m_dwCount;
	if (m_dwCount==0)
	{
		delete this;
		return NULL;
	}
	return this;
}



////////////////////////////////////////////
// CGlobalAlloc

#ifdef WIN32

inline CGlobalAlloc::CGlobalAlloc(SIZE_T nSize,allocFlags nType)
:	m_hGlobal(NULL)
{
	Alloc(nSize,nType);
}

inline CGlobalAlloc::CGlobalAlloc(HGLOBAL hGlobal,allocFlags nFlags)
:	m_hGlobal(hGlobal),m_nFlags(nFlags)
{
	m_pData=m_nFlags&moveable?NULL:(BYTE*)m_hGlobal;
}
	
inline CGlobalAlloc::~CGlobalAlloc()
{
	if (m_nFlags&moveable && m_pData!=NULL)
		GlobalUnlock(m_hGlobal);
	GlobalFree(m_hGlobal);
}

inline BOOL CGlobalAlloc::IsAllocated() const
{
	return m_hGlobal!=NULL;
}

inline void CGlobalAlloc::Lock()
{
	if (m_pData==NULL && m_hGlobal!=NULL)
		m_pData=(BYTE*)GlobalLock(m_hGlobal);
}

inline SIZE_T CGlobalAlloc::GetSize() const
{
	return GlobalSize(m_hGlobal);
}

inline void CGlobalAlloc::Unlock()
{
	if (m_nFlags&moveable && m_pData!=NULL)
	{
		GlobalUnlock(m_hGlobal);
		m_pData=NULL;
	}
}

inline BOOL CGlobalAlloc::IsLocked() const
{
	return m_pData!=NULL;
}

inline CGlobalAlloc::allocFlags CGlobalAlloc::GetFlags() const
{
	return (CGlobalAlloc::allocFlags)GlobalFlags(m_hGlobal);
}

////////////////////////////////////////////
// CHeap

inline CHeap::~CHeap()
{
	if (m_bDestroy)
		::HeapDestroy(m_hHeap);
}

inline CHeap::CHeap()
:	m_bDestroy(FALSE),m_hHeap(NULL)
{
}

inline CHeap::CHeap(SIZE_T dwInitialSize,SIZE_T dwMaximumSize,attributes nAttributes)
:	m_bDestroy(FALSE)
{
	Create(dwInitialSize,dwMaximumSize,nAttributes);
}

inline BOOL CHeap::IsAllocated()
{
	return m_hHeap!=NULL;
}

inline SIZE_T CHeap::Compact(attributes nAttributes)
{
	return HeapCompact(m_hHeap,nAttributes);
}

inline BOOL CHeap::Lock()
{
	return HeapLock(m_hHeap);
}

inline BOOL CHeap::Unlock()
{
	return HeapUnlock(m_hHeap);
}

inline BOOL CHeap::Validate(LPCVOID lpBlock,attributes nAttributes)
{
	return HeapValidate(m_hHeap,nAttributes,lpBlock);
}

inline BOOL CHeap::Walk(LPPROCESS_HEAP_ENTRY lpEntry)
{
	return HeapWalk(m_hHeap,lpEntry);
}

inline CHeap::CHeapBlock* CHeap::Alloc(SIZE_T nSize,attributes nAttributes)
{
	return new CHeapBlock(HeapAlloc(m_hHeap,nAttributes|m_bThrow?HEAP_GENERATE_EXCEPTIONS:0,nSize));
}

inline CHeap CHeap::GetProcessHeap()
{
	return CHeap(::GetProcessHeap());
}

inline BOOL CHeap::Destroy()
{
	if (m_hHeap==NULL)
		return FALSE;
	BOOL nRet=HeapDestroy(m_hHeap);
	m_hHeap=NULL;
	return nRet;
}

#endif

////////////////////////////////////////////
// CMemoryStream

inline CMemoryStream::CMemoryStream(BOOL bThrow)
:	CStream(bThrow),m_dwSize(0),m_dwAllocSize(0),
	m_dwPosition(0),m_pData(NULL)
{
}

inline CMemoryStream::CMemoryStream(DWORD dwInitialSize,BOOL bThrow)
:	CStream(bThrow),m_dwPosition(0),m_dwAllocSize(dwInitialSize),
	m_dwSize(dwInitialSize)
{
	m_pData=new BYTE[dwInitialSize];
	if (m_pData==NULL && bThrow)
		throw CException(CException::cannotAllocate);
}

inline CMemoryStream::~CMemoryStream()
{
	if (m_pData!=NULL)
		delete[] m_pData;
}

inline DWORD CMemoryStream::GetAllocLength() const
{
	return m_dwAllocSize;
}

	

#endif

