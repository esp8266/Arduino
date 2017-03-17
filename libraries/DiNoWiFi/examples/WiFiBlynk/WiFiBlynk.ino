// WiFiBlynk.ino
// Company: KMP Electronics Ltd, Bulgaria
// Web: http://kmpelectronics.eu/
// License: See the GNU General Public License for more details at http://www.gnu.org/copyleft/gpl.html
// Supported boards:
//		KMP ProDino WiFi-ESP with ESP8266 V1.1 (http://www.kmpelectronics.eu/en-us/products/prodinowifi-esp.aspx)
// Description:
//		Blynk example. For this example need add in Blynk mobile application 4 button (200), 1 LCD Display(400). All 1200 points.
// Example link: http://www.kmpelectronics.eu/en-us/examples/prodinowifi-esp/wifiblynk.aspx
// Version: 1.0.0
// Date: 10.05.2016
// Author: Plamen Kovandjiev <p.kovandiev@kmpelectronics.eu>

// --------------------------------------------------------------------------------
// Prerequisites:
//	Before start this example you need to install:
//		Install Blynk library: Sketch\Include library\Menage Libraries... find Blynk and click Install.
//		DHT library: https://github.com/adafruit/DHT-sensor-library
//		Connect DHT22 sensor to GROVE connector. Use pins: 
//			- first sensor GROVE_PIN1, Vcc+, Gnd(-);
//			- second sensor GROVE_PIN2, Vcc+, Gnd(-);
//  Pin maps PRODINo WiFi-ESP -> Blynk:
//		OptoIn and DHT data -> V0 {Type: "LCD", Mode: "Advanced", Input: "V0", Color: "Green" }
//		Relay1 -> V1 {Type: "Button", Name: "Relay 1", Color: "Green", Output: "V1", Mode: "Switch" }
//		Relay2 -> V2 {Type: "Button", Name: "Relay 2", Color: "Blue", Output: "V2", Mode: "Switch" }
//		Relay3 -> V3 {Type: "Button", Name: "Relay 3", Color: "LightBlue", Output: "V3", Mode: "Switch" }
//		Relay4 -> V4 {Type: "Button", Name: "Relay 4", Color: "Orange", Output: "V4", Mode: "Switch" }

#include <KMPDinoWiFiESP.h>
#include <KMPCommon.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DHT.h>

#define BLYNK_DEBUG
#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space

#include <BlynkSimpleEsp8266.h>

// You should get Auth Token in the Blynk App.
char AUTH_TOKEN[] = "0123456789abcdef0123456789abcdef";

const char SSID[] = "******";
const char PASSWORD[] = "******";
const uint8_t HTTP_PORT = 80;

// Define sensors structure.
struct MeasureHT_t
{
	// Enable sensor - true, disable - false.
	bool IsEnable;
	// Name of sensor. Example: "First sensor".
	String Name;
	// DHT object with settings. Example: DHT(GROVE_PIN1 /* connected pin */, DHT22 /* sensor type */, 11 /* Constant for ESP8266 */)
	DHT dht;
	// Store, read humidity from sensor.
	float Humidity;
	// Store, read temperature from sensor.
	float Temperature;
};

// Sensors count. 
#define SENSOR_COUNT 2

// Define array of 2 sensors.
MeasureHT_t _measureHT[SENSOR_COUNT] =
{
	{ true, "Sensor 1", DHT(GROVE_PIN1, DHT22, 11), NAN, NAN },
	{ false, "Sensor 2", DHT(GROVE_PIN2, DHT11, 11), NAN, NAN }
};

// Check sensor data, interval in milliseconds.
const long CHECK_HT_INTERVAL_MS = 5000;
// Store last measure time.
unsigned long _mesureTimeout;				

/**
 * Blynk LCD widget.
 */
WidgetLCD _lcd(V0);

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

	Blynk.begin(AUTH_TOKEN, SSID, PASSWORD);

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

	_lcd.clear();
}

/**
 * @brief Main method.
 *
 * @return void
 */
void loop(void)
{
	// Process data every CHECK_HT_INTERVAL_MS.
	if (millis() > _mesureTimeout)
	{
		ProcessDHTSensors();
		ProcessOptoInputs();

		// Set next time to read data.
		_mesureTimeout = millis() + CHECK_HT_INTERVAL_MS;
	}

	Blynk.run();
}

/**
 * @brief Read data from DHT sensors a specified time and print new data to LCD display Blynk mobile application.
 *
 * @return void
 */
void ProcessDHTSensors()
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

			String dhtData = String(measureHT->Temperature) + " C  " + String(measureHT->Humidity) + " %";
			// Print to LCD display on row 1.
			_lcd.print(0, 0, dhtData);
		}
	}
}

/**
* @brief Read data from opto inputs. Print new data to LCD display Blynk mobile application.
*
* @return void
*/
void ProcessOptoInputs()
{
	String optoInData = "";
	// Check if any opto input changed and set new value.
	for (int i = 0; i < OPTOIN_COUNT; i++)
	{
		// Add separator.
		if (i > 0)
		{
			optoInData += " ";
		}

		// Add data.
		optoInData +=
			// Opto in name.
			String(i + 1) + String(" ")
			// Value.
			+ (KMPDinoWiFiESP.GetOptoInState(i) ? "X" : "_");
	}

	// Print to LCD display on row 2.
	_lcd.print(0, 1, optoInData);
}

/**
 * @brief Set relay state.
 *
 * @return void
 */
void SetRelay(Relay relay, int status)
{
	KMPDinoWiFiESP.SetRelayState(relay, status == 1);
}

/**
 * @brief Set Relay 1 state.
 *			On virtual pin 1.
 */
BLYNK_WRITE(V1)
{
	SetRelay(Relay1, param.asInt());
}

/**
 * @brief Set Relay 2 state.
 *			On virtual pin 2.
 */
BLYNK_WRITE(V2)
{
	SetRelay(Relay2, param.asInt());
}

/**
 * @brief Set Relay 3 state.
 *			On virtual pin 3.
 */
BLYNK_WRITE(V3)
{
	SetRelay(Relay3, param.asInt());
}

/**
 * @brief Set Relay 4 state.
 *			On virtual pin 4.
 */
BLYNK_WRITE(V4)
{
	SetRelay(Relay4, param.asInt());
}