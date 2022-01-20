#define ESP8266WIFIMESH_DISABLE_COMPATIBILITY  // Excludes redundant compatibility code. TODO:
                                               // Should be used for new code until the
                                               // compatibility code is removed with release 3.0.0
                                               // of the Arduino core.

#include <ESP8266WiFi.h>
#include <EspnowMeshBackend.h>
#include <TcpIpMeshBackend.h>
#include <TypeConversionFunctions.h>
#include <assert.h>

namespace TypeCast = MeshTypeConversionFunctions;

/**
   NOTE: Although we could define the strings below as normal String variables,
   here we are using PROGMEM combined with the FPSTR() macro (and also just the F() macro further
   down in the file). The reason is that this approach will place the strings in flash memory which
   will help save RAM during program execution. Reading strings from flash will be slower than
   reading them from RAM, but this will be a negligible difference when printing them to Serial.

   More on F(), FPSTR() and PROGMEM:
   https://github.com/esp8266/Arduino/issues/1143
   https://arduino-esp8266.readthedocs.io/en/latest/PROGMEM.html
*/
constexpr char exampleMeshName[] PROGMEM
    = "MeshNode_";  // The name of the mesh network. Used as prefix for the node SSID and to find
                    // other network nodes in the example networkFilter and broadcastFilter
                    // functions below.
constexpr char exampleWiFiPassword[] PROGMEM
    = "ChangeThisWiFiPassword_TODO";  // Note: " is an illegal character. The password has to be min
                                      // 8 and max 64 characters long, otherwise an AP which uses it
                                      // will not be found during scans.

// A custom encryption key is required when using encrypted ESP-NOW transmissions. There is always a
// default Kok set, but it can be replaced if desired. All ESP-NOW keys below must match in an
// encrypted connection pair for encrypted communication to be possible. Note that it is also
// possible to use Strings as key seeds instead of arrays.
uint8_t espnowEncryptedConnectionKey[16]
    = { 0x33, 0x44, 0x33, 0x44, 0x33,
        0x44, 0x33, 0x44,  // This is the key for encrypting transmissions of encrypted connections.
        0x33, 0x44, 0x33, 0x44, 0x33,
        0x44, 0x32, 0x11 };
uint8_t espnowEncryptionKok[16]
    = { 0x22, 0x44, 0x33, 0x44,
        0x33, 0x44, 0x33, 0x44,  // This is the key for encrypting the encrypted connection key.
        0x33, 0x44, 0x33, 0x44,
        0x33, 0x44, 0x32, 0x33 };
uint8_t espnowHashKey[16] = {
  0xEF, 0x44, 0x33, 0x0C, 0x33,
  0x44, 0xFE, 0x44,  // This is the secret key used for HMAC during encrypted connection requests.
  0x33, 0x44, 0x33, 0xB0, 0x33,
  0x44, 0x32, 0xAD
};

unsigned int requestNumber  = 0;
unsigned int responseNumber = 0;

const char broadcastMetadataDelimiter
    = 23;  // 23 = End-of-Transmission-Block (ETB) control character in ASCII

String                 manageRequest(const String& request, MeshBackendBase& meshInstance);
TransmissionStatusType manageResponse(const String& response, MeshBackendBase& meshInstance);
void                   networkFilter(int numberOfNetworks, MeshBackendBase& meshInstance);
bool                   broadcastFilter(String& firstTransmission, EspnowMeshBackend& meshInstance);

/* Create the mesh node object */
EspnowMeshBackend espnowNode
    = EspnowMeshBackend(manageRequest, manageResponse, networkFilter, broadcastFilter,
                        FPSTR(exampleWiFiPassword), espnowEncryptedConnectionKey, espnowHashKey,
                        FPSTR(exampleMeshName), TypeCast::uint64ToString(ESP.getChipId()), true);

