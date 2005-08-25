#!/bin/sh

QT_JAVA_PATH="$WINDIR\\system32\\QTJava.zip"
if test -f "${QT_JAVA_PATH}"
then
  #echo "Found Quicktime at $QT_JAVA_PATH"
else 
  QT_JAVA_PATH="$WINDIR\\system\\QTJava.zip"
  if test -f "${QT_JAVA_PATH}"
    echo "could not find qtjava.zip in either"
    echo "${WINDIR}\\system32\\qtjava.zip or"
    echo "${WINDIR}\\system\\qtjava.zip"
    echo "quicktime for java must be installed before building."
    exit 1;
  then
    #echo "Found Quicktime at $QT_JAVA_PATH"
  else
  fi
fi

# rxtx testing
#CLASSPATH=java\\lib\\rt.jar\;lib\;lib\\build\;lib\\pde.jar\;lib\\kjc.jar\;lib\\oro.jar\;lib\\RXTXcomm.jar\;${QT_JAVA_PATH}

# will this one work? or do the quotes have to be chopped?
#CLASSPATH=java\\lib\\rt.jar\;lib\;lib\\build\;lib\\pde.jar\;lib\\kjc.jar\;lib\\antlr.jar\;lib\\oro.jar\;lib\\comm.jar\;lib\\RXTXcomm.jar\;${QTJAVA}

# version for javac/1.1 testing
#CLASSPATH=java\\lib\\rt.jar\;lib\;lib\\build\;lib\\pde.jar\;lib\\kjc.jar\;lib\\oro.jar\;java\\lib\\ext\\comm.jar\;${QT_JAVA_PATH}\;..\\..\\macos9\\JDKClasses.zip\;..\\..\\macos9\\JDKToolsClasses.zip

# includes jaws.jar
#CLASSPATH=\"java\\lib\\rt.jar\;java\\lib\\jaws.jar\;lib\;lib\\build\;lib\\pde.jar\;lib\\kjc.jar\;lib\\antlr.jar\;lib\\oro.jar\;lib\\comm.jar\;lib\\RXTXcomm.jar\;${QT_JAVA_PATH}\"

CLASSPATH=\"java\\lib\\rt.jar\;lib\;lib\\build\;lib\\pde.jar\;lib\\core.jar\;lib\\mrj.jar\;lib\\antlr.jar\;lib\\oro.jar\;lib\\registry.jar\;${QT_JAVA_PATH}\"
export CLASSPATH

#cd work && ./java/bin/java -Xint PdeBase
cd work && ./java/bin/java processing.app.Base
#cd work && /cygdrive/c/jdk-1.3.1_11/bin/java PdeBase
