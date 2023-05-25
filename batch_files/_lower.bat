@echo off
REM Converts paths to lower case strings

@if "%~1"=="" goto skip

@pushd %~dp0
FileListViewer -c lower %1
@popd

pause
:skip