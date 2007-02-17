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
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include "axhttp.h"

static const char * index_file = "index.html";

static int special_read(struct connstruct *cn, void *buf, size_t count);
static int special_write(struct connstruct *cn, 
                                        const uint8_t *buf, size_t count);
static void send_error(struct connstruct *cn, int err);
static int hexit(char c);
static void urldecode(char *buf);
static void buildactualfile(struct connstruct *cn);
static int sanitizefile(const char *buf);
static int sanitizehost(char *buf);
static int htaccess_check(struct connstruct *cn);

#if defined(CONFIG_HTTP_DIRECTORIES)
static void urlencode(const uint8_t *s, uint8_t *t);
static void procdirlisting(struct connstruct *cn);
#endif
#if defined(CONFIG_HTTP_HAS_CGI)
static void proccgi(struct connstruct *cn, int has_pathinfo);
static int trycgi_withpathinfo(struct connstruct *cn);
static void split(char *tp, char *sp[], int maxwords, char sc);
static int iscgi(const char *fn);
#endif
#ifdef CONFIG_HTTP_HAS_AUTHORIZATION
static int auth_check(struct connstruct *cn);
#endif

/* Returns 1 if elems should continue being read, 0 otherwise */
static int procheadelem(struct connstruct *cn, char *buf) 
{
    char *delim, *value;
#if defined(CONFIG_HTTP_HAS_CGI)
    char *cgi_delim;
#endif

    if ((delim = strchr(buf, ' ')) == NULL)
        return 0;

    *delim = 0;
    value = delim+1;

    if (strcmp(buf, "GET") == 0 || strcmp(buf, "HEAD") == 0 ||
                                            strcmp(buf, "POST") == 0) 
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
            send_error(cn, 403);
            return 0;
        }

        my_strncpy(cn->filereq, value, MAXREQUESTLENGTH);
        cn->if_modified_since = -1;
#if defined(CONFIG_HTTP_HAS_CGI)
        if ((cgi_delim = strchr(value, '?')))
        {
            *cgi_delim = 0;
            my_strncpy(cn->cgiargs, value+1, MAXREQUESTLENGTH);
        }
#endif
    } 
    else if (strcmp(buf, "Host:") == 0) 
    {
        if (sanitizehost(value) == 0) 
        {
            removeconnection(cn);
            return 0;
        }

        my_strncpy(cn->virtualhostreq, value, MAXREQUESTLENGTH);
    } 
    else if (strcmp(buf, "Connection:") == 0 && strcmp(value, "close") == 0) 
    {
        cn->close_when_done = 1;
    } 
    else if (strcmp(buf, "If-Modified-Since:") == 0) 
    {
        cn->if_modified_since = tdate_parse(value);
    }
#ifdef CONFIG_HTTP_HAS_AUTHORIZATION
    else if (strcmp(buf, "Authorization:") == 0 &&
                                    strncmp(value, "Basic ", 6) == 0)
    {
        int size;
        if (base64_decode(&value[6], strlen(&value[6]), 
                                        cn->authorization, &size))
            cn->authorization[0] = 0;   /* error */
        else
            cn->authorization[size] = 0;
    }
#endif

    return 1;
}

