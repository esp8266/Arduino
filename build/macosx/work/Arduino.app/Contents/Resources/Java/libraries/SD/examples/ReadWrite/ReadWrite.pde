#include <SD.h>

File f;

void setup()
{
  Serial.begin(9600);
  Serial.print("Initializing SD card...");
  // On the Ethernet Shield, CS is pin 4. Note that even if it's not
  // used as the CS pin, the hardware SS pin (10 on most Arduino boards,
  // 53 on the Mega) must be left as an output or the SD library
  // functions will not work.
  if (!SD.begin(4)) {
    Serial.println("failed!");
    return;
  }
  Serial.println("done.");
  
  f = SD.open("test.txt", true, false);
  if (f) {
    Serial.print("Writing to test.txt...");
    f.println("testing 1, 2, 3.");
    f.close();
    Serial.println("done.");
  } else {
    Serial.println("error opening test.txt");
  }
  
  f = SD.open("test.txt");
  if (f) {
    Serial.println("test.txt:");
    while (f.available()) Serial.write(f.read());
    f.close();
  } else {
    Serial.println("error opening test.txt");
  }
}

void loop()
{
}


