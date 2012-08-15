////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////

#ifndef HFCMATH_INL
#define HFCMATH_INL

////////////////////////////////////////////////////////////////////
// Inline functions

// CReal
inline CReal::CReal(double nIrrational)
:	isRational(FALSE)
{
	this->nIrrational=nIrrational;
}

inline CReal::CReal(int nNumerator,unsigned int nDenominator)
:	isRational(TRUE)
{
	nRational.nNumerator=nNumerator;
	nRational.nDenominator=nDenominator;
}

inline CReal::CReal(LPCSTR stNum)
{
	*this=stNum;
}

inline void CReal::DoIrrational()
{
	isRational=FALSE;
	nIrrational=DDIV(nRational.nNumerator,nRational.nDenominator);
}

inline CReal::operator int() const 
{
	if (isRational)
		return int(nRational.nNumerator/nRational.nDenominator); 
	else
		return int(nIrrational);
}

inline CReal::operator double() const 
{
	if (isRational) 
		return nRational.nNumerator/nRational.nDenominator; 
	else 
		return nIrrational; 
}

inline CReal CReal::operator+(const CReal& rReal) const
{
	CReal real(*this);
	real+=rReal;
	return real;
}

inline CReal CReal::operator+(double nIrrational) const
{
	CReal real(*this);
	real+=nIrrational;
	return real;
}

inline CReal CReal::operator+(int nInteger) const
{
	CReal real(*this);
	real+=nInteger;
	return real;
}

inline CReal CReal::operator+(rational& nRational_) const
{
	CReal real(*this);
	real+=nRational_;
	return real;
}

inline CReal CReal::operator-(const CReal& rReal) const
{
	CReal real(*this);
	real-=rReal;
	return real;
}

inline CReal CReal::operator-(double nIrrational) const
{
	CReal real(*this);
	real-=nIrrational;
	return real;
}

inline CReal CReal::operator-(int nInteger) const
{
	CReal real(*this);
	real-=nInteger;
	return real;
}

inline CReal CReal::operator-(rational& nRational_) const
{
	CReal real(*this);
	real-=nRational_;
	return real;
}

inline CReal CReal::operator*(const CReal& rReal) const
{
	CReal real(*this);
	real*=rReal;
	return real;
}

inline CReal CReal::operator*(double nIrrational_) const
{
	CReal real(*this);
	real*=nIrrational_;
	return real;
}

inline CReal CReal::operator*(int nInteger) const
{
	CReal real(*this);
	real*=nInteger;
	return real;
}

inline CReal CReal::operator*(rational& nRational_) const
{
	CReal real(*this);
	real/=nRational_;
	return real;
}

inline CReal CReal::operator/(const CReal& rReal) const
{
	CReal real(*this);
	real/=rReal;
	return real;
}

inline CReal CReal::operator/(double nIrrational_) const
{
	CReal real(*this);
	real/=nIrrational_;
	return real;
}

inline CReal CReal::operator/(int nInteger) const
{
	CReal real(*this);
	real/=nInteger;
	return real;
}

inline CReal CReal::operator/(rational& nRational_) const
{
	CReal real(*this);
	real/=nRational_;
	return real;
}

inline BOOL CReal::operator==(double nIrrational_) const
{
	if (isRational)
		return (DDIV(nRational.nNumerator,nRational.nDenominator)==nIrrational_);
	else
		return (nIrrational==nIrrational_);
}

inline BOOL CReal::operator==(int nInteger) const
{
	if (isRational)
		return (DDIV(nRational.nNumerator,nRational.nDenominator)==DOUBLE(nInteger));
	else
		return (nIrrational==DOUBLE(nInteger));
}

inline BOOL CReal::operator==(rational& rRational) const
{
	if (isRational)
		return (DDIV(nRational.nNumerator,nRational.nDenominator)==DDIV(rRational.nNumerator,rRational.nDenominator));
	else
		return (nIrrational==DDIV(rRational.nNumerator,rRational.nDenominator));
}

inline BOOL CReal::operator!=(int nInteger) const
{
	if (isRational)
		return (DDIV(nRational.nNumerator,nRational.nDenominator)!=DOUBLE(nInteger));
	else
		return (nIrrational!=DOUBLE(nInteger));
}

inline BOOL CReal::operator!=(rational& rRational) const
{
	if (isRational)
		return (DDIV(nRational.nNumerator,nRational.nDenominator)!=DDIV(rRational.nNumerator,rRational.nDenominator));
	else
		return (nIrrational!=DDIV(rRational.nNumerator,rRational.nDenominator));
}

