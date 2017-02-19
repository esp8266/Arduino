/*
 * Copyright (c) 2007-2016, Cameron Rich
 * 
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * * Neither the name of the axTLS project nor the names of its contributors
 *   may be used to endorse or promote products derived from this software
 *   without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file os_port.h
 *
 * Some stuff to minimise the differences between windows and linux/unix
 */

#ifndef HEADER_OS_PORT_H
#define HEADER_OS_PORT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "os_int.h"
#include "config.h"
#include <stdio.h>

#ifdef WIN32
#define STDCALL                 __stdcall
#define EXP_FUNC                __declspec(dllexport)
#else
#define STDCALL
#define EXP_FUNC
#endif

#if defined(_WIN32_WCE)
#undef WIN32
#define WIN32
#endif

#if defined(ESP8266)

#include "util/time.h"
#include <errno.h>
#define alloca(size) __builtin_alloca(size)
#define TTY_FLUSH()
#ifdef putc
#undef putc
#endif
#define putc(x, f)   ets_putc(x)
#ifdef printf
#undef printf
#endif
#define printf(...)  ets_printf(__VA_ARGS__)

#define SOCKET_READ(A,B,C)      ax_port_read(A,B,C)
#define SOCKET_WRITE(A,B,C)     ax_port_write(A,B,C)
#define SOCKET_CLOSE(A)         ax_port_close(A)
#define get_file                ax_get_file
#define EWOULDBLOCK EAGAIN

#define hmac_sha1 ax_hmac_sha1
#define hmac_sha256 ax_hmac_sha256
#define hmac_md5 ax_hmac_md5

#ifndef be64toh
# define __bswap_constant_64(x) \
     ((((x) & 0xff00000000000000ull) >> 56)                                   \
      | (((x) & 0x00ff000000000000ull) >> 40)                                 \
      | (((x) & 0x0000ff0000000000ull) >> 24)                                 \
      | (((x) & 0x000000ff00000000ull) >> 8)                                  \
      | (((x) & 0x00000000ff000000ull) << 8)                                  \
      | (((x) & 0x0000000000ff0000ull) << 24)                                 \
      | (((x) & 0x000000000000ff00ull) << 40)                                 \
      | (((x) & 0x00000000000000ffull) << 56))
#define be64toh(x) __bswap_constant_64(x)
#endif

void ax_wdt_feed();

#ifndef PROGMEM
#define PROGMEM __attribute__((aligned(4))) __attribute__((section(".irom.text")))
#endif

#ifndef WITH_PGM_READ_HELPER
#define ax_array_read_u8(x, y) x[y]
#else

static inline uint8_t pgm_read_byte(const void* addr) {
  register uint32_t res;
  __asm__("extui    %0, %1, 0, 2\n"     /* Extract offset within word (in bytes) */
      "sub      %1, %1, %0\n"       /* Subtract offset from addr, yielding an aligned address */
      "l32i.n   %1, %1, 0x0\n"      /* Load word from aligned address */
      "slli     %0, %0, 3\n"        /* Multiply offset by 8, yielding an offset in bits */
      "ssr      %0\n"               /* Prepare to shift by offset (in bits) */
      "srl      %0, %1\n"           /* Shift right; now the requested byte is the first one */
      :"=r"(res), "=r"(addr)
      :"1"(addr)
      :);
  return (uint8_t) res;     /* This masks the lower byte from the returned word */
}

#define ax_array_read_u8(x, y) pgm_read_byte((x)+(y))
#endif //WITH_PGM_READ_HELPER

#elif defined(WIN32)

/* Windows CE stuff */
#if defined(_WIN32_WCE)
#include <basetsd.h>
#define abort()                 exit(1)
#else
#include <io.h>
#include <process.h>
#include <sys/timeb.h>
#include <fcntl.h>
#endif      /* _WIN32_WCE */

#include <winsock.h>
#include <direct.h>
#undef getpid
#undef open
#undef close
#undef sleep
#undef gettimeofday
#undef dup2
#undef unlink

#define SOCKET_READ(A,B,C)      recv(A,B,C,0)
#define SOCKET_WRITE(A,B,C)     send(A,B,C,0)
#define SOCKET_CLOSE(A)         closesocket(A)
#define srandom(A)              srand(A)
#define random()                rand()
#define getpid()                _getpid()
#define snprintf                _snprintf
#define open(A,B)               _open(A,B)
#define dup2(A,B)               _dup2(A,B)
#define unlink(A)               _unlink(A)
#define close(A)                _close(A)
#define read(A,B,C)             _read(A,B,C)
#define write(A,B,C)            _write(A,B,C)
#define sleep(A)                Sleep(A*1000)
#define usleep(A)               Sleep(A/1000)
#define strdup(A)               _strdup(A)
#define chroot(A)               _chdir(A)
#define chdir(A)                _chdir(A)
#define alloca(A)               _alloca(A)
#ifndef lseek
#define lseek(A,B,C)            _lseek(A,B,C)
#endif

/* This fix gets around a problem where a win32 application on a cygwin xterm
   doesn't display regular output (until a certain buffer limit) - but it works
   fine under a normal DOS window. This is a hack to get around the issue -
   see http://www.khngai.com/emacs/tty.php  */
#define TTY_FLUSH()             if (!_isatty(_fileno(stdout))) fflush(stdout);

/*
 * automatically build some library dependencies.
 */
#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "AdvAPI32.lib")

typedef int socklen_t;

EXP_FUNC void STDCALL gettimeofday(struct timeval* t,void* timezone);
EXP_FUNC int STDCALL strcasecmp(const char *s1, const char *s2);
EXP_FUNC int STDCALL getdomainname(char *buf, int buf_size);

#else   /* Not Win32 */

#include <unistd.h>
#include <pwd.h>
#include <netdb.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <asm/byteorder.h>

#define SOCKET_READ(A,B,C)      read(A,B,C)
#define SOCKET_WRITE(A,B,C)     write(A,B,C)
#define SOCKET_CLOSE(A)         if (A >= 0) close(A)
#define TTY_FLUSH()

#ifndef be64toh
#define be64toh(x) __be64_to_cpu(x)
#endif

#endif  /* Not Win32 */

/* some functions to mutate the way these work */
inline uint32_t htonl(uint32_t n){
  return ((n & 0xff) << 24) |
    ((n & 0xff00) << 8) |
    ((n & 0xff0000UL) >> 8) |
    ((n & 0xff000000UL) >> 24);
}

#define ntohl htonl

EXP_FUNC int STDCALL ax_open(const char *pathname, int flags); 

#ifdef CONFIG_PLATFORM_LINUX
void exit_now(const char *format, ...) __attribute((noreturn));
#else
void exit_now(const char *format, ...);
#endif

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

#ifndef PROGMEM
#define PROGMEM
#endif

#ifdef __cplusplus
}
#endif

#endif
