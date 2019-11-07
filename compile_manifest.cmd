@echo off

setlocal
pushd "%~dp0"

set manifest=manifest.man

mc %manifest%
rc manifest.rc

popd
endlocal

exit /b