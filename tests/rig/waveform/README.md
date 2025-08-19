#PWM Tests

WIP

This directory contains a slave app to be uploaded and run on the ESP8266
and a series of tests which will perform measurements on the PWM performance
of the current PR.

It requires:
* ESP8266
* Sigrok-CLI (https://sigrok.org/)
* USB logic analyzer compatible with Sigrok-CLI

##Prepare for testing
* Configure the IDE for either 80 or 160MHz operation.
* Build and upload the test.ino file to the ESP8266 and verify basic connectivity (you can blink the LED, etc.).
* Connect the logic analyzer to the desired output pins
<insert wiring diagram here>
* NOTE:  You will need to rebuild and reupload the app if you want to swap between 80 and 160mhz operation.

##Running the tests
* Ensure the serial monitor is closed in the Arduino IDE, and any LogicView or Sigrok apps are closed
* Run the test using the commands:  <TBD>

##Checking results
Test results will be output to CSV files, comma delimited.

##Adding Tests
TBD.  For now, edit the test script.
