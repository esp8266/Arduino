
/*
  Serial read/write/verify/benchmark
  Using Serial0 for internal loopback
  Using Serial1 for logging

  Sketch meant for debugging only
  Released to public domain
*/

#include <ESP8266WiFi.h>

#define LOGBAUD 115200       // logger on console for humans
#define BAUD 3000000         // hardware serial stress test
#define BUFFER_SIZE 4096     // may be useless to use more than 2*SERIAL_SIZE_RX
#define SERIAL_SIZE_RX 1024  // Serial.setRxBufferSize()

#define FAKE_INCREASED_AVAILABLE 100  // test readBytes's timeout

#define TIMEOUT 5000
#define DEBUG(x...)  // x

#define READING_PIN 4
#define TIMEOUT_PIN 5

uint8_t buf[BUFFER_SIZE];
uint8_t temp[BUFFER_SIZE];
bool reading = true;
size_t testReadBytesTimeout = 0;

static size_t out_idx = 0, in_idx = 0;
static size_t local_receive_size = 0;
static size_t size_for_1sec, size_for_led = 0;
static size_t maxavail = 0;
static uint64_t in_total = 0, in_prev = 0;
static uint64_t start_ms, last_ms;
static uint64_t timeout;

Stream* logger;

void error(const char* what) {
  logger->printf("\nerror: %s after %ld minutes\nread idx:  %d\nwrite idx: %d\ntotal:     %ld\nlast read: %d\nmaxavail:  %d\n", what, (long)((millis() - start_ms) / 60000), in_idx, out_idx, (long)in_total, (int)local_receive_size, maxavail);
  if (Serial.hasOverrun()) { logger->printf("overrun!\n"); }
  logger->printf("should be (size=%d idx=%d..%d):\n    ", BUFFER_SIZE, in_idx, in_idx + local_receive_size - 1);
  for (size_t i = in_idx; i < in_idx + local_receive_size; i++) { logger->printf("%02x(%c) ", buf[i], (unsigned char)((buf[i] > 31 && buf[i] < 128) ? buf[i] : '.')); }
  logger->print("\n\nis: ");
  for (size_t i = 0; i < local_receive_size; i++) { logger->printf("%02x(%c) ", temp[i], (unsigned char)((temp[i] > 31 && temp[i] < 128) ? temp[i] : '.')); }
  logger->println("\n\n");

  while (true) { delay(1000); }
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(READING_PIN, INPUT);
  pinMode(TIMEOUT_PIN, INPUT);

  Serial.begin(BAUD);
  Serial.swap();  // RX=GPIO13 TX=GPIO15
  Serial.setRxBufferSize(SERIAL_SIZE_RX);

  Serial1.begin(LOGBAUD);  // RX=NONE TX=GPIO2
  logger = &Serial1;

  logger->println();
  logger->printf("\n\nOn Serial1 for logging\n");

  int baud = Serial.baudRate();
  logger->printf(ESP.getFullVersion().c_str());
  logger->printf("\n\nBAUD: %d - CoreRxBuffer: %d bytes - TestBuffer: %d bytes\n", baud, SERIAL_SIZE_RX, BUFFER_SIZE);

  size_for_1sec = baud / 10;  // 8n1=10baudFor8bits
  logger->printf("led changes state every %zd bytes (= 1 second)\n", size_for_1sec);
  logger->printf("press 's' to stop reading, not writing (induces overrun)\n");
  logger->printf("press 't' to toggle timeout testing on readBytes\n");

  // prepare send/compare buffer
  for (size_t i = 0; i < sizeof buf; i++) { buf[i] = (uint8_t)i; }

  // bind RX and TX
  USC0(0) |= (1 << UCLBE);

  while (Serial.read() == -1);
  if (Serial.hasOverrun()) { logger->print("overrun?\n"); }

  timeout = (start_ms = last_ms = millis()) + TIMEOUT;
  logger->println("setup done");
}

void loop() {
  size_t maxlen = Serial.availableForWrite();
  // check remaining space in buffer
  if (maxlen > BUFFER_SIZE - out_idx) { maxlen = BUFFER_SIZE - out_idx; }
  // check if not cycling more than buffer size relatively to input
  size_t in_out = out_idx == in_idx ? BUFFER_SIZE : (in_idx + BUFFER_SIZE - out_idx - 1) % BUFFER_SIZE;
  if (maxlen > in_out) { maxlen = in_out; }
  DEBUG(logger->printf("(aw%i/w%i", Serial.availableForWrite(), maxlen));
  size_t local_written_size = Serial.write(buf + out_idx, maxlen);
  DEBUG(logger->printf(":w%i/aw%i/ar%i)\n", local_written_size, Serial.availableForWrite(), Serial.available()));
  if (local_written_size > maxlen) { error("bad write"); }
  if ((out_idx += local_written_size) == BUFFER_SIZE) { out_idx = 0; }
  yield();

  DEBUG(logger->printf("----------\n"));

  if (Serial.hasOverrun()) { logger->printf("rx overrun!\n"); }
  if (Serial.hasRxError()) { logger->printf("rx error!\n"); }

  if (reading) {
    // receive data
    maxlen = Serial.available() + testReadBytesTimeout;
    if (maxlen > maxavail) { maxavail = maxlen; }
    // check space in temp receive buffer
    if (maxlen > BUFFER_SIZE - in_idx) { maxlen = BUFFER_SIZE - in_idx; }
    DEBUG(logger->printf("(ar%i/r%i", Serial.available(), maxlen));
    local_receive_size = Serial.readBytes(temp, maxlen);
    DEBUG(logger->printf(":r%i/ar%i)\n", local_receive_size, Serial.available()));
    if (local_receive_size > maxlen) { error("bad read"); }
    if (local_receive_size) {
      if (memcmp(buf + in_idx, temp, local_receive_size) != 0) { error("fail"); }
      if ((in_idx += local_receive_size) == BUFFER_SIZE) { in_idx = 0; }
      in_total += local_receive_size;
    }
    DEBUG(logger->printf("r(%d) ok\n", local_receive_size));
  }

  // say something on data every second
  if ((size_for_led += local_written_size) >= size_for_1sec || millis() > timeout) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    size_for_led = 0;

    if (in_prev == in_total) { error("receiving nothing?\n"); }

    unsigned long now_ms = millis();
    int bwkbps_avg = ((((uint64_t)in_total) * 8000) / (now_ms - start_ms)) >> 10;
    int bwkbps_now = (((in_total - in_prev) * 8000) / (now_ms - last_ms)) >> 10;
    logger->printf("bwavg=%d bwnow=%d kbps maxavail=%i\n", bwkbps_avg, bwkbps_now, maxavail);

    in_prev = in_total;
    timeout = (last_ms = now_ms) + TIMEOUT;
  }

  if (reading && (digitalRead(READING_PIN) == 0)) {
    logger->println("now stopping reading, keeping writing");
    reading = false;
  }

  if (digitalRead(TIMEOUT_PIN) == 0) {
    testReadBytesTimeout ^= FAKE_INCREASED_AVAILABLE;
    logger->printf("testing readBytes timeout: %d\n", !!testReadBytesTimeout);
  }
}
