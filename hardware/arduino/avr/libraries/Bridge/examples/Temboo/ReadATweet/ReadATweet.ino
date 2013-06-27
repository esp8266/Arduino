/*
  ReadATweet

  Demonstrates retrieving the most recent Tweet from a user's home timeline 
  using the Temboo Arduino Yun SDK.

  A Temboo account and application key are necessary to run all Temboo examples. 
  If you don't already have one, you can register for a free Temboo account at 
  http://www.temboo.com

  In order to run this sketch, you'll need to register an application using
  the Twitter dev console at https://dev.twitter.com. After creating the 
  app, you'll find OAuth credentials for that application under the "OAuth Tool" tab. 
  Substitute these values for the placeholders below. 

  This example assumes basic familiarity with Arduino sketches, and that your Yun 
  is connected to the Internet. For more tutorials on using the Temboo Library and 
  Temboo Arduino Yun SDK, visit http://www.temboo.com/arduino
  
  This example code is in the public domain.
*/

#include <Bridge.h>
#include <Console.h>
#include <FileIO.h>
#include <HttpClient.h>
#include <Process.h>
#include "TembooAccount.h" // contains Temboo account information, as described below

int numRuns = 0;   // execution count, so this sketch doesn't run forever
int maxRuns = 10;  // the max number of times the Twitter HomeTimeline Choreo should run

void setup() {
  Serial.begin(9600);
  
  // for debugging, wait until a serial console is connected
  delay(4000);
  while(!Serial);
  Bridge.begin();
}

void loop()
{
  // while we haven't reached the max number of runs...
  if (numRuns < maxRuns) {

    // print status
    Serial.println("Running HomeTimeline - Run #" + String(numRuns++));

    // define the Process that will be used to call the "temboo" client            
    Process HomeTimelineChoreo;
    
    // invoke the Temboo client
    HomeTimelineChoreo.begin("temboo");
    
    // set Temboo account credentials
    HomeTimelineChoreo.addParameter("-a");
    HomeTimelineChoreo.addParameter(TEMBOO_ACCOUNT);
    HomeTimelineChoreo.addParameter("-u");
    HomeTimelineChoreo.addParameter(TEMBOO_APP_KEY_NAME);
    HomeTimelineChoreo.addParameter("-p");
    HomeTimelineChoreo.addParameter(TEMBOO_APP_KEY);
    
    // tell the Temboo client which Choreo to run (Twitter > Timelines > HomeTimeline)
    HomeTimelineChoreo.addParameter("-c");
    HomeTimelineChoreo.addParameter("/Library/Twitter/Timelines/HomeTimeline");

    // set the required choreo inputs
    // see https://www.temboo.com/library/Library/Twitter/Timelines/HomeTimeline/
    // for complete details about the inputs for this Choreo

    HomeTimelineChoreo.addParameter("-i");
    HomeTimelineChoreo.addParameter("Count:1");  // the max number of Tweets to return from each request
    
    // IMPORTANT! You'll need to substitute your own Access Token, Access Token Secret, 
    // Consumer Secret, and Consumer Key from the Twitter dev console into the method calls
    // below. (Make sure not to delete the "AccessToken:" etc prefixes!)
    HomeTimelineChoreo.addParameter("-i");
    HomeTimelineChoreo.addParameter("AccessToken:your-twitter-access-token");
    HomeTimelineChoreo.addParameter("-i");
    HomeTimelineChoreo.addParameter("AccessTokenSecret:your-twitter-access-token-secret");
    HomeTimelineChoreo.addParameter("-i");
    HomeTimelineChoreo.addParameter("ConsumerSecret:your-twitter-consumer-secret");
    HomeTimelineChoreo.addParameter("-i");
    HomeTimelineChoreo.addParameter("ConsumerKey:your-twitter-consumer-key");
   
    // next, we'll define two output filters that let us specify the 
    // elements of the response from Twitter that we want to receive.
    // see the examples at http://www.temboo.com/arduino
    // for more on using output filters
   
    // we want the text of the tweet
    HomeTimelineChoreo.addParameter("-o");
    HomeTimelineChoreo.addParameter("tweet:/[1]/text:Response");
    
    // and the name of the author
    HomeTimelineChoreo.addParameter("-o");
    HomeTimelineChoreo.addParameter("author:/[1]/user/screen_name:Response");


    // tell the Process to run and wait for the results. The 
    // return code (rc) will tell us whether the Temboo client 
    // was able to send our request to the Temboo servers
    unsigned int rc = HomeTimelineChoreo.run();
    
    // a response code of 0 means success; print the API response
    if(rc == 0) {
      
      String author; // a String to hold the tweet author's name
      String tweet; // a String to hold the text of the tweet


      // choreo outputs are returned as key/value pairs, delimited with 
      // newlines and record/field terminator characters, for example:
      // Name1\n\x1F
      // Value1\n\x1E
      // Name2\n\x1F
      // Value2\n\x1E      
      
      // see the examples at http://www.temboo.com/arduino for more details
      // we can read this format into separate variables, as follows:
      
      while(HomeTimelineChoreo.available()) {
        // read the name of the output item
        String name = HomeTimelineChoreo.readStringUntil('\x1F');
        name.trim();

        // read the value of the output item
        String data = HomeTimelineChoreo.readStringUntil('\x1E');
        data.trim();

        // assign the value to the appropriate String
        if (name == "tweet") {
          tweet = data;
        } else if (name == "author") {
          author = data;
        }
      }
     
      Serial.println("@" + author + " - " + tweet);
    
    } else {
      // there was an error
      // print the raw output from the choreo
      while(HomeTimelineChoreo.available()) {
        Serial.print((char)HomeTimelineChoreo.read());
      }
    }

    HomeTimelineChoreo.close();
  }

  Serial.println("Sleeping...");
  delay(90000); // sleep 90 seconds between HomeTimeline calls
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
*/