inline BOOL CReal::operator<=(double nIrrational_) const
{
	if (isRational)
		return (DDIV(nRational.nNumerator,nRational.nDenominator)<=nIrrational_);
	else
		return (nIrrational<=nIrrational_);
}

inline BOOL CReal::operator<=(int nInteger) const
{
	if (isRational)
		return (DDIV(nRational.nNumerator,nRational.nDenominator)<=DOUBLE(nInteger));
	else
		return (nIrrational<=DOUBLE(nInteger));
}

inline BOOL CReal::operator<=(rational& rRational) const
{
	if (isRational)
		return (DDIV(nRational.nNumerator,nRational.nDenominator)<=DDIV(rRational.nNumerator,rRational.nDenominator));
	else
		return (nIrrational<=DDIV(rRational.nNumerator,rRational.nDenominator));
}

inline BOOL CReal::operator>=(double nIrrational_) const
{
	if (isRational)
		return (DDIV(nRational.nNumerator,nRational.nDenominator)>=nIrrational_);
	else
		return (nIrrational>=nIrrational_);
}

inline BOOL CReal::operator>=(int nInteger) const
{
	if (isRational)
		return (DDIV(nRational.nNumerator,nRational.nDenominator)>=DOUBLE(nInteger));
	else
		return (nIrrational>=DOUBLE(nInteger));
}

inline BOOL CReal::operator>=(rational& rRational) const
{
	if (isRational)
		return (DDIV(nRational.nNumerator,nRational.nDenominator)>=DDIV(rRational.nNumerator,rRational.nDenominator));
	else
		return (nIrrational>=DDIV(rRational.nNumerator,rRational.nDenominator));
}

inline BOOL CReal::operator<(double nIrrational_) const
{
	if (isRational)
		return (DDIV(nRational.nNumerator,nRational.nDenominator)<nIrrational_);
	else
		return (nIrrational<nIrrational_);
}

inline BOOL CReal::operator<(int nInteger) const
{
	if (isRational)
		return (DDIV(nRational.nNumerator,nRational.nDenominator)<DOUBLE(nInteger));
	else
		return (nIrrational<DOUBLE(nInteger));
}

inline BOOL CReal::operator<(rational& rRational) const
{
	if (isRational)
		return (DDIV(nRational.nNumerator,nRational.nDenominator)<DDIV(rRational.nNumerator,rRational.nDenominator));
	else
		return (nIrrational<DDIV(rRational.nNumerator,rRational.nDenominator));
}

inline BOOL CReal::operator>(double nIrrational_) const
{
	if (isRational)
		return (DDIV(nRational.nNumerator,nRational.nDenominator)>nIrrational_);
	else
		return (nIrrational>nIrrational_);
}

inline BOOL CReal::operator>(int nInteger) const
{
	if (isRational)
		return (DDIV(nRational.nNumerator,nRational.nDenominator)>DOUBLE(nInteger));
	else
		return (nIrrational>DOUBLE(nInteger));
}

inline BOOL CReal::operator>(rational& rRational) const
{
	if (isRational)
		return (DDIV(nRational.nNumerator,nRational.nDenominator)>DDIV(rRational.nNumerator,rRational.nDenominator));
	else
		return (nIrrational>DDIV(rRational.nNumerator,rRational.nDenominator));
}

inline const CReal CReal::Opp() const
{
	if (isRational)
		return CReal(-nRational.nNumerator,nRational.nDenominator);
	else
		return CReal(-nIrrational);
}

inline CReal CReal::operator-() const
{ 
	return Opp(); 
}

// CComplex

inline CComplex::CComplex(const CReal& rReal)
:	Re(rReal)
{
}

inline CReal::operator CComplex() const
{
	return CComplex(*this);
}

inline CComplex::CComplex(const CReal& rReal,const CReal& rImaginary)
:	Re(rReal),Im(rImaginary)
{
}

inline CComplex::CComplex(LPCSTR stNum)
{
	*this=stNum;
}


inline CComplex::CComplex(double dReal,double dImaginary)
:	Re(dReal),Im(dImaginary)
{
}

inline CComplex::CComplex(int nReal,int nImaginary)
:	Re(nReal),Im(nImaginary)
{
}

inline const CComplex& CComplex::operator=(const CComplex& rComplex)
{
	Re=rComplex.Re;
	Im=rComplex.Im;
	return *this; 
}

