#include <GSM3ShieldV1DataNetworkProvider.h>
#include <Arduino.h>

const char _command_CGATT[] PROGMEM = "AT+CGATT=";
const char _command_SEPARATOR[] PROGMEM = "\",\"";

//Attach GPRS main function.	
GSM3_NetworkStatus_t GSM3ShieldV1DataNetworkProvider::attachGPRS(char* apn, char* user_name, char* password, bool synchronous)
{					
	user = user_name;
	passwd = password;
	// A sad use of byte reuse
	theGSM3ShieldV1ModemCore.setPhoneNumber(apn);

	theGSM3ShieldV1ModemCore.openCommand(this,ATTACHGPRS);
	theGSM3ShieldV1ModemCore.setStatus(CONNECTING);

	attachGPRSContinue();

	// If synchronous, wait till attach is over, or not.
	if(synchronous)
	{
		// if we shorten this delay, the command fails
		while(ready()==0) 
			delay(100); 
	}

	return theGSM3ShieldV1ModemCore.getStatus();	
}

//Atthach GPRS continue function.
void GSM3ShieldV1DataNetworkProvider::attachGPRSContinue()
{
	bool resp;
	// 1: Attach to GPRS service "AT+CGATT=1"
	// 2: Wait attach OK and Set the context 0 as FGCNT "AT+QIFGCNT=0"
	// 3: Wait context OK and Set bearer type as GPRS, APN, user name and pasword "AT+QICSGP=1..."
	// 4: Wait bearer OK and Enable the function of MUXIP "AT+QIMUX=1" 
	// 5: Wait for disable MUXIP OK and Set the session mode as non transparent "AT+QIMODE=0"
	// 6: Wait for session mode OK and Enable notification when data received "AT+QINDI=1"
	// 8: Wait domain name OK and Register the TCP/IP stack "AT+QIREGAPP"
	// 9: Wait for Register OK and Activate FGCNT "AT+QIACT"
	// 10: Wait for activate OK
	
	int ct=theGSM3ShieldV1ModemCore.getCommandCounter();
	if(ct==1)
	{
		//AT+CGATT	
		theGSM3ShieldV1ModemCore.genericCommand_rq(_command_CGATT,false);
		theGSM3ShieldV1ModemCore.print(1);
		theGSM3ShieldV1ModemCore.print('\r');
		theGSM3ShieldV1ModemCore.setCommandCounter(2);
	}
	else if(ct==2)
	{
		if(theGSM3ShieldV1ModemCore.genericParse_rsp(resp))
	    {
			if(resp)
			{
				//AT+QIFGCNT
				theGSM3ShieldV1ModemCore.genericCommand_rq(PSTR("AT+QIFGCNT=0"));
				theGSM3ShieldV1ModemCore.setCommandCounter(3);
			}
			else theGSM3ShieldV1ModemCore.closeCommand(3);
		}
	}
	else if(ct==3)
	{
		if(theGSM3ShieldV1ModemCore.genericParse_rsp(resp))
	    {
			if(resp)
			{
				// Great. Go for the next step
				//DEBUG
				//Serial.println("AT+QICSGP.");	
				theGSM3ShieldV1ModemCore.genericCommand_rq(PSTR("AT+QICSGP=1,\""),false);
				theGSM3ShieldV1ModemCore.print(theGSM3ShieldV1ModemCore.getPhoneNumber());
				theGSM3ShieldV1ModemCore.genericCommand_rq(_command_SEPARATOR,false);
				theGSM3ShieldV1ModemCore.print(user);
				theGSM3ShieldV1ModemCore.genericCommand_rq(_command_SEPARATOR,false);
				theGSM3ShieldV1ModemCore.print(passwd);
				theGSM3ShieldV1ModemCore.print("\"\r");
				theGSM3ShieldV1ModemCore.setCommandCounter(4);
			}
			else theGSM3ShieldV1ModemCore.closeCommand(3);
		}
	}
	else if(ct==4)
	{
		if(theGSM3ShieldV1ModemCore.genericParse_rsp(resp))
	    {
			if(resp)
			{
				// AT+QIMUX=1 for multisocket
				theGSM3ShieldV1ModemCore.genericCommand_rq(PSTR("AT+QIMUX=0"));
				theGSM3ShieldV1ModemCore.setCommandCounter(5);
			}
			else theGSM3ShieldV1ModemCore.closeCommand(3);
		}
	}
	else if(ct==5)
	{
		if(theGSM3ShieldV1ModemCore.genericParse_rsp(resp))
	    {
			if(resp)
			{
				//AT+QIMODE=0 for multisocket
				theGSM3ShieldV1ModemCore.genericCommand_rq(PSTR("AT+QIMODE=1"));
				theGSM3ShieldV1ModemCore.setCommandCounter(6);
			}
			else theGSM3ShieldV1ModemCore.closeCommand(3);
		}
	}
	else if(ct==6)
	{
		if(theGSM3ShieldV1ModemCore.genericParse_rsp(resp))
	    {
			if(resp)
			{
				// AT+QINDI=1
				theGSM3ShieldV1ModemCore.genericCommand_rq(PSTR("AT+QINDI=1"));
				theGSM3ShieldV1ModemCore.setCommandCounter(8);
			}
			else theGSM3ShieldV1ModemCore.closeCommand(3);
		}
	}
	else if(ct==8)
	{
		if(theGSM3ShieldV1ModemCore.genericParse_rsp(resp))
	    {
			if(resp)
			{
				// AT+QIREGAPP
				theGSM3ShieldV1ModemCore.genericCommand_rq(PSTR("AT+QIREGAPP"));
				theGSM3ShieldV1ModemCore.setCommandCounter(9);
			}
			else theGSM3ShieldV1ModemCore.closeCommand(3);
		}
	}
	else if(ct==9)
	{
		if(theGSM3ShieldV1ModemCore.genericParse_rsp(resp))
	    {
			if(resp)
			{
				// AT+QIACT	
				theGSM3ShieldV1ModemCore.genericCommand_rq(PSTR("AT+QIACT"));
				theGSM3ShieldV1ModemCore.setCommandCounter(10);
			}
			else theGSM3ShieldV1ModemCore.closeCommand(3);
		}
	}
	else if(ct==10)
	{
		if(theGSM3ShieldV1ModemCore.genericParse_rsp(resp))
	    {
			if (resp) 
				{
					theGSM3ShieldV1ModemCore.setStatus(GPRS_READY);
					theGSM3ShieldV1ModemCore.closeCommand(1);
				}
			else theGSM3ShieldV1ModemCore.closeCommand(3);
		}
	}
}

