/* Locate32 - Copyright (c) 1997-2010 Janne Huttunen */

#include <HFCLib.h>
#include "Locate32.h"

/////////////////////////////////////////////
// CSchedule

CSchedule::CSchedule()
:	m_bFlags(flagEnabled|flagAtThisTime),m_nType(typeDaily),m_pDatabases(NULL),
	m_nThreadPriority(THREAD_PRIORITY_NORMAL),m_wCpuUsageTheshold(WORD(-1))
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	m_tStartTime=st;
	m_tLastStartDate=st;
	m_tLastStartTime=st;
	m_tDaily.wEvery=1;


	CRegKey2 RegKey;
	if (RegKey.OpenKey(HKCU,"\\General",CRegKey::defRead)==ERROR_SUCCESS)
	{
		DWORD dwTemp;
		if (RegKey.QueryValue("Update Process Priority",dwTemp))
			m_nThreadPriority=(int)dwTemp;
	}
}

CSchedule::CSchedule(BYTE*& pData,BYTE nVersion)
{
	sMemCopy(this,pData,SCHEDULE_V1_LEN);
	pData+=SCHEDULE_V1_LEN;	

	if (nVersion!=4)
		m_wCpuUsageTheshold=WORD(-1);

	if (nVersion==1)
	{
		m_pDatabases=NULL;
		m_nThreadPriority=THREAD_PRIORITY_NORMAL;
		
	}
	else if (nVersion>=2 && nVersion<=4)
	{
		BOOL bDatabasesSpecified=*((DWORD*)pData)!=NULL;
		pData+=sizeof(DWORD);

		if (nVersion>=3)
		{
			m_nThreadPriority=*((DWORD*)pData);
			pData+=sizeof(DWORD);
		}
		else
			m_nThreadPriority=THREAD_PRIORITY_NORMAL;
			
			
		if (bDatabasesSpecified) 
		{
			// Databases
			DWORD dwLength=1;
			BYTE* pOrig=pData;
			while (*pData!='\0')
			{
				int iStrLen=(int)istrlen(LPSTR(pData))+1;
				dwLength+=iStrLen;
				pData+=iStrLen;
			}
			m_pDatabases=alloccopyAtoW((LPCSTR)pOrig,dwLength);
		}
		else
			m_pDatabases=NULL;
		pData++;
	}
	
	if (m_tLastStartDate.wYear<1900) // There is something wrong
		GetCurrentDateAndTime(&m_tLastStartDate,&m_tLastStartTime);
}

DWORD CSchedule::GetDataLen() const
{
	if (m_pDatabases==NULL)
	{
		//DebugFormatMessage("GetDataLen returns SCHEDULE_V34_LEN+1=%d",SCHEDULE_V34_LEN+1);
		return SCHEDULE_V34_LEN+1;
	}
    	
	DWORD dwLength=SCHEDULE_V34_LEN+1;
	LPWSTR pPtr=m_pDatabases;
	while (*pPtr!='\0')
	{
        int iStrLen=(int)istrlenw(pPtr)+1;
		dwLength+=iStrLen;
		pPtr+=iStrLen;
	}
	
	//DebugFormatMessage("GetDataLen returns %d",dwLength);
	return dwLength;
}

DWORD CSchedule::GetData(BYTE* pData) const
{
	//DebugMessage("CSchedule::GetData BEGIN");

	CopyMemory(pData,this,SCHEDULE_V1_LEN);
	pData+=SCHEDULE_V1_LEN;

	//DebugMessage("1");

	*((DWORD*)pData)=m_pDatabases!=NULL;
	pData+=sizeof(DWORD);
	
	//DebugMessage("2");

	*((DWORD*)pData)=m_nThreadPriority;
	pData+=sizeof(DWORD);

	//DebugMessage("3");

	DWORD dwLength=SCHEDULE_V34_LEN+1;	
	if (m_pDatabases!=NULL)
	{
		//DebugMessage("4");

		LPWSTR pPtr=m_pDatabases;
		while (*pPtr!='\0')
		{
			int iStrLen=(int)istrlenw(pPtr)+1;
			pData+=MemCopyWtoA((LPSTR)pData,iStrLen*2,pPtr,iStrLen);
			
			dwLength+=iStrLen;
			pPtr+=iStrLen;
		}
	}

	//DebugMessage("5");

    *pData='\0';

	//DebugFormatMessage("CSchedule::GetData END dwLength=%d",dwLength);
	return dwLength;
}


	

