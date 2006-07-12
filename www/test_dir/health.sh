#!/bin/sh

echo "Content-type: text/html"
echo

echo "<HTML><TITLE>System Health</TITLE><BODY>"

echo "<H1>System Health for '`hostname`'</H1>"
echo "<H2>Processes</H2><TABLE BORDER=\"2\">"
ps -ef | sed -e "s/\(.*\)/<tr><td>\1<\/td><\/tr>/"

echo "</TABLE><H2>Free FileSystem Space</H2>"
echo "<TABLE BORDER=\"2\">"
df -h . | sed -e "s/\(.*\)/<tr><td>\1<\/td><\/tr>/"
echo "</TABLE></BODY></HTML>"