inline const CComplex& CComplex::operator=(const CReal& rReal) 
{ 
	Re=rReal; 
	Im=0; 
	return *this; 
}

inline const CComplex& CComplex::operator=(double nIrrational) 
{ 
	Re=nIrrational; 
	Im=0; 
	return *this; 
}

inline const CComplex& CComplex::operator=(int nInteger)
{ 
	Re=nInteger; 
	Im=0; 
	return *this; 
}

inline CComplex CComplex::operator*(const CComplex& rComplex) const
{
	return CComplex(Re*rComplex.Re-Im*rComplex.Im,Re*rComplex.Im+Im*rComplex.Re);
}

inline const CComplex& CComplex::operator+=(const CComplex& rComplex) 
{ 
	Re+=rComplex.Re; 
	Im+=rComplex.Im; 
	return *this; 
}

inline const CComplex& CComplex::operator+=(const CReal& rReal) 
{ 
	Re+=rReal; 
	return *this; 
}

inline const CComplex& CComplex::operator+=(double nIrrational) 
{ 
	Re+=nIrrational; 
	return *this; 
}

inline const CComplex CComplex::Inv() const
{
	CReal tmp=Re*Re+Im*Im;
	return CComplex(Re/tmp,Im.Opp()/tmp);
}

inline const CComplex& CComplex::operator+=(int nInteger) 
{ 
	Re+=nInteger; 
	return *this; 
}

inline const CComplex& CComplex::operator-=(const CComplex& rComplex) { Re-=rComplex.Re; Im-=rComplex.Im; return *this; }
inline const CComplex& CComplex::operator-=(const CReal& rReal) { Re-=rReal; return *this; }
inline const CComplex& CComplex::operator-=(double nIrrational) { Re-=nIrrational; return *this; }
inline const CComplex& CComplex::operator-=(int nInteger) { Re-=nInteger; return *this; }

inline const CComplex& CComplex::operator*=(const CReal& rReal) { Re*=rReal; Im*=rReal; return *this; }
inline const CComplex& CComplex::operator*=(double nIrrational) { Re*=nIrrational; Im*=nIrrational; return *this; }
inline const CComplex& CComplex::operator*=(int nInteger) { Re*=nInteger; Im*=nInteger; return *this; }

inline const CComplex& CComplex::operator/=(const CComplex& rComplex) { return (*this)*=rComplex.Inv(); }
inline const CComplex& CComplex::operator/=(const CReal& rReal) { Re/=rReal; Im/=rReal; return *this; }
inline const CComplex& CComplex::operator/=(double nIrrational) { Re/=nIrrational; Im/=nIrrational; return *this; }
inline const CComplex& CComplex::operator/=(int nInteger) { Re/=nInteger; Im/=nInteger; return *this; }

inline CComplex CComplex::operator+(const CComplex& rComplex) const { return CComplex(Re+rComplex.Re,Im+rComplex.Im); }
inline CComplex CComplex::operator+(const CReal& rReal) const { return CComplex(Re+rReal,Im); }
inline CComplex CComplex::operator+(double nIrrational) const { return CComplex(Re+nIrrational,Im); }
inline CComplex CComplex::operator+(int nInteger) const { return CComplex(Re+nInteger,Im); }

inline CComplex CComplex::operator-(const CComplex& rComplex) const { return CComplex(Re-rComplex.Re,Im-rComplex.Im); }
inline CComplex CComplex::operator-(const CReal& rReal) const { return CComplex(Re-rReal,Im); }
inline CComplex CComplex::operator-(double nIrrational) const { return CComplex(Re-nIrrational,Im); }
inline CComplex CComplex::operator-(int nInteger) const { return CComplex(Re-nInteger,Im); }

inline CComplex CComplex::operator*(const CReal& rReal) const { return CComplex(Re*rReal,Im*rReal); }
inline CComplex CComplex::operator*(double nIrrational) const { return CComplex(Re*nIrrational,Im*nIrrational); }
inline CComplex CComplex::operator*(int nInteger) const { return CComplex(Re*nInteger,Im*nInteger); }

inline CComplex CComplex::operator/(const CComplex& rComplex) const { return (*this)*rComplex.Inv(); }
inline CComplex CComplex::operator/(const CReal& rReal) const { return CComplex(Re/rReal,Im/rReal); }
inline CComplex CComplex::operator/(double nIrrational) const { return CComplex(Re/nIrrational,Im/nIrrational); }
inline CComplex CComplex::operator/(int nInteger) const { return CComplex(Re/nInteger,Im/nInteger); }

