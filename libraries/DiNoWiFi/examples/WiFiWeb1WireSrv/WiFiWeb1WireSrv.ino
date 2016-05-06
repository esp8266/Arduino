// WiFiWeb1WireSrv.ino
// Company: KMP Electronics Ltd, Bulgaria
// Web: http://kmpelectronics.eu/
// License: See the GNU General Public License for more details at http://www.gnu.org/copyleft/gpl.html
// Supported boards:
//		KMP ProDino WiFi-ESP with ESP8266 V1.1 (http://www.kmpelectronics.eu/en-us/products/prodinowifi-esp.aspx)
// Description:
//		Web server 1 Wire measure temperature with DS18B20 example.
// Example link: http://www.kmpelectronics.eu/en-us/examples/prodinowifi-esp/wifiwebdhtserver.aspx
// Version: 1.0.0
// Date: 05.05.2016
// Author: Plamen Kovandjiev <p.kovandiev@kmpelectronics.eu>

// --------------------------------------------------------------------------------
// Prerequisites:
//		Before start this example you need to install folowing libraries:
//			- One wire: https://github.com/PaulStoffregen/OneWire
//			- DallasTemperature library: https://github.com/milesburton/Arduino-Temperature-Control-Library
//		Connect DS18B20 sensor(s) to GROVE connector. Use pins: 
//			- GROVE_PIN1, Vcc+, Gnd(-);

#include <KMPDinoWiFiESP.h>
#include <KMPCommon.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

// One Wire headers
#include <OneWire.h>
#include <DallasTemperature.h>

const char SSID[] = "******";
const char PASSWORD[] = "******";
const uint8_t HTTP_PORT = 80;

const char WHITE[] = "white";
const char BLUE[] = "blue";
const char GREEN[] = "green";
const char RED[] = "red";
const char GRAY[] = "#808080";

const char NA[] = "N/A";
const char DEGREE_SYMBOL[] = "&deg;";

// Thermometer Resolution in bits. http://datasheets.maximintegrated.com/en/ds/DS18B20.pdf page 8. 
// Bits - CONVERSION TIME. 9 - 93.75ms, 10 - 187.5ms, 11 - 375ms, 12 - 750ms. 
#define TEMPERATURE_PRECISION 9

#define SENSORS_PIN GROVE_PIN1

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire _oneWire(SENSORS_PIN);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature _sensors(&_oneWire);

// Number of one wire temperature devices found.
int _oneWireDeviceCount;

// Temp device address.
DeviceAddress _tempDeviceAddress;

// Check sensor data, interval in milliseconds.
const long CHECK_HT_INTERVAL_MS = 5000;
// Store last measure time.
unsigned long _mesureTimeout;				
// Get device count after N time measure.
const uint8_t GET_DEVICE_COUNT_AFTER = 10;

uint8_t _getDeviceCount;

// Buffer to Hex bytes.
char _buffer[8*2 + 1];

ESP8266WebServer _server(HTTP_PORT);

