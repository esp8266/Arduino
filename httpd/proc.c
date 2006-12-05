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
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include "axhttp.h"

static int special_read(struct connstruct *cn, void *buf, size_t count);
static void send301(struct connstruct *cn);
static void send404(struct connstruct *cn);
static int procindex(struct connstruct *cn, struct stat *stp);
static int hexit(char c);
static void urlencode(unsigned char *s, unsigned char *t);
static void urldecode(char *buf);

#if defined(CONFIG_HTTP_HAS_CGI)
static void proccgi(struct connstruct *cn, int has_pathinfo);
#endif

// Returns 1 if elems should continue being read, 0 otherwise
int procheadelem(struct connstruct *cn, char *buf) 
{
    char *delim, *value;
#if defined(CONFIG_HTTP_HAS_CGI)
    char *cgi_delim;
#endif

    if ((delim = strchr(buf, ' ')) == NULL)
        return 0;

    *delim = 0;
    value = delim+1;

    if (strcmp(buf, "GET")==0 ||
            strcmp(buf, "HEAD")==0 ||
            strcmp(buf, "POST")==0) 
    {
        if (buf[0] == 'H') 
            cn->reqtype = TYPE_HEAD;
        else if (buf[0] == 'P') 
            cn->reqtype = TYPE_POST;

        if ((delim = strchr(value, ' ')) == NULL)       /* expect HTTP type */
            return 0;
        *delim = 0;

        urldecode(value);

        if (sanitizefile(value) == 0) 
        {
            send404(cn);
            removeconnection(cn);
            return 0;
        }

        my_strncpy(cn->filereq, value, MAXREQUESTLENGTH);
#if defined(CONFIG_HTTP_HAS_CGI)
        if ((cgi_delim = strchr(value, '?')))
        {
            *cgi_delim = 0;
            my_strncpy(cn->cgiargs, value+1, MAXREQUESTLENGTH);
        }
#endif

    } 
    else if (strcmp(buf, "Host:")==0) 
    {
        if (sanitizehost(value) == 0) 
        {
            send404(cn);
            removeconnection(cn);
            return 0;
        }

        my_strncpy(cn->virtualhostreq, value, MAXREQUESTLENGTH);
    } 
    else if (strcmp(buf, "Connection:")==0 &&
            strcmp(value, "close")==0) {
        cn->close_when_done = 1;
    } 
    else if (strcmp(buf, "If-Modified-Since:") ==0 ) 
    {
        /* TODO: parse this date properly with getdate() or similar */
        cn->modified_since = 1;
    }

    return 1;
}

#if defined(CONFIG_HTTP_DIRECTORIES)
void procdirlisting(struct connstruct *cn) 
{
    char buf[MAXREQUESTLENGTH];
    char actualfile[1024];

#ifndef CONFIG_HTTP_DIRECTORIES
    if (allowdirectorylisting == 0) 
    {
        send404(cn);
        removeconnection(cn);
        return;
    }
#endif

    if (cn->reqtype == TYPE_HEAD) 
    {
        snprintf(buf, sizeof(buf), "HTTP/1.1 200 OK\nContent-Type: text/html\n\n");
        write(cn->networkdesc, buf, strlen(buf));

        removeconnection(cn);
        return;
    }

    strcpy(actualfile, cn->actualfile);
#ifdef WIN32
    strcat(actualfile, "*");
    cn->dirp = FindFirstFile(actualfile, &cn->file_data);
    if (cn->dirp == INVALID_HANDLE_VALUE) 
    {
        send404(cn);
        removeconnection(cn);
        return;
    }
#else

    cn->dirp = opendir(actualfile);

    if (cn->dirp == NULL) 
    {
        send404(cn);
        removeconnection(cn);
        return;
    }

    // Get rid of the "."
    readdir(cn->dirp);
#endif

    sprintf(buf, "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<HTML><BODY>\n<TITLE>Directory Listing</TITLE>\n<H2>Directory listing of %s://%s%s</H2><BR>\n", cn->is_ssl ? "https" : "http", cn->virtualhostreq, cn->filereq);
    special_write(cn, buf, strlen(buf));
    cn->state = STATE_DOING_DIR;
}

