echo off
echo ----------------------------------------------------
echo Press any key to delete all files with ending:
echo  *.aps *.idb *.ncp *.obj *.pch *.tmp *.sbr
echo Visual c++/.Net junk 
echo ----------------------------------------------------
rem /F 强制
rem /S 子目录也删除
rem /Q 安静模式,删除全局通配符时,不要求确认
del /F /Q /S *.aps *.idb *.ncp *.obj *.pch *.sbr *.tmp *.pdb *.bsc *.ilk *.res *.ncb *.opt *.manifest *.dep *.tli *.tlh *.SUP *.user


rem 删除所有文件夹名称为Debug的文件夹
rem 2>nul 不输出错误提示信息
FOR /R . %%d IN (.) DO rd /s /q "%%d\x64" 2>nul

FOR /R . %%d IN (.) DO rd /s /q "%%d\Debug" 2>nul

FOR /R . %%d IN (.) DO rd /s /q "%%d\Release" 2>nul

rem FOR /R . %%d IN (.) DO rd /s /q "%%d\Bin" 2>nul

FOR /R . %%d IN (.) DO rd /s /q "%%d\Obj" 2>nul

rem If the Properties directory is empty, remove it

FOR /R . %%d in (.) do rd /q "%%d\Properties" 2> nul

FOR /R . %%d in (.) do rd /q "%%d\ReleaseDll" 2> nul
