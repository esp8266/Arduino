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

#ifndef _USART_CLASS_
#define _USART_CLASS_

#include "UARTClass.h"
#include "RingBuffer.h"

// Includes Atmel CMSIS
#include <chip.h>

// Define config for Serial.begin(baud, config);
#define SERIAL_5N1 USARTClass::Mode_5N1
#define SERIAL_6N1 USARTClass::Mode_6N1
#define SERIAL_7N1 USARTClass::Mode_7N1
#define SERIAL_5N2 USARTClass::Mode_5N2
#define SERIAL_6N2 USARTClass::Mode_6N2
#define SERIAL_7N2 USARTClass::Mode_7N2
#define SERIAL_8N2 USARTClass::Mode_8N2
#define SERIAL_5E1 USARTClass::Mode_5E1
#define SERIAL_6E1 USARTClass::Mode_6E1
#define SERIAL_7E1 USARTClass::Mode_7E1
#define SERIAL_5E2 USARTClass::Mode_5E2
#define SERIAL_6E2 USARTClass::Mode_6E2
#define SERIAL_7E2 USARTClass::Mode_7E2
#define SERIAL_8E2 USARTClass::Mode_8E2
#define SERIAL_5O1 USARTClass::Mode_5O1
#define SERIAL_6O1 USARTClass::Mode_6O1
#define SERIAL_7O1 USARTClass::Mode_7O1
#define SERIAL_5O2 USARTClass::Mode_5O2
#define SERIAL_6O2 USARTClass::Mode_6O2
#define SERIAL_7O2 USARTClass::Mode_7O2
#define SERIAL_8O2 USARTClass::Mode_8O2
#define SERIAL_5M1 USARTClass::Mode_5M1
#define SERIAL_6M1 USARTClass::Mode_6M1
#define SERIAL_7M1 USARTClass::Mode_7M1
#define SERIAL_5M2 USARTClass::Mode_5M2
#define SERIAL_6M2 USARTClass::Mode_6M2
#define SERIAL_7M2 USARTClass::Mode_7M2
#define SERIAL_8M2 USARTClass::Mode_8M2
#define SERIAL_5S1 USARTClass::Mode_5S1
#define SERIAL_6S1 USARTClass::Mode_6S1
#define SERIAL_7S1 USARTClass::Mode_7S1
#define SERIAL_5S2 USARTClass::Mode_5S2
#define SERIAL_6S2 USARTClass::Mode_6S2
#define SERIAL_7S2 USARTClass::Mode_7S2
#define SERIAL_8S2 USARTClass::Mode_8S2


