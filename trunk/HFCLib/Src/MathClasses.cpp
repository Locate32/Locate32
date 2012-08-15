////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////

#include "HFCLib.h"


//////////////////////////
// Class CReal

/*void CReal::DoRational() //NOT IMPLEMENTED YET
{
	isRational=TRUE;
}*/

void CReal::Cancel()
{
	if (isRational)
	{
		unsigned int c=ccd(nRational.nNumerator,nRational.nDenominator);
		if (c>1)
		{
			nRational.nNumerator/=int(c);
			nRational.nDenominator/=c;
		}
		else if (nRational.nNumerator==0)
			nRational.nDenominator=1;
	}
}

//Opetators
const CReal& CReal::operator=(const CReal& rReal)
{
	isRational=rReal.IsRational();
	if (isRational)
	{
		nRational.nNumerator=rReal.nRational.nNumerator;
		nRational.nDenominator=rReal.nRational.nDenominator;
	}
	else
		nIrrational=rReal.nIrrational;
	return *this;
}

const CReal& CReal::operator=(LPCSTR szNum)
{
	if (FirstCharIndex(szNum,'.')!=-1)
	{
		nIrrational=atof(szNum);
		if (nIrrational==int(nIrrational))
		{
			isRational=TRUE;
			nRational.nNumerator=int(nIrrational);
			nRational.nDenominator=1;
		}
		else
			isRational=FALSE;
		return *this;
	}
	LONG_PTR nIndex=FirstCharIndex(szNum,'/');
	if (nIndex!=-1)
	{
		int nMultipler=(int)FirstCharIndex(szNum,' ');
		if (nMultipler>0 && nMultipler<nIndex)
		{
			char* sz=new char[nMultipler+1];
			fMemCopy(sz,szNum,nMultipler);
			sz[nMultipler]='\0';
			szNum+=nMultipler+1;
			nIndex-=nMultipler+1;
			nMultipler=atoi(sz);
			delete[] sz;
		}
		else
			nMultipler=0;
		char* sz=new char[nIndex+1];
		fMemCopy(sz,szNum,nIndex);
		sz[nIndex]='\0';
		isRational=TRUE;
		nRational.nNumerator=atoi(sz);
		nRational.nDenominator=atoi(szNum+nIndex+1);
		if (nRational.nDenominator==0)
			nRational.nDenominator=1;
		delete[] sz;
		if (nMultipler>=0)
			nRational.nNumerator+=nRational.nDenominator*nMultipler;
		else
		{
			nRational.nNumerator=-nRational.nNumerator;
			nRational.nNumerator-=nRational.nDenominator*nMultipler;
		}
		return *this;
	}
	isRational=TRUE;
	nRational.nNumerator=atoi(szNum);
	nRational.nDenominator=1;
	return *this;
}

CReal::operator CString() const
{
	CString str;
	if (isRational)
	{
		if (nRational.nDenominator==1)
			str.Format("%d",nRational.nNumerator);
		else
			str.Format("%d/%d",nRational.nNumerator,nRational.nDenominator);
	}
	else
		str.Format("%e",nIrrational);
	return str;
}

const CReal& CReal::operator+=(const CReal& rReal)
{
	if (rReal.IsRational())
	{
		if (isRational)
		{
			nRational.nNumerator=rReal.nRational.nDenominator*nRational.nNumerator+nRational.nDenominator*rReal.nRational.nNumerator;
			nRational.nDenominator=nRational.nDenominator*rReal.nRational.nDenominator;
			Cancel();
		}
		else
			nIrrational+=DDIV(rReal.nRational.nNumerator,rReal.nRational.nDenominator);
	}
	else
	{
		if (isRational)
			DoIrrational();
		nIrrational+=rReal.nIrrational;
	}
	return *this;
}


const CReal& CReal::operator+=(double nIrrational_)
{
	if (isRational)
		DoIrrational();
	nIrrational+=nIrrational_;
	return *this;
}

const CReal& CReal::operator+=(int nInteger)
{
	if (isRational)
		nRational.nNumerator+=nRational.nDenominator*nInteger;
	else
		nIrrational+=nInteger;
	return *this;
}