inline BOOL CComplex::operator==(const CComplex& rComplex) const { return (Re==rComplex.Re && Im==rComplex.Im); }
inline BOOL CComplex::operator==(const CReal& rReal) const { return (Re==rReal && Im==0); }
inline BOOL CComplex::operator==(double nIrrational) const { return (Re==nIrrational && Im==0); }
inline BOOL CComplex::operator==(int nInteger) const { return (Re==nInteger && Im==0); }

inline BOOL CComplex::operator!=(const CComplex& rComplex) const { return !(Re==rComplex.Re && Im==rComplex.Im); }
inline BOOL CComplex::operator!=(const CReal& rReal) const { return !(Re==rReal && Im==0); }
inline BOOL CComplex::operator!=(double nIrrational) const { return !(Re==nIrrational && Im==0); }
inline BOOL CComplex::operator!=(int nInteger) const { return !(Re==nInteger && Im==0); }

inline const CComplex CComplex::Conj() const
{
	return CComplex(Re,Im.Opp()); 
}

inline const CReal CComplex::Module() const
{
	double module=sqrt(Re*Re+Im*Im);
	if (module==int(module))
		return CReal(int(module));
	return CReal(module);
}


template<class TYPE>
CMatrix<TYPE>::CMatrix(UINT m_,UINT n_)
:	m(m_),n(n_)
{
	pErrorProc=CAppData::MatrixErrorProc;
	dwData=NULL;
	pMatrix=new TYPE[m*n];
	for (UINT i=0;i<m*n;i++)
		pMatrix[i]=0;
}

template<class TYPE>
CMatrix<TYPE>::CMatrix(UINT m_,UINT n_,const TYPE* pMatrix_)
{
	pErrorProc=CAppData::MatrixErrorProc;
	dwData=NULL;
	m=m_;
	n=n_;
	pMatrix=new TYPE[m*n];
	for (UINT i=0;i<m*n;i++)
		pMatrix[i]=pMatrix_[i];
}

template<class TYPE>
CMatrix<TYPE>::CMatrix(UINT m_,UINT n_,TYPE firstcell,...)
{
	va_list argList;
	va_start(argList,n_);
	pErrorProc=CAppData::MatrixErrorProc;
	dwData=NULL;
	m=m_;
	n=n_;
	pMatrix=new TYPE[m*n];
	for (DWORD i=0;i<m*n;i++)
	{
		pMatrix[i]=va_arg(argList,TYPE);
	}
	va_end(argList);
}
	
template<class TYPE>
CMatrix<TYPE>::CMatrix(const CMatrix& rMatrix)
{
	pErrorProc=rMatrix.pErrorProc;
	dwData=rMatrix.dwData;
	m=rMatrix.m;
	n=rMatrix.n;
	pMatrix=new TYPE[m*n];
	for (UINT i=0;i<m*n;i++)
		pMatrix[i]=rMatrix.pMatrix[i];
}

template<class TYPE>
CMatrix<TYPE>::~CMatrix()
{
	if (pMatrix!=NULL)
		delete[] pMatrix;
}

template<class TYPE>
void CMatrix<TYPE>::SetErrorProc(MATRIXERRORPROC pErrorProc_,DWORD dwData_)
{
	if (pErrorProc==NULL)
		pErrorProc=CAppData::MatrixErrorProc;
	else
		pErrorProc=pErrorProc_;
	dwData=dwData_;
}

template<class TYPE>
inline void CMatrix<TYPE>::SetAt(UINT m_,UINT n_,TYPE nValue)
{
	if (m_<=m && n_<=n)
		e(m_,n_)=nValue;
	else
		pErrorProc(this,MATERR_INDEXOUT,dwData);
}

template<class TYPE>
inline TYPE& CMatrix<TYPE>::GetAt(UINT m_,UINT n_)
{
	if (m_<=m && n_<=n)
		return e(m_,n_);
	pErrorProc(this,MATERR_INDEXOUT,dwData);
	return pMatrix[0];
}

template<class TYPE>
CMatrix<TYPE> CMatrix<TYPE>::Trans() const
{
	CMatrix<TYPE> mTranspose;
	mTranspose.m=n;
	mTranspose.n=m;
	TYPE* pData=new TYPE[m*n];
	for (UINT i=0;i<m;i++)
	{
		for (UINT j=0;j<n;j++)
			pData[i*m+j]=pMatrix[j*n+i];
	}
	mTranspose.SetErrorProc(pErrorProc,dwData);
	mTranspose.pMatrix=pData;
	return mTranspose;
}

