@echo off
echo fost-web
pushd %0\..

..\bjam %*

IF ERRORLEVEL 1 (
    echo fost-web %* BUILD FAIL
    popd
    copy
) ELSE (
    popd
)
