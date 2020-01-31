/* This example demonstrates the different low-power modes of the ESP8266

  The initial setup was a WeMos D1 Mini with 3.3V connected to the 3V3 pin through a meter
  so that it bypassed the on-board voltage regulator and USB chip.  There's still about
  0.3 mA worth of leakage amperage due to the unpowered chips.  These tests should work with
  any module, although on-board components will affect the actual current measurement.
  While the modem is turned on the amperage is > 67 mA or changing with a minimum value.
  To verify the 20 uA Deep Sleep amperage the voltage regulator and USB chip were removed.

  This test series requires an active WiFi connection to illustrate two tests.  If you
  have problems with WiFi, uncomment the #define DEBUG for additional WiFi error messages.
  The test needs a pushbutton switch connected between D3/GPIO0 and GND to advance the tests.
  You'll also need to connect D0/GPIO16 to RST for the Deep Sleep tests.  If you forget to
  connect D0 to RST it will hang after the first Deep Sleep test. D0 is driven high during
  Deep Sleep, so you should use a Schottky diode between D0 and RST if you want to use a 
  reset switch; connect the anode of the diode to RST, and the cathode to D0.

  Additionally, you can connect an LED from any free pin through a 1K ohm resistor to the 
  3.3V supply, though preferably not the 3V3 pin on the module or it adds to the measured 
  amperage.  When the LED blinks you can proceed to the next test.  When the LED is lit 
  continuously it's connecting WiFi, and when it's off the CPU is asleep.  The LED blinks 
  slowly when the tests are complete.  Test progress can also be shown on the serial monitor.

  WiFi connections will be made over twice as fast if you use a static IP address.

  This example is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This example is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this example; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA  */

#include <ESP8266WiFi.h>
#include <coredecls.h>   // crc32()
#include <PolledTimeout.h>
#include <include/WiFiState.h>  // WiFiState structure details

//#define DEBUG  // prints WiFi connection info to serial, uncomment if you want WiFi messages
#ifdef DEBUG
#define DEBUG_PRINTLN(x)  Serial.println(x)
#define DEBUG_PRINT(x)  Serial.print(x)
#else
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINT(x)
#endif

#define WAKE_UP_PIN 0  // D3/GPIO0, can also force a serial flash upload with RESET
// you can use any GPIO for WAKE_UP_PIN except for D0/GPIO16 as it doesn't support interrupts

// uncomment one of the two lines below for your LED connection (optional)
#define LED 5  // D1/GPIO5 external LED for modules with built-in LEDs so it doesn't add amperage
//#define LED 2  // D4/GPIO2 LED for ESP-01,07 modules; D4 is LED_BUILTIN on most other modules
// you can use LED_BUILTIN, but it adds to the measured amperage by 0.3mA to 6mA.

ADC_MODE(ADC_VCC);  // allows you to monitor the internal VCC level; it varies with WiFi load
// don't connect anything to the analog input pin(s)!

// enter your WiFi configuration below
const char* AP_SSID = "SSID";  // your router's SSID here
const char* AP_PASS = "password";  // your router's password here
IPAddress staticIP(0, 0, 0, 0);  // parameters below are for your static IP address, if used
IPAddress gateway(0, 0, 0, 0);
IPAddress subnet(0, 0, 0, 0);
IPAddress dns1(0, 0, 0, 0);
IPAddress dns2(0, 0, 0, 0);
uint32_t wifiTimeout = 30E3;  // 30 second timeout on the WiFi connection

//#define TESTPOINT  // used to track the timing of several test cycles (optional)
#ifdef TESTPOINT
#define testPointPin 4  // D2/GPIO4, you can use any pin that supports interrupts
#define testPoint_HIGH digitalWrite(testPointPin, HIGH)
#define testPoint_LOW digitalWrite(testPointPin, LOW)
#else
#define testPoint_HIGH
#define testPoint_LOW
#endif

// This structure is stored in RTC memory to save the WiFi state and reset count (number of Deep Sleeps),
// and it reconnects twice as fast as the first connection; it's used extensively in this demo
struct nv_s {
  WiFiState wss;  // core's WiFi save state

