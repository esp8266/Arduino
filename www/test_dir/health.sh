#!/bin/sh

echo "Content-Type: text/html"
echo
echo "<html><title>System Health</title><body>"
echo "<h1>System Health for '`hostname`'</h1>"
echo "<h2>Processes</h2><table border=\"2\">"
ps -ef | sed -e "s/\(.*\)/<tr><td>\1<\/td><\/tr>/"
echo "</table><h2>Free FileSystem Space</h2>"
echo "<table border=\"2\">"
df -h . | sed -e "s/\(.*\)/<tr><td>\1<\/td><\/tr>/"
echo "</table></body></html>"
