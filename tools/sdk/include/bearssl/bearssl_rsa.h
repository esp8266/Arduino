/*
 * Copyright (c) 2016 Thomas Pornin <pornin@bolet.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining 
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be 
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef BR_BEARSSL_RSA_H__
#define BR_BEARSSL_RSA_H__

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \file bearssl_rsa.h
 *
 * # RSA
 *
 * This file documents the RSA implementations provided with BearSSL.
 * Note that the SSL engine accesses these implementations through a
 * configurable API, so it is possible to, for instance, run a SSL
 * server which uses a RSA engine which is not based on this code.
 *
 * ## Key Elements
 *
 * RSA public and private keys consist in lists of big integers. All
 * such integers are represented with big-endian unsigned notation:
 * first byte is the most significant, and the value is positive (so
 * there is no dedicated "sign bit"). Public and private key structures
 * thus contain, for each such integer, a pointer to the first value byte
 * (`unsigned char *`), and a length (`size_t`) which is the number of
 * relevant bytes. As a general rule, minimal-length encoding is not
 * enforced: values may have extra leading bytes of value 0.
 *
 * RSA public keys consist in two integers:
 *
 *   - the modulus (`n`);
 *   - the public exponent (`e`).
 *
 * RSA private keys, as defined in
 * [PKCS#1](https://tools.ietf.org/html/rfc3447), contain eight integers:
 *
 *   - the modulus (`n`);
 *   - the public exponent (`e`);
 *   - the private exponent (`d`);
 *   - the first prime factor (`p`);
 *   - the second prime factor (`q`);
 *   - the first reduced exponent (`dp`, which is `d` modulo `p-1`);
 *   - the second reduced exponent (`dq`, which is `d` modulo `q-1`);
 *   - the CRT coefficient (`iq`, the inverse of `q` modulo `p`).
 *
 * However, the implementations defined in BearSSL use only five of
 * these integers: `p`, `q`, `dp`, `dq` and `iq`.
 *
 * ## Security Features and Limitations
 *
 * The implementations contained in BearSSL have the following limitations
 * and features:
 *
 *   - They are constant-time. This means that the execution time and
 *     memory access pattern may depend on the _lengths_ of the private
 *     key components, but not on their value, nor on the value of
 *     the operand. Note that this property is not achieved through
 *     random masking, but "true" constant-time code.
 *
 *   - They support only private keys with two prime factors. RSA private
 *     key with three or more prime factors are nominally supported, but
 *     rarely used; they may offer faster operations, at the expense of
 *     more code and potentially a reduction in security if there are
 *     "too many" prime factors.
 *
 *   - The public exponent may have arbitrary length. Of course, it is
 *     a good idea to keep public exponents small, so that public key
 *     operations are fast; but, contrary to some widely deployed
 *     implementations, BearSSL has no problem with public exponent
 *     longer than 32 bits.
 *
 *   - The two prime factors of the modulus need not have the same length
 *     (but severely imbalanced factor lengths might reduce security).
 *     Similarly, there is no requirement that the first factor (`p`)
 *     be greater than the second factor (`q`).
 *
 *   - Prime factors and modulus must be smaller than a compile-time limit.
 *     This is made necessary by the use of fixed-size stack buffers, and
 *     the limit has been adjusted to keep stack usage under 2 kB for the
 *     RSA operations. Currently, the maximum modulus size is 4096 bits,
 *     and the maximum prime factor size is 2080 bits.
 *
 *   - The RSA functions themselves do not enforce lower size limits,
 *     except that which is absolutely necessary for the operation to
 *     mathematically make sense (e.g. a PKCS#1 v1.5 signature with
 *     SHA-1 requires a modulus of at least 361 bits). It is up to users
 *     of this code to enforce size limitations when appropriate (e.g.
 *     the X.509 validation engine, by default, rejects RSA keys of
 *     less than 1017 bits).
 *
 *   - Within the size constraints expressed above, arbitrary bit lengths
 *     are supported. There is no requirement that prime factors or
 *     modulus have a size multiple of 8 or 16.
 *
 *   - When verifying PKCS#1 v1.5 signatures, both variants of the hash
 *     function identifying header (with and without the ASN.1 NULL) are
 *     supported. When producing such signatures, the variant with the
 *     ASN.1 NULL is used.
 *
 * ## Implementations
 *
 * Three RSA implementations are included:
 *
 *   - The **i32** implementation internally represents big integers
 *     as arrays of 32-bit integers. It is perfunctory and portable,
 *     but not very efficient.
 *
 *   - The **i31** implementation uses 32-bit integers, each containing
 *     31 bits worth of integer data. The i31 implementation is somewhat
 *     faster than the i32 implementation (the reduced integer size makes
 *     carry propagation easier) for a similar code footprint, but uses
 *     very slightly larger stack buffers (about 4% bigger).
 *
 *   - The **i62** implementation is similar to the i31 implementation,
 *     except that it internally leverages the 64x64->128 multiplication
 *     opcode. This implementation is available only on architectures
 *     where such an opcode exists. It is much faster than i31.
 *
 *   - The **i15** implementation uses 16-bit integers, each containing
 *     15 bits worth of integer data. Multiplication results fit on
 *     32 bits, so this won't use the "widening" multiplication routine
 *     on ARM Cortex M0/M0+, for much better performance and constant-time
 *     execution.
 */