  struct {
    uint32_t crc32;
    uint32_t rstCount;  // stores the Deep Sleep reset count
    // you can add anything else here that you want to save
  } rtcData;
};

static nv_s* nv = (nv_s*)RTC_USER_MEM;  // user RTC RAM area

uint32_t resetCount = 0;  // keeps track of the number of Deep Sleep tests / resets

const uint32_t blinkDelay = 100;  // fast blink rate for the LED when waiting for the user
esp8266::polledTimeout::periodicMs blinkLED(blinkDelay);  // LED blink delay without delay()
esp8266::polledTimeout::oneShotFastMs altDelay(blinkDelay);  // tight loop to simulate user code
// use fully qualified type and avoid importing all ::esp8266 namespace to the global namespace

void wakeupCallback() {  // unlike ISRs, you can do a print() from a callback function
  testPoint_LOW;  // testPoint tracks latency from WAKE_UP_PIN LOW to testPoint LOW
  printMillis();  // show time difference across sleep; millis is wrong as the CPU eventually stops
  Serial.println(F("Woke from Light Sleep - this is the callback"));
}

void setup() {
#ifdef TESTPOINT
  pinMode(testPointPin, OUTPUT);  // test point for Light Sleep and Deep Sleep tests
  testPoint_LOW;  // Deep Sleep reset doesn't clear GPIOs, testPoint LOW shows boot time
#endif
  pinMode(LED, OUTPUT);  // activity and status indicator
  digitalWrite(LED, LOW);  // turn on the LED
  pinMode(WAKE_UP_PIN, INPUT_PULLUP);  // polled to advance tests, interrupt for Forced Light Sleep
  Serial.begin(115200);
  Serial.println();
  Serial.print(F("\nReset reason = "));
  String resetCause = ESP.getResetReason();
  Serial.println(resetCause);
  resetCount = 0;
  if ((resetCause == "External System") || (resetCause == "Power on")) {
    Serial.println(F("I'm awake and starting the Low Power tests"));
  }

  // Read previous resets (Deep Sleeps) from RTC memory, if any
  uint32_t crcOfData = crc32((uint8_t*) &nv->rtcData.rstCount, sizeof(nv->rtcData.rstCount));
  if ((crcOfData = nv->rtcData.crc32) && (resetCause == "Deep-Sleep Wake")) {
    resetCount = nv->rtcData.rstCount;  // read the previous reset count
    resetCount++;
  }
  nv->rtcData.rstCount = resetCount;  // update the reset count & CRC
  updateRTCcrc();

  if (resetCount == 1) {  // show that millis() is cleared across the Deep Sleep reset
    printMillis();
  }
}  // end of setup()

void loop() {
  if (resetCount == 0) {  // if first loop() since power on or external reset
    runTest1();
    runTest2();
    runTest3();
    runTest4();
    runTest5();
    runTest6();
    runTest7();  // first Deep Sleep test, all these end with a RESET
  }
  if (resetCount < 4) {
    initWiFi();  // optional re-init of WiFi for the Deep Sleep tests
  }
  if (resetCount == 1) {
    runTest8();
  } else if (resetCount == 2) {
    runTest9();
  } else if (resetCount == 3) {
    runTest10();
  } else if (resetCount == 4) {
    resetTests();
  }
}  //end of loop()

void runTest1() {
  Serial.println(F("\n1st test - running with WiFi unconfigured"));
  readVoltage();  // read internal VCC
  Serial.println(F("press the switch to continue"));
  waitPushbutton(false, blinkDelay);
}

void runTest2() {
  Serial.println(F("\n2nd test - Automatic Modem Sleep"));
  Serial.println(F("connecting WiFi, please wait until the LED blinks"));
  initWiFi();
  if (WiFi.localIP()) {  // won't go into Automatic Sleep without an active WiFi connection
    Serial.println(F("The amperage will drop in 7 seconds."));
    readVoltage();  // read internal VCC
    Serial.println(F("press the switch to continue"));
    waitPushbutton(true, 90);  /* This is using a special feature: below 100 mS blink delay,
         the LED blink delay is padding 100 mS time with 'program cycles' to fill the 100 mS.
         At 90 mS delay, 90% of the blink time is delay(), and 10% is 'your program running'.
         Below 90% you'll see a difference in the average amperage: less delay() = more amperage.
         At 100 mS and above it's essentially all delay() time.  On an oscilloscope you'll see the
         time between beacons at > 67 mA more often with less delay() percentage. You can change
         the '90' mS to other values to see the effect it has on Automatic Modem Sleep. */
  } else {
    Serial.println(F("no WiFi connection, test skipped"));
  }
}

