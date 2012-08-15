/* 
This file contains a modified CCpuUsage class from 
Plug-ins API for ATI Tray Tools v1.0, API Version 1.4, (c) 2005 Ray Adams
*/


#include <HFCLib.h>
#include "CpuUsage.h"


#include <comdef.h>	
#include <WinPerf.h>

#pragma comment(lib, "comsuppw.lib")


#define SYSTEM_OBJECT_INDEX					2		// 'System' object
#define PROCESS_OBJECT_INDEX				230		// 'Process' object
#define PROCESSOR_OBJECT_INDEX				238		// 'Processor' object
#define TOTAL_PROCESSOR_TIME_COUNTER_INDEX	240		// '% Total processor time' counter (valid in WinNT under 'System' object)
#define PROCESSOR_TIME_COUNTER_INDEX		6		// '% processor time' counter (for Win2K/XP)

///////////////////////////////////////////////////////////////////
//
//		GetCpuUsage uses the performance counters to retrieve the
//		system cpu usage.
//		The cpu usage counter is of type PERF_100NSEC_TIMER_INV
//		which as the following calculation:
//
//		Element		Value 
//		=======		===========
//		X			CounterData 
//		Y			100NsTime 
//		Data Size	8 Bytes
//		Time base	100Ns
//		Calculation 100*(1-(X1-X0)/(Y1-Y0)) 
//
//      where the denominator (Y) represents the total elapsed time of the 
//      sample interval and the numerator (X) represents the time during 
//      the interval when the monitored components were inactive.
//
//
//		Note:
//		====
//		On windows NT, cpu usage counter is '% Total processor time'
//		under 'System' object. However, in Win2K/XP Microsoft moved
//		that counter to '% processor time' under '_Total' instance
//		of 'Processor' object.
//		Read 'INFO: Percent Total Performance Counter Changes on Windows 2000'
//		Q259390 in MSDN.
//
///////////////////////////////////////////////////////////////////



CCpuUsage::CCpuUsage()
{
	m_bFirstTime = true;
	m_lnOldValue = 0;
	memset(&m_OldPerfTime100nSec, 0, sizeof(m_OldPerfTime100nSec));

	m_nPlatform = GetPlatform();
}

CCpuUsage::~CCpuUsage()
{
}

BOOL CCpuUsage::EnablePerformaceCounters(BOOL bEnable)
{
	if (m_nPlatform!= WIN2K_XP)
		return TRUE;

	CRegKey regKey;
	if (regKey.OpenKey(HKLM, "SYSTEM\\CurrentControlSet\\Services\\PerfOS\\Performance",CRegKey::defWrite) != ERROR_SUCCESS)
		return FALSE;

	regKey.SetValue("Disable Performance Counters",(DWORD)!bEnable);
	regKey.CloseKey();

	if (regKey.OpenKey(HKLM, "SYSTEM\\CurrentControlSet\\Services\\PerfProc\\Performance",CRegKey::defWrite) != ERROR_SUCCESS)
		return FALSE;

	regKey.SetValue("Disable Performance Counters",(DWORD)!bEnable);
	regKey.CloseKey();

	return TRUE;
}

