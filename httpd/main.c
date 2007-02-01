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
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "axhttp.h"

struct serverstruct *servers;
struct connstruct *usedconns;
struct connstruct *freeconns;
char *webroot = CONFIG_HTTP_WEBROOT;

static void addtoservers(int sd);
static int openlistener(int port);
static void handlenewconnection(int listenfd, int is_ssl);
static void addconnection(int sd, char *ip, int is_ssl);
#if defined(CONFIG_HTTP_PERM_CHECK)
static void procpermcheck(const char *pathtocheck);
#endif

#if defined(CONFIG_HTTP_HAS_CGI)
struct cgiextstruct *cgiexts;
static void addcgiext(char *tp);

#if !defined(WIN32)
static void reaper(int sigtype) 
{
    wait3(NULL, WNOHANG, NULL);
}
#endif
#endif

/* clean up memory for valgrind */
static void sigint_cleanup(int sig)
{
    struct serverstruct *sp;
    struct connstruct *tp;
    int i;

    while (servers != NULL) 
    {
        if (servers->is_ssl)
            ssl_ctx_free(servers->ssl_ctx);

        sp = servers->next;
        free(servers);
        servers = sp;
    }

    for (i = 0; i < INITIAL_CONNECTION_SLOTS; i++) 
    {
        if (freeconns == NULL)
            break;

        tp = freeconns->next;
        free(freeconns);
        freeconns = tp;
    }

    exit(0);
}

static void die(int sigtype) 
{
    exit(0);
}

