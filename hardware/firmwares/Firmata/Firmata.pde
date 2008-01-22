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
 */

/* 
 * TODO: add pulseOut functionality for servos
 * TODO: add software PWM for servos, etc (servo.h or pulse.h)
 * TODO: add device type reporting (i.e. some firmwares will use the Firmata
 *       protocol, but will only support specific devices, like ultrasound 
 *       rangefinders or servos)
 * TODO: use Program Control to load stored profiles from EEPROM
 */

/* cvs version: $Id: Pd_firmware.pde,v 1.29 2007/03/08 05:37:22 eighthave Exp $ */
/* svn version: 334 */

/*==============================================================================
 * MESSAGE FORMATS
 *============================================================================*/

/* -----------------------------------------------------------------------------
 * MAPPING DATA TO MIDI
 *
 * This protocol uses the MIDI message format, but does not use the whole
 * protocol.  Most of the command mappings here will not be directly usable in
 * terms of MIDI controllers and synths.  It should co-exist with MIDI without
 * trouble and can be parsed by standard MIDI interpreters.  Just some of the
 * message data is used differently.
 *
 * MIDI format: http://www.harmony-central.com/MIDI/Doc/table1.html
 * 
 *                              MIDI       
 * type                command  channel    first byte            second byte 
 * -----------------------------------------------------------------------------
 * analog I/O            0xE0   pin #      LSB(bits 0-6)         MSB(bits 7-13)
 * digital I/O           0x90   port base  LSB(bits 0-6)         MSB(bits 7-13)
 * report analog pin     0xC0   pin #      disable/enable(0/1)   - n/a -
 * report digital ports  0xD0   port base  disable/enable(0/1)   - n/a -
 *
 * digital pin mode(I/O) 0xF4   - n/a -    pin # (0-63)          pin state(0=in)
 * firmware version      0xF9   - n/a -    minor version         major version
 * system reset          0xFF   - n/a -    - n/a -               - n/a -
 *
 */

/* proposed extensions using SysEx
 *
 * type      SysEx start  command  data bytes                         SysEx stop
 * -----------------------------------------------------------------------------
 * pulse I/O   0xF0        0xA0   five 7-bit chunks, LSB first             0xF7 
 * shiftOut    0xF0        0xF5   dataPin; clockPin; 7-bit LSB; 7-bit MSB  0xF7
 */

/* -----------------------------------------------------------------------------
 * DATA MESSAGE FORMAT */

/* two byte digital data format
 * ----------------------------
 * 0  digital data, 0x90-0x9F, (MIDI NoteOn, but different data usage)
 * 1  digital pins 0-6 bitmask
 * 2  digital pins 7-13 bitmask 
 */

/* analog 14-bit data format
 * -------------------------
 * 0  analog pin, 0xE0-0xEF, (MIDI Pitch Wheel)
 * 1  analog least significant 7 bits
 * 2  analog most significant 7 bits
 */

/* version report format
 * Send a single byte 0xF9, Arduino will reply with:
 * -------------------------------------------------
 * 0  version report header (0xF9) (MIDI Undefined)
 * 1  minor version (0-127)
 * 2  major version (0-127)
 */

/* pulseIn/Out (uses 32-bit value)
 * -------------------------------
 * 0  START_SYSEX (0xF0) (MIDI System Exclusive)
 * 1  pulseIn/Out (0xA0-0xAF)
 * 2  bits 0-6 (least significant byte)
 * 3  bits 7-13
 * 4  bits 14-20
 * 5  bits 21-27
 * 6  bits 28-34 (most significant byte)
 * 7  END_SYSEX (0xF7) (MIDI End of SysEx - EOX)
 */

/* shiftIn/Out (uses 8-bit value)
 * ------------------------------
 * 0  START_SYSEX (0xF0)
 * 1  shiftOut (0xF5)
 * 2  dataPin (0-127)
 * 3  clockPin (0-127)
 * 4  bits 0-6 (least significant byte)
 * 5  bit 7 (most significant bit)
 * 6  END_SYSEX (0xF7)
 */

/* -----------------------------------------------------------------------------
 * CONTROL MESSAGES */

/* set digital pin mode
 * --------------------
 * 1  set digital pin mode (0xF4) (MIDI Undefined)
 * 2  pin number (0-127)
 * 3  state (INPUT/OUTPUT, 0/1)
 */

/* toggle analogIn reporting by pin
 * --------------------------------
 * 0  toggle digitalIn reporting (0xC0-0xCF) (MIDI Program Change)
 * 1  disable(0)/enable(non-zero) 
 */

/* toggle digitalIn reporting by port pairs
 * ----------------------------------------
 * 0  toggle digitalIn reporting (0xD0-0xDF) (MIDI Aftertouch)
 * 1  disable(0)/enable(non-zero) 
 */

/* request version report
 * ----------------------
 * 0  request version report (0xF9) (MIDI Undefined)
 */

/*==============================================================================
 * MACROS
 *============================================================================*/

/* Version numbers for the protocol.  The protocol is still changing, so these
 * version numbers are important.  This number can be queried so that host
 * software can test whether it will be compatible with the currently
 * installed firmware. */