//
//	GetCpuUsage returns the system-wide cpu usage.
//	Since we calculate the cpu usage by two samplings, the first
//	call to GetCpuUsage() returns 0 and keeps the values for the next
//	sampling.
//  Read the comment at the beginning of this file for the formula.
//
int CCpuUsage::GetCpuUsage()
{
	if (m_bFirstTime)
		EnablePerformaceCounters();
	
	// Cpu usage counter is 8 byte length.
	CPerfCounters PerfCounters;
	char szInstance[256] = {0};

//		Note:
//		====
//		On windows NT, cpu usage counter is '% Total processor time'
//		under 'System' object. However, in Win2K/XP Microsoft moved
//		that counter to '% processor time' under '_Total' instance
//		of 'Processor' object.
//		Read 'INFO: Percent Total Performance Counter Changes on Windows 2000'
//		Q259390 in MSDN.

	DWORD dwObjectIndex;
	DWORD dwCpuUsageIndex;
	switch (m_nPlatform)
	{
	case WINNT:
		dwObjectIndex = SYSTEM_OBJECT_INDEX;
		dwCpuUsageIndex = TOTAL_PROCESSOR_TIME_COUNTER_INDEX;
		break;
	case WIN2K_XP:
		dwObjectIndex = PROCESSOR_OBJECT_INDEX;
		dwCpuUsageIndex = PROCESSOR_TIME_COUNTER_INDEX;
		strcpy_s(szInstance,256,"_Total");
		break;
	default:
		return -1;
	}

	int				CpuUsage = 0;
	LONGLONG		lnNewValue = 0;
	PPERF_DATA_BLOCK pPerfData = NULL;
	LARGE_INTEGER	NewPerfTime100nSec = {0};

	lnNewValue = PerfCounters.GetCounterValue(&pPerfData, dwObjectIndex, dwCpuUsageIndex, szInstance);
	NewPerfTime100nSec = pPerfData->PerfTime100nSec;

	if (m_bFirstTime)
	{
		m_bFirstTime = false;
		m_lnOldValue = lnNewValue;
		m_OldPerfTime100nSec = NewPerfTime100nSec;
		return 0;
	}

	LONGLONG lnValueDelta = lnNewValue - m_lnOldValue;
	double DeltaPerfTime100nSec = (double)NewPerfTime100nSec.QuadPart - (double)m_OldPerfTime100nSec.QuadPart;

	m_lnOldValue = lnNewValue;
	m_OldPerfTime100nSec = NewPerfTime100nSec;

	double a = (double)lnValueDelta / DeltaPerfTime100nSec;

	double f = (1.0 - a) * 100.0;
	CpuUsage = (int)(f );	// rounding the result
	if (CpuUsage < 0)
		return 0;
	return CpuUsage;
}

int CCpuUsage::GetCpuUsage(LPCTSTR pProcessName)
{
	if (m_bFirstTime)
		EnablePerformaceCounters();

	// Cpu usage counter is 8 byte length.
	CPerfCounters PerfCounters;
	char szInstance[256] = {0};


	DWORD dwObjectIndex = PROCESS_OBJECT_INDEX;
	DWORD dwCpuUsageIndex = PROCESSOR_TIME_COUNTER_INDEX;
	strcpy_s(szInstance,256,pProcessName);

	int				CpuUsage = 0;
	LONGLONG		lnNewValue = 0;
	PPERF_DATA_BLOCK pPerfData = NULL;
	LARGE_INTEGER	NewPerfTime100nSec = {0};

	lnNewValue = PerfCounters.GetCounterValue(&pPerfData, dwObjectIndex, dwCpuUsageIndex, szInstance);
	NewPerfTime100nSec = pPerfData->PerfTime100nSec;

	if (m_bFirstTime)
	{
		m_bFirstTime = false;
		m_lnOldValue = lnNewValue;
		m_OldPerfTime100nSec = NewPerfTime100nSec;
		return 0;
	}

	LONGLONG lnValueDelta = lnNewValue - m_lnOldValue;
	double DeltaPerfTime100nSec = (double)NewPerfTime100nSec.QuadPart - (double)m_OldPerfTime100nSec.QuadPart;

	m_lnOldValue = lnNewValue;
	m_OldPerfTime100nSec = NewPerfTime100nSec;

	double a = (double)lnValueDelta / DeltaPerfTime100nSec;

	CpuUsage = (int) (a*100);
	if (CpuUsage < 0)
		return 0;
	return CpuUsage;
}



LONGLONG CCpuUsage::CPerfCounters::GetCounterValue(PERF_DATA_BLOCK **pPerfData, DWORD dwObjectIndex, DWORD dwCounterIndex, LPCTSTR pInstanceName)
{
	QueryPerformanceData(pPerfData, dwObjectIndex, dwCounterIndex);

	PPERF_OBJECT_TYPE pPerfObj = NULL;
	LONGLONG lnValue = {0};

	// Get the first object type.
	pPerfObj = FirstObject( *pPerfData );

	// Look for the given object index

	for( DWORD i=0; i < (*pPerfData)->NumObjectTypes; i++ )
	{

		if (pPerfObj->ObjectNameTitleIndex == dwObjectIndex)
		{
			lnValue = GetCounterValue(pPerfObj, dwCounterIndex, pInstanceName);
			break;
		}

		pPerfObj = NextObject( pPerfObj );
	}
	return lnValue;
}

