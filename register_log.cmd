@echo off

setlocal
pushd "%~dp0"

set manifest=%~dp0manifest.man
set resource=%~dp0service_win.exe

icacls "%resource%" /grant "NT AUTHORITY\LocalService":RX /Q

wevtutil.exe im "%manifest%" /rf:"%resource%" /mf:"%resource%" /pf:"%resource%"

popd
endlocal

exit /b