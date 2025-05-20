@echo off

set DRVNAME=mini_rootkit
set SYSFILE=%~dp0\x64\Debug\%DRVNAME%.sys
set DRVDIR=%WINDIR%\System32\drivers

echo LOAD DRVNAME %DRVNAME% for file %SYSFILE%

echo STOP/DELETE POTENTIAL PREVIOUS SERVICE
sc.exe stop %DRVNAME%
sc.exe delete %DRVNAME%

echo COPY DRIVER IN SYSTEM32\DRIVERS
copy %SYSFILE% %DRVDIR%

echo CREATE SERVICE DEVICE
sc.exe create %DRVNAME% type= kernel start= demand error= normal binPath= %DRVDIR%\%DRVNAME%.sys DisplayName= %DRVNAME%

echo START SERVICE DEVICE
sc.exe start %DRVNAME%

pause
