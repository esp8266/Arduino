/*

  Example of Booting into UART Download using `ESP.rebootIntoUartDownloadMode()`

  Two methods are presented for starting UART Boot Mode.
    1) From `loop()` call the function `proxyEspSync()`, which peeks for a SLIP
       frame marker. Then when present, look for an esptool ESP_SYNC packet on
       the Serial port.
    2) A simple hotkey of 'D'.

  After either of these, `ESP.rebootIntoUartDownloadMode()` is called to place
  the ESP8266 into UART Flash program mode.

  For a quick test to confirm the ESP8266 is responding to esptool.py,
  use this command:
    esptool.py --chip esp8266 --before no_reset --after soft_reset flash_id


  Note with these methods a hard reset is not done, and the esptool.py may not
  detect and report the correct Crystal frequency for the ESP Module. If you
  need that info, it needs to be gathered after a Power-On or Hard Reset.
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>

////////////////////////////////////////////////////////////////////////////////
//
// Check Serial Receive for ESP_SYNC slip packet from esptool.py
//
// If you are already using Serial input for command input, the character '\xC0'
// is not available. We must reserve its use for the SLIP Frame Marker. I am not
// sure which languages if any, would pose a problem. For the non-English
// languages check your character set values to be sure it is not an issue. If
// it is an issue, you will not be able to use this method as presented. The
// '\xC0' character is defined by the SLIP protocol and cannot be changed.

// If your needs require it, you can add logic to loop() for setting and
// clearing uartDownloadEnable. For example, you could add a push button to a
// GPIO pin and monitor for a 5-second press. Then, set uartDownloadEnable to
// true. In addition to that, you could also define a time-to-live for that
// state and clear it after it elapses.
//
// Change this to false if you do not want ESP_SYNC monitor always on.
bool uartDownloadEnable = true;

// Buffer size to receive an ESP_SYNC packet into, larger than the expected
// ESP_SYNC packet length.
constexpr size_t pktBufSz = 64;

// Enough time to receive 115 bytes at 115200bps.
// More than enough to finish receiving an ESP_SYNC packet.
constexpr size_t kSyncTimeoutMs = 10;

// The SLIP Frame end character, which is also used to start a frame.
constexpr char slipFrameMarker = '\xC0';

// General packet format:
//   <0xC0><cmd><payload length><32 bit cksum><payload data ...><0xC0>
// Slip packet for ESP_SYNC, minus the frame markers ('\xC0') captured from
// esptool using the `--trace` option.
const char syncPkt[] PROGMEM = "\x00\x08\x24\x00\x00\x00\x00\x00\x07\x07\x12\x20"
                               "UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU";

constexpr size_t syncPktSz = sizeof(syncPkt) - 1;  // Don't compare zero terminator char

//
//  Use the discovery of an ESP_SYNC packet, to trigger calling UART Download
//  Mode. At entry we expect the Serial FIFO to start with the byte following
//  the slipFrameMarker.
//
void proxyEspSync() {
  if (!uartDownloadEnable) { return; }

  byte buf[pktBufSz];

  // If it is an ESP_SYNC packet, it will not take long for readBytesUntil() to
  // complete.
  Serial.setTimeout(kSyncTimeoutMs);
  int len = Serial.readBytesUntil(slipFrameMarker, buf, pktBufSz);

  // To avoid a false trigger, only start UART Download Mode when we get an
  // exact match to the captured esptool ESP_SYNC packet.
  if (syncPktSz == len && 0 == memcmp_P(buf, syncPkt, len)) {
    ESP.rebootIntoUartDownloadMode();
    // Does not return
  }

  // Assume RX FIFO data is garbled and flush all RX data.
  while (0 <= Serial.read()) {}  // Clear FIFO

  // If your Serial requirements need a specific timeout value, you would
  // restore those here.
}
//
////////////////////////////////////////////////////////////////////////////////

void setup() {
  // For `proxyEspSync()` to work, the Serial.begin() speed needs to be
  // 115200bps. This is the data rate used by esptool.py. It expects the Boot
  // ROM to use its "auto-baud" feature to match up. Since `proxyEspSync()` is
  // acting as a proxy we must use 115200.
  //
  // If on the Arduino IDE Tools menu you use "Upload Speeds" above 115200, it
  // will work. When esptool.py is run with the `--baud BAUD` option specified
  // above 115200, initial communication with the ESP8266 is done at 115200bps.
  // Once esptool.py has synchronize with the ESP8266 and downloaded a short
  // stub, then both devices shift their UART speeds to the command line value.
  Serial.begin(115200);

  Serial.println(F("\r\n\r\n"
                   "Boot UART Download Demo - initialization started.\r\n"
                   "\r\n"
                   "For a quick test to see the UART Download work,\r\n"
                   "stop your serial terminal APP and run:\r\n"
                   "  esptool.py --chip esp8266 --before no_reset --after soft_reset flash_id\r\n"));

  // ...
}

void cmdLoop(Print& oStream, int key) {
  switch (key) {
    case 'e':
      oStream.println(F("Enable monitor for detecting ESP_SYNC from esptool.py"));
      uartDownloadEnable = true;
      break;

    case 'D':
      // This option would be prone to false triggering. It is here for DEMO
      // purposes and debugging.
      oStream.println(F("Boot into UART download mode ..."));
      oStream.flush();
      ESP.rebootIntoUartDownloadMode();
      break;

    case 'R':
      oStream.println(F("Restart ..."));
      oStream.flush();
      ESP.restart();
      break;

      // ...

    case '?':
      oStream.println(F("\r\nHot key help:"));
      if (!uartDownloadEnable) { oStream.println(F("  e - Enable monitor for detecting ESP_SYNC from esptool.py")); }
      oStream.println(F("  D - Boot into UART download mode"));
      oStream.println(F("  R - Restart"));
      oStream.println(F("  ? - This help message\r\n"));
      break;

    default: break;
  }

  oStream.println();
}


void loop() {

  // In this example, we can have Serial data from a user keystroke for our
  // command loop or the esptool trying to SYNC up for flashing.  If the
  // character matches the Slip Frame Marker (the 1st byte of the SYNC packet),
  // we intercept it and call our ESP_SYNC proxy to complete the verification
  // and reboot into the UART Downloader. Otherwise, process the keystroke as
  // normal.
  if (0 < Serial.available()) {
    int keyPress = Serial.read();
    if (slipFrameMarker == keyPress) {
      proxyEspSync();
    } else {
      cmdLoop(Serial, keyPress);
    }
  }

  // ...
}
