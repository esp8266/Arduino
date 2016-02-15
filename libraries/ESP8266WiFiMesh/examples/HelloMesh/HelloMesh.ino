#include <ESP8266WiFi.h>
#include <ESP8266WiFiMesh.h>

unsigned int request_i = 0;
unsigned int response_i = 0;

String manageRequest(String request);

/* Create the mesh node object */
ESP8266WiFiMesh mesh_node = ESP8266WiFiMesh(ESP.getChipId(), manageRequest);

/**
 * Callback for when other nodes send you data
 *
 * @request The string received from another node in the mesh
 * @returns The string to send back to the other node
 */
String manageRequest(String request)
{
	/* Print out received message */
	Serial.print("received: ");
	Serial.println(request);

	/* return a string to send back */
	char response[60];
	sprintf(response, "Hello world response #%d from Mesh_Node%d.", response_i++, ESP.getChipId());
	return response;
}

void setup()
{
	Serial.begin(115200);
	delay(10);

	Serial.println();
	Serial.println();
	Serial.println("Setting up mesh node...");

	/* Initialise the mesh node */
	mesh_node.begin();
}

void loop()
{
	/* Accept any incoming connections */
	mesh_node.acceptRequest();

	/* Scan for other nodes and send them a message */
	char request[60];
	sprintf(request, "Hello world request #%d from Mesh_Node%d.", request_i++, ESP.getChipId());
	mesh_node.attemptScan(request);
	delay(1000);
}
