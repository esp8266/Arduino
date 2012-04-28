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

#ifndef _USB_DRIVER_
#define _USB_DRIVER_

#include <stdint.h>

extern void USBD_WaitIN(void) ;
extern void USBD_WaitOUT(void) ;
extern void USBD_ClearIN(void) ;
extern void USBD_ClearOUT(void) ;
extern uint8_t USBD_WaitForINOrOUT(void) ;

extern void USBD_ClearRxFlag( unsigned char bEndpoint ) ;

extern void USBD_Stall(void) ;
extern uint8_t USBD_Stalled(void) ;

extern uint8_t USBD_ReceivedSetupInt(void) ;
extern void USBD_ClearSetupInt(void) ;

extern uint8_t USBD_ReadWriteAllowed(void) ;

extern void USBD_SetEP( uint8_t ep ) ;
extern uint16_t USBD_FifoByteCount(void) ;
extern uint8_t USBD_FifoFree(void) ;

extern void USBD_ReleaseRX(void) ;
extern void USBD_ReleaseTX(void) ;
extern uint8_t USBD_FrameNumber(void) ;

extern uint8_t USBD_GetConfiguration(void) ;

extern void USBD_Recv( volatile uint8_t* data, uint8_t count ) ;
extern uint8_t USBD_Recv8(void) ;
extern void USBD_Send8( uint8_t d ) ;
//	Blocking Send of data to an endpoint
extern int USBD_Send(uint8_t ep, const void* d, int len) ;
//	Space in send EP
extern uint8_t USBD_SendSpace(uint8_t ep) ;

//	Number of bytes, assumes a rx endpoint
extern uint8_t USBD_Available(uint8_t ep) ;


extern void USBD_InitEP(uint8_t index, uint8_t type, uint8_t size) ;
extern void USBD_InitEndpoints(void) ;

extern void USBD_InitControl(int end) ;

extern void USBD_Attach(void) ;
extern void USBD_Detach(void) ;

#endif /* _USB_DRIVER_*/
