//#define _DEBUG_

#include "utility/server_drv.h"

#include "Arduino.h"
#include "utility/spi_drv.h"

extern "C" {
#include "utility/wl_types.h"
#include "utility/debug.h"
}


// Start server TCP on port specified
void ServerDrv::startServer(uint16_t port, uint8_t sock, uint8_t protMode)
{
	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    SpiDrv::sendCmd(START_SERVER_TCP_CMD, PARAM_NUMS_3);
    SpiDrv::sendParam(port);
    SpiDrv::sendParam(&sock, 1);
    SpiDrv::sendParam(&protMode, 1, LAST_PARAM);

    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!SpiDrv::waitResponseCmd(START_SERVER_TCP_CMD, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
    }
    SpiDrv::spiSlaveDeselect();
}

// Start server TCP on port specified
void ServerDrv::startClient(uint32_t ipAddress, uint16_t port, uint8_t sock, uint8_t protMode)
{
	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    SpiDrv::sendCmd(START_CLIENT_TCP_CMD, PARAM_NUMS_4);
    SpiDrv::sendParam((uint8_t*)&ipAddress, sizeof(ipAddress));
    SpiDrv::sendParam(port);
    SpiDrv::sendParam(&sock, 1);
    SpiDrv::sendParam(&protMode, 1, LAST_PARAM);

    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!SpiDrv::waitResponseCmd(START_CLIENT_TCP_CMD, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
    }
    SpiDrv::spiSlaveDeselect();
}

// Start server TCP on port specified
void ServerDrv::stopClient(uint8_t sock)
{
	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    SpiDrv::sendCmd(STOP_CLIENT_TCP_CMD, PARAM_NUMS_1);
    SpiDrv::sendParam(&sock, 1, LAST_PARAM);

    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!SpiDrv::waitResponseCmd(STOP_CLIENT_TCP_CMD, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
    }
    SpiDrv::spiSlaveDeselect();
}


uint8_t ServerDrv::getServerState(uint8_t sock)
{
	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    SpiDrv::sendCmd(GET_STATE_TCP_CMD, PARAM_NUMS_1);
    SpiDrv::sendParam(&sock, sizeof(sock), LAST_PARAM);

    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!SpiDrv::waitResponseCmd(GET_STATE_TCP_CMD, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
    }
    SpiDrv::spiSlaveDeselect();
   return _data;
}

uint8_t ServerDrv::getClientState(uint8_t sock)
{
	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    SpiDrv::sendCmd(GET_CLIENT_STATE_TCP_CMD, PARAM_NUMS_1);
    SpiDrv::sendParam(&sock, sizeof(sock), LAST_PARAM);

    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!SpiDrv::waitResponseCmd(GET_CLIENT_STATE_TCP_CMD, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
    }
    SpiDrv::spiSlaveDeselect();
   return _data;
}

uint16_t ServerDrv::availData(uint8_t sock)
{
	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    SpiDrv::sendCmd(AVAIL_DATA_TCP_CMD, PARAM_NUMS_1);
    SpiDrv::sendParam(&sock, sizeof(sock), LAST_PARAM);

    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();

    // Wait for reply
    uint8_t _dataLen = 0;
	uint16_t len = 0;

    SpiDrv::waitResponseCmd(AVAIL_DATA_TCP_CMD, PARAM_NUMS_1, (uint8_t*)&len,  &_dataLen);

    SpiDrv::spiSlaveDeselect();

    return len;
}

bool ServerDrv::getData(uint8_t sock, uint8_t *data, uint8_t peek)
{
	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    SpiDrv::sendCmd(GET_DATA_TCP_CMD, PARAM_NUMS_2);
    SpiDrv::sendParam(&sock, sizeof(sock));
    SpiDrv::sendParam(peek, LAST_PARAM);

    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!SpiDrv::waitResponseData8(GET_DATA_TCP_CMD, &_data, &_dataLen))
    {
        WARN("error waitResponse");
    }
    SpiDrv::spiSlaveDeselect();
    if (_dataLen!=0)
    {
        *data = _data;
        return true;
    }
    return false;
}

