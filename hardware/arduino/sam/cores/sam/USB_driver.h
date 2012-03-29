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

extern inline void USBD_WaitIN( void ) ;
extern inline void USBD_ClearIN( void ) ;
extern inline void USBD_WaitOUT( void ) ;
extern inline uint8_t USBD_WaitForINOrOUT( void ) ;
extern inline void USBD_ClearOUT(void) ;

extern void USBD_ClearRxFlag( unsigned char bEndpoint ) ;
extern void USBD_Recv( volatile uint8_t* data, uint8_t count ) ;

extern inline uint8_t USBD_Recv8( void ) ;
extern inline void USBD_Send8( uint8_t d ) ;
extern inline void USBD_SetEP( uint8_t ep ) ;
extern inline u16 USBD_FifoByteCount( void ) ;
extern inline uint8_t USBD_ReceivedSetupInt( void ) ;
extern inline void USBD_ClearSetupInt( void ) ;
extern inline void USBD_Stall( void ) ;
extern inline uint8_t USBD_ReadWriteAllowed( void ) ;
extern inline uint8_t USBD_Stalled( void ) ;

extern uint8_t USBD_FifoFree( void ) ;

extern inline void USBD_ReleaseRX( void ) ;
extern inline void USBD_ReleaseTX( void ) ;
extern inline uint8_t USBD_FrameNumber( void ) ;

extern uint8_t USBD_GetConfiguration( void ) ;


//	Number of bytes, assumes a rx endpoint
extern uint8_t USBD_Available( uint8_t ep ) ;

//	Non Blocking receive
//	Return number of bytes read
extern int USBD_Recv(uint8_t ep, void* d, int len) ;

//	Recv 1 byte if ready
extern int USBD_Recv(uint8_t ep) ;

//	Space in send EP
extern uint8_t USBD_SendSpace(uint8_t ep) ;

//	Blocking Send of data to an endpoint
extern int USBD_Send(uint8_t ep, const void* d, int len) ;
extern void USBD_InitEP(uint8_t index, uint8_t type, uint8_t size) ;
extern void USBD_InitEndpoints() ;

//	Handle CLASS_INTERFACE requests
extern bool USBD_ClassInterfaceRequest(Setup& setup) ;

extern void USBD_InitControl(int end) ;
extern void UDPHS_Handler( void ) ;

extern void USBD_Attach( void ) ;
extern void USBD_Detach( void ) ;

#endif /* _USB_DRIVER_*/
