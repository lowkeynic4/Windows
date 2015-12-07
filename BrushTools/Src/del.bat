echo off
echo ----------------------------------------------------
echo Press any key to delete all files with ending:
echo  *.aps *.idb *.ncp *.obj *.pch *.tmp *.sbr
echo Visual c++/.Net junk 
echo ----------------------------------------------------

del /F /Q /S *.aps *.idb *.ncp *.obj *.pch *.sbr *.tmp *.pdb *.bsc *.ilk *.res *.ncb *.opt *.suo *.manifest *.dep *.tli *.tlh *.SUP

FOR /R . %%d IN (.) DO rd /s /q "%%d\x64" 2>nul

FOR /R . %%d IN (.) DO rd /s /q "%%d\Debug" 2>nul

FOR /R . %%d IN (.) DO rd /s /q "%%d\Release" 2>nul

FOR /R . %%d IN (.) DO rd /s /q "%%d\Bin" 2>nul

FOR /R . %%d IN (.) DO rd /s /q "%%d\Obj" 2>nul

rem If the Properties directory is empty, remove it

FOR /R . %%d in (.) do rd /q "%%d\Properties" 2> nul