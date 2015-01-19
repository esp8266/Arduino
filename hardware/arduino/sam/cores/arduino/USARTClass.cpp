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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "USARTClass.h"

// Constructors ////////////////////////////////////////////////////////////////

USARTClass::USARTClass( Usart* pUsart, IRQn_Type dwIrq, uint32_t dwId, RingBuffer* pRx_buffer, RingBuffer* pTx_buffer )
  : UARTClass((Uart*)pUsart, dwIrq, dwId, pRx_buffer, pTx_buffer)
{
  // In case anyone needs USART specific functionality in the future
  _pUsart=pUsart;
}

// Public Methods //////////////////////////////////////////////////////////////

void USARTClass::begin(const uint32_t dwBaudRate)
{
  begin(dwBaudRate, Mode_8N1);
}

void USARTClass::begin(const uint32_t dwBaudRate, const UARTModes config)
{
  uint32_t modeReg = static_cast<uint32_t>(config);
  modeReg |= US_MR_USART_MODE_NORMAL | US_MR_USCLKS_MCK | US_MR_CHMODE_NORMAL;
  init(dwBaudRate, modeReg);
}

void USARTClass::begin(const uint32_t dwBaudRate, const USARTModes config)
{
  uint32_t modeReg = static_cast<uint32_t>(config);
  modeReg |= US_MR_USART_MODE_NORMAL | US_MR_USCLKS_MCK | US_MR_CHMODE_NORMAL;
  init(dwBaudRate, modeReg);
}

