@echo off

if exist XScreenSaverWin goto skip
if not exist xscreensaver\Release goto fail
if not exist xscreensaver-text\Release goto fail
if not exist non-wgl\Release goto fail
if not exist wgl\Release goto fail

if not exist XScreenSaverWin mkdir XScreenSaverWin

:skip
copy xscreensaver\Release\*.exe XScreenSaverWin
copy xscreensaver-text\Release\*.exe XScreenSaverWin
mkdir XScreenSaverWin\random_scr
copy random\Release\*.scr XScreenSaverWin\random_scr
copy non-wgl\Release\*.scr XScreenSaverWin
copy wgl\Release\*.scr XScreenSaverWin
copy LICENSE.txt XScreenSaverWin

cd XScreenSaverWin
for %%i in (*.scr) do mkdir %%~ni_scr
for %%i in (*.scr) do move %%i %%~ni_scr
cd ..

copy wgl\starwars.txt XScreenSaverWin
exit

:fail
echo ERROR: Not built yet!
