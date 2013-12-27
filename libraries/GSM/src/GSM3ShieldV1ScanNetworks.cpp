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

#include <GSM3ShieldV1ScanNetworks.h>

GSM3ShieldV1ScanNetworks::GSM3ShieldV1ScanNetworks(bool trace): modem(trace)
{	
}

GSM3_NetworkStatus_t GSM3ShieldV1ScanNetworks::begin()
{
	modem.begin();
	modem.restartModem();
	// check modem response
	modem.writeModemCommand("AT", 1000);
	modem.writeModemCommand("ATE0", 1000);
	return IDLE;
}

String GSM3ShieldV1ScanNetworks::getCurrentCarrier()
{
	String modemResponse = modem.writeModemCommand("AT+COPS?", 2000);
	
	// Parse and check response
	char res_to_split[modemResponse.length()];
	modemResponse.toCharArray(res_to_split, modemResponse.length());
	if(strstr(res_to_split,"ERROR") == NULL){
		// Tokenizer
		char *ptr_token;
		ptr_token = strtok(res_to_split, "\"");
		ptr_token = strtok(NULL, "\"");
		String final_result = ptr_token;
		return final_result;
	}else{
		return String(NULL);
	}
}

String GSM3ShieldV1ScanNetworks::getSignalStrength()
{
	String modemResponse = modem.writeModemCommand("AT+CSQ", 2000);
	char res_to_split[modemResponse.length()];
	modemResponse.toCharArray(res_to_split, modemResponse.length());
	if((strstr(res_to_split,"ERROR") == NULL) | (strstr(res_to_split,"99") == NULL)){
		// Tokenizer
		char *ptr_token;
		ptr_token = strtok(res_to_split, ":");
		ptr_token = strtok(NULL, ":");
		ptr_token = strtok(ptr_token, ",");
		String final_result = ptr_token;
		final_result.trim();
		return final_result;
	}else{
		return String(NULL);
	}
}

String GSM3ShieldV1ScanNetworks::readNetworks()
{
	String modemResponse = modem.writeModemCommand("AT+COPS=?",20000);
	String result;
	bool inQuotes=false;
	int quoteCounter=0;
	for(int i=0; i<modemResponse.length();i++)
	{
		if(modemResponse[i]=='"')
		{
			if(!inQuotes)
			{
				inQuotes=true;
				quoteCounter++;
				if(quoteCounter==1)
					result+="> ";
			}
			else
			{
				inQuotes=false;
				if(quoteCounter==3)
					quoteCounter=0;
				if(quoteCounter==1)
					result+="\n";
				
			}
		}
		else
		{
			if(inQuotes&&(quoteCounter==1))
			{
				result+=modemResponse[i];
			}
		}
	}
	return result;
}

