////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////


#ifndef HFCLIST_H
#define HFCLIST_H

////////////////////////////////////////////////////////////
// CList

template<class TYPE>
class CList : public CExceptionObject
{
protected:
	struct CNode
	{
		CNode* pNext;
		CNode* pPrev;
		TYPE data;
	};
public:
	CList();
	~CList();

	int GetCount() const;
	int GetSizeOfType() const { return sizeof(TYPE); }

	BOOL IsEmpty() const;
	
	TYPE& GetHead();
	TYPE GetHead() const;
	TYPE& GetTail();
	TYPE GetTail() const;

	TYPE RemoveHead();
	TYPE RemoveTail();

	POSITION AddHead(TYPE newElement);
	POSITION AddTail(TYPE newElement);

	void RemoveAll();

	POSITION GetHeadPosition() const;
	POSITION GetTailPosition() const;
	static POSITION GetNextPosition(POSITION pPosition);
	static POSITION GetPrevPosition(POSITION pPosition);

	TYPE& GetNext(POSITION pPosition); 
	TYPE GetNext(POSITION pPosition) const;
	TYPE& GetPrev(POSITION pPosition); 
	TYPE GetPrev(POSITION pPosition) const; 

	static TYPE& GetAtRef(POSITION position);
	static TYPE* GetAtPtr(POSITION position);
	static TYPE GetAt(POSITION position);
	static void SetAt(POSITION pos,TYPE newElement);
	void RemoveAt(POSITION position);

	POSITION InsertBefore(POSITION position,TYPE newElement);
	POSITION InsertAfter(POSITION position,TYPE newElement);

	POSITION Find(TYPE searchValue,POSITION startAfter=NULL) const;
	POSITION FindIndex(int nIndex) const;
	
	void Swap(CList<TYPE>& lst);

protected:
	CNode* m_pNodeHead;
	CNode* m_pNodeTail;
	int m_nCount;
};

////////////////////////////////////////////////
// CListFP


template<class TYPE>
class CListFP : public CList<TYPE> // Uses delete to free pointers
{
public:
	~CListFP();

	void RemoveHead();
	void RemoveTail();

	void RemoveAll();
	void RemoveAt(POSITION position);

};


////////////////////////////////////////////////
// CListFAP

template<class TYPE>
class CListFAP : public CList<TYPE> // Uses delete[] to free pointers
{
public:
	~CListFAP();

	void RemoveHead();
	void RemoveTail();

	void RemoveAll();
	void RemoveAt(POSITION position);

};

////////////////////////////////////////////////////////////
// CMDIChildWndList

#ifdef DEF_RESOURCES
class CMDIChildWndList
{
protected:
	struct CNode
	{
		CNode* pNext;
		CNode* pPrev;
		CMDIChildWnd* pWnd;
	};
public:
	CMDIChildWndList();
	~CMDIChildWndList();
	
	int GetCount() const { return m_nCount; }
	BOOL IsEmpty() const { return (m_pNodeHead==NULL); }
	
	POSITION AddWindow(CMDIChildWnd* pWnd);
	BYTE RemoveWindow(POSITION pPos);
	BYTE RemoveAllWindows();
	static CMDIChildWnd* GetWindow(POSITION pPos) { return ((CNode*)pPos)->pWnd; }
	
	POSITION FindWindow(CMDIChildWnd* pWnd) const;
	POSITION FindWindow(HWND hWnd) const;

	POSITION GetFirstPosition() const { return (POSITION)m_pNodeHead; }
	POSITION GetLastPosition() const { return (POSITION)m_pNodeTail; }
	static POSITION GetNextPosition(POSITION pPos) { return (POSITION)(((CNode*)pPos)->pNext); }
	static POSITION GetPrevPosition(POSITION pPos) { return (POSITION)(((CNode*)pPos)->pPrev); }

protected:
	CNode* m_pNodeHead;
	CNode* m_pNodeTail;
	int m_nCount;
};
#endif

////////////////////////////////////////////////////////////
// CMap

template<class LABELTYPE,class TYPE>
class CMap : public CExceptionObject
{
protected:
	struct CNode
	{
		CNode* pNext;
		CNode* pPrev;
		LABELTYPE label;
		TYPE data;
	};
public:
	CMap();
	~CMap();

	int GetCount() const;
	int GetSizeOfType() const { return sizeof(TYPE); }

	BOOL IsEmpty() const;
	
	TYPE& GetHead();
	TYPE GetHead() const;
	TYPE& GetTail();
	TYPE GetTail() const;

	TYPE RemoveHead();
	TYPE RemoveTail();

	POSITION AddHead(LABELTYPE newLabel,TYPE newElement);
	POSITION AddTail(LABELTYPE newLabel,TYPE newElement);

	void RemoveAll();

	POSITION GetHeadPosition() const;
	POSITION GetTailPosition() const;
	static POSITION GetNextPosition(POSITION& rPosition);
	static POSITION GetPrevPosition(POSITION& rPosition);

	TYPE& GetNext(POSITION& rPosition); 
	TYPE GetNext(POSITION& rPosition) const;
	TYPE& GetPrev(POSITION& rPosition); 
	TYPE GetPrev(POSITION& rPosition) const; 

	TYPE& GetRef(LABELTYPE label);
	TYPE* GetPtr(LABELTYPE label);
	TYPE Get(LABELTYPE label);
	BOOL Set(LABELTYPE label,TYPE newElement);
	BOOL Remove(LABELTYPE label);
	
	static LABELTYPE GetLabel(POSITION position);
	static TYPE& GetAtRef(POSITION position);
	static TYPE* GetAtPtr(POSITION position);
	static TYPE GetAt(POSITION position);
	static void SetAt(POSITION pos,TYPE newElement);
	void RemoveAt(POSITION position);

	POSITION InsertBefore(POSITION position,LABELTYPE newLabel,TYPE newElement);
	POSITION InsertBefore(LABELTYPE label,LABELTYPE newLabel,TYPE newElement);
	POSITION InsertAfter(POSITION position,LABELTYPE newLabel,TYPE newElement);
	POSITION InsertAfter(LABELTYPE label,LABELTYPE newLabel,TYPE newElement);

	POSITION Find(LABELTYPE searchLabel,POSITION startAfter=NULL) const;
	POSITION Find(TYPE searchValue,POSITION startAfter=NULL) const;
	POSITION FindIndex(int nIndex) const;
	
	void Swap(CMap<LABELTYPE,TYPE>& lst);

protected:
	CNode* m_pNodeHead;
	CNode* m_pNodeTail;
	int m_nCount;
};

////////////////////////////////////////////////////////////
// CMapFP

template<class LABELTYPE,class TYPE>
class CMapFP : public CMap<LABELTYPE,TYPE>
{
public:
	~CMapFP();

	TYPE RemoveHead();
	TYPE RemoveTail();

	void RemoveAll();

	BOOL Remove(LABELTYPE label);
	void RemoveAt(POSITION position);
};

////////////////////////////////////////////////////////////
// CMapFAP

template<class LABELTYPE,class TYPE>
class CMapFAP : public CMap<LABELTYPE,TYPE>
{

public:
	~CMapFAP();

	TYPE RemoveHead();
	TYPE RemoveTail();

	void RemoveAll();

	BOOL Remove(LABELTYPE label);
	void RemoveAt(POSITION position);
};

////////////////////////////////////////////////////////////
// CStringMap

template<class CHARTYPE,class TYPE>
class CStringMap : public CMap<CHARTYPE*,TYPE>
{
public:
	~CStringMap();

	TYPE RemoveHead();
	TYPE RemoveTail();

	POSITION AddHead(const CHARTYPE* newLabel,TYPE newElement);
	POSITION AddTail(const CHARTYPE* newLabel,TYPE newElement);

