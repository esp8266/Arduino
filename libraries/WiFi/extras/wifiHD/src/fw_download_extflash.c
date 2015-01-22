/*! \page License
 * Copyright (C) 2009, H&D Wireless AB All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of H&D Wireless AB may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY H&D WIRELESS AB ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <top_defs.h>
#include <wl_api.h>
#include <fw_download.h>
#include <nvram.h>
#include <printf-stdarg.h>

int fw_download_init(void)
{
        nvram_init();
        return 0;
}

#define BUF_SIZE 512


size_t fw_read_cb(void* ctx, 
                  const uint8_t** buf, 
                  size_t offset, 
                  size_t len)
{
        static uint8_t* fw_buf = NULL;
        size_t rlen;
        /* when firmware download is completed, this function will be
         * invoked one additional time with buf set to NULL. we can
         * free the firmware buffer at this time since it's no longer
         * needed.
         */
        if (NULL == buf) {
                if (fw_buf) {
                        free(fw_buf);
                        fw_buf = NULL;
                }
                return 0;
        }

        /* first call? then initialize flash and allocate a buffer to hold
         * firmware data.
         */
        if (fw_buf == NULL) {
                fw_buf = malloc(BUF_SIZE);
                
                if (fw_buf == NULL) {
                        printk("could not allocate firmware buffer\n");
                        return 0;
                }
        }
        /* read at most a full buffer */
	rlen = len > BUF_SIZE ? BUF_SIZE : len;
        
        /* read data and update output parameters */
        nvram_read(offset, fw_buf, rlen);
	*buf = fw_buf;

        return rlen;
}
