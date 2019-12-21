/**
   This example makes every node broadcast their AP MAC to the rest of the network during the first 28 seconds, as long as the node thinks it has the highest AP MAC in the network.
   Once 28 seconds have passed, the node that has the highest AP MAC will start broadcasting benchmark messages, which will allow you to see how many messages are lost at the other nodes.
   If you have an onboard LED on your ESP8266 it is recommended that you change the useLED variable below to true.
   That way you will get instant confirmation of the mesh communication without checking the Serial Monitor.
*/

#define ESP8266WIFIMESH_DISABLE_COMPATIBILITY // Excludes redundant compatibility code. Should be used for new code until the compatibility code is removed with release 3.0.0 of the Arduino core.

#include <ESP8266WiFi.h>
#include <TypeConversionFunctions.h>
#include <assert.h>
#include <FloodingMesh.h>

namespace TypeCast = MeshTypeConversionFunctions;

/**
   NOTE: Although we could define the strings below as normal String variables,
   here we are using PROGMEM combined with the FPSTR() macro (and also just the F() macro further down in the file).
   The reason is that this approach will place the strings in flash memory which will help save RAM during program execution.
   Reading strings from flash will be slower than reading them from RAM,
   but this will be a negligible difference when printing them to Serial.

   More on F(), FPSTR() and PROGMEM:
   https://github.com/esp8266/Arduino/issues/1143
   https://arduino-esp8266.readthedocs.io/en/latest/PROGMEM.html
*/
constexpr char exampleMeshName[] PROGMEM = "MeshNode_"; // The name of the mesh network. Used as prefix for the node SSID and to find other network nodes in the example networkFilter and broadcastFilter functions below.
constexpr char exampleWiFiPassword[] PROGMEM = "ChangeThisWiFiPassword_TODO"; // The password has to be min 8 and max 64 characters long, otherwise an AP which uses it will not be found during scans.

// A custom encryption key is required when using encrypted ESP-NOW transmissions. There is always a default Kok set, but it can be replaced if desired.
// All ESP-NOW keys below must match in an encrypted connection pair for encrypted communication to be possible.
// Note that it is also possible to use Strings as key seeds instead of arrays.
uint8_t espnowEncryptedConnectionKey[16] = {0x33, 0x44, 0x33, 0x44, 0x33, 0x44, 0x33, 0x44, // This is the key for encrypting transmissions of encrypted connections.
                                            0x33, 0x44, 0x33, 0x44, 0x33, 0x44, 0x32, 0x11
                                           };
uint8_t espnowHashKey[16] = {0xEF, 0x44, 0x33, 0x0C, 0x33, 0x44, 0xFE, 0x44, // This is the secret key used for HMAC during encrypted connection requests.
                             0x33, 0x44, 0x33, 0xB0, 0x33, 0x44, 0x32, 0xAD
                            };

bool meshMessageHandler(String &message, FloodingMesh &meshInstance);

/* Create the mesh node object */
FloodingMesh floodingMesh = FloodingMesh(meshMessageHandler, FPSTR(exampleWiFiPassword), espnowEncryptedConnectionKey, espnowHashKey, FPSTR(exampleMeshName), TypeCast::uint64ToString(ESP.getChipId()), true);

bool theOne = true;
String theOneMac;

bool useLED = false; // Change this to true if you wish the onboard LED to mark The One.

/**
   Callback for when a message is received from the mesh network.

   @param message The message String received from the mesh.
                  Modifications to this String are passed on when the message is forwarded from this node to other nodes.
                  However, the forwarded message will still use the same messageID.
                  Thus it will not be sent to nodes that have already received this messageID.
                  If you want to send a new message to the whole network, use a new broadcast from within the loop() instead.
   @param meshInstance The FloodingMesh instance that received the message.
   @return True if this node should forward the received message to other nodes. False otherwise.
*/
bool meshMessageHandler(String &message, FloodingMesh &meshInstance) {
  int32_t delimiterIndex = message.indexOf(meshInstance.metadataDelimiter());
  if (delimiterIndex == 0) {
    Serial.print(String(F("Message received from STA MAC ")) + meshInstance.getEspnowMeshBackend().getSenderMac() + F(": "));
    Serial.println(message.substring(2, 102));

    String potentialMac = message.substring(2, 14);

    if (potentialMac >= theOneMac) {
      if (potentialMac > theOneMac) {
        theOne = false;
        theOneMac = potentialMac;
      }

      if (useLED && !theOne) {
        bool ledState = message.charAt(1) == '1';
        digitalWrite(LED_BUILTIN, ledState); // Turn LED on/off (LED_BUILTIN is active low)
      }

      return true;
    } else {
      return false;
    }
  } else if (delimiterIndex > 0) {
    if (meshInstance.getOriginMac() == theOneMac) {
      uint32_t totalBroadcasts = strtoul(message.c_str(), nullptr, 0); // strtoul stops reading input when an invalid character is discovered.

      // Static variables are only initialized once.
      static uint32_t firstBroadcast = totalBroadcasts;

      if (totalBroadcasts - firstBroadcast >= 100) { // Wait a little to avoid start-up glitches
        static uint32_t missedBroadcasts = 1; // Starting at one to compensate for initial -1 below.
        static uint32_t previousTotalBroadcasts = totalBroadcasts;
        static uint32_t totalReceivedBroadcasts = 0;
        totalReceivedBroadcasts++;

        missedBroadcasts += totalBroadcasts - previousTotalBroadcasts - 1; // We expect an increment by 1.
        previousTotalBroadcasts = totalBroadcasts;

        if (totalReceivedBroadcasts % 50 == 0) {
          Serial.println(String(F("missed/total: ")) + String(missedBroadcasts) + '/' + String(totalReceivedBroadcasts));
        }
        if (totalReceivedBroadcasts % 500 == 0) {
          Serial.println(String(F("Benchmark message: ")) + message.substring(0, 100));
        }
      }
    }
  } else {
    // Only show first 100 characters because printing a large String takes a lot of time, which is a bad thing for a callback function.
    // If you need to print the whole String it is better to store it and print it in the loop() later.
    Serial.print(String(F("Message with origin ")) + meshInstance.getOriginMac() + F(" received: "));
    Serial.println(message.substring(0, 100));
  }

  return true;
}

