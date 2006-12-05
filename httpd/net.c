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
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "axhttp.h"

void addtoservers(int sd) 
{
    struct serverstruct *tp = (struct serverstruct *)
                            calloc(1, sizeof(struct serverstruct));
    tp->next = servers;
    tp->sd = sd;
    servers = tp;
}

void selectloop() 
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
