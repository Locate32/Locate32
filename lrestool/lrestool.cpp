#include <hfclib.h>
#include <conio.h>
#include "lrestool.h"

#define COPYRIGHTTEXT "lrestool v 3.1.8.3090 (C) 2003-2008 Janne Huttunen: language resource tool\n\n"


FILE* _stderr=stderr;


int CreateMode(Data& o)
{
	IDENTIFIERS iLResFile;
	
	// First load identifiers from file
	if (o.bVerbose)
		printf("Loading identifiers from file %s\n",LPCSTR(o.strLResFile));

	if (!iLResFile.LoadFromLResFile(o.strLResFile,o.bDoubleVerbose))
	{
		if (o.bVerbose)
			fprintf(_stderr,"Exiting due error...\n");
		return 1;
	}
	if (o.bVerbose)
		printf("\t%d identifiers found.\n",iLResFile.GetCount());

	if (o.strOutFile.IsEmpty())
	{
		o.strOutFile.Copy(o.strLResFile,o.strLResFile.FindLast('.'));
		o.strOutFile << ".rc";
		if (o.bVerbose)
			printf("Output file is set to '%s'\n",LPCSTR(o.strOutFile));
	}
	
	if (o.bVerbose)
		printf("Merging identifies to file '%s'\n",LPCSTR(o.strOutFile));
	
	if (!iLResFile.InsertToOutputFile(o.strOutFile,o.strBaseFile,o.bDoubleVerbose))
	{
		if (o.bVerbose)
			fprintf(_stderr,"Exiting due error...\n");
		return 1;
	}

	// Clearing memory
	iLResFile.RemoveAll(!o.bVerbose);
	return 0;
}

int CheckMode(Data& o)
{
	IDENTIFIERS iRCFile,iReference;
	
	// Loading identifiers from base RC file
	if (o.bVerbose)
		printf("Retrievind identifiers from base file\n");
	
	if (!iRCFile.LoadFromRCFile(o.strBaseFile,o.bDoubleVerbose))
	{
		if (o.bVerbose)
			fprintf(_stderr,"Exiting due error...\n");
		return 1;
	}

	iRCFile.AddHead()->name="FOR_PROGRAM";
	iRCFile.GetHead().bUsed=TRUE;

	if (o.bVerbose)
		printf("\t%d identifiers found.\n",iRCFile.GetCount());

	// Creating temp file for output if needed
	BOOL bIsTemp=o.strOutFile.IsEmpty();
	if (bIsTemp)
	{
		int nIndex=o.strLResFile.FindLast('\\');
		GetTempFileName(nIndex==-1?".":o.strLResFile.Left(nIndex),
			"lrt",0,o.strOutFile.GetBuffer(MAX_PATH));
		if (o.bVerbose)
			printf("Output file is set temporarily to '%s'\n",LPCSTR(o.strOutFile));
	}

	// Loading reference information
	if (FileSystem::IsFile(o.strReference))
		iReference.LoadFromLResFile(o.strReference,FALSE);
	
	if (!iRCFile.UpdateLResFile(o.strLResFile,o.strOutFile,o.bDoubleVerbose,o.bInteractive,iReference))
	{
		if (o.bVerbose)
			fprintf(_stderr,"Exiting due error...\n");
		return 1;
	}

	// If output was temp file, overwriting original
	if (bIsTemp)
	{
		if (!MoveFileEx(o.strOutFile,o.strLResFile,MOVEFILE_COPY_ALLOWED|MOVEFILE_REPLACE_EXISTING))
		{
			fprintf(_stderr,"Cannot overwrite file '%s', modified is still in file '%s'\n",
				LPCSTR(o.strLResFile),LPCSTR(o.strOutFile));
		}
	}
	return 0;
}

