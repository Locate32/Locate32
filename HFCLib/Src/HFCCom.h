////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////
//						COM/OLE Definition
////////////////////////////////////////////////////////////////////


#ifndef HFCCOM_H
#define HFCCOM_H

#if defined (DEF_COM)

// Standart definitions

#undef INTERFACE
#define INTERFACE(ClassName,BaseClass) class DECLSPEC_NOVTABLE ClassName : public BaseClass

#define COMSTD_DECLARATION(ClassName)			\
	public: \
	ClassName(); \
	virtual ~ClassName(); \
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,void __RPC_FAR *__RPC_FAR *ppvObject); \
	virtual ULONG STDMETHODCALLTYPE AddRef(void); \
	virtual ULONG STDMETHODCALLTYPE Release(void); \
	void AutoDelete(BOOL bDelete=TRUE) { m_bAutoDelete=bDelete; } \
	BOOL m_bAutoDelete; \
	protected:\
	BOOL m_cRef; 

#define COMSTD_DECLAREQUERY() \
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,void __RPC_FAR *__RPC_FAR *ppvObject);

#define COMSTD_CONSTRUCTORIMPLEMENTATIONBEGIN(ClassName) \
	ClassName::ClassName() : m_cRef(0), m_bAutoDelete(FALSE) {
#define COMSTD_CONSTRUCTORIMPLEMENTATIONEND }

#define COMSTD_CONSTRUCTORIMPLEMENTATION(ClassName) \
	COMSTD_CONSTRUCTORIMPLEMENTATIONBEGIN(ClassName) \
	COMSTD_CONSTRUCTORIMPLEMENTATIONEND
	
#define COMSTD_DESTRUCTORIMPLEMENTATIONBEGIN(ClassName) \
	ClassName::~ClassName() { 
#define COMSTD_DESTRUCTORIMPLEMENTATIONEND }

#define COMSTD_DESTRUCTORIMPLEMENTATION(ClassName) \
	COMSTD_DESTRUCTORIMPLEMENTATIONBEGIN(ClassName) \
	COMSTD_DESTRUCTORIMPLEMENTATIONEND


#define COMSTD_REFIMPLEMENTATION(ClassName) \
	ULONG STDMETHODCALLTYPE ClassName::AddRef(void) \
	{ \
		return ++m_cRef; \
	} \
	ULONG STDMETHODCALLTYPE ClassName::Release(void) \
	{ \
		if (--m_cRef) \
			return m_cRef; \
		if (m_cRef==0 && m_bAutoDelete) \
			delete this; \
		return 0L; \
	}

#define COMSTD_QUERYINTERFACEIMPLEMENTATIONBEGIN(ClassName) \
	HRESULT STDMETHODCALLTYPE ClassName::QueryInterface(REFIID riid,void __RPC_FAR *__RPC_FAR *ppvObject) {
#define COMSTD_QUERYHANDLEINTERFACE(Interface) \
	if (IsEqualIID(riid, Interface)) \
    { \
        *ppvObject=(void*)this; \
        AddRef(); \
        return NOERROR; \
    }
#define COMSTD_QUERYINTERFACEIMPLEMENTATIONEND \
	*ppvObject=NULL; \
	return E_NOINTERFACE; }


// Interfaces
INTERFACE (CCoUnknown, IUnknown)
{
	COMSTD_DECLARATION(CCoUnknown)
};

INTERFACE (CCoDropSource, IDropSource)
{
	COMSTD_DECLARATION(CCoDropSource)

	virtual HRESULT STDMETHODCALLTYPE QueryContinueDrag(
		BOOL fEscapePressed,DWORD grfKeyState) = 0;
	virtual HRESULT STDMETHODCALLTYPE GiveFeedback(DWORD dwEffect) = 0;
};

INTERFACE (CCoDropTarget, IDropTarget)
{
	COMSTD_DECLARATION(CCoDropTarget)

	virtual HRESULT STDMETHODCALLTYPE DragEnter(IDataObject __RPC_FAR *pDataObj,
		DWORD grfKeyState,POINTL pt,DWORD __RPC_FAR *pdwEffect) = 0;
	virtual HRESULT STDMETHODCALLTYPE DragOver(DWORD grfKeyState,
		POINTL pt,DWORD __RPC_FAR *pdwEffect) = 0;
	virtual HRESULT STDMETHODCALLTYPE DragLeave(void) = 0;
	virtual HRESULT STDMETHODCALLTYPE Drop(IDataObject __RPC_FAR *pDataObj,
		DWORD grfKeyState,POINTL pt,DWORD __RPC_FAR *pdwEffect) = 0;
};

INTERFACE (CCoDataObject, IDataObject)
{
	COMSTD_DECLARATION(CCoDataObject)

	virtual HRESULT STDMETHODCALLTYPE GetData(
		FORMATETC __RPC_FAR *pformatetcIn,STGMEDIUM __RPC_FAR *pmedium) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetDataHere(
		FORMATETC __RPC_FAR *pformatetc,STGMEDIUM __RPC_FAR *pmedium) = 0;
	virtual HRESULT STDMETHODCALLTYPE QueryGetData(FORMATETC __RPC_FAR *pformatetc) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetCanonicalFormatEtc(
		FORMATETC __RPC_FAR *pformatectIn,FORMATETC __RPC_FAR *pformatetcOut) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetData(FORMATETC __RPC_FAR *pformatetc,
		STGMEDIUM __RPC_FAR *pmedium,BOOL fRelease) = 0;
	virtual HRESULT STDMETHODCALLTYPE EnumFormatEtc(DWORD dwDirection,
		IEnumFORMATETC __RPC_FAR *__RPC_FAR *ppenumFormatEtc) = 0;
	virtual HRESULT STDMETHODCALLTYPE DAdvise(FORMATETC __RPC_FAR *pformatetc,
		DWORD advf,IAdviseSink __RPC_FAR *pAdvSink,DWORD __RPC_FAR *pdwConnection) = 0;
	virtual HRESULT STDMETHODCALLTYPE DUnadvise(DWORD dwConnection) = 0;
	virtual HRESULT STDMETHODCALLTYPE EnumDAdvise(
		IEnumSTATDATA __RPC_FAR *__RPC_FAR *ppenumAdvise) = 0;
};

INTERFACE (CCoEnumFORMATETC, IEnumFORMATETC)
{
	COMSTD_DECLARATION(CCoEnumFORMATETC)

	virtual HRESULT STDMETHODCALLTYPE Next(ULONG celt,FORMATETC __RPC_FAR *rgelt,ULONG __RPC_FAR *pceltFetched)=0;
	virtual HRESULT STDMETHODCALLTYPE Skip(ULONG celt)=0;
	virtual HRESULT STDMETHODCALLTYPE Reset(void)=0;
	virtual HRESULT STDMETHODCALLTYPE Clone(IEnumFORMATETC __RPC_FAR *__RPC_FAR *ppenum)=0;
};


// Smart pointer for COM interfaces

template<class INTERFACE> 
class CComPtr
{
public:
	CComPtr(INTERFACE* pInterface=NULL) { m_pInterface=pInterface;  }
	~CComPtr();

	
	operator INTERFACE*() const { return m_pInterface; }
	operator INTERFACE*&() { return m_pInterface; }
	
	INTERFACE* operator ->() { return m_pInterface; }

	void Attach(INTERFACE* pInterface);
	INTERFACE* UnAttach();
	
	CComPtr<INTERFACE>& operator=(INTERFACE* pInterface);


	HRESULT CoCreateInstance(LPCOLESTR szProgID,LPUNKNOWN pUnkOuter=NULL,DWORD dwClsContext=CLSCTX_ALL);
	HRESULT CoCreateInstance(REFCLSID rclsid,LPUNKNOWN pUnkOuter=NULL,DWORD dwClsContext=CLSCTX_ALL);

	
private:
	INTERFACE* m_pInterface;

};


template<class INTERFACE> 
inline CComPtr<INTERFACE>::~CComPtr()
{
	if (m_pInterface!=NULL)
		m_pInterface->Release();
}

template<class INTERFACE> 
inline INTERFACE* CComPtr<INTERFACE>::UnAttach()
{
	TYPE* tmp=m_pInterface;
	m_pInterface=NULL;
	return tmp;
}

template<class INTERFACE> 
inline void CComPtr<INTERFACE>::Attach(INTERFACE* pInterface)
{
	if (m_data!=NULL)
		delete pInterface;
	m_pInterface=pInterface;
}



template<class INTERFACE> 
inline CComPtr<INTERFACE>& CComPtr<INTERFACE>::operator=(INTERFACE* pInterface)
{
	Attach(pInterface);
	return *this;
}


template<class INTERFACE> 
HRESULT CComPtr<INTERFACE>::CoCreateInstance(LPCOLESTR szProgID,LPUNKNOWN pUnkOuter,DWORD dwClsContext)
{
	CLSID clsid;
	HRESULT hr=CLSIDFromProgID(szProgID,&clsid);
	if (!SUCCEEDED(hr))
		return hr;

	return ::CoCreateInstance(clsid,pUnkOuter,dwClsContext,__uuidof(INTERFACE), (void**)&m_pInterface);	
}

template<class INTERFACE> 
HRESULT CComPtr<INTERFACE>::CoCreateInstance(REFCLSID rclsid,LPUNKNOWN pUnkOuter,DWORD dwClsContext)
{
	if (m_pInterface!=NULL)
		m_pInterface->Release();

	return ::CoCreateInstance(rclsid,pUnkOuter,dwClsContext,__uuidof(INTERFACE), (void**)&m_pInterface);	
}


#endif

#endif
