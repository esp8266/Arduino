/*
  Esplora Remote
 
 This sketch allows to test all the Esplora's peripherals.
 It is also used with the ProcessingStart sketch (for Processing).
 
 When uploaded, you can open the Serial monitor and write one of
 the following commands (without quotes) to get an answer:
 
 "D": prints the current value of all sensors, separated by a comma.
 See the dumpInputs() function below to get the meaning of
 each value.
 
 "Rxxx"
 "Gxxx"
 "Bxxx": set the color of the RGB led. For example, write "R255"
 to turn on the red to full brightness, "G128" to turn
 the green to half brightness, or "G0" to turn off
 the green channel.
 
 "Txxxx": play a tone with the buzzer. The number is the
 frequency, e.g. "T440" plays the central A note.
 Write "T0" to turn off the buzzer.
 
 
 Created on 22 november 2012
 By Enrico Gueli <enrico.gueli@gmail.com>
 Modified 23 Dec 2012
 by Tom Igoe
 */

#include <Esplora.h>

void setup() {
  while(!Serial); // needed for Leonardo-based board like Esplora
  Serial.begin(9600);
}

void loop() {
  if (Serial.available())
    parseCommand();
}

/*
 * This function reads a character from the serial line and
 * decide what to do next. The "what to do" part is given by
 * function it calls (e.g. dumpInputs(), setRed() and so on).
 */
void parseCommand() {
  char cmd = Serial.read();
  switch(cmd) {
  case 'D': 
    dumpInputs(); 
    break;
  case 'R': 
    setRed(); 
    break;
  case 'G': 
    setGreen(); 
    break;
  case 'B': 
    setBlue(); 
    break;
  case 'T': 
    setTone(); 
    break;
  }
}

void dumpInputs() {  
  Serial.print(Esplora.readButton(SWITCH_1)); 
  Serial.print(',');
  Serial.print(Esplora.readButton(SWITCH_2)); 
  Serial.print(',');
  Serial.print(Esplora.readButton(SWITCH_3)); 
  Serial.print(',');
  Serial.print(Esplora.readButton(SWITCH_4)); 
  Serial.print(',');
  Serial.print(Esplora.readSlider());         
  Serial.print(',');
  Serial.print(Esplora.readLightSensor());    
  Serial.print(',');
  Serial.print(Esplora.readTemperature(DEGREES_C)); 
  Serial.print(',');
  Serial.print(Esplora.readMicrophone());     
  Serial.print(',');
  Serial.print(Esplora.readJoystickSwitch()); 
  Serial.print(',');
  Serial.print(Esplora.readJoystickX());      
  Serial.print(',');
  Serial.print(Esplora.readJoystickY());      
  Serial.print(',');
  Serial.print(Esplora.readAccelerometer(X_AXIS)); 
  Serial.print(',');
  Serial.print(Esplora.readAccelerometer(Y_AXIS)); 
  Serial.print(',');
  Serial.print(Esplora.readAccelerometer(Z_AXIS)); 
  Serial.println();
}

void setRed() {
  Esplora.writeRed(Serial.parseInt());
}

void setGreen() {
  Esplora.writeGreen(Serial.parseInt());
}

void setBlue() {
  Esplora.writeBlue(Serial.parseInt());
}

void setTone() {
  Esplora.tone(Serial.parseInt());
}

