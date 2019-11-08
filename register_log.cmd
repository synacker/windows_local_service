@echo off

setlocal
pushd "%~dp0"

set manifest=%~dp0manifest.man
set resource=%~dp0service_win.exe

wevtutil.exe im "%manifest%" /rf:"%resource%" /mf:"%resource%" /pf:"%resource%"
wevtutil.exe sl "service_win_log/log" /ab:true /rt:true /ms:4194304
wevtutil.exe sl "service_win_log/audit" /ab:true /rt:true /ms:4194304

popd
endlocal

exit /b