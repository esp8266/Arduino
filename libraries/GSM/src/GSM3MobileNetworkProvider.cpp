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
#include <GSM3MobileNetworkProvider.h>

GSM3MobileNetworkProvider* theProvider;

GSM3MobileNetworkProvider::GSM3MobileNetworkProvider()
{
	socketsAsServer=0x0000;
};


int GSM3MobileNetworkProvider::getNewOccupiedSocketAsServer()
{
	int i;
	for(i=minSocketAsServer(); i<=maxSocketAsServer(); i++)
	{
		if ((!(socketsAsServer&(0x0001<<i))) && getSocketAsServerModemStatus(i))
		{
			socketsAsServer|=((0x0001)<<i);	
			//Serial.print("New occupied=");Serial.println(i);
			return i;
		}
	}
	//Serial.println("No new occupied");
	return -1;
}


bool GSM3MobileNetworkProvider::getStatusSocketAsServer(uint8_t socket)
{
	if(socketsAsServer&(0x0001<<socket))
		return 1;
	else
		return 0;
};




