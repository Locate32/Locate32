////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////


#ifndef HFCMATH_H
#define HFCMATH_H

#include <stdarg.h>

//Error codes
#define MATERR_NOERROR				0
#define MATERR_DIMENSION			1
#define MATERR_NOINVERSE			2
#define MATERR_INDEXOUT				3

typedef void (*MATRIXERRORPROC)(
	void* pMatrix, //Pointer to matrix
	DWORD dwErrorCode, // Error code
	DWORD dwData // User defined data
	);

/* nFunction is 
	0= Error: Invalid parameters or matrix dimensions is not valid, etc...
	1= M, Line a has multipled by value *(TYPE)(c)
	2= A, Line a has multipled by value *(TYPE)(c) and added to line b 
	3= P, Lines a and b have swapped
*/

typedef BOOL (*GJSTAGEPROC)(
    void* pMatA,void* pMatB, // Matrixes
	BYTE nFunction, // What has done
	UINT a,UINT b,void* c,
	DWORD dwData // User defined data
    );



typedef double (*PDIFF_FUNC)(
    double dParam
    );

typedef double (*PSUM_FUNC)(
    int dParam
    );

//Functions
UINT powi(USHORT x,BYTE y);
UINT powi10(BYTE y);
UINT fact(BYTE x);
double derivate(PDIFF_FUNC pFunc,double a,double dx);
double integral(PDIFF_FUNC pFunc,double a,double b,int n);
double sum(PSUM_FUNC pFunc,int i,int k);
unsigned int ccd(int a,int b);


#define STRP(str)			(LPCSTR)(CString)(str)
#define DDIV(a,b)			(DOUBLE(a)/(b))
#define e(i,j)				(pMatrix[((i)-1)*n+((j)-1)])
#define me(m,i,j)			((m).pMatrix[((i)-1)*(m).n+((j)-1)])
#define f(i,j)				(pMatrix[(i)*n+(j)])
#define mf(m,i,j)			((m).pMatrix[(i)*(m).n+(j)])
#define det(A)				(A).Det()
#define det2(A)				(me(A,1,1)*me(A,2,2)-me(A,1,2)*me(A,2,1))
#define det3(A)				(me(A,1,1)*(me(A,2,2)*me(A,3,3)-me(A,3,2)*me(A,2,3))\
						 +   me(A,1,2)*(me(A,2,3)*me(A,3,1)-me(A,2,1)*me(A,3,3))\
						 +   me(A,1,3)*(me(A,2,1)*me(A,3,2)-me(A,2,2)*me(A,3,1)))
#define inv(A)				(A).Inv()


class CReal;
class CComplex;

// Inline functions 

inline UINT powi2(BYTE y)
{
	return 1<<y;
}

inline UINT perm(BYTE n,BYTE k)
{
	if (n<k)
		return 0;
	return (fact(n)/fact(n-k));
}

inline UINT comb(BYTE n,BYTE k)
{
	if (n<k)
		return 0;
	return (fact(n)/(fact(k)*fact(n-k)));
}

inline double derivate(PDIFF_FUNC pFunc,double a,double dx)
{
	return (pFunc(a+dx)-pFunc(a-dx))/(2*dx);
}

//Classes

class CReal
{
public:
	CReal() : isRational(TRUE) {nRational.nNumerator=0;nRational.nDenominator=0;}
	CReal(double nIrrational);
	CReal(int nNumerator,unsigned int nDenominator=1);
	CReal(LPCSTR stNum); // For example 3, 2/2, 3.213

public:
	struct rational
	{
		int nNumerator;
		unsigned int nDenominator;
	};


public:
	BOOL IsRational() const { return isRational; }
	//void DoRational(); Not implemented yet, sorry
	void DoIrrational();
	void Cancel();

	//Opetators
	operator int() const;
	//operator rational&() { if (!isRational) DoRational(); return nRational; } NOT IMPLEMENTED YET AS DoRational()
	operator double() const;
	operator CString() const;
	operator CComplex() const;

