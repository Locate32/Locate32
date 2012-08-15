********************************************************************
* An introduction to compile Locate32 sources using SVN repository *
********************************************************************
* Changes:                                                         *
*  31.12.07 Janne Huttunen: initial version                        *
*  02.10.08 Janne Huttunen: Notification about VS2008 solutions    
********************************************************************



Table of Contents

1. Preface
2. Downloading sources
3. Preliminary arrangements
4. Building sources
5. Using Visual Studio 2005
6. Other compilers


********************************************************************
1. Preface
********************************************************************

This file is intended to be a guide for downloading sources from SVN repository 
and compiling them. Instructions given this guide is dedicated for MS Visual Studio 2008
(Visual Studio 2005, see note below). This is due to the fact that I haven't tried other 
compilers yet. If you have managed to compile the sources using some other compiler, 
please contact the author (janne.huttunen@locate32.net) and share your experiences with us. 

You can also contact the author if you have comments or problems, or if something 
did went like described in this file. All comments are also very welcome. If you wish, 
you can also modify this document directly to add better description or just for spell checking. 
You can send the modified file to the author via email  or upload the file directly to SVN 
repository (if you ave an account to the repository). 




********************************************************************
2. Downloading sources
********************************************************************

At first, you need download the sources from the Locate32's SVN 
repository. To do this, you need a SVN client. If you haven't any SVN clients 
installed, I suggest you to try TortoiseSVN (http://tortoisesvn.tigris.org/), 
which is a very extensive and simple SVN client. 


The operation of downloading sources first time from the repository is called as "Checkout". 
The checkout sources using TortoiseSVN, you just need to riht-click some directory
in Windows Explorer and choose "SVN Checkout". When a dialog appears, you should type 
correct URL (http://svn.locate32.net/locate32) and checkout directory (the target directory, 
for example "C:\My documents\locate-sources") and press OK. Then Locate32's sources
are downloaded to the chosen target directory. 

The source directory can be updated to correspond the latest sources by right-clicking 
the source directory in Windows Explorer and choosing "SVN Update". 

 


********************************************************************
3. Preliminary arrangements
********************************************************************


To compile the sources, you need some libraries which are not included to the repository.
These libraries are bzip2 (http://www.bzip.org), PRRE (http://www.pcre.org/) and MD5 library 
(http://256.com/sources/md5/). Compiled binaries for these libraries can be found from 
the Locate32 source packages (http://www.locate32.net/files/sources/).  This source package
contains also a library called parsers which is also required to compile Locate32 sources but is
not included into the SVN repository. So easiest way is to download the latest source package 
and grab required libraries from it (see instructions below to see where to put these libraries).


Locate32 uses my own class library called HFC. Therefore, to compile Locate32 sources, you need
to compile HFC library first (the source package contains HFC library binaries but those
are old). 


The HFC library can be compiled using Visual Studio solution files. However, these solution 
files copies compiled library files to a certain directory which you have initialize first. 
To do that, do the following steps:

1. Create a directory for HFC library binaries. In this guide this directory is "C:\HFC", but
   you can also use other names. 

2. Extract directories 3rdparty, bin, include, lib andlib64 from the source package to this 
   directory. Note that these lib, lib64 and 3rdparty directories contain the required libraries 
   mentioned above.
   
3. Set environment variable HFCROOT with value "C:\HFC" (or the directory you crated, no apostrophes). 
   To set environment variables in Windows 2000/XP, right-click "My Computer" icon and choose 
   Properties from the pop up menu to open System Properties dialog. Environment Variables can 
   be found from Advanced tab. 

4. You need to setup Visual Studio to use these include and library paths. To do this on 
   Visual Studio 2008, choose Tools -> Options from the menu and choose 
   "Projects and Solutions" -> "VC++ Directories" from the list on left. Specify the following 
   directories (change "C:\HFC" to correspond your own path if necessary):

   "C:\HFC\include" and "C:\HFC\3rdparty\include" for include files (both Win32 and x64 platform)


   "C:\HFC\lib" and "C:\HFC\3rdparty\lib" for include files (Win32 platform)
   "C:\HFC\lib64" and "C:\HFC\3rdparty\lib64" for include files (x64 platform)

   You can ignore x64 bit platform if you have no 64 bit compilers installed.

5. To compile the language file (lan_en.dll) and the help file, you need to setup lrestool. 
   First, ensure that lrestool.exe (which is in bin directory) "is in path" so that it can 
   be found without specifying to the whole path to lrestool.exe file. In other words, check that
   that the environment variable PATH contains directory "C:\HFC\bin". Then give the following 
   commands in the command prompt (change C:\My documents\locate-sources to correspond your 
   source directory):

     lrestool -Ra LOCATE32 "C:\My documents\locate-sources\Locate\Locate32\lres_base.rc
     lrestool -Ha LOCATE32 "C:\My documents\locate-sources\Locate\hlp\page_raw.htm



   
********************************************************************
4. Building sources
********************************************************************


After you have made all initializations described above, building should be very easy task.
First, to compile HFC library, open HFCLib solution file which is in HFCLib directory 
(the sources downloaded from the repository) and choose Build -> Batch build. You should 
build all configurations show in the list (you can ignore x64 configurations). Finally, 
execute copyh.bat file in the same directory (this copies the latest header "*.h" and 
inline "*.inl" files from the source directory to C:\HFC\include). 


Finally, to compile Locate32 sources, open locate solution file (in Locate directory of 
the sources) and build desired configuration. Note that default confiration is usually 
"x64 Debug" (because I use it). 


You can also compile lrestool.exe yourself to ensure that you are using the latest version.
Source code for lrestool and a Visual Studio solution file is in lrestool directory.
The latest lrestool.exe is also in the latest locate_lan-X.XXXXX.zip package in
http://www.locate32.net/files/devel (these packages are dedicated for translators).  


********************************************************************
5. Using Visual Studio 2005
********************************************************************

The solution and project files were converted to Visual Studio 2008 on April 2008. 
The change was done in subversion repository 196.  This means that 
you can found solution and project files for 2005 from revision prior to 196 (e.g. 195).
However, these files do not contain changes made after this date, so you have to update
the solution and project files by yourself. The changes between 195 and the latest (210 
so far) are listed below:

** Changes in HFC Library solution:

HFCLib32 and HFCCon32/Header files:
- add HFCNetwork.h and HFCShell.h

HFCLib32 and HFCCon32/Source files:
- remove ShellExtension.cpp 
- add Shell.cpp and Network.cpp

** Changes in Locate solution:

common/Header files:
- add win95srcfixes.h

lan_en:
- helptext.txt and helptext2.txt are not used anymore. 

locate32/Header files
- Add strnatcmp.h from directory 3rdparty

locate32/Source files
- Add strnatcmp.cpp from directory 3rdparty

locate32/HTML Help Topics:
- There are so many changes that is not worthfile to give a list, an easier way is to 
remove all src files from project and then add all src files which are in Locate/hlp 
directory. 



********************************************************************
6. Other compilers
********************************************************************

This section gives a brief introduction to compile sources using other combilers than
Visual Studio 2008 or 2005. In that case you have to create project/make files by yourself. 
In the following I have listed that which libraries and modules you have to compile. 


1. HFC Libraries:

HFCLib.lib: 
- all cpp files in HFCLib\src directory
- no debug information, full optimizations
- preprosessor definitions: WIN32,NDEBUG,_WINDOWS,HFC_NOFORCELIBS,HFC_COMPILE

HFCLibd.lib: 
- all cpp files in HFCLib\src directory
- full debug information, no optimizations
- preprosessor definitions: WIN32,_DEBUG,_WINDOWS,HFC_NOFORCELIBS,HFC_COMPILE

HFCCon.lib: 
- all cpp files in HFCLib\src directory
- no debug information, full optimizations
- preprocessor definitions: WIN32,NDEBUG,_CONSOLE,HFC_NOFORCELIBS,HFC_COMPILE

HFCCond.lib: 
- all cpp files in HFCLib\src directory
- full debug information, no optimizations
- preprocessor definitions: WIN32,_DEBUG,_CONSOLE,HFC_NOFORCELIBS,HFC_COMPILE



2. Common module:

common.lib:
- Locate\common\common.cpp
- debugging and optimizations depending on what desired (VS solutions compiles
  separate debug and release versions)
- preprosessor definition: nothing special (I use WIN32;_DEBUG or NDEBUG;_LIB)



3. locatedb library

locatedb.lib:
- all cpp files in Locate\LocateDB
- debugging and optimizations depending on what desired (VS solutions compiles
  separate debug and release libraries)
- preprocessor definition: nothing special (I use WIN32;_DEBUG or NDEBUG;_LIB)
- uses common.lib 


4. locater library

locater.lib:
- all cpp files in Locate\Locater
- debugging and optimizations depending on what desired (VS solutions compiles
  separate debug and release libraries)
- preprocessor definition: WIN32;_DEBUG or NDEBUG;_LIB;LOCATER_COMBILELIB,PCRE_STATIC
- uses common.lib and libpcre.lib


5. locate.exe:
- console application
- locate\locate\locate.cpp, Locate\locater\Locater.cpp and Locate\Locatedb\Database.cpp (does 
  not use locater.lib because it is multi-thread). 
- preprocessor definition: WIN32;_DEBUG or NDEBUG;_CONSOLE;LOCATER_NOTHREAD;PCRE_STATIC
- uses common.lib and libpcre.lib


6. updtdb32.exe
- console application
- locate\updatedb\updatedb.cpp and locate\updatedb\updtdb32.rc
- preprocessor definition: WIN32;_DEBUG or NDEBUG;_CONSOLE
- uses common.lib and locatedb.lib


7. keyhook.dll
- Locate\Keyhook\keyhook.cpp and Locate\Locate32\shortcut.cpp
- preprocessor definition: WIN32;_DEBUG or NDEBUG;_WINDOWS;_USRDLL;KEYHOOK_EXPORTS;_WIN32_WINNT=0x0500


8. locate32.exe
- Win32 Application
- All cpp files in Locate\Locate32 and Locate\Locate32\3rdparty\CpuUsage.cpp
- preprocessor definition: WIN32;_DEBUG or NDEBUG;_WINDOWS
- uses common.lib, locater.lib, keyhook.lib and locatedb.lib


9. lan_en.dll
- type "lrestool English.lrf -o English.rc" in Locate\Languages\lan_en to compile resource script
- compile English.rc using resource compiler
- compile Locate\languages\lan_base\lan_base.cpp 
- link to lan_en.dll file


Notes:
- Use multi threated C++ (and other) libraries




