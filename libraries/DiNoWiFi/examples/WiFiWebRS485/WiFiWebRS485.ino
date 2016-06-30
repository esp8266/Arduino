// WiFiWebRS485.ino
// Company: KMP Electronics Ltd, Bulgaria
// Web: http://kmpelectronics.eu/
// License: See the GNU General Public License for more details at http://www.gnu.org/copyleft/gpl.html
// Supported boards:
//		KMP ProDino WiFi-ESP with ESP8266 V1.1 (http://www.kmpelectronics.eu/en-us/products/prodinowifi-esp.aspx)
// Description:
//		Web server RS485 example. 
// Example link: http://www.kmpelectronics.eu/en-us/examples/dinowifiesp/wifiwebrelayserver.aspx
// Version: 1.0.0
// Date: 26.06.2016
// Author: Plamen Kovandjiev <p.kovandiev@kmpelectronics.eu>

// --------------------------------------------------------------------------------
// Prerequisites:
//		Before start this example you need:
//		Connect RS485 (make echo, and configured 19200, 8N1) a device in ProDino RS485 port.
// Attention:
//		The Serial (debug port) and RS485 port is same.

#include <KMPDinoWiFiESP.h>
#include <KMPCommon.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

//#define DEBUG

const char SSID[] = "******";
const char PASSWORD[] = "******";
const uint8_t HTTP_PORT = 80;

ESP8266WebServer _server(HTTP_PORT);

// If Post request is valid, read data from RS485.
bool _isValidPost = false;

/**
* @brief Execute first after start device. Initialize hardware.
*
* @return void
*/
void setup()
{
	// Start RS485 with boud 19200 and 8N1.
	KMPDinoWiFiESP.RS485Begin(19200);
//#ifdef DEBUG
//	// You can open the Arduino IDE Serial Monitor window to see what the code is doing
//	Serial.begin(19200);
//#endif
	// Init KMP Dino WiFi board.
	KMPDinoWiFiESP.init();

	// Connect to WiFi network
	WiFi.begin(SSID, PASSWORD);
#ifdef DEBUG
	Serial.print("\n\r \n\rWorking to connect");
#endif

	// Wait for connection
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
#ifdef DEBUG
		Serial.print(".");
#endif
	}
#ifdef DEBUG
	Serial.println("");
	Serial.println("KMP RS485 Server");
	Serial.print("Connected to ");
	Serial.println(SSID);
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());
#endif

	_server.on("/", HandleRootPage);
	_server.begin();

#ifdef DEBUG
	Serial.println("HTTP server started");
#endif
}


/**
* @brief Main method.
*
* @return void
*/
void loop(void)
{
	_server.handleClient();
}

/**
* @brief Handle root page "/".
*
* @return void
*/
void HandleRootPage()
{
	_isValidPost = false;

#ifdef DEBUG
	Serial.println("Request send.");
#endif

	// Read POST request. data=Test&btn=Transmit
	if (_server.method() == HTTP_POST && _server.args() == 2)
	{
		_isValidPost = true;

		// Get value first argument.
		String argValue = _server.arg(0);

#ifdef DEBUG
		Serial.println("Data to send:");
		Serial.println(argValue);
#endif
		// Send data with RS485.
		KMPDinoWiFiESP.RS485Write((char *)argValue.c_str());
	}

	_server.send(200, TEXT_HTML, BuildPage());
}

/**
* @brief Build HTML page.
*
* @return void
*/
String BuildPage()
{
	String page =
		"<html><head><title>" + String(KMP_ELECTRONICS_LTD) + " " + String(PRODINO_WIFI) + " - RS485</title></head>"
		+ "<body><div style='text-align: center'>"
		+ "<br><hr />"
		+ "<h1 style = 'color: #0066FF;'>" + String(PRODINO_WIFI) + " - RS485 example</h1>"
		+ "<hr /><br><br>"
		+ "<form method='post'>"
		+ "<table border='1' width='300' cellpadding='5' cellspacing='0' align='center' style='text-align:center; font-size:large; font-family:Arial,Helvetica,sans-serif;'>"
		+ "<thead><tr><th width='80%'>Data</th><th>Action</th></tr></thead>"
		+ "<tbody><tr><td><input type='text' name='data' style='width: 100%'></td>"
		+ "<td><input type='submit' name='btn' value='Transmit'/></td></tr>"
		+ "<tr><td>";

	if (_isValidPost)
	{
#ifdef DEBUG
		Serial.println("Read data from RS485.");
		int readBytes = 0;
#endif
		// Read RS485.
		int i;

		// if i = -1 not data to read.
		while ((i = KMPDinoWiFiESP.RS485Read()) > -1)
		{
#ifdef DEBUG
			Serial.write((char)i);
			readBytes++;
#endif
			// Convert unsigned char to char.
			page += (char)i;
		}
#ifdef DEBUG
		Serial.println();
		Serial.print("Bytes read: ");
		Serial.println(readBytes);
#endif
	}

	return page 
		+ "</td><td>Received</td></tr></tbody>"
		+ "</table></form>"
		+ "</table></form><br><br><hr /><h1><a href='" + String(URL_KMPELECTRONICS_EU) + "' target='_blank'>Visit " + String(KMP_ELECTRONICS_LTD) + "</a></h1>"
		+ "<h3><a href='" + String(URL_KMPELECTRONICS_EU_DINO_WIFI) + "' target='_blank'>Information about " + String(PRODINO_WIFI) + " board</a></h3>"
		+ "<hr /></div></body></html>";
}