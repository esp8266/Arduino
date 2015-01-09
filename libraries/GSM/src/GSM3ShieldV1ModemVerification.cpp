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

#include <GSM3ShieldV1ModemVerification.h>

// constructor
GSM3ShieldV1ModemVerification::GSM3ShieldV1ModemVerification()
{
};

// reset the modem for direct access
int GSM3ShieldV1ModemVerification::begin()
{
	int result=0;
	String modemResponse;
	
	// check modem response
	modemAccess.begin();

	// reset hardware
	modemAccess.restartModem();

	modemResponse=modemAccess.writeModemCommand("AT", 1000);
	if(modemResponse.indexOf("OK")>=0)
		result=1;
	modemResponse=modemAccess.writeModemCommand("ATE0", 1000);
	return result;
}

// get IMEI
String GSM3ShieldV1ModemVerification::getIMEI()
{
	String number(NULL);
	// AT command for obtain IMEI
	String modemResponse = modemAccess.writeModemCommand("AT+GSN", 2000);
	// Parse and check response
	char res_to_compare[modemResponse.length()];
	modemResponse.toCharArray(res_to_compare, modemResponse.length());
	if(strstr(res_to_compare,"OK") != NULL)
		number = modemResponse.substring(1, 17);
	return number;
}
