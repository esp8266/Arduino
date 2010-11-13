/*

  A sketch to demonstrate the features of the SD library.

*/

#include <SD.h>

void setup() {

  Serial.begin(9600);
  
  
  // You always need to initialise the SD library
  Serial.println("Init SD...");
  SD.begin();


  // This demonstrates making a directory hierarchy
  Serial.println();
  Serial.println("Make directory...");
  SD.makeDir("/apple/banana/cabbage/");


  // You can check for the existence of specific files/directories
  char *filePathOne = "/apple/banana/cabbage/";
  char *filePathTwo = "/apple/banana/cabbage/dog/";

  Serial.println();
  
  Serial.print(filePathOne);
  Serial.print(" does ");
  if (SD.exists(filePathOne)) {
    Serial.println("exist.");
  } else {
    Serial.println("not exist.");
  }
  
  Serial.print(filePathTwo);
  Serial.print(" does ");
  if (SD.exists(filePathTwo)) {
    Serial.println("exist.");
  } else {
    Serial.println("not exist.");
  }
  

  // Demonstrate writing (and appending to existing file content)
  // to a file in a subdirectory
  Serial.println();
  Serial.println("Writing to 'dolphin.txt'.");
  
  SD.open("/apple/banana/cabbage/dolphin.txt", true);
  
  SD.file.println("This line was appended to the file.");
  
  SD.close();
  

  // Demonstrate writing to a file in the root directory and overwriting any
  // existing content
  Serial.println();  
  Serial.println("Writing to 'top.txt'.");
  
  SD.open("/top.txt", true, false);
  
  SD.file.println("This line overwrote the previous content of the file.");
  
  SD.close();


  // Demonstrate reading from a file in a subdirectory
  Serial.println();  
  Serial.println("Reading 'dolphin.txt':");
  
  SD.open("/apple/banana/cabbage/dolphin.txt");
  
  int c;
  
  // This approach may be easier to follow
  while(true) {
    c = SD.file.read();
    if (c < 0) {
      break;
    }
    Serial.print((char) c);
  }
  
  SD.close();
  

  // Demonstrate reading from a file in the root directory in a slightly different way
  Serial.println();  
  Serial.println("Reading 'top.txt':");
  
  SD.open("/top.txt");
  
  // This approach is more compact
  while((c = SD.file.read()) >= 0) {
    Serial.print((char) c);
  }
  
  SD.close();
  
  
  // Demonstration complete!
  Serial.println();  
  Serial.println("Done.");
}

void loop() {
}