const CReal& CReal::operator+=(rational& nRational_)
{
	if (isRational)
	{
		nRational.nNumerator=nRational_.nDenominator*nRational.nNumerator+nRational.nDenominator*nRational_.nNumerator;
		nRational.nDenominator=nRational.nDenominator*nRational_.nDenominator;
		Cancel();
	}
	else
		nIrrational+=DDIV(nRational_.nNumerator,nRational_.nDenominator);
	return *this;
}

const CReal& CReal::operator-=(const CReal& rReal)
{
	if (rReal.IsRational())
	{
		if (isRational)
		{
			nRational.nNumerator=rReal.nRational.nDenominator*nRational.nNumerator-nRational.nDenominator*rReal.nRational.nNumerator;
			nRational.nDenominator=nRational.nDenominator*rReal.nRational.nDenominator;
			Cancel();
		}
		else
			nIrrational-=DDIV(rReal.nRational.nNumerator,rReal.nRational.nDenominator);
	}
	else
	{
		if (isRational)
			DoIrrational();
		nIrrational-=rReal.nIrrational;
	}
	return *this;
}

const CReal& CReal::operator-=(double nIrrational_)
{
	if (isRational)
		DoIrrational();
	nIrrational-=nIrrational_;
	return *this;
}

const CReal& CReal::operator-=(int nInteger)
{
	if (isRational)
		nRational.nNumerator-=nRational.nDenominator*nInteger;
	else
		nIrrational+=nInteger;
	return *this;
}

const CReal& CReal::operator-=(rational& nRational_)
{
	if (isRational)
	{
		nRational.nNumerator=nRational_.nDenominator*nRational.nNumerator-nRational.nDenominator*nRational_.nNumerator;
		nRational.nDenominator=nRational.nDenominator*nRational_.nDenominator;
		Cancel();
	}
	else
		nIrrational-=DDIV(nRational_.nNumerator,nRational_.nDenominator);
	return *this;
}

const CReal& CReal::operator*=(const CReal& rReal)
{
	if (rReal.IsRational())
	{
		if (isRational)
		{
			nRational.nNumerator*=rReal.nRational.nNumerator;
			nRational.nDenominator*=rReal.nRational.nDenominator;
			Cancel();
		}
		else
			nIrrational*=DDIV(rReal.nRational.nNumerator,rReal.nRational.nDenominator);
	}
	else
	{
		if (isRational)
			DoIrrational();
		nIrrational*=rReal.nIrrational;
	}
	return *this;
}

const CReal& CReal::operator*=(double nIrrational_)
{
	if (isRational)
		DoIrrational();
	nIrrational*=nIrrational_;
	return *this;
}

const CReal& CReal::operator*=(int nInteger)
{
	if (isRational)
	{
		nRational.nNumerator*=nInteger;
		Cancel();
	}
	else
		nIrrational*=nInteger;
	return *this;
}

const CReal& CReal::operator*=(rational& nRational_)
{
	if (isRational)
	{
		nRational.nNumerator*=nRational_.nNumerator;
		nRational.nDenominator*=nRational_.nDenominator;
		Cancel();
	}
	else
		nIrrational*=DDIV(nRational_.nNumerator,nRational_.nDenominator);
	return *this;
}

const CReal& CReal::operator/=(const CReal& rReal)
{
	if (rReal.IsRational())
	{
		if (isRational)
		{
			if (rReal.nRational.nNumerator>=0)
			{
				nRational.nNumerator*=rReal.nRational.nDenominator;
				nRational.nDenominator*=rReal.nRational.nNumerator;
			}
			else
			{
				nRational.nNumerator*=-(int)(rReal.nRational.nDenominator);
				nRational.nDenominator*=(unsigned int)(-rReal.nRational.nNumerator);
			}
			Cancel();
		}
		else
			nIrrational*=DDIV(rReal.nRational.nDenominator,rReal.nRational.nNumerator);
	}
	else
	{
		if (isRational)
			DoIrrational();
		nIrrational/=rReal.nIrrational;
	}
	return *this;
}

