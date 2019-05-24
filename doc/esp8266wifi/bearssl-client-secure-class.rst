:orphan:

BearSSL WiFi Classes
--------------------

Methods and properties described in this section are specific to ESP8266. They are not covered in `Arduino WiFi library <https://www.arduino.cc/en/Reference/WiFi>`__ documentation. Before they are fully documented please refer to information below.

The `BearSSL <https://bearssl.org>`__ library (with modifications for ESP8266 compatibility and to use ROM tables whenever possible) is used to perform all cryptography and TLS operations.  The main ported repo is available `on GitHub <https://github.com/earlephilhower/bearssl-esp8266>`__.

CPU Requirements
~~~~~~~~~~~~~~~~

SSL operations take significant CPU cycles to run, so it is recommended that all TLS/SSL sketches to run at `160 Mhz` and not the default `80 Mhz`.  Even at 160 MHz, certain key exchanges can take multiple *seconds* of runtime to complete.  There is no special cryptographic hardware in the ESP8266, nor is there a 32x32=>64 multiplier, nor is the program stored in onboard RAM, so there is little that can be done to speed this up.

See the section on `sessions <#sessions-resuming-connections-fast>`__ and `limiting cryptographic negotiation <#limiting-ciphers-new-connections-faster>`__ for ways of ensuring faster modes are used.

Memory Requirements
~~~~~~~~~~~~~~~~~~~
BearSSL doesn't perform memory allocations at runtime, but it does require allocation of memory at the beginning of a connection.  There are two memory chunks required:
. A per-application secondary stack
. A per-connection TLS receive/transmit buffer plus overhead

The per-application secondary stack is approximately 5.6KB in size and is used for temporary variables during BearSSL processing.  Only one stack is required, and it will be allocated whenever any `BearSSL::WiFiClientSecure` or `BearSSL::WiFiServerSecure` are instantiated.  So, in the case of a global client or server, the memory will be allocated before `setup()` is called.

The per-connection buffers are approximately 22KB in size, but in certain circumstances it can be reduced dramatically by using MFLN or limiting message sizes.  See the `MLFN section <#mfln-or-maximum-fragment-length-negotiation-saving-ram>`__ below for more information.

Object Lifetimes
~~~~~~~~~~~~~~~~

There are many configuration options that require passing in a pointer to an object (i.e. a pointer to a private key, or a certificate list).  In order to preserve memory, BearSSL does NOT copy the objects passed in via these pointers and as such any pointer passed in to BearSSL needs to be preserved for the life of the client object.  For example, the following code is **in error**:

.. code:: cpp

    BearSSL::WiFiClientSecure client;
    const char x509CA PROGMEM = ".......";
    void setup() {
        BearSSL::X509List x509(x509CA);
        client.setTrustAnchor(&x509);
    }
    void loop() {
        client.connect("192.168.1.1", 443);
    }

Because the pointer to the local object `x509` no longer is valid after setup(), expect to crash in the main `loop()` where it is accessed by the `client` object.

As a rule, either keep your objects global, use `new` to create them, or ensure that all objects needed live inside the same scope as the client.

TLS and HTTPS Basics
~~~~~~~~~~~~~~~~~~~~

The following discussion is only intended to give a rough idea of TLS/HTTPS(which is just HTTP over a TLS connection) and the components an application needs to manage to make a TLS connection.  For more detailed information, please check the relevant `RFC 5246 <https://www.ietf.org/rfc/rfc5246>`__ and others.

TLS can be broken into two stages: verifying the identities of server (and potentially client), and then encrypting blocks of data bidirectionally.  Verifying the identity of the other partner is handled via keys encoded in X509 certificates, optionally signed by a series of other entities.


Public and Private Keys
~~~~~~~~~~~~~~~~~~~~~~~

Cryptographic keys are required for many of the BearSSL functions.  Both public and private keys are supported, with either Elliptic Curve or RSA key support.

To generate a public or private key from an existing PEM (ASCII format) or DER (binary format), the simplest method is to use the constructor:

.. code:: cpp

    BearSSL::PublicKey(const char *pemString)
    ... or ...
    BearSSL::PublicKey(const uint8_t *derArray, size_t derLen)

Note that `PROGMEM` strings and arrays are natively supported by these constructors and no special `*_P` modes are required.  There are additional functions to identify the key type and access the underlying BearSSL proprietary types, but they are not needed by user applications.

TLS Sessions
~~~~~~~~~~~~

TLS supports the notion of a session (completely independent and different from HTTP sessions) which allow clients to reconnect to a server without having to renegotiate encryption settings or validate X509 certificates.  This can save significant time (3-4 seconds in the case of EC keys) and can help save power by allowing the ESP8266 to sleep for a long time, reconnect and transmit some samples using the SSL session, and then jump back to sleep quicker.

