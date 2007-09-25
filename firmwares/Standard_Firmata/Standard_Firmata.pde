/*
 * Copyright (C) 2006 Free Software Foundation
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * See file LICENSE for further informations on licensing terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * -----------------------------------------------------------
 * Firmata, the general purpose sensorbox firmware for Arduino
 * -----------------------------------------------------------
 * 
 * Firmata turns the Arduino into a Plug-n-Play sensorbox, servo
 * controller, and/or PWM motor/lamp controller.
 *
 * It was originally designed to work with the Pd object [arduino]
 * which is included in Pd-extended.  This firmware is intended to
 * work with any host computer software package.  It can easily be
 * used with other programs like Max/MSP, Processing, or whatever can
 * do serial communications.
 *
 * @author: Hans-Christoph Steiner <hans@at.or.at>
 *   help with initial protocol redesign: Jamie Allen <jamie@heavyside.net>
 *   much protocol discussion: the Arduino developers mailing list
 *   key bugfixes: Georg Holzmann <grh@mur.at>
 *                 Gerda Strobl <gerda.strobl@student.tugraz.at>
 * @date: 2006-05-19
 * @locations: STEIM, Amsterdam, Netherlands
 *             IDMI/Polytechnic University, Brookyn, NY, USA
 *             Electrolobby Ars Electronica, Linz, Austria
 *
 * See http://www.arduino.cc/playground/Interfacing/Firmata for docs
 */

/* 
 * TODO: add pulseOut functionality for servos
 * TODO: add software PWM for servos, etc (servo.h or pulse.h)
 * TODO: add device type reporting (i.e. some firmwares will use the Firmata
 *       protocol, but will only support specific devices, like ultrasound 
 *       rangefinders or servos)
 * TODO: use Program Control to load stored profiles from EEPROM
 */

/* cvs version: $Id: Pd_firmware.pde,v 1.32 2007/06/27 20:59:24 eighthave Exp $ */

#include <EEPROM.h>
#include <Firmata.h>

/*==============================================================================
 * GLOBAL VARIABLES
 *============================================================================*/

/* input message handling */
byte waitForData = 0; // this flag says the next serial input will be data
byte executeMultiByteCommand = 0; // execute this after getting multi-byte data
byte multiByteChannel = 0; // channel data for multiByteCommands
byte storedInputData[MAX_DATA_BYTES] = {0,0}; // multi-byte data
/* digital pins */
boolean reportDigitalInputs = false; // output digital inputs or not
int digitalInputs;
int previousDigitalInputs; // previous output to test for change
unsigned int digitalPinStatus = 65535; // store pin status, default OUTPUT
/* PWM/analog outputs */
int pwmStatus = 0; // bitwise array to store PWM status
/* analog inputs */
int analogInputsToReport = 0; // bitwise array to store pin reporting
int analogPin = 0; // counter for reading analog pins
/* timer variables */
extern volatile unsigned long timer0_overflow_count; // timer0 from wiring.c
unsigned long nextExecuteTime; // for comparison with timer0_overflow_count

/*==============================================================================
 * FUNCTIONS                                                                
 *============================================================================*/

/* -----------------------------------------------------------------------------
 * output digital bytes received from the serial port  */
void outputDigitalBytes(byte pin0_6, byte pin7_13) {
  int i;
  int mask;
  int twoBytesForPorts;
    
// this should be converted to use PORTs
  twoBytesForPorts = pin0_6 + (pin7_13 << 7);
  for(i=2; i<TOTAL_DIGITAL_PINS; ++i) { // ignore Rx,Tx pins (0 and 1)
    mask = 1 << i;
    if( (digitalPinStatus & mask) && !(pwmStatus & mask) ) {
      digitalWrite(i, twoBytesForPorts & mask ? HIGH : LOW);
    } 
  }
}

/* -----------------------------------------------------------------------------
 * check all the active digital inputs for change of state, then add any events
 * to the Serial output queue using Serial.print() */
