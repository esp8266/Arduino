/*

 MemoryCard - a slightly more friendly wrapper for sdfatlib

 This library aims to expose a subset of SD card functionality
 in the form of a higher level "wrapper" object.

 License: GNU General Public License V3
          (Because sdfatlib is licensed with this.)

 (C) Copyright 2010 SparkFun Electronics

 */

#ifndef __MEMORY_CARD_DEVICE_H__
#define __MEMORY_CARD_DEVICE_H__

#include <WProgram.h>

#include <SdFat.h>

class MemoryCardDevice {

 private:
  // These are required for initialisation and use of sdfatlib
  Sd2Card card;
  SdVolume volume;
  SdFile root;
  
  
 public:
  // This needs to be called to set up the connection to the memory card
  // before other methods are used.
  void begin();
  
  // Open the specified file/directory with the supplied mode (e.g. read or
  // write, etc). Once opened the file can be accessed via the
  // `MemoryCard.file` field which is a standard `sdfatlib` file object.
  boolean open(char *filename, boolean write = false, boolean append = true);

  // Close an opened file object.
  boolean close();

  // Methods to determine if the requested file path exists.
  boolean exists(char *filepath);
  boolean exists(char *filepath, SdFile& parentDir);

  // Create the requested directory heirarchy--if intermediate directories
  // do not exist they will be created.
  boolean makeDir(char *filepath);

  // At the moment this is how a developer interacts with a file they've
  // opened. It's unclear whether it would be better to make
  // `MemoryCard` provide a `Stream` interface instead.
  SdFile file; // TODO: Don't make this public?

  // This is used to determine the mode used to open a file
  // it's here because it's the easiest place to pass the 
  // information through the directory walking function. But
  // it's probably not the best place for it.
  // It shouldn't be set directly--it is set via the parameters to `open`.
  int fileOpenMode; // TODO: Don't make this public?
};

#endif
