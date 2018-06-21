/*
  WiFiClientBearSSL- SSL client/server for esp8266 using BearSSL libraries
  - Mostly compatible with Arduino WiFi shield library and standard
    WiFiClient/ServerSecure (except for certificate handling).

  Copyright (c) 2018 Earle F. Philhower, III

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/


#ifndef wificlientbearssl_h
#define wificlientbearssl_h
#include "WiFiClient.h"
#include <bearssl/bearssl.h>
#include "BearSSLHelpers.h"
#include "CertStoreBearSSL.h"

namespace BearSSL {

class WiFiClientSecure : public WiFiClient {
  public:
    WiFiClientSecure();
    ~WiFiClientSecure() override;

    int connect(IPAddress ip, uint16_t port) override;
    int connect(const String host, uint16_t port) override;
    int connect(const char* name, uint16_t port) override;

    uint8_t connected() override;
    size_t write(const uint8_t *buf, size_t size) override;
    size_t write_P(PGM_P buf, size_t size) override;
    size_t write(const char *buf) {
      return write((const uint8_t*)buf, strlen(buf));
    }
    size_t write_P(const char *buf) {
      return write_P((PGM_P)buf, strlen_P(buf));
    }
    size_t write(Stream& stream); // Note this is not virtual
    int read(uint8_t *buf, size_t size) override;
    int available() override;
    int read() override;
    int peek() override;
    size_t peekBytes(uint8_t *buffer, size_t length) override;
    void stop() override;
    void flush() override;

    // Don't validate the chain, just accept whatever is given.  VERY INSECURE!
    void setInsecure() {
      _use_insecure = true;
    }
    // Assume a given public key, don't validate or use cert info at all
    void setKnownKey(const BearSSLPublicKey *pk, unsigned usages = BR_KEYTYPE_KEYX | BR_KEYTYPE_SIGN) {
      _knownkey = pk;
      _knownkey_usages = usages;
    }
    // Only check SHA1 fingerprint of certificate
    void setFingerprint(const uint8_t fingerprint[20]) {
      _use_fingerprint = true;
      memcpy_P(_fingerprint, fingerprint, 20);
    }
    // Accept any certificate that's self-signed
    void allowSelfSignedCerts() {
      _use_self_signed = true;
    }
    // Install certificates of trusted CAs or specific site
    void setTrustAnchors(const BearSSLX509List *ta) {
      _ta = ta;
    }
    // In cases when NTP is not used, app must set a time manually to check cert validity
    void setX509Time(time_t now) {
      _now = now;
    }
    // Install a client certificate for this connection, in case the server requires it (i.e. MQTT)
    void setClientRSACert(const BearSSLX509List *cert, const BearSSLPrivateKey *sk);
    void setClientECCert(const BearSSLX509List *cert, const BearSSLPrivateKey *sk,
                         unsigned allowed_usages, unsigned cert_issuer_key_type);

    // Sets the requested buffer size for transmit and receive
    void setBufferSizes(int recv, int xmit);

    // Return an error code and possibly a text string in a passed-in buffer with last SSL failure
    int getLastSSLError(char *dest = NULL, size_t len = 0);

    // Attach a preconfigured certificate store
    void setCertStore(CertStore *certStore) {
      _certStore = certStore;
    }

    // Check for Maximum Fragment Length support for given len
    static bool probeMaxFragmentLength(IPAddress ip, uint16_t port, uint16_t len);
    static bool probeMaxFragmentLength(const char *hostname, uint16_t port, uint16_t len);
    static bool probeMaxFragmentLength(const String host, uint16_t port, uint16_t len);

    // AXTLS compatbile wrappers
    bool verify(const char* fingerprint, const char* domain_name) { (void) fingerprint; (void) domain_name; return false; } // Can't handle this case, need app code changes
    bool verifyCertChain(const char* domain_name) { (void)domain_name; return connected(); } // If we're connected, the cert passed validation during handshake

    bool setCACert(const uint8_t* pk, size_t size);
    bool setCertificate(const uint8_t* pk, size_t size);
    bool setPrivateKey(const uint8_t* pk, size_t size);

    bool setCACert_P(PGM_VOID_P pk, size_t size) { return setCACert((const uint8_t *)pk, size); }
    bool setCertificate_P(PGM_VOID_P pk, size_t size) { return setCertificate((const uint8_t *)pk, size); }
    bool setPrivateKey_P(PGM_VOID_P pk, size_t size) { return setPrivateKey((const uint8_t *)pk, size); }

    bool loadCACert(Stream& stream, size_t size);
    bool loadCertificate(Stream& stream, size_t size);
    bool loadPrivateKey(Stream& stream, size_t size);

    template<typename TFile>
    bool loadCertificate(TFile& file) {
      return loadCertificate(file, file.size());
    }

    template<typename TFile>
    bool loadPrivateKey(TFile& file) {
      return loadPrivateKey(file, file.size());
    }

    template<typename TFile>
    bool loadCACert(TFile& file) {
      return loadCACert(file, file.size());
    }

  private:
    void _clear();
    void _clearAuthenticationSettings();
    // Only one of the following two should ever be != nullptr!
    std::shared_ptr<br_ssl_client_context> _sc;
    std::shared_ptr<br_ssl_server_context> _sc_svr;
    inline bool ctx_present() {
      return (_sc != nullptr) || (_sc_svr != nullptr);
    }
    br_ssl_engine_context *_eng; // &_sc->eng, to allow for client or server contexts
    std::shared_ptr<br_x509_minimal_context> _x509_minimal;
    std::shared_ptr<struct br_x509_insecure_context> _x509_insecure;
    std::shared_ptr<br_x509_knownkey_context> _x509_knownkey;
    std::shared_ptr<unsigned char> _iobuf_in;
    std::shared_ptr<unsigned char> _iobuf_out;
    time_t _now;
    const BearSSLX509List *_ta;
    CertStore *_certStore;
    int _iobuf_in_size;
    int _iobuf_out_size;
    bool _handshake_done;
    bool _oom_err;

    bool _use_insecure;
    bool _use_fingerprint;
    uint8_t _fingerprint[20];
    bool _use_self_signed;
    const BearSSLPublicKey *_knownkey;
    unsigned _knownkey_usages;

    unsigned char *_recvapp_buf;
    size_t _recvapp_len;

    bool _clientConnected(); // Is the underlying socket alive?
    bool _connectSSL(const char *hostName); // Do initial SSL handshake
    void _freeSSL();
    int _run_until(unsigned target, bool blocking = true);
    size_t _write(const uint8_t *buf, size_t size, bool pmem);
    bool _wait_for_handshake(); // Sets and return the _handshake_done after connecting

    // Optional client certificate
    const BearSSLX509List *_chain;
    const BearSSLPrivateKey *_sk;
    unsigned _allowed_usages;
    unsigned _cert_issuer_key_type;

    // Methods for handling server.available() call which returns a client connection.
    friend class WiFiServerSecure; // Server needs to access these constructors
    WiFiClientSecure(ClientContext *client, const BearSSLX509List *chain, unsigned cert_issuer_key_type,
                      const BearSSLPrivateKey *sk, int iobuf_in_size, int iobuf_out_size, const BearSSLX509List *client_CA_ta);
    WiFiClientSecure(ClientContext* client, const BearSSLX509List *chain, const BearSSLPrivateKey *sk,
                      int iobuf_in_size, int iobuf_out_size, const BearSSLX509List *client_CA_ta);

    // RSA keyed server
    bool _connectSSLServerRSA(const BearSSLX509List *chain, const BearSSLPrivateKey *sk, const BearSSLX509List *client_CA_ta);
    // EC keyed server
    bool _connectSSLServerEC(const BearSSLX509List *chain, unsigned cert_issuer_key_type, const BearSSLPrivateKey *sk,
                             const BearSSLX509List *client_CA_ta);

    // X.509 validators differ from server to client
    bool _installClientX509Validator(); // Set up X509 validator for a client conn.
    bool _installServerX509Validator(const BearSSLX509List *client_CA_ta); // Setup X509 client cert validation, if supplied

    uint8_t *_streamLoad(Stream& stream, size_t size);

    // AXTLS compatible mode needs to delete the stored certs and keys on destruction
    bool _deleteChainKeyTA;

  private:
    // Single memory buffer used for BearSSL auxilliary stack, insead of growing main Arduino stack for all apps
    static std::shared_ptr<uint8_t> _bearssl_stack;
    // The local copy, only used to enable a reference count
    std::shared_ptr<uint8_t> _local_bearssl_stack;
};

};

#endif
