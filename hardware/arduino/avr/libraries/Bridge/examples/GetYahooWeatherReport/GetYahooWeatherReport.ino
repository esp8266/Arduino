/*
  GetYahooWeatherReport
  
  Demonstrates making a request to the Yahoo! Weather API using the Temboo Arduino Yun SDK.
  
  A Temboo account and application key are necessary to run all Temboo examples. 
  If you don't already have one,  you can register for a free Temboo account at 
  http://www.temboo.com
  
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

int numRuns = 0;   // execution count, so that this doesn't run forever
int maxRuns = 10;  // max number of times the Yahoo WeatherByAddress Choreo should be run

void setup() {
  Serial.begin(9600);
    
  // for debugging, wait until a serial console is connected
  while(!Serial);
  Bridge.begin();
}

void loop()
{
  // while we haven't reached the max number of runs...
  if (numRuns < maxRuns) {
      
    // print status
    Serial.println("Running GetWeatherByAddress - Run #" + String(numRuns++));

    // we need a Process object to send a Choreo request to Temboo
    Process GetWeatherByAddressChoreo;

    // invoke the Temboo client
    GetWeatherByAddressChoreo.begin("temboo");
        
    // set Temboo account credentials
    GetWeatherByAddressChoreo.addParameter("-a");
    GetWeatherByAddressChoreo.addParameter(TEMBOO_ACCOUNT);
    GetWeatherByAddressChoreo.addParameter("-u");
    GetWeatherByAddressChoreo.addParameter(TEMBOO_APP_KEY_NAME);
    GetWeatherByAddressChoreo.addParameter("-p");
    GetWeatherByAddressChoreo.addParameter(TEMBOO_APP_KEY);
  
   	// identify the Temboo Library choreo to run (Yahoo > Weather > GetWeatherByAddress)
    GetWeatherByAddressChoreo.addParameter("-c");
    GetWeatherByAddressChoreo.addParameter("/Library/Yahoo/Weather/GetWeatherByAddress");
        
    // set choreo inputs; in this case, the address for which to retrieve weather data
    // the Temboo client provides standardized calls to 100+ cloud APIs
    GetWeatherByAddressChoreo.addParameter("-i");
    GetWeatherByAddressChoreo.addParameter("Address:104 Franklin St., New York NY 10013");
            
    // run the choreo 
    GetWeatherByAddressChoreo.run();
        
    // when the choreo results are available, print them to the serial monitor
    while(GetWeatherByAddressChoreo.available()) {
          
      // note that in this example, we just print the raw XML response from Yahoo
      // see the examples on using Temboo SDK output filters at http://www.temboo.com/arduino
      // for information on how to filter this data
          
      Serial.print((char)GetWeatherByAddressChoreo.read());
    }
    GetWeatherByAddressChoreo.close();

  }

  Serial.println("Sleeping...");
  delay(30000); // sleep 30 seconds between GetWeatherByAddress calls
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
