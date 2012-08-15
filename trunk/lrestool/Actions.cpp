#include <hfclib.h>
#include <conio.h>
#include "lrestool.h"


BOOL CIdentifiers::LoadFromRCFile(LPCSTR szFile,BOOL bShowFound)
{
	char* pBase=NULL;
	DWORD dwLength=0;

	if (!ReadFileContent(szFile,pBase,dwLength))
		return FALSE;

	LPCSTR pPointer=pBase;
	
	for (;;)
	{
		// Find next $
		if (!FindNextIdentifierInRCFile(pPointer))
		{
			fprintf(_stderr,"Error in base file.\n");
			delete[] pBase;
			return FALSE;
		}
		
		// Writing 
		if (*pPointer=='\0')
			break;
		pPointer++;
		
		int nIndex=0;
		for (;pPointer[nIndex]!='$' && pPointer[nIndex]!='@';nIndex++)
		{
			if (!ISVALIDCHAR(pPointer[nIndex]))
			{
				fprintf(_stderr,"Error in base file.\n");
				delete[] pBase;
				return FALSE;
			}
		}

		CString name(pPointer,nIndex);
		

		// Ignore formattings
		if (pPointer[nIndex]=='@')
		{
			for (nIndex++;pPointer[nIndex]!='$';nIndex++);
		}

        pPointer+=nIndex+1;
		
		// Finding identified and merging
		PIDENTIFIER pNew=FindIdentifier(name);
		
		if (pNew==NULL)
		{
			pNew=AddTail();
			pNew->name.Swap(name);

			if (bShowFound)
				printf("Found ID: %s\n",LPCSTR(pNew->name));
		}
	}	
	delete[] pBase;
	return TRUE;
}

BOOL CIdentifiers::LoadFromLResFile(LPCSTR szFile,BOOL bShowFound)
{
	char* pBuffer=NULL;
	DWORD dwLength=0;
	
	if (!ReadFileContent(szFile,pBuffer,dwLength))
		return FALSE;

	LPCSTR pPointer=pBuffer;
	DWORD dwReadedLines=1;
	BOOL bIsThereCommentsBefore=FALSE;
	CString name,text;
		
	IDENTIFIER* pNew=NULL;

	for(;;)
	{
		if (!FindNextIdentifierInLresFile(pPointer,dwReadedLines,bIsThereCommentsBefore))
			break;
		if (pNew!=NULL)
			pNew->bCommentsBelow=bIsThereCommentsBefore;
		
		if (!SeperateIdentifierAndText(pPointer,name,text))
		{
			fprintf(_stderr,"Error in line %d in file %s.\n",dwReadedLines,szFile);
			delete[] pBuffer;
			return FALSE;				
		}

		pNew=FindIdentifier(name);
		
		if (pNew==NULL)
		{			
			pNew=AddTail();
			pNew->name.Swap(name);
			pNew->text.Swap(text);
			pNew->bCommentsAbove=bIsThereCommentsBefore;

			
			if (pNew->name.CompareNoCase("FOR_PROGRAM")==0)
			{
				pNew->bUsed=TRUE;
				if (bShowFound)
					printf("Language file is for program: %s",LPCSTR(text));
			}			
			else if (bShowFound)
				printf("Found ID: %s=%s\n",LPCSTR(pNew->name),LPCSTR(pNew->text));
		}
		else
		{
			printf("Dublicated identifier %s, replacing text with %s\n",LPCSTR(name),LPCSTR(text));
			pNew->text.Swap(text);
			pNew->bCommentsAbove=bIsThereCommentsBefore;
		}
		pNew->dwLineNumber=dwReadedLines;
	}
	
	delete[] pBuffer;
	return TRUE;
}

