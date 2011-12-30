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
#include <avr32/io.h>

#include "printf-stdarg.h"
#include "wl_fw.h"
#include "startup.h"
#include "nor_flash.h"

int main(void)
{
        U32 pos, len;

        startup_init();
        flash_init();
        printk("Memory check...");
        if (at45dbx_mem_check() == OK)
        	printk(" OK\n");
        else
        	printk(" FAIL\n");
        printk("Writing firmware data to flash\n");
        pos = 0;
        while (pos < fw_len) {
                if (fw_len - pos > SECTOR_SIZE)
                        len = SECTOR_SIZE;
                else
                        len = fw_len - pos;
                
                flash_write(pos, fw_buf + pos, len);
                pos += len;
        }

        printk("Verifying firmware data\n");
        pos = 0;
        while (pos < fw_len) {
                static U8 page_buf[SECTOR_SIZE];
                U32 i;

                if (fw_len - pos > SECTOR_SIZE)
                        len = SECTOR_SIZE;
                else
                        len = fw_len - pos;
                
                flash_read(pos, page_buf, len);

                for (i = 0; i < len; i++)
                    if (*(page_buf + i) != *(fw_buf + pos + i)) {
                        printk("Verify failed at byte %d, 0x%02x != 0x%02x\n",
                               pos + i, *(page_buf + i), *(fw_buf + pos + i));
                        return 0;
                    }
                        
                
                pos += len;
        }

        printk("Firmware successfully stored in flash!\n");
        return 0;
}
