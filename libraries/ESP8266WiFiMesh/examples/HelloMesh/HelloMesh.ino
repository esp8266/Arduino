#include <ESP8266WiFi.h>
#include <ESP8266WiFiMesh.h>

unsigned int request_i = 0;
unsigned int response_i = 0;

String manageRequest(String request);
void manageResponse(String response);

/* Create the mesh node object */
ESP8266WiFiMesh mesh_node = ESP8266WiFiMesh(ESP.getChipId(), manageRequest, manageResponse, true);

/**
 * Callback for when other nodes send you a request
 *
 * @request The request string received from another node in the mesh
 * @returns The string to send back to the other node
 */
String manageRequest(String request)
{
  /* Print out received message */
  Serial.print("Request received: ");
  Serial.println(request);

  /* return a string to send back */
  char response[60];
  sprintf(response, "Hello world response #%d from Mesh_Node%d.", response_i++, ESP.getChipId());
  return response;
}

/**
 * Callback for when you get a response from other nodes
 *
 * @response The response string received from another node in the mesh
 */
void manageResponse(String response)
{
  /* Print out received message */
  Serial.print("Response received: ");
  Serial.println(response);
  
  request_i++; // Our last request got a response, so time to create a new request.
}

void setup()
{
  // Prevents the flash memory from being worn out, see: https://github.com/esp8266/Arduino/issues/1054 . 
  // This will however delay node WiFi start-up by about 700 ms. The delay is 900 ms if we otherwise would have stored the WiFi network we want to connect to.
  WiFi.persistent(false);
  
  Serial.begin(115200);
  delay(50); // Wait for Serial.

  //yield(); // Use this if you don't want to wait for Serial.
  
  Serial.println();
  Serial.println();
  
  Serial.println("Note that this library uses static IP:s for the nodes to speed up initial connection.\n"
                  "Please use the setStaticIP method to ensure that nodes connecting to the same AP have distinct static IP:s.\n"
                  "Also, remember to change the default mesh network password!\n\n");

  Serial.println("Please use lwIP v1.4 with this library for now (can be changed in the Tools menu of Arduino IDE).\n" 
                  "Nodes seem to be unable to connect to more than one other node when using lwIP 2.0!\n\n");
                 
  Serial.println("Setting up mesh node...");

  /* Initialise the mesh node */
  mesh_node.setStaticIP(IPAddress(192,168,4,22));
  mesh_node.begin();
}

int32_t time_of_last_scan = -10000;
void loop()
{
  if(millis() - time_of_last_scan > 3000 // Give other nodes some time to connect between data transfers.
  || (WiFi.status() != WL_CONNECTED && millis() - time_of_last_scan > 1000)) // Scan for networks once per second when not already connected.
  {
    /* 
    * attemptTransmission(request) will either scan for other nodes and send the request to the first node it connects to
    * or if connection to an AP already exists just send the request to that AP.
    */
    char request[60];
    sprintf(request, "Hello world request #%d from Mesh_Node%d.", request_i, ESP.getChipId());
    mesh_node.attemptTransmission(request);
    time_of_last_scan = millis();
  }
  else
  {
    /* Accept any incoming connections */
    mesh_node.acceptRequest();
  }
}
