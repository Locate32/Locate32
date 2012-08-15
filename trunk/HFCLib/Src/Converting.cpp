////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////

#include "HFCLib.h"



void IntToBin(UINT num,LPTSTR szBuffer,UINT bits)
{
	UINT i,mask;
	for (i=0;i<bits;i++)
	{
		mask=(UINT)1<<i;
		if ((num&mask)==mask)
			szBuffer[bits-i-1]='1';
		else
			szBuffer[bits-i-1]='0';
	}
	szBuffer[bits]='\0';
}

UINT BinToInt(LPCTSTR szBuffer)
{
	UINT num=0,i;
	UINT len=(UINT)istrlen(szBuffer);

    for (i=0;i<len;i++)
	{
		if (szBuffer[i]!='1' && szBuffer[i]!='0')
		{
			SetHFCError(HFC_CORRUPTDATA);
			return FALSE;
		}
		if (szBuffer[i]=='1')
		{
		    if (len-i-1!=0)
			    num=(num|(1<<(len-i-1)));
			else
				num=num|1;
		}
	}
	if (szBuffer[0]!='1' && szBuffer[0]!='0')
	{
		SetHFCError(HFC_CORRUPTDATA);
		return FALSE;
	}
	return num;
}
