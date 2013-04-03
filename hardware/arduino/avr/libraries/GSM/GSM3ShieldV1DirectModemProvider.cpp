/*
This file is part of the GSM3 communications library for Arduino
-- Multi-transport communications platform
-- Fully asynchronous
-- Includes code for the Arduino-Telefonica GSM/GPRS Shield V1
-- Voice calls
-- SMS
-- TCP/IP connections
-- HTTP basic clients

This library has been developed by Telefónica Digital - PDI -
- Physical Internet Lab, as part as its collaboration with
Arduino and the Open Hardware Community. 

September-December 2012

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

The latest version of this library can always be found at
https://github.com/BlueVia/Official-Arduino
*/
#include <GSM3ShieldV1DirectModemProvider.h>
#include <GSM3ShieldV1ModemCore.h>
#include <HardwareSerial.h>
#include <Arduino.h>

#define __RESETPIN__ 7

//Constructor
GSM3ShieldV1DirectModemProvider::GSM3ShieldV1DirectModemProvider(bool t)
{
	trace=t;
};

void GSM3ShieldV1DirectModemProvider::begin()
{
	theGSM3ShieldV1ModemCore.gss.begin(9600);
}

void GSM3ShieldV1DirectModemProvider::restartModem()
{
	pinMode(__RESETPIN__, OUTPUT);
	digitalWrite(__RESETPIN__, HIGH);
	delay(12000);
	digitalWrite(__RESETPIN__, LOW);
	delay(1000);

}

//To enable the debug process
void GSM3ShieldV1DirectModemProvider::connect()
{
        theGSM3ShieldV1ModemCore.registerActiveProvider(this);
}

//To disable the debug process
void GSM3ShieldV1DirectModemProvider::disconnect()
{
        theGSM3ShieldV1ModemCore.registerActiveProvider(0);
}

//Write to the modem by means of SoftSerial
size_t GSM3ShieldV1DirectModemProvider::write(uint8_t c)
{	
        theGSM3ShieldV1ModemCore.write(c);
}

//Detect if data to be read
int/*bool*/ GSM3ShieldV1DirectModemProvider::available()
{
	if (theGSM3ShieldV1ModemCore.gss.cb.peek(1)) return 1;
	else return 0;
} 

//Read data
int/*char*/ GSM3ShieldV1DirectModemProvider::read()
{
	int dataRead;
	dataRead = theGSM3ShieldV1ModemCore.gss.cb.read();
	//In case last char in xof mode.
	if (!(theGSM3ShieldV1ModemCore.gss.cb.peek(0))) {
			theGSM3ShieldV1ModemCore.gss.spaceAvailable();
			delay(100);
		}
	return dataRead;
} 

//Peek data
int/*char*/ GSM3ShieldV1DirectModemProvider::peek()
{
	return theGSM3ShieldV1ModemCore.gss.cb.peek(0);
} 

//Flush data
void GSM3ShieldV1DirectModemProvider::flush()
{
	return theGSM3ShieldV1ModemCore.gss.cb.flush();
}

String GSM3ShieldV1DirectModemProvider::writeModemCommand(String ATcommand, int responseDelay)
{

  if(trace)
	Serial.println(ATcommand);
	
  // Flush other texts
  flush();
  
  //Enter debug mode.
  connect();
  //Send the AT command.
  println(ATcommand);

  delay(responseDelay);

  //Get response data from modem.
  String result = "";
  if(trace)
	theGSM3ShieldV1ModemCore.gss.cb.debugBuffer();

  while (available())
  {
    char c = read();
    result += c;
  }
  if(trace)
	Serial.println(result);
  //Leave the debug mode.
  disconnect();
  return result;
}