	const CReal& operator=(const CReal& rReal);
	const CReal& operator=(double nIrrational_) { isRational=FALSE; nIrrational=nIrrational_; return *this; }
	const CReal& operator=(int nInteger) { isRational=TRUE; nRational.nNumerator=nInteger; nRational.nDenominator=1; return *this; }
	const CReal& operator=(rational& nRational_) { isRational=TRUE; nRational.nNumerator=nRational_.nNumerator; nRational.nDenominator=nRational_.nDenominator; return *this; }
	const CReal& operator=(LPCSTR szNum);
		
	const CReal& operator+=(const CReal& rReal);
	const CReal& operator+=(double nIrrational);
	const CReal& operator+=(int nInteger);
	const CReal& operator+=(rational& nRational);

	const CReal& operator-=(const CReal& rReal);
	const CReal& operator-=(double nIrrational);
	const CReal& operator-=(int nInteger);
	const CReal& operator-=(rational& nRational);

	const CReal& operator*=(const CReal& rReal);
	const CReal& operator*=(double nIrrational);
	const CReal& operator*=(int nInteger);
	const CReal& operator*=(rational& nRational);

	const CReal& operator/=(const CReal& rReal);
	const CReal& operator/=(double nIrrational);
	const CReal& operator/=(int nInteger);
	const CReal& operator/=(rational& nRational);
	
	CReal operator+(const CReal& rReal) const;
	CReal operator+(double nIrrational) const;
	CReal operator+(int nInteger) const;
	CReal operator+(rational& nRational) const;
	
	CReal operator-(const CReal& rReal) const;
	CReal operator-(double nIrrational) const;
	CReal operator-(int nInteger) const;
	CReal operator-(rational& nRational) const;
	CReal operator-() const;
	
	CReal operator*(const CReal& rReal) const;
	CReal operator*(double nIrrational) const;
	CReal operator*(int nInteger) const;
	CReal operator*(rational& nRational) const;

	CReal operator/(const CReal& rReal) const;
	CReal operator/(double nIrrational) const;
	CReal operator/(int nInteger) const;
	CReal operator/(rational& nRational) const;

	BOOL operator==(const CReal& rReal) const;
	BOOL operator==(double nIrrational) const;
	BOOL operator==(int nInteger) const;
	BOOL operator==(rational& rRational) const;
	
	BOOL operator!=(const CReal& rReal) const;
	BOOL operator!=(double nIrrational) const;
	BOOL operator!=(int nInteger) const ;
	BOOL operator!=(rational& rRational) const;

	BOOL operator<=(const CReal& rReal) const;
	BOOL operator<=(double nIrrational) const;
	BOOL operator<=(int nInteger) const;
	BOOL operator<=(rational& rRational) const;
	
	BOOL operator>=(const CReal& rReal) const;
	BOOL operator>=(double nIrrational) const;
	BOOL operator>=(int nInteger) const;
	BOOL operator>=(rational& rRational) const;
	
	BOOL operator<(const CReal& rReal) const;
	BOOL operator<(double nIrrational) const;
	BOOL operator<(int nInteger) const;
	BOOL operator<(rational& rRational) const;
	
	BOOL operator>(const CReal& rReal) const;
	BOOL operator>(double nIrrational) const;
	BOOL operator>(int nInteger) const;
	BOOL operator>(rational& rRational) const;
	
	const CReal Inv() const;
	const CReal Opp() const;

protected:
	BOOL isRational;
	union {
		double nIrrational;
		rational nRational;
	};
};

class CComplex
{
public:
	CComplex() {}
	CComplex(const CReal& rReal);
	CComplex(const CReal& rReal,const CReal& rImaginary);
	CComplex(double dReal,double dImaginary=0);
	CComplex(int nReal,int nImaginary=0);
	CComplex(LPCSTR stNum); // For example 3+2i, 2/2-3/7j, 3.213+1.12i

	//Opetators
	operator CString() const;

	const CComplex& operator=(const CComplex& rComplex);
	const CComplex& operator=(const CReal& rReal);
	const CComplex& operator=(double nIrrational);
	const CComplex& operator=(int nInteger);
	const CComplex& operator=(LPCSTR szNum);
	