int DifferenceMode(Data& o)
{
	IDENTIFIERS iFirst,iSecond;
	if (!iFirst.LoadFromLResFile(o.strLResFile,FALSE))
	{
		fprintf(_stderr,"error: cannot read from file %s\n",LPCSTR(o.strLResFile));
		return 1;
	}
	if (!iSecond.LoadFromLResFile(o.strReference,FALSE))
	{
		fprintf(_stderr,"error: cannot read from file %s\n",LPCSTR(o.strReference));
		return 1;
	}
	

	iFirst.CheckDifferences(iSecond,TRUE);

	printf("\nIdentifiers found only in file %s\n\n",LPCSTR(o.strLResFile));
	iFirst.PrintUnused(o.bShowLineNumbers);

	printf("\n\nIdentifiers found only in file %s\n\n",LPCSTR(o.strReference));
	iSecond.PrintUnused(o.bShowLineNumbers);

	printf("\n\nChanged identifiers (%s <--> %s)\n\n",LPCSTR(o.strLResFile),LPCSTR(o.strReference));
	iFirst.PrintChanged(&iSecond,o.bShowLineNumbers);

	return 0;
}

int GenerateHelpPage(Data& o)
{
	IDENTIFIERS iSrcFile,iLRFFile;
	
	// First, load indentifiers from language files
	if (o.bVerbose)
		printf("Loading identifiers from file %s\n",LPCSTR(o.strLResFile));

	if (!iLRFFile.LoadFromLResFile(o.strLResFile,FALSE))
	{
		fprintf(_stderr,"error: cannot read from file %s\n",LPCSTR(o.strLResFile));
		return 1;
	}
	
	if (o.bVerbose)
		printf("\t%d identifiers found.\n",iLRFFile.GetCount());

	

	// Now parse source file to temporary file
	CHAR szTempFile[MAX_PATH],szTempDir[MAX_PATH];
	if (!GetTempPath(MAX_PATH,szTempDir))
		strcpy_s(szTempDir,MAX_PATH,".");

	GetTempFileName(szTempDir,"lrs_",0,szTempFile);

	if (o.bVerbose)
		printf("Parsing %s and inserting strings from translation file\n",LPCSTR(o.strReference));

	if (!iLRFFile.InsertToOutputFile(szTempFile,o.strReference,o.bDoubleVerbose,TRUE))
	{
		if (o.bVerbose)
			fprintf(_stderr,"Exiting due error...\n");
		return 1;
	}


	// Read base file from registry
	if (o.strBaseFile.IsEmpty())
	{
		PIDENTIFIER pID=iLRFFile.FindIdentifier("FOR_PROGRAM");
		LPCSTR pProgramName="LOCATE32";
		
		if (pID==NULL)
			printf("Program name is not specified in %s, using \"LOCATE32\"\n",o.strLResFile);
		else
			pProgramName=pID->text;

		GetBaseFileFromRegistry(pProgramName,o.strBaseFile,TRUE);
			
		if (o.strBaseFile.IsEmpty())
		{
			printf("No information about base file, using \"page_raw.htm\"\n",o.strLResFile);
			o.strBaseFile="page_raw.htm";
		}
	}

	iLRFFile.RemoveAll(FALSE);
	
	
	// First load identifiers from file
	if (o.bVerbose)
		printf("Loading the html source file\n");

	if (!iSrcFile.LoadFromHtmlRawFile(szTempFile,o.bDoubleVerbose))
	{
		if (o.bVerbose)
			fprintf(_stderr,"Exiting due error...\n");
		return 1;
	}

	if (o.bVerbose)
		printf("\t%d identifiers found.\n",iSrcFile.GetCount());

	DeleteFile(szTempFile);

	

	// Check output file
	if (o.strOutFile.IsEmpty())
	{
		o.strOutFile.Copy(o.strLResFile,o.strLResFile.FindLast('.'));
		o.strOutFile << ".htm";
		if (o.bVerbose)
			printf("Output file is set to '%s'\n",LPCSTR(o.strOutFile));
	}
	
	if (o.bVerbose)
		printf("Merging identifies to file '%s'\n",LPCSTR(o.strOutFile));
	



	if (!iSrcFile.InsertToOutputFile(o.strOutFile,o.strBaseFile,o.bDoubleVerbose,TRUE))
	{
		if (o.bVerbose)
			fprintf(_stderr,"Exiting due error...\n");
		return 1;
	}

	
	// Clearing memory
	iSrcFile.RemoveAll(!o.bVerbose);
	return 0;
}

