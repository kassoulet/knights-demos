# Microsoft Developer Studio Project File - Name="COCHLEA" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=COCHLEA - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "COCHLEA.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "COCHLEA.mak" CFG="COCHLEA - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "COCHLEA - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "COCHLEA - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "COCHLEA - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /GX /O1 /I ".\d3dframe\\" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /Zm1000 /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 winmm.lib ddraw.lib dxguid.lib d3dim.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /out:"Cochlea.exe"
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "COCHLEA - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I ".\d3dframe\\" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib ddraw.lib dxguid.lib d3dim.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Cochlea.scr" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "COCHLEA - Win32 Release"
# Name "COCHLEA - Win32 Debug"
# Begin Group "Bitmaps"

# PROP Default_Filter "*.bmp"
# Begin Source File

SOURCE=.\gfx\anim.bmp
# End Source File
# Begin Source File

SOURCE=.\gfx\cochlea\back.bmp
# End Source File
# Begin Source File

SOURCE=.\gfx\Firework.bmp
# End Source File
# Begin Source File

SOURCE=.\Knight.ico
# End Source File
# Begin Source File

SOURCE=.\gfx\KNIGHTs.bmp
# End Source File
# Begin Source File

SOURCE=.\gfx\Light.bmp
# End Source File
# Begin Source File

SOURCE=.\gfx\line.bmp
# End Source File
# Begin Source File

SOURCE=.\gfx\MY_GIRLA.bmp
# End Source File
# Begin Source File

SOURCE=.\gfx\phantasm8.bmp
# End Source File
# Begin Source File

SOURCE=.\gfx\pub.bmp
# End Source File
# Begin Source File

SOURCE=.\gfx\scroll.bmp
# End Source File
# Begin Source File

SOURCE=.\gfx\title.bmp
# End Source File
# Begin Source File

SOURCE=.\gfx\violet.bmp
# End Source File
# End Group
# Begin Group "D3DFrame"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\D3dframe\D3denum.cpp
# End Source File
# Begin Source File

SOURCE=.\D3dframe\D3denum.h
# End Source File
# Begin Source File

SOURCE=.\D3dframe\D3dframe.cpp
# End Source File
# Begin Source File

SOURCE=.\D3dframe\D3dframe.h
# End Source File
# Begin Source File

SOURCE=.\D3dframe\D3dmath.cpp
# End Source File
# Begin Source File

SOURCE=.\D3dframe\D3dmath.h
# End Source File
# Begin Source File

SOURCE=.\D3dframe\D3dtextr.cpp
# End Source File
# Begin Source File

SOURCE=.\D3dframe\D3dtextr.h
# End Source File
# Begin Source File

SOURCE=.\D3dframe\D3dutil.cpp
# End Source File
# Begin Source File

SOURCE=.\D3dframe\D3dutil.h
# End Source File
# Begin Source File

SOURCE=.\D3dframe\Userdlg.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\Cochlea.cpp
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\Winmain.cpp
# End Source File
# Begin Source File

SOURCE=.\Winmain.rc
# End Source File
# Begin Source File

SOURCE=.\Midas.lib
# End Source File
# End Target
# End Project
