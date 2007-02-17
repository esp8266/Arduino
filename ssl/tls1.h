/*
 *  Copyright(C) 2006 Cameron Rich
 *
 *  This library is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/**
 * @file tls1.h
 *
 * @brief The definitions for the TLS library.
 */
#ifndef HEADER_SSL_LIB_H
#define HEADER_SSL_LIB_H

#ifdef __cplusplus
extern "C" {
#endif

#include "version.h"

/* Mutexing definitions */
#if defined(CONFIG_SSL_CTX_MUTEXING)
#if defined(WIN32)
#define SSL_CTX_MUTEX_TYPE          HANDLE
#define SSL_CTX_MUTEX_INIT(A)       A=CreateMutex(0, FALSE, 0)
#define SSL_CTX_MUTEX_DESTROY(A)    CloseHandle(A)
#define SSL_CTX_LOCK(A)             WaitForSingleObject(A, INFINITE)
#define SSL_CTX_UNLOCK(A)           ReleaseMutex(A)
#else 
#include <pthread.h>
#define SSL_CTX_MUTEX_TYPE          pthread_mutex_t
#define SSL_CTX_MUTEX_INIT(A)       pthread_mutex_init(&A, NULL)
#define SSL_CTX_MUTEX_DESTROY(A)    pthread_mutex_destroy(&A)
#define SSL_CTX_LOCK(A)             pthread_mutex_lock(&A)
#define SSL_CTX_UNLOCK(A)           pthread_mutex_unlock(&A)
#endif
#else   /* no mutexing */
#define SSL_CTX_MUTEX_INIT(A)
#define SSL_CTX_MUTEX_DESTROY(A)
#define SSL_CTX_LOCK(A)
#define SSL_CTX_UNLOCK(A)
#endif

#define SSL_RANDOM_SIZE             32
#define SSL_SECRET_SIZE             48
#define SSL_FINISHED_HASH_SIZE      12
#define SSL_RECORD_SIZE             5
#define SSL_SERVER_READ             0
#define SSL_SERVER_WRITE            1
#define SSL_CLIENT_READ             2
#define SSL_CLIENT_WRITE            3
#define SSL_HS_HDR_SIZE             4

/* the flags we use while establishing a connection */
#define SSL_NEED_RECORD             0x0001
#define SSL_TX_ENCRYPTED            0x0002 
#define SSL_RX_ENCRYPTED            0x0004
#define SSL_SESSION_RESUME          0x0008
#define SSL_IS_CLIENT               0x0010
#define SSL_HAS_CERT_REQ            0x0020

/* some macros to muck around with flag bits */
#define SET_SSL_FLAG(A)             (ssl->flag |= A)
#define CLR_SSL_FLAG(A)             (ssl->flag &= ~A)
#define IS_SET_SSL_FLAG(A)          (ssl->flag & A)

#define MAX_KEY_BYTE_SIZE           512     /* for a 4096 bit key */
#define RT_MAX_PLAIN_LENGTH         16384
#define RT_EXTRA                    1024
#define BM_RECORD_OFFSET            5

#ifdef CONFIG_SSL_SKELETON_MODE
#define NUM_PROTOCOLS               1
#else
#define NUM_PROTOCOLS               4
#endif

#define PARANOIA_CHECK(A, B)        if (A < B) { \
    ret = SSL_ERROR_INVALID_HANDSHAKE; goto error; }

/* protocol types */
enum
{
    PT_CHANGE_CIPHER_SPEC = 20,
    PT_ALERT_PROTOCOL,
    PT_HANDSHAKE_PROTOCOL,
    PT_APP_PROTOCOL_DATA
};

/* handshaking types */
enum
{
    HS_HELLO_REQUEST,
    HS_CLIENT_HELLO,
    HS_SERVER_HELLO,
    HS_CERTIFICATE = 11,
    HS_SERVER_KEY_XCHG,
    HS_CERT_REQ,
    HS_SERVER_HELLO_DONE,
    HS_CERT_VERIFY,
    HS_CLIENT_KEY_XCHG,
    HS_FINISHED = 20
};

typedef struct 
{
    uint8_t cipher;
    uint8_t key_size;
    uint8_t iv_size;
    uint8_t key_block_size;
    uint8_t padding_size;
    uint8_t digest_size;
    hmac_func hmac;
    crypt_func encrypt;
    crypt_func decrypt;
} cipher_info_t;

struct _SSLObjLoader 
{
    uint8_t *buf;
    int len;
};

typedef struct _SSLObjLoader SSLObjLoader;

typedef struct 
{
    time_t conn_time;
    uint8_t session_id[SSL_SESSION_ID_SIZE];
    uint8_t master_secret[SSL_SECRET_SIZE];
} SSL_SESS;

typedef struct
{
    uint8_t *buf;
    int size;
} SSL_CERT;

struct _SSL
{
    uint32_t flag;
    uint16_t need_bytes;
    uint16_t got_bytes;
    uint8_t record_type;
    uint8_t chain_length;
    uint8_t cipher;
    int16_t next_state;
    int16_t hs_status;
    uint8_t *all_pkts; 
    int all_pkts_len;
    int client_fd;
    const cipher_info_t *cipher_info;
    uint8_t *final_finish_mac;
    uint8_t *key_block;
    void *encrypt_ctx;
    void *decrypt_ctx;
    uint8_t bm_all_data[RT_MAX_PLAIN_LENGTH+RT_EXTRA];
    uint8_t *bm_data;
    int bm_index;
    struct _SSL *next;                  /* doubly linked list */
    struct _SSL *prev;
    SSL_CERT *certs;
    struct _SSL_CTX *ssl_ctx;            /* back reference to a clnt/svr ctx */
#ifndef CONFIG_SSL_SKELETON_MODE
    uint16_t session_index;
    SSL_SESS *session;
#endif
#ifdef CONFIG_SSL_CERT_VERIFICATION
    X509_CTX *x509_ctx;
#endif

