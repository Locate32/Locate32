/* Locate32 - Copyright (c) 1997-2010 Janne Huttunen */

#if !defined(DEBUGGING_H)
#define DEBUGGING_H

#if _MSC_VER >= 1000
#pragma once
#endif 


#ifdef _DEBUG
#define _DEBUG_LOGGING
#endif


// Enabling debug logging

#ifdef _DEBUG_LOGGING


//#define DEBUGMSG_DIALOGS
//#define DEBUGMSG_ITEMS
//#define DEBUGMSG_DBCALLBACK
//#define DEBUGMSG_FILENOTIFICATIONS
//#define DEBUGMSG_BACKGROUNDUPDATER
//#define DEBUGMSG_FILEOBJECT
//#define DEBUGMSG_SETTINGSDLG


#ifdef DEBUGMSG_DIALOGS
#define DlgDebugMessage(a)						DebugMessage(a)
#else
#define DlgDebugMessage(a) 
#endif

#ifdef DEBUGMSG_ITEMS
#define ItemDebugMessage(a)						DebugMessage(a)
#define ItemDebugFormatMessage1(a,b1)			DebugFormatMessage(a,b1)
#define ItemDebugFormatMessage4(a,b1,b2,b3,b4)	DebugFormatMessage(a,b1,b2,b3,b4)
#else
#define ItemDebugMessage(a) 
#define ItemDebugFormatMessage1(a,b1)
#define ItemDebugFormatMessage4(a,b1,b2,b3,b4)
#endif

#ifdef DEBUGMSG_FILENOTIFICATIONS
#define FnDebugMessage(a)						DebugMessage(a)
#define FnDebugMessage1(a,b1)					DebugFormatMessage(a,b1)
#define FnDebugMessage2(a,b1,b2)				DebugFormatMessage(a,b1,b2)
#define FnDebugMessage3(a,b1,b2,b3)				DebugFormatMessage(a,b1,b2,b3)
#define FnDebugMessage4(a,b1,b2,b3,b4)			DebugFormatMessage(a,b1,b2,b3,b4)
#define FnDebugMessage5(a,b1,b2,b3,b4,b5)		DebugFormatMessage(a,b1,b2,b3,b4,b5)
#else
#define FnDebugMessage(a)		
#define FnDebugMessage1(a,b1)		
#define FnDebugMessage2(a,b1,b2)			
#define FnDebugMessage3(a,b1,b2,b3)			
#define FnDebugMessage4(a,b1,b2,b3,b4)			
#define FnDebugMessage5(a,b1,b2,b3,b4,b5)		
#endif

#ifdef DEBUGMSG_BACKGROUNDUPDATER
#define BuDebugMessage(a)						DebugMessage(a)
#define BuDebugMessage1(a,b1)					DebugFormatMessage(a,b1)
#define BuDebugMessage2(a,b1,b2)				DebugFormatMessage(a,b1,b2)
#define BuDebugMessage3(a,b1,b2,b3)				DebugFormatMessage(a,b1,b2,b3)
#define BuDebugMessage4(a,b1,b2,b3,b4)			DebugFormatMessage(a,b1,b2,b3,b4)
#define BuDebugMessage5(a,b1,b2,b3,b4,b5)		DebugFormatMessage(a,b1,b2,b3,b4,b5)
#else
#define BuDebugMessage(a)		
#define BuDebugMessage1(a,b1)		
#define BuDebugMessage2(a,b1,b2)			
#define BuDebugMessage3(a,b1,b2,b3)			
#define BuDebugMessage4(a,b1,b2,b3,b4)			
#define BuDebugMessage5(a,b1,b2,b3,b4,b5)		
#endif

#ifdef DEBUGMSG_DBCALLBACK
#define DbcDebugMessage(a)						DebugMessage(a)
#define DbcDebugFormatMessage2(a,b1,b2)			DebugFormatMessage(a,b1,b2)
#else
#define DbcDebugMessage(a) 
#define DbcDebugFormatMessage2(a,b1,b2)
#endif


#ifdef DEBUGMSG_FILEOBJECT
#define FoDebugMessage(a)						DebugMessage(a)
#define FoDebugFormatMessage2(a,b1,b2)			DebugFormatMessage(a,b1,b2)
#define FoDebugFormatMessage4(a,b1,b2,b3,b4)	DebugFormatMessage(a,b1,b2,b3,b4)
#else
#define FoDebugMessage(a) 
#define FoDebugFormatMessage2(a,b1,b2)
#define FoDebugFormatMessage4(a,b1,b2,b3,b4)
#endif



#ifdef DEBUGMSG_SETTINGSDLG
#define SdDebugMessage(a)						DebugMessage(a)
#define SdDebugMessage1(a,b1)					DebugFormatMessage(a,b1)
#define SdDebugMessage2(a,b1,b2)				DebugFormatMessage(a,b1,b2)
#define SdDebugMessage3(a,b1,b2,b3)				DebugFormatMessage(a,b1,b2,b3)
#define SdDebugMessage4(a,b1,b2,b3,b4)			DebugFormatMessage(a,b1,b2,b3,b4)
#define SdDebugMessage5(a,b1,b2,b3,b4,b5)		DebugFormatMessage(a,b1,b2,b3,b4,b5)
#else
#define SdDebugMessage(a)		
#define SdDebugMessage1(a,b1)		
#define SdDebugMessage2(a,b1,b2)			
#define SdDebugMessage3(a,b1,b2,b3)			
#define SdDebugMessage4(a,b1,b2,b3,b4)			
#define SdDebugMessage5(a,b1,b2,b3,b4,b5)		
#endif

#else

// Release version
#define DlgDebugMessage(a)

#define ItemDebugMessage(a)
#define ItemDebugFormatMessage1
#define ItemDebugFormatMessage4

#define FnDebugMessage(a)		
#define FnDebugMessage1(a,b1)		
#define FnDebugMessage2(a,b1,b2)			
#define FnDebugMessage3(a,b1,b2,b3)			
#define FnDebugMessage4(a,b1,b2,b3,b4)			
#define FnDebugMessage5(a,b1,b2,b3,b4,b5)		

#define BuDebugMessage(a)		
#define BuDebugMessage1(a,b1)		
#define BuDebugMessage2(a,b1,b2)			
#define BuDebugMessage3(a,b1,b2,b3)			
#define BuDebugMessage4(a,b1,b2,b3,b4)			
#define BuDebugMessage5(a,b1,b2,b3,b4,b5)		

#define DbcDebugMessage(a)
#define DbcDebugFormatMessage2(a,b1,b2)

#define FoDebugMessage(a) 
#define FoDebugFormatMessage2(a,b1,b2)
#define FoDebugFormatMessage4(a,b1,b2,b3,b4)

#define SdDebugMessage(a)		
#define SdDebugMessage1(a,b1)		
#define SdDebugMessage2(a,b1,b2)			
#define SdDebugMessage3(a,b1,b2,b3)			
#define SdDebugMessage4(a,b1,b2,b3,b4)			
#define SdDebugMessage5(a,b1,b2,b3,b4,b5)
#endif

#endif