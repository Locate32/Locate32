////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////


#ifndef HFCARRAYS_H
#define HFCARRAYS_H

// To work without whole HFCLIB
#ifndef HFCLIB
class CExceptionObject
{
public:
	CExceptionObject();
protected:
	BOOL m_bThrow;
};
inline CExceptionObject::CExceptionObject()
:	m_bThrow(FALSE)
{
}
#endif

// Array template

template<class TYPE>
class CArray : public CExceptionObject 
{
public:
	CArray();
	~CArray();

	int GetSize() const;
	int GetSizeOfType() const { return sizeof(TYPE); }

	void SetSize(int nNewSize);
	void RemoveAll();
	void FreeExtra();

	TYPE GetAt(int nIndex) const;
	void SetAt(int nIndex,TYPE newElement);
	TYPE& GetAtRef(int nIndex);
	TYPE* GetAtPtr(int nIndex);
	
	TYPE GetLast() const;
	void SetLast(TYPE newElement);
	TYPE& GetLastRef();
	TYPE* GetLastPtr();
	
	TYPE& ElementAt(int nIndex);

	const TYPE* GetData() const;
	TYPE* GetData();
	operator TYPE*();
	operator const TYPE*() const;


	int Add(TYPE newElement);
	int Append(const CArray& src);
	void Copy(const CArray& src);

	TYPE operator[](int nIndex) const;
	TYPE& operator[](int nIndex);
	TYPE operator[](DWORD nIndex) const;
	TYPE& operator[](DWORD nIndex);
	
	void InsertAt(int nIndex,TYPE newElement,int nCount=1);
	void RemoveAt(int nIndex);
	void InsertAt(int nStartIndex,CArray* pNewArray);

	int Find(TYPE element,int startat=0) const; // Returns index to element
	
	void Swap(CArray& src);

	TYPE* GiveBuffer();

public:
	TYPE* m_pData;
	int m_nSize;
};

// Array functions

template<class TYPE>
inline CArray<TYPE>::CArray()
:	m_pData(NULL),m_nSize(0)
{
}

template<class TYPE>
CArray<TYPE>::~CArray()
{
	if (m_pData!=NULL)
		delete[] m_pData;
	m_pData=NULL;
	m_nSize=0;
}

template<class TYPE>
inline int CArray<TYPE>::GetSize() const
{
	return (m_pData==NULL?0:m_nSize);
}

template<class TYPE>
void CArray<TYPE>::SetSize(int nNewSize)
{
	if (nNewSize<=0)
	{
		RemoveAll();
		return;
	}
	if (nNewSize<=m_nSize && nNewSize>m_nSize-10)
		m_nSize=nNewSize;
	else
	{
		TYPE* temp=m_pData;
		m_pData=new TYPE[max(nNewSize,2)];
		if (m_pData==NULL)
		{
			m_pData=temp;
			if (m_bThrow)
				throw CException(CException::cannotAllocate);
			else
				SetHFCError(HFC_CANNOTALLOC);
			return;
		}

		if (temp!=NULL)
		{
			MemCopy(m_pData,temp,min(m_nSize,nNewSize)*sizeof(TYPE));
			delete[] temp;	
		}
		m_nSize=nNewSize;
	}
}

template<class TYPE>
void CArray<TYPE>::RemoveAll()
{
	if (m_pData!=NULL)
		delete[] m_pData;
	m_pData=NULL;
	m_nSize=0;
}

template<class TYPE>
void CArray<TYPE>::FreeExtra()
{
	if (m_pData==NULL)
		return;
	TYPE* newarray=new TYPE[max(m_nSize,2)];
	if (newarray==NULL)
	{
		if (m_bThrow)
			throw CException(CException::cannotAllocate);
		else
			SetHFCError(HFC_CANNOTALLOC);
		return;
	}
	MemCopy(newarray,m_pData,m_nSize*sizeof(TYPE));
	delete[] m_pData;
	m_pData=newarray;
}

template<class TYPE>
inline TYPE CArray<TYPE>::GetAt(int nIndex) const
{ return m_pData[nIndex]; }

template<class TYPE>
inline TYPE& CArray<TYPE>::GetAtRef(int nIndex)
{ return m_pData[nIndex]; }

template<class TYPE>
inline TYPE* CArray<TYPE>::GetAtPtr(int nIndex)
{ return &m_pData[nIndex]; }

template<class TYPE>
inline void CArray<TYPE>::SetAt(int nIndex,TYPE newElement)
{ m_pData[nIndex]=newElement; }

