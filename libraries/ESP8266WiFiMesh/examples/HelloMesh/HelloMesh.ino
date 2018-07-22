#include <ESP8266WiFi.h>
#include <ESP8266WiFiMesh.h>

String mesh_name = "Mesh_Node";

unsigned int request_i = 0;
unsigned int response_i = 0;

String manageRequest(String request, ESP8266WiFiMesh *mesh_instance);
transmission_status_t manageResponse(String response, ESP8266WiFiMesh *mesh_instance);
void networkFilter(int number_of_networks, ESP8266WiFiMesh *mesh_instance);

/* Create the mesh node object */
ESP8266WiFiMesh mesh_node = ESP8266WiFiMesh(manageRequest, manageResponse, networkFilter, "ChangeThisWiFiPassword_TODO", mesh_name, "", true);

/**
   Callback for when other nodes send you a request

   @param request The request string received from another node in the mesh
   @param mesh_instance The "this" pointer of the ESP8266WiFiMesh instance that called the function.
   @returns The string to send back to the other node
*/
String manageRequest(String request, ESP8266WiFiMesh *mesh_instance) {
  /* Print out received message */
  Serial.print("Request received: ");
  Serial.println(request);

  /* return a string to send back */
  char response[60];
  sprintf(response, "Hello world response #%d from %s%s.", response_i++, mesh_instance->getMeshName().c_str(), mesh_instance->getNodeID().c_str());
  return response;
}

/**
   Callback used to decide which networks to connect to once a WiFi scan has been completed.

   @param number_of_networks The number of networks found in the WiFi scan.
   @param mesh_instance The "this" pointer of the ESP8266WiFiMesh instance that called the function.
*/
void networkFilter(int number_of_networks, ESP8266WiFiMesh *mesh_instance) {
  for (int i = 0; i < number_of_networks; ++i) {
    String current_ssid = WiFi.SSID(i);
    int mesh_name_index = current_ssid.indexOf(mesh_instance->getMeshName());

    /* Connect to any _suitable_ APs which contain mesh_instance->getMeshName() */
    if (mesh_name_index >= 0) {
      uint64_t target_node_id = ESP8266WiFiMesh::StringToUint64(current_ssid.substring(mesh_name_index + mesh_instance->getMeshName().length()));

      if (target_node_id < ESP8266WiFiMesh::StringToUint64(mesh_instance->getNodeID())) {
        ESP8266WiFiMesh::connection_queue.push_back(NetworkInfo(i));
      }
    }
  }
}

/**
   Callback for when you get a response from other nodes

   @param response The response string received from another node in the mesh
   @param mesh_instance The "this" pointer of the ESP8266WiFiMesh instance that called the function.
   @returns The status code resulting from the response, as an int
*/
transmission_status_t manageResponse(String response, ESP8266WiFiMesh *mesh_instance) {
  transmission_status_t status_code = TS_TRANSMISSION_COMPLETE;

  /* Print out received message */
  Serial.print("Request sent: ");
  Serial.println(mesh_instance->getMessage());
  Serial.print("Response received: ");
  Serial.println(response);

  // Our last request got a response, so time to create a new request.
  mesh_instance->setMessage("Hello world request #" + String(++request_i) + " from " + mesh_instance->getMeshName() + mesh_instance->getNodeID() + ".");

  // (void)mesh_instance; // This is useful to remove a "unused parameter" compiler warning. Does nothing else.
  return status_code;
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
                 "Use the setStaticIP method to enable this and ensure that nodes connecting to the same AP have distinct static IP:s.\n"
                 "Also, remember to change the default mesh network password!\n\n");

  Serial.println("Setting up mesh node...");

  /* Initialise the mesh node */
  mesh_node.begin();
  mesh_node.activateAP(); // Each AP requires a separate server port.
  mesh_node.setStaticIP(IPAddress(192, 168, 4, 22)); // Activate static IP mode to speed up connection times.
}

int32_t time_of_last_scan = -10000;
void loop() {
  if (millis() - time_of_last_scan > 3000 // Give other nodes some time to connect between data transfers.
      || (WiFi.status() != WL_CONNECTED && millis() - time_of_last_scan > 2000)) { // Scan for networks with two second intervals when not already connected.
    char request[60];
    sprintf(request, "Hello world request #%d from %s%s.", request_i, mesh_node.getMeshName().c_str(), mesh_node.getNodeID().c_str());
    mesh_node.attemptTransmission(request, false);
    time_of_last_scan = millis();

    if (ESP8266WiFiMesh::latest_transmission_outcomes.empty()) {
      Serial.println("No mesh AP found.");
    } else {
      for (TransmissionResult &transmission_result : ESP8266WiFiMesh::latest_transmission_outcomes) {
        if (transmission_result.transmission_status == TS_TRANSMISSION_FAILED) {
          Serial.println("Transmission failed to mesh AP " + transmission_result.ssid);
        } else if (transmission_result.transmission_status == TS_CONNECTION_FAILED) {
          Serial.println("Connection failed to mesh AP " + transmission_result.ssid);
        }
      }
    }
    Serial.println();
  } else {
    /* Accept any incoming connections */
    mesh_node.acceptRequest();
  }
}