/**
 * \brief RSA public key.
 *
 * The structure references the modulus and the public exponent. Both
 * integers use unsigned big-endian representation; extra leading bytes
 * of value 0 are allowed.
 */
typedef struct {
	/** \brief Modulus. */
	unsigned char *n;
	/** \brief Modulus length (in bytes). */
	size_t nlen;
	/** \brief Public exponent. */
	unsigned char *e;
	/** \brief Public exponent length (in bytes). */
	size_t elen;
} br_rsa_public_key;

/**
 * \brief RSA private key.
 *
 * The structure references the primvate factors, reduced private
 * exponents, and CRT coefficient. It also contains the bit length of
 * the modulus. The big integers use unsigned big-endian representation;
 * extra leading bytes of value 0 are allowed. However, the modulus bit
 * length (`n_bitlen`) MUST be exact.
 */
typedef struct {
	/** \brief Modulus bit length (in bits, exact value). */
	uint32_t n_bitlen;
	/** \brief First prime factor. */
	unsigned char *p;
	/** \brief First prime factor length (in bytes). */
	size_t plen;
	/** \brief Second prime factor. */
	unsigned char *q;
	/** \brief Second prime factor length (in bytes). */
	size_t qlen;
	/** \brief First reduced private exponent. */
	unsigned char *dp;
	/** \brief First reduced private exponent length (in bytes). */
	size_t dplen;
	/** \brief Second reduced private exponent. */
	unsigned char *dq;
	/** \brief Second reduced private exponent length (in bytes). */
	size_t dqlen;
	/** \brief CRT coefficient. */
	unsigned char *iq;
	/** \brief CRT coefficient length (in bytes). */
	size_t iqlen;
} br_rsa_private_key;

/**
 * \brief Type for a RSA public key engine.
 *
 * The public key engine performs the modular exponentiation of the
 * provided value with the public exponent. The value is modified in
 * place.
 *
 * The value length (`xlen`) is verified to have _exactly_ the same
 * length as the modulus (actual modulus length, without extra leading
 * zeros in the modulus representation in memory). If the length does
 * not match, then this function returns 0 and `x[]` is unmodified.
 * 
 * It `xlen` is correct, then `x[]` is modified. Returned value is 1
 * on success, 0 on error. Error conditions include an oversized `x[]`
 * (the array has the same length as the modulus, but the numerical value
 * is not lower than the modulus) and an invalid modulus (e.g. an even
 * integer). If an error is reported, then the new contents of `x[]` are
 * unspecified.
 *
 * \param x      operand to exponentiate.
 * \param xlen   length of the operand (in bytes).
 * \param pk     RSA public key.
 * \return  1 on success, 0 on error.
 */
typedef uint32_t (*br_rsa_public)(unsigned char *x, size_t xlen,
	const br_rsa_public_key *pk);

