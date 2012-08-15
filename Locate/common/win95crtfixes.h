/* 
This file provides replacements for CRT functions which does not work with WIN9x.
Include this file only once per project.

See http://louis.steelbytes.com/vs2008_vs_win40.html for more information. 
*/

#ifndef WIN95CRTFIXES_H
#define WIN95CRTFIXES_H

#ifndef _WIN64 // All 64 bit systems are 5.0 systems
#if !defined(_DEBUG) && !defined(_DEBUG_LOGGING) // Do not add fix for _DEBUG

extern "C" __declspec(noinline) HMODULE __cdecl _crt_waiting_on_module_handle(LPCWSTR szModuleName)
{
#define INCR_WAIT                          1000
#define _MAX_WAIT_MALLOC_CRT 60000
    char szModuleNameA[MAX_PATH];
    WideCharToMultiByte(CP_ACP,0,szModuleName,-1,szModuleNameA,_countof(szModuleNameA),NULL,NULL);
    unsigned long nWaitTime = INCR_WAIT;
    HMODULE hMod = GetModuleHandleA(szModuleNameA);
    while(hMod == NULL)
    {
        Sleep(nWaitTime);
        hMod = GetModuleHandleA(szModuleNameA);
        nWaitTime += INCR_WAIT;
        if(nWaitTime > _MAX_WAIT_MALLOC_CRT)
        {
                break;
        }
    }
    return hMod;
#undef INCR_WAIT
#undef _MAX_WAIT_MALLOC_CRT
}

extern "C" __declspec(noinline) int __cdecl __crtInitCritSecAndSpinCount (PCRITICAL_SECTION lpCriticalSection,DWORD dwSpinCount)
{
    int ret;
    __try {
        /*
         * Call the real InitializeCriticalSectionAndSpinCount
         */
        ret = InitializeCriticalSectionAndSpinCount(lpCriticalSection, dwSpinCount);
        if (GetVersion()&0x80000000)
            ret = TRUE; // when win9x assume true (else we would have thrown an exception - read the MSDN :-)
    }
    __except (_exception_code()== STATUS_NO_MEMORY ? EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH) {
        /*
         * Initialization failed by raising an exception, which is probably
         * STATUS_NO_MEMORY.  It is not safe to set the CRT errno to ENOMEM,
         * since the per-thread data may not yet exist.  Instead, set the Win32
         * error which can be mapped to ENOMEM later.
         */
        if (GetExceptionCode() == STATUS_NO_MEMORY) {
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        }
        ret = FALSE;
    }
    return ret;
}

#endif
#endif

#endif