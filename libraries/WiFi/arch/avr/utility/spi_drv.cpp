
#include "Arduino.h"
#include <SPI.h>
#include "utility/spi_drv.h"                   
#include "pins_arduino.h"
//#define _DEBUG_
extern "C" {
#include "utility/debug.h"
}

#define DATAOUT     11 // MOSI
#define DATAIN      12 // MISO
#define SPICLOCK    13 // sck
#define SLAVESELECT 10 // ss
#define SLAVEREADY  7  // handshake pin
#define WIFILED     9  // led on wifi shield

#define DELAY_SPI(X) { int ii=0; do { asm volatile("nop"); } while (++ii < (X*F_CPU/16000000)); }
#define DELAY_TRANSFER() DELAY_SPI(10)

void SpiDrv::begin()
{
	  SPI.begin();
	  pinMode(SLAVESELECT, OUTPUT);
	  pinMode(SLAVEREADY, INPUT);
	  pinMode(WIFILED, OUTPUT);

	  digitalWrite(SCK, LOW);
	  digitalWrite(MOSI, LOW);
	  digitalWrite(SS, HIGH);
	  digitalWrite(SLAVESELECT, HIGH);
	  digitalWrite(WIFILED, LOW);

#ifdef _DEBUG_
	  INIT_TRIGGER()
#endif
}

void SpiDrv::end() {
    SPI.end();
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
    char result = SPI.transfer(data);
    DELAY_TRANSFER();

    return result;                    // return the received byte
}

int SpiDrv::waitSpiChar(unsigned char waitChar)
{
    int timeout = TIMEOUT_CHAR;
    unsigned char _readChar = 0;
    do{
        _readChar = readChar(); //get data byte
        if (_readChar == ERR_CMD)
        {
        	WARN("Err cmd received\n");
        	return -1;
        }
    }while((timeout-- > 0) && (_readChar != waitChar));
    return  (_readChar == waitChar);
}

int SpiDrv::readAndCheckChar(char checkChar, char* readChar)
{
    getParam((uint8_t*)readChar);

    return  (*readChar == checkChar);
}

char SpiDrv::readChar()
{
	uint8_t readChar = 0;
	getParam(&readChar);
	return readChar;
}

#define WAIT_START_CMD(x) waitSpiChar(START_CMD)

#define IF_CHECK_START_CMD(x)                      \
    if (!WAIT_START_CMD(_data))                 \
    {                                           \
        TOGGLE_TRIGGER()                        \
        WARN("Error waiting START_CMD");        \
        return 0;                               \
    }else                                       \

#define CHECK_DATA(check, x)                   \
        if (!readAndCheckChar(check, &x))   \
        {                                               \
        	TOGGLE_TRIGGER()                        \
            WARN("Reply error");                        \
            INFO2(check, (uint8_t)x);							\
            return 0;                                   \
        }else                                           \

#define waitSlaveReady() (digitalRead(SLAVEREADY) == LOW)
#define waitSlaveSign() (digitalRead(SLAVEREADY) == HIGH)
#define waitSlaveSignalH() while(digitalRead(SLAVEREADY) != HIGH){}
#define waitSlaveSignalL() while(digitalRead(SLAVEREADY) != LOW){}

void SpiDrv::waitForSlaveSign()
{
	while (!waitSlaveSign());
}

void SpiDrv::waitForSlaveReady()
{
	while (!waitSlaveReady());
}

void SpiDrv::getParam(uint8_t* param)
{
    // Get Params data
    *param = spiTransfer(DUMMY_DATA);
    DELAY_TRANSFER();
}

int SpiDrv::waitResponseCmd(uint8_t cmd, uint8_t numParam, uint8_t* param, uint8_t* param_len)
{
    char _data = 0;
    int ii = 0;

    IF_CHECK_START_CMD(_data)
    {
        CHECK_DATA(cmd | REPLY_FLAG, _data){};

        CHECK_DATA(numParam, _data);
        {
            readParamLen8(param_len);
            for (ii=0; ii<(*param_len); ++ii)
            {
                // Get Params data
                //param[ii] = spiTransfer(DUMMY_DATA);
                getParam(&param[ii]);
            } 
        }         

        readAndCheckChar(END_CMD, &_data);
    }     
    
    return 1;
}
/*
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
*/

int SpiDrv::waitResponseData16(uint8_t cmd, uint8_t* param, uint16_t* param_len)
{
    char _data = 0;
    uint16_t ii = 0;

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

int SpiDrv::waitResponseData8(uint8_t cmd, uint8_t* param, uint8_t* param_len)
{
    char _data = 0;
    int ii = 0;

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

int SpiDrv::waitResponseParams(uint8_t cmd, uint8_t numParam, tParam* params)
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
            return 0;
        }

        if (numParam != _numParam)
        {
            WARN("Mismatch numParam");
            return 0;
        }

        readAndCheckChar(END_CMD, &_data);
    }         
    return 1;
}

/*
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
*/

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
            readAndCheckChar(END_CMD, &_data);
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
    uint16_t i = 0;

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

    //waitForSlaveSign();
    //wait the interrupt trigger on slave
    delayMicroseconds(SPI_START_CMD_DELAY);

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
