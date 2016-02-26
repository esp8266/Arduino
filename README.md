Replacement for Espressif's libssl, kept as close as possible to [axTLS](http://axtls.sourceforge.net/) source.
Currently based on axTLS 1.4.9, will be upgraded to 1.5.3.

[![Build status](https://travis-ci.org/igrr/axtls-8266.svg)](https://travis-ci.org/igrr/axtls-8266)

This is not a self-sufficient library. Application has to provide the following symbols (list not complete yet):
```
ax_port_malloc
ax_port_calloc
ax_port_realloc
ax_port_free
ax_port_read
ax_port_write
ax_port_open
ax_port_close
ax_get_file
phy_get_rand  (provided by the IoT SDK)
ets_printf    (in ESP8266 ROM)
ets_putc      (in ESP8266 ROM)
gettimeofday
time
ctime
```

For use with LwIP raw TCP API, see [compat/README.md](compat/README.md)

To build, add xtensa toolchain to your path, and run `make`.

See [LICENSE](LICENSE) file for axTLS license.