	const CComplex& operator+=(const CComplex& rComplex);
	const CComplex& operator+=(const CReal& rReal);
	const CComplex& operator+=(double nIrrational);
	const CComplex& operator+=(int nInteger);
	
	const CComplex& operator-=(const CComplex& rComplex);
	const CComplex& operator-=(const CReal& rReal);
	const CComplex& operator-=(double nIrrational);
	const CComplex& operator-=(int nInteger);
	
	const CComplex& operator*=(const CComplex& rComplex);
	const CComplex& operator*=(const CReal& rReal);
	const CComplex& operator*=(double nIrrational);
	const CComplex& operator*=(int nInteger);
	
	const CComplex& operator/=(const CComplex& rComplex);
	const CComplex& operator/=(const CReal& rReal);
	const CComplex& operator/=(double nIrrational);
	const CComplex& operator/=(int nInteger);
	
	CComplex operator+(const CComplex& rComplex) const;
	CComplex operator+(const CReal& rReal) const;
	CComplex operator+(double nIrrational) const;
	CComplex operator+(int nInteger) const;
	
	CComplex operator-(const CComplex& rComplex) const;
	CComplex operator-(const CReal& rReal) const;
	CComplex operator-(double nIrrational) const;
	CComplex operator-(int nInteger) const;
	
	CComplex operator*(const CComplex& rComplex) const;
	CComplex operator*(const CReal& rReal) const;
	CComplex operator*(double nIrrational) const;
	CComplex operator*(int nInteger) const;
	
	CComplex operator/(const CComplex& rComplex) const;
	CComplex operator/(const CReal& rReal) const;
	CComplex operator/(double nIrrational) const;
	CComplex operator/(int nInteger) const;
	
	BOOL operator==(const CComplex& rComplex) const;
	BOOL operator==(const CReal& rReal) const;
	BOOL operator==(double nIrrational) const;
	BOOL operator==(int nInteger) const;
	
	BOOL operator!=(const CComplex& rComplex) const;
	BOOL operator!=(const CReal& rReal) const;
	BOOL operator!=(double nIrrational) const;
	BOOL operator!=(int nInteger) const;
	
	const CComplex Inv() const;
	const CComplex Conj() const;
	const CReal Module() const;

public:
	CReal Re;
	CReal Im;
};


template<class TYPE>
class CMatrix
{
public:
	CMatrix(UINT m,UINT n);
	CMatrix(UINT m,UINT n,const TYPE* pMData);
	CMatrix(UINT m,UINT n,TYPE firstcell,...);
	CMatrix(const CMatrix& rMatrix);

	~CMatrix();

protected:
	CMatrix() {}

public:
	void SetErrorProc(MATRIXERRORPROC pErrorProc,DWORD dwData);

	operator TYPE*() const { return pMatrix; }
	
	void SetAt(UINT m,UINT n,TYPE nValue);
	TYPE& GetAt(UINT m,UINT n);

	CMatrix<TYPE> Trans() const;
	CMatrix<TYPE> Inv() const;
	BOOL IsSignular() const { return Det()==0; }

	TYPE Det() const;
	TYPE Det(UINT i,UINT j) const; //Alitedeterminantti
	TYPE Cof(UINT i,UINT j) const { return ((i+j)%2==0?1:-1)*Det(i,j); }

	void GetDimension(UINT& m,UINT& n) const;
	UINT GetDimensionM() const { return m; }
	UINT GetDimensionN() const { return n; }
	
	void MultipleLine(UINT i,TYPE nCoefficient);
	void AddLineToLine(UINT i,UINT j,TYPE nCoefficient);
	void SwapLines(UINT i,UINT j);
	
	static CMatrix Identical(UINT m,TYPE nNum);
	static CMatrix Unity(UINT m) { return Identical(m,TYPE(1)); }
	static CMatrix Eye(UINT m) { return Unity(m); }
	static CMatrix Diagonal(UINT m,...);
	static CMatrix Zero(UINT m,UINT n) { return CMatrix<TYPE>(m,n); }
	
