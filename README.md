This is an ESP8266 port of [axTLS](http://axtls.sourceforge.net/) library, currently based on axTLS 2.1.2 (SVN version 274). 

This library supports TLS 1.2, and the following cipher suites:

Cipher suite name (RFC)           | OpenSSL name  | Key exchange | Encryption |  Hash
----------------------------------|---------------|--------------|------------|---------
TLS_RSA_WITH_AES_128_CBC_SHA      | AES128-SHA    |      RSA     |  AES-128   | SHA-1
TLS_RSA_WITH_AES_256_CBC_SHA      | AES256-SHA    |      RSA     |  AES-256   | SHA-1
TLS_RSA_WITH_AES_128_CBC_SHA256   | AES128-SHA256 |      RSA     |  AES-128   | SHA-256
TLS_RSA_WITH_AES_256_CBC_SHA256   | AES256-SHA256 |      RSA     |  AES-256   | SHA-256

## Using the library

This is not a self-sufficient library. In addition to the standard C library functions, application has to provide the following functions:

```
ax_port_read
ax_port_write
ax_port_open
ax_port_close
ax_get_file
phy_get_rand  (provided by the IoT SDK)
ets_printf    (in ESP8266 ROM)
ets_putc      (in ESP8266 ROM)
```

For use with LwIP raw TCP API, see [compat/README.md](https://github.com/attachix/lwirax/blob/master/README.md)

## Building [![Build status](https://travis-ci.org/igrr/axtls-8266.svg)](https://travis-ci.org/igrr/axtls-8266)

To build, add xtensa toolchain to your path, and run `make`. The library will be built in `bin/` directory.

## Credits and license

[axTLS](http://axtls.sourceforge.net/) is written and maintained by Cameron Rich.

Other people have contributed to this port; see git logs for a full list.

See [LICENSE](LICENSE) file for axTLS license.