/**
 * \brief Type for a RSA signature verification engine (PKCS#1 v1.5).
 *
 * Parameters are:
 *
 *   - The signature itself. The provided array is NOT modified.
 *
 *   - The encoded OID for the hash function. The provided array must begin
 *     with a single byte that contains the length of the OID value (in
 *     bytes), followed by exactly that many bytes. This parameter may
 *     also be `NULL`, in which case the raw hash value should be used
 *     with the PKCS#1 v1.5 "type 1" padding (as used in SSL/TLS up
 *     to TLS-1.1, with a 36-byte hash value).
 *
 *   - The hash output length, in bytes.
 *
 *   - The public key.
 *
 *   - An output buffer for the hash value. The caller must still compare
 *     it with the hash of the data over which the signature is computed.
 *
 * **Constraints:**
 *
 *   - Hash length MUST be no more than 64 bytes.
 *
 *   - OID value length MUST be no more than 32 bytes (i.e. `hash_oid[0]`
 *     must have a value in the 0..32 range, inclusive).
 *
 * This function verifies that the signature length (`xlen`) matches the
 * modulus length (this function returns 0 on mismatch). If the modulus
 * size exceeds the maximum supported RSA size, then the function also
 * returns 0.
 *
 * Returned value is 1 on success, 0 on error.
 *
 * Implementations of this type need not be constant-time.
 *
 * \param x          signature buffer.
 * \param xlen       signature length (in bytes).
 * \param hash_oid   encoded hash algorithm OID (or `NULL`).
 * \param hash_len   expected hash value length (in bytes).
 * \param pk         RSA public key.
 * \param hash_out   output buffer for the hash value.
 * \return  1 on success, 0 on error.
 */
typedef uint32_t (*br_rsa_pkcs1_vrfy)(const unsigned char *x, size_t xlen,
	const unsigned char *hash_oid, size_t hash_len,
	const br_rsa_public_key *pk, unsigned char *hash_out);

/**
 * \brief Type for a RSA private key engine.
 *
 * The `x[]` buffer is modified in place, and its length is inferred from
 * the modulus length (`x[]` is assumed to have a length of
 * `(sk->n_bitlen+7)/8` bytes).
 *
 * Returned value is 1 on success, 0 on error.
 *
 * \param x    operand to exponentiate.
 * \param sk   RSA private key.
 * \return  1 on success, 0 on error.
 */
typedef uint32_t (*br_rsa_private)(unsigned char *x,
	const br_rsa_private_key *sk);

/**
 * \brief Type for a RSA signature generation engine (PKCS#1 v1.5).
 *
 * Parameters are:
 *
 *   - The encoded OID for the hash function. The provided array must begin
 *     with a single byte that contains the length of the OID value (in
 *     bytes), followed by exactly that many bytes. This parameter may
 *     also be `NULL`, in which case the raw hash value should be used
 *     with the PKCS#1 v1.5 "type 1" padding (as used in SSL/TLS up
 *     to TLS-1.1, with a 36-byte hash value).
 *
 *   - The hash value computes over the data to sign (its length is
 *     expressed in bytes).
 *
 *   - The RSA private key.
 *
 *   - The output buffer, that receives the signature.
 *
 * Returned value is 1 on success, 0 on error. Error conditions include
 * a too small modulus for the provided hash OID and value, or some
 * invalid key parameters. The signature length is exactly
 * `(sk->n_bitlen+7)/8` bytes.
 *
 * This function is expected to be constant-time with regards to the
 * private key bytes (lengths of the modulus and the individual factors
 * may leak, though) and to the hashed data.
 *
 * \param hash_oid   encoded hash algorithm OID (or `NULL`).
 * \param hash       hash value.
 * \param hash_len   hash value length (in bytes).
 * \param sk         RSA private key.
 * \param x          output buffer for the signature value.
 * \return  1 on success, 0 on error.
 */
typedef uint32_t (*br_rsa_pkcs1_sign)(const unsigned char *hash_oid,
	const unsigned char *hash, size_t hash_len,
	const br_rsa_private_key *sk, unsigned char *x);

/**
 * \brief Encoded OID for SHA-1 (in RSA PKCS#1 signatures).
 */
#define BR_HASH_OID_SHA1     \
	((const unsigned char *)"\x05\x2B\x0E\x03\x02\x1A")

/**
 * \brief Encoded OID for SHA-224 (in RSA PKCS#1 signatures).
 */
#define BR_HASH_OID_SHA224   \
	((const unsigned char *)"\x09\x60\x86\x48\x01\x65\x03\x04\x02\x04")

/**
 * \brief Encoded OID for SHA-256 (in RSA PKCS#1 signatures).
 */
#define BR_HASH_OID_SHA256   \
	((const unsigned char *)"\x09\x60\x86\x48\x01\x65\x03\x04\x02\x01")

/**
 * \brief Encoded OID for SHA-384 (in RSA PKCS#1 signatures).
 */
#define BR_HASH_OID_SHA384   \
	((const unsigned char *)"\x09\x60\x86\x48\x01\x65\x03\x04\x02\x02")

