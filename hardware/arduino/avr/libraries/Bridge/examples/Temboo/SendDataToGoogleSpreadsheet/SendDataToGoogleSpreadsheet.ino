/*
  SendDataToGoogleSpreadsheet

  Demonstrates appending a row of data to a Google spreadsheet from the Arduino Yun 
  using the Temboo Arduino Yun SDK.  

  Check out the latest Arduino & Temboo examples and support docs at http://www.temboo.com/arduino

  A Temboo account and application key are necessary to run all Temboo examples. 
  If you don't already have one, you can register for a free Temboo account at 
  http://www.temboo.com

  Since this sketch uses a Google spreadsheet, you'll also need a 
  Google account: substitute the placeholders below for your Google account values.

  This example assumes basic familiarity with Arduino sketches, and that your 
  Yun is connected to the Internet.

  The columns in your spreadsheet must have labels for the Choreo to
  work properly. It doesn't matter what the column labels actually are,
  but there must be text in the first row of each column. This example
  assumes there are two columns. The first column is the time (in milliseconds)
  that the row was appended, and the second column is a sensor value
  (simulated in this example via a random number). In other words, your spreadsheet
  should look like:
  
  Time  |  Sensor Value  |     
  ------+-----------------
        |                |
  
  NOTE that the first time you run this sketch, you may receive a warning from
  Google, prompting you to authorize access from a 3rd party system.
  
  Looking for another API? We've got over 100 in our Library!

  This example code is in the public domain.

*/

#include <Bridge.h>
#include <Console.h>
#include <FileIO.h>
#include <HttpClient.h>
#include <Process.h>
#include "TembooAccount.h" // contains Temboo account information, 
                           // as described in the footer comment below


/*** SUBSTITUTE YOUR VALUES BELOW: ***/

const String GOOGLE_USERNAME = "your-google-username";
const String GOOGLE_PASSWORD = "your-google-password";

// the title of the spreadsheet you want to send data to
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
    Process AppendRowChoreo;

    // invoke the Temboo client
    AppendRowChoreo.begin("temboo");
    
    // set Temboo account credentials
    AppendRowChoreo.addParameter("-a");
    AppendRowChoreo.addParameter(TEMBOO_ACCOUNT);
    AppendRowChoreo.addParameter("-u");
    AppendRowChoreo.addParameter(TEMBOO_APP_KEY_NAME);
    AppendRowChoreo.addParameter("-p");
    AppendRowChoreo.addParameter(TEMBOO_APP_KEY);
    
    // identify the Temboo Library choreo to run (Google > Spreadsheets > AppendRow)
    AppendRowChoreo.addParameter("-c");
    AppendRowChoreo.addParameter("/Library/Google/Spreadsheets/AppendRow");
    
    // set the required Choreo inputs
    // see https://www.temboo.com/library/Library/Google/Spreadsheets/AppendRow/ 
    // for complete details about the inputs for this Choreo
    
    // your Google username (usually your email address)
    AppendRowChoreo.addParameter("-i");
    AppendRowChoreo.addParameter("Username:" + GOOGLE_USERNAME);

    // your Google account password
    AppendRowChoreo.addParameter("-i");
    AppendRowChoreo.addParameter("Password:" + GOOGLE_PASSWORD);

    // the title of the spreadsheet you want to append to
    // NOTE: substitute your own value, retaining the "SpreadsheetTitle:" prefix.
    AppendRowChoreo.addParameter("-i");
    AppendRowChoreo.addParameter("SpreadsheetTitle:" + SPREADSHEET_TITLE);

    // convert the time and sensor values to a comma separated string
    String rowData(now);
    rowData += ",";
    rowData += sensorValue;

    // add the RowData input item
    AppendRowChoreo.addParameter("-i");
    AppendRowChoreo.addParameter("RowData:" + rowData);

    // run the Choreo and wait for the results
    // The return code (rc) will indicate success or failure 
    unsigned int rc = AppendRowChoreo.run();

    // return code of zero (0) means success
    if (rc == 0) {
      Serial.println("Success! Appended " + rowData);
      Serial.println("");
    } else {
      // return code of anything other than zero means failure  
      // read and display any error messages
      while (AppendRowChoreo.available()) {
        Serial.print((char)AppendRowChoreo.read());
      }
    }

    AppendRowChoreo.close();
  }
}

// this function simulates reading the value of a sensor 
// in this example, we're generating a random number
unsigned long getSensorValue() {
  return (unsigned long)random(0, 256);
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

