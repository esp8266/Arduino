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
    int volume = 1024;
    Audio.prepare(buffer, S, volume);
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