int SetRegFile(Data& o,BOOL bRawHtmpPage)
{
	if (!FileSystem::IsFile(o.strBaseFile))
	{
		fprintf(_stderr,"File '%s' does not exist",LPCSTR(o.strBaseFile));
		return 1;
	}
	
	char szPath[MAX_PATH];
	LPSTR szTemp;
	GetFullPathName(o.strBaseFile,MAX_PATH,szPath,&szTemp);
	
    if (SetBaseFileToRegistry(o.strLResFile,szPath,bRawHtmpPage))
		printf("name '%s' is set to correspond with file '%s'\n",LPCSTR(o.strLResFile),szPath);
	else
		fprintf(_stderr,"Error: database cannot be setted");
	return 0;
}

int ShowRegFile(Data& o,BOOL bRawHtmpPage)
{
	CString sFile;
	GetBaseFileFromRegistry(o.strLResFile,sFile,bRawHtmpPage);
	printf("name '%s' corresponds with file '%s'\n",LPCSTR(o.strLResFile),LPCSTR(sFile));
	return 0;
}

int DelRegFile(Data& o,BOOL bRawHtmpPage)
{
	DeleteBaseFileFromRegistry(o.strLResFile,bRawHtmpPage);
	return 0;
}


int main(int argc,char* argv[])
{
#ifdef _DEBUG
	CAppData::stdfunc();
#endif
	
	Data o;
	ZeroMemory(&o,sizeof(Data));
	o.nMode=Data::CreateRCFile;

	SetHFCErrorCallback(StdHFCErrorCallbackStderr,DWORD("Error n/o %X occured: %s\n"));

	// Parsing parameters
	for (int i=1;i<argc && !o.bShowHelp;i++)
	{
		if (argv[i][0]=='-')
		{
			switch(argv[i][1])
			{
			case 'c':
				if (o.nMode!=Data::CreateRCFile)
					o.bShowHelp=TRUE;
				else 
				{
                    o.nMode=Data::CheckLResFile;
					if (argv[i][2]=='i')
						o.bInteractive=TRUE;
				}
				break;
			case 'd':
				if (o.nMode!=Data::CreateRCFile)
					o.bShowHelp=TRUE;
				else 
				{
					o.nMode=Data::Difference;
					if (argv[i][2]=='l')
						o.bShowLineNumbers=TRUE;
				}
				break;
			case 'h':
				if (o.nMode!=Data::CreateRCFile)
					o.bShowHelp=TRUE;
				else 
                    o.nMode=Data::GenHelpPage;
				break;
			case 'R':
				if (o.nMode!=Data::CreateRCFile)
					o.bShowHelp=TRUE;
				else
				{
					switch (argv[i][2])
					{
					case 'a':
					case 'A':
						o.nMode=Data::SetBaseName;
						break;
					case 's':
					case 'S':
						o.nMode=Data::ShowBaseName;
						break;
					case 'd':
					case 'D':
						o.nMode=Data::DelBaseName;
						break;
					default:
						o.bShowHelp=TRUE;
						break;
					}
				}
				break;
			case 'H':
				if (o.nMode!=Data::CreateRCFile)
					o.bShowHelp=TRUE;
				else
				{
					switch (argv[i][2])
					{
					case 'a':
					case 'A':
						o.nMode=Data::SetRawHelpFile;
						break;
					case 's':
					case 'S':
						o.nMode=Data::ShowRawHelpFile;
						break;
					case 'd':
					case 'D':
						o.nMode=Data::DelRawHelpFile;
						break;
					default:
						o.bShowHelp=TRUE;
						break;
					}
				}
				break;
			case 'o':
				if (o.nMode!=Data::CreateRCFile && o.nMode!=Data::CheckLResFile && o.nMode!=Data::GenHelpPage)
					o.bShowHelp=TRUE;
				else if (!SetArgString(i,o.strOutFile,argc,argv))
					o.bShowHelp=TRUE;
				break;
			case 'b':
				if (o.nMode!=Data::CreateRCFile && o.nMode!=Data::CheckLResFile && o.nMode!=Data::GenHelpPage)
					o.bShowHelp=TRUE;
				else if (!SetArgString(i,o.strBaseFile,argc,argv))
					o.bShowHelp=TRUE;
				break;
			case 'i':
				if (o.nMode==Data::CheckLResFile)
					o.bInteractive=TRUE;
				else
					o.bShowHelp=TRUE;
				break;
			case 'l':
				if (o.nMode==Data::Difference)
					o.bShowLineNumbers=TRUE;
				else
					o.bShowHelp=TRUE;
				break;
			case 'r':
				if (o.nMode==Data::CheckLResFile)
				{
					if (!SetArgString(i,o.strReference,argc,argv))
						o.bShowHelp=TRUE;
				}
				else
					o.bShowHelp=TRUE;
				break;
			case 'v':
				if (argv[i][2]=='v')
				{
					o.bDoubleVerbose=TRUE;
					o.bVerbose=TRUE;
				}
				else if (o.bVerbose)
					o.bDoubleVerbose=TRUE;
				else
					o.bVerbose=TRUE;
				break;
			case 'e':
				_stderr=stderr;
				break;
			default:
				o.bShowHelp=TRUE;
				break;
			}
		}
		else if (o.nMode==Data::SetBaseName || o.nMode==Data::SetRawHelpFile)
		{
			if (o.strLResFile.IsEmpty())
				o.strLResFile=argv[i];
			else if (o.strBaseFile.IsEmpty())
				o.strBaseFile=argv[i];
			else
				o.bShowHelp=TRUE;
		}
		else if (o.nMode==Data::Difference)
		{
			if (o.strLResFile.IsEmpty())
				o.strLResFile=argv[i];
			else if (o.strReference.IsEmpty())
				o.strReference=argv[i];
			else
				o.bShowHelp=TRUE;
		}
		else if (o.nMode==Data::GenHelpPage)
		{
			if (o.strLResFile.IsEmpty())
				o.strLResFile=argv[i];
			else if (o.strReference.IsEmpty())
				o.strReference=argv[i];
			else
				o.bShowHelp=TRUE;
		}
		else if (o.strLResFile.IsEmpty())
			o.strLResFile=argv[i];
		else
			o.bShowHelp=TRUE;
	}
	if (o.bShowHelp || o.strLResFile.IsEmpty())
	{
		printf(COPYRIGHTTEXT);
		printf("usage: lrestool [-o outfile] [-b basefile] lrffile\n");
		printf("or:    lrestool -c [-i] [-r lrffile] [-o outfile] [-b basefile] lrffile\n");
		printf("or:    lrestool -d 1stfile 2ndfile \n");
		printf("or:    lrestool -h  -o helppage.htm language.lrf helppage.src \n");
		printf("or:    lrestool -R[a,s,d] name [basefile]\n");
		printf("or:    lrestool -H[a,s,d] name [basefile]\n\n");
		printf("\t-o outfile:\tset output file to 'outfile'\n");
		printf("\t-c:\t\tchecks infile, adds missing identifiers\n\t\t\tand marks obsolete identifiers\n");
		printf("\t-i:\t\tmakes checking interactively (use with -c only)\n");
		printf("\t-r file:\tuses lrf file as reference\n");
		printf("\t-d:\t\tchecks difference between to two lanfuage files\n");
		printf("\t-l:\t\tshow line numbers (use with -d only)\n");
		printf("\t-h:\t\tconvert help page source to HTML help page\n");
		printf("\t-b basefile:\tset basefile file to 'basefile'\n");
		printf("\t-e:\t\tuse stdout instead of stderr for error messages\n");
		printf("\t-Ra:\t\tsets program name corresponds to file \n\t\t\t'basefile' (name corresponds with FOR_PROGRAM\n\t\t\tidentifier in lrf file)\n");
		printf("\t-Rs:\t\tshows basefile which corresponds to program name\n");
		printf("\t-Rd:\t\tunsets basefile which corresponds to program name\n");
		printf("\t-Ha:\t\tsets program name corresponds to raw help file \n\t\t\t'basefile')\n");
		printf("\t-Hs:\t\tshows the raw help file corresponds to program name\n");
		printf("\t-Hd:\t\tunsets the raw help file which corresponds to \n\t\t\tprogram name\n");
		printf("\t-v:\t\tverbose\n");

		return 1;
	}

	if ((o.nMode==Data::CreateRCFile || o.nMode==Data::CheckLResFile) &&
		o.strBaseFile.IsEmpty())
	{
		CString Name;

		if (CIdentifiers::FindProgramNameFromLResFile(o.strLResFile,Name))
			GetBaseFileFromRegistry(Name,o.strBaseFile,FALSE);
			
		if (o.strBaseFile.IsEmpty())
			o.strBaseFile="lres_base.rc";
	}
	

	if (o.bDoubleVerbose)
	{
        switch (o.nMode)
		{
		case Data::CreateRCFile:
			printf("create rc file mode with parameters:\n");
			printf("\tinput file: %s\n",LPCSTR(o.strLResFile));
			printf("\toutput file: %s\n",LPCSTR(o.strOutFile));
			printf("\tbase file: %s\n",LPCSTR(o.strBaseFile));
			break;
		case Data::CheckLResFile:
			printf("check mode with parameters:\n");
			printf("\tinput file: %s\n",LPCSTR(o.strLResFile));
			printf("\toutput file: %s\n",LPCSTR(o.strOutFile));
			printf("\tbase file: %s\n",LPCSTR(o.strBaseFile));
			printf("\treference file: %s\n",LPCSTR(o.strReference));
			printf("\tinteractive: %s\n",o.bInteractive?"yes":"no");
			break;
		case Data::Difference:
			printf("difference mode with parameters:\n");
			printf("\tnew file: %s\n",LPCSTR(o.strLResFile));
			printf("\told file: %s\n",LPCSTR(o.strReference));
			break;
		case Data::GenHelpPage:
			printf("generate help file:\n");
			printf("\tlanguage file: %s\n",LPCSTR(o.strLResFile));
			printf("\tinput file: %s\n",LPCSTR(o.strReference));
			printf("\toutput file: %s\n",LPCSTR(o.strOutFile));
			printf("\tbase file: %s\n",LPCSTR(o.strBaseFile));
			break;
		case Data::SetBaseName:
			printf("set base name mode with parameters:\n");
			printf("\tname: %s\n",LPCSTR(o.strLResFile));
			printf("\tfile: %s\n",LPCSTR(o.strBaseFile));
			break;
		case Data::ShowBaseName:
			printf("show base name mode with parameters:\n");
			printf("\tname: %s\n",LPCSTR(o.strLResFile));
			break;
		case Data::DelBaseName:
			printf("delete base name mode with parameters:\n");
			printf("\tname: %s\n",LPCSTR(o.strLResFile));
			break;
		case Data::SetRawHelpFile:
			printf("set raw HTMP page mode with parameters:\n");
			printf("\tname: %s\n",LPCSTR(o.strLResFile));
			printf("\tfile: %s\n",LPCSTR(o.strBaseFile));
			break;
		case Data::ShowRawHelpFile:
			printf("show raw HTMP page mode with parameters:\n");
			printf("\tname: %s\n",LPCSTR(o.strLResFile));
			break;
		case Data::DelRawHelpFile:
			printf("delete raw HTMP page mode with parameters:\n");
			printf("\tname: %s\n",LPCSTR(o.strLResFile));
			break;
		}
	}

	switch (o.nMode)
	{
	case Data::CreateRCFile:
		return CreateMode(o);
	case Data::CheckLResFile:
		return CheckMode(o);
	case Data::Difference:
		return DifferenceMode(o);
	case Data::GenHelpPage:
		return GenerateHelpPage(o);
	case Data::SetBaseName:
		return SetRegFile(o,FALSE);
	case Data::ShowBaseName:
		return ShowRegFile(o,FALSE);
	case Data::DelBaseName:
		return DelRegFile(o,FALSE);
	case Data::SetRawHelpFile:
		return SetRegFile(o,TRUE);
	case Data::ShowRawHelpFile:
		return ShowRegFile(o,TRUE);
	case Data::DelRawHelpFile:
		return DelRegFile(o,TRUE);
	} 
	return 0;
}