	TYPE& GetRef(const CHARTYPE* label);
	TYPE* GetPtr(const CHARTYPE* label);
	TYPE Get(const CHARTYPE* label);
	BOOL Set(const CHARTYPE* label,TYPE newElement);

	void RemoveAll();
	BOOL Remove(const CHARTYPE* label);
	void RemoveAt(POSITION position);

	POSITION InsertBefore(POSITION position,const CHARTYPE* newLabel,TYPE newElement);
	POSITION InsertBefore(const CHARTYPE* label,const CHARTYPE* newLabel,TYPE newElement);
	POSITION InsertAfter(POSITION position,const CHARTYPE* newLabel,TYPE newElement);
	POSITION InsertAfter(const CHARTYPE* label,const CHARTYPE* newLabel,TYPE newElement);

	POSITION Find(const CHARTYPE* label,POSITION startAfter=NULL) const;
	
private:
	static BOOL labelcmp(const CHARTYPE* label1,const CHARTYPE* label2);
	static CHARTYPE* alloclabel(const CHARTYPE* label);
};

////////////////////////////////////////////////////////////
// CStringMapFP

template<class CHARTYPE,class TYPE>
class CStringMapFP : public CStringMap<CHARTYPE,TYPE>
{
public:
	~CStringMapFP();

	TYPE RemoveHead();
	TYPE RemoveTail();

	
	void RemoveAll();
	BOOL Remove(const CHARTYPE* label);
	void RemoveAt(POSITION position);
};

template<class CHARTYPE,class TYPE>
class CStringMapFAP : public CStringMap<CHARTYPE,TYPE>
{
public:
	~CStringMapFAP();

	TYPE RemoveHead();
	TYPE RemoveTail();

	
	void RemoveAll();
	BOOL Remove(const CHARTYPE* label);
	void RemoveAt(POSITION position);
};

////////////////////////////////////////////////
// CList

template<class TYPE>
CList<TYPE>::CList()
{
	m_pNodeHead=NULL;
	m_pNodeTail=NULL;
	m_nCount=0;
}

template<class TYPE>
CList<TYPE>::~CList()
{
	RemoveAll();
}

template<class TYPE>
inline int CList<TYPE>::GetCount() const
{ return m_nCount; }

template<class TYPE>
inline BOOL CList<TYPE>::IsEmpty() const
{ return (m_pNodeHead==NULL); }

template<class TYPE>
TYPE& CList<TYPE>::GetHead()
{ 
	if (m_pNodeHead!=NULL)
		return m_pNodeHead->data;
	else
	{
		TYPE *temp=NULL;
		return *temp;
	}
}

template<class TYPE>
TYPE CList<TYPE>::GetHead() const
{ 
	if (m_pNodeHead!=NULL)
		return m_pNodeHead->data;
	else
		return 0;
}

template<class TYPE>
TYPE& CList<TYPE>::GetTail()
{
	if (m_pNodeTail!=NULL)
		return m_pNodeTail->data;
	else
	{
		TYPE *temp=NULL;
		return *temp;
	}
}

template<class TYPE>
TYPE CList<TYPE>::GetTail() const
{
	if (m_pNodeTail!=NULL)
		return m_pNodeTail->data;
	else
		return 0;
}

template<class TYPE>
TYPE CList<TYPE>::RemoveHead()
{
	if (m_pNodeHead!=NULL)
	{
		TYPE ret;
		m_nCount--;
		if (m_pNodeHead==m_pNodeTail)
		{
			ret=m_pNodeHead->data;
			delete m_pNodeHead;
			m_pNodeHead=m_pNodeTail=NULL;
			return ret;
		}
		CNode* tmp=m_pNodeHead;
		m_pNodeHead=m_pNodeHead->pNext;
		m_pNodeHead->pPrev=NULL;
		ret=tmp->data;
		delete tmp;
		return ret;
	}
	return 0;
}

template<class TYPE>
TYPE CList<TYPE>::RemoveTail()
{
	if (m_pNodeHead!=NULL)
	{
		TYPE ret;
		m_nCount--;
		if (m_pNodeHead==m_pNodeTail)
		{
			ret=m_pNodeHead->data;
			delete m_pNodeHead;
			m_pNodeHead=m_pNodeTail=NULL;
			return ret;
		}
		CNode* tmp=m_pNodeTail;
		m_pNodeTail=m_pNodeTail->pPrev;
		m_pNodeTail->pNext=NULL;
		ret=tmp->data;
		delete tmp;
		return ret;
	}
	return 0;
}


template<class TYPE>
POSITION CList<TYPE>::AddHead(TYPE newElement)
{
	CNode* tmp=new CNode;
	if (tmp==NULL)
	{
		if (m_bThrow)
			throw CException(CException::cannotAllocate);
		else
			SetHFCError(HFC_CANNOTALLOC);
		return NULL;
	}
	tmp->data=newElement;
	tmp->pPrev=NULL;
	tmp->pNext=m_pNodeHead;
	if (m_pNodeHead!=NULL)
		m_pNodeHead->pPrev=tmp;
	else
		m_pNodeTail=tmp;
	m_pNodeHead=tmp;
	m_nCount++;
	return (POSITION) tmp;
}

template<class TYPE>
POSITION CList<TYPE>::AddTail(TYPE newElement)
{
	CNode* tmp=new CNode;
	if (tmp==NULL)
	{
		if (m_bThrow)
			throw CException(CException::cannotAllocate);
		else
			SetHFCError(HFC_CANNOTALLOC);
		return NULL;
	}
	tmp->data=newElement;
	tmp->pNext=NULL;
	tmp->pPrev=m_pNodeTail;
	if (m_pNodeTail!=NULL)
		m_pNodeTail->pNext=tmp;
	else
		m_pNodeHead=tmp;
	m_pNodeTail=tmp;
	m_nCount++;
	return (POSITION) tmp;
}

template<class TYPE>
void CList<TYPE>::RemoveAll()
{
	CNode* tmp;
	while(m_pNodeHead!=NULL)
	{
		tmp=m_pNodeHead;
		m_pNodeHead=tmp->pNext;
		delete tmp;
	}
	m_nCount=0;
	m_pNodeTail=NULL;
}

template<class TYPE>
inline POSITION CList<TYPE>::GetHeadPosition() const
{ return (POSITION) m_pNodeHead; }

template<class TYPE>
inline POSITION CList<TYPE>::GetTailPosition() const
{ return (POSITION) m_pNodeTail; }

template<class TYPE>
inline POSITION CList<TYPE>::GetNextPosition(POSITION rPosition)
{   return (POSITION)(((CNode*)rPosition)->pNext); }

template<class TYPE>
inline POSITION CList<TYPE>::GetPrevPosition(POSITION rPosition)
{   return (POSITION)(((CNode*)rPosition)->pPrev); }

template<class TYPE>
TYPE& CList<TYPE>::GetNext(POSITION rPosition)
{
	rPosition=(POSITION)(((CNode*)rPosition)->pNext);
	TYPE temp=0;
	return (rPosition==NULL?temp:((CNode*)rPosition)->data);
}

template<class TYPE>
TYPE CList<TYPE>::GetNext(POSITION rPosition) const
{
	rPosition=(POSITION)(((CNode*)rPosition)->pNext);
	return (rPosition==NULL?NULL:((CNode*)rPosition)->data);
}

template<class TYPE>
TYPE& CList<TYPE>::GetPrev(POSITION rPosition)
{
	rPosition=(POSITION)(((CNode*)rPosition)->pPrev);
	TYPE temp=0;
	return (rPosition==NULL?temp:((CNode*)rPosition)->data);
}

template<class TYPE>
TYPE CList<TYPE>::GetPrev(POSITION rPosition) const
{
	rPosition=(POSITION)(((CNode*)rPosition)->pPrev);
	return (rPosition==NULL?NULL:((CNode*)rPosition)->data);
}

