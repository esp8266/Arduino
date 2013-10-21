/*
  UploadToDropbox

  Demonstrates uploading a file to a Dropbox account using Temboo from an Arduino Yun.

  Check out the latest Arduino & Temboo examples and support docs at http://www.temboo.com/arduino

  A Temboo account and application key are necessary to run all Temboo examples.
  If you don't already have one, you can register for a free Temboo account at
  http://www.temboo.com

  You'll also need a valid Dropbox app and accompanying OAuth credentials.
  To create a Dropbox app, visit https://www.dropbox.com/developers/apps and
  do the following:

  1. Create a "Dropbox API app"
  2. Select "Files and datastores"
  3. Select "Yes - my app only needs access to the files it creates."

  Once you've created your app, follow the instructions at
  https://www.temboo.com/library/Library/Dropbox/OAuth/ to run the Initialize and Finalize
  OAuth Choreos. These Choreos complete the OAuth handshake and retrieve your Dropbox OAuth access tokens.

  This example assumes basic familiarity with Arduino sketches, and that your Yun is connected
  to the Internet.

  Looking for another API to use with your Arduino Yun? We've got over 100 in our Library!

  This example code is in the public domain.
*/

#include <Bridge.h>
#include <Temboo.h>
#include "TembooAccount.h" // contains Temboo account information
// as described in the footer comment below


/*** SUBSTITUTE YOUR VALUES BELOW: ***/

// Note that for additional security and reusability, you could
// use #define statements to specify these values in a .h file.

// your Dropbox app key, available on the Dropbox developer console after registering an app
const String DROPBOX_APP_KEY = "xxxxxxxxxx";

// your Dropbox app secret, available on the Dropbox developer console after registering an app
const String DROPBOX_APP_SECRET = "xxxxxxxxxx";

// your Dropbox access token, which is returned by the FinalizeOAuth Choreo
const String DROPBOX_ACCESS_TOKEN = "xxxxxxxxxx";

// your Dropbox access token secret, which is returned by the FinalizeOAuth Choreo
const String DROPBOX_ACCESS_TOKEN_SECRET = "xxxxxxxxxx";


boolean success = false; // a flag to indicate whether we've uploaded the file yet

void setup() {
  Serial.begin(9600);

  // For debugging, wait until a serial console is connected.
  delay(4000);
  while (!Serial);
  Bridge.begin();
}

void loop()
{
  // only try to upload the file if we haven't already done so
  if (!success) {

    Serial.println("Base64 encoding data to upload...");

    // base64 encode the data to upload
    String base64EncodedData = base64Encode("Hello, Arduino!");


    Serial.println("Uploading data to Dropbox...");

    // we need a Process object to send a Choreo request to Temboo
    TembooChoreo UploadFileChoreo;

    // invoke the Temboo client
    // NOTE that the client must be reinvoked and repopulated with
    // appropriate arguments each time its run() method is called.
    UploadFileChoreo.begin();

    // set Temboo account credentials
    UploadFileChoreo.setAccountName(TEMBOO_ACCOUNT);
    UploadFileChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    UploadFileChoreo.setAppKey(TEMBOO_APP_KEY);

    // identify the Temboo Library choreo to run (Dropbox > FilesAndMetadata > UploadFile)
    UploadFileChoreo.setChoreo("/Library/Dropbox/FilesAndMetadata/UploadFile");

    // set the required choreo inputs
    // see https://www.temboo.com/library/Library/Dropbox/FilesAndMetadata/UploadFile/
    // for complete details about the inputs for this Choreo

    // first specify the name of the file to create/update on Dropbox
    UploadFileChoreo.addInput("FileName", "ArduinoTest.txt");

    // next, the root folder on Dropbox relative to which the file path is specified.
    // to work with the Dropbox app you created earlier, this should be left as "sandbox"
    // if your Dropbox app has full access to your files, specify "dropbox"
    UploadFileChoreo.addInput("Root", "sandbox");

    // next, the Base64 encoded file data to upload
    UploadFileChoreo.addInput("FileContents", base64EncodedData);

    // finally, the Dropbox OAuth credentials defined above
    UploadFileChoreo.addInput("AppSecret", DROPBOX_APP_SECRET);
    UploadFileChoreo.addInput("AccessToken", DROPBOX_ACCESS_TOKEN);
    UploadFileChoreo.addInput("AccessTokenSecret", DROPBOX_ACCESS_TOKEN_SECRET);
    UploadFileChoreo.addInput("AppKey", DROPBOX_APP_KEY);

    // tell the Process to run and wait for the results. The
    // return code (returnCode) will tell us whether the Temboo client
    // was able to send our request to the Temboo servers
    unsigned int returnCode = UploadFileChoreo.run();

    // a return code of zero (0) means everything worked
    if (returnCode == 0) {
      Serial.println("Success! File uploaded!");
      success = true;
    } else {
      // a non-zero return code means there was an error
      Serial.println("Uh-oh! Something went wrong!");
    }

    // print out the full response to the serial monitor in all
    // cases, just for debugging
    while (UploadFileChoreo.available()) {
      char c = UploadFileChoreo.read();
      Serial.print(c);
    }
    UploadFileChoreo.close();

    Serial.println("Waiting...");
  }

  delay(30000); // wait 30 seconds between upload attempts
}


/*
  A utility function to Base64 encode the specified string
  by calling a Temboo Utilities Choreo.
*/
String base64Encode(String toEncode) {

  // we need a Process object to send a Choreo request to Temboo
  TembooChoreo Base64EncodeChoreo;

  // invoke the Temboo client
  Base64EncodeChoreo.begin();

  // set Temboo account credentials
  Base64EncodeChoreo.setAccountName(TEMBOO_ACCOUNT);
  Base64EncodeChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
  Base64EncodeChoreo.setAppKey(TEMBOO_APP_KEY);

  // identify the Temboo Library choreo to run (Utilities > Encoding > Base64Encode)
  Base64EncodeChoreo.setChoreo("/Library/Utilities/Encoding/Base64Encode");

  // set choreo inputs
  Base64EncodeChoreo.addInput("Text", toEncode);

  // run the choreo
  Base64EncodeChoreo.run();

  // read in the choreo results, and return the "Base64EncodedText" output value.
  // see http://www.temboo.com/arduino for more details on using choreo outputs.
  while (Base64EncodeChoreo.available()) {
    // read the name of the output item
    String name = Base64EncodeChoreo.readStringUntil('\x1F');
    name.trim();

    // read the value of the output item
    String data = Base64EncodeChoreo.readStringUntil('\x1E');
    data.trim();

    if (name == "Base64EncodedText") {
      return data;
    }
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
