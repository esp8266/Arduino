
#include <SPI.h>

#define FLASH_SPI_CHAN 3

void setup() {
  Serial1.begin(9600);
  SPI_0.begin();
  SPI_0.addSlave(FLASH_SPI_CHAN);
  SPI_0.setClockDivider(2); // We are too fast with 1
}

void loop() {
  Serial1.println("Sending 'Identify' cmd to flash => 9F");
  
  // Send cmd and receive response on the same transaction
  // Parameter "false" keeps the SS pin active
  SPI_0.transfer(0x9f, FLASH_SPI_CHAN, false);
  char a1 = SPI_0.transfer(0x00, FLASH_SPI_CHAN, false);
  char a2 = SPI_0.transfer(0x00, FLASH_SPI_CHAN, false);
  char a3 = SPI_0.transfer(0x00, FLASH_SPI_CHAN, false);
  char a4 = SPI_0.transfer(0x00, FLASH_SPI_CHAN, false);
  char a5 = SPI_0.transfer(0x00, FLASH_SPI_CHAN);

  Serial1.print("Received signature: ");
  Serial1.print(a1, HEX);
  Serial1.print(" ");
  Serial1.print(a2, HEX);
  Serial1.print(" ");
  Serial1.print(a3, HEX);
  Serial1.print(" ");
  Serial1.print(a4, HEX);
  Serial1.print(" ");
  Serial1.print(a5, HEX);
  Serial1.println();
  delay(1000);
}

