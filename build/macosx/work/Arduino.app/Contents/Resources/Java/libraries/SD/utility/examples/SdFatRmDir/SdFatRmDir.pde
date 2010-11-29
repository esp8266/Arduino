/*
 * This sketch will remove the files and directories
 * created by the SdFatMakeDir.pde sketch.
 *
 * Performance is erratic due to the large number
 * of flash erase operations caused by many random
 * writes to file structures.
 */
#include <SdFat.h>
#include <SdFatUtil.h>

Sd2Card card;
SdVolume volume;
SdFile root;

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
/*
 * remove all files in dir.
 */
void deleteFiles(SdFile &dir) {
  char name[13];
  SdFile file;
 
  // open and delete files
  for (uint16_t n = 0; ; n++){
     sprintf(name, "%u.TXT", n);
     
     // open start time
     uint32_t t0 = millis();
     
     // assume done if open fails
     if (!file.open(&dir, name, O_WRITE)) return;
     
     // open end time and remove start time
     uint32_t t1 = millis();
     if (!file.remove()) error("file.remove failed");
     
     // remove end time
     uint32_t t2 = millis();
     
     PgmPrint("RM ");
     Serial.print(n);
     Serial.print(' ');
     
     // open time
     Serial.print(t1 - t0);
     Serial.print(' ');
     
     // remove time
     Serial.println(t2 - t1);
  }
}

void setup() {
  Serial.begin(9600);
  PgmPrintln("Type any character to start");
  while (!Serial.available());
  
  // initialize the SD card at SPI_FULL_SPEED for best performance.
  // try SPI_HALF_SPEED if bus errors occur.
  if (!card.init(SPI_FULL_SPEED)) error("card.init failed");
  
  // initialize a FAT volume
  if (!volume.init(&card)) error("volume.init failed");

  // open the root directory
  if (!root.openRoot(&volume)) error("openRoot failed");
  
  // delete files in root if FAT32
  if (volume.fatType() == 32) {
    PgmPrintln("Remove files in root");
    deleteFiles(root);
  }
  
  // open SUB1 and delete files
  SdFile sub1;
  if (!sub1.open(&root, "SUB1", O_READ)) error("open SUB1 failed");
  PgmPrintln("Remove files in SUB1");
  deleteFiles(sub1);

  // open SUB2 and delete files
  SdFile sub2;
  if (!sub2.open(&sub1, "SUB2", O_READ)) error("open SUB2 failed");
  PgmPrintln("Remove files in SUB2");
  deleteFiles(sub2);

  // remove SUB2
  if (!sub2.rmDir()) error("sub2.rmDir failed");
  PgmPrintln("SUB2 removed");
  
  // remove SUB1
  if (!sub1.rmDir()) error("sub1.rmDir failed");
  PgmPrintln("SUB1 removed");

  PgmPrintln("Done");
}

void loop() { }