#if defined(CONFIG_HTTP_DIRECTORIES)
static void procdirlisting(struct connstruct *cn)
{
    char buf[MAXREQUESTLENGTH];
    char actualfile[1024];

    if (cn->reqtype == TYPE_HEAD) 
    {
        snprintf(buf, sizeof(buf), 
                "HTTP/1.1 200 OK\nContent-Type: text/html\n\n");
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
        send_error(cn, 404);
        return;
    }
#else
    if ((cn->dirp = opendir(actualfile)) == NULL) 
    {
        send_error(cn, 404);
        return;
    }

    /* Get rid of the "." */
    readdir(cn->dirp);
#endif

    snprintf(buf, sizeof(buf), "HTTP/1.1 200 OK\nContent-Type: text/html\n\n"
            "<html><body>\n<title>Directory Listing</title>\n"
            "<h3>Directory listing of %s://%s%s</h3><br />\n", 
            cn->is_ssl ? "https" : "http", cn->virtualhostreq, cn->filereq);
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
    char *file;

    do 
    {
       buf[0] = 0;

#ifdef WIN32
        if (!FindNextFile(cn->dirp, &cn->file_data)) 
#else
        if ((dp = readdir(cn->dirp)) == NULL)  
#endif
        {
            snprintf(buf, sizeof(buf), "</body></html>\n");
            special_write(cn, buf, strlen(buf));
            removeconnection(cn);
            return;
        }

#ifdef WIN32
        file = cn->file_data.cFileName;
#else
        file = dp->d_name;
#endif

        /* if no index file, don't display the ".." directory */
        if (cn->filereq[0] == '/' && cn->filereq[1] == '\0' &&
                strcmp(file, "..") == 0) 
            continue;

        /* don't display files beginning with "." */
        if (file[0] == '.' && file[1] != '.')
            continue;

        /* make sure a '/' is at the end of a directory */
        if (cn->filereq[strlen(cn->filereq)-1] != '/')
            strcat(cn->filereq, "/");

        /* see if the dir + file is another directory */
        snprintf(buf, sizeof(buf), "%s%s", cn->actualfile, file);
        if (isdir(buf))
            strcat(file, "/");

        urlencode(file, encbuf);
        snprintf(buf, sizeof(buf), "<a href=\"%s%s\">%s</a><br />\n",
                cn->filereq, encbuf, file);
    } while (special_write(cn, buf, strlen(buf)));
}