/**
 * \brief Encoded OID for SHA-512 (in RSA PKCS#1 signatures).
 */
#define BR_HASH_OID_SHA512   \
	((const unsigned char *)"\x09\x60\x86\x48\x01\x65\x03\x04\x02\x03")

/*
 * RSA "i32" engine. Integers are internally represented as arrays of
 * 32-bit integers, and the core multiplication primitive is the
 * 32x32->64 multiplication.
 */

/**
 * \brief RSA public key engine "i32".
 *
 * \see br_rsa_public
 *
 * \param x      operand to exponentiate.
 * \param xlen   length of the operand (in bytes).
 * \param pk     RSA public key.
 * \return  1 on success, 0 on error.
 */
uint32_t br_rsa_i32_public(unsigned char *x, size_t xlen,
	const br_rsa_public_key *pk);

/**
 * \brief RSA signature verification engine "i32".
 *
 * \see br_rsa_pkcs1_vrfy
 *
 * \param x          signature buffer.
 * \param xlen       signature length (in bytes).
 * \param hash_oid   encoded hash algorithm OID (or `NULL`).
 * \param hash_len   expected hash value length (in bytes).
 * \param pk         RSA public key.
 * \param hash_out   output buffer for the hash value.
 * \return  1 on success, 0 on error.
 */
uint32_t br_rsa_i32_pkcs1_vrfy(const unsigned char *x, size_t xlen,
	const unsigned char *hash_oid, size_t hash_len,
	const br_rsa_public_key *pk, unsigned char *hash_out);

/**
 * \brief RSA private key engine "i32".
 *
 * \see br_rsa_private
 *
 * \param x    operand to exponentiate.
 * \param sk   RSA private key.
 * \return  1 on success, 0 on error.
 */
uint32_t br_rsa_i32_private(unsigned char *x,
	const br_rsa_private_key *sk);

/**
 * \brief RSA signature generation engine "i32".
 *
 * \see br_rsa_pkcs1_sign
 *
 * \param hash_oid   encoded hash algorithm OID (or `NULL`).
 * \param hash       hash value.
 * \param hash_len   hash value length (in bytes).
 * \param sk         RSA private key.
 * \param x          output buffer for the hash value.
 * \return  1 on success, 0 on error.
 */
uint32_t br_rsa_i32_pkcs1_sign(const unsigned char *hash_oid,
	const unsigned char *hash, size_t hash_len,
	const br_rsa_private_key *sk, unsigned char *x);

/*
 * RSA "i31" engine. Similar to i32, but only 31 bits are used per 32-bit
 * word. This uses slightly more stack space (about 4% more) and code
 * space, but it quite faster.
 */

/**
 * \brief RSA public key engine "i31".
 *
 * \see br_rsa_public
 *
 * \param x      operand to exponentiate.
 * \param xlen   length of the operand (in bytes).
 * \param pk     RSA public key.
 * \return  1 on success, 0 on error.
 */
uint32_t br_rsa_i31_public(unsigned char *x, size_t xlen,
	const br_rsa_public_key *pk);

/**
 * \brief RSA signature verification engine "i31".
 *
 * \see br_rsa_pkcs1_vrfy
 *
 * \param x          signature buffer.
 * \param xlen       signature length (in bytes).
 * \param hash_oid   encoded hash algorithm OID (or `NULL`).
 * \param hash_len   expected hash value length (in bytes).
 * \param pk         RSA public key.
 * \param hash_out   output buffer for the hash value.
 * \return  1 on success, 0 on error.
 */
uint32_t br_rsa_i31_pkcs1_vrfy(const unsigned char *x, size_t xlen,
	const unsigned char *hash_oid, size_t hash_len,
	const br_rsa_public_key *pk, unsigned char *hash_out);

/**
 * \brief RSA private key engine "i31".
 *
 * \see br_rsa_private
 *
 * \param x    operand to exponentiate.
 * \param sk   RSA private key.
 * \return  1 on success, 0 on error.
 */
uint32_t br_rsa_i31_private(unsigned char *x,
	const br_rsa_private_key *sk);

/**
 * \brief RSA signature generation engine "i31".
 *
 * \see br_rsa_pkcs1_sign
 *
 * \param hash_oid   encoded hash algorithm OID (or `NULL`).
 * \param hash       hash value.
 * \param hash_len   hash value length (in bytes).
 * \param sk         RSA private key.
 * \param x          output buffer for the hash value.
 * \return  1 on success, 0 on error.
 */
