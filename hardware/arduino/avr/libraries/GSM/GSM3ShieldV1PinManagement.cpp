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

#include <GSM3ShieldV1PinManagement.h>

// constructor
GSM3ShieldV1PinManagement::GSM3ShieldV1PinManagement()
{
};

// reset the modem for direct access
void GSM3ShieldV1PinManagement::begin()
{
	// reset hardware
	gsm.HWrestart();

	pin_used = false;
	
	// check modem response
	modemAccess.writeModemCommand("AT", 1000);
	modemAccess.writeModemCommand("ATE0", 1000);
}

/*
  Check PIN status
*/
int GSM3ShieldV1PinManagement::isPIN()
{
  String res = modemAccess.writeModemCommand("AT+CPIN?",1000);
  // Check response
  char res_to_compare[res.length()];
  res.toCharArray(res_to_compare, res.length());
  if(strstr(res_to_compare, "READY") != NULL)
    return 0;
  else if(strstr(res_to_compare, "SIM PIN") != NULL)
    return 1; 
  else if(strstr(res_to_compare, "SIM PUK") != NULL)
    return -1;
  else
    return -2;
}

/*
  Check PIN code
*/
int GSM3ShieldV1PinManagement::checkPIN(String pin)
{
  String res = modemAccess.writeModemCommand("AT+CPIN=" + pin,1000);
  // check response
  char res_to_compare[res.length()];
  res.toCharArray(res_to_compare, res.length());
  if(strstr(res_to_compare, "OK") == NULL)
    return -1;
  else
    return 0;
}

/*
  Check PUK code
*/
int GSM3ShieldV1PinManagement::checkPUK(String puk, String pin)
{
  String res = modemAccess.writeModemCommand("AT+CPIN=\"" + puk + "\",\"" + pin + "\"",1000);
  // check response
  char res_to_compare[res.length()];
  res.toCharArray(res_to_compare, res.length());
  if(strstr(res_to_compare, "OK") == NULL)
    return -1;
  else
    return 0;
}

/*
  Change PIN code
*/
void GSM3ShieldV1PinManagement::changePIN(String old, String pin)
{
  String res = modemAccess.writeModemCommand("AT+CPWD=\"SC\",\"" + old + "\",\"" + pin + "\"",2000);
  Serial.println(res);
  // check response
  char res_to_compare[res.length()];
  res.toCharArray(res_to_compare, res.length());
  if(strstr(res_to_compare, "OK") != NULL)
    Serial.println("Pin changed succesfully.");
  else
    Serial.println("ERROR");
}

/*
  Switch PIN status
*/
void GSM3ShieldV1PinManagement::switchPIN(String pin)
{
  String res = modemAccess.writeModemCommand("AT+CLCK=\"SC\",2",1000);
  // check response
  char res_to_compare[res.length()];
  res.toCharArray(res_to_compare, res.length());
  if(strstr(res_to_compare, "0") != NULL)
  {
    res = modemAccess.writeModemCommand("AT+CLCK=\"SC\",1,\"" + pin + "\"",1000);
    // check response
    char res_to_compare[res.length()];
    res.toCharArray(res_to_compare, res.length());
    if(strstr(res_to_compare, "OK") == NULL)
    {
      Serial.println("ERROR");
      pin_used = false;
    }
    else
    {
      Serial.println("OK. PIN lock on.");
      pin_used = true;
    }
  }
  else if(strstr(res_to_compare, "1") != NULL)
  {
    res = modemAccess.writeModemCommand("AT+CLCK=\"SC\",0,\"" + pin + "\"",1000);
    // check response
    char res_to_compare[res.length()];
    res.toCharArray(res_to_compare, res.length());
    if(strstr(res_to_compare, "OK") == NULL)
    {
      Serial.println("ERROR");
      pin_used = true;
    }
    else
    {
      Serial.println("OK. PIN lock off.");
      pin_used = false;
    }
  }
  else
  {
    Serial.println("ERROR");
  }
}

/*
  Check registrer
*/
int GSM3ShieldV1PinManagement::checkReg()
{
  delay(5000);
  String res = modemAccess.writeModemCommand("AT+CREG?",1000);
  // check response
  char res_to_compare[res.length()];
  res.toCharArray(res_to_compare, res.length());
  if(strstr(res_to_compare, "1") != NULL)
    return 0;
  else if(strstr(res_to_compare, "5") != NULL)
    return 1;
  else
    return -1; 
}

/*
  Return if PIN lock is used
*/
bool GSM3ShieldV1PinManagement::getPINUsed()
{
	return pin_used;
}

/*
  Set if PIN lock is used
*/
void GSM3ShieldV1PinManagement::setPINUsed(bool used)
{
	pin_used = used;
}