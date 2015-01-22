/* ----------------------------------------------------------------------------
 *         SAM Software Package License
 * ----------------------------------------------------------------------------
 * Copyright (c) 2011-2012, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following condition is met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

#ifndef USB_HOST_H_INCLUDED
#define USB_HOST_H_INCLUDED

#include <stdint.h>

#define tokSETUP		UOTGHS_HSTPIPCFG_PTOKEN_SETUP
#define tokIN			UOTGHS_HSTPIPCFG_PTOKEN_IN
#define tokOUT			UOTGHS_HSTPIPCFG_PTOKEN_OUT
#define tokINHS			UOTGHS_HSTPIPCFG_PTOKEN_IN
#define tokOUTHS		UOTGHS_HSTPIPCFG_PTOKEN_OUT

//! \brief Device speed
/*typedef enum {
	UHD_SPEED_LOW  = 0,
	UHD_SPEED_FULL = 1,
	UHD_SPEED_HIGH = 2,
} uhd_speed_t;*/

//! States of USBB interface
typedef enum {
	UHD_STATE_NO_VBUS = 0,
	UHD_STATE_DISCONNECTED = 1,
	UHD_STATE_CONNECTED = 2,
	UHD_STATE_ERROR = 3,
} uhd_vbus_state_t;

//extern uhd_speed_t uhd_get_speed(void);

extern void UHD_SetStack(void (*pf_isr)(void));
extern void UHD_Init(void);
extern void UHD_BusReset(void);
extern uhd_vbus_state_t UHD_GetVBUSState(void);
extern uint32_t UHD_Pipe0_Alloc(uint32_t ul_add, uint32_t ul_ep_size);
extern uint32_t UHD_Pipe_Alloc(uint32_t ul_dev_addr, uint32_t ul_dev_ep, uint32_t ul_type, uint32_t ul_dir, uint32_t ul_maxsize, uint32_t ul_interval, uint32_t ul_nb_bank);
extern void UHD_Pipe_Free(uint32_t ul_pipe);
extern uint32_t UHD_Pipe_Read(uint32_t ul_pipe, uint32_t ul_size, uint8_t* data);
extern void UHD_Pipe_Write(uint32_t ul_pipe, uint32_t ul_size, uint8_t* data);
extern void UHD_Pipe_Send(uint32_t ul_pipe, uint32_t ul_token_type);
extern uint32_t UHD_Pipe_Is_Transfer_Complete(uint32_t ul_pipe, uint32_t ul_token_type);

#endif /* USB_HOST_H_INCLUDED */
