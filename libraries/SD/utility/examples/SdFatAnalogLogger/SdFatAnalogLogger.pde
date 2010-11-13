// A simple data logger for the Arduino analog pins
#define LOG_INTERVAL  1000 // mills between entries
#define SENSOR_COUNT     3 // number of analog pins to log
#define ECHO_TO_SERIAL   1 // echo data to serial port
#define WAIT_TO_START    1 // Wait for serial input in setup()
#define SYNC_INTERVAL 1000 // mills between calls to sync()
uint32_t syncTime = 0;     // time of last sync()

#include <SdFat.h>
#include <SdFatUtil.h>

Sd2Card card;
SdVolume volume;
SdFile root;
SdFile file;

// store error strings in flash to save RAM
#define error(s) error_P(PSTR(s))

void error_P(const char* str) {
  PgmPrint("error: ");
  SerialPrintln_P(str);
  if (card.errorCode()) {
    PgmPrint("SD error: ");
    Serial.print(card.errorCode(), HEX);
    Serial.print(',');
    Serial.println(card.errorData(), HEX);
  }
  while(1);
}

void setup(void) {
  Serial.begin(9600);
  Serial.println();
  
#if WAIT_TO_START
  Serial.println("Type any character to start");
  while (!Serial.available());
#endif //WAIT_TO_START

  // initialize the SD card at SPI_HALF_SPEED to avoid bus errors with
  // breadboards.  use SPI_FULL_SPEED for better performance.
  if (!card.init(SPI_HALF_SPEED)) error("card.init failed");
  
  // initialize a FAT volume
  if (!volume.init(&card)) error("volume.init failed");
  
  // open root directory
  if (!root.openRoot(&volume)) error("openRoot failed");
  
  // create a new file
  char name[] = "LOGGER00.CSV";
  for (uint8_t i = 0; i < 100; i++) {
    name[6] = i/10 + '0';
    name[7] = i%10 + '0';
    if (file.open(&root, name, O_CREAT | O_EXCL | O_WRITE)) break;
  }
  if (!file.isOpen()) error ("file.create");
  Serial.print("Logging to: ");
  Serial.println(name);

  // write header
  file.writeError = 0;
  file.print("millis");
#if ECHO_TO_SERIAL 
  Serial.print("millis");
#endif //ECHO_TO_SERIAL

#if SENSOR_COUNT > 6
#error SENSOR_COUNT too large
#endif //SENSOR_COUNT

  for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
    file.print(",sens");file.print(i, DEC);    
#if ECHO_TO_SERIAL
    Serial.print(",sens");Serial.print(i, DEC);
#endif //ECHO_TO_SERIAL
  }
  file.println();  
#if ECHO_TO_SERIAL
  Serial.println();
#endif  //ECHO_TO_SERIAL

  if (file.writeError || !file.sync()) {
    error("write header failed");
  }
}

void loop(void) {
  // clear print error
  file.writeError = 0;
  delay((LOG_INTERVAL -1) - (millis() % LOG_INTERVAL));
  
  // log time
  uint32_t m = millis();
  file.print(m);  
#if ECHO_TO_SERIAL
  Serial.print(m);
#endif //ECHO_TO_SERIAL
      
  // add sensor data 
  for (uint8_t ia = 0; ia < SENSOR_COUNT; ia++) {
    uint16_t data = analogRead(ia);
    file.print(',');    
    file.print(data);
#if ECHO_TO_SERIAL
    Serial.print(',');   
    Serial.print(data);
#endif //ECHO_TO_SERIAL
  }
  file.println();  
#if ECHO_TO_SERIAL
  Serial.println();
#endif //ECHO_TO_SERIAL

  if (file.writeError) error("write data failed");
  
  //don't sync too often - requires 2048 bytes of I/O to SD card
  if ((millis() - syncTime) <  SYNC_INTERVAL) return;
  syncTime = millis();
  if (!file.sync()) error("sync failed");
}
