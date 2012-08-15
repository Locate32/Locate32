////////////////////////////////////////////////////////////////////
// HFC Library - Copyright (C) 1999-2010 Janne Huttunen
////////////////////////////////////////////////////////////////////

#include "HFCLib.h"



#ifdef WIN32


HFCERROR VB::InputText32(HANDLE hFile, PUCHAR szBuffer,int len)
{
    DWORD readed;
	TCHAR ch;
    int i=0;
    do
    {
        ReadFile(hFile,&ch,1,&readed,NULL);
		if (!readed)
			return SetHFCError(HFC_CANNOTREAD);
	}
    while(ch!='"');
    ReadFile(hFile,&ch,1,&readed,NULL);
	while (ch!='"' && i<len-1)
    {
        if (!readed)
     		return SetHFCError(HFC_CANNOTREAD);
		szBuffer[i++]=ch;
        ReadFile(hFile,&ch,1,&readed,NULL);
    }
    szBuffer[i]='\0';
    ReadFile(hFile,&ch,1,&readed,NULL);
	while(ch!=',' && ch!='\n')
    {
        if (!readed)
           return HFC_OK;
        ReadFile(hFile,&ch,1,&readed,NULL);
    }
    return HFC_OK;
}

HFCERROR VB::InputNum32(HANDLE hFile, PINT num)
{
    DWORD readed;
	TCHAR ch;
    ReadFile(hFile,&ch,1,&readed,NULL);
    while(ch==' ' || ch=='\n')
    {
        if (!readed)
      		return SetHFCError(HFC_CANNOTREAD);
	    ReadFile(hFile,&ch,1,&readed,NULL);
    }
    if (ch=='#')
    {
        TCHAR szBuffer[6];
        int i=0;
	    ReadFile(hFile,&ch,1,&readed,NULL);
		while (ch!='#' && i<6)
        {
            if (!readed)
         		return SetHFCError(HFC_CANNOTREAD);
			szBuffer[i++]=ch;
            ReadFile(hFile,&ch,1,&readed,NULL);
        }
        while(ch!=',' && ch!='\n')
        {
            if (!readed)
               return HFC_OK;
            ReadFile(hFile,&ch,1,&readed,NULL);
        }
        szBuffer[i]='\0';
        if(strcmp(szBuffer,"TRUE")==0)
        {
            if (num!=NULL)
				*num=TRUE;
            return HFC_OK;
        }
        else if(strcmp(szBuffer,"FALSE")==0)
        {
            if (num!=NULL)
				*num=FALSE;
            return HFC_OK;
        }
        if (num!=NULL)
			*num=0;
		return SetHFCError(HFC_CANNOTREAD);
    }
    else
    {
        TCHAR szBuffer[20];
        int i=0;
        while(ch!=' ' && ch!='\n' && ch!=',' && i<20)
        {
            if (!readed)
      			return SetHFCError(HFC_CANNOTREAD);
			szBuffer[i++]=ch;
            if ((ch<'0' || ch>'9') && ch!='.')
				return SetHFCError(HFC_CANNOTREAD);
            ReadFile(hFile,&ch,1,&readed,NULL);
        }
        szBuffer[i]='\0';
        while(ch!=',' && ch!='\n')
        {
            if (!readed)
               return HFC_OK;
            ReadFile(hFile,&ch,1,&readed,NULL);
        }
        if (num!=NULL)
			 *num=atoi(szBuffer);
        return HFC_OK;
    }
    return HFC_OK;
}

