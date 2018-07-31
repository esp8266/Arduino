#include <ESP8266WiFi.h>
#include <ESP8266WiFiMesh.h>

String exampleMeshName("MeshNode_");

unsigned int requestNumber = 0;
unsigned int responseNumber = 0;

String manageRequest(const String &request, ESP8266WiFiMesh &meshInstance);
transmission_status_t manageResponse(const String &response, ESP8266WiFiMesh &meshInstance);
void networkFilter(int numberOfNetworks, ESP8266WiFiMesh &meshInstance);

/* Create the mesh node object */
ESP8266WiFiMesh meshNode = ESP8266WiFiMesh(manageRequest, manageResponse, networkFilter, "ChangeThisWiFiPassword_TODO", exampleMeshName, "", true);

/**
   Callback for when other nodes send you a request

   @param request The request string received from another node in the mesh
   @param meshInstance The ESP8266WiFiMesh instance that called the function.
   @returns The string to send back to the other node
*/
String manageRequest(const String &request, ESP8266WiFiMesh &meshInstance) {
  /* Print out received message */
  Serial.print("Request received: ");
  Serial.println(request);

  /* return a string to send back */
  return ("Hello world response #" + String(responseNumber++) + " from " + meshInstance.getMeshName() + meshInstance.getNodeID() + ".");
}

/**
   Callback used to decide which networks to connect to once a WiFi scan has been completed.

   @param numberOfNetworks The number of networks found in the WiFi scan.
   @param meshInstance The ESP8266WiFiMesh instance that called the function.
*/
void networkFilter(int numberOfNetworks, ESP8266WiFiMesh &meshInstance) {
  for (int i = 0; i < numberOfNetworks; ++i) {
    String currentSSID = WiFi.SSID(i);
    int meshNameIndex = currentSSID.indexOf(meshInstance.getMeshName());

    /* Connect to any _suitable_ APs which contain meshInstance.getMeshName() */
    if (meshNameIndex >= 0) {
      uint64_t targetNodeID = ESP8266WiFiMesh::stringToUint64(currentSSID.substring(meshNameIndex + meshInstance.getMeshName().length()));

      if (targetNodeID < ESP8266WiFiMesh::stringToUint64(meshInstance.getNodeID())) {
        ESP8266WiFiMesh::connectionQueue.push_back(NetworkInfo(i));
      }
    }
  }
}

/**
   Callback for when you get a response from other nodes

   @param response The response string received from another node in the mesh
   @param meshInstance The ESP8266WiFiMesh instance that called the function.
   @returns The status code resulting from the response, as an int
*/
transmission_status_t manageResponse(const String &response, ESP8266WiFiMesh &meshInstance) {
  transmission_status_t statusCode = TS_TRANSMISSION_COMPLETE;

  /* Print out received message */
  Serial.print("Request sent: ");
  Serial.println(meshInstance.getMessage());
  Serial.print("Response received: ");
  Serial.println(response);

  // Our last request got a response, so time to create a new request.
  meshInstance.setMessage("Hello world request #" + String(++requestNumber) + " from " + meshInstance.getMeshName() + meshInstance.getNodeID() + ".");

  // (void)meshInstance; // This is useful to remove a "unused parameter" compiler warning. Does nothing else.
  return statusCode;
}

void setup() {
  // Prevents the flash memory from being worn out, see: https://github.com/esp8266/Arduino/issues/1054 .
  // This will however delay node WiFi start-up by about 700 ms. The delay is 900 ms if we otherwise would have stored the WiFi network we want to connect to.
  WiFi.persistent(false);

  Serial.begin(115200);
  delay(50); // Wait for Serial.

  //yield(); // Use this if you don't want to wait for Serial.

  Serial.println();
  Serial.println();

  Serial.println("Note that this library can use static IP:s for the nodes to speed up connection times.\n"
                 "Use the setStaticIP method as shown in this example to enable this.\n"
                 "Ensure that nodes connecting to the same AP have distinct static IP:s.\n"
                 "Also, remember to change the default mesh network password!\n\n");

  Serial.println("Setting up mesh node...");

  /* Initialise the mesh node */
  meshNode.begin();
  meshNode.activateAP(); // Each AP requires a separate server port.
  meshNode.setStaticIP(IPAddress(192, 168, 4, 22)); // Activate static IP mode to speed up connection times.
}

int32_t timeOfLastScan = -10000;
void loop() {
  if (millis() - timeOfLastScan > 3000 // Give other nodes some time to connect between data transfers.
      || (WiFi.status() != WL_CONNECTED && millis() - timeOfLastScan > 2000)) { // Scan for networks with two second intervals when not already connected.
    String request = "Hello world request #" + String(requestNumber) + " from " + meshNode.getMeshName() + meshNode.getNodeID() + ".";
    meshNode.attemptTransmission(request, false);
    timeOfLastScan = millis();

    if (ESP8266WiFiMesh::latestTransmissionOutcomes.empty()) {
      Serial.println("No mesh AP found.");
    } else {
      for (TransmissionResult &transmissionResult : ESP8266WiFiMesh::latestTransmissionOutcomes) {
        if (transmissionResult.transmissionStatus == TS_TRANSMISSION_FAILED) {
          Serial.println("Transmission failed to mesh AP " + transmissionResult.SSID);
        } else if (transmissionResult.transmissionStatus == TS_CONNECTION_FAILED) {
          Serial.println("Connection failed to mesh AP " + transmissionResult.SSID);
        } else if (transmissionResult.transmissionStatus == TS_TRANSMISSION_COMPLETE) {
          // No need to do anything, transmission was successful.
        } else {
          Serial.println("Invalid transmission status for " + transmissionResult.SSID + "!");
        }
      }
    }
    Serial.println();
  } else {
    /* Accept any incoming connections */
    meshNode.acceptRequest();
  }
}