//Detach GPRS main function.
GSM3_NetworkStatus_t GSM3ShieldV1DataNetworkProvider::detachGPRS(bool synchronous)
{
	theGSM3ShieldV1ModemCore.openCommand(this,DETACHGPRS);
	theGSM3ShieldV1ModemCore.setStatus(CONNECTING);
	detachGPRSContinue();
	
	if(synchronous)
	{
		while(ready()==0) 
			delay(1); 
	}
	
	return theGSM3ShieldV1ModemCore.getStatus();
}

void GSM3ShieldV1DataNetworkProvider::detachGPRSContinue()
{
	bool resp;
	// 1: Detach to GPRS service "AT+CGATT=0"
	// 2: Wait dettach +PDP DEACT 
	// 3: Wait for OK

	switch (theGSM3ShieldV1ModemCore.getCommandCounter()) {
	case 1:
		//AT+CGATT=0
		theGSM3ShieldV1ModemCore.genericCommand_rq(_command_CGATT,false);
		theGSM3ShieldV1ModemCore.print(0);
		theGSM3ShieldV1ModemCore.print('\r');
		theGSM3ShieldV1ModemCore.setCommandCounter(2);
		break;
	case 2:
		char auxLocate[12];
		prepareAuxLocate(PSTR("+PDP DEACT"), auxLocate);
		if(theGSM3ShieldV1ModemCore.theBuffer().locate(auxLocate))
	    {
			if(resp)
			{
				// Received +PDP DEACT;				
				theGSM3ShieldV1ModemCore.setCommandCounter(3);
			}
			else theGSM3ShieldV1ModemCore.closeCommand(3);
		}	
		break;
	case 3:
		if(theGSM3ShieldV1ModemCore.genericParse_rsp(resp))
	    {
			// OK received
			if (resp) 
				{
					theGSM3ShieldV1ModemCore.setStatus(GSM_READY);
					theGSM3ShieldV1ModemCore.closeCommand(1);
				}
			else theGSM3ShieldV1ModemCore.closeCommand(3);
		}		
		break;	
	}
}

