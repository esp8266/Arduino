/*
  SendAnSMS

  Demonstrates sending an SMS via Twilio using Temboo from an Arduino Yún.

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
  
  This example assumes basic familiarity with Arduino sketches, and that your Yún is connected
  to the Internet.

  Looking for another API to use with your Arduino Yún? We've got over 100 in our Library!
  
  This example code is in the public domain.
*/

#include <Bridge.h>
#include <Temboo.h>
#include "TembooAccount.h" // contains Temboo account information
                           // as described in the footer comment below



/*** SUBSTITUTE YOUR VALUES BELOW: ***/

// Note that for additional security and reusability, you could
// use #define statements to specify these values in a .h file.

// the Account SID from your Twilio account
const String TWILIO_ACCOUNT_SID = "xxxxxxxxxx";

// the Auth Token from your Twilio account
const String TWILIO_AUTH_TOKEN = "xxxxxxxxxx";

// your Twilio phone number, e.g., "+1 555-222-1212"
const String TWILIO_NUMBER = "xxxxxxxxxx";

// the number to which the SMS should be sent, e.g., "+1 555-222-1212"
const String RECIPIENT_NUMBER = "xxxxxxxxxx";

// a flag to indicate whether we've attempted to send the SMS yet or not
boolean attempted = false; 

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
  if (!attempted) {

    Serial.println("Running SendAnSMS...");
    
    // we need a Process object to send a Choreo request to Temboo
    TembooChoreo SendSMSChoreo;

    // invoke the Temboo client
    // NOTE that the client must be reinvoked and repopulated with
    // appropriate arguments each time its run() method is called.
    SendSMSChoreo.begin();
    
    // set Temboo account credentials
    SendSMSChoreo.setAccountName(TEMBOO_ACCOUNT);
    SendSMSChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    SendSMSChoreo.setAppKey(TEMBOO_APP_KEY);

    // identify the Temboo Library choreo to run (Twilio > SMSMessages > SendSMS)
    SendSMSChoreo.setChoreo("/Library/Twilio/SMSMessages/SendSMS");

    // set the required choreo inputs
    // see https://www.temboo.com/library/Library/Twilio/SMSMessages/SendSMS/ 
    // for complete details about the inputs for this Choreo

    // the first input is a your AccountSID
    SendSMSChoreo.addInput("AccountSID", TWILIO_ACCOUNT_SID);
    
    // next is your Auth Token
    SendSMSChoreo.addInput("AuthToken", TWILIO_AUTH_TOKEN);
 
    // next is your Twilio phone number
    SendSMSChoreo.addInput("From", TWILIO_NUMBER);
    
    // next, what number to send the SMS to
    SendSMSChoreo.addInput("To", RECIPIENT_NUMBER);

    // finally, the text of the message to send
    SendSMSChoreo.addInput("Body", "Hey, there! This is a message from your Arduino Yun!");
    
    // tell the Process to run and wait for the results. The 
    // return code (returnCode) will tell us whether the Temboo client 
    // was able to send our request to the Temboo servers
    unsigned int returnCode = SendSMSChoreo.run();

    // a return code of zero (0) means everything worked
    if (returnCode == 0) {
        Serial.println("Success! SMS sent!");
    } else {
      // a non-zero return code means there was an error
      // read and print the error message
      while (SendSMSChoreo.available()) {
        char c = SendSMSChoreo.read();
        Serial.print(c);
      }
    } 
    SendSMSChoreo.close();
    
    // set the flag indicatine we've tried once.
    attempted=true;
  }
}

/*
  IMPORTANT NOTE: TembooAccount.h:

  TembooAccount.h is a file referenced by this sketch that contains your Temboo account information.
  You'll need to edit the placeholder version of TembooAccount.h included with this example sketch,
  by inserting your own Temboo account name and app key information. The contents of the file should
  look like:

  #define TEMBOO_ACCOUNT "myTembooAccountName"  // your Temboo account name 
  #define TEMBOO_APP_KEY_NAME "myFirstApp"  // your Temboo app key name
  #define TEMBOO_APP_KEY  "xxx-xxx-xxx-xx-xxx"  // your Temboo app key

  You can find your Temboo App Key information on the Temboo website, 
  under My Account > Application Keys

  The same TembooAccount.h file settings can be used for all Temboo SDK sketches.

  Keeping your account information in a separate file means you can share the main .ino file without worrying 
  that you forgot to delete your credentials.
*/