template<class TYPE>
inline TYPE& CList<TYPE>::GetAtRef(POSITION position)
{ return ((CNode*)position)->data; }

template<class TYPE>
inline TYPE* CList<TYPE>::GetAtPtr(POSITION position)
{ return &((CNode*)position)->data; }

template<class TYPE>
inline TYPE CList<TYPE>::GetAt(POSITION position)
{ return ((CNode*)position)->data; }

template<class TYPE>
inline void CList<TYPE>::SetAt(POSITION pos,TYPE newElement)
{ ((CNode*)pos)->data=newElement; }

template<class TYPE>
void CList<TYPE>::RemoveAt(POSITION position)
{
	CNode* node=(CNode*)position;
	if (node->pNext!=NULL)
	{
		if (node->pPrev!=NULL)
		{
			node->pPrev->pNext=node->pNext;
			node->pNext->pPrev=node->pPrev;
		}
		else
		{
			m_pNodeHead=node->pNext;	
			node->pNext->pPrev=NULL;
		}
	}
	else
	{
		if (node->pPrev!=NULL)
		{
			m_pNodeTail=node->pPrev;
			node->pPrev->pNext=NULL;
		}
		else
		{
			m_pNodeHead=NULL;
			m_pNodeTail=NULL;
		}
	}
	m_nCount--;
	delete node;
}

template<class TYPE>
POSITION CList<TYPE>::InsertBefore(POSITION position,TYPE newElement)
{
	CNode* tmp=new CNode;
	if (tmp==NULL)
	{
		if (m_bThrow)
			throw CException(CException::cannotAllocate);
		else
			SetHFCError(HFC_CANNOTALLOC);
		return NULL;
	}
	tmp->data=newElement;
	tmp->pPrev=((CNode*)position)->pPrev;
	if (tmp->pPrev!=NULL)
		tmp->pPrev->pNext=tmp;
	else
		m_pNodeHead=tmp;
	tmp->pNext=((CNode*)position);
	((CNode*)position)->pPrev=tmp;
	m_nCount++;
	return (POSITION) tmp;
}

template<class TYPE>
POSITION CList<TYPE>::InsertAfter(POSITION position,TYPE newElement)
{
	CNode* tmp=new CNode;
	if (tmp==NULL)
	{
		if (m_bThrow)
			throw CException(CException::cannotAllocate);
		else
			SetHFCError(HFC_CANNOTALLOC);
		return NULL;
	}
	tmp->data=newElement;
	tmp->pNext=((CNode*)position)->pNext;
	if (tmp->pNext!=NULL)
		tmp->pNext->pPrev=tmp;
	else
		m_pNodeTail=tmp;
	tmp->pPrev=((CNode*)position);
	((CNode*)position)->pNext=tmp;
	m_nCount++;
	return (POSITION) tmp;
}

template<class TYPE>
POSITION CList<TYPE>::Find(TYPE searchValue,POSITION startAfter) const
{
	CNode* tmp;
	if ((CNode*)startAfter==NULL)
		tmp=m_pNodeHead;
	else
		tmp=((CNode*)startAfter)->pNext;
	while (tmp!=NULL)
	{
		if (tmp->data==searchValue)
			return (POSITION) tmp;
		tmp=tmp->pNext;
	}
	return NULL;
}

template<class TYPE>
POSITION CList<TYPE>::FindIndex(int nIndex) const
{
	if (nIndex>=m_nCount)
		return NULL;
	CNode *ptr=m_pNodeHead;
	for (int i=0;i<nIndex;i++)
		ptr=ptr->pNext;
	return (POSITION) ptr;
}

template<class TYPE>
void CList<TYPE>::Swap(CList<TYPE>& lst)
{
	union {
		CNode* pTmp;
		int nTmp;
	} tmp;
	tmp.pTmp=m_pNodeHead;
	m_pNodeHead=lst.m_pNodeHead;
	lst.m_pNodeHead=tmp.pTmp;

	tmp.pTmp=m_pNodeTail;
	m_pNodeTail=lst.m_pNodeTail;
	lst.m_pNodeTail=tmp.pTmp;

	tmp.nTmp=m_nCount;
	m_nCount=lst.m_nCount;
	lst.m_nCount=tmp.nTmp;
}

////////////////////////////////////////////////
// CListFP


template<class TYPE>
CListFP<TYPE>::~CListFP()
{
	RemoveAll();
}

template<class TYPE>
void CListFP<TYPE>::RemoveHead()
{
	if (m_pNodeHead!=NULL)
	{
		m_nCount--;
		if (m_pNodeHead==m_pNodeTail)
		{
			delete m_pNodeHead->data;
			delete m_pNodeHead;
			m_pNodeHead=m_pNodeTail=NULL;
			return;
		}
		CNode* tmp=m_pNodeHead;
		m_pNodeHead=m_pNodeHead->pNext;
		m_pNodeHead->pPrev=NULL;
		delete tmp->data;
		delete tmp;
	}
}

template<class TYPE>
void CListFP<TYPE>::RemoveTail()
{
	if (m_pNodeHead!=NULL)
	{
		m_nCount--;
		if (m_pNodeHead==m_pNodeTail)
		{
			delete m_pNodeHead->data;
			delete m_pNodeHead;
			m_pNodeHead=m_pNodeTail=NULL;
			return;
		}
		CNode* tmp=m_pNodeTail;
		m_pNodeTail=m_pNodeTail->pPrev;
		m_pNodeTail->pNext=NULL;
		delete tmp->data;
		delete tmp;
	}
	return 0;
}

template<class TYPE>
void CListFP<TYPE>::RemoveAll()
{
	CNode* tmp;
	while(m_pNodeHead!=NULL)
	{
		tmp=m_pNodeHead;
		m_pNodeHead=tmp->pNext;
		delete tmp->data;
		delete tmp;
	}
	m_nCount=0;
	m_pNodeTail=NULL;
}


template<class TYPE>
void CListFP<TYPE>::RemoveAt(POSITION position)
{
	CNode* node=(CNode*)position;
	if (node->pNext!=NULL)
	{
		if (node->pPrev!=NULL)
		{
			node->pPrev->pNext=node->pNext;
			node->pNext->pPrev=node->pPrev;
		}
		else
		{
			m_pNodeHead=node->pNext;	
			node->pNext->pPrev=NULL;
		}
	}
	else
	{
		if (node->pPrev!=NULL)
		{
			m_pNodeTail=node->pPrev;
			node->pPrev->pNext=NULL;
		}
		else
		{
			m_pNodeHead=NULL;
			m_pNodeTail=NULL;
		}
	}
	m_nCount--;
	delete node->data;
	delete node;
}

////////////////////////////////////////////////////////
// CListFAP

template<class TYPE>
CListFAP<TYPE>::~CListFAP()
{
	RemoveAll();
}

template<class TYPE>
void CListFAP<TYPE>::RemoveHead()
{
	if (m_pNodeHead!=NULL)
	{
		m_nCount--;
		if (m_pNodeHead==m_pNodeTail)
		{
			delete[] m_pNodeHead->data;
			delete m_pNodeHead;
			m_pNodeHead=m_pNodeTail=NULL;
			return;
		}
		CNode* tmp=m_pNodeHead;
		m_pNodeHead=m_pNodeHead->pNext;
		m_pNodeHead->pPrev=NULL;
		delete[] tmp->data;
		delete tmp;
	}
}

template<class TYPE>
void CListFAP<TYPE>::RemoveTail()
{
	if (m_pNodeHead!=NULL)
	{
		m_nCount--;
		if (m_pNodeHead==m_pNodeTail)
		{
			delete[] m_pNodeHead->data;
			delete m_pNodeHead;
			m_pNodeHead=m_pNodeTail=NULL;
			return;
		}
		CNode* tmp=m_pNodeTail;
		m_pNodeTail=m_pNodeTail->pPrev;
		m_pNodeTail->pNext=NULL;
		delete[] tmp->data;
		delete tmp;
	}
	return 0;
}

