// KMPDinoWiFiESP.cpp
// Company: KMP Electronics Ltd, Bulgaria
// Web: http://kmpelectronics.eu/
// License: See the GNU General Public License for more details at http://www.gnu.org/copyleft/gpl.html
// Supported boards: 
//		KMP DiNo WiFi V1.0 (http://kmpelectronics.eu/en-us/products/dinowifi.aspx)
// Description:
//		Header for KMP Dino WiFi board.
// Version: 1.0.0
// Date: 17.06.2016
// Author: Plamen Kovandjiev <p.kovandiev@kmpelectronics.eu> & Dimitar Antonov <d.antonov@kmpelectronics.eu>

#include "KMPDinoWiFiESP.h"
#include <HardwareSerial.h>

#define CS 0x0F

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

#define RS485PIN 0x10

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

KMPDinoWiFiESPClass KMPDinoWiFiESP;

/**
 * @brief Initialize KMP Dino WiFi board.
 *		   WiFi module ESP8266, Expander MCP23S17, relays and opto inputs.
 *
 * @return void
 */
void KMPDinoWiFiESPClass::init()
{
	// Expander settings.
	SPI.begin();
	SPI.setHwCs(CS);
	SPI.setFrequency(1000000);
	SPI.setDataMode(SPI_MODE0);

	pinMode(CS, OUTPUT);
	digitalWrite(CS, HIGH);
	ExpanderInitGPIO();

	// RS485 init.
	pinMode(RS485PIN, OUTPUT);
	digitalWrite(RS485PIN, HIGH);
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
void KMPDinoWiFiESPClass::SetRelayState(uint8_t relayNumber, bool state)
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
void KMPDinoWiFiESPClass::SetRelayState(Relay relay, bool state)
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
void KMPDinoWiFiESPClass::SetAllRelaysState(bool state)
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
void KMPDinoWiFiESPClass::SetAllRelaysOn()
{
	SetAllRelaysState(true);
}

/**
 * @brief Set all relays in ON state.
 *
 * @return void
 */
void KMPDinoWiFiESPClass::SetAllRelaysOff()
{
	SetAllRelaysState(false);
}

/**
 * @brief Get relay state.
 *
 * @param relayNumber Relay number from 0 to RELAY_COUNT - 1
 *
 * @return bool true relay is On, false is Off. If number is out of range - return false.
 */
bool KMPDinoWiFiESPClass::GetRelayState(uint8_t relayNumber)
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
 * @return bool true relay is On, false is Off. If number is out of range - return false.
 */
bool KMPDinoWiFiESPClass::GetRelayState(Relay relay)
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
 * @return bool true - opto in is On, false is Off. If number is out of range - return false.
 */
bool KMPDinoWiFiESPClass::GetOptoInState(uint8_t optoInNumber)
{
	// Check if optoInNumber is out of range - return false.
	if (optoInNumber > OPTOIN_COUNT - 1)
	{
		return false;
	}

	return !ExpanderGetPin(OPTOIN_PINS[optoInNumber]);
}

/**
 * @brief Get opto in state.
 *
 * @param relay OptoIn1, OptoIn2 ...
 *
 * @return bool true - opto in is On, false is Off. If number is out of range - return false.
 */
bool KMPDinoWiFiESPClass::GetOptoInState(OptoIn optoIn)
{
	return GetOptoInState((uint8_t)optoIn);
}

/* ----------------------------------------------------------------------- */
/* Expander MCP23S17 methods. */
/* ----------------------------------------------------------------------- */

/**
 * @brief Set expander MCP23S17 pin state.
 *
 * @param pinNumber The number of pin to be set.
 * @param state The pin state, true - 1, false - 0.
 *
 * @return void
 */
void KMPDinoWiFiESPClass::ExpanderSetPin(uint8_t pinNumber, bool state)
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
 * @param pinNumber The number of pin to be get.
 *
 * @return State true - 1, false - 0.
 */
bool KMPDinoWiFiESPClass::ExpanderGetPin(uint8_t pinNumber)
{
	uint8_t registerData = ExpanderReadRegister(GPIO);

	return registerData & (1 << pinNumber);
}

/**
 * @brief Read an expander MCP23S17 a register.
 *
 * @param address A register address.
 *
 * @return The data from the register.
 */
uint8_t KMPDinoWiFiESPClass::ExpanderReadRegister(uint8_t address)
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
 * @param address A register address.
 * @param data A byte for write.
 *
 * @return void.
 */
void KMPDinoWiFiESPClass::ExpanderWriteRegister(uint8_t address, uint8_t data)
{
	_expTxData[0] = WRITE_CMD;
	_expTxData[1] = address;
	_expTxData[2] = data;

	digitalWrite(CS, LOW);
	SPI.transferBytes(_expTxData, _expRxData, 3);
	digitalWrite(CS, HIGH);
}

/**
 * @brief Set a expander MCP23S17 the pin direction.
 *
 * @param pinNumber Pin number for set.
 * @param mode direction mode. 0 - INPUT, 1 - OUTPUT.
 *
 * @return void
 */
void KMPDinoWiFiESPClass::ExpanderSetDirection(uint8_t pinNumber, uint8_t mode)
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
void KMPDinoWiFiESPClass::ExpanderInitGPIO()
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
* @brief Connect to RS485. With default configuration SERIAL_8N1.
*
* @param baud Speed.
*     Values: 75, 110, 300, 1200, 2400, 4800, 9600, 19200, 38400, 57600 and 115200 bit/s.
*
* @return void
*/
void KMPDinoWiFiESPClass::RS485Begin(unsigned long baud)
{
	RS485Begin(baud, SERIAL_8N1);
}

/**
* @brief Start connect to RS485.
*
* @param baud Speed.
*             Values: 75, 110, 300, 1200, 2400, 4800, 9600, 19200, 38400, 57600 and 115200 bit/s.
* @param config Configuration - data bits, parity, stop bits.
*               Values: SERIAL_5N1, SERIAL_6N1, SERIAL_7N1, SERIAL_8N1, SERIAL_5N2, SERIAL_6N2, SERIAL_7N2, SERIAL_8N2, SERIAL_5E1, SERIAL_6E1, SERIAL_7E1, SERIAL_8E1, SERIAL_5E2, 
						SERIAL_6E2, SERIAL_7E2, SERIAL_8E2, SERIAL_5O1, SERIAL_6O1, SERIAL_7O1, SERIAL_8O1, SERIAL_5O2, SERIAL_6O2, SERIAL_7O2, SERIAL_8O2
*
* @return void
*/
void KMPDinoWiFiESPClass::RS485Begin(unsigned long baud, SerialConfig config)
{
	Serial.begin(baud, config);
}

/**
* @brief Close connection to RS485.
*
* @return void
*/
void KMPDinoWiFiESPClass::RS485End()
{
	Serial.end();
}

/**
* @brief Begin write data to RS485. 
*
* @return void
*/
void RS485BeginWrite()
{
	digitalWrite(RS485PIN, LOW);
	delay(1);
}

/**
* @brief End write data to RS485.
*
* @return void
*/
void RS485EndWrite()
{
	Serial.flush();
	delay(1);
	digitalWrite(RS485PIN, HIGH);
}

/**
* @brief Transmit one byte data to RS485.
*
* @param data Transmit data.
*
* @return size_t Count of transmitted - one byte.
*/
size_t KMPDinoWiFiESPClass::RS485Write(uint8_t data)
{
	RS485BeginWrite();

	size_t result = Serial.write(data);

	RS485EndWrite();

	return result;
}

/**
* @brief Transmit one char data to RS485.
*
* @param data Transmit data.
*
* @return size_t Count of transmitted - one char.
*/
size_t KMPDinoWiFiESPClass::RS485Write(char data)
{
	return RS485Write((uint8_t)data);
}

/**
* @brief Transmit the text to RS485.
*
* @param data Text data to transmit.
*
* @return size_t Count of transmitted chars.
*/
size_t KMPDinoWiFiESPClass::RS485Write(char* data)
{
	RS485BeginWrite();

	size_t len = strlen(data);
	size_t result = 0;
	while(len > 0)
	{
		result += Serial.write(*data++);
		--len;
	}

	RS485EndWrite();

	return result;
}

/**
* @brief Send array of bytes to RS485.
*
* @param data Array in bytes to be send.
* @param dataLen Array length.
*
* @return size_t Count of transmitted bytes.
*/
size_t KMPDinoWiFiESPClass::RS485Write(uint8_t* data, uint8_t dataLen)
{
	RS485BeginWrite();

	size_t result = 0;
	for (size_t i = 0; i < dataLen; i++)
	{
		result += Serial.write(data[i]);
	}

	RS485EndWrite();

	return result;
}

/**
* @brief Read received data from RS485.
*
*
* @return int Received byte.<para></para>
*   If result = -1 - buffer is empty, no data
*   if result > -1 - valid byte to read.
*/
int KMPDinoWiFiESPClass::RS485Read()
{
	return RS485Read(10, 10);
}

/**
* @brief Read received data from RS485. Reading data with delay and repeating the operation while all data to arrive.
*
* @param delayWait Wait delay if not available to read byte in milliseconds. Default 10.
* @param repeatTime Repeat time if not read bytes. Default 10. All time = delayWait * repeatTime.
*
* @return int Received byte.
*   If result = -1 - buffer is empty, no data<para></para>
*   if result > -1 - valid byte to read.
*/
int KMPDinoWiFiESPClass::RS485Read(unsigned long delayWait, uint8_t repeatTime)
{
	// Wait before read, if the buffer is empty.
	while (Serial.available() == 0 && repeatTime > 0)
	{
		delay(delayWait);
		--repeatTime;
	}

	return Serial.read();
}