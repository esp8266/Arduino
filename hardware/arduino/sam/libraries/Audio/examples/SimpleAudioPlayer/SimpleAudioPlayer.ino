#include <SD.h>
#include <SPI.h>
#include <Audio.h>

void setup()
{
  // debug output at 115.2K
  Serial.begin(115200);

  // setup SD-card
  Serial.print("Initializing SD card...");
  if (!SD.begin(4)) {
    Serial.println(" failed!");
    return;
  }
  Serial.println(" done.");
  // hi-speed SPI transfers
  SPI.setClockDivider(4);

  // 44100Khz stereo => 88200 sample rate
  // 100 mSec of prebuffering.
  Audio.begin(88200, 100);
}

void loop()
{
  int count=0;

  // open wave file from sdcard
  File myFile = SD.open("fadh.wav");
  if (!myFile) {
    // if the file didn't open, print an error and stop
    Serial.println("error opening fadh.wav");
    while (true);
  }

  const int S=1024; // Number of samples to read in block
  int16_t buffer[S];

  Serial.print("Playing");
  // until the file is not finished
  while (myFile.available()) {
    // read from the file into buffer
    myFile.read(buffer, sizeof(buffer));

    // Prepare samples
    int volume = analogRead(2);
    prepare(buffer, S, volume);
    // Feed samples to audio
    Audio.write(buffer, S);

    // Every 100 block print a '.'
    count++;
    if (count == 100) {
      Serial.print(".");
      count = 0;
    }
  }
  myFile.close();


  Serial.println("End of file. Thank you for listening!");
  while (true) ;
}


void prepare(int16_t *buffer, int S, int volume) {
  uint16_t *ubuffer = (uint16_t*) buffer;
  for (int i=0; i<S; i++) {
    // set volume amplitude (signed multiply)
    buffer[i] = buffer[i] * volume / 1024;
    // convert from signed 16 bit to unsigned 12 bit for DAC.
    ubuffer[i] += 0x8000;
    ubuffer[i] >>= 4;
  }
}