// If in debug mode - print debug information in Serial. Comment in production code, this bring performance.
// This method is good for development and verification of results. But increases the amount of code and decreases productivity.
#define DEBUG

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

	// Connect to WiFi network
	WiFi.begin(SSID, PASSWORD);
	Serial.print("\n\r \n\rWorking to connect");

	// Wait for connection
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}
	Serial.println("");
	Serial.println("KMP 1 Wire Server");
	Serial.print("Connected to ");
	Serial.println(SSID);
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());

	_server.on("/", HandleRootPage);
	_server.begin();

	Serial.println("HTTP server started");

	_mesureTimeout = 0;

	// Force get device.
	_getDeviceCount = GET_DEVICE_COUNT_AFTER;

	// Start the One Wire library.
	_sensors.begin();

	// Set precision.
	_sensors.setResolution(10/*TEMPERATURE_PRECISION*/);

	// Select available connected to board One Wire devices.
	GethOneWireDevices();
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
		"<html><head><title>" + String(KMP_ELECTRONICS_LTD) + " " + String(PRODINO_WIFI) + " - Web Relay</title></head>"
		+ "<body><div style='text-align: center'>"
		+ "<br><hr />"
		+ "<h1 style = 'color: #0066FF;'>" + String(PRODINO_WIFI) + " - Web 1Wire example</h1>"
		+ "<hr /><br><br>"
		+ "<table border='1' width='450' cellpadding='5' cellspacing='0' align='center' style='text-align:center; font-size:large; font-family:Arial,Helvetica,sans-serif;'>"
		+ "<thead><tr><th></th><th>C&deg;</th><th>F&deg;</th></tr></thead>";

	// Add table rows.
	String tableBody = "<tbody>";
	for (int i = 0; i < _oneWireDeviceCount; i++)
	{
		// Default color.
		const char* cellColor = GRAY;
		float temp = 0;
		bool sensorAvaible = _sensors.getAddress(_tempDeviceAddress, i);
		String sensorName = "Ghost device";
		String sensorId = NA;
		if (sensorAvaible)
		{
			// Get temperature in Celsius.
			temp = _sensors.getTempC(_tempDeviceAddress);

#ifdef DEBUG
			Serial.print("Device ");
			Serial.print(i);
			Serial.print(" with address: ");
			PrintAddress(_tempDeviceAddress);
			Serial.print("Temperature in C: ");
			Serial.println(temp);
#endif

			// Select cell background.
			if (0.0 >= temp)
			{
				cellColor = BLUE;
			}
			else if (22.0 >= temp)
			{
				cellColor = GREEN;
			}
			else
			{
				cellColor = RED;
			}

			sensorName = "Sensor " + String(IntToChar(i + 1));
			BytesToHexStr(_tempDeviceAddress, 8, _buffer);
			sensorId = _buffer;
		}
		
		// Row i, cell 1
		tableBody += "<tr><td>" + sensorName + "<br><font size='2'>Id: " + sensorId + "</font></td>";

		// Add cell i,2
		tableBody += AddTemperatureCell(sensorAvaible, temp, cellColor);

		// Add cell i,3
		tableBody += AddTemperatureCell(sensorAvaible, _sensors.toFahrenheit(temp), cellColor);

		// End row.
		tableBody += "</tr>";
	}

	tableBody += "</tbody>";

	return page + tableBody
		+ "</table><br><br><hr /><h1><a href='" + String(URL_KMPELECTRONICS_EU) + "' target='_blank'>Visit " + String(KMP_ELECTRONICS_LTD) + "</a></h1>"
		+ "<h3><a href='" + String(URL_KMPELECTRONICS_EU_DINO_WIFI) + "' target='_blank'>Information about " + String(PRODINO_WIFI) + " board</a></h3>"
		+ "<hr /></div></body></html>";
}

/**
* @brief Add cell in table, include temperature data.
*
* @param sensorAvaible Sensor available. If true - available, else not available.
* @param temperature Temperature.
* @param cellColor Cell background color in text.
*
* \return String formated cell.
*/
String AddTemperatureCell(bool sensorAvaible, double temperature, const char* cellColor)
{
	String result = "<td bgcolor='" + String(cellColor) + "'>";
	if (sensorAvaible)
	{
		FloatToChars(temperature, 1, _buffer);
		result += String(_buffer);
	}
	else
	{
		result += String(NA);
	}
	
	result += "</td>";

	return result;
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
		_getDeviceCount++;
		
		if (_getDeviceCount > GET_DEVICE_COUNT_AFTER)
		{
			GethOneWireDevices();
			
			_getDeviceCount = 0;
		}

		// Set next time to read data.
		_mesureTimeout = millis() + CHECK_HT_INTERVAL_MS;
	}
}

/**
 * @brief Get all available One Wire devices.
 *
 *
 * @return void
 */
void GethOneWireDevices()
{
	// Grab a count of devices on the wire.
	_oneWireDeviceCount = _sensors.getDeviceCount();

#ifdef DEBUG
	Serial.print("Number of One Wire devices: ");
	Serial.println(_oneWireDeviceCount);

	// Loop through each device, print out address
	for (int i = 0; i < _oneWireDeviceCount; i++)
	{
		// Search the wire for address
		if (_sensors.getAddress(_tempDeviceAddress, i))
		{
			Serial.print("Device ");
			Serial.print(i);
			Serial.print(" with address: ");
			PrintAddress(_tempDeviceAddress);
		}
		else
		{
			Serial.print("Found ghost device at ");
			Serial.print(i);
			Serial.print(" but could not detect address. Check power and cabling");
		}
	}
#endif
}

#ifdef DEBUG
/**
 * @brief Print device address to Serial.
 *
 * @param deviceAddress Device address.
 *
 * @return void
 */
void PrintAddress(DeviceAddress deviceAddress)
{
	BytesToHexStr(deviceAddress, 8, _buffer);
	Serial.println(_buffer);
}
#endif