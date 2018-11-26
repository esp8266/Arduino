/*
 * ESP8266 I2C master-slave communication, requires Arduno Core with I2C Slave Support (2.5.0+)
 *
 * Expects two ESP8266 devices with three pins connected: GND, SDA and SCL. This is master.
 * Will send "MESA" messages to predefined slave address and then expect "PONG" response,
 * or request to retransfer if message was misunderstood (e.g. CRC check failed).
 * Message can be up to 26 bytes long (plus wrapper).
 *
 * 21-11-2018: initial drop by Matej Sychra (github.com/suculent)
 */

#include <Wire.h>
#include "crc16.h"

#define SDA_PIN 4 // D1
#define SCL_PIN 5 // D2
#define LED_PIN 12 // D4

const int16_t I2C_MASTER = 0x42;
const int16_t I2C_SLAVE = 0x08;
int16_t slave_address = I2C_SLAVE; 

// Keep this structure in sync with the i2c-P2P-slave example. Should be in shared header.
struct MessageData {
  uint8_t sequence;
  uint16_t crc16;
  uint8_t data[26];
  uint8_t terminator;
};

MessageData msgdata;

Crc16 crc;
byte sequence = 0;

bool expectPong = false;
unsigned long nextPing = millis() + 1000;

// analytics
uint16_t errorCount = 0;
uint16_t byteCount = 0;

// forward declarations
uint16_t calculateCRC16(const uint8_t *data, size_t length);
void sendMessage(const int seq, const String &msg);
MessageData encodeMessage(const String &bytes);
void receiveEvent(const size_t howMany);
bool validateMessage(const char* bytes, MessageData &tmp);

//

// should be in shared header
uint16_t calculateCRC16(uint8_t *data, size_t length) {
  crc.clearCrc();
  uint16_t value = (uint16_t)crc.XModemCrc(data,0,length);
  Serial.print("crc = 0x");
  Serial.println(value, HEX);
  return value;
}

// should be in shared header
MessageData encodeMessage(const String &instring) {

  MessageData msgdata;

  msgdata.terminator = '.';

  int datasize = sizeof(msgdata.data);
  Serial.print("\nEncoding data of size: ");
  Serial.println(datasize);

  // Generate new data set for the struct
  for (size_t i = 0; i < datasize; ++i) {
    if (instring.length() >= i) {
      msgdata.data[i] = instring[i];
    } else {
      msgdata.data[i] = '\0';
    }
    Serial.print(msgdata.data[i]);
    Serial.print(" ");
  }
  Serial.print('\n');
  msgdata.crc16 = calculateCRC16((uint8_t*) &msgdata.data[0], datasize);
  return msgdata;
}

// should be in shared header
bool validateMessage(const char* bytes, MessageData &tmp) {

  // Validate PROTOCOL terminator
  if (tmp.terminator != '.') {
    Serial.print("[ERROR] Terminator invalid: '");
    Serial.print(tmp.terminator);
    Serial.println("'");
    return false;
  }

  int datasize = sizeof(tmp.data);
  Serial.print("Data of size: ");
  Serial.println(datasize);

  uint16_t data_crc = calculateCRC16((uint8_t*) &tmp.data[0], datasize);

  // Validate incoming data CRC against remote CRC
  if (tmp.crc16 == data_crc) {
    Serial.println("[OK] Data CRC valid.");
    char inmsg[datasize];
    memcpy(inmsg, &tmp.data, datasize);
    Serial.print("MASTER Incoming message: ");
    Serial.println(String(inmsg));
  } else {
    Serial.print("CRC-A = 0x");
    Serial.println(tmp.crc16, HEX);
    Serial.print("Incoming message CRC-B = 0x");
    Serial.println(data_crc, HEX);
    Serial.print("tmp CRC16: ");
    Serial.println(tmp.crc16, HEX);
    Serial.println("[ERROR] Request retransfer exception.");
    return true;
  }

  // Validate sequence number
  uint8_t remote_sequence = tmp.sequence;
  if (remote_sequence < sequence - 1) {
    Serial.print("[WARNING] TODO: Unexpected sequence number: ");
    Serial.print(remote_sequence);
    Serial.print(" while local is ");
    Serial.println(sequence); // TODO: return error
  }

  return true;
}

