/*
/******************************************************************************
 * Sigma delta module

This module controls the esp8266 internal sigma delta source
Each pin can be connected to the sigma delta source
The target duty and frequency can be modified via the register GPIO_SIGMA_DELTA

THE TARGET FREQUENCY IS DEFINED AS:

FREQ = 80,000,000/prescaler * target /256  HZ,     0<target<128
FREQ = 80,000,000/prescaler * (256-target) /256  HZ,     128<target<256
target: duty ,0-255
prescaler: clk_div,0-255
so the target and prescale will both affect the freq.

Usage :
1. sigma_delta_enable() : activate the sigma delta source with default prescalar (0) & target (0)
2. sigma_delta_attachPin(pin), any pin 0..15, TBC if gpio16 supports sigma-delta source
     This will set the pin to NORMAL output mode (pinMode(pin,OUTPUT))
3. sigma_delta_setPrescaler(uint8_t) : reduce the output frequencies
4. sigma_delta_setTarget(uint8_t) : set the output signal duty cycle, duty cycle = target/256

5. sigma_delta_detachPin(pin), this will revert the pin to NORMAL output mode & GPIO source. 
The sigma delta source remains on until :
6. sigma_delta_disable()

*******************************************************************************/

#include "Arduino.h" // using pinMode

// definitions in esp8266_peri.h style
#define GPSD  ESP8266_REG(0x368) // GPIO_SIGMA_DELTA register @ 0x600000368
#define GPSDT 0  // target, 8 bits
#define GPSDP 8  // prescaler, 8 bits
#define GPSDE 16 // enable

/******************************************************************************
 * FunctionName : sigma_delta_start
 * Description  : enable the internal sigma delta source
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR sigma_delta_enable()
{
  GPSD = (0 << GPSDT) | (0 << GPSDP) | (1 << GPSDE); //SIGMA_DELTA_TARGET(0) | SIGMA_DELTA_PRESCALER(0) | SIGMA_DELTA_ENABLE(ENABLED)
}

/******************************************************************************
 * FunctionName : sigma_delta_disable
 * Description  : stop the internal sigma delta source
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR sigma_delta_disable()
{
  GPSD = (0 << GPSDT) | (0 << GPSDP) | (0 << GPSDE); //SIGMA_DELTA_TARGET(0) | SIGMA_DELTA_PRESCALER(0) | SIGMA_DELTA_ENABLE(DISABLED)
}

/******************************************************************************
 * FunctionName : sigma_delta_attachPin
 * Description  : connects the sigma delta source to a physical output pin
 * Parameters   : pin (0..15)
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR sigma_delta_attachPin(uint8_t pin)
{
  // make the chosen pin an output pin
  pinMode (pin, OUTPUT);
  if (pin < 16) {
    // set its source to the sigma delta source
    GPC(pin) |= (1 << GPCS); //SOURCE 0:GPIO_DATA,1:SigmaDelta
  }
}

/******************************************************************************
 * FunctionName : sigma_delta_detachPin
 * Description  : disconnects the sigma delta source from a physical output pin
 * Parameters   : pin (0..16)
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR sigma_delta_detachPin(uint8_t pin)
{
  if (pin < 16) {
    // set its source to the sigma delta source
    GPC(pin) &=  ~(1 << GPCS); //SOURCE 0:GPIO_DATA,1:SigmaDelta
  }
}

/******************************************************************************
 * FunctionName : sigma_delta_isPinAttached
 * Description  : query if pin is attached
 * Parameters   : pin (0..16)
 * Returns      : bool
*******************************************************************************/
bool ICACHE_FLASH_ATTR sigma_delta_isPinAttached(uint8_t pin)
{
  if (pin < 16) {
    // set its source to the sigma delta source
     return (GPC(pin) & (1 << GPCS)); //SOURCE 0:GPIO_DATA,1:SigmaDelta
  }
  else
    return false;
}

/******************************************************************************
 * FunctionName : sigma_delta_getTarget
 * Description  : get the target value from the GPIO_SIGMA_DELTA register
 * Parameters   : none
 * Returns      : uint8_t target value 0..255
*******************************************************************************/
uint8_t ICACHE_FLASH_ATTR sigma_delta_getTarget(void)
{
  return (uint8_t)((GPSD >> GPSDT) & 0xFF);
}

/******************************************************************************
 * FunctionName : sigma_delta_setTarget
 * Description  : set the target (duty cycle) for the sigma-delta source
 * Parameters   : uint8 target, 0-255, duty cycle = target/256
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR sigma_delta_setTarget(uint8_t target)
{
  uint32_t reg = GPSD;

  reg = (reg & ~(0xFF << GPSDT)) | ((target & 0xFF) << GPSDT);
  GPSD = reg;
}

/******************************************************************************
 * FunctionName : sigma_delta_getPrescaler
 * Description  : get the prescaler value from the GPIO_SIGMA_DELTA register
 * Parameters   : none
 * Returns      : uint8 prescaler, CLK_DIV , 0-255
*******************************************************************************/
uint8_t ICACHE_FLASH_ATTR sigma_delta_getPrescaler(uint8_t prescaler)
{
  return (uint8_t)((GPSD >> GPSDP) & 0xFF);
}

/******************************************************************************
 * FunctionName : sigma_delta_setPrescaler
 * Description  : set the clock divider for the sigma-delta source
 * Parameters   : uint8 prescaler, 0-255, divides the 80MHz base clock by this amount
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR sigma_delta_setPrescaler(uint8_t prescaler)
{
  uint32_t reg = GPSD;

  reg = (reg & ~(0xFF << GPSDP)) | ((prescaler & 0xFF) << GPSDP);
  GPSD = reg;
}
