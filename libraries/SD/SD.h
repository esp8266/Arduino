/*

 SD - a slightly more friendly wrapper for sdfatlib

 This library aims to expose a subset of SD card functionality
 in the form of a higher level "wrapper" object.

 License: GNU General Public License V3
          (Because sdfatlib is licensed with this.)

 (C) Copyright 2010 SparkFun Electronics

 */

#ifndef __SD_H__
#define __SD_H__

#include <WProgram.h>

#include <utility/SdFat.h>
#include <utility/SdFatUtil.h>

// Use this to configure the chip select pin of the SD card.
#define SD_CARD_CHIP_SELECT_PIN 4 // For use with Arduino Ethernet Shield

class File : public Print {
  public:
    virtual void write(uint8_t);
    virtual void write(const char *str);
    virtual void write(const uint8_t *buf, size_t size);
    int read();
    void close();
    operator bool();
};

class SDClass {

 private:
  // These are required for initialisation and use of sdfatlib
  Sd2Card card;
  SdVolume volume;
  SdFile root;
  
  
 public:
  // This needs to be called to set up the connection to the SD card
  // before other methods are used.
  void begin(uint8_t csPin = SD_CARD_CHIP_SELECT_PIN);
  
  // Open the specified file/directory with the supplied mode (e.g. read or
  // write, etc). Returns a File object for interacting with the file.
  // Note that currently only one file can be open at a time.
  File open(char *filename, boolean write = false, boolean append = true);

  // Methods to determine if the requested file path exists.
  boolean exists(char *filepath);

  // Create the requested directory heirarchy--if intermediate directories
  // do not exist they will be created.
  boolean makeDir(char *filepath);

 private:
  SdFile file;

  // This is used to determine the mode used to open a file
  // it's here because it's the easiest place to pass the 
  // information through the directory walking function. But
  // it's probably not the best place for it.
  // It shouldn't be set directly--it is set via the parameters to `open`.
  int fileOpenMode;
  
  friend class File;
  friend boolean callback_openPath(SdFile&, char *, boolean, void *); 
};

extern SDClass SD;

#endif
