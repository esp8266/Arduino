/* Header to define new/delete operators as they aren't provided by avr-gcc by default
   Taken from http://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&t=59453 
 */

#ifndef NEW_H
#define NEW_H

#include <stdlib.h>

void * operator new(size_t size);
void * operator new[](size_t size);
void operator delete(void * ptr);
void operator delete[](void * ptr);

#endif