template<class TYPE>
void CListFAP<TYPE>::RemoveAll()
{
	CNode* tmp;
	while(m_pNodeHead!=NULL)
	{
		tmp=m_pNodeHead;
		m_pNodeHead=tmp->pNext;
		delete[] tmp->data;
		delete tmp;
	}
	m_nCount=0;
	m_pNodeTail=NULL;
}


template<class TYPE>
void CListFAP<TYPE>::RemoveAt(POSITION position)
{
	CNode* node=(CNode*)position;
	if (node->pNext!=NULL)
	{
		if (node->pPrev!=NULL)
		{
			node->pPrev->pNext=node->pNext;
			node->pNext->pPrev=node->pPrev;
		}
		else
		{
			m_pNodeHead=node->pNext;	
			node->pNext->pPrev=NULL;
		}
	}
	else
	{
		if (node->pPrev!=NULL)
		{
			m_pNodeTail=node->pPrev;
			node->pPrev->pNext=NULL;
		}
		else
		{
			m_pNodeHead=NULL;
			m_pNodeTail=NULL;
		}
	}
	m_nCount--;
	delete[] node->data;
	delete node;
}

////////////////////////////////////////////////
// CMap

template<class LABELTYPE,class TYPE>
CMap<LABELTYPE,TYPE>::CMap()
{
	m_pNodeHead=NULL;
	m_pNodeTail=NULL;
	m_nCount=0;
}

template<class LABELTYPE,class TYPE>
CMap<LABELTYPE,TYPE>::~CMap()
{
	RemoveAll();
}

template<class LABELTYPE,class TYPE>
inline int CMap<LABELTYPE,TYPE>::GetCount() const
{ return m_nCount; }

template<class LABELTYPE,class TYPE>
inline BOOL CMap<LABELTYPE,TYPE>::IsEmpty() const
{ return (m_pNodeHead==NULL); }

template<class LABELTYPE,class TYPE>
TYPE& CMap<LABELTYPE,TYPE>::GetHead()
{ 
	if (m_pNodeHead!=NULL)
		return m_pNodeHead->data;
	else
	{
		TYPE *temp=NULL;
		return *temp;
	}
}

template<class LABELTYPE,class TYPE>
TYPE CMap<LABELTYPE,TYPE>::GetHead() const
{ 
	if (m_pNodeHead!=NULL)
		return m_pNodeHead->data;
	else
		return 0;
}

template<class LABELTYPE,class TYPE>
TYPE& CMap<LABELTYPE,TYPE>::GetTail()
{
	if (m_pNodeTail!=NULL)
		return m_pNodeTail->data;
	else
	{
		TYPE *temp=NULL;
		return *temp;
	}
}

template<class LABELTYPE,class TYPE>
TYPE CMap<LABELTYPE,TYPE>::GetTail() const
{
	if (m_pNodeTail!=NULL)
		return m_pNodeTail->data;
	else
		return 0;
}

template<class LABELTYPE,class TYPE>
TYPE CMap<LABELTYPE,TYPE>::RemoveHead()
{
	if (m_pNodeHead!=NULL)
	{
		TYPE ret;
		m_nCount--;
		if (m_pNodeHead==m_pNodeTail)
		{
			ret=m_pNodeHead->data;
			delete m_pNodeHead;
			m_pNodeHead=m_pNodeTail=NULL;
			return ret;
		}
		CNode* tmp=m_pNodeHead;
		m_pNodeHead=m_pNodeHead->pNext;
		m_pNodeHead->pPrev=NULL;
		ret=tmp->data;
		delete tmp;
		return ret;
	}
	return 0;
}

template<class LABELTYPE,class TYPE>
TYPE CMap<LABELTYPE,TYPE>::RemoveTail()
{
	if (m_pNodeHead!=NULL)
	{
		TYPE ret;
		m_nCount--;
		if (m_pNodeHead==m_pNodeTail)
		{
			ret=m_pNodeHead->data;
			delete m_pNodeHead;
			m_pNodeHead=m_pNodeTail=NULL;
			return ret;
		}
		CNode* tmp=m_pNodeTail;
		m_pNodeTail=m_pNodeTail->pPrev;
		m_pNodeTail->pNext=NULL;
		ret=tmp->data;
		delete tmp;
		return ret;
	}
	return 0;
}


template<class LABELTYPE,class TYPE>
POSITION CMap<LABELTYPE,TYPE>::AddHead(LABELTYPE newLabel,TYPE newElement)
{
	CNode* tmp=new CNode;
	if (tmp==NULL)
	{
		if (m_bThrow)
			throw CException(CException::cannotAllocate);
		else
			SetHFCError(HFC_CANNOTALLOC);
		return NULL;
	}
	tmp->label=newLabel;
	tmp->data=newElement;
	tmp->pPrev=NULL;
	tmp->pNext=m_pNodeHead;
	if (m_pNodeHead!=NULL)
		m_pNodeHead->pPrev=tmp;
	else
		m_pNodeTail=tmp;
	m_pNodeHead=tmp;
	m_nCount++;
	return (POSITION) tmp;
}

template<class LABELTYPE,class TYPE>
POSITION CMap<LABELTYPE,TYPE>::AddTail(LABELTYPE newLabel,TYPE newElement)
{
	CNode* tmp=new CNode;
	if (tmp==NULL)
	{
		if (m_bThrow)
			throw CException(CException::cannotAllocate);
		else
			SetHFCError(HFC_CANNOTALLOC);
		return NULL;
	}
	tmp->label=newLabel;
	tmp->data=newElement;
	tmp->pNext=NULL;
	tmp->pPrev=m_pNodeTail;
	if (m_pNodeTail!=NULL)
		m_pNodeTail->pNext=tmp;
	else
		m_pNodeHead=tmp;
	m_pNodeTail=tmp;
	m_nCount++;
	return (POSITION) tmp;
}



template<class LABELTYPE,class TYPE>
void CMap<LABELTYPE,TYPE>::RemoveAll()
{
	CNode* tmp;
	while(m_pNodeHead!=NULL)
	{
		tmp=m_pNodeHead;
		m_pNodeHead=tmp->pNext;
		delete tmp;
	}
	m_nCount=0;
	m_pNodeTail=NULL;
}

template<class LABELTYPE,class TYPE>
inline POSITION CMap<LABELTYPE,TYPE>::GetHeadPosition() const
{ return (POSITION) m_pNodeHead; }

template<class LABELTYPE,class TYPE>
inline POSITION CMap<LABELTYPE,TYPE>::GetTailPosition() const
{ return (POSITION) m_pNodeTail; }

template<class LABELTYPE,class TYPE>
inline POSITION CMap<LABELTYPE,TYPE>::GetNextPosition(POSITION& rPosition)
{   return (POSITION)(((CNode*)rPosition)->pNext); }

template<class LABELTYPE,class TYPE>
inline POSITION CMap<LABELTYPE,TYPE>::GetPrevPosition(POSITION& rPosition)
{   return (POSITION)(((CNode*)rPosition)->pPrev); }

template<class LABELTYPE,class TYPE>
TYPE& CMap<LABELTYPE,TYPE>::GetNext(POSITION& rPosition)
{
	rPosition=(POSITION)(((CNode*)rPosition)->pNext);
	TYPE temp=0;
	return (rPosition==NULL?temp:((CNode*)rPosition)->data);
}

template<class LABELTYPE,class TYPE>
TYPE CMap<LABELTYPE,TYPE>::GetNext(POSITION& rPosition) const
{
	rPosition=(POSITION)(((CNode*)rPosition)->pNext);
	return (rPosition==NULL?NULL:((CNode*)rPosition)->data);
}

