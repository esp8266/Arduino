/*
  SendAnEmail

  Demonstrates sending an email via a Google Gmail account using Temboo from an Arduino Yún.

  Check out the latest Arduino & Temboo examples and tutorials at http://www.temboo.com/arduino

  A Temboo account and application key are necessary to run all Temboo examples. 
  If you don't already have one, you can register for a free Temboo account at 
  http://www.temboo.com

 Instructions:
 
  1. Create a Temboo account: http://www.temboo.com
  
  2. Retrieve your Temboo application details: http://www.temboo.com/account/applications
  
  3. Replace the values in the TembooAccount.h tab with your Temboo application details
 
  4. You'll also need a Gmail account. Update the placeholder Gmail address in the code 
     below with your own details.
     
     https://www.gmail.com
 
  5. Once you have a Gmail account, turn on 2-step authentication, and create an application-specific 
     password to allow Temboo to access your Google account: https://www.google.com/landing/2step/.
     
  6. After you've enabled 2-Step authentication, you'll need to create an App Password:
     https://security.google.com/settings/security/apppasswords
  
  7. In the "Select app" dropdown menu, choose "Other", and give your app a name (e.g., TembooApp).
  
  8. Click "Generate". You'll be given a 16-digit passcode that can be used to access your Google Account from Temboo.
 
  9. Copy and paste this password into the code below, updating the GMAIL_APP_PASSWORD variable
 
  10. Upload the sketch to your Arduino Yún and open the serial monitor
  
  NOTE: You can test this Choreo and find the latest instructions on our website: 
  https://temboo.com/library/Library/Google/Gmail/SendEmail
  
  You can also find an in-depth version of this example here:
  https://temboo.com/arduino/yun/send-an-email

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

// your Gmail username, formatted as a complete email address, eg "bob.smith@gmail.com"
const String GMAIL_USER_NAME = "xxxxxxxxxx";

// your application specific password (see instructions above)
const String GMAIL_APP_PASSWORD = "xxxxxxxxxx";

// the email address you want to send the email to, eg "jane.doe@temboo.com"
const String TO_EMAIL_ADDRESS = "xxxxxxxxxx";

// a flag to indicate whether we've tried to send the email yet or not
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
  // only try to send the email if we haven't already tried
  if (!attempted) {

    Serial.println("Running SendAnEmail...");
  
    TembooChoreo SendEmailChoreo;

    // invoke the Temboo client
    // NOTE that the client must be reinvoked, and repopulated with
    // appropriate arguments, each time its run() method is called.
    SendEmailChoreo.begin();
    
    // set Temboo account credentials
    SendEmailChoreo.setAccountName(TEMBOO_ACCOUNT);
    SendEmailChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    SendEmailChoreo.setAppKey(TEMBOO_APP_KEY);

    // identify the Temboo Library choreo to run (Google > Gmail > SendEmail)
    SendEmailChoreo.setChoreo("/Library/Google/Gmail/SendEmail");
 

    // set the required choreo inputs
    // see https://www.temboo.com/library/Library/Google/Gmail/SendEmail/ 
    // for complete details about the inputs for this Choreo

    // the first input is your Gmail email address.     
    SendEmailChoreo.addInput("Username", GMAIL_USER_NAME);
    // next is your application specific password
    SendEmailChoreo.addInput("Password", GMAIL_APP_PASSWORD);
    // who to send the email to
    SendEmailChoreo.addInput("ToAddress", TO_EMAIL_ADDRESS);
    // then a subject line
    SendEmailChoreo.addInput("Subject", "ALERT: Greenhouse Temperature");

     // next comes the message body, the main content of the email   
    SendEmailChoreo.addInput("MessageBody", "Hey! The greenhouse is too cold!");

    // tell the Choreo to run and wait for the results. The 
    // return code (returnCode) will tell us whether the Temboo client 
    // was able to send our request to the Temboo servers
    unsigned int returnCode = SendEmailChoreo.run();

    // a return code of zero (0) means everything worked
    if (returnCode == 0) {
        Serial.println("Success! Email sent!");
    } else {
      // a non-zero return code means there was an error
      // read and print the error message
      while (SendEmailChoreo.available()) {
        char c = SendEmailChoreo.read();
        Serial.print(c);
      }
    } 
    SendEmailChoreo.close();
    
    // set the flag showing we've tried
    attempted = true;
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