/**
   Callback for when other nodes send you a request

   @param request The request string received from another node in the mesh
   @param meshInstance The MeshBackendBase instance that called the function.
   @return The string to send back to the other node. For ESP-NOW, return an empty string ("") if no
   response should be sent.
*/
String manageRequest(const String& request, MeshBackendBase& meshInstance) {
  // To get the actual class of the polymorphic meshInstance, do as follows (meshBackendCast
  // replaces dynamic_cast since RTTI is disabled)
  if (EspnowMeshBackend* espnowInstance
      = TypeCast::meshBackendCast<EspnowMeshBackend*>(&meshInstance)) {
    String transmissionEncrypted = espnowInstance->receivedEncryptedTransmission() ?
                                       F(", Encrypted transmission") :
                                       F(", Unencrypted transmission");
    Serial.print(String(F("ESP-NOW (")) + espnowInstance->getSenderMac() + transmissionEncrypted
                 + F("): "));
  } else if (TcpIpMeshBackend* tcpIpInstance
             = TypeCast::meshBackendCast<TcpIpMeshBackend*>(&meshInstance)) {
    (void)tcpIpInstance;  // This is useful to remove a "unused parameter" compiler warning. Does
                          // nothing else.
    Serial.print(F("TCP/IP: "));
  } else {
    Serial.print(F("UNKNOWN!: "));
  }

  /* Print out received message */
  // Only show first 100 characters because printing a large String takes a lot of time, which is a
  // bad thing for a callback function. If you need to print the whole String it is better to store
  // it and print it in the loop() later. Note that request.substring will not work as expected if
  // the String contains null values as data.
  Serial.print(F("Request received: "));

  if (request.charAt(0) == 0) {
    Serial.println(request);  // substring will not work for multiStrings.
  } else {
    Serial.println(request.substring(0, 100));
  }

  /* return a string to send back */
  return (String(F("Hello world response #")) + String(responseNumber++) + F(" from ")
          + meshInstance.getMeshName() + meshInstance.getNodeID() + F(" with AP MAC ")
          + WiFi.softAPmacAddress() + String('.'));
}

/**
   Callback for when you get a response from other nodes

   @param response The response string received from another node in the mesh
   @param meshInstance The MeshBackendBase instance that called the function.
   @return The status code resulting from the response, as an int
*/
TransmissionStatusType manageResponse(const String& response, MeshBackendBase& meshInstance) {
  TransmissionStatusType statusCode = TransmissionStatusType::TRANSMISSION_COMPLETE;

  // To get the actual class of the polymorphic meshInstance, do as follows (meshBackendCast
  // replaces dynamic_cast since RTTI is disabled)
  if (EspnowMeshBackend* espnowInstance
      = TypeCast::meshBackendCast<EspnowMeshBackend*>(&meshInstance)) {
    String transmissionEncrypted = espnowInstance->receivedEncryptedTransmission() ?
                                       F(", Encrypted transmission") :
                                       F(", Unencrypted transmission");
    Serial.print(String(F("ESP-NOW (")) + espnowInstance->getSenderMac() + transmissionEncrypted
                 + F("): "));
  } else if (TcpIpMeshBackend* tcpIpInstance
             = TypeCast::meshBackendCast<TcpIpMeshBackend*>(&meshInstance)) {
    Serial.print(F("TCP/IP: "));

    // Getting the sent message like this will work as long as ONLY(!) TCP/IP is used.
    // With TCP/IP the response will follow immediately after the request, so the stored message
    // will not have changed. With ESP-NOW there is no guarantee when or if a response will show up,
    // it can happen before or after the stored message is changed. So for ESP-NOW, adding unique
    // identifiers in the response and request is required to associate a response with a request.
    Serial.print(F("Request sent: "));
    Serial.println(tcpIpInstance->getCurrentMessage().substring(0, 100));
  } else {
    Serial.print(F("UNKNOWN!: "));
  }

  /* Print out received message */
  // Only show first 100 characters because printing a large String takes a lot of time, which is a
  // bad thing for a callback function. If you need to print the whole String it is better to store
  // it and print it in the loop() later. Note that response.substring will not work as expected if
  // the String contains null values as data.
  Serial.print(F("Response received: "));
  Serial.println(response.substring(0, 100));

  return statusCode;
}

