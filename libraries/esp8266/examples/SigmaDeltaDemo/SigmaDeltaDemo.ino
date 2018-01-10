/*
  Sigma Delta Generator example
  
  This example demonstrates the use of the ESP8266 internal hardware sigma delta source.
  Each GPIO pin or multiple pins can be connected to the one sigma delta source.
  THE sigma delta output frequency is defined by the ESP8266 hardware as:

  FREQ = 80,000,000/prescaler * target /256  HZ,          0<target<128
  FREQ = 80,000,000/prescaler * (256-target) /256  HZ,    128<target<256
  with :
  target: 0-255, defines the 8-bit duty cycle of the output signal, , duty cycle = target/256
  prescaler: 0-255, reduce the output frequencies
  
  so both the target and prescaler will affect the freq.
  
  Differences with Arduino library analogWrite :
  - the arduino analogWrite pwm uses a hardware timer and ISR to generate the pwm signal. The sigma delta pwm doesn't need a hardware timer. 
    The FRC1 timer can then be used for other purposes.
  - the arduino analogWrite pwm produces a more or less fixed frequency around 900Hz. The sigma delta source produces a variable output frequency, 
    that depends on the duty cycle. Example, with prescaler = 10, the output frequency varies between 31,kHz and 4MHz
  - the sigma delta source produces much higher frequencies. This allows the output to be easily smoothed by a RC-filter and realise a decent 8-bit DAC
  - the arduino analogWrite pwm has a higher resolution, default 12-bit, and up to 16-bit resolution. The sigma delta source has only 8-bit resolution
    
  The example doesn't require additional hardware.
  
  With an RC-filter (for example R=17k & C=100nF), a 'DC' output signal can be obtained with voltage between 0 and 3.3V
  For higher value capacitors, it might be necessary to add a high-speed buffer between the esp8266 output and RC-filter to limit the current ac load on the 
  pin output driver.

*/
#include "sigma_delta.h"

void setup() {

  Serial.begin(115200);
  pinMode(BUILTIN_LED,OUTPUT); // blinkie & sigma-delta mix
  
  uint32 pin = 4; //D2
  sigma_delta_enable();
  //sigma_delta_disable();
  sigma_delta_attachPin(BUILTIN_LED);
  sigma_delta_attachPin(4); // D2
  
  sigma_delta_setPrescaler(255);

  Serial.println();
  Serial.println("Start Sigma Delta Example\n");
  Serial.println("Attached gpio4 to the sigma delta source\n");
  Serial.printf("Current target = %i, prescaler = %i\n",sigma_delta_getTarget(),sigma_delta_getPrescaler());
}

void loop() {
 
  uint8_t target, iRepeat;
  
  Serial.println("Attaching the built in led to the sigma delta source now\n");
  Serial.printf("Current target = %i, prescaler = %i\n",sigma_delta_getTarget(),sigma_delta_getPrescaler());
  sigma_delta_attachPin(BUILTIN_LED);
  
  Serial.println("dimming builtin led...\n");
  for (iRepeat=0;iRepeat<10;iRepeat++)
  {
    for (target=0; target<255;target=target+5)
    {
      sigma_delta_setTarget(target);
      delay(10);
    }
    
    for (target=255; target>0;target=target-5)
    {
      sigma_delta_setTarget(target);
      delay(10);
    }
    
  }
  Serial.println("detaching builtin led & playing a blinkie\n");
  sigma_delta_detachPin(BUILTIN_LED);
  for (iRepeat=0;iRepeat<20;iRepeat++)
  {
    digitalWrite(BUILTIN_LED,!digitalRead(BUILTIN_LED));
    delay(500);
  }
    
}
