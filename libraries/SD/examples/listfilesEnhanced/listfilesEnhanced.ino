/*
  Listfiles Enhanced

  This example demonstrates how to list files on an SDcard in the following way:
   1) collect all directories
   2) build full path of directories and keep in mind
   3) then print all files with the help of the directorie pathes

  Wiring:
   SDcard attached to SPI bus as follows:
   - MOSI: pin 11
   - MISO: pin 12
   - CLK : pin 13
   - CS  : pin 4

  Created:
   18. Nov 2024 by Frank Häfele

  This example code is in the public domain.

*/
#include <SPI.h>
#include <SD.h>
#include <vector>

#define SD_CS_PIN 4


void dir(String path) {
  std::vector<String> directories;
  collectDirectories(path, directories);
  for (auto directory : directories) {
    printDirectoryName(directory.c_str(), 1);
    File fs = SD.open(directory);
    printFilesInDirectory(fs);
    Serial.println("\n===============");
    fs.close();
  }
}

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);

  Serial.print("\n\n==== List Directory ====\n\n");
  listDirectory();

  Serial.println("done!");
}

void loop() {
  // nothing happens after setup finishes.
}

void listDirectory() {
  Serial.print("\n\nInitializing SD card...");
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.print("initialization successful.\n");
  Serial.print("List Files:\n");
  dir("/");
}


void printDirectoryName(const char *name, uint8_t level) {
  for (uint8_t i = 0; i < level; ++i) {
    Serial.print("  ");
  }
  Serial.println(name);
}



// helper function: combine path
String joinPath(const String &base, const String &name) {
  if (base.endsWith("/")) {
    return base + name;
  }
  return base + "/" + name;
}

// recusive function to collect directory names
void collectDirectories(const String &dirname, std::vector<String> &directories) {
  File root = SD.open(dirname);
  if (!root || !root.isDirectory()) {
    Serial.printf("Error: Cannot open %s\n", dirname.c_str());
    return;
  }
  directories.push_back(dirname);

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      String fullPath = joinPath(dirname, file.name());
      collectDirectories(fullPath, directories);
    }
    file = root.openNextFile();
  }
  root.close();
}

// print filenames
void printFileName(File file) {
  Serial.print("\t");
  Serial.printf("%30s", file.name());
  // files have sizes, directories do not
  Serial.print(" - ");
  Serial.print(file.size(), DEC);
  time_t cr = file.getCreationTime();
  time_t lw = file.getLastWrite();
  struct tm *tmstruct = localtime(&cr);
  Serial.printf("\tCREATION: %d-%02d-%02d %02d:%02d:%02d", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
  tmstruct = localtime(&lw);
  Serial.printf("\tLAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
}


// print files in directories
void printFilesInDirectory(File dir) {
  while (true) {
    auto file = dir.openNextFile();
    if (!file) {
      // no more files
      break;
    }
    if (file.isDirectory()) {
      continue;
    } else {
      printFileName(file);
    }
  }
}
