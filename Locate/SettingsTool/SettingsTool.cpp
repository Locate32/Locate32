
#include <windows.h>
#include <stdio.h>
#include "resource.h"

#include "../common/win95crtfixes.h"

#define MAX_LOADSTRING 100


HINSTANCE hInst;

INT_PTR CALLBACK	DialogProc(HWND, UINT, WPARAM, LPARAM);

void ShowError(HWND hWnd,int nID,LONG nError)
{
	char szError[100];
	char szMsg[400];
	LoadString(hInst,IDS_ERROR,szError,100);
	LoadString(hInst,nID,szMsg,400);
	if (nError==0)
		MessageBox(hWnd,szMsg,szError,MB_ICONERROR|MB_OK);
	else
	{
		char* pMsg=NULL;
		char szMsg2[1000];
		if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,nError,LANG_USER_DEFAULT,(LPSTR)&pMsg,0,NULL)>0)
			sprintf_s(szMsg2,1000,szMsg,pMsg);
		else
			sprintf_s(szMsg2,1000,szMsg,"");
			
		MessageBox(hWnd,szMsg2,szError,MB_ICONERROR|MB_OK);

		if (pMsg!=NULL)
			LocalFree(pMsg);
	}
}

int WINAPI WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	hInst=hInstance;

	HANDLE hToken;
	if (OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES,&hToken))
	{
		PTOKEN_PRIVILEGES ns=(PTOKEN_PRIVILEGES)new BYTE[sizeof(DWORD)+sizeof(LUID_AND_ATTRIBUTES)+2];
		if (LookupPrivilegeValue(NULL,SE_BACKUP_NAME,&(ns->Privileges[0].Luid)))
		{
			ns->PrivilegeCount=1;
			ns->Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
			if (!AdjustTokenPrivileges(hToken,FALSE,ns,0,NULL,NULL))
				ShowError(NULL,IDS_ERRORCANNOTENABLEPRIVILEGE,GetLastError());
		}
		if (LookupPrivilegeValue(NULL,SE_RESTORE_NAME,&(ns->Privileges[0].Luid)))
		{
			ns->PrivilegeCount=1;
			ns->Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
			if (!AdjustTokenPrivileges(hToken,FALSE,ns,0,NULL,NULL))
				ShowError(NULL,IDS_ERRORCANNOTENABLEPRIVILEGE,GetLastError());

		}
		CloseHandle(hToken);
		delete[] (BYTE*)ns;
	}

	DialogBox(hInstance,MAKEINTRESOURCE(IDD_MAIN),NULL,DialogProc);

	return (int) 0;
}



LONG DeleteSubKey(HKEY hKey,LPCSTR szKey)
{
	HKEY hSubKey;
	DWORD cb;
	char szSubKey[200];
	FILETIME ft;

	LONG lRet=RegOpenKeyEx(hKey,szKey,0,
		KEY_ENUMERATE_SUB_KEYS|KEY_SET_VALUE,&hSubKey);

	if (lRet!=ERROR_SUCCESS)
		return lRet;	

	for (;;)
	{
		cb=200;
		if (RegEnumKeyEx(hSubKey,0,szSubKey,&cb,NULL,
			NULL,NULL,&ft)==ERROR_NO_MORE_ITEMS)
			break;
		DeleteSubKey(hSubKey,szSubKey);
	}
	RegCloseKey(hSubKey);
	RegDeleteKey(hKey,szKey);

	return ERROR_SUCCESS;
}

