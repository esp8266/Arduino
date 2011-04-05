
#include "WProgram.h"
#include "spi_drv.h"                   
#include "pins_arduino.h"
#define _DEBUG_
extern "C" {
#include "debug.h"
}

#define DATAOUT 11//MOSI
#define DATAIN  12//MISO 
#define SPICLOCK  13//sck
#define SLAVESELECT 2//ss
#define SLAVEREADY 3
                       

void SpiDrv::begin()
{
	  // Set direction register for SCK and MOSI pin.
	  // MISO pin automatically overrides to INPUT.
	  // When the SS pin is set as OUTPUT, it can be used as
	  // a general purpose output port (it doesn't influence
	  // SPI operations).

	  pinMode(SCK, OUTPUT);
	  pinMode(MOSI, OUTPUT);
	  pinMode(SS, OUTPUT);
	  pinMode(SLAVESELECT, OUTPUT);
	  pinMode(SLAVEREADY, INPUT);

	  digitalWrite(SCK, LOW);
	  digitalWrite(MOSI, LOW);
	  digitalWrite(SS, HIGH);
	  digitalWrite(SLAVESELECT, HIGH);

	  // Warning: if the SS pin ever becomes a LOW INPUT then SPI
	  // automatically switches to Slave, so the data direction of
	  // the SS pin MUST be kept as OUTPUT.
	  SPCR |= _BV(MSTR);
	  SPCR |= _BV(SPE);
}

void SpiDrv::end() {
  SPCR &= ~_BV(SPE);
}

void SpiDrv::spiSlaveSelect()
{
    digitalWrite(SLAVESELECT,LOW);
}


void SpiDrv::spiSlaveDeselect()
{
    digitalWrite(SLAVESELECT,HIGH);
}

char SpiDrv::spiTransfer(volatile char data)
{
    SPDR = data;                    // Start the transmission
    while (!(SPSR & (1<<SPIF)))     // Wait the end of the transmission
    {
    };
    char result = SPDR;
    delayMicroseconds(SPI_TX_DELAY);
    return result;                    // return the received byte
}

int SpiDrv::waitSpiChar(unsigned char waitChar)
{
    int timeout = TIMEOUT_CHAR;
    unsigned char _readChar = 0;
    do{
        _readChar = readChar(); //get data byte
        if (_readChar == WAIT_CMD)
        {
            delayMicroseconds(WAIT_CHAR_DELAY);
        }else
        {
            delayMicroseconds(TIMEOUT_CHAR_DELAY);
        }
//        else if (_readChar != waitChar)
//        {
//            Serial.println(_readChar,16);
//        }
    }while((timeout-- > 0) && (_readChar != waitChar));

    if ((_readChar != waitChar)&&(timeout >=0))
    {
        INFO1("*C*");
        Serial.println(_readChar,16);
    }else if (timeout == 0)
    {
        INFO1("*T*");
    }

    return  (_readChar == waitChar);
}

int SpiDrv::waitSpiChar(char waitChar, char* readChar)
{
    int timeout = TIMEOUT_CHAR;
    do{
        *readChar = spiTransfer(DUMMY_DATA); //get data byte
        if (*readChar == WAIT_CMD)
        {
            INFO1("WAIT");
            delayMicroseconds(WAIT_CHAR_DELAY);
        }
    }while((timeout-- > 0) && (*readChar != waitChar));

    return  (*readChar == waitChar);
}


int SpiDrv::readAndCheckChar(char checkChar, char* readChar)
{
    *readChar = spiTransfer(DUMMY_DATA); //get data byte

    return  (*readChar == checkChar);
}

char SpiDrv::readChar()
{
    return spiTransfer(DUMMY_DATA); //get data byte
}

//#define WAIT_START_CMD(x) waitSpiChar(START_CMD, &x)
//#define WAIT_START_CMD(x) readAndCheckChar(START_CMD, &x)
#define WAIT_START_CMD(x) waitSpiChar(START_CMD)

#define IF_CHECK_START_CMD(x)                      \
    if (!WAIT_START_CMD(_data))                 \
    {                                           \
        TOGGLE_TRIGGER()                        \
        WARN("Error waiting START_CMD");        \
        Serial.println(cmd, 16);                \
        return 0;                               \
    }else                                       \

#define CHECK_DATA(check, x)                   \
        if (!readAndCheckChar(check, &x))   \
        {                                               \
            WARN("Reply error");                        \
            INFO2(check, (uint8_t)x);							\
            return 0;                                   \
        }else                                           \

bool SpiDrv::waitSlaveReady()
{
	return (digitalRead(SLAVEREADY) == LOW);
}

