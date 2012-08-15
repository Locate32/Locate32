/* Copyright (c) 1997-2010 Janne Huttunen
   database updater v3.1.10.8220              */

#if !defined(DBDEFINITIONS_H)
#define DBDEFINITIONS_H

#if _MSC_VER >= 1000
#pragma once
#endif


enum CallingReason {
	Initializing=0,
	ClassShouldDelete=1,
	StartedDatabase=2, // New database, header info not read
	FinishedDatabase=3,
	FinishedUpdating=4,
	FinishedLocating=4,
	ScanningDatabase=5, // Header info has been read
	ErrorOccured=6,
	RootChanged=7,

	// Only for CDatabaseUpdater
	InitializeWriting=8,
	WritingDatabase=9, 
	
	// Only for CLocater
	SearchingStarted=8, 
	SearchingEnded=9, 
	RootInformationAvail=10 
};

enum UpdateError {
	ueSuccess = 0,
	ueError = 1,
	ueCreate = 2,
	ueWrite = 3,
	ueOpen = 4,
	ueRead = 5,
	ueAlloc = 6,
	ueUnknown = 7,
	ueCannotCreateThread = 8,
	ueStopped = 9,
	ueFolderUnavailable = 10,
	ueInvalidDatabase = 11,
	ueLimitReached = 12, // Only for CLocater
	ueCannotIncrement = 13,
	ueWrongCharset = 14,
	ueStillWorking = 0xF000
};


// Status flags (statusNotChanged is not by CDatabaseUpdater)
enum UpdateStatus {
	statusNotChanged,
	statusInitializing,
	statusScanning,
	statusWritingDB,
	statusInitializeWriting,
	statusFinishing,
	statusFinished,
	statusCustom1, // Application can use these
	statusCustom2
};	


#endif