const CReal& CReal::operator/=(double nIrrational_)
{
	if (isRational)
		DoIrrational();
	nIrrational/=nIrrational_;
	return *this;
}

const CReal& CReal::operator/=(int nInteger)
{
	if (isRational)
	{
		if (nInteger>=0)
			nRational.nDenominator*=nInteger;
		else
		{
			nRational.nNumerator=-nRational.nNumerator;
			nRational.nDenominator*=(unsigned int)(-nInteger);
		}			
		Cancel();
	}
	else
		nIrrational/=nInteger;
	return *this;
}

const CReal& CReal::operator/=(rational& nRational_)
{
	if (isRational)
	{
		if (nRational_.nNumerator>=0)
		{
			nRational.nNumerator*=nRational_.nDenominator;
			nRational.nDenominator*=nRational_.nNumerator;
		}
		else
		{
			nRational.nNumerator*=-(int)(nRational_.nDenominator);
			nRational.nDenominator*=(unsigned int)(-nRational_.nNumerator);
		}
		Cancel();
	}
	else
		nIrrational*=DDIV(nRational_.nDenominator,nRational_.nNumerator);
	return *this;
}

BOOL CReal::operator==(const CReal& rReal) const
{
	if (rReal.isRational)
	{
		if (isRational)
			return (DDIV(nRational.nNumerator,nRational.nDenominator)==DDIV(rReal.nRational.nNumerator,rReal.nRational.nDenominator));
		else
			return (DDIV(rReal.nRational.nNumerator,rReal.nRational.nDenominator)==nIrrational);
	}
	else
	{
		if (isRational)
			return (DDIV(nRational.nNumerator,nRational.nDenominator)==rReal.nIrrational);
		else
			return (nIrrational==rReal.nIrrational);
	}
}


BOOL CReal::operator!=(const CReal& rReal) const
{
	if (rReal.isRational)
	{
		if (isRational)
			return (DDIV(nRational.nNumerator,nRational.nDenominator)!=DDIV(rReal.nRational.nNumerator,rReal.nRational.nDenominator));
		else
			return (DDIV(rReal.nRational.nNumerator,rReal.nRational.nDenominator)!=nIrrational);
	}
	else
	{
		if (isRational)
			return (DDIV(nRational.nNumerator,nRational.nDenominator)!=rReal.nIrrational);
		else
			return (nIrrational!=rReal.nIrrational);
	}
}

BOOL CReal::operator!=(double nIrrational_) const
{
	if (isRational)
		return (DDIV(nRational.nNumerator,nRational.nDenominator)!=nIrrational_);
	else
		return (nIrrational!=nIrrational_);
}

BOOL CReal::operator<=(const CReal& rReal) const
{
	if (rReal.isRational)
	{
		if (isRational)
			return (DDIV(nRational.nNumerator,nRational.nDenominator)<=DDIV(rReal.nRational.nNumerator,rReal.nRational.nDenominator));
		else
			return (DDIV(rReal.nRational.nNumerator,rReal.nRational.nDenominator)<=nIrrational);
	}
	else
	{
		if (isRational)
			return (DDIV(nRational.nNumerator,nRational.nDenominator)<=rReal.nIrrational);
		else
			return (nIrrational<=rReal.nIrrational);
	}
}

BOOL CReal::operator>=(const CReal& rReal) const
{
	if (rReal.isRational)
	{
		if (isRational)
			return (DDIV(nRational.nNumerator,nRational.nDenominator)>=DDIV(rReal.nRational.nNumerator,rReal.nRational.nDenominator));
		else
			return (DDIV(rReal.nRational.nNumerator,rReal.nRational.nDenominator)>=nIrrational);
	}
	else
	{
		if (isRational)
			return (DDIV(nRational.nNumerator,nRational.nDenominator)>=rReal.nIrrational);
		else
			return (nIrrational>=rReal.nIrrational);
	}
}