void CSchedule::GetString(CStringW& str) const
{
	CStringW time;
	SYSTEMTIME st;
	GetLocalTime(&st);
	st.wHour=m_tStartTime.bHour;
	st.wMinute=m_tStartTime.bMinute;
	st.wSecond=m_tStartTime.bSecond;
	str.Empty();
	
	if (IsUnicodeSystem())
	{
		GetTimeFormatW(LOCALE_USER_DEFAULT,0,&st,NULL,time.GetBuffer(200),200);
		time.FreeExtra();
	}
	else
	{
		char szBuffer[200];
		GetTimeFormat(LOCALE_USER_DEFAULT,0,&st,NULL,szBuffer,200);
		time=szBuffer;
	}

	switch (m_nType)
	{
	case typeMinutely:
		if (m_tMinutely.wEvery==1)
			str.LoadString(IDS_MINUTELYEVERYMINUTE);
		else
			str.Format(IDS_MINUTELYEVERYXMINUTE,(long)m_tMinutely.wEvery);
		break;
	case typeHourly:
		if (m_tHourly.wEvery==1)
			str.Format(IDS_HOURLYLYEVERYHOUR,(long)m_tHourly.wMinute);
		else
			str.FormatEx(IDS_HOURLYLYEVERYXHOUR,(long)m_tHourly.wEvery,(long)m_tHourly.wMinute);
		break;
	case typeDaily:
		if (m_tDaily.wEvery==1)
			str.Format(IDS_DAILYEVERYDAY,(LPCWSTR)time);
		else
			str.FormatEx(IDS_DAILYEVERYXDAYS,(LPCWSTR)time,(long)m_tDaily.wEvery);
		break;
	case typeWeekly:
		{
			CStringW days;
			if (m_tWeekly.bDays==0)
			{
				days.AddString(IDS_NONEPARENTHESIS);
				days << L' ';
			}
			else
			{

				if (m_tWeekly.bDays&CSchedule::SWEEKLYTYPE::Monday)
				{
					days.AddString(IDS_MON);
					days << L' ';
				}
				if (m_tWeekly.bDays&CSchedule::SWEEKLYTYPE::Tuesday)
				{
					days.AddString(IDS_TUE);
					days << L' ';
				}
				if (m_tWeekly.bDays&CSchedule::SWEEKLYTYPE::Wednesday)
				{
					days.AddString(IDS_WED);
					days << L' ';
				}
				if (m_tWeekly.bDays&CSchedule::SWEEKLYTYPE::Thursday)
				{
					days.AddString(IDS_THU);
					days << L' ';
				}
				if (m_tWeekly.bDays&CSchedule::SWEEKLYTYPE::Friday)
				{
					days.AddString(IDS_FRI);
					days << L' ';
				}
				if (m_tWeekly.bDays&CSchedule::SWEEKLYTYPE::Saturday)
				{
					days.AddString(IDS_SAT);
					days << L' ';
				}
				if (m_tWeekly.bDays&CSchedule::SWEEKLYTYPE::Sunday)
				{
					days.AddString(IDS_SUN);
					days << L' ';
				}
				
			}

			if (m_tWeekly.wEvery==1)
				str.FormatEx(IDS_WEEKLYEVERYWEEK,(LPCWSTR)time,(LPCWSTR)days);
			else
				str.FormatEx(IDS_WEEKLYEVERYXWEEKS,(LPCWSTR)time,(LPCWSTR)days,(int)m_tWeekly.wEvery);	
			break;
		}
	case typeMonthly:
		if (m_tMonthly.nType==CSchedule::SMONTHLYTYPE::Day)
			str.FormatEx(IDS_MONTHLYEVERYDAY,(LPCWSTR)time,(int)m_tMonthly.bDay);
		else
		{
			CStringW type;
			type.LoadString(IDS_WEEKFIRST+m_tMonthly.nWeek);
			type.MakeLower();
			st.wYear=1999;
			st.wMonth=8;
			st.wDay=2+m_tMonthly.bDay;
			str.FormatEx(IDS_MONTHLYEVERYTYPE,(LPCWSTR)time,(LPCWSTR)type,(LPCWSTR)ID2W(IDS_MONDAY+m_tMonthly.bDay));
		}
		break;
	case typeOnce:
		{
			st.wYear=m_dStartDate.wYear;
			st.wMonth=m_dStartDate.bMonth;
			st.wDay=m_dStartDate.bDay;
			if (IsUnicodeSystem())
			{
				WCHAR szDate[100];
				GetDateFormatW(LOCALE_USER_DEFAULT,DATE_SHORTDATE,&st,NULL,szDate,100);
				str.FormatEx(IDS_ATTIMEON,(LPCWSTR)time,szDate);
			}
			else
			{
				char szDate[100];
				GetDateFormat(LOCALE_USER_DEFAULT,DATE_SHORTDATE,&st,NULL,szDate,100);
				str.FormatEx(IDS_ATTIMEON,(LPCWSTR)time,A2W(szDate));
			}

			break;
		}
	case typeAtStartup:
		str.LoadString(IDS_ATSTARTUPSTR);
		break;
	}
	if (m_bFlags&flagRunned &&
		!(m_tLastStartTime.bHour==0 && m_tLastStartTime.bMinute==0 && m_tLastStartTime.bSecond==0 &&
		m_tLastStartDate.wYear<1995 && m_tLastStartDate.bMonth==0 && m_tLastStartDate.bDay==0))
	{
		SYSTEMTIME st;
		st.wYear=m_tLastStartDate.wYear;
		st.wMonth=m_tLastStartDate.bMonth;
		st.wDay=m_tLastStartDate.bDay;
		st.wHour=m_tLastStartTime.bHour;
		st.wMinute=m_tLastStartTime.bMinute;
		st.wSecond=m_tLastStartTime.bSecond;
		st.wMilliseconds=0;
		if (IsUnicodeSystem())
		{
			CStringW tmp;
			WCHAR szDate[100];
			WCHAR szTime[100];
			GetTimeFormatW(LOCALE_USER_DEFAULT,0,&st,NULL,szTime,100);
			GetDateFormatW(LOCALE_USER_DEFAULT,DATE_SHORTDATE,&st,NULL,szDate,100);
			tmp.FormatEx(IDS_LASTRUN,szDate,szTime);
			str << tmp;
		}
		else
		{
			CStringA tmp;
			CHAR szDate[100];
			CHAR szTime[100];
			GetTimeFormat(LOCALE_USER_DEFAULT,0,&st,NULL,szTime,100);
			GetDateFormat(LOCALE_USER_DEFAULT,DATE_SHORTDATE,&st,NULL,szDate,100);
			tmp.FormatEx(IDS_LASTRUN,szDate,szTime);
			str << tmp;
		}
		
	}
}

