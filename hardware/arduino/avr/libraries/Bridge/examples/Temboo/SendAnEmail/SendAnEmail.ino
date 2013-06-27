/*
  SendAnEmail

  Demonstrates sending an email via a Google Gmail account 
  using the Temboo Arduino Yun SDK.

  A Temboo account and application key are necessary to run all Temboo examples. 
  If you don't already have one, you can register for a free Temboo account at 
  http://www.temboo.com

  Since this sketch uses Gmail to send the email, you'll also need a valid 
  Google Gmail account. The sketch needs the username and password you use 
  to log into your Gmail account: substitute the placeholders below for these values.

  This example assumes basic familiarity with Arduino sketches, and that your Yun is connected
  to the Internet. For more tutorials on using the Temboo Library and Temboo Arduino Yun SDK, visit 
  http://www.temboo.com/arduino
  
  This example code is in the public domain.
*/

#include <Bridge.h>
#include <Console.h>
#include <FileIO.h>
#include <HttpClient.h>
#include <Process.h>
#include "TembooAccount.h" // contains Temboo account information, as described below


boolean success = false; // a flag to indicate whether we've sent the email yet or not

void setup() {
  Serial.begin(9600);

  // for debugging, wait until a serial console is connected
  delay(4000);
  while(!Serial);

  Bridge.begin();
}

void loop()
{
  // only try to send the email if we haven't already sent it successfully
  if (!success) {

    Serial.println("Running SendEmail ...");
    
    // we need a Process object to send a Choreo request to Temboo
    Process SendEmailChoreo;

    // invoke the Temboo client
    SendEmailChoreo.begin("temboo");
    
    // set Temboo account credentials
    SendEmailChoreo.addParameter("-a");
    SendEmailChoreo.addParameter(TEMBOO_ACCOUNT);
    SendEmailChoreo.addParameter("-u");
    SendEmailChoreo.addParameter(TEMBOO_APP_KEY_NAME);
    SendEmailChoreo.addParameter("-p");
    SendEmailChoreo.addParameter(TEMBOO_APP_KEY);

   	// identify the Temboo Library choreo to run (Google > Gmail > SendEmail)
    SendEmailChoreo.addParameter("-c");
    SendEmailChoreo.addParameter("/Library/Google/Gmail/SendEmail");

    // set the required choreo inputs
    // see https://www.temboo.com/library/Library/Google/Gmail/SendEmail/ 
    // for complete details about the inputs for this Choreo

    // the first input is a your Gmail user name. 
    // NOTE: substitute your own value, retaining the "Username:" prefix.
    SendEmailChoreo.addParameter("-i");
    SendEmailChoreo.addParameter("Username:your-gmail-username");
    
    // next is your Gmail password.
    // NOTE: substitute your own value, retaining the "Password:" prefix
    SendEmailChoreo.addParameter("-i");
    SendEmailChoreo.addParameter("Password:your-gmail-password");
 
    // then a subject line
    SendEmailChoreo.addParameter("-i");
    SendEmailChoreo.addParameter("Subject:ALERT: Greenhouse Temperature");
    
    // next comes the message body, the main content of the email
    SendEmailChoreo.addParameter("-i");
    SendEmailChoreo.addParameter("MessageBody:Hey! The greenhouse is too cold!");

    // next is the email address of who the email is from.
    // NOTE: substitute your Gmail address, retaining the "FromAddress:" prefix.
    SendEmailChoreo.addParameter("-i");
    SendEmailChoreo.addParameter("FromAddress:your-gmail-email-address");

    // who to send the email to
    // NOTE: substitute your desired recipient, retaining the "ToAddress:" prefix
    SendEmailChoreo.addParameter("-i");
    SendEmailChoreo.addParameter("ToAddress:recipient-email-address");
    
    // tell the Process to run and wait for the results. The 
    // return code (rc) will tell us whether the Temboo client 
    // was able to send our request to the Temboo servers
    unsigned int rc = SendEmailChoreo.run();

    // a return code of zero (0) means everything worked
    if (rc == 0) {
        Serial.println("Success! Email sent!");
        success = true;
    } else {
      // a non-zero return code means there was an error
      // read and print the error message
      while (SendEmailChoreo.available()) {
        Serial.print((char)SendEmailChoreo.read());
      }
    } 
    SendEmailChoreo.close();

    // do nothing for the next 60 seconds
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

  Visit https://www.temboo.com/account to access your Temboo account credentials. 

  Keeping your account information in a separate file means you can save it once, 
  then just distribute the main .ino file without worrying that you forgot to delete your credentials.

  (Be sure to delete this comment after creating your TembooAccount.h file!)
*/