template<class LABELTYPE,class TYPE>
inline TYPE& CMap<LABELTYPE,TYPE>::GetPrev(POSITION& rPosition)
{
	rPosition=(POSITION)(((CNode*)rPosition)->pPrev);
	TYPE temp=0;
	return (rPosition==NULL?temp:((CNode*)rPosition)->data);
}

template<class LABELTYPE,class TYPE>
inline TYPE CMap<LABELTYPE,TYPE>::GetPrev(POSITION& rPosition) const
{
	rPosition=(POSITION)(((CNode*)rPosition)->pPrev);
	return (rPosition==NULL?NULL:((CNode*)rPosition)->data);
}

template<class LABELTYPE,class TYPE>
inline TYPE& CMap<LABELTYPE,TYPE>::GetRef(LABELTYPE label)
{
	POSITION position=Find(label);
	if (position==NULL)
		return *((TYPE*)NULL);
	return ((CNode*)position)->data; 
}

template<class LABELTYPE,class TYPE>
inline TYPE* CMap<LABELTYPE,TYPE>::GetPtr(LABELTYPE label)
{ 
	POSITION position=Find(label);
	if (position==NULL)
		return NULL;
	return &((CNode*)position)->data;
}

template<class LABELTYPE,class TYPE>
inline TYPE CMap<LABELTYPE,TYPE>::Get(LABELTYPE label)
{ 
	POSITION position=Find(label);
	if (position==NULL)
		return (TYPE)NULL;
	return ((CNode*)position)->data; 
}

template<class LABELTYPE,class TYPE>
inline BOOL CMap<LABELTYPE,TYPE>::Set(LABELTYPE label,TYPE newElement)
{ 
	POSITION position=Find(label);
	if (position==NULL)
		return FALSE;
	((CNode*)pos)->data=newElement; 
	return TRUE;
}
	
template<class LABELTYPE,class TYPE>
inline TYPE& CMap<LABELTYPE,TYPE>::GetAtRef(POSITION position)
{ return ((CNode*)position)->data; }

template<class LABELTYPE,class TYPE>
inline TYPE* CMap<LABELTYPE,TYPE>::GetAtPtr(POSITION position)
{ return &((CNode*)position)->data; }

template<class LABELTYPE,class TYPE>
inline TYPE CMap<LABELTYPE,TYPE>::GetAt(POSITION position)
{ return ((CNode*)position)->data; }

template<class LABELTYPE,class TYPE>
inline void CMap<LABELTYPE,TYPE>::SetAt(POSITION pos,TYPE newElement)
{ ((CNode*)pos)->data=newElement; }

template<class LABELTYPE,class TYPE>
inline BOOL CMap<LABELTYPE,TYPE>::Remove(LABELTYPE label)
{
	POSITION position=Find(label);
	if (position==NULL)
		return FALSE;
	RemoveAt(position);
	return TRUE;
}

template<class LABELTYPE,class TYPE>
void CMap<LABELTYPE,TYPE>::RemoveAt(POSITION position)
{
	CNode* node=(CNode*)position;
	if (node->pNext!=NULL)
	{
		if (node->pPrev!=NULL)
		{
			node->pPrev->pNext=node->pNext;
			node->pNext->pPrev=node->pPrev;
		}
		else
		{
			m_pNodeHead=node->pNext;	
			node->pNext->pPrev=NULL;
		}
	}
	else
	{
		if (node->pPrev!=NULL)
		{
			m_pNodeTail=node->pPrev;
			node->pPrev->pNext=NULL;
		}
		else
		{
			m_pNodeHead=NULL;
			m_pNodeTail=NULL;
		}
	}
	m_nCount--;
	delete node;
}

template<class LABELTYPE,class TYPE>
inline POSITION CMap<LABELTYPE,TYPE>::InsertBefore(LABELTYPE label,LABELTYPE newLabel,TYPE newElement)
{
	POSITION position=Find(label);
	if (position==NULL)
		return NULL;
	return InsertBefore(position,newLabel,newElement);
}


template<class LABELTYPE,class TYPE>
inline POSITION CMap<LABELTYPE,TYPE>::InsertAfter(LABELTYPE label,LABELTYPE newLabel,TYPE newElement)
{
	POSITION position=Find(label);
	if (position==NULL)
		return NULL;
	return InsertAfter(position,newLabel,newElement);
}

template<class LABELTYPE,class TYPE>
POSITION CMap<LABELTYPE,TYPE>::InsertBefore(POSITION position,LABELTYPE newLabel,TYPE newElement)
{
	CNode* tmp=new CNode;
	if (tmp==NULL)
	{
		if (m_bThrow)
			throw CException(CException::cannotAllocate);
		else
			SetHFCError(HFC_CANNOTALLOC);
		return NULL;
	}
	tmp->label=newLabel;
	tmp->data=newElement;
	tmp->pPrev=((CNode*)position)->pPrev;
	if (tmp->pPrev!=NULL)
		tmp->pPrev->pNext=tmp;
	else
		m_pNodeHead=tmp;
	tmp->pNext=((CNode*)position);
	((CNode*)position)->pPrev=tmp;
	m_nCount++;
	return (POSITION) tmp;
}

template<class LABELTYPE,class TYPE>
POSITION CMap<LABELTYPE,TYPE>::InsertAfter(POSITION position,LABELTYPE newLabel,TYPE newElement)
{
	CNode* tmp=new CNode;
	if (tmp==NULL)
	{
		if (m_bThrow)
			throw CException(CException::cannotAllocate);
		else
			SetHFCError(HFC_CANNOTALLOC);
		return NULL;
	}
	tmp->label=newLabel;
	tmp->data=newElement;
	tmp->pNext=((CNode*)position)->pNext;
	if (tmp->pNext!=NULL)
		tmp->pNext->pPrev=tmp;
	else
		m_pNodeTail=tmp;
	tmp->pPrev=((CNode*)position);
	((CNode*)position)->pNext=tmp;
	m_nCount++;
	return (POSITION) tmp;
}

template<class LABELTYPE,class TYPE>
POSITION CMap<LABELTYPE,TYPE>::Find(LABELTYPE searchLabel,POSITION startAfter) const
{
	CNode* tmp;
	if ((CNode*)startAfter==NULL)
		tmp=m_pNodeHead;
	else
		tmp=((CNode*)startAfter)->pNext;
	while (tmp!=NULL)
	{
		if (tmp->label==searchLabel)
			return (POSITION) tmp;
		tmp=tmp->pNext;
	}
	return NULL;
}

template<class LABELTYPE,class TYPE>
POSITION CMap<LABELTYPE,TYPE>::Find(TYPE searchValue,POSITION startAfter) const
{
	CNode* tmp;
	if ((CNode*)startAfter==NULL)
		tmp=m_pNodeHead;
	else
		tmp=((CNode*)startAfter)->pNext;
	while (tmp!=NULL)
	{
		if (tmp->data==searchValue)
			return (POSITION) tmp;
		tmp=tmp->pNext;
	}
	return NULL;
}

template<class LABELTYPE,class TYPE>
POSITION CMap<LABELTYPE,TYPE>::FindIndex(int nIndex) const
{
	if (nIndex>=m_nCount)
		return NULL;
	CNode *ptr=m_pNodeHead;
	for (int i=0;i<nIndex;i++)
		ptr=ptr->pNext;
	return (POSITION) ptr;
}

template<class LABELTYPE,class TYPE>
void CMap<LABELTYPE,TYPE>::Swap(CMap<LABELTYPE,TYPE>& lst)
{
	union {
		CNode* pTmp;
		int nTmp;
	} tmp;
	tmp.pTmp=m_pNodeHead;
	m_pNodeHead=lst.m_pNodeHead;
	lst.m_pNodeHead=tmp.pTmp;

	tmp.pTmp=m_pNodeTail;
	m_pNodeTail=lst.m_pNodeTail;
	lst.m_pNodeTail=tmp.pTmp;

	tmp.nTmp=m_nCount;
	m_nCount=lst.m_nCount;
	lst.m_nCount=tmp.nTmp;
}