/**
   Callback used to decide which networks to connect to once a WiFi scan has been completed.

   @param numberOfNetworks The number of networks found in the WiFi scan.
   @param meshInstance The MeshBackendBase instance that called the function.
*/
void networkFilter(int numberOfNetworks, MeshBackendBase& meshInstance) {
  // Note that the network index of a given node may change whenever a new scan is done.
  for (int networkIndex = 0; networkIndex < numberOfNetworks; ++networkIndex) {
    String currentSSID   = WiFi.SSID(networkIndex);
    int    meshNameIndex = currentSSID.indexOf(meshInstance.getMeshName());

    /* Connect to any _suitable_ APs which contain meshInstance.getMeshName() */
    if (meshNameIndex >= 0) {
      uint64_t targetNodeID = TypeCast::stringToUint64(
          currentSSID.substring(meshNameIndex + meshInstance.getMeshName().length()));

      if (targetNodeID < TypeCast::stringToUint64(meshInstance.getNodeID())) {
        if (EspnowMeshBackend* espnowInstance
            = TypeCast::meshBackendCast<EspnowMeshBackend*>(&meshInstance)) {
          espnowInstance->connectionQueue().emplace_back(networkIndex);
        } else if (TcpIpMeshBackend* tcpIpInstance
                   = TypeCast::meshBackendCast<TcpIpMeshBackend*>(&meshInstance)) {
          tcpIpInstance->connectionQueue().emplace_back(networkIndex);
        } else {
          Serial.println(F("Invalid mesh backend!"));
        }
      }
    }
  }
}

/**
   Callback used to decide which broadcast messages to accept. Only called for the first
   transmission in each broadcast. If true is returned from this callback, the first broadcast
   transmission is saved until the entire broadcast message has been received. The complete
   broadcast message will then be sent to the requestHandler (manageRequest in this example). If
   false is returned from this callback, the broadcast message is discarded. Note that the
   BroadcastFilter may be called multiple times for messages that are discarded in this way, but is
   only called once for accepted messages.

   @param firstTransmission The first transmission of the broadcast. Modifications to this String
   are passed on to the broadcast message.
   @param meshInstance The EspnowMeshBackend instance that called the function.

   @return True if the broadcast should be accepted. False otherwise.
*/
bool broadcastFilter(String& firstTransmission, EspnowMeshBackend& meshInstance) {
  // This example broadcastFilter will accept a transmission if it contains the
  // broadcastMetadataDelimiter and as metaData either no targetMeshName or a targetMeshName that
  // matches the MeshName of meshInstance.

  int32_t metadataEndIndex = firstTransmission.indexOf(broadcastMetadataDelimiter);

  if (metadataEndIndex == -1) {
    return false;  // broadcastMetadataDelimiter not found
  }

  String targetMeshName = firstTransmission.substring(0, metadataEndIndex);

  if (!targetMeshName.isEmpty() && meshInstance.getMeshName() != targetMeshName) {
    return false;  // Broadcast is for another mesh network
  } else {
    // Remove metadata from message and mark as accepted broadcast.
    // Note that when you modify firstTransmission it is best to avoid using substring or other
    // String methods that rely on null values for String length determination. Otherwise your
    // broadcasts cannot include null values in the message bytes.
    firstTransmission.remove(0, metadataEndIndex + 1);
    return true;
  }
}

/**
   Once passed to the setTransmissionOutcomesUpdateHook method of the ESP-NOW backend,
   this function will be called after each update of the latestTransmissionOutcomes vector during
   attemptTransmission. (which happens after each individual transmission has finished)

   Example use cases is modifying getMessage() between transmissions, or aborting
   attemptTransmission before all nodes in the connectionQueue have been contacted.

   @param meshInstance The MeshBackendBase instance that called the function.

   @return True if attemptTransmission should continue with the next entry in the connectionQueue.
   False if attemptTransmission should stop.
*/
bool exampleTransmissionOutcomesUpdateHook(MeshBackendBase& meshInstance) {
  // Currently this is exactly the same as the default hook, but you can modify it to alter the
  // behaviour of attemptTransmission.

  (void)meshInstance;  // This is useful to remove a "unused parameter" compiler warning. Does
                       // nothing else.

  return true;
}

