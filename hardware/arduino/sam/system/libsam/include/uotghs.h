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

#ifndef UOTGHS_H_INCLUDED
#define UOTGHS_H_INCLUDED

#define EP_SINGLE_64 (0x32UL) // EP0
#define EP_DOUBLE_64 (0x36UL) // Other endpoints


// Control Endpoint
#define EP_TYPE_CONTROL           (UOTGHS_DEVEPTCFG_EPSIZE_64_BYTE | UOTGHS_DEVEPTCFG_EPTYPE_CTRL | UOTGHS_DEVEPTCFG_EPBK_1_BANK)

// CDC Endpoints
#ifdef CDC_ENABLED
#define EP_TYPE_BULK_IN           (UOTGHS_DEVEPTCFG_EPSIZE_512_BYTE | UOTGHS_DEVEPTCFG_EPDIR_IN | UOTGHS_DEVEPTCFG_EPTYPE_BLK | UOTGHS_DEVEPTCFG_EPBK_2_BANK)
#define EP_TYPE_BULK_OUT          (UOTGHS_DEVEPTCFG_EPSIZE_512_BYTE | UOTGHS_DEVEPTCFG_EPTYPE_BLK | UOTGHS_DEVEPTCFG_EPBK_2_BANK)
#define EP_TYPE_INTERRUPT_IN      (UOTGHS_DEVEPTCFG_EPSIZE_64_BYTE | UOTGHS_DEVEPTCFG_EPDIR_IN | UOTGHS_DEVEPTCFG_EPTYPE_INTRPT | UOTGHS_DEVEPTCFG_EPBK_2_BANK)
#endif

// HID Endpoints
#ifdef HID_ENABLED
#define EP_TYPE_INTERRUPT_IN_HID	(UOTGHS_DEVEPTCFG_EPSIZE_64_BYTE | UOTGHS_DEVEPTCFG_EPDIR_IN | UOTGHS_DEVEPTCFG_EPTYPE_INTRPT | UOTGHS_DEVEPTCFG_EPBK_2_BANK)
#endif

// Various definitions
#define EP_TYPE_INTERRUPT_OUT		  (UOTGHS_DEVEPTCFG_EPSIZE_64_BYTE | UOTGHS_DEVEPTCFG_EPTYPE_INTRPT | UOTGHS_DEVEPTCFG_EPTYPE_INTRPT | UOTGHS_DEVEPTCFG_EPBK_1_BANK)
#define EP_TYPE_ISOCHRONOUS_IN		(UOTGHS_DEVEPTCFG_EPSIZE_1024_BYTE | UOTGHS_DEVEPTCFG_EPDIR_IN | UOTGHS_DEVEPTCFG_EPTYPE_ISO | UOTGHS_DEVEPTCFG_EPBK_3_BANK)
#define EP_TYPE_ISOCHRONOUS_OUT		(UOTGHS_DEVEPTCFG_EPSIZE_1024_BYTE | UOTGHS_DEVEPTCFG_EPTYPE_ISO | UOTGHS_DEVEPTCFG_EPBK_3_BANK)


#endif /* UOTGHS_H_INCLUDED */