template<class TYPE>
CMatrix<TYPE> CMatrix<TYPE>::Inv() const
{
	if (m!=n)
	{
		pErrorProc((void*)this,MATERR_DIMENSION,dwData);
		return *this;
	}
	else
	{
		switch (m)
		{
		case 1:
			if (pMatrix[0]==0)
			{
				pErrorProc((void*)this,MATERR_NOINVERSE,dwData);
				return *this;
			}
			return CMatrix<TYPE>(1,1,1/pMatrix[0]);
		case 2:
			{
				TYPE nInvDet(det2(*this));
				if (nInvDet==0)
				{
					pErrorProc((void*)this,MATERR_NOINVERSE,dwData);
					return *this;
				}
				nInvDet=1/nInvDet;
				return CMatrix<TYPE>(2,2,nInvDet*e(2,2),-nInvDet*e(1,2),-nInvDet*e(2,1),nInvDet*e(1,1));
			}
		default:
			{
				CMatrix<TYPE> inv;
				inv.SetErrorProc(pErrorProc,dwData);
				inv.m=inv.n=m;
				CMatrix<TYPE> tmp(*this);
				inv.pMatrix=new TYPE[m*m];
				for (DWORD i=0;i<m;i++)
				{
					for (DWORD j=0;j<m;j++)
					{
						if (i==j)
							inv.pMatrix[n*i+j]=1;
						else
							inv.pMatrix[n*i+j]=0;
					}
				}
				tmp.GaussJordan(&inv);
				if (tmp==CMatrix<TYPE>::Identical(m))
					return inv;
			}
		}
	}
	pErrorProc((void*)this,MATERR_NOINVERSE,dwData);
	return *this;
}

template<class TYPE>
inline void CMatrix<TYPE>::GetDimension(UINT& m_,UINT& n_) const
{
	m_=m;
	n_=n;
}

template<class TYPE>
void CMatrix<TYPE>::MultipleLine(UINT i,TYPE nCoefficient)
{
	if (i<=m)
	{
		i--;
		for (UINT k=0;k<n;k++)
			f(i,k)*=nCoefficient;
	}
	else
		pErrorProc(this,MATERR_INDEXOUT,dwData);
}

template<class TYPE>
void CMatrix<TYPE>::AddLineToLine(UINT i,UINT j,TYPE nCoefficient)
{
	if (i<=m && j<=m)
	{
		for (UINT k=1;k<=n;k++)
			e(j,k)+=nCoefficient*e(i,k);
	}
	else
		pErrorProc(this,MATERR_INDEXOUT,dwData);
}

template<class TYPE>
void CMatrix<TYPE>::SwapLines(UINT i,UINT j)
{
	if (i<=m && j<=m)
	{
		for (UINT k=1;k<=n;k++)
		{
			TYPE tmp=e(j,k);
			e(j,k)=e(i,k);
			e(i,k)=tmp;
		}
	}
	else
		pErrorProc(this,MATERR_INDEXOUT,dwData);
}

template<class TYPE>
void CMatrix<TYPE>::GaussJordan(CMatrix* pMatB,GJSTAGEPROC pStageProc,DWORD dwData)
{
	if (pMatB!=NULL)
	{
		if (m!=pMatB->m)
		{
			if (pStageProc!=NULL)
				pStageProc(this,pMatB,0,0,0,NULL,dwData);
			return;
		}
	}

	UINT i,j,a;
	for (i=1,j=1;i<=m && j<=n;i++,j++)
	{
		if (e(i,j)==0)
		{
			for (a=i+1;a<=m;a++)
			{
				if (a==i)
					continue;
				if (e(a,j)!=0)
				{
					P(i,a);
					if (pMatB!=NULL)
						pMatB->P(i,a);
					if (pStageProc!=NULL)
					{
						if (!pStageProc(this,pMatB,3,i,a,NULL,dwData))
							return;
					}
					break;
				}
			}
			if (e(i,j)==0)
			{
				i--;
				continue;
			}
		}
		if (e(i,j)!=1)
		{
			TYPE nCoefficient=1;
			nCoefficient/=e(i,j);
			M(i,nCoefficient);
			if (pMatB!=NULL)
				pMatB->M(i,nCoefficient);
			if (pStageProc!=NULL)
			{
				if (!pStageProc(this,pMatB,1,i,0,&nCoefficient,dwData))
					return;
			}
		}
		for (a=1;a<=m;a++)
		{
			if (a==i)
				continue;
			if (e(a,j)!=0)
			{
				TYPE nCoefficient=-1;
				nCoefficient*=e(a,j);
				A(i,a,nCoefficient);
				if (pMatB!=NULL)
					pMatB->A(i,a,nCoefficient);
				if (pStageProc!=NULL)
				{
					if (!pStageProc(this,pMatB,2,i,a,&nCoefficient,dwData))
						return;
				}
			}
		}
	}
}

