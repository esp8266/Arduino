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
#include <GSM3MobileMockupProvider.h>
#include <inttypes.h>
#include <HardwareSerial.h>
#include <Arduino.h>


GSM3MobileMockupProvider::GSM3MobileMockupProvider()
{
	lineStatus=IDLE;
	msgExample="Hello#World";
	msgIndex=0;
};

int GSM3MobileMockupProvider::begin(char* pin)
{
	Serial.println("GSM3MobileMockupProvider::begin()");
	return 0;
};

int GSM3MobileMockupProvider::ready()
{
	Serial.println("GSM3MobileMockupProvider::ready()");
	return 1;
};

int GSM3MobileMockupProvider::beginSMS(const char* number)
{
	Serial.println("SM3MobileMockupProvider::beginSMS()");
	return 0;
};

void GSM3MobileMockupProvider::writeSMS(char c)
{
	Serial.print(c);
};

int GSM3MobileMockupProvider::endSMS()
{
	Serial.println("GSM3MobileMockupProvider::endSMS()");
};

int GSM3MobileMockupProvider::availableSMS()
{
	Serial.println("GSM3MobileMockupProvider::availableSMS()");
	return 120;
};

int GSM3MobileMockupProvider::peek()
{
	return (int)'H';
};

int GSM3MobileMockupProvider::remoteSMSNumber(char* number, int nlength)
{
	if(nlength>=13)
		strcpy(number, "+34630538546");
	return 12;
};


void GSM3MobileMockupProvider::flushSMS()
{
	Serial.println("GSM3MobileMockupProvider::flushSMS()");
};

int GSM3MobileMockupProvider::readSMS()
{
	if(msgExample[msgIndex]==0)
	{
		msgIndex=0;
		return 0;
	}
	else
	{
		msgIndex++;
		return msgExample[msgIndex-1];
	};
};

int GSM3MobileMockupProvider::connectTCPClient(const char* server, int port, int id_socket)
{
	Serial.println("GSM3MobileMockupProvider::connectTCPClient()");
	Serial.print(server);Serial.print(":");Serial.print(port);Serial.print("-");Serial.println(id_socket);
}

void GSM3MobileMockupProvider::writeSocket(const uint8_t *buf, size_t size, int id_socket)
{
	int i;
	for(i=0;i<size;i++)
		Serial.print(buf[i]);
}
/* I'm taking this off. We'll reply from the NetworkProvider
uint8_t GSM3MobileMockupProvider::getStatus(uint8_t socket)
{
    if((socket>=minSocket())&&(socket<=maxSocket()))
		return 1;
	else
		return 0;
};
*/

int GSM3MobileMockupProvider::readSocket(uint8_t *buf, size_t size, int idsocket)
{
	int i;
	int l=strlen(msgExample);
	for(i=0;(i<size)&&(i<l);i++)
		buf[i]=msgExample[i];
	buf[i]=0;
	return i;
}

int GSM3MobileMockupProvider::availableSocket(int idsocket)
{
	return 1;
};

int GSM3MobileMockupProvider::readSocket(int idsocket, bool advance)
{
	char c;
	if(msgExample[msgIndex]==0)
	{
		msgIndex=0;
		return 0;
	}
	else
	{
		c=msgExample[msgIndex];
		if(advance)
			msgIndex++;
	};
	return c;
};

void GSM3MobileMockupProvider::flushSocket(int idsocket)
{
	while(readSocket(idsocket));
};

int GSM3MobileMockupProvider::disconnectTCP(int idsocket)
{
	Serial.println("GSM3MobileMockupProvider::disconnectTCP()");
	return 1;
};

int GSM3MobileMockupProvider::connectTCPServer(int port, char* localIP, int* localIPlength)
{
	Serial.println("GSM3MobileMockupProvider::connectTCPServer()");
	if((localIP!=0)&&(*localIPlength>12))
		strcpy("192.168.1.1", localIP);
	return 1;
};

bool GSM3MobileMockupProvider::getSocketModemStatus(uint8_t s)
{
	// Feeling lazy
	return true;
}

