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
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include "axhttp.h"

// GLOBALS
struct serverstruct *servers;
struct connstruct *usedconns;
struct connstruct *freeconns;
#if defined(CONFIG_HTTP_HAS_CGI)
struct cgiextstruct *cgiexts;
#endif
struct indexstruct *indexlist;

char *webroot = CONFIG_HTTP_WEBROOT;

static void addindex(char *tp);
static void addtoservers(int sd);
static void selectloop(void);
static int openlistener(int port);
static void handlenewconnection(int listenfd, int is_ssl);
#if defined(CONFIG_HTTP_PERM_CHECK)
static void procpermcheck(char *pathtocheck);
#endif
#if defined(CONFIG_HTTP_HAS_CGI)
static void addcgiext(char *tp);
#endif

/* clean up memory for valgrind */
static void sigint_cleanup(int sig)
{
    struct serverstruct *sp;
    struct connstruct *tp;
    int i;

    while(servers != NULL) 
    {
        if (servers->is_ssl)
            ssl_ctx_free(servers->ssl_ctx);

        sp = servers->next;
        free(servers);
        servers = sp;
    }

    free(indexlist->name);
    free(indexlist);

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

void initlists() 
{
    int i;
    struct connstruct *tp;

    servers = NULL;
    usedconns = NULL;
    freeconns = NULL;
#if defined(CONFIG_HTTP_HAS_CGI)
    cgiexts = NULL;
#endif
    indexlist = NULL;

    for (i=0; i<INITIAL_CONNECTION_SLOTS; i++) 
    {
        tp = freeconns;
        freeconns = (struct connstruct *) calloc(1, sizeof(struct connstruct));
        freeconns->next = tp;
    }
}

int main(int argc, char *argv[]) 
{
    int tp;
#if defined(CONFIG_HTTP_IS_DAEMON)
    int pid;
#endif

#ifdef WIN32
    WORD wVersionRequested = MAKEWORD(2,2);
    WSADATA wsaData;
    WSAStartup(wVersionRequested,&wsaData);
#endif

    mime_init();
    initlists();
    tp = strlen(webroot);

    if (webroot[tp-1] == '/') 
        webroot[tp-1] = '\0';

    if (isdir(webroot) == 0) 
    {
#ifdef CONFIG_HTTP_VERBOSE
        fprintf(stderr, "'%s' is not a directory\n", webroot);
#endif
        exit(1);
    }

    if ((tp = openlistener(CONFIG_HTTP_PORT)) == -1) 
    {
#ifdef CONFIG_HTTP_VERBOSE
        fprintf(stderr, "ERR: Couldn't bind to port %d (IPv4)\n",
                CONFIG_HTTP_PORT);
#endif
        exit(1);
    }

    addindex("index.html");
    addtoservers(tp);

#ifndef WIN32
    if (getuid() == 0)
    {
        setgid(32767);
        setuid(32767);
    }
#endif

    if ((tp = openlistener(CONFIG_HTTP_HTTPS_PORT)) == -1) 
    {
#ifdef CONFIG_HTTP_VERBOSE
        fprintf(stderr, "ERR: Couldn't bind to port %d (IPv4)\n", 
                CONFIG_HTTP_HTTPS_PORT);
#endif
        exit(1);
    }

    addtoservers(tp);
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
    printf("axhttpd: listening on ports http:%d and https:%d\n", 
            CONFIG_HTTP_PORT, CONFIG_HTTP_HTTPS_PORT);
    TTY_FLUSH();
#endif

#if defined(CONFIG_HTTP_IS_DAEMON)
    pid = fork();

    if (pid > 0) 
    {
        exit(0);
    } 
    else if(pid == -1) 
    {
#ifdef CONFIG_HTTP_VERBOSE
        fprintf(stderr,"axhttpd: Sorry, fork failed... Tough dice.\n");
#endif
        exit(1);
    }

    setsid();
#endif

    /* SIGNALS */
    signal(SIGINT, sigint_cleanup);
    signal(SIGTERM, die);
#if defined(CONFIG_HTTP_HAS_CGI)
#ifndef WIN32
    signal(SIGCHLD, reaper);
#endif
#endif
#ifndef WIN32
    signal(SIGQUIT, die);
    signal(SIGPIPE, SIG_IGN);
#endif

    selectloop();
    return 0;
}

static void addindex(char *tp) 
{
    struct indexstruct *ex = (struct indexstruct *)
                        malloc(sizeof(struct indexstruct));
    ex->name = strdup(tp);
    ex->next = indexlist;
    indexlist = ex;
}

#if defined(CONFIG_HTTP_PERM_CHECK)
static void procpermcheck(char *pathtocheck) 
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
        return;
    }

    while ((dp=readdir(tpdir))) 
    {
        if (strcmp(dp->d_name, "..")==0) 
            continue;

        if (strcmp(dp->d_name, ".")==0) 
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

static void selectloop(void) 
{
    fd_set rfds, wfds;
    struct connstruct *tp, *to;
    struct serverstruct *sp;
    int rnum, wnum, active;
    int currtime;

    while (1)
    {   
        // MAIN SELECT LOOP
        FD_ZERO(&rfds);
        FD_ZERO(&wfds);
        rnum = wnum = -1;

        // Add the listening sockets
        sp = servers;
        while (sp != NULL) 
        {
            FD_SET(sp->sd, &rfds);
            if (sp->sd > rnum) rnum = sp->sd;
            sp = sp->next;
        }

        // Add the established sockets
        tp = usedconns;
        currtime = time(NULL);

        while (tp != NULL) 
        {
            if (currtime > tp->timeout) 
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

        // Handle the listening sockets
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

        // Handle the established sockets
        tp = usedconns;

        while (active > 0 && tp != NULL) 
        {
            to = tp;
            tp = tp->next;

            if (to->state == STATE_WANT_TO_READ_HEAD)
                if (FD_ISSET(to->networkdesc, &rfds)) 
                {
                    active--;
                    procreadhead(to);
                } 

            if (to->state == STATE_WANT_TO_SEND_HEAD)
                if (FD_ISSET(to->networkdesc, &wfds)) 
                {
                    active--;
                    procsendhead(to);
                } 

            if (to->state == STATE_WANT_TO_READ_FILE)
                if (FD_ISSET(to->filedesc, &rfds)) 
                {
                    active--;
                    procreadfile(to);
                } 

            if (to->state == STATE_WANT_TO_SEND_FILE)
                if (FD_ISSET(to->networkdesc, &wfds)) 
                {
                    active--;
                    procsendfile(to);
                }

#if defined(CONFIG_HTTP_DIRECTORIES)
            if (to->state == STATE_DOING_DIR)
                if (FD_ISSET(to->networkdesc, &wfds)) 
                {
                    active--;
                    procdodir(to);
                }
#endif
        }
    }  // MAIN SELECT LOOP
}

#ifdef HAVE_IPV6
static void handlenewconnection(int listenfd, int is_ssl) 
{
    struct sockaddr_in6 their_addr;
    int tp = sizeof(their_addr);
    char ipbuf[100];
    int connfd = accept(listenfd, (struct sockaddr *)&their_addr, &tp);

    if (connfd == -1) 
        return;

    if (tp == sizeof(struct sockaddr_in6)) 
    {
        inet_ntop(AF_INET6, &their_addr.sin6_addr, ipbuf, sizeof(ipbuf));
    } 
    else if (tp == sizeof(struct sockaddr_in)) 
    {
        inet_ntop(AF_INET, &(((struct sockaddr_in *)&their_addr)->sin_addr),
                ipbuf, sizeof(ipbuf));
    } 
    else 
    {
        *ipbuf = '\0';
    }

    addconnection(connfd, ipbuf, is_ssl);
}

#else
static void handlenewconnection(int listenfd, int is_ssl) 
{
    struct sockaddr_in their_addr;
    int tp = sizeof(struct sockaddr_in);
    int connfd = accept(listenfd, (struct sockaddr *)&their_addr, &tp);

    if (connfd == -1) 
        return;

    addconnection(connfd, inet_ntoa(their_addr.sin_addr), is_ssl);
}
#endif

static int openlistener(int port) 
{
    int sd;
#ifdef WIN32
    char tp=1;
#else
    int tp=1;
#endif
    struct sockaddr_in my_addr;

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
        return -1;

    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &tp, sizeof(tp));
    my_addr.sin_family = AF_INET;         // host byte order
    my_addr.sin_port = htons((short)port);       // short, network byte order
    my_addr.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP
    memset(&(my_addr.sin_zero), 0, 8);    // zero the rest of the struct

    if (bind(sd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) 
    {
        close(sd);
        return -1;
    }

    if (listen(sd, BACKLOG) == -1) 
    {
        close(sd);
        return -1;
    }

    return sd;
}

#ifdef HAVE_IPV6
static int openlistener6(int port) 
{
    int sd,tp;
    struct sockaddr_in6 my_addr;

    if ((sd = socket(AF_INET6, SOCK_STREAM, 0)) == -1) 
        return -1;

    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &tp, sizeof(tp));
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin6_family = AF_INET6;
    my_addr.sin6_port = htons(port);

    if (bind(sd, (struct sockaddr *)&my_addr, sizeof(my_addr)) == -1) 
    {
        close(sd);
        return -1;
    }

    if (listen(sd, BACKLOG) == -1)
    {
        close(sd);
        return -1;
    }

    return sd;
}
#endif
