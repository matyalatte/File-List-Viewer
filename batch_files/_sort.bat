@echo off
REM Sorts paths in alphabetical order

@if "%~1"=="" goto skip

@pushd %~dp0
FileListViewer -c sort %1
@popd

pause
:skip