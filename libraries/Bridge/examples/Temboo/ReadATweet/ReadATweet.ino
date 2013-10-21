/*
  ReadATweet

  Demonstrates retrieving the most recent Tweet from a user's home timeline
  using Temboo from an Arduino Yun.

  Check out the latest Arduino & Temboo examples and support docs at http://www.temboo.com/arduino

  A Temboo account and application key are necessary to run all Temboo examples.
  If you don't already have one, you can register for a free Temboo account at
  http://www.temboo.com

  In order to run this sketch, you'll need to register an application using
  the Twitter dev console at https://dev.twitter.com. After creating the
  app, you'll find OAuth credentials for that application under the "OAuth Tool" tab.
  Substitute these values for the placeholders below.

  This example assumes basic familiarity with Arduino sketches, and that your Yun
  is connected to the Internet.

  Want to use another social API with your Arduino Yun? We've got Facebook,
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

int numRuns = 1;   // execution count, so this doesn't run forever
int maxRuns = 10;   // the max number of times the Twitter HomeTimeline Choreo should run

void setup() {
  Serial.begin(9600);

  // For debugging, wait until a serial console is connected.
  delay(4000);
  while (!Serial);
  Bridge.begin();
}
void loop()
{
  // while we haven't reached the max number of runs...
  if (numRuns <= maxRuns) {
    Serial.println("Running ReadATweet - Run #" + String(numRuns++));

    TembooChoreo HomeTimelineChoreo;

    // invoke the Temboo client.
    // NOTE that the client must be reinvoked, and repopulated with
    // appropriate arguments, each time its run() method is called.
    HomeTimelineChoreo.begin();

    // set Temboo account credentials
    HomeTimelineChoreo.setAccountName(TEMBOO_ACCOUNT);
    HomeTimelineChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    HomeTimelineChoreo.setAppKey(TEMBOO_APP_KEY);

    // tell the Temboo client which Choreo to run (Twitter > Timelines > HomeTimeline)
    HomeTimelineChoreo.setChoreo("/Library/Twitter/Timelines/HomeTimeline");


    // set the required choreo inputs
    // see https://www.temboo.com/library/Library/Twitter/Timelines/HomeTimeline/
    // for complete details about the inputs for this Choreo

    HomeTimelineChoreo.addInput("Count", "1"); // the max number of Tweets to return from each request
    HomeTimelineChoreo.addInput("AccessToken", TWITTER_ACCESS_TOKEN);
    HomeTimelineChoreo.addInput("AccessTokenSecret", TWITTER_ACCESS_TOKEN_SECRET);
    HomeTimelineChoreo.addInput("ConsumerKey", TWITTER_CONSUMER_KEY);
    HomeTimelineChoreo.addInput("ConsumerSecret", TWITTER_CONSUMER_SECRET);

    // next, we'll define two output filters that let us specify the
    // elements of the response from Twitter that we want to receive.
    // see the examples at http://www.temboo.com/arduino
    // for more on using output filters

    // we want the text of the tweet
    HomeTimelineChoreo.addOutputFilter("tweet", "/[1]/text", "Response");

    // and the name of the author
    HomeTimelineChoreo.addOutputFilter("author", "/[1]/user/screen_name", "Response");


    // tell the Process to run and wait for the results. The
    // return code will tell us whether the Temboo client
    // was able to send our request to the Temboo servers
    unsigned int returnCode = HomeTimelineChoreo.run();

    // a response code of 0 means success; print the API response
    if (returnCode == 0) {

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

      while (HomeTimelineChoreo.available()) {
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
      while (HomeTimelineChoreo.available()) {
        char c = HomeTimelineChoreo.read();
        Serial.print(c);
      }
    }

    HomeTimelineChoreo.close();

  }

  Serial.println("Waiting...");
  delay(90000); // wait 90 seconds between HomeTimeline calls
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
