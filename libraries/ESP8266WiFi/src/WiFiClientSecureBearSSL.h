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
#include <vector>
#include "WiFiClient.h"
#include <bearssl/bearssl.h>
#include "BearSSLHelpers.h"
#include "CertStoreBearSSL.h"

namespace BearSSL {

class WiFiClientSecure : public WiFiClient {
  public:
    WiFiClientSecure();
    WiFiClientSecure(const WiFiClientSecure &rhs);
    ~WiFiClientSecure() override;

    WiFiClientSecure& operator=(const WiFiClientSecure&) = default; // The shared-ptrs handle themselves automatically

    int connect(IPAddress ip, uint16_t port) override;
    int connect(const String& host, uint16_t port) override;
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
    bool flush(unsigned int maxWaitMs);
    bool stop(unsigned int maxWaitMs);
    void flush() override { (void)flush(0); }
    void stop() override { (void)stop(0); }

    // Allow sessions to be saved/restored automatically to a memory area
    void setSession(Session *session) { _session = session; }

    // Don't validate the chain, just accept whatever is given.  VERY INSECURE!
    void setInsecure() {
      _clearAuthenticationSettings();
      _use_insecure = true;
    }
    // Assume a given public key, don't validate or use cert info at all
    void setKnownKey(const PublicKey *pk, unsigned usages = BR_KEYTYPE_KEYX | BR_KEYTYPE_SIGN) {
      _clearAuthenticationSettings();
      _knownkey = pk;
      _knownkey_usages = usages;
    }
    // Only check SHA1 fingerprint of certificate
    bool setFingerprint(const uint8_t fingerprint[20]) {
      _clearAuthenticationSettings();
      _use_fingerprint = true;
      memcpy_P(_fingerprint, fingerprint, 20);
      return true;
    }
    bool setFingerprint(const char *fpStr);
    // Accept any certificate that's self-signed
    void allowSelfSignedCerts() {
      _clearAuthenticationSettings();
      _use_self_signed = true;
    }
    // Install certificates of trusted CAs or specific site
    void setTrustAnchors(const X509List *ta) {
      _clearAuthenticationSettings();
      _ta = ta;
    }
    // In cases when NTP is not used, app must set a time manually to check cert validity
    void setX509Time(time_t now) {
      _now = now;
    }
    // Install a client certificate for this connection, in case the server requires it (i.e. MQTT)
    void setClientRSACert(const X509List *cert, const PrivateKey *sk);
    void setClientECCert(const X509List *cert, const PrivateKey *sk,
                         unsigned allowed_usages, unsigned cert_issuer_key_type);

    // Sets the requested buffer size for transmit and receive
    void setBufferSizes(int recv, int xmit);

    // Returns whether MFLN negotiation for the above buffer sizes succeeded (after connection)
    int getMFLNStatus() {
      return connected() && br_ssl_engine_get_mfln_negotiated(_eng);
    }

    // Return an error code and possibly a text string in a passed-in buffer with last SSL failure
    int getLastSSLError(char *dest = NULL, size_t len = 0);

    // Attach a preconfigured certificate store
    void setCertStore(CertStore *certStore) {
      _certStore = certStore;
    }

    // Select specific ciphers (i.e. optimize for speed over security)
    // These may be in PROGMEM or RAM, either will run properly
    bool setCiphers(const uint16_t *cipherAry, int cipherCount);
    bool setCiphers(std::vector<uint16_t> list);
    bool setCiphersLessSecure(); // Only use the limited set of RSA ciphers without EC

    // Check for Maximum Fragment Length support for given len before connection (possibly insecure)
    static bool probeMaxFragmentLength(IPAddress ip, uint16_t port, uint16_t len);
    static bool probeMaxFragmentLength(const char *hostname, uint16_t port, uint16_t len);
    static bool probeMaxFragmentLength(const String& host, uint16_t port, uint16_t len);

    ////////////////////////////////////////////////////
    // AxTLS API deprecated warnings to help upgrading

    #define AXTLS_DEPRECATED \
      __attribute__((deprecated( \
        "This is deprecated AxTLS API, " \
        "check https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/WiFiClientSecure.h#L25-L99")))

