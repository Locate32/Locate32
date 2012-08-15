/* 
This file contains a modified CCpuUsage class from 
Plug-ins API for ATI Tray Tools v1.0, API Version 1.4, (c) 2005 Ray Adams
*/

#ifndef _CPUUSAGE_H
#define _CPUUSAGE_H


#define TOTALBYTES    100*1024
#define BYTEINCREMENT 10*1024

class CCpuUsage
{
private:
	class CPerfCounters
	{
	public:
		CPerfCounters() {}
		~CPerfCounters() {}

		LONGLONG GetCounterValue(PERF_DATA_BLOCK **pPerfData, DWORD dwObjectIndex, 
			DWORD dwCounterIndex, LPCTSTR pInstanceName = NULL);
		

	protected:

		class CBuffer
		{
		public:
			CBuffer(UINT Size)
			{
				m_Size = Size;
				m_pBuffer = (LPBYTE) malloc( Size*sizeof(BYTE) );
			}
			~CBuffer() 
			{
				free(m_pBuffer);
			}
			void *Realloc(UINT Size)
			{
				m_Size = Size;
				m_pBuffer = (LPBYTE) realloc( m_pBuffer, Size );
				return m_pBuffer;
			}

			void Reset()
			{
				memset(m_pBuffer,NULL,m_Size);
			}
			operator LPBYTE ()
			{
				return m_pBuffer;
			}

			UINT GetSize()
			{
				return m_Size;
			}
		public:
			LPBYTE m_pBuffer;
		private:
			UINT m_Size;
		};

		//
		//	The performance data is accessed through the registry key 
		//	HKEY_PEFORMANCE_DATA.
		//	However, although we use the registry to collect performance data, 
		//	the data is not stored in the registry database.
		//	Instead, calling the registry functions with the HKEY_PEFORMANCE_DATA key 
		//	causes the system to collect the data from the appropriate system 
		//	object managers.
		//
		//	QueryPerformanceData allocates memory block for getting the
		//	performance data.
		//
		//
		void QueryPerformanceData(PERF_DATA_BLOCK **pPerfData, DWORD dwObjectIndex, DWORD dwCounterIndex);
		
		//
		//	GetCounterValue gets performance object structure
		//	and returns the value of given counter index .
		//	This functions iterates through the counters of the input object
		//	structure and looks for the given counter index.
		//
		//	For objects that have instances, this function returns the counter value
		//	of the instance pInstanceName.
		//
		LONGLONG GetCounterValue(PPERF_OBJECT_TYPE pPerfObj, DWORD dwCounterIndex, LPCTSTR pInstanceName);
		

		/*****************************************************************
		 *                                                               *
		 * Functions used to navigate through the performance data.      *
		 *                                                               *
		 *****************************************************************/

		PPERF_OBJECT_TYPE FirstObject( PPERF_DATA_BLOCK PerfData )
		{
			return( (PPERF_OBJECT_TYPE)((PBYTE)PerfData + PerfData->HeaderLength) );
		}

		PPERF_OBJECT_TYPE NextObject( PPERF_OBJECT_TYPE PerfObj )
		{
			return( (PPERF_OBJECT_TYPE)((PBYTE)PerfObj + PerfObj->TotalByteLength) );
		}

		PPERF_COUNTER_DEFINITION FirstCounter( PPERF_OBJECT_TYPE PerfObj )
		{
			return( (PPERF_COUNTER_DEFINITION) ((PBYTE)PerfObj + PerfObj->HeaderLength) );
		}

		PPERF_COUNTER_DEFINITION NextCounter( PPERF_COUNTER_DEFINITION PerfCntr )
		{
			return( (PPERF_COUNTER_DEFINITION)((PBYTE)PerfCntr + PerfCntr->ByteLength) );
		}

		PPERF_INSTANCE_DEFINITION FirstInstance( PPERF_OBJECT_TYPE PerfObj )
		{
			return( (PPERF_INSTANCE_DEFINITION)((PBYTE)PerfObj + PerfObj->DefinitionLength) );
		}

		PPERF_INSTANCE_DEFINITION NextInstance( PPERF_INSTANCE_DEFINITION PerfInst )
		{
			PPERF_COUNTER_BLOCK PerfCntrBlk;

			PerfCntrBlk = (PPERF_COUNTER_BLOCK)((PBYTE)PerfInst + PerfInst->ByteLength);

			return( (PPERF_INSTANCE_DEFINITION)((PBYTE)PerfCntrBlk + PerfCntrBlk->ByteLength) );
		}
	};

public:
	CCpuUsage();
	virtual ~CCpuUsage();

// Methods
	int GetCpuUsage();
	int GetCpuUsage(LPCTSTR pProcessName);

	BOOL EnablePerformaceCounters(BOOL bEnable = TRUE);

// Attributes
private:
	bool			m_bFirstTime;
	LONGLONG		m_lnOldValue ;
	LARGE_INTEGER	m_OldPerfTime100nSec;

	enum PLATFORM {
		WINNT,	WIN2K_XP, WIN9X, UNKNOWN
	} m_nPlatform;

	PLATFORM GetPlatform();
};


#endif