/**
   Once passed to the setResponseTransmittedHook method of the ESP-NOW backend,
   this function will be called after each attempted ESP-NOW response transmission.
   In case of a successful response transmission, this happens just before the response is removed
   from the waiting list. Only the hook of the EspnowMeshBackend instance that is
   getEspnowRequestManager() will be called.

   @param transmissionSuccessful True if the response was transmitted successfully. False otherwise.
   @param response The sent response.
   @param recipientMac The MAC address the response was sent to.
   @param responseIndex The index of the response in the waiting list.
   @param meshInstance The EspnowMeshBackend instance that called the function.

   @return True if the response transmission process should continue with the next response in the
   waiting list. False if the response transmission process should stop once processing of the just
   sent response is complete.
*/
bool exampleResponseTransmittedHook(bool transmissionSuccessful, const String& response,
                                    const uint8_t* recipientMac, uint32_t responseIndex,
                                    EspnowMeshBackend& meshInstance) {
  // Currently this is exactly the same as the default hook, but you can modify it to alter the
  // behaviour of sendEspnowResponses.

  (void)transmissionSuccessful;  // This is useful to remove a "unused parameter" compiler warning.
                                 // Does nothing else.
  (void)response;
  (void)recipientMac;
  (void)responseIndex;
  (void)meshInstance;

  return true;
}

void setup() {
  // Prevents the flash memory from being worn out, see:
  // https://github.com/esp8266/Arduino/issues/1054 . This will however delay node WiFi start-up by
  // about 700 ms. The delay is 900 ms if we otherwise would have stored the WiFi network we want to
  // connect to.
  WiFi.persistent(false);

  Serial.begin(115200);

  Serial.println();
  Serial.println();

  Serial.println(
      F("Note that this library can use static IP:s for the nodes with the TCP/IP backend to speed "
        "up connection times.\n"
        "Use the setStaticIP method to enable this.\n"
        "Ensure that nodes connecting to the same AP have distinct static IP:s.\n"
        "Also, remember to change the default mesh network password and ESP-NOW keys!\n\n"));

  Serial.println(F("Setting up mesh node..."));

  /* Initialise the mesh node */
  espnowNode.begin();

  // Note: This changes the Kok for all EspnowMeshBackend instances on this ESP8266.
  // Encrypted connections added before the Kok change will retain their old Kok.
  // Both Kok and encrypted connection key must match in an encrypted connection pair for encrypted
  // communication to be possible. Otherwise the transmissions will never reach the recipient, even
  // though acks are received by the sender.
  EspnowMeshBackend::setEspnowEncryptionKok(espnowEncryptionKok);
  espnowNode.setEspnowEncryptedConnectionKey(espnowEncryptedConnectionKey);

  // Makes it possible to find the node through scans, makes it possible to recover from an
  // encrypted connection where only the other node is encrypted, and also makes it possible to
  // receive broadcast transmissions. Note that only one AP can be active at a time in total, and
  // this will always be the one which was last activated. Thus the AP is shared by all backends.
  espnowNode.activateAP();

  // Storing our message in the EspnowMeshBackend instance is not required, but can be useful for
  // organizing code, especially when using many EspnowMeshBackend instances. Note that calling the
  // multi-recipient versions of espnowNode.attemptTransmission and
  // espnowNode.attemptAutoEncryptingTransmission will replace the stored message with whatever
  // message is transmitted. Also note that the maximum allowed number of ASCII characters in a
  // ESP-NOW message is given by EspnowMeshBackend::getMaxMessageLength().
  espnowNode.setMessage(String(F("Hello world request #")) + String(requestNumber) + F(" from ")
                        + espnowNode.getMeshName() + espnowNode.getNodeID() + String('.'));

  espnowNode.setTransmissionOutcomesUpdateHook(exampleTransmissionOutcomesUpdateHook);
  espnowNode.setResponseTransmittedHook(exampleResponseTransmittedHook);

  // In addition to using encrypted ESP-NOW connections the framework can also send automatically
  // encrypted messages (AEAD) over both encrypted and unencrypted connections. Using AEAD will only
  // encrypt the message content, not the transmission metadata. The AEAD encryption does not
  // require any pairing, and is thus faster for single messages than establishing a new encrypted
  // connection before transfer. AEAD encryption also works with ESP-NOW broadcasts and supports an
  // unlimited number of nodes, which is not true for encrypted connections. Encrypted ESP-NOW
  // connections do however come with built in replay attack protection, which is not provided by
  // the framework when using AEAD encryption, and allow EspnowProtocolInterpreter::aeadMetadataSize
  // extra message bytes per transmission. Transmissions via encrypted connections are also slightly
  // faster than via AEAD once a connection has been established.
  //
  // Uncomment the lines below to use automatic AEAD encryption/decryption of messages
  // sent/received. All nodes this node wishes to communicate with must then also use encrypted
  // messages with the same getEspnowMessageEncryptionKey(), or messages will not be accepted. Note
  // that using AEAD encrypted messages will reduce the number of message bytes that can be
  // transmitted.
  // espnowNode.setEspnowMessageEncryptionKey(F("ChangeThisKeySeed_TODO")); // The message
  // encryption key should always be set manually. Otherwise a default key (all zeroes) is used.
  // espnowNode.setUseEncryptedMessages(true);
}

