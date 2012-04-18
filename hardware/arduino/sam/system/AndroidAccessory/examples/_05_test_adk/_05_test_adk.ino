#include <Max3421e.h>
#include <Usb.h>
#include <AndroidAccessory.h>

AndroidAccessory acc("Google, Inc.",
		     "DemoKit",
		     "DemoKit Arduino Board",
		     "1.0",
		     "http://www.android.com",
		     "0000000012345678");
void setup();
void loop();

void setup()
{
	Serial.begin(115200);
	Serial.print("\r\nStart");
	acc.powerOn();
}

void loop()
{
  byte msg[3];
  
	if (acc.isConnected()) {
                Serial.print("Accessory connected. ");
		int len = acc.read(msg, sizeof(msg), 1);
                Serial.print("Message length: ");
                Serial.println(len, DEC);
        }

	delay(100);
}

