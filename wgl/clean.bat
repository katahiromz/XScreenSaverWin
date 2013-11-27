@echo off

if exist *.user del *.user
if exist *.ncb del *.ncb
if exist *.suo del *.suo
if exist *.suo del /A:H *.suo

if not exist Debug goto skip1
cd Debug
if exist *.obj del *.obj
if exist *.pdb del *.pdb
if exist *.idb del *.idb
if exist *.ilk del *.ilk
if exist *.mani* del *.mani*
if exist *.res del *.res
if exist *.pch del *.pch
if exist BuildLog.htm del BuildLog.htm
if exist mt.dep del mt.dep
cd ..
:skip1

if not exist Release goto skip2
cd Release
if exist *.obj del *.obj
if exist *.pdb del *.pdb
if exist *.idb del *.idb
if exist *.ilk del *.ilk
if exist *.mani* del *.mani*
if exist *.res del *.res
if exist *.pch del *.pch
if exist BuildLog.htm del BuildLog.htm
if exist mt.dep del mt.dep
cd ..
:skip2