void runTest3() {
  Serial.println(F("\n3rd test - Forced Modem Sleep"));
  WiFi.mode(WIFI_SHUTDOWN, &nv->wss);  // shut the modem down and save the WiFi state for faster reconnection
  //  WiFi.forceSleepBegin(delay_in_uS);  // alternate method of Forced Modem Sleep for an optional timed shutdown,
  // with WiFi.forceSleepBegin(0xFFFFFFF); the modem sleeps until you wake it, with values <= 0xFFFFFFE it's timed
  //  delay(10);  // it doesn't always go to sleep unless you delay(10); yield() wasn't reliable
  readVoltage();  // read internal VCC
  Serial.println(F("press the switch to continue"));
  waitPushbutton(true, 99);  /* Using the same < 100 mS feature. If you drop the delay below 100, you
      will see the effect of program time vs. delay() time on minimum amperage.  Above ~ 97 (97% of the
      time in delay) there is little change in amperage, so you need to spend maximum time in delay()
      to get minimum amperage.*/
}

void runTest4() {
  Serial.println(F("\n4th test - Automatic Light Sleep"));
  Serial.println(F("reconnecting WiFi with forceSleepWake"));
  Serial.println(F("Automatic Light Sleep begins after WiFi connects (LED blinks)"));
  // on successive loops after power-on, WiFi shows 'connected' several seconds before Sleep happens
  // and WiFi reconnects after the forceSleepWake more quickly
  digitalWrite(LED, LOW);  // visual cue that we're reconnecting
  WiFi.setSleepMode(WIFI_LIGHT_SLEEP, 3);  // Automatic Light Sleep, DTIM listen interval = 3
  // at higher DTIM intervals you'll have a hard time establishing and maintaining a connection
  WiFi.forceSleepWake();  // reconnect with previous STA mode and connection settings
  uint32_t wifiStart = millis();
  while (((!WiFi.localIP()) || (WiFi.status() != WL_CONNECTED)) && (millis() - wifiStart < wifiTimeout)) {
    yield();
  }
  if (WiFi.localIP()) {  // won't go into Automatic Sleep without an active WiFi connection
    float reConn = (millis() - wifiStart);
    Serial.print(F("WiFi connect time = "));
    Serial.printf("%1.3f seconds\n", reConn / 1000);
    readVoltage();  // read internal VCC
    Serial.println(F("long press of the switch to continue"));
    waitPushbutton(true, 350);  /* Below 100 mS delay it only goes into 'Automatic Modem Sleep',
        and below ~ 350 mS delay() the 'Automatic Light Sleep' is less frequent.  Above 500 mS
        delay() doesn't make significant improvement in power savings. */
  } else {
    Serial.println(F("no WiFi connection, test skipped"));
  }
}

