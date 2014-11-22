/*
 * Copyright (c) Cameron Rich
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

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/stat.h>

#if !defined(WIN32)
#include <pwd.h>
#endif
#include "axhttp.h"

struct serverstruct *servers;
struct connstruct *usedconns;
struct connstruct *freeconns;
const char * const server_version = "axhttpd/"AXTLS_VERSION;
static const char *webroot = CONFIG_HTTP_WEBROOT;

static void addtoservers(int sd);
static int openlistener(char *address, int port);
static void handlenewconnection(int listenfd, int is_ssl);
static void addconnection(int sd, char *ip, int is_ssl);
static void ax_chdir(void);

#if defined(CONFIG_HTTP_HAS_CGI)
struct cgiextstruct *cgiexts;
static void addcgiext(const char *tp);

#if !defined(WIN32)
static void reaper(int sigtype) 
{
    while (wait3(NULL, WNOHANG, NULL) > 0)
        continue;
}
#endif
#endif

#ifdef CONFIG_HTTP_VERBOSE  /* should really be in debug mode or something */
/* clean up memory for valgrind */
static void sigint_cleanup(int sig)
{
    struct serverstruct *sp;
    struct connstruct *tp;

    while (servers != NULL) 
    {
        if (servers->is_ssl)
            ssl_ctx_free(servers->ssl_ctx);

        sp = servers->next;
        free(servers);
        servers = sp;
    }

    while (freeconns != NULL)
    {
        tp = freeconns->next;
        free(freeconns);
        freeconns = tp;
    }

    while (usedconns != NULL)
    {
        tp = usedconns->next;
        free(usedconns);
        usedconns = tp;
    }

#if defined(CONFIG_HTTP_HAS_CGI)
    while (cgiexts)
    {
        struct cgiextstruct *cp = cgiexts->next;
        if (cp == NULL) /* last entry */
            free(cgiexts->ext);
        free(cgiexts);
        cgiexts = cp;
    }
#endif

    exit(0);
}

static void die(int sigtype) 
{
    exit(0);
}
#endif

