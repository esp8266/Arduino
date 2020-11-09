/* Copyright (c) 2015-2016 Ivan Grokhotkov. All rights reserved.
 * This file is part of eboot bootloader.
 *
 * Redistribution and use is permitted according to the conditions of the
 * 3-clause BSD license to be found in the LICENSE file.
 */


#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "flash.h"
#include "eboot_command.h"
#include <uzlib.h>

extern unsigned char _gzip_dict;

#define SWRST do { (*((volatile uint32_t*) 0x60000700)) |= 0x80000000; } while(0);

extern void ets_wdt_enable(void);
extern void ets_wdt_disable(void);

// Converts bit of a string into a uint32
#define S(a,b,c,d) ( (((uint32_t)a) & 0xff) | (((uint32_t)b) << 8) | (((uint32_t)c) << 16) | (((uint32_t)d)<<24) )

int print_version(const uint32_t flash_addr)
{
    uint32_t ver;
    if (SPIRead(flash_addr + APP_START_OFFSET + sizeof(image_header_t) + sizeof(section_header_t), &ver, sizeof(ver))) {
        return 1;
    }
    // We don't have BSS and can't print from flash, so build up string
    // 4 chars at a time.  Smaller code than byte-wise assignment.
    uint32_t fmt[2];
    fmt[0] = S('v', '%', '0', '8');
    fmt[1] = S('x', '\n', 0, 0);
    ets_printf((const char*) fmt, ver);
    return 0;
}

int load_app_from_flash_raw(const uint32_t flash_addr)
{
    image_header_t image_header;
    uint32_t pos = flash_addr + APP_START_OFFSET;

    if (SPIRead(pos, &image_header, sizeof(image_header))) {
        return 1;
    }
    pos += sizeof(image_header);


    for (uint32_t section_index = 0;
        section_index < image_header.num_segments;
        ++section_index)
    {
        section_header_t section_header = {0};
        if (SPIRead(pos, &section_header, sizeof(section_header))) {
            return 2;
        }
        pos += sizeof(section_header);

        const uint32_t address = section_header.address;

        bool load = false;

        if (address < 0x40000000) {
            load = true;
        }

        if (address >= 0x40100000 && address < 0x40108000) {
            load = true;
        }

        if (address >= 0x60000000) {
            load = true;
        }

        if (!load) {
            pos += section_header.size;
            continue;
        }

        if (SPIRead(pos, (void*)address, section_header.size))
            return 3;

        pos += section_header.size;
    }

    asm volatile("" ::: "memory");
    asm volatile ("mov.n a1, %0\n"
        "mov.n a3, %1\n"
        "jx a3\n" : : "r" (0x3ffffff0), "r" (image_header.entry) );

    __builtin_unreachable(); // Save a few bytes by letting GCC know no need to pop regs/return
    return 0;
}

uint8_t read_flash_byte(const uint32_t addr)
{
    uint8_t __attribute__((aligned(4))) buff[4];
    SPIRead(addr & ~3, buff, 4);
    return buff[addr & 3];
}
unsigned char __attribute__((aligned(4))) uzlib_flash_read_cb_buff[4096];
uint32_t uzlib_flash_read_cb_addr;
int uzlib_flash_read_cb(struct uzlib_uncomp *m)
{
    m->source = uzlib_flash_read_cb_buff;
    m->source_limit = uzlib_flash_read_cb_buff + sizeof(uzlib_flash_read_cb_buff);
    SPIRead(uzlib_flash_read_cb_addr, uzlib_flash_read_cb_buff, sizeof(uzlib_flash_read_cb_buff));
    uzlib_flash_read_cb_addr += sizeof(uzlib_flash_read_cb_buff);
    return *(m->source++);
}

unsigned char gzip_dict[32768];
uint8_t buffer2[FLASH_SECTOR_SIZE]; // no room for this on the stack