void runTest5() {
  Serial.println(F("\n5th test - Timed Light Sleep, wake in 10 seconds"));
  Serial.println(F("Press the button when you're ready to proceed"));
  waitPushbutton(true, blinkDelay);
  WiFi.mode(WIFI_OFF);  // you must turn the modem off; using disconnect won't work
  readVoltage();  // read internal VCC
  printMillis();  // show millis() across sleep, including Serial.flush()
  digitalWrite(LED, HIGH);  // turn the LED off so they know the CPU isn't running
  testPoint_HIGH;  // testPoint LOW in callback tracks delay from testPoint HIGH to LOW
  extern os_timer_t *timer_list;
  timer_list = nullptr;  // stop (but don't disable) the 4 OS timers
  wifi_fpm_set_sleep_type(LIGHT_SLEEP_T);
  gpio_pin_wakeup_enable(GPIO_ID_PIN(WAKE_UP_PIN), GPIO_PIN_INTR_LOLEVEL);  // GPIO wakeup (optional)
  // only LOLEVEL or HILEVEL interrupts work, no edge, that's an SDK or CPU limitation
  wifi_fpm_set_wakeup_cb(wakeupCallback); // set wakeup callback
  // the callback is optional, but without it the modem will wake in 10 seconds then delay(10 seconds)
  // with the callback the sleep time is only 10 seconds total, no extra delay() afterward
  wifi_fpm_open();
  wifi_fpm_do_sleep(10E6);  // Sleep range = 10000 ~ 268,435,454 uS (0xFFFFFFE, 2^28-1)
  delay(10e3 + 1);  // delay needs to be 1 mS longer than sleep or it only goes into Modem Sleep
  Serial.println(F("Woke up!"));  // the interrupt callback hits before this is executed
}

void runTest6() {
  Serial.println(F("\n6th test - Forced Light Sleep, wake with GPIO interrupt"));
  Serial.flush();
  WiFi.mode(WIFI_OFF);  // you must turn the modem off; using disconnect won't work
  digitalWrite(LED, HIGH);  // turn the LED off so they know the CPU isn't running
  readVoltage();  // read internal VCC
  Serial.println(F("CPU going to sleep, pull WAKE_UP_PIN low to wake it (press the switch)"));
  printMillis();  // show millis() across sleep, including Serial.flush()
  testPoint_HIGH;  // testPoint tracks latency from WAKE_UP_PIN LOW to testPoint LOW in callback
  wifi_fpm_set_sleep_type(LIGHT_SLEEP_T);
  gpio_pin_wakeup_enable(GPIO_ID_PIN(WAKE_UP_PIN), GPIO_PIN_INTR_LOLEVEL);
  // only LOLEVEL or HILEVEL interrupts work, no edge, that's an SDK or CPU limitation
  wifi_fpm_set_wakeup_cb(wakeupCallback); // Set wakeup callback (optional)
  wifi_fpm_open();
  wifi_fpm_do_sleep(0xFFFFFFF);  // only 0xFFFFFFF, any other value and it won't disconnect the RTC timer
  delay(10);  // it goes to sleep during this delay() and waits for an interrupt
  Serial.println(F("Woke up!"));  // the interrupt callback hits before this is executed*/
}

void runTest7() {
  Serial.println(F("\n7th test - Deep Sleep for 10 seconds, reset and wake with RF_DEFAULT"));
  initWiFi();  // initialize WiFi since we turned it off in the last test
  readVoltage();  // read internal VCC
  Serial.println(F("press the switch to continue"));
  while (!digitalRead(WAKE_UP_PIN)) {  // wait for them to release the switch from the previous test
    delay(10);
  }
  delay(50);  // debounce time for the switch, pushbutton released
  waitPushbutton(false, blinkDelay);  // set true if you want to see Automatic Modem Sleep
  digitalWrite(LED, LOW);  // turn the LED on, at least briefly
  //WiFi.mode(WIFI_SHUTDOWN, &nv->wss);  // Forced Modem Sleep for a more Instant Deep Sleep,
  // and no extended RFCAL as it goes into Deep Sleep
  Serial.println(F("going into Deep Sleep now..."));
  printMillis();  // show time difference across sleep
  testPoint_HIGH;  // testPoint set HIGH to track Deep Sleep period, cleared at startup()
  ESP.deepSleep(10E6, WAKE_RF_DEFAULT); // good night!  D0 fires a reset in 10 seconds...
  // if you do ESP.deepSleep(0, mode); it needs a RESET to come out of sleep (RTC is disconnected)
  // maximum timed Deep Sleep interval = 71.58 minutes with 0xFFFFFFFF
  // the 2 uA GPIO amperage during Deep Sleep can't drive the LED so it's not lit now, although
  // depending on the LED used, you might see it very dimly lit in a dark room during this test
  Serial.println(F("What... I'm not asleep?!?"));  // it will never get here
}