BOOL CIdentifiers::LoadFromHtmlRawFile(LPCSTR szFile,BOOL bShowFound)
{
	char* pBuffer=NULL;
	DWORD dwLength=0;
	
	if (!ReadFileContent(szFile,pBuffer,dwLength))
		return FALSE;

	LPCSTR pPointer=pBuffer;
	DWORD dwReadedLines=1;
	BOOL bIsThereCommentsBefore=FALSE;
	CString name,text;
		
	IDENTIFIER* pNew=NULL;

	for(;;)
	{
		if (!FindNextIdentifierInLresFile(pPointer,dwReadedLines,bIsThereCommentsBefore))
			break;
		
		if (!SeperateIdentifierAndText(pPointer,name,text))
		{
			fprintf(_stderr,"Error in line %d in file %s.\n",dwReadedLines,szFile);
			delete[] pBuffer;
			return FALSE;				
		}

		pNew=FindIdentifier(name);
		
		if (pNew==NULL)
		{			
			pNew=AddTail();
			pNew->name.Swap(name);
			
			if (text.IsEmpty())
			{
				// The text is the rest of file
				pNew->text=pPointer;
				if (bShowFound)
					printf("Found ID for the rest of file: %s\n",LPCSTR(pNew->name));
				break;
			}
			else
			{
				pNew->text.Swap(text);
				if (bShowFound)
					printf("Found ID: %s=%s\n",LPCSTR(pNew->name),LPCSTR(pNew->text));
			}
		}
		else
		{
			printf("Dublicated identifier %s, replacing text with %s\n",LPCSTR(name),LPCSTR(text));
			pNew->text.Swap(text);
			pNew->bCommentsAbove=bIsThereCommentsBefore;
		}
		pNew->dwLineNumber=dwReadedLines;
	}
	
	delete[] pBuffer;
	return TRUE;
}

BOOL CIdentifiers::InsertToOutputFile(LPCSTR szNewFile,LPCSTR szBaseFile,BOOL bShowMerging,BOOL bAllowNonExisting)
{
	char* pBase=NULL;
	DWORD dwLength=0;
	if (!ReadFileContent(szBaseFile,pBase,dwLength))
		return FALSE;

	try {
		CFile file;
		file.SetToThrow();

		// Creating output file
		file.Open(szNewFile,CFile::defWrite|CFile::typeText);
		
		LPCSTR pPointer=pBase;
		
        for (;;)
		{
			LPCSTR pWrited=pPointer;
		
			// Find next $
			if (!FindNextIdentifierInRCFile(pPointer))
			{
				fprintf(_stderr,"Error in base file.\n");
				delete[] pBase;
				return FALSE;
			}
			
			// Writing 
			file.Write(pWrited,pPointer-pWrited);
			pWrited=pPointer;
			if (*pPointer=='\0')
				break;

			// Skip first '$'
			pPointer++;

			// Count length
			int nIndex=0;
			for (;pPointer[nIndex]!='$' && pPointer[nIndex]!='@';nIndex++)
			{
				if (!ISVALIDCHAR(pPointer[nIndex]))
				{
					fprintf(_stderr,"Error in base file.\n");
					delete[] pBase;
					return FALSE;
				}
			}

            CString name(pPointer,nIndex);
			pPointer+=nIndex;
			
			enum Flags {
				fMemoveDoubleAndChar = 0x1, // 'A'
				fDoubpleApostrophes = 0x2, // 'P'
				fAddColonToEnd = 0x4, // 'c'
				fRemoveColonFromEnd = 0x8,  // 'C'
				fRemoveMnemonics = 0x10, // 'M'
				fRemoveTextInParenthesis = 0x20 // 'p'
			};
			DWORD dwFlags=0;

			if (*pPointer=='@')
			{
				// Parsing needed
				pPointer++;

				for (;*pPointer!='$';pPointer++)
				{
					switch (*pPointer)
					{
					case 'A':
						dwFlags|=fMemoveDoubleAndChar;
						break;
					case 'P':
						dwFlags|=fDoubpleApostrophes;
						break;
					case 'c':
						dwFlags|=fAddColonToEnd;
						break;
					case 'C':
						dwFlags|=fRemoveColonFromEnd;
						break;
					case 'M':
						dwFlags|=fRemoveMnemonics;
						break;
					case 'p':
						dwFlags|=fRemoveTextInParenthesis;
						break;
					};
				}
			}
			

			pPointer++;
			
			// Finding identified and merging
			IDENTIFIER* pID=FindIdentifier(name);
			if (pID==NULL)
			{
				if (!bAllowNonExisting)
				{
					printf("Identified %s not found.\n",LPCSTR(name));
					file.Write('$');
					file.Write(name);
					file.Write('$');
				}
			}
			else
			{
				if (dwFlags==0)
					file.Write(pID->text);
				else
				{
					CString text(pID->text);
					if (dwFlags&fDoubpleApostrophes)
					{
						// Add another apostrophis
						for (int i=0;i<text.GetLength();i++)
						{
							if (text[i]=='\"')
								text.InsChar(++i,'\"');
						}
					}

					if (dwFlags&fAddColonToEnd && text.LastChar()!=':')
						text.Append(':');
				
					if (dwFlags&fRemoveColonFromEnd && text.LastChar()==':')
						text.DelLastChar();
	
					if (dwFlags&fRemoveMnemonics)
					{
						for (int i=0;i<text.GetLength();i++)
						{
							if (text[i]=='&')
								text.DelChar(i--);
						}
					}

					if (dwFlags&fMemoveDoubleAndChar)
					{
						for (int i=0;i<text.GetLength();i++)
						{
							if (text[i]=='&' && text[i+1]=='&')
								text.DelChar(i);
						}
					}

					if (dwFlags&fRemoveTextInParenthesis)
					{
						CString origText;
						origText.Swap(text);
						for (int i=0;i<origText.GetLength();i++)
						{
							if (origText[i]=='(')
							{
								if (text.LastChar()==' ')
									text.DelLastChar();

								while (origText[i]!=')')
									i++;
							}
							else
								text << origText[i];
						}
					}

					file.Write(text);
				}
				pID->bUsed=TRUE;
			}

		}		
		file.Close();
		delete[] pBase;
	}
	catch(...)
	{
		if (pBase!=NULL)
			delete[] pBase;
		return FALSE;
	}
	return TRUE;
}

