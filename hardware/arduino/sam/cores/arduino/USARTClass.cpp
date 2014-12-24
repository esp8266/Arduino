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

USARTClass::USARTClass( Usart* pUsart, IRQn_Type dwIrq, uint32_t dwId, RingBuffer* pRx_buffer, RingBuffer* pTx_buffer ) : UARTClass((Uart*)pUsart, dwIrq, dwId, pRx_buffer, pTx_buffer)
{

  _pUsart=pUsart ; //In case anyone needs USART specific functionality in the future
}

// Public Methods //////////////////////////////////////////////////////////////

void USARTClass::begin( const uint32_t dwBaudRate )
{
	begin( dwBaudRate, SERIAL_8N1 );
}

void USARTClass::begin( const uint32_t dwBaudRate, const uint32_t config )
{
	UARTClass::begin(dwBaudRate, config);
}

void USARTClass::end( void )
{
	UARTClass::end();
}

void USARTClass::flush( void )
{
	UARTClass::flush();
}

size_t USARTClass::write( const uint8_t uc_data )
{
	return UARTClass::write(uc_data);
}

void USARTClass::IrqHandler( void )
{
	UARTClass::IrqHandler();
}

