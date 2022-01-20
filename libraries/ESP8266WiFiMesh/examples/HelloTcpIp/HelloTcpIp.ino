#define ESP8266WIFIMESH_DISABLE_COMPATIBILITY  // Excludes redundant compatibility code. TODO:
                                               // Should be used for new code until the
                                               // compatibility code is removed with release 3.0.0
                                               // of the Arduino core.

#include <ESP8266WiFi.h>
#include <TcpIpMeshBackend.h>
#include <EspnowMeshBackend.h>
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
constexpr char exampleMeshName[] PROGMEM = "MeshNode_";
constexpr char exampleWiFiPassword[] PROGMEM
    = "ChangeThisWiFiPassword_TODO";  // Note: " is an illegal character. The password has to be min
                                      // 8 and max 64 characters long, otherwise an AP which uses it
                                      // will not be found during scans.

unsigned int requestNumber  = 0;
unsigned int responseNumber = 0;

String                 manageRequest(const String& request, MeshBackendBase& meshInstance);
TransmissionStatusType manageResponse(const String& response, MeshBackendBase& meshInstance);
void                   networkFilter(int numberOfNetworks, MeshBackendBase& meshInstance);

/* Create the mesh node object */
TcpIpMeshBackend tcpIpNode
    = TcpIpMeshBackend(manageRequest, manageResponse, networkFilter, FPSTR(exampleWiFiPassword),
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
  Serial.println(request.substring(0, 100));

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
   Once passed to the setTransmissionOutcomesUpdateHook method of the TCP/IP backend,
   this function will be called after each update of the latestTransmissionOutcomes vector during
   attemptTransmission. (which happens after each individual transmission has finished)

   Example use cases is modifying getMessage() between transmissions, or aborting
   attemptTransmission before all nodes in the connectionQueue have been contacted.

   @param meshInstance The MeshBackendBase instance that called the function.

   @return True if attemptTransmission should continue with the next entry in the connectionQueue.
   False if attemptTransmission should stop.
*/
bool exampleTransmissionOutcomesUpdateHook(MeshBackendBase& meshInstance) {
  // The default hook only returns true and does nothing else.

  if (TcpIpMeshBackend* tcpIpInstance
      = TypeCast::meshBackendCast<TcpIpMeshBackend*>(&meshInstance)) {
    if (tcpIpInstance->latestTransmissionOutcomes().back().transmissionStatus()
        == TransmissionStatusType::TRANSMISSION_COMPLETE) {
      // Our last request got a response, so time to create a new request.
      meshInstance.setMessage(String(F("Hello world request #")) + String(++requestNumber)
                              + F(" from ") + meshInstance.getMeshName() + meshInstance.getNodeID()
                              + String('.'));
    }
  } else {
    Serial.println(F("Invalid mesh backend!"));
  }

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
      F("Note that this library can use static IP:s for the nodes to speed up connection times.\n"
        "Use the setStaticIP method as shown in this example to enable this.\n"
        "Ensure that nodes connecting to the same AP have distinct static IP:s.\n"
        "Also, remember to change the default mesh network password!\n\n"));

  Serial.println(F("Setting up mesh node..."));

  /* Initialise the mesh node */
  tcpIpNode.begin();
  tcpIpNode.activateAP();  // Each AP requires a separate server port.
  tcpIpNode.setStaticIP(
      IPAddress(192, 168, 4, 22));  // Activate static IP mode to speed up connection times.

  // Storing our message in the TcpIpMeshBackend instance is not required, but can be useful for
  // organizing code, especially when using many TcpIpMeshBackend instances. Note that calling the
  // multi-recipient tcpIpNode.attemptTransmission will replace the stored message with whatever
  // message is transmitted.
  tcpIpNode.setMessage(String(F("Hello world request #")) + String(requestNumber) + F(" from ")
                       + tcpIpNode.getMeshName() + tcpIpNode.getNodeID() + String('.'));

  tcpIpNode.setTransmissionOutcomesUpdateHook(exampleTransmissionOutcomesUpdateHook);
}

int32_t timeOfLastScan = -10000;
void    loop() {
  if (millis() - timeOfLastScan
          > 3000  // Give other nodes some time to connect between data transfers.
      || (WiFi.status() != WL_CONNECTED
          && millis() - timeOfLastScan > 2000)) {  // Scan for networks with two second intervals
                                                   // when not already connected.

    // attemptTransmission(message, scan, scanAllWiFiChannels, concludingDisconnect,
    // initialDisconnect = false)
    tcpIpNode.attemptTransmission(tcpIpNode.getMessage(), true, false, false);
    timeOfLastScan = millis();

    // One way to check how attemptTransmission worked out
    if (tcpIpNode.latestTransmissionSuccessful()) {
      Serial.println(F("Transmission successful."));
    }

    // Another way to check how attemptTransmission worked out
    if (tcpIpNode.latestTransmissionOutcomes().empty()) {
      Serial.println(F("No mesh AP found."));
    } else {
      for (TransmissionOutcome& transmissionOutcome : tcpIpNode.latestTransmissionOutcomes()) {
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
    }
    Serial.println();
  } else {
    /* Accept any incoming connections */
    tcpIpNode.acceptRequests();
  }
}