    uint8_t session_id[SSL_SESSION_ID_SIZE]; 
    uint8_t client_mac[SHA1_SIZE];  /* for HMAC verification */
    uint8_t server_mac[SHA1_SIZE];  /* for HMAC verification */
    uint8_t client_random[SSL_RANDOM_SIZE]; /* client's random sequence */
    uint8_t server_random[SSL_RANDOM_SIZE]; /* server's random sequence */
    uint8_t *master_secret;
    uint8_t read_sequence[8];       /* 64 bit sequence number */
    uint8_t write_sequence[8];      /* 64 bit sequence number */
    uint8_t record_buf[SSL_RECORD_SIZE];    /* storage for hmac calls later */
};

typedef struct _SSL SSL;

struct _SSL_CTX
{
    uint32_t options;
    uint8_t chain_length;
    RSA_CTX *rsa_ctx;
#ifdef CONFIG_SSL_CERT_VERIFICATION
    CA_CERT_CTX *ca_cert_ctx;
#endif
    SSL *head;
    SSL *tail;
    SSL_CERT certs[CONFIG_SSL_MAX_CERTS];
#ifndef CONFIG_SSL_SKELETON_MODE
    uint16_t num_sessions;
    SSL_SESS **ssl_sessions;
#endif
#ifdef CONFIG_SSL_CTX_MUTEXING
    SSL_CTX_MUTEX_TYPE mutex;
#endif
};

typedef struct _SSL_CTX SSL_CTX;

/* backwards compatibility */
typedef struct _SSL_CTX SSLCTX;

extern const uint8_t ssl_prot_prefs[NUM_PROTOCOLS];

SSL *ssl_new(SSL_CTX *ssl_ctx, int client_fd);
int send_packet(SSL *ssl, uint8_t protocol, 
        const uint8_t *in, int length);
int do_svr_handshake(SSL *ssl, int handshake_type, uint8_t *buf, int hs_len);
int do_clnt_handshake(SSL *ssl, int handshake_type, uint8_t *buf, int hs_len);
int process_finished(SSL *ssl, int hs_len);
int process_sslv23_client_hello(SSL *ssl);
int send_alert(SSL *ssl, int error_code);
int send_finished(SSL *ssl);
int send_certificate(SSL *ssl);
int basic_read(SSL *ssl, uint8_t **in_data);
int send_change_cipher_spec(SSL *ssl);
void finished_digest(SSL *ssl, const char *label, uint8_t *digest);
void generate_master_secret(SSL *ssl, const uint8_t *premaster_secret);
void add_packet(SSL *ssl, const uint8_t *pkt, int len);
int add_cert(SSL_CTX *ssl_ctx, const uint8_t *buf, int len);
int add_private_key(SSL_CTX *ssl_ctx, SSLObjLoader *ssl_obj);
void ssl_obj_free(SSLObjLoader *ssl_obj);
int pkcs8_decode(SSL_CTX *ssl_ctx, SSLObjLoader *ssl_obj, const char *password);
int pkcs12_decode(SSL_CTX *ssl_ctx, SSLObjLoader *ssl_obj, const char *password);
#ifdef CONFIG_SSL_CERT_VERIFICATION
int add_cert_auth(SSL_CTX *ssl_ctx, const uint8_t *buf, int len);
void remove_ca_certs(CA_CERT_CTX *ca_cert_ctx);
#endif
#ifdef CONFIG_SSL_ENABLE_CLIENT
int do_client_connect(SSL *ssl);
#endif

#ifdef CONFIG_SSL_FULL_MODE
void DISPLAY_STATE(SSL *ssl, int is_send, uint8_t state, int not_ok);
void DISPLAY_BYTES(SSL *ssl, const char *format, 
        const uint8_t *data, int size, ...);
void DISPLAY_CERT(SSL *ssl, const char *label, const X509_CTX *x509_ctx);
void DISPLAY_RSA(SSL *ssl, const char *label, const RSA_CTX *rsa_ctx);
void DISPLAY_ALERT(SSL *ssl, int alert);
#else
#define DISPLAY_STATE(A,B,C,D)
#define DISPLAY_CERT(A,B,C)
#define DISPLAY_RSA(A,B,C)
#define DISPLAY_ALERT(A, B)
#ifdef WIN32
void DISPLAY_BYTES(SSL *ssl, const char *format,/* win32 has no variadic macros */
        const uint8_t *data, int size, ...);
#else
#define DISPLAY_BYTES(A,B,C,D,...)
#endif
#endif

#ifdef CONFIG_SSL_CERT_VERIFICATION
int process_certificate(SSL *ssl, X509_CTX **x509_ctx);
#endif

SSL_SESS *ssl_session_update(int max_sessions, 
        SSL_SESS *ssl_sessions[], SSL *ssl,
        const uint8_t *session_id);
void kill_ssl_session(SSL_SESS **ssl_sessions, SSL *ssl);

#ifdef __cplusplus
}
#endif

#endif 
