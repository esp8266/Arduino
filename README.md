Replacement for Espressif's libssl, kept as close as possible to [axTLS](http://axtls.sourceforge.net/) source.
Currently based on axTLS 1.4.9, will be upgraded to 1.5.3.

This is not a self-sufficient library. Application has to provide the following symbols (list not complete yet):
```
malloc
calloc
free
abort
gettimeofday
time
ctime
printf
vprintf
```

Additionally, functions for non-blocking TCP socket reads and writes have to be provided (details TBD).

To build, add xtensa toolchain to your path, and run `make`.

See [LICENSE](LICENSE) file for axTLS license.
