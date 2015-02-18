# Microsoft Developer Studio Project File - Name="NagEvLogCtrl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=NagEvLogCtrl - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "NagEvLogCtrl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "NagEvLogCtrl.mak" CFG="NagEvLogCtrl - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "NagEvLogCtrl - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "NagEvLogCtrl - Win32 Win NT Release" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "NagEvLogCtrl - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W4 /WX /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /fo"Release/NagEvLogCtrl.res" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib ws2_32.lib libmcrypt.lib /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /incremental:yes

!ELSEIF  "$(CFG)" == "NagEvLogCtrl - Win32 Win NT Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "NagEvLogCtrl___Win32_Win_NT_Release"
# PROP BASE Intermediate_Dir "NagEvLogCtrl___Win32_Win_NT_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "NagEvLogCtrl___Win32_Win_NT_Release"
# PROP Intermediate_Dir "NagEvLogCtrl___Win32_Win_NT_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /WX /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W4 /WX /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /D "WINDOWS_NT" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /fo"Release/NagEvLogCtrl.res" /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /fo"Release/NagEvLogCtrl.res" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib ws2_32.lib libmcrypt.lib /nologo /subsystem:windows /machine:I386
# SUBTRACT BASE LINK32 /incremental:yes
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib ws2_32.lib libmcrypt.lib /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /incremental:yes

!ENDIF 

# Begin Target

# Name "NagEvLogCtrl - Win32 Release"
# Name "NagEvLogCtrl - Win32 Win NT Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ConfigLogging.cpp
# End Source File
# Begin Source File

SOURCE=.\FGenEvent.cpp
# End Source File
# Begin Source File

SOURCE=".\nsca-netutils.c"
# SUBTRACT CPP /WX /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=".\nsca-utils.c"
# SUBTRACT CPP /WX /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\nsca.c
# SUBTRACT CPP /WX /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\NTService.cpp
# End Source File
# Begin Source File

SOURCE=.\NTSyslogCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\NTSyslogCtrl.rc
# End Source File
# Begin Source File

SOURCE=.\NTSyslogCtrlDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\nagevlog\safestr.c
# ADD CPP /Za
# SUBTRACT CPP /WX /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SelectServerDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\SyslogDaemonDlg.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ConfigLogging.h
# End Source File
# Begin Source File

SOURCE=.\FGenEvent.h
# End Source File
# Begin Source File

SOURCE=..\libmcrypt\mcrypt.h
# End Source File
# Begin Source File

SOURCE=.\nsca.h
# End Source File
# Begin Source File

SOURCE=.\NTService.h
# End Source File
# Begin Source File

SOURCE=.\NTService_msg.h
# End Source File
# Begin Source File

SOURCE=.\ntsl.h
# End Source File
# Begin Source File

SOURCE=.\NTSyslogCtrl.h
# End Source File
# Begin Source File

SOURCE=.\NTSyslogCtrlDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\safestr.h
# End Source File
# Begin Source File

SOURCE=.\SelectServerDlg.h
# End Source File
# Begin Source File

SOURCE=".\send-nsca.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\SyslogDaemonDlg.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\Error.ICO
# End Source File
# Begin Source File

SOURCE=.\res\GreenLight.ICO
# End Source File
# Begin Source File

SOURCE=.\res\NTSyslogCtrl.ico
# End Source File
# Begin Source File

SOURCE=.\res\NTSyslogCtrl.rc2
# End Source File
# Begin Source File

SOURCE=.\res\RedLight.ICO
# End Source File
# Begin Source File

SOURCE=.\res\YellowLight.ICO
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
