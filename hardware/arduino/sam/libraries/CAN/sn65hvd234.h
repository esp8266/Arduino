/*
  Copyright (c) 2013 Arduino.  All right reserved.

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

 /**
   *  \file
   *
   *  Include Defines & macros for the SN65HVD234.
   */

#ifndef _CAN_SN65HVD234_
#define _CAN_SN65HVD234_

#include "variant.h"

#ifdef __cplusplus
 extern "C" {
#endif


typedef struct _SSN65HVD234_Data
{
    /** Rs Pin on PIO */
    uint32_t dwPin_Rs ;

    /** EN Pin on PIO */
    uint32_t dwPin_EN ;
} SSN65HVD234_Data ;

extern uint32_t SN65HVD234_Init( SSN65HVD234_Data* pComponent ) ;
extern uint32_t SN65HVD234_SetRs( SSN65HVD234_Data* pComponent, uint32_t dwPin_Rs ) ;
extern uint32_t SN65HVD234_SetEN( SSN65HVD234_Data* pComponent, uint32_t dwPin_EN ) ;

extern uint32_t SN65HVD234_Enable( SSN65HVD234_Data* pComponent ) ;
extern uint32_t SN65HVD234_Disable( SSN65HVD234_Data* pComponent ) ;

extern uint32_t SN65HVD234_EnableLowPower( SSN65HVD234_Data* pComponent ) ;
extern uint32_t SN65HVD234_DisableLowPower( SSN65HVD234_Data* pComponent ) ;

#ifdef __cplusplus
}
#endif

#endif /* _CAN_SN65HVD234_ */