void setup() {
  // Prevents the flash memory from being worn out, see: https://github.com/esp8266/Arduino/issues/1054 .
  // This will however delay node WiFi start-up by about 700 ms. The delay is 900 ms if we otherwise would have stored the WiFi network we want to connect to.
  WiFi.persistent(false);

  Serial.begin(115200);
  delay(50); // Wait for Serial.

  //yield(); // Use this if you don't want to wait for Serial, but not with the ESP-NOW backend (yield() causes crashes with ESP-NOW).

  // The WiFi.disconnect() ensures that the WiFi is working correctly. If this is not done before receiving WiFi connections,
  // those WiFi connections will take a long time to make or sometimes will not work at all.
  WiFi.disconnect();

  Serial.println();
  Serial.println();

  Serial.println(F("If you have an onboard LED on your ESP8266 it is recommended that you change the useLED variable to true.\n"
                   "That way you will get instant confirmation of the mesh communication.\n"
                   "Also, remember to change the default mesh network password and ESP-NOW keys!\n"));

  Serial.println(F("Setting up mesh node..."));

  floodingMesh.begin();
  floodingMesh.activateAP();

  uint8_t apMacArray[6] {0};
  theOneMac = TypeCast::macToString(WiFi.softAPmacAddress(apMacArray));

  if (useLED) {
    pinMode(LED_BUILTIN, OUTPUT); // Initialize the LED_BUILTIN pin as an output
    digitalWrite(LED_BUILTIN, LOW); // Turn LED on (LED_BUILTIN is active low)
  }

  // Uncomment the lines below to use automatic AEAD encryption/decryption of messages sent/received via broadcast() and encryptedBroadcast().
  // The main benefit of AEAD encryption is that it can be used with normal broadcasts (which are substantially faster than encryptedBroadcasts).
  // The main drawbacks are that AEAD only encrypts the message data (not transmission metadata), transfers less data per message and lacks replay attack protection.
  // When using AEAD, potential replay attacks must thus be handled manually.
  //floodingMesh.getEspnowMeshBackend().setEspnowMessageEncryptionKey(F("ChangeThisKeySeed_TODO")); // The message encryption key should always be set manually. Otherwise a default key (all zeroes) is used.
  //floodingMesh.getEspnowMeshBackend().setUseEncryptedMessages(true);

  floodingMeshDelay(5000); // Give some time for user to start the nodes
}

int32_t timeOfLastProclamation = -10000;
void loop() {
  static bool ledState = 1;
  static uint32_t benchmarkCount = 0;
  static uint32_t loopStart = millis();

  // The floodingMeshDelay() method performs all the background operations for the FloodingMesh (via FloodingMesh::performMeshMaintenance()).
  // It is recommended to place one of these methods in the beginning of the loop(), unless there is a need to put them elsewhere.
  // Among other things, the method cleans up old ESP-NOW log entries (freeing up RAM) and forwards received mesh messages.
  // Note that depending on the amount of messages to forward and their length, this method can take tens or even hundreds of milliseconds to complete.
  // More intense transmission activity and less frequent calls to performMeshMaintenance will likely cause the method to take longer to complete, so plan accordingly.
  // The maintenance methods should not be used inside the meshMessageHandler callback, since they can alter the mesh node state. The framework will alert you during runtime if you make this mistake.
  floodingMeshDelay(1);

  // If you wish to transmit only to a single node, try using one of the following methods (requires the node to be within range and know the MAC of the recipient):
  // Unencrypted: transmission_status_t floodingMesh.getEspnowMeshBackend().attemptTransmission(message, EspnowNetworkInfo(recipientMac));
  // Encrypted (slow): floodingMesh.getEspnowMeshBackend().attemptAutoEncryptingTransmission(message, EspnowNetworkInfo(recipientMac));

  if (theOne) {
    if (millis() - timeOfLastProclamation > 10000) {
      uint32_t startTime = millis();
      ledState = ledState ^ bool(benchmarkCount); // Make other nodes' LEDs alternate between on and off once benchmarking begins.

      // Note: The maximum length of an unencrypted broadcast message is given by floodingMesh.maxUnencryptedMessageLength(). It is around 670 bytes by default.
      floodingMesh.broadcast(String(floodingMesh.metadataDelimiter()) + String(ledState) + theOneMac + F(" is The One."));
      Serial.println(String(F("Proclamation broadcast done in ")) + String(millis() - startTime) + F(" ms."));

      timeOfLastProclamation = millis();
      floodingMeshDelay(20);
    }

    if (millis() - loopStart > 23000) { // Start benchmarking the mesh once three proclamations have been made
      uint32_t startTime = millis();
      floodingMesh.broadcast(String(benchmarkCount++) + String(floodingMesh.metadataDelimiter()) + F(": Not a spoon in sight."));
      Serial.println(String(F("Benchmark broadcast done in ")) + String(millis() - startTime) + F(" ms."));
      floodingMeshDelay(20);
    }
  }
}