template<class TYPE>
inline TYPE CArray<TYPE>::GetLast() const
{ return m_pData[m_nSize-1]; }

template<class TYPE>
inline TYPE& CArray<TYPE>::GetLastRef()
{ return m_pData[m_nSize-1]; }

template<class TYPE>
inline TYPE* CArray<TYPE>::GetLastPtr()
{ return &m_pData[m_nSize-1]; }

template<class TYPE>
inline void CArray<TYPE>::SetLast(TYPE newElement)
{ m_pData[m_nSize-1]=newElement; }

template<class TYPE>
inline TYPE& CArray<TYPE>::ElementAt(int nIndex)
{	return m_pData[nIndex];  }

template<class TYPE>
inline const TYPE* CArray<TYPE>::GetData() const
{ return m_pData; }

template<class TYPE>
inline TYPE* CArray<TYPE>::GetData()
{ return m_pData; }

template<class TYPE>
inline CArray<TYPE>::operator TYPE*()
{ return m_pData; }

template<class TYPE>
inline CArray<TYPE>::operator const TYPE*() const
{ return m_pData; }

template<class TYPE>
int CArray<TYPE>::Add(TYPE newElement)
{
	if (m_pData==NULL)
	{
		m_pData=new TYPE[2];
		if (m_pData==NULL)
		{
			if (m_bThrow)
				throw CException(CException::cannotAllocate);
			else
				SetHFCError(HFC_CANNOTALLOC);
			return 0;
		}
		m_pData[0]=newElement;
		m_nSize=1;
		return 0;
	}
	TYPE* temp=m_pData;
	m_pData=new TYPE[max(m_nSize+1,2)];
	if (m_pData==NULL)
	{
		SetHFCError(HFC_CANNOTALLOC);
		return 0;
	}
	MemCopy(m_pData,temp,m_nSize*sizeof(TYPE));
	delete[] temp;
	m_pData[m_nSize]=newElement;
	return m_nSize++;
}

template<class TYPE>
int CArray<TYPE>::Append(const CArray& src)
{
	if (src.m_pData==NULL)
		return 0;
	if (m_pData==NULL)
	{
		m_pData=new TYPE[max(src.m_nSize,2)];
		if (m_pData==NULL)
		{
			if (m_bThrow)
				throw CException(CException::cannotAllocate);
			else
				SetHFCError(HFC_CANNOTALLOC);
			return;
		}
		MemCopy(m_pData,src.m_pData,src.m_nSize*sizeof(TYPE));
		return m_nSize=src.m_nSize;
	}
	TYPE* newarray=new TYPE[m_nSize+src.m_nSize];
	if (newarray==NULL)
	{
		if (m_bThrow)
			throw CException(CException::cannotAllocate);
		else
			SetHFCError(HFC_CANNOTALLOC);
		return;
	}
	MemCopy(newarray,m_pData,m_nSize*sizeof(TYPE));
	MemCopy(&newarray[m_nSize],src.m_pData,src.m_nSize*sizeof(TYPE));
	delete [] m_pData;
	m_pData=newarray;
	return (m_nSize+=src.m_nSize);
}

template<class TYPE>
void CArray<TYPE>::Copy(const CArray& src)
{
	if (m_pData!=NULL)
		delete[] m_pData;
	if (src.m_pData==NULL)
	{
		m_pData=NULL;
		m_nSize=0;
		return;
	}
	m_pData=new TYPE[max(src.m_nSize,2)];
	if (m_pData==NULL)
	{
		if (m_bThrow)
			throw CException(CException::cannotAllocate);
		else
			SetHFCError(HFC_CANNOTALLOC);
		return;
	}
	MemCopy(m_pData,src.m_pData,src.m_nSize*sizeof(TYPE));
	return m_nSize=src.m_nSize;
}

template<class TYPE>
inline TYPE CArray<TYPE>::operator[](int nIndex) const
{ return m_pData[nIndex]; }

template<class TYPE>
inline TYPE& CArray<TYPE>::operator[](int nIndex)
{ return m_pData[nIndex]; }

template<class TYPE>
inline TYPE CArray<TYPE>::operator[](DWORD nIndex) const
{ return m_pData[nIndex]; }

template<class TYPE>
inline TYPE& CArray<TYPE>::operator[](DWORD nIndex)
{ return m_pData[nIndex]; }

