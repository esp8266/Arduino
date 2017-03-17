// KMPCommon.h
// Company: KMP Electronics Ltd, Bulgaria
// Web: http://kmpelectronics.eu/
// License: See the GNU General Public License for more details at http://www.gnu.org/copyleft/gpl.html
// Description:
//		Common library.
// Version: 1.0.0
// Date: 09.01.2015
// Author: Plamen Kovandjiev <p.kovandiev@kmpelectronics.eu>

#ifndef _KMPCOMMON_h
#define _KMPCOMMON_h

#include "Arduino.h"

#include "IPAddress.h"

#ifndef UINT8_MAX
#define UINT8_MAX (~(uint8_t)0)
#endif

// Length in bytes for IP addresses.
#define IP_LEN 4

const char CH_NONE = '\0';
const char CH_AMPERSAND = '&';
const char CH_0 = '0';
const char CH_1 = '1';
const char CH_2 = '2';
const char CH_3 = '3';
const char CH_4 = '4';
const char CH_5 = '5';
const char CH_6 = '6';
const char CH_7 = '7';
const char CH_8 = '8';
const char CH_9 = '9';
const char CH_DOT = '.';
const char CH_SPACE = ' ';
const char CH_CR = '\r';
const char CH_LF = '\n';
const char CR_LF[] = "\r\n";
const char CH_DQUOTE = '"';
const char CH_LT = '<';
const char CH_GT = '>';
const char CH_A = 'A';
const char CH_R = 'R';
const char CH_I = 'I';
const char CH_E = 'E';
const char CH_EQUAL = '=';
const char W_ON[] = "On";
const char W_OFF[] = "Off";
const char KMP_ELECTRONICS_LTD[] = "KMP Electronics Ltd.";
const char URL_KMPELECTRONICS_EU[] = "http://kmpelectronics.eu/";

struct TimeSpan
{
    unsigned long AllSeconds;
    unsigned long AllMinutes;
    unsigned long AllHours;
    unsigned long AllDays;

    uint8_t Seconds;
    uint8_t Minutes;
    uint8_t Hours;
};

/**
 * \brief Convert char array to IP address - for bytes.
 *
 * \param data Char array to convert.
 *             Example: chars            | result
 *                      0.0.0.0\0        | {0, 0, 0, 0}
 *                      192.168.0.100\0  | {192, 168, 0, 1}
 *                      192.168.0.21&    | {192, 168, 0, 21}
 *                      192.168.0.256\0  | error - last byte is greater from 255.
 *                      .0.0.0\0         | error - first byte missing.
 *                      0..0.0\0         | error - second byte missing.
 *                      a.0.0.0\0        | error - char 'a' is not digit or dot.
 *                      0.0\0            | error - missing bytes. Bytes is 2.
 * \param result IP address in bytes. Exam.: { 192, 168, 0, 1}.
 *				Result array len must be IP_LEN.
 * 
 * \return bool If result equal:
 *                  - true - convert OK;
 *                  - false - error converting.
 */
bool atoip(char *data, uint8_t* result);

/**
 * \brief Convert char array to uint16_t.
 * 
 * \param data Char array convert to uint16_t result.
 *             Example: chars  |  result
 *                      0\0    | 0
 *                      g\0    | error - first char (g) is not digit.
 *                      42!    | 42
 *                      12323& | 12323
 *                      a123\0 | error - first char (a) is not digit.
 *                      77777\0| error - is too big result > 65535.
 * \param result Result of convention.
 * 
 * \return bool If result equal:
 *                  - true - convert OK;
 *                  - false - error converting.
 */
bool atoUint8(char *data, uint8_t& result);

/**
 * \brief Check if "text" start with "findTxt".
 * 
 * \param text In which will search "findTxt".
 * \param findTxt Text to find.
 * \param caseSensitive Check case sensitive if true or case insensitive chars. Default false.
 * 
 * \return bool If result equal:
 *                  - true - "text" start with "findTxt";
 *                  - false - "text" not start with "findTxt".
 */
bool startsWith(const char* text, const char* findTxt, bool caseSensitive = false);

/**
 * \brief Convert ip address (array of bytes) to string.
 * 
 * \param ip Address - array of bytes.
 * \param result String IP address separated by dot. Exam.: 192.168.0.1
 *				Result array len must be greater chars.
 * \return void
 */
