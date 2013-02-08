#ifndef SPI_Drv_h
#define SPI_Drv_h

#include <inttypes.h>
#include "wifi_spi.h"

#define SPI_START_CMD_DELAY 	10

#define NO_LAST_PARAM   0
#define LAST_PARAM      1

#define DUMMY_DATA  0xFF

#define WAIT_FOR_SLAVE_SELECT()	 \
	SpiDrv::waitForSlaveReady(); \
	SpiDrv::spiSlaveSelect();



class SpiDrv
{
private:
	//static bool waitSlaveReady();
	static void waitForSlaveSign();
	static void getParam(uint8_t* param);
public:

    static void begin();

    static void end();
    
    static void spiDriverInit();
        
    static void spiSlaveSelect();
    
    static void spiSlaveDeselect();
    
    static char spiTransfer(volatile char data);

    static void waitForSlaveReady();

    //static int waitSpiChar(char waitChar, char* readChar);

    static int waitSpiChar(unsigned char waitChar);
    
    static int readAndCheckChar(char checkChar, char* readChar);

    static char readChar();

    static int waitResponseParams(uint8_t cmd, uint8_t numParam, tParam* params);
    
    static int waitResponseCmd(uint8_t cmd, uint8_t numParam, uint8_t* param, uint8_t* param_len);

    static int waitResponseData8(uint8_t cmd, uint8_t* param, uint8_t* param_len);
     
    static int waitResponseData16(uint8_t cmd, uint8_t* param, uint16_t* param_len);
 /*
    static int waitResponse(uint8_t cmd, tParam* params, uint8_t* numParamRead, uint8_t maxNumParams);
    
    static int waitResponse(uint8_t cmd, uint8_t numParam, uint8_t* param, uint16_t* param_len);
*/
    static int waitResponse(uint8_t cmd, uint8_t* numParamRead, uint8_t** params, uint8_t maxNumParams);

    static void sendParam(uint8_t* param, uint8_t param_len, uint8_t lastParam = NO_LAST_PARAM);

    static void sendParamLen8(uint8_t param_len);

    static void sendParamLen16(uint16_t param_len);

    static uint8_t readParamLen8(uint8_t* param_len = NULL);

    static uint16_t readParamLen16(uint16_t* param_len = NULL);

    static void sendBuffer(uint8_t* param, uint16_t param_len, uint8_t lastParam = NO_LAST_PARAM);

    static void sendParam(uint16_t param, uint8_t lastParam = NO_LAST_PARAM);
    
    static void sendCmd(uint8_t cmd, uint8_t numParam);
};                                                                 

extern SpiDrv spiDrv;

#endif
