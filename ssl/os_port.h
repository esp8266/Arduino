/*
 *  Copyright(C) 2006
 *
 *  This library is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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

#if defined(WIN32) || defined(CYGWIN)
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

#ifdef WIN32

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
#define lseek(A,B,C)            _lseek(A,B,C)
#define strdup(A)               _strdup(A)

/* This fix gets around a problem where a win32 application on a cygwin xterm
   doesn't display regular output (until a certain buffer limit) - but it works
   fine under a normal DOS window. This is a hack to get around the issue - 
   see http://www.khngai.com/emacs/tty.php  */
#define TTY_FLUSH()             if (!_isatty(_fileno(stdout))) fflush(stdout);

/*
 * automatically build some library dependencies.
 */
#pragma comment(lib, "WS2_32.lib")

#ifdef CONFIG_WIN32_USE_CRYPTO_LIB
#pragma comment(lib, "AdvAPI32.lib")
#endif

#define uint8_t unsigned char
#define uint16_t unsigned short
#ifndef INT16
typedef signed short INT16;
#endif

#define int16_t INT16
#define uint32_t UINT32
#define uint64_t UINT64
#define int64_t INT64

extern EXP_FUNC void gettimeofday(struct timeval* t,void* timezone);
extern EXP_FUNC int strcasecmp(const char *s1, const char *s2);

#else   /* Not Win32 */

#ifdef SOLARIS
#include <inttypes.h>
#else
#include <stdint.h>
#endif /* Not Solaris */

#include <unistd.h>
#include <pwd.h>
#include <netdb.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SOCKET_READ(A,B,C)      read(A,B,C)
#define SOCKET_WRITE(A,B,C)     write(A,B,C)
#define SOCKET_CLOSE(A)         close(A)
#define TTY_FLUSH()

#endif  /* Not Win32 */

#ifdef __cplusplus
}
#endif

#endif 