////////////////////////////////////////////////
// CMapFP

template<class LABELTYPE,class TYPE>
CMapFP<LABELTYPE,TYPE>::~CMapFP()
{
	RemoveAll();
}

template<class LABELTYPE,class TYPE>
TYPE CMapFP<LABELTYPE,TYPE>::RemoveHead()
{
	if (m_pNodeHead!=NULL)
	{
		TYPE ret;
		m_nCount--;
		if (m_pNodeHead==m_pNodeTail)
		{
			ret=m_pNodeHead->data;
			delete m_pNodeHead->data;
			delete m_pNodeHead;
			m_pNodeHead=m_pNodeTail=NULL;
			return ret;
		}
		CNode* tmp=m_pNodeHead;
		m_pNodeHead=m_pNodeHead->pNext;
		m_pNodeHead->pPrev=NULL;
		ret=tmp->data;
		delete tmp->data;
		delete tmp;
		return ret;
	}
	return 0;
}

template<class LABELTYPE,class TYPE>
TYPE CMapFP<LABELTYPE,TYPE>::RemoveTail()
{
	if (m_pNodeHead!=NULL)
	{
		TYPE ret;
		m_nCount--;
		if (m_pNodeHead==m_pNodeTail)
		{
			ret=m_pNodeHead->data;
			delete m_pNodeHead->data;
			delete m_pNodeHead;
			m_pNodeHead=m_pNodeTail=NULL;
			return ret;
		}
		CNode* tmp=m_pNodeTail;
		m_pNodeTail=m_pNodeTail->pPrev;
		m_pNodeTail->pNext=NULL;
		ret=tmp->data;
		delete tmp->data;
		delete tmp;
		return ret;
	}
	return 0;
}




template<class LABELTYPE,class TYPE>
void CMapFP<LABELTYPE,TYPE>::RemoveAll()
{
	CNode* tmp;
	while(m_pNodeHead!=NULL)
	{
		tmp=m_pNodeHead;
		m_pNodeHead=tmp->pNext;
		delete tmp->data;
		delete tmp;
	}
	m_nCount=0;
	m_pNodeTail=NULL;
}

template<class LABELTYPE,class TYPE>
inline BOOL CMapFP<LABELTYPE,TYPE>::Remove(LABELTYPE label)
{
	POSITION position=Find(label);
	if (position==NULL)
		return FALSE;
	RemoveAt(position);
	return TRUE;
}

template<class LABELTYPE,class TYPE>
void CMapFP<LABELTYPE,TYPE>::RemoveAt(POSITION position)
{
	CNode* node=(CNode*)position;
	if (node->pNext!=NULL)
	{
		if (node->pPrev!=NULL)
		{
			node->pPrev->pNext=node->pNext;
			node->pNext->pPrev=node->pPrev;
		}
		else
		{
			m_pNodeHead=node->pNext;	
			node->pNext->pPrev=NULL;
		}
	}
	else
	{
		if (node->pPrev!=NULL)
		{
			m_pNodeTail=node->pPrev;
			node->pPrev->pNext=NULL;
		}
		else
		{
			m_pNodeHead=NULL;
			m_pNodeTail=NULL;
		}
	}
	m_nCount--;
	delete node->data;
	delete node;
}

////////////////////////////////////////////////
// CMapFAP

template<class LABELTYPE,class TYPE>
CMapFAP<LABELTYPE,TYPE>::~CMapFAP()
{
	RemoveAll();
}

template<class LABELTYPE,class TYPE>
TYPE CMapFAP<LABELTYPE,TYPE>::RemoveHead()
{
	if (m_pNodeHead!=NULL)
	{
		TYPE ret;
		m_nCount--;
		if (m_pNodeHead==m_pNodeTail)
		{
			ret=m_pNodeHead->data;
			delete[] m_pNodeHead->data;
			delete m_pNodeHead;
			m_pNodeHead=m_pNodeTail=NULL;
			return ret;
		}
		CNode* tmp=m_pNodeHead;
		m_pNodeHead=m_pNodeHead->pNext;
		m_pNodeHead->pPrev=NULL;
		ret=tmp->data;
		delete[] tmp->data;
		delete tmp;
		return ret;
	}
	return 0;
}

template<class LABELTYPE,class TYPE>
TYPE CMapFAP<LABELTYPE,TYPE>::RemoveTail()
{
	if (m_pNodeHead!=NULL)
	{
		TYPE ret;
		m_nCount--;
		if (m_pNodeHead==m_pNodeTail)
		{
			ret=m_pNodeHead->data;
			delete[] m_pNodeHead->data;
			delete m_pNodeHead;
			m_pNodeHead=m_pNodeTail=NULL;
			return ret;
		}
		CNode* tmp=m_pNodeTail;
		m_pNodeTail=m_pNodeTail->pPrev;
		m_pNodeTail->pNext=NULL;
		ret=tmp->data;
		delete[] tmp->data;
		delete tmp;
		return ret;
	}
	return 0;
}




template<class LABELTYPE,class TYPE>
void CMapFAP<LABELTYPE,TYPE>::RemoveAll()
{
	CNode* tmp;
	while(m_pNodeHead!=NULL)
	{
		tmp=m_pNodeHead;
		m_pNodeHead=tmp->pNext;
		delete[] tmp->data;
		delete tmp;
	}
	m_nCount=0;
	m_pNodeTail=NULL;
}

template<class LABELTYPE,class TYPE>
inline BOOL CMapFAP<LABELTYPE,TYPE>::Remove(LABELTYPE label)
{
	POSITION position=Find(label);
	if (position==NULL)
		return FALSE;
	RemoveAt(position);
	return TRUE;
}

template<class LABELTYPE,class TYPE>
void CMapFAP<LABELTYPE,TYPE>::RemoveAt(POSITION position)
{
	CNode* node=(CNode*)position;
	if (node->pNext!=NULL)
	{
		if (node->pPrev!=NULL)
		{
			node->pPrev->pNext=node->pNext;
			node->pNext->pPrev=node->pPrev;
		}
		else
		{
			m_pNodeHead=node->pNext;	
			node->pNext->pPrev=NULL;
		}
	}
	else
	{
		if (node->pPrev!=NULL)
		{
			m_pNodeTail=node->pPrev;
			node->pPrev->pNext=NULL;
		}
		else
		{
			m_pNodeHead=NULL;
			m_pNodeTail=NULL;
		}
	}
	m_nCount--;
	delete[] node->data;
	delete node;
}

////////////////////////////////////////////////
// CStringMap


template<class CHARTYPE,class TYPE>
CStringMap<CHARTYPE,TYPE>::~CStringMap()
{
	RemoveAll();
}


template<class CHARTYPE,class TYPE>
TYPE CStringMap<CHARTYPE,TYPE>::RemoveHead()
{
	if (m_pNodeHead!=NULL)
	{
		TYPE ret;
		m_nCount--;
		if (m_pNodeHead==m_pNodeTail)
		{
			ret=m_pNodeHead->data;
			delete[] m_pNodeHead->label;
			delete m_pNodeHead;
			m_pNodeHead=m_pNodeTail=NULL;
			return ret;
		}
		CNode* tmp=m_pNodeHead;
		m_pNodeHead=m_pNodeHead->pNext;
		m_pNodeHead->pPrev=NULL;
		ret=tmp->data;
		delete[] tmp->label;
		delete tmp;
		return ret;
	}
	return 0;
}

