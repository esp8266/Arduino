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
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <printf-stdarg.h>
#include <console.h>
#include <board_init.h>
#include <usart.h>

#define MAX_CMD_CONSOLE_NUM 15
struct {
        cmd_cb_t cb;
        const char* str;
        void* ctx;
} cmd_list[MAX_CMD_CONSOLE_NUM] = { { 0 } };

#ifndef CMD_MAX_LEN
#define CMD_MAX_LEN 80
#endif
extern int board_putchar(char c);
int io_getc(char *c)
{
        int ci;
        int status;
        status = usart_read_char(&CONFIG_CONSOLE_PORT, &ci);
        if (status == USART_RX_EMPTY)
                return 1;
        
        if (status == USART_RX_ERROR) {
                CONFIG_CONSOLE_PORT.cr = AVR32_USART_CR_RSTSTA_MASK;
                return 1;
        }

        if (ci == '\r') {
                board_putchar('\n');
        /* Echo char. */
        } else if (ci == '\b') {
                board_putchar(ci);
                board_putchar(' ');
                board_putchar(ci);
        } else
                board_putchar(ci);
        

        *c = ci;
        return 0;
}

static uint8_t is_initialized = 0;

char* console_gets()
{
        static char buf[CMD_MAX_LEN];
        static int pos = 0;
        char c;
        
        for (;;) {
                if (io_getc(&c))
                        return NULL;
                
                if (c == '\r' || c == '\n') {
                        buf[pos] = 0;
                        pos = 0;
                        return buf;
                }
                if (c == '\b') {
                        pos -= 1;
                        if (pos < 0) pos = 0;
                        buf[pos] = 0;
                }
                else
                        buf[pos++] = c;
                if (pos == sizeof(buf))
                        pos = 0;
        }
        return NULL;
}

int console_add_cmd(const char* str, cmd_cb_t cb, void* ctx)
{
        uint32_t i;
        for (i = 0; i < ARRAY_SIZE(cmd_list); i++)
                if (!cmd_list[i].cb)
                        break;
        
        if (i == ARRAY_SIZE(cmd_list))
                return -1;
        
        cmd_list[i].str = str;
        cmd_list[i].cb = cb;
        cmd_list[i].ctx = ctx;
        return 0;
}

void console_init(void)
{
        printk("\n$ ");
        is_initialized = 1;
}

void console_init_silent(void) {
        is_initialized = 1;
}

int console_schedule_cmd(char *cmd, int interactive) {
#define MAX_ARGS 16
        static int argc, i;
        static char* argv[MAX_ARGS];
        static char *buf;
        static enum { INPUT, RUN } state = INPUT;

        switch (state) {
        case INPUT: {
                char* token;
                if (NULL == cmd) {
                        return 0;
                }
                buf = strdup(cmd);
                if (!buf)
                        return 0;
                if (!strlen(buf)) {
                        interactive ? printk("$ ") : 0;
                        free(buf);
                        return 0;
                }
#ifdef WIFI_DEBUG_ON
                printk("%s : Scheduling command \"%s\"\n",
                       __func__,
                       buf);
#endif
                for (i = 0; i < ARRAY_SIZE(cmd_list); i++)
                        if(cmd_list[i].str && !strncmp(cmd_list[i].str, buf, min(strlen(cmd_list[i].str), strlen(buf))))
                                break;

                if (ARRAY_SIZE(cmd_list) == 0) {
                        printk("No commands available. Is the WiFi card responding?\n");
                }
                if (i == ARRAY_SIZE(cmd_list)) {
                        if (interactive) {
                                printk("available commands:\n");
                                for (i = 0; i < ARRAY_SIZE(cmd_list); i++)
                                        if (cmd_list[i].cb)
                                                printk("  %s\n", cmd_list[i].str);
                                printk("$ ");
                        }
                        free(buf);
                        return 0;
                }
                
                for (token = strtok(buf, " "); token != NULL;
                     token = strtok(NULL, " ")) {
                        argv[argc] = token;
                        argc++;
                        if (argc == MAX_ARGS)
                                break;
                }
                
                state = RUN;
        } /* fall through */
                
        case RUN: {
                cmd_state_t s = cmd_list[i].cb(argc, argv, cmd_list[i].ctx);
                if (s == CMD_INPROGRESS)
                        return 1;

                interactive ? printk("$ ") : 0;

                argc = 0;
                memset(argv, 0, sizeof argv);
                free(buf);
                state = INPUT;
        }
        }

        return 1;
}


void console_poll(void)
{
        char *buf;
        buf = console_gets();
        console_schedule_cmd(buf, 1);
}