	CMatrix<TYPE>& operator+=(CMatrix<TYPE>& rMatrix);
	CMatrix<TYPE>& operator-=(CMatrix<TYPE>& rMatrix);
	CMatrix<TYPE>& operator*=(CMatrix<TYPE>& rMatrix);
	CMatrix<TYPE>& operator/=(CMatrix<TYPE>& rMatrix) { return ((*this)*=rMatrix.Inv()); }
	CMatrix<TYPE>& operator*=(TYPE nNum);
	CMatrix<TYPE>& operator/=(TYPE nNum) { return ((*this)*=(1/nNum)); }
	
	CMatrix<TYPE> operator+(CMatrix<TYPE>& rMatrix) const;
	CMatrix<TYPE> operator-(CMatrix<TYPE>& rMatrix) const;
	CMatrix<TYPE> operator*(CMatrix<TYPE>& rMatrix) const;
	CMatrix<TYPE> operator/(CMatrix<TYPE>& rMatrix) const { return ((*this)*rMatrix.Inv()); }
	CMatrix<TYPE> operator*(TYPE nNum);
	CMatrix<TYPE> operator/(TYPE nNum) { return ((*this)*(1/nNum)); }
	
	BOOL operator==(CMatrix<TYPE>& rMatrix) const;
	BOOL operator!=(CMatrix<TYPE>& rMatrix) const { return !(*this==rMatrix); }
	BOOL operator==(TYPE nValue) const;
	BOOL operator!=(TYPE nValue) const { return !(*this==nValue); }

	CMatrix<TYPE>& MultipleCells(CMatrix<TYPE>& b);
	static CMatrix<TYPE> MultipleCells(CMatrix<TYPE>& a,CMatrix<TYPE>& b);
	CMatrix<TYPE>& DivideCells(CMatrix<TYPE>& b);
	static CMatrix<TYPE> DivideCells(CMatrix<TYPE>& a,CMatrix<TYPE>& b);
	
	TYPE DotProduct(CMatrix<TYPE>& rMatrix) const; // Only in 1xm or mx1 vectors
	CMatrix<TYPE> VectorProduct(CMatrix<TYPE>& rMatrix) const; // Only in 1x3 or 3x1 vectors
	

// Special functions
	void GaussJordan(CMatrix<TYPE>* pMatB=NULL,GJSTAGEPROC pStageProc=NULL,DWORD dwData=0);

// Gauss-Jordan opetarions:
	void M(UINT i,TYPE nCoefficient) { MultipleLine(i,nCoefficient); }
	void A(UINT i,UINT j,TYPE nCoefficient) { AddLineToLine(i,j,nCoefficient); }
	void P(UINT i,UINT j) { SwapLines(i,j); };

public:
	UINT m,n;
	TYPE* pMatrix;
	
	MATRIXERRORPROC pErrorProc;
	DWORD dwData;
};

class CByteMatrix : public CMatrix<BYTE> // BYTE is module 256
{
public:
	CByteMatrix(UINT m,UINT n);
	CByteMatrix(UINT m,UINT n,const BYTE* pMData);
	CByteMatrix(UINT m,UINT n,BYTE firstcell,...);
	CByteMatrix(const CByteMatrix& rMatrix);
	
protected:
	CByteMatrix() {}
public:

	CByteMatrix Inv() const;
	BOOL IsSignular() const;
};

class CComplexMatrix : public CMatrix<CComplex>
{
public:
	CComplexMatrix(UINT m,UINT n);
	CComplexMatrix(UINT m,UINT n,const CComplex* pMData);
	CComplexMatrix(UINT m,UINT n,CComplex firstcell,...);
	CComplexMatrix(const CComplexMatrix& rMatrix);

protected:
	CComplexMatrix() {}
public:
	CComplexMatrix Hermite() const;
};

#define REAL				CReal
#define COMPLEX				CComplex
#define MATRIX(A)			CMatrix<A>
#define RMATRIX				CMatrix<CReal>
#define CMATRIX				CComplexMatrix
#define DMATRIX				CMatrix<DOUBLE>
#define BMATRIX				CByteMatrix

#include "math.inl"

#endif
