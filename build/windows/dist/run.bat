@echo off

REM --- if you're running out of memory, change the 128m 
REM --- (which means 128 megabytes) to something higher. 

set SAVEDCP=%CLASSPATH%
set SAVEDPATH=%PATH%

set CLASSPATH=java\lib\rt.jar;lib;lib\build;lib\pde.jar;lib\core.jar;lib\antlr.jar;lib\oro.jar;lib\registry.jar;lib\mrj.jar
set PATH=java\bin;%PATH%

start javaw -ms128m -mx128m processing.app.Base
REM start .\java\bin\javaw -ms128m -mx128m PdeBase

set CLASSPATH=%SAVEDCP%
set PATH=%SAVEDPATH%
