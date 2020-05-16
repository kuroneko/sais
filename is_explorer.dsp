# Microsoft Developer Studio Project File - Name="is_explorer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=is_explorer - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "is_explorer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "is_explorer.mak" CFG="is_explorer - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "is_explorer - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "is_explorer - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "is_explorer - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /I "c:\coding\SDL-1.2.8\include" /I "C:\coding\SDL_mixer-1.2.5\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 SDLmain.lib SDL.lib SDL_mixer.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libcmt" /out:"demo\strange.exe" /libpath:"c:\coding\SDL-1.2.8\lib" /libpath:"C:\coding\SDL_mixer-1.2.5\lib"

!ELSEIF  "$(CFG)" == "is_explorer - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "is_explorer___Win32_Debug"
# PROP BASE Intermediate_Dir "is_explorer___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "is_explorer___Win32_Debug"
# PROP Intermediate_Dir "is_explorer___Win32_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "c:\coding\SDL-1.2.8\include" /I "C:\coding\SDL_mixer-1.2.5\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 SDLmain.lib SDL.lib SDL_mixer.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"msvcrt" /nodefaultlib:"libcmt" /out:"demo\strange_dbg.exe" /pdbtype:sept /libpath:"c:\coding\SDL-1.2.8\lib" /libpath:"C:\coding\SDL_mixer-1.2.5\lib"

!ENDIF 

# Begin Target

# Name "is_explorer - Win32 Release"
# Name "is_explorer - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\cards.cpp
# End Source File
# Begin Source File

SOURCE=.\src\combat.cpp
# End Source File
# Begin Source File

SOURCE=.\src\combat_display.cpp
# End Source File
# Begin Source File

SOURCE=.\src\combat_init.cpp
# End Source File
# Begin Source File

SOURCE=.\src\combat_sim.cpp
# End Source File
# Begin Source File

SOURCE=.\src\combat_weapons.cpp
# End Source File
# Begin Source File

SOURCE=.\src\endgame.cpp
# End Source File
# Begin Source File

SOURCE=.\src\font.cpp
# End Source File
# Begin Source File

SOURCE=.\src\gfx.cpp
# End Source File
# Begin Source File

SOURCE=.\src\interface.cpp
# End Source File
# Begin Source File

SOURCE=.\src\is_fileio.cpp
# End Source File
# Begin Source File

SOURCE=.\src\main.cpp
# End Source File
# Begin Source File

SOURCE=.\src\modconfig.cpp
# End Source File
# Begin Source File

SOURCE=.\src\sdl_iface.cpp
# End Source File
# Begin Source File

SOURCE=.\src\sdl_main.cpp
# End Source File
# Begin Source File

SOURCE=.\src\sound.cpp
# End Source File
# Begin Source File

SOURCE=.\src\sprites.cpp
# End Source File
# Begin Source File

SOURCE=.\src\starmap.cpp
# End Source File
# Begin Source File

SOURCE=.\src\starmap_encounters.cpp
# End Source File
# Begin Source File

SOURCE=.\src\starmap_init.cpp
# End Source File
# Begin Source File

SOURCE=.\src\starmap_inventory.cpp
# End Source File
# Begin Source File

SOURCE=.\src\startgame.cpp
# End Source File
# Begin Source File

SOURCE=.\src\textstr.cpp
# End Source File
# Begin Source File

SOURCE=.\src\w32_gfx.cpp
# End Source File
# Begin Source File

SOURCE=.\src\w32_sound.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\src\cards.h
# End Source File
# Begin Source File

SOURCE=.\src\combat.h
# End Source File
# Begin Source File

SOURCE=.\src\endgame.h
# End Source File
# Begin Source File

SOURCE=.\src\gfx.h
# End Source File
# Begin Source File

SOURCE=.\src\iface_globals.h
# End Source File
# Begin Source File

SOURCE=.\src\interface.h
# End Source File
# Begin Source File

SOURCE=.\src\is_fileio.h
# End Source File
# Begin Source File

SOURCE=.\src\snd.h
# End Source File
# Begin Source File

SOURCE=.\src\starmap.h
# End Source File
# Begin Source File

SOURCE=.\src\startgame.h
# End Source File
# Begin Source File

SOURCE=.\src\textstr.h
# End Source File
# Begin Source File

SOURCE=.\src\Typedefs.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\src\Script1.rc
# End Source File
# Begin Source File

SOURCE=.\winicon.ico
# End Source File
# End Group
# End Target
# End Project