BOOL CReal::operator<(const CReal& rReal) const
{
	if (rReal.isRational)
	{
		if (isRational)
			return (DDIV(nRational.nNumerator,nRational.nDenominator)<DDIV(rReal.nRational.nNumerator,rReal.nRational.nDenominator));
		else
			return (DDIV(rReal.nRational.nNumerator,rReal.nRational.nDenominator)<nIrrational);
	}
	else
	{
		if (isRational)
			return (DDIV(nRational.nNumerator,nRational.nDenominator)<rReal.nIrrational);
		else
			return (nIrrational<rReal.nIrrational);
	}
}

BOOL CReal::operator>(const CReal& rReal) const
{
	if (rReal.isRational)
	{
		if (isRational)
			return (DDIV(nRational.nNumerator,nRational.nDenominator)>DDIV(rReal.nRational.nNumerator,rReal.nRational.nDenominator));
		else
			return (DDIV(rReal.nRational.nNumerator,rReal.nRational.nDenominator)>nIrrational);
	}
	else
	{
		if (isRational)
			return (DDIV(nRational.nNumerator,nRational.nDenominator)>rReal.nIrrational);
		else
			return (nIrrational>rReal.nIrrational);
	}
}


const CReal CReal::Inv() const
{
	if (isRational)
	{
		if (nRational.nNumerator>=0)
			return CReal(nRational.nDenominator,nRational.nNumerator);
		else
			return CReal(-int(nRational.nDenominator),-nRational.nNumerator);
	}
	else
		return CReal(1/nIrrational);
}

//////////////////////////
// Class CComplex


CComplex::operator CString() const
{
	if (Im==0)
		return Re;
	CString str;
	if (Re!=0)
	{
		str << (CString)Re;
		if (Im<0)
		{
			str << "-";
			if (Im!=-1)
				str << (CString)Im.Opp();
		}
		else
		{
			str << "+";
			if (Im!=1)
				str << (CString)Im;
		}
		str << 'i';
		return str;
	}
	if (Im==-1)
		str << '-';
	else if (Im!=1)
		str << (CString)Im;
	str << 'i';
	return str;
}

const CComplex& CComplex::operator=(LPCSTR szNum)
{
	while (szNum[0]==' ') szNum++;
	int nIndex=FirstCharIndex(szNum+1,'+')+1;
	if (nIndex==0) nIndex=FirstCharIndex(szNum+1,'-')+1;
	if (nIndex==0)
	{
		nIndex=FirstCharIndex(szNum,'i');
		if (nIndex==-1) nIndex=FirstCharIndex(szNum,'j');
		if (nIndex==-1)
		{
			Re=szNum;
			Im=0;
		}
		else
		{
			Re=0;
			CString im(szNum,nIndex);
			if (im.IsEmpty())
				Im=1;
			else if (im.Compare("-")==0)
				Im=-1;
			else
				Im=szNum;
		}
	}
	else
	{
		CString re(szNum,nIndex);
		CString im;
		Re=re;
		while (szNum[nIndex]==' ') nIndex++;
		if (szNum[nIndex]=='+')
			nIndex++;
		else if (szNum[nIndex]=='-')
		{
			nIndex++;
			im='-';
		}
		while (szNum[nIndex]>='0' && szNum[nIndex]<='9')
		{
			im << szNum[nIndex];
			nIndex++;
		}
		if (szNum[nIndex]=='i' || szNum[nIndex]=='j')
		{
			if (im.IsEmpty())
				Im=1;
			else if (im.Compare("-")==0)
				Im=-1;
			else
				Im=im;
		}
		else
			Im=0;
	}
	return *this;
}

const CComplex& CComplex::operator*=(const CComplex& rComplex)
{
	CReal re,im;
	re=Re*rComplex.Re-Im*rComplex.Im;
	im=Re*rComplex.Im+Im*rComplex.Re;
	Re=re;Im=im;
	return *this;
}



//////////////////////////
// Class CMatrix<TYPE>

void CAppData::MatrixErrorProc(void* pMatrix,DWORD dwErrorCode,DWORD dwData)
{
	switch (dwErrorCode)
	{
	case MATERR_DIMENSION:
		DebugMessage("MatrixErrorProc(): matrix dimension error");
		break;
	case MATERR_NOINVERSE:
		DebugMessage("MatrixErrorProc(): no inverse matrix");
		break;
	case MATERR_INDEXOUT:
		DebugMessage("MatrixErrorProc(): index out of range");
		break;
	}
}

