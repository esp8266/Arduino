@echo off
if "%ANT_HOME%"=="" goto noAntHome
if "%JAVA_HOME%"=="" goto noJavaHome
call "%ANT_HOME%\bin\ant.bat" exe
goto end

:noAntHome
echo ANT_HOME environment variable is not set
goto end

:noJavaHome
echo JAVA_HOME environment variable is not set

:end
