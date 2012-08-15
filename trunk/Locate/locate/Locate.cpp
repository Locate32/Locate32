/* Copyright (c) 1997-2010 Janne Huttunen
   database locater v3.1.10.8220              */

const char* szVersionStr="locate 3.1 RC3m build 10.8220";

#include <hfclib.h>
#ifndef WIN32
#include <go32.h>
#include <sys/farptr.h>
#include <conio.h>
#endif
#include <locale.h>
      
#include "../locater/locater.h"
#include "../lan_resources.h"
#include "../common/common.h"

#include "../common/win95crtfixes.h"


int Lines=0;

enum {
	flagWholeWord=0x1,
	flagReplaceSpaces=0x2,
	flagShowWhatAreWeLookingFor=0x4,
	flagOutputIsPaged=0x8,
	flagNoSubDirectories=0x10
};
BYTE dwMainFlags=flagReplaceSpaces;


#ifdef WIN32
HANDLE hStdOut;
HANDLE hStdIn;
volatile BOOL nShouldQuit=FALSE;

UINT g_uiOriginalConsoleCP;
LPSTR g_szRegKey;
LPSTR g_szRegFile;
BYTE g_bFileIsReg;

void SetRegKey(int argc,wchar_t* argv[])
{
	CString Branch;

	int i;
	for (i=1;i<argc;i++)
	{
		if (argv[i][0]==L'-' || argv[i][0]==L'/')
		{
			switch (argv[i][1])
			{
	        case L'X':
				if (argv[i][2]==L'\0')
				{
					if (i<argc-1)
						Branch=argv[++i];
				}
				else
	                   Branch=(argv[i]+2);
				break;
			}
		}
	}
	
	
	g_szRegKey=ReadIniFile(&g_szRegFile,Branch.GetPData(),g_bFileIsReg);
	
	if (g_szRegKey!=NULL)
	{
		if (g_szRegFile!=NULL)
			LoadSettingsFromFile(g_szRegKey,g_szRegFile,g_bFileIsReg);
	}
	else
	{
		// Use default
		g_szRegKey=alloccopy("Software\\Update");
	}

}

void FinishRegKey()
{
	if (g_szRegKey==NULL)
		return;
	
	if (g_szRegFile!=NULL)
	{
		SaveSettingsToFile(g_szRegKey,g_szRegFile,g_bFileIsReg);

		delete[] g_szRegFile;
	}

	delete[] g_szRegKey;
}


DWORD GetConsoleLines()
{
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
	if (!GetConsoleScreenBufferInfo(hStdOut,&csbiInfo))
		return 24;		
	return csbiInfo.srWindow.Bottom-csbiInfo.srWindow.Top;
}
#endif

BOOL SetLanguageSpecifigHandles(LPCWSTR szAppPath)
{
	CRegKey RegKey;
	CStringW LangFile;
	if (RegKey.OpenKey(HKCU,g_szRegKey,
		CRegKey::openExist|CRegKey::samRead)==ERROR_SUCCESS)
	{
		RegKey.QueryValue(L"Language",LangFile);
		RegKey.CloseKey();
	}
	if (LangFile.IsEmpty())
		LangFile=L"lan_en.dll";

	CStringW Path(szAppPath,LastCharIndex(szAppPath,L'\\')+1);
	
	HINSTANCE hLib=FileSystem::LoadLibrary(Path+LangFile);
	if (hLib==NULL)
	{
		hLib=FileSystem::LoadLibrary(Path+L"lan_en.dll");

		if (hLib==NULL)
		{
			fwprintf(stderr,L"Cannot load language file '%s'\n",(LPCWSTR)LangFile);
			return FALSE;
		}

		fwprintf(stderr,L"Cannot load language file '%s', using 'lan_en.dll'\n",(LPCWSTR)LangFile);
	}

	SetResourceHandle(hLib,LanguageSpecificResource);
	return TRUE;
}

    
BOOL CALLBACK LocateProc(DWORD_PTR dwParam,CallingReason crReason,UpdateError ue,DWORD_PTR dwInfo,const CLocater* pLocater)
{          
	switch (crReason)
	{
	case ScanningDatabase:
		if (IsUnicodeSystem())
		{
			BOOL (WINAPI *pSetConsoleOutputCP)(UINT)=
				(BOOL (WINAPI *)(UINT))GetProcAddress(GetModuleHandle("kernel32.dll"),"SetConsoleOutputCP");

			if (pSetConsoleOutputCP!=NULL)
				pSetConsoleOutputCP(pLocater->IsCurrentDatabaseUnicode()?CP_UTF8:g_uiOriginalConsoleCP);
			else
				_wsetlocale(LC_CTYPE,L".OCP"); 
		}
		break;
	case ErrorOccured:
		switch (ue)
		{
		case ueUnknown:
			fwprintf(stderr,(LPCWSTR)ID2W(IDS_LOCATEUNKNOWNERROR));
			break;
		case ueOpen:
			fwprintf(stderr,(LPCWSTR)ID2W(IDS_LOCATECANNOTOPEN),pLocater->GetCurrentDatabaseFile());
			break;
		case ueRead:
			fwprintf(stderr,(LPCWSTR)ID2W(IDS_LOCATECANNOTREAD),pLocater->GetCurrentDatabaseFile());
			break;
		case ueInvalidDatabase:
			fwprintf(stderr,(LPCWSTR)ID2W(IDS_LOCATEINVALIDDATABASE),pLocater->GetCurrentDatabaseFile());
			break;
		}
		break;
	}
    return TRUE;
}