void procdodir(struct connstruct *cn) 
{
#ifndef WIN32
    struct dirent *dp;
#endif
    char buf[MAXREQUESTLENGTH];
    char encbuf[1024];
    int putslash;
    char *file;

    do 
    {
#ifdef WIN32
        if (!FindNextFile(cn->dirp, &cn->file_data)) 
#else
            if ((dp = readdir(cn->dirp)) == NULL)  
#endif
            {
                snprintf(buf, sizeof(buf), "</BODY></HTML>\n");
                special_write(cn, buf, strlen(buf));
                removeconnection(cn);
                return;
            }

#ifdef WIN32
        file = cn->file_data.cFileName;
#else
        file = dp->d_name;
#endif

        if (cn->filereq[0] == '/' && cn->filereq[1] == '\0' &&
                strcmp(file, "..") == 0) continue;

        snprintf(buf, sizeof(buf), "%s%s", cn->actualfile, file);
        putslash = isdir(buf);

        urlencode(file, encbuf);
        snprintf(buf, sizeof(buf), "<A HREF=\"%s%s\">%s%s</A><BR>\n",
                encbuf, putslash ? "/" : "", file, putslash ? "/" : "");
        special_write(cn, buf, strlen(buf));

    } 
    while (issockwriteable(cn->networkdesc));
}
#endif

void procreadhead(struct connstruct *cn) 
{
    char buf[MAXREQUESTLENGTH*4], *tp, *next;
    int rv;

    rv = special_read(cn, buf, sizeof(buf)-1);
    if (rv <= 0) {
        if (rv < 0)
            removeconnection(cn);
        return;
    }

    buf[rv] = '\0';

    next = tp = buf;

    // Split up lines and send to procheadelem()
    while(*next != '\0') 
    {
        // If we have a blank line, advance to next stage!
        if (*next == '\r' || *next == '\n') 
        {
            buildactualfile(cn);

            cn->state = STATE_WANT_TO_SEND_HEAD;
            return;
        }

        while(*next != '\r' && *next != '\n' && *next != '\0') 
            next++;

        if (*next == '\r') 
        {
            *next = '\0';
            next+=2;
        }
        else if (*next == '\n') 
            *next++ = '\0';

        if (procheadelem(cn, tp) == 0) 
            return;

        tp = next;
    }
}

/* In this function we assume that the file has been checked for
 * maliciousness (".."s, etc) and has been decoded
 */
