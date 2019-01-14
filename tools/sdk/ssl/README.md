# BearSSL ESP8266 builder

This directory contains the git submodule for the ESP8266 ported bearssl low-level library, a port of [BearSSL](https://www.bearssl.org)

If you are only working on the `BearSSL::` namespace functions in the
Arduino `ESP8266WiFi` library (`BearSSL::WiFiClientSecure`,
`BearSSL::WiFiServerSecure`, etc.) you do _NOT_ need to work in this
directory.  

Normal users can simply use the libbearssl.a file already included in
the `Arduino` repo.  Experienced users looking to work on the underlying
BearSSL-ESP8266 ported library can use this directory to automate the
build flow.


## Prerequisites
The tools directory needs to be populated (i.e. Arduino IDE should be able
to compile an executable probect. `get.py` should ensure this).

### UNIX-like system (Linux, Mac):
If you need to change the *.t0 (Forth-like language) you will need a
.NET-compatible runtime (such as `mono` under Linux) to rebuild the
resulant `.c` files.

### For Windows (untested)
Microsoft's .NET runtime must be installed to run the `.t0`->`.c` workflow.


## Building
* `make all`: Init the submodule, if needed, then build _but do not install_ the library
* `make install`: Init the submodule, if needed, then build and copy the library to the standard location in `tools/sdk/lib`

## Editing the library
`https://github.com/earlephilhower/bearssl-esp8266` is the current repository
for this library.  A `git remote` to the original BearSSL sources from
`https://bearssl.org/git/BearSSL` is added on submodule init.  You can either
manually do pulls, or `make merge-upstream` to bring in any BearSSL upstream
changes.

Documentation in the library README-esp8266 and git log describes the changes done.


Feel free to drop me a line at <earlephilhower@yahoo.com> if you have questions.

-Earle F. Philhower, III