int copy_raw(const uint32_t src_addr,
             const uint32_t dst_addr,
             const uint32_t size,
             const bool verify)
{
    // require regions to be aligned
    if ((src_addr & 0xfff) != 0 ||
        (dst_addr & 0xfff) != 0) {
        return 1;
    }

    const uint32_t buffer_size = FLASH_SECTOR_SIZE;
    uint8_t buffer[buffer_size];
    int32_t left = ((size+buffer_size-1) & ~(buffer_size-1));
    uint32_t saddr = src_addr;
    uint32_t daddr = dst_addr;
    struct uzlib_uncomp m_uncomp;
    bool gzip = false;

    // Check if we are uncompressing a GZIP upload or not
    if ((read_flash_byte(saddr) == 0x1f) && (read_flash_byte(saddr + 1) == 0x8b)) {
        // GZIP signature matched.  Find real size as encoded at the end
        left = read_flash_byte(saddr + size - 4);
        left += read_flash_byte(saddr + size - 3)<<8;
        left += read_flash_byte(saddr + size - 2)<<16;
        left += read_flash_byte(saddr + size - 1)<<24;

        uzlib_init();

        /* all 3 fields below must be initialized by user */
        m_uncomp.source = NULL;
        m_uncomp.source_limit = NULL;
        uzlib_flash_read_cb_addr = src_addr;
        m_uncomp.source_read_cb = uzlib_flash_read_cb;
        uzlib_uncompress_init(&m_uncomp, gzip_dict, sizeof(gzip_dict));

        int res = uzlib_gzip_parse_header(&m_uncomp);
        if (res != TINF_OK) {
            return 5; // Error uncompress header read
        }
	gzip = true;
    }
    while (left > 0) {
        if (!gzip) {
            if (SPIRead(saddr, buffer, buffer_size)) {
                return 3;
            }
        } else {
            m_uncomp.dest_start = buffer;
            m_uncomp.dest = buffer;
            int to_read = (left > buffer_size) ? buffer_size : left;
            m_uncomp.dest_limit = buffer + to_read;
            int res = uzlib_uncompress(&m_uncomp);
            if ((res != TINF_DONE) && (res != TINF_OK)) {
                return 6;
            }
            // Fill any remaining with 0xff
            for (int i = to_read; i < buffer_size; i++) {
                buffer[i] = 0xff;
            }
        }
        if (verify) {
            if (SPIRead(daddr, buffer2, buffer_size)) {
                return 4;
            }
            if (memcmp(buffer, buffer2, buffer_size)) {
                return 9;
            }
        } else {
            // Special treatment for address 0 (bootloader).  Only erase and
            // rewrite if the data is different (i.e. very rarely).
            bool skip = false;
            if (daddr == 0) {
                if (SPIRead(daddr, buffer2, buffer_size)) {
                    return 4;
                }
                if (!memcmp(buffer2, buffer, buffer_size)) {
                    ets_putc('B'); // Note we skipped the bootloader in output
                    skip = true;   // And skip erase/write
                }
            }
            if (!skip) {
                if (SPIEraseSector(daddr/buffer_size)) {
                   return 2;
                }
                if (SPIWrite(daddr, buffer, buffer_size)) {
                    return 4;
                }
            }
        }
        saddr += buffer_size;
        daddr += buffer_size;
        left  -= buffer_size;
    }

    return 0;
}

int main()
{
    int res = 9;
    bool clear_cmd = false;
    struct eboot_command cmd;

    print_version(0);

    if (eboot_command_read(&cmd) == 0) {
        // valid command was passed via RTC_MEM
        clear_cmd = true;
        ets_putc('@');
    } else {
        // no valid command found
        cmd.action = ACTION_LOAD_APP;
        cmd.args[0] = 0;
        ets_putc('~');
    }

    if (cmd.action == ACTION_COPY_RAW) {
        uint32_t cp = S('c', 'p', ':', 0);
        ets_printf((const char *)&cp);

        ets_wdt_disable();
        res = copy_raw(cmd.args[0], cmd.args[1], cmd.args[2], false);
        ets_wdt_enable();

        cp = S('0' + res, '\n', 0, 0 );
        ets_printf((const char *)&cp);
#if 0
	//devyte: this verify step below (cmp:) only works when the end of copy operation above does not overwrite the 
	//beginning of the image in the empty area, see #7458. Disabling for now. 
        //TODO: replace the below verify with hash type, crc, or similar.
        // Verify the copy
        uint32_t v[2];
        v[0] = S('c', 'm', 'p', ':');
        v[1] = 0;
        ets_printf(const char *)v);
        if (res == 0) {
            ets_wdt_disable();
            res = copy_raw(cmd.args[0], cmd.args[1], cmd.args[2], true);
            ets_wdt_enable();
            }

        cp = S('0' + res, '\n', 0, 0 );
        ets_printf((const char *)&cp);
#endif	    
        if (res == 0) {
            cmd.action = ACTION_LOAD_APP;
            cmd.args[0] = cmd.args[1];
        }
    }

    if (clear_cmd) {
        eboot_command_clear();
    }

    if (cmd.action == ACTION_LOAD_APP) {
        ets_putc('l'); ets_putc('d'); ets_putc('\n');
        res = load_app_from_flash_raw(cmd.args[0]);
        // We will get to this only on load fail
        uint32_t e[2];
        e[0] = S('e', ':', '0' + res, '\n' );
        e[1] = 0;
        ets_printf((const char*)e);
    }

    if (res) {
        SWRST;
    }

    while(true){}

    __builtin_unreachable();
    return 0;
}