BOOL SaveSettings(HWND hWnd)
{
	char szPath[MAX_PATH]="";
	char szTitle[100],szFilter[200];
	LoadString(hInst,IDS_SAVESETTINGS,szTitle,100);
	
	OSVERSIONINFO ve;
	ve.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
	if (GetVersionEx(&ve))
	{
		if (ve.dwPlatformId==VER_PLATFORM_WIN32_NT)
			LoadString(hInst,IDS_SAVEFILTER,szFilter,200);
		else
			LoadString(hInst,IDS_SAVEFILTER9x,szFilter,200);
	}
	else
		LoadString(hInst,IDS_SAVEFILTER,szFilter,200);

	for (int i=0;szFilter[i]!='\0';i++)
	{
		if (szFilter[i]=='|')
			szFilter[i]='\0';
	}

	OPENFILENAME ofn;
	ZeroMemory(&ofn,sizeof(OPENFILENAME));
	ofn.lStructSize=OPENFILENAME_SIZE_VERSION_400;
	ofn.hwndOwner=hWnd;
	ofn.hInstance=hInst;
	ofn.lpstrFilter=szFilter;
	ofn.lpstrFile=szPath;
	ofn.nMaxFile=MAX_PATH;
	ofn.lpstrTitle=szTitle;
	ofn.Flags=OFN_ENABLESIZING|OFN_EXPLORER|OFN_LONGNAMES|OFN_OVERWRITEPROMPT;
	ofn.lpstrDefExt="*.reg";

	if (!GetSaveFileName(&ofn))
		return FALSE;
	
	if (ofn.nFilterIndex==1)
	{
		// Registry script
		char szCommand[2000];
		sprintf_s(szCommand,2000,"regedit /ea \"%s\" HKEY_CURRENT_USER\\Software\\Update",szPath);

		PROCESS_INFORMATION pi;
		STARTUPINFO si; // Ansi and Unicode versions are same
		ZeroMemory(&si,sizeof(STARTUPINFO));
		si.cb=sizeof(STARTUPINFO);
		
		if (CreateProcess(NULL,szCommand,NULL,
			NULL,FALSE,CREATE_DEFAULT_ERROR_MODE|NORMAL_PRIORITY_CLASS,
			NULL,NULL,&si,&pi))
		{
			WaitForSingleObject(pi.hProcess,2000);
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);	
		}
		else
			ShowError(hWnd,IDS_ERRORCANNOTRUNREGEDIT,GetLastError());
		return TRUE;		
	}

	// Data format
	HKEY hRegKey;
	LONG lRet=RegOpenKeyEx(HKEY_CURRENT_USER,"SOFTWARE\\Update",
		0,KEY_READ,&hRegKey);

	if (lRet!=ERROR_SUCCESS)
	{
		ShowError(hWnd,IDS_ERRORCANNOTOPENKEY,lRet);
		return FALSE;
	}

	DeleteFile(szPath);
	lRet=RegSaveKey(hRegKey,szPath,NULL);
	RegCloseKey(hRegKey);		
	
	if (lRet!=ERROR_SUCCESS)
	{
		ShowError(hWnd,IDS_ERRORCANNOTEXPORTSETTINGS,lRet);
		return FALSE;
	}

	return TRUE;
}

BOOL RestoreSettings(HWND hWnd)
{	
	// Check whether locate32 is running
	HWND hLocateSTWindow=FindWindow("LOCATEAPPST",NULL);
	if (hLocateSTWindow!=NULL)
	{
		char szText[100];
		LoadString(hInst,IDS_LOCATE32RUNNING,szText,100);
		if (MessageBox(hWnd,szText,NULL,MB_OKCANCEL|MB_ICONINFORMATION))
			return FALSE;
	}



	char szPath[MAX_PATH]="";
	char szTitle[100],szFilter[200];
	OSVERSIONINFO ve;
	ve.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
	if (GetVersionEx(&ve))
	{
		if (ve.dwPlatformId==VER_PLATFORM_WIN32_NT)
			LoadString(hInst,IDS_RESTOREFILTER,szFilter,200);
		else
			LoadString(hInst,IDS_RESTOREFILTER9x,szFilter,200);
	}
	else
		LoadString(hInst,IDS_RESTOREFILTER,szFilter,200);
	
	LoadString(hInst,IDS_RESTORESETTINGS,szTitle,100);

	for (int i=0;szFilter[i]!='\0';i++)
	{
		if (szFilter[i]=='|')
			szFilter[i]='\0';
	}

	OPENFILENAME ofn;
	ZeroMemory(&ofn,sizeof(OPENFILENAME));
	ofn.lStructSize=OPENFILENAME_SIZE_VERSION_400;
	ofn.hwndOwner=hWnd;
	ofn.hInstance=hInst;
	ofn.lpstrFilter=szFilter;
	ofn.lpstrFile=szPath;
	ofn.nMaxFile=MAX_PATH;
	ofn.lpstrTitle=szTitle;
	ofn.Flags=OFN_ENABLESIZING|OFN_EXPLORER|OFN_FILEMUSTEXIST|OFN_LONGNAMES|OFN_HIDEREADONLY;
	ofn.lpstrDefExt="*.reg";

	if (!GetOpenFileName(&ofn))
		return FALSE;
	
	int nDotIndex;
	for (nDotIndex=(int)strlen(szPath)-1;nDotIndex>=0 && szPath[nDotIndex]!='.';nDotIndex--);

	if (nDotIndex>=0 && _stricmp(szPath+nDotIndex+1,"reg")==0)
	{
		char szBackup[MAX_PATH];
		CopyMemory(szBackup,szPath,nDotIndex+1);
		strcpy_s(szBackup+nDotIndex+1,MAX_PATH-nDotIndex-1,"old.reg");
				
		// Backing up
		char szCommand[2000];
		sprintf_s(szCommand,2000,"regedit /ea \"%s\" HKEY_CURRENT_USER\\Software\\Update",szBackup);

		PROCESS_INFORMATION pi;
		STARTUPINFO si; // Ansi and Unicode versions are same
		ZeroMemory(&si,sizeof(STARTUPINFO));
		si.cb=sizeof(STARTUPINFO);
		
		if (CreateProcess(NULL,szCommand,NULL,
			NULL,FALSE,CREATE_DEFAULT_ERROR_MODE|NORMAL_PRIORITY_CLASS,
			NULL,NULL,&si,&pi))
		{
			WaitForSingleObject(pi.hProcess,2000);
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);	
		}
		else
			ShowError(hWnd,IDS_ERRORCANNOTRUNREGEDIT,GetLastError());

		
		
		// Restore key
		DeleteSubKey(HKEY_CURRENT_USER,"SOFTWARE\\Update");
		sprintf_s(szCommand,2000,"regedit /s \"%s\"",szPath);

		ZeroMemory(&si,sizeof(STARTUPINFO));
		si.cb=sizeof(STARTUPINFO);
		
		if (CreateProcess(NULL,szCommand,NULL,
			NULL,FALSE,CREATE_DEFAULT_ERROR_MODE|NORMAL_PRIORITY_CLASS,
			NULL,NULL,&si,&pi))
		{
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);	
		}
		else
			ShowError(hWnd,IDS_ERRORCANNOTRUNREGEDIT,GetLastError());


		return TRUE;		
	}
	
	// First, check that we can restore key
	HKEY hRegKey;
	LONG lRet=RegCreateKeyEx(HKEY_CURRENT_USER,"SOFTWARE\\Update_tmpTmp",
		0,NULL,REG_OPTION_BACKUP_RESTORE,KEY_ALL_ACCESS,NULL,&hRegKey,NULL);
	if (lRet!=ERROR_SUCCESS)
	{
		ShowError(hWnd,IDS_ERRORCANNOTCREATEKEY,lRet);
		return FALSE;
	}
	lRet=RegRestoreKey(hRegKey,szPath,0);
	RegCloseKey(hRegKey);		
	DeleteSubKey(HKEY_CURRENT_USER,"SOFTWARE\\Update_tmpTmp");

	if (lRet!=ERROR_SUCCESS)
	{
		ShowError(hWnd,IDS_ERRORCANNOTRESTOREKEY,lRet);
		return FALSE;
	}

	
	
	// Clear existing key
	DeleteSubKey(HKEY_CURRENT_USER,"SOFTWARE\\Update");
	
	// Restore key
	lRet=RegCreateKeyEx(HKEY_CURRENT_USER,"SOFTWARE\\Update",
		0,NULL,REG_OPTION_BACKUP_RESTORE,KEY_ALL_ACCESS,NULL,&hRegKey,NULL);

	if (lRet!=ERROR_SUCCESS)
	{
		ShowError(hWnd,IDS_ERRORCANNOTCREATEKEY,lRet);
		return FALSE;
	}
	
	
	lRet=RegRestoreKey(hRegKey,szPath,0);
	RegCloseKey(hRegKey);		
	if (lRet!=ERROR_SUCCESS)
	{
		ShowError(hWnd,IDS_ERRORCANNOTRESTOREKEY,lRet);
		return FALSE;
	}
	return TRUE;
}