int32_t timeOfLastScan = -10000;
void    loop() {
  // The performEspnowMaintenance() method performs all the background operations for the
  // EspnowMeshBackend. It is recommended to place it in the beginning of the loop(), unless there
  // is a need to put it elsewhere. Among other things, the method cleans up old Espnow log entries
  // (freeing up RAM) and sends the responses you provide to Espnow requests. Note that depending on
  // the amount of responses to send and their length, this method can take tens or even hundreds of
  // milliseconds to complete. More intense transmission activity and less frequent calls to
  // performEspnowMaintenance will likely cause the method to take longer to complete, so plan
  // accordingly.

  // Should not be used inside responseHandler, requestHandler, networkFilter or broadcastFilter
  // callbacks since performEspnowMaintenance() can alter the ESP-NOW state.
  EspnowMeshBackend::performEspnowMaintenance();

  if (millis() - timeOfLastScan
      > 10000) {  // Give other nodes some time to connect between data transfers.
    Serial.println(F("\nPerforming unencrypted ESP-NOW transmissions."));

    uint32_t startTime = millis();
    espnowNode.attemptTransmission(espnowNode.getMessage());
    Serial.println(String(F("Scan and ")) + String(espnowNode.latestTransmissionOutcomes().size())
                   + F(" transmissions done in ") + String(millis() - startTime) + F(" ms."));

    timeOfLastScan = millis();

    // Wait for response. espnowDelay continuously calls performEspnowMaintenance() so we will
    // respond to ESP-NOW request while waiting. Should not be used inside responseHandler,
    // requestHandler, networkFilter or broadcastFilter callbacks since performEspnowMaintenance()
    // can alter the ESP-NOW state.
    espnowDelay(100);

    // One way to check how attemptTransmission worked out
    if (espnowNode.latestTransmissionSuccessful()) {
      Serial.println(F("Transmission successful."));
    }

    // Another way to check how attemptTransmission worked out
    if (espnowNode.latestTransmissionOutcomes().empty()) {
      Serial.println(F("No mesh AP found."));
    } else {
      for (TransmissionOutcome& transmissionOutcome : espnowNode.latestTransmissionOutcomes()) {
        if (transmissionOutcome.transmissionStatus()
            == TransmissionStatusType::TRANSMISSION_FAILED) {
          Serial.println(String(F("Transmission failed to mesh AP ")) + transmissionOutcome.SSID());
        } else if (transmissionOutcome.transmissionStatus()
                   == TransmissionStatusType::CONNECTION_FAILED) {
          Serial.println(String(F("Connection failed to mesh AP ")) + transmissionOutcome.SSID());
        } else if (transmissionOutcome.transmissionStatus()
                   == TransmissionStatusType::TRANSMISSION_COMPLETE) {
          // No need to do anything, transmission was successful.
        } else {
          Serial.println(String(F("Invalid transmission status for ")) + transmissionOutcome.SSID()
                         + String('!'));
          assert(F("Invalid transmission status returned from responseHandler!") && false);
        }
      }

      Serial.println(F("\nPerforming ESP-NOW broadcast."));

      startTime = millis();

      // Remove espnowNode.getMeshName() from the broadcastMetadata below to broadcast to all
      // ESP-NOW nodes regardless of MeshName. Note that data that comes before
      // broadcastMetadataDelimiter should not contain any broadcastMetadataDelimiter characters,
      // otherwise the broadcastFilter function used in this example file will not work.
      String broadcastMetadata = espnowNode.getMeshName() + String(broadcastMetadataDelimiter);
      String broadcastMessage  = String(F("Broadcast #")) + String(requestNumber) + F(" from ")
                                + espnowNode.getMeshName() + espnowNode.getNodeID() + String('.');
      espnowNode.broadcast(broadcastMetadata + broadcastMessage);
      Serial.println(String(F("Broadcast to all mesh nodes done in "))
                     + String(millis() - startTime) + F(" ms."));

      espnowDelay(100);  // Wait for responses (broadcasts can receive an unlimited number of
                         // responses, other transmissions can only receive one response).

      // If you have a data array containing null values it is possible to transmit the raw data by
      // making the array into a multiString as shown below. You can use String::c_str() or
      // String::begin() to retrieve the data array later. Note that certain String methods such as
      // String::substring use null values to determine String length, which means they will not
      // work as normal with multiStrings.
      uint8_t dataArray[]
          = { 0, '\'', 0, '\'', ' ', '(', 'n', 'u', 'l', 'l', ')', ' ', 'v', 'a', 'l', 'u', 'e' };
      String espnowMessage = TypeCast::uint8ArrayToMultiString(dataArray, sizeof dataArray)
                             + F(" from ") + espnowNode.getMeshName() + espnowNode.getNodeID()
                             + String('.');
      Serial.println(String(F("\nTransmitting: ")) + espnowMessage);
      espnowNode.attemptTransmission(espnowMessage, false);
      espnowDelay(100);  // Wait for response.

      Serial.println(F("\nPerforming encrypted ESP-NOW transmissions."));

      uint8_t targetBSSID[6] { 0 };

      // We can create encrypted connections to individual nodes so that all ESP-NOW communication
      // with the node will be encrypted.
      if (espnowNode.constConnectionQueue()[0].getBSSID(targetBSSID)
          && espnowNode.requestEncryptedConnection(targetBSSID)
                 == EncryptedConnectionStatus::CONNECTION_ESTABLISHED) {
        // The WiFi scan will detect the AP MAC, but this will automatically be converted to the
        // encrypted STA MAC by the framework.
        String peerMac = TypeCast::macToString(targetBSSID);

        Serial.println(String(F("Encrypted ESP-NOW connection with ")) + peerMac
                       + F(" established!"));

        // Making a transmission now will cause messages to targetBSSID to be encrypted.
        String espnowMessage
            = String(F("This message is encrypted only when received by node ")) + peerMac;
        Serial.println(String(F("\nTransmitting: ")) + espnowMessage);
        espnowNode.attemptTransmission(espnowMessage, false);
        espnowDelay(100);  // Wait for response.

        // A connection can be serialized and stored for later use.
        // Note that this saves the current state only, so if encrypted communication between the
        // nodes happen after this, the stored state is invalid.
        String serializedEncryptedConnection
            = EspnowMeshBackend::serializeEncryptedConnection(targetBSSID);

        Serial.println();
        // We can remove an encrypted connection like so.
        espnowNode.removeEncryptedConnection(targetBSSID);

        // Note that the peer will still be encrypted, so although we can send unencrypted messages
        // to the peer, we cannot read the encrypted responses it sends back.
        espnowMessage
            = String(F("This message is no longer encrypted when received by node ")) + peerMac;
        Serial.println(String(F("\nTransmitting: ")) + espnowMessage);
        espnowNode.attemptTransmission(espnowMessage, false);
        espnowDelay(100);  // Wait for response.
        Serial.println(F("Cannot read the encrypted response..."));

        // Let's re-add our stored connection so we can communicate properly with targetBSSID again!
        espnowNode.addEncryptedConnection(serializedEncryptedConnection);

        espnowMessage
            = String(F("This message is once again encrypted when received by node ")) + peerMac;
        Serial.println(String(F("\nTransmitting: ")) + espnowMessage);
        espnowNode.attemptTransmission(espnowMessage, false);
        espnowDelay(100);  // Wait for response.

        Serial.println();
        // If we want to remove the encrypted connection on both nodes, we can do it like this.
        EncryptedConnectionRemovalOutcome removalOutcome
            = espnowNode.requestEncryptedConnectionRemoval(targetBSSID);
        if (removalOutcome == EncryptedConnectionRemovalOutcome::REMOVAL_SUCCEEDED) {
          Serial.println(peerMac + F(" is no longer encrypted!"));

          espnowMessage = String(F("This message is only received by node ")) + peerMac
                          + F(". Transmitting in this way will not change the transmission state "
                              "of the sender.");
          Serial.println(String(F("Transmitting: ")) + espnowMessage);
          espnowNode.attemptTransmission(espnowMessage, EspnowNetworkInfo(targetBSSID));
          espnowDelay(100);  // Wait for response.

          Serial.println();

          // Of course, we can also just create a temporary encrypted connection that will remove
          // itself once its duration has passed.
          if (espnowNode.requestTemporaryEncryptedConnection(targetBSSID, 1000)
              == EncryptedConnectionStatus::CONNECTION_ESTABLISHED) {
            espnowDelay(42);
            uint32_t remainingDuration = 0;
            EspnowMeshBackend::getConnectionInfo(targetBSSID, &remainingDuration);

            espnowMessage = String(F("Messages this node sends to ")) + peerMac
                            + F(" will be encrypted for ") + String(remainingDuration)
                            + F(" ms more.");
            Serial.println(String(F("\nTransmitting: ")) + espnowMessage);
            espnowNode.attemptTransmission(espnowMessage, false);

            EspnowMeshBackend::getConnectionInfo(targetBSSID, &remainingDuration);
            espnowDelay(remainingDuration + 100);

            espnowMessage = String(F("Due to encrypted connection expiration, this message is no "
                                     "longer encrypted when received by node "))
                            + peerMac;
            Serial.println(String(F("\nTransmitting: ")) + espnowMessage);
            espnowNode.attemptTransmission(espnowMessage, false);
            espnowDelay(100);  // Wait for response.
          }

          // Or if we prefer we can just let the library automatically create brief encrypted
          // connections which are long enough to transmit an encrypted message. Note that encrypted
          // responses will not be received, unless there already was an encrypted connection
          // established with the peer before attemptAutoEncryptingTransmission was called. This can
          // be remedied via the requestPermanentConnections argument, though it must be noted that
          // the maximum number of encrypted connections supported at a time is 6.
          espnowMessage = F("This message is always encrypted, regardless of receiver.");
          Serial.println(String(F("\nTransmitting: ")) + espnowMessage);
          espnowNode.attemptAutoEncryptingTransmission(espnowMessage);
          espnowDelay(100);  // Wait for response.
        } else {
          Serial.println(String(F("Ooops! Encrypted connection removal failed. Status: "))
                         + String(static_cast<int>(removalOutcome)));
        }

        // Finally, should you ever want to stop other parties from sending unencrypted messages to
        // the node setAcceptsUnencryptedRequests(false); can be used for this. It applies to both
        // encrypted connection requests and regular transmissions.

        Serial.println(F("\n#######################################################################"
                         "#######################"));
      }

      // Our last request was sent to all nodes found, so time to create a new request.
      espnowNode.setMessage(String(F("Hello world request #")) + String(++requestNumber)
                            + F(" from ") + espnowNode.getMeshName() + espnowNode.getNodeID()
                            + String('.'));
    }

    Serial.println();
  }
}
