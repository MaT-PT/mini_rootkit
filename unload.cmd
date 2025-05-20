@echo off

set DRVNAME=mini_rootkit
set SYSFILE=%~dp0\x64\Debug\%DRVNAME%.sys
set DRVDIR=%WINDIR%\System32\drivers

echo UNLOAD DRIVER %DRVNAME%

echo STOP/DELETE SERVICE
sc.exe stop %DRVNAME%
sc.exe delete %DRVNAME%

echo DELETE DRIVER FROM SYSTEM32\DRIVERS
del %DRVDIR%\%DRVNAME%.sys

pause
