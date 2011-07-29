/*
 * Copyright (c) 2009, Steve Bennett
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

/*
 * sslwrap re-implemented with axTLS - a way to wrap an existing webserver
 * with axTLS.
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/poll.h>
#include "os_port.h"
#include "ssl.h"

/* If nothing is received or sent in this many seconds, give up */
static int opt_timeout = 60;

static int opt_verbose = 0;

int main(int argc, char *argv[])
{
	int log_opts = LOG_PERROR;
	int fd[2]; /* output from child */
	int df[2]; /* input to child */
	int pid;
	unsigned char *readbuf;
	int readlen;

	SSL_CTX *ssl_ctx;
	SSL *ssl;

	/* This relies on stdin and stdout being one and the same */
	int sslfd = fileno(stdin);

	while (argc > 2 && argv[1][0] == '-') 
    {
		if (argc > 3 && strcmp(argv[1], "-t") == 0) 
        {
			opt_timeout = atoi(argv[2]);
			argv += 2;
			argc -= 2;
			continue;
		}

		if (strcmp(argv[1], "-q") == 0) 
        {
			log_opts = 0;
			argv++;
			argc--;
			continue;
		}

		if (strcmp(argv[1], "-v") == 0) 
        {
			opt_verbose++;
			argv++;
			argc--;
			continue;
		}
	}

	if (argc < 2) 
    {
		fprintf(stderr, "Usage: axtlswrap [-v] [-q] "
                "[-t timeout] command ...\n");
		return 1;
	}

	if (access(argv[1], X_OK) != 0) 
    {
		fprintf(stderr, "Not an executabled: %s\n", argv[1]);
		return 1;
	}

	openlog("axtlswrap", LOG_PID | log_opts, LOG_DAEMON);

	/* Create an SSL context with the required options */
	ssl_ctx = ssl_ctx_new(opt_verbose > 1 ? 
                    SSL_DISPLAY_STATES | SSL_DISPLAY_CERTS : 0, 1);

	if (ssl_ctx == NULL) 
    {
		syslog(LOG_ERR, "Failed to create SSL ctx");
		return 1;
	}

	/* And create an ssl session attached to sslfd */
	ssl = ssl_server_new(ssl_ctx, sslfd);
	if (ssl == NULL) 
    {
		syslog(LOG_ERR, "Failed to create SSL connection");
		return 1;
	}

	/* Get past the handshaking */
	while ((readlen = ssl_read(ssl, &readbuf)) == SSL_OK) 
    {
		/* Still handshaking */
	}

	if (readlen < 0) 
    {
		syslog(LOG_ERR, "SSL handshake failed: %d", readlen);
		return 1;
	}

	if (opt_verbose) 
    {
		syslog(LOG_INFO, "SSL handshake OK");
	}

	/* Looks OK, we have data, so fork the child and start */
	if (pipe(fd) < 0 || pipe(df) < 0) 
    {
		syslog(LOG_ERR, "pipe failed: %m");
		return 1;
	}

	/* Give some indication to the child that we are running SSL
	 * It would be possible to provide other details
	 * too. Perhaps as in: http://httpd.apache.org/docs/2.0/mod/mod_ssl.html
	 */
	setenv("SSL_PROTOCOL", "TLSv1", 1);

#ifndef NOMMU
	if (opt_verbose) 
    {
		pid = fork();
	}
	else
#endif
	pid = vfork();
	if (pid < 0) 
    {
		syslog(LOG_ERR, "vfork failed: %m");
		return 1;
	}

	if (pid > 0) 
    {
		/* This is the parent */
		unsigned char writebuf[4096];
		int writelen = 0;
		struct pollfd pfd[3];
		int timeout_count = 0;

		int cwfd = df[1];	/* write to child */
		int crfd = fd[0];	/* read from child */

		int child_alive = 1;

		/* Don't die on SIGPIPE */
		signal(SIGPIPE, SIG_IGN);

		close(df[0]);
		close(fd[1]);

		pfd[0].fd = sslfd;
		pfd[1].fd = cwfd;
		pfd[2].fd = crfd;

		/* While the child is alive or there is something to return...  */
		while (child_alive || writelen > 0) 
        {
			/* Work out what to read and what to write */
			int ret;

			pfd[0].events = 0;
			pfd[0].revents = 0;

			/* Only want to read ssl data if there is nothing else to do */
			if (readlen == 0) 
            {
				/* can read ssl data */
				pfd[0].events |= POLLIN;
			}

			if (writelen > 0) 
            {
				/* can write ssl data - will block to do this */
				pfd[0].events |= POLLOUT;
			}

			pfd[1].events = 0;
			pfd[1].revents = 0;

			if (child_alive && readlen > 0) 
            {
				pfd[1].events |= POLLOUT;
			}

			pfd[2].events = 0;
			pfd[2].revents = 0;

			if (child_alive && writelen == 0) 
            {
				pfd[2].events |= POLLIN;
			}

			/* Timeout after 1 second so we can increment timeout_count */
			ret = poll(pfd, 3, 1000);

			if (ret < 0) 
            {
				if (errno != EAGAIN) 
                {
					/* Kill off the child */
					kill(pid, SIGTERM);
					break;
				}

				continue;
			}

			if (ret == 0) 
            {
				if (++timeout_count >= opt_timeout) 
                {
					/* Kill off the child */
					kill(pid, SIGTERM);
					break;
				}

				continue;
			}

			timeout_count = 0;

			if (pfd[2].revents & POLLNVAL) 
            {
				/* REVISIT: This can probably be removed */
				syslog(LOG_ERR, "Child closed output pipe");
				child_alive = 0;
			}
			else if (pfd[2].revents & POLLIN) 
            {
				/* Can read from (3) */
				writelen = read(crfd, writebuf, sizeof(writebuf));
				if (writelen <= 0) 
                {
					if (writelen < 0) 
                    {
						syslog(LOG_WARNING, "Failed to read from child: len=%d",
                                writelen);
					}
					break;
				}
			}
			else if ((pfd[2].revents & POLLHUP) && kill(pid, 0) == 0) 
            {
				if (opt_verbose) 
                {
					syslog(LOG_INFO, "Child died and pipe gave POLLHUP");
				}

				child_alive = 0;
			}

			if (writelen > 0) 
            {
				const unsigned char *pt = writebuf;
				while (writelen > 0) 
                {
					ret = ssl_write(ssl, pt, writelen);
					if (ret <= 0) 
                    {
						syslog(LOG_WARNING, "Failed to write ssl: ret=%d", ret);
						/* Kill off the child now */
						kill(pid, SIGTERM);
						writelen = -1;
						break;
					}
					else 
                    {
						pt += ret;
						writelen -= ret;
					}
				}
				if (writelen < 0) 
                {
					break;
				}
			}
			else if (pfd[0].revents & POLLIN) 
            {
				readlen = ssl_read(ssl, &readbuf);
				if (readlen <= 0 && opt_verbose) 
                {
					syslog(LOG_INFO, "ssl_read() returned %d", readlen);
				}

				if (readlen < 0) 
                {
					/* Kill off the child */
					kill(pid, SIGTERM);
					break;
				}
			}

			if (pfd[1].revents & POLLNVAL) 
            {
				/* REVISIT: This can probably be removed */
				syslog(LOG_ERR, "Child closed input pipe");
				readlen = -1;
				child_alive = 0;
			}
			else if (pfd[1].revents & POLLOUT) 
            {
				const unsigned char *pt = readbuf;
				while (readlen > 0) 
                {
					int len = write(cwfd, pt, readlen);
					if (len <= 0) 
                    {
						syslog(LOG_WARNING, "Failed to write to child: len=%d", 
                                len);
						break;
					}

					readlen -= len;
					pt += len;
				}
			}

		}

		ssl_free(ssl);
#if 0
		fprintf(stderr, "[%d] SSL done: timeout_count=%d, readlen=%d, writelen=%d, child_alive=%d\n",
			getpid(), timeout_count, readlen, writelen, child_alive);
#endif
		return 0;
	}

	/* Child */
	close(df[1]);
	close(fd[0]);

	dup2(df[0],0);
	dup2(fd[1],1);

	close(df[0]);
	close(fd[1]);

	execv(argv[1], argv + 1);
	_exit(1);
}
