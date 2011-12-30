#if defined(DATAFLASH)
/* This source file is part of the ATMEL AVR-UC3-SoftwareFramework-1.7.0 Release */

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
#include "wl_api.h"
#include "fw_download.h"
#include "nor_flash.h"
#include "printf-stdarg.h"

int fw_download_init(void)
{
       flash_init();
       return 0;
}

void fw_download_cb(void* ctx, uint8_t** buf, uint32_t* len)
{
	/* remember accross different calls */
        static uint8_t* fw_buf = NULL;
        static uint32_t offset = 0;

        //printk("Starting download... 0x%x-0x%x", *buf, *len);
        /* when firmware download is completed, this function will be invoked
         * on additional time with the input value of len set to 0. we can free
         * the firmware buffer at this time since it's no longer needed.
         */
        if (*len == 0) {
                if (fw_buf)
                        free(fw_buf);
                return;
        }

        /* first call? then initialize flash and allocate a buffer to hold
         * firmware data.
         */
        if (fw_buf == NULL) {
                fw_buf = malloc(SECTOR_SIZE);
                
                if (fw_buf == NULL) {
                        printk("could not allocate firmware buffer\n");
                        *len = 0;
                        return;
                }
        }

        /* decide how much to read. we know *len bytes remains, but we only have
         * room for SECTOR_SIEZ bytes in our buffer (fw_buf)
         */
	uint32_t fw_len = *len > SECTOR_SIZE ? SECTOR_SIZE : *len;
        
        /* read data and update output parameters */
        flash_read(offset, fw_buf, fw_len);
	*buf = fw_buf;
	*len = fw_len;

        /* we need to know where to start reading upon next call */
        offset += fw_len;
}
#endif
