/*
 * ESP8266 I2C master-slave communication, requires Arduno Core with I2C Slave Support (2.5.0+)
 *
 * Expects two ESP8266 devices with three pins connected: GND, SDA and SCL. This is slave.
 * Will wait for "MESA" message and then respond with "PONG" message,
 * or request retransfer if message was misunderstood (e.g. CRC check failed).
 * Message can be up to 26 bytes long (plus wrapper).
 *
 * 21-11-2018: initial drop by Matej Sychra (github.com/suculent)
 */

#include <Wire.h>
#include "crc16.h"

#define SDA_PIN 4 // D1
#define SCL_PIN 5 // D2
#define LED_PIN 12 // D4

const uint16_t I2C_ADDRESS = 0x08;
const uint16_t I2C_MASTER = 0x42;
uint16_t local_address = I2C_ADDRESS; // slave

// Keep this structure in sync with the i2c-P2P-master example. Should be in shared header.
struct MESSAGE_DATA {
  uint8_t sequence;
  uint16_t crc16;
  uint8_t data[26];
  uint8_t terminator;
};

MESSAGE_DATA message;
MESSAGE_DATA msgdata;

Crc16 crc;
byte seq = 0;

// forward declarations
uint16_t calculateCRC16(uint8_t *data, size_t length);
void sendEvent(int a, String msg);
void sendMessage(int seq, String msg);
void receiveEvent(int howMany);
MESSAGE_DATA validateMessage(char* bytes_message);

//

// should be in shared header
uint16_t calculateCRC16(uint8_t *data, size_t length) {
  crc.clearCrc();
  unsigned short value = crc.XModemCrc(data, 0, length);
  Serial.print("crc = 0x"); Serial.println(value, HEX);
  return (uint16_t)value;
}

void setup() {

  // Enable internal pullup (there's always one from the master side)
  //digitalWrite(SDA_PIN, HIGH);
  //digitalWrite(SCL_PIN, HIGH);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Wire.pins(SDA_PIN, SCL_PIN);
  Wire.begin(I2C_ADDRESS); // address required for slave

  Serial.begin(230400);
  //while (!Serial); if you want to wait for first messages

  delay(2000);
  digitalWrite(LED_PIN, HIGH);

  Wire.onReceive(receiveEvent);

  Serial.print("I2C Slave started on address: 0x0");
  Serial.println(I2C_ADDRESS, HEX);
}

// should be in shared header
MESSAGE_DATA encodeMessage(String bytes) {

  msgdata.terminator = '.';

  unsigned int datasize = sizeof(msgdata.data);
  Serial.print("Encoding data of size: ");
  Serial.println(datasize);

  // Generate new data set for the struct
  for (size_t i = 0; i < datasize; i++) {
    if (bytes.length() >= i) {
      msgdata.data[i] = bytes[i];
    } else {
      msgdata.data[i] = '\0';
    }
    Serial.print(msgdata.data[i]);
    Serial.print(" ");
  }
  Serial.println();
  msgdata.crc16 = calculateCRC16((uint8_t*) &msgdata.data[0], datasize);
  Serial.print("Outgoing Message CRC16: "); Serial.println(msgdata.crc16, HEX);
  return msgdata;
}

// should be in shared header
MESSAGE_DATA validateMessage(char* bytes_message) {

  MESSAGE_DATA tmp;
  memcpy(&tmp, bytes_message, sizeof(tmp));

  // Validate terminator
  if (tmp.terminator == '.') {
    Serial.println("[OK] Terminator valid.");
  } else {
    Serial.print("[ERROR] Terminator invalid: '");
    Serial.print(tmp.terminator);
    Serial.println("'");
    return tmp;
  }

  int datasize = sizeof(tmp.data);
  if (datasize != 28) {
    Serial.print("Data of size: "); Serial.println(datasize);
    if (datasize > 28) {
      datasize = 28;
    }
  }

  // Calculate data CRC
  uint16_t data_crc = calculateCRC16((uint8_t*) &tmp.data[0], sizeof(datasize));

  // should be only when CRC is good like commented below
  char inmsg[datasize];
  memcpy(inmsg, &tmp.data, datasize);

  // Validate incoming data CRC against remote CRC
  if (tmp.crc16 == data_crc) {
    Serial.println("[OK] Data CRC valid.");
    Serial.print("SLAVE Incoming message: "); Serial.println(String(inmsg));
  } else {
    Serial.print("CRC-A = 0x");
    Serial.println(tmp.crc16, HEX);
    Serial.print("CRC-B = 0x");
    Serial.println(data_crc, HEX);
    Serial.print("tmp CRC16: ");
    Serial.println(tmp.crc16, HEX);
    Serial.print("SLAVE Incoming message: "); Serial.println(String(inmsg));
    Serial.println("[ERROR] TODO: Request retransfer exception.");
    return tmp;
  }

  // Validate sequence number ( should be already updated from byteParser )
  uint8_t remote_sequence = tmp.sequence;
  if (remote_sequence != seq) {
    Serial.println((char*)tmp.data);
  }

  msgdata = tmp; // tmp is valid, assign to result address

  return msgdata;

}