void sendMessage(const int seq, const String &msg) {

  Serial.print("[MASTER] Sending message: ");
  Serial.print(msg);

  if (msg.indexOf("MESA") == 0) {
    expectPong = true;
  } else {
    expectPong = false;
  }

  Wire.beginTransmission(I2C_SLAVE);
  //Sending Side
  MessageData struct_data = encodeMessage(msg);
  struct_data.sequence = seq;
  char buf[sizeof(struct_data)];
  Serial.print(" of size ");
  Serial.println(sizeof(struct_data));
  Serial.print("Encoding struct of size: ");
  Serial.println(sizeof(struct_data));
  memcpy(buf, &struct_data, sizeof(struct_data));
  for (int i = 0; i < sizeof(struct_data); ++i) {
    Wire.write(buf[i]);
    Serial.print(buf[i]);
    Serial.print(" ");
  }
  Wire.endTransmission();    // stop transmitting
  Serial.println("");
}

void receiveEvent(const size_t howMany) {

  byteCount = byteCount + howMany;

  char incoming[howMany];
  incoming[howMany - 1] = '\0';
  int index = 0;

  digitalWrite(LED_PIN, LOW);

  unsigned long interval = 100;
  unsigned long currentMillis = millis();
  unsigned long previousMillis = millis();
 // message duration may not exceed 100 ms (100 bytes, 3x buffer retransferred)
  if(currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;
    // this must be fast, keep free from logging to serial here if possible
    while (0 < Wire.available()) { // loop through all but the last
      int c = Wire.read(); // receive byte as a character
      if (index < howMany) {
        incoming[index] = (char)c; // save the character
      }
      ++index;
    }
  }

  Serial.println();
  Serial.print("[MASTER] Received ");
  Serial.print(index);
  Serial.println(" bytes from SLAVE.");
  Serial.println((int)sequence);

  MessageData message;

  bool success = validateMessage(incoming, message);

  char inmsg[sizeof(message.data)];
  memcpy(inmsg, &message.data, sizeof(message.data));
  if (inmsg[0] == 'R') {
    String retransfer_command = String(inmsg);
    retransfer_command.replace("R", "");
    int retransfer_offset = retransfer_command.toInt();
    Serial.println(sequence);
    Serial.print("[MASTER] Retransfer request: ");
    // dumb retransfer, only changes sequence number when expecting PONG
    if (expectPong) {
      sequence = retransfer_offset + 1;
    } else {
      // debug only, WARNING! may loose important messages in production
      sequence = retransfer_offset + 1;
    }
    Serial.println(sequence);
  }

  String instring = String(inmsg);
  if (instring.indexOf("PONG")) {
    // alles kitz...
    expectPong = false;
  }

  //  Do something with the message...
  digitalWrite(LED_PIN, HIGH);
}

void setup() {

  delay(2000);

  Wire.pins(SDA_PIN, SCL_PIN);
  Wire.begin(I2C_MASTER);
  //Wire.begin(SDA_PIN, SCL_PIN, I2C_MASTER); // new syntax: join i2c bus (address optional for master)
  Wire.onReceive(receiveEvent);

  Serial.begin(230400); // keep serial fast as possible for debug logging
  while (!Serial);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
}

unsigned long secondTimer = millis() + 1000;
void loop() {

  if (millis() > secondTimer) {
    Serial.print("Errors/Bytes: ");
    Serial.print(errorCount);
    Serial.print("/");
    Serial.print(byteCount);
    Serial.println(" per second");
    errorCount = 0;
    byteCount = 0;
    secondTimer = millis() + 1000;
  }

  if (millis() > nextPing) {
    digitalWrite(LED_PIN, LOW);
    nextPing = millis() + 200;
    Serial.print("[MASTER] Sequence » ");
    Serial.println(sequence);
    digitalWrite(LED_PIN, HIGH);
    sendMessage(sequence, "MESA");
    if (expectPong) {
      errorCount++;
    }
    expectPong = true;
    sequence++;
  }

}
