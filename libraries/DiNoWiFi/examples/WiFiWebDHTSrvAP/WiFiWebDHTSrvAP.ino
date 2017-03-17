// WiFiWebDHTSrvAP.ino
// Company: KMP Electronics Ltd, Bulgaria
// Web: http://kmpelectronics.eu/
// License: See the GNU General Public License for more details at http://www.gnu.org/copyleft/gpl.html
// Supported boards:
//		KMP ProDino WiFi-ESP with ESP8266 V1.1 (http://www.kmpelectronics.eu/en-us/products/prodinowifi-esp.aspx)
// Description:
//		Web server AP DHT example.
// Example link: http://www.kmpelectronics.eu/en-us/examples/prodinowifi-esp/wifiwebdhtserverap.aspx
// Version: 1.0.0
// Date: 05.05.2016
// Author: Plamen Kovandjiev <p.kovandiev@kmpelectronics.eu>

// --------------------------------------------------------------------------------
// Prerequisites:
//		Before start this example you need to install DHT library: https://github.com/adafruit/DHT-sensor-library
//		Connect DHT22 sensor to GROVE connector. Use pins: 
//			- first sensor GROVE_PIN1, Vcc+, Gnd(-);
//			- second sensor GROVE_PIN2, Vcc+, Gnd(-);

#include <KMPDinoWiFiESP.h>
#include <KMPCommon.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DHT.h>

const uint8_t HTTP_PORT = 80;

// Define sensors structure.
typedef struct
{
	// Enable sensor - true.
	bool IsEnable;
	// Name of sensor. Example: "First sensor".
	String Name;
	// DHT object with settings. Example: DHT(GROVE_PIN1 /* connected pin */, DHT22 /* sensor type */, 11 /* Constant for ESP8266 */)
	DHT dht;
	// Store, read humidity from sensor.
	float Humidity;
	// Store, read temperature from sensor.
	float Temperature;
} MeasureHT_t;

// Sensors count. 
#define SENSOR_COUNT 2

// Define array of 2 sensors.
MeasureHT_t _measureHT[SENSOR_COUNT] =
{
	{ true, "Sensor 1", DHT(GROVE_PIN1, DHT22, 11), NAN, NAN },
	{ false, "Sensor 2", DHT(GROVE_PIN2, DHT11, 11), NAN, NAN }
};

// Gray color.
const char GRAY[] = "#808080";
// Check sensor data, interval in milliseconds.
const long CHECK_HT_INTERVAL_MS = 5000;
// Store last measure time.
unsigned long _mesureTimeout;				

const char WI_FI_APPSK[] = "kmp12345";

ESP8266WebServer _server(HTTP_PORT);

/**
 * @brief Execute first after start device. Initialize hardware.
 *
 * @return void
 */
void setup(void)
{
	// You can open the Arduino IDE Serial Monitor window to see what the code is doing
	// Serial connection from ESP-01 via 3.3v console cable
	Serial.begin(115200);

	// Init KMP ProDino WiFi-ESP board.
	KMPDinoWiFiESP.init();

	Serial.println("KMP DHT Server Access Point");

	// Setup WiFi AP.
	WiFi.mode(WIFI_AP);

	// Get a unique name, append the last two bytes of the MAC (HEX'd) to device name.
	String mac = WiFi.softAPmacAddress(); // 5E:CF:7F:81:70:3E
	String apName = "KMP ProDino WiFi-ESP "
		+ mac.substring(12);

	Serial.print("AP name: ");
	Serial.println(apName);

	WiFi.softAP(apName.c_str(), WI_FI_APPSK);

	Serial.print("AP IP address: ");
	Serial.println(WiFi.softAPIP());

	_server.on("/", HandleRootPage);
	_server.begin();

	Serial.println("AP HTTP server started");

	// Start sensors.
	for (uint8_t i = 0; i < SENSOR_COUNT; i++)
	{
		MeasureHT_t* measureHT = &_measureHT[i];

		Serial.print("Sensor name: \"" + measureHT->Name + "\" - ");
		if (measureHT->IsEnable)
		{
			measureHT->dht.begin();
			Serial.println("Start");
		}
		else
		{
			Serial.println("Disable");
		}
	}

	_mesureTimeout = 0;
}

/**
 * @brief Main method.
 *
 * @return void
 */
void loop(void)
{
	_server.handleClient();
	GetDataFromSensors();
}

/**
 * @brief Handle root page "/". 
 *
 * @return void
 */
void HandleRootPage()
{
	//KMPDinoWiFiESP.LedOn();

	_server.send(200, TEXT_HTML, BuildPage());

	//KMPDinoWiFiESP.LedOff();
}

/**
 * @brief Build HTML page.
 *
 * @return void
 */
String BuildPage()
{
	String page =
		"<html><head><title>" + String(KMP_ELECTRONICS_LTD) + " " + String(PRODINO_WIFI) + " - Web DHT AP</title></head>"
		+ "<body><div style='text-align: center'>"
		+ "<br><hr />"
		+ "<h1 style = 'color: #0066FF;'>" + String(PRODINO_WIFI) + " - Web DHT AP example</h1>"
		+ "<hr /><br><br>"
		+ "<table border='1' width='450' cellpadding='5' cellspacing='0' align='center' style='text-align:center; font-size:large; font-family:Arial,Helvetica,sans-serif;'>"
		+ "<thead><tr><th style='width:30%'></th><th style='width:35%'>Temperature C&deg;</th><th>Humidity</th></tr></thead>";

	// Add table rows, relay information.
	String tableBody = "<tbody>";
	for (uint8_t i = 0; i < SENSOR_COUNT; i++)
	{
		// Row i, cell 1
		MeasureHT_t* measureHT = &_measureHT[i];
		tableBody += "<tr><td"+ (measureHT->IsEnable ? "" : " bgcolor='" + String(GRAY) + "'") + ">" + measureHT->Name + "</td>";

		// Cell i,2
		tableBody += "<td>" + FormatMeasure(measureHT->IsEnable, measureHT->Temperature) + "</td>";

		// Cell i,3
		tableBody += "<td>" + FormatMeasure(measureHT->IsEnable, measureHT->Humidity) + "</td></tr>";
	}
	tableBody += "</tbody>";

	return page + tableBody
		+ "</table><br><br><hr /><h1><a href='" + String(URL_KMPELECTRONICS_EU) + "' target='_blank'>Visit " + String(KMP_ELECTRONICS_LTD) + "</a></h1>"
		+ "<h3><a href='" + String(URL_KMPELECTRONICS_EU_DINO_WIFI) + "' target='_blank'>Information about " + String(PRODINO_WIFI) + " board</a></h3>"
		+ "<hr /></div></body></html>";
}

/**
 * @brief Prepare sensor result.
 *
 * @return void
 */
String FormatMeasure(bool isEnable, float val)
{
	return isEnable ? String(val) : "-";
}

/**
 * @brief Read data from sensors a specified time.
 *
 * @return void
 */
void GetDataFromSensors()
{
	if (millis() > _mesureTimeout)
	{
		for (uint8_t i = 0; i < SENSOR_COUNT; i++)
		{
			// Get sensor structure.
			MeasureHT_t* measureHT = &_measureHT[i];
			// Is enable - read data from sensor.
			if (measureHT->IsEnable)
			{
				measureHT->dht.read(true);
				measureHT->Humidity = measureHT->dht.readHumidity();
				measureHT->Temperature = measureHT->dht.readTemperature();
			}
		}

		// Set next time to read data.
		_mesureTimeout = millis() + CHECK_HT_INTERVAL_MS;
	}
}