/*
  SendAnSMS

  Demonstrates sending an SMS via a Twilio account using the Temboo Arduino Yun SDK.

  Check out the latest Arduino & Temboo examples and support docs at http://www.temboo.com/arduino

  A Temboo account and application key are necessary to run all Temboo examples. 
  If you don't already have one, you can register for a free Temboo account at 
  http://www.temboo.com

  Since this sketch uses Twilio to send the SMS, you'll also need a valid 
  Twilio account. You can create one for free at https://www.twilio.com.
 
  The sketch needs your Twilio phone number, along with
  the Account SID and Auth Token you get when you register with Twilio.
  Make sure to use the Account SID and Auth Token from your Twilio Dashboard 
  (not your test credentials from the Dev Tools panel).

  Also note that if you're using a free Twilio account, you'll need to verify 
  the phone number to which messages are being sent by going to twilio.com and following
  the instructions under the "Numbers > Verified Caller IDs" tab (this restriction
  doesn't apply if you have a paid Twilio account).
  
  This example assumes basic familiarity with Arduino sketches, and that your Yun is connected
  to the Internet.

  Looking for another API? We've got over 100 in our Library!
  
  This example code is in the public domain.
*/

#include <Bridge.h>
#include <Console.h>
#include <FileIO.h>
#include <HttpClient.h>
#include <Process.h>
#include "TembooAccount.h" // contains Temboo account information
                           // as described in the footer comment below



/*** SUBSTITUTE YOUR VALUES BELOW: ***/

// the Account SID from your Twilio account
const String TWILIO_ACCOUNT_SID = "xxxxxxxxxx";

// the Auth Token from your Twilio account
const String TWILIO_AUTH_TOKEN = "xxxxxxxxxx";

// your Twilio phone number, e.g., "+1 555-222-1212"
const String TWILIO_NUMBER = "xxxxxxxxxx";

// the number to which the SMS should be sent, e.g., "+1 555-222-1212"
const String RECIPIENT_NUMBER = "xxxxxxxxxx";

boolean success = false; // a flag to indicate whether we've sent the SMS yet or not

void setup() {
  Serial.begin(9600);

  // for debugging, wait until a serial console is connected
  delay(4000);
  while(!Serial);

  Bridge.begin();
}

void loop()
{
  // only try to send the SMS if we haven't already sent it successfully
  if (!success) {

    Serial.println("Running SendAnSMS...");
    
    // we need a Process object to send a Choreo request to Temboo
    Process SendSMSChoreo;

    // invoke the Temboo client
    SendSMSChoreo.begin("temboo");
    
    // set Temboo account credentials
    SendSMSChoreo.addParameter("-a");
    SendSMSChoreo.addParameter(TEMBOO_ACCOUNT);
    SendSMSChoreo.addParameter("-u");
    SendSMSChoreo.addParameter(TEMBOO_APP_KEY_NAME);
    SendSMSChoreo.addParameter("-p");
    SendSMSChoreo.addParameter(TEMBOO_APP_KEY);

    // identify the Temboo Library choreo to run (Twilio > SMSMessages > SendSMS)
    SendSMSChoreo.addParameter("-c");
    SendSMSChoreo.addParameter("/Library/Twilio/SMSMessages/SendSMS");

    // set the required choreo inputs
    // see https://www.temboo.com/library/Library/Twilio/SMSMessages/SendSMS/ 
    // for complete details about the inputs for this Choreo

    // the first input is a your AccountSID
    SendSMSChoreo.addParameter("-i");
    SendSMSChoreo.addParameter("AccountSID:" + TWILIO_ACCOUNT_SID);
    
    // next is your Auth Token
    SendSMSChoreo.addParameter("-i");
    SendSMSChoreo.addParameter("AuthToken:" + TWILIO_AUTH_TOKEN);
 
    // next is your Twilio phone number
    SendSMSChoreo.addParameter("-i");
    SendSMSChoreo.addParameter("From:" + TWILIO_NUMBER);
    
    // next, what number to send the SMS to
    SendSMSChoreo.addParameter("-i");
    SendSMSChoreo.addParameter("To:" + RECIPIENT_NUMBER);

    // finally, the text of the message to send
    SendSMSChoreo.addParameter("-i");
    SendSMSChoreo.addParameter("Body:Hey, there! This is a message from your Arduino Yun!");
    
    // tell the Process to run and wait for the results. The 
    // return code (rc) will tell us whether the Temboo client 
    // was able to send our request to the Temboo servers
    unsigned int rc = SendSMSChoreo.run();

    // a return code of zero (0) means everything worked
    if (rc == 0) {
        Serial.println("Success! SMS sent!");
        success = true;
    } else {
      // a non-zero return code means there was an error
      // read and print the error message
      while (SendSMSChoreo.available()) {
        Serial.print((char)SendSMSChoreo.read());
      }
    } 
    SendSMSChoreo.close();

    // do nothing for the next 60 seconds
    Serial.println("Sleeping...");
    delay(60000);
  }
}

/*
  IMPORTANT NOTE: TembooAccount.h:

  TembooAccount.h is a file referenced by this sketch that contains your Temboo account information. 
  You need to create this file. To do so, make a new tab in Arduino, call it TembooAccount.h, and 
  include the following variables and constants:

  #define TEMBOO_ACCOUNT "myTembooAccountName"  // your Temboo account name 
  #define TEMBOO_APP_KEY_NAME "myFirstApp"  // your Temboo app key name
  #define TEMBOO_APP_KEY  "xxx-xxx-xxx-xx-xxx"  // your Temboo app key

  The same TembooAccount.h file settings can be used for all Temboo SDK sketches.

  You can find your Temboo App Key information on the Temboo website, 
  under My Account > Application Keys

  Keeping your account information in a separate file means you can save it once, 
  then just distribute the main .ino file without worrying that you forgot to delete your credentials.
*/