void checkDigitalInputs(void) {
  if(reportDigitalInputs) {
	 previousDigitalInputs = digitalInputs;
	 digitalInputs = PINB << 8;  // get pins 8-13
	 digitalInputs += PIND;      // get pins 0-7
	 digitalInputs = digitalInputs &~ digitalPinStatus; // ignore pins set OUTPUT
	 if(digitalInputs != previousDigitalInputs) {
		// TODO: implement more ports as channels for more than 16 digital pins
		Firmata.sendDigitalPortPair(0, digitalInputs); // port 0 till more are implemented
		/*		Serial.print(DIGITAL_MESSAGE,BYTE);
		Serial.print(digitalInputs % 128, BYTE); // Tx pins 0-6
		Serial.print(digitalInputs >> 7, BYTE);  // Tx pins 7-13*/
	 }
  }
}

// -----------------------------------------------------------------------------
/* sets the pin mode to the correct state and sets the relevant bits in the
 * two bit-arrays that track Digital I/O and PWM status
 */
void setPinMode(byte pin, byte mode) {
  if(pin > 1) { // ignore RxTx pins (0,1)
	if(mode == INPUT) {
	  digitalPinStatus = digitalPinStatus &~ (1 << pin);
	  pwmStatus = pwmStatus &~ (1 << pin);
	  digitalWrite(pin,LOW); // turn off pin before switching to INPUT
	  pinMode(pin,INPUT);
	}
	else if(mode == OUTPUT) {
	  digitalPinStatus = digitalPinStatus | (1 << pin);
	  pwmStatus = pwmStatus &~ (1 << pin);
	  pinMode(pin,OUTPUT);
	}
	else if( mode == PWM ) {
	  digitalPinStatus = digitalPinStatus | (1 << pin);
	  pwmStatus = pwmStatus | (1 << pin);
	  pinMode(pin,OUTPUT);
	}
  // TODO: save status to EEPROM here, if changed
  }
}

// -----------------------------------------------------------------------------
/* sets bits in a bit array (int) to toggle the reporting of the analogIns
 */
void setAnalogPinReporting(byte pin, byte state) {
  if(state == 0) {
    analogInputsToReport = analogInputsToReport &~ (1 << pin);
  }
  else { // everything but 0 enables reporting of that pin
    analogInputsToReport = analogInputsToReport | (1 << pin);
  }
  // TODO: save status to EEPROM here, if changed
}

/* -----------------------------------------------------------------------------
 * processInput() is called whenever a byte is available on the
 * Arduino's serial port.  This is where the commands are handled. */
void processInput(int inputData) {
  int command;
  
  // a few commands have byte(s) of data following the command
  if( (waitForData > 0) && (inputData < 128) ) {  
    waitForData--;
    storedInputData[waitForData] = inputData;
    if( (waitForData==0) && executeMultiByteCommand ) { // got the whole message
      switch(executeMultiByteCommand) {
      case ANALOG_MESSAGE:
		setPinMode(multiByteChannel,PWM);
		analogWrite(multiByteChannel, 
		(storedInputData[0] << 7) + storedInputData[1] );
        break;
      case DIGITAL_MESSAGE:
		outputDigitalBytes(storedInputData[1], storedInputData[0]); //(LSB, MSB)
		break;
      case SET_DIGITAL_PIN_MODE:
		setPinMode(storedInputData[1], storedInputData[0]); // (pin#, mode)
		if(storedInputData[0] == INPUT) 
		  reportDigitalInputs = true; // enable reporting of digital inputs
        break;
      case REPORT_ANALOG_PIN:
		setAnalogPinReporting(multiByteChannel,storedInputData[0]);
        break;
      case REPORT_DIGITAL_PORTS:
		// TODO: implement MIDI channel as port base for more than 16 digital inputs
		if(storedInputData[0] == 0)
		  reportDigitalInputs = false;
		else
		  reportDigitalInputs = true;
        break;
      }
      executeMultiByteCommand = 0;
    }	
  } else {
    // remove channel info from command byte if less than 0xF0
    if(inputData < 0xF0) {
      command = inputData & 0xF0;
	  multiByteChannel = inputData & 0x0F;
    } else {
      command = inputData;
	  // commands in the 0xF* range don't use channel data
    }
    switch (command) { // TODO: these needs to be switched to command
    case ANALOG_MESSAGE:
    case DIGITAL_MESSAGE:
    case SET_DIGITAL_PIN_MODE:
      waitForData = 2; // two data bytes needed
      executeMultiByteCommand = command;
      break;
    case REPORT_ANALOG_PIN:
    case REPORT_DIGITAL_PORTS:
      waitForData = 1; // two data bytes needed
      executeMultiByteCommand = command;
      break;
    case SYSTEM_RESET:
      // this doesn't do anything yet
      break;
    case REPORT_VERSION:
	  Firmata.printVersion();
      break;
    }
  }
}