`BearSSL::Session` is an opaque class.  Use the `BearSSL::WiFiClientSecure.setSession(&BearSSLSession)` method to apply it before the first `BearSSL::WiFiClientSecure.connect()` and it will be updated with session parameters during the operation of the connection.  After the connection has had `.close()` called on it, serialize the `BearSSL::Session` object to stable storage (EEPROM, RTC RAM, etc.) and restore it before trying to reconnect.  See the `BearSSL_Sessions` example for a detailed example.

`Sessions <#sessions-resuming-connections-fast>`__ contains additional information on the sessions API.

X.509 Certificate(s)
~~~~~~~~~~~~~~~~~~~~

X509 certificates are used to identify peers in TLS connections.  Normally only the server identifies itself, but the client can also supply an X509 certificate if desired (this is often done in MQTT applications).  The certificate contains many fields, but the most interesting in our applications are the name, the public key, and potentially a chain of signing that leads back to a trusted authority (like a global internet CA or a company-wide private certificate authority).

Any call that takes an X509 certificate can also take a list of X509 certificates, so there is no special `X509` class, simply `BearSSL::X509List` (which may only contain a single certificate).

Generating a certificate to be used to validate using the constructor

.. code:: cpp

    BearSSL::X509List(const char *pemX509);
    ...or...
    BearSSL::X509List(const uint8_t *derCert, size_t derLen);

If you need to add additional certificates (unlikely in normal operation), the `::append()` operation can be used.


Certificate Stores
~~~~~~~~~~~~~~~~~~

The web browser you're using to read this document keeps a list of 100s of certification authorities (CAs) worldwide that it trusts to attest to the identity of websites.

In many cases your application will know the specific CA it needs to validate web or MQTT servers against (often just a single, self-signing CA private to your institution).  Simply load your private CA in a `BearSSL::X509List` and use that as your trust anchor.

However, there are cases where you will not know beforehand which CA you will need (i.e. a user enters a website through a keypad), and you need to keep the list of CAs just like your web browser.  In those cases, you need to generate a certificate bundle on the PC while compiling your application, upload the `certs.ar` bundle to SPIFFS or SD when uploading your application binary, and pass it to a `BearSSL::CertStore()` in order to validate TLS peers.

See the `BearSSL_CertStore` example for full details as the `BearSSL::CertStore` requires the creation of a cookie-cutter object for filesystem access (because the SD and SPIFFS filesystems are presently incompatible with each other).  At a high level in your `setup()` you will call `BearSSL::initCertStore()` on a global object, and then pass this global certificate store to `client.setCertStore(&gCA)` before every connection attempt to enable it as a validation option.

Supported Crypto
~~~~~~~~~~~~~~~~

Please see the `BearSSL website <htps://bearssl.org>`__ for detailed cryptographic information.  In general, TLS 1.2, TLS 1.1, and TLS 1.0 are supported with RSA and Elliptic Curve keys and a very rich set of hashing and symmetric encryption codes.  Please note that Elliptic Curve (EC) key operations take a significant amount of time.


BearSSL::WiFiClientSecure Class
-------------------------------

`BearSSL::WiFiClientSecure` is the object which actually handles TLS encrypted WiFi connections to a remote server or client.  It extends `WiFiClient` and so can be used with minimal changes to code that does unsecured communications.

