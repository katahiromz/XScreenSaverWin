# Microsoft Developer Studio Project File - Name="gle" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=gle - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "gle.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "gle.mak" CFG="gle - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gle - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "gle - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
F90=df.exe
RSC=rc.exe

!IF  "$(CFG)" == "gle - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE F90 /compile_only /nologo /warn:nofileopt
# ADD F90 /compile_only /nologo /warn:nofileopt
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x41d /d "NDEBUG"
# ADD RSC /l 0x41d /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\gle.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=md ..\include	md ..\include\GL	copy ..\..\src\gle.h ..\include\GL
# End Special Build Tool

!ELSEIF  "$(CFG)" == "gle - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE F90 /check:bounds /compile_only /debug:full /nologo /traceback /warn:argument_checking /warn:nofileopt
# ADD F90 /check:bounds /compile_only /debug:full /nologo /traceback /warn:argument_checking /warn:nofileopt
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "." /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x41d /d "_DEBUG"
# ADD RSC /l 0x41d /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\gled.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=md ..\include	md ..\include\GL	copy ..\..\src\gle.h ..\include\GL
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "gle - Win32 Release"
# Name "gle - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\src\ex_angle.c
# End Source File
# Begin Source File

SOURCE=..\..\src\ex_cut_round.c
# End Source File
# Begin Source File

SOURCE=..\..\src\ex_raw.c
# End Source File
# Begin Source File

SOURCE=..\..\src\extrude.c
# End Source File
# Begin Source File

SOURCE=..\..\src\intersect.c
# End Source File
# Begin Source File

SOURCE=..\..\src\qmesh.c
# End Source File
# Begin Source File

SOURCE=..\..\src\rot_prince.c
# End Source File
# Begin Source File

SOURCE=..\..\src\rotate.c
# End Source File
# Begin Source File

SOURCE=..\..\src\round_cap.c
# End Source File
# Begin Source File

SOURCE=..\..\src\segment.c
# End Source File
# Begin Source File

SOURCE=..\..\src\texgen.c
# End Source File
# Begin Source File

SOURCE=..\..\src\urotate.c
# End Source File
# Begin Source File

SOURCE=..\..\src\view.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\src\copy.h
# End Source File
# Begin Source File

SOURCE=..\..\src\extrude.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gle.h
# End Source File
# Begin Source File

SOURCE=..\..\src\intersect.h
# End Source File
# Begin Source File

SOURCE=..\..\src\port.h
# End Source File
# Begin Source File

SOURCE=..\..\src\rot.h
# End Source File
# Begin Source File

SOURCE=..\..\src\segment.h
# End Source File
# Begin Source File

SOURCE=..\..\src\tube_gc.h
# End Source File
# Begin Source File

SOURCE=..\..\src\vvector.h
# End Source File
# End Group
# End Target
# End Project
