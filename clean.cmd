@echo off

echo Deleting companion.exe...
del companion.exe

echo Deleting directory obj...
rmdir /S /Q obj

echo Deleting directory x64...
rmdir /S /Q x64

:: Test if the script was called from the command line or from Windows Explorer
echo %CMDCMDLINE% | findstr /I /C:"cmd.exe /c" >NUL 2>NUL

:: If the script was called from the command line, exit immediately
IF ERRORLEVEL 1 GOTO :EOF

:: Otherwise, pause before exiting
pause