template<class TYPE>
BOOL CMatrix<TYPE>::operator==(CMatrix<TYPE>& rMatrix) const
{
	if (m!=rMatrix.m || n!=rMatrix.n)
	{
		pErrorProc((void*)this,MATERR_DIMENSION,dwData);
		return FALSE;
	}
	for (DWORD i=0;i<m*n;i++)
	{
		if (pMatrix[i]!=rMatrix.pMatrix[i])
			return FALSE;
	}
	return TRUE;
}

template<class TYPE>
BOOL CMatrix<TYPE>::operator==(TYPE nValue) const
{
	if (m!=n)
	{
		pErrorProc((void*)this,MATERR_DIMENSION,dwData);
		return FALSE;
	}
	for (DWORD i=0;i<m;i++)
	{
		for (DOWRD j=0;j<n;j++)
		{
			if (i==j)
			{
				if (e(i,i)!=nValue)
					return FALSE;
			}
			else
			{
				if (e(i,j)!=0)
					return FALSE;
			}
		}
	}
	return TRUE;
}

template<class TYPE>
CMatrix<TYPE>& CMatrix<TYPE>::operator+=(CMatrix<TYPE>& rMatrix)
{
	if (m!=rMatrix.m || n!=rMatrix.n)
	{
		pErrorProc((void*)this,MATERR_DIMENSION,dwData);
		return *this;
	}
	for (DWORD i=0;i<m*n;i++)
		pMatrix[i]+=rMatrix.pMatrix[i];
	return *this;
}

template<class TYPE>
CMatrix<TYPE>& CMatrix<TYPE>::operator-=(CMatrix<TYPE>& rMatrix)
{
	if (m!=rMatrix.m || n!=rMatrix.n)
	{
		pErrorProc((void*)this,MATERR_DIMENSION,dwData);
		return *this;
	}
	for (DWORD i=0;i<m*n;i++)
		pMatrix[i]-=rMatrix.pMatrix[i];
	return *this;
}

template<class TYPE>
inline CMatrix<TYPE>& CMatrix<TYPE>::operator*=(CMatrix<TYPE>& rMatrix)
{
	*this=(*this)*rMatrix;
	return *this;
}

template<class TYPE>
CMatrix<TYPE>& CMatrix<TYPE>::operator*=(TYPE nNum)
{
	for (DWORD i=0;i<m*n;i++)
		pMatrix[i]*=nNum;
	return *this;
}
	
template<class TYPE>
CMatrix<TYPE> CMatrix<TYPE>::operator+(CMatrix<TYPE>& rMatrix) const
{
	if (m!=rMatrix.m || n!=rMatrix.n)
	{
		pErrorProc((void*)this,MATERR_DIMENSION,dwData);
		return *this;
	}
	CMatrix<TYPE> mat;
	mat.pErrorProc=pErrorProc;
	mat.dwData=dwData;
	mat.m=m;
	mat.n=n;
	mat.pMatrix=new TYPE[m*n];
	for (DWORD i=0;i<m*n;i++)
		mat.pMatrix[i]=pMatrix[i]+rMatrix.pMatrix[i];
	return mat;
}

template<class TYPE>
CMatrix<TYPE> CMatrix<TYPE>::operator-(CMatrix<TYPE>& rMatrix) const
{
	if (m!=rMatrix.m || n!=rMatrix.n)
	{
		pErrorProc((void*)this,MATERR_DIMENSION,dwData);
		return *this;
	}
	CMatrix<TYPE> mat;
	mat.pErrorProc=pErrorProc;
	mat.dwData=dwData;
	mat.m=m;
	mat.n=n;
	mat.pMatrix=new TYPE[m*n];
	for (DWORD i=0;i<m*n;i++)
		mat.pMatrix[i]=pMatrix[i]-rMatrix.pMatrix[i];
	return mat;
}

