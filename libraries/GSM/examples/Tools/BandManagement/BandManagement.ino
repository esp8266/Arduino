/*
 Band Management
 
 This sketch, for the Arduino GSM shield, checks the band
 currently configured in the modem and allows you to change 
 it.
 
 Please check http://www.worldtimezone.com/gsm.html
 Usual configurations:
 Europe, Africa, Middle East: E-GSM(900)+DCS(1800)
 USA, Canada, South America: GSM(850)+PCS(1900)
 Mexico: PCS(1900)
 Brazil: GSM(850)+E-GSM(900)+DCS(1800)+PCS(1900)
 
 
 Circuit:
 * GSM shield 
 
 created 12 June 2012
 by Javier Zorzano, Scott Fitzgerald
 
 This example is in the public domain.
 */

// libraries
#include <GSM.h>

// initialize the library instance
GSMBand band;

void setup()
{
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  
  // Beginning the band manager restarts the modem
  Serial.println("Restarting modem...");
  band.begin();
  Serial.println("Modem restarted.");
  
};


void loop()
{
  // Get current band 
  String bandName = band.getBand(); // Get and print band name
  Serial.print("Current band:");
  Serial.println(bandName);
  Serial.println("Want to change the band you’re on?");
  String newBandName;
  newBandName = askUser();
  // Tell the user what we are about to do…
  Serial.print("\nConfiguring band "); 
  Serial.println(newBandName); 
  // Change the band 
  boolean operationSuccess; 
  operationSuccess = band.setBand(newBandName); 
  // Tell the user if the operation was OK 
  if(operationSuccess)
  { 
    Serial.println("Success");
  } 
  else
  { 
    Serial.println("Error while changing band");
  } 
   
  if(operationSuccess)
  { 
    while(true); 
  } 
}

// This function offers the user different options
// through the Serial interface
// The user selects one
String askUser()
{
  String newBand; 
  Serial.println("Select band:"); 
  // Print the different options 
  Serial.println("1 : E-GSM(900)"); 
  Serial.println("2 : DCS(1800)"); 
  Serial.println("3 : PCS(1900)"); 
  Serial.println("4 : E-GSM(900)+DCS(1800) ex: Europe"); 
  Serial.println("5 : GSM(850)+PCS(1900) Ex: USA, South Am."); 
  Serial.println("6 : GSM(850)+E-GSM(900)+DCS(1800)+PCS(1900)"); 
  
  // Empty the incoming buffer 
  while(Serial.available()) 
  Serial.read(); 
  
  // Wait for an answer, just look at the first character 
  while(!Serial.available()); 
  char c= Serial.read(); 
  if(c=='1')
    newBand=GSM_MODE_EGSM; 
  else if(c=='2')
    newBand=GSM_MODE_DCS; 
  else if(c=='3')
    newBand=GSM_MODE_PCS; 
  else if(c=='4')
    newBand=GSM_MODE_EGSM_DCS; 
  else if(c=='5')
    newBand=GSM_MODE_GSM850_PCS; 
  else if(c=='6')
    newBand=GSM_MODE_GSM850_EGSM_DCS_PCS; 
  else
    newBand="GSM_MODE_UNDEFINED";
  return newBand;
}

 



