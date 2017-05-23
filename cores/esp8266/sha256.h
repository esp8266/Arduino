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
} sha256_context_t;

extern void SHA256_Init(sha256_context_t *ctx);
extern void SHA256_Update(sha256_context_t *ctx, const uint8_t * msg, const int len);
extern void SHA256_Final(uint8_t *digest, sha256_context_t *ctx);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
