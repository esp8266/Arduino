/*
  SendATweet

  Demonstrates sending a tweet via a Twitter account using Temboo from an Arduino Yún.

  Check out the latest Arduino & Temboo examples and support docs at http://www.temboo.com/arduino

  A Temboo account and application key are necessary to run all Temboo examples. 
  If you don't already have one, you can register for a free Temboo account at 
  http://www.temboo.com

  In order to run this sketch, you'll need to register an application using
  the Twitter dev console at https://dev.twitter.com. Note that since this 
  sketch creates a new tweet, your application will need to be configured with
  read+write permissions. After creating the app, you'll find OAuth credentials 
  for that application under the "OAuth Tool" tab. Substitute these values for 
  the placeholders below. 

  This example assumes basic familiarity with Arduino sketches, and that your Yún is connected
  to the Internet.

  Want to use another social API with your Arduino Yún? We've got Facebook,
  Google+, Instagram, Tumblr and more in our Library!
  
  This example code is in the public domain.
*/

#include <Bridge.h>
#include <Temboo.h>
#include "TembooAccount.h" // contains Temboo account information
                           // as described in the footer comment below


/*** SUBSTITUTE YOUR VALUES BELOW: ***/

// Note that for additional security and reusability, you could
// use #define statements to specify these values in a .h file.
const String TWITTER_ACCESS_TOKEN = "your-twitter-access-token";
const String TWITTER_ACCESS_TOKEN_SECRET = "your-twitter-access-token-secret";
const String TWITTER_CONSUMER_KEY = "your-twitter-consumer-key";
const String TWITTER_CONSUMER_SECRET = "your-twitter-consumer-secret";

int numRuns = 1;   // execution count, so this sketch doesn't run forever
int maxRuns = 3;  // the max number of times the Twitter Update Choreo should run

void setup() {
  Serial.begin(9600);

  // for debugging, wait until a serial console is connected
  delay(4000);
  while(!Serial);

  Bridge.begin();
}

void loop()
{
  // only try to send the tweet if we haven't already sent it successfully
  if (numRuns <= maxRuns) {

    Serial.println("Running SendATweet - Run #" + String(numRuns++) + "...");
  
    // define the text of the tweet we want to send
    String tweetText("My Arduino Yun has been running for " + String(millis()) + " milliseconds.");

    
    TembooChoreo StatusesUpdateChoreo;

    // invoke the Temboo client
    // NOTE that the client must be reinvoked, and repopulated with
    // appropriate arguments, each time its run() method is called.
    StatusesUpdateChoreo.begin();
    
    // set Temboo account credentials
    StatusesUpdateChoreo.setAccountName(TEMBOO_ACCOUNT);
    StatusesUpdateChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    StatusesUpdateChoreo.setAppKey(TEMBOO_APP_KEY);

    // identify the Temboo Library choreo to run (Twitter > Tweets > StatusesUpdate)
    StatusesUpdateChoreo.setChoreo("/Library/Twitter/Tweets/StatusesUpdate");

    // set the required choreo inputs
    // see https://www.temboo.com/library/Library/Twitter/Tweets/StatusesUpdate/ 
    // for complete details about the inputs for this Choreo
 
    // add the Twitter account information
    StatusesUpdateChoreo.addInput("AccessToken", TWITTER_ACCESS_TOKEN);
    StatusesUpdateChoreo.addInput("AccessTokenSecret", TWITTER_ACCESS_TOKEN_SECRET);
    StatusesUpdateChoreo.addInput("ConsumerKey", TWITTER_CONSUMER_KEY);    
    StatusesUpdateChoreo.addInput("ConsumerSecret", TWITTER_CONSUMER_SECRET);

    // and the tweet we want to send
    StatusesUpdateChoreo.addInput("StatusUpdate", tweetText);

    // tell the Process to run and wait for the results. The 
    // return code (returnCode) will tell us whether the Temboo client 
    // was able to send our request to the Temboo servers
    unsigned int returnCode = StatusesUpdateChoreo.run();

    // a return code of zero (0) means everything worked
    if (returnCode == 0) {
        Serial.println("Success! Tweet sent!");
    } else {
      // a non-zero return code means there was an error
      // read and print the error message
      while (StatusesUpdateChoreo.available()) {
        char c = StatusesUpdateChoreo.read();
        Serial.print(c);
      }
    } 
    StatusesUpdateChoreo.close();

    // do nothing for the next 90 seconds
    Serial.println("Waiting...");
    delay(90000);
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
