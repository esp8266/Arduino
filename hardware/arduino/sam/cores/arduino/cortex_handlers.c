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

#include "Arduino.h"

#ifdef __cplusplus
extern "C" {
#endif

void NMI_Handler( void )
{
  for ( ;; ) ;
}

void HardFault_Handler( void )
{
  for ( ;; ) ;
}

void MemManage_Handler( void )
{
  for ( ;; ) ;
}

void BusFault_Handler( void )
{
  for ( ;; ) ;
}

void UsageFault_Handler( void )
{
  for ( ;; ) ;
}

void SVC_Handler( void )
{
  for ( ;; ) ;
}

void DebugMon_Handler( void )
{
  for ( ;; ) ;
}

void PendSV_Handler( void )
{
  for ( ;; ) ;
}

#ifdef __cplusplus
}
#endif

