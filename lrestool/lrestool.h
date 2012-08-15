#ifndef LRESTOOL_H
#define LRESTOOL_H

extern FILE* _stderr;


struct CIdentifier {
	CIdentifier();
	CIdentifier(LPCSTR szName);

	CString name;
	CString text;
	BOOL bUsed:1;
	BOOL bChanged:1;
	BOOL bCommentsBelow:1;
	BOOL bCommentsAbove:1;

	DWORD dwLineNumber;
	
};

typedef CIdentifier IDENTIFIER,*PIDENTIFIER;
typedef const CIdentifier * PCIDENTIFIER;
 
inline CIdentifier::CIdentifier()
:	bUsed(FALSE),bCommentsAbove(FALSE),bCommentsBelow(FALSE),bChanged(FALSE),
	dwLineNumber(DWORD(-1))
{
}

inline CIdentifier::CIdentifier(LPCSTR szName)
:	name(szName),bUsed(TRUE),bCommentsAbove(FALSE),bCommentsBelow(FALSE),bChanged(FALSE),
	dwLineNumber(DWORD(-1))
{
}

struct Data{
	CString strLResFile; // In difference mode this is new file, in register mode, this is name
	CString strReference; // In difference mode this is old file, in help mode, this is src file
	CString strBaseFile;
	CString strOutFile;	
	
	enum Mode {
		CreateRCFile,
        CheckLResFile,
		Difference,
		GenHelpPage,
		SetBaseName,
		ShowBaseName,
		DelBaseName,
		SetRawHelpFile,
		ShowRawHelpFile,
		DelRawHelpFile
	} nMode;
	BYTE bInteractive:1;
	BYTE bShowHelp:1;
	BYTE bVerbose:1;
	BYTE bDoubleVerbose:1;
	BYTE bShowLineNumbers:1;
};
	
class CIdentifiers : public CList <IDENTIFIER>
{
public:
	PIDENTIFIER AddHead();
	PIDENTIFIER AddTail();

	PIDENTIFIER InsertAfter(LPCSTR szIdentifier);
	PIDENTIFIER InsertBefore(LPCSTR szIdentifier);

	BOOL LoadFromRCFile(LPCSTR szFile,BOOL bShowFound);
	BOOL LoadFromLResFile(LPCSTR szFile,BOOL bShowFound);
	BOOL LoadFromHtmlRawFile(LPCSTR szFile,BOOL bShowFound);
	
	void RemoveAll(BOOL bShowUnused);

	PIDENTIFIER FindIdentifier(LPCSTR szName);	
	PCIDENTIFIER FindIdentifier(LPCSTR szName) const;	
	POSITION FindIdentifierPos(LPCSTR szName) const;	
	
	
	BOOL InsertToOutputFile(LPCSTR szNewFile,LPCSTR szBaseFile,BOOL bShowMerging,BOOL bAllowNonExisting=FALSE);
	BOOL UpdateLResFile(LPCSTR szInputFile,LPCSTR szOutputFile,BYTE bShowFound,BYTE bInteractive,const CIdentifiers& iReference);
	void CheckDifferences(CIdentifiers& rAnother,BOOL bCheckChanges);

	void PrintUnused(BOOL bLineNumbers=FALSE);
	void PrintChanged(CIdentifiers* pAnother=NULL,BOOL bLineNumbers=FALSE);


public:
	static BOOL ReadFileContent(LPCSTR szFile,char*& pData,DWORD& dwLength);
	static BOOL WriteFileContent(LPCSTR szFile,char* pData,DWORD dwLength);

	static BOOL DeleteIdentifier(LPSTR& pData,DWORD& dwLength,LPCSTR szId,BOOL bCommentOnly);
	static BOOL InsertAfterIdentifier(LPSTR& pData,DWORD& dwLength,LPCSTR szId,LPCSTR szText,LPCSTR szAfterID,BOOL bComment);
	static BOOL InsertBeforeIdentifier(LPSTR& pData,DWORD& dwLength,LPCSTR szId,LPCSTR szText,LPCSTR szBeforeID,BOOL bComment);
	static BOOL InsertToEndOfFile(LPSTR& pData,DWORD& dwLength,LPCSTR szId,LPCSTR szText,BOOL bComment);
	static BOOL InsertTextToIndex(LPSTR& pData,DWORD& dwLength,DWORD nIndex,LPCSTR szText,DWORD dwTextLength);
	static BOOL RemoveTextFromIndex(LPSTR& pData,DWORD& dwLength,DWORD nIndex,DWORD dwTextLength);

public:
	static BOOL FindProgramNameFromLResFile(LPCSTR szFile,CString& sName);
	static BOOL FindNextIdentifierInLresFile(LPCSTR& pPointer,DWORD& dwReadedLines,BOOL& bIsThereComments);
	static BOOL FindNextIdentifierInRCFile(LPCSTR& pPointer);
	static BOOL FindIdentifierInLresFile(LPCSTR& pPointer,LPCSTR szIdName,int iIdNameLength=-1);
	static BOOL SeperateIdentifierAndText(LPCSTR& pPointer,LPCSTR& szName,DWORD& dwNameLength);
	static BOOL SeperateIdentifierAndText(LPCSTR& pPointer,CString& name,CString& text);

};
typedef CIdentifiers IDENTIFIERS,*PIDENTIFIERS;

#define ISVALIDCHAR(c) ((c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9') || c=='_')



BOOL SetArgString(int& i,CString& str,int argc,char* argv[]);

BOOL SetBaseFileToRegistry(LPCSTR szName,LPCSTR szFile,BOOL bRawHelpPage);
BOOL GetBaseFileFromRegistry(LPCSTR szName,CString& sFile,BOOL bRawHelpPage);
BOOL DeleteBaseFileFromRegistry(LPCSTR szName,BOOL bRawHelpPage);

#endif