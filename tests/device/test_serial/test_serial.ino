#include <BSTest.h>
BS_ENV_DECLARE();

// this is the serialStress.ino example, stripped down

/*
  Serial read/write/verify/benchmark
  Using internal loopback

  Released to public domain
*/

#include <ESP8266WiFi.h>

#define SSBAUD          115200  // console for humans
#define BAUD            3000000 // hardware serial stress test
#define BUFFER_SIZE     4096    // may be useless to use more than 2*SERIAL_SIZE_RX
#define SERIAL_SIZE_RX  1024    // Serial.setRxBufferSize()

#define TIMEOUT 5000
#define DEBUG(x...) //x

uint8_t buf [BUFFER_SIZE];
uint8_t temp [BUFFER_SIZE];
bool reading = true;
bool overrun = false;

static size_t out_idx = 0, in_idx = 0;
static size_t local_receive_size = 0;
static size_t size_for_1sec, size_for_led = 0;
static size_t maxavail = 0;
static uint64_t in_total = 0, in_prev = 0;
static uint64_t start_ms, last_ms;
static uint64_t timeout;

void setup()
{
  Serial.begin(SSBAUD);

  int baud = BAUD;
  size_for_1sec = baud / 10; // 8n1=10baudFor8bits
  //Serial.printf(ESP.getFullVersion().c_str());
  //Serial.printf("\n\nBAUD: %d - CoreRxBuffer: %d bytes - TestBuffer: %d bytes\n",
  //               baud, SERIAL_SIZE_RX, BUFFER_SIZE);

  //Serial.printf("led changes state every %zd bytes (= 1 second)\n", size_for_1sec);
  //Serial.printf("press 's' to stop reading, not writing (induces overrun)\n");

  BS_RUN(Serial);
}

bool pretest()
{
    return true;
}

void test_setup()
{
  Serial.begin(BAUD);

  // bind RX and TX
  USC0(0) |= (1 << UCLBE);

  Serial.flush();
  while (Serial.read() != -1);
  timeout = (start_ms = last_ms = millis()) + TIMEOUT;
}

void test_setdown ()
{
  // unbind RX and TX
  Serial.flush();
  USC0(0) &= ~(1 << UCLBE);
  while (Serial.read() != -1);
  Serial.begin(SSBAUD);
}

int bwkbps_avg = 0;

bool test_loop ()
{
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
  size_t local_written_size = Serial.write(buf + out_idx, maxlen);
  if (local_written_size > maxlen) {
    return false;
  }
  if ((out_idx += local_written_size) == BUFFER_SIZE) {
    out_idx = 0;
  }
  delay(0);

  if (Serial.hasOverrun()) {
    overrun = true;
  }
  if (Serial.hasRxError()) {
  }

  if (reading)
  {
    // receive data
    maxlen = Serial.available();
    if (maxlen > maxavail) {
      maxavail = maxlen;
    }
    // check space in temp receive buffer
    if (maxlen > BUFFER_SIZE - in_idx) {
      maxlen = BUFFER_SIZE - in_idx;
    }
    local_receive_size = Serial.readBytes(temp, maxlen);
    if (local_receive_size > maxlen) {
      return false;
    }
    if (local_receive_size) {
      if (memcmp(buf + in_idx, temp, local_receive_size) != 0) {
        return false;
      }
      if ((in_idx += local_receive_size) == BUFFER_SIZE) {
        in_idx = 0;
      }
      in_total += local_receive_size;
    }
  }

  // say something on data every second
  if ((size_for_led += local_written_size) >= size_for_1sec || millis() > timeout) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    size_for_led = 0;

    if (in_prev == in_total) {
      return false;
    }

    unsigned long now_ms = millis();
    bwkbps_avg = ((((uint64_t)in_total) * 8000) / (now_ms - start_ms)) >> 10;
    //bwkbps_now = (((in_total - in_prev) * 8000) / (now_ms - last_ms)) >> 10 ;

    in_prev = in_total;
    timeout = (last_ms = now_ms) + TIMEOUT;
  }

  if (millis() > 5000)
  {
    reading = false;
  }
  if (millis() > 6000)
  {
    return false;
  }
  
  return true;
}

TEST_CASE("bandwidth and overrun", "[serial]")
{
  overrun = false;
  bwkbps_avg = 0;
  CHECK(overrun == false);
  CHECK(bwkbps_avg == 0);

  // let serial flush its BS output before flushing and switching to 3MBPS
  delay(100);
  
  test_setup();
  while (test_loop());
  test_setdown();

  Serial.printf("bandwidth = %d kbps  -  overrun=%d\n", bwkbps_avg, overrun);

  // BAUD*10/8/1000 =>kbps *9/10 => 90% at least
  CHECK(bwkbps_avg > ((((BAUD*8/10)/1000)*9)/10));
  CHECK(overrun == true);

  while (Serial.read() != -1);
  Serial.flush();
}

void loop ()
{
}
