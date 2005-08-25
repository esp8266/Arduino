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

CLASSPATH=\"java\\lib\\rt.jar\;lib\;lib\\build\;lib\\pde.jar\;lib\\kjc.jar\;lib\\antlr.jar\;lib\\oro.jar\;lib\\comm.jar\;lib\\RXTXcomm.jar\;${QT_JAVA_PATH}\"

export CLASSPATH

cd work && ./java/bin/java -Djava.compiler=NONE PdeBase
