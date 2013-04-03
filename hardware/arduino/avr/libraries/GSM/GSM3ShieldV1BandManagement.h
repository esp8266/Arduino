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
#ifndef __GSM3SHIELDV1BANDMANAGEMENT__
#define __GSM3SHIELDV1BANDMANAGEMENT__

// This class executes band management functions for the ShieldV1
#include <GSM3ShieldV1DirectModemProvider.h>

#define NUMBEROFBANDS 7
#define GSM_MODE_UNDEFINED "UNDEFINED"
#define GSM_MODE_EGSM "EGSM_MODE"
#define GSM_MODE_DCS "DCS_MODE"
#define GSM_MODE_PCS "PCS_MODE"
#define GSM_MODE_EGSM_DCS "EGSM_DCS_MODE"
#define GSM_MODE_GSM850_PCS "GSM850_PCS_MODE"
#define GSM_MODE_GSM850_EGSM_DCS_PCS "GSM850_EGSM_DCS_PCS_MODE"

typedef enum GSM3GSMBand {UNDEFINED, EGSM_MODE, DCS_MODE, PCS_MODE, EGSM_DCS_MODE, GSM850_PCS_MODE, GSM850_EGSM_DCS_PCS_MODE};

// 
// These are the bands and scopes:
//
//	E-GSM(900)
//  DCS(1800)
//  PCS(1900)
//	E-GSM(900)+DCS(1800) ex: Europe
//	GSM(850)+PCS(1900) Ex: USA, South Am.
//	GSM(850)+E-GSM(900)+DCS(1800)+PCS(1900)

class GSM3ShieldV1BandManagement
{
	private:
	
		GSM3ShieldV1DirectModemProvider modem; // Direct access to modem
		
		char* quectelStrings[NUMBEROFBANDS];// = {"\"EGSM_MODE\"", "\"DCS_MODE\"", "\"PCS_MODE\"",
								//"\"EGSM_DCS_MODE\"", "\"GSM850_PCS_MODE\"", 
								//"\"GSM850_EGSM_DCS_PCS_MODE\""};


	public:
	
		/** Constructor
			@param trace		If true, dumps all AT dialogue to Serial
		 */
		GSM3ShieldV1BandManagement(bool trace=false);
		
		/** Forces modem hardware restart, so we begin from scratch
			@return always returns IDLE status
		 */
		GSM3_NetworkStatus_t begin();

		/** Get current modem work band 
			@return current modem work band
		 */		
		String getBand();

		/** Changes the modem operating band 
			@param band			Desired new band
			@return true if success, false otherwise
		 */			
		bool setBand(String band);
			
};
#endif