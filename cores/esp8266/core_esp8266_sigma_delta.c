/*
  core_esp8266_sigma_delta.c - sigma delta library for esp8266
  
  Copyright (c) 2014 Ivan Grokhotkov. All rights reserved.
  This file is part of the esp8266 core for Arduino environment.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "Arduino.h" // using pinMode

// definitions in esp8266_peri.h style
#define GPSD  ESP8266_REG(0x368) // GPIO_SIGMA_DELTA register @ 0x600000368
#define GPSDT 0  // target, 8 bits
#define GPSDP 8  // prescaler, 8 bits
#define GPSDE 16 // enable

void sigmaDeltaSetPrescaler(uint8_t prescaler); // avoids compiler warning

/******************************************************************************
 * FunctionName : sigmaDeltaEnable
 * Description  : enable the internal sigma delta source
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR sigmaDeltaEnable()
{
  GPSD = (0 << GPSDT) | (0 << GPSDP) | (1 << GPSDE); //SIGMA_DELTA_TARGET(0) | SIGMA_DELTA_PRESCALER(0) | SIGMA_DELTA_ENABLE(ENABLED)
}

/******************************************************************************
 * FunctionName : sigmaDeltaDisable
 * Description  : stop the internal sigma delta source
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR sigmaDeltaDisable()
{
  GPSD = (0 << GPSDT) | (0 << GPSDP) | (0 << GPSDE); //SIGMA_DELTA_TARGET(0) | SIGMA_DELTA_PRESCALER(0) | SIGMA_DELTA_ENABLE(DISABLED)
}

/******************************************************************************
 * FunctionName : sigmaDeltaAttachPin
 * Description  : connects the sigma delta source to a physical output pin
 * Parameters   : pin (0..15), channel = unused, for compatibility with ESP32
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR sigmaDeltaAttachPin(uint8_t pin, uint8_t channel)
{
  (void) channel;
  // make the chosen pin an output pin
  pinMode (pin, OUTPUT);
  if (pin < 16) {
    // set its source to the sigma delta source
    GPC(pin) |= (1 << GPCS); //SOURCE 0:GPIO_DATA,1:SigmaDelta
  }
}

/******************************************************************************
 * FunctionName : sigmaDeltaDetachPin
 * Description  : disconnects the sigma delta source from a physical output pin
 * Parameters   : pin (0..16)
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR sigmaDeltaDetachPin(uint8_t pin)
{
  if (pin < 16) {
    // set its source to the sigma delta source
    GPC(pin) &=  ~(1 << GPCS); //SOURCE 0:GPIO_DATA,1:SigmaDelta
  }
}

/******************************************************************************
 * FunctionName : sigmaDeltaIsPinAttached
 * Description  : query if pin is attached
 * Parameters   : pin (0..16)
 * Returns      : bool
*******************************************************************************/
bool ICACHE_FLASH_ATTR sigmaDeltaIsPinAttached(uint8_t pin)
{
  if (pin < 16) {
    // set its source to the sigma delta source
     return (GPC(pin) & (1 << GPCS)); //SOURCE 0:GPIO_DATA,1:SigmaDelta
  }
  else
    return false;
}

/******************************************************************************
 * FunctionName : sigmaDeltaSetup
 * Description  : start the sigma delta generator with the chosen parameters
 * Parameters   : channel = unused (for compatibility with ESP32), 
 *                freq : 1220-312500 (lowest frequency in the output signal's spectrum)
 * Returns      : uint32_t the actual frequency, closest to the input parameter
*******************************************************************************/
uint32_t ICACHE_FLASH_ATTR sigmaDeltaSetup(uint8_t channel, uint32_t freq)
{
  (void) channel;
  
  uint32_t prescaler = ((uint32_t)10000000/(freq*32)) - 1;
  
  if(prescaler > 0xFF) {
      prescaler = 0xFF;
  }
  sigmaDeltaEnable();
  sigmaDeltaSetPrescaler ((uint8_t) prescaler);
  
  return 10000000/((prescaler + 1) * 32);
}

/******************************************************************************
 * FunctionName : sigmaDeltaWrite
 * Description  : set the duty cycle for the sigma-delta source
 * Parameters   : uint8 duty, 0-255, duty cycle = target/256, 
 *                channel = unused, for compatibility with ESP32
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR sigmaDeltaWrite(uint8_t channel, uint8_t duty)
{
  uint32_t reg = GPSD;
  (void) channel;

  reg = (reg & ~(0xFF << GPSDT)) | ((duty & 0xFF) << GPSDT);
  GPSD = reg;
  
}
/******************************************************************************
 * FunctionName : sigmaDeltaRead
 * Description  : get the duty cycle for the sigma-delta source
 * Parameters   : channel = unused, for compatibility with ESP32
 * Returns      : uint8_t duty cycle value 0..255
*******************************************************************************/
uint8_t ICACHE_FLASH_ATTR sigmaDeltaRead(uint8_t channel)
{
  (void) channel;
  return (uint8_t)((GPSD >> GPSDT) & 0xFF);
}

/******************************************************************************
 * FunctionName : sigmaDeltaSetPrescaler
 * Description  : set the clock divider for the sigma-delta source
 * Parameters   : uint8 prescaler, 0-255, divides the 80MHz base clock by this amount
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR sigmaDeltaSetPrescaler(uint8_t prescaler)
{
  uint32_t reg = GPSD;

  reg = (reg & ~(0xFF << GPSDP)) | ((prescaler & 0xFF) << GPSDP);
  GPSD = reg;
}

/******************************************************************************
 * FunctionName : sigmaDeltaGetPrescaler
 * Description  : get the prescaler value from the GPIO_SIGMA_DELTA register
 * Parameters   : none
 * Returns      : uint8 prescaler, CLK_DIV , 0-255
*******************************************************************************/
uint8_t ICACHE_FLASH_ATTR sigmaDeltaGetPrescaler(void)
{
  return (uint8_t)((GPSD >> GPSDP) & 0xFF);
}