BOOL CIdentifiers::UpdateLResFile(LPCSTR szInputFile,LPCSTR szOutputFile,BYTE bShowFound,BYTE bInteractive,const CIdentifiers& iReference)
{
	CIdentifiers iInputFile;
	if (!iInputFile.LoadFromLResFile(szInputFile,FALSE))
	{
		fprintf(_stderr,"Error: cannot load identifiers from file %s\n",szInputFile);
		return FALSE;
	}

	// Checks new stuff
	CheckDifferences(iInputFile,FALSE);

	char* pFileContent=NULL;
	DWORD dwLength=0;
	if (!ReadFileContent(szInputFile,pFileContent,dwLength))
		return FALSE;
	
	// Checking obsolete ID
	POSITION pPos=iInputFile.GetHeadPosition();
	while (pPos!=NULL)
	{
		PIDENTIFIER pID=GetAtPtr(pPos);
		if (!pID->bUsed)
		{
			// Obsolete identifier
			enum Action{
				DoNothing,
				Delete,
				Comment
			} nAction=Comment;

            if (bInteractive)
			{
				// Ask what to do
				char ret;
				do {
					printf("Found obsolete ID '%s'. Comment/Delete/doNothing [C]?",LPCSTR(pID->name));
					ret=_getch();
					if (ret!='\r' && ret!='\n')
						printf("%c\n",ret);
					else
					{
						ret='c';
						printf("C\n");
					}
				}
				while (ret!='c' && ret!='C' && ret!='n' && ret!='N' && ret!='d' && ret!='D');
			
				if (ret=='c' || ret=='C')
					nAction=Comment;
				else if (ret=='n' || ret=='N')
					nAction=DoNothing;
				else
					nAction=Delete;
			}

			if (nAction!=DoNothing)
				DeleteIdentifier(pFileContent,dwLength,pID->name,nAction==Comment?TRUE:FALSE);
		}		
		pPos=iInputFile.GetNextPosition(pPos);
	}

	// Checking new IDs	
	pPos=GetHeadPosition();
	
	BOOL bCommentAddedToEndOfFile=FALSE;
	POSITION pRefPos,pAfterPos,pBeforePos;
	PIDENTIFIER pReference,pAfter,pBefore;
	PIDENTIFIER pAfterInFile,pBeforeInFile,pNew;
				

	while (pPos!=NULL)
	{
		PIDENTIFIER pID=GetAtPtr(pPos);
		if (!pID->bUsed)
		{
			// Obsolete identifier
			pRefPos=iReference.FindIdentifierPos(pID->name);
			if (pRefPos!=NULL)
			{
				pReference=iReference.GetAtPtr(pRefPos);
				
				pBeforePos=iReference.GetNextPosition(pRefPos);
				pAfterPos=iReference.GetPrevPosition(pRefPos);
				
				if (pBeforePos!=NULL)
				{
                    pBefore=iReference.GetAtPtr(pBeforePos);
					pBeforeInFile=iInputFile.FindIdentifier(pBefore->name);
				}
				else
					pBefore=pBeforeInFile=NULL;

				if (pAfterPos!=NULL)
				{
					pAfter=iReference.GetAtPtr(pAfterPos);
					pAfterInFile=iInputFile.FindIdentifier(pAfter->name);
				}
			}
			else
			{
				pReference=NULL;
				pAfter=NULL;
				pBefore=NULL;
				pAfterInFile=NULL;
				pBeforeInFile=NULL;
			}
			
				
			enum AddAs{
				DoNothing,
				Plain,
				WithText,
				Reference
			} nAction;
			
            if (bInteractive)
			{
				// Ask what to do
				char ret;
				if (pRefPos!=NULL)
				{
					do {
						printf("New ID %s found, ref='%s'\n",LPCSTR(pID->name),LPCSTR(pReference->text));
						printf("Merging it (Yes,No,withText,Ref) [R]?");
							
						ret=_getch();
						if (ret!='\r' && ret!='\n')
							printf("%c\n",ret);
						else
						{
							ret='r';
							printf("R\n");
						}
					}
					while (ret!='y' && ret!='Y' && ret!='n' && ret!='N' && ret!='T' && ret!='t'  && ret!='R' && ret!='r');
				}
				else
				{
					do {
						printf("Merging new identifier '%s' (Yes,No,withText) [Y]?",LPCSTR(pID->name));
							
						ret=_getch();
						if (ret!='\r' && ret!='\n')
							printf("%c\n",ret);
						else
						{
							ret='y';
							printf("Y\n");
						}
					}
					while (ret!='y' && ret!='Y' && ret!='n' && ret!='N' && ret!='T' && ret!='t');
				}
			
				if (ret=='y' || ret=='y')
					nAction=Plain;
				else if (ret=='r' || ret=='R')
					nAction=Reference;
				else if (ret=='t' || ret=='R')
					nAction=WithText;
				else
					nAction=DoNothing;
			}
			else
				nAction=pReference!=NULL?Reference:Plain;

			
			if (nAction!=DoNothing)
			{
				CString text;
				if (nAction==Reference)
					text=pReference->text;
				else if (nAction==WithText)
				{
					do 
					printf("  enter text:");
					while (gets_s(text.GetBuffer(2000),2000)!=NULL);
					text.FreeExtra();		
				}		

				enum Position{
					After,
					Before,
					EndOfFile
				} nPosition;

			
				if (pRefPos!=NULL)
				{
					// Trying to determine the best bosition
					if (pAfterInFile!=NULL && !pAfter->bCommentsBelow)
						nPosition=After;
					else if (pBeforeInFile!=NULL && !pBefore->bCommentsAbove)
						nPosition=Before;
					else if (pAfterInFile!=NULL)
						nPosition=After;
					else if (pBeforeInFile!=NULL)
						nPosition=Before;
					else
						nPosition=EndOfFile;
				}
				else
					nPosition=EndOfFile;

				if (bInteractive && nPosition!=EndOfFile && 
					(pAfterInFile!=NULL || pBeforeInFile!=NULL))
				{
					// Ask the position
					char ret;
					do {
						CString str("Insert: ");
						char def;

						if (pAfterInFile!=NULL)
							str<<"After:\'" << pAfter->name << "\'/";
						if (pBeforeInFile!=NULL)
							str<<"Before:\'" << pBefore->name << "\'/";
						if (nPosition==After)
						    def='A';
						else
							def='B';

						str<<"Eof [" << def << "]?";

						printf(str);
						ret=_getch();
						if (ret!='\r' && ret!='\n')
							printf("%c\n",ret);
						else
						{
							ret=def;
							printf("%c\n",def);
						}
					}
					while (!((ret=='a' || ret=='A') && pAfter!=NULL)&&
							!((ret=='b' || ret=='B') && pBefore!=NULL) &&
							ret!='e' && ret!='E');
					
					if (ret=='a' || ret=='A')
						nPosition=After;
					else if (ret=='b' || ret=='B')
						nPosition=Before;
					else
						nPosition=EndOfFile;
				}
				
				
				
				switch (nPosition)
				{
				case After:
					InsertAfterIdentifier(pFileContent,dwLength,pID->name,text,pAfter->name,TRUE);
					pNew=iInputFile.InsertAfter(pAfter->name);
					pNew->name=pID->name;
					pNew->text=text;
					pNew->bUsed=TRUE;
					break;
				case Before:
					InsertBeforeIdentifier(pFileContent,dwLength,pID->name,text,pBefore->name,TRUE);
					pNew=iInputFile.InsertBefore(pBefore->name);
					pNew->name=pID->name;
					pNew->text=text;
					pNew->bUsed=TRUE;
					break;
				case EndOfFile:
					InsertToEndOfFile(pFileContent,dwLength,pID->name,text,!bCommentAddedToEndOfFile);
					pNew=AddTail();
					pNew->name=pID->name;
					pNew->text=text;
					pNew->bUsed=TRUE;
					bCommentAddedToEndOfFile=TRUE;
					break;
				}
			}
		}		
		pPos=GetNextPosition(pPos);
	}

	return WriteFileContent(szOutputFile,pFileContent,dwLength);
}