void procsendhead(struct connstruct *cn) 
{
    char buf[1024];
    char actualfile[1024];
    struct stat stbuf;
    time_t now = cn->timeout - CONFIG_HTTP_TIMEOUT;
    char date[32];
    strcpy(date, ctime(&now));

    strcpy(actualfile, cn->actualfile);

#ifdef WIN32
    /* stat() under win32 can't deal with trail slash */
    if (actualfile[strlen(actualfile)-1] == '\\')
        actualfile[strlen(actualfile)-1] = 0;
#endif

    if (stat(actualfile, &stbuf) == -1) 
    {
#if defined(CONFIG_HTTP_HAS_CGI)
        if (trycgi_withpathinfo(cn) == 0) 
        { // We Try To Find A CGI
            proccgi(cn,1);
            return;
        }
#endif

        send404(cn);
        removeconnection(cn);
        return;
    }

#if defined(CONFIG_HTTP_HAS_CGI)
    if (iscgi(cn->actualfile)) 
    {
#ifndef WIN32
        // Set up CGI script
        if ((stbuf.st_mode & S_IEXEC) == 0 || isdir(cn->actualfile)) 
        {
            send404(cn);
            removeconnection(cn);
            return;
        }
#endif

        proccgi(cn,0);
        return;
    }
#endif

    if ((stbuf.st_mode & S_IFMT) == S_IFDIR) 
    {
        if (cn->filereq[strlen(cn->filereq)-1] != '/') 
        {
            send301(cn);
            removeconnection(cn);
            return;
        }

        // Check to see if this dir has an index file
        if (procindex(cn, &stbuf) == 0) 
        {
#if defined(CONFIG_HTTP_DIRECTORIES)
            // If not, we do a directory listing of it
            procdirlisting(cn);
#else
            send404(cn);
            removeconnection(cn);
#endif
            return;
        }

#if defined(CONFIG_HTTP_HAS_CGI)
        // If the index is a CGI file, handle it like any other CGI
        if (iscgi(cn->actualfile)) 
        {
            // Set up CGI script
            if ((stbuf.st_mode & S_IEXEC) == 0 || isdir(cn->actualfile)) 
            {
                send404(cn);
                removeconnection(cn);
                return;
            }

            proccgi(cn,0);
            return;
        }
#endif
        // If the index isn't a CGI, we continue on with the index file
    }

    if ((stbuf.st_mode & S_IFMT) == S_IFDIR) 
    {
        if (cn->filereq[strlen(cn->filereq)-1] != '/') 
        {
            send301(cn);
            removeconnection(cn);
            return;
        }

        // Check to see if this dir has an index file
        if (procindex(cn, &stbuf) == 0) 
        {
#if defined(CONFIG_HTTP_DIRECTORIES)
            // If not, we do a directory listing of it
            procdirlisting(cn);
#endif
            return;
        }

#if defined(CONFIG_HTTP_HAS_CGI)
        // If the index is a CGI file, handle it like any other CGI
        if (iscgi(cn->actualfile)) 
        {
            // Set up CGI script
            if ((stbuf.st_mode & S_IEXEC) == 0 || isdir(cn->actualfile)) 
            {
                send404(cn);
                removeconnection(cn);
                return;
            }

            proccgi(cn,0);
            return;
        }
#endif
        // If the index isn't a CGI, we continue on with the index file
    }

    if (cn->modified_since) 
    {
        snprintf(buf, sizeof(buf), "HTTP/1.1 304 Not Modified\nServer: axhttpd V%s\nDate: %s\n", VERSION, date);
        special_write(cn, buf, strlen(buf));
        cn->modified_since = 0;
        cn->state = STATE_WANT_TO_READ_HEAD;
        return;
    }
    else 
    {
#ifdef CONFIG_HTTP_VERBOSE
        printf("axhttpd: %s send %s\n", 
                cn->is_ssl ? "https" : "http", cn->actualfile);
        TTY_FLUSH();
#endif

        snprintf(buf, sizeof(buf), "HTTP/1.1 200 OK\nServer: axhttpd V%s\nContent-Type: %s\nContent-Length: %ld\nDate: %sLast-Modified: %s\n",
                VERSION,
                getmimetype(cn->actualfile),
                (long) stbuf.st_size,
                date,
                ctime(&(stbuf.st_mtime))); // ctime() has a \n on the end
    }

    special_write(cn, buf, strlen(buf));

    if (cn->reqtype == TYPE_HEAD) 
    {
        removeconnection(cn);
        return;
    } 
    else 
    {
        int flags = O_RDONLY;
#if defined(WIN32) || defined(CYGWIN)
        flags |= O_BINARY;
#endif

        cn->filedesc = open(cn->actualfile, flags);
        if (cn->filedesc == -1) 
        {
            send404(cn);
            removeconnection(cn);
            return;
        }

#ifdef WIN32
        for (;;)
        {
            procreadfile(cn);
            if (cn->filedesc == -1)
                break;

            do 
            {
                procsendfile(cn);
            } while (cn->state != STATE_WANT_TO_READ_FILE);
        }
#else
        cn->state = STATE_WANT_TO_READ_FILE;
#endif
        return;
    }
}

void procreadfile(struct connstruct *cn) 
{
    int rv = read(cn->filedesc, cn->databuf, BLOCKSIZE);

    if (rv == 0 || rv == -1) 
    {
        close(cn->filedesc);
        cn->filedesc = -1;
        if (cn->close_when_done)      /* close immediately */
            removeconnection(cn);
        else 
        {                        /* keep socket open - HTTP 1.1 */
            cn->state = STATE_WANT_TO_READ_HEAD;
            cn->numbytes = 0;
        }

        return;
    }

    cn->numbytes = rv;
    cn->state = STATE_WANT_TO_SEND_FILE;
}

