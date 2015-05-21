

# Current version

- Add 32-bit Linux toolchain
- Better connection handling in ESP8266WebServer.
  The server now sends Content-Length and Connection: close headers,
  then waits for the client to disconnect. By not closing the connection
  actively, server avoids TIME_WAIT TCP state, and the TCP stack is able to 
  release the memory immediately, without waiting for 2xMSL period. 
  If the client doesn't disconnect in 2000ms, the server closes the connection
  actively.
- Add Hash library, which has a function to calculate SHA1 hash.

---

# 1.6.4-g545ffde
19 May, 2015

- Initial release of board manager package

