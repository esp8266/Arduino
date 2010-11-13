/*
 * This sketch reads and prints the tail of all files
 * created by SdFatAppend.pde, SdFatPrint.pde, and
 * SdFatWrite.pde.
 */
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
  Serial.println("type any character to start");
  while (!Serial.available());
  Serial.println();
  
  // initialize the SD card at SPI_HALF_SPEED to avoid bus errors with
  // breadboards.  use SPI_FULL_SPEED for better performance.
  if (!card.init(SPI_HALF_SPEED)) error("card.init failed");
  
  // initialize a FAT volume
  if (!volume.init(&card)) error("volume.init failed");
  
  // open the root directory
  if (!root.openRoot(&volume)) error("openRoot failed");
}

/*
 * Print tail of all SdFat example files
 */
void loop(void) {
  dir_t dir;
  char name[13];
  
  // read next directory entry
  if (root.readDir(&dir) != sizeof(dir)) {
    Serial.println("End of Directory");
    while(1);
  }
  
  // check for file name "APPEND.TXT", "PRINT*.TXT"
  // or "WRITE*.TXT"
  // first 8 bytes are blank filled name
  // last three bytes are blank filled extension
  if ((strncmp((char *)dir.name, "APPEND  ", 8) &&
      strncmp((char *)dir.name, "PRINT", 5) &&
      strncmp((char *)dir.name, "WRITE", 5)) ||
      strncmp((char *)&dir.name[8], "TXT", 3)) {
        return;
  }
  // format file name
  SdFile::dirName(dir, name);
  
  // remember position in root dir
  uint32_t pos = root.curPosition();
  
  // open file
  if (!file.open(&root, name, O_READ)) error("file.open failed");
  
  // restore root position
  if (!root.seekSet(pos)) error("root.seekSet failed");
  
  // print file name message
  Serial.print("Tail of: ");
  Serial.println(name);
  
  // position to tail of file
  if (file.fileSize() > 100) {
    if (!file.seekSet(file.fileSize() - 100)) error("file.seekSet failed");
  }
  int16_t c;
  // find end of line  
  while ((c = file.read()) > 0 && c != '\n');
  
  // print rest of file
  while ((c = file.read()) > 0) Serial.print((char)c);
  file.close();
  Serial.println();
}
