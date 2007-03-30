#!/bin/sh

#
#  Copyright(C) 2006 Cameron Rich
#
#  This license is free software; you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This license is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public License
#  along with this license; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

#
# Test the various axssl bindings. To run it, got to the _install directory
# and run this script from there.
#

if grep "CONFIG_PLATFORM_WIN32=y" "../config/.config"  > /dev/null; then
    JAVA_BIN="/cygdrive/c/Program Files/Java/jdk1.5.0_06/bin"
    PERL_BIN="/cygdrive/c/Perl/bin/perl"
    KILL_AXSSL="kill %1"
    KILL_CSHARP="kill %1"
    KILL_PERL="kill %1"
    KILL_JAVA="kill %1"
else
    if grep "CONFIG_PLATFORM_CYGWIN=y" "../config/.config"  > /dev/null; then
        # no .net or java on cygwin
        PERL_BIN=/usr/bin/perl
        KILL_AXSSL="killall axssl"
        KILL_PERL="killall /usr/bin/perl"
    else     # Linux
        JAVA_BIN=/usr/java/default/bin
        PERL_BIN=/usr/bin/perl
        KILL_AXSSL="killall axssl"
        KILL_CSHARP="killall mono"
        KILL_PERL="killall /usr/bin/perl"
        RUN_CSHARP="mono"
        KILL_JAVA="killall $JAVA_BIN/java"
    fi
fi

BASE=..
SERVER_ARGS="s_server -accept 15001 -verify -CAfile $BASE/ssl/test/axTLS.ca_x509.cer"
CLIENT_ARGS="s_client -reconnect -connect localhost:15001 -verify -CAfile $BASE/ssl/test/axTLS.ca_x509.cer -key $BASE/ssl/test/axTLS.key_1024 -cert $BASE/ssl/test/axTLS.x509_1024.cer"

# check pem arguments
SERVER_PEM_ARGS="s_server -accept 15001 -pass abcd -key $BASE/ssl/test/axTLS.key_aes128.pem -cert $BASE/ssl/test/axTLS.x509_aes128.pem"
CLIENT_PEM_ARGS="s_client -connect localhost:15001 -CAfile $BASE/ssl/test/axTLS.ca_x509.pem -key $BASE/ssl/test/axTLS.key_1024.pem -cert $BASE/ssl/test/axTLS.x509_1024.pem"

export LD_LIBRARY_PATH=.:`perl -e 'use Config; print $Config{archlib};'`/CORE

if [ -x ./axssl ]; then
echo "############################# C SAMPLE ###########################"
./axssl $SERVER_ARGS &
echo "C Test passed" | ./axssl $CLIENT_ARGS
$KILL_AXSSL
sleep 1

./axssl $SERVER_PEM_ARGS &
echo "C Test passed" | ./axssl $CLIENT_PEM_ARGS
$KILL_AXSSL
sleep 2
echo "### C tests complete"
fi

if [ -f ./axtls.jar ]; then
echo "########################## JAVA SAMPLE ###########################"
"$JAVA_BIN/java" -jar ./axtls.jar $SERVER_ARGS &
echo "Java Test passed" | "$JAVA_BIN/java" -jar ./axtls.jar $CLIENT_ARGS
$KILL_JAVA
sleep 1

"$JAVA_BIN/java" -jar ./axtls.jar $SERVER_PEM_ARGS &
echo "Java Test passed" | "$JAVA_BIN/java" -jar ./axtls.jar $CLIENT_PEM_ARGS
$KILL_JAVA
sleep 2

echo "### Java tests complete"
fi

if [ -x ./axssl.csharp.exe ]; then
echo "############################ C# SAMPLE ###########################"
$RUN_CSHARP ./axssl.csharp.exe $SERVER_ARGS &
echo "C# Test passed" | $RUN_CSHARP ./axssl.csharp.exe $CLIENT_ARGS
sleep 1
$KILL_CSHARP
sleep 1

$RUN_CSHARP ./axssl.csharp.exe $SERVER_PEM_ARGS &
echo "C# Test passed" | $RUN_CSHARP ./axssl.csharp.exe $CLIENT_PEM_ARGS
sleep 1
$KILL_CSHARP
sleep 1

echo "### C# tests complete"
fi

if [ -x ./axssl.vbnet.exe ]; then
echo "######################## VB.NET SAMPLE ###########################"
./axssl.vbnet $SERVER_ARGS &
sleep 1
echo "VB.NET Test passed" | ./axssl.vbnet.exe $CLIENT_ARGS
kill %1
sleep 1

./axssl.vbnet $SERVER_PEM_ARGS &
sleep 1
echo "VB.NET Test passed" | ./axssl.vbnet.exe $CLIENT_PEM_ARGS
kill %1
sleep 1
echo "### VB.NET tests complete"
fi

if [ -f ./axssl.pl ]; then
echo "########################## PERL SAMPLE ###########################"
"$PERL_BIN" ./axssl.pl $SERVER_ARGS &
echo "Perl Test passed" | "$PERL_BIN" ./axssl.pl $CLIENT_ARGS
$KILL_PERL
sleep 1

"$PERL_BIN" ./axssl.pl $SERVER_PEM_ARGS &
echo "Perl Test passed" | "$PERL_BIN" ./axssl.pl $CLIENT_PEM_ARGS
$KILL_PERL
sleep 1
echo "### Perl tests complete"
fi

echo "########################## ALL TESTS COMPLETE ###########################"
