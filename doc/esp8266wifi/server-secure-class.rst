:orphan:

WiFiSecureServer Class
----------------------

Implements a TLS encrypted server with optional client certificate validation.  See `Server Class <server-class.rst>`__ for general information unrelated to SSL.

setBufferSizes(int recv, int xmit)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Similar to the `WiFiClientSecure` method, sets the receive and transmit buffer sizes.  Note that servers cannot request a buffer size from the client, so if these are shrunk and the client tries to send a chunk larger than the receive buffer, it will always fail.  This must be called before the server is 

Setting Server Certificates
~~~~~~~~~~~~~~~~~~~~~~~~~~~

TLS servers require a certificate identifying itself and containing its public key, and a private key they will use to encrypt information with.  The application author is responsible for generating this certificate and key, either using a self-signed generator or using a commercial certification authority.  **Do not re-use the certificates included in the examples provided.**

This example command will generate a RSA 2048-bit key and certificate:

.. code:: bash

    openssl req -x509 -nodes -newkey rsa:2048 -keyout key.pem -out cert.pem -days 4096

Again, it is up to the application author to generate this certificate and key and keep the private key safe and **private.**

setRSACert(const X509List \*chain, const PrivateKey \*sk)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Sets a RSA certificate and key to be used by the server when connections are received.  Needs to be called before `begin()`

setECCert(const X509List \*chain, unsigned cert_issuer_key_type, const PrivateKey \*sk)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Sets an elliptic curve certificate and key for the server.  Needs to be called before `begin()`.

Requiring Client Certificates
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

TLS servers can request the client to identify itself by transmitting a certificate during handshake.  If the client cannot transmit the certificate, the connection will be dropped by the server.

setClientTrustAnchor(const X509List \*client_CA_ta)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Sets the trust anchor (normally a self-signing CA) that all received certificates will be verified against.  Needs to be called before `begin()`.