#ifdef WIN32
BOOL WINAPI HandlerRoutine(DWORD dwCtrlType)
{
	switch (dwCtrlType)
	{
	case CTRL_C_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_CLOSE_EVENT:
		nShouldQuit=TRUE;
		break;
	}
	return 1;
}
#endif

void showverbose(LPCWSTR* ppStrings,UINT nStrings,LPCWSTR* ppExtensions,UINT nExtension,LPCWSTR* ppDirectories,UINT nDirectories)
{
	if (nStrings>0)
	{
		wprintf(ID2W(IDS_LOCATEVERBOSESTR));
		wprintf(ppStrings[0]);

		for (UINT i=1;i<nStrings;i++)
			wprintf(L", %s",ppStrings[i]);
		putchar('\n');
	}

	if (nExtension>0)
	{
		wprintf(ID2W(IDS_LOCATEVERBOSEEXT));
		wprintf(ppExtensions[0]);

		for (UINT i=1;i<nExtension;i++)
			wprintf(L", %s",ppExtensions[i]);
		putchar('\n');
	}


	if (nDirectories>0)
	{
		wprintf(ID2W(IDS_LOCATEVERBOSEDIR));
		for (UINT i=0;i<nDirectories;i++)
			wprintf(L"%s\n",ppDirectories[i]);
	}
	putchar('\n');
}

BOOL CALLBACK LocateFoundProc(DWORD_PTR dwParam,BOOL bFolder,const CLocater* pLocater)
{
	if (nShouldQuit)
		return FALSE;

	if (dwMainFlags&flagOutputIsPaged)
	{
#ifdef WIN32
		if (Lines>int(GetConsoleLines()-1))
#else
		if (Lines>(_farpeekb(_dos_ds,0x484)-1))
#endif
		{
			DWORD nTmp=1;char szbuf[2];
			SetConsoleMode(hStdIn,0);
			ReadConsole(hStdIn,szbuf,1,&nTmp,NULL);
			SetConsoleMode(hStdIn,ENABLE_PROCESSED_INPUT);
			if (szbuf[0]==3) // Ctrl+C
				return FALSE;
			Lines=0;
		}
		Lines++;
	}
	if (bFolder)
		printf("%s\\%s\n",pLocater->GetCurrentPath(),pLocater->GetFolderName());
	else
		printf("%s\\%s\n",pLocater->GetCurrentPath(),pLocater->GetFileName());
	return TRUE;
}



