@echo off

setlocal
pushd "%~dp0"

set manifest=%~dp0manifest.man

wevtutil.exe um "%manifest%"

popd
endlocal

exit /b