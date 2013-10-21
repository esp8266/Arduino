/*
  ToxicFacilitiesSearch

  Demonstrates making a request to the Envirofacts API using Temboo from an Arduino Yun.
  This example retrieves the names and addresses of EPA-regulated facilities in the
  Toxins Release Inventory (TRI) database within a given zip code.

  Check out the latest Arduino & Temboo examples and support docs at http://www.temboo.com/arduino

  A Temboo account and application key are necessary to run all Temboo examples.
  If you don't already have one, you can register for a free Temboo account at
  http://www.temboo.com

  This example assumes basic familiarity with Arduino sketches, and that your Yun is connected
  to the Internet.

  Looking for another API to use with your Arduino Yun? We've got over 100 in our Library!

  This example code is in the public domain.
*/

#include <Bridge.h>
#include <Temboo.h>
#include "TembooAccount.h" // contains Temboo account information
// as described in the footer comment below

// the zip code to search for toxin-emitting facilities
String US_ZIP_CODE = "11215";

int numRuns = 1;   // execution count, so that this doesn't run forever
int maxRuns = 10;  // max number of times the Envirofacts FacilitiesSearch Choreo should be run

void setup() {
  Serial.begin(9600);

  // for debugging, wait until a serial console is connected
  delay(4000);
  while (!Serial);
  Bridge.begin();
}

void loop()
{
  // while we haven't reached the max number of runs...
  if (numRuns <= maxRuns) {

    // print status
    Serial.println("Running ToxicFacilitiesSearch - Run #" + String(numRuns++) + "...");

    // we need a Process object to send a Choreo request to Temboo
    TembooChoreo FacilitiesSearchByZipChoreo;

    // invoke the Temboo client
    // NOTE that the client must be reinvoked and repopulated with
    // appropriate arguments each time its run() method is called.
    FacilitiesSearchByZipChoreo.begin();

    // set Temboo account credentials
    FacilitiesSearchByZipChoreo.setAccountName(TEMBOO_ACCOUNT);
    FacilitiesSearchByZipChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    FacilitiesSearchByZipChoreo.setAppKey(TEMBOO_APP_KEY);

    // identify the Temboo Library choreo to run (EnviroFacts > Toxins > FacilitiesSearchByZip)
    FacilitiesSearchByZipChoreo.setChoreo("/Library/EnviroFacts/Toxins/FacilitiesSearchByZip");

    // set choreo inputs; in this case, the US zip code for which to retrieve toxin release data
    // the Temboo client provides standardized calls to 100+ cloud APIs
    FacilitiesSearchByZipChoreo.addInput("Zip", US_ZIP_CODE);

    // specify two output filters, to help simplify the Envirofacts API results.
    // see the tutorials on using Temboo SDK output filters at http://www.temboo.com/arduino
    FacilitiesSearchByZipChoreo.addOutputFilter("fac", "FACILITY_NAME", "Response");

    FacilitiesSearchByZipChoreo.addOutputFilter("addr", "STREET_ADDRESS", "Response");

    // run the choreo
    unsigned int returnCode = FacilitiesSearchByZipChoreo.run();
    if (returnCode == 0) {
      String facilities;
      String addresses;

      // when the choreo results are available, process them.
      // the output filters we specified will return comma delimited
      // lists containing the name and street address of the facilities
      // located in the specified zip code.
      while (FacilitiesSearchByZipChoreo.available()) {
        String name = FacilitiesSearchByZipChoreo.readStringUntil('\x1F');
        name.trim();

        String data = FacilitiesSearchByZipChoreo.readStringUntil('\x1E');
        data.trim();

        if (name == "fac") {
          facilities = data;
        } else if (name == "addr") {
          addresses = data;
        }
      }
      FacilitiesSearchByZipChoreo.close();

      // parse the comma delimited lists of facilities to join the
      // name with the address and print it to the serial monitor
      if (facilities.length() > 0) {
        int i = -1;
        int facilityStart = 0;
        int addressStart = 0;
        String facility;
        String address;
        do {
          i = facilities.indexOf(',', facilityStart);
          if (i >= 0) {
            facility = facilities.substring(facilityStart, i);
            facilityStart = i + 1;
          }

          i = addresses.indexOf(',', addressStart);
          if (i >= 0) {
            address = addresses.substring(addressStart, i);
            addressStart = i + 1;
          }

          if (i >= 0) {
            printResult(facility, address);
          }

        } while (i >= 0);
        facility = facilities.substring(facilityStart);
        address = addresses.substring(addressStart);
        printResult(facility, address);
      } else {
        Serial.println("No facilities found in zip code " + US_ZIP_CODE);
      }
    } else {
      while (FacilitiesSearchByZipChoreo.available()) {
        char c = FacilitiesSearchByZipChoreo.read();
        Serial.print(c);
      }
    }
  }
  Serial.println("Waiting...");
  Serial.println("");
  delay(30000); // wait 30 seconds between calls
}

// a simple utility function, to output the facility name and address in the serial monitor.
void printResult(String facility, String address) {
  Serial.print(facility);
  Serial.print(" - ");
  Serial.println(address);
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