BOOL CSchedule::GetCurrentDateAndTime(SDATE* pDate,STIME* pTime,UINT* pnWeekDay)
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	if (pDate!=NULL)
		*pDate=st;
	if (pTime!=NULL)
		*pTime=st;
	if (pnWeekDay!=NULL)
		*pnWeekDay=st.wDayOfWeek;
	return TRUE;
}

DWORD CSchedule::WhenShouldRun(STIME& tTime,SDATE& tDate,UINT nDayOfWeek) const
{
	if (!(m_bFlags&flagEnabled))
		return (DWORD)-1;
	ASSERT(m_tLastStartDate.wYear>1900);
			
	switch (m_nType)
	{
	case typeMinutely:
		{
			// Computing difference of time in minutes
			
			// Minutes in day=24*60
			int dMinutes=(CTimeX::GetIndex(tDate)-CTimeX::GetIndex(m_tLastStartDate))*(24*60)+
				(int(tTime.bHour)-int(m_tLastStartTime.bHour))*60+
                int(tTime.bMinute)-int(m_tLastStartTime.bMinute);
			
			if (dMinutes>=m_tMinutely.wEvery)
				return 0;
			return (DWORD)-1;
		}
	case typeHourly:
		{
			int dDays=CTimeX::GetIndex(tDate)-CTimeX::GetIndex(m_tLastStartDate);
			if (dDays>1)
			{
				// Runned at least 2 days ago
				if (m_bFlags&flagAtThisTime)
					return tTime.bMinute==m_tHourly.wMinute?0:(DWORD)-1;
				return 0;
			}
			else if (dDays==1)
			{
				// Next day
				if (int(tTime.bHour)>int(m_tLastStartTime.bHour)-24+m_tHourly.wEvery)
				{
					if (m_bFlags&flagAtThisTime)
						return tTime.bMinute==m_tHourly.wMinute?0:(DWORD)-1;
					return 0;
				}
				else if (int(tTime.bHour)==int(m_tLastStartTime.bHour)-24+m_tHourly.wEvery)
				{
					if (m_bFlags&flagAtThisTime)
						return tTime.bMinute==m_tHourly.wMinute?0:(DWORD)-1;
					return tTime.bMinute>m_tHourly.wMinute?0:(DWORD)-1;
				}
				return (DWORD)-1;
			}
			if (tTime.bHour>m_tLastStartTime.bHour+m_tHourly.wEvery)
			{
				if (m_bFlags&flagAtThisTime)
					return tTime.bMinute==m_tHourly.wMinute?0:(DWORD)-1;
				return 0;
			}
			else if (tTime.bHour==m_tLastStartTime.bHour+m_tHourly.wEvery)
			{
				if (m_bFlags&flagAtThisTime)
					return tTime.bMinute==m_tHourly.wMinute?0:(DWORD)-1;
				return tTime.bMinute>=m_tHourly.wMinute?0:(DWORD)-1;
			}
			return (DWORD)-1;
		}
	case typeDaily: 
		{
			if (m_bFlags&flagRunned) // This has been runned before, 
									 // thus there should be at least one day between new run
			{
				int dx=CTimeX::GetIndex(tDate)-CTimeX::GetIndex(m_tLastStartDate);
				if (m_bFlags&flagAtThisTime)
				{
					if (dx>=(int)m_tDaily.wEvery)
					{
						int diff=tTime-m_tStartTime;
						if (diff>=0 && diff<60)
							return 0;
					}
				}
				else
				{
					if ((dx==(int)m_tDaily.wEvery && tTime>=m_tStartTime) ||
						dx>(int)m_tDaily.wEvery)
						return 0;
				}
			}
			else
			{
				ASSERT(m_tLastStartDate<=tDate);
				if (m_bFlags&flagAtThisTime)
				{
					int diff=tTime-m_tStartTime;
					if (diff>0 && diff<60)
						return 0;
				}
				else
				{
					
					if ((m_tLastStartDate<tDate &&  tTime>=m_tStartTime) ||
						(m_tLastStartTime<m_tStartTime && tTime>=m_tStartTime))
						return 0;
				}
			}
			return (DWORD)-1;
		}
	case typeWeekly: 
		{
			if (m_tWeekly.bDays==0)
				return (DWORD)-1;
			if (tDate==m_tLastStartDate && // This has been runned at this day
				(m_bFlags&flagRunned && m_tLastStartTime>m_tStartTime))
				return (DWORD)-1;
			BOOL bIsMondayFirst='0';
			GetLocaleInfo(LOCALE_USER_DEFAULT,LOCALE_IFIRSTDAYOFWEEK,(LPSTR)&bIsMondayFirst,2);
			bIsMondayFirst=bIsMondayFirst=='0'?TRUE:FALSE;


			if (m_bFlags&flagRunned)
			{
				int nWeekDiff=CTimeX::GetWeekIndex(tDate,bIsMondayFirst)-CTimeX::GetWeekIndex(m_tLastStartDate,bIsMondayFirst);
				ASSERT(nWeekDiff>=0);
				if (nWeekDiff>0 && nWeekDiff<m_tWeekly.wEvery)
					return (DWORD)-1;
			}
			
			switch (nDayOfWeek)
			{
			case 0:
				if (m_tWeekly.bDays&CSchedule::SWEEKLYTYPE::Sunday)
					break;
				return (DWORD)-1;
			case 1:
				if (m_tWeekly.bDays&CSchedule::SWEEKLYTYPE::Monday)
					break;
				return (DWORD)-1;
			case 2:
				if (m_tWeekly.bDays&CSchedule::SWEEKLYTYPE::Tuesday)
					break;
				return (DWORD)-1;
			case 3:
				if (m_tWeekly.bDays&CSchedule::SWEEKLYTYPE::Wednesday)
					break;
				return (DWORD)-1;
			case 4:
				if (m_tWeekly.bDays&CSchedule::SWEEKLYTYPE::Thursday)
					break;
				return (DWORD)-1;
			case 5:
				if (m_tWeekly.bDays&CSchedule::SWEEKLYTYPE::Friday)
					break;
				return (DWORD)-1;
			case 6:
				if (m_tWeekly.bDays&CSchedule::SWEEKLYTYPE::Saturday)
					break;
				return (DWORD)-1;
			}
			int diff=tTime-m_tStartTime;
			if (diff>0)
			{
				if (!(m_bFlags&flagAtThisTime) || diff<60)
					return 0;
			}
			return (DWORD)-1;
		}
	case typeMonthly: 
		{
			ASSERT(tDate>=m_tLastStartDate);

			if (m_bFlags&flagRunned && // Runned in this month?
				tDate.wYear==m_tLastStartDate.wYear && 
				tDate.bMonth==m_tLastStartDate.bMonth)
				return (DWORD)-1;

			if (m_tMonthly.nType==CSchedule::SMONTHLYTYPE::Day)
			{
				if (m_tMonthly.bDay==tDate.bDay)
				{
					int diff=tTime-m_tStartTime;
					if (diff>0)
					{
						if (!(m_bFlags&flagAtThisTime) || diff<60)
							return 0;
					}
					return (DWORD)-1;
				}
			}
			else
			{
				BOOL bIsMondayFirst='0';
				GetLocaleInfo(LOCALE_USER_DEFAULT,LOCALE_IFIRSTDAYOFWEEK,(LPSTR)&bIsMondayFirst,2);
				bIsMondayFirst=bIsMondayFirst=='0'?TRUE:FALSE;
				BYTE bDay=m_tMonthly.bDay==6?0:m_tMonthly.bDay+1;
				if (nDayOfWeek!=bDay)
					return (DWORD)-1;

				int nCurrentWeek=CTimeX::GetWeekIndex(tDate,bIsMondayFirst);
				int nWeekIndex;
				if (m_tMonthly.nWeek==SMONTHLYTYPE::LastWeek)
				{
					BYTE nLastDay=CTime::GetDaysInMonth(tDate.bMonth,tDate.wYear);
					int nLastDayIndex=CTime::GetIndex(nLastDay,tDate.bMonth,tDate.wYear);
					nWeekIndex=CTimeX::GetWeekIndex(nLastDayIndex,bIsMondayFirst);
					if (nLastDayIndex%7==0 && !bIsMondayFirst) // Last week contains only synday
					{
						if (bDay!=0)
							nWeekIndex--;
					}
					else
					{
						if (nLastDayIndex%7<bDay) // Last week does not have required day
							nWeekIndex--;
					}
				}
				else
				{
					int nFirstDayIndex=CTime::GetIndex(1,tDate.bMonth,tDate.wYear);
					nWeekIndex=CTimeX::GetWeekIndex(nFirstDayIndex,bIsMondayFirst)+m_tMonthly.nWeek;
					
					if (nFirstDayIndex%7==0 && bIsMondayFirst) // First week contains only sunday
					{
						if (bDay!=0)
							nWeekIndex++;
					}
					else
					{
						if (nFirstDayIndex%7>bDay) // First week does not have required day
							nWeekIndex++;	
					}
				}
				if (nWeekIndex!=nCurrentWeek)
					return (DWORD)-1;
			}
			int diff=tTime-m_tStartTime;
			if (diff>0)
			{
				if (!(m_bFlags&flagAtThisTime) || diff<60)
					return 0;
			}
			break;
		}
	case typeOnce: 
		if (m_bFlags&flagRunned)
			return (DWORD)-1;
		if (tDate==m_dStartDate)
		{
			int diff=tTime-m_tStartTime;
			if (diff>0)
			{
				if (!(m_bFlags&flagAtThisTime) || diff<60)
					return 0;
			}
			return (DWORD)-1;
		}
		else if (tDate>=m_dStartDate && !(m_bFlags&flagAtThisTime))
			return 0;
		else
			return (DWORD)-1;
		break;		
	case typeAtStartup: 
		return (DWORD)-1;
	}
	return (DWORD)-1;
}
	
