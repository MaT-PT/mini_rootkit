@echo off
setlocal enabledelayedexpansion

title [ROOTKIT] Building COMPANION

:: Check if the compiler is already in the path
echo Checking for MSVC compiler (cl.exe)...
where /q cl.exe

:: If it is, build the program
IF NOT ERRORLEVEL 1 GOTO BUILD

:: Otherwise, try to find the compiler using vswhere
:: https://github.com/microsoft/vswhere
echo cl.exe not found in path, searching for Visual Studio installation...
FOR /F "usebackq tokens=*" %%i IN (`.\tools\vswhere.exe -products * -latest -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -find "VC\**\Build\vcvars64.bat"`) DO (
    :: Call vcvars64.bat to set the environment variables and build the program
    echo Found Visual Studio installation: %%i
    echo Setting up build environment...
    CALL "%%i"
    GOTO BUILD
)

GOTO NOTFOUND

:BUILD
:: Build the program using the MSVC compiler
echo Building companion program...

IF NOT EXIST .\obj\ MD obj

cl src\companion\companion.c /Fecompanion.exe /Fo:.\obj\ /Isrc

GOTO END

:NOTFOUND
echo "MSVC not found"

:END
:: Test if the script was called from the command line or from Windows Explorer
echo %CMDCMDLINE% | findstr /I /C:"cmd.exe /c" >NUL 2>NUL

:: If the script was called from the command line, exit immediately
IF ERRORLEVEL 1 GOTO :EOF

:: Otherwise, pause before exiting
pause
