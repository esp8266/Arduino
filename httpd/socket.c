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
#include <sys/types.h>
#include <string.h>
#include "axhttp.h"

#ifdef HAVE_IPV6

void handlenewconnection(int listenfd, int is_ssl) 
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
void handlenewconnection(int listenfd, int is_ssl) 
{
    struct sockaddr_in their_addr;
    int tp = sizeof(struct sockaddr_in);
    int connfd = accept(listenfd, (struct sockaddr *)&their_addr, &tp);

    if (connfd == -1) 
        return;

    addconnection(connfd, inet_ntoa(their_addr.sin_addr), is_ssl);
}
#endif

int openlistener(int port) 
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
int openlistener6(int port) 
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
