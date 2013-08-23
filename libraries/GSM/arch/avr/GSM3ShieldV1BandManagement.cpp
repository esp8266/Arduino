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