/* -----------------------------------------------------------------------------
 * this function checks to see if there is data waiting on the serial port 
 * then processes all of the stored data
 */
void checkForSerialReceive() {
  while(Serial.available())
	processInput(Serial.read());
}

/* -----------------------------------------------------------------------------
 * these functions are for loading and saving the state of the digital pins and
 * pin reporting so that the Arduino will start up again in the same state. The
 * EEPROM is supposed to have a life of at least 100,000 writes.
 */
void loadSettings() {
	//EEPROM.read();
}

void saveSettings() {
	EEPROM.write(ANALOGINPUTSTOREPORT_LOW_BYTE, analogInputsToReport & 0xFF);
	EEPROM.write(ANALOGINPUTSTOREPORT_HIGH_BYTE, analogInputsToReport >> 8);
	EEPROM.write(REPORTDIGITALINPUTS_BYTE, reportDigitalInputs & 0xFF);
	EEPROM.write(DIGITALPINSTATUS_LOW_BYTE, digitalPinStatus & 0xFF);
	EEPROM.write(DIGITALPINSTATUS_HIGH_BYTE, digitalPinStatus >> 8);
	EEPROM.write(PWMSTATUS_LOW_BYTE, pwmStatus & 0xFF);
	EEPROM.write(PWMSTATUS_HIGH_BYTE, pwmStatus >> 8);
}

// =============================================================================
// used for flashing the pin for the version number
void pin13strobe(int count, int onInterval, int offInterval) {
  byte i;
  pinMode(13, OUTPUT);
  for(i=0; i<count; i++) {
    delay(offInterval);
    digitalWrite(13,1);
    delay(onInterval);
    digitalWrite(13,0);
  }
}

/*==============================================================================
 * SETUP()
 *============================================================================*/
void setup() {
  byte i;

  // flash the pin 13 with the protocol version
  pinMode(13,OUTPUT);
  pin13strobe(2,1,4); // separator, a quick burst
  delay(500);
  pin13strobe(FIRMATA_MAJOR_VERSION, 200, 400);
  delay(500);
  pin13strobe(2,1,4); // separator, a quick burst
  delay(500);
  pin13strobe(FIRMATA_MINOR_VERSION, 200, 400);
  delay(500);
  pin13strobe(2,1,4); // separator, a quick burst

  for(i=0; i<TOTAL_DIGITAL_PINS; ++i) {
    setPinMode(i,OUTPUT);
  }
  // TODO: load state from EEPROM here

  /* TODO: send digital inputs here, if enabled, to set the initial state on the
   * host computer, since once in the loop(), the Arduino will only send data on
   * change. */
}

/*==============================================================================
 * LOOP()
 *============================================================================*/
void loop() {
/* DIGITALREAD - as fast as possible, check for changes and output them to the
 * FTDI buffer using Serial.print()  */
  checkDigitalInputs();  
  if(timer0_overflow_count > nextExecuteTime) {  
	nextExecuteTime = timer0_overflow_count + 19; // run this every 20ms
	/* SERIALREAD - Serial.read() uses a 128 byte circular buffer, so handle
	 * all serialReads at once, i.e. empty the buffer */
	checkForSerialReceive();
	/* SEND FTDI WRITE BUFFER - make sure that the FTDI buffer doesn't go over
	 * 60 bytes. use a timer to sending an event character every 4 ms to
	 * trigger the buffer to dump. */
	
	/* ANALOGREAD - right after the event character, do all of the
	 * analogReads().  These only need to be done every 4ms. */
	for(analogPin=0;analogPin<TOTAL_ANALOG_PINS;analogPin++) {
	  if( analogInputsToReport & (1 << analogPin) ) 
		  Firmata.sendAnalog(analogPin, analogRead(analogPin));
	}
  }
}