    bool setCACert(const uint8_t* pk, size_t size)      AXTLS_DEPRECATED;
    bool setCertificate(const uint8_t* pk, size_t size) AXTLS_DEPRECATED;
    bool setPrivateKey(const uint8_t* pk, size_t size)  AXTLS_DEPRECATED;

    bool loadCACert(Stream& stream, size_t size)        AXTLS_DEPRECATED;
    bool loadCertificate(Stream& stream, size_t size)   AXTLS_DEPRECATED;
    bool loadPrivateKey(Stream& stream, size_t size)    AXTLS_DEPRECATED;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored  "-Wdeprecated-declarations"

    bool setCACert_P(PGM_VOID_P pk, size_t size) AXTLS_DEPRECATED {
      return setCACert((const uint8_t *)pk, size);
    }

    bool setCertificate_P(PGM_VOID_P pk, size_t size) AXTLS_DEPRECATED {
      return setCertificate((const uint8_t *)pk, size);
    }

    bool setPrivateKey_P(PGM_VOID_P pk, size_t size) AXTLS_DEPRECATED {
      return setPrivateKey((const uint8_t *)pk, size);
    }

#pragma GCC diagnostic pop

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

    bool verify(const char* fingerprint, const char* domain_name) AXTLS_DEPRECATED {
      (void)fingerprint;
      (void)domain_name;
      return connected();
    }

    bool verifyCertChain(const char* domain_name) AXTLS_DEPRECATED {
      (void)domain_name;
      return connected();
    }

    // AxTLS API deprecated section end
    /////////////////////////////////////

  protected:
    bool _connectSSL(const char *hostName); // Do initial SSL handshake

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
    const X509List *_ta;
    CertStore *_certStore;
    int _iobuf_in_size;
    int _iobuf_out_size;
    bool _handshake_done;
    bool _oom_err;

    // AXTLS compatibility shim elements:
    // AXTLS managed memory for certs and keys, while BearSSL assumes
    // the app manages these.  Use this local storage for holding the
    // BearSSL created objects in a shared form.
    std::shared_ptr<X509List>   _axtls_ta;
    std::shared_ptr<X509List>   _axtls_chain;
    std::shared_ptr<PrivateKey> _axtls_sk;

    // Optional storage space pointer for session parameters
    // Will be used on connect and updated on close
    Session *_session;

    bool _use_insecure;
    bool _use_fingerprint;
    uint8_t _fingerprint[20];
    bool _use_self_signed;
    const PublicKey *_knownkey;
    unsigned _knownkey_usages;

    // Custom cipher list pointer or NULL if default
    std::shared_ptr<uint16_t> _cipher_list;
    uint8_t _cipher_cnt;

    unsigned char *_recvapp_buf;
    size_t _recvapp_len;

    bool _clientConnected(); // Is the underlying socket alive?
    void _freeSSL();
    int _run_until(unsigned target, bool blocking = true);
    size_t _write(const uint8_t *buf, size_t size, bool pmem);
    bool _wait_for_handshake(); // Sets and return the _handshake_done after connecting

    // Optional client certificate
    const X509List *_chain;
    const PrivateKey *_sk;
    unsigned _allowed_usages;
    unsigned _cert_issuer_key_type;

    // Methods for handling server.available() call which returns a client connection.
    friend class WiFiServerSecure; // Server needs to access these constructors
    WiFiClientSecure(ClientContext *client, const X509List *chain, unsigned cert_issuer_key_type,
                      const PrivateKey *sk, int iobuf_in_size, int iobuf_out_size, const X509List *client_CA_ta);
    WiFiClientSecure(ClientContext* client, const X509List *chain, const PrivateKey *sk,
                      int iobuf_in_size, int iobuf_out_size, const X509List *client_CA_ta);

    // RSA keyed server
    bool _connectSSLServerRSA(const X509List *chain, const PrivateKey *sk, const X509List *client_CA_ta);
    // EC keyed server
    bool _connectSSLServerEC(const X509List *chain, unsigned cert_issuer_key_type, const PrivateKey *sk,
                             const X509List *client_CA_ta);

    // X.509 validators differ from server to client
    bool _installClientX509Validator(); // Set up X509 validator for a client conn.
    bool _installServerX509Validator(const X509List *client_CA_ta); // Setup X509 client cert validation, if supplied

    uint8_t *_streamLoad(Stream& stream, size_t size);
};

};

#endif
