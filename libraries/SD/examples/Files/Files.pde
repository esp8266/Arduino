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
  
  if (SD.exists("example.txt")) Serial.println("example.txt exists.");
  else Serial.println("example.txt doesn't exist.");
  
  Serial.println("Creating example.txt...");
  f = SD.open("example.txt", true);
  f.close();
  
  if (SD.exists("example.txt")) Serial.println("example.txt exists.");
  else Serial.println("example.txt doesn't exist.");  
  
  Serial.println("Removing example.txt...");
  SD.remove("example.txt");

  if (SD.exists("example.txt")) Serial.println("example.txt exists.");
  else Serial.println("example.txt doesn't exist.");  
}

void loop()
{
}


