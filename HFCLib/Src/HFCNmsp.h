////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////


#ifndef HFCNAMESPACES_H
#define HFCNAMESPACES_H

namespace Crypt // Interface
{
	UINT EnCrypt(const PUCHAR szSrc,DWORD nDataLen,PUCHAR szDst,DWORD pKey[3]);
	UINT DeCrypt(const PUCHAR szSrc,DWORD nDataLen,PUCHAR szDst,DWORD pKey[3]);
}

#ifdef WIN32
namespace VB // interface
{
	HFCERROR InputText32(HANDLE,PUCHAR,int);
	HFCERROR InputNum32(HANDLE,PINT);
	HFCERROR InputFloat32(HANDLE,PDOUBLE);
	HFCERROR PrintText32(HANDLE,LPCTSTR,TCHAR);
	HFCERROR PrintNum32(HANDLE,int,TCHAR);
	HFCERROR PrintFloat32(HANDLE,double,TCHAR);
	HFCERROR PrintBool32(HANDLE,UCHAR,TCHAR);
}
#endif

#endif
