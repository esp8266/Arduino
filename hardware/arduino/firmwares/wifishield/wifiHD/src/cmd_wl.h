/* This header file is part of the ATMEL AVR-UC3-SoftwareFramework-1.7.0 Release */

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
#ifndef CMD_CM_H
#define CMD_CM_H

#include <console.h>
#include "netif/wlif.h"

/*! A pointer to a struct of type "struct net_cfg" should be passed as
 *  the ctx pointer in the callbacks below. The struct must have a
 *  single instance per netif.
 */
#define _DNS_CMD_

cmd_state_t cmd_scan(int argc, char* argv[], void* ctx);
cmd_state_t cmd_connect(int argc, char* argv[], void* ctx);
cmd_state_t cmd_set_ip(int argc, char* argv[], void* ctx);
cmd_state_t cmd_setkey(int argc, char* argv[], void* ctx);
cmd_state_t cmd_status(int argc, char* argv[], void* ctx);
cmd_state_t cmd_power(int argc, char* argv[], void* ctx);
cmd_state_t cmd_psconf(int argc, char* argv[], void* ctx);
cmd_state_t cmd_setpass(int argc, char* argv[], void* ctx);
cmd_state_t cmd_delpass(int argc, char* argv[], void* ctx);
cmd_state_t cmd_debug(int argc, char* argv[], void* ctx);
cmd_state_t cmd_debug_toggle(int argc, char* argv[], void* ctx);
cmd_state_t cmd_statSpi(int argc, char* argv[], void* ctx);
cmd_state_t cmd_resetStatSpi(int argc, char* argv[], void* ctx);
cmd_state_t cmd_gethostbyname(int argc, char* argv[], void* ctx);
cmd_state_t cmd_setDnsServer(int argc, char* argv[], void* ctx);
cmd_state_t cmd_startTcpSrv(int argc, char* argv[], void* ctx);
#ifdef WFE_6_12
cmd_state_t cmd_ibss(int argc, char* argv[], void* ctx);
#endif


#endif