void runTest8() {
  Serial.println(F("\n8th test - in RF_DEFAULT, Deep Sleep for 10 seconds, reset and wake with RFCAL"));
  readVoltage();  // read internal VCC
  Serial.println(F("press the switch to continue"));
  waitPushbutton(false, blinkDelay);  // set true if you want to see Automatic Modem Sleep
  //WiFi.mode(WIFI_SHUTDOWN, &nv->wss);  // Forced Modem Sleep for a more Instant Deep Sleep,
  // and no extended RFCAL as it goes into Deep Sleep
  Serial.println(F("going into Deep Sleep now..."));
  Serial.flush();  // needs a delay(10) or Serial.flush() else it doesn't print the whole message
  testPoint_HIGH;  // testPoint set HIGH to track Deep Sleep period, cleared at startup()
  ESP.deepSleep(10E6, WAKE_RFCAL); // good night!  D0 fires a reset in 10 seconds...
  Serial.println(F("What... I'm not asleep?!?"));  // it will never get here
}

void runTest9() {
  Serial.println(F("\n9th test - in RFCAL, Deep Sleep Instant for 10 seconds, reset and wake with NO_RFCAL"));
  readVoltage();  // read internal VCC
  Serial.println(F("press the switch to continue"));
  waitPushbutton(false, blinkDelay);  // set true if you want to see Automatic Modem Sleep
  WiFi.mode(WIFI_SHUTDOWN, &nv->wss);  // Forced Modem Sleep for a more Instant Deep Sleep
  Serial.println(F("going into Deep Sleep now..."));
  Serial.flush();  // needs a delay(10) or Serial.flush() else it doesn't print the whole message
  testPoint_HIGH;  // testPoint set HIGH to track Deep Sleep period, cleared at startup()
  ESP.deepSleepInstant(10E6, WAKE_NO_RFCAL); // good night!  D0 fires a reset in 10 seconds...
  Serial.println(F("What... I'm not asleep?!?"));  // it will never get here
}

void runTest10() {
  Serial.println(F("\n10th test - in NO_RFCAL, Deep Sleep Instant for 10 seconds, reset and wake with RF_DISABLED"));
  readVoltage();  // read internal VCC
  Serial.println(F("press the switch to continue"));
  waitPushbutton(false, blinkDelay);  // set true if you want to see Automatic Modem Sleep
  //WiFi.mode(WIFI_SHUTDOWN);  // Forced Modem Sleep for a more Instant Deep Sleep
  Serial.println(F("going into Deep Sleep now..."));
  Serial.flush();  // needs a delay(10) or Serial.flush() else it doesn't print the whole message
  testPoint_HIGH;  // testPoint set HIGH to track Deep Sleep period, cleared at startup()
  ESP.deepSleepInstant(10E6, WAKE_RF_DISABLED); // good night!  D0 fires a reset in 10 seconds...
  Serial.println(F("What... I'm not asleep?!?"));  // it will never get here
}

void resetTests() {
  readVoltage();  // read internal VCC
  Serial.println(F("\nTests completed, in RF_DISABLED, press the switch to do an ESP.restart()"));
  memset(&nv->wss, 0, sizeof(nv->wss) * 2);  // wipe saved WiFi states, comment this if you want to keep them
  waitPushbutton(false, 1000);
  ESP.restart();
}

void waitPushbutton(bool usesDelay, unsigned int delayTime) {  // loop until they press the switch
  // note: 2 different modes, as 3 of the power saving modes need a delay() to activate fully
  if (!usesDelay) {  // quick interception of pushbutton press, no delay() used
    blinkLED.reset(delayTime);
    while (digitalRead(WAKE_UP_PIN)) {  // wait for a pushbutton press
      if (blinkLED) {
        digitalWrite(LED, !digitalRead(LED));  // toggle the activity LED
      }
      yield();  // this would be a good place for ArduinoOTA.handle();
    }
  } else {  // long delay() for the 3 modes that need it, but it misses quick switch presses
    while (digitalRead(WAKE_UP_PIN)) {  // wait for a pushbutton press
      digitalWrite(LED, !digitalRead(LED));  // toggle the activity LED
      delay(delayTime);  // another good place for ArduinoOTA.handle();
      if (delayTime < 100) {
        altDelay.reset(100 - delayTime);  // pad the time < 100 mS with some real CPU cycles
        while (!altDelay) {  // this simulates 'your program running', not delay() time
        }
      }
    }
  }
  delay(50);  // debounce time for the switch, pushbutton pressed
  while (!digitalRead(WAKE_UP_PIN)) {  // now wait for them to release the pushbutton
    delay(10);
  }
  delay(50);  // debounce time for the switch, pushbutton released
}