int main(int argc, char *argv[]) 
{
    fd_set rfds, wfds;
    struct connstruct *tp, *to;
    struct serverstruct *sp;
    int rnum, wnum, active;
    int webrootlen, i;
    time_t currtime;

#ifdef WIN32
    WORD wVersionRequested = MAKEWORD(2, 2);
    WSADATA wsaData;
    WSAStartup(wVersionRequested,&wsaData);
#else
    if (getuid() == 0)  /* change our uid if we are root */
    {
        setgid(32767);
        setuid(32767);
    }

    signal(SIGQUIT, die);
    signal(SIGPIPE, SIG_IGN);
#if defined(CONFIG_HTTP_HAS_CGI)
    signal(SIGCHLD, reaper);
#endif
#endif

    signal(SIGINT, sigint_cleanup);
    signal(SIGTERM, die);
    mime_init();

    for (i = 0; i < INITIAL_CONNECTION_SLOTS; i++) 
    {
        tp = freeconns;
        freeconns = (struct connstruct *)calloc(1, sizeof(struct connstruct));
        freeconns->next = tp;
    }

    webrootlen = strlen(webroot);

    if (webroot[webrootlen-1] == '/') 
        webroot[webrootlen-1] = '\0';

    if (isdir(webroot) == 0) 
    {
#ifdef CONFIG_HTTP_VERBOSE
        fprintf(stderr, "'%s' is not a directory\n", webroot);
#endif
        exit(1);
    }

    if ((active = openlistener(CONFIG_HTTP_PORT)) == -1) 
    {
#ifdef CONFIG_HTTP_VERBOSE
        fprintf(stderr, "ERR: Couldn't bind to port %d\n",
                CONFIG_HTTP_PORT);
#endif
        exit(1);
    }

    addtoservers(active);

    if ((active = openlistener(CONFIG_HTTP_HTTPS_PORT)) == -1) 
    {
#ifdef CONFIG_HTTP_VERBOSE
        fprintf(stderr, "ERR: Couldn't bind to port %d\n", 
                CONFIG_HTTP_HTTPS_PORT);
#endif
        exit(1);
    }

    addtoservers(active);
    servers->ssl_ctx = ssl_ctx_new(CONFIG_HTTP_DEFAULT_SSL_OPTIONS, 
                                CONFIG_HTTP_SESSION_CACHE_SIZE);
    servers->is_ssl = 1;

#if defined(CONFIG_HTTP_PERM_CHECK) 
    procpermcheck(webroot);
#endif
#if defined(CONFIG_HTTP_HAS_CGI)
    addcgiext(CONFIG_HTTP_CGI_EXTENSION);
#endif
#if defined(CONFIG_HTTP_VERBOSE)
    printf("axhttpd (%s): listening on ports %d (http) and %d (https)\n", 
            ssl_version(), CONFIG_HTTP_PORT, CONFIG_HTTP_HTTPS_PORT);
    TTY_FLUSH();
#endif
#if defined(CONFIG_HTTP_IS_DAEMON)
    if (fork() > 0)  /* parent will die */
        exit(0);

    setsid();
#endif

    /* main loop */
    while (1)
    {
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
                NULL, NULL);

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

#if defined(CONFIG_HTTP_PERM_CHECK)
static void procpermcheck(const char *pathtocheck) 
{
    char thepath[MAXREQUESTLENGTH];
#ifndef WIN32
    DIR *tpdir;
    struct dirent *dp;

    tpdir = opendir(pathtocheck);

    if (tpdir == NULL) 
    {
        printf("WARNING: UID (%d) is unable to read %s\n", 
                (int)getuid(), pathtocheck);
        TTY_FLUSH();
        return;
    }

    while ((dp = readdir(tpdir))) 
    {
        if (strcmp(dp->d_name, "..") == 0)
            continue;

        if (strcmp(dp->d_name, ".") == 0)
            continue;

        snprintf(thepath, sizeof(thepath), "%s/%s", pathtocheck, dp->d_name);

        if (isdir(thepath))
        {
            procpermcheck(thepath);
            continue;
        }

        if (access(thepath, R_OK) != 0)
            printf("WARNING: UID (%d) is unable to read %s\n",
                                (int)getuid(), thepath);

        if (access(thepath, W_OK) == 0)
            printf("SECURITY: UID (%d) is ABLE TO WRITE TO %s\n",
                                (int)getuid(), thepath);

        TTY_FLUSH();
    }

    closedir(tpdir);
#else   /* Win32 */
    HANDLE tpdir;
    WIN32_FIND_DATA file_data;
    struct stat st;
    char buf2[1024];

    strcpy(buf2, pathtocheck);
    strcat(buf2, "\\*");
    tpdir = FindFirstFile(buf2, &file_data);

    if (tpdir == INVALID_HANDLE_VALUE) 
    {
        printf("WARNING: unable to read %s\n", buf2);
        TTY_FLUSH();
        return;
    }

    while (FindNextFile(tpdir, &file_data)) 
    {
        if (strcmp(file_data.cFileName, "..") == 0) 
            continue;

        if (strcmp(file_data.cFileName, ".") == 0) 
            continue;

        snprintf(thepath, sizeof(thepath), "%s\\%s", 
                pathtocheck, file_data.cFileName);

        if (isdir(thepath)) 
        {
            procpermcheck(thepath);
            continue;
        }

        if (stat(thepath, &st) >= 0) 
        {
            if ((st.st_mode & _S_IREAD) == 0) 
            {
                printf("WARNING: unable to read %s\n", thepath);
                TTY_FLUSH();
            }

            if (st.st_mode & _S_IWRITE) 
            {
                printf("SECURITY: ABLE TO WRITE TO %s\n", thepath);
                TTY_FLUSH();
            }
        }
    }

    FindClose(tpdir);
#endif
}
#endif  /* CONFIG_HTTP_PERM_CHECK */

#if defined(CONFIG_HTTP_HAS_CGI)
static void addcgiext(char *tp)
{
    struct cgiextstruct *ex = (struct cgiextstruct *)
                        malloc(sizeof(struct cgiextstruct));
    ex->ext = strdup(tp);
    ex->next = cgiexts;
    cgiexts = ex;
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
    int tp = sizeof(their_addr);
    char ipbuf[100];
    int connfd = accept(listenfd, (struct sockaddr *)&their_addr, &tp);

    if (tp == sizeof(struct sockaddr_in6)) 
        inet_ntop(AF_INET6, &their_addr.sin6_addr, ipbuf, sizeof(ipbuf));
    else if (tp == sizeof(struct sockaddr_in)) 
        inet_ntop(AF_INET, &(((struct sockaddr_in *)&their_addr)->sin_addr),
                ipbuf, sizeof(ipbuf));
    else 
        *ipbuf = '\0';

    addconnection(connfd, ipbuf, is_ssl);
}

#else
static void handlenewconnection(int listenfd, int is_ssl) 
{
    struct sockaddr_in their_addr;
    int tp = sizeof(struct sockaddr_in);
    int connfd = accept(listenfd, (struct sockaddr *)&their_addr, &tp);
    addconnection(connfd, inet_ntoa(their_addr.sin_addr), is_ssl);
}
#endif

static int openlistener(int port) 
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
    my_addr.sin_addr.s_addr = INADDR_ANY;
#else
    struct sockaddr_in6 my_addr;

    if ((sd = socket(AF_INET6, SOCK_STREAM, 0)) == -1) 
        return -1;

    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin6_family = AF_INET6;
    my_addr.sin6_port = htons(port);
    my_addr.sin6_addr.s_addr = INADDR_ANY;
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

    if (stat(tpbuf, &st) == -1) 
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
        tp = (struct connstruct *)malloc(sizeof(struct connstruct));
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
        ssl_server_new(servers->ssl_ctx, sd);

    tp->is_ssl = is_ssl;
    tp->filedesc = -1;
#if defined(CONFIG_HTTP_HAS_DIRECTORIES)
    tp->dirp = NULL;
#endif
    *(tp->actualfile) = '\0';
    *(tp->filereq) = '\0';
#if defined(CONFIG_HTTP_HAS_CGI)
    *(tp->cgiargs) = '\0';
#endif
    *(tp->virtualhostreq) = '\0';
    tp->state = STATE_WANT_TO_READ_HEAD;
    tp->reqtype = TYPE_GET;
    my_strncpy(tp->ip, ip, MAXIPLEN);
    tp->close_when_done = 0;
    tp->modified_since = 0;
    tp->timeout = time(NULL) + CONFIG_HTTP_TIMEOUT;
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
            ssl_free(ssl_find(servers->ssl_ctx, cn->networkdesc));

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