uint32_t br_rsa_i31_pkcs1_sign(const unsigned char *hash_oid,
	const unsigned char *hash, size_t hash_len,
	const br_rsa_private_key *sk, unsigned char *x);

/*
 * RSA "i62" engine. Similar to i31, but internal multiplication use
 * 64x64->128 multiplications. This is available only on architecture
 * that offer such an opcode.
 */

/**
 * \brief RSA public key engine "i62".
 *
 * This function is defined only on architecture that offer a 64x64->128
 * opcode. Use `br_rsa_i62_public_get()` to dynamically obtain a pointer
 * to that functiom.
 *
 * \see br_rsa_public
 *
 * \param x      operand to exponentiate.
 * \param xlen   length of the operand (in bytes).
 * \param pk     RSA public key.
 * \return  1 on success, 0 on error.
 */
uint32_t br_rsa_i62_public(unsigned char *x, size_t xlen,
	const br_rsa_public_key *pk);

/**
 * \brief RSA signature verification engine "i62".
 *
 * This function is defined only on architecture that offer a 64x64->128
 * opcode. Use `br_rsa_i62_pkcs1_vrfy_get()` to dynamically obtain a pointer
 * to that functiom.
 *
 * \see br_rsa_pkcs1_vrfy
 *
 * \param x          signature buffer.
 * \param xlen       signature length (in bytes).
 * \param hash_oid   encoded hash algorithm OID (or `NULL`).
 * \param hash_len   expected hash value length (in bytes).
 * \param pk         RSA public key.
 * \param hash_out   output buffer for the hash value.
 * \return  1 on success, 0 on error.
 */
uint32_t br_rsa_i62_pkcs1_vrfy(const unsigned char *x, size_t xlen,
	const unsigned char *hash_oid, size_t hash_len,
	const br_rsa_public_key *pk, unsigned char *hash_out);

/**
 * \brief RSA private key engine "i62".
 *
 * This function is defined only on architecture that offer a 64x64->128
 * opcode. Use `br_rsa_i62_private_get()` to dynamically obtain a pointer
 * to that functiom.
 *
 * \see br_rsa_private
 *
 * \param x    operand to exponentiate.
 * \param sk   RSA private key.
 * \return  1 on success, 0 on error.
 */
uint32_t br_rsa_i62_private(unsigned char *x,
	const br_rsa_private_key *sk);

/**
 * \brief RSA signature generation engine "i62".
 *
 * This function is defined only on architecture that offer a 64x64->128
 * opcode. Use `br_rsa_i62_pkcs1_sign_get()` to dynamically obtain a pointer
 * to that functiom.
 *
 * \see br_rsa_pkcs1_sign
 *
 * \param hash_oid   encoded hash algorithm OID (or `NULL`).
 * \param hash       hash value.
 * \param hash_len   hash value length (in bytes).
 * \param sk         RSA private key.
 * \param x          output buffer for the hash value.
 * \return  1 on success, 0 on error.
 */
uint32_t br_rsa_i62_pkcs1_sign(const unsigned char *hash_oid,
	const unsigned char *hash, size_t hash_len,
	const br_rsa_private_key *sk, unsigned char *x);

/**
 * \brief Get the RSA "i62" implementation (public key operations),
 * if available.
 *
 * \return  the implementation, or 0.
 */
br_rsa_public br_rsa_i62_public_get(void);

/**
 * \brief Get the RSA "i62" implementation (PKCS#1 signature verification),
 * if available.
 *
 * \return  the implementation, or 0.
 */
br_rsa_pkcs1_vrfy br_rsa_i62_pkcs1_vrfy_get(void);

/**
 * \brief Get the RSA "i62" implementation (private key operations),
 * if available.
 *
 * \return  the implementation, or 0.
 */
br_rsa_private br_rsa_i62_private_get(void);

/**
 * \brief Get the RSA "i62" implementation (PKCS#1 signature generation),
 * if available.
 *
 * \return  the implementation, or 0.
 */
br_rsa_pkcs1_sign br_rsa_i62_pkcs1_sign_get(void);

/*
 * RSA "i15" engine. Integers are represented as 15-bit integers, so
 * the code uses only 32-bit multiplication (no 64-bit result), which
 * is vastly faster (and constant-time) on the ARM Cortex M0/M0+.
 */

