@echo off

setlocal
pushd "%~dp0"

sc delete service_win

popd
endlocal

exit /b