void RemoveSettings(HWND hWnd)
{
	char szTitle[100];
	char szMsg[200];
	LoadString(hInst,IDS_REMOVESETTINGS,szTitle,100);
	LoadString(hInst,IDS_WANTBACKUP,szMsg,200);

	int nRet=MessageBox(hWnd,szMsg,szTitle,MB_ICONQUESTION|MB_YESNOCANCEL);

	if (nRet==IDCANCEL)
		return;
	if (nRet==IDYES)
	{
		if (!SaveSettings(hWnd))
			return;
	}

	HKEY hRegKey;
	LONG lRet=RegOpenKeyEx(HKEY_CURRENT_USER,"SOFTWARE",
		0,KEY_ALL_ACCESS|DELETE,&hRegKey);

	if (lRet!=ERROR_SUCCESS)
	{
		ShowError(hWnd,IDS_ERRORCANNOTOPENKEY2,lRet);
		return;
	}
	
	lRet=DeleteSubKey(hRegKey,"Update");
	RegCloseKey(hRegKey);		
	
	if (lRet!=ERROR_SUCCESS)
	{
		ShowError(hWnd,IDS_ERRORCANNOTDELETEKEY,lRet);
		return;
	}
	

}

INT_PTR CALLBACK DialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		{
			HICON hIcon=(HICON)LoadImage(hInst,MAKEINTRESOURCE(IDI_SETTINGSTOOL),IMAGE_ICON,32,32,LR_SHARED);
			//SetIcon(hWnd,hIcon,TRUE);
			SetClassLongPtr(hWnd,GCLP_HICON,(LONG_PTR)hIcon);
			hIcon=(HICON)LoadImage(hInst,MAKEINTRESOURCE(IDI_SETTINGSTOOL),IMAGE_ICON,16,16,LR_SHARED);
			//SetIcon(hWnd,hIcon,FALSE);
			SetClassLongPtr(hWnd,GCLP_HICONSM,(LONG_PTR)hIcon);
			break;
		}
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			break;
		case IDCANCEL:
			EndDialog(hWnd,0);
			break;
		case IDC_SAVESETTINGS:
			SaveSettings(hWnd);
			break;
		case IDC_RESTORESETTINGS:
			RestoreSettings(hWnd);
			break;
		case IDC_REMOVESETTINGS:
			RemoveSettings(hWnd);
			break;
		default:
			return DefDlgProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_CLOSE:
		EndDialog(hWnd,0);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	//default:
		//return DefDlgProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