void CCpuUsage::CPerfCounters::QueryPerformanceData(PERF_DATA_BLOCK **pPerfData, DWORD dwObjectIndex, DWORD dwCounterIndex)
{
	//
	// Since i want to use the same allocated area for each query,
	// i declare CBuffer as static.
	// The allocated is changed only when RegQueryValueEx return ERROR_MORE_DATA
	//
	static CBuffer Buffer(TOTALBYTES);

	DWORD BufferSize = Buffer.GetSize();
	LONG lRes;

	char keyName[8];
	sprintf_s(keyName,8,"%d %d",dwObjectIndex, dwCounterIndex);

	Buffer.Reset();
	while( (lRes = RegQueryValueEx( HKEY_PERFORMANCE_DATA,
							   keyName,
							   NULL,
							   NULL,
							   Buffer,
							   &BufferSize )) == ERROR_MORE_DATA )
	{
		// Get a buffer that is big enough.

		BufferSize += BYTEINCREMENT;
		Buffer.Realloc(BufferSize);
	}
	*pPerfData = (PPERF_DATA_BLOCK) Buffer.m_pBuffer;
}

LONGLONG CCpuUsage::CPerfCounters::GetCounterValue(PPERF_OBJECT_TYPE pPerfObj, DWORD dwCounterIndex, LPCTSTR pInstanceName)
{
	PPERF_COUNTER_DEFINITION pPerfCntr = NULL;
	PPERF_INSTANCE_DEFINITION pPerfInst = NULL;
	PPERF_COUNTER_BLOCK pCounterBlock = NULL;

	// Get the first counter.

	pPerfCntr = FirstCounter( pPerfObj );

	// Look for the index of '% Total processor time'

	for( DWORD j=0; j < pPerfObj->NumCounters; j++ )
	{
		if (pPerfCntr->CounterNameTitleIndex == dwCounterIndex)
			break;

		// Get the next counter.

		pPerfCntr = NextCounter( pPerfCntr );
	}

	if( pPerfObj->NumInstances == PERF_NO_INSTANCES )		
	{
		pCounterBlock = (PPERF_COUNTER_BLOCK) ((LPBYTE) pPerfObj + pPerfObj->DefinitionLength);
	}
	else
	{
		pPerfInst = FirstInstance( pPerfObj );
	
		// Look for instance pInstanceName
		_bstr_t bstrInstance;
		_bstr_t bstrInputInstance = pInstanceName;
		for( int k=0; k < pPerfObj->NumInstances; k++ )
		{
			bstrInstance = (wchar_t *)((PBYTE)pPerfInst + pPerfInst->NameOffset);
			if (!_stricmp((LPCTSTR)bstrInstance, (LPCTSTR)bstrInputInstance))
			{
				pCounterBlock = (PPERF_COUNTER_BLOCK) ((LPBYTE) pPerfInst + pPerfInst->ByteLength);
				break;
			}
			
			// Get the next instance.

			pPerfInst = NextInstance( pPerfInst );
		}
	}

	if (pCounterBlock)
	{
		LONGLONG *lnValue = NULL;
		lnValue = (LONGLONG*)((LPBYTE) pCounterBlock + pPerfCntr->CounterOffset);
		return *lnValue;
	}
	return -1;
}

CCpuUsage::PLATFORM CCpuUsage::GetPlatform()
{
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if (!GetVersionEx(&osvi))
		return UNKNOWN;
	switch (osvi.dwPlatformId)
	{
	case VER_PLATFORM_WIN32_WINDOWS:
		return WIN9X;
	case VER_PLATFORM_WIN32_NT:
		if (osvi.dwMajorVersion == 4)
			return WINNT;
		else
			return WIN2K_XP;
	}
	return UNKNOWN;
}