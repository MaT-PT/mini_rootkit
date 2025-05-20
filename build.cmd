@echo off

echo #######################################
echo # Launching driver build script...    #
echo #######################################
echo.
start /I /B /WAIT cmd /C build_rootkit.cmd

echo.
echo #######################################
echo # Launching companion build script... #
echo #######################################
echo.
start /I /B /WAIT cmd /C build_companion.cmd

:: Test if the script was called from the command line or from Windows Explorer
echo %CMDCMDLINE% | findstr /I /C:"cmd.exe /c" >NUL 2>NUL

:: If the script was called from the command line, exit immediately
IF ERRORLEVEL 1 GOTO :EOF

:: Otherwise, pause before exiting
pause
