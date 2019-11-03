#include <ESP8266WiFi.h>
#include <TypeConversionFunctions.h>
#include <assert.h>
#include <FloodingMesh.h>

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
const char exampleMeshName[] PROGMEM = "MeshNode_"; // The name of the mesh network. Used as prefix for the node SSID and to find other network nodes in the example networkFilter and broadcastFilter functions below.
const char exampleWiFiPassword[] PROGMEM = "ChangeThisWiFiPassword_TODO"; // The password has to be min 8 and max 64 characters long, otherwise an AP which uses it will not be found during scans.

// A custom encryption key is required when using encrypted ESP-NOW transmissions. There is always a default Kok set, but it can be replaced if desired.
// All ESP-NOW keys below must match in an encrypted connection pair for encrypted communication to be possible.
uint8_t espnowEncryptionKey[16] = {0x33, 0x44, 0x33, 0x44, 0x33, 0x44, 0x33, 0x44, // This is the key for encrypting transmissions.
                                   0x33, 0x44, 0x33, 0x44, 0x33, 0x44, 0x32, 0x11
                                  };
uint8_t espnowHashKey[16] = {0xEF, 0x44, 0x33, 0x0C, 0x33, 0x44, 0xFE, 0x44, // This is the secret key used for HMAC during encrypted connection requests.
                             0x33, 0x44, 0x33, 0xB0, 0x33, 0x44, 0x32, 0xAD
                            };

bool meshMessageHandler(String &message, FloodingMesh &meshInstance);

/* Create the mesh node object */
FloodingMesh floodingMesh = FloodingMesh(meshMessageHandler, FPSTR(exampleWiFiPassword), espnowEncryptionKey, espnowHashKey, FPSTR(exampleMeshName), uint64ToString(ESP.getChipId()), true);

bool theOne = true;
String theOneMac = "";

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
  int32_t delimiterIndex = message.indexOf(meshInstance.broadcastMetadataDelimiter());
  if (delimiterIndex == 0) {
    Serial.print("Message received from STA " + meshInstance.getEspnowMeshBackend().getSenderMac() + ": ");
    Serial.println(message.substring(1, 101));

    String potentialMac = message.substring(1, 13);

    if (potentialMac > theOneMac) {
      if (theOne) {
        if (useLED) {
          digitalWrite(LED_BUILTIN, HIGH); // Turn LED off (LED is active low)
        }

        theOne = false;
      }

      theOneMac = potentialMac;

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
          Serial.println("missed/total: " + String(missedBroadcasts) + '/' + String(totalReceivedBroadcasts));
        }
        if (totalReceivedBroadcasts % 500 == 0) {
          Serial.println("Benchmark message: " + message.substring(0, 100));
        }
      }
    }
  } else {
    // Only show first 100 characters because printing a large String takes a lot of time, which is a bad thing for a callback function.
    // If you need to print the whole String it is better to store it and print it in the loop() later.
    Serial.print("Message with origin " + meshInstance.getOriginMac() + " received: ");
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

  uint8_t apMacArray[6] {0};
  theOneMac = macToString(WiFi.softAPmacAddress(apMacArray));

  if (useLED) {
    pinMode(LED_BUILTIN, OUTPUT); // Initialize the LED_BUILTIN pin as an output
    digitalWrite(LED_BUILTIN, LOW); // Turn LED on (LED is active low)
  }

  floodingMeshDelay(5000); // Give some time for user to start the nodes
}

int32_t timeOfLastProclamation = -10000;
void loop() {
  static uint32_t benchmarkCount = 0;
  static uint32_t loopStart = millis();

  // The floodingMeshDelay() method performs all the background operations for the FloodingMesh (via FloodingMesh::performMeshMaintainance()).
  // It is recommended to place one of these methods in the beginning of the loop(), unless there is a need to put them elsewhere.
  // Among other things, the method cleans up old ESP-NOW log entries (freeing up RAM) and forwards received mesh messages.
  // Note that depending on the amount of messages to forward and their length, this method can take tens or even hundreds of milliseconds to complete.
  // More intense transmission activity and less frequent calls to performMeshMaintainance will likely cause the method to take longer to complete, so plan accordingly.
  // The maintainance methods should not be used inside the meshMessageHandler callback, since they can alter the mesh node state. The framework will alert you during runtime if you make this mistake.
  floodingMeshDelay(1);

  // If you wish to transmit only to a single node, try using one of the following methods (requires the node to be within range and know the MAC of the recipient):
  // Unencrypted: transmission_status_t floodingMesh.getEspnowMeshBackend().attemptTransmission(message, EspnowNetworkInfo(recipientMac));
  // Encrypted (slow): floodingMesh.getEspnowMeshBackend().attemptAutoEncryptingTransmission(message, EspnowNetworkInfo(recipientMac));

  if (theOne) {
    if (millis() - timeOfLastProclamation > 10000) {
      uint32_t startTime = millis();
      floodingMesh.broadcast(String(floodingMesh.broadcastMetadataDelimiter()) + theOneMac + " is The One.");
      Serial.println("Proclamation broadcast done in " + String(millis() - startTime) + " ms.");

      timeOfLastProclamation = millis();
      floodingMeshDelay(20);
    }

    if (millis() - loopStart > 23000) { // Start benchmarking the mesh once three proclamations have been made
      uint32_t startTime = millis();
      floodingMesh.broadcast(String(benchmarkCount++) + String(floodingMesh.broadcastMetadataDelimiter()) + ": Not a spoon in sight.");
      Serial.println("Benchmark broadcast done in " + String(millis() - startTime) + " ms.");
      floodingMeshDelay(20);
    }
  }
}