void receiveEvent(int howMany) {

  Serial.print("Received "); Serial.print(howMany); Serial.println(" bytes...");
  char c;
  char chars[howMany];
  chars[howMany - 1] = '\0';
  int index = 0;

  Serial.print("Local Sequence: "); Serial.println((int)seq);
  digitalWrite(LED_PIN, LOW);

  bool requestRetransfer = false;

  unsigned long rtimeout = millis() + 200; // skip 100ms timeouts
  while (millis() < rtimeout) {
    while (0 < Wire.available()) { // loop through all but the last
      //digitalWrite(LED_PIN, LOW);
      c = Wire.read();
      chars[index] = c;
      //Serial.print(index); Serial.print(" : "); Serial.println((int)c);
      Serial.print((int)c);
      Serial.print(" ");

      // Parses first byte for sequence number, contains recovery logic...
      if (index == 0) {

         Serial.print("Remote Sequence: "); Serial.println((int)c);

         if ( c!= seq && ((c > seq - 4) || (c < seq + 4)) && c != seq + 1) {
          Serial.print("[DIFF] Sequence offset [!]: ");
          Serial.println(c - seq);
          Serial.print("Re-assigning sequence number: ");
          Serial.println(seq);
          seq = c;
          requestRetransfer = true;

        } else if (c != seq + 1) {

          Serial.print("[+DIFF] Local Sequence: ");
          Serial.println(seq);
          Serial.print("[+DIFF] Sequence offset [!]: ");
          Serial.println(c - seq);
          requestRetransfer = true;
        }

        if (seq == 0) {
          Serial.print("Assigning sequence 0: ");
          Serial.println(seq);
          seq = c;

        } else if (c != seq + 1) {
          Serial.print("Re-assigning sequence number: ");
          Serial.println(seq);
          seq = c;
        }
        seq = c;

      } // if 0

      digitalWrite(LED_PIN, HIGH);
      index++;
    } // while
  } // while

  Serial.println(String(chars));

  if (requestRetransfer) {
      String event = String("R") + String(seq) + String('\0');
      sendMessage(seq, event);
    } else {

  }

  Serial.print("Decoding data of size: "); Serial.println(sizeof(chars));
  message = validateMessage(chars); // &error

  // TODO: Do something with the message.
  char inmessage[5] = {0};
  memcpy(inmessage, (const char*)&message.data, 4);
  String inmsg = String(inmessage);
  if (inmsg.indexOf("MESA") == 0) {
    String event = String("PONG");
    sendMessage(seq, event);
  }

}

void sendMessage(int seq, String msg) {
  Wire.beginTransmission(0x42); // transmit to device #8
  //Sending Side
  MESSAGE_DATA struct_data = encodeMessage(msg);
  struct_data.sequence = seq;
  char b[sizeof(struct_data)];
  memcpy(b, &struct_data, sizeof(struct_data));
  Serial.print("Sending message of size "); Serial.print(sizeof(struct_data)); Serial.println();
  Serial.print("'");
  for (unsigned int i = 0; i < sizeof(struct_data); i++) {
    Wire.write(b[i]);
    Serial.print(b[i]);
  }
  Serial.println("' sent...");
  Wire.endTransmission();    // stop transmitting
}

// wire_transmit
void sendEvent(int a, String msg) {
  Serial.print("Sending ");
  Serial.println(msg);
  Wire.beginTransmission(0x42); // transmit to master
  Wire.write(a);
  Wire.write(";");        // sends five bytes
  Wire.write(msg.c_str());
  Wire.endTransmission();    // stop transmitting
}

void loop() {
  // nothing here, slave receives data and sends reply in own callback
}