template<class TYPE>
void CArray<TYPE>::InsertAt(int nIndex,TYPE newElement,int nCount)
{
	if (!nCount)
		return;
	if (m_pData==NULL)
	{
		m_pData=new TYPE[nCount+2];
		if (m_pData==NULL)
		{
			if (m_bThrow)
				throw CException(CException::cannotAllocate);
			else
				SetHFCError(HFC_CANNOTALLOC);
			return;
		}
		for (int i=0;i<nCount;i++)
			m_pData[i]=newElement;
		m_nSize=nCount;
		return;
	}
	if (nIndex>m_nSize)
		nIndex=m_nSize;
	TYPE* newarray=new TYPE[m_nSize+nCount];
	if (newarray==NULL)
	{
		if (m_bThrow)
			throw CException(CException::cannotAllocate);
		else
			SetHFCError(HFC_CANNOTALLOC);
		return;
	}
	
	MemCopy(newarray,m_pData,nIndex*sizeof(TYPE));
	for (int i=0;i<nCount;i++)
		newarray[i+nIndex]=newElement;
	MemCopy(&newarray[nIndex+nCount],&m_pData[nIndex],(m_nSize-nIndex)*sizeof(TYPE));
	m_nSize+=nCount;
	delete[] m_pData;
	m_pData=newarray;
}

template<class TYPE>
void CArray<TYPE>::RemoveAt(int nIndex)
{
	int i;
	for (i=nIndex;i<m_nSize-1;i++)
		m_pData[i]=m_pData[i+1];
	m_nSize--;
}

template<class TYPE>
void CArray<TYPE>::InsertAt(int nStartIndex,CArray* pNewArray)
{
	if (pNewArray==NULL)
		return;
	if (pNewArrat->m_pData==NULL)
		return;
	if (m_pData==NULL)
	{
		m_pData=new TYPE[m_nSize=pNewArray->m_nSize];
		if (m_pData==NULL)
		{
			if (m_bThrow)
				throw CException(CException::cannotAllocate);
			else
				SetHFCError(HFC_CANNOTALLOC);
			return;
		}
		MemCopy(m_pData,pNewArray->m_pData,m_nSize*sizeof(TYPE));
		return;
	}
	if (nStartIndex>m_nSize)
		nStartIndex=m_nSize;
	TYPE* newarray=new TYPE[m_nSize+pNewArray->m_nSize];
	if (newarray==NULL)
	{
		if (m_bThrow)
			throw CException(CException::cannotAllocate);
		else
			SetHFCError(HFC_CANNOTALLOC);
		return;
	}
	MemCopy(newarray,m_pData,nIndex*sizeof(TYPE));
	MemCopy(&newarrat[nIndex],pNewArray->m_pData,pNewArray->m_nSize*sizeof(TYPE));
	MemCopy(&newarray[nIndex+pNewArray->m_nSize],&m_pData[nIndex],(m_nSize-nIndex)*sizeof(TYPE));
	m_nSize+=m_pNewArray->m_nSize;
	delete[] m_pData;
	m_pData=newarray;
	return m_nSize;
}

template<class TYPE>
int CArray<TYPE>::Find(TYPE element,int startat) const
{
	for (;startat<m_nSize;startat++)
	{
		if (m_pData[startat]==element)
			return startat;
	}
	return -1;
}
	
template<class TYPE>
void CArray<TYPE>::Swap(CArray& src)
{
	union{
		TYPE* temp;
		DWORD len;
	} tmp;
	tmp.temp=m_pData;
	m_pData=src.m_pData;
	src.m_pData=tmp.temp;

	tmp.len=m_nSize;
	m_nSize=src.m_nSize;
	src.m_nSize=tmp.len;
}

template<class TYPE>
inline TYPE* CArray<TYPE>::GiveBuffer()
{
	TYPE* pRet=m_pData;
	m_pData=NULL;
	m_nSize=0;
	return pRet;
}


template<class TYPE>
class CArrayFP : public CArray<TYPE>
{
public:
	CArrayFP();
	~CArrayFP();
	
	void Copy(const CArrayFP& src);

	void RemoveAll();
	void RemoveAt(int nIndex);

	void Swap(CArrayFP& src);
};

// Array functions

template<class TYPE>
inline CArrayFP<TYPE>::CArrayFP()
:	CArray<TYPE>()
{
}

template<class TYPE>
void CArrayFP<TYPE>::RemoveAll()
{
	if (m_pData!=NULL)
	{
		for (int i=0;i<m_nSize;i++)
		{
			if (m_pData[i]!=NULL)
				delete m_pData[i];
		}
		delete[] m_pData;
	}
	m_pData=NULL;
	m_nSize=0;
}

template<class TYPE>
CArrayFP<TYPE>::~CArrayFP()
{
	RemoveAll();
}




