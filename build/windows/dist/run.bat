@echo off

REM --- if you're running out of memory, change the 128m 
REM --- (which means 128 megabytes) to something higher. 

set SAVEDCP=%CLASSPATH%
set SAVEDPATH=%PATH%
set JAVA_HOME="java\bin"

if %JAVA_HOME% == "" GOTO Error

set CLASSPATH=java\lib\rt.jar;lib;lib\build;lib\pde.jar;lib\antlr.jar;lib\RXTXcomm.jar;lib\oro.jar;lib\registry.jar;lib\mrj.jar
set PATH=java\bin;lib;%PATH%

REM start javaw -ms128m -mx128m processing.app.Base
%JAVA_HOME%\java processing.app.Base

set CLASSPATH=%SAVEDCP%
set PATH=%SAVEDPATH%

GOTO End

:Error
echo *** Error: cannot execute Arduino
echo *** Reason: lacking the directory where to find Java in your computer
echo ***
echo *** Fix: Edit run.bat and add the JAVA_HOME directory 
echo ***      at the 'set JAVA_HOME=""' line
echo ***
echo *** then call the batch file run.bat in order to execute Arduino

:End
Pause
echo Bye.