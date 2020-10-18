/*
  PrintBuffer.h - Library for ESP8266.
  Copyright (c) 2020 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <Arduino.h>

#include "PrintBuffer.h"

/*
 *  printBuffer - output a hex dump of a buffer
 *
 *  fd is file descriptor to write to
 *  data is pointer to the buffer
 *  length is length of buffer to write
 *  linelen is number of chars to output per line
 *  split is number of chars in each chunk on a line
 */
void printBuffer(void const *data, size_t length, int linelen, int split)
{
    char buffer[512];
    char *ptr;
    char *inptr;
    int pos;
    int remaining = length;

    inptr = (char *)data;

    // Loop through each line remaining
    while (remaining > 0) {
        int lrem;
        int splitcount;
        ptr = buffer;

        // Loop through the hex chars of this line
        lrem = remaining;
        splitcount = 0;
        for (pos = 0; pos < linelen; pos++) {
            // Split hex section if required
            if (split == splitcount++) {
                sprintf(ptr, "  ");
                ptr += 2;
                splitcount = 1;
            }

            // If still remaining chars, output, else leave a space
            if (lrem) {
                sprintf(ptr, "%.2X ", *((unsigned char *) inptr + pos));
                lrem--;
            } else {
                sprintf(ptr, "   ");
            }
            ptr += 3;
        }

        *ptr++ = ' ';
        *ptr++ = ' ';

        // Loop through the ASCII chars of this line
        lrem = remaining;
        splitcount = 0;
        for (pos = 0; pos < linelen; pos++) {
            unsigned char c;

            /* Split ASCII section if required */
            if (split == splitcount++) {
                sprintf(ptr, "  ");
                ptr += 2;
                splitcount = 1;
            }

            if (lrem) {
                c = *((unsigned char *)inptr + pos);
                if (c > 31 && c < 127) {
                    sprintf(ptr, "%c", c);
                } else {
                    sprintf(ptr, ".");
                }
                lrem--;
            }
            ptr++;
        }

        *ptr = '\0';
        Serial.println(buffer);

        inptr += linelen;
        remaining -= linelen;
    }
}
