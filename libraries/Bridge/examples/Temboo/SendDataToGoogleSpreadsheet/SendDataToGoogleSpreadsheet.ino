/*
  SendDataToGoogleSpreadsheet

  Demonstrates appending a row of data to a Google spreadsheet using Temboo from an Arduino Yún.

  Check out the latest Arduino & Temboo examples and tutorials at http://www.temboo.com/arduino

  A Temboo account and application key are necessary to run all Temboo examples. 
  If you don't already have one, you can register for a free Temboo account at 
  http://www.temboo.com

 Instructions:
 
  1. Create a Temboo account: http://www.temboo.com
  
  2. Retrieve your Temboo application details: http://www.temboo.com/account/applications
  
  3. Replace the values in the TembooAccount.h tab with your Temboo application details
  
  4. You'll also need a Google Spreadsheet that includes a title in the first row 
     of each column that data will be written to. This example assumes there are two columns. 
     The first column is the time (in milliseconds) that the row was appended, and the second 
     column is a sensor value. In other words, your spreadsheet should look like:
  
      Time  |  Sensor Value  |     
      ------+-----------------
            |                |  
 
  5. Google Spreadsheets requires you to authenticate via OAuth. Follow the steps
     in the link below to find your ClientID, ClientSecret, and RefreshToken, and then 
     use those values to overwrite the placeholders in the code below. 
     
     https://temboo.com/library/Library/Google/OAuth/
     
     For the scope field, you need to use: https://spreadsheets.google.com/feeds/
     
     Here's a video outlines how Temboo helps with the OAuth process: 
     
     https://www.temboo.com/videos#oauthchoreos
     
     And here's a more in-depth version of this example on our website: 
     
     https://temboo.com/arduino/yun/update-google-spreadsheet
 
  6. Next, upload the sketch to your Arduino Yún and open the serial monitor
 
  Note: you can test this Choreo and find the latest instructions on our website:
  https://temboo.com/library/Library/Google/Spreadsheets/AppendRow/
  
  Looking for another API to use with your Arduino Yún? We've got over 100 in our Library!

  This example code is in the public domain.

*/

#include <Bridge.h>
#include <Temboo.h>
#include "TembooAccount.h" // contains Temboo account information, 
                           // as described in the footer comment below


/*** SUBSTITUTE YOUR VALUES BELOW: ***/

// Note that for additional security and reusability, you could
// use #define statements to specify these values in a .h file.

// the clientID found in Google's Developer Console under APIs & Auth > Credentials
const String CLIENT_ID = "your-client-id";

// the clientSecret found in Google's Developer Console under APIs & Auth > Credentials
const String CLIENT_SECRET = "your-client-secret";

// returned after running FinalizeOAuth
const String REFRESH_TOKEN = "your-oauth-refresh-token";

// the title of the spreadsheet you want to send data to
// (Note that this must actually be the title of a Google spreadsheet
// that exists in your Google Drive/Docs account, and is configured
// as described above.)
const String SPREADSHEET_TITLE = "your-spreadsheet-title";

const unsigned long RUN_INTERVAL_MILLIS = 60000; // how often to run the Choreo (in milliseconds)

// the last time we ran the Choreo 
// (initialized to 60 seconds ago so the
// Choreo is run immediately when we start up)
unsigned long lastRun = (unsigned long)-60000;

void setup() {
  
  // for debugging, wait until a serial console is connected
  Serial.begin(9600);
  delay(4000);
  while(!Serial);

  Serial.print("Initializing the bridge...");
  Bridge.begin();
  Serial.println("Done");
}

void loop()
{
  // get the number of milliseconds this sketch has been running
  unsigned long now = millis();

  // run again if it's been 60 seconds since we last ran
  if (now - lastRun >= RUN_INTERVAL_MILLIS) {

    // remember 'now' as the last time we ran the choreo
    lastRun = now;
    
    Serial.println("Getting sensor value...");

    // get the value we want to append to our spreadsheet
    unsigned long sensorValue = getSensorValue();

    Serial.println("Appending value to spreadsheet...");

    // we need a Process object to send a Choreo request to Temboo
    TembooChoreo AppendRowChoreo;

    // invoke the Temboo client
    // NOTE that the client must be reinvoked and repopulated with
    // appropriate arguments each time its run() method is called.
    AppendRowChoreo.begin();
    
    // set Temboo account credentials
    AppendRowChoreo.setAccountName(TEMBOO_ACCOUNT);
    AppendRowChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    AppendRowChoreo.setAppKey(TEMBOO_APP_KEY);
    
    // identify the Temboo Library choreo to run (Google > Spreadsheets > AppendRow)
    AppendRowChoreo.setChoreo("/Library/Google/Spreadsheets/AppendRow");
    
    // set the required Choreo inputs
    // see https://www.temboo.com/library/Library/Google/Spreadsheets/AppendRow/ 
    // for complete details about the inputs for this Choreo
    
    // your Google application client ID
    AppendRowChoreo.addInput("ClientID", CLIENT_ID);
    // your Google application client secert
    AppendRowChoreo.addInput("ClientSecret", CLIENT_SECRET);
    // your Google OAuth refresh token
    AppendRowChoreo.addInput("RefreshToken", REFRESH_TOKEN);

    // the title of the spreadsheet you want to append to
    // NOTE: substitute your own value, retaining the "SpreadsheetTitle:" prefix.
    AppendRowChoreo.addInput("SpreadsheetTitle", SPREADSHEET_TITLE);

    // convert the time and sensor values to a comma separated string
    String rowData(now);
    rowData += ",";
    rowData += sensorValue;

    // add the RowData input item
    AppendRowChoreo.addInput("RowData", rowData);

    // run the Choreo and wait for the results
    // The return code (returnCode) will indicate success or failure 
    unsigned int returnCode = AppendRowChoreo.run();

    // return code of zero (0) means success
    if (returnCode == 0) {
      Serial.println("Success! Appended " + rowData);
      Serial.println("");
    } else {
      // return code of anything other than zero means failure  
      // read and display any error messages
      while (AppendRowChoreo.available()) {
        char c = AppendRowChoreo.read();
        Serial.print(c);
      }
    }

    AppendRowChoreo.close();
  }
}

// this function simulates reading the value of a sensor 
unsigned long getSensorValue() {
  return analogRead(A0);
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