/**
 * \brief RSA public key engine "i15".
 *
 * \see br_rsa_public
 *
 * \param x      operand to exponentiate.
 * \param xlen   length of the operand (in bytes).
 * \param pk     RSA public key.
 * \return  1 on success, 0 on error.
 */
uint32_t br_rsa_i15_public(unsigned char *x, size_t xlen,
	const br_rsa_public_key *pk);

/**
 * \brief RSA signature verification engine "i15".
 *
 * \see br_rsa_pkcs1_vrfy
 *
 * \param x          signature buffer.
 * \param xlen       signature length (in bytes).
 * \param hash_oid   encoded hash algorithm OID (or `NULL`).
 * \param hash_len   expected hash value length (in bytes).
 * \param pk         RSA public key.
 * \param hash_out   output buffer for the hash value.
 * \return  1 on success, 0 on error.
 */
uint32_t br_rsa_i15_pkcs1_vrfy(const unsigned char *x, size_t xlen,
	const unsigned char *hash_oid, size_t hash_len,
	const br_rsa_public_key *pk, unsigned char *hash_out);

/**
 * \brief RSA private key engine "i15".
 *
 * \see br_rsa_private
 *
 * \param x    operand to exponentiate.
 * \param sk   RSA private key.
 * \return  1 on success, 0 on error.
 */
uint32_t br_rsa_i15_private(unsigned char *x,
	const br_rsa_private_key *sk);

/**
 * \brief RSA signature generation engine "i15".
 *
 * \see br_rsa_pkcs1_sign
 *
 * \param hash_oid   encoded hash algorithm OID (or `NULL`).
 * \param hash       hash value.
 * \param hash_len   hash value length (in bytes).
 * \param sk         RSA private key.
 * \param x          output buffer for the hash value.
 * \return  1 on success, 0 on error.
 */
uint32_t br_rsa_i15_pkcs1_sign(const unsigned char *hash_oid,
	const unsigned char *hash, size_t hash_len,
	const br_rsa_private_key *sk, unsigned char *x);

/**
 * \brief Get "default" RSA implementation (public-key operations).
 *
 * This returns the preferred implementation of RSA (public-key operations)
 * on the current system.
 *
 * \return  the default implementation.
 */
br_rsa_public br_rsa_public_get_default(void);

/**
 * \brief Get "default" RSA implementation (private-key operations).
 *
 * This returns the preferred implementation of RSA (private-key operations)
 * on the current system.
 *
 * \return  the default implementation.
 */
br_rsa_private br_rsa_private_get_default(void);

/**
 * \brief Get "default" RSA implementation (PKCS#1 signature verification).
 *
 * This returns the preferred implementation of RSA (signature verification)
 * on the current system.
 *
 * \return  the default implementation.
 */
br_rsa_pkcs1_vrfy br_rsa_pkcs1_vrfy_get_default(void);

/**
 * \brief Get "default" RSA implementation (PKCS#1 signature generation).
 *
 * This returns the preferred implementation of RSA (signature generation)
 * on the current system.
 *
 * \return  the default implementation.
 */
br_rsa_pkcs1_sign br_rsa_pkcs1_sign_get_default(void);

/**
 * \brief RSA decryption helper, for SSL/TLS.
 *
 * This function performs the RSA decryption for a RSA-based key exchange
 * in a SSL/TLS server. The provided RSA engine is used. The `data`
 * parameter points to the value to decrypt, of length `len` bytes. On
 * success, the 48-byte pre-master secret is copied into `data`, starting
 * at the first byte of that buffer; on error, the contents of `data`
 * become indeterminate.
 *
 * This function first checks that the provided value length (`len`) is
 * not lower than 59 bytes, and matches the RSA modulus length; if neither
 * of this property is met, then this function returns 0 and the buffer
 * is unmodified.
 *
 * Otherwise, decryption and then padding verification are performed, both
 * in constant-time. A decryption error, or a bad padding, or an
 * incorrect decrypted value length are reported with a returned value of
 * 0; on success, 1 is returned. The caller (SSL server engine) is supposed
 * to proceed with a random pre-master secret in case of error.
 *
 * \param core   RSA private key engine.
 * \param sk     RSA private key.
 * \param data   input/output buffer.
 * \param len    length (in bytes) of the data to decrypt.
 * \return  1 on success, 0 on error.
 */
uint32_t br_rsa_ssl_decrypt(br_rsa_private core, const br_rsa_private_key *sk,
	unsigned char *data, size_t len);

#ifdef __cplusplus
}
#endif

#endif
