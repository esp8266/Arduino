#include <Bridge.h>
#include <SpacebrewYun.h>

/**
 *	Arduino Yun Spacebrew Library Example
 *
 *	This example code is in the public domain.
 *	
 * 	@date 		July 16, 2013
 *  @author		Julio Terra
 *   
 */

SpacebrewYun sb = SpacebrewYun("aYun", "Arduino Yun spacebrew test");

int counter = 0;
long last = 0;
int interval = 2000;

void setup() { 

	Serial.begin(57600);
	delay(4000);
	while (!Serial) { 	Serial.println("connecting"); }


	//Initialize Console and wait for port to open:
	Bridge.begin();
	Serial.println("Bridge Started"); 

	Serial.println("Configuring Spacebrew Client"); 
	sb.verbose(true);
	sb.addPublish("string test", "string");
	sb.addPublish("range test", "range");
	sb.addPublish("boolean test", "boolean");
	sb.addPublish("custom test", "crazy");
	sb.addSubscribe("string test", "string");
	sb.addSubscribe("range test", "range");
	sb.addSubscribe("boolean test", "boolean");
	sb.addSubscribe("custom test", "crazy");
	sb.onRangeMessage(handleRange);
	sb.onStringMessage(handleString);
	sb.onBooleanMessage(handleBoolean);
	sb.onCustomMessage(handleCustom);
	sb.connect("sandbox.spacebrew.cc"); 

} 


void loop() { 
	sb.monitor();
	if ( sb.connected() ) {
		if ( (millis() - last) > interval ) {
			String test_str_msg = "testing, testing, ";
			test_str_msg += counter;
			counter ++;

			sb.send("string test", test_str_msg);
			sb.send("range test", 500);
			sb.send("boolean test", true);
			sb.send("custom test", "youre loco");

			last = millis();

		}
	}
} 

void handleRange (String route, int value) {
	Serial.print("Range msg ");
	Serial.print(route);
	Serial.print(", value ");
	Serial.println(value);
}

void handleString (String route, String value) {
	Serial.print("String msg ");
	Serial.print(route);
	Serial.print(", value ");
	Serial.println(value);
}

void handleBoolean (String route, boolean value) {
	Serial.print("Boolen msg ");
	Serial.print(route);
	Serial.print(", value ");
	Serial.println(value ? "true" : "false");
}

void handleCustom (String route, String value, String type) {
	Serial.print("Custom msg ");
	Serial.print(route);
	Serial.print(" of type ");
	Serial.print(type);
	Serial.print(", value ");
	Serial.println(value);
}