class USARTClass : public UARTClass
{
  public:
    // 8x1 bit modes are inherited from UARTClass
    enum USARTModes {
      Mode_5N1 = US_MR_CHRL_5_BIT | US_MR_PAR_NO    | US_MR_NBSTOP_1_BIT,
      Mode_6N1 = US_MR_CHRL_6_BIT | US_MR_PAR_NO    | US_MR_NBSTOP_1_BIT,
      Mode_7N1 = US_MR_CHRL_7_BIT | US_MR_PAR_NO    | US_MR_NBSTOP_1_BIT,
      Mode_5N2 = US_MR_CHRL_5_BIT | US_MR_PAR_NO    | US_MR_NBSTOP_2_BIT,
      Mode_6N2 = US_MR_CHRL_6_BIT | US_MR_PAR_NO    | US_MR_NBSTOP_2_BIT,
      Mode_7N2 = US_MR_CHRL_7_BIT | US_MR_PAR_NO    | US_MR_NBSTOP_2_BIT,
      Mode_8N2 = US_MR_CHRL_8_BIT | US_MR_PAR_NO    | US_MR_NBSTOP_2_BIT,
      Mode_5E1 = US_MR_CHRL_5_BIT | US_MR_PAR_EVEN  | US_MR_NBSTOP_1_BIT,
      Mode_6E1 = US_MR_CHRL_6_BIT | US_MR_PAR_EVEN  | US_MR_NBSTOP_1_BIT,
      Mode_7E1 = US_MR_CHRL_7_BIT | US_MR_PAR_EVEN  | US_MR_NBSTOP_1_BIT,
      Mode_5E2 = US_MR_CHRL_5_BIT | US_MR_PAR_EVEN  | US_MR_NBSTOP_2_BIT,
      Mode_6E2 = US_MR_CHRL_6_BIT | US_MR_PAR_EVEN  | US_MR_NBSTOP_2_BIT,
      Mode_7E2 = US_MR_CHRL_7_BIT | US_MR_PAR_EVEN  | US_MR_NBSTOP_2_BIT,
      Mode_8E2 = US_MR_CHRL_8_BIT | US_MR_PAR_EVEN  | US_MR_NBSTOP_2_BIT,
      Mode_5O1 = US_MR_CHRL_5_BIT | US_MR_PAR_ODD   | US_MR_NBSTOP_1_BIT,
      Mode_6O1 = US_MR_CHRL_6_BIT | US_MR_PAR_ODD   | US_MR_NBSTOP_1_BIT,
      Mode_7O1 = US_MR_CHRL_7_BIT | US_MR_PAR_ODD   | US_MR_NBSTOP_1_BIT,
      Mode_5O2 = US_MR_CHRL_5_BIT | US_MR_PAR_ODD   | US_MR_NBSTOP_2_BIT,
      Mode_6O2 = US_MR_CHRL_6_BIT | US_MR_PAR_ODD   | US_MR_NBSTOP_2_BIT,
      Mode_7O2 = US_MR_CHRL_7_BIT | US_MR_PAR_ODD   | US_MR_NBSTOP_2_BIT,
      Mode_8O2 = US_MR_CHRL_8_BIT | US_MR_PAR_ODD   | US_MR_NBSTOP_2_BIT,
      Mode_5M1 = US_MR_CHRL_5_BIT | US_MR_PAR_MARK  | US_MR_NBSTOP_1_BIT,
      Mode_6M1 = US_MR_CHRL_6_BIT | US_MR_PAR_MARK  | US_MR_NBSTOP_1_BIT,
      Mode_7M1 = US_MR_CHRL_7_BIT | US_MR_PAR_MARK  | US_MR_NBSTOP_1_BIT,
      Mode_5M2 = US_MR_CHRL_5_BIT | US_MR_PAR_MARK  | US_MR_NBSTOP_2_BIT,
      Mode_6M2 = US_MR_CHRL_6_BIT | US_MR_PAR_MARK  | US_MR_NBSTOP_2_BIT,
      Mode_7M2 = US_MR_CHRL_7_BIT | US_MR_PAR_MARK  | US_MR_NBSTOP_2_BIT,
      Mode_8M2 = US_MR_CHRL_8_BIT | US_MR_PAR_MARK  | US_MR_NBSTOP_2_BIT,
      Mode_5S1 = US_MR_CHRL_5_BIT | US_MR_PAR_SPACE | US_MR_NBSTOP_1_BIT,
      Mode_6S1 = US_MR_CHRL_6_BIT | US_MR_PAR_SPACE | US_MR_NBSTOP_1_BIT,
      Mode_7S1 = US_MR_CHRL_7_BIT | US_MR_PAR_SPACE | US_MR_NBSTOP_1_BIT,
      Mode_5S2 = US_MR_CHRL_5_BIT | US_MR_PAR_SPACE | US_MR_NBSTOP_2_BIT,
      Mode_6S2 = US_MR_CHRL_6_BIT | US_MR_PAR_SPACE | US_MR_NBSTOP_2_BIT,
      Mode_7S2 = US_MR_CHRL_7_BIT | US_MR_PAR_SPACE | US_MR_NBSTOP_2_BIT,
      Mode_8S2 = US_MR_CHRL_8_BIT | US_MR_PAR_SPACE | US_MR_NBSTOP_2_BIT,
    };

    USARTClass(Usart* pUsart, IRQn_Type dwIrq, uint32_t dwId, RingBuffer* pRx_buffer, RingBuffer* pTx_buffer);

    void begin(const uint32_t dwBaudRate);
    void begin(const uint32_t dwBaudRate, const USARTModes config);
    void begin(const uint32_t dwBaudRate, const UARTModes config);

  protected:
    Usart* _pUsart;
};

#endif // _USART_CLASS_
