/*
Copyright (c) 1998.  The Regents of the University of California (Regents).
All Rights Reserved.

Permission to use, copy, modify, and distribute this software and its
documentation for educational, research, and not-for-profit purposes, without
fee and without a signed licensing agreement, is hereby granted, provided that
the above copyright notice, this paragraph and the following two paragraphs
appear in all copies, modifications, and distributions.  Contact The Office of
Technology Licensing, UC Berkeley, 2150 Shattuck Avenue, Suite 510, Berkeley,
CA 94720-1620, (510) 643-7201, for commercial licensing opportunities.

Written by Matt Wright, The Center for New Music and Audio Technologies,
University of California, Berkeley.

     IN NO EVENT SHALL REGENTS BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT,
     SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS,
     ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
     REGENTS HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

     REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT
     LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
     FOR A PARTICULAR PURPOSE. THE SOFTWARE AND ACCOMPANYING
     DOCUMENTATION, IF ANY, PROVIDED HEREUNDER IS PROVIDED "AS IS".
     REGENTS HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
     ENHANCEMENTS, OR MODIFICATIONS.

The OpenSound Control WWW page is 
    http://www.cnmat.berkeley.edu/OpenSoundControl
*/

/*
 OSC_timeTag.h: library for manipulating OSC time tags
 Matt Wright, 5/29/97

 Time tags in OSC have the same format as in NTP: 64 bit fixed point, with the
 top 32 bits giving number of seconds sinve midnight 1/1/1900 and the bottom
 32 bits giving fractional parts of a second.  We represent this by a 64-bit
 unsigned long if possible, or else a struct. 

 NB: On many architectures with 64-bit ints, it's illegal (like maybe a bus error)
 to dereference a pointer to a 64-bit int that's not 64-bit aligned.  
*/

#ifndef OSC_TIMETAG
#define OSC_TIMETAG

#include <inttypes.h>

#ifdef __sgi
    #define HAS8BYTEINT
    /* You may have to change this typedef if there's some other
       way to specify 64 bit ints on your system */
    typedef long long int64;
    typedef unsigned long long uint64;
    typedef unsigned long uint32;
#else
    /* You may have to redefine this typedef if ints on your system 
       aren't 32 bits. */
    typedef uint32_t uint32;
#endif


#ifdef HAS8BYTEINT
    typedef uint64 OSCTimeTag;
#else
    typedef struct {
	uint32 seconds;
	uint32 fraction;
    } OSCTimeTag;
#endif



/* Return a time tag representing the current time (as of when this
   procedure is called). */
OSCTimeTag OSCTT_CurrentTime(void);

/* Return the time tag 0x0000000000000001, indicating to the receiving device
   that it should process the message immediately. */
OSCTimeTag OSCTT_Immediately(void);

/* Return the time tag 0xffffffffffffffff, a time so far in the future that
   it's effectively infinity. */
OSCTimeTag OSCTT_BiggestPossibleTimeTag(void);

/* Given a time tag and a number of seconds to add to the time tag, return
   the new time tag */
OSCTimeTag OSCTT_PlusSeconds(OSCTimeTag original, float secondsOffset);

/* Compare two time tags.  Return negative if first is < second, 0 if
   they're equal, and positive if first > second. */
int OSCTT_Compare(OSCTimeTag left, OSCTimeTag right);

#endif /*  OSC_TIMETAG */
