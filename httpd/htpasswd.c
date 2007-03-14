/*
 *  Copyright(C) 2007 Cameron Rich
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ssl.h"

int tfd;

void base64_encode(const uint8_t *in, size_t inlen, char *out, size_t outlen)
{
    static const char b64str[64] =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    while (inlen && outlen)
    {
        *out++ = b64str[(in[0] >> 2) & 0x3f];
        if (!--outlen)
            break;

        *out++ = b64str[((in[0] << 4)
                + (--inlen ? in[1] >> 4 : 0)) & 0x3f];
        if (!--outlen)
            break;
        *out++ = (inlen
             ? b64str[((in[1] << 2)
                 + (--inlen ? in[2] >> 6 : 0))
             & 0x3f]
             : '=');
        if (!--outlen)
            break;
        *out++ = inlen ? b64str[in[2] & 0x3f] : '=';
        if (!--outlen)
            break;
        if (inlen)
            inlen--;
        if (inlen)
            in += 3;
    }

    if (outlen)
        *out = '\0';
}

static void usage(void) 
{
    fprintf(stderr,"Usage: htpasswd username\n");
    exit(1);
}

#ifdef WIN32
static char * getpass(const char *prompt)
{
    static char buf[127];
    FILE *fp = stdin;

    printf(prompt); TTY_FLUSH();
#if 0
    fp = fopen("/dev/tty", "w");
    if (fp == NULL) 
    {
        printf("null\n"); TTY_FLUSH();
        fp = stdin;
    }
#endif

    fgets(buf, sizeof(buf), fp);
    while (buf[strlen(buf)-1] < ' ') 
        buf[strlen(buf)-1] = '\0';

    //if (fp != stdin) 
    //    fclose(fp);
    return buf;
}
#endif

int main(int argc, char *argv[]) 
{
    char* pw;
    uint8_t md5_salt[MD5_SIZE], md5_pass[MD5_SIZE];
    char b64_salt[MD5_SIZE+10], b64_pass[MD5_SIZE+10];
    MD5_CTX ctx;

    if (argc != 2)
        usage();

    pw = strdup(getpass("New password:"));
    if (strcmp(pw, getpass("Re-type new password:")) != 0)
    {
        fprintf(stderr, "They don't match, sorry.\n" );
        exit(1);
    }

    RNG_initialize((uint8_t *)pw, sizeof(pw));
    get_random(MD5_SIZE, md5_salt);
    RNG_terminate();
    base64_encode(md5_salt, MD5_SIZE, b64_salt, sizeof(b64_salt));

    MD5Init(&ctx);
    MD5Update(&ctx, md5_salt, MD5_SIZE);
    MD5Update(&ctx, (uint8_t *)pw, strlen(pw));
    MD5Final(&ctx, md5_pass);
    base64_encode(md5_pass, MD5_SIZE, b64_pass, sizeof(b64_pass));

    printf("Add the following to your '.htpasswd' file\n");
    printf("%s:%s$%s\n", argv[1], b64_salt, b64_pass);
    return 0;
}