void readVoltage() { // read internal VCC
  float volts = ESP.getVcc();
  Serial.printf("The internal VCC reads %1.3f volts\n", volts / 1000);
}

void printMillis() {
  Serial.print(F("millis() = "));  // show that millis() isn't correct across most Sleep modes
  Serial.println(millis());
  Serial.flush();  // needs a Serial.flush() else it may not print the whole message before sleeping
}

void updateRTCcrc() {  // updates the reset count CRC
  nv->rtcData.crc32 = crc32((uint8_t*) &nv->rtcData.rstCount, sizeof(nv->rtcData.rstCount));
}

void initWiFi() {
  digitalWrite(LED, LOW);  // give a visual indication that we're alive but busy with WiFi
  uint32_t wifiBegin = millis();  // how long does it take to connect
  if ((crc32((uint8_t*) &nv->rtcData.rstCount + 1, sizeof(nv->wss)) && !WiFi.shutdownValidCRC(&nv->wss))) {
    // if good copy of wss, overwrite invalid (primary) copy
    memcpy((uint32_t*) &nv->wss, (uint32_t*) &nv->rtcData.rstCount + 1, sizeof(nv->wss));
  }
  if (WiFi.shutdownValidCRC(&nv->wss)) {  // if we have a valid WiFi saved state
    memcpy((uint32_t*) &nv->rtcData.rstCount + 1, (uint32_t*) &nv->wss, sizeof(nv->wss)); // save a copy of it
    Serial.println(F("resuming WiFi"));
  }
  if (!(WiFi.mode(WIFI_RESUME, &nv->wss))) {  // couldn't resume, or no valid saved WiFi state yet
    /* Explicitly set the ESP8266 as a WiFi-client (STAtion mode), otherwise by default it
      would try to act as both a client and an access-point and could cause network issues
      with other WiFi devices on your network. */
    WiFi.persistent(false);  // don't store the connection each time to save wear on the flash
    WiFi.mode(WIFI_STA);
    WiFi.setOutputPower(10);  // reduce RF output power, increase if it won't connect
    WiFi.config(staticIP, gateway, subnet);  // if using static IP, enter parameters at the top
    WiFi.begin(AP_SSID, AP_PASS);
    Serial.print(F("connecting to WiFi "));
    Serial.println(AP_SSID);
    DEBUG_PRINT(F("my MAC: "));
    DEBUG_PRINTLN(WiFi.macAddress());
  }
  uint32_t wifiStart = millis();
  while ((WiFi.status() != WL_CONNECTED) && (millis() - wifiStart < wifiTimeout)) {
    yield();
  }
  if (WiFi.status() == WL_CONNECTED) {
    DEBUG_PRINTLN(F("WiFi connected"));
  } else {
    Serial.println(F("WiFi timed out and didn't connect"));
  }
  uint32_t wifiFinish = millis();  // timeout if we don't get the IP addresses from DHCP
  while ((!WiFi.localIP() && (WiFi.status() == WL_CONNECTED)) && (millis() - wifiFinish < wifiTimeout)) {
    yield();
  }
  WiFi.setAutoReconnect(true);
  if (WiFi.localIP()) {
    Serial.print(F("WiFi connect time = "));
    float reConn = (millis() - wifiBegin);
    Serial.printf("%1.3f seconds\n", reConn / 1000);
    DEBUG_PRINT(F("WiFi Gateway IP: "));
    DEBUG_PRINTLN(WiFi.gatewayIP());
    DEBUG_PRINT(F("my IP address: "));
    DEBUG_PRINTLN(WiFi.localIP());
  } else {
    DEBUG_PRINTLN(F("IP addresses not acquired from DHCP"));
  }
}
