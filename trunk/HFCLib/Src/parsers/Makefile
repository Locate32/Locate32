# Makefile for HFC installation library
# usage: make -f makefile

CC=cl
LIB=lib
CP=copy
RM=rm
RMDIR=rmdir
MKDIR=mkdir


PREFIX="$(HFCROOT)"

# USING MT LIBS
CFLAGS=/O1 /D "WIN32" /D "HFC_MTLIBS" /D "NDEBUG" /D "_WINDOWS" /D "LIB" /FD \
	/EHsc /MT  /W3 /nologo /c /TP
CFLAGSD=/Od /D "WIN32" /D "HFC_MTLIBS" /D "_DEBUG" /D "_WINDOWS" /D "LIB" /FD \
	/EHsc /MTd /GS /W3 /nologo /c /Z7 /TP



OBJS=Release/Parsers.obj

DOBJS=Debug/Parsers.obj

all: debuglib releaselib

install: all
	$(CP) parsers.h $(PREFIX)\include
	$(CP) Library\parsers.lib $(PREFIX)\lib
	$(CP) Library\parsersd.lib $(PREFIX)\lib

install64: all
	$(CP) Library\parsers.lib $(PREFIX)\lib64
	$(CP) Library\parsersd.lib $(PREFIX)\lib64

debuglib: library Library/parsersd.lib

releaselib: library Library/parsers.lib

Library/parsers.lib: Release $(OBJS)
	$(LIB) /OUT:"Library/parsers.lib" $(OBJS) /NOLOGO 

Library/parsersd.lib: Debug $(DOBJS)
	$(LIB) /OUT:"Library/parsersd.lib" $(DOBJS) /NOLOGO 


clean: 
	$(RM) -rf Release > nul
	$(RM) -rf Debug > nul
	$(RM) -rf Library > nul

Release/Parsers.obj:
	$(CC) $(CFLAGS) /Fo".\$*.obj" Parsers.cpp 


Debug/Parsers.obj:
	$(CC) $(CFLAGSD) /Fo".\$*.obj" Parsers.cpp 


library:
	$(MKDIR) Library

release:
	$(MKDIR) Release

debug:
	$(MKDIR) Debug