void SpiDrv::waitForSlaveReady()
{
#if 0
	int count = 0;
	while (!waitSlaveReady() && (++count<TIMEOUT_READY_SLAVE))
	{
		delayMicroseconds(1);
	}
#else
	while (!waitSlaveReady());
#endif
}

int SpiDrv::waitResponse(uint8_t cmd, uint8_t numParam, uint8_t* param, uint8_t* param_len)
{
    char _data = 0;
    int i =0, ii = 0;

    IF_CHECK_START_CMD(_data)
    {
        CHECK_DATA(cmd | REPLY_FLAG, _data){};

        CHECK_DATA(numParam, _data);
        {
            readParamLen8(param_len);
            for (ii=0; ii<(*param_len); ++ii)
            {
                // Get Params data
                param[ii] = spiTransfer(DUMMY_DATA);
            } 
        }         

        readAndCheckChar(END_CMD, &_data);
    }     
    
    return 1;
}

int SpiDrv::waitResponse(uint8_t cmd, uint8_t numParam, uint8_t* param, uint16_t* param_len)
{
    char _data = 0;
    int i =0, ii = 0;

    IF_CHECK_START_CMD(_data)
    {
        CHECK_DATA(cmd | REPLY_FLAG, _data){};

        CHECK_DATA(numParam, _data);
        {
            readParamLen16(param_len);
            for (ii=0; ii<(*param_len); ++ii)
            {
                // Get Params data
                param[ii] = spiTransfer(DUMMY_DATA);
            } 
        }         

        readAndCheckChar(END_CMD, &_data);
    }     
    
    return 1;
}


int SpiDrv::waitResponse(uint8_t cmd, uint8_t* param, uint16_t* param_len)
{
    char _data = 0;
    int i =0, ii = 0;

    IF_CHECK_START_CMD(_data)
    {
        CHECK_DATA(cmd | REPLY_FLAG, _data){};

        uint8_t numParam = readChar();
        if (numParam != 0)
        {        
            readParamLen16(param_len);
            for (ii=0; ii<(*param_len); ++ii)
            {
                // Get Params data
                param[ii] = spiTransfer(DUMMY_DATA);
            } 
        }         

        readAndCheckChar(END_CMD, &_data);
    }     
    
    return 1;
}

int SpiDrv::waitResponse(uint8_t cmd, uint8_t* param, uint8_t* param_len)
{
    char _data = 0;
    int i =0, ii = 0;

    IF_CHECK_START_CMD(_data)
    {
        CHECK_DATA(cmd | REPLY_FLAG, _data){};

        uint8_t numParam = readChar();
        if (numParam != 0)
        {        
            readParamLen8(param_len);
            for (ii=0; ii<(*param_len); ++ii)
            {
                // Get Params data
                param[ii] = spiTransfer(DUMMY_DATA);
            } 
        }         

        readAndCheckChar(END_CMD, &_data);
    }     
    
    return 1;
}

int SpiDrv::waitResponse(uint8_t cmd, uint8_t numParam, tParam* params)
{
    char _data = 0;
    int i =0, ii = 0;


    IF_CHECK_START_CMD(_data)
    {
        CHECK_DATA(cmd | REPLY_FLAG, _data){};

        uint8_t _numParam = readChar();
        if (_numParam != 0)
        {        
            for (i=0; i<_numParam; ++i)
            {
                params[i].paramLen = readParamLen8();
                for (ii=0; ii<params[i].paramLen; ++ii)
                {
                    // Get Params data
                    params[i].param[ii] = spiTransfer(DUMMY_DATA);
                } 
            }
        } else
        {
            WARN("Error numParam == 0");
            Serial.println(cmd, 16);
            return 0;
        }

        if (numParam != _numParam)
        {
            WARN("Mismatch numParam");
            Serial.println(cmd, 16);
            return 0;
        }

        readAndCheckChar(END_CMD, &_data);
    }         
    return 1;
}

int SpiDrv::waitResponse(uint8_t cmd, tParam* params, uint8_t* numParamRead, uint8_t maxNumParams)
{
    char _data = 0;
    int i =0, ii = 0;

    IF_CHECK_START_CMD(_data)
    {
        CHECK_DATA(cmd | REPLY_FLAG, _data){};

        uint8_t numParam = readChar();

        if (numParam > maxNumParams)
        {
            numParam = maxNumParams;
        }
        *numParamRead = numParam;
        if (numParam != 0)
        {
            for (i=0; i<numParam; ++i)
            {
                params[i].paramLen = readParamLen8();

                for (ii=0; ii<params[i].paramLen; ++ii)
                {
                    // Get Params data
                    params[i].param[ii] = spiTransfer(DUMMY_DATA);
                } 
            }
        } else
        {
            WARN("Error numParams == 0");
            Serial.println(cmd, 16);
            return 0;
        }
        readAndCheckChar(END_CMD, &_data);
    }         
    return 1;
}