BOOL CSchedule::STIME::operator>=(const STIME& t) const
{
	if (bHour>t.bHour)
		return TRUE;
	if (bHour<t.bHour)
		return FALSE;
	if (bMinute>t.bMinute)
		return TRUE;
	if (bMinute<t.bMinute)
		return FALSE;
	if (bSecond>=t.bSecond)
		return TRUE;
	return FALSE;
}

BOOL CSchedule::STIME::operator>(const STIME& t) const
{
	if (bHour>t.bHour)
		return TRUE;
	if (bHour<t.bHour)
		return FALSE;
	if (bMinute>t.bMinute)
		return TRUE;
	if (bMinute<t.bMinute)
		return FALSE;
	if (bSecond>t.bSecond)
		return TRUE;
	return FALSE;
}

BOOL CSchedule::SDATE::operator>=(const SDATE& t) const
{
	if (wYear>t.wYear)
		return TRUE;
	if (wYear<t.wYear)
		return FALSE;
	if (bMonth>t.bMonth)
		return TRUE;
	if (bMonth<t.bMonth)
		return FALSE;
	if (bDay>=t.bDay)
		return TRUE;
	return FALSE;
}

BOOL CSchedule::SDATE::operator>(const SDATE& t) const
{
	if (wYear>t.wYear)
		return TRUE;
	if (wYear<t.wYear)
		return FALSE;
	if (bMonth>t.bMonth)
		return TRUE;
	if (bMonth<t.bMonth)
		return FALSE;
	if (bDay>t.bDay)
		return TRUE;
	return FALSE;
}

