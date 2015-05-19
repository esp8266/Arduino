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
#include <GSM3ShieldV1BandManagement.h>

GSM3ShieldV1BandManagement::GSM3ShieldV1BandManagement(bool trace): modem(trace)
{
	quectelStrings[UNDEFINED]="";
	quectelStrings[EGSM_MODE]="\"EGSM_MODE\"";
	quectelStrings[DCS_MODE]="\"DCS_MODE\"";
	quectelStrings[PCS_MODE]="\"PCS_MODE\"";
	quectelStrings[EGSM_DCS_MODE]="\"EGSM_DCS_MODE\"";
	quectelStrings[GSM850_PCS_MODE]="\"GSM850_PCS_MODE\"";
	quectelStrings[GSM850_EGSM_DCS_PCS_MODE]="\"GSM850_EGSM_DCS_PCS_MODE\"";	
}

GSM3_NetworkStatus_t GSM3ShieldV1BandManagement::begin()
{
	// check modem response
	modem.begin();

	// reset hardware
	modem.restartModem();

	return IDLE;
}

String GSM3ShieldV1BandManagement::getBand()
{
  String modemResponse=modem.writeModemCommand("AT+QBAND?", 2000);
	
  for(GSM3GSMBand i=GSM850_EGSM_DCS_PCS_MODE;i>UNDEFINED;i=(GSM3GSMBand)((int)i-1))
  {
    if(modemResponse.indexOf(quectelStrings[i])>=0)
      return quectelStrings[i];
  }

  Serial.print("Unrecognized modem answer:");
  Serial.println(modemResponse);
  
  return "";
}

bool GSM3ShieldV1BandManagement::setBand(String band)
{
	String command;
	String modemResponse;
	bool found=false;

	command="AT+QBAND=";
	for(GSM3GSMBand i=EGSM_MODE;((i<=GSM850_EGSM_DCS_PCS_MODE)&&(!found));i=(GSM3GSMBand)((int)i+1))
	{
		String aux=quectelStrings[i];
		if(aux.indexOf(band)>=0)
		{
		    command+=aux;
			found=true;
		}
	}
	
	if(!found)
		return false;
	// Quad-band takes an awful lot of time
    modemResponse=modem.writeModemCommand(command, 15000);
	
	if(modemResponse.indexOf("QBAND")>=0)
      return true;
	else
		return false;
}
