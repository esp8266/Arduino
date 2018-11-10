
// include host's STL before any other include file
// because core definition like max() is in the way

#ifdef __cplusplus
#include <vector>
#endif

// exotic typedefs used in the sdk

#include <stdint.h>
typedef uint8_t uint8;
typedef uint32_t uint32;

//

#include <Arduino.h>

#define lwip_htons htons
#define lwip_htonl htonl

#ifdef __cplusplus
#ifndef CCBUFSIZE
#define CCBUFSIZE 512
#endif
int    mockConnect   (uint32_t addr, int& sock, int port);
size_t mockFillInBuf (int sock, char* ccinbuf, size_t& ccinbufsize);
size_t mockPeekBytes (int sock, char* dst, size_t size, int timeout_ms, char* buf, size_t& bufsize);
size_t mockRead      (int sock, char* dst, size_t size, int timeout_ms, char* buf, size_t& bufsize);
size_t mockWrite     (int sock, const uint8_t* data, size_t size, int timeout_ms);
#endif
