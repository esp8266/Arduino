/*
 *  Copyright(C) 2006 Cameron Rich
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
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "axhttp.h"

void nada(int sigtype) { }

void die(int sigtype) 
{
    exit(0);
}

#if defined(CONFIG_HTTP_HAS_CGI)
#ifndef WIN32
void reaper(int sigtype) 
{
    wait3(NULL,WNOHANG,NULL);
}
#endif
#endif

void stripcrlf(char *p) 
{
    while (p && *p) 
    {
        if (*p=='\n' || *p=='\r') 
        {
            *p='\0';
            return;
        }
        p++;
    }
}

/* Wrapper function for strncpy() that guarantees
   a null-terminated string. This is to avoid any possible
   issues due to strncpy()'s behaviour. Thanks to
   Werner Almesberger for pointing out this potential
   issue. Needless to say, make sure sizeof(dest) > 0
   and sizeof(dest) >= n.
 */
char *my_strncpy(char *dest, const char *src, size_t n) 
{
    strncpy(dest, src, n);
    dest[n-1] = '\0';
    return dest;
}

/* strnlen is a GNU Extension */
#ifndef __HAVE_ARCH_STRNLEN
size_t strnlen (const char * str, size_t maxlen)
{
	const char *p;

	for (p=str; maxlen-- && *p!='\0'; ++p);
	return (p - str);
}
#endif

int sanitizefile(char *buf) 
{
    int len, i;

    // Don't accept anything not starting with a /
    if (*buf != '/') 
        return 0;

    len = strlen(buf);
    for (i = 0; i < len; i++) 
    {
        // Check for "/." : In other words, don't send files starting with a .
        // Notice, GOBBLES, that this includes ".."
        if (buf[i] == '/' && buf[i+1] == '.') 
            return 0;
    }

    return 1;
}

int sanitizehost(char *buf) 
{
    while(*buf != '\0') 
    {
        // Handle the port
        if (*buf == ':') 
        {
            *buf = '\0';
            return 1;
        }

        // Enforce some basic URL rules...
        if (isalnum(*buf)==0 && *buf != '-' && *buf != '.') return 0;
        if (*buf == '.' && *(buf+1) == '.') return 0;
        if (*buf == '.' && *(buf+1) == '-') return 0;
        if (*buf == '-' && *(buf+1) == '.') return 0;
        buf++;
    }

    return 1;
}

void buildactualfile(struct connstruct *cn) 
{
    snprintf(cn->actualfile, MAXREQUESTLENGTH, "%s%s",
            webroot,
            cn->filereq);

    /* Add directory slash if not there */
    if (isdir(cn->actualfile) && 
            cn->actualfile[strlen(cn->actualfile)-1] != '/')
        strcat(cn->actualfile, "/");

#ifdef WIN32
    /* convert all the forward slashes to back slashes */
    {
        char *t = cn->actualfile;
        while ((t = strchr(t, '/')))
        {
            *t++ = '\\';
        }
    }
#endif
}

#if defined(CONFIG_HTTP_DIRECTORIES)
int issockwriteable(int sd) 
{
    fd_set wfds;
    struct timeval tv;

    tv.tv_sec = 0;
    tv.tv_usec = 0;

    FD_ZERO(&wfds);
    FD_SET(sd, &wfds);

    select(FD_SETSIZE, NULL, &wfds, NULL, &tv);

    return FD_ISSET(sd, &wfds);
}
#endif

int isdir(char *tpbuf) 
{
    struct stat st;

    if (stat(tpbuf, &st) == -1) 
        return 0;

    if ((st.st_mode & S_IFMT) == S_IFDIR) 
        return 1;

    return 0;
}

#if defined(CONFIG_HTTP_HAS_CGI)
int iscgi(char *fn) 
{
    struct cgiextstruct *tp;
    int fnlen, extlen;

    fnlen = strlen(fn);
    tp = cgiexts;

    while (tp != NULL) 
    {
        extlen = strlen(tp->ext);

        if (strcasecmp(fn+(fnlen-extlen), tp->ext) == 0)
            return 1;

        tp = tp->next;
    }

    return 0;
}

void split(char *tp, char *sp[], int maxwords, char sc) 
{
    int i = 0;

    while(1) 
    {
        /* Skip leading whitespace */
        while(*tp == sc) tp++;

        if (*tp == '\0') 
        {
            sp[i] = NULL;
            break;
        }

        if (i==maxwords-2) 
        {
            sp[maxwords-2] = NULL;
            break;
        }

        sp[i] = tp;

        while(*tp != sc && *tp != '\0') 
            tp++;

        if (*tp == sc) 
            *tp++ = '\0';

        i++;
    }
}

int trycgi_withpathinfo(struct connstruct *cn) 
{
    char tpfile[MAXREQUESTLENGTH];
    char fr_str[MAXREQUESTLENGTH];
    char *fr_rs[MAXCGIARGS]; // filereq splitted
    int i = 0, offset;

    my_strncpy(fr_str, cn->filereq, MAXREQUESTLENGTH);
    split(fr_str, fr_rs, MAXCGIARGS, '/');

    while (fr_rs[i] != NULL) 
    {
        snprintf(tpfile, sizeof(tpfile), "%s/%s%s", 
                            webroot, cn->virtualhostreq, fr_str);

        if (iscgi(tpfile) && isdir(tpfile) == 0) 
        {
            /* We've found our CGI file! */
            my_strncpy(cn->actualfile, tpfile, MAXREQUESTLENGTH);
            my_strncpy(cn->cgiscriptinfo, fr_str, MAXREQUESTLENGTH);

            offset = (fr_rs[i] + strlen(fr_rs[i])) - fr_str;
            my_strncpy(cn->cgipathinfo, cn->filereq+offset, MAXREQUESTLENGTH);

            return 0;
        }

        *(fr_rs[i]+strlen(fr_rs[i])) = '/';
        i++;
    }

    /* Couldn't find any CGIs :( */
    *(cn->cgiscriptinfo) = '\0';
    *(cn->cgipathinfo) = '\0';
    return -1;
}
#endif
