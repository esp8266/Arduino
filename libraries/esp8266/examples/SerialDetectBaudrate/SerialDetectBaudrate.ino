#define TIMEOUT (10000UL)  // Maximum time to wait for serial activity to start

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  // Serial.detectBaudrate() may also be called before Serial.begin()
  // There must be activity on the serial port for the baudrate to be detected
  unsigned long detectedBaudrate = Serial.detectBaudrate(TIMEOUT);

  if (detectedBaudrate) {
    Serial.printf("\nDetected baudrate is %lu, switching to that baudrate now...\n", detectedBaudrate);

    // Wait for printf to finish
    while (Serial.availableForWrite() != UART_TX_FIFO_SIZE) {
      yield();
    }

    // Clear Tx buffer to avoid extra characters being printed
    Serial.flush();

    // After this, any writing to Serial will print gibberish on the serial monitor if the baudrate doesn't match
    Serial.begin(detectedBaudrate);
  } else {
    Serial.println("\nNothing detected");
  }
}

void loop() {
  // put your main code here, to run repeatedly:
}
