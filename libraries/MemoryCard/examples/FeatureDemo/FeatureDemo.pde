/*

  A sketch to demonstrate the features of the MemoryCard library.

*/

// At the moment you need to include both the SdFat and MemoryCard libraries.
#include <SdFat.h>
#include <MemoryCard.h>

void setup() {

  Serial.begin(9600);
  
  
  // You always need to initialise the MemoryCard library
  Serial.println("Init MemoryCard...");
  MemoryCard.begin();


  // This demonstrates making a directory hierarchy
  Serial.println();
  Serial.println("Make directory...");
  MemoryCard.makeDir("/apple/banana/cabbage/");


  // You can check for the existence of specific files/directories
  char *filePathOne = "/apple/banana/cabbage/";
  char *filePathTwo = "/apple/banana/cabbage/dog/";

  Serial.println();
  
  Serial.print(filePathOne);
  Serial.print(" does ");
  if (MemoryCard.exists(filePathOne)) {
    Serial.println("exist.");
  } else {
    Serial.println("not exist.");
  }
  
  Serial.print(filePathTwo);
  Serial.print(" does ");
  if (MemoryCard.exists(filePathTwo)) {
    Serial.println("exist.");
  } else {
    Serial.println("not exist.");
  }
  

  // Demonstrate writing (and appending to existing file content)
  // to a file in a subdirectory
  Serial.println();
  Serial.println("Writing to 'dolphin.txt'.");
  
  MemoryCard.open("/apple/banana/cabbage/dolphin.txt", true);
  
  MemoryCard.file.println("This line was appended to the file.");
  
  MemoryCard.close();
  

  // Demonstrate writing to a file in the root directory and overwriting any
  // existing content
  Serial.println();  
  Serial.println("Writing to 'top.txt'.");
  
  MemoryCard.open("/top.txt", true, false);
  
  MemoryCard.file.println("This line overwrote the previous content of the file.");
  
  MemoryCard.close();


  // Demonstrate reading from a file in a subdirectory
  Serial.println();  
  Serial.println("Reading 'dolphin.txt':");
  
  MemoryCard.open("/apple/banana/cabbage/dolphin.txt");
  
  int c;
  
  // This approach may be easier to follow
  while(true) {
    c = MemoryCard.file.read();
    if (c < 0) {
      break;
    }
    Serial.print((char) c);
  }
  
  MemoryCard.close();
  

  // Demonstrate reading from a file in the root directory in a slightly different way
  Serial.println();  
  Serial.println("Reading 'top.txt':");
  
  MemoryCard.open("/top.txt");
  
  // This approach is more compact
  while((c = MemoryCard.file.read()) >= 0) {
    Serial.print((char) c);
  }
  
  MemoryCard.close();
  
  
  // Demonstration complete!
  Serial.println();  
  Serial.println("Done.");
}

void loop() {
}