int main(int argc, char *argv[]) 
{
    fd_set rfds, wfds;
    struct connstruct *tp, *to;
    struct serverstruct *sp;
    int rnum, wnum, active;
    int i = 1;
    time_t currtime;
    char *httpAddress = NULL;
    int httpPort = CONFIG_HTTP_PORT;
    char *httpsAddress = NULL;
    int httpsPort = CONFIG_HTTP_HTTPS_PORT;
    uint32_t options = CONFIG_HTTP_DEFAULT_SSL_OPTIONS;
    char *portStr;
    char *private_key = NULL;
    char *cert = NULL;

#ifdef WIN32
    WORD wVersionRequested = MAKEWORD(2, 2);
    WSADATA wsaData;
    WSAStartup(wVersionRequested,&wsaData);
#else
    signal(SIGPIPE, SIG_IGN);
#if defined(CONFIG_HTTP_HAS_CGI)
    signal(SIGCHLD, reaper);
#endif
#ifdef CONFIG_HTTP_VERBOSE
    signal(SIGQUIT, die);
#endif
#endif

#ifdef CONFIG_HTTP_VERBOSE
    signal(SIGTERM, die);
    signal(SIGINT, sigint_cleanup);
#endif
    tdate_init();

    /* get some command-line parameters */
    while (argv[i] != NULL)
    {
        if (strcmp(argv[i], "-p") == 0 && argv[i+1] != NULL)
        {
            if ((portStr = strchr(argv[i+1], ':')) != NULL)
            {
                httpAddress = argv[i+1];
                *portStr = 0;
                httpPort = atoi(portStr + 1);
            }
            else
                httpPort = atoi(argv[i+1]);

            i += 2;
            continue;
        }

        if (strcmp(argv[i], "-s") == 0 && argv[i+1] != NULL)
        {
            if ((portStr = strchr(argv[i+1], ':')) != NULL)
            {
                httpsAddress = argv[i+1];
                *portStr = 0;
                httpsPort = atoi(portStr + 1);
            }
            else
                httpsPort = atoi(argv[i+1]);

            i += 2;
            continue;
        }

        if (strcmp(argv[i], "-w") == 0 && argv[i+1] != NULL)
        {
            webroot = argv[i+1];
            i += 2;
            continue;
        }

        if (strcmp(argv[i], "-cert") == 0 && argv[i+1] != NULL)
        {
            cert = argv[i+1];
            i += 2;
            continue;
        }

        if (strcmp(argv[i], "-key") == 0 && argv[i+1] != NULL)
        {
            private_key = argv[i+1];
            i += 2;
            continue;
        }
        printf("%s:\n"
               "    [-p [address:]httpport]\n"
               "    [-s [address:]httpsport]\n"
               "    [-key private_key]\n"
               "    [-cert cert]\n"
               "    [-w webroot]\n", argv[0]);
        exit(1);
    }

    for (i = 0; i < INITIAL_CONNECTION_SLOTS; i++) 
    {
        tp = freeconns;
        freeconns = (struct connstruct *)calloc(1, sizeof(struct connstruct));
        freeconns->next = tp;
    }

    if ((active = openlistener(httpAddress, httpPort)) == -1) 
    {
#ifdef CONFIG_HTTP_VERBOSE
        fprintf(stderr, "ERR: Couldn't bind to port %d\n", httpPort);
#endif
        exit(1);
    }

    addtoservers(active);

    if ((active = openlistener(httpsAddress, httpsPort)) == -1) 
    {
#ifdef CONFIG_HTTP_VERBOSE
        fprintf(stderr, "ERR: Couldn't bind to port %d\n", httpsPort);
#endif
        exit(1);
    }

    addtoservers(active);

    if (cert != NULL && private_key != NULL)
        options |=  SSL_NO_DEFAULT_KEY;

    servers->ssl_ctx = ssl_ctx_new(options, CONFIG_HTTP_SESSION_CACHE_SIZE);
    servers->is_ssl = 1;

    if (cert != NULL && private_key != NULL)
    {
        printf("YEAH\n");
        if (ssl_obj_load(servers->ssl_ctx, SSL_OBJ_RSA_KEY, private_key,
                    NULL))
        {
#ifdef CONFIG_HTTP_VERBOSE
            fprintf(stderr, "ERR: Couldn't load private key %s\n", private_key);
#endif
            exit(1);
        }

        if (ssl_obj_load(servers->ssl_ctx, SSL_OBJ_X509_CERT, cert,
                    NULL))
        {
#ifdef CONFIG_HTTP_VERBOSE
            fprintf(stderr, "ERR: Couldn't load cert %s\n", cert);
#endif
            exit(1);
        }
    }

#if defined(CONFIG_HTTP_HAS_CGI)
    addcgiext(CONFIG_HTTP_CGI_EXTENSIONS);
#endif

#if defined(CONFIG_HTTP_VERBOSE)
#if defined(CONFIG_HTTP_HAS_CGI)
    printf("addcgiext %s\n", CONFIG_HTTP_CGI_EXTENSIONS); 
#endif
    printf("%s: listening on ports %d (http) and %d (https)\n", 
            server_version, httpPort, httpsPort);
    TTY_FLUSH();
#endif

    ax_chdir();

#ifdef CONFIG_HTTP_ENABLE_DIFFERENT_USER
    {
        struct passwd *pd = getpwnam(CONFIG_HTTP_USER);

        if (pd != NULL)
        {
            int res = setuid(pd->pw_uid);
            res |= setgid(pd->pw_gid);

#if defined(CONFIG_HTTP_VERBOSE)
            if (res == 0)
            {
                printf("change to '%s' successful\n", CONFIG_HTTP_USER); 
                TTY_FLUSH();
            }
#endif
        }

    }
#endif

#ifndef WIN32 
#ifdef CONFIG_HTTP_IS_DAEMON
    if (fork() > 0)  /* parent will die */
        exit(0);

    setsid();
#endif
#endif

    /* main loop */
    while (1)
    {
        struct timeval tv = { 10, 0 };
        FD_ZERO(&rfds);
        FD_ZERO(&wfds);
        rnum = wnum = -1;
        sp = servers;

        while (sp != NULL)  /* read each server port */
        {
            FD_SET(sp->sd, &rfds);

            if (sp->sd > rnum) 
                rnum = sp->sd;
            sp = sp->next;
        }

        /* Add the established sockets */
        tp = usedconns;
        currtime = time(NULL);

        while (tp != NULL) 
        {
            if (currtime > tp->timeout)     /* timed out? Kill it. */
            {
                to = tp;
                tp = tp->next;
                removeconnection(to);
                continue;
            }

            if (tp->state == STATE_WANT_TO_READ_HEAD) 
            {
                FD_SET(tp->networkdesc, &rfds);
                if (tp->networkdesc > rnum) 
                    rnum = tp->networkdesc;
            }

            if (tp->state == STATE_WANT_TO_SEND_HEAD) 
            {
                FD_SET(tp->networkdesc, &wfds);
                if (tp->networkdesc > wnum) 
                    wnum = tp->networkdesc;
            }

            if (tp->state == STATE_WANT_TO_READ_FILE) 
            {
                FD_SET(tp->filedesc, &rfds);
                if (tp->filedesc > rnum) 
                    rnum = tp->filedesc;
            }

            if (tp->state == STATE_WANT_TO_SEND_FILE) 
            {
                FD_SET(tp->networkdesc, &wfds);
                if (tp->networkdesc > wnum) 
                    wnum = tp->networkdesc;
            }

#if defined(CONFIG_HTTP_DIRECTORIES)
            if (tp->state == STATE_DOING_DIR) 
            {
                FD_SET(tp->networkdesc, &wfds);
                if (tp->networkdesc > wnum) 
                    wnum = tp->networkdesc;
            }
#endif
            tp = tp->next;
        }

        active = select(wnum > rnum ? wnum+1 : rnum+1,
                rnum != -1 ? &rfds : NULL, 
                wnum != -1 ? &wfds : NULL,
                NULL, usedconns ? &tv : NULL);

        /* timeout? */
        if (active == 0)
            continue;

        /* New connection? */
        sp = servers;
        while (active > 0 && sp != NULL) 
        {
            if (FD_ISSET(sp->sd, &rfds)) 
            {
                handlenewconnection(sp->sd, sp->is_ssl);
                active--;
            }

            sp = sp->next;
        }

        /* Handle the established sockets */
        tp = usedconns;

        while (active > 0 && tp != NULL) 
        {
            to = tp;
            tp = tp->next;

            if (to->state == STATE_WANT_TO_READ_HEAD &&
                        FD_ISSET(to->networkdesc, &rfds)) 
            {
                active--;
#if defined(CONFIG_HTTP_HAS_CGI)
                if (to->post_state)
                    read_post_data(to);
                else
#endif
                    procreadhead(to);
            } 

            if (to->state == STATE_WANT_TO_SEND_HEAD &&
                        FD_ISSET(to->networkdesc, &wfds)) 
            {
                active--;
                procsendhead(to);
            } 

            if (to->state == STATE_WANT_TO_READ_FILE && 
                        FD_ISSET(to->filedesc, &rfds)) 
            {
                active--;
                procreadfile(to);
            } 

            if (to->state == STATE_WANT_TO_SEND_FILE && 
                        FD_ISSET(to->networkdesc, &wfds)) 
            {
                active--;
                procsendfile(to);
            }

#if defined(CONFIG_HTTP_DIRECTORIES)
            if (to->state == STATE_DOING_DIR &&
                        FD_ISSET(to->networkdesc, &wfds)) 
            {
                active--;
                procdodir(to);
            }
#endif
        }
    }

    return 0;
}

