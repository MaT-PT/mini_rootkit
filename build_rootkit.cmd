@echo off

title [ROOTKIT] Building DRIVER

echo Checking if we're already in a WDK build environment...
IF NOT "%WDK_CURRENT_KIT_VERSION%"=="" GOTO BUILD

echo Setting up EWDK build environment for x64 target on x64 host...
call D:\BuildEnv\SetupBuildEnv.cmd amd64
:: Apparently, echo is turned back on after calling SetupBuildEnv.cmd, so we turn it off again
@echo off
title [ROOTKIT] Building DRIVER

:BUILD
echo Building the rootkit...
msbuild /t:clean /t:build /p:Platform=x64

:: Test if the script was called from the command line or from Windows Explorer
echo %CMDCMDLINE% | findstr /I /C:"cmd.exe /c" >NUL 2>NUL

:: If the script was called from the command line, exit immediately
IF ERRORLEVEL 1 GOTO :EOF

:: Otherwise, pause before exiting
pause