template<class CHARTYPE,class TYPE>
TYPE CStringMap<CHARTYPE,TYPE>::RemoveTail()
{
	if (m_pNodeHead!=NULL)
	{
		TYPE ret;
		m_nCount--;
		if (m_pNodeHead==m_pNodeTail)
		{
			ret=m_pNodeHead->data;
			delete[] m_pNodeHead->label;
			delete m_pNodeHead;
			m_pNodeHead=m_pNodeTail=NULL;
			return ret;
		}
		CNode* tmp=m_pNodeTail;
		m_pNodeTail=m_pNodeTail->pPrev;
		m_pNodeTail->pNext=NULL;
		ret=tmp->data;
		delete[] tmp->label;
		delete tmp;
		return ret;
	}
	return 0;
}


template<class CHARTYPE,class TYPE>
POSITION CStringMap<CHARTYPE,TYPE>::AddHead(const CHARTYPE* newLabel,TYPE newElement)
{
	CNode* tmp=new CNode;
	if (tmp==NULL)
	{
		if (m_bThrow)
			throw CException(CException::cannotAllocate);
		else
			SetHFCError(HFC_CANNOTALLOC);
		return NULL;
	}
	tmp->label=alloclabel(newLabel);
	tmp->data=newElement;
	tmp->pPrev=NULL;
	tmp->pNext=m_pNodeHead;
	if (m_pNodeHead!=NULL)
		m_pNodeHead->pPrev=tmp;
	else
		m_pNodeTail=tmp;
	m_pNodeHead=tmp;
	m_nCount++;
	return (POSITION) tmp;
}

template<class CHARTYPE,class TYPE>
POSITION CStringMap<CHARTYPE,TYPE>::AddTail(const CHARTYPE* newLabel,TYPE newElement)
{
	CNode* tmp=new CNode;
	if (tmp==NULL)
	{
		if (m_bThrow)
			throw CException(CException::cannotAllocate);
		else
			SetHFCError(HFC_CANNOTALLOC);
		return NULL;
	}
	tmp->label=alloclabel(newLabel);
	tmp->data=newElement;
	tmp->pNext=NULL;
	tmp->pPrev=m_pNodeTail;
	if (m_pNodeTail!=NULL)
		m_pNodeTail->pNext=tmp;
	else
		m_pNodeHead=tmp;
	m_pNodeTail=tmp;
	m_nCount++;
	return (POSITION) tmp;
}



template<class CHARTYPE,class TYPE>
void CStringMap<CHARTYPE,TYPE>::RemoveAll()
{
	CNode* tmp;
	while(m_pNodeHead!=NULL)
	{
		tmp=m_pNodeHead;
		m_pNodeHead=tmp->pNext;
		delete[] tmp->label;
		delete tmp;
	}
	m_nCount=0;
	m_pNodeTail=NULL;
}



template<class CHARTYPE,class TYPE>
inline TYPE& CStringMap<CHARTYPE,TYPE>::GetRef(const CHARTYPE* label)
{
	POSITION position=Find(label);
	if (position==NULL)
		return *((TYPE*)NULL);
	return ((CNode*)position)->data; 
}

template<class CHARTYPE,class TYPE>
inline TYPE* CStringMap<CHARTYPE,TYPE>::GetPtr(const CHARTYPE* label)
{ 
	POSITION position=Find(label);
	if (position==NULL)
		return NULL;
	return &((CNode*)position)->data;
}

template<class CHARTYPE,class TYPE>
inline TYPE CStringMap<CHARTYPE,TYPE>::Get(const CHARTYPE* label)
{ 
	POSITION position=Find(label);
	if (position==NULL)
		return (TYPE)NULL;
	return ((CNode*)position)->data; 
}

template<class CHARTYPE,class TYPE>
inline BOOL CStringMap<CHARTYPE,TYPE>::Set(const CHARTYPE* label,TYPE newElement)
{ 
	POSITION position=Find(label);
	if (position==NULL)
		return FALSE;
	((CNode*)pos)->data=newElement; 
	return TRUE;
}
	

template<class CHARTYPE,class TYPE>
inline BOOL CStringMap<CHARTYPE,TYPE>::Remove(const CHARTYPE* label)
{
	POSITION position=Find(label);
	if (position==NULL)
		return FALSE;
	RemoveAt(position);
	return TRUE;
}

template<class CHARTYPE,class TYPE>
void CStringMap<CHARTYPE,TYPE>::RemoveAt(POSITION position)
{
	CNode* node=(CNode*)position;
	if (node->pNext!=NULL)
	{
		if (node->pPrev!=NULL)
		{
			node->pPrev->pNext=node->pNext;
			node->pNext->pPrev=node->pPrev;
		}
		else
		{
			m_pNodeHead=node->pNext;	
			node->pNext->pPrev=NULL;
		}
	}
	else
	{
		if (node->pPrev!=NULL)
		{
			m_pNodeTail=node->pPrev;
			node->pPrev->pNext=NULL;
		}
		else
		{
			m_pNodeHead=NULL;
			m_pNodeTail=NULL;
		}
	}
	m_nCount--;
	delete[] node->label;
	delete node;
}

template<class CHARTYPE,class TYPE>
inline POSITION CStringMap<CHARTYPE,TYPE>::InsertBefore(const CHARTYPE* label,const CHARTYPE* newLabel,TYPE newElement)
{
	POSITION position=Find(label);
	if (position==NULL)
		return NULL;
	return InsertBefore(position,newLabel,newElement);
}


template<class CHARTYPE,class TYPE>
inline POSITION CStringMap<CHARTYPE,TYPE>::InsertAfter(const CHARTYPE* label,const CHARTYPE* newLabel,TYPE newElement)
{
	POSITION position=Find(label);
	if (position==NULL)
		return NULL;
	return InsertAfter(position,newLabel,newElement);
}

template<class CHARTYPE,class TYPE>
POSITION CStringMap<CHARTYPE,TYPE>::InsertBefore(POSITION position,const CHARTYPE* newLabel,TYPE newElement)
{
	CNode* tmp=new CNode;
	if (tmp==NULL)
	{
		if (m_bThrow)
			throw CException(CException::cannotAllocate);
		else
			SetHFCError(HFC_CANNOTALLOC);
		return NULL;
	}
	tmp->label=alloclabel(newLabel);
	tmp->data=newElement;
	tmp->pPrev=((CNode*)position)->pPrev;
	if (tmp->pPrev!=NULL)
		tmp->pPrev->pNext=tmp;
	else
		m_pNodeHead=tmp;
	tmp->pNext=((CNode*)position);
	((CNode*)position)->pPrev=tmp;
	m_nCount++;
	return (POSITION) tmp;
}

template<class CHARTYPE,class TYPE>
POSITION CStringMap<CHARTYPE,TYPE>::InsertAfter(POSITION position,const CHARTYPE* newLabel,TYPE newElement)
{
	CNode* tmp=new CNode;
	if (tmp==NULL)
	{
		if (m_bThrow)
			throw CException(CException::cannotAllocate);
		else
			SetHFCError(HFC_CANNOTALLOC);
		return NULL;
	}
	tmp->label=alloclabel(newLabel);
	tmp->data=newElement;
	tmp->pNext=((CNode*)position)->pNext;
	if (tmp->pNext!=NULL)
		tmp->pNext->pPrev=tmp;
	else
		m_pNodeTail=tmp;
	tmp->pPrev=((CNode*)position);
	((CNode*)position)->pNext=tmp;
	m_nCount++;
	return (POSITION) tmp;
}

template<class CHARTYPE,class TYPE>
POSITION CStringMap<CHARTYPE,TYPE>::Find(const CHARTYPE* searchLabel,POSITION startAfter) const
{
	CNode* tmp;
	if ((CNode*)startAfter==NULL)
		tmp=m_pNodeHead;
	else
		tmp=((CNode*)startAfter)->pNext;
	while (tmp!=NULL)
	{
		if (labelcmp(tmp->label,searchLabel))
			return (POSITION) tmp;
		tmp=tmp->pNext;
	}
	return NULL;
}




