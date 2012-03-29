/*
  Copyright (c) 2012 Arduino.  All right reserved.

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

#ifndef UDPHS_H_INCLUDED
#define UDPHS_H_INCLUDED

#define NUM_IT_MAX 3

#define EP_SINGLE_64 0x32	// EP0
#define EP_DOUBLE_64 0x36	// Other endpoints


// Endpoint 0:
#define EP_TYPE_CONTROL				UDPHS_EPTCFG_EPT_SIZE_64 \
                                  | UDPHS_EPTCFG_EPT_TYPE_CTRL8 \
                                  | UDPHS_EPTCFG_BK_NUMBER_1
#ifdef CDC_ENABLED
#define EP_TYPE_BULK_IN				UDPHS_EPTCFG_EPT_SIZE_512 \
                                  | UDPHS_EPTCFG_EPT_DIR \
                                  | UDPHS_EPTCFG_EPT_TYPE_BULK \
                                  | UDPHS_EPTCFG_BK_NUMBER_2
#define EP_TYPE_BULK_OUT			UDPHS_EPTCFG_EPT_SIZE_512 \
                                  | UDPHS_EPTCFG_EPT_TYPE_BULK \
                                  | UDPHS_EPTCFG_BK_NUMBER_2
#define EP_TYPE_INTERRUPT_IN		UDPHS_EPTCFG_EPT_SIZE_64 \
                                  | UDPHS_EPTCFG_EPT_DIR \
                                  | UDPHS_EPTCFG_EPT_TYPE_INT \
                                  | UDPHS_EPTCFG_BK_NUMBER_2
#endif

#ifdef HID_ENABLED
#define EP_TYPE_INTERRUPT_IN_HID	UDPHS_EPTCFG_EPT_SIZE_64 \
                                  | UDPHS_EPTCFG_EPT_DIR \
                                  | UDPHS_EPTCFG_EPT_TYPE_INT \
                                  | UDPHS_EPTCFG_BK_NUMBER_2
#endif

#define EP_TYPE_INTERRUPT_OUT		UDPHS_EPTCFG_EPT_SIZE_64 \
                                  | UDPHS_EPTCFG_EPT_TYPE_INT \
                                  | UDPHS_EPTCFG_EPT_TYPE_INT \
                                  | UDPHS_EPTCFG_BK_NUMBER_1
#define EP_TYPE_ISOCHRONOUS_IN		UDPHS_EPTCFG_EPT_SIZE_1024 \
                                  | UDPHS_EPTCFG_EPT_DIR \
                                  | UDPHS_EPTCFG_EPT_TYPE_ISO \
                                  | UDPHS_EPTCFG_BK_NUMBER_3
#define EP_TYPE_ISOCHRONOUS_OUT		UDPHS_EPTCFG_EPT_SIZE_1024 \
                                  | UDPHS_EPTCFG_EPT_TYPE_ISO \
                                  | UDPHS_EPTCFG_BK_NUMBER_3


#ifndef TXLED1
#define TXLED0
#define RXLED0
#define TXLED1 
#define RXLED1 
#endif


#define UDFNUML ((UDPHS->UDPHS_FNUM & UDPHS_FNUM_FRAME_NUMBER_Msk)>>3)

#define USB_RECV_TIMEOUT

#define UDPHS_EPTFIFO (0x20180000) // (UDPHS_EPTFIFO) Base Address

#endif /* UDPHS_H_INCLUDED */
