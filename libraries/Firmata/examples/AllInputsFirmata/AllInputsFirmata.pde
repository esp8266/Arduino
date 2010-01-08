/* 
 * This firmware reads all inputs and sends them as fast as it can.  It was
 * inspired by the ease-of-use of the Arduino2Max program.
 *
 * This example code is in the public domain.
 */
#include <Firmata.h>

byte pin;

int analogValue;
int previousAnalogValues[TOTAL_ANALOG_PINS];

byte portStatus[TOTAL_PORTS];
byte previousPINs[TOTAL_PORTS];

/* timer variables */
unsigned long currentMillis;     // store the current value from millis()
unsigned long nextExecuteMillis; // for comparison with currentMillis
/* make sure that the FTDI buffer doesn't go over 60 bytes, otherwise you
   get long, random delays.  So only read analogs every 20ms or so */
int samplingInterval = 19;      // how often to run the main loop (in ms)

void sendPort(byte portNumber, byte portValue)
{
  portValue = portValue &~ portStatus[portNumber];
  if(previousPINs[portNumber] != portValue) {
    Firmata.sendDigitalPort(portNumber, portValue);
    previousPINs[portNumber] = portValue;
  }
}

void setup()
{
  Firmata.setFirmwareVersion(0, 1);

  for(pin = 0; pin < TOTAL_DIGITAL_PINS; pin++) {
    pinMode(pin, INPUT);
  }

  portStatus[0] = B00000011;  // ignore Tx/RX pins
  portStatus[1] = B11000000;  // ignore 14/15 pins 
  portStatus[2] = B00000000;

  Firmata.begin(57600);
}

void loop()
{
  sendPort(0, PIND);
  sendPort(1, PINB);
  sendPort(2, PINC);
  /* make sure that the FTDI buffer doesn't go over 60 bytes, otherwise you
     get long, random delays.  So only read analogs every 20ms or so */
  currentMillis = millis();
  if(currentMillis > nextExecuteMillis) {  
    nextExecuteMillis = currentMillis + samplingInterval;
    while(Firmata.available()) {
      Firmata.processInput();
    }
    for(pin = 0; pin < TOTAL_ANALOG_PINS; pin++) {
      analogValue = analogRead(pin);
      if(analogValue != previousAnalogValues[pin]) {
        Firmata.sendAnalog(pin, analogValue); 
        previousAnalogValues[pin] = analogValue;
      }
    }
  }
}


