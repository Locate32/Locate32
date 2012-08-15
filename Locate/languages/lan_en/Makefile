#Makefile for VC++ compiler

LANGUAGE=english
LANDLL=lan_en.dll
LRFFILE=$(LANGUAGE).lrf
RESFILE=$(LANGUAGE).res
RCFILE=$(LANGUAGE).rc

#LDEXTRAPATH=-LIBPATH:"D:\Program Files\Microsoft Visual Studio .NET 2003\Vc7\lib"
#CCEXTRAPATH=-I "D:\Program Files\Microsoft Visual Studio .NET 2003\Vc7\include"

LINK=link
RC=rc
CC=cl
LRESTOOL=lrestool

INCLUDE=-I ../.. -I ./include 
LIBPATH=-LIBPATH:./lib


CPPFLAGS=/O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" \
	/D "_WINDLL" /D "_MBCS" /FD /MT /GS /W3 /nologo /c /Wp64 /Zi /TP \
	/D "WIN32_LEAN_AND_MEAN" /D "NOCRYPT" /D "NOGDI" /D "NOIME" /D "NOMCX"
LDFLAGS=/INCREMENTAL:NO /NOLOGO /DLL /SUBSYSTEM:WINDOWS /MACHINE:X86 
EXTRALIBS=version.lib user32.lib

all: $(LANDLL)

$(LANDLL): $(RESFILE) lan_base.obj
	$(LINK) $(LDFLAGS) /OUT:$(LANDLL) $(RESFILE) lan_base.obj $(LIBPATH) $(LDEXTRAPATH) $(EXTRALIBS) 
	
$(RESFILE): $(RCFILE)
	$(RC) $(INCLUDE) /fo $(RESFILE) $(RCFILE)
		
$(RCFILE): $(LRFFILE)
	$(LRESTOOL) -o $(RCFILE) $(LRFFILE)
	
lan_base.obj: lan_base.cpp
	$(CC) $(CPPFLAGS) $(INCLUDE) /out:lan_base.obj lan_base.cpp $(CCEXTRAPATH)
	
clean:
	del lan_base.obj
	del $(RESFILE)
	del $(RCFILE)
	del $(LANDLL)
