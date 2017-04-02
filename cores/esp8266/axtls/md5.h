#ifndef HEADER_MD5_H
#define HEADER_MD5_H

#ifdef __cplusplus
extern "C" {
#endif

#define MD5_SIZE    16

typedef struct
{
  uint32_t state[4];        /* state (ABCD) */
  uint32_t count[2];        /* number of bits, modulo 2^64 (lsb first) */
  uint8_t buffer[64];       /* input buffer */
} MD5_CTX;

extern void MD5_Init(MD5_CTX *);
extern void MD5_Update(MD5_CTX *, const uint8_t *msg, int len);
extern void MD5_Final(uint8_t *digest, MD5_CTX *);

#ifdef __cplusplus
} // extern "C"
#endif

#endif