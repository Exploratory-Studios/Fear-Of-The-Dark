@echo off

SETLOCAL


call :makedirectories
call :copyassets
call :makedesktopshortcut
call :installdlls

exit /B 0

:makedirectories
mkdir "%AppData%/Exploratory Studios/Fear Of The Dark/Assets"
mkdir "%AppData%/Exploratory Studios/Fear Of The Dark/Saves"
exit /B 0

:copyassets
robocopy Assets "%AppData%/Exploratory Studios/Fear Of The Dark/Assets" /np /nfl /s /e /njh /njs /ndl /nc /ns
exit /B 0

:makedesktopshortcut
set SCRIPT="%TEMP%\%RANDOM%-%RANDOM%-%RANDOM%-%RANDOM%.vbs"
echo Set oWS = WScript.CreateObject("WScript.Shell") >> %SCRIPT%
echo sLinkFile = "%USERPROFILE%\Desktop\Fear Of The Dark.lnk" >> %SCRIPT%
echo Set oLink = oWS.CreateShortcut(sLinkFile) >> %SCRIPT%
echo oLink.TargetPath = "%~dp0\Fear Of The Dark.exe" >> %SCRIPT%
echo oLink.IconLocation = "%AppData%\Exploratory Studios\Fear Of The Dark\Assets\Logo.ico" >> %SCRIPT%
echo oLink.Save >> %SCRIPT%

cscript /nologo %SCRIPT%
del %SCRIPT%
exit /B 0

:installdlls
exit /B 0
