/*
 * This sketch attempts to initialize a SD card and analyze its structure.
 */
#include <SdFat.h>
#include <SdFatUtil.h>

// offset to partition table
#define PART_OFFSET (512-64-2) 

Sd2Card card;
SdVolume vol;

//global for card erase sector size
uint32_t sectorSize; 

void sdError(void) {
  PgmPrintln("SD error");
  PgmPrint("errorCode: ");
  Serial.println(card.errorCode(), HEX);
  PgmPrint("errorData: ");
  Serial.println(card.errorData(), HEX);  
  return;
}

uint8_t cidDmp(void) {
  cid_t cid;
  if (!card.readCID(&cid)) {
    PgmPrint("readCID failed");
    sdError();
    return false;
  }
  PgmPrint("\nManufacturer ID: ");
  Serial.println(cid.mid, HEX);
  PgmPrint("OEM ID: ");
  Serial.print(cid.oid[0]);
  Serial.println(cid.oid[1]);
  PgmPrint("Product: ");
  for (uint8_t i = 0; i < 5; i++) {
    Serial.print(cid.pnm[i]);
  }
  PgmPrint("\nVersion: ");
  Serial.print(cid.prv_n, DEC);
  Serial.print('.');
  Serial.println(cid.prv_m, DEC);
  PgmPrint("Serial number: ");
  Serial.println(cid.psn);
  PgmPrint("Manufacturing date: ");
  Serial.print(cid.mdt_month);
  Serial.print('/');
  Serial.println(2000 + cid.mdt_year_low + (cid.mdt_year_high <<4));
  Serial.println();
  return true;
}

uint8_t csdDmp(void) {
  csd_t csd;
  uint8_t eraseSingleBlock;
  uint32_t cardSize = card.cardSize();
  if (cardSize == 0 || !card.readCSD(&csd)) {
    PgmPrintln("readCSD failed");
    sdError();
    return false;
  }
  if (csd.v1.csd_ver == 0) {
    eraseSingleBlock = csd.v1.erase_blk_en;
    sectorSize = (csd.v1.sector_size_high << 1) | csd.v1.sector_size_low;
  }
  else if (csd.v2.csd_ver == 1) {
    eraseSingleBlock = csd.v2.erase_blk_en;
    sectorSize = (csd.v2.sector_size_high << 1) | csd.v2.sector_size_low; 
  }
  else {
    PgmPrintln("csd version error");
    return false;
  }
  sectorSize++;
  PgmPrint("cardSize: ");
  Serial.print(cardSize);
  PgmPrintln(" (512 byte blocks)");
  PgmPrint("flashEraseSize: ");
  Serial.print(sectorSize, DEC);
  PgmPrintln(" blocks");
  PgmPrint("eraseSingleBlock: ");
  if (eraseSingleBlock) {
    PgmPrintln("true");
  }
  else {
    PgmPrintln("false");
  }
  return true;
}
// print partition table
uint8_t partDmp(void) {
  part_t pt;
  PgmPrintln("\npart,boot,type,start,length");  
  for (uint8_t ip = 1; ip < 5; ip++) {
    if (!card.readData(0, PART_OFFSET + 16*(ip-1), 16, (uint8_t *)&pt)) {
      PgmPrint("read partition table failed");
      sdError();
      return false;
    }
    Serial.print(ip, DEC);
    Serial.print(',');
    Serial.print(pt.boot,HEX);
    Serial.print(',');
    Serial.print(pt.type, HEX);
    Serial.print(',');
    Serial.print(pt.firstSector);
    Serial.print(',');
    Serial.println(pt.totalSectors); 
  }
  return true;
}

void volDmp(void) {
  PgmPrint("\nVolume is FAT");
  Serial.println(vol.fatType(), DEC);
  PgmPrint("blocksPerCluster: ");
  Serial.println(vol.blocksPerCluster(), DEC);
  PgmPrint("clusterCount: ");
  Serial.println(vol.clusterCount());
  PgmPrint("fatStartBlock: ");
  Serial.println(vol.fatStartBlock());
  PgmPrint("fatCount: ");
  Serial.println(vol.fatCount(), DEC);
  PgmPrint("blocksPerFat: ");
  Serial.println(vol.blocksPerFat());
  PgmPrint("rootDirStart: ");
  Serial.println(vol.rootDirStart());
  PgmPrint("dataStartBlock: ");
  Serial.println(vol.dataStartBlock());
  if (vol.dataStartBlock()%sectorSize) {
    PgmPrintln("Data area is not aligned on flash erase boundaries!");
  }
}

void setup() {
  Serial.begin(9600);
}

void loop() {
  PgmPrintln("\ntype any character to start");
  while (!Serial.available());
  Serial.flush();
  uint32_t t = millis();
  // initialize the SD card at SPI_HALF_SPEED to avoid bus errors with
  // breadboards.  use SPI_FULL_SPEED for better performance.
  uint8_t r = card.init(SPI_HALF_SPEED);
  t = millis() - t;
  if (!r) {
    PgmPrintln("\ncard.init failed");
    sdError();
    return;
  }
  PgmPrint("\ninit time: ");
  Serial.println(t);
  PgmPrint("\nCard type: ");
  switch(card.type()) {
    case SD_CARD_TYPE_SD1:
      PgmPrintln("SD1");
      break;
    case SD_CARD_TYPE_SD2:
      PgmPrintln("SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      PgmPrintln("SDHC");
      break;
    default:
      PgmPrintln("Unknown");
  }
  if(!cidDmp()) return;
  if(!csdDmp()) return;
  if(!partDmp()) return;
  if (!vol.init(&card)) {
    PgmPrintln("\nvol.init failed");
    sdError();
    return;
  }
  volDmp();
}
