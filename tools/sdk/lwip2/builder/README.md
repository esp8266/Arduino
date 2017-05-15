
# lwIP-2 from original source

This repo offers an abstraction layer for esp8266 (nonos-sdk-2.0.0) for any
other ip implementation than the originally provided one (patched lwip-1.4). 
The original goal is to try and use clean lwIP for stability reasons.
original repository: https://github.com/d-a-v/esp8266-phy

# Note

* ipv6 not tried yet
* tcp is more stable
* needs testing

# Tested to work so far

* NTPClient
* WiFiAccessPoint
* OTA
* seems to solve some TCP issues

# rebuild

makefiles are working with linux/osx, and maybe with windows (using 'make' included in msys from mingw...)

```
cd <path-to-your>/esp8266/tools/sdk/lwip2/builder
```

get lwIP sources
```
./lwip2-update-stable
```

optionnally tune lwIP configuration in tools/sdk/lwip2/builder/glue-lwip2/lwipopts.h

build & install
```
make install
```

this will overwrite tools/sdk/{lib/liblwip2.a,lwip2/include/}

# about MSS

Remember the MSS footprint: 4*MSS bytes in RAM per tcp connection.
The lowest recommanded value is 536 which is the default here.
