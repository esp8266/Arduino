/* This example demonstrates the different low-power modes of the ESP8266

   My initial setup was a WeMos D1 Mini with 3.3V connected to the 3V3 pin through a meter
   so that it bypassed the on-board voltage regulator and USB chip.  There's still about
   0.3 mA worth of leakage current due to the unpowered chips, so an ESP-01 will show lower
   current readings than what I could achieve.  These tests should work with any module.
   While the modem is on the current is 67 mA or jumping around with a listed minimum.
   To verify the 20 uA Deep Sleep current I removed the voltage regulator and USB chip.

   Since I'm now missing the USB chip, I've included OTA upload.  You'll need to upload
   from USB or a USB-to-TTL converter the first time, then you can disconnect and use OTA
   afterwards during any test if the WiFi is connected.  Some tests disconnect or sleep WiFi
   so OTA won't go through.  If you want OTA upload, hit RESET & press the test button once.

   This test assumes you have a pushbutton switch connected between D3 and GND to advance
   the tests.  You'll also need to connect D0/GPIO16 to RST for the Deep Sleep tests.
   If you forget to connect D0 to RST it will hang after the first Deep Sleep test.
   Connect an LED from any free pin through a 330 ohm resistor to the 3.3V supply, NOT the 3V3
   pin on the module or it adds to the measured current.  When it blinks you can proceed.
   When the LED is lit continuously it's connecting WiFi, when it's off the CPU is asleep.
   The LED blinks slowly when the tests are complete.

   WiFi connections will be made over twice as fast if you can use a static IP address.

   This example code is in the public domain, and was inspired by code from numerous sources */

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
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

// un-comment one of the two lines below for your LED connection
#define LED 5  // D1/GPIO5 external LED for modules with built-in LEDs so it doesn't add to the current
//#define LED 2  // D4/GPIO2 LED for ESP-01,07 modules; D4 is LED_BUILTIN on most other modules

ADC_MODE(ADC_VCC);  // allows us to monitor the internal VCC level; it varies with WiFi load
// don't connect anything to the analog input pin(s)!

// enter your WiFi configuration below
const char* AP_SSID = "SSID";  // your router's SSID here
const char* AP_PASS = "password";  // your router's password here
IPAddress staticIP(0, 0, 0, 0); // parameters below are for your static IP address, if used
IPAddress gateway(0, 0, 0, 0);
IPAddress subnet(0, 0, 0, 0);
IPAddress dns1(0, 0, 0, 0);
IPAddress dns2(0, 0, 0, 0);


// CRC function used to ensure data validity of RTC User Memory
uint32_t calculateCRC32(const uint8_t *data, size_t length);

// This structure will be stored in RTC memory to remember the reset loop count.
// First field is CRC32, which is calculated based on the rest of the structure contents.
// Any fields can go after the CRC32.  The structure must be 4-byte aligned.
struct {
  uint32_t crc32;
  byte data[4];  // the last byte stores the reset count
} rtcData;

byte resetCount = 0;  // keeps track of the number of Deep Sleep tests / resets
String resetCause = "";

const unsigned int blinkDelay = 100; // fast blink rate for the LED when waiting for the user
const unsigned int longDelay = 350;  // longer delay() for the two AUTOMATIC modes
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
    uint32_t crcOfData = calculateCRC32((uint8_t*) &rtcData.data[0], sizeof(rtcData.data));
    if (crcOfData != rtcData.crc32) {  // if the CRC is invalid
      resetCount = 0;  // set first test loop since power on or external reset
    } else {
      resetCount = rtcData.data[3];  // read the previous reset count
    }
  }
}  // end of Setup()

