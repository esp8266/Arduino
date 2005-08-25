/*
Copyright (c) 1996,1997.  The Regents of the University of California (Regents).
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
*/

/* 

   OSC-client.h: library for constructing OpenSoundControl messages.
   Derived from SynthControl.h
   Author: Matt Wright
   Version 0.1: 6/13/97
   Version 0.2: 7/21/2000: Support for type-tagged messages


   General notes:

   This library abstracts away the data format for the OpenSoundControl
   protocol.  Users of this library can construct OpenSoundControl packets
   with a function call interface instead of knowing how to lay out the bits.

   All issues of memory allocation are deferred to the user of this library.
   There are two data structures that the user must allocate.  The first
   is the actual buffer that the message will be written into.  This buffer
   can be any size, but if it's too small there's a possibility that it
   will become overfull.  The other data structure is called an OSCbuf,
   and it holds all the state used by the library as it's constructing
   a buffer.

   All procedures that have the possibility of an error condition return int,
   with 0 indicating no error and nonzero indicating an error.  The variable
   OSC_errorMessage will be set to point to a string containing an error
   message explaining what the problem is.

*/


#include "OSC-timetag.h"
#include "global.h"
#include <progmem.h>

#define ATMEL

#ifdef ATMEL
#define htonl(x) \
				((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8) |    \
		      (((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24))

#endif

/* The int4byte type has to be a 4-byte integer.  You may have to
   change this to long or something else on your system.  */
#ifdef __MWERKS__
  /* In Metrowerks you can set ints to be 2 or 4 bytes on 68K, but long is
     always 4 bytes */
    typedef long int4byte;
#else
    typedef s32 int4byte;
#endif

/* The maximum depth of bundles within bundles within bundles within...
   This is the size of a static array.  If you exceed this limit you'll 
   get an error message. */
#define MAX_BUNDLE_NESTING 32


/* Don't ever manipulate the data in the OSCbuf struct directly.  (It's
   declared here in the header file only so your program will be able to
   declare variables of type OSCbuf and have the right amount of memory
   be allocated.) */

typedef struct OSCbuf_struct {
    char *buffer;            /* The buffer to hold the OSC packet */
    int size;                /* Size of the buffer */
    char *bufptr;            /* Current position as we fill the buffer */
    int state;		     /* State of partially-constructed message */
    int4byte *thisMsgSize;   /* Pointer to count field before 
			        currently-being-written message */
    int4byte *prevCounts[MAX_BUNDLE_NESTING];
			     /* Pointers to count field before each currently
			        open bundle */
    int bundleDepth;	     /* How many sub-sub-bundles are we in now? */
    char *typeStringPtr;    /* This pointer advances through the type
			       tag string as you add arguments. */
    int gettingFirstUntypedArg;	/* nonzero if this message doesn't have
				   a type tag and we're waiting for the 1st arg */
} OSCbuf;



/* Initialize the given OSCbuf.  The user of this module must pass in the
   block of memory that this OSCbuf will use for a buffer, and the number of
   bytes in that block.  (It's the user's job to allocate the memory because
   you do it differently in different systems.) */
void OSC_initBuffer(OSCbuf *buf, int size, char *byteArray);


/* Reset the given OSCbuf.  Do this after you send out the contents of
   the buffer and want to start writing new data into it. */
void OSC_resetBuffer(OSCbuf *buf);


/* Is the buffer empty?  (I.e., would it be stupid to send the buffer
   contents to the synth?) */
int OSC_isBufferEmpty(OSCbuf *buf);


/* How much space is left in the buffer? */
int OSC_freeSpaceInBuffer(OSCbuf *buf);

/* Does the buffer contain a valid OSC packet?  (Returns nonzero if yes.) */
int OSC_isBufferDone(OSCbuf *buf);

/* When you're ready to send out the buffer (i.e., when OSC_isBufferDone()
   returns true), call these two procedures to get the OSC packet that's been
   assembled and its size in bytes.  (And then call OSC_resetBuffer() if you
   want to re-use this OSCbuf for the next packet.)  */
char *OSC_getPacket(OSCbuf *buf);
int OSC_packetSize(OSCbuf *buf);



/* Here's the basic model for building up OSC messages in an OSCbuf:

    - Make sure the OSCbuf has been initialized with OSC_initBuffer().

    - To open a bundle, call OSC_openBundle().  You can then write 
      messages or open new bundles within the bundle you opened.
      Call OSC_closeBundle() to close the bundle.  Note that a packet
      does not have to have a bundle; it can instead consist of just a 
      single message.
								  

    - For each message you want to send:

	- Call OSC_writeAddress() with the name of your message.  (In
	  addition to writing your message name into the buffer, this
	  procedure will also leave space for the size count of this message.)

        - Alternately, call OSC_writeAddressAndTypes() with the name of
          your message and with a type string listing the types of all the
          arguments you will be putting in this message.
	
	- Now write each of the arguments into the buffer, by calling one of:
	    OSC_writeFloatArg()
	    OSC_writeFloatArgs()
	    OSC_writeIntArg()
	    OSC_writeStringArg()

	- Now your message is complete; you can send out the buffer or you can
	  add another message to it.
*/

int OSC_openBundle(OSCbuf *buf, OSCTimeTag tt);
int OSC_closeBundle(OSCbuf *buf);
int OSC_closeAllBundles(OSCbuf *buf);

int OSC_writeAddress(OSCbuf *buf, char PROGMEM *name);
int OSC_writeAddressAndTypes(OSCbuf *buf, char PROGMEM *name, char PROGMEM *types);
int OSC_writeFloatArg(OSCbuf *buf, float arg);
int OSC_writeFloatArgs(OSCbuf *buf, int numFloats, float *args);
int OSC_writeIntArg(OSCbuf *buf, int4byte arg);
int OSC_writeStringArg(OSCbuf *buf, char PROGMEM *arg);

extern char *OSC_errorMessage;

/* How many bytes will be needed in the OSC format to hold the given
   string?  The length of the string, plus the null char, plus any padding
   needed for 4-byte alignment. */ 
int OSC_effectiveStringLength(char PROGMEM *string);
