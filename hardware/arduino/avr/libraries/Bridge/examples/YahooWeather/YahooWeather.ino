/*
  Yahoo Weather Forecast parser
 
 http://developer.yahoo.com/weather/
 This sketch demonstrate how to use the Linux command line tools
 to parse a simple XML file on the Arduino Yún.
 
 First thing download the XML file from the Yahoo Weather service 
 than use "grep" and "cut" to extract the data you want.
 
 To find  the location ID of your location, browse or search for your 
 city from the Weather home page. The location ID is in the URL for 
 the forecast page for that city.
 
 created 21 Jun 2013
 by Federico Vanzati
 
 */

#include <Bridge.h>

String locationID = "725003";  // Turin, Italy

// table with keywords to search in the XML file
// the third column is the tag to the field
String forecast[10][3] = {
  "location",   "2",  "city",
  "condition",  "6",  "temperature",
  "condition",  "2",  "condition",
  "astronomy",  "2",  "sunrise",
  "astronomy",  "4",  "sunset",
  "atmosphere", "2",  "humidity",
  "atmosphere", "6",  "pressure",
  "wind",       "6",  "wind speed",
  "wind",       "4",  "wind direction",
  "wind",       "2",  "chill temperature"  
};


void setup() {
  Bridge.begin();
  Serial.begin(9600);
  while(!Serial);
  
  Serial.println("Weather Forecast for your location: \n");
}

void loop() {

  for(int i=0; i<10; i++) {
    
    // Compose the request
    
    // curl is a program that connect to an URL an download the content
    // is used to get the weather forecast from yahoo in XML format
    String command = "curl -s ";  // -s is the silent option
    command += "http://weather.yahooapis.com/forecastrss";  // yahoo weather RSS service
    command += "?w=";  // query for the location
    command += locationID;
    //command += "\\&u=c";  // ask for celsius degrees
    
    // add a new process 
    // grep is used to extract a single line of content containig a search keyword form the XML
    command += " | ";  // pipe a new process
    command += "grep "; 
    command += forecast[i][0];  // word to search in the XML file

    // add a new process 
    // cut is a program that split a text in different fields 
    // when encouter the passed  character delimiter
    command += " | ";  // pipe a new process
    command += "cut ";
    command += "-d \\\" ";  // -d parameter split the string every " char
    command += "-f ";  // -f parameter is to return the 6th splitted element 
    command += forecast[i][1]; // the field are already manually calculated and inserted in the forecast table


    Serial.print(forecast[i][2]);
    Serial.print("= ");
    
    // run the command
    Process wf;
    wf.runShellCommand(command);
    
    while(wf.available()>0)
    {
      Serial.print( (char)wf.read() );
    }
  }
  
  //do nothing forevermore
  while(1);  
}

