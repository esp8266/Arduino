/*
 * ADK usb analogWrite
 *
 * TADA!
 *
 * (c) 2012 D. Cuartielles & A. Goransson
 * http://arduino.cc, http://1scale1.com
 *
 */

#include <Max3421e.h>
#include <Usb.h>
#include <AndroidAccessory.h>

// accessory descriptor. It's how Arduino identifies itself to Android
char applicationName[] = "Mega_ADK"; // the app on your phone
char accessoryName[] = "Mega_ADK"; // your Arduino board
char companyName[] = "Arduino SA";

// make up anything you want for these
char versionNumber[] = "1.0";
char serialNumber[] = "1";
char url[] = "http://labs.arduino.cc/adk/ADK_count"; // the URL of your app online

// led variables
int ledPin = 10;

// counters
long timer = millis();

// initialize the accessory:
AndroidAccessory usb(companyName, applicationName,
accessoryName,versionNumber,url,serialNumber);

void setup() {
  Serial.begin( 9600 );
  // start the connection to the device over the USB host:
  usb.powerOn();

  pinMode(ledPin, OUTPUT);   
}

void loop() {
  /* Print to usb */
  if(millis()-timer>100) { // sending 10 times per second
    if (usb.isConnected()) { // isConnected makes sure the USB connection is ope
      int val = usb.read();
      Serial.println( val );
      analogWrite( ledPin, val );
    }
    timer = millis();
  }
}








