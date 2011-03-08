#include "server_drv.h"

#include "WProgram.h"
#include "spi_drv.h"

extern "C" {
#include "wl_types.h"
#include "debug.h"
}


// Start server TCP on port specified
void ServerDrv::StartServer(uint16_t port, uint8_t sock)
{
	SpiDrv::spiSlaveSelect();
    // Send Command
    SpiDrv::sendCmd(START_SERVER_TCP_CMD, PARAM_NUMS_2);
    SpiDrv::sendParam(port);
    SpiDrv::sendParam(&sock, 1, LAST_PARAM);

    //Wait the reply elaboration
    delayMicroseconds(DELAY_RX_TX);

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!SpiDrv::waitResponse(START_SERVER_TCP_CMD, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
    }
    SpiDrv::spiSlaveDeselect();
}


uint8_t ServerDrv::getState(uint8_t sock)
{
    SpiDrv::spiSlaveSelect();
    // Send Command
    SpiDrv::sendCmd(GET_STATE_TCP_CMD, PARAM_NUMS_1);
    SpiDrv::sendParam(&sock, sizeof(sock), LAST_PARAM);

    //Wait the reply elaboration
    delayMicroseconds(DELAY_RX_TX);

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!SpiDrv::waitResponse(GET_STATE_TCP_CMD, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
    }
    SpiDrv::spiSlaveDeselect();

    delayMicroseconds(DELAY_POST_CMD);
    return _data;
}


uint8_t ServerDrv::availData(uint8_t sock)
{
    SpiDrv::spiSlaveSelect();
    // Send Command
    SpiDrv::sendCmd(AVAIL_DATA_TCP_CMD, PARAM_NUMS_1);
    SpiDrv::sendParam(&sock, sizeof(sock), LAST_PARAM);

    //Wait the reply elaboration
    delayMicroseconds(DELAY_RX_TX);

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!SpiDrv::waitResponse(AVAIL_DATA_TCP_CMD, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
    }
    SpiDrv::spiSlaveDeselect();

    delayMicroseconds(DELAY_POST_CMD);

    if (_dataLen!=0)
    {
        return (_data == 1);
    }
    return false;
}

bool ServerDrv::getData(uint8_t sock, uint8_t *data)
{
    SpiDrv::spiSlaveSelect();
    // Send Command
    SpiDrv::sendCmd(GET_DATA_TCP_CMD, PARAM_NUMS_1);
    SpiDrv::sendParam(&sock, sizeof(sock), LAST_PARAM);

    //Wait the reply elaboration
    delayMicroseconds(DELAY_RX_TX);

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!SpiDrv::waitResponse(GET_DATA_TCP_CMD, &_data, &_dataLen))
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
    SpiDrv::spiSlaveSelect();
    // Send Command
    SpiDrv::sendCmd(GET_DATABUF_TCP_CMD, PARAM_NUMS_1);
    SpiDrv::sendParam(&sock, sizeof(sock), LAST_PARAM);

    //Wait the reply elaboration
    delayMicroseconds(DELAY_RX_TX);

    // Wait for reply
    if (!SpiDrv::waitResponse(GET_DATABUF_TCP_CMD, _data, _dataLen))
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


bool ServerDrv::sendData(uint8_t sock, const uint8_t *data, uint16_t len)
{
    SpiDrv::spiSlaveSelect();
    // Send Command
    SpiDrv::sendCmd(SEND_DATA_TCP_CMD, PARAM_NUMS_2);
    SpiDrv::sendParam(&sock, sizeof(sock));
    SpiDrv::sendBuffer((uint8_t *)data, len, LAST_PARAM);

    //Wait the reply elaboration
    delayMicroseconds(DELAY_RX_TX);

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!SpiDrv::waitResponse(SEND_DATA_TCP_CMD, &_data, &_dataLen))
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


uint8_t ServerDrv::isDataSent(uint8_t sock)
{
	SpiDrv::spiSlaveSelect();
    // Send Command
    SpiDrv::sendCmd(DATA_SENT_TCP_CMD, PARAM_NUMS_1);
    SpiDrv::sendParam(&sock, sizeof(sock), LAST_PARAM);

    //Wait the reply elaboration
    delayMicroseconds(DELAY_RX_TX);

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!SpiDrv::waitResponse(DATA_SENT_TCP_CMD, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse isDataSent");
    }
    SpiDrv::spiSlaveDeselect();

    delayMicroseconds(DELAY_POST_CMD);
    return _data;
}

ServerDrv serverDrv;
