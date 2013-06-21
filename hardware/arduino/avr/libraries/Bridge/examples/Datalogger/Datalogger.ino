/*
  SD card datalogger
 
 This example shows how to log data from three analog sensors 
 to an SD card mounted on the Linux using the Bridge library.
 	
 The circuit:
 * analog sensors on analog ins 0, 1, and 2
 * SD card attached to SD card slot of the Arduino Yun
 
 You can remove the SD card while the Linux and the 
 sketch are running but becareful to don't remove it while
 the system is writing on it.
 
 created  24 Nov 2010
 modified 9 Apr 2012
 by Tom Igoe
 adapted to the Yun Bridge library 20 Jun 2013
 by Federico Vanzati
 
 This example code is in the public domain.
 	 
 */

#include <FileIO.h>
#include <Console.h>

void setup() {
  // Initialize the Bridge and the Console
  Bridge.begin();
  Console.begin();
  FileSystem.begin();

  while(!Console){
    ;  // wait for Console port to connect.
  }
}


void loop () {
  // make a string that start with a timestamp for assembling the data to log:
  String dataString = "";
  addTimeStamp(dataString);
  dataString += " = ";

  // read three sensors and append to the string:
  for (int analogPin = 0; analogPin < 3; analogPin++) {
    int sensor = analogRead(analogPin);
    dataString += String(sensor);
    if (analogPin < 2) {
      dataString += ","; 
    }
  }

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  // The FileSystem card is mounted at the following "/mnt/FileSystema1"
  File dataFile = FileSystem.open("/mnt/sda1/datalog.txt", FILE_APPEND);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Console.println(dataString);
  }  
  // if the file isn't open, pop up an error:
  else {
    Console.println("error opening datalog.txt");
  } 
  
  delay(15000);

}

// This function append a time stamp to the string passed as argument
void addTimeStamp(String &string) {
  Process time;
  time.begin("date"); 
  time.addParameter("+%D-%T");
  time.run();

  while(time.available()>0) {
    char c = time.read();
    if(c != '\n')
      string += c;
  }
}
