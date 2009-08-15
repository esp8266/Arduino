#!/bin/sh

javadoc -public -notimestamp -d core \
    ../../core/src/processing/core/*.java \
    ../../core/src/processing/xml/*.java

# These have to be done in a certain order... Most classes need to know about
# core, and SVG needs to have the XML library opened earler. I'm probably not
# setting this up right, so if anyone knows how to do it without specifying
# all the directories like this, please let us know.
javadoc -public -notimestamp -d everything \
    -classpath ../../app/lib/antlr.jar:../../app/lib/jna.jar:../../serial/library/RXTXcomm.jar:../../opengl/library/jogl.jar:../../pdf/library/itext.jar:../../app/lib/ecj.jar \
    ../../core/src/processing/core/*.java \
    ../../core/src/processing/xml/*.java \
    ../../app/src/antlr/*.java \
    ../../app/src/antlr/java/*.java \
    ../../app/src/processing/app/*.java \
    ../../app/src/processing/app/debug/*.java \
    ../../app/src/processing/app/linux/*.java \
    ../../app/src/processing/app/macosx/*.java \
    ../../app/src/processing/app/preproc/*.java \
    ../../app/src/processing/app/syntax/*.java \
    ../../app/src/processing/app/tools/*.java \
    ../../app/src/processing/app/windows/*.java \
    ../../dxf/src/processing/dxf/*.java \
    ../../net/src/processing/net/*.java \
    ../../opengl/src/processing/opengl/*.java \
    ../../pdf/src/processing/pdf/*.java \
    ../../serial/src/processing/serial/*.java \
    ../../video/src/processing/video/*.java 

cp stylesheet.css core/
cp stylesheet.css everything/
