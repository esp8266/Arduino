/*

 ADK Terminal Test

 This demonstrates USB Host connectivity between an 
 Android phone and an Arduino Due.

 The ADK for the Arduino Due is a work in progress
 For additional information on the Arduino ADK visit 
 http://labs.arduino.cc/ADK/Index

 created 27 June 2012
 by Cristian Maglie

*/

#include "variant.h"
#include <stdio.h>
#include <adk.h>

// Accessory descriptor. It's how Arduino identifies itself to Android.
char applicationName[] = "Arduino_Terminal"; // the app on your phone
char accessoryName[] = "Arduino Due"; // your Arduino board
char companyName[] = "Arduino SA";

// Make up anything you want for these
char versionNumber[] = "1.0";
char serialNumber[] = "1";
char url[] = "http://labs.arduino.cc/uploads/ADK/ArduinoTerminal/ThibaultTerminal_ICS_0001.apk";

USBHost Usb;
ADK adk(&Usb, companyName, applicationName, accessoryName,versionNumber,url,serialNumber);

void setup()
{
	cpu_irq_enable();
	printf("\r\nADK demo start\r\n");
	delay(200);
}

#define RCVSIZE 128

void loop()
{
	uint8_t buf[RCVSIZE];
	uint32_t nbread = 0;
	char helloworld[] = "Hello World!\r\n";

	Usb.Task();

	if (adk.isReady())
	{
		/* Write hello string to ADK */
		adk.write(strlen(helloworld), (uint8_t *)helloworld);

		delay(1000);

		/* Read data from ADK and print to UART */
		adk.read(&nbread, RCVSIZE, buf);
		if (nbread > 0)
		{
			printf("RCV: ");
			for (uint32_t i = 0; i < nbread; ++i)
			{
				printf("%c", (char)buf[i]);
			}
			printf("\r\n");
		}
	}
}
