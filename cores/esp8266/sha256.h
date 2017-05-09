#ifndef __ESP8266_SHA256__
#define __ESP8266_SHA256__

#ifdef __cplusplus
extern "C" {
#endif

#define SHA256_SIZE   32

typedef struct
{
    uint32_t total[2];
    uint32_t state[8];
    uint8_t buffer[64];
} SHA256_CTX;

extern void SHA256_Init(SHA256_CTX *ctx);
extern void SHA256_Update(SHA256_CTX *ctx, const uint8_t * msg, const int len);
extern void SHA256_Final(uint8_t *digest, SHA256_CTX *ctx);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