BOOL CALLBACK LocateFoundProcW(DWORD_PTR dwParam,BOOL bFolder,const CLocater* pLocater)
{
	if (nShouldQuit)
		return FALSE;

	if (dwMainFlags&flagOutputIsPaged)
	{
#ifdef WIN32
		if (Lines>int(GetConsoleLines()-1))
#else
		if (Lines>(_farpeekb(_dos_ds,0x484)-1))
#endif
		{
			DWORD nTmp=1;char szbuf[2];
			SetConsoleMode(hStdIn,0);
			ReadConsole(hStdIn,szbuf,1,&nTmp,NULL);
			SetConsoleMode(hStdIn,ENABLE_PROCESSED_INPUT);
			if (szbuf[0]==3) // Ctrl+C
				return FALSE;
			Lines=0;
		}
		Lines++;
	}
	
	
	/*if (bFolder)
		wprintf(L"%s\\%s\n",pLocater->GetCurrentPathW(),pLocater->GetFolderNameW());
	else
		wprintf(L"%s\\%s\n",pLocater->GetCurrentPathW(),pLocater->GetFileNameW());*/
	
	
	

	if (bFolder)
		wprintf(L"%S\\%S\n",(LPCSTR)W2UTF8(pLocater->GetCurrentPathW()),(LPCSTR)W2UTF8(pLocater->GetFolderNameW()));
	else
		wprintf(L"%S\\%S\n",(LPCSTR)W2UTF8(pLocater->GetCurrentPathW()),(LPCSTR)W2UTF8(pLocater->GetFileNameW()));

	return TRUE;
}