void procsendfile(struct connstruct *cn) 
{
    int rv = special_write(cn, cn->databuf, cn->numbytes);

    if (rv < 0)
        removeconnection(cn);
    else if (rv == cn->numbytes)
        cn->state = STATE_WANT_TO_READ_FILE;
    else if (rv == 0)
    { /* Do nothing */ }
    else 
    {
        memmove(cn->databuf, cn->databuf + rv, cn->numbytes - rv);
        cn->numbytes -= rv;
    }
}

int special_write(struct connstruct *cn, const uint8_t *buf, size_t count)
{
    int res;

    if (cn->is_ssl)
    {
        SSL *ssl = ssl_find(servers->ssl_ctx, cn->networkdesc);
        if (ssl)
        {
            res = ssl_write(ssl, (unsigned char *)buf, count);
        }
        else
            return -1;
    }
    else
        res = SOCKET_WRITE(cn->networkdesc, buf, count);

    return res;
}

static int special_read(struct connstruct *cn, void *buf, size_t count)
{
    int res;

    if (cn->is_ssl)
    {
        SSL *ssl = ssl_find(servers->ssl_ctx, cn->networkdesc);
        unsigned char *read_buf;

        if ((res = ssl_read(ssl, &read_buf)) > SSL_OK)
            memcpy(buf, read_buf, res > (int)count ? count : res);
    }
    else
        res = SOCKET_READ(cn->networkdesc, buf, count);

    return res;
}

static void send301(struct connstruct *cn)
{
    char buf[2048];
    sprintf(buf, "HTTP/1.1 301 Moved Permanently\nLocation: %s/\n\n<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\n<HTML><HEAD>\n<TITLE>301 Moved Permanently</TITLE>\n</HEAD><BODY>\n<H1>Moved Permanently</H1>\nThe document has moved <A HREF=\"%s/\">here</A>.<P>\n<HR>\n</BODY></HTML>\n", cn->filereq, cn->filereq);
    special_write(cn, buf, strlen(buf));
}

static void send404(struct connstruct *cn)
{
    char buf[1024];
    sprintf(buf, "HTTP/1.0 404 Not Found\nContent-Type: text/html\n\n<HTML><BODY>\n<TITLE>404 Not Found</TITLE><H1>It ain't there my friend. (404 Not Found)</H1>\n</BODY></HTML>\n");
    special_write(cn, buf, strlen(buf));
}

// Returns 0 if no index was found and doesn't modify cn->actualfile
// Returns 1 if an index was found and puts the index in cn->actualfile
//              and puts its stat info into stp
static int procindex(struct connstruct *cn, struct stat *stp) 
{
    char tbuf[MAXREQUESTLENGTH];
    struct indexstruct *tp;

    tp = indexlist;

    while(tp != NULL) {
        sprintf(tbuf, "%s%s%s", cn->actualfile, 
#ifdef WIN32
                "\\",
#else
                "/",
#endif
                tp->name);

        if (stat(tbuf, stp) != -1) 
        {
            my_strncpy(cn->actualfile, tbuf, MAXREQUESTLENGTH);
            return 1;
        }

        tp = tp->next;
    }

    return 0;
}