template<class CHARTYPE,class TYPE>
BOOL CStringMap<CHARTYPE,TYPE>::labelcmp(const CHARTYPE* label1,const CHARTYPE* label2)
{
	int i;
	for (i=0;label1[i]!='\0';i++)
	{
		if (label1[i]!=label2[i])
			return FALSE;
	}
	return label2[i]=='\0';
}

template<class CHARTYPE,class TYPE>
CHARTYPE* CStringMap<CHARTYPE,TYPE>::alloclabel(const CHARTYPE* label)
{
	// Count buffer len
	int len;
	for (len=0;label[len]!='\0';len++);
	len++;

	// Form copy
	CHARTYPE* copy=new CHARTYPE[len];
	CopyMemory(copy,label,len*sizeof(CHARTYPE));
	return copy;
}

////////////////////////////////////////////////
// CStringMapFP


template<class CHARTYPE,class TYPE>
CStringMapFP<CHARTYPE,TYPE>::~CStringMapFP()
{
	RemoveAll();
}


template<class CHARTYPE,class TYPE>
TYPE CStringMapFP<CHARTYPE,TYPE>::RemoveHead()
{
	if (m_pNodeHead!=NULL)
	{
		TYPE ret;
		m_nCount--;
		if (m_pNodeHead==m_pNodeTail)
		{
			ret=m_pNodeHead->data;
			delete[] m_pNodeHead->label;
			delete m_pNodeHead->data;
			delete m_pNodeHead;
			m_pNodeHead=m_pNodeTail=NULL;
			return ret;
		}
		CNode* tmp=m_pNodeHead;
		m_pNodeHead=m_pNodeHead->pNext;
		m_pNodeHead->pPrev=NULL;
		ret=tmp->data;
		delete[] tmp->label;
		delete tmp->data;
		delete tmp;
		return ret;
	}
	return 0;
}

template<class CHARTYPE,class TYPE>
TYPE CStringMapFP<CHARTYPE,TYPE>::RemoveTail()
{
	if (m_pNodeHead!=NULL)
	{
		TYPE ret;
		m_nCount--;
		if (m_pNodeHead==m_pNodeTail)
		{
			ret=m_pNodeHead->data;
			delete[] m_pNodeHead->label;
			delete m_pNodeHead->data;
			delete m_pNodeHead;
			m_pNodeHead=m_pNodeTail=NULL;
			return ret;
		}
		CNode* tmp=m_pNodeTail;
		m_pNodeTail=m_pNodeTail->pPrev;
		m_pNodeTail->pNext=NULL;
		ret=tmp->data;
		delete[] tmp->label;
		delete tmp;
		delete tmp->data;
		return ret;
	}
	return 0;
}

template<class CHARTYPE,class TYPE>
void CStringMapFP<CHARTYPE,TYPE>::RemoveAll()
{
	CNode* tmp;
	while(m_pNodeHead!=NULL)
	{
		tmp=m_pNodeHead;
		m_pNodeHead=tmp->pNext;
		delete[] tmp->label;
		delete tmp->data;
		delete tmp;
	}
	m_nCount=0;
	m_pNodeTail=NULL;
}

template<class CHARTYPE,class TYPE>
inline BOOL CStringMapFP<CHARTYPE,TYPE>::Remove(const CHARTYPE* label)
{
	POSITION position=Find(label);
	if (position==NULL)
		return FALSE;
	RemoveAt(position);
	return TRUE;
}

template<class CHARTYPE,class TYPE>
void CStringMapFP<CHARTYPE,TYPE>::RemoveAt(POSITION position)
{
	CNode* node=(CNode*)position;
	if (node->pNext!=NULL)
	{
		if (node->pPrev!=NULL)
		{
			node->pPrev->pNext=node->pNext;
			node->pNext->pPrev=node->pPrev;
		}
		else
		{
			m_pNodeHead=node->pNext;	
			node->pNext->pPrev=NULL;
		}
	}
	else
	{
		if (node->pPrev!=NULL)
		{
			m_pNodeTail=node->pPrev;
			node->pPrev->pNext=NULL;
		}
		else
		{
			m_pNodeHead=NULL;
			m_pNodeTail=NULL;
		}
	}
	m_nCount--;
	delete[] node->label;
	delete node->data;
	delete node;
}

////////////////////////////////////////////////
// CStringMapFAP


template<class CHARTYPE,class TYPE>
CStringMapFAP<CHARTYPE,TYPE>::~CStringMapFAP()
{
	RemoveAll();
}


template<class CHARTYPE,class TYPE>
TYPE CStringMapFAP<CHARTYPE,TYPE>::RemoveHead()
{
	if (m_pNodeHead!=NULL)
	{
		TYPE ret;
		m_nCount--;
		if (m_pNodeHead==m_pNodeTail)
		{
			ret=m_pNodeHead->data;
			delete[] m_pNodeHead->label;
			delete[] m_pNodeHead->data;
			delete m_pNodeHead;
			m_pNodeHead=m_pNodeTail=NULL;
			return ret;
		}
		CNode* tmp=m_pNodeHead;
		m_pNodeHead=m_pNodeHead->pNext;
		m_pNodeHead->pPrev=NULL;
		ret=tmp->data;
		delete[] tmp->label;
		delete[] tmp->data;
		delete tmp;
		return ret;
	}
	return 0;
}

template<class CHARTYPE,class TYPE>
TYPE CStringMapFAP<CHARTYPE,TYPE>::RemoveTail()
{
	if (m_pNodeHead!=NULL)
	{
		TYPE ret;
		m_nCount--;
		if (m_pNodeHead==m_pNodeTail)
		{
			ret=m_pNodeHead->data;
			delete[] m_pNodeHead->label;
			delete[] m_pNodeHead->data;
			delete m_pNodeHead;
			m_pNodeHead=m_pNodeTail=NULL;
			return ret;
		}
		CNode* tmp=m_pNodeTail;
		m_pNodeTail=m_pNodeTail->pPrev;
		m_pNodeTail->pNext=NULL;
		ret=tmp->data;
		delete[] tmp->label;
		delete[] tmp;
		delete tmp->data;
		return ret;
	}
	return 0;
}

template<class CHARTYPE,class TYPE>
void CStringMapFAP<CHARTYPE,TYPE>::RemoveAll()
{
	CNode* tmp;
	while(m_pNodeHead!=NULL)
	{
		tmp=m_pNodeHead;
		m_pNodeHead=tmp->pNext;
		delete[] tmp->label;
		delete[] tmp->data;
		delete tmp;
	}
	m_nCount=0;
	m_pNodeTail=NULL;
}

template<class CHARTYPE,class TYPE>
inline BOOL CStringMapFAP<CHARTYPE,TYPE>::Remove(const CHARTYPE* label)
{
	POSITION position=Find(label);
	if (position==NULL)
		return FALSE;
	RemoveAt(position);
	return TRUE;
}

template<class CHARTYPE,class TYPE>
void CStringMapFAP<CHARTYPE,TYPE>::RemoveAt(POSITION position)
{
	CNode* node=(CNode*)position;
	if (node->pNext!=NULL)
	{
		if (node->pPrev!=NULL)
		{
			node->pPrev->pNext=node->pNext;
			node->pNext->pPrev=node->pPrev;
		}
		else
		{
			m_pNodeHead=node->pNext;	
			node->pNext->pPrev=NULL;
		}
	}
	else
	{
		if (node->pPrev!=NULL)
		{
			m_pNodeTail=node->pPrev;
			node->pPrev->pNext=NULL;
		}
		else
		{
			m_pNodeHead=NULL;
			m_pNodeTail=NULL;
		}
	}
	m_nCount--;
	delete[] node->label;
	delete[] node->data;
	delete node;
}

class CObList			: public CList <CObject> {};
class CStringList		: public CList <CString> {};

#endif

