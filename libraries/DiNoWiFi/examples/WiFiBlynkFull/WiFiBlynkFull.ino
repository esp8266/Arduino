// WiFiBlynkFull.ino
// Company: KMP Electronics Ltd, Bulgaria
// Web: http://kmpelectronics.eu/
// License: See the GNU General Public License for more details at http://www.gnu.org/copyleft/gpl.html
// Supported boards:
//		KMP ProDino WiFi-ESP with ESP8266 V1.1 (http://www.kmpelectronics.eu/en-us/products/prodinowifi-esp.aspx)
// Description:
//		Blynk example. For this example need add in Blynk mobile application 4 button (200), 4 LEDs(100) and 2 value display(200). All 1600 points.
//		Free points in Blynk application is 1200. If you do not want to buy points, use the example: WiFiBlynk.ino
// Example link: http://www.kmpelectronics.eu/en-us/examples/prodinowifi-esp/wifiblynkfull.aspx
// Version: 1.0.0
// Date: 07.07.2016
// Author: Plamen Kovandjiev <p.kovandiev@kmpelectronics.eu>

// --------------------------------------------------------------------------------
// Prerequisites:
//	Before start this example you need to install:
//		Install Blynk library: Sketch\Include library\Menage Libraries... find Blynk and click Install.
//		DHT library: https://github.com/adafruit/DHT-sensor-library
//		Connect DHT22 sensor to GROVE connector. Use pins: 
//			- first sensor GROVE_PIN1, Vcc+, Gnd(-);
//			- second sensor GROVE_PIN2, Vcc+, Gnd(-);
//  PRODINo WiFi-ESP -> Blynk pins map:
//		Relay1 -> V1 {Type: "Button", Name: "Relay 1", Color: "Green", Output: "V1", Mode: "Switch" }
//		Relay2 -> V2 {Type: "Button", Name: "Relay 2", Color: "Blue", Output: "V2", Mode: "Switch" }
//		Relay3 -> V3 {Type: "Button", Name: "Relay 3", Color: "LightBlue", Output: "V3", Mode: "Switch" }
//		Relay4 -> V4 {Type: "Button", Name: "Relay 4", Color: "Orange", Output: "V4", Mode: "Switch" }
//		OptoIn1 -> V5 {Type: "LED", Name: "In 1", Color: "Green", Input: "V5" }
//		OptoIn2 -> V6 {Type: "LED", Name: "In 2", Color: "Blue", Input: "V6" }
//		OptoIn3 -> V7 {Type: "LED", Name: "In 3", Color: "LightBlue", Input: "V7" }
//		OptoIn4 -> V8 {Type: "LED", Name: "In 4", Color: "Orange", Input: "V8" }
//		DHT1T -> V9 {Type: "Value Display", Name: "Temperature", Color: "Green", Input: "V9", Min:"-40", Max:"80", ReadingFrecuency: "5sec" }
//		DHT1H -> V10 {Type: "Value Display", Name: "Humidity", Color: "Green", Input: "V10", Min:"0", Max:"100", ReadingFrecuency: "5sec" }

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

// Opto input structure.
struct OptoIn_t
{
	OptoIn Input;
	WidgetLED Widget;
	bool Status;
};

// Store opto input data, settings and processing objects.
OptoIn_t _optoInputs[OPTOIN_COUNT] =
{
	{ OptoIn1, WidgetLED(V5), false },
	{ OptoIn2, WidgetLED(V6), false },
	{ OptoIn3, WidgetLED(V7), false },
	{ OptoIn4, WidgetLED(V8), false }
};

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
}

/**
 * @brief Main method.
 *
 * @return void
 */
void loop(void)
{
	ProcessDHTSensors();
	ProcessOptoInputs();

	Blynk.run();
}

/**
 * @brief Read data from DHT sensors a specified time and send to Blynk.
 *
 * @return void
 */
void ProcessDHTSensors()
{
	if (millis() > _mesureTimeout)
	{
		int firstFreeVirtualPin = V9;

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

				// Write pair of data in pins V9, V10. If have second write V11, V12.
				Blynk.virtualWrite(firstFreeVirtualPin++, measureHT->Temperature);
				Blynk.virtualWrite(firstFreeVirtualPin++, measureHT->Humidity);
			}
		}

		// Set next time to read data.
		_mesureTimeout = millis() + CHECK_HT_INTERVAL_MS;
	}
}

/**
* @brief Read data from opto inputs. Send data if only changes to Blynk.
*
* @return void
*/
void ProcessOptoInputs()
{
	for (int i = 0; i < OPTOIN_COUNT; i++)
	{
		OptoIn_t* optoInput = &_optoInputs[i];
		bool currentStatus = KMPDinoWiFiESP.GetOptoInState(optoInput->Input);
		if (optoInput->Status != currentStatus || ((bool)optoInput->Widget.getValue()) != currentStatus)
		{
			Serial.println("Opto input " + String(i + 1) + " status changed to -> \"" + currentStatus + "\". WidgetLED value: " + optoInput->Widget.getValue());

			currentStatus ? optoInput->Widget.on() : optoInput->Widget.off();
			optoInput->Status = currentStatus;
		}
	}
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

/**
 * @brief Get opto input 1 state.
 *			On virtual pin 5.
 */
BLYNK_READ(V5)
{
	//Blynk.virtualWrite(V5, KMPDinoWiFiESP.GetOptoInState(OptoIn1));
	Blynk.virtualWrite(V5, 255);
}

/**
 * @brief Get opto input 2 state.
 *			On virtual pin 6.
 */
BLYNK_READ(V6)
{
	Blynk.virtualWrite(V6, KMPDinoWiFiESP.GetOptoInState(OptoIn2));
}

/**
 * @brief Get opto input 3 state.
 *			On virtual pin 7.
 */
BLYNK_READ(V7)
{
	Blynk.virtualWrite(V7, KMPDinoWiFiESP.GetOptoInState(OptoIn3));
}

/**
 * @brief Get opto input 4 state.
 *			On virtual pin 8.
 */
BLYNK_READ(V8)
{
	Blynk.virtualWrite(V8, KMPDinoWiFiESP.GetOptoInState(OptoIn4));
}