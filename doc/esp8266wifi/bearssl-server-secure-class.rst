:orphan:

BearSSL Secure Server Class
---------------------------

Implements a TLS encrypted server with optional client certificate validation.  See `Server Class <server-class.rst>`__ for general information and `BearSSL Secure Client Class <bearssl-client-secure-class.rst>`__ for basic server and BearSSL concepts.

setBufferSizes(int recv, int xmit)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Similar to the `BearSSL::WiFiClientSecure` method, sets the receive and transmit buffer sizes.  Note that servers cannot request a buffer size from the client, so if these are shrunk and the client tries to send a chunk larger than the receive buffer, it will always fail.  Needs to be called before `begin()`

Setting Server Certificates
~~~~~~~~~~~~~~~~~~~~~~~~~~~

TLS servers require a certificate identifying itself and containing its public key, and a private key they will use to encrypt information with.  The application author is responsible for generating this certificate and key, either using a self-signed generator or using a commercial certification authority.  **Do not re-use the certificates included in the examples provided.**

This example command will generate a RSA 2048-bit key and certificate:

.. code:: bash

    openssl req -x509 -nodes -newkey rsa:2048 -keyout key.pem -out cert.pem -days 4096

Again, it is up to the application author to generate this certificate and key and keep the private key safe and **private.**

setRSACert(const BearSSL::X509List \*chain, const BearSSL::PrivateKey \*sk)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Sets a RSA certificate and key to be used by the server when connections are received.  Needs to be called before `begin()`

setECCert(const BearSSL::X509List \*chain, unsigned cert_issuer_key_type, const BearSSL::PrivateKey \*sk)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Sets an elliptic curve certificate and key for the server.  Needs to be called before `begin()`.

Client sessions (Resuming connections fast)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The TLS handshake process takes a long time because of all the back and forth between the client and the server.  You can shorten it by caching the clients' sessions which will skip a few steps in the TLS handshake.  In order for this to work, your client also needs to cache the session. `BearSSL::WiFiClientSecure <bearssl-client-secure-class.rst#sessions-resuming-connections-fast>`__ can do that as well as modern web browers.

Here are the kind of performance improvements that you'll be able to see for TLS handshakes with an ESP8266 with it's clock set at 160MHz on a network with fairly low latency:

* With an EC key of 256 bits, a request taking ~360ms without caching takes ~60ms with caching.
* With an RSA key of 2048 bits, a request taking ~1850ms without caching takes ~70ms with caching.

setCache(BearSSL::ServerSessions \*cache)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Sets the cache for the server's sessions.  When choosing the size of the cache, remember that each client session takes 100 bytes.  If you setup a cache for 10 sessions, it will take 1000 bytes.  Needs to be called before `begin()`

When creating the cache, you can use any of the 2 available constructors:

* `BearSSL::ServerSessions(ServerSession *sessions, uint32_t size)`: Creates a cache with the given buffer and number of sessions.
* `BearSSL::ServerSessions(uint32_t size)`: Dynamically allocates a cache for the given number of sessions.

Requiring Client Certificates
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

TLS servers can request the client to identify itself by transmitting a certificate during handshake.  If the client cannot transmit the certificate, the connection will be dropped by the server.

setClientTrustAnchor(const BearSSL::X509List \*client_CA_ta)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Sets the trust anchor (normally a self-signing CA) that all received certificates will be verified against.  Needs to be called before `begin()`.
