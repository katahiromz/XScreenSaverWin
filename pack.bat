@echo off

if not exist build goto fail

set BUILD_DIR=build\x64\Release
if exist %BUILD_DIR% goto found_build_dir

set BUILD_DIR=build\Release
if exist %BUILD_DIR% goto found_build_dir

set BUILD_DIR=build\x64\Debug
if exist %BUILD_DIR% goto found_build_dir

set BUILD_DIR=build\Debug
if exist %BUILD_DIR% goto found_build_dir

set BUILD_DIR=build
if exist %BUILD_DIR% goto found_build_dir

:found_build_dir

if not exist XScreenSaverWin mkdir XScreenSaverWin

:skip
copy %BUILD_DIR%\*.exe XScreenSaverWin
copy %BUILD_DIR%\*.scr XScreenSaverWin
copy LICENSE.txt XScreenSaverWin
copy README.txt XScreenSaverWin
copy READMEJP.txt XScreenSaverWin
copy CHANGELOG.txt XScreenSaverWin

cd XScreenSaverWin
for %%i in (*.scr) do mkdir %%~ni_scr
for %%i in (*.scr) do move %%i %%~ni_scr
cd ..

copy savers\wgl\starwars.txt XScreenSaverWin
copy savers\wgl\starwars.txt XScreenSaverWin\starwars_scr

@rem delete NG screensavers
if exist XScreenSaverWin\apple2_scr rd /S /Q XScreenSaverWin\apple2_scr
if exist XScreenSaverWin\blitspin_scr rd /S /Q XScreenSaverWin\blitspin_scr
if exist XScreenSaverWin\fontglide_scr rd /S /Q XScreenSaverWin\fontglide_scr
if exist XScreenSaverWin\halo_scr rd /S /Q XScreenSaverWin\halo_scr
if exist XScreenSaverWin\mirrorblob_scr rd /S /Q XScreenSaverWin\mirrorblob_scr
if exist XScreenSaverWin\moire2_scr rd /S /Q XScreenSaverWin\moire2_scr
if exist XScreenSaverWin\piecewise_scr rd /S /Q XScreenSaverWin\piecewise_scr
if exist XScreenSaverWin\pyro_scr rd /S /Q XScreenSaverWin\pyro_scr
if exist XScreenSaverWin\qix_scr rd /S /Q XScreenSaverWin\qix_scr
if exist XScreenSaverWin\quasiCrystal_scr rd /S /Q XScreenSaverWin\quasiCrystal_scr
if exist XScreenSaverWin\slidescreen_scr rd /S /Q XScreenSaverWin\slidescreen_scr
if exist XScreenSaverWin\t3d_scr rd /S /Q XScreenSaverWin\t3d_scr
if exist XScreenSaverWin\timetunnel_scr rd /S /Q XScreenSaverWin\timetunnel_scr
if exist XScreenSaverWin\worm_scr rd /S /Q XScreenSaverWin\worm_scr

echo Success!
exit /b

:fail
echo ERROR: Not built yet!
