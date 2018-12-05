
/*
  Serial read/write/verify/benchmark
  Using internal loopback
  Using SoftwareSerial library for logging

  Sketch meant for debugging only
  Released to public domain
*/

#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

#define SSBAUD          115200  // logger on console for humans
#define BAUD            3000000 // hardware serial stress test
#define BUFFER_SIZE     4096    // may be useless to use more than 2*SERIAL_SIZE_RX
#define SERIAL_SIZE_RX  1024    // Serial.setRxBufferSize()

#define TIMEOUT 5000
#define DEBUG(x...) //x

uint8_t buf [BUFFER_SIZE];
uint8_t temp [BUFFER_SIZE];

static size_t out_idx = 0, in_idx = 0;
static size_t local_receive_size = 0;
static size_t size_for_1sec, size_for_led = 0;
static size_t maxavail = 0;
static uint64_t in_total = 0, in_prev = 0;
static uint64_t start_ms, last_ms;
static uint64_t timeout;

Stream* logger;

void error(const char* what) {
  logger->printf("\r\nerror: %s after %ld minutes\r\nread idx:  %d\r\nwrite idx: %d\r\ntotal:     %ld\r\nlast read: %d\r\nmaxavail:  %d\r\n",
                 what, (long)((millis() - start_ms) / 60000), in_idx, out_idx, (long)in_total, (int)local_receive_size, maxavail);
  if (Serial.hasOverrun()) {
    logger->printf("overrun!\r\n");
  }
  logger->printf("should be (size=%d idx=%d..%d):\r\n    ", BUFFER_SIZE, in_idx, in_idx + local_receive_size - 1);
  for (size_t i = in_idx; i < in_idx + local_receive_size; i++) {
    logger->printf("%02x(%c) ", buf[i], (unsigned char)((buf[i] > 31 && buf[i] < 128) ? buf[i] : '.'));
  }
  logger->print("\r\n\r\nis: ");
  for (size_t i = 0; i < local_receive_size; i++) {
    logger->printf("%02x(%c) ", temp[i], (unsigned char)((temp[i] > 31 && temp[i] < 128) ? temp[i] : '.'));
  }
  logger->println("\r\n\r\n");

  while (true) {
    delay(1000);
  }
}

void preinit() {
  // (no C++ in function)
  // disable wifi
  ESP8266WiFiClass::preinitWiFiOff();
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(BAUD);
  Serial.swap(); // RX=GPIO13 TX=GPIO15
  Serial.setRxBufferSize(SERIAL_SIZE_RX);

  // using HardwareSerial0 pins,
  // so we can still log to the regular usbserial chips
  SoftwareSerial* ss = new SoftwareSerial(3, 1);
  ss->begin(SSBAUD);
  logger = ss;
  logger->println();
  logger->printf("\r\n\r\nOn Software Serial for logging\r\n");

  int baud = Serial.baudRate();
  logger->printf(ESP.getFullVersion().c_str());
  logger->printf("\r\n\r\nBAUD: %d - CoreRxBuffer: %d bytes - TestBuffer: %d bytes\r\n",
                 baud, SERIAL_SIZE_RX, BUFFER_SIZE);

  size_for_1sec = baud / 10; // 8n1=10baudFor8bits
  logger->printf("led changes state every %zd bytes (= 1 second)\r\n", size_for_1sec);

  // prepare send/compare buffer
  for (size_t i = 0; i < sizeof buf; i++) {
    buf[i] = (uint8_t)i;
  }

  // bind RX and TX
  USC0(0) |= (1 << UCLBE);

  while (Serial.read() == -1);
  if (Serial.hasOverrun()) {
    logger->print("overrun?\r\n");
  }

  timeout = (start_ms = last_ms = millis()) + TIMEOUT;
  logger->println("setup done");
}

void loop() {
  size_t maxlen = Serial.availableForWrite();
  // check remaining space in buffer
  if (maxlen > BUFFER_SIZE - out_idx) {
    maxlen = BUFFER_SIZE - out_idx;
  }
  // check if not cycling more than buffer size relatively to input
  size_t in_out = out_idx == in_idx ?
                  BUFFER_SIZE :
                  (in_idx + BUFFER_SIZE - out_idx - 1) % BUFFER_SIZE;
  if (maxlen > in_out) {
    maxlen = in_out;
  }
  DEBUG(logger->printf("(aw%i/w%i", Serial.availableForWrite(), maxlen));
  size_t local_written_size = Serial.write(buf + out_idx, maxlen);
  DEBUG(logger->printf(":w%i/aw%i/ar%i)\r\n", local_written_size, Serial.availableForWrite(), Serial.available()));
  if (local_written_size > maxlen) {
    error("bad write");
  }
  if ((out_idx += local_written_size) == BUFFER_SIZE) {
    out_idx = 0;
  }
  delay(0);

  DEBUG(logger->printf("----------\r\n"));

  if (Serial.hasOverrun()) {
    logger->printf("overrun!\r\n");
  }

  // receive data
  maxlen = Serial.available();
  if (maxlen > maxavail) {
    maxavail = maxlen;
  }
  // check space in temp receive buffer
  if (maxlen > BUFFER_SIZE - in_idx) {
    maxlen = BUFFER_SIZE - in_idx;
  }
  DEBUG(logger->printf("(ar%i/r%i", Serial.available(), maxlen));
  local_receive_size = Serial.readBytes(temp, maxlen);
  DEBUG(logger->printf(":r%i/ar%i)\r\n", local_receive_size, Serial.available()));
  if (local_receive_size > maxlen) {
    error("bad read");
  }
  if (local_receive_size) {
    if (memcmp(buf + in_idx, temp, local_receive_size) != 0) {
      error("fail");
    }
    if ((in_idx += local_receive_size) == BUFFER_SIZE) {
      in_idx = 0;
    }
    in_total += local_receive_size;
  }
  DEBUG(logger->printf("r(%d) ok\r\n", local_receive_size));

  // say something on data every second
  if ((size_for_led += local_written_size) >= size_for_1sec || millis() > timeout) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    size_for_led = 0;

    if (in_prev == in_total) {
      error("receiving nothing?\r\n");
    }

    unsigned long now_ms = millis();
    int bwkbps_avg = ((((uint64_t)in_total) * 8000) / (now_ms - start_ms)) >> 10;
    int bwkbps_now = (((in_total - in_prev) * 8000) / (now_ms - last_ms)) >> 10 ;
    logger->printf("bwavg=%d bwnow=%d kbps maxavail=%i\r\n", bwkbps_avg, bwkbps_now, maxavail);

    in_prev = in_total;
    timeout = (last_ms = now_ms) + TIMEOUT;
  }
}
