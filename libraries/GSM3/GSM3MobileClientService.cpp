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
#include <GSM3MobileClientService.h>
#include <GSM3MobileClientProvider.h>
#include <Arduino.h>

// While there is only a shield (ShieldV1) we will include it by default
#include <GSM3ShieldV1ClientProvider.h>
GSM3ShieldV1ClientProvider theShieldV1ClientProvider;


#define GSM3MOBILECLIENTSERVICE_CLIENT 0x01 // 1: This side is Client. 0: This side is Server
#define GSM3MOBILECLIENTSERVICE_WRITING 0x02 // 1: TRUE 0: FALSE
#define GSM3MOBILECLIENTSERVICE_SYNCH 0x04 // 1: TRUE, compatible with other clients 0: FALSE

#define __TOUTBEGINWRITE__ 10000


GSM3MobileClientService::GSM3MobileClientService(bool synch)
{
	flags = GSM3MOBILECLIENTSERVICE_CLIENT;
	if(synch)
		flags |= GSM3MOBILECLIENTSERVICE_SYNCH;
	mySocket=255;
}

GSM3MobileClientService::GSM3MobileClientService(int socket, bool synch)
{
	// We are creating a socket on an existing, occupied one.
	flags=0;
	if(synch)
		flags |= GSM3MOBILECLIENTSERVICE_SYNCH;
	mySocket=socket;
	theGSM3MobileClientProvider->getSocket(socket);
	
}

// Returns 0 if last command is still executing
// 1 if success
// >1 if error 
int GSM3MobileClientService::ready()
{	
	return theGSM3MobileClientProvider->ready();
}

int GSM3MobileClientService::connect(IPAddress add, uint16_t port) 
{
	if(theGSM3MobileClientProvider==0)
		return 2;
		
	// TODO: ask for the socket id
	mySocket=theGSM3MobileClientProvider->getSocket();

	if(mySocket<0)
		return 2;
	
	int res=theGSM3MobileClientProvider->connectTCPClient(add, port, mySocket);
	if(flags & GSM3MOBILECLIENTSERVICE_SYNCH)
		res=waitForAnswer();
	
	return res;
};

int GSM3MobileClientService::connect(const char *host, uint16_t port)
{

	if(theGSM3MobileClientProvider==0)
		return 2;		
	// TODO: ask for the socket id
	mySocket=theGSM3MobileClientProvider->getSocket();

	if(mySocket<0)
		return 2;
	
	int res=theGSM3MobileClientProvider->connectTCPClient(host, port, mySocket);
	if(flags & GSM3MOBILECLIENTSERVICE_SYNCH)
		res=waitForAnswer();
		
	return res;
}

int GSM3MobileClientService::waitForAnswer()
{
	unsigned long m;
	m=millis();
	int res;
	
	while(((millis()-m)< __TOUTBEGINWRITE__ )&&(ready()==0)) 
		delay(100);
	
	res=ready();

	// If we get something different from a 1, we are having a problem
	if(res!=1)
		res=0;

	return res;
}

void GSM3MobileClientService::beginWrite(bool sync)
{
	flags |= GSM3MOBILECLIENTSERVICE_WRITING;
	theGSM3MobileClientProvider->beginWriteSocket(flags & GSM3MOBILECLIENTSERVICE_CLIENT, mySocket);
	if(sync)
		waitForAnswer();
}

size_t GSM3MobileClientService::write(uint8_t c)
{	
	if(!(flags & GSM3MOBILECLIENTSERVICE_WRITING))
		beginWrite(true);
	theGSM3MobileClientProvider->writeSocket(c);
	return 1;
}

size_t GSM3MobileClientService::write(const uint8_t* buf)
{
	if(!(flags & GSM3MOBILECLIENTSERVICE_WRITING))
		beginWrite(true);
	theGSM3MobileClientProvider->writeSocket((const char*)(buf));
	return strlen((const char*)buf);
}

size_t GSM3MobileClientService::write(const uint8_t* buf, size_t sz)
{
	if(!(flags & GSM3MOBILECLIENTSERVICE_WRITING))
		beginWrite(true);
	for(int i=0;i<sz;i++)
		theGSM3MobileClientProvider->writeSocket(buf[i]);
	return sz;
}

void GSM3MobileClientService::endWrite(bool sync)
{
	flags ^= GSM3MOBILECLIENTSERVICE_WRITING;
	theGSM3MobileClientProvider->endWriteSocket();
	if(sync)
		waitForAnswer();
}

uint8_t GSM3MobileClientService::connected()
{
	if(mySocket==255)
		return 0;
	return theGSM3MobileClientProvider->getStatusSocketClient(mySocket);	 
}

GSM3MobileClientService::operator bool()
{
	return connected()==1;
};

int GSM3MobileClientService::available()
{
	int res;

	// Even if not connected, we are looking for available data
	
	if(flags & GSM3MOBILECLIENTSERVICE_WRITING)
		endWrite(true);

	res=theGSM3MobileClientProvider->availableSocket(flags & GSM3MOBILECLIENTSERVICE_CLIENT,mySocket);
	if(flags & GSM3MOBILECLIENTSERVICE_SYNCH)
		res=waitForAnswer();

	return res;
}

int GSM3MobileClientService::read(uint8_t *buf, size_t size)
{
	int i;
	uint8_t c;
	
	for(i=0;i<size;i++)
	{
		c=read();
		if(c==0)
			break;
		buf[i]=c;
	}
	
	return i;
/* This is the old implementation, testing a simpler one
	int res;
	// If we were writing, just stop doing it.
	if(flags & GSM3MOBILECLIENTSERVICE_WRITING)
		endWrite(true);
	res=theGSM3MobileClientProvider->readSocket(flags & GSM3MOBILECLIENTSERVICE_CLIENT, (char *)(buf), size, mySocket);

	return res;
*/
}

int GSM3MobileClientService::read()
{
	if(flags & GSM3MOBILECLIENTSERVICE_WRITING)
		endWrite(true);
	int c=theGSM3MobileClientProvider->readSocket();
	return c;
}

int GSM3MobileClientService::peek()
{
	if(flags & GSM3MOBILECLIENTSERVICE_WRITING)
		endWrite(true);
	return theGSM3MobileClientProvider->peekSocket(/*mySocket, false*/);
}

void GSM3MobileClientService::flush()
{
	if(flags & GSM3MOBILECLIENTSERVICE_WRITING)
		endWrite(true);
	theGSM3MobileClientProvider->flushSocket(/*mySocket*/);
	if(flags & GSM3MOBILECLIENTSERVICE_SYNCH)
		waitForAnswer();

}

void GSM3MobileClientService::stop()
{
	if(flags & GSM3MOBILECLIENTSERVICE_WRITING)
		endWrite(true);
	theGSM3MobileClientProvider->disconnectTCP(flags & GSM3MOBILECLIENTSERVICE_CLIENT, mySocket);
	theGSM3MobileClientProvider->releaseSocket(mySocket);
	mySocket = 0;
	if(flags & GSM3MOBILECLIENTSERVICE_SYNCH)
		waitForAnswer();
}