void iptoa(uint8_t * ip, char * result);

/**
 * \brief Convert IPAddress to array of bytes.
 * 
 * \param ip IPAddress to convert.
 * \param result IP address array.
 * 
 * \return void
 */
void IPAddressToA(const IPAddress & ip, uint8_t * result);

/**
 * \brief Convert int to string.
 * 
 * \param i Integer to convert to text.
 * \param result Converted to text integer.
 * 
 * \return void
 */
void IntToChars(int i, char * result);

/**
 * \brief Convert one digit to char.
 *        0 -> '0', 1 -> '1', ... 9 -> '9'.
 * 
 * \param i One digit from 0 to 9.
 * 
 * \return char If i > 9 return E.
 */
char IntToChar(uint8_t i);

/**
 * \brief Convert float to string.
 * 
 * \param f Float to convert to text.
 * \param precision Determines the number of digits after the decimal sign.
 * \param result Converted to text float.
 *
 * \return void
 */
void FloatToChars(float f, unsigned char precision, char * result);

/**
 * \brief Convert hex char to byte. From '0' to 'F' or 'f' case insensitive.
 *        '0' -> 0, '1' -> 1, ... 'F' -> 15, 'f' -> 15.
 * 
 * \param c Char to convert to byte. 
 *
 * \return int Converted char to int. If char is not valid to convert - return -1.
 */
int CharToInt(char c);

/**
 * \brief Convert two hex chars to byte.
 *        Example 'f', 'F' -> 255; '1', 'a' -> 26.
 * 
 * \param c1 Most significant four bits in char.
 * \param c2 Least significant four bits in char.
 * 
 * \return int Converted two chars to byte. If any char is not valid to convert - return -1.
 */
int HexToByte(char& c1, char& c2);

/**
 * \brief Convert byte to hex chars.
 *        0 -> "00", 128 -> "80", 255 -> "FF".
 * 
 * \param b Byte to convert.
 * \param result Hex two chars. Example: ['F', 'F']
 * 
 * \return void
 */
void ByteToHex(uint8_t b, char * result);

/**
 * \brief Convert byte to hex \0 terminated string.
 *        0 -> '00\0', 128 -> '80\0', 255 -> 'FF\0'.
 * 
 * \param b Byte to convert.
 * \param result Hex two chars. Example: ['F', 'F', '\0']
 * 
 * \return void
 */
void ByteToHexStr(uint8_t b, char * result);

void BytesToHexStr(const uint8_t* b, const int len,  char * result);

/**
 * \brief Convert low bits from byte (0 - 16) to Hex char (0 - F).
 * 
 * \param i Low bits from byte (0 - 16).
 * 
 * \return char Hex char (0 - F).
 */
char BitsToHex(uint8_t i);

/**
 * \brief Copy N chars from source to destination.
 * 
 * \param destination Copy chars here.
 * \param source Get chars from here.
 * \param copyLen Number of chars to copy.
 * 
 * \return void
 */
void strNCopy(char * destination, const char * source, uint8_t copyLen);

/**
 * \brief 
 * 
 * \param data data with in calculate CRC sum.
 * \param dataLen data length.
 * 
 * \return uint8_t crc sum.
 */
uint8_t crcCalc(uint8_t* data, uint8_t dataLen);


/**
 * \brief Add crc sum to end of data array, in last element. Algorithm - sum all, sum = ~sum, sum += 1
 *        Example: 0x01, 0x01, 0x01, 0x00 (crc);
 *        Result:  0x01, 0x01, 0x01, 0xFD (crc);
 * 
 * \param data data with in calculate CRC sum.
 * \param dataLen data length include last empty byte.
 * 
 * \return void
 */
void addCrcToEnd(uint8_t* data, uint8_t dataLen);

/**
 * \brief 
 * 
 * \param str String to change.
 * \param strLen String length.
 * \param oldChar Char must change.
 * \param newChar Char to changed.
 * 
 * \return void
 */
void strReplace(char * str, uint8_t strLen, char oldChar, char newChar);

/**
 * \brief Convert millis to time (day, hour, minute, second).
 * 
 * \param millis Time in milliseconds.
 * \param time Structure with all time data. See structure TimeSpan.
 * 
 * \return void
 */
void MillisToTime(unsigned long millis, TimeSpan & time);

#endif