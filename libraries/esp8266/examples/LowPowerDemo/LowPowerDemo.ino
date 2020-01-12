/* This example demonstrates the different low-power modes of the ESP8266

   The initial setup was a WeMos D1 Mini with 3.3V connected to the 3V3 pin through a meter
   so that it bypassed the on-board voltage regulator and USB chip.  There's still about
   0.3 mA worth of leakage current due to the unpowered chips.  These tests should work with
   any module, although on-board components will affect the actual current measurement.
   While the modem is turned on the current is > 67 mA or jumping around with a minimum value.
   To verify the 20 uA Deep Sleep current the voltage regulator and USB chip were removed.

   This test series requires an active WiFi connection to illustrate two tests.  If you
   have problems with WiFi, uncomment the #ifdef DEBUG for additional WiFi error messages.
   The test requires a pushbutton switch connected between D3 and GND to advance the tests.
   You'll also need to connect D0/GPIO16 to RST for the Deep Sleep tests.  If you forget to
   connect D0 to RST it will hang after the first Deep Sleep test.  Additionally, you can
   connect an LED from any free pin through a 1K ohm resistor to the 3.3V supply, though
   preferably not the 3V3 pin on the module or it adds to the measured current.  When the
   LED blinks you can proceed to the next test.  When the LED is lit continuously it's
   connecting WiFi, and when it's off the CPU is asleep.  The LED blinks slowly when the
   tests are complete.  Test progress is also shown on the serial monitor.

   WiFi connections will be made over twice as fast if you can use a static IP address.

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
#include <coredecls.h>         // crc32()
#include <PolledTimeout.h>

//#define DEBUG  // prints WiFi connection info to serial, uncomment if you want WiFi messages
#ifdef DEBUG
#define DEBUG_PRINTLN(x)  Serial.println(x)
#define DEBUG_PRINT(x)  Serial.print(x)
#else
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINT(x)
#endif

#define WAKE_UP_PIN 0  // D3/GPIO0, can also force a serial flash upload with RESET
// you can use any pin for WAKE_UP_PIN except for D0/GPIO16 as it doesn't support interrupts

// uncomment one of the two lines below for your LED connection, if used
#define LED 5  // D1/GPIO5 external LED for modules with built-in LEDs so it doesn't add to the current
//#define LED 2  // D4/GPIO2 LED for ESP-01,07 modules; D4 is LED_BUILTIN on most other modules
// you can use LED_BUILTIN, but it adds to the measured current by 0.3mA to 6mA.

ADC_MODE(ADC_VCC);  // allows you to monitor the internal VCC level; it varies with WiFi load
// don't connect anything to the analog input pin(s)!

// enter your WiFi configuration below
const char* AP_SSID = "SSID";  // your router's SSID here
const char* AP_PASS = "password";  // your router's password here
IPAddress staticIP(0, 0, 0, 0); // parameters below are for your static IP address, if used
IPAddress gateway(0, 0, 0, 0);
IPAddress subnet(0, 0, 0, 0);
IPAddress dns1(0, 0, 0, 0);
IPAddress dns2(0, 0, 0, 0);
uint32_t wifiTimeout = 10E3;  // 10 second timeout on the WiFi connection

// This structure will be stored in RTC memory to remember the reset count (number of Deep Sleeps).
// First field is CRC32, which is calculated based on the rest of the structure contents.
// Any fields can go after the CRC32.  The structure must be 4-byte aligned.
struct {
  uint32_t crc32;
  byte data[4];  // the last byte stores the reset count
} rtcData;

byte resetCount = 0;  // keeps track of the number of Deep Sleep tests / resets
String resetCause = "";

const uint32_t blinkDelay = 100; // fast blink rate for the LED when waiting for the user
const uint32_t longDelay = 350;  // longer delay() for the two AUTOMATIC modes
esp8266::polledTimeout::periodicFastMs blinkLED(blinkDelay);
// use fully qualified type and avoid importing all ::esp8266 namespace to the global namespace

void wakeupCallback() {  // unlike ISRs, you can do a print() from a callback function
  Serial.println(F("Woke from Forced Light Sleep - this is the callback"));
}

void setup() {
  pinMode(LED, OUTPUT);  // Activity and Status indicator
  digitalWrite(LED, LOW);  // turn on the LED
  pinMode(WAKE_UP_PIN, INPUT_PULLUP);  // polled to advance tests, INTR for Forced Light Sleep
  Serial.begin(115200);
  Serial.print(F("\nReset reason = "));
  String resetCause = ESP.getResetReason();
  Serial.println(resetCause);
  if (resetCause == "External System") {
    Serial.println(F("I'm awake and starting the low power tests"));
    resetCount = 5;
    updateRTC();  // if external reset, wipe the RTC memory and start all over
  }

  // Read struct from RTC memory
  if (ESP.rtcUserMemoryRead(64, (uint32_t*) &rtcData, sizeof(rtcData))) {
    uint32_t crcOfData = crc32((uint8_t*) &rtcData.data[0], sizeof(rtcData.data));
    if (crcOfData != rtcData.crc32) {  // if the CRC is invalid
      resetCount = 0;  // set first test loop since power on or external reset
    } else {
      resetCount = rtcData.data[3];  // read the previous reset count
    }
  }
}  // end of setup()

void loop() {
  if (resetCount == 0) {
    // 1st test - running with WiFi unconfigured, reads ~67 mA minimum
    Serial.println(F("\n1st test - running with WiFi unconfigured"));
    float volts = ESP.getVcc();
    Serial.printf("The internal VCC reads %1.3f volts\n", volts / 1000);
    Serial.println(F("press the button to continue"));
    waitPushbutton(false, blinkDelay);

    // 2nd test - Automatic Modem Sleep 7 seconds after WiFi is connected (LED flashes)
    Serial.println(F("\n2nd test - Automatic Modem Sleep"));
    Serial.println(F("connecting WiFi, please wait until the LED blinks"));
    init_WiFi();
    if (WiFi.localIP()) {  // won't go into Automatic Sleep without an active WiFi connection
      Serial.println(F("The current will drop in 7 seconds."));
      volts = ESP.getVcc();
      Serial.printf("The internal VCC reads %1.3f volts\n", volts / 1000);
      Serial.println(F("press the button to continue"));
      waitPushbutton(true, longDelay);
    } else {
      Serial.println(F("no WiFi connection, test skipped"));
    }

    // 3rd test - Forced Modem Sleep
    Serial.println(F("\n3rd test - Forced Modem Sleep"));
    WiFi.forceSleepBegin();
    delay(10);  // it doesn't always go to sleep unless you delay(10); yield() wasn't reliable
    volts = ESP.getVcc();
    Serial.printf("The internal VCC reads %1.3f volts\n", volts / 1000);
    Serial.println(F("press the button to continue"));
    waitPushbutton(true, longDelay);

    // 4th test - Automatic Light Sleep
    Serial.println(F("\n4th test - Automatic Light Sleep"));
    Serial.println(F("reconnecting WiFi"));
    Serial.println(F("it will be in Automatic Light Sleep once WiFi connects (LED blinks)"));
    digitalWrite(LED, LOW);  // visual cue that we're reconnecting
    WiFi.setSleepMode(WIFI_LIGHT_SLEEP, 5);  // Automatic Light Sleep
    WiFi.forceSleepWake();  // reconnect with previous STA mode and connection settings
    uint32_t wifiStart = millis();
    while ((!WiFi.localIP()) && (millis() - wifiStart < wifiTimeout)) {
      delay(50);
    }
    if (WiFi.localIP()) {  // won't go into Automatic Sleep without an active WiFi connection
      volts = ESP.getVcc();
      Serial.printf("The internal VCC reads %1.3f volts\n", volts / 1000);
      Serial.println(F("press the button to continue"));
      waitPushbutton(true, longDelay);
    } else {
      Serial.println(F("no WiFi connection, test skipped"));
    }

    // 5th test - Forced Light Sleep using Non-OS SDK calls
    Serial.println(F("\n5th test - Forced Light Sleep using Non-OS SDK calls"));
    WiFi.mode(WIFI_OFF);  // you must turn the modem off; using disconnect won't work
    yield();
    digitalWrite(LED, HIGH);  // turn the LED off so they know the CPU isn't running
    volts = ESP.getVcc();
    Serial.printf("The internal VCC reads %1.3f volts\n", volts / 1000);
    Serial.println(F("CPU going to sleep, pull WAKE_UP_PIN low to wake it (press the button)"));
    delay(100);  // needs a delay() after the print or it may not print the whole message
    wifi_fpm_set_sleep_type(LIGHT_SLEEP_T);
    gpio_pin_wakeup_enable(GPIO_ID_PIN(WAKE_UP_PIN), GPIO_PIN_INTR_LOLEVEL);
    // only LOLEVEL or HILEVEL interrupts work, no edge, that's an SDK or CPU limitation
    wifi_fpm_set_wakeup_cb(wakeupCallback); // Set wakeup callback (optional)
    wifi_fpm_open();
    wifi_fpm_do_sleep(0xFFFFFFF);  // only 0xFFFFFFF works; any other value and it won't sleep
    delay(10);  // it goes to sleep some time during this delay() and waits for an interrupt
    Serial.println(F("Woke up!"));  // the interrupt callback hits before this is executed

    // 6th test - Deep Sleep for 10 seconds, wake with RF_DEFAULT
    Serial.println(F("\n6th test - Deep Sleep for 10 seconds, reset and wake with RF_DEFAULT"));
    init_WiFi();  // initialize WiFi since we turned it off in the last test
    resetCount = 1;  // advance to the next Deep Sleep test after the reset
    updateRTC();  // save the current test state in RTC memory
    volts = ESP.getVcc();
    Serial.printf("The internal VCC reads %1.3f volts\n", volts / 1000);
    Serial.println(F("press the button to continue"));
    while (!digitalRead(WAKE_UP_PIN)) {  // wait for them to release the button from the last test
      delay(10);
    }
    delay(50);  // debounce time for the switch, button released
    waitPushbutton(false, blinkDelay);
    digitalWrite(LED, LOW);  // turn the LED on, at least briefly
    Serial.println(F("going into Deep Sleep now..."));
    delay(10);  // sometimes the \n isn't printed without a short delay
    ESP.deepSleep(10E6, WAKE_RF_DEFAULT); // good night!  D0 fires a reset in 10 seconds...
    delay(10);
    // if you do ESP.deepSleep(0, mode); it needs a RESET to come out of sleep (RTC is off)
    // maximum timed Deep Sleep interval = 71.58 minutes with 0xFFFFFFFF
    // the 2 uA GPIO current during Deep Sleep can't drive the LED so it's not lit now, although
    // depending on the LED used, you might see it very dimly lit in a dark room during this test
    Serial.println(F("What... I'm not asleep?!?"));  // it will never get here
  }

  // 7th test - Deep Sleep for 10 seconds, wake with RFCAL
  if (resetCount < 4) {
    init_WiFi();  // need to reinitialize WiFi due to Deep Sleep reset
  }
  if (resetCount == 1) {  // second reset loop since power on
    resetCount = 2;  // advance to the next Deep Sleep test after the reset
    updateRTC();  // save the current test state in RTC memory
    Serial.println(F("\n7th test - in RF_DEFAULT, Deep Sleep for 10 seconds, reset and wake with RFCAL"));
    float volts = ESP.getVcc();
    Serial.printf("The internal VCC reads %1.3f volts\n", volts / 1000);
    Serial.println(F("press the button to continue"));
    waitPushbutton(false, blinkDelay);
    Serial.println(F("going into Deep Sleep now..."));
    delay(10);  // sometimes the \n isn't printed without a short delay
    ESP.deepSleep(10E6, WAKE_RFCAL); // good night!  D0 fires a reset in 10 seconds...
    delay(10);
    Serial.println(F("What... I'm not asleep?!?"));  // it will never get here
  }

  // 8th test - Deep Sleep Instant for 10 seconds, wake with NO_RFCAL
  if (resetCount == 2) {  // third reset loop since power on
    resetCount = 3;  // advance to the next Deep Sleep test after the reset
    updateRTC();  // save the current test state in RTC memory
    Serial.println(F("\n8th test - in RFCAL, Deep Sleep Instant for 10 seconds, reset and wake with NO_RFCAL"));
    float volts = ESP.getVcc();
    Serial.printf("The internal VCC reads %1.3f volts\n", volts / 1000);
    Serial.println(F("press the button to continue"));
    waitPushbutton(false, blinkDelay);
    Serial.println(F("going into Deep Sleep now..."));
    delay(10);  // sometimes the \n isn't printed without a short delay
    ESP.deepSleepInstant(10E6, WAKE_NO_RFCAL); // good night!  D0 fires a reset in 10 seconds...
    delay(10);
    Serial.println(F("What... I'm not asleep?!?"));  // it will never get here
  }

  // 9th test - Deep Sleep Instant for 10 seconds, wake with RF_DISABLED
  if (resetCount == 3) {  // fourth reset loop since power on
    resetCount = 4;  // advance to the next Deep Sleep test after the reset
    updateRTC();  // save the current test state in RTC memory
    Serial.println(F("\n9th test - in NO_RFCAL, Deep Sleep Instant for 10 seconds, reset and wake with RF_DISABLED"));
    float volts = ESP.getVcc();
    Serial.printf("The internal VCC reads %1.3f volts\n", volts / 1000);
    Serial.println(F("press the button to continue"));
    waitPushbutton(false, blinkDelay);
    Serial.println(F("going into Deep Sleep now..."));
    delay(10);  // sometimes the \n isn't printed without a short delay
    ESP.deepSleepInstant(10E6, WAKE_RF_DISABLED); // good night!  D0 fires a reset in 10 seconds...
    delay(10);
    Serial.println(F("What... I'm not asleep?!?"));  // it will never get here
  }

  if (resetCount == 4) {
    resetCount = 5;  // start all over, do an ESP.restart to insure it's a clean state
    updateRTC();  // save the current test state in RTC memory
    float volts = ESP.getVcc();
    Serial.printf("The internal VCC reads %1.3f volts\n", volts / 1000);
    Serial.println(F("\nTests completed, in RF_DISABLED, press the button to do an ESP.restart()"));
    waitPushbutton(false, 1000);
    ESP.restart();
  }
}

void waitPushbutton(bool usesDelay, unsigned int delayTime) {  // loop until they press the button
  // note: 2 different modes, as both of the AUTOMATIC power saving modes need a long delay()
  if (!usesDelay) {  // quick interception of button press, no delay() used
    blinkLED.reset(delayTime);
    while (digitalRead(WAKE_UP_PIN)) {  // wait for a button press
      if (blinkLED) {
        digitalWrite(LED, !digitalRead(LED));  // toggle the activity LED
      }
      yield();  // this would be a good spot for ArduinoOTA.handle();
    }
  } else {  // long delay() for the 2 AUTOMATIC modes, but it misses quick button presses
    while (digitalRead(WAKE_UP_PIN)) {  // wait for a button press
      digitalWrite(LED, !digitalRead(LED));  // toggle the activity LED
      delay(delayTime);  // another good spot for ArduinoOTA.handle();
    }
  }
  delay(50);  // debounce time for the switch, button pressed
  while (!digitalRead(WAKE_UP_PIN)) {  // now wait for them to release the button
    delay(10);
  }
  delay(50);  // debounce time for the switch, button released
}

void updateRTC() {
  rtcData.data[3] = resetCount;  // save the reset count for the next test run
  // Update CRC32 of data
  rtcData.crc32 = crc32((uint8_t*) &rtcData.data[0], sizeof(rtcData.data));
  if (resetCount == 5) {  // wipe the CRC in RTC memory when we're done with all tests
    rtcData.crc32 = 0;
  }
  // Write struct to RTC memory
  ESP.rtcUserMemoryWrite(64, (uint32_t*) &rtcData, sizeof(rtcData));
}

void init_WiFi() {
  /* Explicitly set the ESP8266 as a WiFi-client (STAtion mode), otherwise by default it
    would try to act as both a client and an access-point and could cause network issues
    with other WiFi devices on your network. */
  digitalWrite(LED, LOW);  // give a visual indication that we're alive but busy with WiFi
  WiFi.persistent(false);  // don't store the connection each time to save wear on the flash
  WiFi.mode(WIFI_STA);
  WiFi.config(staticIP, gateway, subnet);  // if using static IP, enter parameters at the top
  WiFi.begin(AP_SSID, AP_PASS);
  Serial.print(F("connecting to WiFi "));
  Serial.println(AP_SSID);
  DEBUG_PRINT(F("my MAC: "));
  DEBUG_PRINTLN(WiFi.macAddress());
  uint32_t wifiStart = millis();
  while ((WiFi.status() != WL_CONNECTED) && (millis() - wifiStart < wifiTimeout)) {
    delay(50);
  }
  if (WiFi.status() == WL_CONNECTED) {
    DEBUG_PRINTLN(F("WiFi connected"));
  } else {
    Serial.println(F("WiFi timed out and didn't connect"));
  }
  while (!WiFi.localIP() && (WiFi.status() == WL_CONNECTED)) {
    delay(50);
  }
  WiFi.setAutoReconnect(true);
  DEBUG_PRINT(F("WiFi Gateway IP: "));
  DEBUG_PRINTLN(WiFi.gatewayIP());
  DEBUG_PRINT(F("my IP address: "));
  DEBUG_PRINTLN(WiFi.localIP());
}