bool ServerDrv::getDataBuf(uint8_t sock, uint8_t *_data, uint16_t *_dataLen)
{
	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    SpiDrv::sendCmd(GET_DATABUF_TCP_CMD, PARAM_NUMS_1);
    SpiDrv::sendBuffer(&sock, sizeof(sock), LAST_PARAM);

    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();

    // Wait for reply
    if (!SpiDrv::waitResponseData16(GET_DATABUF_TCP_CMD, _data, _dataLen))
    {
        WARN("error waitResponse");
    }
    SpiDrv::spiSlaveDeselect();
    if (*_dataLen!=0)
    {
        return true;
    }
    return false;
}

bool ServerDrv::insertDataBuf(uint8_t sock, const uint8_t *data, uint16_t _len)
{
	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    SpiDrv::sendCmd(INSERT_DATABUF_CMD, PARAM_NUMS_2);
    SpiDrv::sendBuffer(&sock, sizeof(sock));
    SpiDrv::sendBuffer((uint8_t *)data, _len, LAST_PARAM);

    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!SpiDrv::waitResponseData8(INSERT_DATABUF_CMD, &_data, &_dataLen))
    {
        WARN("error waitResponse");
    }
    SpiDrv::spiSlaveDeselect();
    if (_dataLen!=0)
    {
        return (_data == 1);
    }
    return false;
}

bool ServerDrv::sendUdpData(uint8_t sock)
{
	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    SpiDrv::sendCmd(SEND_DATA_UDP_CMD, PARAM_NUMS_1);
    SpiDrv::sendParam(&sock, sizeof(sock), LAST_PARAM);

    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!SpiDrv::waitResponseData8(SEND_DATA_UDP_CMD, &_data, &_dataLen))
    {
        WARN("error waitResponse");
    }
    SpiDrv::spiSlaveDeselect();
    if (_dataLen!=0)
    {
        return (_data == 1);
    }
    return false;
}


bool ServerDrv::sendData(uint8_t sock, const uint8_t *data, uint16_t len)
{
	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    SpiDrv::sendCmd(SEND_DATA_TCP_CMD, PARAM_NUMS_2);
    SpiDrv::sendBuffer(&sock, sizeof(sock));
    SpiDrv::sendBuffer((uint8_t *)data, len, LAST_PARAM);

    //Wait the reply elaboration
    SpiDrv::waitForSlaveReady();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!SpiDrv::waitResponseData8(SEND_DATA_TCP_CMD, &_data, &_dataLen))
    {
        WARN("error waitResponse");
    }
    SpiDrv::spiSlaveDeselect();
    if (_dataLen!=0)
    {
        return (_data == 1);
    }
    return false;
}


uint8_t ServerDrv::checkDataSent(uint8_t sock)
{
	const uint16_t TIMEOUT_DATA_SENT = 25;
    uint16_t timeout = 0;
	uint8_t _data = 0;
	uint8_t _dataLen = 0;

	do {
		WAIT_FOR_SLAVE_SELECT();
		// Send Command
		SpiDrv::sendCmd(DATA_SENT_TCP_CMD, PARAM_NUMS_1);
		SpiDrv::sendParam(&sock, sizeof(sock), LAST_PARAM);

		//Wait the reply elaboration
		SpiDrv::waitForSlaveReady();

		// Wait for reply
		if (!SpiDrv::waitResponseCmd(DATA_SENT_TCP_CMD, PARAM_NUMS_1, &_data, &_dataLen))
		{
			WARN("error waitResponse isDataSent");
		}
		SpiDrv::spiSlaveDeselect();

		if (_data) timeout = 0;
		else{
			++timeout;
			delay(100);
		}

	}while((_data==0)&&(timeout<TIMEOUT_DATA_SENT));
    return (timeout==TIMEOUT_DATA_SENT)?0:1;
}

ServerDrv serverDrv;
