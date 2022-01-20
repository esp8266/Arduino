// Example: Storing struct data in RTC user rtcDataory
//
// Struct data with the maximum size of 512 bytes can be stored
// in the RTC user rtcDataory using the ESP-specifc APIs.
// The stored data can be retained between deep sleep cycles.
// However, the data might be lost after power cycling the ESP8266.
//
// This example uses deep sleep mode, so connect GPIO16 and RST
// pins before running it.
//
// Created Mar 30, 2016 by Macro Yau.
//
// This example code is in the public domain.

// CRC function used to ensure data validity
uint32_t calculateCRC32(const uint8_t* data, size_t length);

// helper function to dump memory contents as hex
void     printMemory();

// Structure which will be stored in RTC memory.
// First field is CRC32, which is calculated based on the
// rest of structure contents.
// Any fields can go after CRC32.
// We use byte array as an example.
struct
{
  uint32_t crc32;
  byte     data[508];
} rtcData;

void setup()
{
  Serial.begin(115200);
  Serial.println();
  delay(1000);

  // Read struct from RTC memory
  if (ESP.rtcUserMemoryRead(0, (uint32_t*)&rtcData, sizeof(rtcData)))
  {
    Serial.println("Read: ");
    printMemory();
    Serial.println();
    uint32_t crcOfData = calculateCRC32((uint8_t*)&rtcData.data[0], sizeof(rtcData.data));
    Serial.print("CRC32 of data: ");
    Serial.println(crcOfData, HEX);
    Serial.print("CRC32 read from RTC: ");
    Serial.println(rtcData.crc32, HEX);
    if (crcOfData != rtcData.crc32)
    {
      Serial.println("CRC32 in RTC memory doesn't match CRC32 of data. Data is probably invalid!");
    }
    else
    {
      Serial.println("CRC32 check ok, data is probably valid.");
    }
  }

  // Generate new data set for the struct
  for (size_t i = 0; i < sizeof(rtcData.data); i++)
  {
    rtcData.data[i] = random(0, 128);
  }
  // Update CRC32 of data
  rtcData.crc32 = calculateCRC32((uint8_t*)&rtcData.data[0], sizeof(rtcData.data));
  // Write struct to RTC memory
  if (ESP.rtcUserMemoryWrite(0, (uint32_t*)&rtcData, sizeof(rtcData)))
  {
    Serial.println("Write: ");
    printMemory();
    Serial.println();
  }

  Serial.println("Going into deep sleep for 5 seconds");
  ESP.deepSleep(5e6);
}

void loop()
{
}

uint32_t calculateCRC32(const uint8_t* data, size_t length)
{
  uint32_t crc = 0xffffffff;
  while (length--)
  {
    uint8_t c = *data++;
    for (uint32_t i = 0x80; i > 0; i >>= 1)
    {
      bool bit = crc & 0x80000000;
      if (c & i)
      {
        bit = !bit;
      }
      crc <<= 1;
      if (bit)
      {
        crc ^= 0x04c11db7;
      }
    }
  }
  return crc;
}

//prints all rtcData, including the leading crc32
void printMemory()
{
  char     buf[3];
  uint8_t* ptr = (uint8_t*)&rtcData;
  for (size_t i = 0; i < sizeof(rtcData); i++)
  {
    sprintf(buf, "%02X", ptr[i]);
    Serial.print(buf);
    if ((i + 1) % 32 == 0)
    {
      Serial.println();
    }
    else
    {
      Serial.print(" ");
    }
  }
  Serial.println();
}