template<class TYPE>
void CArrayFP<TYPE>::Copy(const CArrayFP& src)
{
	if (m_pData!=NULL)
	{
		for (int i=0;i<m_nSize;i++)
			delete m_pData[i];
		delete[] m_pData;
	}
	
	if (src.m_pData==NULL)
	{
		m_pData=NULL;
		m_nSize=0;
		return;
	}
	m_pData=new TYPE[max(src.m_nSize,2)];
	if (m_pData==NULL)
	{
		if (m_bThrow)
			throw CException(CException::cannotAllocate);
		else
			SetHFCError(HFC_CANNOTALLOC);
		return;
	}
	MemCopy(m_pData,src.m_pData,src.m_nSize*sizeof(TYPE));
	return m_nSize=src.m_nSize;
}


template<class TYPE>
void CArrayFP<TYPE>::RemoveAt(int nIndex)
{
	int i;
	if (m_pData[nIndex]!=NULL)
		delete m_pData[nIndex];
	
	for (i=nIndex;i<m_nSize-1;i++)
		m_pData[i]=m_pData[i+1];
	
	m_nSize--;
}

	
template<class TYPE>
void CArrayFP<TYPE>::Swap(CArrayFP& src)
{
	union{
		TYPE* temp;
		DWORD len;
	} tmp;
	tmp.temp=m_pData;
	m_pData=src.m_pData;
	src.m_pData=tmp.temp;

	tmp.len=m_nSize;
	m_nSize=src.m_nSize;
	src.m_nSize=tmp.len;
}


template<class TYPE>
class CArrayFAP : public CArray<TYPE>
{
public:
	CArrayFAP();
	~CArrayFAP();

	void Copy(const CArrayFAP& src);

	void RemoveAll();
	void RemoveAt(int nIndex);
	
	void Swap(CArrayFAP& src);

};

// Array functions

template<class TYPE>
inline CArrayFAP<TYPE>::CArrayFAP()
:	CArray<TYPE>()
{
}

template<class TYPE>
void CArrayFAP<TYPE>::RemoveAll()
{
	if (m_pData!=NULL)
	{
		for (int i=0;i<m_nSize;i++)
		{
			if (m_pData[i]!=NULL)
				delete[] m_pData[i];
		}
		delete[] m_pData;
	}
	m_pData=NULL;
	m_nSize=0;
}

template<class TYPE>
CArrayFAP<TYPE>::~CArrayFAP()
{
	RemoveAll();
}



template<class TYPE>
void CArrayFAP<TYPE>::Copy(const CArrayFAP& src)
{
	if (m_pData!=NULL)
	{
		for (int i=0;i<m_nSize;i++)
			delete[] m_pData[i];
		delete[] m_pData;
	}
	if (src.m_pData==NULL)
	{
		m_pData=NULL;
		m_nSize=0;
		return;
	}
	m_pData=new TYPE[max(src.m_nSize,2)];
	if (m_pData==NULL)
	{
		if (m_bThrow)
			throw CException(CException::cannotAllocate);
		else
			SetHFCError(HFC_CANNOTALLOC);
		return;
	}
	MemCopy(m_pData,src.m_pData,src.m_nSize*sizeof(TYPE));
	return m_nSize=src.m_nSize;
}

template<class TYPE>
void CArrayFAP<TYPE>::RemoveAt(int nIndex)
{
	int i;
	if (m_pData[nIndex]!=NULL)
		delete[] m_pData[nIndex];
	
	for (i=nIndex;i<m_nSize-1;i++)
		m_pData[i]=m_pData[i+1];
	
	m_nSize--;
}
	
template<class TYPE>
void CArrayFAP<TYPE>::Swap(CArrayFAP& src)
{
	union{
		TYPE* temp;
		DWORD len;
	} tmp;
	tmp.temp=m_pData;
	m_pData=src.m_pData;
	src.m_pData=tmp.temp;

	tmp.len=m_nSize;
	m_nSize=src.m_nSize;
	src.m_nSize=tmp.len;
}


// Array classes
#ifdef HFCLIB
class CCharArray	: public CArray<CHAR>		{};
class CByteArray	: public CArray<BYTE>		{};
class CShortArray	: public CArray<SHORT>		{};
class CWordArray	: public CArray<WORD>		{};
class CDWordArray	: public CArray<DWORD>		{};
class CIntArray		: public CArray<INT>		{};
class CUIntArray	: public CArray<UINT>		{};
class CStringArray	: public CArray<CString>	{};
class CObArray		: public CArray<CObject>	{};
class CPtrArray		: public CArray<LPVOID>	{};
#endif

#endif