#if defined(CONFIG_HTTP_HAS_CGI)
static void proccgi(struct connstruct *cn, int has_pathinfo) 
{
    int tpipe[2];
    char *myargs[5];
    char buf[MAXREQUESTLENGTH];
#ifdef WIN32
    int tmp_stdout;
#else
    int fv;
#endif

    snprintf(buf, sizeof(buf), "HTTP/1.1 200 OK\nServer: axhttpd V%s\n%s",
            VERSION, (cn->reqtype == TYPE_HEAD) ? "\n" : "");
    special_write(cn, buf, strlen(buf));

    if (cn->reqtype == TYPE_HEAD) 
    {
        removeconnection(cn);
        return;
    }

#ifndef WIN32
    if (pipe(tpipe) == -1) 
    {
        removeconnection(cn);
        return;
    }

    fv = fork();

    if (fv == -1) 
    {
        close(tpipe[0]);
        close(tpipe[1]);
        removeconnection(cn);
        return;
    }

    if (fv != 0) 
    {
        // Close the write descriptor
        close(tpipe[1]);
        cn->filedesc = tpipe[0];
        cn->state = STATE_WANT_TO_READ_FILE;
        return;
    }

    // The problem child...

    // Our stdout/stderr goes to the socket
    dup2(tpipe[1], 1);
    dup2(tpipe[1], 2);

    // If it was a POST request, send the socket data to our stdin
    if (cn->reqtype == TYPE_POST) 
        dup2(cn->networkdesc, 0);
    else // Otherwise we can shutdown the read side of the sock
        shutdown(cn->networkdesc, 0);

    close(tpipe[0]);
    close(tpipe[1]);

    myargs[0] = cn->actualfile;
    myargs[1] = cn->cgiargs;
    myargs[2] = NULL;

    if (!has_pathinfo) 
    {
        my_strncpy(cn->cgipathinfo, "/", MAXREQUESTLENGTH);
        my_strncpy(cn->cgiscriptinfo, cn->filereq, MAXREQUESTLENGTH);
    }

    execv(cn->actualfile, myargs);
#else /* WIN32 */
    if (_pipe(tpipe, 4096, O_BINARY| O_NOINHERIT) == -1) 
    {
        removeconnection(cn);
        return;
    }

    myargs[0] = "sh";
    myargs[1] = "-c";
    myargs[2] = cn->actualfile;
    myargs[3] = cn->cgiargs;
    myargs[4] = NULL;

    /* convert all the forward slashes to back slashes */
    {
        char *t = myargs[2];
        while ((t = strchr(t, '\\')))
        {
            *t++ = '/';
        }
    }

    tmp_stdout = _dup(_fileno(stdout));
    _dup2(tpipe[1], _fileno(stdout));
    close(tpipe[1]);

    /* change to suit execution method */
    if (spawnl(P_NOWAIT, "c:\\Program Files\\cygwin\\bin\\sh.exe", 
                myargs[0], myargs[1], myargs[2], myargs[3], myargs[4]) == -1) 
    {
        removeconnection(cn);
        return;
    }

    _dup2(tmp_stdout, _fileno(stdout));
    close(tmp_stdout);
    cn->filedesc = tpipe[0];
    cn->state = STATE_WANT_TO_READ_FILE;

    for (;;)
    {
        procreadfile(cn);

        if (cn->filedesc == -1)
            break;

        procsendfile(cn);
        usleep(200000); /* don't know why this delay makes it work (yet) */
    }
#endif
}
#endif  /* CONFIG_HTTP_HAS_CGI */

/* Encode funny chars -> %xx in newly allocated storage */
/* (preserves '/' !) */
static void urlencode(unsigned char *s, unsigned char *t) 
{
    uint8_t *p, *tp;

    tp =t ;

    for (p=s; *p; p++) 
    {
        if ((*p > 0x00 && *p < ',') ||
                (*p > '9' && *p < 'A') ||
                (*p > 'Z' && *p < '_') ||
                (*p > '_' && *p < 'a') ||
                (*p > 'z' && *p < 0xA1)) {
            sprintf((char *)tp, "%%%02X", *p);
            tp += 3; 
        } 
        else 
        {
            *tp = *p;
            tp++;
        }
    }

    *tp='\0';
}

/* Decode string %xx -> char (in place) */
static void urldecode(char *buf) 
{
    int v;
    char *p, *s, *w;

    w = p = buf;

    while (*p) 
    {
        v = 0;

        if (*p=='%') 
        {
            s = p;
            s++;

            if (isxdigit((int) s[0]) && isxdigit((int) s[1]))
            {
                v = hexit(s[0])*16+hexit(s[1]);
                if (v) 
                { /* do not decode %00 to null char */
                    *w=(char)v;
                    p=&s[1];
                }
            }

        }

        if (!v) 
            *w=*p;
        p++; w++;
    }

    *w='\0';
}

static int hexit(char c) 
{
    if ( c >= '0' && c <= '9' )
        return c - '0';
    if ( c >= 'a' && c <= 'f' )
        return c - 'a' + 10;
    if ( c >= 'A' && c <= 'F' )
        return c - 'A' + 10;

    return 0;
}