#if defined(CONFIG_HTTP_HAS_CGI)
static void addcgiext(const char *cgi_exts)
{
    char *cp = strdup(cgi_exts);

    /* extenstions are comma separated */
    do 
    {
        struct cgiextstruct *ex = (struct cgiextstruct *)
                            malloc(sizeof(struct cgiextstruct));
        ex->ext = cp;
        ex->next = cgiexts;
        cgiexts = ex;
        if ((cp = strchr(cp, ',')) != NULL)
            *cp++ = 0;
    } while (cp != NULL);
}
#endif

static void addtoservers(int sd) 
{
    struct serverstruct *tp = (struct serverstruct *)
                            calloc(1, sizeof(struct serverstruct));
    tp->next = servers;
    tp->sd = sd;
    servers = tp;
}

#ifdef HAVE_IPV6
static void handlenewconnection(int listenfd, int is_ssl) 
{
    struct sockaddr_in6 their_addr;
    socklen_t tp = sizeof(their_addr);
    char ipbuf[100];
    int connfd = accept(listenfd, (struct sockaddr *)&their_addr, &tp);

    if (tp == sizeof(struct sockaddr_in6)) 
        inet_ntop(AF_INET6, &their_addr.sin6_addr, ipbuf, sizeof(ipbuf));
    else if (tp == sizeof(struct sockaddr_in)) 
        inet_ntop(AF_INET, &(((struct sockaddr_in *)&their_addr)->sin_addr),
                ipbuf, sizeof(ipbuf));
    else 
        *ipbuf = '\0';

    if (connfd != -1) /* check for error condition */
        addconnection(connfd, ipbuf, is_ssl);
}

#else
static void handlenewconnection(int listenfd, int is_ssl) 
{
    struct sockaddr_in their_addr;
    socklen_t tp = sizeof(struct sockaddr_in);
    int connfd = accept(listenfd, (struct sockaddr *)&their_addr, &tp);
    addconnection(connfd, inet_ntoa(their_addr.sin_addr), is_ssl);
}
#endif

