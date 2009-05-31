#!/bin/sh

CLASSPATH=lib\\pde.jar\;lib\\core.jar\;lib\\jna.jar\;lib\\ecj.jar\;lib\\antlr.jar\;java\\lib\\tools.jar
export CLASSPATH

cd work && ./java/bin/java processing.app.Base
