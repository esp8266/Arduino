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
#include <stdlib.h>
#include "axhttp.h"

void addconnection(int sd, char *ip, int is_ssl) 
{
    struct connstruct *tp;

    // Get ourselves a connstruct
    if (freeconns == NULL) 
    {
        tp = (struct connstruct *) malloc(sizeof(struct connstruct));
    } 
    else 
    {
        tp = freeconns;
        freeconns = tp->next;
    }

    // Attach it to the used list
    tp->next = usedconns;
    usedconns = tp;

    tp->networkdesc = sd;
    if (is_ssl)
        ssl_server_new(servers->ssl_ctx, sd);
    tp->filedesc = -1;
#if defined(CONFIG_HTTP_HAS_DIRECTORIES)
    tp->dirp = NULL;
#endif
    tp->is_ssl = is_ssl;

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
    int shouldret=0;

    tp = usedconns;

    if (tp == NULL || cn == NULL) 
        shouldret=1;
    else if (tp == cn) 
        usedconns = tp->next;
    else 
    {
        while(tp != NULL) 
        {
            if (tp->next == cn) 
            {
                tp->next = (tp->next)->next;
                shouldret=0;
                break;
            }

            tp = tp->next;
            shouldret=1;
        }
    }

    if (shouldret) 
        return;

    // If we did, add it to the free list
    cn->next = freeconns;
    freeconns = cn;

    // Close it all down
    if (cn->networkdesc != -1) 
    {
        if (cn->is_ssl) 
        {
            ssl_free(ssl_find(servers->ssl_ctx, cn->networkdesc));
        }

        SOCKET_CLOSE(cn->networkdesc);
    }

    if (cn->filedesc != -1) close(cn->filedesc);
#if defined(CONFIG_HTTP_HAS_DIRECTORIES)
    if (cn->dirp != NULL) 
#ifdef WIN32
        FindClose(cn->dirp);
#else
    closedir(cn->dirp);
#endif
#endif
}