void loop() {
  if (resetCount == 0) {
    // 1st test - running with WiFi unconfigured, reads ~67 mA minimum
    Serial.println(F("\n1st test - running with WiFi unconfigured"));
    float volts = ESP.getVcc();
    Serial.printf("The internal VCC reads %1.3f volts\n", volts / 1000 );
    Serial.println(F("press the button to continue"));
    waitPushbutton(false, blinkDelay);

    // 2nd test - Automatic Modem Sleep 7 seconds after WiFi is connected (LED flashes)
    Serial.println(F("\n2nd test - Automatic Modem Sleep"));
    Serial.println(F("connecting WiFi, please wait until the LED blinks"));
    init_WiFi();
    init_OTA();
    Serial.println(F("The current will drop in 7 seconds."));
    volts = ESP.getVcc();
    Serial.printf("The internal VCC reads %1.3f volts\n", volts / 1000 );
    Serial.println(F("press the button to continue"));
    waitPushbutton(true, longDelay);

    // 3rd test - Forced Modem Sleep
    Serial.println(F("\n3rd test - Forced Modem Sleep"));
    WiFi.forceSleepBegin();
    delay(10);  // it doesn't always go to sleep unless you delay(10)
    volts = ESP.getVcc();
    Serial.printf("The internal VCC reads %1.3f volts\n", volts / 1000 );
    Serial.println(F("press the button to continue"));
    waitPushbutton(false, blinkDelay);

    // 4th test - Automatic Light Sleep
    Serial.println(F("\n4th test - Automatic Light Sleep"));
    Serial.println(F("reconnecting WiFi"));
    Serial.println(F("it will be in Automatic Light Sleep once WiFi connects (LED blinks)"));
    digitalWrite(LED, LOW);  // visual cue that we're reconnecting
    WiFi.setSleepMode(WIFI_LIGHT_SLEEP, 5);  // Automatic Light Sleep
    WiFi.forceSleepWake();  // reconnect with previous STA mode and connection settings
    while (!WiFi.localIP())
      delay(50);
    volts = ESP.getVcc();
    Serial.printf("The internal VCC reads %1.3f volts\n", volts / 1000 );
    Serial.println(F("press the button to continue"));
    waitPushbutton(true, longDelay);

    // 5th test - Forced Light Sleep using Non-OS SDK calls
    Serial.println(F("\n5th test - Forced Light Sleep using Non-OS SDK calls"));
    WiFi.mode(WIFI_OFF);  // you must turn the modem off; using disconnect won't work
    yield();
    digitalWrite(LED, HIGH);  // turn the LED off so they know the CPU isn't running
    volts = ESP.getVcc();
    Serial.printf("The internal VCC reads %1.3f volts\n", volts / 1000 );
    Serial.println(F("CPU going to sleep, pull WAKE_UP_PIN low to wake it (press the button)"));
    delay(100);  // needs a brief delay after the print or it may print the whole message
    wifi_fpm_set_sleep_type(LIGHT_SLEEP_T);
    gpio_pin_wakeup_enable(GPIO_ID_PIN(WAKE_UP_PIN), GPIO_PIN_INTR_LOLEVEL);
    // only LOLEVEL or HILEVEL interrupts work, no edge, that's an SDK or CPU limitation
    wifi_fpm_set_wakeup_cb(wakeupCallback); // Set wakeup callback (optional)
    wifi_fpm_open();
    wifi_fpm_do_sleep(0xFFFFFFF);  // only 0xFFFFFFF allowed; any other value and it won't sleep
    delay(10);  // it goes to sleep some time during this delay() and waits for an interrupt
    Serial.println(F("Woke up!"));  // the interrupt callback hits before this is executed

    // 6th test - Deep Sleep for 10 seconds, wake with RF_DEFAULT
    Serial.println(F("\n6th test - Deep Sleep for 10 seconds, wake with RF_DEFAULT"));
    init_WiFi();  // initialize WiFi since we turned it off in the last test
    init_OTA();
    resetCount = 1;  // advance to the next Deep Sleep test after the reset
    updateRTC();  // save the current test state in RTC memory
    volts = ESP.getVcc();
    Serial.printf("The internal VCC reads %1.3f volts\n", volts / 1000 );
    Serial.println(F("press the button to continue"));
    while (!digitalRead(WAKE_UP_PIN))  // wait for them to release the button from the last test
      delay(10);
    delay(50);  // debounce time for the switch, button released
    waitPushbutton(false, blinkDelay);
    digitalWrite(LED, LOW);  // turn the LED on, at least briefly
    Serial.println(F("going into Deep Sleep now..."));
    delay(10);  // sometimes the \n isn't printed without a short delay
    ESP.deepSleep(10E6, WAKE_RF_DEFAULT); // good night!  D0 fires a reset in 10 seconds...
    delay(10);
    // if you do ESP.deepSleep(0, mode); it needs a RESET to come out of sleep (RTC is off)
    // maximum timed Deep Sleep interval = 71.58 minutes with 0xFFFFFFFF
    // the 2 uA GPIO current during Deep Sleep can't drive the LED so it's off now
    Serial.println(F("What... I'm not asleep?!?"));  // it will never get here
  }

  // 7th test - Deep Sleep for 10 seconds, wake with RFCAL
  if (resetCount < 4) {
    init_WiFi();  // need to reinitialize WiFi & OTA due to Deep Sleep resets
    init_OTA();   // since we didn't do it in setup() because of the first test
  }
  if (resetCount == 1) {  // second reset loop since power on
    resetCount = 2;  // advance to the next Deep Sleep test after the reset
    updateRTC();  // save the current test state in RTC memory
    Serial.println(F("\n7th test - in RF_DEFAULT, Deep Sleep for 10 seconds, wake with RFCAL"));
    float volts = ESP.getVcc();
    Serial.printf("The internal VCC reads %1.3f volts\n", volts / 1000 );
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
    Serial.println(F("\n8th test - in RFCAL, Deep Sleep Instant for 10 seconds, wake with NO_RFCAL"));
    float volts = ESP.getVcc();
    Serial.printf("The internal VCC reads %1.3f volts\n", volts / 1000 );
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
    Serial.println(F("\n9th test - in NO_RFCAL, Deep Sleep Instant for 10 seconds, wake with RF_DISABLED"));
    float volts = ESP.getVcc();
    Serial.printf("The internal VCC reads %1.3f volts\n", volts / 1000 );
    Serial.println(F("press the button to continue"));
    waitPushbutton(false, blinkDelay);
    Serial.println(F("going into Deep Sleep now..."));
    delay(10);  // sometimes the \n isn't printed without a short delay
    ESP.deepSleepInstant(10E6, WAKE_RF_DISABLED); // good night!  D0 fires a reset in 10 seconds...
    delay(10);
    Serial.println(F("What... I'm not asleep?!?"));  // it will never get here
  }

  if (resetCount == 4) {
    resetCount = 5;  // start all over
    updateRTC();  // save the current test state in RTC memory
    float volts = ESP.getVcc();
    Serial.printf("The internal VCC reads %1.3f volts\n", volts / 1000 );
    Serial.println(F("\nTests completed, in RF_DISABLED, press the button to do an ESP.restart()"));
    waitPushbutton(false, 1000);
    ESP.restart();
  }
}

