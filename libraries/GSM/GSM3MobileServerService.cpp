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
#include <GSM3MobileServerService.h>
#include <GSM3MobileServerProvider.h>
#include <GSM3MobileClientProvider.h>


#define __TOUTSERVER__ 10000
#define BUFFERSIZETWEET 100

#define GSM3MOBILESERVERSERVICE_SYNCH 0x01 // 1: TRUE, compatible with other clients 0: FALSE

// While there is only a shield (ShieldV1) we will include it by default
#include <GSM3ShieldV1ServerProvider.h>
GSM3ShieldV1ServerProvider theShieldV1ServerProvider;


GSM3MobileServerService::GSM3MobileServerService(uint8_t port, bool synch)
{
	mySocket=0;
	_port=port;
	flags = 0;
	
	// If synchronous
	if(synch)
		flags |= GSM3MOBILESERVERSERVICE_SYNCH;
}

// Returns 0 if last command is still executing
// 1 if success
// >1 if error 
int GSM3MobileServerService::ready()
{	
	return theGSM3MobileServerProvider->ready();
}

void GSM3MobileServerService::begin()
{	
	if(theGSM3MobileServerProvider==0)
		return;
	theGSM3MobileServerProvider->connectTCPServer(_port);
	
	if(flags & GSM3MOBILESERVERSERVICE_SYNCH)
		waitForAnswer();
}

GSM3MobileClientService GSM3MobileServerService::available(bool synch)
{	
	int newSocket;
	// In case we are debugging, we'll need to force a look at the buffer
	ready();
	
	newSocket=theGSM3MobileServerProvider->getNewOccupiedSocketAsServer();
	
	// Instatiate new client. If we are synch, the client is synchronous/blocking
	GSM3MobileClientService client((uint8_t)(newSocket), (flags & GSM3MOBILESERVERSERVICE_SYNCH));

	return client;
}

size_t GSM3MobileServerService::write(uint8_t c)
{
// Adapt to the new, lean implementation	
//	theGSM3MobileServerProvider->writeSocket(c);
	return 1;
}

void GSM3MobileServerService::beginWrite()
{
// Adapt to the new, lean implementation
//	theGSM3MobileServerProvider->beginWriteSocket(local1Remote0, mySocket);
}

size_t GSM3MobileServerService::write(const uint8_t* buf)
{
// Adapt to the new, lean implementation
//	theGSM3MobileServerProvider->writeSocket((const char*)(buf));
	return strlen((const char*)buf);
}

size_t GSM3MobileServerService::write(const uint8_t* buf, size_t sz)
{
// Adapt to the new, lean implementation
//	theGSM3MobileServerProvider->writeSocket((const char*)(buf));
}

void GSM3MobileServerService::endWrite()
{
// Adapt to the new, lean implementation
//	theGSM3MobileServerProvider->endWriteSocket();
}

void GSM3MobileServerService::stop()
{
	
	// Review, should be the server?
	theGSM3MobileClientProvider->disconnectTCP(local1Remote0, mySocket);
	if(flags & GSM3MOBILESERVERSERVICE_SYNCH)
		waitForAnswer();
	theGSM3MobileClientProvider->releaseSocket(mySocket);
	mySocket = -1;
}


/*int GSM3MobileServerService::getIP(char* LocalIP, int LocalIPlength)
{
	return theGSM3MobileServerProvider->getIP(LocalIP, LocalIPlength);
}*/

int GSM3MobileServerService::waitForAnswer()
{
	unsigned long m;
	m=millis();
	int res;
	
	while(((millis()-m)< __TOUTSERVER__ )&&(ready()==0)) 
		delay(10);
	
	res=ready();

	// If we get something different from a 1, we are having a problem
	if(res!=1)
		res=0;

	return res;
}