int SpiDrv::waitResponse(uint8_t cmd, uint8_t* numParamRead, uint8_t** params, uint8_t maxNumParams)
{
    char _data = 0;
    int i =0, ii = 0;

    char    *index[WL_SSID_MAX_LENGTH];

    for (i = 0 ; i < WL_NETWORKS_LIST_MAXNUM ; i++)
            index[i] = (char *)params + WL_SSID_MAX_LENGTH*i;

    IF_CHECK_START_CMD(_data)
    {
        CHECK_DATA(cmd | REPLY_FLAG, _data){};

        uint8_t numParam = readChar();

        if (numParam > maxNumParams)
        {
            numParam = maxNumParams;
        }
        *numParamRead = numParam;
        if (numParam != 0)
        {
            for (i=0; i<numParam; ++i)
            {
            	uint8_t paramLen = readParamLen8();
                for (ii=0; ii<paramLen; ++ii)
                {
                	//ssid[ii] = spiTransfer(DUMMY_DATA);
                    // Get Params data
                    index[i][ii] = (uint8_t)spiTransfer(DUMMY_DATA);

                }
                index[i][ii]=0;
            }
        } else
        {
            WARN("Error numParams == 0");
            Serial.println(cmd, 16);
            return 0;
        }
        readAndCheckChar(END_CMD, &_data);
    }
    return 1;
}


void SpiDrv::sendParam(uint8_t* param, uint8_t param_len, uint8_t lastParam)
{
    int i = 0;
    // Send Spi paramLen
    sendParamLen8(param_len);

    // Send Spi param data
    for (i=0; i<param_len; ++i)
    {
        spiTransfer(param[i]);
    }

    // if lastParam==1 Send Spi END CMD
    if (lastParam == 1)
        spiTransfer(END_CMD);
}

void SpiDrv::sendParamLen8(uint8_t param_len)
{
    // Send Spi paramLen
    spiTransfer(param_len);
}


void SpiDrv::sendParamLen16(uint16_t param_len)
{
    // Send Spi paramLen
    spiTransfer((uint8_t)((param_len & 0xff00)>>8));
    spiTransfer((uint8_t)(param_len & 0xff));
}


uint8_t SpiDrv::readParamLen8(uint8_t* param_len)
{
    uint8_t _param_len = spiTransfer(DUMMY_DATA);
    if (param_len != NULL)
    {
        *param_len = _param_len;
    }
    return _param_len;
}

uint16_t SpiDrv::readParamLen16(uint16_t* param_len)
{
    uint16_t _param_len = spiTransfer(DUMMY_DATA)<<8 | (spiTransfer(DUMMY_DATA)& 0xff);
    if (param_len != NULL)
    {
        *param_len = _param_len;
    }
    return _param_len;
}


void SpiDrv::sendBuffer(uint8_t* param, uint16_t param_len, uint8_t lastParam)
{
    int i = 0;

    // Send Spi paramLen
    sendParamLen16(param_len);

    // Send Spi param data
    for (i=0; i<param_len; ++i)
    {
        spiTransfer(param[i]);
    }

    // if lastParam==1 Send Spi END CMD
    if (lastParam == 1)
        spiTransfer(END_CMD);
}


void SpiDrv::sendParam(uint16_t param, uint8_t lastParam)
{
    int i = 0;
    // Send Spi paramLen
    sendParamLen8(2);

    spiTransfer((uint8_t)((param & 0xff00)>>8));
    spiTransfer((uint8_t)(param & 0xff));

    // if lastParam==1 Send Spi END CMD
    if (lastParam == 1)
        spiTransfer(END_CMD);
}

/* Cmd Struct Message */
/* _________________________________________________________________________________  */
/*| START CMD | C/R  | CMD  |[TOT LEN]| N.PARAM | PARAM LEN | PARAM  | .. | END CMD | */
/*|___________|______|______|_________|_________|___________|________|____|_________| */
/*|   8 bit   | 1bit | 7bit |  8bit   |  8bit   |   8bit    | nbytes | .. |   8bit  | */
/*|___________|______|______|_________|_________|___________|________|____|_________| */

void SpiDrv::sendCmd(uint8_t cmd, uint8_t numParam)
{
    // Send Spi START CMD
    spiTransfer(START_CMD);

    // Send Spi C + cmd
    spiTransfer(cmd & ~(REPLY_FLAG));

    // Send Spi totLen
    //spiTransfer(totLen);

    // Send Spi numParam
    spiTransfer(numParam);

    // If numParam == 0 send END CMD
    if (numParam == 0)
        spiTransfer(END_CMD);

}

SpiDrv spiDrv;
