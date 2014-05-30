/*
  UpdateFacebookStatus

  Demonstrates sending a Facebook status update using Temboo from an Arduino Yún.

  Check out the latest Arduino & Temboo examples and support docs at http://www.temboo.com/arduino
  
  A Temboo account and application key are necessary to run all Temboo examples. 
  If you don't already have one, you can register for a free Temboo account at 
  http://www.temboo.com

  In order to run this sketch, you'll need to register an application using
  the Facebook dev console at https://developers.facebook.com/apps -- after creating
  the app, log in to Temboo and visit https://www.temboo.com/library/Library/Facebook/Publishing/SetStatus/
  to use our OAuth Wizard (or OAuth Choreos) to obtain a Facebook access token.
  Substitute your access token for the placeholder value of FACEBOOK_ACCESS_TOKEN below.

  This example assumes basic familiarity with Arduino sketches, and that your Yún
  is connected to the Internet.
  
  Want to use another social API with your Arduino Yún? We've got Twitter, Google+,
  Instagram, Tumblr and more in our Library!

  This example code is in the public domain. 
*/

#include <Bridge.h>
#include <Temboo.h>
#include "TembooAccount.h" // contains Temboo account information, 
                           // as described in the footer comment below

/*** SUBSTITUTE YOUR VALUES BELOW: ***/

// Note that for additional security and reusability, you could
// use a #define statement to specify this value in a .h file.

// the Facebook Access Token, which can be obtained using the Temboo OAuth Wizard or Choreos
const String FACEBOOK_ACCESS_TOKEN = "xxxxxxxxxx";


int numRuns = 1;   // execution count, so this sketch doesn't run forever
int maxRuns = 10;  // the max number of times the Facebook SetStatus Choreo should run

void setup() {
  Serial.begin(9600);
  
  // For debugging, wait until a serial console is connected.
  delay(4000);
  while(!Serial);
  Bridge.begin();
}

void loop() {
  // while we haven't reached the max number of runs...
  if (numRuns <= maxRuns) {

    // print status
    Serial.println("Running UpdateFacebookStatus - Run #" + String(numRuns++) + "...");
    
    // Define the status message we want to post on Facebook; since Facebook
    // doesn't allow duplicate status messages, we'll include a changing value.
    String statusMsg = "My Arduino Yun has been running for " + String(millis()) + " milliseconds!";

    // define the Process that will be used to call the "temboo" client                
    TembooChoreo SetStatusChoreo;

    // invoke the Temboo client
    // NOTE that the client must be reinvoked and repopulated with
    // appropriate arguments each time its run() method is called.
    SetStatusChoreo.begin();
    
    // set Temboo account credentials
    SetStatusChoreo.setAccountName(TEMBOO_ACCOUNT);
    SetStatusChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    SetStatusChoreo.setAppKey(TEMBOO_APP_KEY);

    // tell the Temboo client which Choreo to run (Facebook > Publishing > SetStatus)
    SetStatusChoreo.setChoreo("/Library/Facebook/Publishing/SetStatus");

    // set the required choreo inputs
    // see  https://www.temboo.com/library/Library/Facebook/Publishing/SetStatus/
    // for complete details about the inputs for this Choreo
    
    SetStatusChoreo.addInput("AccessToken", FACEBOOK_ACCESS_TOKEN);    
    SetStatusChoreo.addInput("Message", statusMsg);


    // tell the Process to run and wait for the results. The 
    // return code (returnCode) will tell us whether the Temboo client 
    // was able to send our request to the Temboo servers
    unsigned int returnCode = SetStatusChoreo.run();
    
    // print the response code and API response.
    Serial.println("Response code: " + String(returnCode));

    // note that in this case, we're just printing the raw response from Facebook.
    // see the examples on using Temboo SDK output filters at http://www.temboo.com/arduino
    // for information on how to filter this data    
    while(SetStatusChoreo.available()) {
      char c = SetStatusChoreo.read();
      Serial.print(c);
    }

    SetStatusChoreo.close();
  }

  Serial.println("Waiting...");
  Serial.println("");

  delay(30000); // wait 30 seconds between SetStatus calls  
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