template<class TYPE>
CMatrix<TYPE> CMatrix<TYPE>::operator*(CMatrix<TYPE>& rMatrix) const
{
	if (n!=rMatrix.m)
	{
		pErrorProc((void*)this,MATERR_DIMENSION,dwData);
		return *this;
	}
	CMatrix<TYPE> mat;
	mat.pErrorProc=pErrorProc;
	mat.dwData=dwData;
	mat.m=m;
	mat.n=rMatrix.n;
	mat.pMatrix=new TYPE[mat.m*mat.n];
	for (DWORD i=0;i<mat.m;i++)
	{
		for (DWORD j=0;j<mat.n;j++)
		{
			mf(mat,i,j)=0;
			for (DWORD k=0;k<n;k++)
				mf(mat,i,j)+=f(i,k)*mf(rMatrix,k,j);
		}
	}
	return mat;
}

template<class TYPE>
CMatrix<TYPE> CMatrix<TYPE>::operator*(TYPE nNum)
{
	CMatrix<TYPE> mat;
	mat.pErrorProc=pErrorProc;
	mat.dwData=dwData;
	mat.m=m;
	mat.n=n;
	mat.pMatrix=new TYPE[m*n];
	for (DWORD i=0;i<m*n;i++)
		mat.pMatrix[i]=pMatrix[i]*nNum;
	return mat;
}

template<class TYPE>
CMatrix<TYPE>& CMatrix<TYPE>::MultipleCells(CMatrix<TYPE>& b)
{
	if (m!=b.m || n!=b.n)
	{
		pErrorProc((void*)this,MATERR_DIMENSION,dwData);
		return *this;
	}

	for (DWORD i=0;i<m*m;i++)
		pMatrix[i]*=bpMatrix[i];
	return *this;
}

template<class TYPE>
CMatrix<TYPE> CMatrix<TYPE>::MultipleCells(CMatrix<TYPE>& a,CMatrix<TYPE>& b)
{
	if (a.m!=b.m || a.n!=b.n)
	{
		pErrorProc((void*)this,MATERR_DIMENSION,dwData);
		return *this;
	}
	CMatrix<TYPE> mat;
	mat.pErrorProc=pErrorProc;
	mat.dwData=dwData;
	mat.m=m;
	mat.n=rMatrix.n;
	mat.pMatrix=new TYPE[mat.m*mat.n];
	for (DWORD i=0;i<a.m*a.n;i++)
		mat.pMatrix[i]=a.pMatrix[i]*b.pMatrix[i];
	return mat;
}

template<class TYPE>
CMatrix<TYPE>& CMatrix<TYPE>::DivideCells(CMatrix<TYPE>& b)
{
	if (m!=b.m || n!=b.n)
	{
		pErrorProc((void*)this,MATERR_DIMENSION,dwData);
		return *this;
	}

	for (DWORD i=0;i<m*m;i++)
		pMatrix[i]/=bpMatrix[i];
	return *this;
}

template<class TYPE>
CMatrix<TYPE> CMatrix<TYPE>::DivideCells(CMatrix<TYPE>& a,CMatrix<TYPE>& b)
{
	if (a.m!=b.m || a.n!=b.n)
	{
		pErrorProc((void*)this,MATERR_DIMENSION,dwData);
		return *this;
	}
	CMatrix<TYPE> mat;
	mat.pErrorProc=pErrorProc;
	mat.dwData=dwData;
	mat.m=m;
	mat.n=rMatrix.n;
	mat.pMatrix=new TYPE[mat.m*mat.n];
	for (DWORD i=0;i<a.m*a.n;i++)
		mat.pMatrix[i]=a.pMatrix[i]/b.pMatrix[i];
	return mat;
}


template<class TYPE>
TYPE CMatrix<TYPE>::DotProduct(CMatrix<TYPE>& rMatrix) const
{
	if (n!=1 && m!=1)
	{
		pErrorProc((void*)this,MATERR_DIMENSION,dwData);
		return 0;
	}
	TYPE nAns(0);
	for (LONG i=max(n,m)-1;i>0;i--)
		nAns+=pMatrix[i]*rMatrix.pMatrix[i];
	return nAns;
}

