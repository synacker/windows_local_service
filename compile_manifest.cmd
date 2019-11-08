@echo off

setlocal
pushd "%~dp0"

set manifest=manifest.man

mc -um %manifest%
rc manifest.rc

popd
endlocal

exit /b