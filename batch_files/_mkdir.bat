@echo off
REM Makes directories from paths

@if "%~1"=="" goto skip

@pushd %~dp0

FileListViewer -c mkdir %1
@popd

pause
:skip