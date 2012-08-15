////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////

#include "HFCLib.h"
#include <math.h>


UINT powi(USHORT x,BYTE y)
{
	UINT ret=x;
	if (!y)
		return 1;
	for (BYTE i=1;i<y;i++)
		ret=ret*x;
	return ret;
}


UINT powi10(BYTE y)
{
	UINT ret=10;
	if (!y)
		return 1;
	for (BYTE i=1;i<y;i++)
		ret=ret*10;
	return ret;
}

UINT fact(BYTE x)
{
	UINT ret=1;
	for (BYTE i=2;i<=x;i++)
		ret=ret*i;
	return ret;
}

double integral(PDIFF_FUNC pFunc,double a,double b,int n)
{
	if (n<0)
		n=int(a+b);
	double dx=fabs(b-a)/(double)powi2((30<n?30:n));
	double s=0,S=0;
	if (a<b)
	{
		for (double x=a;x<b;x+=dx)
		{
			s+=pFunc(x)*dx;
			S+=pFunc(x+dx)*dx;
		}
		return (s+S)/2;
	}
	else
	{
		for (double x=b;x<a;x+=dx)
		{
			s+=pFunc(x)*dx;
			S+=pFunc(x+dx)*dx;
		}
		return -(s+S)/2;
	}
}

double sum(PSUM_FUNC pFunc,int i,int k)
{
	double ret=0;
	for (int j=i;j<=k;j++)
		ret+=pFunc(j);
	return ret;
}

unsigned int ccd(int a,int b)
{
	if (!a || !b)
		return 0;
	if (a<0)
		a=-a;
	if (b<0)
		b=-b;
	if (a<b)
	{
		int tmp=a;
		a=b;
		b=tmp;
	}
	int c=a%b;
	while (c)
	{
		a=b;
		b=c;
		c=a%b;
	}
	return b;
}
