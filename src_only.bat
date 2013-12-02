@echo off

cd wgl
call src_only.bat
cd ..

cd non-wgl
call src_only.bat
cd ..

cd debughelper
call src_only.bat
cd ..

cd xscreensaver
call src_only.bat
cd ..

cd xscreensaver-text
call src_only.bat
cd ..

cd random
call src_only.bat
cd ..

if exist XScreenSaverWin rmdir /S /Q XScreenSaverWin
if exist Output rmdir /S /Q Output