//////////////////////////
// Class CByteMatrix

CByteMatrix CByteMatrix::Inv() const
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
			if (GetModulo256Inv(pMatrix[0])==0)
			{
				pErrorProc((void*)this,MATERR_NOINVERSE,dwData);
				return *this;
			}
			return CByteMatrix(1,1,GetModulo256Inv(pMatrix[0]));
		case 2:
			{
				BYTE nInvDet=GetModulo256Inv(det2(*this));
				if (nInvDet==0)
				{
					pErrorProc((void*)this,MATERR_NOINVERSE,dwData);
					return *this;
				}
				return CByteMatrix(2,2,nInvDet*e(2,2),-nInvDet*e(1,2),-nInvDet*e(2,1),nInvDet*e(1,1));
			}
		case 3:
			{
				BYTE nInvDet=GetModulo256Inv(det3(*this));
				if (nInvDet==0)
				{
					pErrorProc((void*)this,MATERR_NOINVERSE,dwData);
					return *this;
				}
				CByteMatrix inv;
				inv.SetErrorProc(pErrorProc,dwData);
				inv.m=inv.n=3;
				inv.pMatrix=new BYTE[9];
				for (UINT i=1;i<=3;i++)
				{
					for (UINT j=1;j<=3;j++)
					{
						me(inv,j,i)=nInvDet*Cof(i,j);
					}
				}
				return inv;
			}
		default:
			{
				BYTE nInvDet=GetModulo256Inv(det(*this));
				if (nInvDet==0)
				{
					pErrorProc((void*)this,MATERR_NOINVERSE,dwData);
					return *this;
				}
				CByteMatrix inv;
				inv.SetErrorProc(pErrorProc,dwData);
				inv.m=inv.n=m;
				inv.pMatrix=new BYTE[inv.m*inv.m];
				for (UINT i=1;i<=inv.m;i++)
				{
					for (UINT j=1;j<=inv.n;j++)
					{
						me(inv,j,i)=nInvDet*Cof(i,j);
					}
				}
				return inv;
			}
		}
	}
	pErrorProc((void*)this,MATERR_NOINVERSE,dwData);
	return *this;
}

CByteMatrix::CByteMatrix(UINT m_,UINT n_,BYTE firstcell,...)
{
	va_list argList;
	va_start(argList,n_);
	pErrorProc=CAppData::MatrixErrorProc;
	dwData=NULL;
	m=m_;
	n=n_;
	pMatrix=new BYTE[m*n];
	for (DWORD i=0;i<m*n;i++)
	{
		pMatrix[i]=va_arg(argList,BYTE);
	}
	va_end(argList);
}

CByteMatrix::CByteMatrix(const CByteMatrix& rMatrix)
{
	pErrorProc=rMatrix.pErrorProc;
	dwData=rMatrix.dwData;
	m=rMatrix.m;
	n=rMatrix.n;
	pMatrix=new BYTE[m*n];
	fMemCopy(pMatrix,rMatrix.pMatrix,sizeof(BYTE)*m*n);
}


CComplexMatrix::CComplexMatrix(UINT m_,UINT n_,CComplex firstcell,...)
{
	va_list argList;
	va_start(argList,n_);
	pErrorProc=CAppData::MatrixErrorProc;
	dwData=NULL;
	m=m_;
	n=n_;
	pMatrix=new CComplex[m*n];
	for (DWORD i=0;i<m*n;i++)
	{
		pMatrix[i]=va_arg(argList,CComplex);
	}
	va_end(argList);
}

CComplexMatrix::CComplexMatrix(const CComplexMatrix& rMatrix)
{
	pErrorProc=rMatrix.pErrorProc;
	dwData=rMatrix.dwData;
	m=rMatrix.m;
	n=rMatrix.n;
	pMatrix=new CComplex[m*n];
	for (UINT i=0;i<m*n;i++)
		pMatrix[i]=rMatrix.pMatrix[i];
}
