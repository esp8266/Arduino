/* Any copyright is dedicated to the Public Domain. */

#include "sigma_delta.h"

void setup() {

  Serial.begin(115200);
  pinMode(BUILTIN_LED,OUTPUT); // blinkie & sigma-delta mix
  uint32_t reqFreq = 1000;
  uint32_t realFreq;
  
  realFreq = sigmaDeltaSetup(0,reqFreq); // chose a low frequency

  Serial.println();
  Serial.println("Start Sigma Delta Example\n");
  Serial.printf("Frequency = %u\n",realFreq);
  
}

void loop() {
 
  uint8_t duty, iRepeat;
  
  Serial.println("Attaching the built in led to the sigma delta source now\n");
  Serial.printf("Current duty = %i, prescaler = %i\n",sigmaDeltaRead(),sigmaDeltaGetPrescaler());
  sigmaDeltaAttachPin(BUILTIN_LED);
  
  Serial.println("dimming builtin led...\n");
  for (iRepeat=0;iRepeat<10;iRepeat++)
  {
    for (duty=0; duty<255;duty=duty+5)
    {
      sigmaDeltaWrite(0,duty);
      delay(10);
    }
    
    for (duty=255; duty>0;duty=duty-5)
    {
      sigmaDeltaWrite(0,duty);
      delay(10);
    }
    
  }
  Serial.println("detaching builtin led & playing a blinkie\n");
  sigmaDeltaDetachPin(BUILTIN_LED);
  for (iRepeat=0;iRepeat<20;iRepeat++)
  {
    digitalWrite(BUILTIN_LED,!digitalRead(BUILTIN_LED));
    delay(500);
  }
    
}
