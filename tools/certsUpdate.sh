
# find `certUpdate` scripts in libraries, and execute them

cd ${0%/*} 2>/dev/null
find ../libraries -name certUpdate -exec bash -c "echo 'updating {}...'; {};" \;
echo done