/* Encode funny chars -> %xx in newly allocated storage */
/* (preserves '/' !) */
static void urlencode(const uint8_t *s, uint8_t *t) 
{
    const uint8_t *p = s;
    uint8_t *tp;

    tp = t;

    for (; *p; p++) 
    {
        if ((*p > 0x00 && *p < ',') ||
                (*p > '9' && *p < 'A') ||
                (*p > 'Z' && *p < '_') ||
                (*p > '_' && *p < 'a') ||
                (*p > 'z' && *p < 0xA1)) 
        {
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

#endif

void procreadhead(struct connstruct *cn) 
{
    char buf[MAXREQUESTLENGTH*4], *tp, *next;
    int rv;

    rv = special_read(cn, buf, sizeof(buf)-1);
    if (rv <= 0) 
    {
        if (rv < 0) /* really dead? */
            removeconnection(cn);
        return;
    }

    buf[rv] = '\0';
    next = tp = buf;

#ifdef CONFIG_HTTP_HAS_AUTHORIZATION
    cn->authorization[0] = 0;
#endif

    /* Split up lines and send to procheadelem() */
    while (*next != '\0') 
    {
        /* If we have a blank line, advance to next stage! */
        if (*next == '\r' || *next == '\n') 
        {
            buildactualfile(cn);
            cn->state = STATE_WANT_TO_SEND_HEAD;
            return;
        }

        while (*next != '\r' && *next != '\n' && *next != '\0') 
            next++;

        if (*next == '\r') 
        {
            *next = '\0';
            next += 2;
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
    char buf[MAXREQUESTLENGTH];
    struct stat stbuf;
    time_t now = cn->timeout - CONFIG_HTTP_TIMEOUT;
    char date[32];

    /* are we trying to access a file over the HTTP connection instead of a
     * HTTPS connection? Or is this directory disabled? */
    if (htaccess_check(cn))      
    {
        send_error(cn, 403);
        return;
    }

#ifdef CONFIG_HTTP_HAS_AUTHORIZATION
    if (auth_check(cn))     /* see if there is a '.htpasswd' file */
    {
#ifdef CONFIG_HTTP_VERBOSE
        printf("axhttpd: access to %s denied\n", cn->filereq); TTY_FLUSH();
#endif
        removeconnection(cn);
        return;
    }
#endif

    if (stat(cn->actualfile, &stbuf) == -1)
    {
#if defined(CONFIG_HTTP_HAS_CGI)
        if (stat(cn->actualfile, &stbuf) == -1 && trycgi_withpathinfo(cn) == 0) 
        { 
            /* We Try To Find A CGI */
            proccgi(cn, 1);
            return;
        }
#endif
    }

#if defined(CONFIG_HTTP_HAS_CGI)
    if (iscgi(cn->actualfile))
    {
#ifndef WIN32
        /* An executable file? */
        if ((stbuf.st_mode & S_IEXEC) == 0 || isdir(cn->actualfile)) 
        {
            send_error(cn, 404);
            return;
        }
#endif

        proccgi(cn, 0);
        return;
    }
#endif

    /* look for "index.html"? */
    if (isdir(cn->actualfile))
    {
        char tbuf[MAXREQUESTLENGTH];
        sprintf(tbuf, "%s%s", cn->actualfile, index_file);
        if (stat(tbuf, &stbuf) != -1) 
            strcat(cn->actualfile, index_file);
        else
        {
#if defined(CONFIG_HTTP_DIRECTORIES)
            /* If not, we do a directory listing of it */
            procdirlisting(cn);
#else
            send_error(cn, 404);
#endif
            return;
        }

#if defined(CONFIG_HTTP_HAS_CGI)
        /* If the index is a CGI file, handle it like any other CGI */
        if (iscgi(cn->actualfile))
        {
            /* Set up CGI script */
            if ((stbuf.st_mode & S_IEXEC) == 0 || isdir(cn->actualfile)) 
            {
                send_error(cn, 404);
                return;
            }

            proccgi(cn, 0);
            return;
        }
#endif
    }

    strcpy(date, ctime(&now));

    /* has the file been read before? */
    if (cn->if_modified_since != -1 && (cn->if_modified_since == 0 || 
                                       cn->if_modified_since >= stbuf.st_mtime))
    {
        snprintf(buf, sizeof(buf), "HTTP/1.1 304 Not Modified\nServer: "
                "axhttpd V%s\nDate: %s\n", VERSION, date);
        special_write(cn, buf, strlen(buf));
        cn->state = STATE_WANT_TO_READ_HEAD;
        return;
    }

    if (cn->reqtype == TYPE_HEAD) 
    {
        removeconnection(cn);
        return;
    } 
    else 
    {
        int flags = O_RDONLY;
#if defined(WIN32) || defined(CONFIG_PLATFORM_CYGWIN)
        flags |= O_BINARY;
#endif

        cn->filedesc = open(cn->actualfile, flags);
        if (cn->filedesc == -1) 
        {
            send_error(cn, 404);
            return;
        }

        snprintf(buf, sizeof(buf), "HTTP/1.1 200 OK\nServer: axhttpd V%s\n"
            "Content-Type: %s\nContent-Length: %ld\n"
            "Date: %sLast-Modified: %s\n", VERSION,
            getmimetype(cn->actualfile), (long) stbuf.st_size,
            date, ctime(&stbuf.st_mtime)); /* ctime() has a \n on the end */

        special_write(cn, buf, strlen(buf));

#ifdef CONFIG_HTTP_VERBOSE
        printf("axhttpd: %s:/%s\n", cn->is_ssl ? "https" : "http", cn->filereq);
        TTY_FLUSH();
#endif

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
    }
}

void procreadfile(struct connstruct *cn) 
{
    int rv = read(cn->filedesc, cn->databuf, BLOCKSIZE);

    if (rv == 0 || rv == -1) 
    {
        close(cn->filedesc);
        cn->filedesc = -1;

        if (cn->close_when_done)        /* close immediately */
            removeconnection(cn);
        else 
        {                               /* keep socket open - HTTP 1.1 */
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
    { 
        /* Do nothing */ 
    }
    else 
    {
        memmove(cn->databuf, cn->databuf + rv, cn->numbytes - rv);
        cn->numbytes -= rv;
    }
}

static int special_write(struct connstruct *cn, 
                                        const uint8_t *buf, size_t count)
{
    if (cn->is_ssl)
    {
        SSL *ssl = ssl_find(servers->ssl_ctx, cn->networkdesc);
        return ssl ? ssl_write(ssl, (uint8_t *)buf, count) : -1;
    }
    else
        return SOCKET_WRITE(cn->networkdesc, buf, count);
}

static int special_read(struct connstruct *cn, void *buf, size_t count)
{
    int res;

    if (cn->is_ssl)
    {
        uint8_t *read_buf;
        SSL *ssl = ssl_find(servers->ssl_ctx, cn->networkdesc);

        if ((res = ssl_read(ssl, &read_buf)) > SSL_OK)
            memcpy(buf, read_buf, res > (int)count ? count : res);
    }
    else
        res = SOCKET_READ(cn->networkdesc, buf, count);

    return res;
}

#if defined(CONFIG_HTTP_HAS_CGI)
static void proccgi(struct connstruct *cn, int has_pathinfo) 
{
    int tpipe[2];
    char *myargs[5];
    char buf[MAXREQUESTLENGTH];
#ifdef WIN32
    int tmp_stdout;
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
    pipe(tpipe);

    if (fork() > 0)  /* parent */
    {
        /* Close the write descriptor */
        close(tpipe[1]);
        cn->filedesc = tpipe[0];
        cn->state = STATE_WANT_TO_READ_FILE;
        cn->close_when_done = 1;
        return;
    }

    /* The problem child... */

    /* Our stdout/stderr goes to the socket */
    dup2(tpipe[1], 1);
    dup2(tpipe[1], 2);

    /* If it was a POST request, send the socket data to our stdin */
    if (cn->reqtype == TYPE_POST) 
        dup2(cn->networkdesc, 0);
    else    /* Otherwise we can shutdown the read side of the sock */
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
    _pipe(tpipe, 8192, O_BINARY| O_NOINHERIT);

    myargs[0] = "sh";
    myargs[1] = "-c";
    myargs[2] = &cn->filereq[1];    /* ignore the inital "/" */
    myargs[3] = cn->cgiargs;
    myargs[4] = NULL;

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
    cn->close_when_done = 1;

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

static int trycgi_withpathinfo(struct connstruct *cn)
{
    char tpfile[MAXREQUESTLENGTH];
    char fr_str[MAXREQUESTLENGTH];
    char *fr_rs[MAXCGIARGS]; /* filereq splitted */
    int i = 0, offset;

    my_strncpy(fr_str, cn->filereq, MAXREQUESTLENGTH);
    split(fr_str, fr_rs, MAXCGIARGS, '/');

    while (fr_rs[i] != NULL) 
    {
        snprintf(tpfile, sizeof(tpfile), "/%s%s", 
                            cn->virtualhostreq, fr_str);

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

static int iscgi(const char *fn)
{
    struct cgiextstruct *tp = cgiexts;
    int fnlen, extlen;

    fnlen = strlen(fn);

    while (tp != NULL) 
    {
        extlen = strlen(tp->ext);

        if (strcasecmp(fn+(fnlen-extlen), tp->ext) == 0)
            return 1;

        tp = tp->next;
    }

    return 0;
}

static void split(char *tp, char *sp[], int maxwords, char sc)
{
    int i = 0;

    while(1) 
    {
        /* Skip leading whitespace */
        while (*tp == sc) tp++;

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
#endif  /* CONFIG_HTTP_HAS_CGI */

/* Decode string %xx -> char (in place) */
static void urldecode(char *buf) 
{
    int v;
    char *p, *s, *w;

    w = p = buf;

    while (*p) 
    {
        v = 0;

        if (*p == '%') 
        {
            s = p;
            s++;

            if (isxdigit((int) s[0]) && isxdigit((int) s[1]))
            {
                v = hexit(s[0])*16 + hexit(s[1]);

                if (v) 
                { 
                    /* do not decode %00 to null char */
                    *w = (char)v;
                    p = &s[1];
                }
            }

        }

        if (!v) *w=*p;
        p++; 
        w++;
    }

    *w='\0';
}

static int hexit(char c) 
{
    if (c >= '0' && c <= '9')
        return c - '0';
    else if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    else if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    else
        return 0;
}

static void buildactualfile(struct connstruct *cn)
{
    char *cp;
    snprintf(cn->actualfile, MAXREQUESTLENGTH, "%s", cn->filereq);

#ifndef WIN32
    /* Add directory slash if not there */
    if (isdir(cn->actualfile) && 
            cn->actualfile[strlen(cn->actualfile)-1] != '/')
        strcat(cn->actualfile, "/");

    /* work out the directory name */
    strncpy(cn->dirname, cn->actualfile, MAXREQUESTLENGTH);
    if ((cp = strrchr(cn->dirname, '/')) == NULL)
        cn->dirname[0] = 0;
    else
        *cp = 0;
#else
    {
        char curr_dir[MAXREQUESTLENGTH];
        char path[MAXREQUESTLENGTH];
        char *t = cn->actualfile;

        GetCurrentDirectory(MAXREQUESTLENGTH, curr_dir);

        /* convert all the forward slashes to back slashes */
        while ((t = strchr(t, '/')))
            *t++ = '\\';

        snprintf(path, MAXREQUESTLENGTH, "%s%s", curr_dir, cn->actualfile);
        memcpy(cn->actualfile, path, MAXREQUESTLENGTH);

        /* Add directory slash if not there */
        if (isdir(cn->actualfile) && 
                    cn->actualfile[strlen(cn->actualfile)-1] != '\\')
            strcat(cn->actualfile, "\\");

        /* work out the directory name */
        strncpy(cn->dirname, cn->actualfile, MAXREQUESTLENGTH);
        if ((cp = strrchr(cn->dirname, '\\')) == NULL)
            cn->dirname[0] = 0;
        else
            *cp = 0;
    }
#endif
}

static int sanitizefile(const char *buf) 
{
    int len, i;

    /* Don't accept anything not starting with a / */
    if (*buf != '/') 
        return 0;

    len = strlen(buf);
    for (i = 0; i < len; i++) 
    {
        /* Check for "/." i.e. don't send files starting with a . */
        if (buf[i] == '/' && buf[i+1] == '.') 
            return 0;
    }

    return 1;
}

static int sanitizehost(char *buf)
{
    while (*buf != '\0') 
    {
        /* Handle the port */
        if (*buf == ':') 
        {
            *buf = '\0';
            return 1;
        }

        /* Enforce some basic URL rules... */
        if ((isalnum(*buf) == 0 && *buf != '-' && *buf != '.') ||
                (*buf == '.' && *(buf+1) == '.') ||
                (*buf == '.' && *(buf+1) == '-') ||
                (*buf == '-' && *(buf+1) == '.'))
            return 0;

        buf++;
    }

    return 1;
}

static FILE * exist_check(struct connstruct *cn, const char *check_file)
{
    char pathname[MAXREQUESTLENGTH];
    snprintf(pathname, MAXREQUESTLENGTH, "%s/%s", cn->dirname, check_file);
    return fopen(pathname, "r");
}

#ifdef CONFIG_HTTP_HAS_AUTHORIZATION
static void send_authenticate(struct connstruct *cn, const char *realm)
{
    char buf[1024];

    snprintf(buf, sizeof(buf), "HTTP/1.1 401 Unauthorized\n"
         "WWW-Authenticate: Basic\n"
                 "realm=\"%s\"\n", realm);
    special_write(cn, buf, strlen(buf));
}

static int check_digest(char *salt, const char *msg_passwd)
{
    uint8_t b256_salt[MAXREQUESTLENGTH];
    uint8_t real_passwd[MD5_SIZE];
    int salt_size;
    char *b64_passwd;
    uint8_t md5_result[MD5_SIZE];
    MD5_CTX ctx;

    /* retrieve the salt */
    if ((b64_passwd = strchr(salt, '$')) == NULL)
        return -1;

    *b64_passwd++ = 0;
    if (base64_decode(salt, strlen(salt), b256_salt, &salt_size))
        return -1;

    if (base64_decode(b64_passwd, strlen(b64_passwd), real_passwd, NULL))
        return -1;

    /* very simple MD5 crypt algorithm, but then the salt we use is large */
    MD5Init(&ctx);
    MD5Update(&ctx, b256_salt, salt_size);           /* process the salt */
    MD5Update(&ctx, msg_passwd, strlen(msg_passwd)); /* process the password */
    MD5Final(&ctx, md5_result);
    return memcmp(md5_result, real_passwd, MD5_SIZE);/* 0 = ok */
}

static int auth_check(struct connstruct *cn)
{
    char line[MAXREQUESTLENGTH];
    FILE *fp;
    char *cp;

    if ((fp = exist_check(cn, ".htpasswd")) == NULL)
        return 0;               /* no .htpasswd file, so let though */

    if (cn->authorization[0] == 0)
        goto error;

    /* cn->authorization is in form "username:password" */
    if ((cp = strchr(cn->authorization, ':')) == NULL)
        goto error;
    else
        *cp++ = 0;  /* cp becomes the password */

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        char *b64_file_passwd;
        int l = strlen(line);

        /* nuke newline */
        if (line[l-1] == '\n')
            line[l-1] = 0;

        /* line is form "username:salt(b64)$password(b64)" */
        if ((b64_file_passwd = strchr(line, ':')) == NULL)
            continue;

        *b64_file_passwd++ = 0;

        if (strcmp(line, cn->authorization)) /* our user? */
            continue;

        if (check_digest(b64_file_passwd, cp) == 0)
        {
            fclose(fp);
            return 0;
        }
    }

error:
    fclose(fp);
    send_authenticate(cn, cn->virtualhostreq);
    return -1;
}
#endif

static int htaccess_check(struct connstruct *cn)
{
    char line[MAXREQUESTLENGTH];
    FILE *fp;
    int ret = 0;

    if ((fp = exist_check(cn, ".htaccess")) == NULL)
        return 0;               /* no .htaccess file, so let though */

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        if (strstr(line, "Deny all") || /* access to this dir denied */
                    /* Access will be denied unless SSL is active */
                    (!cn->is_ssl && strstr(line, "SSLRequireSSL")) ||
                    /* Access will be denied if SSL is active */
                    (cn->is_ssl && strstr(line, "SSLDenySSL")))
        {
            ret = -1;
            break;
        }
    }

    fclose(fp);
    return ret;
}

static void send_error(struct connstruct *cn, int err)
{
    char buf[MAXREQUESTLENGTH];
    char *title;
    char *text;

    switch (err)
    {
        case 403:
            title = "Forbidden";
            text = "File is protected";
#ifdef CONFIG_HTTP_VERBOSE
            printf("axhttpd: access to %s denied\n", cn->filereq); TTY_FLUSH();
#endif
            break;

        case 404:
            title = "Not Found";
            text = title;
            break;
    }

    snprintf(buf, MAXREQUESTLENGTH, "HTTP/1.1 %d %s\n"
            "Content-Type: text/html\n"
            "Cache-Control: no-cache,no-store\n"
            "Connection: close\n\n"
            "<html>\n<head>\n<title>%d %s</title></head>\n"
            "<body><h1>%d %s</h1>\n</body></html>\n", 
            err, title, err, title, err, text);
    special_write(cn, buf, strlen(buf));
    removeconnection(cn);
}