//QILOCIP parse.
bool GSM3ShieldV1DataNetworkProvider::parseQILOCIP_rsp(char* LocalIP, int LocalIPlength, bool& rsp)
{
	if (!(theGSM3ShieldV1ModemCore.theBuffer().extractSubstring("\r\n","\r\n", LocalIP, LocalIPlength)))
		rsp = false;
	else 
		rsp = true;
	return true;
}

//Get IP main function.
int GSM3ShieldV1DataNetworkProvider::getIP(char* LocalIP, int LocalIPlength)
{
	theGSM3ShieldV1ModemCore.setPhoneNumber(LocalIP);
	theGSM3ShieldV1ModemCore.setPort(LocalIPlength);
	theGSM3ShieldV1ModemCore.openCommand(this,GETIP);
	getIPContinue();
	return theGSM3ShieldV1ModemCore.getCommandError();
}

void GSM3ShieldV1DataNetworkProvider::getIPContinue()
{

	bool resp;
	// 1: Read Local IP "AT+QILOCIP"
	// 2: Waiting for IP.

	switch (theGSM3ShieldV1ModemCore.getCommandCounter()) {
	case 1:
		//AT+QILOCIP
		theGSM3ShieldV1ModemCore.genericCommand_rq(PSTR("AT+QILOCIP"));
		theGSM3ShieldV1ModemCore.setCommandCounter(2);
		break;
	case 2:
		if(parseQILOCIP_rsp(theGSM3ShieldV1ModemCore.getPhoneNumber(), theGSM3ShieldV1ModemCore.getPort(), resp))
	    {
			if (resp) 
				theGSM3ShieldV1ModemCore.closeCommand(1);
			else 
				theGSM3ShieldV1ModemCore.closeCommand(3);
		}
		theGSM3ShieldV1ModemCore.theBuffer().flush();
		theGSM3ShieldV1ModemCore.gss.spaceAvailable();
		break;	
	}
}

//Get IP with IPAddress object
IPAddress GSM3ShieldV1DataNetworkProvider::getIPAddress() {
	char ip_temp[15]="";
	getIP(ip_temp, 15);
	unsigned long m=millis();

	while((millis()-m)<10*1000 && (!ready())){
		// wait for a response from the modem:
		delay(100);
	} 
	IPAddress ip;
	inet_aton(ip_temp, ip);
	return ip;
}

int GSM3ShieldV1DataNetworkProvider::inet_aton(const char* aIPAddrString, IPAddress& aResult)
{
    // See if we've been given a valid IP address
    const char* p =aIPAddrString;
    while (*p &&
           ( (*p == '.') || (*p >= '0') || (*p <= '9') ))
    {
        p++;
    }

    if (*p == '\0')
    {
        // It's looking promising, we haven't found any invalid characters
        p = aIPAddrString;
        int segment =0;
        int segmentValue =0;
        while (*p && (segment < 4))
        {
            if (*p == '.')
            {
                // We've reached the end of a segment
                if (segmentValue > 255)
                {
                    // You can't have IP address segments that don't fit in a byte
                    return 0;
                }
                else
                {
                    aResult[segment] = (byte)segmentValue;
                    segment++;
                    segmentValue = 0;
                }
            }
            else
            {
                // Next digit
                segmentValue = (segmentValue*10)+(*p - '0');
            }
            p++;
        }
        // We've reached the end of address, but there'll still be the last
        // segment to deal with
        if ((segmentValue > 255) || (segment > 3))
        {
            // You can't have IP address segments that don't fit in a byte,
            // or more than four segments
            return 0;
        }
        else
        {
            aResult[segment] = (byte)segmentValue;
            return 1;
        }
    }
    else
    {
        return 0;
    }
}

//Response management.
void GSM3ShieldV1DataNetworkProvider::manageResponse(byte from, byte to)
{
	switch(theGSM3ShieldV1ModemCore.getOngoingCommand())
	{
		case ATTACHGPRS:
			attachGPRSContinue();
			break;	
		case DETACHGPRS:
			detachGPRSContinue();
			break;
		case GETIP:
			getIPContinue();
			break;		
	}
}
