// KMPCommon.cpp
// Company: KMP Electronics Ltd, Bulgaria
// Web: http://kmpelectronics.eu/
// License: See the GNU General Public License for more details at http://www.gnu.org/copyleft/gpl.html
// Description:
//		Common library.
// Version: 1.0.0
// Date: 09.01.2015
// Author: Plamen Kovandjiev <p.kovandiev@kmpelectronics.eu>

#include "KMPCommon.h"

bool atoip(char *data, uint8_t* result)
{
    uint8_t bPos = 0;
    
    while (bPos < IP_LEN)
    {
        uint16_t n = 0;
        uint8_t maxChr = 0;
        while ( *data >= CH_0 && *data <= CH_9 )
        {
            n = n*10 + (*data-CH_0);
            data++;
                    
            if (++maxChr > 3)
            { return false; }
        }
                
        // if value > UINT8_MAX or no find char - error.
        if (n > UINT8_MAX || maxChr == 0)
        { return false; }

        result[bPos++] = n;
                
        // Skip dot. If not dot break.
        if (*data == CH_DOT)
        { data++; }
        else
        { break; }
    }

    // If byte not 4 (0..3) error. Missing IP byte.
    if (bPos != IP_LEN)
    { return false; }
    
    return true;
}

bool atoUint8(char *data, uint8_t& result)
{
    bool isProcessed = false;
    result = 0;
    
    uint16_t n = 0;
    // Get only number char. If not number exit.
    while (*data >= CH_0 && *data <= CH_9)
    {
        isProcessed = true;
        
        n = n*10 + (*data - CH_0);

        // if value > UINT16_MAX error.
        if (n > UINT8_MAX)
        { return false; }

        data++;
    }
    
    // If not processed - error.
    if (!isProcessed)
    { return false; }
    
    result = n;
    return true;
}

bool startsWith(const char* text, const char* findTxt, bool caseSensitive /*= false*/)
{
    if (caseSensitive)
    {
        return !strncmp(findTxt, text, strlen(findTxt));
    }
    else
    {
        //Compare first strlen(findTxt) ignore case.
        return !strncasecmp(findTxt, text, strlen(findTxt));
    }
}

void iptoa(uint8_t * ip, char * result)
{
	uint8_t pos = 0;
    char buff[4];
    
	for (uint8_t i = 0; i < IP_LEN; i++)
	{
		// Add dot.
		if (i > 0)
		{ result[pos++] = CH_DOT; }
            
        itoa(ip[i], buff, 10);

        uint8_t len = strlen(buff);
  	
		for (uint8_t j = 0; j < len; j++)
		{
			result[pos++] = buff[j];
		}
	}
	
	result[pos] = CH_NONE;
}

void strNCopy(char * destination, const char * source, uint8_t copyLen)
{
    destination[copyLen] = CH_NONE;

    while (copyLen-- > 0)
    {
        *(destination++) = *(source++);
    }
    
    //*(destination) = CH_NONE;
    
    //for (uint8_t i = 0; i < copyLen; i++)
    //{
        //destination[i] = source[i];
    //}
    //destination[copyLen] = CH_NONE;
}

void IntToChars(int i, char * result)
{
	itoa(i, result, 10);
}

void FloatToChars(float f, unsigned char precision, char * result)
{
	dtostrf(f, 2, precision, result);
}

int CharToInt(char c)
{
	// Old code.
	//return c - '0';
	if(c >= '0' && c <= '9')
	{ return c - '0'; }
	
	if(c >= 'A' && c <= 'F')
	{ return c - 'A' + 10; }
	
	if(c >= 'a' && c <= 'f')
	{ return c - 'a' + 10; }

	// if char is not valid to convert return -1
	return -1;
}

int HexToByte(char& c1, char& c2)
{
	int i1 = CharToInt(c1);
	if(i1 == -1)
	{
		return -1;
	}

	int i2 = CharToInt(c2);
	if(i2 == -1)
	{
		return -1;
	}
	
	return (i1 << 4) + i2;
}

void ByteToHex(uint8_t b, char * result)
{
    result[0] = BitsToHex(b >> 4);
    result[1] = BitsToHex(b & 0x0F);
}

void ByteToHexStr( uint8_t b, char * result )
{
    ByteToHex(b, result);
    result[2] = CH_NONE;
}

char BitsToHex(uint8_t i)
{
    if (i > 15)
    {
        return CH_0;
    }
    
    return i > 9 ? i - 10 + CH_A : i + CH_0;
}
 
uint8_t crcCalc( uint8_t* data, uint8_t dataLen )
{
	uint8_t crc = 0;
		
	for (uint8_t i = 0; i < dataLen; i++)
	{
		crc += data[i];
	}
	crc = ~crc;
	crc += 1;
		
	return crc;
}

void addCrcToEnd( uint8_t* data, uint8_t dataLen )
{
	uint8_t crc = crcCalc(data, dataLen - 1);
	data[dataLen - 1] = crc;
}

void strReplace(char * str, uint8_t strLen, char oldChar, char newChar )
{
	for(int i = 0; i < strLen; i++)
	{
		if (str[i] == oldChar)
		{
			str[i] = newChar;
		}
	}
}

void IPAddressToA(const IPAddress & ip, uint8_t * result )
{
	for (uint8_t i = 0; i < 4; i++)
	{
		result[i] = ip[i];
	}
}

char IntToChar(uint8_t i)
{
    if (i > 9)
    {
        return CH_E;
    }
    
    return CH_0 + i;
}

void MillisToTime(unsigned long millis, TimeSpan & time )
{
    // Convert milliseconds to seconds.
    time.AllSeconds = millis / 1000;
    // Convert seconds to minutes.
    time.AllMinutes = time.AllSeconds / 60;
    // Convert minutes to hours.
    time.AllHours = time.AllMinutes / 60;
    // Convert hours to days.
    time.AllDays = time.AllHours / 24;
    
    // Subtract the converted seconds to minutes in order to display 59 secs max.
    time.Seconds = time.AllSeconds - (time.AllMinutes * 60);
    // Subtract the converted minutes to hours in order to display 59 minutes max.
    time.Minutes = time.AllMinutes - (time.AllHours * 60);
    //subtract the converted hours to days in order to display 23 hours max.
    time.Hours = time.AllHours - (time.AllDays * 24);
}