template<class TYPE>
CMatrix<TYPE> CMatrix<TYPE>::VectorProduct(CMatrix<TYPE>& b) const
{
	if ((m==1 && n==3) || (m==3 && n==1))
	{
		return CMatrix<TYPE> ans(m,n,
			pMatrix[2-1]*b.pMatrix[3-1]-pMatrix[3-1]*b.pMatrix[2-1],
			pMatrix[3-1]*b.pMatrix[1-1]-pMatrix[1-1]*b.pMatrix[3-1],
			pMatrix[1-1]*b.pMatrix[2-1]-pMatrix[2-1]*b.pMatrix[1-1]
			);
	}
	pErrorProc((void*)this,MATERR_DIMENSION,dwData);
	return Zero(m,n);
}
	
template<class TYPE>
CMatrix<TYPE> CMatrix<TYPE>::Identical(UINT m,TYPE nNum)
{
	CMatrix<TYPE> mat;
	mat.m=m_;
	mat.n=m_;
	mat.pErrorProc=CAppData::MatrixErrorProc;
	mat.dwData=NULL;
	mat.pMatrix=new TYPE[m_*m_];
	for (UINT i=0;i<m_;i++)
	{
		for (UINT j=0;j<m_;j++)
		{
			if (i==j)
				mat.pMatrix[i*m_+j]=nNum;
			else
				mat.pMatrix[i*m_+j]=0;
		}
	}
	return mat;
}

template<class TYPE>
CMatrix<TYPE> CMatrix<TYPE>::Diagonal(UINT m,...)
{
	CMatrix<TYPE> mat;
	va_list argLisT;
	va_start(argList,m);
	mat.m=m;
	mat.n=m;
	mat.pErrorProc=CAppData::MatrixErrorProc;
	mat.dwData=NULL;
	mat.pMatrix=new TYPE[m*m];
	for (UINT i=0;i<m;i++)
	{
		for (UINT j=0;j<m;j++)
		{
			if (i==j)
				mat.pMatrix[i*m+j]=va_arg(argList,TYPE); // m==n
			else
				mat.pMatrix[i*m+j]=0;
		}
	}
	va_end(argList);
	return mat;
}

template<class TYPE>
TYPE CMatrix<TYPE>::Det() const
{
	if (m!=n)
	{
		pErrorProc((void*)this,MATERR_DIMENSION,dwData);
		return TYPE(0);
	}
	switch (m)
	{
	case 1:
		return pMatrix[0];
	case 2:
		return det2(*this);
	case 3:
		return det3(*this);
	default:
		TYPE nDet;
		nDet=0;
		for (UINT k=1;k<=n;k++)
			nDet+=e(1,k)*Cof(1,k);
		return nDet;
	}
}	

template<class TYPE>
TYPE CMatrix<TYPE>::Det(UINT i,UINT j) const
{
	if (m!=n || m==1)
	{
		pErrorProc((void*)this,MATERR_DIMENSION,dwData);
		return 0;
	}
	if (m==2)
		return e(3-i,3-j);
	if (i>m || j>n)
	{
		pErrorProc((void*)this,MATERR_INDEXOUT,dwData);
		return pMatrix[0];
	}
	CMatrix<TYPE> mat;
	mat.m=m-1;
	mat.n=n-1;
	mat.pErrorProc=pErrorProc;
	mat.dwData=dwData;
	mat.pMatrix=new TYPE[mat.m*mat.n];
	for (UINT k=1;k<=mat.m;k++)
	{
		for (UINT l=1;l<=mat.n;l++)
		{
			me(mat,k,l)=e(k+(k>=i?1:0),l+(l>=j?1:0));
		}
	}
	return mat.Det();
}

inline BYTE GetModulo256Inv(BYTE nNum)
{
	if (nNum==0 || (nNum&1)==0)
		return 0;
	for (DWORD i=3;i<256;i+=2)
	{
		if (BYTE(i*nNum)==1)
			return (BYTE)i;
	}
	return 0;
}

inline BOOL CByteMatrix::IsSignular() const
{
	return GetModulo256Inv(Det())==0;
}

inline CByteMatrix::CByteMatrix(UINT m_,UINT n_)
:	CMatrix<BYTE>(m_,n_)
{
}

inline CByteMatrix::CByteMatrix(UINT m_,UINT n_,const BYTE* pMData)
:	CMatrix<BYTE>(m_,n_,pMData)
{
}

inline CComplexMatrix::CComplexMatrix(UINT m_,UINT n_)
:	CMatrix<CComplex>(m_,n_)
{
}

inline CComplexMatrix::CComplexMatrix(UINT m_,UINT n_,const CComplex* pMData)
:	CMatrix<CComplex>(m_,n_,pMData)
{
}


#endif

