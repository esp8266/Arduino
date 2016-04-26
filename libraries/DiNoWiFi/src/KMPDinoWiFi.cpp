// KMPDinoWiFi.cpp
// Company: KMP Electronics Ltd, Bulgaria
// Web: http://kmpelectronics.eu/
// License: See the GNU General Public License for more details at http://www.gnu.org/copyleft/gpl.html
// Supported boards: 
//		KMP DiNo WiFi V1.0 (http://kmpelectronics.eu/en-us/products/dinowifi.aspx)
// Description:
//		Header for KMP Dino WiFi board.
// Version: 1.0.0
// Date: 17.04.2016
// Author: Plamen Kovandjiev <p.kovandiev@kmpelectronics.eu> & Dimitar Antonov <d.antonov@kmpelectronics.eu>

#include "KMPDinoWiFi.h"

#define CS 15

#define LED_PIN 16

#define READ_CMD  0x41
#define WRITE_CMD 0x40

#define IODIR   0x00
#define IPOL    0x01
#define GPINTEN 0x02
#define DEFVAL  0x03
#define INTCON  0x04
#define IOCON   0x05
#define GPPU    0x06
#define INTF    0x07
#define INTCAP  0x08
#define GPIO    0x09
#define OLAT    0x0A

// Relay pins
#define REL1PIN 0x04
#define REL2PIN 0x05
#define REL3PIN 0x06
#define REL4PIN 0x07


// Opto input pins
#define IN1PIN 0x00
#define IN2PIN 0x01
#define IN3PIN 0x02
#define IN4PIN 0x03

/**
 * @brief Relay pins.
 */
const uint8_t RELAY_PINS[RELAY_COUNT] =
{ REL1PIN, REL2PIN, REL3PIN, REL4PIN };

/**
 * @brief Input pins.
 */
const int OPTOIN_PINS[OPTOIN_COUNT] =
{ IN1PIN, IN2PIN, IN3PIN, IN4PIN };


uint8_t  _expTxData[16];
uint8_t  _expRxData[16];

KMPDinoWiFiClass KMPDinoWiFi;

/**
 * @brief Initialize KMP Dino WiFi board.
 *		   WiFi modul ESP8266, Expander MCP23S17, relays, opto inputs and LED.
 *
 * @return void
 */
void KMPDinoWiFiClass::init()
{
	// Expander setings.
	SPI.begin();
	SPI.setHwCs(CS);
	SPI.setFrequency(1000000);
	SPI.setDataMode(SPI_MODE0);

	pinMode(CS, OUTPUT);
	digitalWrite(CS, HIGH);
	ExpanderInitGPIO();

	// Initialize the LED_BUILTIN pin as an output.
	pinMode(LED_PIN, OUTPUT);     
	LedOff();
}

/* ----------------------------------------------------------------------- */
/* Relays methods. */
/* ----------------------------------------------------------------------- */

/**
 * @brief Set relay new state.
 *
 * @param relayNumber Number of relay from 0 to RELAY_COUNT - 1. 0 - Relay1, 1 - Relay2 ...
 * @param state New state of relay, true - On, false = Off.
 *
 * @return void
 */
void KMPDinoWiFiClass::SetRelayState(uint8_t relayNumber, bool state)
{
	// Check if relayNumber is out of range - return.
	if (relayNumber > RELAY_COUNT - 1)
	{
		return;
	}
	
	ExpanderSetPin(RELAY_PINS[relayNumber], state);
}

/**
 * @brief Set relay new state.
 *
 * @param relay Relays - Relay1, Relay2 ...
 * @param state New state of relay, true - On, false = Off.
 *
 * @return void
 */
void KMPDinoWiFiClass::SetRelayState(Relay relay, bool state)
{
	SetRelayState((uint8_t)relay, state);
}

/**
 * @brief Set all relays new state.
 *
 * @param state New state of relay, true - On, false = Off.
 *
 * @return void
 */
void KMPDinoWiFiClass::SetAllRelaysState(bool state)
{
	for (uint8_t i = 0; i < RELAY_COUNT; i++)
	{
		SetRelayState(i, state);
	}
}

/**
 * @brief Set all relays in ON state.
 *
 * @return void
 */
void KMPDinoWiFiClass::SetAllRelaysOn()
{
	SetAllRelaysState(true);
}

/**
 * @brief Set all relays in ON state.
 *
 * @return void
 */
void KMPDinoWiFiClass::SetAllRelaysOff()
{
	SetAllRelaysState(false);
}

/**
 * @brief Get relay state.
 *
 * @param relayNumber Relay number from 0 to RELAY_COUNT - 1
 *
 * @return bool true relay is On, false is Off. If number is out ot range - return false.
 */
bool KMPDinoWiFiClass::GetRelayState(uint8_t relayNumber)
{
	// Check if relayNumber is out of range - return false.
	if (relayNumber > RELAY_COUNT - 1)
	{
		return false;
	}

	return ExpanderGetPin(RELAY_PINS[relayNumber]);
}

/**
 * @brief Get relay state.
 *
 * @param relay Relay1, Relay2 ...
 *
 * @return bool true relay is On, false is Off. If number is out ot range - return false.
 */
bool KMPDinoWiFiClass::GetRelayState(Relay relay)
{
	return GetRelayState((uint8_t)relay);
}

