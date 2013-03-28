/* Copyright (c) 2004, 2010 Joerg Wunsch
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.

   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.

   * Neither the name of the copyright holders nor the names of
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
*/
/* $Id: realloc.c 2127 2010-06-07 14:49:37Z joerg_wunsch $ */

#include <stdlib.h>
#include <string.h>
#include "sectionname.h"
#include "stdlib_private.h"

#include <avr/io.h>

ATTRIBUTE_CLIB_SECTION
void *
realloc(void *ptr, size_t len)
{
	struct __freelist *fp1, *fp2, *fp3, *ofp3;
	char *cp, *cp1;
	void *memp;
	size_t s, incr;

	/* Trivial case, required by C standard. */
	if (ptr == 0)
		return malloc(len);

	cp1 = (char *)ptr;
	cp1 -= sizeof(size_t);
	fp1 = (struct __freelist *)cp1;

	cp = (char *)ptr + len; /* new next pointer */
	if (cp < cp1)
		/* Pointer wrapped across top of RAM, fail. */
		return 0;

	/*
	 * See whether we are growing or shrinking.  When shrinking,
	 * we split off a chunk for the released portion, and call
	 * free() on it.  Therefore, we can only shrink if the new
	 * size is at least sizeof(struct __freelist) smaller than the
	 * previous size.
	 */
	if (len <= fp1->sz) {
		/* The first test catches a possible unsigned int
		 * rollover condition. */
		if (fp1->sz <= sizeof(struct __freelist) ||
		    len > fp1->sz - sizeof(struct __freelist))
			return ptr;
		fp2 = (struct __freelist *)cp;
		fp2->sz = fp1->sz - len - sizeof(size_t);
		fp1->sz = len;
		free(&(fp2->nx));
		return ptr;
	}

	/*
	 * If we get here, we are growing.  First, see whether there
	 * is space in the free list on top of our current chunk.
	 */
	incr = len - fp1->sz;
	cp = (char *)ptr + fp1->sz;
	fp2 = (struct __freelist *)cp;
	for (s = 0, ofp3 = 0, fp3 = __flp;
	     fp3;
	     ofp3 = fp3, fp3 = fp3->nx) {
		if (fp3 == fp2 && fp3->sz + sizeof(size_t) >= incr) {
			/* found something that fits */
			if (fp3->sz + sizeof(size_t) - incr > sizeof(struct __freelist)) {
				/* split off a new freelist entry */
				cp = (char *)ptr + len;
				fp2 = (struct __freelist *)cp;
				fp2->nx = fp3->nx;
				fp2->sz = fp3->sz - incr;
				fp1->sz = len;
			} else {
				/* it just fits, so use it entirely */
				fp1->sz += fp3->sz + sizeof(size_t);
				fp2 = fp3->nx;
			}
			if (ofp3)
				ofp3->nx = fp2;
			else
				__flp = fp2;
			return ptr;
		}
		/*
		 * Find the largest chunk on the freelist while
		 * walking it.
		 */
		if (fp3->sz > s)
			s = fp3->sz;
	}
	/*
	 * If we are the topmost chunk in memory, and there was no
	 * large enough chunk on the freelist that could be re-used
	 * (by a call to malloc() below), quickly extend the
	 * allocation area if possible, without need to copy the old
	 * data.
	 */
	if (__brkval == (char *)ptr + fp1->sz && len > s) {
		cp1 = __malloc_heap_end;
		cp = (char *)ptr + len;
		if (cp1 == 0)
			cp1 = STACK_POINTER() - __malloc_margin;
		if (cp < cp1) {
			__brkval = cp;
			fp1->sz = len;
			return ptr;
		}
		/* If that failed, we are out of luck. */
		return 0;
	}

	/*
	 * Call malloc() for a new chunk, then copy over the data, and
	 * release the old region.
	 */
	if ((memp = malloc(len)) == 0)
		return 0;
	memcpy(memp, ptr, fp1->sz);
	free(ptr);
	return memp;
}

