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
#include <GSM3CircularBuffer.h>
#include <Arduino.h>

GSM3CircularBuffer::GSM3CircularBuffer(GSM3CircularBufferManager* mgr)
{
	head=0;
	tail=0;
	cbm=mgr;
}

int GSM3CircularBuffer::write(char c)
{
	byte aux=(tail+1)& __BUFFERMASK__;
	if(aux!=head)
	{
		theBuffer[tail]=c;
		// Lets put an extra zero at the end, so we can
		// read chains as we like.
		// This is not exactly perfect, we are always 1+ behind the head
		theBuffer[aux]=0;
		tail=aux;
		return 1;
	}
	return 0;
}

char GSM3CircularBuffer::read()
{
	char res;
	if(head!=tail)
	{
		res=theBuffer[head];
		head=(head+1)& __BUFFERMASK__;
		//if(cbm)
		//	cbm->spaceAvailable();
		return res;
	}	
	else
	{
		return 0;
	}
}

char GSM3CircularBuffer::peek(int increment)
{
	char res;
	byte num_aux;
	
	if (tail>head) num_aux = tail-head;
	else num_aux = 128 - head + tail;
	
	if(increment < num_aux)
	{
		res=theBuffer[head];
		return res;
	}	
	else
	{
		return 0;
	}
}

void GSM3CircularBufferManager::spaceAvailable(){return;};

void GSM3CircularBuffer::flush()
{
	head=tail;
}

char* GSM3CircularBuffer::nextString()
{
	while(head!=tail)
	{
		head=(head+1) & __BUFFERMASK__;
		if(theBuffer[head]==0)
		{
			head=(head+1) & __BUFFERMASK__;
			return (char*)theBuffer+head;
		}
	}
	return 0;
}


bool GSM3CircularBuffer::locate(const char* reference)
{

	return locate(reference, head, tail, 0, 0);
}

bool GSM3CircularBuffer::chopUntil(const char* reference, bool movetotheend, bool usehead)
{
	byte from, to;

	if(locate(reference, head, tail, &from, &to))
	{
		if(usehead)
		{
			if(movetotheend)
				head=(to+1) & __BUFFERMASK__;
			else
				head=from;
		}
		else
		{
			if(movetotheend)
				tail=(to+1) & __BUFFERMASK__;
			else
				tail=from;
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool GSM3CircularBuffer::locate(const char* reference, byte thishead, byte thistail, byte* from, byte* to)
{
	int refcursor=0;
	bool into=false;
	byte b2, binit;
	bool possible=1;
	
	if(reference[0]==0)
		return true;
		
	for(byte b1=thishead; b1!=thistail;b1=(b1+1)& __BUFFERMASK__)
	{
		possible = 1;
		b2 = b1;
		while (possible&&(b2!=thistail))
		{	
			if(theBuffer[b2]==reference[refcursor])
			{
				if(!into)	
					binit=b2;
				into=true;
				refcursor++;
				if(reference[refcursor]==0)
				{
					if(from) 
						*from=binit;
					if(to)	
						*to=b2;
					return true;
				}
			}
			else if (into==true)
			{
				possible = 0;
				into=false;
				refcursor=0;
			}
			b2=(b2+1)& __BUFFERMASK__;
		}
	}
	return false;
}

bool GSM3CircularBuffer::extractSubstring(const char* from, const char* to, char* buffer, int bufsize)
{
	byte t1;
	byte h2;
	byte b;
	int i;
	
//DEBUG
//Serial.println("Beginning extractSubstring");
//Serial.print("head,tail=");Serial.print(int(head));Serial.print(",");Serial.println(int(tail));
	
	if(!locate(from, head, tail, 0, &t1))
		return false;
		
//DEBUG
//Serial.println("Located chain from.");

	t1++; //To point the next.
	if(!locate(to, t1, tail, &h2, 0))
		return false;
		
//DEBUG		
//Serial.println("Located chain to.");
/*Serial.print("t1=");Serial.println(int(t1));
Serial.print("h2=");Serial.println(int(h2));*/


	for(i=0,b=t1;i<bufsize, b!=((h2) & __BUFFERMASK__); i++, b=(b+1)& __BUFFERMASK__)
		buffer[i]=theBuffer[b];
	buffer[i]=0;

//DEBUG		
//Serial.println("");
//Serial.println("Finishing extractSubstring");	
	
	return true;	
}

int GSM3CircularBuffer::readInt()
{
	int res=0;
	byte c;
	bool anyfound=false;
	bool negative=false;
	for(byte b=head + 1; b!=tail; b=(b+1)& __BUFFERMASK__)
	{
		c=theBuffer[b];
		if((c==' ' )&&(!anyfound))
		{
		} else if((c=='-' )&&(!anyfound))
		{
			negative=true;
			anyfound=true;  // Don't admit blanks after -
		} else if((c>='0')&&(c<='9'))
		{
			anyfound=true;
			res=(res*10)+(int)c-48;
		}
		else
			{
				if(negative)
					res=(-1)*res;
				return res;
			}
	}
	if(negative)
		res=(-1)*res;
	return res;
}

void GSM3CircularBuffer::debugBuffer()
{
	byte h1=head;
	byte t1=tail;
	Serial.println();
	Serial.print(h1);
	Serial.print(" ");
	Serial.print(t1);
	Serial.print('>');
	for(byte b=h1; b!=t1; b=(b+1)& __BUFFERMASK__)
		printCharDebug(theBuffer[b]);
	Serial.println();
}

void GSM3CircularBuffer::printCharDebug(uint8_t c)
{
	if((c>31)&&(c<127))
		Serial.print((char)c);
	else
	{
		Serial.print('%');
		Serial.print(c);
		Serial.print('%');
	}
}

bool GSM3CircularBuffer::retrieveBuffer(char* buffer, int bufsize, int& SizeWritten)
{
	byte b;
	int i;
	
	/*for(i=0,b=head;i<bufsize, b!=tail; i++, b=(b+1)& __BUFFERMASK__)
		{
			buffer[i]=theBuffer[b];
		}
	buffer[i]=0;
	SizeWritten = i;*/
	b=head;
	for(i=0;i<bufsize; i++)
		{
			if (b!=tail)
				{
					buffer[i]=theBuffer[b];
					buffer[i+1]=0;
					b=(b+1)& __BUFFERMASK__;
					SizeWritten = i + 1;
				}
		}
	
	
	return true;	
}