Validating X509 Certificates (Am I talking to the server I think I'm talking to?)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Prior to connecting to a server, the `BearSSL::WiFiClientSecure` needs to be told how to verify the identity of the other machine.  **By default BearSSL will not validate any connections and will refuse to connect to any server.**  This is a significant difference from the earlier `axTLS::WiFiClientSecure` in that the deprecated axTLS client would connect to any server and would only attempt to validate the identity of the remote server if asked to, after connection.

There are multiple modes to tell BearSSL how to verify the identity of the remote server.  See the `BearSSL_Validation` example for real uses of the following methods:

setInsecure()
^^^^^^^^^^^^^

Don't verify any X509 certificates.  There is no guarantee that the server connected to is the one you think it is in this case, but this call will mimic the behavior of the deprecated axTLS code.

setKnownKey(const BearSSL::PublicKey \*pk)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Assume the server is using the specific public key.  This does not verify the identity of the server or the X509 certificate it sends, it simply assumes that its public key is the one given.  If the server updates its public key at a later point then connections will fail.

setFingerprint(const uint8_t fp[20]) / setFingerprint(const char \*fpStr)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Verify the SHA1 fingerprint of the certificate returned matches this one.  If the server certificate changes, it will fail.  If an array of 20 bytes are sent in, it is assumed they are the binary SHA1 values.  If a `char*` string is passed in, it is parsed as a series of human-readable hex values separated by spaces or colons (e.g. `setFingerprint("00:01:02:03:...:1f");`)

setTrustAnchors(BearSSL::X509List \*ta)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Use the passed-in certificate(s) as a trust anchor, accepting remote certificates signed by any of these.  If you have many trust anchors it may make sense to use a `BearSSL::CertStore` because it will only require RAM for a single trust anchor (while the `setTrustAnchors` call requires memory for all certificates in the list).

setX509Time(time_t now)
^^^^^^^^^^^^^^^^^^^^^^^

For `setTrustAnchors` and `CertStore` , the current time (set via SNTP) is used to verify the certificate against the list, so SNTP must be enabled and functioning before the connection is attempted.  If you cannot use SNTP for some reason, you can manually set the "present time" that BearSSL will use to validate a certificate with this call where `now` is standard UNIX time.

Client Certificates (Proving I'm who I say I am to the server)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

TLS servers can request that a client identify themselves with an X509 certificate signed by a trust anchor it honors (i.e. a global TA or a private CA).  This is commonly done for applications like MQTT.  By default the client doesn't send a certificate, and in cases where a certificate is required the server will disconnect and no connection will be possible.

setClientRSACert / setClientECCert
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Sets a client certificate to send to a TLS server that requests one.  It should be called before `connect()` to add a certificate to the client in case the server requests it.  Note that certificates include both a certificate and a private key.  Both should be provided to you by your certificate generator.  Elliptic Curve (EC) keys require additional information, as shown in the prototype.

MFLN or Maximum Fragment Length Negotiation (Saving RAM)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Because TLS was developed on systems with many megabytes of memory, they require by default a 16KB buffer for receive and transmit.  That's enormous for the ESP8266, which has only around 40KB total heap available.

We can (and do) minimize the transmission buffer down to slightly more than 512 bytes to save memory, since BearSSL can internally ensure transmissions larger than that are broken up into smaller chunks that do fit.  But that still leaves the 16KB receive buffer requirement since we cannot in general guarantee the TLS peer will send in smaller chunks.

TLS 1.2 added MFLN, which lets a client negotiate smaller buffers with a server and reduce the memory requirements on the ESP8266.  Unfortunately, BearSSL needs to know the buffer sizes before it begins connection, so applications that want to use smaller buffers need to check the remote server's support before `connect()` .

probeMaxFragmentLength(host, port, len)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Use one of these calls **before** connection to determine if a specific fragment length is supported (len must be a power of two from 512 to 4096, per the specification).  This does **not** initiate a SSL connection, it simply opens a TCP port and performs a trial handshake to check support.

setBufferSizes(int recv, int xmit)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Once you have verified (or know beforehand) that MFLN is supported you can use this call to set the size of memory buffers allocated by the connection object.  This must be called **before** `connect()` or it will be ignored.

In certain applications where the TLS server does not support MFLN (not many do as of this writing as it is relatively new to OpenSSL), but you control both the ESP8266 and the server to which it is communicating, you may still be able to `setBufferSizes()` smaller if you guarantee no chunk of data will overflow those buffers.

bool getMFLNStatus()
^^^^^^^^^^^^^^^^^^^^

After a successful connection, this method returns whether or not MFLN negotiation succeeded or not.  If it did not succeed, and you reduced the receive buffer with `setBufferSizes` then you may experience reception errors if the server attempts to send messages larger than your receive buffer.

Sessions (Resuming connections fast)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

setSession(BearSSL::Session &sess)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If you are connecting to a server repeatedly in a fixed time period (usually 30 or 60 minutes, but normally configurable at the server), a TLS session can be used to cache crypto settings and speed up connections significantly.

Errors
~~~~~~

BearSSL can fail in many more unique and interesting ways then the deprecated axTLS.  Use these calls to get more information when something fails.  

getLastSSLError(char \*dest = NULL, size_t len = 0)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Returns the last BearSSL error code encountered and optionally set a user-allocated buffer to a human-readable form of the error.  To only get the last error integer code, just call without any parameters (`int errCode = getLastSSLError();`).

Limiting Ciphers (New connections faster)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

There is very rarely reason to use these calls, but they are available.

setCiphers()
^^^^^^^^^^^^

Takes an array (in PROGMEM is valid) or a std::vector of 16-bit BearSSL cipher identifiers and restricts BearSSL to only use them.  If the server requires a different cipher, then connection will fail.  Generally this is not useful except in cases where you want to connect to servers using a specific cipher.  See the BearSSL headers for more information on the supported ciphers.

setCiphersLessSecure()
^^^^^^^^^^^^^^^^^^^^^^

Helper function which essentially limits BearSSL to ciphers that were supported by the deprecated axTLS.  These may be less secure than the ones BearSSL would natively choose, but they may be helpful and faster if your server depended on specific axTLS crypto options.
