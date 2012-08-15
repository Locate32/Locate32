////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////

#include "HFCLib.h"



namespace Crypt // Implementation
{
	UINT EnCrypt(const PUCHAR szSrc,DWORD nDataLen,PUCHAR szDst,DWORD pKey[3]);
	UINT DeCrypt(const PUCHAR szSrc,DWORD nDataLen,PUCHAR szDst,DWORD pKey[3]);
	BOOL GetHill3Matrix(DWORD* pKey,CByteMatrix& rMatrix);
}

BOOL Crypt::GetHill3Matrix(DWORD* pKey,CByteMatrix& rMatrix)
{
	DWORD nCount=HIWORD(pKey[0])<<8|HIBYTE(LOWORD(pKey[0]));
	while (++nCount)
	{
		if (rMatrix.IsSignular())
		{
			mf(rMatrix,(nCount%7)%3,nCount%3)|=1;
			mf(rMatrix,(nCount%6)%3,nCount%3)++;
			mf(rMatrix,(nCount%8)%3,nCount%3)&=~1;
			continue;
		}
		return TRUE;
	}
	return FALSE;
}

#define PARAMS(A)		LOBYTE(LOWORD(A)),HIBYTE(LOWORD(A)),LOBYTE(HIWORD(A)),HIBYTE(HIWORD(A))

UINT Crypt::EnCrypt(const PUCHAR szSrc,DWORD nDataLen,PUCHAR szDst,DWORD pKey[3])
{
	CByteMatrix A(3,3,PARAMS(pKey[2]),PARAMS(pKey[1]),pKey[0]); // Initial data
	GetHill3Matrix(pKey,A);
	
	DWORD p;
	DWORD crc=0;
	DWORD nLen=DWORD(nDataLen/3)*3;
	for (p=0;p<nLen;p+=3)
	{
		szDst[p]=(BYTE)(mf(A,0,0)*szSrc[p]+mf(A,0,1)*szSrc[p+1]+mf(A,0,2)*szSrc[p+2]);
		szDst[p+1]=(BYTE)(mf(A,1,0)*szSrc[p]+mf(A,1,1)*szSrc[p+1]+mf(A,1,2)*szSrc[p+2]);
		szDst[p+2]=(BYTE)(mf(A,2,0)*szSrc[p]+mf(A,2,1)*szSrc[p+1]+mf(A,2,2)*szSrc[p+2]);
		crc^=szSrc[p]<<24|szSrc[p+1]<<16|szSrc[p+2]<<8;
	}
	if (nLen!=nDataLen)
	{
		UCHAR szExtraData[3];
		szExtraData[0]=szSrc[nLen];
		if (nDataLen-nLen==2)
			szExtraData[1]=szSrc[nLen+1];
		else
			szExtraData[1]=HIBYTE(HIWORD(pKey[0]));
		szExtraData[2]=LOBYTE(HIWORD(pKey[0]));
		szDst[nLen]=(BYTE)(mf(A,0,0)*szExtraData[0]+mf(A,0,1)*szExtraData[1]+mf(A,0,2)*szExtraData[2]);
		szDst[nLen+1]=(BYTE)(mf(A,1,0)*szExtraData[0]+mf(A,1,1)*szExtraData[1]+mf(A,1,2)*szExtraData[2]);
		szDst[nLen+2]=(BYTE)(mf(A,2,0)*szExtraData[0]+mf(A,2,1)*szExtraData[1]+mf(A,2,2)*szExtraData[2]);
		p=nLen+3;
		crc^=szExtraData[0]^szExtraData[1]^szExtraData[2];
	}
	((DWORD*)(szDst+p))[0]=crc;
	return p+5;
}

UINT Crypt::DeCrypt(const PUCHAR szSrc,DWORD nDataLen,PUCHAR szDst,DWORD pKey[3])
{
	BYTE badcrc=FALSE;
	CByteMatrix B(3,3,PARAMS(pKey[2]),PARAMS(pKey[1]),pKey[0]); // Initial data
	GetHill3Matrix(pKey,B);
	CByteMatrix A(inv(B));
	
	DWORD p;
	DWORD crc=0;
	DWORD nLen=DWORD(nDataLen/3)*3;
	for (p=0;p<nLen;p+=3)
	{
		szDst[p]=(BYTE)(mf(A,0,0)*szSrc[p]+mf(A,0,1)*szSrc[p+1]+mf(A,0,2)*szSrc[p+2]);
		szDst[p+1]=(BYTE)(mf(A,1,0)*szSrc[p]+mf(A,1,1)*szSrc[p+1]+mf(A,1,2)*szSrc[p+2]);
		szDst[p+2]=(BYTE)(mf(A,2,0)*szSrc[p]+mf(A,2,1)*szSrc[p+1]+mf(A,2,2)*szSrc[p+2]);
		crc^=szDst[p]<<24|szDst[p+1]<<16|szDst[p+2]<<8;
	}
	if (nLen!=nDataLen)
	{
		szDst[p]=(BYTE)(mf(A,0,0)*szSrc[p]+mf(A,0,1)*szSrc[p+1]+mf(A,0,2)*szSrc[p+2]);
		if (nDataLen-nLen==2)
			szDst[p+1]=(BYTE)(mf(A,1,0)*szSrc[p]+mf(A,1,1)*szSrc[p+1]+mf(A,1,2)*szSrc[p+2]);
		else
			if ((BYTE)(mf(A,1,0)*szSrc[p]+mf(A,1,1)*szSrc[p+1]+mf(A,1,2)*szSrc[p+2])!=HIBYTE(HIWORD(pKey[0])))
				badcrc=TRUE;
		if ((BYTE)(mf(A,2,0)*szSrc[p]+mf(A,2,1)*szSrc[p+1]+mf(A,2,2)*szSrc[p+2])!=LOBYTE(HIWORD(pKey[0])))
			badcrc=TRUE;
		crc^=szDst[p]^(BYTE)(mf(A,1,0)*szSrc[p]+mf(A,1,1)*szSrc[p+1]+mf(A,1,2)*szSrc[p+2])^(BYTE)(mf(A,2,0)*szSrc[p]+mf(A,2,1)*szSrc[p+1]+mf(A,2,2)*szSrc[p+2]);
		p+=3;
	}
	if (badcrc || ((DWORD*)(szSrc+p))[0]!=crc)
	{
		DebugMessage("CRC Error: Bad key or corrupt data");
		fMemSet(szDst,0,nDataLen);
		return 0;
	}
	return p;
}	
