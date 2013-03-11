/*
 Receive Voice Call
 
 This sketch, for the Arduino GSM shield, receives voice calls, 
 displays the calling number, waits a few seconds then hangs up.
 
 Circuit:
 * GSM shield 
 * Voice circuit. Refer to to the GSM shield getting started guide
   at http://arduino.cc/en/Guide/ArduinoGSMShield#toc11
 * SIM card that can accept voice calls
 
 With no voice circuit the call will connect, but will not send or receive sound
  
 created Mar 2012
 by Javier Zorzano
 
 This example is in the public domain.
 
 http://arduino.cc/en/Tutorial/GSMExamplesReceiveVoiceCall
 
 */

// Include the GSM library
#include <GSM.h>

// PIN Number
#define PINNUMBER ""

// initialize the library instance
GSM gsmAccess;
GSMVoiceCall vcs;

// Array to hold the number for the incoming call
char numtel[20];           

void setup()
{
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  Serial.println("Receive Voice Call");
  
  // connection state
  boolean notConnected = true;
  
  // Start GSM shield
  // If your SIM has PIN, pass it as a parameter of begin() in quotes
  while(notConnected)
  {
    if(gsmAccess.begin(PINNUMBER)==GSM_READY)
      notConnected = false;
    else
    {
      Serial.println("Not connected");
      delay(1000);
    }
  }
  
  // This makes sure the modem correctly reports incoming events
  vcs.hangCall();
  
  Serial.println("Waiting for a call");
}

void loop()
{
  // Check the status of the voice call
  switch (vcs.getvoiceCallStatus()) 
  {
    case IDLE_CALL: // Nothing is happening
      
      break;
      
    case RECEIVINGCALL: // Yes! Someone is calling us
      
      Serial.println("RECEIVING CALL");
      
      // Retrieve the calling number
      vcs.retrieveCallingNumber(numtel, 20);
      
      // Print the calling number
      Serial.print("Number:");
      Serial.println(numtel);
      
      // Answer the call, establish the call
      vcs.answerCall();         
      break;
      
    case TALKING:  // In this case the call would be established
      
      Serial.println("TALKING. Press enter to hang up.");
      while(Serial.read()!='\n')
        delay(100);
      vcs.hangCall();
      Serial.println("Hanging up and waiting for the next call.");      
      break;
  }
  delay(1000);
}