void waitPushbutton(bool usesDelay, unsigned int delayTime) {  // loop until they press the button
  // note: 2 different modes, as both of the AUTOMATIC power saving modes need a long delay()
  if (!usesDelay) {  // quick interception of button press, no delay()
    blinkLED.reset(delayTime);
    while (digitalRead(WAKE_UP_PIN)) {  // wait for a button press
      if (blinkLED) {
        digitalWrite(LED, !digitalRead(LED));  // toggle the activity LED
        if (WiFi.localIP())  // don't check OTA if WiFi isn't connected
          ArduinoOTA.handle();  //see if we need to reflash
      }
      yield();
    }
  } else {  // long delay() for the 2 AUTOMATIC modes, but it misses quick button presses
    while (digitalRead(WAKE_UP_PIN)) {  // wait for a button press
      digitalWrite(LED, !digitalRead(LED));  // toggle the activity LED
      if (WiFi.localIP())  // don't check OTA if WiFi isn't connected
        ArduinoOTA.handle();  //see if we need to reflash
      delay(delayTime);
    }
  }
  delay(50);  // debounce time for the switch, button pressed
  while (!digitalRead(WAKE_UP_PIN))  // now wait for them to release the button
    delay(10);
  delay(50);  // debounce time for the switch, button released
}

uint32_t calculateCRC32(const uint8_t *data, size_t length) {
  uint32_t crc = 0xffffffff;
  while (length--) {
    uint8_t c = *data++;
    for (uint32_t i = 0x80; i > 0; i >>= 1) {
      bool bit = crc & 0x80000000;
      if (c & i) {
        bit = !bit;
      }
      crc <<= 1;
      if (bit) {
        crc ^= 0x04c11db7;
      }
    }
  }
  return crc;
}

void updateRTC() {
  rtcData.data[3] = resetCount;  // save the loop count for the next reset
  // Update CRC32 of data
  rtcData.crc32 = calculateCRC32((uint8_t*) &rtcData.data[0], sizeof(rtcData.data));
  if (resetCount == 5)  // wipe the CRC in RTC memory when we're done with all tests
    rtcData.crc32 = 0;
  // Write struct to RTC memory
  ESP.rtcUserMemoryWrite(64, (uint32_t*) &rtcData, sizeof(rtcData));
}

void init_WiFi() {
  /* Explicitly set the ESP8266 as a WiFi-client (STAtion mode), otherwise by default it
    would try to act as both a client and an access-point and could cause network issues
    with other WiFi devices on your network. */
  digitalWrite(LED, LOW);  // give a visual indication that we're alive but busy
  WiFi.persistent(false);  // don't store the connection each time to save wear on the flash
  WiFi.mode(WIFI_STA);
  WiFi.config(staticIP, gateway, subnet);  // if using static IP, enter parameters at the top
  WiFi.begin(AP_SSID, AP_PASS);
  Serial.print(F("connecting to WiFi "));
  Serial.println(AP_SSID);
  DEBUG_PRINT(F("my MAC: "));
  DEBUG_PRINTLN(WiFi.macAddress());
  while (WiFi.status() != WL_CONNECTED)
    delay(50);
  DEBUG_PRINTLN(F("WiFi connected"));
  while (!WiFi.localIP())
    delay(50);
  WiFi.setAutoReconnect(true);
  DEBUG_PRINT(F("WiFi Gateway IP: "));
  DEBUG_PRINTLN(WiFi.gatewayIP());
  DEBUG_PRINT(F("my IP address: "));
  DEBUG_PRINTLN(WiFi.localIP());
}

void init_OTA() {
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266"));

  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123"));

  ArduinoOTA.onStart([]() {
    Serial.println(F("Start"));
  });
  ArduinoOTA.onEnd([]() {
    Serial.println(F("\nEnd"));
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println(F("Auth Failed"));
    else if (error == OTA_BEGIN_ERROR) Serial.println(F("Begin Failed"));
    else if (error == OTA_CONNECT_ERROR) Serial.println(F("Connect Failed"));
    else if (error == OTA_RECEIVE_ERROR) Serial.println(F("Receive Failed"));
    else if (error == OTA_END_ERROR) Serial.println(F("End Failed"));
  });
  ArduinoOTA.begin();
  yield();
}
