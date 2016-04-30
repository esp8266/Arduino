// KMPDinoWiFiESP.h
// Company: KMP Electronics Ltd, Bulgaria
// Web: http://kmpelectronics.eu/
// License: See the GNU General Public License for more details at http://www.gnu.org/copyleft/gpl.html
// Supported boards: 
//		KMP DiNo WiFi V1.0 (http://kmpelectronics.eu/en-us/products/dinowifi.aspx)
// Description:
//		Header for KMP Dino WiFi board.
// Version: 1.0.0
// Date: 30.04.2016
// Author: Plamen Kovandjiev <p.kovandiev@kmpelectronics.eu> & Dimitar Antonov <d.antonov@kmpelectronics.eu>

#ifndef _KMPDINOWIFIESP_H
#define _KMPDINOWIFIESP_H

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <SPI.h>

// Count of inputs and outputs.
#define RELAY_COUNT  4
#define OPTOIN_COUNT 4

/**
 * @brief Relays.
 */
enum Relay {
	Relay1 = 0x00,
	Relay2 = 0x01,
	Relay3 = 0x02,
	Relay4 = 0x03
};

/**
 * @brief Inputs.
 */
enum OptoIn {
	OptoIn1 = 0x00,
	OptoIn2 = 0x01,
	OptoIn3 = 0x02,
	OptoIn4 = 0x03
};

const char TEXT_HTML[] = "text/html; charset=utf-8";
const char DINO_WIFI[] = "DiNo WiFi";
const char URL_KMPELECTRONICS_EU_DINO_WIFI[] = "http://kmpelectronics.eu/en-us/products/dinowifi.aspx";

class KMPDinoWiFiESPClass
{
 protected:
	 void ExpanderSetPin(uint8_t pinNumber, bool state);
	 bool ExpanderGetPin(uint8_t pinNumber);
	 uint8_t ExpanderReadRegister(uint8_t address);
	 void ExpanderWriteRegister(uint8_t address, uint8_t data);
	 void ExpanderSetDirection(uint8_t pinNumber, uint8_t mode);
	 void ExpanderInitGPIO();

 public:
	void init();

	void SetRelayState(uint8_t relayNumber, bool state);
	void SetRelayState(Relay relay, bool state);
	void SetAllRelaysState(bool state);
	void SetAllRelaysOn();
	void SetAllRelaysOff();
	bool GetRelayState(uint8_t relayNumber);
	bool GetRelayState(Relay relay);

	bool GetOptoInState(uint8_t optoInNumber);
	bool GetOptoInState(OptoIn optoIn);

	bool GetLedState();
	void SetLedState(bool on);
	void LedOn();
	void LedOff();
};

extern KMPDinoWiFiESPClass KMPDinoWiFiESP;

#endif

