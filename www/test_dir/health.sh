#!/bin/sh

echo "Content-type: text/html"
echo

echo "<HTML>"
echo "<TITLE>System Health</TITLE><BODY>"

echo "<H1>System Health for '`hostname`'</H1>"
echo "<H2>Processes</H2>"
echo "<TABLE BORDEr=\"2\">"
ps -ef | sed -e "s/\(.*\)/<tr><td>\1<\/td><\/tr>/"
echo "</TABLE>"

echo "<H2>Free FileSystem Space</H2>"
echo "<TABLE border=\"2\">"
df -h . | sed -e "s/\(.*\)/<tr><td>\1<\/td><\/tr>/"
echo "</TABLE>"

echo "</BODY></HTML>"

