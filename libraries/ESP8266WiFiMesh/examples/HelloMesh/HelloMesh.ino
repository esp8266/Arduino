#include <ESP8266WiFi.h>
#include <ESP8266WiFiMesh.h>

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
	char message[50];
	sprintf(message, "Hello world response from %d.", ESP.getChipId());
	return message;
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
	char message[50];
	sprintf(message, "Hello world request from %d.", ESP.getChipId());
	mesh_node.attemptScan(message);
	delay(1000);
}
