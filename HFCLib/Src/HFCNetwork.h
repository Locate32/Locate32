////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////


#ifndef HFCNETWORK_H
#define HFCNETWORK_H

#ifdef WIN32

namespace Network {
	// Resolved computer name from ID list
	DWORD GetComputerNameFromIDList(LPITEMIDLIST lpiil,LPSTR szName,DWORD dwBufferLen);
#ifdef DEF_WCHAR
	DWORD GetComputerNameFromIDList(LPITEMIDLIST lpiil,LPWSTR szName,DWORD dwBufferLen);


	// Resolved nethood target
	BOOL GetNethoodTarget(LPCWSTR szFolder,LPWSTR szTarget,DWORD nBufferLen);
#endif

};

#endif
#endif