int wmain (int argc,wchar_t * argv[])
{
#ifdef _DEBUG
	CAppData::stdfunc();
#endif

	//_wsetlocale(LC_CTYPE,L".OCP"); 

#ifdef WIN32
	hStdOut=GetStdHandle(STD_OUTPUT_HANDLE);
	hStdIn=GetStdHandle(STD_INPUT_HANDLE);
	SetConsoleMode(hStdIn,ENABLE_PROCESSED_INPUT);
	SetConsoleCtrlHandler(HandlerRoutine,TRUE);

	g_uiOriginalConsoleCP=GetConsoleOutputCP();
	
#endif

	InitLocaterLibrary();

	SetRegKey(argc,argv);

	CString RegKey("");	

	if (!SetLanguageSpecifigHandles(argv[0]))
	{
#ifdef _DEBUG
		fprintf(stderr,"No language dll file");
#else
		return 1;
#endif
	}

	CArrayFP<CDatabase*> aDatabases;
	struct {
		BYTE helps:3;
		BYTE verbose:1;
		BYTE logicalops:1;
		BYTE noargs:1;
	} options={0,0,0,0};

	ULONGLONG ulMinSize=ULONGLONG(-1);
	ULONGLONG ulMaxSize=ULONGLONG(-1);
	WORD wMaxDate=WORD(-1);
	WORD wMinDate=WORD(-1);

	DWORD dwMaxFoundFiles=DWORD(-1);
	DWORD dwFlags=LOCATE_FILENAMES|LOCATE_CONTAINTEXTISMATCHCASE;
	DWORD dwExtraFlags=0;
	BYTE* pContainData=NULL;
	DWORD dwContainDataLength=0;

	CStringW String;
	CArrayFAP<LPWSTR> aDirectories;
	CArrayFAP<LPWSTR> aExtensions;
	
	int i;
	for (i=1;i<argc;i++)
	{
		if ((argv[i][0]==L'-' || argv[i][0]==L'/') && !options.noargs)
		{
			switch (argv[i][1])
			{
			case L'X':
				if (argv[i][2]==L'\0')
				{
					if (i<argc-1)
						i++; // RegKeyName already set
				}
				break;
			case L's':
			case L'S':
				dwMainFlags|=flagOutputIsPaged;
				if (argv[i][2]=='c' || argv[i][2]=='C')
	           			dwFlags|=LOCATE_CONTAINTEXTISMATCHCASE;
				break;
			case L'c':
			case L'C':
           		dwFlags|=LOCATE_CONTAINTEXTISMATCHCASE;
                if (argv[i][2]=='s' || argv[i][2]=='S')
					dwMainFlags|=flagOutputIsPaged;
    			break;
			case L'i':
				dwMainFlags|=flagShowWhatAreWeLookingFor;
				break;
            case L'd':
				{
					// Using database by file
					CStringW sFile;
					if (argv[i][2]==L'\0')
					{
						if (i>=argc-1)
							sFile=L"";
						else
							sFile=argv[++i];
					}
					else
	                    sFile=(argv[i]+2);
					sFile.ReplaceChars('/','\\');
					if (CDatabase::FindByFile(aDatabases,sFile)==NULL)
					{
						CDatabase* pDatabase=CDatabase::FromFile(sFile);
						if (pDatabase!=NULL)
							aDatabases.Add(pDatabase);
					}
					break;
				}
			case L'D':
				{
					// Using database by name
					CStringW sName;
					if (argv[i][2]==L'\0')
					{
						if (i>=argc-1)
							sName=L"";
						else
							sName=argv[++i];
					}
					else
	                    sName=(argv[i]+2);
	
					if (CDatabase::FindByName(aDatabases,sName)==NULL)
					{
						CDatabase* pDatabase=CDatabase::FromName(HKCU,
							CString(g_szRegKey)+"\\Databases",sName);
						if (pDatabase!=NULL)
						{
							pDatabase->SetThreadId(0);
							aDatabases.Add(pDatabase);
						}
					}
					break;
				}
			case L'p':
			case L'P':
				{
					CStringW sDirectory;
					if (argv[i][2]==L'\0')
					{
						if (i<argc-1)
						{
							++i;
							sDirectory=argv[i];
						}
					}
					else
						sDirectory=argv[i]+(argv[i][2]==L':'?3:2);
	
					sDirectory.ReplaceChars('/','\\');

					WCHAR szPath[MAX_PATH];
					int nRet=FileSystem::GetFullPathName(sDirectory,MAX_PATH,szPath,NULL);
						

					if (nRet>0)
						aDirectories.Add(alloccopy(szPath,nRet));
					else
						aDirectories.Add(alloccopy(sDirectory));
				}
				break;
			case 't':
			case 'T': // Extensions
				{
					LPCWSTR szExtensions=NULL;				
					if (argv[i][2]==L'\0')
					{
						if (i>=argc-1)
						{
							aExtensions.Add(allocemptyW());
							break;
						}
						else
							szExtensions=argv[++i];
					}
					else
						szExtensions=argv[i]+2;

					ASSERT_VALID(szExtensions);
					
					// Parse spaces
					for (;;) 
					{
						int nLen=FirstCharIndex(szExtensions,L' ');
						if (nLen==-1)
						{
							aExtensions.Add(alloccopy(szExtensions));
							break;
						}
						aExtensions.Add(alloccopy(szExtensions,nLen));
						szExtensions+=nLen+1;
					}
					break;
				}
			case L'r':
				dwFlags|=LOCATE_NAMEREGULAREXPRESSION;
				if (argv[i][2]==L'c' || argv[i][2]==L'C')
					dwFlags|=LOCATE_REGEXPCASESENSITIVE;
				break;
			case L'w':
			case L'W':
				dwFlags|=LOCATE_CHECKWHOLEPATH;
				break;
			case L'v':
				options.verbose=1;
				break;
			case L'V':
				options.helps=2;
				break;
			case L'?':
			case L'h':
			case L'H':
				options.helps=1;
				break;
			case L'R':
				if (argv[i][2]==L'n' || argv[i][2]==L'N')
					dwFlags|=LOCATE_NOSUBDIRECTORIES;
				else
					dwFlags&=~LOCATE_NOSUBDIRECTORIES;
				break;
			case L'+':
				if (argv[i][2]==L'n' || argv[i][2]==L'N')
					options.logicalops=0;
				else
					options.logicalops=1;
				break;
			case L'-':
				options.noargs=1;
				break;
			case L'l':
			case L'L':
			{
				WCHAR* ep;
            	switch (argv[i][2])
            	{
				case L'w':
					if (argv[i][3]==L'n')
						dwMainFlags&=~flagWholeWord;
					else
						dwMainFlags|=flagWholeWord;
					break;
				case L'r':
					if (argv[i][3]==L'n')
						dwMainFlags&=~flagReplaceSpaces;
					else
						dwMainFlags|=flagReplaceSpaces;
					break;
				case L'm':
            		if (argv[i][3]==L':')
            			ulMinSize=_wcstoi64(argv[i]+4,&ep,0);
            		else
            			ulMinSize=_wcstoi64(argv[i]+3,&ep,0);
                    if (*ep==L'k' || *ep==L'K')
                       ulMinSize*=1024;
                    else if (*ep==L'M' || *ep==L'm')
                         ulMinSize*=1024*1024;
	           		break;
            	case L'M':
            		if (argv[i][3]==L':')
            			ulMaxSize=_wcstoi64(argv[i]+4,&ep,0);
            		else
            			ulMaxSize=_wcstoi64(argv[i]+3,&ep,0);
                    if (*ep==L'k' || *ep==L'K')
                       ulMaxSize*=1024;
                    else if (*ep==L'M' || *ep==L'm')
                         ulMaxSize*=1024*1024;
	           		break;
            	case L'f':
            		dwFlags&=~LOCATE_FOLDERNAMES;
            		dwFlags|=LOCATE_FILENAMES;
            		if (argv[i][3]==L'd' || argv[i][3]==L'D') 
						dwFlags|=LOCATE_FOLDERNAMES;
            		break;
            	case L'd':                      
            		dwFlags&=~LOCATE_FILENAMES;
            		dwFlags|=LOCATE_FOLDERNAMES;
            		if (argv[i][3]==L'f' || argv[i][3]==L'F')
						dwFlags|=LOCATE_FILENAMES;
            		break;
            	case L'n':
            		if (argv[i][3]==':')
            			dwMaxFoundFiles=_wtol(argv[i]+4);
            		else
            			dwMaxFoundFiles=_wtol(argv[i]+3);
	           		break;
	           	case L'c':
	           	case L'C':
	           		if (argv[i][3]==L'm')
	           			dwFlags|=LOCATE_CONTAINTEXTISMATCHCASE;
            		else if (argv[i][3]==L'n')
	           			dwFlags&=~LOCATE_CONTAINTEXTISMATCHCASE;
            		else if (argv[i][3]==L':')
					{
	            		if (pContainData!=NULL)
							free(pContainData);
						dwFlags&=~LOCATE_REGULAREXPRESSIONSEARCH;
            			pContainData=dataparser(argv[i]+4,&dwContainDataLength);
					}
            		else
					{
	            		if (pContainData!=NULL)
							free(pContainData);
            			dwFlags&=~LOCATE_REGULAREXPRESSIONSEARCH;
            			pContainData=dataparser(argv[i]+3,&dwContainDataLength);
					}
            		break;
	           	case L'R':
	           		{
					
						dwFlags|=LOCATE_REGULAREXPRESSIONSEARCH;
            			if (pContainData!=NULL)
							delete[] pContainData;
					
						int j=argv[i][3]==L':'?4:3;
						if (argv[i][j]==L'\0')
						{
							i++;j=0;
						}

						if (argv[i][j]==L'\"')
						{						
							j++;
							dwContainDataLength=(DWORD)FirstCharIndex(argv[i]+j,L'\"');
							if (dwContainDataLength==DWORD(-1))
								dwContainDataLength=istrlenw(argv[i]+j);
							pContainData=(PBYTE)alloccopyWtoA(argv[i]+j,dwContainDataLength);
						}
						else
						{
							dwContainDataLength=(DWORD)istrlenw(argv[i]+j);
							pContainData=(PBYTE)alloccopyWtoA(argv[i]+j,dwContainDataLength);
						}
					}
					break;
				case L'D': // dates
				{
					const WCHAR* lpCmdLine=argv[i]+3;

					int nLength=(int)istrlenw(lpCmdLine);

					if (nLength<2)
                        break;

					BOOL bMaxDate=isupper(lpCmdLine[0]);
					if (bMaxDate) 
					{
						switch (lpCmdLine[0])
						{
						case L'A':
							dwFlags|=LOCATE_MAXACCESSDATE;
							break;
						case L'C':
							dwFlags|=LOCATE_MAXCREATIONDATE;
							break;
						}
					}
					else
					{
						switch (lpCmdLine[0])
						{
						case L'a':
							dwFlags|=LOCATE_MINACCESSDATE;
							break;
						case L'c':
							dwFlags|=LOCATE_MINCREATIONDATE;
							break;
						}
					}
					lpCmdLine++;
					nLength--;

					WORD wDate;
					if (nLength<6)
					{
						int nRel=_wtoi(lpCmdLine);
						SYSTEMTIME st;
						GetLocalTime(&st);
						CTime::DecreaseDaysInSystemTime(&st,nRel);
						wDate=(BYTE(st.wYear-1980)<<9)|(st.wMonth<<5)|(st.wDay);
					}
					else
					{
						WCHAR szBuf[]=L"XX";
						szBuf[0]=lpCmdLine[0];
						szBuf[1]=lpCmdLine[1];
						WORD bYear=_wtoi(szBuf);
						if (bYear<60)
							bYear+=2000;
						else
							bYear+=1900;
						bYear-=1980;
						szBuf[0]=lpCmdLine[2];
						szBuf[1]=lpCmdLine[3];
						BYTE bMonth=_wtoi(szBuf);
						if (bMonth<1 || bMonth>12)
							bMonth=1;
						szBuf[0]=lpCmdLine[4];
						szBuf[1]=lpCmdLine[5];
						BYTE bDay=_wtoi(szBuf);
						if (bDay<1 || bDay>CTime::GetDaysInMonth(bMonth,bYear))
							bDay=1;					
						wDate=(BYTE(bYear)<<9)|(bMonth<<5)|(bDay);
					}
					
					if (bMaxDate) // max date
						wMaxDate=wDate;
					else
						wMinDate=wDate;
					
					break;
				}
				}
            }
			}
      }
      else
      {
		  if (dwFlags&LOCATE_NAMEREGULAREXPRESSION)
		  {
				if (argv[i][0]==L'\"')
				{
					String << (argv[i]+1);
					if (String.LastChar()==L'\"')
						String.DelLastChar();
				}
				else
					String << argv[i];
		  }
		  else
		  {
				if (!String.IsEmpty())
				{
					// Inserting '/' or space 
					if (dwMainFlags&flagReplaceSpaces)
						String << L'*';
					else
						String << L' ';
	
				}
				
				String << argv[i];
		  }
		}
	}

	if (!(dwFlags&LOCATE_NAMEREGULAREXPRESSION))
		String.ReplaceChars('/','\\');		

   
	if (options.helps==1)
	{
		fprintf(stdout,"%s\n",szVersionStr);
		
		HRSRC hRc=FindResource(GetLanguageSpecificResourceHandle(),MAKEINTRESOURCE(IDR_LOCATEHELP),"HELPTEXT");
		HGLOBAL hGlobal=LoadResource(GetLanguageSpecificResourceHandle(),hRc);
		LPCSTR pStr=(LPCSTR)LockResource(hGlobal);
		
		// Counting length
		int len;
		for (len=0;pStr[len]!='\0';len++)
		{
			if (pStr[len]=='E' && pStr[len+1]=='O' && pStr[len+2]=='F')
				break;
		}


		fwrite(pStr,1,len,stdout);
		
		FreeLibrary(GetLanguageSpecificResourceHandle());
		return 1;
	}
	else if (options.helps==2)
	{
		puts(szVersionStr);

		FreeLibrary(GetLanguageSpecificResourceHandle());
		return 1;
	}

	// Checking databases
	// First, check that there is database 
	if (aDatabases.GetSize()==0)
		CDatabase::LoadFromRegistry(HKCU,CString(g_szRegKey)+"\\Databases",aDatabases);   
	
	// If there is still no any available database, try to load old style db
	if (aDatabases.GetSize()==0)
	{
		CDatabase* pDatabase=CDatabase::FromOldStyleDatabase(HKCU,CString(g_szRegKey)+"\\Database");
		if (pDatabase==NULL)
			pDatabase=CDatabase::FromDefaults(TRUE); // Nothing else can be done?
		aDatabases.Add(pDatabase);
	}
	CDatabase::CheckValidNames(aDatabases);
	CDatabase::CheckDoubleNames(aDatabases);

	
	if (aExtensions.GetSize()==0)
		dwFlags|=LOCATE_EXTENSIONWITHNAME;

#ifdef _DEBUG
	wprintf (L"flags: %X name: %s\n",dwMainFlags,(LPCWSTR)String);
	Lines++;
#endif
	if (dwMainFlags&flagShowWhatAreWeLookingFor)
	{
		wprintf(ID2W(IDS_LOCATESTRING),(LPCWSTR)String);
		Lines++;
	}
	
   
	CLocater locater(aDatabases);
	locater.SetSizeAndDate(dwFlags,ulMinSize,ulMaxSize,wMinDate,wMaxDate);
	locater.SetAdvanced(dwFlags,pContainData,(DWORD)min(dwContainDataLength,MAXDWORD),dwMaxFoundFiles);

	locater.SetFunctions(LocateProc,LocateFoundProc,LocateFoundProcW,NULL);

	
	if (dwFlags&LOCATE_NAMEREGULAREXPRESSION)
	{
		if (options.verbose)
		{
			wprintf(ID2W(IDS_LOCATEVERBOSEREG),(LPCWSTR)String);
			putchar(' ');

			if (aDirectories.GetSize()>0)
			{
				wprintf(ID2W(IDS_LOCATEVERBOSEDIR));
				for (int i=0;i<aDirectories.GetSize();i++)
					printf("%s\n",aDirectories[i]);
			}
			putchar('\n');
		}

		LPCWSTR names[1];
		names[1]=String;
		locater.LocateFiles(FALSE,names,1,NULL,0,
			(LPCWSTR*)aDirectories.GetData(),aDirectories.GetSize());
	}
	else if (!String.IsEmpty())
	{
		int nIndex=(int)String.FindFirst(',');
		if (nIndex==-1)
		{
			if (options.logicalops &&
				(String[0]==L'+' || String[0]==L'-'))
			{
				locater.AddAdvancedFlags(LOCATE_LOGICALOPERATIONS);

				// Inserting '*':s if needed
				if (!(dwMainFlags&flagWholeWord))
				{
					if (String[1]!=L'*')
						String.InsChar(1,L'*');
					if (String.LastChar()!=L'*')
						String << L'*';
				}
			}
			else
			{
				// Inserting '*':s if needed
				if (!(dwMainFlags&flagWholeWord))
				{
					if (String[0]!=L'*')
						String.InsChar(0,L'*');
					if (String.LastChar()!=L'*')
						String << L'*';
				}
			}


		
			
			
			LPCWSTR s=String;

			if (options.verbose)
			{
				showverbose(&s,1,
					(LPCWSTR*)aExtensions.GetData(),aExtensions.GetSize(),
					(LPCWSTR*)aDirectories.GetData(),aDirectories.GetSize());
			}

			locater.LocateFiles(FALSE,&s,1,
				(LPCWSTR*)aExtensions.GetData(),aExtensions.GetSize(),
				(LPCWSTR*)aDirectories.GetData(),aDirectories.GetSize());
		}
		else
		{
			// Separate strings
			
			CArrayFAP<LPWSTR> aStrings;
			LPCWSTR pStr=String;
			BOOL bContinue=TRUE;
			BOOL bPlusOrMinusFound=FALSE;

			for (;;)
			{
				
				if (nIndex==-1)
				{
					bContinue=FALSE;
					nIndex=(int)istrlenw(pStr);
				}

				// First, if logical operations can be used, check is + or - present
				enum {
					NotSpecified,
					MustExist, // + found
					MustNotExist // - found
				} nType=NotSpecified;
				
				if (options.logicalops)
				{
					if (pStr[0]==L'+')
					{
						nType=MustExist;
						bPlusOrMinusFound=TRUE;
						pStr++;
						nIndex--;
					}
					else if (pStr[0]==L'-')
					{
						nType=MustNotExist;
						bPlusOrMinusFound=TRUE;
						pStr++;
						nIndex--;
					}
				}

				if (nIndex>0)
				{
					if (dwMainFlags&flagWholeWord)
					{
						if (nType==NotSpecified)
							aStrings.Add(alloccopy(pStr,nIndex));
						else
						{
							// Insert '+' or '-'
							WCHAR* pNewString=new WCHAR[nIndex+2];
							pNewString[0]=nType==MustExist?L'+':L'-';
							MemCopyW(pNewString+1,pStr,nIndex);
							pNewString[nIndex+1]='\0';
							aStrings.Add(pNewString);
						}
					}
					else
					{
						// Inserting '*'
						// 4 is enough for '*' at the begin and end of string,
						// for '\0' and for '+' or '-'
						WCHAR* pNewString=new WCHAR[nIndex+4]; 

						if (nType!=NotSpecified)
						{
							pNewString[0]=nType==MustExist?L'+':L'-';
							// Move pointer forward
							pNewString++;
						}
						
						if (pStr[0]!=L'*')
						{
							pNewString[0]=L'*';
							MemCopyW(pNewString+1,pStr,nIndex);
							if (pStr[nIndex-1]!=L'*')
							{	
								pNewString[nIndex+1]=L'*';
								pNewString[nIndex+2]=L'\0';
							}
							else
								pNewString[nIndex+1]=L'\0';
						}
						else
						{
							MemCopyW(pNewString,pStr,nIndex);
							if (pStr[nIndex-1]!=L'*')
							{	
								pNewString[nIndex]=L'*';
								pNewString[nIndex+1]=L'\0';
							}
							else
								pNewString[nIndex]='\0';
						}
						
						
						if (nType!=NotSpecified)
						{
							// Move pointer back
							pNewString--;
						}
						aStrings.Add(pNewString);

					}

					if (!bContinue)
						break;

					pStr+=nIndex+1;
					nIndex=(int)FirstCharIndex(pStr,L',');
				
				}
			
			}

			// Enable LogicalOperations
			if (bPlusOrMinusFound)
			{
				// '+' or '-' found
				locater.AddAdvancedFlags(LOCATE_LOGICALOPERATIONS);

				// Insert '+' for string which does not have '+' or '-'
				for (int i=0;i<aStrings.GetSize();i++)
				{
					if (aStrings[i][0]!=L'+' && aStrings[i][0]!=L'-')
					{
						int nLen=istrlen(aStrings[i])+1;
						LPWSTR pNew=new WCHAR[nLen+1];
						pNew[0]=L'+';
						MemCopyW(pNew+1,aStrings[i],nLen);
						delete[] aStrings[i];
						aStrings[i]=pNew;
					}
				}
			}

			if (options.verbose)
			{
				showverbose((LPCWSTR*)aStrings.GetData(),aStrings.GetSize(),
					(LPCWSTR*)aExtensions.GetData(),aExtensions.GetSize(),
					(LPCWSTR*)aDirectories.GetData(),aDirectories.GetSize());
			}

			locater.LocateFiles(FALSE,(LPCWSTR*)aStrings.GetData(),aStrings.GetSize(),
				(LPCWSTR*)aExtensions.GetData(),aExtensions.GetSize(),
				(LPCWSTR*)aDirectories.GetData(),aDirectories.GetSize());

		}

		
	
	}
	else
	{
		if (options.verbose)
		{
			showverbose(NULL,0,
				(LPCWSTR*)aExtensions.GetData(),aExtensions.GetSize(),
				(LPCWSTR*)aDirectories.GetData(),aDirectories.GetSize());
		}

		locater.LocateFiles(FALSE,NULL,0,
			(LPCWSTR*)aExtensions.GetData(),aExtensions.GetSize(),
			(LPCWSTR*)aDirectories.GetData(),aDirectories.GetSize());
	}


	if (options.verbose)
		wprintf(ID2W(IDS_LOCATEVERBOSEFOUNDFILES),locater.GetNumberOfResults());

	if (pContainData!=NULL)
		delete[] pContainData;

	FinishRegKey();

	FreeLibrary(GetLanguageSpecificResourceHandle());

	if (IsUnicodeSystem())
	{
		BOOL (WINAPI *pSetConsoleOutputCP)(UINT)=
			(BOOL (WINAPI *)(UINT))GetProcAddress(GetModuleHandle("kernel32.dll"),"SetConsoleOutputCP");
		if (pSetConsoleOutputCP!=NULL)
			pSetConsoleOutputCP(g_uiOriginalConsoleCP);
	}
	return 0;
}