#define FIRMATA_MAJOR_VERSION   1 // for non-compatible changes
#define FIRMATA_MINOR_VERSION   0 // for backwards compatible changes

/* total number of pins currently supported */  
#define TOTAL_ANALOG_PINS       6
#define TOTAL_DIGITAL_PINS      14

// for comparing along with INPUT and OUTPUT
#define PWM                     2

// for selecting digital inputs
#define PB  2  // digital input, pins 8-13
#define PC  3  // analog input port
#define PD  4  // digital input, pins 0-7

#define MAX_DATA_BYTES 2 // max number of data bytes in non-SysEx messages
/* message command bytes */
#define DIGITAL_MESSAGE         0x90 // send data for a digital pin
#define ANALOG_MESSAGE          0xE0 // send data for an analog pin (or PWM)
//#define PULSE_MESSAGE           0xA0 // proposed pulseIn/Out message (SysEx)
//#define SHIFTOUT_MESSAGE        0xB0 // proposed shiftOut message (SysEx)
#define REPORT_ANALOG_PIN       0xC0 // enable analog input by pin #
#define REPORT_DIGITAL_PORTS    0xD0 // enable digital input by port pair
#define START_SYSEX             0xF0 // start a MIDI SysEx message
#define SET_DIGITAL_PIN_MODE    0xF4 // set a digital pin to INPUT or OUTPUT 
#define END_SYSEX               0xF7 // end a MIDI SysEx message
#define REPORT_VERSION          0xF9 // report firmware version
#define SYSTEM_RESET            0xFF // reset from MIDI

/*==============================================================================
 * GLOBAL VARIABLES
 *============================================================================*/

/* input message handling */
byte waitForData = 0; // this flag says the next serial input will be data
byte executeMultiByteCommand = 0; // execute this after getting multi-byte data
byte multiByteChannel = 0; // channel data for multiByteCommands
byte storedInputData[MAX_DATA_BYTES] = {0,0}; // multi-byte data
/* digital pins */
boolean digitalInputsEnabled = false; // output digital inputs or not
int digitalInputs;
int previousDigitalInputs; // previous output to test for change
int digitalPinStatus = 3; // bitwise array to store pin status, ignore RxTx pins
/* PWM/analog outputs */
int pwmStatus = 0; // bitwise array to store PWM status
/* analog inputs */
unsigned int analogPinsToReport = 0; // bitwise array to store pin reporting
int analogPin = 0; // counter for reading analog pins
int analogData; // storage variable for data from analogRead()
/* timer variables */
extern volatile unsigned long timer0_overflow_count; // timer0 from wiring.c
unsigned long nextExecuteTime; // for comparison with timer0_overflow_count

/*==============================================================================
 * FUNCTIONS                                                                
 *============================================================================*/
/* -----------------------------------------------------------------------------
 * output the version message to the serial port  */
void printVersion() {
  Serial.print(REPORT_VERSION, BYTE);
  Serial.print(FIRMATA_MINOR_VERSION, BYTE);
  Serial.print(FIRMATA_MAJOR_VERSION, BYTE);
}

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
  if(digitalInputsEnabled) {
	previousDigitalInputs = digitalInputs;
	digitalInputs = PINB << 8;  // get pins 8-13
	digitalInputs += PIND;      // get pins 0-7
	digitalInputs = digitalInputs &~ digitalPinStatus; // ignore pins set OUTPUT
	if(digitalInputs != previousDigitalInputs) {
	  // TODO: implement more ports as channels for more than 16 digital pins
	  Serial.print(DIGITAL_MESSAGE,BYTE);
	  Serial.print(digitalInputs % 128, BYTE); // Tx pins 0-6
	  Serial.print(digitalInputs >> 7, BYTE);  // Tx pins 7-13
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
    analogPinsToReport = analogPinsToReport &~ (1 << pin);
  }
  else { // everything but 0 enables reporting of that pin
    analogPinsToReport = analogPinsToReport | (1 << pin);
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
		  digitalInputsEnabled = true; // enable reporting of digital inputs
        break;
      case REPORT_ANALOG_PIN:
		setAnalogPinReporting(multiByteChannel,storedInputData[0]);
        break;
      case REPORT_DIGITAL_PORTS:
		// TODO: implement MIDI channel as port base for more than 16 digital inputs
		if(storedInputData[0] == 0)
		  digitalInputsEnabled = false;
		else
		  digitalInputsEnabled = true;
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
	  printVersion();
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

  Serial.begin(57600); // 9600, 14400, 38400, 57600, 115200

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
    setPinMode(i,INPUT);
  }
  // TODO: load state from EEPROM here

  printVersion();

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
	  if( analogPinsToReport & (1 << analogPin) ) {
		analogData = analogRead(analogPin);
		Serial.print(ANALOG_MESSAGE + analogPin, BYTE);
		// These two bytes converted back into the 10-bit value on host
		Serial.print(analogData % 128, BYTE);
		Serial.print(analogData >> 7, BYTE); 
	  }
	}
  }
}
