@echo off

setlocal
pushd "%~dp0"

set service=%~dp0service_win.exe
sc create service_win binPath=%service% type=own obj="NT AUTHORITY\LocalService"

popd
endlocal

exit /b