static int openlistener(char *address, int port) 
{
    int sd;
#ifdef WIN32
    char tp = 1;
#else
    int tp = 1;
#endif
#ifndef HAVE_IPV6
    struct sockaddr_in my_addr;

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
        return -1;

    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons((short)port);
    my_addr.sin_addr.s_addr = address == NULL ? 
                        INADDR_ANY : inet_addr(address);
#else
    struct sockaddr_in6 my_addr;

    if ((sd = socket(AF_INET6, SOCK_STREAM, 0)) == -1) 
        return -1;

    my_addr.sin6_family = AF_INET6;
    my_addr.sin6_port = htons(port);

    if (address == NULL)
        my_addr.sin6_addr = in6addr_any;
    else 
        inet_pton(AF_INET6, address, &my_addr.sin6_addr);
#endif

    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &tp, sizeof(tp));
    if (bind(sd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1)
    {
        close(sd);
        return -1;
    }

    listen(sd, BACKLOG);
    return sd;
}

/* Wrapper function for strncpy() that guarantees
   a null-terminated string. This is to avoid any possible
   issues due to strncpy()'s behaviour.
 */
char *my_strncpy(char *dest, const char *src, size_t n) 
{
    strncpy(dest, src, n);
    dest[n-1] = '\0';
    return dest;
}

int isdir(const char *tpbuf) 
{
    struct stat st;
    char path[MAXREQUESTLENGTH];
    strcpy(path, tpbuf);

#ifdef WIN32        /* win32 stat() can't handle trailing '\' */
    if (path[strlen(path)-1] == '\\')
        path[strlen(path)-1] = 0;
#endif

    if (stat(path, &st) == -1) 
        return 0;

    if ((st.st_mode & S_IFMT) == S_IFDIR) 
        return 1;

    return 0;
}

static void addconnection(int sd, char *ip, int is_ssl) 
{
    struct connstruct *tp;

    /* Get ourselves a connstruct */
    if (freeconns == NULL) 
        tp = (struct connstruct *)calloc(1, sizeof(struct connstruct));
    else 
    {
        tp = freeconns;
        freeconns = tp->next;
    }

    /* Attach it to the used list */
    tp->next = usedconns;
    usedconns = tp;
    tp->networkdesc = sd;

    if (is_ssl)
        tp->ssl = ssl_server_new(servers->ssl_ctx, sd);

    tp->is_ssl = is_ssl;
    tp->filedesc = -1;
#if defined(CONFIG_HTTP_HAS_DIRECTORIES)
    tp->dirp = NULL;
#endif
    *tp->actualfile = '\0';
    *tp->filereq = '\0';
    tp->state = STATE_WANT_TO_READ_HEAD;
    tp->reqtype = TYPE_GET;
    tp->close_when_done = 0;
    tp->timeout = time(NULL) + CONFIG_HTTP_TIMEOUT;
#if defined(CONFIG_HTTP_HAS_CGI)
    strcpy(tp->remote_addr, ip);
#endif
}

void removeconnection(struct connstruct *cn) 
{
    struct connstruct *tp;
    int shouldret = 0;

    tp = usedconns;

    if (tp == NULL || cn == NULL) 
        shouldret = 1;
    else if (tp == cn) 
        usedconns = tp->next;
    else 
    {
        while (tp != NULL) 
        {
            if (tp->next == cn) 
            {
                tp->next = (tp->next)->next;
                shouldret = 0;
                break;
            }

            tp = tp->next;
            shouldret = 1;
        }
    }

    if (shouldret) 
        return;

    /* If we did, add it to the free list */
    cn->next = freeconns;
    freeconns = cn;

    /* Close it all down */
    if (cn->networkdesc != -1) 
    {
        if (cn->is_ssl) 
        {
            ssl_free(cn->ssl);
            cn->ssl = NULL;
        }

#ifndef WIN32
        shutdown(cn->networkdesc, SHUT_WR);
#endif
        SOCKET_CLOSE(cn->networkdesc);
    }

    if (cn->filedesc != -1) 
        close(cn->filedesc);

#if defined(CONFIG_HTTP_HAS_DIRECTORIES)
    if (cn->dirp != NULL) 
#ifdef WIN32
        FindClose(cn->dirp);
#else
        closedir(cn->dirp);
#endif
#endif
}

/*
 * Change directories one way or the other.
 */

static void ax_chdir(void)
{
    if (chdir(webroot))
    {
#ifdef CONFIG_HTTP_VERBOSE
        fprintf(stderr, "'%s' is not a directory\n", webroot);
#endif
        exit(1);
    }
}

