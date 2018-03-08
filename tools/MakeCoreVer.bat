@echo off

set buildpath=%1
set platformpath=%2
set version=%3

REM test by hand, from .\tools
REM set buildpath=.
REM set platformpath=..
REM set version=3.7.2-plus

REM todo: check args

if not exist "%buildpath%\core" mkdir "%buildpath%\core"

git --version > nul
if NOT ERRORLEVEL 1 goto :git

echo *** git is not available
( echo #define ARDUINO_ESP8266_GIT_VER 0x00000000 & echo #define ARDUINO_ESP8266_GIT_DESC win-%version% ) > "%buildpath%\core\core_version.h"
goto :end

:git
REM echo without newline: echo | set /p="nonewline"

( echo | set /p="#define ARDUINO_ESP8266_GIT_VER 0x" & git --git-dir "%platformpath%/.git" rev-parse --short=8 HEAD ) > "%buildpath%\core\core_version.h"
( echo | set /p="#define ARDUINO_ESP8266_GIT_DESC " & git --git-dir "%platformpath%/.git" describe --tags ) >> "%buildpath%\core\core_version.h"

:end
