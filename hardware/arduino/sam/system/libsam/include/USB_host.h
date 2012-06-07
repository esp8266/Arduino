/*
  Copyright (c) 2011 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
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
extern uint32_t UHD_EP0_Alloc(uint32_t ul_add, uint32_t ul_ep_size);
extern void UHD_EP_Free(uint32_t add, uint32_t endp);
extern uint32_t UHD_EP_Read(uint32_t ul_ep, uint32_t ul_size, uint8_t* data);
extern void UHD_EP_Write(uint32_t ul_ep, uint32_t ul_size, uint8_t* data);
extern void UHD_EP_Send(uint32_t ul_ep, uint32_t ul_token_type);
extern uint32_t UHD_EP_Is_Transfer_Complete(uint32_t ul_ep, uint32_t ul_token_type);

#endif /* USB_HOST_H_INCLUDED */