/* ----------------------------------------------------------------------- */
/* Opto input methods. */
/* ----------------------------------------------------------------------- */

/**
 * @brief Get opto in state.
 *
 * @param optoInNumber OptoIn number from 0 to OPTOIN_COUNT - 1
 *
 * @return bool true - opto in is On, false is Off. If number is out ot range - return false.
 */
bool KMPDinoWiFiClass::GetOptoInState(uint8_t optoInNumber)
{
	// Check if optoInNumber is out of range - return false.
	if (optoInNumber >= OPTOIN_COUNT - 1)
	{
		return false;
	}

	return ExpanderGetPin(OPTOIN_PINS[optoInNumber]);
}

/**
 * @brief Get opto in state.
 *
 * @param relay OptoIn1, OptoIn2 ...
 *
 * @return bool true - opto in is On, false is Off. If number is out ot range - return false.
 */
bool KMPDinoWiFiClass::GetOptoInState(OptoIn optoIn)
{
	return GetOptoInState((uint8_t)optoIn);
}

/* ----------------------------------------------------------------------- */
/* Expander MCP23S17 methods. */
/* ----------------------------------------------------------------------- */

/**
 * @brief Set expander MCP23S17 pin state.
 *
 * @param pinNumber Pin number to set.
 * @param state State of the pin, true - 1, false - 0.
 *
 * @return void
 */
void KMPDinoWiFiClass::ExpanderSetPin(uint8_t pinNumber, bool state)
{
	uint8_t registerData = ExpanderReadRegister(OLAT);

	if (state)
	{
		registerData |= (1 << pinNumber);
	}
	else
	{
		registerData &= ~(1 << pinNumber);
	}

	ExpanderWriteRegister(OLAT, registerData);
}

/**
 * @brief Get expander MCP23S17 pin state.
 *
 * @param pinNumber Pin number to read.
 *
 * @return State true - 1, false - 0.
 */
bool KMPDinoWiFiClass::ExpanderGetPin(uint8_t pinNumber)
{
	uint8_t registerData = ExpanderReadRegister(GPIO);

	return registerData & (1 << pinNumber) ? true : false;
}

/**
 * @brief Read expander MCP23S17 register.
 *
 * @param address Register address.
 *
 * @return Data from register.
 */
uint8_t KMPDinoWiFiClass::ExpanderReadRegister(uint8_t address)
{
	_expTxData[0] = READ_CMD;
	_expTxData[1] = address;

	digitalWrite(CS, LOW);
	SPI.transferBytes(_expTxData, _expRxData, 3);
	digitalWrite(CS, HIGH);

	return _expRxData[2];
}

/**
 * @brief Write data in expander MCP23S17 register.
 *
 * @param address Register address.
 * @param data Byte to write.
 *
 * @return void.
 */
void KMPDinoWiFiClass::ExpanderWriteRegister(uint8_t address, uint8_t data)
{
	_expTxData[0] = WRITE_CMD;
	_expTxData[1] = address;
	_expTxData[2] = data;

	digitalWrite(CS, LOW);
	SPI.transferBytes(_expTxData, _expRxData, 3);
	digitalWrite(CS, HIGH);
}

/**
 * @brief Set expander MCP23S17 pin direction.
 *
 * @param pinNumber Pin number to set.
 * @param mode direction mode. 0 - INPUT, 1 - OUTPUT.
 *
 * @return void
 */
void KMPDinoWiFiClass::ExpanderSetDirection(uint8_t pinNumber, uint8_t mode)
{
	uint8_t registerData = ExpanderReadRegister(IODIR);

	if (INPUT == mode)
	{
		registerData |= (1 << pinNumber);
	}
	else
	{
		registerData &= ~(1 << pinNumber);
	}

	ExpanderWriteRegister(IODIR, registerData);
}

/**
 * @brief Set expander MCP23S17 pin direction.
 *
 * @param address Register address.
 * @param data Byte to write.
 *
 * @return void.
 */
void KMPDinoWiFiClass::ExpanderInitGPIO()
{
	// Relays.
	for (uint8_t i = 0; i < RELAY_COUNT; i++)
	{
		ExpanderSetDirection(RELAY_PINS[i], OUTPUT);
	}

	// Opto inputs.
	for (uint8_t i = 0; i < OPTOIN_COUNT; i++)
	{
		ExpanderSetDirection(OPTOIN_PINS[i], INPUT);
	}
}

/**
 * @brief Get current state LED.
 *
 * @return bool If result equal - true LED On, else Off.
 */
bool KMPDinoWiFiClass::GetLedState()
{
	return !digitalRead(LED_PIN);
}

/**
 * @brief Set status LED.
 *
 * @param on. Status LED true - On, false - Off.
 *
 * @return void
 */
void KMPDinoWiFiClass::SetLedState(bool on)
{
	digitalWrite(LED_PIN, on ? LOW : HIGH);
}

/**
 * @brief Set status LED On.
 *
 * @return void
 */
void KMPDinoWiFiClass::LedOn()
{
	SetLedState(true);
}

/**
 * @brief Set status LED On.
 *
 * @return void
 */
void KMPDinoWiFiClass::LedOff()
{
	SetLedState(false);
}