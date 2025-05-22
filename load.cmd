@echo off

set DRVNAME=mini_rootkit
set SYSFILE=%~dp0\x64\Debug\%DRVNAME%.sys
set DRVDIR=%WINDIR%\System32\drivers

echo LOAD DRVNAME %DRVNAME% for file %SYSFILE%

echo STOP/DELETE POTENTIAL PREVIOUS SERVICE
sc stop %DRVNAME%
sc delete %DRVNAME%

echo CLEAN REGISTRY KEYS
reg delete HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\%DRVNAME% /va /f

echo COPY DRIVER IN SYSTEM32\DRIVERS
copy /Y %SYSFILE% %DRVDIR%

echo ADD REGISTRY KEYS
reg import mini_rootkit_load.reg

echo CREATE SERVICE DEVICE
sc create %DRVNAME% type= kernel start= demand error= normal binPath= %DRVDIR%\%DRVNAME%.sys DisplayName= %DRVNAME%

echo START SERVICE DEVICE
sc start %DRVNAME%

:: pause