HFCERROR VB::InputFloat32(HANDLE hFile, PDOUBLE num)
{
    DWORD readed;
	TCHAR ch;
	ReadFile(hFile,&ch,1,&readed,NULL);
	while(ch==' ' || ch=='\n')
    {
        if (!readed)
			return SetHFCError(HFC_CANNOTREAD);
        ReadFile(hFile,&ch,1,&readed,NULL);
    }
    if (ch=='#')
    {
        TCHAR szBuffer[6];
        int i=0;
        ReadFile(hFile,&ch,1,&readed,NULL);
        while (ch!='#' && i<6)
        {
            if (!readed)
				return SetHFCError(HFC_CANNOTREAD);
			szBuffer[i++]=ch;
            ReadFile(hFile,&ch,1,&readed,NULL);
        }
        while(ch!=',' && ch!='\n')
        {
            if (!readed)
               return HFC_OK;
            ReadFile(hFile,&ch,1,&readed,NULL);
        }
        szBuffer[i]='\0';
        if(strcmp(szBuffer,"TRUE")==0)
        {
            if (num!=NULL)
				*num=(double)TRUE;
            return HFC_OK;
        }
        else if(strcmp(szBuffer,"FALSE")==0)
        {
            if (num!=NULL)
				*num=(double)FALSE;
            return HFC_OK;
        }
        if (num!=NULL)
			*num=(float)0;
 		return SetHFCError(HFC_CANNOTREAD);
    }
    else
    {
        TCHAR szBuffer[20];
        int i=0;
        while(ch!=' ' && ch!='\n' && ch!=',' && i<20)
        {
            if (!readed)
				return SetHFCError(HFC_CANNOTREAD);
		    szBuffer[i++]=ch;
            if ((ch<'0' || ch>'9') && ch!='.')
				return SetHFCError(HFC_CANNOTREAD);
            ReadFile(hFile,&ch,1,&readed,NULL);
        }
        szBuffer[i]='\0';
        while(ch!=',' && ch!='\n')
        {
            if (!readed)
               return HFC_OK;
            ReadFile(hFile,&ch,1,&readed,NULL);
        }
        if (num!=NULL)
			*num=atof(szBuffer);
        return HFC_OK;
    }
    return HFC_OK;
}

HFCERROR VB::PrintText32(HANDLE hFile, LPCTSTR bfr,TCHAR LF)
{
    DWORD writed;
	WriteFile(hFile,"\"",1,&writed,NULL);
	if (!writed)
		return SetHFCError(HFC_CANNOTWRITE);
	WriteFile(hFile,bfr,(DWORD)strlen(bfr),&writed,NULL);
	if (writed<strlen(bfr))
		return SetHFCError(HFC_CANNOTWRITE);
	if (LF)
        WriteFile(hFile,"\"\n",2,&writed,NULL);
	else
        WriteFile(hFile,"\",",2,&writed,NULL);
	if (writed<2)
		return SetHFCError(HFC_CANNOTWRITE);
	return HFC_OK;
}


HFCERROR VB::PrintNum32(HANDLE hFile, int num,TCHAR LF)
{
    DWORD writed;
	TCHAR szBuffer[14];
	if (LF)
        StringCbPrintf(szBuffer,14,"%d\n",num);
	else
		StringCbPrintf(szBuffer,14,"%d,",num);
	WriteFile(hFile,szBuffer,(DWORD)strlen(szBuffer),&writed,NULL);
	if (writed<strlen(szBuffer))
		return SetHFCError(HFC_CANNOTWRITE);
    return HFC_OK;
}

HFCERROR VB::PrintFloat32(HANDLE hFile, double num,TCHAR LF)
{
    DWORD writed;
	TCHAR szBuffer[40];
	if (LF)
        StringCbPrintf(szBuffer,40,"%f\n",num);
	else
	    StringCbPrintf(szBuffer,40,"%f,",num);
	WriteFile(hFile,szBuffer,(DWORD)strlen(szBuffer),&writed,NULL);
	if (writed<strlen(szBuffer))
		return SetHFCError(HFC_CANNOTWRITE);
    return HFC_OK;
}

HFCERROR VB::PrintBool32(HANDLE hFile,UCHAR boolean,TCHAR LF)
{
	DWORD writed;
	if (boolean)
		WriteFile(hFile,"#TRUE#",6,&writed,NULL);
	else
		WriteFile(hFile,"#FALSE#",7,&writed,NULL);
	if (writed<6)
		return SetHFCError(HFC_CANNOTWRITE);
	if (LF)
		WriteFile(hFile,"\n",1,&writed,NULL);
	else
		WriteFile(hFile,",",1,&writed,NULL);
	if (!writed)
		return SetHFCError(HFC_CANNOTWRITE);
	return HFC_OK;
}

#endif